#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>

#include "defines.h"
#include "commonStructures.h"
#include "commonVariables.h"
#include "common.h"
#include "inter-prediction.h"

#include "me.h"

int inter_luma_coding(avs3_enc_t *h, cu_t *cu, aec_t *aec, int uiBitSize, int force_zero, int cal_ssd);
int chroma_residual_coding(avs3_enc_t *h, cu_t *cu, aec_t *aec, int uiBitSize, int force_zero, int cal_ssd);

void intra_pred_chroma(avs3_enc_t *h, int uiBitSize, int luma_mode, int mode);
double write_one_cu_rdo(avs3_enc_t *h, aec_t *aec, cu_t *cu, int uiBitSize, double lambda, double mcost);
int  writeLumaCoeff8x8(int **Quant_Coeff, cu_t *cu, int uiBitSize, unsigned int uiPixInSMB_x, unsigned int uiPixInSMB_y, int block8x8, int intra8x8mode, int uiPosition);

void no_mem_exit(char_t *where);

void init_frame(avs3_ctrl_t *ctrl, avs3_enc_t *pic, frame_t *frm, double* cpp_list, int cpps);
frame_t* prepare_RefInfo(avs3_ctrl_t *ctrl, ref_man* curr_RPS, int img_type, i64s_t poc);

void intra_prepare_edge_luma(avs3_enc_t *h, cu_t *cu, pel_t *EP, int uiBitSize, int *ava_up, int *ava_left);

void get_cu_mvd(avs3_enc_t *h, cu_t *cu, int uiBitSize);
double analyze_cu_tree(avs3_enc_t *h, aec_t *aec, int uiBitSize, int uiMaxBitSize, int uiMinBitSize, int uiPositionInPic, int qp, double lambda, double mcost, double *est_bits);

void  init_lcu_pos(avs3_enc_t *h, int lcu_idx); 

void  Init_Curr_codingUnit(avs3_enc_t *h, cu_t *tmpMB, int smbSize, int uiPositionInPic, int qp, double lambda); 
void  write_cu_tree(avs3_enc_t *h, aec_t *aec, int uiBitSize, int uiPositionInPic);

void  error(char_t *text, int code);
void  write_coeffs(avs3_enc_t *h, aec_t *aec, coef_t* p_coef, cu_t *cu, int uiBitSize, int bluma, int max_bits);



#define MAX_REGION_NUM  100

/* ---------------------------------------------------------------------------
* run-level infos
*/
typedef struct runlevel_t {
    int    DCT_pairs;
    i16s_t DCT_Level[64 * 64 + 1];
    i16s_t DCT_Run[64 * 64 + 1];

    int    DCT_CGs;
    i16s_t DCT_PairsInCG[CG_SIZE *CG_SIZE + 1];
    i16s_t DCT_CGLastIdx[CG_SIZE *CG_SIZE + 1];
} runlevel_t;

void sao_lcu_write(avs3_enc_t *h, int lcu_x, int lcu_y, aec_t *aec, SAOBlkParam *sao_cur_param);

void deblock_one_lcu(avs3_enc_t *h, image_t *img, int lcu_x, int lcu_y);

void set_cu_deblk_flag(avs3_enc_t *h, int uiBitSize, unsigned int uiPositionInPic);

#define TEST_CNT(equ1, equ2, print)                         \
    if (equ1){                                              \
        static int all_cnt = 0;                             \
        static int hit_cnt = 0;                             \
        all_cnt++;                                          \
        if (equ2) {                                         \
            hit_cnt++;                                      \
        }                                                   \
        if (all_cnt % print == 0) {                         \
            printf("%5.2f\n", hit_cnt * 100.0 / all_cnt);   \
        }                                                   \
    }

static void __inline wait_ref_available(avs3_enc_t *h, frame_t *frm, int lines)
{
    int real_lines = Clip3(0, h->input->img_height, lines);

    if (frm->finished_lines < real_lines) {
        avs3_pthread_mutex_lock(&frm->mutex);

        while (frm->finished_lines < real_lines) {
            avs3_pthread_cond_wait(&frm->cv, &frm->mutex);
        }

        avs3_pthread_mutex_unlock(&frm->mutex);
    }
}


#endif