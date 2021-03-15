#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "commonVariables.h"
#include "defines.h"

#define shift_trans 10   // 8-pt trans: 9 + 17 - 16 = 10
#define shift_out   15   // 8-pt inv_trans: 13 + (28-17) - 9 = 15#

#define LIMIT_BIT       16
#define FACTO_BIT       5

#define CHECK_FAST_TRQ(w, t) (((w) == 32 || (w) == 64) && ((t) == B_FRM))

extern char com_tbl_tm2[3][2][2];
extern char com_tbl_tm4[3][4][4];
extern char com_tbl_tm8[3][8][8];
extern char com_tbl_tm16[3][16][16];
extern char com_tbl_tm32[3][32][32];
extern char com_tbl_tm64[3][64][64];

extern double com_tbl_sqrt[2];

typedef enum uavs3e_trans_type_t {
    DCT2,
    DCT8,
    DST7,
    NUM_TRANS_TYPE
} trans_type_t;

void transform_blk     (avs3_enc_t *h, int blkidx, pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int i_dst, int tu_bitsize, cu_t *cu, int b_chroma);
void inv_transform     (avs3_enc_t *h, int blkidx, coef_t *src, int i_src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int tu_bitsize, cu_t *cu, int b_chroma);

extern tab_i16s_t tab_iq_shift[80];
extern tab_i16u_t tab_iq_tab[80];
extern tab_i16u_t tab_q_tab[80];

#endif //#ifndef _TRANSFORM_H_