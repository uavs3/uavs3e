#if defined(__aarch64__) && !defined(__arm64__)
#define __arm64__ 1
#endif

#if defined(__arm64__)
#ifndef __ARM64_H__
#define __ARM64_H__
#include "../../inc/com_util.h"
#include "../../inc/com_define.h"

void uavs3e_if_cpy_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w64_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
void uavs3e_if_cpy_w128_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);

void uavs3e_if_hor_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w16x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_chroma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w16x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_hor_luma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);

void uavs3e_if_ver_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w64_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_chroma_w128_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w16x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
void uavs3e_if_ver_luma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);

void uavs3e_if_hor_ver_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_chroma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);
void uavs3e_if_hor_ver_luma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);

//void uavs3e_if_hor_luma_frame_arm64(const pel  *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth);
//void uavs3e_if_ver_luma_frame_arm64(const pel  *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);
//void uavs3e_if_ver_luma_frame_ext_arm64(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);

void uavs3e_deblock_ver_luma_arm64(pel *src, int stride, int alpha, int beta, int flt_flag);
void uavs3e_deblock_hor_luma_arm64(pel *src, int stride, int alpha, int beta, int flt_flag);
void uavs3e_deblock_ver_chroma_arm64(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag);
void uavs3e_deblock_hor_chroma_arm64(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag);
void uavs3e_sao_on_lcu_arm64(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *sao_params, int height,
                             int width, int available_left, int available_right, int available_up, int available_down, int sample_bit_depth);
void uavs3e_sao_on_lcu_chroma_arm64(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *sao_params, int smb_pix_height,
                                  int smb_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down, int sample_bit_depth);
void uavs3e_alf_filter_block_arm64(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
void uavs3e_alf_calc_corr_arm64(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail);

void uavs3e_pel_avrg_4_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_8_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_16_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_32_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_64_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
void uavs3e_pel_avrg_128_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);

void uavs3e_padding_rows_luma_arm64(pel *src, int i_src, int width, int height, int start, int rows, int padh, int padv);
void uavs3e_padding_rows_chroma_arm64(pel *src, int i_src, int width, int height, int start, int rows, int padh, int padv);
void uavs3e_conv_fmt_8bit_arm64(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16bit_arm64(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);
void uavs3e_conv_fmt_16to8bit_arm64(unsigned char *src_y, unsigned char *src_uv, unsigned char *dst[3], int width, int height, int src_stride, int src_stridec, int dst_stride[3], int uv_shift);

void uavs3e_ipred_ipf_core_arm64(pel *src, pel *dst, int i_dst, int ipm, int w, int h, int bit_depth);
void uavs3e_ipred_ipf_core_s16_arm64(pel *src, pel *dst, int i_dst, s16 *pred, int ipm, int w, int h, int bit_depth);
void uavs3e_intra_pred_dc_arm64(pel *src, pel *dst, int i_dst, int width, int height, u16 avail_cu, int bit_depth);
void uavs3e_intra_pred_plane_arm64(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
void uavs3e_intra_pred_bi_arm64(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
void uavs3e_intra_pred_hor_arm64(pel *src, pel *dst, int i_dst, int width, int height);
void uavs3e_intra_pred_ver_arm64(pel *src, pel *dst, int i_dst, int width, int height);
void uavs3e_intra_pred_bi_ipf_arm64(pel *pSrc, s16 *dst, int width, int height);
void uavs3e_intra_pred_plane_ipf_arm64(pel *src, s16 *dst, int width, int height);

void uavs3e_ipred_ang_x_arm64(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
void uavs3e_ipred_ang_y_arm64(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

void uavs3e_ipred_ang_x_3_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_4_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_5_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_6_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_7_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_8_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_9_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_10_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_x_11_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);

void uavs3e_ipred_ang_xy_14_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_16_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_18_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_20_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_xy_22_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);

void uavs3e_ipred_ang_y_25_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_26_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_27_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_28_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_29_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_30_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_31_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);
void uavs3e_ipred_ang_y_32_arm64(pel *src, pel *dst, int i_dst, int mode, int width, int height);

void uavs3e_recon_w4_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w8_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w16_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w32_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
void uavs3e_recon_w64_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);

void uavs3e_itrans_dct2_h4_w4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h4_w32_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h8_w4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h8_w64_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h16_w4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h16_w64_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h32_w4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h32_w64_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct2_h64_w8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_itrans_dct2_h64_w64_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_itrans_dct8_pb4_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val, s8 *iT);
void uavs3e_itrans_dct8_pb8_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val, s8 *iT);
void uavs3e_itrans_dct8_pb16_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val, s8 *iT);

void uavs3e_itrans_dst7_pb4_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val, s8 *iT);
void uavs3e_itrans_dst7_pb8_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val, s8 *iT);
void uavs3e_itrans_dst7_pb16_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val, s8 *iT);

void uavs3e_trans_dct2_w4_h4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w4_h8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w4_h16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w4_h32_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w8_h4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w8_h64_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w16_h4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w16_h64_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w32_h4_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w32_h64_arm64(s16 *src, s16 *dst, int bit_depth);

void uavs3e_trans_dct2_w64_h8_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w64_h16_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w64_h32_arm64(s16 *src, s16 *dst, int bit_depth);
void uavs3e_trans_dct2_w64_h64_arm64(s16 *src, s16 *dst, int bit_depth);

void tx_dct2_pb4_arm64(s16* src, s16* dst, int line, int limit_line, int shift);
void tx_dct2_pb8_arm64(s16* src, s16* dst, int line, int limit_line, int shift);
void tx_dct2_pb16_arm64(s16* src, s16* dst, int line, int limit_line, int shift);
void tx_dct2_pb32_arm64(s16* src, s16* dst, int line, int limit_line, int shift);
void tx_dct2_pb64_arm64(s16* src, s16* dst, int line, int limit_line, int shift);

u32 uavs3e_get_sad_4_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_8_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_16_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_32_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_64_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u32 uavs3e_get_sad_128_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

void uavs3e_get_sad_x3_4_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_8_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_16_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_32_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_64_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x3_128_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height);

void uavs3e_get_sad_x4_4_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_8_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_16_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_32_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_64_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);
void uavs3e_get_sad_x4_128_arm64(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[3], int height);

u64 uavs3e_get_ssd_4_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_8_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_16_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_32_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_64_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
u64 uavs3e_get_ssd_128_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

u32 uavs3e_had_4x4_arm64(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_4x8_arm64(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_8x4_arm64(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_8x8_arm64(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_16x8_arm64(pel *org, int s_org, pel *cur, int s_cur);
u32 uavs3e_had_8x16_arm64(pel *org, int s_org, pel *cur, int s_cur);

void uavs3e_pel_diff_4_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_8_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_16_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_32_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_64_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);
void uavs3e_pel_diff_128_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height);

void affine_sobel_flt_hor_arm64(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
void affine_sobel_flt_ver_arm64(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
void affine_coef_computer_arm64(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[5], int width, int height);

#endif
#endif
