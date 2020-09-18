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

static void get_ipred_neighbor(pel *dst, int x, int y, int w, int h, int pic_width, int pic_height, pel *src, int s_src, int bit_depth)
{
    pel *srcT  = src - s_src;
    pel *srcTL = srcT - 1;
    pel *srcL  = src - 1;
    pel *left  = dst - 1;
    pel *up    = dst + 1;
    int default_val = 1 << (bit_depth - 1);
    int pad_range = COM_MAX(w, h) * 4 + 4;

    if (y) {
        int size = COM_MIN(2 * w, pic_width - x);
        com_mcpy(up, srcT, size * sizeof(pel));
        up += size, srcT += size;
        com_mset_pel(up, up[-1], pad_range - size);
    } else {
        com_mset_pel(up, default_val, pad_range);
    }
    if (x) {
        pel *src = srcL;
        int size = COM_MIN(2 * h, pic_height - y);

        for (int i = 0; i < size; ++i) {
            *left-- = *src;
            src += s_src;
        }
        int pads = pad_range - size;
        com_mset_pel(left - pads + 1, left[1], pad_range - size);
    } else {
        com_mset_pel(left - pad_range + 1, default_val, pad_range);
    }
    if (x && y) {
        dst[0] = srcTL[0];
    } else if (y) {
        dst[0] = srcTL[1];
    } else if (x) {
        dst[0] = srcL[0];
    } else {
        dst[0] = default_val;
    }
}

double loka_estimate_coding_cost(inter_search_t *pi, com_img_t *img_org, com_img_t **ref_l0, com_img_t **ref_l1, int num_ref[2], 
                                 int bit_depth, double *icost, double icost_uv[2], float* map_dqp)
{
    const int    base_qp = 32;
    const double base_lambda = 1.43631 * pow(2.0, (base_qp - 16.0) / 4.0);
    const int is_bi = (num_ref[0] && num_ref[1]);

    int pic_width  = img_org->width [0];
    int pic_height = img_org->height[0];

    double total_cost = 0;
    double total_icost = 0;
    double total_icost_u = 0;
    double total_icost_v = 0;
    double total_var = 0;

    int i_org   = STRIDE_IMGB2PIC(img_org->stride[0]);
    int i_org_c = STRIDE_IMGB2PIC(img_org->stride[1]);
    com_pic_t pic = { 0 };
    com_subpel_t subpel;

    pic.subpel = &subpel;

    pi->ptr       = img_org->ptr;
    pi->curr_mvr  = 2;
    pi->lambda_mv = (u32)floor(65536.0 * sqrt(base_lambda));
    pi->i_org     = i_org;
    pi->adaptive_raster_range   = 0;
    pi->subpel_cost_type        = 0;

#define WRITE_REC_PIC 0

#if WRITE_REC_PIC
    static pel *buf = NULL;
    if (buf == NULL) buf = malloc(pic_width * pic_height);
    static FILE *fp = NULL;
    if (fp == NULL) fp = fopen("preprocess.yuv", "wb");
    static FILE *fp_org = NULL;
    if (fp_org == NULL) fp_org = fopen("preprocess_org.yuv", "wb");
#endif

    for (int y = 0; y < pic_height - UNIT_SIZE + 1; y += UNIT_SIZE) {
        float *var = map_dqp + (y / UNIT_SIZE) * (pic_width / UNIT_SIZE);

        for (int x = 0; x < pic_width - UNIT_SIZE + 1; x += UNIT_SIZE) {
            ALIGNED_32(pel pred_buf[MAX_CU_DIM]);
            ALIGNED_32(pel pred_buf_fwd[MAX_CU_DIM]);
            ALIGNED_32(pel pred_buf_bwd[MAX_CU_DIM]);

            u64 min_cost = COM_UINT64_MAX;
            pel *org = (pel*)img_org->planes[0] + y * i_org + x;

            pi->org = org;

            for (int lidx = 0; lidx < 2; lidx++) { // uni-prediction (L0 or L1)
                pi->num_refp = num_ref[lidx];

                for (int refi = 0; refi < num_ref[lidx]; refi++) {
                    com_img_t *ref_img = (lidx ? ref_l1 : ref_l0)[refi];
                    pic.ptr = ref_img->ptr;
                    pic.img = ref_img;
                    pic.stride_luma = STRIDE_IMGB2PIC(ref_img->stride[0]);
                    pic.y = ref_img->planes[0];
                    pic.subpel->imgs[0][0] = ref_img;

                    com_pic_t *ref_pic = pi->ref_pic = &pic;
                    s16 mv[2], mvp[2] = { 0 };

                    me_search_tz(pi, x, y, UNIT_SIZE, UNIT_SIZE, pic_width, pic_height, refi, lidx, mvp, mv, 0);
                    com_mc_blk_luma(ref_pic, pred_buf, UNIT_SIZE, (x << 2) + mv[MV_X], (y << 2) + mv[MV_Y], UNIT_SIZE, UNIT_SIZE, UNIT_WIDX, pi->max_coord[MV_X], pi->max_coord[MV_Y], (1 << bit_depth) - 1, 0);

                    u32 cost = com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);
                    if (cost < min_cost) {
#if WRITE_REC_PIC 
                        uavs3e_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                        min_cost = cost;

                        if (is_bi) {
                            pel *d = lidx ? pred_buf_bwd : pred_buf_fwd;
                            uavs3e_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, d, UNIT_SIZE, UNIT_SIZE, UNIT_SIZE);
                        }
                    }
                }
            }

            if (is_bi) {
                uavs3e_funs_handle.pel_avrg[UNIT_WIDX](pred_buf, UNIT_SIZE, pred_buf_fwd, pred_buf_bwd, UNIT_SIZE);
                u32 cost = com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);
                if (cost < min_cost) {
#if WRITE_REC_PIC 
                    uavs3e_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                    min_cost = cost;
                }

                pic.ptr = ref_l0[0]->ptr;
                pic.img = ref_l0[0];
                pic.stride_luma = STRIDE_IMGB2PIC(ref_l0[0]->stride[0]);
                pic.y = ref_l0[0]->planes[0];
                pic.subpel->imgs[0][0] = ref_l0[0];

                com_mc_blk_luma(&pic, pred_buf_fwd, UNIT_SIZE, (x << 2), (y << 2), UNIT_SIZE, UNIT_SIZE, UNIT_WIDX, pi->max_coord[MV_X], pi->max_coord[MV_Y], (1 << bit_depth) - 1, 0);
               
                pic.ptr = ref_l1[0]->ptr;
                pic.img = ref_l1[0];
                pic.stride_luma = STRIDE_IMGB2PIC(ref_l1[0]->stride[0]);
                pic.y = ref_l1[0]->planes[0];
                pic.subpel->imgs[0][0] = ref_l1[0];
                
                com_mc_blk_luma(&pic, pred_buf_bwd, UNIT_SIZE, (x << 2), (y << 2), UNIT_SIZE, UNIT_SIZE, UNIT_WIDX, pi->max_coord[MV_X], pi->max_coord[MV_Y], (1 << bit_depth) - 1, 0);

                uavs3e_funs_handle.pel_avrg[UNIT_WIDX](pred_buf, UNIT_SIZE, pred_buf_fwd, pred_buf_bwd, UNIT_SIZE);
                
                cost = com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);
                if (cost < min_cost) {
#if WRITE_REC_PIC 
                    uavs3e_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                    min_cost = cost;
                }

            }

            pel nb_buf[INTRA_NEIB_SIZE];
            get_ipred_neighbor(nb_buf + INTRA_NEIB_MID, x, y, UNIT_SIZE, UNIT_SIZE, pic_width, pic_height, org, i_org, bit_depth);

            int avaliable_nb = (x ? AVAIL_LE : 0) | (y ? AVAIL_UP : 0) | ((x && y) ? AVAIL_UP_LE : 0);
            static tab_s8 ipm_tab[] = { 0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32 };
            u32 min_icost = COM_UINT64_MAX;
            int best_mode;

            for (int i = 0; i < sizeof(ipm_tab); i++) {
                com_intra_pred(nb_buf + INTRA_NEIB_MID, pred_buf, ipm_tab[i], UNIT_SIZE, UNIT_SIZE, bit_depth, avaliable_nb, 0);
                u32 cost = com_had(UNIT_SIZE, UNIT_SIZE, org, i_org, pred_buf, UNIT_SIZE, bit_depth);

                if (cost < min_cost) {
#if WRITE_REC_PIC 
                    uavs3e_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                    min_cost = cost;
                }
                if (cost < min_icost) {
                    min_icost = cost;
                    best_mode = ipm_tab[i];
                }
            }
            if (icost_uv) {
                int xc = x >> 1;
                int yc = y >> 1;
                pel *orgu = (pel*)img_org->planes[1] + yc * i_org_c + xc;
                pel *orgv = (pel*)img_org->planes[2] + yc * i_org_c + xc;

                get_ipred_neighbor(nb_buf + INTRA_NEIB_MID, xc, yc, UNITC_SIZE, UNITC_SIZE, pic_width / 2, pic_height / 2, orgu, i_org_c, bit_depth);
                com_intra_pred(nb_buf + INTRA_NEIB_MID, pred_buf, best_mode, UNITC_SIZE, UNITC_SIZE, bit_depth, avaliable_nb, 0);
                total_icost_u += com_had(UNITC_SIZE, UNITC_SIZE, orgu, i_org_c, pred_buf, UNITC_SIZE, bit_depth);

                get_ipred_neighbor(nb_buf + INTRA_NEIB_MID, xc, yc, UNITC_SIZE, UNITC_SIZE, pic_width / 2, pic_height / 2, orgv, i_org_c, bit_depth);
                com_intra_pred(nb_buf + INTRA_NEIB_MID, pred_buf, best_mode, UNITC_SIZE, UNITC_SIZE, bit_depth, avaliable_nb, 0);
                total_icost_v += com_had(UNITC_SIZE, UNITC_SIZE, orgv, i_org_c, pred_buf, UNITC_SIZE, bit_depth);
            }

            total_cost  += min_cost;
            total_icost += min_icost;

            if (map_dqp) {
                u64 energy = 0;
                int xc = x >> 1;
                int yc = y >> 1;
                pel *orgu = (pel*)img_org->planes[1] + yc * i_org_c + xc;
                pel *orgv = (pel*)img_org->planes[2] + yc * i_org_c + xc;

                energy += uavs3e_funs_handle.cost_var[UNIT_WIDX](org, i_org);
                energy += uavs3e_funs_handle.cost_var[UNITC_WIDX](orgu, i_org_c);
                energy += uavs3e_funs_handle.cost_var[UNITC_WIDX](orgv, i_org_c);
                energy >>= (bit_depth - 8) * 2;

                var[x / UNIT_SIZE] = (float)(0.8 * log2((double)COM_MAX(energy, 1)));
                total_var += var[x / UNIT_SIZE];
            }
        }        
    }

#if WRITE_REC_PIC
    fwrite(buf, 1, pic_width * pic_height, fp);
    fwrite(img_org->planes[0], 1, pic_width * pic_height, fp_org);
#endif

#undef WRITE_REC_PIC

    int blk_num = (pic_width / UNIT_SIZE) * (pic_height / UNIT_SIZE);

    if (icost) {
        *icost = total_icost / blk_num / UNIT_SIZE / UNIT_SIZE;
    }
    if (icost_uv) {
        icost_uv[0] = total_icost_u / blk_num / UNITC_SIZE / UNITC_SIZE;
        icost_uv[1] = total_icost_v / blk_num / UNITC_SIZE / UNITC_SIZE;
    }

    if (map_dqp) {
        float avg_var = (float)(total_var / blk_num);
        for (int i = 0; i < blk_num; i++) {
            map_dqp[i] -= avg_var;
        }
    }
    return total_cost / blk_num / UNIT_SIZE / UNIT_SIZE;
}
int check_scenecut_histogram_next_gop(enc_ctrl_t *h, int *last_histo_data, int cur_scenecut_idx, int max_gop_idx, int bit_depth)
{
    int count_unlike = 0;
    int thred_unlike = (int)(0.8 * (max_gop_idx - 1));
    for (int i = cur_scenecut_idx + 1; i < cur_scenecut_idx + max_gop_idx; i++) {
        if (!check_scenecut_histogram(&h->pinter, &h->img_rlist[i], last_histo_data, bit_depth)) {
            h->img_rlist[cur_scenecut_idx].is_scenecut = -1;
            return 0;
        }
        if (check_scenecut_histogram(&h->pinter, &h->img_rlist[i], h->img_rlist[cur_scenecut_idx].histo_data, bit_depth)) {
            count_unlike++;
            if (count_unlike > thred_unlike) {
                h->img_rlist[cur_scenecut_idx].is_scenecut = -1;
                return 0;
            }
        }
    }
    for (int i = 0; i < cur_scenecut_idx; i++)
    {
        if (!check_scenecut_histogram(&h->pinter, &h->img_rlist[cur_scenecut_idx], h->img_rlist[i].histo_data, bit_depth))
        {
            return 0;
        }
    }


    return 1;
}
int check_scenecut_histogram(inter_search_t *pi, analyze_node_t *img_org, int *last_histo_data, int bit_depth)
{
    int histo_block[HISBLOCK_NUM] = { 0 };
    int blocknumber_width = img_org->img->width[0] / HISBLOCK_SIZE;
    int blocknumber_height = img_org->img->height[0] / HISBLOCK_SIZE;
    int thread1num = 0;
    int thread2num = 0;
    int bitdepth = pi->bit_depth;
    int max_num_partitions = bitdepth == 8 ? 256 : 1024;
    for (int pixel_row = 0; pixel_row < HISBLOCK_SIZE; pixel_row++)
    {
        for (int pixel_col = 0; pixel_col < HISBLOCK_SIZE; pixel_col++)
        {
            for (int i = 0; i < max_num_partitions; i++)
            {
                histo_block[pixel_row * HISBLOCK_SIZE + pixel_col] += abs(img_org->histo_data[(pixel_row * HISBLOCK_SIZE + pixel_col)* MAX_NUM_PARTITIONS + i] - last_histo_data[(pixel_row * HISBLOCK_SIZE + pixel_col) * MAX_NUM_PARTITIONS + i]);
            }
        }
    }
    for (int i = 0; i < HISBLOCK_NUM; i++)
    {
        thread1num += (histo_block[i] > (blocknumber_width * blocknumber_height * 0.7));
        thread2num += (histo_block[i] > (blocknumber_width * blocknumber_height * 1.4));
    }

    if ((thread1num >= (HISBLOCK_NUM * 0.7) && thread2num >= (HISBLOCK_NUM * 0.2) || thread1num >= (HISBLOCK_NUM * 0.875) || thread2num >= (HISBLOCK_NUM * 0.375)))
    {
        return 1;
    }

    return 0;
}
void calculate_histogram(inter_search_t *pi, com_img_t *img_org, int *histo_data, int bit_depth)
{
    int width_block = img_org->width[0] / HISBLOCK_SIZE;
    int height_block = img_org->height[0] / HISBLOCK_SIZE;
    int stride = img_org->width[0];
    //memset(img_org->histo_data, 0, HISBLOCK_NUM*MAX_NUM_PARTITIONS);

    for (int histo_row = 0; histo_row < HISBLOCK_SIZE; histo_row++)
    {
        for (int histo_col = 0; histo_col < HISBLOCK_SIZE; histo_col++)
        {
            for (int block_row = 0; block_row < height_block; block_row++)
            {
                for (int block_col = 0; block_col < width_block; block_col++)
                {
                    int offset = histo_row * height_block * stride + histo_col * width_block + block_row * stride + block_col;
                    pel *org = (pel*)img_org->planes[0] + offset;
                    histo_data[(histo_row * HISBLOCK_SIZE + histo_col) * MAX_NUM_PARTITIONS + org[0]] ++;
                }
            }
        }
    }
}
double loka_get_sc_ratio(inter_search_t *pi, com_img_t *img_org, com_img_t *img_last, int bit_depth)
{
    int num_refp[2] = { 1, 0 };
    com_img_t *ref_l0[1] = { img_last };

    double icost;
    double pcost = loka_estimate_coding_cost(pi, img_org, ref_l0, NULL, num_refp, bit_depth, &icost, NULL, NULL);
    return pcost / icost;
}

static double loka_get_ref_cost(inter_search_t *pi, com_img_t *img_org, com_img_t *ref0, com_img_t *ref1, int bit_depth)
{
    int num_refp[2];
    com_img_t *ref_l0[1] = { ref0 };
    com_img_t *ref_l1[1] = { ref1 };
    num_refp[0] = (ref0 == NULL ? 0 : 1);
    num_refp[1] = (ref1 == NULL ? 0 : 1);

    double pcost = loka_estimate_coding_cost(pi, img_org, ref_l0, ref_l1, num_refp, bit_depth, NULL, NULL, NULL);
    return pcost;
}

static void update_last_ip(enc_ctrl_t *h, analyze_node_t *img_t, int type)
{
    com_img_release(h->img_lastIP);
    h->img_lastIP = img_t->img;
    com_img_addref(img_t->img);


    if (type == SLICE_I) {
        h->lastI_ptr = img_t->img->ptr;
    }
    if (h->cfg.scenecut_histogram) {
        h->lastIP_sc_ratio = img_t->sc_ratio;
        memcpy(h->lastIP_histo_data, img_t->histo_data, sizeof(h->lastIP_histo_data));
    }

}

static void push_sub_gop(enc_ctrl_t *h, int start, int num, int level)
{
    if (num <= 2) {
        if (start < h->img_rsize) {
            add_input_node(h, h->img_rlist[start].img, 0, level, SLICE_B);

            if (num == 2 && start + 1 < h->img_rsize) {
                add_input_node(h, h->img_rlist[start + 1].img, 0, level, SLICE_B);
            }
        }
    } else {
        int idx = start + num / 2;

        if (idx < h->img_rsize) {
            add_input_node(h, h->img_rlist[idx].img, 1, level, SLICE_B);
        }
        push_sub_gop(h, start, num / 2, level + 1);
        push_sub_gop(h, idx + 1,  num - num / 2 - 1, level + 1);
    }
}

void loka_slicetype_decision(enc_ctrl_t *h)
{
#define UPDATE_LAST_IP(h,img) { com_img_release(h->img_lastIP);  h->img_lastIP = img; com_img_addref(img); }

    int bit_depth = h->cfg.bit_depth_internal;
    com_assert(!(h->cfg.scenecut_histogram && h->cfg.scenecut));
    int next_ifrm_idx = h->cfg.i_period ? h->cfg.i_period - (int)(h->img_rlist[0].img->ptr - h->lastI_ptr) : h->img_rsize;
    int cur_ip_idx = COM_MIN(h->cfg.max_b_frames, h->img_rsize - 1);
    double sc_threshold = 1.0 - h->cfg.scenecut / 100.0;
    if (!h->cfg.scenecut_histogram)
    {
        cur_ip_idx = COM_MIN(cur_ip_idx, next_ifrm_idx);
    }
    for (int i = 0; i < cur_ip_idx; i++) {
        if (h->img_rlist[i].insert_idr) { // insert user-defined IDR frame
            if (i > 0) {
                add_input_node(h, h->img_rlist[i - 1].img, 1, FRM_DEPTH_1, SLICE_B);
                if (i > 1) {
                    push_sub_gop(h, 0, i - 1, FRM_DEPTH_2);
                }
            }
            add_input_node(h, h->img_rlist[i].img, 1, FRM_DEPTH_0, SLICE_I);
            update_last_ip(h, &h->img_rlist[i], SLICE_I);
            shift_reorder_list(h, i);
            return;
        }
    }
    int end_of_seq = 0;
    if (h->img_rsize < h->cfg.max_b_frames) {
        end_of_seq = 1;
    }
    int max_ip_idx = cur_ip_idx;
    int avg_histo_sum = 0;
    double avg_scratio = 0;
    if (h->cfg.scenecut_histogram && !end_of_seq) {
        int noscenecut = 0;
        int fluctuate = 0;
        for (int i = 0; i <= cur_ip_idx; i++) {
            int *last_histo_data = i ? h->img_rlist[i - 1].histo_data : h->lastIP_histo_data;
            if (!check_scenecut_histogram(&h->pinter, &h->img_rlist[i], h->lastIP_histo_data, bit_depth)) {
                for (int j = i; j >= 0; j--) {
                    h->img_rlist[j].is_scenecut = 0;
                    noscenecut = 0;
                }
            }
            else if (check_scenecut_histogram(&h->pinter, &h->img_rlist[i], last_histo_data, bit_depth) && h->img_rlist[i].is_scenecut >= 0) {// -1 means already been tested in last gop and confirmed not a scenecut frame
                h->img_rlist[i].is_scenecut = 1;
                noscenecut = 1;
            }
            avg_scratio += h->img_rlist[i].sc_ratio;
        }
        avg_histo_sum /= cur_ip_idx + 1;
        avg_scratio /= cur_ip_idx + 1;
        if (noscenecut) {
            int flash = 1;
            for (int i = cur_ip_idx; i >= 0; i--) {
                int *last_histo_data = i ? h->img_rlist[i - 1].histo_data : h->lastIP_histo_data;
                double cur_scratio = h->img_rlist[i].sc_ratio;
                double prev_scratio = i ? h->img_rlist[i - 1].sc_ratio : h->lastIP_sc_ratio;
                if (prev_scratio) {
                    if (h->img_rlist[i].sc_ratio > 0.95 || (fabs(cur_scratio - prev_scratio) > 0.1 * prev_scratio || fabs(cur_scratio - avg_scratio) > 0.1 * avg_scratio))// || abs(cur_histo_sum - prev_histo_sum) > 0.06 * prev_histo_sum || abs(cur_histo_sum - avg_histo_sum) > 0.06 *avg_histo_sum)
                    {
                        fluctuate = 1;

                        if (h->img_rlist[i].is_scenecut && !h->SceneTransition) {
                            int max_gop_idx = i + h->cfg.max_b_frames > h->img_rsize - 1 ? h->img_rsize - 1 - i : h->cfg.max_b_frames;
                            if (i == cur_ip_idx) {
                                flash = 0;
                                break;
                            }
                            else if (check_scenecut_histogram_next_gop(h, last_histo_data, i, max_gop_idx, bit_depth)) {
                                for (int j = 0; j < i; j++) {
                                    h->img_rlist[j].is_scenecut = 0;
                                }
                                cur_ip_idx = i - 1;
                                break;
                            }
                        }
                    }
                }
                h->img_rlist[i].is_scenecut = 0;
            }
            if (!flash) {
                for (int i = 0; i <= cur_ip_idx; i++) {
                    int *last_histo_data = i ? h->img_rlist[i - 1].histo_data : h->lastIP_histo_data;
                    double cur_scratio = h->img_rlist[i].sc_ratio;
                    double prev_scratio = i ? h->img_rlist[i - 1].sc_ratio : h->lastIP_sc_ratio;
                    if (prev_scratio) {
                        if (h->img_rlist[i].sc_ratio > 0.95 || (fabs(cur_scratio - prev_scratio) > 0.1 * prev_scratio || fabs(cur_scratio - avg_scratio) > 0.1 * avg_scratio))
                        {
                            fluctuate = 1;
                            int max_gop_idx = i + h->cfg.max_b_frames > h->img_rsize - 1 ? h->img_rsize - 1 - i : h->cfg.max_b_frames;
                            if (h->img_rlist[i].is_scenecut && !h->SceneTransition && check_scenecut_histogram_next_gop(h, last_histo_data, i, max_gop_idx, bit_depth)) {
                                for (int j = i + 1; j <= cur_ip_idx; j++) {
                                    h->img_rlist[j].is_scenecut = 0;
                                }
                                cur_ip_idx = i - 1;
                                break;
                            }
                        }
                    }
                    h->img_rlist[i].is_scenecut = 0;
                }
            }
        }
        if (!fluctuate && !noscenecut)
            h->SceneTransition = 0;
    }
    else if (h->cfg.scenecut) {
        if (h->img_rlist[0].sc_ratio > sc_threshold) {
            while (cur_ip_idx) {
                double sc_ratio = loka_get_sc_ratio(&h->pinter, h->img_rlist[cur_ip_idx].img, h->img_lastIP, bit_depth);
                if (sc_ratio <= sc_threshold) {
                    for (int i = 0; i <= cur_ip_idx; i++) {
                        h->img_rlist[i].sc_ratio = 0;
                    }
                    break;
                }
                cur_ip_idx--;
            }
            if (cur_ip_idx == 0) {
                add_input_node(h, h->img_rlist[cur_ip_idx].img, 1, FRM_DEPTH_0, SLICE_I);
                update_last_ip(h, &h->img_rlist[cur_ip_idx], SLICE_I);
                shift_reorder_list(h, cur_ip_idx);
                return;
            }
        }
    }

    if (h->cfg.scenecut_histogram) {
        if (cur_ip_idx == -1)
            cur_ip_idx++;
        if (cur_ip_idx > next_ifrm_idx) {
            int tmp = 0;
            for (int i = 0; i < next_ifrm_idx; i++) {
                if (h->img_rlist[i].is_scenecut) {
                    tmp = 1;
                }
            }
            if (tmp == 0) {
                cur_ip_idx = next_ifrm_idx;
            }
        }
    }

    int is_ifrm = (next_ifrm_idx == cur_ip_idx);

    if (!is_ifrm && h->cfg.adaptive_gop) {
        while (cur_ip_idx) {
            com_img_t *img_cur = h->img_rlist[cur_ip_idx].img;
            com_img_t *img_mid = h->img_rlist[cur_ip_idx / 2].img;
            double cost_long = loka_get_ref_cost(&h->pinter, img_cur, h->img_lastIP, NULL, bit_depth) +
                loka_get_ref_cost(&h->pinter, img_mid, h->img_lastIP, img_cur, bit_depth);
            double cost_short = loka_get_ref_cost(&h->pinter, img_cur, img_mid, NULL, bit_depth) +
                loka_get_ref_cost(&h->pinter, img_mid, h->img_lastIP, NULL, bit_depth);

            if (cost_long < cost_short) {
                break;
            }
            cur_ip_idx /= 2;
        }
    }

    if (!h->cfg.scenecut_histogram &&h->cfg.scenecut) {
        if (is_ifrm) {
            for (int i = 0; i < cur_ip_idx; i++) {
                if (h->img_rlist[i].sc_ratio > sc_threshold) {
                    while (cur_ip_idx > i) {
                        if (h->img_rlist[cur_ip_idx].sc_ratio > sc_threshold) {
                            break;
                        }
                        cur_ip_idx--;
                    }
                    break;
                }
            }
        }
        else {
            for (int i = 0; i <= cur_ip_idx; i++) {
                if (h->img_rlist[i].sc_ratio > sc_threshold) {
                    com_img_t *img_last = i ? h->img_rlist[i - 1].img : h->img_lastIP;
                    while (i <= cur_ip_idx) {
                        double sc_ratio = loka_get_sc_ratio(&h->pinter, h->img_rlist[cur_ip_idx].img, img_last, bit_depth);
                        if (sc_ratio <= sc_threshold) {
                            break;
                        }
                        cur_ip_idx--;
                    }
                    break;
                }
            }
        }
    }
    com_assert(cur_ip_idx >= 0);
    if (h->cfg.scenecut_histogram)
    {
        is_ifrm = (next_ifrm_idx == cur_ip_idx) || (h->img_rlist[0].is_scenecut == 1);
        if (h->img_rlist[0].is_scenecut == 1 && next_ifrm_idx != cur_ip_idx) {
            h->SceneTransition = 1;
        }
    }

    if (is_ifrm) { // insert I frame
        if (h->cfg.close_gop) {
            if (cur_ip_idx > 0) {
                add_input_node(h, h->img_rlist[cur_ip_idx - 1].img, 1, FRM_DEPTH_1, SLICE_B);
                if (cur_ip_idx > 1) {
                    push_sub_gop(h, 0, cur_ip_idx - 1, FRM_DEPTH_2);
                }
            }
            add_input_node(h, h->img_rlist[cur_ip_idx].img, 1, FRM_DEPTH_0, SLICE_I);
            update_last_ip(h, &h->img_rlist[cur_ip_idx], SLICE_I);
        }
        else {
            add_input_node(h, h->img_rlist[cur_ip_idx].img, 1, FRM_DEPTH_0, SLICE_I);
            update_last_ip(h, &h->img_rlist[cur_ip_idx], SLICE_I);

            if (cur_ip_idx > 0) {
                push_sub_gop(h, 0, cur_ip_idx, FRM_DEPTH_2);
            }
        }
    }
    else {
        if (cur_ip_idx == h->img_rsize - 1 && h->img_rsize <= h->cfg.max_b_frames) { // flush
            push_sub_gop(h, 0, h->cfg.max_b_frames, FRM_DEPTH_2);
        }
        else {
            add_input_node(h, h->img_rlist[cur_ip_idx].img, 1, FRM_DEPTH_1, SLICE_B);
            update_last_ip(h, &h->img_rlist[cur_ip_idx], SLICE_B);

            if (cur_ip_idx > 0) {
                push_sub_gop(h, 0, cur_ip_idx, FRM_DEPTH_2);
            }
        }
        if (h->cfg.scenecut_histogram)
        {
            if (cur_ip_idx + 1 <= max_ip_idx && h->SceneTransition) {
                for (int i = cur_ip_idx + 1; i < max_ip_idx; i++) {
                    if (h->img_rlist[i].is_scenecut == 1)
                        h->SceneTransition = 0;
                }
            }
            if (!check_scenecut_histogram(&h->pinter, &h->img_rlist[max_ip_idx], h->lastIP_histo_data, bit_depth)) {
                h->SceneTransition = 0;
            }
        }
    }

    shift_reorder_list(h, cur_ip_idx);
}
