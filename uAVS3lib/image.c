#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include "global.h"
#include "commonVariables.h"
#include "image.h"
#include "header.h"
#include "vlc.h"
#include "AEC.h"

static void try_to_release_rec(avs3_ctrl_t *ctrl, frame_t *frm)
{
    if (frm->b_ref == 0 && frm->ref_cnt == 0 && frm->rec) {
        xl_append(&ctrl->rec_list, frm->rec);
        xl_append(&ctrl->imglist_idle, frm->img_org);
        frm->rec = NULL;
        frm->img_org = NULL;
    }
}

void end_one_frame(avs3_ctrl_t *ctrl, avs3_enc_t *pic, int frm_time)
{
    bit_stream_t *strm;
    const cfg_param_t *input = pic->input;
    int i, header_bits, data_bits;
    char_t ext_info[1024] = "";
    image_t *img_org = pic->img_org;

    strm = &pic->hdr_stream;
    header_bits = strm->byte_pos << 3;
    assert(strm->bits_to_go == 8);
    if (ctrl->func_out_strm) {
        ctrl->func_out_strm(ctrl->priv_data, strm->streamBuffer, strm->byte_pos, img_org->pts, img_org->dts, PIC_HDR);
    }

    strm = &pic->bit_stream;
    data_bits = strm->byte_pos << 3;
    assert(strm->bits_to_go == 8);
    if (ctrl->func_out_strm) {
        ctrl->func_out_strm(ctrl->priv_data, strm->streamBuffer, strm->byte_pos, img_org->pts, img_org->dts, pic->type);
    }

    if (input->rate_ctrl) {
        avs3_rc_update_param_t rc_param;
        
        rc_param.info.frameLevel    = (pic->type & I_FRM) ? 0 : pic->curr_RPS.layer + 1;
        rc_param.info.coi           = pic->coi;
        rc_param.info.qp            = (int)(pic->real_qp + 0.5);
        rc_param.info.lambda        = pic->real_lambda;
        rc_param.coef_bits          = pic->coef_bits;

        rc_param.info.type           = img_org->type;
        rc_param.info.poc            = img_org->poc;
        rc_param.info.target_bits    = img_org->target_bits;
        rc_param.info.pic_cpp        = img_org->cpp;
        rc_param.info.scene_cut      = img_org->scene_cut;
        rc_param.info.coded_list_idx = img_org->coded_list_idx;

        rc_param.header_bits        = header_bits;
        rc_param.data_bits          = data_bits;

        if (input->bit_depth == 10) {
            rc_param.info.qp -= 16;
            rc_param.info.lambda /= 16.8838;
        }

        ctrl->rc_update(ctrl->rc_handle, &rc_param, ext_info, 1024);
    } else {
        sprintf(ext_info, "(¦Ë: %8.2f Cpp: %8.6f aBpp: %8.6f Coef: %4.1f%% )", 
            input->bit_depth == 8 ? img_org->initLambda : img_org->initLambda / 16.8838,
            img_org->cpp,
            (header_bits + data_bits) * 1.0 / input->img_width / input->img_height,
            pic->coef_bits * 100.0 / (header_bits + data_bits));
    }

    if (ctrl->func_out_rec) {
        ctrl->func_out_rec(ctrl->priv_data, pic->img_rec, pic->img_org, NULL, pic->type, img_org->poc, header_bits + data_bits, pic->real_qp, frm_time, ext_info);
    }

    avs3_pthread_mutex_lock(&ctrl->list_mutex);

    pic->p_cur_frm->ref_cnt--;
    try_to_release_rec(ctrl, pic->p_cur_frm);

    for (i = 0; i < pic->refs_num; i++) {
        pic->ref_list[i]->ref_cnt--;
        try_to_release_rec(ctrl, pic->ref_list[i]);
    }

    avs3_pthread_mutex_unlock(&ctrl->list_mutex);
}

void encode_one_frame(avs3_ctrl_t *ctrl, frame_t *frm, double* cpp_list, int cpps)
{
    const cfg_param_t *input = &ctrl->input;
    avs3_enc_t *pic = &ctrl->frm_encoder[ctrl->frm_idx];
    image_t *img_org = frm->img_org;

    ctrl->frm_idx = (ctrl->frm_idx + 1) % input->threads_frm;

    init_frame(ctrl, pic, frm, cpp_list, cpps);

    assert(frm->rec == NULL);
    frm->rec = xl_remove_head(&ctrl->rec_list, 1);
    assert(frm->rec);

    pic->adaptiveQP = input->rate_ctrl == RC_TYPE_CBR;
    pic->write_seqs_hdr = 0;
    pic->write_seqs_end = 0;

    if (img_org->type & I_FRM) {
        pic->write_seqs_hdr = 1;
        if (ctrl->write_end_code > 0) {
            pic->write_seqs_end = 1;
        }
    }

    if (input->tools.use_sao) {
        pic->sao_enc.sao_on[0] = 1;
        pic->sao_enc.sao_on[1] = 1;
        pic->sao_enc.sao_on[2] = 1;

        if (img_org->type == B_FRM && input->bit_depth == 8) {
            pic->sao_enc.sao_on[0] = 0;
        }
        if (img_org->type == B_FRM) {
            pic->sao_enc.sao_on[1] = 0;
            pic->sao_enc.sao_on[2] = 0;

            if (!frm->curr_RPS.referd_by_others) {
                pic->sao_enc.sao_on[0] = 0;
            }
        }
        if (input->slice_sao_enable_Y != -1) {
            pic->sao_enc.sao_on[0] = input->slice_sao_enable_Y;
        }
        if (input->slice_sao_enable_Cb != -1) {
            pic->sao_enc.sao_on[1] = input->slice_sao_enable_Cb;
        }
        if (input->slice_sao_enable_Cr != -1) {
            pic->sao_enc.sao_on[2] = input->slice_sao_enable_Cr;
        }
    } else {
        pic->sao_enc.sao_on[0] = pic->sao_enc.sao_on[1] = pic->sao_enc.sao_on[2] = 0;
    }
    pic->sao_enc.sao_on[3] = pic->sao_enc.sao_on[0] | pic->sao_enc.sao_on[1] | pic->sao_enc.sao_on[2];

    pic->type           = img_org->type;
    pic->poc            = img_org->poc;
    pic->refs_num       = frm->curr_RPS.num_of_ref;
    pic->coi            = frm->coi;
    pic->curr_RPS       = frm->curr_RPS;
    pic->p_cur_frm      = frm;
    pic->img_org        = frm->img_org;
    pic->img_rec        = &frm->rec->img_1_4th[0][0];
    pic->coef_bits      = 0;

    memcpy(pic->ref_list, frm->ref_list, sizeof(frame_t*) * 4);

    avs3_threadpool_run(ctrl->frm_threadpool, picture_data, pic, 1);

    if (ctrl->act_frm_thds < input->threads_frm) {
        ctrl->act_frm_thds++;
    }

    if (ctrl->act_frm_thds == input->threads_frm) {
        avs3_enc_t *pic = avs3_threadpool_wait(ctrl->frm_threadpool, &ctrl->frm_encoder[ctrl->frm_idx]);
        end_one_frame(ctrl, pic, pic->encode_time);
    }
}

void flush_frames(avs3_ctrl_t *ctrl)
{
    const cfg_param_t *input = &ctrl->input;
    avs3_enc_t *pic;
    frame_t *frm_list = ctrl->all_frm_list;
    int i;

    if (ctrl->act_frm_thds < input->threads_frm) {
        ctrl->frm_idx = input->threads_frm - 1;
        ctrl->act_frm_thds++;
    }

    while (--ctrl->act_frm_thds) {
        ctrl->frm_idx = (ctrl->frm_idx + 1) % input->threads_frm;
        pic = avs3_threadpool_wait(ctrl->frm_threadpool, &ctrl->frm_encoder[ctrl->frm_idx]);
        end_one_frame(ctrl, pic, pic->encode_time);
    }

    for (i = 0; i < ctrl->frm_list_size; i++) {
        if (frm_list[i].rec) {
            xl_append(&ctrl->rec_list, frm_list[i].rec);
            frm_list[i].rec = NULL;
        }
        if (frm_list[i].img_org) {
            xl_append(&ctrl->imglist_idle, frm_list[i].img_org);
            frm_list[i].img_org = NULL;
        }
    }
}

frame_t* prepare_RefInfo(avs3_ctrl_t *ctrl, ref_man* curr_RPS, int type, i64s_t poc)
{
    int i, j, k;
    frame_t *p_cur_frm;
    frame_t *frm_list = ctrl->all_frm_list;
    int refs_num;
    int frm_list_size = ctrl->frm_list_size;

    if (poc > ctrl->curr_idr_poc) {
        ctrl->last_idr_poc = ctrl->curr_idr_poc;
        ctrl->last_idr_coi = ctrl->curr_idr_coi;
    }

    for (i = 0, k = 0; i < curr_RPS->num_to_remove; i++) {
        for (j = 0; j < frm_list_size; j++) {
            if (frm_list[j].b_ref && frm_list[j].coi == ctrl->coi - curr_RPS->remove_pic[i]) {
                frm_list[j].b_ref = 0;
                try_to_release_rec(ctrl, &frm_list[j]);
                curr_RPS->remove_pic[k] = curr_RPS->remove_pic[i];
                k++;
                break;
            }
        }
    }
    for (i = 0; i < frm_list_size; i++) {
        if (frm_list[i].b_ref && (frm_list[i].coi < ctrl->last_idr_coi || frm_list[i].coi + 16 < ctrl->coi)) {
            frm_list[i].b_ref = 0;
            try_to_release_rec(ctrl, &frm_list[i]);
            curr_RPS->remove_pic[k] = (int)(ctrl->coi - frm_list[i].coi);
            k++;
            break;
        }
    }
    curr_RPS->num_to_remove = k;

    /* look for a free frm */
    for (i = 0; i < frm_list_size; i++) {
        if (frm_list[i].ref_cnt == 0 && frm_list[i].b_ref == 0) {
            break;
        }
    }

    assert(i < frm_list_size);

    p_cur_frm = &frm_list[i];
    p_cur_frm->poc = poc;
    p_cur_frm->coi = ctrl->coi;
    p_cur_frm->finished_lines = 0;
    p_cur_frm->randomaccese_flag = (p_cur_frm->poc >= ctrl->curr_idr_poc);

    /* build ref list */
    refs_num = 0;

    for (i = 0; i < curr_RPS->num_of_ref; i++) {
        i64s_t tmp_poc = -1;
        for (k = 0; k < frm_list_size; k++) {
            if (frm_list[k].b_ref && p_cur_frm->coi - curr_RPS->ref_pic[i] == frm_list[k].coi) {
                tmp_poc = frm_list[k].poc;
                break;
            }
        }
        if (type == B_FRM) {
            if (i == 0 && tmp_poc < poc) {
                for (k = 0; k < frm_list_size; k++) {
                    if (frm_list[k].b_ref && frm_list[k].poc > poc) {
                        tmp_poc = frm_list[k].poc;
                        curr_RPS->ref_pic[i] = (int)(p_cur_frm->coi - frm_list[k].coi);
                        break;
                    }
                }
            }
            if (i == 1 && (tmp_poc > poc || tmp_poc == -1)) {
                for (k = 0; k < frm_list_size; k++) {
                    if (frm_list[k].b_ref && frm_list[k].poc < poc && frm_list[k].poc >= ctrl->last_idr_poc) {
                        tmp_poc = frm_list[k].poc;
                        curr_RPS->ref_pic[i] = (int)(p_cur_frm->coi - frm_list[k].coi);
                        break;
                    }
                }
            }
        } else if (tmp_poc < ctrl->last_idr_poc) { /* didn't find ref frame, change to IDR frame */
            int accumulate = 0;
            curr_RPS->ref_pic[i] = (int)(p_cur_frm->coi - ctrl->last_idr_coi);

            for (j = 0; j < curr_RPS->num_of_ref; j++) {
                if (j != i && curr_RPS->ref_pic[j] == curr_RPS->ref_pic[i]) {
                    accumulate++;
                    break;
                }
            }
            if (!accumulate) {
                for (k = 0; k < frm_list_size; k++) {
                    if (frm_list[k].b_ref && frm_list[k].coi == ctrl->last_idr_coi) {
                        break;
                    }
                }
            } else {
                k = frm_list_size;
            }
        }

        if (k < frm_list_size) {
            p_cur_frm->ref_list[refs_num++] = &frm_list[k];
        }
    }

    if ((type & FP_FLG) && refs_num < curr_RPS->num_of_ref) {
        for (i = refs_num; i < curr_RPS->num_of_ref; i++) {
            i64s_t min_poc = ctrl->last_idr_poc;

            for (j = 0; j < frm_list_size; j++) {
                if (frm_list[j].b_ref && frm_list[j].poc > min_poc) {
                    int m;
                    i64s_t ref_poc = frm_list[j].poc;
                    assert(poc - ref_poc < 128);
                    for (m = 0; m < refs_num; m++) {
                        if (p_cur_frm->ref_list[m] == &frm_list[j]) {
                            break;
                        }
                    }
                    if (m == refs_num) {
                        k = j;
                        min_poc = ref_poc;
                    }
                }
            }
            if (min_poc > ctrl->last_idr_poc) {
                curr_RPS->ref_pic[i] = (int)(p_cur_frm->coi - frm_list[k].coi);
                p_cur_frm->ref_list[refs_num++] = &frm_list[k];
            } else {
                break;
            }
        }
    }

    com_rpl_t *l0 = &curr_RPS->l0;
    com_rpl_t *l1 = &curr_RPS->l1;

    curr_RPS->num_of_ref = refs_num;
    l0->num = l0->active = 0;
    l1->num = l1->active = 0;

    if (type & I_FRM) {
        for (j = 0; j < 4; j++) {
            p_cur_frm->ref_poc[j] = poc;
        }
    } else if (type != B_FRM) {
        for (j = 0; j < refs_num; j++) {
            p_cur_frm->ref_poc[j] = p_cur_frm->ref_list[j]->poc;
            l0->delta_doi[l0->active++] = (int)(p_cur_frm->coi - p_cur_frm->ref_list[j]->coi);
            l0->num++;
        }
        for (; j < 4; j++) {
            p_cur_frm->ref_poc[j] = 0;
        }
    } else {
        p_cur_frm->ref_poc[0] = p_cur_frm->ref_list[1]->poc;
        p_cur_frm->ref_poc[1] = p_cur_frm->ref_list[0]->poc;
        p_cur_frm->ref_poc[2] = 0;
        p_cur_frm->ref_poc[3] = 0;

        l0->delta_doi[l0->active++] = (int)(p_cur_frm->coi - p_cur_frm->ref_list[1]->coi);
        l0->num++;
        l1->delta_doi[l1->active++] = (int)(p_cur_frm->coi - p_cur_frm->ref_list[0]->coi);
        l1->num++;
    }

    for (j = 0; j < frm_list_size; j++) {
        if (frm_list[j].b_ref) {
            int found = 0;
            int delta_doi = (int)(p_cur_frm->coi - frm_list[j].coi);

            for (int i = 0; i < l0->num; i++) {
                if (delta_doi == l0->delta_doi[i]) {
                    found = 1;
                }
            }
            if (!found) {
                for (int i = 0; i < l1->num; i++) {
                    if (delta_doi == l1->delta_doi[i]) {
                        found = 1;
                    }
                }
            }
            if (!found) {
                l0->delta_doi[l0->num++] = delta_doi;
            }
        }
    }

    p_cur_frm->ref_cnt++;

    if (curr_RPS->referd_by_others) {
        p_cur_frm->b_ref = 1;
    }
    for (j = 0; j < refs_num; j++) {
        p_cur_frm->ref_list[j]->ref_cnt++;
    }


    return p_cur_frm;
}

void refine_pic_qp(const cfg_param_t *input, int initQP, image_t *img_org)
{
    double lambda;
    int qp = max(0, initQP - SHIFT_QP);
    int scale_qp = max(0, qp - input->bitdepth_qp_offset);

    if (input->intra_period == 1) {
        lambda = 0.85 * pow(2, qp / 4.0) *  LAM_2Level_TU;
        img_org->initQP = initQP;
    } else {
        double adj_ratio = (img_org->type == B_FRM ? 1.2 : 0.8);

        lambda = 0.68 * pow(2, qp / 4.0) * adj_ratio;

        if (img_org->type == B_FRM) {
            lambda *= max(2.00, min(4.00, scale_qp / 8.0));
        } else if (img_org->type & FP_FLG) {
            lambda *= 1.25;
        }
        img_org->initQP = (int)(5.661 * log(lambda) + 13.131 + 0.5);
    }

    img_org->initLambda = lambda;
    img_org->initQP = Clip3(0, MAX_QP + input->bitdepth_qp_offset, img_org->initQP);
}

void init_frame(avs3_ctrl_t *ctrl, avs3_enc_t *pic, frame_t *frm, double* cpp_list, int cpps)
{
    image_t *img_org = frm->img_org;
    const cfg_param_t *input = &ctrl->input;
    int gop_size = input->gop_size;
    int i;
    int b_IFrm = (img_org->type & I_FRM);
    ref_man *curr_RPS = &frm->curr_RPS;

    for (i = 0; i < curr_RPS->num_of_ref; i++) {
        pic->ref_dist[i] = abs((int)(img_org->poc - frm->ref_list[i]->poc));
    }

    if (!input->rate_ctrl) { 
        int baseQP;
        if (b_IFrm) {
            baseQP = input->baseQP;
        } else {
            baseQP = input->baseQP + curr_RPS->qp_offset;
        }
        refine_pic_qp(input, baseQP, img_org);
    } else {
        avs3_rc_get_qp_param_t rc_param;

        rc_param.info.frameLevel = b_IFrm ? 0 : curr_RPS->layer + 1;
        rc_param.info.coi           = frm->coi;
        rc_param.info.type          = img_org->type;
        rc_param.info.poc           = img_org->poc;
        rc_param.info.pic_cpp       = b_IFrm ? img_org->intra_cpp : img_org->inter_cpp;
        rc_param.info.inter_cpp     = img_org->inter_cpp;
        rc_param.info.intra_cpp     = img_org->intra_cpp;
        rc_param.info.scene_cut     = img_org->scene_cut;
        rc_param.bfrms_in_subgop    = cpps - 1;
        rc_param.sub_gop_cpp_list   = cpp_list;

        if (!b_IFrm) {
            rc_param.ref_lambda[0] = frm->ref_list[0]->img_org->initLambda;
            if (img_org->type == B_FRM) {
                rc_param.ref_lambda[1] = frm->ref_list[1]->img_org->initLambda;
            }
            if (input->bit_depth == 10) {
                rc_param.ref_lambda[0] /= 16.8838;
                rc_param.ref_lambda[1] /= 16.8838;
            }
        }

        ctrl->rc_get_qp(ctrl->rc_handle, &rc_param);

        img_org->initQP         = (int)rc_param.info.qp;
        img_org->initLambda     = rc_param.info.lambda;
        img_org->target_bits    = rc_param.info.target_bits;
        img_org->cpp            = rc_param.info.pic_cpp;
        img_org->coded_list_idx = rc_param.info.coded_list_idx;

        if (input->bit_depth == 10) {
            img_org->initQP += 16;
            img_org->initLambda *= 16.8838;
        }

        img_org->initQP = Clip3(0, MAX_QP + (input->sample_bit_depth - 8) * 8, img_org->initQP);
    }
	
    for (i = 0; i < input->pic_height_in_lcu; i++) {
        pic->slice_nr[i] = i / input->lcu_rows_in_slice;
    }

    if (input->use_hdr_chroma_opt && img_org->type != B_FRM) {
        pic->chroma_delta_qp[0] = (int)(-0.1141 * img_org->initQP - 0.8831 - 0.5);
        pic->chroma_delta_qp[0] = max(pic->chroma_delta_qp[0], -6);
        pic->chroma_delta_qp[1] = (img_org->initQP > 40 ? -6 : -5);
    }
}

int frame_create(frame_t *frm, cfg_param_t *input)
{
    int frm_mem_size = sizeof(frame_t);
    int frm_data_size = 0;
    uchar_t *mem_base;

    memset(frm, 0, sizeof(frame_t));

    frm->coi = -1;
    frm->ref_cnt = 0;
    frm->b_ref = 0;

    frm_data_size += input->b8_size * sizeof(i16s_t)* 2  + // frm->mvbuf
                     input->b8_size * sizeof(char_t)     + // frm->refbuf
                     ALIGN_BASIC * 2;

    frm_mem_size += frm_data_size;

    mem_base = com_malloc(frm_data_size, 1);
    frm->frm_data_base = mem_base;

    frm->mvbuf  = ((i16s_t(*)[2])mem_base) + input->b8_stride + 1; mem_base += input->b8_size * sizeof(i16s_t)* 2;
    frm->refbuf = ((char_t*     )mem_base) + input->b8_stride + 1; mem_base += input->b8_size * sizeof(char_t);

    avs3_pthread_mutex_init(&frm->mutex, NULL);
    avs3_pthread_cond_init(&frm->cv, NULL);

    return frm_mem_size + sizeof(frame_t);
}

void frame_destroy(frame_t *frm)
{
    avs3_pthread_mutex_destroy(&frm->mutex);
    avs3_pthread_cond_destroy(&frm->cv);
    com_free(frm->frm_data_base);
}