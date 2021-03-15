#include <assert.h>

#include "inter-prediction.h"

ALIGNED_8(tab_char_t if_coef_luma[3][8]) = {
    { -1, 4, -10, 57, 19,  -7, 3, -1 },
    { -1, 4, -11, 40, 40, -11, 4, -1 },
    { -1, 3,  -7, 19, 57, -10, 4, -1 }
};

ALIGNED_8(tab_char_t if_coef_chroma[9][4]) = {
    {  0, 64,  0,  0 }, // for full-pixel, no use
    { -4, 62,  6,  0 },
    { -6, 56, 15, -1 },
    { -5, 47, 25, -3 },
    { -4, 36, 36, -4 },
    { -3, 25, 47, -5 },
    { -1, 15, 56, -6 },
    {  0,  6, 62, -4 },
    {  0,  0,  0,  0 }, // padding buffer
};

void get_col_bskip_mv(avs3_enc_t *h, int uiBitSize)
{
    const cfg_param_t *input = h->input;
    analyzer_t *a = &h->analyzer;
    me_info_t *me = &a->me_info;
    int  b8x = h->cu_pix_x >> 3;
    int  b8y = h->cu_pix_y >> 3;

    int num_mb_inblock = 1 << (uiBitSize - 3);   //qyu 0820 add 4:1 5:2 6:4

    int  refframe; 
    frame_t *col_frm = h->ref_list[0];

    int b8_strie = input->b8_stride;

    int col_x, col_y;
    i16s_t *fmv = me->allFwMv[0][0][0][0];
    i16s_t *bmv = me->allBwMv[0][0][0][0];

    refine_col_pos(h, b8x, b8y, &col_x, &col_y);

    refframe = col_frm->refbuf[col_y * b8_strie + col_x];

    if (refframe == -1) {
        init_pu_inter_pos(h, 0, 1, uiBitSize, 0);
        get_predicted_mv(h, fmv, 0,  0, NULL, NULL);
        get_predicted_mv(h, bmv, 0, -1, NULL, NULL);
    } else {
        i16s_t  *col_mv = col_frm->mvbuf[col_y * b8_strie + col_x];

        int dist_col  = (h->ref_list[0]->poc - h->ref_list[0]->ref_poc[refframe] + 256) % 256;
        int dist_l1  = (int)(h->poc - h->ref_list[0]->poc);
        int dist_l0  = (int)(h->poc - h->ref_list[1]->poc);

        scaling_mv(dist_l0, dist_col, col_mv, fmv);
        scaling_mv(dist_l1, dist_col, col_mv, bmv);
    }
}

void get_col_pskip_mv(avs3_enc_t *h, int uiBitSize)
{
    me_info_t *me = &h->analyzer.me_info;
    const cfg_param_t *input = h->input;
    int  b8x = h->cu_pix_x >> 3;
    int  b8y = h->cu_pix_y >> 3;

    int b8w = 1 << (uiBitSize - 3);
    int b8h = 1 << (uiBitSize - 3);

    frame_t *col_frm = h->ref_list[0];
    i64s_t *col_pic_dist = col_frm->ref_poc;

    int refframe;
    int curT, colT;
    int b8_stride = input->b8_stride;

    int col_x, col_y;
    i16s_t *fmv = me->allFwMv[0][0][0][0];

    refine_col_pos(h, b8x, b8y, &col_x, &col_y);
    refframe = col_frm->refbuf[col_y * b8_stride + col_x];

    if (refframe >= 0) {
        i16s_t *col_mv = col_frm->mvbuf[col_y * b8_stride + col_x];
        curT = (int)((h->poc - h->ref_list[0]->poc));
        colT = (int)((h->ref_list[0]->poc - col_pic_dist[refframe]));
        scaling_mv(curT, colT, col_mv, fmv);
    } else {
        fmv[0] = 0;
        fmv[1] = 0;
    }
}

void get_mhp_bskip_mv(avs3_enc_t *h, int uiBitSize)
{
    me_info_t *me = &h->analyzer.me_info;
    const cfg_param_t *input = h->input;
    int b8x = h->cu_pix_x >> 3;
    int b8y = h->cu_pix_y >> 3;
    int b8size = 1 << (uiBitSize - 3);
	int b8_stride = input->b8_stride;
	int offset = b8y * b8_stride + b8x;
	char_t  *fw_refFrArr = h->p_cur_frm->refbuf + offset;
	char_t  *bw_refFrArr = h->refbuf_bw + offset;
	char_t  *pdir = h->pdir + offset;
	i16s_t(*tmp_fwmv)[2] = h->p_cur_frm->mvbuf + offset;
	i16s_t(*tmp_bwmv)[2] = h->mvbuf_bw + offset;
    int hv;
    int neighbor[6];
    int pdir_modes[6];
    i16s_t pmv[2][2][6];
    int i, j, dir;
    int L0_motion_found = 0, L1_motion_found = 0, BI_motion_found = 0;
    int last_bi_idx = 0;

    neighbor[0] = (b8size - 1) * b8_stride - 1;
    pdir_modes[0] = pdir[neighbor[0]];

    neighbor[1] = - b8_stride + b8size - 1;
    pdir_modes[1] = pdir[neighbor[1]];

    neighbor[2] = - b8_stride + b8size;
    pdir_modes[2] = pdir[neighbor[2]];

    neighbor[3] = - 1;
    pdir_modes[3] = pdir[neighbor[3]];

    neighbor[4] = - b8_stride;
    pdir_modes[4] = pdir[neighbor[4]];

    neighbor[5] = - b8_stride - 1;
    pdir_modes[5] = pdir[neighbor[5]];

    for (hv = 0; hv < 2; hv++) {
        for (int i = 0; i < 6; i++) {
            pmv[0][hv][i] = tmp_bwmv[neighbor[i]][hv];
            pmv[1][hv][i] = tmp_fwmv[neighbor[i]][hv];
        }
    }
    for (i = 0; i < 2; i++) {
        for (dir = 0; dir < 5; dir++) {
            me->skip_bw_mv[dir][i] = 0;
            me->skip_fw_mv[dir][i] = 0;
        }
    }
    for (j = 0; j < 6; j++) {
        if (pdir_modes[j] == BID) {
            if (!BI_motion_found) {
                me->skip_bw_mv[DS_BID][0] = pmv[0][0][j];
                me->skip_bw_mv[DS_BID][1] = pmv[0][1][j];
                me->skip_fw_mv[DS_BID][0] = pmv[1][0][j];
                me->skip_fw_mv[DS_BID][1] = pmv[1][1][j];
            }
            BI_motion_found = 1;
            last_bi_idx = j;
        }
        else if (pdir_modes[j] == BACKWARD && !L1_motion_found) {
            L1_motion_found = 1;
            me->skip_bw_mv[DS_BACKWARD][0] = pmv[0][0][j];
            me->skip_bw_mv[DS_BACKWARD][1] = pmv[0][1][j];
        }
        else if (pdir_modes[j] == FORWARD && !L0_motion_found) {
            L0_motion_found = 1;
            me->skip_fw_mv[DS_FORWARD][0] = pmv[1][0][j];
            me->skip_fw_mv[DS_FORWARD][1] = pmv[1][1][j];
        }
    }

    if (L0_motion_found && L1_motion_found && !BI_motion_found) {
        me->skip_bw_mv[DS_BID][0] = me->skip_bw_mv[DS_BACKWARD][0];
        me->skip_bw_mv[DS_BID][1] = me->skip_bw_mv[DS_BACKWARD][1];
        me->skip_fw_mv[DS_BID][0] = me->skip_fw_mv[DS_FORWARD][0];
        me->skip_fw_mv[DS_BID][1] = me->skip_fw_mv[DS_FORWARD][1];
    }
    if (!L1_motion_found && BI_motion_found) {
        me->skip_bw_mv[DS_BACKWARD][0] = pmv[0][0][last_bi_idx];
        me->skip_bw_mv[DS_BACKWARD][1] = pmv[0][1][last_bi_idx];
    }
    if (!L0_motion_found && BI_motion_found) {
        me->skip_fw_mv[DS_FORWARD][0] = pmv[1][0][last_bi_idx];
        me->skip_fw_mv[DS_FORWARD][1] = pmv[1][1][last_bi_idx];
    }
}

void get_predicted_mv(avs3_enc_t *h, i16s_t *pmv, int ref_frame, int ref, int *neighbor_ref, i16s_t (*neighbor_mv)[2])
{
    char_t *ref_buf = ref >= 0 ? h->p_cur_frm->refbuf : h->refbuf_bw;
    i16s_t(*mv_buf)[2] = ref >= 0 ? h->p_cur_frm->mvbuf : h->mvbuf_bw;
    const cfg_param_t *input = h->input;
    int  blockshape_x = h->pu_size;
    int  blockshape_y = h->pu_size;
    int mb_pix_x = h->pu_pix_x - h->cu_pix_x;
    int mb_pix_y = h->pu_pix_y - h->cu_pix_y;
    int bfrm_fw = (h->type == B_FRM && ref >= 0) ? 1 : 0;
    int b8x = h->pu_b8_x;
    int b8y = h->pu_b8_y;
    i16s_t pred_vec = 0;
    int mvPredType, rFrameL, rFrameU, rFrameUR;
    int hv;
    ALIGNED_32(i16s_t tmva[2]);
    ALIGNED_32(i16s_t tmvb[2]);
    ALIGNED_32(i16s_t tmvc[2]);
    int b8_stride = input->b8_stride;
    int mult_distance = h->ref_dist[ref_frame + bfrm_fw];
    int devide_distance;

    mvPredType = MVPRED_xy_MIN;

    ref_buf += b8y * b8_stride + b8x;

    rFrameL  = ref_buf[-1];
    rFrameU  = ref_buf[-b8_stride];
    rFrameUR = ref_buf[-b8_stride + (blockshape_x >> 3)];

    mv_buf += b8y * b8_stride + b8x;

    if (rFrameL != -1) {
        devide_distance = h->ref_dist[rFrameL + bfrm_fw];
        scaling_mv(mult_distance, devide_distance, mv_buf[-1], tmva);
    } else {
        tmva[0] = tmva[1] = 0;
    }
    if (rFrameU != -1) {
        devide_distance = h->ref_dist[rFrameU + bfrm_fw];
        scaling_mv(mult_distance, devide_distance, mv_buf[-b8_stride], tmvb);
    } else {
        tmvb[0] = tmvb[1] = 0;
    }
    if (rFrameUR != -1) {
        devide_distance = h->ref_dist[rFrameUR + bfrm_fw];
        scaling_mv(mult_distance, devide_distance, mv_buf[-b8_stride + (blockshape_x >> 3)], tmvc);
    } else {
        rFrameUR = ref_buf[-b8_stride - 1];

        if (rFrameUR != -1) {
            devide_distance = h->ref_dist[rFrameUR + bfrm_fw];
            scaling_mv(mult_distance, devide_distance, mv_buf[-b8_stride - 1], tmvc);
        } else {
            tmvc[0] = tmvc[1] = 0;
        }
    }

    if ((rFrameL != -1) && (rFrameU == -1) && (rFrameUR == -1)) {
        mvPredType = MVPRED_L;
    } else if ((rFrameL == -1) && (rFrameU != -1) && (rFrameUR == -1)) {
        mvPredType = MVPRED_U;
    } else if ((rFrameL == -1) && (rFrameU == -1) && (rFrameUR != -1)) {
        mvPredType = MVPRED_UR;
    } else if (blockshape_x < blockshape_y) {
        if (mb_pix_x == 0) {
            if (rFrameL == ref_frame) {
                mvPredType = MVPRED_L;
            }
        } else {
            if (rFrameUR == ref_frame) {
                mvPredType = MVPRED_UR;
            }
        }
    } else if (blockshape_x > blockshape_y) {
        if (mb_pix_y == 0) {
            if (rFrameU == ref_frame) {
                mvPredType = MVPRED_U;
            }
        } else {
            if (rFrameL == ref_frame) {
                mvPredType = MVPRED_L;
            }
        }
    }
    
    switch (mvPredType) {
    case MVPRED_xy_MIN:
        for (hv = 0; hv < 2; hv++) {
            int mva = tmva[hv], mvb = tmvb[hv], mvc = tmvc[hv];
            if (mva == 0 || mvb == 0 || mvc == 0 || ((mvb ^ mvc) >= 0 && (mva ^ mvb) >= 0)) {
                i16s_t mva_ext = (i16s_t)abs(mva - mvb);
                i16s_t mvb_ext = (i16s_t)abs(mvb - mvc);
                i16s_t mvc_ext = (i16s_t)abs(mvc - mva);

                pred_vec = min(mva_ext, min(mvb_ext, mvc_ext));

                if (pred_vec == mva_ext) {
                    pmv[hv] = (mva + mvb) / 2;
                } else if (pred_vec == mvb_ext) {
                    pmv[hv] = (mvb + mvc) / 2;
                } else {
                    pmv[hv] = (mvc + mva) / 2;
                }
            } else if ((mvb ^ mvc) >= 0 && (mva ^ mvb) < 0) {
                pmv[hv] = (mvb + mvc) / 2;
            } else if ((mva ^ mvc) >= 0 && (mva ^ mvb) < 0) {
                pmv[hv] = (mvc + mva) / 2;
            } else {
                pmv[hv] = (mva + mvb) / 2;
            }
        }
        break;
    case MVPRED_L:
        M32(pmv) = M32(tmva);
        break;
    case MVPRED_U:
        M32(pmv) = M32(tmvb);
        break;
    case MVPRED_UR:
        M32(pmv) = M32(tmvc);
        break;
    default:
        assert(0);
        break;
    }

    if (neighbor_ref) {
        neighbor_ref[0] = rFrameL;
        neighbor_ref[1] = rFrameU;
        neighbor_ref[2] = rFrameUR;
    }
    if (neighbor_mv) {
        M32(neighbor_mv[0]) = M32(tmva);
        M32(neighbor_mv[1]) = M32(tmvb);
        M32(neighbor_mv[2]) = M32(tmvc);
    }
}


void refine_col_pos(avs3_enc_t *h, int x, int y, int *real_x, int *real_y)
{
    int xPos, yPos;
    const cfg_param_t *input = h->input;
    int b8w = input->pic_width_in_mcu;
    int b8h = input->pic_height_in_mcu;

    yPos = ((y >> 1) << 1) + 1;
    xPos = ((x >> 1) << 1) + 1;

    if (yPos >= b8h) {
        yPos = (((y >> 1) << 1) + b8h) >> 1;
    }
    if (xPos >= b8w) {
        xPos = (((x >> 1) << 1) + b8w) >> 1;
    }
    *real_x = xPos;
    *real_y = yPos;
}

#define FLT_8TAP_HOR(src, i, coef) ( \
    (src)[i-3] * (coef)[0] + \
    (src)[i-2] * (coef)[1] + \
    (src)[i-1] * (coef)[2] + \
    (src)[i  ] * (coef)[3] + \
    (src)[i+1] * (coef)[4] + \
    (src)[i+2] * (coef)[5] + \
    (src)[i+3] * (coef)[6] + \
    (src)[i+4] * (coef)[7])

#define FLT_8TAP_VER(src, i, i_src, coef) ( \
    (src)[i-3 * i_src] * (coef)[0] + \
    (src)[i-2 * i_src] * (coef)[1] + \
    (src)[i-1 * i_src] * (coef)[2] + \
    (src)[i          ] * (coef)[3] + \
    (src)[i+1 * i_src] * (coef)[4] + \
    (src)[i+2 * i_src] * (coef)[5] + \
    (src)[i+3 * i_src] * (coef)[6] + \
    (src)[i+4 * i_src] * (coef)[7])

#define FLT_4TAP_HOR(src, i, coef) ( \
    (src)[i - 1] * (coef)[0] + \
    (src)[i    ] * (coef)[1] + \
    (src)[i + 1] * (coef)[2] + \
    (src)[i + 2] * (coef)[3])

#define FLT_4TAP_VER(src, i, i_src, coef) ( \
    (src)[i-1 * i_src] * (coef)[0] + \
    (src)[i          ] * (coef)[1] + \
    (src)[i+1 * i_src] * (coef)[2] + \
    (src)[i+2 * i_src] * (coef)[3])

static void com_if_filter_hor_4(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int bit_depth)
{
    int row, col;
    int sum, val;
    int max_pixel = (1 << bit_depth) - 1;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            sum = FLT_4TAP_HOR(src, col, coeff);
            val = (sum + 32) >> 6;
            dst[col] = avs3_pixel_clip(val, max_pixel);
        }
        src += i_src;
        dst += i_dst;
    }
}

static void com_if_filter_ver_4(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int bit_depth)
{
    int row, col;
    int sum, val;
    int max_pixel = (1 << bit_depth) - 1;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            sum = FLT_4TAP_VER(src, col, i_src, coeff);
            val = (sum + 32) >> 6;
            dst[col] = avs3_pixel_clip(val, max_pixel);
        }
        src += i_src;
        dst += i_dst;
    }
}

static void com_if_filter_hor_ver_4(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff_h, const char_t *coeff_v, int bit_depth)
{
    int row, col;
    int sum, val;
    int shift1 = bit_depth - 8;
    int shift2 = 20 - bit_depth;
    int add1 = (1 << (shift1)) >> 1;
    int add2 = 1 << (shift2 - 1);
    i16s_t tmp_res[(32 + 3) * 32];
    i16s_t *tmp;
    int max_pixel = (1 << bit_depth) - 1;

    src += -1 * i_src;
    tmp = tmp_res;

    for (row = -1; row < height + 2; row++) {
        for (col = 0; col < width; col++) {
            val = FLT_4TAP_HOR(src, col, coeff_h);
            tmp[col] = (val + add1) >> shift1;
        }
        src += i_src;
        tmp += 32;
    }

    tmp = tmp_res + 1 * 32;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            sum = FLT_4TAP_VER(tmp, col, 32, coeff_v);
            val = (sum + add2) >> shift2;
            dst[col] = avs3_pixel_clip(val, max_pixel);
        }
        dst += i_dst;
        tmp += 32;
    }
}

static void com_if_filter_hor_8(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    i16s_t *dt0 = dst_tmp[0];
    i16s_t *dt1 = dst_tmp[1];
    i16s_t *dt2 = dst_tmp[2];
    int shift_tmp = bit_depth - 8;
    int add_tmp = (1 << (shift_tmp)) >> 1;
    int t1, t2, t3;
    int max_pixel = (1 << bit_depth) - 1;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            t1 = FLT_8TAP_HOR(src, i, coeff[0]);
            dt0[i] = (t1 + add_tmp) >> shift_tmp;
            d0 [i] = avs3_pixel_clip((t1 + 32) >> 6, max_pixel);
            t2 = FLT_8TAP_HOR(src, i, coeff[1]);
            dt1[i] = (t2 + add_tmp) >> shift_tmp;
            d1 [i] = avs3_pixel_clip((t2 + 32) >> 6, max_pixel);
            t3 = FLT_8TAP_HOR(src, i, coeff[2]);
            dt2[i] = (t3 + add_tmp) >> shift_tmp;
            d2 [i] = avs3_pixel_clip((t3 + 32) >> 6, max_pixel);
        }
        d0  += i_dst;
        d1  += i_dst;
        d2  += i_dst;
        dt0 += i_dst_tmp;
        dt1 += i_dst_tmp;
        dt2 += i_dst_tmp;
        src += i_src;
    }
}

static void com_if_filter_ver_8(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j, val;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            val = (FLT_8TAP_VER(src, i, i_src, coeff[0]) + 32) >> 6;
            d0[i] = avs3_pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[1]) + 32) >> 6;
            d1[i] = avs3_pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[2]) + 32) >> 6;
            d2[i] = avs3_pixel_clip(val, max_pixel);
        }

        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }
}

static void com_if_filter_ver_8_ext(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j, val;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;
    int shift1 = 20 - bit_depth;
    int add1 = 1 << (shift1 - 1);

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            val = (FLT_8TAP_VER(src, i, i_src, coeff[0]) + add1) >> shift1;
            d0[i] = avs3_pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[1]) + add1) >> shift1;
            d1[i] = avs3_pixel_clip(val, max_pixel);

            val = (FLT_8TAP_VER(src, i, i_src, coeff[2]) + add1) >> shift1;
            d2[i] = avs3_pixel_clip(val, max_pixel);
        }

        d0  += i_dst;
        d1  += i_dst;
        d2  += i_dst;
        src += i_src;
    }
}

void com_if_luma_frame(avs3_enc_t *h, image_t img_list[4][4])
{
    int dx, dy;
    const cfg_param_t *input = h->input;
    int i_stride = img_list[0][0].i_stride[0];
    int tmp_stride = input->img_width + 16;
    pel_t *dst;
    pel_t *dst_tmp[3];

    int ip_width = input->img_width + 8;
    int ip_height = input->img_height + 8;

    //horizontal positions: a,1,b
    dst_tmp[0] = img_list[0][1].plane[0] - 8 * i_stride - 8;
    dst_tmp[1] = img_list[0][2].plane[0] - 8 * i_stride - 8;
    dst_tmp[2] = img_list[0][3].plane[0] - 8 * i_stride - 8;
    g_funs_handle.ip_flt_y_hor(img_list[0][0].plane[0] - 8 * i_stride - 8, i_stride, dst_tmp, i_stride, h->tmp_buf, tmp_stride, input->img_width + 16, input->img_height + 16, if_coef_luma, input->bit_depth);

    //vertical positions: c,2,j
    dst_tmp[0] = img_list[1][0].plane[0] - 4 * i_stride - 4;
    dst_tmp[1] = img_list[2][0].plane[0] - 4 * i_stride - 4;
    dst_tmp[2] = img_list[3][0].plane[0] - 4 * i_stride - 4;
    g_funs_handle.ip_flt_y_ver(img_list[0][0].plane[0] - 4 * i_stride - 4, i_stride, dst_tmp, i_stride, ip_width, ip_height, if_coef_luma, input->bit_depth);

    //vertical positions: d,h,k; e,3,1; f,i,m
    for (dx = 1; dx < 4; dx++) {
        dst_tmp[0] = img_list[1][dx].plane[0] - 4 * i_stride - 4;
        dst_tmp[1] = img_list[2][dx].plane[0] - 4 * i_stride - 4;
        dst_tmp[2] = img_list[3][dx].plane[0] - 4 * i_stride - 4;
        g_funs_handle.ip_flt_y_ver_ext(h->tmp_buf[dx - 1] + 4 * tmp_stride + 4, tmp_stride, dst_tmp, i_stride, ip_width, ip_height, if_coef_luma, input->bit_depth);
    }

    for (dx = 0; dx < 4; dx++) {
        for (dy = 0; dy < 4; dy++) {
            if (dx != 0 || dy != 0) {
                dst = img_list[dy][dx].plane[0] - 4 * i_stride - 4;
                g_funs_handle.padding_rows(dst, i_stride, ip_width, ip_height, 0, ip_height, IMG_PAD_SIZE - 4);
            }
        }
    }
}

int com_if_luma_lcu_row(avs3_enc_t *h, image_t img_list[4][4], int lcu_y, uavs3e_sem_t *sem_up, uavs3e_sem_t *sem_curr)
{
    const cfg_param_t *input = h->input;
  
    const int lcu_pix_y = lcu_y * LCU_SIZE;
    const int b_last_row = (lcu_y == h->input->pic_height_in_lcu - 1);
    const int ip_width = input->img_width + 8;
    const int ip_height = input->img_height + 8;
    const int start = lcu_pix_y ? lcu_pix_y - 4 : 0;
    const int end   = b_last_row ? lcu_pix_y + LCU_SIZE : lcu_pix_y + LCU_SIZE - 4;

    int i_stride = img_list[0][0].i_stride[0];
    int tmp_stride = input->img_width + 2 * 8;

    int dx, s, e;
    pel_t *dst_tmp[3];
    i16s_t *tbuf[3];

    s = start ? start : -8;
    e = b_last_row ? input->img_height + 8 : end;
    tbuf[0] = h->tmp_buf[0] + (s + 8) * tmp_stride;
    tbuf[1] = h->tmp_buf[1] + (s + 8) * tmp_stride;
    tbuf[2] = h->tmp_buf[2] + (s + 8) * tmp_stride;
    dst_tmp[0] = img_list[0][1].plane[0] + s * i_stride - 4;
    dst_tmp[1] = img_list[0][2].plane[0] + s * i_stride - 4;
    dst_tmp[2] = img_list[0][3].plane[0] + s * i_stride - 4;

    g_funs_handle.ip_flt_y_hor(img_list[0][0].plane[0] + s * i_stride - 4, i_stride, dst_tmp, i_stride, tbuf, tmp_stride, ip_width, e - s, if_coef_luma, input->bit_depth);
    safe_sem_post(sem_curr);
    safe_sem_wait(sem_up);

    s = start ? start : -4;
    e = b_last_row ? input->img_height + 4 : end;
    g_funs_handle.padding_rows(img_list[0][1].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);
    g_funs_handle.padding_rows(img_list[0][2].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);
    g_funs_handle.padding_rows(img_list[0][3].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);

    s = start - 4;
    e = b_last_row ? input->img_height + 4 : end - 4;
    for (dx = 1; dx < 4; dx++) {
        dst_tmp[0] = img_list[1][dx].plane[0] + s * i_stride - 4;
        dst_tmp[1] = img_list[2][dx].plane[0] + s * i_stride - 4;
        dst_tmp[2] = img_list[3][dx].plane[0] + s * i_stride - 4;
        g_funs_handle.ip_flt_y_ver_ext(h->tmp_buf[dx - 1] + (s + 8) * tmp_stride, tmp_stride, dst_tmp, i_stride, ip_width, e - s, if_coef_luma, input->bit_depth);
        g_funs_handle.padding_rows(img_list[1][dx].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);
        g_funs_handle.padding_rows(img_list[2][dx].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);
        g_funs_handle.padding_rows(img_list[3][dx].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);
    }

    s = start - 4;
    e = b_last_row ? input->img_height + 4 : end - 4;
    dst_tmp[0] = img_list[1][0].plane[0] + s * i_stride - 4;
    dst_tmp[1] = img_list[2][0].plane[0] + s * i_stride - 4;
    dst_tmp[2] = img_list[3][0].plane[0] + s * i_stride - 4;

    g_funs_handle.ip_flt_y_ver(img_list[0][0].plane[0] + s * i_stride - 4, i_stride, dst_tmp, i_stride, ip_width, e - s, if_coef_luma, input->bit_depth);
    g_funs_handle.padding_rows(img_list[1][0].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);
    g_funs_handle.padding_rows(img_list[2][0].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);
    g_funs_handle.padding_rows(img_list[3][0].plane[0] - 4 * i_stride - 4, i_stride, ip_width, ip_height, s + 4, e - s, IMG_PAD_SIZE - 4);

    return b_last_row ? input->img_height : lcu_pix_y + LCU_SIZE - 8;
}


void com_funs_init_ip_filter()
{
    int i;

    g_funs_handle.ip_flt_y_hor = com_if_filter_hor_8;
    g_funs_handle.ip_flt_y_ver = com_if_filter_ver_8;
    g_funs_handle.ip_flt_y_ver_ext = com_if_filter_ver_8_ext;

    for (i = 0; i < 16; i++) {
        g_funs_handle.ip_flt_c[IP_FLT_C_H][i] = com_if_filter_hor_4;
        g_funs_handle.ip_flt_c[IP_FLT_C_V][i] = com_if_filter_ver_4;
        g_funs_handle.ip_flt_c_ext[i] = com_if_filter_hor_ver_4;
    }
}