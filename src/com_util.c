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

#include "com_util.h"

funs_handle_t uavs3e_funs_handle;

void *uavs3e_align_malloc(int i_size)
{
    int mask = ALIGN_BASIC - 1;
    u8 *align_buf = NULL;
    u8 *buf = (u8 *)malloc(i_size + mask + sizeof(void **));

    if (buf) {
        align_buf = buf + mask + sizeof(void **);
        align_buf -= (intptr_t)align_buf & mask;
        *(((void **)align_buf) - 1) = buf;
        memset(align_buf, 0, i_size);
    }
    return align_buf;
}

void uavs3e_align_free(void *p)
{
    free(*(((void **)p) - 1));
}

com_img_t *com_img_create(int width, int height, int pad[MAX_PLANES], int planes)
{
    const int scale = sizeof(pel);
    com_img_t *img;
    img = (com_img_t *)com_malloc(sizeof(com_img_t));
    com_assert_rv(img, NULL);
    com_mset(img, 0, sizeof(com_img_t));

    for (int i = 0; i < planes; i++) {
        int p_size = (pad != NULL) ? pad[i] : 0;

        img->width [i] = width;
        img->height[i] = height;
        img->pad   [i] = p_size;
        img->stride[i] = (img->width[i] + p_size * 2) * scale;

        img->buf   [i] = com_malloc((img->height[i] + p_size * 2) * img->stride[i]);
        img->planes[i] = ((u8 *)img->buf[i]) + p_size * img->stride[i] + p_size * scale;

        if (i == 0) {
            width  = (width  + 1) >> 1;
            height = (height + 1) >> 1;
        }
    }
    img->num_planes = planes;

    com_img_addref(img);

    return img;
}

void com_img_free(com_img_t *img)
{
    int i;
    com_assert_r(img);
    for (i = 0; i < img->num_planes; i++) {
        if (img->buf[i]) {
            com_mfree(img->buf[i]);
        }
    }
    com_mfree(img);
}

static void picture_padding(pel *a, int s, int width, int height, int exp_hor, int exp_upper, int exp_below)
{
    int i, j;
    pel pixel;
    pel *src, *dst;
    /* left */
    src = a;
    dst = a - exp_hor;
    for (i = 0; i < height; i++) {
        pixel = *src; /* get boundary pixel */
        for (j = 0; j < exp_hor; j++) {
            dst[j] = pixel;
        }
        dst += s;
        src += s;
    }
    /* right */
    src = a + (width - 1);
    dst = a + width;
    for (i = 0; i < height; i++) {
        pixel = *src; /* get boundary pixel */
        for (j = 0; j < exp_hor; j++) {
            dst[j] = pixel;
        }
        dst += s;
        src += s;
    }
    /* upper */
    src = a - exp_upper;
    dst = a - exp_upper - (exp_upper * s);
    for (i = 0; i < exp_upper; i++) {
        com_mcpy(dst, src, s * sizeof(pel));
        dst += s;
    }
    /* below */
    src = a + ((height - 1) * s) - exp_below;
    dst = a + ((height - 1) * s) - exp_below + s;
    for (i = 0; i < exp_below; i++) {
        com_mcpy(dst, src, s * sizeof(pel));
        dst += s;
    }
}

void com_img_padding(com_img_t *img, int planes, int ext_size)
{
    for (int i = 0; i < planes; i++) {
        int stride = img->stride[i] / sizeof(pel);
        pel *p = (pel*)img->planes[i] - ext_size * stride - ext_size;

        picture_padding(p, stride,
            img->width [i] + ext_size * 2, 
            img->height[i] + ext_size * 2,
            img->pad   [i] - ext_size,
            img->pad   [i] - ext_size,
            img->pad   [i] - ext_size);

        if (i == 0) {
            ext_size >>= 1;
        }
    }
}

com_pic_t *com_pic_create(int width, int height, int pad_l, int pad_c, int *err)
{
    com_pic_t *pic = NULL;
    com_img_t *img = NULL;
    int ret, pad[MAX_PLANES];
    int i_scu, h_scu, f_scu, size;
    /* allocate PIC structure */
    pic = com_malloc(sizeof(com_pic_t));
    com_assert_gv(pic != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_mset(pic, 0, sizeof(com_pic_t));

    /* set padding value*/
    pad[0] = pad_l;
    pad[1] = pad_c;
    pad[2] = pad_c;
    img = com_img_create(width, height, pad, 3);
    com_assert_gv(img != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);

    /* set com_pic_t */
    pic->y               = img->planes[0];
    pic->u               = img->planes[1];
    pic->v               = img->planes[2];
    pic->width_luma      = img->width [0];
    pic->height_luma     = img->height[0];
    pic->width_chroma    = img->width [1];
    pic->height_chroma   = img->height[1];
    pic->stride_luma     = STRIDE_IMGB2PIC(img->stride[0]);
    pic->stride_chroma   = STRIDE_IMGB2PIC(img->stride[1]);
    pic->padsize_luma    = pad_l;
    pic->padsize_chroma  = pad_c;
    pic->img  = img;

    /* allocate maps */
    i_scu = ((pic->width_luma  + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2) + 2;
    h_scu = ((pic->height_luma + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2) + 2;
    f_scu = i_scu * h_scu;
    size = sizeof(s8) * f_scu * REFP_NUM;

    u8 *buf;
    int total_mem_size = ALIGN_MASK + sizeof( s8) * f_scu * REFP_NUM            + // map_refi
                         ALIGN_MASK + sizeof(s16) * f_scu * REFP_NUM * MV_D;    + // map_mv
                         ALIGN_MASK;
    pic->mem_base = buf = com_malloc(total_mem_size);
    com_assert_gv(buf, ret, COM_ERR_OUT_OF_MEMORY, ERR);
    com_mset_x64a(buf, 0, total_mem_size);

    buf = ALIGN_POINTER(buf);
    GIVE_BUFFER(pic->map_refi, buf, sizeof( s8) * f_scu * REFP_NUM       ); pic->map_refi += i_scu + 1;
    GIVE_BUFFER(pic->map_mv  , buf, sizeof(s16) * f_scu * REFP_NUM * MV_D); pic->map_mv   += i_scu + 1;

    pic->end_line = 0;
    uavs3e_pthread_mutex_init(&pic->mutex, NULL);
    uavs3e_pthread_cond_init(&pic->cv, NULL);


    if (err) {
        *err = COM_OK;
    }
    return pic;
ERR:
    if (pic) {
        com_mfree(pic->mem_base);
        com_mfree(pic);
    }
    if (err) {
        *err = ret;
    }
    return NULL;
}

void com_pic_destroy(com_pic_t *pic)
{
    if (pic) {
        if (pic->img) {
            com_img_release(pic->img);
        }
        uavs3e_pthread_mutex_destroy(&pic->mutex);
        uavs3e_pthread_cond_destroy(&pic->cv);

        com_mfree(pic->mem_base);
        com_mfree(pic);
    }
}

com_subpel_t* com_subpel_create(int width, int height, int pad_l, int pad_c, int *err)
{
    int ret, pad[MAX_PLANES];
    com_subpel_t *p = (com_subpel_t*)com_malloc(sizeof(com_subpel_t));
    com_assert_gv(p, ret, COM_ERR_OUT_OF_MEMORY, ERR);

    pad[0] = pad_l;
    pad[1] = pad_c;
    pad[2] = pad_c;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            p->imgs[i][j] = com_img_create(width, height, pad, 1);
            com_assert_gv(p->imgs[i][j] != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        }
    }

    if (err) {
        *err = COM_OK;
    }
    return p;
ERR:
    if (p) {
        com_subpel_free(p);
    }
    if (err) {
        *err = ret;
    }
    return NULL;

}

void com_subpel_free(com_subpel_t *p)
{
    if (p) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i == 0 && j == 0) {
                    continue;
                }
                if (p->imgs[i][j]) {
                    com_img_release(p->imgs[i][j]);
                }
            }
        }
        com_mfree(p);
    }
}

int get_colocal_scup(int scup, int i_scu, int pic_width_in_scu, int pic_height_in_scu)
{
    const int mask = (-1) ^ 3;
    int bx = scup % i_scu;
    int by = scup / i_scu;
    int xpos = (bx & mask) + 2;
    int ypos = (by & mask) + 2;

    if (ypos >= pic_height_in_scu) {
        ypos = ((by & mask) + pic_height_in_scu) >> 1;
    }
    if (xpos >= pic_width_in_scu) {
        xpos = ((bx & mask) + pic_width_in_scu) >> 1;
    }

    return ypos * i_scu + xpos;
}


void scaling_mv1(s64 ptr_cur, s64 ptr_cur_ref, s64 ptr_neb, s64 ptr_neb_ref, s16 mvp[MV_D], s16 mv[MV_D])
{
    int ratio;
    int t0 = (int)(ptr_neb * 2 - ptr_neb_ref * 2);
    int t1 = (int)(ptr_cur * 2 - ptr_cur_ref * 2);

    assert(t0 != 0 && t1 != 0);
    ratio = (1 << MV_SCALE_PREC) / t0 * t1; // note: divide first for constraining bit-depth

    int offset = 1 << (MV_SCALE_PREC - 1);
    s64 tmp_mv;
    tmp_mv = (s64)mvp[MV_X] * ratio;
    tmp_mv = tmp_mv == 0 ? 0 : tmp_mv > 0 ? ((tmp_mv + offset) >> MV_SCALE_PREC) : -((-tmp_mv + offset) >> MV_SCALE_PREC);
    mv[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, tmp_mv);

    tmp_mv = (s64)mvp[MV_Y] * ratio;
    tmp_mv = tmp_mv == 0 ? 0 : tmp_mv > 0 ? ((tmp_mv + offset) >> MV_SCALE_PREC) : -((-tmp_mv + offset) >> MV_SCALE_PREC);
    mv[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, tmp_mv);
}

void check_mvp_motion_availability(int scup, int cu_width, int cu_height, int i_scu, int neb_addr[NUM_AVS2_SPATIAL_MV], int valid_flag[NUM_AVS2_SPATIAL_MV], com_scu_t *map_scu, s8(*map_refi)[REFP_NUM], int lidx)
{
    int x_scu = scup % i_scu;
    int y_scu = scup / i_scu;
    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
    neb_addr[0] = scup - 1;                                     // A
    neb_addr[1] = scup - i_scu;                      // B
    neb_addr[2] = scup - i_scu + cu_width_in_scu;    // C
    valid_flag[0] = REFI_IS_VALID(map_refi[neb_addr[0]][lidx]);
    valid_flag[1] = REFI_IS_VALID(map_refi[neb_addr[1]][lidx]);
    valid_flag[2] = REFI_IS_VALID(map_refi[neb_addr[2]][lidx]);
    
    if (!valid_flag[2]) {
        neb_addr[2] = scup - i_scu - 1;              // D
        valid_flag[2] = REFI_IS_VALID(map_refi[neb_addr[2]][lidx]);
    }
}

void init_motion(com_motion_t *motion_dst)
{
    int i;
    for (i = 0; i < PRED_BI; i++) {
        motion_dst->ref_idx[i] = -1;
        motion_dst->mv[i][MV_X] = 0;
        motion_dst->mv[i][MV_Y] = 0;
    }
}

void create_motion(com_motion_t *motion_dst, s16 mv_new[REFP_NUM][MV_D], s8 refi_new[REFP_NUM])
{
    int i;
    for (i = 0; i < PRED_BI; i++) {
        CP32(motion_dst->mv[i], mv_new[i]);
        motion_dst->ref_idx[i] = refi_new[i];
    }
}

void copy_motion(com_motion_t *motion_dst, com_motion_t motion_src)
{
    int i;
    for (i = 0; i < PRED_BI; i++) {
        CP32(motion_dst->mv[i], motion_src.mv[i]);
        motion_dst->ref_idx[i] = motion_src.ref_idx[i];
    }
}

void copy_motion_table(com_motion_t *motion_dst, s8 *cnt_cands_dst, const com_motion_t *motion_src, const s8 cnt_cands_src)
{
    *cnt_cands_dst = cnt_cands_src;
    memcpy(motion_dst, motion_src, sizeof(com_motion_t) * cnt_cands_src);
}

int same_motion(com_motion_t motion1, com_motion_t motion2)
{
    if (motion1.ref_idx[PRED_L0] != motion2.ref_idx[PRED_L0]) {
        return 0;
    }
    if (REFI_IS_VALID(motion1.ref_idx[PRED_L0]) && !SAME_MV(motion1.mv[PRED_L0], motion2.mv[PRED_L0])) {
        return 0;
    }
    if (motion1.ref_idx[PRED_L1] != motion2.ref_idx[PRED_L1]) {
        return 0;
    }
    if (REFI_IS_VALID(motion1.ref_idx[PRED_L1]) && !SAME_MV(motion1.mv[PRED_L1], motion2.mv[PRED_L1])) {
        return 0;
    }
    return 1;
}

void check_umve_motion_availability(int scup, int cu_width, int cu_height, int i_scu, int neb_addr[NUM_AVS2_SPATIAL_MV], int valid_flag[NUM_AVS2_SPATIAL_MV], com_scu_t *map_scu, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM])
{
    int x_scu = scup % i_scu;
    int y_scu = scup / i_scu;
    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
    int cu_height_in_scu = cu_height >> MIN_CU_LOG2;
    int tmp_flag[5];

    neb_addr[0] = scup + i_scu * (cu_height_in_scu - 1) - 1; //F
    neb_addr[1] = scup - i_scu + cu_width_in_scu - 1; //G
    neb_addr[2] = scup - i_scu + cu_width_in_scu; //C
    neb_addr[3] = scup - 1; //A
    neb_addr[4] = scup - i_scu - 1; //D

    valid_flag[0] = tmp_flag[0] = COM_IS_INTER_SCU(map_scu[neb_addr[0]]);
    valid_flag[1] = tmp_flag[1] = COM_IS_INTER_SCU(map_scu[neb_addr[1]]);
    valid_flag[2] = tmp_flag[2] = COM_IS_INTER_SCU(map_scu[neb_addr[2]]);
    valid_flag[3] = tmp_flag[3] = COM_IS_INTER_SCU(map_scu[neb_addr[3]]);
    valid_flag[4] = tmp_flag[4] = COM_IS_INTER_SCU(map_scu[neb_addr[4]]);

    com_motion_t m0, m1, m2;

    if (tmp_flag[0] && tmp_flag[1]) {
        create_motion(&m0, map_mv[neb_addr[1]], map_refi[neb_addr[1]]);
        create_motion(&m1, map_mv[neb_addr[0]], map_refi[neb_addr[0]]);
        valid_flag[1] = !same_motion(m0, m1);
    }
    if (tmp_flag[1] && tmp_flag[2]) {
        create_motion(&m0, map_mv[neb_addr[2]], map_refi[neb_addr[2]]);
        create_motion(&m1, map_mv[neb_addr[1]], map_refi[neb_addr[1]]);
        valid_flag[2] = !same_motion(m0, m1);
    }
    if (tmp_flag[0] && tmp_flag[3]) {
        create_motion(&m0, map_mv[neb_addr[3]], map_refi[neb_addr[3]]);
        create_motion(&m1, map_mv[neb_addr[0]], map_refi[neb_addr[0]]);
        valid_flag[3] = !same_motion(m0, m1);
    }
    if (tmp_flag[4]) {
        create_motion(&m0, map_mv[neb_addr[4]], map_refi[neb_addr[4]]);
        if (tmp_flag[3]) {
            create_motion(&m1, map_mv[neb_addr[3]], map_refi[neb_addr[3]]);
        } else {
            init_motion(&m1);
        }
        if (tmp_flag[1]) {
            create_motion(&m2, map_mv[neb_addr[1]], map_refi[neb_addr[1]]);
        } else {
            init_motion(&m2);
        }
        valid_flag[4] = (!tmp_flag[3] || !same_motion(m0, m1)) && (!tmp_flag[1] || !same_motion(m0, m2));
    }
}

void com_get_mvp_hmvp(com_motion_t motion, int lidx, s64 ptr_cur, s8 cur_refi, s16 mvp[MV_D], com_ref_pic_t(*refp)[REFP_NUM], int amvr_idx)
{
    s64 ptr_hmvp_ref;
    s8  refi_hmvp = motion.ref_idx[lidx];
    s64 ptr_cur_ref = refp[cur_refi][lidx].ptr;

    if (REFI_IS_VALID(refi_hmvp)) {
        ptr_hmvp_ref = refp[refi_hmvp][lidx].ptr;
        scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_hmvp_ref, motion.mv[lidx], mvp);
    } else {
        int lidx1 = (lidx == PRED_L0) ? PRED_L1 : PRED_L0;
        refi_hmvp = motion.ref_idx[lidx1];
        ptr_hmvp_ref = refp[refi_hmvp][lidx1].ptr;
        scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_hmvp_ref, motion.mv[lidx1], mvp);
    }

    // clip MVP after rounding (rounding process might result in 32768)
    int mvp_x, mvp_y;
    com_mv_rounding_s32((s32)mvp[MV_X], (s32)mvp[MV_Y], &mvp_x, &mvp_y, amvr_idx, amvr_idx);
    mvp[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_x);
    mvp[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_y);
}


void com_derive_mvp(com_info_t *info, s64 ptr, int scup, int ref_list, int ref_idx, int emvp_flag, int cnt_hmvp_cands, com_motion_t *motion_cands, com_map_t *map, com_ref_pic_t(*refp)[REFP_NUM], int mvr_idx, int cu_width, int cu_height, s16 mvp[MV_D])
{
    if (!emvp_flag) {
        com_get_mvp_default(ptr, scup, ref_list, ref_idx, map->map_mv, map->map_refi, refp, mvr_idx, cu_width, cu_height, info->i_scu, mvp, map->map_scu);
    } else {
        if (cnt_hmvp_cands == 0) {
            mvp[MV_X] = 0;
            mvp[MV_Y] = 0;
        } else if (cnt_hmvp_cands < mvr_idx + 1) {
            com_motion_t motion = motion_cands[cnt_hmvp_cands - 1];
            com_get_mvp_hmvp(motion, ref_list, ptr, ref_idx, mvp, refp, mvr_idx);
        } else {
            com_motion_t motion = motion_cands[cnt_hmvp_cands - 1 - mvr_idx];
            com_get_mvp_hmvp(motion, ref_list, ptr, ref_idx, mvp, refp, mvr_idx);
        }
    }
}

void com_get_mvp_default(s64 ptr_cur, int scup, int lidx, s8 cur_refi, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], com_ref_pic_t(*refp)[REFP_NUM],
                         u8 amvr_idx, int cu_width, int cu_height, int i_scu, s16 mvp[MV_D], com_scu_t *map_scu)
{
    s64 ptr_cur_ref;
    int cnt, hv;
    int mvPredType = MVPRED_xy_MIN;
    int rFrameL, rFrameU, rFrameUR;
    int neb_addr[NUM_AVS2_SPATIAL_MV], valid_flag[NUM_AVS2_SPATIAL_MV];
    s8 refi[NUM_AVS2_SPATIAL_MV];
    s16 MVPs[NUM_AVS2_SPATIAL_MV][MV_D];

    check_mvp_motion_availability(scup, cu_width, cu_height, i_scu, neb_addr, valid_flag, map_scu, map_refi, lidx);
    ptr_cur_ref = refp[cur_refi][lidx].ptr;
    for (cnt = 0; cnt < NUM_AVS2_SPATIAL_MV; cnt++) {
        if (valid_flag[cnt]) {
            refi[cnt] = map_refi[neb_addr[cnt]][lidx];
            assert(REFI_IS_VALID(refi[cnt]));
            s64 ptr_neb_ref = refp[refi[cnt]][lidx].ptr;
            scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, map_mv[neb_addr[cnt]][lidx], MVPs[cnt]);
        } else {
            refi[cnt] = REFI_INVALID;
            MVPs[cnt][MV_X] = 0;
            MVPs[cnt][MV_Y] = 0;
        }
    }
    rFrameL = refi[0];
    rFrameU = refi[1];
    rFrameUR = refi[2];
    if ((rFrameL != REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR == REFI_INVALID)) {
        mvPredType = MVPRED_L;
    } else if ((rFrameL == REFI_INVALID) && (rFrameU != REFI_INVALID) && (rFrameUR == REFI_INVALID)) {
        mvPredType = MVPRED_U;
    } else if ((rFrameL == REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR != REFI_INVALID)) {
        mvPredType = MVPRED_UR;
    }

    for (hv = 0; hv < MV_D; hv++) {
        s32 mva = (s32)MVPs[0][hv], mvb = (s32)MVPs[1][hv], mvc = (s32)MVPs[2][hv];
        switch (mvPredType) {
        case MVPRED_xy_MIN:
            if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
                mvp[hv] = (s16)((mvb + mvc) / 2);
            } else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
                mvp[hv] = (s16)((mvc + mva) / 2);
            } else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
                mvp[hv] = (s16)((mva + mvb) / 2);
            } else {
                s32 mva_ext = abs(mva - mvb);
                s32 mvb_ext = abs(mvb - mvc);
                s32 mvc_ext = abs(mvc - mva);
                s32 pred_vec = COM_MIN(mva_ext, COM_MIN(mvb_ext, mvc_ext));
                if (pred_vec == mva_ext) {
                    mvp[hv] = (s16)((mva + mvb) / 2);
                } else if (pred_vec == mvb_ext) {
                    mvp[hv] = (s16)((mvb + mvc) / 2);
                } else {
                    mvp[hv] = (s16)((mvc + mva) / 2);
                }
            }
            break;
        case MVPRED_L:
            mvp[hv] = (s16)mva;
            break;
        case MVPRED_U:
            mvp[hv] = (s16)mvb;
            break;
        case MVPRED_UR:
            mvp[hv] = (s16)mvc;
            break;
        default:
            assert(0);
            break;
        }
    }

    // clip MVP after rounding (rounding process might result in 32768)
    int mvp_x, mvp_y;
    com_mv_rounding_s32((s32)mvp[MV_X], (s32)mvp[MV_Y], &mvp_x, &mvp_y, amvr_idx, amvr_idx);
    mvp[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_x);
    mvp[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_y);
}

void derive_MHBskip_spatial_motions(int scup, int cu_width, int cu_height, int i_scu, com_scu_t *map_scu, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], s16 skip_pmv[PRED_DIR_NUM][REFP_NUM][MV_D], s8 skip_refi[PRED_DIR_NUM][REFP_NUM])
{
    int x_scu = scup % i_scu;
    int y_scu = scup / i_scu;
    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
    int cu_height_in_scu = cu_height >> MIN_CU_LOG2;
    int k;
    int last_bi_idx = 0;
    u8 L0_motion_found = 0, L1_motion_found = 0, BI_motion_found = 0;
    int neb_addr[NUM_SKIP_SPATIAL_MV];
    int valid_flag[NUM_SKIP_SPATIAL_MV];
    s8 refi[REFP_NUM];

    for (k = 0; k < PRED_DIR_NUM; k++) {
        skip_pmv[k][REFP_0][MV_X] = 0;
        skip_pmv[k][REFP_0][MV_Y] = 0;
        skip_pmv[k][REFP_1][MV_X] = 0;
        skip_pmv[k][REFP_1][MV_Y] = 0;
    }
    skip_refi[2][REFP_0] = 0;
    skip_refi[2][REFP_1] = REFI_INVALID;
    skip_refi[1][REFP_0] = REFI_INVALID;
    skip_refi[1][REFP_1] = 0;
    skip_refi[0][REFP_0] = 0;
    skip_refi[0][REFP_1] = 0;

    //! F: left-below neighbor (inside)
    neb_addr[0] = scup + (cu_height_in_scu - 1) * i_scu - 1;
    valid_flag[0] = COM_IS_INTER_SCU(map_scu[neb_addr[0]]);

    //! G: above-right neighbor (inside)
    neb_addr[1] = scup - i_scu + cu_width_in_scu - 1;
    valid_flag[1] = COM_IS_INTER_SCU(map_scu[neb_addr[1]]);

    //! C: above-right neighbor (outside)
    neb_addr[2] = scup - i_scu + cu_width_in_scu;
    valid_flag[2] = COM_IS_INTER_SCU(map_scu[neb_addr[2]]);

    //! A: left neighbor
    neb_addr[3] = scup - 1;
    valid_flag[3] = COM_IS_INTER_SCU(map_scu[neb_addr[3]]);

    //! B: above neighbor
    neb_addr[4] = scup - i_scu;
    valid_flag[4] = COM_IS_INTER_SCU(map_scu[neb_addr[4]]);

    //! D: above-left neighbor
    neb_addr[5] = scup - i_scu - 1;
    valid_flag[5] = COM_IS_INTER_SCU(map_scu[neb_addr[5]]);

    for (k = 0; k < NUM_SKIP_SPATIAL_MV; k++) {
        if (valid_flag[k]) {
            refi[REFP_0] = REFI_IS_VALID(map_refi[neb_addr[k]][REFP_0]) ? map_refi[neb_addr[k]][REFP_0] : REFI_INVALID;
            refi[REFP_1] = REFI_IS_VALID(map_refi[neb_addr[k]][REFP_1]) ? map_refi[neb_addr[k]][REFP_1] : REFI_INVALID;
            // Search first L0
            if (REFI_IS_VALID(refi[REFP_0]) && !REFI_IS_VALID(refi[REFP_1]) && !L0_motion_found) {
                L0_motion_found = 1;
                skip_pmv[2][REFP_0][MV_X] = map_mv[neb_addr[k]][REFP_0][MV_X];
                skip_pmv[2][REFP_0][MV_Y] = map_mv[neb_addr[k]][REFP_0][MV_Y];
                skip_refi[2][REFP_0] = refi[REFP_0];
            }
            // Search first L1
            if (!REFI_IS_VALID(refi[REFP_0]) && REFI_IS_VALID(refi[REFP_1]) && !L1_motion_found) {
                L1_motion_found = 1;
                skip_pmv[1][REFP_1][MV_X] = map_mv[neb_addr[k]][REFP_1][MV_X];
                skip_pmv[1][REFP_1][MV_Y] = map_mv[neb_addr[k]][REFP_1][MV_Y];
                skip_refi[1][REFP_1] = refi[REFP_1];
            }
            // Search first and last BI
            if (REFI_IS_VALID(refi[REFP_0]) && REFI_IS_VALID(refi[REFP_1])) {
                if (!BI_motion_found) {
                    skip_pmv[0][REFP_0][MV_X] = map_mv[neb_addr[k]][REFP_0][MV_X];
                    skip_pmv[0][REFP_0][MV_Y] = map_mv[neb_addr[k]][REFP_0][MV_Y];
                    skip_refi[0][REFP_0] = refi[REFP_0];
                    skip_pmv[0][REFP_1][MV_X] = map_mv[neb_addr[k]][REFP_1][MV_X];
                    skip_pmv[0][REFP_1][MV_Y] = map_mv[neb_addr[k]][REFP_1][MV_Y];
                    skip_refi[0][REFP_1] = refi[REFP_1];
                }
                BI_motion_found++;
                last_bi_idx = k; // save last BI index (first in order: D, B, A, C, G, F)
            }
        }
    }

    // combine L0+L1 to BI
    if (L0_motion_found && L1_motion_found && !BI_motion_found) {
        skip_pmv[0][REFP_0][MV_X] = skip_pmv[2][REFP_0][MV_X];
        skip_pmv[0][REFP_0][MV_Y] = skip_pmv[2][REFP_0][MV_Y];
        skip_refi[0][REFP_0] = skip_refi[2][REFP_0];
        skip_pmv[0][REFP_1][MV_X] = skip_pmv[1][REFP_1][MV_X];
        skip_pmv[0][REFP_1][MV_Y] = skip_pmv[1][REFP_1][MV_Y];
        skip_refi[0][REFP_1] = skip_refi[1][REFP_1];
    }
    // Separate last BI to L0
    if (!L0_motion_found && BI_motion_found) {
        skip_pmv[2][REFP_0][MV_X] = map_mv[neb_addr[last_bi_idx]][REFP_0][MV_X];
        skip_pmv[2][REFP_0][MV_Y] = map_mv[neb_addr[last_bi_idx]][REFP_0][MV_Y];
        skip_refi[2][REFP_0] = map_refi[neb_addr[last_bi_idx]][REFP_0];
        assert(REFI_IS_VALID(map_refi[neb_addr[last_bi_idx]][REFP_0]));
    }
    // Separate last BI to L1
    if (!L1_motion_found && BI_motion_found) {
        skip_pmv[1][REFP_1][MV_X] = map_mv[neb_addr[last_bi_idx]][REFP_1][MV_X];
        skip_pmv[1][REFP_1][MV_Y] = map_mv[neb_addr[last_bi_idx]][REFP_1][MV_Y];
        skip_refi[1][REFP_1] = map_refi[neb_addr[last_bi_idx]][REFP_1];
        assert(REFI_IS_VALID(map_refi[neb_addr[last_bi_idx]][REFP_1]));
    }

    assert(REFI_IS_VALID(skip_refi[2][REFP_0]) && (!REFI_IS_VALID(skip_refi[2][REFP_1])));
    assert((!REFI_IS_VALID(skip_refi[1][REFP_0])) && REFI_IS_VALID(skip_refi[1][REFP_1]));
    assert(REFI_IS_VALID(skip_refi[0][REFP_0]) && REFI_IS_VALID(skip_refi[0][REFP_1]));
}

void derive_umve_base_motions(int scup, int cu_width, int cu_height, int i_scu, com_scu_t *map_scu, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], s16 t_mv[REFP_NUM][MV_D], s8 t_refi[REFP_NUM], s16 umve_base_pmv[UMVE_BASE_NUM][REFP_NUM][MV_D], s8 umve_base_refi[UMVE_BASE_NUM][REFP_NUM])
{
    int i, j;
    int x_scu = scup % i_scu;
    int y_scu = scup / i_scu;
    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
    int cu_height_in_scu = cu_height >> MIN_CU_LOG2;
    int cnt = 0;

    int neb_addr[5];
    int valid_flag[5];

    for (i = 0; i < UMVE_BASE_NUM; i++) {
        for (j = 0; j < REFP_NUM; j++) {
            umve_base_refi[i][j] = REFI_INVALID;
        }
    }

    //F -> G -> C -> A -> D
    check_umve_motion_availability(scup, cu_width, cu_height, i_scu, neb_addr, valid_flag, map_scu, map_mv, map_refi);

    for (i = 0; i < 5; i++) {
        if (valid_flag[i]) {
            if (REFI_IS_VALID(map_refi[neb_addr[i]][REFP_0])) {
                umve_base_pmv[cnt][REFP_0][MV_X] = map_mv[neb_addr[i]][REFP_0][MV_X];
                umve_base_pmv[cnt][REFP_0][MV_Y] = map_mv[neb_addr[i]][REFP_0][MV_Y];
                umve_base_refi[cnt][REFP_0] = map_refi[neb_addr[i]][REFP_0];
            }
            if (REFI_IS_VALID(map_refi[neb_addr[i]][REFP_1])) {
                umve_base_pmv[cnt][REFP_1][MV_X] = map_mv[neb_addr[i]][REFP_1][MV_X];
                umve_base_pmv[cnt][REFP_1][MV_Y] = map_mv[neb_addr[i]][REFP_1][MV_Y];
                umve_base_refi[cnt][REFP_1] = map_refi[neb_addr[i]][REFP_1];
            }
            cnt++;
        }
        if (cnt == UMVE_BASE_NUM) {
            break;
        }
    }
    if (cnt < UMVE_BASE_NUM) {
        umve_base_pmv[cnt][REFP_0][MV_X] = t_mv[REFP_0][MV_X];
        umve_base_pmv[cnt][REFP_0][MV_Y] = t_mv[REFP_0][MV_Y];
        umve_base_refi[cnt][REFP_0] = t_refi[REFP_0];

        umve_base_pmv[cnt][REFP_1][MV_X] = t_mv[REFP_1][MV_X];
        umve_base_pmv[cnt][REFP_1][MV_Y] = t_mv[REFP_1][MV_Y];
        umve_base_refi[cnt][REFP_1] = t_refi[REFP_1];
        cnt++;
    }

    if (cnt < UMVE_BASE_NUM) {
        umve_base_pmv[cnt][REFP_0][MV_X] = 0;
        umve_base_pmv[cnt][REFP_0][MV_Y] = 0;
        umve_base_refi[cnt][REFP_0] = 0;

        umve_base_pmv[cnt][REFP_1][MV_X] = 0;
        umve_base_pmv[cnt][REFP_1][MV_Y] = 0;
        umve_base_refi[cnt][REFP_1] = -1;
        cnt++;
    }
}

void derive_umve_final_motions(int umve_idx, com_ref_pic_t(*refp)[REFP_NUM], s64 cur_poc, s16 umve_base_pmv[UMVE_BASE_NUM][REFP_NUM][MV_D], s8 umve_base_refi[UMVE_BASE_NUM][REFP_NUM], s16 umve_final_pmv[UMVE_BASE_NUM *UMVE_MAX_REFINE_NUM][REFP_NUM][MV_D], s8 umve_final_refi[UMVE_BASE_NUM *UMVE_MAX_REFINE_NUM][REFP_NUM])
{
    int base_idx = umve_idx / UMVE_MAX_REFINE_NUM;
    int refine_step = (umve_idx - (base_idx * UMVE_MAX_REFINE_NUM)) / 4;
    int direction = umve_idx - base_idx * UMVE_MAX_REFINE_NUM - refine_step * 4;
    s32 mv_offset[REFP_NUM][MV_D];
    int ref_mvd0, ref_mvd1;

    const int ref_mvd_cands[5] = { 1, 2, 4, 8, 16 };
    const int refi0 = umve_base_refi[base_idx][REFP_0];
    const int refi1 = umve_base_refi[base_idx][REFP_1];

    ref_mvd0 = ref_mvd_cands[refine_step];
    ref_mvd1 = ref_mvd_cands[refine_step];
    if (REFI_IS_VALID(refi0) && REFI_IS_VALID(refi1)) {
        const s64 poc0 = refp[refi0][REFP_0].ptr * 2;
        const s64 poc1 = refp[refi1][REFP_1].ptr * 2;
        int list0_weight = 1 << MV_SCALE_PREC;
        int list1_weight = 1 << MV_SCALE_PREC;
        int list0_sign = 1;
        int list1_sign = 1;
        cur_poc *= 2;

        int dist0 = (int)(poc0 - cur_poc);
        int dist1 = (int)(poc1 - cur_poc);

        if (abs(dist1) >= abs(dist0)) {
            list0_weight = (1 << MV_SCALE_PREC) / (abs(dist1)) * abs(dist0);
            if (dist1 * dist0 < 0) {
                list0_sign = -1;
            }
        } else {
            list1_weight = (1 << MV_SCALE_PREC) / (abs(dist0)) * abs(dist1);
            if (dist1 * dist0 < 0) {
                list1_sign = -1;
            }
        }

        ref_mvd0 = (list0_weight * ref_mvd0 + (1 << (MV_SCALE_PREC - 1))) >> MV_SCALE_PREC;
        ref_mvd1 = (list1_weight * ref_mvd1 + (1 << (MV_SCALE_PREC - 1))) >> MV_SCALE_PREC;

        ref_mvd0 = COM_CLIP3(-(1 << 15), (1 << 15) - 1, list0_sign * ref_mvd0);
        ref_mvd1 = COM_CLIP3(-(1 << 15), (1 << 15) - 1, list1_sign * ref_mvd1);

        if (direction == 0) {
            mv_offset[REFP_0][MV_X] = ref_mvd0;
            mv_offset[REFP_0][MV_Y] = 0;
            mv_offset[REFP_1][MV_X] = ref_mvd1;
            mv_offset[REFP_1][MV_Y] = 0;
        } else if (direction == 1) {
            mv_offset[REFP_0][MV_X] = -ref_mvd0;
            mv_offset[REFP_0][MV_Y] = 0;
            mv_offset[REFP_1][MV_X] = -ref_mvd1;
            mv_offset[REFP_1][MV_Y] = 0;
        } else if (direction == 2) {
            mv_offset[REFP_0][MV_X] = 0;
            mv_offset[REFP_0][MV_Y] = ref_mvd0;
            mv_offset[REFP_1][MV_X] = 0;
            mv_offset[REFP_1][MV_Y] = ref_mvd1;
        } else {
            mv_offset[REFP_0][MV_X] = 0;
            mv_offset[REFP_0][MV_Y] = -ref_mvd0;
            mv_offset[REFP_1][MV_X] = 0;
            mv_offset[REFP_1][MV_Y] = -ref_mvd1;
        }

        s32 mv_x = (s32)umve_base_pmv[base_idx][REFP_0][MV_X] + mv_offset[REFP_0][MV_X];
        s32 mv_y = (s32)umve_base_pmv[base_idx][REFP_0][MV_Y] + mv_offset[REFP_0][MV_Y];

        umve_final_pmv[umve_idx][REFP_0][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
        umve_final_pmv[umve_idx][REFP_0][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
        umve_final_refi[umve_idx][REFP_0] = umve_base_refi[base_idx][REFP_0];

        mv_x = (s32)umve_base_pmv[base_idx][REFP_1][MV_X] + mv_offset[REFP_1][MV_X];
        mv_y = (s32)umve_base_pmv[base_idx][REFP_1][MV_Y] + mv_offset[REFP_1][MV_Y];

        umve_final_pmv[umve_idx][REFP_1][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
        umve_final_pmv[umve_idx][REFP_1][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
        umve_final_refi[umve_idx][REFP_1] = umve_base_refi[base_idx][REFP_1];
    } else if (REFI_IS_VALID(refi0)) {
        if (direction == 0) {
            mv_offset[REFP_0][MV_X] = ref_mvd0;
            mv_offset[REFP_0][MV_Y] = 0;
        } else if (direction == 1) {
            mv_offset[REFP_0][MV_X] = -ref_mvd0;
            mv_offset[REFP_0][MV_Y] = 0;
        } else if (direction == 2) {
            mv_offset[REFP_0][MV_X] = 0;
            mv_offset[REFP_0][MV_Y] = ref_mvd0;
        } else { // (direction == 3)
            mv_offset[REFP_0][MV_X] = 0;
            mv_offset[REFP_0][MV_Y] = -ref_mvd0;
        }

        s32 mv_x = (s32)umve_base_pmv[base_idx][REFP_0][MV_X] + mv_offset[REFP_0][MV_X];
        s32 mv_y = (s32)umve_base_pmv[base_idx][REFP_0][MV_Y] + mv_offset[REFP_0][MV_Y];

        umve_final_pmv[umve_idx][REFP_0][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
        umve_final_pmv[umve_idx][REFP_0][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
        umve_final_refi[umve_idx][REFP_0] = umve_base_refi[base_idx][REFP_0];

        umve_final_pmv[umve_idx][REFP_1][MV_X] = 0;
        umve_final_pmv[umve_idx][REFP_1][MV_Y] = 0;
        umve_final_refi[umve_idx][REFP_1] = REFI_INVALID;
    } else if (REFI_IS_VALID(refi1)) {
        if (direction == 0) {
            mv_offset[REFP_1][MV_X] = ref_mvd1;
            mv_offset[REFP_1][MV_Y] = 0;
        } else if (direction == 1) {
            mv_offset[REFP_1][MV_X] = -ref_mvd1;
            mv_offset[REFP_1][MV_Y] = 0;
        } else if (direction == 2) {
            mv_offset[REFP_1][MV_X] = 0;
            mv_offset[REFP_1][MV_Y] = ref_mvd1;
        } else { // (direction == 3)
            mv_offset[REFP_1][MV_X] = 0;
            mv_offset[REFP_1][MV_Y] = -ref_mvd1;
        }
        s32 mv_x = (s32)umve_base_pmv[base_idx][REFP_1][MV_X] + mv_offset[REFP_1][MV_X];
        s32 mv_y = (s32)umve_base_pmv[base_idx][REFP_1][MV_Y] + mv_offset[REFP_1][MV_Y];

        umve_final_pmv[umve_idx][REFP_1][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
        umve_final_pmv[umve_idx][REFP_1][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
        umve_final_refi[umve_idx][REFP_1] = umve_base_refi[base_idx][REFP_1];

        umve_final_pmv[umve_idx][REFP_0][MV_X] = 0;
        umve_final_pmv[umve_idx][REFP_0][MV_Y] = 0;
        umve_final_refi[umve_idx][REFP_0] = REFI_INVALID;
    } else {
        umve_final_pmv[umve_idx][REFP_0][MV_X] = 0;
        umve_final_pmv[umve_idx][REFP_0][MV_Y] = 0;
        umve_final_refi[umve_idx][REFP_0] = REFI_INVALID;

        umve_final_pmv[umve_idx][REFP_1][MV_X] = 0;
        umve_final_pmv[umve_idx][REFP_1][MV_Y] = 0;
        umve_final_refi[umve_idx][REFP_1] = REFI_INVALID;
    }
}

void print_motion(com_motion_t motion)
{
    printf(" %d %d %d", motion.mv[PRED_L0][MV_X], motion.mv[PRED_L0][MV_Y], motion.ref_idx[PRED_L0]);
    printf(" %d %d %d", motion.mv[PRED_L1][MV_X], motion.mv[PRED_L1][MV_Y], motion.ref_idx[PRED_L1]);
    printf("\n");
}

void update_skip_candidates(com_motion_t motion_cands[ALLOWED_HMVP_NUM], s8 *num_cands, const int max_hmvp_num, s16 mv_new[REFP_NUM][MV_D], s8 refi_new[REFP_NUM])
{
    int i, equal_idx = -1;
    com_motion_t motion_curr;
    if (!REFI_IS_VALID(refi_new[REFP_0]) && !REFI_IS_VALID(refi_new[REFP_1])) {
        return;
    }
    create_motion(&motion_curr, mv_new, refi_new);
    assert(*num_cands <= max_hmvp_num);
    for (i = *num_cands - 1; i >= 0; i--) {
        if (same_motion(motion_cands[i], motion_curr)) {
            equal_idx = i;
            break;
        }
    }
    if (equal_idx == -1) { // can be simplified
        if (*num_cands < max_hmvp_num) {
            (*num_cands)++;
        } else {
            int valid_pos = COM_MIN(max_hmvp_num - 1, *num_cands);
            for (i = 1; i <= valid_pos; i++) {
                copy_motion(&motion_cands[i - 1], motion_cands[i]);
            }
        }

        copy_motion(&motion_cands[*num_cands - 1], motion_curr);
    } else {
        for (i = equal_idx; i < *num_cands - 1; i++) {
            copy_motion(&motion_cands[i], motion_cands[i + 1]);
        }
        copy_motion(&motion_cands[*num_cands - 1], motion_curr);
    }

}

void fill_skip_candidates(com_motion_t motion_cands[ALLOWED_HMVP_NUM], s8 *num_cands, const int num_hmvp_cands, s16 mv_new[REFP_NUM][MV_D], s8 refi_new[REFP_NUM], int bRemDuplicate)
{
    int maxNumSkipCand = TRADITIONAL_SKIP_NUM + num_hmvp_cands;
    int i;
    com_motion_t motion_curr;
    assert(REFI_IS_VALID(refi_new[REFP_0]) || REFI_IS_VALID(refi_new[REFP_1]));

    create_motion(&motion_curr, mv_new, refi_new);
    assert(*num_cands <= maxNumSkipCand);
    if (bRemDuplicate) { //! No duplicate motion information exists among HMVP candidates, so duplicate removal is not needed for HMVP itself
        for (i = 0; i < *num_cands; i++) {
            if (same_motion(motion_cands[i], motion_curr)) {
                return;
            }
        }
    }
    if (*num_cands < maxNumSkipCand) {
        copy_motion(&motion_cands[*num_cands], motion_curr);
        (*num_cands)++;
    }
}

void get_hmvp_skip_cands(const com_motion_t motion_cands[ALLOWED_HMVP_NUM], const u8 num_cands, s16(*skip_mvs)[REFP_NUM][MV_D], s8(*skip_refi)[REFP_NUM])
{
    int i, dir;
    for (i = 0; i < num_cands; i++) {
        for (dir = 0; dir < PRED_BI; dir++) {
            CP32(skip_mvs[i][dir], motion_cands[i].mv[dir]);
            skip_refi[i][dir] = motion_cands[i].ref_idx[dir];
        }
    }
}

void get_col_mv_from_list0(com_ref_pic_t refp[REFP_NUM], s64 ptr_curr, int scup_co, s16 mvp[REFP_NUM][MV_D])
{
    s16 mvc[MV_D];
    assert(REFI_IS_VALID(refp[REFP_0].map_refi[scup_co][REFP_0]));

    mvc[MV_X] = refp[REFP_0].map_mv[scup_co][0][MV_X];
    mvc[MV_Y] = refp[REFP_0].map_mv[scup_co][0][MV_Y];
    s64 ptr_col = refp[REFP_0].ptr;
    s64 ptr_col_ref = refp[REFP_0].list_ptr[refp[REFP_0].map_refi[scup_co][REFP_0]];
    scaling_mv1(ptr_curr, refp[REFP_0].ptr, ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
}

// text 9.5.8.4.3
void get_col_mv(com_ref_pic_t refp[REFP_NUM], s64 ptr_curr, int scup_co, s16 mvp[REFP_NUM][MV_D])
{
    s16 mvc[MV_D];
    assert(REFI_IS_VALID(refp[REFP_1].map_refi[scup_co][REFP_0]));

    mvc[MV_X] = refp[REFP_1].map_mv[scup_co][0][MV_X];
    mvc[MV_Y] = refp[REFP_1].map_mv[scup_co][0][MV_Y];
    s64 ptr_col = refp[REFP_1].ptr;
    s64 ptr_col_ref = refp[REFP_1].list_ptr[refp[REFP_1].map_refi[scup_co][REFP_0]];
    scaling_mv1(ptr_curr, refp[REFP_0].ptr, ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
    scaling_mv1(ptr_curr, refp[REFP_1].ptr, ptr_col, ptr_col_ref, mvc, mvp[REFP_1]);
}

u16 com_get_avail_intra(int x_scu, int y_scu, int i_scu, int scup, com_scu_t *map_scu)
{
    u16 avail = 0;
    if (x_scu > 0 && map_scu[scup - 1].coded) {
        SET_AVAIL(avail, AVAIL_LE);
    }
    if (y_scu > 0) {
        if (map_scu[scup - i_scu].coded) {
            SET_AVAIL(avail, AVAIL_UP);
        }
        if (x_scu > 0 && map_scu[scup - i_scu - 1].coded) {
            SET_AVAIL(avail, AVAIL_UP_LE);
        }
    }
    return avail;
}

/* MD5 functions */
#define MD5FUNC(f, w, x, y, z, msg1, s,msg2 ) \
    ( w += f(x, y, z) + msg1 + msg2,  w = w<<s | w>>(32-s),  w += x )
#define FF(x, y, z) (z ^ (x & (y ^ z)))
#define GG(x, y, z) (y ^ (z & (x ^ y)))
#define HH(x, y, z) (x ^ y ^ z)
#define II(x, y, z) (y ^ (x | ~z))

static void com_md5_trans(u32 *buf, u32 *msg)
{
    register u32 a, b, c, d;
    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];
    MD5FUNC(FF, a, b, c, d, msg[ 0],  7, 0xd76aa478); /* 1 */
    MD5FUNC(FF, d, a, b, c, msg[ 1], 12, 0xe8c7b756); /* 2 */
    MD5FUNC(FF, c, d, a, b, msg[ 2], 17, 0x242070db); /* 3 */
    MD5FUNC(FF, b, c, d, a, msg[ 3], 22, 0xc1bdceee); /* 4 */
    MD5FUNC(FF, a, b, c, d, msg[ 4],  7, 0xf57c0faf); /* 5 */
    MD5FUNC(FF, d, a, b, c, msg[ 5], 12, 0x4787c62a); /* 6 */
    MD5FUNC(FF, c, d, a, b, msg[ 6], 17, 0xa8304613); /* 7 */
    MD5FUNC(FF, b, c, d, a, msg[ 7], 22, 0xfd469501); /* 8 */
    MD5FUNC(FF, a, b, c, d, msg[ 8],  7, 0x698098d8); /* 9 */
    MD5FUNC(FF, d, a, b, c, msg[ 9], 12, 0x8b44f7af); /* 10 */
    MD5FUNC(FF, c, d, a, b, msg[10], 17, 0xffff5bb1); /* 11 */
    MD5FUNC(FF, b, c, d, a, msg[11], 22, 0x895cd7be); /* 12 */
    MD5FUNC(FF, a, b, c, d, msg[12],  7, 0x6b901122); /* 13 */
    MD5FUNC(FF, d, a, b, c, msg[13], 12, 0xfd987193); /* 14 */
    MD5FUNC(FF, c, d, a, b, msg[14], 17, 0xa679438e); /* 15 */
    MD5FUNC(FF, b, c, d, a, msg[15], 22, 0x49b40821); /* 16 */
    /* Round 2 */
    MD5FUNC(GG, a, b, c, d, msg[ 1],  5, 0xf61e2562); /* 17 */
    MD5FUNC(GG, d, a, b, c, msg[ 6],  9, 0xc040b340); /* 18 */
    MD5FUNC(GG, c, d, a, b, msg[11], 14, 0x265e5a51); /* 19 */
    MD5FUNC(GG, b, c, d, a, msg[ 0], 20, 0xe9b6c7aa); /* 20 */
    MD5FUNC(GG, a, b, c, d, msg[ 5],  5, 0xd62f105d); /* 21 */
    MD5FUNC(GG, d, a, b, c, msg[10],  9,  0x2441453); /* 22 */
    MD5FUNC(GG, c, d, a, b, msg[15], 14, 0xd8a1e681); /* 23 */
    MD5FUNC(GG, b, c, d, a, msg[ 4], 20, 0xe7d3fbc8); /* 24 */
    MD5FUNC(GG, a, b, c, d, msg[ 9],  5, 0x21e1cde6); /* 25 */
    MD5FUNC(GG, d, a, b, c, msg[14],  9, 0xc33707d6); /* 26 */
    MD5FUNC(GG, c, d, a, b, msg[ 3], 14, 0xf4d50d87); /* 27 */
    MD5FUNC(GG, b, c, d, a, msg[ 8], 20, 0x455a14ed); /* 28 */
    MD5FUNC(GG, a, b, c, d, msg[13],  5, 0xa9e3e905); /* 29 */
    MD5FUNC(GG, d, a, b, c, msg[ 2],  9, 0xfcefa3f8); /* 30 */
    MD5FUNC(GG, c, d, a, b, msg[ 7], 14, 0x676f02d9); /* 31 */
    MD5FUNC(GG, b, c, d, a, msg[12], 20, 0x8d2a4c8a); /* 32 */
    /* Round 3 */
    MD5FUNC(HH, a, b, c, d, msg[ 5],  4, 0xfffa3942); /* 33 */
    MD5FUNC(HH, d, a, b, c, msg[ 8], 11, 0x8771f681); /* 34 */
    MD5FUNC(HH, c, d, a, b, msg[11], 16, 0x6d9d6122); /* 35 */
    MD5FUNC(HH, b, c, d, a, msg[14], 23, 0xfde5380c); /* 36 */
    MD5FUNC(HH, a, b, c, d, msg[ 1],  4, 0xa4beea44); /* 37 */
    MD5FUNC(HH, d, a, b, c, msg[ 4], 11, 0x4bdecfa9); /* 38 */
    MD5FUNC(HH, c, d, a, b, msg[ 7], 16, 0xf6bb4b60); /* 39 */
    MD5FUNC(HH, b, c, d, a, msg[10], 23, 0xbebfbc70); /* 40 */
    MD5FUNC(HH, a, b, c, d, msg[13],  4, 0x289b7ec6); /* 41 */
    MD5FUNC(HH, d, a, b, c, msg[ 0], 11, 0xeaa127fa); /* 42 */
    MD5FUNC(HH, c, d, a, b, msg[ 3], 16, 0xd4ef3085); /* 43 */
    MD5FUNC(HH, b, c, d, a, msg[ 6], 23,  0x4881d05); /* 44 */
    MD5FUNC(HH, a, b, c, d, msg[ 9],  4, 0xd9d4d039); /* 45 */
    MD5FUNC(HH, d, a, b, c, msg[12], 11, 0xe6db99e5); /* 46 */
    MD5FUNC(HH, c, d, a, b, msg[15], 16, 0x1fa27cf8); /* 47 */
    MD5FUNC(HH, b, c, d, a, msg[ 2], 23, 0xc4ac5665); /* 48 */
    /* Round 4 */
    MD5FUNC(II, a, b, c, d, msg[ 0],  6, 0xf4292244); /* 49 */
    MD5FUNC(II, d, a, b, c, msg[ 7], 10, 0x432aff97); /* 50 */
    MD5FUNC(II, c, d, a, b, msg[14], 15, 0xab9423a7); /* 51 */
    MD5FUNC(II, b, c, d, a, msg[ 5], 21, 0xfc93a039); /* 52 */
    MD5FUNC(II, a, b, c, d, msg[12],  6, 0x655b59c3); /* 53 */
    MD5FUNC(II, d, a, b, c, msg[ 3], 10, 0x8f0ccc92); /* 54 */
    MD5FUNC(II, c, d, a, b, msg[10], 15, 0xffeff47d); /* 55 */
    MD5FUNC(II, b, c, d, a, msg[ 1], 21, 0x85845dd1); /* 56 */
    MD5FUNC(II, a, b, c, d, msg[ 8],  6, 0x6fa87e4f); /* 57 */
    MD5FUNC(II, d, a, b, c, msg[15], 10, 0xfe2ce6e0); /* 58 */
    MD5FUNC(II, c, d, a, b, msg[ 6], 15, 0xa3014314); /* 59 */
    MD5FUNC(II, b, c, d, a, msg[13], 21, 0x4e0811a1); /* 60 */
    MD5FUNC(II, a, b, c, d, msg[ 4],  6, 0xf7537e82); /* 61 */
    MD5FUNC(II, d, a, b, c, msg[11], 10, 0xbd3af235); /* 62 */
    MD5FUNC(II, c, d, a, b, msg[ 2], 15, 0x2ad7d2bb); /* 63 */
    MD5FUNC(II, b, c, d, a, msg[ 9], 21, 0xeb86d391); /* 64 */
    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

void com_md5_init(com_md5_t *md5)
{
    md5->h[0] = 0x67452301;
    md5->h[1] = 0xefcdab89;
    md5->h[2] = 0x98badcfe;
    md5->h[3] = 0x10325476;
    md5->bits[0] = 0;
    md5->bits[1] = 0;
}

void com_md5_update(com_md5_t *md5, void *buf_t, u32 len)
{
    u8 *buf;
    u32 i, idx, part_len;
    buf = (u8 *)buf_t;
    idx = (u32)((md5->bits[0] >> 3) & 0x3f);
    md5->bits[0] += (len << 3);
    if (md5->bits[0] < (len << 3)) {
        (md5->bits[1])++;
    }
    md5->bits[1] += (len >> 29);
    part_len = 64 - idx;
    if (len >= part_len) {
        com_mcpy(md5->msg + idx, buf, part_len);
        com_md5_trans(md5->h, (u32 *)md5->msg);
        for (i = part_len; i + 63 < len; i += 64) {
            com_md5_trans(md5->h, (u32 *)(buf + i));
        }
        idx = 0;
    } else {
        i = 0;
    }
    if (len - i > 0) {
        com_mcpy(md5->msg + idx, buf + i, len - i);
    }
}

static void com_md5_update_s16(com_md5_t *md5, void *buf_t, u32 len)
{
    u8 *buf;
    u32 i, idx, part_len, j;
    u8 t[8196 * 10];
    buf = (u8 *)buf_t;
    idx = (u32)((md5->bits[0] >> 3) & 0x3f);
    len = len * 2;
    for (j = 0; j < len; j += 2) {
        t[j] = (u8)(*(buf));
        t[j + 1] = 0;
        buf++;
    }
    md5->bits[0] += (len << 3);
    if (md5->bits[0] < (len << 3)) {
        (md5->bits[1])++;
    }
    md5->bits[1] += (len >> 29);
    part_len = 64 - idx;
    if (len >= part_len) {
        com_mcpy(md5->msg + idx, t, part_len);
        com_md5_trans(md5->h, (u32 *)md5->msg);
        for (i = part_len; i + 63 < len; i += 64) {
            com_md5_trans(md5->h, (u32 *)(t + i));
        }
        idx = 0;
    } else {
        i = 0;
    }
    if (len - i > 0) {
        com_mcpy(md5->msg + idx, t + i, len - i);
    }
}

void com_md5_finish(com_md5_t *md5, u8 digest[16])
{
    u8 *pos;
    int cnt;
    cnt = (md5->bits[0] >> 3) & 0x3F;
    pos = md5->msg + cnt;
    *pos++ = 0x80;
    cnt = 64 - 1 - cnt;
    if (cnt < 8) {
        com_mset(pos, 0, cnt);
        com_md5_trans(md5->h, (u32 *)md5->msg);
        com_mset(md5->msg, 0, 56);
    } else {
        com_mset(pos, 0, cnt - 8);
    }
    com_mcpy((md5->msg + 14 * sizeof(u32)), &md5->bits[0], sizeof(u32));
    com_mcpy((md5->msg + 15 * sizeof(u32)), &md5->bits[1], sizeof(u32));
    com_md5_trans(md5->h, (u32 *)md5->msg);
    com_mcpy(digest, md5->h, 16);
    com_mset(md5, 0, sizeof(com_md5_t));
}

void com_md5_img(com_img_t *img, u8 digest[16])
{
    com_md5_t md5;
    int i, j;
    com_md5_init(&md5);

    for (i = 0; i < img->num_planes; i++) {
        for (j = 0; j < img->height[i]; j++) {
#if (BIT_DEPTH == 8)
            com_md5_update_s16(&md5, ((u8 *)img->planes[i]) + j * img->stride[i], img->width[i]);
#else
            com_md5_update(&md5, ((u8 *)img->planes[i]) + j * img->stride[i], img->width[i] * 2);
#endif
        }
    }

    com_md5_finish(&md5, digest);
}

void init_scan(u16 *scan, int size_x, int size_y)
{
    int x, y, l, pos, num_line;
    pos = 0;
    num_line = size_x + size_y - 1;

    /* starting point */
    scan[pos] = 0;
    pos++;
    /* loop */
    for (l = 1; l < num_line; l++) {
        if (l % 2) { /* decreasing loop */
            x = COM_MIN(l, size_x - 1);
            y = COM_MAX(0, l - (size_x - 1));
            while (x >= 0 && y < size_y) {
                scan[pos] = (u16)(y * size_x + x);
                pos++;
                x--;
                y++;
            }
        } else { /* increasing loop */
            y = COM_MIN(l, size_y - 1);
            x = COM_MAX(0, l - (size_y - 1));
            while (y >= 0 && x < size_x) {
                scan[pos] = (u16)(y * size_x + x);
                pos++;
                x++;
                y--;
            }
        }
    }
}

int com_scan_tbl_init()
{
    int x, y;
    int size_y, size_x;

    for (y = 0; y < MAX_CU_LOG2; y++) {
        size_y = 1 << (y + 1);
        for (x = 0; x < MAX_CU_LOG2; x++) {
            size_x = 1 << (x + 1);
            com_tbl_scan[x][y] = (u16 *)com_malloc(size_y * size_x * sizeof(u16));
            init_scan(com_tbl_scan[x][y], size_x, size_y);
        }
    }
 
    return COM_OK;
}

int com_scan_tbl_delete()
{
    int x, y;

    for (y = 0; y < MAX_CU_LOG2 - 1; y++) {
        for (x = 0; x < MAX_CU_LOG2 - 1; x++) {
            if (com_tbl_scan[x][y] != NULL) {
                com_mfree(com_tbl_scan[x][y]);
            }
        }
    }
 
    return COM_OK;
}

u8 com_get_cons_pred_mode(int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE])
{
    int pos = cup + (((cu_height >> 1) >> MIN_CU_LOG2) * (lcu_s >> MIN_CU_LOG2) + ((cu_width >> 1) >> MIN_CU_LOG2));
    int shape = SQUARE + (CONV_LOG2(cu_width) - CONV_LOG2(cu_height));
    int cons_pred_mode = NO_MODE_CONS;
    assert(shape >= 0 && shape < NUM_BLOCK_SHAPE);
    if (cu_width < 8 && cu_height < 8) {
        assert(0);
    }
    cons_pred_mode = (split_mode_buf[pos][cud][shape] >> 3) & 0x03;
    assert(cons_pred_mode == ONLY_INTRA || cons_pred_mode == ONLY_INTER);
    return cons_pred_mode;
}

void com_set_cons_pred_mode(u8 cons_pred_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE])
{
    int pos = cup + (((cu_height >> 1) >> MIN_CU_LOG2) * (lcu_s >> MIN_CU_LOG2) + ((cu_width >> 1) >> MIN_CU_LOG2));
    int shape = SQUARE + (CONV_LOG2(cu_width) - CONV_LOG2(cu_height));
    assert(shape >= 0 && shape < NUM_BLOCK_SHAPE);
    if (cu_width < 8 && cu_height < 8) {
        assert(0);
    }
    split_mode_buf[pos][cud][shape] = (split_mode_buf[pos][cud][shape] & 0x67) + (cons_pred_mode << 3); //01100111 = 0x67
}


int com_get_split_mode(s8 *split_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE])
{
    int ret = COM_OK;
    int pos = cup + (((cu_height >> 1) >> MIN_CU_LOG2) * (lcu_s >> MIN_CU_LOG2) + ((cu_width >> 1) >> MIN_CU_LOG2));
    int shape = SQUARE + (CONV_LOG2(cu_width) - CONV_LOG2(cu_height));
    assert(shape >= 0 && shape < NUM_BLOCK_SHAPE);
    if (cu_width < 8 && cu_height < 8) {
        *split_mode = NO_SPLIT;
        return ret;
    }
    *split_mode = split_mode_buf[pos][cud][shape] & 0x07;
    return ret;
}

int com_set_split_mode(s8 split_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE])
{
    int ret = COM_OK;
    int pos = cup + (((cu_height >> 1) >> MIN_CU_LOG2) * (lcu_s >> MIN_CU_LOG2) + ((cu_width >> 1) >> MIN_CU_LOG2));
    int shape = SQUARE + (CONV_LOG2(cu_width) - CONV_LOG2(cu_height));
    assert(shape >= 0 && shape < NUM_BLOCK_SHAPE);
    if (cu_width < 8 && cu_height < 8) {
        return ret;
    }
    split_mode_buf[pos][cud][shape] = split_mode;
    return ret;
}

void com_check_split_mode(com_seqh_t *sqh, int *split_allow, int cu_width_log2, int cu_height_log2, int boundary, int boundary_b, int boundary_r, int log2_max_cuwh,
                          const int parent_split, int qt_depth, int bet_depth, int slice_type)
{
    //constraints parameters
    const int min_cu_size      = sqh->min_cu_size;
    const int min_bt_size      = sqh->min_cu_size;
    const int min_eqt_size     = sqh->min_cu_size;
    const int max_split_depth  = sqh->max_split_times;
    const int max_aspect_ratio = sqh->max_part_ratio;
    const int min_qt_size      = sqh->min_qt_size;
    const int max_bt_size      = sqh->max_bt_size;
    const int max_eqt_size     = sqh->max_eqt_size;
    int max_aspect_ratio_eqt = max_aspect_ratio >> 1;
    int cu_w = 1 << cu_width_log2;
    int cu_h = 1 << cu_height_log2;
    int i;

    for (i = NO_SPLIT; i <= SPLIT_QUAD; i++) {
        split_allow[i] = 0;
    }

    if (boundary) {
        // VPDU previous than boundary
        if ((cu_w == 64 && cu_h == 128) || (cu_h == 64 && cu_w == 128)) {
            split_allow[SPLIT_BI_HOR] = 1;
            split_allow[SPLIT_BI_VER] = 1;
        }
        // large block previous than boundary
        else if (slice_type == SLICE_I && cu_w == 128 && cu_h == 128) {
            split_allow[SPLIT_QUAD] = 1;
            split_allow[NO_SPLIT] = 1;
        } else if (!boundary_r && !boundary_b) {
            split_allow[SPLIT_QUAD] = 1;
        } else if (boundary_r) {
            split_allow[SPLIT_BI_VER] = 1;
        } else if (boundary_b) {
            split_allow[SPLIT_BI_HOR] = 1;
        }
        assert(qt_depth + bet_depth < max_split_depth);
    } else {
        // VPDU
        if ((cu_w == 64 && cu_h == 128) || (cu_h == 64 && cu_w == 128)) {
            split_allow[SPLIT_BI_HOR] = 1;
            split_allow[SPLIT_BI_VER] = 1;
            split_allow[NO_SPLIT] = 1;
        }
        //max qt-bt depth constraint
        else if (qt_depth + bet_depth >= max_split_depth) {
            split_allow[NO_SPLIT]   = 1; //no further split allowed
        } else if (slice_type == SLICE_I && cu_w == 128 && cu_h == 128) {
            split_allow[SPLIT_QUAD] = 1;
            split_allow[NO_SPLIT] = 1;
        } else {
            //not split
            if (cu_w <= cu_h *max_aspect_ratio && cu_h <= cu_w * max_aspect_ratio) {
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

/*******************************************/
/* Neighbor location: Graphical indication */
/*                                         */
/*           D B_____________G C           */
/*           A|               |            */
/*            |               |            */
/*            |               |            */
/*            |      cu    cuh|            */
/*            |               |            */
/*            |               |            */
/*           F|_____cuw______H|            */
/*                                         */
/*                                         */
/*******************************************/

void com_set_affine_mvf(int scup, int log2_cuw, int log2_cuh, int i_scu, com_mode_t *cur_info, com_map_t *pic_map, com_pic_header_t *sh)
{
    int   cp_num = cur_info->affine_flag + 1;
    int   aff_scup[VER_NUM];
    int   sub_w = 4;
    int   sub_h = 4;

    if (sh->affine_subblk_size_idx == 1) {
        sub_w = 8;
        sub_h = 8;
    }
    if (REFI_IS_VALID(cur_info->refi[REFP_0]) && REFI_IS_VALID(cur_info->refi[REFP_1])) {
        sub_w = 8;
        sub_h = 8;
    }

    int half_w = sub_w >> 1;
    int half_h = sub_h >> 1;
    int cu_w_in_scu = (1 << log2_cuw) >> MIN_CU_LOG2;
    int cu_h_in_scu = (1 << log2_cuh) >> MIN_CU_LOG2;
    int sub_w_in_scu = sub_w >> MIN_CU_LOG2;
    int sub_h_in_scu = sub_h >> MIN_CU_LOG2;

    s8(*map_refi)[REFP_NUM] = pic_map->map_refi + scup;

    for (int h_sch = 0; h_sch < cu_h_in_scu; h_sch++) {
        for (int w_sch = 0; w_sch < cu_w_in_scu; w_sch++) {
            CP16(map_refi + w_sch, cur_info->refi);
        }
        map_refi += i_scu;
    }

    aff_scup[0] = 0;
    aff_scup[1] = (cu_w_in_scu - 1);
    aff_scup[2] = (cu_h_in_scu - 1) * i_scu;
    aff_scup[3] = (cu_w_in_scu - 1) + (cu_h_in_scu - 1) * i_scu;

    for (int lidx = 0; lidx < REFP_NUM; lidx++) {
        if (cur_info->refi[lidx] >= 0) {
            CPMV(*ac_mv)[MV_D] = cur_info->affine_mv[lidx];
            s32 dmv_ver_x, dmv_ver_y;
            s32 mv_scale_hor = (s32)ac_mv[0][MV_X] << 7;
            s32 mv_scale_ver = (s32)ac_mv[0][MV_Y] << 7;
            s32 mv_scale_tmp_hor, mv_scale_tmp_ver;

            // convert to 2^(storeBit + iBit) precision
            s32 dmv_hor_x = (((s32)ac_mv[1][MV_X] - (s32)ac_mv[0][MV_X]) << 7) >> log2_cuw;
            s32 dmv_hor_y = (((s32)ac_mv[1][MV_Y] - (s32)ac_mv[0][MV_Y]) << 7) >> log2_cuw;

            if (cp_num == 3) {
                dmv_ver_x = (((s32)ac_mv[2][MV_X] - (s32)ac_mv[0][MV_X]) << 7) >> log2_cuh;
                dmv_ver_y = (((s32)ac_mv[2][MV_Y] - (s32)ac_mv[0][MV_Y]) << 7) >> log2_cuh;
            } else {
                dmv_ver_x = -dmv_hor_y;                                                    
                dmv_ver_y =  dmv_hor_x;
            }

            s16(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv + scup; 

            for (int h_sch = 0; h_sch < cu_h_in_scu; h_sch += sub_h_in_scu) {
                for (int w_sch = 0; w_sch < cu_w_in_scu; w_sch += sub_w_in_scu) {
                    int pos_x = (w_sch << MIN_CU_LOG2) + half_w;
                    int pos_y = (h_sch << MIN_CU_LOG2) + half_h;
                    if (w_sch == 0 && h_sch == 0) {
                        pos_x = 0;
                        pos_y = 0;
                    } else if (w_sch + sub_w_in_scu == cu_w_in_scu && h_sch == 0) {
                        pos_x = cu_w_in_scu << MIN_CU_LOG2;
                        pos_y = 0;
                    } else if (w_sch == 0 && h_sch + sub_h_in_scu == cu_h_in_scu && cp_num == 3) {
                        pos_x = 0;
                        pos_y = cu_h_in_scu << MIN_CU_LOG2;
                    }

                    mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
                    mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;

                    // 1/16 precision, 18 bits, for MC
                    com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
                    mv_scale_tmp_hor = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_hor);
                    mv_scale_tmp_ver = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_ver);

                    // 1/4 precision, 16 bits, for mv storage
                    com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 2, 0);
                    mv_scale_tmp_hor = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_scale_tmp_hor);
                    mv_scale_tmp_ver = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_scale_tmp_ver);

                    // save MV for each 4x4 block
                    map_mv[w_sch][lidx][MV_X] = (s16)mv_scale_tmp_hor;
                    map_mv[w_sch][lidx][MV_Y] = (s16)mv_scale_tmp_ver;

                    if (sub_w_in_scu > 1) {
                        map_mv[w_sch +         1][lidx][MV_X] = (s16)mv_scale_tmp_hor;
                        map_mv[w_sch +         1][lidx][MV_Y] = (s16)mv_scale_tmp_ver;
                        map_mv[w_sch + i_scu    ][lidx][MV_X] = (s16)mv_scale_tmp_hor;
                        map_mv[w_sch + i_scu    ][lidx][MV_Y] = (s16)mv_scale_tmp_ver;
                        map_mv[w_sch + i_scu + 1][lidx][MV_X] = (s16)mv_scale_tmp_hor;
                        map_mv[w_sch + i_scu + 1][lidx][MV_Y] = (s16)mv_scale_tmp_ver;
                    } 
                }
                map_mv += sub_h_in_scu * i_scu;
            }
        }
    }
}

int com_get_affine_memory_access(CPMV mv[VER_NUM][MV_D], int cu_width, int cu_height)
{
    s16 mv_tmp[VER_NUM][MV_D];
    for (int i = 0; i < VER_NUM; i++) {
        mv_tmp[i][MV_X] = mv[i][MV_X] >> 2;
        mv_tmp[i][MV_Y] = mv[i][MV_Y] >> 2;
    }
    int max_x = COM_MAX(mv_tmp[0][MV_X], COM_MAX(mv_tmp[1][MV_X] + cu_width, COM_MAX(mv_tmp[2][MV_X], mv_tmp[3][MV_X] + cu_width))) >> 2;
    int min_x = COM_MIN(mv_tmp[0][MV_X], COM_MIN(mv_tmp[1][MV_X] + cu_width, COM_MIN(mv_tmp[2][MV_X], mv_tmp[3][MV_X] + cu_width))) >> 2;
    int max_y = COM_MAX(mv_tmp[0][MV_Y], COM_MAX(mv_tmp[1][MV_Y], COM_MAX(mv_tmp[2][MV_Y] + cu_height, mv_tmp[3][MV_Y] + cu_height))) >> 2;
    int min_y = COM_MIN(mv_tmp[0][MV_Y], COM_MIN(mv_tmp[1][MV_Y], COM_MIN(mv_tmp[2][MV_Y] + cu_height, mv_tmp[3][MV_Y] + cu_height))) >> 2;
    return (abs(max_x - min_x) + 7) * (abs(max_y - min_y) + 7);
}

void com_derive_affine_model_mv(int scup, int scun, int lidx, s16(*map_mv)[REFP_NUM][MV_D], int cu_width, int cu_height, int i_scu, CPMV mvp[VER_NUM][MV_D], u32 *map_pos, int *vertex_num, int log2_max_cuwh)
{
    s16 neb_mv[VER_NUM][MV_D] = {{0, }, };
    int i;
    int neb_addr[VER_NUM];
    int neb_log_w = MCU_GET_LOGW(map_pos[scun]);
    int neb_log_h = MCU_GET_LOGH(map_pos[scun]);
    int neb_w = 1 << neb_log_w;
    int neb_h = 1 << neb_log_h;
    int neb_x, neb_y;
    int cur_x, cur_y;
    int diff_w = 7 - neb_log_w;
    int diff_h = 7 - neb_log_h;
    s32 dmv_hor_x, dmv_hor_y, dmv_ver_x, dmv_ver_y, hor_base, ver_base;
    s32 tmp_hor, tmp_ver;
    neb_addr[0] = MCU_GET_SCUP(map_pos[scun]);
    neb_addr[1] = neb_addr[0] + ((neb_w >> MIN_CU_LOG2) - 1);
    neb_addr[2] = neb_addr[0] + ((neb_h >> MIN_CU_LOG2) - 1) * i_scu;
    neb_addr[3] = neb_addr[2] + ((neb_w >> MIN_CU_LOG2) - 1);

    neb_x = (neb_addr[0] % i_scu) << MIN_CU_LOG2;
    neb_y = (neb_addr[0] / i_scu) << MIN_CU_LOG2;
    cur_x = (scup % i_scu) << MIN_CU_LOG2;
    cur_y = (scup / i_scu) << MIN_CU_LOG2;

    for (i = 0; i < VER_NUM; i++) {
        neb_mv[i][MV_X] = map_mv[neb_addr[i]][lidx][MV_X];
        neb_mv[i][MV_Y] = map_mv[neb_addr[i]][lidx][MV_Y];
    }

    int is_top_ctu_boundary = FALSE;
    if ((neb_y + neb_h) % (1 << log2_max_cuwh) == 0 && (neb_y + neb_h) == cur_y) {
        is_top_ctu_boundary = TRUE;
        neb_y += neb_h;

        neb_mv[0][MV_X] = neb_mv[2][MV_X];
        neb_mv[0][MV_Y] = neb_mv[2][MV_Y];
        neb_mv[1][MV_X] = neb_mv[3][MV_X];
        neb_mv[1][MV_Y] = neb_mv[3][MV_Y];
    }

    dmv_hor_x = (s32)(neb_mv[1][MV_X] - neb_mv[0][MV_X]) << diff_w;      // deltaMvHor
    dmv_hor_y = (s32)(neb_mv[1][MV_Y] - neb_mv[0][MV_Y]) << diff_w;
    if (*vertex_num == 3 && !is_top_ctu_boundary) {
        dmv_ver_x = (s32)(neb_mv[2][MV_X] - neb_mv[0][MV_X]) << diff_h;  // deltaMvVer
        dmv_ver_y = (s32)(neb_mv[2][MV_Y] - neb_mv[0][MV_Y]) << diff_h;
    } else {
        dmv_ver_x = -dmv_hor_y;                                          // deltaMvVer
        dmv_ver_y = dmv_hor_x;
        *vertex_num = 2;
    }
    hor_base = (s32)neb_mv[0][MV_X] << 7;
    ver_base = (s32)neb_mv[0][MV_Y] << 7;

    // derive CPMV 0
    tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y) + hor_base;
    tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y) + ver_base;
    com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);

    tmp_hor <<= 2;
    tmp_ver <<= 2;

    mvp[0][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_hor);
    mvp[0][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_ver);

    // derive CPMV 1
    tmp_hor = dmv_hor_x * (cur_x - neb_x + cu_width) + dmv_ver_x * (cur_y - neb_y) + hor_base;
    tmp_ver = dmv_hor_y * (cur_x - neb_x + cu_width) + dmv_ver_y * (cur_y - neb_y) + ver_base;
    com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);

    tmp_hor <<= 2;
    tmp_ver <<= 2;

    mvp[1][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_hor);
    mvp[1][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_ver);

    // derive CPMV 2
    if (*vertex_num == 3) {
        tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y + cu_height) + hor_base;
        tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y + cu_height) + ver_base;
        com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);

        tmp_hor <<= 2;
        tmp_ver <<= 2;

        mvp[2][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_hor);
        mvp[2][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_ver);
    }
}

int com_derive_affine_constructed_candidate(int cu_width, int cu_height, int cp_valid[VER_NUM], s16 cp_mv[REFP_NUM][VER_NUM][MV_D], int cp_refi[REFP_NUM][VER_NUM], int cp_idx[VER_NUM], int model_idx, int ver_num, CPMV mrg_list_cp_mv[AFF_MAX_NUM_MRG][REFP_NUM][VER_NUM][MV_D], s8 mrg_list_refi[AFF_MAX_NUM_MRG][REFP_NUM], int mrg_list_cp_num[AFF_MAX_NUM_MRG], int *mrg_idx)
{
    assert(com_tbl_log2[cu_width] >= 4);
    assert(com_tbl_log2[cu_height] >= 4);

    int lidx, i;
    int valid_model[2] = { 0, 0 };
    s32 cpmv_tmp[REFP_NUM][VER_NUM][MV_D];
    int tmp_hor, tmp_ver;
    int shiftHtoW = 7 + com_tbl_log2[cu_width] - com_tbl_log2[cu_height]; // x * cuWidth / cuHeight

    // early terminate
    if (*mrg_idx >= AFF_MAX_NUM_MRG) {
        return 0;
    }

    // check valid model and decide ref idx
    if (ver_num == 2) {
        int idx0 = cp_idx[0], idx1 = cp_idx[1];
        if (!cp_valid[idx0] || !cp_valid[idx1]) {
            return 0;
        }
        for (lidx = 0; lidx < REFP_NUM; lidx++) {
            if (REFI_IS_VALID(cp_refi[lidx][idx0]) && REFI_IS_VALID(cp_refi[lidx][idx1]) && cp_refi[lidx][idx0] == cp_refi[lidx][idx1]) {
                valid_model[lidx] = 1;
            }
        }
    } else if (ver_num == 3) {
        int idx0 = cp_idx[0], idx1 = cp_idx[1], idx2 = cp_idx[2];
        if (!cp_valid[idx0] || !cp_valid[idx1] || !cp_valid[idx2]) {
            return 0;
        }

        for (lidx = 0; lidx < REFP_NUM; lidx++) {
            if (REFI_IS_VALID(cp_refi[lidx][idx0]) && REFI_IS_VALID(cp_refi[lidx][idx1]) && REFI_IS_VALID(cp_refi[lidx][idx2]) && cp_refi[lidx][idx0] == cp_refi[lidx][idx1] && cp_refi[lidx][idx0] == cp_refi[lidx][idx2]) {
                valid_model[lidx] = 1;
            }
        }
    }

    // set merge index and vertex num for valid model
    if (valid_model[0] || valid_model[1]) {
        mrg_list_cp_num[*mrg_idx] = ver_num;
    } else {
        return 0;
    }

    for (lidx = 0; lidx < REFP_NUM; lidx++) {
        if (valid_model[lidx]) {
            for (i = 0; i < ver_num; i++) {
                cpmv_tmp[lidx][cp_idx[i]][MV_X] = (s32)cp_mv[lidx][cp_idx[i]][MV_X];
                cpmv_tmp[lidx][cp_idx[i]][MV_Y] = (s32)cp_mv[lidx][cp_idx[i]][MV_Y];
            }

            // convert to LT, RT[, [LB], [RB]]
            switch (model_idx) {
            case 0: // 0 : LT, RT, LB
                break;
            case 1: // 1 : LT, RT, RB
                cpmv_tmp[lidx][2][MV_X] = cpmv_tmp[lidx][3][MV_X] + cpmv_tmp[lidx][0][MV_X] - cpmv_tmp[lidx][1][MV_X];
                cpmv_tmp[lidx][2][MV_Y] = cpmv_tmp[lidx][3][MV_Y] + cpmv_tmp[lidx][0][MV_Y] - cpmv_tmp[lidx][1][MV_Y];
                break;
            case 2: // 2 : LT, LB, RB
                cpmv_tmp[lidx][1][MV_X] = cpmv_tmp[lidx][3][MV_X] + cpmv_tmp[lidx][0][MV_X] - cpmv_tmp[lidx][2][MV_X];
                cpmv_tmp[lidx][1][MV_Y] = cpmv_tmp[lidx][3][MV_Y] + cpmv_tmp[lidx][0][MV_Y] - cpmv_tmp[lidx][2][MV_Y];
                break;
            case 3: // 3 : RT, LB, RB
                cpmv_tmp[lidx][0][MV_X] = cpmv_tmp[lidx][1][MV_X] + cpmv_tmp[lidx][2][MV_X] - cpmv_tmp[lidx][3][MV_X];
                cpmv_tmp[lidx][0][MV_Y] = cpmv_tmp[lidx][1][MV_Y] + cpmv_tmp[lidx][2][MV_Y] - cpmv_tmp[lidx][3][MV_Y];
                break;
            case 4: // 4 : LT, RT
                break;
            case 5: // 5 : LT, LB
                tmp_hor = +((cpmv_tmp[lidx][2][MV_Y] - cpmv_tmp[lidx][0][MV_Y]) << shiftHtoW) + (cpmv_tmp[lidx][0][MV_X] << 7);
                tmp_ver = -((cpmv_tmp[lidx][2][MV_X] - cpmv_tmp[lidx][0][MV_X]) << shiftHtoW) + (cpmv_tmp[lidx][0][MV_Y] << 7);
                com_mv_rounding_s32(tmp_hor, tmp_ver, &cpmv_tmp[lidx][1][MV_X], &cpmv_tmp[lidx][1][MV_Y], 7, 0);
                break;
            default:
                com_assert(0);
            }

            mrg_list_refi[*mrg_idx][lidx] = cp_refi[lidx][cp_idx[0]];
            for (i = 0; i < ver_num; i++) {
                // convert to 1/16 precision
                cpmv_tmp[lidx][i][MV_X] <<= 2;
                cpmv_tmp[lidx][i][MV_Y] <<= 2;

                mrg_list_cp_mv[*mrg_idx][lidx][i][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, cpmv_tmp[lidx][i][MV_X]);
                mrg_list_cp_mv[*mrg_idx][lidx][i][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, cpmv_tmp[lidx][i][MV_Y]);
            }
        } else {
            mrg_list_refi[*mrg_idx][lidx] = -1;
            for (i = 0; i < ver_num; i++) {
                mrg_list_cp_mv[*mrg_idx][lidx][i][MV_X] = 0;
                mrg_list_cp_mv[*mrg_idx][lidx][i][MV_Y] = 0;
            }
        }
    }

    (*mrg_idx)++;

    return 1;
}

/* merge affine mode */
int com_get_affine_merge_candidate(s64 ptr, int scup, s8(*map_refi)[REFP_NUM], s16(*map_mv)[REFP_NUM][MV_D], com_scu_t *map_scu, u32 *map_pos, com_ref_pic_t(*refp)[REFP_NUM], int cu_width, int cu_height, int i_scu, int pic_width_in_scu, int pic_height_in_scu, int slice_type, s8 mrg_list_refi[AFF_MAX_NUM_MRG][REFP_NUM], CPMV mrg_list_cpmv[AFF_MAX_NUM_MRG][REFP_NUM][VER_NUM][MV_D], int mrg_list_cp_num[AFF_MAX_NUM_MRG], int log2_max_cuwh)
{
    int lidx, i, k;
    int x_scu = scup % i_scu;
    int y_scu = scup / i_scu;
    int cu_width_in_scu  = cu_width >> MIN_CU_LOG2;
    int cu_height_in_scu = cu_height >> MIN_CU_LOG2;
    int cnt = 0;

    //-------------------  Model based affine MVP  -------------------//
    {
        int neb_addr[5];
        int valid_flag[5];
        int top_left[5];

        neb_addr[0] = scup + i_scu * (cu_height_in_scu - 1) - 1; // F
        neb_addr[1] = scup - i_scu + cu_width_in_scu - 1;        // G
        neb_addr[2] = scup - i_scu + cu_width_in_scu;            // C
        neb_addr[3] = scup - 1;                                             // A
        neb_addr[4] = scup - i_scu - 1;                          // D

        valid_flag[0] = map_scu[neb_addr[0]].affine;
        valid_flag[1] = map_scu[neb_addr[1]].affine;
        valid_flag[2] = map_scu[neb_addr[2]].affine;
        valid_flag[3] = map_scu[neb_addr[3]].affine;
        valid_flag[4] = map_scu[neb_addr[4]].affine;

        for (k = 0; k < 5; k++) {
            if (valid_flag[k]) {
                top_left[k] = MCU_GET_SCUP(map_pos[neb_addr[k]]);
            }
        }

        if (valid_flag[2] && valid_flag[1] && top_left[1] == top_left[2]) { // exclude same CU cases
            valid_flag[2] = 0;
        }
        int valid_flag_3_bakup = valid_flag[3];

        if (valid_flag[3] && valid_flag[0] && top_left[0] == top_left[3]) {
            valid_flag[3] = 0;
        }

        if ((valid_flag[4] && valid_flag_3_bakup && top_left[4] == top_left[3]) || (valid_flag[4] && valid_flag[1] && top_left[4] == top_left[1])) {
            valid_flag[4] = 0;
        }

        for (k = 0; k < 5; k++) {
            if (valid_flag[k]) {
                // set vertex number: affine flag == 1, set to 2 vertex, otherwise, set to 3 vertex
                mrg_list_cp_num[cnt] = (map_scu[neb_addr[k]].affine == 1) ? 2 : 3;

                for (lidx = 0; lidx < REFP_NUM; lidx++) {
                    if (REFI_IS_VALID(map_refi[neb_addr[k]][lidx])) {
                        mrg_list_refi[cnt][lidx] = map_refi[neb_addr[k]][lidx];
                        com_derive_affine_model_mv(scup, neb_addr[k], lidx, map_mv, cu_width, cu_height, i_scu, mrg_list_cpmv[cnt][lidx], map_pos, &mrg_list_cp_num[cnt], log2_max_cuwh);
                    } else { // set to default value
                        mrg_list_refi[cnt][lidx] = -1;
                        for (i = 0; i < VER_NUM; i++) {
                            mrg_list_cpmv[cnt][lidx][i][MV_X] = 0;
                            mrg_list_cpmv[cnt][lidx][i][MV_Y] = 0;
                        }
                    }
                }
                cnt++;
            }

            if (cnt >= AFF_MODEL_CAND) {
                break;
            }
        }
    }

    //-------------------  control point based affine MVP  -------------------//
    {
        s16 cp_mv[REFP_NUM][VER_NUM][MV_D];
        int cp_refi[REFP_NUM][VER_NUM];
        int cp_valid[VER_NUM];

        int neb_addr_lt[AFFINE_MAX_NUM_LT];
        int neb_addr_rt[AFFINE_MAX_NUM_RT];
        int neb_addr_lb;

        int valid_flag_lt[AFFINE_MAX_NUM_LT];
        int valid_flag_rt[AFFINE_MAX_NUM_RT];
        int valid_flag_lb;

        //------------------  INIT  ------------------//
        for (i = 0; i < VER_NUM; i++) {
            for (lidx = 0; lidx < REFP_NUM; lidx++) {
                cp_mv[lidx][i][MV_X] = 0;
                cp_mv[lidx][i][MV_Y] = 0;
                cp_refi[lidx][i] = -1;
            }
            cp_valid[i] = 0;
        }

        //-------------------  LT  -------------------//
        neb_addr_lt[0] = scup - 1;                     // A
        neb_addr_lt[1] = scup - i_scu;      // B
        neb_addr_lt[2] = scup - i_scu - 1;  // D

        valid_flag_lt[0] = COM_IS_INTER_SCU(map_scu[neb_addr_lt[0]]);
        valid_flag_lt[1] = COM_IS_INTER_SCU(map_scu[neb_addr_lt[1]]);
        valid_flag_lt[2] = COM_IS_INTER_SCU(map_scu[neb_addr_lt[2]]);

        for (k = 0; k < AFFINE_MAX_NUM_LT; k++) {
            if (valid_flag_lt[k]) {
                for (lidx = 0; lidx < REFP_NUM; lidx++) {
                    cp_refi[lidx][0] = map_refi[neb_addr_lt[k]][lidx];
                    cp_mv[lidx][0][MV_X] = map_mv[neb_addr_lt[k]][lidx][MV_X];
                    cp_mv[lidx][0][MV_Y] = map_mv[neb_addr_lt[k]][lidx][MV_Y];
                }
                cp_valid[0] = 1;
                break;
            }
        }

        //-------------------  RT  -------------------//
        neb_addr_rt[0] = scup - i_scu + cu_width_in_scu - 1;     // G
        neb_addr_rt[1] = scup - i_scu + cu_width_in_scu;         // C
        valid_flag_rt[0] = COM_IS_INTER_SCU(map_scu[neb_addr_rt[0]]);
        valid_flag_rt[1] = COM_IS_INTER_SCU(map_scu[neb_addr_rt[1]]);

        for (k = 0; k < AFFINE_MAX_NUM_RT; k++) {
            if (valid_flag_rt[k]) {
                for (lidx = 0; lidx < REFP_NUM; lidx++) {
                    cp_refi[lidx][1] = map_refi[neb_addr_rt[k]][lidx];
                    cp_mv[lidx][1][MV_X] = map_mv[neb_addr_rt[k]][lidx][MV_X];
                    cp_mv[lidx][1][MV_Y] = map_mv[neb_addr_rt[k]][lidx][MV_Y];
                }
                cp_valid[1] = 1;
                break;
            }
        }

        //-------------------  LB  -------------------//
        neb_addr_lb = scup + i_scu * (cu_height_in_scu - 1) - 1;  // F
        valid_flag_lb = COM_IS_INTER_SCU(map_scu[neb_addr_lb]);

        if (valid_flag_lb) {
            for (lidx = 0; lidx < REFP_NUM; lidx++) {
                cp_refi[lidx][2] = map_refi[neb_addr_lb][lidx];
                cp_mv[lidx][2][MV_X] = map_mv[neb_addr_lb][lidx][MV_X];
                cp_mv[lidx][2][MV_Y] = map_mv[neb_addr_lb][lidx][MV_Y];
            }
            cp_valid[2] = 1;
        }

        //-------------------  RB  -------------------//
        s16 mv_tmp[REFP_NUM][MV_D];
        int neb_addr_rb = scup + i_scu * (cu_height_in_scu - 1) + (cu_width_in_scu - 1);     // Col
        int scup_co = get_colocal_scup(neb_addr_rb, i_scu, pic_width_in_scu, pic_height_in_scu);

        if (slice_type == SLICE_B) {
            if (!REFI_IS_VALID(refp[0][REFP_1].map_refi[scup_co][REFP_0])) {
                cp_valid[3] = 0;
            } else {
                get_col_mv(refp[0], ptr, scup_co, mv_tmp);
                for (lidx = 0; lidx < REFP_NUM; lidx++) {
                    cp_refi[lidx][3] = 0; // ref idx
                    cp_mv[lidx][3][MV_X] = mv_tmp[lidx][MV_X];
                    cp_mv[lidx][3][MV_Y] = mv_tmp[lidx][MV_Y];
                }
                cp_valid[3] = 1;
            }
        } else {
            if (REFI_IS_VALID(refp[0][REFP_0].map_refi[scup_co][REFP_0])) {
                get_col_mv_from_list0(refp[0], ptr, scup_co, mv_tmp);
                cp_refi[0][3] = 0; // ref idx
                cp_mv[0][3][MV_X] = mv_tmp[0][MV_X];
                cp_mv[0][3][MV_Y] = mv_tmp[0][MV_Y];

                cp_refi[1][3] = REFI_INVALID;
                cp_mv[1][3][MV_X] = 0;
                cp_mv[1][3][MV_Y] = 0;
                cp_valid[3] = 1;
            } else {
                cp_valid[3] = 0;
            }
        }

        //-------------------  insert model  -------------------//
        int const_order[6] = { 0, 1, 2, 3, 4, 5 };
        int const_num = 6;

        int idx = 0;
        int const_model[6][VER_NUM] = {
            { 0, 1, 2 },          // 0: LT, RT, LB
            { 0, 1, 3 },          // 1: LT, RT, RB
            { 0, 2, 3 },          // 2: LT, LB, RB
            { 1, 2, 3 },          // 3: RT, LB, RB
            { 0, 1 },             // 4: LT, RT
            { 0, 2 },             // 5: LT, LB
        };

        int cp_num[6] = { 3, 3, 3, 3, 2, 2 };
        for (idx = 0; idx < const_num; idx++) {
            int const_idx = const_order[idx];
            com_derive_affine_constructed_candidate(cu_width, cu_height, cp_valid, cp_mv, cp_refi, const_model[const_idx], const_idx, cp_num[const_idx], mrg_list_cpmv, mrg_list_refi, mrg_list_cp_num, &cnt);
        }
    }

    // Zero padding
    {
        int pad, cp_idx;
        for (pad = cnt; pad < AFF_MAX_NUM_MRG; pad++) {
            mrg_list_cp_num[pad] = 2;
            for (lidx = 0; lidx < REFP_NUM; lidx++) {
                for (cp_idx = 0; cp_idx < 2; cp_idx++) {
                    mrg_list_cpmv[pad][lidx][cp_idx][MV_X] = 0;
                    mrg_list_cpmv[pad][lidx][cp_idx][MV_Y] = 0;
                }
            }
            mrg_list_refi[pad][REFP_0] = 0;
            mrg_list_refi[pad][REFP_1] = REFI_INVALID;
        }
    }

    return cnt;
}

/* inter affine mode */
void com_get_affine_mvp_scaling(s64 ptr, int scup, int lidx, s8 cur_refi, \
                                s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], com_ref_pic_t(*refp)[REFP_NUM], \
                                int cu_width, int cu_height, int i_scu, CPMV mvp[VER_NUM][MV_D], com_scu_t *map_scu, u32 *map_pos, int vertex_num
                                , u8 curr_mvr
                               )
{
    s64 ptr_cur_ref;
    int x_scu = scup % i_scu;
    int y_scu = scup / i_scu;
    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
    int cnt = 0, cnt_lt = 0, cnt_rt = 0;
    int i, j, k;

    s16 mvp_lt[MV_D], mvp_rt[MV_D];
    int neb_addr_lt[AFFINE_MAX_NUM_LT];
    int valid_flag_lt[AFFINE_MAX_NUM_LT];
    int neb_addr_rt[AFFINE_MAX_NUM_RT];
    int valid_flag_rt[AFFINE_MAX_NUM_RT];

    ptr_cur_ref = refp[cur_refi][lidx].ptr;
    for (j = 0; j < VER_NUM; j++) {
        mvp[j][MV_X] = 0;
        mvp[j][MV_Y] = 0;
    }

    //-------------------  LT  -------------------//
    neb_addr_lt[0] = scup - 1;                     // A
    neb_addr_lt[1] = scup - i_scu;      // B
    neb_addr_lt[2] = scup - i_scu - 1;  // D
    valid_flag_lt[0] = REFI_IS_VALID(map_refi[neb_addr_lt[0]][lidx]);
    valid_flag_lt[1] = REFI_IS_VALID(map_refi[neb_addr_lt[1]][lidx]);
    valid_flag_lt[2] = REFI_IS_VALID(map_refi[neb_addr_lt[2]][lidx]);

    for (k = 0; k < AFFINE_MAX_NUM_LT; k++) {
        if (valid_flag_lt[k]) {
            s64 ptr_neb_ref = refp[map_refi[neb_addr_lt[k]][lidx]][lidx].ptr;
            scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, map_mv[neb_addr_lt[k]][lidx], mvp_lt);
            cnt_lt++;
            break;
        }
    }

    if (cnt_lt == 0) {
        mvp_lt[MV_X] = 0;
        mvp_lt[MV_Y] = 0;
        cnt_lt++;
    }

    //-------------------  RT  -------------------//
    neb_addr_rt[0] = scup - i_scu + cu_width_in_scu - 1;     // G
    neb_addr_rt[1] = scup - i_scu + cu_width_in_scu;         // C
    valid_flag_rt[0] = REFI_IS_VALID(map_refi[neb_addr_rt[0]][lidx]);
    valid_flag_rt[1] = REFI_IS_VALID(map_refi[neb_addr_rt[1]][lidx]);

    for (k = 0; k < AFFINE_MAX_NUM_RT; k++) {
        if (valid_flag_rt[k]) {
            s64 ptr_neb_ref = refp[map_refi[neb_addr_rt[k]][lidx]][lidx].ptr;
            scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, map_mv[neb_addr_rt[k]][lidx], mvp_rt);
            cnt_rt++;
            break;
        }
    }

    if (cnt_rt == 0) {
        mvp_rt[MV_X] = 0;
        mvp_rt[MV_Y] = 0;
        cnt_rt++;
    }

    mvp[0][MV_X] = mvp_lt[MV_X];
    mvp[0][MV_Y] = mvp_lt[MV_Y];
    mvp[1][MV_X] = mvp_rt[MV_X];
    mvp[1][MV_Y] = mvp_rt[MV_Y];


    for (i = 0; i < 2; i++) {
        // convert to 1/16 precision
        s32 mvp_x = (s32)mvp[i][MV_X] << 2;
        s32 mvp_y = (s32)mvp[i][MV_Y] << 2;

        // rounding
        int amvr_shift = Tab_Affine_AMVR(curr_mvr);
        com_mv_rounding_s32(mvp_x, mvp_y, &mvp_x, &mvp_y, amvr_shift, amvr_shift);

        // clipping
        mvp[i][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mvp_x);
        mvp[i][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mvp_y);
    }
}


void com_lbac_ctx_init(com_lbac_all_ctx_t *lbac_ctx)
{
    int i, num;
    lbac_ctx_model_t *p;
    com_mset(lbac_ctx, 0x00, sizeof(*lbac_ctx));

    /* Initialization of the context models */
    num = sizeof(com_lbac_all_ctx_t) / sizeof(lbac_ctx_model_t);
    p = (lbac_ctx_model_t *)lbac_ctx;

    for (i = 0; i < num; i++) {
        p[i] = PROB_INIT;
    }
}


int com_split_part_count(int split_mode)
{
    switch (split_mode) {
    case SPLIT_BI_VER:
    case SPLIT_BI_HOR:
        return 2;
    case SPLIT_EQT_VER:
    case SPLIT_EQT_HOR:
        return 4;
    case SPLIT_QUAD:
        return 4;
    default:
        // NO_SPLIT
        return 0;
    }
}

int com_split_get_part_size(int split_mode, int part_num, int length)
{
    int ans = length;
    switch (split_mode) {
    case SPLIT_QUAD:
    case SPLIT_BI_HOR:
    case SPLIT_BI_VER:
        ans = length >> 1;
        break;
    case SPLIT_EQT_HOR:
    case SPLIT_EQT_VER:
        if ((part_num == 1) || (part_num == 2)) {
            ans = length >> 1;
        } else {
            ans = length >> 2;
        }
        break;
    }
    return ans;
}

int com_split_get_part_size_idx(int split_mode, int part_num, int length_idx)
{
    int ans = length_idx;
    switch (split_mode) {
    case SPLIT_QUAD:
    case SPLIT_BI_HOR:
    case SPLIT_BI_VER:
        ans = length_idx - 1;
        break;
    case SPLIT_EQT_HOR:
    case SPLIT_EQT_VER:
        if ((part_num == 1) || (part_num == 2)) {
            ans = length_idx - 1;
        } else {
            ans = length_idx - 2;
        }
        break;
    }
    return ans;
}

void com_split_get_part_structure(int split_mode, int x0, int y0, int cu_width, int cu_height, int cup, int cud, int log2_lcuwh_in_scu, com_split_struct_t *split_struct)
{
    int i;
    int log_cuw, log_cuh;
    int cup_w, cup_h;
    split_struct->part_count = com_split_part_count(split_mode);
    log_cuw = CONV_LOG2(cu_width);
    log_cuh = CONV_LOG2(cu_height);
    split_struct->x_pos[0] = x0;
    split_struct->y_pos[0] = y0;
    split_struct->cup[0] = cup;
    switch (split_mode) {
    case NO_SPLIT: {
        split_struct->width[0] = cu_width;
        split_struct->height[0] = cu_height;
        split_struct->log_cuw[0] = log_cuw;
        split_struct->log_cuh[0] = log_cuh;
    }
    break;
    case SPLIT_QUAD: {
        split_struct->width[0] = cu_width >> 1;
        split_struct->height[0] = cu_height >> 1;
        split_struct->log_cuw[0] = log_cuw - 1;
        split_struct->log_cuh[0] = log_cuh - 1;
        for (i = 1; i < split_struct->part_count; ++i) {
            split_struct->width[i] = split_struct->width[0];
            split_struct->height[i] = split_struct->height[0];
            split_struct->log_cuw[i] = split_struct->log_cuw[0];
            split_struct->log_cuh[i] = split_struct->log_cuh[0];
        }
        split_struct->x_pos[1] = x0 + split_struct->width[0];
        split_struct->y_pos[1] = y0;
        split_struct->x_pos[2] = x0;
        split_struct->y_pos[2] = y0 + split_struct->height[0];
        split_struct->x_pos[3] = split_struct->x_pos[1];
        split_struct->y_pos[3] = split_struct->y_pos[2];
        cup_w = (split_struct->width[0] >> MIN_CU_LOG2);
        cup_h = ((split_struct->height[0] >> MIN_CU_LOG2) << log2_lcuwh_in_scu);
        split_struct->cup[1] = cup + cup_w;
        split_struct->cup[2] = cup + cup_h;
        split_struct->cup[3] = split_struct->cup[1] + cup_h;
        split_struct->cud = cud + 1;
    }
    break;
    default: {
        if (com_split_is_vertical(split_mode)) {
            for (i = 0; i < split_struct->part_count; ++i) {
                split_struct->width[i] = com_split_get_part_size(split_mode, i, cu_width);
                split_struct->log_cuw[i] = com_split_get_part_size_idx(split_mode, i, log_cuw);

                if (split_mode == SPLIT_EQT_VER) {
                    if (i == 0 || i == 3) {
                        split_struct->height[i] = cu_height;
                        split_struct->log_cuh[i] = log_cuh;
                    } else {
                        split_struct->height[i] = cu_height >> 1;
                        split_struct->log_cuh[i] = log_cuh - 1;
                    }
                } else {
                    split_struct->height[i] = cu_height;
                    split_struct->log_cuh[i] = log_cuh;
                    if (i) {
                        split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
                        split_struct->y_pos[i] = split_struct->y_pos[i - 1];
                        split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> MIN_CU_LOG2);
                    }
                }
            }
            if (split_mode == SPLIT_EQT_VER) {
                split_struct->x_pos[1] = split_struct->x_pos[0] + split_struct->width[0];
                split_struct->y_pos[1] = split_struct->y_pos[0];
                split_struct->cup[1] = split_struct->cup[0] + (split_struct->width[0] >> MIN_CU_LOG2);
                cup_h = ((split_struct->height[1] >> MIN_CU_LOG2) << log2_lcuwh_in_scu);
                split_struct->x_pos[2] = split_struct->x_pos[1];
                split_struct->y_pos[2] = split_struct->y_pos[1] + split_struct->height[1];
                split_struct->cup[2] = split_struct->cup[1] + cup_h;
                split_struct->x_pos[3] = split_struct->x_pos[1] + split_struct->width[1];
                split_struct->y_pos[3] = split_struct->y_pos[1];
                split_struct->cup[3] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
            }
        } else {
            for (i = 0; i < split_struct->part_count; ++i) {
                if (split_mode == SPLIT_EQT_HOR) {
                    if (i == 0 || i == 3) {
                        split_struct->width[i] = cu_width;
                        split_struct->log_cuw[i] = log_cuw;
                    } else {
                        split_struct->width[i] = cu_width >> 1;
                        split_struct->log_cuw[i] = log_cuw - 1;
                    }
                } else {
                    split_struct->width[i] = cu_width;
                    split_struct->log_cuw[i] = log_cuw;
                    if (i) {
                        split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
                        split_struct->x_pos[i] = split_struct->x_pos[i - 1];
                        split_struct->cup[i] = split_struct->cup[i - 1] + ((split_struct->height[i - 1] >> MIN_CU_LOG2) << log2_lcuwh_in_scu);
                    }
                }
                split_struct->height[i] = com_split_get_part_size(split_mode, i, cu_height);
                split_struct->log_cuh[i] = com_split_get_part_size_idx(split_mode, i, log_cuh);
            }
            if (split_mode == SPLIT_EQT_HOR) {
                split_struct->y_pos[1] = split_struct->y_pos[0] + split_struct->height[0];
                split_struct->x_pos[1] = split_struct->x_pos[0];
                split_struct->cup[1] = split_struct->cup[0] + ((split_struct->height[0] >> MIN_CU_LOG2) << log2_lcuwh_in_scu);
                split_struct->y_pos[2] = split_struct->y_pos[1];
                split_struct->x_pos[2] = split_struct->x_pos[1] + split_struct->width[1];
                split_struct->cup[2] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
                split_struct->y_pos[3] = split_struct->y_pos[1] + split_struct->height[1];
                split_struct->x_pos[3] = split_struct->x_pos[1];
                split_struct->cup[3] = split_struct->cup[1] + ((split_struct->height[1] >> MIN_CU_LOG2) << log2_lcuwh_in_scu);
            }
        }
        switch (split_mode) {
        case SPLIT_BI_VER:
            split_struct->cud = cud + ((cu_width == cu_height || cu_width < cu_height) ? 0 : 1);
            break;
        case SPLIT_BI_HOR:
            split_struct->cud = cud + ((cu_width == cu_height || cu_width > cu_height) ? 0 : 1);
            break;
        default:
            // Triple tree case
            split_struct->cud = cud + (cu_width == cu_height ? 0 : 1);
            break;
        }
    }
    break;
    }
}

void com_split_get_split_rdo_order(int cu_width, int cu_height, split_mode_t splits[NUM_SPLIT_MODE])
{
    splits[0] = NO_SPLIT;
    //qt must be tried first; otherwise, due to the split save & load fast algorithm, qt will be never tried in RDO (previous split decision is made base on bt/eqt)
    splits[1] = SPLIT_QUAD;
    splits[2] = cu_width < cu_height ? SPLIT_BI_HOR : SPLIT_BI_VER;
    splits[3] = cu_width < cu_height ? SPLIT_BI_VER : SPLIT_BI_HOR;
    splits[4] = cu_width < cu_height ? SPLIT_EQT_HOR : SPLIT_EQT_VER;
    splits[5] = cu_width < cu_height ? SPLIT_EQT_VER : SPLIT_EQT_HOR;
}

split_dir_t com_split_get_direction(split_mode_t mode)
{
    switch (mode) {
    case SPLIT_BI_HOR:
    case SPLIT_EQT_HOR:
        return SPLIT_HOR;
    case SPLIT_BI_VER:
    case SPLIT_EQT_VER:
        return SPLIT_VER;
    default:
        return SPLIT_QT;
    }
}

int com_split_is_vertical(split_mode_t mode)
{
    return com_split_get_direction(mode) == SPLIT_VER ? 1 : 0;
}

int com_split_is_horizontal(split_mode_t mode)
{
    return com_split_get_direction(mode) == SPLIT_HOR ? 1 : 0;
}

int  com_split_is_EQT(split_mode_t mode)
{
    return (mode == SPLIT_EQT_HOR) || (mode == SPLIT_EQT_VER) ? 1 : 0;
}
int  com_split_is_BT(split_mode_t mode)
{
    return (mode == SPLIT_BI_HOR) || (mode == SPLIT_BI_VER) ? 1 : 0;
}

int com_dt_allow(int cu_w, int cu_h, int pred_mode, int max_dt_size)
{
    //only allow intra DT
    if (pred_mode != MODE_INTRA) {
        return 0;
    }

    int max_size = max_dt_size;
    int min_size = 16;
    int hori_allow = cu_h >= min_size && (cu_w <= max_size && cu_h <= max_size) && cu_w < cu_h * 4;
    int vert_allow = cu_w >= min_size && (cu_w <= max_size && cu_h <= max_size) && cu_h < cu_w * 4;

    return hori_allow + (vert_allow << 1);
}

void init_tb_part(com_mode_t *mode)
{
    mode->tb_part = SIZE_2Nx2N;
}

void init_pb_part(com_mode_t *mode)
{
    mode->pb_part = SIZE_2Nx2N;
    cu_nz_cln(mode->num_nz);
}

void set_pb_part(com_mode_t *mode, part_size_t part_size)
{
    mode->pb_part = part_size;
}

void set_tb_part(com_mode_t *mode, part_size_t part_size)
{
    mode->tb_part = part_size;
}

void get_part_info(int i_scu, int x, int y, int w, int h, part_size_t part_size, com_part_info_t *sub_info)
{
    int i;
    int qw = w >> 2;
    int qh = h >> 2;
    int x_scu, y_scu;
    memset(sub_info, 0, sizeof(com_part_info_t));

    //derive sub_part x, y, w, h
    switch (part_size) {
    case SIZE_2Nx2N:
        sub_info->num_sub_part = 1;
        sub_info->sub_x[0] = x;
        sub_info->sub_y[0] = y;
        sub_info->sub_w[0] = w;
        sub_info->sub_h[0] = h;
        break;
    case SIZE_2NxhN:
        sub_info->num_sub_part = 4;
        for (i = 0; i < sub_info->num_sub_part; i++) {
            sub_info->sub_x[i] = x;
            sub_info->sub_y[i] = qh * i + y;
            sub_info->sub_w[i] = w;
            sub_info->sub_h[i] = qh;
        }
        break;
    case SIZE_2NxnU:
        sub_info->num_sub_part = 2;
        for (i = 0; i < sub_info->num_sub_part; i++) {
            sub_info->sub_x[i] = x;
            sub_info->sub_y[i] = qh * (i == 0 ? 0 : 1) + y;
            sub_info->sub_w[i] = w;
            sub_info->sub_h[i] = qh * (i == 0 ? 1 : 3);
        }
        break;
    case SIZE_2NxnD:
        sub_info->num_sub_part = 2;
        for (i = 0; i < sub_info->num_sub_part; i++) {
            sub_info->sub_x[i] = x;
            sub_info->sub_y[i] = qh * (i == 0 ? 0 : 3) + y;
            sub_info->sub_w[i] = w;
            sub_info->sub_h[i] = qh * (i == 0 ? 3 : 1);
        }
        break;
    case SIZE_hNx2N:
        sub_info->num_sub_part = 4;
        for (i = 0; i < sub_info->num_sub_part; i++) {
            sub_info->sub_x[i] = qw * i + x;
            sub_info->sub_y[i] = y;
            sub_info->sub_w[i] = qw;
            sub_info->sub_h[i] = h;
        }
        break;
    case SIZE_nLx2N:
        sub_info->num_sub_part = 2;
        for (i = 0; i < sub_info->num_sub_part; i++) {
            sub_info->sub_x[i] = qw * (i == 0 ? 0 : 1) + x;
            sub_info->sub_y[i] = y;
            sub_info->sub_w[i] = qw * (i == 0 ? 1 : 3);
            sub_info->sub_h[i] = h;
        }
        break;
    case SIZE_nRx2N:
        sub_info->num_sub_part = 2;
        for (i = 0; i < sub_info->num_sub_part; i++) {
            sub_info->sub_x[i] = qw * (i == 0 ? 0 : 3) + x;
            sub_info->sub_y[i] = y;
            sub_info->sub_w[i] = qw * (i == 0 ? 3 : 1);
            sub_info->sub_h[i] = h;
        }
        break;
    case SIZE_NxN:
        sub_info->num_sub_part = 4;
        for (i = 0; i < sub_info->num_sub_part; i++) {
            sub_info->sub_x[i] = qw * (i == 0 || i == 2 ? 0 : 2) + x;
            sub_info->sub_y[i] = qh * (i == 0 || i == 1 ? 0 : 2) + y;
            sub_info->sub_w[i] = qw * 2;
            sub_info->sub_h[i] = qh * 2;
        }
        break;
    default:
        assert(0);
    }

    //derive sub_part scup
    for (i = 0; i < sub_info->num_sub_part; i++) {
        x_scu = PEL2SCU(sub_info->sub_x[i]);
        y_scu = PEL2SCU(sub_info->sub_y[i]);
        sub_info->sub_scup[i] = x_scu + y_scu * i_scu;
    }
}

int  get_part_idx(part_size_t part_size, int x, int y, int w, int h)
{
    int idx = 0;
    int hw = w >> 1; //half width
    int hh = h >> 1; //half height
    int qw = w >> 2; //quarter width
    int qh = h >> 2; //quarter height

    assert(x < w);
    assert(y < h);
    assert(w >= 4);
    assert(h >= 4);

    if (part_size == SIZE_2Nx2N) {
        idx = 0;
    } else if (part_size == SIZE_NxN) {
        if (x < hw && y < hh) {
            idx = 0;
        } else if (x >= hw && y < hh) {
            idx = 1;
        } else if (x < hw && y >= hh) {
            idx = 2;
        } else {
            idx = 3;
        }
    } else if (part_size == SIZE_2NxhN) {
        if (y < qh) {         // 1/4
            idx = 0;
        } else if (y < hh) {  // 2/4
            idx = 1;
        } else if (y < qh + hh) { // 3/4
            idx = 2;
        } else {
            idx = 3;
        }
    } else if (part_size == SIZE_hNx2N) {
        if (x < qw) {         // 1/4
            idx = 0;
        } else if (x < hw) {  // 2/4
            idx = 1;
        } else if (x < qw + hw) { // 3/4
            idx = 2;
        } else {
            idx = 3;
        }
    } else if (part_size == SIZE_2NxnU) {
        if (y < qh) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else if (part_size == SIZE_2NxnD) {
        if (y < qh + hh) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else if (part_size == SIZE_nLx2N) {
        if (x < qw) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else if (part_size == SIZE_nRx2N) {
        if (x < qw + hw) {
            idx = 0;
        } else {
            idx = 1;
        }
    } else {
        printf("\nError: part_size not expected");
        assert(0);
    }
    return idx;
}

void update_intra_info_map_scu(com_scu_t *map_scu, s8 *map_ipm, int tb_x, int tb_y, int tb_w, int tb_h, int i_scu, int ipm)
{
    int scu_x = PEL2SCU(tb_x);
    int scu_y = PEL2SCU(tb_y);
    int scu_w = PEL2SCU(tb_w);
    int scu_h = PEL2SCU(tb_h);

    map_scu = map_scu + scu_y * i_scu + scu_x;
    map_ipm = map_ipm + scu_y * i_scu + scu_x;

    com_scu_t scu = map_scu[0];
    scu.coded = 1;
    scu.intra = 1;

    for (int j = 0; j < scu_h; j++) {
        for (int i = 0; i < scu_w; i++) {
            map_scu[i] = scu;
            map_ipm[i] = ipm;
        }
        map_scu += i_scu;
        map_ipm += i_scu;
    }
}

int get_part_num(part_size_t size)
{
    switch (size) {
    case SIZE_2Nx2N:
        return 1;
    case SIZE_NxN:
        return 4;
    case SIZE_2NxnU:
        return 2;
    case SIZE_2NxnD:
        return 2;
    case SIZE_2NxhN:
        return 4;
    case SIZE_nLx2N:
        return 2;
    case SIZE_nRx2N:
        return 2;
    case SIZE_hNx2N:
        return 4;
    default:
        assert(0);
        return -1;
    }
}

int get_part_num_tb_in_pb(part_size_t pb_part_size, int pb_part_idx)
{
    switch (pb_part_size) {
    case SIZE_2NxnU:
    case SIZE_nLx2N:
        return pb_part_idx == 0 ? 1 : 3;
    case SIZE_2NxnD:
    case SIZE_nRx2N:
        return pb_part_idx == 0 ? 3 : 1;
    case SIZE_2NxhN:
    case SIZE_hNx2N:
    case SIZE_2Nx2N:
        return 1;
    default:
        assert(0);
        return -1;
    }
}

int get_tb_idx_offset(part_size_t pb_part_size, int pb_part_idx)
{
    switch (pb_part_size) {
    case SIZE_2NxnU:
    case SIZE_nLx2N:
        assert(pb_part_idx <= 1);
        return pb_part_idx == 0 ? 0 : 1;
    case SIZE_2NxnD:
    case SIZE_nRx2N:
        assert(pb_part_idx <= 1);
        return pb_part_idx == 0 ? 0 : 3;
    case SIZE_2NxhN:
    case SIZE_hNx2N:
        assert(pb_part_idx <= 3);
        return pb_part_idx;
    case SIZE_2Nx2N:
        assert(pb_part_idx == 0);
        return 0;
    default:
        assert(0);
        return -1;
    }
}


//note: this function only works for DT intra
void get_tb_width_height_in_pb(int pb_w, int pb_h, part_size_t pb_part_size, int pb_part_idx, int *tb_w, int *tb_h)
{
    switch (pb_part_size) {
    case SIZE_2NxnU:
        *tb_w = pb_w;
        *tb_h = pb_part_idx == 0 ? pb_h : pb_h / 3;
        break;
    case SIZE_2NxnD:
        *tb_w = pb_w;
        *tb_h = pb_part_idx == 1 ? pb_h : pb_h / 3;
        break;
    case SIZE_nLx2N:
        *tb_w = pb_part_idx == 0 ? pb_w : pb_w / 3;
        *tb_h = pb_h;
        break;
    case SIZE_nRx2N:
        *tb_w = pb_part_idx == 1 ? pb_w : pb_w / 3;
        *tb_h = pb_h;
        break;
    case SIZE_2NxhN:
    case SIZE_hNx2N:
    case SIZE_2Nx2N:
        *tb_w = pb_w;
        *tb_h = pb_h;
        break;
    default:
        assert(0);
        break;
    }
}

//note: this function only works for DT intra
void get_tb_pos_in_pb(int pb_x, int pb_y, part_size_t pb_part_size, int tb_w, int tb_h, int tb_part_idx, int *tb_x, int *tb_y)
{
    switch (pb_part_size) {
    case SIZE_2NxnU:
    case SIZE_2NxnD:
    case SIZE_2NxhN:
        *tb_x = pb_x;
        *tb_y = pb_y + tb_part_idx * tb_h;
        break;
    case SIZE_nLx2N:
    case SIZE_nRx2N:
    case SIZE_hNx2N:
        *tb_x = pb_x + tb_part_idx * tb_w;
        *tb_y = pb_y;
        break;
    case SIZE_2Nx2N:
        *tb_x = pb_x;
        *tb_y = pb_y;
        break;
    default:
        assert(0);
        break;
    }
}

part_size_t get_tb_part_size_by_pb(part_size_t pb_part, int pred_mode)
{
    part_size_t tb_part = 0;

    switch (pb_part) {
    case SIZE_2Nx2N:
        tb_part = pred_mode == MODE_INTRA ? SIZE_2Nx2N : SIZE_NxN;
        break;
    case SIZE_2NxnU:
    case SIZE_2NxnD:
    case SIZE_2NxhN:
        tb_part = SIZE_2NxhN;
        break;
    case SIZE_nLx2N:
    case SIZE_nRx2N:
    case SIZE_hNx2N:
        tb_part = SIZE_hNx2N;
        break;
    case SIZE_NxN:
        assert(0);
        tb_part = SIZE_NxN;
        break;
    default:
        assert(0);
        break;
    }

    return tb_part;
}

void get_tb_width_height_log2(int log2_w, int log2_h, part_size_t part, int *log2_tb_w, int *log2_tb_h)
{
    switch (part) {
    case SIZE_2Nx2N:
        break;
    case SIZE_NxN:
        log2_w--;
        log2_h--;
        break;
    case SIZE_2NxhN:
        log2_h -= 2;
        break;
    case SIZE_hNx2N:
        log2_w -= 2;
        break;
    default:
        assert(0);
        break;
    }

    *log2_tb_w = log2_w;
    *log2_tb_h = log2_h;
}

void get_tb_width_height(int w, int h, part_size_t part, int *tb_w, int *tb_h)
{
    switch (part) {
    case SIZE_2Nx2N:
        break;
    case SIZE_NxN:
        w >>= 1;
        h >>= 1;
        break;
    case SIZE_2NxhN:
        h >>= 2;
        break;
    case SIZE_hNx2N:
        w >>= 2;
        break;
    default:
        assert(0);
        break;
    }

    *tb_w = w;
    *tb_h = h;
}

void get_tb_start_pos(int w, int h, part_size_t part, int idx, int *pos_x, int *pos_y)
{
    int x = 0, y = 0;

    switch (part) {
    case SIZE_2Nx2N:
        x = y = 0;
        break;
    case SIZE_NxN:
        y = (idx / 2) * h / 2;
        x = (idx % 2) * w / 2;
        break;
    case SIZE_2NxhN:
        x = 0;
        y = idx * (h / 4);
        break;
    case SIZE_hNx2N:
        y = 0;
        x = idx * (w / 4);
        break;
    default:
        assert(0);
        break;
    }

    *pos_x = x;
    *pos_y = y;
}

int get_coef_offset_tb(int cu_x, int cu_y, int tb_x, int tb_y, int cu_w, int cu_h, int tb_part_size)
{
    int offset;
    switch (tb_part_size) {
    case SIZE_2Nx2N:
        offset = 0;
        break;
    case SIZE_NxN:
        if (tb_x == cu_x && tb_y == cu_y) {
            offset = 0;
        } else if (tb_x > cu_x && tb_y == cu_y) {
            offset = (cu_w * cu_h) / 4;
        } else if (tb_x == cu_x && tb_y > cu_y) {
            offset = (cu_w * cu_h) / 2;
        } else {
            offset = (cu_w * cu_h * 3) / 4;
        }
        break;
    case SIZE_2NxhN:
        offset = (tb_y - cu_y) * cu_w;
        break;
    case SIZE_hNx2N:
        offset = (tb_x - cu_x) * cu_h;
        break;
    default:
        assert(0);
        break;
    }

    return offset;
}

int is_tb_avaliable(com_info_t *info, int log2_w, int log2_h, part_size_t pb_part_size, int pred_mode)
{
    //intra: always infer TB part
    if (pred_mode == MODE_INTRA) {
        return 0;
    }

    //inter: signal for DT and PBT cases
    int avaliable = 0;
    if (info->sqh.pbt_enable && (pb_part_size == SIZE_2Nx2N && abs(log2_w - log2_h) <= 1 && log2_w <= 5 && log2_w >= 3 && log2_h <= 5 && log2_h >= 3)) {
        avaliable = 1;
    }
    return avaliable;
}

int is_cu_nz(int nz[MAX_NUM_TB][N_C])
{
    int cu_nz = 0;
    int i, j;

    for (i = 0; i < MAX_NUM_TB; i++) {
        for (j = 0; j < N_C; j++) {
            cu_nz |= nz[i][j];
        }
    }
    return cu_nz ? 1 : 0;
}

int is_cu_plane_nz(int nz[MAX_NUM_TB][N_C], int plane)
{
    int cu_nz = 0;
    int i;

    for (i = 0; i < MAX_NUM_TB; i++) {
        cu_nz |= nz[i][plane];
    }
    return cu_nz ? 1 : 0;
}

void cu_plane_nz_cpy(int dst[MAX_NUM_TB][N_C], int src[MAX_NUM_TB][N_C], int plane)
{
    int i;

    for (i = 0; i < MAX_NUM_TB; i++) {
        dst[i][plane] = src[i][plane];
    }
}

void cu_plane_nz_cln(int dst[MAX_NUM_TB][N_C], int plane)
{
    int i;

    for (i = 0; i < MAX_NUM_TB; i++) {
        dst[i][plane] = 0;
    }
}

int is_cu_nz_equ(int dst[MAX_NUM_TB][N_C], int src[MAX_NUM_TB][N_C])
{
    int i, j;
    int equ = 1;

    for (i = 0; i < N_C; i++) {
        for (j = 0; j < MAX_NUM_TB; j++) {
            if (dst[j][i] != src[j][i]) {
                equ = 0;
                break;
            }
        }
    }
    return equ;
}

void cu_nz_cln(int dst[MAX_NUM_TB][N_C])
{
    memset(dst, 0, sizeof(int) * MAX_NUM_TB * N_C);
}

void check_set_tb_part(com_mode_t *mode)
{
    if (!is_cu_plane_nz(mode->num_nz, Y_C)) {
        mode->tb_part = SIZE_2Nx2N;
    }
}

void check_tb_part(com_mode_t *mode)
{
    if (!is_cu_plane_nz(mode->num_nz, Y_C) && mode->tb_part != SIZE_2Nx2N) {
        com_assert(0);
    }
}

void copy_rec_y_to_pic(pel *src, int x, int y, int w, int h, int stride, com_pic_t *pic)
{
    pel *dst;
    int  j, s_pic;

    s_pic = pic->stride_luma;
    dst = pic->y + x + y * s_pic;
    for (j = 0; j < h; j++) {
        com_mcpy(dst, src, sizeof(pel) * w);
        src += stride;
        dst += s_pic;
    }
}

u8 is_use_cons(int w, int h, split_mode_t split, u8 slice_type)
{
    if (slice_type == SLICE_I) {
        return 0;
    } else {
        int s = w * h;
        if ((com_split_is_EQT(split) && s == 128) || ((com_split_is_BT(split) || split == SPLIT_QUAD) && s == 64)) {
            return 1;
        } else {
            return 0;
        }
    }
}


u8 com_tree_split(int w, int h, split_mode_t split, u8 slice_type)
{
    if (split == SPLIT_QUAD) {
        if (w == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if (split == SPLIT_EQT_HOR) {
        if (h == 16 || w == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if (split == SPLIT_EQT_VER) {
        if (w == 16 || h == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if (split == SPLIT_BI_HOR) {
        if (h == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if (split == SPLIT_BI_VER) {
        if (w == 8) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void set_pic_wq_matrix_by_param(int *param_vector, int mode, u8 *pic_wq_matrix4x4, u8 *pic_wq_matrix8x8)
{
    int i;
    for (i = 0; i < 16; i++) {
        pic_wq_matrix4x4[i] = param_vector[com_tbl_wq_model_4x4[mode][i]];
    }
    for (i = 0; i < 64; i++) {
        pic_wq_matrix8x8[i] = param_vector[com_tbl_wq_model_8x8[mode][i]];
    }
}

void init_pic_wq_matrix(u8 *pic_wq_matrix4x4, u8 *pic_wq_matrix8x8)
{
    int i;
    for (i = 0; i < 16; i++) {
        pic_wq_matrix4x4[i] = 64;
    }
    for (i = 0; i < 64; i++) {
        pic_wq_matrix8x8[i] = 64;
    }
}

static void affine_sobel_flt_hor(pel *pred, int i_pred, int *deriv, int i_deriv, int width, int height)
{
    int j, k;
    for (j = 1; j < height - 1; j++) {
        for (k = 1; k < width - 1; k++) {
            int center = j * i_pred + k;
            deriv[j *i_deriv + k] =
                pred[center + 1 - i_pred] -
                pred[center - 1 - i_pred] +
                (pred[center + 1] * 2) -
                (pred[center - 1] * 2) +
                pred[center + 1 + i_pred] -
                pred[center - 1 + i_pred];
        }
        deriv[j *i_deriv] = deriv[j * i_deriv + 1];
        deriv[j *i_deriv + width - 1] = deriv[j * i_deriv + width - 2];
    }
    deriv[0] = deriv[i_deriv + 1];
    deriv[width - 1] = deriv[i_deriv + width - 2];
    deriv[(height - 1) * i_deriv] = deriv[(height - 2) * i_deriv + 1];
    deriv[(height - 1) * i_deriv + width - 1] = deriv[(height - 2) * i_deriv + (width - 2)];
    for (j = 1; j < width - 1; j++) {
        deriv[j] = deriv[i_deriv + j];
        deriv[(height - 1) * i_deriv + j] = deriv[(height - 2) * i_deriv + j];
    }
}

static void affine_sobel_flt_ver(pel *pred, int i_pred, int *deriv, int i_deriv, int width, int height)
{
    int k, j;
    for (k = 1; k < width - 1; k++) {
        for (j = 1; j < height - 1; j++) {
            int center = j * i_pred + k;
            deriv[j *i_deriv + k] =
                pred[center + i_pred - 1] -
                pred[center - i_pred - 1] +
                (pred[center + i_pred] * 2) -
                (pred[center - i_pred] * 2) +
                pred[center + i_pred + 1] -
                pred[center - i_pred + 1];
        }
        deriv[k] = deriv[i_deriv + k];
        deriv[(height - 1) * i_deriv + k] = deriv[(height - 2) * i_deriv + k];
    }
    deriv[0] = deriv[i_deriv + 1];
    deriv[width - 1] = deriv[i_deriv + width - 2];
    deriv[(height - 1) * i_deriv] = deriv[(height - 2) * i_deriv + 1];
    deriv[(height - 1) * i_deriv + width - 1] = deriv[(height - 2) * i_deriv + (width - 2)];
    for (j = 1; j < height - 1; j++) {
        deriv[j *i_deriv] = deriv[j * i_deriv + 1];
        deriv[j *i_deriv + width - 1] = deriv[j * i_deriv + width - 2];
    }
}

static void affine_coef_computer(s16 *resi, int i_resi, int(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[7], int width, int height, int vertex_num)
{
    int affine_param_num = (vertex_num << 1);
    int j, k, col, row;
    for (j = 0; j != height; j++) {
        for (k = 0; k != width; k++) {
            s64 intermediates[2];
            int iC[6];
            int iIdx = j * i_deriv + k;
            if (vertex_num == 2) {
                iC[0] = deriv[0][iIdx];
                iC[1] = k * deriv[0][iIdx];
                iC[1] += j * deriv[1][iIdx];
                iC[2] = deriv[1][iIdx];
                iC[3] = j * deriv[0][iIdx];
                iC[3] -= k * deriv[1][iIdx];
            } else {
                iC[0] = deriv[0][iIdx];
                iC[1] = k * deriv[0][iIdx];
                iC[2] = deriv[1][iIdx];
                iC[3] = k * deriv[1][iIdx];
                iC[4] = j * deriv[0][iIdx];
                iC[5] = j * deriv[1][iIdx];
            }
            for (col = 0; col < affine_param_num; col++) {
                intermediates[0] = iC[col];
                for (row = 0; row < affine_param_num; row++) {
                    intermediates[1] = intermediates[0] * iC[row];
                    coef[col + 1][row] += intermediates[1];
                }
                intermediates[1] = intermediates[0] * resi[iIdx];
                coef[col + 1][affine_param_num] += intermediates[1] * 8;
            }
        }
    }
}

void com_recon_plane(part_size_t part, s16 *resi, pel *pred, int(*is_coef)[N_C], int plane, int cu_width, int cu_height, int s_rec, pel *rec, int bit_depth)
{
    int k, part_num = get_part_num(part);
    int tb_height, tb_width;

    get_tb_width_height(cu_width, cu_height, part, &tb_width, &tb_height);

    for (k = 0; k < part_num; k++) {
        int tb_x, tb_y;
        get_tb_start_pos(cu_width, cu_height, part, k, &tb_x, &tb_y);

        pel *p = pred + tb_y * cu_width + tb_x;
        pel *r = rec + tb_y * s_rec + tb_x;
        uavs3e_funs_handle.recon[CONV_LOG2(tb_width) - MIN_CU_LOG2](resi, p, cu_width, tb_width, tb_height, r, s_rec, is_coef[k][plane], bit_depth);

        resi += tb_width * tb_height;
    }
}

#define DEFINE_DIFF(w) \
    void com_pel_diff_##w(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height) { \
        while (height--) {                                                                                       \
            for (int i = 0; i < w; i++) {                                                                        \
                p_resi[i] = p_org[i] - p_pred[i];                                                                \
            }                                                                                                    \
            p_org  += i_org;                                                                                     \
            p_pred += i_pred;                                                                                    \
            p_resi += i_resi;                                                                                    \
        }                                                                                                        \
    }

DEFINE_DIFF(4)
DEFINE_DIFF(8)
DEFINE_DIFF(16)
DEFINE_DIFF(32)
DEFINE_DIFF(64)
DEFINE_DIFF(128)


#define DEFINE_AVRG(w) \
    void com_pel_avrg_##w(pel *dst, int i_dst, pel *src1, pel *src2, int height) { \
        while (height--) {                                                         \
            for (int i = 0; i < w; i++) {                                          \
                dst[i] = (src1[i] + src2[i] + 1) >> 1;                             \
            }                                                                      \
            src1 += w;                                                             \
            src2 += w;                                                             \
            dst  += i_dst;                                                         \
        }                                                                          \
    }

DEFINE_AVRG(4)
DEFINE_AVRG(8)
DEFINE_AVRG(16)
DEFINE_AVRG(32)
DEFINE_AVRG(64)
DEFINE_AVRG(128)

void com_recon(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
{
    int i, j;

    if (cbf == 0) {
        for (i = 0; i < height; i++) {
            memcpy(rec, pred, width * sizeof(pel));
            rec += i_rec;
            pred += i_pred;
        }
    } else {
        int max_val = (1 << bit_depth) - 1;

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                int t0 = *resi++ + pred[j];
                rec[j] = COM_CLIP3(0, max_val, t0);
            }
            pred += i_pred;
            rec += i_rec;
        }
    }
}

void uavs3e_funs_init_c()
{
    uavs3e_funs_init_intra_pred_c();
    uavs3e_funs_init_deblock_c();
    uavs3e_funs_init_sao_c();
    uavs3e_funs_init_alf_c();
    uavs3e_funs_init_mc_c();
    uavs3e_funs_init_itrans_c();
    uavs3e_funs_init_trans_c();
    uavs3e_funs_init_cost_c();

    uavs3e_funs_handle.affine_sobel_flt_hor = affine_sobel_flt_hor;
    uavs3e_funs_handle.affine_sobel_flt_ver = affine_sobel_flt_ver;
    uavs3e_funs_handle.affine_coef_computer = affine_coef_computer;

    uavs3e_funs_handle.pel_diff[0] = com_pel_diff_4;
    uavs3e_funs_handle.pel_diff[1] = com_pel_diff_8;
    uavs3e_funs_handle.pel_diff[2] = com_pel_diff_16;
    uavs3e_funs_handle.pel_diff[3] = com_pel_diff_32;
    uavs3e_funs_handle.pel_diff[4] = com_pel_diff_64;
    uavs3e_funs_handle.pel_diff[5] = com_pel_diff_128;

    uavs3e_funs_handle.pel_avrg[0] = com_pel_avrg_4;
    uavs3e_funs_handle.pel_avrg[1] = com_pel_avrg_8;
    uavs3e_funs_handle.pel_avrg[2] = com_pel_avrg_16;
    uavs3e_funs_handle.pel_avrg[3] = com_pel_avrg_32;
    uavs3e_funs_handle.pel_avrg[4] = com_pel_avrg_64;
    uavs3e_funs_handle.pel_avrg[5] = com_pel_avrg_128;

    for (int i = 0; i < CU_SIZE_NUM; i++) {
        uavs3e_funs_handle.recon[i] = com_recon;
    }

}