#include "arm64.h"
#if defined(__arm64__)
#ifndef __SAO_ARM64_H__
#define __SAO_ARM64_H__


void uavs3e_sao_eo_0_arm64(pel* src, pel* dst, int src_stride, int dst_stride, int* offset, int start_x, int end_x, int mb_height, pel* mask, int bit_depth);
void uavs3e_sao_eo_90_arm64(pel* src, pel* dst, int src_stride, int dst_stride, int* offset, int start_y, int end_y, int mb_width, int bit_depth);
void uavs3e_sao_eo_135_arm64(pel* src, pel* dst, int src_stride, int dst_stride, int* offset, pel* mask, int mb_height, int bit_depth, int start_x_r0, int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn);
void uavs3e_sao_eo_45_arm64(pel* src, pel* dst, int src_stride, int dst_stride, int* offset, pel* mask, int mb_height, int bit_depth, int start_x_r0, int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn);
void uavs3e_sao_bo_arm64(pel* src, pel* dst, int src_stride, int dst_stride, int* offset, int *band_ids, int mb_width, int mb_height, int bit_depth);

#endif
#endif