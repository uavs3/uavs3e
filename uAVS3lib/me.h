#ifndef _MV_SEARCH_H_
#define _MV_SEARCH_H_

#define IME_TZ  0
#define IME_HEX 1

#include "commonStructures.h"

int me_module_init(me_info_t *me);
void avs3_me_free(me_info_t *me);
void me_module_create_mvcost(me_info_t *me, int motion_factor);

int pu_motion_search(avs3_enc_t *h, me_info_t *me, int ref, int bit_size);
int pu_motion_search_b(avs3_enc_t *h, me_info_t *me, int mode, int bit_size, int *mcost_bid, int min_mcost);

int me_pre_search(me_info_t *me, i16s_t *pmv, i16s_t *mv, i32u_t *ret_mcost, i32u_t *d_mcost, int skip_lines);
int me_integer_hex(me_info_t *me, i16s_t *pmv, i16s_t *mv, i32u_t *d_mcost, i32u_t *s_mcost, i32u_t min_mcost, int skip_lines);
int me_integer_tz (me_info_t *me, i16s_t *pmv, i16s_t *mv, i32u_t *d_mcost, i32u_t *s_mcost, i32u_t min_mcost, int skip_lines, int layer);

#endif