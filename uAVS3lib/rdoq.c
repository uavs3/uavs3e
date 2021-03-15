#include "defines.h"
#include "transform.h"
#include "rdoq.h"
#include "AEC.h"
#include <memory.h>

#define ENTROPY_BITS_TABLE_BiTS          10
#define ENTROPY_BITS_TABLE_BiTS_SHIFP   (PROB_BITS-ENTROPY_BITS_TABLE_BiTS)
#define ENTROPY_BITS_TABLE_SIZE         (1<<ENTROPY_BITS_TABLE_BiTS)

#define QUANT(c, scale, offset, shift) ((i16s_t)((((c)*(scale)) + (offset)) >> (shift)))
#define GET_I_COST(rate, lamba)  (rate*lamba)
#define GET_IEP_RATE             (32768)

// for RDOQ
#define SCALE_BITS               15   
#define MAX_CU_DEPTH 6

#define MAX_TR_DIM  (32 * 32)
#define MAX_TX_DYNAMIC_RANGE               15
#define QUANT_SHIFT                        14
#define COM_GET_TRANS_SHIFT(bit_depth, tr_size_log2) (MAX_TX_DYNAMIC_RANGE - (bit_depth) - (tr_size_log2))

static int tbl_rdoq_err_scale[80][MAX_CU_DEPTH]; // [64 + 16][MAX_CU_DEPTH]
static int tbl_rdoq_prob_2_bits[ENTROPY_BITS_TABLE_SIZE];

static i16u_t enc_tbl_quant_scale[80] = { // [64 + 16]
    16302, 15024, 13777, 12634, 11626, 10624,  9742,  8958,
     8192,  7512,  6889,  6305,  5793,  5303,  4878,  4467,
     4091,  3756,  3444,  3161,  2894,  2654,  2435,  2235,
     2048,  1878,  1722,  1579,  1449,  1329,  1218,  1117,
     1024,   939,   861,   790,   724,   664,   609,   558,
      512,   470,   430,   395,   362,   332,   304,   279,
      256,   235,   215,   197,   181,   166,   152,   140,
      128,   116,   108,    99,    91,    83,    76,    69,
       64,    59,    54,    49,    45,    41,    38,    35,
       32,    30,    27,    25,    23,    21,    19,    18
};

void rdoq_init_err_scale(int bit_depth)
{
    int qp;
    int i;
    for (qp = 0; qp < 80; qp++) {
        int q_value = enc_tbl_quant_scale[qp];
        for (i = 0; i < MAX_CU_DEPTH; i++) {
            int tr_shift = COM_GET_TRANS_SHIFT(bit_depth, i + 1);
            double err_scale = pow(2.0, SCALE_BITS - tr_shift);
            err_scale = err_scale / q_value / (1 << ((bit_depth - 8)));
            tbl_rdoq_err_scale[qp][i] = (int)(err_scale * (double)(1 << ERR_SCALE_PRECISION_BITS));
        }
    }
    return;
}

void rdoq_init_prob_2_bits()
{
    for (int i = 0; i < ENTROPY_BITS_TABLE_SIZE; i++) {
        double p = (MAX_PROB * (i + 0.5)) / ENTROPY_BITS_TABLE_SIZE;
        tbl_rdoq_prob_2_bits[i] = (int)(-32000 * (log(p) / log(2.0) - PROB_BITS));
    }
}

static void rdoq_get_sym_bits(int *pbits, bin_ctx_t *cm)
{
    i16u_t prob_lps = ((*cm) & PROB_MASK) >> 1;

    if ((*cm) & 1) {
        pbits[0] = tbl_rdoq_prob_2_bits[            prob_lps  >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
        pbits[1] = tbl_rdoq_prob_2_bits[(MAX_PROB - prob_lps) >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
    } else {
        pbits[1] = tbl_rdoq_prob_2_bits[            prob_lps  >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
        pbits[0] = tbl_rdoq_prob_2_bits[(MAX_PROB - prob_lps) >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
    }
}

static void rdoq_get_sym_bitsW(int *pbits, bin_ctx_t *cm1, bin_ctx_t *cm2)
{
    i16u_t prob_lps;
    i16u_t prob_lps1 = ((*cm1) & PROB_MASK) >> 1;
    i16u_t prob_lps2 = ((*cm2) & PROB_MASK) >> 1;
    i16u_t cmps;
    i16u_t cmps1 = (*cm1) & 1;
    i16u_t cmps2 = (*cm2) & 1;

    if (cmps1 == cmps2) {
        cmps = cmps1;
        prob_lps = (prob_lps1 + prob_lps2) >> 1;
    } else {
        if (prob_lps1 < prob_lps2) {
            cmps = cmps1;
            prob_lps = (256 << LG_PMPS_SHIFTNO) - 1 - ((prob_lps2 - prob_lps1) >> 1);
        } else {
            cmps = cmps2;
            prob_lps = (256 << LG_PMPS_SHIFTNO) - 1 - ((prob_lps1 - prob_lps2) >> 1);
        }
    }
    if (cmps) {
        pbits[0] = tbl_rdoq_prob_2_bits[prob_lps              >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
        pbits[1] = tbl_rdoq_prob_2_bits[(MAX_PROB - prob_lps) >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
    } else {
        pbits[1] = tbl_rdoq_prob_2_bits[prob_lps              >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
        pbits[0] = tbl_rdoq_prob_2_bits[(MAX_PROB - prob_lps) >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
    }
}

void rdoq_init_cu_est_bits(avs3_enc_t *core, aec_t *aec)
{
    rdoq_get_sym_bits(core->rdoq_bin_est_ctp, &aec->syn_ctx->ctp_zero_flag);

    for (int h = 0; h < LBAC_CTX_CBF; h++) {
        rdoq_get_sym_bits(core->rdoq_bin_est_cbf[h], aec->syn_ctx->cbf + h);
    }
    for (int h = 0; h < LBAC_CTX_RUN_RDOQ; h++) {
        rdoq_get_sym_bits(core->rdoq_bin_est_run[h], aec->syn_ctx->run_rdoq + h);
    }
    for (int h = 0; h < LBAC_CTX_LEVEL; h++) {
        rdoq_get_sym_bits(core->rdoq_bin_est_lvl[h], aec->syn_ctx->level + h);
    }
    for (int i = 0; i < LBAC_CTX_LAST1; i++) {
        for (int j = 0; j < LBAC_CTX_LAST2; j++) {
            rdoq_get_sym_bitsW(core->rdoq_bin_est_lst[0][i][j], aec->syn_ctx->last1 + i,                  aec->syn_ctx->last2 + j);
            rdoq_get_sym_bitsW(core->rdoq_bin_est_lst[1][i][j], aec->syn_ctx->last1 + i + LBAC_CTX_LAST1, aec->syn_ctx->last2 + j + LBAC_CTX_LAST2);
        }
    }
}

static avs3_always_inline i32u_t rdoq_one_coef(i64s_t rd64_uncoded_cost, i64s_t *rd64_coded_cost, i32u_t level_double, i32u_t abs_level,
                                       int *est_run, int(*est_level)[2], int q_bits, int err_scale, i64s_t lambda, int last_pos)
{
    i32u_t best_abs_level = 0;
    int checks = 2;

    *rd64_coded_cost = rd64_uncoded_cost + (i64s_t)GET_I_COST(est_run[0], lambda);

    while (abs_level && checks--) {
        int diff = level_double - (abs_level << q_bits);
        i64s_t err = (diff * (i64s_t)err_scale) >> ERR_SCALE_PRECISION_BITS;
        int rate = GET_IEP_RATE; // sign

        if (!last_pos) {
            rate += est_run[1];
        }
        if (abs_level == 1) {
            rate += est_level[0][1];
        } else if (abs_level < 9) {
            rate += est_level[0][0];
            rate += est_level[1][0] * (int)(abs_level - 2);
            rate += est_level[1][1];
        } else {
            rate += est_level[0][0];
            rate += est_level[1][0] * 7;
            rate += ((uavs3e_get_log2(abs_level - 9 + 1) << 1) + 1) * GET_IEP_RATE;
        }

        i64s_t dCurrCost = err * err + (i64s_t)GET_I_COST(rate, lambda);

        if (dCurrCost < *rd64_coded_cost) {
            best_abs_level = abs_level;
            *rd64_coded_cost = dCurrCost;
        }
        abs_level--;
    }
    return best_abs_level;
}


static int quant_check(i16s_t *coef, int num, int threshold)
{
    num >>= 2;
    while (num--) {
        int lev10 = coef[0];
        int lev11 = coef[1];
        int lev12 = coef[2];
        int lev13 = coef[3];

        if (lev10 > threshold || lev10 < -threshold ||
            lev11 > threshold || lev11 < -threshold ||
            lev12 > threshold || lev12 < -threshold ||
            lev13 > threshold || lev13 < -threshold) {
            return 0;
        }
        coef += 4;
    }
    return 1;
}

int rdoq_quant_block(avs3_enc_t *core, int slice_type, int qp, double d_lambda, int is_intra, 
    i16s_t *coef, int cu_width_log2, int cu_height_log2, int ch_type, int bit_depth)
{
    const int scale = enc_tbl_quant_scale[qp];
    const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
    const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
    const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
    const int tr_shift = COM_GET_TRANS_SHIFT(bit_depth, log2_size);
    const int q_bits = QUANT_SHIFT + tr_shift;
    const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
    const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
    const int max_num_coef = 1 << (cu_width_log2 + cu_height_log2);
    const int max_used_coef = 1 << (cu_width_log2 + COM_MIN(5, cu_height_log2));

    int offset = ((core->type & I_FRM) ? 201 : 153) << (q_bits - 9);
    int nz_threshold = ((((1 << q_bits) - offset) << 10) / q_value - 1) >> 10;

    if (quant_check(coef, max_used_coef, nz_threshold)) {
        return 0;
    }
    
    int num_nz_coef = 0;
    ALIGNED_64(i16s_t coef_1d[MAX_TR_DIM]);
    ALIGNED_64(i32u_t abs_level[MAX_TR_DIM]);
    ALIGNED_64(i32u_t abs_coef[MAX_TR_DIM]);
    ALIGNED_64(i64s_t zero_coef_err[MAX_TR_DIM]);
    int err_scale = tbl_rdoq_err_scale[qp][log2_size - 1];

    const i16u_t *scan = com_tbl_scan[cu_width_log2 - 1];

    for (int i = 0; i < max_num_coef; i += 8) {
        coef_1d[i + 0] = coef[scan[i + 0]];
        coef_1d[i + 1] = coef[scan[i + 1]];
        coef_1d[i + 2] = coef[scan[i + 2]];
        coef_1d[i + 3] = coef[scan[i + 3]];
        coef_1d[i + 4] = coef[scan[i + 4]];
        coef_1d[i + 5] = coef[scan[i + 5]];
        coef_1d[i + 6] = coef[scan[i + 6]];
        coef_1d[i + 7] = coef[scan[i + 7]];
    }

    int last_nz = g_funs_handle.pre_quant_rdoq(coef_1d, max_num_coef, q_value, q_bits, err_scale, abs_coef, abs_level, zero_coef_err);

    const i64s_t lambda = (i64s_t)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
    i64s_t cost_best_delta; 
    i64s_t cost_curr_delta;

    if (!is_intra && ch_type == 0) {
        cost_best_delta = GET_I_COST(core->rdoq_bin_est_ctp[1], lambda);
        cost_curr_delta = GET_I_COST(core->rdoq_bin_est_ctp[0], lambda);
    } else {    
        cost_best_delta = GET_I_COST(core->rdoq_bin_est_cbf[ch_type][0], lambda);
        cost_curr_delta = GET_I_COST(core->rdoq_bin_est_cbf[ch_type][1], lambda);
    }

    i32u_t last_b_zero     = 0;
    int last_checked_nz = 0;
    int tmp_nz_coef     = 0;
    i32u_t end_pos         = 0;

    int(*rdoq_bin_est_lst_base)[LBAC_CTX_LAST2][2] = core->rdoq_bin_est_lst[ch_type != 0];
    int(*rdoq_bin_est_lst)[2] = rdoq_bin_est_lst_base[5];
    int zero_coef_bins = 0;

    int ctx_run_level_base = (ch_type == 0 ? 0 : 12);
    int (*est_run  )[2] = core->rdoq_bin_est_run + 10 + ctx_run_level_base;
    int (*est_level)[2] = core->rdoq_bin_est_lvl + 10 + ctx_run_level_base;

    memset(coef, 0, sizeof(i16s_t) * max_num_coef);

    for (int i = 0; i <= last_nz; i++) {
        if (!abs_level[i]) {
            zero_coef_bins += est_run[last_b_zero][0];
            last_b_zero = 1;
            continue;
        } else if (zero_coef_bins) {
            cost_curr_delta += (i64s_t)GET_I_COST(zero_coef_bins, lambda);
            zero_coef_bins = 0;
        }

        i64s_t cost_curr_coef;
        int level = rdoq_one_coef(zero_coef_err[i], &cost_curr_coef, abs_coef[i], abs_level[i], est_run[last_b_zero], est_level, 
                                    q_bits, err_scale, lambda, i == max_num_coef - 1);

        cost_curr_delta -= zero_coef_err[i];
        cost_curr_delta += cost_curr_coef;

        if (level) {
            last_checked_nz = i;
            coef[scan[i]] = (i16s_t)(coef_1d[i] < 0 ? -level : level);

            /* ----- check for last flag ----- */
            i64s_t cost_flag0 = GET_I_COST(rdoq_bin_est_lst[uavs3e_get_log2(i + 1)][0], lambda);
            i64s_t cost_flag1 = GET_I_COST(rdoq_bin_est_lst[uavs3e_get_log2(i + 1)][1], lambda);
            i64s_t cost_curr_end = cost_curr_delta + cost_flag1;

            cost_curr_delta += cost_flag0;
            tmp_nz_coef++;

            if (cost_curr_end < cost_best_delta) {
                cost_best_delta = cost_curr_end;
                end_pos = i;
                num_nz_coef += tmp_nz_coef;
                tmp_nz_coef = 0;
            }
            last_b_zero = 0;

            int ctx_offset = ((COM_MIN(level - 1, 5)) << 1) + ctx_run_level_base;
            est_level = core->rdoq_bin_est_lvl + ctx_offset;
            est_run   = core->rdoq_bin_est_run + ctx_offset;
            rdoq_bin_est_lst = rdoq_bin_est_lst_base[COM_MIN(level - 1, 5)];
        } else {
            last_b_zero = 1;
        }
    }
    if (num_nz_coef) {
        /* ===== clean uncoded coeficients ===== */
        for (int i = end_pos + 1; i <= last_checked_nz; i++) {
            coef[scan[i]] = 0;
        }
    }
    return num_nz_coef;
}
