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

    uavs3e_funs_handle.cost_sad[2] = uavs3e_get_sad_16_avx2;
    uavs3e_funs_handle.cost_sad[3] = uavs3e_get_sad_32_avx2;
    uavs3e_funs_handle.cost_sad[4] = uavs3e_get_sad_64_avx2;
    uavs3e_funs_handle.cost_sad[5] = uavs3e_get_sad_128_avx2;

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

    for (i = IPD_BI + 1; i < IPD_VER; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = uavs3e_ipred_ang_x_avx2;
    }
    uavs3e_funs_handle.intra_pred_ang[4] = uavs3e_ipred_ang_x_4_avx2;
    uavs3e_funs_handle.intra_pred_ang[8] = uavs3e_ipred_ang_x_8_avx2;

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

    uavs3e_funs_handle.cost_sad[0] = uavs3e_get_sad_4_avx2;
    uavs3e_funs_handle.cost_sad[1] = uavs3e_get_sad_8_avx2;
    uavs3e_funs_handle.cost_sad[2] = uavs3e_get_sad_16_avx2;
    uavs3e_funs_handle.cost_sad[3] = uavs3e_get_sad_32_avx2;
    uavs3e_funs_handle.cost_sad[4] = uavs3e_get_sad_64_avx2;
    uavs3e_funs_handle.cost_sad[5] = uavs3e_get_sad_128_avx2;

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