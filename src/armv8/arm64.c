#include "arm64.h"

#if defined(__arm64__)
void uavs3e_funs_init_arm64()
{
#if(BIT_DEPTH==8)
    
    uavs3e_funs_handle.intra_pred_dc        = uavs3e_intra_pred_dc_arm64;
    uavs3e_funs_handle.intra_pred_bi        = uavs3e_intra_pred_bi_arm64;
    uavs3e_funs_handle.intra_pred_plane     = uavs3e_intra_pred_plane_arm64;
    uavs3e_funs_handle.intra_pred_hor       = uavs3e_intra_pred_hor_arm64;
    uavs3e_funs_handle.intra_pred_ver       = uavs3e_intra_pred_ver_arm64;
    uavs3e_funs_handle.intra_pred_bi_ipf    = uavs3e_intra_pred_bi_ipf_arm64;
    uavs3e_funs_handle.intra_pred_plane_ipf = uavs3e_intra_pred_plane_ipf_arm64;
    
    uavs3e_funs_handle.ipcpy[0] = uavs3e_if_cpy_w4_arm64;
    uavs3e_funs_handle.ipcpy[1] = uavs3e_if_cpy_w8_arm64;
    uavs3e_funs_handle.ipcpy[2] = uavs3e_if_cpy_w16_arm64;
    uavs3e_funs_handle.ipcpy[3] = uavs3e_if_cpy_w32_arm64;
    uavs3e_funs_handle.ipcpy[4] = uavs3e_if_cpy_w64_arm64;
    uavs3e_funs_handle.ipcpy[5] = uavs3e_if_cpy_w128_arm64;

    uavs3e_funs_handle.ipflt[IPFILTER_H_8][0] = uavs3e_if_hor_luma_w4_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][1] = uavs3e_if_hor_luma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][2] = uavs3e_if_hor_luma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][3] = uavs3e_if_hor_luma_w32_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][4] = uavs3e_if_hor_luma_w32x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][5] = uavs3e_if_hor_luma_w32x_arm64;

    uavs3e_funs_handle.ipflt[IPFILTER_H_4][1] = uavs3e_if_hor_chroma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][2] = uavs3e_if_hor_chroma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][3] = uavs3e_if_hor_chroma_w32_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][4] = uavs3e_if_hor_chroma_w32x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][5] = uavs3e_if_hor_chroma_w32x_arm64;

    uavs3e_funs_handle.ipflt[IPFILTER_V_8][0] = uavs3e_if_ver_luma_w4_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][1] = uavs3e_if_ver_luma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][2] = uavs3e_if_ver_luma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][3] = uavs3e_if_ver_luma_w32_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][4] = uavs3e_if_ver_luma_w32x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][5] = uavs3e_if_ver_luma_w32x_arm64;

    uavs3e_funs_handle.ipflt[IPFILTER_V_4][1] = uavs3e_if_ver_chroma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][2] = uavs3e_if_ver_chroma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][3] = uavs3e_if_ver_chroma_w32_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][4] = uavs3e_if_ver_chroma_w64_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][5] = uavs3e_if_ver_chroma_w128_arm64;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][0] = uavs3e_if_hor_ver_luma_w4_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][1] = uavs3e_if_hor_ver_luma_w8_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][2] = uavs3e_if_hor_ver_luma_w16_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][3] = uavs3e_if_hor_ver_luma_w32_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][4] = uavs3e_if_hor_ver_luma_w32x_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][5] = uavs3e_if_hor_ver_luma_w32x_arm64;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][1] = uavs3e_if_hor_ver_chroma_w8_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][2] = uavs3e_if_hor_ver_chroma_w16_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][3] = uavs3e_if_hor_ver_chroma_w32_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][4] = uavs3e_if_hor_ver_chroma_w32x_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][5] = uavs3e_if_hor_ver_chroma_w32x_arm64;
    
    uavs3e_funs_handle.trans_dct2[1][1] = uavs3e_trans_dct2_w4_h4_arm64;
    uavs3e_funs_handle.trans_dct2[1][2] = uavs3e_trans_dct2_w4_h8_arm64;
    uavs3e_funs_handle.trans_dct2[1][3] = uavs3e_trans_dct2_w4_h16_arm64;
    uavs3e_funs_handle.trans_dct2[1][4] = uavs3e_trans_dct2_w4_h32_arm64;

    uavs3e_funs_handle.trans_dct2[2][1] = uavs3e_trans_dct2_w8_h4_arm64;
    uavs3e_funs_handle.trans_dct2[2][2] = uavs3e_trans_dct2_w8_h8_arm64;
    uavs3e_funs_handle.trans_dct2[2][3] = uavs3e_trans_dct2_w8_h16_arm64;
    uavs3e_funs_handle.trans_dct2[2][4] = uavs3e_trans_dct2_w8_h32_arm64;
    uavs3e_funs_handle.trans_dct2[2][5] = uavs3e_trans_dct2_w8_h64_arm64;

    uavs3e_funs_handle.trans_dct2[3][1] = uavs3e_trans_dct2_w16_h4_arm64;
    uavs3e_funs_handle.trans_dct2[3][2] = uavs3e_trans_dct2_w16_h8_arm64;
    uavs3e_funs_handle.trans_dct2[3][3] = uavs3e_trans_dct2_w16_h16_arm64;
    uavs3e_funs_handle.trans_dct2[3][4] = uavs3e_trans_dct2_w16_h32_arm64;
    uavs3e_funs_handle.trans_dct2[3][5] = uavs3e_trans_dct2_w16_h64_arm64;

    uavs3e_funs_handle.trans_dct2[4][1] = uavs3e_trans_dct2_w32_h4_arm64;
    uavs3e_funs_handle.trans_dct2[4][2] = uavs3e_trans_dct2_w32_h8_arm64;
    uavs3e_funs_handle.trans_dct2[4][3] = uavs3e_trans_dct2_w32_h16_arm64;
    uavs3e_funs_handle.trans_dct2[4][4] = uavs3e_trans_dct2_w32_h32_arm64;
    uavs3e_funs_handle.trans_dct2[4][5] = uavs3e_trans_dct2_w32_h64_arm64;
    
    uavs3e_funs_handle.trans_dct2[5][2] = uavs3e_trans_dct2_w64_h8_arm64;
    uavs3e_funs_handle.trans_dct2[5][3] = uavs3e_trans_dct2_w64_h16_arm64;
    uavs3e_funs_handle.trans_dct2[5][4] = uavs3e_trans_dct2_w64_h32_arm64;
    uavs3e_funs_handle.trans_dct2[5][5] = uavs3e_trans_dct2_w64_h64_arm64;

    uavs3e_funs_handle.itrans_dct2[1][1] = uavs3e_itrans_dct2_h4_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[1][2] = uavs3e_itrans_dct2_h4_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[1][3] = uavs3e_itrans_dct2_h4_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[1][4] = uavs3e_itrans_dct2_h4_w32_arm64;

    uavs3e_funs_handle.itrans_dct2[2][1] = uavs3e_itrans_dct2_h8_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[2][2] = uavs3e_itrans_dct2_h8_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[2][3] = uavs3e_itrans_dct2_h8_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[2][4] = uavs3e_itrans_dct2_h8_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[2][5] = uavs3e_itrans_dct2_h8_w64_arm64;

    uavs3e_funs_handle.itrans_dct2[3][1] = uavs3e_itrans_dct2_h16_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[3][2] = uavs3e_itrans_dct2_h16_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[3][3] = uavs3e_itrans_dct2_h16_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[3][4] = uavs3e_itrans_dct2_h16_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[3][5] = uavs3e_itrans_dct2_h16_w64_arm64;

    uavs3e_funs_handle.itrans_dct2[4][1] = uavs3e_itrans_dct2_h32_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[4][2] = uavs3e_itrans_dct2_h32_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[4][3] = uavs3e_itrans_dct2_h32_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[4][4] = uavs3e_itrans_dct2_h32_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[4][5] = uavs3e_itrans_dct2_h32_w64_arm64;

    uavs3e_funs_handle.itrans_dct2[5][2] = uavs3e_itrans_dct2_h64_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[5][3] = uavs3e_itrans_dct2_h64_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[5][4] = uavs3e_itrans_dct2_h64_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[5][5] = uavs3e_itrans_dct2_h64_w64_arm64;

    //uavs3e_funs_handle.itrans_dct8[1] = uavs3e_itrans_dct8_pb4_arm64;
    //uavs3e_funs_handle.itrans_dct8[2] = uavs3e_itrans_dct8_pb8_arm64;
    //uavs3e_funs_handle.itrans_dct8[3] = uavs3e_itrans_dct8_pb16_arm64;
    
    //uavs3e_funs_handle.itrans_dst7[1] = uavs3e_itrans_dst7_pb4_arm64;
    //uavs3e_funs_handle.itrans_dst7[2] = uavs3e_itrans_dst7_pb8_arm64;
    //uavs3e_funs_handle.itrans_dst7[3] = uavs3e_itrans_dst7_pb16_arm64;

    uavs3e_funs_handle.deblock_luma[0] = uavs3e_deblock_ver_luma_arm64;
    uavs3e_funs_handle.deblock_luma[1] = uavs3e_deblock_hor_luma_arm64;
    uavs3e_funs_handle.deblock_chroma[0] = uavs3e_deblock_ver_chroma_arm64;
    uavs3e_funs_handle.deblock_chroma[1] = uavs3e_deblock_hor_chroma_arm64;
    
    uavs3e_funs_handle.sao = uavs3e_sao_on_lcu_arm64;
    uavs3e_funs_handle.alf = uavs3e_alf_filter_block_arm64;

    uavs3e_funs_handle.pel_diff[0] = uavs3e_pel_diff_4_arm64;
    uavs3e_funs_handle.pel_diff[1] = uavs3e_pel_diff_8_arm64;
    uavs3e_funs_handle.pel_diff[2] = uavs3e_pel_diff_16_arm64;
    uavs3e_funs_handle.pel_diff[3] = uavs3e_pel_diff_32_arm64;
    uavs3e_funs_handle.pel_diff[4] = uavs3e_pel_diff_64_arm64;
    uavs3e_funs_handle.pel_diff[5] = uavs3e_pel_diff_128_arm64;
    
    uavs3e_funs_handle.cost_sad[0] = uavs3e_get_sad_4_arm64;
    uavs3e_funs_handle.cost_sad[1] = uavs3e_get_sad_8_arm64;
    uavs3e_funs_handle.cost_sad[2] = uavs3e_get_sad_16_arm64;
    uavs3e_funs_handle.cost_sad[3] = uavs3e_get_sad_32_arm64;
    uavs3e_funs_handle.cost_sad[4] = uavs3e_get_sad_64_arm64;
    uavs3e_funs_handle.cost_sad[5] = uavs3e_get_sad_128_arm64;
    
    uavs3e_funs_handle.cost_ssd[0] = uavs3e_get_ssd_4_arm64;
    uavs3e_funs_handle.cost_ssd[1] = uavs3e_get_ssd_8_arm64;
    uavs3e_funs_handle.cost_ssd[2] = uavs3e_get_ssd_16_arm64;
    uavs3e_funs_handle.cost_ssd[3] = uavs3e_get_ssd_32_arm64;
    uavs3e_funs_handle.cost_ssd[4] = uavs3e_get_ssd_64_arm64;
    uavs3e_funs_handle.cost_ssd[5] = uavs3e_get_ssd_128_arm64;

    uavs3e_funs_handle.cost_sad_x3[0] = uavs3e_get_sad_x3_4_arm64;
    uavs3e_funs_handle.cost_sad_x3[1] = uavs3e_get_sad_x3_8_arm64;
    uavs3e_funs_handle.cost_sad_x3[2] = uavs3e_get_sad_x3_16_arm64;
    uavs3e_funs_handle.cost_sad_x3[3] = uavs3e_get_sad_x3_32_arm64;
    uavs3e_funs_handle.cost_sad_x3[4] = uavs3e_get_sad_x3_64_arm64;
    uavs3e_funs_handle.cost_sad_x3[5] = uavs3e_get_sad_x3_128_arm64;
    
    uavs3e_funs_handle.cost_sad_x4[0] = uavs3e_get_sad_x4_4_arm64;
    uavs3e_funs_handle.cost_sad_x4[1] = uavs3e_get_sad_x4_8_arm64;
    uavs3e_funs_handle.cost_sad_x4[2] = uavs3e_get_sad_x4_16_arm64;
    uavs3e_funs_handle.cost_sad_x4[3] = uavs3e_get_sad_x4_32_arm64;
    uavs3e_funs_handle.cost_sad_x4[4] = uavs3e_get_sad_x4_64_arm64;
    uavs3e_funs_handle.cost_sad_x4[5] = uavs3e_get_sad_x4_128_arm64;
    
    uavs3e_funs_handle.cost_satd[0][0] = uavs3e_had_4x4_arm64;
    uavs3e_funs_handle.cost_satd[1][0] = uavs3e_had_8x4_arm64;
    uavs3e_funs_handle.cost_satd[0][1] = uavs3e_had_4x8_arm64;
    uavs3e_funs_handle.cost_satd[1][1] = uavs3e_had_8x8_arm64;
    //uavs3e_funs_handle.cost_satd[2][1] = uavs3e_had_16x8_arm64;   //error
    uavs3e_funs_handle.cost_satd[1][2] = uavs3e_had_8x16_arm64;
    
    uavs3e_funs_handle.pel_diff[0] = uavs3e_pel_diff_4_arm64;
    uavs3e_funs_handle.pel_diff[1] = uavs3e_pel_diff_8_arm64;
    uavs3e_funs_handle.pel_diff[2] = uavs3e_pel_diff_16_arm64;
    uavs3e_funs_handle.pel_diff[3] = uavs3e_pel_diff_32_arm64;
    uavs3e_funs_handle.pel_diff[4] = uavs3e_pel_diff_64_arm64;
    uavs3e_funs_handle.pel_diff[5] = uavs3e_pel_diff_128_arm64;
    
    uavs3e_funs_handle.recon[0] = uavs3e_recon_w4_arm64;
    uavs3e_funs_handle.recon[1] = uavs3e_recon_w8_arm64;
    uavs3e_funs_handle.recon[2] = uavs3e_recon_w16_arm64;
    uavs3e_funs_handle.recon[3] = uavs3e_recon_w32_arm64;

#else
    uavs3e_funs_handle.trans_dct2[1][1] = uavs3e_trans_dct2_w4_h4_arm64;
    uavs3e_funs_handle.trans_dct2[1][2] = uavs3e_trans_dct2_w4_h8_arm64;
    uavs3e_funs_handle.trans_dct2[1][3] = uavs3e_trans_dct2_w4_h16_arm64;
    uavs3e_funs_handle.trans_dct2[1][4] = uavs3e_trans_dct2_w4_h32_arm64;

    uavs3e_funs_handle.trans_dct2[2][1] = uavs3e_trans_dct2_w8_h4_arm64;
    uavs3e_funs_handle.trans_dct2[2][2] = uavs3e_trans_dct2_w8_h8_arm64;
    uavs3e_funs_handle.trans_dct2[2][3] = uavs3e_trans_dct2_w8_h16_arm64;
    uavs3e_funs_handle.trans_dct2[2][4] = uavs3e_trans_dct2_w8_h32_arm64;
    uavs3e_funs_handle.trans_dct2[2][5] = uavs3e_trans_dct2_w8_h64_arm64;

    uavs3e_funs_handle.trans_dct2[3][1] = uavs3e_trans_dct2_w16_h4_arm64;
    uavs3e_funs_handle.trans_dct2[3][2] = uavs3e_trans_dct2_w16_h8_arm64;
    uavs3e_funs_handle.trans_dct2[3][3] = uavs3e_trans_dct2_w16_h16_arm64;
    uavs3e_funs_handle.trans_dct2[3][4] = uavs3e_trans_dct2_w16_h32_arm64;
    uavs3e_funs_handle.trans_dct2[3][5] = uavs3e_trans_dct2_w16_h64_arm64;

    uavs3e_funs_handle.trans_dct2[4][1] = uavs3e_trans_dct2_w32_h4_arm64;
    uavs3e_funs_handle.trans_dct2[4][2] = uavs3e_trans_dct2_w32_h8_arm64;
    uavs3e_funs_handle.trans_dct2[4][3] = uavs3e_trans_dct2_w32_h16_arm64;
    uavs3e_funs_handle.trans_dct2[4][4] = uavs3e_trans_dct2_w32_h32_arm64;
    uavs3e_funs_handle.trans_dct2[4][5] = uavs3e_trans_dct2_w32_h64_arm64;

    uavs3e_funs_handle.trans_dct2[5][2] = uavs3e_trans_dct2_w64_h8_arm64;
    uavs3e_funs_handle.trans_dct2[5][3] = uavs3e_trans_dct2_w64_h16_arm64;
    uavs3e_funs_handle.trans_dct2[5][4] = uavs3e_trans_dct2_w64_h32_arm64;
    uavs3e_funs_handle.trans_dct2[5][5] = uavs3e_trans_dct2_w64_h64_arm64;

    uavs3e_funs_handle.itrans_dct2[1][1] = uavs3e_itrans_dct2_h4_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[1][2] = uavs3e_itrans_dct2_h4_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[1][3] = uavs3e_itrans_dct2_h4_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[1][4] = uavs3e_itrans_dct2_h4_w32_arm64;

    uavs3e_funs_handle.itrans_dct2[2][1] = uavs3e_itrans_dct2_h8_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[2][2] = uavs3e_itrans_dct2_h8_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[2][3] = uavs3e_itrans_dct2_h8_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[2][4] = uavs3e_itrans_dct2_h8_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[2][5] = uavs3e_itrans_dct2_h8_w64_arm64;

    uavs3e_funs_handle.itrans_dct2[3][1] = uavs3e_itrans_dct2_h16_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[3][2] = uavs3e_itrans_dct2_h16_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[3][3] = uavs3e_itrans_dct2_h16_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[3][4] = uavs3e_itrans_dct2_h16_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[3][5] = uavs3e_itrans_dct2_h16_w64_arm64;

    uavs3e_funs_handle.itrans_dct2[4][1] = uavs3e_itrans_dct2_h32_w4_arm64;
    uavs3e_funs_handle.itrans_dct2[4][2] = uavs3e_itrans_dct2_h32_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[4][3] = uavs3e_itrans_dct2_h32_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[4][4] = uavs3e_itrans_dct2_h32_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[4][5] = uavs3e_itrans_dct2_h32_w64_arm64;

    uavs3e_funs_handle.itrans_dct2[5][2] = uavs3e_itrans_dct2_h64_w8_arm64;
    uavs3e_funs_handle.itrans_dct2[5][3] = uavs3e_itrans_dct2_h64_w16_arm64;
    uavs3e_funs_handle.itrans_dct2[5][4] = uavs3e_itrans_dct2_h64_w32_arm64;
    uavs3e_funs_handle.itrans_dct2[5][5] = uavs3e_itrans_dct2_h64_w64_arm64;

    //uavs3e_funs_handle.itrans_dct8_dst7[0][1] = uavs3e_itrans_dct8_pb4_arm64;
    //uavs3e_funs_handle.itrans_dct8_dst7[0][2] = uavs3e_itrans_dct8_pb8_arm64;
    //uavs3e_funs_handle.itrans_dct8_dst7[0][3] = uavs3e_itrans_dct8_pb16_arm64;

    //uavs3e_funs_handle.itrans_dct8_dst7[1][1] = uavs3e_itrans_dst7_pb4_arm64;
    //uavs3e_funs_handle.itrans_dct8_dst7[1][2] = uavs3e_itrans_dst7_pb8_arm64;
    //uavs3e_funs_handle.itrans_dct8_dst7[1][3] = uavs3e_itrans_dst7_pb16_arm64;

    uavs3e_funs_handle.ipcpy[0] = uavs3e_if_cpy_w4_arm64;
    uavs3e_funs_handle.ipcpy[1] = uavs3e_if_cpy_w8_arm64;
    uavs3e_funs_handle.ipcpy[2] = uavs3e_if_cpy_w16_arm64;
    uavs3e_funs_handle.ipcpy[3] = uavs3e_if_cpy_w32_arm64;
    uavs3e_funs_handle.ipcpy[4] = uavs3e_if_cpy_w64_arm64;
    uavs3e_funs_handle.ipcpy[5] = uavs3e_if_cpy_w128_arm64;

    uavs3e_funs_handle.ipflt[IPFILTER_H_8][0] = uavs3e_if_hor_luma_w4_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][1] = uavs3e_if_hor_luma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][2] = uavs3e_if_hor_luma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][3] = uavs3e_if_hor_luma_w16x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][4] = uavs3e_if_hor_luma_w16x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][5] = uavs3e_if_hor_luma_w16x_arm64;

    uavs3e_funs_handle.ipflt[IPFILTER_H_4][1] = uavs3e_if_hor_chroma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][2] = uavs3e_if_hor_chroma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][3] = uavs3e_if_hor_chroma_w16x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][4] = uavs3e_if_hor_chroma_w16x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][5] = uavs3e_if_hor_chroma_w16x_arm64;

    uavs3e_funs_handle.ipflt[IPFILTER_V_8][0] = uavs3e_if_ver_luma_w4_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][1] = uavs3e_if_ver_luma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][2] = uavs3e_if_ver_luma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][3] = uavs3e_if_ver_luma_w16x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][4] = uavs3e_if_ver_luma_w16x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][5] = uavs3e_if_ver_luma_w16x_arm64;

    //uavs3e_funs_handle.ipflt[IPFILTER_V_4][0] = uavs3e_if_ver_chroma_w4_arm64;            //后面补
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][1] = uavs3e_if_ver_chroma_w8_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][2] = uavs3e_if_ver_chroma_w16_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][3] = uavs3e_if_ver_chroma_w32_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][4] = uavs3e_if_ver_chroma_w32x_arm64;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][5] = uavs3e_if_ver_chroma_w32x_arm64;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][0] = uavs3e_if_hor_ver_luma_w4_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][1] = uavs3e_if_hor_ver_luma_w8_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][2] = uavs3e_if_hor_ver_luma_w16_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][3] = uavs3e_if_hor_ver_luma_w32_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][4] = uavs3e_if_hor_ver_luma_w32x_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][5] = uavs3e_if_hor_ver_luma_w32x_arm64;

    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][1] = uavs3e_if_hor_ver_chroma_w8_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][2] = uavs3e_if_hor_ver_chroma_w16_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][3] = uavs3e_if_hor_ver_chroma_w32_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][4] = uavs3e_if_hor_ver_chroma_w32x_arm64;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][5] = uavs3e_if_hor_ver_chroma_w32x_arm64;

    uavs3e_funs_handle.intra_pred_dc        = uavs3e_intra_pred_dc_arm64;
    //uavs3e_funs_handle.intra_pred_bi        = uavs3e_intra_pred_bi_arm64;
    uavs3e_funs_handle.intra_pred_plane     = uavs3e_intra_pred_plane_arm64;
    uavs3e_funs_handle.intra_pred_hor       = uavs3e_intra_pred_hor_arm64;
    uavs3e_funs_handle.intra_pred_ver       = uavs3e_intra_pred_ver_arm64;
    //uavs3e_funs_handle.intra_pred_bi_ipf    = ipred_bi_ipf;
    //uavs3e_funs_handle.intra_pred_plane_ipf = ipred_plane_ipf;
    //uavs3e_funs_handle.intra_pred_ipf_core  = ipf_core;
    //uavs3e_funs_handle.intra_pred_ipf_core_s16 = ipf_core_s16;

    uavs3e_funs_handle.deblock_luma[0] = uavs3e_deblock_ver_luma_arm64;
    uavs3e_funs_handle.deblock_luma[1] = uavs3e_deblock_hor_luma_arm64;
    uavs3e_funs_handle.deblock_chroma[0] = uavs3e_deblock_ver_chroma_arm64;
    uavs3e_funs_handle.deblock_chroma[1] = uavs3e_deblock_hor_chroma_arm64;

    uavs3e_funs_handle.sao = uavs3e_sao_on_lcu_arm64;

    uavs3e_funs_handle.alf      = uavs3e_alf_filter_block_arm64;

    uavs3e_funs_handle.cost_sad[0] = uavs3e_get_sad_4_arm64;
    uavs3e_funs_handle.cost_sad[1] = uavs3e_get_sad_8_arm64;
    uavs3e_funs_handle.cost_sad[2] = uavs3e_get_sad_16_arm64;
    uavs3e_funs_handle.cost_sad[3] = uavs3e_get_sad_32_arm64;
    uavs3e_funs_handle.cost_sad[4] = uavs3e_get_sad_64_arm64;
    uavs3e_funs_handle.cost_sad[5] = uavs3e_get_sad_128_arm64;

    uavs3e_funs_handle.cost_ssd[0] = uavs3e_get_ssd_4_arm64;
    uavs3e_funs_handle.cost_ssd[1] = uavs3e_get_ssd_8_arm64;
    uavs3e_funs_handle.cost_ssd[2] = uavs3e_get_ssd_16_arm64;
    uavs3e_funs_handle.cost_ssd[3] = uavs3e_get_ssd_32_arm64;
    uavs3e_funs_handle.cost_ssd[4] = uavs3e_get_ssd_64_arm64;
    uavs3e_funs_handle.cost_ssd[5] = uavs3e_get_ssd_128_arm64;

    uavs3e_funs_handle.cost_sad_x3[0] = uavs3e_get_sad_x3_4_arm64;
    uavs3e_funs_handle.cost_sad_x3[1] = uavs3e_get_sad_x3_8_arm64;
    uavs3e_funs_handle.cost_sad_x3[2] = uavs3e_get_sad_x3_16_arm64;
    uavs3e_funs_handle.cost_sad_x3[3] = uavs3e_get_sad_x3_32_arm64;
    uavs3e_funs_handle.cost_sad_x3[4] = uavs3e_get_sad_x3_64_arm64;
    uavs3e_funs_handle.cost_sad_x3[5] = uavs3e_get_sad_x3_128_arm64;

    uavs3e_funs_handle.cost_sad_x4[0] = uavs3e_get_sad_x4_4_arm64;
    uavs3e_funs_handle.cost_sad_x4[1] = uavs3e_get_sad_x4_8_arm64;
    uavs3e_funs_handle.cost_sad_x4[2] = uavs3e_get_sad_x4_16_arm64;
    uavs3e_funs_handle.cost_sad_x4[3] = uavs3e_get_sad_x4_32_arm64;
    uavs3e_funs_handle.cost_sad_x4[4] = uavs3e_get_sad_x4_64_arm64;
    uavs3e_funs_handle.cost_sad_x4[5] = uavs3e_get_sad_x4_128_arm64;

    uavs3e_funs_handle.cost_satd[0][0] = uavs3e_had_4x4_arm64;
    uavs3e_funs_handle.cost_satd[1][0] = uavs3e_had_8x4_arm64;
    uavs3e_funs_handle.cost_satd[0][1] = uavs3e_had_4x8_arm64;
    uavs3e_funs_handle.cost_satd[1][1] = uavs3e_had_8x8_arm64;
    //uavs3e_funs_handle.cost_satd[2][1] = uavs3e_had_16x8_arm64;
    uavs3e_funs_handle.cost_satd[1][2] = uavs3e_had_8x16_arm64;

    uavs3e_funs_handle.recon[0] = uavs3e_recon_w4_arm64;
    uavs3e_funs_handle.recon[1] = uavs3e_recon_w8_arm64;
    uavs3e_funs_handle.recon[2] = uavs3e_recon_w16_arm64;
    uavs3e_funs_handle.recon[3] = uavs3e_recon_w32_arm64;

    uavs3e_funs_handle.pel_diff[0] = uavs3e_pel_diff_4_arm64;
    uavs3e_funs_handle.pel_diff[1] = uavs3e_pel_diff_8_arm64;
    uavs3e_funs_handle.pel_diff[2] = uavs3e_pel_diff_16_arm64;
    uavs3e_funs_handle.pel_diff[3] = uavs3e_pel_diff_32_arm64;
    uavs3e_funs_handle.pel_diff[4] = uavs3e_pel_diff_64_arm64;
    uavs3e_funs_handle.pel_diff[5] = uavs3e_pel_diff_128_arm64;

    uavs3e_funs_handle.pel_avrg[0] = uavs3e_pel_avrg_4_arm64;
    uavs3e_funs_handle.pel_avrg[1] = uavs3e_pel_avrg_8_arm64;
    uavs3e_funs_handle.pel_avrg[2] = uavs3e_pel_avrg_16_arm64;
    uavs3e_funs_handle.pel_avrg[3] = uavs3e_pel_avrg_32_arm64;
    uavs3e_funs_handle.pel_avrg[4] = uavs3e_pel_avrg_64_arm64;
    uavs3e_funs_handle.pel_avrg[5] = uavs3e_pel_avrg_128_arm64;

#endif

}
#endif
