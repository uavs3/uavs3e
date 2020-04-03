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

#define MV_COST64(mv_bits) (lambda_mv * (mv_bits))
#define MV_COST(mv_bits) (u32)((lambda_mv * mv_bits + (1 << 15)) >> 16)

static void avs3_inline com_ipel_range_rounding(s16 mv[MV_D], s32 mvr)
{
    if (mvr > 2) {
        int shift = mvr - 2;
        mv[0] = (mv[0] >= 0) ? ((mv[0] >> shift) << shift) : -(((-mv[0]) >> shift) << shift);
        mv[1] = (mv[1] >= 0) ? ((mv[1] >> shift) << shift) : -(((-mv[1]) >> shift) << shift);
    }
}

static void com_mv_mvr_check(s16* mv, int mvr)
{
    int mvr_mask = (1 << mvr) - 1;
    if ((mv[MV_X] & mvr_mask) || (mv[MV_Y] & mvr_mask)) {
        com_assert(0);
    }
}

static void get_raster_range(inter_search_t *pi, int lidx, int refi, s16 center[MV_D], s16 range[MV_RANGE_DIM][MV_D], int search_step)
{
    com_pic_t *ref_pic = pi->ref_pic;
    int mvr = pi->curr_mvr;
    int dist = COM_ABS((int)(pi->ptr - ref_pic->ptr));
    int max_sr = pi->max_search_range >> COM_MAX(3 - mvr, 0);
    int range_xy = COM_CLIP3(max_sr >> 2, max_sr, (max_sr * dist + (pi->gop_size >> 1)) / pi->gop_size);
    int range_x = range_xy;
    int range_y = range_xy;

    if (mvr && pi->fast_me) {
        range_x = COM_ABS(pi->mv_ipel[lidx][refi][0] - center[0]) * 4;
        range_y = COM_ABS(pi->mv_ipel[lidx][refi][1] - center[1]) * 2;
        range_x = COM_MAX(range_x, max_sr >> 2);
        range_y = COM_MAX(range_y, max_sr >> 2);
        range_x = COM_MAX(range_x, search_step * 8);
        range_y = COM_MAX(range_y, search_step * 4);
        range_x = COM_MIN(range_x, range_xy);
        range_y = COM_MIN(range_y, range_xy);
    }

    if (mvr > 2) {
        int shift = mvr - 2;
        range_x = (range_x >> shift) << shift;
        range_y = (range_y >> shift) << shift;
    }

    /* define search range for int-pel search and clip it if needs */
    range[MV_RANGE_MIN][MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], center[MV_X] - (s16)range_x);
    range[MV_RANGE_MAX][MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], center[MV_X] + (s16)range_x);
    range[MV_RANGE_MIN][MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], center[MV_Y] - (s16)range_y);
    range[MV_RANGE_MAX][MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], center[MV_Y] + (s16)range_y);

    com_assert(range[MV_RANGE_MIN][MV_X] <= range[MV_RANGE_MAX][MV_X]);
    com_assert(range[MV_RANGE_MIN][MV_Y] <= range[MV_RANGE_MAX][MV_Y]);
}


static void search_raster(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], u64 *cost_best)
{
    static const int step_base[5] = { 5, 5, 5, 6, 8 };
    static const int step_min[5] = { 1, 1, 1, 2, 4 };
    s16 range[MV_RANGE_DIM][MV_D];

    int bit_depth      = pi->bit_depth;
    int i_org          = pi->i_org;
    pel *org           = pi->org;
    com_pic_t *ref_pic = pi->ref_pic;
    u8 *tab_mvbits_x   = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y   = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx        = pi->curr_mvr;
    u32 lambda_mv      = pi->lambda_mv;
    int i_ref          = ref_pic->stride_luma;
    pel *ref           = ref_pic->y + y * i_ref + x;
    int shift          = 16 - (bit_depth - 8); // sad << 16 for u64 cost
    const int widx     = CONV_LOG2(w) - 2;
    int search_step    = COM_MAX(step_base[pi->curr_mvr], COM_MIN(w >> 1, h >> 1)) * (refi + 1);
    int round          = 0;
    int step_shift     = pi->curr_mvr - 2;

    get_raster_range(pi, lidx, refi, mv, range, search_step);

    while (search_step >= step_min[pi->curr_mvr]) {
        s16 min_cmv_x, min_cmv_y;
        s16 max_cmv_x, max_cmv_y;

        if (round++ == 0) {
            min_cmv_x = range[MV_RANGE_MIN][MV_X];
            min_cmv_y = range[MV_RANGE_MIN][MV_Y];
            max_cmv_x = range[MV_RANGE_MAX][MV_X];
            max_cmv_y = range[MV_RANGE_MAX][MV_Y];
        } else {
            min_cmv_x = COM_MAX(mv[MV_X] - search_step, pi->min_mv[MV_X]);
            min_cmv_y = COM_MAX(mv[MV_Y] - search_step, pi->min_mv[MV_Y]);
            max_cmv_x = COM_MIN(mv[MV_X] + search_step, pi->max_mv[MV_X]);
            max_cmv_y = COM_MIN(mv[MV_Y] + search_step, pi->max_mv[MV_Y]);
        }

        u32 cost_mvx[(SEARCH_RANGE_IPEL_RA * 2 + 1) / 5 + 1];
        u32* pcmvx = cost_mvx;

        for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += search_step) {
            *pcmvx++ = GET_MVBITS_IPEL_X(mv_x) * lambda_mv;
        }
        for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += search_step) {
            pel* p = ref + mv_y * i_ref;
            u32 cost_mvy = GET_MVBITS_IPEL_Y(mv_y) * lambda_mv;
            s16 mv_x = min_cmv_x;
            u32 sad[4];

            pcmvx = cost_mvx;

            for (; mv_x + 3 * search_step <= max_cmv_x; mv_x += 4 * search_step) {
                uavs3e_funs_handle.cost_sad_x4[widx](org, i_org, p + mv_x, p + mv_x + search_step, p + mv_x + search_step * 2, p + mv_x + search_step * 3, i_ref, sad, h);
                u64 cost0 = (cost_mvy + (*pcmvx++)) + ((u64)sad[0] << shift);
                u64 cost1 = (cost_mvy + (*pcmvx++)) + ((u64)sad[1] << shift);
                u64 cost2 = (cost_mvy + (*pcmvx++)) + ((u64)sad[2] << shift);
                u64 cost3 = (cost_mvy + (*pcmvx++)) + ((u64)sad[3] << shift);
                if (cost0 < *cost_best) { mv[0] = mv_x,                   mv[1] = mv_y, *cost_best = cost0; }
                if (cost1 < *cost_best) { mv[0] = mv_x + search_step,     mv[1] = mv_y, *cost_best = cost1; }
                if (cost2 < *cost_best) { mv[0] = mv_x + search_step * 2, mv[1] = mv_y, *cost_best = cost2; }
                if (cost3 < *cost_best) { mv[0] = mv_x + search_step * 3, mv[1] = mv_y, *cost_best = cost3; }
            }
            if (mv_x + 2 * search_step <= max_cmv_x) {
                uavs3e_funs_handle.cost_sad_x3[widx](org, i_org, p + mv_x, p + mv_x + search_step, p + mv_x + search_step * 2, i_ref, sad, h);
                u64 cost0 = (cost_mvy + (*pcmvx++)) + ((u64)sad[0] << shift);
                u64 cost1 = (cost_mvy + (*pcmvx++)) + ((u64)sad[1] << shift);
                u64 cost2 = (cost_mvy + (*pcmvx++)) + ((u64)sad[2] << shift);
                if (cost0 < *cost_best) { mv[0] = mv_x,                   mv[1] = mv_y, *cost_best = cost0; }
                if (cost1 < *cost_best) { mv[0] = mv_x + search_step,     mv[1] = mv_y, *cost_best = cost1; }
                if (cost2 < *cost_best) { mv[0] = mv_x + search_step * 2, mv[1] = mv_y, *cost_best = cost2; }
                mv_x += 3 * search_step;
            }
            for (; mv_x <= max_cmv_x; mv_x += search_step) {
                u64 cost = (cost_mvy + (*pcmvx++)) + block_pel_sad(widx, h, shift, org, p + mv_x, i_org, i_ref);
                if (cost < *cost_best) { mv[MV_X] = mv_x; mv[MV_Y] = mv_y; *cost_best = cost; }
            }
        }
        search_step >>= 1;

        if (pi->curr_mvr > 2) {
            search_step = (search_step >> step_shift) << step_shift;
        }
    }
}

static int search_diamond(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], u64* cost_best, int bi, int max_step)
{
    int best_step      = 0;
    int bit_depth      = pi->bit_depth;
    int i_org          = pi->i_org;
    pel *org           = pi->org;
    com_pic_t *ref_pic = pi->ref_pic;
    u8 *tab_mvbits_x   = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y   = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx        = pi->curr_mvr;
    u32 lambda_mv      = pi->lambda_mv;
    int i_ref          = ref_pic->stride_luma;
    pel *ref           = ref_pic->y + y * i_ref + x;
    int shift          = 16 - bi - (bit_depth - 8); // sad << 16 for u64 cost
    int core_size      = (bi ? 5 : 2);
    int core_step      = 1;
    const s16 center_x = mv[MV_X];
    const s16 center_y = mv[MV_Y];
    const int widx     = CONV_LOG2(w) - 2;

    if (pi->curr_mvr > 2) {
        core_size += bi;
        core_size <<= (pi->curr_mvr - 2);
        core_step <<= (pi->curr_mvr - 2);
    } 

    int min_cmv_x = center_x - core_size;
    int min_cmv_y = center_y - core_size;
    int max_cmv_x = center_x + core_size;
    int max_cmv_y = center_y + core_size;

    while (min_cmv_x <= pi->min_mv[MV_X]) min_cmv_x += core_step;
    while (min_cmv_y <= pi->min_mv[MV_Y]) min_cmv_y += core_step;
    while (max_cmv_x >= pi->max_mv[MV_X]) max_cmv_x -= core_step;
    while (max_cmv_y >= pi->max_mv[MV_Y]) max_cmv_y -= core_step;

    /* 1. try center pointer first */
    pel *p = ref + center_y * i_ref + center_x;
    u64 cost = block_pel_sad(widx, h, shift, org, p, i_org, i_ref) + (GET_MVBITS_IPEL_Y(center_y) + GET_MVBITS_IPEL_X(center_x)) * lambda_mv;

    if (cost < *cost_best) {
        mv[MV_X] = center_x;
        mv[MV_Y] = center_y;
        best_step = 0;
        *cost_best = cost;
    }

    /* 2. try core squre */
    if (max_step) {
        u32 cost_mvx[13];
        u32* pcmvx = cost_mvx;
        for (s16 mv_x = min_cmv_x; mv_x <= max_cmv_x; mv_x += core_step) {
            *pcmvx++ = GET_MVBITS_IPEL_X(mv_x) * lambda_mv;
        }
        for (s16 mv_y = min_cmv_y; mv_y <= max_cmv_y; mv_y += core_step) {
            u32 cost_mvy = GET_MVBITS_IPEL_Y(mv_y) * lambda_mv;
            pel *p = ref + mv_y * i_ref;
            s16 mv_x = min_cmv_x;
            u32 sad[4];

            pcmvx = cost_mvx;

            for (; mv_x + 3 * core_step <= max_cmv_x; mv_x += 4 * core_step) {
                uavs3e_funs_handle.cost_sad_x4[widx](org, i_org, p + mv_x, p + mv_x + core_step, p + mv_x + core_step * 2, p + mv_x + core_step * 3, i_ref, sad, h);
                u64 cost0 = (cost_mvy + (*pcmvx++)) + ((u64)sad[0] << shift);
                u64 cost1 = (cost_mvy + (*pcmvx++)) + ((u64)sad[1] << shift);
                u64 cost2 = (cost_mvy + (*pcmvx++)) + ((u64)sad[2] << shift);
                u64 cost3 = (cost_mvy + (*pcmvx++)) + ((u64)sad[3] << shift);
                if (cost0 < *cost_best) { mv[0] = mv_x,                 mv[1] = mv_y, *cost_best = cost0; }
                if (cost1 < *cost_best) { mv[0] = mv_x + core_step,     mv[1] = mv_y, *cost_best = cost1; }
                if (cost2 < *cost_best) { mv[0] = mv_x + core_step * 2, mv[1] = mv_y, *cost_best = cost2; }
                if (cost3 < *cost_best) { mv[0] = mv_x + core_step * 3, mv[1] = mv_y, *cost_best = cost3; }
            }
            if (mv_x + 2 * core_step <= max_cmv_x) {
                uavs3e_funs_handle.cost_sad_x3[widx](org, i_org, p + mv_x, p + mv_x + core_step, p + mv_x + core_step * 2, i_ref, sad, h);
                u64 cost0 = (cost_mvy + (*pcmvx++)) + ((u64)sad[0] << shift);
                u64 cost1 = (cost_mvy + (*pcmvx++)) + ((u64)sad[1] << shift);
                u64 cost2 = (cost_mvy + (*pcmvx++)) + ((u64)sad[2] << shift);
                if (cost0 < *cost_best) { mv[0] = mv_x,                 mv[1] = mv_y, *cost_best = cost0; }
                if (cost1 < *cost_best) { mv[0] = mv_x + core_step,     mv[1] = mv_y, *cost_best = cost1; }
                if (cost2 < *cost_best) { mv[0] = mv_x + core_step * 2, mv[1] = mv_y, *cost_best = cost2; }
                mv_x += 3 * core_step;
            }
            for (; mv_x <= max_cmv_x; mv_x += core_step, pcmvx++) {
                u64 cost = cost_mvy + (*pcmvx) + block_pel_sad(widx, h, shift, org, p + mv_x, i_org, i_ref);
                if (cost < *cost_best) { mv[0] = mv_x; mv[1] = mv_y; *cost_best = cost; }
            }
        }
    }

    if (mv[0] != center_x || mv[1] != center_y) {
        best_step = 2;
    }

    if (bi) {
        return best_step;
    }

    /* 3. try multilayer diamond */
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

            if (mv_x <= pi->max_mv[MV_X] && mv_x >= pi->min_mv[MV_X] &&
                mv_y <= pi->max_mv[MV_Y] && mv_y >= pi->min_mv[MV_Y]) {
                u64 cost = block_pel_sad(widx, h, shift, org, ref + mv_x + mv_y * i_ref, i_org, i_ref) + (GET_MVBITS_IPEL_X(mv_x) + GET_MVBITS_IPEL_Y(mv_y)) * lambda_mv;

                if (cost < *cost_best) {
                    mv[MV_X] = mv_x;
                    mv[MV_Y] = mv_y;
                    best_step = step;
                    *cost_best = cost;
                }
            }
        }
    }

    return best_step;
}

static u64 me_sub_pel_search(inter_search_t *pi, int x, int y, int w, int h, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], int bi)
{
    int bit_depth = pi->bit_depth;
    int i_org = pi->i_org;
    pel *org = pi->org;
    u64 cost, cost_best = COM_UINT64_MAX;
    s16 mv_x, mv_y, cx, cy;
    int i, mv_bits;
    int max_posx = pi->max_coord[MV_X];
    int max_posy = pi->max_coord[MV_Y];
    int widx = CONV_LOG2(w) - MIN_CU_LOG2;
    com_pic_t *ref_pic = pi->ref_pic;
    int i_ref = ref_pic->stride_luma;
    pel *ref = ref_pic->y + y * i_ref + x;
    u8 *tab_mvbits_x = pi->tab_mvbits - (mvp[MV_X] >> pi->curr_mvr);
    u8 *tab_mvbits_y = pi->tab_mvbits - (mvp[MV_Y] >> pi->curr_mvr);
    int mvr_idx = pi->curr_mvr;
    u32 lambda_mv = pi->lambda_mv;

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

        pel* pred = com_mc_blk_luma_pointer(ref_pic, mv_x + (x << 2), mv_y + (y << 2), max_posx, max_posy);
        cost += (u64)(calc_satd_16b(w, h, org, pred, i_org, ref_pic->stride_luma, bit_depth) >> bi) << 16;
   
        if (cost < cost_best) {
            mv[MV_X] = mv_x;
            mv[MV_Y] = mv_y;
            cost_best = cost;
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

            pel* pred = com_mc_blk_luma_pointer(ref_pic, mv_x + (x << 2), mv_y + (y << 2), max_posx, max_posy);
            cost += (u64)(calc_satd_16b(w, h, org, pred, i_org, ref_pic->stride_luma, bit_depth) >> bi) << 16;

            if (cost < cost_best) {
                mv[MV_X] = mv_x;
                mv[MV_Y] = mv_y;
                cost_best = cost;
            }
        }
    }
    return cost_best;
}

static int get_max_search_range(inter_search_t *pi, const s16 mvp[2], int x, int y, int w, int h, int pic_width, int pic_height)
{
    int max_table_offset = (pi->tab_mvbits_offset << pi->curr_mvr) >> 2;
    int mvp_x = mvp[0] >> 2;
    int mvp_y = mvp[1] >> 2;

    if (pi->is_padding) {
        pi->min_mv[0] = COM_MAX(-MAX_CU_SIZE + 1 - x, mvp_x - max_table_offset + 4);
        pi->min_mv[1] = COM_MAX(-MAX_CU_SIZE + 1 - y, mvp_y - max_table_offset + 4);
        pi->max_mv[0] = COM_MIN(pic_width  - 1 - x,   mvp_x + max_table_offset - 4);
        pi->max_mv[1] = COM_MIN(pic_height - 1 - y,   mvp_y + max_table_offset - 4);
    } else {
        pi->min_mv[0] = COM_MAX(- x, mvp_x - max_table_offset + 4);
        pi->min_mv[1] = COM_MAX(- y, mvp_y - max_table_offset + 4);
        pi->max_mv[0] = COM_MIN(pic_width  - 1 - x - w, mvp_x + max_table_offset - 4);
        pi->max_mv[1] = COM_MIN(pic_height - 1 - y - h, mvp_y + max_table_offset - 4);
    }

    com_ipel_range_rounding(pi->min_mv, pi->curr_mvr); 
    com_ipel_range_rounding(pi->max_mv, pi->curr_mvr);

    if (pi->min_mv[MV_X] > pi->max_mv[MV_X] || pi->min_mv[MV_Y] > pi->max_mv[MV_Y]) {
        return 0;
    }
    return 1;
}

u64 me_search_tz(inter_search_t *pi, int x, int y, int w, int h, int pic_width, int pic_height, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], int bi)
{
    if (!get_max_search_range(pi, mvp, x, y, w, h, pic_width, pic_height)) {
        return COM_UINT64_MAX;
    }
    if (!bi) {
        CP32(mv, mvp);
    }

    com_mv_mvr_check(mv, pi->curr_mvr);

    mv[MV_X] = COM_CLIP3(pi->min_mv[MV_X], pi->max_mv[MV_X], (mv[MV_X] + 2) >> 2);
    mv[MV_Y] = COM_CLIP3(pi->min_mv[MV_Y], pi->max_mv[MV_Y], (mv[MV_Y] + 2) >> 2);

    u64 cost_best = COM_UINT64_MAX;
    int best_step = search_diamond(pi, x, y, w, h, refi, lidx, mvp, mv, &cost_best, bi, MAX_FIRST_SEARCH_STEP);

    if (!bi && (abs(mvp[MV_X] - (mv[MV_X] << 2)) > 2 || abs(mvp[MV_Y] - (mv[MV_Y] << 2)) > 2)) {
        if (best_step > RASTER_SEARCH_THD) {
            search_raster(pi, x, y, w, h, refi, lidx, mvp, mv, &cost_best);
        }
        search_diamond(pi, x, y, w, h, refi, lidx, mvp, mv, &cost_best, bi, MAX_REFINE_SEARCH_STEP);
    }
    if (!bi) {
        CP32(pi->mv_ipel[lidx][refi], mv);
    }
    mv[0] <<= 2, mv[1] <<= 2;

    if (pi->curr_mvr < 2) {
        cost_best = me_sub_pel_search(pi, x, y, w, h,  refi, lidx, mvp, mv, bi); // this cost is satd
    }

    int extra_bits = tbl_refi_bits[pi->num_refp][refi] + 
                     pi->curr_mvr + (pi->curr_mvr < MAX_NUM_MVR - 1) +
                    (bi ? pi->mot_bits[(lidx == REFP_0) ? REFP_1 : REFP_0] + 1 : 2);

    cost_best += pi->lambda_mv * (extra_bits);

    com_mv_mvr_check(mv, pi->curr_mvr);

    return (u32)((cost_best + (1 << 15)) >> 16);
}