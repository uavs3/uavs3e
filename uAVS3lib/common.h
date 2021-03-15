#include "defines.h"
#include "commonStructures.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>


/* ---------------------------------------------------------------------------
* log level
*/
#define COM_LOG_NONE       (-1)
#define COM_LOG_ERROR      0
#define COM_LOG_WARNING    1
#define COM_LOG_INFO       2
#define COM_LOG_DEBUG      3

extern tab_uchar_t tab_scan_4x4[16][2];
extern tab_uchar_t tab_scan_8x8[64][2];

extern tab_uchar_t tab_scan_2x8[16][2];
extern tab_uchar_t tab_scan_8x2[16][2];

extern tab_i16u_t *tab_1d_scan_list[4];
extern tab_i16u_t *tab_1d_scan_swap_list[4];

extern tab_i16u_t *tab_cg_idx_list[4];
extern tab_i16u_t *tab_cg_idx_swap_list[4];

extern tab_uchar_t tab_inter_pu_num[MAXMODE];
extern tab_uchar_t tab_inter_pu_2_blk_idx[MAXMODE][4];

extern funs_handle_t g_funs_handle;

void com_log(int i_level, const char_t *psz_fmt, ...);
void *com_malloc(int i_size, int inited);
void com_free(void *p);

int image_create(image_t *p_img, int width, int height, int b_org, int planes);
#define image_create_rec(p_img, width, height, planes) image_create(p_img, width, height, 0, planes);
#define image_create_org(p_img, width, height)         image_create(p_img, width, height, 1, 3);

void image_free(image_t *p_img);
void image_copy(image_t *p_dst, image_t *p_src);
void image_copy_rows(image_t *p_dst, image_t *p_src, int start, int rows);
void image_copy_lcu_row_for_sao(avs3_enc_t *h, image_t *p_dst, image_t *p_src, int lcu_y);
void image_padding(image_t *img, int pad);
void image_padding_lcu_row(avs3_enc_t *h, image_t *img, int lcu_y, int pad);

void init_cu_pos(avs3_enc_t *h, int mcu_idx, int bit_size);
void init_pu_inter_pos(avs3_enc_t *h, int mode, int md_direct, int bit_size, int blk);
void init_pu_pos(avs3_enc_t *h, int mode, int md_direct, int bit_size, int blk);
void init_tu_pos(avs3_enc_t *h, int mode, int cu_bitsize, int trans_size, int blk);

void com_funs_init_ip_filter();
void com_funs_init_pixel_opt();
void com_funs_init_intra_pred();
void com_funs_init_transform();
void com_funs_init_quant();
void com_funs_init_rdcost();
void com_funs_init_deblock();
void com_funs_init_sao();

float avs3_pixel_ssim_wxh(pel_t *pix1, int stride1, pel_t *pix2, int stride2, int width, int height, int *cnt);

void com_update_cand_list(int uiMode, double uiCost, int uiFullCandNum, int *CandModeList, double *CandCostList);

void com_funs_init_intrinsic_functions();
void com_funs_init_intrinsic_functions_10bit();
void com_funs_init_intrinsic_functions_avx2();
void com_funs_init_intrinsic_functions_avx2_10bit();
void com_funs_init_intrinsic_functions_avx512_10bit();

int simd_avx_level(int* phwavx);


