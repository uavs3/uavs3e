#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "../version.h"
#include "uAVS3lib.h"

#define SPEED_LEVEL(level, x) ((x) >= (level))

#define LIMIT_FRAMES (0 * (3600 * 50))
#define WATER_MARK 0
#define WATER_MARK_AVS3 0  //1: AVS3 logo 0:UVVISION logo

#if WATER_MARK_AVS3
#define WATER_MARK_WIDTH 192
#define WATER_MARK_HEIGHT 66
#define WATER_MARK_SMALL_WIDTH 96
#define WATER_MARK_SMALL_HEIGHT 33
#else
#define WATER_MARK_WIDTH 124
#define WATER_MARK_HEIGHT 100
#define WATER_MARK_SMALL_WIDTH 64
#define WATER_MARK_SMALL_HEIGHT 52
#endif


#define LOOKAHEAD 0
#define USE_WQ 0

#define RESERVED_PROFILE_ID      0x24
#define BASELINE_PICTURE_PROFILE 18
#define BASELINE_PROFILE         32  //0x20
#define BASELINE10_PROFILE       34  //0x22
#include <stdlib.h>

#ifndef _WIN32
#include <sys/types.h>
#endif

#if defined(_MSC_VER)  
#pragma warning(disable: 4324) // structure was padded due to __declspec(align())
#endif

#define KEEP_CONST 1
#define MAX_SEARCH_RANGE  64

//////////////////// prediction techniques /////////////////////////////
#define LAM_2Level_TU            0.8

#define DIRECTION                3
#define DS_FORWARD               3
#define DS_BACKWARD              2
#define DS_BID                   1

#define FORWARD                      0
#define BACKWARD                     1
#define BID                          2

#define MH_PSKIP_NUM             4
#define BID_P_FST                1
#define BID_P_SND                2
#define FW_P_FST                 3
#define FW_P_SND                 4

#define WPM_NUM                  3
#define DMH_MODE_NUM             5     // Number of DMH mode

//////////////////// reference picture management /////////////////////////////
#define REF_MAXBUFFER            7

//////////////////// coefficient coding /////////////////////////////
#define CG_SIZE                  8    // M3035 size of an coefficient group, 4x4
#define SWAP(x,y)                {(y)=(y)^(x);(x)=(y)^(x);(y)=(x)^(y);}
#define TH                       2

#define MAX_NUM_SAO_CLASSES       32
#define NUM_SAO_EO_TYPES_LOG2 2

#define IMG_PAD_SIZE              (64 + 16)   //!< Number of pixels padded around the reference frame (>=4)

#define absm(A) ((A)<(0) ? (-(A)):(A)) //!< abs macro, faster than procedure
#define Clip3(min,max,val)  (((val)<(min))?(min):(((val)>(max))?(max):(val)))

#define COM_ABS(a)         ((a) < (0) ? (-(a)) : (a))
#define COM_MAX(a, b)      ((a) > (b) ? (a) : (b))
#define COM_MIN(a, b)      ((a) < (b) ? (a) : (b))
#define COM_ADD_MODE(v, mode) ((v) & (mode - 1))
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define MAX_COST (1 << 30)

#if defined(__ANDROID__)
#define log2(x) (log((double)(x)) * 1.4426950408889640513713538072172)
#endif

// block size of block transformed by AVS
#define PSKIPDIRECT               0
#define P2NX2N                    1
#define I16MB                     2 
#define MAXMODE                   3

#define LAMBDA_ACCURACY_BITS      16

#define LAMBDA_FACTOR(lambda) ((int)((double)(1<<LAMBDA_ACCURACY_BITS)*lambda+0.5))
#define WEIGHTED_COST(bits)   ((bits) >> LAMBDA_ACCURACY_BITS)

#define MV_COST(cx,cy,px,py)   (WEIGHTED_COST(me->tab_mvbits[cx - px] + me->tab_mvbits[cy - py]))
#define MV_COST_FULPEL(cx, cy) (WEIGHTED_COST(tab_mvbits_x[(cx) << 2] + tab_mvbits_y[(cy) << 2]))
#define MV_COST_SUBPEL(cx, cy) (WEIGHTED_COST(tab_mvbits_x[ cx      ] + tab_mvbits_y[ cy      ]))

#define  BWD_IDX(ref)               (((ref) < 2)? 1 - (ref): (ref))
#define  REF_COST_FWD(ref)          (WEIGHTED_COST(me->tab_refbits[ref]))

#define B_INTRA(mode) (mode == I16MB)

#define IS_INTRA(MB)                  ((MB)->cuType==I16MB)
#define IS_INTER(MB)                  ((MB)->cuType!=I16MB)

// Quantization parameter range
#define SHIFT_QP                     11

// Direct Mode types

#define LCU_SIZE_IN_BITS             5
#define LCU_SIZE                    (1 << LCU_SIZE_IN_BITS)
#define MAX_CU_SIZE                 32
#define NUM_INTRA_PMODE             33         //!< # luma intra prediction modes
#define NUM_MODE_FULL_RD             9         // number of luma modes for full RD search
#define NUM_INTRA_PMODE_CHROMA       5         //!< #chroma intra prediction modes

#define CACHE_STRIDE LCU_SIZE

// luma intra prediction modes
#define DC_PRED                      0
#define PLANE_PRED                   1
#define BI_PRED                      2
#define VERT_PRED                    12
#define HOR_PRED                     24

/* Loop Filter */
#define LOOPFLT_CROSS_SLICE 1

/* ALF params */
#define ALF_MAX_NUM_COEF        9
#define NO_VAR_BINS            16
#define ALF_FOOTPRINT_SIZE      7
#define DF_CHANGED_SIZE         3
#define ALF_NUM_BIT_SHIFT       6

// chroma intra prediction modes
#define DM_PRED_C                    0
#define DC_PRED_C                    1
#define HOR_PRED_C                   2
#define VERT_PRED_C                  3
#define BI_PRED_C                    4

#define XY_MIN_PMV                   1
#define MVPRED_xy_MIN                0
#define MVPRED_L                     1
#define MVPRED_U                     2
#define MVPRED_UR                    3

#define ANG_X_OFFSET   3
#define ANG_XY_OFFSET 13
#define ANG_Y_OFFSET  25

#define MULTI                        16384
#define HALF_MULTI                   8192
#define OFFSET                       14

#if defined(_WIN32) && !defined(__GNUC__)
#define DECLARE_ALIGNED(var, n) __declspec(align(n)) var
#else
#define DECLARE_ALIGNED(var, n) var __attribute__((aligned (n))) 
#endif

#define ALIGNED_64(var)    DECLARE_ALIGNED(var, 64)
#define ALIGNED_32(var)    DECLARE_ALIGNED(var, 32)
#define ALIGNED_16(var)    DECLARE_ALIGNED(var, 16)
#define ALIGNED_8(var)     DECLARE_ALIGNED(var,  8)
#define ALIGNED_4(var)     DECLARE_ALIGNED(var,  4)

#define ALIGN_BASIC 64 // for new generation CPU with 256-bits SIMD
#define ALIGN_MASK (ALIGN_BASIC - 1)
#define ALIGN_POINTER(x) (x + ALIGN_MASK - (((intptr_t)x + ALIGN_MASK) & ((intptr_t)ALIGN_MASK)))

/**********************************************************************
* C O N T E X T S   F O R   T M L   S Y N T A X   E L E M E N T S
**********************************************************************
*/
#define NUM_BLOCK_TYPES              5
#define NUM_MVD_CTX                  5
#define NUM_DMH_CTX                 15
#define NUM_REF_NO_CTX               6
#define NUM_DELTA_QP_CTX             4
#define NUM_INTER_DIR_CTX            4
#define NUM_INTER_DIR_DHP_CTX        3
#define NUM_AMP_CTX                  2
#define NUM_C_INTRA_MODE_CTX         2
#define NUM_CBP_CTX                  4
#define NUM_MAP_CTX                 17
#define NUM_LAST_CTX                17
#define NUM_INTRA_MODE_CTX           7
#define NUM_TU_CTX                   2
#define NUM_SPLIT_CTX                3 
#define NUM_LAST_CG_CTX_LUMA        12
#define NUM_LAST_CG_CTX_CHROMA       6
#define NUM_SIGCG_CTX_LUMA           2
#define NUM_SIGCG_CTX_CHROMA         1
#define NUM_LAST_POS_CTX_LUMA       56
#define NUM_LAST_POS_CTX_CHROMA     16
#define NUM_ALF_LCU_CTX              4
#define NUM_LAST_CG_CTX             (NUM_LAST_CG_CTX_LUMA+NUM_LAST_CG_CTX_CHROMA)
#define NUM_SIGCG_CTX               (NUM_SIGCG_CTX_LUMA+NUM_SIGCG_CTX_CHROMA)
#define NUM_LAST_POS_CTX            (NUM_LAST_POS_CTX_LUMA+NUM_LAST_POS_CTX_CHROMA)
#define NUM_SAO_MERGE_FLAG_CTX       3
#define NUM_SAO_MODE_CTX             1
#define NUM_SAO_OFFSET_CTX           2
#define NUM_INTER_DIR_MIN_CTX        2

#define RANK_NUM 6
#define LBAC_CTX_LEVEL                      (RANK_NUM * 4)
#define LBAC_CTX_RUN                        (RANK_NUM * 4)
#define LBAC_CTX_RUN_RDOQ                   (RANK_NUM * 4)
#define LBAC_CTX_LAST1                       RANK_NUM
#define LBAC_CTX_LAST2                      12

#define LBAC_CTX_CBF                        3       /* number of context models for QT CBF */

#define LBAC_CTX_SPLIT_FLAG                 4
#define LBAC_CTX_SPLIT_MODE                 3
#define LBAC_CTX_BT_SPLIT_FLAG              9
#define LBAC_CTX_SPLIT_DIR                  3
#define LBAC_CTX_DELTA_QP                   4

typedef signed long long   i64s_t;
typedef signed int         i32s_t;
typedef signed short       i16s_t;
typedef signed char        char_t;
typedef unsigned long long i64u_t;
typedef unsigned int       i32u_t;
typedef unsigned short     i16u_t;
typedef unsigned char      uchar_t;

typedef i16s_t    coef_t;
typedef i16s_t    resi_t;

typedef const i32s_t tab_i32s_t;
typedef const i16s_t tab_i16s_t;
typedef const i16u_t tab_i16u_t;
typedef const char_t tab_char_t;
typedef const uchar_t tab_uchar_t;

typedef const double tab_double_t;

#if defined(__linux__)
typedef i64s_t intptr_t;
#endif


/* ---------------------------------------------------------------------------
 * unions for type-punning.
 * Mn: load or store n bits, aligned, native-endian
 * CPn: copy n bits, aligned, native-endian
 * we don't use memcpy for CPn because memcpy's args aren't assumed
 * to be aligned */
typedef union {
    i16u_t    i;
    uchar_t   c[2];
} avs3_union16_t;
typedef union {
    i32u_t    i;
    i16u_t    b[2];
    uchar_t   c[4];
} avs3_union32_t;
typedef union {
    i64u_t    i;
    i32u_t    a[2];
    i16u_t    b[4];
    uchar_t     c[8];
} avs3_union64_t;



#if defined(_WIN64) && !defined(__GNUC__)

#include <mmintrin.h>
#include <emmIntrin.h>
#include <tmmIntrin.h>
#include <smmIntrin.h>
#include <immintrin.h>

#define M128(src) (*(__m128*)(src))

#else

typedef struct {
    i64u_t    i[2];
} avs3_uint128_t;

typedef union {
    avs3_uint128_t i;
    i64u_t    a[2];
    i32u_t    b[4];
    i16u_t    c[8];
    uchar_t   d[16];
} avs3_union128_t;

#define M128(src)               (((avs3_union128_t*)(src))->i)

#endif // defined(_MSC_VER)

#define M16(src)                (((avs3_union16_t*)(src))->i)
#define M32(src)                (((avs3_union32_t*)(src))->i)
#define M64(src)                (((avs3_union64_t*)(src))->i)

#define CP16(dst,src)           M16(dst) = M16(src)
#define CP32(dst,src)           M32(dst) = M32(src)
#define CP64(dst,src)           M64(dst) = M64(src)
#define CP128(dst,src)          M128(dst) = M128(src)


#define avs3d_inline __inline

#ifdef __GNUC__
#    define AVS3_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > x || __GNUC__ == x && __GNUC_MINOR__ >= y)
#else
#    define AVS3_GCC_VERSION_AT_LEAST(x,y) 0
#endif

#if AVS3_GCC_VERSION_AT_LEAST(3,1)
#    define avs3_always_inline __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#    define avs3_always_inline __forceinline
#else
#    define avs3_always_inline inline
#endif

#if AVS3_GCC_VERSION_AT_LEAST(3,1)
#    define avs3_no_inline __attribute__((noinline))
#elif defined(_MSC_VER)
#    define avs3_no_inline __declspec(noinline)
#else
#    define avs3_no_inline
#endif

static __inline pel_t avs3_pixel_clip(int x, int max_pixel_val)
{
    return (pel_t)((x < 0)? 0 : ((x > max_pixel_val)? max_pixel_val : x));
}

#endif // #if _DEFINES_H_
