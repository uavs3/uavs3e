#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include "../defines.h"
#include "../uAVS3lib.h"
#include "../me.h"

double pre_cal_I_cost(const cfg_param_t *input, pel_t *p_org, int i_org, int width, int height, int bit_depth, double* cpp_list);
double pre_cal_PB_cost(const cfg_param_t *input, pre_cal_PB_param_t* param_list, image_t *ref_img, image_t *org_img, double lambda, me_info_t *me, double* cpp_list, double *intra_cost, avs3_threadpool_t* pool);
double pre_cal_P_SAD(const cfg_param_t *input, image_t *ref_img, image_t *org_img);

#endif // #ifndef _PREPROCESS_H_