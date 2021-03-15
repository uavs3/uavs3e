#ifndef _RATECONTRAL_H_
#define _RATECONTRAL_H_

#include "../uAVS3lib.h"
#include <memory.h>

#define RC_TYPE_CBR 1
#define RC_TYPE_ABR 2
#define RC_TYPE_CRF 3

void* rc_cbr_init(cfg_param_t *input);
void  rc_cbr_get_qp(void *rc_handle, avs3_rc_get_qp_param_t *param);
void  rc_cbr_update(void *rc_handle, avs3_rc_update_param_t *param, char *ext_info, int info_len);
void  rc_cbr_destroy(void *rc_handle);
 
void* rc_abr_init(cfg_param_t *input);
void  rc_abr_get_qp(void *rc_handle, avs3_rc_get_qp_param_t *param);
void  rc_abr_update(void *rc_handle, avs3_rc_update_param_t *param, char *ext_info, int info_len);
void  rc_abr_destroy(void *rc_handle);

void* rc_crf_init(cfg_param_t *input);
void  rc_crf_get_qp(void *rc_handle, avs3_rc_get_qp_param_t *param);
void  rc_crf_update(void *rc_handle, avs3_rc_update_param_t *param, char *ext_info, int info_len);
void  rc_crf_destroy(void *rc_handle);

#endif // #ifndef _RATECONTRAL_H_