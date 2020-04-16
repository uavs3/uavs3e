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
#include <math.h>

#define MV_COST(mv_bits) (u32)((lambda_mv * mv_bits + (1 << 15)) >> 16)

#define SWAP(a, b, t) { (t) = (a); (a) = (b); (b) = (t); }

static avs3_always_inline int get_mv_bits_with_mvr(inter_search_t *pi, int mvd_x, int mvd_y, u8 mvr_idx)
{
    return pi->tab_mvbits[mvd_x >> mvr_idx] + pi->tab_mvbits[mvd_y >> mvr_idx] + mvr_idx + (mvr_idx < MAX_NUM_MVR - 1);
}

static avs3_always_inline int get_mv_bits(inter_search_t *pi, int mvd_x, int mvd_y, int num_refp, int refi, u8 mvr_idx)
{
    return tbl_refi_bits[num_refp][refi] + pi->tab_mvbits[mvd_x >> mvr_idx] + pi->tab_mvbits[mvd_y >> mvr_idx];
}

static void create_bi_org(pel *org, pel *pred, int s_o, int cu_width, int cu_height, pel *org_bi, int s_pred, int bit_depth)
{
    const int max_val = (1 << bit_depth) - 1;

    for (int j = 0; j < cu_height; j++) {
        for (int i = 0; i < cu_width; i++) {
            int val = (((int)org[i]) << 1) - pred[i];
            org_bi[i] = COM_CLIP3(0, max_val, val);
        }
        org    += s_o;
        pred   += s_pred;
        org_bi += s_pred;
    }
}

static void make_cand_list(core_t *core, int *mode_list, double *cost_list, int num_cands_woUMVE, int num_cands_all, int num_rdo, s16 pmv_skip_cand[MAX_SKIP_NUM][REFP_NUM][MV_D],  s8 refi_skip_cand[MAX_SKIP_NUM][REFP_NUM])
{
    com_info_t *info     =  core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    com_pic_t *pic_org   =  core->pic_org;
    int slice_type       =  core->pichdr->slice_type;
    int x                =  core->cu_pix_x;
    int y                =  core->cu_pix_y;
    int cu_width         =  core->cu_width;
    int cu_height        =  core->cu_height;
    int cu_width_log2    =  core->cu_width_log2;
    int cu_height_log2   =  core->cu_height_log2;
    int bit_depth        =  info->bit_depth_internal;

    pel *y_org = pic_org->y +  x       +  y       * pic_org->stride_luma;
    pel *u_org = pic_org->u + (x >> 1) + (y >> 1) * pic_org->stride_chroma;
    pel *v_org = pic_org->v + (x >> 1) + (y >> 1) * pic_org->stride_chroma;

    cur_info->cu_mode = MODE_SKIP;

    for (int i = 0; i < num_rdo; i++) {
        mode_list[i] = 0;
        cost_list[i] = MAX_D_COST;
    }
    for (int skip_idx = 0; skip_idx < num_cands_all; skip_idx++) {
        int shift = 0;

        com_assert(REFI_IS_VALID(refi_skip_cand[skip_idx][REFP_0]) || REFI_IS_VALID(refi_skip_cand[skip_idx][REFP_1]));

        if (skip_idx < num_cands_woUMVE) {
            cur_info->umve_flag = 0;
            cur_info->skip_idx = skip_idx;
        } else {
            cur_info->umve_flag = 1;
            cur_info->umve_idx = skip_idx - num_cands_woUMVE;
        }
        if ((slice_type == SLICE_P) && (cur_info->skip_idx == 1 || cur_info->skip_idx == 2) && (cur_info->umve_flag == 0)) {
            continue;
        }
        com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi_skip_cand[skip_idx], pmv_skip_cand[skip_idx], core->refp, cur_info->pred, cu_width, CHANNEL_L, bit_depth);
        
        double cost = com_had(cu_width, cu_height, y_org, pic_org->stride_luma, cur_info->pred[Y_C], cu_width, bit_depth);

        lbac_t lbac_temp;
        lbac_copy(&lbac_temp, &core->lbac_bakup);
        int bit_cnt = lbac_get_bits(&lbac_temp);
        enc_bits_inter_skip_flag(core, &lbac_temp);
        bit_cnt = lbac_get_bits(&lbac_temp) - bit_cnt;
        cost += core->sqrt_lambda[0] * bit_cnt;

        while (shift < num_rdo && cost < cost_list[num_rdo - 1 - shift]) {
            shift++;
        }
        if (shift != 0) {
            for (int i = 1; i < shift; i++) {
                mode_list[num_rdo - i] = mode_list[num_rdo - 1 - i];
                cost_list[num_rdo - i] = cost_list[num_rdo - 1 - i];
            }
            mode_list[num_rdo - shift] = skip_idx;
            cost_list[num_rdo - shift] = cost;
        }
    }
}

static void check_best_mode(core_t *core, lbac_t *lbac_best, lbac_t *lbac, const double cost_curr, pel(*pred)[MAX_CU_DIM])
{
    com_mode_t *bst_info = &core->mod_info_best;
    com_mode_t *cur_info = &core->mod_info_curr;

    if (cost_curr < core->cost_best) {
        int lidx;
        int cu_width_log2 = core->cu_width_log2;
        int cu_height_log2 = core->cu_height_log2;
        int cu_width = 1 << cu_width_log2;
        int cu_height = 1 << cu_height_log2;
        bst_info->cu_mode = cur_info->cu_mode;

        check_tb_part(cur_info);
        bst_info->pb_part = cur_info->pb_part;
        bst_info->tb_part = cur_info->tb_part;
        memcpy(&bst_info->pb_info, &cur_info->pb_info, sizeof(com_part_info_t));
        memcpy(&bst_info->tb_info, &cur_info->tb_info, sizeof(com_part_info_t));
        bst_info->umve_flag = cur_info->umve_flag;

        core->cost_best = cost_curr;

        lbac_copy(lbac_best, lbac);

        if (bst_info->cu_mode != MODE_INTRA) {
            if (cur_info->affine_flag) {
                assert(cur_info->mvr_idx < MAX_NUM_AFFINE_MVR);
            }

            if (bst_info->cu_mode == MODE_SKIP || bst_info->cu_mode == MODE_DIR) {
                bst_info->mvr_idx = 0;
            } else {
                bst_info->hmvp_flag = cur_info->hmvp_flag;
                bst_info->mvr_idx = cur_info->mvr_idx;
            }

            bst_info->refi[REFP_0] = cur_info->refi[REFP_0];
            bst_info->refi[REFP_1] = cur_info->refi[REFP_1];
            for (lidx = 0; lidx < REFP_NUM; lidx++) {
                bst_info->mv[lidx][MV_X] = cur_info->mv[lidx][MV_X];
                bst_info->mv[lidx][MV_Y] = cur_info->mv[lidx][MV_Y];
                bst_info->mvd[lidx][MV_X] = cur_info->mvd[lidx][MV_X];
                bst_info->mvd[lidx][MV_Y] = cur_info->mvd[lidx][MV_Y];
            }

            bst_info->smvd_flag = cur_info->smvd_flag;
            if (cur_info->smvd_flag) {
                assert(cur_info->affine_flag == 0);
            }

            bst_info->affine_flag = cur_info->affine_flag;
            if (bst_info->affine_flag) {
                int vertex;
                int vertex_num = bst_info->affine_flag + 1;
                for (lidx = 0; lidx < REFP_NUM; lidx++) {
                    for (vertex = 0; vertex < vertex_num; vertex++) {
                        CP64(bst_info->affine_mv [lidx][vertex], cur_info->affine_mv [lidx][vertex]);
                        CP32(bst_info->affine_mvd[lidx][vertex], cur_info->affine_mvd[lidx][vertex]);
                    }
                }
            }

            if (bst_info->cu_mode == MODE_SKIP) {
                if (bst_info->umve_flag != 0) {
                    bst_info->umve_idx = cur_info->umve_idx;
                } else {
                    bst_info->skip_idx = cur_info->skip_idx;
                }

                for (int i = 0; i < N_C; i++) {
                    int size = cu_width * cu_height >> (i ? 2 : 0);
                    com_mcpy(bst_info->rec[i], pred[i], size * sizeof(pel));
                }
                com_mset(bst_info->num_nz, 0, sizeof(int)*N_C * MAX_NUM_TB);
                assert(bst_info->pb_part == SIZE_2Nx2N);
            } else {
                if (bst_info->cu_mode == MODE_DIR) {
                    if (bst_info->umve_flag) {
                        bst_info->umve_idx = cur_info->umve_idx;
                    } else {
                        bst_info->skip_idx = cur_info->skip_idx;
                    }
                }
                for (int i = 0; i < N_C; i++) {
                    int size = (cu_width * cu_height) >> (i ? 2 : 0);
                    cu_plane_nz_cpy(bst_info->num_nz, cur_info->num_nz, i);

                    com_mcpy(bst_info->coef[i], cur_info->coef[i], size * sizeof(s16));
                    com_mcpy(bst_info->pred[i], pred[i], size * sizeof(pel));

                    if (is_cu_plane_nz(bst_info->num_nz, i)) {
                        com_mcpy(bst_info->rec[i], cur_info->rec[i], size * sizeof(pel));
                    } else {
                        com_mcpy(bst_info->rec[i], pred[i], size * sizeof(pel));
                    }
                }
            }
        }
    }
    double skip_mode_2_threshold = core->lcu_qp_y / 60.0 * (THRESHOLD_MVPS_CHECK - 1) + 1;
    if (bst_info->cu_mode == MODE_SKIP && cur_info->cu_mode == MODE_INTER && (core->cost_best * skip_mode_2_threshold) > cost_curr) {
        core->skip_mvps_check = 0;
    }
}

#if TR_SAVE_LOAD
static u8 search_inter_tr_info(core_t *core, u16 cu_ssd)
{
    enc_history_t *p_data = &core->history_data[core->cu_width_log2 - 2][core->cu_height_log2 - 2][core->cu_scup_in_lcu];

    for (int idx = 0; idx < p_data->num_inter_pred; idx++) {
        if (p_data->inter_pred_dist[idx] == cu_ssd) {
            return p_data->inter_tb_part[idx];
        }
    }
    return 255;
}

static void save_inter_tr_info(core_t *core, u16 cu_ssd, u8 tb_part_size)
{
    enc_history_t *p_data = &core->history_data[core->cu_width_log2 - 2][core->cu_height_log2 - 2][core->cu_scup_in_lcu];

    if (p_data->num_inter_pred == NUM_SL_INTER) {
        return;
    }
    p_data->inter_pred_dist[p_data->num_inter_pred] = cu_ssd;
    p_data->inter_tb_part  [p_data->num_inter_pred] = tb_part_size;
    p_data->num_inter_pred++;
}
#endif

static double inter_rdcost(core_t *core, lbac_t *lbac_best_ret, int bForceAllZero, int need_mc, s64 dist_input[2][N_C], s64 *dist_pred_input)
{
    com_pic_t *pic_org     =  core->pic_org;
    com_info_t *info       =  core->info;
    com_map_t *map         =  core->map;
    com_mode_t *cur_info   = &core->mod_info_curr;
    int x                  = core->cu_pix_x;
    int y                  = core->cu_pix_y;
    int cu_width_log2      = core->cu_width_log2;
    int cu_height_log2     = core->cu_height_log2;
    int cu_width           = 1 << cu_width_log2;
    int cu_height          = 1 << cu_height_log2;

    s16(*coef)[MAX_CU_DIM] = cur_info->coef;
    pel(*pred)[MAX_CU_DIM] = cur_info->pred;
    pel(* rec)[MAX_CU_DIM] = cur_info->rec;
    s16(*mv)[MV_D]         = cur_info->mv;
    s8 *refi               = cur_info->refi;
    int(*num_nz_coef)[N_C] = cur_info->num_nz;

    s64 dist_local[2][N_C] = { 0 };
    s64 dist_pred_local[N_C] = { 0 };
    int cbf_best [N_C] = { 0 };
    int cbf_comps[N_C] = { 0 };
    s64 (*dist)[N_C] = dist_input      ? dist_input      : dist_local;
    s64  *dist_pred  = dist_pred_input ? dist_pred_input : dist_pred_local;

    double cost_best = MAX_D_COST;
    int num_n_c = core->tree_status == TREE_LC ? N_C : 1;
    int bit_depth = info->bit_depth_internal;
    u8  is_from_mv_field = 0;
    int slice_type = core->pichdr->slice_type;
    lbac_t  lbac_best;
    lbac_t  lbac_temp;
    lbac_t *lbac = &lbac_temp;

    int width[N_C], height[N_C], log2_w[N_C], log2_h[N_C], stride_org[N_C];
    pel *org[N_C];

    width     [Y_C] = 1 << cu_width_log2;
    height    [Y_C] = 1 << cu_height_log2;
    width     [U_C] = width [V_C] = 1 << (cu_width_log2  - 1);
    height    [U_C] = height[V_C] = 1 << (cu_height_log2 - 1);
    log2_w    [Y_C] = cu_width_log2;
    log2_h    [Y_C] = cu_height_log2;
    log2_w    [U_C] = log2_w[V_C] = cu_width_log2  - 1;
    log2_h    [U_C] = log2_h[V_C] = cu_height_log2 - 1;
    org       [Y_C] = pic_org->y + (y * pic_org->stride_luma) + x;
    org       [U_C] = pic_org->u + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);
    org       [V_C] = pic_org->v + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);
    stride_org[Y_C] = pic_org->stride_luma;
    stride_org[U_C] = pic_org->stride_chroma;
    stride_org[V_C] = pic_org->stride_chroma;

    cur_info->tb_part = SIZE_2Nx2N;

    if (cur_info->affine_flag) {
        com_set_affine_mvf(core->cu_scup_in_pic, cu_width_log2, cu_height_log2, info->i_scu, cur_info, map, core->pichdr);
        is_from_mv_field = 1;
    }
    if (need_mc) { // prediction
        if (cur_info->affine_flag) {
            com_mc_cu_affine(x, y, info->pic_width, info->pic_height, width[0], height[0], refi, cur_info->affine_mv, core->refp, pred, cur_info->affine_flag + 1, core->pichdr, bit_depth);
        } else {
            com_mc_cu(x, y, info->pic_width, info->pic_height, width[0], height[0], refi, mv, core->refp, pred, width[0], core->tree_status, bit_depth);
        }
        for (int i = 0; i < num_n_c; i++) {
            dist[0][i] = dist_pred[i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], pred[i], org[i], width[i], stride_org[i], bit_depth);
        }
        dist[0][0] += calc_dist_filter_boundary(core, core->pic_rec, core->pic_org, cu_width, cu_height, pred[Y_C], cu_width, x, y, 0, 0, refi, mv, is_from_mv_field, 1);
    } 

    /* test all zero case */
    if (cur_info->cu_mode != MODE_DIR) { // do not check forced zero for direct mode
        memset(num_nz_coef, 0, sizeof(int) * MAX_NUM_TB * N_C);

        if (core->tree_status == TREE_LC) {
            cost_best = (double)dist[0][Y_C] + ((dist[0][U_C] + dist[0][V_C]) * core->dist_chroma_weight[0]);
        } else {
            assert(core->tree_status == TREE_L);
            cost_best = (double)dist[0][Y_C];
        }
        cost_best += get_bits_cost(core, &lbac_best, slice_type, core->lambda[0]);
    }

    /* transform and quantization */
    bForceAllZero |= (cur_info->cu_mode == MODE_SKIP);
    bForceAllZero |= cu_width_log2 > 6 || cu_height_log2 > 6;

    if (!bForceAllZero) {
        ALIGNED_32(s16 resi_t[N_C][MAX_CU_DIM]);

#if TR_EARLY_TERMINATE
        core->dist_pred_luma = dist_pred[Y_C];
#endif
#if TR_SAVE_LOAD 
        u16 cu_ssd_u16 = 0;
        core->best_tb_part_hist = 255;
        if (info->sqh.pbt_enable && core->tree_status != TREE_L &&
            is_tb_avaliable(info, cu_width_log2, cu_height_log2, cur_info->pb_part, MODE_INTER) && 
            cur_info->pb_part == SIZE_2Nx2N) 
        {
            s64 cu_ssd_s64 = dist_pred[Y_C] + dist_pred[U_C] + dist_pred[V_C];
            int shift_val = COM_MIN(cu_width_log2 + cu_height_log2, 9);
            cu_ssd_u16 = (u16)(cu_ssd_s64 + (s64)(1 << (shift_val - 1))) >> shift_val;
            core->best_tb_part_hist = search_inter_tr_info(core, cu_ssd_u16);
        }
#endif
        cu_pel_sub(core->tree_status, x, y, cu_width_log2, cu_height_log2, core->pic_org, pred, resi_t);

        if (enc_tq_itdq_yuv_nnz(core, lbac, cur_info, coef, resi_t, pred, rec, refi, mv)) {
            for (int i = 0; i < num_n_c; i++) {
                if (is_cu_plane_nz(num_nz_coef, i)) {
                    if (i == 0) {
                        dist[1][i] = calc_dist_filter_boundary(core, core->pic_rec, core->pic_org, cu_width, cu_height, rec[Y_C], cu_width, x, y, 0, 1, refi, mv, is_from_mv_field, 0);
                    } else {
                        dist[1][i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], rec[i], org[i], width[i], stride_org[i], bit_depth);
                    }
                } else {
                    dist[1][i] = dist[0][i];
                }
            }

            int cbf_y = is_cu_plane_nz(num_nz_coef, Y_C) > 0 ? 1 : 0;
            int cbf_u = is_cu_plane_nz(num_nz_coef, U_C) > 0 ? 1 : 0;
            int cbf_v = is_cu_plane_nz(num_nz_coef, V_C) > 0 ? 1 : 0;

            double cost = get_bits_cost(core, lbac, slice_type, core->lambda[0]);

            if (core->tree_status == TREE_LC) {
                cost += (double)dist[cbf_y][Y_C] + ((dist[cbf_u][U_C] + dist[cbf_v][V_C]) * core->dist_chroma_weight[0]);
            } else {
                cost += (double)dist[cbf_y][Y_C];
            }
            if (cost < cost_best) {
                cost_best = cost;
                cbf_best[Y_C] = cbf_y;
                cbf_best[U_C] = cbf_u;
                cbf_best[V_C] = cbf_v;
                lbac_copy(&lbac_best, lbac);
            }

            int nnz_store[MAX_NUM_TB][N_C];
            com_mcpy(nnz_store, num_nz_coef, sizeof(int) * MAX_NUM_TB * N_C);
            int tb_part_store = cur_info->tb_part;

            if (cbf_y + cbf_u + cbf_v > 1) {
                if (core->tree_status == TREE_LC) {
                    lbac_t lbac_cur_comp_best;
                    lbac_copy(&lbac_cur_comp_best, &core->lbac_bakup);

                    for (int i = 0; i < N_C; i++) {
                        if (is_cu_plane_nz(nnz_store, i)) {
                            double cost_comp_best = MAX_D_COST;
                            lbac_t lbac_cur_comp;
                            lbac_copy(&lbac_cur_comp, &lbac_cur_comp_best);

                            for (int j = 0; j < 2; j++) {
                                cost = dist[j][i] * (i == 0 ? 1 : core->dist_chroma_weight[i - 1]);
                                if (j) {
                                    cu_plane_nz_cpy(num_nz_coef, nnz_store, i);
                                    if (i == 0) {
                                        cur_info->tb_part = tb_part_store;
                                    }
                                } else {
                                    cu_plane_nz_cln(num_nz_coef, i);
                                    if (i == 0) {
                                        cur_info->tb_part = SIZE_2Nx2N;
                                    }
                                }
                                cost += get_bits_cost_comp(core, lbac, &lbac_cur_comp, coef[i], core->lambda[i], i);

                                if (cost < cost_comp_best) {
                                    cost_comp_best = cost;
                                    cbf_comps[i] = j;
                                    lbac_copy(&lbac_cur_comp_best, lbac);
                                }
                            }
                        } else {
                            cbf_comps[i] = 0;
                        }
                    }
                
                    if (cbf_comps[Y_C] || cbf_comps[U_C] || cbf_comps[V_C]) {
                        for (int i = 0; i < N_C; i++) {
                            if (cbf_comps[i]) {
                                cu_plane_nz_cpy(num_nz_coef, nnz_store, i);
                                if (i == 0) {
                                    cur_info->tb_part = tb_part_store;
                                }
                            } else {
                                cu_plane_nz_cln(num_nz_coef, i);
                                if (i == 0) {
                                    cur_info->tb_part = SIZE_2Nx2N;
                                }
                            }
                        }
                        if (!is_cu_nz_equ(num_nz_coef, nnz_store)) {
                            cbf_y = cbf_comps[Y_C];
                            cbf_u = cbf_comps[U_C];
                            cbf_v = cbf_comps[V_C];
                            cost = dist[cbf_y][Y_C] + ((dist[cbf_u][U_C] + dist[cbf_v][V_C]) * core->dist_chroma_weight[0]);
                            cost += get_bits_cost(core, lbac, slice_type, core->lambda[0]);

                            if (cost < cost_best) {
                                cost_best = cost;
                                cbf_best[Y_C] = cbf_y;
                                cbf_best[U_C] = cbf_u;
                                cbf_best[V_C] = cbf_v;
                                lbac_copy(&lbac_best, lbac);
                            }
                        }
                    }
                }
            }
            for (int i = 0; i < num_n_c; i++) {
                if (cbf_best[i]) {
                    cu_plane_nz_cpy(num_nz_coef, nnz_store, i);
                    if (i == 0) {
                        cur_info->tb_part = tb_part_store;
                    }
                } else {
                    cu_plane_nz_cln(num_nz_coef, i);
                    if (i == 0) {
                        cur_info->tb_part = SIZE_2Nx2N;
                    }
                }
            }
        }
#if TR_SAVE_LOAD 
        if (core->best_tb_part_hist == 255 && core->tree_status != TREE_L && cur_info->pb_part == SIZE_2Nx2N) {
            save_inter_tr_info(core, cu_ssd_u16, (u8)cur_info->tb_part);
        }
#endif
    }

    check_best_mode(core, lbac_best_ret, &lbac_best, cost_best, pred);

    return cost_best;
}

double pinter_residue_rdo_chroma(core_t *core)
{
    com_pic_t *pic_org   = core->pic_org;
    com_info_t *info     = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    int x                = core->cu_pix_x;
    int y                = core->cu_pix_y;
    int cu_width_log2    = core->cu_width_log2;
    int cu_height_log2   = core->cu_height_log2;
    int cu_width         = 1 << cu_width_log2;
    int cu_height        = 1 << cu_height_log2;

    s16(*coef)[MAX_CU_DIM] = cur_info->coef;
    pel(*pred)[MAX_CU_DIM] = cur_info->pred;
    pel(*rec )[MAX_CU_DIM] = cur_info->rec;
    s16(*mv)[MV_D]         = cur_info->mv;
    s8 *refi               = cur_info->refi;
    int(*num_nz_coef)[N_C] = cur_info->num_nz;

    ALIGNED_32(s16 resi_t[N_C][MAX_CU_DIM]);

    int bit_depth = info->bit_depth_internal;
    s64    dist[2] [N_C] = { 0 };
    int    cbf_best[N_C] = { 0 };
    double cost, cost_best = MAX_D_COST;

    pel  *org[N_C];
    int width[N_C], height[N_C], log2_w[N_C], log2_h[N_C];;

    width [Y_C] = 1 << cu_width_log2;
    height[Y_C] = 1 << cu_height_log2;
    width [U_C] = width[V_C] = 1 << (cu_width_log2 - 1);
    height[U_C] = height[V_C] = 1 << (cu_height_log2 - 1);
    log2_w[Y_C] = cu_width_log2;
    log2_h[Y_C] = cu_height_log2;
    log2_w[U_C] = log2_w[V_C] = cu_width_log2 - 1;
    log2_h[U_C] = log2_h[V_C] = cu_height_log2 - 1;
    org   [Y_C] = pic_org->y + (y * pic_org->stride_luma) + x;
    org   [U_C] = pic_org->u + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);
    org   [V_C] = pic_org->v + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);

    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);

    int luma_scup = core->cu_scup_in_pic + PEL2SCU(core->cu_width - 1) + PEL2SCU(core->cu_height - 1) * info->i_scu;

    com_map_t *map = core->map;

    for (int i = 0; i < REFP_NUM; i++) {
        refi[i] = map->map_refi[luma_scup][i];
        mv[i][MV_X] = map->map_mv[luma_scup][i][MV_X];
        mv[i][MV_Y] = map->map_mv[luma_scup][i][MV_Y];
    }

    assert(cur_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);
    com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi, mv, core->refp, pred, cu_width, core->tree_status, bit_depth);

    cu_pel_sub(core->tree_status, x, y, cu_width_log2, cu_height_log2, core->pic_org, pred, resi_t);
    memset(dist, 0, sizeof(s64) * 2 * N_C);

    for (int i = 1; i < N_C; i++) {
        dist[0][i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], pred[i], org[i], width[i], pic_org->stride_chroma, bit_depth);
    }

    memset(num_nz_coef, 0, sizeof(int) * MAX_NUM_TB * N_C);
    assert(cur_info->tb_part == SIZE_2Nx2N);
    cost_best = (double)((dist[0][U_C] + dist[0][V_C]) * core->dist_chroma_weight[0]);

    lbac_t lbac_temp;
    cost_best += get_bits_cost_coef(core, &lbac_temp, coef);
    cur_info->tb_part = SIZE_2Nx2N;

    if (enc_tq_itdq_yuv_nnz(core, &lbac_temp, cur_info, coef, resi_t, pred, rec, refi, mv)) {
        for (int i = 1; i < N_C; i++) {
            if (is_cu_plane_nz(num_nz_coef, i)) {
                dist[1][i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], rec[i], org[i], width[i], pic_org->stride_chroma, bit_depth);
            } else {
                dist[1][i] = dist[0][i];
            }
        }

        int cbf_y = is_cu_plane_nz(num_nz_coef, Y_C) > 0 ? 1 : 0;
        int cbf_u = is_cu_plane_nz(num_nz_coef, U_C) > 0 ? 1 : 0;
        int cbf_v = is_cu_plane_nz(num_nz_coef, V_C) > 0 ? 1 : 0;

        cost = (double)((dist[cbf_u][U_C] + dist[cbf_v][V_C]) * core->dist_chroma_weight[0]);
        cost += get_bits_cost_coef(core, &lbac_temp, coef);

        if (cost < cost_best) {
            cost_best = cost;
            cbf_best[Y_C] = cbf_y;
            cbf_best[U_C] = cbf_u;
            cbf_best[V_C] = cbf_v;
        }
    }

    com_mode_t *mod_best = &core->mod_info_best;

    for (int i = 1; i < N_C; i++) {
        int size_tmp = (cu_width * cu_height) >> (i == 0 ? 0 : 2);
        if (cbf_best[i] == 0) {
            cu_plane_nz_cln(mod_best->num_nz, i);
            com_mset(mod_best->coef[i], 0      , sizeof(s16) * size_tmp);
            com_mcpy(mod_best->rec [i], pred[i], sizeof(s16) * size_tmp);
        } else {
            cu_plane_nz_cpy(mod_best->num_nz, num_nz_coef, i);
            com_mcpy(mod_best->coef[i], coef[i], sizeof(s16) * size_tmp);
            com_mcpy(mod_best->rec [i], rec [i], sizeof(s16) * size_tmp);
        }
    }

    return cost_best;
}

static void init_inter_data(core_t *core)
{
    com_mode_t *cur_info = &core->mod_info_curr;
    com_info_t *info = core->info;

    cur_info->skip_idx = 0;
    cur_info->smvd_flag = 0;

    get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, cur_info->pb_part, &cur_info->pb_info);
    assert(cur_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);

    com_mset(cur_info->num_nz,     0, sizeof( int) * N_C * MAX_NUM_TB);
    com_mset(cur_info->mv,         0, sizeof( s16) * REFP_NUM * MV_D);
    com_mset(cur_info->mvd,        0, sizeof( s16) * REFP_NUM * MV_D);
    com_mset(cur_info->refi,       0, sizeof(  s8) * REFP_NUM);
    com_mset(cur_info->affine_mv,  0, sizeof(CPMV) * REFP_NUM * VER_NUM * MV_D);
    com_mset(cur_info->affine_mvd, 0, sizeof( s16) * REFP_NUM * VER_NUM * MV_D);
}

static void derive_inter_cands(core_t *core, s16(*pmv_cands)[REFP_NUM][MV_D], s8(*refi_cands)[REFP_NUM], int *num_cands_all, int *num_cands_woUMVE)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    int num_cands = 0;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    int scup_co = get_colocal_scup(core->cu_scup_in_pic, info->i_scu, info->pic_width_in_scu, info->pic_height_in_scu);
    com_map_t *map = core->map;
    com_motion_t motion_cands_curr[MAX_SKIP_NUM];
    s8 cnt_hmvp_cands_curr = 0;

    int umve_idx;
    s16 pmv_base_cands[UMVE_BASE_NUM][REFP_NUM][MV_D];
    s8 refi_base_cands[UMVE_BASE_NUM][REFP_NUM];

    cur_info->affine_flag = 0;

    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);
    init_inter_data(core);

    num_cands = 0;

    if (core->pichdr->slice_type == SLICE_P) {
        refi_cands[num_cands][REFP_0] = 0;
        refi_cands[num_cands][REFP_1] = -1;
        if (REFI_IS_VALID(core->refp[0][REFP_0].map_refi[scup_co][REFP_0])) {
            get_col_mv_from_list0(core->refp[0], core->ptr, scup_co, pmv_cands[num_cands]);
        } else {
            M32(pmv_cands[num_cands][REFP_0]) = 0;
        }
        M32(pmv_cands[num_cands][REFP_1]) = 0;
    } else {
        if (!REFI_IS_VALID(core->refp[0][REFP_1].map_refi[scup_co][REFP_0])) {
            com_get_mvp_default(core->ptr, core->cu_scup_in_pic, REFP_0, 0, map->map_mv, map->map_refi, core->refp,
                                0, core->cu_width, core->cu_height, info->i_scu, pmv_cands[num_cands][REFP_0], map->map_scu);

            com_get_mvp_default(core->ptr, core->cu_scup_in_pic, REFP_1, 0, map->map_mv, map->map_refi, core->refp,
                                0, core->cu_width, core->cu_height, info->i_scu, pmv_cands[num_cands][REFP_1], map->map_scu);
        } else {
            get_col_mv(core->refp[0], core->ptr, scup_co, pmv_cands[num_cands]);
        }
        SET_REFI(refi_cands[num_cands], 0, 0);
    }
    num_cands++;

    derive_MHBskip_spatial_motions(core->cu_scup_in_pic, cu_width, cu_height, info->i_scu, map->map_scu, map->map_mv, map->map_refi, &pmv_cands[num_cands], &refi_cands[num_cands]);
    num_cands += PRED_DIR_NUM;

    if (info->sqh.num_of_hmvp) {
        int skip_idx;
        for (skip_idx = 0; skip_idx < num_cands; skip_idx++) {
            fill_skip_candidates(motion_cands_curr, &cnt_hmvp_cands_curr, info->sqh.num_of_hmvp, pmv_cands[skip_idx], refi_cands[skip_idx], 0);
        }
        for (skip_idx = core->cnt_hmvp_cands; skip_idx > 0; skip_idx--) { 
            com_motion_t motion = core->motion_cands[skip_idx - 1];
            fill_skip_candidates(motion_cands_curr, &cnt_hmvp_cands_curr, info->sqh.num_of_hmvp, motion.mv, motion.ref_idx, 1);
        }

        s8 cnt_hmvp_extend = cnt_hmvp_cands_curr;
        com_motion_t motion = core->cnt_hmvp_cands ? core->motion_cands[core->cnt_hmvp_cands - 1] : motion_cands_curr[TRADITIONAL_SKIP_NUM - 1];
        
        // use last HMVP candidate or last spatial candidate to fill the rest
        for (skip_idx = cnt_hmvp_cands_curr; skip_idx < (TRADITIONAL_SKIP_NUM + info->sqh.num_of_hmvp); skip_idx++) { 
            fill_skip_candidates(motion_cands_curr, &cnt_hmvp_extend, info->sqh.num_of_hmvp, motion.mv, motion.ref_idx, 0);
        }
        assert(cnt_hmvp_extend == (TRADITIONAL_SKIP_NUM + info->sqh.num_of_hmvp));

        get_hmvp_skip_cands(motion_cands_curr, cnt_hmvp_extend, pmv_cands, refi_cands);
        num_cands = cnt_hmvp_cands_curr;
    }

    *num_cands_woUMVE = num_cands;
    if (info->sqh.umve_enable) {
        derive_umve_base_motions(core->cu_scup_in_pic, cu_width, cu_height, info->i_scu, map->map_scu, map->map_mv, map->map_refi, pmv_cands[0], refi_cands[0], pmv_base_cands, refi_base_cands);

        for (umve_idx = 0; umve_idx < UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM; umve_idx++) {
            derive_umve_final_motions(umve_idx, core->refp, core->ptr, pmv_base_cands, refi_base_cands, &pmv_cands[*num_cands_woUMVE], &refi_cands[*num_cands_woUMVE]);
        }
        num_cands += UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM;
    }
    *num_cands_all = num_cands;
}

static void analyze_direct_skip(core_t *core, lbac_t *lbac_best)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    s16 pmv_cands[MAX_SKIP_NUM + UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM][REFP_NUM][MV_D];
    s8 refi_cands[MAX_SKIP_NUM + UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM][REFP_NUM];
    double cost_list[MAX_INTER_SKIP_RDO];
    int    mode_list[MAX_INTER_SKIP_RDO];
    int num_cands_all, num_rdo, num_cands_woUMVE;

    derive_inter_cands(core, pmv_cands, refi_cands, &num_cands_all, &num_cands_woUMVE);
    num_rdo = num_cands_woUMVE;
    assert(num_rdo <= COM_MIN(MAX_INTER_SKIP_RDO, TRADITIONAL_SKIP_NUM + info->sqh.num_of_hmvp));

    make_cand_list(core, mode_list, cost_list, num_cands_woUMVE, num_cands_all, num_rdo, pmv_cands, refi_cands);

    for (int skip_idx = 0; skip_idx < num_rdo; skip_idx++) {
        int mode = mode_list[skip_idx];

        if (mode < num_cands_woUMVE) {
            cur_info->umve_flag = 0;
            cur_info->skip_idx = mode;
        } else {
            cur_info->umve_flag = 1;
            cur_info->umve_idx = mode - num_cands_woUMVE;
        }

        CP32(cur_info->mv[REFP_0], pmv_cands[mode][REFP_0]);
        CP32(cur_info->mv[REFP_1], pmv_cands[mode][REFP_1]);
        CP16(cur_info->refi, refi_cands[mode]);

        if (!REFI_IS_VALID(cur_info->refi[REFP_0]) && !REFI_IS_VALID(cur_info->refi[REFP_1])) {
            continue;
        }
        if ((core->pichdr->slice_type == SLICE_P) && (cur_info->skip_idx == 1 || cur_info->skip_idx == 2) && (cur_info->umve_flag == 0)) {
            continue;
        }

        s64 dist[2][N_C] = { 0 };
        s64 dist_pred[N_C] = { 0 };

        cur_info->cu_mode = MODE_DIR;
        inter_rdcost(core, lbac_best, 0, 1, dist, dist_pred);

        cur_info->cu_mode = MODE_SKIP;
        inter_rdcost(core, lbac_best, 1, 0, dist, dist_pred);
    }
}

static void analyze_affine_merge(core_t *core, lbac_t *lbac_best)
{
    com_info_t *info     = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    com_pic_t *pic_org   = core->pic_org;
    int x                = core->cu_pix_x;
    int y                = core->cu_pix_y;
    int log2_cuw         = core->cu_width_log2;
    int log2_cuh         = core->cu_height_log2;
    int cu_width         = 1 << log2_cuw;
    int cu_height        = 1 << log2_cuh;

    s8           mrg_list_refi[AFF_MAX_NUM_MRG][REFP_NUM];
    int          mrg_list_cp_num[AFF_MAX_NUM_MRG];
    CPMV         mrg_list_cp_mv[AFF_MAX_NUM_MRG][REFP_NUM][VER_NUM][MV_D];
    double       cost = MAX_D_COST;
    int          mrg_idx, num_cands = 0;

    pel*y_org = pic_org->y + x + y * pic_org->stride_luma;
    pel*u_org = pic_org->u + (x >> 1) + ((y >> 1) * pic_org->stride_chroma);
    pel*v_org = pic_org->v + (x >> 1) + ((y >> 1) * pic_org->stride_chroma);

    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);
    init_inter_data(core);

    cur_info->mvr_idx = 0;
    cur_info->affine_flag = 1;

    com_map_t *map = core->map;
    num_cands = com_get_affine_merge_candidate(core->ptr, core->cu_scup_in_pic, map->map_refi, map->map_mv, map->map_scu, map->map_pos, core->refp, cu_width, cu_height, info->i_scu, info->pic_width_in_scu, info->pic_height_in_scu, core->pichdr->slice_type, mrg_list_refi, mrg_list_cp_mv, mrg_list_cp_num, info->log2_max_cuwh);

    if (num_cands == 0) {
        return;
    }

    for (mrg_idx = 0; mrg_idx < num_cands; mrg_idx++) {
        int memory_access[REFP_NUM];
        int allowed = 1;
        int i;
        for (i = 0; i < REFP_NUM; i++) {
            if (REFI_IS_VALID(mrg_list_refi[mrg_idx][i])) {
                if (mrg_list_cp_num[mrg_idx] == 3) { // derive RB
                    mrg_list_cp_mv[mrg_idx][i][3][MV_X] = mrg_list_cp_mv[mrg_idx][i][1][MV_X] + mrg_list_cp_mv[mrg_idx][i][2][MV_X] - mrg_list_cp_mv[mrg_idx][i][0][MV_X];
                    mrg_list_cp_mv[mrg_idx][i][3][MV_Y] = mrg_list_cp_mv[mrg_idx][i][1][MV_Y] + mrg_list_cp_mv[mrg_idx][i][2][MV_Y] - mrg_list_cp_mv[mrg_idx][i][0][MV_Y];
                } else { // derive LB, RB
                    mrg_list_cp_mv[mrg_idx][i][2][MV_X] = mrg_list_cp_mv[mrg_idx][i][0][MV_X] - (mrg_list_cp_mv[mrg_idx][i][1][MV_Y] - mrg_list_cp_mv[mrg_idx][i][0][MV_Y]) * (s16)cu_height / (s16)cu_width;
                    mrg_list_cp_mv[mrg_idx][i][2][MV_Y] = mrg_list_cp_mv[mrg_idx][i][0][MV_Y] + (mrg_list_cp_mv[mrg_idx][i][1][MV_X] - mrg_list_cp_mv[mrg_idx][i][0][MV_X]) * (s16)cu_height / (s16)cu_width;
                    mrg_list_cp_mv[mrg_idx][i][3][MV_X] = mrg_list_cp_mv[mrg_idx][i][1][MV_X] - (mrg_list_cp_mv[mrg_idx][i][1][MV_Y] - mrg_list_cp_mv[mrg_idx][i][0][MV_Y]) * (s16)cu_height / (s16)cu_width;
                    mrg_list_cp_mv[mrg_idx][i][3][MV_Y] = mrg_list_cp_mv[mrg_idx][i][1][MV_Y] + (mrg_list_cp_mv[mrg_idx][i][1][MV_X] - mrg_list_cp_mv[mrg_idx][i][0][MV_X]) * (s16)cu_height / (s16)cu_width;
                }
                memory_access[i] = com_get_affine_memory_access(mrg_list_cp_mv[mrg_idx][i], cu_width, cu_height);
            }
        }

        if (REFI_IS_VALID(mrg_list_refi[mrg_idx][0]) && REFI_IS_VALID(mrg_list_refi[mrg_idx][1])) {
            int mem = MAX_MEMORY_ACCESS_BI * cu_width * cu_height;
            if (memory_access[0] > mem || memory_access[1] > mem) {
                allowed = 0;
            }
        } else {
            int valid_idx = REFI_IS_VALID(mrg_list_refi[mrg_idx][0]) ? 0 : 1;
            int mem = MAX_MEMORY_ACCESS_UNI * cu_width * cu_height;
            if (memory_access[valid_idx] > mem) {
                allowed = 0;
            }
        }
        if (!allowed) {
            continue;
        }

        cur_info->umve_flag = 0;
        cur_info->affine_flag = (u8)mrg_list_cp_num[mrg_idx] - 1;
        cur_info->skip_idx = (u8)mrg_idx;

        for (int ver = 0; ver < mrg_list_cp_num[mrg_idx]; ver++) {
            CP64(cur_info->affine_mv[REFP_0][ver], mrg_list_cp_mv[mrg_idx][REFP_0][ver]);
            CP64(cur_info->affine_mv[REFP_1][ver], mrg_list_cp_mv[mrg_idx][REFP_1][ver]);
        }
        CP16(cur_info->refi, mrg_list_refi[mrg_idx]);

        s64 dist[2][N_C] = { 0 };
        s64 dist_pred[N_C] = { 0 };

        cur_info->cu_mode = MODE_DIR;
        cost = inter_rdcost(core, lbac_best, 0, 1, dist, dist_pred);

        cur_info->cu_mode = MODE_SKIP;
        cost = inter_rdcost(core, lbac_best, 1, 0, dist, dist_pred);
    }
}

static void analyze_uni_pred(core_t *core, lbac_t *lbac_best, double *cost_L0L1, s16 mv_L0L1[REFP_NUM][MV_D], s8 *refi_L0L1)
{
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi  = &core->pinter;
    int x                = core->cu_pix_x;
    int y                = core->cu_pix_y;
    int cu_width_log2    = core->cu_width_log2;
    int cu_height_log2   = core->cu_height_log2;
    int cu_width         = 1 << cu_width_log2;
    int cu_height        = 1 << cu_height_log2;
    s8 best_refi = 0;

    pi->i_org   = core->pic_org->stride_luma;
    pi->org     = core->pic_org->y + y * pi->i_org + x;
    pi->fast_me = core->param->speed_level;

    cur_info->cu_mode = MODE_INTER;

    for (int lidx = 0; lidx <= ((core->slice_type == SLICE_P) ? PRED_L0 : PRED_L1); lidx++) {
        u64 best_mecost = COM_UINT64_MAX;

        init_inter_data(core);
        pi->num_refp = (u8)core->num_refp[lidx];

        for (int refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            s16 *mvp = pi->mvp_scale[lidx][refi_cur];
            s16 *mv  = pi->mv_scale [lidx][refi_cur];

            com_derive_mvp(core->info, core->ptr, core->cu_scup_in_pic, lidx, refi_cur, cur_info->hmvp_flag, core->cnt_hmvp_cands,
                           core->motion_cands, core->map, core->refp, cur_info->mvr_idx, cu_width, cu_height, mvp);

            pi->ref_pic = core->refp[refi_cur][lidx].pic;
            u64 mecost = me_search_tz(pi, x, y, cu_width, cu_height, core->info->pic_width, core->info->pic_height, refi_cur, lidx, mvp, mv, 0);

            if (mecost < best_mecost) {
                best_mecost = mecost;
                best_refi = refi_cur;
            }
            if (cur_info->mvr_idx < MAX_NUM_AFFINE_MVR) {
                pi->best_mv_uni[lidx][refi_cur][MV_X] = mv[MV_X];
                pi->best_mv_uni[lidx][refi_cur][MV_Y] = mv[MV_Y];
            }
        }

        s16 *mv  = cur_info->mv [lidx];
        s16 *mvd = cur_info->mvd[lidx];
        s16 *mvp = pi->mvp_scale[lidx][best_refi];
        s8 t0 = (lidx == 0) ? best_refi : REFI_INVALID;
        s8 t1 = (lidx == 1) ? best_refi : REFI_INVALID;

        SET_REFI(cur_info->refi, t0, t1);

        M32(mv_L0L1[lidx]) = M32(mv) = M32(pi->mv_scale[lidx][best_refi]);
        mvd[MV_X] = mv[MV_X] - mvp[MV_X];
        mvd[MV_Y] = mv[MV_Y] - mvp[MV_Y];

        pi->mot_bits[lidx] = get_mv_bits(pi, mvd[MV_X], mvd[MV_Y], pi->num_refp, best_refi, cur_info->mvr_idx);

        refi_L0L1[lidx] = best_refi;
        cost_L0L1[lidx] = inter_rdcost(core, lbac_best, 0, 1, NULL, NULL);
    }
}

static void analyze_bi(core_t *core, lbac_t *lbac_best, s16 mv_L0L1[REFP_NUM][MV_D], const s8 *refi_L0L1, const double *cost_L0L1)
{
    com_pic_t *pic_org   = core->pic_org;
    com_info_t *info     = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi   = &core->pinter;
    int x                = core->cu_pix_x;
    int y                = core->cu_pix_y;
    int cu_width_log2    = core->cu_width_log2;
    int cu_height_log2   = core->cu_height_log2;
    int cu_width         = 1 << cu_width_log2;
    int cu_height        = 1 << cu_height_log2;

    int bit_depth   = info->bit_depth_internal;
    u64 best_mecost = COM_UINT64_MAX;
    s8  lidx_ref, lidx_cnd;

    cur_info->cu_mode = MODE_INTER;
    init_inter_data(core);

    if (cost_L0L1[PRED_L0] <= cost_L0L1[PRED_L1]) {
        lidx_ref = REFP_0;
        lidx_cnd = REFP_1;
    } else {
        lidx_ref = REFP_1;
        lidx_cnd = REFP_0;
    }

    s8 t0 = (lidx_ref == REFP_0) ? refi_L0L1[lidx_ref] : REFI_INVALID;
    s8 t1 = (lidx_ref == REFP_1) ? refi_L0L1[lidx_ref] : REFI_INVALID;
    s8 refi[REFP_NUM] = { t0, t1 };

    CP16(cur_info->refi, refi_L0L1);
    CP32(cur_info->mv[lidx_ref], mv_L0L1[lidx_ref]);
    CP32(cur_info->mv[lidx_cnd], mv_L0L1[lidx_cnd]);

    pel *org = pic_org->y + x + y * pic_org->stride_luma;
    pel(*pred)[MAX_CU_DIM] = cur_info->pred;
    s8  refi_best = 0;
    int changed = 0;

    for (int i = 0; i < 2; i++) {
        ALIGNED_32(pel org_bi[MAX_CU_DIM]);
        pi->i_org = cu_width;
        pi->org = org_bi;
        pi->fast_me = 0;

        com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi, cur_info->mv, core->refp, pred, cu_width, CHANNEL_L, bit_depth);
        create_bi_org(org, pred[0], pic_org->stride_luma, cu_width, cu_height, org_bi, cu_width, info->bit_depth_internal);

        SWAP(refi[lidx_ref], refi[lidx_cnd], t0);
        SWAP(lidx_ref, lidx_cnd, t0);
        changed = 0;
        pi->num_refp = (u8)core->num_refp[lidx_ref];

        for (int refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            pi->ref_pic = core->refp[refi_cur][lidx_ref].pic;
            u64 mecost = me_search_tz(pi, x, y, cu_width, cu_height, core->info->pic_width, core->info->pic_height, refi_cur, lidx_ref, pi->mvp_scale[lidx_ref][refi_cur], pi->mv_scale[lidx_ref][refi_cur], 1);
      
            if (mecost < best_mecost) {
                refi_best = refi_cur;
                best_mecost = mecost;
                changed = 1;
                t0 = (lidx_ref == REFP_0) ? refi_best : cur_info->refi[lidx_cnd];
                t1 = (lidx_ref == REFP_1) ? refi_best : cur_info->refi[lidx_cnd];
                SET_REFI(cur_info->refi, t0, t1);
                cur_info->mv[lidx_ref][MV_X] = pi->mv_scale[lidx_ref][refi_cur][MV_X];
                cur_info->mv[lidx_ref][MV_Y] = pi->mv_scale[lidx_ref][refi_cur][MV_Y];

                pi->mot_bits[lidx_ref] = get_mv_bits(pi, cur_info->mv[lidx_ref][MV_X] - pi->mvp_scale[lidx_ref][refi_cur][MV_X], cur_info->mv[lidx_ref][MV_Y] - pi->mvp_scale[lidx_ref][refi_cur][MV_Y], pi->num_refp, refi_cur, cur_info->mvr_idx);
            }
        }
        t0 = (lidx_ref == REFP_0) ? refi_best : REFI_INVALID;
        t1 = (lidx_ref == REFP_1) ? refi_best : REFI_INVALID;
        SET_REFI(refi, t0, t1);
        if (!changed) {
            break;
        }
    }
    cur_info->mvd[REFP_0][MV_X] = cur_info->mv[REFP_0][MV_X] - pi->mvp_scale[REFP_0][cur_info->refi[REFP_0]][MV_X];
    cur_info->mvd[REFP_0][MV_Y] = cur_info->mv[REFP_0][MV_Y] - pi->mvp_scale[REFP_0][cur_info->refi[REFP_0]][MV_Y];
    cur_info->mvd[REFP_1][MV_X] = cur_info->mv[REFP_1][MV_X] - pi->mvp_scale[REFP_1][cur_info->refi[REFP_1]][MV_X];
    cur_info->mvd[REFP_1][MV_Y] = cur_info->mv[REFP_1][MV_Y] - pi->mvp_scale[REFP_1][cur_info->refi[REFP_1]][MV_Y];

    inter_rdcost(core, lbac_best, 0, 1, NULL, NULL);
}

static u32 smvd_refine(core_t *core, int x, int y, int log2_cuw, int log2_cuh, s16 mv[REFP_NUM][MV_D], s16 mvp[REFP_NUM][MV_D], s8 refi[REFP_NUM], s32 lidx_cur, s32 lidx_tar, u32 mecost, s32 search_pattern, s32 search_round, s32 search_shift)
{
    com_info_t *info     = core->info;
    com_pic_t *pic_org   = core->pic_org;
    inter_search_t *pi   = &core->pinter;
    com_mode_t *cur_info = &core->mod_info_curr;
    int bit_depth = info->bit_depth_internal;
    int cu_width  = (1 << log2_cuw);
    int cu_height = (1 << log2_cuh);

    static const s32 search_offset_cross  [4][MV_D] = { {  0, 1 }, { 1, 0 }, {  0, -1 }, { -1,  0 } };
    static const s32 search_offset_square [8][MV_D] = { { -1, 1 }, { 0, 1 }, {  1,  1 }, {  1,  0 }, {  1, -1 }, {  0, -1 }, { -1, -1 }, { -1, 0 } };
    static const s32 search_offset_diamond[8][MV_D] = { {  0, 2 }, { 1, 1 }, {  2,  0 }, {  1, -1 }, {  0, -2 }, { -1, -1 }, { -2,  0 }, { -1, 1 } };
    static const s32 search_offset_hexagon[6][MV_D] = { {  2, 0 }, { 1, 2 }, { -1,  2 }, { -2,  0 }, { -1, -2 }, {  1, -2 } };
    const s32(*search_offset)[MV_D];

    s32 direct_start = 0;
    s32 direct_end;
    s32 direct_rounding = 0;
    s32 direct_mask = 0;

    int ref_mvr_bits = tbl_refi_bits[0][cur_info->refi[lidx_cur]] + pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1);
    u8 *tab_mvbits_x = pi->tab_mvbits;
    u8 *tab_mvbits_y = pi->tab_mvbits;
    u32 lambda_mv    = pi->lambda_mv;

    int mvr_idx = cur_info->mvr_idx;
    pel *org = pic_org->y + x + y * pic_org->stride_luma;

    if (search_pattern == 0) {
        direct_end      = 3;
        direct_rounding = 4;
        direct_mask     = 0x03;
        search_offset   = search_offset_cross;
    } else if (search_pattern == 1) {
        direct_end      = 7;
        direct_rounding = 8;
        direct_mask     = 0x07;
        search_offset   = search_offset_square;
    } else if (search_pattern == 2) {
        direct_end      = 7;
        direct_rounding = 8;
        direct_mask     = 0x07;
        search_offset   = search_offset_diamond;
    } else if (search_pattern == 3) {
        direct_end      = 5;
        search_offset   = search_offset_hexagon;
    } else {
        search_offset   = search_offset_diamond;
        com_assert(0);
    }

    s32 step = 1;

    for (int round = 0; round < search_round; round++) {
        s16 mv_cur_center[MV_D];
        s32 index;
        int best_direct = -1;

        mv_cur_center[MV_X] = mv[lidx_cur][MV_X];
        mv_cur_center[MV_Y] = mv[lidx_cur][MV_Y];

        for (index = direct_start; index <= direct_end; index++) {
            s32 direct;
            u32 mecost_tmp;
            s16 mv_cand[REFP_NUM][MV_D], mvd_cand[REFP_NUM][MV_D];

            if (search_pattern == 3) {
                direct = index < 0 ? index + 6 : index >= 6 ? index - 6 : index;
            } else {
                direct = (index + direct_rounding) & direct_mask;
            }
            mv_cand [lidx_cur][MV_X] = mv_cur_center[MV_X] + (s16)(search_offset[direct][MV_X] << search_shift);
            mv_cand [lidx_cur][MV_Y] = mv_cur_center[MV_Y] + (s16)(search_offset[direct][MV_Y] << search_shift);
            mvd_cand[lidx_cur][MV_X] = mv_cand[lidx_cur][MV_X] - mvp[lidx_cur][MV_X];
            mvd_cand[lidx_cur][MV_Y] = mv_cand[lidx_cur][MV_Y] - mvp[lidx_cur][MV_Y];

            mv_cand[lidx_tar][MV_X] = mvp[lidx_tar][MV_X] - mvd_cand[lidx_cur][MV_X];
            mv_cand[lidx_tar][MV_Y] = mvp[lidx_tar][MV_Y] - mvd_cand[lidx_cur][MV_Y];

            com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, cur_info->refi, mv_cand, core->refp, cur_info->pred, cu_width, CHANNEL_L, bit_depth);

            int mv_bits = ref_mvr_bits + GET_MVBITS_X(mvd_cand[lidx_cur][MV_X]) + GET_MVBITS_Y(mvd_cand[lidx_cur][MV_Y]);

            mecost_tmp = com_had(cu_width, cu_height, org, pic_org->stride_luma, cur_info->pred[0], cu_width, bit_depth);
            mecost_tmp += MV_COST(mv_bits);

            if (mecost_tmp < mecost) {
                mecost = mecost_tmp;
                CP32(mv[lidx_cur], mv_cand[lidx_cur]);
                CP32(mv[lidx_tar], mv_cand[lidx_tar]);
                best_direct = direct;
            }
        }
        if (best_direct == -1) {
            break;
        }
        step = 1;
        if ((search_pattern == 1) || (search_pattern == 2)) {
            step = 2 - (best_direct & 0x01);
        }
        direct_start = best_direct - step;
        direct_end   = best_direct + step;
    }

    return mecost;
};

static void analyze_smvd(core_t *core, lbac_t *lbac_best)
{
    com_info_t *info     = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi   = &core->pinter;
    int x                = core->cu_pix_x;
    int y                = core->cu_pix_y;
    int log2_cuw         = core->cu_width_log2;
    int log2_cuh         = core->cu_height_log2;
    com_pic_t *pic_org   = core->pic_org;
    int cu_width         = 1 << log2_cuw;
    int cu_height        = 1 << log2_cuh;

    pel *org             = pic_org->y + x + y * pic_org->stride_luma;
    u32 lambda_mv        = pi->lambda_mv;
    int bit_depth        = info->bit_depth_internal;

    s16 mv[REFP_NUM][MV_D], mvp[REFP_NUM][MV_D], mvd[MV_D];

    init_inter_data(core);

    cur_info->cu_mode      = MODE_INTER;
    cur_info->smvd_flag    = 1;
    cur_info->refi[REFP_0] = 0;
    cur_info->refi[REFP_1] = 0;

    M32(mv[0]) = M32(mvp[REFP_0]) = M32(pi->mvp_scale[REFP_0][0]);
    M32(mv[1]) = M32(mvp[REFP_1]) = M32(pi->mvp_scale[REFP_1][0]);

    com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, cur_info->refi, mv, core->refp, cur_info->pred, cu_width, CHANNEL_L, bit_depth);

    u32 mecost = com_had(cu_width, cu_height, org, pic_org->stride_luma, cur_info->pred[0], cu_width, bit_depth);
    mecost += MV_COST(get_mv_bits_with_mvr(pi, 0, 0, cur_info->mvr_idx));

    s16 mv_bi[REFP_NUM][MV_D];
    CP32(mv_bi[REFP_0], pi->mv_scale[REFP_0][0]);
    mvd[MV_X] = mv_bi[REFP_0][MV_X] - mvp[REFP_0][MV_X];
    mvd[MV_Y] = mv_bi[REFP_0][MV_Y] - mvp[REFP_0][MV_Y];
    mv_bi[REFP_1][MV_X] = mvp[REFP_1][MV_X] - mvd[MV_X];
    mv_bi[REFP_1][MV_Y] = mvp[REFP_1][MV_Y] - mvd[MV_Y];

    com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, cur_info->refi, mv_bi, core->refp, cur_info->pred, cu_width, CHANNEL_L, bit_depth);

    u32 mecost_bi = com_had(cu_width, cu_height, org, pic_org->stride_luma, cur_info->pred[0], cu_width, bit_depth);
    mecost_bi += MV_COST(get_mv_bits_with_mvr(pi, mvd[MV_X], mvd[MV_Y], cur_info->mvr_idx));

    if (mecost_bi < mecost) {
        mecost = mecost_bi;
        CP32(mv[0], mv_bi[0]);
        CP32(mv[1], mv_bi[1]);
    }

    // refine
    mecost = smvd_refine(core, x, y, log2_cuw, log2_cuh, mv, mvp, cur_info->refi, 0, 1, mecost, 2, 8, cur_info->mvr_idx);
    mecost = smvd_refine(core, x, y, log2_cuw, log2_cuh, mv, mvp, cur_info->refi, 0, 1, mecost, 0, 1, cur_info->mvr_idx);

    CP32(cur_info->mv[REFP_0], mv[REFP_0]);
    CP32(cur_info->mv[REFP_1], mv[REFP_1]);

    cur_info->mvd[REFP_0][MV_X] = mv[REFP_0][MV_X] - mvp[REFP_0][MV_X];
    cur_info->mvd[REFP_0][MV_Y] = mv[REFP_0][MV_Y] - mvp[REFP_0][MV_Y];
    cur_info->mvd[REFP_1][MV_X] = mv[REFP_1][MV_X] - mvp[REFP_1][MV_X];
    cur_info->mvd[REFP_1][MV_Y] = mv[REFP_1][MV_Y] - mvp[REFP_1][MV_Y];

    cur_info->mvd[REFP_1][MV_X] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, -cur_info->mvd[REFP_0][MV_X]);
    cur_info->mvd[REFP_1][MV_Y] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, -cur_info->mvd[REFP_0][MV_Y]);

    inter_rdcost(core, lbac_best, 0, 1, NULL, NULL);
}

static void solve_equal(double(*equal_coeff)[5], int order, double *affine_para)
{
    int i, j, k;
  
    for (i = 1; i < order; i++) {

        double temp = fabs(equal_coeff[i][i - 1]);
        int temp_idx = i;
        for (j = i + 1; j < order + 1; j++) {
            if (fabs(equal_coeff[j][i - 1]) > temp) {
                temp = fabs(equal_coeff[j][i - 1]);
                temp_idx = j;
            }
        }
        if (temp_idx != i) {
            for (j = 0; j < order + 1; j++) {
                equal_coeff[0][j] = equal_coeff[i][j];
                equal_coeff[i][j] = equal_coeff[temp_idx][j];
                equal_coeff[temp_idx][j] = equal_coeff[0][j];
            }
        }
        for (j = i + 1; j < order + 1; j++) {
            for (k = i; k < order + 1; k++) {
                equal_coeff[j][k] = equal_coeff[j][k] - equal_coeff[i][k] * equal_coeff[j][i - 1] / equal_coeff[i][i - 1];
            }
        }
    }
    affine_para[order - 1] = equal_coeff[order][order] / equal_coeff[order][order - 1];
    for (i = order - 2; i >= 0; i--) {
        double temp = 0;
        for (j = i + 1; j < order; j++) {
            temp += equal_coeff[i + 1][j] * affine_para[j];
        }
        affine_para[i] = (equal_coeff[i + 1][order] - temp) / equal_coeff[i + 1][i];
    }
}

static int affine_mv_bits(CPMV mv[VER_NUM][MV_D], CPMV mvp[VER_NUM][MV_D], int num_refp, int refi, u8 curr_mvr)
{
    int bits = tbl_refi_bits[num_refp][refi];
    u8  amvr_shift = Tab_Affine_AMVR(curr_mvr);

    for (int vertex = 0; vertex < 2; vertex++) {
        int mvd_x = mv[vertex][MV_X] - mvp[vertex][MV_X];
        int mvd_y = mv[vertex][MV_Y] - mvp[vertex][MV_Y];

        if (mv[vertex][MV_X] != COM_CPMV_MAX && mvp[vertex][MV_X] != COM_CPMV_MAX) {
            assert(mvd_x == ((mvd_x >> amvr_shift) << amvr_shift));
        }
        if (mv[vertex][MV_Y] != COM_CPMV_MAX && mvp[vertex][MV_Y] != COM_CPMV_MAX) {
            assert(mvd_y == ((mvd_y >> amvr_shift) << amvr_shift));
        }
        int dx = COM_ABS(mvd_x >> amvr_shift);
        int dy = COM_ABS(mvd_y >> amvr_shift);
        bits += ((uavs3e_get_log2(dx + 1) + uavs3e_get_log2(dy + 1)) << 1) + 2 + !!dx + !!dy;
    }
    return bits;
}

static u64 affine_me_gradient(inter_search_t *pi, int x, int y, int cu_width_log2, int cu_height_log2, s8 *refi, int lidx, CPMV mvp[VER_NUM][MV_D], CPMV mv[VER_NUM][MV_D], int bi, int sub_w, int sub_h)
{
    int bit_depth = pi->bit_depth;
    CPMV mvt[VER_NUM][MV_D];
    s16  mvd[VER_NUM][MV_D] = { 0 };
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    u64 cost, cost_best = COM_UINT64_MAX;
    s8 ri = *refi;
    com_pic_t *refp = pi->ref_pic;
    ALIGNED_32(pel pred[MAX_CU_DIM]);
    pel *org = pi->org;
    int s_org = pi->i_org;
    int mv_bits, best_bits;
    int vertex, iter;
    int iter_num = bi ? AF_ITER_BI : AF_ITER_UNI;
    const int para_num = (2 << 1) + 1;
    const int affine_param_num = para_num - 1;
    double affine_para[6];
    double delta_mv[6];
    s64    equal_coeff_t[5][5];
    double equal_coeff  [5][5];
    ALIGNED_32(s16 error[MAX_CU_DIM]);
    ALIGNED_32(s16 derivate[2][MAX_CU_DIM]);
    u32 lambda_mv = pi->lambda_mv;

    cu_width  = 1 << cu_width_log2;
    cu_height = 1 << cu_height_log2;

    CP128(mvt, mv);

    com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, mvt, refp, pred, sub_w, sub_h, bit_depth);
    best_bits = affine_mv_bits(mvt, mvp, pi->num_refp, ri, pi->curr_mvr);

    if (bi) {
        best_bits += pi->mot_bits[1 - lidx];
    }
    cost_best = MV_COST(best_bits);
    cost_best += com_had(cu_width, cu_height, org, s_org, pred, cu_width, bit_depth) >> bi;

    for (iter = 0; iter < iter_num; iter++) {
        block_pel_sub(cu_width_log2, cu_height_log2, org, pred, s_org, cu_width, cu_width, error);
        // sobel x direction
        // -1 0 1
        // -2 0 2
        // -1 0 1
        uavs3e_funs_handle.affine_sobel_flt_hor(pred, cu_width, derivate[0], cu_width, cu_width, cu_height);
        // sobel y direction
        // -1 -2 -1
        //  0  0  0
        //  1  2  1
        uavs3e_funs_handle.affine_sobel_flt_ver(pred, cu_width, derivate[1], cu_width, cu_width, cu_height);

        // solve delta x and y
        com_mset(equal_coeff_t, 0, para_num * para_num * sizeof(s64));
        uavs3e_funs_handle.affine_coef_computer(error, cu_width, derivate, cu_width, equal_coeff_t, cu_width, cu_height);

        for (int row = 0; row < para_num; row++) {
            equal_coeff[row][0] = (double)equal_coeff_t[row][0];
            equal_coeff[row][1] = (double)equal_coeff_t[row][1];
            equal_coeff[row][2] = (double)equal_coeff_t[row][2];
            equal_coeff[row][3] = (double)equal_coeff_t[row][3];
            equal_coeff[row][4] = (double)equal_coeff_t[row][4];
        }
        solve_equal(equal_coeff, affine_param_num, affine_para);
        // convert to delta mv

        delta_mv[0] = affine_para[0];
        delta_mv[2] = affine_para[2];
        delta_mv[1] = affine_para[1] * cu_width + affine_para[0];
        delta_mv[3] = -affine_para[3] * cu_width + affine_para[2];

        u8 amvr_shift = Tab_Affine_AMVR(pi->curr_mvr);
        if (amvr_shift == 0) {
            mvd[0][MV_X] = (s16)(delta_mv[0] * 16 + (delta_mv[0] >= 0 ? 0.5 : -0.5));
            mvd[0][MV_Y] = (s16)(delta_mv[2] * 16 + (delta_mv[2] >= 0 ? 0.5 : -0.5));
            mvd[1][MV_X] = (s16)(delta_mv[1] * 16 + (delta_mv[1] >= 0 ? 0.5 : -0.5));
            mvd[1][MV_Y] = (s16)(delta_mv[3] * 16 + (delta_mv[3] >= 0 ? 0.5 : -0.5));
        } else {
            mvd[0][MV_X] = (s16)(delta_mv[0] * 4 + (delta_mv[0] >= 0 ? 0.5 : -0.5));
            mvd[0][MV_Y] = (s16)(delta_mv[2] * 4 + (delta_mv[2] >= 0 ? 0.5 : -0.5));
            mvd[1][MV_X] = (s16)(delta_mv[1] * 4 + (delta_mv[1] >= 0 ? 0.5 : -0.5));
            mvd[1][MV_Y] = (s16)(delta_mv[3] * 4 + (delta_mv[3] >= 0 ? 0.5 : -0.5));
            mvd[0][MV_X] <<= 2;//  1/16-pixel
            mvd[0][MV_Y] <<= 2;
            mvd[1][MV_X] <<= 2;
            mvd[1][MV_Y] <<= 2;

            if (amvr_shift > 0) {
                com_mv_rounding_s16(mvd[0][MV_X], mvd[0][MV_Y], &mvd[0][MV_X], &mvd[0][MV_Y], amvr_shift);
                com_mv_rounding_s16(mvd[1][MV_X], mvd[1][MV_Y], &mvd[1][MV_X], &mvd[1][MV_Y], amvr_shift);
            }
        }

        // check early terminate
        if (!M64(mvd)) {
            break;
        }

        /* update mv */
        for (vertex = 0; vertex < 2; vertex++) {
            s32 mvx = (s32)mvt[vertex][MV_X] + (s32)mvd[vertex][MV_X];
            s32 mvy = (s32)mvt[vertex][MV_Y] + (s32)mvd[vertex][MV_Y];
            mvt[vertex][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mvx);
            mvt[vertex][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mvy);

            // after clipping, last 2/4 bits of mv may not be zero when amvr_shift is 2/4, perform rounding without offset
            u8 amvr_shift = Tab_Affine_AMVR(pi->curr_mvr);
            if (mvt[vertex][MV_X] == COM_CPMV_MAX) {
                mvt[vertex][MV_X] = mvt[vertex][MV_X] >> amvr_shift << amvr_shift;
            }
            if (mvt[vertex][MV_Y] == COM_CPMV_MAX) {
                mvt[vertex][MV_Y] = mvt[vertex][MV_Y] >> amvr_shift << amvr_shift;
            }
        }
        com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, mvt, refp, pred, sub_w, sub_h, bit_depth);

        mv_bits = affine_mv_bits(mvt, mvp, pi->num_refp, ri, pi->curr_mvr);
        if (bi) {
            mv_bits += pi->mot_bits[1 - lidx];
        }
        cost = MV_COST(mv_bits);
        cost += com_had(cu_width, cu_height, org, s_org, pred, cu_width, bit_depth) >> bi;

        if (cost < cost_best) {
            cost_best = cost;
            best_bits = mv_bits;
            CP128(mv, mvt);
        }
    }
    return (cost_best - MV_COST(best_bits));
}

static void mv_clip(int x, int y, int pic_w, int pic_h, int w, int h, s8 refi[REFP_NUM], s16 mv[REFP_NUM][MV_D], s16(*mv_t)[MV_D])
{
    int min_clip[MV_D], max_clip[MV_D];
    x <<= 2;
    y <<= 2;
    w <<= 2;
    h <<= 2;
    min_clip[MV_X] = (-MAX_CU_SIZE - 4) << 2;
    min_clip[MV_Y] = (-MAX_CU_SIZE - 4) << 2;
    max_clip[MV_X] = (pic_w - 1 + MAX_CU_SIZE + 4) << 2;
    max_clip[MV_Y] = (pic_h - 1 + MAX_CU_SIZE + 4) << 2;
    mv_t[REFP_0][MV_X] = mv[REFP_0][MV_X];
    mv_t[REFP_0][MV_Y] = mv[REFP_0][MV_Y];
    mv_t[REFP_1][MV_X] = mv[REFP_1][MV_X];
    mv_t[REFP_1][MV_Y] = mv[REFP_1][MV_Y];

    if (REFI_IS_VALID(refi[REFP_0])) {
        if (x + mv[REFP_0][MV_X] < min_clip[MV_X]) {
            mv_t[REFP_0][MV_X] = (s16)(min_clip[MV_X] - x);
        }
        if (y + mv[REFP_0][MV_Y] < min_clip[MV_Y]) {
            mv_t[REFP_0][MV_Y] = (s16)(min_clip[MV_Y] - y);
        }
        if (x + mv[REFP_0][MV_X] + w - 4 > max_clip[MV_X]) {
            mv_t[REFP_0][MV_X] = (s16)(max_clip[MV_X] - x - w + 4);
        }
        if (y + mv[REFP_0][MV_Y] + h - 4 > max_clip[MV_Y]) {
            mv_t[REFP_0][MV_Y] = (s16)(max_clip[MV_Y] - y - h + 4);
        }
    }
    if (REFI_IS_VALID(refi[REFP_1])) {
        if (x + mv[REFP_1][MV_X] < min_clip[MV_X]) {
            mv_t[REFP_1][MV_X] = (s16)(min_clip[MV_X] - x);
        }
        if (y + mv[REFP_1][MV_Y] < min_clip[MV_Y]) {
            mv_t[REFP_1][MV_Y] = (s16)(min_clip[MV_Y] - y);
        }
        if (x + mv[REFP_1][MV_X] + w - 4 > max_clip[MV_X]) {
            mv_t[REFP_1][MV_X] = (s16)(max_clip[MV_X] - x - w + 4);
        }
        if (y + mv[REFP_1][MV_Y] + h - 4 > max_clip[MV_Y]) {
            mv_t[REFP_1][MV_Y] = (s16)(max_clip[MV_Y] - y - h + 4);
        }
    }
}

static void analyze_affine_uni(core_t *core, lbac_t *lbac_best, CPMV aff_mv_L0L1[REFP_NUM][VER_NUM][MV_D], s8 *refi_L0L1, double *cost_L0L1)
{
    com_info_t *info     =  core->info;
    com_map_t *map       =  core->map;
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi   = &core->pinter;
    int x                =  core->cu_pix_x;
    int y                =  core->cu_pix_y;
    int cu_width_log2    =  core->cu_width_log2;
    int cu_height_log2   =  core->cu_height_log2;
    com_pic_t *pic_org   =  core->pic_org;
    pel *org             = pic_org->y + x + y * pic_org->stride_luma;
    int s_org            = pic_org->stride_luma;
    int cu_width         = 1 << cu_width_log2;
    int cu_height        = 1 << cu_height_log2;
    s8* refi             = cur_info->refi;
    int bit_depth        = info->bit_depth_internal;
    const int mem        = MAX_MEMORY_ACCESS_UNI * (1 << cu_width_log2) * (1 << cu_height_log2);
    u32 lambda_mv        = pi->lambda_mv;
    int sub_w            = 4;
    int sub_h            = 4;

    u64 mecost, best_mecost;
    int mebits, best_bits;

    if (core->pichdr->affine_subblk_size_idx > 0) {
        sub_w = 8;
        sub_h = 8;
    }

    /* AFFINE 4 parameters Motion Search */
    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);
    cur_info->cu_mode = MODE_INTER;
    cur_info->affine_flag = 1;

    pi->org   = org;
    pi->i_org = s_org;

    for (int lidx = 0; lidx <= ((core->slice_type == SLICE_P) ? PRED_L0 : PRED_L1); lidx++) {
        init_inter_data(core);

        int refi_bst;

        CPMV (*affine_mv)[MV_D] = cur_info->affine_mv [lidx];
        s16 (*affine_mvd)[MV_D] = cur_info->affine_mvd[lidx];
        CPMV(*affine_mvp)[MV_D];

        pi->num_refp = (u8)core->num_refp[lidx];
        best_mecost = COM_UINT64_MAX;

        for (s8 refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            affine_mvp = pi->affine_mvp_scale[lidx][refi_cur];
            com_get_affine_mvp_scaling(core->ptr, core->cu_scup_in_pic, lidx, refi_cur, map->map_mv, map->map_refi, core->refp,
                                        core->cu_width, core->cu_height, info->i_scu, affine_mvp, map->map_scu, map->map_pos, cur_info->mvr_idx);
            s8  refi_t[REFP_NUM];
            com_pic_t *refp = core->refp[refi_cur][lidx].pic;
            ALIGNED_32(pel pred[MAX_CU_DIM]);

            com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, affine_mvp, refp, pred, sub_w, sub_h, bit_depth);
            
            u64 mvp_best = com_had(cu_width, cu_height, org, s_org, pred, cu_width, bit_depth);
            mebits = affine_mv_bits(affine_mvp, affine_mvp, pi->num_refp, refi_cur, cur_info->mvr_idx);
            mvp_best += MV_COST(mebits);

            s16 mv_cliped[REFP_NUM][MV_D];
            mv_cliped[lidx][MV_X] = (s16)(pi->best_mv_uni[lidx][refi_cur][MV_X]);
            mv_cliped[lidx][MV_Y] = (s16)(pi->best_mv_uni[lidx][refi_cur][MV_Y]);

            refi_t[lidx] = refi_cur;
            refi_t[1 - lidx] = -1;
            mv_clip(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi_t, mv_cliped, mv_cliped);

            CPMV tmp_mv_array[VER_NUM][MV_D];

            for (int vertex = 0; vertex < 2; vertex++) {
                tmp_mv_array[vertex][MV_X] = mv_cliped[lidx][MV_X];
                tmp_mv_array[vertex][MV_Y] = mv_cliped[lidx][MV_Y];

                s32 tmp_mvx = tmp_mv_array[vertex][MV_X] << 2;
                s32 tmp_mvy = tmp_mv_array[vertex][MV_Y] << 2;

                if (cur_info->mvr_idx == 1) {
                    com_mv_rounding_s32(tmp_mvx, tmp_mvy, &tmp_mvx, &tmp_mvy, 4, 4);
                }
                tmp_mv_array[vertex][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_mvx);
                tmp_mv_array[vertex][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_mvy);
            }
            com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, tmp_mv_array, refp, pred, sub_w, sub_h, bit_depth);
             
            u64 mvp_temp = com_had(cu_width, cu_height, org, s_org, pred, cu_width, bit_depth);
            mebits = affine_mv_bits(tmp_mv_array, affine_mvp, pi->num_refp, refi_cur, cur_info->mvr_idx);
            mvp_temp += MV_COST(mebits);

            if (mvp_temp < mvp_best) {
                CP128(affine_mv, tmp_mv_array);
            } else {
                CP128(affine_mv, affine_mvp);
            }
            pi->ref_pic = core->refp[refi_cur][lidx].pic;
            mecost = affine_me_gradient(pi, x, y, cu_width_log2, cu_height_log2, &refi_cur, lidx, affine_mvp, affine_mv, 0, sub_w, sub_h);

            refi[REFP_0] = (lidx == 0) ? refi_cur : REFI_INVALID;
            refi[REFP_1] = (lidx == 1) ? refi_cur : REFI_INVALID;

            mebits = affine_mv_bits(affine_mv, affine_mvp, pi->num_refp, refi_cur, cur_info->mvr_idx);
            mecost += MV_COST(mebits);

            CP128(pi->affine_mv_scale[lidx][refi_cur], affine_mv);

            if (mecost < best_mecost) {
                best_mecost = mecost;
                best_bits = mebits;
                refi_bst = refi_cur;
            }
        }

        CP128(affine_mv, pi->affine_mv_scale[lidx][refi_bst]);

        affine_mvp = pi->affine_mvp_scale[lidx][refi_bst];
        refi[REFP_0] = (lidx == 0) ? refi_bst : REFI_INVALID;
        refi[REFP_1] = (lidx == 1) ? refi_bst : REFI_INVALID;
        refi_L0L1[lidx] = refi_bst;

        for (int vertex = 0; vertex < 2; vertex++) {
            affine_mvd[vertex][MV_X] = affine_mv[vertex][MV_X] - affine_mvp[vertex][MV_X];
            affine_mvd[vertex][MV_Y] = affine_mv[vertex][MV_Y] - affine_mvp[vertex][MV_Y];
        }

        pi->mot_bits[lidx] = best_bits;
        affine_mv[2][MV_X] = affine_mv[0][MV_X] - (affine_mv[1][MV_Y] - affine_mv[0][MV_Y]) * (s16)cu_height / (s16)cu_width;
        affine_mv[2][MV_Y] = affine_mv[0][MV_Y] + (affine_mv[1][MV_X] - affine_mv[0][MV_X]) * (s16)cu_height / (s16)cu_width;
        affine_mv[3][MV_X] = affine_mv[1][MV_X] - (affine_mv[1][MV_Y] - affine_mv[0][MV_Y]) * (s16)cu_height / (s16)cu_width;
        affine_mv[3][MV_Y] = affine_mv[1][MV_Y] + (affine_mv[1][MV_X] - affine_mv[0][MV_X]) * (s16)cu_height / (s16)cu_width;

        memcpy(aff_mv_L0L1[lidx], affine_mv, 2 * MV_D * sizeof(CPMV));

        if (com_get_affine_memory_access(affine_mv, cu_width, cu_height) <= mem) {
            cost_L0L1[lidx] = inter_rdcost(core, lbac_best, 0, 1, NULL, NULL);
        }
    }
}

static void analyze_affine_bi(core_t *core, lbac_t *lbac_best, CPMV aff_mv_L0L1[REFP_NUM][VER_NUM][MV_D], const s8 *refi_L0L1, const double *cost_L0L1)
{
    com_info_t *info       =  core->info;
    com_mode_t *cur_info   = &core->mod_info_curr;
    inter_search_t *pi     = &core->pinter;
    com_pic_t *pic_org     =  core->pic_org;
    int x                  =  core->cu_pix_x;
    int y                  =  core->cu_pix_y;
    int cu_width_log2      =  core->cu_width_log2;
    int cu_height_log2     =  core->cu_height_log2;
    int cu_width           = 1 << cu_width_log2;
    int cu_height          = 1 << cu_height_log2;
    int bit_depth          = info->bit_depth_internal;
    const int mem          = MAX_MEMORY_ACCESS_BI * (1 << cu_width_log2) * (1 << cu_height_log2);
    u32 lambda_mv          = pi->lambda_mv;
    pel(*pred)[MAX_CU_DIM] = cur_info->pred;
    pel *org               = pic_org->y + x + y * pic_org->stride_luma;

    ALIGNED_32(pel org_bi[MAX_CU_DIM]);
    s8  refi[REFP_NUM] = { REFI_INVALID, REFI_INVALID };
    s8  lidx_ref, lidx_cnd;

    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);
    cur_info->cu_mode = MODE_INTER;
    init_inter_data(core);

    if (cost_L0L1[REFP_0] <= cost_L0L1[REFP_1]) {
        lidx_ref = REFP_0;
        lidx_cnd = REFP_1;
    } else {
        lidx_ref = REFP_1;
        lidx_cnd = REFP_0;
    }
    cur_info->refi[REFP_0] = refi_L0L1[REFP_0];
    cur_info->refi[REFP_1] = refi_L0L1[REFP_1];
    CP128(cur_info->affine_mv[lidx_ref], aff_mv_L0L1[lidx_ref]);
    CP128(cur_info->affine_mv[lidx_cnd], aff_mv_L0L1[lidx_cnd]);

    int t0 = (lidx_ref == REFP_0) ? cur_info->refi[lidx_ref] : REFI_INVALID;
    int t1 = (lidx_ref == REFP_1) ? cur_info->refi[lidx_ref] : REFI_INVALID;
    SET_REFI(refi, t0, t1);
    pi->org      = org_bi;
    pi->i_org    = cu_width;

    u64 best_mecost = COM_UINT64_MAX;
    for (int i = 0; i < AFFINE_BI_ITER; i++) {
        com_mc_blk_affine_luma(x, y, info->pic_width, info->pic_height, cu_width, cu_height, cur_info->affine_mv[lidx_ref], core->refp[refi[lidx_ref]][lidx_ref].pic, pred[Y_C], 8, 8, bit_depth);
        create_bi_org(org, pred[Y_C], pic_org->stride_luma, cu_width, cu_height, org_bi, cu_width, info->bit_depth_internal);
        SWAP(refi[lidx_ref], refi[lidx_cnd], t0);
        SWAP(lidx_ref, lidx_cnd, t0);
        pi->num_refp = (u8)core->num_refp[lidx_ref];

        int changed   = 0;
        s8  refi_best = 0;

        for (s8 refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            refi[lidx_ref] = refi_cur;
            pi->ref_pic = core->refp[refi_cur][lidx_ref].pic;

            u64 mecost = affine_me_gradient(pi, x, y, cu_width_log2, cu_height_log2, &refi[lidx_ref], lidx_ref, \
            pi->affine_mvp_scale[lidx_ref][refi_cur], pi->affine_mv_scale[lidx_ref][refi_cur], 1, 8, 8);

            int mebits = affine_mv_bits(pi->affine_mv_scale[lidx_ref][refi_cur], pi->affine_mvp_scale[lidx_ref][refi_cur], pi->num_refp, refi_cur, cur_info->mvr_idx);
            mebits += pi->mot_bits[1 - lidx_ref]; 
            mecost += MV_COST(mebits);

            if (mecost < best_mecost) {
                pi->mot_bits[lidx_ref] = mebits - pi->mot_bits[1 - lidx_ref];
                refi_best = refi_cur;
                best_mecost = mecost;
                changed = 1;
                t0 = (lidx_ref == REFP_0) ? refi_best : cur_info->refi[lidx_cnd];
                t1 = (lidx_ref == REFP_1) ? refi_best : cur_info->refi[lidx_cnd];
                SET_REFI(cur_info->refi, t0, t1);
                CP128(cur_info->affine_mv[lidx_ref], pi->affine_mv_scale[lidx_ref][refi_cur]);
            }
        }
        t0 = (lidx_ref == REFP_0) ? refi_best : REFI_INVALID;
        t1 = (lidx_ref == REFP_1) ? refi_best : REFI_INVALID;
        SET_REFI(refi, t0, t1);
        if (!changed) {
            break;
        }
    }
    for (int vertex = 0; vertex < 2; vertex++) {
        cur_info->affine_mvd[REFP_0][vertex][MV_X] = cur_info->affine_mv[REFP_0][vertex][MV_X] - pi->affine_mvp_scale[REFP_0][cur_info->refi[REFP_0]][vertex][MV_X];
        cur_info->affine_mvd[REFP_0][vertex][MV_Y] = cur_info->affine_mv[REFP_0][vertex][MV_Y] - pi->affine_mvp_scale[REFP_0][cur_info->refi[REFP_0]][vertex][MV_Y];
        cur_info->affine_mvd[REFP_1][vertex][MV_X] = cur_info->affine_mv[REFP_1][vertex][MV_X] - pi->affine_mvp_scale[REFP_1][cur_info->refi[REFP_1]][vertex][MV_X];
        cur_info->affine_mvd[REFP_1][vertex][MV_Y] = cur_info->affine_mv[REFP_1][vertex][MV_Y] - pi->affine_mvp_scale[REFP_1][cur_info->refi[REFP_1]][vertex][MV_Y];
    }
    for (int i = 0; i < REFP_NUM; i++) {
        cur_info->affine_mv[i][2][MV_X] = cur_info->affine_mv[i][0][MV_X] - (cur_info->affine_mv[i][1][MV_Y] - cur_info->affine_mv[i][0][MV_Y]) * (s16)cu_height / (s16)cu_width;
        cur_info->affine_mv[i][2][MV_Y] = cur_info->affine_mv[i][0][MV_Y] + (cur_info->affine_mv[i][1][MV_X] - cur_info->affine_mv[i][0][MV_X]) * (s16)cu_height / (s16)cu_width;
        cur_info->affine_mv[i][3][MV_X] = cur_info->affine_mv[i][1][MV_X] - (cur_info->affine_mv[i][1][MV_Y] - cur_info->affine_mv[i][0][MV_Y]) * (s16)cu_height / (s16)cu_width;
        cur_info->affine_mv[i][3][MV_Y] = cur_info->affine_mv[i][1][MV_Y] + (cur_info->affine_mv[i][1][MV_X] - cur_info->affine_mv[i][0][MV_X]) * (s16)cu_height / (s16)cu_width;

        if (com_get_affine_memory_access(cur_info->affine_mv[i], cu_width, cu_height) > mem) {
            return;
        }
    }
    inter_rdcost(core, lbac_best, 0, 1, NULL, NULL);
}

static int is_same_mv(core_t *core, com_motion_t hmvp_motion)
{
    com_info_t *info    = core->info;
    int x_scu           = core->cu_scu_x;
    int y_scu           = core->cu_scu_y;
    com_map_t *map      = core->map;
    com_scu_t *map_scu  = map->map_scu;
    int neb_addr        = core->cu_scup_in_pic - info->i_scu + (core->cu_width >> MIN_CU_LOG2);
    int neb_avaliable   = COM_IS_INTER_SCU(map_scu[neb_addr]);

    com_motion_t c_motion;

    if (neb_avaliable && core->cnt_hmvp_cands) {
        CP32(c_motion.mv[0], map->map_mv[neb_addr][0]);
        CP32(c_motion.mv[1], map->map_mv[neb_addr][1]);
        CP16(c_motion.ref_idx, map->map_refi[neb_addr]);

        return same_motion(hmvp_motion, c_motion);
    }
    return 0;
}

void analyze_inter_cu(core_t *core, lbac_t *lbac_best)
{
    com_info_t *info      = core->info;
    inter_search_t *pi    = &core->pinter;
    com_mode_t *cur_info  = &core->mod_info_curr;
    com_mode_t *bst_info  = &core->mod_info_best;
    int cu_width_log2     = core->cu_width_log2;
    int cu_height_log2    = core->cu_height_log2;
    int cu_width          = 1 << cu_width_log2;
    int cu_height         = 1 << cu_height_log2;

    int bit_depth = info->bit_depth_internal;
    enc_history_t *history = &core->history_data[cu_width_log2 - 2][cu_height_log2 - 2][core->cu_scup_in_lcu];

    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);
    cur_info->mvr_idx = 0;

    int num_iter_mvp = 2;
    int num_hmvp_inter = MAX_NUM_MVR;
    int num_amvr = MAX_NUM_MVR;
    int allow_affine = info->sqh.affine_enable;

    if (history->visit && info->sqh.emvr_enable) { 
        num_hmvp_inter = history->mvr_hmvp_idx_history + 1;
        if (num_hmvp_inter > MAX_NUM_MVR) {
            num_hmvp_inter = MAX_NUM_MVR;
        }
    }
    if (info->sqh.amvr_enable) {
        if (history->visit) {
            num_amvr = COM_MIN(MAX_NUM_MVR, history->mvr_idx_history + 1);
        }
    } else {
        num_amvr = 1;
    }
    if (history->visit && history->affine_flag_history == 0) {
        allow_affine = 0;
    }
    if (cu_width *cu_height >= 64) {
        analyze_direct_skip(core, lbac_best);
    }

    memset(pi->hpel_satd, 0, sizeof(pi->hpel_satd));
    memset(pi->qpel_satd, 0, sizeof(pi->qpel_satd));

    for (cur_info->hmvp_flag = 0; cur_info->hmvp_flag < num_iter_mvp; cur_info->hmvp_flag++) {
        if (cur_info->hmvp_flag) {
            num_amvr = 0;
            if (info->sqh.emvr_enable) { 
                if ((bst_info->cu_mode == MODE_SKIP && core->skip_mvps_check == 0) || (bst_info->cu_mode != MODE_SKIP)) {
                    num_amvr = COM_MIN(num_hmvp_inter, core->cnt_hmvp_cands);
                }
            }
        }
        for (cur_info->mvr_idx = 0; cur_info->mvr_idx < num_amvr; cur_info->mvr_idx++) {
            double cost_L0L1[2] = { MAX_D_COST, MAX_D_COST };
            s8 refi_L0L1[2] = { REFI_INVALID, REFI_INVALID };
            s16 mv_L0L1[REFP_NUM][MV_D];
            pi->curr_mvr = cur_info->mvr_idx;

            if (cur_info->hmvp_flag) {
                if (is_same_mv(core, core->motion_cands[core->cnt_hmvp_cands - 1 - cur_info->mvr_idx])) {
                    continue;
                }
                if (bst_info->cu_mode == MODE_SKIP && bst_info->skip_idx >= TRADITIONAL_SKIP_NUM && bst_info->skip_idx - TRADITIONAL_SKIP_NUM == cur_info->mvr_idx) {
                    continue;
                }
            }

            init_pb_part(cur_info);
            init_tb_part(cur_info);
            get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
            get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);
            analyze_uni_pred(core, lbac_best, cost_L0L1, mv_L0L1, refi_L0L1);

            if (core->slice_type == SLICE_B && cu_width *cu_height >= 64) {
                analyze_bi(core, lbac_best, mv_L0L1, refi_L0L1, cost_L0L1);

                if (info->sqh.smvd_enable && core->ptr - core->refp[0][REFP_0].ptr == core->refp[0][REFP_1].ptr - core->ptr && !cur_info->hmvp_flag) {
                    analyze_smvd(core, lbac_best);
                }
            }

            if (cur_info->hmvp_flag) {
                if (cur_info->mvr_idx > 1 && (bst_info->cu_mode == MODE_SKIP)) {
                    break;
                }
            } else if (cur_info->mvr_idx && ((bst_info->cu_mode == MODE_SKIP || bst_info->cu_mode == MODE_DIR))) {
                break;
            }
            if (cur_info->mvr_idx > 1 && M32(bst_info->mvd[REFP_0]) == 0 && M32(bst_info->mvd[REFP_1]) == 0) {
                break;
            }
        }
    }
    if (allow_affine && cu_width >= AFF_SIZE && cu_height >= AFF_SIZE) {
        analyze_affine_merge(core, lbac_best);

        if (!(bst_info->cu_mode == MODE_SKIP && !bst_info->affine_flag)) { 
            double cost_L0L1[2] = { MAX_D_COST,   MAX_D_COST   };
            s8     refi_L0L1[2] = { REFI_INVALID, REFI_INVALID };
            CPMV   aff_mv_L0L1[REFP_NUM][VER_NUM][MV_D];
            int num_affine_amvr = info->sqh.amvr_enable ? MAX_NUM_AFFINE_MVR : 1;

            for (cur_info->mvr_idx = 0; cur_info->mvr_idx < num_affine_amvr; cur_info->mvr_idx++) {
                pi->curr_mvr = cur_info->mvr_idx;
                analyze_affine_uni(core, lbac_best, aff_mv_L0L1, refi_L0L1, cost_L0L1);
                if (core->slice_type == SLICE_B) {
                    analyze_affine_bi(core, lbac_best, aff_mv_L0L1, refi_L0L1, cost_L0L1);
                }
            }
        }
    }
    if (!history->visit) {
        history->affine_flag_history = bst_info->affine_flag;
        history->mvr_idx_history     = bst_info->mvr_idx;

        if (bst_info->hmvp_flag) {
            history->mvr_hmvp_idx_history = bst_info->mvr_idx;
        }
    }
}

void inter_search_init(inter_search_t *pi, com_info_t *info, int is_padding)
{
    pi->tab_mvbits        = info->tab_mvbits;
    pi->bit_depth         = info->bit_depth_internal;
    pi->gop_size          = info->gop_size;
    pi->max_search_range  = info->sqh.low_delay ? SEARCH_RANGE_IPEL_LD : SEARCH_RANGE_IPEL_RA;
    pi->max_coord[MV_X]   = (s16)info->pic_width + 4;
    pi->max_coord[MV_Y]   = (s16)info->pic_height + 4;
    pi->is_padding        = is_padding;

    pi->tab_mvbits_offset = (COM_MAX(info->pic_width, info->pic_height) << 2) + 3; // max abs(MV)
	pi->lambda_mv = 0;
}

int inter_search_create(u8 **pptab, com_info_t *info)
{
    int tab_mvbits_offset;
    u8 *tab_mvbits;

    tab_mvbits_offset = (COM_MAX(info->pic_width, info->pic_height) << 2) + 3; // max abs(MV)
    tab_mvbits = (u8 *)com_malloc(sizeof(u8) * (tab_mvbits_offset * 2 + 1)) + tab_mvbits_offset;

    tab_mvbits[0] = 1;
    tab_mvbits[-1] = tab_mvbits[1] = 2;
    tab_mvbits[-2] = tab_mvbits[2] = 3;

    for (int exp_bits = 2; ; exp_bits++) {
        int imax = (1 << exp_bits) - 1;
        int imin = (1 << (exp_bits - 1)) - 1;
        int bits = exp_bits << 1;

        imax = COM_MIN(imax, tab_mvbits_offset);

        for (int i = imin; i < imax; i++) {
            tab_mvbits[-i] = tab_mvbits[i] = bits;
        }
        if (imax == tab_mvbits_offset) {
            break;
        }
    }

    *pptab = tab_mvbits;
    return tab_mvbits_offset;
}

void inter_search_free(u8 *tab_mvbits, int tab_mvbits_offset)
{
    u8 *addr = tab_mvbits - tab_mvbits_offset;
    com_mfree(addr);
}