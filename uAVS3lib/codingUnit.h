#ifndef _MACROBLOCK_H_
#define _MACROBLOCK_H_

#include "global.h"
#include "commonVariables.h"

void inter_pred_one_pu(avs3_enc_t *h, cu_t *cu, int pu_idx, int need_pred_luma);
void inter_pred_one_pu_luma(avs3_enc_t *h, cu_t *cu, pel_t *pred, int pu_idx);

#endif

