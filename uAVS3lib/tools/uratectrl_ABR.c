#include "uratectrl.h"
#include <math.h>
#include <stdio.h>
#include "../common.h"

#define ABR_BETA1 2

typedef struct rc_abr_ctrl
{
    /* Sequence level const value */
    cfg_param_t *enc_cfg;
    double m_seq_tar_bit_rate;
    double m_seq_avg_frm_bits;
    double m_seq_frm_rate;
    double m_seq_avg_subgop_bpp;
    int m_seq_pic_pixels;

    /* Sequence level */
    int    last_ip_type;
    double last_ip_qp;
    double last_finished_ip_lambda;
    double last_finished_ip_cplxr;

    double m_shortTermCplxSum;
    double m_shortTermCplxCount;
    double m_cplxrSum;
    double m_total_err_bits;
    i64s_t m_total_bits;
    i64s_t m_total_frms;
    i64s_t m_subgops;

    /* Sub-GOP level */
    int    m_subgop_bfrms;
    double m_bpp_in_subgop;
    double curr_ip_qp;
    double curr_ip_lambda;
} rc_abr_ctrl;

void* rc_abr_init(cfg_param_t *input)
{
    rc_abr_ctrl *p = com_malloc(sizeof(rc_abr_ctrl), 0);

    memset(p, 0, sizeof(rc_abr_ctrl));

    p->enc_cfg = input;
    p->m_seq_tar_bit_rate = input->target_bitrate * 1000.0;
    p->m_seq_frm_rate = input->frame_rate;

    p->m_seq_pic_pixels = input->img_width * input->img_height;
    p->m_seq_avg_frm_bits = p->m_seq_tar_bit_rate / p->m_seq_frm_rate;
    p->m_seq_avg_subgop_bpp = p->m_seq_avg_frm_bits * 8 / p->m_seq_pic_pixels;

    return p;
}

void  rc_abr_get_qp(void *rc_handle, avs3_rc_get_qp_param_t *param)
{
    rc_abr_ctrl *prc = (rc_abr_ctrl*)rc_handle;
    cfg_param_t *input = prc->enc_cfg;
    int qp;
    double lambda;

    if (param->info.type == I_FRM && prc->m_subgops == 0) { // first I frame
        double cpp = pow(param->info.pic_cpp, 1.31);
        double bpp = prc->m_seq_avg_subgop_bpp / 8;
        double alpha = bpp < 0.025 ? 0.25 : 0.3;
        double ratio = 0.7 * alpha* pow(pow(cpp, 1.0 / 1.31) * 4.0 / bpp, 0.5582);
        bpp *= ratio;
        lambda = 6.7542 / 256 * pow(bpp / cpp, -1.7860);
    } 
    else if (param->info.type == B_FRM) {
        double baseQP = (prc->last_ip_qp + prc->curr_ip_qp) / 2 + input->seq_ref_cfg[param->info.frameLevel - 1].qp_offset - 1;
        baseQP = max(0, baseQP - SHIFT_QP);
        lambda = 0.68 * pow(2, baseQP / 4.0) * 1.2;
        lambda *= max(2.00, min(4.00, baseQP / 8.0));
    } 
    else {
        double blurredComplexity;

        if (prc->last_finished_ip_cplxr > 0.53) {
            prc->m_cplxrSum += prc->m_bpp_in_subgop * pow(prc->last_finished_ip_lambda, 1.0 / ABR_BETA1) / prc->last_finished_ip_cplxr;
            prc->m_subgops++;
        }
        prc->m_bpp_in_subgop = 0;
        prc->m_subgop_bfrms = 0;

        prc->m_shortTermCplxSum *= 0.5;
        prc->m_shortTermCplxCount *= 0.5;
        prc->m_shortTermCplxSum += max(param->info.inter_cpp, 0.2);
        prc->m_shortTermCplxCount++;

        blurredComplexity = pow(prc->m_shortTermCplxSum / prc->m_shortTermCplxCount, 0.4);

        param->info.pic_cpp = blurredComplexity;

        if (prc->m_subgops == 0) {
            if (prc->last_ip_type & FP_FLG) {
                lambda = prc->curr_ip_lambda;
            } else {
                double baseQP = max(0, prc->curr_ip_qp + 1 - SHIFT_QP);
                lambda = 0.68 * pow(2, baseQP / 4.0);
            }
        } else {
            double timeDone = prc->m_subgops * 8 / prc->m_seq_frm_rate;
            double abrBuffer = prc->m_seq_tar_bit_rate * 2 * max(sqrt(timeDone), 1);

            lambda = pow(blurredComplexity * (prc->m_cplxrSum / prc->m_subgops) / prc->m_seq_avg_subgop_bpp, ABR_BETA1);

            if (param->info.type & I_FRM) {
                lambda *= 0.86;
            }
            lambda = min(lambda, 4 * prc->curr_ip_lambda);
            lambda *= Clip3(0.5, 2.0, 1.0 + prc->m_total_err_bits / abrBuffer);
        }
    }
    lambda = max(lambda, 0.1);

    qp = (int)(5.661 * log(lambda) + 13.131 + 0.5);
    qp = Clip3(0, MAX_QP, qp);
    param->info.qp = qp;
    param->info.lambda = lambda;

    if (param->info.type != B_FRM) {
        prc->last_ip_qp = prc->curr_ip_qp;
        prc->curr_ip_qp = qp; 
        prc->curr_ip_lambda = lambda; 
        prc->last_ip_type = param->info.type;
    }
}

void  rc_abr_update(void *rc_handle, avs3_rc_update_param_t *param, char *ext_info, int info_len)
{   
    rc_abr_ctrl *prc = (rc_abr_ctrl*)rc_handle;
    cfg_param_t *input = prc->enc_cfg;

    int bits = param->header_bits + param->data_bits;

    prc->m_bpp_in_subgop += bits * 1.0 / prc->m_seq_pic_pixels;
     

    if (param->info.type == B_FRM) {
        prc->m_subgop_bfrms++;
    } else {
        prc->last_finished_ip_cplxr = param->info.pic_cpp;
        prc->last_finished_ip_lambda = param->info.lambda;
    }

    prc->m_total_frms++;
    prc->m_total_bits += bits;
    prc->m_total_err_bits += bits - prc->m_seq_avg_frm_bits;

    sprintf(ext_info, " %9.2f   %7.2f  %f  %f %f", prc->m_total_bits / prc->m_total_frms / prc->m_seq_avg_frm_bits * prc->m_seq_tar_bit_rate / 1000 , 
        param->info.lambda, prc->m_total_err_bits, prc->last_finished_ip_cplxr, prc->m_cplxrSum / prc->m_subgops);
}

void  rc_abr_destroy(void *rc_handle)
{
    com_free(rc_handle);
}