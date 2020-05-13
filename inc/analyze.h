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
*    This product includes software developed by the <organization>.
* 4. Neither the name of the <organization> nor the
*    names of its contributors may be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* For more information, contact us at rgwang@pkusz.edu.cn.
**************************************************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "modules.h"

#define RATE_TO_COST_LAMBDA(l, r)       ((double)r * l)
#define RATE_TO_COST_SQRT_LAMBDA(l, r)  ((double)r * l)

int  enc_mode_init_lcu     (core_t *core);
int  enc_mode_analyze_lcu  (core_t *core, const lbac_t *lbac);

void enc_bits_intra        (core_t *core, lbac_t *lbac, s32 slice_type, s16 coef[N_C][MAX_CU_DIM]);
void enc_bits_intra_pu     (core_t *core, lbac_t *lbac, s32 slice_type, s16 coef[N_C][MAX_CU_DIM], int pb_part_idx);
void enc_bits_intra_chroma (core_t *core, lbac_t *lbac, s16 coef[N_C][MAX_CU_DIM]);
void enc_bits_inter        (core_t *core, lbac_t *lbac, s32 slice_type);
void enc_bits_inter_comp   (core_t *core, lbac_t *lbac, s16 coef[MAX_CU_DIM], int ch_type);

void enc_bits_inter_skip_flag(core_t *core, lbac_t *lbac);

s64 calc_dist_filter_boundary(core_t *core, com_pic_t *pic_rec, com_pic_t *pic_org, int cu_width, int cu_height, pel *src, int s_src, int x, int y, u8 intra_flag, u8 cu_cbf, s8 *refi, s16(*mv)[MV_D], u8 is_mv_from_mvf, int only_delta);

void analyze_inter_cu(core_t *core, lbac_t *lbac_best);
double pinter_residue_rdo_chroma(core_t *core);
u64 me_search_tz(inter_search_t *pi, int x, int y, int w, int h, int pic_width, int pic_height, s8 refi, int lidx, const s16 mvp[MV_D], s16 mv[MV_D], int bi);

void inter_search_init(inter_search_t *pi, com_info_t *info, int is_padding);
int  inter_search_create(u8 **pptab, com_info_t *info);
void inter_search_free(u8 *tab_mvbits, int tab_mvbits_offset);

void analyze_intra_cu(core_t *core, lbac_t *lbac_best_ret);

int enc_tq_itdq_yuv_nnz(core_t *core, lbac_t *lbac, com_mode_t *cur_mode, s16 coef[N_C][MAX_CU_DIM], s16 resi[N_C][MAX_CU_DIM], pel pred[N_C][MAX_CU_DIM], pel rec[N_C][MAX_CU_DIM], s8 refi[REFP_NUM], s16 mv[REFP_NUM][MV_D]);

static avs3_always_inline int enc_tq_nnz(core_t *core, com_mode_t *mode, int plane, int blk_idx, int qp, double lambda, s16 *coef, s16 *resi, int cu_width_log2, int cu_height_log2, int slice_type, int is_intra, int secT_Ver_Hor, int use_alt4x4Trans)
{
    transform(mode, plane, blk_idx, coef, resi, cu_width_log2, cu_height_log2, core->info->bit_depth_internal, secT_Ver_Hor, use_alt4x4Trans);
    return quant_non_zero(core, qp, lambda, is_intra, coef, cu_width_log2, cu_height_log2, plane, slice_type);
}


static avs3_always_inline double get_bits_cost(core_t *core, lbac_t* lbac, int slice_type, double lambda)
{
    lbac_copy(lbac, &core->lbac_bakup);
    int bit_cnt = lbac_get_bits(lbac);
    enc_bits_inter(core, lbac, slice_type);
    bit_cnt = lbac_get_bits(lbac) - bit_cnt;
    return lambda * bit_cnt;
}

static avs3_always_inline double get_bits_cost_comp(core_t *core, lbac_t* lbac, lbac_t *lbac_bakup, s16* coef, double lambda, int ch_type)
{
    lbac_copy(lbac, lbac_bakup);
    int bit_cnt = lbac_get_bits(lbac);
    enc_bits_inter_comp(core, lbac, coef, ch_type);
    bit_cnt = lbac_get_bits(lbac) - bit_cnt;
    return lambda * bit_cnt;
}

static avs3_always_inline double get_bits_cost_coef(core_t *core, lbac_t* lbac, s16 coef[N_C][MAX_CU_DIM])
{
    com_mode_t *cur_info = &core->mod_info_curr;
    lbac_copy(lbac, &core->lbac_bakup);
    int bit_cnt = lbac_get_bits(lbac);
    lbac_enc_coef(lbac, NULL, core, coef, core->cu_width_log2, core->cu_height_log2, cur_info->cu_mode, cur_info, core->tree_status); // only count coeff bits for chroma tree
    bit_cnt = lbac_get_bits(lbac) - bit_cnt;
    return core->lambda[0] * bit_cnt;
}

#endif /* _ANALYZE_H_ */