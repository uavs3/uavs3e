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

static __inline int get_mv_bits_with_mvr(inter_search_t *pi, int mvd_x, int mvd_y, u8 mvr_idx)
{
    return pi->tab_mvbits[mvd_x >> mvr_idx] + pi->tab_mvbits[mvd_y >> mvr_idx] + mvr_idx + (mvr_idx < MAX_NUM_MVR - 1);
}

static __inline int get_mv_bits(inter_search_t *pi, int mvd_x, int mvd_y, int num_refp, int refi, u8 mvr_idx)
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
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    int bit_depth = info->bit_depth_internal;
    com_pic_t *pic_org = core->pic_org;
    int skip_idx, i;
    s16 mvp[REFP_NUM][MV_D];
    s8 refi[REFP_NUM];
    u32 cy/*, cu, cv*/;
    double cost_y;
    int slice_type = core->pichdr->slice_type;
    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int cu_width  = core->cu_width;
    int cu_height = core->cu_height;
    int cu_width_log2  = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;

    pel *y_org = pic_org->y + x + y * pic_org->stride_luma;
    pel *u_org = pic_org->u + (x >> 1) + ((y >> 1) * pic_org->stride_chroma);
    pel *v_org = pic_org->v + (x >> 1) + ((y >> 1) * pic_org->stride_chroma);

    cur_info->cu_mode = MODE_SKIP;
    for (i = 0; i < num_rdo; i++) {
        mode_list[i] = 0;
        cost_list[i] = MAX_D_COST;
    }

    for (skip_idx = 0; skip_idx < num_cands_all; skip_idx++) {
        int bit_cnt, shift = 0;

        mvp[REFP_0][MV_X] = pmv_skip_cand[skip_idx][REFP_0][MV_X];
        mvp[REFP_0][MV_Y] = pmv_skip_cand[skip_idx][REFP_0][MV_Y];
        mvp[REFP_1][MV_X] = pmv_skip_cand[skip_idx][REFP_1][MV_X];
        mvp[REFP_1][MV_Y] = pmv_skip_cand[skip_idx][REFP_1][MV_Y];
        refi[REFP_0] = refi_skip_cand[skip_idx][REFP_0];
        refi[REFP_1] = refi_skip_cand[skip_idx][REFP_1];

        if (!REFI_IS_VALID(refi[REFP_0]) && !REFI_IS_VALID(refi[REFP_1])) {
            continue;
        }

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

        com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi, mvp, core->refp, cur_info->pred, cu_width, CHANNEL_L, bit_depth);
        cy = calc_satd_16b(cu_width, cu_height, y_org, cur_info->pred[Y_C], pic_org->stride_luma, cu_width, bit_depth);
        cost_y = (double)cy;

        lbac_t *lbac = &core->sbac_rdo;
        lbac_copy(lbac, &core->sbac_bakup);
        bit_cnt = lbac_get_bits(lbac);
        enc_bits_inter(core, lbac, slice_type);
        bit_cnt = lbac_get_bits(lbac) - bit_cnt;

        cost_y += RATE_TO_COST_SQRT_LAMBDA(core->sqrt_lambda[0], bit_cnt);
        while (shift < num_rdo && cost_y < cost_list[num_rdo - 1 - shift]) {
            shift++;
        }
        if (shift != 0) {
            for (i = 1; i < shift; i++) {
                mode_list[num_rdo - i] = mode_list[num_rdo - 1 - i];
                cost_list[num_rdo - i] = cost_list[num_rdo - 1 - i];
            }
            mode_list[num_rdo - shift] = skip_idx;
            cost_list[num_rdo - shift] = cost_y;
        }
    }

}

static void check_best_mode(core_t *core, lbac_t *sbac_best, lbac_t *lbac, const double cost_curr)
{
    com_mode_t *bst_info = &core->mod_info_best;
    com_mode_t *cur_info = &core->mod_info_curr;

    if (cost_curr < core->cost_best) {
        int j, lidx;
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

        lbac_copy(sbac_best, lbac);

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
                        bst_info->affine_mv[lidx][vertex][MV_X] = cur_info->affine_mv[lidx][vertex][MV_X];
                        bst_info->affine_mv[lidx][vertex][MV_Y] = cur_info->affine_mv[lidx][vertex][MV_Y];
                        bst_info->affine_mvd[lidx][vertex][MV_X] = cur_info->affine_mvd[lidx][vertex][MV_X];
                        bst_info->affine_mvd[lidx][vertex][MV_Y] = cur_info->affine_mvd[lidx][vertex][MV_Y];
                    }
                }
            }

            if (bst_info->cu_mode == MODE_SKIP) {
                if (bst_info->umve_flag != 0) {
                    bst_info->umve_idx = cur_info->umve_idx;
                } else {
                    bst_info->skip_idx = cur_info->skip_idx;
                }

                for (j = 0; j < N_C; j++) {
                    int size_tmp = (cu_width * cu_height) >> (j == 0 ? 0 : 2);
                    com_mcpy(bst_info->pred[j], cur_info->pred[j], size_tmp * sizeof(pel));
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
                for (j = 0; j < N_C; j++) {
                    int size_tmp = (cu_width * cu_height) >> (j == 0 ? 0 : 2);
                    cu_plane_nz_cpy(bst_info->num_nz, cur_info->num_nz, j);
                    com_mcpy(bst_info->pred[j], cur_info->pred[j], size_tmp * sizeof(pel));
                    com_mcpy(bst_info->coef[j], cur_info->coef[j], size_tmp * sizeof(s16));
                }
            }
        }
    }
    double skip_mode_2_thread = core->lcu_qp_y / 60.0 * (THRESHOLD_MVPS_CHECK - 1) + 1;
    if (bst_info->cu_mode == MODE_SKIP && cur_info->cu_mode == MODE_INTER && (core->cost_best * skip_mode_2_thread) > cost_curr) {
        core->skip_mvps_check = 0;
    }
}

#if TR_SAVE_LOAD
static u8 search_inter_tr_info(core_t *core, u16 cu_ssd)
{
    u8 tb_part_size = 255;
    enc_history_t *p_data = &core->bef_data[core->cu_width_log2 - 2][core->cu_height_log2 - 2][core->cu_scup_in_lcu];

    for (int idx = 0; idx < p_data->num_inter_pred; idx++) {
        if (p_data->inter_pred_dist[idx] == cu_ssd) {
            return p_data->inter_tb_part[idx];
        }
    }
    return tb_part_size;
}

static void save_inter_tr_info(core_t *core, u16 cu_ssd, u8 tb_part_size)
{
    enc_history_t *p_data = &core->bef_data[core->cu_width_log2 - 2][core->cu_height_log2 - 2][core->cu_scup_in_lcu];

    if (p_data->num_inter_pred == NUM_SL_INTER) {
        return;
    }
    p_data->inter_pred_dist[p_data->num_inter_pred] = cu_ssd;
    p_data->inter_tb_part[p_data->num_inter_pred] = tb_part_size;
    p_data->num_inter_pred++;
}
#endif

static double inter_rdcost(core_t *core, lbac_t *sbac_best, int bForceAllZero)
{
    com_pic_t *pic_org = core->pic_org;
    com_info_t *info = core->info;
    com_map_t *map = core->map;
    com_mode_t *cur_info = &core->mod_info_curr;
    s16(*coef)[MAX_CU_DIM] = cur_info->coef;
    s16(*mv)[MV_D] = cur_info->mv;
    s8 *refi = cur_info->refi;
    pel(*pred)[MAX_CU_DIM] = cur_info->pred;
    int bit_depth = info->bit_depth_internal;
    int(*num_nz_coef)[N_C], tnnz, width[N_C], height[N_C], log2_w[N_C], log2_h[N_C], stride_org[N_C];
    pel(*rec)[MAX_CU_DIM];
    s64 dist[2][N_C] = { 0 }, dist_pred[N_C] = { 0 };
    double cost, cost_best = MAX_D_COST;
    int cbf_best[N_C], nnz_store[MAX_NUM_TB][N_C], tb_part_store;
    int num_n_c = core->tree_status == TREE_LC ? N_C : 1;
    int bit_cnt;
    int i, cbf_y, cbf_u, cbf_v;
    pel *org[N_C];
    int cbf_comps[N_C] = { 0, };
    int j;
    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    ALIGNED_32(s16 resi_t[N_C][MAX_CU_DIM]);
    lbac_t sbac_cur_mode;
    u8  is_from_mv_field = 0;
    int slice_type = core->pichdr->slice_type;
    rec = cur_info->rec;
    num_nz_coef = cur_info->num_nz;
    width [Y_C] = 1 << cu_width_log2 ;
    height[Y_C] = 1 << cu_height_log2;
    width [U_C] = width[V_C] = 1 << (cu_width_log2 - 1);
    height[U_C] = height[V_C] = 1 << (cu_height_log2 - 1);
    log2_w[Y_C] = cu_width_log2;
    log2_h[Y_C] = cu_height_log2;
    log2_w[U_C] = log2_w[V_C] = cu_width_log2 - 1;
    log2_h[U_C] = log2_h[V_C] = cu_height_log2 - 1;
    org[Y_C] = pic_org->y + (y * pic_org->stride_luma) + x;
    org[U_C] = pic_org->u + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);
    org[V_C] = pic_org->v + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);
    stride_org[Y_C] = pic_org->stride_luma;
    stride_org[U_C] = pic_org->stride_chroma;
    stride_org[V_C] = pic_org->stride_chroma;

    /* prediction */
    if (slice_type == SLICE_P) {
        assert(REFI_IS_VALID(cur_info->refi[REFP_0]) && !REFI_IS_VALID(cur_info->refi[REFP_1]));
    }

    if (cur_info->affine_flag) {
        com_mc_cu_affine(x, y, info->pic_width, info->pic_height, width[0], height[0], refi, cur_info->affine_mv, core->refp, pred, cur_info->affine_flag + 1, core->pichdr, core->map->map_mv, bit_depth);

#if RDO_WITH_DBLOCK
        com_set_affine_mvf(core->cu_scup_in_pic, cu_width_log2, cu_height_log2, info->i_scu, cur_info, map, core->pichdr);
        is_from_mv_field = 1;
#endif
    } else {
        assert(cur_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);
        com_mc_cu(x, y, info->pic_width, info->pic_height, width[0], height[0], refi, mv, core->refp, pred, width[0], core->tree_status, bit_depth);
    }

    /* get residual */
    cu_pel_sub(core->tree_status, x, y, cu_width_log2, cu_height_log2, core->pic_org, pred, resi_t);

    for (i = 0; i < num_n_c; i++) {
        dist[0][i] = dist_pred[i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], pred[i], org[i], width[i], stride_org[i], bit_depth);
    }

#if RDO_WITH_DBLOCK
    dist[0][Y_C] += calc_dist_filter_boundary(core, core->pic_rec, core->pic_org, cu_width, cu_height, pred[Y_C], cu_width, x, y, 0, 0, refi, mv, is_from_mv_field, 1);
#endif

    /* test all zero case */
    memset(cbf_best, 0, sizeof(int) * N_C);

    if (cur_info->cu_mode != MODE_DIR) { // do not check forced zero for direct mode
        memset(num_nz_coef, 0, sizeof(int) * MAX_NUM_TB * N_C);
        cur_info->tb_part = SIZE_2Nx2N;
        if (core->tree_status == TREE_LC) {
            cost_best = (double)dist[0][Y_C] + ((dist[0][U_C] + dist[0][V_C]) * core->dist_chroma_weight[0]);
        } else {
            assert(core->tree_status == TREE_L);
            cost_best = (double)dist[0][Y_C];
        }
        lbac_t *lbac = &core->sbac_rdo;
        lbac_copy(lbac, &core->sbac_bakup);
        bit_cnt = lbac_get_bits(lbac);
        enc_bits_inter(core, lbac, slice_type);
        bit_cnt = lbac_get_bits(lbac) - bit_cnt;
        cost_best += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
        lbac_copy(&sbac_cur_mode, lbac);
    }

    /* transform and quantization */
    bForceAllZero |= (cur_info->cu_mode == MODE_SKIP);
    bForceAllZero |= cu_width_log2 > 6 || cu_height_log2 > 6;

    if (!bForceAllZero) {
#if TR_EARLY_TERMINATE
        core->dist_pred_luma = dist_pred[Y_C];
#endif
#if TR_SAVE_LOAD 
        u16 cu_ssd_u16 = 0;
        core->best_tb_part_hist = 255;
        if (info->sqh.pbt_enable && core->tree_status != TREE_L &&
            is_tb_avaliable(info, cu_width_log2, cu_height_log2, cur_info->pb_part, MODE_INTER) && cur_info->pb_part == SIZE_2Nx2N) {
            s64 cu_ssd_s64 = dist_pred[Y_C] + dist_pred[U_C] + dist_pred[V_C];
            int shift_val = COM_MIN(cu_width_log2 + cu_height_log2, 9);
            cu_ssd_u16 = (u16)(cu_ssd_s64 + (s64)(1 << (shift_val - 1))) >> shift_val;
            core->best_tb_part_hist = search_inter_tr_info(core, cu_ssd_u16);
        }
#endif
        lbac_t *lbac = &core->sbac_rdo;
        tnnz = enc_tq_itdq_yuv_nnz(core, lbac, cur_info, coef, resi_t, pred, rec, refi, mv, is_from_mv_field);

        if (tnnz) {
            for (i = 0; i < num_n_c; i++) {
                if (is_cu_plane_nz(num_nz_coef, i)) {
#if RDO_WITH_DBLOCK
                    if (i == 0) {
                        dist[1][Y_C] = calc_dist_filter_boundary(core, core->pic_rec, core->pic_org, cu_width, cu_height, rec[Y_C], cu_width, x, y, 0, 1, refi, mv, is_from_mv_field, 0);
                    } else {
                        dist[1][i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], rec[i], org[i], width[i], stride_org[i], bit_depth);
                    }
#else
                    dist[1][i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], rec[i], org[i], width[i], stride_org[i], bit_depth);
#endif
                } else {
                    dist[1][i] = dist[0][i];
                }
            }

            cbf_y = is_cu_plane_nz(num_nz_coef, Y_C) > 0 ? 1 : 0;
            cbf_u = is_cu_plane_nz(num_nz_coef, U_C) > 0 ? 1 : 0;
            cbf_v = is_cu_plane_nz(num_nz_coef, V_C) > 0 ? 1 : 0;

            if (core->tree_status == TREE_LC) {
                cost = (double)dist[cbf_y][Y_C] + ((dist[cbf_u][U_C] + dist[cbf_v][V_C]) * core->dist_chroma_weight[0]);
            } else {
                assert(core->tree_status == TREE_L);
                cost = (double)dist[cbf_y][Y_C];
            }

            lbac_copy(lbac, &core->sbac_bakup);
            bit_cnt = lbac_get_bits(lbac);
            enc_bits_inter(core, lbac, slice_type);
            bit_cnt = lbac_get_bits(lbac) - bit_cnt;
            cost += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
            if (cost < cost_best) {
                cost_best = cost;
                cbf_best[Y_C] = cbf_y;
                cbf_best[U_C] = cbf_u;
                cbf_best[V_C] = cbf_v;
                lbac_copy(&sbac_cur_mode, lbac);
            }

            com_mcpy(nnz_store, num_nz_coef, sizeof(int) * MAX_NUM_TB * N_C);
            tb_part_store = cur_info->tb_part;

            lbac_t sbac_cur_comp_best;
            lbac_copy(&sbac_cur_comp_best, &core->sbac_bakup);

            if (core->tree_status == TREE_LC) { 
                for (i = 0; i < N_C; i++) {
                    if (is_cu_plane_nz(nnz_store, i) > 0) {
                        double cost_comp_best = MAX_D_COST;
                        lbac_t sbac_cur_comp;
                        lbac_copy(&sbac_cur_comp, &sbac_cur_comp_best);

                        for (j = 0; j < 2; j++) {
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

                            lbac_copy(lbac, &sbac_cur_comp);
                            bit_cnt = lbac_get_bits(lbac);
                            enc_bits_inter_comp(core, lbac, coef[i], i);
                            bit_cnt = lbac_get_bits(lbac) - bit_cnt;
                            cost += RATE_TO_COST_LAMBDA(core->lambda[i], bit_cnt);

                            if (cost < cost_comp_best) {
                                cost_comp_best = cost;
                                cbf_comps[i] = j;
                                lbac_copy(&sbac_cur_comp_best, lbac);
                            }
                        }
                    } else {
                        cbf_comps[i] = 0;
                    }
                }

                if (cbf_comps[Y_C] != 0 || cbf_comps[U_C] != 0 || cbf_comps[V_C] != 0) {
                    for (i = 0; i < N_C; i++) {
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

                        lbac_copy(lbac, &core->sbac_bakup);
                        bit_cnt = lbac_get_bits(lbac);
                        enc_bits_inter(core, lbac, slice_type);
                        bit_cnt = lbac_get_bits(lbac) - bit_cnt;
                        cost += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
                        if (cost < cost_best) {
                            cost_best = cost;
                            cbf_best[Y_C] = cbf_y;
                            cbf_best[U_C] = cbf_u;
                            cbf_best[V_C] = cbf_v;
                            lbac_copy(&sbac_cur_mode, lbac);
                        }
                    }
                }
            }

            for (i = 0; i < num_n_c; i++) {
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

    if (!is_cu_plane_nz(num_nz_coef, Y_C)) {
        cur_info->tb_part = SIZE_2Nx2N; 
    }

    check_best_mode(core, sbac_best, &sbac_cur_mode, cost_best);

    return cost_best;
}

double pinter_residue_rdo_chroma(core_t *core)
{
    com_pic_t *pic_org = core->pic_org;
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    s16(*coef)[MAX_CU_DIM] = cur_info->coef;
    s16(*mv)[MV_D] = cur_info->mv;
    s8 *refi = cur_info->refi;
    pel(*pred)[MAX_CU_DIM] = cur_info->pred;
    int bit_depth = info->bit_depth_internal;
    int(*num_nz_coef)[N_C], tnnz, width[N_C], height[N_C], log2_w[N_C], log2_h[N_C];;
    pel(*rec)[MAX_CU_DIM];
    s64    dist[2][N_C];
    double cost, cost_best = MAX_D_COST;
    int    cbf_best[N_C];
    int    bit_cnt;
    int    i, cbf_y, cbf_u, cbf_v;
    pel   *org[N_C];
    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    ALIGNED_32(s16 resi_t[N_C][MAX_CU_DIM]);

    rec = cur_info->rec;
    num_nz_coef = cur_info->num_nz;
    width[Y_C] = 1 << cu_width_log2;
    height[Y_C] = 1 << cu_height_log2;
    width[U_C] = width[V_C] = 1 << (cu_width_log2 - 1);
    height[U_C] = height[V_C] = 1 << (cu_height_log2 - 1);
    log2_w[Y_C] = cu_width_log2;
    log2_h[Y_C] = cu_height_log2;
    log2_w[U_C] = log2_w[V_C] = cu_width_log2 - 1;
    log2_h[U_C] = log2_h[V_C] = cu_height_log2 - 1;
    org[Y_C] = pic_org->y + (y * pic_org->stride_luma) + x;
    org[U_C] = pic_org->u + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);
    org[V_C] = pic_org->v + ((y >> 1) * pic_org->stride_chroma) + (x >> 1);

    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);

    int luma_scup = core->cu_scup_in_pic + PEL2SCU(core->cu_width - 1) + PEL2SCU(core->cu_height - 1) * info->i_scu;

    com_map_t *map = core->map;

    for (i = 0; i < REFP_NUM; i++) {
        refi[i] = map->map_refi[luma_scup][i];
        mv[i][MV_X] = map->map_mv[luma_scup][i][MV_X];
        mv[i][MV_Y] = map->map_mv[luma_scup][i][MV_Y];
    }

    assert(cur_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);
    com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi, mv, core->refp, pred, cu_width, core->tree_status, bit_depth);

    cu_pel_sub(core->tree_status, x, y, cu_width_log2, cu_height_log2, core->pic_org, pred, resi_t);
    memset(dist, 0, sizeof(s64) * 2 * N_C);

    for (i = 1; i < N_C; i++) {
        dist[0][i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], pred[i], org[i], width[i], pic_org->stride_chroma, bit_depth);
    }

    memset(cbf_best, 0, sizeof(int) * N_C);
    memset(num_nz_coef, 0, sizeof(int) * MAX_NUM_TB * N_C);
    assert(cur_info->tb_part == SIZE_2Nx2N);
    cost_best = (double)((dist[0][U_C] + dist[0][V_C]) * core->dist_chroma_weight[0]);

    lbac_t *lbac = &core->sbac_rdo;
    lbac_copy(lbac, &core->sbac_bakup);
    bit_cnt = lbac_get_bits(lbac);
    lbac_enc_coef(lbac, NULL, core, coef, core->cu_width_log2, core->cu_height_log2, cur_info->cu_mode, cur_info, core->tree_status); // only count coeff bits for chroma tree
    bit_cnt = lbac_get_bits(lbac) - bit_cnt;
    cost_best += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);

    tnnz = enc_tq_itdq_yuv_nnz(core, lbac, cur_info, coef, resi_t, pred, rec, refi, mv, 0);

    if (tnnz) {
        for (i = 1; i < N_C; i++) {
            if (is_cu_plane_nz(num_nz_coef, i)) {
                dist[1][i] = block_pel_ssd(log2_w[i], 1 << log2_h[i], rec[i], org[i], width[i], pic_org->stride_chroma, bit_depth);
            } else {
                dist[1][i] = dist[0][i];
            }
        }

        cbf_y = is_cu_plane_nz(num_nz_coef, Y_C) > 0 ? 1 : 0;
        cbf_u = is_cu_plane_nz(num_nz_coef, U_C) > 0 ? 1 : 0;
        cbf_v = is_cu_plane_nz(num_nz_coef, V_C) > 0 ? 1 : 0;

        cost = (double)((dist[cbf_u][U_C] + dist[cbf_v][V_C]) * core->dist_chroma_weight[0]);

        lbac_copy(lbac, &core->sbac_bakup);
        bit_cnt = lbac_get_bits(lbac);
        lbac_enc_coef(lbac, NULL, core, coef, core->cu_width_log2, core->cu_height_log2, cur_info->cu_mode, cur_info, core->tree_status); // only count coeff bits for chroma tree
        bit_cnt = lbac_get_bits(lbac) - bit_cnt;
        cost += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
        if (cost < cost_best) {
            cost_best = cost;
            cbf_best[Y_C] = cbf_y;
            cbf_best[U_C] = cbf_u;
            cbf_best[V_C] = cbf_v;
        }
    }

    com_mode_t *mod_best = &core->mod_info_best;

    for (i = 0; i < N_C; i++) {
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
            pmv_cands[num_cands][REFP_0][MV_X] = 0;
            pmv_cands[num_cands][REFP_0][MV_Y] = 0;
        }
        pmv_cands[num_cands][REFP_1][MV_X] = 0;
        pmv_cands[num_cands][REFP_1][MV_Y] = 0;
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

static void analyze_direct_skip(core_t *core, lbac_t *sbac_best)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    double       cost;
    int          skip_idx;
    const int num_iter = 2;
    int bNoResidual;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;

    int num_cands_woUMVE = 0;
    s16 pmv_cands[MAX_SKIP_NUM + UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM][REFP_NUM][MV_D];
    s8 refi_cands[MAX_SKIP_NUM + UMVE_MAX_REFINE_NUM * UMVE_BASE_NUM][REFP_NUM];
    int num_cands_all, num_rdo;
    double cost_list[MAX_INTER_SKIP_RDO];
    int mode_list[MAX_INTER_SKIP_RDO];

    derive_inter_cands(core, pmv_cands, refi_cands, &num_cands_all, &num_cands_woUMVE);
    num_rdo = num_cands_woUMVE;
    assert(num_rdo <= COM_MIN(MAX_INTER_SKIP_RDO, TRADITIONAL_SKIP_NUM + info->sqh.num_of_hmvp));

    make_cand_list(core, mode_list, cost_list, num_cands_woUMVE, num_cands_all, num_rdo, pmv_cands, refi_cands);

    for (skip_idx = 0; skip_idx < num_rdo; skip_idx++) {
        int skip_idx_true = mode_list[skip_idx];
        if (skip_idx_true < num_cands_woUMVE) {
            cur_info->umve_flag = 0;
            cur_info->skip_idx = skip_idx_true;
        } else {
            cur_info->umve_flag = 1;
            cur_info->umve_idx = skip_idx_true - num_cands_woUMVE;
        }

        cur_info->mv[REFP_0][MV_X] = pmv_cands[skip_idx_true][REFP_0][MV_X];
        cur_info->mv[REFP_0][MV_Y] = pmv_cands[skip_idx_true][REFP_0][MV_Y];
        cur_info->mv[REFP_1][MV_X] = pmv_cands[skip_idx_true][REFP_1][MV_X];
        cur_info->mv[REFP_1][MV_Y] = pmv_cands[skip_idx_true][REFP_1][MV_Y];
        cur_info->refi[REFP_0] = refi_cands[skip_idx_true][REFP_0];
        cur_info->refi[REFP_1] = refi_cands[skip_idx_true][REFP_1];

        if (!REFI_IS_VALID(cur_info->refi[REFP_0]) && !REFI_IS_VALID(cur_info->refi[REFP_1])) {
            continue;
        }
        if ((core->pichdr->slice_type == SLICE_P) && (cur_info->skip_idx == 1 || cur_info->skip_idx == 2) && (cur_info->umve_flag == 0)) {
            continue;
        }
        for (bNoResidual = 0; bNoResidual < num_iter; bNoResidual++) {
            cur_info->cu_mode = bNoResidual ? MODE_SKIP : MODE_DIR;
            cost = inter_rdcost(core, sbac_best, 0);
        }
    }
}

static void analyze_affine_merge(core_t *core, lbac_t *sbac_best)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    pel          *y_org, *u_org, *v_org;
    com_pic_t *pic_org = core->pic_org;
    s8           mrg_list_refi[AFF_MAX_NUM_MRG][REFP_NUM];
    int          mrg_list_cp_num[AFF_MAX_NUM_MRG];
    CPMV         mrg_list_cp_mv[AFF_MAX_NUM_MRG][REFP_NUM][VER_NUM][MV_D];

    double       cost = MAX_D_COST;
    int          mrg_idx, num_cands = 0;
    int          ver = 0, iter = 0, iter_start = 0, iter_end = 2;

    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int log2_cuw = core->cu_width_log2;
    int log2_cuh = core->cu_height_log2;
    int cu_width = 1 << log2_cuw;
    int cu_height = 1 << log2_cuh;
    y_org = pic_org->y + x + y * pic_org->stride_luma;
    u_org = pic_org->u + (x >> 1) + ((y >> 1) * pic_org->stride_chroma);
    v_org = pic_org->v + (x >> 1) + ((y >> 1) * pic_org->stride_chroma);

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

        for (ver = 0; ver < mrg_list_cp_num[mrg_idx]; ver++) {
            cur_info->affine_mv[REFP_0][ver][MV_X] = mrg_list_cp_mv[mrg_idx][REFP_0][ver][MV_X];
            cur_info->affine_mv[REFP_0][ver][MV_Y] = mrg_list_cp_mv[mrg_idx][REFP_0][ver][MV_Y];
            cur_info->affine_mv[REFP_1][ver][MV_X] = mrg_list_cp_mv[mrg_idx][REFP_1][ver][MV_X];
            cur_info->affine_mv[REFP_1][ver][MV_Y] = mrg_list_cp_mv[mrg_idx][REFP_1][ver][MV_Y];
        }
        cur_info->refi[REFP_0] = mrg_list_refi[mrg_idx][REFP_0];
        cur_info->refi[REFP_1] = mrg_list_refi[mrg_idx][REFP_1];

        for (iter = iter_start; iter < iter_end; iter++) {
            cur_info->cu_mode = iter ? MODE_SKIP : MODE_DIR;
            cost = inter_rdcost(core, sbac_best, 0);
        }
    }
}

static void analyze_uni_pred(core_t *core, lbac_t *sbac_best, double *cost_L0L1, s16 mv_L0L1[REFP_NUM][MV_D], s8 *refi_L0L1)
{
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi = &core->pinter;
    s8 best_refi = 0;
    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;

    cur_info->cu_mode = MODE_INTER;

    pi->i_org = core->pic_org->stride_luma;
    pi->org = core->pic_org->y + y * pi->i_org + x;

    for (int lidx = 0; lidx <= ((core->slice_type == SLICE_P) ? PRED_L0 : PRED_L1); lidx++) {
        u64 best_mecost = COM_UINT64_MAX;

        init_inter_data(core);
        pi->num_refp = (u8)core->num_refp[lidx];

        for (int refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            s16 *mvp = pi->mvp_scale[lidx][refi_cur];
            s16 *mv  = pi->mv_scale [lidx][refi_cur];

            com_derive_mvp(core->info, core->ptr, core->cu_scup_in_pic, lidx, refi_cur, cur_info->hmvp_flag, core->cnt_hmvp_cands,
                           core->motion_cands, core->map, core->refp, cur_info->mvr_idx, cu_width, cu_height, mvp);

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

        mv_L0L1[lidx][MV_X] = mv[MV_X] = pi->mv_scale[lidx][best_refi][MV_X];
        mv_L0L1[lidx][MV_Y] = mv[MV_Y] = pi->mv_scale[lidx][best_refi][MV_Y];
        mvd[MV_X] = mv[MV_X] - mvp[MV_X];
        mvd[MV_Y] = mv[MV_Y] - mvp[MV_Y];

        pi->mot_bits[lidx] = get_mv_bits(pi, mvd[MV_X], mvd[MV_Y], pi->num_refp, best_refi, cur_info->mvr_idx);

        refi_L0L1[lidx] = best_refi;
        cost_L0L1[lidx] = inter_rdcost(core, sbac_best, 0);
    }
}

static void analyze_bi(core_t *core, lbac_t *sbac_best, s16 mv_L0L1[REFP_NUM][MV_D], const s8 *refi_L0L1, const double *cost_L0L1)
{
    com_pic_t *pic_org = core->pic_org;
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi = &core->pinter;
    int bit_depth = info->bit_depth_internal;
    s8  refi[REFP_NUM] = { REFI_INVALID, REFI_INVALID };
    u64 best_mecost = COM_UINT64_MAX;
    s8  refi_best = 0, refi_cur;
    int changed = 0;
    u64 mecost;
    pel *org;
    pel(*pred)[MAX_CU_DIM];
    s8 t0, t1;
    double cost;
    s8  lidx_ref, lidx_cnd;
    int i;
    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int cu_width_log2  = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width  = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;

    cur_info->cu_mode = MODE_INTER;

    init_inter_data(core);
    if (cost_L0L1[PRED_L0] <= cost_L0L1[PRED_L1]) {
        lidx_ref = REFP_0;
        lidx_cnd = REFP_1;
    } else {
        lidx_ref = REFP_1;
        lidx_cnd = REFP_0;
    }
    cur_info->refi[REFP_0] = refi_L0L1[REFP_0];
    cur_info->refi[REFP_1] = refi_L0L1[REFP_1];
    t0 = (lidx_ref == REFP_0) ? refi_L0L1[lidx_ref] : REFI_INVALID;
    t1 = (lidx_ref == REFP_1) ? refi_L0L1[lidx_ref] : REFI_INVALID;
    SET_REFI(refi, t0, t1);
    cur_info->mv[lidx_ref][MV_X] = mv_L0L1[lidx_ref][MV_X];
    cur_info->mv[lidx_ref][MV_Y] = mv_L0L1[lidx_ref][MV_Y];
    cur_info->mv[lidx_cnd][MV_X] = mv_L0L1[lidx_cnd][MV_X];
    cur_info->mv[lidx_cnd][MV_Y] = mv_L0L1[lidx_cnd][MV_Y];

    org = pic_org->y + x + y * pic_org->stride_luma;
    pred = cur_info->pred;

    for (i = 0; i < 2; i++) {
        ALIGNED_32(pel org_bi[MAX_CU_DIM]);
        pi->i_org = cu_width;
        pi->org = org_bi;

        com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi, cur_info->mv, core->refp, pred, cu_width, CHANNEL_L, bit_depth);
        create_bi_org(org, pred[Y_C], pic_org->stride_luma, cu_width, cu_height, org_bi, cu_width, info->bit_depth_internal);

        SWAP(refi[lidx_ref], refi[lidx_cnd], t0);
        SWAP(lidx_ref, lidx_cnd, t0);
        changed = 0;
        pi->num_refp = (u8)core->num_refp[lidx_ref];

        for (refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            mecost = me_search_tz(pi, x, y, cu_width, cu_height, core->info->pic_width, core->info->pic_height, refi_cur, lidx_ref, pi->mvp_scale[lidx_ref][refi_cur], pi->mv_scale[lidx_ref][refi_cur], 1);
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

    cur_info->mv[REFP_0][MV_X] = (cur_info->mv[REFP_0][MV_X] >> cur_info->mvr_idx) << cur_info->mvr_idx;
    cur_info->mv[REFP_0][MV_Y] = (cur_info->mv[REFP_0][MV_Y] >> cur_info->mvr_idx) << cur_info->mvr_idx;
    cur_info->mv[REFP_1][MV_X] = (cur_info->mv[REFP_1][MV_X] >> cur_info->mvr_idx) << cur_info->mvr_idx;
    cur_info->mv[REFP_1][MV_Y] = (cur_info->mv[REFP_1][MV_Y] >> cur_info->mvr_idx) << cur_info->mvr_idx;

    cur_info->mvd[REFP_0][MV_X] = cur_info->mv[REFP_0][MV_X] - pi->mvp_scale[REFP_0][cur_info->refi[REFP_0]][MV_X];
    cur_info->mvd[REFP_0][MV_Y] = cur_info->mv[REFP_0][MV_Y] - pi->mvp_scale[REFP_0][cur_info->refi[REFP_0]][MV_Y];
    cur_info->mvd[REFP_1][MV_X] = cur_info->mv[REFP_1][MV_X] - pi->mvp_scale[REFP_1][cur_info->refi[REFP_1]][MV_X];
    cur_info->mvd[REFP_1][MV_Y] = cur_info->mv[REFP_1][MV_Y] - pi->mvp_scale[REFP_1][cur_info->refi[REFP_1]][MV_Y];

    for (i = 0; i < REFP_NUM; i++) {
        int amvr_shift = cur_info->mvr_idx;
        cur_info->mvd[i][MV_X] = cur_info->mvd[i][MV_X] >> amvr_shift << amvr_shift;
        cur_info->mvd[i][MV_Y] = cur_info->mvd[i][MV_Y] >> amvr_shift << amvr_shift;

        int mv_x = (s32)cur_info->mvd[i][MV_X] + pi->mvp_scale[i][cur_info->refi[i]][MV_X];
        int mv_y = (s32)cur_info->mvd[i][MV_Y] + pi->mvp_scale[i][cur_info->refi[i]][MV_Y];
        cur_info->mv[i][MV_X] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
        cur_info->mv[i][MV_Y] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
    }

    cost = inter_rdcost(core, sbac_best, 0);
}

static u32 smvd_refine(core_t *core, int x, int y, int log2_cuw, int log2_cuh, s16 mv[REFP_NUM][MV_D], s16 mvp[REFP_NUM][MV_D], s8 refi[REFP_NUM], s32 lidx_cur, s32 lidx_tar, u32 mecost, s32 search_pattern, s32 search_round, s32 search_shift)
{
    com_info_t *info = core->info;
    com_pic_t *pic_org = core->pic_org;
    inter_search_t *pi = &core->pinter;
    com_mode_t *cur_info = &core->mod_info_curr;
    int bit_depth = info->bit_depth_internal;

    int cu_width, cu_height;
    u8  mv_bits = 0;
    pel *org;

    s32 search_offset_cross[4][MV_D] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
    s32 search_offset_square[8][MV_D] = { { -1, 1 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 } };
    s32 search_offset_diamond[8][MV_D] = { { 0, 2 }, { 1, 1 }, { 2, 0 }, { 1, -1 }, { 0, -2 }, { -1, -1 }, { -2, 0 }, { -1, 1 } };
    s32 search_offset_hexagon[6][MV_D] = { { 2, 0 }, { 1, 2 }, { -1, 2 }, { -2, 0 }, { -1, -2 }, { 1, -2 } };

    s32 direct_start = 0;
    s32 direct_end = 0;
    s32 direct_rounding = 0;
    s32 direct_mask = 0;
    s32(*search_offset)[MV_D] = search_offset_diamond;

    s32 round;
    s32 best_direct = -1;
    s32 step = 1;
    int ref_mvr_bits = tbl_refi_bits[0][cur_info->refi[lidx_cur]] + pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1);
    u8 *tab_mvbits_x = pi->tab_mvbits;
    u8 *tab_mvbits_y = pi->tab_mvbits;
    int mvr_idx = cur_info->mvr_idx;
    u32 lambda_mv = pi->lambda_mv;

    org = pic_org->y + x + y * pic_org->stride_luma;

    cu_width = (1 << log2_cuw);
    cu_height = (1 << log2_cuh);

    if (search_pattern == 0) {
        direct_end = 3;
        direct_rounding = 4;
        direct_mask = 0x03;
        search_offset = search_offset_cross;
    } else if (search_pattern == 1) {
        direct_end = 7;
        direct_rounding = 8;
        direct_mask = 0x07;
        search_offset = search_offset_square;
    } else if (search_pattern == 2) {
        direct_end = 7;
        direct_rounding = 8;
        direct_mask = 0x07;
        search_offset = search_offset_diamond;
    } else if (search_pattern == 3) {
        direct_end = 5;
        search_offset = search_offset_hexagon;
    }

    for (round = 0; round < search_round; round++) {
        s16 mv_cur_center[MV_D];
        s32 index;

        best_direct = -1;
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

            mv_bits = GET_MVBITS_X(mvd_cand[lidx_cur][MV_X]) + GET_MVBITS_Y(mvd_cand[lidx_cur][MV_Y]);
            mv_bits += ref_mvr_bits;

            mecost_tmp = block_pel_satd(log2_cuw, log2_cuh, org, cur_info->pred[0], pic_org->stride_luma, cu_width, bit_depth);
            mecost_tmp += MV_COST(mv_bits);

            if (mecost_tmp < mecost) {
                mecost = mecost_tmp;
                mv[lidx_cur][MV_X] = mv_cand[lidx_cur][MV_X];
                mv[lidx_cur][MV_Y] = mv_cand[lidx_cur][MV_Y];

                mv[lidx_tar][MV_X] = mv_cand[lidx_tar][MV_X];
                mv[lidx_tar][MV_Y] = mv_cand[lidx_tar][MV_Y];

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
        direct_end = best_direct + step;
    }

    return mecost;
};

static void analyze_smvd(core_t *core, lbac_t *sbac_best)
{
    s16 mv[REFP_NUM][MV_D], mvp[REFP_NUM][MV_D], mvd[MV_D];
    com_info_t *info     =  core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi   = &core->pinter;
    int bit_depth        = info->bit_depth_internal;
    int x                = core->cu_pix_x;
    int y                = core->cu_pix_y;
    int log2_cuw         = core->cu_width_log2;
    int log2_cuh         = core->cu_height_log2;
    int cu_width         = 1 << log2_cuw;
    int cu_height        = 1 << log2_cuh;
    com_pic_t *pic_org   = core->pic_org;
    pel *org             = pic_org->y + x + y * pic_org->stride_luma;
    u32 lambda_mv        = pi->lambda_mv;

    init_inter_data(core);

    cur_info->cu_mode      = MODE_INTER;
    cur_info->smvd_flag    = 1;
    cur_info->refi[REFP_0] = 0;
    cur_info->refi[REFP_1] = 0;

    M32(mv[0]) = M32(mvp[REFP_0]) = M32(pi->mvp_scale[REFP_0][0]);
    M32(mv[1]) = M32(mvp[REFP_1]) = M32(pi->mvp_scale[REFP_1][0]);

    com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, cur_info->refi, mv, core->refp, cur_info->pred, cu_width, CHANNEL_L, bit_depth);

    u32 mecost = block_pel_satd(log2_cuw, log2_cuh, org, cur_info->pred[0], pic_org->stride_luma, cu_width, bit_depth);
    mecost += MV_COST(get_mv_bits_with_mvr(pi, 0, 0, cur_info->mvr_idx));

    s16 mv_bi[REFP_NUM][MV_D];
    CP32(mv_bi[REFP_0], pi->mv_scale[REFP_0][0]);
    mvd[MV_X] = mv_bi[REFP_0][MV_X] - mvp[REFP_0][MV_X];
    mvd[MV_Y] = mv_bi[REFP_0][MV_Y] - mvp[REFP_0][MV_Y];
    mv_bi[REFP_1][MV_X] = mvp[REFP_1][MV_X] - mvd[MV_X];
    mv_bi[REFP_1][MV_Y] = mvp[REFP_1][MV_Y] - mvd[MV_Y];

    com_mc_cu(x, y, info->pic_width, info->pic_height, cu_width, cu_height, cur_info->refi, mv_bi, core->refp, cur_info->pred, cu_width, CHANNEL_L, bit_depth);

    u32 mecost_bi = block_pel_satd(log2_cuw, log2_cuh, org, cur_info->pred[0], pic_org->stride_luma, cu_width, bit_depth);
    mecost_bi += MV_COST(get_mv_bits_with_mvr(pi, mvd[MV_X], mvd[MV_Y], cur_info->mvr_idx));

    if (mecost_bi < mecost) {
        mecost = mecost_bi;
        mv[0][MV_X] = mv_bi[0][MV_X];
        mv[0][MV_Y] = mv_bi[0][MV_Y];
        mv[1][MV_X] = mv_bi[1][MV_X];
        mv[1][MV_Y] = mv_bi[1][MV_Y];
    }

    // refine
    mecost = smvd_refine(core, x, y, log2_cuw, log2_cuh, mv, mvp, cur_info->refi, 0, 1, mecost, 2, 8, cur_info->mvr_idx);
    mecost = smvd_refine(core, x, y, log2_cuw, log2_cuh, mv, mvp, cur_info->refi, 0, 1, mecost, 0, 1, cur_info->mvr_idx);

    cur_info->mv [REFP_0][MV_X] = mv[REFP_0][MV_X];
    cur_info->mv [REFP_0][MV_Y] = mv[REFP_0][MV_Y];
    cur_info->mv [REFP_1][MV_X] = mv[REFP_1][MV_X];
    cur_info->mv [REFP_1][MV_Y] = mv[REFP_1][MV_Y];

    cur_info->mvd[REFP_0][MV_X] = mv[REFP_0][MV_X] - mvp[REFP_0][MV_X];
    cur_info->mvd[REFP_0][MV_Y] = mv[REFP_0][MV_Y] - mvp[REFP_0][MV_Y];
    cur_info->mvd[REFP_1][MV_X] = mv[REFP_1][MV_X] - mvp[REFP_1][MV_X];
    cur_info->mvd[REFP_1][MV_Y] = mv[REFP_1][MV_Y] - mvp[REFP_1][MV_Y];

    cur_info->mvd[REFP_1][MV_X] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, -cur_info->mvd[REFP_0][MV_X]);
    cur_info->mvd[REFP_1][MV_Y] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, -cur_info->mvd[REFP_0][MV_Y]);

    inter_rdcost(core, sbac_best, 0);
}

static void solve_equal(double(*equal_coeff)[7], int order, double *affine_para)
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

static int affine_mv_bits(inter_search_t *pi, CPMV mv[VER_NUM][MV_D], CPMV mvp[VER_NUM][MV_D], int num_refp, int refi, int vertex_num, u8 curr_mvr)
{
    int bits = tbl_refi_bits[num_refp][refi];
    u8  amvr_shift = Tab_Affine_AMVR(curr_mvr);

    for (int vertex = 0; vertex < vertex_num; vertex++) {
        int mvd_x = mv[vertex][MV_X] - mvp[vertex][MV_X];
        int mvd_y = mv[vertex][MV_Y] - mvp[vertex][MV_Y];

        if (mv[vertex][MV_X] != COM_CPMV_MAX && mvp[vertex][MV_X] != COM_CPMV_MAX) {
            assert(mvd_x == ((mvd_x >> amvr_shift) << amvr_shift));
        }
        if (mv[vertex][MV_Y] != COM_CPMV_MAX && mvp[vertex][MV_Y] != COM_CPMV_MAX) {
            assert(mvd_y == ((mvd_y >> amvr_shift) << amvr_shift));
        }
        bits += pi->tab_mvbits[mvd_x >> amvr_shift] + pi->tab_mvbits[mvd_y >> amvr_shift];
    }
    return bits;
}

static u64 affine_me_gradient(inter_search_t *pi, int x, int y, int cu_width_log2, int cu_height_log2, s8 *refi, int lidx, CPMV mvp[VER_NUM][MV_D], CPMV mv[VER_NUM][MV_D], int bi, int vertex_num, int sub_w, int sub_h)
{
    int bit_depth = pi->bit_depth;
    CPMV mvt[VER_NUM][MV_D];
    s16 mvd[VER_NUM][MV_D];
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    u64 cost, cost_best = COM_UINT64_MAX;
    s8 ri = *refi;
    com_pic_t *refp = pi->refp[ri][lidx].pic;
    ALIGNED_32(pel pred[MAX_CU_DIM]);
    pel *org = pi->org;
    int s_org = pi->i_org;
    int mv_bits, best_bits;
    int vertex, iter;
    int iter_num = bi ? AF_ITER_BI : AF_ITER_UNI;
    int para_num = (vertex_num << 1) + 1;
    int affine_param_num = para_num - 1;
    double affine_para[6];
    double delta_mv[6];
    s64    equal_coeff_t[7][7];
    double equal_coeff[7][7];
    ALIGNED_32(s16 error[MAX_CU_DIM]);
    ALIGNED_32(int derivate[2][MAX_CU_DIM]);
    u32 lambda_mv = pi->lambda_mv;

    cu_width  = 1 << cu_width_log2;
    cu_height = 1 << cu_height_log2;

    for (vertex = 0; vertex < vertex_num; vertex++) {
        mvt[vertex][MV_X] = mv[vertex][MV_X];
        mvt[vertex][MV_Y] = mv[vertex][MV_Y];
        mvd[vertex][MV_X] = 0;
        mvd[vertex][MV_Y] = 0;
    }

    com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, mvt, refp, pred, vertex_num, sub_w, sub_h, bit_depth);
    best_bits = affine_mv_bits(pi, mvt, mvp, pi->num_refp, ri, vertex_num, pi->curr_mvr);

    if (bi) {
        best_bits += pi->mot_bits[1 - lidx];
    }
    cost_best = MV_COST(best_bits);

    cost_best += calc_satd_16b(cu_width, cu_height, org, pred, s_org, cu_width, bit_depth) >> bi;
    if (vertex_num == 3) {
        iter_num = bi ? (AF_ITER_BI - 2) : (AF_ITER_UNI - 2);
    }
    for (iter = 0; iter < iter_num; iter++) {
        int row, col;
        int all_zero = 0;
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
        for (row = 0; row < para_num; row++) {
            com_mset(&equal_coeff_t[row][0], 0, para_num * sizeof(s64));
        }
        uavs3e_funs_handle.affine_coef_computer(error, cu_width, derivate, cu_width, equal_coeff_t, cu_width, cu_height, vertex_num);
        for (row = 0; row < para_num; row++) {
            for (col = 0; col < para_num; col++) {
                equal_coeff[row][col] = (double)equal_coeff_t[row][col];
            }
        }
        solve_equal(equal_coeff, affine_param_num, affine_para);
        // convert to delta mv
        if (vertex_num == 3) {
            delta_mv[0] = affine_para[0];
            delta_mv[2] = affine_para[2];
            delta_mv[1] = affine_para[1] * cu_width  + affine_para[0];
            delta_mv[3] = affine_para[3] * cu_width  + affine_para[2];
            delta_mv[4] = affine_para[4] * cu_height + affine_para[0];
            delta_mv[5] = affine_para[5] * cu_height + affine_para[2];

            u8 amvr_shift = Tab_Affine_AMVR(pi->curr_mvr);
            if (amvr_shift == 0) {
                mvd[0][MV_X] = (s16)(delta_mv[0] * 16 + (delta_mv[0] >= 0 ? 0.5 : -0.5)); //  1/16-pixel
                mvd[0][MV_Y] = (s16)(delta_mv[2] * 16 + (delta_mv[2] >= 0 ? 0.5 : -0.5));
                mvd[1][MV_X] = (s16)(delta_mv[1] * 16 + (delta_mv[1] >= 0 ? 0.5 : -0.5));
                mvd[1][MV_Y] = (s16)(delta_mv[3] * 16 + (delta_mv[3] >= 0 ? 0.5 : -0.5));
                mvd[2][MV_X] = (s16)(delta_mv[4] * 16 + (delta_mv[4] >= 0 ? 0.5 : -0.5));
                mvd[2][MV_Y] = (s16)(delta_mv[5] * 16 + (delta_mv[5] >= 0 ? 0.5 : -0.5));
            } else {
                mvd[0][MV_X] = (s16)(delta_mv[0] * 4 + (delta_mv[0] >= 0 ? 0.5 : -0.5));
                mvd[0][MV_Y] = (s16)(delta_mv[2] * 4 + (delta_mv[2] >= 0 ? 0.5 : -0.5));
                mvd[1][MV_X] = (s16)(delta_mv[1] * 4 + (delta_mv[1] >= 0 ? 0.5 : -0.5));
                mvd[1][MV_Y] = (s16)(delta_mv[3] * 4 + (delta_mv[3] >= 0 ? 0.5 : -0.5));
                mvd[2][MV_X] = (s16)(delta_mv[1] * 4 + (delta_mv[4] >= 0 ? 0.5 : -0.5));
                mvd[2][MV_Y] = (s16)(delta_mv[3] * 4 + (delta_mv[5] >= 0 ? 0.5 : -0.5));
                mvd[0][MV_X] <<= 2; // 1/16-pixel
                mvd[0][MV_Y] <<= 2;
                mvd[1][MV_X] <<= 2;
                mvd[1][MV_Y] <<= 2;
                mvd[2][MV_X] <<= 2;
                mvd[2][MV_Y] <<= 2;

                if (amvr_shift > 0) {
                    com_mv_rounding_s16(mvd[0][MV_X], mvd[0][MV_Y], &mvd[0][MV_X], &mvd[0][MV_Y], amvr_shift);
                    com_mv_rounding_s16(mvd[1][MV_X], mvd[1][MV_Y], &mvd[1][MV_X], &mvd[1][MV_Y], amvr_shift);
                    com_mv_rounding_s16(mvd[2][MV_X], mvd[2][MV_Y], &mvd[2][MV_X], &mvd[2][MV_Y], amvr_shift);
                }
            }
        } else {
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
        }
        // check early terminate
        for (vertex = 0; vertex < vertex_num; vertex++) {
            if (mvd[vertex][MV_X] != 0 || mvd[vertex][MV_Y] != 0) {
                all_zero = 0;
                break;
            }
            all_zero = 1;
        }
        if (all_zero) {
            break;
        }

        /* update mv */
        for (vertex = 0; vertex < vertex_num; vertex++) {
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
        com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, mvt, refp, pred, vertex_num, sub_w, sub_h, bit_depth);

        mv_bits = affine_mv_bits(pi, mvt, mvp, pi->num_refp, ri, vertex_num, pi->curr_mvr);
        if (bi) {
            mv_bits += pi->mot_bits[1 - lidx];
        }
        cost = MV_COST(mv_bits);
        cost += calc_satd_16b(cu_width, cu_height, org, pred, s_org, cu_width, bit_depth) >> bi;

        if (cost < cost_best) {
            cost_best = cost;
            best_bits = mv_bits;
            for (vertex = 0; vertex < vertex_num; vertex++) {
                mv[vertex][MV_X] = mvt[vertex][MV_X];
                mv[vertex][MV_Y] = mvt[vertex][MV_Y];
            }
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

static void analyze_affine_uni(core_t *core, lbac_t *sbac_best, CPMV aff_mv_L0L1[REFP_NUM][VER_NUM][MV_D], s8 *refi_L0L1, double *cost_L0L1)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi = &core->pinter;
    int bit_depth = info->bit_depth_internal;
    int lidx;
    s8 *refi;
    s8 refi_cur;
    u64 mecost, best_mecost;
    s8 t0, t1;
    s8 refi_temp = 0;
    CPMV(*affine_mvp)[MV_D], (*affine_mv)[MV_D];
    s16(*affine_mvd)[MV_D];
    int vertex = 0;
    int vertex_num = 2;
    int mebits, best_bits = 0;
    u64 cost_trans[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME];
    CPMV tmp_mv_array[VER_NUM][MV_D];
    int memory_access;
    int allowed = 1;
    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    int mem = MAX_MEMORY_ACCESS_UNI * (1 << cu_width_log2) * (1 << cu_height_log2);
    u32 lambda_mv = pi->lambda_mv;

    int sub_w = 4;
    int sub_h = 4;
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

    for (lidx = 0; lidx <= ((core->slice_type == SLICE_P) ? PRED_L0 : PRED_L1); lidx++) {
        init_inter_data(core);
        refi = cur_info->refi;
        affine_mv = cur_info->affine_mv[lidx];
        affine_mvd = cur_info->affine_mvd[lidx];
        pi->num_refp = (u8)core->num_refp[lidx];
        best_mecost = COM_UINT64_MAX;
        for (refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            com_map_t *map = core->map;
            affine_mvp = pi->affine_mvp_scale[lidx][refi_cur];
            com_get_affine_mvp_scaling(core->ptr, core->cu_scup_in_pic, lidx, refi_cur, map->map_mv, map->map_refi, core->refp,
            core->cu_width, core->cu_height, info->i_scu, affine_mvp,
            map->map_scu, map->map_pos, vertex_num, cur_info->mvr_idx);

            u64 mvp_best = COM_UINT64_MAX;
            u64 mvp_temp = COM_UINT64_MAX;
            s8  refi_t[REFP_NUM];
            com_pic_t *refp = core->refp[refi_cur][lidx].pic;
            ALIGNED_32(pel pred[MAX_CU_DIM]);
            com_pic_t *pic_org = core->pic_org;
            pel *org = pic_org->y + x + y * pic_org->stride_luma;
            int s_org = pic_org->stride_luma;
            pi->org = org;
            pi->i_org = s_org;

            com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, affine_mvp, refp, pred, vertex_num, sub_w, sub_h, bit_depth);
            mvp_best = calc_satd_16b(cu_width, cu_height, org, pred, s_org, cu_width, bit_depth);
            mebits = affine_mv_bits(pi, affine_mvp, affine_mvp, pi->num_refp, refi_cur, vertex_num, cur_info->mvr_idx);
            mvp_best += MV_COST(mebits);

            s16 mv_cliped[REFP_NUM][MV_D];
            mv_cliped[lidx][MV_X] = (s16)(pi->best_mv_uni[lidx][refi_cur][MV_X]);
            mv_cliped[lidx][MV_Y] = (s16)(pi->best_mv_uni[lidx][refi_cur][MV_Y]);

            refi_t[lidx] = refi_cur;
            refi_t[1 - lidx] = -1;
            mv_clip(x, y, info->pic_width, info->pic_height, cu_width, cu_height, refi_t, mv_cliped, mv_cliped);

            for (vertex = 0; vertex < vertex_num; vertex++) {
                tmp_mv_array[vertex][MV_X] = mv_cliped[lidx][MV_X];
                tmp_mv_array[vertex][MV_Y] = mv_cliped[lidx][MV_Y];

                s32 tmp_mvx, tmp_mvy;
                tmp_mvx = tmp_mv_array[vertex][MV_X] << 2;
                tmp_mvy = tmp_mv_array[vertex][MV_Y] << 2;
                if (cur_info->mvr_idx == 1) {
                    com_mv_rounding_s32(tmp_mvx, tmp_mvy, &tmp_mvx, &tmp_mvy, 4, 4);
                }
                tmp_mv_array[vertex][MV_X] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_mvx);
                tmp_mv_array[vertex][MV_Y] = (CPMV)COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, tmp_mvy);
            }
            com_mc_blk_affine_luma(x, y, refp->width_luma, refp->height_luma, cu_width, cu_height, tmp_mv_array, refp, pred, vertex_num, sub_w, sub_h, bit_depth);
            cost_trans[lidx][refi_cur] = calc_satd_16b(cu_width, cu_height, org, pred, s_org, cu_width, bit_depth);
            mebits = affine_mv_bits(pi, tmp_mv_array, affine_mvp, pi->num_refp, refi_cur, vertex_num, cur_info->mvr_idx);
            mvp_temp = cost_trans[lidx][refi_cur] + MV_COST(mebits);

            if (mvp_temp < mvp_best) {
                for (vertex = 0; vertex < vertex_num; vertex++) {
                    affine_mv[vertex][MV_X] = tmp_mv_array[vertex][MV_X];
                    affine_mv[vertex][MV_Y] = tmp_mv_array[vertex][MV_Y];
                }
            } else {
                for (vertex = 0; vertex < vertex_num; vertex++) {
                    affine_mv[vertex][MV_X] = affine_mvp[vertex][MV_X];
                    affine_mv[vertex][MV_Y] = affine_mvp[vertex][MV_Y];
                }
            }

            mecost = affine_me_gradient(pi, x, y, cu_width_log2, cu_height_log2, &refi_cur, lidx, affine_mvp, affine_mv, 0, vertex_num, sub_w, sub_h);

            t0 = (lidx == 0) ? refi_cur : REFI_INVALID;
            t1 = (lidx == 1) ? refi_cur : REFI_INVALID;
            SET_REFI(refi, t0, t1);
            mebits = affine_mv_bits(pi, affine_mv, affine_mvp, pi->num_refp, refi_cur, vertex_num, cur_info->mvr_idx);
            mecost += MV_COST(mebits);

            for (vertex = 0; vertex < vertex_num; vertex++) {
                pi->affine_mv_scale[lidx][refi_cur][vertex][MV_X] = affine_mv[vertex][MV_X];
                pi->affine_mv_scale[lidx][refi_cur][vertex][MV_Y] = affine_mv[vertex][MV_Y];
            }
            if (mecost < best_mecost) {
                best_mecost = mecost;
                best_bits = mebits;
                refi_temp = refi_cur;
            }
        }
        refi_cur = refi_temp;
        for (vertex = 0; vertex < vertex_num; vertex++) {
            affine_mv[vertex][MV_X] = pi->affine_mv_scale[lidx][refi_cur][vertex][MV_X];
            affine_mv[vertex][MV_Y] = pi->affine_mv_scale[lidx][refi_cur][vertex][MV_Y];
        }
        affine_mvp = pi->affine_mvp_scale[lidx][refi_cur];
        t0 = (lidx == 0) ? refi_cur : REFI_INVALID;
        t1 = (lidx == 1) ? refi_cur : REFI_INVALID;
        SET_REFI(refi, t0, t1);
        refi_L0L1[lidx] = refi_cur;

        for (vertex = 0; vertex < vertex_num; vertex++) {
            affine_mvd[vertex][MV_X] = affine_mv[vertex][MV_X] - affine_mvp[vertex][MV_X];
            affine_mvd[vertex][MV_Y] = affine_mv[vertex][MV_Y] - affine_mvp[vertex][MV_Y];
        }

        for (vertex = 0; vertex < vertex_num; vertex++) {
            int amvr_shift = Tab_Affine_AMVR(cur_info->mvr_idx);
            affine_mvd[vertex][MV_X] = affine_mvd[vertex][MV_X] >> amvr_shift << amvr_shift;
            affine_mvd[vertex][MV_Y] = affine_mvd[vertex][MV_Y] >> amvr_shift << amvr_shift;

            int mv_x = (s32)affine_mvd[vertex][MV_X] + affine_mvp[vertex][MV_X];
            int mv_y = (s32)affine_mvd[vertex][MV_Y] + affine_mvp[vertex][MV_Y];
            affine_mv[vertex][MV_X] = COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mv_x);
            affine_mv[vertex][MV_Y] = COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mv_y);
        }

        pi->mot_bits[lidx] = best_bits;
        affine_mv[2][MV_X] = affine_mv[0][MV_X] - (affine_mv[1][MV_Y] - affine_mv[0][MV_Y]) * (s16)cu_height / (s16)cu_width;
        affine_mv[2][MV_Y] = affine_mv[0][MV_Y] + (affine_mv[1][MV_X] - affine_mv[0][MV_X]) * (s16)cu_height / (s16)cu_width;
        affine_mv[3][MV_X] = affine_mv[1][MV_X] - (affine_mv[1][MV_Y] - affine_mv[0][MV_Y]) * (s16)cu_height / (s16)cu_width;
        affine_mv[3][MV_Y] = affine_mv[1][MV_Y] + (affine_mv[1][MV_X] - affine_mv[0][MV_X]) * (s16)cu_height / (s16)cu_width;

        for (vertex = 0; vertex < vertex_num; vertex++) {
            aff_mv_L0L1[lidx][vertex][MV_X] = affine_mv[vertex][MV_X];
            aff_mv_L0L1[lidx][vertex][MV_Y] = affine_mv[vertex][MV_Y];
        }
        allowed = 1;
        memory_access = com_get_affine_memory_access(affine_mv, cu_width, cu_height);
        if (memory_access > mem) {
            allowed = 0;
        }
        if (allowed) {
            cost_L0L1[lidx] = inter_rdcost(core, sbac_best, 0);
        }
    }
}

static void analyze_affine_bi(core_t *core, lbac_t *sbac_best, CPMV aff_mv_L0L1[REFP_NUM][VER_NUM][MV_D], const s8 *refi_L0L1, const double *cost_L0L1)
{
    com_info_t *info = core->info;
    com_mode_t *cur_info = &core->mod_info_curr;
    inter_search_t *pi = &core->pinter;
    com_pic_t *pic_org = core->pic_org;
    int bit_depth = info->bit_depth_internal;
    s8         refi[REFP_NUM] = { REFI_INVALID, REFI_INVALID };
    u64        best_mecost = COM_UINT64_MAX;
    s8        refi_best = 0, refi_cur;
    int        changed = 0;
    u64        mecost;
    pel        *org;
    pel(*pred)[MAX_CU_DIM];
    int        vertex_num = 2;
    int x = core->cu_pix_x;
    int y = core->cu_pix_y;
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    s8         t0, t1;
    double      cost;
    s8         lidx_ref, lidx_cnd;
    u8          mvp_idx = 0;
    int         i;
    int         vertex;
    int         mebits;
    int         memory_access[REFP_NUM];
    int         mem = MAX_MEMORY_ACCESS_BI * (1 << cu_width_log2) * (1 << cu_height_log2);
    u32 lambda_mv = pi->lambda_mv;

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
    for (vertex = 0; vertex < vertex_num; vertex++) {
        cur_info->affine_mv[lidx_ref][vertex][MV_X] = aff_mv_L0L1[lidx_ref][vertex][MV_X];
        cur_info->affine_mv[lidx_ref][vertex][MV_Y] = aff_mv_L0L1[lidx_ref][vertex][MV_Y];
        cur_info->affine_mv[lidx_cnd][vertex][MV_X] = aff_mv_L0L1[lidx_cnd][vertex][MV_X];
        cur_info->affine_mv[lidx_cnd][vertex][MV_Y] = aff_mv_L0L1[lidx_cnd][vertex][MV_Y];
    }

    org = pic_org->y + x + y * pic_org->stride_luma;
    pred = cur_info->pred;
    t0 = (lidx_ref == REFP_0) ? cur_info->refi[lidx_ref] : REFI_INVALID;
    t1 = (lidx_ref == REFP_1) ? cur_info->refi[lidx_ref] : REFI_INVALID;
    SET_REFI(refi, t0, t1);

    for (i = 0; i < AFFINE_BI_ITER; i++) {
        ALIGNED_32(pel org_bi[MAX_CU_DIM]);
        pi->org = org_bi;
        pi->i_org = cu_width;

        com_mc_blk_affine_luma(x, y, info->pic_width, info->pic_height, cu_width, cu_height, cur_info->affine_mv[lidx_ref], core->refp[refi[lidx_ref]][lidx_ref].pic, pred[Y_C], vertex_num, 8, 8, bit_depth);
        create_bi_org(org, pred[Y_C], pic_org->stride_luma, cu_width, cu_height, org_bi, cu_width, info->bit_depth_internal);
        SWAP(refi[lidx_ref], refi[lidx_cnd], t0);
        SWAP(lidx_ref, lidx_cnd, t0);
        changed = 0;
        pi->num_refp = (u8)core->num_refp[lidx_ref];

        for (refi_cur = 0; refi_cur < pi->num_refp; refi_cur++) {
            refi[lidx_ref] = refi_cur;
            mecost = affine_me_gradient(pi, x, y, cu_width_log2, cu_height_log2, &refi[lidx_ref], lidx_ref, \
            pi->affine_mvp_scale[lidx_ref][refi_cur], pi->affine_mv_scale[lidx_ref][refi_cur], 1, vertex_num, 8, 8);

            mebits = affine_mv_bits(pi, pi->affine_mv_scale[lidx_ref][refi_cur], pi->affine_mvp_scale[lidx_ref][refi_cur], pi->num_refp, refi_cur, vertex_num, cur_info->mvr_idx);
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
                for (vertex = 0; vertex < vertex_num; vertex++) {
                    cur_info->affine_mv[lidx_ref][vertex][MV_X] = pi->affine_mv_scale[lidx_ref][refi_cur][vertex][MV_X];
                    cur_info->affine_mv[lidx_ref][vertex][MV_Y] = pi->affine_mv_scale[lidx_ref][refi_cur][vertex][MV_Y];
                }
            }
        }
        t0 = (lidx_ref == REFP_0) ? refi_best : REFI_INVALID;
        t1 = (lidx_ref == REFP_1) ? refi_best : REFI_INVALID;
        SET_REFI(refi, t0, t1);
        if (!changed) {
            break;
        }
    }

    for (vertex = 0; vertex < vertex_num; vertex++) {
        cur_info->affine_mvd[REFP_0][vertex][MV_X] = cur_info->affine_mv[REFP_0][vertex][MV_X] - pi->affine_mvp_scale[REFP_0][cur_info->refi[REFP_0]][vertex][MV_X];
        cur_info->affine_mvd[REFP_0][vertex][MV_Y] = cur_info->affine_mv[REFP_0][vertex][MV_Y] - pi->affine_mvp_scale[REFP_0][cur_info->refi[REFP_0]][vertex][MV_Y];
        cur_info->affine_mvd[REFP_1][vertex][MV_X] = cur_info->affine_mv[REFP_1][vertex][MV_X] - pi->affine_mvp_scale[REFP_1][cur_info->refi[REFP_1]][vertex][MV_X];
        cur_info->affine_mvd[REFP_1][vertex][MV_Y] = cur_info->affine_mv[REFP_1][vertex][MV_Y] - pi->affine_mvp_scale[REFP_1][cur_info->refi[REFP_1]][vertex][MV_Y];
    }

    for (i = 0; i < REFP_NUM; i++) {
        for (vertex = 0; vertex < vertex_num; vertex++) {
            int amvr_shift = Tab_Affine_AMVR(cur_info->mvr_idx);
            cur_info->affine_mvd[i][vertex][MV_X] = cur_info->affine_mvd[i][vertex][MV_X] >> amvr_shift << amvr_shift;
            cur_info->affine_mvd[i][vertex][MV_Y] = cur_info->affine_mvd[i][vertex][MV_Y] >> amvr_shift << amvr_shift;

            int mv_x = (s32)cur_info->affine_mvd[i][vertex][MV_X] + pi->affine_mvp_scale[i][cur_info->refi[i]][vertex][MV_X];
            int mv_y = (s32)cur_info->affine_mvd[i][vertex][MV_Y] + pi->affine_mvp_scale[i][cur_info->refi[i]][vertex][MV_Y];
            cur_info->affine_mv[i][vertex][MV_X] = COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mv_x);
            cur_info->affine_mv[i][vertex][MV_Y] = COM_CLIP3(COM_CPMV_MIN, COM_CPMV_MAX, mv_y);
        }
    }

    for (i = 0; i < REFP_NUM; i++) {
        if (vertex_num == 3) {
            cur_info->affine_mv[i][3][MV_X] = cur_info->affine_mv[i][1][MV_X] + cur_info->affine_mv[i][2][MV_X] - cur_info->affine_mv[i][0][MV_X];
            cur_info->affine_mv[i][3][MV_Y] = cur_info->affine_mv[i][1][MV_Y] + cur_info->affine_mv[i][2][MV_Y] - cur_info->affine_mv[i][0][MV_Y];
        } else {
            cur_info->affine_mv[i][2][MV_X] = cur_info->affine_mv[i][0][MV_X] - (cur_info->affine_mv[i][1][MV_Y] - cur_info->affine_mv[i][0][MV_Y]) * (s16)cu_height / (s16)cu_width;
            cur_info->affine_mv[i][2][MV_Y] = cur_info->affine_mv[i][0][MV_Y] + (cur_info->affine_mv[i][1][MV_X] - cur_info->affine_mv[i][0][MV_X]) * (s16)cu_height / (s16)cu_width;
            cur_info->affine_mv[i][3][MV_X] = cur_info->affine_mv[i][1][MV_X] - (cur_info->affine_mv[i][1][MV_Y] - cur_info->affine_mv[i][0][MV_Y]) * (s16)cu_height / (s16)cu_width;
            cur_info->affine_mv[i][3][MV_Y] = cur_info->affine_mv[i][1][MV_Y] + (cur_info->affine_mv[i][1][MV_X] - cur_info->affine_mv[i][0][MV_X]) * (s16)cu_height / (s16)cu_width;
        }
        memory_access[i] = com_get_affine_memory_access(cur_info->affine_mv[i], cu_width, cu_height);
    }
    if (memory_access[0] > mem || memory_access[1] > mem) {
        return;
    } else {
        cost = inter_rdcost(core, sbac_best, 0);
    }
}

static int is_same_mv(core_t *core, com_motion_t hmvp_motion)
{
    com_info_t *info = core->info;
    com_motion_t c_motion;
    int x_scu = core->cu_scu_x;
    int y_scu = core->cu_scu_y;
    int cu_width_in_scu = core->cu_width >> MIN_CU_LOG2;
    com_map_t *map = core->map;
    com_scu_t *map_scu = map->map_scu;
    int cnt_hmvp = core->cnt_hmvp_cands;

    int neb_addr = core->cu_scup_in_pic - info->i_scu + cu_width_in_scu;
    int neb_avaliable = COM_IS_INTER_SCU(map_scu[neb_addr]);

    if (neb_avaliable && cnt_hmvp) {
        c_motion.mv[0][0] = map->map_mv[neb_addr][0][0];
        c_motion.mv[0][1] = map->map_mv[neb_addr][0][1];
        c_motion.mv[1][0] = map->map_mv[neb_addr][1][0];
        c_motion.mv[1][1] = map->map_mv[neb_addr][1][1];

        c_motion.ref_idx[0] = map->map_refi[neb_addr][0];
        c_motion.ref_idx[1] = map->map_refi[neb_addr][1];
        return same_motion(hmvp_motion, c_motion);
    }
    return 0;
}

void analyze_inter_cu(core_t *core, lbac_t *sbac_best)
{
    com_info_t *info = core->info;
    inter_search_t *pi = &core->pinter;
    com_mode_t *cur_info = &core->mod_info_curr;
    com_mode_t *bst_info = &core->mod_info_best;
    int bit_depth = info->bit_depth_internal;
    int i, j;
    ALIGNED_32(s16 coef_blk[N_C][MAX_CU_DIM]);
    ALIGNED_32(s16 resi[N_C][MAX_CU_DIM]);
    int cu_width_log2 = core->cu_width_log2;
    int cu_height_log2 = core->cu_height_log2;
    int cu_width = 1 << cu_width_log2;
    int cu_height = 1 << cu_height_log2;
    enc_history_t *p_bef_data = &core->bef_data[cu_width_log2 - 2][cu_height_log2 - 2][core->cu_scup_in_lcu];
    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);
    cur_info->mvr_idx = 0;

    int num_iter_mvp = 2;
    int num_hmvp_inter = MAX_NUM_MVR;
    int num_amvr = MAX_NUM_MVR;
    int allow_affine = info->sqh.affine_enable;

    if (p_bef_data->visit && info->sqh.emvr_enable) { 
        num_hmvp_inter = p_bef_data->mvr_hmvp_idx_history + 1;
        if (num_hmvp_inter > MAX_NUM_MVR) {
            num_hmvp_inter = MAX_NUM_MVR;
        }
    }
    if (info->sqh.amvr_enable) {
        if (p_bef_data->visit) {
            num_amvr = COM_MIN(MAX_NUM_MVR, p_bef_data->mvr_idx_history + 1);
        }
    } else {
        num_amvr = 1;
    }
    if (p_bef_data->visit && p_bef_data->affine_flag_history == 0) {
        allow_affine = 0;
    }

    if (cu_width *cu_height >= 64) {
        analyze_direct_skip(core, sbac_best);
    }

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
            analyze_uni_pred(core, sbac_best, cost_L0L1, mv_L0L1, refi_L0L1);

            if (core->slice_type == SLICE_B && cu_width *cu_height >= 64) {
                analyze_bi(core, sbac_best, mv_L0L1, refi_L0L1, cost_L0L1);

                if (info->sqh.smvd_enable && core->ptr - core->refp[0][REFP_0].ptr == core->refp[0][REFP_1].ptr - core->ptr && !cur_info->hmvp_flag) {
                    analyze_smvd(core, sbac_best);
                }
            }

            if (cur_info->hmvp_flag) {
                if (cur_info->mvr_idx >= SKIP_MVR_IDX + 1 && (bst_info->cu_mode == MODE_SKIP)) {
                    break;
                }
            } else if (cur_info->mvr_idx >= SKIP_MVR_IDX && ((bst_info->cu_mode == MODE_SKIP || bst_info->cu_mode == MODE_DIR))) {
                break;
            }

            if (cur_info->mvr_idx >= FAST_MVR_IDX) {
                if (abs(bst_info->mvd[REFP_0][MV_X]) <= 0 &&
                abs(bst_info->mvd[REFP_0][MV_Y]) <= 0 &&
                abs(bst_info->mvd[REFP_1][MV_X]) <= 0 &&
                abs(bst_info->mvd[REFP_1][MV_Y]) <= 0) {
                    break;
                }
            }
        }
    }

    double cost_L0L1[2] = { MAX_D_COST, MAX_D_COST };
    s8 refi_L0L1[2] = { REFI_INVALID, REFI_INVALID };
    CPMV aff_mv_L0L1[REFP_NUM][VER_NUM][MV_D];

    if (allow_affine && cu_width >= AFF_SIZE && cu_height >= AFF_SIZE) {
        analyze_affine_merge(core, sbac_best);
    }

    if (allow_affine && cu_width >= AFF_SIZE && cu_height >= AFF_SIZE) {
        if (!(bst_info->cu_mode == MODE_SKIP && !bst_info->affine_flag)) { 
            int num_affine_amvr = info->sqh.amvr_enable ? MAX_NUM_AFFINE_MVR : 1;
            for (cur_info->mvr_idx = 0; cur_info->mvr_idx < num_affine_amvr; cur_info->mvr_idx++) {
                pi->curr_mvr = cur_info->mvr_idx;
                analyze_affine_uni(core, sbac_best, aff_mv_L0L1, refi_L0L1, cost_L0L1);
                if (core->slice_type == SLICE_B) {
                    analyze_affine_bi(core, sbac_best, aff_mv_L0L1, refi_L0L1, cost_L0L1);
                }
            }
        }
    }

    /* reconstruct */
    int start_comp = (core->tree_status == TREE_L || core->tree_status == TREE_LC) ? Y_C : U_C;
    int num_comp = core->tree_status == TREE_LC ? 3 : (core->tree_status == TREE_L ? 1 : 2);
    for (j = start_comp; j < start_comp + num_comp; j++) {
        int size_tmp = (cu_width * cu_height) >> (j == 0 ? 0 : 2);
        com_mcpy(coef_blk[j], bst_info->coef[j], sizeof(s16) * size_tmp);
    }

    com_invqt_inter_yuv(bst_info, core->tree_status, coef_blk, resi, core->wq, cu_width_log2, cu_height_log2, core->lcu_qp_y, core->lcu_qp_u, core->lcu_qp_v, bit_depth);

    for (i = start_comp; i < start_comp + num_comp; i++) {
        int stride = (i == 0 ? cu_width : cu_width >> 1);
        com_recon_plane(i == Y_C ? bst_info->tb_part : SIZE_2Nx2N, resi[i], bst_info->pred[i], bst_info->num_nz, i, stride, (i == 0 ? cu_height : cu_height >> 1), stride, bst_info->rec[i], bit_depth);
    }

    if (!p_bef_data->visit) {
        p_bef_data->affine_flag_history = bst_info->affine_flag;
        p_bef_data->mvr_idx_history     = bst_info->mvr_idx;

        if (bst_info->hmvp_flag) {
            p_bef_data->mvr_hmvp_idx_history = bst_info->mvr_idx;
        }
    }
}

void inter_search_free(inter_search_t *pi)
{
    u8 *addr = pi->tab_mvbits - pi->tab_mvbits_offset;
    com_mfree(addr);
    pi->tab_mvbits = NULL;
}

void inter_search_create(inter_search_t *pi, int width, int height)
{
    pi->tab_mvbits_offset = (COM_MAX(width, height) << 2) + 3; // max abs(MV)
    pi->tab_mvbits = (u8 *)com_malloc(sizeof(u8 ) * (pi->tab_mvbits_offset * 2 + 1)) + pi->tab_mvbits_offset;

    pi->tab_mvbits[ 0] = 1;
    pi->tab_mvbits[-1] = pi->tab_mvbits[1] = 2;
    pi->tab_mvbits[-2] = pi->tab_mvbits[2] = 3;

    for (int exp_bits = 2; ; exp_bits++) {
        int imax = (1 <<  exp_bits     ) - 1;
        int imin = (1 << (exp_bits - 1)) - 1;
        int bits = exp_bits << 1;

        imax = COM_MIN(imax, pi->tab_mvbits_offset);

        for (int i = imin; i < imax; i++) {
            pi->tab_mvbits[-i] = pi->tab_mvbits[i] = bits;
        }
        if (imax == pi->tab_mvbits_offset) {
            break;
        }
    }
	
	pi->lambda_mv = 0;
}
