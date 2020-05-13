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
 *    This product includes software developed by the <organization>.
 * 4. Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
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
#include <math.h>

static double intra_pu_rdcost(core_t *core, lbac_t *lbac, pel rec[N_C][MAX_CU_DIM], pel *org_luma, pel *org_cb, pel *org_cr, int s_org, int s_org_c, int cu_width_log2, int cu_height_log2, s32 *dist, int bChroma, int pb_idx, int x, int y)
{
    com_mode_t *cur_info = &core->mod_info_curr;
    com_info_t *info     = core->info;
    com_pic_t *pic       = core->pic_rec;
    com_map_t *map       = core->map;
    int cu_width         = 1 << cu_width_log2;
    int cu_height        = 1 << cu_height_log2;
    int bit_depth        = info->bit_depth_internal;

    ALIGNED_32(s16 resi[MAX_CU_DIM]);
    double cost = 0;

    if (!bChroma) {
        int pb_part_size = cur_info->pb_part;
        int num_tb_in_pb = get_part_num_tb_in_pb(pb_part_size, pb_idx);
        int pb_w = cur_info->pb_info.sub_w[pb_idx];
        int pb_h = cur_info->pb_info.sub_h[pb_idx];
        int pb_x = cur_info->pb_info.sub_x[pb_idx];
        int pb_y = cur_info->pb_info.sub_y[pb_idx];
        int tb_w, tb_h, tb_x, tb_y;
        int use_alt4x4Trans = info->sqh.sectrans_enable;

        cu_plane_nz_cln(cur_info->num_nz, Y_C);
        get_tb_width_height_in_pb(pb_w, pb_h, pb_part_size, pb_idx, &tb_w, &tb_h);

        for (int tb_idx = 0; tb_idx < num_tb_in_pb; tb_idx++) {
            get_tb_pos_in_pb(pb_x, pb_y, pb_part_size, tb_w, tb_h, tb_idx, &tb_x, &tb_y);

            int coef_offset_tb  = tb_idx * tb_w * tb_h;
            int tb_x_scu        = PEL2SCU(tb_x);
            int tb_y_scu        = PEL2SCU(tb_y);
            int tb_scup         = tb_x_scu + (tb_y_scu * info->i_scu);
            s8  intra_mode      = cur_info->ipm[pb_idx][0];
            int tb_width_log2   = com_tbl_log2[tb_w];
            int tb_height_log2  = com_tbl_log2[tb_h];
            int use_secTrans    = use_alt4x4Trans && (tb_width_log2 > 2 || tb_height_log2 > 2);
            int secT_Ver_Hor    = 0;
            u16 avail_tb        = com_get_avail_intra(tb_x_scu, tb_y_scu, info->i_scu, tb_scup, map->map_scu);
            pel *rec_tb         = rec[Y_C] + (tb_y - core->cu_pix_y) * cu_width + (tb_x - core->cu_pix_x);
            pel *pred_tb;

            if (use_secTrans) {
                int vt = IS_AVAIL(avail_tb, AVAIL_UP) && (intra_mode < IPD_HOR);
                int ht = IS_AVAIL(avail_tb, AVAIL_LE) && ((intra_mode > IPD_VER && intra_mode < IPD_CNT) || intra_mode <= IPD_BI);
                secT_Ver_Hor = (vt << 1) | ht;
            }
            if (num_tb_in_pb == 1) {
                pred_tb = core->intra_pred_all[intra_mode];
            } else {
                int i_rec_y = pic->stride_luma;
                pel *mod = pic->y + (tb_y * i_rec_y) + tb_x;
                pred_tb = cur_info->pred[Y_C];
                com_intra_get_nbr(tb_x, tb_y, tb_w, tb_h, mod, i_rec_y, core->linebuf_intra[1][0] + tb_x, info->max_cuwh, avail_tb, core->nb, tb_scup, map->map_scu, info->i_scu, bit_depth, Y_C);
                com_intra_pred(core->nb[Y_C] + INTRA_NEIB_MID, pred_tb, intra_mode, tb_w, tb_h, bit_depth, avail_tb, cur_info->ipf_flag);
            }

            pel *org_luma_tb = org_luma + (tb_x - pb_x) + (tb_y - pb_y) * s_org;
            s16 *coef_tb = cur_info->coef[Y_C] + coef_offset_tb;

            block_pel_sub(tb_width_log2, tb_height_log2, org_luma_tb, pred_tb, s_org, tb_w, tb_w, coef_tb);
            cur_info->num_nz[tb_idx][Y_C] = enc_tq_nnz(core, cur_info, Y_C, 0, core->lcu_qp_y, core->lambda[0], coef_tb, coef_tb, tb_width_log2, tb_height_log2, core->slice_type, 1, secT_Ver_Hor, use_alt4x4Trans);

            if (cur_info->num_nz[tb_idx][Y_C]) {
                com_invqt(cur_info, Y_C, 0, coef_tb, resi, core->wq, tb_width_log2, tb_height_log2, core->lcu_qp_y, bit_depth, secT_Ver_Hor, use_alt4x4Trans);
            }

            // call com_recon_plane() as a CU
            int num_nz_temp[MAX_NUM_TB][N_C];
            num_nz_temp[TB0][Y_C] = cur_info->num_nz[tb_idx][Y_C];
            com_recon_plane(SIZE_2Nx2N, resi, pred_tb, num_nz_temp, Y_C, tb_w, tb_h, cu_width, rec_tb, bit_depth);

            if (cur_info->pb_part == SIZE_2Nx2N) {
                cost += calc_dist_filter_boundary(core, pic, core->pic_org, cu_width, cu_height, rec[Y_C], cu_width, x, y, 1, cur_info->num_nz[TB0][Y_C] != 0, NULL, NULL, 0, 0);
            } else {
                cost += block_pel_ssd(tb_width_log2, tb_h, rec_tb, org_luma_tb, cu_width, s_org, bit_depth); 
            }
            update_intra_info_map_scu(map->map_scu, map->map_ipm, pb_x, pb_y, pb_w, pb_h, info->i_scu, intra_mode);
            copy_rec_y_to_pic(rec_tb, tb_x, tb_y, tb_w, tb_h, cu_width, pic);
        }

        *dist = (s32)cost;
        if (pb_idx == 0) {
            lbac_copy(lbac, &core->lbac_bakup);
        } else {
            lbac_copy(lbac, &core->lbac_intra_prev_pu);
        }
        int bit_cnt = lbac_get_bits(lbac);
        enc_bits_intra_pu(core, lbac, core->pichdr->slice_type, cur_info->coef, pb_idx);
        bit_cnt = lbac_get_bits(lbac) - bit_cnt;
        cost += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
    } else {
        int i_rec_y = pic->stride_luma;
        pel *piRecoY = pic->y + (y * pic->stride_luma) + x;
        ALIGNED_32(pel pred[N_C][MAX_CU_DIM]);
        u16 avail_cu = com_get_avail_intra(core->cu_scu_x, core->cu_scu_y, info->i_scu, core->cu_scup_in_pic, map->map_scu);

        // U plane
        com_intra_pred_chroma(pred[U_C], cur_info->ipm[PB0][1], cur_info->ipm[PB0][0], cu_width >> 1, cu_height >> 1, bit_depth, avail_cu, U_C, piRecoY, i_rec_y, core->nb);
        block_pel_sub(cu_width_log2 - 1, cu_height_log2 - 1, org_cb, pred[U_C], s_org_c, cu_width >> 1, cu_width >> 1, cur_info->coef[U_C]);
        cur_info->num_nz[TB0][U_C] = enc_tq_nnz(core, cur_info, U_C, 0, core->lcu_qp_u, core->lambda[1], cur_info->coef[U_C], cur_info->coef[U_C], cu_width_log2 - 1, cu_height_log2 - 1, core->slice_type, 1, 0, 0);
        if (cur_info->num_nz[TB0][U_C]) {
            com_invqt(cur_info, U_C, 0, cur_info->coef[U_C], resi, core->wq, core->cu_width_log2 - 1, core->cu_height_log2 - 1, core->lcu_qp_u, bit_depth, 0, 0);
        }
        com_recon_plane(SIZE_2Nx2N, resi, pred[U_C], cur_info->num_nz, U_C, cu_width >> 1, cu_height >> 1, cu_width >> 1, rec[U_C], bit_depth);
        
        // V plane
        com_intra_pred_chroma(pred[V_C], cur_info->ipm[PB0][1], cur_info->ipm[PB0][0], cu_width >> 1, cu_height >> 1, bit_depth, avail_cu, V_C, piRecoY, i_rec_y, core->nb);
        block_pel_sub(cu_width_log2 - 1, cu_height_log2 - 1, org_cr, pred[V_C], s_org_c, cu_width >> 1, cu_width >> 1, cur_info->coef[V_C]);
        cur_info->num_nz[TB0][V_C] = enc_tq_nnz(core, cur_info, V_C, 0, core->lcu_qp_v, core->lambda[2], cur_info->coef[V_C], cur_info->coef[V_C], cu_width_log2 - 1, cu_height_log2 - 1, core->slice_type, 1, 0, 0);
        if (cur_info->num_nz[TB0][V_C]) {
            com_invqt(cur_info, V_C, 0, cur_info->coef[V_C], resi, core->wq, core->cu_width_log2 - 1, core->cu_height_log2 - 1, core->lcu_qp_v, bit_depth, 0, 0);
        }
        com_recon_plane(SIZE_2Nx2N, resi, pred[V_C], cur_info->num_nz, V_C, cu_width >> 1, cu_height >> 1, cu_width >> 1, rec[V_C], bit_depth);

        // calc rdcost
        lbac_copy(lbac, &core->lbac_intra_prev_pu);
        int bit_cnt = lbac_get_bits(lbac);
        enc_bits_intra_chroma(core, lbac, cur_info->coef);
        bit_cnt = lbac_get_bits(lbac) - bit_cnt;
        cost += core->dist_chroma_weight[0] * block_pel_ssd(cu_width_log2 - 1, cu_height >> 1, rec[U_C], org_cb, cu_width >> 1, s_org_c, bit_depth);
        cost += core->dist_chroma_weight[1] * block_pel_ssd(cu_width_log2 - 1, cu_height >> 1, rec[V_C], org_cr, cu_width >> 1, s_org_c, bit_depth);
        *dist = (s32)cost;

        if (core->tree_status != TREE_C) {
            cost += block_pel_ssd(cu_width_log2, cu_height, rec[Y_C], org_luma, cu_width, s_org, bit_depth);
        }
        cost += RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
    }
    return cost;
}

static void com_update_cand_list(const int ipm, u64 cost_satd, double cost, int ipred_list_len, int *rmd_ipred_list, u64 *rmd_cand_satd, double *rmd_cand_cost)
{
	int shift = 0;
    double *pcand = rmd_cand_cost + ipred_list_len - 1;

	while (shift < ipred_list_len && cost < *pcand--) {
		shift++;
	}
	if (shift--) {
        rmd_ipred_list += ipred_list_len - 1;
        rmd_cand_satd  += ipred_list_len - 1;
        rmd_cand_cost  += ipred_list_len - 1;

		while (shift--) {
			*rmd_ipred_list = rmd_ipred_list[-1];
            *rmd_cand_satd  = rmd_cand_satd [-1];
            *rmd_cand_cost  = rmd_cand_cost [-1];
            rmd_ipred_list--;
            rmd_cand_satd --;
            rmd_cand_cost --;
		}
		*rmd_ipred_list = ipm;
		*rmd_cand_satd  = cost_satd;
		*rmd_cand_cost  = cost;
	}
}

static void check_one_mode(core_t *core, pel *org, int s_org, int ipm, int ipred_list_len, int *rmd_ipred_list, u64 *rmd_cand_satd, double *rmd_cand_cost, int part_idx, int pb_w, int pb_h, u16 avail_cu)
{
	com_mode_t *cur_info = &core->mod_info_curr;
	int bit_depth        =  core->info->bit_depth_internal;
	pel *pred_buf        =  core->intra_pred_all[ipm];
    
	com_intra_pred(core->nb[Y_C] + INTRA_NEIB_MID, pred_buf, ipm, pb_w, pb_h, bit_depth, avail_cu, cur_info->ipf_flag);

	u64 satd = calc_satd_intra(pb_w, pb_h, org, pred_buf, s_org, pb_w, bit_depth);

    lbac_t lbac_temp;
    lbac_copy(&lbac_temp, &core->lbac_bakup);
    int bit_cnt = lbac_get_bits(&lbac_temp);
	lbac_enc_intra_dir(&lbac_temp, NULL, (u8)ipm, cur_info->mpm[part_idx]);
	bit_cnt = lbac_get_bits(&lbac_temp) - bit_cnt;

	double cost = satd + RATE_TO_COST_SQRT_LAMBDA(core->sqrt_lambda[0], bit_cnt);
	com_update_cand_list(ipm, satd, cost, ipred_list_len, rmd_ipred_list, rmd_cand_satd, rmd_cand_cost);
}

static int make_ipred_list(core_t *core, int pb_width, int pb_height, int cu_width_log2, int cu_height_log2, pel *org, int s_org, int *ipred_list, int part_idx, u16 avail_cu, int skip_ipd)
{
	int ipd_rdo_cnt = (pb_width >= pb_height * 4 || pb_height >= pb_width * 4) ? IPD_RDO_CNT - 1 : IPD_RDO_CNT;
    com_mode_t *cur_info = &core->mod_info_curr;

	double rmd_cand_cost[10];
	u64    rmd_cand_satd[10];

    if (SPEED_LEVEL(1, core->param->speed_level)) {
        int       ipm_check_map[IPD_CNT] = { 0 };
        const int rmd_range_4[10] = { 0,1,2,6,10,14,18,22,26,30 };
        const int num_cand_4_in = 10;
        const int num_cand_4_out = 7;

        for (int i = 0; i < num_cand_4_in; i++) {
            ipred_list[i] = IPD_DC;
            rmd_cand_satd[i] = COM_UINT64_MAX;
            rmd_cand_cost[i] = MAX_D_COST;
            ipm_check_map[rmd_range_4[i]] = 1;
        }
	    for (int i = 0; i < num_cand_4_in; i++) {
            int mode = rmd_range_4[i];
		    if (skip_ipd == 1 && (mode == IPD_PLN || mode == IPD_BI || mode == IPD_DC)) {
			    continue;
		    }
		    check_one_mode(core, org, s_org, mode, i + 1, ipred_list, rmd_cand_satd, rmd_cand_cost, part_idx, pb_width, pb_height, avail_cu);
	    }

        int rmd_range_2[12] = { 0 };
        int num_cand_2_in  = 0;
        int num_cand_2_out = 3;

	    for (int rmd_idx = 0; rmd_idx < num_cand_4_out; rmd_idx++) {
		    int ipm = ipred_list[rmd_idx];
		    int ipm_sub_2 = ipm - 2;
		    int ipm_add_2 = ipm + 2;

		    if (ipm >= 6) {
			    if (ipm_check_map[ipm_sub_2] == 0) {
				    rmd_range_2[num_cand_2_in++] = ipm_sub_2;
				    ipm_check_map[ipm_sub_2] = 1;
			    }
			    if (ipm_check_map[ipm_add_2] == 0) {
				    rmd_range_2[num_cand_2_in++] = ipm_add_2;
				    ipm_check_map[ipm_add_2] = 1;
			    }
		    }
	    }
	    for (int i = 0; i < num_cand_2_in; i++) {
		    check_one_mode(core, org, s_org, rmd_range_2[i], COM_MAX(ipd_rdo_cnt, num_cand_2_out), ipred_list, rmd_cand_satd, rmd_cand_cost, part_idx, pb_width, pb_height, avail_cu);
	    }

        int rmd_range_1[8] = { 0 };
        int num_cand_1_in = 0;

	    for (int rmd_idx = 0, i = 0; rmd_idx < num_cand_2_out && i < num_cand_4_in;) {
		    int ipm = ipred_list[i++];
		    int ipm_sub_1 = ipm - 1;
		    int ipm_add_1 = ipm + 1;

		    if (ipm >= 4) {
			    rmd_idx++;
				if (ipm_check_map[ipm_sub_1] == 0) {
				    rmd_range_1[num_cand_1_in++] = ipm_sub_1;
				    ipm_check_map[ipm_sub_1] = 1;
				}
				if (ipm_check_map[ipm_add_1] == 0 && ipm_add_1 < IPD_CNT) {
				    rmd_range_1[num_cand_1_in++] = ipm_add_1;
				    ipm_check_map[ipm_add_1] = 1;
				}
		    }
	    }
	    for (int i = 0; i < num_cand_1_in; i++) {
		    check_one_mode(core, org, s_org, rmd_range_1[i], COM_MIN(ipd_rdo_cnt, IPD_RDO_CNT), ipred_list, rmd_cand_satd, rmd_cand_cost, part_idx, pb_width, pb_height, avail_cu);
	    }
	    for (int mpmidx = 0; mpmidx < 2; mpmidx++) {
		    int cur_mpm = cur_info->mpm[part_idx][mpmidx];
		    if (cur_mpm < 33) {
			    if (skip_ipd == 1 && (cur_mpm == IPD_PLN || cur_mpm == IPD_BI || cur_mpm == IPD_DC)) {
				    continue;
			    }
			    if (ipm_check_map[cur_mpm] == 0) {
				    check_one_mode(core, org, s_org, cur_mpm, COM_MIN(ipd_rdo_cnt, IPD_RDO_CNT), ipred_list, rmd_cand_satd, rmd_cand_cost, part_idx, pb_width, pb_height, avail_cu);
			    }
		    }
	    }
    } else {
        for (int i = 0; i < ipd_rdo_cnt; i++) {
            rmd_cand_satd[i] = COM_UINT64_MAX;
            rmd_cand_cost[i] = MAX_D_COST;
        }
        for (int i = 0; i < IPD_CNT; i++) {
            if (skip_ipd == 1 && (i == IPD_PLN || i == IPD_BI || i == IPD_DC)) {
                continue;
            }
            check_one_mode(core, org, s_org, i, ipd_rdo_cnt, ipred_list, rmd_cand_satd, rmd_cand_cost, part_idx, pb_width, pb_height, avail_cu);
        }
    }
	for (int i = ipd_rdo_cnt - 1; i >= 0; i--) {
		if (rmd_cand_satd[i] > core->inter_satd *(1.1)) {
            ipd_rdo_cnt--;
		} else {
			break;
		}
	}
	return ipd_rdo_cnt;

}
void analyze_intra_cu(core_t *core, lbac_t *lbac_best)
{
    com_pic_t *pic_rec   = core->pic_rec;
    com_pic_t *pic_org   = core->pic_org;
    com_mode_t *bst_info = &core->mod_info_best;
    com_mode_t *cur_info = &core->mod_info_curr;
    com_map_t *map       = core->map;
    com_info_t *info     = core->info;
    int x                = core->cu_pix_x;
    int y                = core->cu_pix_y;
    int cu_width_log2    = core->cu_width_log2;
    int cu_height_log2   = core->cu_height_log2;
    int cu_width         = 1 << cu_width_log2;
    int cu_height        = 1 << cu_height_log2;

    int bit_depth = info->bit_depth_internal;
    int best_ipd_c = IPD_INVALID;
    s32 best_dist_y = 0, best_dist_c = 0;
    int best_ipd           [MAX_NUM_TB] = { IPD_INVALID, IPD_INVALID, IPD_INVALID, IPD_INVALID };
    s32 best_dist_y_pb_part[MAX_NUM_TB] = { 0 };
    u8  best_mpm_pb_part   [MAX_NUM_TB][2];
    u8  best_mpm           [MAX_NUM_TB][2];
    int  num_nz_y_pb_part  [MAX_NUM_TB];

    ALIGNED_32(s16 coef_y_pb_part[MAX_CU_DIM]);
    ALIGNED_32(pel rec_y_pb_part [MAX_CU_DIM]);
    int ipm_l2c = 0;
    int chk_bypass = 0;
    int bit_cnt = 0;
    int ipred_list[IPD_CNT];
    int pred_cnt = IPD_CNT;
    double cost_temp, cost_best = MAX_D_COST;
    double cost_pb_temp, cost_pb_best;
    int s_mod = pic_rec->stride_luma;
    int s_org = pic_org->stride_luma;
    pel *mod  = pic_rec->y + (y * s_mod) + x;
    pel *org  = pic_org->y + (y * s_org) + x;
    int s_mod_c = pic_rec->stride_chroma;
    int s_org_c = pic_org->stride_chroma;

    int ipd_add[4][3] = { { IPD_INVALID, IPD_INVALID, IPD_INVALID }, { IPD_INVALID, IPD_INVALID, IPD_INVALID },
        { IPD_INVALID, IPD_INVALID, IPD_INVALID }, { IPD_INVALID, IPD_INVALID, IPD_INVALID }
    };

#if DT_SAVE_LOAD
    enc_history_t *pData = &core->history_data[cu_width_log2 - 2][cu_height_log2 - 2][core->cu_scup_in_lcu];
#endif
    ALIGNED_32(pel rec[N_C][MAX_CU_DIM]);
    lbac_t lbac_temp;
    lbac_t *lbac = &lbac_temp;

    init_pb_part(cur_info);
    init_tb_part(cur_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);

    core->best_pb_part_intra = SIZE_2Nx2N;

    if (core->tree_status == TREE_LC || core->tree_status == TREE_L) {
        int allowed_part_size[7] = { SIZE_2Nx2N };
        int num_allowed_part_size = 1;
        int dt_allow = info->sqh.dt_enable ? com_dt_allow(core->cu_width, core->cu_height, MODE_INTRA, info->sqh.max_dt_size) : 0;
#if DT_INTRA_BOUNDARY_FILTER_OFF
        if (cur_info->ipf_flag) {
            dt_allow = 0;
        }
#endif
        if (dt_allow & 0x1) { // horizontal
            allowed_part_size[num_allowed_part_size++] = SIZE_2NxhN;
        }
        if (dt_allow & 0x2) { // vertical
            allowed_part_size[num_allowed_part_size++] = SIZE_hNx2N;
        }
        if (dt_allow & 0x1) { // horizontal
            allowed_part_size[num_allowed_part_size++] = SIZE_2NxnU;
            allowed_part_size[num_allowed_part_size++] = SIZE_2NxnD;
        }
        if (dt_allow & 0x2) { // vertical
            allowed_part_size[num_allowed_part_size++] = SIZE_nLx2N;
            allowed_part_size[num_allowed_part_size++] = SIZE_nRx2N;
        }

        cost_best = MAX_D_COST;
#if DT_INTRA_FAST_BY_RD
        u8 try_non_2NxhN = 1, try_non_hNx2N = 1;
        double cost_2Nx2N = MAX_D_COST, cost_hNx2N = MAX_D_COST, cost_2NxhN = MAX_D_COST;
#endif
        for (int part_size_idx = 0; part_size_idx < num_allowed_part_size; part_size_idx++) {
            part_size_t pb_part_size = allowed_part_size[part_size_idx];
            part_size_t tb_part_size = get_tb_part_size_by_pb(pb_part_size, MODE_INTRA);
            int best_ipd_pb_part[MAX_NUM_TB];

            set_pb_part(cur_info, pb_part_size);
            set_tb_part(cur_info, tb_part_size);
            get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, pb_part_size, &cur_info->pb_info);
            get_part_info(info->i_scu, core->cu_pix_x, core->cu_pix_y, core->cu_width, core->cu_height, tb_part_size, &cur_info->tb_info);
            assert(cur_info->pb_info.sub_scup[0] == core->cu_scup_in_pic);
            cost_temp = 0;
            memset(num_nz_y_pb_part, 0, MAX_NUM_TB * sizeof(int));

#if DT_INTRA_FAST_BY_RD
            if (((pb_part_size == SIZE_2NxnU || pb_part_size == SIZE_2NxnD) && !try_non_2NxhN)
                || ((pb_part_size == SIZE_nLx2N || pb_part_size == SIZE_nRx2N) && !try_non_hNx2N)) {
                continue;
            }
#endif
#if DT_SAVE_LOAD
            if (pb_part_size != SIZE_2Nx2N && pData->num_intra_history > 1 && pData->best_part_size_intra[0] == SIZE_2Nx2N && pData->best_part_size_intra[1] == SIZE_2Nx2N) {
                break;
            }
#endif

            for (int pb_part_idx = 0; pb_part_idx < cur_info->pb_info.num_sub_part /*&& cost_temp <= core->cost_best*/; pb_part_idx++) {
                int pb_x = cur_info->pb_info.sub_x[pb_part_idx];
                int pb_y = cur_info->pb_info.sub_y[pb_part_idx];
                int pb_w = cur_info->pb_info.sub_w[pb_part_idx];
                int pb_h = cur_info->pb_info.sub_h[pb_part_idx];
                int pb_scup = cur_info->pb_info.sub_scup[pb_part_idx];
                int pb_x_scu = PEL2SCU(pb_x);
                int pb_y_scu = PEL2SCU(pb_y);
                int pb_coef_offset = get_coef_offset_tb(core->cu_pix_x, core->cu_pix_y, pb_x, pb_y, cu_width, cu_height, tb_part_size);
                int tb_idx_offset = get_tb_idx_offset(pb_part_size, pb_part_idx);
                int num_tb_in_pb = get_part_num_tb_in_pb(pb_part_size, pb_part_idx);
                int skip_ipd = 0;
                if (((pb_part_size == SIZE_2NxnU || pb_part_size == SIZE_nLx2N) && pb_part_idx == 1) ||
                    ((pb_part_size == SIZE_2NxnD || pb_part_size == SIZE_nRx2N) && pb_part_idx == 0)) {
                    skip_ipd = 1;
                }
                cost_pb_best = MAX_D_COST;
                cu_nz_cln(cur_info->num_nz);

                mod = pic_rec->y + (pb_y * s_mod) + pb_x;
                org = pic_org->y + (pb_y * s_org) + pb_x;

                u16 avail_cu = com_get_avail_intra(pb_x_scu, pb_y_scu, info->i_scu, pb_scup, map->map_scu);
                com_intra_get_nbr(pb_x, pb_y, pb_w, pb_h, mod, s_mod, core->linebuf_intra[1][0] + pb_x, info->max_cuwh, avail_cu, core->nb, pb_scup, map->map_scu, info->i_scu, bit_depth, Y_C);
                com_intra_get_mpm(pb_x_scu, pb_y_scu, map->map_scu, map->map_ipm, pb_scup, info->i_scu, cur_info->mpm[pb_part_idx]);

                pred_cnt = make_ipred_list(core, pb_w, pb_h, cu_width_log2, cu_height_log2, org, s_org, ipred_list, pb_part_idx, avail_cu, skip_ipd);

                if (skip_ipd == 1) {
                    static tab_s8 ipd_add_idx_tab[8] = { 0, 0, 0, 0, 0, 1, 2, 3 };
                    if (ipd_add[ipd_add_idx_tab[pb_part_size]][0] == 1) {
                        ipred_list[pred_cnt++] = IPD_PLN;
                    }
                    if (ipd_add[ipd_add_idx_tab[pb_part_size]][1] == 1) {
                        ipred_list[pred_cnt++] = IPD_BI;
                    }
                    if (ipd_add[ipd_add_idx_tab[pb_part_size]][2] == 1) {
                        ipred_list[pred_cnt++] = IPD_DC;
                    }
                }
                if (pred_cnt == 0) {
                    return;
                }
                for (int j = 0; j < pred_cnt; j++) { /* Y */
                    s32 dist_t = 0;
                    cur_info->ipm[pb_part_idx][0] = (s8)ipred_list[j];
                    cur_info->ipm[pb_part_idx][1] = IPD_INVALID;
                    cost_pb_temp = intra_pu_rdcost(core, lbac, rec, org, NULL, NULL, s_org, s_org_c, cu_width_log2, cu_height_log2, &dist_t, 0, pb_part_idx, x, y);

                    if (cost_pb_temp < cost_pb_best) {
                        cost_pb_best = cost_pb_temp;
                        best_dist_y_pb_part[pb_part_idx] = dist_t;
                        best_ipd_pb_part   [pb_part_idx] = ipred_list[j];
                        best_mpm_pb_part   [pb_part_idx][0] = cur_info->mpm[pb_part_idx][0];
                        best_mpm_pb_part   [pb_part_idx][1] = cur_info->mpm[pb_part_idx][1];

                        com_mcpy(coef_y_pb_part + pb_coef_offset, cur_info->coef[Y_C], pb_w * pb_h * sizeof(s16));
                        for (int j = 0; j < pb_h; j++) {
                            int rec_offset = ((pb_y - y) + j) * cu_width + (pb_x - x);
                            com_mcpy(rec_y_pb_part + rec_offset, rec[Y_C] + rec_offset, pb_w * sizeof(pel));
                        }
                        for (int tb_idx = 0; tb_idx < num_tb_in_pb; tb_idx++) {
                            num_nz_y_pb_part[tb_idx + tb_idx_offset] = cur_info->num_nz[tb_idx][Y_C];
                        }
                        lbac_copy(&core->lbac_intra_prev_pu, lbac);
                    }
                }
                cost_temp += cost_pb_best;
            
                //update PU map
                update_intra_info_map_scu(map->map_scu, map->map_ipm, pb_x, pb_y, pb_w, pb_h, info->i_scu, best_ipd_pb_part[pb_part_idx]);
                copy_rec_y_to_pic(rec_y_pb_part + (pb_y - y) * cu_width + (pb_x - x), pb_x, pb_y, pb_w, pb_h, cu_width, pic_rec);
            }

            if (pb_part_size == SIZE_2NxhN || pb_part_size == SIZE_hNx2N) {
                int mem_offset = pb_part_size == SIZE_hNx2N ? 2 : 0;
                if (best_ipd_pb_part[1] == IPD_PLN || best_ipd_pb_part[2] == IPD_PLN || best_ipd_pb_part[3] == IPD_PLN) {
                    ipd_add[mem_offset + 0][0] = 1;
                }
                if (best_ipd_pb_part[1] == IPD_BI || best_ipd_pb_part[2] == IPD_BI || best_ipd_pb_part[3] == IPD_BI) {
                    ipd_add[mem_offset + 0][1] = 1;
                }
                if (best_ipd_pb_part[1] == IPD_DC || best_ipd_pb_part[2] == IPD_DC || best_ipd_pb_part[3] == IPD_DC) {
                    ipd_add[mem_offset + 0][2] = 1;
                }

                if (best_ipd_pb_part[0] == IPD_PLN || best_ipd_pb_part[1] == IPD_PLN || best_ipd_pb_part[2] == IPD_PLN) {
                    ipd_add[mem_offset + 1][0] = 1;
                }
                if (best_ipd_pb_part[0] == IPD_BI || best_ipd_pb_part[1] == IPD_BI || best_ipd_pb_part[2] == IPD_BI) {
                    ipd_add[mem_offset + 1][1] = 1;
                }
                if (best_ipd_pb_part[0] == IPD_DC || best_ipd_pb_part[1] == IPD_DC || best_ipd_pb_part[2] == IPD_DC) {
                    ipd_add[mem_offset + 1][2] = 1;
                }
            }

            com_mcpy(rec[Y_C], rec_y_pb_part, cu_width * cu_height * sizeof(pel));

            if (cur_info->pb_part != SIZE_2Nx2N) {
                int cbf_y = num_nz_y_pb_part[0] + num_nz_y_pb_part[1] + num_nz_y_pb_part[2] + num_nz_y_pb_part[3];
                s64 delta_dist = calc_dist_filter_boundary(core, pic_rec, pic_org, cu_width, cu_height, rec[Y_C], cu_width, x, y, 1, cbf_y, NULL, NULL, 0, 1);
                cost_temp += delta_dist;
                best_dist_y_pb_part[PB0] += (s32)delta_dist; 
            }

#if DT_INTRA_FAST_BY_RD
            if (pb_part_size == SIZE_2Nx2N) {
                cost_2Nx2N = cost_temp;
            } else if (pb_part_size == SIZE_2NxhN) {
                cost_2NxhN = cost_temp;
                assert(cost_2Nx2N < MAX_D_COST);
                if (cost_2NxhN > cost_2Nx2N * 1.05) {
                    try_non_2NxhN = 0;
                }
            } else if (pb_part_size == SIZE_hNx2N) {
                cost_hNx2N = cost_temp;
                assert(cost_2Nx2N < MAX_D_COST);
                if (cost_hNx2N > cost_2Nx2N * 1.05) {
                    try_non_hNx2N = 0;
                }
            }

            if (cost_hNx2N < MAX_D_COST && cost_2NxhN < MAX_D_COST) {
                if (cost_hNx2N > cost_2NxhN * 1.1) {
                    try_non_hNx2N = 0;
                } else if (cost_2NxhN > cost_hNx2N * 1.1) {
                    try_non_2NxhN = 0;
                }
            }
#endif
            //save best pu type
            if (cost_temp < cost_best) {
                cost_best = cost_temp;
                best_dist_y = 0;
                for (int pb_idx = 0; pb_idx < cur_info->pb_info.num_sub_part; pb_idx++) {
                    best_dist_y += best_dist_y_pb_part[pb_idx];
                    best_ipd[pb_idx] = best_ipd_pb_part[pb_idx];
                    best_mpm[pb_idx][0] = best_mpm_pb_part[pb_idx][0];
                    best_mpm[pb_idx][1] = best_mpm_pb_part[pb_idx][1];
                }
                com_mcpy(bst_info->coef[Y_C], coef_y_pb_part, cu_width * cu_height * sizeof(s16));
                com_mcpy(bst_info->rec[Y_C], rec_y_pb_part, cu_width * cu_height * sizeof(pel));
                assert(cur_info->pb_info.num_sub_part <= cur_info->tb_info.num_sub_part);
                for (int tb_idx = 0; tb_idx < MAX_NUM_TB; tb_idx++) {
                    bst_info->num_nz[tb_idx][Y_C] = num_nz_y_pb_part[tb_idx];
                }
                core->best_pb_part_intra = cur_info->pb_part;
                core->best_tb_part_intra = cur_info->tb_part;
                lbac_copy(&core->lbac_tree_c, &core->lbac_intra_prev_pu);
            }
        }
        pel *src = bst_info->rec[Y_C];
        pel *dst = pic_rec->y + (y * s_mod) + x;
        for (int h = 0; h < cu_height; h++) {
            com_mcpy(dst, src, cu_width * sizeof(pel));
            dst += s_mod;
            src += cu_width;
        }
    }

    if (core->tree_status == TREE_LC || core->tree_status == TREE_C) { // Chroma RDO
        u16 avail_cu = com_get_avail_intra(core->cu_scu_x, core->cu_scu_y, info->i_scu, core->cu_scup_in_pic, map->map_scu);
        pel *mod_cb = pic_rec->u + ((y >> 1) * s_mod_c) + (x >> 1);
        pel *mod_cr = pic_rec->v + ((y >> 1) * s_mod_c) + (x >> 1);
        pel *org_cb = pic_org->u + ((y >> 1) * s_org_c) + (x >> 1);
        pel *org_cr = pic_org->v + ((y >> 1) * s_org_c) + (x >> 1);
        org = pic_org->y + (y * s_org) + x;
        mod = pic_rec->y + (y * s_mod) + x;

        lbac_copy(&core->lbac_intra_prev_pu, &core->lbac_tree_c);

        com_intra_get_nbr(x,      y,      cu_width,      cu_height,      mod,    s_mod  , core->linebuf_intra[1][0] +  x      , info->max_cuwh     , avail_cu, core->nb, core->cu_scup_in_pic, map->map_scu, info->i_scu, bit_depth, Y_C);
        com_intra_get_nbr(x >> 1, y >> 1, cu_width >> 1, cu_height >> 1, mod_cb, s_mod_c, core->linebuf_intra[1][1] + (x >> 1), info->max_cuwh >> 1, avail_cu, core->nb, core->cu_scup_in_pic, map->map_scu, info->i_scu, bit_depth, U_C);
        com_intra_get_nbr(x >> 1, y >> 1, cu_width >> 1, cu_height >> 1, mod_cr, s_mod_c, core->linebuf_intra[1][2] + (x >> 1), info->max_cuwh >> 1, avail_cu, core->nb, core->cu_scup_in_pic, map->map_scu, info->i_scu, bit_depth, V_C);

        cost_best = MAX_D_COST;

        if (core->tree_status == TREE_C) {
            assert(cu_width >= 8 && cu_height >= 8);
            int luma_scup = PEL2SCU(x + (cu_width - 1)) + PEL2SCU(y + (cu_height - 1)) * info->i_scu;
            best_ipd[PB0] = map->map_ipm[luma_scup];
            assert((best_ipd[PB0] >= 0 && best_ipd[PB0] < IPD_CNT) || best_ipd[PB0] == IPD_IPCM);
            assert(map->map_scu[luma_scup].intra);
        }
        ipm_l2c = best_ipd[PB0];
        cur_info->ipm[PB0][0] = (s8)best_ipd[PB0];
        COM_IPRED_CONV_L2C_CHK(ipm_l2c, chk_bypass);

        for (int i = 0; i < IPD_CHROMA_CNT + 1; i++) { /* UV */
            s32 dist_t = 0;
            cur_info->ipm[PB0][1] = (s8)i;
            if (i != IPD_DM_C && chk_bypass && i == ipm_l2c) {
                continue;
            }
            if (i == IPD_DM_C && best_ipd[PB0] == IPD_IPCM) {
                continue;
            }
            if (!info->sqh.tscpm_enable && i == IPD_TSCPM_C) {
                continue;
            }

            cost_temp = intra_pu_rdcost(core, lbac, rec, org, org_cb, org_cr, s_org, s_org_c, cu_width_log2, cu_height_log2, &dist_t, 1, 0, x, y);

            if (cost_temp < cost_best) {
                cost_best = cost_temp;
                best_dist_c = dist_t;
                best_ipd_c = i;
                for (int j = U_C; j < N_C; j++) {
                    int size_tmp = (cu_width * cu_height) >> (j == 0 ? 0 : 2);
                    com_mcpy(bst_info->coef[j], cur_info->coef[j], size_tmp * sizeof(s16));
                    com_mcpy(bst_info->rec[j], rec[j], size_tmp * sizeof(pel));
                    bst_info->num_nz[TB0][j] = cur_info->num_nz[TB0][j];
                }
            }
        }
    }

    int pb_part_size_best = core->best_pb_part_intra;
    int tb_part_size_best = get_tb_part_size_by_pb(pb_part_size_best, MODE_INTRA);
    int num_pb_best = get_part_num(pb_part_size_best);
    int num_tb_best = get_part_num(tb_part_size_best);

    if (core->tree_status == TREE_LC) {
        assert(best_ipd_c != IPD_INVALID);
    } else if (core->tree_status == TREE_L) {
        assert(bst_info->num_nz[TBUV0][U_C] == 0);
        assert(bst_info->num_nz[TBUV0][V_C] == 0);
        assert(best_dist_c == 0);
    } else if (core->tree_status == TREE_C) {
        assert(best_ipd_c != IPD_INVALID);
        assert(bst_info->num_nz[TBUV0][Y_C] == 0);
        assert(num_pb_best == 1 && num_tb_best == 1);
        assert(best_dist_y == 0);
    } else {
        assert(0);
    }

    for (int pb_part_idx = 0; pb_part_idx < num_pb_best; pb_part_idx++) {
        bst_info->mpm[pb_part_idx][0] = best_mpm[pb_part_idx][0];
        bst_info->mpm[pb_part_idx][1] = best_mpm[pb_part_idx][1];
        bst_info->ipm[pb_part_idx][0] = cur_info->ipm[pb_part_idx][0] = (s8)best_ipd[pb_part_idx];
        bst_info->ipm[pb_part_idx][1] = cur_info->ipm[pb_part_idx][1] = (s8)best_ipd_c;
    }
    for (int tb_part_idx = 0; tb_part_idx < num_tb_best; tb_part_idx++) {
        for (int j = 0; j < N_C; j++) {
            cur_info->num_nz[tb_part_idx][j] = bst_info->num_nz[tb_part_idx][j];
        }
    }
    cur_info->pb_part = core->best_pb_part_intra;
    cur_info->tb_part = core->best_tb_part_intra;
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->pb_part, &cur_info->pb_info);
    get_part_info(info->i_scu, core->cu_scu_x << 2, core->cu_scu_y << 2, cu_width, cu_height, cur_info->tb_part, &cur_info->tb_info);

    lbac_copy(lbac, &core->lbac_bakup);
    bit_cnt = lbac_get_bits(lbac);
    enc_bits_intra(core, lbac, core->pichdr->slice_type, bst_info->coef);
    bit_cnt = lbac_get_bits(lbac) - bit_cnt;
    cost_best = (best_dist_y + best_dist_c) + RATE_TO_COST_LAMBDA(core->lambda[0], bit_cnt);
    core->dist_cu = best_dist_y + best_dist_c;

    if (cost_best < core->cost_best) {
        core->cost_best = cost_best;
        if (lbac_best) {
            lbac_copy(lbac_best, lbac);
        }
    }

#if DT_SAVE_LOAD
    if (pData->num_intra_history < 2 && cur_info->ipf_flag == 0 && core->tree_status != TREE_C) {
        pData->best_part_size_intra[pData->num_intra_history++] = core->best_pb_part_intra;
    }
#endif
}
