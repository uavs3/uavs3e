#include "global.h"

#define HEXAGON_OPI	0	//optimize hexagon search by kelly

#define SAD_SKIP_LINE(x) ((x) > 16 ? 1 : 0) 

#define COPY3_IF_LT(x,y,a,b,c,d)\
    if ((y) < (x)) {\
        (x) = (y);\
        (a) = (b);\
        (c) = (d);\
    }

#define COPY4_IF_LT(x,y,a,b,c,d,e,f)\
    if ((y) < (x)) {\
        (x) = (y);\
        (a) = (b);\
        (c) = (d);\
        (e) = (f);\
    }

/* ----------------------------------------------------------------------------
 *  Full-Pixel ME
 *----------------------------------------------------------------------------*/

static tab_i32s_t Diamond_x    [ 4] = { -1,  0,  1,  0 };
static tab_i32s_t Diamond_y    [ 4] = {  0,  1,  0, -1 };
static tab_i32s_t Hexagon_x    [ 6] = {  2,  1, -1, -2, -1,  1 };
static tab_i32s_t Hexagon_y    [ 6] = {  0, -2, -2,  0,  2,  2 };
static tab_i32s_t Big_Hexagon_x[16] = {  0, -2, -4, -4, -4, -4, -4, -2,  0,  2,  4,  4,  4,  4,  4,  2 };
static tab_i32s_t Big_Hexagon_y[16] = { 4, 3, 2, 1, 0, -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3 };

/* ---------------------------------------------------------------------------
* radius 2 hexagon
* repeated entries are to avoid having to compute mod6 every time */
static const int HEX2[8][2] = {
		{ -1, -2 },		/* 0,       0(6)   5        */
		{ -2, 0 },		/* 1,                       */
		{ -1, 2 },		/* 2,                       */
		{ 1, 2 },		/* 3,   1(7)   *       4    */
		{ 2, 0 },       /* 4,                       */
		{ 1, -2 },		/* 5,                       */
		{ -1, -2 },		/* 6,       2      3        */
		{ -2, 0 }		/* 7,                       */
};
// (x - 1) % 6
static const int M1MOD6[8] = { 5, 0, 1, 2, 3, 4, 5, 0 };

/* ---------------------------------------------------------------------------
* radius 1 diamond
* repeated entries are to avoid having to compute mod4 every time */
static const int DIA1[6][2] = {
		{ 0, -1 },		/* 0,                       */
		{ -1, 0 },		/* 1,           0(4)        */
		{ 0, 1 },		/* 2,                       */
		{ 1, 0 },		/* 3,      1(5) *   3       */
		{ 0, -1 },		/* 4,                       */
		{ -1, 0 }		/* 5,           2           */
};
// (x - 1) % 4
static const int M1MOD4[6] = { 3, 0, 1, 2, 3, 0 };

#define pack16to32_mask(x,y) (((x) << 16)|((y) & 0xFFFF))
#define pack16to32_mask2(x,y) (((x) << 16)|((y) & 0x7FFF))

#define CHECK_MVRANGE(x,y) (!(((pack16to32_mask2(x,y) + pkt_mv_min) | (pkt_mv_max - pack16to32_mask2(x,y))) & 0x80004000))

#define CHECK_MVRANGE_X4(x0,y0,x1,y1,x2,y2,x3,y3) (!((                                  \
    (pack16to32_mask2(x0, y0) + pkt_mv_min) | (pkt_mv_max - pack16to32_mask2(x0, y0)) | \
    (pack16to32_mask2(x1, y1) + pkt_mv_min) | (pkt_mv_max - pack16to32_mask2(x1, y1)) | \
    (pack16to32_mask2(x2, y2) + pkt_mv_min) | (pkt_mv_max - pack16to32_mask2(x2, y2)) | \
    (pack16to32_mask2(x3, y3) + pkt_mv_min) | (pkt_mv_max - pack16to32_mask2(x3, y3))   \
    ) & 0x80004000))

#define PartCalMadCenter(mcost, cand_x, cand_y) {                                                                                                          \
    int x = get_safe_mv(img_width,  width,  cand_x + pix_x) - pix_x;                                                                                       \
    int y = get_safe_mv(img_height, height, cand_y + pix_y) - pix_y;                                                                                       \
    mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref + y * i_ref + x, i_ref, height, skip_lines) + MV_COST_FULPEL(cand_x, cand_y);            \
}

#define SEARCH_ONE_PIXEL(x, y, mcost) {                                                                                                            \
    mcost = MAX_COST;                                                                                                                                \
    if(CHECK_MVRANGE((x), (y))) {                                                                                                                        \
        mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref + (y) * i_ref + (x), i_ref, height, skip_lines) + MV_COST_FULPEL(x, y);            \
        COPY3_IF_LT(min_mcost, mcost, best_x, (x), best_y, (y));                                                                                         \
    }                                                                                                                                                \
}

#define SEARCH_ONE_PIXEL_EXT(dx, dy, t, mcost) {                                                                                                     \
	int x = base_x + (dx), y = base_y + (dy);                                                                                                        \
    mcost = MAX_COST;                                                                                                                                \
    if(CHECK_MVRANGE(x, y)) {                                                                                                                        \
        mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref + y * i_ref + x, i_ref, height, skip_lines) + MV_COST_FULPEL(x, y);            \
        COPY4_IF_LT(min_mcost, mcost, best_x, x, best_y, y, t, 1);                                                                                   \
	}																																				 \
}

#define CAL_MCOST(x0, y0, mcost) {                                                                                                                   \
    int x = x0, y = y0;                                                                                                                              \
	if(CHECK_MVRANGE(x, y))	{                                                                                                                        \
		mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref + y * i_ref + x, i_ref, height, skip_lines) + MV_COST_FULPEL(x, y);            \
    }                                                                                                                                                \
}

#define SEARCH_FOUR_PIXELS(dx0, dy0, mcost0, dx1, dy1, mcost1, dx2, dy2, mcost2, dx3, dy3, mcost3) {                                \
    int x0 = base_x + (dx0), x1 = base_x + (dx1), x2 = base_x + (dx2), x3 = base_x + (dx3);                                         \
    int y0 = base_y + (dy0), y1 = base_y + (dy1), y2 = base_y + (dy2), y3 = base_y + (dy3);                                         \
    if(CHECK_MVRANGE_X4(x0, y0, x1, y1, x2, y2, x3, y3)) {                                                                          \
        i32u_t sad[4];                                                                                                              \
        g_funs_handle.cost_sad_x4[width_idx](p_org, i_org, p_ref + y0 * i_ref + x0,                                                 \
                                                                 p_ref + y1 * i_ref + x1,                                           \
                                                                 p_ref + y2 * i_ref + x2,                                           \
                                                                 p_ref + y3 * i_ref + x3,                                           \
                                                                                                 i_ref, sad, height, skip_lines);   \
        mcost0 = sad[0] + MV_COST_FULPEL(x0, y0);                                                                                   \
        mcost1 = sad[1] + MV_COST_FULPEL(x1, y1);                                                                                   \
        mcost2 = sad[2] + MV_COST_FULPEL(x2, y2);                                                                                   \
        mcost3 = sad[3] + MV_COST_FULPEL(x3, y3);                                                                                   \
        COPY3_IF_LT(min_mcost, mcost0, best_x, x0, best_y, y0);                                                                     \
        COPY3_IF_LT(min_mcost, mcost1, best_x, x1, best_y, y1);                                                                     \
        COPY3_IF_LT(min_mcost, mcost2, best_x, x2, best_y, y2);                                                                     \
        COPY3_IF_LT(min_mcost, mcost3, best_x, x3, best_y, y3);                                                                     \
    }  else {                                                                                                                       \
        SEARCH_ONE_PIXEL((x0), (y0), mcost0);                                                                                     \
        SEARCH_ONE_PIXEL((x1), (y1), mcost1);                                                                                     \
        SEARCH_ONE_PIXEL((x2), (y2), mcost2);                                                                                     \
        SEARCH_ONE_PIXEL((x3), (y3), mcost3);                                                                                     \
    }                                                                                                                               \
}

#define SEARCH_DIAMOND(mcost0, mcost1, mcost2, mcost3) \
    SEARCH_FOUR_PIXELS(- 1, + 0, mcost0, \
                       + 0, + 1, mcost1, \
                       + 1, + 0, mcost2, \
                       + 0, - 1, mcost3);

#define SEARCH_DIAMOND_EXT(i, mcost0, mcost1, mcost2, mcost3, mcost4, mcost5, mcost6, mcost7)   \
    SEARCH_FOUR_PIXELS(- (2 << i), + (0 << i), mcost0,  \
                       + (0 << i), + (2 << i), mcost1,  \
                       + (2 << i), + (0 << i), mcost2,  \
                       + (0 << i), - (2 << i), mcost3); \
    SEARCH_FOUR_PIXELS(- (1 << i), + (1 << i), mcost4,  \
                       + (1 << i), + (1 << i), mcost5,  \
                       + (1 << i), - (1 << i), mcost6,  \
                       - (1 << i), - (1 << i), mcost7); \
    if (last_bestx == best_x && last_besty == best_y) { \
        round++;                                        \
    } else {                                            \
        round = 0;                                      \
    }                                                   \
    last_bestx = best_x; last_besty = best_y;

#define SEARCH_ONE_PIXEL_EX(x0, y0, flag) {																										     \
    int x = x0, y = y0;                                                                                                                              \
    if(CHECK_MVRANGE(x, y)) {                                                                                                                        \
        i32u_t mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref + y * i_ref + x, i_ref, height, skip_lines) + MV_COST_FULPEL(x, y);     \
        COPY4_IF_LT(min_mcost, mcost, best_x, x, best_y, y, dir, flag);                                                                              \
	}																																				 \
}

#define SEARCH_ONE_PIXEL_EX_R(x0, y0, flag, mcost) {                                                                                                 \
    int x = x0, y = y0;                                                                                                                              \
    mcost = MAX_COST;                                                                                                                                \
    if(CHECK_MVRANGE(x, y)) {                                                                                                                        \
        mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref + y * i_ref + x, i_ref, height, skip_lines) + MV_COST_FULPEL(x, y);            \
        COPY4_IF_LT(min_mcost, mcost, best_x, x, best_y, y, dir, flag);                                                                              \
	}																																				 \
}

#define SEARCH_FOUR_PIXELS_EX(dx0, dy0, f0, dx1, dy1, f1, dx2, dy2, f2, dx3, dy3, f3) {				                                \
    int x0 = base_x + (dx0), x1 = base_x + (dx1), x2 = base_x + (dx2), x3 = base_x + (dx3);                                         \
    int y0 = base_y + (dy0), y1 = base_y + (dy1), y2 = base_y + (dy2), y3 = base_y + (dy3);                                         \
    if(CHECK_MVRANGE_X4(x0, y0, x1, y1, x2, y2, x3, y3)) {                                                                          \
        i32u_t sad[4];                                                                                                              \
        g_funs_handle.cost_sad_x4[width_idx](p_org, i_org, p_ref + y0 * i_ref + x0,                                                 \
                                                                 p_ref + y1 * i_ref + x1,                                           \
                                                                 p_ref + y2 * i_ref + x2,                                           \
                                                                 p_ref + y3 * i_ref + x3,                                           \
                                                                                                 i_ref, sad, height, skip_lines);   \
        mcost0 = sad[0] + MV_COST_FULPEL(x0, y0);                                                                                   \
        mcost1 = sad[1] + MV_COST_FULPEL(x1, y1);                                                                                   \
        mcost2 = sad[2] + MV_COST_FULPEL(x2, y2);                                                                                   \
        mcost3 = sad[3] + MV_COST_FULPEL(x3, y3);                                                                                   \
        COPY4_IF_LT(min_mcost, mcost0, best_x, x0, best_y, y0, dir, f0);                                                            \
        COPY4_IF_LT(min_mcost, mcost1, best_x, x1, best_y, y1, dir, f1);                                                            \
        COPY4_IF_LT(min_mcost, mcost2, best_x, x2, best_y, y2, dir, f2);                                                            \
        COPY4_IF_LT(min_mcost, mcost3, best_x, x3, best_y, y3, dir, f3);                                                            \
	}  else {																													    \
        SEARCH_ONE_PIXEL_EX((dx0), (dy0), f0);																					    \
        SEARCH_ONE_PIXEL_EX((dx1), (dy1), f1);																					    \
        SEARCH_ONE_PIXEL_EX((dx2), (dy2), f2);																					    \
        SEARCH_ONE_PIXEL_EX((dx3), (dy3), f3);																					    \
	}                                                                                                                               \
}

#define SEARCH_FOUR_PIXELS_EX_R(dx0, dy0, f0, mcost0, dx1, dy1, f1, mcost1, dx2, dy2, f2, mcost2, dx3, dy3, f3, mcost3) {		\
    int x0 = base_x + (dx0), x1 = base_x + (dx1), x2 = base_x + (dx2), x3 = base_x + (dx3);                                     \
    int y0 = base_y + (dy0), y1 = base_y + (dy1), y2 = base_y + (dy2), y3 = base_y + (dy3);                                     \
    if(CHECK_MVRANGE_X4(x0, y0, x1, y1, x2, y2, x3, y3)) {                                                                      \
        i32u_t sad[4];                                                                                                          \
        g_funs_handle.cost_sad_x4[width_idx](p_org, i_org, p_ref + y0 * i_ref + x0,                                \
                                                                 p_ref + y1 * i_ref + x1,                                \
                                                                 p_ref + y2 * i_ref + x2,                                \
                                                                 p_ref + y3 * i_ref + x3,                                \
                                                                                                 i_ref, sad, height, skip_lines); \
        mcost0 = sad[0] + MV_COST_FULPEL(x0, y0);                                                                               \
        mcost1 = sad[1] + MV_COST_FULPEL(x1, y1);                                                                               \
        mcost2 = sad[2] + MV_COST_FULPEL(x2, y2);                                                                               \
        mcost3 = sad[3] + MV_COST_FULPEL(x3, y3);                                                                               \
        COPY4_IF_LT(min_mcost, mcost0, best_x, x0, best_y, y0, dir, f0);                                                        \
        COPY4_IF_LT(min_mcost, mcost1, best_x, x1, best_y, y1, dir, f1);                                                        \
        COPY4_IF_LT(min_mcost, mcost2, best_x, x2, best_y, y2, dir, f2);                                                        \
        COPY4_IF_LT(min_mcost, mcost3, best_x, x3, best_y, y3, dir, f3);                                                        \
	}  else {																													\
        SEARCH_ONE_PIXEL_EX_R((dx0), (dy0), f0, mcost0);																		\
        SEARCH_ONE_PIXEL_EX_R((dx1), (dy1), f1, mcost1);																		\
        SEARCH_ONE_PIXEL_EX_R((dx2), (dy2), f2, mcost2);																		\
        SEARCH_ONE_PIXEL_EX_R((dx3), (dy3), f3, mcost3);																		\
	}                                                                                                                           \
}

#define UPDATE_SEARCH_CENTER() {base_x = best_x; base_y = best_y;}

int me_pre_search(me_info_t *me, i16s_t *pmv, i16s_t *mv, i32u_t *ret_mcost, i32u_t *d_mcost, int skip_lines)
{
    int img_width   = me->img_width;
    int img_height  = me->img_height;
    int width       = me->blk_width;
    int height      = me->blk_height;
    int pix_x       = me->blk_x;
    int pix_y       = me->blk_y;
    pel_t *p_org    = me->p_org;
    int i_org       = me->i_org;
    int i_ref       = me->i_ref;
    pel_t *p_ref    = me->p_ref;

    int width_idx = width >> 2;
    i32u_t pkt_mv_min = pack16to32_mask2(-me->min_mv_x, -me->min_mv_y);
    i32u_t pkt_mv_max = pack16to32_mask2(me->max_mv_x, me->max_mv_y) | 0x8000;

    i32u_t min_mcost = MAX_COST;
    i32u_t mcost;

    const int center_x = (pmv[0] + 2) >> 2;
    const int center_y = (pmv[1] + 2) >> 2;

    int start_mvc = 1;
    int best_x, best_y, base_x, base_y;
    int i;

    i32u_t *tab_mvbits_x = me->tab_mvbits - pmv[0];
    i32u_t *tab_mvbits_y = me->tab_mvbits - pmv[1];

    PartCalMadCenter(min_mcost, center_x, center_y);
    best_x = center_x;
    best_y = center_y;

    if (me->out_of_range) {
        mv[0] = (i16s_t)best_x;
        mv[1] = (i16s_t)best_y;
        *ret_mcost = min_mcost;
        return 0;
    }

    if (center_x | center_y) {
        start_mvc++;
        if (CHECK_MVRANGE(0, 0) || (pmv[0] < 2048 && pmv[0] > -2048 && pmv[1] < 2048 && pmv[1] > -2048)) {
            mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref, i_ref, height, skip_lines) + MV_COST_FULPEL(0, 0);
            COPY3_IF_LT(min_mcost, mcost, best_x, 0, best_y, 0);
        } 
    }

    for (i = start_mvc; i < me->i_mvc; i++) {
        int x = me->mvc[i][0];
        int y = me->mvc[i][1];
        if (CHECK_MVRANGE(x, y)) {
            mcost = g_funs_handle.cost_sad[width_idx](p_org, i_org, p_ref + y * i_ref + x, i_ref, height, skip_lines) + MV_COST_FULPEL(x, y);
            COPY3_IF_LT(min_mcost, mcost, best_x, x, best_y, y);
        }
    }

    UPDATE_SEARCH_CENTER();
    SEARCH_DIAMOND(d_mcost[0], d_mcost[1], d_mcost[2], d_mcost[3]);

    mv[0] = (i16s_t)best_x;
    mv[1] = (i16s_t)best_y;
    *ret_mcost = min_mcost;

    return 1;
}

int me_integer_tz(me_info_t *me, i16s_t *pmv, i16s_t *mv, i32u_t *d_mcost, i32u_t *s_mcost, i32u_t min_mcost, int skip_lines, int layer)
{
    int width      = me->blk_width;
    int height     = me->blk_height;
    pel_t *p_org   = me->p_org;
    int i_org      = me->i_org;
    int i_ref      = me->i_ref;
    pel_t *p_ref   = me->p_ref;

    int width_idx = width >> 2;

    i32u_t pkt_mv_min = pack16to32_mask2(-me->min_mv_x, -me->min_mv_y);
    i32u_t pkt_mv_max = pack16to32_mask2(me->max_mv_x, me->max_mv_y) | 0x8000;

    i32u_t mcost0, mcost1, mcost2, mcost3;

    const int center_x = (pmv[0] + 2) >> 2;
    const int center_y = (pmv[1] + 2) >> 2;
    const int mv_center = pack16to32_mask2(center_x, center_y);
    int mv_pkt;
    int best_x, best_y, base_x, base_y;
    int temp_x, temp_y, distance_center;
    int i, j;
    //record
    int d_x[4] = { 0, 0, 0, 0 };
    int d_y[4] = { 0, 0, 0, 0 };
    i32u_t d2_mcost_1[4] = { MAX_COST, MAX_COST, MAX_COST, MAX_COST };	//EXT0 diamond
    i32u_t d2_mcost_2[4] = { MAX_COST, MAX_COST, MAX_COST, MAX_COST };	//EXT0 square
    i32u_t d4_mcost_2[4] = { MAX_COST, MAX_COST, MAX_COST, MAX_COST };	//EXT1 square
    i32u_t base_cost, base2_cost, t_mcost1, t_mcost2;
    int choice1 = 0, choice2 = 0;
    int round;
    int last_bestx, last_besty;
    i32u_t *tab_mvbits_x = me->tab_mvbits - pmv[0];
    i32u_t *tab_mvbits_y = me->tab_mvbits - pmv[1];

    base_x = best_x = mv[0]; 
    base_y = best_y = mv[1];

    mv_pkt = pack16to32_mask2(best_x, best_y);
    base_cost = min_mcost;

    if (mv_pkt != mv_center && mv_pkt != 0) {
        SEARCH_DIAMOND(d_mcost[0], d_mcost[1], d_mcost[2], d_mcost[3]);
    }

    round = 0;
    last_bestx = base_x;
    last_besty = base_y;
    SEARCH_DIAMOND_EXT(0, d2_mcost_1[0], d2_mcost_1[1], d2_mcost_1[2], d2_mcost_1[3], d2_mcost_2[0], d2_mcost_2[1], d2_mcost_2[2], d2_mcost_2[3]);

    if (best_x != base_x || best_y != base_y) {
        SEARCH_DIAMOND_EXT(1, mcost0, mcost1, mcost2, mcost3, d4_mcost_2[0], d4_mcost_2[1], d4_mcost_2[2], d4_mcost_2[3]);
        SEARCH_DIAMOND_EXT(2, mcost0, mcost1, mcost2, mcost3, mcost0, mcost1, mcost2, mcost3);

        if (layer < 3) {
            if (round < 2) {
                SEARCH_DIAMOND_EXT(3, mcost0, mcost1, mcost2, mcost3, mcost0, mcost1, mcost2, mcost3);
            }
            if (layer < 2) {
                if (round < 2) {
                    SEARCH_DIAMOND_EXT(4, mcost0, mcost1, mcost2, mcost3, mcost0, mcost1, mcost2, mcost3);
                }
                if (layer < 1) {
                    if (round < 2) {
                        SEARCH_DIAMOND_EXT(5, mcost0, mcost1, mcost2, mcost3, mcost0, mcost1, mcost2, mcost3);
                    }
                }
            }
        }
    }

    temp_x = best_x - base_x;
    temp_y = best_y - base_y;

    if ((temp_x | temp_y) == 0) {
        for (i = 0; i < 4; i++) {
            s_mcost[i] = d2_mcost_2[i];
        }
        goto tz_end;
    }

    distance_center = temp_x*temp_x + temp_y*temp_y;

    if (distance_center == 1) {
        base2_cost = min_mcost;
        if ((best_x - base_x) == 0 && (best_y - base_y) == 1) {
            SEARCH_ONE_PIXEL_EXT(-1, +2, choice1, t_mcost1);
            SEARCH_ONE_PIXEL_EXT(+1, +2, choice2, t_mcost2);
            if (me->b_fast_fme) {
                if (choice1 == 0 && choice2 == 0){
                    s_mcost[0] = t_mcost1;
                    s_mcost[1] = t_mcost2;
                    s_mcost[2] = d_mcost[2];
                    s_mcost[3] = d_mcost[0];
                    d_mcost[0] = d2_mcost_2[0];
                    d_mcost[1] = d2_mcost_1[1];
                    d_mcost[2] = d2_mcost_2[1];
                    d_mcost[3] = base_cost;
                    d_y[0]++; d_y[1]++; d_y[2]++; d_y[3]++;
                } else if (choice1){	//base(-1. 2)
                    s_mcost[2] = base2_cost;
                    CAL_MCOST(-2, 3, s_mcost[0]);
                    CAL_MCOST(0, 3, s_mcost[1]);
                    CAL_MCOST(-2, 1, s_mcost[3]);
                    d_mcost[3] = d2_mcost_2[0];
                    d_mcost[2] = d2_mcost_1[1];
                    d_mcost[0] = d4_mcost_2[0];
                    CAL_MCOST(-1, 3, d_mcost[1]);
                    d_x[0]--; d_x[1]--; d_x[2]--; d_x[3]--;
                    d_y[0] += 2; d_y[1] += 2; d_y[2] += 2; d_y[3] += 2;
                } else{ //base(1,2)
                    s_mcost[3] = base2_cost;
                    CAL_MCOST(0, 3, s_mcost[0]);
                    CAL_MCOST(2, 3, s_mcost[1]);
                    CAL_MCOST(2, 1, s_mcost[2]);
                    d_mcost[3] = d2_mcost_2[1];
                    d_mcost[2] = d4_mcost_2[1];
                    d_mcost[0] = d2_mcost_1[1];
                    CAL_MCOST(1, 3, d_mcost[1]);
                    d_x[0]++; d_x[1]++; d_x[2]++; d_x[3]++;
                    d_y[0] += 2; d_y[1] += 2; d_y[2] += 2; d_y[3] += 2;
                }
            }
        }
        if ((best_x - base_x) == 0 && (best_y - base_y) == -1) {
            SEARCH_ONE_PIXEL_EXT(-1, -2, choice1, t_mcost1);
            SEARCH_ONE_PIXEL_EXT(+1, -2, choice2, t_mcost2);
            if (me->b_fast_fme) {
                if (choice1 == 0 && choice2 == 0){
                    s_mcost[0] = d_mcost[0];
                    s_mcost[1] = d_mcost[2];
                    s_mcost[2] = t_mcost2;
                    s_mcost[3] = t_mcost1;
                    d_mcost[0] = d2_mcost_2[3];
                    d_mcost[1] = base_cost;
                    d_mcost[2] = d2_mcost_2[2];
                    d_mcost[3] = d2_mcost_1[3];
                    d_y[0]--; d_y[1]--; d_y[2]--; d_y[3]--;
                } else if (choice1){	//base(-1,-2)
                    s_mcost[1] = base2_cost;
                    CAL_MCOST(-2, -1, s_mcost[0]);
                    CAL_MCOST(0, -3, s_mcost[2]);
                    CAL_MCOST(-2, -3, s_mcost[3]);
                    d_mcost[1] = d2_mcost_2[3];
                    d_mcost[2] = d2_mcost_1[3];
                    d_mcost[0] = d4_mcost_2[3];
                    CAL_MCOST(-1, -3, d_mcost[3]);
                    d_x[0]--; d_x[1]--; d_x[2]--; d_x[3]--;
                    d_y[0] -= 2; d_y[1] -= 2; d_y[2] -= 2; d_y[3] -= 2;
                } else{	//base(1,-2)
                    s_mcost[0] = base2_cost;
                    CAL_MCOST(2, -1, s_mcost[1]);
                    CAL_MCOST(2, -3, s_mcost[2]);
                    CAL_MCOST(0, -3, s_mcost[3]);
                    d_mcost[1] = d2_mcost_2[2];
                    d_mcost[2] = d4_mcost_2[1];
                    d_mcost[0] = d2_mcost_1[3];
                    CAL_MCOST(1, -3, d_mcost[3]);
                    d_x[0]++; d_x[1]++; d_x[2]++; d_x[3]++;
                    d_y[0] -= 2; d_y[1] -= 2; d_y[2] -= 2; d_y[3] -= 2;
                }
            }
        }
        if ((best_x - base_x) == -1 && (best_y - base_y) == 0) {
            SEARCH_ONE_PIXEL_EXT(-2, -1, choice1, t_mcost1);
            SEARCH_ONE_PIXEL_EXT(-2, +1, choice2, t_mcost2);
            if (me->b_fast_fme) {
                if (choice1 == 0 && choice2 == 0){
                    s_mcost[0] = t_mcost2;
                    s_mcost[1] = d_mcost[1];
                    s_mcost[2] = d_mcost[3];
                    s_mcost[3] = t_mcost1;
                    d_mcost[0] = d2_mcost_1[0];
                    d_mcost[1] = d2_mcost_2[0];
                    d_mcost[2] = base_cost;
                    d_mcost[3] = d2_mcost_2[3];
                    d_x[0]--; d_x[1]--; d_x[2]--; d_x[3]--;
                } else if (choice1){	//base(-2,-1)
                    s_mcost[1] = base2_cost;
                    CAL_MCOST(-3, 0, s_mcost[0]);
                    CAL_MCOST(-1, -2, s_mcost[2]);
                    CAL_MCOST(-3, -2, s_mcost[3]);
                    d_mcost[1] = d2_mcost_1[0];
                    d_mcost[2] = d2_mcost_2[3];
                    d_mcost[3] = d4_mcost_2[3];
                    CAL_MCOST(-3, -1, d_mcost[0]);
                    d_x[0] -= 2; d_x[1] -= 2; d_x[2] -= 2; d_x[3] -= 2;
                    d_y[0]--; d_y[1]--; d_y[2]--; d_y[3]--;
                } else{	//base(-2,1)
                    s_mcost[2] = base2_cost;
                    CAL_MCOST(-3, 2, s_mcost[0]);
                    CAL_MCOST(-1, 2, s_mcost[1]);
                    CAL_MCOST(-3, 0, s_mcost[3]);
                    d_mcost[1] = d4_mcost_2[0];
                    d_mcost[2] = d2_mcost_2[0];
                    d_mcost[3] = d2_mcost_1[0];
                    CAL_MCOST(-3, 1, d_mcost[0]);
                    d_x[0] -= 2; d_x[1] -= 2; d_x[2] -= 2; d_x[3] -= 2;
                    d_y[0]++; d_y[1]++; d_y[2]++; d_y[3]++;
                }
            }
        }
        if ((best_x - base_x) == 1 && (best_y - base_y) == 0) {
            SEARCH_ONE_PIXEL_EXT(+2, +1, choice1, t_mcost1);
            SEARCH_ONE_PIXEL_EXT(+2, -1, choice2, t_mcost2);
            if (me->b_fast_fme) {
                if (choice1 == 0 && choice2 == 0){
                    s_mcost[0] = d_mcost[1];
                    s_mcost[1] = t_mcost1;
                    s_mcost[2] = t_mcost2;
                    s_mcost[3] = d_mcost[3];
                    d_mcost[0] = base_cost;
                    d_mcost[1] = d2_mcost_2[1];
                    d_mcost[2] = d2_mcost_1[2];
                    d_mcost[3] = d2_mcost_2[2];
                    d_x[0]++; d_x[1]++; d_x[2]++; d_x[3]++;
                } else if (choice1){ //base(2,1)
                    s_mcost[3] = base2_cost;
                    CAL_MCOST(1, 2, s_mcost[0]);
                    CAL_MCOST(3, 2, s_mcost[1]);
                    CAL_MCOST(3, 0, s_mcost[2]);
                    d_mcost[0] = d2_mcost_2[1];
                    d_mcost[1] = d4_mcost_2[1];
                    d_mcost[3] = d2_mcost_1[2];
                    CAL_MCOST(3, 1, d_mcost[2]);
                    d_x[0] += 2; d_x[1] += 2; d_x[2] += 2; d_x[3] += 2;
                    d_y[0]++; d_y[1]++; d_y[2]++; d_y[3]++;
                } else{	//base(2,-1)
                    s_mcost[0] = base2_cost;
                    CAL_MCOST(3, 0, s_mcost[1]);
                    CAL_MCOST(3, -2, s_mcost[2]);
                    CAL_MCOST(1, -2, s_mcost[3]);
                    d_mcost[0] = d2_mcost_2[2];
                    d_mcost[1] = d2_mcost_1[2];
                    d_mcost[3] = d4_mcost_2[2];
                    CAL_MCOST(3, -1, d_mcost[2]);
                    d_x[0] += 2; d_x[1] += 2; d_x[2] += 2; d_x[3] += 2;
                    d_y[0]--; d_y[1]--; d_y[2]--; d_y[3]--;
                }
            }
        }
        goto tz_end;
    } else if (distance_center > 16) { //光栅搜索步长为4
        int scan_range = 16;

        switch (layer) {
        case 2:
            scan_range -= 4;
            break;
        case 3:
            scan_range -= 8;
            break;
        default:
            break;
        }
  
        UPDATE_SEARCH_CENTER();
        for (i = -scan_range + 2; i < scan_range + 2; i += 8) {
            for (j = -scan_range + 2; j < scan_range + 2; j += 8) {
                SEARCH_FOUR_PIXELS(+ j    , + i    , mcost0, \
                                   + j + 4, + i    , mcost1, \
                                   + j    , + i + 4, mcost2, \
                                   + j + 4, + i + 4, mcost3);
            }
        }
    }

    do {
        mv_pkt = pack16to32_mask2(best_x, best_y);

        UPDATE_SEARCH_CENTER();

        if (mv_pkt != mv_center && mv_pkt != 0) {
            SEARCH_DIAMOND(d_mcost[0], d_mcost[1], d_mcost[2], d_mcost[3]);
        }
        SEARCH_DIAMOND_EXT(0, mcost0, mcost1, mcost2, mcost3, s_mcost[0], s_mcost[1], s_mcost[2], s_mcost[3]);
    } while (base_x != best_x || base_y != best_y);

tz_end:

    mv[0] = (i16s_t)best_x;
    mv[1] = (i16s_t)best_y;

    return min_mcost;
}

int me_integer_hex(me_info_t *me, i16s_t *pmv, i16s_t *mv, i32u_t *d_mcost, i32u_t *s_mcost, i32u_t min_mcost, int skip_lines)
{
	int width      = me->blk_width;
	int height     = me->blk_height;
    pel_t *p_org   = me->p_org;
    int i_org      = me->i_org;
    int i_ref      = me->i_ref;
    pel_t *p_ref   = me->p_ref;

	int width_idx = width >> 2;
    
	i32u_t pkt_mv_min = pack16to32_mask2(-me->min_mv_x, -me->min_mv_y);
	i32u_t pkt_mv_max = pack16to32_mask2(me->max_mv_x, me->max_mv_y) | 0x8000;

	i32u_t base_cost, mcost0, mcost1, mcost2, mcost3;

	const int center_x = (pmv[0] + 2) >> 2;
	const int center_y = (pmv[1] + 2) >> 2;
	int mv_pkt, pmv_pkt;
	int best_x, best_y, base_x, base_y, mx, my;
	int i, dir;

	i32u_t *tab_mvbits_x = me->tab_mvbits - pmv[0];
	i32u_t *tab_mvbits_y = me->tab_mvbits - pmv[1];

	int me_range = MAX_SEARCH_RANGE;

	/* step 1 ------------------------------------------------------
	* get best start search point */
    base_x = best_x = mv[0];
    base_y = best_y = mv[1];

    mv_pkt = pack16to32_mask2(best_x, best_y);
    base_cost = min_mcost;

	/* step 2 ------------------------------------------------------
	* 3x3 full search. is the central point the best matching one? */
	dir = 0;
	pmv_pkt = pack16to32_mask2(center_x, center_y);

	if (mv_pkt && mv_pkt != pmv_pkt) {
		SEARCH_FOUR_PIXELS_EX(-1, -1, 1, 0, -1, 2, 1, -1, 3, -1, 0, 4);
		SEARCH_FOUR_PIXELS_EX(1, 0, 5, -1, 1, 6, 0, 1, 7, 1, 1, 8);
		/* 1,               */
		/* 2,   1   2   3   */
		/* 3,               */
		/* 4,   4   *   5   */
		/* 5,               */
		/* 6,   6,  7,  8   */
		/* 7,               */
		/* 8,               */
	}
	else {
		SEARCH_FOUR_PIXELS_EX(-1, -1, 1, 1, -1, 3, -1, 1, 6, 1, 1, 8);
		/* 1,   1        3  */
		/* 3,               */
		/* 6,               */
		/* 8,   6        8  */
	}

	/* step 3 ------------------------------------------------------
	* mix-pattern search. if the best matching point is not in the central */
	if (dir) {
		/* step 3.1, cross search */
		int cross_flag = 0;
		int idx;                /* index of array of search pattern */

		/* horizontal direction */
		UPDATE_SEARCH_CENTER();
		for (i = 2; i < me_range; i += 2) {
			mx = base_x - i;
			dir = 0;
			SEARCH_ONE_PIXEL_EX(mx, base_y, 1);      /* left */
			mx = base_x + i;
			SEARCH_ONE_PIXEL_EX(mx, base_y, 2);      /* right */
			if (!dir) {
				if (i == 2) {
					cross_flag = 1;
				}
				break;          /* early terminate */
			}
		}

		/* vertical direction */
		UPDATE_SEARCH_CENTER();
		for (i = 2; i < me_range; i += 2) {
			my = base_y - i;
			dir = 0;
			SEARCH_ONE_PIXEL_EX(base_x, my, 4);      /* top */
			my = base_y + i;
			SEARCH_ONE_PIXEL_EX(base_x, my, 7);      /* bottom */
			if (!dir) {
				if (i == 2) {
					cross_flag++;
				}
				break;          /* early terminate */
			}
		}

		me_range >>= 1;         /* smaller search range */


		/* step 3.2, hexagon/octagon search */
#if HEXAGON_OPI
		if (cross_flag != 2) {
            int iDist;
			dir = 0;
			UPDATE_SEARCH_CENTER();

			//mutiple layers of hexagon search
			for (iDist = 2; iDist < MAX_SEARCH_RANGE + 1; iDist <<= 1) {

				const int iTop = base_y - iDist;
				const int iBottom = base_y + iDist;
				const int iLeft = base_x - iDist;
				const int iRight = base_x + iDist;
				const int iLeft1 = base_x - (iDist >> 1);
				const int iRight1 = base_x + (iDist >> 1);

				const int iMRight = (iRight + iRight1) >> 1;
				const int iMLeft = (iLeft + iLeft1) >> 1;
				const int iMTop = (iTop + base_y) >> 1;
				const int iMBottom = (iBottom + base_y) >> 1;

				SEARCH_FOUR_PIXELS_EX(
					iLeft1, iTop, 6,
					iRight1, iTop, 5,
					iLeft, base_y, 1,
					iRight, base_y, 4
					);
				SEARCH_FOUR_PIXELS_EX(
					iLeft1, iBottom, 2,
					iRight1, iBottom, 3,
					base_x, iBottom, 7,
					base_x, iTop, 8
					);
				if (iDist > 2){
					SEARCH_FOUR_PIXELS_EX(
						iMLeft, iMBottom, 9,
						iMRight, iMBottom, 10,
						iMRight, iMTop, 11,
						iMLeft, iMTop, 12
						);
				}
			}
			//iteration
			if(dir){
				dir = 0;
				UPDATE_SEARCH_CENTER();
				/* hexagon search */								/*       6     5        */
				SEARCH_ONE_PIXEL_EX(base_x - 1, base_y - 2, 6);     /*                      */
				SEARCH_ONE_PIXEL_EX(base_x + 1, base_y - 2, 5);     /*                      */
				SEARCH_ONE_PIXEL_EX(base_x - 2, base_y, 1);			/*   1      *      4    */
				SEARCH_ONE_PIXEL_EX(base_x + 2, base_y, 4);			/*                      */
				SEARCH_ONE_PIXEL_EX(base_x - 1, base_y + 2, 2);     /*                      */
				SEARCH_ONE_PIXEL_EX(base_x + 1, base_y + 2, 3);     /*       2     3        */
				if (dir) {
					idx = dir - 1;      /* start array index */
					/* half hexagon, not overlapping the previous iteration */
					for (i = 0; i < me_range; i++) {
						dir = 0;
						UPDATE_SEARCH_CENTER();
						SEARCH_ONE_PIXEL_EX(base_x + HEX2[idx][0], base_y + HEX2[idx][1], 1);
						SEARCH_ONE_PIXEL_EX(base_x + HEX2[idx + 1][0], base_y + HEX2[idx + 1][1], 2);
						SEARCH_ONE_PIXEL_EX(base_x + HEX2[idx + 2][0], base_y + HEX2[idx + 2][1], 3);
						if (!dir) {
							break;      /* early terminate */
						}
						idx = M1MOD6[dir + idx - 1];    /* next start array index */
					}
				}
			}
		}
#else
		if (cross_flag != 2) {
			dir = 0;
			UPDATE_SEARCH_CENTER();
			/* hexagon search */								/*       6     5        */
			SEARCH_ONE_PIXEL_EX(base_x - 1, base_y - 2, 6);     /*                      */
			SEARCH_ONE_PIXEL_EX(base_x + 1, base_y - 2, 5);     /*                      */
			SEARCH_ONE_PIXEL_EX(base_x - 2, base_y, 1);			/*   1      *      4    */
			SEARCH_ONE_PIXEL_EX(base_x + 2, base_y, 4);			/*                      */
			SEARCH_ONE_PIXEL_EX(base_x - 1, base_y + 2, 2);     /*                      */
			SEARCH_ONE_PIXEL_EX(base_x + 1, base_y + 2, 3);     /*       2     3        */
			if (dir) {
				idx = dir - 1;      /* start array index */
				/* half hexagon, not overlapping the previous iteration */
				for (i = 0; i < me_range; i++) {
					dir = 0;
					UPDATE_SEARCH_CENTER();
					SEARCH_ONE_PIXEL_EX(base_x + HEX2[idx][0], base_y + HEX2[idx][1], 1);
					SEARCH_ONE_PIXEL_EX(base_x + HEX2[idx + 1][0], base_y + HEX2[idx + 1][1], 2);
					SEARCH_ONE_PIXEL_EX(base_x + HEX2[idx + 2][0], base_y + HEX2[idx + 2][1], 3);
					if (!dir) {
						break;      /* early terminate */
					}
					idx = M1MOD6[dir + idx - 1];    /* next start array index */
				}
			}
		}
#endif

		/* step 3.3, refine: diamond search */
		dir = 0;
		UPDATE_SEARCH_CENTER();
		SEARCH_FOUR_PIXELS_EX_R(0, -1, 4, d_mcost[3], -1, 0, 1, d_mcost[0], 1, 0, 3, d_mcost[2], 0, 1, 2, d_mcost[1]);
		/*    4    */
		/*  1 * 3  */
		/*    2    */
		if (me->b_fast_fme){
			SEARCH_FOUR_PIXELS_EX_R(-1, 1, 1, s_mcost[0], 1, 1, 2, s_mcost[1], 1, -1, 3, s_mcost[2], -1, -1, 4, s_mcost[3]);
			/*  4   3  */
			/*    *    */
			/*  1   2  */
		}

	}

	mv[0] = (i16s_t)best_x;
	mv[1] = (i16s_t)best_y;

	return min_mcost;
}


/* ----------------------------------------------------------------------------
*  Sub-Pixel ME
*----------------------------------------------------------------------------*/

tab_char_t tab_square_pos[16][2] = {
    { -1,  0 }, {  1,  0 }, {  0, -1 }, {  0,  1 }, 
    { -1, -1 }, {  1, -1 }, { -1,  1 }, {  1,  1 },
    { -2,  0 }, {  2,  0 }, {  0, -2 }, {  0,  2 }, 
    { -2, -2 }, {  2, -2 }, { -2,  2 }, {  2,  2 }
};

#define CAL_SUBPEL_MCOST(cx, cy, mcost) {																\
	image_t *p_img = &(*ref_img)[cy & 3][cx & 3];														\
	int x = get_safe_mv(input->img_width, width, pic_pix_x + (cx >> 2));								\
	int y = get_safe_mv(input->img_height, height, pic_pix_y + (cy >> 2));								\
	int i_src = p_img->i_stride[0];																		\
	pel_t *src = p_img->plane[0] + y * i_src + x;														\
    mcost = g_funs_handle.cost_sad[width >> 2](org, CACHE_STRIDE, src, i_src, height, 0);			    \
	mcost += MV_COST_SUBPEL(cx, cy);																	\
}

#define CHECK_SUBPEL(dx, dy, mcost) {                                                                       \
    int cx = center[0] + (dx), cy = center[1] + (dy);                                                       \
    CAL_SUBPEL_MCOST(cx, cy, mcost);                                                                        \
    COPY3_IF_LT(min_mcost, mcost, best_x, cx, best_y, cy);                                                  \
}

#define CHECK_SUBPEL_X4(dx0, dy0, dx1, dy1, dx2, dy2, dx3, dy3) {                                           \
    i32u_t sad[4];                                                                                          \
    int cx0 = center[0] + (dx0), cy0 = center[1] + (dy0);                                                   \
    int cx1 = center[0] + (dx1), cy1 = center[1] + (dy1);                                                   \
    int cx2 = center[0] + (dx2), cy2 = center[1] + (dy2);                                                   \
    int cx3 = center[0] + (dx3), cy3 = center[1] + (dy3);                                                   \
    image_t *p_img0 = &(*ref_img)[cy0 & 3][cx0 & 3];                                                        \
    image_t *p_img1 = &(*ref_img)[cy1 & 3][cx1 & 3];                                                        \
    image_t *p_img2 = &(*ref_img)[cy2 & 3][cx2 & 3];                                                        \
    image_t *p_img3 = &(*ref_img)[cy3 & 3][cx3 & 3];                                                        \
    int x0 = get_safe_mv(input->img_width, width, pic_pix_x + (cx0 >> 2));                                  \
    int x1 = get_safe_mv(input->img_width, width, pic_pix_x + (cx1 >> 2));                                  \
    int x2 = get_safe_mv(input->img_width, width, pic_pix_x + (cx2 >> 2));                                  \
    int x3 = get_safe_mv(input->img_width, width, pic_pix_x + (cx3 >> 2));                                  \
    int y0 = get_safe_mv(input->img_height, height, pic_pix_y + (cy0 >> 2));                                \
    int y1 = get_safe_mv(input->img_height, height, pic_pix_y + (cy1 >> 2));                                \
    int y2 = get_safe_mv(input->img_height, height, pic_pix_y + (cy2 >> 2));                                \
    int y3 = get_safe_mv(input->img_height, height, pic_pix_y + (cy3 >> 2));                                \
    int i_src = p_img0->i_stride[0];                                                                        \
    pel_t *src0 = p_img0->plane[0] + y0 * i_src + x0;                                                       \
    pel_t *src1 = p_img1->plane[0] + y1 * i_src + x1;                                                       \
    pel_t *src2 = p_img2->plane[0] + y2 * i_src + x2;                                                       \
    pel_t *src3 = p_img3->plane[0] + y3 * i_src + x3;                                                       \
    g_funs_handle.cost_sad_x4[width >> 2](org, CACHE_STRIDE, src0, src1, src2, src3, i_src, sad, height, 0); \
    sad[0] += MV_COST_SUBPEL(cx0, cy0);                                                                     \
    sad[1] += MV_COST_SUBPEL(cx1, cy1);                                                                     \
    sad[2] += MV_COST_SUBPEL(cx2, cy2);                                                                     \
    sad[3] += MV_COST_SUBPEL(cx3, cy3);                                                                     \
    COPY3_IF_LT(min_mcost, sad[0], best_x, cx0, best_y, cy0);                                               \
    COPY3_IF_LT(min_mcost, sad[1], best_x, cx1, best_y, cy1);                                               \
    COPY3_IF_LT(min_mcost, sad[2], best_x, cx2, best_y, cy2);                                               \
    COPY3_IF_LT(min_mcost, sad[3], best_x, cx3, best_y, cy3);                                               \
}

#define CAL_SUBPEL_BID(cand_mv_x, cand_mv_y, mcost) {                                                               \
    int x0 = get_safe_mv(input->img_width,  width,  pic_pix_x + (cand_mv_x >> 2));                                  \
    int y0 = get_safe_mv(input->img_height, height, pic_pix_y + (cand_mv_y >> 2));                                  \
    image_t *p_img0 = &ref_img[cand_mv_y & 3][cand_mv_x & 3];                                                       \
    int i_src0 = p_img0->i_stride[0];                                                                               \
    pel_t *src0 = p_img0->plane[0] + y0 * i_src0 + x0;                                                              \
    mcost = g_funs_handle.cost_avg_sad[width >> 2](org, CACHE_STRIDE, src0, i_src0, src1, i_src1, height, 0);       \
    mcost += MV_COST_SUBPEL(cand_mv_x, cand_mv_y) + bw_mv_cost;                                                     \
}

#define CHECK_SUBPEL_BID(cand_mv_x, cand_mv_y, mcost) {                                                             \
    CAL_SUBPEL_BID(cand_mv_x, cand_mv_y, mcost);                                                                    \
    COPY3_IF_LT(min_mcost, mcost, best_x, cand_mv_x, best_y, cand_mv_y);                                            \
}

double quant2quarter(double x){
	double x_q = 0.0;
	if (x < -0.125){
		if (x >= -0.375) {
			x_q = -0.25;
        } else if (x >= -0.625) {
			x_q = -0.5;
        } else {
			x_q = -0.75;
        }
	} else if (x > 0.125){
		if (x <= 0.375) {
			x_q = 0.25;
        } else if (x <= 0.625) {
			x_q = 0.5;
        } else {
			x_q = 0.75;
        }
	} else {
		x_q = 0;
    }
	return x_q;
}

double quant2half(double x){
    double x_q = 0.0;
    if (x < -0.25){
        x_q = -0.5;
    }
    else if (x > 0.25){
        x_q = 0.5;
    }
    else {
        x_q = 0;
    }
    return x_q;
}


int SubPelBlockMotionSearch(avs3_enc_t *h, me_info_t *me, int ref, i16s_t *pmv, i16s_t *mv, i32u_t min_mcost)
{
    const cfg_param_t *input = h->input;
    i32u_t mcost;
    int pos;
    int cand_mv_x, cand_mv_y;
    int best_x, best_y;
    image_t(*ref_img)[4][4] = h->type == B_FRM ? &h->ref_list[ref + 1]->rec->img_1_4th : &h->ref_list[ref]->rec->img_1_4th;
    int width = h->pu_size;
    int height = h->pu_size;
    int pic_pix_x = h->pu_pix_x;
    int pic_pix_y = h->pu_pix_y;
    int   ctr_x = (pmv[0] >> 1) << 1;
    int   ctr_y = (pmv[1] >> 1) << 1;
    i32u_t *tab_mvbits_x = me->tab_mvbits - pmv[0];
    i32u_t *tab_mvbits_y = me->tab_mvbits - pmv[1];
    pel_t *org = h->p_org[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
    int center[2];
    int quar_pel_list[8][2];
    int    quar_pel_cnt = 0;

    center[0] = best_x = mv[0] << 2;
    center[1] = best_y = mv[1] << 2;

    CHECK_SUBPEL_X4(- 2, + 0, \
                    + 2, + 0, \
                    + 0, - 2, \
                    + 0, + 2);

    CHECK_SUBPEL_X4(- 2, - 2, \
                    + 2, - 2, \
                    - 2, + 2, \
                    + 2, + 2); 

    center[0] = best_x;
    center[1] = best_y;

    for (pos = 0; pos < 8; pos++) {
        cand_mv_x = center[0] + tab_square_pos[pos][0];    // quarter-pel units
        cand_mv_y = center[1] + tab_square_pos[pos][1];    // quarter-pel units

        quar_pel_list[quar_pel_cnt][0] = cand_mv_x - center[0];
        quar_pel_list[quar_pel_cnt][1] = cand_mv_y - center[1];
        quar_pel_cnt++;
    }

    pos = 0;
    if (quar_pel_cnt - pos > 3) {
        CHECK_SUBPEL_X4(quar_pel_list[pos    ][0], quar_pel_list[pos    ][1], \
                        quar_pel_list[pos + 1][0], quar_pel_list[pos + 1][1], \
                        quar_pel_list[pos + 2][0], quar_pel_list[pos + 2][1], \
                        quar_pel_list[pos + 3][0], quar_pel_list[pos + 3][1]);
        pos += 4;
        if (quar_pel_cnt - pos > 3) {
            CHECK_SUBPEL_X4(quar_pel_list[pos    ][0], quar_pel_list[pos    ][1], \
                            quar_pel_list[pos + 1][0], quar_pel_list[pos + 1][1], \
                            quar_pel_list[pos + 2][0], quar_pel_list[pos + 2][1], \
                            quar_pel_list[pos + 3][0], quar_pel_list[pos + 3][1]);
            pos += 4;
        }
    }
    for (; pos < quar_pel_cnt; pos++) {
        CHECK_SUBPEL(quar_pel_list[pos][0], quar_pel_list[pos][1], mcost);
    }

    mv[0] = (i16s_t)best_x;
    mv[1] = (i16s_t)best_y;
   
    return min_mcost;
}

int SubPelBlockMotionSearch_bid(avs3_enc_t *h, me_info_t *me, i16s_t *pmv_1st, i16s_t *pmv_2nd, i16s_t *mv_1st, i16s_t *mv_2nd)
{
    const cfg_param_t *input = h->input;
    i32u_t mcost;
    int pos;
    int cand_mv_x, cand_mv_y;
    int best_x, best_y;

    int pic_pix_x = h->pu_pix_x;
    int pic_pix_y = h->pu_pix_y;
    int width  = h->pu_size;
    int height = h->pu_size;

    image_t(*ref_img)[4] = h->ref_list[1]->rec->img_1_4th;
    image_t(*ref_img_bid)[4] = h->ref_list[0]->rec->img_1_4th;
    pel_t *org = h->p_org[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
    int x1 = get_safe_mv(input->img_width, width, pic_pix_x + (mv_2nd[0] >> 2));
    int y1 = get_safe_mv(input->img_height, height, pic_pix_y + (mv_2nd[1] >> 2));
    image_t *p_img1 = &ref_img_bid[mv_2nd[1] & 3][mv_2nd[0] & 3];
    int  i_src1 = p_img1->i_stride[0];
    pel_t *src1 = p_img1->plane[0] + y1 * i_src1 + x1;

    int   ctr_x = (pmv_1st[0] >> 1) << 1;
    int   ctr_y = (pmv_1st[1] >> 1) << 1;
    i32u_t *tab_mvbits_x = me->tab_mvbits - pmv_1st[0];
    i32u_t *tab_mvbits_y = me->tab_mvbits - pmv_1st[1];
    int bw_mv_cost = MV_COST(mv_2nd[0], mv_2nd[1], pmv_2nd[0], pmv_2nd[1]);
    int center[2];
    i32u_t min_mcost = MAX_COST;

    center[0] = best_x = mv_1st[0] << 2;
    center[1] = best_y = mv_1st[1] << 2;

    CHECK_SUBPEL_BID(center[0], center[1], mcost);

    if (!me->out_of_range) {
        for (pos = 8; pos < 16; pos++) {
            cand_mv_x = center[0] + tab_square_pos[pos][0];    // quarter-pel units
            cand_mv_y = center[1] + tab_square_pos[pos][1];    // quarter-pel units
            CHECK_SUBPEL_BID(cand_mv_x, cand_mv_y, mcost);
        }

        center[0] = best_x;
        center[1] = best_y;

        for (pos = 0; pos < 8; pos++) {
            cand_mv_x = center[0] + tab_square_pos[pos][0];    // quarter-pel units
            cand_mv_y = center[1] + tab_square_pos[pos][1];    // quarter-pel units

            CHECK_SUBPEL_BID(cand_mv_x, cand_mv_y, mcost);
        }
    }
    mv_1st[0] = (i16s_t)best_x;
    mv_1st[1] = (i16s_t)best_y;

    return min_mcost;
}

int FastSubPelBlockMotionSearch(avs3_enc_t *h, me_info_t *me, int ref, i16s_t *pmv, i16s_t *mv, i32u_t min_mcost)
{
    const cfg_param_t *input = h->input;

    i32u_t   mcost;
    int   cand_mv_x, cand_mv_y;
    image_t(*ref_img)[4][4] = h->type == B_FRM ? &h->ref_list[ref + 1]->rec->img_1_4th : &h->ref_list[ref]->rec->img_1_4th;
    int best_x, best_y;
    int width     = h->pu_size;
    int height    = h->pu_size;
    int pic_pix_x = h->pu_pix_x;
    int pic_pix_y = h->pu_pix_y;
    pel_t *org = h->p_org[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
    int   m, i;
    int   pred_frac_mv_x, pred_frac_mv_y;
    int   ctr_x = (pmv[0] >> 1) << 1;
    int   ctr_y = (pmv[1] >> 1) << 1;
    uchar_t SearchState[7][7];
    uchar_t(*p_search_state)[7] = (uchar_t(*)[7])(&SearchState[3][3]);
    i32u_t *tab_mvbits_x = me->tab_mvbits - pmv[0];
    i32u_t *tab_mvbits_y = me->tab_mvbits - pmv[1];
    int center[2];

    center[0] = best_x = mv[0] << 2;
    center[1] = best_y = mv[1] << 2;

    pred_frac_mv_x = (pmv[0] - center[0]) % 4;
    pred_frac_mv_y = (pmv[1] - center[1]) % 4;

    memset(SearchState[0], 0, 7 * 7 * sizeof(uchar_t));

    p_search_state[0][0] = 1;

    if (pred_frac_mv_x || pred_frac_mv_y) {
        cand_mv_x = center[0] + pred_frac_mv_x;
        cand_mv_y = center[1] + pred_frac_mv_y;

        CHECK_SUBPEL(pred_frac_mv_x, pred_frac_mv_y, mcost);
        p_search_state[pred_frac_mv_y][pred_frac_mv_x] = 1;
    }

    for (i = 0; i < 7; i++) {
        int iXMinNow = best_x;
        int iYMinNow = best_y;

        for (m = 0; m < 4; m++) {
            int dx, dy;

            cand_mv_x = iXMinNow + Diamond_x[m];
            cand_mv_y = iYMinNow + Diamond_y[m];

            dx = cand_mv_x - center[0];
            dy = cand_mv_y - center[1];

            if (abs(dx) <= 3 && abs(dy) <= 3) {
                if (!p_search_state[dy][dx]) {
                    CHECK_SUBPEL(dx, dy, mcost);
                    p_search_state[dy][dx] = 1;
                }
            }
        }
        if (iXMinNow == best_x && iYMinNow == best_y) {
            break;
        }
    }

    mv[0] = (i16s_t)best_x;
    mv[1] = (i16s_t)best_y;

    //===== return minimum motion cost =====
    return min_mcost;
}

int FastSubPelBlockMotionSearch_bid(avs3_enc_t *h, me_info_t *me, i16s_t *pmv_1st, i16s_t *pmv_2nd, i16s_t *mv_1st, i16s_t *mv_2nd)
{
    i32u_t   mcost;
    int   cand_mv_x, cand_mv_y;
    const cfg_param_t *input = h->input;
    int pic_pix_x = h->pu_pix_x;
    int pic_pix_y = h->pu_pix_y;
    int width  = h->pu_size;
    int height = h->pu_size;

    image_t(*ref_img)[4] = h->ref_list[1]->rec->img_1_4th;
    image_t(*ref_img_bid)[4] = h->ref_list[0]->rec->img_1_4th;
    pel_t *org = h->p_org[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
    int x1 = get_safe_mv(input->img_width , width,  pic_pix_x + (mv_2nd[0] >> 2));
    int y1 = get_safe_mv(input->img_height, height, pic_pix_y + (mv_2nd[1] >> 2));
    image_t *p_img1 = &ref_img_bid[mv_2nd[1] & 3][mv_2nd[0] & 3];
    int  i_src1 = p_img1->i_stride[0];
    pel_t *src1 = p_img1->plane[0] + y1 * i_src1 + x1;

    int   i;
    int   m, best_x, best_y;
    int   pred_frac_mv_x, pred_frac_mv_y;
    int   ctr_x = (pmv_1st[0] >> 1) << 1;
    int   ctr_y = (pmv_1st[1] >> 1) << 1;
    i32u_t   *tab_mvbits_x = me->tab_mvbits - pmv_1st[0];
    i32u_t   *tab_mvbits_y = me->tab_mvbits - pmv_1st[1];
    int bw_mv_cost = MV_COST(mv_2nd[0], mv_2nd[1], pmv_2nd[0], pmv_2nd[1]);
    uchar_t SearchState[7][7];
    uchar_t(*p_search_state)[7] = (uchar_t(*)[7])(&SearchState[3][3]);
    i32u_t min_mcost = MAX_COST;

    mv_1st[0] <<= 2;
    mv_1st[1] <<= 2;

    pred_frac_mv_x = (pmv_1st[0] - mv_1st[0]) % 4;
    pred_frac_mv_y = (pmv_1st[1] - mv_1st[1]) % 4;

    memset(SearchState[0], 0, 7 * 7 * sizeof(uchar_t));

    p_search_state[0][0] = 1;

    best_x = mv_1st[0];
    best_y = mv_1st[1];

    CHECK_SUBPEL_BID(mv_1st[0], mv_1st[1], mcost);

    if (!me->out_of_range) {
        if (pred_frac_mv_x != 0 || pred_frac_mv_y != 0) {
            cand_mv_x = mv_1st[0] + pred_frac_mv_x;
            cand_mv_y = mv_1st[1] + pred_frac_mv_y;
     
            CHECK_SUBPEL_BID(cand_mv_x, cand_mv_y, mcost);
            p_search_state[cand_mv_y - mv_1st[1]][cand_mv_x - mv_1st[0]] = 1;
        }

        for (i = 0; i < 7; i++) {
            int iXMinNow = best_x;
            int iYMinNow = best_y;

            for (m = 0; m < 4; m++) {
                cand_mv_x = iXMinNow + Diamond_x[m];
                cand_mv_y = iYMinNow + Diamond_y[m];
         
                if (abs(cand_mv_x - mv_1st[0]) <= 3 && abs(cand_mv_y - mv_1st[1]) <= 3) {
                    if (!p_search_state[cand_mv_y - mv_1st[1]][cand_mv_x - mv_1st[0]]) {
                        CHECK_SUBPEL_BID(cand_mv_x, cand_mv_y, mcost);
                        p_search_state[cand_mv_y - mv_1st[1]][cand_mv_x - mv_1st[0]] = 1;
                    }
                }
            }
            if (iXMinNow == best_x && iYMinNow == best_y) {
                break;
            }
        }
    }
    mv_1st[0] = (i16s_t)best_x;
    mv_1st[1] = (i16s_t)best_y;

    return min_mcost;
}

int SubPelBlockMotionSearch_esti(avs3_enc_t *h, me_info_t *me, int ref, i16s_t *pmv, i16s_t *mv, i32u_t min_mcost, i32u_t *d_mcost, i32u_t *s_mcost){
	const cfg_param_t *input = h->input;
	int best_x, best_y;
	image_t(*ref_img)[4][4] = h->type == B_FRM ? &h->ref_list[ref + 1]->rec->img_1_4th : &h->ref_list[ref]->rec->img_1_4th;
	int width = h->pu_size;
	int height = h->pu_size;
	int pic_pix_x = h->pu_pix_x;
	int pic_pix_y = h->pu_pix_y;
	i32u_t *tab_mvbits_x = me->tab_mvbits - pmv[0];
	i32u_t *tab_mvbits_y = me->tab_mvbits - pmv[1];
	pel_t *org = h->p_org[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
	int   ctr_x = (pmv[0] >> 1) << 1;
	int   ctr_y = (pmv[1] >> 1) << 1;
	int center[2];

	//
	double x1, y1, x2, y2, x2_t, y2_t, x3, y3;
	double x1_q, y1_q, x2_q, y2_q, x3_q, y3_q;
	i32u_t mcost1 = MAX_COST, mcost2 = MAX_COST, mcost3 = MAX_COST;
	i32u_t m_mcost = min_mcost;
    int dx1, dy1, dx2, dy2, dx3, dy3, cx1, cy1, cx2, cy2, cx3, cy3;
    int I,  J,  K,  L,  A1, B1, C1, D1;
    int I2, J2, K2, L2, A2, B2, C2, D2;

	center[0] = best_x = mv[0] << 2;
	center[1] = best_y = mv[1] << 2;

	/*---estimate first 1/4 point---*/

	//calculate
	I = d_mcost[2] - m_mcost;
	J = d_mcost[0] - m_mcost;
	K = d_mcost[1] - m_mcost;
	L = d_mcost[3] - m_mcost;

	A1 = (I + J) >> 1;
	B1 = (I - J) >> 1;
	C1 = (K + L) >> 1;
	D1 = (K - L) >> 1;

	if (A1 == 0) {
		x1 = 0;
    } else {
		x1 = (double)-B1 / (2 * A1);
    }
	if (C1 == 0) {
		y1 = 0;
    } else {
		y1 = (double)-D1 / (2 * C1);
    }

	//quant
	x1_q = quant2quarter(x1);
	y1_q = quant2quarter(y1);
	if (x1_q == 0.0 && y1_q == 0.0)
		goto esti_end;		//early terminal

	//calculate mcost
	dx1 = (int)(x1_q * 4);
	dy1 = (int)(y1_q * 4);
	cx1 = center[0] + dx1;
	cy1 = center[1] + dy1;

	CHECK_SUBPEL(dx1, dy1, mcost1);

	/*---estimate second 1/4 point---*/

	//calculate
	I2 = s_mcost[1] - m_mcost;
	J2 = s_mcost[3] - m_mcost;
	K2 = s_mcost[0] - m_mcost;
	L2 = s_mcost[2] - m_mcost;

	A2 = (I2 + J2) >> 1;
	B2 = (I2 - J2) >> 1;
	C2 = (K2 + L2) >> 1;
	D2 = (K2 - L2) >> 1;
	//E2 = min_mcost

	if (A2 == 0) {
		x2_t = 0;
    } else {
		x2_t = (double)-B2 / (2 * A2);
    }
	if (C2 == 0) {
		y2_t = 0;
    } else {
		y2_t = (double)-D2 / (2 * C2);
    }
	//transform
	x2 = x2_t - y2_t;
	y2 = x2_t + y2_t;

	//quant
	x2_q = quant2quarter(x2);
	y2_q = quant2quarter(y2);
	if (x1_q == x2_q && y1_q == y2_q)
		goto esti_end;		//early terminal

	//calculate mcost
	dx2 = (int)(x2_q * 4);
	dy2 = (int)(y2_q * 4);
	cx2 = center[0] + dx2;
	cy2 = center[1] + dy2;

	CHECK_SUBPEL(dx2, dy2, mcost2);

	/*---estimate third 1/4 point---*/

	x3 = (x1 + x2) / 2;
	y3 = (y1 + y2) / 2;

	x3_q = quant2quarter(x3);
	y3_q = quant2quarter(y3);

	dx3 = (int)(x3_q * 4);
	dy3 = (int)(y3_q * 4);
	cx3 = center[0] + dx3;
	cy3 = center[1] + dy3;

    CHECK_SUBPEL(dx3, dy3, mcost3);

esti_end:

	mv[0] = (i16s_t)best_x;
	mv[1] = (i16s_t)best_y;

	return min_mcost;
}

int SubPelBlockMotionSearch_bid_esti(avs3_enc_t *h, me_info_t *me, i16s_t *pmv_1st, i16s_t *pmv_2nd, i16s_t *mv_1st, i16s_t *mv_2nd, i32u_t min_mcost)
{
	const cfg_param_t *input = h->input;
	i32u_t mcost;
	int best_x, best_y;
	int pic_pix_x = h->pu_pix_x;
	int pic_pix_y = h->pu_pix_y;
	int width = h->pu_size;
	int height = h->pu_size;

	image_t(*ref_img)[4] = h->ref_list[1]->rec->img_1_4th;
	image_t(*ref_img_bid)[4] = h->ref_list[0]->rec->img_1_4th;
	pel_t *org = h->p_org[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
	int x1 = get_safe_mv(input->img_width, width, pic_pix_x + (mv_2nd[0] >> 2));
	int y1 = get_safe_mv(input->img_height, height, pic_pix_y + (mv_2nd[1] >> 2));
	image_t *p_img1 = &ref_img_bid[mv_2nd[1] & 3][mv_2nd[0] & 3];
	int  i_src1 = p_img1->i_stride[0];
	pel_t *src1 = p_img1->plane[0] + y1 * i_src1 + x1;

	i32u_t *tab_mvbits_x = me->tab_mvbits - pmv_1st[0];
	i32u_t *tab_mvbits_y = me->tab_mvbits - pmv_1st[1];
	int   ctr_x = (pmv_1st[0] >> 1) << 1;
	int   ctr_y = (pmv_1st[1] >> 1) << 1;
	int bw_mv_cost = MV_COST(mv_2nd[0], mv_2nd[1], pmv_2nd[0], pmv_2nd[1]);
	int center[2];

	//
	double xx1, yy1, x2, y2, x2_t, y2_t, x3, y3;
	double x1_q, y1_q, x2_q, y2_q, x3_q, y3_q;
	i32u_t mcost1 = MAX_COST, mcost2 = MAX_COST, mcost3 = MAX_COST;
	i32u_t c_mcost;
	int dx1, dy1, dx2, dy2, dx3, dy3, cx1, cy1, cx2, cy2, cx3, cy3;
    int I,  J,  K,  L,  A1, B1, C1, D1;
    int I2, J2, K2, L2, A2, B2, C2, D2;

	center[0] = best_x = mv_1st[0] << 2;
	center[1] = best_y = mv_1st[1] << 2;

	c_mcost = min_mcost;
	min_mcost = MAX_COST;
	CHECK_SUBPEL_BID(center[0], center[1], mcost);

	/*---estimate first 1/4 point---*/

	//calculate
	I = me->d_mcost[0][2] - c_mcost;
	J = me->d_mcost[0][0] - c_mcost;
	K = me->d_mcost[0][1] - c_mcost;
	L = me->d_mcost[0][3] - c_mcost;

	A1 = (I + J) >> 1;
	B1 = (I - J) >> 1;
	C1 = (K + L) >> 1;
	D1 = (K - L) >> 1;

	if (A1 == 0) {
		xx1 = 0;
    } else {
		xx1 = (double)-B1 / (2 * A1);
    }

	if (C1 == 0) {
		yy1 = 0;
    } else {
		yy1 = (double)-D1 / (2 * C1);
    }

	//quant
	x1_q = quant2quarter(xx1);
	y1_q = quant2quarter(yy1);
	if (x1_q == 0.0 && y1_q == 0.0)
		goto bid_end;		//early terminal

	//calculate mcost
	dx1 = (int)(x1_q * 4);
	dy1 = (int)(y1_q * 4);
	cx1 = center[0] + dx1;
	cy1 = center[1] + dy1;

	CHECK_SUBPEL_BID(cx1, cy1, mcost1);

	/*---estimate second 1/4 point---*/

	//calculate
	I2 = me->s_mcost[0][1] - c_mcost;
	J2 = me->s_mcost[0][3] - c_mcost;
	K2 = me->s_mcost[0][0] - c_mcost;
	L2 = me->s_mcost[0][2] - c_mcost;

	A2 = (I2 + J2) >> 1;
	B2 = (I2 - J2) >> 1;
	C2 = (K2 + L2) >> 1;
	D2 = (K2 - L2) >> 1;
	//E2 = min_mcost

	if (A2 == 0) {
		x2_t = 0;
    } else {
		x2_t = (double)-B2 / (2 * A2);
    }

	if (C2 == 0) {
		y2_t = 0;
    } else {
		y2_t = (double)-D2 / (2 * C2);
    }

	x2 = x2_t - y2_t;
	y2 = x2_t + y2_t;

	//quant
	x2_q = quant2quarter(x2);
	y2_q = quant2quarter(y2);
	if (x1_q == x2_q && y1_q == y2_q)
		goto bid_end;		//early terminal

	//calculate mcost
	dx2 = (int)(x2_q * 4);
	dy2 = (int)(y2_q * 4);
	cx2 = center[0] + dx2;
	cy2 = center[1] + dy2;

	CHECK_SUBPEL_BID(cx2, cy2, mcost2);

	/*---estimate third 1/4 point---*/
	x3 = (xx1 + x2) / 2;
	y3 = (yy1 + y2) / 2;

	x3_q = quant2quarter(x3);
	y3_q = quant2quarter(y3);

	//calculate mcost3
	dx3 = (int)(x3_q * 4);
	dy3 = (int)(y3_q * 4);
	cx3 = center[0] + dx3;
	cy3 = center[1] + dy3;

	CHECK_SUBPEL_BID(cx3, cy3, mcost3);

bid_end:

	mv_1st[0] = (i16s_t)best_x;
	mv_1st[1] = (i16s_t)best_y;

	return min_mcost;
}


/* ----------------------------------------------------------------------------
*  ME Control
*----------------------------------------------------------------------------*/

void get_me_region(const cfg_param_t *input, me_info_t *me, i16s_t *pmv, int pu_size, int pu_pix_x, int pu_pix_y)
{
    int search_range = MAX_SEARCH_RANGE;
    me->min_mv_x = (i16s_t)(get_safe_mv(input->img_width , pu_size, pu_pix_x + ((pmv[0] + 2) >> 2) - search_range) - pu_pix_x);
    me->max_mv_x = (i16s_t)(get_safe_mv(input->img_width , pu_size, pu_pix_x + ((pmv[0] + 2) >> 2) + search_range) - pu_pix_x);
    me->min_mv_y = (i16s_t)(get_safe_mv(input->img_height, pu_size, pu_pix_y + ((pmv[1] + 2) >> 2) - search_range) - pu_pix_y);
    me->max_mv_y = (i16s_t)(get_safe_mv(input->img_height, pu_size, pu_pix_y + ((pmv[1] + 2) >> 2) + search_range) - pu_pix_y);

    me->out_of_range = (me->min_mv_x == me->max_mv_x || me->min_mv_y == me->max_mv_y);
}

int add_mv_candidate(i16s_t(*mvc)[2], int i_mvc, i16s_t x, i16s_t y)
{
    int i;

    x = (x + 2) >> 2;
    y = (y + 2) >> 2;

    for (i = 0; i < i_mvc; i++) {
        if (mvc[i][0] == x && mvc[i][1] == y) {
            break;
        }
    }
    if (i == i_mvc) {
        mvc[i][0] = x;
        mvc[i][1] = y;
        return i_mvc + 1;
    } else {
        return i_mvc;
    }
}

int pu_motion_search(avs3_enc_t *h, me_info_t *me, int ref, int bit_size)
{
    const cfg_param_t *input = h->input;
    int i, j;
    int min_mcost = MAX_COST;
    frame_t *ref_frm = h->type == B_FRM ? h->ref_list[ref + 1] : h->ref_list[ref];
    image_t *ref_img = &ref_frm->rec->img_1_4th[0][0];
    int refframe = (ref == -1 ? 0 : ref);
    i16s_t *pred_mv;
    i16s_t(*dstMv)[2][2][2][2] = (ref < 0 ? me->allBwMv : me->allFwMv);
    int m;
    int b8_x = h->pu_pix_x == h->cu_pix_x ? 0 : 1;
    int b8_y = h->pu_pix_y == h->cu_pix_y ? 0 : 1;
    i16s_t mv[2];
    int neighbor_ref[3];
    ALIGNED_32(i16s_t neighbor_mv[3][2]);

    int offset = h->pu_b8_y * input->b8_stride + h->pu_b8_x;
    frame_t *f_ref = h->ref_list[1];
    frame_t *b_ref = h->ref_list[0];
    i16s_t	fmv[2] = { 0, 0 };
    i16s_t	bmv[2] = { 0, 0 };

    int TRp, TRd;
    int skip_lines = SAD_SKIP_LINE(h->pu_size);

    me->i_org = CACHE_STRIDE;
    me->p_org = h->p_org[0] + (h->pu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->pu_pix_x - h->lcu_pix_x;
    me->i_ref = ref_img->i_stride[0];
    me->p_ref = ref_img->plane[0] + h->pu_pix_y * me->i_ref + h->pu_pix_x;
    me->blk_width = h->pu_size;
    me->blk_height = h->pu_size;
    me->blk_x = h->pu_pix_x;
    me->blk_y = h->pu_pix_y;
    me->img_width = input->img_width;
    me->img_height = input->img_height;
    me->i_mvc = 0;
    pred_mv = ((h->type != B_FRM || ref >= 0) ? me->predFwMv : me->predBwMv)[1][b8_y][b8_x][refframe];

    get_predicted_mv(h, pred_mv, refframe, ref, neighbor_ref, neighbor_mv);
    get_me_region(input, me, pred_mv, h->pu_size, h->pu_pix_x, h->pu_pix_y);

    me->i_mvc = add_mv_candidate(me->mvc, me->i_mvc, pred_mv[0], pred_mv[1]);
    me->i_mvc = add_mv_candidate(me->mvc, me->i_mvc, 0, 0);

    for (i = 0; i < 3; i++) {
        if (neighbor_ref[i] != -1) {
            me->i_mvc = add_mv_candidate(me->mvc, me->i_mvc, neighbor_mv[i][0], neighbor_mv[i][1]);
        }
    }

    if (refframe > 0) {
        int mult_distance = h->ref_dist[refframe];
        int devide_distance = h->ref_dist[refframe - 1];
        i16s_t scale_mv[2];
        scaling_mv(mult_distance, devide_distance, dstMv[1][b8_y][b8_x][refframe - 1], scale_mv);
        me->i_mvc = add_mv_candidate(me->mvc, me->i_mvc, scale_mv[0], scale_mv[1]);
    }
    if (h->type == B_FRM) {
        if (ref == 0) {
            int f_ref_ref = f_ref->refbuf[offset];
            i16s_t  *f_ref_mv = f_ref->mvbuf[offset];

            me->i_mvc = add_mv_candidate(me->mvc, me->i_mvc, -me->allBwMv[1][b8_y][b8_x][0][0], -me->allBwMv[1][b8_y][b8_x][0][1]);

            if (f_ref_ref != -1){
                TRp = (f_ref->poc - f_ref->ref_poc[f_ref_ref] + 256) % 256;
                TRd = (int)(h->poc - f_ref->poc);
                scaling_mv(TRd, TRp, f_ref_mv, fmv);
                me->i_mvc = add_mv_candidate(me->mvc, me->i_mvc, fmv[0], fmv[1]);
            }
        } else {
            int b_ref_ref = b_ref->refbuf[offset];
            i16s_t  *b_ref_mv = b_ref->mvbuf[offset];
            i64s_t frame_no_next_P = b_ref->poc;
            i64s_t frame_no_B = h->poc;
            if (b_ref_ref != -1){
                TRp = (b_ref->poc - b_ref->ref_poc[b_ref_ref] + 256) % 256;
                TRd = (int)(frame_no_next_P - frame_no_B);
                scaling_mv(TRd, TRp, b_ref_mv, bmv);
                me->i_mvc = add_mv_candidate(me->mvc, me->i_mvc, bmv[0], bmv[1]);
            }
        }
    }

    wait_ref_available(h, (h->type == B_FRM ? h->ref_list[ref + 1] : h->ref_list[ref]), h->pu_pix_y + me->max_mv_y + h->pu_size + 2);

    if (me_pre_search(me, pred_mv, mv, &min_mcost, me->d_mcost[refframe], skip_lines)) {
        switch(h->analyzer.me_method) {
        case IME_TZ:
            min_mcost = me_integer_tz(me, pred_mv, mv, me->d_mcost[refframe], me->s_mcost[refframe], min_mcost, skip_lines, h->curr_RPS.layer);
            break;
        case IME_HEX:
            min_mcost = me_integer_hex(me, pred_mv, mv, me->d_mcost[refframe], me->s_mcost[refframe], min_mcost, skip_lines);
            break;
        default:
            assert(0);
            break;
        }
    }

    CP32(me->integer_fw_mv[refframe], mv);

    me->integer_mcost[refframe] = min_mcost;

    m = (h->pu_size == 1 << bit_size) ? 2 : 1;

    if (me->out_of_range) {
        mv[0] <<= 2;
        mv[1] <<= 2;
    } else {
		if (me->b_fast_fme) {
            min_mcost = SubPelBlockMotionSearch_esti(h, me, ref, pred_mv, mv, min_mcost, me->d_mcost[refframe], me->s_mcost[refframe]);
        } else {
			min_mcost = SubPelBlockMotionSearch(h, me, ref, pred_mv, mv, min_mcost);
		}
    }

    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            CP32(dstMv[1][b8_y + j][b8_x + i][refframe], mv);
        }
    }

    return min_mcost;
}

int pu_motion_search_b(avs3_enc_t *h, me_info_t *me, int mode, int bit_size, int *mcost_bid, int min_mcost)
{
    const cfg_param_t *input = h->input;
    int min_mcost_bid = MAX_COST;

    int mb_pix_x = h->pu_pix_x - h->cu_pix_x;
    int mb_pix_y = h->pu_pix_y - h->cu_pix_y;

    int b8_x = mb_pix_x == 0 ? 0 : 1;
    int b8_y = mb_pix_y == 0 ? 0 : 1;

    i16s_t *allBwMv  = me->allBwMv [mode][b8_y][b8_x][0];
    i16s_t *allBidMv = me->allBidMv[mode][b8_y][b8_x][0];

    i16s_t *pmv_1st = me->predFwMv[mode][b8_y][b8_x][0];
    i16s_t *pmv_2nd = me->predBwMv[mode][b8_y][b8_x][0];

    CP32(allBidMv, me->integer_fw_mv[0]);

    if (me->b_fast_fme) {
        min_mcost_bid = SubPelBlockMotionSearch_bid_esti(h, me, pmv_1st, pmv_2nd, allBidMv, allBwMv, me->integer_mcost[0]);
    } else {
		min_mcost_bid = SubPelBlockMotionSearch_bid(h, me, pmv_1st, pmv_2nd, allBidMv, allBwMv);
	}
    *mcost_bid = min_mcost_bid;

    return min_mcost_bid;
}

/* ----------------------------------------------------------------------------
*  ME Module
*----------------------------------------------------------------------------*/

void me_module_create_mvcost(me_info_t *me, int motion_factor)
{
    int i;
    int bits, imax, imin;
    int max_ref_bits = me->max_ref_bits;
    int max_mv_bits = me->max_mv_bits;
    i32u_t val;

    if (me->curr_lambda_mf != motion_factor) {
        int mf = motion_factor;
        me->curr_lambda_mf = motion_factor;

        me->tab_mvbits[0] = mf;

        for (bits = 3; bits <= max_mv_bits; bits += 2) {
            imax = 1 << (bits >> 1);
            imin = imax >> 1;

            val = (bits * mf);

            for (i = imin; i < imax; i++) {
                me->tab_mvbits[-i] = me->tab_mvbits[i] = val;
            }
        }

        me->tab_refbits[0] = mf;

        for (bits = 3; bits <= max_ref_bits; bits += 2) {
            imax = (1 << ((bits >> 1) + 1)) - 1;
            imin = imax >> 1;

            val = (bits * mf);

            for (i = imin; i < imax; i++) {
                me->tab_refbits[i] = val;
            }
        }
    }
}

int me_module_init(me_info_t *me)
{
    int number_of_reference_frames = REF_MAXBUFFER;
    int max_ref_bits = 1 + 2 * (int)floor(log(max(16, number_of_reference_frames + 1)) / log(2) + 1e-10);
    int max_ref = (1 << ((max_ref_bits >> 1) + 1)) - 1;
    int number_of_subpel_positions = 4 * (2 * MAX_SEARCH_RANGE + 3);
    int max_mv_bits = 9 + 2 * (int)ceil(log(number_of_subpel_positions + 1) / log(2) + 1e-10);

    me->max_ref_bits = max_ref_bits;
    me->max_mv_bits = max_mv_bits;
    me->max_mvd = (1 << ((max_mv_bits >> 1))) - 1;
    me->curr_lambda_mf = -1;

    me->tab_mvbits = (i32u_t *)com_malloc((2 * me->max_mvd + 1) * sizeof(i32u_t), 1);
    me->tab_refbits = (i32u_t *)com_malloc(max_ref * sizeof(i32u_t), 1);
    me->tab_mvbits += me->max_mvd;

    return (2 * me->max_mvd + 1) * sizeof(int)+max_ref * sizeof(int);
}

void avs3_me_free(me_info_t *me)
{
    //--- correct array offset ---
    me->tab_mvbits -= me->max_mvd;

    //--- delete arrays ---
    if (me->tab_mvbits) {
        com_free(me->tab_mvbits);
    }
    if (me->tab_refbits) {
        com_free(me->tab_refbits);
    }
}
