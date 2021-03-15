#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "codingUnit.h"
#include "vlc.h"
#include "block.h"
#include "header.h"
#include "global.h"
#include "AEC.h"
#include "rdoq.h"
#include "commonVariables.h"

#include "transform.h"
#include "intra-prediction.h"

ALIGNED_32(extern tab_char_t if_coef_chroma[9][4]);

tab_char_t tab_QP_SCALE_CR[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
    46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
    50, 50, 50, 51,
};

/*
*************************************************************************
* Function:Update the coordinates for the next cu_t to be processed
* Input:mb: MB address in scan order
* Output:
* Return:
* Attention:
*************************************************************************
*/

void init_lcu_pos(avs3_enc_t *h, int lcu_idx)
{
    const cfg_param_t *input = h->input;
    int lcu_size = LCU_SIZE;
    int lcu_x = lcu_idx % input->pic_width_in_lcu;
    int lcu_y = lcu_idx / input->pic_width_in_lcu;
    int pix_x = lcu_x * lcu_size;
    int pix_y = lcu_y * lcu_size;

    h->lcu_idx   = lcu_idx;
    h->lcu_y     = lcu_y;
    h->lcu_pix_x = pix_x;
    h->lcu_pix_y = pix_y;
    h->lcu_b8_x  = pix_x >> 3;
    h->lcu_b8_y  = pix_y >> 3;

    if (pix_x + lcu_size >= input->img_width) {
        h->lcu_width = input->img_width - pix_x;
    } else {
        h->lcu_width = lcu_size;
    }

    if (pix_y + lcu_size >= input->img_height) {
        h->lcu_height = input->img_height - pix_y;
    } else {
        h->lcu_height = lcu_size;
    }
}

void Init_Curr_codingUnit(avs3_enc_t *h, cu_t *cu, int uiBitSize, int uiPositionInPic, int qp, double lambda)
{
    const cfg_param_t *input = h->input;
    analyzer_t *a = &h->analyzer;
    image_t *img_org = h->img_org;

    cu->bitsize = uiBitSize;
    cu->QP = qp;
    cu->lambda = lambda;
}

void inter_pred_luma_blk(avs3_enc_t *h, pel_t *mpred, int i_pred, int pic_pix_x, int pic_pix_y, int PU_size_x, int PU_size_y, i16s_t *mv, int ref) 
{
    const cfg_param_t *input = h->input;
    frame_t *ref_frm = h->type == B_FRM ? h->ref_list[ref + 1] : h->ref_list[ref];
    image_t(*ref_img)[4][4] = &ref_frm->rec->img_1_4th;
    int x = mv[0];
    int y = mv[1];

    int i_src = (*ref_img)[y & 3][x & 3].i_stride[0];
    pel_t *src = (*ref_img)[y & 3][x & 3].plane[0];

    wait_ref_available(h, ref_frm, pic_pix_y + (mv[1] >> 2) + PU_size_y + 1);

    x = get_safe_mv(input->img_width, PU_size_x, pic_pix_x + (x >> 2));
    y = get_safe_mv(input->img_height, PU_size_y, pic_pix_y + (y >> 2));

    src += y * i_src + x;

    g_funs_handle.cpy_pel[PU_size_x >> 1](src, i_src, mpred, i_pred, PU_size_x, PU_size_y);
}


pel_t* inter_pred_luma_blk_get_pointer(avs3_enc_t *h, int *i_pred, int pic_pix_x, int pic_pix_y, int PU_size_x, int PU_size_y, i16s_t *mv, int ref)
{
    const cfg_param_t *input = h->input;
    frame_t *ref_frm = h->type == B_FRM ? h->ref_list[ref + 1] : h->ref_list[ref];
    image_t(*ref_img)[4][4] = &ref_frm->rec->img_1_4th;

    int x = mv[0];
    int y = mv[1];

    int i_src = (*ref_img)[y & 3][x & 3].i_stride[0];
    pel_t *src = (*ref_img)[y & 3][x & 3].plane[0];

    wait_ref_available(h, ref_frm, pic_pix_y + (mv[1] >> 2) + PU_size_y + 1);

    x = get_safe_mv(input->img_width, PU_size_x, pic_pix_x + (x >> 2));
    y = get_safe_mv(input->img_height, PU_size_y, pic_pix_y + (y >> 2));

    src += y * i_src + x;

    *i_pred = i_src;
    return src;

}

void inter_pred_chroma_blk(avs3_enc_t *h, pel_t *dstu, pel_t *dstv, int i_dst, int step_h, int step_v, i16s_t *mv, int ref)
{
    int ref_bak = ref;
    int x0, y0;
    int posx, posy;
    int mv_x, mv_y;
    int width_idx = step_h / 2 - 1;
    image_t *ref_img;
    pel_t *p_refu;
    pel_t *p_refv;
    int i_ref;
    const cfg_param_t *input = h->input;

    ref = (h->type == B_FRM) ? ref + 1 : ref;

    ref_img = &h->ref_list[ref]->rec->img_1_4th[0][0];

    mv_x = mv[0];
    mv_y = mv[1];

    x0 = get_safe_mv(input->img_width,  h->pu_size,  h->pu_pix_x + (mv_x >> 2)) >> 1;
    y0 = get_safe_mv(input->img_height, h->pu_size, h->pu_pix_y + (mv_y >> 2)) >> 1;

    posx = (mv_x & 7);
    posy = (mv_y & 7);

    i_ref = ref_img->i_stride[1];
    p_refu = ref_img->plane[1] + y0 * i_ref + x0;
    p_refv = ref_img->plane[2] + y0 * i_ref + x0;

    if (posy == 0 && posx == 0) {
        g_funs_handle.cpy_pel[width_idx + 1](p_refu, i_ref, dstu, i_dst, step_h, step_v);
        g_funs_handle.cpy_pel[width_idx + 1](p_refv, i_ref, dstv, i_dst, step_h, step_v);
    } else if (posy == 0) {
        g_funs_handle.ip_flt_c[IP_FLT_C_H][width_idx](p_refu, i_ref, dstu, i_dst, step_h, step_v, if_coef_chroma[posx], input->bit_depth);
        g_funs_handle.ip_flt_c[IP_FLT_C_H][width_idx](p_refv, i_ref, dstv, i_dst, step_h, step_v, if_coef_chroma[posx], input->bit_depth);
    } else if (posx == 0) {
        g_funs_handle.ip_flt_c[IP_FLT_C_V][width_idx](p_refu, i_ref, dstu, i_dst, step_h, step_v, if_coef_chroma[posy], input->bit_depth);
        g_funs_handle.ip_flt_c[IP_FLT_C_V][width_idx](p_refv, i_ref, dstv, i_dst, step_h, step_v, if_coef_chroma[posy], input->bit_depth);
    } else {
        g_funs_handle.ip_flt_c_ext[width_idx](p_refu, i_ref, dstu, i_dst, step_h, step_v, if_coef_chroma[posx], if_coef_chroma[posy], input->bit_depth);
        g_funs_handle.ip_flt_c_ext[width_idx](p_refv, i_ref, dstv, i_dst, step_h, step_v, if_coef_chroma[posx], if_coef_chroma[posy], input->bit_depth);
    }
}

void inter_pred_one_pu_luma(avs3_enc_t *h, cu_t *cu, pel_t *pred, int pu_idx)
{
    const cfg_param_t *input = h->input;
    int mode = cu->cuType;
    int pdir = cu->b8pdir;
    int md_direct = cu->mdirect_mode;
    int pic_pix_x = h->pu_pix_x;
    int pic_pix_y = h->pu_pix_y;
    int pu_size = h->pu_size;

    int b8x = pic_pix_x >> 3;
    int b8y = pic_pix_y >> 3;

    int i_dst = 1 << (cu->bitsize);

    pel_t *dst = pred + (pic_pix_y - h->cu_pix_y) * i_dst + (pic_pix_x - h->cu_pix_x);

    int bw_exist = 0;
	int offset = b8y * input->b8_stride + b8x;
    int fw_ref = h->p_cur_frm->refbuf  [offset];
    int bw_ref = h->refbuf_bw          [offset];
    i16s_t *fw_mv = h->p_cur_frm->mvbuf[offset];
    i16s_t *bw_mv = h->mvbuf_bw        [offset];

    pel_t *fw_luma = NULL, *bw_luma = NULL;
    int i_fw_luma = 0, i_bw_luma = 0;

    if (h->type == B_FRM && pdir != FORWARD) {
        bw_ref = -1;
        bw_exist = 1;
    }

    if (pdir != BACKWARD) {
        if (bw_exist) {
            fw_luma = inter_pred_luma_blk_get_pointer(h, &i_fw_luma, pic_pix_x, pic_pix_y, pu_size, pu_size, fw_mv, fw_ref);
        } else {
            inter_pred_luma_blk(h, dst, i_dst, pic_pix_x, pic_pix_y, pu_size, pu_size, fw_mv, fw_ref);
        }
    }

    if (bw_exist) {
        if (pdir == BACKWARD) {
            inter_pred_luma_blk(h, dst, i_dst, pic_pix_x, pic_pix_y, pu_size, pu_size, bw_mv, bw_ref);
        } else {
            bw_luma = inter_pred_luma_blk_get_pointer(h, &i_bw_luma, pic_pix_x, pic_pix_y, pu_size, pu_size, bw_mv, bw_ref);
            g_funs_handle.avg_pel[pu_size >> 1](dst, i_dst, fw_luma, i_fw_luma, bw_luma, i_bw_luma, pu_size, pu_size);
        }
    }
}

void inter_pred_one_pu(avs3_enc_t *h, cu_t *cu, int pu_idx, int need_pred_luma)
{
    const cfg_param_t *input = h->input;
    int mode = cu->cuType;
    int pdir = cu->b8pdir;
    int md_direct = cu->mdirect_mode;
    int pic_pix_x = h->pu_pix_x;
    int pic_pix_y = h->pu_pix_y;
    int pu_size  = h->pu_size;

    int b8x = pic_pix_x >> 3;
    int b8y = pic_pix_y >> 3;
    
    int pu_widthc  = h->pu_size >> 1;
    int pu_heightc = h->pu_size >> 1;

    int i_dst = 1 << (cu->bitsize);
    int i_dstc = i_dst >> 1;

    pel_t *dst = (mode ? h->pred_inter_luma : h->pred_inter_luma_skip[md_direct]) + (pic_pix_y - h->cu_pix_y) * i_dst + (pic_pix_x - h->cu_pix_x);
    pel_t *dstu = (mode ? h->pred_inter_chroma[0] : h->pred_inter_chroma_skip[md_direct][0]) + (((pic_pix_y - h->cu_pix_y) * i_dstc + (pic_pix_x - h->cu_pix_x)) >> 1);
    pel_t *dstv = (mode ? h->pred_inter_chroma[1] : h->pred_inter_chroma_skip[md_direct][1]) + (((pic_pix_y - h->cu_pix_y) * i_dstc + (pic_pix_x - h->cu_pix_x)) >> 1);

    int bw_exist = 0;
	int offset = b8y * input->b8_stride + b8x;
    int fw_ref    = h->p_cur_frm->refbuf[offset];
    int bw_ref    = h->refbuf_bw        [offset];
    i16s_t *fw_mv = h->p_cur_frm->mvbuf [offset];
    i16s_t *bw_mv = h->mvbuf_bw         [offset];

    pel_t *fw_luma = NULL, *bw_luma = NULL;
    int i_fw_luma = 0, i_bw_luma = 0;

    if (h->type == B_FRM && pdir != FORWARD) {
        bw_ref = -1;
        bw_exist = 1;
    }

    if (pdir != BACKWARD) {
        if (need_pred_luma) {
            if (bw_exist) {
                fw_luma = inter_pred_luma_blk_get_pointer(h, &i_fw_luma, pic_pix_x, pic_pix_y, pu_size, pu_size, fw_mv, fw_ref);
            } else {
                inter_pred_luma_blk(h, dst, i_dst, pic_pix_x, pic_pix_y, pu_size, pu_size, fw_mv, fw_ref);
            }
        }
        inter_pred_chroma_blk(h, dstu, dstv, i_dstc, pu_widthc, pu_heightc, fw_mv, fw_ref);
    }

    if (bw_exist) {
        if (pdir == BACKWARD) {
            if (need_pred_luma) {
                inter_pred_luma_blk(h, dst, i_dst, pic_pix_x, pic_pix_y, pu_size, pu_size, bw_mv, bw_ref);
            }
            inter_pred_chroma_blk(h, dstu, dstv, i_dstc, pu_widthc, pu_heightc, bw_mv, bw_ref);
        } else {
            ALIGNED_64(pel_t bw_pred[64 * 64]);
            if (need_pred_luma) {
                bw_luma = inter_pred_luma_blk_get_pointer(h, &i_bw_luma, pic_pix_x, pic_pix_y, pu_size, pu_size, bw_mv, bw_ref);
                g_funs_handle.avg_pel[pu_size >> 1](dst, i_dst, fw_luma, i_fw_luma, bw_luma, i_bw_luma, pu_size, pu_size);
            }
            inter_pred_chroma_blk(h, bw_pred, bw_pred + 32 * 32, pu_widthc, pu_widthc, pu_heightc, bw_mv, bw_ref);

            if (pu_widthc == i_dst) {
                int len = pu_widthc * pu_heightc;
                g_funs_handle.avg_pel_1d(dstu, dstu, bw_pred, len);
                g_funs_handle.avg_pel_1d(dstv, dstv, bw_pred + 32 * 32, len);
            } else {
                g_funs_handle.avg_pel[pu_widthc >> 1](dstu, i_dstc, dstu, i_dstc, bw_pred, pu_widthc, pu_widthc, pu_heightc);
                g_funs_handle.avg_pel[pu_widthc >> 1](dstv, i_dstc, dstv, i_dstc, bw_pred + 32 * 32, pu_widthc, pu_widthc, pu_heightc);
            }
        }
    }
}

int inter_luma_residual_coding(avs3_enc_t *h, aec_t *aec, pel_t *pred, int i_pred, cu_t *cu, int tu_bitsize, int b8, int force_zero)
{
    int nz = 0;
    int offset = (h->tu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->tu_pix_x - h->lcu_pix_x;
    pel_t *rec = h->p_rec[0] + offset;

    if (force_zero) {
        g_funs_handle.cpy_pel[h->tu_size >> 1](pred, i_pred, rec, CACHE_STRIDE, h->tu_size, h->tu_size);
    } else {
        pel_t *p_org = h->p_org[0] + offset;
        coef_t *p_cu_coef = h->cu_coefs.coef_y + b8 * (1 << tu_bitsize) * (1 << tu_bitsize);
        int i_coef = h->tu_q_size;
        int blkidx = b8 + (cu->trans_size == 1);

        transform_blk(h, blkidx, p_org, CACHE_STRIDE, pred, i_pred, p_cu_coef, i_coef, tu_bitsize, cu, 0);
        nz = quant_blk(h, aec, cu->QP, 0, p_cu_coef, tu_bitsize, cu, 0, 0);

        if (nz) {
            ALIGNED_16(coef_t tmp_coef[MAX_CU_SIZE * MAX_CU_SIZE]);
            inv_quant_blk(h, cu->QP, p_cu_coef, i_coef, tmp_coef, i_coef, tu_bitsize, 0);
            inv_transform(h, blkidx, tmp_coef, i_coef, pred, i_pred, rec, CACHE_STRIDE, tu_bitsize, cu, 0);
        } else {
            g_funs_handle.cpy_pel[h->tu_size >> 1](pred, i_pred, rec, CACHE_STRIDE, h->tu_size, h->tu_size);
        }
    }

    return nz;
}

int inter_luma_coding(avs3_enc_t *h, cu_t *cu, aec_t *aec, int uiBitSize, int force_zero, int cal_ssd)
{
    int cu_size = 1 << uiBitSize;
    int mode = cu->cuType;
    int offset = (h->cu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->cu_pix_x - h->lcu_pix_x;
    pel_t *rec = h->p_rec[0] + offset;
    pel_t *org = h->p_org[0] + offset;
    pel_t *pred = mode ? h->pred_inter_luma : h->pred_inter_luma_skip[cu->mdirect_mode];

    cu->cbp &= 0xfffffff0;

    if (cu->trans_size == 1) {
        int block8x8;
        for (block8x8 = 0; block8x8 < 4; block8x8++) {
            pel_t *p = pred;
            init_tu_pos(h, mode, uiBitSize, 1, block8x8);
            p = pred + (h->tu_pix_y - h->cu_pix_y) * cu_size + (h->tu_pix_x - h->cu_pix_x);

            if (inter_luma_residual_coding(h, aec, p, cu_size, cu, uiBitSize - 1, block8x8, force_zero)) {
                cu->cbp |= 1 << block8x8;
            }
        }
    } else {
        init_tu_pos(h, mode, uiBitSize, 0, 0);

        if (inter_luma_residual_coding(h, aec, pred, cu_size, cu, uiBitSize, 0, force_zero)) {
            cu->cbp |= 15;
        }
    }
    
    if (!cal_ssd) {
        return 0;
    } else {
        return g_funs_handle.cost_ssd[uiBitSize - 2](org, CACHE_STRIDE, rec, CACHE_STRIDE, 1 << uiBitSize);
    }
}

int chroma_residual_coding(avs3_enc_t *h, cu_t *cu, aec_t *aec, int uiBitSize, int force_zero, int cal_ssd)
{
    int uv;
    int all_ssd = 0;
    int IntraPrediction = IS_INTRA(cu);
    int pix_c_y = h->cu_pix_y >> 1;
    int pix_c_x = h->cu_pix_x >> 1;
    int cu_c_size = 1 << uiBitSize;
    int cr_cbp = 0;
    int mode = cu->cuType;
    int md_direct = cu->mdirect_mode;
    int offset = (pix_c_y - (h->lcu_pix_y >> 1)) * CACHE_STRIDE + pix_c_x - (h->lcu_pix_x >> 1);

    for (uv = 0; uv < 2; uv++) {
        pel_t *rec = h->p_rec[1 + uv] + offset;
        pel_t *org = h->p_org[1 + uv] + offset;
        pel_t *pred;
        int base_qp = min(cu->QP + h->chroma_delta_qp[uv], 79);
        int CurrentChromaQP = tab_QP_SCALE_CR[base_qp - h->input->bitdepth_qp_offset] + h->input->bitdepth_qp_offset;

        if (IntraPrediction) {
            pred = h->pred_intra_chroma[uv][cu->ipred_mode_c];
        } else {
            pred = (mode ? h->pred_inter_chroma[uv] : h->pred_inter_chroma_skip[md_direct][uv]);
        }
        
        if (force_zero) {
            g_funs_handle.cpy_pel[cu_c_size >> 1](pred, cu_c_size, rec, CACHE_STRIDE, cu_c_size, cu_c_size);
            if (cal_ssd) {
                all_ssd += g_funs_handle.cost_ssd[uiBitSize - 2](org, CACHE_STRIDE, rec, CACHE_STRIDE, 1 << uiBitSize);
            }
        } else {
            ALIGNED_64(coef_t tmp_block_88[MAX_CU_SIZE * MAX_CU_SIZE]);
            coef_t *p_cu_coef = uv ? h->cu_coefs.coef_v : h->cu_coefs.coef_u;
     
            transform_blk(h, 0, org, CACHE_STRIDE, pred, cu_c_size, p_cu_coef, cu_c_size, uiBitSize, cu, 1);

            if (quant_blk(h, aec, CurrentChromaQP, IntraPrediction ? 4 : 0, p_cu_coef, uiBitSize, cu, 1 + uv, DC_PRED)) {
                cr_cbp |= 1 << (4 + uv);
                inv_quant_blk(h, CurrentChromaQP, p_cu_coef, cu_c_size, tmp_block_88, cu_c_size, uiBitSize, 1);
                inv_transform(h, 0, tmp_block_88, cu_c_size, pred, cu_c_size, rec, CACHE_STRIDE, uiBitSize, cu, 1);
            } else {
                g_funs_handle.cpy_pel[cu_c_size >> 1](pred, cu_c_size, rec, CACHE_STRIDE, cu_c_size, cu_c_size);
            }
            if (cal_ssd) {
                all_ssd += g_funs_handle.cost_ssd[uiBitSize - 2](org, CACHE_STRIDE, rec, CACHE_STRIDE, 1 << uiBitSize);
            }
        }
    }

    cu->cbp |= (cr_cbp);   

    return all_ssd;
}

void intra_prepare_edge_luma(avs3_enc_t *h, cu_t *cu, pel_t *EP, int uiBitSize, int *ava_up, int *ava_left)
{
    const cfg_param_t *input = h->input;
    pel_t* edgepixels_trans_y = EP + 144;
    int x, y;
    int b8_x, b8_y;
    int bs_x = 1 << uiBitSize;
    int bs_y = 1 << uiBitSize;
    int img_y = h->pu_pix_y;
    int img_x = h->pu_pix_x;
    int i_left;
    pel_t *pTL, *pT, *pL, *pTemp;
    int init_val = 1 << (input->bit_depth - 1);
    int lcu_pix_x = h->lcu_pix_x;
    int lcu_pix_y = h->lcu_pix_y;
    int block_available_left      = img_x > 0;
    int block_available_up        = img_y > h->cu_pix_y ? 1 : h->cu_available_up;
    int block_available_up_right;
    int block_available_left_down;

    bs_x = bs_y = 1 << uiBitSize;

    block_available_up_right  = block_available_up   && (img_x + bs_x * 2 <= input->img_width);
    block_available_left_down = block_available_left && (img_y + bs_y * 2 <= lcu_pix_y + h->lcu_height);
    block_available_up_right  &= tab_Up_Right_Avail_Matrix64 [((img_y - lcu_pix_y) >> 3)][((img_x - lcu_pix_x + bs_x) >> 3) - 1];
    block_available_left_down &= tab_Left_Down_Avail_Matrix64[((img_y - lcu_pix_y + bs_y) >> 3) - 1][((img_x - lcu_pix_x) >> 3)];

    h->tu_available_up   = block_available_up;
    h->tu_available_left = block_available_left;

    if (img_y == lcu_pix_y && img_y) {
        pTL = h->p_rec_uprow[(lcu_pix_y >> LCU_SIZE_IN_BITS) - 1][0] + img_x - 1;
    } else {
        pTL = h->p_rec[0] + (img_y - lcu_pix_y - 1) * CACHE_STRIDE + img_x - lcu_pix_x - 1;
    }
    pT = pTL + 1;
    if (img_x == lcu_pix_x) {
        if (img_y != lcu_pix_y) {
            pTL = h->cache.intra_left_luma + img_y - lcu_pix_y;
            pL = pTL + 1;
        } else {
            pL = h->cache.intra_left_luma + img_y - lcu_pix_y + 1;
        }
        i_left = 1;
    } else {
        pL = h->p_rec[0] + (img_y - lcu_pix_y) * CACHE_STRIDE + img_x - lcu_pix_x - 1;
        i_left = CACHE_STRIDE;
    }

    b8_x = img_x >> 2;  
    b8_y = img_y >> 2;  

    for (x = -2 * bs_y; x <= 2 * bs_x; x++) {
        EP[x] = init_val;
    }

    *ava_left = block_available_left;
    *ava_up   = block_available_up;

    if (block_available_up) {
        for (x = 0; x < bs_x; x++) {
            EP[x + 1] = pT[x];
        }
        if (block_available_up_right) {
            for (x = 0; x < bs_x; x++) {
                if (img_x + bs_x + x >= input->img_width) {
                    EP[1 + x + bs_x] = pT[input->img_width - img_x - 1];
                } else {
                    EP[1 + x + bs_x] = pT[bs_x + x];
                }
            }
        } else {
            for (x = 0; x < bs_x; x++) {
                EP[1 + x + bs_x] = EP[bs_x];
            }
        }

        if (block_available_left) {
            EP[0] = *pTL;
        } else {
            EP[0] = *pT;
        }
    }

    pTemp = EP + (bs_x << 1);
    pTemp[1] = pTemp[2] = pTemp[3] = pTemp[4] = pTemp[0];

    if (block_available_left) {
        pTemp = pL;

        for (y = 0; y < bs_y; y++) {
            EP[-1 - y] = *pTemp;
            pTemp += i_left;
        }
        if (block_available_left_down) {
            for (y = 0; y < bs_y; y++) {
                if (img_y + bs_y + y >= input->img_height) {
                    EP[-1 - y - bs_y] = pTemp[-i_left];
                } else {
                    EP[-1 - y - bs_y] = *pTemp;
                    pTemp += i_left;
                }
            }
        } else {
            for (y = 0; y < bs_y; y++) {
                EP[-1 - y - bs_y] = EP[-bs_y];
            }
        }
        if (!block_available_up) {
            EP[0] = *pL;
        }
    }

    if (block_available_up && block_available_left) {
        EP[0] = *pTL;
    }

    pTemp = EP - (bs_y << 1);
    pTemp[-1] = pTemp[-2] = pTemp[-3] = pTemp[-4] = pTemp[0];

    for (y = 0; y < 2 * bs_y + 4; y++) {
        edgepixels_trans_y[y] = EP[-y];
    }
}

void intra_pred_chroma(avs3_enc_t *h, int uiBitSize, int predLmode, int mode)
{
    pel_t edgepixels[512 + 80];
#define EP ( edgepixels + ( ( 1 << uiBitSize ) * 2 ) + 4)
    pel_t* edgepixels_trans_y = EP + 144;

    const cfg_param_t *input = h->input;
    int bs_x = (1 << uiBitSize);
    int bs_y = (1 << uiBitSize);
    int i, x, y, uv;
    int img_cx = h->cu_pix_x >> 1;
    int img_cy = h->cu_pix_y >> 1;
    int b8x = img_cx >> 2;
    int b8y = img_cy >> 2;

    int mb_available_up_right;
    int mb_available_up;
    int mb_available_left;
    int mb_available_left_down;

    int pic_width_cr = input->img_width >> 1;
    int pic_height_cr = input->img_height >> 1;
    int lcu_c_x = h->lcu_pix_x >> 1;
    int lcu_c_y = h->lcu_pix_y >> 1;
    int init_val = 1 << (input->bit_depth - 1);

    mb_available_up   = h->cu_available_up;
    mb_available_left = (img_cx == 0) ? 0 : 1;
    mb_available_up_right  = mb_available_up && (img_cx + (1 << (uiBitSize + 1)) <= pic_width_cr);
    mb_available_left_down = mb_available_left && (img_cy + (1 << (uiBitSize + 1)) <= (h->lcu_pix_y + h->lcu_height) / 2);
    mb_available_up_right  &= tab_Up_Right_Avail_Matrix64[b8y - h->lcu_b8_y][b8x - h->lcu_b8_x + (bs_y >> 2) - 1];
    mb_available_left_down &= tab_Left_Down_Avail_Matrix64[b8y - h->lcu_b8_y + (bs_y >> 2) - 1][b8x - h->lcu_b8_x];

    // compute all chroma intra prediction modes for both U and V
    for (uv = 0; uv < 2; uv++) {
        pel_t *pTL, *pT, *pL, *pTemp;
        int i_left;

        if (img_cy == lcu_c_y && img_cy) {
            pTL = h->p_rec_uprow[(h->lcu_pix_y >> LCU_SIZE_IN_BITS) - 1][1 + uv] + img_cx - 1;
        } else {
            pTL = h->p_rec[1 + uv] + (img_cy - lcu_c_y - 1) * CACHE_STRIDE + img_cx - lcu_c_x - 1;
        }
        pT = pTL + 1;
        if (img_cx == lcu_c_x) {
            if (img_cy != lcu_c_y) {
                pTL = h->cache.intra_left_chroma[uv] + img_cy - lcu_c_y;
                pL = pTL + 1;
            } else {
                pL = h->cache.intra_left_chroma[uv] + img_cy - lcu_c_y + 1;
            }
            i_left = 1;
        } else {
            pL = h->p_rec[1 + uv] + (img_cy - lcu_c_y) * CACHE_STRIDE + img_cx - lcu_c_x - 1;
            i_left = CACHE_STRIDE;
        }

        for (i = -2 * bs_y; i <= 2 * bs_x; i++) {
            EP[i] = init_val;
        }

        if (mb_available_up) {
            memcpy(EP + 1, pT, bs_x * sizeof(pel_t));

            if (mb_available_up_right) {
                for (x = 0; x < bs_x; x++) {
                    if (img_cx + bs_x + x >= pic_width_cr) {
                        EP[1 + x + bs_x] = pT[pic_width_cr - img_cx - 1];
                    } else {
                        EP[1 + x + bs_x] = pT[bs_x + x];
                    }
                }
            } else {
                for (x = 0; x < bs_x; x++) {
                    EP[1 + x + bs_x] = EP[bs_x];
                }
            }

            if (mb_available_left) {
                EP[0] = *pTL;
            } else {
                EP[0] = *pT;
            }
        }

        pTemp = EP + (bs_x << 1);
        pTemp[1] = pTemp[2] = pTemp[3] = pTemp[4] = pTemp[0];

        if (mb_available_left) {
            pTemp = pL;

            for (y = 0; y < bs_y; y++) {
                EP[-1 - y] = *pTemp;
                pTemp += i_left;
            }

            if (mb_available_left_down) {
                for (y = 0; y < bs_y; y++) {
                    if (img_cy + bs_y + y >= pic_height_cr) {
                        EP[-1 - y - bs_y] = pTemp[-i_left];
                    } else {
                        EP[-1 - y - bs_y]  = *pTemp;
                        pTemp += i_left;
                    }
                }
            } else {
                for (y = 0; y < bs_y; y++) {
                    EP[-1 - y - bs_y] = EP[-bs_y];
                }
            }

            if (!mb_available_up) {
                EP[0] = *pL;
            }
        }

        if (mb_available_up && mb_available_left) {
            EP[0] = *pTL;
        }

        pTemp = EP - (bs_y << 1);
        pTemp[-1] = pTemp[-2] = pTemp[-3] = pTemp[-4] = pTemp[0];

        for (y = 0; y < 2 * bs_y + 4; y++) {
            edgepixels_trans_y[y] = EP[-y];
        }

        if (mode == -1) {
            for (i = 0; i < NUM_INTRA_PMODE_CHROMA; i++) {
                core_intra_get_chroma(EP, h->pred_intra_chroma[uv][i], 1 << uiBitSize, i, uiBitSize, mb_available_up, mb_available_left, predLmode, input->bit_depth);
            }
        } else {
            core_intra_get_chroma(EP, h->pred_intra_chroma[uv][mode], 1 << uiBitSize, mode, uiBitSize, mb_available_up, mb_available_left, predLmode, input->bit_depth);
        }
    }

#undef EP

}

void write_coeffs(avs3_enc_t *h, aec_t *aec, coef_t* p_coef, cu_t *cu, int uiBitSize, int bluma, int max_bits)
{
    if (aec->engin_flag == 2) {
        const i16u_t *scanp = com_tbl_scan[uiBitSize - 1];
        int num_coeff = 1 << (uiBitSize * 2);
        int run = 0;
        int bits = 0;
        int scan_pos = 0;
        int num_sig = g_funs_handle.get_nz_num(p_coef, num_coeff);

#define CHECK_EARLY_RETURN_RUNLEVEL() if (bits > max_bits) { aec->Ebits_cnt += bits; return; }

        while (num_sig) {
            int coef_cur = p_coef[scanp[scan_pos++]];

            if (coef_cur) {
                int level = COM_ABS(coef_cur);
                num_sig--;

                /* Run coding */
                if (run < 16) {
                    bits += run + 1;
                }
                else {
                    run -= 16;
                    bits += 18 + (uavs3e_get_log2(run) << 1);
                }

                /* Level coding */
                bits += level + 2; // level + sign + laft_flag
                CHECK_EARLY_RETURN_RUNLEVEL();

                run = 0;
            }
            else {
                run++;
            }
        }
        aec->Ebits_cnt += bits;

    } else {
        write_run_length_cc(aec, p_coef, uiBitSize, bluma);
    }
}

static __inline int pmvr_sign(int val)
{
    if (val > 0) {
        return 1;
    }
    else if (val < 0) {
        return -1;
    }
    else {
        return 0;
    }
}

/*
*************************************************************************
* Function:Writes CBP, DQUANT, and Luma Coefficients of an cu_t
* Input:
* Output:
* Return:
* Attention:
*************************************************************************
*/

int cal_pu_mvd(avs3_enc_t *h, cu_t *cu, int pu_idx, int ref, int bwflag, int mode, int pdir)
{
    int blk_idx = tab_inter_pu_2_blk_idx[cu->cuType][pu_idx];
    int j0 = blk_idx / 2;
    int i0 = blk_idx % 2;
    const cfg_param_t *input = h->input;
    me_info_t *me = &h->analyzer.me_info;

    i16s_t *allFwMv = (bwflag ? me->allBwMv : (pdir == BID ? me->allBidMv : me->allFwMv))[mode][j0][i0][ref];
    i16s_t *pred_mv = (bwflag ? me->predBwMv : me->predFwMv)[cu->cuType][j0][i0][ref];

    cu->mvd[pu_idx][bwflag][0] = allFwMv[0] - pred_mv[0];
    cu->mvd[pu_idx][bwflag][1] = allFwMv[1] - pred_mv[1];

    return 0;
}

int writeCBPandDqp(avs3_enc_t *h, aec_t *aec, cu_t *cu)
{
    int bak_len = arienco_bits_written(aec);
    int rate;

    writeCBP(h, cu, aec, cu->cbp);

    rate = arienco_bits_written(aec) - bak_len;

    return rate - (rate / 2) * 2;

}


/*
*************************************************************************
* Function:Writes motion info
* Input:
* Output:
* Return:
* Attention:
*************************************************************************
*/
void get_cu_mvd(avs3_enc_t *h, cu_t *cu, int uiBitSize)
{
    int pu_num, pu_idx;
    int mode = cu->cuType;
    int ref;
    const cfg_param_t *input = h->input;
    int b8_stride = input->b8_stride;
    int pdir = cu->b8pdir;

    pu_num = tab_inter_pu_num[mode];

    for (pu_idx = 0; pu_idx < pu_num; pu_idx++) {
        if (pdir == FORWARD || pdir == BID) {
            init_pu_inter_pos(h, mode, cu->mdirect_mode, uiBitSize, pu_idx);
            ref = h->p_cur_frm->refbuf[h->pu_b8_y * b8_stride + h->pu_b8_x];
            cal_pu_mvd(h, cu, pu_idx, ref, 0, mode, pdir);
        }
    }

    if (h->type == B_FRM) {
        for (pu_idx = 0; pu_idx < pu_num; pu_idx++) {
            if (pdir == BACKWARD || pdir == BID ) {
                cal_pu_mvd(h, cu, pu_idx, 0, 1, mode, pdir);
            }
        }
    }
}

void write_cu_header(avs3_enc_t *h, aec_t *aec, cu_t *cu, int uiBitSize)
{
    const cfg_param_t *input = h->input;

    if (!(h->type & I_FRM)) {
        writeCuTypeInfo(h, cu, aec);
  
        if (h->type == B_FRM && cu->cuType == P2NX2N) {
            writePdir(cu, aec, cu->b8pdir);
        }
    }

    if (IS_INTRA(cu)) {
        writeIntraPredMode(aec, cu->ipred_mode);
        writeCIPredMode(h, aec, cu->ipred_mode_c, cu->ipred_mode_real);
    }
}

void write_cu_ref_idx(avs3_enc_t *h, aec_t *aec, cu_t *cu, int uiBitSize)
{
    int pu_num, pu_idx;
    int mode = cu->cuType;
    int ref;
    const cfg_param_t *input = h->input;
    pu_num = tab_inter_pu_num[mode];

    for (pu_idx = 0; pu_idx < pu_num; pu_idx++) {
        init_pu_inter_pos(h, mode, cu->mdirect_mode, uiBitSize, pu_idx);
        ref = h->p_cur_frm->refbuf[h->pu_b8_y * input->b8_stride + h->pu_b8_x];
        writeRefFrame(h, aec, ref);
    }
}

void write_cu_mvd(avs3_enc_t *h, aec_t *aec, cu_t *cu)
{
    int pu_num, pu_idx;
    int mode = cu->cuType;
    int pdir = cu->b8pdir;

    pu_num = tab_inter_pu_num[mode];

    for (pu_idx = 0; pu_idx < pu_num; pu_idx++) {
        if (pdir == FORWARD || pdir == BID) {

            if (aec->engin_flag == 0) {
                //printf("mvd: %d x %d\n", cu->mvd[pu_idx][0][0], cu->mvd[pu_idx][0][1]);
            }
            writeMVD_AEC(aec, cu->mvd[pu_idx][0][0], 0);
            writeMVD_AEC(aec, cu->mvd[pu_idx][0][1], 1);
        }
    }

    if (h->type == B_FRM) {
        for (pu_idx = 0; pu_idx < pu_num; pu_idx++) {
            if (pdir == BACKWARD || pdir == BID) {
                writeMVD_AEC(aec, cu->mvd[pu_idx][1][0], 0);
                writeMVD_AEC(aec, cu->mvd[pu_idx][1][1], 1);
            }
        }
    }
}

void write_one_cu(avs3_enc_t *h, cu_t *cu, aec_t *aec, int uiBitSize, unsigned int uiPositionInPic)
{
    const cfg_param_t *input = h->input;

    init_cu_pos(h, uiPositionInPic, uiBitSize);

    if (aec->engin_flag == 0 && h->poc == 1) {
        if (h->cu_pix_x == 1904 && h->cu_pix_y == 32) {
            int a = 0;
        }
    }

    write_cu_header(h, aec, cu, uiBitSize);  

    if (!IS_INTRA(cu) && cu->cuType) {
        if (h->type != B_FRM && h->refs_num > 1) {
            write_cu_ref_idx(h, aec, cu, uiBitSize);
        }
        write_cu_mvd(h, aec, cu);
    }

    if (cu->cuType || cu->cbp) {
        int i;
        int currSMB_nr = (h->lcu_pix_y >> LCU_SIZE_IN_BITS) * input->pic_width_in_lcu + (h->lcu_pix_x >> LCU_SIZE_IN_BITS);
        int pix_x_in_lcu = h->cu_pix_x - h->lcu_pix_x;
        int pix_y_in_lcu = h->cu_pix_y - h->lcu_pix_y;
        coef_t *p_coef, *p_coef_u, *p_coef_v;
        int pos_bak;
        writeCBPandDqp(h, aec, cu);

        if (aec->engin_flag == 1) {
            return; 
        } else if (aec->engin_flag == 0) {
            pos_bak = arienco_bits_real_written(aec);
        }

        if (cu->trans_size) {
            for (i = 0; i < 4; i++) {
                if (cu->cbp & (1 << i)) {
                    init_tu_pos(h, cu->cuType, uiBitSize, 1, i);
                    p_coef = h->all_quant_coefs[currSMB_nr].coef_y + tab_b8_xy_cvto_zigzag[pix_y_in_lcu >> 3][pix_x_in_lcu >> 3] * 64;
                    p_coef += i * (1 << uiBitSize) * (1 << uiBitSize) >> 2;
                    write_coeffs(h, aec, p_coef, cu, uiBitSize - 1, 1, MAX_COST);
                }
            }
        } else {
            if (cu->cbp & 1) {
                init_tu_pos(h, cu->cuType, uiBitSize, 0, 0);
                p_coef = h->all_quant_coefs[currSMB_nr].coef_y + tab_b8_xy_cvto_zigzag[pix_y_in_lcu >> 3][pix_x_in_lcu >> 3] * 64;
                write_coeffs(h, aec, p_coef, cu, uiBitSize, 1, MAX_COST);
            }
        }
        
        p_coef_u = h->all_quant_coefs[currSMB_nr].coef_u + tab_b8_xy_cvto_zigzag[pix_y_in_lcu >> 3][pix_x_in_lcu >> 3] * 16;
        p_coef_v = h->all_quant_coefs[currSMB_nr].coef_v + tab_b8_xy_cvto_zigzag[pix_y_in_lcu >> 3][pix_x_in_lcu >> 3] * 16;

        if (cu->cbp & (1 << 4)) {
            write_coeffs(h, aec, p_coef_u, cu, uiBitSize - 1, 0, MAX_COST);
        }
        if (cu->cbp & (1 << 5)) {
            write_coeffs(h, aec, p_coef_v, cu, uiBitSize - 1, 0, MAX_COST);
        }

        if (aec->engin_flag == 0) {
            h->coef_bits += arienco_bits_real_written(aec) - pos_bak;
        }
    }
}

void write_cu_tree(avs3_enc_t *h, aec_t *aec, int uiBitSize, int uiPositionInPic)
{
    int i;
    cu_t *cu = &h->cu_data[uiPositionInPic];
    const cfg_param_t *input = h->input;
    int pix_x_InPic_start = (uiPositionInPic % input->pic_width_in_mcu) << 3;
    int pix_y_InPic_start = (uiPositionInPic / input->pic_width_in_mcu) << 3;
    int pix_x_InPic_end  = ((uiPositionInPic % input->pic_width_in_mcu) << 3) + (1 << uiBitSize);
    int pix_y_InPic_end  = ((uiPositionInPic / input->pic_width_in_mcu) << 3) + (1 << uiBitSize);
    int iBoundary_start   = (pix_x_InPic_start >= input->img_width) || (pix_y_InPic_start >= input->img_height);
    int out_of_pic     = (pix_x_InPic_end   >  input->img_width) || (pix_y_InPic_end   >  input->img_height);
    int split_flag = 0;
    int blksize = 1 << uiBitSize;


    if (!iBoundary_start) {
        lbac_enc_split_mode(h, aec, cu->bitsize == uiBitSize ? NO_SPLIT : SPLIT_QUAD, 
            blksize, blksize, LCU_SIZE_IN_BITS - uiBitSize, 0, pix_x_InPic_start, pix_y_InPic_start);

        if (cu->bitsize == uiBitSize) {
            write_one_cu(h, cu, aec, uiBitSize, uiPositionInPic);
        } else {
            for (i = 0; i < 4; i++) {
                int mb_x = (i % 2) << (uiBitSize - 3 - 1);   
                int mb_y = (i / 2) << (uiBitSize - 3 - 1);   
                int pos   = uiPositionInPic + mb_y * input->pic_width_in_mcu + mb_x;
                int pos_x = pix_x_InPic_start + (mb_x << 3);
                int pos_y = pix_y_InPic_start + (mb_y << 3);

                if (pos_x >= input->img_width || pos_y >= input->img_height) {
                    continue;
                }
                write_cu_tree(h, aec, uiBitSize - 1, pos);
            }
        }
    }
}

double write_one_cu_rdo(avs3_enc_t *h, aec_t *aec, cu_t *cu, int uiBitSize, double lambda, double mcost)
{
    int bit_pos = arienco_bits_written(aec);
    int max_bits = (int)(min((i64s_t)(mcost / lambda + 1), MAX_COST));

    write_cu_header(h, aec, cu, uiBitSize);

#define CHECK_EARLY_RETURN(aec)  if (arienco_bits_written(aec) - bit_pos > max_bits) { return MAX_COST; }
    
    CHECK_EARLY_RETURN(aec);

    if (!B_INTRA(cu->cuType) && cu->cuType) {
        if (h->type != B_FRM && h->refs_num > 1) {
            write_cu_ref_idx(h, aec, cu, uiBitSize);
        }
        write_cu_mvd(h, aec, cu);
        CHECK_EARLY_RETURN(aec);
    }

    if (cu->cuType || cu->cbp) {
        int i;
        int cu_size = 1 << uiBitSize;
        int cu_blk_size = cu_size * cu_size;

        bit_pos += writeCBPandDqp(h, aec, cu);

        CHECK_EARLY_RETURN(aec);

        if (cu->trans_size == 1) {
            for (i = 0; i < 4; i++) {
                if (cu->cbp & (1 << i)) {
                    coef_t *p_coef;
                    init_tu_pos(h, cu->cuType, uiBitSize, 1, i);
                    p_coef = h->cu_coefs.coef_y + (i * cu_blk_size >> 2);
                    write_coeffs(h, aec, p_coef, cu, uiBitSize - 1, 1, max_bits - (arienco_bits_written(aec) - bit_pos));
                    CHECK_EARLY_RETURN(aec);
                }
            }
        } else {
            if (cu->cbp & 1) {
                init_tu_pos(h, cu->cuType, uiBitSize, 0, 0);
                write_coeffs(h, aec, h->cu_coefs.coef_y, cu, uiBitSize, 1, max_bits - (arienco_bits_written(aec) - bit_pos));
                CHECK_EARLY_RETURN(aec);
            }
        }
        if (cu->cbp & (1 << 4)) {
            write_coeffs(h, aec, h->cu_coefs.coef_u, cu, uiBitSize - 1, 0, max_bits - (arienco_bits_written(aec) - bit_pos));
            CHECK_EARLY_RETURN(aec);
        }
        if (cu->cbp & (1 << 5)) {
            write_coeffs(h, aec, h->cu_coefs.coef_v, cu, uiBitSize - 1, 0, max_bits - (arienco_bits_written(aec) - bit_pos));
        }
    }
    return lambda * (arienco_bits_written(aec) - bit_pos);
}
