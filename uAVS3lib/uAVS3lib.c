#include <math.h>
#include "uAVS3lib.h"
#include "global.h"
#include "commonVariables.h"

#include "AEC.h"
#include "image.h"
#include "header.h"
#include "vlc.h"
#include "wquant.h"
#include "block.h"
#include "rdoq.h"

#include "tools/uratectrl.h"
#include "tools/preprocess.h"

int cal_reorder_delay(cfg_param_t *input)
{
    int delta_dd = 100;
    int tmp_delta_dd;
    int cfg_dd_poc[32];
    int i;

    if (input->succ_bfrms) {
        for (i = 0; i < input->gop_size; i++) {
            cfg_dd_poc[i] = input->seq_ref_cfg[i].poc;
        }

        for (i = 0; i < input->gop_size; i++) {
            tmp_delta_dd = input->seq_ref_cfg[i].poc - (i + 1);
            if (tmp_delta_dd < delta_dd) {
                delta_dd = tmp_delta_dd;
            }
        }
        if (delta_dd < 0) {
            return abs(delta_dd);
        }
        else {
            return 0;
        }
    }
    return 0;
}

double imglist_create(avs3_ctrl_t *ctrl, cfg_param_t *input, int input_bufsize)
{
    int i, j, k;
    double mem_size = 0;
    
    xl_init(&ctrl->imglist_idle);
    xl_init(&ctrl->imglist_read);
    xl_init(&ctrl->frmlist_ready);

    xl_init(&ctrl->rec_list);

    for (i = 0; i < input_bufsize; i++) {
        image_t *img = com_malloc(sizeof(image_t), 1);
        mem_size += image_create_org(img, input->img_width, input->img_height) + sizeof(image_t);
        xl_append(&ctrl->imglist_idle, img);
    }

    for (k = 0; k < 1 + input->threads_frm + REF_MAXBUFFER + 1; k++) {
        frame_rec_t *rec = com_malloc(sizeof(frame_rec_t), 1);
    
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (i == 0 && j == 0) {
                    mem_size += image_create_rec(&rec->img_1_4th[0][0], input->img_width, input->img_height, 3);
                } else {
                    mem_size += image_create_rec(&rec->img_1_4th[i][j], input->img_width, input->img_height, 1);
                }
            }
        }

        xl_append(&ctrl->rec_list, rec);
    }

    return mem_size;
}

void imglist_destroy(avs3_ctrl_t *ctrl)
{
    image_t *node;
    frame_rec_t *rec;

    if (ctrl->img_next) {
        xl_append(&ctrl->imglist_idle, ctrl->img_next);
    }

    for (;;) {
        if ((node = xl_remove_head(&ctrl->imglist_read, 0)) == NULL) {
            break;
        }
        image_free(node);
        com_free(node);
    }

    for (;;) {
        if ((node = xl_remove_head(&ctrl->imglist_idle, 0)) == NULL) {
            break;
        }
        image_free(node);
        com_free(node);
    }

    for (;;) {
        int i, j;
        if ((rec = xl_remove_head(&ctrl->rec_list, 0)) == NULL) {
            break;
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                image_free(&rec->img_1_4th[i][j]);
            }
        }
        com_free(rec);
    }

    xl_destroy(&ctrl->imglist_idle);
    xl_destroy(&ctrl->imglist_read);
    xl_destroy(&ctrl->frmlist_ready);

    xl_destroy(&ctrl->rec_list);
}

void avs3_cor_init(avs3_enc_t *h)
{
    h->coi = -1;
    h->aec.syn_ctx = &h->syn_ctx;
    h->p_org[0] = h->cache.org_buf;
    h->p_org[1] = h->cache.org_buf + LCU_SIZE * CACHE_STRIDE;
    h->p_org[2] = h->cache.org_buf + LCU_SIZE * CACHE_STRIDE + LCU_SIZE / 2;
    h->p_rec[0] = h->cache.rec_buf;
    h->p_rec[1] = h->cache.rec_buf + LCU_SIZE * CACHE_STRIDE;
    h->p_rec[2] = h->cache.rec_buf + LCU_SIZE * CACHE_STRIDE + LCU_SIZE/ 2;
}

int avs3_core_pic_create(avs3_enc_t *h, cfg_param_t *input)
{
    int i, memory_size = 0;
    uchar_t *mem_base;

    avs3_cor_init(h);
    h->input = input;

    assert(input->img_height % 8 == 0 && input->img_width % 8 == 0);

    memory_size += input->pic_size_in_mcu     * sizeof(cu_t)    + // h->cu_data
        input->b8_size             * sizeof(i16s_t)* 2          + // h->mvbuf_bw
        input->b8_size             * sizeof(char_t)             + // h->refbuf_bw
        input->b8_size             * sizeof(char_t)             + // h->pdir
        input->b8_size             * sizeof(char_t)             + // h->ipredmode
        input->b8_size             * sizeof(char_t)             + // h->size
        input->pic_size_in_mcu     * sizeof(char_t)             + // h->deblk_flag
        input->pic_size_in_lcu     * sizeof(lcu_coef_t)         + // h->all_quant_coefs
        input->pic_height_in_lcu   * sizeof(aec_t)              + // h->wpp_aec
        input->pic_height_in_lcu   * sizeof(bin_ctx_sets_t)     + // h->wpp_syn_ctx
        input->pic_height_in_lcu + 2   * sizeof(int)            + // h->slice_nr
        input->pic_height_in_lcu   * sizeof(uavs3e_sem_t)       + // h->wpp_sem
        input->pic_height_in_lcu   * sizeof(uavs3e_sem_t)       + // h->sem_enc
        input->pic_height_in_lcu   * sizeof(avs3_enc_t)         + // h->wpp_encoder
        input->pic_height_in_lcu   * sizeof(double)             + // h->lcu_row_est_bits
        input->pic_height_in_lcu   * sizeof(int)                + // h->lcu_row_finished
        sizeof(int)                                             + // h->pic_delta_qp;
        
        (1024 + 256 + 64 + 16 + 16 * 2 + 64 * 2 + 256 * 2) * sizeof(i16s_t)                                 + // h->wquant_matrix
        (input->img_width + 16) * (input->img_height + 16) * 3 * sizeof(i16s_t)                             + // h->tmp_buf[3]
        (input->img_width + 64) *  input->pic_height_in_lcu * 2 * sizeof(pel_t) + input->pic_height_in_lcu * sizeof(void*)  + // h->p_rec_uprow[3]
        ALIGN_BASIC * 256;

    h->dat_base = mem_base = com_malloc(memory_size, 1);

#define ASSIGN_BUFFER(dst,type,offset,size) dst = (((type)mem_base)+(offset)); mem_base += (size);  mem_base = ALIGN_POINTER(mem_base);

    ASSIGN_BUFFER(h->mvbuf_bw         , i16s_t(*)[2]          , input->b8_stride + 1      , input->b8_size * sizeof(i16s_t)* 2);                   
    ASSIGN_BUFFER(h->refbuf_bw        , char_t*               , input->b8_stride + 1      , input->b8_size * sizeof(char_t));
    ASSIGN_BUFFER(h->pdir             , char_t*               , input->b8_stride + 1      , input->b8_size * sizeof(char_t));
    ASSIGN_BUFFER(h->size             , char_t*               , input->b8_stride + 1      , input->b8_size * sizeof(char_t));

    ASSIGN_BUFFER(h->ipredmode        , char_t*               , input->b8_stride + 1      , input->b8_size * sizeof(char_t));
    ASSIGN_BUFFER(h->cu_data          , cu_t*                 , 0                         ,  input->pic_size_in_mcu * sizeof(cu_t));
    ASSIGN_BUFFER(h->deblk_flag       , char_t*               , 0                         ,  input->pic_size_in_mcu * sizeof(char_t));
    ASSIGN_BUFFER(h->all_quant_coefs  , lcu_coef_t*           , 0                         ,  input->pic_size_in_lcu * sizeof(lcu_coef_t));
    ASSIGN_BUFFER(h->tmp_buf[0]       , i16s_t*               , 0                         ,  (input->img_height + 2 * 8) * (input->img_width + 2 * 8) * sizeof(i16s_t));
    ASSIGN_BUFFER(h->tmp_buf[1]       , i16s_t*               , 0                         ,  (input->img_height + 2 * 8) * (input->img_width + 2 * 8) * sizeof(i16s_t));
    ASSIGN_BUFFER(h->tmp_buf[2]       , i16s_t*               , 0                         ,  (input->img_height + 2 * 8) * (input->img_width + 2 * 8) * sizeof(i16s_t));
    ASSIGN_BUFFER(h->wpp_aec          , aec_t*                , 0                         ,  input->pic_height_in_lcu * sizeof(aec_t));
    ASSIGN_BUFFER(h->wpp_syn_ctx      , bin_ctx_sets_t*       , 0                         ,  input->pic_height_in_lcu * sizeof(bin_ctx_sets_t));
    ASSIGN_BUFFER(h->slice_nr         , int*                  , 1                         ,  (input->pic_height_in_lcu + 2) * sizeof(int));
    ASSIGN_BUFFER(h->wpp_sem          , uavs3e_sem_t*         , 0                         ,  input->pic_height_in_lcu * sizeof(uavs3e_sem_t));
    ASSIGN_BUFFER(h->wpp_encoder      , avs3_enc_t*           , 0                         ,  input->pic_height_in_lcu * sizeof(avs3_enc_t));
    ASSIGN_BUFFER(h->p_rec_uprow      , pel_t*(*)[3]          , 0                         ,  input->pic_height_in_lcu * sizeof(void*)* 3);
    ASSIGN_BUFFER(h->sem_enc          , uavs3e_sem_t*         , 0                         ,  input->pic_height_in_lcu * sizeof(uavs3e_sem_t));
    ASSIGN_BUFFER(h->lcu_row_est_bits , double*               , 0                         , input->pic_height_in_lcu * sizeof(double));
    ASSIGN_BUFFER(h->lcu_row_finished , int*                  , 0                         , input->pic_height_in_lcu * sizeof(int));
    ASSIGN_BUFFER(h->pic_delta_qp     , int*                  , 0                         ,  sizeof(int));

    memset(h->pdir - input->b8_stride - 1, -1, input->b8_size * sizeof(char_t));

    h->wpp_threadpool = avs3_threadpool_init(input->threads_wpp, input->pic_height_in_lcu, NULL, NULL);
    h->post_threadpool = avs3_threadpool_init(1, 1, NULL, NULL);

    h->slice_nr[-1] = h->slice_nr[input->pic_height_in_lcu] = -1;

    for (i = 0; i < input->pic_height_in_lcu; i++) {
        h->wpp_aec[i].syn_ctx = &h->wpp_syn_ctx[i];
        h->p_rec_uprow[i][0] = ((pel_t*)mem_base); mem_base += (input->img_width     + 63) / 64 * 64 * sizeof(pel_t);
        h->p_rec_uprow[i][1] = ((pel_t*)mem_base); mem_base += (input->img_width / 2 + 31) / 32 * 32 * sizeof(pel_t);
        h->p_rec_uprow[i][2] = ((pel_t*)mem_base); mem_base += (input->img_width / 2 + 31) / 32 * 32 * sizeof(pel_t);
        uavs3e_sem_init(&h->wpp_sem[i], 0, 0);
        uavs3e_sem_init(&h->sem_enc[i], 0, 0);
        avs3_cor_init(&h->wpp_encoder[i]);
        me_module_init(&h->wpp_encoder[i].analyzer.me_info);
    }
    mem_base = ALIGN_POINTER(mem_base);

    h->wquant_matrix[0][0] = (i16s_t*)mem_base;
    h->wquant_matrix[0][1] = h->wquant_matrix[0][0] +  16;
    h->wquant_matrix[0][2] = h->wquant_matrix[0][1] +  64;
    h->wquant_matrix[0][3] = h->wquant_matrix[0][2] + 256;
    
    h->wquant_matrix[1][1] = h->wquant_matrix[0][3] + 1024;
    h->wquant_matrix[1][2] = h->wquant_matrix[1][1] + 16;
    h->wquant_matrix[1][3] = h->wquant_matrix[1][2] + 64;

    h->wquant_matrix[2][1] = h->wquant_matrix[1][3] + 256;
    h->wquant_matrix[2][2] = h->wquant_matrix[2][1] + 16;
    h->wquant_matrix[2][3] = h->wquant_matrix[2][2] + 64;

    h->wquant_matrix[1][0] = NULL;
    h->wquant_matrix[2][0] = NULL;

    mem_base += (1024 + 256 + 64 + 16 + 16 * 2 + 64 * 2 + 256 * 2) * sizeof(i16s_t);

    cal_all_wq_matrix(h->wquant_matrix, (i16s_t*)tab_WqMDefault4x4, (i16s_t*)tab_WqMDefault8x8);

    h->hdr_stream.streamBuffer = (uchar_t *)com_malloc(1024 * 1024, 0);
    h->bit_stream.streamBuffer = (uchar_t *)com_malloc(input->img_width * input->img_height, 0);
    memory_size += input->img_width * input->img_height + 1024 * 1024;

    memory_size += image_create_rec(&h->img_sao, input->img_width, input->img_height, 3);
    h->sao_enc.saoBlkParams = (SAOBlkParam(*)[NUM_SAO_COMPONENTS])com_malloc(input->pic_size_in_lcu * sizeof(SAOBlkParam)* NUM_SAO_COMPONENTS, 0);
    memory_size += input->pic_size_in_lcu * (sizeof(SAOBlkParam)* NUM_SAO_COMPONENTS);

    return memory_size;
}

void avs3_core_pic_free(avs3_enc_t *h)
{
    int i;
    const cfg_param_t *input = h->input;

    sao_enc_t *sao_enc = &h->sao_enc;

    for (i = 0; i < input->pic_height_in_lcu; i++) {
        uavs3e_sem_destroy(&h->wpp_sem[i]);
        uavs3e_sem_destroy(&h->sem_enc[i]);
        avs3_me_free(&h->wpp_encoder[i].analyzer.me_info);
    }

    avs3_threadpool_delete(h->wpp_threadpool);
    avs3_threadpool_delete(h->post_threadpool);

    com_free(h->hdr_stream.streamBuffer);
    com_free(h->bit_stream.streamBuffer);

    image_free(&h->img_sao);

    com_free(sao_enc->saoBlkParams);

    com_free(h->dat_base);
}

void refine_input(cfg_param_t *input)
{
    int i;
    static tab_double_t FrameRateTab[9] = { 0, 24000.0 / 1001.0, 24.0, 25.0, 30000.0 / 1001.0, 30.0, 50.0, 60000.0 / 1001.0, 60.0 };
    double min_diff = 100;
    ref_man* refman = input->seq_ref_cfg;

    input->gop_size = input->succ_bfrms + 1;
    input->frame_rate = input->frame_rate_num * 1.0 / input->frame_rate_den;

    for (i = 0; i < 9; i++) {
        double diff = fabs(input->frame_rate - FrameRateTab[i]);
        if (diff < min_diff) {
            min_diff = diff;
            input->frame_rate_code = i;
        }
    }

    if (input->img_width % 32 != 0) {
        input->auto_crop_right = 32 - input->img_width % 32;
    } else {
        input->auto_crop_right = 0;
    }
    if (input->img_height % 32 != 0) {
        input->auto_crop_bottom = 32 - input->img_height % 32;
    } else {
        input->auto_crop_bottom = 0;
    }
    input->img_width  += input->auto_crop_right;
    input->img_height += input->auto_crop_bottom;

    input->pic_width_in_mcu = input->img_width >> 3;
    input->pic_height_in_mcu = input->img_height >> 3;
    input->pic_size_in_mcu = input->pic_width_in_mcu * input->pic_height_in_mcu;
     
    input->b8_stride = input->pic_width_in_mcu + 2;
    input->b8_size = input->b8_stride * (input->pic_height_in_mcu + 2);

    input->pic_width_in_lcu = (input->img_width + (LCU_SIZE - 1)) / LCU_SIZE;
    input->pic_height_in_lcu = (input->img_height + (LCU_SIZE - 1)) / LCU_SIZE;
    input->pic_size_in_lcu = input->pic_width_in_lcu * input->pic_height_in_lcu;

    input->pic_reorder_delay = cal_reorder_delay(input);
    input->bitdepth_qp_offset = (input->bit_depth - 8) * 8;
    input->threads_wpp = Clip3(1, input->pic_height_in_lcu, input->threads_wpp);
    input->threads_frm = Clip3(1, 33, input->threads_frm);

    input->intra_period = max(2, input->intra_period);

    printf("WPP threads of one frame: %d\n", input->threads_wpp);
    printf("FRM threads of one frame: %d\n\n", input->threads_frm);

    if (input->lcu_rows_in_slice <= 0) {
        input->lcu_rows_in_slice = input->pic_height_in_lcu;
    }

#if !COMPILE_10BIT
    if (input->bit_depth != 8) {
        input->bit_depth = 8;
        printf("!!! This program is compiled only for 8bit coding! CodingBitDepth are reset to 8 automatically !!!\n");
    }
#endif

    if (input->bit_depth == 8) {
        input->profile_id = 32;
    } else {
        input->profile_id = 34;
    }

    input->baseQP += input->bitdepth_qp_offset;
    input->baseQP = max(0, min(input->baseQP, MAX_QP + input->bitdepth_qp_offset));

    input->Min_V_MV = -2048;
    input->Max_V_MV =  2047;
    input->Min_H_MV = -8192;
    input->Max_H_MV =  8191;

    refman->num_of_ref = min(refman->num_of_ref, 2);
   
    /* decision basic tools */
    input->tools.use_deblk = 1;
    input->tools.use_sao  = 1;
    input->tools.use_secT = 1;

    input->speed_adj_rate = 1.0f;
}

void* encoding_thread(void *handle);
void* lookahead_thread(void *handle);

void* avs3_lib_create(cfg_param_t *input_init, strm_out_t strm_callbak, rec_out_t rec_callbak, int input_bufsize, void *priv_data)
{
    int i;
    avs3_ctrl_t *ctrl;
    double total_memory = 0;
    cfg_param_t *input;

    refine_input(input_init);

    input_bufsize = max(2 * input_init->succ_bfrms + 1 + input_init->threads_frm + REF_MAXBUFFER + LOOKAHEAD, input_bufsize);

    com_scan_tbl_init();
    rdoq_init_err_scale(input_init->bit_depth);
    rdoq_init_prob_2_bits();

    com_funs_init_ip_filter();
    com_funs_init_pixel_opt();
    com_funs_init_intra_pred();
    com_funs_init_transform();
    com_funs_init_quant();
    com_funs_init_rdcost();
    com_funs_init_deblock();
    com_funs_init_sao();

#if COMPILE_10BIT
    com_funs_init_intrinsic_functions_10bit();

    if (simd_avx_level(NULL) >= 2) {
        com_funs_init_intrinsic_functions_avx2_10bit();
        com_funs_init_intrinsic_functions_avx512_10bit();
    }
#else
    #if defined(__ANDROID__)
    #elif defined(__APPLE__)
        #if defined(__arm64__)
        #elif defined(__ARM_NEON__)
            com_funs_init_arm32();
        #else
            com_funs_init_intrinsic_functions();
    
            if (simd_avx_level(NULL) >= 2) {
                com_funs_init_intrinsic_functions_avx2();
            }
        #endif
    #else
        com_funs_init_intrinsic_functions();

        if (simd_avx_level(NULL) >= 2) {
            com_funs_init_intrinsic_functions_avx2();
        }
    #endif
#endif

    ctrl = com_malloc(sizeof(avs3_ctrl_t), 1);
    total_memory = sizeof(avs3_ctrl_t);

    memcpy(&ctrl->input, input_init, sizeof(cfg_param_t));

    input = &ctrl->input;

    ctrl->priv_data = priv_data;
    ctrl->func_out_strm = strm_callbak;
    ctrl->func_out_rec = rec_callbak;
    ctrl->last_idr_poc = 0;
    ctrl->last_idr_coi = 0;
    ctrl->curr_idr_poc = 0;
    ctrl->curr_idr_coi = 0;
    ctrl->coi = -1;
    ctrl->last_ip_pts = -1;
    ctrl->frm_list_size = input_bufsize + REF_MAXBUFFER;
    ctrl->frm_encoder = (avs3_enc_t*)com_malloc(input->threads_frm * sizeof(avs3_enc_t), 1);
    ctrl->frm_idx = 0;
    ctrl->act_frm_thds = 0;
    ctrl->exit_flag = 0;

    avs3_pthread_mutex_init(&ctrl->list_mutex, NULL);

    ctrl->all_frm_list = com_malloc(ctrl->frm_list_size * sizeof(frame_t), 1);
    total_memory += ctrl->frm_list_size * sizeof(frame_t);

    for (i = 0; i < ctrl->frm_list_size; i++) {
        total_memory += frame_create(&ctrl->all_frm_list[i], input);
    }

    total_memory += imglist_create(ctrl, input, input_bufsize);

    for (i = 0; i < input->threads_frm; i++) {
        total_memory += avs3_core_pic_create(&ctrl->frm_encoder[i], &ctrl->input);
    }

    ctrl->frm_threadpool = avs3_threadpool_init(input->threads_frm, input->threads_frm, NULL, NULL);
    ctrl->ctl_threadpool = avs3_threadpool_init( 2,  2, NULL, NULL);
    ctrl->pre_threadpool = avs3_threadpool_init(min(16, input->pic_height_in_lcu), input->pic_height_in_lcu, NULL, NULL);

    avs3_threadpool_run(ctrl->ctl_threadpool, encoding_thread, ctrl, 1);
    avs3_threadpool_run(ctrl->ctl_threadpool, lookahead_thread, ctrl, 1);

    if (input->rate_ctrl) {
        avs3_lib_set_rc(ctrl, NULL, NULL, NULL, NULL);
    }

    ctrl->lookahead.last_p_cpp_list     = com_malloc(sizeof(double) * input->pic_height_in_lcu, 0);
    ctrl->lookahead.last_b_fwd_cpp_list = com_malloc(sizeof(double) * input->pic_height_in_lcu, 0);
    ctrl->lookahead.last_b_bwd_cpp_list = com_malloc(sizeof(double) * input->pic_height_in_lcu, 0);
    ctrl->lookahead.param_list          = com_malloc(sizeof(pre_cal_PB_param_t) * input->pic_height_in_lcu, 0);

    image_create_org(&ctrl->lookahead.pre_proc_last_img, input->img_width, input->img_height);
    me_module_init(&ctrl->lookahead.pre_proc_me);

    printf("Version: %s_%s,  SHA-1: %s\n", VERSION_STR, VERSION_TYPE, VERSION_SHA1);
    printf("Total Memory: %.2f\n", total_memory/1024/1024);

    return ctrl;
}

void avs3_lib_free(void *handle)
{
    int i;
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    const cfg_param_t *input = &ctrl->input;

    while (ctrl->ctl_threadpool->uninit.i_size != 2) {
        avs3_threadpool_wait(ctrl->ctl_threadpool, ctrl);
    }

    for (i = 0; i < input->threads_frm; i++) {
        avs3_core_pic_free(&ctrl->frm_encoder[i]);
    }

    com_free(ctrl->frm_encoder);

    imglist_destroy(ctrl);

    avs3_threadpool_delete(ctrl->ctl_threadpool);
    avs3_threadpool_delete(ctrl->frm_threadpool);
    avs3_threadpool_delete(ctrl->pre_threadpool);

    for (i = 0; i < ctrl->frm_list_size; i++) {
        frame_destroy(&ctrl->all_frm_list[i]);
    }

    com_free(ctrl->all_frm_list);

    avs3_pthread_mutex_destroy(&ctrl->list_mutex);

    if (ctrl->rc_handle) {
        ctrl->rc_destroy(ctrl->rc_handle);
        ctrl->rc_handle = NULL;
    }

    com_free(ctrl->lookahead.last_p_cpp_list);
    com_free(ctrl->lookahead.last_b_fwd_cpp_list);
    com_free(ctrl->lookahead.last_b_bwd_cpp_list);
    com_free(ctrl->lookahead.param_list);

    avs3_me_free(&ctrl->lookahead.pre_proc_me);
    image_free(&ctrl->lookahead.pre_proc_last_img);

    com_scan_tbl_delete();

    com_free(ctrl);
}

void avs3_lib_reset(void *handle, cfg_param_t *input, void *priv_data)
{
    int i;
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    frame_t *frm_list = ctrl->all_frm_list;

    while (ctrl->ctl_threadpool->uninit.i_size != 2) {
        avs3_threadpool_wait(ctrl->ctl_threadpool, ctrl);
    }

    ctrl->input.target_bitrate = input->target_bitrate;

    for (i = 0; i < ctrl->frm_list_size; i++) {
        frm_list[i].coi = -1;
        frm_list[i].ref_cnt = 0;
        frm_list[i].b_ref = 0;
    }

    ctrl->priv_data         = priv_data;
    ctrl->frm_idx           = 0;
    ctrl->act_frm_thds      = 0;
    ctrl->write_end_code    = 0;
    ctrl->last_idr_poc      = 0;
    ctrl->last_idr_coi      = 0;
    ctrl->curr_idr_poc      = 0;
    ctrl->curr_idr_coi      = 0;
    ctrl->exit_flag         = 0;
    ctrl->coi               = -1;
    ctrl->last_ip_pts       = -1;

    if (ctrl->rc_handle) {
        ctrl->rc_destroy(ctrl->rc_handle);
        ctrl->rc_handle = ctrl->rc_init(&ctrl->input);
    }

    if (ctrl->ctl_threadpool->uninit.i_size == 2) {
        avs3_threadpool_run(ctrl->ctl_threadpool, encoding_thread, ctrl, 1);
		avs3_threadpool_run(ctrl->ctl_threadpool, lookahead_thread, ctrl, 1);
    }
}

image_t *avs3_lib_imgbuf(void *handle)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;

    if (ctrl->img_next == NULL) {
        ctrl->img_next = xl_remove_head(&ctrl->imglist_idle, 1);
    }
    return ctrl->img_next;
}

int frame_type_decision(const cfg_param_t *input, int bflush, int ip_frm_num, int force_p)
{
    int type;

    if (input->intra_period == 0) {
        type = (ip_frm_num == 0 ? I_FRM : P_FRM);
    } else if (input->intra_period == 1) {
        type = I_FRM;
    } else {
        if (ip_frm_num == 0 || (ip_frm_num % input->intra_period == 0 && !bflush)) {
            type = I_FRM;
        } else {
            type = P_FRM;
        }
    }

    if (force_p) {
        type = P_FRM;
    }

    return type;
}

void avs3_lib_encode(void *handle, int bflush, int output_seq_end)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    image_t *img = avs3_lib_imgbuf(ctrl);

    if (bflush == 2) {
        ctrl->exit_flag = 1;
    }

    if (bflush) {
        img->flush_type = output_seq_end ? 2 : 1;
    } else {
        img->flush_type = 0;
    }

    xl_append(&ctrl->imglist_read, img);
    ctrl->img_next = NULL;
}

void lookahead_process_one_frame(avs3_ctrl_t *ctrl, avs3_lookahead_t *lookahead, image_t *img)
{
    const cfg_param_t *input = &ctrl->input;
    int RPS_idx;
    int i;
    int b_IFrm = (img->type & I_FRM);
    ref_man curr_RPS;
    frame_t *p_curr_frm;

    if (img->flush_type) {
        frame_t *frm_list;

        if (lookahead->fst_ifrm) {
            xl_append(&ctrl->frmlist_ready, lookahead->fst_ifrm);
            lookahead->fst_ifrm->img_org->inter_cpp = 7.0;
            lookahead->fst_ifrm = NULL;
        }

        /* look for a free frm */
        frm_list = ctrl->all_frm_list;

        avs3_pthread_mutex_lock(&ctrl->list_mutex);
        for (i = 0; i < ctrl->frm_list_size; i++) {
            if (frm_list[i].ref_cnt == 0 && frm_list[i].b_ref == 0) {
                break;
            }
        }
        avs3_pthread_mutex_unlock(&ctrl->list_mutex);

        p_curr_frm = &frm_list[i];
        p_curr_frm->flush_type = img->flush_type;
        xl_append(&ctrl->frmlist_ready, p_curr_frm);
        xl_append(&ctrl->imglist_idle, img);
        return;
    }

    ctrl->coi++;

    RPS_idx = ((ctrl->coi - 1) % input->gop_size);

    if (b_IFrm) {
        ctrl->curr_idr_poc = img->poc;
        ctrl->curr_idr_coi = ctrl->coi;
        curr_RPS.layer = 0;
        curr_RPS.num_of_ref = 0;
        curr_RPS.referd_by_others = 1;
        curr_RPS.num_to_remove = 0;
    } else {
        curr_RPS = input->seq_ref_cfg[RPS_idx];
    }

    avs3_pthread_mutex_lock(&ctrl->list_mutex);
    p_curr_frm = prepare_RefInfo(ctrl, &curr_RPS, img->type, img->poc);
    avs3_pthread_mutex_unlock(&ctrl->list_mutex);

    p_curr_frm->img_org = img;
    p_curr_frm->flush_type = 0;
    p_curr_frm->curr_RPS = curr_RPS;
    p_curr_frm->curr_RPS.qp_offset = input->seq_ref_cfg[RPS_idx + img->bfrm_coi_offset].qp_offset;

    if (input->rate_ctrl) {
        double intra_cost;
        if (img->type == I_FRM) {
            img->intra_cpp = pre_cal_I_cost(input, img->plane[0], img->i_stride[0], input->img_width, input->img_height, input->bit_depth, img->intra_cpp_list);
            img->inter_cpp = lookahead->last_p_cpp;
        } else if (img->type != B_FRM){
            lookahead->last_p_cpp = img->inter_cpp = pre_cal_PB_cost(input, lookahead->param_list, p_curr_frm->ref_list[0]->img_org, img, 32, &lookahead->pre_proc_me, lookahead->last_p_cpp_list, &intra_cost, ctrl->pre_threadpool);
            img->intra_cpp = intra_cost;
            memcpy(img->inter_cpp_list, lookahead->last_p_cpp_list, sizeof(double) * input->pic_height_in_lcu);
        } else {
            double fwd_cpp = pre_cal_PB_cost(input, lookahead->param_list, p_curr_frm->ref_list[1]->img_org, img, 32, &lookahead->pre_proc_me, lookahead->last_b_fwd_cpp_list, &intra_cost, ctrl->pre_threadpool);
            double bwd_cpp = pre_cal_PB_cost(input, lookahead->param_list, p_curr_frm->ref_list[0]->img_org, img, 32, &lookahead->pre_proc_me, lookahead->last_b_bwd_cpp_list, &intra_cost, ctrl->pre_threadpool);

            if (fwd_cpp < bwd_cpp) {
                memcpy(img->inter_cpp_list, lookahead->last_b_fwd_cpp_list, sizeof(double) * input->pic_height_in_lcu);
                img->inter_cpp = fwd_cpp;
            } else {
                memcpy(img->inter_cpp_list, lookahead->last_b_bwd_cpp_list, sizeof(double) * input->pic_height_in_lcu);
                img->inter_cpp = bwd_cpp;
            }
            img->intra_cpp = intra_cost;
        }
    }

    if (lookahead->total_frames == 0) {
        lookahead->fst_ifrm = p_curr_frm;
    } else {
        if (lookahead->total_frames == 1) {
            memcpy(lookahead->fst_ifrm->img_org->inter_cpp_list, lookahead->last_p_cpp_list, sizeof(double) * input->pic_height_in_lcu);
            lookahead->fst_ifrm->img_org->inter_cpp = lookahead->last_p_cpp;
            xl_append(&ctrl->frmlist_ready, lookahead->fst_ifrm);
            lookahead->fst_ifrm = NULL;
        }
        xl_append(&ctrl->frmlist_ready, p_curr_frm);
    }

    lookahead->total_frames++;
}

void* lookahead_thread(void *handle)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    const cfg_param_t *input = &ctrl->input;
    int i, gop_size = input->intra_period * input->gop_size;
    i64s_t poc = 0;
    image_t *img_bfrm_buf[MAXGOP];
    int bfrms_in_buf = 0;
    int bfrm_coi_offset = 0;
    int ip_num = 0;
    avs3_lookahead_t *lookahead = &ctrl->lookahead;
    int scene_cut = 0;
    i64s_t scene_cut_poc;

    lookahead->last_p_cpp = -1.0;
    lookahead->total_frames = 0;
    lookahead->last_inter_sad = -1.0;
    lookahead->fst_ifrm = NULL;

    while (!ctrl->exit_flag) {
        image_t *input_img = xl_remove_head(&ctrl->imglist_read, 1);
        int bflush = input_img->flush_type;
        int force_p = input->close_gop && ip_num && (ip_num % input->intra_period == 0);

        input_img->poc = poc++;

        if (input->rate_ctrl && !bflush) {
            if (lookahead->total_frames) {
                double inter_sad = pre_cal_P_SAD(input, &lookahead->pre_proc_last_img, input_img);
                if (inter_sad < 0.1) {
                    input_img->static_frm = 1;
                } else {
                    input_img->static_frm = 0;
                }
                if (lookahead->last_inter_sad < 0) {
                    lookahead->last_inter_sad = inter_sad;
                } else {
                    double last_inter_sad = lookahead->last_inter_sad;
                    if (inter_sad > 5 && (inter_sad / last_inter_sad > 2.5 || last_inter_sad / inter_sad > 2.5)) {
                        scene_cut = 1;
                        scene_cut_poc = input_img->poc;
                        lookahead->last_inter_sad = -1.0;
                    } else {
                        lookahead->last_inter_sad = 0.5 * lookahead->last_inter_sad + 0.5 * inter_sad;
                    }
                }
            }
            image_copy(&lookahead->pre_proc_last_img, input_img);
        }

        if (!bflush && ip_num && bfrms_in_buf < input->succ_bfrms && (!force_p || bfrms_in_buf < input->succ_bfrms - 1)) {
            img_bfrm_buf[bfrms_in_buf++] = input_img;
        } else if (!bflush || bfrms_in_buf) {
            image_t *output_img;
            i64s_t curr_ip_pts;
            int delay_frms;

            if (bflush) {
                output_img = img_bfrm_buf[--bfrms_in_buf];
            } else {
                output_img = input_img;
            }

            curr_ip_pts = output_img->pts;
            delay_frms = bfrms_in_buf + 1;

            output_img->type = frame_type_decision(input, bflush, ip_num, force_p);
            output_img->bfrm_coi_offset = bfrm_coi_offset;
            output_img->dts = (ctrl->last_ip_pts == -1 ? output_img->pts : ctrl->last_ip_pts + (curr_ip_pts - ctrl->last_ip_pts) / delay_frms);
            output_img->scene_cut = scene_cut;

            ctrl->last_dts = output_img->dts;

            lookahead_process_one_frame(ctrl, lookahead, output_img);

            for (i = 1; i <= bfrms_in_buf; i++) {
                int offset = i + bfrm_coi_offset;
                int idx_in_subgop = input->seq_ref_cfg[offset].poc;

                while (idx_in_subgop > bfrms_in_buf) {
                    bfrm_coi_offset++;
                    offset++;
                    idx_in_subgop = input->seq_ref_cfg[offset].poc;
                }

                output_img = img_bfrm_buf[input->seq_ref_cfg[offset].poc - 1];

                output_img->type = B_FRM;
                output_img->bfrm_coi_offset = bfrm_coi_offset;
                output_img->dts = ctrl->last_ip_pts + (i + 1) * (curr_ip_pts - ctrl->last_ip_pts) / delay_frms;
                output_img->scene_cut = scene_cut ? (output_img->poc < scene_cut_poc) : 0;

                ctrl->last_dts = output_img->dts;

                lookahead_process_one_frame(ctrl, lookahead, output_img);
            }

            ctrl->last_ip_pts = curr_ip_pts;

            scene_cut = 0;
            bfrms_in_buf = 0;
            ip_num++;

            if (force_p) {
                ip_num = 0;
            }
        }

        if (bflush) {
            lookahead_process_one_frame(ctrl, lookahead, input_img);
            break;
        }
    }

    return NULL;
}

void* encoding_thread(void *handle)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    const cfg_param_t *input = &ctrl->input;
    frame_t *frame_buffer[MAXGOP];
    int i, frames = 0;
    double frame_buffer_cpp[MAXGOP];

    while (!ctrl->exit_flag) {
        frame_t *frm = xl_remove_head(&ctrl->frmlist_ready, 1);
        
        if (frames && (frm->flush_type || frm->img_org->type != B_FRM)) {
            for (i = 0; i < frames; i++) {
                encode_one_frame(ctrl, frame_buffer[i], frame_buffer_cpp, frames);
                ctrl->write_end_code = 1;
            } 
            frames = 0;
        }

        if (frm->flush_type) {
            if (ctrl->act_frm_thds) {
                flush_frames(ctrl);
                if (frm->flush_type == 2) {
                    static uchar_t end_code[4] = { 0x00, 0x00, 0x01, 0xB1 };
                    if (ctrl->func_out_strm) {
                        ctrl->func_out_strm(ctrl->priv_data, end_code, 4, -1, ctrl->last_dts, SEQ_END);
                    }
                }
            }
            break;
        } else {
            frame_buffer_cpp[frames] = frm->img_org->inter_cpp;
            frame_buffer    [frames] = frm;
            frames++;
        }
    }
    return NULL;
}

void avs3_lib_set_priority(void *handle, int priority)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    const cfg_param_t* input = &ctrl->input;
    int i;

    avs3_threadpool_set_priority(ctrl->frm_threadpool, priority);
    avs3_threadpool_set_priority(ctrl->ctl_threadpool, priority);
    avs3_threadpool_set_priority(ctrl->pre_threadpool, priority);

    for (i = 0; i < input->threads_frm; i++) {
        avs3_threadpool_set_priority(ctrl->frm_encoder[i].wpp_threadpool , priority);
        avs3_threadpool_set_priority(ctrl->frm_encoder[i].post_threadpool, priority);
    }
}

void avs3_lib_set_affinity(void *handle, void *mask)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    const cfg_param_t* input = &ctrl->input;
    int i;

    avs3_threadpool_set_affinity(ctrl->frm_threadpool, mask);
    avs3_threadpool_set_affinity(ctrl->ctl_threadpool, mask);
    avs3_threadpool_set_affinity(ctrl->pre_threadpool, mask);

    for (i = 0; i < input->threads_frm; i++) {
        avs3_threadpool_set_affinity(ctrl->frm_encoder[i].wpp_threadpool, mask);
        avs3_threadpool_set_affinity(ctrl->frm_encoder[i].post_threadpool, mask);
    }
}

void  avs3_lib_set_rc(void *handle, rc_frame_init_t init, rc_frame_get_qp_t get_qp, rc_frame_update_t update, rc_frame_destroy_t free)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    cfg_param_t * input = &ctrl->input;

    if (input->rate_ctrl == 0) {
        return;
    }

	if (ctrl->rc_handle) {
		ctrl->rc_destroy(ctrl->rc_handle);
		ctrl->rc_handle = NULL;
	}

    if (1 || init == NULL || get_qp == NULL || update == NULL || free == NULL) {
        if (input->rate_ctrl == 1) {
            ctrl->rc_init = rc_cbr_init;
            ctrl->rc_get_qp = rc_cbr_get_qp;
            ctrl->rc_update = rc_cbr_update;
            ctrl->rc_destroy = rc_cbr_destroy;
        } else if (input->rate_ctrl == 2) {
            ctrl->rc_init = rc_abr_init;
            ctrl->rc_get_qp = rc_abr_get_qp;
            ctrl->rc_update = rc_abr_update;
            ctrl->rc_destroy = rc_abr_destroy;
        } else {
            ctrl->rc_init = rc_crf_init;
            ctrl->rc_get_qp = rc_crf_get_qp;
            ctrl->rc_update = rc_crf_update;
            ctrl->rc_destroy = rc_crf_destroy;
        }
    } else {
        ctrl->rc_init = init;
        ctrl->rc_get_qp = get_qp;
        ctrl->rc_update = update;
        ctrl->rc_destroy = free;
    }

    ctrl->rc_handle = ctrl->rc_init(&ctrl->input);
}

void  avs3_lib_speed_adjust(void *handle, double adj)
{
    avs3_ctrl_t *ctrl = (avs3_ctrl_t*)handle;
    cfg_param_t * input = &ctrl->input;

    input->speed_adj_rate = Clip3(0.5f, 5.0f, adj);
}

void avs3_find_psnr(cfg_param_t* input, image_t *org_img, image_t *rec_img, double *snr_y, double *snr_u, double *snr_v)
{
    i64s_t diff_y, diff_u, diff_v;
    int impix;
    int bit_depth = input->bit_depth;
    double maxSignal = (double)((1 << bit_depth) - 1) * (double)((1 << bit_depth) - 1);
    pel_t *rec, *recu, *recv;
    int i_rec, i_rec_uv;
    pel_t *org, *orgu, *orgv;
    int i_org, i_org_uv;
    int width = org_img->width;
    int height = org_img->height;

    impix = width * height;

    diff_y = diff_u = diff_v = 0;

    i_rec = rec_img->i_stride[0];
    i_rec_uv = rec_img->i_stride[1];
    i_org = org_img->i_stride[0];
    i_org_uv = org_img->i_stride[1];

    rec = rec_img->plane[0];
    recu = rec_img->plane[1];
    recv = rec_img->plane[2];

    org = org_img->plane[0];
    orgu = org_img->plane[1];
    orgv = org_img->plane[2];

    diff_y = g_funs_handle.cost_ssd_psnr(org, i_org, rec, i_rec, width, height);
    diff_u = g_funs_handle.cost_ssd_psnr(orgu, i_org_uv, recu, i_rec_uv, width / 2, height / 2);
    diff_v = g_funs_handle.cost_ssd_psnr(orgv, i_org_uv, recv, i_rec_uv, width / 2, height / 2);

    *snr_y = diff_y ? (10.0 * log10(maxSignal * (double)impix / (double)diff_y)) : 99.99f;
    *snr_u = diff_u ? (10.0 * log10(maxSignal * (double)impix / (double)(4 * diff_u))) : 99.99f;
    *snr_v = diff_v ? (10.0 * log10(maxSignal * (double)impix / (double)(4 * diff_v))) : 99.99f;
}

void avs3_find_ssim(cfg_param_t* input, image_t *org_img, image_t *rec_img, double *ssim_y, double *ssim_u, double *ssim_v)
{
    int impix;
    int bit_depth = input->bit_depth;
    double maxSignal = (double)((1 << bit_depth) - 1) * (double)((1 << bit_depth) - 1);
    pel_t *rec, *recu, *recv;
    int i_rec, i_rec_uv;
    pel_t *org, *orgu, *orgv;
    int i_org, i_org_uv;
    int width = org_img->width;
    int height = org_img->height;
    float ssim_all;
    int pixel_cnt;

    impix = width * height;

    i_rec = rec_img->i_stride[0];
    i_rec_uv = rec_img->i_stride[1];
    i_org = org_img->i_stride[0];
    i_org_uv = org_img->i_stride[1];

    rec = rec_img->plane[0];
    recu = rec_img->plane[1];
    recv = rec_img->plane[2];

    org = org_img->plane[0];
    orgu = org_img->plane[1];
    orgv = org_img->plane[2];

    ssim_all = avs3_pixel_ssim_wxh(org + 2, i_org, rec + 2, i_rec, width - 2, height, &pixel_cnt);
    *ssim_y = ssim_all / pixel_cnt;

    ssim_all = avs3_pixel_ssim_wxh(orgu + 2, i_org_uv, recu + 2, i_rec_uv, width / 2 - 2, height / 2, &pixel_cnt);
    *ssim_u = ssim_all / pixel_cnt;

    ssim_all = avs3_pixel_ssim_wxh(orgv + 2, i_org_uv, recv + 2, i_rec_uv, width / 2 - 2, height / 2, &pixel_cnt);
    *ssim_v = ssim_all / pixel_cnt;
}
