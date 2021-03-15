#ifndef _HEADER_H_
#define _HEADER_H_
#include "global.h"
#include "commonVariables.h"

int WriteSequenceHeader(bit_stream_t *strm, cfg_param_t *input);
int WriteSequenceDisplayExtension(bit_stream_t *strm, const cfg_param_t *input);
int WriteHdrExtension(bit_stream_t *strm, const hdr_ext_data_t *hdr);
int WriteSequenceUserData(bit_stream_t *strm);
void write_slice_header(avs3_enc_t *h, bit_stream_t *strm, int patch_idx, int slice_qp);
void write_pic_hdr_i(avs3_enc_t *h);
void write_pic_hdr_pb(avs3_enc_t *h);
int write_md5_usr_data(avs3_enc_t *h, image_t *img);

void finish_picture_header(bit_stream_t *strm);
void flush_frames(avs3_ctrl_t *ctrl);
void sao_lcu_statistics(avs3_enc_t *h, int lcu_x, int lcu_y, SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES]);
void sao_lcu_rd_decision(avs3_enc_t *h, int lcu_x, int lcu_y, aec_t* aec, SAOStatData saostatData[NUM_SAO_COMPONENTS][NUM_SAO_NEW_TYPES], SAOBlkParam *saoBlkParam, double sao_labmda);


#endif
