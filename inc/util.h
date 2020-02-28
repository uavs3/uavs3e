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

#ifndef _ENC_UTIL_H_
#define _ENC_UTIL_H_

#define GET_MVBITS_X(mv) (tab_mvbits_x[(mv) >> mvr_idx])
#define GET_MVBITS_Y(mv) (tab_mvbits_y[(mv) >> mvr_idx])
#define GET_MVBITS_IPEL_X(mv) (tab_mvbits_x[((mv) << 2) >> mvr_idx])
#define GET_MVBITS_IPEL_Y(mv) (tab_mvbits_y[((mv) << 2) >> mvr_idx])

u32 calc_satd_16b(int pu_w, int pu_h, pel *src1, pel *src2, int s_src1, int s_src2, int bit_depth);

static u32 avs3_always_inline block_pel_satd(int log2w, int log2h, pel *src1, pel *src2, int s_src1, int s_src2, int bit_depth)
{
    return com_had(1 << log2w, 1 << log2h, src1, src2, s_src1, s_src2, bit_depth);
}

static u64 avs3_always_inline block_pel_ssd(int log2w, int height, pel *src1, pel *src2, int s_src1, int s_src2, int bit_depth)
{
    int shift = (bit_depth - 8) << 1;
    return uavs3e_funs_handle.cost_ssd[log2w - 2](src1, s_src1, src2, s_src2, height) >> shift;
}

static u64 avs3_always_inline block_pel_sad(int pu_w, int pu_h, int shift, pel *src1, pel *src2, int s_src1, int s_src2, int bit_depth, int bi)
{
    return ((u64)uavs3e_funs_handle.cost_sad[CONV_LOG2(pu_w) - 2](src1, s_src1, src2, s_src2, pu_h)) << shift;
}

static void avs3_always_inline block_pel_sub(int log2w, int log2h, pel *src1, pel *src2, int s_src1, int s_src2, int s_diff, s16 *diff)
{
    uavs3e_funs_handle.pel_diff[log2w - 2](src1, s_src1, src2, s_src2, diff, s_diff, 1 << log2h);
}

void cu_pel_sub(u8 tree_status, int x, int y, int cu_width_log2, int cu_height_log2, com_pic_t *org, pel pred[N_C][MAX_CU_DIM], s16 diff[N_C][MAX_CU_DIM]);

int enc_create_cu_data(enc_cu_t *cu_data, int cu_width_log2, int cu_height_log2);
int enc_delete_cu_data(enc_cu_t *cu_data);

double enc_get_hgop_qp(double base_qp, int frm_depth, int is_ld);


#endif /* _ENC_UTIL_H_ */