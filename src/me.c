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

#define MV_COST64(mv_bits) (lambda_mv * (u64)(mv_bits))

static u64 search_raster(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, s16 range[MV_RANGE_DIM][MV_D], const s16 mvp[MV_D], s16 mv[MV_D])
{
    u32 best_mv_bits   = 0;
    int bit_depth      = pi->bit_depth;
    int i_org          = pi->i_org;
    pel *org           = pi->org;
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;
    u8 *tab_mvbits_x   = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y   = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx        = pi->curr_mvr;
    u32 lambda_mv      = pi->lambda_mv;
    int i_ref          = ref_pic->stride_luma;
    pel *ref           = ref_pic->y + y * i_ref + x;
    int shift          = 16 - (bit_depth - 8); // sad << 16 for u64 cost
    u64 cost_best      = COM_UINT64_MAX;

    static const int step_base[5] = { 5, 5, 5, 6, 8 };
    static const int step_min [5] = { 1, 1, 1, 2, 4 };

    int search_step = COM_MAX(step_base[pi->curr_mvr], COM_MIN(w >> 1, h >> 1)) * (refi + 1);
    int round = 0;

    while (search_step >= step_min[pi->curr_mvr]) {
        s16 min_cmv_x, min_cmv_y;
        s16 max_cmv_x, max_cmv_y;

        if (round++ == 0) {
            min_cmv_x = range[MV_RANGE_MIN][MV_X];
            min_cmv_y = range[MV_RANGE_MIN][MV_Y];
            max_cmv_x = range[MV_RANGE_MAX][MV_X];
            max_cmv_y = range[MV_RANGE_MAX][MV_Y];
        } else {
            min_cmv_x = COM_MAX(mv[MV_X] - search_step, range[MV_RANGE_MIN][MV_X]);
            min_cmv_y = COM_MAX(mv[MV_Y] - search_step, range[MV_RANGE_MIN][MV_Y]);
            max_cmv_x = COM_MIN(mv[MV_X] + search_step, range[MV_RANGE_MAX][MV_X]);
            max_cmv_y = COM_MIN(mv[MV_Y] + search_step, range[MV_RANGE_MAX][MV_Y]);
        }
        if (pi->curr_mvr > 2) {
            com_mv_rounding_s16(min_cmv_x + x, min_cmv_y + y, &min_cmv_x, &min_cmv_y, pi->curr_mvr - 2);
            min_cmv_x -= x;
            min_cmv_y -= y;
        }

        for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += search_step) {
            int mv_bits_y = GET_MVBITS_IPEL_Y(mv_y);
            for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += search_step) {
                int mv_bits = GET_MVBITS_IPEL_X(mv_x) + mv_bits_y;
                u64 cost = MV_COST64(mv_bits) + block_pel_sad(w, h, shift, org, ref + mv_x + mv_y * i_ref, i_org, i_ref, bit_depth, 0);
                if (cost < cost_best) {
                    mv[MV_X] = mv_x;
                    mv[MV_Y] = mv_y;
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

static void get_diamond_range(inter_search_t *pi, com_pic_t *ref_pic, s16 center[MV_D], s16 range[MV_RANGE_DIM][MV_D], int dist)
{
    int max_sr = pi->max_search_range >> COM_MAX(3 - pi->curr_mvr, 0);
    int range_xy = COM_CLIP3(max_sr >> 2, max_sr, (max_sr * dist + (pi->gop_size >> 1)) / pi->gop_size);

    /* define search range for int-pel search and clip it if needs */
    range[MV_RANGE_MIN][MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], center[MV_X] - (s16)range_xy);
    range[MV_RANGE_MAX][MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], center[MV_X] + (s16)range_xy);
    range[MV_RANGE_MIN][MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], center[MV_Y] - (s16)range_xy);
    range[MV_RANGE_MAX][MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], center[MV_Y] + (s16)range_xy);

    wait_ref_available(ref_pic, range[MV_RANGE_MAX][MV_Y] + 4);

    com_assert(range[MV_RANGE_MIN][MV_X] <= range[MV_RANGE_MAX][MV_X]);
    com_assert(range[MV_RANGE_MIN][MV_Y] <= range[MV_RANGE_MAX][MV_Y]);
}

static int search_diamond(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, s16 range[MV_RANGE_DIM][MV_D], 
                          const s16 mvp[MV_D], s16 mv[MV_D], u64* cost_best, int bi, int max_step)
{
    int best_step      = 0;
    int best_mv_bits   = 0;
    int bit_depth      = pi->bit_depth;
    int i_org          = pi->i_org;
    pel *org           = pi->org;
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;
    u8 *tab_mvbits_x   = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y   = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx        = pi->curr_mvr;
    u32 lambda_mv      = pi->lambda_mv;
    int i_ref          = ref_pic->stride_luma;
    pel *ref           = ref_pic->y + y * i_ref + x;
    int shift          = 16 - bi - (bit_depth - 8); // sad << 16 for u64 cost
    int core_size      = (bi ? 5 : 2);
    int core_step      = 1;
    s16 mv_best_x      = mv[MV_X];
    s16 mv_best_y      = mv[MV_Y];

    get_diamond_range(pi, ref_pic, mv, range, COM_ABS((int)(pi->ptr - ref_pic->ptr)));

    mv_best_x = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], mv_best_x);
    mv_best_y = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], mv_best_y);

    if (pi->curr_mvr > 2) {
        com_mv_rounding_s16(mv_best_x + x, mv_best_y + y, &mv_best_x, &mv_best_y, pi->curr_mvr - 2);
        mv_best_x -= x;
        mv_best_y -= y;
        core_size += bi;
        core_size <<= (pi->curr_mvr - 2);
        core_step <<= (pi->curr_mvr - 2);
    } 

    s16 center_x = mv_best_x;
    s16 center_y = mv_best_y;
    int min_cmv_x = mv_best_x - core_size;
    int min_cmv_y = mv_best_y - core_size;
    int max_cmv_x = mv_best_x + core_size;
    int max_cmv_y = mv_best_y + core_size;

    while (min_cmv_x <= range[MV_RANGE_MIN][MV_X]) min_cmv_x += core_step;
    while (min_cmv_y <= range[MV_RANGE_MIN][MV_Y]) min_cmv_y += core_step;
    while (max_cmv_x >= range[MV_RANGE_MAX][MV_X]) max_cmv_x -= core_step;
    while (max_cmv_y >= range[MV_RANGE_MAX][MV_Y]) max_cmv_y -= core_step;

    for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += core_step) {
        int bits_mv_y = GET_MVBITS_IPEL_Y(mv_y);
        pel *p = ref + mv_y * i_ref;

        for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += core_step) {
            int mv_bits = GET_MVBITS_IPEL_X(mv_x) + bits_mv_y;
            u64 cost = MV_COST64(mv_bits) + block_pel_sad(w, h, shift, org, p + mv_x, i_org, i_ref, bit_depth, bi);

            if (cost < *cost_best) {
                mv_best_x = mv_x;
                mv_best_y = mv_y;
                best_step = 2;
                *cost_best = cost;
                best_mv_bits = mv_bits;
            }
        }
    }

    mv[MV_X] = mv_best_x;
    mv[MV_Y] = mv_best_y;

    if (bi) {
        return best_step;
    }

    /* Step2: search multilayer diamond */
    get_diamond_range(pi, ref_pic, mv, range, COM_ABS((int)(pi->ptr - ref_pic->ptr)));

    int mvr_scale = COM_MAX(pi->curr_mvr - 2, 0);

    for (int step = 4, round = 0; step < max_step; step <<= 1, round++) {
        int step_scale = (step << mvr_scale) >> 1;
        int search_num = round < 2 ? 8 : 16;
        int search_add = round < 1 ? 2 :  1;
        int search_idx = round < 2 ? 0 :  1;

        static tab_s8 tbl_diapos_partial[2][16][2] = {
            { { -2, 0}, { -1, 1}, {  0, 2}, {  1, 1}, {2, 0}, {1, -1}, {0, -2}, { -1, -1} }, 
            { { -4, 0}, { -3, 1}, { -2, 2}, { -1, 3}, {0, 4}, {1,  3}, {2,  2}, {  3,  1}, {4, 0}, {3, -1}, {2, -2}, {1, -3}, {0, -4}, { -1, -3}, { -2, -2}, { -3, -1}}
        };

        for (int i = 0; i < search_num; i += search_add) {
            s16 mv_x = center_x + ((s16)(step_scale >> search_idx) * tbl_diapos_partial[search_idx][i][MV_X]);
            s16 mv_y = center_y + ((s16)(step_scale >> search_idx) * tbl_diapos_partial[search_idx][i][MV_Y]);

            if (mv_x <= range[MV_RANGE_MAX][MV_X] && mv_x >= range[MV_RANGE_MIN][MV_X] &&
                mv_y <= range[MV_RANGE_MAX][MV_Y] && mv_y >= range[MV_RANGE_MIN][MV_Y]) {
                int mv_bits = GET_MVBITS_IPEL_X(mv_x) + GET_MVBITS_IPEL_Y(mv_y);
                u64 cost = MV_COST64(mv_bits) + block_pel_sad(w, h, shift, org, ref + mv_x + mv_y * i_ref, i_org, i_ref, bit_depth, bi);

                if (cost < *cost_best) {
                    mv_best_x = mv_x;
                    mv_best_y = mv_y;
                    best_step = step;
                    *cost_best = cost;
                    best_mv_bits = mv_bits;
                }
            }
        }
    }

    mv[MV_X] = mv_best_x;
    mv[MV_Y] = mv_best_y;

    if (best_mv_bits) {
        pi->mot_bits[lidx] = best_mv_bits;
    }
    return best_step;
}

static u64 me_sub_pel_search(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], int bi)
{
    int bit_depth = pi->bit_depth;
    int i_org = pi->i_org;
    pel *org = pi->org;
    ALIGNED_32(pel pred[MAX_CU_DIM]);
    u64 cost, cost_best = COM_UINT64_MAX;
    s16 mv_x, mv_y, cx, cy;
    int i, mv_bits, best_mv_bits;
    int max_posx = pi->max_coord[MV_X];
    int max_posy = pi->max_coord[MV_Y];
    int widx = CONV_LOG2(w) - MIN_CU_LOG2;
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;
    int i_ref = ref_pic->stride_luma;
    pel *ref = ref_pic->y + y * i_ref + x;
    u8 *tab_mvbits_x = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx = pi->curr_mvr;
    u32 lambda_mv = pi->lambda_mv;

    best_mv_bits = 0;

    // make mv to be global coordinate 
    cx = mv[MV_X];
    cy = mv[MV_Y];

    // initial satd cost 
    mv_bits = GET_MVBITS_X(cx) + GET_MVBITS_Y(cy);
    cost_best = MV_COST64(mv_bits);
    cost_best += ((u64)(calc_satd_16b(w, h, org, ref + (cx >> 2) + (cy >> 2) * i_ref, i_org, i_ref, bit_depth) >> bi) << 16);

    // hpel-level 
    for (i = 0; i < 8; i++) {
        static tab_s8 tbl_search_pattern_hpel_partial[8][2] = {
            { -2, 0}, { -2, 2}, {0, 2}, {2, 2}, {2, 0}, {2, -2}, {0, -2}, { -2, -2}
        };
        mv_x = cx + tbl_search_pattern_hpel_partial[i][0];
        mv_y = cy + tbl_search_pattern_hpel_partial[i][1];
        mv_bits = GET_MVBITS_X(mv_x) + GET_MVBITS_Y(mv_y);
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

static int get_max_search_range(inter_search_t *pi, const s16 mvp[2], int x, int y, int pic_width, int pic_height)
{
    int max_table_offset = (pi->tab_mvbits_offset << pi->curr_mvr) >> 2;
    int mvp_x = mvp[0] >> 2;
    int mvp_y = mvp[1] >> 2;

    pi->min_mv[0] = COM_MAX(-MAX_CU_SIZE + 1 - x, mvp_x - max_table_offset + 4);
    pi->min_mv[1] = COM_MAX(-MAX_CU_SIZE + 1 - y, mvp_y - max_table_offset + 4);
    pi->max_mv[0] = COM_MIN(pic_width    - 1 - x, mvp_x + max_table_offset - 4);
    pi->max_mv[1] = COM_MIN(pic_height   - 1 - y, mvp_y + max_table_offset - 4);

    if (pi->min_mv[MV_X] > pi->max_mv[MV_X] || pi->min_mv[MV_Y] > pi->max_mv[MV_Y]) {
        return 0;
    }
    return 1;
}

u32 me_search_tz(inter_search_t *pi, int x, int y, int w, int h, int pic_width, int pic_height, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], int bi)
{
    s16 range[MV_RANGE_DIM][MV_D];
    com_pic_t *ref_pic = pi->refp[refi][lidx].pic;

    if (!get_max_search_range(pi, mvp, x, y, pic_width, pic_height)) {
        return COM_UINT64_MAX;
    }
    if (!bi) {
        CP32(mv, mvp);
    }
    mv[MV_X] = mv[MV_X] >> 2;
    mv[MV_Y] = mv[MV_Y] >> 2;

    u64 cost_best = COM_UINT64_MAX;
    int best_step = search_diamond(pi, x, y, w, h, refi, lidx, range, mvp, mv, &cost_best, bi, MAX_FIRST_SEARCH_STEP);

    if (!bi && (abs(mvp[MV_X] - (mv[MV_X] << 2)) >= 2 || abs(mvp[MV_Y] - (mv[MV_Y] << 2)) >= 2)) {
        if (best_step > RASTER_SEARCH_THD) {
            s16 mvt[MV_D];

            u64 cost = search_raster(pi, x, y, w, h, refi, lidx, range, mvp, mvt);

            if (cost < cost_best) {
                cost_best = cost;
                CP32(mv, mvt);
            }
        }
        s16 mvt[2] = { mv[0], mv[1] };
        u64 cost = COM_UINT64_MAX;
        search_diamond(pi, x, y, w, h, refi, lidx, range, mvp, mvt, &cost, bi, MAX_REFINE_SEARCH_STEP);
        if (cost < cost_best) {
            cost_best = cost;
            CP32(mv, mvt);
        }
    }
    mv[0] <<= 2, mv[1] <<= 2;

    if (pi->curr_mvr < 2) {
        cost_best = me_sub_pel_search(pi, x, y, w, h,  refi, lidx, mvp, mv, bi); // this cost is satd
    }

    int extra_bits = tbl_refi_bits[pi->num_refp][refi] + 
                     pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1) +
                    (bi ? pi->mot_bits[(lidx == REFP_0) ? REFP_1 : REFP_0] + 1 : 2);

    cost_best += pi->lambda_mv * (extra_bits);

    return (u32)((cost_best + (1 << 15)) >> 16);
}