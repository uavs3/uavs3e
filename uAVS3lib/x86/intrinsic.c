#include "intrinsic.h" 
#include "intrinsic_10.h"

ALIGNED_16(char_t intrinsic_mask[15][16]) = {
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

ALIGNED_32(i16s_t intrinsic_mask_10bit[15][16]) = {
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

const char* simd_sse_names[] = {
    "None",
    "SSE",
    "SSE2",
    "SSE3",
    "SSSE3",
    "SSE4.1",
    "SSE4.2",
};


// AVX系列指令集的支持级别. simd_avx_level 函数的返回值。  
#define SIMD_AVX_NONE   0   // 不支持  
#define SIMD_AVX_1  1   // AVX  
#define SIMD_AVX_2  2   // AVX2  

const char* simd_avx_names[] = {
    "None",
    "AVX",
    "AVX2"
};


// 根据CPUIDFIELD从缓冲区中获取字段.  
unsigned int  getcpuidfield_buf(const int dwBuf[4], int cpuf)
{
    return __GETBITS32(dwBuf[CPUIDFIELD_REG(cpuf)], CPUIDFIELD_POS(cpuf), CPUIDFIELD_LEN(cpuf));
}

// 根据CPUIDFIELD获取CPUID字段.  
unsigned int  getcpuidfield(int cpuf)
{
#if defined(_WIN32) && !defined(__GNUC__) 
    int dwBuf[4];
    __cpuidex(dwBuf, CPUIDFIELD_FID(cpuf), CPUIDFIELD_FIDSUB(cpuf));
    return getcpuidfield_buf(dwBuf, cpuf);
#else
    return 6;
#endif
}

// 检测AVX系列指令集的支持级别.  
int simd_avx_level(int* phwavx)
{
    int rt = SIMD_AVX_NONE; // result  

    // check processor support  
    if (0 != getcpuidfield(CPUF_AVX))
    {
        rt = SIMD_AVX_1;
        if (0 != getcpuidfield(CPUF_AVX2))
        {
            rt = SIMD_AVX_2;
        }
    }
    if (NULL != phwavx)   *phwavx = rt;

    // check OS support  
    if (0 != getcpuidfield(CPUF_OSXSAVE)) // XGETBV enabled for application use.  
    {
        unsigned int n = getcpuidfield(CPUF_XFeatureSupportedMaskLo); // XCR0: XFeatureSupportedMask register.  
        if (6 == (n & 6))   // XCR0[2:1] = ‘11b’ (XMM state and YMM state are enabled by OS).  
        {
            return rt;
        }
    }
    return SIMD_AVX_NONE;
}

void com_funs_init_intrinsic_functions()
{
    int i;

    g_funs_handle.cost_ssd[0] = xGetSSE4_sse128;
    g_funs_handle.cost_ssd[1] = xGetSSE8_sse128;
    g_funs_handle.cost_ssd[2] = xGetSSE16_sse128;
    g_funs_handle.cost_ssd[3] = xGetSSE32_sse128;
    g_funs_handle.cost_ssd[4] = xGetSSE64_sse128;
	g_funs_handle.cost_ssd_ext = xGetSSE_Ext_sse128;
	g_funs_handle.cost_ssd_psnr = xGetSSE_Psnr_sse128;

    g_funs_handle.cost_sad[ 1] = xGetSAD4_sse128;
    g_funs_handle.cost_sad[ 2] = xGetSAD8_sse128;
    g_funs_handle.cost_sad[ 4] = xGetSAD16_sse128;
    g_funs_handle.cost_sad[ 8] = xGetSAD32_sse128;
    g_funs_handle.cost_sad[16] = xGetSAD64_sse128;

    g_funs_handle.cost_avg_sad[ 1] = xGetAVGSAD4_sse128;
    g_funs_handle.cost_avg_sad[ 2] = xGetAVGSAD8_sse128;
    g_funs_handle.cost_avg_sad[ 4] = xGetAVGSAD16_sse128;
    g_funs_handle.cost_avg_sad[ 8] = xGetAVGSAD32_sse128;
    g_funs_handle.cost_avg_sad[16] = xGetAVGSAD64_sse128;

    g_funs_handle.cost_sad_x4[ 1] = xGetSAD4_x4_sse128;
    g_funs_handle.cost_sad_x4[ 2] = xGetSAD8_x4_sse128;
    g_funs_handle.cost_sad_x4[ 4] = xGetSAD16_x4_sse128;
    g_funs_handle.cost_sad_x4[ 8] = xGetSAD32_x4_sse128;
    g_funs_handle.cost_sad_x4[16] = xGetSAD64_x4_sse128;

    g_funs_handle.cost_blk_satd[0] = xCalcHAD4x4_sse128;
    g_funs_handle.cost_blk_satd[1] = xCalcHAD8x8_sse128;

    g_funs_handle.cost_satd_i = xCalcHADs8x8_I_sse128;

    for (i = 0; i < 33; i++) {
        g_funs_handle.avg_pel[i] = avg_pel_0_sse128;
    }
    g_funs_handle.avg_pel[ 1] = avg_pel_2_sse128;
    g_funs_handle.avg_pel[ 2] = avg_pel_4_sse128;
    g_funs_handle.avg_pel[ 4] = avg_pel_8_sse128;
    g_funs_handle.avg_pel[ 8] = avg_pel_16_sse128;
    g_funs_handle.avg_pel[16] = avg_pel_x16_sse128;
    g_funs_handle.avg_pel[32] = avg_pel_x16_sse128;
    g_funs_handle.avg_pel_1d  = avg_pel_1d_sse128;

    g_funs_handle.cpy_pel[ 8] = com_mem_cpy16_sse128;
    g_funs_handle.cpy_pel[16] = com_mem_cpy32_sse128;
    g_funs_handle.cpy_pel[32] = com_mem_cpy64_sse128;

    g_funs_handle.padding_rows = padding_rows_sse128;

    g_funs_handle.intra_pred_ang_x[ 3 - ANG_X_OFFSET] = xPredIntraAngAdi_X_3_sse128;
    g_funs_handle.intra_pred_ang_x[ 4 - ANG_X_OFFSET] = xPredIntraAngAdi_X_4_sse128;
    g_funs_handle.intra_pred_ang_x[ 5 - ANG_X_OFFSET] = xPredIntraAngAdi_X_5_sse128;
    g_funs_handle.intra_pred_ang_x[ 6 - ANG_X_OFFSET] = xPredIntraAngAdi_X_6_sse128;
    g_funs_handle.intra_pred_ang_x[ 7 - ANG_X_OFFSET] = xPredIntraAngAdi_X_7_sse128;
    g_funs_handle.intra_pred_ang_x[ 8 - ANG_X_OFFSET] = xPredIntraAngAdi_X_8_sse128;
    g_funs_handle.intra_pred_ang_x[ 9 - ANG_X_OFFSET] = xPredIntraAngAdi_X_9_sse128;
    g_funs_handle.intra_pred_ang_x[10 - ANG_X_OFFSET] = xPredIntraAngAdi_X_10_sse128;
    g_funs_handle.intra_pred_ang_x[11 - ANG_X_OFFSET] = xPredIntraAngAdi_X_11_sse128;

    g_funs_handle.intra_pred_ang_xy[14 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_14_sse128;
    g_funs_handle.intra_pred_ang_xy[16 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_16_sse128;
    g_funs_handle.intra_pred_ang_xy[18 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_18_sse128;
    g_funs_handle.intra_pred_ang_xy[20 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_20_sse128;
    g_funs_handle.intra_pred_ang_xy[22 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_22_sse128;

    g_funs_handle.intra_pred_ang_y[25 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_25_sse128;
    g_funs_handle.intra_pred_ang_y[26 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_26_sse128;
    g_funs_handle.intra_pred_ang_y[27 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_27_sse128;
    g_funs_handle.intra_pred_ang_y[28 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_28_sse128;
    g_funs_handle.intra_pred_ang_y[29 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_29_sse128;
    g_funs_handle.intra_pred_ang_y[30 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_30_sse128;
    g_funs_handle.intra_pred_ang_y[31 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_31_sse128;
    g_funs_handle.intra_pred_ang_y[32 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_32_sse128;

    g_funs_handle.intra_pred_plane = xPredIntraPlaneAdi_sse128;
    g_funs_handle.intra_pred_hor   = xPredIntraHorAdi_sse128;
    g_funs_handle.intra_pred_ver   = xPredIntraVertAdi_sse128;
    g_funs_handle.intra_pred_dc    = xPredIntraDCAdi_sse128;
    g_funs_handle.intra_pred_bi    = xPredIntraBiAdi_sse128;

    for (i = 0; i < 16; i++) {
        g_funs_handle.ip_flt_c[IP_FLT_C_H][i] = com_if_filter_hor_4_sse128;
        g_funs_handle.ip_flt_c[IP_FLT_C_V][i] = com_if_filter_ver_4_sse128;
        g_funs_handle.ip_flt_c_ext[i] = com_if_filter_hor_ver_4_sse128;
    }

	g_funs_handle.ip_flt_y_hor = com_if_filter_hor_8_sse128;
	g_funs_handle.ip_flt_y_ver = com_if_filter_ver_8_sse128;
	g_funs_handle.ip_flt_y_ver_ext = com_if_filter_ver_8_ext_sse128;

    g_funs_handle.quant_ext = quant_ext_sse128;
	g_funs_handle.quant = quant_normal_sse128;
    g_funs_handle.inv_quant = inv_quant_normal_sse128;
    g_funs_handle.inv_quant_ext = inv_quant_ext_sse128;
    g_funs_handle.add_sign = add_sign_sse128;
    g_funs_handle.pre_quant = pre_quant_sse128;

    g_funs_handle.deblk_luma[0] = deblock_luma_ver_sse128;
    g_funs_handle.deblk_luma[1] = deblock_luma_hor_sse128;
    g_funs_handle.deblk_chroma[0] = deblock_chroma_ver_sse128;
    g_funs_handle.deblk_chroma[1] = deblock_chroma_hor_sse128;
#ifdef _WIN32
    g_funs_handle.sao_flt[0] = sao_get_stats_sse128;
#endif
    g_funs_handle.sao_flt[1] = SAO_on_block_sse128;

	g_funs_handle.add_inv_trans[0] = add_inv_trans_4x4_sec_sse128;
    g_funs_handle.add_inv_trans[1] = add_inv_trans_4x4_sse128;
    g_funs_handle.add_inv_trans[2] = add_inv_trans_8x8_sse128;
    g_funs_handle.add_inv_trans[3] = add_inv_trans_16x16_sse128;
    g_funs_handle.add_inv_trans[4] = add_inv_trans_32x32_sse128;

	g_funs_handle.inv_trans_2nd_Hor = xTr2nd_8_1d_Inv_Hor_sse128;
	g_funs_handle.inv_trans_2nd_Ver = xTr2nd_8_1d_Inv_Vert_sse128;

	g_funs_handle.sub_trans[0] = sub_trans_4x4_sec_sse128;
	g_funs_handle.sub_trans[1] = sub_trans_4x4_sse128;
	g_funs_handle.sub_trans[2] = sub_trans_8x8_sse128;
	g_funs_handle.sub_trans[3] = sub_trans_16x16_sse128;
	g_funs_handle.sub_trans[4] = sub_trans_32x32_sse128;

    g_funs_handle.add_inv_trans_ext[4] = add_inv_trans_ext_32x32_sse128;
    g_funs_handle.sub_trans_ext[4] = sub_trans_ext_32x32_sse128;

	g_funs_handle.trans_2nd_Hor = xTr2nd_8_1d_Hor_sse128;
	g_funs_handle.trans_2nd_Ver = xTr2nd_8_1d_Vert_sse128;

    g_funs_handle.calc_blk_ave = calc_ave_sse128;
    g_funs_handle.calc_blk_var = calc_var_sse128;

    g_funs_handle.get_cg_bits = get_cg_bits_sse128;
    g_funs_handle.zero_cg_check[1] = zero_cg_check8_sse128;
}

void com_funs_init_intrinsic_functions_avx2()
{
    g_funs_handle.cost_sad[ 8] = xGetSAD32_sse256;
    g_funs_handle.cost_sad[16] = xGetSAD64_sse256;

    g_funs_handle.cost_avg_sad[ 8] = xGetAVGSAD32_sse256;
    g_funs_handle.cost_avg_sad[16] = xGetAVGSAD64_sse256;

    g_funs_handle.cost_sad_x4[ 4] = xGetSAD16_x4_sse256;
    g_funs_handle.cost_sad_x4[ 8] = xGetSAD32_x4_sse256;
    g_funs_handle.cost_sad_x4[16] = xGetSAD64_x4_sse256;

    g_funs_handle.cost_ssd[2] = xGetSSE16_sse256;
    g_funs_handle.cost_ssd[3] = xGetSSE32_sse256;
    g_funs_handle.cost_ssd[4] = xGetSSE64_sse256;

    g_funs_handle.cost_blk_satd[2] = xCalcHAD16x16_sse256;
    g_funs_handle.cost_blk_satd[3] = xCalcHAD32x32_sse256;
    g_funs_handle.cost_blk_satd[4] = xCalcHAD64x64_sse256;

    g_funs_handle.ip_flt_c[IP_FLT_C_H][ 7] = com_if_filter_hor_4_w16_sse256;
    g_funs_handle.ip_flt_c[IP_FLT_C_H][15] = com_if_filter_hor_4_w16_sse256;
    g_funs_handle.ip_flt_c[IP_FLT_C_V][ 7] = com_if_filter_ver_4_w16_sse256;
    g_funs_handle.ip_flt_c[IP_FLT_C_V][15] = com_if_filter_ver_4_w32_sse256;
    g_funs_handle.ip_flt_c_ext[ 7] = com_if_filter_hor_ver_4_w16_sse256;
    g_funs_handle.ip_flt_c_ext[15] = com_if_filter_hor_ver_4_w16_sse256;

    g_funs_handle.avg_pel[16] = avg_pel_x16_sse256;
    g_funs_handle.avg_pel[32] = avg_pel_x16_sse256;
    g_funs_handle.avg_pel_1d = avg_pel_1d_sse256;

    g_funs_handle.quant = quant_normal_sse256;
    g_funs_handle.quant_ext = quant_ext_sse256;
    g_funs_handle.pre_quant = pre_quant_sse256;
    g_funs_handle.add_sign = add_sign_sse256;
    g_funs_handle.inv_quant = inv_quant_normal_sse256;

#pragma warning(disable: 4127)  // conditional expression is constant

    g_funs_handle.ip_flt_y_hor = com_if_filter_hor_8_sse256;
    g_funs_handle.ip_flt_y_ver = com_if_filter_ver_8_sse256;
    g_funs_handle.ip_flt_y_ver_ext = com_if_filter_ver_8_ext_sse256;

    g_funs_handle.cpy_pel[16] = com_mem_cpy32_sse256;
    g_funs_handle.cpy_pel[32] = com_mem_cpy64_sse256;

	g_funs_handle.add_inv_trans[3] = add_inv_trans_16x16_sse256;
	g_funs_handle.add_inv_trans[4] = add_inv_trans_32x32_sse256;

	g_funs_handle.add_inv_trans_ext[4] = add_inv_trans_ext_32x32_sse256;

	g_funs_handle.sub_trans[3] = sub_trans_16x16_sse256;
	g_funs_handle.sub_trans[4] = sub_trans_32x32_sse256;

    g_funs_handle.sub_trans_ext[4] = sub_trans_ext_32x32_sse256;

    g_funs_handle.zero_cg_check[2] = zero_cg_check16_sse256;
    g_funs_handle.zero_cg_check[3] = zero_cg_check32_sse256;

    g_funs_handle.trans_dct8_dst7[0][1] = trans_dct8_pb4_avx2;
    g_funs_handle.trans_dct8_dst7[0][2] = trans_dct8_pb8_avx2;
    g_funs_handle.trans_dct8_dst7[0][3] = trans_dct8_pb16_avx2;
    g_funs_handle.trans_dct8_dst7[1][1] = trans_dst7_pb4_avx2;
    g_funs_handle.trans_dct8_dst7[1][2] = trans_dst7_pb8_avx2;
    g_funs_handle.trans_dct8_dst7[1][3] = trans_dst7_pb16_avx2;

    g_funs_handle.itrans_dct8_dst7[0][1] = uavs3e_itrans_dct8_pb4_avx2;
    g_funs_handle.itrans_dct8_dst7[0][2] = uavs3e_itrans_dct8_pb8_avx2;
    g_funs_handle.itrans_dct8_dst7[0][3] = uavs3e_itrans_dct8_pb16_avx2;
    g_funs_handle.itrans_dct8_dst7[1][1] = uavs3e_itrans_dst7_pb4_avx2;
    g_funs_handle.itrans_dct8_dst7[1][2] = uavs3e_itrans_dst7_pb8_avx2;
    g_funs_handle.itrans_dct8_dst7[1][3] = uavs3e_itrans_dst7_pb16_avx2;

    g_funs_handle.pix_sub[0] = pix_sub_b4_sse256;
    g_funs_handle.pix_sub[1] = pix_sub_b8_sse256;
    g_funs_handle.pix_sub[2] = pix_sub_b16_sse256;
    g_funs_handle.pix_sub[3] = pix_sub_b32_sse256;

    g_funs_handle.pix_add[0] = pix_add_b4_sse256;
    g_funs_handle.pix_add[1] = pix_add_b8_sse256;
    g_funs_handle.pix_add[2] = pix_add_b16_sse256;
    g_funs_handle.pix_add[3] = pix_add_b32_sse256;

    g_funs_handle.get_nz_num = get_nz_num_sse256;
    g_funs_handle.pre_quant_rdoq = pre_quant_rdoq_sse256;
}

void com_funs_init_intrinsic_functions_10bit()
{
    int i;
  
	g_funs_handle.cost_sad[ 1] = xGetSAD4_sse128_10bit; 
	g_funs_handle.cost_sad[ 2] = xGetSAD8_sse128_10bit;
	g_funs_handle.cost_sad[ 4] = xGetSAD16_sse128_10bit;
	g_funs_handle.cost_sad[ 8] = xGetSAD32_sse128_10bit;
	g_funs_handle.cost_sad[16] = xGetSAD64_sse128_10bit;

	g_funs_handle.cost_ssd[0] = xGetSSE4_sse128_10bit;
	g_funs_handle.cost_ssd[1] = xGetSSE8_sse128_10bit;
	g_funs_handle.cost_ssd[2] = xGetSSE16_sse128_10bit;
	g_funs_handle.cost_ssd[3] = xGetSSE32_sse128_10bit;
	g_funs_handle.cost_ssd[4] = xGetSSE64_sse128_10bit;
	g_funs_handle.cost_ssd_ext = xGetSSE_Ext_sse128_10bit;
	g_funs_handle.cost_ssd_psnr = xGetSSE_Psnr_sse128_10bit;

	g_funs_handle.cost_sad_x4[ 1] = xGetSAD4_x4_sse128_10bit;
	g_funs_handle.cost_sad_x4[ 2] = xGetSAD8_x4_sse128_10bit;
	g_funs_handle.cost_sad_x4[ 4] = xGetSAD16_x4_sse128_10bit;
	g_funs_handle.cost_sad_x4[ 8] = xGetSAD32_x4_sse128_10bit;
	g_funs_handle.cost_sad_x4[16] = xGetSAD64_x4_sse128_10bit;

	g_funs_handle.cost_avg_sad[ 1] = xGetAVGSAD4_sse128_10bit;
	g_funs_handle.cost_avg_sad[ 2] = xGetAVGSAD8_sse128_10bit;
	g_funs_handle.cost_avg_sad[ 4] = xGetAVGSAD16_sse128_10bit;
	g_funs_handle.cost_avg_sad[ 8] = xGetAVGSAD32_sse128_10bit;
	g_funs_handle.cost_avg_sad[16] = xGetAVGSAD64_sse128_10bit;

	g_funs_handle.cost_blk_satd[0] = xCalcHAD4x4_sse128_10bit;
	g_funs_handle.cost_blk_satd[1] = xCalcHAD8x8_sse128_10bit;
    
    g_funs_handle.cost_satd_i = xCalcHADs8x8_I_sse128_10bit;

    for (i = 0; i < 33; i++) {
        g_funs_handle.avg_pel[i] = avg_pel_0_sse128_10bit;
    }
	g_funs_handle.avg_pel[ 2] = avg_pel_4_sse128_10bit;
	g_funs_handle.avg_pel[ 4] = avg_pel_8_sse128_10bit;
	g_funs_handle.avg_pel[ 8] = avg_pel_16_sse128_10bit;
	g_funs_handle.avg_pel[16] = avg_pel_x16_sse128_10bit;
	g_funs_handle.avg_pel[32] = avg_pel_x16_sse128_10bit;
	g_funs_handle.avg_pel_1d = avg_pel_1d_sse128_10bit;

    g_funs_handle.cpy_pel[ 4] = com_mem_cpy8_sse128_10bit;
    g_funs_handle.cpy_pel[ 8] = com_mem_cpy16_sse128_10bit;
    g_funs_handle.cpy_pel[16] = com_mem_cpy32_sse128_10bit;
    g_funs_handle.cpy_pel[32] = com_mem_cpy64_sse128_10bit;

    g_funs_handle.padding_rows = padding_rows_sse128_10bit;

	g_funs_handle.intra_pred_ang_x[3 - ANG_X_OFFSET] = xPredIntraAngAdi_X_3_sse128_10bit;
    g_funs_handle.intra_pred_ang_x[4 - ANG_X_OFFSET] = xPredIntraAngAdi_X_4_sse128_10bit;
	g_funs_handle.intra_pred_ang_x[5 - ANG_X_OFFSET] = xPredIntraAngAdi_X_5_sse128_10bit;
	g_funs_handle.intra_pred_ang_x[6 - ANG_X_OFFSET] = xPredIntraAngAdi_X_6_sse128_10bit;
	g_funs_handle.intra_pred_ang_x[7 - ANG_X_OFFSET] = xPredIntraAngAdi_X_7_sse128_10bit;
	g_funs_handle.intra_pred_ang_x[8 - ANG_X_OFFSET] = xPredIntraAngAdi_X_8_sse128_10bit;
    g_funs_handle.intra_pred_ang_x[9 - ANG_X_OFFSET] = xPredIntraAngAdi_X_9_sse128_10bit;
	g_funs_handle.intra_pred_ang_x[10 - ANG_X_OFFSET] = xPredIntraAngAdi_X_10_sse128_10bit;
    g_funs_handle.intra_pred_ang_x[11 - ANG_X_OFFSET] = xPredIntraAngAdi_X_11_sse128_10bit;

	g_funs_handle.intra_pred_ang_xy[14 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_14_sse128_10bit;
	g_funs_handle.intra_pred_ang_xy[16 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_16_sse128_10bit;
    g_funs_handle.intra_pred_ang_xy[18 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_18_sse128_10bit;
    g_funs_handle.intra_pred_ang_xy[20 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_20_sse128_10bit;
    g_funs_handle.intra_pred_ang_xy[22 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_22_sse128_10bit;

	g_funs_handle.intra_pred_ang_y[25 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_25_sse128_10bit;
	g_funs_handle.intra_pred_ang_y[26 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_26_sse128_10bit;
    g_funs_handle.intra_pred_ang_y[27 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_27_sse128_10bit;
    g_funs_handle.intra_pred_ang_y[28 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_28_sse128_10bit;
    g_funs_handle.intra_pred_ang_y[29 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_29_sse128_10bit;
	g_funs_handle.intra_pred_ang_y[30 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_30_sse128_10bit;
    g_funs_handle.intra_pred_ang_y[31 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_31_sse128_10bit;
    g_funs_handle.intra_pred_ang_y[32 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_32_sse128_10bit;

    g_funs_handle.intra_pred_plane = xPredIntraPlaneAdi_sse128_10bit;
    g_funs_handle.intra_pred_hor   = xPredIntraHorAdi_sse128_10bit;
    g_funs_handle.intra_pred_ver   = xPredIntraVertAdi_sse128_10bit;
    g_funs_handle.intra_pred_dc    = xPredIntraDCAdi_sse128_10bit;
    g_funs_handle.intra_pred_bi    = xPredIntraBiAdi_sse128_10bit;

    for (i = 0; i < 16; i++) {
        g_funs_handle.ip_flt_c[IP_FLT_C_H][i] = com_if_filter_hor_4_sse128_10bit;
		g_funs_handle.ip_flt_c[IP_FLT_C_V][i] = com_if_filter_ver_4_sse128_10bit;
        g_funs_handle.ip_flt_c_ext[i] = com_if_filter_hor_ver_4_sse128_10bit;
    }

    g_funs_handle.ip_flt_y_hor = com_if_filter_hor_8_sse128_10bit;
    g_funs_handle.ip_flt_y_ver = com_if_filter_ver_8_sse128_10bit;
	g_funs_handle.ip_flt_y_ver_ext = com_if_filter_ver_8_ext_sse128_10bit;

    
    g_funs_handle.quant_ext = quant_ext_sse128;
    g_funs_handle.quant = quant_normal_sse128;
    g_funs_handle.inv_quant = inv_quant_normal_sse128;
    g_funs_handle.inv_quant_ext = inv_quant_ext_sse128;
    g_funs_handle.add_sign = add_sign_sse128;
    g_funs_handle.pre_quant = pre_quant_sse128;

    g_funs_handle.deblk_luma[0] = deblock_luma_ver_sse128_10bit;
    g_funs_handle.deblk_luma[1] = deblock_luma_hor_sse128_10bit;
    g_funs_handle.deblk_chroma[0] = deblock_chroma_ver_sse128_10bit;
    g_funs_handle.deblk_chroma[1] = deblock_chroma_hor_sse128_10bit;
#ifdef _WIN32
    g_funs_handle.sao_flt[0] = sao_get_stats_sse128_10bit;
#endif
    g_funs_handle.sao_flt[1] = SAO_on_block_sse128_10bit;

	g_funs_handle.add_inv_trans[0] = add_inv_trans_4x4_sec_10bit_sse128;
	g_funs_handle.add_inv_trans[1] = add_inv_trans_4x4_10bit_sse128;
	g_funs_handle.add_inv_trans[2] = add_inv_trans_8x8_10bit_sse128;
	g_funs_handle.add_inv_trans[3] = add_inv_trans_16x16_10bit_sse128;
	g_funs_handle.add_inv_trans[4] = add_inv_trans_32x32_10bit_sse128;

    g_funs_handle.inv_trans_2nd_Hor = xTr2nd_8_1d_Inv_Hor_sse128;
    g_funs_handle.inv_trans_2nd_Ver = xTr2nd_8_1d_Inv_Vert_sse128;

    g_funs_handle.sub_trans[0] = sub_trans_4x4_sec_10bit_sse128;
    g_funs_handle.sub_trans[1] = sub_trans_4x4_10bit_sse128;
    g_funs_handle.sub_trans[2] = sub_trans_8x8_10bit_sse128;
    g_funs_handle.sub_trans[3] = sub_trans_16x16_10bit_sse128;
    g_funs_handle.sub_trans[4] = sub_trans_32x32_10bit_sse128;

    g_funs_handle.add_inv_trans_ext[4] = add_inv_trans_ext_32x32_10bit_sse128;
    g_funs_handle.sub_trans_ext[4] = sub_trans_ext_32x32_10bit_sse128;

    g_funs_handle.trans_2nd_Hor = xTr2nd_8_1d_Hor_sse128;
    g_funs_handle.trans_2nd_Ver = xTr2nd_8_1d_Vert_sse128;

    g_funs_handle.calc_blk_ave = calc_ave_sse128_10bit;
    g_funs_handle.calc_blk_var = calc_var_sse128_10bit;

    g_funs_handle.get_cg_bits = get_cg_bits_sse128;
    g_funs_handle.zero_cg_check[1] = zero_cg_check8_sse128;
}

void com_funs_init_intrinsic_functions_avx2_10bit()
{
    g_funs_handle.cost_sad[ 4] = xGetSAD16_sse256_10bit;
    g_funs_handle.cost_sad[ 8] = xGetSAD32_sse256_10bit;
    g_funs_handle.cost_sad[16] = xGetSAD64_sse256_10bit;

    g_funs_handle.cost_avg_sad[ 4] = xGetAVGSAD16_sse256_10bit;
    g_funs_handle.cost_avg_sad[ 8] = xGetAVGSAD32_sse256_10bit;
    g_funs_handle.cost_avg_sad[16] = xGetAVGSAD64_sse256_10bit;

    g_funs_handle.cost_sad_x4[ 4] = xGetSAD16_x4_sse256_10bit;
    g_funs_handle.cost_sad_x4[ 8] = xGetSAD32_x4_sse256_10bit;
    g_funs_handle.cost_sad_x4[16] = xGetSAD64_x4_sse256_10bit;

    g_funs_handle.cost_ssd[2] = xGetSSE16_sse256_10bit;
    g_funs_handle.cost_ssd[3] = xGetSSE32_sse256_10bit;
    g_funs_handle.cost_ssd[4] = xGetSSE64_sse256_10bit;

    g_funs_handle.cost_blk_satd[2] = xCalcHAD16x16_sse256_10bit;
    g_funs_handle.cost_blk_satd[3] = xCalcHAD32x32_sse256_10bit;
    g_funs_handle.cost_blk_satd[4] = xCalcHAD64x64_sse256_10bit;

    g_funs_handle.ip_flt_c[IP_FLT_C_H][ 3] = com_if_filter_hor_4_w8_sse256_10bit;
    g_funs_handle.ip_flt_c[IP_FLT_C_H][ 7] = com_if_filter_hor_4_w8_sse256_10bit;
    g_funs_handle.ip_flt_c[IP_FLT_C_H][15] = com_if_filter_hor_4_w8_sse256_10bit;
    g_funs_handle.ip_flt_c[IP_FLT_C_V][ 7] = com_if_filter_ver_4_w16_sse256_10bit;
    g_funs_handle.ip_flt_c[IP_FLT_C_V][15] = com_if_filter_ver_4_w32_sse256_10bit;
    g_funs_handle.ip_flt_c_ext[7] = com_if_filter_hor_ver_4_w16_sse256_10bit;
    g_funs_handle.ip_flt_c_ext[15] = com_if_filter_hor_ver_4_w16_sse256_10bit;

    g_funs_handle.avg_pel[ 8] = avg_pel_16_sse256_10bit;
    g_funs_handle.avg_pel[16] = avg_pel_x16_sse256_10bit;
    g_funs_handle.avg_pel[32] = avg_pel_x16_sse256_10bit;
    g_funs_handle.avg_pel_1d  = avg_pel_1d_sse256_10bit;

    g_funs_handle.quant = quant_normal_sse256;
    g_funs_handle.quant_ext = quant_ext_sse256;
    g_funs_handle.pre_quant = pre_quant_sse256;
    g_funs_handle.add_sign = add_sign_sse256;
    g_funs_handle.inv_quant = inv_quant_normal_sse256;

#pragma warning(disable: 4127)  // conditional expression is constant

    g_funs_handle.ip_flt_y_hor = com_if_filter_hor_8_sse256_10bit;
    g_funs_handle.ip_flt_y_ver = com_if_filter_ver_8_sse256_10bit;
    g_funs_handle.ip_flt_y_ver_ext = com_if_filter_ver_8_ext_sse256_10bit;

    g_funs_handle.cpy_pel[ 8] = com_mem_cpy16_sse256_10bit;
    g_funs_handle.cpy_pel[16] = com_mem_cpy32_sse256_10bit;
    g_funs_handle.cpy_pel[32] = com_mem_cpy64_sse256_10bit;

    g_funs_handle.add_inv_trans[3] = add_inv_trans_16x16_10bit_sse256;
	g_funs_handle.add_inv_trans[4] = add_inv_trans_32x32_10bit_sse256;

	g_funs_handle.add_inv_trans_ext[4] = add_inv_trans_ext_32x32_10bit_sse256;

	g_funs_handle.sub_trans[3] = sub_trans_16x16_10bit_sse256;
	g_funs_handle.sub_trans[4] = sub_trans_32x32_10bit_sse256;

	g_funs_handle.sub_trans_ext[4] = sub_trans_ext_32x32_10bit_sse256;

    g_funs_handle.trans_dct8_dst7[0][1] = trans_dct8_pb4_avx2;
    g_funs_handle.trans_dct8_dst7[0][2] = trans_dct8_pb8_avx2;
    g_funs_handle.trans_dct8_dst7[0][3] = trans_dct8_pb16_avx2;
    g_funs_handle.trans_dct8_dst7[1][1] = trans_dst7_pb4_avx2;
    g_funs_handle.trans_dct8_dst7[1][2] = trans_dst7_pb8_avx2;
    g_funs_handle.trans_dct8_dst7[1][3] = trans_dst7_pb16_avx2;

    g_funs_handle.itrans_dct8_dst7[0][1] = uavs3e_itrans_dct8_pb4_avx2;
    g_funs_handle.itrans_dct8_dst7[0][2] = uavs3e_itrans_dct8_pb8_avx2;
    g_funs_handle.itrans_dct8_dst7[0][3] = uavs3e_itrans_dct8_pb16_avx2;
    g_funs_handle.itrans_dct8_dst7[1][1] = uavs3e_itrans_dst7_pb4_avx2;
    g_funs_handle.itrans_dct8_dst7[1][2] = uavs3e_itrans_dst7_pb8_avx2;
    g_funs_handle.itrans_dct8_dst7[1][3] = uavs3e_itrans_dst7_pb16_avx2;

    g_funs_handle.zero_cg_check[2] = zero_cg_check16_sse256;
    g_funs_handle.zero_cg_check[3] = zero_cg_check32_sse256;

    g_funs_handle.pix_sub[0] = pix_sub_b4_10bit_sse256;
    g_funs_handle.pix_sub[1] = pix_sub_b8_10bit_sse256;
    g_funs_handle.pix_sub[2] = pix_sub_b16_10bit_sse256;
    g_funs_handle.pix_sub[3] = pix_sub_b32_10bit_sse256;
    
    g_funs_handle.pix_add[0] = pix_add_b4_10bit_sse256;
    g_funs_handle.pix_add[1] = pix_add_b8_10bit_sse256;
    g_funs_handle.pix_add[2] = pix_add_b16_10bit_sse256;
    g_funs_handle.pix_add[3] = pix_add_b32_10bit_sse256;

    g_funs_handle.get_nz_num = get_nz_num_sse256;
    g_funs_handle.pre_quant_rdoq = pre_quant_rdoq_sse256;
}

void com_funs_init_intrinsic_functions_avx512_10bit()
{
#if ENABLE_AVX512
    g_funs_handle.cost_sad[8] = xGetSAD32_sse512_10bit;
    g_funs_handle.cost_sad_x4[8] = xGetSAD32_x4_sse512_10bit;

    g_funs_handle.cost_blk_satd[2] = xCalcHAD16x16_sse512_10bit;
    g_funs_handle.cost_blk_satd[3] = xCalcHAD32x32_sse512_10bit;

    g_funs_handle.ip_flt_y_ver_ext = com_if_filter_ver_8_ext_sse512_10bit;

    g_funs_handle.avg_pel[ 8] = avg_pel_16_sse512_10bit;
    g_funs_handle.avg_pel[16] = avg_pel_32_sse512_10bit;

    g_funs_handle.quant = quant_normal_sse512;
    g_funs_handle.inv_quant = inv_quant_normal_sse512;
#endif
}