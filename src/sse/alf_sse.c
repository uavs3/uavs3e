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

#include "sse.h"

#if (BIT_DEPTH == 8)

void uavs3e_alf_one_lcu_sse(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;

    __m128i T00, T01, T10, T11, T20, T21, T30, T31, T40, T41, T50, T51;
    __m128i T1, T2, T3, T4, T5, T6, T7, T8;
    __m128i E00, E01, E10, E11, E20, E21, E30, E31, E40, E41;
    __m128i C0, C1, C2, C3, C4, C30, C31, C32, C33;
    __m128i S0, S00, S01, S1, S10, S11, S2, S20, S21, S3, S30, S31, S4, S40, S41, S5, S50, S51, S52, S53, S6, S7, S8, SS1, SS2, SS3, S;
    __m128i mSwitch1, mSwitch2, mSwitch3, mSwitch4, mSwitch5;
    __m128i mAddOffset;
    __m128i mZero = _mm_set1_epi16(0);
    __m128i mMax = _mm_set1_epi16((short)((1 << sample_bit_depth) - 1));
    __m128i ones = _mm_set1_epi16(1);

    int i, j;

    C0 = _mm_set1_epi8(coef[0]);
    C1 = _mm_set1_epi8(coef[1]);
    C2 = _mm_set1_epi8(coef[2]);
    C3 = _mm_set1_epi8(coef[3]);
    C4 = _mm_set1_epi8(coef[4]);

    mSwitch1 = _mm_setr_epi8(0, 1, 2, 3, 2, 1, 0, 3, 0, 1, 2, 3, 2, 1, 0, 3);
    C30 = _mm_loadu_si128((__m128i *)&coef[5]);
    C31 = _mm_packs_epi32(C30, C30);
    C32 = _mm_packs_epi16(C31, C31);
    C33 = _mm_shuffle_epi8(C32, mSwitch1);
    mSwitch2 = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, -1, 1, 2, 3, 4, 5, 6, 7, -1);
    mSwitch3 = _mm_setr_epi8(2, 3, 4, 5, 6, 7, 8, -1, 3, 4, 5, 6, 7, 8, 9, -1);
    mSwitch4 = _mm_setr_epi8(4, 5, 6, 7, 8, 9, 10, -1, 5, 6, 7, 8, 9, 10, 11, -1);
    mSwitch5 = _mm_setr_epi8(6, 7, 8, 9, 10, 11, 12, -1, 7, 8, 9, 10, 11, 12, 13, -1);
    mAddOffset = _mm_set1_epi32(32);

    for (i = 0; i < lcu_height; i++) {
        imgPad1 = src + i_src;
        imgPad2 = src - i_src;
        imgPad3 = src + 2 * i_src;
        imgPad4 = src - 2 * i_src;
        imgPad5 = src + 3 * i_src;
        imgPad6 = src - 3 * i_src;
        if (i < 3) {
            if (i == 0) {
                imgPad4 = imgPad2 = src;
            } else if (i == 1) {
                imgPad4 = imgPad2;
            }
            imgPad6 = imgPad4;
        } else if (i > lcu_height - 4) {
            if (i == lcu_height - 1) {
                imgPad3 = imgPad1 = src;
            } else if (i == lcu_height - 2) {
                imgPad3 = imgPad1;
            }
            imgPad5 = imgPad3;
        }

        for (j = 0; j < lcu_width - 15; j += 16) {
            T00 = _mm_load_si128((__m128i *)&imgPad6[j]);
            T01 = _mm_load_si128((__m128i *)&imgPad5[j]);
            E00 = _mm_unpacklo_epi8(T00, T01);
            E01 = _mm_unpackhi_epi8(T00, T01);
            S00 = _mm_maddubs_epi16(E00, C0);//前8个像素所有C0*P0的结果
            S01 = _mm_maddubs_epi16(E01, C0);//后8个像素所有C0*P0的结果

            T10 = _mm_load_si128((__m128i *)&imgPad4[j]);
            T11 = _mm_load_si128((__m128i *)&imgPad3[j]);
            E10 = _mm_unpacklo_epi8(T10, T11);
            E11 = _mm_unpackhi_epi8(T10, T11);
            S10 = _mm_maddubs_epi16(E10, C1);//前8个像素所有C1*P1的结果
            S11 = _mm_maddubs_epi16(E11, C1);//后8个像素所有C1*P1的结果

            T20 = _mm_loadu_si128((__m128i *)&imgPad2[j - 1]);
            T21 = _mm_loadu_si128((__m128i *)&imgPad1[j + 1]);
            E20 = _mm_unpacklo_epi8(T20, T21);
            E21 = _mm_unpackhi_epi8(T20, T21);
            S20 = _mm_maddubs_epi16(E20, C2);
            S21 = _mm_maddubs_epi16(E21, C2);

            T30 = _mm_load_si128((__m128i *)&imgPad2[j]);
            T31 = _mm_load_si128((__m128i *)&imgPad1[j]);
            E30 = _mm_unpacklo_epi8(T30, T31);
            E31 = _mm_unpackhi_epi8(T30, T31);
            S30 = _mm_maddubs_epi16(E30, C3);
            S31 = _mm_maddubs_epi16(E31, C3);

            T40 = _mm_loadu_si128((__m128i *)&imgPad2[j + 1]);
            T41 = _mm_loadu_si128((__m128i *)&imgPad1[j - 1]);
            E40 = _mm_unpacklo_epi8(T40, T41);
            E41 = _mm_unpackhi_epi8(T40, T41);
            S40 = _mm_maddubs_epi16(E40, C4);
            S41 = _mm_maddubs_epi16(E41, C4);

            T50 = _mm_loadu_si128((__m128i *)&src[j - 3]);
            T51 = _mm_loadu_si128((__m128i *)&src[j + 5]);
            T1 = _mm_shuffle_epi8(T50, mSwitch2);
            T2 = _mm_shuffle_epi8(T50, mSwitch3);
            T3 = _mm_shuffle_epi8(T50, mSwitch4);
            T4 = _mm_shuffle_epi8(T50, mSwitch5);
            T5 = _mm_shuffle_epi8(T51, mSwitch2);
            T6 = _mm_shuffle_epi8(T51, mSwitch3);
            T7 = _mm_shuffle_epi8(T51, mSwitch4);
            T8 = _mm_shuffle_epi8(T51, mSwitch5);

            S5 = _mm_maddubs_epi16(T1, C33);
            S6 = _mm_maddubs_epi16(T2, C33);
            S7 = _mm_maddubs_epi16(T3, C33);
            S8 = _mm_maddubs_epi16(T4, C33);
            S50 = _mm_madd_epi16(S5, ones);
            S51 = _mm_madd_epi16(S6, ones);
            S52 = _mm_madd_epi16(S7, ones);
            S53 = _mm_madd_epi16(S8, ones);
            S5 = _mm_hadd_epi32(S50, S51);
            S6 = _mm_hadd_epi32(S52, S53);//前8个
            S3 = _mm_maddubs_epi16(T5, C33);
            S4 = _mm_maddubs_epi16(T6, C33);
            S7 = _mm_maddubs_epi16(T7, C33);
            S8 = _mm_maddubs_epi16(T8, C33);
            S50 = _mm_madd_epi16(S3, ones);
            S51 = _mm_madd_epi16(S4, ones);
            S52 = _mm_madd_epi16(S7, ones);
            S53 = _mm_madd_epi16(S8, ones);
            S7 = _mm_hadd_epi32(S50, S51);
            S8 = _mm_hadd_epi32(S52, S53);//后8个

            S0 = _mm_cvtepi16_epi32(S00);
            S1 = _mm_cvtepi16_epi32(S10);
            S2 = _mm_cvtepi16_epi32(S20);
            S3 = _mm_cvtepi16_epi32(S30);
            S4 = _mm_cvtepi16_epi32(S40);
            S0 = _mm_add_epi32(S0, S1);
            S2 = _mm_add_epi32(S2, S3);
            S5 = _mm_add_epi32(S4, S5);
            S5 = _mm_add_epi32(S0, S5);
            S5 = _mm_add_epi32(S2, S5);
            S0 = _mm_cvtepi16_epi32(_mm_srli_si128(S00, 8));
            S1 = _mm_cvtepi16_epi32(_mm_srli_si128(S10, 8));
            S2 = _mm_cvtepi16_epi32(_mm_srli_si128(S20, 8));
            S3 = _mm_cvtepi16_epi32(_mm_srli_si128(S30, 8));
            S4 = _mm_cvtepi16_epi32(_mm_srli_si128(S40, 8));
            S0 = _mm_add_epi32(S0, S1);
            S2 = _mm_add_epi32(S2, S3);
            S6 = _mm_add_epi32(S4, S6);
            S6 = _mm_add_epi32(S0, S6);
            S6 = _mm_add_epi32(S2, S6);//前8个
            S0 = _mm_cvtepi16_epi32(S01);
            S1 = _mm_cvtepi16_epi32(S11);
            S2 = _mm_cvtepi16_epi32(S21);
            S3 = _mm_cvtepi16_epi32(S31);
            S4 = _mm_cvtepi16_epi32(S41);
            S0 = _mm_add_epi32(S0, S1);
            S2 = _mm_add_epi32(S2, S3);
            S7 = _mm_add_epi32(S4, S7);
            S7 = _mm_add_epi32(S0, S7);
            S7 = _mm_add_epi32(S2, S7);
            S0 = _mm_cvtepi16_epi32(_mm_srli_si128(S01, 8));
            S1 = _mm_cvtepi16_epi32(_mm_srli_si128(S11, 8));
            S2 = _mm_cvtepi16_epi32(_mm_srli_si128(S21, 8));
            S3 = _mm_cvtepi16_epi32(_mm_srli_si128(S31, 8));
            S4 = _mm_cvtepi16_epi32(_mm_srli_si128(S41, 8));
            S0 = _mm_add_epi32(S0, S1);
            S2 = _mm_add_epi32(S2, S3);
            S8 = _mm_add_epi32(S4, S8);
            S8 = _mm_add_epi32(S0, S8);
            S8 = _mm_add_epi32(S2, S8);//后8个

            SS1 = _mm_add_epi32(S5, mAddOffset);
            SS1 = _mm_srai_epi32(SS1, 6);
            SS2 = _mm_add_epi32(S6, mAddOffset);
            SS2 = _mm_srai_epi32(SS2, 6);
            SS1 = _mm_packus_epi32(SS1, SS2);
            SS1 = _mm_min_epi16(SS1, mMax);
            SS1 = _mm_max_epi16(SS1, mZero);

            SS2 = _mm_add_epi32(S7, mAddOffset);
            SS2 = _mm_srai_epi32(SS2, 6);
            SS3 = _mm_add_epi32(S8, mAddOffset);
            SS3 = _mm_srai_epi32(SS3, 6);
            SS2 = _mm_packus_epi32(SS2, SS3);
            SS2 = _mm_min_epi16(SS2, mMax);
            SS2 = _mm_max_epi16(SS2, mZero);

            S = _mm_packus_epi16(SS1, SS2);
            _mm_storeu_si128((__m128i *)(dst + j), S);
        }

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_alf_calc_corr_sse(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail)
{

    __m128i T00, T01, T10, T11, T20, T21, T30, T31, T40, T41, T50, T51, T60, T61, T70, T71;
    __m128i T8;
    __m128i E00, E01, E10, E11, E20, E21, E30, E31, E40, E41, E50, E51, E60, E61, E70, E71, E80, E81, E0, E1, E2, E3;
    __m128i C0, C1, C;
    __m128i S0, S1, S2, S3, S00, S01;
    __m128i mZero = _mm_set1_epi16(0);
    __m128i R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14;

    int xPosEnd = xPos + width;
    int startPos = isAboveAvail ? (yPos - 4) : yPos;
    int endPos = isBelowAvail ? (yPos + height - 4) : (yPos + height);

    int yUp, yBottom;

    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;
    int i, j, k, l;

    pel *imgPad = p_alf + startPos * i_alf;
    pel *imgOrg = p_org + startPos * i_org;

    R1 = R2 = R3 = R4 = R5 = R6 = R7 = R8 = R9 = R10 = R11 = R12 = R13 = R14 = mZero;

    for (i = startPos; i < endPos; i++) {
        yUp = COM_CLIP3(startPos, endPos - 1, i - 1);
        yBottom = COM_CLIP3(startPos, endPos - 1, i + 1);
        imgPad1 = imgPad + (yBottom - i) * i_alf;
        imgPad2 = imgPad + (yUp - i) * i_alf;

        yUp = COM_CLIP3(startPos, endPos - 1, i - 2);
        yBottom = COM_CLIP3(startPos, endPos - 1, i + 2);
        imgPad3 = imgPad + (yBottom - i) * i_alf;
        imgPad4 = imgPad + (yUp - i) * i_alf;

        yUp = COM_CLIP3(startPos, endPos - 1, i - 3);
        yBottom = COM_CLIP3(startPos, endPos - 1, i + 3);
        imgPad5 = imgPad + (yBottom - i) * i_alf;
        imgPad6 = imgPad + (yUp - i) * i_alf;

        for (j = xPos; j < xPosEnd; j += 16) {
            T00 = _mm_loadu_si128((__m128i*)&imgPad6[j]);
            T01 = _mm_loadu_si128((__m128i*)&imgPad5[j]);
            E0 = _mm_unpacklo_epi8(T00, mZero);
            E1 = _mm_unpackhi_epi8(T00, mZero);
            E2 = _mm_unpacklo_epi8(T01, mZero);
            E3 = _mm_unpackhi_epi8(T01, mZero);
            E00 = _mm_add_epi16(E0, E2);
            E01 = _mm_add_epi16(E1, E3);

            T10 = _mm_loadu_si128((__m128i*)&imgPad4[j]);
            T11 = _mm_loadu_si128((__m128i*)&imgPad3[j]);
            E0 = _mm_unpacklo_epi8(T10, mZero);
            E1 = _mm_unpackhi_epi8(T10, mZero);
            E2 = _mm_unpacklo_epi8(T11, mZero);
            E3 = _mm_unpackhi_epi8(T11, mZero);
            E10 = _mm_add_epi16(E0, E2);
            E11 = _mm_add_epi16(E1, E3);

            T20 = _mm_loadu_si128((__m128i*)&imgPad2[j - 1]);
            T21 = _mm_loadu_si128((__m128i*)&imgPad1[j + 1]);
            E0 = _mm_unpacklo_epi8(T20, mZero);
            E1 = _mm_unpackhi_epi8(T20, mZero);
            E2 = _mm_unpacklo_epi8(T21, mZero);
            E3 = _mm_unpackhi_epi8(T21, mZero);
            E20 = _mm_add_epi16(E0, E2);
            E21 = _mm_add_epi16(E1, E3);

            T30 = _mm_loadu_si128((__m128i*)&imgPad2[j]);
            T31 = _mm_loadu_si128((__m128i*)&imgPad1[j]);
            E0 = _mm_unpacklo_epi8(T30, mZero);
            E1 = _mm_unpackhi_epi8(T30, mZero);
            E2 = _mm_unpacklo_epi8(T31, mZero);
            E3 = _mm_unpackhi_epi8(T31, mZero);
            E30 = _mm_add_epi16(E0, E2);
            E31 = _mm_add_epi16(E1, E3);

            T40 = _mm_loadu_si128((__m128i*)&imgPad2[j + 1]);
            T41 = _mm_loadu_si128((__m128i*)&imgPad1[j - 1]);
            E0 = _mm_unpacklo_epi8(T40, mZero);
            E1 = _mm_unpackhi_epi8(T40, mZero);
            E2 = _mm_unpacklo_epi8(T41, mZero);
            E3 = _mm_unpackhi_epi8(T41, mZero);
            E40 = _mm_add_epi16(E0, E2);
            E41 = _mm_add_epi16(E1, E3);

            T50 = _mm_loadu_si128((__m128i*)&imgPad[j - 3]);
            T51 = _mm_loadu_si128((__m128i*)&imgPad[j + 3]);
            E0 = _mm_unpacklo_epi8(T50, mZero);
            E1 = _mm_unpackhi_epi8(T50, mZero);
            E2 = _mm_unpacklo_epi8(T51, mZero);
            E3 = _mm_unpackhi_epi8(T51, mZero);
            E50 = _mm_add_epi16(E0, E2);
            E51 = _mm_add_epi16(E1, E3);

            T60 = _mm_loadu_si128((__m128i*)&imgPad[j - 2]);
            T61 = _mm_loadu_si128((__m128i*)&imgPad[j + 2]);
            E0 = _mm_unpacklo_epi8(T60, mZero);
            E1 = _mm_unpackhi_epi8(T60, mZero);
            E2 = _mm_unpacklo_epi8(T61, mZero);
            E3 = _mm_unpackhi_epi8(T61, mZero);
            E60 = _mm_add_epi16(E0, E2);
            E61 = _mm_add_epi16(E1, E3);

            T70 = _mm_loadu_si128((__m128i*)&imgPad[j - 1]);
            T71 = _mm_loadu_si128((__m128i*)&imgPad[j + 1]);
            E0 = _mm_unpacklo_epi8(T70, mZero);
            E1 = _mm_unpackhi_epi8(T70, mZero);
            E2 = _mm_unpacklo_epi8(T71, mZero);
            E3 = _mm_unpackhi_epi8(T71, mZero);
            E70 = _mm_add_epi16(E0, E2);
            E71 = _mm_add_epi16(E1, E3);

            T8 = _mm_loadu_si128((__m128i*)&imgPad[j]);
            E80 = _mm_unpacklo_epi8(T8, mZero);
            E81 = _mm_unpackhi_epi8(T8, mZero);

            C = _mm_loadu_si128((__m128i*)&imgOrg[j]);
            C0 = _mm_unpacklo_epi8(C, mZero);
            C1 = _mm_unpackhi_epi8(C, mZero);

            S00 = _mm_madd_epi16(E00, E00);
            S01 = _mm_madd_epi16(E01, E01);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E10);
            S01 = _mm_madd_epi16(E01, E11);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E20);
            S01 = _mm_madd_epi16(E01, E21);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E30);
            S01 = _mm_madd_epi16(E01, E31);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R1 = _mm_add_epi32(R1, S0);

            S00 = _mm_madd_epi16(E00, E40);
            S01 = _mm_madd_epi16(E01, E41);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E50);
            S01 = _mm_madd_epi16(E01, E51);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E60);
            S01 = _mm_madd_epi16(E01, E61);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E70);
            S01 = _mm_madd_epi16(E01, E71);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R2 = _mm_add_epi32(R2, S0);

            S00 = _mm_madd_epi16(E00, E80);
            S01 = _mm_madd_epi16(E01, E81);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E10);
            S01 = _mm_madd_epi16(E11, E11);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E20);
            S01 = _mm_madd_epi16(E11, E21);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E30);
            S01 = _mm_madd_epi16(E11, E31);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R3 = _mm_add_epi32(R3, S0);

            S00 = _mm_madd_epi16(E10, E40);
            S01 = _mm_madd_epi16(E11, E41);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E50);
            S01 = _mm_madd_epi16(E11, E51);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E60);
            S01 = _mm_madd_epi16(E11, E61);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E70);
            S01 = _mm_madd_epi16(E11, E71);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R4 = _mm_add_epi32(R4, S0);

            S00 = _mm_madd_epi16(E10, E80);
            S01 = _mm_madd_epi16(E11, E81);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E20);
            S01 = _mm_madd_epi16(E21, E21);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E30);
            S01 = _mm_madd_epi16(E21, E31);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E40);
            S01 = _mm_madd_epi16(E21, E41);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R5 = _mm_add_epi32(R5, S0);

            S00 = _mm_madd_epi16(E20, E50);
            S01 = _mm_madd_epi16(E21, E51);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E60);
            S01 = _mm_madd_epi16(E21, E61);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E70);
            S01 = _mm_madd_epi16(E21, E71);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E80);
            S01 = _mm_madd_epi16(E21, E81);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R6 = _mm_add_epi32(R6, S0);

            S00 = _mm_madd_epi16(E30, E30);
            S01 = _mm_madd_epi16(E31, E31);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E40);
            S01 = _mm_madd_epi16(E31, E41);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E50);
            S01 = _mm_madd_epi16(E31, E51);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E60);
            S01 = _mm_madd_epi16(E31, E61);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R7 = _mm_add_epi32(R7, S0);

            S00 = _mm_madd_epi16(E30, E70);
            S01 = _mm_madd_epi16(E31, E71);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E80);
            S01 = _mm_madd_epi16(E31, E81);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E40);
            S01 = _mm_madd_epi16(E41, E41);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E50);
            S01 = _mm_madd_epi16(E41, E51);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R8 = _mm_add_epi32(R8, S0);

            S00 = _mm_madd_epi16(E40, E60);
            S01 = _mm_madd_epi16(E41, E61);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E70);
            S01 = _mm_madd_epi16(E41, E71);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E80);
            S01 = _mm_madd_epi16(E41, E81);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, E50);
            S01 = _mm_madd_epi16(E51, E51);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R9 = _mm_add_epi32(R9, S0);

            S00 = _mm_madd_epi16(E50, E60);
            S01 = _mm_madd_epi16(E51, E61);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, E70);
            S01 = _mm_madd_epi16(E51, E71);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, E80);
            S01 = _mm_madd_epi16(E51, E81);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E60, E60);
            S01 = _mm_madd_epi16(E61, E61);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R10 = _mm_add_epi32(R10, S0);

            S00 = _mm_madd_epi16(E60, E70);
            S01 = _mm_madd_epi16(E61, E71);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E60, E80);
            S01 = _mm_madd_epi16(E61, E81);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E70, E70);
            S01 = _mm_madd_epi16(E71, E71);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E70, E80);
            S01 = _mm_madd_epi16(E71, E81);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R11 = _mm_add_epi32(R11, S0);

            S00 = _mm_madd_epi16(E80, E80);
            S01 = _mm_madd_epi16(E81, E81);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, C0);
            S01 = _mm_madd_epi16(E01, C1);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, C0);
            S01 = _mm_madd_epi16(E11, C1);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, C0);
            S01 = _mm_madd_epi16(E21, C1);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R12 = _mm_add_epi32(R12, S0);

            S00 = _mm_madd_epi16(E30, C0);
            S01 = _mm_madd_epi16(E31, C1);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, C0);
            S01 = _mm_madd_epi16(E41, C1);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, C0);
            S01 = _mm_madd_epi16(E51, C1);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E60, C0);
            S01 = _mm_madd_epi16(E61, C1);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            R13 = _mm_add_epi32(R13, S0);

            S00 = _mm_madd_epi16(E70, C0);
            S01 = _mm_madd_epi16(E71, C1);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E80, C0);
            S01 = _mm_madd_epi16(E81, C1);
            S1 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            R14 = _mm_add_epi32(R14, S0);
        }

        imgPad += i_alf;
        imgOrg += i_org;
    }

    eCorr[0][0] = (u32)_mm_extract_epi32(R1, 0);
    eCorr[0][1] = (u32)_mm_extract_epi32(R1, 1);
    eCorr[0][2] = (u32)_mm_extract_epi32(R1, 2);
    eCorr[0][3] = (u32)_mm_extract_epi32(R1, 3);

    eCorr[0][4] = (u32)_mm_extract_epi32(R2, 0);
    eCorr[0][5] = (u32)_mm_extract_epi32(R2, 1);
    eCorr[0][6] = (u32)_mm_extract_epi32(R2, 2);
    eCorr[0][7] = (u32)_mm_extract_epi32(R2, 3);

    eCorr[0][8] = (u32)_mm_extract_epi32(R3, 0);
    eCorr[1][1] = (u32)_mm_extract_epi32(R3, 1);
    eCorr[1][2] = (u32)_mm_extract_epi32(R3, 2);
    eCorr[1][3] = (u32)_mm_extract_epi32(R3, 3);

    eCorr[1][4] = (u32)_mm_extract_epi32(R4, 0);
    eCorr[1][5] = (u32)_mm_extract_epi32(R4, 1);
    eCorr[1][6] = (u32)_mm_extract_epi32(R4, 2);
    eCorr[1][7] = (u32)_mm_extract_epi32(R4, 3);

    eCorr[1][8] = (u32)_mm_extract_epi32(R5, 0);
    eCorr[2][2] = (u32)_mm_extract_epi32(R5, 1);
    eCorr[2][3] = (u32)_mm_extract_epi32(R5, 2);
    eCorr[2][4] = (u32)_mm_extract_epi32(R5, 3);

    eCorr[2][5] = (u32)_mm_extract_epi32(R6, 0);
    eCorr[2][6] = (u32)_mm_extract_epi32(R6, 1);
    eCorr[2][7] = (u32)_mm_extract_epi32(R6, 2);
    eCorr[2][8] = (u32)_mm_extract_epi32(R6, 3);

    eCorr[3][3] = (u32)_mm_extract_epi32(R7, 0);
    eCorr[3][4] = (u32)_mm_extract_epi32(R7, 1);
    eCorr[3][5] = (u32)_mm_extract_epi32(R7, 2);
    eCorr[3][6] = (u32)_mm_extract_epi32(R7, 3);

    eCorr[3][7] = (u32)_mm_extract_epi32(R8, 0);
    eCorr[3][8] = (u32)_mm_extract_epi32(R8, 1);
    eCorr[4][4] = (u32)_mm_extract_epi32(R8, 2);
    eCorr[4][5] = (u32)_mm_extract_epi32(R8, 3);

    eCorr[4][6] = (u32)_mm_extract_epi32(R9, 0);
    eCorr[4][7] = (u32)_mm_extract_epi32(R9, 1);
    eCorr[4][8] = (u32)_mm_extract_epi32(R9, 2);
    eCorr[5][5] = (u32)_mm_extract_epi32(R9, 3);

    eCorr[5][6] = (u32)_mm_extract_epi32(R10, 0);
    eCorr[5][7] = (u32)_mm_extract_epi32(R10, 1);
    eCorr[5][8] = (u32)_mm_extract_epi32(R10, 2);
    eCorr[6][6] = (u32)_mm_extract_epi32(R10, 3);

    eCorr[6][7] = (u32)_mm_extract_epi32(R11, 0);
    eCorr[6][8] = (u32)_mm_extract_epi32(R11, 1);
    eCorr[7][7] = (u32)_mm_extract_epi32(R11, 2);
    eCorr[7][8] = (u32)_mm_extract_epi32(R11, 3);

    eCorr[8][8] = (u32)_mm_extract_epi32(R12, 0);
    yCorr[0] = (u32)_mm_extract_epi32(R12, 1);
    yCorr[1] = (u32)_mm_extract_epi32(R12, 2);
    yCorr[2] = (u32)_mm_extract_epi32(R12, 3);

    yCorr[3] = (u32)_mm_extract_epi32(R13, 0);
    yCorr[4] = (u32)_mm_extract_epi32(R13, 1);
    yCorr[5] = (u32)_mm_extract_epi32(R13, 2);
    yCorr[6] = (u32)_mm_extract_epi32(R13, 3);

    R14 = _mm_hadd_epi32(R14, R14);

    yCorr[7] = (u32)_mm_extract_epi32(R14, 0);
    yCorr[8] = (u32)_mm_extract_epi32(R14, 1);

    for (k = 1; k < ALF_MAX_NUM_COEF; k++) {
        for (l = 0; l < k; l++) {
            eCorr[k][l] = eCorr[l][k];
        }
    }
}

#else

void uavs3e_alf_one_lcu_sse(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;

    __m128i T00, T01, T10, T11, T20, T21, T30, T31, T40, T41;
    __m128i E00, E01, E10, E11, E20, E21, E30, E31, E40, E41;
    __m128i C0, C1, C2, C3, C4, C5, C6, C7, C8;
    __m128i S00, S01, S10, S11, S20, S21, S30, S31, S40, S41, S50, S51, S60, S61, SS1, SS2, S, S70, S71, S80, S81;
    __m128i mAddOffset;
    __m128i zero = _mm_setzero_si128();
    int max_pixel = (1 << sample_bit_depth) - 1;
    __m128i max_val = _mm_set1_epi16(max_pixel);

    int i, j;

    C0 = _mm_set1_epi16((pel)coef[0]);
    C1 = _mm_set1_epi16((pel)coef[1]);
    C2 = _mm_set1_epi16((pel)coef[2]);
    C3 = _mm_set1_epi16((pel)coef[3]);
    C4 = _mm_set1_epi16((pel)coef[4]);
    C5 = _mm_set1_epi16((pel)coef[5]);
    C6 = _mm_set1_epi16((pel)coef[6]);
    C7 = _mm_set1_epi16((pel)coef[7]);
    C8 = _mm_set1_epi16((pel)coef[8]);

    mAddOffset = _mm_set1_epi32(32);

    for (i = 0; i < lcu_height; i++) {
        imgPad1 = src + i_src;
        imgPad2 = src - i_src;
        imgPad3 = src + 2 * i_src;
        imgPad4 = src - 2 * i_src;
        imgPad5 = src + 3 * i_src;
        imgPad6 = src - 3 * i_src;
        if (i < 3) {
            if (i == 0) {
                imgPad4 = imgPad2 = src;
            } else if (i == 1) {
                imgPad4 = imgPad2;
            }
            imgPad6 = imgPad4;
        } else if (i > lcu_height - 4) {
            if (i == lcu_height - 1) {
                imgPad3 = imgPad1 = src;
            } else if (i == lcu_height - 2) {
                imgPad3 = imgPad1;
            }
            imgPad5 = imgPad3;
        }

        for (j = 0; j < lcu_width; j += 8) {
            T00 = _mm_load_si128((__m128i *)&imgPad6[j]);
            T01 = _mm_load_si128((__m128i *)&imgPad5[j]);
            E00 = _mm_unpacklo_epi16(T00, T01);
            E01 = _mm_unpackhi_epi16(T00, T01);
            S00 = _mm_madd_epi16(E00, C0);
            S01 = _mm_madd_epi16(E01, C0);

            T10 = _mm_load_si128((__m128i *)&imgPad4[j]);
            T11 = _mm_load_si128((__m128i *)&imgPad3[j]);
            E10 = _mm_unpacklo_epi16(T10, T11);
            E11 = _mm_unpackhi_epi16(T10, T11);
            S10 = _mm_madd_epi16(E10, C1);
            S11 = _mm_madd_epi16(E11, C1);

            T20 = _mm_loadu_si128((__m128i *)&imgPad2[j - 1]);
            T21 = _mm_loadu_si128((__m128i *)&imgPad1[j + 1]);
            E20 = _mm_unpacklo_epi16(T20, T21);
            E21 = _mm_unpackhi_epi16(T20, T21);
            S20 = _mm_madd_epi16(E20, C2);
            S21 = _mm_madd_epi16(E21, C2);

            T30 = _mm_load_si128((__m128i *)&imgPad2[j]);
            T31 = _mm_load_si128((__m128i *)&imgPad1[j]);
            E30 = _mm_unpacklo_epi16(T30, T31);
            E31 = _mm_unpackhi_epi16(T30, T31);
            S30 = _mm_madd_epi16(E30, C3);
            S31 = _mm_madd_epi16(E31, C3);

            T40 = _mm_loadu_si128((__m128i *)&imgPad2[j + 1]);
            T41 = _mm_loadu_si128((__m128i *)&imgPad1[j - 1]);
            E40 = _mm_unpacklo_epi16(T40, T41);
            E41 = _mm_unpackhi_epi16(T40, T41);
            S40 = _mm_madd_epi16(E40, C4);
            S41 = _mm_madd_epi16(E41, C4);

            T40 = _mm_loadu_si128((__m128i *)&src[j - 3]);
            T41 = _mm_loadu_si128((__m128i *)&src[j + 3]);
            E40 = _mm_unpacklo_epi16(T40, T41);
            E41 = _mm_unpackhi_epi16(T40, T41);
            S50 = _mm_madd_epi16(E40, C5);
            S51 = _mm_madd_epi16(E41, C5);

            T40 = _mm_loadu_si128((__m128i *)&src[j - 2]);
            T41 = _mm_loadu_si128((__m128i *)&src[j + 2]);
            E40 = _mm_unpacklo_epi16(T40, T41);
            E41 = _mm_unpackhi_epi16(T40, T41);
            S60 = _mm_madd_epi16(E40, C6);
            S61 = _mm_madd_epi16(E41, C6);

            T40 = _mm_loadu_si128((__m128i *)&src[j - 1]);
            T41 = _mm_loadu_si128((__m128i *)&src[j + 1]);
            E40 = _mm_unpacklo_epi16(T40, T41);
            E41 = _mm_unpackhi_epi16(T40, T41);
            S70 = _mm_madd_epi16(E40, C7);
            S71 = _mm_madd_epi16(E41, C7);

            T40 = _mm_load_si128((__m128i *)&src[j]);
            E40 = _mm_unpacklo_epi16(T40, zero);
            E41 = _mm_unpackhi_epi16(T40, zero);
            S80 = _mm_madd_epi16(E40, C8);
            S81 = _mm_madd_epi16(E41, C8);

            SS1 = _mm_add_epi32(S00, S10);
            SS1 = _mm_add_epi32(SS1, S20);
            SS1 = _mm_add_epi32(SS1, S30);
            SS1 = _mm_add_epi32(SS1, S40);
            SS1 = _mm_add_epi32(SS1, S50);
            SS1 = _mm_add_epi32(SS1, S60);
            SS1 = _mm_add_epi32(SS1, S70);
            SS1 = _mm_add_epi32(SS1, S80);

            SS2 = _mm_add_epi32(S01, S11);
            SS2 = _mm_add_epi32(SS2, S21);
            SS2 = _mm_add_epi32(SS2, S31);
            SS2 = _mm_add_epi32(SS2, S41);
            SS2 = _mm_add_epi32(SS2, S51);
            SS2 = _mm_add_epi32(SS2, S61);
            SS2 = _mm_add_epi32(SS2, S71);
            SS2 = _mm_add_epi32(SS2, S81);

            SS1 = _mm_add_epi32(SS1, mAddOffset);
            SS1 = _mm_srai_epi32(SS1, 6);

            SS2 = _mm_add_epi32(SS2, mAddOffset);
            SS2 = _mm_srai_epi32(SS2, 6);

            S = _mm_packus_epi32(SS1, SS2);
            S = _mm_min_epu16(S, max_val);

            _mm_storeu_si128((__m128i *)(dst + j), S);

        }

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_alf_calc_corr_sse(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail)
{
    __m128i E00, E01, E10, E11, E20, E21, E30, E31, E40, E41, E50, E51, E60, E61, E70, E71, E80, E81, E0, E1, E2, E3;
    __m128i C0, C1;
    __m128i S0, S1, S2, S3, S00, S01;
    __m128i mZero = _mm_set1_epi16(0);
    __m128i R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14;
    __m128i M1, M2, M3, M4, M5, M6, M7, M8, M9, M10, M11, M12, M13, M14;
    __m128i C;

    int xPosEnd = xPos + width;
    int startPos = isAboveAvail ? (yPos - 4) : yPos;
    int endPos = isBelowAvail ? (yPos + height - 4) : (yPos + height);

    int yUp, yBottom;

    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;
    int i, j, k, l;

    pel *imgPad = p_alf + startPos * i_alf;
    pel *imgOrg = p_org + startPos * i_org;

    R1 = R2 = R3 = R4 = R5 = R6 = R7 = R8 = R9 = R10 = R11 = R12 = R13 = R14 = mZero;
    M1 = M2 = M3 = M4 = M5 = M6 = M7 = M8 = M9 = M10 = M11 = M12 = M13 = M14 = mZero;

    for (i = startPos; i < endPos; i++) {
        yUp = COM_CLIP3(startPos, endPos - 1, i - 1);
        yBottom = COM_CLIP3(startPos, endPos - 1, i + 1);
        imgPad1 = imgPad + (yBottom - i) * i_alf;
        imgPad2 = imgPad + (yUp - i) * i_alf;

        yUp = COM_CLIP3(startPos, endPos - 1, i - 2);
        yBottom = COM_CLIP3(startPos, endPos - 1, i + 2);
        imgPad3 = imgPad + (yBottom - i) * i_alf;
        imgPad4 = imgPad + (yUp - i) * i_alf;

        yUp = COM_CLIP3(startPos, endPos - 1, i - 3);
        yBottom = COM_CLIP3(startPos, endPos - 1, i + 3);
        imgPad5 = imgPad + (yBottom - i) * i_alf;
        imgPad6 = imgPad + (yUp - i) * i_alf;

        for (j = xPos; j < xPosEnd; j += 16) {
            E0 = _mm_loadu_si128((__m128i*)&imgPad6[j]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad6[j + 8]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad5[j]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad5[j + 8]);
            E00 = _mm_add_epi16(E0, E2);
            E01 = _mm_add_epi16(E1, E3);

            E0 = _mm_loadu_si128((__m128i*)&imgPad4[j]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad4[j + 8]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad3[j]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad3[j + 8]);
            E10 = _mm_add_epi16(E0, E2);
            E11 = _mm_add_epi16(E1, E3);

            E0 = _mm_loadu_si128((__m128i*)&imgPad2[j - 1]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad2[j + 7]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad1[j + 1]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad1[j + 9]);
            E20 = _mm_add_epi16(E0, E2);
            E21 = _mm_add_epi16(E1, E3);

            E0 = _mm_loadu_si128((__m128i*)&imgPad2[j]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad2[j + 8]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad1[j]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad1[j + 8]);
            E30 = _mm_add_epi16(E0, E2);
            E31 = _mm_add_epi16(E1, E3);

            E0 = _mm_loadu_si128((__m128i*)&imgPad2[j + 1]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad2[j + 9]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad1[j - 1]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad1[j + 7]);
            E40 = _mm_add_epi16(E0, E2);
            E41 = _mm_add_epi16(E1, E3);

            E0 = _mm_loadu_si128((__m128i*)&imgPad[j - 3]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad[j + 5]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad[j + 3]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad[j + 11]);
            E50 = _mm_add_epi16(E0, E2);
            E51 = _mm_add_epi16(E1, E3);

            E0 = _mm_loadu_si128((__m128i*)&imgPad[j - 2]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad[j + 6]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad[j + 2]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad[j + 10]);
            E60 = _mm_add_epi16(E0, E2);
            E61 = _mm_add_epi16(E1, E3);

            E0 = _mm_loadu_si128((__m128i*)&imgPad[j - 1]);
            E1 = _mm_loadu_si128((__m128i*)&imgPad[j + 7]);
            E2 = _mm_loadu_si128((__m128i*)&imgPad[j + 1]);
            E3 = _mm_loadu_si128((__m128i*)&imgPad[j + 9]);
            E70 = _mm_add_epi16(E0, E2);
            E71 = _mm_add_epi16(E1, E3);

            E80 = _mm_loadu_si128((__m128i*)&imgPad[j]);
            E81 = _mm_loadu_si128((__m128i*)&imgPad[j + 8]);

            C0 = _mm_loadu_si128((__m128i*)&imgOrg[j]);
            C1 = _mm_loadu_si128((__m128i*)&imgOrg[j + 8]);

            S00 = _mm_madd_epi16(E00, E00);
            S01 = _mm_madd_epi16(E01, E01);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E10);
            S01 = _mm_madd_epi16(E01, E11);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E20);
            S01 = _mm_madd_epi16(E01, E21);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E30);
            S01 = _mm_madd_epi16(E01, E31);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R1 = _mm_add_epi64(R1, C);
            M1 = _mm_add_epi64(M1, S0);

            S00 = _mm_madd_epi16(E00, E40);
            S01 = _mm_madd_epi16(E01, E41);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E50);
            S01 = _mm_madd_epi16(E01, E51);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E60);
            S01 = _mm_madd_epi16(E01, E61);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, E70);
            S01 = _mm_madd_epi16(E01, E71);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R2 = _mm_add_epi64(R2, C);
            M2 = _mm_add_epi64(M2, S0);

            S00 = _mm_madd_epi16(E00, E80);
            S01 = _mm_madd_epi16(E01, E81);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E10);
            S01 = _mm_madd_epi16(E11, E11);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E20);
            S01 = _mm_madd_epi16(E11, E21);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E30);
            S01 = _mm_madd_epi16(E11, E31);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R3 = _mm_add_epi64(R3, C);
            M3 = _mm_add_epi64(M3, S0);

            S00 = _mm_madd_epi16(E10, E40);
            S01 = _mm_madd_epi16(E11, E41);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E50);
            S01 = _mm_madd_epi16(E11, E51);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E60);
            S01 = _mm_madd_epi16(E11, E61);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, E70);
            S01 = _mm_madd_epi16(E11, E71);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R4 = _mm_add_epi64(R4, C);
            M4 = _mm_add_epi64(M4, S0);

            S00 = _mm_madd_epi16(E10, E80);
            S01 = _mm_madd_epi16(E11, E81);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E20);
            S01 = _mm_madd_epi16(E21, E21);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E30);
            S01 = _mm_madd_epi16(E21, E31);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E40);
            S01 = _mm_madd_epi16(E21, E41);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R5 = _mm_add_epi64(R5, C);
            M5 = _mm_add_epi64(M5, S0);

            S00 = _mm_madd_epi16(E20, E50);
            S01 = _mm_madd_epi16(E21, E51);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E60);
            S01 = _mm_madd_epi16(E21, E61);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E70);
            S01 = _mm_madd_epi16(E21, E71);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, E80);
            S01 = _mm_madd_epi16(E21, E81);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R6 = _mm_add_epi64(R6, C);
            M6 = _mm_add_epi64(M6, S0);

            S00 = _mm_madd_epi16(E30, E30);
            S01 = _mm_madd_epi16(E31, E31);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E40);
            S01 = _mm_madd_epi16(E31, E41);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E50);
            S01 = _mm_madd_epi16(E31, E51);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E60);
            S01 = _mm_madd_epi16(E31, E61);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R7 = _mm_add_epi64(R7, C);
            M7 = _mm_add_epi64(M7, S0);

            S00 = _mm_madd_epi16(E30, E70);
            S01 = _mm_madd_epi16(E31, E71);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E30, E80);
            S01 = _mm_madd_epi16(E31, E81);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E40);
            S01 = _mm_madd_epi16(E41, E41);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E50);
            S01 = _mm_madd_epi16(E41, E51);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R8 = _mm_add_epi64(R8, C);
            M8 = _mm_add_epi64(M8, S0);

            S00 = _mm_madd_epi16(E40, E60);
            S01 = _mm_madd_epi16(E41, E61);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E70);
            S01 = _mm_madd_epi16(E41, E71);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, E80);
            S01 = _mm_madd_epi16(E41, E81);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, E50);
            S01 = _mm_madd_epi16(E51, E51);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R9 = _mm_add_epi64(R9, C);
            M9 = _mm_add_epi64(M9, S0);

            S00 = _mm_madd_epi16(E50, E60);
            S01 = _mm_madd_epi16(E51, E61);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, E70);
            S01 = _mm_madd_epi16(E51, E71);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, E80);
            S01 = _mm_madd_epi16(E51, E81);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E60, E60);
            S01 = _mm_madd_epi16(E61, E61);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R10 = _mm_add_epi64(R10, C);
            M10 = _mm_add_epi64(M10, S0);

            S00 = _mm_madd_epi16(E60, E70);
            S01 = _mm_madd_epi16(E61, E71);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E60, E80);
            S01 = _mm_madd_epi16(E61, E81);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E70, E70);
            S01 = _mm_madd_epi16(E71, E71);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E70, E80);
            S01 = _mm_madd_epi16(E71, E81);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R11 = _mm_add_epi64(R11, C);
            M11 = _mm_add_epi64(M11, S0);

            S00 = _mm_madd_epi16(E80, E80);
            S01 = _mm_madd_epi16(E81, E81);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E00, C0);
            S01 = _mm_madd_epi16(E01, C1);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E10, C0);
            S01 = _mm_madd_epi16(E11, C1);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E20, C0);
            S01 = _mm_madd_epi16(E21, C1);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R12 = _mm_add_epi64(R12, C);
            M12 = _mm_add_epi64(M12, S0);

            S00 = _mm_madd_epi16(E30, C0);
            S01 = _mm_madd_epi16(E31, C1);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E40, C0);
            S01 = _mm_madd_epi16(E41, C1);
            S1 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E50, C0);
            S01 = _mm_madd_epi16(E51, C1);
            S2 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E60, C0);
            S01 = _mm_madd_epi16(E61, C1);
            S3 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            S2 = _mm_hadd_epi32(S2, S3);
            S0 = _mm_hadd_epi32(S0, S2);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R13 = _mm_add_epi64(R13, C);
            M13 = _mm_add_epi64(M13, S0);

            S00 = _mm_madd_epi16(E70, C0);
            S01 = _mm_madd_epi16(E71, C1);
            S0 = _mm_add_epi32(S00, S01);

            S00 = _mm_madd_epi16(E80, C0);
            S01 = _mm_madd_epi16(E81, C1);
            S1 = _mm_add_epi32(S00, S01);

            S0 = _mm_hadd_epi32(S0, S1);
            C = _mm_cvtepi32_epi64(S0);
            S0 = _mm_srli_si128(S0, 8);
            S0 = _mm_cvtepi32_epi64(S0);
            R14 = _mm_add_epi64(R14, C);
            M14 = _mm_add_epi64(M14, S0);
        }

        imgPad += i_alf;
        imgOrg += i_org;
    }

    eCorr[0][0] = (double)_mm_extract_epi64(R1, 0);
    eCorr[0][1] = (double)_mm_extract_epi64(R1, 1);
    eCorr[0][2] = (double)_mm_extract_epi64(M1, 0);
    eCorr[0][3] = (double)_mm_extract_epi64(M1, 1);

    eCorr[0][4] = (double)_mm_extract_epi64(R2, 0);
    eCorr[0][5] = (double)_mm_extract_epi64(R2, 1);
    eCorr[0][6] = (double)_mm_extract_epi64(M2, 0);
    eCorr[0][7] = (double)_mm_extract_epi64(M2, 1);

    eCorr[0][8] = (double)_mm_extract_epi64(R3, 0);
    eCorr[1][1] = (double)_mm_extract_epi64(R3, 1);
    eCorr[1][2] = (double)_mm_extract_epi64(M3, 0);
    eCorr[1][3] = (double)_mm_extract_epi64(M3, 1);

    eCorr[1][4] = (double)_mm_extract_epi64(R4, 0);
    eCorr[1][5] = (double)_mm_extract_epi64(R4, 1);
    eCorr[1][6] = (double)_mm_extract_epi64(M4, 0);
    eCorr[1][7] = (double)_mm_extract_epi64(M4, 1);

    eCorr[1][8] = (double)_mm_extract_epi64(R5, 0);
    eCorr[2][2] = (double)_mm_extract_epi64(R5, 1);
    eCorr[2][3] = (double)_mm_extract_epi64(M5, 0);
    eCorr[2][4] = (double)_mm_extract_epi64(M5, 1);

    eCorr[2][5] = (double)_mm_extract_epi64(R6, 0);
    eCorr[2][6] = (double)_mm_extract_epi64(R6, 1);
    eCorr[2][7] = (double)_mm_extract_epi64(M6, 0);
    eCorr[2][8] = (double)_mm_extract_epi64(M6, 1);

    eCorr[3][3] = (double)_mm_extract_epi64(R7, 0);
    eCorr[3][4] = (double)_mm_extract_epi64(R7, 1);
    eCorr[3][5] = (double)_mm_extract_epi64(M7, 0);
    eCorr[3][6] = (double)_mm_extract_epi64(M7, 1);

    eCorr[3][7] = (double)_mm_extract_epi64(R8, 0);
    eCorr[3][8] = (double)_mm_extract_epi64(R8, 1);
    eCorr[4][4] = (double)_mm_extract_epi64(M8, 0);
    eCorr[4][5] = (double)_mm_extract_epi64(M8, 1);

    eCorr[4][6] = (double)_mm_extract_epi64(R9, 0);
    eCorr[4][7] = (double)_mm_extract_epi64(R9, 1);
    eCorr[4][8] = (double)_mm_extract_epi64(M9, 0);
    eCorr[5][5] = (double)_mm_extract_epi64(M9, 1);

    eCorr[5][6] = (double)_mm_extract_epi64(R10, 0);
    eCorr[5][7] = (double)_mm_extract_epi64(R10, 1);
    eCorr[5][8] = (double)_mm_extract_epi64(M10, 0);
    eCorr[6][6] = (double)_mm_extract_epi64(M10, 1);

    eCorr[6][7] = (double)_mm_extract_epi64(R11, 0);
    eCorr[6][8] = (double)_mm_extract_epi64(R11, 1);
    eCorr[7][7] = (double)_mm_extract_epi64(M11, 0);
    eCorr[7][8] = (double)_mm_extract_epi64(M11, 1);

    eCorr[8][8] = (double)_mm_extract_epi64(R12, 0);
    yCorr[0] = (double)_mm_extract_epi64(R12, 1);
    yCorr[1] = (double)_mm_extract_epi64(M12, 0);
    yCorr[2] = (double)_mm_extract_epi64(M12, 1);

    yCorr[3] = (double)_mm_extract_epi64(R13, 0);
    yCorr[4] = (double)_mm_extract_epi64(R13, 1);
    yCorr[5] = (double)_mm_extract_epi64(M13, 0);
    yCorr[6] = (double)_mm_extract_epi64(M13, 1);

    yCorr[7] = (double)(_mm_extract_epi64(R14, 0) + _mm_extract_epi64(R14, 1));
    yCorr[8] = (double)(_mm_extract_epi64(M14, 0) + _mm_extract_epi64(M14, 1));

    for (k = 1; k < ALF_MAX_NUM_COEF; k++) {
        for (l = 0; l < k; l++) {
            eCorr[k][l] = eCorr[l][k];
        }
    }
}

#endif
