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

typedef struct uavs3e_enc_rc_t
{
    /* Sequence level const data */
    double    target_bitrate;
    double    max_bitrate;
    double    frame_rate;
    int       frame_pixels;
    double    rfConstant;
    int       win_size;

    /* Sequence level global data */
    double    total_factor;
    long long total_subgops;
    long long total_bits;
    long long total_frms;

    /* Sequence level local data */
    long long top_last_ptr;
    double    top_fwd_qp;
    double    top_bwd_qp;

    double    shortTermCplxSum;
    double    shortTermCplxCount;
    
    int      *win_bits_list;
    int       win_bits;
    int       win_idx;
    int       win_frames;

    /* Sub-GOP level */
    int       subgop_frms;
    long long subgop_bits;
    double    subgop_cplx;   // we use top-level frame's cplx   as subgop_cplx
    double    subgop_qscale; // we use top-level frame's qscale as subgop_qscale

} enc_rc_t;

double uavs3e_qScale2qp(double qScale)
{
    return 16.0 + 8.0 * COM_LOG2(qScale / 1.43631);
}

double uavs3e_qp2qScale(double qp)
{
    return 1.43631 * pow(2.0, (qp - 16.0) / 8.0);
}

void* rc_init(enc_cfg_t *param)
{
    enc_rc_t *p = com_malloc(sizeof(enc_rc_t));

    if (param->rc_type == RC_TYPE_CRF) {
        double baseCplx = 2.8;
        p->rfConstant = pow(baseCplx, 1 - QCompress) / uavs3e_qp2qScale(param->rc_crf);
    }

    /* const data */
    p->target_bitrate = param->rc_bitrate * 1000.0;
    p->max_bitrate    = param->rc_max_bitrate * 1000.0;
    p->frame_rate     = param->fps_num * 1.0 / param->fps_den;
    p->frame_pixels   = param->pic_width * param->pic_height;
    p->win_size       = (int)(p->frame_rate * 1.0);

    p->win_idx = 0;
    p->win_bits = 0;
    p->win_frames = 0;
    p->win_bits_list = com_malloc(sizeof(int) * p->win_size);

    p->total_subgops = -2;

    return p;
}

void rc_update(void *rc_handle, com_pic_t *pic, char *ext_info, int info_buf_size)
{
    enc_rc_t *p = (enc_rc_t*)rc_handle;
   
    int layer_id = pic->layer_id;

    if (layer_id <= 1) {
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

    p->win_bits -= p->win_bits_list[p->win_idx];
    p->win_bits_list[p->win_idx] = pic->picture_bits;
    p->win_bits += p->win_bits_list[p->win_idx];
    p->win_idx  = (p->win_idx + 1) % p->win_size;
    p->win_frames = COM_MIN(p->win_frames + 1, p->win_size);

    if (ext_info) {
        sprintf(ext_info, "tid:%d cost:%5.2f br:%9.2fkbps", pic->layer_id, pic->picture_satd, p->total_bits / (p->total_frms / p->frame_rate) / 1000);

        if (p->total_subgops > 0) {
            sprintf(ext_info, "%s C:%9.2f", ext_info, p->total_factor / p->total_subgops);
        }
        if (p->win_frames == p->win_size) {
            sprintf(ext_info, "%s brCur:%9.2fkbps", ext_info, p->win_bits * p->frame_rate / p->win_frames / 1000);
        }
    }
}

void rc_destroy(void *rc_handle)
{
    com_mfree(rc_handle);
}

/*************************************************************************************************/
//
//  CRF:  baseCplx^(1-QCompress)/uavs3e_qp2qScale(crf) == currCplx^(1-QCompress)/uavs3e_qp2qScale(qp) 
//
//  ABR:  Bits * qScale / (complex^(1 - QCompress))= C, calculate factor C through historical data
//
/*************************************************************************************************/
int rc_get_qp(void *rc_handle, enc_ctrl_t *h, com_pic_t *pic)
{
    double qp;
    double min_qp = h->cfg.rc_min_qp;
    enc_rc_t *p = (enc_rc_t*)rc_handle;
    int layer_id = pic->layer_id;
    long long ptr = pic->img->ptr;

    if (layer_id <= 1) {
        p->shortTermCplxSum *= 0.5;
        p->shortTermCplxCount *= 0.5;
        p->shortTermCplxSum += pic->picture_satd;
        p->shortTermCplxCount++;

        double blurredComplexity = pic->picture_satd_blur = pow(p->shortTermCplxSum / p->shortTermCplxCount, 1 - QCompress);
        int sub_gop_frms = (int)(ptr - p->top_last_ptr);

        if (p->max_bitrate != 0 && p->win_frames >= p->win_size - sub_gop_frms) {
            // calculate max_qp
            int shift_out_bits = 0;
            int shift_out_frms = p->win_frames + sub_gop_frms - p->win_size;
            int idx = p->win_idx;

            for (int i = 0; i < shift_out_frms; i++) {
                shift_out_bits += p->win_bits_list[idx];
                idx = COM_MIN(idx + 1, p->win_size);
            }
            double max_bits = p->max_bitrate / p->frame_rate * p->win_size - p->win_bits + shift_out_bits;
            double frame_bits = COM_MAX(max_bits / sub_gop_frms, p->target_bitrate / p->frame_rate / 2);
            double qScale = blurredComplexity * (p->total_factor / p->total_subgops) / frame_bits;

            min_qp = uavs3e_qScale2qp(qScale) - (layer_id == 0 ? 3 : 0);
            //printf("%d  %f  %f  %f\n", sub_gop_frms, frame_bits, qScale, min_qp);
            min_qp = COM_MIN(min_qp, h->cfg.rc_max_qp);
        }

        if (h->cfg.rc_type == RC_TYPE_CRF) {
            qp = uavs3e_qScale2qp(blurredComplexity / p->rfConstant);
            p->top_fwd_qp = p->top_bwd_qp ? p->top_bwd_qp : qp;
        } 
        else if (h->cfg.rc_type == RC_TYPE_ABR) {
            if (p->total_factor == 0) {
                if (layer_id == 0) { // I frame
                    double cpp = pow(pic->picture_satd, 1.31);
                    double bpp = p->target_bitrate / p->frame_rate / p->frame_pixels;
                    double alpha = bpp < 0.025 ? 0.25 : 0.3;
                    double ratio = 0.7 * alpha* pow(pow(cpp, 1.0 / 1.31) * 4.0 / bpp, 0.5582);
                    bpp *= ratio;
                    double lambda = 6.7542 / 256 * pow(bpp / cpp, -1.7860);
                    qp = (int)(5.661 * log(lambda) + 13.131 + 0.5);
                    p->top_fwd_qp = qp;
                } else {
                    qp = enc_get_hgop_qp(p->top_fwd_qp, layer_id, h->info.sqh.low_delay);
                    // don't assign p->top_fwd_qp
                }
            } else {
                double timeDone   = p->total_frms / p->frame_rate;
                double abrBuffer  = p->target_bitrate * 2 * COM_MAX(sqrt(timeDone), 1);
                double err_bits   = p->total_bits - p->target_bitrate * timeDone;
                double frame_bits = p->target_bitrate / p->frame_rate / (COM_CLIP3(0.5, 2.0, (abrBuffer + err_bits) / abrBuffer));
                double qScale     = blurredComplexity * (p->total_factor / p->total_subgops) / frame_bits;

                qp = uavs3e_qScale2qp(qScale);

                if (layer_id == 0) {
                    qp -= 3;
                }
                p->top_fwd_qp = p->top_bwd_qp;
            }
        }
        qp = COM_MAX(qp, min_qp);
        p->top_bwd_qp = qp;
        p->top_last_ptr = ptr;
    } else {
        if (h->info.sqh.low_delay) {
            qp = enc_get_hgop_qp(p->top_bwd_qp, layer_id, 1);
        }else {
            qp = enc_get_hgop_qp((p->top_fwd_qp + p->top_bwd_qp * 3) / 4, layer_id, 0);
        }
    }
    return COM_CLIP3(h->cfg.rc_min_qp, h->cfg.rc_max_qp, (int)(qp + 0.5));
}

/******************************************* init RC ********************************************/
void avs3e_init_rc(enc_rc_handle_t* rc, enc_cfg_t *param)
{
    rc->type = param->rc_type;

    if (rc->handle) {
        rc->destroy(rc->handle);
        rc->handle = NULL;
    }
    if (rc->type != RC_TYPE_NULL) {
        if (rc->type == RC_TYPE_CBR) {
            //rc->init = rc_cbr_init;
            //rc->get_qp = rc_cbr_get_qp;
            //rc->update = rc_cbr_update;
            //rc->destroy = rc_cbr_destroy;
        }
        else if (rc->type == RC_TYPE_CRF || rc->type == RC_TYPE_ABR) {
            rc->init    = rc_init;
            rc->get_qp  = rc_get_qp;
            rc->update  = rc_update;
            rc->destroy = rc_destroy;
        }
        rc->handle = rc->init(param);
    }
}