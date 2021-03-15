#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdio.h>    
#include "defines.h"
#include "tools/xlist.h"
#include "tools/threadpool.h"
#include "tools/uratectrl.h"

#include <memory.h>

enum IPFilterConf {
    IP_FLT_C_H,
    IP_FLT_C_V,
    NUM_IP_FLT_C
};

enum IntraPredDir {
    INTRA_PRED_VER = 0,
    INTRA_PRED_HOR,
    INTRA_PRED_DC_DIAG
};

enum SAOComponentIdx {
    SAO_Y = 0,
    SAO_Cb,
    SAO_Cr,
    NUM_SAO_COMPONENTS
};

enum SAOMode { //mode
    SAO_MODE_NEW = 0,
    SAO_MERGE_MODE_LEFT,
    SAO_MERGE_MODE_ABOVE,
    NUM_SAO_MODES
};

enum SAOModeMergeTypes {
    SAO_MERGE_LEFT = 0,
    SAO_MERGE_ABOVE,
    NUM_SAO_MERGE_TYPES
};

enum SAOModeNewTypes { //NEW: types
    SAO_TYPE_EO_0,
    SAO_TYPE_EO_90,
    SAO_TYPE_EO_135,
    SAO_TYPE_EO_45,
    NUM_SAO_NEW_TYPES
};

enum SAOEOClasses { // EO Groups, the assignments depended on how you implement the edgeType calculation
    SAO_CLASS_EO_FULL_VALLEY = 0,
    SAO_CLASS_EO_HALF_VALLEY = 1,
    SAO_CLASS_EO_PLAIN       = 2,
    SAO_CLASS_EO_HALF_PEAK   = 3,
    SAO_CLASS_EO_FULL_PEAK   = 4,
    SAO_CLASS_BO             = 5,
    NUM_SAO_EO_CLASSES = SAO_CLASS_BO,
    NUM_SAO_OFFSET
};

typedef struct cu_t {
    /* cu level data */
    char_t  bitsize;    
    char_t  QP ;
    double  lambda;
    
    /* cu mode info */
    char_t  cuType;
    char_t  mdirect_mode;
    char_t  trans_size;
    i16s_t  mvd[2][2][2]; // [blk_idx][fwd/bwd][x/y]
    char_t  ipred_mode;
    char_t  ipred_mode_real;
    char_t  ipred_mode_c; 
    char_t  cbp;
    char_t  b8pdir;
 
} cu_t;

typedef i16u_t bin_ctx_t;

typedef struct {
    bin_ctx_t skip_flag[3];
    bin_ctx_t direct_flag[1];
    bin_ctx_t intra_flag[5];
    bin_ctx_t skip_idx[6];

    bin_ctx_t pdir_contexts[2];
    bin_ctx_t smvd_flag[1];
    bin_ctx_t p_skip_mode_contexts[MH_PSKIP_NUM];
    bin_ctx_t mvd_contexts[2][3];
    bin_ctx_t ref_no_contexts[NUM_REF_NO_CTX];
    bin_ctx_t delta_qp_contexts[NUM_DELTA_QP_CTX];
    bin_ctx_t l_intra_mode_contexts[NUM_INTRA_MODE_CTX];
    bin_ctx_t c_intra_mode_contexts[NUM_C_INTRA_MODE_CTX];
  
    bin_ctx_t split_contexts[NUM_SPLIT_CTX];
    bin_ctx_t tu_contexts[NUM_TU_CTX];
    bin_ctx_t saomergeflag_context[NUM_SAO_MERGE_FLAG_CTX];
    bin_ctx_t saomode_context[NUM_SAO_MODE_CTX];
    bin_ctx_t saooffset_context[NUM_SAO_OFFSET_CTX];

    bin_ctx_t   run[LBAC_CTX_RUN];
    bin_ctx_t   run_rdoq[LBAC_CTX_RUN];
    bin_ctx_t   last1[LBAC_CTX_LAST1 * 2];
    bin_ctx_t   last2[LBAC_CTX_LAST2 * 2 - 2];
    bin_ctx_t   level[LBAC_CTX_LEVEL];

    bin_ctx_t   ctp_zero_flag;
    bin_ctx_t   cbf[LBAC_CTX_CBF];

    bin_ctx_t   split_flag[LBAC_CTX_SPLIT_FLAG];
    bin_ctx_t   bt_split_flag[LBAC_CTX_BT_SPLIT_FLAG];
    bin_ctx_t   split_dir[LBAC_CTX_SPLIT_DIR];
    bin_ctx_t   split_mode[LBAC_CTX_SPLIT_MODE];

    bin_ctx_t   lcu_qp_delta[LBAC_CTX_DELTA_QP];
} bin_ctx_sets_t;

typedef struct aec_t {
    int Elow;
    int Ebits_cnt;
    int engin_flag;

    void(*enc_symbol      )(struct aec_t *aec, int symbol, bin_ctx_t * bi_ct);
    void(*enc_symbolW     )(struct aec_t *aec, int symbol, bin_ctx_t * bi_ct1, bin_ctx_t * bi_ct2);

    void(*enc_symbol_eq   )(struct aec_t *aec, int symbol);
    void(*enc_symbol_final)(struct aec_t *aec, int symbol);
    
    int           Ebuffer;
    uchar_t       *Ecodestrm;
    int           *Ecodestrm_len;
    
    //Original Domain Arithmetic Coder
    unsigned int  range;
    int           num_buffer_bytes;
    int           bits_left;
    unsigned int  buffer_byte;
    int           bits_add;

    bin_ctx_sets_t *syn_ctx;
} aec_t;

typedef struct SAOStatData {
    i32s_t diff[MAX_NUM_SAO_CLASSES];
    i32s_t count[MAX_NUM_SAO_CLASSES];
} SAOStatData;

typedef struct {
    int mergeIdc; //MERGE flag
    int typeIdc;  //OFF(-1) EO_0, EO_90, EO_135, EO_45, BO
    int startBand; //BO: starting band index
    int startBand2;
    int deltaband;
    int offset[MAX_NUM_SAO_CLASSES];
} SAOBlkParam;

typedef struct {
    int alf_flag;
    int num_coeff;
    int filters_per_group;
    int componentID;
    int filterPattern[NO_VAR_BINS];
    int coeffmulti[NO_VAR_BINS][ALF_MAX_NUM_COEF];
} ALFParam;

enum ALFComponentID {
    ALF_Y = 0,
    ALF_Cb,
    ALF_Cr,
    NUM_ALF_COMPONENT
};

typedef struct sao_enc_t{
    int sao_on[4]; // 0: Y    1: Cb     2: Cr     3: ALL
    SAOBlkParam(*saoBlkParams)[NUM_SAO_COMPONENTS]; //[SMB][comp]
} sao_enc_t;


typedef struct lcu_coef_t {
    ALIGNED_64(coef_t coef_y[LCU_SIZE * LCU_SIZE]);
    ALIGNED_64(coef_t coef_u[LCU_SIZE * LCU_SIZE / 4]);
    ALIGNED_64(coef_t coef_v[LCU_SIZE * LCU_SIZE / 4]);
} lcu_coef_t;

typedef struct {
    void(*ip_flt_c[NUM_IP_FLT_C][16]) (const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth);
    void(*ip_flt_c_ext[16]) (const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth);

    void(*ip_flt_y_hor    )(const pel_t  *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
    void(*ip_flt_y_ver    )(const pel_t  *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);
    void(*ip_flt_y_ver_ext)(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth);

    void(*cpy_pel[33]) (const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height);

    void(*avg_pel[33])(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height);
    void(*avg_pel_1d )(pel_t *dst, pel_t *src1, pel_t *src2, int len);

    void(*pix_sub[4])(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred);
    void(*pix_add[4])(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth);

    void(*add_inv_trans[5])(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
    void(*add_inv_trans_ext[5])(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth);
    void(*inv_trans_2nd_Hor)(coef_t *src, int i_src);
    void(*inv_trans_2nd_Ver)(coef_t *src, int i_src);

    void(*sub_trans    [5])(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
    void(*sub_trans_ext[5])(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift);
    void(*trans_2nd_Hor)(coef_t *src, int i_src);
    void(*trans_2nd_Ver)(coef_t *src, int i_src);
   
    void(*trans_dct8_dst7 [2][4])(coef_t *src, coef_t *dst, int shift);
    void(*itrans_dct8_dst7[2][4])(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val);

    int  (*quant         )(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift);
    int  (*quant_ext     )(coef_t *curr_blk, int size, int Q, int qp_const, int shift);
    void (*inv_quant     )(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift);
    void (*inv_quant_ext )(coef_t *src, coef_t *dst, int size, int QPI, int shift);
    int  (*pre_quant     )(coef_t *curr_blk, i16u_t *abs_blk, int len, int threshold);
    int  (*get_nz_num    )(coef_t *p_coef, int num_coeff);
    int  (*pre_quant_rdoq)(i16s_t *coef, int num, int q_value, int q_bits, int err_scale, i32u_t* abs_coef, i32u_t* abs_level, long long *uncoded_err);

    int  (*wq_quant        )(coef_t *curr_blk, i16s_t *wq_blk, int coef_num, int Q, int qp_const, int shift);
    int  (*wq_quant_ext    )(coef_t *curr_blk, i16s_t *wq_blk, int width, int height, int Q, int qp_const, int shift);
    void (*wq_inv_quant    )(coef_t *src, coef_t *dst, i16s_t *wq_blk, int coef_num, int QPI, int shift);
    void (*wq_inv_quant_ext)(coef_t *src, coef_t *dst, i16s_t *wq_blk, int width, int height, int QPI, int shift);
    int  (*wq_pre_quant    )(coef_t *curr_blk, i16u_t *abs_blk, i16s_t *wq_blk, int len, int threshold);

    int(*add_sign)(coef_t *dat, i16u_t *sign, int len);
    int(*get_cg_bits)(coef_t* coef);

    void(*zero_cg_check[4])(coef_t *coef, i64u_t *cg_mask);

    void(*padding_rows)(pel_t *src, int i_src, int width, int height, int start, int rows, int pad);

    void(*intra_pred_ang_x [ 9])(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
    void(*intra_pred_ang_y [ 8])(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);
    void(*intra_pred_ang_xy[11])(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight);

    void(*intra_pred_dc   )(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail, int bit_depth);
    void(*intra_pred_ver  )(pel_t *pSrc, pel_t *dst, int i_dst, int bs_x, int bs_y);
    void(*intra_pred_hor  )(pel_t *pSrc, pel_t *dst, int i_dst, int bs_x, int bs_y);
    void(*intra_pred_plane)(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth);
    void(*intra_pred_bi   )(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth);

    void(*deblk_luma  [2])(pel_t *SrcPtr, int stride, int Alpha, int Beta);
    void(*deblk_chroma[2])(pel_t *SrcPtrU, pel_t *SrcPtrV, int stride, int AlphaU, int BetaU, int AlphaV, int BetaV);

    void(*sao_flt[2])(void *h, void *sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height, int lcu_pix_width, 
        int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down);
    
    i32u_t (*cost_sad     [17])(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines);
    void   (*cost_sad_x4  [17])(pel_t *p_org, int i_org, pel_t *p_pred0, pel_t *p_pred1, pel_t *p_pred2, pel_t *p_pred3, int i_pred, i32u_t sad[4], int height, int skip_lines);
    i32u_t (*cost_ssd     [ 5])(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height);
    i32u_t (*cost_ssd_ext     )(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int width, int height);
    i64u_t (*cost_ssd_psnr    )(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int width, int height);
    void   (*ssim_4x4x2_core  )(const pel_t *pix1, int stride1, const pel_t *pix2, int stride2, int sums[2][4]);
    float  (*ssim_end4)(int sum0[5][4], int sum1[5][4], int width);
    i32u_t (*cost_satd_i      )(pel_t *p_org, int i_org);
    i32u_t (*cost_blk_satd[ 5])(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred);
    i32u_t (*cost_avg_sad [17])(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines);

    double (*calc_blk_ave)(pel_t* p_org, int width, int height, int iDownScale, int bit_depth);
    double (*calc_blk_var)(pel_t* p_org, int width, int height, int iDownScale, double ave, int bit_depth);

} funs_handle_t;

typedef struct frame_rec_t {
    void  *magic;             /* for rec list */
    image_t img_1_4th[4][4];
} frame_rec_t;

typedef struct frame_t {
    void  *magic;             /* for frame list */
    i64s_t poc;
    i64s_t coi;
    i32u_t ref_cnt;
    i32u_t b_ref;
    i64s_t ref_poc[4]; // for co-local predict
    struct frame_t *ref_list[4];
    image_t *img_org;
    ref_man curr_RPS;
    int randomaccese_flag;
    int flush_type;

    uchar_t *frm_data_base;
    char_t  *refbuf;
    i16s_t (*mvbuf)[2];
    frame_rec_t* rec;

    i32s_t finished_lines;
    avs3_pthread_mutex_t mutex;
    avs3_pthread_cond_t  cv;
} frame_t;

typedef struct me_info_t {
    /* -----------------------------------------
     * global info
     *----------------------------------------*/
    int max_ref_bits;
    int max_mv_bits;
    i32u_t *tab_mvbits;
    i32u_t *tab_refbits;
    int curr_lambda_mf;

    int max_mvd;
    
    /* -----------------------------------------
     * local info
     *----------------------------------------*/
    i16s_t min_mv_x;
    i16s_t max_mv_x;
    i16s_t min_mv_y;
    i16s_t max_mv_y;
    int out_of_range;

    int img_width;
    int img_height;
    int blk_x;
    int blk_y;
    int blk_width;
    int blk_height;
    pel_t *p_org;
    pel_t *p_ref;
    int i_org;
    int i_ref;
    int b_fast_fme;

    int i_mvc;
    i16s_t mvc[16][2];

    /* ref & mv for skip/direct */
    i16s_t skip_fw_mv [4][2];
    i16s_t skip_bw_mv [4][2];

    int    integer_mcost[4];
    i16s_t integer_fw_mv[4][2];

    /* -----------------------------------------
     * output of ME model
     *----------------------------------------*/

    /* ref & mv from ME */
    char_t best8x8pdir   [2];
    char_t best8x8ref    [2];

    /* [mode][bx][by][ref][x/y] */

    i16s_t predFwMv[2][2][2][2][2];
    i16s_t predBwMv[2][2][2][2][2];

    i16s_t allFwMv [2][2][2][2][2];
    i16s_t allBwMv [2][2][2][2][2];
    i16s_t allBidMv[2][2][2][2][2];

    //record costs of points surrounding the centered one. d_mcost:  
    i32u_t d_mcost[4][4]; // diamond positions
    i32u_t s_mcost[4][4]; // square  positions

} me_info_t;

typedef struct mv_info_t {
    i16s_t mv[2];
    char_t r;
} mv_info_t;

typedef struct best_mode_t {
    cu_t bst_cu;
    double rd_cost;
    mv_info_t mode_mv [4][2];     // [blk_id ][fwd/bwd]
    coef_t *coef_y;
    coef_t *coef_u;
    coef_t *coef_v;
    pel_t *rec_y;
    pel_t *rec_u;
    pel_t *rec_v;
} best_mode_t;

typedef struct analyzer_t {
    int    use_rdoq;
    int    me_method;
    double rdoq_lambda;
    double lambda_mode;
    double cu_threshold;
    me_info_t me_info;
} analyzer_t;

typedef struct {
    int       byte_pos;           //!< current position in bitstream;
    int       bits_to_go;         //!< current bitcounter
    uchar_t   byte_buf;           //!< current buffer for last written pel_t
    uchar_t  *streamBuffer;       //!< actual buffer for written bytes
} bit_stream_t;

typedef struct avs3_enc_t {
    const cfg_param_t *input; // global level data

    /* copy from frm_mgr */
    int type;
    int refs_num;
    i64s_t poc;
    i64s_t coi;
    i32u_t  ref_dist[4];
    ref_man curr_RPS;

    frame_t *ref_list[4];
    frame_t *p_cur_frm;
    image_t *img_org;
    image_t *img_rec;

    /* frame-level data */
    uchar_t    *dat_base;
    char_t     *pdir;
    char_t     *size;
    char_t     *refbuf_bw;
    i16s_t    (*mvbuf_bw)[2];
    char_t     *ipredmode;
    char_t     *deblk_flag;
    lcu_coef_t *all_quant_coefs;
    cu_t       *cu_data;
    aec_t      *wpp_aec;            /* back-up coding status for every LCU row */
    bin_ctx_sets_t *wpp_syn_ctx;
    pel_t      *(*p_rec_uprow)[3];  /* back-up rec pix for every LCU row       */
    int        *slice_nr;
    i16s_t     *tmp_buf[3]; // for luma interpolation
    uavs3e_sem_t *wpp_sem;
    uavs3e_sem_t *sem_enc;
    double       *lcu_row_est_bits;
    int          *lcu_row_finished;
    int          *pic_delta_qp;

    struct avs3_enc_t *wpp_encoder;
    avs3_threadpool_t *wpp_threadpool;
    avs3_threadpool_t *post_threadpool;

    int coef_bits;
    int is_top_field;
    int write_seqs_hdr;
    int write_seqs_end;
    int chroma_delta_qp[2];
    int adaptiveQP;
    double real_qp;
    double real_lambda;
    i16s_t *wquant_matrix[3][4]; // [square/hor/ver][blk size]

    /* coding tools */
    sao_enc_t sao_enc;

    /* image data */
    image_t  img_sao;

    /* frame-level stream */
    bit_stream_t hdr_stream;
    bit_stream_t bit_stream;

    char_t sync_end;

    /************************************************************************/
    /* Sync end! below is local data, when encoding one LCU                 */
    /************************************************************************/
    int encode_time;
    aec_t aec;
    bin_ctx_sets_t syn_ctx;

    /************************************************************************/
    /* lcu level data                                                       */
    /************************************************************************/
    int lcu_idx;
    int lcu_y;
    int lcu_b8_x;
    int lcu_b8_y;
    int lcu_pix_x;
    int lcu_pix_y;
    int lcu_width;
    int lcu_height;

    struct {
        ALIGNED_64(pel_t org_buf[(CACHE_STRIDE + CACHE_STRIDE/2) * CACHE_STRIDE]);
        ALIGNED_64(pel_t rec_buf[(CACHE_STRIDE + CACHE_STRIDE/2) * CACHE_STRIDE]);
        pel_t intra_left_luma     [64 + 1]; 
        pel_t intra_left_chroma[2][32 + 1];
    } cache;

    pel_t *p_org[3];
    pel_t *p_rec[3];

    /************************************************************************/
    /* cu level data                                                        */
    /************************************************************************/
    analyzer_t analyzer;
    lcu_coef_t cu_coefs;

    int cu_bitsize;
    int cu_pix_x;
    int cu_pix_y;
    int cu_available_up;
    int pu_pix_x;
    int pu_pix_y;
    int pu_b8_x;
    int pu_b8_y;
    int pu_size;

    int tu_pix_x;
    int tu_pix_y;
    int tu_size;
    int tu_q_size;
    int tu_available_up;
    int tu_available_left;

    ALIGNED_64(pel_t pred_inter_luma_skip  [5][MAX_CU_SIZE * MAX_CU_SIZE]);
    ALIGNED_64(pel_t pred_inter_chroma_skip[5][2][MAX_CU_SIZE * MAX_CU_SIZE / 4]);

    ALIGNED_64(pel_t pred_inter_luma       [MAX_CU_SIZE * MAX_CU_SIZE]);
    ALIGNED_64(pel_t pred_inter_chroma     [2][MAX_CU_SIZE * MAX_CU_SIZE / 4]);

    ALIGNED_64(pel_t pred_intra_luma  [NUM_INTRA_PMODE][MAX_CU_SIZE * MAX_CU_SIZE + 32]);
    ALIGNED_64(pel_t pred_intra_chroma[2][NUM_INTRA_PMODE_CHROMA][MAX_CU_SIZE * MAX_CU_SIZE / 4 + 32]);

    int            rdoq_bin_est_ctp[2];
    int            rdoq_bin_est_cbf[LBAC_CTX_CBF][2];
    int            rdoq_bin_est_run[LBAC_CTX_RUN][2];
    int            rdoq_bin_est_lvl[LBAC_CTX_LEVEL][2];
    int            rdoq_bin_est_lst[2][LBAC_CTX_LAST1][LBAC_CTX_LAST2][2];

} avs3_enc_t;

typedef struct pre_cal_PB_param_t {
    int lcu_y;
    cfg_param_t *input;
    image_t *ref_img;
    image_t *org_img;
    me_info_t me;
    double* cpp_list;
    int shift;
    int offset;
    double inter_cost;
    double intra_cost;
} pre_cal_PB_param_t;

typedef struct avs3_lookahead_t {
    i64s_t total_frames;
    double last_p_cpp;
    double last_inter_sad;
    double *last_p_cpp_list;
    double *last_b_fwd_cpp_list;
    double *last_b_bwd_cpp_list;
    frame_t *fst_ifrm;

    pre_cal_PB_param_t* param_list;
    image_t pre_proc_last_img;
    me_info_t pre_proc_me;
} avs3_lookahead_t;

typedef struct avs3_ctrl_t{
    cfg_param_t input;
    void* priv_data;

    i64s_t coi;
    int frm_idx;
    int write_end_code;
    int act_frm_thds;
    i64s_t last_ip_pts;
    i64s_t last_dts;

    avs3_enc_t *frm_encoder;
    
    avs3_threadpool_t *frm_threadpool;
    avs3_threadpool_t *ctl_threadpool;
    avs3_threadpool_t *pre_threadpool;
    
    /* output callback functions handle */
    strm_out_t func_out_strm;
    rec_out_t  func_out_rec;

    /* input re-order buffer */
    xlist_t imglist_idle;
    xlist_t imglist_read;
    xlist_t frmlist_ready;
    image_t *img_next;
    
    /* rec buffer list */
    xlist_t rec_list;

    /* ref list manager */
    avs3_pthread_mutex_t list_mutex;
    int frm_list_size;
    frame_t *all_frm_list; // all frames 
    i64s_t last_idr_coi;
    i64s_t last_idr_poc;
    i64s_t curr_idr_poc;
    i64s_t curr_idr_coi;

    /* rate control data  */
    void *rc_handle;
    rc_frame_init_t   rc_init;
    rc_frame_get_qp_t rc_get_qp;
    rc_frame_update_t rc_update;
    rc_frame_destroy_t rc_destroy;

    avs3_lookahead_t lookahead;

    int exit_flag;
} avs3_ctrl_t;

#endif // #ifndef _TYPES_H_

