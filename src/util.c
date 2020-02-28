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
#include "define.h"
#include "util.h"

void cu_pel_sub(u8 tree_status, int x, int y, int cu_width_log2, int cu_height_log2, com_pic_t *org, pel pred[N_C][MAX_CU_DIM], s16 diff[N_C][MAX_CU_DIM])
{
    pel *buf;
    int cu_width, cu_height, stride;
    cu_width   = 1 << cu_width_log2;
    cu_height   = 1 << cu_height_log2;
    stride = org->stride_luma;
    /* Y */
    if (tree_status != TREE_C) {
        buf = org->y + (y * stride) + x;
        block_pel_sub(cu_width_log2, cu_height_log2, buf, pred[Y_C], stride, cu_width, cu_width, diff[Y_C]);
    }
    if (tree_status != TREE_L) {
        cu_width >>= 1;
        cu_height >>= 1;
        x >>= 1;
        y >>= 1;
        cu_width_log2--;
        cu_height_log2--;
        stride = org->stride_chroma;
        /* U */
        buf = org->u + (y * stride) + x;
        block_pel_sub(cu_width_log2, cu_height_log2, buf, pred[U_C], stride, cu_width, cu_width, diff[U_C]);
        /* V */
        buf = org->v + (y * stride) + x;
        block_pel_sub(cu_width_log2, cu_height_log2, buf, pred[V_C], stride, cu_width, cu_width, diff[V_C]);
    }
}

double enc_get_hgop_qp(double base_qp, int frm_depth, int is_ld)
{
    static const enc_aqp_param_t tbl_qp_adapt_param_ra[8] = {
        { -4,  0,      0 },
        {  1,  0,      0 },
        {  1, -4.0604, 0.154575 },
        {  5, -4.8332, 0.17145 },
        {  7, -4.9668, 0.174975 },
        {  8, -5.9444, 0.225 },
        {  9, -5.9444, 0.225 },
        { 10, -5.9444, 0.225 }
    };
    static const enc_aqp_param_t tbl_qp_adapt_param_ld[8] = {
        { -1,  0.0000, 0.0000},
        { 1,  0.0000, 0.0000},
        { 4, -6.5000, 0.2590},
        { 5, -6.5000, 0.2590},
        { 6, -6.5000, 0.2590},
        { 7, -6.5000, 0.2590},
        { 8, -6.5000, 0.2590},
        { 9, -6.5000, 0.2590},
    };

    const enc_aqp_param_t *qp_adapt_param = is_ld ? tbl_qp_adapt_param_ld : tbl_qp_adapt_param_ra;
    base_qp += qp_adapt_param[frm_depth].qp_offset_layer;

    double dqp_offset = COM_MAX(0, base_qp) * qp_adapt_param[frm_depth].qp_offset_model_scale + qp_adapt_param[frm_depth].qp_offset_model_offset;
    return base_qp + COM_CLIP3(0.0, 4.0, dqp_offset);
}

static void enc_malloc_1d(void **dst, int size)
{
    if (*dst == NULL) {
        *dst = com_malloc(size);
        com_mset(*dst, 0, size);
    }
}

int enc_create_cu_data(enc_cu_t *cu_data, int cu_width_log2, int cu_height_log2)
{
    int i;
    int cuw_scu, cuh_scu;
    int size_8b, size_16b, size_32b, cu_cnt, pixel_cnt;
    cuw_scu = 1 << cu_width_log2;
    cuh_scu = 1 << cu_height_log2;
    size_8b  = cuw_scu * cuh_scu * sizeof(s8);
    size_16b = cuw_scu * cuh_scu * sizeof(s16);
    size_32b = cuw_scu * cuh_scu * sizeof(s32);
    cu_cnt    = cuw_scu * cuh_scu;
    pixel_cnt = cu_cnt << 4;
    
    enc_malloc_1d((void **)&cu_data->split_mode, size_8b * MAX_CU_DEPTH * NUM_BLOCK_SHAPE);
    enc_malloc_1d((void **)&cu_data->pred_mode, size_8b);
    enc_malloc_1d((void **)&cu_data->ipf_flag, size_8b);
    enc_malloc_1d((void **)&cu_data->umve_flag, size_8b);
    enc_malloc_1d((void **)&cu_data->umve_idx, size_8b);
    enc_malloc_1d((void **)&cu_data->hmvp_flag, size_8b);
    enc_malloc_1d((void **)&cu_data->mpm, size_16b);
    enc_malloc_1d((void **)&cu_data->ipm_l, size_8b);
    enc_malloc_1d((void **)&cu_data->ipm_c, size_8b);
    enc_malloc_1d((void **)&cu_data->refi, 2 * cu_cnt * REFP_NUM * sizeof(u8));
    enc_malloc_1d((void **)&cu_data->mvr_idx, size_8b);
    enc_malloc_1d((void **)&cu_data->skip_idx, size_8b);
    for (i = 0; i < N_C; i++) {
        enc_malloc_1d((void **)&cu_data->num_nz_coef[i], size_32b);
    }
    enc_malloc_1d((void **)&cu_data->pb_part, size_32b);
    enc_malloc_1d((void **)&cu_data->tb_part, size_32b);
    enc_malloc_1d((void **)&cu_data->map_scu, cuw_scu * cuh_scu * sizeof(com_scu_t));
    enc_malloc_1d((void **)&cu_data->affine_flag, size_8b);
    enc_malloc_1d((void **)&cu_data->smvd_flag, size_8b);
    enc_malloc_1d((void **)&cu_data->map_pos, size_32b);
    enc_malloc_1d((void **)&cu_data->mv, size_16b * REFP_NUM * MV_D);
    enc_malloc_1d((void **)&cu_data->mvd, size_16b * REFP_NUM * MV_D);

    for (i = 0; i < N_C; i++) {
        enc_malloc_1d((void **)&cu_data->coef[i], (pixel_cnt >> (!!(i) * 2)) * sizeof(s16));
        enc_malloc_1d((void **)&cu_data->reco[i], (pixel_cnt >> (!!(i) * 2)) * sizeof(pel));
    }
    return COM_OK;
}

static void enc_free_1d(void *dst)
{
    if (dst != NULL) {
        com_mfree(dst);
    }
}

int enc_delete_cu_data(enc_cu_t *cu_data)
{
    int i;

    enc_free_1d((void *)cu_data->split_mode);
    enc_free_1d((void *)cu_data->pred_mode);
    enc_free_1d((void *)cu_data->ipf_flag);
    enc_free_1d((void *)cu_data->umve_flag);
    enc_free_1d((void *)cu_data->umve_idx);
    enc_free_1d(cu_data->hmvp_flag);
    enc_free_1d((void *)cu_data->mpm);
    enc_free_1d((void *)cu_data->ipm_l);
    enc_free_1d((void *)cu_data->ipm_c);
    enc_free_1d((void *)cu_data->refi);
    enc_free_1d(cu_data->mvr_idx);
    enc_free_1d(cu_data->skip_idx);
    for (i = 0; i < N_C; i++) {
        enc_free_1d((void *)cu_data->num_nz_coef[i]);
    }
    enc_free_1d((void *)cu_data->pb_part);
    enc_free_1d((void *)cu_data->tb_part);
    enc_free_1d((void *)cu_data->map_scu);
    enc_free_1d((void *)cu_data->affine_flag);
    enc_free_1d((void *)cu_data->smvd_flag);
    enc_free_1d((void *)cu_data->map_pos);
    enc_free_1d((void *)cu_data->mv);
    enc_free_1d((void *)cu_data->mvd);

    for (i = 0; i < N_C; i++) {
        enc_free_1d((void *)cu_data->coef[i]);
        enc_free_1d((void *)cu_data->reco[i]);
    }
    return COM_OK;
}

u32 calc_satd_16b(int pu_w, int pu_h, pel *src1, pel *src2, int s_src1, int s_src2, int bit_depth)
{
    u32 cost = 0;
    int num_seg_in_pu_w = 1, num_seg_in_pu_h = 1;
    int subblk_w_log2 = com_tbl_log2[pu_w];
    int subblk_h_log2 = com_tbl_log2[pu_h];
    pel *src1_seg, *src2_seg;
    pel *s1 = (pel *)src1;
    pel *s2 = (pel *)src2;

    if (subblk_w_log2 == -1) {
        num_seg_in_pu_w = 3;
        subblk_w_log2 = (pu_w == 48) ? 4 : (pu_w == 24 ? 3 : 2);
    }
    if (subblk_h_log2 == -1) {
        num_seg_in_pu_h = 3;
        subblk_h_log2 = (pu_h == 48) ? 4 : (pu_h == 24 ? 3 : 2);
    }
    if (num_seg_in_pu_w == 1 && num_seg_in_pu_h == 1) {
        cost += block_pel_satd(subblk_w_log2, subblk_h_log2, s1, s2, s_src1, s_src2, bit_depth);
        return cost;
    }
    for (int j = 0; j < num_seg_in_pu_h; j++) {
        for (int i = 0; i < num_seg_in_pu_w; i++) {
            src1_seg = s1 + (1 << subblk_w_log2) * i + (1 << subblk_h_log2) * j * s_src1;
            src2_seg = s2 + (1 << subblk_w_log2) * i + (1 << subblk_h_log2) * j * s_src2;
            cost += block_pel_satd(subblk_w_log2, subblk_h_log2, src1_seg, src2_seg, s_src1, s_src2, bit_depth);
        }
    }
    return cost;
}