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
 *    This product includes the software uAVS3d developed by
 *    Peking University Shenzhen Graduate School, Peng Cheng Laboratory
 *    and Guangdong Bohua UHD Innovation Corporation.
 * 4. Neither the name of the organizations (Peking University Shenzhen Graduate School,
 *    Peng Cheng Laboratory and Guangdong Bohua UHD Innovation Corporation) nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#ifndef __UAVS3D_AVX2_H__
#define __UAVS3D_AVX2_H__

#include <mmintrin.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#include "com_modules.h"

#if defined(_WIN32) && !defined(__GNUC__)

#define _mm256_extract_epi64(a, i) (a.m256i_i64[i])
#define _mm256_extract_epi32(a, i) (a.m256i_i32[i])
#define _mm256_extract_epi16(a, i) (a.m256i_i16[i])
#define _mm256_extract_epi8(a, i)  (a.m256i_i8 [i])
#define _mm256_insert_epi64(a, v, i) (a.m256i_i64[i] = v)
#define _mm_extract_epi64(r, i) r.m128i_i64[i]

#else

#define _mm256_set_m128i(/* __m128i */ hi, /* __m128i */ lo)  _mm256_insertf128_si256(_mm256_castsi128_si256(lo), (hi), 0x1)
#define _mm256_loadu2_m128i(/* __m128i const* */ hiaddr, /* __m128i const* */ loaddr) _mm256_set_m128i(_mm_loadu_si128(hiaddr), _mm_loadu_si128(loaddr))

#define _mm256_storeu2_m128i(/* __m128i* */ hiaddr, /* __m128i* */ loaddr, \
        /* __m256i */ a) \
do { \
    __m256i _a = (a); /* reference a only once in macro body */ \
    _mm_storeu_si128((loaddr), _mm256_castsi256_si128(_a)); \
    _mm_storeu_si128((hiaddr), _mm256_extractf128_si256(_a, 0x1)); \
} while (0)

#endif

void uavs3e_conv_fmt_8bit_avx2(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16bit_avx2(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16to8bit_avx2(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);

void uavs3e_sao_on_lcu_avx2(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *saoBlkParam, int smb_pix_height, int smb_pix_width,
                            int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down, int sample_bit_depth);
void uavs3e_sao_on_lcu_chroma_avx2(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *saoBlkParam, int smb_pix_height, int smb_pix_width,
                                   int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down, int sample_bit_depth);

void uavs3e_alf_one_lcu_avx2(pel *dst, int i_dst, pel *src, int stride, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
void uavs3e_alf_calc_corr_avx2(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail);

void uavs3e_if_cpy_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w64_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w128_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);

void uavs3e_if_hor_chroma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);

void uavs3e_if_hor_luma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);

void uavs3e_if_ver_chroma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w64_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w128_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);

void uavs3e_if_ver_luma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w64_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w128_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);

void uavs3e_if_hor_ver_chroma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);

void uavs3e_if_hor_luma_frame_avx2(const pel  *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth);
void uavs3e_if_ver_luma_frame_avx2(const pel  *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);
void uavs3e_if_ver_luma_frame_ext_avx2(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);


void uavs3e_conv_fmt_8bit_avx2(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16bit_avx2(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16to8bit_avx2(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);

void uavs3e_ipred_ver_avx2(pel *src, pel *dst, int i_dst, int width, int height);
void uavs3e_ipred_hor_avx2(pel *src, pel *dst, int i_dst, int width, int height);
void uavs3e_ipred_dc_avx2(pel *src, pel *dst, int i_dst, int width, int height, u16 avail_cu, int bit_depth);
void uavs3e_ipred_ang_x_avx2(pel *pSrc, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_4_avx2(pel *pSrc, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_6_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_8_avx2(pel *pSrc, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_10_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_avx2(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void uavs3e_ipred_ang_y_26_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_28_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_30_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_32_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_13_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_14_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_15_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_16_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_17_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_18_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_19_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_20_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_21_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_22_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_23_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_offsets_seteps_init();

void uavs3e_recon_w8_avx2(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w16_avx2(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w32_avx2(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w64_avx2(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);

void uavs3e_dquant_avx2(s16 *src, s16 *dst, u8 *wq_matrix[2], int log2_w, int log2_h, int scale, int shift, int bit_depth);

void uavs3e_itrans_dct2_h4_w8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w32_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h8_w4_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w64_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h16_w4_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w64_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h32_w4_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w64_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h64_w8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w64_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct8_pb4_avx2(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dct8_pb8_avx2(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dct8_pb16_avx2(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dst7_pb4_avx2(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dst7_pb8_avx2(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dst7_pb16_avx2(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);

void uavs3e_trans_dct2_w4_h4_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w4_h8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w4_h16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w4_h32_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w8_h4_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h64_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w16_h4_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h64_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w32_h4_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h64_avx2(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w64_h8_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w64_h16_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w64_h32_avx2(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w64_h64_avx2(s16 *src, s16 *dst, int bit_depth);

void trans_dct8_pb4_avx2(s16 *src, s16 *dst, int shift, int line);
void trans_dct8_pb8_avx2(s16 *src, s16 *dst, int shift, int line);
void trans_dct8_pb16_avx2(s16 *src, s16 *dst, int shift, int line);
void trans_dst7_pb4_avx2(s16 *src, s16 *dst, int shift, int line);
void trans_dst7_pb8_avx2(s16 *src, s16 *dst, int shift, int line);
void trans_dst7_pb16_avx2(s16 *src, s16 *dst, int shift, int line);

u32 uavs3e_get_sad_4_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_32_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_64_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_128_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

void uavs3e_get_sad_x3_4_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_8_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_16_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_32_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_64_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_128_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);

void uavs3e_get_sad_x4_4_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);
void uavs3e_get_sad_x4_8_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);
void uavs3e_get_sad_x4_16_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);
void uavs3e_get_sad_x4_32_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);
void uavs3e_get_sad_x4_64_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);
void uavs3e_get_sad_x4_128_avx2(pel *p_org, int i_org, pel *p_pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);

u64 uavs3e_get_ssd_4_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_32_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_64_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_128_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

void uavs3e_pel_diff_16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_32_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_64_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_128_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);

void uavs3e_pel_avrg_4_avx2(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_8_avx2(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_16_avx2(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_32_avx2(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_64_avx2(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_128_avx2(pel *dst, int i_dst, pel *src1, pel *src2, int height);

int quant_check_avx2(s16 *coef, int num, int threshold);
int quant_rdoq_avx2(s16 *coef, int num, int q_value, int q_bits, s32 err_scale, int precision_bits, u32* abs_coef, s16* abs_level, s64 *uncoded_err);

u32 uavs3e_had_4x8_avx2(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_8x4_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred);
u32 uavs3e_had_8x8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred);
u32 uavs3e_had_16x8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred);
u32 uavs3e_had_8x16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred);

void affine_sobel_flt_hor_avx2(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
void affine_sobel_flt_ver_avx2(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
void affine_coef_computer_avx2(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[5], int width, int height);

#endif