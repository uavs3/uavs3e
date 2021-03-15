#include "preprocess.h"
#include "../common.h"

#define PRE_BITSIZE 4
#define PRE_BLKSIZE (1 << PRE_BITSIZE)

double pre_cal_I_cost(const cfg_param_t *input, pel_t *p_org, int i_org, int width, int height, int bit_depth, double* cpp_list)
{
    int i, j;
    double cost = 0;
    const int shift = bit_depth - 8;
    const int offset = (shift > 0) ? (1 << (shift - 1)) : 0;
    int pixels = width * height;
    double lcu_row_intra_cost = 0;

    width >>= 3;
    height >>= 3;

    for (i = 0; i < height; i++) {
        int next_lcu_y;

        for (j = 0; j < width; j++) {
            double c = (g_funs_handle.cost_satd_i(p_org + (j << 3), i_org) + offset) >> shift;
            cost += c;
            lcu_row_intra_cost += c;
        }
        p_org += i_org << 3;

        next_lcu_y = (i + 1) << 3;

        if (next_lcu_y % LCU_SIZE == 0) {
            cpp_list[next_lcu_y / LCU_SIZE - 1] = max(0.001, lcu_row_intra_cost / input->img_width / LCU_SIZE);
            lcu_row_intra_cost = 0;
        }
    }

    if (height > 4) {
        cpp_list[input->pic_height_in_lcu - 1] = cpp_list[input->pic_height_in_lcu - 2];
    }
    return cost / pixels;
}

void* pre_cal_PB_one_lcu_row(pre_cal_PB_param_t* param)
{
    const cfg_param_t *input = param->input;
    int lcu_y = param->lcu_y;
    image_t *ref_img = param->ref_img;
    image_t *org_img = param->org_img;
    me_info_t* me = &param->me;
    double *cpp_list = param->cpp_list;
    int shift = param->shift;
    int offset = param->offset;
    i16s_t pred_mv[2] = { 0, 0 };
    i16s_t mv[2];
    i32u_t d_mcost[4], s_mcost[4];
    int i, j;
    int pix_x, pix_y;
    ALIGNED_64(pel_t org_buf[PRE_BLKSIZE * PRE_BLKSIZE]);
    double satd_cost_intra = 0;
    double satd_cost = 0;
    i32u_t min_mcost = MAX_COST;
    int width_in_lcu_row = input->img_width / PRE_BLKSIZE;
    int height_in_lcu_row = LCU_SIZE / PRE_BLKSIZE;
    int skip_lines = org_img->type == B_FRM ? 2 : 0;
    int scale = org_img->type == B_FRM ? 2 : 1;

    pix_y = LCU_SIZE * lcu_y;
    me->p_org = org_buf;

    height_in_lcu_row = height_in_lcu_row / scale;
    width_in_lcu_row = width_in_lcu_row / scale;

    for (i = 0; i < height_in_lcu_row; i++) {

        pix_x = 0;

        for (j = 0; j < width_in_lcu_row; j++) {
            double satd_inter, satd_intra;
            int m, n;

            g_funs_handle.cpy_pel[PRE_BLKSIZE >> 1](org_img->plane[0] + pix_y * org_img->i_stride[0] + pix_x, org_img->i_stride[0], org_buf, PRE_BLKSIZE, PRE_BLKSIZE, PRE_BLKSIZE);
            me->blk_x = pix_x;
            me->blk_y = pix_y;
            me->p_ref = ref_img->plane[0] + pix_y * me->i_ref + pix_x;

#define get_safe_mv_pre(img_size, mv) min((img_size) - PRE_BLKSIZE, max(0, (mv)))

            me->min_mv_x = (i16s_t)(get_safe_mv_pre(input->img_width, pix_x - MAX_SEARCH_RANGE) - pix_x);
            me->max_mv_x = (i16s_t)(get_safe_mv_pre(input->img_width, pix_x + MAX_SEARCH_RANGE) - pix_x);
            me->min_mv_y = (i16s_t)(get_safe_mv_pre(input->img_height, pix_y - MAX_SEARCH_RANGE) - pix_y);
            me->max_mv_y = (i16s_t)(get_safe_mv_pre(input->img_height, pix_y + MAX_SEARCH_RANGE) - pix_y);

            me->out_of_range = (me->min_mv_x == me->max_mv_x || me->min_mv_y == me->max_mv_y);

            /* neighbor block's MV */
            me->i_mvc = 0;

            if (me_pre_search(me, pred_mv, mv, &min_mcost, d_mcost, skip_lines)) {
                if (org_img->type == B_FRM) {
                    me_integer_hex(me, pred_mv, mv, d_mcost, s_mcost, min_mcost, skip_lines);
                } else {
                    me_integer_tz(me, pred_mv, mv, d_mcost, s_mcost, min_mcost, 0, skip_lines);
                }
            }

            satd_inter = (g_funs_handle.cost_blk_satd[PRE_BITSIZE - 2](org_buf, PRE_BLKSIZE, ref_img->plane[0] + (pix_y + mv[1]) * ref_img->i_stride[0] + pix_x + mv[0], ref_img->i_stride[0]) + offset) >> shift;
            satd_intra = 0;

            for (m = 0; m < PRE_BLKSIZE; m += 8) {
                for (n = 0; n < PRE_BLKSIZE; n += 8) {
                    satd_intra += (g_funs_handle.cost_satd_i(org_buf + m * PRE_BLKSIZE + n, PRE_BLKSIZE) + offset) >> shift;
                }
            }

            satd_cost += min(satd_inter, satd_intra) * scale * scale;
            satd_cost_intra += satd_intra * scale * scale;

            pix_x += PRE_BLKSIZE * scale;
        }

        pix_y += PRE_BLKSIZE * scale;
    }

    if (org_img->static_frm) {
        cpp_list[pix_y / LCU_SIZE - 1] = max(0.001, satd_cost_intra / input->img_width / LCU_SIZE);
    } else {
        cpp_list[pix_y / LCU_SIZE - 1] = max(0.001, satd_cost / input->img_width / LCU_SIZE);
    }
    param->inter_cost = satd_cost;
    param->intra_cost = satd_cost_intra;

    return NULL;
}

double pre_cal_PB_cost(const cfg_param_t *input, pre_cal_PB_param_t* param_list, image_t *ref_img, image_t *org_img, double lambda, me_info_t *me, double* cpp_list, double *intra_cost, avs3_threadpool_t *pool)
{
    int lcu_y;
    int bit_depth = input->bit_depth;
    const int shift = bit_depth - 8;
    const int offset = (shift > 0) ? (1 << (shift - 1)) : 0;
    int width_in_blk = input->img_width / PRE_BLKSIZE;
    int height_in_blk = input->img_height / PRE_BLKSIZE;
    double satd_cost = 0;
    double satd_intra_cost = 0;

    me->i_mvc = 0;
    me->img_width  = input->img_width;
    me->img_height = input->img_height;
    me->i_org = PRE_BLKSIZE;
    me->i_ref = ref_img->i_stride[0];
    me->blk_width  = PRE_BLKSIZE;
    me->blk_height = PRE_BLKSIZE;
    me->b_fast_fme = 0;

    me_module_create_mvcost(me, 0);

    for (lcu_y = 0; lcu_y < input->img_height / LCU_SIZE; lcu_y++) {
        pre_cal_PB_param_t *param = param_list + lcu_y;
        param->lcu_y = lcu_y;
        param->input = (cfg_param_t *)input;
        param->ref_img = ref_img;
        param->org_img = org_img;
        param->cpp_list = cpp_list;
        param->shift = shift;
        param->offset = offset;
        memcpy(&param->me, me, sizeof(me_info_t));
        avs3_threadpool_run(pool, pre_cal_PB_one_lcu_row, param, 1);
    }

    for (lcu_y = 0; lcu_y < input->img_height / LCU_SIZE; lcu_y++) {
        pre_cal_PB_param_t *param = param_list + lcu_y;
        avs3_threadpool_wait(pool, param);
        satd_cost += param->inter_cost;
        satd_intra_cost += param->intra_cost;
    }

    satd_cost /= input->img_width * (input->img_height / PRE_BLKSIZE * PRE_BLKSIZE);
    satd_intra_cost /= input->img_width * (input->img_height / PRE_BLKSIZE * PRE_BLKSIZE);

    if (height_in_blk > 2) {
        cpp_list[input->pic_height_in_lcu - 1] = cpp_list[input->pic_height_in_lcu - 2];
    }
    *intra_cost = satd_intra_cost;

    return max(satd_cost, 0.001);
}


double pre_cal_P_SAD(const cfg_param_t *input, image_t *ref_img, image_t *org_img)
{
    int i, j;
    int pix_x, pix_y;
    int width  = input->img_width;
    int height = input->img_height;
    int width_in_blk  = input->img_width  / 64;
    int height_in_blk = input->img_height / 64;
    double SAD = 0;

    pix_y = 0;

    for (i = 0; i < height_in_blk; i++) {
        pix_x = 0;

        for (j = 0; j < width_in_blk; j++) {
            SAD += g_funs_handle.cost_sad[64 / 4](
                org_img->plane[0] + pix_y * org_img->i_stride[0] + pix_x, org_img->i_stride[0],
                ref_img->plane[0] + pix_y * ref_img->i_stride[0] + pix_x, ref_img->i_stride[0], 64, 2);

            pix_x += 64;
        }
        pix_y += 64;
    }

    SAD /= input->img_width * input->img_height / 4;

    return max(SAD, 0.001);
}
