/**************************************************************************************
 * Copyright (C) 2018-2019 uavs3e project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Open-Intelligence Open Source License V1.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Open-Intelligence Open Source License V1.1 for more details.
 *
 * You should have received a copy of the Open-Intelligence Open Source License V1.1
 * along with this program; if not, you can download it on:
 * http://www.aitisa.org.cn/uploadfile/2018/0910/20180910031548314.pdf
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include "define.h"
#include <math.h>

#define ENTROPY_BITS_TABLE_BiTS          10
#define ENTROPY_BITS_TABLE_BiTS_SHIFP   (PROB_BITS-ENTROPY_BITS_TABLE_BiTS)
#define ENTROPY_BITS_TABLE_SIZE         (1<<ENTROPY_BITS_TABLE_BiTS)

#define QUANT(c, scale, offset, shift) ((s16)((((c)*(scale)) + (offset)) >> (shift)))
#define GET_I_COST(rate, lamba)  (rate*lamba)
#define GET_IEP_RATE             (32768)

// for RDOQ
#define SCALE_BITS               15   
#define ERR_SCALE_PRECISION_BITS 20

static s32 tbl_rdoq_err_scale[80][MAX_CU_DEPTH]; // [64 + 16][MAX_CU_DEPTH]
static s32 tbl_rdoq_prob_2_bits[ENTROPY_BITS_TABLE_SIZE];

static tab_u16 enc_tbl_quant_scale[80] = { // [64 + 16]
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
            tbl_rdoq_err_scale[qp][i] = (s32)(err_scale * (double)(1 << ERR_SCALE_PRECISION_BITS));
        }
    }
    return;
}

void rdoq_init_prob_2_bits()
{
    for (int i = 0; i < ENTROPY_BITS_TABLE_SIZE; i++) {
        double p = (MAX_PROB * (i + 0.5)) / ENTROPY_BITS_TABLE_SIZE;
        tbl_rdoq_prob_2_bits[i] = (s32)(-32000 * (log(p) / log(2.0) - PROB_BITS));
    }
}

static void rdoq_get_sym_bits(s32 *pbits, lbac_ctx_model_t *cm)
{
    u16 prob_lps = ((*cm) & PROB_MASK) >> 1;

    if ((*cm) & 1) {
        pbits[0] = tbl_rdoq_prob_2_bits[prob_lps              >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
        pbits[1] = tbl_rdoq_prob_2_bits[(MAX_PROB - prob_lps) >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
    } else {
        pbits[1] = tbl_rdoq_prob_2_bits[prob_lps              >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
        pbits[0] = tbl_rdoq_prob_2_bits[(MAX_PROB - prob_lps) >> ENTROPY_BITS_TABLE_BiTS_SHIFP];
    }
}

static void rdoq_get_sym_bitsW(s32 *pbits, lbac_ctx_model_t *cm1, lbac_ctx_model_t *cm2)
{
    u16 prob_lps;
    u16 prob_lps1 = ((*cm1) & PROB_MASK) >> 1;
    u16 prob_lps2 = ((*cm2) & PROB_MASK) >> 1;
    u16 cmps;
    u16 cmps1 = (*cm1) & 1;
    u16 cmps2 = (*cm2) & 1;

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

void rdoq_init_cu_est_bits(core_t *core, lbac_t *lbac)
{
    int h;
 
    rdoq_get_sym_bits(core->rdoq_bin_est_ctp, lbac->h.ctp_zero_flag);

    for (h = 0; h < LBAC_CTX_CBF; h++) {
        rdoq_get_sym_bits(core->rdoq_bin_est_cbf[h], lbac->h.cbf + h);
    }
    for (h = 0; h < LBAC_CTX_RUN_RDOQ; h++) {
        rdoq_get_sym_bits(core->rdoq_bin_est_run[h], lbac->h.run_rdoq + h);
    }
    for (h = 0; h < LBAC_CTX_LEVEL; h++) {
        rdoq_get_sym_bits(core->rdoq_bin_est_lvl[h], lbac->h.level + h);
    }
    for (h = 0; h < 2; h++) { // luma / chroma
        int i, j;
        int chroma_offset1 = h * LBAC_CTX_LAST1;
        int chroma_offset2 = h * LBAC_CTX_LAST2;

        for (i = 0; i < LBAC_CTX_LAST1; i++) {
            for (j = 0; j < LBAC_CTX_LAST2; j++) {
                rdoq_get_sym_bitsW(core->rdoq_bin_est_lst[h][i][j], lbac->h.last1 + i + chroma_offset1, lbac->h.last2 + j + chroma_offset2);
            }
        }
    }
}

static __inline u32 rdoq_one_coef(s64 rd64_uncoded_cost, s64 *rd64_coded_cost, u32 level_double, u32 abs_level,
                                       s32 *est_run, s32(*est_level)[2], s32 q_bits, s32 err_scale, s64 lambda, int last_pos)
{
    u32 best_abs_level = 0;
    int checks = 2;

    *rd64_coded_cost = rd64_uncoded_cost + (s64)GET_I_COST(est_run[0], lambda);

    while (abs_level && checks--) {
        s32 diff = level_double - (abs_level << q_bits);
        s64 err = (diff * (s64)err_scale) >> ERR_SCALE_PRECISION_BITS;
        s32 rate = GET_IEP_RATE; // sign

        if (!last_pos) {
            rate += est_run[1];
        }
        if (abs_level == 1) {
            rate += est_level[0][1];
        } else if (abs_level < 9) {
            rate += est_level[0][0];
            rate += est_level[1][0] * (s32)(abs_level - 2);
            rate += est_level[1][1];
        } else {
            rate += est_level[0][0];
            rate += est_level[1][0] * 7;
            rate += ((uavs3e_get_log2(abs_level - 9 + 1) << 1) + 1) * GET_IEP_RATE;
        }

        s64 dCurrCost = err * err + (s64)GET_I_COST(rate, lambda);

        if (dCurrCost < *rd64_coded_cost) {
            best_abs_level = abs_level;
            *rd64_coded_cost = dCurrCost;
        }
        abs_level--;
    }
    return best_abs_level;
}

static int rdoq_quant_block(core_t *core, int slice_type, int qp, double d_lambda, int is_intra, s16 *coef, int cu_width_log2, int cu_height_log2, int ch_type, int bit_depth)
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
    const int pre_check_scale_bits = 10;
    const int q_bits_scale = q_bits + pre_check_scale_bits;

#define FAST_RDOQ_INTRA_RND_OFST  201
#define FAST_RDOQ_INTER_RND_OFST  153 

    int offset = ((slice_type == SLICE_I) ? FAST_RDOQ_INTRA_RND_OFST : FAST_RDOQ_INTER_RND_OFST) << (q_bits_scale - 9);
    int zero_coeff_threshold = ((1 << q_bits_scale) - offset) / q_value;
    const int max_used_coef = 1 << (cu_width_log2 + COM_MIN(5, cu_height_log2));

    if (uavs3e_funs_handle.quant_check(coef, max_used_coef, pre_check_scale_bits, zero_coeff_threshold - 1)) {
        return 0;
    }
    
    int num_nz_coef = 0;
    s16 coef_1d[MAX_TR_DIM];
    s16 abs_level[MAX_TR_DIM];
    u32 abs_coef[MAX_TR_DIM];
    s64 zero_coef_err[MAX_TR_DIM];
    s32 err_scale = tbl_rdoq_err_scale[qp][log2_size - 1];
    const u16 *scan = com_tbl_scan[cu_width_log2 - 1][cu_height_log2 - 1];

    for (int i = 0; i < max_num_coef; i += 4) {
        coef_1d[i + 0] = coef[scan[i + 0]];
        coef_1d[i + 1] = coef[scan[i + 1]];
        coef_1d[i + 2] = coef[scan[i + 2]];
        coef_1d[i + 3] = coef[scan[i + 3]];
    }

    s64 allzero_cost = uavs3e_funs_handle.quant_rdoq(coef_1d, max_num_coef, q_value, q_bits, err_scale,
                                                        ERR_SCALE_PRECISION_BITS, abs_coef, abs_level, zero_coef_err);

    const s64 lambda = (s64)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
    s64 cost_best = allzero_cost;
    s64 cost_curr = allzero_cost;

    if (!is_intra && ch_type == Y_C) {
        cost_best += GET_I_COST(core->rdoq_bin_est_ctp[1], lambda);
        cost_curr += GET_I_COST(core->rdoq_bin_est_ctp[0], lambda);
    } else {
        cost_best += GET_I_COST(core->rdoq_bin_est_cbf[ch_type][0], lambda);
        cost_curr += GET_I_COST(core->rdoq_bin_est_cbf[ch_type][1], lambda);
    }

    u32 last_b_zero     = 0;
    int last_checked_nz = 0;
    int tmp_nz_coef     = 0;
    u32 last_coef_nz    = 0;

    s32(*rdoq_bin_est_lst_base)[LBAC_CTX_LAST2][2] = core->rdoq_bin_est_lst[ch_type != Y_C];
    s32(*rdoq_bin_est_lst)[2] = rdoq_bin_est_lst_base[5];
    int zero_coef_bins = 0;

    int ctx_run_level_base = (ch_type == Y_C ? 0 : 12);
    s32 (*est_run  )[2] = core->rdoq_bin_est_run + 10 + ctx_run_level_base;
    s32 (*est_level)[2] = core->rdoq_bin_est_lvl + 10 + ctx_run_level_base;

    memset(coef, 0, sizeof(s16) * max_num_coef);

    for (int i = 0; i < max_num_coef; i++) {
        if (!abs_level[i]) {
            zero_coef_bins += est_run[last_b_zero][0];
            last_b_zero = 1;
            continue;
        } else if (zero_coef_bins) {
            cost_curr += (s64)GET_I_COST(zero_coef_bins, lambda);
            zero_coef_bins = 0;
        }

        s64 cost_curr_coef;
        s32 level = rdoq_one_coef(zero_coef_err[i], &cost_curr_coef, abs_coef[i], abs_level[i], est_run[last_b_zero], est_level, 
                                    q_bits, err_scale, lambda, i == max_num_coef - 1);

        cost_curr -= zero_coef_err[i];
        cost_curr += cost_curr_coef;

        if (level) {
            last_checked_nz = i;
            coef[scan[i]] = (s16)(coef_1d[i] < 0 ? -level : level);

            /* ----- check for last flag ----- */
            s64 cost_flag0 = GET_I_COST(rdoq_bin_est_lst[uavs3e_get_log2(i + 1)][0], lambda);
            s64 cost_flag1 = GET_I_COST(rdoq_bin_est_lst[uavs3e_get_log2(i + 1)][1], lambda);
            s64 cost_curr_end = cost_curr + cost_flag1;

            cost_curr += cost_flag0;
            tmp_nz_coef++;

            if (cost_curr_end < cost_best) {
                cost_best = cost_curr_end;
                last_coef_nz = i;
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
        for (int i = last_coef_nz + 1; i <= last_checked_nz; i++) {
            coef[scan[i]] = 0;
        }
    }
    return num_nz_coef;
}

int quant_non_zero(core_t *core, int qp, double lambda, int is_intra, s16 *coef, int cu_width_log2, int cu_height_log2, int ch_type, int slice_type)
{
    com_info_t *info = core->info;
    int bit_depth = info->bit_depth_internal;
    int num_nz_coef = 0;
    int scale  = enc_tbl_quant_scale[qp];
    int width = 1 << cu_width_log2;
    int height = 1 << cu_height_log2;

    if (width > 32) {
        s16 *p = coef + 32;
        for (int i = 0; i < height; i++) {
            memset(p, 0, sizeof(s16) * 32);
            p += width;
        }
    }

    if (!core->pichdr->pic_wq_enable) {
        num_nz_coef = rdoq_quant_block(core, slice_type, qp, lambda, is_intra, coef, cu_width_log2, cu_height_log2, ch_type, bit_depth);
    } else {
        s64 offset;
        int i, j;
        int w = 1 << cu_width_log2;
        int h = 1 << cu_height_log2;
        int shift;
        int tr_shift;
        int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
        int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
        int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
        int wq_width;
        int idx_shift;
        int idx_step;
        u8 *wq;

        if (height > 32) {
            memset(coef + 32 * width, 0, sizeof(s16) * width * height - 32 * width);
        }

        tr_shift = COM_GET_TRANS_SHIFT(bit_depth, log2_size - ns_shift);
        shift = QUANT_SHIFT + tr_shift;
        offset = (s64)((slice_type == SLICE_I) ? 171 : 85) << (s64)(shift - 9);

        if (cu_width_log2 == 2 && cu_height_log2 == 2) {
            wq = core->wq[0];
            idx_shift = 0;
            idx_step = 1;
            wq_width = 4;
        } else {
            wq = core->wq[1];
            idx_shift = COM_MAX(cu_width_log2, cu_height_log2) - 3;
            idx_step = 1 << idx_shift;
            wq_width = 8;
        }
        for (i = 0; i < h; i++) {
            for (j = 0; j < w; j++) {
                int weight = wq[j >> idx_shift];
                int sign = COM_SIGN_GET(coef[j]);
                int lev = (s16)(((s64)COM_ABS(coef[j]) * (s64)scale * ns_scale * 64 / weight + offset) >> shift);
                coef[j] = (s16)COM_SIGN_SET(lev, sign);
                num_nz_coef += !!(coef[j]);
            }
            coef += w;

            if ((i + 1) % idx_step == 0) {
                wq += wq_width;
            }
        }
    }
    return num_nz_coef;
}
