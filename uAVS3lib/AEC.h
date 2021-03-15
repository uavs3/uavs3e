#ifndef _AEC_H_
#define _AEC_H_

#include "global.h"
#include "biariencode.h"
#include "commonVariables.h"

typedef enum uavs3e_split_mode_t {
    NO_SPLIT = 0,
    SPLIT_BI_VER = 1,
    SPLIT_BI_HOR = 2,
    SPLIT_EQT_VER = 3,
    SPLIT_EQT_HOR = 4,
    SPLIT_QUAD = 5,
    NUM_SPLIT_MODE
} split_mode_t;

typedef enum uavs3e_split_dir_t {
    SPLIT_VER = 0,
    SPLIT_HOR = 1,
    SPLIT_QT = 2,
} split_dir_t;

void copy_coding_state(aec_t *dst, aec_t *src);

#define store_coding_state(cs) copy_coding_state(cs, &h->aec);
#define reset_coding_state(cs) copy_coding_state(&h->aec, cs);

void copy_coding_state_hdr(aec_t *dst, aec_t *src);

#define store_coding_state_hdr(cs) copy_coding_state_hdr(cs, &h->aec);
#define reset_coding_state_hdr(cs) copy_coding_state_hdr(&h->aec, cs);

void arienco_start_encoding(aec_t *aec, uchar_t *code_buffer, int *code_len);
void arienco_done_encoding(aec_t *aec);

#define arienco_bits_written(aec) aec->Ebits_cnt
#define arienco_bits_real_written(aec) ((*aec->Ecodestrm_len + aec->num_buffer_bytes) * 8 + (23 - aec->bits_left))

void init_contexts(bin_ctx_sets_t *ctx_sets);

void set_aec_functions(aec_t *aec, int flag);

void write_terminating_bit(aec_t *aec, uchar_t);
void writeIntraPredMode  (aec_t *aec, int val);
void writeMVD_AEC        (aec_t *aec, int val, int xy);
int  writeSplitFlag_AEC  (aec_t *aec, int val, int cu_bitsize);

void writePdir           (cu_t *cu, aec_t *aec, int dir);

void writeRefFrame       (avs3_enc_t *h, aec_t *aec, int val);
void writeCIPredMode     (avs3_enc_t *h, aec_t *aec, int val, int luma_mode);

void writeCuTypeInfo     (avs3_enc_t *h, cu_t *cu, aec_t *aec);
void writeCBP            (avs3_enc_t *h, cu_t *cu, aec_t *aec, int val);

int write_sao_mergeflag(aec_t *aec, int mergeleft_avail, int mergeup_avail, SAOBlkParam *saoBlkParam);
int write_sao_mode     (aec_t *aec, SAOBlkParam *saoBlkParam);
int write_sao_offset   (aec_t *aec, SAOBlkParam *saoBlkParam);
int write_sao_type     (aec_t *aec, SAOBlkParam *saoBlkParam);

void write_run_length_cc(aec_t *aec, i16s_t *coef, int uiBitSize, int bluma);

extern tab_i32s_t tab_saoclip[NUM_SAO_OFFSET][3];
extern tab_i32s_t tab_eo_offset_map[];

int lbac_enc_split_mode(avs3_enc_t *h, aec_t *aec, int split_mode, int cu_width, int cu_height, int qt_depth, int bet_depth, int x, int y);

void lbac_enc_lcu_delta_qp(aec_t *lbac, int val, int last_dqp);

#endif  // AEC_H

