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

#ifndef _MODULES_H_
#define _MODULES_H_

#include "tables.h"

/*** rc ***/
void rc_init(enc_rc_t* p, enc_cfg_t *param);
int  rc_get_qp(enc_rc_t *p, com_pic_t *pic, int qp_l0, int qp_l1);
void rc_update(enc_rc_t *p, com_pic_t *pic, char *ext_info, int info_buf_size);
void rc_destroy(enc_rc_t *p);

/*** alf ***/
int enc_alf_avs2(enc_pic_t *ep, com_pic_t *pic_rec, com_pic_t *pic_org, double lambda);

/*** sao ***/
void enc_sao_rdo(core_t *core, const lbac_t *lbac);

/*** transform ***/
void transform(com_mode_t *mode, int plane, int blk_idx, s16 *coef, s16 *resi, int cu_width_log2, int cu_height_log2, int bit_depth, int secT_Ver_Hor, int use_alt4x4Trans);

/*** quant ***/
void rdoq_init_err_scale(int bit_depth);
void rdoq_init_prob_2_bits();
void rdoq_init_cu_est_bits(core_t *core, lbac_t *lbac);
int quant_non_zero(core_t *core, int qp, double lambda, int is_intra, s16 *coef, int cu_width_log2, int cu_height_log2, int ch_type, int slice_type);

/*** entropy ***/
void lbac_reset(lbac_t *lbac);
void lbac_finish(lbac_t *lbac, bs_t *bs);
void lbac_encode_bin(u32 bin, lbac_t *lbac, lbac_ctx_model_t *ctx_model, bs_t *bs);
void lbac_encode_bin_trm(u32 bin, lbac_t *lbac, bs_t *bs);
u32  lbac_get_bits(lbac_t *lbac);

#define lbac_copy(dst,src) com_mcpy(dst, src, sizeof(lbac_t))

int  ec_write_sqh              (bs_t *bs, com_seqh_t *sqh);
int  ec_write_pichdr           (bs_t *bs, com_pic_header_t *sh, com_seqh_t *sqh, com_pic_t *pic);
int  ec_write_patch_hdr        (bs_t *bs, com_seqh_t *sqh, com_pic_header_t *ph, com_patch_header_t *sh, u8 patch_idx);
int  ec_write_patch_end        (bs_t *bs);
int  ec_write_ext_and_usr_data (bs_t *bs, int signature, com_pic_t *pic_rec, int i, int slice_type, u8 isExtension);
                               
int  lbac_enc_unit             (lbac_t *lbac, bs_t *bs, core_t *core, enc_cu_t *cu_data, int x, int y, int cup, int cu_width, int cu_height);
int  lbac_enc_unit_chroma      (lbac_t *lbac, bs_t *bs, core_t *core, enc_cu_t *cu_data, int x, int y, int cup, int cu_width, int cu_height);
void lbac_enc_lcu_delta_qp     (lbac_t *lbac, bs_t *bs, int val, int last_dqp);
int  lbac_enc_split_mode       (lbac_t *lbac, bs_t *bs, core_t *core, s8 split_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, const int parent_split, int qt_depth, int bet_depth, int x, int y);
int  lbac_enc_pred_mode        (lbac_t *lbac, bs_t *bs, core_t *core, u8 pred_mode);
int  lbac_enc_ipf_flag         (lbac_t *lbac, bs_t *bs, u8 ipf_flag);
int  lbac_enc_coef             (lbac_t *lbac, bs_t *bs, core_t *core, s16 coef[N_C][MAX_CU_DIM], int cu_width_log2, int cu_height_log2, u8 pred_mode, com_mode_t *mi, u8 tree_status);
void lbac_enc_cons_pred_mode   (lbac_t *lbac, bs_t *bs, u8 cons_pred_mode_child);
void lbac_enc_slice_end_flag   (lbac_t *lbac, bs_t *bs, int flag);
int  lbac_enc_mvd              (lbac_t *lbac, bs_t *bs, s16 mvd[MV_D]);
int  lbac_enc_refi             (lbac_t *lbac, bs_t *bs, int num_refp, int refi);
void lbac_enc_inter_dir        (lbac_t *lbac, bs_t *bs, core_t *core, s8 refi[REFP_NUM], int part_size);
void lbac_enc_skip_flag        (lbac_t *lbac, bs_t *bs, core_t *core, int flag);
void lbac_enc_umve_flag        (lbac_t *lbac, bs_t *bs, int flag);
void lbac_enc_umve_idx         (lbac_t *lbac, bs_t *bs, int umve_idx);
void lbac_enc_skip_idx         (lbac_t *lbac, bs_t *bs, com_pic_header_t *pichdr, int skip_idx, int num_hmvp_cands);
void lbac_enc_direct_flag      (lbac_t *lbac, bs_t *bs, core_t *core, int t_direct_flag);
void lbac_enc_xcoef            (lbac_t *lbac, bs_t *bs, s16 *coef, int log2_w, int log2_h, int num_sig, int ch_type);
int  lbac_enc_intra_dir        (lbac_t *lbac, bs_t *bs, u8 ipm, u8 mpm[2]);
int  lbac_enc_intra_dir_c      (lbac_t *lbac, bs_t *bs, u8 ipm, u8 ipm_l, u8 tscpm_enable);
void lbac_enc_ipcm             (lbac_t *lbac, bs_t *bs, s16 pcm[MAX_CU_DIM], int tb_width, int tb_height, int cu_width, int bit_depth, int ch_type);
int  lbac_enc_mvr_idx          (lbac_t *lbac, bs_t *bs, u8 mvr_idx, BOOL is_affine_mode);
void lbac_enc_mvr_flag_extend  (lbac_t *lbac, bs_t *bs, u8 hmvp_flag);
void lbac_enc_affine_flag      (lbac_t *lbac, bs_t *bs, core_t *core, int flag);
void lbac_enc_affine_mrg_idx   (lbac_t *lbac, bs_t *bs, s16 affine_mrg_idx);
void lbac_enc_smvd_flag        (lbac_t *lbac, bs_t *bs, int flag);
void lbac_enc_part_size        (lbac_t *lbac, bs_t *bs, core_t *core, int part_size, int cu_w, int cu_h, int pred_mode);
void lbac_enc_sao_mrg_flag     (lbac_t *lbac, bs_t *bs, int mergeleft_avail, int mergeup_avail, com_sao_param_t *saoBlkParam);
void lbac_enc_sao_mode         (lbac_t *lbac, bs_t *bs, com_sao_param_t *saoBlkParam);
void lbac_enc_sao_offset       (lbac_t *lbac, bs_t *bs, com_sao_param_t *saoBlkParam);
void lbac_enc_sao_type         (lbac_t *lbac, bs_t *bs, com_sao_param_t *saoBlkParam);
void lbac_enc_alf_flag         (lbac_t *lbac, bs_t *bs, int iflag);
void lbac_enc_sao_param        (lbac_t *lbac, bs_t *bs, com_patch_header_t *pathdr, com_map_t *map, com_info_t *info, int lcu_y, int lcu_x, com_sao_param_t sao_cur_param[N_C], com_sao_param_t(*sao_blk_params_map)[N_C]);

void bs_demulate(bs_t *bs);

#endif // #ifndef _MODULES_H_