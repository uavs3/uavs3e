#ifndef _AVS_TRANSFORM_H_
#define _AVS_TRANSFORM_H_

#include "global.h"
#include "commonVariables.h"
#include "transform.h"

#define ERR_SCALE_PRECISION_BITS 20

void com_scan_tbl_init();
void com_scan_tbl_delete();

void com_update_cand_list(int uiMode, double uiCost, int uiFullCandNum, int *CandModeList, double *CandCostList);
int quant_blk(avs3_enc_t *h, aec_t *aec, int qp, int mode, coef_t *curr_blk, int tu_bitsize, cu_t *cu, int plane_id, int intraPredMode);
void inv_quant_blk(avs3_enc_t *h, int qp, coef_t *src, int i_src, coef_t *dst, int i_dst, int uiBitSize, int isChroma);

extern tab_i32s_t tab_max_coef_factor[2][5];

#if defined(_MSC_VER)  
#pragma warning(disable: 4127)  // conditional expression is constant
#endif

static __inline void scan_cg_coef(i16u_t *dst, i16u_t *src, tab_i16u_t *p_scan)
{
#define SCAN_ONE_COEF(x) dst[x] = src[p_scan[x]];

    SCAN_ONE_COEF( 0);
    SCAN_ONE_COEF( 1);
    SCAN_ONE_COEF( 2);
    SCAN_ONE_COEF( 3);
    SCAN_ONE_COEF( 4);
    SCAN_ONE_COEF( 5);
    SCAN_ONE_COEF( 6);
    SCAN_ONE_COEF( 7);
    SCAN_ONE_COEF( 8);
    SCAN_ONE_COEF( 9);
    SCAN_ONE_COEF(10);
    SCAN_ONE_COEF(11);
    SCAN_ONE_COEF(12);
    SCAN_ONE_COEF(13);
    SCAN_ONE_COEF(14);
    SCAN_ONE_COEF(15);

#undef SCAN_ONE_COEF
}

static __inline void inv_scan_zero_cg(i16u_t *dst, int i_dst)
{
    *(i64s_t*)(dst) = 0;
    *(i64s_t*)(dst + i_dst) = 0;
    *(i64s_t*)(dst + (i_dst << 1)) = 0;
    *(i64s_t*)(dst + (i_dst << 1) + i_dst) = 0;
}

#endif // _AVS_TRANSFORM_H_
