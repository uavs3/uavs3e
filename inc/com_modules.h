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

#ifndef _COM_MODELS_H_
#define _COM_MODELS_H_

#include "com_util.h"

////////////// ALF
void com_alf_recon_coef(com_alf_pic_param_t *alfParam, int(*filterCoeff)[ALF_MAX_NUM_COEF]);
void com_alf_check_coef(int *filter, int filterLength);
void com_alf_copy_param(com_alf_pic_param_t *dst, com_alf_pic_param_t *src);
void com_alf_copy_frm(com_pic_t *pic_dst, com_pic_t *pic_src);
void com_alf_buf_init(com_info_t *info, u8 *alf_var_map);

////////////// DEBLOCK
void com_df_lcu(com_info_t *info, com_pic_header_t *pichdr, com_map_t *map, com_pic_t *pic, int lcu_x, int lcu_y);
void com_df_rdo_luma(com_info_t *info, com_pic_header_t *pichdr, com_map_t *map, u8 *edge, int i_edge, pel *src, int i_src, int pix_x, int pix_y, int cuw, int cuh);
void com_df_set_edge(com_info_t *info, com_map_t *map, u8 *edge, int i_edge, com_ref_pic_t refp[MAX_REFS][REFP_NUM], int x, int y, int cuw, int cuh, int cud, int cup, BOOL b_recurse);
void com_df_clear_edge(int x, int y, int w, int h, u8 *map_edge, int i_scu);

////////////// INTRA
void com_intra_get_nbr(int x, int y, int width, int height, pel *src, int s_src, pel *top, int lcu_size, u16 avail_cu, pel nb[N_C][INTRA_NEIB_SIZE], int scup, com_scu_t *map_scu, int i_scu, int bit_depth, int ch_type);
void com_intra_pred(pel *src, pel *dst,  int ipm, int w, int h, int bit_depth, u16 avail_cu, u8 ipf_flag);
void com_intra_pred_chroma(pel *dst, int ipm_c, int ipm, int w, int h, int bit_depth, u16 avail_cu, int chType, pel *piRecoY, int uiStrideY, pel nb[N_C][INTRA_NEIB_SIZE]);
void com_intra_get_mpm(int x_scu, int y_scu, com_scu_t *map_scu, s8 *map_ipm, int scup, int i_scu, u8 mpm[2]);

////////////// IDTQ
#define COM_GET_TRANS_SHIFT(bit_depth, tr_size_log2) (MAX_TX_DYNAMIC_RANGE - (bit_depth) - (tr_size_log2))

void com_invqt(com_mode_t *mode, int plane, int blk_idx, s16 *coef, s16 *resi, u8 *wq[2], int log2_w, int log2_h, int qp, int bit_depth, int secT_Ver_Hor, int use_alt4x4Trans);
void com_invqt_inter_yuv(com_mode_t *mode, int tree_status, s16 coef[N_C][MAX_CU_DIM], s16 resi[N_C][MAX_CU_DIM], u8 *wq[2], int cu_width_log2, int cu_height_log2, int qp_y, int qp_u, int qp_v, int bit_depth);
void com_invqt_inter_plane(com_mode_t *mode, int plane, s16 coef[MAX_CU_DIM], s16 resi[MAX_CU_DIM], u8 *wq[2], int cu_width_log2, int cu_height_log2, int qp, int bit_depth);

////////////// MC
void com_mc_blk_luma(com_pic_t *pic, pel *dst, int dst_stride, int x_pos, int y_pos, int width, int height, int widx, int max_posx, int max_posy, int max_val, int hp_flag);
pel* com_mc_blk_luma_pointer(com_pic_t *pic, int x_pos, int y_pos, int max_posx, int max_posy);
void com_mc_cu(int x, int y, int pic_w, int pic_h, int w, int h, s8 refi[REFP_NUM], s16 mv[REFP_NUM][MV_D], com_ref_pic_t(*refp)[REFP_NUM], pel pred_buf[N_C][MAX_CU_DIM], int pred_stride, channel_type_t channel, int bit_depth);
void com_mc_cu_affine(int x, int y, int pic_w, int pic_h, int w, int h, s8 refi[REFP_NUM], CPMV mv[REFP_NUM][VER_NUM][MV_D], com_ref_pic_t(*refp)[REFP_NUM], pel pred[N_C][MAX_CU_DIM], int cp_num, com_pic_header_t *pichdr, int bit_depth);
void com_mc_blk_affine_luma(int x, int y, int pic_w, int pic_h, int w, int h, CPMV ac_mv[VER_NUM][MV_D], com_pic_t *ref_pic, pel pred[MAX_CU_DIM], int sub_w, int sub_h, int bit_depth);
void com_if_luma_frame(com_img_t *img_list[4][4], s16 *tmp_buf[3], int bit_depth);

////////////// PICMAN
int com_refm_create(com_pic_manager_t *pm, int max_pb_size, int width, int height);
int com_refm_free(com_pic_manager_t *pm);
int com_refm_create_rpl(com_pic_manager_t *pm, com_pic_header_t *pichdr, com_ref_pic_t(*refp)[REFP_NUM], com_pic_t *top_pic[REFP_NUM], int is_top_level);
void com_refm_pick_seqhdr_idx(com_seqh_t *seqhdr, com_pic_header_t *pichdr);
void com_refm_build_ref_buf(com_pic_manager_t *pm);
int com_refm_insert_rec_pic(com_pic_manager_t *pm, com_pic_t *pic, com_ref_pic_t(*refp)[REFP_NUM]);
void com_refm_remove_ref_pic(com_pic_manager_t *pm, com_pic_header_t *pichdr, com_pic_t *pic, int close_gop, int is_ld);

com_pic_t *com_refm_find_free_pic(com_pic_manager_t *pm, int b_ref, int *err);

////////////// RECON
void com_recon_plane(part_size_t part, s16 *resi, pel *pred, int(*is_coef)[N_C], int plane, int cu_width, int cu_height, int s_rec, pel *rec, int bit_depth);

////////////// SAO
long long int  get_distortion(int compIdx, int type, com_sao_stat_t saostatData[N_C][NUM_SAO_NEW_TYPES], com_sao_param_t sao_cur_param[N_C]);
long long int  distortion_cal(long long int count, int offset, long long int diff);
void off_sao(com_sao_param_t *saoblkparam);
BOOL is_same_patch(s8 *map_patch, int mb_nr1, int mb_nr2);
void getSaoMergeNeighbor(com_info_t *info, s8 *map_patch, int pic_width_scu, int pic_width_lcu, int smb_pos, int mb_y, int mb_x, com_sao_param_t(*sao_blk_params)[N_C], int *MergeAvail, com_sao_param_t sao_merge_param[][N_C]);
void com_sao_one_row(com_info_t *info, com_map_t *map, com_pic_t  *pic_rec, com_sao_param_t(*sao_blk_params)[N_C], int lcu_y, pel *sao_src_buf[3], pel *linebuf_sao[3]);

static void avs3_always_inline copySAOParam_for_blk(com_sao_param_t *saopara_dst, com_sao_param_t *saopara_src)
{
    for (int i = 0; i < N_C; i++) {
        saopara_dst[i] = saopara_src[i];
    }
}
static void avs3_always_inline copySAOParam_for_blk_onecomponent(com_sao_param_t *saopara_dst, com_sao_param_t *saopara_src)
{
    *saopara_dst = *saopara_src;
}


#endif