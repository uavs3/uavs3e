#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "global.h"
#include "commonVariables.h"
#include "header.h"
#include "rdoq.h"
#include "vlc.h"
#include "image.h"


#include "AEC.h"
#include "loop-filter.h"
#include "commonStructures.h"
#include "image.h"


void terminate_slice(avs3_enc_t *h)
{
    bit_stream_t *strm = &h->bit_stream;
    aec_t *aec = &h->aec;
    
    arienco_done_encoding(aec);

    strm->bits_to_go = 8;
    strm->byte_buf = 0;

    U_V(24, "start code prefix", 1, strm);
    U_V(8,  "patch end", 0x8F, strm);
}


void lcu_start(avs3_enc_t *h)
{
    int i;
    int i_stride = h->img_org->i_stride[0];
    int pix_x  = h->lcu_pix_x;
    int pix_y  = h->lcu_pix_y;
    int width  = h->lcu_width;
    int height = h->lcu_height;
    int lcu_size = LCU_SIZE;
    int lcu_y = pix_y / lcu_size;

    pel_t *p_recy = h->p_rec[0] - CACHE_STRIDE;
    pel_t *p_recu = h->p_rec[1] - CACHE_STRIDE;
    pel_t *p_recv = h->p_rec[2] - CACHE_STRIDE;

    pel_t *p_left_y = h->cache.intra_left_luma;
    pel_t *p_left_u = h->cache.intra_left_chroma[0];
    pel_t *p_left_v = h->cache.intra_left_chroma[1];

    for (i = 0; i <= height; i++) {
        *p_left_y++ = p_recy[lcu_size - 1]; p_recy += CACHE_STRIDE;
    }

    g_funs_handle.cpy_pel[width >> 1](h->img_org->plane[0] + pix_y * i_stride + pix_x, i_stride, h->p_org[0], CACHE_STRIDE, width, height);

    i_stride = h->img_org->i_stride[1];

    pix_x    >>= 1;
    pix_y    >>= 1;
    width    >>= 1;
    height   >>= 1;
    lcu_size >>= 1;

    for (i = 0; i <= height; i++) {
        *p_left_u++ = p_recu[lcu_size - 1]; p_recu += CACHE_STRIDE;
        *p_left_v++ = p_recv[lcu_size - 1]; p_recv += CACHE_STRIDE;
    }

    g_funs_handle.cpy_pel[width >> 1](h->img_org->plane[1] + pix_y * i_stride + pix_x, i_stride, h->p_org[1], CACHE_STRIDE, width, height);
    g_funs_handle.cpy_pel[width >> 1](h->img_org->plane[2] + pix_y * i_stride + pix_x, i_stride, h->p_org[2], CACHE_STRIDE, width, height);
}

void lcu_end(avs3_enc_t *h)
{
    int i_stride = h->img_rec->i_stride[0];
    int pix_x = h->lcu_pix_x;
    int pix_y = h->lcu_pix_y;
    int width = h->lcu_width;
    int height = h->lcu_height;
    pel_t *p_recy = h->p_rec[0];
    pel_t *p_recu = h->p_rec[1];
    pel_t *p_recv = h->p_rec[2];
    int lcu_y = pix_y >> LCU_SIZE_IN_BITS;

    g_funs_handle.cpy_pel[width >> 1](p_recy, CACHE_STRIDE, h->img_rec->plane[0] + pix_y * i_stride + pix_x, i_stride, width, height);
    memcpy(h->p_rec_uprow[lcu_y][0] + pix_x, p_recy + (height - 1) * CACHE_STRIDE, width * sizeof(pel_t));

    i_stride = h->img_rec->i_stride[1];
    pix_x >>= 1;
    pix_y >>= 1;
    width >>= 1;
    height >>= 1;

    g_funs_handle.cpy_pel[width >> 1](p_recu, CACHE_STRIDE, h->img_rec->plane[1] + pix_y * i_stride + pix_x, i_stride, width, height);
    g_funs_handle.cpy_pel[width >> 1](p_recv, CACHE_STRIDE, h->img_rec->plane[2] + pix_y * i_stride + pix_x, i_stride, width, height);
    memcpy(h->p_rec_uprow[lcu_y][1] + pix_x, p_recu + (height - 1) * CACHE_STRIDE, width * sizeof(pel_t));
    memcpy(h->p_rec_uprow[lcu_y][2] + pix_x, p_recv + (height - 1) * CACHE_STRIDE, width * sizeof(pel_t));
}
 
void* encode_lcu_row(void* param)
{
    avs3_enc_t *h = (avs3_enc_t*)param;
    analyzer_t *a = &h->analyzer;
    const int lcu_y = h->lcu_y;
	int lcu_x;
    const cfg_param_t *input = h->input;
    sao_enc_t *sao_enc = &h->sao_enc;

    int bfinished = (lcu_y == input->pic_height_in_lcu - 1);
    int lcu_size = LCU_SIZE;
    int max_cu = tab_log2[MAX_CU_SIZE], min_cu = 3;

    uavs3e_sem_t *sem_up   = NULL;
    uavs3e_sem_t *sem_curr = NULL;

    if (input->threads_wpp > 1) {
        sem_up = lcu_y ? &h->wpp_sem[lcu_y - 1] : NULL;
        sem_curr = (lcu_y == input->pic_height_in_lcu - 1) ? NULL : &h->wpp_sem[lcu_y];

        if (lcu_y % input->lcu_rows_in_slice == 0) {
            sem_up = NULL;
        } else if ((lcu_y + 1) % input->lcu_rows_in_slice == 0) {
            sem_curr = NULL;
        }
    }

    a->lambda_mode =  h->img_org->initLambda;
    a->rdoq_lambda = (a->lambda_mode * 75) / 100;

    if (h->type & FP_FLG) {
        a->rdoq_lambda = (a->rdoq_lambda * 120) / 100;
    }
    if (input->bit_depth == 10) {
        a->rdoq_lambda /= 16.8838;
    }

    me_module_create_mvcost(&a->me_info, LAMBDA_FACTOR(sqrt(a->lambda_mode)));

    if (h->refs_num) {
        wait_ref_available(h, h->ref_list[0], (lcu_y + 1) * LCU_SIZE - 8);
    }

    for (lcu_x = 0; lcu_x < input->pic_width_in_lcu; lcu_x++) {
        int lcu_idx = lcu_y * input->pic_width_in_lcu + lcu_x;
        int uiPositionInPic = lcu_y * (LCU_SIZE / 8) * input->pic_width_in_mcu + lcu_x * (LCU_SIZE / 8);

        aec_t cs_bak;
        int lcu_qp;
        double est_bits;

        init_lcu_pos(h, lcu_idx);
        lcu_start(h);

        safe_sem_wait(sem_up);

        if (lcu_x == 0) {
            if (lcu_y % input->lcu_rows_in_slice == 0) {
                arienco_start_encoding(&h->aec, NULL, NULL);
            } else {
                reset_coding_state(&h->wpp_aec[lcu_y - 1]);
            }
        }
        set_aec_functions(&h->aec, 2);
        store_coding_state_hdr(&cs_bak);

       
        if (h->adaptiveQP) {
            lcu_qp = h->img_org->initQP + h->pic_delta_qp[0];
            a->lambda_mode = exp((lcu_qp - 13.131) / 5.661);
            a->rdoq_lambda = (a->lambda_mode * 75) / 100;

            if (h->type & FP_FLG) {
                a->rdoq_lambda = (a->rdoq_lambda * 120) / 100;
            }
            if (input->bit_depth == 10) {
                a->rdoq_lambda /= 16.8838;
            }
            lcu_qp = Clip3(0, MAX_QP + (input->bit_depth - 8) * 8, lcu_qp);
        } else {
            lcu_qp = h->img_org->initQP;
        }

        analyze_cu_tree(h, &h->aec, LCU_SIZE_IN_BITS, max_cu, min_cu, uiPositionInPic, lcu_qp, a->lambda_mode, MAX_COST, &est_bits);

        h->lcu_row_est_bits[lcu_y] += est_bits;
        h->lcu_row_finished[lcu_y] = lcu_x + 1;
        lcu_end(h);

        if (input->tools.use_deblk) {
            set_cu_deblk_flag(h, LCU_SIZE_IN_BITS, uiPositionInPic);
            deblock_one_lcu(h, h->img_rec, lcu_x, lcu_y);
        }
        if (lcu_x > 1) {
            safe_sem_post(sem_curr);
        }
        if (h->sao_enc.sao_on[3]) {
            SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES];
            memset(&saostatData, 0, sizeof(saostatData));
            sao_lcu_statistics(h, lcu_x, lcu_y, saostatData);
            sao_lcu_rd_decision(h, lcu_x, lcu_y, &h->aec, saostatData, sao_enc->saoBlkParams[lcu_idx], h->img_org->initLambda);
            sao_lcu_write(h, lcu_x, lcu_y, &h->aec, sao_enc->saoBlkParams[lcu_idx]);
        }

        reset_coding_state_hdr(&cs_bak);
        set_aec_functions(&h->aec, 1);

        write_cu_tree(h, &h->aec, LCU_SIZE_IN_BITS, uiPositionInPic);

        rdoq_init_cu_est_bits(h, &h->aec);

        if (input->threads_wpp > 1 || KEEP_CONST) {
            if (lcu_x == 1) {
                store_coding_state(&h->wpp_aec[lcu_y]);
                safe_sem_post(sem_curr);
            }
        } else {
            if (lcu_x == input->pic_width_in_lcu - 1) {
                store_coding_state(&h->wpp_aec[lcu_y]);
            }
        }

        if (!KEEP_CONST && input->rate_ctrl == RC_TYPE_CBR && (lcu_x % (input->pic_width_in_lcu / 5) == 0)) {
            int i;
            double est_percent, target_percent = 0;
            double est_bits = 0;
            double *cpp_list;

            if (h->type == I_FRM) {
                cpp_list = h->img_org->intra_cpp_list;
            } else {
                cpp_list = h->img_org->inter_cpp_list;
            }
            for (i = 0; i < input->pic_height_in_lcu; i++) {
                target_percent += cpp_list[i] * h->lcu_row_finished[i] / input->pic_width_in_lcu;
                est_bits += h->lcu_row_est_bits[i];
            }
            est_percent = est_bits * 1.0 / h->img_org->target_bits;

            if (target_percent > 0.05 || est_percent > 0.05) {
                double diff = est_percent / target_percent;
                h->pic_delta_qp[0] = (int)Clip3(-6, 8, 8 * (diff - 1) + 0.5);
            }
        }
    }

    safe_sem_post(h->sem_enc + lcu_y);
    safe_sem_post(sem_curr);
    safe_sem_wait(sem_up);

    if (h->sao_enc.sao_on[3]) {
        int lcu_idx = lcu_y * input->pic_width_in_lcu;
        image_copy_lcu_row_for_sao(h, &h->img_sao, h->img_rec, lcu_y);

        for (lcu_x = 0; lcu_x < input->pic_width_in_lcu; lcu_x++, lcu_idx++) {
            SAO_on_smb(h, lcu_x, lcu_y, sao_enc->saoBlkParams[lcu_idx]);
        }
    }

    safe_sem_post(sem_curr);
    

    if (h->curr_RPS.referd_by_others) {
        frame_t *p_cur_frm = h->p_cur_frm;
        int finished_lines;
        image_padding_lcu_row(h, h->img_rec, lcu_y, IMG_PAD_SIZE);
        finished_lines = com_if_luma_lcu_row(h, (image_t(*)[4])h->img_rec, lcu_y, sem_up, sem_curr);

        safe_sem_wait(sem_up);

        avs3_pthread_mutex_lock(&p_cur_frm->mutex);
        p_cur_frm->finished_lines = finished_lines;
        avs3_pthread_cond_broadcast(&p_cur_frm->cv);
        avs3_pthread_mutex_unlock(&p_cur_frm->mutex);

        safe_sem_post(sem_curr);
    }

    return NULL;
}

void *post_entropy_thread(void *param) 
{
    avs3_enc_t *pic = (avs3_enc_t *)param;
    const cfg_param_t *input = pic->input;
    int lcu_y, lcu_x;
    bit_stream_t *strm = &pic->bit_stream;
    sao_enc_t *sao_enc = &pic->sao_enc;
    image_t *img_org = pic->img_org;
    int patch_idx = 0; // todo: get the real idx
    int last_lcu_qp = img_org->initQP;
    int last_lcu_delta_qp = 0;

    strm->byte_pos = 0;
    strm->bits_to_go = 8;

    for (lcu_y = 0; lcu_y < input->pic_height_in_lcu; lcu_y++) {
        int last_row = 0;

        if ((lcu_y + 1) % input->lcu_rows_in_slice == 0 || lcu_y == input->pic_height_in_lcu - 1) {
            last_row = 1;
        }

        safe_sem_wait(pic->sem_enc + lcu_y);

        if (lcu_y % input->lcu_rows_in_slice == 0) {
            write_slice_header(pic, strm, patch_idx, pic->img_org->initQP);
            set_aec_functions(&pic->aec, 0);
            arienco_start_encoding(&pic->aec, strm->streamBuffer, &(strm->byte_pos));
        }

        for (lcu_x = 0; lcu_x < input->pic_width_in_lcu; lcu_x++) {
            int lcu_idx = lcu_y * input->pic_width_in_lcu + lcu_x;
            int uiPositionInPic = (lcu_y * input->pic_width_in_mcu + lcu_x) * (LCU_SIZE / 8);
            cu_t *cu = &pic->cu_data[uiPositionInPic];

            pic->real_qp += cu->QP;
            pic->real_lambda += cu->lambda;

            init_lcu_pos(pic, lcu_idx);

            if (pic->adaptiveQP) {
                int qp = cu->QP;
                int dqp = qp - last_lcu_qp;
                lbac_enc_lcu_delta_qp(&pic->aec, dqp, last_lcu_delta_qp);
                last_lcu_qp = qp;
                last_lcu_delta_qp = dqp;
            }

            if (pic->sao_enc.sao_on[3]) {
                sao_lcu_write(pic, lcu_x, lcu_y, &pic->aec, sao_enc->saoBlkParams[lcu_idx]);
            }
            write_cu_tree(pic, &pic->aec, LCU_SIZE_IN_BITS, uiPositionInPic);
            write_terminating_bit(&pic->aec, (lcu_x == input->pic_width_in_lcu - 1) && last_row);
        }

        if (last_row) {
            terminate_slice(pic);
        }
    }

    pic->real_qp     /= pic->input->pic_size_in_lcu;
    pic->real_lambda /= pic->input->pic_size_in_lcu;

    return NULL;
}

#if WATER_MARK
extern unsigned char water_mark_logo[WATER_MARK_SMALL_WIDTH * WATER_MARK_SMALL_HEIGHT];
extern unsigned char water_mark_logo_small[WATER_MARK_WIDTH * WATER_MARK_HEIGHT];
#endif

#if LIMIT_FRAMES
i64s_t total_frame_encoded = 0;
#endif

void* picture_data(void *param)
{
    avs3_enc_t *pic = (avs3_enc_t*)param;
    int lcu_y;
    const cfg_param_t *input = pic->input;
    i64s_t start_time = get_mdate();

#if WATER_MARK
    pel_t water_mark[WATER_MARK_WIDTH * WATER_MARK_HEIGHT];
    int i, water_mark_shift = pic->input->bit_depth - 8;
    int small_logo = pic->img_org->width < 1280;
    unsigned char *p_water_mark;
    int wm_width, wm_height;
    if (small_logo) {
        p_water_mark = water_mark_logo_small;
        wm_width = WATER_MARK_SMALL_WIDTH;
        wm_height = WATER_MARK_SMALL_HEIGHT;
    }
    else {
        p_water_mark = water_mark_logo;
        wm_width = WATER_MARK_WIDTH;
        wm_height = WATER_MARK_HEIGHT;
    }
    for (i = 0; i < wm_width * wm_height; i++) {
        water_mark[i] = ((pel_t)p_water_mark[i]) << water_mark_shift;
    }
    g_funs_handle.avg_pel[0](pic->img_org->plane[0], pic->img_org->i_stride[0], pic->img_org->plane[0], pic->img_org->i_stride[0], water_mark, wm_width, wm_width, wm_height);
#endif
#if LIMIT_FRAMES
    if (total_frame_encoded++ > LIMIT_FRAMES) {
        return pic;
    }
#endif

    memset(pic->p_cur_frm->refbuf - input->b8_stride - 1, -1, input->b8_size * sizeof(char_t));
    memset(pic->refbuf_bw         - input->b8_stride - 1, -1, input->b8_size * sizeof(char_t));
    memset(pic->pdir              - input->b8_stride - 1, -1, input->b8_size * sizeof(char_t));

    pic->hdr_stream.byte_pos = 0;
    pic->hdr_stream.bits_to_go = 8;

    if (pic->write_seqs_hdr) {
        if (pic->write_seqs_end) {
            U_V(32, "seqence end code", 0x1b1, &pic->hdr_stream);
        }
        WriteSequenceHeader(&pic->hdr_stream, (cfg_param_t *)input);
        //WriteSequenceUserData(&pic->hdr_stream);

        if (input->ColourPrimary) {
            WriteSequenceDisplayExtension(&pic->hdr_stream, input);
        }
        if (input->hdr_ext.enable) {
            WriteHdrExtension(&pic->hdr_stream, &input->hdr_ext);
        }
    }
    if (pic->type & I_FRM) {
        write_pic_hdr_i(pic);
    } else {
        write_pic_hdr_pb(pic);
    }
     
    memset(pic->deblk_flag, 0, input->pic_size_in_mcu * sizeof(char_t));
    memset(pic->lcu_row_est_bits, 0, input->pic_height_in_lcu * sizeof(double));
    memset(pic->lcu_row_finished, 0, input->pic_height_in_lcu * sizeof(int));

    pic->real_qp = 0;
    pic->real_lambda = 0;
    pic->pic_delta_qp[0] = 0;

    if (input->rate_ctrl == RC_TYPE_CBR) {
        double total_cpp = 0;
        image_t *img_org = pic->img_org;

        if (pic->type == I_FRM) {
            for (lcu_y = 0; lcu_y < input->pic_height_in_lcu; lcu_y++) {
                total_cpp += img_org->intra_cpp_list[lcu_y] = pow(img_org->intra_cpp_list[lcu_y], 1.33);
            }
            for (lcu_y = 0; lcu_y < input->pic_height_in_lcu; lcu_y++) {
                img_org->intra_cpp_list[lcu_y] = img_org->intra_cpp_list[lcu_y] / total_cpp;
            }
        } else {
            for (lcu_y = 0; lcu_y < input->pic_height_in_lcu; lcu_y++) {
                total_cpp += img_org->inter_cpp_list[lcu_y] = 0.05 * pow(img_org->inter_cpp_list[lcu_y], 1.5);
            }
            for (lcu_y = 0; lcu_y < input->pic_height_in_lcu; lcu_y++) {
                img_org->inter_cpp_list[lcu_y] = img_org->inter_cpp_list[lcu_y] / total_cpp;
            }
        }
    }

    avs3_threadpool_run(pic->post_threadpool, post_entropy_thread, pic, 1);
    
    for (lcu_y = 0; lcu_y < input->pic_height_in_lcu; lcu_y++) {
        memcpy(&pic->wpp_encoder[lcu_y], pic, &pic->sync_end - (char_t*)pic);
        pic->wpp_encoder[lcu_y].lcu_y = lcu_y;
        avs3_threadpool_run(pic->wpp_threadpool, encode_lcu_row, pic->wpp_encoder + lcu_y, 1);
    }

    avs3_threadpool_wait(pic->post_threadpool, pic);

    for (lcu_y = 0; lcu_y < input->pic_height_in_lcu; lcu_y++) {
        avs3_threadpool_wait(pic->wpp_threadpool, pic->wpp_encoder + lcu_y);
    }

    finish_picture_header(&pic->hdr_stream);

#if INSERT_MD5
    write_md5_usr_data(pic, pic->img_rec);
#endif

    pic->encode_time = (int)((get_mdate() - start_time) / 1000);
    return pic;
}



