#ifndef __HM_INTELSSE_H__
#define __HM_INTELSSE_H__

#include "../common.h"

#define ENABLE_AVX512 0

#include <mmintrin.h>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

ALIGNED_16(extern char_t intrinsic_mask[15][16]);
ALIGNED_32(extern i16s_t intrinsic_mask_10bit[15][16]);

#if defined(_WIN32) && !defined(__GNUC__) 

#ifndef _WIN64
#define _mm_extract_epi64(a, i) (a.m128i_i64[i])
#endif

#define _mm256_extract_epi64(a, i) (a.m256i_i64[i])
#define _mm256_extract_epi32(a, i) (a.m256i_i32[i])
#define _mm256_extract_epi16(a, i) (a.m256i_i16[i])
#define _mm256_extract_epi8(a, i)  (a.m256i_i8 [i])
#define _mm256_insert_epi64(a, v, i) (a.m256i_i64[i] = v)

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

void avg_pel_0_sse128  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_2_sse128  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_4_sse128  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_8_sse128  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_16_sse128 (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_x16_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_1d_sse128 (pel_t *dst, pel_t *src1, pel_t *src2, int len);

void avg_pel_x16_sse256(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_1d_sse256(pel_t *dst, pel_t *src1, pel_t *src2, int len);

void com_mem_cpy16_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);
void com_mem_cpy32_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);
void com_mem_cpy64_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);

void com_mem_cpy32_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);
void com_mem_cpy64_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);

void padding_rows_sse128(pel_t *src, int i_src, int width, int height, int start, int rows, int pad);

i32u_t xGetSAD4_sse128   (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD8_sse128   (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD16_sse128  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD32_sse128  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD64_sse128  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD32_sse256  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD64_sse256  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);

i32u_t xGetAVGSAD4_sse128 (pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD8_sse128 (pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD16_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD32_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD64_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD32_sse256(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD64_sse256(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);

void xGetSAD4_x4_sse128 (pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD8_x4_sse128 (pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD16_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD32_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD64_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD16_x4_sse256(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD32_x4_sse256(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD64_x4_sse256(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);

i32u_t xGetSSE4_sse128   (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE8_sse128   (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE16_sse128  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE32_sse128  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE64_sse128  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE_Ext_sse128 (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight);
i64u_t xGetSSE_Psnr_sse128 (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight);

i32u_t xGetSSE16_sse256  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE32_sse256  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE64_sse256  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);

i32u_t xCalcHAD4x4_sse128 (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD8x8_sse128 (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD8x16_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD8x16_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD16x16_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD32x32_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD64x64_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);

i32u_t xCalcHADs8x8_I_sse128(pel_t *p_org, int i_org);

void xPredIntraAngAdi_X_3_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_5_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_7_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_9_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_11_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void xPredIntraAngAdi_X_4_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_6_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_8_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_10_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void xPredIntraAngAdi_Y_25_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_27_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_29_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_31_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void xPredIntraAngAdi_Y_26_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_28_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_30_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_32_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void xPredIntraAngAdi_XY_14_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_16_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_18_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_20_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_22_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void xPredIntraVertAdi_sse128 (pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight);
void xPredIntraHorAdi_sse128  (pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight);
void xPredIntraDCAdi_sse128   (pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail, int bit_depth);
void xPredIntraPlaneAdi_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth);
void xPredIntraBiAdi_sse128   (pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth);

void com_if_filter_hor_4_sse128     (const pel_t  *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int bit_depth);
void com_if_filter_ver_4_sse128     (const pel_t  *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int bit_depth);
void com_if_filter_hor_ver_4_sse128 (const pel_t  *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff_h, const char_t *coeff_v, int bit_depth);
void com_if_filter_hor_8_sse128     (const pel_t  *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_sse128     (const pel_t  *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_ext_sse128 (const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);

void com_if_filter_hor_4_w16_sse256     (const pel_t  *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_ver_4_w16_sse256     (const pel_t  *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_ver_4_w32_sse256     (const pel_t  *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_hor_ver_4_w16_sse256 (const pel_t  *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth);
void com_if_filter_hor_8_sse256         (const pel_t  *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_sse256         (const pel_t  *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_ext_sse256     (const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);

int  quant_ext_sse128(coef_t *curr_blk, int size, int Q, int qp_const, int shift);

int  quant_normal_sse128(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift);
void inv_quant_normal_sse128(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift);
void inv_quant_ext_sse128(coef_t *src, coef_t *dst, int size, int QPI, int shift);
int  add_sign_sse128(coef_t *dat, i16u_t *sign, int len);
int  pre_quant_sse128(coef_t *curr_blk, i16u_t *abs_blk, int len, int Q_threshold);
void inv_quant_normal_sse256(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift);
void inv_quant_normal_sse512(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift);

int get_cg_bits_sse128(coef_t* coef);

void zero_cg_check8_sse128(coef_t *coef, i64u_t *cg_mask);
void zero_cg_check16_sse256(coef_t *coef, i64u_t *cg_mask);
void zero_cg_check32_sse256(coef_t *coef, i64u_t *cg_mask);

int  add_sign_sse256(coef_t *dat, i16u_t *abs_val, int len);
int  pre_quant_sse256(coef_t *curr_blk, i16u_t *abs_blk, int len, int Q_threshold);
int  quant_normal_sse256(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift);
int  quant_normal_sse512(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift);
int  quant_ext_sse256(coef_t *curr_blk, int size, int Q, int qp_const, int shift);

void deblock_luma_ver_sse128(pel_t *SrcPtr, int stride, int Alpha, int Beta);
void deblock_luma_hor_sse128(pel_t *SrcPtr, int stride, int Alpha, int Beta);
void deblock_chroma_ver_sse128(pel_t *SrcPtrU, pel_t *SrcPtrV, int stride, int AlphaU, int BetaU, int AlphaV, int BetaV);
void deblock_chroma_hor_sse128(pel_t *SrcPtrU, pel_t *SrcPtrV, int stride, int AlphaU, int BetaU, int AlphaV, int BetaV);

void SAO_on_block_sse128(void *handle, void* sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height, int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down);
void sao_get_stats_sse128(void *handle, void* sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height, int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down);

void add_inv_trans_4x4_sec_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_4x4_sse128    (coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_8x8_sse128    (coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_16x16_sse128  (coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_32x32_sse128  (coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_64x64_sse128  (coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);

void sub_trans_4x4_sec_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_4x4_sse128    (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_8x8_sse128    (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_16x16_sse128  (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_32x32_sse128  (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_64x64_sse128  (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);

void add_inv_trans_ext_32x32_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_ext_64x64_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);

void sub_trans_ext_32x32_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_ext_64x64_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);

void xTr2nd_8_1d_Inv_Hor_sse128(coef_t *src, int i_src);
void xTr2nd_8_1d_Inv_Vert_sse128(coef_t *src, int i_src);
void xTr2nd_8_1d_Hor_sse128(coef_t *src, int i_src);
void xTr2nd_8_1d_Vert_sse128(coef_t *src, int i_src);

void sub_trans_16x16_sse256    (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_32x32_sse256    (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_64x64_sse256    (pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_ext_32x32_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_ext_64x64_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);

void add_inv_trans_16x16_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_32x32_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_64x64_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_ext_32x32_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_ext_64x64_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);

void trans_dct8_pb4_avx2 (coef_t *src, coef_t *dst, int shift);
void trans_dct8_pb8_avx2 (coef_t *src, coef_t *dst, int shift);
void trans_dct8_pb16_avx2(coef_t *src, coef_t *dst, int shift);
void trans_dst7_pb4_avx2 (coef_t *src, coef_t *dst, int shift);
void trans_dst7_pb8_avx2 (coef_t *src, coef_t *dst, int shift);
void trans_dst7_pb16_avx2(coef_t *src, coef_t *dst, int shift);

void uavs3e_itrans_dct8_pb4_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dct8_pb8_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dct8_pb16_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dst7_pb4_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dst7_pb8_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val);
void uavs3e_itrans_dst7_pb16_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val);

double calc_ave_sse128(pel_t* p_org, int width, int height, int iDownScale, int bit_depth);
double calc_var_sse128(pel_t* p_org, int width, int height, int iDownScale, double ave, int bit_depth);

i32u_t pixel_var_8_sse128(const pel_t* pix, int i_pix);
i32u_t pixel_var_16_sse128(const pel_t* pix, int i_pix);

void pix_sub_b4_sse256 (resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);
void pix_sub_b8_sse256 (resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);
void pix_sub_b16_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);
void pix_sub_b32_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);

void pix_add_b4_sse256 (pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);
void pix_add_b8_sse256 (pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);
void pix_add_b16_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);
void pix_add_b32_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);

int get_nz_num_sse256(coef_t *p_coef, int num_coeff);
int pre_quant_rdoq_sse256(i16s_t *coef, int num, int q_value, int q_bits, int err_scale, i32u_t* abs_coef, i32u_t* abs_level, long long *uncoded_err);

#endif // __HM_INTELSSE_H__