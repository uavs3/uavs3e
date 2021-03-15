#include "uratectrl.h"
#include <math.h>
#include <stdio.h>
#include "../common.h"

#define g_RCMaxPicListSize 40
#define COST_BETA_I 1.33
#define MIN_INTER_CPP 0.1

const int g_RCInvalidQPValue = -999;
const double g_RCAlphaMinValue = 0.01;
const double g_RCAlphaMaxValue = 500.0;
const double g_RCBetaMinValue = -3.0;
const double g_RCBetaMaxValue = -0.2;

typedef struct coded_pic_info {
    int frameLevel;
    double lambda;
} coded_pic_info;

typedef struct rlambda_param
{
    double m_alpha;
    double m_beta;
} rlambda_param;

typedef struct rc_rlambda_ctrl
{
    /* Sequence level const value */
    cfg_param_t *enc_cfg;
    double m_seq_tar_bit_rate;
    double m_seq_alpha_ratio;
    double m_seq_beta_ratio;
    double m_seq_pic_pixels;
    double m_seq_avg_bits;
    double m_seq_frm_rate;
    int    m_seq_smooth_window;
    int    m_seq_gopid_2_level[8];

    /* Sequence level */
    coded_pic_info m_seq_coded_frm_list[g_RCMaxPicListSize + 1];
    int m_seq_coded_frm_idx;
    int m_seq_coded_frm_list_size;
    rlambda_param m_seq_pic_params[5];
    rlambda_param m_seq_pic_params_old[5];
    double       m_seq_bits_error;
    double       m_seq_last_lambda;

    /* GOP level info data */
    int m_gop_coded_bits;
    int m_gop_coded_frms;

    /* sub-GOP level */
    double m_sub_gop_pic_bits[8];
    int m_sub_gop_pic;
    int m_sub_gop_pic_left;
    double m_sub_gop_bits_left;
    int m_scene_cut_flag;
} rc_rlambda_ctrl;

static double rlambda_solve_equa(rc_rlambda_ctrl *rc_ctrl, double targetBpp, double* equaCoeffA, double* equaCoeffB, double GOPSize)
{
    double solution = 100.0;
    double minNumber = 0.1;
    double maxNumber = 10000.0;
    int i, j;

    for (i = 0; i < 20; i++) {
        double fx = 0.0;
        for (j = 0; j < GOPSize; j++) {
            fx += equaCoeffA[j] * pow(solution, equaCoeffB[j]);
        }

        if (fabs(fx - targetBpp) < 0.000001) {
            break;
        }

        if (fx > targetBpp) {
            minNumber = solution;
            solution = (solution + maxNumber) / 2.0;
        }
        else {
            maxNumber = solution;
            solution = (solution + minNumber) / 2.0;
        }
    }

    solution = Clip3(0.1, 10000.0, solution);
    return solution;
}

static double rlambda_estimatePicLambda(rc_rlambda_ctrl *rc_ctrl, double pic_tar_bits, double cpp, double intra_cpp, int frameLevel, int scene_cut, int *codec_list_idx, double *ref_lambda)
{
    double alpha = rc_ctrl->m_seq_pic_params[frameLevel].m_alpha;
    double beta  = rc_ctrl->m_seq_pic_params[frameLevel].m_beta;
    double bpp = pic_tar_bits / rc_ctrl->m_seq_pic_pixels / cpp;
    double estLambda = alpha * pow(bpp, beta);

    if (frameLevel != 0) {
        double lastLevelLambda   = -1.0;
        double lastUpLevelLambda = -1.0;
        int i, idx = (rc_ctrl->m_seq_coded_frm_idx + g_RCMaxPicListSize + 1 - rc_ctrl->m_seq_coded_frm_list_size) % (g_RCMaxPicListSize + 1);

        for (i = 0; i < rc_ctrl->m_seq_coded_frm_list_size; i++) {
            coded_pic_info *info = &rc_ctrl->m_seq_coded_frm_list[idx];
            if (info->frameLevel == frameLevel) {
                lastLevelLambda = info->lambda;
            } else if (frameLevel == 1 && info->frameLevel == 0) {
                lastUpLevelLambda = info->lambda;
                info->lambda *= pow(2.0, -4.0 / 4.0);
            }

            idx = (idx + 1) % (g_RCMaxPicListSize + 1);
        }
        if (lastUpLevelLambda > 0.0) {
            estLambda = max(lastUpLevelLambda * pow(2.0, -3.0 / 4.0), estLambda);
        }
        if (lastLevelLambda > 0.0) {
            if (frameLevel == 1) {
                estLambda = max(lastLevelLambda * pow(2.0, -5.0 / 4.0), estLambda);
            } else {
                estLambda = max(lastLevelLambda * pow(2.0, -8.0 / 4.0), estLambda);
            }
        }
        if (frameLevel > 1) {
            double max_ref_lambda = max(ref_lambda[0], ref_lambda[1]);
            double limit_1 = (ref_lambda[0] + ref_lambda[1]) / 2;
            double limit_2 = max_ref_lambda * pow(2.0, -3.0 / 4.0);
            

            estLambda = max(limit_1, estLambda);
            estLambda = max(limit_2, estLambda);

            if (intra_cpp > 15) {
                double DQP = Clip3(-3, -1, 0.1333 * intra_cpp - 5);
                double limit = max_ref_lambda * pow(2.0, DQP / 4.0);
                estLambda = max(limit, estLambda);
            }
        } else {
            if (intra_cpp > 15) {
                double DQP = Clip3(-5, -2, 0.2 * intra_cpp - 8);
                double limit = ref_lambda[0] * pow(2.0, DQP / 4.0);
                estLambda = max(limit, estLambda);
            }
        }

        if (estLambda < 0.3) {
            estLambda = 0.3;
        }
    }

    if (!scene_cut || frameLevel < 2) {
        rc_ctrl->m_seq_coded_frm_list[rc_ctrl->m_seq_coded_frm_idx].frameLevel = frameLevel;
        rc_ctrl->m_seq_coded_frm_list[rc_ctrl->m_seq_coded_frm_idx].lambda = estLambda;
        *codec_list_idx = rc_ctrl->m_seq_coded_frm_idx;

        rc_ctrl->m_seq_coded_frm_idx = (rc_ctrl->m_seq_coded_frm_idx + 1) % (g_RCMaxPicListSize + 1);
        rc_ctrl->m_seq_coded_frm_list_size = min(rc_ctrl->m_seq_coded_frm_list_size + 1, g_RCMaxPicListSize + 1);
    } else {
        *codec_list_idx = -1;
    }

    return estLambda;
}
#include <stdio.h>

static void rlambda_updateAfterPicture(rc_rlambda_ctrl *rc_ctrl, int actualHeaderBits, int actualTotalBits, double averageQP, double averageLambda, int frameLevel, double cpp)
{
    double alpha = rc_ctrl->m_seq_pic_params[frameLevel].m_alpha;
    double beta  = rc_ctrl->m_seq_pic_params[frameLevel].m_beta;
    double bpp   = (double)actualTotalBits / (double)rc_ctrl->m_seq_pic_pixels / cpp;
    double calLambda = alpha * pow(bpp, beta);
    double diffLambda = (log(averageLambda) - log(calLambda));
    double lnbpp = Clip3(-5.0, -0.1, log(bpp));
    int iteration = 5;

    do {
        diffLambda = Clip3(-2.3, 2.3, diffLambda);
     
        alpha *= exp(rc_ctrl->m_seq_alpha_ratio * diffLambda);
        beta += rc_ctrl->m_seq_beta_ratio * diffLambda * lnbpp;

        alpha = Clip3(g_RCAlphaMinValue, g_RCAlphaMaxValue, alpha);
        beta  = Clip3(g_RCBetaMinValue , g_RCBetaMaxValue , beta );

        calLambda = alpha * pow(bpp, beta);
        diffLambda = (log(averageLambda) - log(calLambda));
    } while (iteration-- && (diffLambda > 0.02 || diffLambda < -0.02));

    rc_ctrl->m_seq_pic_params[frameLevel].m_alpha = alpha;
    rc_ctrl->m_seq_pic_params[frameLevel].m_beta  = beta;
}

static double rlambda_getRefineBitsForIntra(rc_rlambda_ctrl *rc_ctrl, double cpp)
{
    double alpha;
    double ratio;
    double orgBits = rc_ctrl->m_seq_avg_bits;
    double bpp = orgBits * 1.0 / rc_ctrl->m_seq_pic_pixels;

    if (bpp < 0.025) {
        alpha = 0.25;
    } else { 
        alpha = 0.3;
    }

    ratio = 0.85 * alpha* pow(pow(cpp, 1.0 / COST_BETA_I) * 4.0 / bpp, 0.5582);

    return ratio * orgBits;
}

static void rlambda_get_subgop_target_bits(rc_rlambda_ctrl *rc_ctrl, int numPic, double* cpp_list, int max_cpp_idx)
{
    double currentTargetBitsPerPic = (rc_ctrl->m_seq_smooth_window * rc_ctrl->m_seq_avg_bits - rc_ctrl->m_seq_bits_error) / rc_ctrl->m_seq_smooth_window;
    double targetBits = min(rc_ctrl->m_seq_avg_bits * 14, max(200, (currentTargetBitsPerPic * 8)));
    int i;
    double totalPicRatio = 0;
    double currPicRatio = 0;
    double targetBpp = targetBits / rc_ctrl->m_seq_pic_pixels;
    double basicLambda = 0.0;
    double lambdaRatio[8];
    double equaCoeffA[8];
    double equaCoeffB[8];
    int    bitsRatio[8];
    double last_lambda = Clip3(7.2, 800.0, rc_ctrl->m_seq_last_lambda);
    double p_frm_cpp = max(MIN_INTER_CPP, 0.05 * pow(cpp_list[0], 1.5));
    double cpp_ratio = Clip3(0.7, 1.0, -0.6667 * p_frm_cpp + 1.4667);

    rc_ctrl->m_sub_gop_bits_left = targetBits;

    lambdaRatio[0] = 1.0;

    lambdaRatio[1] = 0.71 * log(last_lambda) + 0.29; // last_qp / 8
    lambdaRatio[1] *= cpp_ratio;

    lambdaRatio[2] = 1.1892 * lambdaRatio[1]; // = power(2,0.25) * lambdaRatio[1]
    lambdaRatio[3] = 1.4142 * lambdaRatio[2]; // = power(2,0.75) * lambdaRatio[1]
    lambdaRatio[4] = 1.4142 * lambdaRatio[2]; // = power(2,0.75) * lambdaRatio[1]
    lambdaRatio[5] = 1.1892 * lambdaRatio[1]; // = power(2,0.25) * lambdaRatio[1]
    lambdaRatio[6] = 1.4142 * lambdaRatio[2]; // = power(2,0.75) * lambdaRatio[1]
    lambdaRatio[7] = 1.4142 * lambdaRatio[2]; // = power(2,0.75) * lambdaRatio[1]

    for (i = 0; i < numPic; i++) {
        int frameLevel = rc_ctrl->m_seq_gopid_2_level[i];
        double alpha   = rc_ctrl->m_seq_pic_params[frameLevel].m_alpha;
        double beta    = rc_ctrl->m_seq_pic_params[frameLevel].m_beta;
        double cpp     = max(MIN_INTER_CPP, 0.05 * pow(cpp_list[min(max_cpp_idx, i)], 1.5));

        equaCoeffA[i] = pow(lambdaRatio[i] / alpha, 1.0 / beta) * cpp;
        equaCoeffB[i] = 1.0 / beta;
    }

    basicLambda = rlambda_solve_equa(rc_ctrl, targetBpp, equaCoeffA, equaCoeffB, numPic);

    for (i = 0; i < numPic; i++) {
        bitsRatio[i] = (int)(equaCoeffA[i] * pow(basicLambda, equaCoeffB[i]) * rc_ctrl->m_seq_pic_pixels);
    }

    for (i = 0; i < numPic; i++) {
        totalPicRatio += bitsRatio[i];
    }
    for (i = 0; i < numPic; i++) {
        currPicRatio = bitsRatio[i];
        rc_ctrl->m_sub_gop_pic_bits[i] = (targetBits) * currPicRatio / totalPicRatio;
    }

    rc_ctrl->m_sub_gop_pic = rc_ctrl->m_sub_gop_pic_left = numPic;
}

void* rc_cbr_init(cfg_param_t *input)
{
    double target_bpp;
    int i;
    rc_rlambda_ctrl* rc_ctrl = com_malloc(sizeof(rc_rlambda_ctrl), 0);

    memset(rc_ctrl, 0, sizeof(rc_rlambda_ctrl));

    rc_ctrl->enc_cfg = input;
    rc_ctrl->m_seq_tar_bit_rate = input->target_bitrate * 1000.0;
    rc_ctrl->m_seq_frm_rate = input->frame_rate;

    rc_ctrl->m_seq_pic_pixels = input->img_width * input->img_height;
    rc_ctrl->m_seq_avg_bits = (rc_ctrl->m_seq_tar_bit_rate / rc_ctrl->m_seq_frm_rate);

    rc_ctrl->m_gop_coded_bits = 0;
    rc_ctrl->m_gop_coded_frms = 0;

    target_bpp = (double)rc_ctrl->m_seq_tar_bit_rate / (double)rc_ctrl->m_seq_frm_rate / (double)rc_ctrl->m_seq_pic_pixels;

    if (target_bpp < 0.03) {
        rc_ctrl->m_seq_alpha_ratio = 0.01;
        rc_ctrl->m_seq_beta_ratio = 0.005;
    }
    else if (target_bpp < 0.08) {
        rc_ctrl->m_seq_alpha_ratio = 0.05;
        rc_ctrl->m_seq_beta_ratio = 0.025;
    }
    else if (target_bpp < 0.2) {
        rc_ctrl->m_seq_alpha_ratio = 0.1;
        rc_ctrl->m_seq_beta_ratio = 0.05;
    }
    else if (target_bpp < 0.5) {
        rc_ctrl->m_seq_alpha_ratio = 0.2;
        rc_ctrl->m_seq_beta_ratio = 0.1;
    }
    else {
        rc_ctrl->m_seq_alpha_ratio = 0.4;
        rc_ctrl->m_seq_beta_ratio = 0.2;
    }

    rc_ctrl->m_seq_alpha_ratio /= 5;
    rc_ctrl->m_seq_beta_ratio /= 5;

    rc_ctrl->m_seq_gopid_2_level[0] = 1;
    rc_ctrl->m_seq_gopid_2_level[1] = 2;
    rc_ctrl->m_seq_gopid_2_level[2] = 3;
    rc_ctrl->m_seq_gopid_2_level[3] = 4;
    rc_ctrl->m_seq_gopid_2_level[4] = 4;
    rc_ctrl->m_seq_gopid_2_level[5] = 3;
    rc_ctrl->m_seq_gopid_2_level[6] = 4;
    rc_ctrl->m_seq_gopid_2_level[7] = 4;

    rc_ctrl->m_seq_pic_params[0].m_alpha = 6.7542 / 256;
    rc_ctrl->m_seq_pic_params[0].m_beta = -1.7860;

    for (i = 1; i < 5; i++) {
        rc_ctrl->m_seq_pic_params[i].m_alpha = 3.2003;
        rc_ctrl->m_seq_pic_params[i].m_beta = -1.367;
    }

    rc_ctrl->m_seq_smooth_window = input->gop_size * input->intra_period;
    rc_ctrl->m_seq_bits_error = 0;
    rc_ctrl->m_seq_last_lambda = 50.0;
    rc_ctrl->m_seq_coded_frm_idx = 0;
    rc_ctrl->m_seq_coded_frm_list_size = 0;

    return rc_ctrl;
}

void rc_cbr_destroy(void* rc_handle)
{
    com_free(rc_handle);
}

void rc_cbr_get_qp(void *rc_handle, avs3_rc_get_qp_param_t* param)
{
    rc_rlambda_ctrl *pRC = (rc_rlambda_ctrl *)rc_handle;
    cfg_param_t *cfg = pRC->enc_cfg;
    int i, picQP;
    double picLambda;
    int frameLevel = param->info.frameLevel;
    double targetBits;
    double adj_rate;
    int coded_list_idx;
    double cpp;
    double inter_cpp = cpp = max(MIN_INTER_CPP, 0.05 * pow(param->info.inter_cpp, 1.5));

    if (frameLevel < 2) {
        if (param->info.scene_cut) {
            memcpy(pRC->m_seq_pic_params_old, pRC->m_seq_pic_params, sizeof(pRC->m_seq_pic_params));
            pRC->m_seq_pic_params[0].m_alpha = 6.7542 / 256;
            pRC->m_seq_pic_params[0].m_beta = -1.7860;

            for (i = 1; i < 5; i++) {
                pRC->m_seq_pic_params[i].m_alpha = 3.2003;
                pRC->m_seq_pic_params[i].m_beta = -1.367;
            }
            pRC->m_seq_coded_frm_list_size = 0;
            pRC->m_seq_last_lambda = 50.0;
            pRC->m_scene_cut_flag = 1;
        }
        if (frameLevel == 0) {
            pRC->m_seq_smooth_window = cfg->gop_size * cfg->intra_period;
            pRC->m_seq_bits_error = 0;
        }
        rlambda_get_subgop_target_bits(pRC, frameLevel ? param->bfrms_in_subgop + 1 : 8, param->sub_gop_cpp_list, param->bfrms_in_subgop);
    }

    if (frameLevel == 0) {
        double alpha = pRC->m_seq_pic_params[1].m_alpha;
        double beta  = pRC->m_seq_pic_params[1].m_beta;
        double bpp   = pRC->m_sub_gop_pic_bits[0] / pRC->m_seq_pic_pixels / inter_cpp;
        double estLambda = alpha * pow(bpp, beta);
        
        alpha = pRC->m_seq_pic_params[0].m_alpha;
        beta  = pRC->m_seq_pic_params[0].m_beta;
        cpp = max(1.5, pow(param->info.pic_cpp, COST_BETA_I));

        targetBits = pow(0.75 * estLambda / alpha, 1 / beta) * cpp * pRC->m_seq_pic_pixels;
        //targetBits = rlambda_getRefineBitsForIntra(pRC, pRC->m_seq_pic_params[0].m_cpp);
        targetBits = min(pRC->m_seq_avg_bits * 8, targetBits);
    } else {
        double target_left = 0;
        for (i = 1; i <= pRC->m_sub_gop_pic_left; i++) {
            target_left += pRC->m_sub_gop_pic_bits[pRC->m_sub_gop_pic - i];
        }
        targetBits = pRC->m_sub_gop_pic_bits[pRC->m_sub_gop_pic - pRC->m_sub_gop_pic_left];
        adj_rate = pRC->m_sub_gop_bits_left / target_left;
        adj_rate = min(5, max(0.2, adj_rate));
        targetBits *= adj_rate;

        if (frameLevel == 1) {
            targetBits = min(pRC->m_seq_avg_bits * 4, targetBits);
        }
    }

    pRC->m_sub_gop_pic_left--;

    if (targetBits < 200) {
        targetBits = 200;   // at least allocate 100 bits for picture data
    }

    picLambda = rlambda_estimatePicLambda(pRC, targetBits, cpp, max(1.5, pow(param->info.intra_cpp, COST_BETA_I)), frameLevel, param->info.scene_cut, &coded_list_idx, param->ref_lambda);

    if (pRC->m_seq_coded_frm_list_size == 1 && frameLevel == 0) { // first I frame
        pRC->m_seq_last_lambda = picLambda * 1.49;
    } else if (frameLevel == 1) {
        pRC->m_seq_last_lambda = 0.5 * pRC->m_seq_last_lambda + 0.5 * picLambda;
    }

    picQP = (int)(5.661 * log(picLambda) + 13.131 + 0.5); 

    picQP = Clip3(0, MAX_QP, picQP);

    pRC->m_seq_bits_error += (int)(targetBits - pRC->m_seq_avg_bits);
    pRC->m_sub_gop_bits_left -= targetBits;

    param->info.target_bits = targetBits;
    param->info.qp = picQP;
    param->info.lambda = picLambda;
    param->info.pic_cpp = cpp;
    param->info.coded_list_idx = coded_list_idx;

    pRC->m_seq_smooth_window--;
}

void rc_cbr_update(void *rc_handle, avs3_rc_update_param_t *param, char *ext_info, int info_len)
{
    rc_rlambda_ctrl *pRC = (rc_rlambda_ctrl *)rc_handle;
    cfg_param_t *cfg = pRC->enc_cfg;
    double avg_rate_from_last_I = 0;
    double cpb = 0;
    int type = param->info.type;
    int header_bits = param->header_bits;
    int data_bits = param->data_bits;

    if (type & I_FRM) {
        if (pRC->m_gop_coded_frms) {
            avg_rate_from_last_I = pRC->m_gop_coded_bits * 1.0 / pRC->m_gop_coded_frms * cfg->frame_rate / 1000;
        }
        pRC->m_gop_coded_frms = 0;
        pRC->m_gop_coded_bits = 0;
    }

    pRC->m_gop_coded_frms++;
    pRC->m_gop_coded_bits     += header_bits + data_bits;
    pRC->m_seq_bits_error     += header_bits + data_bits - param->info.target_bits;
    pRC->m_sub_gop_bits_left  -= header_bits + data_bits - param->info.target_bits;

    if ((!param->info.scene_cut && !pRC->m_scene_cut_flag) || type != B_FRM) {
        if (param->info.frameLevel) {
            rlambda_updateAfterPicture(pRC, header_bits, header_bits + data_bits, param->info.qp, param->info.lambda, param->info.frameLevel, param->info.pic_cpp);
        }
        if (type != B_FRM) {
            pRC->m_scene_cut_flag = 0;
        }
    }

    if (param->info.coded_list_idx >= 0) {
        pRC->m_seq_coded_frm_list[param->info.coded_list_idx].lambda = param->info.lambda;
    }

    sprintf(ext_info, "(¦Á: %6.3f ¦Â: %6.3f ¦Ë: %8.2f Cpp: %6.2f tBpp: %6.4f ) aBpp: %6.4f Err: %6.2f %% Coef: %4.1f%%",
        pRC->m_seq_pic_params[param->info.frameLevel].m_alpha,
        pRC->m_seq_pic_params[param->info.frameLevel].m_beta,
        param->info.lambda,
        param->info.pic_cpp,
        param->info.target_bits * 1.0 / cfg->img_width / cfg->img_height, 
        (header_bits + data_bits) * 1.0 / cfg->img_width / cfg->img_height, 
        (header_bits + data_bits) * 100.0 / param->info.target_bits - 100,
        param->coef_bits * 100.0 / (header_bits + data_bits)
        );

    if (type & I_FRM) {
        sprintf(ext_info, "%s  rate: %.2f ( %.2f ) total-err: %f( %.2f s)", ext_info, avg_rate_from_last_I, avg_rate_from_last_I / cfg->target_bitrate, pRC->m_seq_bits_error, pRC->m_seq_bits_error / 1000.0 / cfg->target_bitrate);
    }
}
