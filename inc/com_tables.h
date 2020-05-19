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