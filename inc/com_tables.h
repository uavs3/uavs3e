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

#ifndef _COM_TBL_H_
#define _COM_TBL_H_

#include "com_define.h"

extern s8 com_tbl_tm2[NUM_TRANS_TYPE][2][2];
extern s8 com_tbl_tm4[NUM_TRANS_TYPE][4][4];
extern s8 com_tbl_tm8[NUM_TRANS_TYPE][8][8];
extern s8 com_tbl_tm16[NUM_TRANS_TYPE][16][16];
extern s8 com_tbl_tm32[NUM_TRANS_TYPE][32][32];
extern s8 com_tbl_tm64[NUM_TRANS_TYPE][64][64];

extern u16   *com_tbl_scan[MAX_CU_LOG2][MAX_CU_LOG2];
extern tab_s8 com_tbl_c4_trans[4][4];
extern tab_s8 com_tbl_c8_trans[4][4];

extern tab_u8 com_tbl_subset_inter[2];

extern tab_u8          com_tbl_part_num[8];
extern const part_size_t com_tbl_tb_part[8];

extern tab_s8 com_tbl_log2[257];

extern tab_s8 com_tbl_mc_l_coeff_hp[16][8];
extern tab_s8 com_tbl_mc_c_coeff_hp[32][4];
extern tab_s8 com_tbl_mc_l_coeff[4][8];
extern tab_s8 com_tbl_mc_c_coeff[8][4];

extern tab_u8 com_tbl_qp_chroma_ajudst[64];
extern tab_u8 com_tbl_qp_chroma_adjust_enc[64];

extern tab_u32 com_tbl_wq_default_param[2][6];
extern tab_u8 com_tbl_wq_default_matrix_4x4[16];
extern tab_u8 com_tbl_wq_default_matrix_8x8[64];
extern tab_u8 com_tbl_wq_model_4x4[4][16];
extern tab_u8 com_tbl_wq_model_8x8[4][64];

extern double com_tbl_sqrt[2];
extern tab_s8 tab_auc_dir_dxdy[2][IPD_CNT][2];

#endif /* _COM_TBL_H_ */