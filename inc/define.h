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

#ifndef _ENC_DEF_H_
#define _ENC_DEF_H_

#include "com_modules.h"
#include "uavs3e.h"
#include "bitstream.h"

#define MAX_BS_BUF                        (32*1024*1024)
#define RDO_WITH_DBLOCK                            1 // include DBK changes to luma samples into distortion
#define DT_INTRA_BOUNDARY_FILTER_OFF        1 ///< turn off boundary filter if intra DT is ON
#define MAX_INTER_SKIP_RDO                 MAX_SKIP_NUM
#define THRESHOLD_MVPS_CHECK               1.1
#define NUM_SL_INTER                       10
#define NUM_SL_INTRA                       8
#define INC_QT_DEPTH(qtd, smode)           (smode == SPLIT_QUAD? (qtd + 1) : qtd)
#define INC_BET_DEPTH(betd, smode)         (smode != SPLIT_QUAD? (betd + 1) : betd)

//fast algorithm (common)
#define TR_SAVE_LOAD                       1  // fast algorithm for PBT
#define TR_EARLY_TERMINATE                 1  // fast algorithm for PBT
#define DT_INTRA_FAST_BY_RD                1  // fast algorithm: early skip based on RD cost comparison
#define DT_SAVE_LOAD                       1  // fast algorithm: save & load best part_size
#define ENC_ECU_DEPTH                      4 // for early CU termination
#define ENC_ECU_ADAPTIVE                   1 // for early CU termination

/* maximum pico_img count */
#define ENC_MAX_INPUT_BUF      32
#define MAX_REORDER_BUF        33

/* maximum cost value */
#define MAX_COST                (1.7e+308)
#define MAX_COST_EXT            (MAX_COST * 0.999999999)

/* virtual frame depth B picture */
#define FRM_DEPTH_0                   0 // only for I frame
#define FRM_DEPTH_1                   1
#define FRM_DEPTH_2                   2
#define FRM_DEPTH_3                   3
#define FRM_DEPTH_4                   4
#define FRM_DEPTH_5                   5
#define FRM_DEPTH_MAX                 FRM_DEPTH_5



typedef struct uavs3e_enc_rc_handle_t enc_rc_handle_t;
typedef struct uavs3e_enc_ctrl_t enc_ctrl_t;

typedef void*(*rc_frame_init_t   )(enc_cfg_t *param);
typedef int  (*rc_frame_get_qp_t )(void *rc_handle, enc_ctrl_t *h, com_pic_t *pic);
typedef void (*rc_frame_update_t )(void *rc_handle, com_pic_t *pic, char *ext_info, int info_buf_size);
typedef void (*rc_frame_destroy_t)(void *rc_handle);

struct uavs3e_enc_rc_handle_t {
    int type;
    void *handle;
    rc_frame_init_t    init;
    rc_frame_get_qp_t  get_qp;
    rc_frame_update_t  update;
    rc_frame_destroy_t destroy;
};

/*****************************************************************************
 * input picture buffer structure
 *****************************************************************************/
typedef struct uavs3e_input_node_t {
    com_img_t *img;      /* original picture store     */
    int        b_ref;
    int        layer_id;
    int        type;
} input_node_t;

/*****************************************************************************
 * inter prediction structure
 *****************************************************************************/

#define MV_RANGE_MIN           0
#define MV_RANGE_MAX           1
#define MV_RANGE_DIM           2

typedef struct uavs3e_enc_inter_data_t {
    /*** sequence level ***/
    int bit_depth;
    int gop_size;                 /* gop size           */
    int max_search_range;
    s16 min_mv_offset[MV_D];      /* minimum clip value */
    s16 max_mv_offset[MV_D];      /* maximum clip value */
    s16 max_mv_pos   [MV_D];      /* maximum MV */

    /*** picture level ***/
    s64  ptr;                      /* current frame numbser */
    s16(*map_mv)[REFP_NUM][MV_D];  /* motion vector map     */
    com_ref_pic_t(*refp)[REFP_NUM];     /* reference pictures    */

    /*** local data ***/
    u8   num_refp;
    u8   curr_mvr;
    pel *org;
    int  i_org;
    u32  lambda_mv;

    s32  mot_bits         [REFP_NUM];
    s16  mvp_scale        [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];
    s16  mv_scale         [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];
    CPMV affine_mvp_scale [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][VER_NUM][MV_D];
    CPMV affine_mv_scale  [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][VER_NUM][MV_D];
    int  best_mv_uni      [REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D];

    u8 *tab_mvbits;
    int tab_mvbits_offset;
} inter_search_t;

typedef struct uavs3e_lbac_t {
    u32            range;
    u32            code;
    int            left_bits;
    u32            stacked_ff;
    u32            pending_byte;
    u32            is_pending_byte;
    com_lbac_all_ctx_t   h;
    u32            bitcounter;
} lbac_t;


typedef struct uavs3e_enc_aqp_param_t {
    int qp_offset_layer;
    double qp_offset_model_offset;
    double qp_offset_model_scale;
} enc_aqp_param_t;

typedef struct uavs3e_enc_cu_t {
    com_scu_t *map_scu;
    s8      *ipm_l;
    s16    (*mv)[REFP_NUM][MV_D];
    s8     (*refi)[REFP_NUM];
    u32     *map_pos;

    s8  (*split_mode)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE];
    part_size_t *pb_part;
    part_size_t *tb_part;
    u8  *pred_mode;
    u8  (*mpm)[2];
    s8  *ipm_c;
    u8  *mvr_idx;
    u8  *umve_flag;
    u8  *umve_idx;
    u8  *skip_idx;
    u8  *hmvp_flag;
    s16 (*mvd)[REFP_NUM][MV_D];
    int *num_nz_coef[N_C];
    u8  *affine_flag;
    u8  *smvd_flag;
    u8 *ipf_flag;
    s16 *coef[N_C];
    pel *reco[N_C];
} enc_cu_t;

typedef struct uavs3e_enc_history_t {
    int    visit;
    int    nosplit;
    int    split;
    int    split_visit;
    double split_cost[NUM_SPLIT_MODE];
    int    mvr_idx_history;
    int    affine_flag_history;
    int    mvr_hmvp_idx_history;
#if TR_SAVE_LOAD
    u8     num_inter_pred;
    u16    inter_pred_dist[NUM_SL_INTER];
    u8     inter_tb_part[NUM_SL_INTER];  // luma TB part size for inter prediction block
#endif
#if DT_SAVE_LOAD
    u8     best_part_size_intra[2];
    u8     num_intra_history;
#endif
} enc_history_t;

typedef struct uavs3e_enc_alf_corr_t {
    double ECorr[NO_VAR_BINS][ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF];  //!< auto-correlation matrix
    double yCorr[NO_VAR_BINS][ALF_MAX_NUM_COEF]; //!< cross-correlation
    double pixAcc[NO_VAR_BINS];
    int componentID;
} enc_alf_corr_t;

typedef struct uavs3e_enc_alf_var_t {
    enc_alf_corr_t (*m_alfCorr)[N_C];
    BOOL(*m_AlfLCUEnabled)[N_C];

    double    m_y_merged[NO_VAR_BINS][ALF_MAX_NUM_COEF];
    double    m_E_merged[NO_VAR_BINS][ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF];
    double    m_y_temp[ALF_MAX_NUM_COEF];
    double    m_pixAcc_merged[NO_VAR_BINS];
    double    m_E_temp[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF];
    com_alf_pic_param_t  m_alfPictureParam[N_C];
    int       m_coeffNoFilter[ALF_MAX_NUM_COEF];
    int       m_varIndTab[NO_VAR_BINS];

    unsigned int m_uiBitIncrement;

} enc_alf_var_t;

typedef struct uavs3e_enc_pic_param_t {
    enc_cfg_t          *param;
    com_pic_header_t   *pichdr;
    com_patch_header_t *pathdr;
    com_info_t         *info;
    com_map_t          *map;
    enc_cu_t           *map_cu_data;
    com_pic_t          *pic_org;
    com_pic_t          *pic_rec;
    com_sao_param_t   (*sao_blk_params)[N_C];
    pel                *linebuf_sao[3];
    com_ref_pic_t     (*refp)[REFP_NUM];                          /* reference picture (0: foward, 1: backward) */
    s64                 ptr;
    int                *num_refp;                                      /* reference picture manager */
    pel              *(*linebuf_intra)[3];
} enc_pic_param_t;

typedef struct uavs3e_enc_lcu_row_t {
    int            lcu_y;
    lbac_t     sbac_row;
    lbac_t    *sbac_row_next;
    uavs3e_sem_t   sem;
    uavs3e_sem_t  *sem_up;
    uavs3e_sem_t  *sem_curr;
    enc_pic_param_t     *pic_info;
    int           total_qp;

} enc_lcu_row_t;

/*****************************************************************************
 * CORE information used for encoding process.
 *
 * The variables in this structure are very often used in encoding process.
 *****************************************************************************/
typedef struct uavs3e_core_t {
    /*** frame level, copy from enc_ctrl_t ***/
    enc_cfg_t          *param;
    com_info_t         *info;
    com_pic_header_t   *pichdr;
    com_patch_header_t *pathdr;
    com_map_t          *map;
    enc_cu_t           *map_cu_data;
    com_pic_t          *pic_org;
    com_pic_t          *pic_rec;
    com_sao_param_t   (*sao_blk_params)[N_C];                     //[SMB][comp]
    pel                *linebuf_sao[3];
    com_ref_pic_t     (*refp)[REFP_NUM];                             /* reference picture (0: foward, 1: backward) */
    s64                 ptr;
    int                *num_refp;                                      /* reference picture manager */
    u8                  slice_type;
    u8                 *wq[2];


    /*** frame level ***/
    com_motion_t     motion_cands[ALLOWED_HMVP_NUM];
    s8               cnt_hmvp_cands;
    pel             *linebuf_intra[2][3]; // [0: curr row,  1: last row][y/u/v]

    /*** LCU row level ***/
    pel          *sao_src_buf[3];

    /*** LCU level ***/
    int            lcu_x;
    int            lcu_y;
    int            lcu_pix_x;
    int            lcu_pix_y;
    int            lcu_qp_y;
    int            lcu_qp_u;
    int            lcu_qp_v;
    double         lambda[3];
    double         sqrt_lambda[3];
    double         dist_chroma_weight[2];
    enc_cu_t       cu_data_best[MAX_CU_DEPTH][MAX_CU_DEPTH];
    enc_cu_t       cu_data_temp[MAX_CU_DEPTH][MAX_CU_DEPTH];
    enc_history_t   bef_data[MAX_CU_DEPTH][MAX_CU_DEPTH][MAX_CU_CNT_IN_LCU];

    u8             tree_status;
    u8             cons_pred_mode;


    /***  CU level ***/
    int            cu_width;
    int            cu_height;
    int            cu_width_log2;
    int            cu_height_log2;
    int            cu_scu_x;
    int            cu_scu_y;
    int            cu_pix_x;
    int            cu_pix_y;
    int            cu_scup_in_pic;
    int            cu_scup_in_lcu;

    u8             skip_flag;
    u8             split_flag;
    int            best_pb_part_intra;
    int            best_tb_part_intra;
    pel            nb[N_C][INTRA_NEIB_SIZE];
    s16            coef[N_C][MAX_CU_DIM];
    s16            ctmp[N_C][MAX_CU_DIM];

    lbac_t     sbac_rdo;
    lbac_t     sbac_bakup; // h before mode decision
    lbac_t     sbac_prev_intra_pu;
    lbac_t     sbac_tree_c;

    s32            rdoq_bin_est_ctp[2];
    s32            rdoq_bin_est_cbf[LBAC_CTX_CBF][2];
    s32            rdoq_bin_est_run[LBAC_CTX_RUN][2];
    s32            rdoq_bin_est_lvl[LBAC_CTX_LEVEL][2];
    s32            rdoq_bin_est_lst[2][LBAC_CTX_LAST1][LBAC_CTX_LAST2][2];

    com_mode_t     mod_info_best;
    com_mode_t     mod_info_curr;

    inter_search_t     pinter;  /* inter prediction analysis */

    pel            intra_pred_all[IPD_CNT][MAX_CU_DIM]; // only for luma

    double         cost_best;

    /*** for fast algorithm ***/
    u32            inter_satd;
    s32            dist_cu;
    s32            dist_cu_best; //dist of the best intra mode (note: only updated in intra coding now)
    u8             skip_mvps_check;
#if TR_SAVE_LOAD
    u8             best_tb_part_hist;
#endif
#if TR_EARLY_TERMINATE
    s64            dist_pred_luma;
#endif
} core_t;

typedef struct uavs3e_enc_pic_t {
    threadpool_t *wpp_threads_pool;
    com_map_t    map;
    com_info_t   info;

    u8           *bs_buf_demulate;

    pel        *(*linebuf_intra)[3];
    pel          *linebuf_sao[3];

    enc_cu_t     *map_cu_data;

    int             pic_alf_on[N_C];
    enc_alf_var_t   Enc_ALF;
    com_pic_t      *pic_alf_Rec;
    u8             *alf_var_map;
    com_sao_param_t(*sao_blk_params)[N_C];                        //[SMB][comp]

    enc_lcu_row_t  *array_row;
    core_t     *main_core;     /* for SBAC */

} enc_pic_t;

typedef struct uavs3e_pic_thd_param_t {
    com_pic_t         pic_org;
    com_pic_t        *pic_rec;
    enc_cfg_t        *param;
    com_pic_header_t  pichdr;
    int               num_refp[REFP_NUM];        /* number of reference pictures */
    com_ref_pic_t     refp[MAX_REFS][REFP_NUM];  /* reference picture (0: foward, 1: backward) */
    s64               ptr;                       /* current picture's presentation temporal reference */

    /*** coding results ***/
    int           total_bytes;
    int           user_bytes;
    int           total_qp;
    u8           *bs_buf;
} pic_thd_param_t;

/******************************************************************************
 * CONTEXT used for encoding process.
 *
 * All have to be stored are in this structure.
 *****************************************************************************/
struct uavs3e_enc_ctrl_t {
    enc_cfg_t cfg;                                   /* encoding parameter */

    com_img_t        *img_ilist[MAX_REORDER_BUF];
    int               img_isize;
    input_node_t      node_list[MAX_REORDER_BUF];
    int               node_size;
    long long         lastI_ptr;

    int               ilist_size;       
    com_img_t       **ilist_imgs;                    /* image buffer for input, include used and idle */
                      
    u8                prev_dtr;                      /* dtr % DOI_CYCLE_LENGTH */
    s64               dtr;                           /* index of decoder order */
    s64               ptr;                           /* index of play order    */
                      
    s64               prev_pts;                      /* used to calculate dts */
    s64               prev_ptr;                      /* used to calculate dts */
                      
    /*** copy to enc_pic_t ***/
    com_info_t        info;
    com_pic_header_t  pichdr;
    com_ref_pic_t     refp[MAX_REFS][REFP_NUM];      /* reference picture (0: foward, 1: backward) */
    com_pic_manager_t rpm;                           /* reference picture manager */

    /*** parallel data ***/
    threadpool_t   *frm_threads_pool;
    pic_thd_param_t *pic_thd_params;
    int              pic_thd_idx;
    int              pic_thd_active;

    /*** RC data ***/
    inter_search_t preprocess_pinter;
    enc_rc_handle_t rc;
};

int  enc_pic_finish (enc_ctrl_t *h, pic_thd_param_t *pic_thd, enc_stat_t *stat);

#include "analyze.h"
#include "util.h"
#include "lookahead.h"

#endif /* _ENC_DEF_H_ */