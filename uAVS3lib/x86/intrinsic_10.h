#include "intrinsic.h"

#define com_mem_cpy8_sse128_10bit  com_mem_cpy16_sse128
#define com_mem_cpy16_sse128_10bit com_mem_cpy32_sse128
#define com_mem_cpy32_sse128_10bit com_mem_cpy64_sse128
void    com_mem_cpy64_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);
#define com_mem_cpy16_sse256_10bit com_mem_cpy32_sse256
#define com_mem_cpy32_sse256_10bit com_mem_cpy64_sse256
void    com_mem_cpy64_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);

void padding_rows_sse128_10bit(pel_t *src, int i_src, int width, int height, int start, int rows, int pad);

void avg_pel_0_sse128_10bit  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_2_sse128_10bit  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_4_sse128_10bit  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_8_sse128_10bit  (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_16_sse128_10bit (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_x16_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_1d_sse128_10bit (pel_t *dst, pel_t *src1, pel_t *src2, int len);

void avg_pel_16_sse256_10bit (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_16_sse512_10bit (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_x16_sse256_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_32_sse512_10bit (pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
void avg_pel_1d_sse256_10bit (pel_t *dst, pel_t *src1, pel_t *src2, int len);

i32u_t xGetSAD4_sse128_10bit (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD8_sse128_10bit (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD16_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines); 
i32u_t xGetSAD32_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD64_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
i32u_t xGetSAD32_sse512_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);

i32u_t xGetSSE4_sse128_10bit   (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE8_sse128_10bit   (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE16_sse128_10bit  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE32_sse128_10bit  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE64_sse128_10bit  (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE_Ext_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight);
i64u_t xGetSSE_Psnr_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight);
i32u_t xGetSSE16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
i32u_t xGetSSE64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);

void xGetSAD4_x4_sse128_10bit (pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD8_x4_sse128_10bit (pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD16_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD32_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD64_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD16_x4_sse256_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD32_x4_sse256_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD64_x4_sse256_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
void xGetSAD32_x4_sse512_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);

i32u_t xGetAVGSAD4_sse128_10bit (pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD8_sse128_10bit (pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD16_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD32_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD64_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);
i32u_t xGetAVGSAD64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);

i32u_t xCalcHAD4x4_sse128_10bit (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD8x8_sse128_10bit (pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD8x16_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD8x16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);

i32u_t xCalcHAD16x16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD32x32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
i32u_t xCalcHAD64x64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);

i32u_t xCalcHADs8x8_I_sse128_10bit(pel_t *p_org, int i_org);

i32u_t xCalcHAD16x16_sse512_10bit(pel_t *org, int i_org, pel_t *pred, int i_pred);
i32u_t xCalcHAD32x32_sse512_10bit(pel_t *org, int i_org, pel_t *pred, int i_pred);

void com_if_filter_hor_4_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_ver_4_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_hor_ver_4_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth);
void com_if_filter_ver_4_w16_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_ver_4_w32_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_hor_4_w8_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
void com_if_filter_hor_ver_4_w16_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth);

void com_if_filter_hor_8_sse128_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_sse128_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_ext_sse128_10bit(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_hor_8_sse256_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_ext_sse256_10bit(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_sse256_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
void com_if_filter_ver_8_ext_sse512_10bit(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);

void xPredIntraDCAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail, int bit_depth);
void xPredIntraVertAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight);
void xPredIntraHorAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight);
void xPredIntraPlaneAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth);
void xPredIntraBiAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth);

double calc_ave_sse128_10bit(pel_t* p_org, int width, int height, int iDownScale, int bit_depth);
double calc_var_sse128_10bit(pel_t* p_org, int width, int height, int iDownScale, double ave, int bit_depth);

void add_inv_trans_4x4_sec_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_4x4_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_8x8_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_16x16_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_32x32_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_64x64_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);

void add_inv_trans_ext_32x32_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_ext_64x64_10bit_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);

void add_inv_trans_16x16_10bit_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_32x32_10bit_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_64x64_10bit_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_ext_32x32_10bit_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
void add_inv_trans_ext_64x64_10bit_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);

void sub_trans_4x4_sec_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_4x4_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_8x8_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_16x16_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_32x32_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_64x64_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);

void sub_trans_ext_32x32_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_ext_64x64_10bit_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);

void sub_trans_16x16_10bit_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_32x32_10bit_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_64x64_10bit_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_ext_32x32_10bit_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
void sub_trans_ext_64x64_10bit_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);

void sao_get_stats_sse128_10bit(void *handle, void *sao_data, int compIdx, const int pix_y, const int pix_x, int lcu_pix_height, int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down);
void SAO_on_block_sse128_10bit(void *handle, void* sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height,	int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down);

void xPredIntraAngAdi_X_3_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_4_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_5_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_6_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_7_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_8_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_9_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_10_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_X_11_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_14_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_16_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_18_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_20_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_XY_22_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_25_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_26_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_27_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_28_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_29_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_30_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_31_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void xPredIntraAngAdi_Y_32_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void deblock_luma_ver_sse128_10bit(pel_t *SrcPtr, int stride, int Alpha, int Beta);
void deblock_luma_hor_sse128_10bit(pel_t *SrcPtr, int stride, int Alpha, int Beta);
void deblock_chroma_ver_sse128_10bit(pel_t *SrcPtrU, pel_t *SrcPtrV, int stride, int AlphaU, int BetaU, int AlphaV, int BetaV);
void deblock_chroma_hor_sse128_10bit(pel_t *SrcPtrU, pel_t *SrcPtrV, int stride, int AlphaU, int BetaU, int AlphaV, int BetaV);

i32u_t pixel_var_8_10bit_sse128(const pel_t* pix, int i_pix);
i32u_t pixel_var_16_10bit_sse128(const pel_t* pix, int i_pix);

void pix_sub_b4_10bit_sse256 (resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);
void pix_sub_b8_10bit_sse256 (resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);
void pix_sub_b16_10bit_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);
void pix_sub_b32_10bit_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);

void pix_add_b4_10bit_sse256 (pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);
void pix_add_b8_10bit_sse256 (pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);
void pix_add_b16_10bit_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);
void pix_add_b32_10bit_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);
