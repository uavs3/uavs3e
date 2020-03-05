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

#include "sse.h"

#if defined(__GNUC__)    // GCC
#include <cpuid.h>
#endif

ALIGNED_32(pel uavs3e_simd_mask[15][16]) = {
    { -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0 }
};


// CPUIDFIELD

#define  CPUIDFIELD_MASK_POS    0x0000001F  // 位偏移. 0~31.  
#define  CPUIDFIELD_MASK_LEN    0x000003E0  // 位长. 1~32  
#define  CPUIDFIELD_MASK_REG    0x00000C00  // 寄存器. 0=EAX, 1=EBX, 2=ECX, 3=EDX.  
#define  CPUIDFIELD_MASK_FIDSUB 0x000FF000  // 子功能号(低8位).  
#define  CPUIDFIELD_MASK_FID    0xFFF00000  // 功能号(最高4位 和 低8位).  

#define CPUIDFIELD_SHIFT_POS    0
#define CPUIDFIELD_SHIFT_LEN    5
#define CPUIDFIELD_SHIFT_REG    10
#define CPUIDFIELD_SHIFT_FIDSUB 12
#define CPUIDFIELD_SHIFT_FID    20

#define CPUIDFIELD_MAKE(fid,fidsub,reg,pos,len) (((fid)&0xF0000000)     \
        | ((fid) << CPUIDFIELD_SHIFT_FID & 0x0FF00000)                      \
        | ((fidsub) << CPUIDFIELD_SHIFT_FIDSUB & CPUIDFIELD_MASK_FIDSUB)    \
        | ((reg) << CPUIDFIELD_SHIFT_REG & CPUIDFIELD_MASK_REG)             \
        | ((pos) << CPUIDFIELD_SHIFT_POS & CPUIDFIELD_MASK_POS)             \
        | (((len)-1) << CPUIDFIELD_SHIFT_LEN & CPUIDFIELD_MASK_LEN)         \
                                                )

#define CPUIDFIELD_FID(cpuidfield)  ( ((cpuidfield)&0xF0000000) | (((cpuidfield) & 0x0FF00000)>>CPUIDFIELD_SHIFT_FID) )
#define CPUIDFIELD_FIDSUB(cpuidfield)   ( ((cpuidfield) & CPUIDFIELD_MASK_FIDSUB)>>CPUIDFIELD_SHIFT_FIDSUB )
#define CPUIDFIELD_REG(cpuidfield)  ( ((cpuidfield) & CPUIDFIELD_MASK_REG)>>CPUIDFIELD_SHIFT_REG )
#define CPUIDFIELD_POS(cpuidfield)  ( ((cpuidfield) & CPUIDFIELD_MASK_POS)>>CPUIDFIELD_SHIFT_POS )
#define CPUIDFIELD_LEN(cpuidfield)  ( (((cpuidfield) & CPUIDFIELD_MASK_LEN)>>CPUIDFIELD_SHIFT_LEN) + 1 )

// 取得位域
#ifndef __GETBITS32
#define __GETBITS32(src,pos,len)    ( ((src)>>(pos)) & (((unsigned int)-1)>>(32-len)) )
#endif


#define CPUF_SSE4A  CPUIDFIELD_MAKE(0x80000001,0,2,6,1)
#define CPUF_AES    CPUIDFIELD_MAKE(1,0,2,25,1)
#define CPUF_PCLMULQDQ  CPUIDFIELD_MAKE(1,0,2,1,1)

#define CPUF_AVX    CPUIDFIELD_MAKE(1,0,2,28,1)
#define CPUF_AVX2   CPUIDFIELD_MAKE(7,0,1,5,1)
#define CPUF_OSXSAVE    CPUIDFIELD_MAKE(1,0,2,27,1)
#define CPUF_XFeatureSupportedMaskLo    CPUIDFIELD_MAKE(0xD,0,0,0,32)
#define CPUF_F16C   CPUIDFIELD_MAKE(1,0,2,29,1)
#define CPUF_FMA    CPUIDFIELD_MAKE(1,0,2,12,1)
#define CPUF_FMA4   CPUIDFIELD_MAKE(0x80000001,0,2,16,1)
#define CPUF_XOP    CPUIDFIELD_MAKE(0x80000001,0,2,11,1)


// SSE系列指令集的支持级别. simd_sse_level 函数的返回值。
#define SIMD_SSE_NONE   0   // 不支持  
#define SIMD_SSE_1  1   // SSE  
#define SIMD_SSE_2  2   // SSE2  
#define SIMD_SSE_3  3   // SSE3  
#define SIMD_SSE_3S 4   // SSSE3  
#define SIMD_SSE_41 5   // SSE4.1  
#define SIMD_SSE_42 6   // SSE4.2  

const char *uavs3e_simd_sse_names[] = {
    "None",
    "SSE",
    "SSE2",
    "SSE3",
    "SSSE3",
    "SSE4.1",
    "SSE4.2",
};


// AVX系列指令集的支持级别. uavs3e_simd_avx_level 函数的返回值。
#define SIMD_AVX_NONE   0   // 不支持  
#define SIMD_AVX_1  1   // AVX  
#define SIMD_AVX_2  2   // AVX2  

const char *uavs3e_simd_avx_names[] = {
    "None",
    "AVX",
    "AVX2"
};


// 根据CPUIDFIELD从缓冲区中获取字段.
unsigned int  uavs3e_getcpuidfield_buf(const int dwBuf[4], int cpuf)
{
    return __GETBITS32(dwBuf[CPUIDFIELD_REG(cpuf)], CPUIDFIELD_POS(cpuf), CPUIDFIELD_LEN(cpuf));
}

// 根据CPUIDFIELD获取CPUID字段.

void uavs3e_getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(__GNUC__)    // GCC
    __cpuid_count(InfoType, ECXValue, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
#if defined(_WIN64) || _MSC_VER>=1600    // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
    __cpuidex((int *)(void *)CPUInfo, (int)InfoType, (int)ECXValue);
#else
    if (NULL == CPUInfo) {
        return;
    }
    _asm {
        // load. 读取参数到寄存器.
        mov edi, CPUInfo;    // 准备用edi寻址CPUInfo
        mov eax, InfoType;
        mov ecx, ECXValue;
        // CPUID
        cpuid;
        // save. 将寄存器保存到CPUInfo
        mov[edi], eax;
        mov[edi + 4], ebx;
        mov[edi + 8], ecx;
        mov[edi + 12], edx;
    }
#endif
#endif    // #if defined(__GNUC__)
}

unsigned int  uavs3e_getcpuidfield(int cpuf)
{
    int dwBuf[4];
    uavs3e_getcpuidex(dwBuf, CPUIDFIELD_FID(cpuf), CPUIDFIELD_FIDSUB(cpuf));
    return uavs3e_getcpuidfield_buf(dwBuf, cpuf);
}

// 检测AVX系列指令集的支持级别.
int uavs3e_simd_avx_level(int *phwavx)
{
    int rt = SIMD_AVX_NONE; // result

    // check processor support
    if (0 != uavs3e_getcpuidfield(CPUF_AVX)) {
        rt = SIMD_AVX_1;
        if (0 != uavs3e_getcpuidfield(CPUF_AVX2)) {
            rt = SIMD_AVX_2;
        }
    }
    if (NULL != phwavx) {
        *phwavx = rt;
    }

    // check OS support
    if (0 != uavs3e_getcpuidfield(CPUF_OSXSAVE)) { // XGETBV enabled for application use.
        unsigned int n = uavs3e_getcpuidfield(CPUF_XFeatureSupportedMaskLo); // XCR0: XFeatureSupportedMask register.
        if (6 == (n & 6)) { // XCR0[2:1] = ‘11b’ (XMM state and YMM state are enabled by OS).
            return rt;
        }
    }
    return SIMD_AVX_NONE;
}

#if (BIT_DEPTH == 8)
void uavs3e_funs_init_sse()
{
    int i;
    for (i = 0; i < CU_SIZE_NUM; i++) {
        uavs3e_funs_handle.ipcpy[i] = uavs3e_if_cpy_w16x_sse;
        uavs3e_funs_handle.ipflt[IPFILTER_H_8][i] = uavs3e_if_hor_luma_w8x_sse;
        uavs3e_funs_handle.ipflt[IPFILTER_V_8][i] = uavs3e_if_ver_luma_w16x_sse;
        uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][i] = uavs3e_if_hor_ver_luma_w8x_sse;

        uavs3e_funs_handle.ipflt[IPFILTER_H_4][i] = uavs3e_if_hor_chroma_w8x_sse;
        uavs3e_funs_handle.ipflt[IPFILTER_V_4][i] = uavs3e_if_ver_chroma_w16x_sse;
        uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][i] = uavs3e_if_hor_ver_chroma_w8x_sse;
    }

    uavs3e_funs_handle.ipcpy[0] = uavs3e_if_cpy_w4_sse;
    uavs3e_funs_handle.ipcpy[1] = uavs3e_if_cpy_w8_sse;
    uavs3e_funs_handle.ipcpy[2] = uavs3e_if_cpy_w16_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][0] = uavs3e_if_hor_luma_w4_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][1] = uavs3e_if_hor_luma_w8_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][0] = uavs3e_if_ver_luma_w4_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][1] = uavs3e_if_ver_luma_w8_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][2] = uavs3e_if_ver_luma_w16_sse;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][0] = uavs3e_if_hor_ver_luma_w4_sse;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][1] = uavs3e_if_hor_ver_luma_w8_sse;

    uavs3e_funs_handle.ipflt[IPFILTER_H_4][0] = uavs3e_if_hor_chroma_w4_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_H_4][1] = uavs3e_if_hor_chroma_w8_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][0] = uavs3e_if_ver_chroma_w4_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][1] = uavs3e_if_ver_chroma_w8_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_4][2] = uavs3e_if_ver_chroma_w16_sse;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][0] = uavs3e_if_hor_ver_chroma_w4_sse;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_4][1] = uavs3e_if_hor_ver_chroma_w8_sse;


    //uavs3e_funs_handle.padding_rows_luma = uavs3e_padding_rows_luma_sse;
    //uavs3e_funs_handle.padding_rows_chroma = uavs3e_padding_rows_chroma_sse;
    //uavs3e_funs_handle.conv_fmt_8bit = uavs3e_conv_fmt_8bit_sse;
    //uavs3e_funs_handle.conv_fmt_16bit = uavs3e_conv_fmt_16bit_sse;
    //uavs3e_funs_handle.conv_fmt_16to8bit = uavs3e_conv_fmt_16to8bit_sse;
    //

    uavs3e_funs_handle.recon[0] = uavs3e_recon_w4_sse;
    uavs3e_funs_handle.recon[1] = uavs3e_recon_w8_sse;
    uavs3e_funs_handle.recon[2] = uavs3e_recon_w16_sse;
    uavs3e_funs_handle.recon[3] = uavs3e_recon_w32_sse;
    uavs3e_funs_handle.recon[4] = uavs3e_recon_w64_sse;

    uavs3e_funs_handle.itrans_dct2[1][1] = uavs3e_itrans_dct2_h4_w4_sse;
    uavs3e_funs_handle.itrans_dct2[1][2] = uavs3e_itrans_dct2_h4_w8_sse;
    uavs3e_funs_handle.itrans_dct2[1][3] = uavs3e_itrans_dct2_h4_w16_sse;
    uavs3e_funs_handle.itrans_dct2[1][4] = uavs3e_itrans_dct2_h4_w32_sse;
    
    uavs3e_funs_handle.itrans_dct2[2][1] = uavs3e_itrans_dct2_h8_w4_sse;
    uavs3e_funs_handle.itrans_dct2[2][2] = uavs3e_itrans_dct2_h8_w8_sse;
    uavs3e_funs_handle.itrans_dct2[2][3] = uavs3e_itrans_dct2_h8_w16_sse;
    uavs3e_funs_handle.itrans_dct2[2][4] = uavs3e_itrans_dct2_h8_w32_sse;
    uavs3e_funs_handle.itrans_dct2[2][5] = uavs3e_itrans_dct2_h8_w64_sse;
    
    uavs3e_funs_handle.itrans_dct2[3][1] = uavs3e_itrans_dct2_h16_w4_sse;
    uavs3e_funs_handle.itrans_dct2[3][2] = uavs3e_itrans_dct2_h16_w8_sse;
    uavs3e_funs_handle.itrans_dct2[3][3] = uavs3e_itrans_dct2_h16_w16_sse;
    uavs3e_funs_handle.itrans_dct2[3][4] = uavs3e_itrans_dct2_h16_w32_sse;
    uavs3e_funs_handle.itrans_dct2[3][5] = uavs3e_itrans_dct2_h16_w64_sse;
    
    uavs3e_funs_handle.itrans_dct2[4][1] = uavs3e_itrans_dct2_h32_w4_sse;
    uavs3e_funs_handle.itrans_dct2[4][2] = uavs3e_itrans_dct2_h32_w8_sse;
    uavs3e_funs_handle.itrans_dct2[4][3] = uavs3e_itrans_dct2_h32_w16_sse;
    uavs3e_funs_handle.itrans_dct2[4][4] = uavs3e_itrans_dct2_h32_w32_sse;
    uavs3e_funs_handle.itrans_dct2[4][5] = uavs3e_itrans_dct2_h32_w64_sse;
    
    uavs3e_funs_handle.itrans_dct2[5][2] = uavs3e_itrans_dct2_h64_w8_sse;
    uavs3e_funs_handle.itrans_dct2[5][3] = uavs3e_itrans_dct2_h64_w16_sse;
    uavs3e_funs_handle.itrans_dct2[5][4] = uavs3e_itrans_dct2_h64_w32_sse;
    uavs3e_funs_handle.itrans_dct2[5][5] = uavs3e_itrans_dct2_h64_w64_sse;

    uavs3e_funs_handle.deblock_luma[0] = uavs3e_deblock_ver_luma_sse;
    uavs3e_funs_handle.deblock_luma[1] = uavs3e_deblock_hor_luma_sse;
    //uavs3e_funs_handle.deblock_chroma[0] = uavs3e_deblock_ver_chroma_sse;
    //uavs3e_funs_handle.deblock_chroma[1] = uavs3e_deblock_hor_chroma_sse;

    uavs3e_funs_handle.sao = uavs3e_sao_on_lcu_sse;
    //uavs3e_funs_handle.alf = uavs3e_alf_one_lcu_sse; // ERROR

    uavs3e_funs_handle.cost_sad[0] = uavs3e_get_sad_4_sse;
    uavs3e_funs_handle.cost_sad[1] = uavs3e_get_sad_8_sse;
    uavs3e_funs_handle.cost_sad[2] = uavs3e_get_sad_16_sse;
    uavs3e_funs_handle.cost_sad[3] = uavs3e_get_sad_32_sse;
    uavs3e_funs_handle.cost_sad[4] = uavs3e_get_sad_64_sse;
    uavs3e_funs_handle.cost_sad[5] = uavs3e_get_sad_128_sse;

    uavs3e_funs_handle.cost_ssd[0] = uavs3e_get_ssd_4_sse;
    uavs3e_funs_handle.cost_ssd[1] = uavs3e_get_ssd_8_sse;
    uavs3e_funs_handle.cost_ssd[2] = uavs3e_get_ssd_16_sse;
    uavs3e_funs_handle.cost_ssd[3] = uavs3e_get_ssd_32_sse;
    uavs3e_funs_handle.cost_ssd[4] = uavs3e_get_ssd_64_sse;
    uavs3e_funs_handle.cost_ssd[5] = uavs3e_get_ssd_128_sse;

    uavs3e_funs_handle.cost_satd[0][0] = uavs3e_had_4x4_sse;
    uavs3e_funs_handle.cost_satd[1][0] = uavs3e_had_8x4_sse;
    uavs3e_funs_handle.cost_satd[0][1] = uavs3e_had_4x8_sse;
    uavs3e_funs_handle.cost_satd[1][1] = uavs3e_had_8x8_sse;
    uavs3e_funs_handle.cost_satd[2][1] = uavs3e_had_16x8_sse;
    uavs3e_funs_handle.cost_satd[1][2] = uavs3e_had_8x16_sse;

    uavs3e_funs_handle.pel_diff[0] = uavs3e_pel_diff_4_sse;
    uavs3e_funs_handle.pel_diff[1] = uavs3e_pel_diff_8_sse;
    uavs3e_funs_handle.pel_diff[2] = uavs3e_pel_diff_16_sse;
    uavs3e_funs_handle.pel_diff[3] = uavs3e_pel_diff_32_sse;
    uavs3e_funs_handle.pel_diff[4] = uavs3e_pel_diff_64_sse;
    uavs3e_funs_handle.pel_diff[5] = uavs3e_pel_diff_128_sse;

    uavs3e_funs_handle.pel_avrg[0] = uavs3e_pel_avrg_4_sse;
    uavs3e_funs_handle.pel_avrg[1] = uavs3e_pel_avrg_8_sse;
    uavs3e_funs_handle.pel_avrg[2] = uavs3e_pel_avrg_16_sse;
    uavs3e_funs_handle.pel_avrg[3] = uavs3e_pel_avrg_32_sse;
    uavs3e_funs_handle.pel_avrg[4] = uavs3e_pel_avrg_64_sse;
    uavs3e_funs_handle.pel_avrg[5] = uavs3e_pel_avrg_128_sse;

    uavs3e_funs_handle.affine_sobel_flt_hor = affine_sobel_flt_hor_sse;
    uavs3e_funs_handle.affine_sobel_flt_ver = affine_sobel_flt_ver_sse;
    uavs3e_funs_handle.affine_coef_computer = affine_coef_computer_sse;

    uavs3e_funs_handle.intra_pred_dc    = uavs3e_ipred_dc_sse;
    uavs3e_funs_handle.intra_pred_plane = uavs3e_ipred_plane_sse;
    uavs3e_funs_handle.intra_pred_bi    = uavs3e_ipred_bi_sse;
    uavs3e_funs_handle.intra_pred_hor   = uavs3e_ipred_hor_sse;
    uavs3e_funs_handle.intra_pred_ver   = uavs3e_ipred_ver_sse;
    uavs3e_funs_handle.intra_pred_ipf_core = uavs3e_ipred_ipf_core_sse;
    uavs3e_funs_handle.intra_pred_ipf_core_s16 = uavs3e_ipred_ipf_core_s16_sse;

    for (i = IPD_BI + 1; i < IPD_VER; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = uavs3e_ipred_ang_x_sse;
    }
    for (i = IPD_HOR + 1; i < IPD_CNT; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = uavs3e_ipred_ang_y_sse;
    }
    //uavs3e_funs_handle.intra_pred_ang[3] = uavs3e_ipred_ang_x_3_sse;
    uavs3e_funs_handle.intra_pred_ang[4] = uavs3e_ipred_ang_x_4_sse;
    //uavs3e_funs_handle.intra_pred_ang[5] = uavs3e_ipred_ang_x_5_sse;
    //uavs3e_funs_handle.intra_pred_ang[6] = uavs3e_ipred_ang_x_6_sse;
    //uavs3e_funs_handle.intra_pred_ang[7] = uavs3e_ipred_ang_x_7_sse;
    uavs3e_funs_handle.intra_pred_ang[8] = uavs3e_ipred_ang_x_8_sse;
    //uavs3e_funs_handle.intra_pred_ang[9] = uavs3e_ipred_ang_x_9_sse;
    //uavs3e_funs_handle.intra_pred_ang[10] = uavs3e_ipred_ang_x_10_sse;
    //uavs3e_funs_handle.intra_pred_ang[11] = uavs3e_ipred_ang_x_11_sse;
    
    //uavs3e_funs_handle.intra_pred_ang[14] = uavs3e_ipred_ang_xy_14_sse;
    uavs3e_funs_handle.intra_pred_ang[16] = uavs3e_ipred_ang_xy_16_sse;
    uavs3e_funs_handle.intra_pred_ang[18] = uavs3e_ipred_ang_xy_18_sse;
    uavs3e_funs_handle.intra_pred_ang[20] = uavs3e_ipred_ang_xy_20_sse;
    //uavs3e_funs_handle.intra_pred_ang[22] = uavs3e_ipred_ang_xy_22_sse;
    
    //uavs3e_funs_handle.intra_pred_ang[25] = uavs3e_ipred_ang_y_25_sse;
    //uavs3e_funs_handle.intra_pred_ang[26] = uavs3e_ipred_ang_y_26_sse;
    //uavs3e_funs_handle.intra_pred_ang[27] = uavs3e_ipred_ang_y_27_sse;
    uavs3e_funs_handle.intra_pred_ang[28] = uavs3e_ipred_ang_y_28_sse;
    //uavs3e_funs_handle.intra_pred_ang[29] = uavs3e_ipred_ang_y_29_sse;
    //uavs3e_funs_handle.intra_pred_ang[30] = uavs3e_ipred_ang_y_30_sse;
    //uavs3e_funs_handle.intra_pred_ang[31] = uavs3e_ipred_ang_y_31_sse;
    uavs3e_funs_handle.intra_pred_ang[32] = uavs3e_ipred_ang_y_32_sse;
}

#elif (BIT_DEPTH == 10)
void uavs3e_funs_init_sse()
{
    int i;
    for (i = 0; i < CU_SIZE_NUM; i++) {
        uavs3e_funs_handle.ipcpy[i] = uavs3e_if_cpy_w16x_sse;
        uavs3e_funs_handle.ipflt[IPFILTER_H_8][i] = uavs3e_if_hor_luma_w8x_sse;
        uavs3e_funs_handle.ipflt[IPFILTER_V_8][i] = uavs3e_if_ver_luma_w8x_sse;
        uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][i] = uavs3e_if_hor_ver_luma_w8x_sse;
    }

    uavs3e_funs_handle.ipcpy[0] = uavs3e_if_cpy_w4_sse;
    uavs3e_funs_handle.ipcpy[1] = uavs3e_if_cpy_w8_sse;
    uavs3e_funs_handle.ipcpy[2] = uavs3e_if_cpy_w16_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][0] = uavs3e_if_hor_luma_w4_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_H_8][1] = uavs3e_if_hor_luma_w8_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][0] = uavs3e_if_ver_luma_w4_sse;
    uavs3e_funs_handle.ipflt[IPFILTER_V_8][1] = uavs3e_if_ver_luma_w8_sse;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][0] = uavs3e_if_hor_ver_luma_w4_sse;
    uavs3e_funs_handle.ipflt_ext[IPFILTER_EXT_8][1] = uavs3e_if_hor_ver_luma_w8_sse;

    uavs3e_funs_handle.itrans_dct2[1][1] = uavs3e_itrans_dct2_h4_w4_sse;
    uavs3e_funs_handle.itrans_dct2[1][2] = uavs3e_itrans_dct2_h4_w8_sse;
    uavs3e_funs_handle.itrans_dct2[1][3] = uavs3e_itrans_dct2_h4_w16_sse;
    uavs3e_funs_handle.itrans_dct2[1][4] = uavs3e_itrans_dct2_h4_w32_sse;

    uavs3e_funs_handle.itrans_dct2[2][1] = uavs3e_itrans_dct2_h8_w4_sse;
    uavs3e_funs_handle.itrans_dct2[2][2] = uavs3e_itrans_dct2_h8_w8_sse;
    uavs3e_funs_handle.itrans_dct2[2][3] = uavs3e_itrans_dct2_h8_w16_sse;
    uavs3e_funs_handle.itrans_dct2[2][4] = uavs3e_itrans_dct2_h8_w32_sse;
    uavs3e_funs_handle.itrans_dct2[2][5] = uavs3e_itrans_dct2_h8_w64_sse;

    uavs3e_funs_handle.itrans_dct2[3][1] = uavs3e_itrans_dct2_h16_w4_sse;
    uavs3e_funs_handle.itrans_dct2[3][2] = uavs3e_itrans_dct2_h16_w8_sse;
    uavs3e_funs_handle.itrans_dct2[3][3] = uavs3e_itrans_dct2_h16_w16_sse;
    uavs3e_funs_handle.itrans_dct2[3][4] = uavs3e_itrans_dct2_h16_w32_sse;
    uavs3e_funs_handle.itrans_dct2[3][5] = uavs3e_itrans_dct2_h16_w64_sse;

    uavs3e_funs_handle.itrans_dct2[4][1] = uavs3e_itrans_dct2_h32_w4_sse;
    uavs3e_funs_handle.itrans_dct2[4][2] = uavs3e_itrans_dct2_h32_w8_sse;
    uavs3e_funs_handle.itrans_dct2[4][3] = uavs3e_itrans_dct2_h32_w16_sse;
    uavs3e_funs_handle.itrans_dct2[4][4] = uavs3e_itrans_dct2_h32_w32_sse;
    uavs3e_funs_handle.itrans_dct2[4][5] = uavs3e_itrans_dct2_h32_w64_sse;

    uavs3e_funs_handle.itrans_dct2[5][2] = uavs3e_itrans_dct2_h64_w8_sse;
    uavs3e_funs_handle.itrans_dct2[5][3] = uavs3e_itrans_dct2_h64_w16_sse;
    uavs3e_funs_handle.itrans_dct2[5][4] = uavs3e_itrans_dct2_h64_w32_sse;
    uavs3e_funs_handle.itrans_dct2[5][5] = uavs3e_itrans_dct2_h64_w64_sse;

    uavs3e_funs_handle.deblock_luma[0] = uavs3e_deblock_ver_luma_sse;
    uavs3e_funs_handle.deblock_luma[1] = uavs3e_deblock_hor_luma_sse;
    uavs3e_funs_handle.deblock_chroma[0] = uavs3e_deblock_ver_chroma_sse;
    uavs3e_funs_handle.deblock_chroma[1] = uavs3e_deblock_hor_chroma_sse;

    uavs3e_funs_handle.sao = uavs3e_sao_on_lcu_sse;
    //uavs3e_funs_handle.alf = uavs3e_alf_one_lcu_sse; // ERROR

    uavs3e_funs_handle.cost_satd[0][0] = uavs3e_had_4x4_sse;

    uavs3e_funs_handle.recon[0] = uavs3e_recon_w4_sse;

    uavs3e_funs_handle.pel_diff[0] = uavs3e_pel_diff_4_sse;
    uavs3e_funs_handle.pel_diff[1] = uavs3e_pel_diff_8_sse;

    uavs3e_funs_handle.affine_sobel_flt_hor = affine_sobel_flt_hor_sse;
    uavs3e_funs_handle.affine_sobel_flt_ver = affine_sobel_flt_ver_sse;
    uavs3e_funs_handle.affine_coef_computer = affine_coef_computer_sse;

}

#endif