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

#include "define.h"
#include "analyze.h"
#include "com_tables.h"
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
    com_info_t *info = core->info;
    com_mode_t *bst_info = &core->mod_info_best;
    com_mode_t *cur_info = &core->mod_info_curr;

    core->cu_pix_x = x;
    core->cu_pix_y = y;
    core->cu_width = 1 << cu_width_log2;
    core->cu_height = 1 << cu_height_log2;
    core->cu_width_log2 = cu_width_log2;
    core->cu_height_log2 = cu_height_log2;
    core->cu_scu_x = PEL2SCU(x);
    core->cu_scu_y = PEL2SCU(y);
    core->cu_scup_in_pic = ((u32)core->cu_scu_y * info->i_scu) + core->cu_scu_x;
    core->skip_mvps_check = 1;

    cu_nz_cln(cur_info->num_nz);
    cu_nz_cln(bst_info->num_nz);

    if (core->tree_status == TREE_C) {
        return COM_OK;
    }

    bst_info->cu_mode = MODE_INTRA;

    //init the best cu info
    init_pb_part(bst_info);
    init_tb_part(bst_info);
    get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, bst_info->pb_part, &bst_info->pb_info);
    get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, bst_info->tb_part, &bst_info->tb_info);
    assert(bst_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);
   
    com_mset(bst_info->mv,   0, sizeof(s16) * REFP_NUM * MV_D);
    com_mset(bst_info->mvd,  0, sizeof(s16) * REFP_NUM * MV_D);
    com_mset(bst_info->refi, 0, sizeof(s16) * REFP_NUM);

    bst_info->mvr_idx = 0;
    bst_info->skip_idx = 0;
    bst_info->umve_flag = 0;
    bst_info->umve_idx = -1;
    bst_info->hmvp_flag = 0;
    bst_info->ipf_flag = 0;
    bst_info->affine_flag = 0;
    bst_info->smvd_flag = 0;

    com_mset(bst_info->affine_mv,  0, sizeof(CPMV) * REFP_NUM * VER_NUM * MV_D);
    com_mset(bst_info->affine_mvd, 0, sizeof( s16) * REFP_NUM * VER_NUM * MV_D);

    cur_info->affine_flag = 0;
    cur_info->smvd_flag = 0;

    rdoq_init_cu_est_bits(core, &core->sbac_bakup);

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
    int i, j, idx, size;
    int cu_width_log2  = CONV_LOG2(core->cu_width);
    int cu_height_log2 = CONV_LOG2(core->cu_height);

    if (core->tree_status != TREE_C) {
        com_mcpy(cu_data->coef[Y_C], mi->coef[Y_C], core->cu_width * core->cu_height * sizeof(s16));
        com_mcpy(cu_data->reco[Y_C], mi->rec [Y_C], core->cu_width * core->cu_height * sizeof(pel));
    }
    if (core->tree_status != TREE_L) {
        size = (core->cu_width * core->cu_height * sizeof(s16)) >> 2;
        com_mcpy(cu_data->coef[U_C], mi->coef[U_C], size);
        com_mcpy(cu_data->coef[V_C], mi->coef[V_C], size);
        size = (core->cu_width * core->cu_height * sizeof(pel)) >> 2;
        com_mcpy(cu_data->reco[U_C], mi->rec[U_C], size);
        com_mcpy(cu_data->reco[V_C], mi->rec[V_C], size);
    }

    /* mode info */
    if (core->tree_status == TREE_C) {
        idx = 0;
        for (j = 0; j < core->cu_height >> MIN_CU_LOG2; j++) {
            for (i = 0; i < core->cu_width >> MIN_CU_LOG2; i++) {
                if (mi->cu_mode == MODE_INTRA) {
                    cu_data->ipm_c[idx + i] = mi->ipm[PB0][1];
                }
                cu_data->num_nz_coef[1][idx + i] = mi->num_nz[TBUV0][1];
                cu_data->num_nz_coef[2][idx + i] = mi->num_nz[TBUV0][2];
            }
            idx += core->cu_width >> MIN_CU_LOG2;
        }
    } else {
        idx = 0;
        int cu_cbf_flag;
        if (core->tree_status == TREE_LC) {
            cu_cbf_flag = is_cu_nz(mi->num_nz);
        } else if (core->tree_status == TREE_L) {
            cu_cbf_flag = is_cu_plane_nz(mi->num_nz, Y_C);
        }
        com_scu_t scu;
        scu.coded = 1;
        scu.intra = mi->cu_mode == MODE_INTRA;
        scu.cbf = cu_cbf_flag;
        scu.skip = mi->cu_mode == MODE_SKIP;
        scu.affine = mi->affine_flag;
        scu.tbpart = mi->tb_part;

        u32 pos;
        MCU_SET_SCUP(pos, core->cu_scup_in_pic);
        MCU_SET_LOGW(pos, cu_width_log2);
        MCU_SET_LOGH(pos, cu_height_log2);

        for (j = 0; j < core->cu_height >> MIN_CU_LOG2; j++) {
            for (i = 0; i < core->cu_width >> MIN_CU_LOG2; i++) {
                int tb_idx_y = get_part_idx(mi->tb_part, i << 2, j << 2, core->cu_width, core->cu_height);

                cu_data->map_scu     [idx + i] = scu;
                cu_data->map_pos     [idx + i] = pos;
                cu_data->pred_mode   [idx + i] = (u8)mi->cu_mode;
                cu_data->umve_flag   [idx + i] = mi->umve_flag;
                cu_data->umve_idx    [idx + i] = mi->umve_idx;
                cu_data->pb_part     [idx + i] = mi->pb_part;
                cu_data->tb_part     [idx + i] = mi->tb_part;
                cu_data->affine_flag [idx + i] = mi->affine_flag;

                cu_data->num_nz_coef[0][idx + i] = mi->num_nz[tb_idx_y][0];
                cu_data->num_nz_coef[1][idx + i] = mi->num_nz[0][1];
                cu_data->num_nz_coef[2][idx + i] = mi->num_nz[0][2];

                if (mi->cu_mode == MODE_INTRA) {
                    int pb_idx_y = get_part_idx(mi->pb_part, i << 2, j << 2, core->cu_width, core->cu_height);
                    M16 (cu_data->refi[idx + i]) = -1;
                    CP16(cu_data->mpm [idx + i], mi->mpm[pb_idx_y]);
                    cu_data->ipm_l[idx + i] = mi->ipm[pb_idx_y][0];
                    cu_data->ipm_c[idx + i] = mi->ipm[0][1];
                    cu_data->ipf_flag[idx + i] = mi->ipf_flag;
                } else {
                    CP16(cu_data->refi[idx + i], mi->refi);
                    CP64(cu_data->mv  [idx + i], mi->mv);
                    CP64(cu_data->mvd [idx + i], mi->mvd);
                    cu_data->mvr_idx  [idx + i] = mi->mvr_idx;
                    cu_data->hmvp_flag[idx + i] = mi->hmvp_flag;
                    cu_data->smvd_flag[idx + i] = mi->smvd_flag;
                    cu_data->skip_idx [idx + i] = mi->skip_idx;
                }
            }
            idx += core->cu_width >> MIN_CU_LOG2;
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

    com_scu_t *dst_map_scu               = map->map_scu  + map_offset;
    s8      *dst_map_ipm                 = map->map_ipm  + map_offset;
    s16    (*dst_map_mv)[REFP_NUM][MV_D] = map->map_mv   + map_offset;
    s8     (*dst_map_refi)[REFP_NUM]     = map->map_refi + map_offset;
    u32     *dst_map_pos                 = map->map_pos  + map_offset;

    int w = COM_MIN(src_cuw, info->pic_width  - x) >> MIN_CU_LOG2;
    int h = COM_MIN(src_cuh, info->pic_height - y) >> MIN_CU_LOG2;
  
    int size_scu  = sizeof(com_scu_t) * w;
    int size_pos  = sizeof(      u32) * w;
    int size_ipm  = sizeof(       u8) * w;
    int size_mv   = sizeof(      s16) * w * REFP_NUM * MV_D;
    int size_refi = sizeof(       s8) * w * REFP_NUM;

    assert(core->tree_status != TREE_C);

    for (int i = 0; i < h; i++) {
#define COPY_ONE_DATA(d,s,size) com_mcpy(d, s, size); d += i_dst; s += i_src;
        COPY_ONE_DATA(dst_map_scu,  src_map_scu,     size_scu);
        COPY_ONE_DATA(dst_map_pos,  src_map_cu_mode, size_pos);
        COPY_ONE_DATA(dst_map_ipm,  src_map_ipm,     size_ipm);
        COPY_ONE_DATA(dst_map_mv,   src_map_mv,      size_mv);
        COPY_ONE_DATA(dst_map_refi, src_map_refi,    size_refi);
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

static double mode_coding_unit(core_t *core, lbac_t *sbac_best, int x, int y, int cu_width_log2, int cu_height_log2, int cud, enc_cu_t *cu_data)
{
    com_info_t *info = core->info;
    com_mode_t *bst_info = &core->mod_info_best;
    com_mode_t *cur_info = &core->mod_info_curr;
    int bit_depth = info->bit_depth_internal;
    u8 cons_pred_mode = NO_MODE_CONS;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    s8 ipf_flag;
    s8 ipf_passes_num = (info->sqh.ipf_enable_flag && (cu_width < MAX_CU_SIZE) && (cu_height < MAX_CU_SIZE)) ? 2 : 1;
    com_pic_t *pic_org = core->pic_org;

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
    double cost_best = MAX_COST;
    core->cost_best = MAX_COST;

    if (core->slice_type != SLICE_I && cons_pred_mode != ONLY_INTRA) {
        analyze_inter_cu(core, sbac_best);

        if (core->cost_best < cost_best) {
            cost_best = core->cost_best;
            copy_to_cu_data(core, bst_info, cu_data);
        }
    }

    //**************** intra ********************
    if ((core->slice_type == SLICE_I || is_cu_nz(bst_info->num_nz) || cost_best == MAX_COST) && cons_pred_mode != ONLY_INTER) {
        if (cu_width <= 64 && cu_height <= 64) {
            core->dist_cu_best = COM_INT32_MAX;
            if (core->cost_best != MAX_COST) {
                core->inter_satd = block_pel_satd(cu_width_log2, cu_height_log2, pic_org->y + (y * pic_org->stride_luma) + x, bst_info->pred[0], pic_org->stride_luma, 1 << cu_width_log2, bit_depth);
            } else {
                core->inter_satd = COM_UINT32_MAX;
            }

            for (ipf_flag = 0; ipf_flag < ipf_passes_num; ++ipf_flag) {
                cur_info->ipf_flag = ipf_flag;
                analyze_intra_cu(core, sbac_best);

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
            }
        }
    }
    return cost_best;
}

static void check_run_split(core_t *core, int cu_width_log2, int cu_height_log2, int cup, int *split_allow)
{
    int i;
    double min_cost = MAX_COST;
    int run_list[NUM_SPLIT_MODE];
    enc_history_t *p_bef_data = &core->bef_data[cu_width_log2 - 2][cu_height_log2 - 2][cup];

    if (p_bef_data->split_visit) {
        if (p_bef_data->nosplit < 1 && p_bef_data->split >= 1) {
            run_list[0] = 0;
            for (i = 1; i < NUM_SPLIT_MODE; i++) {
                if (p_bef_data->split_cost[i] < min_cost && split_allow[i]) {
                    min_cost = p_bef_data->split_cost[i];
                }
            }
            if (min_cost == MAX_COST) {
                run_list[0] = 1;
                for (i = 1; i < NUM_SPLIT_MODE; i++) {
                    run_list[i] = 0;
                }
            } else {
                for (i = 1; i < NUM_SPLIT_MODE; i++) {
#if RDO_WITH_DBLOCK 
                    double th = (min_cost < 0) ? 0.99 : 1.01;
                    if (p_bef_data->split_cost[i] <= th * min_cost)
#else
                    if (p_bef_data->split_cost[i] <= (1.01) * min_cost)
#endif
                    {
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
            p_bef_data->split_cost[i] = MAX_COST;
        }
    }
}

#if RDO_WITH_DBLOCK
s64 calc_dist_filter_boundary(core_t *core, com_pic_t *pic_rec, com_pic_t *pic_org, int cu_width, int cu_height,
                                    pel *src, int s_src, int x, int y, u8 intra_flag, u8 cu_cbf, s8 *refi, s16(*mv)[MV_D], u8 is_mv_from_mvf, int only_delta)
{
    com_info_t *info = core->info;

    if (core->pichdr->loop_filter_disable_flag) {
        return 0;
    }
    com_map_t *map = core->map;
    com_mode_t *cur_info = &core->mod_info_curr;
    int bit_depth = info->bit_depth_internal;
    int i, j;
    int cu_width_log2 = CONV_LOG2(cu_width);
    assert(cu_width >= 4 && cu_height >= 4);
    const int x_offset = 8;
    const int y_offset = 8;
    const int x_tm = 4;
    const int y_tm = 4; 
    const int log2_x_tm = CONV_LOG2(x_tm);
    int s_l_org = pic_org->stride_luma;
    int s_l_rec = pic_rec->stride_luma;
    int s_l_dbk = cu_width + x_offset;

    pel df_buf[(MAX_CU_SIZE + 8) *(MAX_CU_SIZE + 8)];
    pel *dst_y = df_buf + y_offset * s_l_dbk + x_offset;
    pel *org_y = pic_org->y + y * s_l_org + x;
    int x_scu = x >> MIN_CU_LOG2;
    int y_scu = y >> MIN_CU_LOG2;
    int t = x_scu + y_scu * info->i_scu;
    s64 dist_nofilt = 0, dist_filter = 0;

    for (i = 0; i < cu_height; i++) { //copy curr block
        com_mcpy(dst_y + i * s_l_dbk, src + i * s_src, cu_width * sizeof(pel));
    }
    if (y != 0) { //copy top
        for (i = 0; i < y_offset; i++) {
            com_mcpy(dst_y + (-y_offset + i)*s_l_dbk, pic_rec->y + (y - y_offset + i)*s_l_rec + x, cu_width * sizeof(pel));
        }
    }
    if (x != 0) { //copy left
        for (i = 0; i < cu_height; i++) {
            com_mcpy(dst_y + i * s_l_dbk - x_offset, pic_rec->y + (y + i)*s_l_rec + (x - x_offset), x_offset * sizeof(pel));
        }
    }

    //******** no filter ****************************
    if (only_delta) {
        if (y != 0) {
            dist_nofilt += block_pel_ssd(cu_width_log2, cu_height + y_tm, dst_y - y_tm * s_l_dbk, org_y - y_tm * s_l_org, s_l_dbk, s_l_org, bit_depth);
        } else {
            dist_nofilt += block_pel_ssd(cu_width_log2, cu_height, dst_y, org_y, s_l_dbk, s_l_org, bit_depth);
        }
    } else if (y != 0) {
        dist_nofilt += block_pel_ssd(cu_width_log2, y_tm, dst_y - y_tm * s_l_dbk, org_y - y_tm * s_l_org, s_l_dbk, s_l_org, bit_depth);
    }
    if (x != 0) {
        dist_nofilt += block_pel_ssd(log2_x_tm, cu_height, dst_y - x_tm, org_y - x_tm, s_l_dbk, s_l_org, bit_depth);
    }

    //*********** filter ****************************

    int w_scu = cu_width >> MIN_CU_LOG2;
    int h_scu = cu_height >> MIN_CU_LOG2;
    com_scu_t scu = map->map_scu[t];
    scu.intra = intra_flag;
    scu.cbf = cu_cbf;
    scu.tbpart = cur_info->tb_part;
    scu.coded = 0; //clear coded (necessary)

    for (j = 0; j < h_scu; j++) {
        int idx = (y_scu + j) * info->i_scu + x_scu;
        if (refi != NULL && !is_mv_from_mvf) {
            for (i = 0; i < w_scu; i++, idx++) {
                map->map_scu[idx] = scu;
                CP16(map->map_refi[idx], refi);
                CP64(map->map_mv[idx], mv);
            }
        } else {
            for (i = 0; i < w_scu; i++, idx++) {
                map->map_scu[idx] = scu;
            }
        }
    }

    u8 df_rdo_edge[MAX_CU_CNT_IN_LCU];
    memset(df_rdo_edge, 0, sizeof(u8) * w_scu * h_scu);

    com_df_set_edge(info, map, df_rdo_edge, w_scu, core->refp, x, y, cu_width, cu_height, 0, 0, 0);
    com_df_rdo_luma(info, core->pichdr, map, df_rdo_edge, w_scu, dst_y, s_l_dbk, x, y, cu_width, cu_height);

    if (y != 0) {
        dist_filter += block_pel_ssd(cu_width_log2, cu_height + y_tm, dst_y - y_tm * s_l_dbk, org_y - y_tm * s_l_org, s_l_dbk, s_l_org, bit_depth);
    } else {
        dist_filter += block_pel_ssd(cu_width_log2, cu_height, dst_y, org_y, s_l_dbk, s_l_org, bit_depth);
    }
    if (x != 0) {
        dist_filter += block_pel_ssd(log2_x_tm, cu_height, dst_y - x_tm, org_y - x_tm, s_l_dbk, s_l_org, bit_depth);
    }

    return dist_filter - dist_nofilt;
}

#endif

static double mode_coding_tree(core_t *core, lbac_t *sbac_cur, int x0, int y0, int cup, int cu_width_log2, int cu_height_log2, int cud
                               , const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status, double max_cost)
{
    com_info_t *info = core->info;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    s8 best_split_mode = NO_SPLIT;
    u8 best_cons_pred_mode = NO_MODE_CONS;
    int bit_cnt;
    double cost_best = MAX_COST;
    int boundary = !(x0 + cu_width <= info->pic_width && y0 + cu_height <= info->pic_height);
    int boundary_r = 0, boundary_b = 0;
    int split_allow[SPLIT_QUAD + 1]; 
    double best_curr_cost = MAX_COST;
    int num_split_tried = 0;
    int num_split_to_try = 0;
    int next_split = 1; //early termination flag
    enc_cu_t *cu_data_tmp = &core->cu_data_temp[cu_width_log2 - 2][cu_height_log2 - 2];       // stack structure 
    enc_cu_t *cu_data_bst = &core->cu_data_best[cu_width_log2 - 2][cu_height_log2 - 2];       // stack structure 
    enc_history_t *p_bef_data = &core->bef_data[cu_width_log2 - 2][cu_height_log2 - 2][cup];  // stack structure 
    com_motion_t motion_cands_curr[ALLOWED_HMVP_NUM];
    s8 cnt_hmvp_cands_curr = 0;
    com_motion_t motion_cands_last[ALLOWED_HMVP_NUM];
    s8 cnt_hmvp_cands_last = 0;
    com_mode_t *bst_info = &core->mod_info_best;
    lbac_t sbac_cur_node;
    int slice_type = core->pichdr->slice_type;

    if (info->sqh.num_of_hmvp) {
        copy_motion_table(motion_cands_last, &cnt_hmvp_cands_last, core->motion_cands, core->cnt_hmvp_cands);
    }

    core->tree_status = tree_status;
    core->cons_pred_mode = cons_pred_mode;

    lbac_copy(&sbac_cur_node, sbac_cur);

    if (cu_width > MIN_CU_SIZE || cu_height > MIN_CU_SIZE) {

        // ******** 1. normatively split modes **********
        boundary_b = boundary && (y0 + cu_height > info->pic_height) && !(x0 + cu_width > info->pic_width);
        boundary_r = boundary && (x0 + cu_width > info->pic_width) && !(y0 + cu_height > info->pic_height);
        com_check_split_mode(&info->sqh, split_allow, cu_width_log2, cu_height_log2, boundary, boundary_b, boundary_r, info->log2_max_cuwh,
                             parent_split, qt_depth, bet_depth, slice_type);
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

        // ******** 2. fast algorithm to reduce modes **********
        check_run_split(core, cu_width_log2, cu_height_log2, cup, split_allow);
    } else {
        split_allow[0] = 1;
        for (int i = 1; i < NUM_SPLIT_MODE; i++) {
            split_allow[i] = 0;
        }
    }

    if (!boundary) {
        double cost_temp = 0.0;

        if (split_allow[NO_SPLIT]) {
            if (cu_width > MIN_CU_SIZE || cu_height > MIN_CU_SIZE) {
                bit_cnt = lbac_get_bits(sbac_cur);
                com_set_split_mode(NO_SPLIT, cud, 0, cu_width, cu_height, cu_width, cu_data_tmp->split_mode);
                lbac_enc_split_mode(sbac_cur, NULL, core, NO_SPLIT, cud, 0, cu_width, cu_height, cu_width, parent_split, qt_depth, bet_depth, x0, y0);
                bit_cnt = lbac_get_bits(sbac_cur) - bit_cnt;
                cost_temp += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
            }
            lbac_copy(&core->sbac_bakup, sbac_cur);
            core->cu_scup_in_lcu = cup;
            clear_map_scu(core, x0, y0, cu_width, cu_height);

            if (info->sqh.num_of_hmvp && core->pichdr->slice_type != SLICE_I) {
                copy_motion_table(motion_cands_curr, &cnt_hmvp_cands_curr, motion_cands_last, cnt_hmvp_cands_last);
            }
            core->tree_status = tree_status;
            core->cons_pred_mode = cons_pred_mode;
            cost_temp += mode_coding_unit(core, sbac_cur, x0, y0, cu_width_log2, cu_height_log2, cud, cu_data_tmp);

            copy_cu_data(cu_data_bst, cu_data_tmp, 0, 0, cu_width_log2, cu_height_log2, cu_width_log2, cud, tree_status);

            if (info->sqh.num_of_hmvp && bst_info->cu_mode != MODE_INTRA && !bst_info->affine_flag) {
                update_skip_candidates(motion_cands_curr, &cnt_hmvp_cands_curr, info->sqh.num_of_hmvp, cu_data_tmp->mv[0], cu_data_tmp->refi[0]);
            }
            cost_best = cost_temp;
            best_split_mode = NO_SPLIT;
            best_cons_pred_mode = cons_pred_mode;
            p_bef_data->visit = 1;
        } else {
            cost_temp = MAX_COST;
        }
        if (!p_bef_data->split_visit) {
            p_bef_data->split_cost[NO_SPLIT] = cost_temp;
            best_curr_cost = cost_temp;
        }
    }
#if ENC_ECU_ADAPTIVE
    if (cost_best != MAX_COST && cud >= (core->ptr % 2 ? ENC_ECU_DEPTH - 1 : ENC_ECU_DEPTH)
#else
    if (cost_best != MAX_COST && cud >= ENC_ECU_DEPTH
#endif
        && bst_info->cu_mode == MODE_SKIP) {
        next_split = 0;
    }

    if (cost_best != MAX_COST) {
        if (slice_type == SLICE_I) {
            if (bst_info->ipm[PB0][0] != IPD_IPCM && core->dist_cu_best < (1 << (cu_width_log2 + cu_height_log2 + 7))) {
                u8 bits_inc_by_split = ((cu_width_log2 + cu_height_log2 >= 6) ? 2 : 0) + 8; //two split flags + one more (intra dir + cbf + edi_flag + mtr info) + 1-bit penalty, approximately 8 bits
                if (core->dist_cu_best < core->lambda[0] * bits_inc_by_split) {
                    next_split = 0;
                }
            }
        } else if (next_split && 0) {
            double qstep = pow(2, core->lcu_qp_y / 8.0);
            double T = 0;

            if (cu_width * cu_height == 32) {
                T = 0.3746 * qstep * qstep + 41.059 * qstep - 312.26;
                T *= 2;
            } else if (cu_width * cu_height == 64) {
                if (cu_width == cu_height) {
                    T = 0.5508 * qstep * qstep + 19.486 * qstep + 65.381;
                } else {
                    T = 0.432 * qstep * qstep + 24.619 * qstep - 59.853;
                }
                T *= 1.8;
            } else if (cu_width * cu_height == 128) {
                if (cu_width == 4 || cu_height == 4) {
                    T = 0.138 * qstep * qstep + 52.016 * qstep - 260.29;
                } else {
                    T = 0.2759 * qstep * qstep + 63.053 * qstep - 255.7;
                }
                T *= 1.6;
            } else if (cu_width * cu_height == 256) {
                if (cu_width == cu_height) {
                    T = 0.2627 * qstep * qstep + 111.01 * qstep - 473.56;
                } else {
                    T = 0.0785 * qstep * qstep + 106.78 * qstep - 456.72;
                }
                T *= 1.4;
            } else if (cu_width * cu_height == 512) {
                if (cu_width == 8 || cu_height == 8) {
                    T = 0.4271 * qstep * qstep + 124.08 * qstep + 199.36;
                } else {
                    T = 0.2652 * qstep * qstep + 117.49 * qstep + 178.75;
                }
                T *= 1.2;
            } else if (cu_width * cu_height == 1024) {
                if (cu_width == cu_height) {
                    T = 0.6111 * qstep * qstep + 204.52 * qstep + 806;
                } else {
                    T = 0.5071 * qstep * qstep + 187.66 * qstep + 1104.4;
                }
                T *= 1.1;
            } else if (cu_width * cu_height == 2048) {
                T = 1.3384 * qstep * qstep + 397.56 * qstep + 8830.1;
            }
            if (cost_best < T * 2) {
                next_split = 0;
            }
        }
    }
    if ((cu_width > MIN_CU_SIZE || cu_height > MIN_CU_SIZE) && next_split) {
        split_mode_t split_mode_order[NUM_SPLIT_MODE];
        double best_split_cost = MAX_COST;

        max_cost = COM_MIN(cost_best, max_cost);

        com_split_get_split_rdo_order(cu_width, cu_height, split_mode_order);
        for (int split_mode_num = 1; split_mode_num < NUM_SPLIT_MODE; ++split_mode_num) {
            split_mode_t split_mode = split_mode_order[split_mode_num];
            int is_mode_EQT = com_split_is_EQT(split_mode);
            int EQT_not_skiped = is_mode_EQT ? (best_split_mode != NO_SPLIT || cost_best == MAX_COST) : 1;

            if (split_allow[split_mode] && EQT_not_skiped) {
                double best_cons_cost = MAX_COST;
                com_split_struct_t split_struct;
                int prev_log2_sub_cuw = split_struct.log_cuw[0];
                int prev_log2_sub_cuh = split_struct.log_cuh[0];
                u8 tree_status_child = TREE_LC;
                u8 num_cons_pred_mode_loop;
                u8 cons_pred_mode_child = NO_MODE_CONS;

                if (split_mode == SPLIT_EQT_VER && cu_width == 16 && cu_height== 16) {
                    int a = 0;
                }
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

                    clear_map_scu(core, x0, y0, cu_width, cu_height);
                    int part_num = 0;
                    double cost_temp = 0.0;
                    lbac_t sbac_split;
                    lbac_copy(&sbac_split, &sbac_cur_node);

                    if (x0 + cu_width <= info->pic_width && y0 + cu_height <= info->pic_height) {
                        bit_cnt = lbac_get_bits(&sbac_split);
                        com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_tmp->split_mode);
                        lbac_enc_split_mode(&sbac_split, NULL, core, split_mode, cud, 0, cu_width, cu_height, cu_width, parent_split, qt_depth, bet_depth, x0, y0);

                        if (cons_pred_mode == NO_MODE_CONS && is_use_cons(cu_width, cu_height, split_mode, core->slice_type)) {
                            lbac_enc_cons_pred_mode(&sbac_split, NULL, cons_pred_mode_child);
                        } else {
                            assert(cons_pred_mode_child == cons_pred_mode);
                        }

                        bit_cnt = lbac_get_bits(&sbac_split) - bit_cnt;
                        cost_temp += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
                    }

                    lbac_t sbac_tree_c;

                    if (tree_status_child == TREE_L && tree_status == TREE_LC) {
                        lbac_copy(&sbac_tree_c, &sbac_split);
                    }

                    for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
                        int cur_part_num = part_num;
                        int log2_sub_cuw = split_struct.log_cuw[cur_part_num];
                        int log2_sub_cuh = split_struct.log_cuh[cur_part_num];
                        int x_pos = split_struct.x_pos[cur_part_num];
                        int y_pos = split_struct.y_pos[cur_part_num];
                        int cur_cuw = split_struct.width[cur_part_num];
                        int cur_cuh = split_struct.height[cur_part_num];
                        double part_max_cost = max_cost - cost_temp;

                        if (part_max_cost < 0) {
                           // cost_temp = MAX_COST;
                           // break;
                        }
                        if ((x_pos < info->pic_width) && (y_pos < info->pic_height)) {
                            cost_temp += mode_coding_tree(core, &sbac_split, x_pos, y_pos, split_struct.cup[cur_part_num], log2_sub_cuw, log2_sub_cuh, split_struct.cud
                                                          , split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child, part_max_cost);
                            copy_cu_data(cu_data_tmp, &core->cu_data_best[log2_sub_cuw - 2][log2_sub_cuh - 2], x_pos - split_struct.x_pos[0], y_pos - split_struct.y_pos[0], log2_sub_cuw, log2_sub_cuh, cu_width_log2, cud, tree_status_child);
                            prev_log2_sub_cuw = log2_sub_cuw;
                            prev_log2_sub_cuh = log2_sub_cuh;
                        }
                    }
                    if (cost_temp != MAX_COST && tree_status_child == TREE_L && tree_status == TREE_LC) {
                        core->tree_status = TREE_C;
                        core->cons_pred_mode = NO_MODE_CONS;
                        lbac_copy(&core->sbac_bakup, &sbac_tree_c);
                        cost_temp += mode_coding_unit(core, NULL, x0, y0, cu_width_log2, cu_height_log2, cud, cu_data_tmp);
                        core->tree_status = TREE_LC;
                    }
                    if (cost_temp < best_cons_cost) {
                        best_cons_cost = cost_temp;

                        if (best_cons_cost < best_split_cost) {
                            best_split_cost = best_cons_cost;

                            if (best_split_cost < cost_best - 0.0001) { /* backup the current best data */
                                copy_cu_data(cu_data_bst, cu_data_tmp, 0, 0, cu_width_log2, cu_height_log2, cu_width_log2, cud, tree_status);
                                cost_best = best_split_cost;
                                lbac_copy(sbac_cur, &sbac_split);
                                best_split_mode = split_mode;

                                best_cons_pred_mode = cons_pred_mode_child;

                                if (info->sqh.num_of_hmvp && slice_type != SLICE_I) {
                                    copy_motion_table(motion_cands_curr, &cnt_hmvp_cands_curr, core->motion_cands, core->cnt_hmvp_cands);
                                }
                            }
                        }
                    }
                }
                if (!p_bef_data->split_visit) {
                    p_bef_data->split_cost[split_mode] = best_cons_cost;
                }
            }
            if (!p_bef_data->split_visit && num_split_tried > 0) {
                if ((best_curr_cost *(1.10)) < best_split_cost) {
                    break;
                }
            }
        }
    }

    if (cost_best == MAX_COST) {
        return MAX_COST;
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
            p_bef_data->nosplit += 1;
        } else {
            p_bef_data->split += 1;
        }
        p_bef_data->split_visit = 1;
    }
    update_map_scu(core, x0, y0, 1 << cu_width_log2, 1 << cu_height_log2);

    return (cost_best > MAX_COST) ? MAX_COST : cost_best;
}

void enc_mode_init_frame(core_t *core)
{
    /* initialize pinter */
    inter_search_t *pi;
    
    pi         = &core->pinter;
    pi->refp   =  core->refp;
    pi->map_mv =  core->map->map_mv;
    pi->ptr    =  core->ptr;

}

int enc_mode_init_lcu(core_t *core)
{
    inter_search_t *pi;
    pi = &core->pinter;
    pi->lambda_mv = (u32)floor(65536.0 * core->sqrt_lambda[0]);
    core->lcu_qp_y = core->lcu_qp_y;
    core->lcu_qp_u = core->lcu_qp_u;
    core->lcu_qp_v = core->lcu_qp_v;

    return COM_OK;
}

int enc_mode_analyze_lcu(core_t *core, const lbac_t *lbac)
{
    com_scu_t *map_scu;
    int i, j, w, h;
    lbac_t sbac_root;
    com_info_t *info = core->info;
    com_map_t *map = core->map;
    int lcu_pos = core->lcu_y * info->pic_width_in_lcu + core->lcu_x;
    enc_cu_t *cu_data = &core->cu_data_best[info->log2_max_cuwh - 2][info->log2_max_cuwh - 2];

    lbac_copy(&sbac_root, lbac);

    /* decide mode */
    mode_coding_tree(core, &sbac_root, core->lcu_pix_x, core->lcu_pix_y, 0, info->log2_max_cuwh, info->log2_max_cuwh, 0 , NO_SPLIT, 0, 0, NO_MODE_CONS, TREE_LC, MAX_COST);

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


int enc_tq_nnz(core_t *core, com_mode_t *mode, int plane, int blk_idx, int qp, double lambda, s16 *coef, s16 *resi, int cu_width_log2, int cu_height_log2, int slice_type, int ch_type, int is_intra, int secT_Ver_Hor, int use_alt4x4Trans)
{
    transform(mode, plane, blk_idx, coef, resi, cu_width_log2, cu_height_log2, is_intra, ch_type, core->info->bit_depth_internal, secT_Ver_Hor, use_alt4x4Trans);
    return quant_non_zero(core, qp, lambda, is_intra, coef, cu_width_log2, cu_height_log2, ch_type, slice_type);
}

#if TR_EARLY_TERMINATE
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
#endif

int enc_tq_itdq_yuv_nnz(core_t *core, lbac_t *lbac, com_mode_t *cur_mode, s16 coef[N_C][MAX_CU_DIM], s16 resi[N_C][MAX_CU_DIM], pel pred[N_C][MAX_CU_DIM], pel rec[N_C][MAX_CU_DIM],
    s8 refi[REFP_NUM], s16 mv[REFP_NUM][MV_D], u8 is_mv_from_mvf)
{
    int i;
    int bit_depth = core->info->bit_depth_internal;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int slice_type = core->slice_type;

    cu_nz_cln(cur_mode->num_nz);

    // test 2Nx2N, loop luma and chroma
    cur_mode->tb_part = SIZE_2Nx2N;

    for (i = 0; i < N_C; i++) {
        if ((core->tree_status == TREE_L && i != Y_C) || (core->tree_status == TREE_C && i == Y_C)) {
            cur_mode->num_nz[TB0][i] = 0;
            continue;
        }
#if TR_SAVE_LOAD
        if (core->best_tb_part_hist == 255 || core->best_tb_part_hist == SIZE_2Nx2N || i != Y_C) {
#endif
            int plane_width_log2 = cu_width_log2 - (i != Y_C);
            int plane_height_log2 = cu_height_log2 - (i != Y_C);
            int qp = (i == Y_C ? core->lcu_qp_y : (i == U_C ? core->lcu_qp_u : core->lcu_qp_v));

            cur_mode->num_nz[TB0][i] = enc_tq_nnz(core, cur_mode, i, 0, qp, core->lambda[i], coef[i], resi[i], plane_width_log2, plane_height_log2, slice_type, i, 0, 0, 0);

            if (cur_mode->num_nz[TB0][i]) {
                ALIGNED_32(s16 resi_it[MAX_CU_DIM]);
                com_invqt(cur_mode, i, 0, coef[i], resi_it, core->wq, plane_width_log2, plane_height_log2, qp, bit_depth, 0, 0);
                uavs3e_funs_handle.recon[plane_width_log2 - MIN_CU_LOG2](resi_it, pred[i], 1 << plane_width_log2, 1 << plane_width_log2, 1 << plane_height_log2, rec[i], 1 << plane_width_log2, cur_mode->num_nz[TB0][i], bit_depth);
            }
#if TR_SAVE_LOAD
        }
        else {
            cur_mode->num_nz[TB0][i] = 0; //no need to try 2Nx2N transform
        }
#endif
    }

    int try_sub_block_transform = core->tree_status != TREE_C &&
        is_tb_avaliable(core->info, cu_width_log2, cu_height_log2, cur_mode->pb_part, MODE_INTER);

    //fast algorithm
    if (try_sub_block_transform) {
#if TR_SAVE_LOAD
        if (core->best_tb_part_hist == SIZE_2Nx2N) {
            try_sub_block_transform = 0;
        }
#endif
#if TR_EARLY_TERMINATE
        if (try_sub_block_transform && core->best_tb_part_hist == 255) {
            com_mode_t *mod_curr = &core->mod_info_curr;
            int bits_est = est_pred_info_bits(core);
            double bits_cost = RATE_TO_COST_LAMBDA(core->lambda[Y_C], bits_est);
            s64    dist_cost = core->dist_pred_luma >> (cur_mode->num_nz[TB0][Y_C] == 0 ? 4 : 6);
            if (bits_cost + dist_cost > core->cost_best) {
                try_sub_block_transform = 0;
            }
        }
#endif
    }

    if (try_sub_block_transform) {
        ALIGNED_32(s16 coef_NxN[MAX_CU_DIM]);
        int bak_2Nx2N_num_nz = cur_mode->num_nz[TB0][Y_C];
        int cu_size = 1 << (cu_width_log2 + cu_height_log2);
        int cu_width = 1 << cu_width_log2;
        int cu_height = 1 << cu_height_log2;
        int x = core->cu_pix_x;
        int y = core->cu_pix_y;
        pel *pred = cur_mode->pred[Y_C];
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

        for (i = 0; i < part_num; i++) {
            ALIGNED_32(s16 resi_buf[MAX_CU_DIM]);
            s16 *tb_resi = resi[Y_C];

            if (part_num > 1) {
                int k, pos_x, pos_y;
                int tu_w = 1 << log2_tb_w;
                int tu_h = 1 << log2_tb_h;
                s16 *s, *d;

                get_tb_start_pos(cu_w, cu_h, part_size, i, &pos_x, &pos_y);

                s = resi[Y_C] + pos_y * cu_w + pos_x;
                d = resi_buf;

                for (k = 0; k < tu_h; k++) {
                    memcpy(d, s, sizeof(s16) * tu_w);
                    d += tu_w;
                    s += cu_w;
                }
                tb_resi = resi_buf;
            }
            cur_mode->num_nz[i][Y_C] = enc_tq_nnz(core, cur_mode, Y_C, i, core->lcu_qp_y, core->lambda[Y_C], coef_NxN + i * tb_size, tb_resi, log2_tb_w, log2_tb_h, slice_type, Y_C, 0, 0, 0);
        }

        if (bak_2Nx2N_num_nz && is_cu_plane_nz(cur_mode->num_nz, Y_C)) {
            ALIGNED_32(pel rec_NxN[MAX_CU_DIM]);
            ALIGNED_32(s16 resi_it[MAX_CU_DIM]);
            int j, bit_cnt;
            int bak_NxN_num_nz[MAX_NUM_TB];

            /*************************************************************************************************************/
            /* cal NxN */
            for (j = 0; j < MAX_NUM_TB; j++) {
                bak_NxN_num_nz[j] = cur_mode->num_nz[j][Y_C];
            }
            com_invqt_inter_plane(cur_mode, Y_C, coef_NxN, resi_it, core->wq, cu_width_log2, cu_height_log2, core->lcu_qp_y, bit_depth);
            com_recon_plane(cur_mode->tb_part, resi_it, pred, cur_mode->num_nz, Y_C, cu_width, cu_height, cu_width, rec_NxN, bit_depth);
            cost_NxN = (double)(block_pel_ssd(cu_width_log2, cu_height, rec_NxN, org, cu_width, pic_org->stride_luma, bit_depth));

            lbac_copy(lbac, &core->sbac_bakup);
            bit_cnt = lbac_get_bits(lbac);

            enc_bits_inter_comp(core, lbac, coef_NxN, Y_C);
            bit_cnt = lbac_get_bits(lbac) - bit_cnt;
            cost_NxN += RATE_TO_COST_LAMBDA(core->lambda[Y_C], bit_cnt);

            /*************************************************************************************************************/
            /* cal 2Nx2N */
            cu_plane_nz_cln(cur_mode->num_nz, Y_C);
            cur_mode->num_nz[TB0][Y_C] = bak_2Nx2N_num_nz;
            cur_mode->tb_part = SIZE_2Nx2N;

            cost_2Nx2N = (double)(block_pel_ssd(cu_width_log2, cu_height, rec[Y_C], org, cu_width, pic_org->stride_luma, bit_depth));

            lbac_copy(lbac, &core->sbac_bakup);
            bit_cnt = lbac_get_bits(lbac);

            enc_bits_inter_comp(core, lbac, coef[Y_C], Y_C);
            bit_cnt = lbac_get_bits(lbac) - bit_cnt;
            cost_2Nx2N += RATE_TO_COST_LAMBDA(core->lambda[Y_C], bit_cnt);

            if (cost_NxN < cost_2Nx2N) {
                memcpy(coef[Y_C], coef_NxN, sizeof(s16) *cu_size);
                memcpy(rec[Y_C], rec_NxN, sizeof(pel) *cu_size);

                for (j = 0; j < MAX_NUM_TB; j++) {
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
            com_recon_plane(cur_mode->tb_part, resi_it, pred, cur_mode->num_nz, Y_C, cu_width, cu_height, cu_width, rec[Y_C], bit_depth);
        }
    }
    check_set_tb_part(cur_mode);
    return is_cu_nz(cur_mode->num_nz);
}
