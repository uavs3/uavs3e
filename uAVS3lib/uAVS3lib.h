#ifndef __uAVS3lib_H__
#define __uAVS3lib_H__

#ifdef __cplusplus
extern "C" {
#endif

#define COMPILE_10BIT 0

#if COMPILE_10BIT
typedef unsigned short pel_t;
#else
typedef unsigned char pel_t;
#endif

#define INSERT_MD5 0

/**
 * ===========================================================================
 * define uAVS3_API
 * ===========================================================================
 */
#ifdef UAVS3LIB_EXPORTS
#  ifdef __GNUC__                     /* for Linux  */
#    if __GNUC__ >= 4
#      define uAVS3API __attribute__((visibility("default")))
#    else
#      define uAVS3API __attribute__((dllexport))
#    endif
#    define __cdecl
#  else                               /* for windows */
#    define uAVS3API __declspec(dllexport)
#  endif
#else
#  ifdef __GNUC__                     /* for Linux   */
#    define uAVS3API
#    define __cdecl
#  else                               /* for windows */
#    define uAVS3API __declspec(dllimport)
#  endif
#endif

/**
 * ===========================================================================
 * const defines
 * ===========================================================================
 */

// Picture types
#define I_FRM   0x01  /* 0000 0001 */
#define P_FRM   0x02  /* 0000 0010 */
#define B_FRM   0x04  /* 0000 0100 */
#define F_FRM   0x08  /* 0000 1000 */
#define S_FRM   0x12  /* 0001 0010 */
#define G_FRM   0x21  /* 0010 0001 */
#define GB_FRM  0x61  /* 0110 0001 */

#define FP_FLG  0x0A  /* 0000 1010 */
#define BG_FLG  0x20  /* 0010 0000 */

#define SEQ_HDR 0x100
#define PIC_HDR 0x1000
#define SEQ_END 0x10000

#define MAXREF  4
#define MAXGOP 32
#define MAX_QP 63

/* ---------------------------------------------------------------------------
* log level
*/
#define COM_LOG_NONE       (-1)
#define COM_LOG_ERROR      0
#define COM_LOG_WARNING    1
#define COM_LOG_INFO       2
#define COM_LOG_DEBUG      3

/**
 * ===========================================================================
 * type defines
 * ===========================================================================
 */
typedef struct hdr_ext_data_t {
    int enable;
    int display_primaries_x[3];
    int display_primaries_y[3];
    int white_point_x;
    int white_point_y;
    int max_display_mastering_luminance;
    int min_display_mastering_luminance;
    int max_content_light_level;
    int max_picture_average_light_level;
} hdr_ext_data_t;

typedef struct image_t {
    void  *magic;             /* for image list */
    long long pts;
    long long dts;
    int    width;
    int    height;
    int    flush_type;
    int    i_stride[4];       /* strides for each plane */
    pel_t  *plane[4];         /* pointers to each plane */

    /* used in encode lib */
    // 1. basic info
    long long poc;
    int type;
    int next_bframes;
    int bfrm_coi_offset;

    // 2. rc info
    int    scene_cut;
    int    static_frm;
    int    initQP;
    double intra_cpp;
    double inter_cpp;
    double cpp;
    double* inter_cpp_list;
    double* intra_cpp_list;
    double target_bits;
    double initLambda;
    int    coded_list_idx;
} image_t;

typedef struct uavs3e_com_rpl_t {
    int num;
    int active;
    int delta_doi[16];
} com_rpl_t;

typedef struct reference_management {
    int layer;
    int poc;
    int qp_offset;
    int num_of_ref;
    int referd_by_others;
    int ref_pic[MAXREF];
    int num_to_remove;
    int remove_pic[MAXREF];

    com_rpl_t l0;
    com_rpl_t l1;
} ref_man;

typedef struct cfg_param_t {

    /* ----------------------------------------------------
    *  Basic informations below, read from configure file
    * ---------------------------------------------------*/
    signed char infile   [1000];      
    signed char outfile  [1000];
    signed char ReconFile[1000];
    signed char TraceFile[1000];

    int sample_bit_depth;
    int bit_depth;

    int ColourPrimary;
    int TransferChar;
    int MatrixCoeff;

    int threads_wpp;
    int threads_frm;  // no use, only for RT
    int lcu_rows_in_slice;
    int no_frames;                //!< number of frames to be encoded
    int baseQP;                      //!< QP of first frame
    
    int speed_level;
      
    int img_width;                //!< GH: if CUSTOM image format is chosen, use this size
    int img_height;               //!< GH: width and height must be a multiple of 16 pels

    int intra_period;
    int close_gop;
    int succ_bfrms;

    int InterlaceCodingOption;
    int frame_rate_num;
    int frame_rate_den;

    int loop_filter_parameter_flag;
    int alpha_c_offset;
    int beta_offset;

    int use_wquant;
    int use_hdr_chroma_opt;
    int bg_enable;      // only for Off-line  codec
    int bg_mode;        // only for Off-line  codec
    int bg_period;      // only for Off-line  codec
    int bg_qp_offset;   // only for Off-line  codec

    int slice_sao_enable_Y;
    int slice_sao_enable_Cb;
    int slice_sao_enable_Cr;

    int rate_ctrl;
    int target_bitrate;
    ref_man seq_ref_cfg[MAXGOP];
    hdr_ext_data_t hdr_ext;

    /* ----------------------------------------------------------------------------
    *  Extern informations below, calculated by encoder, from basic informations
    * ----------------------------------------------------------------------------*/
    int auto_crop_right;
    int auto_crop_bottom;
    int profile_id;
    int gop_size;

    struct {
        int use_deblk;
        int use_mhp;
        int use_sao;
        int use_secT;
    } tools;
    /* ----------------------------------------------------------------------------
     *  Extern informations below, calculated by encoder, from basic informations 
     * ----------------------------------------------------------------------------*/
    int pic_width_in_lcu;
    int pic_height_in_lcu;
    int pic_size_in_lcu;

    int pic_width_in_mcu;
    int pic_height_in_mcu;
    int pic_size_in_mcu;

    int pic_width_in_b4;  // only for Off-line  codec
    int pic_height_in_b4; // only for Off-line  codec

    int b4_stride;        // only for Off-line  codec
    int b4_size;          // only for Off-line  codec
    int b8_stride;        // only for Real-time codec
    int b8_size;          // only for Real-time codec

    int Min_V_MV;
    int Max_V_MV;
    int Min_H_MV;
    int Max_H_MV;

    int pic_reorder_delay;
    int bitdepth_qp_offset;
    int frame_rate_code;
    double frame_rate;

    double speed_adj_rate;
} cfg_param_t;

/* ---------------------------------------------------------------------------
 *  output functions types define
 */
typedef void(*rec_out_t )(void* priv_data, image_t *rec, image_t *org, image_t *trace, int frm_type, long long idx, int bits, double qp, int time, signed char* ext_info);
typedef void(*strm_out_t)(void* priv_data, unsigned char *buf, int len, long long pts, long long dts, int type);


/* ---------------------------------------------------------------------------
 *  Rate Control types and callback functions 
 */
typedef struct avs3_rc_basic_info_t {
    int type;               // in
    int frameLevel;         // in
    long long poc;          // in
    long long coi;          // in
    int    scene_cut;       // in
    int    qp;              // out/in         
    double lambda;          // out/in       
    double target_bits;     // out/in, only used for log-info
    double pic_cpp;         // out/in  
    double inter_cpp;       // in
    double intra_cpp;       // in
    int    coded_list_idx;  // out
} avs3_rc_basic_info_t;

typedef struct avs3_rc_get_qp_param_t {
    avs3_rc_basic_info_t info;
    int    bfrms_in_subgop;
    double *sub_gop_cpp_list;
    double ref_lambda[2];
} avs3_rc_get_qp_param_t;

typedef struct avs3_rc_update_param_t {
    avs3_rc_basic_info_t info;
    int    header_bits;
    int    data_bits;
    int    coef_bits;
} avs3_rc_update_param_t;

typedef void*(*rc_frame_init_t)(cfg_param_t *input);
typedef void(*rc_frame_get_qp_t)(void *rc_handle, avs3_rc_get_qp_param_t* param);
typedef void(*rc_frame_update_t)(void *rc_handle, avs3_rc_update_param_t* param, char *ext_info, int info_len);
typedef void(*rc_frame_destroy_t)(void *rc_handle);

/* ---------------------------------------------------------------------------
*  Back Ground Coding types and callback functions
*/
typedef void*(*bg_model_init_t)(cfg_param_t *input);
typedef void(*bg_model_read_t)(void *bg_handle, image_t* img);
typedef void(*bg_model_build_t)(void *bg_handle, image_t* img);
typedef void(*bg_model_destroy_t)(void *bg_handle);

/**
 * ===========================================================================
 * interface function declares (uAVS3 library APIs for AVS3 video encoder)
 * ===========================================================================
 */

/* ---------------------------------------------------------------------------
 *  alloc data for a picture. 
 */
uAVS3API image_t * __cdecl avs3_lib_imgbuf(void *handle);

/* ---------------------------------------------------------------------------
 *  encoder core functions
 */

uAVS3API void* __cdecl avs3_lib_create(cfg_param_t *input, strm_out_t strm_callbak, rec_out_t rec_callbak, int input_bufsize, void *priv_data);
uAVS3API void  __cdecl avs3_lib_encode(void *handle, int bflush, int output_seq_end);
uAVS3API void  __cdecl avs3_lib_free (void *handle);
uAVS3API void  __cdecl avs3_lib_reset(void *handle, cfg_param_t *input, void *priv_data);

/* ---------------------------------------------------------------------------
 *  utilities
 */
uAVS3API void __cdecl avs3_find_psnr(cfg_param_t *input, image_t *org_img, image_t *rec_img, double *snr_y, double *snr_u, double *snr_v);
uAVS3API void __cdecl avs3_find_ssim(cfg_param_t* input, image_t *org_img, image_t *rec_img, double *ssim_y, double *ssim_u, double *ssim_v);
uAVS3API void __cdecl avs3_lib_set_priority(void *handle, int priority);
uAVS3API void __cdecl avs3_lib_set_affinity(void *handle, void *mask);

/* ---------------------------------------------------------------------------
 *  rate control
 */
uAVS3API void __cdecl avs3_lib_set_rc(void *handle, rc_frame_init_t init,  rc_frame_get_qp_t get_qp, rc_frame_update_t update, rc_frame_destroy_t free);

/* ---------------------------------------------------------------------------
*  back ground
*/
uAVS3API void  __cdecl avs3_lib_set_bg(void *handle, bg_model_init_t init, bg_model_build_t build, bg_model_read_t read, bg_model_destroy_t free);

/* ---------------------------------------------------------------------------
*  back ground
*/
uAVS3API void  __cdecl avs3_lib_speed_adjust(void *handle, double adj);


/* ---------------------------------------------------------------------------
 *  function types define
 */
typedef image_t *(__cdecl *avs3_lib_imgbuf_t)(void *handle);
typedef void*    (__cdecl *avs3_lib_create_t)(cfg_param_t *input, strm_out_t strm_callbak, rec_out_t rec_callbak, int input_bufsize, void *priv_data);
typedef void     (__cdecl *avs3_lib_encode_t)(void *handle, int bflush, int output_seq_end);
typedef void     (__cdecl *avs3_lib_free_t)(void *handle);
typedef void     (__cdecl *avs3_lib_reset_t)(void *handle, cfg_param_t *input, void *priv_data);
typedef void     (__cdecl *avs3_find_psnr_t)(cfg_param_t *input, image_t *org_img, image_t *rec_img, double *snr_y, double *snr_u, double *snr_v);
typedef void     (__cdecl *avs3_find_ssim_t)(cfg_param_t* input, image_t *org_img, image_t *rec_img, double *ssim_y, double *ssim_u, double *ssim_v);
typedef void     (__cdecl *avs3_lib_set_priority_t)(void *handle, int priority);
typedef void     (__cdecl *avs3_lib_set_affinity_t)(void *handle, void* mask);
typedef void     (__cdecl *avs3_lib_set_rc_t)(void *handle, rc_frame_init_t init, rc_frame_get_qp_t get_qp, rc_frame_update_t update, rc_frame_destroy_t free);
typedef void     (__cdecl *avs3_lib_set_bg_t)(void *handle, bg_model_init_t init, bg_model_build_t build, bg_model_read_t read, bg_model_destroy_t free);
typedef void     (__cdecl *avs3_lib_speed_adjust_t)(void *handle, double adj);


/*********************************************************************************************/
// util

#if defined(_WIN32) || defined(__linux__)
#include <sys/timeb.h>
#elif defined(__GNUC__)
#include <sys/time.h>
#endif

#include "stddef.h"

static __inline long long get_mdate(void)
{
#if defined(_WIN32) || defined(__linux__)
    struct timeb tb;
    ftime(&tb);
    return ((long long)tb.time * 1000 + (long long)tb.millitm) * 1000;
#elif defined(__GNUC__) 
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
#else
    return -1;
#endif
}

typedef struct {
    signed char *TokenName;
    int offset;
    int Type;
    int Value;
} Mapping;

static const char refheadstr[] = "Frame1:";
static const char hdrheadstr[] = "HDRExt:";

static const ref_man RA_RPS[8] = {
    { 0, 8, 1, 2, 1,{ 8,  7, 0,  0 }, 0,{ 0,  0, 0, 0 }, { 4, 2, {  8,   7,  16,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 0, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} }, },
    { 1, 4, 1, 2, 1,{ 1,  9, 0,  0 }, 2,{ 4, 17, 0, 0 }, { 2, 1, {  9,   8,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 1, 1, { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} }, },
    { 2, 2, 2, 2, 1,{ 1, 10, 0,  0 }, 1,{ 9,  0, 0, 0 }, { 2, 1, { 10,   2,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 1, 1, { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} }, },
    { 3, 1, 3, 2, 0,{ 1, 11, 0,  0 }, 0,{ 0,  0, 0, 0 }, { 3, 1, { 11,   3,   2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 1, 1, { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} }, },
    { 3, 3, 3, 2, 0,{ 3,  2, 0,  0 }, 0,{ 0,  0, 0, 0 }, { 3, 1, {  2,  12,   4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 1, 1, { 3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} }, },
    { 2, 6, 2, 2, 1,{ 5,  4, 0,  0 }, 0,{ 0,  0, 0, 0 }, { 3, 1, {  4,  13,   3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 1, 1, { 5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} }, },
    { 3, 5, 3, 2, 0,{ 1,  5, 0,  0 }, 1,{ 4,  0, 0, 0 }, { 3, 1, {  5,  14,   6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 1, 1, { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} }, },
    { 3, 7, 3, 2, 0,{ 7,  2, 0,  0 }, 0,{ 0,  0, 0, 0 }, { 3, 1, {  2,  15,   6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } },{ 1, 1, { 7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0} } }
};

#define PARAM_OFFSET(x) offsetof(cfg_param_t, x)

static const Mapping tab_cfg_map[] = {
    { "threads-wpp",            PARAM_OFFSET(threads_wpp),                  0,     1 },
    { "threads-frm",            PARAM_OFFSET(threads_frm),                  0,     1 },
    { "LcuRowInSlice",          PARAM_OFFSET(lcu_rows_in_slice),            0,     0 },
    { "IntraPeriod",            PARAM_OFFSET(intra_period),                 0,     6 },
    { "FramesToBeEncoded",      PARAM_OFFSET(no_frames),                    0,     0 },
    { "QP",                     PARAM_OFFSET(baseQP),                       0,    32 },
    { "SpeedLevel",             PARAM_OFFSET(speed_level),                  0,     6 },
    { "SourceWidth",            PARAM_OFFSET(img_width),                    0,  3840 },
    { "SourceHeight",           PARAM_OFFSET(img_height),                   0,  2160 },
    { "InputFile",              PARAM_OFFSET(infile),                       1,     0 },
    { "OutputFile",             PARAM_OFFSET(outfile),                      1,     0 },
    { "ReconFile",              PARAM_OFFSET(ReconFile),                    1,     0 },
    { "TraceFile",              PARAM_OFFSET(TraceFile),                    1,     0 },
    { "NumberBFrames",          PARAM_OFFSET(succ_bfrms),                   0,     7 },
    { "InterlaceCodingOption",  PARAM_OFFSET(InterlaceCodingOption),        0,     0 },
    { "LoopFilterParameter",    PARAM_OFFSET(loop_filter_parameter_flag),   0,     0 },
    { "LoopFilterAlphaOffset",  PARAM_OFFSET(alpha_c_offset),               0,     0 },
    { "LoopFilterBetaOffset",   PARAM_OFFSET(beta_offset),                  0,     0 },
    { "FrameRate",              PARAM_OFFSET(frame_rate_num),               0,    50 },
    { "FrameRateDen",           PARAM_OFFSET(frame_rate_den),               0,     1 },
    { "CodingBitDepth",         PARAM_OFFSET(bit_depth),                    0,    10 },
    { "SampleBitDepth",         PARAM_OFFSET(sample_bit_depth),             0,    10 },
    { "ColourPrimary",          PARAM_OFFSET(ColourPrimary),                0,     0 },
    { "TransferChar",           PARAM_OFFSET(TransferChar),                 0,     6 },
    { "MatrixCoeff",            PARAM_OFFSET(MatrixCoeff),                  0,     8 },
    { "WQuant",                 PARAM_OFFSET(use_wquant),                   0,     0 },
    { "HdrUVDelta",             PARAM_OFFSET(use_hdr_chroma_opt),           0,     0 },
    { "RateControl",            PARAM_OFFSET(rate_ctrl),                    0,     1 },
    { "TargetBitRate",          PARAM_OFFSET(target_bitrate),               0, 35000 },
    { "BackgroundEnable",       PARAM_OFFSET(bg_enable),                    0,     0 },
    { "BGMode",                 PARAM_OFFSET(bg_mode),                      0,     0 },
    { "GBQOffset",              PARAM_OFFSET(bg_qp_offset),                 0,     9 },
    { "GBPeriod",               PARAM_OFFSET(bg_period),                    0,     4 },
    { "CloseGOP",               PARAM_OFFSET(close_gop),                    0,     0 },
    { "slice_sao_enable_Y",     PARAM_OFFSET(slice_sao_enable_Y),           0,    -1 },
    { "slice_sao_enable_Cb",    PARAM_OFFSET(slice_sao_enable_Cb),          0,    -1 },
    { "slice_sao_enable_Cr",    PARAM_OFFSET(slice_sao_enable_Cr),          0,    -1 },
};

#ifdef __cplusplus
} // extern "C" 
#endif

#endif /* __uAVS3lib_H__ */
