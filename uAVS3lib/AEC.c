#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include "AEC.h"
#include "image.h"
#include "vlc.h"

void copy_coding_state(aec_t *dst, aec_t *src)
{
    memcpy(dst, src, (uchar_t*)&src->syn_ctx - (uchar_t*)src);
    memcpy(dst->syn_ctx, src->syn_ctx, sizeof(bin_ctx_sets_t));
}

void copy_coding_state_hdr(aec_t *dst, aec_t *src)
{
    dst->range            = src->range;
    dst->Elow             = src->Elow;
    dst->Ebits_cnt        = src->Ebits_cnt;
    dst->engin_flag       = src->engin_flag;
    dst->enc_symbol       = src->enc_symbol;
    dst->enc_symbolW      = src->enc_symbolW;
    dst->enc_symbol_eq    = src->enc_symbol_eq;
    dst->enc_symbol_final = src->enc_symbol_final;
    dst->syn_ctx          = src->syn_ctx;
}

void write_terminating_bit(aec_t *aec, uchar_t bit)
{
    aec->enc_symbol_final(aec, bit);
}

void unary_bin_max_encode(aec_t *aec, unsigned int symbol, bin_ctx_t * ctx, int ctx_offset, unsigned int max_symbol)
{
    unsigned int l;
    bin_ctx_t * ictx;
    

    if (symbol == 0) {
        aec->enc_symbol(aec, 1, ctx);
    } else {
        aec->enc_symbol(aec, 0, ctx);
        l = symbol;
        ictx = ctx + ctx_offset;

        while ((--l) > 0) {
            aec->enc_symbol(aec, 0, ictx);
        }

        if (symbol < max_symbol) {
            aec->enc_symbol(aec, 1, ictx);
        }
    }
}

void writeCuTypeInfo(avs3_enc_t *h, cu_t *cu, aec_t *aec)
{
    const cfg_param_t *input = h->input;
    int i_scu = input->pic_width_in_mcu;
    int scup = (h->cu_pix_y >> 3) * i_scu + (h->cu_pix_x >> 3);
    cu_t *cu_left = &h->cu_data[scup - 1];
    cu_t *cu_up = &h->cu_data[scup - i_scu];
    int left_is_skip = h->cu_pix_x && (cu_left->cuType == 0 && cu_left->cbp == 0);
    int up_is_skip   = h->cu_pix_y && (cu_up->cuType   == 0 && cu_up->cbp   == 0);

    bin_ctx_sets_t *ctx = aec->syn_ctx;

    int skip_flag = (cu->cuType == 0 && cu->cbp == 0);
    int direct_flag = (cu->cuType == 0 && cu->cbp);

    aec->enc_symbol(aec, skip_flag, ctx->skip_flag + left_is_skip + up_is_skip);

    if (!skip_flag) {
        aec->enc_symbol(aec, direct_flag, ctx->direct_flag);
    }
 
    if (skip_flag || direct_flag) {
        // skip_idx
        if (h->type != B_FRM) {
            aec->enc_symbol(aec, 1, ctx->skip_idx);
        } else {
            int max_skip_num = 4 + 1; //1:num_hmvp_cands
            bin_ctx_t * pCTX = aec->syn_ctx->skip_idx;
            int symbol = cu->mdirect_mode;

            while (symbol-- > 0) {
                aec->enc_symbol(aec, 0, pCTX++);
            }
            if (symbol < max_skip_num) {
                aec->enc_symbol(aec, 1, pCTX);
            }
        }
    } else {
        int ctx_inc = (h->cu_pix_x && (cu_left->cuType == I16MB)) + 
                      (h->cu_pix_y && (cu_up->cuType   == I16MB));
        if (ctx_inc == 0) {
            ctx_inc = h->cu_bitsize > 4 ? 0 : (h->cu_bitsize > 3 ? 3 : 4);
        }
        aec->enc_symbol(aec, cu->cuType == I16MB, ctx->intra_flag + ctx_inc);
    }
}

void writeIntraPredMode(aec_t *aec, int val)
{
    
    bin_ctx_t * pCTX;
    int symbol = val;
    pCTX = aec->syn_ctx->l_intra_mode_contexts;

    if (symbol >= 0) {
        aec->enc_symbol(aec,                    0, pCTX    );
        aec->enc_symbol(aec, (symbol & 0x10) >> 4, pCTX + 1);
        aec->enc_symbol(aec, (symbol & 0x08) >> 3, pCTX + 2);
        aec->enc_symbol(aec, (symbol & 0x04) >> 2, pCTX + 3);
        aec->enc_symbol(aec, (symbol & 0x02) >> 1, pCTX + 4);
        aec->enc_symbol(aec, (symbol & 0x01)     , pCTX + 5);
    } else {
        aec->enc_symbol(aec,          1, pCTX    );
        aec->enc_symbol(aec, symbol + 2, pCTX + 6);
    }
}
 
void writeRefFrame(avs3_enc_t *h, aec_t *aec, int val)
{
    
    bin_ctx_sets_t  *ctx = aec->syn_ctx;
    int   act_sym = val;

    if (act_sym == 0) {
        aec->enc_symbol(aec, 1, ctx->ref_no_contexts);  
    } else {
        aec->enc_symbol(aec, 0, ctx->ref_no_contexts);

        if (h->type != B_FRM) {
            bin_ctx_t * pCTX = ctx->ref_no_contexts + 4;
            if (--act_sym >= 1) {
                aec->enc_symbol(aec, 0, pCTX++);
            }
            while (--act_sym >= 1) {
                aec->enc_symbol(aec, 0, pCTX);
            }
            if (val < h->refs_num - 1) {
                aec->enc_symbol(aec, 1, pCTX);
            }
        }
    }
}

void writeMVD_AEC(aec_t *aec, int val, int xy)
{
    int act_sym = absm(val);
    int exp_golomb_order = 0;
    
    bin_ctx_sets_t  *ctx = aec->syn_ctx;

    if (act_sym < 3) { // 0, 1, 2
        if (act_sym == 0) {
            aec->enc_symbol(aec, 0, &ctx->mvd_contexts[xy][0]);
        } else if (act_sym == 1) {
            aec->enc_symbol(aec, 1, &ctx->mvd_contexts[xy][0]);
            aec->enc_symbol(aec, 0, &ctx->mvd_contexts[xy][1]);
        } else if (act_sym == 2) {
            aec->enc_symbol(aec, 1, &ctx->mvd_contexts[xy][0]);
            aec->enc_symbol(aec, 1, &ctx->mvd_contexts[xy][1]);
            aec->enc_symbol(aec, 0, &ctx->mvd_contexts[xy][2]);
        }
    } else {
        int offset = 1 - (act_sym & 1);
        aec->enc_symbol(aec, 1, &ctx->mvd_contexts[xy][0]);
        aec->enc_symbol(aec, 1, &ctx->mvd_contexts[xy][1]);
        aec->enc_symbol(aec, 1, &ctx->mvd_contexts[xy][2]);

        aec->enc_symbol_eq(aec, offset);
        act_sym = (act_sym - 3 - offset) >> 1;

        // exp_golomb part
        while (act_sym >= (1 << exp_golomb_order)) {
            aec->enc_symbol_eq(aec, 0);
            act_sym = act_sym - (1 << exp_golomb_order);
            exp_golomb_order++;
        }

        aec->enc_symbol_eq(aec, 1);

        while (exp_golomb_order--) { //next binary part
            aec->enc_symbol_eq(aec, (act_sym >> exp_golomb_order) & 1);
        }
    }

    if (val) {
        aec->enc_symbol_eq(aec, val >> 31);
    }
}

void writePdir(cu_t *cu, aec_t *aec, int pdir)
{
    bin_ctx_sets_t *ctx = aec->syn_ctx;
    
    aec->enc_symbol(aec, pdir >> 1, ctx->pdir_contexts);

    if ((pdir >> 1) == 0) {
        aec->enc_symbol(aec, pdir == BACKWARD, ctx->pdir_contexts + 1); // forward or backward ?
    }
}

void writeCIPredMode(avs3_enc_t *h, aec_t *aec, int val, int luma_mode)
{
    bin_ctx_t *pCTX = aec->syn_ctx->c_intra_mode_contexts;
    int act_sym  = val;
    
    const cfg_param_t *input = h->input;
    int b8y = h->cu_pix_y >> 3;
    int b8x = h->cu_pix_x >> 3;

    if (act_sym == 0) {
        aec->enc_symbol(aec, 1, pCTX);
    } else {
        int lmode = 0;
        switch (luma_mode) {
        case VERT_PRED:
            lmode = VERT_PRED_C;
            break;
        case HOR_PRED:
            lmode = HOR_PRED_C;
            break;
        case DC_PRED:
            lmode = DC_PRED_C;
            break;
        case BI_PRED:
            lmode = BI_PRED_C;
            break;
        default:
            break;
        }
        aec->enc_symbol(aec, 0, pCTX);
     
        if (lmode && act_sym > lmode) {
            act_sym = act_sym - 2;
        } else {
            act_sym = act_sym - 1;
        }
        unary_bin_max_encode(aec, (unsigned int)act_sym, pCTX + 1, 0, 3);
    }
}

void writeCBP(avs3_enc_t *h, cu_t *cu, aec_t *aec, int val)
{
    bin_ctx_sets_t *ctx = aec->syn_ctx;
    
    int cbp = val; 

    if (!IS_INTRA(cu)) {
        if (cu->cuType) {
            aec->enc_symbol(aec, !cbp, &ctx->ctp_zero_flag);
        }
        if (cbp) {
            aec->enc_symbol(aec, cu->trans_size, ctx->tu_contexts);

            aec->enc_symbol(aec, (cbp >> 4) & 1, &ctx->cbf[1]);
            aec->enc_symbol(aec, (cbp >> 5) & 1, &ctx->cbf[2]);

            if ((cbp >> 4) == 0 && cu->trans_size == 0) {
                return;
            }
            if (cu->trans_size == 0) {
                aec->enc_symbol(aec, cbp & 1, ctx->cbf);
            } else {
                aec->enc_symbol(aec, (cbp     ) & 1, ctx->cbf);
                aec->enc_symbol(aec, (cbp >> 1) & 1, ctx->cbf);
                aec->enc_symbol(aec, (cbp >> 2) & 1, ctx->cbf);
                aec->enc_symbol(aec, (cbp >> 3) & 1, ctx->cbf);
            }
        }
    } else {
        aec->enc_symbol(aec,  cbp & 1   , &ctx->cbf[0]);
        aec->enc_symbol(aec, (cbp >> 4) & 1, &ctx->cbf[1]);
        aec->enc_symbol(aec, (cbp >> 5) & 1, &ctx->cbf[2]);
    }
}

int write_sao_mergeflag_AEC(aec_t *aec, int val, int act_ctx)
{
    
    int bak_len = arienco_bits_written(aec);
    bin_ctx_sets_t  *ctx = aec->syn_ctx;
    int  act_sym = val;

    if (act_ctx == 1) {
        assert(act_sym <= 1);
        aec->enc_symbol(aec, act_sym, &ctx->saomergeflag_context[0]);
    } else if (act_ctx == 2) {
        assert(act_sym <= 2);
        aec->enc_symbol(aec, act_sym & 0x01, &ctx->saomergeflag_context[1]);
        if (act_sym != 1) {
            aec->enc_symbol(aec, (act_sym >> 1) & 0x01, &ctx->saomergeflag_context[2]);
        }
    }
    return arienco_bits_written(aec) - bak_len;
}

int write_sao_mergeflag(aec_t *aec, int mergeleft_avail, int mergeup_avail, SAOBlkParam *saoBlkParam)
{
    int MergeLeft = 0;
    int MergeUp = 0;
    int flag_val = 0;

    if (mergeleft_avail) {
        MergeLeft = (saoBlkParam->mergeIdc == SAO_MERGE_MODE_LEFT);
        flag_val = MergeLeft ? 1 : 0;

    }
    if (mergeup_avail && !MergeLeft) {
        MergeUp = (saoBlkParam->mergeIdc == SAO_MERGE_MODE_ABOVE);
        flag_val = MergeUp ? (1 + mergeleft_avail) : 0;
    }

    return write_sao_mergeflag_AEC(aec, flag_val, mergeleft_avail + mergeup_avail);
}

int write_sao_mode_AEC(aec_t *aec, int val)
{
    
    int bak_len = arienco_bits_written(aec);
    bin_ctx_sets_t  *ctx = aec->syn_ctx;
    int  act_sym = val;

    if (act_sym == 0) {
        aec->enc_symbol(aec, 1, ctx->saomode_context);
    } else {
        aec->enc_symbol(aec, 0, ctx->saomode_context);
        aec->enc_symbol_eq(aec, !(act_sym & 2));
    }
    return arienco_bits_written(aec) - bak_len;
}

int write_sao_mode(aec_t *aec, SAOBlkParam *saoBlkParam)
{
    int sao_mode;

    if (saoBlkParam->typeIdc == -1) {
        sao_mode = 0;
    } else {
        sao_mode = 3;
    }

    return write_sao_mode_AEC(aec, sao_mode);
}

int write_sao_offset_AEC(aec_t *aec, int val, int offset_type)
{
    
    int bak_len = arienco_bits_written(aec);
    bin_ctx_sets_t  *ctx = aec->syn_ctx;
    int  act_sym;
    int signsymbol = (val >> 31);
    int temp, maxvalue;

    assert(offset_type != SAO_CLASS_EO_PLAIN);
    if (offset_type == SAO_CLASS_EO_FULL_VALLEY) {
        act_sym = tab_eo_offset_map[val + 1];
    } else if (offset_type == SAO_CLASS_EO_FULL_PEAK) {
        act_sym = tab_eo_offset_map[-val + 1];
    } else {
        act_sym = abs(val);
    }
    maxvalue = tab_saoclip[offset_type][2];
    temp = act_sym;

    if (temp == 0) {
        if (offset_type == SAO_CLASS_BO) {
            aec->enc_symbol(aec, 1, &ctx->saooffset_context[0]);
        } else {
            aec->enc_symbol_eq(aec,  1);
        }
    } else {
        while (temp != 0) {
            if (offset_type == SAO_CLASS_BO && temp == act_sym) {
                aec->enc_symbol(aec, 0, &ctx->saooffset_context[0]);
            } else {
                aec->enc_symbol_eq(aec, 0);
            }

            temp--;
        }
        if (act_sym < maxvalue) {
            aec->enc_symbol_eq(aec, 1);
        }
    }

    if (offset_type == SAO_CLASS_BO && act_sym) {
        aec->enc_symbol_eq(aec, signsymbol);
    }
    return arienco_bits_written(aec) - bak_len;
}

int write_sao_offset(aec_t *aec, SAOBlkParam *saoBlkParam)
{
    int rate = 0;
    int bits;
    int i;

    assert(saoBlkParam->mergeIdc == SAO_MODE_NEW);
    assert(saoBlkParam->typeIdc >= SAO_TYPE_EO_0 && saoBlkParam->typeIdc <= SAO_TYPE_EO_45);

    for (i = SAO_CLASS_EO_FULL_VALLEY; i < NUM_SAO_EO_CLASSES; i++) {
        if (i == SAO_CLASS_EO_PLAIN) {
            continue;
        }
        bits = write_sao_offset_AEC(aec, saoBlkParam->offset[i], i);
        rate += bits;
    }

    return rate;
}

int write_sao_type_AEC(aec_t *aec, int val)
{
    int bak_len = arienco_bits_written(aec);
    int i;
    int exp_golomb_order = 1;
  
    for (i = 0; i < NUM_SAO_EO_TYPES_LOG2; i++) {
        aec->enc_symbol_eq(aec, val & 1);
        val = val >> 1;
    }
  
    return arienco_bits_written(aec) - bak_len;
}

int write_sao_type(aec_t *aec, SAOBlkParam *saoBlkParam)
{
    int rate = 0;
    int bits;

    assert(saoBlkParam->mergeIdc == SAO_MODE_NEW);
    assert(saoBlkParam->typeIdc >= SAO_TYPE_EO_0 && saoBlkParam->typeIdc <= SAO_TYPE_EO_45);

    bits = write_sao_type_AEC(aec, saoBlkParam->typeIdc);
    rate += bits;

    return rate;
}

void writeAlfCoeff(bit_stream_t *strm, ALFParam *Alfp)
{
    int pos, i;
    int groupIdx[NO_VAR_BINS];

    int f = 0;
    const int numCoeff = (int)ALF_MAX_NUM_COEF;
    unsigned int noFilters;

    switch (Alfp->componentID) {
    case ALF_Cb:
    case ALF_Cr: {
        for (pos = 0; pos < numCoeff; pos++) {
            SE_V("Chroma ALF coefficients", Alfp->coeffmulti[0][pos], strm);
        }
    }
    break;
    case ALF_Y: {
        noFilters = Alfp->filters_per_group - 1;

        UE_V("ALF filter number", noFilters, strm);

        groupIdx[0] = 0;
        f++;
        if (Alfp->filters_per_group > 1) {
            for (i = 1; i < NO_VAR_BINS; i++) {
                if (Alfp->filterPattern[i] == 1) {
                    groupIdx[f] = i;
                    f++;
                }
            }
        }

        for (f = 0; f < Alfp->filters_per_group; f++) {
            if (f > 0) {
                if (Alfp->filters_per_group != 16) {
                    UE_V("Region distance", (unsigned int)(groupIdx[f] - groupIdx[f - 1]), strm);
                } else {
                    assert(groupIdx[f] - groupIdx[f - 1] == 1);
                }
            }

            for (pos = 0; pos < numCoeff; pos++) {
                SE_V("Luma ALF coefficients", Alfp->coeffmulti[f][pos], strm);
            }
        }
    }
    break;
    default: {
        printf("Not a legal component ID\n");
        assert(0);
        exit(-1);
    }
    }
}

static void lbac_write_truncate_unary_sym(int sym, int num_ctx, int max_num, aec_t *aec, bin_ctx_t *model_base)
{
    int ctx_idx = 0;

    do {
        aec->enc_symbol(aec, sym ? 0 : 1, model_base + COM_MIN(ctx_idx, num_ctx - 1));
    } while (++ctx_idx < max_num - 1 && sym--);
}

static void lbac_write_unary_sym_ep(int sym, aec_t *aec)
{
    do {
        aec->enc_symbol_eq(aec, sym ? 0 : 1);
    } while (sym--);
}

static void lbac_encode_bins_ep_msb(int value, int num_bin, aec_t *aec)
{
    for (int bin = num_bin - 1; bin >= 0; bin--) {
        aec->enc_symbol_eq(aec, (value >> bin) & 1);
    }
}

static void lbac_enc_run(int sym, aec_t *aec, bin_ctx_t *model)
{
    int exp_golomb_order = 0;

    if (sym < 16) {
        lbac_write_truncate_unary_sym(sym, 2, 17, aec, model);
    }
    else {
        sym -= 16;
        lbac_write_truncate_unary_sym(16, 2, 17, aec, model);

        while ((int)sym >= (1 << exp_golomb_order)) {
            sym = sym - (1 << exp_golomb_order);
            exp_golomb_order++;
        }

        lbac_write_unary_sym_ep(exp_golomb_order, aec);
        lbac_encode_bins_ep_msb(sym, exp_golomb_order, aec);
    }
}

static void lbac_enc_run_for_rdoq(int sym, int num_ctx, bin_ctx_t *model)
{
    int ctx_idx = 0;

    do {
        lbac_encode_bin_for_rdoq(sym ? 0 : 1, model + COM_MIN(ctx_idx, num_ctx - 1));
        ctx_idx++;
    } while (sym--);
}

static void lbac_enc_level(int sym, aec_t *aec, bin_ctx_t *model)
{
    int exp_golomb_order = 0;

    if (sym < 8) {
        lbac_write_truncate_unary_sym(sym, 2, 9, aec, model);
    } else {
        sym -= 8;
        lbac_write_truncate_unary_sym(8, 2, 9, aec, model);

        while ((int)sym >= (1 << exp_golomb_order)) {
            sym = sym - (1 << exp_golomb_order);
            exp_golomb_order++;
        }
        lbac_write_unary_sym_ep(exp_golomb_order, aec);
        lbac_encode_bins_ep_msb(sym, exp_golomb_order, aec);
    }
}

static void lbac_write_truncate_unary_sym_rdo(int sym, int num_ctx, int max_num, aec_t *aec, bin_ctx_t *model_base)
{
    int ctx_idx = 0;
    int range = aec->range;
    int bits = 0;

    do {
        bin_ctx_t* bi_ct = model_base + COM_MIN(ctx_idx, num_ctx - 1);
        i32u_t lg_pmps = (*bi_ct) & MASK_LGPMPS_MPS;
        const i32u_t lg_pmps_shifted = lg_pmps >> LG_PMPS_SHIFTNO_PLUS1;
        i32s_t rMPS = range - lg_pmps_shifted;
        int s = rMPS < QUARTER;

        if ((!sym) == (lg_pmps & 1)) { //MPS happens
            bits += s;
            range = rMPS | 0x100;
        } else { //--LPS
            i32s_t rLPS = (range << s) - (rMPS | 0x100);
            int shift = aec_get_shift(rLPS);
            range = rLPS << shift;
            bits += s + shift;
        }

    } while (++ctx_idx < max_num - 1 && sym--);

    aec->range = range;
    aec->Ebits_cnt += bits;
}

void write_run_length_cc(aec_t *aec, i16s_t *coef, int uiBitSize, int bluma) 
{
    bin_ctx_sets_t *lbac_ctx = aec->syn_ctx;
    const i16u_t *scanp = com_tbl_scan[uiBitSize - 1];
    bin_ctx_t *ctx_last1    = lbac_ctx->last1    + (bluma ? 0 : LBAC_CTX_LAST1);
    bin_ctx_t *ctx_last2    = lbac_ctx->last2    + (bluma ? 0 : LBAC_CTX_LAST2);
    bin_ctx_t *ctx_run      = lbac_ctx->run      + (bluma ? 0 : 12);
    bin_ctx_t *ctx_run_rdoq = lbac_ctx->run_rdoq + (bluma ? 0 : 12);
    bin_ctx_t *ctx_level    = lbac_ctx->level    + (bluma ? 0 : 12);
    int num_coeff = 1 << (uiBitSize * 2);
    int run = 0;
    int prev_level = 6;
    int num_sig = g_funs_handle.get_nz_num(coef, num_coeff);

    for (int scan_pos = 0; scan_pos < num_coeff; scan_pos++) {
        int coef_cur = coef[scanp[scan_pos]];

        if (coef_cur) {
            int level = COM_ABS(coef_cur);
            int t0 = (COM_MIN(prev_level - 1, 5)) << 1;

            /* Run coding */
            lbac_enc_run(run, aec, &ctx_run[t0]);

            if (aec->engin_flag < 2) {
                lbac_enc_run_for_rdoq(run, 2, &ctx_run_rdoq[t0]);
            }
            /* Level coding */
            lbac_enc_level(level - 1, aec, &ctx_level[t0]);

            /* Sign coding */
            aec->enc_symbol_eq(aec, coef_cur <= 0);
            if (scan_pos == num_coeff - 1) {
                assert(num_sig == 1);
                break;
            }
            run = 0;
            num_sig--;
            /* Last flag coding */
            int last_flag = (num_sig == 0) ? 1 : 0;
            aec->enc_symbolW(aec, num_sig == 0, &ctx_last1[t0 >> 1], &ctx_last2[uavs3e_get_log2(scan_pos + 1)]);
            prev_level = level;

            if (last_flag) {
                break;
            }
        } else {
            run++;
        }
    }
}

void com_check_split_mode(int *split_allow, int cu_width_log2, int cu_height_log2, int boundary, int boundary_b, int boundary_r, int qt_depth, int bet_depth)
{
    //constraints parameters
    const int min_cu_size      =  4; //sqh->min_cu_size;
    const int min_bt_size      =  4; //sqh->min_cu_size;
    const int min_eqt_size     =  4; //sqh->min_cu_size;
    const int max_split_depth  =  6; //sqh->max_split_times;
    const int max_aspect_ratio =  8; //sqh->max_part_ratio;
    const int min_qt_size      =  8; //sqh->min_qt_size;
    const int max_bt_size      = 64; //sqh->max_bt_size;
    const int max_eqt_size     =  8; //sqh->max_eqt_size;
    int max_aspect_ratio_eqt = max_aspect_ratio >> 1;

    int cu_w = 1 << cu_width_log2;
    int cu_h = 1 << cu_height_log2;
    int i;

    for (i = NO_SPLIT; i <= SPLIT_QUAD; i++) {
        split_allow[i] = 0;
    }

    if (boundary) {
        if (!boundary_r && !boundary_b) {
            split_allow[SPLIT_QUAD] = 1;
        } else if (boundary_r) {
            split_allow[SPLIT_BI_VER] = 1;
        } else if (boundary_b) {
            split_allow[SPLIT_BI_HOR] = 1;
        }
        assert(qt_depth + bet_depth < max_split_depth);
    } else {
        //max qt-bt depth constraint
        if (qt_depth + bet_depth >= max_split_depth) {
            split_allow[NO_SPLIT] = 1; //no further split allowed
        } else {
            //not split
            if (cu_w <= cu_h * max_aspect_ratio && cu_h <= cu_w * max_aspect_ratio) {
                split_allow[NO_SPLIT] = 1;
            }
            //qt
            if (cu_w > min_qt_size && bet_depth == 0) {
                split_allow[SPLIT_QUAD] = 1;
            }
            //hbt
            if (((cu_w <= max_bt_size && cu_h <= max_bt_size) && cu_h > min_bt_size && cu_w < cu_h * max_aspect_ratio)) {
                split_allow[SPLIT_BI_HOR] = 1;
            }
            //vbt
            if (((cu_w <= max_bt_size && cu_h <= max_bt_size) && cu_w > min_bt_size && cu_h < cu_w * max_aspect_ratio)) {
                split_allow[SPLIT_BI_VER] = 1;
            }
            //heqt
            if ((cu_w <= max_eqt_size && cu_h <= max_eqt_size) && cu_h > min_eqt_size * 2 && cu_w > min_eqt_size && cu_w < cu_h * max_aspect_ratio_eqt) {
                split_allow[SPLIT_EQT_HOR] = 1;
            }
            //veqt
            if ((cu_w <= max_eqt_size && cu_h <= max_eqt_size) && cu_w > min_eqt_size * 2 && cu_h > min_eqt_size && cu_h < cu_w * max_aspect_ratio_eqt) {
                split_allow[SPLIT_EQT_VER] = 1;
            }
        }
    }

    int num_allowed = 0;
    for (i = NO_SPLIT; i <= SPLIT_QUAD; i++) {
        num_allowed += split_allow[i] == 1;
    }
    assert(num_allowed);
}

void lbac_enc_split_flag(avs3_enc_t *h, aec_t *aec, int cu_width, int cu_height, int x, int y, int flag)
{
    int idx = 0;
    int i_scu = h->input->b8_stride;
    int scup = (y >> 3) * i_scu + (x >> 3);

    if (y > 0) {
        idx += h->size[scup - i_scu] < cu_width;
    }
    if (x > 0) {
        idx += h->size[scup - 1] < cu_height;
    }

    aec->enc_symbol(aec, flag, aec->syn_ctx->split_flag + idx);
}

int lbac_enc_split_mode(avs3_enc_t *h, aec_t *aec, int split_mode, int cu_width, int cu_height, int qt_depth, int bet_depth, int x, int y)
{
    int ret = 0;
    int split_allow[NUM_SPLIT_MODE];
    int boundary = 0, boundary_b = 0, boundary_r = 0;
    const cfg_param_t *input = h->input;
    
    boundary = !(x + cu_width <= input->img_width && y + cu_height <= input->img_height);
    boundary_b = boundary && (y + cu_height > input->img_height) && !(x + cu_width > input->img_width);
    boundary_r = boundary && (x + cu_width > input->img_width) && !(y + cu_height > input->img_height);
    
    com_check_split_mode(split_allow, tab_log2[cu_width], tab_log2[cu_height], boundary, boundary_b, boundary_r, qt_depth, bet_depth);

    int non_QT_split_mode_num = 0;
    for (int i = 1; i < SPLIT_QUAD; i++) {
        non_QT_split_mode_num += split_allow[i];
    }
    if (split_allow[SPLIT_QUAD] && !(non_QT_split_mode_num || split_allow[NO_SPLIT])) { //only QT is allowed
        assert(split_mode == SPLIT_QUAD);
        return ret;
    } else if (split_allow[SPLIT_QUAD]) {
        lbac_enc_split_flag(h, aec, cu_width, cu_height, x, y, split_mode == SPLIT_QUAD);
        if (split_mode == SPLIT_QUAD) {
            return ret;
        }
    }
    if (non_QT_split_mode_num) {
        int cu_width_log2 = tab_log2[cu_width];
        int cu_height_log2 = tab_log2[cu_height];
        int x_scu = x >> 3;
        int y_scu = y >> 3;
        int i_scu = h->input->b8_stride;
        int scup = x_scu + y_scu * i_scu;
        int idx = 0;

        if (y_scu > 0) {
            idx += h->size[scup - i_scu] < cu_width;
        }
        if (x_scu > 0) {
            idx += h->size[scup - 1] < cu_height;
        }

        int sample = cu_width * cu_height;
        int ctx_set = (sample > 1024) ? 0 : (sample > 256 ? 1 : 2);

        if (split_allow[NO_SPLIT]) {
            aec->enc_symbol(aec, split_mode != NO_SPLIT, aec->syn_ctx->bt_split_flag + idx + ctx_set * 3);
        } else {
            assert(split_mode != NO_SPLIT);
        }
        if (split_mode != NO_SPLIT) {
            int HBT = split_allow[SPLIT_BI_HOR];
            int VBT = split_allow[SPLIT_BI_VER];
            int EnableBT = HBT || VBT;

            int HEQT = split_allow[SPLIT_EQT_HOR];
            int VEQT = split_allow[SPLIT_EQT_VER];
            int EnableEQT = HEQT || VEQT;

            int ctx_dir = cu_width_log2 == cu_height_log2 ? 0 : (cu_width_log2 > cu_height_log2 ? 1 : 2);

            int split_dir = (split_mode == SPLIT_BI_VER)  || (split_mode == SPLIT_EQT_VER);
            int split_typ = (split_mode == SPLIT_EQT_HOR) || (split_mode == SPLIT_EQT_VER);

            if (EnableEQT && EnableBT) {
                aec->enc_symbol(aec, split_typ, aec->syn_ctx->split_mode + idx);
            }
            if (split_typ == 0) {
                if (HBT && VBT) {
                    aec->enc_symbol(aec, split_dir, aec->syn_ctx->split_dir + ctx_dir);
                }
            }
            if (split_typ == 1) {
                if (HEQT && VEQT) {
                    aec->enc_symbol(aec, split_dir, aec->syn_ctx->split_dir + ctx_dir);
                }
            }
        }
    }
    return ret;
}

int writeSplitFlag_AEC(aec_t *aec, int val, int cu_bitsize)
{
    int bak_len = arienco_bits_written(aec);
    bin_ctx_sets_t  *ctx = aec->syn_ctx;

    aec->enc_symbol(aec, val, ctx->split_contexts + cu_bitsize - 4);
    return arienco_bits_written(aec) - bak_len;
}

void lbac_enc_lcu_delta_qp(aec_t *aec, int val, int last_dqp) 
{
    bin_ctx_sets_t *lbac_ctx = aec->syn_ctx;
    int act_sym;
    int act_ctx = ((last_dqp != 0) ? 1 : 0);

    if (val > 0) {
        act_sym = 2 * val - 1;
    } else {
        act_sym = -2 * val;
    }

    if (act_sym == 0) {
        aec->enc_symbol(aec, 1, lbac_ctx->lcu_qp_delta + act_ctx);
    } else {
        aec->enc_symbol(aec, 0, lbac_ctx->lcu_qp_delta + act_ctx);

        act_ctx = 2;

        if (act_sym == 1) {
            aec->enc_symbol(aec, 1, lbac_ctx->lcu_qp_delta + act_ctx);
        } else {
            aec->enc_symbol(aec, 0, lbac_ctx->lcu_qp_delta + act_ctx);

            act_ctx++;
            while (act_sym > 2) {
                aec->enc_symbol(aec, 0, lbac_ctx->lcu_qp_delta + act_ctx);
                act_sym--;
            }
            aec->enc_symbol(aec, 1, lbac_ctx->lcu_qp_delta + act_ctx);
        }
    }
}
