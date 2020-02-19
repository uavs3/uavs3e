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

#define MAX_FIRST_SEARCH_STEP              32
#define MAX_REFINE_SEARCH_STEP             8
#define RASTER_SEARCH_THD                  5
#define REFINE_SEARCH_THD                  0
#define BI_STEP                            5

#define MV_COST64(mv_bits) (lambda_mv * (u64)mv_bits)

static void get_search_range(inter_search_t *pi, com_pic_t *ref_pic, s16 mvc[MV_D], int x, int y, s16 range[MV_RANGE_DIM][MV_D], int dist)
{
    int offset = pi->gop_size >> 1;
    int offset_x, offset_y, rangexy;
    int range_offset = 3 * (1 << (pi->curr_mvr - 1));

    if (pi->curr_mvr == 0) {
        int max_qpel_sr = pi->max_search_range >> 3;
        rangexy = COM_CLIP3(max_qpel_sr >> 2, max_qpel_sr, (max_qpel_sr * dist + offset) / pi->gop_size);
    } else if (pi->curr_mvr == 1) {
        int max_hpel_sr = pi->max_search_range >> 2;
        rangexy = COM_CLIP3(max_hpel_sr >> 2, max_hpel_sr, (max_hpel_sr * dist + offset) / pi->gop_size);
    } else if (pi->curr_mvr == 2) {
        int max_ipel_sr = pi->max_search_range >> 1;
        rangexy = COM_CLIP3(max_ipel_sr >> 2, max_ipel_sr, (max_ipel_sr * dist + offset) / pi->gop_size);
    } else {
        int max_spel_sr = pi->max_search_range;
        rangexy = COM_CLIP3(max_spel_sr >> 2, max_spel_sr, (max_spel_sr * dist + offset) / pi->gop_size);
    }
    offset_x = rangexy;
    offset_y = rangexy;

    /* define search range for int-pel search and clip it if needs */
    range[MV_RANGE_MIN][MV_X] = COM_CLIP3(pi->min_mv_offset[MV_X], pi->max_mv_offset[MV_X], mvc[MV_X] - (s16)offset_x);
    range[MV_RANGE_MAX][MV_X] = COM_CLIP3(pi->min_mv_offset[MV_X], pi->max_mv_offset[MV_X], mvc[MV_X] + (s16)offset_x);
    range[MV_RANGE_MIN][MV_Y] = COM_CLIP3(pi->min_mv_offset[MV_Y], pi->max_mv_offset[MV_Y], mvc[MV_Y] - (s16)offset_y);
    range[MV_RANGE_MAX][MV_Y] = COM_CLIP3(pi->min_mv_offset[MV_Y], pi->max_mv_offset[MV_Y], mvc[MV_Y] + (s16)offset_y);

    wait_ref_available(ref_pic, range[MV_RANGE_MAX][MV_Y] + 4);

    com_assert(range[MV_RANGE_MIN][MV_X] <= range[MV_RANGE_MAX][MV_X]);
    com_assert(range[MV_RANGE_MIN][MV_Y] <= range[MV_RANGE_MAX][MV_Y]);
}


static u64 search_raster(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, s16 range[MV_RANGE_DIM][MV_D], s16 mvp[MV_D], s16 mv[MV_D])
{
    int bit_depth = pi->bit_depth;
    int i_org = pi->i_org;
    pel *org = pi->org;
    u32 mv_bits, best_mv_bits = 0;
    u64 cost_best = COM_UINT64_MAX, cost;
    s16 center_mv[MV_D];
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;
    int i_ref = ref_pic->stride_luma;
    pel *ref  = ref_pic->y + y * i_ref + x;
    int dir_ref_mvr_bits = 2 + tbl_refi_bits[pi->num_refp][refi] + pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1);
    static const int step_base[5] = { 5, 5, 5, 6, 8 };
    int search_step = COM_MAX(step_base[pi->curr_mvr], COM_MIN(w >> 1, h >> 1)) * (refi + 1);
    s16 min_cmv_x = range[MV_RANGE_MIN][MV_X];
    s16 min_cmv_y = range[MV_RANGE_MIN][MV_Y];
    s16 max_cmv_x = range[MV_RANGE_MAX][MV_X];
    s16 max_cmv_y = range[MV_RANGE_MAX][MV_Y];
    u8 *tab_mvbits_x = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx = pi->curr_mvr;
    int shift = 16 - (bit_depth - 8); // sad << 16 for u64 cost
    u32 lambda_mv = pi->lambda_mv;

    if (pi->curr_mvr > 2) {
        com_mv_rounding_s16(min_cmv_x + x, min_cmv_y + y, &min_cmv_x, &min_cmv_y, pi->curr_mvr - 2, pi->curr_mvr - 2);
        min_cmv_x -= x;
        min_cmv_y -= y;
    }
    for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += search_step) {
        int mv_bits_y = GET_MVBITS_IPEL_Y(mv_y) + dir_ref_mvr_bits;
        for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += search_step) {
            mv_bits = GET_MVBITS_IPEL_X(mv_x) + mv_bits_y;
            cost = MV_COST64(mv_bits) + block_pel_sad(w, h, shift, org, ref + mv_x + mv_y * i_ref, i_org, i_ref, bit_depth, 0);
            if (cost < cost_best) {
                mv[MV_X] = mv_x << 2;
                mv[MV_Y] = mv_y << 2;
                cost_best = cost;
                best_mv_bits = mv_bits;
            }
        }
    }

    search_step >>= 1;
    static const int min_step[5] = { 1, 1, 1, 2, 4 };

    while (search_step >= min_step[pi->curr_mvr]) {
        center_mv[MV_X] = mv[MV_X];
        center_mv[MV_Y] = mv[MV_Y];
        s16 min_cmv_x = (center_mv[MV_X] >> 2) - search_step;
        s16 min_cmv_y = (center_mv[MV_Y] >> 2) - search_step;
        s16 max_cmv_x = (center_mv[MV_X] >> 2) + search_step;
        s16 max_cmv_y = (center_mv[MV_Y] >> 2) + search_step;

        if (pi->curr_mvr > 2) {
            com_mv_rounding_s16(min_cmv_x + x, min_cmv_y + y, &min_cmv_x, &min_cmv_y, pi->curr_mvr - 2, pi->curr_mvr - 2);
            min_cmv_x -= x;
            min_cmv_y -= y;
        }

        for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += search_step) {
            if (mv_y < range[MV_RANGE_MIN][MV_Y] || mv_y > range[MV_RANGE_MAX][MV_Y]) {
                continue;
            }
            int mv_bits_y = GET_MVBITS_IPEL_Y(mv_y) + dir_ref_mvr_bits;

            for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += search_step) {
                if (mv_x < range[MV_RANGE_MIN][MV_X] || mv_x > range[MV_RANGE_MAX][MV_X]) {
                    continue;
                }
                mv_bits = GET_MVBITS_IPEL_X(mv_x) + mv_bits_y;
                cost = MV_COST64(mv_bits) + block_pel_sad(w, h, shift, org, ref + mv_x + mv_y * i_ref, i_org, i_ref, bit_depth, 0);
        
                if (cost < cost_best) {
                    mv[MV_X] = mv_x << 2;
                    mv[MV_Y] = mv_y << 2;
                    cost_best = cost;
                    best_mv_bits = mv_bits;
                }
            }
        }
        search_step >>= 1;
    }
    if (best_mv_bits > 0) {
        pi->mot_bits[lidx] = best_mv_bits;
    }
    return cost_best;
}

static u64 search_diamond(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, s16 range[MV_RANGE_DIM][MV_D], 
                          s16 mvp[MV_D], s16 mvi[MV_D], s16 mv[MV_D], int bi, int *beststep, int faststep)
{
    int bit_depth = pi->bit_depth;
    int i_org = pi->i_org;
    pel *org = pi->org;
    u64 cost, cost_best = COM_UINT64_MAX;
    int best_mv_bits = 0;
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;
    int i_ref = ref_pic->stride_luma;
    pel *ref = ref_pic->y + y * i_ref + x;
    int dir_ref_mvr_bits = (bi ? 1 : 2) + tbl_refi_bits[pi->num_refp][refi] + pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1);
    int core_offset, mvsize;
    s16 mv_best_x = mvi[MV_X] >> 2;
    s16 mv_best_y = mvi[MV_Y] >> 2;

    if (bi) {
        dir_ref_mvr_bits += pi->mot_bits[(lidx == REFP_0) ? REFP_1 : REFP_0];
    }
    mv_best_x = COM_CLIP3(pi->min_mv_offset[MV_X], pi->max_mv_offset[MV_X], mv_best_x);
    mv_best_y = COM_CLIP3(pi->min_mv_offset[MV_Y], pi->max_mv_offset[MV_Y], mv_best_y);

    if (pi->curr_mvr > 2) {
        com_mv_rounding_s16(mv_best_x + x, mv_best_y + y, &mv_best_x, &mv_best_y, pi->curr_mvr - 2, pi->curr_mvr - 2);
        mv_best_x -= x;
        mv_best_y -= y;

        core_offset = ((bi ? (BI_STEP - 2) : 1) << (pi->curr_mvr - 1));
        mvsize = 1 << (pi->curr_mvr - 2);
    } else {
        core_offset = (bi ? BI_STEP : 2);
        mvsize = 1;
    }

    s16 bak_imv_x = mv_best_x;
    s16 bak_imv_y = mv_best_y;
    int min_cmv_x = mv_best_x - core_offset;
    int min_cmv_y = mv_best_y - core_offset;
    int max_cmv_x = mv_best_x + core_offset;
    int max_cmv_y = mv_best_y + core_offset;

    while (min_cmv_x <= range[MV_RANGE_MIN][MV_X]) min_cmv_x += mvsize;
    while (min_cmv_y <= range[MV_RANGE_MIN][MV_Y]) min_cmv_y += mvsize;
    while (max_cmv_x >= range[MV_RANGE_MAX][MV_X]) max_cmv_x -= mvsize;
    while (max_cmv_y >= range[MV_RANGE_MAX][MV_Y]) max_cmv_y -= mvsize;

    u8 *tab_mvbits_x = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx = pi->curr_mvr;
    int shift = 16 - bi - (bit_depth - 8); // sad << 16 for u64 cost
    u32 lambda_mv = pi->lambda_mv;

    for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += mvsize) {
        int bits_mv_y = GET_MVBITS_IPEL_Y(mv_y) + dir_ref_mvr_bits;
        pel *p = ref + mv_y * i_ref;

        for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += mvsize) {
            int mv_bits = GET_MVBITS_IPEL_X(mv_x) + bits_mv_y;
            cost = MV_COST64(mv_bits) + block_pel_sad(w, h, shift, org, p + mv_x, i_org, i_ref, bit_depth, bi);

            if (cost < cost_best) {
                mv_best_x = mv_x;
                mv_best_y = mv_y;
                *beststep = 2;
                cost_best = cost;
                best_mv_bits = mv_bits;
            }
        }
    }
    s16 mvc[MV_D] = { mv_best_x , mv_best_y };
    get_search_range(pi, ref_pic, mvc, x, y, range, COM_ABS((int)(pi->ptr - pi->refp[refi][lidx].ptr)));

    if (!bi && faststep > 4) {
        for (int step = 4; step < faststep; step <<= 1) {
            int meidx = step > 8 ? 2 : 1;
            int multi = pi->curr_mvr > 2 ? (step * (1 << (pi->curr_mvr - 2))) : step;

            static tab_s8 tbl_diapos_partial[2][16][2] = {
                {{ -2, 0}, { -1, 1}, {0, 2}, {1, 1}, {2, 0}, {1, -1}, {0, -2}, { -1, -1}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
                {{ -4, 0}, { -3, 1}, { -2, 2}, { -1, 3}, {0, 4}, {1, 3}, {2, 2}, {3, 1}, {4, 0}, {3, -1}, {2, -2}, {1, -3}, {0, -4}, { -1, -3}, { -2, -2}, { -3, -1}}
            };

            for (int i = 0; i < 16; i++) {
                if (meidx == 1 && i > 8) {
                    break;
                }
                if ((step == 4) && (i == 1 || i == 3 || i == 5 || i == 7)) {
                    continue;
                }
                s16 mv_x = bak_imv_x + ((s16)(multi >> meidx) * tbl_diapos_partial[meidx - 1][i][MV_X]);
                s16 mv_y = bak_imv_y + ((s16)(multi >> meidx) * tbl_diapos_partial[meidx - 1][i][MV_Y]);

                if (mv_x > range[MV_RANGE_MAX][MV_X] || mv_x < range[MV_RANGE_MIN][MV_X] ||
                    mv_y > range[MV_RANGE_MAX][MV_Y] || mv_y < range[MV_RANGE_MIN][MV_Y]) {
                    cost = COM_UINT64_MAX;
                } else {
                    int mv_bits = GET_MVBITS_IPEL_X(mv_x) + GET_MVBITS_IPEL_Y(mv_y) + dir_ref_mvr_bits;
                    cost = MV_COST64(mv_bits) + block_pel_sad(w, h, shift, org, ref + mv_x + mv_y * i_ref, i_org, i_ref, bit_depth, bi);

                    if (cost < cost_best) {
                        mv_best_x = mv_x;
                        mv_best_y = mv_y;
                        *beststep = step;
                        cost_best = cost;
                        best_mv_bits = mv_bits;
                    }
                }
            }
        }
    }
    mv[MV_X] = mv_best_x << 2;
    mv[MV_Y] = mv_best_y << 2;

    if (!bi && best_mv_bits > 0) {
        pi->mot_bits[lidx] = best_mv_bits;
    }
    return cost_best;
}

static u64 me_sub_pel_search(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, s16 mvp[MV_D], s16 mvi[MV_D], s16 mv[MV_D], int bi)
{
    int bit_depth = pi->bit_depth;
    int i_org = pi->i_org;
    pel *org = pi->org;
    ALIGNED_32(pel pred[MAX_CU_DIM]);
    u64 cost, cost_best = COM_UINT64_MAX;
    s16 mv_x, mv_y, cx, cy;
    int lidx_r = (lidx == REFP_0) ? REFP_1 : REFP_0;
    int i, mv_bits, best_mv_bits;
    int max_posx = pi->max_mv_pos[MV_X];
    int max_posy = pi->max_mv_pos[MV_Y];
    int widx = CONV_LOG2(w) - MIN_CU_LOG2;
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;
    int i_ref = ref_pic->stride_luma;
    pel *ref = ref_pic->y + y * i_ref + x;
    int dir_ref_mvr_bits = (bi ? pi->mot_bits[lidx_r] + 1 : 2) + tbl_refi_bits[pi->num_refp][refi] + pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1);
    u8 *tab_mvbits_x = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx = pi->curr_mvr;
    u32 lambda_mv = pi->lambda_mv;

    best_mv_bits = 0;

    // make mv to be global coordinate 
    cx = mv[MV_X] = mvi[MV_X];
    cy = mv[MV_Y] = mvi[MV_Y];

    // initial satd cost 
    mv_bits = GET_MVBITS_X(cx) + GET_MVBITS_Y(cy) + dir_ref_mvr_bits;
    cost_best = MV_COST64(mv_bits);
    cost_best += ((u64)(calc_satd_16b(w, h, org, ref + (cx >> 2) + (cy >> 2) * i_ref, i_org, i_ref, bit_depth) >> bi) << 16);

    // hpel-level 
    for (i = 0; i < 8; i++) {
        static tab_s8 tbl_search_pattern_hpel_partial[8][2] = {
            { -2, 0}, { -2, 2}, {0, 2}, {2, 2}, {2, 0}, {2, -2}, {0, -2}, { -2, -2}
        };
        mv_x = cx + tbl_search_pattern_hpel_partial[i][0];
        mv_y = cy + tbl_search_pattern_hpel_partial[i][1];
        mv_bits = GET_MVBITS_X(mv_x) + GET_MVBITS_Y(mv_y) + dir_ref_mvr_bits;
        cost = MV_COST64(mv_bits);
        com_mc_blk_luma(ref_pic, pred, w, mv_x + (x << 2), mv_y + (y << 2), w, h, widx, max_posx, max_posy, (1 << bit_depth) - 1, 0);

        cost += (u64)(calc_satd_16b(w, h, org, pred, i_org, w, bit_depth) >> bi) << 16;
   
        if (cost < cost_best) {
            mv[MV_X] = mv_x;
            mv[MV_Y] = mv_y;
            cost_best = cost;
            best_mv_bits = mv_bits;
        }
    }

    // qpel-level 
    if (pi->curr_mvr == 0) {
        cx = mv[MV_X];
        cy = mv[MV_Y];
        for (i = 0; i < 8; i++) {
            static tab_s8 tbl_search_pattern_qpel_8point[8][2] = {
                { -1,  0}, { 0,  1}, { 1,  0}, { 0, -1}, { -1,  1}, { 1,  1}, { -1, -1}, { 1, -1}
            };

            mv_x = cx + tbl_search_pattern_qpel_8point[i][0];
            mv_y = cy + tbl_search_pattern_qpel_8point[i][1];
            mv_bits = GET_MVBITS_X(mv_x) + GET_MVBITS_Y(mv_y);
            mv_bits += dir_ref_mvr_bits;

            cost = MV_COST64(mv_bits);
            com_mc_blk_luma(ref_pic, pred, w, mv_x + (x << 2), mv_y + (y << 2), w, h, widx, max_posx, max_posy, (1 << bit_depth) - 1, 0);

            cost += (u64)(calc_satd_16b(w, h, org, pred, i_org, w, bit_depth) >> bi) << 16;

            if (cost < cost_best) {
                mv[MV_X] = mv_x;
                mv[MV_Y] = mv_y;
                cost_best = cost;
                best_mv_bits = mv_bits;
            }
        }
    }
    if (!bi && best_mv_bits > 0) {
        pi->mot_bits[lidx] = best_mv_bits;
    }
    return cost_best;
}

u32 me_search_tz(inter_search_t *pi, int x, int y, int w, int h, int pic_width, int pic_height, s8 refi, int lidx, s16 mvp[MV_D], s16 mv[MV_D], int bi)
{
    s16 mvc[MV_D]; 
    s16 range[MV_RANGE_DIM][MV_D];
    s16 mvi[MV_D];
    s16 mvt[MV_D];
    u64 cost, cost_best = COM_UINT64_MAX;
    int tmpstep = 0;
    int beststep = 0;
    int mvp_x = mvp[MV_X] >> pi->curr_mvr;
    int mvp_y = mvp[MV_Y] >> pi->curr_mvr;
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;

    pi->min_mv_offset[MV_X] = COM_MAX(-MAX_CU_SIZE + 1 - x, mvp_x - pi->tab_mvbits_offset + 4);
    pi->min_mv_offset[MV_Y] = COM_MAX(-MAX_CU_SIZE + 1 - y, mvp_y - pi->tab_mvbits_offset + 4);
    pi->max_mv_offset[MV_X] = COM_MIN(pic_width    - 1 - x, mvp_x + pi->tab_mvbits_offset - 4);
    pi->max_mv_offset[MV_Y] = COM_MIN(pic_height   - 1 - y, mvp_y + pi->tab_mvbits_offset - 4);

    if (pi->min_mv_offset[MV_X] > pi->max_mv_offset[MV_X] || pi->min_mv_offset[MV_Y] > pi->max_mv_offset[MV_Y]) {
        return COM_UINT64_MAX;
    }

    if (bi) {
        mvi[MV_X] = mv[MV_X];
        mvi[MV_Y] = mv[MV_Y];
        mvc[MV_X] = mv[MV_X] >> 2;
        mvc[MV_Y] = mv[MV_Y] >> 2;
    } else {
        mvi[MV_X] = mvp[MV_X];
        mvi[MV_Y] = mvp[MV_Y];
        mvc[MV_X] = mvp[MV_X] >> 2;
        mvc[MV_Y] = mvp[MV_Y] >> 2;
    }

    get_search_range(pi, ref_pic, mvc, x, y, range, COM_ABS((int)(pi->ptr - ref_pic->ptr)));

    cost = search_diamond(pi, x, y, w, h, refi, lidx, range, mvp, mvi, mvt, bi, &tmpstep, MAX_FIRST_SEARCH_STEP);

    if (cost < cost_best) {
        cost_best = cost;
        mv[MV_X] = mvt[MV_X];
        mv[MV_Y] = mvt[MV_Y];
        if (abs(mvp[MV_X] - mv[MV_X]) < 2 && abs(mvp[MV_Y] - mv[MV_Y]) < 2) {
            beststep = 0;
        } else {
            beststep = tmpstep;
        }
    }
    if (!bi && beststep > RASTER_SEARCH_THD) {
        cost = search_raster(pi, x, y, w, h, refi, lidx, range, mvp, mvt);
        if (cost < cost_best) {
            beststep = RASTER_SEARCH_THD;
            cost_best = cost;
            mv[MV_X] = mvt[MV_X];
            mv[MV_Y] = mvt[MV_Y];
        }
    }
    if (!bi && beststep > REFINE_SEARCH_THD) {
        mvc[MV_X] = mv[MV_X] >> 2;
        mvc[MV_Y] = mv[MV_Y] >> 2;
        get_search_range(pi, ref_pic, mvc, x, y, range, COM_ABS((int)(pi->ptr - pi->refp[refi][lidx].ptr)));

        mvi[MV_X] = mv[MV_X];
        mvi[MV_Y] = mv[MV_Y];
        cost = search_diamond(pi, x, y, w, h, refi, lidx, range, mvp, mvi, mvt, bi, &tmpstep, MAX_REFINE_SEARCH_STEP);
        if (cost < cost_best) {
            cost_best = cost;
            mv[MV_X] = mvt[MV_X];
            mv[MV_Y] = mvt[MV_Y];
        }
    }

    if (pi->curr_mvr == 0 || pi->curr_mvr == 1) {
        cost = me_sub_pel_search(pi, x, y, w, h,  refi, lidx, mvp, mv, mvt, bi);
        cost_best = cost;
        mv[MV_X] = mvt[MV_X];
        mv[MV_Y] = mvt[MV_Y];
    }
    return (u32)((cost_best + (1 << 15)) >> 16);
}