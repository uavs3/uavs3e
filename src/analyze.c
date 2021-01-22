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

#include "define.h"
#include "analyze.h"
#include <math.h>
#include "com_util.h"

void enc_bits_intra_pu(core_t *core, lbac_t *lbac, s32 slice_type, s16 coef[N_C][MAX_CU_DIM], int pb_part_idx)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;

    assert(core->cons_pred_mode != ONLY_INTER);

    if (pb_part_idx == 0) {
        if (slice_type != SLICE_I) {
            if (core->cons_pred_mode != ONLY_INTRA) {
                lbac_enc_skip_flag  (lbac, NULL, core, 0);
                lbac_enc_direct_flag(lbac, NULL, core, 0);
            }
            if (core->cons_pred_mode == NO_MODE_CONS) {
                lbac_enc_pred_mode(lbac, NULL, core, MODE_INTRA);
            }
        }
        lbac_enc_part_size(lbac, NULL, core, cur_info->pb_part, core->cu_width, core->cu_height, MODE_INTRA);
    }

    get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, cur_info->pb_part, &cur_info->pb_info);
    assert(cur_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);

    lbac_enc_intra_dir(lbac, NULL, cur_info->ipm[pb_part_idx][0], cur_info->mpm[pb_part_idx]);

    int log2_tb_w, log2_tb_h, tb_size;
    int num_tb_in_pb = get_part_num_tb_in_pb(cur_info->pb_part, pb_part_idx);

    get_tb_width_height_log2(core->cu_width_log2, core->cu_height_log2, cur_info->tb_part, &log2_tb_w, &log2_tb_h);
    tb_size = 1 << (log2_tb_w + log2_tb_h);

    for (int tb_idx = 0; tb_idx < num_tb_in_pb; tb_idx++) {
        int *num_nz_coef = cur_info->num_nz[tb_idx];
        int cbf_y = (num_nz_coef[Y_C] > 0) ? 1 : 0;

        lbac_encode_bin(cbf_y, lbac, lbac->h.cbf + 0, NULL);

        if (cbf_y) {
            s16 *coef_tb = coef[Y_C] + tb_size * tb_idx;
            lbac_enc_xcoef(lbac, NULL, coef_tb, log2_tb_w, log2_tb_h, num_nz_coef[Y_C], Y_C);
        }
    }
	int cu_width_log2 = core->cu_width_log2;
	int cu_height_log2 = core->cu_height_log2;
#if DT_INTRA_BOUNDARY_FILTER_OFF
	if (info->sqh.ipf_enable_flag && (cu_width_log2 < MAX_CU_LOG2) && (cu_height_log2 < MAX_CU_LOG2) && cur_info->pb_part == SIZE_2Nx2N && core->tree_status != TREE_C)
#else
	if (info->sqh.ipf_enable_flag && (cu_width_log2 < MAX_CU_LOG2) && (cu_height_log2 < MAX_CU_LOG2) && core->tree_status != TREE_C)
#endif
	{
		lbac_enc_ipf_flag(lbac, NULL, cur_info->ipf_flag);
	}
}

void enc_bits_intra_chroma(core_t *core, lbac_t *lbac, s16 coef[N_C][MAX_CU_DIM])
{
    com_info_t *info = core->info;
    int cbf_u, cbf_v;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    com_mode_t *cur_info = &core->mod_info_curr;
    int *num_nz_coef = cur_info->num_nz[TB0];

    lbac_enc_intra_dir_c(lbac, NULL, cur_info->ipm[PB0][1], cur_info->ipm[PB0][0], info->sqh.tscpm_enable);

    cbf_u = (num_nz_coef[U_C] > 0) ? 1 : 0;
    cbf_v = (num_nz_coef[V_C] > 0) ? 1 : 0;
    lbac_encode_bin(cbf_u, lbac, lbac->h.cbf + 1, NULL);
    lbac_encode_bin(cbf_v, lbac, lbac->h.cbf + 2, NULL);
    cu_width_log2--;
    cu_height_log2--;
    if (cbf_u) {
        lbac_enc_xcoef(lbac, NULL, coef[U_C], cu_width_log2, cu_height_log2, num_nz_coef[U_C], U_C);
    }
    if (cbf_v) {
        lbac_enc_xcoef(lbac, NULL, coef[V_C], cu_width_log2, cu_height_log2, num_nz_coef[V_C], V_C);
    }
}

void enc_bits_intra(core_t *core, lbac_t *lbac, s32 slice_type, s16 coef[N_C][MAX_CU_DIM])
{
    com_info_t *info = core->info;
    int cbf_y, cbf_u, cbf_v;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int *num_nz_coef;
    com_mode_t *cur_info = &core->mod_info_curr;

    if (slice_type != SLICE_I && core->tree_status != TREE_C) {
        if (core->cons_pred_mode != ONLY_INTRA) {
            lbac_enc_skip_flag(lbac, NULL, core, 0);
            lbac_enc_direct_flag(lbac, NULL, core, 0);
        }
        if (core->cons_pred_mode == NO_MODE_CONS) {
            lbac_enc_pred_mode(lbac, NULL, core, MODE_INTRA);
        }
    }

    if (core->tree_status != TREE_C) { // luma
        lbac_enc_part_size(lbac, NULL, core, cur_info->pb_part, core->cu_width, core->cu_height, MODE_INTRA);
        get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, cur_info->pb_part, &cur_info->pb_info);
        assert(cur_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);

        for (int pb_idx = 0; pb_idx < cur_info->pb_info.num_sub_part; pb_idx++) {
            lbac_enc_intra_dir(lbac, NULL, cur_info->ipm[pb_idx][0], cur_info->mpm[pb_idx]);
        }
    }

    if (core->tree_status != TREE_L) { // chroma
        lbac_enc_intra_dir_c(lbac, NULL, cur_info->ipm[PB0][1], cur_info->ipm[PB0][0], info->sqh.tscpm_enable);
    }

    if (!(cur_info->ipm[PB0][0] == IPD_IPCM && cur_info->ipm[PB0][1] == IPD_DM_C)) {
#if DT_INTRA_BOUNDARY_FILTER_OFF
        if (info->sqh.ipf_enable_flag && (cu_width_log2 < MAX_CU_LOG2) && (cu_height_log2 < MAX_CU_LOG2) && cur_info->pb_part == SIZE_2Nx2N && core->tree_status != TREE_C)
#else
        if (info->sqh.ipf_enable_flag && (cu_width_log2 < MAX_CU_LOG2) && (cu_height_log2 < MAX_CU_LOG2) && core->tree_status != TREE_C)
#endif
            lbac_enc_ipf_flag(lbac, NULL, cur_info->ipf_flag);

#if DT_INTRA_BOUNDARY_FILTER_OFF
        if (cur_info->pb_part != SIZE_2Nx2N) {
            assert(cur_info->ipf_flag == 0);
        }
#endif

        if (core->tree_status != TREE_C) {
            for (int tb_idx = 0; tb_idx < cur_info->tb_info.num_sub_part; tb_idx++) {
                num_nz_coef = cur_info->num_nz[tb_idx];
                cbf_y = (num_nz_coef[Y_C] > 0) ? 1 : 0;
                lbac_encode_bin(cbf_y, lbac, lbac->h.cbf + 0, NULL);
            }
        } else {
            assert(cur_info->num_nz[TB0][Y_C] == 0);
            assert(cur_info->pb_part == SIZE_2Nx2N);
        }

        num_nz_coef = cur_info->num_nz[TB0];
        cbf_u = (num_nz_coef[U_C] > 0) ? 1 : 0;
        cbf_v = (num_nz_coef[V_C] > 0) ? 1 : 0;
        if (core->tree_status != TREE_L) {
            lbac_encode_bin(cbf_u, lbac, lbac->h.cbf + 1, NULL);
            lbac_encode_bin(cbf_v, lbac, lbac->h.cbf + 2, NULL);
        } else {
            assert(cbf_u == 0);
            assert(cbf_v == 0);
        }
    }

    if (cur_info->ipm[PB0][0] == IPD_IPCM && core->tree_status != TREE_C) {
        int tb_w = cu_width_log2 > 5 ? 32 : (1 << cu_width_log2);
        int tb_h = cu_height_log2 > 5 ? 32 : (1 << cu_height_log2);
        int num_tb_w = cu_width_log2 > 5 ? 1 << (cu_width_log2 - 5) : 1;
        int num_tb_h = cu_height_log2 > 5 ? 1 << (cu_height_log2 - 5) : 1;
        for (int h = 0; h < num_tb_h; h++) {
            for (int w = 0; w < num_tb_w; w++) {
                s16 *coef_tb = coef[Y_C] + (1 << cu_width_log2) * h * tb_h + w * tb_w;
                lbac_enc_ipcm(lbac, NULL, coef_tb, tb_w, tb_h, 1 << cu_width_log2, info->bit_depth_input, Y_C);
            }
        }
    } else {

        assert(cur_info->tb_info.num_sub_part == get_part_num(cur_info->tb_part));
        int log2_tb_w, log2_tb_h, tb_size;
        get_tb_width_height_log2(core->cu_width_log2, core->cu_height_log2, cur_info->tb_part, &log2_tb_w, &log2_tb_h);
        tb_size = 1 << (log2_tb_w + log2_tb_h);
        for (int tb_idx = 0; tb_idx < cur_info->tb_info.num_sub_part; tb_idx++) {
            num_nz_coef = cur_info->num_nz[tb_idx];
            assert(num_nz_coef[Y_C] >= 0);
            cbf_y = (num_nz_coef[Y_C] > 0) ? 1 : 0;
            if (cbf_y) {
                s16 *coef_tb = coef[Y_C] + tb_size * tb_idx;
                lbac_enc_xcoef(lbac, NULL, coef_tb, log2_tb_w, log2_tb_h, num_nz_coef[Y_C], Y_C);
            }
        }
    }

    if (cur_info->ipm[PB0][0] == IPD_IPCM && cur_info->ipm[PB0][1] == IPD_DM_C && core->tree_status != TREE_L) {
        cu_width_log2--;
        cu_height_log2--;
        int tb_w = cu_width_log2 > 5 ? 32 : (1 << cu_width_log2);
        int tb_h = cu_height_log2 > 5 ? 32 : (1 << cu_height_log2);
        int num_tb_w = cu_width_log2 > 5 ? 1 << (cu_width_log2 - 5) : 1;
        int num_tb_h = cu_height_log2 > 5 ? 1 << (cu_height_log2 - 5) : 1;
        for (int h = 0; h < num_tb_h; h++) {
            for (int w = 0; w < num_tb_w; w++) {
                s16 *coef_tb_u = coef[U_C] + (1 << cu_width_log2) * h * tb_h + w * tb_w;
                lbac_enc_ipcm(lbac, NULL, coef_tb_u, tb_w, tb_h, 1 << cu_width_log2, info->bit_depth_input, U_C);
                s16 *coef_tb_v = coef[V_C] + (1 << cu_width_log2) * h * tb_h + w * tb_w;
                lbac_enc_ipcm(lbac, NULL, coef_tb_v, tb_w, tb_h, 1 << cu_width_log2, info->bit_depth_input, V_C);
            }
        }
    } else {
        cu_width_log2--;
        cu_height_log2--;
        num_nz_coef = cur_info->num_nz[TB0];
        cbf_u = (num_nz_coef[U_C] > 0) ? 1 : 0;
        cbf_v = (num_nz_coef[V_C] > 0) ? 1 : 0;
        if (cbf_u) {
            lbac_enc_xcoef(lbac, NULL, coef[U_C], cu_width_log2, cu_height_log2, num_nz_coef[U_C], U_C);
        }
        if (cbf_v) {
            lbac_enc_xcoef(lbac, NULL, coef[V_C], cu_width_log2, cu_height_log2, num_nz_coef[V_C], V_C);
        }
    }
}

void enc_bits_inter_comp(core_t *core, lbac_t *lbac, s16 coef[MAX_CU_DIM], int ch_type)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    int (*num_nz_coef)[N_C] = cur_info->num_nz;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int i, part_num;
    int log2_tb_w, log2_tb_h, tb_size;

    if (ch_type != Y_C) {
        cu_width_log2--;
        cu_height_log2--;
    }

    if (ch_type == Y_C) {
        int tb_avaliable = is_tb_avaliable(info, cu_width_log2, cu_height_log2, cur_info->pb_part, MODE_INTER);
        if (tb_avaliable) {
            lbac_encode_bin(cur_info->tb_part != SIZE_2Nx2N, lbac, lbac->h.tb_split, NULL);
        }
    }

    part_num = get_part_num(ch_type == Y_C ? cur_info->tb_part : SIZE_2Nx2N);
    get_tb_width_height_log2(cu_width_log2, cu_height_log2, ch_type == Y_C ? cur_info->tb_part : SIZE_2Nx2N, &log2_tb_w, &log2_tb_h);
    tb_size = 1 << (log2_tb_w + log2_tb_h);

    for (i = 0; i < part_num; i++) {
        int cbf = (num_nz_coef[i][ch_type] > 0) ? 1 : 0;
        lbac_encode_bin(cbf, lbac, lbac->h.cbf + ch_type, NULL);

        if (cbf) {
            lbac_enc_xcoef(lbac, NULL, coef + i * tb_size, log2_tb_w, log2_tb_h, num_nz_coef[i][ch_type], ch_type);
        }
    }
}

void enc_bits_inter_skip_flag(core_t *core, lbac_t *lbac)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;

    lbac_enc_skip_flag(lbac, NULL, core, 1);
  
    assert(cur_info->pb_part == SIZE_2Nx2N);
    if (info->sqh.umve_enable) {
        lbac_enc_umve_flag(lbac, NULL, cur_info->umve_flag);
    }
    if (cur_info->umve_flag) {
        lbac_enc_umve_idx(lbac, NULL, cur_info->umve_idx);
    } else {
        lbac_enc_affine_flag(lbac, NULL, core, cur_info->affine_flag != 0); /* skip/direct affine_flag */
        if (cur_info->affine_flag) {
            lbac_enc_affine_mrg_idx(lbac, NULL, cur_info->skip_idx);
        } else {
            lbac_enc_skip_idx(lbac, NULL, core->pichdr, cur_info->skip_idx, info->sqh.num_of_hmvp);
        }
    }
}

void enc_bits_inter(core_t *core, lbac_t *lbac, s32 slice_type)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    s16(*coef)[MAX_CU_DIM] = cur_info->coef;
    s8 *refi = cur_info->refi;
    s32 cup = core->cu_scup_in_pic;
    int refi0, refi1;
    int vertex = 0;
    int vertex_num = cur_info->affine_flag + 1;

    int mvr_idx = cur_info->mvr_idx;

    assert(slice_type != SLICE_I);
    assert(core->cons_pred_mode != ONLY_INTRA);
    int skip_flag = (cur_info->cu_mode == MODE_SKIP);
    int dir_flag = (cur_info->cu_mode == MODE_DIR);

    if (core->cons_pred_mode != ONLY_INTRA) {
        lbac_enc_skip_flag(lbac, NULL, core, skip_flag);
        if (!skip_flag) {
            lbac_enc_direct_flag(lbac, NULL, core, dir_flag);
        }
    }

    if (skip_flag || dir_flag) {
        assert(cur_info->pb_part == SIZE_2Nx2N);
        if (info->sqh.umve_enable) {
            lbac_enc_umve_flag(lbac, NULL, cur_info->umve_flag);
        }
        if (cur_info->umve_flag) {
            lbac_enc_umve_idx(lbac, NULL, cur_info->umve_idx);
        } else {
            lbac_enc_affine_flag(lbac, NULL, core, cur_info->affine_flag != 0); /* skip/direct affine_flag */
            if (cur_info->affine_flag) {
                lbac_enc_affine_mrg_idx(lbac, NULL, cur_info->skip_idx);
            } else {
                lbac_enc_skip_idx(lbac, NULL, core->pichdr, cur_info->skip_idx, info->sqh.num_of_hmvp);
            }
        }
    } else {
        if (core->cons_pred_mode == NO_MODE_CONS) {
            lbac_enc_pred_mode(lbac, NULL, core, MODE_INTER);
        }

        lbac_enc_affine_flag(lbac, NULL, core, cur_info->affine_flag != 0);

        if (info->sqh.amvr_enable) {
            if (info->sqh.emvr_enable && !cur_info->affine_flag) {
                lbac_enc_mvr_flag_extend(lbac, NULL, cur_info->hmvp_flag);
            }
            lbac_enc_mvr_idx(lbac, NULL, mvr_idx, cur_info->affine_flag);
        }

        {
            if (slice_type == SLICE_B) {
                lbac_enc_inter_dir(lbac, NULL, core, refi, cur_info->pb_part);
            }

            refi0 = refi[REFP_0];
            refi1 = refi[REFP_1];

            if (info->sqh.smvd_enable && REFI_IS_VALID(refi0) && REFI_IS_VALID(refi1)
                && (core->ptr - core->refp[0][REFP_0].ptr == core->refp[0][REFP_1].ptr - core->ptr) && cur_info->affine_flag == 0
                && !cur_info->hmvp_flag) {
                lbac_enc_smvd_flag(lbac, NULL, cur_info->smvd_flag);
            }
            // forward
            if (IS_INTER_SLICE(slice_type) && REFI_IS_VALID(refi0)) {
                if (cur_info->smvd_flag == 0) {
                    lbac_enc_refi(lbac, NULL, core->num_refp[REFP_0], refi0);
                }

                if (cur_info->affine_flag) {
                    for (vertex = 0; vertex < vertex_num; vertex++) {
                        s16 affine_mvd_real[MV_D];
                        u8 amvr_shift = Tab_Affine_AMVR(mvr_idx);
                        affine_mvd_real[MV_X] = cur_info->affine_mvd[REFP_0][vertex][MV_X] >> amvr_shift;
                        affine_mvd_real[MV_Y] = cur_info->affine_mvd[REFP_0][vertex][MV_Y] >> amvr_shift;
                        lbac_enc_mvd(lbac, NULL, affine_mvd_real);
                    }
                } else {
                    s16 mvd[MV_D];
                    mvd[MV_X] = cur_info->mvd[REFP_0][MV_X] >> mvr_idx;
                    mvd[MV_Y] = cur_info->mvd[REFP_0][MV_Y] >> mvr_idx;
                    lbac_enc_mvd(lbac, NULL, mvd);
                }
            }

            // backward
            if (slice_type == SLICE_B && REFI_IS_VALID(refi1)) {
                if (cur_info->smvd_flag == 0) {
                    lbac_enc_refi(lbac, NULL, core->num_refp[REFP_1], refi1);
                }

                if (cur_info->affine_flag) {
                    for (vertex = 0; vertex < vertex_num; vertex++) {
                        s16 affine_mvd_real[MV_D];
                        u8 amvr_shift = Tab_Affine_AMVR(mvr_idx);
                        affine_mvd_real[MV_X] = cur_info->affine_mvd[REFP_1][vertex][MV_X] >> amvr_shift;
                        affine_mvd_real[MV_Y] = cur_info->affine_mvd[REFP_1][vertex][MV_Y] >> amvr_shift;
                        lbac_enc_mvd(lbac, NULL, affine_mvd_real);
                    }
                } else {
                    if (cur_info->smvd_flag == 0) {
                        s16 mvd[MV_D];
                        mvd[MV_X] = cur_info->mvd[REFP_1][MV_X] >> mvr_idx;
                        mvd[MV_Y] = cur_info->mvd[REFP_1][MV_Y] >> mvr_idx;
                        lbac_enc_mvd(lbac, NULL, mvd);
                    }
                }
            }
        }
    }

    if (!skip_flag) {
        assert(core->tree_status != TREE_C);
        lbac_enc_coef(lbac, NULL, core, coef, core->cu_width_log2, core->cu_height_log2, cur_info->cu_mode, cur_info, core->tree_status);
    }
}

static int copy_cu_data(enc_cu_t *dst, enc_cu_t *src, int x, int y, int cu_width_log2, int cu_height_log2, int log2_cus, int cud, u8 tree_status)
{
    int i, j, k;
    int cx = x >> MIN_CU_LOG2;
    int cy = y >> MIN_CU_LOG2;
    int cu_width  = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    int cus       = 1 << log2_cus;
    int cuw_scu   = 1 << (cu_width_log2  - MIN_CU_LOG2);
    int cuh_scu   = 1 << (cu_height_log2 - MIN_CU_LOG2);
    int cus_scu   = 1 << (log2_cus       - MIN_CU_LOG2);

    assert(tree_status != TREE_C);

    for (j = 0; j < cuh_scu; j++) {
        int idx_dst = (cy + j) * cus_scu + cx;
        int idx_src = j * cuw_scu;
 
        for (i = 0; i < cuw_scu; i++) {
            com_mcpy(dst->split_mode[idx_dst + i][cud], src->split_mode[idx_src + i][cud], (MAX_CU_DEPTH - cud) * NUM_BLOCK_SHAPE * sizeof(s8));
        }
        for (k = 0; k < N_C; k++) {
            com_mcpy(dst->num_nz_coef[k] + idx_dst, src->num_nz_coef[k] + idx_src, cuw_scu * sizeof(int));
        }
        com_mcpy(dst->pred_mode   + idx_dst, src->pred_mode   + idx_src, cuw_scu);
        com_mcpy(dst->mpm         + idx_dst, src->mpm         + idx_src, cuw_scu * 2);
        com_mcpy(dst->ipm_l       + idx_dst, src->ipm_l       + idx_src, cuw_scu);
        com_mcpy(dst->ipm_c       + idx_dst, src->ipm_c       + idx_src, cuw_scu);
        com_mcpy(dst->affine_flag + idx_dst, src->affine_flag + idx_src, cuw_scu);
        com_mcpy(dst->smvd_flag   + idx_dst, src->smvd_flag   + idx_src, cuw_scu);
        com_mcpy(dst->map_scu     + idx_dst, src->map_scu     + idx_src, cuw_scu * sizeof(com_scu_t));
        com_mcpy(dst->map_pos     + idx_dst, src->map_pos     + idx_src, cuw_scu * sizeof(u32));
        com_mcpy(dst->refi        + idx_dst, src->refi        + idx_src, cuw_scu * REFP_NUM);
        com_mcpy(dst->umve_flag   + idx_dst, src->umve_flag   + idx_src, cuw_scu);
        com_mcpy(dst->umve_idx    + idx_dst, src->umve_idx    + idx_src, cuw_scu);
        com_mcpy(dst->skip_idx    + idx_dst, src->skip_idx    + idx_src, cuw_scu);
        com_mcpy(dst->mvr_idx     + idx_dst, src->mvr_idx     + idx_src, cuw_scu);
        com_mcpy(dst->hmvp_flag   + idx_dst, src->hmvp_flag   + idx_src, cuw_scu);
        com_mcpy(dst->ipf_flag    + idx_dst, src->ipf_flag    + idx_src, cuw_scu);
        com_mcpy(dst->mv          + idx_dst, src->mv          + idx_src, cuw_scu * sizeof(s16) * REFP_NUM * MV_D);
        com_mcpy(dst->mvd         + idx_dst, src->mvd         + idx_src, cuw_scu * sizeof(s16) * REFP_NUM * MV_D);
        com_mcpy(dst->pb_part     + idx_dst, src->pb_part     + idx_src, cuw_scu * sizeof(int));
        com_mcpy(dst->tb_part     + idx_dst, src->tb_part     + idx_src, cuw_scu * sizeof(int));
        com_mcpy(dst->qtd         + idx_dst, src->qtd         + idx_src, cuw_scu * sizeof(s8));
        com_mcpy(dst->border      + idx_dst, src->border      + idx_src, cuw_scu * sizeof(s8));
        com_mcpy(dst->cudepth     + idx_dst, src->cudepth     + idx_src, cuw_scu * sizeof(s8));
    }
    for (j = 0; j < cu_height; j++) {
        int idx_dst = (y + j) * cus + x;
        int idx_src = j * cu_width;
        com_mcpy(dst->coef[Y_C] + idx_dst, src->coef[Y_C] + idx_src, cu_width * sizeof(s16));
        com_mcpy(dst->reco[Y_C] + idx_dst, src->reco[Y_C] + idx_src, cu_width * sizeof(pel));
    }
    for (j = 0; j < cu_height >> 1; j++) {
        int idx_dst = ((y >> 1) + j) * (cus >> 1) + (x >> 1);
        int idx_src = j * (cu_width >> 1);
        com_mcpy(dst->coef[U_C] + idx_dst, src->coef[U_C] + idx_src, (cu_width >> 1) * sizeof(s16));
        com_mcpy(dst->coef[V_C] + idx_dst, src->coef[V_C] + idx_src, (cu_width >> 1) * sizeof(s16));
        com_mcpy(dst->reco[U_C] + idx_dst, src->reco[U_C] + idx_src, (cu_width >> 1) * sizeof(pel));
        com_mcpy(dst->reco[V_C] + idx_dst, src->reco[V_C] + idx_src, (cu_width >> 1) * sizeof(pel));
    }
    return COM_OK;
}

static int enc_mode_init_cu(core_t *core, int x, int y, int cu_width_log2, int cu_height_log2)
{
    com_info_t *info     =  core->info;
    com_mode_t *bst_info = &core->mod_info_best;
    com_mode_t *cur_info = &core->mod_info_curr;

    core->cu_pix_x        = x;
    core->cu_pix_y        = y;
    core->cu_width        = 1 << cu_width_log2;
    core->cu_height       = 1 << cu_height_log2;
    core->cu_width_log2   = cu_width_log2;
    core->cu_height_log2  = cu_height_log2;
    core->cu_scu_x        = PEL2SCU(x);
    core->cu_scu_y        = PEL2SCU(y);
    core->cu_scup_in_pic  = ((u32)core->cu_scu_y * info->i_scu) + core->cu_scu_x;
    core->skip_mvps_check = 1;

    cu_nz_cln(cur_info->num_nz);
    cu_nz_cln(bst_info->num_nz);

    if (core->tree_status == TREE_C) {
        return COM_OK;
    }

    //init the best cu info
    init_pb_part(bst_info);
    init_tb_part(bst_info);
    get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, bst_info->pb_part, &bst_info->pb_info);
    get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, bst_info->tb_part, &bst_info->tb_info);
    assert(bst_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);
   
    com_mset(bst_info->mv,   0, sizeof(s16) * REFP_NUM * MV_D);
    com_mset(bst_info->mvd,  0, sizeof(s16) * REFP_NUM * MV_D);
    com_mset(bst_info->refi, 0, sizeof(s16) * REFP_NUM);

    bst_info->cu_mode     = MODE_INTRA;
    bst_info->mvr_idx     =  0;
    bst_info->skip_idx    =  0;
    bst_info->umve_flag   =  0;
    bst_info->umve_idx    = -1;
    bst_info->hmvp_flag   =  0;
    bst_info->ipf_flag    =  0;
    bst_info->affine_flag =  0;
    bst_info->smvd_flag   =  0;
    cur_info->affine_flag =  0;
    cur_info->smvd_flag   =  0;

    com_mset(bst_info->affine_mv,  0, sizeof(CPMV) * REFP_NUM * VER_NUM * MV_D);
    com_mset(bst_info->affine_mvd, 0, sizeof( s16) * REFP_NUM * VER_NUM * MV_D);

    rdoq_init_cu_est_bits(core, &core->lbac_bakup);

    return COM_OK;
}

static void cpy_rec_2_pic(core_t *core, int x, int y, int w, int h, com_pic_t *pic, u8 tree_status)
{
    enc_cu_t *cu_data;
    pel           *src, *dst;
    int            j, s_pic, off, size;
    int            log2_w, log2_h;
    int            stride;
    log2_w = CONV_LOG2(w);
    log2_h = CONV_LOG2(h);
    cu_data = &core->cu_data_best[log2_w - 2][log2_h - 2];
    s_pic = pic->stride_luma;
    stride = w;
    if (x + w > pic->width_luma) {
        w = pic->width_luma - x;
    }
    if (y + h > pic->height_luma) {
        h = pic->height_luma - y;
    }

    assert(tree_status != TREE_C);

    /* luma */
    if (tree_status != TREE_C) {
        src = cu_data->reco[Y_C];
        dst = pic->y + x + y * s_pic;
        size = sizeof(pel) * w;
        for (j = 0; j < h; j++) {
            com_mcpy(dst, src, size);
            src += stride;
            dst += s_pic;
        }

    }

    /* chroma */
    if (tree_status != TREE_L) {
        s_pic = pic->stride_chroma;
        off = (x >> 1) + (y >> 1) * s_pic;
        size = (sizeof(pel) * w) >> 1;
        src = cu_data->reco[U_C];
        dst = pic->u + off;
        for (j = 0; j < (h >> 1); j++) {
            com_mcpy(dst, src, size);
            src += (stride >> 1);
            dst += s_pic;
        }
        src = cu_data->reco[V_C];
        dst = pic->v + off;
        for (j = 0; j < (h >> 1); j++) {
            com_mcpy(dst, src, size);
            src += (stride >> 1);
            dst += s_pic;
        }
    }
}

static void set_affine_mvf(core_t *core, com_mode_t *mi, enc_cu_t *cu_data)
{
    int   cu_width_log2, cu_height_log2;
    int   cu_w_in_scu, cu_h_in_scu;
    int   sub_w_in_scu, sub_h_in_scu;
    int   w, h, x, y;
    int   lidx;
    int   idx;
    int   cp_num = mi->affine_flag + 1;
    int   aff_scup[VER_NUM];
    int   sub_w = 4;
    int   sub_h = 4;

    if (core->pichdr->affine_subblk_size_idx > 0) {
        sub_w = 8;
        sub_h = 8;
    }
    if (REFI_IS_VALID(mi->refi[REFP_0]) && REFI_IS_VALID(mi->refi[REFP_1])) {
        sub_w = 8;
        sub_h = 8;
    }

    int   half_w = sub_w >> 1;
    int   half_h = sub_h >> 1;

    sub_w_in_scu = sub_w >> MIN_CU_LOG2;
    sub_h_in_scu = sub_h >> MIN_CU_LOG2;

    cu_width_log2 = CONV_LOG2(core->cu_width);
    cu_height_log2 = CONV_LOG2(core->cu_height);
    cu_w_in_scu = core->cu_width >> MIN_CU_LOG2;
    cu_h_in_scu = core->cu_height >> MIN_CU_LOG2;
    aff_scup[0] = 0;
    aff_scup[1] = (cu_w_in_scu - 1);
    aff_scup[2] = (cu_h_in_scu - 1) * cu_w_in_scu;
    aff_scup[3] = (cu_w_in_scu - 1) + (cu_h_in_scu - 1) * cu_w_in_scu;

    for (lidx = 0; lidx < REFP_NUM; lidx++) {
        if (mi->refi[lidx] >= 0) {
            CPMV(*ac_mv)[MV_D] = mi->affine_mv[lidx];
            s32 dmv_hor_x, dmv_ver_x, dmv_hor_y, dmv_ver_y;
            s32 mv_scale_hor = (s32)ac_mv[0][MV_X] << 7;
            s32 mv_scale_ver = (s32)ac_mv[0][MV_Y] << 7;
            s32 mv_scale_tmp_hor, mv_scale_tmp_ver;

            dmv_hor_x = (((s32)ac_mv[1][MV_X] - (s32)ac_mv[0][MV_X]) << 7) >> core->cu_width_log2;      
            dmv_hor_y = (((s32)ac_mv[1][MV_Y] - (s32)ac_mv[0][MV_Y]) << 7) >> core->cu_width_log2;

            if (cp_num == 3) {
                dmv_ver_x = (((s32)ac_mv[2][MV_X] - (s32)ac_mv[0][MV_X]) << 7) >> core->cu_height_log2;
                dmv_ver_y = (((s32)ac_mv[2][MV_Y] - (s32)ac_mv[0][MV_Y]) << 7) >> core->cu_height_log2;
            } else {
                dmv_ver_x = -dmv_hor_y;  
                dmv_ver_y = dmv_hor_x;
            }

            idx = 0;
            for (h = 0; h < cu_h_in_scu; h += sub_h_in_scu) {
                for (w = 0; w < cu_w_in_scu; w += sub_w_in_scu) {
                    int pos_x, pos_y;

                    if (w == 0 && h == 0) {
                        pos_x = 0;
                        pos_y = 0;
                    } else if (w + sub_w_in_scu == cu_w_in_scu && h == 0) {
                        pos_x = cu_w_in_scu << MIN_CU_LOG2;
                        pos_y = 0;
                    } else if (w == 0 && h + sub_h_in_scu == cu_h_in_scu && cp_num == 3) {
                        pos_x = 0;
                        pos_y = cu_h_in_scu << MIN_CU_LOG2;
                    } else {
                        pos_x = (w << MIN_CU_LOG2) + half_w;
                        pos_y = (h << MIN_CU_LOG2) + half_h;
                    }

                    mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
                    mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;

                    // 1/16 precision, 18 bits for mc
                    com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
                    mv_scale_tmp_hor = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_hor);
                    mv_scale_tmp_ver = COM_CLIP3(COM_INT18_MIN, COM_INT18_MAX, mv_scale_tmp_ver);

                    // 1/4 precision, 16 bits for storage
                    com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 2, 0);
                    mv_scale_tmp_hor = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_scale_tmp_hor);
                    mv_scale_tmp_ver = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_scale_tmp_ver);

                    for (y = h; y < h + sub_h_in_scu; y++) {
                        for (x = w; x < w + sub_w_in_scu; x++) {
                            idx = x + y * cu_w_in_scu;
                            cu_data->mv[idx][lidx][MV_X] = (s16)mv_scale_tmp_hor;
                            cu_data->mv[idx][lidx][MV_Y] = (s16)mv_scale_tmp_ver;
                        }
                    }
                }
            }

            for (h = 0; h < cp_num; h++) {
                cu_data->mvd[aff_scup[h]][lidx][MV_X] = mi->affine_mvd[lidx][h][MV_X];
                cu_data->mvd[aff_scup[h]][lidx][MV_Y] = mi->affine_mvd[lidx][h][MV_Y];
            }
        }
    }
}

static void copy_to_cu_data(core_t *core, com_mode_t *mi, enc_cu_t *cu_data)
{
    int cu_width_log2  = CONV_LOG2(core->cu_width);
    int cu_height_log2 = CONV_LOG2(core->cu_height);

    if (core->tree_status != TREE_C) {
        com_mcpy(cu_data->coef[Y_C], mi->coef[Y_C], core->cu_width * core->cu_height * sizeof(s16));
        com_mcpy(cu_data->reco[Y_C], mi->rec [Y_C], core->cu_width * core->cu_height * sizeof(pel));
    }
    if (core->tree_status != TREE_L) {
        int size = (core->cu_width * core->cu_height * sizeof(s16)) >> 2;
        com_mcpy(cu_data->coef[U_C], mi->coef[U_C], size);
        com_mcpy(cu_data->coef[V_C], mi->coef[V_C], size);
        size = (core->cu_width * core->cu_height * sizeof(pel)) >> 2;
        com_mcpy(cu_data->reco[U_C], mi->rec[U_C], size);
        com_mcpy(cu_data->reco[V_C], mi->rec[V_C], size);
    }

    /* mode info */
    if (core->tree_status == TREE_C) {
        for (int j = 0; j < core->cu_height >> MIN_CU_LOG2; j++) {
            int idx = j * (core->cu_width >> MIN_CU_LOG2);

            for (int i = 0; i < core->cu_width >> MIN_CU_LOG2; i++, idx++) {
                if (mi->cu_mode == MODE_INTRA) {
                    cu_data->ipm_c[idx] = mi->ipm[PB0][1];
                }
                cu_data->num_nz_coef[1][idx] = mi->num_nz[TBUV0][1];
                cu_data->num_nz_coef[2][idx] = mi->num_nz[TBUV0][2];
            }
        }
    } else {
        int cu_cbf_flag;
        com_scu_t scu;
        u32 pos;

        if (core->tree_status == TREE_LC) {
            cu_cbf_flag = is_cu_nz(mi->num_nz);
        } else if (core->tree_status == TREE_L) {
            cu_cbf_flag = is_cu_plane_nz(mi->num_nz, Y_C);
        }
        scu.coded = 1;
        scu.intra = mi->cu_mode == MODE_INTRA;
        scu.cbf = cu_cbf_flag;
        scu.skip = mi->cu_mode == MODE_SKIP;
        scu.affine = mi->affine_flag;
        scu.tbpart = mi->tb_part;

        MCU_SET_SCUP(pos, core->cu_scup_in_pic);
        MCU_SET_LOGW(pos, cu_width_log2);
        MCU_SET_LOGH(pos, cu_height_log2);

        for (int j = 0; j < core->cu_height >> MIN_CU_LOG2; j++) {
            int idx = j * (core->cu_width >> MIN_CU_LOG2);

            for (int i = 0; i < core->cu_width >> MIN_CU_LOG2; i++, idx++) {
                int tb_idx_y = get_part_idx(mi->tb_part, i << 2, j << 2, core->cu_width, core->cu_height);

                cu_data->map_scu     [idx] = scu;
                cu_data->map_pos     [idx] = pos;
                cu_data->pred_mode   [idx] = (u8)mi->cu_mode;
                cu_data->umve_flag   [idx] = mi->umve_flag;
                cu_data->umve_idx    [idx] = mi->umve_idx;
                cu_data->pb_part     [idx] = mi->pb_part;
                cu_data->tb_part     [idx] = mi->tb_part;
                cu_data->affine_flag [idx] = mi->affine_flag;

                cu_data->num_nz_coef[0][idx] = mi->num_nz[tb_idx_y][0];
                cu_data->num_nz_coef[1][idx] = mi->num_nz[0][1];
                cu_data->num_nz_coef[2][idx] = mi->num_nz[0][2];

                if (mi->cu_mode == MODE_INTRA) {
                    int pb_idx_y = get_part_idx(mi->pb_part, i << 2, j << 2, core->cu_width, core->cu_height);
                    M16 (cu_data->refi[idx]) = -1;
                    CP16(cu_data->mpm [idx], mi->mpm[pb_idx_y]);
                    cu_data->ipm_l    [idx] = mi->ipm[pb_idx_y][0];
                    cu_data->ipm_c    [idx] = mi->ipm[0][1];
                    cu_data->ipf_flag [idx] = mi->ipf_flag;
                } else {
                    CP16(cu_data->refi[idx], mi->refi);
                    CP64(cu_data->mv  [idx], mi->mv);
                    CP64(cu_data->mvd [idx], mi->mvd);
                    cu_data->mvr_idx  [idx] = mi->mvr_idx;
                    cu_data->hmvp_flag[idx] = mi->hmvp_flag;
                    cu_data->smvd_flag[idx] = mi->smvd_flag;
                    cu_data->skip_idx [idx] = mi->skip_idx;
                }
            }
        }
        if (mi->affine_flag) {
            set_affine_mvf(core, mi, cu_data);
        }
    }
}

static void update_map_scu(core_t *core, int x, int y, int src_cuw, int src_cuh)
{
    com_map_t *map = core->map;
    com_info_t *info = core->info;
    int i_src = src_cuw >> MIN_CU_LOG2;
    int i_dst = info->i_scu;
    int map_offset = (y >> MIN_CU_LOG2) * i_dst + (x >> MIN_CU_LOG2);
    enc_cu_t *cu_data_bst = &core->cu_data_best[CONV_LOG2(src_cuw) - 2][CONV_LOG2(src_cuh) - 2];

    com_scu_t *src_map_scu               = cu_data_bst->map_scu;
    s8      *src_map_ipm                 = cu_data_bst->ipm_l;
    s16    (*src_map_mv)[REFP_NUM][MV_D] = cu_data_bst->mv;
    s8     (*src_map_refi)[REFP_NUM]     = cu_data_bst->refi;
    u32     *src_map_cu_mode             = cu_data_bst->map_pos;
    s8      *src_map_cud                 = cu_data_bst->qtd;

    com_scu_t *dst_map_scu               = map->map_scu  + map_offset;
    s8      *dst_map_ipm                 = map->map_ipm  + map_offset;
    s16    (*dst_map_mv)[REFP_NUM][MV_D] = map->map_mv   + map_offset;
    s8     (*dst_map_refi)[REFP_NUM]     = map->map_refi + map_offset;
    u32     *dst_map_pos                 = map->map_pos  + map_offset;
    s8      *dst_map_cud                 = map->map_cud  + map_offset;

    int w = COM_MIN(src_cuw, info->pic_width  - x) >> MIN_CU_LOG2;
    int h = COM_MIN(src_cuh, info->pic_height - y) >> MIN_CU_LOG2;
  
    int size_scu  = sizeof(com_scu_t) * w;
    int size_pos  = sizeof(      u32) * w;
    int size_ipm  = sizeof(       u8) * w;
    int size_mv   = sizeof(      s16) * w * REFP_NUM * MV_D;
    int size_refi = sizeof(       s8) * w * REFP_NUM;
    int size_cud  = sizeof(       s8) * w;

    assert(core->tree_status != TREE_C);

    for (int i = 0; i < h; i++) {
#define COPY_ONE_DATA(d,s,size) com_mcpy(d, s, size); d += i_dst; s += i_src;
        COPY_ONE_DATA(dst_map_scu,  src_map_scu,     size_scu);
        COPY_ONE_DATA(dst_map_pos,  src_map_cu_mode, size_pos);
        COPY_ONE_DATA(dst_map_ipm,  src_map_ipm,     size_ipm);
        COPY_ONE_DATA(dst_map_mv,   src_map_mv,      size_mv);
        COPY_ONE_DATA(dst_map_refi, src_map_refi,    size_refi);
        COPY_ONE_DATA(dst_map_cud,  src_map_cud,     size_cud);
#undef COPY_ONE_DATA
    }
}

static void clear_map_scu(core_t *core, int x, int y, int cu_width, int cu_height)
{
    com_map_t *map = core->map;
    com_info_t *info = core->info;
    int i_dst = info->i_scu;
    int map_offset = (y >> MIN_CU_LOG2) * i_dst + (x >> MIN_CU_LOG2);
    com_scu_t *map_scu        = map->map_scu  + map_offset;
    s8(*map_refi)[REFP_NUM] = map->map_refi + map_offset;

    int w = COM_MIN(cu_width,  info->pic_width  - x) >> MIN_CU_LOG2;
    int h = COM_MIN(cu_height, info->pic_height - y) >> MIN_CU_LOG2;

    int size_scu  = sizeof(com_scu_t) * w;
    int size_refi = sizeof(     s8) * w * REFP_NUM;

    for (int i = 0; i < h; i++) {
#define CLEAR_ONE_DATA(d,v,size) com_mset(d, v, size); d += i_dst;
        CLEAR_ONE_DATA(map_scu,   0, size_scu );
        CLEAR_ONE_DATA(map_refi, -1, size_refi);
#undef CLEAR_ONE_DATA
    }
}

static double mode_coding_unit(core_t *core, lbac_t *lbac_best, int x, int y, int cu_width_log2, int cu_height_log2, int cud, enc_cu_t *cu_data, int texture_dir)
{
    com_info_t *info     = core->info;
    com_mode_t *bst_info = &core->mod_info_best;
    com_mode_t *cur_info = &core->mod_info_curr;
    int cu_width         = 1 << cu_width_log2;
    int cu_height        = 1 << cu_height_log2;
    int bit_depth        = info->bit_depth_internal;
    u8 cons_pred_mode    = NO_MODE_CONS;
    com_pic_t *pic_org   = core->pic_org;
    s8 ipf_passes_num    = (info->sqh.ipf_enable_flag && (cu_width < MAX_CU_SIZE) && (cu_height < MAX_CU_SIZE)) ? 2 : 1;
    double rdcost_intra  = MAX_D_COST;
    double rdcost_inter  = MAX_D_COST;

    enc_mode_init_cu(core, x, y, cu_width_log2, cu_height_log2);

    cons_pred_mode = core->cons_pred_mode;

    if (core->tree_status == TREE_C) {
        int cu_w_scu = PEL2SCU(cu_width);
        int cu_h_scu = PEL2SCU(cu_height);
        int luma_pos_scu = (cu_w_scu - 1) + (cu_h_scu - 1) * cu_w_scu; // bottom-right
        u8  luma_pred_mode = cu_data->pred_mode[luma_pos_scu];
        cons_pred_mode = (luma_pred_mode == MODE_INTRA) ? ONLY_INTRA : ONLY_INTER;

        if (luma_pred_mode != MODE_INTRA) {
            double cost = pinter_residue_rdo_chroma(core);
            copy_to_cu_data(core, bst_info, cu_data);
            return cost;
        } else {
            ipf_passes_num = 1;
        }
    }

    //**************** inter ********************
    double cost_best = MAX_D_COST;
    core->cost_best  = MAX_D_COST;
    core->inter_satd = COM_UINT64_MAX;

    if (core->slice_type != SLICE_I && cons_pred_mode != ONLY_INTRA) {
        analyze_inter_cu(core, lbac_best);

        if (core->cost_best < cost_best) {
            cost_best = core->cost_best;
            copy_to_cu_data(core, bst_info, cu_data);
        }
        rdcost_inter = cost_best;
    }

    //**************** intra ********************
    if ((core->slice_type == SLICE_I || is_cu_nz(bst_info->num_nz) || cost_best > MAX_D_COST_EXT) && cons_pred_mode != ONLY_INTER) {
        enc_history_t *history = &core->history_data[cu_width_log2 - 2][cu_height_log2 - 2][core->cu_scup_in_lcu];

        if (cu_width <= 64 && cu_height <= 64 && (!history->skip_intra || cons_pred_mode == ONLY_INTRA)) {
            core->dist_cu_best = COM_UINT64_MAX;
   
            for (int ipf_flag = 0; ipf_flag < ipf_passes_num; ++ipf_flag) {
                cur_info->ipf_flag = ipf_flag;
                
                double rdcost = analyze_intra_cu(core, lbac_best, texture_dir);

                if (core->cost_best < cost_best) {
                    cost_best = core->cost_best;
                    core->dist_cu_best = core->dist_cu;

                    bst_info->cu_mode = MODE_INTRA;
                    bst_info->ipf_flag = ipf_flag;
                    bst_info->pb_part = core->best_pb_part_intra;
                    bst_info->tb_part = core->best_tb_part_intra;
                    bst_info->affine_flag = 0;
                    copy_to_cu_data(core, bst_info, cu_data);
                }
                if (rdcost < rdcost_intra) {
                    rdcost_intra = rdcost;
                }
            }

            if (info->history_skip_intra && rdcost_intra < MAX_D_COST_EXT && rdcost_inter < MAX_D_COST_EXT) {
                if (rdcost_intra > 1.001 * rdcost_inter) {
                    history->skip_intra = 1;
                }
            }
        }
    }

    return cost_best;
}

static void check_history_split_result(core_t *core, int cu_width_log2, int cu_height_log2, int cup, int *split_allow)
{
    int i;
    double min_cost = MAX_D_COST;
    int run_list[NUM_SPLIT_MODE];
    enc_history_t *history = &core->history_data[cu_width_log2 - 2][cu_height_log2 - 2][cup];

    if (history->visit_split) {
        if (history->split) {
            run_list[0] = 0;
            for (i = 1; i < NUM_SPLIT_MODE; i++) {
                if (history->split_cost[i] < min_cost && split_allow[i]) {
                    min_cost = history->split_cost[i];
                }
            }
            if (min_cost > MAX_D_COST_EXT) {
                run_list[0] = 1;
                for (i = 1; i < NUM_SPLIT_MODE; i++) {
                    run_list[i] = 0;
                }
            } else {
                for (i = 1; i < NUM_SPLIT_MODE; i++) {
                    double th = (min_cost < 0) ? 0.99 : 1.01;
                    if (history->split_cost[i] <= th * min_cost) {
                        run_list[i] = 1;
                    } else {
                        run_list[i] = 0;
                    }
                }
            }
        } else {
            run_list[0] = 1;
            for (i = 1; i < NUM_SPLIT_MODE; i++) {
                run_list[i] = 0;
            }
        }
   
        int num_run = 0;
        for (i = 1; i < NUM_SPLIT_MODE; i++) {
            split_allow[i] = run_list[i] && split_allow[i];
            num_run += split_allow[i];
        }
        if (num_run == 0) {
            assert(split_allow[0] == 1);
        } else {
            split_allow[0] = run_list[0] && split_allow[0];
        }
    } else {
        for (i = 0; i < NUM_SPLIT_MODE; i++) {
            run_list[i] = 1;
            history->split_cost[i] = MAX_D_COST;
        }
    }
}

s64 calc_dist_filter_boundary(core_t *core, com_pic_t *pic_rec, com_pic_t *pic_org, int cu_width, int cu_height,
                                    pel *src, int s_src, int x, int y, u8 intra_flag, u8 cu_cbf, s8 *refi, s16(*mv)[MV_D], u8 is_mv_from_mvf, int only_delta)
{
    if (core->pichdr->loop_filter_disable_flag) {
        return 0;
    }
    pel df_buf[(MAX_CU_SIZE + 4) *(MAX_CU_SIZE + 4)];

    com_info_t *info     =  core->info;
    com_map_t  *map      =  core->map;
    com_mode_t *cur_info = &core->mod_info_curr;
    int bit_depth        = info->bit_depth_internal;
    int cu_width_log2    = CONV_LOG2(cu_width);
    const int pad        = 4;
    const int log2_pad   = CONV_LOG2(pad);
    int s_l_org          = pic_org->stride_luma;
    int s_l_rec          = pic_rec->stride_luma;
    int s_l_dbk          = cu_width + pad;
    pel *dst_y           = df_buf + pad * s_l_dbk + pad;
    pel *org_y           = pic_org->y + y * s_l_org + x;
    int x_scu            = x >> MIN_CU_LOG2;
    int y_scu            = y >> MIN_CU_LOG2;
    s64 dist_nofilt      = 0;
    s64 dist_filter      = 0;

    if (y != 0) { //copy top
        pel *d = dst_y - pad * s_l_dbk;
        pel *s = pic_rec->y + (y - pad) * s_l_rec + x;
        for (int i = 0; i < pad; i++) {
            com_mcpy(d, s, cu_width * sizeof(pel));
            d += s_l_dbk;
            s += s_l_rec;
        }
    }
    if (x != 0) { //copy left
        pel *d = dst_y;
        pel *sc = src;
        pel *sl = pic_rec->y + y * s_l_rec + x - pad;
        for (int i = 0; i < cu_height; i++) {
            com_mcpy(d-pad, sl, pad * sizeof(pel));
            com_mcpy(d, sc, cu_width * sizeof(pel));
            d  += s_l_dbk;
            sl += s_l_rec;
            sc += s_src;
        }
    } else {
        pel *d = dst_y;
        pel *s = src;

        for (int i = 0; i < cu_height; i++) { //copy curr&left block
            com_mcpy(d, s, cu_width * sizeof(pel));
            d += s_l_dbk, s += s_src;
        }
    }
    //******** no filter ****************************
    if (only_delta) {
        if (y != 0) {
            dist_nofilt += block_pel_ssd(cu_width_log2, cu_height + pad, dst_y - pad * s_l_dbk, org_y - pad * s_l_org, s_l_dbk, s_l_org, bit_depth);
        } else {
            dist_nofilt += block_pel_ssd(cu_width_log2, cu_height, dst_y, org_y, s_l_dbk, s_l_org, bit_depth);
        }
    } else if (y != 0) {
        dist_nofilt += block_pel_ssd(cu_width_log2, pad, dst_y - pad * s_l_dbk, org_y - pad * s_l_org, s_l_dbk, s_l_org, bit_depth);
    }
    if (x != 0) {
        dist_nofilt += block_pel_ssd(log2_pad, cu_height, dst_y - pad, org_y - pad, s_l_dbk, s_l_org, bit_depth);
    }

    //*********** filter ****************************
    int w_scu = cu_width  >> MIN_CU_LOG2;
    int h_scu = cu_height >> MIN_CU_LOG2;
    int idx = y_scu * info->i_scu + x_scu;
    com_scu_t scu = map->map_scu[idx];

    scu.intra  = intra_flag;
    scu.cbf    = cu_cbf;
    scu.tbpart = cur_info->tb_part;
    scu.coded  = 0; //clear coded (necessary)

    com_scu_t*pscu               = map->map_scu  + idx;
    s8(*map_refi)[REFP_NUM]      = map->map_refi + idx;
    s16(*map_mv)[REFP_NUM][MV_D] = map->map_mv   + idx;

    for (int j = 0; j < h_scu; j++) {
        if (refi != NULL && !is_mv_from_mvf) {
            for (int i = 0; i < w_scu; i++) {
                pscu[i] = scu;
                CP16(map_refi[i], refi);
                CP64(map_mv[i], mv);
            }
            pscu     += info->i_scu;
            map_refi += info->i_scu;
            map_mv   += info->i_scu;
        } else {
            for (int i = 0; i < w_scu; i++) {
                pscu[i] = scu;
            }
            pscu += info->i_scu;
        }
    }

    u8 df_rdo_edge[MAX_CU_CNT_IN_LCU];
    memset(df_rdo_edge, 0, sizeof(u8) * w_scu * h_scu);

    com_df_set_edge(info, map, df_rdo_edge, w_scu, core->refp, x, y, cu_width, cu_height, 0, 0, 0);
    com_df_rdo_luma(info, core->pichdr, map, df_rdo_edge, w_scu, dst_y, s_l_dbk, x, y, cu_width, cu_height);

    if (y != 0) {
        dist_filter += block_pel_ssd(cu_width_log2, cu_height + pad, dst_y - pad * s_l_dbk, org_y - pad * s_l_org, s_l_dbk, s_l_org, bit_depth);
    } else {
        dist_filter += block_pel_ssd(cu_width_log2, cu_height, dst_y, org_y, s_l_dbk, s_l_org, bit_depth);
    }
    if (x != 0) {
        dist_filter += block_pel_ssd(log2_pad, cu_height, dst_y - pad, org_y - pad, s_l_dbk, s_l_org, bit_depth);
    }

    return dist_filter - dist_nofilt;
}

static void check_neighbor_depth(core_t *core, int* split_allow, int x0, int y0, int cu_width, int cu_height, int qt_depth, int bet_depth, int boundary)
{
    com_info_t *info = core->info;

    int neb_addr[6];
    int valid_flag[6];
    int valid_num = 0;
    int neb_cud[6] = { -1 };
    int cu_width_in_scu  = x0 + cu_width  <= info->pic_width  ? cu_width  >> MIN_CU_LOG2 : (info->pic_width  - x0) >> MIN_CU_LOG2;
    int cu_height_in_scu = y0 + cu_height <= info->pic_height ? cu_height >> MIN_CU_LOG2 : (info->pic_height - y0) >> MIN_CU_LOG2;
    int cupthis = ((u32)PEL2SCU(y0) * info->i_scu) + PEL2SCU(x0);
    int min_cud = 100, max_cud = -1;
    int loop_cud = 0, loop_cubed = 0;
    com_map_t* map = core->map;

    //! F: left-below neighbor (inside)
    neb_addr[0] = cupthis + (cu_height_in_scu - 1) * info->i_scu - 1;
    valid_flag[0] = (x0 > 0 && neb_addr[0] >= 0 && map->map_cud[neb_addr[0]] != -1);
    if (valid_flag[0])
        neb_cud[0] = map->map_cud[neb_addr[0]];

    //! G: above-right neighbor (inside)
    neb_addr[1] = cupthis - info->i_scu + cu_width_in_scu - 1;
    valid_flag[1] = (y0 > 0 && neb_addr[1] >= 0 && map->map_cud[neb_addr[1]] != -1);
    if (valid_flag[1])
        neb_cud[1] = map->map_cud[neb_addr[1]];

    //! C: above-right neighbor (outside)
    neb_addr[2] = cupthis - info->i_scu + cu_width_in_scu;
    valid_flag[2] = (y0 > 0 && x0 < info->pic_width&& neb_addr[2] >= 0 && map->map_cud[neb_addr[2]] != -1);
    if (valid_flag[2])
        neb_cud[2] = map->map_cud[neb_addr[2]];

    //! A: left neighbor
    neb_addr[3] = cupthis - 1;
    valid_flag[3] = (x0 > 0 && neb_addr[3] >= 0 && map->map_cud[neb_addr[3]] != -1);
    if (valid_flag[3])
        neb_cud[3] = map->map_cud[neb_addr[3]];

    //! B: above neighbor
    neb_addr[4] = cupthis - info->i_scu;
    valid_flag[4] = (y0 > 0 && neb_addr[4] >= 0 && map->map_cud[neb_addr[4]] != -1);
    if (valid_flag[4])
        neb_cud[4] = map->map_cud[neb_addr[4]];

    //! D: above-left neighbor
    neb_addr[5] = cupthis - info->i_scu - 1;
    valid_flag[5] = (x0 > 0 && y0 > 0 && neb_addr[5] >= 0 && map->map_cud[neb_addr[5]] != -1);

    if (valid_flag[5]) {
        neb_cud[5] = map->map_cud[neb_addr[5]];
    }

    for (int i = 0; i < 6; i++) {
        if (valid_flag[i]) {
            valid_num++;
            if (neb_cud[i] < min_cud)
                min_cud = neb_cud[i];
            if (neb_cud[i] > max_cud)
                max_cud = neb_cud[i];
        }
    }

    if (max_cud == min_cud) {
        loop_cud = 1;
    } else {
        loop_cud = 0;
    }

    // qt depth
    if (qt_depth < min_cud - loop_cud && valid_num >= 2 && bet_depth == 0 && !boundary) {
        split_allow[SPLIT_QUAD] = 1;
        split_allow[SPLIT_BI_HOR] = 0;
        split_allow[SPLIT_EQT_HOR] = 0;
        split_allow[SPLIT_EQT_VER] = 0;
        split_allow[SPLIT_BI_VER] = 0;
    }
    if (qt_depth > max_cud + loop_cud && valid_num >= 2) {
        split_allow[SPLIT_QUAD] = 0;
    }
}

static void score_split(core_t* core, int cur_split, int x0, int y0, int cu_width, int cu_height, int split_score[5], int allow_EQT_V, int allow_EQT_H, enc_cu_t* cu_data_bst) {
    com_info_t* info = core->info;

    if (x0 + cu_width > info->pic_width || y0 + cu_height > info->pic_height) {
        return;
    }
    int w_in_scu = cu_width  >> MIN_CU_LOG2;
    int h_in_scu = cu_height >> MIN_CU_LOG2;

    if (cur_split == SPLIT_BI_HOR) {
        s8 *b = cu_data_bst->border + w_in_scu / 2;
        for (int i = 0; i < h_in_scu; i++, b += w_in_scu) {
            split_score[SPLIT_BI_VER] += (*b & 1) ? 1 : -1;
        }
        if (allow_EQT_H) {
            s8* b  = cu_data_bst->border + (h_in_scu / 4) * w_in_scu + w_in_scu / 2;

            for (int i = 0; i < h_in_scu / 2; i++, b += w_in_scu) {
                split_score[SPLIT_EQT_HOR] += (*b & 1) ? 1 : -1;
            }
        }
    } else if (cur_split == SPLIT_BI_VER) {
        s8 *b = cu_data_bst->border + h_in_scu / 2 * w_in_scu;
        for (int i = 0; i < w_in_scu; i++, b++) {
            split_score[SPLIT_BI_HOR] += (*b & 2) ? 1 : -1;
        }
        if (allow_EQT_V) {
            s8* b = cu_data_bst->border + w_in_scu / 4 + h_in_scu / 2 * w_in_scu;

            for (int i = 0; i < w_in_scu / 2; i++, b++) {
                split_score[SPLIT_EQT_VER] += (*b & 2) ? 1 : -1;
            }
        }
    } 

    if (allow_EQT_V) {
        s8* b1 = cu_data_bst->border + w_in_scu / 4;
        s8* b2 = b1 + w_in_scu / 2;

        for (int i = 0; i < h_in_scu; i++, b1 += w_in_scu, b2 += w_in_scu) {
            split_score[SPLIT_EQT_VER] += (*b1 & 1) ? 1 : -1;
            split_score[SPLIT_EQT_VER] += (*b2 & 1) ? 1 : -1;
        }
    }
    if (allow_EQT_H) {
        s8* b1 = cu_data_bst->border + h_in_scu / 4 * w_in_scu;
        s8* b2 = b1 + h_in_scu / 2 * w_in_scu;

        for (int i = 0; i < w_in_scu; i++, b1++, b2++) {
            split_score[SPLIT_EQT_HOR] += (*b1 & 2) ? 1 : -1;
            split_score[SPLIT_EQT_HOR] += (*b2 & 2) ? 1 : -1;
        }
    }

    return;
}

static int check_split_dir_by_sobel(core_t *core, int *split_allow, int x0, int y0, int cu_width, int cu_height)
{
    com_info_t *info = core->info;

    int check_sobel_cost = 1;
    int min_size = info->ai_split_dir_decision_P2 ? 16 : (info->ai_split_dir_decision_P1 ? 32 : 64); 

    if (cu_width < min_size || cu_height < min_size) {
        check_sobel_cost = 0;
    }
    if (info->ai_split_dir_decision_P2 && cu_width * cu_height <= 256) {
        check_sobel_cost = 0;
    }

    if (check_sobel_cost) {
        com_pic_t *pic_org = core->pic_org;
        int x = x0, y = y0, w = cu_width, h = cu_height;
        ////int ver, hor;
        int ver[5], hor[5];
        double cu_grad_dis[4];
        double cu_gradh_dis_max;
        double cu_gradv_dis_max;
        double avg_grad;
        double grad_dis_minmax;

        if (x == 0) {
            x = 1;
            w -= 1;
        }
        if (y == 0) {
            y = 1;
            h -= 1;
        }
        if (x + w == pic_org->width_luma) {
            w -= 1;
        }
        if (y + h == pic_org->height_luma) {
            h -= 1;
        }
        uavs3e_funs_handle.sobel_cost(pic_org->y + y * pic_org->stride_luma + x, pic_org->stride_luma, w, h, ver, hor);

        cu_grad_dis[0] = COM_MAX((ver[1] / (double)ver[2]), (ver[2] / (double)ver[1]));
        cu_grad_dis[1] = COM_MAX((ver[3] / (double)ver[4]), (ver[4] / (double)ver[3]));
        cu_grad_dis[2] = COM_MAX((hor[1] / (double)hor[3]), (hor[3] / (double)hor[1]));
        cu_grad_dis[3] = COM_MAX((hor[2] / (double)hor[4]), (hor[4] / (double)hor[2]));

        cu_gradh_dis_max = COM_MAX(cu_grad_dis[0], cu_grad_dis[1]);
        cu_gradv_dis_max = COM_MAX(cu_grad_dis[2], cu_grad_dis[3]);

        grad_dis_minmax = COM_MIN(cu_gradh_dis_max, cu_gradv_dis_max);
        avg_grad = (cu_grad_dis[0] + cu_grad_dis[1] + cu_grad_dis[2] + cu_grad_dis[3]);

        if (avg_grad > 2.0 * 4) {
            split_allow[NO_SPLIT] = 0;
        }
        else if (grad_dis_minmax > 2.0) {
            split_allow[NO_SPLIT] = 0;
        }

        if (ver[0] > 1.04 * hor[0]) {
            split_allow[SPLIT_BI_HOR] = 0;
            split_allow[SPLIT_EQT_HOR] = 0;
        }
        if (hor[0] > 1.04 * ver[0]) {
            split_allow[SPLIT_BI_VER] = 0;
            split_allow[SPLIT_EQT_VER] = 0;
        }
    }

    return 0;
}

static double mode_coding_tree(core_t *core, lbac_t *lbac_cur, int x0, int y0, int cup, int cu_width_log2, int cu_height_log2, int cud
                               , const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status)
{
    enc_cu_t *cu_data_tmp  = &core->cu_data_temp[cu_width_log2 - 2][cu_height_log2 - 2];       // stack structure 
    enc_cu_t *cu_data_bst  = &core->cu_data_best[cu_width_log2 - 2][cu_height_log2 - 2];       // stack structure 
    enc_history_t *history = &core->history_data[cu_width_log2 - 2][cu_height_log2 - 2][cup];  // stack structure 
    com_mode_t *bst_info   = &core->mod_info_best;
    int slice_type         =  core->pichdr->slice_type;
    com_info_t *info       =  core->info;
    int cu_width           = 1 << cu_width_log2;
    int cu_height          = 1 << cu_height_log2;
    int boundary           = !(x0 + cu_width <= info->pic_width && y0 + cu_height <= info->pic_height);

    s8 cnt_hmvp_cands_curr = 0;
    s8 cnt_hmvp_cands_last = 0;
    int num_split_tried    = 0;
    int num_split_to_try   = 0;
    double cost_best       = MAX_D_COST;
    double best_curr_cost  = MAX_D_COST;    
    s8 best_split_mode     = NO_SPLIT;
    u8 best_cons_pred_mode = NO_MODE_CONS;
    int next_split         = 1; //early termination flag
	int texture_dir      = 0;

    int score_from_qt [5] = { 0 };
    int score_from_btv[5] = { 0 };
    int score_from_bth[5] = { 0 };

    int split_allow[SPLIT_QUAD + 1];
    com_motion_t motion_cands_curr[ALLOWED_HMVP_NUM];
    com_motion_t motion_cands_last[ALLOWED_HMVP_NUM];
    lbac_t lbac_cur_node;

    if (info->sqh.num_of_hmvp) {
        copy_motion_table(motion_cands_last, &cnt_hmvp_cands_last, core->motion_cands, core->cnt_hmvp_cands);
    }
    core->tree_status    = tree_status;
    core->cons_pred_mode = cons_pred_mode;

    lbac_copy(&lbac_cur_node, lbac_cur);

    if (cu_width > MIN_CU_SIZE || cu_height > MIN_CU_SIZE) {
        // ******** 1. normatively split modes **********
        int boundary_b = boundary && (y0 + cu_height > info->pic_height) && !(x0 + cu_width > info->pic_width);
        int boundary_r = boundary && (x0 + cu_width > info->pic_width) && !(y0 + cu_height > info->pic_height);
        com_check_split_mode(&info->sqh, split_allow, cu_width_log2, cu_height_log2, boundary, boundary_b, boundary_r, info->log2_max_cuwh, parent_split, qt_depth, bet_depth, slice_type);
       
        for (int i = 1; i < NUM_SPLIT_MODE; i++) {
            num_split_to_try += split_allow[i];
        }
        // ******** 2. split constraints **********
        if (cu_width == 64 && cu_height == 128) {
            split_allow[SPLIT_BI_VER] = 0;
        }
        if (cu_width == 128 && cu_height == 64) {
            split_allow[SPLIT_BI_HOR] = 0;
        }
        if (core->slice_type == SLICE_I && cu_width == 128 && cu_height == 128) {
            split_allow[NO_SPLIT] = 0;
        }
        // ******** 3. fast algorithm to reduce modes **********

        // 3.1 limit bt size
        if (info->depth_max_bt_32 && (cu_width > 32 || cu_height > 32) && !boundary && split_allow[SPLIT_QUAD]) {
            split_allow[SPLIT_BI_VER] = 0;
            split_allow[SPLIT_BI_HOR] = 0;
        }

        if (info->depth_limit_part_ratio && (split_allow[NO_SPLIT] || split_allow[SPLIT_QUAD])) {
            int cu_w = cu_width;
            int cu_h = cu_height;

            if (split_allow[SPLIT_BI_HOR] && cu_w >= cu_h * 4) {
                split_allow[SPLIT_BI_HOR] = 0;
            }
            if (split_allow[SPLIT_BI_VER] && cu_h >= cu_w * 4) {
                split_allow[SPLIT_BI_VER] = 0;
            }
            if (split_allow[SPLIT_EQT_HOR] && cu_w >= cu_h * 2) {
                split_allow[SPLIT_EQT_HOR] = 0;
            }
            if (split_allow[SPLIT_EQT_VER] && cu_h >= cu_w * 2) {
                split_allow[SPLIT_EQT_VER] = 0;
            }
        }

        // 3.2 check history_split_result
        check_history_split_result(core, cu_width_log2, cu_height_log2, cup, split_allow);

        // 3.3 limit eqt size for all intra config
        if (info->ai_skip_large_cu_eqt == 1) { 
			if ((cu_width == 64) || (cu_width == 32 && cu_height == 64)) { // EQT-V is not allowed if max cu size is 64
				split_allow[SPLIT_EQT_VER] = 0;
			}
			if ((cu_height == 64) || (cu_width == 64 && cu_height == 32)) { // EQT-H is not allowed if max cu size is 64
				split_allow[SPLIT_EQT_HOR] = 0;
			}
		}
        // 3.4 check split direction for all intra config
        if (info->ai_split_dir_decision == 1 && (!boundary) && cu_width < 128 && cu_height < 128) {
            texture_dir = check_split_dir_by_sobel(core, split_allow, x0, y0, cu_width, cu_height);
		}
        // 3.5 check depth of neighbor scu
        if (info->depth_neb_qtd) {
            check_neighbor_depth(core, split_allow, x0, y0, cu_width, cu_height, qt_depth, bet_depth, boundary);

            num_split_to_try = 0;

            for (int i = 1; i < NUM_SPLIT_MODE; i++) {
                num_split_to_try += split_allow[i];
            }
            if (num_split_to_try == 0) {
                split_allow[NO_SPLIT] = 1;
            }
        }
    } else {
        split_allow[0] = 1;
        for (int i = 1; i < NUM_SPLIT_MODE; i++) {
            split_allow[i] = 0;
        }
    }
    double nscost = MAX_D_COST;
    if (!boundary) {
        double cost_temp = 0.0;

        if (split_allow[NO_SPLIT]) {
            if (cu_width > MIN_CU_SIZE || cu_height > MIN_CU_SIZE) {
                int bit_cnt = lbac_get_bits(lbac_cur);
                com_set_split_mode(NO_SPLIT, cud, 0, cu_width, cu_height, cu_width, cu_data_bst->split_mode);
                lbac_enc_split_mode(lbac_cur, NULL, core, NO_SPLIT, cud, 0, cu_width, cu_height, cu_width, parent_split, qt_depth, bet_depth, x0, y0);
                bit_cnt = lbac_get_bits(lbac_cur) - bit_cnt;
                cost_temp += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
            }
            lbac_copy(&core->lbac_bakup, lbac_cur);
            core->cu_scup_in_lcu = cup;
            clear_map_scu(core, x0, y0, cu_width, cu_height);

            if (info->sqh.num_of_hmvp && core->pichdr->slice_type != SLICE_I) {
                copy_motion_table(motion_cands_curr, &cnt_hmvp_cands_curr, motion_cands_last, cnt_hmvp_cands_last);
            }
            core->tree_status = tree_status;
            core->cons_pred_mode = cons_pred_mode;
            cost_temp += mode_coding_unit(core, lbac_cur, x0, y0, cu_width_log2, cu_height_log2, cud, cu_data_bst, texture_dir);

            memset(cu_data_bst->qtd, qt_depth, (cu_width >> MIN_CU_LOG2) * (cu_height >> MIN_CU_LOG2));
            memset(cu_data_bst->border, 0,     (cu_width >> MIN_CU_LOG2) * (cu_height >> MIN_CU_LOG2));
            memset(cu_data_bst->cudepth, 2 * (info->log2_max_cuwh - qt_depth) - cu_width_log2 - cu_height_log2, (cu_width >> MIN_CU_LOG2)* (cu_height >> MIN_CU_LOG2));
            s8* b = cu_data_bst->border;
            for (int i = 0; i < cu_width >> MIN_CU_LOG2; i++) {
                b[i] = 2;
            }
            for (int i = 0; i < cu_height >> MIN_CU_LOG2; i++, b += (cu_width >> MIN_CU_LOG2)) {
                *b |= 1;
            }

            if (info->sqh.num_of_hmvp && bst_info->cu_mode != MODE_INTRA && !bst_info->affine_flag) {
                update_skip_candidates(motion_cands_curr, &cnt_hmvp_cands_curr, info->sqh.num_of_hmvp, cu_data_bst->mv[0], cu_data_bst->refi[0]);
            }
            cost_best = cost_temp;
            best_split_mode = NO_SPLIT;
            best_cons_pred_mode = cons_pred_mode;
            history->visit_mode_decision = 1;
        } else {
            cost_temp = MAX_D_COST;
        }
        if (!history->visit_split) {
            history->split_cost[NO_SPLIT] = cost_temp;
            best_curr_cost = cost_temp;
        }
        nscost = cost_temp;
    }
 
    if (cost_best < MAX_D_COST_EXT && bst_info->cu_mode == MODE_SKIP) {
        int enc_ecu_depth = (core->pic_org->layer_id == FRM_DEPTH_5 ? 3 : 4);

        if (cud >= enc_ecu_depth) {
            next_split = 0;
        }
    }

    if (cost_best < MAX_D_COST_EXT) {
        if (slice_type == SLICE_I) {
            if (bst_info->ipm[PB0][0] != IPD_IPCM && core->dist_cu_best < ((u64)1 << (cu_width_log2 + cu_height_log2 + 7))) {
                u8 bits_inc_by_split = ((cu_width_log2 + cu_height_log2 >= 6) ? 2 : 0) + 8; //two split flags + one more (intra dir + cbf + edi_flag + mtr info) + 1-bit penalty, approximately 8 bits
                if (core->dist_cu_best < core->lambda[0] * bits_inc_by_split) {
                    next_split = 0;
                }
            }
        }
    }
    if ((cu_width > MIN_CU_SIZE || cu_height > MIN_CU_SIZE) && next_split) {
        split_mode_t split_mode_order[NUM_SPLIT_MODE];
        double best_split_cost = MAX_D_COST;

        com_split_get_split_rdo_order(cu_width, cu_height, split_mode_order);

        for (int split_mode_num = 1; split_mode_num < NUM_SPLIT_MODE; ++split_mode_num) {
            split_mode_t split_mode = split_mode_order[split_mode_num];
            int is_mode_EQT = com_split_is_EQT(split_mode);
            int enable_cur_split = is_mode_EQT ? (best_split_mode != NO_SPLIT || cost_best > MAX_D_COST_EXT) : 1;

            if (info->depth_limit_eqt_by_cudepth) {
                if (split_mode == SPLIT_EQT_HOR && enable_cur_split) {
                    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
                    int cu_height_in_scu = cu_height >> MIN_CU_LOG2;
                    int depth1 = cu_data_bst->cudepth[0];
                    int depth2 = cu_data_bst->cudepth[cu_height_in_scu / 4 * cu_width_in_scu];
                    int depth3 = cu_data_bst->cudepth[cu_height_in_scu / 4 * cu_width_in_scu + cu_width_in_scu / 2];
                    int depth4 = cu_data_bst->cudepth[(cu_height_in_scu - 1) * cu_width_in_scu];
                    double avg_depth = ((double)(depth1 + depth2 + depth3 + depth4)) / 4.0;
                    if (avg_depth < 2 && cu_width_log2 + cu_height_log2 > 9)
                        enable_cur_split = 0;
                }

                if (split_mode == SPLIT_EQT_VER && enable_cur_split) {
                    int cu_width_in_scu = cu_width >> MIN_CU_LOG2;
                    int cu_height_in_scu = cu_height >> MIN_CU_LOG2;
                    int depth1 = cu_data_bst->cudepth[0];
                    int depth2 = cu_data_bst->cudepth[cu_width_in_scu / 4];
                    int depth3 = cu_data_bst->cudepth[3 * cu_width_in_scu / 4];
                    int depth4 = cu_data_bst->cudepth[(cu_height_in_scu / 2) * cu_width_in_scu + cu_width_in_scu / 4];
                    double avg_depth = ((double)(depth1 + depth2 + depth3 + depth4)) / 4.0;
                    if (avg_depth < 2 && cu_width_log2 + cu_height_log2 > 9)
                        enable_cur_split = 0;
                }
            }
            
            if (info->depth_rm_splite_by_border) {
                if (split_mode == SPLIT_EQT_HOR && enable_cur_split && split_allow[SPLIT_EQT_HOR] && cu_width_log2 + cu_height_log2 >= 11) {
                    if (score_from_bth[SPLIT_EQT_HOR] + score_from_btv[SPLIT_EQT_HOR] + score_from_qt[SPLIT_EQT_HOR] < 0) {
                        enable_cur_split = 0;
                    }
                }
                if (split_mode == SPLIT_EQT_VER && enable_cur_split && split_allow[SPLIT_EQT_VER] && cu_width_log2 + cu_height_log2 >= 11) {
                    if (score_from_bth[SPLIT_EQT_VER] + score_from_btv[SPLIT_EQT_VER] + score_from_qt[SPLIT_EQT_VER] < 0) {
                        enable_cur_split = 0;
                    }
                }
                if (split_mode_num == 3 && split_mode == SPLIT_BI_HOR && score_from_btv[SPLIT_BI_HOR] < 0 && cu_width_log2 + cu_height_log2 >= 11) {
                    enable_cur_split = 0;
                }
                if (split_mode_num == 3 && split_mode == SPLIT_BI_VER && score_from_bth[SPLIT_BI_VER] < 0 && cu_width_log2 + cu_height_log2 >= 11) {
                    enable_cur_split = 0;
                }
            }

            if (split_allow[split_mode] && enable_cur_split) {
                double best_cons_cost = MAX_D_COST;
                com_split_struct_t split_struct;
                u8 tree_status_child = TREE_LC;
                u8 num_cons_pred_mode_loop;
                u8 cons_pred_mode_child = NO_MODE_CONS;
                u8 curr_best_cons = NO_MODE_CONS;

                num_split_tried++;
                com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, info->log2_lcuwh_in_scu, &split_struct);

                tree_status_child = (tree_status == TREE_LC && com_tree_split(cu_width, cu_height, split_mode, core->slice_type)) ? TREE_L : tree_status;
                num_cons_pred_mode_loop = (cons_pred_mode == NO_MODE_CONS && is_use_cons(cu_width, cu_height, split_mode, core->slice_type)) ? 2 : 1;

                for (int loop_idx = 0; loop_idx < num_cons_pred_mode_loop; loop_idx++) {
                    if (info->sqh.num_of_hmvp && slice_type != SLICE_I) {
                        copy_motion_table(core->motion_cands, &core->cnt_hmvp_cands, motion_cands_last, cnt_hmvp_cands_last);
                    }
                    if (cons_pred_mode == NO_MODE_CONS) {
                        if (num_cons_pred_mode_loop == 1) {
                            cons_pred_mode_child = NO_MODE_CONS;
                        } else {
                            cons_pred_mode_child = loop_idx == 0 ? ONLY_INTER : ONLY_INTRA;
                        }
                    } else {
                        cons_pred_mode_child = cons_pred_mode;
                    }
                    if (num_cons_pred_mode_loop == 2 && history->cons_mode_history[split_mode - 1] && history->cons_mode_history[split_mode - 1] != cons_pred_mode_child) {
                        continue;
                    }

                    clear_map_scu(core, x0, y0, cu_width, cu_height);
                    int part_num = 0;
                    double cost_temp = 0.0;
                    lbac_t lbac_split;
                    lbac_copy(&lbac_split, &lbac_cur_node);

                    if (x0 + cu_width <= info->pic_width && y0 + cu_height <= info->pic_height) {
                        int bit_cnt = lbac_get_bits(&lbac_split);
                        com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_tmp->split_mode);
                        lbac_enc_split_mode(&lbac_split, NULL, core, split_mode, cud, 0, cu_width, cu_height, cu_width, parent_split, qt_depth, bet_depth, x0, y0);

                        if (cons_pred_mode == NO_MODE_CONS && is_use_cons(cu_width, cu_height, split_mode, core->slice_type)) {
                            lbac_enc_cons_pred_mode(&lbac_split, NULL, cons_pred_mode_child);
                        } else {
                            assert(cons_pred_mode_child == cons_pred_mode);
                        }

                        bit_cnt = lbac_get_bits(&lbac_split) - bit_cnt;
                        cost_temp += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);

                        //RDcostNS * a + lambda * (SplitBits + b) > RDcostNS
                        double a = 0.9, b = 1.0;

                        if (info->depth_terminate_P1) {
                            a = 0.8, b = 10.0;
                            if (info->depth_terminate_P2) {
                                a = 0.94, b = 4.0;
                            }
                        }
                        if (nscost * a + cost_temp + RATE_TO_COST_LAMBDA(core->lambda[0], b) > nscost) {
                            continue;
                        }
                    }

                    lbac_t lbac_tree_c;

                    if (tree_status_child == TREE_L && tree_status == TREE_LC) {
                        lbac_copy(&lbac_tree_c, &lbac_split);
                    }

                    for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
                        int cur_part_num = part_num;
                        int log2_sub_cuw = split_struct.log_cuw[cur_part_num];
                        int log2_sub_cuh = split_struct.log_cuh[cur_part_num];
                        int x_pos        = split_struct.x_pos  [cur_part_num];
                        int y_pos        = split_struct.y_pos  [cur_part_num];
                        int cur_cuw      = split_struct.width  [cur_part_num];
                        int cur_cuh      = split_struct.height [cur_part_num];

                        if ((x_pos < info->pic_width) && (y_pos < info->pic_height)) {
                            cost_temp += mode_coding_tree(core, &lbac_split, x_pos, y_pos, split_struct.cup[cur_part_num], log2_sub_cuw, log2_sub_cuh, split_struct.cud
                                                          , split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child);
                            copy_cu_data(cu_data_tmp, &core->cu_data_best[log2_sub_cuw - 2][log2_sub_cuh - 2], x_pos - split_struct.x_pos[0], y_pos - split_struct.y_pos[0], log2_sub_cuw, log2_sub_cuh, cu_width_log2, cud, tree_status_child);
                        }
                    }
                    // split_score
                    if (split_mode == SPLIT_QUAD || split_mode == SPLIT_BI_HOR || split_mode == SPLIT_BI_VER) {
                        int* score = split_mode == SPLIT_QUAD ? score_from_qt : split_mode == SPLIT_BI_HOR ? score_from_bth : score_from_btv;
                        score_split(core, split_mode, x0, y0, cu_width, cu_height, score, split_allow[SPLIT_EQT_VER], split_allow[SPLIT_EQT_HOR], cu_data_tmp);
                    }
                    if (cost_temp < MAX_D_COST_EXT && tree_status_child == TREE_L && tree_status == TREE_LC) {
                        core->tree_status = TREE_C;
                        core->cons_pred_mode = NO_MODE_CONS;
                        lbac_copy(&core->lbac_bakup, &lbac_tree_c);
                        cost_temp += mode_coding_unit(core, NULL, x0, y0, cu_width_log2, cu_height_log2, cud, cu_data_tmp, 0);
                        core->tree_status = TREE_LC;
                    }
                    if (cost_temp < best_cons_cost) {
                        best_cons_cost = cost_temp;
                        curr_best_cons = cons_pred_mode_child;

                        if (best_cons_cost < best_split_cost) {
                            best_split_cost = best_cons_cost;

                            if (best_split_cost < cost_best - 0.0001) { /* backup the current best data */
                                copy_cu_data(cu_data_bst, cu_data_tmp, 0, 0, cu_width_log2, cu_height_log2, cu_width_log2, cud, tree_status);
                                cost_best = best_split_cost;
                                lbac_copy(lbac_cur, &lbac_split);
                                best_split_mode = split_mode;

                                best_cons_pred_mode = cons_pred_mode_child;

                                if (info->sqh.num_of_hmvp && slice_type != SLICE_I) {
                                    copy_motion_table(motion_cands_curr, &cnt_hmvp_cands_curr, core->motion_cands, core->cnt_hmvp_cands);
                                }
                            }
                        }
                    }
                }

                if (num_cons_pred_mode_loop == 2 && !history->cons_mode_history[split_mode - 1]) {
                    history->cons_mode_history[split_mode - 1] = curr_best_cons;
                }
                if (!history->visit_split) {
                    history->split_cost[split_mode] = best_cons_cost;
                }
            }

            if (!history->visit_split && num_split_tried > 0) {
                if ((best_curr_cost *(1.10)) < best_split_cost) {
                    break;
                }
            }
        }
    }

    if (cost_best > MAX_D_COST_EXT) {
        return MAX_D_COST;
    }

    cpy_rec_2_pic(core, x0, y0, cu_width, cu_height, core->pic_rec, tree_status);

    /* restore best data */
    com_set_split_mode(best_split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_bst->split_mode);

    if (cons_pred_mode == NO_MODE_CONS && is_use_cons(cu_width, cu_height, best_split_mode, core->slice_type)) {
        com_set_cons_pred_mode(best_cons_pred_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_bst->split_mode);
    }

    if (info->sqh.num_of_hmvp && slice_type != SLICE_I) {
        copy_motion_table(core->motion_cands, &core->cnt_hmvp_cands, motion_cands_curr, cnt_hmvp_cands_curr);
    }

    if (num_split_to_try > 0) {
        if (best_split_mode == NO_SPLIT) {
            history->split = 0;
        } else {
            history->split = 1;
        }
        history->visit_split = 1;
    }
    update_map_scu(core, x0, y0, 1 << cu_width_log2, 1 << cu_height_log2);
    return (cost_best > MAX_D_COST_EXT) ? MAX_D_COST : cost_best;
}

int enc_mode_analyze_lcu(core_t *core, const lbac_t *lbac)
{
    com_scu_t *map_scu;
    int i, j, w, h;
    lbac_t lbac_root;
    com_info_t *info = core->info;
    com_map_t *map = core->map;
    int lcu_pos = core->lcu_y * info->pic_width_in_lcu + core->lcu_x;
    enc_cu_t *cu_data = &core->cu_data_best[info->log2_max_cuwh - 2][info->log2_max_cuwh - 2];

    lbac_copy(&lbac_root, lbac);

    /* decide mode */
    mode_coding_tree(core, &lbac_root, core->lcu_pix_x, core->lcu_pix_y, 0, info->log2_max_cuwh, info->log2_max_cuwh, 0 , NO_SPLIT, 0, 0, NO_MODE_CONS, TREE_LC);

    memcpy(core->map->map_split + lcu_pos * info->cus_in_lcu, cu_data->split_mode, sizeof(s8) * MAX_CU_DEPTH * NUM_BLOCK_SHAPE * info->cus_in_lcu);

    copy_cu_data(&core->map_cu_data[lcu_pos], &core->cu_data_best[info->log2_max_cuwh - 2][info->log2_max_cuwh - 2], 0, 0, info->log2_max_cuwh, info->log2_max_cuwh, info->log2_max_cuwh, 0, TREE_LC);

    com_pic_t *pic = core->pic_rec;
    int pix_x = core->lcu_pix_x;
    int pix_y = core->lcu_pix_y;
    int lcuw = COM_MIN(info->max_cuwh, info->pic_width - pix_x);
    int lcuh = COM_MIN(info->max_cuwh, info->pic_height - pix_y);
    int s_l = pic->stride_luma;
    int s_c = pic->stride_chroma;
    int add = (pix_x + lcuw == info->pic_width) ? 1 : 0;
    int i_scu = info->i_scu;
    int scu_x = PEL2SCU(pix_x);
    int scu_y = PEL2SCU(pix_y);

    memcpy(core->linebuf_intra[0][0] + core->lcu_pix_x     - 1, pic->y + (pix_y     + lcuh     - 1) * s_l + pix_x     - 1, (lcuw     + add) * sizeof(pel));
    memcpy(core->linebuf_intra[0][1] + core->lcu_pix_x / 2 - 1, pic->u + (pix_y / 2 + lcuh / 2 - 1) * s_c + pix_x / 2 - 1, (lcuw / 2 + add) * sizeof(pel));
    memcpy(core->linebuf_intra[0][2] + core->lcu_pix_x / 2 - 1, pic->v + (pix_y / 2 + lcuh / 2 - 1) * s_c + pix_x / 2 - 1, (lcuw / 2 + add) * sizeof(pel));

    if (!core->pichdr->loop_filter_disable_flag) {
        int scup = scu_y * i_scu + scu_x;
        com_df_set_edge(info, map, map->map_edge + scup, i_scu, core->refp, pix_x, pix_y, info->max_cuwh, info->max_cuwh, 0, 0, 1);
        com_df_lcu(info, core->pichdr, map, pic, core->lcu_x, core->lcu_y);
    }
    if (info->sqh.sao_enable) {
        enc_sao_rdo(core, lbac);
    }

    map_scu = map->map_scu + (scu_y * info->i_scu) + scu_x;
    w = COM_MIN(1 << (info->log2_max_cuwh - MIN_CU_LOG2), info->pic_width_in_scu  - scu_x);
    h = COM_MIN(1 << (info->log2_max_cuwh - MIN_CU_LOG2), info->pic_height_in_scu - scu_y);

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            map_scu[j].coded = 0;
        }
        map_scu += info->i_scu;
    }
    return COM_OK;
}

int est_pred_info_bits(core_t *core)
{
    com_mode_t *mod_cur = &core->mod_info_curr;
    int bits_residual = 2 + 4 + (mod_cur->num_nz[TB0][Y_C] == 0 ? 5 : 10);//(cbf u + cbf v) + (4 cbf_y) + (run + sign + level + last)
    int bits_pred = 0;
    if (mod_cur->cu_mode == MODE_DIR) {
        bits_pred = 6;
    }
    else if (mod_cur->cu_mode == MODE_INTER) {
        bits_pred = 10;
    }
    else {
        bits_pred = 2;
    }
    return bits_pred + bits_residual;
}

int enc_tq_itdq_yuv_nnz(core_t *core, lbac_t *lbac, com_mode_t *cur_mode, s16 coef[N_C][MAX_CU_DIM], s16 resi[N_C][MAX_CU_DIM], pel pred[N_C][MAX_CU_DIM], pel rec[N_C][MAX_CU_DIM], s8 refi[REFP_NUM], s16 mv[REFP_NUM][MV_D])
{
    int bit_depth      = core->info->bit_depth_internal;
    int cu_width_log2  = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int slice_type     = core->slice_type;

    cu_nz_cln(cur_mode->num_nz);

    for (int i = 0; i < N_C; i++) {
        if ((core->tree_status == TREE_L && i != Y_C) || (core->tree_status == TREE_C && i == Y_C)) {
            cur_mode->num_nz[TB0][i] = 0;
            continue;
        }
        if (core->best_tb_part_hist == 255 || core->best_tb_part_hist == SIZE_2Nx2N || i != Y_C) {
            int plane_width_log2  = cu_width_log2  - (i != Y_C);
            int plane_height_log2 = cu_height_log2 - (i != Y_C);
            int qp = (i == Y_C ? core->lcu_qp_y : (i == U_C ? core->lcu_qp_u : core->lcu_qp_v));

            cur_mode->num_nz[TB0][i] = enc_tq_nnz(core, cur_mode, i, 0, qp, core->lambda[i], coef[i], resi[i], plane_width_log2, plane_height_log2, slice_type, 0, 0, 0);

            if (cur_mode->num_nz[TB0][i]) {
                ALIGNED_32(s16 resi_it[MAX_CU_DIM]);
                com_invqt(cur_mode, i, 0, coef[i], resi_it, core->wq, plane_width_log2, plane_height_log2, qp, bit_depth, 0, 0);
                uavs3e_funs_handle.recon[plane_width_log2 - MIN_CU_LOG2](resi_it, pred[i], 1 << plane_width_log2, 1 << plane_width_log2, 1 << plane_height_log2, rec[i], 1 << plane_width_log2, cur_mode->num_nz[TB0][i], bit_depth);
            }
        } else {
            cur_mode->num_nz[TB0][i] = 0; //no need to try 2Nx2N transform
        }
    }

    int try_sub_block_transform = core->tree_status != TREE_C && is_tb_avaliable(core->info, cu_width_log2, cu_height_log2, cur_mode->pb_part, MODE_INTER);

    if (try_sub_block_transform) { //fast algorithm
        if (core->best_tb_part_hist == SIZE_2Nx2N) {
            try_sub_block_transform = 0;
        }
        if (try_sub_block_transform && core->best_tb_part_hist == 255) {
            com_mode_t *mod_curr = &core->mod_info_curr;
            int bits_est = est_pred_info_bits(core);
            double bits_cost = RATE_TO_COST_LAMBDA(core->lambda[Y_C], bits_est);
            s64    dist_cost = core->dist_pred_luma >> (cur_mode->num_nz[TB0][Y_C] == 0 ? 2 : 3);
            if (bits_cost + dist_cost > core->cost_best) {
                try_sub_block_transform = 0;
            }
        }
    }
    if (try_sub_block_transform) {
        ALIGNED_32(s16 coef_NxN[MAX_CU_DIM]);
        int bak_2Nx2N_num_nz = cur_mode->num_nz[TB0][Y_C];
        int cu_size   = 1 << (cu_width_log2 + cu_height_log2);
        int cu_width  = 1 << cu_width_log2;
        int cu_height = 1 << cu_height_log2;
        int x = core->cu_pix_x;
        int y = core->cu_pix_y;
        com_pic_t *pic_org = core->pic_org;
        pel *org = pic_org->y + (y * pic_org->stride_luma) + x;
        double cost_2Nx2N, cost_NxN;
        int part_num;
        int log2_tb_w, log2_tb_h, tb_size;
        int cu_w = 1 << cu_width_log2;
        int cu_h = 1 << cu_height_log2;
        part_size_t part_size = get_tb_part_size_by_pb(cur_mode->pb_part, MODE_INTER);

        // check sub-TB
        cur_mode->tb_part = part_size;
        part_num = get_part_num(part_size);
        get_tb_width_height_log2(cu_width_log2, cu_height_log2, part_size, &log2_tb_w, &log2_tb_h);
        tb_size = 1 << (log2_tb_w + log2_tb_h);

        for (int i = 0; i < part_num; i++) {
            ALIGNED_32(s16 resi_buf[MAX_CU_DIM]);
            s16 *tb_resi = resi[Y_C];

            if (part_num > 1) {
                int pos_x, pos_y;
                int tu_w = 1 << log2_tb_w;
                int tu_h = 1 << log2_tb_h;

                get_tb_start_pos(cu_w, cu_h, part_size, i, &pos_x, &pos_y);

                s16 *s = resi[Y_C] + pos_y * cu_w + pos_x;
                s16 *d = resi_buf;

                for (int k = 0; k < tu_h; k++) {
                    memcpy(d, s, sizeof(s16) * tu_w);
                    d += tu_w;
                    s += cu_w;
                }
                tb_resi = resi_buf;
            }
            cur_mode->num_nz[i][Y_C] = enc_tq_nnz(core, cur_mode, Y_C, i, core->lcu_qp_y, core->lambda[Y_C], coef_NxN + i * tb_size, tb_resi, log2_tb_w, log2_tb_h, slice_type, 0, 0, 0);
        }

        if (bak_2Nx2N_num_nz && is_cu_plane_nz(cur_mode->num_nz, Y_C)) {
            ALIGNED_32(pel rec_NxN[MAX_CU_DIM]);
            ALIGNED_32(s16 resi_it[MAX_CU_DIM]);
            int bak_NxN_num_nz[MAX_NUM_TB];

            /*************************************************************************************************************/
            /* cal NxN */
            for (int j = 0; j < MAX_NUM_TB; j++) {
                bak_NxN_num_nz[j] = cur_mode->num_nz[j][Y_C];
            }
            com_invqt_inter_plane(cur_mode, Y_C, coef_NxN, resi_it, core->wq, cu_width_log2, cu_height_log2, core->lcu_qp_y, bit_depth);
            com_recon_plane(cur_mode->tb_part, resi_it, pred[Y_C], cur_mode->num_nz, Y_C, cu_width, cu_height, cu_width, rec_NxN, bit_depth);
            cost_NxN = (double)(block_pel_ssd(cu_width_log2, cu_height, rec_NxN, org, cu_width, pic_org->stride_luma, bit_depth));
            cost_NxN += get_bits_cost_comp(core, lbac, &core->lbac_bakup, coef_NxN, core->lambda[0], Y_C);

            /*************************************************************************************************************/
            /* cal 2Nx2N */
            cu_plane_nz_cln(cur_mode->num_nz, Y_C);
            cur_mode->num_nz[TB0][Y_C] = bak_2Nx2N_num_nz;
            cur_mode->tb_part = SIZE_2Nx2N;

            cost_2Nx2N = (double)(block_pel_ssd(cu_width_log2, cu_height, rec[Y_C], org, cu_width, pic_org->stride_luma, bit_depth));
            cost_2Nx2N += get_bits_cost_comp(core, lbac, &core->lbac_bakup, coef[0], core->lambda[0], Y_C);

            if (cost_NxN < cost_2Nx2N) {
                memcpy(coef[Y_C], coef_NxN, sizeof(s16) *cu_size);
                memcpy(rec [Y_C], rec_NxN,  sizeof(pel) *cu_size);

                for (int j = 0; j < MAX_NUM_TB; j++) {
                    cur_mode->num_nz[j][Y_C] = bak_NxN_num_nz[j];
                }
                cur_mode->tb_part = get_tb_part_size_by_pb(cur_mode->pb_part, MODE_INTER);
            }
        }
        else if (bak_2Nx2N_num_nz) {
            cu_plane_nz_cln(cur_mode->num_nz, Y_C);
            cur_mode->num_nz[TB0][Y_C] = bak_2Nx2N_num_nz;
            cur_mode->tb_part = SIZE_2Nx2N;
        }
        else if (is_cu_plane_nz(cur_mode->num_nz, Y_C)) {
            ALIGNED_32(s16 resi_it[MAX_CU_DIM]);
            memcpy(coef[Y_C], coef_NxN, sizeof(s16) *cu_size);
            com_invqt_inter_plane(cur_mode, Y_C, coef_NxN, resi_it, core->wq, cu_width_log2, cu_height_log2, core->lcu_qp_y, bit_depth);
            com_recon_plane(cur_mode->tb_part, resi_it, pred[Y_C], cur_mode->num_nz, Y_C, cu_width, cu_height, cu_width, rec[Y_C], bit_depth);
        }
    }
    check_set_tb_part(cur_mode);
    return is_cu_nz(cur_mode->num_nz);
}
