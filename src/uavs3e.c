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

#include <math.h>

#include "define.h"
#include "analyze.h"
#include "com_util.h"

#include "../version.h"

static core_t *core_alloc(com_info_t *info)
{
    core_t *core;
    int i, j;
    core = (core_t *)com_malloc(sizeof(core_t));
    com_assert_rv(core, NULL);
    com_mset_x64a(core, 0, sizeof(core_t));

    for (i = 0; i < MAX_CU_DEPTH; i++) {
        for (j = 0; j < MAX_CU_DEPTH; j++) {
            enc_create_cu_data(&core->cu_data_best[i][j], i, j);
            enc_create_cu_data(&core->cu_data_temp[i][j], i, j);
        }
    }

    inter_search_init(&core->pinter, info, 1);

    int l_size = sizeof(pel) * ((MAX_CU_SIZE + SAO_SHIFT_PIX_NUM + 2) * (MAX_CU_SIZE + SAO_SHIFT_PIX_NUM + 2) + info->pic_width + ALIGN_MASK);
    int c_size = sizeof(pel) * ((MAX_CU_SIZE / 2 + SAO_SHIFT_PIX_NUM + 2) * (MAX_CU_SIZE / 2 + SAO_SHIFT_PIX_NUM + 2) + info->pic_width / 2 + ALIGN_MASK);

    core->sao_src_buf[0] = com_malloc(l_size);
    com_assert_rv(core->sao_src_buf[0], NULL);
    core->sao_src_buf[1] = com_malloc(c_size);
    com_assert_rv(core->sao_src_buf[1], NULL);
    core->sao_src_buf[2] = com_malloc(c_size);
    com_assert_rv(core->sao_src_buf[2], NULL);

    return core;
}

static void core_free(core_t *core)
{
    int i, j;
    for (i = 0; i < MAX_CU_DEPTH; i++) {
        for (j = 0; j < MAX_CU_DEPTH; j++) {
            enc_delete_cu_data(&core->cu_data_best[i][j]);
            enc_delete_cu_data(&core->cu_data_temp[i][j]);
        }
    }

    com_mfree(core->sao_src_buf[0]);
    com_mfree(core->sao_src_buf[1]);
    com_mfree(core->sao_src_buf[2]);

    com_mfree(core);
}

static enc_pic_t *pic_enc_alloc(com_info_t *info)
{
    enc_pic_t *ep;
    u8 *buf;
    int ret;
    int dqp_bufsize = (info->pic_width / UNIT_SIZE) * (info->pic_height / UNIT_SIZE);
    int linebuf_intra_size = ALIGN_MASK * 5 + sizeof(pel) * ((info->pic_width + MAX_CU_SIZE) * 2 + 3);

    int total_mem_size =
        ALIGN_MASK * 1 + sizeof(enc_pic_t) +
        ALIGN_MASK * 1 + sizeof( enc_cu_t) * info->f_lcu +                                                      // map_cu_data
        ALIGN_MASK * 1 + sizeof(       s8) * info->f_lcu * info->cus_in_lcu * MAX_CU_DEPTH * NUM_BLOCK_SHAPE +  // map.map_split
        ALIGN_MASK * 1 + sizeof(       s8) * info->f_lcu +                                                      // map.map_qp
        ALIGN_MASK * 1 + sizeof(com_scu_t) * info->f_scu +                                                      // map.map_scu
        ALIGN_MASK * 1 + sizeof(       u8) * info->f_scu +                                                      // map.map_edge
        ALIGN_MASK * 1 + sizeof(       s8) * info->f_scu +                                                      // map.map_ipm
        ALIGN_MASK * 1 + sizeof(       s8) * info->f_scu +                                                      // map.map_patch
        ALIGN_MASK * 1 + sizeof(      u32) * info->f_scu +                                                      // map.map_pos
        ALIGN_MASK * 1 + sizeof(    float) * dqp_bufsize +                                                     // map.map_dqp
        ALIGN_MASK * 1 + (linebuf_intra_size + sizeof(pel *) * 3) * info->pic_height_in_lcu +                   // linebuf_intra
        ALIGN_MASK * 3 + sizeof(pel) * ((info->pic_width + MAX_CU_SIZE) * 2 + 3) +                              // linebuf_sao
        ALIGN_MASK * 1 + sizeof(             u8) * info->f_lcu +                                                // alf_var_map
        ALIGN_MASK * 1 + sizeof( enc_alf_corr_t) * info->f_lcu * N_C +                                          // Enc_ALF.m_alfCorr
        ALIGN_MASK * 1 + sizeof(           BOOL) * info->f_lcu * N_C +                                          // Enc_ALF.m_AlfLCUEnabled
        ALIGN_MASK * 1 + sizeof(com_sao_param_t) * info->f_lcu * N_C +                                          // sao_blk_params 
        ALIGN_MASK * 1 + sizeof(enc_lcu_row_t) * info->pic_height_in_lcu +                                      // array_row
        ALIGN_MASK * 2 + MAX_BS_BUF * 2 +                                                                       // bs_buf + bs_buf_demulate
        ALIGN_MASK * 3 + sizeof(s16) * (info->pic_height + 2 * 8) * (info->pic_width + 2 * 8) * 3 +             // ip_tmp_buf
        ALIGN_MASK;

    buf = com_malloc(total_mem_size);

    GIVE_BUFFER(ep, buf, sizeof(enc_pic_t));
    memcpy(&ep->info, info, sizeof(com_info_t));

    GIVE_BUFFER(ep->map_cu_data    , buf, sizeof(enc_cu_t ) * info->f_lcu);
    GIVE_BUFFER(ep->map.map_split  , buf, sizeof(s8       ) * info->f_lcu * info->cus_in_lcu * MAX_CU_DEPTH * NUM_BLOCK_SHAPE);
    GIVE_BUFFER(ep->map.map_qp     , buf, sizeof(s8       ) * info->f_lcu);
    GIVE_BUFFER(ep->map.map_dqp    , buf, sizeof(float    ) * dqp_bufsize);
    GIVE_BUFFER(ep->map.map_scu    , buf, sizeof(com_scu_t) * info->f_scu); ep->map.map_scu   += info->i_scu + 1;
    GIVE_BUFFER(ep->map.map_edge   , buf, sizeof(u8       ) * info->f_scu); ep->map.map_edge  += info->i_scu + 1;
    GIVE_BUFFER(ep->map.map_ipm    , buf, sizeof(s8       ) * info->f_scu); ep->map.map_ipm   += info->i_scu + 1;
    GIVE_BUFFER(ep->map.map_patch  , buf, sizeof(s8       ) * info->f_scu); ep->map.map_patch += info->i_scu + 1;
    GIVE_BUFFER(ep->map.map_pos    , buf, sizeof(u32      ) * info->f_scu); ep->map.map_pos   += info->i_scu + 1;
    GIVE_BUFFER(ep->linebuf_intra  , buf, sizeof(pel     *) * 3 * info->pic_height_in_lcu);

    for (int i = 0; i < info->pic_height_in_lcu; i++) {
        GIVE_BUFFER(ep->linebuf_intra[i][0], buf, sizeof(pel) *(info->pic_width + MAX_CU_SIZE + 1));
        ep->linebuf_intra[i][0] += 1;
        GIVE_BUFFER(ep->linebuf_intra[i][1], buf, sizeof(pel) *(info->pic_width / 2 + MAX_CU_SIZE / 2 + 1));
        ep->linebuf_intra[i][1] += 1;
        GIVE_BUFFER(ep->linebuf_intra[i][2], buf, sizeof(pel) *(info->pic_width / 2 + MAX_CU_SIZE / 2 + 1));
        ep->linebuf_intra[i][2] += 1;
    }
    GIVE_BUFFER(ep->linebuf_sao[0], buf, sizeof(pel) *(info->pic_width + MAX_CU_SIZE + 1));
    ep->linebuf_sao[0] += 1;
    GIVE_BUFFER(ep->linebuf_sao[1], buf, sizeof(pel) *(info->pic_width / 2 + MAX_CU_SIZE / 2 + 1));
    ep->linebuf_sao[1] += 1;
    GIVE_BUFFER(ep->linebuf_sao[2], buf, sizeof(pel) *(info->pic_width / 2 + MAX_CU_SIZE / 2 + 1));
    ep->linebuf_sao[2] += 1;

    GIVE_BUFFER(ep->alf_var_map, buf, sizeof(s8) * info->f_lcu);
    com_alf_buf_init(info, ep->alf_var_map);
    ep->Enc_ALF.m_coeffNoFilter[ALF_MAX_NUM_COEF - 1] = (1 << ALF_NUM_BIT_SHIFT);

    GIVE_BUFFER(ep->Enc_ALF.m_alfCorr,       buf, sizeof( enc_alf_corr_t) * info->f_lcu * N_C);
    GIVE_BUFFER(ep->Enc_ALF.m_AlfLCUEnabled, buf, sizeof(           BOOL) * info->f_lcu * N_C);
    GIVE_BUFFER(ep->sao_blk_params,          buf, sizeof(com_sao_param_t) * info->f_lcu * N_C);
    GIVE_BUFFER(ep->array_row,               buf, sizeof(enc_lcu_row_t  ) * info->pic_height_in_lcu);
    GIVE_BUFFER(ep->bs_buf_demulate,         buf, MAX_BS_BUF);

    GIVE_BUFFER(ep->ip_tmp_buf[0],           buf, sizeof(s16) * (info->pic_width + 2 * 8) * (info->pic_height + 2 * 8));
    GIVE_BUFFER(ep->ip_tmp_buf[1],           buf, sizeof(s16) * (info->pic_width + 2 * 8) * (info->pic_height + 2 * 8));
    GIVE_BUFFER(ep->ip_tmp_buf[2],           buf, sizeof(s16) * (info->pic_width + 2 * 8) * (info->pic_height + 2 * 8));

    for (int i = 0; i < info->pic_height_in_lcu; i++) {
        uavs3e_sem_init(&ep->array_row[i].sem, 0, 0);
    }

    uavs3e_threadpool_init(&ep->wpp_threads_pool, info->wpp_threads, info->pic_height_in_lcu, (void * (*)(void *))core_alloc, info, (void(*)(void *))core_free);

    for (int i = 0; i < info->f_lcu; i++) {
        enc_create_cu_data(ep->map_cu_data + i, info->log2_max_cuwh - MIN_CU_LOG2, info->log2_max_cuwh - MIN_CU_LOG2);
    }

    ep->pic_alf_Rec = com_pic_create(info->pic_width, info->pic_height, PIC_PAD_SIZE_L, PIC_PAD_SIZE_C, &ret);

    ep->main_core = core_alloc(info);

    inter_search_init(&ep->pinter, info, 1);

    return ep;
}

static void pic_enc_free(enc_pic_t *ep)
{
    uavs3e_threadpool_delete(ep->wpp_threads_pool);

    for (int i = 0; i < ep->info.f_lcu; i++) {
        enc_delete_cu_data(ep->map_cu_data + i);
    }
    for (int i = 0; i < ep->info.pic_height_in_lcu; i++) {
        uavs3e_sem_destroy(&ep->array_row[i].sem);
    }
    com_pic_destroy(ep->pic_alf_Rec);
    core_free(ep->main_core);

    com_mfree(ep);
}

static int refine_input_cfg(enc_cfg_t *param, enc_cfg_t *cfg_org)
{
    com_mcpy(param, cfg_org, sizeof(enc_cfg_t));

    if (param->bit_depth_internal == 0) {
        param->bit_depth_internal = param->bit_depth_input;
    }
    if (param->bit_depth_internal == 8) {
        param->qp = COM_CLIP3(0, MAX_QUANT_BASE, param->qp);
    } else {
        param->qp = COM_CLIP3(-16, MAX_QUANT_BASE, param->qp);
    }
    param->pic_width  = (param->horizontal_size + PIC_ALIGN_SIZE - 1) / PIC_ALIGN_SIZE * PIC_ALIGN_SIZE;
    param->pic_height = (param->vertical_size   + PIC_ALIGN_SIZE - 1) / PIC_ALIGN_SIZE * PIC_ALIGN_SIZE;

    if (param->horizontal_size % PIC_ALIGN_SIZE != 0) {
        printf("Note: picture width in encoding process is padded to %d (input value %d)\n", param->pic_width, param->horizontal_size);
    }
    if (param->vertical_size % PIC_ALIGN_SIZE != 0) {
        printf("Note: picture height in encoding process is padded to %d (input value %d)\n", param->pic_height, param->vertical_size);
    }
    if (param->bit_depth_input > param->bit_depth_internal) {
        printf("Warning: Data precision may lose because input bit depth is higher than internal one !\n");
    }
    param->wpp_threads = COM_CLIP3(1, (param->pic_height + param->ctu_size - 1) / param->ctu_size, param->wpp_threads);
    param->frm_threads = COM_MAX(param->frm_threads, 1);

    if (param->i_period == 1) {
        param->max_b_frames = 0;
        param->close_gop = 1;
    }
    if (param->max_b_frames == 0) {
        param->adaptive_gop = 0;
    }
    param->lookahead = COM_MAX(param->lookahead, param->max_b_frames + 1);

    /* check input parameters */
    com_assert_rv (param->pic_width > 0 && param->pic_height > 0, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv((param->pic_width & (MIN_CU_SIZE - 1)) == 0, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv((param->pic_height & (MIN_CU_SIZE - 1)) == 0, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->qp >= ( - (8 *(param->bit_depth_internal - 8))), COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->qp <= MAX_QUANT_BASE, COM_ERR_INVALID_ARGUMENT); // this assertion is align with the constraint for input QP
    com_assert_rv(param->i_period >= 0, COM_ERR_INVALID_ARGUMENT);

    com_assert_rv(param->chroma_format == 1, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->patch_width == 0 && param->patch_height == 0, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->bit_depth_input == 8 || param->bit_depth_input == 10, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->bit_depth_internal == 8 || param->bit_depth_internal == 10, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->emvr_enable == 0 || (param->amvr_enable && param->num_of_hmvp), COM_ERR_INVALID_ARGUMENT);

    com_assert_rv(param->ctu_size == 32 || param->ctu_size == 64 || param->ctu_size == 128, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->min_cu_size == 4, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->max_part_ratio == 8, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->max_split_times == 6, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->min_qt_size == 4 || param->min_qt_size == 8 || param->min_qt_size == 16 || param->min_qt_size == 32 || param->min_qt_size == 64 || param->min_qt_size == 128, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->max_bt_size == 64 || param->max_bt_size == 128, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->max_eqt_size == 8 || param->max_eqt_size == 16 || param->max_eqt_size == 32 || param->max_eqt_size == 64, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->max_dt_size == 16 || param->max_dt_size == 32 || param->max_dt_size == 64, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->min_cu_size <= param->ctu_size, COM_ERR_INVALID_ARGUMENT);

    com_assert_rv(com_tbl_log2[param->max_b_frames + 1] != -1, COM_ERR_INVALID_ARGUMENT);
    com_assert_rv(param->max_b_frames < MAX_SUBGOP_SIZE, COM_ERR_INVALID_ARGUMENT);

#if (BIT_DEPTH == 8)
    if (param->bit_depth_internal == 10) {
        com_assert_rv(0, COM_ERR_INVALID_ARGUMENT);
    }
#endif
    return COM_OK;
}

int load_wq_matrix(char *fn, u8 *m4x4_out, u8 *m8x8_out)
{
    int i, m4x4[16], m8x8[64];
    sscanf(fn, "[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",
           &m4x4[ 0], &m4x4[ 1], &m4x4[ 2], &m4x4[ 3], &m4x4[ 4], &m4x4[ 5], &m4x4[ 6], &m4x4[ 7],
           &m4x4[ 8], &m4x4[ 9], &m4x4[10], &m4x4[11], &m4x4[12], &m4x4[13], &m4x4[14], &m4x4[15],
           &m8x8[ 0], &m8x8[ 1], &m8x8[ 2], &m8x8[ 3], &m8x8[ 4], &m8x8[ 5], &m8x8[ 6], &m8x8[ 7],
           &m8x8[ 8], &m8x8[ 9], &m8x8[10], &m8x8[11], &m8x8[12], &m8x8[13], &m8x8[14], &m8x8[15],
           &m8x8[16], &m8x8[17], &m8x8[18], &m8x8[19], &m8x8[20], &m8x8[21], &m8x8[22], &m8x8[23],
           &m8x8[24], &m8x8[25], &m8x8[26], &m8x8[27], &m8x8[28], &m8x8[29], &m8x8[30], &m8x8[31],
           &m8x8[32], &m8x8[33], &m8x8[34], &m8x8[35], &m8x8[36], &m8x8[37], &m8x8[38], &m8x8[39],
           &m8x8[40], &m8x8[41], &m8x8[42], &m8x8[43], &m8x8[44], &m8x8[45], &m8x8[46], &m8x8[47],
           &m8x8[48], &m8x8[49], &m8x8[50], &m8x8[51], &m8x8[52], &m8x8[53], &m8x8[54], &m8x8[55],
           &m8x8[56], &m8x8[57], &m8x8[58], &m8x8[59], &m8x8[60], &m8x8[61], &m8x8[62], &m8x8[63]);

    for (i = 0; i < 16; i++) {
        m4x4_out[i] = m4x4[i];
    }
    for (i = 0; i < 64; i++) {
        m8x8_out[i] = m8x8[i];
    }

    return COM_OK;
}

static void set_sqh(enc_ctrl_t *h, com_seqh_t *sqh)
{
    int i;
    static const double fps_code_tab[14] = { 0, 24000.0 / 1001.0, 24.0, 25.0, 30000.0 / 1001.0, 30.0, 50.0, 60000.0 / 1001.0, 60.0, 100, 120, 200, 240, 300 };
    double fps = h->cfg.fps_num * 1.0 / h->cfg.fps_den;
    double min_diff = 100;

    sqh->profile_id = PROFILE_ID;
    sqh->level_id = LEVEL_ID;
    sqh->progressive_sequence = 1;
    sqh->field_coded_sequence = 0;
    sqh->sample_precision = h->cfg.bit_depth_input == 8 ? 1 : 2;
    sqh->aspect_ratio = 1;

    sqh->frame_rate_code = 15;

    for (i = 0; i < 14; i++) {
        double diff = fabs(fps - fps_code_tab[i]);
        if (diff < min_diff) {
            min_diff = diff;
            sqh->frame_rate_code = i;
        }
    }

    sqh->bit_rate_lower             = 0;
    sqh->bit_rate_upper             = 0;
    sqh->low_delay                  = h->cfg.max_b_frames == 0 ? 1 : 0;
    sqh->temporal_id_enable_flag    = 1;
    sqh->bbv_buffer_size            = (1 << 18) - 1;
    sqh->max_dpb_size               = 16;

    sqh->rpl1_index_exist_flag      = 1;

    if (h->cfg.max_b_frames) {
        static com_rpl_t rpl_l0[MAX_RPLS] = {
            { 3, MAX_RA_ACTIVE, { 16, 32, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, { 17, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, { 18, 17,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, { 19, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, { 20,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  2, 21,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  4, 22,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  5, 23,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  2,  6, 24,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  8, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  9, 26,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, { 10, 27,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  2, 11, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  4, 12, 29,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  5, 13, 30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 4, MAX_RA_ACTIVE, {  2,  6, 14, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }}        
        };
        static com_rpl_t rpl_l1[MAX_RPLS] = {
            { 2, MAX_RA_ACTIVE, { 16, 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  1, 33,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  1,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 4, MAX_RA_ACTIVE, {  1,  2,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  3,  4,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  5,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  1,  6,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  7,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  9,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  1, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 3, MAX_RA_ACTIVE, {  1,  2, 11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  3, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, { 13,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, {  1, 14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { 2, MAX_RA_ACTIVE, { 15,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }}        
        };
        sqh->rpl1_same_as_rpl0_flag = 0;
        sqh->rpls_l0_num = MAX_RPLS;
        sqh->rpls_l1_num = MAX_RPLS;
        memcpy(sqh->rpls_l0, &rpl_l0, sizeof(rpl_l0));
        memcpy(sqh->rpls_l1, &rpl_l1, sizeof(rpl_l1));
    } else {
        static com_rpl_t rpl[4] = {
            { MAX_LD_ACTIVE, MAX_LD_ACTIVE, { 1, 5, 9, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { MAX_LD_ACTIVE, MAX_LD_ACTIVE, { 1, 2, 6, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { MAX_LD_ACTIVE, MAX_LD_ACTIVE, { 1, 3, 7, 11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }},
            { MAX_LD_ACTIVE, MAX_LD_ACTIVE, { 1, 4, 8, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  }}
        };
        sqh->rpl1_same_as_rpl0_flag = 1;
        sqh->rpls_l0_num = 4;
        sqh->rpls_l1_num = 4;
        memcpy(sqh->rpls_l0, &rpl, sizeof(rpl));
        memcpy(sqh->rpls_l1, &rpl, sizeof(rpl));
    }

    sqh->horizontal_size  = h->cfg.horizontal_size;
    sqh->vertical_size    = h->cfg.vertical_size;
    sqh->log2_max_cuwh    = h->info.log2_max_cuwh;
    sqh->min_cu_size      = h->cfg.min_cu_size;
    sqh->max_part_ratio   = h->cfg.max_part_ratio;
    sqh->max_split_times  = h->cfg.max_split_times;
    sqh->min_qt_size      = h->cfg.min_qt_size;
    sqh->max_bt_size      = h->cfg.max_bt_size;
    sqh->max_eqt_size     = h->cfg.max_eqt_size;
    sqh->max_dt_size      = h->cfg.max_dt_size;
    sqh->amvr_enable      = h->cfg.amvr_enable;
    sqh->ipf_enable_flag  = h->cfg.ipf_flag;
    sqh->affine_enable    = h->cfg.affine_enable;
    sqh->smvd_enable      = h->cfg.smvd_enable;
    sqh->num_of_hmvp      = h->cfg.num_of_hmvp;
    sqh->umve_enable      = h->cfg.umve_enable;
    sqh->emvr_enable      = h->cfg.emvr_enable;
    sqh->tscpm_enable     = h->cfg.tscpm_enable;
    sqh->dt_enable        = h->cfg.dt_enable;
    sqh->wq_enable        = h->cfg.wq_enable;
    sqh->seq_wq_mode      = h->cfg.seq_wq_mode;
    sqh->pbt_enable       = h->cfg.pbt_enable;
    sqh->sao_enable       = h->cfg.sao_enable;
    sqh->alf_enable       = h->cfg.alf_enable;
    sqh->ipcm_enable_flag = 0;

    if (sqh->wq_enable && sqh->seq_wq_mode) {
        load_wq_matrix(h->cfg.seq_wq_user, sqh->wq_4x4_matrix, sqh->wq_8x8_matrix);
    }
    if (sqh->wq_enable) {
        if (sqh->seq_wq_mode) {
            load_wq_matrix(h->cfg.seq_wq_user, sqh->wq_4x4_matrix, sqh->wq_8x8_matrix);
        } else {
            memcpy(sqh->wq_4x4_matrix, com_tbl_wq_default_matrix_4x4, sizeof(com_tbl_wq_default_matrix_4x4));
            memcpy(sqh->wq_8x8_matrix, com_tbl_wq_default_matrix_8x8, sizeof(com_tbl_wq_default_matrix_8x8));
        }
    }

    sqh->sectrans_enable = (u8)h->cfg.sectrans_enable;
    sqh->chroma_format = h->cfg.chroma_format;
    sqh->encoding_precision = (h->cfg.bit_depth_internal == 8) ? 1 : 2;

    if (sqh->low_delay == 1) {
        sqh->output_reorder_delay = 0;
    } else {
        //for random access mode, GOP 16
        sqh->output_reorder_delay = com_tbl_log2[h->cfg.max_b_frames + 1];//log2(GopSize)
        if (com_tbl_log2[h->cfg.max_b_frames + 1] == -1) {
            for (int i = h->cfg.max_b_frames + 1; i < 257; i++) {
                if (com_tbl_log2[i] != -1) {
                    sqh->output_reorder_delay = com_tbl_log2[i];
                    break;
                }
            }
        }
        assert(sqh->output_reorder_delay != -1);
    }

    sqh->colocated_patch        = h->cfg.colocated_patch;
    sqh->filter_cross_patch     = h->cfg.filter_cross_patch;
    sqh->patch_width            = h->cfg.patch_width  == 0 ? h->info.pic_width_in_lcu  : h->cfg.patch_width;
    sqh->patch_height           = h->cfg.patch_height == 0 ? h->info.pic_height_in_lcu : h->cfg.patch_height;
}

static void set_pic_header(enc_ctrl_t *h, com_pic_t *pic_rec)
{
    com_pic_header_t *pichdr = &h->pichdr;
    pichdr->bbv_delay =  0xFFFFFFFF;
    pichdr->time_code_flag = 0;
    pichdr->time_code = 0;
    pichdr->low_delay = h->info.sqh.low_delay;

    if (pichdr->low_delay) {
        pichdr->bbv_check_time = (1 << 16) - 2;
    }

    pichdr->loop_filter_disable_flag = (h->cfg.use_deblock) ? 0 : 1;

    if (h->info.sqh.wq_enable) {
        pichdr->pic_wq_enable = 1;//default
        pichdr->pic_wq_data_idx = h->cfg.pic_wq_data_idx;
        if (pichdr->pic_wq_data_idx == 0) {
            memcpy(pichdr->wq_4x4_matrix, h->info.sqh.wq_4x4_matrix, sizeof(pichdr->wq_4x4_matrix));
            memcpy(pichdr->wq_8x8_matrix, h->info.sqh.wq_8x8_matrix, sizeof(pichdr->wq_8x8_matrix));
        } else if (pichdr->pic_wq_data_idx == 1) {
            pichdr->wq_param = h->cfg.wq_param;
            pichdr->wq_model = h->cfg.wq_model;

            if (pichdr->wq_param == 0) {
                memcpy(pichdr->wq_param_vector, com_tbl_wq_default_param[1], sizeof(pichdr->wq_param_vector));
            } else if (pichdr->wq_param == 1) {
                sscanf(h->cfg.wq_param_undetailed, "[%d,%d,%d,%d,%d,%d]",
                       &pichdr->wq_param_vector[0],
                       &pichdr->wq_param_vector[1],
                       &pichdr->wq_param_vector[2],
                       &pichdr->wq_param_vector[3],
                       &pichdr->wq_param_vector[4],
                       &pichdr->wq_param_vector[5]);
            } else {
                sscanf(h->cfg.wq_param_detailed, "[%d,%d,%d,%d,%d,%d]",
                       &pichdr->wq_param_vector[0],
                       &pichdr->wq_param_vector[1],
                       &pichdr->wq_param_vector[2],
                       &pichdr->wq_param_vector[3],
                       &pichdr->wq_param_vector[4],
                       &pichdr->wq_param_vector[5]);
            }
            set_pic_wq_matrix_by_param(pichdr->wq_param_vector, pichdr->wq_model, pichdr->wq_4x4_matrix, pichdr->wq_8x8_matrix);
        } else {
            load_wq_matrix(h->cfg.pic_wq_user, pichdr->wq_4x4_matrix, pichdr->wq_8x8_matrix);
        }
    } else {
        pichdr->pic_wq_enable = 0;
        init_pic_wq_matrix(pichdr->wq_4x4_matrix, pichdr->wq_8x8_matrix);
    }

    pichdr->progressive_frame  = 1;
    pichdr->picture_structure  = 1;
    pichdr->top_field_first    = 0;
    pichdr->repeat_first_field = 0;
    pichdr->top_field_picture_flag = 0;
    pichdr->fixed_picture_qp_flag = !h->cfg.adaptive_dqp;
    pichdr->random_access_decodable_flag = 1; 
    pichdr->loop_filter_parameter_flag = 0;
    pichdr->alpha_c_offset = 0;
    pichdr->beta_offset = 0;
    pichdr->tool_alf_on = h->info.sqh.alf_enable;
    pichdr->affine_subblk_size_idx = 0; // 0->4X4, 1->8X8
    pichdr->picture_output_delay = (int)(pichdr->poc - h->pichdr.dtr + h->info.sqh.output_reorder_delay);

    if ((pichdr->dtr % DOI_CYCLE_LENGTH) < h->prev_dtr) {
        for (int i = 0; i < h->rpm.max_pb_size; i++) {
            com_pic_t *pic = h->rpm.pic[i];
            if (pic != NULL) {
                pic->dtr -= DOI_CYCLE_LENGTH;
                assert(pic->dtr >= (-256));
            }
        }
    }

    h->top_pic[0] = h->top_pic[1] = NULL;

    if (pichdr->poc == 0) {
        pichdr->ref_pic_list_sps_flag[0] = 1;
        pichdr->ref_pic_list_sps_flag[1] = 1;
        pichdr->rpl_l0_idx    = 0;
        pichdr->rpl_l0_idx    = 0;
        pichdr->rpl_l0.active = 0;
        pichdr->rpl_l1.active = 0;
        h->rpm.ptr_l_ip       = 0;
        h->rpm.ptr_l_l_ip     = 0;
        h->rpm.ptr_l_i        = 0;
    } else {
        int is_ld = h->info.sqh.low_delay;
        int is_top = pic_rec->layer_id < FRM_DEPTH_2 && !is_ld;

        com_refm_remove_ref_pic(&h->rpm, pichdr, pic_rec, h->cfg.close_gop, is_ld);
        com_refm_build_ref_buf (&h->rpm);

        if (pichdr->slice_type == SLICE_I) {
            pichdr->rpl_l0.active = 0;
            pichdr->rpl_l1.active = 0;
        } else if (is_ld) {
            pichdr->rpl_l0.active = MAX_LD_ACTIVE;
            pichdr->rpl_l1.active = MAX_LD_ACTIVE;
        } else {
            pichdr->rpl_l0.active = MAX_RA_ACTIVE;
            pichdr->rpl_l1.active = MAX_RA_ACTIVE;
        }

        for (int i = 0; i < h->rpm.cur_num_ref_pics; i++) {
            com_pic_t *ref = h->rpm.pic[i];
            if (ref && ref->b_ref && ref->layer_id <= FRM_DEPTH_1 && (h->top_pic[0] == NULL || h->top_pic[0]->ptr < ref->ptr)) {
                h->top_pic[0] = ref;
            }
        }
        com_refm_create_rpl(&h->rpm, pichdr, h->refp, h->top_pic, is_top);
        com_refm_pick_seqhdr_idx(&h->info.sqh, pichdr);

        for (int i = 0; i < 2; i++) {
            if (h->top_pic[i]) {
                com_img_addref(h->top_pic[i]->img);
            }
        }
    }
    com_refm_insert_rec_pic(&h->rpm, pic_rec, h->refp);
}

static int lbac_enc_tree(core_t *core, lbac_t *lbac, bs_t *bs, int x0, int y0, int cup, int cu_width, int cu_height, int cud
                        , const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status)
{
    int ret;
    com_info_t *info = core->info;
    s8 split_mode;
    int lcu_pos = core->lcu_y * info->pic_width_in_lcu + core->lcu_x;
    enc_cu_t *cu_data = &core->map_cu_data[lcu_pos];
    com_get_split_mode(&split_mode, cud, cup, cu_width, cu_height, info->max_cuwh, cu_data->split_mode);
    u8 tree_status_child = TREE_LC;
    u8 cons_pred_mode_child;

    if (split_mode != NO_SPLIT) {
        lbac_enc_split_mode(lbac, bs, core, split_mode, cud, cup, cu_width, cu_height, info->max_cuwh, parent_split, qt_depth, bet_depth, x0, y0);
        tree_status_child = (tree_status == TREE_LC && com_tree_split(cu_width, cu_height, split_mode, core->pichdr->slice_type)) ? TREE_L : tree_status;
        if (cons_pred_mode == NO_MODE_CONS && is_use_cons(cu_width, cu_height, split_mode, core->pichdr->slice_type)) {
            cons_pred_mode_child = com_get_cons_pred_mode(cud, cup, cu_width, cu_height, info->max_cuwh, cu_data->split_mode);
            lbac_enc_cons_pred_mode(lbac, bs, cons_pred_mode_child);
        } else {
            cons_pred_mode_child = cons_pred_mode;
        }
        com_split_struct_t split_struct;
        com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, info->log2_lcuwh_in_scu, &split_struct);
        for (int part_num = 0; part_num < split_struct.part_count; ++part_num) {
            int cur_part_num = part_num;
            int sub_cuw = split_struct.width[cur_part_num];
            int sub_cuh = split_struct.height[cur_part_num];
            int x_pos = split_struct.x_pos[cur_part_num];
            int y_pos = split_struct.y_pos[cur_part_num];

            if (x_pos < info->pic_width && y_pos < info->pic_height) {
                ret = lbac_enc_tree(core, lbac, bs, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
                                   , split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child);
                com_assert_g(!ret, ERR);
            }
        }
        if (tree_status_child == TREE_L && tree_status == TREE_LC) {
            com_assert(x0 + cu_width <= info->pic_width && y0 + cu_height <= info->pic_height);
            core->tree_status = TREE_C;
            core->cons_pred_mode = NO_MODE_CONS;
            ret = lbac_enc_unit_chroma(lbac, bs, core, cu_data, x0, y0, cup, cu_width, cu_height);
            core->tree_status = TREE_LC;
        }
    } else {
        com_assert(x0 + cu_width <= info->pic_width && y0 + cu_height <= info->pic_height);
        lbac_enc_split_mode(lbac, bs, core, split_mode, cud, cup, cu_width, cu_height, info->max_cuwh, parent_split, qt_depth, bet_depth, x0, y0);
        core->tree_status = tree_status;
        core->cons_pred_mode = cons_pred_mode;

        ret = lbac_enc_unit(lbac, bs, core, cu_data, x0, y0, cup, cu_width, cu_height);
        com_assert_g(!ret, ERR);
    }
    return COM_OK;
ERR:
    return ret;
}


int uavs3e_get_img(void *id, com_img_t **img)
{
    enc_ctrl_t *h = (enc_ctrl_t *)id;

    for (int i = 0; i < h->ilist_size; i++) {
        if (h->ilist_imgs[i] == NULL) {
            *img = com_img_create(h->cfg.pic_width, h->cfg.pic_height, NULL, 3);
            com_assert_rv(*img != NULL, COM_ERR_OUT_OF_MEMORY);
            h->ilist_imgs[i] = *img;
            return COM_OK;
        } else if (com_img_getref(h->ilist_imgs[i]) == 1) {
            *img = h->ilist_imgs[i];
            return COM_OK;
        }
    }
    return COM_ERR_UNEXPECTED;
}

int enc_pic_finish(enc_ctrl_t *h, pic_thd_param_t *pic_thd, enc_stat_t *stat)
{
    int i, j;

    com_pic_t *pic_org = &pic_thd->pic_org;
    com_pic_t *pic_rec = pic_thd->pic_rec;
    com_img_t *imgb_o  = pic_org->img;
    com_img_t *imgb_c  = pic_rec->img;

    stat->type       = pic_thd->pichdr.slice_type;
    stat->qp         = (float)pic_rec->picture_qp_real;
    stat->poc        = pic_thd->ptr;
    stat->rec_img    = imgb_c;
    stat->org_img    = imgb_o;
    stat->pts        = imgb_c->pts = imgb_o->pts;
    stat->dts        = imgb_c->dts = imgb_o->dts;
    stat->buf        = pic_thd->bs_buf;
    stat->bytes      = pic_thd->total_bytes;
    stat->user_bytes = pic_thd->user_bytes;

    com_img_release(imgb_c);

    for (i = 0; i < REFP_NUM; i++) {
        if (pic_thd->top_pic[i]) {
            com_img_release(pic_thd->top_pic[i]->img);
        }
        stat->refpic_num[i] = pic_thd->num_refp[i];
        for (j = 0; j < stat->refpic_num[i]; j++) {
            com_pic_t *refpic = pic_thd->refp[j][i].pic;
            stat->refpic[i][j] = refpic->ptr;
            com_img_release(refpic->img);

            if (!refpic->b_ref && refpic->subpel && 1 == com_img_getref(refpic->img)) {
                refpic->subpel->b_used = 0;
            }
        }
    }

    if (imgb_o) {
        com_img_release(imgb_o);
    }

    return COM_OK;
}

void *enc_lcu_row(core_t *core, enc_lcu_row_t *row)
{
    int lcu_y                =  row->lcu_y;
    enc_pic_param_t *pic_ctx =  row->pic_info;
    uavs3e_sem_t *sem_up     =  row->sem_up;
    uavs3e_sem_t *sem_curr   =  row->sem_curr;
    lbac_t *lbac             = &row->lbac_row;
    lbac_t *lbac_row_next    =  row->lbac_row_next;

    int last_lcu_qp          = pic_ctx->pathdr->slice_qp;
    com_info_t *info         = pic_ctx->info;
    com_pic_header_t *pichdr = pic_ctx->pichdr;
    u8 *map_qp               = pic_ctx->map->map_qp + lcu_y * info->pic_width_in_lcu;

    row->total_qp        = 0;
    core->cnt_hmvp_cands = 0;
    core->lcu_y          = lcu_y;
    core->lcu_pix_y      = lcu_y << info->log2_max_cuwh;

    core->param          = pic_ctx->param;
    core->pichdr         = pic_ctx->pichdr;
    core->pathdr         = pic_ctx->pathdr;
    core->info           = pic_ctx->info;
    core->map            = pic_ctx->map;
    core->map_cu_data    = pic_ctx->map_cu_data;
    core->sao_blk_params = pic_ctx->sao_blk_params;
    core->pic_org        = pic_ctx->pic_org;
    core->pic_rec        = pic_ctx->pic_rec;
    core->refp           = pic_ctx->refp;
    core->ptr            = pic_ctx->ptr;
    core->num_refp       = pic_ctx->num_refp;
    core->slice_type     = pichdr->slice_type;
    core->pinter.ptr     = core->ptr;

    memcpy(core->linebuf_sao,      pic_ctx->linebuf_sao,                sizeof(pel *) * 3);
    memcpy(core->linebuf_intra[0], pic_ctx->linebuf_intra[core->lcu_y], sizeof(pel *) * 3);

    if (core->lcu_y) {
        memcpy(core->linebuf_intra[1], pic_ctx->linebuf_intra[core->lcu_y - 1], sizeof(pel *) * 3);
    }

    if (pichdr->pic_wq_enable) {
        core->wq[0] = pichdr->wq_4x4_matrix;
        core->wq[1] = pichdr->wq_8x8_matrix;
    } else {
        core->wq[0] = core->wq[1] = NULL;
    }

    com_mset_x64a(core->motion_cands, 0, sizeof(com_motion_t)*ALLOWED_HMVP_NUM);

    for (core->lcu_x = 0; core->lcu_x < info->pic_width_in_lcu; core->lcu_x++) {
        com_info_t *info = core->info;
        int lcu_qp;
        int adj_qp_cb, adj_qp_cr;

        if (core->lcu_x < info->pic_width_in_lcu - 1) {
            safe_sem_wait(sem_up);
        }

        core->lcu_pix_x = core->lcu_x << info->log2_max_cuwh;

        if (core->pathdr->fixed_slice_qp_flag) {
            lcu_qp = core->pathdr->slice_qp;
        } else {
            if (core->param->adaptive_dqp) {
                float *dqp_map = core->map->map_dqp + (core->lcu_pix_y / UNIT_SIZE) * (info->pic_width / UNIT_SIZE) + (core->lcu_pix_x / UNIT_SIZE);
                double dqp_all = 0;
                double blk_cnt = 0;
                int lcu_w = COM_MIN(info->max_cuwh, info->pic_width  - core->lcu_pix_x);
                int lcu_h = COM_MIN(info->max_cuwh, info->pic_height - core->lcu_pix_y);

                for (int i = 0; i < lcu_h - (UNIT_SIZE - 1); i += UNIT_SIZE) {
                    for (int j = 0; j < lcu_w - (UNIT_SIZE - 1); j += UNIT_SIZE) {
                        dqp_all += dqp_map[j / UNIT_SIZE];
                        blk_cnt++;
                    }
                    dqp_map += info->pic_width / UNIT_SIZE;
                }
                if (blk_cnt) {
                    lcu_qp = (int)(core->pathdr->slice_qp + (dqp_all / blk_cnt) + 0.5);
                    lcu_qp = COM_CLIP3(0, (MAX_QUANT_BASE + info->qp_offset_bit_depth), lcu_qp);
                    last_lcu_qp = lcu_qp;
                } else {
                    lcu_qp = core->pathdr->slice_qp;
                }
            } else {
                com_assert(0);
            }
        }
        *map_qp++ = (u8)lcu_qp;

        core->lcu_qp_y = lcu_qp;

        adj_qp_cb = core->lcu_qp_y + pichdr->chroma_quant_param_delta_cb - info->qp_offset_bit_depth;
        adj_qp_cr = core->lcu_qp_y + pichdr->chroma_quant_param_delta_cr - info->qp_offset_bit_depth;

        adj_qp_cb = COM_CLIP(adj_qp_cb, -16, MAX_QUANT_BASE);
        adj_qp_cr = COM_CLIP(adj_qp_cr, -16, MAX_QUANT_BASE);

        if (adj_qp_cb >= 0) {
            adj_qp_cb = com_tbl_qp_chroma_ajudst[COM_MIN(MAX_QUANT_BASE, adj_qp_cb)];
        }
        if (adj_qp_cr >= 0) {
            adj_qp_cr = com_tbl_qp_chroma_ajudst[COM_MIN(MAX_QUANT_BASE, adj_qp_cr)];
        }
        core->lcu_qp_u = COM_CLIP(adj_qp_cb + info->qp_offset_bit_depth, 0, MAX_QUANT_BASE + info->qp_offset_bit_depth);
        core->lcu_qp_v = COM_CLIP(adj_qp_cr + info->qp_offset_bit_depth, 0, MAX_QUANT_BASE + info->qp_offset_bit_depth);

        lcu_qp -= info->qp_offset_bit_depth; // calculate lambda for 8-bit distortion
        row->total_qp += lcu_qp;
        core->lambda[0] = 1.43631 * pow(2.0, (lcu_qp - 16.0) / 4.0);
        core->dist_chroma_weight[0] = pow(2.0, (lcu_qp - adj_qp_cb) / 4.0);
        core->dist_chroma_weight[1] = pow(2.0, (lcu_qp - adj_qp_cr) / 4.0);

        core->lambda[1] = core->lambda[0] / core->dist_chroma_weight[0];
        core->lambda[2] = core->lambda[0] / core->dist_chroma_weight[1];
        core->sqrt_lambda[0] = sqrt(core->lambda[0]);
        core->sqrt_lambda[1] = sqrt(core->lambda[1]);
        core->sqrt_lambda[2] = sqrt(core->lambda[2]);

        /* initialize structures *****************************************/
        enc_mode_init_lcu(core);
        com_mset(core->history_data, 0, sizeof(enc_history_t) * MAX_CU_DEPTH * MAX_CU_DEPTH * MAX_CU_CNT_IN_LCU);

        /* mode decision *************************************************/
        enc_mode_analyze_lcu(core, lbac);

        /* entropy coding ************************************************/
        //write to bitstream
        if (info->sqh.sao_enable) {
            int lcu_pos = core->lcu_x + core->lcu_y * info->pic_width_in_lcu;
            lbac_enc_sao_param(lbac, NULL, core->pathdr, core->map, core->info, lcu_y, core->lcu_x, core->sao_blk_params[lcu_pos], core->sao_blk_params);
        }
        lbac_enc_tree(core, lbac, NULL, core->lcu_pix_x, core->lcu_pix_y, 0, info->max_cuwh, info->max_cuwh, 0, NO_SPLIT, 0, 0, NO_MODE_CONS, TREE_LC);

        /* end_of_picture_flag */
        lbac_enc_slice_end_flag(lbac, NULL, 0);
        printf("*");
        fflush(stdout);

#define KEEP_CONST 0

        if (lbac_row_next) {
            if (core->param->wpp_threads > 1 || KEEP_CONST) {
                if (core->lcu_x == 1) {
                    lbac_copy(lbac_row_next, lbac);
                }
            } else {
                if (core->lcu_x == info->pic_width_in_lcu - 1) {
                    lbac_copy(lbac_row_next, lbac);
                }
            }
        }
        if (core->lcu_x >= 1) {
            safe_sem_post(sem_curr);
        }
    }

    com_pic_t *pic_rec = core->pic_rec;

    if (info->sqh.sao_enable) {
        safe_sem_wait(sem_up);
        com_sao_one_row(info, core->map, pic_rec, core->sao_blk_params, lcu_y, core->sao_src_buf, core->linebuf_sao);
        safe_sem_post(sem_curr);
    }

    return core;
}

void enc_get_pic_qp(enc_pic_t *ep, pic_thd_param_t *p)
{
    com_pic_header_t *pichdr   = &p->pichdr;
    enc_cfg_t        *param    =  p->param;
    com_pic_t        *pic_org  = &p->pic_org;
    com_info_t       *info     = &ep->info;
    com_img_t        *img_org  = pic_org->img;
    com_img_t        *ref_l0[4];
    com_img_t        *ref_l1[4];
    int               base_qp;
    double            icost, icost_uv[2];

    for (int refi = 0; refi < p->num_refp[PRED_L0]; refi++) {
        ref_l0[refi] = p->refp[refi][PRED_L0].pic->img;
    }
    for (int refi = 0; refi < p->num_refp[PRED_L1]; refi++) {
        ref_l1[refi] = p->refp[refi][PRED_L1].pic->img;
    }
    for (int lidx = 0; lidx < 2; lidx++) {
        for (int ref = 0; ref < p->num_refp[lidx]; ref++) {
            wait_ref_available(p->refp[ref][lidx].pic, info->pic_height);
        }
        if (p->top_pic[lidx]) {
            wait_ref_available(p->top_pic[lidx], info->pic_height);
        }
    }

    float *dqp_map = param->adaptive_dqp ? ep->map.map_dqp : NULL;

    if (param->chroma_dqp) {
        pic_org->picture_satd = loka_estimate_coding_cost(&ep->pinter, img_org, ref_l0, ref_l1, p->num_refp, info->bit_depth_internal, &icost, icost_uv, dqp_map);
    } else {
        pic_org->picture_satd = loka_estimate_coding_cost(&ep->pinter, img_org, ref_l0, ref_l1, p->num_refp, info->bit_depth_internal, NULL, NULL, dqp_map);
    }
    if (param->rc_type == RC_TYPE_NULL) {
        base_qp = (int)(enc_get_hgop_qp(param->qp, pic_org->layer_id, info->sqh.low_delay) + 0.5);
    } else {
        int qp_l0 = p->top_pic[0] ? ((int)(p->top_pic[0]->picture_qp_real + 0.5) + (p->top_pic[0]->layer_id == FRM_DEPTH_0 ? 3 : 0)) : -1;
        int qp_l1 = p->top_pic[1] ? ((int)(p->top_pic[1]->picture_qp_real + 0.5) + (p->top_pic[1]->layer_id == FRM_DEPTH_0 ? 3 : 0)) : -1;
        base_qp = rc_get_qp(p->rc, pic_org, qp_l0, qp_l1);
    }
    pic_org->picture_qp = (u8)COM_CLIP3(0, (MAX_QUANT_BASE + info->qp_offset_bit_depth), base_qp + info->qp_offset_bit_depth);

    int qp_l = pic_org->picture_qp - info->qp_offset_bit_depth;
    int target_chroma_qp, opt_c_dqp;

    // delta QP of Cb
    if (p->param->chroma_dqp) {
        target_chroma_qp = com_tbl_qp_chroma_adjust_enc[COM_CLIP(qp_l + 1, 0, 63)] - (int)(icost / icost_uv[0]) + 1;
    } else {
        target_chroma_qp = com_tbl_qp_chroma_adjust_enc[COM_CLIP(qp_l + 1, 0, 63)];
    }
    target_chroma_qp = COM_MAX(0, target_chroma_qp);
    for (opt_c_dqp = -10; opt_c_dqp < 10; opt_c_dqp++) {
        if (target_chroma_qp == com_tbl_qp_chroma_ajudst[COM_CLIP(qp_l + opt_c_dqp, 0, 63)]) {
            break;
        }
    }
    opt_c_dqp = COM_CLIP(opt_c_dqp, -qp_l, 63 - qp_l);
    pichdr->chroma_quant_param_delta_cb = param->qp_offset_cb + opt_c_dqp;
    pichdr->chroma_quant_param_delta_cb = COM_CLIP(pichdr->chroma_quant_param_delta_cb, -16, 16);

    // delta QP of Cr
    if (p->param->chroma_dqp) {
        target_chroma_qp = com_tbl_qp_chroma_adjust_enc[COM_CLIP(qp_l + 1, 0, 63)] - (int)(icost / icost_uv[1]) + 1;
    } else {
        target_chroma_qp = com_tbl_qp_chroma_adjust_enc[COM_CLIP(qp_l + 1, 0, 63)];
    }
    target_chroma_qp = COM_MAX(0, target_chroma_qp);
    for (opt_c_dqp = -10; opt_c_dqp < 10; opt_c_dqp++) {
        if (target_chroma_qp == com_tbl_qp_chroma_ajudst[COM_CLIP(qp_l + opt_c_dqp, 0, 63)]) {
            break;
        }
    }
    opt_c_dqp = COM_CLIP(opt_c_dqp, -qp_l, 63 - qp_l);
    pichdr->chroma_quant_param_delta_cr = param->qp_offset_cr + opt_c_dqp;
    pichdr->chroma_quant_param_delta_cr = COM_CLIP(pichdr->chroma_quant_param_delta_cr, -16, 16);

    if (pichdr->chroma_quant_param_delta_cb == 0 && pichdr->chroma_quant_param_delta_cr == 0) {
        pichdr->chroma_quant_param_disable_flag = 1;
    } else {
        pichdr->chroma_quant_param_disable_flag = 0;
    }
}

void *enc_pic_thread(enc_pic_t *ep, pic_thd_param_t *p)
{
    enc_pic_param_t   pic_ctx;
    enc_cfg_t          *param = p->param;
    com_patch_header_t pathdr;
    com_pic_header_t  *pichdr = &p->pichdr;
    com_pic_t        *pic_org = &p->pic_org;
    com_pic_t        *pic_rec =  p->pic_rec;
    com_map_t            *map = &ep->map;
    com_info_t          *info = &ep->info;
    com_img_t        *img_org = pic_org->img;

    enc_get_pic_qp(ep, p);

    map->map_refi = pic_rec->map_refi;
    map->map_mv   = pic_rec->map_mv;

    /* clear map */
    com_mset_x64a(map->map_refi  - info->i_scu - 1, -1, sizeof(s8       ) * info->f_scu * REFP_NUM);
    com_mset_x64a(map->map_mv    - info->i_scu - 1,  0, sizeof(s16      ) * info->f_scu * REFP_NUM * MV_D);
    com_mset_x64a(map->map_scu   - info->i_scu - 1,  0, sizeof(com_scu_t) * info->f_scu);
    com_mset_x64a(map->map_pos   - info->i_scu - 1,  0, sizeof(u32      ) * info->f_scu);
    com_mset_x64a(map->map_edge  - info->i_scu - 1,  0, sizeof(u8       ) * info->f_scu);
    com_mset_x64a(map->map_patch - info->i_scu - 1,  0, sizeof(u8       ) * info->f_scu);

    pathdr.fixed_slice_qp_flag = pichdr->fixed_picture_qp_flag;
    pathdr.slice_qp = pic_org->picture_qp;
    pathdr.slice_sao_enable[Y_C] = pathdr.slice_sao_enable[U_C] = pathdr.slice_sao_enable[V_C] = 1;

    pic_ctx.param            =  param;
    pic_ctx.pichdr           =  pichdr;
    pic_ctx.pic_org          =  pic_org;
    pic_ctx.pic_rec          =  pic_rec;
    pic_ctx.refp             =  p->refp;
    pic_ctx.ptr              =  p->ptr;
    pic_ctx.num_refp         =  p->num_refp;
    pic_ctx.pathdr           = &pathdr;
    pic_ctx.sao_blk_params   =  ep->sao_blk_params;
    pic_ctx.info             = &ep->info;
    pic_ctx.map              = &ep->map;
    pic_ctx.map_cu_data      =  ep->map_cu_data;
    pic_ctx.linebuf_intra    =  ep->linebuf_intra;
    memcpy(pic_ctx.linebuf_sao, ep->linebuf_sao, sizeof(pel*) * 3);

    p->total_qp = 0;

    lbac_t *lbac_fst_row = &ep->array_row[0].lbac_row;
    lbac_reset(lbac_fst_row);
    lbac_ctx_init(&lbac_fst_row->h);

    if (info->wpp_threads == 1) {
        for (int lcu_y = 0; lcu_y < info->pic_height_in_lcu; lcu_y++) {
            enc_lcu_row_t *row = &ep->array_row[lcu_y];
            row->pic_info      = &pic_ctx;
            row->lcu_y         = lcu_y;
            row->sem_up        = NULL;
            row->sem_curr      = NULL;
            row->lbac_row_next = (lcu_y == info->pic_height_in_lcu - 1) ? NULL : &ep->array_row[lcu_y + 1].lbac_row;
            enc_lcu_row(ep->main_core, row);
            p->total_qp += row->total_qp;
        }
    } else {
        if (pic_org->layer_id < 3) {
            uavs3e_threadpool_set_priority(ep->wpp_threads_pool, THREAD_PRIORITY_HIGHEST);
        } else if (pic_org->b_ref == 0) {
            uavs3e_threadpool_set_priority(ep->wpp_threads_pool, THREAD_PRIORITY_LOWEST);
        }
        for (int lcu_y = 0; lcu_y < info->pic_height_in_lcu; lcu_y++) {
            enc_lcu_row_t *row = &ep->array_row[lcu_y];
            row->pic_info      = &pic_ctx;
            row->lcu_y         = lcu_y;
            row->sem_up        = lcu_y ? &ep->array_row[lcu_y - 1].sem : NULL;
            row->sem_curr      = (lcu_y == info->pic_height_in_lcu - 1) ? NULL : &row->sem;
            row->lbac_row_next = (lcu_y == info->pic_height_in_lcu - 1) ? NULL : &ep->array_row[lcu_y + 1].lbac_row;
            uavs3e_threadpool_run(ep->wpp_threads_pool, (void*(*)(void *, void*))enc_lcu_row, row, 1);
        }
        for (int lcu_y = 0; lcu_y < info->pic_height_in_lcu; lcu_y++) {
            enc_lcu_row_t *row = &ep->array_row[lcu_y];
            uavs3e_threadpool_wait(ep->wpp_threads_pool, row);
            p->total_qp += row->total_qp;
        }
        uavs3e_threadpool_set_priority(ep->wpp_threads_pool, THREAD_PRIORITY_NORMAL);
    }
    printf("\n");

    if (info->sqh.alf_enable) {
        double lambda = 1.43631 * pow(2.0, (p->total_qp * 1.0 / info->f_lcu - 16.0) / 4.0);
        pichdr->m_alfPictureParam = ep->Enc_ALF.m_alfPictureParam;
        pichdr->pic_alf_on = ep->pic_alf_on;
        uavs3e_alf_frame(ep, pic_rec, pic_org, lambda);
    } else {
        pichdr->m_alfPictureParam = NULL;
        pichdr->pic_alf_on = NULL;
    }

    pic_rec->picture_qp_real   = (p->total_qp * 1.0 / info->f_lcu) - info->qp_offset_bit_depth;
    pic_rec->picture_qp        = pic_org->picture_qp;
    pic_rec->picture_satd      = pic_org->picture_satd;
    pic_rec->layer_id          = pic_org->layer_id;

    if (pic_org->b_ref) {
        com_img_padding(pic_rec->img, 3, 0);
        com_if_luma_frame(pic_rec->subpel->imgs, ep->ip_tmp_buf, info->bit_depth_internal);
        uavs3e_pthread_mutex_lock(&pic_rec->mutex);
        pic_rec->end_line = info->pic_height;
        uavs3e_pthread_cond_broadcast(&pic_rec->cv);
        uavs3e_pthread_mutex_unlock(&pic_rec->mutex);
    }

    /* Bit-stream re-writing (START) */
    bs_t bs;
    bs_init(&bs, p->bs_buf, ep->bs_buf_demulate, MAX_BS_BUF);

    if (pichdr->slice_type == SLICE_I) {
        ec_write_sqh(&bs, &info->sqh);
        bs_flush(&bs);
    }
    ec_write_pichdr(&bs, pichdr, &info->sqh, pic_org);
    bs_flush(&bs);

    p->user_bytes = 0;

    if (param->use_pic_sign) {
        int cur_bytes = BS_GET_BYTES(&bs);
        ec_write_ext_and_usr_data(&bs, param->use_pic_sign, pic_rec, 1, 0, 0);
        bs_flush(&bs);
        p->user_bytes = BS_GET_BYTES(&bs) - cur_bytes;
    }

    p->total_bytes = BS_GET_BYTES(&bs);

    bs_init(&bs, bs.cur, bs.buftmp, ((int)(bs.end - bs.cur)) + 1);

    /* Encode patch header */
    ec_write_patch_hdr(&bs, &info->sqh, pichdr, &pathdr, 0);

    core_t *core = ep->main_core;
    core->lcu_x           =  0;
    core->lcu_y           =  0;
    core->lcu_pix_x       =  0;
    core->lcu_pix_y       =  0;
    core->param           =  param;
    core->pichdr          =  pichdr;
    core->info            =  info;
    core->pic_org         =  pic_org;
    core->pic_rec         =  pic_rec;
    core->refp            =  p->refp;
    core->ptr             =  p->ptr;
    core->num_refp        =  p->num_refp;
    core->pathdr          = &pathdr;
    core->sao_blk_params  =  ep->sao_blk_params;
    core->map             = &ep->map;
    core->map_cu_data     =  ep->map_cu_data;
    core->slice_type      = pichdr->slice_type;

    for (int i = 0; i < info->f_scu; i++) {
        map->map_scu[i].coded = 0;
    }

    int lcu_pos = 0;
    lbac_t lbac_enc;
    lbac_t *lbac = &lbac_enc;
    lbac_reset(lbac);
    lbac_ctx_init(&lbac->h);

    /* Encode slice data */
    int last_lcu_qp = pathdr.slice_qp;
    int last_lcu_delta_qp = 0;
    u8 *map_qp = map->map_qp;

    for (core->lcu_y = 0; core->lcu_y < info->pic_height_in_lcu; core->lcu_y++) {
        core->lcu_pix_y = core->lcu_y << info->log2_max_cuwh;

        for (core->lcu_x = 0; core->lcu_x < info->pic_width_in_lcu; core->lcu_x++) {
            core->lcu_pix_x = core->lcu_x << info->log2_max_cuwh;

            if (!pathdr.fixed_slice_qp_flag) {
                int qp = *map_qp++;
                int dqp = qp - last_lcu_qp;
                lbac_enc_lcu_delta_qp(lbac, &bs, dqp, last_lcu_delta_qp);
                last_lcu_qp = qp;
                last_lcu_delta_qp = dqp;
            }
            if (info->sqh.sao_enable) {
                lbac_enc_sao_param(lbac, &bs, &pathdr, &ep->map, info, core->lcu_y, core->lcu_x, ep->sao_blk_params[lcu_pos], ep->sao_blk_params);
            }
            if (info->sqh.alf_enable) {
                for (int compIdx = Y_C; compIdx < N_C; compIdx++) {
                    if (ep->pic_alf_on[compIdx]) {
                        lbac_enc_alf_flag(lbac, &bs, (int)ep->Enc_ALF.m_AlfLCUEnabled[lcu_pos][compIdx]);
                    }
                }
            }
            lbac_enc_tree(core, lbac, &bs, core->lcu_pix_x, core->lcu_pix_y, 0, info->max_cuwh, info->max_cuwh, 0, NO_SPLIT, 0, 0, NO_MODE_CONS, TREE_LC);

            /* end_of_picture_flag */
            if (++lcu_pos < info->f_lcu) {
                lbac_enc_slice_end_flag(lbac, &bs, 0);
            } else {
                lbac_enc_slice_end_flag(lbac, &bs, 1);
                lbac_finish(lbac, &bs);
                break;
            }
        }
    }

    bs_write1(&bs, 1);// stuffing bit

    while (!IS_BYTE_ALIGN(&bs)) {
        bs_write1(&bs, 0);
    }

    bs_demulate(&bs);

    /*encode patch end*/
    ec_write_patch_end(&bs);

    /* de-init BSW */
    bs_flush(&bs);

    /* set stat */
    p->total_bytes +=  BS_GET_BYTES(&bs);
    pic_rec->picture_bits = (p->total_bytes - p->user_bytes) * 8;

    return ep;
}

static void enc_push_frm(enc_ctrl_t *h, com_img_t *img, int insert_idr)
{
    img->ptr = h->ptr++;

    com_img_addref(img);

    if (h->cfg.i_period == 1) { // AI
        add_input_node(h, img, 1, FRM_DEPTH_0, SLICE_I);
        h->lastI_ptr = img->ptr;
        return;
    }
    if (h->info.sqh.low_delay) { // LD
        if (h->lastI_ptr == -1 || insert_idr || (h->cfg.i_period && img->ptr - h->lastI_ptr == h->cfg.i_period)) {
            add_input_node(h, img, 1, FRM_DEPTH_0, SLICE_I);
            h->lastI_ptr = img->ptr;
        } else {
            static tab_s8 tbl_slice_depth_P[4] = { FRM_DEPTH_3,  FRM_DEPTH_2, FRM_DEPTH_3, FRM_DEPTH_1 };
            int frm_depth = tbl_slice_depth_P[(img->ptr - h->lastI_ptr - 1) % 4];
            add_input_node(h, img, 1, frm_depth, SLICE_B);
        }
        return;
    }

    /*** RA ***/
    if (h->lastI_ptr == -1) { // First frame
        add_input_node(h, img, 1, FRM_DEPTH_0, SLICE_I);
        h->lastI_ptr = img->ptr;
        h->img_lastIP = img;
        com_img_addref(img);
        return;
    } 

    com_img_t *last_img = h->img_rsize ? h->img_rlist[h->img_rsize - 1].img : h->img_lastIP;
    int bit_depth = h->info.bit_depth_internal;

    h->img_rlist[h->img_rsize].img        = img;
    h->img_rlist[h->img_rsize].insert_idr = insert_idr;
    h->img_rlist[h->img_rsize].sc_ratio   = loka_get_sc_ratio(&h->pinter, img, last_img, bit_depth);
    h->img_rsize++;

    if (h->img_rsize < h->cfg.lookahead) {
        return;
    }
    loka_slicetype_decision(h);
}

void *uavs3e_create(enc_cfg_t *cfg, int *err)
{
    enc_ctrl_t *h;
    com_info_t *info;
    int pic_width, pic_height;

    printf("Version: %s_%s,  SHA-1: %s\n", VERSION_STR, VERSION_TYPE, VERSION_SHA1);

    h = (enc_ctrl_t *)com_malloc(sizeof(enc_ctrl_t));
    info = &h->info;

    refine_input_cfg(&h->cfg, cfg);
    memcpy(cfg, &h->cfg, sizeof(enc_cfg_t));

    pic_width  = info->pic_width  = h->cfg.pic_width;
    pic_height = info->pic_height = h->cfg.pic_height;

    info->wpp_threads         = h->cfg.wpp_threads;
    info->frm_threads         = h->cfg.frm_threads;
    info->max_cuwh            = h->cfg.ctu_size;
    info->log2_max_cuwh       = CONV_LOG2(info->max_cuwh);
    info->pic_width_in_lcu    = (pic_width  + info->max_cuwh - 1) >> info->log2_max_cuwh;
    info->pic_height_in_lcu   = (pic_height + info->max_cuwh - 1) >> info->log2_max_cuwh;
    info->f_lcu               = info->pic_width_in_lcu * info->pic_height_in_lcu;
    info->cus_in_lcu          = (info->max_cuwh >> MIN_CU_LOG2) * (info->max_cuwh >> MIN_CU_LOG2);
    info->pic_width_in_scu    = (pic_width  + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2;
    info->pic_height_in_scu   = (pic_height + ((1 << MIN_CU_LOG2) - 1)) >> MIN_CU_LOG2;
    info->i_scu               = info->pic_width_in_scu + 2;
    info->f_scu               = info->i_scu * (info->pic_height_in_scu + 2);
    info->log2_lcuwh_in_scu   = (u8)info->log2_max_cuwh - MIN_CU_LOG2;
    info->bit_depth_internal  = h->cfg.bit_depth_internal;
    info->gop_size            = h->cfg.max_b_frames + 1;
    info->max_b_frames        = h->cfg.max_b_frames;
    info->bit_depth_input     = h->cfg.bit_depth_input;
    info->qp_offset_bit_depth = (8 * (info->bit_depth_internal - 8));

    set_sqh(h, &h->info.sqh);

    h->prev_ptr  = -1;
    h->lastI_ptr = -1;

    com_refm_create(&h->rpm, MAX_PB_SIZE + h->cfg.frm_threads, info->pic_width, info->pic_height);

    h->ilist_size = h->cfg.lookahead + h->cfg.frm_threads + 1 /* lastIP */; 
    h->ilist_imgs = com_malloc(sizeof(com_img_t*) * h->ilist_size);

    h->img_rlist = com_malloc(sizeof(analyze_node_t) * h->cfg.lookahead);

    h->pic_thd_params = (pic_thd_param_t*)com_malloc(sizeof(pic_thd_param_t) * h->cfg.frm_threads);

    for (int i = 0; i < h->cfg.frm_threads; i++) {
        h->pic_thd_params[i].bs_buf = com_malloc(MAX_BS_BUF);
    }

    h->pic_thd_head   = 0;
    h->pic_thd_tail   = 0;
    h->pic_thd_active = 0;

    h->tab_mvbits_offset = inter_search_create(&h->tab_mvbits, info);
    h->info.tab_mvbits = h->tab_mvbits;
    inter_search_init(&h->pinter, info, 0);

    uavs3e_threadpool_init(&h->frm_threads_pool, h->cfg.frm_threads, h->cfg.frm_threads, (void * (*)(void *))pic_enc_alloc, &h->info, (void(*)(void *))pic_enc_free);

    rc_init(&h->rc, &h->cfg);

#if defined(ENABLE_FUNCTION_C)
    uavs3e_funs_init_c();
#endif

    uavs3e_funs_init_sse();
    if (uavs3e_simd_avx_level(NULL) >= 2) {
        uavs3e_funs_init_avx2();
    }

    com_scan_tbl_init();
    com_dct_coef_create();
    rdoq_init_err_scale(h->cfg.bit_depth_internal);
    rdoq_init_prob_2_bits();

    return h;
}

void uavs3e_free(void *id)
{
    int ret;
    enc_ctrl_t *h = (enc_ctrl_t *)id;

    do { // flush
        enc_stat_t stat;
        stat.ext_info = NULL;
        stat.ext_info_buf_size = 0;
        stat.insert_idr = 0;
        ret = uavs3e_enc(id, &stat, NULL);
    } while (ret == COM_OK);

    com_assert(h);
    com_refm_free(&h->rpm);

    if (h->frm_threads_pool) {
        uavs3e_threadpool_delete(h->frm_threads_pool);
    }

    if (h->pic_thd_params) {
        for (int i = 0; i < h->cfg.frm_threads; i++) {
            if (h->pic_thd_params[i].bs_buf) {
                com_mfree(h->pic_thd_params[i].bs_buf);
            }
        }
        com_mfree(h->pic_thd_params);
    }
    for (int i = 0; i < h->ilist_size; i++) {
        if (h->ilist_imgs[i]) {
            com_img_release(h->ilist_imgs[i]);
        }
    }
    rc_destroy(&h->rc);
    inter_search_free(h->tab_mvbits, h->tab_mvbits_offset);

    com_mfree(h->ilist_imgs);
    com_mfree(h->img_rlist);

    com_mfree(h);
    com_scan_tbl_delete();
}

static int start_one_frame(enc_ctrl_t *h)
{
    int ret;

    /* initialize variables for a picture encoding */
    input_node_t node = h->node_list[0];
    com_pic_t *pic_rec = com_refm_find_free_pic(&h->rpm, node.b_ref, &ret);

    if (ret != COM_OK) {
        return ret;
    }

    for (int i = 0; i < h->node_size - 1; i++) {
        h->node_list[i] = h->node_list[i + 1];
    }
    memset(&h->node_list[h->node_size - 1], 0, sizeof(input_node_t));
    h->node_size--;
    
    com_img_t *img_org = node.img;
    com_pic_t pic_org;

    com_mset(&pic_org, 0, sizeof(com_pic_t));
    pic_org.y             = img_org->planes[0];
    pic_org.u             = img_org->planes[1];
    pic_org.v             = img_org->planes[2];
    pic_org.width_luma    = img_org->width [0];
    pic_org.height_luma   = img_org->height[0];
    pic_org.width_chroma  = img_org->width [1];
    pic_org.height_chroma = img_org->height[1];
    pic_org.stride_luma   = STRIDE_IMGB2PIC(img_org->stride[0]);
    pic_org.stride_chroma = STRIDE_IMGB2PIC(img_org->stride[1]);
    pic_org.img           = img_org;
    pic_org.layer_id      = node.layer_id;
    pic_org.b_ref         = node.b_ref;

    com_info_t *info         = &h->info;
    com_seqh_t  *sqh         = &info->sqh;
    com_pic_header_t *pichdr = &h->pichdr;

    pichdr->slice_type = node.type;
    pichdr->poc = node.img->ptr;
    pichdr->dtr = h->dtr++;

    /* Set picture header */
    pic_rec->b_ref    = pic_org.b_ref;
    pic_rec->layer_id = pic_org.layer_id;
    pic_rec->ptr      = img_org->ptr;
    pic_rec->dtr      = (s32)(h->pichdr.dtr % DOI_CYCLE_LENGTH);
    pic_rec->end_line = 0;

    set_pic_header(h, pic_rec);

    /* encode one picture */
    if (h->prev_ptr >= 0) {
        img_org->dts = img_org->pts + (pichdr->dtr - img_org->ptr) * (img_org->pts - h->prev_pts) / (img_org->ptr - h->prev_ptr);
    } else {
        img_org->dts = 0;
    }
    h->prev_dtr = pichdr->dtr % DOI_CYCLE_LENGTH;
    h->prev_ptr = img_org->ptr;
    h->prev_pts = img_org->pts;

    pic_thd_param_t *pic_thd_param = &h->pic_thd_params[h->pic_thd_head];
    pic_thd_param->param      = &h->cfg;
    pic_thd_param->pic_rec    = pic_rec;
    pic_thd_param->ptr        = img_org->ptr;
    pic_thd_param->rc         = &h->rc;
    pic_thd_param->top_pic[0] = h->top_pic[0];
    pic_thd_param->top_pic[1] = h->top_pic[1];

    memcpy(&pic_thd_param->pic_org,  &pic_org,         sizeof(com_pic_t));
    memcpy(&pic_thd_param->pichdr,   &h->pichdr,       sizeof(com_pic_header_t));
    memcpy(&pic_thd_param->num_refp, &h->rpm.num_refp, sizeof(h->rpm.num_refp));
    memcpy(&pic_thd_param->refp,     &h->refp,         sizeof(h->refp));

    uavs3e_threadpool_run(h->frm_threads_pool, (void*(*)(void *, void*))enc_pic_thread, pic_thd_param, 1);

    h->pic_thd_head = (h->pic_thd_head + 1) % h->cfg.frm_threads;
    h->pic_thd_active++;

    return ret;
}

int uavs3e_enc(void *id, enc_stat_t *stat, com_img_t *img_enc)
{
    int ret;
    enc_ctrl_t *h = (enc_ctrl_t *)id;

    if (img_enc) {
        enc_push_frm(h, img_enc, stat->insert_idr);

        /* store input picture and return if needed */
        if (h->ptr < h->cfg.lookahead && h->cfg.max_b_frames) {
            return COM_OK_OUT_NOT_AVAILABLE;
        }
    } else { // flush
        if (h->img_rsize && !h->node_size) {
            loka_slicetype_decision(h);
        }

        while (h->pic_thd_active < h->cfg.frm_threads - 1 && (h->img_rsize || h->node_size)) {
            if (h->img_rsize && !h->node_size) {
                loka_slicetype_decision(h);
            }
            ret = start_one_frame(h);
        }

        /* check whether input pictures are remaining or not in node_input[] */
        if (!h->node_size) { // bumping
            if (h->pic_thd_active) {
                pic_thd_param_t *pic_thd_param = &h->pic_thd_params[h->pic_thd_tail];
                h->pic_thd_tail = (h->pic_thd_tail + 1) % h->cfg.frm_threads;
                uavs3e_threadpool_wait(h->frm_threads_pool, pic_thd_param);
                enc_pic_finish(h, pic_thd_param, stat);
                h->pic_thd_active--;

                if (h->cfg.rc_type != RC_TYPE_NULL) {
                    rc_update(&h->rc, pic_thd_param->pic_rec, stat->ext_info, stat->ext_info_buf_size);
                }
                return COM_OK;
            } else {
                return COM_OK_NO_MORE_FRM;
            }
        }
    }

    assert(h->node_size > 0);

    ret = start_one_frame(h);

    if (h->pic_thd_active == h->cfg.frm_threads) {
        pic_thd_param_t *pic_thd_param = &h->pic_thd_params[h->pic_thd_tail];
        h->pic_thd_tail = (h->pic_thd_tail + 1) % h->cfg.frm_threads;
        uavs3e_threadpool_wait(h->frm_threads_pool, pic_thd_param);
        enc_pic_finish(h, pic_thd_param, stat);

        h->pic_thd_active--;

        if (h->cfg.rc_type != RC_TYPE_NULL) {
            rc_update(&h->rc, pic_thd_param->pic_rec, stat->ext_info, stat->ext_info_buf_size);
        }
        return COM_OK;
    } else {
        return COM_OK_OUT_NOT_AVAILABLE;
    }
}

void uavs3e_load_default_cfg(enc_cfg_t *cfg)
{
    memset(cfg, 0, sizeof(enc_cfg_t));

    //#=========== Misc. ===============================
    cfg->use_pic_sign        =   1;
    cfg->bit_depth_internal  =   8;
    cfg->chroma_format       =   1;

    //#========== speed/quality trade-off ==============
    cfg->speed_level         =   1;

    //#========== parallel configuration ===============
    cfg->wpp_threads         =   1;
    cfg->frm_threads         =   1;

    //#=========== split configuration =================
    cfg->ctu_size            = 128;
    cfg->min_cu_size         =   4;
    cfg->max_part_ratio      =   8;
    cfg->max_split_times     =   6;
    cfg->min_qt_size         =   8;
    cfg->max_bt_size         = 128;
    cfg->max_eqt_size        =  64;
    cfg->max_dt_size         =  64;

    //#======== Coding Structure =======================
    cfg->i_period            =  64;
    cfg->max_b_frames        =  15;
    cfg->close_gop           =   0;
    cfg->scenecut            =   0;
    cfg->adaptive_gop        =   0;
    cfg->lookahead           =  40;

    //#========== Rate Control =========================
    cfg->rc_type             =    0;
    cfg->rc_bitrate          = 3000; 
    cfg->rc_max_bitrate      =    0;
    cfg->rc_min_qp           =    0;
    cfg->rc_max_qp           =   63;
    cfg->rc_crf              =   34;

    cfg->qp                  =   34;
    cfg->qp_offset_cb        =    0;
    cfg->qp_offset_cr        =    0;

    //#=========== Coding Tools ========================
    cfg->amvr_enable         =   1;
    cfg->affine_enable       =   1;
    cfg->smvd_enable         =   1;
    cfg->num_of_hmvp         =   8;
    cfg->ipf_flag            =   1;
    cfg->tscpm_enable        =   1;
    cfg->umve_enable         =   1;
    cfg->emvr_enable         =   1;
    cfg->dt_enable           =   1;
    cfg->sectrans_enable     =   1;
    cfg->pbt_enable          =   1;
    cfg->use_deblock         =   1;
    cfg->sao_enable          =   1;
    cfg->alf_enable          =   1;

    //#=========== weight quant ========================
    cfg->wq_enable           =   0;
    cfg->seq_wq_mode         =   0;
    cfg->pic_wq_data_idx     =   1;
    strcpy(cfg->seq_wq_user, "[64,64,64,68,64,64,68,72,64,68,76,80,72,76,84,96,64,64,64,64,68,68,72,76,64,64,64,68,72,76,84,92,64,64,68,72,76,80,88,100,64,68,72,80,84,92,100,112,68,72,80,84,92,104,112,128,76,80,84,92,104,116,132,152,96,100,104,116,124,140,164,188,104,108,116,128,152,172,192,216]");
    strcpy(cfg->pic_wq_user, "[64,64,64,68,64,64,68,72,64,68,76,80,72,76,84,96,64,64,64,64,68,68,72,76,64,64,64,68,72,76,84,92,64,64,68,72,76,80,88,100,64,68,72,80,84,92,100,112,68,72,80,84,92,104,112,128,76,80,84,92,104,116,132,152,96,100,104,116,124,140,164,188,104,108,116,128,152,172,192,216]");
    cfg->wq_param = 0;
    cfg->wq_model = 1;
    strcpy(cfg->wq_param_detailed, "[64,49,53,58,58,64]");
    strcpy(cfg->wq_param_undetailed, "[67,71,71,80,80,106]");

    //#=========== patch ===============================
    cfg->filter_cross_patch  =   1;
    cfg->colocated_patch     =   0;
    cfg->patch_width         =   0;
    cfg->patch_height        =   0;

    //#======= other encoder-size tools ================
    cfg->chroma_dqp   = 1;
    cfg->adaptive_dqp = 0;

}
