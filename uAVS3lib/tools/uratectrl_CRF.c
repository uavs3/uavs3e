#include "uratectrl.h"
#include <math.h>
#include <stdio.h>
#include "../common.h"

#define CRF_BETA1 2
#define QCompress 0.6

typedef struct rc_crf_ctrl
{
    /* Sequence level const value */
    cfg_param_t *enc_cfg;
    double m_rfConst;
    int m_anchor_qp;

    /* Sequence level */
    int    last_ip_type;
    double last_ip_qp;

    double m_shortTermCplxSum;
    double m_shortTermCplxCount;
    i64s_t m_subgops;

    /* Sub-GOP level */
    double curr_ip_qp;
    double curr_ip_lambda;
} rc_crf_ctrl;

void* rc_crf_init(cfg_param_t *input)
{
    rc_crf_ctrl *p = com_malloc(sizeof(rc_crf_ctrl), 0);
    double lambda;
    memset(p, 0, sizeof(rc_crf_ctrl));

    p->enc_cfg = input;
    p->m_anchor_qp = input->baseQP - input->bitdepth_qp_offset;

    lambda = pow(2.0, (p->m_anchor_qp - 13.131) / 5.661);
    lambda = pow(lambda, 1.0 / CRF_BETA1);

    p->m_rfConst = pow(1.0, QCompress) / lambda;

    return p;
}

void  rc_crf_get_qp(void *rc_handle, avs3_rc_get_qp_param_t *param)
{
    rc_crf_ctrl *prc = (rc_crf_ctrl*)rc_handle;
    cfg_param_t *input = prc->enc_cfg;
    int qp;
    double lambda;

    if (param->info.type == I_FRM && prc->m_subgops == 0) { // first I frame
        int tmp_qp = max(1, prc->m_anchor_qp - SHIFT_QP);
        lambda = 0.68 * pow(2, tmp_qp / 4.0) * 0.8;
    } 
    else if (param->info.type == B_FRM) {
        double baseQP = (prc->last_ip_qp + prc->curr_ip_qp) / 2 + input->seq_ref_cfg[param->info.frameLevel - 1].qp_offset - 1;
        baseQP = max(0, baseQP - SHIFT_QP);
        lambda = 0.68 * pow(2, baseQP / 4.0) * 1.2;
        lambda *= max(2.00, min(4.00, baseQP / 8.0));
    } 
    else {
        double blurredComplexity;

        prc->m_shortTermCplxSum *= 0.5;
        prc->m_shortTermCplxCount *= 0.5;
        prc->m_shortTermCplxSum += max(param->info.inter_cpp, 0.2);
        prc->m_shortTermCplxCount++;

        blurredComplexity = pow(prc->m_shortTermCplxSum / prc->m_shortTermCplxCount, QCompress);

        param->info.pic_cpp = blurredComplexity;

        if (prc->m_subgops == 0) {
            if (prc->last_ip_type & FP_FLG) {
                lambda = prc->curr_ip_lambda;
            } else {
                double baseQP = max(0, prc->curr_ip_qp + 1 - SHIFT_QP);
                lambda = 0.68 * pow(2, baseQP / 4.0);
            }
        } else {
            lambda = pow(blurredComplexity / prc->m_rfConst, CRF_BETA1);

            if (param->info.type & I_FRM) {
                lambda *= 0.86;
            }
            lambda = min(lambda, 4 * prc->curr_ip_lambda);
        }
        prc->m_subgops++;
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

void  rc_crf_update(void *rc_handle, avs3_rc_update_param_t *param, char *ext_info, int info_len)
{   
    sprintf(ext_info, " %7.2f", param->info.lambda);

    if (param->info.type != B_FRM) {
        sprintf(ext_info, "%s  %.2f", ext_info, param->info.pic_cpp);
    }
}

void  rc_crf_destroy(void *rc_handle)
{
    com_free(rc_handle);
}