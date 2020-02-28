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

#ifndef _ENC_API_H_
#define _ENC_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32) && !defined(__GNUC__)

#ifdef UAVS3E_EXPORTS
#define UAVS3E_API __declspec(dllexport)
#else
#define UAVS3E_API __declspec(dllimport)
#endif

#else

#define UAVS3E_API
#define __cdecl

#endif

#include "com_api.h"

#define PIC_ALIGN_SIZE 8

typedef struct uavs3e_enc_stat_t {
    void          *buf;                // [ in] bitstream buffer                                       
    int            bytes;              // [out] size of bitstream
    int            user_bytes;
    int            bytes_seq_hdr;
    long long      pts;                // [out] pts  of current frame
    long long      dts;                // [out] dts  of current frame
    int            type;               // [out] picture type 
    int            qp;                 // [out] qp of current frame
    long long      poc;                // [out] poc of current frame
    int            refpic_num[2];      // [out] 
    long long      refpic[2][16];      // [out] 
    com_img_t     *rec_img;            // [out]
    com_img_t     *org_img;            // [out]

    char          *ext_info;           // [out] info buffer for log
    int            ext_info_buf_size;  // [out] info buffer size
} enc_stat_t;

typedef struct uavs3e_enc_cfg_t {
    int            horizontal_size;
    int            vertical_size;
    int            pic_width;
    int            pic_height;
    int            qp;
    int            fps_num;
    int            fps_den;
    int            i_period;
    int            bit_depth_input;
    int            bit_depth_internal;
    int            use_pic_sign;
    int            max_b_frames;
    int            disable_hgop;
    int            close_gop;
    int            amvr_enable;
    int            affine_enable;
    int            smvd_enable;
    int            use_deblock;
    int            num_of_hmvp;
    int            ipf_flag;
    int            tscpm_enable;
    int            umve_enable;
    int            emvr_enable;
    int            dt_enable;
    int            wq_enable;
    int            seq_wq_mode;
    char           seq_wq_user[2048];
    int            pic_wq_data_idx;
    char           pic_wq_user[2048];
    int            wq_param;
    int            wq_model;
    char           wq_param_detailed[256];
    char           wq_param_undetailed[256];
    int            sao_enable;
    int            alf_enable;
    int            sectrans_enable;
    int            pbt_enable;
    int            dqp_enable;
    int            chroma_format;
    com_rpl_t      rpls_l0[MAX_RPLS];
    com_rpl_t      rpls_l1[MAX_RPLS];
    int            rpls_l0_cfg_num;
    int            rpls_l1_cfg_num;
    int            filter_cross_patch;
    int            colocated_patch;
    int            patch_width;
    int            patch_height;
    int            ctu_size;
    int            min_cu_size;
    int            max_part_ratio;
    int            max_split_times;
    int            min_qt_size;
    int            max_bt_size;
    int            max_eqt_size;
    int            max_dt_size;
    int            qp_offset_cb;
    int            qp_offset_cr;

    int            wpp_threads;
    int            frm_threads;

    int            rc_type;
    int            rc_bitrate;
    int            rc_max_bitrate;
    int            rc_crf;
    int            rc_min_qp;
    int            rc_max_qp;
} enc_cfg_t;

typedef    void*  (__cdecl *uavs3e_create_t)(enc_cfg_t *param, int *err);
UAVS3E_API void*   __cdecl  uavs3e_create   (enc_cfg_t *param, int *err);

typedef    void   (__cdecl *uavs3e_free_t)(void *id);
UAVS3E_API void    __cdecl  uavs3e_free   (void *id);

typedef    int    (__cdecl *uavs3e_enc_t)(void *id, enc_stat_t *stat, com_img_t *img_enc);
UAVS3E_API int     __cdecl  uavs3e_enc   (void *id, enc_stat_t *stat, com_img_t *img_enc);

typedef    int    (__cdecl *uavs3e_get_img_t)(void *id, com_img_t **img);
UAVS3E_API int     __cdecl  uavs3e_get_img   (void *id, com_img_t **img);

#ifdef __cplusplus
}
#endif


#endif // #ifndef _ENC_API_H_