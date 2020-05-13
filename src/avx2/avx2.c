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
 *    This product includes software developed by the <organization>.
 * 4. Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include "avx2.h"

#if (BIT_DEPTH == 8)
void uavs3e_funs_init_avx2()
{
    int i;

    uavs3e_funs_handle.ip_flt_y_hor = uavs3e_if_hor_luma_frame_avx2;
    uavs3e_funs_handle.ip_flt_y_ver = uavs3e_if_ver_luma_frame_avx2;
    uavs3e_funs_handle.ip_flt_y_ver_ext = uavs3e_if_ver_luma_frame_ext_avx2;

    uavs3e_funs_handle.ipcpy[3] = uavs3e_if_cpy_w32_avx2;
    uavs3e_funs_handle.ipcpy[4] = uavs3e_if_cpy_w64_avx2;
    uavs3e_funs_handle.ipcpy[5] = uavs3e_if_cpy_w128_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_H_4][0] = uavs3e_if_hor_chroma_w4_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][1] = uavs3e_if_hor_chroma_w8_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][2] = uavs3e_if_hor_chroma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][3] = uavs3e_if_hor_chroma_w32_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][4] = uavs3e_if_hor_chroma_w32x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][5] = uavs3e_if_hor_chroma_w32x_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_H_8][0] = uavs3e_if_hor_luma_w4_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][1] = uavs3e_if_hor_luma_w8_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][2] = uavs3e_if_hor_luma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][3] = uavs3e_if_hor_luma_w32_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][4] = uavs3e_if_hor_luma_w32x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][5] = uavs3e_if_hor_luma_w32x_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_V_4][1] = uavs3e_if_ver_chroma_w8_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][2] = uavs3e_if_ver_chroma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][3] = uavs3e_if_ver_chroma_w32_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][4] = uavs3e_if_ver_chroma_w64_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][5] = uavs3e_if_ver_chroma_w128_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_V_8][0] = uavs3e_if_ver_luma_w4_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][1] = uavs3e_if_ver_luma_w8_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][2] = uavs3e_if_ver_luma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][3] = uavs3e_if_ver_luma_w32_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][4] = uavs3e_if_ver_luma_w64_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][5] = uavs3e_if_ver_luma_w128_avx2;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][0] = uavs3e_if_hor_ver_chroma_w4_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][1] = uavs3e_if_hor_ver_chroma_w8_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][2] = uavs3e_if_hor_ver_chroma_w16_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][3] = uavs3e_if_hor_ver_chroma_w32x_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][4] = uavs3e_if_hor_ver_chroma_w32x_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][5] = uavs3e_if_hor_ver_chroma_w32x_avx2;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][0] = uavs3e_if_hor_ver_luma_w4_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][1] = uavs3e_if_hor_ver_luma_w8_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][2] = uavs3e_if_hor_ver_luma_w16_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][3] = uavs3e_if_hor_ver_luma_w32_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][4] = uavs3e_if_hor_ver_luma_w32x_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][5] = uavs3e_if_hor_ver_luma_w32x_avx2;

    //uavs3e_funs_handle.padding_rows_luma = uavs3e_padding_rows_luma_avx2;
    //uavs3e_funs_handle.padding_rows_chroma = uavs3e_padding_rows_chroma_avx2;
    //uavs3e_funs_handle.conv_fmt_8bit = uavs3e_conv_fmt_8bit_avx2;
    //uavs3e_funs_handle.conv_fmt_16bit = uavs3e_conv_fmt_16bit_avx2;
    //uavs3e_funs_handle.conv_fmt_16to8bit = uavs3e_conv_fmt_16to8bit_avx2;
   
    uavs3e_funs_handle.recon[2] = uavs3e_recon_w16_avx2;
    uavs3e_funs_handle.recon[3] = uavs3e_recon_w32_avx2;
    uavs3e_funs_handle.recon[4] = uavs3e_recon_w64_avx2;

    uavs3e_funs_handle.dquant[1] = uavs3e_dquant_avx2;

    uavs3e_funs_handle.itrans_dct2[1][2] = uavs3e_itrans_dct2_h4_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[1][3] = uavs3e_itrans_dct2_h4_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[1][4] = uavs3e_itrans_dct2_h4_w32_avx2;
    
    uavs3e_funs_handle.itrans_dct2[2][1] = uavs3e_itrans_dct2_h8_w4_avx2;
    uavs3e_funs_handle.itrans_dct2[2][2] = uavs3e_itrans_dct2_h8_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[2][3] = uavs3e_itrans_dct2_h8_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[2][4] = uavs3e_itrans_dct2_h8_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[2][5] = uavs3e_itrans_dct2_h8_w64_avx2;
    
    uavs3e_funs_handle.itrans_dct2[3][1] = uavs3e_itrans_dct2_h16_w4_avx2;
    uavs3e_funs_handle.itrans_dct2[3][2] = uavs3e_itrans_dct2_h16_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[3][3] = uavs3e_itrans_dct2_h16_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[3][4] = uavs3e_itrans_dct2_h16_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[3][5] = uavs3e_itrans_dct2_h16_w64_avx2;
    
    uavs3e_funs_handle.itrans_dct2[4][1] = uavs3e_itrans_dct2_h32_w4_avx2;
    uavs3e_funs_handle.itrans_dct2[4][2] = uavs3e_itrans_dct2_h32_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[4][3] = uavs3e_itrans_dct2_h32_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[4][4] = uavs3e_itrans_dct2_h32_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[4][5] = uavs3e_itrans_dct2_h32_w64_avx2;
    
    uavs3e_funs_handle.itrans_dct2[5][2] = uavs3e_itrans_dct2_h64_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[5][3] = uavs3e_itrans_dct2_h64_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[5][4] = uavs3e_itrans_dct2_h64_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[5][5] = uavs3e_itrans_dct2_h64_w64_avx2;

    uavs3e_funs_handle.itrans_dct8_dst7[0][1] = uavs3e_itrans_dct8_pb4_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[0][2] = uavs3e_itrans_dct8_pb8_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[0][3] = uavs3e_itrans_dct8_pb16_avx2;

    uavs3e_funs_handle.itrans_dct8_dst7[1][1] = uavs3e_itrans_dst7_pb4_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[1][2] = uavs3e_itrans_dst7_pb8_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[1][3] = uavs3e_itrans_dst7_pb16_avx2;

    uavs3e_funs_handle.trans_dct2[1][1] = uavs3e_trans_dct2_w4_h4_avx2;
    uavs3e_funs_handle.trans_dct2[1][2] = uavs3e_trans_dct2_w4_h8_avx2;
    uavs3e_funs_handle.trans_dct2[1][3] = uavs3e_trans_dct2_w4_h16_avx2;
    uavs3e_funs_handle.trans_dct2[1][4] = uavs3e_trans_dct2_w4_h32_avx2;

    uavs3e_funs_handle.trans_dct2[2][1] = uavs3e_trans_dct2_w8_h4_avx2;
    uavs3e_funs_handle.trans_dct2[2][2] = uavs3e_trans_dct2_w8_h8_avx2;
    uavs3e_funs_handle.trans_dct2[2][3] = uavs3e_trans_dct2_w8_h16_avx2;
    uavs3e_funs_handle.trans_dct2[2][4] = uavs3e_trans_dct2_w8_h32_avx2;
    uavs3e_funs_handle.trans_dct2[2][5] = uavs3e_trans_dct2_w8_h64_avx2;

    uavs3e_funs_handle.trans_dct2[3][1] = uavs3e_trans_dct2_w16_h4_avx2;
    uavs3e_funs_handle.trans_dct2[3][2] = uavs3e_trans_dct2_w16_h8_avx2;
    uavs3e_funs_handle.trans_dct2[3][3] = uavs3e_trans_dct2_w16_h16_avx2;
    uavs3e_funs_handle.trans_dct2[3][4] = uavs3e_trans_dct2_w16_h32_avx2;
    uavs3e_funs_handle.trans_dct2[3][5] = uavs3e_trans_dct2_w16_h64_avx2;

    uavs3e_funs_handle.trans_dct2[4][1] = uavs3e_trans_dct2_w32_h4_avx2;
    uavs3e_funs_handle.trans_dct2[4][2] = uavs3e_trans_dct2_w32_h8_avx2;
    uavs3e_funs_handle.trans_dct2[4][3] = uavs3e_trans_dct2_w32_h16_avx2;
    uavs3e_funs_handle.trans_dct2[4][4] = uavs3e_trans_dct2_w32_h32_avx2;
    uavs3e_funs_handle.trans_dct2[4][5] = uavs3e_trans_dct2_w32_h64_avx2;

    uavs3e_funs_handle.trans_dct2[5][2] = uavs3e_trans_dct2_w64_h8_avx2;
    uavs3e_funs_handle.trans_dct2[5][3] = uavs3e_trans_dct2_w64_h16_avx2;
    uavs3e_funs_handle.trans_dct2[5][4] = uavs3e_trans_dct2_w64_h32_avx2;
    uavs3e_funs_handle.trans_dct2[5][5] = uavs3e_trans_dct2_w64_h64_avx2;

    uavs3e_funs_handle.trans_dct8_dst7[0][1] = trans_dct8_pb4_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[0][2] = trans_dct8_pb8_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[0][3] = trans_dct8_pb16_avx2;

    uavs3e_funs_handle.trans_dct8_dst7[1][1] = trans_dst7_pb4_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[1][2] = trans_dst7_pb8_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[1][3] = trans_dst7_pb16_avx2;

    uavs3e_funs_handle.sao = uavs3e_sao_on_lcu_avx2;
    uavs3e_funs_handle.alf = uavs3e_alf_one_lcu_avx2;
    uavs3e_funs_handle.alf_calc = uavs3e_alf_calc_corr_avx2;

    uavs3e_funs_handle.cost_sad[2] = uavs3e_get_sad_16_avx2;
    uavs3e_funs_handle.cost_sad[3] = uavs3e_get_sad_32_avx2;
    uavs3e_funs_handle.cost_sad[4] = uavs3e_get_sad_64_avx2;
    uavs3e_funs_handle.cost_sad[5] = uavs3e_get_sad_128_avx2;

    uavs3e_funs_handle.cost_sad_x3[2] = uavs3e_get_sad_x3_16_avx2;
    uavs3e_funs_handle.cost_sad_x3[3] = uavs3e_get_sad_x3_32_avx2;
    uavs3e_funs_handle.cost_sad_x3[4] = uavs3e_get_sad_x3_64_avx2;
    uavs3e_funs_handle.cost_sad_x3[5] = uavs3e_get_sad_x3_128_avx2;

    uavs3e_funs_handle.cost_sad_x4[0] = uavs3e_get_sad_x4_4_avx2;
    uavs3e_funs_handle.cost_sad_x4[1] = uavs3e_get_sad_x4_8_avx2;
    uavs3e_funs_handle.cost_sad_x4[2] = uavs3e_get_sad_x4_16_avx2;
    uavs3e_funs_handle.cost_sad_x4[3] = uavs3e_get_sad_x4_32_avx2;
    uavs3e_funs_handle.cost_sad_x4[4] = uavs3e_get_sad_x4_64_avx2;
    uavs3e_funs_handle.cost_sad_x4[5] = uavs3e_get_sad_x4_128_avx2;

    uavs3e_funs_handle.cost_ssd[0] = uavs3e_get_ssd_4_avx2;
    uavs3e_funs_handle.cost_ssd[1] = uavs3e_get_ssd_8_avx2;
    uavs3e_funs_handle.cost_ssd[2] = uavs3e_get_ssd_16_avx2;
    uavs3e_funs_handle.cost_ssd[3] = uavs3e_get_ssd_32_avx2;
    uavs3e_funs_handle.cost_ssd[4] = uavs3e_get_ssd_64_avx2;
    uavs3e_funs_handle.cost_ssd[5] = uavs3e_get_ssd_128_avx2;

    uavs3e_funs_handle.pel_diff[2] = uavs3e_pel_diff_16_avx2;
    uavs3e_funs_handle.pel_diff[3] = uavs3e_pel_diff_32_avx2;
    uavs3e_funs_handle.pel_diff[4] = uavs3e_pel_diff_64_avx2;
    uavs3e_funs_handle.pel_diff[5] = uavs3e_pel_diff_128_avx2;

    uavs3e_funs_handle.pel_avrg[1] = uavs3e_pel_avrg_8_avx2;
    uavs3e_funs_handle.pel_avrg[2] = uavs3e_pel_avrg_16_avx2;
    uavs3e_funs_handle.pel_avrg[3] = uavs3e_pel_avrg_32_avx2;
    uavs3e_funs_handle.pel_avrg[4] = uavs3e_pel_avrg_64_avx2;
    uavs3e_funs_handle.pel_avrg[5] = uavs3e_pel_avrg_128_avx2;

    uavs3e_ipred_offsets_seteps_init();
    for (i = IPD_BI + 1; i < IPD_VER; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = uavs3e_ipred_ang_x_avx2;
    }
    for (i = IPD_HOR + 1; i < IPD_CNT - 2; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = uavs3e_ipred_ang_y_avx2;
    }
    uavs3e_funs_handle.intra_pred_ang[4] = uavs3e_ipred_ang_x_4_avx2;
    uavs3e_funs_handle.intra_pred_ang[6] = uavs3e_ipred_ang_x_6_avx2;
    uavs3e_funs_handle.intra_pred_ang[8] = uavs3e_ipred_ang_x_8_avx2;
    uavs3e_funs_handle.intra_pred_ang[10] = uavs3e_ipred_ang_x_10_avx2;
    uavs3e_funs_handle.intra_pred_ang[26] = uavs3e_ipred_ang_y_26_avx2;
    uavs3e_funs_handle.intra_pred_ang[28] = uavs3e_ipred_ang_y_28_avx2;
    uavs3e_funs_handle.intra_pred_ang[30] = uavs3e_ipred_ang_y_30_avx2;
    uavs3e_funs_handle.intra_pred_ang[32] = uavs3e_ipred_ang_y_32_avx2;
    uavs3e_funs_handle.intra_pred_ang[13] = uavs3e_ipred_ang_xy_13_avx2;
    uavs3e_funs_handle.intra_pred_ang[14] = uavs3e_ipred_ang_xy_14_avx2;
    uavs3e_funs_handle.intra_pred_ang[15] = uavs3e_ipred_ang_xy_15_avx2;
    uavs3e_funs_handle.intra_pred_ang[16] = uavs3e_ipred_ang_xy_16_avx2;
    uavs3e_funs_handle.intra_pred_ang[17] = uavs3e_ipred_ang_xy_17_avx2;
    uavs3e_funs_handle.intra_pred_ang[18] = uavs3e_ipred_ang_xy_18_avx2;
    uavs3e_funs_handle.intra_pred_ang[19] = uavs3e_ipred_ang_xy_19_avx2;
    uavs3e_funs_handle.intra_pred_ang[20] = uavs3e_ipred_ang_xy_20_avx2;
    uavs3e_funs_handle.intra_pred_ang[21] = uavs3e_ipred_ang_xy_21_avx2;
    uavs3e_funs_handle.intra_pred_ang[22] = uavs3e_ipred_ang_xy_22_avx2;
    uavs3e_funs_handle.intra_pred_ang[23] = uavs3e_ipred_ang_xy_23_avx2;

    uavs3e_funs_handle.intra_pred_dc = uavs3e_ipred_dc_avx2;
    uavs3e_funs_handle.intra_pred_ver = uavs3e_ipred_ver_avx2;
    uavs3e_funs_handle.intra_pred_hor = uavs3e_ipred_hor_avx2;

    uavs3e_funs_handle.quant_check = quant_check_avx2;
    uavs3e_funs_handle.quant_rdoq  = quant_rdoq_avx2;

    uavs3e_funs_handle.cost_satd[0][1] = uavs3e_had_4x8_avx2;
    uavs3e_funs_handle.cost_satd[1][0] = uavs3e_had_8x4_avx2;
    uavs3e_funs_handle.cost_satd[1][1] = uavs3e_had_8x8_avx2;
    uavs3e_funs_handle.cost_satd[2][1] = uavs3e_had_16x8_avx2;
    uavs3e_funs_handle.cost_satd[1][2] = uavs3e_had_8x16_avx2;

    uavs3e_funs_handle.affine_sobel_flt_hor = affine_sobel_flt_hor_avx2;
    uavs3e_funs_handle.affine_sobel_flt_ver = affine_sobel_flt_ver_avx2;
    uavs3e_funs_handle.affine_coef_computer = affine_coef_computer_avx2;
}

#elif (BIT_DEPTH == 10)
void uavs3e_funs_init_avx2()
{
    uavs3e_funs_handle.ip_flt_y_hor = uavs3e_if_hor_luma_frame_avx2;
    uavs3e_funs_handle.ip_flt_y_ver = uavs3e_if_ver_luma_frame_avx2;
    uavs3e_funs_handle.ip_flt_y_ver_ext = uavs3e_if_ver_luma_frame_ext_avx2;

    uavs3e_funs_handle.ipcpy[3] = uavs3e_if_cpy_w32_avx2;
    uavs3e_funs_handle.ipcpy[4] = uavs3e_if_cpy_w64_avx2;
    uavs3e_funs_handle.ipcpy[5] = uavs3e_if_cpy_w128_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_H_8][2] = uavs3e_if_hor_luma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][3] = uavs3e_if_hor_luma_w16x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][4] = uavs3e_if_hor_luma_w16x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][5] = uavs3e_if_hor_luma_w16x_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_H_4][0] = uavs3e_if_hor_chroma_w4_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][1] = uavs3e_if_hor_chroma_w8_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][2] = uavs3e_if_hor_chroma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][3] = uavs3e_if_hor_chroma_w16x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][4] = uavs3e_if_hor_chroma_w16x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][5] = uavs3e_if_hor_chroma_w16x_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_V_4][0] = uavs3e_if_ver_chroma_w4_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][1] = uavs3e_if_ver_chroma_w8_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][2] = uavs3e_if_ver_chroma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][3] = uavs3e_if_ver_chroma_w32_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][4] = uavs3e_if_ver_chroma_w64_avx2;

    uavs3e_funs_handle.ipflt[IPFILTER_V_8][2] = uavs3e_if_ver_luma_w16_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][3] = uavs3e_if_ver_luma_w16x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][4] = uavs3e_if_ver_luma_w16x_avx2;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][5] = uavs3e_if_ver_luma_w16x_avx2;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][2] = uavs3e_if_hor_ver_luma_w16x_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][3] = uavs3e_if_hor_ver_luma_w16x_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][4] = uavs3e_if_hor_ver_luma_w16x_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][5] = uavs3e_if_hor_ver_luma_w16x_avx2;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][0] = uavs3e_if_hor_ver_chroma_w4_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][1] = uavs3e_if_hor_ver_chroma_w8_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][2] = uavs3e_if_hor_ver_chroma_w16_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][3] = uavs3e_if_hor_ver_chroma_w16x_avx2;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][4] = uavs3e_if_hor_ver_chroma_w16x_avx2;

    uavs3e_funs_handle.recon[1] = uavs3e_recon_w8_avx2;
    uavs3e_funs_handle.recon[2] = uavs3e_recon_w16_avx2;
    uavs3e_funs_handle.recon[3] = uavs3e_recon_w32_avx2;
    uavs3e_funs_handle.recon[4] = uavs3e_recon_w64_avx2;

    uavs3e_funs_handle.dquant[1] = uavs3e_dquant_avx2;

    uavs3e_funs_handle.itrans_dct2[1][2] = uavs3e_itrans_dct2_h4_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[1][3] = uavs3e_itrans_dct2_h4_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[1][4] = uavs3e_itrans_dct2_h4_w32_avx2;

    uavs3e_funs_handle.itrans_dct2[2][1] = uavs3e_itrans_dct2_h8_w4_avx2;
    uavs3e_funs_handle.itrans_dct2[2][2] = uavs3e_itrans_dct2_h8_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[2][3] = uavs3e_itrans_dct2_h8_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[2][4] = uavs3e_itrans_dct2_h8_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[2][5] = uavs3e_itrans_dct2_h8_w64_avx2;

    uavs3e_funs_handle.itrans_dct2[3][1] = uavs3e_itrans_dct2_h16_w4_avx2;
    uavs3e_funs_handle.itrans_dct2[3][2] = uavs3e_itrans_dct2_h16_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[3][3] = uavs3e_itrans_dct2_h16_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[3][4] = uavs3e_itrans_dct2_h16_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[3][5] = uavs3e_itrans_dct2_h16_w64_avx2;

    uavs3e_funs_handle.itrans_dct2[4][1] = uavs3e_itrans_dct2_h32_w4_avx2;
    uavs3e_funs_handle.itrans_dct2[4][2] = uavs3e_itrans_dct2_h32_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[4][3] = uavs3e_itrans_dct2_h32_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[4][4] = uavs3e_itrans_dct2_h32_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[4][5] = uavs3e_itrans_dct2_h32_w64_avx2;

    uavs3e_funs_handle.itrans_dct2[5][2] = uavs3e_itrans_dct2_h64_w8_avx2;
    uavs3e_funs_handle.itrans_dct2[5][3] = uavs3e_itrans_dct2_h64_w16_avx2;
    uavs3e_funs_handle.itrans_dct2[5][4] = uavs3e_itrans_dct2_h64_w32_avx2;
    uavs3e_funs_handle.itrans_dct2[5][5] = uavs3e_itrans_dct2_h64_w64_avx2;

    uavs3e_funs_handle.itrans_dct8_dst7[0][1] = uavs3e_itrans_dct8_pb4_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[0][2] = uavs3e_itrans_dct8_pb8_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[0][3] = uavs3e_itrans_dct8_pb16_avx2;

    uavs3e_funs_handle.itrans_dct8_dst7[1][1] = uavs3e_itrans_dst7_pb4_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[1][2] = uavs3e_itrans_dst7_pb8_avx2;
    uavs3e_funs_handle.itrans_dct8_dst7[1][3] = uavs3e_itrans_dst7_pb16_avx2;

    uavs3e_funs_handle.trans_dct2[1][1] = uavs3e_trans_dct2_w4_h4_avx2;
    uavs3e_funs_handle.trans_dct2[1][2] = uavs3e_trans_dct2_w4_h8_avx2;
    uavs3e_funs_handle.trans_dct2[1][3] = uavs3e_trans_dct2_w4_h16_avx2;
    uavs3e_funs_handle.trans_dct2[1][4] = uavs3e_trans_dct2_w4_h32_avx2;

    uavs3e_funs_handle.trans_dct2[2][1] = uavs3e_trans_dct2_w8_h4_avx2;
    uavs3e_funs_handle.trans_dct2[2][2] = uavs3e_trans_dct2_w8_h8_avx2;
    uavs3e_funs_handle.trans_dct2[2][3] = uavs3e_trans_dct2_w8_h16_avx2;
    uavs3e_funs_handle.trans_dct2[2][4] = uavs3e_trans_dct2_w8_h32_avx2;
    uavs3e_funs_handle.trans_dct2[2][5] = uavs3e_trans_dct2_w8_h64_avx2;

    uavs3e_funs_handle.trans_dct2[3][1] = uavs3e_trans_dct2_w16_h4_avx2;
    uavs3e_funs_handle.trans_dct2[3][2] = uavs3e_trans_dct2_w16_h8_avx2;
    uavs3e_funs_handle.trans_dct2[3][3] = uavs3e_trans_dct2_w16_h16_avx2;
    uavs3e_funs_handle.trans_dct2[3][4] = uavs3e_trans_dct2_w16_h32_avx2;
    uavs3e_funs_handle.trans_dct2[3][5] = uavs3e_trans_dct2_w16_h64_avx2;

    uavs3e_funs_handle.trans_dct2[4][1] = uavs3e_trans_dct2_w32_h4_avx2;
    uavs3e_funs_handle.trans_dct2[4][2] = uavs3e_trans_dct2_w32_h8_avx2;
    uavs3e_funs_handle.trans_dct2[4][3] = uavs3e_trans_dct2_w32_h16_avx2;
    uavs3e_funs_handle.trans_dct2[4][4] = uavs3e_trans_dct2_w32_h32_avx2;
    uavs3e_funs_handle.trans_dct2[4][5] = uavs3e_trans_dct2_w32_h64_avx2;

    uavs3e_funs_handle.trans_dct2[5][2] = uavs3e_trans_dct2_w64_h8_avx2;
    uavs3e_funs_handle.trans_dct2[5][3] = uavs3e_trans_dct2_w64_h16_avx2;
    uavs3e_funs_handle.trans_dct2[5][4] = uavs3e_trans_dct2_w64_h32_avx2;
    uavs3e_funs_handle.trans_dct2[5][5] = uavs3e_trans_dct2_w64_h64_avx2;

    uavs3e_funs_handle.trans_dct8_dst7[0][1] = trans_dct8_pb4_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[0][2] = trans_dct8_pb8_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[0][3] = trans_dct8_pb16_avx2;

    uavs3e_funs_handle.trans_dct8_dst7[1][1] = trans_dst7_pb4_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[1][2] = trans_dst7_pb8_avx2;
    uavs3e_funs_handle.trans_dct8_dst7[1][3] = trans_dst7_pb16_avx2;

    uavs3e_funs_handle.sao = uavs3e_sao_on_lcu_avx2;
    uavs3e_funs_handle.alf = uavs3e_alf_one_lcu_avx2;
    uavs3e_funs_handle.alf_calc = uavs3e_alf_calc_corr_avx2;

    uavs3e_funs_handle.cost_sad[0] = uavs3e_get_sad_4_avx2;
    uavs3e_funs_handle.cost_sad[1] = uavs3e_get_sad_8_avx2;
    uavs3e_funs_handle.cost_sad[2] = uavs3e_get_sad_16_avx2;
    uavs3e_funs_handle.cost_sad[3] = uavs3e_get_sad_32_avx2;
    uavs3e_funs_handle.cost_sad[4] = uavs3e_get_sad_64_avx2;
    uavs3e_funs_handle.cost_sad[5] = uavs3e_get_sad_128_avx2;

    uavs3e_funs_handle.cost_sad_x3[0] = uavs3e_get_sad_x3_4_avx2;
    uavs3e_funs_handle.cost_sad_x3[1] = uavs3e_get_sad_x3_8_avx2;
    uavs3e_funs_handle.cost_sad_x3[2] = uavs3e_get_sad_x3_16_avx2;
    uavs3e_funs_handle.cost_sad_x3[3] = uavs3e_get_sad_x3_32_avx2;
    uavs3e_funs_handle.cost_sad_x3[4] = uavs3e_get_sad_x3_64_avx2;
    uavs3e_funs_handle.cost_sad_x3[5] = uavs3e_get_sad_x3_128_avx2;

    uavs3e_funs_handle.cost_sad_x4[0] = uavs3e_get_sad_x4_4_avx2;
    uavs3e_funs_handle.cost_sad_x4[1] = uavs3e_get_sad_x4_8_avx2;
    uavs3e_funs_handle.cost_sad_x4[2] = uavs3e_get_sad_x4_16_avx2;
    uavs3e_funs_handle.cost_sad_x4[3] = uavs3e_get_sad_x4_32_avx2;
    uavs3e_funs_handle.cost_sad_x4[4] = uavs3e_get_sad_x4_64_avx2;
    uavs3e_funs_handle.cost_sad_x4[5] = uavs3e_get_sad_x4_128_avx2;

    uavs3e_funs_handle.cost_ssd[0] = uavs3e_get_ssd_4_avx2;
    uavs3e_funs_handle.cost_ssd[1] = uavs3e_get_ssd_8_avx2;
    uavs3e_funs_handle.cost_ssd[2] = uavs3e_get_ssd_16_avx2;
    uavs3e_funs_handle.cost_ssd[3] = uavs3e_get_ssd_32_avx2;
    uavs3e_funs_handle.cost_ssd[4] = uavs3e_get_ssd_64_avx2;
    uavs3e_funs_handle.cost_ssd[5] = uavs3e_get_ssd_128_avx2;
    
    uavs3e_funs_handle.pel_diff[2] = uavs3e_pel_diff_16_avx2;
    uavs3e_funs_handle.pel_diff[3] = uavs3e_pel_diff_32_avx2;
    uavs3e_funs_handle.pel_diff[4] = uavs3e_pel_diff_64_avx2;
    uavs3e_funs_handle.pel_diff[5] = uavs3e_pel_diff_128_avx2;

    uavs3e_funs_handle.pel_avrg[0] = uavs3e_pel_avrg_4_avx2;
    uavs3e_funs_handle.pel_avrg[1] = uavs3e_pel_avrg_8_avx2;
    uavs3e_funs_handle.pel_avrg[2] = uavs3e_pel_avrg_16_avx2;
    uavs3e_funs_handle.pel_avrg[3] = uavs3e_pel_avrg_32_avx2;
    uavs3e_funs_handle.pel_avrg[4] = uavs3e_pel_avrg_64_avx2;
    uavs3e_funs_handle.pel_avrg[5] = uavs3e_pel_avrg_128_avx2;
    
    uavs3e_funs_handle.intra_pred_dc = uavs3e_ipred_dc_avx2;
    uavs3e_funs_handle.intra_pred_ver = uavs3e_ipred_ver_avx2;
    uavs3e_funs_handle.intra_pred_hor = uavs3e_ipred_hor_avx2;
    
    uavs3e_funs_handle.quant_check = quant_check_avx2;
    uavs3e_funs_handle.quant_rdoq = quant_rdoq_avx2;

    uavs3e_funs_handle.cost_satd[0][1] = uavs3e_had_4x8_avx2;
    uavs3e_funs_handle.cost_satd[1][0] = uavs3e_had_8x4_avx2;
    uavs3e_funs_handle.cost_satd[1][1] = uavs3e_had_8x8_avx2;
    uavs3e_funs_handle.cost_satd[2][1] = uavs3e_had_16x8_avx2;
    uavs3e_funs_handle.cost_satd[1][2] = uavs3e_had_8x16_avx2;

    uavs3e_funs_handle.affine_sobel_flt_hor = affine_sobel_flt_hor_avx2;
    uavs3e_funs_handle.affine_sobel_flt_ver = affine_sobel_flt_ver_avx2;
    uavs3e_funs_handle.affine_coef_computer = affine_coef_computer_avx2;
}

#endif