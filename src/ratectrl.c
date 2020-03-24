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

#include <math.h>
#include <stdio.h>
#include "define.h"

/* input/output qp is for bit_depth of 8 */

#define SHIFT_QP 11
#define QCompress 0.6

static __inline double uavs3e_qScale2qp(double qScale)
{
    return 16.0 + 8.0 * COM_LOG2(qScale / 1.43631);
}

static __inline double uavs3e_qp2qScale(double qp)
{
    return 1.43631 * pow(2.0, (qp - 16.0) / 8.0);
}

void rc_init(enc_rc_t* p, enc_cfg_t *param)
{
    memset(p, 0, sizeof(enc_rc_t));

    /* const data */
    p->type           = param->rc_type;
    p->rfConstant     = pow(15, 1 - QCompress) / uavs3e_qp2qScale(param->rc_crf);
    p->target_bitrate = param->rc_bitrate * 1000.0;
    p->max_bitrate    = param->rc_max_bitrate * 1000.0;
    p->frame_rate     = param->fps_num * 1.0 / param->fps_den;
    p->frame_pixels   = param->pic_width * param->pic_height;
    p->min_qp         = param->rc_min_qp;
    p->max_qp         = param->rc_max_qp;
    p->low_delay      = !param->max_b_frames;
    p->win_size       = (int)(p->frame_rate * 1.0 + 0.5);

    p->total_subgops  = p->low_delay ? -1 : -2;
    p->win_idx        = 0;
    p->win_bits       = 0;
    p->win_frames     = 0;
    p->win_bits_list  = com_malloc(sizeof(int) * p->win_size);

    uavs3e_pthread_mutex_init(&p->mutex, NULL);
}

void rc_destroy(enc_rc_t *p)
{
    if (p->win_bits_list) {
        com_mfree(p->win_bits_list);
    }
    uavs3e_pthread_mutex_destroy(&p->mutex);
}

void rc_update(enc_rc_t *p, com_pic_t *pic, char *ext_info, int info_buf_size)
{   
    int layer_id = pic->layer_id;

    if (layer_id <= FRM_DEPTH_1) {
        if (p->total_subgops >= 0) {
            p->total_factor += (p->subgop_bits / p->subgop_frms) * p->subgop_qscale / p->subgop_cplx;
        }
        p->total_subgops++;
        p->subgop_frms = 0;
        p->subgop_bits = 0;
        p->subgop_cplx = pic->picture_satd_blur;
        p->subgop_qscale = uavs3e_qp2qScale(pic->picture_qp_real);
    }
    p->subgop_frms++;
    p->subgop_bits += pic->picture_bits;

    p->total_frms++;
    p->total_bits += pic->picture_bits;

    p->win_bits  -= p->win_bits_list[p->win_idx];
    p->win_bits  += pic->picture_bits;

    p->win_bits_list[p->win_idx] = pic->picture_bits;
    p->win_idx = (p->win_idx + 1) % p->win_size;

    p->win_frames = COM_MIN(p->win_frames + 1, p->win_size);

    if (ext_info) {
        sprintf(ext_info, "layer:%d cost:%5.2f br:%9.2fkbps", pic->layer_id, pic->picture_satd, p->total_bits / (p->total_frms / p->frame_rate) / 1000);

        if (p->total_subgops > 0) {
            sprintf(ext_info, "%s C:%9.2f", ext_info, p->total_factor / p->total_subgops);
        }
        if (p->win_frames == p->win_size) {
            sprintf(ext_info, "%s brCur:%9.2fkbps", ext_info, p->win_bits * p->frame_rate / p->win_frames / 1000);
        }
    }
}

/*************************************************************************************************/
//
//  CRF:  baseCplx^(1-QCompress)/uavs3e_qp2qScale(crf) == currCplx^(1-QCompress)/uavs3e_qp2qScale(qp) 
//
//  ABR:  Bits * qScale / (complex^(1 - QCompress))= C, calculate factor C through historical data
//
/*************************************************************************************************/
int rc_get_qp(enc_rc_t *p,  com_pic_t *pic, int qp_l0, int qp_l1)
{
    double qp;
    double min_qp = p->min_qp;
    double max_qp = p->max_qp;
    int layer_id = pic->layer_id;
    long long ptr = pic->img->ptr;

    if (layer_id > FRM_DEPTH_1) {
        if (p->low_delay) {
            qp = enc_get_hgop_qp(qp_l0, layer_id, 1);
        } else {
            qp = enc_get_hgop_qp((qp_l0 + qp_l1 * 3) / 4, layer_id, 0);
        }
        return (int)(COM_CLIP3(min_qp, max_qp, (qp + 0.5)));
    }

    /*** frames in top layer ***/

    p->shortTermCplxSum *= 0.5;
    p->shortTermCplxCount *= 0.5;
    p->shortTermCplxSum += pic->picture_satd;
    p->shortTermCplxCount++;

    double blurredComplexity = pic->picture_satd_blur = pow(p->shortTermCplxSum / p->shortTermCplxCount, 1 - QCompress);

    if (p->max_bitrate != 0) {
        int sub_win = 16;

        if (p->win_frames == p->win_size) { // calculate min_qp
            int shift_out_bits = 0;
            int idx = p->win_idx;

            for (int i = 0; i < sub_win; i++) {
                shift_out_bits += p->win_bits_list[idx];
                idx = COM_MIN(idx + 1, p->win_size);
            }
            double max_bits = p->max_bitrate / p->frame_rate * p->win_size - p->win_bits + shift_out_bits;
            double frame_bits = COM_MAX(max_bits / sub_win, p->target_bitrate / p->frame_rate / 2);
            double qScale = blurredComplexity * (p->total_factor / p->total_subgops) / frame_bits;

            min_qp = uavs3e_qScale2qp(qScale) - (layer_id == 0 ? 3 : 0);
            //printf("%d  %f  %f  %f\n", sub_gop_frms, frame_bits, qScale, min_qp);
            min_qp = COM_MIN(min_qp, max_qp);
        }
    }

    if (p->type == RC_TYPE_CRF) {
        qp = uavs3e_qScale2qp(blurredComplexity / p->rfConstant);
    } else if(p->type == RC_TYPE_ABR) {
        if (p->total_factor == 0) {
            if (layer_id == FRM_DEPTH_0) { // first I frame, r-lambda model
                double cpp = pow(pic->picture_satd, 1.31);
                double bpp = p->target_bitrate / p->frame_rate / p->frame_pixels;
                double alpha = bpp < 0.025 ? 0.25 : 0.3;
                double ratio = 0.7 * alpha* pow(pow(cpp, 1.0 / 1.31) * 4.0 / bpp, 0.5582);
                double lambda = 6.7542 / 256 * pow(bpp * ratio / cpp, -1.7860);
                qp = (int)(5.661 * log(lambda) + 13.131 + 0.5);
            } else {
                qp = enc_get_hgop_qp(qp_l0, layer_id, p->low_delay);
            }
        } else {
            double timeDone = p->total_frms / p->frame_rate;
            double abrBuffer = p->target_bitrate * 2 * COM_MAX(sqrt(timeDone), 1);
            double err_bits = p->total_bits - p->target_bitrate * timeDone;
            double frame_bits = p->target_bitrate / p->frame_rate / (COM_CLIP3(0.5, 2.0, (abrBuffer + err_bits) / abrBuffer));

            double qScale = blurredComplexity * (p->total_factor / p->total_subgops) / frame_bits;

            qp = uavs3e_qScale2qp(qScale);
        }
    }
    if (layer_id == FRM_DEPTH_0) {
        qp -= 3;
    }
    return (int)(COM_CLIP3(min_qp, max_qp, (qp + 0.5)));
}

