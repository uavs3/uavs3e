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

#ifndef _COM_H_
#define _COM_H_

#include "com_define.h"
#include "com_tables.h"

/* picture store structure */
typedef struct uavs3e_com_pic_t {
    /*** yuv data info **/
    pel        *y, *u, *v;       /* Start address of Y/U/V component (except padding)                */
    int         stride_luma;     /* Stride of luma picture   */
    int         stride_chroma;   /* Stride of chroma picture */
    int         width_luma;      /* Width of luma picture    */
    int         height_luma;     /* Height of luma picture   */
    int         width_chroma;    /* Width of chroma picture  */
    int         height_chroma;   /* Height of chroma picture */
    int         padsize_luma;    /* padding size of luma     */
    int         padsize_chroma;  /* padding size of chroma   */
    com_img_t   *img;            /* image buffer             */

    /*** ref info **/
    s64         list_ptr[MAX_REFS];     /* [rec] */
    u8          b_ref;                  /* [rec] - 0: not used for reference buffer, reference picture type */
    s16         dtr;                    /* [rec] - decoding temporal reference of this picture, (-256 ~255) */
    s64         ptr;                    /* [rec] - playing  temporal reference of this picture              */
    u8          layer_id;               /* [org] - scalable layer id                                        */
    
    /*** extension info **/
    double      picture_satd;                   /* [org] */
    double      picture_satd_blur;              /* [rec] */
    int         picture_qp;                     /* [org] */
    double      picture_qp_real;                /* [rec] - real qp in bit depth of 8 */
    int         picture_bits;                   /* [rec] */

    /*** pic-level map **/
    u8         *mem_base;                       /* [rec] */
    s16       (*map_mv)[REFP_NUM][MV_D];        /* [rec] */
    s8        (*map_refi)[REFP_NUM];            /* [rec] */

    /*** frame parallel ***/
    int         finished_line;
    uavs3e_pthread_mutex_t mutex;
    uavs3e_pthread_cond_t  cv;

} com_pic_t;

/*****************************************************************************
 * picture manager for DPB in decoder and RPB in encoder
 *****************************************************************************/
typedef struct uavs3e_com_pic_manager_t {
    com_pic_t        **pic;                       /* picture store (including reference and non-reference) */
    com_pic_t         *pic_ref[MAX_REFS];         /* address of reference pictures */
    int                max_num_ref_pics;          /* maximum reference picture count */
    int                cur_num_ref_pics;          /* current count of available reference pictures in PB */
    int                num_refp[REFP_NUM];        /* number of reference pictures */
    int                ptr_next_output;           /* next output POC */
    int                ptr_increase;              /* POC increment */
    int                max_pb_size;               /* max number of picture buffer */

    int                pic_width;
    int                pic_height;
    int                pad_l;
    int                pad_c;
} com_pic_manager_t;

/* reference picture structure */
typedef struct uavs3e_com_ref_pic_t {
    com_pic_t   *pic;    /* address of reference picture */
    s64        ptr;    /* PTR of reference picture */
    s16(*map_mv)[REFP_NUM][MV_D];
    s8(*map_refi)[REFP_NUM];
    s64       *list_ptr;
} com_ref_pic_t;

typedef struct uavs3e_com_scu_t {
    u8 coded    : 1;
    u8 intra    : 1;
    u8 skip     : 1;
    u8 cbf      : 1;
    u8 affine   : 2;
    u8 tbpart   : 2;
} com_scu_t;

/*****************************************************************************
* map structure
*****************************************************************************/
typedef struct uavs3e_com_map_t {
    com_scu_t *map_scu;
    s8       (*map_split)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE];
    s16      (*map_mv   )[REFP_NUM][MV_D];
    s8       (*map_refi )[REFP_NUM];
    s8        *map_ipm;
    u32       *map_pos;
    u8        *map_qp;
    s8        *map_patch;
    u8        *map_edge;
} com_map_t;

typedef struct uavs3e_com_sao_param_t {
    int modeIdc; //NEW, MERGE, OFF
    int typeIdc; //NEW: EO_0, EO_90, EO_135, EO_45, BO. MERGE: left, above
    int bandIdx[4];
    int offset[5];
    int deltaband;
} com_sao_param_t;

typedef struct uavs3e_com_alf_pic_param_t {
    int alf_flag;
    int filters_per_group;
    int componentID;
    int filterPattern[NO_VAR_BINS];
    int coeffmulti[NO_VAR_BINS][ALF_MAX_NUM_COEF];
} com_alf_pic_param_t;

/*****************************************************************************
 * picture header
 *****************************************************************************/
typedef struct uavs3e_com_pic_header_t {
    u8               low_delay;  /* low delay flag, info from sqh */
    s64              poc;
    int              rpl_l0_idx;         //-1 means this slice does not use RPL candidate in SPS for RPL0
    int              rpl_l1_idx;         //-1 means this slice does not use RPL candidate in SPS for RPL1
    com_rpl_t        rpl_l0;
    com_rpl_t        rpl_l1;
    u8               num_ref_idx_active_override_flag;
    u8               ref_pic_list_sps_flag[2];

    u8               slice_type;
    u8               loop_filter_disable_flag;
    u32              bbv_delay;
    u16              bbv_check_time;

    u8               loop_filter_parameter_flag;
    int              alpha_c_offset;
    int              beta_offset;

    u8               chroma_quant_param_disable_flag;
    s8               chroma_quant_param_delta_cb;
    s8               chroma_quant_param_delta_cr;

    /*Flag and coeff for ALF*/
    int              tool_alf_on;
    int             *pic_alf_on;
    com_alf_pic_param_t *m_alfPictureParam;

    int              fixed_picture_qp_flag;
    int              random_access_decodable_flag;
    int              time_code_flag;
    int              time_code;
    s64              dtr;
    int              picture_output_delay;
    int              bbv_check_times;
    int              progressive_frame;
    int              picture_structure;
    int              top_field_first;
    int              repeat_first_field;
    int              top_field_picture_flag;
    int              affine_subblk_size_idx;
    int              pic_wq_enable;
    int              pic_wq_data_idx;
    int              wq_param;
    int              wq_model;
    int              wq_param_vector[6];
    u8               wq_4x4_matrix[16];
    u8               wq_8x8_matrix[64];
} com_pic_header_t;

typedef struct uavs3e_com_patch_header_t {
    u8               slice_sao_enable[N_C];
    u8               fixed_slice_qp_flag;
    u8               slice_qp;
} com_patch_header_t;

typedef struct uavs3e_com_part_info_t {
    u8 num_sub_part;
    int sub_x[MAX_NUM_PB]; //sub part x, y, w and h
    int sub_y[MAX_NUM_PB];
    int sub_w[MAX_NUM_PB];
    int sub_h[MAX_NUM_PB];
    int sub_scup[MAX_NUM_PB];
} com_part_info_t;

typedef struct uavs3e_com_motion_t {
    s16 mv[REFP_NUM][MV_D];
    s8 ref_idx[REFP_NUM];
} com_motion_t;

typedef s16 lbac_ctx_model_t;

/* context models for arithemetic coding */
typedef struct uavs3e_com_lbac_all_ctx_t {
    lbac_ctx_model_t   skip_flag[LBAC_CTX_SKIP_FLAG];
    lbac_ctx_model_t   skip_idx_ctx[LBAC_CTX_SKIP_IDX];
    lbac_ctx_model_t   direct_flag[LBAC_CTX_DIRECT_FLAG];
    lbac_ctx_model_t   umve_flag;
    lbac_ctx_model_t   umve_base_idx[LBAC_CTX_UMVE_BASE_IDX];
    lbac_ctx_model_t   umve_step_idx[LBAC_CTX_UMVE_STEP_IDX];
    lbac_ctx_model_t   umve_dir_idx[LBAC_CTX_UMVE_DIR_IDX];

    lbac_ctx_model_t   inter_dir[LBAC_CTX_INTER_DIR];
    lbac_ctx_model_t   intra_dir[LBAC_CTX_INTRA_DIR];
    lbac_ctx_model_t   pred_mode[LBAC_CTX_PRED_MODE];
    lbac_ctx_model_t   cons_mode[LBAC_CTX_CONS_MODE];
    lbac_ctx_model_t   ipf_flag[LBAC_CTX_IPF];
    lbac_ctx_model_t   refi[LBAC_CTX_REFI];
    lbac_ctx_model_t   mvr_idx[LBAC_CTX_MVR_IDX];
    lbac_ctx_model_t   affine_mvr_idx[LBAC_CTX_AFFINE_MVR_IDX];

    lbac_ctx_model_t   hmvp_flag[LBAC_CTX_EXTEND_AMVR_FLAG];
    lbac_ctx_model_t   mvd[2][LBAC_CTX_MVD];
    lbac_ctx_model_t   ctp_zero_flag[LBAC_CTX_CTP_ZERO_FLAG];
    lbac_ctx_model_t   cbf[LBAC_CTX_CBF];
    lbac_ctx_model_t   tb_split[LBAC_CTX_TB_SPLIT];
    lbac_ctx_model_t   run[LBAC_CTX_RUN];
    lbac_ctx_model_t   run_rdoq[LBAC_CTX_RUN];
    lbac_ctx_model_t   last1[LBAC_CTX_LAST1 * 2];
    lbac_ctx_model_t   last2[LBAC_CTX_LAST2 * 2 - 2];
    lbac_ctx_model_t   level[LBAC_CTX_LEVEL];
    lbac_ctx_model_t   split_flag[LBAC_CTX_SPLIT_FLAG];
    lbac_ctx_model_t   bt_split_flag[LBAC_CTX_BT_SPLIT_FLAG];
    lbac_ctx_model_t   split_dir[LBAC_CTX_SPLIT_DIR];
    lbac_ctx_model_t   split_mode[LBAC_CTX_SPLIT_MODE];
    lbac_ctx_model_t   affine_flag[LBAC_CTX_AFFINE_FLAG];
    lbac_ctx_model_t   affine_mrg_idx[LBAC_CTX_AFFINE_MRG];
    lbac_ctx_model_t   smvd_flag[LBAC_CTX_SMVD_FLAG];
    lbac_ctx_model_t   part_size[LBAC_CTX_PART_SIZE];
    lbac_ctx_model_t   sao_merge_flag[LBAC_CTX_SAO_MERGE_FLAG];
    lbac_ctx_model_t   sao_mode[LBAC_CTX_SAO_MODE];
    lbac_ctx_model_t   sao_offset[LBAC_CTX_SAO_OFFSET];
    lbac_ctx_model_t   alf_lcu_enable[LBAC_CTX_ALF];
    lbac_ctx_model_t   lcu_qp_delta[LBAC_CTX_DELTA_QP];
} com_lbac_all_ctx_t;

typedef struct uavs3e_com_sao_stat_t {
    long long int diff[MAX_NUM_SAO_CLASSES];
    int count[MAX_NUM_SAO_CLASSES];
} com_sao_stat_t;

/*****************************************************************************
 * sequence header
 *****************************************************************************/
typedef struct uavs3e_com_seqh_t {
    u8               profile_id;                
    u8               level_id;                  
    u8               progressive_sequence;      
    u8               field_coded_sequence;      
    u8               chroma_format;             
    u8               encoding_precision;        
    u8               output_reorder_delay;      
    u8               sample_precision;          
    u8               aspect_ratio;              
    u8               frame_rate_code;           
    u32              bit_rate_lower;            
    u32              bit_rate_upper;            
    u8               low_delay;                 
    u8               temporal_id_enable_flag;   
    u32              bbv_buffer_size;           
    int              horizontal_size;           
    int              vertical_size;             
    u8               log2_max_cuwh;  
    u8               min_cu_size;
    u8               max_part_ratio;
    u8               max_split_times;
    u8               min_qt_size;
    u8               max_bt_size;
    u8               max_eqt_size;
    u8               max_dt_size;

    int              rpl1_index_exist_flag;
    int              rpl1_same_as_rpl0_flag;
    com_rpl_t        rpls_l0[MAX_RPLS];
    com_rpl_t        rpls_l1[MAX_RPLS];
    int              rpls_l0_num;
    int              rpls_l1_num;
    int              active_ref_minus1[2];
    int              max_dpb_size;

    int              ipcm_enable_flag;
    u8               amvr_enable;
    int              umve_enable;
    int              ipf_enable_flag;
    int              emvr_enable;

    u8               affine_enable;
    u8               smvd_enable;
    u8               dt_enable;

    u8               num_of_hmvp;
    u8               tscpm_enable;

    u8               sao_enable;
    u8               alf_enable;
    u8               sectrans_enable;
    u8               pbt_enable;

    u8               wq_enable;
    u8               seq_wq_mode;
    u8               wq_4x4_matrix[16];
    u8               wq_8x8_matrix[64];

    u8               filter_cross_patch;
    u8               colocated_patch;
    u8               patch_width;
    u8               patch_height;
    u8               patch_columns;
    u8               patch_rows;
    int              column_width[64];
    int              row_height[32];

} com_seqh_t;






/*****************************************************************************
* mode decision structure
*****************************************************************************/
typedef struct uavs3e_com_mode_t {
    int  cu_mode;
    //note: DT can apply to intra CU and only use normal amvp for inter CU (no skip, direct, amvr, affine, hmvp)
    part_size_t  pb_part;
    part_size_t  tb_part;
    com_part_info_t  pb_info;
    com_part_info_t  tb_info;

    ALIGNED_32(pel rec[N_C][MAX_CU_DIM]);
    ALIGNED_32(s16 coef[N_C][MAX_CU_DIM]);
    ALIGNED_32(pel pred[N_C][MAX_CU_DIM]);

    int  num_nz[MAX_NUM_TB][N_C];

    /* reference indices */
    s8   refi[REFP_NUM];

    /* MVR indices */
    u8   mvr_idx;
    u8   umve_flag;
    u8   umve_idx;
    u8   skip_idx;
    u8   hmvp_flag;

    /* mv difference */
    s16  mvd[REFP_NUM][MV_D];
    /* mv */
    s16  mv[REFP_NUM][MV_D];

    u8   affine_flag;
    CPMV affine_mv[REFP_NUM][VER_NUM][MV_D];
    s16  affine_mvd[REFP_NUM][VER_NUM][MV_D];
    u8   smvd_flag;

    /* intra prediction mode */
    u8   mpm[MAX_NUM_PB][2];
    s8   ipm[MAX_NUM_PB][2];
    u8   ipf_flag;
} com_mode_t;


/*****************************************************************************
* common info
*****************************************************************************/
typedef struct uavs3e_com_info_t {
    int                     wpp_threads;
    int                     frm_threads;

    /* sequence header */
    com_seqh_t              sqh;

    /* decoding picture width */
    int                     pic_width;
    /* decoding picture height */
    int                     pic_height;
    /* maximum CU width and height */
    int                     max_cuwh;
    /* log2 of maximum CU width and height */
    int                     log2_max_cuwh;
    /* number of cu in LCU */
    int                     cus_in_lcu;

    /* picture width in LCU unit */
    int                     pic_width_in_lcu;
    /* picture height in LCU unit */
    int                     pic_height_in_lcu;

    /* picture size in LCU unit (= w_lcu * h_lcu) */
    int                     f_lcu;
    /* picture width in SCU unit */
    int                     pic_width_in_scu;
    /* picture height in SCU unit */
    int                     pic_height_in_scu;
    /* picture size in SCU unit (= pic_width_in_scu * h_scu) */
    int                     f_scu;
    int                     i_scu;
    int                     bit_depth_internal;
    int                     bit_depth_input;
    int                     qp_offset_bit_depth;

    u8                      log2_lcuwh_in_scu;  /* log2 of SCU count in a LCU row */

    int                     max_b_frames;
    int                     gop_size;
} com_info_t;

#endif /* _COM_H_ */