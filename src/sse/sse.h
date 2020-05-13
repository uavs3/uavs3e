/**************************************************************************************
 * Copyright (c) 2018-2020 ["Peking University Shenzhen Graduate School",
 *   "Peng Cheng Laboratory", and "Guangdong Bohua UHD Innovation Corporation"]
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the <organization>.
 * 4. Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#ifndef __UAVS3D_SSE_H__
#define __UAVS3D_SSE_H__

#include <mmintrin.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>

#include "com_modules.h"

#ifdef _WIN32

#ifndef _WIN64
#define _mm_extract_epi64(a, i) (a.m128i_i64[i])
#endif

#endif

ALIGNED_32(extern pel uavs3e_simd_mask[15][16]);

void uavs3e_if_cpy_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w16_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w16x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);

void uavs3e_if_hor_chroma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w16_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w16x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w16_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w16x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_ver_chroma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w4_sse (const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w8_sse (const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);

void uavs3e_if_hor_luma_frame_sse    (const pel  *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth);
void uavs3e_if_ver_luma_frame_sse    (const pel  *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);
void uavs3e_if_ver_luma_frame_ext_sse(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);

void uavs3e_deblock_ver_luma_sse(pel *src, int stride, int alpha, int beta, int flt_flag);
void uavs3e_deblock_hor_luma_sse(pel *src, int stride, int alpha, int beta, int flt_flag);
void uavs3e_deblock_ver_chroma_sse(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag);
void uavs3e_deblock_hor_chroma_sse(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag);
void uavs3e_sao_on_lcu_sse(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *sao_params, int smb_pix_height,
                           int smb_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down, int sample_bit_depth);
void uavs3e_sao_on_lcu_chroma_sse(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *sao_params, int smb_pix_height,
                                  int smb_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down, int sample_bit_depth);
void uavs3e_alf_one_lcu_sse(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
void uavs3e_alf_calc_corr_sse(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail);

void uavs3e_pel_avrg_4_sse(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_8_sse(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_16_sse(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_32_sse(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_64_sse(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_128_sse(pel *dst, int i_dst, pel *src1, pel *src2, int height);

void uavs3e_padding_rows_luma_sse(pel *src, int i_src, int width, int height, int start, int rows, int padh, int padv);
void uavs3e_padding_rows_chroma_sse(pel *src, int i_src, int width, int height, int start, int rows, int padh, int padv);
void uavs3e_conv_fmt_8bit_sse(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16bit_sse(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16to8bit_sse(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);

void uavs3e_ipred_ipf_core_sse(pel *src, pel *dst, int i_dst, int ipm, int w, int h, int bit_depth);
void uavs3e_ipred_ipf_core_s16_sse(pel *src, pel *dst, int i_dst, s16 *pred, int ipm, int w, int h, int bit_depth);
void uavs3e_ipred_dc_sse(pel *src, pel *dst, int i_dst, int width, int height, u16 avail_cu, int bit_depth);
void uavs3e_ipred_plane_sse(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
void uavs3e_ipred_bi_sse(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
void uavs3e_ipred_hor_sse(pel *src, pel *dst, int i_dst, int width, int height);
void uavs3e_ipred_ver_sse(pel *src, pel *dst, int i_dst, int width, int height);

void uavs3e_ipred_ang_x_sse(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void uavs3e_ipred_ang_y_sse(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void uavs3e_ipred_ang_x_3_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_4_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_5_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_6_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_7_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_8_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_9_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_10_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_11_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);

void uavs3e_ipred_ang_xy_14_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_16_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_18_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_20_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_22_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);

void uavs3e_ipred_ang_y_25_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_26_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_27_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_28_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_29_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_30_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_31_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_32_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height);

void uavs3e_recon_w4_sse(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w8_sse(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w16_sse(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w32_sse(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w64_sse(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);

void uavs3e_dct2_butterfly_h8_sse(s16 *src, s16 *dst, int line, int shift, int bit_depth);
void uavs3e_dct2_butterfly_h16_sse(s16 *src, s16 *dst, int line, int shift, int bit_depth);
void uavs3e_dct2_butterfly_h32_sse(s16 *src, s16 *dst, int line, int shift, int bit_depth);

void uavs3e_itrans_dct2_h4_w4_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w8_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w16_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w32_sse(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h8_w4_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w8_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w16_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w32_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w64_sse(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h16_w4_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w8_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w16_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w32_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w64_sse(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h32_w4_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w8_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w16_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w32_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w64_sse(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h64_w8_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w16_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w32_sse(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w64_sse(s16 *src, s16 *dst, int bit_depth);

u32 uavs3e_get_sad_4_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_8_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_16_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_32_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_64_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_128_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

void uavs3e_get_sad_x3_4_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_8_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_16_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_32_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_64_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_128_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);

void uavs3e_get_sad_x4_4_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_8_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_16_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_32_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_64_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_128_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);

u64 uavs3e_get_ssd_4_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_8_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_16_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_32_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_64_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_128_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

u32 uavs3e_had_4x4_sse(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_4x8_sse(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_8x4_sse(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_8x8_sse(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_16x8_sse(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_8x16_sse(pel *org, int s_org, pel *cur, int s_cur);

void uavs3e_pel_diff_4_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_8_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_16_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_32_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_64_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_128_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);

void affine_sobel_flt_hor_sse(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
void affine_sobel_flt_ver_sse(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
void affine_coef_computer_sse(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[5], int width, int height);

#endif // #ifndef __INTRINSIC_H__