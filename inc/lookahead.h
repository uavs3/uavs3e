/**************************************************************************************
 * Copyright (C) 2018-2019 uavs3e project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Open-Intelligence Open Source License V1.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Open-Intelligence Open Source License V1.1 for more details.
 *
 * You should have received a copy of the Open-Intelligence Open Source License V1.1
 * along with this program; if not, you can download it on:
 * http://www.aitisa.org.cn/uploadfile/2018/0910/20180910031548314.pdf
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#ifndef _ENC_LOOKAHEAD_H_
#define _ENC_LOOKAHEAD_H_

double loka_estimate_coding_cost(inter_search_t *pi, com_img_t *img_org, com_img_t **ref_l0, com_img_t **ref_l1, int num_ref[2], int bit_depth, double *icost);
int    loka_check_scenecut(inter_search_t *pi, com_img_t *img_org, com_img_t *img_last, int bit_depth, int threshold);

#endif //_ENC_LOOKAHEAD_H_