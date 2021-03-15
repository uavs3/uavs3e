#ifndef _RDOQH_
#define _RDOQH_

#include "global.h"
#include "commonVariables.h"
#include "block.h"

#include "codingUnit.h"

typedef struct level_data_struct {
    int     level[2];     // candidate levels
    float   errLevel[3];  // quant errors of each candidate
    int     noLevels;     // number of candidate levels
    int     scanPos;      // position in transform block zig-zag scan order
} levelDataStruct;

void rdoq_init_err_scale(int bit_depth);
void rdoq_init_prob_2_bits();
void rdoq_init_cu_est_bits(avs3_enc_t *core, aec_t *aec);

int rdoq_quant_block(avs3_enc_t *core, int slice_type, int qp, double d_lambda, int is_intra,
    i16s_t *coef, int cu_width_log2, int cu_height_log2, int ch_type, int bit_depth);


#endif