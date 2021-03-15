#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "global.h"
#include "commonVariables.h"
#include "AEC.h"
#include "loop-filter.h"

extern tab_char_t tab_QP_SCALE_CR[64];

static tab_char_t tab_deblock_alpha[64]  = {
    0,  0,  0,  0,  0,  0,  0,  0,
    1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  2,  2,  2,
    2,  2,  2,  3,  3,  3,  3,  4,
    4,  4,  5,  5,  6,  6,  7,  7,
    8,  9,  10, 10, 11, 12, 13, 15,
    16, 17, 19, 21, 23, 25, 27, 29,
    32, 35, 38, 41, 45, 49, 54, 59
} ;

static tab_char_t  tab_deblock_beta[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  2,  2,
    2,  2,  2,  2,  3,  3,  3,  3,
    4,  4,  5,  5,  5,  6,  6,  7,
    8,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22,
    23, 23, 24, 24, 25, 25, 26, 27
};

tab_i32s_t tab_eo_offset_map[] = { 3, 1, 0, 2, 4, 5, 6, 7 };

static tab_i32s_t tab_delta_band_cost[] = { -1, -1, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 16 };

#define LOOPFILTER_SIZE 3   //the minimum 8X8

#define distortion_cal(count, offset, diff) ((count) * (offset) - ((diff) << 1)) * (offset);


i32s_t get_distortion(int compIdx, int type, SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES], SAOBlkParam *sao_cur_param)
{
    int classIdc;
    i32s_t dist = 0;
    switch (type) {
    case SAO_TYPE_EO_0:
    case SAO_TYPE_EO_90:
    case SAO_TYPE_EO_135:
    case SAO_TYPE_EO_45:
        for (classIdc = 0; classIdc < NUM_SAO_EO_CLASSES; classIdc++) {
            dist += distortion_cal(saostatData[compIdx][type].count[classIdc], sao_cur_param[compIdx].offset[classIdc], saostatData[compIdx][type].diff[classIdc]);

        }
        break;
    default:
        printf("Not a supported type");
        assert(0);
        exit(-1);
    }

    return dist;
}

static int check_mv(int bfrm, int p_ref0, int p_ref1, int q_ref0, int q_ref1,
                          i16s_t *p_mv0, i16s_t *p_mv1, i16s_t *q_mv0, i16s_t *q_mv1)
{
    if (!bfrm) {
        if (p_ref0 != q_ref0 || p_ref0 == -1) {
            return 0;
        }
        if (abs(p_mv0[0] - q_mv0[0]) >= 4 || abs(p_mv0[1] - q_mv0[1]) >= 4) {
            return 0;
        }
    } else {
        if (p_ref0 != q_ref0 || p_ref1 != q_ref1 || (p_ref0 == -1 && p_ref1 == -1)) {
            return 0;
        }
        if (p_ref0 != -1) {
            if (abs(p_mv0[0] - q_mv0[0]) >= 4 || abs(p_mv0[1] - q_mv0[1]) >= 4) {
                return 0;
            }
        }
        if (p_ref1 != -1) {
            if (abs(p_mv1[0] - q_mv1[0]) >= 4 || abs(p_mv1[1] - q_mv1[1]) >= 4) {
                return 0;
            }
        }
    }
    return 1;
}

void xSetEdgeFilterParam(avs3_enc_t *h, int uiBitSize, int b8x, int b8y, int idir, int flag)
{
    int i;
    const cfg_param_t *input = h->input;

    int edge_size = 1 << (uiBitSize - LOOPFILTER_SIZE);

    int i_mcu = input->pic_width_in_mcu;
    int i_b8 = input->b8_stride;

    i16s_t(*p_mv)[2] = h->p_cur_frm->mvbuf  + b8y * i_b8 + b8x;
    char_t *p_ref    = h->p_cur_frm->refbuf + b8y * i_b8 + b8x;
    i16s_t(*p_mv_bw)[2] = h->mvbuf_bw  + b8y * i_b8 + b8x;
    char_t *p_ref_bw    = h->refbuf_bw + b8y * i_b8 + b8x;

    char_t *deblk_flag = h->deblk_flag + b8y * i_mcu + b8x;

    cu_t *cuQ = h->cu_data + b8y * i_mcu + b8x;
    cu_t *cuP;
    int bfrm = h->type & B_FRM;

#define IS_DEBLOCK_SKIPED(off1, off2)(              \
        (cuP->cbp == 0 && cuQ->cbp == 0)         && \
        check_mv(bfrm, p_ref[off1], p_ref_bw[off1], p_ref[off2], p_ref_bw[off2], p_mv[off1], p_mv_bw[off1], p_mv[off2], p_mv_bw[off2]) \
    )

    if (idir == 0) {
        for (i = 0; i < edge_size; i++) {
            cuP = cuQ - 1;
            deblk_flag[i_mcu * i] |= IS_DEBLOCK_SKIPED(0, -1) ? 0 : flag;
            p_mv  += i_b8;
            p_ref += i_b8;
            p_mv_bw  += i_b8;
            p_ref_bw += i_b8;
            cuQ += i_mcu;
        }
    } else {
        for (i = 0; i < edge_size; i++) {
            cuP = cuQ - i_mcu;
            deblk_flag[i] |= IS_DEBLOCK_SKIPED(0, -i_b8) ? 0 : flag << 4;
            p_mv++;
            p_ref++;
            p_mv_bw++;
            p_ref_bw++;
            cuQ++;
        }
    }
}


void set_cu_deblk_flag(avs3_enc_t *h, int uiBitSize, unsigned int uiPositionInPic)
{
    int i;
    const cfg_param_t *input = h->input;
    int pix_x_InPic_start = (uiPositionInPic % input->pic_width_in_mcu) << 3;
    int pix_y_InPic_start = (uiPositionInPic / input->pic_width_in_mcu) << 3;
    int pos_x, pos_y, pos_InPic;

    int b8x = uiPositionInPic % input->pic_width_in_mcu;
    int b8y = uiPositionInPic / input->pic_width_in_mcu;

    cu_t *cu = &h->cu_data[uiPositionInPic];
    int flag_deblk_all = 3;
    int flag_deblk_luma = 2;

    if (cu->bitsize < uiBitSize) {
        for (i = 0; i < 4; i++) {
            int mb_x = (i &  1) << (uiBitSize - 3 - 1);     //uiBitSize 5:1 ; 6:2
            int mb_y = (i >> 1) << (uiBitSize - 3 - 1);     //uiBitSize 5:1 ; 6:2
            int pos = uiPositionInPic + mb_y * input->pic_width_in_mcu + mb_x;

            pos_x = pix_x_InPic_start + (mb_x << 3);
            pos_y = pix_y_InPic_start + (mb_y << 3);
            pos_InPic = (pos_x >= input->img_width || pos_y >= input->img_height);

            if (pos_InPic) {
                continue;
            }
            set_cu_deblk_flag(h, uiBitSize - 1, pos);
        }
        return;
    }

    if (b8x) {
        xSetEdgeFilterParam(h, uiBitSize, b8x, b8y, 0, flag_deblk_all);
    }
    if (b8y) {
        xSetEdgeFilterParam(h, uiBitSize, b8x, b8y, 1, flag_deblk_all);
    }

    if (uiBitSize <= 3) {
        return;
    }

    i = (uiBitSize - LOOPFILTER_SIZE - 1);

    if (cu->trans_size == 1 && cu->cbp != 0) {
        xSetEdgeFilterParam(h, uiBitSize, b8x + (1 << i), b8y, 0, flag_deblk_luma);
        xSetEdgeFilterParam(h, uiBitSize, b8x, b8y + (1 << i), 1, flag_deblk_luma);
    }
}

void deblock_luma_ver(pel_t *src, int stride, int Alpha, int Beta)
{
    for (int pel = 0; pel < 8; pel++) {
        int L3 = src[-4];
        int L2 = src[-3];
        int L1 = src[-2];
        int L0 = src[-1];
        int R0 = src[0];
        int R1 = src[1];
        int R2 = src[2];
        int R3 = src[3];
        int abs0 = COM_ABS(R0 - L0);
        int flat_l = (COM_ABS(L1 - L0) < Beta) ? 2 : 0;
        int flat_r = (COM_ABS(R0 - R1) < Beta) ? 2 : 0;
        int fs;

        if (COM_ABS(L2 - L0) < Beta) {
            flat_l++;
        }
        if (COM_ABS(R0 - R2) < Beta) {
            flat_r++;
        }

        switch (flat_l + flat_r) {
        case 6:
            fs = (COM_ABS(R0 - R1) <= Beta / 4 && COM_ABS(L0 - L1) <= Beta / 4 && abs0 < Alpha) ? 4 : 3;
            break;
        case 5:
            fs = (R1 == R0 && L0 == L1) ? 3 : 2;
            break;
        case 4:
            fs = (flat_l == 2) ? 2 : 1;
            break;
        case 3:
            fs = (COM_ABS(L1 - R1) < Beta) ? 1 : 0;
            break;
        default:
            fs = 0;
            break;
        }

        switch (fs) {
        case 4:
            src[-3] = (pel_t)((((L3 + L2 + L1) << 1) + L0 + R0 + 4) >> 3);                          //L2
            src[-2] = (pel_t)((((L2 + L0 + L1) << 2) + L1 + R0 * 3 + 8) >> 4);                      //L1
            src[-1] = (pel_t)(((L2 + R1) * 3 + ((L1 + L0 + R0) << 3) + (L0 << 1) + 16) >> 5);       //L0
            src[0] = (pel_t)(((R2 + L1) * 3 + ((R1 + R0 + L0) << 3) + (R0 << 1) + 16) >> 5);       //R0
            src[1] = (pel_t)((((R2 + R1 + R0) << 2) + R1 + L0 * 3 + 8) >> 4);                      //R1
            src[2] = (pel_t)((((R3 + R2 + R1) << 1) + R0 + L0 + 4) >> 3);                          //R2
            break;
        case 3:
            src[-2] = (pel_t)((L2 * 3 + L1 * 8 + L0 * 4 + R0 + 8) >> 4);                           //L1
            src[-1] = (pel_t)((L2 + (L1 << 2) + (L0 << 2) + (L0 << 1) + (R0 << 2) + R1 + 8) >> 4); //L0
            src[0] = (pel_t)((L1 + (L0 << 2) + (R0 << 2) + (R0 << 1) + (R1 << 2) + R2 + 8) >> 4); //R0
            src[1] = (pel_t)((R2 * 3 + R1 * 8 + R0 * 4 + L0 + 8) >> 4);                           //R1
            break;
        case 2:
            src[-1] = (pel_t)(((L1 << 1) + L1 + (L0 << 3) + (L0 << 1) + (R0 << 1) + R0 + 8) >> 4);
            src[0] = (pel_t)(((L0 << 1) + L0 + (R0 << 3) + (R0 << 1) + (R1 << 1) + R1 + 8) >> 4);
            break;
        case 1:
            src[-1] = (pel_t)((L0 * 3 + R0 + 2) >> 2);
            src[0] = (pel_t)((R0 * 3 + L0 + 2) >> 2);
            break;
        default:
            break;
        }
        src += stride;
    }
}

void deblock_luma_hor(pel_t *src, int stride, int Alpha, int Beta)
{
    int i, line_size = 8;
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int inc4 = inc + inc3;

    for (i = 0; i < line_size; i++, src++) {
        int L3 = src[-inc4];
        int L2 = src[-inc3];
        int L1 = src[-inc2];
        int L0 = src[-inc];
        int R0 = src[0];
        int R1 = src[inc];
        int R2 = src[inc2];
        int R3 = src[inc3];
        int abs0 = COM_ABS(R0 - L0);
        int flat_l = (COM_ABS(L1 - L0) < Beta) ? 2 : 0;
        int flat_r = (COM_ABS(R0 - R1) < Beta) ? 2 : 0;
        int fs;

        if (COM_ABS(L2 - L0) < Beta) {
            flat_l++;
        }
        if (COM_ABS(R0 - R2) < Beta) {
            flat_r++;
        }

        switch (flat_l + flat_r) {
        case 6:
            fs = (COM_ABS(R0 - R1) <= Beta / 4 && COM_ABS(L0 - L1) <= Beta / 4 && abs0 < Alpha) ? 4 : 3;
            break;
        case 5:
            fs = (R1 == R0 && L0 == L1) ? 3 : 2;
            break;
        case 4:
            fs = (flat_l == 2) ? 2 : 1;
            break;
        case 3:
            fs = (COM_ABS(L1 - R1) < Beta) ? 1 : 0;
            break;
        default:
            fs = 0;
            break;
        }

        switch (fs) {
        case 4:
            src[-inc3] = (pel_t)((((L3 + L2 + L1) << 1) + L0 + R0 + 4) >> 3);                         //L2
            src[-inc2] = (pel_t)((((L2 + L0 + L1) << 2) + L1 + R0 * 3 + 8) >> 4);                     //L1
            src[-inc] = (pel_t)(((L2 + R1) * 3 + ((L1 + L0 + R0) << 3) + (L0 << 1) + 16) >> 5);      //L0
            src[0] = (pel_t)(((R2 + L1) * 3 + ((R1 + R0 + L0) << 3) + (R0 << 1) + 16) >> 5);      //R0
            src[inc] = (pel_t)((((R2 + R1 + R0) << 2) + R1 + L0 * 3 + 8) >> 4);                     //R1
            src[inc2] = (pel_t)((((R3 + R2 + R1) << 1) + R0 + L0 + 4) >> 3);                         //R2
            break;
        case 3:
            src[-inc2] = (pel_t)((L2 * 3 + L1 * 8 + L0 * 4 + R0 + 8) >> 4);                           //L1
            src[-inc] = (pel_t)((L2 + (L1 << 2) + (L0 << 2) + (L0 << 1) + (R0 << 2) + R1 + 8) >> 4); //L0
            src[0] = (pel_t)((L1 + (L0 << 2) + (R0 << 2) + (R0 << 1) + (R1 << 2) + R2 + 8) >> 4); //R0
            src[inc] = (pel_t)((R2 * 3 + R1 * 8 + R0 * 4 + L0 + 8) >> 4);                           //R2
            break;
        case 2:
            src[-inc] = (pel_t)(((L1 << 1) + L1 + (L0 << 3) + (L0 << 1) + (R0 << 1) + R0 + 8) >> 4);
            src[0] = (pel_t)(((L0 << 1) + L0 + (R0 << 3) + (R0 << 1) + (R1 << 1) + R1 + 8) >> 4);
            break;
        case 1:
            src[-inc] = (pel_t)((L0 * 3 + R0 + 2) >> 2);
            src[0] = (pel_t)((R0 * 3 + L0 + 2) >> 2);
            break;
        default:
            break;
        }
    }
}

void deblock_chroma_ver(pel_t *srcu, pel_t *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v)
{
    int i, line_size = 4;
    int alpha[2] = { alpha_u, alpha_v };
    int beta[2] = { beta_u, beta_v };
    pel_t* p_src;
    int uv;

    for (uv = 0; uv < 2; uv++) {
        p_src = uv ? srcv : srcu;
        for (i = 0; i < line_size; i++, p_src += stride) {
            int L2 = p_src[-3];
            int L1 = p_src[-2];
            int L0 = p_src[-1];
            int R0 = p_src[0];
            int R1 = p_src[1];
            int R2 = p_src[2];
            int delta_m = COM_ABS(R0 - L0);
            int delta_l = COM_ABS(L1 - L0);
            int delta_r = COM_ABS(R0 - R1);

            if ((delta_l < beta[uv]) && (delta_r < beta[uv])) {
                p_src[-1] = (pel_t)((L1 * 3 + L0 * 10 + R0 * 3 + 8) >> 4);                  // L0
                p_src[0] = (pel_t)((R1 * 3 + R0 * 10 + L0 * 3 + 8) >> 4);                  // R0
                if ((COM_ABS(L2 - L0) < beta[uv]) && (COM_ABS(R2 - R0) < beta[uv]) &&
                    delta_r <= beta[uv] / 4 && delta_l <= beta[uv] / 4 && delta_m < alpha[uv]) {
                    p_src[-2] = (pel_t)((L2 * 3 + L1 * 8 + L0 * 3 + R0 * 2 + 8) >> 4);      // L1
                    p_src[1] = (pel_t)((R2 * 3 + R1 * 8 + R0 * 3 + L0 * 2 + 8) >> 4);      // R1
                }
            }
        }
    }
}

void deblock_chroma_hor(pel_t *srcu, pel_t *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v)
{
    int i, line_size = 4;
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int alpha[2] = { alpha_u, alpha_v };
    int beta[2] = { beta_u, beta_v };
    pel_t *p_src;
    int uv;

    for (uv = 0; uv < 2; uv++) {
        p_src = uv ? srcv : srcu;
        for (i = 0; i < line_size; i++, p_src ++) {
            int L2 = p_src[-inc3];
            int L1 = p_src[-inc2];
            int L0 = p_src[-inc];
            int R0 = p_src[0];
            int R1 = p_src[inc];
            int R2 = p_src[inc2];
            int delta_m = COM_ABS(R0 - L0);
            int delta_l = COM_ABS(L1 - L0);
            int delta_r = COM_ABS(R0 - R1);

            if ((delta_l < beta[uv]) && (delta_r < beta[uv])) {
                p_src[-inc] = (pel_t)((L1 * 3 + L0 * 10 + R0 * 3 + 8) >> 4);                  // L0
                p_src[0] = (pel_t)((R1 * 3 + R0 * 10 + L0 * 3 + 8) >> 4);                     // R0
                if ((COM_ABS(L2 - L0) < beta[uv]) && (COM_ABS(R2 - R0) < beta[uv]) &&
                    delta_r <= beta[uv] / 4 && delta_l <= beta[uv] / 4 && delta_m < alpha[uv]) {
                    p_src[-inc2] = (pel_t)((L2 * 3 + L1 * 8 + L0 * 3 + R0 * 2 + 8) >> 4);     // L1
                    p_src[inc] = (pel_t)((R2 * 3 + R1 * 8 + R0 * 3 + L0 * 2 + 8) >> 4);       // R1
                }
            }
        }
    }
}


/*
*************************************************************************
* Function:
* Input:
* Output:
* Return:
* Attention:
*************************************************************************
*/
void DeblockMb(avs3_enc_t *h, image_t *rec_img, int mb_y, int mb_x, int dir)
{
    const cfg_param_t *input = h->input;
    int deblk_flag = (h->deblk_flag[mb_y * input->pic_width_in_mcu + mb_x] >> (dir * 4)) & 0xF;
    int shift = input->bit_depth - 8;

    if (deblk_flag) {
        int Alpha, Beta;
        cu_t *MbQ = h->cu_data + mb_y * input->pic_width_in_mcu + mb_x;
        cu_t *MbP = (dir) ? (MbQ - (input->img_width >> 3)) : (MbQ - 1);
        int QP = (MbP->QP + MbQ->QP + 1) >> 1;
        int i_src  = rec_img->i_stride[0];
        pel_t *src = rec_img->plane[0] + (mb_y * 8) * i_src + mb_x * 8;

        Alpha = tab_deblock_alpha[Clip3(0, MAX_QP, QP - input->bitdepth_qp_offset + input->alpha_c_offset)];
        Beta  = tab_deblock_beta [Clip3(0, MAX_QP, QP - input->bitdepth_qp_offset + input->beta_offset)];
        Alpha <<= shift;
        Beta  <<= shift;

        if (deblk_flag & 0x2) {
            g_funs_handle.deblk_luma[dir](src, i_src, Alpha, Beta);
        }

        if ((deblk_flag & 0x1) && ((mb_y % 2 == 0 && dir) || (mb_x % 2 == 0) && (!dir))) {
            int i_src_uv = rec_img->i_stride[1];
            pel_t *srcu = rec_img->plane[1] + (mb_y * 4) * i_src_uv + mb_x * 4;
            pel_t *srcv = rec_img->plane[2] + (mb_y * 4) * i_src_uv + mb_x * 4;
            int QpPU = MbP->QP + h->chroma_delta_qp[0];
            int QpPV = MbP->QP + h->chroma_delta_qp[1];
            int QpQU = MbQ->QP + h->chroma_delta_qp[0];
            int QpQV = MbQ->QP + h->chroma_delta_qp[1];
            int QpU, QpV;
            int AlphaU, AlphaV, BetaU, BetaV;

            if (QpPU >= input->bitdepth_qp_offset) {
                QpPU = tab_QP_SCALE_CR[QpPU - input->bitdepth_qp_offset];
            }
            if (QpPV >= input->bitdepth_qp_offset) {
                QpPV = tab_QP_SCALE_CR[QpPV - input->bitdepth_qp_offset];
            }
            if (QpQU >= input->bitdepth_qp_offset) {
                QpQU = tab_QP_SCALE_CR[QpQU - input->bitdepth_qp_offset];
            }
            if (QpQV >= input->bitdepth_qp_offset) {
                QpQV = tab_QP_SCALE_CR[QpQV - input->bitdepth_qp_offset];
            }
            QpU = ((QpPU + QpQU + 1) >> 1);
            QpV = ((QpPV + QpQV + 1) >> 1);

            AlphaU = tab_deblock_alpha[Clip3(0, MAX_QP, QpU + input->alpha_c_offset)] << shift;
            BetaU = tab_deblock_beta[Clip3(0, MAX_QP, QpU + input->beta_offset)] << shift;
            AlphaV = tab_deblock_alpha[Clip3(0, MAX_QP, QpV + input->alpha_c_offset)] << shift;
            BetaV = tab_deblock_beta[Clip3(0, MAX_QP, QpV + input->beta_offset)] << shift;

            g_funs_handle.deblk_chroma[dir](srcu, srcv, i_src_uv, AlphaU, BetaU, AlphaV, BetaV);
        }
    }
}

void getStatblk(void *handle, void *sao_data, int compIdx, const int pix_y, const int pix_x, int lcu_pix_height, int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down)
{
    avs3_enc_t *h = (avs3_enc_t*)handle;
    SAOStatData *saostatsData = (SAOStatData*)sao_data;
    int type;
    int start_x, end_x, start_y, end_y;
    int start_x_r0, end_x_r0, start_x_r, end_x_r, start_x_rn, end_x_rn;
    int x, y;
    char_t leftsign, rightsign, upsign, downsign;
    int diff;
    SAOStatData *statsDate;
    char_t signupline[69];
    char_t *signupline1;
    int reg = 0;
    int edgetype;
    pel_t *org_base, *rec_base;
    pel_t *rec, *org;
    int i_rec, i_org;
    int bit_depth = h->input->bit_depth;
    int smb_available_upleft = (smb_available_up && smb_available_left);
    int smb_available_upright = (smb_available_up && smb_available_right);
    int smb_available_leftdown = (smb_available_down && smb_available_left);
    int smb_available_rightdwon = (smb_available_down && smb_available_right);

    i_rec = h->img_rec->i_stride[compIdx];
    i_org = h->img_org->i_stride[compIdx];

    for (type = 0; type < NUM_SAO_NEW_TYPES; type++) {
        rec = h->img_rec->plane[compIdx] + pix_y * i_rec + pix_x;
        org = h->img_org->plane[compIdx] + pix_y * i_org + pix_x;

        statsDate = &(saostatsData[type]);
        switch (type) {
        case SAO_TYPE_EO_0:
            end_y = lcu_pix_height;
            start_x = smb_available_left ? 0 : 1;
            end_x = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);

            for (y = 0; y < end_y; y++) {
                diff = rec[start_x] - rec[start_x - 1];
                leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                for (x = start_x; x < end_x; x++) {
                    diff = rec[x] - rec[x + 1];
                    rightsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    edgetype = leftsign + rightsign;
                    leftsign = -rightsign;
                    statsDate->diff[edgetype + 2] += (org[x] - rec[x]);
                    statsDate->count[edgetype + 2]++;
                }
                org += i_org;
                rec += i_rec;
            }

            break;
        case SAO_TYPE_EO_90:
            end_x = lcu_pix_width;
            start_y = smb_available_up ? 0 : 1;
            end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);

            org_base = org + start_y * i_org;
            rec_base = rec + start_y * i_rec;

            for (x = 0; x < end_x; x++) {
                org = org_base + x;
                rec = rec_base + x;
                diff = rec[0] - rec[-i_rec];

                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                for (y = start_y; y < end_y; y++) {
                    diff = rec[0] - rec[i_rec];
                    downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    edgetype = downsign + upsign;
                    upsign = -downsign;
                    statsDate->diff[edgetype + 2] += (org[0] - rec[0]);
                    statsDate->count[edgetype + 2]++;
                    org += i_org;
                    rec += i_rec;
                }

            }

            break;
        case SAO_TYPE_EO_135:
            start_x_r0 = smb_available_upleft ? 0 : 1;
            end_x_r0 = smb_available_up ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;
            start_x_r = smb_available_left ? 0 : 1;
            end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
            start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
            end_x_rn = smb_available_rightdwon ? lcu_pix_width : (lcu_pix_width - 1);

            //init the line buffer
            for (x = start_x_r + 1; x < end_x_r + 1; x++) {
                diff = rec[x + i_rec] - rec[x - 1];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                signupline[x] = upsign;
            }
            //first row
            for (x = start_x_r0; x < end_x_r0; x++) {
                diff = rec[x] - rec[x - 1 - i_rec];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = upsign - signupline[x + 1];
                statsDate->diff[edgetype + 2] += (org[x] - rec[x]);
                statsDate->count[edgetype + 2]++;
            }

            //middle rows
            org += i_org;
            rec += i_rec;

            for (y = 1; y < lcu_pix_height - 1; y++) {
                for (x = start_x_r; x < end_x_r; x++) {
                    if (x == start_x_r) {
                        diff = rec[x] - rec[x - 1 - i_rec];
                        upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                        signupline[x] = upsign;
                    }
                    diff = rec[x] - rec[x + 1 + i_rec];
                    downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    edgetype = downsign + signupline[x];
                    statsDate->diff[edgetype + 2] += (org[x] - rec[x]);
                    statsDate->count[edgetype + 2]++;
                    signupline[x] = (char_t)reg;
                    reg = -downsign;
                }
                org += i_org;
                rec += i_rec;
            }
            //last row
            if (lcu_pix_height == 0) {
                org -= i_org * 2;
                rec -= i_rec * 2;
            }

            for (x = start_x_rn; x < end_x_rn; x++) {
                if (x == start_x_r) {
                    diff = rec[x] - rec[x - 1 - i_rec];
                    upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    signupline[x] = upsign;
                }
                diff = rec[x] - rec[x + 1 + i_rec];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + signupline[x];
                statsDate->diff[edgetype + 2] += (org[x] - rec[x]);
                statsDate->count[edgetype + 2]++;
            }

            break;
        case SAO_TYPE_EO_45:
            start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
            end_x_r0 = smb_available_upright ? lcu_pix_width : (lcu_pix_width - 1);
            start_x_r = smb_available_left ? 0 : 1;
            end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
            start_x_rn = smb_available_leftdown ? 0 : 1;
            end_x_rn = smb_available_down ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;

            //init the line buffer
            signupline1 = signupline + 1;
            for (x = start_x_r - 1; x < max(end_x_r - 1, end_x_r0 - 1); x++) {
                diff = rec[x + i_rec] - rec[x + 1];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                signupline1[x] = upsign;
            }
            //first row
            for (x = start_x_r0; x < end_x_r0; x++) {
                diff = rec[x] - rec[x - i_rec + 1];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = upsign - signupline1[x - 1];
                statsDate->diff[edgetype + 2] += (org[x] - rec[x]);
                statsDate->count[edgetype + 2]++;
            }

            //middle rows
            org += i_org;
            rec += i_rec;

            for (y = 1; y < lcu_pix_height - 1; y++) {
                for (x = start_x_r; x < end_x_r; x++) {
                    if (x == end_x_r - 1) {
                        diff = rec[x] - rec[x + 1 - i_rec];
                        upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                        signupline1[x] = upsign;
                    }
                    diff = rec[x] - rec[x - 1 + i_rec];
                    downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    edgetype = downsign + signupline1[x];
                    statsDate->diff[edgetype + 2] += (org[x] - rec[x]);
                    statsDate->count[edgetype + 2]++;
                    signupline1[x - 1] = -downsign;
                }
                org += i_org;
                rec += i_rec;
            }
            if (lcu_pix_height == 0) {
                org -= i_org * 2;
                rec -= i_rec * 2;
            }
            for (x = start_x_rn; x < end_x_rn; x++) {
                if (x == end_x_r - 1) {
                    diff = rec[x] - rec[x + 1 - i_rec];
                    upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    signupline1[x] = upsign;
                }
                diff = rec[x] - rec[x - 1 + i_rec];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + signupline1[x];
                statsDate->diff[edgetype + 2] += (org[x] - rec[x]);
                statsDate->count[edgetype + 2]++;
            }
            break;
        default:
            printf("Not a supported SAO types\n");
            assert(0);
            exit(-1);

        }
    }
}

void sao_lcu_statistics(avs3_enc_t *h, int lcu_x, int lcu_y, SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES])
{
    sao_enc_t *sao = &h->sao_enc;
    const cfg_param_t* input = h->input;
    int pix_y = lcu_y * LCU_SIZE;
    int pix_x = lcu_x * LCU_SIZE;
    int lcu_pix_width = lcu_x == input->pic_width_in_lcu - 1 ? input->img_width - lcu_x * LCU_SIZE : LCU_SIZE;
    int lcu_pix_height = lcu_y == input->pic_height_in_lcu - 1 ? input->img_height - lcu_y * LCU_SIZE : LCU_SIZE;
    int isAboveAvail = pix_y ? 1 : 0;
    int isBelowAvail = pix_y + lcu_pix_height < input->img_height;
    int isLeftAvail = pix_x ? 1 : 0;
    int isRightAvail = pix_x + lcu_pix_width  < input->img_width;
    int isFltSliceBorder = isAboveAvail;
    int *sli_nr = h->slice_nr + lcu_y;

    int y_offset = 0, x_offset = 0, width_offset = 0, height_offset = 0;
    int blk_x, blk_y, blk_width, blk_height;

    if (!LOOPFLT_CROSS_SLICE && isFltSliceBorder && sli_nr[0] != sli_nr[-1]) {
        isFltSliceBorder = 0;
    }

    if (isLeftAvail) {
        x_offset -= 4;
        width_offset += 4;
    }
    if (isRightAvail) {
        width_offset -= 4;
    }
    if (isBelowAvail) {
        height_offset -= 4;
    }
    if (isAboveAvail && isFltSliceBorder) {
        y_offset -= 3;
        height_offset += 3;
    }

    blk_width = width_offset + lcu_pix_width;
    blk_height = height_offset + lcu_pix_height;
    blk_x = x_offset + pix_x;
    blk_y = y_offset + pix_y;

    if (sao->sao_on[0]) {
        g_funs_handle.sao_flt[0](h, saostatData[0], 0, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail);
    } 

    blk_width = width_offset + (lcu_pix_width >> 1);
    blk_height = height_offset + (lcu_pix_height >> 1);
    blk_x = x_offset + (pix_x >> 1);
    blk_y = y_offset + (pix_y >> 1);

    if (sao->sao_on[1]) {
        g_funs_handle.sao_flt[0](h, saostatData[1], 1, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail);
    }
    if (sao->sao_on[2]) {
        g_funs_handle.sao_flt[0](h, saostatData[2], 2, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail);
    }
}


double SAORDCost_for_mode_merge(avs3_enc_t *h, int lcu_x, int lcu_y, aec_t *aec, double sao_labmda, 
    SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES], 
    SAOBlkParam(*rec_saoBlkParam)[NUM_SAO_COMPONENTS], 
    SAOBlkParam *sao_cur_param, int *MergeLeftAvail, int *MergeUpAvail)
{
    SAOBlkParam merge_candidate[NUM_SAO_MERGE_TYPES][NUM_SAO_COMPONENTS];
    SAOBlkParam temp_sao_param[NUM_SAO_COMPONENTS];
    sao_enc_t *sao = &h->sao_enc;
    int merge_avail[NUM_SAO_MERGE_TYPES];
    int mergeIdx, compIdx;
    double curdist = 0;
    int currate;
    int type;
    double curcost, mincost;
    aec_t cs_bakup;
    store_coding_state_hdr(&cs_bakup);

    mincost = MAX_COST;
    getMergeNeighbor(h, lcu_x, lcu_y, LCU_SIZE_IN_BITS, rec_saoBlkParam, merge_avail, merge_candidate);

    *MergeLeftAvail = merge_avail[SAO_MERGE_LEFT];
    *MergeUpAvail   = merge_avail[SAO_MERGE_ABOVE];

    for (mergeIdx = 0; mergeIdx < NUM_SAO_MERGE_TYPES; mergeIdx++) {
        if (merge_avail[mergeIdx]) {
            curdist = 0;
            cpy_sao_param_blk(temp_sao_param, merge_candidate[mergeIdx]);
            for (compIdx = 0; compIdx < NUM_SAO_COMPONENTS; compIdx++) {
                if (!sao->sao_on[compIdx]) {
                    continue;
                }
                type = merge_candidate[mergeIdx][compIdx].typeIdc;
                temp_sao_param[compIdx].mergeIdc = mergeIdx ? SAO_MERGE_MODE_ABOVE : SAO_MERGE_MODE_LEFT;

                if (type != -1) {
                    curdist += get_distortion(compIdx, type, saostatData, temp_sao_param) / sao_labmda;
                }
            }
            currate = write_sao_mergeflag(aec, *MergeLeftAvail, *MergeUpAvail, temp_sao_param);
            curcost = currate + curdist;
            if (curcost < mincost) {
                mincost = curcost;
                cpy_sao_param_blk(sao_cur_param, temp_sao_param);
            }
            reset_coding_state_hdr(&cs_bakup);
        }
    }
    return mincost;

}

int offset_estimation(int typeIdx, int classIdx, double lambda, i32s_t offset_ori, int count, i32s_t diff, double *bestCost)
{
    int cur_offset = offset_ori;
    int offset_best;
    int lower_bd, upper_bd, Th;
    int temp_offset, start_offset, end_offset;
    int temprate;
    i32s_t tempdist;
    double tempcost, mincost;
    const int *eo_offset_bins = &(tab_eo_offset_map[1]);
    int offset_type = classIdx;

    lower_bd = tab_saoclip[offset_type][0];
    upper_bd = tab_saoclip[offset_type][1];
    Th = tab_saoclip[offset_type][2];
    cur_offset = Clip3(lower_bd, upper_bd, cur_offset);

    assert(typeIdx >= SAO_TYPE_EO_0 && typeIdx <= SAO_TYPE_EO_45);
    switch (classIdx) {
    case SAO_CLASS_EO_FULL_VALLEY:
        start_offset = -1;
        end_offset = max(cur_offset, 1);
        break;
    case SAO_CLASS_EO_HALF_VALLEY:
        start_offset = 0;
        end_offset = 1;
        break;
    case SAO_CLASS_EO_HALF_PEAK:
        start_offset = -1;
        end_offset = 0;
        break;
    case SAO_CLASS_EO_FULL_PEAK:
        start_offset = min(cur_offset, -1);
        end_offset = 1;
        break;
    default:
        printf("Not a supported SAO mode\n");
        assert(0);
        exit(-1);
    }
 
    mincost = MAX_COST;
    offset_best = 0;

#define SAO_CHECK_COST { \
    temprate = (temprate == Th) ? temprate : (temprate + 1);  \
    tempdist = distortion_cal(count, temp_offset, diff);      \
    tempcost = (double)tempdist + lambda * (double)temprate;  \
    if (tempcost < mincost) {                                 \
        mincost = tempcost;                                   \
        offset_best = temp_offset;                            \
        *bestCost = tempcost;                                 \
    }                                                         \
}

    if (classIdx == SAO_CLASS_EO_HALF_VALLEY || classIdx == SAO_CLASS_EO_HALF_PEAK) {
        for (temp_offset = start_offset; temp_offset <= end_offset; temp_offset++) {
            temprate = abs(temp_offset);
            SAO_CHECK_COST;
        }
    } else {
        for (temp_offset = start_offset; temp_offset <= end_offset; temp_offset++) {
            temprate = eo_offset_bins[classIdx == SAO_CLASS_EO_FULL_VALLEY ? temp_offset : -temp_offset];
            SAO_CHECK_COST;
        }
    }
    return offset_best;
}


void find_offset(int compIdx, int typeIdc, SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES], SAOBlkParam *saoBlkParam, double lambda)
{
    int class_i;
    double classcost[MAX_NUM_SAO_CLASSES];
    double offth;
    int num_class = NUM_SAO_EO_CLASSES;

    for (class_i = 0; class_i < num_class; class_i++) {
        if (class_i == SAO_CLASS_EO_PLAIN) {
            saoBlkParam[compIdx].offset[class_i] = 0;
            continue;
        }
        if (saostatData[compIdx][typeIdc].count[class_i] == 0) {
            saoBlkParam[compIdx].offset[class_i] = 0; //offset will be zero
            continue;
        }
        offth = saostatData[compIdx][typeIdc].diff[class_i] > 0 ? 0.5 : (saostatData[compIdx][typeIdc].diff[class_i] < 0 ? -0.5 : 0);
        saoBlkParam[compIdx].offset[class_i] = (int)((double)saostatData[compIdx][typeIdc].diff[class_i] / (double)saostatData[compIdx][typeIdc].count[class_i] + offth);
    }

    assert(typeIdc >= SAO_TYPE_EO_0 && typeIdc <= SAO_TYPE_EO_45);
    for (class_i = 0; class_i < num_class; class_i++) {
        if (class_i == SAO_CLASS_EO_PLAIN) {
            saoBlkParam[compIdx].offset[class_i] = 0;
            classcost[class_i] = 0;
        }
        else {
            saoBlkParam[compIdx].offset[class_i] = offset_estimation(typeIdc, class_i, lambda, saoBlkParam[compIdx].offset[class_i], saostatData[compIdx][typeIdc].count[class_i], saostatData[compIdx][typeIdc].diff[class_i], &(classcost[class_i]));
        }
    }
    saoBlkParam[compIdx].startBand = 0;
}

double SAORDCost_for_mode_new_YUV_sep(avs3_enc_t *h, aec_t *aec, int MergeLeftAvail, int MergeUpAvail, double sao_lambda, 
    SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES], SAOBlkParam *sao_cur_param)
{
    int compIdx;
    int type;
    i32s_t mindist[NUM_SAO_COMPONENTS] = {0, 0, 0};
    int  minrate[NUM_SAO_COMPONENTS] = {0, 0, 0};
    double mincost;
    double normmodecost;
    SAOBlkParam temp_sao_param[NUM_SAO_COMPONENTS];
    sao_enc_t *sao = &h->sao_enc;
    int mergeflag_rate;
    aec_t cs_sao_best;
    aec_t cs_sao_type_bakup;

    mergeflag_rate = 0;

    if (MergeLeftAvail + MergeUpAvail) {
        sao_cur_param[SAO_Y].mergeIdc = SAO_MODE_NEW;
        mergeflag_rate = write_sao_mergeflag(aec, MergeLeftAvail, MergeUpAvail, &(sao_cur_param[SAO_Y]));
    }

    for (compIdx = 0; compIdx < NUM_SAO_COMPONENTS; compIdx++) {
        sao_cur_param[compIdx].typeIdc = -1;
        if (!sao->sao_on[compIdx]) {
            continue;
        }
        sao_cur_param[compIdx].mergeIdc = SAO_MODE_NEW;
        store_coding_state_hdr(&cs_sao_type_bakup);
		
        minrate[compIdx] = write_sao_mode(aec, &(sao_cur_param[compIdx]));
        mindist[compIdx] = 0;
        mincost = sao_lambda * ((double)minrate[compIdx]);
        store_coding_state_hdr(&cs_sao_best);
        reset_coding_state_hdr(&cs_sao_type_bakup);

        for (type = 0; type < NUM_SAO_NEW_TYPES; type++) {
            int dist, rate;
            double cost;
            temp_sao_param[compIdx].typeIdc = type;
            temp_sao_param[compIdx].mergeIdc = SAO_MODE_NEW;
            find_offset(compIdx, type, saostatData, temp_sao_param, sao_lambda);
            dist = get_distortion(compIdx, type, saostatData, temp_sao_param);

            rate = write_sao_mode(aec, &temp_sao_param[compIdx]);
            rate += write_sao_offset(aec, &temp_sao_param[compIdx]);
            rate += write_sao_type(aec, &temp_sao_param[compIdx]);

            cost = dist + sao_lambda * rate;

            if (cost < mincost) {
                mincost = cost;
                minrate[compIdx] = rate;
                mindist[compIdx] = dist;
                sao_cur_param[compIdx] = temp_sao_param[compIdx];
                store_coding_state_hdr(&cs_sao_best);
            }
            reset_coding_state_hdr(&cs_sao_type_bakup);
        }
        reset_coding_state_hdr(&cs_sao_best);
    }

    normmodecost = (double)(mindist[SAO_Y] + mindist[SAO_Cb] + mindist[SAO_Cr]) / sao_lambda;
    normmodecost += minrate[SAO_Y] + minrate[SAO_Cb] + minrate[SAO_Cr] + mergeflag_rate;
    return normmodecost;
}


void sao_lcu_rd_decision(avs3_enc_t *h, int lcu_x, int lcu_y, aec_t *aec, SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES], SAOBlkParam *saoBlkParam, double sao_labmda)
{
    int MergeLeftAvail = 0, MergeUpAvail = 0;
    double mcost, mincost;
    SAOBlkParam sao_cur_param[NUM_SAO_COMPONENTS];
    sao_enc_t *sao_enc = &h->sao_enc;
    aec_t cs_sao_bakup;

    store_coding_state_hdr(&cs_sao_bakup);

    mincost = SAORDCost_for_mode_merge(h, lcu_x, lcu_y, aec, sao_labmda, saostatData, sao_enc->saoBlkParams, saoBlkParam, &MergeLeftAvail, &MergeUpAvail);
    mcost   = SAORDCost_for_mode_new_YUV_sep(h, aec, MergeLeftAvail, MergeUpAvail, sao_labmda, saostatData, sao_cur_param);

    if (mcost < mincost) {
        mincost = mcost;
        cpy_sao_param_blk(saoBlkParam, sao_cur_param);
    }
    reset_coding_state_hdr(&cs_sao_bakup);
}


void sao_lcu_write(avs3_enc_t *h, int lcu_x, int lcu_y, aec_t *aec, SAOBlkParam *sao_cur_param)
{
    sao_enc_t *sao = &h->sao_enc;
    SAOBlkParam merge_candidate[NUM_SAO_MERGE_TYPES][NUM_SAO_COMPONENTS];
    int merge_avail[NUM_SAO_MERGE_TYPES];
    int MergeLeftAvail, MergeUpAvail;

    int compIdx;
    getMergeNeighbor(h, lcu_x, lcu_y, LCU_SIZE_IN_BITS, sao->saoBlkParams, merge_avail, merge_candidate);

    MergeLeftAvail = merge_avail[0];
    MergeUpAvail = merge_avail[1];

    if (MergeLeftAvail + MergeUpAvail) {
        write_sao_mergeflag(aec, MergeLeftAvail, MergeUpAvail, &(sao_cur_param[SAO_Y]));
    }

    if (sao_cur_param[SAO_Y].mergeIdc == SAO_MODE_NEW) {
        for (compIdx = 0; compIdx < NUM_SAO_COMPONENTS; compIdx++) {
            if (!sao->sao_on[compIdx]) {
                continue;
            }
            write_sao_mode(aec, &sao_cur_param[compIdx]);
            if (sao_cur_param[compIdx].typeIdc != -1) {
                write_sao_offset(aec, &sao_cur_param[compIdx]);
                write_sao_type(aec, &sao_cur_param[compIdx]);
            }
        }
    }
}

void deblock_one_lcu(avs3_enc_t *h, image_t *img, int lcu_x, int lcu_y)
{
    int i, j;
    const cfg_param_t *input = h->input;
    int b8x = lcu_x << (LCU_SIZE_IN_BITS - 3);
    int b8y = lcu_y << (LCU_SIZE_IN_BITS - 3);
    int lcu_width  = lcu_x == input->pic_width_in_lcu  - 1 ? input->img_width  - lcu_x * LCU_SIZE : LCU_SIZE;
    int lcu_height = lcu_y == input->pic_height_in_lcu - 1 ? input->img_height - lcu_y * LCU_SIZE : LCU_SIZE;
    int b8x_end = b8x + (lcu_width  >> 3);
    int b8y_end = b8y + (lcu_height >> 3);
    int b8w = input->pic_width_in_mcu;
    int slice_border = 0;

    if (!LOOPFLT_CROSS_SLICE && h->slice_nr[lcu_y] != h->slice_nr[lcu_y - 1]) {
        slice_border = 1;
    }

    for (j = b8y; j < b8y_end; j++) {
        int do_hor = !slice_border || j > b8y;

        if (b8x) {
            DeblockMb(h, img, j, b8x,     0);
            if (do_hor) {
                DeblockMb(h, img, j, b8x - 1, 1);
            }
        }

        for (i = b8x; i < b8x_end - 1; i++) {
            DeblockMb(h, img, j, i + 1, 0);
            if (do_hor) {
                DeblockMb(h, img, j, i, 1);
            }
        }

        if (b8x_end == b8w && do_hor) {
            DeblockMb(h, img, j, b8x_end - 1, 1);
        }
    }
}

void com_funs_init_deblock()
{
    g_funs_handle.deblk_luma[0] = deblock_luma_ver;
    g_funs_handle.deblk_luma[1] = deblock_luma_hor;
    g_funs_handle.deblk_chroma[0] = deblock_chroma_ver;
    g_funs_handle.deblk_chroma[1] = deblock_chroma_hor;
}

void com_funs_init_sao(){
    g_funs_handle.sao_flt[0] = getStatblk;
    g_funs_handle.sao_flt[1] = SAO_on_block;
}