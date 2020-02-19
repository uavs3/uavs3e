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

#include "com_modules.h"

long long int get_distortion(int compIdx, int type, com_sao_stat_t saostatData[N_C][NUM_SAO_NEW_TYPES], com_sao_param_t sao_cur_param[N_C])
{
    int classIdc, bandIdx;
    long long int dist = 0;
    switch (type) {
    case SAO_TYPE_EO_0:
    case SAO_TYPE_EO_90:
    case SAO_TYPE_EO_135:
    case SAO_TYPE_EO_45: {
        for (classIdc = 0; classIdc < NUM_SAO_EO_CLASSES; classIdc++) {
            dist += distortion_cal(saostatData[compIdx][type].count[classIdc], sao_cur_param[compIdx].offset[classIdc],
                                   saostatData[compIdx][type].diff[classIdc]);
        }
    }
    break;
    case SAO_TYPE_BO: {
        for (int i = 0; i < 4; i++) {
            bandIdx = sao_cur_param[compIdx].bandIdx[i];
            dist += distortion_cal(saostatData[compIdx][type].count[bandIdx], sao_cur_param[compIdx].offset[i], saostatData[compIdx][type].diff[bandIdx]);
        }
    }
    break;
    default: {
        printf("Not a supported type");
        assert(0);
        exit(-1);
    }
    }
    return dist;
}
long long int  distortion_cal(long long int count, int offset, long long int diff)
{
    return (count * (long long int)offset * (long long int)offset - diff * offset * 2);
}



void off_sao(com_sao_param_t *saoblkparam)
{
    int i;
    for (i = 0; i < N_C; i++) {
        saoblkparam[i].typeIdc = -1;
    }
}

BOOL is_same_patch(s8 *map_patch, int mb_nr1, int mb_nr2)
{
    assert(mb_nr1 >= 0);
    assert(mb_nr2 >= 0);
    return (map_patch[mb_nr1] == map_patch[mb_nr2]);
}

void getSaoMergeNeighbor(com_info_t *info, s8 *map_patch, int i_scu, int pic_width_lcu, int lcu_pos, int mb_y, int mb_x,
                         com_sao_param_t(*sao_blk_params)[N_C], int *MergeAvail, com_sao_param_t sao_merge_param[][N_C])
{
    int mergeup_avail, mergeleft_avail;
    com_sao_param_t *sao_left_param;
    com_sao_param_t *sao_up_param;
    int scup = mb_y * i_scu + mb_x;
    mergeup_avail   = (mb_y == 0) ? 0 : is_same_patch(map_patch, scup, scup - i_scu) ? 1 : info->sqh.filter_cross_patch;
    mergeleft_avail = (mb_x == 0) ? 0 : is_same_patch(map_patch, scup, scup - 1) ? 1 : info->sqh.filter_cross_patch;

    if (mergeleft_avail) {
        sao_left_param = sao_blk_params[lcu_pos - 1];
        copySAOParam_for_blk(sao_merge_param[SAO_MERGE_LEFT], sao_left_param);
    }
    if (mergeup_avail) {
        sao_up_param = sao_blk_params[lcu_pos - pic_width_lcu];
        copySAOParam_for_blk(sao_merge_param[SAO_MERGE_ABOVE], sao_up_param);
    }
    MergeAvail[SAO_MERGE_LEFT ] = mergeleft_avail;
    MergeAvail[SAO_MERGE_ABOVE] = mergeup_avail;
}

static void sao_on_lcu(pel *src, int i_src, pel *dst, int i_dst, com_sao_param_t *sao_params, int smb_pix_height, int smb_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down, int sample_bit_depth)
{
    int type;
    int start_x, end_x, start_y, end_y;
    int start_x_r0, end_x_r0, start_x_r, end_x_r, start_x_rn, end_x_rn;
    int x, y;
    s8 leftsign, rightsign, upsign, downsign;
    int diff;
    s8 signupline[MAX_CU_SIZE + 8], *signupline1;
    int reg = 0;
    int edgetype;
    int max_pel = (1 << sample_bit_depth) - 1;

    type = sao_params->typeIdc;

    switch (type) {
    case SAO_TYPE_EO_0: {

        start_x = smb_available_left ? 0 : 1;
        end_x = smb_available_right ? smb_pix_width : (smb_pix_width - 1);

        for (y = 0; y < smb_pix_height; y++) {
            diff = src[start_x] - src[start_x - 1];
            leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            for (x = start_x; x < end_x; x++) {
                diff = src[x] - src[x + 1];
                rightsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = leftsign + rightsign;
                leftsign = -rightsign;
                dst[x] = COM_CLIP3(0, max_pel, src[x] + sao_params->offset[edgetype + 2]);
            }
            dst += i_dst;
            src += i_src;
        }

    }
    break;
    case SAO_TYPE_EO_90: {
        pel *dst_base = dst;
        pel *src_base = src;
        start_y = smb_available_up ? 0 : 1;
        end_y = smb_available_down ? smb_pix_height : (smb_pix_height - 1);
        for (x = 0; x < smb_pix_width; x++) {
            src = src_base + start_y * i_src;
            diff = src[0] - src[-i_src];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            dst = dst_base + start_y * i_dst;
            for (y = start_y; y < end_y; y++) {
                diff = src[0] - src[i_src];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + upsign;
                upsign = -downsign;
                *dst = COM_CLIP3(0, max_pel, src[0] + sao_params->offset[edgetype + 2]);
                dst += i_dst;
                src += i_src;
            }
            dst_base++;
            src_base++;
        }
    }
    break;
    case SAO_TYPE_EO_135: {
        start_x_r0 = (smb_available_up && smb_available_left) ? 0 : 1;
        end_x_r0 = smb_available_up ? (smb_available_right ? smb_pix_width : (smb_pix_width - 1)) : 1;
        start_x_r = smb_available_left ? 0 : 1;
        end_x_r = smb_available_right ? smb_pix_width : (smb_pix_width - 1);
        start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (smb_pix_width - 1);
        end_x_rn = (smb_available_right && smb_available_down) ? smb_pix_width : (smb_pix_width - 1);

        //init the line buffer
        for (x = start_x_r + 1; x < end_x_r + 1; x++) {
            diff = src[x + i_src] - src[x - 1];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            signupline[x] = upsign;
        }
        //first row
        for (x = start_x_r0; x < end_x_r0; x++) {
            diff = src[x] - src[x - 1 - i_src];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = upsign - signupline[x + 1];
            dst[x] = COM_CLIP3(0, max_pel, src[x] + sao_params->offset[edgetype + 2]);
        }
        dst += i_dst;
        src += i_src;

        //middle rows
        for (y = 1; y < smb_pix_height - 1; y++) {
            for (x = start_x_r; x < end_x_r; x++) {
                if (x == start_x_r) {
                    diff = src[x] - src[x - 1 - i_src];
                    upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    signupline[x] = upsign;
                }
                diff = src[x] - src[x + 1 + i_src];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + signupline[x];
                dst[x] = COM_CLIP3(0, max_pel, src[x] + sao_params->offset[edgetype + 2]);
                signupline[x] = reg;
                reg = -downsign;
            }
            dst += i_dst;
            src += i_src;
        }
        //last row
        for (x = start_x_rn; x < end_x_rn; x++) {
            if (x == start_x_r) {
                diff = src[x] - src[x - 1 - i_src];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                signupline[x] = upsign;
            }
            diff = src[x] - src[x + 1 + i_src];
            downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = downsign + signupline[x];
            dst[x] = COM_CLIP3(0, max_pel, src[x] + sao_params->offset[edgetype + 2]);
        }
    }
    break;
    case SAO_TYPE_EO_45: {
        start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (smb_pix_width - 1);
        end_x_r0 = (smb_available_up && smb_available_right) ? smb_pix_width : (smb_pix_width - 1);
        start_x_r = smb_available_left ? 0 : 1;
        end_x_r = smb_available_right ? smb_pix_width : (smb_pix_width - 1);
        start_x_rn = (smb_available_left && smb_available_down) ? 0 : 1;
        end_x_rn = smb_available_down ? (smb_available_right ? smb_pix_width : (smb_pix_width - 1)) : 1;

        //init the line buffer
        signupline1 = signupline + 1;
        for (x = start_x_r - 1; x < end_x_r - 1; x++) {
            diff = src[x + i_src] - src[x + 1];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            signupline1[x] = upsign;
        }
        //first row
        for (x = start_x_r0; x < end_x_r0; x++) {
            diff = src[x] - src[x + 1 - i_src];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = upsign - signupline1[x - 1];
            dst[x] = COM_CLIP3(0, max_pel, src[x] + sao_params->offset[edgetype + 2]);
        }
        dst += i_dst;
        src += i_src;

        //middle rows
        for (y = 1; y < smb_pix_height - 1; y++) {
            for (x = start_x_r; x < end_x_r; x++) {
                if (x == end_x_r - 1) {
                    diff = src[x] - src[x + 1 - i_src];
                    upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    signupline1[x] = upsign;
                }
                diff = src[x] - src[x - 1 + i_src];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + signupline1[x];
                dst[x] = COM_CLIP3(0, max_pel, src[x] + sao_params->offset[edgetype + 2]);
                signupline1[x - 1] = -downsign;
            }
            dst += i_dst;
            src += i_src;
        }
        for (x = start_x_rn; x < end_x_rn; x++) {
            if (x == end_x_r - 1) {
                diff = src[x] - src[x + 1 - i_src];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                signupline1[x] = upsign;
            }
            diff = src[x] - src[x - 1 + i_src];
            downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = downsign + signupline1[x];
            dst[x] = COM_CLIP3(0, max_pel, src[x] + sao_params->offset[edgetype + 2]);
        }
    }
    break;
    case SAO_TYPE_BO: {
        for (y = 0; y < smb_pix_height; y++) {
            for (x = 0; x < smb_pix_width; x++) {
                int val = dst[x];
                int band_idx = val >> (sample_bit_depth - NUM_SAO_BO_CLASSES_IN_BIT);
                if (band_idx == sao_params->bandIdx[0]) {
                    dst[x] = COM_CLIP3(0, max_pel, val + sao_params->offset[0]);
                } else if (band_idx == sao_params->bandIdx[1]) {
                    dst[x] = COM_CLIP3(0, max_pel, val + sao_params->offset[1]);
                } else if (band_idx == sao_params->bandIdx[2]) {
                    dst[x] = COM_CLIP3(0, max_pel, val + sao_params->offset[2]);
                } else if (band_idx == sao_params->bandIdx[3]) {
                    dst[x] = COM_CLIP3(0, max_pel, val + sao_params->offset[3]);
                }

            }
            dst += i_dst;
        }
    }
    break;
    default: {
        fprintf(stderr, "Not a supported SAO types\n");
        com_assert(0);
        exit(-1);
    }
    }
}

void com_sao_one_row(com_info_t *info, com_map_t *map, com_pic_t  *pic_rec, com_sao_param_t(*sao_blk_params)[N_C], int lcu_y, pel *sao_src_buf[3], pel *linebuf_sao[3])
{
    int pic_pix_height = info->pic_height;
    int pic_pix_width  = info->pic_width;
    int lcu_size = info->max_cuwh;
    int sample_bit_depth = info->bit_depth_internal;
    s8 *map_patch = map->map_patch;
    int pix_x, pix_y = lcu_y * lcu_size;
    int lcu_pix_height = COM_MIN(lcu_size, pic_pix_height - pix_y);
    int lcu_idx = lcu_y * info->pic_width_in_lcu;
    int i_scu = info->i_scu;
    int scup = (pix_y >> MIN_CU_LOG2) * i_scu;
    int isAboveAvail = pix_y ? 1 : 0;
    int isBelowAvail = pix_y + lcu_size < info->pic_height ? 1 : 0;
    int isFltSliceBorder = isAboveAvail;
    int i_bufl = lcu_size     + SAO_SHIFT_PIX_NUM + 2;
    int i_bufc = lcu_size / 2 + SAO_SHIFT_PIX_NUM + 2;
    int i_dstl = pic_rec->stride_luma;
    int i_dstc = pic_rec->stride_chroma;
    pel *bufl = sao_src_buf[0] + (SAO_SHIFT_PIX_NUM + 1) * i_bufl + (SAO_SHIFT_PIX_NUM + 1);
    pel *bufu = sao_src_buf[1] + (SAO_SHIFT_PIX_NUM + 1) * i_bufc + (SAO_SHIFT_PIX_NUM + 1);
    pel *bufv = sao_src_buf[2] + (SAO_SHIFT_PIX_NUM + 1) * i_bufc + (SAO_SHIFT_PIX_NUM + 1);
    pel *dstl = pic_rec->y + pix_y     * i_dstl;
    pel *dstu = pic_rec->u + pix_y / 2 * i_dstc;
    pel *dstv = pic_rec->v + pix_y / 2 * i_dstc;
    pel *linel = linebuf_sao[0];
    pel *lineu = linebuf_sao[1];
    pel *linev = linebuf_sao[2];
    pel *line_srcl = dstl + (lcu_size     - SAO_SHIFT_PIX_NUM - 1) * i_dstl;
    pel *line_srcu = dstu + (lcu_size / 2 - SAO_SHIFT_PIX_NUM - 1) * i_dstc;
    pel *line_srcv = dstv + (lcu_size / 2 - SAO_SHIFT_PIX_NUM - 1) * i_dstc;
    pel topleftl = 0, topleftu = 0, topleftv = 0;
    int adj_l = 0, adj_u = 0, adj_v = 0;
    int y_offset = 0, height_offset = 0, height_offset_bo = 0;

    if (isAboveAvail) {
        y_offset -= SAO_SHIFT_PIX_NUM;
        height_offset_bo += SAO_SHIFT_PIX_NUM;
    }
    if (isBelowAvail) {
        height_offset -= SAO_SHIFT_PIX_NUM;
    }

    if (!info->sqh.filter_cross_patch && isFltSliceBorder && map_patch[scup] != map_patch[scup - i_scu]) {
        isFltSliceBorder = 0;
    }

    for (pix_x = 0; pix_x < pic_pix_width; pix_x += lcu_size, lcu_idx++, dstl += lcu_size, dstu += lcu_size / 2, dstv += lcu_size / 2) {
        com_sao_param_t *saoBlkParam = sao_blk_params[lcu_idx];
        int lcu_pix_width = COM_MIN(lcu_size, pic_pix_width - pix_x);
        int isLeftAvail = (pix_x != 0);
        int isRightAvail = pix_x + lcu_pix_width < info->pic_width;
        int x_offset = 0, width_offset = 0;
        int blk_x_l, blk_x_c, blk_widthl, blk_widthc;
        pel *src, *dst;
        pel *ll, *lu, *lv;
        const int uv_shift = 1;

        if (isLeftAvail) {
            x_offset     -= SAO_SHIFT_PIX_NUM;
            width_offset += SAO_SHIFT_PIX_NUM;
        }
        if (isRightAvail) {
            width_offset -= SAO_SHIFT_PIX_NUM;
        }

        blk_widthl = width_offset +  lcu_pix_width;
        blk_widthc = width_offset + (lcu_pix_width >> uv_shift);
        blk_x_l = pix_x     + x_offset;
        blk_x_c = pix_x / 2 + x_offset;
        ll = linel + blk_x_l;
        lu = lineu + blk_x_c;
        lv = linev + blk_x_c;

#define FLUSH_LUMA() { \
        memcpy(ll, line_srcl + blk_x_l, (blk_widthl - isRightAvail) * sizeof(pel)); \
        ll[-1] = topleftl; \
        topleftl = line_srcl[blk_x_l + blk_widthl - isRightAvail]; \
    }
#define FLUSH_CHROMA_U() { \
        memcpy(lu, line_srcu + blk_x_c, (blk_widthc - isRightAvail) * sizeof(pel)); \
        lu[-1] = topleftu; \
        topleftu = line_srcu[blk_x_c + blk_widthc - isRightAvail]; \
    }
#define FLUSH_CHROMA_V() { \
        memcpy(lv, line_srcv + blk_x_c, (blk_widthc - isRightAvail) * sizeof(pel)); \
        lv[-1] = topleftv; \
        topleftv = line_srcv[blk_x_c + blk_widthc - isRightAvail]; \
    }

        if (saoBlkParam[Y_C].typeIdc == -1) {
            FLUSH_LUMA();
            adj_l = 1;
        } else if (saoBlkParam[Y_C].typeIdc == SAO_TYPE_BO) {
            int cpy_offset = blk_widthl + adj_l - 1;
            pel *tmp_d = bufl + y_offset * i_bufl + x_offset - adj_l - i_bufl + cpy_offset;
            pel *tmp_s = dstl + y_offset * i_dstl + x_offset - adj_l + cpy_offset;
            int blk_height = height_offset + lcu_pix_height + height_offset_bo;
            tmp_d[0] = ll[cpy_offset - adj_l];
            tmp_d[1] = ll[cpy_offset - adj_l + 1];
            tmp_d += i_bufl;

            for (int i = -1; i < blk_height; i++) {
                tmp_d[0] = tmp_s[0];
                tmp_d[1] = tmp_s[1];
                tmp_d += i_bufl;
                tmp_s += i_dstl;
            }
            FLUSH_LUMA();

            dst = dstl + y_offset * i_dstl + x_offset;
            uavs3e_funs_handle.sao(dst, i_dstl, dst, i_dstl, &(saoBlkParam[Y_C]), blk_height, blk_widthl, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail, sample_bit_depth);

            bufl += lcu_size;
            adj_l = -1;

        } else {
            pel *tmp_d = bufl - SAO_SHIFT_PIX_NUM * i_bufl + x_offset - adj_l - i_bufl;
            pel *tmp_s = dstl - SAO_SHIFT_PIX_NUM * i_dstl + x_offset - adj_l;
            int blk_height = height_offset + lcu_pix_height;
            int cpy_size = (blk_widthl + 1 + adj_l) * sizeof(pel);

            memcpy(tmp_d, ll - adj_l, cpy_size);
            tmp_d += i_bufl;

            for (int i = -SAO_SHIFT_PIX_NUM; i <= blk_height; i++) {
                memcpy(tmp_d, tmp_s, cpy_size);
                tmp_d += i_bufl;
                tmp_s += i_dstl;
            }
            FLUSH_LUMA();
            if (isAboveAvail) {
                dst = dstl - SAO_SHIFT_PIX_NUM * i_dstl + x_offset;
                src = bufl - SAO_SHIFT_PIX_NUM * i_bufl + x_offset;
                uavs3e_funs_handle.sao(src, i_bufl, dst, i_dstl, &(saoBlkParam[Y_C]), SAO_SHIFT_PIX_NUM, blk_widthl, isLeftAvail, isRightAvail, 1, isFltSliceBorder, sample_bit_depth);
            }
            dst = dstl + x_offset;
            src = bufl + x_offset;
            uavs3e_funs_handle.sao(src, i_bufl, dst, i_dstl, &(saoBlkParam[Y_C]), blk_height, blk_widthl, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail, sample_bit_depth);

            bufl += lcu_size;
            adj_l = -1;
        }

        if (saoBlkParam[U_C].typeIdc == -1) {
            FLUSH_CHROMA_U();
            adj_u = 1;
        } else if (saoBlkParam[U_C].typeIdc == SAO_TYPE_BO) {
            int cpy_offset = blk_widthc + adj_u - 1;
            pel *tmp_d = bufu + y_offset * i_bufc + x_offset - adj_u - i_bufc + cpy_offset;
            pel *tmp_s = dstu + y_offset * i_dstc + x_offset - adj_u + cpy_offset;
            int blk_height = height_offset + (lcu_pix_height >> uv_shift) + height_offset_bo;

            tmp_d[0] = lu[cpy_offset - adj_u];
            tmp_d[1] = lu[cpy_offset - adj_u + 1];

            tmp_d += i_bufc;

            for (int i = -1; i < blk_height; i++) {
                tmp_d[0] = tmp_s[0];
                tmp_d[1] = tmp_s[1];
                tmp_d += i_bufc;
                tmp_s += i_dstc;
            }
            FLUSH_CHROMA_U();

            dst = dstu + y_offset * i_dstc + x_offset;
            if (saoBlkParam[U_C].typeIdc != -1) {
                uavs3e_funs_handle.sao(dst, i_dstc, dst, i_dstc, &(saoBlkParam[U_C]), blk_height, blk_widthc, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail, sample_bit_depth);
            }
            bufu += lcu_size / 2;
            adj_u = -1;
        } else {
            pel *tmp_d = bufu - SAO_SHIFT_PIX_NUM * i_bufc + x_offset - adj_u - i_bufc;
            pel *tmp_s = dstu - SAO_SHIFT_PIX_NUM * i_dstc + x_offset - adj_u;
            int blk_height = height_offset + (lcu_pix_height >> uv_shift);
            int cpy_size = (blk_widthc + 1 + adj_u) * sizeof(pel);

            memcpy(tmp_d, lu - adj_u, cpy_size);
            tmp_d += i_bufc;

            for (int i = -SAO_SHIFT_PIX_NUM; i <= blk_height; i++) {
                memcpy(tmp_d, tmp_s, cpy_size);
                tmp_d += i_bufc;
                tmp_s += i_dstc;
            }
            FLUSH_CHROMA_U();

            if (isAboveAvail) {
                dst = dstu - SAO_SHIFT_PIX_NUM * i_dstc + x_offset;
                src = bufu - SAO_SHIFT_PIX_NUM * i_bufc + x_offset;
                if (saoBlkParam[U_C].typeIdc != -1) {
                    uavs3e_funs_handle.sao(src, i_bufc, dst, i_dstc, &(saoBlkParam[U_C]), SAO_SHIFT_PIX_NUM, blk_widthc, isLeftAvail, isRightAvail, 1, isFltSliceBorder, sample_bit_depth);
                }
            }
            dst = dstu + x_offset;
            src = bufu + x_offset;
            if (saoBlkParam[U_C].typeIdc != -1) {
                uavs3e_funs_handle.sao(src, i_bufc, dst, i_dstc, &(saoBlkParam[U_C]), blk_height, blk_widthc, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail, sample_bit_depth);
            }
            bufu += lcu_size / 2;
            adj_u = -1;
        }

        if (saoBlkParam[V_C].typeIdc == -1) {
            FLUSH_CHROMA_V();
            adj_v = 1;
        } else if (saoBlkParam[V_C].typeIdc == SAO_TYPE_BO) {
            int cpy_offset = blk_widthc + adj_v - 1;
            pel *tmp_d = bufv + y_offset * i_bufc + x_offset - adj_v - i_bufc + cpy_offset;
            pel *tmp_s = dstv + y_offset * i_dstc + x_offset - adj_v + cpy_offset;
            int blk_height = height_offset + (lcu_pix_height >> uv_shift) + height_offset_bo;

            tmp_d[0] = lv[cpy_offset - adj_v];
            tmp_d[1] = lv[cpy_offset - adj_v + 1];

            tmp_d += i_bufc;

            for (int i = -1; i < blk_height; i++) {
                tmp_d[0] = tmp_s[0];
                tmp_d[1] = tmp_s[1];
                tmp_d += i_bufc;
                tmp_s += i_dstc;
            }
            FLUSH_CHROMA_V();

            dst = dstv + y_offset * i_dstc + x_offset;
            if (saoBlkParam[V_C].typeIdc != -1) {
                uavs3e_funs_handle.sao(dst, i_dstc, dst, i_dstc, &(saoBlkParam[V_C]), blk_height, blk_widthc, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail, sample_bit_depth);
            }
            bufv += lcu_size / 2;
            adj_v = -1;
        } else {
            pel *tmp_d = bufv - SAO_SHIFT_PIX_NUM * i_bufc + x_offset - adj_v - i_bufc;
            pel *tmp_s = dstv - SAO_SHIFT_PIX_NUM * i_dstc + x_offset - adj_v;
            int blk_height = height_offset + (lcu_pix_height >> uv_shift);
            int cpy_size = (blk_widthc + 1 + adj_v) * sizeof(pel);

            memcpy(tmp_d, lv - adj_v, cpy_size);
            tmp_d += i_bufc;

            for (int i = -SAO_SHIFT_PIX_NUM; i <= blk_height; i++) {
                memcpy(tmp_d, tmp_s, cpy_size);
                tmp_d += i_bufc;
                tmp_s += i_dstc;
            }
            FLUSH_CHROMA_V();

            if (isAboveAvail) {
                dst = dstv - SAO_SHIFT_PIX_NUM * i_dstc + x_offset;
                src = bufv - SAO_SHIFT_PIX_NUM * i_bufc + x_offset;
                if (saoBlkParam[V_C].typeIdc != -1) {
                    uavs3e_funs_handle.sao(src, i_bufc, dst, i_dstc, &(saoBlkParam[V_C]), SAO_SHIFT_PIX_NUM, blk_widthc, isLeftAvail, isRightAvail, 1, isFltSliceBorder, sample_bit_depth);
                }
            }
            dst = dstv + x_offset;
            src = bufv + x_offset;
            if (saoBlkParam[V_C].typeIdc != -1) {
                uavs3e_funs_handle.sao(src, i_bufc, dst, i_dstc, &(saoBlkParam[V_C]), blk_height, blk_widthc, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail, sample_bit_depth);
            }
            bufv += lcu_size / 2;
            adj_v = -1;
        }
    }
}

void uavs3e_funs_init_sao_c()
{
    uavs3e_funs_handle.sao = sao_on_lcu;
}
