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

#ifndef _COM_UTIL_H_
#define _COM_UTIL_H_

#include "com_type.h"

#include <stdlib.h>

/*! macro to determine maximum */
#define COM_MAX(a,b)                   (((a) > (b)) ? (a) : (b))

/*! macro to determine minimum */
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))

/*! macro to absolute a value */
#define COM_ABS(a)                     abs(a)

/*! macro to absolute a 64-bit value */
#define COM_ABS64(a)                   (((a)^((a)>>63)) - ((a)>>63))

/*! macro to absolute a 32-bit value */
#define COM_ABS32(a)                   (((a)^((a)>>31)) - ((a)>>31))

/*! macro to absolute a 16-bit value */
#define COM_ABS16(a)                   (((a)^((a)>>15)) - ((a)>>15))

/*! macro to clipping within min and max */
#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))

/*! macro to clipping within min and max */
#define COM_CLIP(n,min,max)            (((n)>(max))? (max) : (((n)<(min))? (min) : (n)))

#define COM_SIGN(x)                    (((x) < 0) ? -1 : 1)

/*! macro to get a sign from a 16-bit value.\n
operation: if(val < 0) return 1, else return 0 */
#define COM_SIGN_GET(val)              ((val<0)? 1: 0)

/*! macro to set sign into a value.\n
operation: if(sign == 0) return val, else if(sign == 1) return -val */
#define COM_SIGN_SET(val, sign)        ((sign)? -val : val)

/*! macro to get a sign from a 16-bit value.\n
operation: if(val < 0) return 1, else return 0 */
#define COM_SIGN_GET16(val)            (((val)>>15) & 1)

/*! macro to set sign into a 16-bit value.\n
operation: if(sign == 0) return val, else if(sign == 1) return -val */
#define COM_SIGN_SET16(val, sign)      (((val) ^ ((s16)((sign)<<15)>>15)) + (sign))

#define COM_ALIGN(val, align)          ((((val) + (align) - 1) / (align)) * (align))

#define IS_BYTE_ALIGN(bs) !((bs)->leftbits & 0x7)

void com_dct_coef_create();
void set_pic_wq_matrix_by_param(int *param_vector, int mode, u8 *pic_wq_matrix4x4, u8 *pic_wq_matrix8x8);
void init_pic_wq_matrix(u8 *pic_wq_matrix4x4, u8 *pic_wq_matrix8x8);

#define SAME_MV(MV0, MV1) (M32(MV0) == M32(MV1))

void copy_motion_table(com_motion_t *motion_dst, s8 *cnt_cands_dst, const com_motion_t *motion_src, const s8 cnt_cands_src);

int same_motion(com_motion_t motion1, com_motion_t motion2);

u16 com_get_avail_intra(int x_scu, int y_scu, int i_scu, int scup, com_scu_t *map_scu);

com_pic_t *com_pic_create(int w, int h, int pad_l, int pad_c, int *err);
void       com_pic_destroy(com_pic_t *pic);

com_subpel_t* com_subpel_create(int width, int height, int pad_l, int pad_c, int *err);
void          com_subpel_free(com_subpel_t *p);

void check_mvp_motion_availability(int scup, int cu_width, int cu_height, int i_scu, int neb_addr[NUM_AVS2_SPATIAL_MV], int valid_flag[NUM_AVS2_SPATIAL_MV], com_scu_t *map_scu, s8(*map_refi)[REFP_NUM], int lidx);
void check_umve_motion_availability(int scup, int cu_width, int cu_height, int i_scu, int neb_addr[NUM_AVS2_SPATIAL_MV], int valid_flag[NUM_AVS2_SPATIAL_MV], com_scu_t *map_scu, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM]);

void update_skip_candidates(com_motion_t motion_cands[ALLOWED_HMVP_NUM], s8 *cands_num, const int max_hmvp_num, s16 mv_new[REFP_NUM][MV_D], s8 refi_new[REFP_NUM]);
void fill_skip_candidates(com_motion_t motion_cands[ALLOWED_HMVP_NUM], s8 *num_cands, const int num_hmvp_cands, s16 mv_new[REFP_NUM][MV_D], s8 refi_new[REFP_NUM], int bRemDuplicate);
void get_hmvp_skip_cands(const com_motion_t motion_cands[ALLOWED_HMVP_NUM], const u8 num_cands, s16(*skip_mvs)[REFP_NUM][MV_D], s8(*skip_refi)[REFP_NUM]);

void derive_MHBskip_spatial_motions(int scup, int cu_width, int cu_height, int i_scu, com_scu_t *map_scu, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], s16 skip_pmv[PRED_DIR_NUM][REFP_NUM][MV_D], s8 skip_refi[PRED_DIR_NUM][REFP_NUM]);

void derive_umve_base_motions(int scup, int cu_width, int cu_height, int i_scu, com_scu_t *map_scu, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], s16 t_mv[REFP_NUM][MV_D], s8 t_refi[REFP_NUM], s16 umve_base_pmv[UMVE_BASE_NUM][REFP_NUM][MV_D], s8 umve_base_refi[UMVE_BASE_NUM][REFP_NUM]);

void derive_umve_final_motions(int umve_idx, com_ref_pic_t(*refp)[REFP_NUM], s64 cur_poc, s16 umve_base_pmv[UMVE_BASE_NUM][REFP_NUM][MV_D], s8 umve_base_refi[UMVE_BASE_NUM][REFP_NUM], s16 umve_final_pmv[UMVE_BASE_NUM *UMVE_MAX_REFINE_NUM][REFP_NUM][MV_D], s8 umve_final_refi[UMVE_BASE_NUM *UMVE_MAX_REFINE_NUM][REFP_NUM]);

void com_get_mvp_default(s64 ptr_cur, int scup, int lidx, s8 cur_refi, s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], com_ref_pic_t(*refp)[REFP_NUM],
                         u8 amvr_idx, int cu_width, int cu_height, int i_scu, s16 mvp[MV_D], com_scu_t *map_scu);

void com_derive_mvp(com_info_t *info, s64 ptr, int scup, int ref_list, int ref_idx, int emvp_flag, int cnt_hmvp_cands, com_motion_t *motion_cands, com_map_t *map, com_ref_pic_t(*refp)[REFP_NUM], int mvr_idx, int cu_width, int cu_height, s16 mvp[MV_D]);

#define COM_IPRED_CONV_L2C(mode)\
    ((mode) == IPD_VER) ? IPD_VER_C : \
    ((mode) == IPD_HOR ? IPD_HOR_C : ((mode) == IPD_DC ? IPD_DC_C : IPD_BI_C))

#define COM_IPRED_CONV_L2C_CHK(mode, chk) \
    if(COM_IPRED_CHK_CONV(mode)) \
    {\
        (mode) = ((mode) == IPD_VER) ? IPD_VER_C : ((mode) == IPD_HOR ? IPD_HOR_C:\
                 ((mode) == IPD_DC ? IPD_DC_C : IPD_BI_C)); \
        (chk) = 1; \
    }\
    else \
        (chk) = 0;

typedef struct uavs3e_com_split_struct_t {
    int       part_count;
    int       cud;
    int       width  [4];
    int       height [4];
    int       log_cuw[4];
    int       log_cuh[4];
    int       x_pos  [4];
    int       y_pos  [4];
    int       cup    [4];
} com_split_struct_t;

//! Count of partitions, correspond to split_mode
int com_split_part_count(int split_mode);
//! Get partition size
int com_split_get_part_size(int split_mode, int part_num, int length);
//! Get partition size log
int com_split_get_part_size_idx(int split_mode, int part_num, int length_idx);
//! Get partition split structure
void com_split_get_part_structure(int split_mode, int x0, int y0, int cu_width, int cu_height, int cup, int cud, int log2_lcuwh_in_scu, com_split_struct_t *split_struct);
//! Get array of split modes tried sequentially in RDO
void com_split_get_split_rdo_order(int cu_width, int cu_height, split_mode_t splits[NUM_SPLIT_MODE]);
//! Get split direction. Quad will return vertical direction.
split_dir_t com_split_get_direction(split_mode_t mode);

//! Is mode triple tree?
int  com_split_is_EQT(split_mode_t mode);

//! Is mode BT?
int  com_split_is_BT(split_mode_t mode);
//! Check that mode is vertical
int com_split_is_vertical(split_mode_t mode);
//! Check that mode is horizontal
int com_split_is_horizontal(split_mode_t mode);

int get_colocal_scup(int scup, int i_scu, int pic_width_in_scu, int pic_height_in_scu);

void get_col_mv(com_ref_pic_t refp[REFP_NUM], s64 ptr, int scup, s16 mvp[REFP_NUM][MV_D]);
void get_col_mv_from_list0(com_ref_pic_t refp[REFP_NUM], s64 ptr, int scup, s16 mvp[REFP_NUM][MV_D]);

int com_scan_tbl_init();
int com_scan_tbl_delete();
int com_get_split_mode(s8 *split_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE]);
int com_set_split_mode(s8  split_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE]);

u8   com_get_cons_pred_mode(int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE]);
void com_set_cons_pred_mode(u8 cons_pred_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE]);

static avs3_inline void com_mv_rounding_s32(s32 hor, int ver, s32 *rounded_hor, s32 *rounded_ver, s32 right_shift, int left_shift)
{
    int add = (right_shift > 0) ? (1 << (right_shift - 1)) : 0;
    *rounded_hor = (hor >= 0) ? (((hor + add) >> right_shift) << left_shift) : -(((-hor + add) >> right_shift) << left_shift);
    *rounded_ver = (ver >= 0) ? (((ver + add) >> right_shift) << left_shift) : -(((-ver + add) >> right_shift) << left_shift);
}

static avs3_inline void com_mv_rounding_s16(s32 hor, s32 ver, s16 *rounded_hor, s16 *rounded_ver, int shift)
{
    com_assert(shift);
    int add = (1 << (shift - 1));
    *rounded_hor = ((hor + add) >> shift) << shift;
    *rounded_ver = ((ver + add) >> shift) << shift;
}

void com_get_affine_mvp_scaling(s64 ptr, int scup, int lidx, s8 cur_refi, \
                                s16(*map_mv)[REFP_NUM][MV_D], s8(*map_refi)[REFP_NUM], com_ref_pic_t(*refp)[REFP_NUM], \
                                int cu_width, int cu_height, int i_scu, CPMV mvp[VER_NUM][MV_D],
                                com_scu_t *map_scu, u32 *map_pos, int vertex_num
                                , u8 curr_mvr
                               );

int com_get_affine_memory_access(CPMV mv[VER_NUM][MV_D], int cu_width, int cu_height);

void com_set_affine_mvf(int scup, int log2_cuw, int log2_cuh, int i_scu, com_mode_t *cur_info, com_map_t *pic_map, com_pic_header_t *sh);

int com_get_affine_merge_candidate(s64 ptr, int scup, s8(*map_refi)[REFP_NUM], s16(*map_mv)[REFP_NUM][MV_D], com_scu_t *map_scu, u32 *map_pos, com_ref_pic_t(*refp)[REFP_NUM], int cu_width, int cu_height, int i_scu, int pic_width_in_scu, int pic_height_in_scu, int slice_type, s8 mrg_list_refi[AFF_MAX_NUM_MRG][REFP_NUM], CPMV mrg_list_cpmv[AFF_MAX_NUM_MRG][REFP_NUM][VER_NUM][MV_D], int mrg_list_cp_num[AFF_MAX_NUM_MRG], int log2_max_cuwh);


/* MD5 structure */
typedef struct uavs3e_com_md5_t {
    u32     h[4]; /* hash state ABCD */
    u8      msg[64]; /*input buffer (nal message) */
    u32     bits[2]; /* number of bits, modulo 2^64 (lsb first)*/
} com_md5_t;

/* MD5 Functions */
void com_md5_init(com_md5_t *md5);
void com_md5_finish(com_md5_t *md5, u8 digest[16]);
void com_md5_img(com_img_t *img, u8 digest[16]);

void com_check_split_mode(com_seqh_t *sqh, int *split_allow, int cu_width_log2, int cu_height_log2, int boundary, int boundary_b, int boundary_r, int log2_max_cuwh,
                          const int parent_split, int qt_depth, int bet_depth, int slice_type);


void com_lbac_ctx_init(com_lbac_all_ctx_t *lbac_ctx);

int  com_dt_allow(int cu_w, int cu_h, int pred_mode, int max_dt_size);

void init_tb_part(com_mode_t *mode);
void init_pb_part(com_mode_t *mode);
void set_pb_part(com_mode_t *mode, part_size_t part_size);
void set_tb_part(com_mode_t *mode, part_size_t part_size);
void get_part_info(int i_scu, int x, int y, int w, int h, part_size_t part_size, com_part_info_t *sub_info);
int  get_part_idx(part_size_t part_size, int x, int y, int w, int h);
void update_intra_info_map_scu(com_scu_t *map_scu, s8 *map_ipm, int tb_x, int tb_y, int tb_w, int tb_h, int i_scu, int ipm);

int  get_part_num(part_size_t size);
int  get_part_num_tb_in_pb(part_size_t pb_part_size, int pb_part_idx);
int  get_tb_idx_offset(part_size_t pb_part_size, int pb_part_idx);
void get_tb_width_height_in_pb(int pb_w, int pb_h, part_size_t pb_part_size, int pb_part_idx, int *tb_w, int *tb_h);
void get_tb_pos_in_pb(int pb_x, int pb_y, part_size_t pb_part_size, int tb_w, int tb_h, int tb_part_idx, int *tb_x, int *tb_y);
int get_coef_offset_tb(int cu_x, int cu_y, int tb_x, int tb_y, int cu_w, int cu_h, int tb_part_size);
part_size_t get_tb_part_size_by_pb(part_size_t pb_part, int pred_mode);
void get_tb_width_height_log2(int log2_w, int log2_h, part_size_t part, int *log2_tb_w, int *log2_tb_h);
void get_tb_width_height(int w, int h, part_size_t part, int *tb_w, int *tb_h);
void get_tb_start_pos(int w, int h, part_size_t part, int idx, int *pos_x, int *pos_y);
int  is_tb_avaliable(com_info_t *info, int log2_w, int log2_h, part_size_t pb_part_size, int pred_mode);
int  is_cu_nz(int nz[MAX_NUM_TB][N_C]);
int  is_cu_plane_nz(int nz[MAX_NUM_TB][N_C], int plane);
void cu_plane_nz_cpy(int dst[MAX_NUM_TB][N_C], int src[MAX_NUM_TB][N_C], int plane);
void cu_plane_nz_cln(int dst[MAX_NUM_TB][N_C], int plane);
int is_cu_nz_equ(int dst[MAX_NUM_TB][N_C], int src[MAX_NUM_TB][N_C]);
void cu_nz_cln(int dst[MAX_NUM_TB][N_C]);
void check_set_tb_part(com_mode_t *mode);
void check_tb_part(com_mode_t *mode);
void copy_rec_y_to_pic(pel *src, int x, int y, int w, int h, int stride, com_pic_t *pic);

u8 is_use_cons(int w, int h, split_mode_t split, u8 slice_type);
u8 com_tree_split(int w, int h, split_mode_t split, u8 slice_type);


/* function selection define based on platforms */
#if (defined(__ANDROID__) && defined(__aarch64__)) || (defined(__APPLE__) && defined(__arm64__))
#define ENABLE_FUNCTION_C 1
#define ENABLE_FUNCTION_ARM64 1
#elif (defined(__ANDROID__) && defined(__arm__)) || (defined(__APPLE__) && defined(__ARM_NEON__))
#define ENABLE_FUNCTION_C 1
#define ENABLE_FUNCTION_ARM32 1
#elif (defined(__WIN32__) || defined(_WIN32)) || (defined(__MACOSX__) || defined(macintosh) || defined(__linux__) || defined(__unix__)) && (defined(__i386__) || defined(__x86_64__) || defined(__AMD64__))
#define ENABLE_FUNCTION_X86 1
#define ENABLE_FUNCTION_C 1
#else
#define ENABLE_FUNCTION_C 1
#endif

typedef struct uavs3e_funs_handle_t {
    void(*intra_pred_dc)(pel *src, pel *dst, int i_dst, int width, int height, u16 cu_avail, int bit_depth);
    void(*intra_pred_plane)(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
    void(*intra_pred_bi)(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth);
    void(*intra_pred_plane_ipf)(pel *src, s16 *dst, int width, int height);
    void(*intra_pred_bi_ipf)(pel *src, s16 *dst, int width, int height);
    void(*intra_pred_ver)(pel *src, pel *dst, int i_dst, int width, int height);
    void(*intra_pred_hor)(pel *src, pel *dst, int i_dst, int width, int height);
    void(*intra_pred_ang[IPD_CNT])(pel *src, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
    void(*intra_pred_ipf_core)(pel *src, pel *dst, int i_dst, int ipm, int w, int h, int bit_depth);
    void(*intra_pred_ipf_core_s16)(pel *src, pel *dst, int i_dst, s16 *pred, int ipm, int w, int h, int bit_depth);

    void(*deblock_luma  [2])(pel *src, int stride, int alpha,  int beta,  int flag);
    void(*deblock_chroma[2])(pel *srcu, pel *srcv, int stride, int alphau, int betau, int alphav, int betav, int flag);

    void(*sao)(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *sao_params, int height, int width, int avail_left, int avail_right, int avail_up, int avail_down, int bit_depth);
    void(*sao_stat)(com_pic_t *pic_org, com_pic_t *pic_rec, com_sao_stat_t *saostatsData, int bit_depth, int compIdx, int pix_x, int pix_y, int lcu_pix_width, int lcu_pix_height, int lcu_available_left, int lcu_available_right, int lcu_available_up, int lcu_available_down);

    void(*alf)(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
    void(*alf_fix)(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
    void(*alf_calc)(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail);

    void(*ipcpy                      [CU_SIZE_NUM])(const pel *src, int i_src, pel *dst, int i_dst, int width, int height);
    void(*ipflt    [NUM_IPFILTER    ][CU_SIZE_NUM])(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val);
    void(*ipflt_ext[NUM_IPFILTER_Ext][CU_SIZE_NUM])(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val);

    void(*ip_flt_y_hor    )(const pel *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth);
    void(*ip_flt_y_ver    )(const pel *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);
    void(*ip_flt_y_ver_ext)(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth);

    void(*recon[CU_SIZE_NUM])(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth);
    void(*dquant[2])(s16 *coef, s16 *coef_out, u8 *wq_matrix[2], int log2_w, int log2_h, int scale, int shift, int bit_depth);

    void(*itrans_dct2[MAX_TR_LOG2][MAX_TR_LOG2])(s16 *coef, s16 *resi, int bit_depth);
    void(*itrans_dct8_dst7[2][MAX_TR_LOG2])(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
    void(*trans_dct2[MAX_TR_LOG2][MAX_TR_LOG2])(s16 *coef, s16 *resi, int bit_depth);
    void(*trans_dct8_dst7[2][MAX_TR_LOG2])(s16 *src, s16 *dst, int shift, int line);

    u32 (*cost_sad   [CU_SIZE_NUM])(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);
    void(*cost_sad_x3[CU_SIZE_NUM])(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height);
    void(*cost_sad_x4[CU_SIZE_NUM])(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel *pred3, int i_pred, u32 sad[4], int height);

    u64 (*cost_ssd   [CU_SIZE_NUM])(pel *p_org, int i_org, pel *p_pred, int i_pred, int height);

    u32 (*cost_satd[3][3])(pel *p_org, int i_org, pel *p_pred, int i_pred);

    void(*pel_diff[CU_SIZE_NUM])(pel *org, int i_org, pel *pred, int i_pred, s16 *resi, int i_resi, int height);
    void(*pel_avrg[CU_SIZE_NUM])(pel *dst, int i_dst, pel *src1, pel *src2, int height);

    void(*affine_sobel_flt_hor)(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
    void(*affine_sobel_flt_ver)(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height);
    void(*affine_coef_computer)(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[7], int width, int height, int vertex_num);

    int(*quant_rdoq)(s16 *coef, int num, int q_value, int q_bits, s32 err_scale, int precision_bits, u32* abs_coef, s16* abs_level, s64 *uncoded_err);
    int(*quant_check)(s16 *coef, int num, int threshold);


} funs_handle_t;

static avs3_always_inline void com_mset_pel(pel *dst, s16 v, int cnt)
{
#if (BIT_DEPTH == 8)
    memset(dst, v, cnt);
#else
    int i;
    for (i = 0; i < cnt; i++) {
        dst[i] = v;
    }
#endif
}

extern funs_handle_t uavs3e_funs_handle;

#if ENABLE_FUNCTION_C
void uavs3e_funs_init_cost_c();
void uavs3e_funs_init_itrans_c();
void uavs3e_funs_init_trans_c();
void uavs3e_funs_init_mc_c();
void uavs3e_funs_init_intra_pred_c();
//void uavs3e_funs_init_recon_c();
void uavs3e_funs_init_deblock_c();
void uavs3e_funs_init_sao_c();
void uavs3e_funs_init_alf_c();
void uavs3e_funs_init_c();
#endif

#if ENABLE_FUNCTION_X86
int  uavs3e_simd_avx_level(int *phwavx);
void uavs3e_funs_init_sse();
void uavs3e_funs_init_avx2();
#endif

#if ENABLE_FUNCTION_ARM64
//void uavs3e_funs_init_arm64();
#endif

void *uavs3e_align_malloc(int i_size);
void uavs3e_align_free(void *p);

static avs3_always_inline int uavs3e_get_log2(int v)
{
#ifdef _WIN32
    unsigned long index;
    _BitScanReverse(&index, v);
    return index;
#else
    return 31 - __builtin_clz(v);
#endif
}

static int avs3_always_inline getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset)
{
    int imult = tab_auc_dir_dxdy[uiXYflag][uiDirMode][0];
    int ishift = tab_auc_dir_dxdy[uiXYflag][uiDirMode][1];

    int iTempDn = iTempD * imult >> ishift;
    *offset = ((iTempD * imult * 32) >> ishift) - iTempDn * 32;
    return iTempDn;
}

static void avs3_always_inline wait_ref_available(com_pic_t *pic, int lines)
{
    int real_lines = COM_MAX(0, lines);
    real_lines = COM_MIN(pic->height_luma, real_lines);

    if (pic->end_line < real_lines) {
        uavs3e_pthread_mutex_lock(&pic->mutex);

        while (pic->end_line < real_lines) {
            uavs3e_pthread_cond_wait(&pic->cv, &pic->mutex);
        }

        uavs3e_pthread_mutex_unlock(&pic->mutex);
    }
}

u32 com_had(int w, int h, void *addr_org, void *addr_curr, int s_org, int s_cur, int bit_depth);

com_img_t *com_img_create(int w, int h, int pad[MAX_PLANES], int planes);
void       com_img_free(com_img_t *img);
void       com_img_padding(com_img_t *img, int planes, int ext_size);

static int com_img_addref(com_img_t *img)
{
    return ++img->refcnt;
}
static int com_img_getref(com_img_t *img)
{
    return img->refcnt;
}
static int com_img_release(com_img_t *img)
{
    if (--img->refcnt == 0) {
        com_img_free(img);
    }
    return img->refcnt;
}

#endif /* _COM_UTIL_H_ */