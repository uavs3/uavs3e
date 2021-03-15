#ifndef _IMAGE_H_
#define _IMAGE_H_

void encode_one_frame(avs3_ctrl_t *ctrl, frame_t *frm, double* cpp_list, int cpps);
void* picture_data(void *param);
int frame_create(frame_t *frm, cfg_param_t *input);
void frame_destroy(frame_t *frm);

#endif


