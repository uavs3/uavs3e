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

#define _mm_subabs_epu16(a, b) _mm_abs_epi16(_mm_subs_epi16(a, b))

#if (BIT_DEPTH == 8)
void uavs3e_deblock_ver_luma_sse(pel *src, int stride, int alpha, int beta, int flt_flag)
{
    pel *p_src = src - 4;
    int flag0 = -(flt_flag & 1);
    int flag1 = -((flt_flag >> 8) & 1);
    __m128i TL0, TL1, TL2, TL3;
    __m128i TR0, TR1, TR2, TR3;
    __m128i TL0w, TL1w, TL2w;
    __m128i TR0w, TR1w, TR2w;
    __m128i V0, V1, V2, V3;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2;
    __m128i FLT_L, FLT_R, FLT, FS;
    __m128i FS3, FS4, FS5, FS6;

    __m128i ALPHA = _mm_set1_epi16((short)alpha);
    __m128i BETA = _mm_set1_epi16((short)beta);
    __m128i c_0 = _mm_set1_epi16(0);
    __m128i c_1 = _mm_set1_epi16(1);
    __m128i c_2 = _mm_set1_epi16(2);
    __m128i c_3 = _mm_set1_epi16(3);
    __m128i c_4 = _mm_set1_epi16(4);
    __m128i c_5 = _mm_set1_epi16(5);
    __m128i c_6 = _mm_set1_epi16(6);
    __m128i c_8 = _mm_set1_epi16(8);

    T0 = _mm_loadl_epi64((__m128i *)(p_src));
    T1 = _mm_loadl_epi64((__m128i *)(p_src + stride));
    T2 = _mm_loadl_epi64((__m128i *)(p_src + stride * 2));
    T3 = _mm_loadl_epi64((__m128i *)(p_src + stride * 3));
    T4 = _mm_loadl_epi64((__m128i *)(p_src + stride * 4));
    T5 = _mm_loadl_epi64((__m128i *)(p_src + stride * 5));
    T6 = _mm_loadl_epi64((__m128i *)(p_src + stride * 6));
    T7 = _mm_loadl_epi64((__m128i *)(p_src + stride * 7));

    T0 = _mm_unpacklo_epi8(T0, T1);
    T1 = _mm_unpacklo_epi8(T2, T3);
    T2 = _mm_unpacklo_epi8(T4, T5);
    T3 = _mm_unpacklo_epi8(T6, T7);

    T4 = _mm_unpacklo_epi16(T0, T1);
    T5 = _mm_unpacklo_epi16(T2, T3);
    T6 = _mm_unpackhi_epi16(T0, T1);
    T7 = _mm_unpackhi_epi16(T2, T3);

    T0 = _mm_unpacklo_epi32(T4, T5);
    T1 = _mm_unpackhi_epi32(T4, T5);
    T2 = _mm_unpacklo_epi32(T6, T7);
    T3 = _mm_unpackhi_epi32(T6, T7);

    TL3 = _mm_unpacklo_epi8(T0, c_0);
    TL2 = _mm_unpackhi_epi8(T0, c_0);
    TL1 = _mm_unpacklo_epi8(T1, c_0);
    TL0 = _mm_unpackhi_epi8(T1, c_0);

    TR0 = _mm_unpacklo_epi8(T2, c_0);
    TR1 = _mm_unpackhi_epi8(T2, c_0);
    TR2 = _mm_unpacklo_epi8(T3, c_0);
    TR3 = _mm_unpackhi_epi8(T3, c_0);

    M0 = _mm_set_epi32(flag1, flag1, flag0, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    FLT_L = _mm_and_si128(_mm_cmpgt_epi16(BETA, T0), c_2);
    FLT_R = _mm_and_si128(_mm_cmpgt_epi16(BETA, T1), c_2);

    T4 = _mm_subabs_epu16(TL2, TL0);
    T5 = _mm_subabs_epu16(TR2, TR0);
    M1 = _mm_cmpgt_epi16(BETA, T4);
    M2 = _mm_cmpgt_epi16(BETA, T5);

    T6 = _mm_subabs_epu16(TL0, TR0); // AbsDelta
    T2 = _mm_cmpgt_epi16(ALPHA, T6); // AbsDelta < alpha
    T3 = _mm_srai_epi16(BETA, 2);    // Beta / 4

    FLT_L = _mm_add_epi16(_mm_and_si128(M1, c_1), FLT_L);
    FLT_R = _mm_add_epi16(_mm_and_si128(M2, c_1), FLT_R);
    FLT = _mm_add_epi16(FLT_L, FLT_R);

    // !(COM_ABS(R0 - R1) <= Beta / 4 || COM_ABS(L0 - L1) <= Beta / 4)
    M1 = _mm_or_si128(_mm_cmpgt_epi16(T0, T3), _mm_cmpgt_epi16(T1, T3));
    M2 = _mm_and_si128(_mm_cmpeq_epi16(TR0, TR1), _mm_cmpeq_epi16(TL0, TL1));
    M1 = _mm_andnot_si128(M1, T2);

    T7 = _mm_subabs_epu16(TL1, TR1);
    FS6 = _mm_blendv_epi8(c_3, c_4, M1);
    FS5 = _mm_blendv_epi8(c_2, c_3, M2);
    FS4 = _mm_blendv_epi8(c_1, c_2, _mm_cmpeq_epi16(FLT_L, c_2));
    FS3 = _mm_blendv_epi8(c_0, c_1, _mm_cmpgt_epi16(BETA, T7));

    FS = _mm_blendv_epi8(c_0, FS6, _mm_cmpeq_epi16(FLT, c_6));
    FS = _mm_blendv_epi8(FS, FS5, _mm_cmpeq_epi16(FLT, c_5));
    FS = _mm_blendv_epi8(FS, FS4, _mm_cmpeq_epi16(FLT, c_4));
    FS = _mm_blendv_epi8(FS, FS3, _mm_cmpeq_epi16(FLT, c_3));

    FS = _mm_and_si128(FS, M0);

    TL0w = TL0;
    TL1w = TL1;
    TR0w = TR0;
    TR1w = TR1;

    /* fs == 1 */
    V0 = _mm_add_epi16(TL0w, TR0w);         // L0 + R0
    V1 = _mm_add_epi16(V0, c_2);            // L0 + R0 + 2
    V2 = _mm_slli_epi16(TL0w, 1);           // L0 << 1
    V3 = _mm_slli_epi16(TR0w, 1);

    T2 = _mm_cmpeq_epi16(FS, c_1);
    T0 = _mm_srli_epi16(_mm_add_epi16(V2, V1), 2);
    T1 = _mm_srli_epi16(_mm_add_epi16(V3, V1), 2);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 2 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 1) + (R0 << 1) + 4
    T0 = _mm_add_epi16(TL1w, TR0w);         // L1 + R0
    T1 = _mm_add_epi16(TR1w, TL0w);
    T2 = _mm_slli_epi16(TL1w, 1);           // L1 << 1
    T3 = _mm_slli_epi16(TR1w, 1);
    T0 = _mm_add_epi16(T0, V1);             // L1 + R0 + L0*2 + R0*2 + 4
    T1 = _mm_add_epi16(T1, V1);
    T4 = _mm_slli_epi16(TL0w, 3);           // L0*8
    T5 = _mm_slli_epi16(TR0w, 3);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_4);
    T5 = _mm_add_epi16(T5, c_4);

    T2 = _mm_cmpeq_epi16(FS, c_2);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 3 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 2) + (R0 << 2) + 8
    T0 = _mm_slli_epi16(TL1w, 2);           // L1 << 2
    T1 = _mm_slli_epi16(TR1w, 2);
    T4 = _mm_add_epi16(TL2, TR1w);          // L2 + R1
    T5 = _mm_add_epi16(TR2, TL1w);

    T0 = _mm_add_epi16(T0, V2);             // (L1 << 2) + (L0 << 1)
    T1 = _mm_add_epi16(T1, V3);
    T4 = _mm_add_epi16(T4, V1);
    T5 = _mm_add_epi16(T5, V1);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T6 = _mm_cmpeq_epi16(FS, c_3);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T6);
    TR0 = _mm_blendv_epi8(TR0, T1, T6);

    T2 = _mm_add_epi16(TL2, TR0w);          // L2 + R0
    T3 = _mm_add_epi16(TR2, TL0w);
    T4 = _mm_slli_epi16(TL2, 1);
    T5 = _mm_slli_epi16(TR2, 1);
    T0 = _mm_slli_epi16(TL1w, 3);
    T1 = _mm_slli_epi16(TR1w, 3);
    T2 = _mm_add_epi16(T2, T4);             // L2 + R0 + L2*2
    T3 = _mm_add_epi16(T3, T5);
    T4 = _mm_slli_epi16(TL0w, 2);           // L0*4
    T5 = _mm_slli_epi16(TR0w, 2);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_8);
    T5 = _mm_add_epi16(T5, c_8);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL1 = _mm_blendv_epi8(TL1, T0, T6);
    TR1 = _mm_blendv_epi8(TR1, T1, T6);

    FS = _mm_cmpeq_epi16(FS, c_4);

    if (!_mm_testz_si128(FS, _mm_set1_epi16(-1))) { /* fs == 4 */
        TL2w = TL2;
        TR2w = TR2;

        /* cal L0/R0 */
        V1 = _mm_slli_epi16(V1, 1);         // (L0 << 3) + (R0 << 3) + 16
        T0 = _mm_slli_epi16(TL1w, 3);       // L1 << 3
        T1 = _mm_slli_epi16(TR1w, 3);
        T2 = _mm_add_epi16(TL2w, TR1w);     // L2 + R1
        T3 = _mm_add_epi16(TR2w, TL1w);
        T0 = _mm_add_epi16(T0, V2);         // L0*2 + L1*8
        T1 = _mm_add_epi16(T1, V3);
        T4 = _mm_slli_epi16(T2, 1);
        T5 = _mm_slli_epi16(T3, 1);
        T0 = _mm_add_epi16(T0, V1);
        T1 = _mm_add_epi16(T1, V1);
        T2 = _mm_add_epi16(T2, T4);         // (L2 + R1) * 3
        T3 = _mm_add_epi16(T3, T5);

        T0 = _mm_add_epi16(T0, T2);
        T1 = _mm_add_epi16(T1, T3);
        T0 = _mm_srli_epi16(T0, 5);
        T1 = _mm_srli_epi16(T1, 5);

        TL0 = _mm_blendv_epi8(TL0, T0, FS);
        TR0 = _mm_blendv_epi8(TR0, T1, FS);

        /* cal L1/R1 */
        T4 = _mm_add_epi16(TL2w, TL1w);      // L2 + L1
        T5 = _mm_add_epi16(TR2w, TR1w);
        T2 = _mm_add_epi16(TR0w, TL1w);      // L1 + R0
        T3 = _mm_add_epi16(TL0w, TR1w);
        T0 = _mm_add_epi16(T4, TL0w);        // L0 + L1 + L2
        T1 = _mm_add_epi16(T5, TR0w);
        T2 = _mm_add_epi16(T2, V3);          // L1 + R0*3
        T3 = _mm_add_epi16(T3, V2);
        T0 = _mm_add_epi16(_mm_slli_epi16(T0, 2), T2);
        T1 = _mm_add_epi16(_mm_slli_epi16(T1, 2), T3);
        T0 = _mm_add_epi16(T0, c_8);
        T1 = _mm_add_epi16(T1, c_8);
        T0 = _mm_srli_epi16(T0, 4);
        T1 = _mm_srli_epi16(T1, 4);

        TL1 = _mm_blendv_epi8(TL1, T0, FS);
        TR1 = _mm_blendv_epi8(TR1, T1, FS);

        /* cal L2/R2 */
        T2 = _mm_add_epi16(T4, TL3);        // L1 + L2 + L3
        T3 = _mm_add_epi16(T5, TR3);
        T0 = _mm_add_epi16(V0, c_4);
        T2 = _mm_slli_epi16(T2, 1);
        T3 = _mm_slli_epi16(T3, 1);
        T2 = _mm_add_epi16(T2, T0);
        T3 = _mm_add_epi16(T3, T0);
        T0 = _mm_srli_epi16(T2, 3);
        T1 = _mm_srli_epi16(T3, 3);

        TL2 = _mm_blendv_epi8(TL2, T0, FS);
        TR2 = _mm_blendv_epi8(TR2, T1, FS);
    }

    /* stroe result */
    T0 = _mm_packus_epi16(TL3, TR0);
    T1 = _mm_packus_epi16(TL2, TR1);
    T2 = _mm_packus_epi16(TL1, TR2);
    T3 = _mm_packus_epi16(TL0, TR3);

    T4 = _mm_unpacklo_epi8(T0, T1);
    T5 = _mm_unpacklo_epi8(T2, T3);
    T6 = _mm_unpackhi_epi8(T0, T1);
    T7 = _mm_unpackhi_epi8(T2, T3);

    V0 = _mm_unpacklo_epi16(T4, T5);
    V1 = _mm_unpacklo_epi16(T6, T7);
    V2 = _mm_unpackhi_epi16(T4, T5);
    V3 = _mm_unpackhi_epi16(T6, T7);

    T0 = _mm_unpacklo_epi32(V0, V1);
    T1 = _mm_unpackhi_epi32(V0, V1);
    T2 = _mm_unpacklo_epi32(V2, V3);
    T3 = _mm_unpackhi_epi32(V2, V3);

    p_src = src - 4;
    _mm_storel_epi64((__m128i *)(p_src), T0);
    p_src += stride;
    _mm_storel_epi64((__m128i *)(p_src), _mm_srli_si128(T0, 8));
    p_src += stride;
    _mm_storel_epi64((__m128i *)(p_src), T1);
    p_src += stride;
    _mm_storel_epi64((__m128i *)(p_src), _mm_srli_si128(T1, 8));
    p_src += stride;
    _mm_storel_epi64((__m128i *)(p_src), T2);
    p_src += stride;
    _mm_storel_epi64((__m128i *)(p_src), _mm_srli_si128(T2, 8));
    p_src += stride;
    _mm_storel_epi64((__m128i *)(p_src), T3);
    p_src += stride;
    _mm_storel_epi64((__m128i *)(p_src), _mm_srli_si128(T3, 8));
}

void uavs3e_deblock_ver_chroma_sse(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag)
{
    int flag0 = -((flt_flag >> 1) & 1);
    int flag1 = -((flt_flag >> 9) & 1);

    __m128i UVL1, UVR1;
    __m128i TL0, TL1, TL2, TL3;
    __m128i TR0, TR1, TR2, TR3;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i P0, P1, P2, P3, P4, P5, P6, P7;
    __m128i V0, V1, V2, V3;
    __m128i M0, M1, M2;
    __m128i FLT0, FLT1;
    __m128i ALPHAU = _mm_set1_epi16((short)alpha_u);
    __m128i ALPHAV = _mm_set1_epi16((short)alpha_v);
    __m128i BETAU = _mm_set1_epi16((short)beta_u);
    __m128i BETAV = _mm_set1_epi16((short)beta_v);
    __m128i ALPHA = _mm_unpacklo_epi64(ALPHAU, ALPHAV);
    __m128i BETA = _mm_unpacklo_epi64(BETAU, BETAV);
    __m128i c_0 = _mm_set1_epi16(0);
    __m128i c_8 = _mm_set1_epi16(8);

    src_u -= 4;
    src_v -= 4;
    T0 = _mm_loadl_epi64((__m128i *)(src_u));
    T1 = _mm_loadl_epi64((__m128i *)(src_u + stride));
    T2 = _mm_loadl_epi64((__m128i *)(src_u + stride * 2));
    T3 = _mm_loadl_epi64((__m128i *)(src_u + stride * 3));
    T4 = _mm_loadl_epi64((__m128i *)(src_v));
    T5 = _mm_loadl_epi64((__m128i *)(src_v + stride));
    T6 = _mm_loadl_epi64((__m128i *)(src_v + stride * 2));
    T7 = _mm_loadl_epi64((__m128i *)(src_v + stride * 3));

    P0 = _mm_unpacklo_epi8(T0, T1);
    P1 = _mm_unpacklo_epi8(T2, T3);
    P2 = _mm_unpacklo_epi8(T4, T5);
    P3 = _mm_unpacklo_epi8(T6, T7);
     
    P4 = _mm_unpacklo_epi16(P0, P1);    // u: 00 10 20 30 01 11 21 31 02 12 22 32 03 13 23 33
    P5 = _mm_unpackhi_epi16(P0, P1);
    P6 = _mm_unpacklo_epi16(P2, P3);
    P7 = _mm_unpackhi_epi16(P2, P3);

    T0 = _mm_unpacklo_epi32(P4, P6);    // u(00 10 20 30), v(00 10 20 30), u(01 11 21 31), v(01 11 21 31)
    T1 = _mm_unpackhi_epi32(P4, P6);
    T2 = _mm_unpacklo_epi32(P5, P7);
    T3 = _mm_unpackhi_epi32(P5, P7);

    TL3 = _mm_unpacklo_epi8(T0, c_0);
    TL2 = _mm_unpackhi_epi8(T0, c_0);
    TL1 = _mm_unpacklo_epi8(T1, c_0);
    TL0 = _mm_unpackhi_epi8(T1, c_0);

    TR0 = _mm_unpacklo_epi8(T2, c_0);
    TR1 = _mm_unpackhi_epi8(T2, c_0);
    TR2 = _mm_unpacklo_epi8(T3, c_0);
    TR3 = _mm_unpackhi_epi8(T3, c_0);

    M0 = _mm_set_epi32(flag1, flag0, flag1, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    T2 = _mm_cmpgt_epi16(BETA, T0);     // abs(L1 - L0) < beta
    T3 = _mm_cmpgt_epi16(BETA, T1);     // abs(R1 - R0) < beta

    V0 = _mm_subabs_epu16(TL2, TL0);
    V1 = _mm_subabs_epu16(TR2, TR0);
    V2 = _mm_subabs_epu16(TL0, TR0);    // abs(L0 - R0)
    M1 = _mm_cmpgt_epi16(BETA, V0);     // abs(L2 - L0) < beta
    M2 = _mm_cmpgt_epi16(BETA, V1);     // abs(R2 - R0) < beta

    V0 = _mm_cmpgt_epi16(ALPHA, V2);    // abs(L0 - R0) < alpha
    V1 = _mm_srai_epi16(BETA, 2);       // Beta / 4

    FLT0 = _mm_and_si128(T2, T3);       // abs(L1 - L0) < beta && abs(R1 - R0) < beta
    FLT0 = _mm_and_si128(FLT0, M0);

    // abs(L1 - L0) > Beta / 4 || abs(R1 - R0) > Beta / 4
    FLT1 = _mm_or_si128(_mm_cmpgt_epi16(T0, V1), _mm_cmpgt_epi16(T1, V1));
    M1   = _mm_and_si128(M1, M2);       // abs(L2 - L0) < beta && abs(R2 - R0) < beta
    FLT1 = _mm_andnot_si128(FLT1, V0);  // abs(L1 - L0) <= Beta / 4 && abs(R1 - R0) <= Beta / 4 && abs(L0 - R0) < alpha
    M1   = _mm_and_si128(FLT0, M1);
    FLT1 = _mm_and_si128(FLT1, M1);

    UVL1 = TL1;
    UVR1 = TR1;

    /* fs == 4 */
    V2 = _mm_slli_epi16(TR0, 1);        // R0 * 2
    V3 = _mm_slli_epi16(TL0, 1);        // L0 * 2
    T0 = _mm_add_epi16(TL0, TL2);       // L0 + L2
    T1 = _mm_add_epi16(TR0, TR2);
    T2 = _mm_slli_epi16(TL1, 3);        //
    T3 = _mm_slli_epi16(TR1, 3);
    V0 = _mm_slli_epi16(T0, 1);
    V1 = _mm_slli_epi16(T1, 1);
    T2 = _mm_add_epi16(T2, T0);         // L1*8 + L0+L2
    T3 = _mm_add_epi16(T3, T1);
    V0 = _mm_add_epi16(V0, V2);         // (L0+L2)*2 + R0*2
    V1 = _mm_add_epi16(V1, V3);
    T2 = _mm_add_epi16(T2, c_8);
    T3 = _mm_add_epi16(T3, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL1 = _mm_blendv_epi8(TL1, V0, FLT1);
    TR1 = _mm_blendv_epi8(TR1, V1, FLT1);

    /* fs == 2 */
    T0 = _mm_add_epi16(UVL1, TR0);      // L1 + R0
    T1 = _mm_add_epi16(UVR1, TL0);
    T2 = _mm_slli_epi16(TL0, 3);        // L0 * 8
    T3 = _mm_slli_epi16(TR0, 3);
    V0 = _mm_slli_epi16(T0, 1);         // L1*2 + R0*2
    V1 = _mm_slli_epi16(T1, 1);
    V3 = _mm_add_epi16(T2, V3);         // L0 * 10
    V2 = _mm_add_epi16(T3, V2);         // R0 * 10
    V0 = _mm_add_epi16(V0, T0);         // L1*3 + R0*3
    V1 = _mm_add_epi16(V1, T1);
    T2 = _mm_add_epi16(V3, c_8);
    T3 = _mm_add_epi16(V2, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL0 = _mm_blendv_epi8(TL0, V0, FLT0);
    TR0 = _mm_blendv_epi8(TR0, V1, FLT0);

    /* stroe result */
    T0 = _mm_packus_epi16(TL3, TR0);
    T1 = _mm_packus_epi16(TL2, TR1);
    T2 = _mm_packus_epi16(TL1, TR2);
    T3 = _mm_packus_epi16(TL0, TR3);

    P0 = _mm_unpacklo_epi8(T0, T1);
    P1 = _mm_unpacklo_epi8(T2, T3);
    P2 = _mm_unpackhi_epi8(T0, T1);
    P3 = _mm_unpackhi_epi8(T2, T3);

    P4 = _mm_unpacklo_epi16(P0, P1);
    P5 = _mm_unpacklo_epi16(P2, P3);
    P6 = _mm_unpackhi_epi16(P0, P1);
    P7 = _mm_unpackhi_epi16(P2, P3);

    T0 = _mm_unpacklo_epi32(P4, P5);    // U: 0[0-7], 1[0-7]
    T1 = _mm_unpackhi_epi32(P4, P5);    // U: 2[0-7], 3[0-7]
    T2 = _mm_unpacklo_epi32(P6, P7);
    T3 = _mm_unpackhi_epi32(P6, P7);

    _mm_storel_epi64((__m128i *)(src_u), T0);
    _mm_storel_epi64((__m128i *)(src_u + stride), _mm_srli_si128(T0, 8));
    _mm_storel_epi64((__m128i *)(src_u + (stride << 1)), T1);
    _mm_storel_epi64((__m128i *)(src_u + stride * 3), _mm_srli_si128(T1, 8));

    _mm_storel_epi64((__m128i *)(src_v), T2);
    _mm_storel_epi64((__m128i *)(src_v + stride), _mm_srli_si128(T2, 8));
    _mm_storel_epi64((__m128i *)(src_v + (stride << 1)), T3);
    _mm_storel_epi64((__m128i *)(src_v + stride * 3), _mm_srli_si128(T3, 8));
}

void uavs3e_deblock_hor_luma_sse(pel *src, int stride, int alpha, int beta, int flt_flag)
{
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int inc4 = inc << 2;
    int flag0 = -(flt_flag & 1);
    int flag1 = -((flt_flag >> 8) & 1);

    __m128i TL0, TL1, TL2, TL3;
    __m128i TR0, TR1, TR2, TR3;
    __m128i TL0w, TL1w, TL2w, TR0w, TR1w, TR2w; //for swap
    __m128i V0, V1, V2, V3;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2;
    __m128i FLT_L, FLT_R, FLT, FS;
    __m128i FS3, FS4, FS5, FS6;

    __m128i ALPHA = _mm_set1_epi16((short)alpha);
    __m128i BETA = _mm_set1_epi16((short)beta);
    __m128i c_0 = _mm_set1_epi16(0);
    __m128i c_1 = _mm_set1_epi16(1);
    __m128i c_2 = _mm_set1_epi16(2);
    __m128i c_3 = _mm_set1_epi16(3);
    __m128i c_4 = _mm_set1_epi16(4);
    __m128i c_5 = _mm_set1_epi16(5);
    __m128i c_6 = _mm_set1_epi16(6);
    __m128i c_8 = _mm_set1_epi16(8);

    TL3 = _mm_loadl_epi64((__m128i *)(src - inc4));
    TL2 = _mm_loadl_epi64((__m128i *)(src - inc3));
    TL1 = _mm_loadl_epi64((__m128i *)(src - inc2));
    TL0 = _mm_loadl_epi64((__m128i *)(src - inc));
    TR0 = _mm_loadl_epi64((__m128i *)(src + 0));
    TR1 = _mm_loadl_epi64((__m128i *)(src + inc));
    TR2 = _mm_loadl_epi64((__m128i *)(src + inc2));
    TR3 = _mm_loadl_epi64((__m128i *)(src + inc3));

    TL3 = _mm_unpacklo_epi8(TL3, c_0);
    TL2 = _mm_unpacklo_epi8(TL2, c_0);
    TL1 = _mm_unpacklo_epi8(TL1, c_0);
    TL0 = _mm_unpacklo_epi8(TL0, c_0);
    TR0 = _mm_unpacklo_epi8(TR0, c_0);
    TR1 = _mm_unpacklo_epi8(TR1, c_0);
    TR2 = _mm_unpacklo_epi8(TR2, c_0);
    TR3 = _mm_unpacklo_epi8(TR3, c_0);


    M0 = _mm_set_epi32(flag1, flag1, flag0, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    FLT_L = _mm_and_si128(_mm_cmpgt_epi16(BETA, T0), c_2);
    FLT_R = _mm_and_si128(_mm_cmpgt_epi16(BETA, T1), c_2);

    T4 = _mm_subabs_epu16(TL2, TL0);
    T5 = _mm_subabs_epu16(TR2, TR0);
    M1 = _mm_cmpgt_epi16(BETA, T4);
    M2 = _mm_cmpgt_epi16(BETA, T5);

    T6 = _mm_subabs_epu16(TL0, TR0); // AbsDelta
    T2 = _mm_cmpgt_epi16(ALPHA, T6); // AbsDelta < alpha
    T3 = _mm_srai_epi16(BETA, 2);    // Beta / 4

    FLT_L = _mm_add_epi16(_mm_and_si128(M1, c_1), FLT_L);
    FLT_R = _mm_add_epi16(_mm_and_si128(M2, c_1), FLT_R);
    FLT = _mm_add_epi16(FLT_L, FLT_R);

    // !(COM_ABS(R0 - R1) <= Beta / 4 || COM_ABS(L0 - L1) <= Beta / 4)
    M1 = _mm_or_si128(_mm_cmpgt_epi16(T0, T3), _mm_cmpgt_epi16(T1, T3));
    M2 = _mm_and_si128(_mm_cmpeq_epi16(TR0, TR1), _mm_cmpeq_epi16(TL0, TL1));
    M1 = _mm_andnot_si128(M1, T2);

    T7 = _mm_subabs_epu16(TL1, TR1);
    FS6 = _mm_blendv_epi8(c_3, c_4, M1);
    FS5 = _mm_blendv_epi8(c_2, c_3, M2);
    FS4 = _mm_blendv_epi8(c_1, c_2, _mm_cmpeq_epi16(FLT_L, c_2));
    FS3 = _mm_blendv_epi8(c_0, c_1, _mm_cmpgt_epi16(BETA, T7));

    FS = _mm_blendv_epi8(c_0, FS6, _mm_cmpeq_epi16(FLT, c_6));
    FS = _mm_blendv_epi8(FS, FS5, _mm_cmpeq_epi16(FLT, c_5));
    FS = _mm_blendv_epi8(FS, FS4, _mm_cmpeq_epi16(FLT, c_4));
    FS = _mm_blendv_epi8(FS, FS3, _mm_cmpeq_epi16(FLT, c_3));

    FS = _mm_and_si128(FS, M0);

    TL0w = TL0;
    TL1w = TL1;
    TR0w = TR0;
    TR1w = TR1;

    /* fs == 1 */
    V0 = _mm_add_epi16(TL0w, TR0w);         // L0 + R0
    V1 = _mm_add_epi16(V0, c_2);            // L0 + R0 + 2
    V2 = _mm_slli_epi16(TL0w, 1);           // L0 << 1
    V3 = _mm_slli_epi16(TR0w, 1);

    T2 = _mm_cmpeq_epi16(FS, c_1);
    T0 = _mm_srli_epi16(_mm_add_epi16(V2, V1), 2);
    T1 = _mm_srli_epi16(_mm_add_epi16(V3, V1), 2);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 2 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 1) + (R0 << 1) + 4
    T0 = _mm_add_epi16(TL1w, TR0w);         // L1 + R0
    T1 = _mm_add_epi16(TR1w, TL0w);
    T2 = _mm_slli_epi16(TL1w, 1);           // L1 << 1
    T3 = _mm_slli_epi16(TR1w, 1);
    T0 = _mm_add_epi16(T0, V1);             // L1 + R0 + L0*2 + R0*2 + 4
    T1 = _mm_add_epi16(T1, V1);
    T4 = _mm_slli_epi16(TL0w, 3);           // L0*8
    T5 = _mm_slli_epi16(TR0w, 3);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_4);
    T5 = _mm_add_epi16(T5, c_4);

    T2 = _mm_cmpeq_epi16(FS, c_2);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 3 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 2) + (R0 << 2) + 8
    T0 = _mm_slli_epi16(TL1w, 2);           // L1 << 2
    T1 = _mm_slli_epi16(TR1w, 2);
    T4 = _mm_add_epi16(TL2, TR1w);          // L2 + R1
    T5 = _mm_add_epi16(TR2, TL1w);

    T0 = _mm_add_epi16(T0, V2);             // (L1 << 2) + (L0 << 1)
    T1 = _mm_add_epi16(T1, V3);
    T4 = _mm_add_epi16(T4, V1);
    T5 = _mm_add_epi16(T5, V1);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T6 = _mm_cmpeq_epi16(FS, c_3);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T6);
    TR0 = _mm_blendv_epi8(TR0, T1, T6);

    T2 = _mm_add_epi16(TL2, TR0w);          // L2 + R0
    T3 = _mm_add_epi16(TR2, TL0w);
    T4 = _mm_slli_epi16(TL2, 1);
    T5 = _mm_slli_epi16(TR2, 1);
    T0 = _mm_slli_epi16(TL1w, 3);
    T1 = _mm_slli_epi16(TR1w, 3);
    T2 = _mm_add_epi16(T2, T4);             // L2 + R0 + L2*2
    T3 = _mm_add_epi16(T3, T5);
    T4 = _mm_slli_epi16(TL0w, 2);           // L0*4
    T5 = _mm_slli_epi16(TR0w, 2);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_8);
    T5 = _mm_add_epi16(T5, c_8);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL1 = _mm_blendv_epi8(TL1, T0, T6);
    TR1 = _mm_blendv_epi8(TR1, T1, T6);

    FS = _mm_cmpeq_epi16(FS, c_4);

    if (!_mm_testz_si128(FS, _mm_set1_epi16(-1))) { /* fs == 4 */
        TL2w = TL2;
        TR2w = TR2;

        /* cal L0/R0 */
        V1 = _mm_slli_epi16(V1, 1);         // (L0 << 3) + (R0 << 3) + 16
        T0 = _mm_slli_epi16(TL1w, 3);       // L1 << 3
        T1 = _mm_slli_epi16(TR1w, 3);
        T2 = _mm_add_epi16(TL2w, TR1w);     // L2 + R1
        T3 = _mm_add_epi16(TR2w, TL1w);
        T0 = _mm_add_epi16(T0, V2);         // L0*2 + L1*8
        T1 = _mm_add_epi16(T1, V3);
        T4 = _mm_slli_epi16(T2, 1);
        T5 = _mm_slli_epi16(T3, 1);
        T0 = _mm_add_epi16(T0, V1);
        T1 = _mm_add_epi16(T1, V1);
        T2 = _mm_add_epi16(T2, T4);         // (L2 + R1) * 3
        T3 = _mm_add_epi16(T3, T5);

        T0 = _mm_add_epi16(T0, T2);
        T1 = _mm_add_epi16(T1, T3);
        T0 = _mm_srli_epi16(T0, 5);
        T1 = _mm_srli_epi16(T1, 5);

        TL0 = _mm_blendv_epi8(TL0, T0, FS);
        TR0 = _mm_blendv_epi8(TR0, T1, FS);

        /* cal L1/R1 */
        T4 = _mm_add_epi16(TL2w, TL1w);      // L2 + L1
        T5 = _mm_add_epi16(TR2w, TR1w);
        T2 = _mm_add_epi16(TR0w, TL1w);      // L1 + R0
        T3 = _mm_add_epi16(TL0w, TR1w);
        T0 = _mm_add_epi16(T4, TL0w);        // L0 + L1 + L2
        T1 = _mm_add_epi16(T5, TR0w);
        T2 = _mm_add_epi16(T2, V3);          // L1 + R0*3
        T3 = _mm_add_epi16(T3, V2);
        T0 = _mm_add_epi16(_mm_slli_epi16(T0, 2), T2);
        T1 = _mm_add_epi16(_mm_slli_epi16(T1, 2), T3);
        T0 = _mm_add_epi16(T0, c_8);
        T1 = _mm_add_epi16(T1, c_8);
        T0 = _mm_srli_epi16(T0, 4);
        T1 = _mm_srli_epi16(T1, 4);

        TL1 = _mm_blendv_epi8(TL1, T0, FS);
        TR1 = _mm_blendv_epi8(TR1, T1, FS);

        /* cal L2/R2 */
        T2 = _mm_add_epi16(T4, TL3);        // L1 + L2 + L3
        T3 = _mm_add_epi16(T5, TR3);
        T0 = _mm_add_epi16(V0, c_4);
        T2 = _mm_slli_epi16(T2, 1);
        T3 = _mm_slli_epi16(T3, 1);
        T2 = _mm_add_epi16(T2, T0);
        T3 = _mm_add_epi16(T3, T0);
        T0 = _mm_srli_epi16(T2, 3);
        T1 = _mm_srli_epi16(T3, 3);

        TL2 = _mm_blendv_epi8(TL2, T0, FS);
        TR2 = _mm_blendv_epi8(TR2, T1, FS);

        /* stroe result */
        _mm_storel_epi64((__m128i *)(src - inc), _mm_packus_epi16(TL0, c_0));
        _mm_storel_epi64((__m128i *)(src - 0), _mm_packus_epi16(TR0, c_0));

        _mm_storel_epi64((__m128i *)(src - inc2), _mm_packus_epi16(TL1, c_0));
        _mm_storel_epi64((__m128i *)(src + inc), _mm_packus_epi16(TR1, c_0));

        _mm_storel_epi64((__m128i *)(src - inc3), _mm_packus_epi16(TL2, c_0));
        _mm_storel_epi64((__m128i *)(src + inc2), _mm_packus_epi16(TR2, c_0));
    } else {
        /* stroe result */
        _mm_storel_epi64((__m128i *)(src - inc), _mm_packus_epi16(TL0, c_0));
        _mm_storel_epi64((__m128i *)(src - 0), _mm_packus_epi16(TR0, c_0));

        _mm_storel_epi64((__m128i *)(src - inc2), _mm_packus_epi16(TL1, c_0));
        _mm_storel_epi64((__m128i *)(src + inc), _mm_packus_epi16(TR1, c_0));
    }

}

void uavs3e_deblock_hor_chroma_sse(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag)
{
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int flag0 = -((flt_flag >> 1) & 1);
    int flag1 = -((flt_flag >> 9) & 1);

    __m128i UL1, UR1;
    __m128i TL0, TL1, TL2;
    __m128i TR0, TR1, TR2;
    __m128i T0, T1, T2, T3;
    __m128i V0, V1, V2, V3;
    __m128i M0, M1, M2;
    __m128i FLT0, FLT1;

    __m128i ALPHAU = _mm_set1_epi16((short)alpha_u);
    __m128i ALPHAV = _mm_set1_epi16((short)alpha_v);
    __m128i BETAU = _mm_set1_epi16((short)beta_u);
    __m128i BETAV = _mm_set1_epi16((short)beta_v);
    __m128i ALPHA = _mm_unpacklo_epi64(ALPHAU, ALPHAV);
    __m128i BETA = _mm_unpacklo_epi64(BETAU, BETAV);
    __m128i c_0 = _mm_set1_epi16(0);
    __m128i c_8 = _mm_set1_epi16(8);

    TL2 = _mm_set_epi32(0, 0, *(s32*)(src_v - inc3), *(s32*)(src_u - inc3));
    TL1 = _mm_set_epi32(0, 0, *(s32*)(src_v - inc2), *(s32*)(src_u - inc2));
    TL0 = _mm_set_epi32(0, 0, *(s32*)(src_v - inc), *(s32*)(src_u - inc));
    TR0 = _mm_set_epi32(0, 0, *(s32*)(src_v), *(s32*)(src_u));
    TR1 = _mm_set_epi32(0, 0, *(s32*)(src_v + inc), *(s32*)(src_u + inc));
    TR2 = _mm_set_epi32(0, 0, *(s32*)(src_v + inc2), *(s32*)(src_u + inc2));

    TL0 = _mm_unpacklo_epi8(TL0, c_0);
    TL1 = _mm_unpacklo_epi8(TL1, c_0);
    TL2 = _mm_unpacklo_epi8(TL2, c_0);
    TR0 = _mm_unpacklo_epi8(TR0, c_0);
    TR1 = _mm_unpacklo_epi8(TR1, c_0);
    TR2 = _mm_unpacklo_epi8(TR2, c_0);

    M0 = _mm_set_epi32(flag1, flag0, flag1, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    T2 = _mm_cmpgt_epi16(BETA, T0);     // abs(L1 - L0) < beta
    T3 = _mm_cmpgt_epi16(BETA, T1);     // abs(R1 - R0) < beta

    V0 = _mm_subabs_epu16(TL2, TL0);
    V1 = _mm_subabs_epu16(TR2, TR0);
    V2 = _mm_subabs_epu16(TL0, TR0);    // abs(L0 - R0)
    M1 = _mm_cmpgt_epi16(BETA, V0);     // abs(L2 - L0) < beta
    M2 = _mm_cmpgt_epi16(BETA, V1);     // abs(R2 - R0) < beta

    V0 = _mm_cmpgt_epi16(ALPHA, V2);    // abs(L0 - R0) < alpha
    V1 = _mm_srai_epi16(BETA, 2);       // Beta / 4

    FLT0 = _mm_and_si128(T2, T3);       // abs(L1 - L0) < beta && abs(R1 - R0) < beta
    FLT0 = _mm_and_si128(FLT0, M0);

    // abs(L1 - L0) > Beta / 4 || abs(R1 - R0) > Beta / 4
    FLT1 = _mm_or_si128(_mm_cmpgt_epi16(T0, V1), _mm_cmpgt_epi16(T1, V1));
    M1   = _mm_and_si128(M1, M2);       // abs(L2 - L0) < beta && abs(R2 - R0) < beta
    FLT1 = _mm_andnot_si128(FLT1, V0);  // abs(L1 - L0) <= Beta / 4 && abs(R1 - R0) <= Beta / 4 && abs(L0 - R0) < alpha
    M1   = _mm_and_si128(FLT0, M1);
    FLT1 = _mm_and_si128(FLT1, M1);

    UR1 = TR1;
    UL1 = TL1;

    /* fs == 4 */
    V2 = _mm_slli_epi16(TR0, 1);        // R0 * 2
    V3 = _mm_slli_epi16(TL0, 1);        // L0 * 2
    T0 = _mm_add_epi16(TL0, TL2);       // L0 + L2
    T1 = _mm_add_epi16(TR0, TR2);
    T2 = _mm_slli_epi16(TL1, 3);        //
    T3 = _mm_slli_epi16(TR1, 3);
    V0 = _mm_slli_epi16(T0, 1);
    V1 = _mm_slli_epi16(T1, 1);
    T2 = _mm_add_epi16(T2, T0);         // L1*8 + L0+L2
    T3 = _mm_add_epi16(T3, T1);
    V0 = _mm_add_epi16(V0, V2);         // (L0+L2)*2 + R0*2
    V1 = _mm_add_epi16(V1, V3);
    T2 = _mm_add_epi16(T2, c_8);
    T3 = _mm_add_epi16(T3, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL1 = _mm_blendv_epi8(TL1, V0, FLT1);
    TR1 = _mm_blendv_epi8(TR1, V1, FLT1);

    /* fs == 2 */
    T0 = _mm_add_epi16(UL1, TR0);       // L1 + R0
    T1 = _mm_add_epi16(UR1, TL0);
    T2 = _mm_slli_epi16(TL0, 3);        // L0 * 8
    T3 = _mm_slli_epi16(TR0, 3);
    V0 = _mm_slli_epi16(T0, 1);         // L1*2 + R0*2
    V1 = _mm_slli_epi16(T1, 1);
    V3 = _mm_add_epi16(T2, V3);         // L0 * 10
    V2 = _mm_add_epi16(T3, V2);         // R0 * 10
    V0 = _mm_add_epi16(V0, T0);         // L1*3 + R0*3
    V1 = _mm_add_epi16(V1, T1);
    T2 = _mm_add_epi16(V3, c_8);
    T3 = _mm_add_epi16(V2, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL0 = _mm_blendv_epi8(TL0, V0, FLT0);
    TR0 = _mm_blendv_epi8(TR0, V1, FLT0);

    /* stroe result */
    T0 = _mm_packus_epi16(TL1, TL0);
    T1 = _mm_packus_epi16(TR0, TR1);

    *(s32*)(src_u - inc2) = _mm_extract_epi32(T0, 0);
    *(s32*)(src_u - inc)  = _mm_extract_epi32(T0, 2);
    *(s32*)(src_u)        = _mm_extract_epi32(T1, 0);
    *(s32*)(src_u + inc)  = _mm_extract_epi32(T1, 2);
    *(s32*)(src_v - inc2) = _mm_extract_epi32(T0, 1);
    *(s32*)(src_v - inc)  = _mm_extract_epi32(T0, 3);
    *(s32*)(src_v)        = _mm_extract_epi32(T1, 1);
    *(s32*)(src_v + inc)  = _mm_extract_epi32(T1, 3);
}
#else
void uavs3e_deblock_ver_luma_sse(pel *src, int stride, int alpha, int beta, int flt_flag)
{
    pel *p_src = src - 4;
    int flag0 = -(flt_flag & 1);
    int flag1 = -((flt_flag >> 8) & 1);
    __m128i TL0, TL1, TL2, TL3;
    __m128i TR0, TR1, TR2, TR3;
    __m128i TL0w, TL1w, TL2w;
    __m128i TR0w, TR1w, TR2w;
    __m128i V0, V1, V2, V3;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2, M3, M4, M5, M6, M7;
    __m128i FLT_L, FLT_R, FLT, FS;
    __m128i FS3, FS4, FS5, FS6;

    __m128i ALPHA = _mm_set1_epi16((short)alpha);
    __m128i BETA = _mm_set1_epi16((short)beta);
    __m128i c_0 = _mm_set1_epi16(0);
    __m128i c_1 = _mm_set1_epi16(1);
    __m128i c_2 = _mm_set1_epi16(2);
    __m128i c_3 = _mm_set1_epi16(3);
    __m128i c_4 = _mm_set1_epi16(4);
    __m128i c_5 = _mm_set1_epi16(5);
    __m128i c_6 = _mm_set1_epi16(6);
    __m128i c_8 = _mm_set1_epi16(8);

    T0 = _mm_loadu_si128((__m128i *)(p_src));
    T1 = _mm_loadu_si128((__m128i *)(p_src + stride));
    T2 = _mm_loadu_si128((__m128i *)(p_src + stride * 2));
    T3 = _mm_loadu_si128((__m128i *)(p_src + stride * 3));
    T4 = _mm_loadu_si128((__m128i *)(p_src + stride * 4));
    T5 = _mm_loadu_si128((__m128i *)(p_src + stride * 5));
    T6 = _mm_loadu_si128((__m128i *)(p_src + stride * 6));
    T7 = _mm_loadu_si128((__m128i *)(p_src + stride * 7));

    M0 = _mm_unpacklo_epi16(T0, T1);
    M1 = _mm_unpackhi_epi16(T0, T1);
    M2 = _mm_unpacklo_epi16(T2, T3);
    M3 = _mm_unpackhi_epi16(T2, T3);
    M4 = _mm_unpacklo_epi16(T4, T5);
    M5 = _mm_unpackhi_epi16(T4, T5);
    M6 = _mm_unpacklo_epi16(T6, T7);
    M7 = _mm_unpackhi_epi16(T6, T7);

    T0 = _mm_unpacklo_epi32(M0, M2);
    T1 = _mm_unpackhi_epi32(M0, M2);
    T2 = _mm_unpacklo_epi32(M1, M3);
    T3 = _mm_unpackhi_epi32(M1, M3);
    T4 = _mm_unpacklo_epi32(M4, M6);
    T5 = _mm_unpackhi_epi32(M4, M6);
    T6 = _mm_unpacklo_epi32(M5, M7);
    T7 = _mm_unpackhi_epi32(M5, M7);

    TL3 = _mm_unpacklo_epi64(T0, T4);
    TL2 = _mm_unpackhi_epi64(T0, T4);
    TR0 = _mm_unpacklo_epi64(T2, T6);
    TR1 = _mm_unpackhi_epi64(T2, T6);
    TL1 = _mm_unpacklo_epi64(T1, T5);
    TL0 = _mm_unpackhi_epi64(T1, T5);
    TR2 = _mm_unpacklo_epi64(T3, T7);
    TR3 = _mm_unpackhi_epi64(T3, T7);

    M0 = _mm_set_epi32(flag1, flag1, flag0, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    FLT_L = _mm_and_si128(_mm_cmpgt_epi16(BETA, T0), c_2);
    FLT_R = _mm_and_si128(_mm_cmpgt_epi16(BETA, T1), c_2);

    T4 = _mm_subabs_epu16(TL2, TL0);
    T5 = _mm_subabs_epu16(TR2, TR0);
    M1 = _mm_cmpgt_epi16(BETA, T4);
    M2 = _mm_cmpgt_epi16(BETA, T5);

    T6 = _mm_subabs_epu16(TL0, TR0); // AbsDelta
    T2 = _mm_cmpgt_epi16(ALPHA, T6); // AbsDelta < alpha
    T3 = _mm_srai_epi16(BETA, 2);    // Beta / 4

    FLT_L = _mm_add_epi16(_mm_and_si128(M1, c_1), FLT_L);
    FLT_R = _mm_add_epi16(_mm_and_si128(M2, c_1), FLT_R);
    FLT = _mm_add_epi16(FLT_L, FLT_R);

    // !(COM_ABS(R0 - R1) <= Beta / 4 || COM_ABS(L0 - L1) <= Beta / 4)
    M1 = _mm_or_si128(_mm_cmpgt_epi16(T0, T3), _mm_cmpgt_epi16(T1, T3));
    M2 = _mm_and_si128(_mm_cmpeq_epi16(TR0, TR1), _mm_cmpeq_epi16(TL0, TL1));
    M1 = _mm_andnot_si128(M1, T2);

    T7 = _mm_subabs_epu16(TL1, TR1);
    FS6 = _mm_blendv_epi8(c_3, c_4, M1);
    FS5 = _mm_blendv_epi8(c_2, c_3, M2);
    FS4 = _mm_blendv_epi8(c_1, c_2, _mm_cmpeq_epi16(FLT_L, c_2));
    FS3 = _mm_blendv_epi8(c_0, c_1, _mm_cmpgt_epi16(BETA, T7));

    FS = _mm_blendv_epi8(c_0, FS6, _mm_cmpeq_epi16(FLT, c_6));
    FS = _mm_blendv_epi8(FS, FS5, _mm_cmpeq_epi16(FLT, c_5));
    FS = _mm_blendv_epi8(FS, FS4, _mm_cmpeq_epi16(FLT, c_4));
    FS = _mm_blendv_epi8(FS, FS3, _mm_cmpeq_epi16(FLT, c_3));

    FS = _mm_and_si128(FS, M0);

    TL0w = TL0;
    TL1w = TL1;
    TR0w = TR0;
    TR1w = TR1;

    /* fs == 1 */
    V0 = _mm_add_epi16(TL0w, TR0w);         // L0 + R0
    V1 = _mm_add_epi16(V0, c_2);            // L0 + R0 + 2
    V2 = _mm_slli_epi16(TL0w, 1);           // L0 << 1
    V3 = _mm_slli_epi16(TR0w, 1);

    T2 = _mm_cmpeq_epi16(FS, c_1);
    T0 = _mm_srli_epi16(_mm_add_epi16(V2, V1), 2);
    T1 = _mm_srli_epi16(_mm_add_epi16(V3, V1), 2);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 2 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 1) + (R0 << 1) + 4
    T0 = _mm_add_epi16(TL1w, TR0w);         // L1 + R0
    T1 = _mm_add_epi16(TR1w, TL0w);
    T2 = _mm_slli_epi16(TL1w, 1);           // L1 << 1
    T3 = _mm_slli_epi16(TR1w, 1);
    T0 = _mm_add_epi16(T0, V1);             // L1 + R0 + L0*2 + R0*2 + 4
    T1 = _mm_add_epi16(T1, V1);
    T4 = _mm_slli_epi16(TL0w, 3);           // L0*8
    T5 = _mm_slli_epi16(TR0w, 3);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_4);
    T5 = _mm_add_epi16(T5, c_4);

    T2 = _mm_cmpeq_epi16(FS, c_2);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 3 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 2) + (R0 << 2) + 8
    T0 = _mm_slli_epi16(TL1w, 2);           // L1 << 2
    T1 = _mm_slli_epi16(TR1w, 2);
    T4 = _mm_add_epi16(TL2, TR1w);          // L2 + R1
    T5 = _mm_add_epi16(TR2, TL1w);

    T0 = _mm_add_epi16(T0, V2);             // (L1 << 2) + (L0 << 1)
    T1 = _mm_add_epi16(T1, V3);
    T4 = _mm_add_epi16(T4, V1);
    T5 = _mm_add_epi16(T5, V1);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T6 = _mm_cmpeq_epi16(FS, c_3);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T6);
    TR0 = _mm_blendv_epi8(TR0, T1, T6);

    T2 = _mm_add_epi16(TL2, TR0w);          // L2 + R0
    T3 = _mm_add_epi16(TR2, TL0w);
    T4 = _mm_slli_epi16(TL2, 1);
    T5 = _mm_slli_epi16(TR2, 1);
    T0 = _mm_slli_epi16(TL1w, 3);
    T1 = _mm_slli_epi16(TR1w, 3);
    T2 = _mm_add_epi16(T2, T4);             // L2 + R0 + L2*2
    T3 = _mm_add_epi16(T3, T5);
    T4 = _mm_slli_epi16(TL0w, 2);           // L0*4
    T5 = _mm_slli_epi16(TR0w, 2);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_8);
    T5 = _mm_add_epi16(T5, c_8);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL1 = _mm_blendv_epi8(TL1, T0, T6);
    TR1 = _mm_blendv_epi8(TR1, T1, T6);

    FS = _mm_cmpeq_epi16(FS, c_4);

    if (!_mm_testz_si128(FS, _mm_set1_epi16(-1))) { /* fs == 4 */
        TL2w = TL2;
        TR2w = TR2;

        /* cal L0/R0 */
        V1 = _mm_slli_epi16(V1, 1);         // (L0 << 3) + (R0 << 3) + 16
        T0 = _mm_slli_epi16(TL1w, 3);       // L1 << 3
        T1 = _mm_slli_epi16(TR1w, 3);
        T2 = _mm_add_epi16(TL2w, TR1w);     // L2 + R1
        T3 = _mm_add_epi16(TR2w, TL1w);
        T0 = _mm_add_epi16(T0, V2);         // L0*2 + L1*8
        T1 = _mm_add_epi16(T1, V3);
        T4 = _mm_slli_epi16(T2, 1);
        T5 = _mm_slli_epi16(T3, 1);
        T0 = _mm_add_epi16(T0, V1);
        T1 = _mm_add_epi16(T1, V1);
        T2 = _mm_add_epi16(T2, T4);         // (L2 + R1) * 3
        T3 = _mm_add_epi16(T3, T5);

        T0 = _mm_add_epi16(T0, T2);
        T1 = _mm_add_epi16(T1, T3);
        T0 = _mm_srli_epi16(T0, 5);
        T1 = _mm_srli_epi16(T1, 5);

        TL0 = _mm_blendv_epi8(TL0, T0, FS);
        TR0 = _mm_blendv_epi8(TR0, T1, FS);

        /* cal L1/R1 */
        T4 = _mm_add_epi16(TL2w, TL1w);      // L2 + L1
        T5 = _mm_add_epi16(TR2w, TR1w);
        T2 = _mm_add_epi16(TR0w, TL1w);      // L1 + R0
        T3 = _mm_add_epi16(TL0w, TR1w);
        T0 = _mm_add_epi16(T4, TL0w);        // L0 + L1 + L2
        T1 = _mm_add_epi16(T5, TR0w);
        T2 = _mm_add_epi16(T2, V3);          // L1 + R0*3
        T3 = _mm_add_epi16(T3, V2);
        T0 = _mm_add_epi16(_mm_slli_epi16(T0, 2), T2);
        T1 = _mm_add_epi16(_mm_slli_epi16(T1, 2), T3);
        T0 = _mm_add_epi16(T0, c_8);
        T1 = _mm_add_epi16(T1, c_8);
        T0 = _mm_srli_epi16(T0, 4);
        T1 = _mm_srli_epi16(T1, 4);

        TL1 = _mm_blendv_epi8(TL1, T0, FS);
        TR1 = _mm_blendv_epi8(TR1, T1, FS);

        /* cal L2/R2 */
        T2 = _mm_add_epi16(T4, TL3);        // L1 + L2 + L3
        T3 = _mm_add_epi16(T5, TR3);
        T0 = _mm_add_epi16(V0, c_4);
        T2 = _mm_slli_epi16(T2, 1);
        T3 = _mm_slli_epi16(T3, 1);
        T2 = _mm_add_epi16(T2, T0);
        T3 = _mm_add_epi16(T3, T0);
        T0 = _mm_srli_epi16(T2, 3);
        T1 = _mm_srli_epi16(T3, 3);

        TL2 = _mm_blendv_epi8(TL2, T0, FS);
        TR2 = _mm_blendv_epi8(TR2, T1, FS);
    }

    /* stroe result */
    M0 = _mm_unpacklo_epi16(TL3, TL2);
    M1 = _mm_unpackhi_epi16(TL3, TL2);
    M2 = _mm_unpacklo_epi16(TL1, TL0);
    M3 = _mm_unpackhi_epi16(TL1, TL0);
    M4 = _mm_unpacklo_epi16(TR0, TR1);
    M5 = _mm_unpackhi_epi16(TR0, TR1);
    M6 = _mm_unpacklo_epi16(TR2, TR3);
    M7 = _mm_unpackhi_epi16(TR2, TR3);

    T0 = _mm_unpacklo_epi32(M0, M2);
    T1 = _mm_unpackhi_epi32(M0, M2);
    T2 = _mm_unpacklo_epi32(M1, M3);
    T3 = _mm_unpackhi_epi32(M1, M3);
    T4 = _mm_unpacklo_epi32(M4, M6);
    T5 = _mm_unpackhi_epi32(M4, M6);
    T6 = _mm_unpacklo_epi32(M5, M7);
    T7 = _mm_unpackhi_epi32(M5, M7);

    M0 = _mm_unpacklo_epi64(T0, T4);
    M1 = _mm_unpackhi_epi64(T0, T4);
    M4 = _mm_unpacklo_epi64(T2, T6);
    M5 = _mm_unpackhi_epi64(T2, T6);
    M2 = _mm_unpacklo_epi64(T1, T5);
    M3 = _mm_unpackhi_epi64(T1, T5);
    M6 = _mm_unpacklo_epi64(T3, T7);
    M7 = _mm_unpackhi_epi64(T3, T7);

    p_src = src - 4;
    _mm_storeu_si128((__m128i *)(p_src), M0);
    p_src += stride;
    _mm_storeu_si128((__m128i *)(p_src), M1);
    p_src += stride;
    _mm_storeu_si128((__m128i *)(p_src), M2);
    p_src += stride;
    _mm_storeu_si128((__m128i *)(p_src), M3);
    p_src += stride;
    _mm_storeu_si128((__m128i *)(p_src), M4);
    p_src += stride;
    _mm_storeu_si128((__m128i *)(p_src), M5);
    p_src += stride;
    _mm_storeu_si128((__m128i *)(p_src), M6);
    p_src += stride;
    _mm_storeu_si128((__m128i *)(p_src), M7);
}

void uavs3e_deblock_ver_chroma_sse(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag)
{
    int flag0 = -((flt_flag >> 1) & 1);
    int flag1 = -((flt_flag >> 9) & 1);

    __m128i UVL1, UVR1;
    __m128i TL0, TL1, TL2, TL3;
    __m128i TR0, TR1, TR2, TR3;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i P0, P1, P2, P3, P4, P5, P6, P7;
    __m128i V0, V1, V2, V3;
    __m128i M0, M1, M2;
    __m128i FLT0, FLT1;
    __m128i ALPHAU = _mm_set1_epi16((short)alpha_u);
    __m128i ALPHAV = _mm_set1_epi16((short)alpha_v);
    __m128i BETAU = _mm_set1_epi16((short)beta_u);
    __m128i BETAV = _mm_set1_epi16((short)beta_v);
    __m128i ALPHA = _mm_unpacklo_epi64(ALPHAU, ALPHAV);
    __m128i BETA = _mm_unpacklo_epi64(BETAU, BETAV);
    __m128i c_8 = _mm_set1_epi16(8);
    int stride2 = stride << 1;
    int stride3 = stride2 + stride;

    src_u -= 4;
    src_v -= 4;

    T0 = _mm_loadu_si128((__m128i *)(src_u));
    T4 = _mm_loadu_si128((__m128i *)(src_v));
    T1 = _mm_loadu_si128((__m128i *)(src_u + stride));
    T5 = _mm_loadu_si128((__m128i *)(src_v + stride));
    T2 = _mm_loadu_si128((__m128i *)(src_u + stride2));
    T6 = _mm_loadu_si128((__m128i *)(src_v + stride2));
    T3 = _mm_loadu_si128((__m128i *)(src_u + stride3));
    T7 = _mm_loadu_si128((__m128i *)(src_v + stride3));

    P0 = _mm_unpacklo_epi16(T0, T1);
    P1 = _mm_unpackhi_epi16(T0, T1);
    P2 = _mm_unpacklo_epi16(T2, T3);
    P3 = _mm_unpackhi_epi16(T2, T3);
    P4 = _mm_unpacklo_epi16(T4, T5);
    P5 = _mm_unpackhi_epi16(T4, T5);
    P6 = _mm_unpacklo_epi16(T6, T7);
    P7 = _mm_unpackhi_epi16(T6, T7);

    T0 = _mm_unpacklo_epi32(P0, P2);
    T1 = _mm_unpackhi_epi32(P0, P2);
    T2 = _mm_unpacklo_epi32(P1, P3);
    T3 = _mm_unpackhi_epi32(P1, P3);
    T4 = _mm_unpacklo_epi32(P4, P6);
    T5 = _mm_unpackhi_epi32(P4, P6);
    T6 = _mm_unpacklo_epi32(P5, P7);
    T7 = _mm_unpackhi_epi32(P5, P7);

    TL3 = _mm_unpacklo_epi64(T0, T4);
    TL2 = _mm_unpackhi_epi64(T0, T4);
    TL1 = _mm_unpacklo_epi64(T1, T5);
    TL0 = _mm_unpackhi_epi64(T1, T5);
    TR0 = _mm_unpacklo_epi64(T2, T6);
    TR1 = _mm_unpackhi_epi64(T2, T6);
    TR2 = _mm_unpacklo_epi64(T3, T7);
    TR3 = _mm_unpackhi_epi64(T3, T7);

    M0 = _mm_set_epi32(flag1, flag0, flag1, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    T2 = _mm_cmpgt_epi16(BETA, T0);     // abs(L1 - L0) < beta
    T3 = _mm_cmpgt_epi16(BETA, T1);     // abs(R1 - R0) < beta

    V0 = _mm_subabs_epu16(TL2, TL0);
    V1 = _mm_subabs_epu16(TR2, TR0);
    V2 = _mm_subabs_epu16(TL0, TR0);    // abs(L0 - R0)
    M1 = _mm_cmpgt_epi16(BETA, V0);     // abs(L2 - L0) < beta
    M2 = _mm_cmpgt_epi16(BETA, V1);     // abs(R2 - R0) < beta

    V0 = _mm_cmpgt_epi16(ALPHA, V2);    // abs(L0 - R0) < alpha
    V1 = _mm_srai_epi16(BETA, 2);       // Beta / 4

    FLT0 = _mm_and_si128(T2, T3);       // abs(L1 - L0) < beta && abs(R1 - R0) < beta
    FLT0 = _mm_and_si128(FLT0, M0);

    // abs(L1 - L0) > Beta / 4 || abs(R1 - R0) > Beta / 4
    FLT1 = _mm_or_si128(_mm_cmpgt_epi16(T0, V1), _mm_cmpgt_epi16(T1, V1));
    M1 = _mm_and_si128(M1, M2);       // abs(L2 - L0) < beta && abs(R2 - R0) < beta
    FLT1 = _mm_andnot_si128(FLT1, V0);  // abs(L1 - L0) <= Beta / 4 && abs(R1 - R0) <= Beta / 4 && abs(L0 - R0) < alpha
    M1 = _mm_and_si128(FLT0, M1);
    FLT1 = _mm_and_si128(FLT1, M1);

    UVL1 = TL1;
    UVR1 = TR1;

    /* fs == 4 */
    V2 = _mm_slli_epi16(TR0, 1);        // R0 * 2
    V3 = _mm_slli_epi16(TL0, 1);        // L0 * 2
    T0 = _mm_add_epi16(TL0, TL2);       // L0 + L2
    T1 = _mm_add_epi16(TR0, TR2);
    T2 = _mm_slli_epi16(TL1, 3);        //
    T3 = _mm_slli_epi16(TR1, 3);
    V0 = _mm_slli_epi16(T0, 1);
    V1 = _mm_slli_epi16(T1, 1);
    T2 = _mm_add_epi16(T2, T0);         // L1*8 + L0+L2
    T3 = _mm_add_epi16(T3, T1);
    V0 = _mm_add_epi16(V0, V2);         // (L0+L2)*2 + R0*2
    V1 = _mm_add_epi16(V1, V3);
    T2 = _mm_add_epi16(T2, c_8);
    T3 = _mm_add_epi16(T3, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL1 = _mm_blendv_epi8(TL1, V0, FLT1);
    TR1 = _mm_blendv_epi8(TR1, V1, FLT1);

    /* fs == 2 */
    T0 = _mm_add_epi16(UVL1, TR0);      // L1 + R0
    T1 = _mm_add_epi16(UVR1, TL0);
    T2 = _mm_slli_epi16(TL0, 3);        // L0 * 8
    T3 = _mm_slli_epi16(TR0, 3);
    V0 = _mm_slli_epi16(T0, 1);         // L1*2 + R0*2
    V1 = _mm_slli_epi16(T1, 1);
    V3 = _mm_add_epi16(T2, V3);         // L0 * 10
    V2 = _mm_add_epi16(T3, V2);         // R0 * 10
    V0 = _mm_add_epi16(V0, T0);         // L1*3 + R0*3
    V1 = _mm_add_epi16(V1, T1);
    T2 = _mm_add_epi16(V3, c_8);
    T3 = _mm_add_epi16(V2, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL0 = _mm_blendv_epi8(TL0, V0, FLT0);
    TR0 = _mm_blendv_epi8(TR0, V1, FLT0);

    /* stroe result */
    P0 = _mm_unpacklo_epi16(TL3, TL2);
    P1 = _mm_unpackhi_epi16(TL3, TL2);
    P2 = _mm_unpacklo_epi16(TL1, TL0);
    P3 = _mm_unpackhi_epi16(TL1, TL0);
    P4 = _mm_unpacklo_epi16(TR0, TR1);
    P5 = _mm_unpackhi_epi16(TR0, TR1);
    P6 = _mm_unpacklo_epi16(TR2, TR3);
    P7 = _mm_unpackhi_epi16(TR2, TR3);

    T0 = _mm_unpacklo_epi32(P0, P2);
    T1 = _mm_unpackhi_epi32(P0, P2);
    T2 = _mm_unpacklo_epi32(P1, P3);
    T3 = _mm_unpackhi_epi32(P1, P3);
    T4 = _mm_unpacklo_epi32(P4, P6);
    T5 = _mm_unpackhi_epi32(P4, P6);
    T6 = _mm_unpacklo_epi32(P5, P7);
    T7 = _mm_unpackhi_epi32(P5, P7);

    P0 = _mm_unpacklo_epi64(T0, T4);
    P1 = _mm_unpackhi_epi64(T0, T4);
    P2 = _mm_unpacklo_epi64(T1, T5);
    P3 = _mm_unpackhi_epi64(T1, T5);
    P4 = _mm_unpacklo_epi64(T2, T6);
    P5 = _mm_unpackhi_epi64(T2, T6);
    P6 = _mm_unpacklo_epi64(T3, T7);
    P7 = _mm_unpackhi_epi64(T3, T7);

    _mm_storeu_si128((__m128i *)(src_u), P0);
    _mm_storeu_si128((__m128i *)(src_v), P4);
    _mm_storeu_si128((__m128i *)(src_u + stride), P1);
    _mm_storeu_si128((__m128i *)(src_v + stride), P5);
    _mm_storeu_si128((__m128i *)(src_u + stride2), P2);
    _mm_storeu_si128((__m128i *)(src_v + stride2), P6);
    _mm_storeu_si128((__m128i *)(src_u + stride3), P3);
    _mm_storeu_si128((__m128i *)(src_v + stride3), P7);

}

void uavs3e_deblock_hor_luma_sse(pel *src, int stride, int alpha, int beta, int flt_flag)
{
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int inc4 = inc << 2;
    int flag0 = -(flt_flag & 1);
    int flag1 = -((flt_flag >> 8) & 1);

    __m128i TL0, TL1, TL2, TL3;
    __m128i TR0, TR1, TR2, TR3;
    __m128i TL0w, TL1w, TL2w, TR0w, TR1w, TR2w; //for swap
    __m128i V0, V1, V2, V3;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2;
    __m128i FLT_L, FLT_R, FLT, FS;
    __m128i FS3, FS4, FS5, FS6;

    __m128i ALPHA = _mm_set1_epi16((short)alpha);
    __m128i BETA = _mm_set1_epi16((short)beta);
    __m128i c_0 = _mm_set1_epi16(0);
    __m128i c_1 = _mm_set1_epi16(1);
    __m128i c_2 = _mm_set1_epi16(2);
    __m128i c_3 = _mm_set1_epi16(3);
    __m128i c_4 = _mm_set1_epi16(4);
    __m128i c_5 = _mm_set1_epi16(5);
    __m128i c_6 = _mm_set1_epi16(6);
    __m128i c_8 = _mm_set1_epi16(8);

    TL3 = _mm_loadu_si128((__m128i *)(src - inc4));
    TL2 = _mm_loadu_si128((__m128i *)(src - inc3));
    TL1 = _mm_loadu_si128((__m128i *)(src - inc2));
    TL0 = _mm_loadu_si128((__m128i *)(src - inc));
    TR0 = _mm_loadu_si128((__m128i *)(src + 0));
    TR1 = _mm_loadu_si128((__m128i *)(src + inc));
    TR2 = _mm_loadu_si128((__m128i *)(src + inc2));
    TR3 = _mm_loadu_si128((__m128i *)(src + inc3));

    M0 = _mm_set_epi32(flag1, flag1, flag0, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    FLT_L = _mm_and_si128(_mm_cmpgt_epi16(BETA, T0), c_2);
    FLT_R = _mm_and_si128(_mm_cmpgt_epi16(BETA, T1), c_2);

    T4 = _mm_subabs_epu16(TL2, TL0);
    T5 = _mm_subabs_epu16(TR2, TR0);
    M1 = _mm_cmpgt_epi16(BETA, T4);
    M2 = _mm_cmpgt_epi16(BETA, T5);

    T6 = _mm_subabs_epu16(TL0, TR0); // AbsDelta
    T2 = _mm_cmpgt_epi16(ALPHA, T6); // AbsDelta < alpha
    T3 = _mm_srai_epi16(BETA, 2);    // Beta / 4

    FLT_L = _mm_add_epi16(_mm_and_si128(M1, c_1), FLT_L);
    FLT_R = _mm_add_epi16(_mm_and_si128(M2, c_1), FLT_R);
    FLT = _mm_add_epi16(FLT_L, FLT_R);

    // !(COM_ABS(R0 - R1) <= Beta / 4 || COM_ABS(L0 - L1) <= Beta / 4)
    M1 = _mm_or_si128(_mm_cmpgt_epi16(T0, T3), _mm_cmpgt_epi16(T1, T3));
    M2 = _mm_and_si128(_mm_cmpeq_epi16(TR0, TR1), _mm_cmpeq_epi16(TL0, TL1));
    M1 = _mm_andnot_si128(M1, T2);

    T7 = _mm_subabs_epu16(TL1, TR1);
    FS6 = _mm_blendv_epi8(c_3, c_4, M1);
    FS5 = _mm_blendv_epi8(c_2, c_3, M2);
    FS4 = _mm_blendv_epi8(c_1, c_2, _mm_cmpeq_epi16(FLT_L, c_2));
    FS3 = _mm_blendv_epi8(c_0, c_1, _mm_cmpgt_epi16(BETA, T7));

    FS = _mm_blendv_epi8(c_0, FS6, _mm_cmpeq_epi16(FLT, c_6));
    FS = _mm_blendv_epi8(FS, FS5, _mm_cmpeq_epi16(FLT, c_5));
    FS = _mm_blendv_epi8(FS, FS4, _mm_cmpeq_epi16(FLT, c_4));
    FS = _mm_blendv_epi8(FS, FS3, _mm_cmpeq_epi16(FLT, c_3));

    FS = _mm_and_si128(FS, M0);

    TL0w = TL0;
    TL1w = TL1;
    TR0w = TR0;
    TR1w = TR1;

    /* fs == 1 */
    V0 = _mm_add_epi16(TL0w, TR0w);         // L0 + R0
    V1 = _mm_add_epi16(V0, c_2);            // L0 + R0 + 2
    V2 = _mm_slli_epi16(TL0w, 1);           // L0 << 1
    V3 = _mm_slli_epi16(TR0w, 1);

    T2 = _mm_cmpeq_epi16(FS, c_1);
    T0 = _mm_srli_epi16(_mm_add_epi16(V2, V1), 2);
    T1 = _mm_srli_epi16(_mm_add_epi16(V3, V1), 2);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 2 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 1) + (R0 << 1) + 4
    T0 = _mm_add_epi16(TL1w, TR0w);         // L1 + R0
    T1 = _mm_add_epi16(TR1w, TL0w);
    T2 = _mm_slli_epi16(TL1w, 1);           // L1 << 1
    T3 = _mm_slli_epi16(TR1w, 1);
    T0 = _mm_add_epi16(T0, V1);             // L1 + R0 + L0*2 + R0*2 + 4
    T1 = _mm_add_epi16(T1, V1);
    T4 = _mm_slli_epi16(TL0w, 3);           // L0*8
    T5 = _mm_slli_epi16(TR0w, 3);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_4);
    T5 = _mm_add_epi16(T5, c_4);

    T2 = _mm_cmpeq_epi16(FS, c_2);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T2);
    TR0 = _mm_blendv_epi8(TR0, T1, T2);

    /* fs == 3 */
    V1 = _mm_slli_epi16(V1, 1);             // (L0 << 2) + (R0 << 2) + 8
    T0 = _mm_slli_epi16(TL1w, 2);           // L1 << 2
    T1 = _mm_slli_epi16(TR1w, 2);
    T4 = _mm_add_epi16(TL2, TR1w);          // L2 + R1
    T5 = _mm_add_epi16(TR2, TL1w);

    T0 = _mm_add_epi16(T0, V2);             // (L1 << 2) + (L0 << 1)
    T1 = _mm_add_epi16(T1, V3);
    T4 = _mm_add_epi16(T4, V1);
    T5 = _mm_add_epi16(T5, V1);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T6 = _mm_cmpeq_epi16(FS, c_3);
    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL0 = _mm_blendv_epi8(TL0, T0, T6);
    TR0 = _mm_blendv_epi8(TR0, T1, T6);

    T2 = _mm_add_epi16(TL2, TR0w);          // L2 + R0
    T3 = _mm_add_epi16(TR2, TL0w);
    T4 = _mm_slli_epi16(TL2, 1);
    T5 = _mm_slli_epi16(TR2, 1);
    T0 = _mm_slli_epi16(TL1w, 3);
    T1 = _mm_slli_epi16(TR1w, 3);
    T2 = _mm_add_epi16(T2, T4);             // L2 + R0 + L2*2
    T3 = _mm_add_epi16(T3, T5);
    T4 = _mm_slli_epi16(TL0w, 2);           // L0*4
    T5 = _mm_slli_epi16(TR0w, 2);
    T0 = _mm_add_epi16(T0, T2);
    T1 = _mm_add_epi16(T1, T3);
    T4 = _mm_add_epi16(T4, c_8);
    T5 = _mm_add_epi16(T5, c_8);
    T0 = _mm_add_epi16(T0, T4);
    T1 = _mm_add_epi16(T1, T5);

    T0 = _mm_srli_epi16(T0, 4);
    T1 = _mm_srli_epi16(T1, 4);

    TL1 = _mm_blendv_epi8(TL1, T0, T6);
    TR1 = _mm_blendv_epi8(TR1, T1, T6);

    FS = _mm_cmpeq_epi16(FS, c_4);

    if (!_mm_testz_si128(FS, _mm_set1_epi16(-1))) { /* fs == 4 */
        TL2w = TL2;
        TR2w = TR2;

        /* cal L0/R0 */
        V1 = _mm_slli_epi16(V1, 1);         // (L0 << 3) + (R0 << 3) + 16
        T0 = _mm_slli_epi16(TL1w, 3);       // L1 << 3
        T1 = _mm_slli_epi16(TR1w, 3);
        T2 = _mm_add_epi16(TL2w, TR1w);     // L2 + R1
        T3 = _mm_add_epi16(TR2w, TL1w);
        T0 = _mm_add_epi16(T0, V2);         // L0*2 + L1*8
        T1 = _mm_add_epi16(T1, V3);
        T4 = _mm_slli_epi16(T2, 1);
        T5 = _mm_slli_epi16(T3, 1);
        T0 = _mm_add_epi16(T0, V1);
        T1 = _mm_add_epi16(T1, V1);
        T2 = _mm_add_epi16(T2, T4);         // (L2 + R1) * 3
        T3 = _mm_add_epi16(T3, T5);

        T0 = _mm_add_epi16(T0, T2);
        T1 = _mm_add_epi16(T1, T3);
        T0 = _mm_srli_epi16(T0, 5);
        T1 = _mm_srli_epi16(T1, 5);

        TL0 = _mm_blendv_epi8(TL0, T0, FS);
        TR0 = _mm_blendv_epi8(TR0, T1, FS);

        /* cal L1/R1 */
        T4 = _mm_add_epi16(TL2w, TL1w);      // L2 + L1
        T5 = _mm_add_epi16(TR2w, TR1w);
        T2 = _mm_add_epi16(TR0w, TL1w);      // L1 + R0
        T3 = _mm_add_epi16(TL0w, TR1w);
        T0 = _mm_add_epi16(T4, TL0w);        // L0 + L1 + L2
        T1 = _mm_add_epi16(T5, TR0w);
        T2 = _mm_add_epi16(T2, V3);          // L1 + R0*3
        T3 = _mm_add_epi16(T3, V2);
        T0 = _mm_add_epi16(_mm_slli_epi16(T0, 2), T2);
        T1 = _mm_add_epi16(_mm_slli_epi16(T1, 2), T3);
        T0 = _mm_add_epi16(T0, c_8);
        T1 = _mm_add_epi16(T1, c_8);
        T0 = _mm_srli_epi16(T0, 4);
        T1 = _mm_srli_epi16(T1, 4);

        TL1 = _mm_blendv_epi8(TL1, T0, FS);
        TR1 = _mm_blendv_epi8(TR1, T1, FS);

        /* cal L2/R2 */
        T2 = _mm_add_epi16(T4, TL3);        // L1 + L2 + L3
        T3 = _mm_add_epi16(T5, TR3);
        T0 = _mm_add_epi16(V0, c_4);
        T2 = _mm_slli_epi16(T2, 1);
        T3 = _mm_slli_epi16(T3, 1);
        T2 = _mm_add_epi16(T2, T0);
        T3 = _mm_add_epi16(T3, T0);
        T0 = _mm_srli_epi16(T2, 3);
        T1 = _mm_srli_epi16(T3, 3);

        TL2 = _mm_blendv_epi8(TL2, T0, FS);
        TR2 = _mm_blendv_epi8(TR2, T1, FS);

        /* stroe result */
        _mm_storeu_si128((__m128i *)(src - inc), TL0);
        _mm_storeu_si128((__m128i *)(src - 0), TR0);

        _mm_storeu_si128((__m128i *)(src - inc2), TL1);
        _mm_storeu_si128((__m128i *)(src + inc), TR1);

        _mm_storeu_si128((__m128i *)(src - inc3), TL2);
        _mm_storeu_si128((__m128i *)(src + inc2), TR2);
    } else {
        /* stroe result */
        _mm_storeu_si128((__m128i *)(src - inc), TL0);
        _mm_storeu_si128((__m128i *)(src - 0), TR0);

        _mm_storeu_si128((__m128i *)(src - inc2), TL1);
        _mm_storeu_si128((__m128i *)(src + inc), TR1);
    }

}

void uavs3e_deblock_hor_chroma_sse(pel *src_u, pel *src_v, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag)
{
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int flag0 = -((flt_flag >> 1) & 1);
    int flag1 = -((flt_flag >> 9) & 1);

    __m128i UL1, UR1;
    __m128i TL0, TL1, TL2;
    __m128i TR0, TR1, TR2;
    __m128i T0, T1, T2, T3;
    __m128i V0, V1, V2, V3;
    __m128i M0, M1, M2;
    __m128i FLT0, FLT1;

    __m128i ALPHAU = _mm_set1_epi16((short)alpha_u);
    __m128i ALPHAV = _mm_set1_epi16((short)alpha_v);
    __m128i ALPHA = _mm_unpacklo_epi64(ALPHAU, ALPHAV);
    __m128i BETAU = _mm_set1_epi16((short)beta_u);
    __m128i BETAV = _mm_set1_epi16((short)beta_v);
    __m128i BETA = _mm_unpacklo_epi64(BETAU, BETAV);
    __m128i c_8 = _mm_set1_epi16(8);

    TL2 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i *)(src_u - inc3)), _mm_loadl_epi64((__m128i *)(src_v - inc3)));
    TL1 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i *)(src_u - inc2)), _mm_loadl_epi64((__m128i *)(src_v - inc2)));
    TL0 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i *)(src_u - inc)) , _mm_loadl_epi64((__m128i *)(src_v - inc)));
    TR0 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i *)(src_u + 0))   , _mm_loadl_epi64((__m128i *)(src_v + 0)));
    TR1 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i *)(src_u + inc)) , _mm_loadl_epi64((__m128i *)(src_v + inc)));
    TR2 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i *)(src_u + inc2)), _mm_loadl_epi64((__m128i *)(src_v + inc2)));

    M0 = _mm_set_epi32(flag1, flag0, flag1, flag0);

    T0 = _mm_subabs_epu16(TL1, TL0);
    T1 = _mm_subabs_epu16(TR1, TR0);
    T2 = _mm_cmpgt_epi16(BETA, T0);     // abs(L1 - L0) < beta
    T3 = _mm_cmpgt_epi16(BETA, T1);     // abs(R1 - R0) < beta

    V0 = _mm_subabs_epu16(TL2, TL0);
    V1 = _mm_subabs_epu16(TR2, TR0);
    V2 = _mm_subabs_epu16(TL0, TR0);    // abs(L0 - R0)
    M1 = _mm_cmpgt_epi16(BETA, V0);     // abs(L2 - L0) < beta
    M2 = _mm_cmpgt_epi16(BETA, V1);     // abs(R2 - R0) < beta

    V0 = _mm_cmpgt_epi16(ALPHA, V2);    // abs(L0 - R0) < alpha
    V1 = _mm_srai_epi16(BETA, 2);       // Beta / 4

    FLT0 = _mm_and_si128(T2, T3);       // abs(L1 - L0) < beta && abs(R1 - R0) < beta
    FLT0 = _mm_and_si128(FLT0, M0);

    // abs(L1 - L0) > Beta / 4 || abs(R1 - R0) > Beta / 4
    FLT1 = _mm_or_si128(_mm_cmpgt_epi16(T0, V1), _mm_cmpgt_epi16(T1, V1));
    M1 = _mm_and_si128(M1, M2);       // abs(L2 - L0) < beta && abs(R2 - R0) < beta
    FLT1 = _mm_andnot_si128(FLT1, V0);  // abs(L1 - L0) <= Beta / 4 && abs(R1 - R0) <= Beta / 4 && abs(L0 - R0) < alpha
    M1 = _mm_and_si128(FLT0, M1);
    FLT1 = _mm_and_si128(FLT1, M1);

    UR1 = TR1;
    UL1 = TL1;

    /* fs == 4 */
    V2 = _mm_slli_epi16(TR0, 1);        // R0 * 2
    V3 = _mm_slli_epi16(TL0, 1);        // L0 * 2
    T0 = _mm_add_epi16(TL0, TL2);       // L0 + L2
    T1 = _mm_add_epi16(TR0, TR2);
    T2 = _mm_slli_epi16(TL1, 3);        //
    T3 = _mm_slli_epi16(TR1, 3);
    V0 = _mm_slli_epi16(T0, 1);
    V1 = _mm_slli_epi16(T1, 1);
    T2 = _mm_add_epi16(T2, T0);         // L1*8 + L0+L2
    T3 = _mm_add_epi16(T3, T1);
    V0 = _mm_add_epi16(V0, V2);         // (L0+L2)*2 + R0*2
    V1 = _mm_add_epi16(V1, V3);
    T2 = _mm_add_epi16(T2, c_8);
    T3 = _mm_add_epi16(T3, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL1 = _mm_blendv_epi8(TL1, V0, FLT1);
    TR1 = _mm_blendv_epi8(TR1, V1, FLT1);

    /* fs == 2 */
    T0 = _mm_add_epi16(UL1, TR0);       // L1 + R0
    T1 = _mm_add_epi16(UR1, TL0);
    T2 = _mm_slli_epi16(TL0, 3);        // L0 * 8
    T3 = _mm_slli_epi16(TR0, 3);
    V0 = _mm_slli_epi16(T0, 1);         // L1*2 + R0*2
    V1 = _mm_slli_epi16(T1, 1);
    V3 = _mm_add_epi16(T2, V3);         // L0 * 10
    V2 = _mm_add_epi16(T3, V2);         // R0 * 10
    V0 = _mm_add_epi16(V0, T0);         // L1*3 + R0*3
    V1 = _mm_add_epi16(V1, T1);
    T2 = _mm_add_epi16(V3, c_8);
    T3 = _mm_add_epi16(V2, c_8);
    V0 = _mm_add_epi16(V0, T2);
    V1 = _mm_add_epi16(V1, T3);
    V0 = _mm_srli_epi16(V0, 4);
    V1 = _mm_srli_epi16(V1, 4);

    TL0 = _mm_blendv_epi8(TL0, V0, FLT0);
    TR0 = _mm_blendv_epi8(TR0, V1, FLT0);

    /* stroe result */
    _mm_storel_epi64((__m128i *)(src_u - inc2), TL1);
    _mm_storel_epi64((__m128i *)(src_u - inc), TL0);
    _mm_storel_epi64((__m128i *)(src_u), TR0);
    _mm_storel_epi64((__m128i *)(src_u + inc), TR1);

    TL1 = _mm_srli_si128(TL1, 8);
    TL0 = _mm_srli_si128(TL0, 8);
    TR0 = _mm_srli_si128(TR0, 8);
    TR1 = _mm_srli_si128(TR1, 8);

    _mm_storel_epi64((__m128i *)(src_v - inc2), TL1);
    _mm_storel_epi64((__m128i *)(src_v - inc), TL0);
    _mm_storel_epi64((__m128i *)(src_v), TR0);
    _mm_storel_epi64((__m128i *)(src_v + inc), TR1);
}

#endif
#undef _mm_subabs_epu16
