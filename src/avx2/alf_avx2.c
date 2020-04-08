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

void uavs3e_alf_one_lcu_avx2(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;

    __m256i T00, T01, T10, T11, T20, T21, T30, T31, T40, T41, T50, T51, T60, T61, T70, T71, T80, T81, T82, T83;
    __m256i T8;
    __m256i T000, T001, T100, T101, T200, T201, T300, T301, T400, T401, T500, T501, T600, T601, T700, T701;
    __m256i C0, C1, C2, C3, C4, C5, C6, C7, C8;
    __m256i S0, S00, S01, S1, S10, S11, S2, S20, S21, S3, S30, S31, S4, S40, S41, S5, S50, S51, S6, S7, S60, S61, S70, S71, SS1, SS2, SS3, SS4, S;
    __m256i mAddOffset;
    __m256i mZero = _mm256_set1_epi16(0);
    __m256i mMax = _mm256_set1_epi16((short)((1 << sample_bit_depth) - 1));
    __m128i m0, m1;

    int i, j;
    int startPos = 0;
    int endPos = lcu_height;

    C0 = _mm256_set1_epi8(coef[0]);     // C0-C7: [-64, 63]
    C1 = _mm256_set1_epi8(coef[1]);
    C2 = _mm256_set1_epi8(coef[2]);
    C3 = _mm256_set1_epi8(coef[3]);
    C4 = _mm256_set1_epi8(coef[4]);
    C5 = _mm256_set1_epi8(coef[5]);
    C6 = _mm256_set1_epi8(coef[6]);
    C7 = _mm256_set1_epi8(coef[7]);
    C8 = _mm256_set1_epi32(coef[8]);    // [-1088, 1071]

    mAddOffset = _mm256_set1_epi32(32);

    for (i = startPos; i < endPos; i++) {
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

        for (j = 0; j < lcu_width; j += 32) {
            T00 = _mm256_loadu_si256((__m256i *)&imgPad6[j]);
            T01 = _mm256_loadu_si256((__m256i *)&imgPad5[j]);
            T000 = _mm256_unpacklo_epi8(T00, T01);
            T001 = _mm256_unpackhi_epi8(T00, T01);
            S00 = _mm256_maddubs_epi16(T000, C0);
            S01 = _mm256_maddubs_epi16(T001, C0);

            T10 = _mm256_loadu_si256((__m256i *)&imgPad4[j]);
            T11 = _mm256_loadu_si256((__m256i *)&imgPad3[j]);
            T100 = _mm256_unpacklo_epi8(T10, T11);
            T101 = _mm256_unpackhi_epi8(T10, T11);
            S10 = _mm256_maddubs_epi16(T100, C1);
            S11 = _mm256_maddubs_epi16(T101, C1);

            T20 = _mm256_loadu_si256((__m256i *)&imgPad2[j - 1]);
            T30 = _mm256_loadu_si256((__m256i *)&imgPad2[j]);
            T40 = _mm256_loadu_si256((__m256i *)&imgPad2[j + 1]);
            T41 = _mm256_loadu_si256((__m256i *)&imgPad1[j - 1]);
            T31 = _mm256_loadu_si256((__m256i *)&imgPad1[j]);
            T21 = _mm256_loadu_si256((__m256i *)&imgPad1[j + 1]);

            T200 = _mm256_unpacklo_epi8(T20, T21);
            T201 = _mm256_unpackhi_epi8(T20, T21);
            T300 = _mm256_unpacklo_epi8(T30, T31);
            T301 = _mm256_unpackhi_epi8(T30, T31);
            T400 = _mm256_unpacklo_epi8(T40, T41);
            T401 = _mm256_unpackhi_epi8(T40, T41);
            S20 = _mm256_maddubs_epi16(T200, C2);
            S21 = _mm256_maddubs_epi16(T201, C2);
            S30 = _mm256_maddubs_epi16(T300, C3);
            S31 = _mm256_maddubs_epi16(T301, C3);
            S40 = _mm256_maddubs_epi16(T400, C4);
            S41 = _mm256_maddubs_epi16(T401, C4);

            T50 = _mm256_loadu_si256((__m256i *)&src[j - 3]);
            T60 = _mm256_loadu_si256((__m256i *)&src[j - 2]);
            T70 = _mm256_loadu_si256((__m256i *)&src[j - 1]);
            T8 = _mm256_loadu_si256((__m256i *)&src[j]);
            T71 = _mm256_loadu_si256((__m256i *)&src[j + 1]);
            T61 = _mm256_loadu_si256((__m256i *)&src[j + 2]);
            T51 = _mm256_loadu_si256((__m256i *)&src[j + 3]);

            m0 = _mm256_castsi256_si128(T8);
            m1 = _mm256_extracti128_si256(T8, 1);

            T80 = _mm256_cvtepu8_epi32(m0);
            T81 = _mm256_cvtepu8_epi32(_mm_srli_si128(m0, 8));
            T82 = _mm256_cvtepu8_epi32(m1);
            T83 = _mm256_cvtepu8_epi32(_mm_srli_si128(m1, 8));
            T80 = _mm256_mullo_epi32(T80, C8);
            T81 = _mm256_mullo_epi32(T81, C8);
            T82 = _mm256_mullo_epi32(T82, C8);
            T83 = _mm256_mullo_epi32(T83, C8);

            T500 = _mm256_unpacklo_epi8(T50, T51);
            T501 = _mm256_unpackhi_epi8(T50, T51);
            T600 = _mm256_unpacklo_epi8(T60, T61);
            T601 = _mm256_unpackhi_epi8(T60, T61);
            T700 = _mm256_unpacklo_epi8(T70, T71);
            T701 = _mm256_unpackhi_epi8(T70, T71);
            S50 = _mm256_maddubs_epi16(T500, C5);
            S51 = _mm256_maddubs_epi16(T501, C5);
            S60 = _mm256_maddubs_epi16(T600, C6);
            S61 = _mm256_maddubs_epi16(T601, C6);
            S70 = _mm256_maddubs_epi16(T700, C7);
            S71 = _mm256_maddubs_epi16(T701, C7);

            S0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S00));
            S1 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S10));
            S2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S20));
            S3 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S30));
            S4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S40));
            S5 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S50));
            S6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S60));
            S7 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S70));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS1 = _mm256_add_epi32(S0, S4);
            SS1 = _mm256_add_epi32(SS1, T80);    //0-7

            S0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S00, 1));
            S1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S10, 1));
            S2 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S20, 1));
            S3 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S30, 1));
            S4 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S40, 1));
            S5 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S50, 1));
            S6 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S60, 1));
            S7 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S70, 1));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS2 = _mm256_add_epi32(S0, S4);
            SS2 = _mm256_add_epi32(SS2, T82);    //16-23

            S0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S01));
            S1 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S11));
            S2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S21));
            S3 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S31));
            S4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S41));
            S5 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S51));
            S6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S61));
            S7 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S71));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS3 = _mm256_add_epi32(S0, S4);
            SS3 = _mm256_add_epi32(SS3, T81);    //8-15

            S0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S01, 1));
            S1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S11, 1));
            S2 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S21, 1));
            S3 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S31, 1));
            S4 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S41, 1));
            S5 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S51, 1));
            S6 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S61, 1));
            S7 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S71, 1));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS4 = _mm256_add_epi32(S0, S4);
            SS4 = _mm256_add_epi32(SS4, T83);    //24-31

            SS1 = _mm256_add_epi32(SS1, mAddOffset);
            SS2 = _mm256_add_epi32(SS2, mAddOffset);
            SS3 = _mm256_add_epi32(SS3, mAddOffset);
            SS4 = _mm256_add_epi32(SS4, mAddOffset);

            SS1 = _mm256_srai_epi32(SS1, 6);
            SS2 = _mm256_srai_epi32(SS2, 6);
            SS3 = _mm256_srai_epi32(SS3, 6);
            SS4 = _mm256_srai_epi32(SS4, 6);

            SS1 = _mm256_packs_epi32(SS1, SS2);
            SS3 = _mm256_packs_epi32(SS3, SS4);
            SS1 = _mm256_permute4x64_epi64(SS1, 0xd8);
            SS3 = _mm256_permute4x64_epi64(SS3, 0xd8);

            SS1 = _mm256_min_epi16(SS1, mMax);
            SS1 = _mm256_max_epi16(SS1, mZero);
            SS3 = _mm256_min_epi16(SS3, mMax);
            SS3 = _mm256_max_epi16(SS3, mZero);

            S = _mm256_packus_epi16(SS1, SS3);
            _mm256_storeu_si256((__m256i *)(dst + j), S);
        }

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_alf_calc_corr_avx2(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail)
{
    __m256i E00, E10, E20, E30, E40, E50, E60, E70, E80;
    __m256i C0;
    __m256i S0, S1, S2, S3;
    __m256i R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14;
    __m256i mZero = _mm256_set1_epi16(0);

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
            E00 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad6[j])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad5[j])));
            E10 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad4[j])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad3[j])));
            E20 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad2[j - 1])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad1[j + 1])));
            E30 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad2[j])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad1[j])));
            E40 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad2[j + 1])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad1[j - 1])));
            E50 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad[j - 3])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad[j + 3])));
            E60 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad[j - 2])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad[j + 2])));
            E70 = _mm256_add_epi16(_mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad[j - 1])), _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad[j + 1])));
            E80 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgPad[j]));
            C0 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)&imgOrg[j]));

            S0 = _mm256_madd_epi16(E00, E00);
            S1 = _mm256_madd_epi16(E00, E10);
            S2 = _mm256_madd_epi16(E00, E20);
            S3 = _mm256_madd_epi16(E00, E30);
            R1 = _mm256_add_epi32(R1, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E00, E40);
            S1 = _mm256_madd_epi16(E00, E50);
            S2 = _mm256_madd_epi16(E00, E60);
            S3 = _mm256_madd_epi16(E00, E70);
            R2 = _mm256_add_epi32(R2, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E00, E80);
            S1 = _mm256_madd_epi16(E10, E10);
            S2 = _mm256_madd_epi16(E10, E20);
            S3 = _mm256_madd_epi16(E10, E30);
            R3 = _mm256_add_epi32(R3, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E10, E40);
            S1 = _mm256_madd_epi16(E10, E50);
            S2 = _mm256_madd_epi16(E10, E60);
            S3 = _mm256_madd_epi16(E10, E70);
            R4 = _mm256_add_epi32(R4, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E10, E80);
            S1 = _mm256_madd_epi16(E20, E20);
            S2 = _mm256_madd_epi16(E20, E30);
            S3 = _mm256_madd_epi16(E20, E40);
            R5 = _mm256_add_epi32(R5, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E20, E50);
            S1 = _mm256_madd_epi16(E20, E60);
            S2 = _mm256_madd_epi16(E20, E70);
            S3 = _mm256_madd_epi16(E20, E80);
            R6 = _mm256_add_epi32(R6, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E30, E30);
            S1 = _mm256_madd_epi16(E30, E40);
            S2 = _mm256_madd_epi16(E30, E50);
            S3 = _mm256_madd_epi16(E30, E60);
            R7 = _mm256_add_epi32(R7, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E30, E70);
            S1 = _mm256_madd_epi16(E30, E80);
            S2 = _mm256_madd_epi16(E40, E40);
            S3 = _mm256_madd_epi16(E40, E50);
            R8 = _mm256_add_epi32(R8, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E40, E60);
            S1 = _mm256_madd_epi16(E40, E70);
            S2 = _mm256_madd_epi16(E40, E80);
            S3 = _mm256_madd_epi16(E50, E50);
            R9 = _mm256_add_epi32(R9, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E50, E60);
            S1 = _mm256_madd_epi16(E50, E70);
            S2 = _mm256_madd_epi16(E50, E80);
            S3 = _mm256_madd_epi16(E60, E60);
            R10 = _mm256_add_epi32(R10, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E60, E70);
            S1 = _mm256_madd_epi16(E60, E80);
            S2 = _mm256_madd_epi16(E70, E70);
            S3 = _mm256_madd_epi16(E70, E80);
            R11 = _mm256_add_epi32(R11, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E80, E80);
            S1 = _mm256_madd_epi16(E00, C0);
            S2 = _mm256_madd_epi16(E10, C0);
            S3 = _mm256_madd_epi16(E20, C0);
            R12 = _mm256_add_epi32(R12, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E30, C0);
            S1 = _mm256_madd_epi16(E40, C0);
            S2 = _mm256_madd_epi16(E50, C0);
            S3 = _mm256_madd_epi16(E60, C0);
            R13 = _mm256_add_epi32(R13, _mm256_hadd_epi32(_mm256_hadd_epi32(S0, S1), _mm256_hadd_epi32(S2, S3)));

            S0 = _mm256_madd_epi16(E70, C0);
            S1 = _mm256_madd_epi16(E80, C0);

            S0 = _mm256_hadd_epi32(S0, S1);
            R14 = _mm256_add_epi32(R14, S0);
        }

        imgPad += i_alf;
        imgOrg += i_org;
    }

    R1 = _mm256_add_epi32(R1, _mm256_castsi128_si256(_mm256_extracti128_si256(R1, 1)));
    R2 = _mm256_add_epi32(R2, _mm256_castsi128_si256(_mm256_extracti128_si256(R2, 1)));
    R3 = _mm256_add_epi32(R3, _mm256_castsi128_si256(_mm256_extracti128_si256(R3, 1)));
    R4 = _mm256_add_epi32(R4, _mm256_castsi128_si256(_mm256_extracti128_si256(R4, 1)));
    R5 = _mm256_add_epi32(R5, _mm256_castsi128_si256(_mm256_extracti128_si256(R5, 1)));
    R6 = _mm256_add_epi32(R6, _mm256_castsi128_si256(_mm256_extracti128_si256(R6, 1)));
    R7 = _mm256_add_epi32(R7, _mm256_castsi128_si256(_mm256_extracti128_si256(R7, 1)));
    R8 = _mm256_add_epi32(R8, _mm256_castsi128_si256(_mm256_extracti128_si256(R8, 1)));
    R9 = _mm256_add_epi32(R9, _mm256_castsi128_si256(_mm256_extracti128_si256(R9, 1)));
    R10 = _mm256_add_epi32(R10, _mm256_castsi128_si256(_mm256_extracti128_si256(R10, 1)));
    R11 = _mm256_add_epi32(R11, _mm256_castsi128_si256(_mm256_extracti128_si256(R11, 1)));
    R12 = _mm256_add_epi32(R12, _mm256_castsi128_si256(_mm256_extracti128_si256(R12, 1)));
    R13 = _mm256_add_epi32(R13, _mm256_castsi128_si256(_mm256_extracti128_si256(R13, 1)));
    R14 = _mm256_add_epi32(R14, _mm256_castsi128_si256(_mm256_extracti128_si256(R14, 1)));

    eCorr[0][0] = (u32)_mm256_extract_epi32(R1, 0);
    eCorr[0][1] = (u32)_mm256_extract_epi32(R1, 1);
    eCorr[0][2] = (u32)_mm256_extract_epi32(R1, 2);
    eCorr[0][3] = (u32)_mm256_extract_epi32(R1, 3);

    eCorr[0][4] = (u32)_mm256_extract_epi32(R2, 0);
    eCorr[0][5] = (u32)_mm256_extract_epi32(R2, 1);
    eCorr[0][6] = (u32)_mm256_extract_epi32(R2, 2);
    eCorr[0][7] = (u32)_mm256_extract_epi32(R2, 3);

    eCorr[0][8] = (u32)_mm256_extract_epi32(R3, 0);
    eCorr[1][1] = (u32)_mm256_extract_epi32(R3, 1);
    eCorr[1][2] = (u32)_mm256_extract_epi32(R3, 2);
    eCorr[1][3] = (u32)_mm256_extract_epi32(R3, 3);

    eCorr[1][4] = (u32)_mm256_extract_epi32(R4, 0);
    eCorr[1][5] = (u32)_mm256_extract_epi32(R4, 1);
    eCorr[1][6] = (u32)_mm256_extract_epi32(R4, 2);
    eCorr[1][7] = (u32)_mm256_extract_epi32(R4, 3);

    eCorr[1][8] = (u32)_mm256_extract_epi32(R5, 0);
    eCorr[2][2] = (u32)_mm256_extract_epi32(R5, 1);
    eCorr[2][3] = (u32)_mm256_extract_epi32(R5, 2);
    eCorr[2][4] = (u32)_mm256_extract_epi32(R5, 3);

    eCorr[2][5] = (u32)_mm256_extract_epi32(R6, 0);
    eCorr[2][6] = (u32)_mm256_extract_epi32(R6, 1);
    eCorr[2][7] = (u32)_mm256_extract_epi32(R6, 2);
    eCorr[2][8] = (u32)_mm256_extract_epi32(R6, 3);

    eCorr[3][3] = (u32)_mm256_extract_epi32(R7, 0);
    eCorr[3][4] = (u32)_mm256_extract_epi32(R7, 1);
    eCorr[3][5] = (u32)_mm256_extract_epi32(R7, 2);
    eCorr[3][6] = (u32)_mm256_extract_epi32(R7, 3);

    eCorr[3][7] = (u32)_mm256_extract_epi32(R8, 0);
    eCorr[3][8] = (u32)_mm256_extract_epi32(R8, 1);
    eCorr[4][4] = (u32)_mm256_extract_epi32(R8, 2);
    eCorr[4][5] = (u32)_mm256_extract_epi32(R8, 3);

    eCorr[4][6] = (u32)_mm256_extract_epi32(R9, 0);
    eCorr[4][7] = (u32)_mm256_extract_epi32(R9, 1);
    eCorr[4][8] = (u32)_mm256_extract_epi32(R9, 2);
    eCorr[5][5] = (u32)_mm256_extract_epi32(R9, 3);

    eCorr[5][6] = (u32)_mm256_extract_epi32(R10, 0);
    eCorr[5][7] = (u32)_mm256_extract_epi32(R10, 1);
    eCorr[5][8] = (u32)_mm256_extract_epi32(R10, 2);
    eCorr[6][6] = (u32)_mm256_extract_epi32(R10, 3);

    eCorr[6][7] = (u32)_mm256_extract_epi32(R11, 0);
    eCorr[6][8] = (u32)_mm256_extract_epi32(R11, 1);
    eCorr[7][7] = (u32)_mm256_extract_epi32(R11, 2);
    eCorr[7][8] = (u32)_mm256_extract_epi32(R11, 3);

    eCorr[8][8] = (u32)_mm256_extract_epi32(R12, 0);
    yCorr[0] = (u32)_mm256_extract_epi32(R12, 1);
    yCorr[1] = (u32)_mm256_extract_epi32(R12, 2);
    yCorr[2] = (u32)_mm256_extract_epi32(R12, 3);

    yCorr[3] = (u32)_mm256_extract_epi32(R13, 0);
    yCorr[4] = (u32)_mm256_extract_epi32(R13, 1);
    yCorr[5] = (u32)_mm256_extract_epi32(R13, 2);
    yCorr[6] = (u32)_mm256_extract_epi32(R13, 3);

    R14 = _mm256_hadd_epi32(R14, R14);

    yCorr[7] = (u32)_mm256_extract_epi32(R14, 0);
    yCorr[8] = (u32)_mm256_extract_epi32(R14, 1);

    for (k = 1; k < ALF_MAX_NUM_COEF; k++) {
        for (l = 0; l < k; l++) {
            eCorr[k][l] = eCorr[l][k];
        }
    }
}

#else

void uavs3e_alf_one_lcu_avx2(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;

    __m256i T00, T01, T10, T11, T20, T21, T30, T31, T40, T41, T42, T43, T44, T45;
    __m256i E00, E01, E10, E11, E20, E21, E30, E31, E40, E41, E42, E43, E44, E45;
    __m256i C0, C1, C2, C3, C4, C5, C6, C7, C8;
    __m256i S00, S01, S10, S11, S20, S21, S30, S31, S40, S41, S50, S51, S60, S61, SS1, SS2, S, S70, S71, S80, S81;
    __m256i mAddOffset;
    __m256i zero = _mm256_setzero_si256();
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16(max_pixel);

    int i, j;
    int startPos = 0;
    int endPos = lcu_height;

    C0 = _mm256_set1_epi16((pel)coef[0]);
    C1 = _mm256_set1_epi16((pel)coef[1]);
    C2 = _mm256_set1_epi16((pel)coef[2]);
    C3 = _mm256_set1_epi16((pel)coef[3]);
    C4 = _mm256_set1_epi16((pel)coef[4]);
    C5 = _mm256_set1_epi16((pel)coef[5]);
    C6 = _mm256_set1_epi16((pel)coef[6]);
    C7 = _mm256_set1_epi16((pel)coef[7]);
    C8 = _mm256_set1_epi16((pel)coef[8]);

    mAddOffset = _mm256_set1_epi32(32);

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

        for (j = 0; j < lcu_width; j += 16) {
            T00 = _mm256_loadu_si256((__m256i *)&imgPad6[j]);
            T01 = _mm256_loadu_si256((__m256i *)&imgPad5[j]);
            T10 = _mm256_loadu_si256((__m256i *)&imgPad4[j]);
            T11 = _mm256_loadu_si256((__m256i *)&imgPad3[j]);
            E00 = _mm256_unpacklo_epi16(T00, T01);
            E01 = _mm256_unpackhi_epi16(T00, T01);
            E10 = _mm256_unpacklo_epi16(T10, T11);
            E11 = _mm256_unpackhi_epi16(T10, T11);

            S00 = _mm256_madd_epi16(E00, C0);//前8个像素所有C0*P0的结果
            S01 = _mm256_madd_epi16(E01, C0);//后8个像素所有C0*P0的结果
            S10 = _mm256_madd_epi16(E10, C1);//前8个像素所有C1*P1的结果
            S11 = _mm256_madd_epi16(E11, C1);//后8个像素所有C1*P1的结果

            T20 = _mm256_loadu_si256((__m256i *)&imgPad2[j - 1]);
            T21 = _mm256_loadu_si256((__m256i *)&imgPad1[j + 1]);
            T30 = _mm256_loadu_si256((__m256i *)&imgPad2[j]);
            T31 = _mm256_loadu_si256((__m256i *)&imgPad1[j]);
            T40 = _mm256_loadu_si256((__m256i *)&imgPad2[j + 1]);
            T41 = _mm256_loadu_si256((__m256i *)&imgPad1[j - 1]);

            E20 = _mm256_unpacklo_epi16(T20, T21);
            E21 = _mm256_unpackhi_epi16(T20, T21);
            E30 = _mm256_unpacklo_epi16(T30, T31);
            E31 = _mm256_unpackhi_epi16(T30, T31);
            E40 = _mm256_unpacklo_epi16(T40, T41);
            E41 = _mm256_unpackhi_epi16(T40, T41);

            S20 = _mm256_madd_epi16(E20, C2);
            S21 = _mm256_madd_epi16(E21, C2);
            S30 = _mm256_madd_epi16(E30, C3);
            S31 = _mm256_madd_epi16(E31, C3);
            S40 = _mm256_madd_epi16(E40, C4);
            S41 = _mm256_madd_epi16(E41, C4);

            T40 = _mm256_loadu_si256((__m256i *)&src[j - 3]);
            T41 = _mm256_loadu_si256((__m256i *)&src[j + 3]);
            T42 = _mm256_loadu_si256((__m256i *)&src[j - 2]);
            T43 = _mm256_loadu_si256((__m256i *)&src[j + 2]);
            T44 = _mm256_loadu_si256((__m256i *)&src[j - 1]);
            T45 = _mm256_loadu_si256((__m256i *)&src[j + 1]);

            E40 = _mm256_unpacklo_epi16(T40, T41);
            E41 = _mm256_unpackhi_epi16(T40, T41);
            E42 = _mm256_unpacklo_epi16(T42, T43);
            E43 = _mm256_unpackhi_epi16(T42, T43);
            E44 = _mm256_unpacklo_epi16(T44, T45);
            E45 = _mm256_unpackhi_epi16(T44, T45);

            S50 = _mm256_madd_epi16(E40, C5);
            S51 = _mm256_madd_epi16(E41, C5);
            S60 = _mm256_madd_epi16(E42, C6);
            S61 = _mm256_madd_epi16(E43, C6);
            S70 = _mm256_madd_epi16(E44, C7);
            S71 = _mm256_madd_epi16(E45, C7);

            T40 = _mm256_loadu_si256((__m256i *)&src[j]);
            E40 = _mm256_unpacklo_epi16(T40, zero);
            E41 = _mm256_unpackhi_epi16(T40, zero);
            S80 = _mm256_madd_epi16(E40, C8);
            S81 = _mm256_madd_epi16(E41, C8);

            SS1 = _mm256_add_epi32(S00, S10);
            SS1 = _mm256_add_epi32(SS1, S20);
            SS1 = _mm256_add_epi32(SS1, S30);
            SS1 = _mm256_add_epi32(SS1, S40);
            SS1 = _mm256_add_epi32(SS1, S50);
            SS1 = _mm256_add_epi32(SS1, S60);
            SS1 = _mm256_add_epi32(SS1, S70);
            SS1 = _mm256_add_epi32(SS1, S80);

            SS2 = _mm256_add_epi32(S01, S11);
            SS2 = _mm256_add_epi32(SS2, S21);
            SS2 = _mm256_add_epi32(SS2, S31);
            SS2 = _mm256_add_epi32(SS2, S41);
            SS2 = _mm256_add_epi32(SS2, S51);
            SS2 = _mm256_add_epi32(SS2, S61);
            SS2 = _mm256_add_epi32(SS2, S71);
            SS2 = _mm256_add_epi32(SS2, S81);

            SS1 = _mm256_add_epi32(SS1, mAddOffset);
            SS1 = _mm256_srai_epi32(SS1, 6);

            SS2 = _mm256_add_epi32(SS2, mAddOffset);
            SS2 = _mm256_srai_epi32(SS2, 6);

            S = _mm256_packus_epi32(SS1, SS2);
            S = _mm256_min_epu16(S, max_val);

            _mm256_store_si256((__m256i *)(dst + j), S);

        }

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_alf_calc_corr_avx2(pel *p_org, int i_org, pel *p_alf, int i_alf, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail)
{
    __m256i E00, E10, E20, E30, E40, E50, E60, E70, E80;
    __m256i C0, C1, C2;
    __m256i S0, S1, S2, S3;
    __m256i R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14;
    __m256i M1, M2, M3, M4, M5, M6, M7, M8, M9, M10, M11, M12, M13, M14;
    __m256i mZero = _mm256_set1_epi16(0);

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
            E00 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad6[j]), _mm256_loadu_si256((__m256i*)&imgPad5[j]));
            E10 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad4[j]), _mm256_loadu_si256((__m256i*)&imgPad3[j]));
            E20 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad2[j - 1]), _mm256_loadu_si256((__m256i*)&imgPad1[j + 1]));
            E30 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad2[j]), _mm256_loadu_si256((__m256i*)&imgPad1[j]));
            E40 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad2[j + 1]), _mm256_loadu_si256((__m256i*)&imgPad1[j - 1]));
            E50 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad[j - 3]), _mm256_loadu_si256((__m256i*)&imgPad[j + 3]));
            E60 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad[j - 2]), _mm256_loadu_si256((__m256i*)&imgPad[j + 2]));
            E70 = _mm256_add_epi16(_mm256_loadu_si256((__m256i*)&imgPad[j - 1]), _mm256_loadu_si256((__m256i*)&imgPad[j + 1]));
            E80 = _mm256_loadu_si256((__m256i*)&imgPad[j]);
            C0 = _mm256_loadu_si256((__m256i*)&imgOrg[j]);

            S0 = _mm256_madd_epi16(E00, E00);
            S1 = _mm256_madd_epi16(E00, E10);
            S2 = _mm256_madd_epi16(E00, E20);
            S3 = _mm256_madd_epi16(E00, E30);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R1 = _mm256_add_epi64(R1, C1);
            M1 = _mm256_add_epi64(M1, C2);

            S0 = _mm256_madd_epi16(E00, E40);
            S1 = _mm256_madd_epi16(E00, E50);
            S2 = _mm256_madd_epi16(E00, E60);
            S3 = _mm256_madd_epi16(E00, E70);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R2 = _mm256_add_epi64(R2, C1);
            M2 = _mm256_add_epi64(M2, C2);

            S0 = _mm256_madd_epi16(E00, E80);
            S1 = _mm256_madd_epi16(E10, E10);
            S2 = _mm256_madd_epi16(E10, E20);
            S3 = _mm256_madd_epi16(E10, E30);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R3 = _mm256_add_epi64(R3, C1);
            M3 = _mm256_add_epi64(M3, C2);

            S0 = _mm256_madd_epi16(E10, E40);
            S1 = _mm256_madd_epi16(E10, E50);
            S2 = _mm256_madd_epi16(E10, E60);
            S3 = _mm256_madd_epi16(E10, E70);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R4 = _mm256_add_epi64(R4, C1);
            M4 = _mm256_add_epi64(M4, C2);

            S0 = _mm256_madd_epi16(E10, E80);
            S1 = _mm256_madd_epi16(E20, E20);
            S2 = _mm256_madd_epi16(E20, E30);
            S3 = _mm256_madd_epi16(E20, E40);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R5 = _mm256_add_epi64(R5, C1);
            M5 = _mm256_add_epi64(M5, C2);

            S0 = _mm256_madd_epi16(E20, E50);
            S1 = _mm256_madd_epi16(E20, E60);
            S2 = _mm256_madd_epi16(E20, E70);
            S3 = _mm256_madd_epi16(E20, E80);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R6 = _mm256_add_epi64(R6, C1);
            M6 = _mm256_add_epi64(M6, C2);

            S0 = _mm256_madd_epi16(E30, E30);
            S1 = _mm256_madd_epi16(E30, E40);
            S2 = _mm256_madd_epi16(E30, E50);
            S3 = _mm256_madd_epi16(E30, E60);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R7 = _mm256_add_epi64(R7, C1);
            M7 = _mm256_add_epi64(M7, C2);

            S0 = _mm256_madd_epi16(E30, E70);
            S1 = _mm256_madd_epi16(E30, E80);
            S2 = _mm256_madd_epi16(E40, E40);
            S3 = _mm256_madd_epi16(E40, E50);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R8 = _mm256_add_epi64(R8, C1);
            M8 = _mm256_add_epi64(M8, C2);

            S0 = _mm256_madd_epi16(E40, E60);
            S1 = _mm256_madd_epi16(E40, E70);
            S2 = _mm256_madd_epi16(E40, E80);
            S3 = _mm256_madd_epi16(E50, E50);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R9 = _mm256_add_epi64(R9, C1);
            M9 = _mm256_add_epi64(M9, C2);

            S0 = _mm256_madd_epi16(E50, E60);
            S1 = _mm256_madd_epi16(E50, E70);
            S2 = _mm256_madd_epi16(E50, E80);
            S3 = _mm256_madd_epi16(E60, E60);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R10 = _mm256_add_epi64(R10, C1);
            M10 = _mm256_add_epi64(M10, C2);

            S0 = _mm256_madd_epi16(E60, E70);
            S1 = _mm256_madd_epi16(E60, E80);
            S2 = _mm256_madd_epi16(E70, E70);
            S3 = _mm256_madd_epi16(E70, E80);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R11 = _mm256_add_epi64(R11, C1);
            M11 = _mm256_add_epi64(M11, C2);

            S0 = _mm256_madd_epi16(E80, E80);
            S1 = _mm256_madd_epi16(E00, C0);
            S2 = _mm256_madd_epi16(E10, C0);
            S3 = _mm256_madd_epi16(E20, C0);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R12 = _mm256_add_epi64(R12, C1);
            M12 = _mm256_add_epi64(M12, C2);

            S0 = _mm256_madd_epi16(E30, C0);
            S1 = _mm256_madd_epi16(E40, C0);
            S2 = _mm256_madd_epi16(E50, C0);
            S3 = _mm256_madd_epi16(E60, C0);
            S0 = _mm256_hadd_epi32(S0, S1);
            S2 = _mm256_hadd_epi32(S2, S3);
            S0 = _mm256_hadd_epi32(S0, S2);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R13 = _mm256_add_epi64(R13, C1);
            M13 = _mm256_add_epi64(M13, C2);

            S0 = _mm256_madd_epi16(E70, C0);
            S1 = _mm256_madd_epi16(E80, C0);

            S0 = _mm256_hadd_epi32(S0, S1);
            C1 = _mm256_unpacklo_epi32(S0, mZero);
            C2 = _mm256_unpackhi_epi32(S0, mZero);
            R14 = _mm256_add_epi64(R14, C1);
            M14 = _mm256_add_epi64(M14, C2);
        }

        imgPad += i_alf;
        imgOrg += i_org;
    }

    eCorr[0][0] = (double)(_mm256_extract_epi64(R1, 0) + _mm256_extract_epi64(R1, 2));
    eCorr[0][1] = (double)(_mm256_extract_epi64(R1, 1) + _mm256_extract_epi64(R1, 3));
    eCorr[0][2] = (double)(_mm256_extract_epi64(M1, 0) + _mm256_extract_epi64(M1, 2));
    eCorr[0][3] = (double)(_mm256_extract_epi64(M1, 1) + _mm256_extract_epi64(M1, 3));

    eCorr[0][4] = (double)(_mm256_extract_epi64(R2, 0) + _mm256_extract_epi64(R2, 2));
    eCorr[0][5] = (double)(_mm256_extract_epi64(R2, 1) + _mm256_extract_epi64(R2, 3));
    eCorr[0][6] = (double)(_mm256_extract_epi64(M2, 0) + _mm256_extract_epi64(M2, 2));
    eCorr[0][7] = (double)(_mm256_extract_epi64(M2, 1) + _mm256_extract_epi64(M2, 3));

    eCorr[0][8] = (double)(_mm256_extract_epi64(R3, 0) + _mm256_extract_epi64(R3, 2));
    eCorr[1][1] = (double)(_mm256_extract_epi64(R3, 1) + _mm256_extract_epi64(R3, 3));
    eCorr[1][2] = (double)(_mm256_extract_epi64(M3, 0) + _mm256_extract_epi64(M3, 2));
    eCorr[1][3] = (double)(_mm256_extract_epi64(M3, 1) + _mm256_extract_epi64(M3, 3));

    eCorr[1][4] = (double)(_mm256_extract_epi64(R4, 0) + _mm256_extract_epi64(R4, 2));
    eCorr[1][5] = (double)(_mm256_extract_epi64(R4, 1) + _mm256_extract_epi64(R4, 3));
    eCorr[1][6] = (double)(_mm256_extract_epi64(M4, 0) + _mm256_extract_epi64(M4, 2));
    eCorr[1][7] = (double)(_mm256_extract_epi64(M4, 1) + _mm256_extract_epi64(M4, 3));

    eCorr[1][8] = (double)(_mm256_extract_epi64(R5, 0) + _mm256_extract_epi64(R5, 2));
    eCorr[2][2] = (double)(_mm256_extract_epi64(R5, 1) + _mm256_extract_epi64(R5, 3));
    eCorr[2][3] = (double)(_mm256_extract_epi64(M5, 0) + _mm256_extract_epi64(M5, 2));
    eCorr[2][4] = (double)(_mm256_extract_epi64(M5, 1) + _mm256_extract_epi64(M5, 3));

    eCorr[2][5] = (double)(_mm256_extract_epi64(R6, 0) + _mm256_extract_epi64(R6, 2));
    eCorr[2][6] = (double)(_mm256_extract_epi64(R6, 1) + _mm256_extract_epi64(R6, 3));
    eCorr[2][7] = (double)(_mm256_extract_epi64(M6, 0) + _mm256_extract_epi64(M6, 2));
    eCorr[2][8] = (double)(_mm256_extract_epi64(M6, 1) + _mm256_extract_epi64(M6, 3));

    eCorr[3][3] = (double)(_mm256_extract_epi64(R7, 0) + _mm256_extract_epi64(R7, 2));
    eCorr[3][4] = (double)(_mm256_extract_epi64(R7, 1) + _mm256_extract_epi64(R7, 3));
    eCorr[3][5] = (double)(_mm256_extract_epi64(M7, 0) + _mm256_extract_epi64(M7, 2));
    eCorr[3][6] = (double)(_mm256_extract_epi64(M7, 1) + _mm256_extract_epi64(M7, 3));

    eCorr[3][7] = (double)(_mm256_extract_epi64(R8, 0) + _mm256_extract_epi64(R8, 2));
    eCorr[3][8] = (double)(_mm256_extract_epi64(R8, 1) + _mm256_extract_epi64(R8, 3));
    eCorr[4][4] = (double)(_mm256_extract_epi64(M8, 0) + _mm256_extract_epi64(M8, 2));
    eCorr[4][5] = (double)(_mm256_extract_epi64(M8, 1) + _mm256_extract_epi64(M8, 3));

    eCorr[4][6] = (double)(_mm256_extract_epi64(R9, 0) + _mm256_extract_epi64(R9, 2));
    eCorr[4][7] = (double)(_mm256_extract_epi64(R9, 1) + _mm256_extract_epi64(R9, 3));
    eCorr[4][8] = (double)(_mm256_extract_epi64(M9, 0) + _mm256_extract_epi64(M9, 2));
    eCorr[5][5] = (double)(_mm256_extract_epi64(M9, 1) + _mm256_extract_epi64(M9, 3));

    eCorr[5][6] = (double)(_mm256_extract_epi64(R10, 0) + _mm256_extract_epi64(R10, 2));
    eCorr[5][7] = (double)(_mm256_extract_epi64(R10, 1) + _mm256_extract_epi64(R10, 3));
    eCorr[5][8] = (double)(_mm256_extract_epi64(M10, 0) + _mm256_extract_epi64(M10, 2));
    eCorr[6][6] = (double)(_mm256_extract_epi64(M10, 1) + _mm256_extract_epi64(M10, 3));

    eCorr[6][7] = (double)(_mm256_extract_epi64(R11, 0) + _mm256_extract_epi64(R11, 2));
    eCorr[6][8] = (double)(_mm256_extract_epi64(R11, 1) + _mm256_extract_epi64(R11, 3));
    eCorr[7][7] = (double)(_mm256_extract_epi64(M11, 0) + _mm256_extract_epi64(M11, 2));
    eCorr[7][8] = (double)(_mm256_extract_epi64(M11, 1) + _mm256_extract_epi64(M11, 3));

    eCorr[8][8] = (double)(_mm256_extract_epi64(R12, 0) + _mm256_extract_epi64(R12, 2));
    yCorr[0] = (double)(_mm256_extract_epi64(R12, 1) + _mm256_extract_epi64(R12, 3));
    yCorr[1] = (double)(_mm256_extract_epi64(M12, 0) + _mm256_extract_epi64(M12, 2));
    yCorr[2] = (double)(_mm256_extract_epi64(M12, 1) + _mm256_extract_epi64(M12, 3));

    yCorr[3] = (double)(_mm256_extract_epi64(R13, 0) + _mm256_extract_epi64(R13, 2));
    yCorr[4] = (double)(_mm256_extract_epi64(R13, 1) + _mm256_extract_epi64(R13, 3));
    yCorr[5] = (double)(_mm256_extract_epi64(M13, 0) + _mm256_extract_epi64(M13, 2));
    yCorr[6] = (double)(_mm256_extract_epi64(M13, 1) + _mm256_extract_epi64(M13, 3));

    yCorr[7] = (double)(_mm256_extract_epi64(R14, 0) + _mm256_extract_epi64(R14, 1) + _mm256_extract_epi64(R14, 2) + _mm256_extract_epi64(R14, 3));
    yCorr[8] = (double)(_mm256_extract_epi64(M14, 0) + _mm256_extract_epi64(M14, 1) + _mm256_extract_epi64(M14, 2) + _mm256_extract_epi64(M14, 3));

    for (k = 1; k < ALF_MAX_NUM_COEF; k++) {
        for (l = 0; l < k; l++) {
            eCorr[k][l] = eCorr[l][k];
        }
    }
}


#endif