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

static double sao_rdcost_merge(core_t *core, const lbac_t *lbac, int lcu_pos, int pix_y, int pix_x,
                        double *sao_labmda, com_sao_stat_t saostatData[N_C][NUM_SAO_NEW_TYPES], com_sao_param_t(*sao_blk_params)[N_C],
                        com_sao_param_t sao_cur_param[N_C], int *MergeLeftAvail, int *MergeUpAvail)
{
    com_info_t *info = core->info;
    com_sao_param_t merge_candidate[NUM_SAO_MERGE_TYPES][N_C];
    com_sao_param_t temp_sao_param[N_C];
    int mb_x = pix_x >> MIN_CU_LOG2;
    int mb_y = pix_y >> MIN_CU_LOG2;
    int merge_avail[NUM_SAO_MERGE_TYPES];
    int mergeIdx, compIdx;
    double curdist = 0;
    int currate;
    double curcost, mincost;
    mincost = MAX_D_COST;
    getSaoMergeNeighbor(info, core->map->map_patch, info->pic_width_in_scu, info->pic_width_in_lcu, lcu_pos, mb_y, mb_x, sao_blk_params, merge_avail, merge_candidate);
    *MergeLeftAvail = merge_avail[SAO_MERGE_LEFT];
    *MergeUpAvail = merge_avail[SAO_MERGE_ABOVE];

    for (mergeIdx = 0; mergeIdx < NUM_SAO_MERGE_TYPES; mergeIdx++) {
        if (merge_avail[mergeIdx]) {
            lbac_t sao_lbac_tmp;
            lbac_copy(&sao_lbac_tmp, lbac);

            curdist = 0;
            copySAOParam_for_blk(temp_sao_param, merge_candidate[mergeIdx]);

            for (compIdx = Y_C; compIdx < N_C; compIdx++) {
                int type = merge_candidate[mergeIdx][compIdx].typeIdc;
                temp_sao_param[compIdx].modeIdc = mergeIdx == SAO_MERGE_LEFT ? SAO_MODE_MERGE_LEFT : SAO_MODE_MERGE_ABOVE;
                if (type != -1) {
                    curdist += (com_sao_get_dist(compIdx, type, saostatData, temp_sao_param)) / sao_labmda[compIdx];
                }
            }

            currate = lbac_get_bits(&sao_lbac_tmp);
            lbac_enc_sao_mrg_flag(&sao_lbac_tmp, NULL, *MergeLeftAvail, *MergeUpAvail, temp_sao_param);
            currate = lbac_get_bits(&sao_lbac_tmp) - currate;
            curcost = currate + curdist;

            if (curcost < mincost) {
                mincost = curcost;
                copySAOParam_for_blk(sao_cur_param, temp_sao_param);
            }
        }
    }
    return mincost;
}

static int sao_offset_estimation(int typeIdx, int classIdx, double lambda, int offset_ori, int count, long long int diff, double *bestCost)
{
    int cur_offset = offset_ori;
    int offset_best = 0;
    int lower_bd, upper_bd, Th;
    int temp_offset, start_offset, end_offset;
    int temprate;
    long long int tempdist;
    double tempcost, mincost;
    tab_u8 *eo_offset_bins = &(tbl_sao_eo_offset_map[1]);
    int offset_type;
    int offset_step;
    if (typeIdx == SAO_TYPE_BO) {
        offset_type = SAO_CLASS_BO;
    } else {
        offset_type = classIdx;
    }
    lower_bd = tbl_sao_bound_clip[offset_type][0];
    upper_bd = tbl_sao_bound_clip[offset_type][1];
    Th = tbl_sao_bound_clip[offset_type][2];
    offset_step = 1;

    cur_offset = COM_CLIP3(lower_bd, upper_bd, cur_offset);


    if (typeIdx == SAO_TYPE_BO) {
        start_offset = cur_offset >= 0 ? 0 : cur_offset;
        end_offset = cur_offset >= 0 ? cur_offset : 0;
    } else {
        assert(typeIdx >= SAO_TYPE_EO_0 && typeIdx <= SAO_TYPE_EO_45);
        switch (classIdx) {
        case SAO_CLASS_EO_FULL_VALLEY:
            start_offset = -1;
            end_offset = COM_MAX(cur_offset, 1);
            break;
        case SAO_CLASS_EO_HALF_VALLEY:
            start_offset = 0;
            end_offset = 1;
            break;
        case SAO_CLASS_EO_HALF_PEAK:
            start_offset = -1;
            end_offset = 0;
            break;
        case SAO_CLASS_EO_FULL_PEAK:
            start_offset = COM_MIN(cur_offset, -1);
            end_offset = 1;
            break;
        default:
            printf("Not a supported SAO mode\n");
            assert(0);
            exit(-1);
        }
    }
    mincost = MAX_D_COST;
    for (temp_offset = start_offset; temp_offset <= end_offset; temp_offset += offset_step) {
        int offset = temp_offset;
        assert(offset >= -7 && offset <= 7);
        if (typeIdx == SAO_TYPE_BO) {
            assert(offset_type == SAO_CLASS_BO);
            temprate = abs(offset);
            temprate = temprate ? (temprate + 1) : 0;
        } else if (classIdx == SAO_CLASS_EO_HALF_VALLEY || classIdx == SAO_CLASS_EO_HALF_PEAK) {
            temprate = abs(offset);
        } else {
            temprate = eo_offset_bins[classIdx == SAO_CLASS_EO_FULL_VALLEY ? offset : -offset];
        }
        temprate = (temprate == Th) ? temprate : (temprate + 1);

        tempdist = distortion_cal(count, temp_offset, diff);
        tempcost = (double)tempdist + lambda * (double)temprate;
        if (tempcost < mincost) {
            mincost = tempcost;
            offset_best = temp_offset;
            *bestCost = tempcost;
        }
    }
    return offset_best;
}

static void sao_find_offset(int compIdx, int typeIdc, com_sao_stat_t saostatData[N_C][NUM_SAO_NEW_TYPES], com_sao_param_t saoBlkParam[N_C], double lambda)
{
    int class_i;
    double classcost[MAX_NUM_SAO_CLASSES];
    double mincost_bandsum, cost_bandsum;
    int start_band1, start_band2, delta_band12;
    int sb_temp[2];
    int db_temp;

    if (typeIdc == SAO_TYPE_BO) {
        int startBand, startBand2;
        int offset[MAX_NUM_SAO_CLASSES];

        for (class_i = 0; class_i < NUM_SAO_BO_CLASSES; class_i++) {
            if (saostatData[compIdx][typeIdc].count[class_i] == 0) { //count == 0, offset will be zero
                offset[class_i] = 0;
                continue;
            }
            double offth = saostatData[compIdx][typeIdc].diff[class_i] > 0 ? 0.5 : (saostatData[compIdx][typeIdc].diff[class_i] < 0 ? -0.5 : 0);
            offset[class_i] = (int)((double)saostatData[compIdx][typeIdc].diff[class_i] / (double)saostatData[compIdx][typeIdc].count[class_i] + offth);
        }
        for (class_i = 0; class_i < NUM_SAO_BO_CLASSES; class_i++) {
            offset[class_i] = sao_offset_estimation(typeIdc, class_i, lambda, offset[class_i], saostatData[compIdx][typeIdc].count[class_i], saostatData[compIdx][typeIdc].diff[class_i], &(classcost[class_i]));
        }

        mincost_bandsum = 1.7e+308;

        for (start_band1 = 0; start_band1 < (NUM_SAO_BO_CLASSES - 1); start_band1++) {
            for (start_band2 = start_band1 + 2; start_band2 < (NUM_SAO_BO_CLASSES - 1); start_band2++) {
                static tab_s8 deltaband_cost[17] = { -1, -1, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 16 };

                cost_bandsum = classcost[start_band1] + classcost[start_band1 + 1] + classcost[start_band2] + classcost[start_band2 + 1];
                delta_band12 = (start_band2 - start_band1) > (NUM_SAO_BO_CLASSES >> 1) ? (32 - start_band2 + start_band1) : (start_band2 - start_band1);
                assert(delta_band12 >= 0 && delta_band12 <= (NUM_SAO_BO_CLASSES >> 1));
                cost_bandsum += lambda * (double)deltaband_cost[delta_band12];
                if (cost_bandsum < mincost_bandsum) {
                    mincost_bandsum = cost_bandsum;
                    startBand = start_band1;
                    startBand2 = start_band2;
                }
            }
        }

        sb_temp[0] = COM_MIN(startBand, startBand2);
        sb_temp[1] = COM_MAX(startBand, startBand2);
        db_temp = (sb_temp[1] - sb_temp[0]);

        if (db_temp > (NUM_SAO_BO_CLASSES >> 1)) {
            saoBlkParam[compIdx].deltaband = 32 - db_temp;
            startBand = sb_temp[1];
            startBand2 = sb_temp[0];
        } else {
            saoBlkParam[compIdx].deltaband = db_temp;
            startBand  = sb_temp[0];
            startBand2 = sb_temp[1];
        }

        saoBlkParam[compIdx].bandIdx[0] = startBand;
        saoBlkParam[compIdx].bandIdx[1] = startBand + 1;
        saoBlkParam[compIdx].bandIdx[2] = startBand2;
        saoBlkParam[compIdx].bandIdx[3] = startBand2 + 1;

        saoBlkParam[compIdx].offset[0] = offset[startBand];
        saoBlkParam[compIdx].offset[1] = offset[startBand + 1];
        saoBlkParam[compIdx].offset[2] = offset[startBand2];
        saoBlkParam[compIdx].offset[3] = offset[startBand2 + 1];

    } else {
        for (class_i = 0; class_i < NUM_SAO_EO_CLASSES; class_i++) {
            if (class_i == SAO_CLASS_EO_PLAIN) { //EO - PLAIN
                saoBlkParam[compIdx].offset[class_i] = 0;
                continue;
            }
            if (saostatData[compIdx][typeIdc].count[class_i] == 0) { //count == 0, offset will be zero
                saoBlkParam[compIdx].offset[class_i] = 0;
                continue;
            }
            double offth = saostatData[compIdx][typeIdc].diff[class_i] > 0 ? 0.5 : (saostatData[compIdx][typeIdc].diff[class_i] < 0 ? -0.5 : 0);
            saoBlkParam[compIdx].offset[class_i] = (int)((double)saostatData[compIdx][typeIdc].diff[class_i] / (double)saostatData[compIdx][typeIdc].count[class_i] + offth);
        }

        assert(typeIdc >= SAO_TYPE_EO_0 && typeIdc <= SAO_TYPE_EO_45);

        for (class_i = 0; class_i < NUM_SAO_EO_CLASSES; class_i++) { // LOOP EO 5 types (find best offset for each type)
            if (class_i == SAO_CLASS_EO_PLAIN) {
                saoBlkParam[compIdx].offset[class_i] = 0;
                classcost[class_i] = 0;
            } else {
                saoBlkParam[compIdx].offset[class_i] = sao_offset_estimation(typeIdc, class_i, lambda, saoBlkParam[compIdx].offset[class_i],
                                                       saostatData[compIdx][typeIdc].count[class_i], saostatData[compIdx][typeIdc].diff[class_i], &(classcost[class_i]));
            }
        }
    }
}

static double sao_rdcost_new(core_t *core, lbac_t *sao_sbac, int MergeLeftAvail, int MergeUpAvail, double *sao_lambda, com_sao_stat_t saostatData[N_C][NUM_SAO_NEW_TYPES], com_sao_param_t sao_cur_param[N_C])
{
    com_patch_header_t *pathdr = core->pathdr;
    int compIdx;
    int type;
    long long int mindist[N_C], curdist[N_C];
    int  minrate[N_C], currate[N_C];
    double curcost, mincost;
    double normmodecost;
    com_sao_param_t temp_sao_param[N_C];
    int mergeflag_rate;
    mergeflag_rate = 0;
    sao_cur_param[Y_C].modeIdc = SAO_MODE_NEW;
    sao_cur_param[Y_C].typeIdc = -1;

    if (MergeLeftAvail + MergeUpAvail) {
        mergeflag_rate = lbac_get_bits(sao_sbac);
        lbac_enc_sao_mrg_flag(sao_sbac, NULL, MergeLeftAvail, MergeUpAvail, &(sao_cur_param[Y_C]));
        mergeflag_rate = lbac_get_bits(sao_sbac) - mergeflag_rate;
    }
    for (compIdx = Y_C; compIdx < N_C; compIdx++) {
        lbac_t sao_lbac_backup;
        lbac_copy(&sao_lbac_backup, sao_sbac);
        minrate[compIdx] = lbac_get_bits(sao_sbac);
        sao_cur_param[compIdx].modeIdc = SAO_MODE_NEW;
        sao_cur_param[compIdx].typeIdc = -1;
        lbac_enc_sao_mode(sao_sbac, NULL, &(sao_cur_param[compIdx]));
        minrate[compIdx] = lbac_get_bits(sao_sbac) - minrate[compIdx];
        mindist[compIdx] = 0;
        mincost = RATE_TO_COST_LAMBDA(sao_lambda[compIdx], minrate[compIdx]);

        if (pathdr->slice_sao_enable[compIdx]) {
            for (type = 0; type < NUM_SAO_NEW_TYPES; type++) {
                lbac_t sao_lbac_tmp;
                lbac_copy(&sao_lbac_tmp, &sao_lbac_backup);
                temp_sao_param[compIdx].modeIdc = SAO_MODE_NEW;
                temp_sao_param[compIdx].typeIdc = type;
                sao_find_offset(compIdx, type, saostatData, temp_sao_param, sao_lambda[compIdx]);
                curdist[compIdx] = com_sao_get_dist(compIdx, type, saostatData, temp_sao_param);
                currate[compIdx] = lbac_get_bits(&sao_lbac_tmp);
                lbac_enc_sao_mode(&sao_lbac_tmp, NULL, &(temp_sao_param[compIdx]));
                lbac_enc_sao_offset(&sao_lbac_tmp, NULL, &(temp_sao_param[compIdx]));
                lbac_enc_sao_type(&sao_lbac_tmp, NULL, &(temp_sao_param[compIdx]));
                currate[compIdx] = lbac_get_bits(&sao_lbac_tmp) - currate[compIdx];
                curcost = (double)(curdist[compIdx]) + RATE_TO_COST_LAMBDA(sao_lambda[compIdx], currate[compIdx]);
                if (curcost < mincost) {
                    mincost = curcost;
                    minrate[compIdx] = currate[compIdx];
                    mindist[compIdx] = curdist[compIdx];
                    copySAOParam_for_blk_onecomponent(&sao_cur_param[compIdx], &temp_sao_param[compIdx]);
                    lbac_copy(sao_sbac, &sao_lbac_tmp);
                }
            }
        } else {
            mindist[compIdx] = 0;
            minrate[compIdx] = 0;
        }
    }
    normmodecost = (double)(mindist[Y_C] + mindist[U_C] + mindist[V_C]) / sao_lambda[Y_C];
    normmodecost += minrate[Y_C] + minrate[U_C] + minrate[V_C] + mergeflag_rate;
    return normmodecost;
}

static void sao_get_lcu_param(core_t *core, const lbac_t *lbac, int lcu_pos, int pix_x, int pix_y,
                       com_sao_stat_t saostatData[N_C][NUM_SAO_NEW_TYPES], com_sao_param_t(*saoBlkParam_map)[N_C], double *sao_labmda)
{
    com_patch_header_t *pathdr = core->pathdr;
    com_sao_param_t *saoBlkParam = saoBlkParam_map[lcu_pos];
    int MergeLeftAvail = 0;
    int MergeUpAvail = 0;
    double mcost, mincost;
    com_sao_param_t sao_cur_param[N_C];
    lbac_t sao_lbac_new;

    if (!pathdr->slice_sao_enable[Y_C] && !pathdr->slice_sao_enable[U_C] && !pathdr->slice_sao_enable[V_C]) {
        off_sao(saoBlkParam);
        return;
    }

    mincost = sao_rdcost_merge(core, lbac, lcu_pos, pix_y, pix_x, sao_labmda, saostatData, saoBlkParam_map, sao_cur_param, &MergeLeftAvail, &MergeUpAvail);

    copySAOParam_for_blk(saoBlkParam, sao_cur_param);

    lbac_copy(&sao_lbac_new, lbac);

    mcost = sao_rdcost_new(core, &sao_lbac_new, MergeLeftAvail, MergeUpAvail, sao_labmda, saostatData, sao_cur_param);

    if (mcost < mincost) {
        mincost = mcost;
        copySAOParam_for_blk(saoBlkParam, sao_cur_param);
    }
}

void sao_get_statistics(com_info_t *info, com_patch_header_t *pathdr, com_map_t *map, com_pic_t  *pic_org, com_pic_t  *pic_rec, int pix_x, int pix_y, int lcu_pix_width, int lcu_pix_height, com_sao_stat_t saostatData[N_C][NUM_SAO_NEW_TYPES])
{
    int bit_depth = info->bit_depth_internal;
    int isAboveAvail = pix_y > 0;
    int isBelowAvail = pix_y + lcu_pix_height < info->pic_height;
    int isLeftAvail  = pix_x > 0;
    int isRightAvail = pix_x + lcu_pix_width < info->pic_width;
    int x_offset = 0, y_offset = 0, width_add = 0, height_add = 0;
    int w, h, x, y;

    if (isLeftAvail) {
        x_offset  -= SAO_SHIFT_PIX_NUM;
        width_add += SAO_SHIFT_PIX_NUM;
    }
    if (isRightAvail) {
        width_add -= SAO_SHIFT_PIX_NUM;
    }
    if (isBelowAvail) {
        height_add -= SAO_SHIFT_PIX_NUM;
    }
    if (isAboveAvail) {
        y_offset   -= SAO_SHIFT_PIX_NUM - 1; // -1 for keep const of WPP
        height_add += SAO_SHIFT_PIX_NUM - 1;
    }

    if (pathdr->slice_sao_enable[0]) {
        w = lcu_pix_width  + width_add;
        h = lcu_pix_height + height_add;
        x = pix_x + x_offset;
        y = pix_y + y_offset;
        uavs3e_funs_handle.sao_stat(pic_org, pic_rec, saostatData[0], bit_depth, 0, x, y, w, h, isLeftAvail, isRightAvail, isAboveAvail, isBelowAvail);
    }

    w = (lcu_pix_width  >> 1) + width_add;
    h = (lcu_pix_height >> 1) + height_add;
    x = (pix_x >> 1) + x_offset;
    y = (pix_y >> 1) + y_offset;

    if (pathdr->slice_sao_enable[1]) {
        uavs3e_funs_handle.sao_stat(pic_org, pic_rec, saostatData[1], bit_depth, 1, x, y, w, h, isLeftAvail, isRightAvail, isAboveAvail, isBelowAvail);
    }
    if (pathdr->slice_sao_enable[2]) {
        uavs3e_funs_handle.sao_stat(pic_org, pic_rec, saostatData[2], bit_depth, 2, x, y, w, h, isLeftAvail, isRightAvail, isAboveAvail, isBelowAvail);
    }
}

void enc_sao_rdo(core_t *core, const lbac_t *lbac)
{
    com_info_t *info = core->info;
    com_map_t *map = core->map;
    int bit_depth = info->bit_depth_internal;

    int lcu_x = core->lcu_pix_x;
    int lcu_y = core->lcu_pix_y;
    int lcuw = COM_MIN(1 << info->log2_max_cuwh, info->pic_width  - lcu_x);
    int lcuh = COM_MIN(1 << info->log2_max_cuwh, info->pic_height - lcu_y);
    int lcu_pos = core->lcu_x + core->lcu_y * info->pic_width_in_lcu;
    com_sao_stat_t sao_blk_stats[N_C][NUM_SAO_NEW_TYPES] = { 0 };

    sao_get_statistics(info, core->pathdr, map, core->pic_org, core->pic_rec, lcu_x, lcu_y, lcuw, lcuh, sao_blk_stats);

    double sao_lambda[N_C];
    int scale_lambda = (bit_depth == 10) ? info->qp_offset_bit_depth : 1;

    for (int compIdx = Y_C; compIdx < N_C; compIdx++) {
        sao_lambda[compIdx] = core->lambda[0] * scale_lambda;
    }
    sao_get_lcu_param(core, lbac, lcu_pos, lcu_x, lcu_y, sao_blk_stats, core->sao_blk_params, sao_lambda);
}