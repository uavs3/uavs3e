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

#ifndef _COM_DEF_H_
#define _COM_DEF_H_

#include "com_system.h"
#include "com_api.h"

/*************************  profile & level **********************************************/
#define PROFILE_ID  0x22
#define LEVEL_ID    0x6A

/* MCABAC (START) */
#define PROB_BITS                         11 // LPS_PROB(10-bit) + MPS(1-bit)
#define PROB_MASK                         ((1 << PROB_BITS) - 1) // mask for LPS_PROB + MPS
#define MAX_PROB                          ((1 << PROB_BITS) - 1) // equal to PROB_LPS + PROB_MPS, 0x7FF
#define HALF_PROB                         (MAX_PROB >> 1)
#define QUAR_HALF_PROB                    (1 << (PROB_BITS-3))
#define LG_PMPS_SHIFTNO                   2

#define PROB_INIT                         (HALF_PROB << 1) /* 1/2 of initialization = (HALF_PROB << 1)+ MPS(0) */

#define LBAC_CTX_SKIP_FLAG                  4
#define LBAC_CTX_SKIP_IDX                   (MAX_SKIP_NUM - 1)
#define LBAC_CTX_DIRECT_FLAG                2
#define LBAC_CTX_UMVE_BASE_IDX              1
#define LBAC_CTX_UMVE_STEP_IDX              1
#define LBAC_CTX_UMVE_DIR_IDX               2
#define LBAC_CTX_SPLIT_FLAG                 4
#define LBAC_CTX_SPLIT_MODE                 3
#define LBAC_CTX_BT_SPLIT_FLAG              9
#define LBAC_CTX_SPLIT_DIR                  5
#define LBAC_CTX_CBF                        3       /* number of context models for QT CBF */
#define LBAC_CTX_CTP_ZERO_FLAG              2       /* number of context models for ctp_zero_flag */
#define LBAC_CTX_PRED_MODE                  6       /* number of context models for prediction mode */
#define LBAC_CTX_CONS_MODE                  1       /* number of context models for constrained prediction mode */
#define LBAC_CTX_TB_SPLIT                   1
#define LBAC_CTX_DELTA_QP                   4
#define LBAC_CTX_INTER_DIR                  3       /* number of context models for inter prediction direction */
#define LBAC_CTX_REFI                       3
#define LBAC_CTX_IPF                        1
#define LBAC_CTX_MVR_IDX                    (MAX_NUM_MVR - 1)
#define LBAC_CTX_AFFINE_MVR_IDX             (MAX_NUM_AFFINE_MVR - 1)
#define LBAC_CTX_EXTEND_AMVR_FLAG           1
#define LBAC_CTX_MVD                        3       /* number of context models for motion vector difference */
#define LBAC_CTX_INTRA_DIR                  10
#define LBAC_CTX_AFFINE_FLAG                1
#define LBAC_CTX_AFFINE_MRG                 AFF_MAX_NUM_MRG - 1
#define LBAC_CTX_SMVD_FLAG                  1
#define LBAC_CTX_PART_SIZE                  6
#define LBAC_CTX_SAO_MERGE_FLAG             3
#define LBAC_CTX_SAO_MODE                   1
#define LBAC_CTX_SAO_OFFSET                 1
#define LBAC_CTX_ALF                        1

#define RANK_NUM 6
#define LBAC_CTX_LEVEL                      (RANK_NUM * 4)
#define LBAC_CTX_RUN                        (RANK_NUM * 4)
#define LBAC_CTX_RUN_RDOQ                   (RANK_NUM * 4)
#define LBAC_CTX_LAST1                       RANK_NUM
#define LBAC_CTX_LAST2                      12
#define LBAC_CTX_LAST                       2

/************************* virtual frame depth B *********************************/
#define FRM_DEPTH_0                   0 // only for I frame
#define FRM_DEPTH_1                   1
#define FRM_DEPTH_2                   2
#define FRM_DEPTH_3                   3

/* rpl structure */
#define MAX_RPLS  16
#define MAX_REFS  17
#define MAX_RA_ACTIVE 2
#define MAX_LD_ACTIVE 4

/************************* QUANT ************************************************/
#define MAX_QUANT_BASE                     63

/************************* AMVR ************************************************/
#define MAX_NUM_MVR                        5  /* 0 (1/4-pel) ~ 4 (4-pel) */
#define MAX_NUM_AFFINE_MVR                 3

/************************* DEBLOCK ************************************************/
#define EDGE_TYPE_LUMA                     0x1
#define EDGE_TYPE_ALL                      0x3
#define LOOPFILTER_GRID                    8
#define LOOPFILTER_GRID_C                  (LOOPFILTER_GRID << 1)

/************************* SAO ************************************************/
#define NUM_BO_OFFSET                      32
#define MAX_NUM_SAO_CLASSES                32
#define NUM_SAO_BO_CLASSES_LOG2            5
#define NUM_SAO_BO_CLASSES_IN_BIT          5
#define NUM_SAO_EO_TYPES_LOG2              2
#define NUM_SAO_BO_CLASSES                 (1<<NUM_SAO_BO_CLASSES_LOG2)
#define SAO_SHIFT_PIX_NUM                  4

/************************** ME ************************************************/
/* Define the Search Range for int-pel */
#define SEARCH_RANGE_IPEL_RA               384
#define SEARCH_RANGE_IPEL_LD               64

/* SAO_AVS2(START) */
typedef enum uavs3e_sao_mode_t {
    SAO_MODE_NEW,
    SAO_MODE_MERGE_LEFT,
    SAO_MODE_MERGE_ABOVE,
    NUM_SAO_MODES
} sao_mode_t;

typedef enum uavs3e_sao_merge_type_t {
    SAO_MERGE_LEFT = 0,
    SAO_MERGE_ABOVE,
    NUM_SAO_MERGE_TYPES
} sao_merge_type_t;

typedef enum uavs3e_sao_type_t {
    SAO_TYPE_EO_0,
    SAO_TYPE_EO_90,
    SAO_TYPE_EO_135,
    SAO_TYPE_EO_45,
    SAO_TYPE_BO,
    NUM_SAO_NEW_TYPES
} sao_type_t;

typedef enum uavs3e_sao_eo_class_t {
    SAO_CLASS_EO_FULL_VALLEY = 0,
    SAO_CLASS_EO_HALF_VALLEY = 1,
    SAO_CLASS_EO_PLAIN = 2,
    SAO_CLASS_EO_HALF_PEAK = 3,
    SAO_CLASS_EO_FULL_PEAK = 4,
    SAO_CLASS_BO = 5,
    NUM_SAO_EO_CLASSES = SAO_CLASS_BO,
    NUM_SAO_OFFSET
} sao_eo_class_t;

/************************* ALF ************************************************/
#define ALF_MAX_NUM_COEF                    9
#define NO_VAR_BINS                        16
#define ALF_NUM_BIT_SHIFT                   6
#define ALF_REDESIGN_ITERATION             3
#define LOG2_VAR_SIZE_H                    2
#define LOG2_VAR_SIZE_W                    2
#define ALF_FOOTPRINT_SIZE                 7
#define DF_CHANGED_SIZE                    3

/************************* AFFINE ************************************************/
#define VER_NUM                             4
#define AFFINE_MAX_NUM_LT                   3 ///< max number of motion candidates in top-left corner
#define AFFINE_MAX_NUM_RT                   2 ///< max number of motion candidates in top-right corner
#define AFFINE_MAX_NUM_LB                   2 ///< max number of motion candidates in left-bottom corner
#define AFFINE_MAX_NUM_RB                   1 ///< max number of motion candidates in right-bottom corner
#define AFFINE_MIN_BLOCK_SIZE               4 ///< Minimum affine MC block size
#define AFF_MAX_NUM_MRG                     5 // maximum affine merge candidates
#define AFF_MODEL_CAND                      2 // maximum affine model based candidate

#define MAX_MEMORY_ACCESS_BI                ((8 + 7) * (8 + 7) / 64)
#define MAX_MEMORY_ACCESS_UNI               ((8 + 7) * (4 + 7) / 32)

// AFFINE ME configuration (non-normative)
#define AF_ITER_UNI                         7 // uni search iteration time
#define AF_ITER_BI                          5 // bi search iteration time
#define AFFINE_BI_ITER                      1

#define AFF_SIZE                            16
#define Tab_Affine_AMVR(x)                 ((x==0) ? 2 : ((x==1) ? 4 : 0) )

/************************* COLOR PLANE ************************************************/
#define Y_C                                0  /* Y luma */
#define U_C                                1  /* Cb Chroma */
#define V_C                                2  /* Cr Chroma */
#define N_C                                3  /* number of color component */

/************************* REF LIST DEFINE ************************************************/
#define REFP_0                             0
#define REFP_1                             1
#define REFP_NUM                           2

typedef enum uavs3e_pred_direction {
    PRED_L0 = 0,
    PRED_L1 = 1,
    PRED_BI = 2,
    PRED_DIR_NUM = 3,
} pred_direction;

/************************* MC ************************************************/
#define MV_X                               0
#define MV_Y                               1
#define MV_D                               2

#define CPMV_BIT_DEPTH                     18
#define COM_CPMV_MAX                       ((s32)((1<<(CPMV_BIT_DEPTH - 1)) - 1))
#define COM_CPMV_MIN                       ((s32)(-(1<<(CPMV_BIT_DEPTH - 1))))
typedef int                                CPMV;

#define NUM_AVS2_SPATIAL_MV                3
#define NUM_SKIP_SPATIAL_MV                6
#define MVPRED_L                           0
#define MVPRED_U                           1
#define MVPRED_UR                          2
#define MVPRED_xy_MIN                      3


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                         Certain Tools Parameters                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define READ_BITS_INIT                    16
#define DEC_RANGE_SHIFT                   (READ_BITS_INIT-(PROB_BITS-2))
#define MV_SCALE_PREC                      14 /* Scaling precision for motion vector prediction (2^MVP_SCALING_PRECISION) */

enum uavs3e_ipfilter_conf {
    IPFILTER_H_8,
    IPFILTER_H_4,
    IPFILTER_V_8,
    IPFILTER_V_4,
    NUM_IPFILTER
};
enum uavs3e_ipfilter_ext_conf {
    IPFILTER_EXT_8,
    IPFILTER_EXT_4,
    NUM_IPFILTER_Ext
};

#define STRIDE_IMGB2PIC(s_imgb)          ((s_imgb) / sizeof(pel))

#define MAX_CU_LOG2                        7
#define MIN_CU_LOG2                        2
#define MAX_CU_SIZE                       (1 << MAX_CU_LOG2)
#define MIN_CU_SIZE                       (1 << MIN_CU_LOG2)
#define MAX_CU_DIM                        (MAX_CU_SIZE * MAX_CU_SIZE)
#define MIN_CU_DIM                        (MIN_CU_SIZE * MIN_CU_SIZE)
#define MAX_CU_DEPTH                       6  /* 128x128 ~ 4x4 */
#define MAX_TR_LOG2                        6  /* 64x64 */
#define MAX_TR_SIZE                        (1 << MAX_TR_LOG2)
#define MAX_TR_DIM                         (MAX_TR_SIZE * MAX_TR_SIZE)

#define INTRA_NEIB_SIZE (MAX_CU_SIZE / 2 * 9)
#define INTRA_NEIB_MID (INTRA_NEIB_SIZE / 2)

#define MAX_NUM_PB                         4
#define MAX_NUM_TB                         4
#define MAX_CU_CNT_IN_LCU                  (MAX_CU_DIM/MIN_CU_DIM)
#define PEL2SCU(pel)                       ((pel) >> MIN_CU_LOG2)
#define CU_SIZE_NUM              (MAX_CU_LOG2 - MIN_CU_LOG2 + 1) // 4 --> 128

/************************* PB/TB BLOCK ************************************************/
#define PB0                                0  // default PB idx
#define TB0                                0  // default TB idx
#define TBUV0                              0  // default TB idx for chroma


/************************* PIC PADDING ************************************************/
#define PIC_PAD_SIZE_L                     (MAX_CU_SIZE + 16)
#define PIC_PAD_SIZE_C                     (PIC_PAD_SIZE_L >> 1)

/************************* REFERENCE FRAME MANAGER ************************************************/
#define MAX_NUM_ACTIVE_REF_FRAME           4
#define EXTRA_FRAME     MAX_REFS  /* DPB Extra size */
#define MAX_PB_SIZE    (MAX_REFS + EXTRA_FRAME) /* maximum picture buffer size */
#define DOI_CYCLE_LENGTH                 256 // the length of the DOI cycle.


/************************* Neighboring block availability ************************************************/
#define AVAIL_BIT_UP                       0
#define AVAIL_BIT_LE                       1
#define AVAIL_BIT_UP_LE                    2

#define AVAIL_UP                          (1 << AVAIL_BIT_UP)
#define AVAIL_LE                          (1 << AVAIL_BIT_LE)
#define AVAIL_UP_LE                       (1 << AVAIL_BIT_UP_LE)

#define IS_AVAIL(avail, pos)            (((avail)&(pos)) == (pos))
#define SET_AVAIL(avail, pos)             (avail) |= (pos)
#define REM_AVAIL(avail, pos)             (avail) &= (~(pos))
#define GET_AVAIL_FLAG(avail, bit)      (((avail)>>(bit)) & 0x1)

#define MAX_TX_DYNAMIC_RANGE               15
#define QUANT_SHIFT                        14

/* neighbor CUs
   neighbor position:

   D     B     C

   A     X,<G>

   E          <F>
*/
#define MAX_NEB                            5
#define NEB_A                              0  /* left */
#define NEB_B                              1  /* up */
#define NEB_C                              2  /* up-right */
#define NEB_D                              3  /* up-left */
#define NEB_E                              4  /* low-left */

#define NEB_F                              5  /* co-located of low-right */
#define NEB_G                              6  /* co-located of X */
#define NEB_X                              7  /* center (current block) */
#define NEB_H                              8  /* right */
#define NEB_I                              9  /* low-right */
#define MAX_NEB2                           10

#define IS_INTRA_SLICE(slice_type)       ((slice_type) == SLICE_I))
#define IS_INTER_SLICE(slice_type)      (((slice_type) == SLICE_P) || ((slice_type) == SLICE_B))

/************************* PREDICT MODE ************************************************/
#define MODE_INTRA                         0
#define MODE_INTER                         1
#define MODE_SKIP                          2
#define MODE_DIR                           3

/************************* skip / direct mode ************************************************/
#define TRADITIONAL_SKIP_NUM               (PRED_DIR_NUM + 1)
#define ALLOWED_HMVP_NUM                   8
#define MAX_SKIP_NUM                       (TRADITIONAL_SKIP_NUM + ALLOWED_HMVP_NUM)

#define UMVE_BASE_NUM                      2
#define UMVE_REFINE_STEP                   5
#define UMVE_MAX_REFINE_NUM                (UMVE_REFINE_STEP * 4)


/************************* INTRA MODE ************************************************/
#define IPD_DC                             0
#define IPD_PLN                            1  /* Luma, Planar */
#define IPD_BI                             2  /* Luma, Bilinear */
#define IPD_DM_C                           0  /* Chroma, DM*/
#define IPD_DC_C                           1  /* Chroma, DC */
#define IPD_HOR_C                          2  /* Chroma, Horizontal*/
#define IPD_VER_C                          3  /* Chroma, Vertical */
#define IPD_BI_C                           4  /* Chroma, Bilinear */
#define IPD_TSCPM_C                        5
#define IPD_CHROMA_CNT                     5
#define IPD_INVALID                       (-1)
#define IPD_RDO_CNT                        5
#define IPD_HOR                            24 /* Luma, Horizontal */
#define IPD_VER                            12 /* Luma, Vertical */
#define IPD_CNT                            33
#define IPD_IPCM                           33
#define IPD_DIA_R                         18 /* Luma, Right diagonal */
#define IPD_DIA_L                         3  /* Luma, Left diagonal */
#define IPD_DIA_U                         32 /* Luma, up diagonal */

#define COM_IPRED_CHK_CONV(mode) ((mode) == IPD_VER || (mode) == IPD_HOR || (mode) == IPD_DC || (mode) == IPD_BI)


/************************* REF idx Set ************************************************/
#define REFI_INVALID                      (-1)
#define REFI_IS_VALID(refi)               ((refi) >= 0)
#define SET_REFI(refi, idx0, idx1)        (refi)[REFP_0] = (idx0); (refi)[REFP_1] = (idx1)

#define COM_IS_INTRA_SCU(m) ((*(u8*)&(m) & 0x3) == 0x3)
#define COM_IS_INTER_SCU(m) ((*(u8*)&(m) & 0x3) == 0x1)

/************************* SCU MAP SET ************************************************/
#define MCU_SET_SCUP(m,v)               (m)=(((m) & 0x3F)|(v)<<6)
#define MCU_GET_SCUP(m)                 (int)((m) >> 6)
/* set cu_log2w & cu_log2h to map */
#define MCU_SET_LOGW(m, v)              (m)=(((m) & (~0x07))|((v)&0x07)   )
#define MCU_SET_LOGH(m, v)              (m)=(((m) & (~0x38))|((v)&0x07)<<3)
/* get cu_log2w & cu_log2h to map */
#define MCU_GET_LOGW(m)                 (int)(((m   ))&0x07)
#define MCU_GET_LOGH(m)                 (int)(((m)>>3)&0x07)

/*************************** user data types **********************************************/
#define COM_UD_PIC_SIGNATURE              0x10
#define COM_UD_END                        0xFF

/*****************************************************************************
 * for binary and triple tree structure
 *****************************************************************************/
typedef enum uavs3e_split_mode_t {
    NO_SPLIT        = 0,
    SPLIT_BI_VER    = 1,
    SPLIT_BI_HOR    = 2,
    SPLIT_EQT_VER   = 3,
    SPLIT_EQT_HOR   = 4,
    SPLIT_QUAD      = 5,
    NUM_SPLIT_MODE
} split_mode_t;

typedef enum uavs3e_split_dir_t {
    SPLIT_VER = 0,
    SPLIT_HOR = 1,
    SPLIT_QT = 2,
} split_dir_t;

typedef enum uavs3e_const_pred_mode_t {
    NO_MODE_CONS = 0,
    ONLY_INTER = 1,
    ONLY_INTRA = 2,
} const_pred_mode_t;

typedef enum uavs3e_tree_status_t {
    TREE_LC = 0,
    TREE_L  = 1,
    TREE_C  = 2,
} tree_status_t;

typedef enum uavs3e_channel_type_t {
    CHANNEL_LC = 0,
    CHANNEL_L  = 1,
    CHANNEL_C  = 2,
} channel_type_t;

typedef enum uavs3e_part_size_t {
    SIZE_2Nx2N = 0,           ///< symmetric partition,  2Nx2N
    SIZE_NxN   = 1,           ///< symmetric partition, NxN
    SIZE_2NxhN = 2,           ///< symmetric partition, 2N x 0.5N
    SIZE_hNx2N = 3,           ///< symmetric partition, 0.5N x 2N
    SIZE_2NxnU = 4,           ///< asymmetric partition, 2Nx( N/2) + 2Nx(3N/2)
    SIZE_2NxnD = 5,           ///< asymmetric partition, 2Nx(3N/2) + 2Nx( N/2)
    SIZE_nLx2N = 6,           ///< asymmetric partition, ( N/2)x2N + (3N/2)x2N
    SIZE_nRx2N = 7,           ///< asymmetric partition, (3N/2)x2N + ( N/2)x2N
    NUM_PART_SIZE
} part_size_t;

typedef enum uavs3e_block_shape_t {
    NON_SQUARE_18,
    NON_SQUARE_14,
    NON_SQUARE_12,
    SQUARE,
    NON_SQUARE_21,
    NON_SQUARE_41,
    NON_SQUARE_81,
    NUM_BLOCK_SHAPE,
} block_shape_t;

typedef enum uavs3e_trans_type_t {
    DCT2,
    DCT8,
    DST7,
    NUM_TRANS_TYPE
} trans_type_t;

#endif /* _COM_DEF_H_ */