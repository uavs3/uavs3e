#ifndef __INTER_PREDICTION_H__
#define __INTER_PREDICTION_H__

#include "commonVariables.h"
#include "defines.h"

#define get_safe_mv(img_size, blk_size, mv) min((img_size) + 4, max(-(blk_size) - 4, (mv)))
#define check_safe_mv(img_size, blk_size, mv) assert((mv) <= (img_size) + 4 && (mv) >= -(blk_size) - 4)

void refine_col_pos(avs3_enc_t *h, int x, int y, int *real_x, int *real_y);

void com_if_luma_frame(avs3_enc_t *h, image_t img_list[4][4]);
int com_if_luma_lcu_row(avs3_enc_t *h, image_t img_list[4][4], int lcu_y, uavs3e_sem_t *sem_up, uavs3e_sem_t *sem_curr);

void get_col_pskip_mv(avs3_enc_t *h, int uiBitSize);
void get_col_bskip_mv(avs3_enc_t *h, int uiBitSize);
void get_mhp_bskip_mv(avs3_enc_t *h, int uiBitSize);
void get_predicted_mv(avs3_enc_t *h, i16s_t *pmv, int  ref_frame, int  ref, int *neighbor_ref, i16s_t (*neighbor_mv)[2]);

static void scaling_mv(int dist_cur, int dist_neb, i16s_t mvp[2], i16s_t mv[2])
{
#define COM_INT16_MAX           ((i16s_t)0x7FFF)
#define COM_INT16_MIN           ((i16s_t)0x8000)
#define COM_CLIP3(min_x, max_x, value)  COM_MAX((min_x), COM_MIN((max_x), (value)))
#define MV_X                               0
#define MV_Y                               1

    M32(mv) = 0;

    if (M32(mvp)) {
        if (dist_neb == dist_cur && (1 << 14) % dist_neb == 0) {
            M32(mv) = M32(mvp);
        } else {
            const int offset = 1 << (14 - 1);
            int ratio = offset / dist_neb * dist_cur << 1; // note: divide first for constraining bit-depth

            if (mvp[MV_X]) {
                i64s_t tmp_mv = (i64s_t)mvp[MV_X] * ratio;
                i64s_t mask = tmp_mv >> 63;
                tmp_mv = (mask ^ tmp_mv) - mask;
                tmp_mv = (mask ^ ((tmp_mv + offset) >> 14)) - mask;
                mv[MV_X] = (i16s_t)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, tmp_mv);
            }
            if (mvp[MV_Y]) {
                i64s_t tmp_mv = (i64s_t)mvp[MV_Y] * ratio;
                i64s_t mask = tmp_mv >> 63;
                tmp_mv = (mask ^ tmp_mv) - mask;
                tmp_mv = (mask ^ ((tmp_mv + offset) >> 14)) - mask;
                mv[MV_Y] = (i16s_t)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, tmp_mv);
            }
        }
    }
}

#endif // #ifndef __INTER_PREDICTION_H__