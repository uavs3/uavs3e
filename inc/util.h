/**************************************************************************************
 * Copyright (c) 2018-2020 ["Peking University Shenzhen Graduate School",
 *   "Peng Cheng Laboratory", and "Guangdong Bohua UHD Innovation Corporation"]
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes the software uAVS3d developed by
 *    Peking University Shenzhen Graduate School, Peng Cheng Laboratory
 *    and Guangdong Bohua UHD Innovation Corporation.
 * 4. Neither the name of the organizations (Peking University Shenzhen Graduate School,
 *    Peng Cheng Laboratory and Guangdong Bohua UHD Innovation Corporation) nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#ifndef _ENC_UTIL_H_
#define _ENC_UTIL_H_

#define GET_MVBITS_X(mv) (tab_mvbits_x[(mv) >> mvr_idx])
#define GET_MVBITS_Y(mv) (tab_mvbits_y[(mv) >> mvr_idx])
#define GET_MVBITS_IPEL_X(mv) (tab_mvbits_x[((mv) << 2) >> mvr_idx])
#define GET_MVBITS_IPEL_Y(mv) (tab_mvbits_y[((mv) << 2) >> mvr_idx])

u32 calc_satd_intra(int pu_w, int pu_h, pel *src1, pel *src2, int s_src1, int s_src2, int bit_depth);

static u64 avs3_always_inline block_pel_ssd(int log2w, int height, pel *src1, pel *src2, int s_src1, int s_src2, int bit_depth)
{
    int shift = (bit_depth - 8) << 1;
    return uavs3e_funs_handle.cost_ssd[log2w - 2](src1, s_src1, src2, s_src2, height) >> shift;
}

static void avs3_always_inline block_pel_sub(int log2w, int log2h, pel *src1, pel *src2, int s_src1, int s_src2, int s_diff, s16 *diff)
{
    uavs3e_funs_handle.pel_diff[log2w - 2](src1, s_src1, src2, s_src2, diff, s_diff, 1 << log2h);
}

void cu_pel_sub(u8 tree_status, int x, int y, int cu_width_log2, int cu_height_log2, com_pic_t *org, pel pred[N_C][MAX_CU_DIM], s16 diff[N_C][MAX_CU_DIM]);

int enc_create_cu_data(enc_cu_t *cu_data, int cu_width_log2, int cu_height_log2);
int enc_delete_cu_data(enc_cu_t *cu_data);

double enc_get_hgop_qp(double base_qp, int frm_depth, int is_ld);

static void avs3_always_inline add_input_node(enc_ctrl_t *h, com_img_t *img, int bref, int layer, int type, com_img_t* list0, com_img_t* list1)
{
    input_node_t *node;

    node           = &h->node_list[h->node_size++];
    node->img      = img; 
	if (h->cfg.use_ref_block_aq)
	{
		img->list[0] = list0;
		img->list[1] = list1;
		img->cucost_done = 0;

		int num = ((img->height[0] + UNIT_SIZE - 1) / UNIT_SIZE) *((img->width[0] + UNIT_SIZE - 1) / UNIT_SIZE);

		for (int i = 0; i < num; i++)
		{
			img->intra_satd[i] = 0.0;
			img->propagateCost[i] = 0.0;
		}
	}
    node->b_ref    = bref;
    node->layer_id = layer; 
    node->type     = type;
}

static void avs3_always_inline shift_reorder_list(enc_ctrl_t *h, int cur_ip_idx)
{
    h->img_rsize -= cur_ip_idx + 1;

    for (int i = 0; i < h->img_rsize; i++) {
        h->img_rlist[i] = h->img_rlist[cur_ip_idx + i + 1];
    }
    memset(h->img_rlist + h->img_rsize, 0, (cur_ip_idx + 1) * sizeof(analyze_node_t));
}

#endif /* _ENC_UTIL_H_ */