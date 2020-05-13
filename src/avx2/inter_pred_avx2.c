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

void uavs3e_if_cpy_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    int i_src3 = i_src2 + i_src;
    int i_dst3 = i_dst2 + i_dst;
    int i_src4 = i_src << 2;
    int i_dst4 = i_dst << 2;
    __m256i m0, m1, m2, m3;
    while (height > 0) {
        m0 = _mm256_loadu_si256((const __m256i *)(src));
        m1 = _mm256_loadu_si256((const __m256i *)(src + i_src));
        m2 = _mm256_loadu_si256((const __m256i *)(src + i_src2));
        m3 = _mm256_loadu_si256((const __m256i *)(src + i_src3));
        _mm256_storeu_si256((__m256i *)dst, m0);
        _mm256_storeu_si256((__m256i *)(dst + i_dst), m1);
        _mm256_storeu_si256((__m256i *)(dst + i_dst2), m2);
        _mm256_storeu_si256((__m256i *)(dst + i_dst3), m3);
        src += i_src4;
        dst += i_dst4;
        height -= 4;
    }
}

void uavs3e_if_cpy_w64_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    int i_src3 = i_src2 + i_src;
    int i_dst3 = i_dst2 + i_dst;
    int i_src4 = i_src << 2;
    int i_dst4 = i_dst << 2;
    __m256i m0, m1, m2, m3, m4, m5, m6, m7;
    while (height) {
        m0 = _mm256_loadu_si256((const __m256i *)(src));
        m1 = _mm256_loadu_si256((const __m256i *)(src + 32));
        m2 = _mm256_loadu_si256((const __m256i *)(src + i_src));
        m3 = _mm256_loadu_si256((const __m256i *)(src + i_src + 32));
        m4 = _mm256_loadu_si256((const __m256i *)(src + i_src2));
        m5 = _mm256_loadu_si256((const __m256i *)(src + i_src2 + 32));
        m6 = _mm256_loadu_si256((const __m256i *)(src + i_src3));
        m7 = _mm256_loadu_si256((const __m256i *)(src + i_src3 + 32));

        _mm256_store_si256((__m256i *)(dst), m0);
        _mm256_store_si256((__m256i *)(dst + 32), m1);
        _mm256_store_si256((__m256i *)(dst + i_dst), m2);
        _mm256_store_si256((__m256i *)(dst + i_dst + 32), m3);
        _mm256_store_si256((__m256i *)(dst + i_dst2), m4);
        _mm256_store_si256((__m256i *)(dst + i_dst2 + 32), m5);
        _mm256_store_si256((__m256i *)(dst + i_dst3), m6);
        _mm256_store_si256((__m256i *)(dst + i_dst3 + 32), m7);

        height -= 4;
        src += i_src4;
        dst += i_dst4;
    }
}

void uavs3e_if_cpy_w128_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    __m256i m0, m1, m2, m3, m4, m5, m6, m7;
    while (height) {
        m0 = _mm256_loadu_si256((const __m256i *)(src));
        m1 = _mm256_loadu_si256((const __m256i *)(src + 32));
        m2 = _mm256_loadu_si256((const __m256i *)(src + 64));
        m3 = _mm256_loadu_si256((const __m256i *)(src + 96));
        m4 = _mm256_loadu_si256((const __m256i *)(src + i_src));
        m5 = _mm256_loadu_si256((const __m256i *)(src + i_src + 32));
        m6 = _mm256_loadu_si256((const __m256i *)(src + i_src + 64));
        m7 = _mm256_loadu_si256((const __m256i *)(src + i_src + 96));

        _mm256_store_si256((__m256i *)(dst), m0);
        _mm256_store_si256((__m256i *)(dst + 32), m1);
        _mm256_store_si256((__m256i *)(dst + 64), m2);
        _mm256_store_si256((__m256i *)(dst + 96), m3);
        _mm256_store_si256((__m256i *)(dst + i_dst), m4);
        _mm256_store_si256((__m256i *)(dst + i_dst + 32), m5);
        _mm256_store_si256((__m256i *)(dst + i_dst + 64), m6);
        _mm256_store_si256((__m256i *)(dst + i_dst + 96), m7);

        height -= 2;
        src += i_src2;
        dst += i_dst2;
    }
}

void uavs3e_if_hor_chroma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12, 0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14, 2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i T0, T1, S0, R0, R1, sum;
    __m128i s0, s1, s2, s3;
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    int i_src3 = i_src2 + i_src;
    int i_dst3 = i_dst2 + i_dst;
    src -= 1;

    while (height > 0) {
        s0 = _mm_loadl_epi64((__m128i *)(src));
        s1 = _mm_loadl_epi64((__m128i *)(src + i_src));
        s2 = _mm_loadl_epi64((__m128i *)(src + i_src2));
        s3 = _mm_loadl_epi64((__m128i *)(src + i_src3));

        s0 = _mm_unpacklo_epi64(s0, s1);
        s2 = _mm_unpacklo_epi64(s2, s3);
        S0 = _mm256_set_m128i(s2, s0);

        R0 = _mm256_shuffle_epi8(S0, mSwitch1);      // 4 rows s0 and s1
        R1 = _mm256_shuffle_epi8(S0, mSwitch2);

        T0 = _mm256_maddubs_epi16(R0, mCoefy1_hor); // 4x4: s0*c0 + s1*c1
        T1 = _mm256_maddubs_epi16(R1, mCoefy2_hor);
        sum = _mm256_add_epi16(T0, T1);

        sum = _mm256_add_epi16(sum, mAddOffset);
        sum = _mm256_srai_epi16(sum, shift);

        s0 = _mm_packus_epi16(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));

        M32(dst) = _mm_extract_epi32(s0, 0);
        M32(dst + i_dst) = _mm_extract_epi32(s0, 1);
        M32(dst + i_dst2) = _mm_extract_epi32(s0, 2);
        M32(dst + i_dst3) = _mm_extract_epi32(s0, 3);

        height -= 4;
        src += i_src << 2;
        dst += i_dst << 2;
    }
}

void uavs3e_if_hor_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i T0, T1, S0, R0, R1, sum;
    __m128i s0, s1;
    src -= 1;

    while (height > 0) {
        s0 = _mm_loadu_si128((__m128i *)(src));
        s1 = _mm_loadu_si128((__m128i *)(src + i_src));

        S0 = _mm256_set_m128i(s1, s0);

        R0 = _mm256_shuffle_epi8(S0, mSwitch1);      // 4 rows s0 and s1
        R1 = _mm256_shuffle_epi8(S0, mSwitch2);

        T0 = _mm256_maddubs_epi16(R0, mCoefy1_hor);  // 4x4: s0*c0 + s1*c1
        T1 = _mm256_maddubs_epi16(R1, mCoefy2_hor);
        sum = _mm256_add_epi16(T0, T1);

        sum = _mm256_add_epi16(sum, mAddOffset);
        sum = _mm256_srai_epi16(sum, shift);

        s0 = _mm_packus_epi16(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
        s1 = _mm_srli_si128(s0, 8);

        _mm_storel_epi64((__m128i *)(dst), s0);
        _mm_storel_epi64((__m128i *)(dst + i_dst), s1);

        height -= 2;
        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_hor_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, R0, R1, R2, R3, sum0, sum1;

    src -= 1;

    while (height > 0) {
        uavs3e_prefetch(src + i_src * 2, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);
        R0 = _mm256_shuffle_epi8(S2, mSwitch1);
        R1 = _mm256_shuffle_epi8(S2, mSwitch2);
        R2 = _mm256_shuffle_epi8(S3, mSwitch1);
        R3 = _mm256_shuffle_epi8(S3, mSwitch2);
        T0 = _mm256_maddubs_epi16(R0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(R1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(R2, mCoefy1_hor);
        T3 = _mm256_maddubs_epi16(R3, mCoefy2_hor);
        sum0 = _mm256_add_epi16(T0, T1);
        sum1 = _mm256_add_epi16(T2, T3);

        sum0 = _mm256_add_epi16(sum0, mAddOffset);
        sum1 = _mm256_add_epi16(sum1, mAddOffset);
        sum0 = _mm256_srai_epi16(sum0, shift);
        sum1 = _mm256_srai_epi16(sum1, shift);
        _mm_storeu_si128((__m128i *)(dst), _mm_packus_epi16(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1)));
        _mm_storeu_si128((__m128i *)(dst + i_dst), _mm_packus_epi16(_mm256_castsi256_si128(sum1), _mm256_extracti128_si256(sum1, 1)));

        height -= 2;
        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_hor_chroma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, R0, R1, R2, R3, sum0, sum1;

    src -= 1;

    while (height > 0) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);

        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + 16));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);
        R0 = _mm256_shuffle_epi8(S2, mSwitch1);
        R1 = _mm256_shuffle_epi8(S2, mSwitch2);
        R2 = _mm256_shuffle_epi8(S3, mSwitch1);
        R3 = _mm256_shuffle_epi8(S3, mSwitch2);
        T0 = _mm256_maddubs_epi16(R0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(R1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(R2, mCoefy1_hor);
        T3 = _mm256_maddubs_epi16(R3, mCoefy2_hor);
        sum0 = _mm256_add_epi16(T0, T1);
        sum1 = _mm256_add_epi16(T2, T3);

        sum0 = _mm256_add_epi16(sum0, mAddOffset);
        sum1 = _mm256_add_epi16(sum1, mAddOffset);
        sum0 = _mm256_srai_epi16(sum0, shift);
        sum1 = _mm256_srai_epi16(sum1, shift);

        _mm_store_si128((__m128i *)(dst), _mm_packus_epi16(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1)));
        _mm_store_si128((__m128i *)(dst + 16), _mm_packus_epi16(_mm256_castsi256_si128(sum1), _mm256_extracti128_si256(sum1, 1)));

        height--;
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_chroma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int col;
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, R0, R1, R2, R3, sum0, sum1;

    src -= 1;

    while (height--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 32) {
            S0 = _mm256_loadu_si256((__m256i *)(src + col));
            S1 = _mm256_loadu_si256((__m256i *)(src + col + 16));
            S2 = _mm256_permute4x64_epi64(S0, 0x94);
            S3 = _mm256_permute4x64_epi64(S1, 0x94);
            R0 = _mm256_shuffle_epi8(S2, mSwitch1);
            R1 = _mm256_shuffle_epi8(S2, mSwitch2);
            R2 = _mm256_shuffle_epi8(S3, mSwitch1);
            R3 = _mm256_shuffle_epi8(S3, mSwitch2);
            T0 = _mm256_maddubs_epi16(R0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(R1, mCoefy2_hor);
            T2 = _mm256_maddubs_epi16(R2, mCoefy1_hor);
            T3 = _mm256_maddubs_epi16(R3, mCoefy2_hor);
            sum0 = _mm256_add_epi16(T0, T1);
            sum1 = _mm256_add_epi16(T2, T3);

            sum0 = _mm256_add_epi16(sum0, mAddOffset);
            sum1 = _mm256_add_epi16(sum1, mAddOffset);
            sum0 = _mm256_srai_epi16(sum0, shift);
            sum1 = _mm256_srai_epi16(sum1, shift);
            _mm_store_si128((__m128i *)(dst + col), _mm_packus_epi16(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1)));
            _mm_store_si128((__m128i *)(dst + col + 16), _mm_packus_epi16(_mm256_castsi256_si128(sum1), _mm256_extracti128_si256(sum1, 1)));
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_luma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12, 0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14, 2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, sum;
    __m256i r0, r1, r2, r3;
    __m128i s0, s1, s2, s3;
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coeff + 6));
    src -= 3;

    while (height > 0) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        s0 = _mm_loadu_si128((__m128i *)(src));
        s1 = _mm_loadu_si128((__m128i *)(src + i_src));
        s2 = _mm_loadu_si128((__m128i *)(src + i_src * 2));
        s3 = _mm_loadu_si128((__m128i *)(src + i_src * 3));

        S0 = _mm256_set_m128i(s2, s0);
        S1 = _mm256_set_m128i(s3, s1);

        S2 = _mm256_srli_si256(S0, 4);
        S3 = _mm256_srli_si256(S1, 4);

        T0 = _mm256_unpacklo_epi64(S0, S1);
        T1 = _mm256_unpacklo_epi64(S2, S3);

        r0 = _mm256_shuffle_epi8(T0, mSwitch1);
        r1 = _mm256_shuffle_epi8(T0, mSwitch2);
        r2 = _mm256_shuffle_epi8(T1, mSwitch1);
        r3 = _mm256_shuffle_epi8(T1, mSwitch2);

        T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        T0 = _mm256_add_epi16(T0, T1);
        T1 = _mm256_add_epi16(T2, T3);
        sum = _mm256_add_epi16(T0, T1);

        sum = _mm256_add_epi16(sum, mAddOffset);
        sum = _mm256_srai_epi16(sum, shift);

        s0 = _mm_packus_epi16(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));

        M32(dst) = _mm_extract_epi32(s0, 0);
        M32(dst + i_dst) = _mm_extract_epi32(s0, 1);
        M32(dst + i_dst * 2) = _mm_extract_epi32(s0, 2);
        M32(dst + i_dst * 3) = _mm_extract_epi32(s0, 3);

        height -= 4;
        src += i_src << 2;
        dst += i_dst << 2;
    }
}

void uavs3e_if_hor_luma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i T0, T1, T2, T3, S, sum;
    __m256i r0, r1, r2, r3;
    __m128i s0, s1;
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coeff + 6));

    src -= 3;

    while (height) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        s0 = _mm_loadu_si128((__m128i *)(src));
        s1 = _mm_loadu_si128((__m128i *)(src + i_src));
        S = _mm256_set_m128i(s1, s0);

        r0 = _mm256_shuffle_epi8(S, mSwitch1);
        r1 = _mm256_shuffle_epi8(S, mSwitch2);
        r2 = _mm256_shuffle_epi8(S, mSwitch3);
        r3 = _mm256_shuffle_epi8(S, mSwitch4);

        T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        T0 = _mm256_add_epi16(T0, T1);
        T1 = _mm256_add_epi16(T2, T3);
        sum = _mm256_add_epi16(T0, T1);

        sum = _mm256_add_epi16(sum, mAddOffset);
        sum = _mm256_srai_epi16(sum, shift);

        s0 = _mm_packus_epi16(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
        s1 = _mm_srli_si128(s0, 8);
        _mm_storel_epi64((__m128i *)(dst), s0);
        _mm_storel_epi64((__m128i *)(dst + i_dst), s1);

        height -= 2;
        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_hor_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i T0, T1, S0, S1, S2, S3, sum0, sum1, T2, T3;
    __m256i r0, r1, r2, r3;
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coeff + 6));

    src -= 3;

    while (height) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);

        r0 = _mm256_shuffle_epi8(S2, mSwitch1);
        r1 = _mm256_shuffle_epi8(S2, mSwitch2);
        r2 = _mm256_shuffle_epi8(S2, mSwitch3);
        r3 = _mm256_shuffle_epi8(S2, mSwitch4);

        T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        r0 = _mm256_shuffle_epi8(S3, mSwitch1);
        r1 = _mm256_shuffle_epi8(S3, mSwitch2);
        r2 = _mm256_shuffle_epi8(S3, mSwitch3);
        r3 = _mm256_shuffle_epi8(S3, mSwitch4);

        r0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        r1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        r2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        r3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        T0 = _mm256_add_epi16(T0, T1);
        T1 = _mm256_add_epi16(T2, T3);
        r0 = _mm256_add_epi16(r0, r1);
        r1 = _mm256_add_epi16(r2, r3);
        sum0 = _mm256_add_epi16(T0, T1);
        sum1 = _mm256_add_epi16(r0, r1);

        sum0 = _mm256_add_epi16(sum0, mAddOffset);
        sum1 = _mm256_add_epi16(sum1, mAddOffset);
        sum0 = _mm256_srai_epi16(sum0, shift);
        sum1 = _mm256_srai_epi16(sum1, shift);

        _mm_storeu_si128((__m128i *)(dst), _mm_packus_epi16(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1)));
        _mm_storeu_si128((__m128i *)(dst + i_dst), _mm_packus_epi16(_mm256_castsi256_si128(sum1), _mm256_extracti128_si256(sum1, 1)));

        height -= 2;
        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_hor_luma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i T0, T1, S0, S1, S2, S3, sum0, sum1, T2, T3;
    __m256i r0, r1, r2, r3;
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coeff + 6));

    src -= 3;

    while (height--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);

        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + 16));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);

        r0 = _mm256_shuffle_epi8(S2, mSwitch1);
        r1 = _mm256_shuffle_epi8(S2, mSwitch2);
        r2 = _mm256_shuffle_epi8(S2, mSwitch3);
        r3 = _mm256_shuffle_epi8(S2, mSwitch4);

        T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        r0 = _mm256_shuffle_epi8(S3, mSwitch1);
        r1 = _mm256_shuffle_epi8(S3, mSwitch2);
        r2 = _mm256_shuffle_epi8(S3, mSwitch3);
        r3 = _mm256_shuffle_epi8(S3, mSwitch4);

        r0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        r1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        r2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        r3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        T0 = _mm256_add_epi16(T0, T1);
        T1 = _mm256_add_epi16(T2, T3);
        r0 = _mm256_add_epi16(r0, r1);
        r1 = _mm256_add_epi16(r2, r3);
        sum0 = _mm256_add_epi16(T0, T1);
        sum1 = _mm256_add_epi16(r0, r1);

        sum0 = _mm256_add_epi16(sum0, mAddOffset);
        sum1 = _mm256_add_epi16(sum1, mAddOffset);
        sum0 = _mm256_srai_epi16(sum0, shift);
        sum1 = _mm256_srai_epi16(sum1, shift);

        _mm_store_si128((__m128i *)(dst), _mm_packus_epi16(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1)));
        _mm_store_si128((__m128i *)(dst + 16), _mm_packus_epi16(_mm256_castsi256_si128(sum1), _mm256_extracti128_si256(sum1, 1)));

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_luma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int col;
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i T0, T1, S0, S1, S2, S3, sum0, sum1, T2, T3;
    __m256i r0, r1, r2, r3;
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coeff + 6));

    src -= 3;

    while (height--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 32) {
            S0 = _mm256_loadu_si256((__m256i *)(src + col));
            S1 = _mm256_loadu_si256((__m256i *)(src + col + 16));
            S2 = _mm256_permute4x64_epi64(S0, 0x94);
            S3 = _mm256_permute4x64_epi64(S1, 0x94);

            r0 = _mm256_shuffle_epi8(S2, mSwitch1);
            r1 = _mm256_shuffle_epi8(S2, mSwitch2);
            r2 = _mm256_shuffle_epi8(S2, mSwitch3);
            r3 = _mm256_shuffle_epi8(S2, mSwitch4);

            T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
            T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
            T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

            r0 = _mm256_shuffle_epi8(S3, mSwitch1);
            r1 = _mm256_shuffle_epi8(S3, mSwitch2);
            r2 = _mm256_shuffle_epi8(S3, mSwitch3);
            r3 = _mm256_shuffle_epi8(S3, mSwitch4);

            r0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
            r1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
            r2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
            r3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

            T0 = _mm256_add_epi16(T0, T1);
            T1 = _mm256_add_epi16(T2, T3);
            r0 = _mm256_add_epi16(r0, r1);
            r1 = _mm256_add_epi16(r2, r3);
            sum0 = _mm256_add_epi16(T0, T1);
            sum1 = _mm256_add_epi16(r0, r1);

            sum0 = _mm256_add_epi16(sum0, mAddOffset);
            sum1 = _mm256_add_epi16(sum1, mAddOffset);
            sum0 = _mm256_srai_epi16(sum0, shift);
            sum1 = _mm256_srai_epi16(sum1, shift);

            _mm_storeu_si128((__m128i *)(dst + col), _mm_packus_epi16(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1)));
            _mm_storeu_si128((__m128i *)(dst + col + 16), _mm_packus_epi16(_mm256_castsi256_si128(sum1), _mm256_extracti128_si256(sum1, 1)));
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i T0, T1, mVal;
    __m256i S0, S1, S2, S3;
    __m128i s0, s1, s2, s3, s4;

    src -= i_src;

    while (height) {
        uavs3e_prefetch(src + 5 * i_src, _MM_HINT_NTA);
        uavs3e_prefetch(src + 6 * i_src, _MM_HINT_NTA);
        height -= 2;
        s0 = _mm_loadl_epi64((__m128i *)(src));
        s1 = _mm_loadl_epi64((__m128i *)(src + i_src));
        s2 = _mm_loadl_epi64((__m128i *)(src + i_src2));
        s3 = _mm_loadl_epi64((__m128i *)(src + i_src3));
        s4 = _mm_loadl_epi64((__m128i *)(src + i_src4));

        S0 = _mm256_set_m128i(s1, s0);
        S1 = _mm256_set_m128i(s2, s1);
        S2 = _mm256_set_m128i(s3, s2);
        S3 = _mm256_set_m128i(s4, s3);

        S0 = _mm256_unpacklo_epi8(S0, S1);
        S2 = _mm256_unpacklo_epi8(S2, S3);

        T0 = _mm256_maddubs_epi16(S0, coeff0);
        T1 = _mm256_maddubs_epi16(S2, coeff1);

        mVal = _mm256_add_epi16(T0, T1);

        mVal = _mm256_add_epi16(mVal, mAddOffset);
        mVal = _mm256_srai_epi16(mVal, shift);
        s0 = _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1));
        s1 = _mm_srli_si128(s0, 8);

        _mm_storel_epi64((__m128i *)(dst), s0);
        _mm_storel_epi64((__m128i *)(dst + i_dst), s1);

        src += 2 * i_src;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_ver_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i T0, T1, T2, T3, mVal0, mVal1;
    __m128i s0, s1, s2, s3, s4;
    __m256i S0, S1, S2, S3;

    src -= i_src;

    while (height) {
        uavs3e_prefetch(src + 5 * i_src, _MM_HINT_NTA);
        uavs3e_prefetch(src + 6 * i_src, _MM_HINT_NTA);
        height -= 2;
        s0 = _mm_loadu_si128((__m128i *)(src));
        s1 = _mm_loadu_si128((__m128i *)(src + i_src));
        s2 = _mm_loadu_si128((__m128i *)(src + i_src2));
        s3 = _mm_loadu_si128((__m128i *)(src + i_src3));
        s4 = _mm_loadu_si128((__m128i *)(src + i_src4));

        S0 = _mm256_set_m128i(s1, s0);
        S1 = _mm256_set_m128i(s2, s1);
        S2 = _mm256_set_m128i(s3, s2);
        S3 = _mm256_set_m128i(s4, s3);

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpackhi_epi8(S0, S1);
        T2 = _mm256_unpacklo_epi8(S2, S3);
        T3 = _mm256_unpackhi_epi8(S2, S3);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff0);
        T2 = _mm256_maddubs_epi16(T2, coeff1);
        T3 = _mm256_maddubs_epi16(T3, coeff1);

        mVal0 = _mm256_add_epi16(T0, T2);
        mVal1 = _mm256_add_epi16(T1, T3);

        mVal0 = _mm256_add_epi16(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal0 = _mm256_srai_epi16(mVal0, shift);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal0 = _mm256_packus_epi16(mVal0, mVal1);

        _mm_storeu_si128((__m128i *)dst, _mm256_castsi256_si128(mVal0));
        _mm_storeu_si128((__m128i *)(dst + i_dst), _mm256_extracti128_si256(mVal0, 1));

        src += 2 * i_src;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_ver_chroma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal0, mVal1, mVal2, mVal3;
    __m256i S0, S1, S2, S3, S4;

    src -= i_src;

    while (height) {
        uavs3e_prefetch(src + 5 * i_src, _MM_HINT_NTA);
        uavs3e_prefetch(src + 6 * i_src, _MM_HINT_NTA);
        height -= 2;
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(src + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(src + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(src + i_src4));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpackhi_epi8(S0, S1);
        T2 = _mm256_unpacklo_epi8(S2, S3);
        T3 = _mm256_unpackhi_epi8(S2, S3);
        T4 = _mm256_unpacklo_epi8(S1, S2);
        T5 = _mm256_unpackhi_epi8(S1, S2);
        T6 = _mm256_unpacklo_epi8(S3, S4);
        T7 = _mm256_unpackhi_epi8(S3, S4);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff0);
        T2 = _mm256_maddubs_epi16(T2, coeff1);
        T3 = _mm256_maddubs_epi16(T3, coeff1);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff0);
        T6 = _mm256_maddubs_epi16(T6, coeff1);
        T7 = _mm256_maddubs_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi16(T0, T2);
        mVal1 = _mm256_add_epi16(T1, T3);
        mVal2 = _mm256_add_epi16(T4, T6);
        mVal3 = _mm256_add_epi16(T5, T7);

        mVal0 = _mm256_add_epi16(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi16(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi16(mVal0, shift);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal3 = _mm256_srai_epi16(mVal3, shift);
        mVal0 = _mm256_packus_epi16(mVal0, mVal1);
        mVal2 = _mm256_packus_epi16(mVal2, mVal3);

        _mm256_storeu_si256((__m256i *)dst, mVal0);
        _mm256_storeu_si256((__m256i *)(dst + i_dst), mVal2);

        src += 2 * i_src;
        dst += 2 * i_dst;

    }
}

void uavs3e_if_ver_chroma_w64_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal0, mVal1, mVal2, mVal3;

    src -= i_src;

    while (height--) {
        uavs3e_prefetch(src + 4 * i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S4 = _mm256_loadu_si256((__m256i *)(src + 32));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S5 = _mm256_loadu_si256((__m256i *)(src + i_src + 32));
        S2 = _mm256_loadu_si256((__m256i *)(src + i_src2));
        S6 = _mm256_loadu_si256((__m256i *)(src + i_src2 + 32));
        S3 = _mm256_loadu_si256((__m256i *)(src + i_src3));
        S7 = _mm256_loadu_si256((__m256i *)(src + i_src3 + 32));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpackhi_epi8(S0, S1);
        T3 = _mm256_unpackhi_epi8(S2, S3);
        T4 = _mm256_unpacklo_epi8(S4, S5);
        T5 = _mm256_unpacklo_epi8(S6, S7);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff0);
        T3 = _mm256_maddubs_epi16(T3, coeff1);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff0);
        T7 = _mm256_maddubs_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi16(T0, T1);
        mVal1 = _mm256_add_epi16(T2, T3);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal3 = _mm256_add_epi16(T6, T7);

        mVal0 = _mm256_add_epi16(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi16(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi16(mVal0, shift);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal3 = _mm256_srai_epi16(mVal3, shift);
        mVal0 = _mm256_packus_epi16(mVal0, mVal1);
        mVal1 = _mm256_packus_epi16(mVal2, mVal3);

        _mm256_store_si256((__m256i *)(dst), mVal0);
        _mm256_store_si256((__m256i *)(dst + 32), mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_chroma_w128_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal0, mVal1, mVal2, mVal3;

    src -= i_src;

    while (height--) {
        uavs3e_prefetch(src + 4 * i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S4 = _mm256_loadu_si256((__m256i *)(src + 32));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S5 = _mm256_loadu_si256((__m256i *)(src + i_src + 32));
        S2 = _mm256_loadu_si256((__m256i *)(src + i_src2));
        S6 = _mm256_loadu_si256((__m256i *)(src + i_src2 + 32));
        S3 = _mm256_loadu_si256((__m256i *)(src + i_src3));
        S7 = _mm256_loadu_si256((__m256i *)(src + i_src3 + 32));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpackhi_epi8(S0, S1);
        T3 = _mm256_unpackhi_epi8(S2, S3);
        T4 = _mm256_unpacklo_epi8(S4, S5);
        T5 = _mm256_unpacklo_epi8(S6, S7);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff0);
        T3 = _mm256_maddubs_epi16(T3, coeff1);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff0);
        T7 = _mm256_maddubs_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi16(T0, T1);
        mVal1 = _mm256_add_epi16(T2, T3);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal3 = _mm256_add_epi16(T6, T7);

        mVal0 = _mm256_add_epi16(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi16(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi16(mVal0, shift);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal3 = _mm256_srai_epi16(mVal3, shift);
        mVal0 = _mm256_packus_epi16(mVal0, mVal1);
        mVal1 = _mm256_packus_epi16(mVal2, mVal3);

        _mm256_store_si256((__m256i *)(dst), mVal0);
        _mm256_store_si256((__m256i *)(dst + 32), mVal1);

        S0 = _mm256_loadu_si256((__m256i *)(src + 64));
        S4 = _mm256_loadu_si256((__m256i *)(src + 96));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src + 64));
        S5 = _mm256_loadu_si256((__m256i *)(src + i_src + 96));
        S2 = _mm256_loadu_si256((__m256i *)(src + i_src2 + 64));
        S6 = _mm256_loadu_si256((__m256i *)(src + i_src2 + 96));
        S3 = _mm256_loadu_si256((__m256i *)(src + i_src3 + 64));
        S7 = _mm256_loadu_si256((__m256i *)(src + i_src3 + 96));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpackhi_epi8(S0, S1);
        T3 = _mm256_unpackhi_epi8(S2, S3);
        T4 = _mm256_unpacklo_epi8(S4, S5);
        T5 = _mm256_unpacklo_epi8(S6, S7);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff0);
        T3 = _mm256_maddubs_epi16(T3, coeff1);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff0);
        T7 = _mm256_maddubs_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi16(T0, T1);
        mVal1 = _mm256_add_epi16(T2, T3);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal3 = _mm256_add_epi16(T6, T7);

        mVal0 = _mm256_add_epi16(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi16(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi16(mVal0, shift);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal3 = _mm256_srai_epi16(mVal3, shift);
        mVal0 = _mm256_packus_epi16(mVal0, mVal1);
        mVal1 = _mm256_packus_epi16(mVal2, mVal3);

        _mm256_store_si256((__m256i *)(dst + 64), mVal0);
        _mm256_store_si256((__m256i *)(dst + 96), mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i coeff2 = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i coeff3 = _mm256_set1_epi16(*(s16 *)(coeff + 6));
    __m256i T0, T1, T2, T3, T4, mVal;
    __m256i R0, R1, R2, R3, R4, R5, R6, R7, R8, R9;

    src -= 3 * i_src;

    while (height > 0) {
        __m128i S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10;

        S0 = _mm_loadl_epi64((__m128i *)(src));
        S1 = _mm_loadl_epi64((__m128i *)(src + i_src));
        S2 = _mm_loadl_epi64((__m128i *)(src + i_src * 2));
        S3 = _mm_loadl_epi64((__m128i *)(src + i_src * 3));
        S4 = _mm_loadl_epi64((__m128i *)(src + i_src * 4));
        S5 = _mm_loadl_epi64((__m128i *)(src + i_src * 5));

        R0 = _mm256_set_m128i(S1, S0);
        R1 = _mm256_set_m128i(S2, S1);
        R2 = _mm256_set_m128i(S3, S2);
        R3 = _mm256_set_m128i(S4, S3);

        S6 = _mm_loadl_epi64((__m128i *)(src + i_src * 6));
        S7 = _mm_loadl_epi64((__m128i *)(src + i_src * 7));
        S8 = _mm_loadl_epi64((__m128i *)(src + i_src * 8));
        S9 = _mm_loadl_epi64((__m128i *)(src + i_src * 9));
        S10 = _mm_loadl_epi64((__m128i *)(src + i_src * 10));

        T0 = _mm256_unpacklo_epi8(R0, R1);
        T1 = _mm256_unpacklo_epi8(R2, R3);

        R4 = _mm256_set_m128i(S5, S4);
        R5 = _mm256_set_m128i(S6, S5);
        R6 = _mm256_set_m128i(S7, S6);
        R7 = _mm256_set_m128i(S8, S7);
        R8 = _mm256_set_m128i(S9, S8);
        R9 = _mm256_set_m128i(S10, S9);

        T2 = _mm256_unpacklo_epi8(R4, R5);
        T3 = _mm256_unpacklo_epi8(R6, R7);
        T4 = _mm256_unpacklo_epi8(R8, R9);

        T0 = _mm256_unpacklo_epi64(T0, T1);
        T1 = _mm256_unpacklo_epi64(T1, T2);
        T2 = _mm256_unpacklo_epi64(T2, T3);
        T3 = _mm256_unpacklo_epi64(T3, T4);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);

        T0 = _mm256_add_epi16(T0, T1);
        T2 = _mm256_add_epi16(T2, T3);
        mVal = _mm256_add_epi16(T0, T2);

        mVal = _mm256_add_epi16(mVal, mAddOffset);
        mVal = _mm256_srai_epi16(mVal, shift);
        S0 = _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1));

        M32(dst) = _mm_extract_epi32(S0, 0);
        M32(dst + i_dst) = _mm_extract_epi32(S0, 2);
        M32(dst + i_dst * 2) = _mm_extract_epi32(S0, 1);
        M32(dst + i_dst * 3) = _mm_extract_epi32(S0, 3);

        height -= 4;
        src += i_src << 2;
        dst += i_dst << 2;
    }
}

void uavs3e_if_ver_luma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    const int i_src5 = i_src * 5;
    const int i_src6 = i_src * 6;
    const int i_src7 = i_src * 7;
    const int i_src8 = i_src * 8;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i coeff2 = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i coeff3 = _mm256_set1_epi16(*(s16 *)(coeff + 6));
    __m256i T0, T1, T2, T3, mVal;
    __m256i R0, R1, R2, R3, R4, R5, R6, R7;

    src -= 3 * i_src;

    while (height) {
        __m128i S0, S1, S2, S3, S4, S5, S6, S7, S8;

        height -= 2;
        S0 = _mm_loadl_epi64((__m128i *)(src));
        S1 = _mm_loadl_epi64((__m128i *)(src + i_src));
        S2 = _mm_loadl_epi64((__m128i *)(src + i_src2));
        S3 = _mm_loadl_epi64((__m128i *)(src + i_src3));
        S4 = _mm_loadl_epi64((__m128i *)(src + i_src4));
        S5 = _mm_loadl_epi64((__m128i *)(src + i_src5));
        S6 = _mm_loadl_epi64((__m128i *)(src + i_src6));
        S7 = _mm_loadl_epi64((__m128i *)(src + i_src7));
        S8 = _mm_loadl_epi64((__m128i *)(src + i_src8));

        R0 = _mm256_set_m128i(S1, S0);
        R1 = _mm256_set_m128i(S2, S1);
        R2 = _mm256_set_m128i(S3, S2);
        R3 = _mm256_set_m128i(S4, S3);
        R4 = _mm256_set_m128i(S5, S4);
        R5 = _mm256_set_m128i(S6, S5);
        R6 = _mm256_set_m128i(S7, S6);
        R7 = _mm256_set_m128i(S8, S7);

        T0 = _mm256_unpacklo_epi8(R0, R1);
        T1 = _mm256_unpacklo_epi8(R2, R3);
        T2 = _mm256_unpacklo_epi8(R4, R5);
        T3 = _mm256_unpacklo_epi8(R6, R7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);

        T0 = _mm256_add_epi16(T0, T1);
        T2 = _mm256_add_epi16(T2, T3);
        mVal = _mm256_add_epi16(T0, T2);

        mVal = _mm256_add_epi16(mVal, mAddOffset);
        mVal = _mm256_srai_epi16(mVal, shift);
        S0 = _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1));
        S1 = _mm_srli_si128(S0, 8);

        _mm_storel_epi64((__m128i *)(dst), S0);
        _mm_storel_epi64((__m128i *)(dst + i_dst), S1);
        src += 2 * i_src;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_ver_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    const int i_src5 = i_src * 5;
    const int i_src6 = i_src * 6;
    const int i_src7 = i_src * 7;
    const int i_src8 = i_src * 8;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i coeff2 = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i coeff3 = _mm256_set1_epi16(*(s16 *)(coeff + 6));
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal1, mVal2;
    __m256i R0, R1, R2, R3, R4, R5, R6, R7;

    src -= 3 * i_src;

    while (height) {
        __m128i S0, S1, S2, S3, S4, S5, S6, S7, S8;
        uavs3e_prefetch(src + 9 * i_src, _MM_HINT_NTA);
        uavs3e_prefetch(src + 10 * i_src, _MM_HINT_NTA);

        height -= 2;
        S0 = _mm_loadu_si128((__m128i *)(src));
        S1 = _mm_loadu_si128((__m128i *)(src + i_src));
        S2 = _mm_loadu_si128((__m128i *)(src + i_src2));
        S3 = _mm_loadu_si128((__m128i *)(src + i_src3));
        S4 = _mm_loadu_si128((__m128i *)(src + i_src4));
        S5 = _mm_loadu_si128((__m128i *)(src + i_src5));
        S6 = _mm_loadu_si128((__m128i *)(src + i_src6));
        S7 = _mm_loadu_si128((__m128i *)(src + i_src7));
        S8 = _mm_loadu_si128((__m128i *)(src + i_src8));

        R0 = _mm256_set_m128i(S0, S1);
        R1 = _mm256_set_m128i(S1, S2);
        R2 = _mm256_set_m128i(S2, S3);
        R3 = _mm256_set_m128i(S3, S4);
        R4 = _mm256_set_m128i(S4, S5);
        R5 = _mm256_set_m128i(S5, S6);
        R6 = _mm256_set_m128i(S6, S7);
        R7 = _mm256_set_m128i(S7, S8);

        T0 = _mm256_unpacklo_epi8(R0, R1);
        T1 = _mm256_unpackhi_epi8(R0, R1);
        T2 = _mm256_unpacklo_epi8(R2, R3);
        T3 = _mm256_unpackhi_epi8(R2, R3);
        T4 = _mm256_unpacklo_epi8(R4, R5);
        T5 = _mm256_unpackhi_epi8(R4, R5);
        T6 = _mm256_unpacklo_epi8(R6, R7);
        T7 = _mm256_unpackhi_epi8(R6, R7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff0);
        T2 = _mm256_maddubs_epi16(T2, coeff1);
        T3 = _mm256_maddubs_epi16(T3, coeff1);
        T4 = _mm256_maddubs_epi16(T4, coeff2);
        T5 = _mm256_maddubs_epi16(T5, coeff2);
        T6 = _mm256_maddubs_epi16(T6, coeff3);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T2);
        mVal2 = _mm256_add_epi16(T1, T3);
        mVal1 = _mm256_add_epi16(mVal1, T4);
        mVal2 = _mm256_add_epi16(mVal2, T5);
        mVal1 = _mm256_add_epi16(mVal1, T6);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm_storeu_si128((__m128i *)dst, _mm256_extractf128_si256(mVal1, 1));
        _mm_storeu_si128((__m128i *)(dst + i_dst), _mm256_castsi256_si128(mVal1));
        src += 2 * i_src;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_ver_luma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    const int i_src5 = i_src * 5;
    const int i_src6 = i_src * 6;
    const int i_src7 = i_src * 7;

    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i coeff2 = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i coeff3 = _mm256_set1_epi16(*(s16 *)(coeff + 6));
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal1, mVal2;


    src -= 3 * i_src;
    while (height--) {
        __m256i S0, S1, S2, S3, S4, S5, S6, S7;
        uavs3e_prefetch(src + 8 * i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(src + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(src + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(src + i_src4));
        S5 = _mm256_loadu_si256((__m256i *)(src + i_src5));
        S6 = _mm256_loadu_si256((__m256i *)(src + i_src6));
        S7 = _mm256_loadu_si256((__m256i *)(src + i_src7));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpacklo_epi8(S4, S5);
        T3 = _mm256_unpacklo_epi8(S6, S7);
        T4 = _mm256_unpackhi_epi8(S0, S1);
        T5 = _mm256_unpackhi_epi8(S2, S3);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff2);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T1);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal1 = _mm256_add_epi16(mVal1, T2);
        mVal2 = _mm256_add_epi16(mVal2, T6);
        mVal1 = _mm256_add_epi16(mVal1, T3);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm256_storeu_si256((__m256i *)(dst), mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w64_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    const int i_src5 = i_src * 5;
    const int i_src6 = i_src * 6;
    const int i_src7 = i_src * 7;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i coeff2 = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i coeff3 = _mm256_set1_epi16(*(s16 *)(coeff + 6));
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal1, mVal2;

    src -= 3 * i_src;

    while (height--) {
        const pel *p = src + 32;
        __m256i S0, S1, S2, S3, S4, S5, S6, S7;
        uavs3e_prefetch(src + 8 * i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(src + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(src + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(src + i_src4));
        S5 = _mm256_loadu_si256((__m256i *)(src + i_src5));
        S6 = _mm256_loadu_si256((__m256i *)(src + i_src6));
        S7 = _mm256_loadu_si256((__m256i *)(src + i_src7));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpacklo_epi8(S4, S5);
        T3 = _mm256_unpacklo_epi8(S6, S7);
        T4 = _mm256_unpackhi_epi8(S0, S1);
        T5 = _mm256_unpackhi_epi8(S2, S3);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff2);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T1);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal1 = _mm256_add_epi16(mVal1, T2);
        mVal2 = _mm256_add_epi16(mVal2, T6);
        mVal1 = _mm256_add_epi16(mVal1, T3);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm256_store_si256((__m256i *)(dst), mVal1);

        S0 = _mm256_loadu_si256((__m256i *)(p));
        S1 = _mm256_loadu_si256((__m256i *)(p + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(p + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(p + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(p + i_src4));
        S5 = _mm256_loadu_si256((__m256i *)(p + i_src5));
        S6 = _mm256_loadu_si256((__m256i *)(p + i_src6));
        S7 = _mm256_loadu_si256((__m256i *)(p + i_src7));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpacklo_epi8(S4, S5);
        T3 = _mm256_unpacklo_epi8(S6, S7);
        T4 = _mm256_unpackhi_epi8(S0, S1);
        T5 = _mm256_unpackhi_epi8(S2, S3);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff2);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T1);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal1 = _mm256_add_epi16(mVal1, T2);
        mVal2 = _mm256_add_epi16(mVal2, T6);
        mVal1 = _mm256_add_epi16(mVal1, T3);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm256_store_si256((__m256i *)(dst + 32), mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w128_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    const int i_src5 = i_src * 5;
    const int i_src6 = i_src * 6;
    const int i_src7 = i_src * 7;
    __m256i mAddOffset = _mm256_set1_epi16(offset);
    __m256i coeff0 = _mm256_set1_epi16(*(s16 *)coeff);
    __m256i coeff1 = _mm256_set1_epi16(*(s16 *)(coeff + 2));
    __m256i coeff2 = _mm256_set1_epi16(*(s16 *)(coeff + 4));
    __m256i coeff3 = _mm256_set1_epi16(*(s16 *)(coeff + 6));
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal1, mVal2;

    src -= 3 * i_src;

    while (height--) {
        const pel *p = src + 32;
        __m256i S0, S1, S2, S3, S4, S5, S6, S7;
        uavs3e_prefetch(src + 8 * i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(src + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(src + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(src + i_src4));
        S5 = _mm256_loadu_si256((__m256i *)(src + i_src5));
        S6 = _mm256_loadu_si256((__m256i *)(src + i_src6));
        S7 = _mm256_loadu_si256((__m256i *)(src + i_src7));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpacklo_epi8(S4, S5);
        T3 = _mm256_unpacklo_epi8(S6, S7);
        T4 = _mm256_unpackhi_epi8(S0, S1);
        T5 = _mm256_unpackhi_epi8(S2, S3);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff2);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T1);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal1 = _mm256_add_epi16(mVal1, T2);
        mVal2 = _mm256_add_epi16(mVal2, T6);
        mVal1 = _mm256_add_epi16(mVal1, T3);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm256_store_si256((__m256i *)(dst), mVal1);

        S0 = _mm256_loadu_si256((__m256i *)(p));
        S1 = _mm256_loadu_si256((__m256i *)(p + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(p + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(p + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(p + i_src4));
        S5 = _mm256_loadu_si256((__m256i *)(p + i_src5));
        S6 = _mm256_loadu_si256((__m256i *)(p + i_src6));
        S7 = _mm256_loadu_si256((__m256i *)(p + i_src7));

        p += 32;

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpacklo_epi8(S4, S5);
        T3 = _mm256_unpacklo_epi8(S6, S7);
        T4 = _mm256_unpackhi_epi8(S0, S1);
        T5 = _mm256_unpackhi_epi8(S2, S3);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff2);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T1);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal1 = _mm256_add_epi16(mVal1, T2);
        mVal2 = _mm256_add_epi16(mVal2, T6);
        mVal1 = _mm256_add_epi16(mVal1, T3);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm256_store_si256((__m256i *)(dst + 32), mVal1);

        S0 = _mm256_loadu_si256((__m256i *)(p));
        S1 = _mm256_loadu_si256((__m256i *)(p + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(p + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(p + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(p + i_src4));
        S5 = _mm256_loadu_si256((__m256i *)(p + i_src5));
        S6 = _mm256_loadu_si256((__m256i *)(p + i_src6));
        S7 = _mm256_loadu_si256((__m256i *)(p + i_src7));

        p += 32;

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpacklo_epi8(S4, S5);
        T3 = _mm256_unpacklo_epi8(S6, S7);
        T4 = _mm256_unpackhi_epi8(S0, S1);
        T5 = _mm256_unpackhi_epi8(S2, S3);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff2);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T1);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal1 = _mm256_add_epi16(mVal1, T2);
        mVal2 = _mm256_add_epi16(mVal2, T6);
        mVal1 = _mm256_add_epi16(mVal1, T3);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm256_store_si256((__m256i *)(dst + 64), mVal1);

        S0 = _mm256_loadu_si256((__m256i *)(p));
        S1 = _mm256_loadu_si256((__m256i *)(p + i_src));
        S2 = _mm256_loadu_si256((__m256i *)(p + i_src2));
        S3 = _mm256_loadu_si256((__m256i *)(p + i_src3));
        S4 = _mm256_loadu_si256((__m256i *)(p + i_src4));
        S5 = _mm256_loadu_si256((__m256i *)(p + i_src5));
        S6 = _mm256_loadu_si256((__m256i *)(p + i_src6));
        S7 = _mm256_loadu_si256((__m256i *)(p + i_src7));

        T0 = _mm256_unpacklo_epi8(S0, S1);
        T1 = _mm256_unpacklo_epi8(S2, S3);
        T2 = _mm256_unpacklo_epi8(S4, S5);
        T3 = _mm256_unpacklo_epi8(S6, S7);
        T4 = _mm256_unpackhi_epi8(S0, S1);
        T5 = _mm256_unpackhi_epi8(S2, S3);
        T6 = _mm256_unpackhi_epi8(S4, S5);
        T7 = _mm256_unpackhi_epi8(S6, S7);

        T0 = _mm256_maddubs_epi16(T0, coeff0);
        T1 = _mm256_maddubs_epi16(T1, coeff1);
        T2 = _mm256_maddubs_epi16(T2, coeff2);
        T3 = _mm256_maddubs_epi16(T3, coeff3);
        T4 = _mm256_maddubs_epi16(T4, coeff0);
        T5 = _mm256_maddubs_epi16(T5, coeff1);
        T6 = _mm256_maddubs_epi16(T6, coeff2);
        T7 = _mm256_maddubs_epi16(T7, coeff3);

        mVal1 = _mm256_add_epi16(T0, T1);
        mVal2 = _mm256_add_epi16(T4, T5);
        mVal1 = _mm256_add_epi16(mVal1, T2);
        mVal2 = _mm256_add_epi16(mVal2, T6);
        mVal1 = _mm256_add_epi16(mVal1, T3);
        mVal2 = _mm256_add_epi16(mVal2, T7);

        mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi16(mVal1, shift);
        mVal2 = _mm256_srai_epi16(mVal2, shift);
        mVal1 = _mm256_packus_epi16(mVal1, mVal2);

        _mm256_store_si256((__m256i *)(dst + 96), mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_ver_chroma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(32 + 3) * 4]);
    s16 *tmp = tmp_res;
    int i_src2 = i_src << 1;
    int i_src3 = i_src2 + i_src;
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coef_x);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12, 0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14, 2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14);
    __m128i s0, s1, s2, s3, s4, s5;
    __m128i t0, t1, t2, t3, t4, t5;
    __m128i mCoefy11 = _mm_set1_epi16(*(s16 *)coef_y);
    __m128i mCoefy22 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    __m256i mCoefy1_ver = _mm256_cvtepi8_epi16(mCoefy11);
    __m256i mCoefy2_ver = _mm256_cvtepi8_epi16(mCoefy22);
    __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
    __m256i R0, R1, R2, R3;
    __m256i T0, T1, T2;
    int shift = 12;
    int i_dst2 = i_dst << 1;
    int i_dst3 = i_dst + i_dst2;

    src = src - i_src - 1;

    // hor filter first 3 rows
    s0 = _mm_loadu_si128((__m128i *)(src));
    s1 = _mm_loadu_si128((__m128i *)(src + i_src));
    s2 = _mm_loadu_si128((__m128i *)(src + i_src2));

    s0 = _mm_unpacklo_epi64(s0, s1);
    T2 = _mm256_set_m128i(s2, s0);

    T0 = _mm256_shuffle_epi8(T2, mSwitch1);
    T1 = _mm256_shuffle_epi8(T2, mSwitch2);

    T0 = _mm256_maddubs_epi16(T0, mCoefy1_hor);
    T1 = _mm256_maddubs_epi16(T1, mCoefy2_hor);

    T0 = _mm256_add_epi16(T0, T1);

    src += i_src3;

    _mm_store_si128((__m128i *)(tmp), _mm256_castsi256_si128(T0));
    _mm_storel_epi64((__m128i*)(tmp + 8), _mm256_extracti128_si256(T0, 1));

    while (height > 0) {
        // hor
        s0 = _mm_loadu_si128((__m128i *)(src));
        s1 = _mm_loadu_si128((__m128i *)(src + i_src));
        s2 = _mm_loadu_si128((__m128i *)(src + i_src2));
        s3 = _mm_loadu_si128((__m128i *)(src + i_src3));

        s0 = _mm_unpacklo_epi64(s0, s1);
        s2 = _mm_unpacklo_epi64(s2, s3);
        T2 = _mm256_set_m128i(s2, s0);

        T0 = _mm256_shuffle_epi8(T2, mSwitch1);
        T1 = _mm256_shuffle_epi8(T2, mSwitch2);

        T0 = _mm256_maddubs_epi16(T0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(T1, mCoefy2_hor);

        T0 = _mm256_add_epi16(T0, T1);

        _mm256_storeu_si256((__m256i *)(tmp + 12), T0);
        src += i_src << 2;

        // ver
        s0 = _mm_load_si128 ((__m128i *)(tmp));
        s1 = _mm_loadu_si128((__m128i *)(tmp + 4));
        s2 = _mm_load_si128 ((__m128i *)(tmp + 8));
        s3 = _mm256_castsi256_si128(T0);
        s4 = _mm_load_si128 ((__m128i *)(tmp + 16));
        s5 = _mm256_extracti128_si256(T0, 1);

        t0 = _mm_unpacklo_epi16(s0, s1);
        t1 = _mm_unpacklo_epi16(s2, s3);
        t2 = _mm_unpackhi_epi16(s0, s1);
        t3 = _mm_unpackhi_epi16(s2, s3);
        t4 = _mm_unpacklo_epi16(s4, s5);
        t5 = _mm_unpackhi_epi16(s4, s5);

        T0 = _mm256_set_m128i(t2, t0);
        T1 = _mm256_set_m128i(t3, t1);
        T2 = _mm256_set_m128i(t5, t4);

        R0 = _mm256_madd_epi16(T0, mCoefy1_ver);
        R1 = _mm256_madd_epi16(T1, mCoefy2_ver);
        R2 = _mm256_madd_epi16(T1, mCoefy1_ver);
        R3 = _mm256_madd_epi16(T2, mCoefy2_ver);

        R0 = _mm256_add_epi32(R0, R1);
        R2 = _mm256_add_epi32(R2, R3);

        R0 = _mm256_add_epi32(R0, mAddOffset);
        R2 = _mm256_add_epi32(R2, mAddOffset);
        R0 = _mm256_srai_epi32(R0, shift);
        R2 = _mm256_srai_epi32(R2, shift);

        R0 = _mm256_packs_epi32(R0, R2);  // row0 row2 row1 row3
        s0 = _mm_packus_epi16(_mm256_castsi256_si128(R0), _mm256_extracti128_si256(R0, 1));

        M32(dst) = _mm_extract_epi32(s0, 0);
        M32(dst + i_dst) = _mm_extract_epi32(s0, 2);
        M32(dst + i_dst2) = _mm_extract_epi32(s0, 1);
        M32(dst + i_dst3) = _mm_extract_epi32(s0, 3);

        tmp += 16;
        dst += i_dst << 2;
        height -= 4;
    }
}

void uavs3e_if_hor_ver_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(64 + 3) * 8]);
    s16 *tmp = tmp_res;
    int row;

    //HOR
    {
        __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coef_x);
        __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 2));
        __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        __m256i T0, T1, S0;
        __m128i s0, s1;
        int i_src2 = i_src << 1;

        src = src - i_src - 1;
        row = height + 2; // height + 3 - 1

        while (row) {
            s0 = _mm_loadu_si128((__m128i *)(src));
            s1 = _mm_loadu_si128((__m128i *)(src + i_src));
            uavs3e_prefetch(src + i_src2, _MM_HINT_NTA);

            S0 = _mm256_set_m128i(s1, s0);

            T0 = _mm256_shuffle_epi8(S0, mSwitch1);
            T1 = _mm256_shuffle_epi8(S0, mSwitch2);

            T0 = _mm256_maddubs_epi16(T0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(T1, mCoefy2_hor);

            T0 = _mm256_add_epi16(T0, T1);

            _mm256_store_si256((__m256i *)(tmp), T0);
            row -= 2;
            src += i_src2;
            tmp += 16;
        }
        // last row
        {
            s0 = _mm_loadu_si128((__m128i *)(src));

            S0 = _mm256_set_m128i(s0, s0);

            T0 = _mm256_shuffle_epi8(S0, mSwitch1);
            T1 = _mm256_shuffle_epi8(S0, mSwitch2);

            T0 = _mm256_maddubs_epi16(T0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(T1, mCoefy2_hor);

            T0 = _mm256_add_epi16(T0, T1);

            _mm_store_si128((__m128i *)(tmp), _mm256_castsi256_si128(T0));
        }

    }
    // VER
    {
        __m128i s0, s1;
        __m128i mCoefy11 = _mm_set1_epi16(*(s16 *)coef_y);
        __m128i mCoefy22 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
        __m256i mCoefy1_ver = _mm256_cvtepi8_epi16(mCoefy11);
        __m256i mCoefy2_ver = _mm256_cvtepi8_epi16(mCoefy22);
        __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
        __m256i T4, T5, S5;
        __m256i R0, R1, R2, R3;
        __m256i T0, T1, T2, T3, S0, S1, S2, S3, S4;
        __m256i mVal1, mVal2;
        int shift = 12;
        tmp = tmp_res;
        while (height > 0) {
            S0 = _mm256_load_si256((__m256i *)(tmp));
            S1 = _mm256_loadu_si256((__m256i *)(tmp + 8));
            S2 = _mm256_load_si256((__m256i *)(tmp + 16));
            S3 = _mm256_loadu_si256((__m256i *)(tmp + 24));
            S4 = _mm256_load_si256((__m256i *)(tmp + 32));
            S5 = _mm256_loadu_si256((__m256i *)(tmp + 40));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpacklo_epi16(S2, S3);
            T2 = _mm256_unpackhi_epi16(S0, S1);
            T3 = _mm256_unpackhi_epi16(S2, S3);
            T4 = _mm256_unpacklo_epi16(S4, S5);
            T5 = _mm256_unpackhi_epi16(S4, S5);

            R0 = _mm256_madd_epi16(T0, mCoefy1_ver);
            R1 = _mm256_madd_epi16(T1, mCoefy2_ver);
            R2 = _mm256_madd_epi16(T2, mCoefy1_ver);
            R3 = _mm256_madd_epi16(T3, mCoefy2_ver);

            mVal1 = _mm256_add_epi32(R0, R1);
            mVal2 = _mm256_add_epi32(R2, R3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal1 = _mm256_packs_epi32(mVal1, mVal2);
            s0 = _mm_packus_epi16(_mm256_castsi256_si128(mVal1), _mm256_extracti128_si256(mVal1, 1));
            s1 = _mm_srli_si128(s0, 8);

            _mm_storel_epi64((__m128i *)(dst), s0);
            _mm_storel_epi64((__m128i *)(dst + i_dst), s1);

            T0 = _mm256_madd_epi16(T1, mCoefy1_ver);
            T1 = _mm256_madd_epi16(T4, mCoefy2_ver);
            T2 = _mm256_madd_epi16(T3, mCoefy1_ver);
            T3 = _mm256_madd_epi16(T5, mCoefy2_ver);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T2, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal1 = _mm256_packs_epi32(mVal1, mVal2);
            s0 = _mm_packus_epi16(_mm256_castsi256_si128(mVal1), _mm256_extracti128_si256(mVal1, 1));
            s1 = _mm_srli_si128(s0, 8);

            _mm_storel_epi64((__m128i *)(dst + i_dst * 2), s0);
            _mm_storel_epi64((__m128i *)(dst + i_dst * 3), s1);

            tmp += 32;
            dst += i_dst << 2;
            height -= 4;
        }
    }
}

void uavs3e_if_hor_ver_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(128 + 3) * 16]);
    s16 *tmp = tmp_res;
    const int i_tmp  = 16;
    const int i_tmp2 = 32;
    const int i_tmp3 = 48;
    const int i_tmp4 = 64;
    int row;
    int shift = 12;
    __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coef_x);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, S4, R0, R1, sum;
    __m128i mCoefy11 = _mm_set1_epi16(*(s16 *)coef_y);
    __m128i mCoefy22 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    __m256i mVal1, mVal2, mVal0;

    __m256i mCoefy1_ver = _mm256_cvtepi8_epi16(mCoefy11);
    __m256i mCoefy2_ver = _mm256_cvtepi8_epi16(mCoefy22);
    int i_dst2 = i_dst * 2;

    //HOR
    src = src - i_src - 1;
    row = height + 3;

    while (row--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_permute4x64_epi64(S0, 0x94);
        R0 = _mm256_shuffle_epi8(S1, mSwitch1);
        R1 = _mm256_shuffle_epi8(S1, mSwitch2);
        T0 = _mm256_maddubs_epi16(R0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(R1, mCoefy2_hor);
        sum = _mm256_add_epi16(T0, T1);

        _mm256_store_si256((__m256i *)(tmp), sum);
        src += i_src;
        tmp += i_tmp;
    }

    // VER
    tmp = tmp_res;
    while (height > 0) {
        S0 = _mm256_load_si256((__m256i *)(tmp));
        S1 = _mm256_load_si256((__m256i *)(tmp + i_tmp));
        S2 = _mm256_load_si256((__m256i *)(tmp + i_tmp2));
        S3 = _mm256_load_si256((__m256i *)(tmp + i_tmp3));
        S4 = _mm256_load_si256((__m256i *)(tmp + i_tmp4));

        T0 = _mm256_unpacklo_epi16(S0, S1);
        T1 = _mm256_unpacklo_epi16(S2, S3);
        T2 = _mm256_unpackhi_epi16(S0, S1);
        T3 = _mm256_unpackhi_epi16(S2, S3);

        T0 = _mm256_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm256_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm256_madd_epi16(T2, mCoefy1_ver);
        T3 = _mm256_madd_epi16(T3, mCoefy2_ver);

        mVal1 = _mm256_add_epi32(T0, T1);
        mVal2 = _mm256_add_epi32(T2, T3);

        mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi32(mVal1, shift);
        mVal2 = _mm256_srai_epi32(mVal2, shift);

        mVal0 = _mm256_packs_epi32(mVal1, mVal2);

        T0 = _mm256_unpacklo_epi16(S1, S2);
        T1 = _mm256_unpacklo_epi16(S3, S4);
        T2 = _mm256_unpackhi_epi16(S1, S2);
        T3 = _mm256_unpackhi_epi16(S3, S4);

        T0 = _mm256_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm256_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm256_madd_epi16(T2, mCoefy1_ver);
        T3 = _mm256_madd_epi16(T3, mCoefy2_ver);

        mVal1 = _mm256_add_epi32(T0, T1);
        mVal2 = _mm256_add_epi32(T2, T3);

        mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm256_srai_epi32(mVal1, shift);
        mVal2 = _mm256_srai_epi32(mVal2, shift);

        mVal1 = _mm256_packs_epi32(mVal1, mVal2);

        _mm_storeu_si128((__m128i *)(dst), _mm_packus_epi16(_mm256_castsi256_si128(mVal0), _mm256_extracti128_si256(mVal0, 1)));
        _mm_storeu_si128((__m128i *)(dst + i_dst), _mm_packus_epi16(_mm256_castsi256_si128(mVal1), _mm256_extracti128_si256(mVal1, 1)));

        tmp += i_tmp2;
        dst += i_dst2;
        height -= 2;
    }
}

void uavs3e_if_hor_ver_chroma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(128 + 3) * 128]);
    s16 *tmp = tmp_res;
    const int i_tmp = width;
    const int i_tmp2 = width << 1;
    const int i_tmp3 = width + i_tmp2;
    const int i_tmp4 = width << 2;
    const int i_tmp5 = i_tmp2 + i_tmp3;
    const int i_tmp6 = i_tmp3 << 1;

    int row, col;
    int shift = 12;
    __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coef_x);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 2));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, S4, S5, S6, R0, R1, R2, R3, sum0, sum1;

    __m128i mCoefy11 = _mm_set1_epi16(*(s16 *)coef_y);
    __m128i mCoefy22 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    __m256i mVal1, mVal2, mVal;

    __m256i mCoefy1_ver = _mm256_cvtepi8_epi16(mCoefy11);
    __m256i mCoefy2_ver = _mm256_cvtepi8_epi16(mCoefy22);

    //HOR
    src = src - i_src - 1;
    row = height + 3;

    while (row--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 32) {
            S0 = _mm256_loadu_si256((__m256i *)(src + col));
            S1 = _mm256_loadu_si256((__m256i *)(src + col + 16));
            S2 = _mm256_permute4x64_epi64(S0, 0x94);
            S3 = _mm256_permute4x64_epi64(S1, 0x94);
            R0 = _mm256_shuffle_epi8(S2, mSwitch1);
            R1 = _mm256_shuffle_epi8(S2, mSwitch2);
            R2 = _mm256_shuffle_epi8(S3, mSwitch1);
            R3 = _mm256_shuffle_epi8(S3, mSwitch2);
            T0 = _mm256_maddubs_epi16(R0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(R1, mCoefy2_hor);
            T2 = _mm256_maddubs_epi16(R2, mCoefy1_hor);
            T3 = _mm256_maddubs_epi16(R3, mCoefy2_hor);
            sum0 = _mm256_add_epi16(T0, T1);
            sum1 = _mm256_add_epi16(T2, T3);

            _mm256_store_si256((__m256i *)(tmp + col), sum0);
            _mm256_store_si256((__m256i *)(tmp + col + 16), sum1);
        }
        src += i_src;
        tmp += i_tmp;
    }

    // VER
    tmp = tmp_res;

    while (height) {
        for (col = 0; col < width; col += 16) {
            S0 = _mm256_load_si256((__m256i *)(tmp + col));
            S1 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp));
            S2 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp2));
            S3 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp3));
            S4 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp4));
            S5 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp5));
            S6 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp6));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpacklo_epi16(S2, S3);
            T2 = _mm256_unpackhi_epi16(S0, S1);
            T3 = _mm256_unpackhi_epi16(S2, S3);

            T0 = _mm256_madd_epi16(T0, mCoefy1_ver);
            T1 = _mm256_madd_epi16(T1, mCoefy2_ver);
            T2 = _mm256_madd_epi16(T2, mCoefy1_ver);
            T3 = _mm256_madd_epi16(T3, mCoefy2_ver);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T2, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

            T0 = _mm256_unpacklo_epi16(S1, S2);
            T1 = _mm256_unpacklo_epi16(S3, S4);
            T2 = _mm256_unpackhi_epi16(S1, S2);
            T3 = _mm256_unpackhi_epi16(S3, S4);

            T0 = _mm256_madd_epi16(T0, mCoefy1_ver);
            T1 = _mm256_madd_epi16(T1, mCoefy2_ver);
            T2 = _mm256_madd_epi16(T2, mCoefy1_ver);
            T3 = _mm256_madd_epi16(T3, mCoefy2_ver);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T2, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst + i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

            T0 = _mm256_unpacklo_epi16(S2, S3);
            T1 = _mm256_unpacklo_epi16(S4, S5);
            T2 = _mm256_unpackhi_epi16(S2, S3);
            T3 = _mm256_unpackhi_epi16(S4, S5);

            T0 = _mm256_madd_epi16(T0, mCoefy1_ver);
            T1 = _mm256_madd_epi16(T1, mCoefy2_ver);
            T2 = _mm256_madd_epi16(T2, mCoefy1_ver);
            T3 = _mm256_madd_epi16(T3, mCoefy2_ver);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T2, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst + 2 * i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

            T0 = _mm256_unpacklo_epi16(S3, S4);
            T1 = _mm256_unpacklo_epi16(S5, S6);
            T2 = _mm256_unpackhi_epi16(S3, S4);
            T3 = _mm256_unpackhi_epi16(S5, S6);

            T0 = _mm256_madd_epi16(T0, mCoefy1_ver);
            T1 = _mm256_madd_epi16(T1, mCoefy2_ver);
            T2 = _mm256_madd_epi16(T2, mCoefy1_ver);
            T3 = _mm256_madd_epi16(T3, mCoefy2_ver);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T2, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst + 3 * i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));
        }
        tmp += 4 * i_tmp;
        dst += 4 * i_dst;
        height -= 4;
    }
}

void uavs3e_if_hor_ver_luma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(32 + 7) * 4]);
    s16 *tmp = tmp_res;
    int shift = 12;
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12, 0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14, 2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16*)coef_x);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16*)(coef_x + 2));
    __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16*)(coef_x + 4));
    __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16*)(coef_x + 6));
    __m256i S0, S1, S2, S3;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i r0, r1, r2, r3, r4, r5, r6, r7;
    __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
    __m128i mCoefy11 = _mm_set1_epi16(*(s16*)coef_y);
    __m128i mCoefy22 = _mm_set1_epi16(*(s16*)(coef_y + 2));
    __m128i mCoefy33 = _mm_set1_epi16(*(s16*)(coef_y + 4));
    __m128i mCoefy44 = _mm_set1_epi16(*(s16*)(coef_y + 6));
    __m256i mCoefy1 = _mm256_cvtepi8_epi16(mCoefy11);
    __m256i mCoefy2 = _mm256_cvtepi8_epi16(mCoefy22);
    __m256i mCoefy3 = _mm256_cvtepi8_epi16(mCoefy33);
    __m256i mCoefy4 = _mm256_cvtepi8_epi16(mCoefy44);
    __m128i s0, s1, s2, s3;
    const int i_src2 = i_src << 1;
    const int i_src3 = i_src + i_src2;
    const int i_src4 = i_src << 2;

    src = src - 3 * i_src - 3;

    // hor
    // first 7 rows
    s0 = _mm_loadu_si128((__m128i*)(src));
    s1 = _mm_loadu_si128((__m128i*)(src + i_src));
    s2 = _mm_loadu_si128((__m128i*)(src + i_src2));

    S0 = _mm256_set_m128i(s2, s0);
    S1 = _mm256_set_m128i(s1, s1);

    S2 = _mm256_srli_si256(S0, 4);
    S3 = _mm256_srli_si256(S1, 4);

    T0 = _mm256_unpacklo_epi64(S0, S1);
    T1 = _mm256_unpacklo_epi64(S2, S3);

    r0 = _mm256_shuffle_epi8(T0, mSwitch1);
    r1 = _mm256_shuffle_epi8(T0, mSwitch2);
    r2 = _mm256_shuffle_epi8(T1, mSwitch1);
    r3 = _mm256_shuffle_epi8(T1, mSwitch2);

    T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
    T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
    T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
    T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

    T0 = _mm256_add_epi16(T0, T1);
    T1 = _mm256_add_epi16(T2, T3);
    T0 = _mm256_add_epi16(T0, T1);

    _mm_storeu_si128((__m128i*)(tmp), _mm256_castsi256_si128(T0));
    _mm_storel_epi64((__m128i*)(tmp + 8), _mm256_extracti128_si256(T0, 1));

    src += i_src3;

    s0 = _mm_loadu_si128((__m128i*)(src));
    s1 = _mm_loadu_si128((__m128i*)(src + i_src));
    s2 = _mm_loadu_si128((__m128i*)(src + i_src * 2));
    s3 = _mm_loadu_si128((__m128i*)(src + i_src * 3));

    S0 = _mm256_set_m128i(s2, s0);
    S1 = _mm256_set_m128i(s3, s1);

    S2 = _mm256_srli_si256(S0, 4);
    S3 = _mm256_srli_si256(S1, 4);

    T0 = _mm256_unpacklo_epi64(S0, S1);
    T1 = _mm256_unpacklo_epi64(S2, S3);

    r0 = _mm256_shuffle_epi8(T0, mSwitch1);
    r1 = _mm256_shuffle_epi8(T0, mSwitch2);
    r2 = _mm256_shuffle_epi8(T1, mSwitch1);
    r3 = _mm256_shuffle_epi8(T1, mSwitch2);

    T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
    T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
    T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
    T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

    T0 = _mm256_add_epi16(T0, T1);
    T1 = _mm256_add_epi16(T2, T3);
    T0 = _mm256_add_epi16(T0, T1);

    _mm256_storeu_si256((__m256i*)(tmp + 12), T0);

    src += i_src4;

    while (height > 0) {
        __m128i d0;
        // hor
        s0 = _mm_loadu_si128((__m128i*)(src));
        s1 = _mm_loadu_si128((__m128i*)(src + i_src));
        s2 = _mm_loadu_si128((__m128i*)(src + i_src * 2));
        s3 = _mm_loadu_si128((__m128i*)(src + i_src * 3));

        S0 = _mm256_set_m128i(s2, s0);
        S1 = _mm256_set_m128i(s3, s1);

        S2 = _mm256_srli_si256(S0, 4);
        S3 = _mm256_srli_si256(S1, 4);

        T0 = _mm256_unpacklo_epi64(S0, S1);
        T1 = _mm256_unpacklo_epi64(S2, S3);

        r0 = _mm256_shuffle_epi8(T0, mSwitch1);
        r1 = _mm256_shuffle_epi8(T0, mSwitch2);
        r2 = _mm256_shuffle_epi8(T1, mSwitch1);
        r3 = _mm256_shuffle_epi8(T1, mSwitch2);

        T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        T0 = _mm256_add_epi16(T0, T1);
        T1 = _mm256_add_epi16(T2, T3);
        T7 = _mm256_add_epi16(T0, T1);
        _mm256_storeu_si256((__m256i*)(tmp + 28), T7);

        src += i_src4;

        // ver
        T0 = _mm256_load_si256((__m256i*)(tmp));
        T1 = _mm256_loadu_si256((__m256i*)(tmp + 4));
        T2 = _mm256_loadu_si256((__m256i*)(tmp + 8));
        T3 = _mm256_loadu_si256((__m256i*)(tmp + 12));
        T4 = _mm256_load_si256((__m256i*)(tmp + 16));
        T5 = _mm256_loadu_si256((__m256i*)(tmp + 20));
        T6 = _mm256_loadu_si256((__m256i*)(tmp + 24));

        r0 = _mm256_unpacklo_epi16(T0, T1);     // line0-1 line2-3
        r1 = _mm256_unpacklo_epi16(T2, T3);
        r2 = _mm256_unpacklo_epi16(T4, T5);
        r3 = _mm256_unpacklo_epi16(T6, T7);
        r4 = _mm256_unpackhi_epi16(T0, T1);     // line1-2 line3-4
        r5 = _mm256_unpackhi_epi16(T2, T3);
        r6 = _mm256_unpackhi_epi16(T4, T5);
        r7 = _mm256_unpackhi_epi16(T6, T7);

        T0 = _mm256_madd_epi16(r0, mCoefy1);
        T1 = _mm256_madd_epi16(r1, mCoefy2);
        T2 = _mm256_madd_epi16(r2, mCoefy3);
        T3 = _mm256_madd_epi16(r3, mCoefy4);
        T4 = _mm256_madd_epi16(r4, mCoefy1);
        T5 = _mm256_madd_epi16(r5, mCoefy2);
        T6 = _mm256_madd_epi16(r6, mCoefy3);
        T7 = _mm256_madd_epi16(r7, mCoefy4);

        T0 = _mm256_add_epi32(T0, T1);
        T2 = _mm256_add_epi32(T2, T3);
        T4 = _mm256_add_epi32(T4, T5);
        T6 = _mm256_add_epi32(T6, T7);
        T0 = _mm256_add_epi32(T0, T2);
        T4 = _mm256_add_epi32(T4, T6);
        T0 = _mm256_add_epi32(T0, mAddOffset);
        T4 = _mm256_add_epi32(T4, mAddOffset);
        T0 = _mm256_srai_epi32(T0, shift);    // dst line0, line2
        T4 = _mm256_srai_epi32(T4, shift);    // dst line1, line3

        T0 = _mm256_packs_epi32(T0, T4);
        d0 = _mm_packus_epi16(_mm256_castsi256_si128(T0), _mm256_extracti128_si256(T0, 1));

        M32(dst) = _mm_extract_epi32(d0, 0);
        M32(dst + i_dst) = _mm_extract_epi32(d0, 1);
        M32(dst + i_dst * 2) = _mm_extract_epi32(d0, 2);
        M32(dst + i_dst * 3) = _mm_extract_epi32(d0, 3);

        tmp += 16;
        dst += i_dst << 2;
        height -= 4;
    }
}

void uavs3e_if_hor_ver_luma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    const int i_tmp = 8;
    const int i_tmp2 = 16;
    const int i_tmp3 = 24;
    const int i_tmp4 = 32;
    const int i_tmp5 = 40;
    const int i_tmp6 = 48;
    const int i_tmp7 = 56;;
    const int i_tmp8 = 64;
    const int i_tmp9 = 72;;
    const int i_tmp10 = 80;
    const int i_src2 = i_src << 1;
    int row;
    int shift = 12;

    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i r0, r1, r2, r3, r4, r5, r6, r7;
    __m256i mVal[6];
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);
    __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16*)coef_x);
    __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16*)(coef_x + 2));
    __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16*)(coef_x + 4));
    __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16*)(coef_x + 6));

    src = src - 3 * i_src - 3;

    // first row
    {
        __m128i mSrc0 = _mm_loadu_si128((__m128i*)(src));
        T0 = _mm256_set_m128i(mSrc0, mSrc0);

        r0 = _mm256_shuffle_epi8(T0, mSwitch1);
        r1 = _mm256_shuffle_epi8(T0, mSwitch2);
        r2 = _mm256_shuffle_epi8(T0, mSwitch3);
        r3 = _mm256_shuffle_epi8(T0, mSwitch4);

        T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        T0 = _mm256_add_epi16(T0, T1);
        T1 = _mm256_add_epi16(T2, T3);
        mVal[0] = _mm256_add_epi16(T0, T1);

        mVal[0] = _mm256_permute4x64_epi64(mVal[0], 0x44);

        src += i_src;
    }

    // row[1-6]
    for (row = 1; row < 4; row++) {
        __m128i mSrc0 = _mm_loadu_si128((__m128i*)(src));
        __m128i mSrc1 = _mm_loadu_si128((__m128i*)(src + i_src));
        T0 = _mm256_set_m128i(mSrc1, mSrc0);

        r0 = _mm256_shuffle_epi8(T0, mSwitch1);
        r1 = _mm256_shuffle_epi8(T0, mSwitch2);
        r2 = _mm256_shuffle_epi8(T0, mSwitch3);
        r3 = _mm256_shuffle_epi8(T0, mSwitch4);

        T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
        T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
        T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
        T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

        T0 = _mm256_add_epi16(T0, T1);
        T1 = _mm256_add_epi16(T2, T3);
        mVal[row] = _mm256_add_epi16(T0, T1);

        src += i_src2;
    }

    {
        __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
        __m128i mCoefy11 = _mm_set1_epi16(*(s16*)coef_y);
        __m128i mCoefy22 = _mm_set1_epi16(*(s16*)(coef_y + 2));
        __m128i mCoefy33 = _mm_set1_epi16(*(s16*)(coef_y + 4));
        __m128i mCoefy44 = _mm_set1_epi16(*(s16*)(coef_y + 6));
        __m256i mCoefy1 = _mm256_cvtepi8_epi16(mCoefy11);
        __m256i mCoefy2 = _mm256_cvtepi8_epi16(mCoefy22);
        __m256i mCoefy3 = _mm256_cvtepi8_epi16(mCoefy33);
        __m256i mCoefy4 = _mm256_cvtepi8_epi16(mCoefy44);

        while (height > 0) {
            __m128i s0, s1;
            //hor
            s0 = _mm_loadu_si128((__m128i*)(src));
            s1 = _mm_loadu_si128((__m128i*)(src + i_src));
            T0 = _mm256_set_m128i(s1, s0);

            r0 = _mm256_shuffle_epi8(T0, mSwitch1);
            r1 = _mm256_shuffle_epi8(T0, mSwitch2);
            r2 = _mm256_shuffle_epi8(T0, mSwitch3);
            r3 = _mm256_shuffle_epi8(T0, mSwitch4);

            src += i_src2;

            T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
            T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
            T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

            s0 = _mm_loadu_si128((__m128i*)(src));
            s1 = _mm_loadu_si128((__m128i*)(src + i_src));

            T0 = _mm256_add_epi16(T0, T1);
            T1 = _mm256_add_epi16(T2, T3);
            mVal[4] = _mm256_add_epi16(T0, T1);

            T0 = _mm256_set_m128i(s1, s0);

            r0 = _mm256_shuffle_epi8(T0, mSwitch1);
            r1 = _mm256_shuffle_epi8(T0, mSwitch2);
            r2 = _mm256_shuffle_epi8(T0, mSwitch3);
            r3 = _mm256_shuffle_epi8(T0, mSwitch4);

            T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
            T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
            T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

            T0 = _mm256_add_epi16(T0, T1);
            T1 = _mm256_add_epi16(T2, T3);
            mVal[5] = _mm256_add_epi16(T0, T1);

            src += i_src2;

            T0 = _mm256_permute2x128_si256(mVal[0], mVal[1], 0x21);
            T1 = mVal[1];
            T2 = _mm256_permute2x128_si256(mVal[1], mVal[2], 0x21);
            T3 = mVal[2];
            T4 = _mm256_permute2x128_si256(mVal[2], mVal[3], 0x21);
            T5 = mVal[3];
            T6 = _mm256_permute2x128_si256(mVal[3], mVal[4], 0x21);
            T7 = mVal[4];

            mVal[0] = mVal[2];
            mVal[1] = mVal[3];
            mVal[2] = mVal[4];
            mVal[3] = mVal[5];

            r0 = _mm256_unpacklo_epi16(T0, T1);
            r1 = _mm256_unpacklo_epi16(T2, T3);
            r2 = _mm256_unpacklo_epi16(T4, T5);
            r3 = _mm256_unpacklo_epi16(T6, T7);
            r4 = _mm256_unpackhi_epi16(T0, T1);
            r5 = _mm256_unpackhi_epi16(T2, T3);
            r6 = _mm256_unpackhi_epi16(T4, T5);
            r7 = _mm256_unpackhi_epi16(T6, T7);

            T0 = _mm256_madd_epi16(r0, mCoefy1);
            T1 = _mm256_madd_epi16(r1, mCoefy2);
            T2 = _mm256_madd_epi16(r2, mCoefy3);
            T3 = _mm256_madd_epi16(r3, mCoefy4);
            T4 = _mm256_madd_epi16(r4, mCoefy1);
            T5 = _mm256_madd_epi16(r5, mCoefy2);
            T6 = _mm256_madd_epi16(r6, mCoefy3);
            T7 = _mm256_madd_epi16(r7, mCoefy4);

            T0 = _mm256_add_epi32(T0, T1);
            T2 = _mm256_add_epi32(T2, T3);
            T4 = _mm256_add_epi32(T4, T5);
            T6 = _mm256_add_epi32(T6, T7);
            T0 = _mm256_add_epi32(T0, T2);
            T4 = _mm256_add_epi32(T4, T6);
            T0 = _mm256_add_epi32(T0, mAddOffset);
            T4 = _mm256_add_epi32(T4, mAddOffset);
            T0 = _mm256_srai_epi32(T0, shift);
            T4 = _mm256_srai_epi32(T4, shift);

            T0 = _mm256_packs_epi32(T0, T4);
            s0 = _mm_packus_epi16(_mm256_castsi256_si128(T0), _mm256_extracti128_si256(T0, 1));
            s1 = _mm_srli_si128(s0, 8);

            T4 = _mm256_permute2x128_si256(mVal[2], mVal[3], 0x21);

            _mm_storel_epi64((__m128i*)(dst), s0);
            _mm_storel_epi64((__m128i*)(dst + i_dst), s1);

            r0 = _mm256_unpacklo_epi16(T4, mVal[3]);
            r4 = _mm256_unpackhi_epi16(T4, mVal[3]);

            T0 = _mm256_madd_epi16(r1, mCoefy1);
            T1 = _mm256_madd_epi16(r2, mCoefy2);
            T2 = _mm256_madd_epi16(r3, mCoefy3);
            T3 = _mm256_madd_epi16(r0, mCoefy4);
            T4 = _mm256_madd_epi16(r5, mCoefy1);
            T5 = _mm256_madd_epi16(r6, mCoefy2);
            T6 = _mm256_madd_epi16(r7, mCoefy3);
            T7 = _mm256_madd_epi16(r4, mCoefy4);

            T0 = _mm256_add_epi32(T0, T1);
            T2 = _mm256_add_epi32(T2, T3);
            T4 = _mm256_add_epi32(T4, T5);
            T6 = _mm256_add_epi32(T6, T7);
            T0 = _mm256_add_epi32(T0, T2);
            T4 = _mm256_add_epi32(T4, T6);
            T0 = _mm256_add_epi32(T0, mAddOffset);
            T4 = _mm256_add_epi32(T4, mAddOffset);
            T0 = _mm256_srai_epi32(T0, shift);
            T4 = _mm256_srai_epi32(T4, shift);

            T0 = _mm256_packs_epi32(T0, T4);
            s0 = _mm_packus_epi16(_mm256_castsi256_si128(T0), _mm256_extracti128_si256(T0, 1));
            s1 = _mm_srli_si128(s0, 8);

            _mm_storel_epi64((__m128i*)(dst + i_dst * 2), s0);
            _mm_storel_epi64((__m128i*)(dst + i_dst * 3), s1);

            dst += i_dst << 2;
            height -= 4;
        }
    }
}

void uavs3e_if_hor_ver_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(128 + 7) * 16]);
    s16 *tmp = tmp_res;
    int row;
    __m256i mVal1, mVal2, mVal;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, T8, T9;
    __m256i S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10;
    __m256i r0, r1, r2, r3;
    __m256i sum, S;

    //HOR
    {
        __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
        __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);

        __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coef_x);
        __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 2));
        __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 4));
        __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 6));

        src = src - 3 * i_src - 3;

        row = height + 7;
        while (row--) {
            S = _mm256_loadu_si256((__m256i *)(src));
            uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
            S0 = _mm256_permute4x64_epi64(S, 0x94);

            r0 = _mm256_shuffle_epi8(S0, mSwitch1);
            r1 = _mm256_shuffle_epi8(S0, mSwitch2);
            r2 = _mm256_shuffle_epi8(S0, mSwitch3);
            r3 = _mm256_shuffle_epi8(S0, mSwitch4);

            T0 = _mm256_maddubs_epi16(r0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(r1, mCoefy2_hor);
            T2 = _mm256_maddubs_epi16(r2, mCoefy3_hor);
            T3 = _mm256_maddubs_epi16(r3, mCoefy4_hor);

            T0 = _mm256_add_epi16(T0, T1);
            T1 = _mm256_add_epi16(T2, T3);
            sum = _mm256_add_epi16(T0, T1);

            _mm256_store_si256((__m256i *)(tmp), sum);

            src += i_src;
            tmp += 16;
        }
    }
    // VER
    {
        const int i_tmp = 16;
        const int i_tmp2 = 32;
        const int i_tmp3 = 16 * 3;
        const int i_tmp4 = 16 * 4;
        const int i_tmp5 = 16 * 5;
        const int i_tmp6 = 16 * 6;
        const int i_tmp7 = 16 * 7;
        const int i_tmp8 = 16 * 8;
        const int i_tmp9 = 16 * 9;
        const int i_tmp10 = 16 * 10;
        int shift = 12;
        __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
        __m128i mCoefy11 = _mm_set1_epi16(*(s16 *)coef_y);
        __m128i mCoefy22 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
        __m128i mCoefy33 = _mm_set1_epi16(*(s16 *)(coef_y + 4));
        __m128i mCoefy44 = _mm_set1_epi16(*(s16 *)(coef_y + 6));
        __m256i mCoefy1 = _mm256_cvtepi8_epi16(mCoefy11);
        __m256i mCoefy2 = _mm256_cvtepi8_epi16(mCoefy22);
        __m256i mCoefy3 = _mm256_cvtepi8_epi16(mCoefy33);
        __m256i mCoefy4 = _mm256_cvtepi8_epi16(mCoefy44);

        tmp = tmp_res;

        while (height > 0) {
            __m256i r4, r5, r6, r7;
            S0 = _mm256_load_si256((__m256i *)(tmp));
            S1 = _mm256_load_si256((__m256i *)(tmp + i_tmp));
            S2 = _mm256_load_si256((__m256i *)(tmp + i_tmp2));
            S3 = _mm256_load_si256((__m256i *)(tmp + i_tmp3));
            S4 = _mm256_load_si256((__m256i *)(tmp + i_tmp4));
            S5 = _mm256_load_si256((__m256i *)(tmp + i_tmp5));
            S6 = _mm256_load_si256((__m256i *)(tmp + i_tmp6));
            S7 = _mm256_load_si256((__m256i *)(tmp + i_tmp7));
            S8 = _mm256_load_si256((__m256i *)(tmp + i_tmp8));
            S9 = _mm256_load_si256((__m256i *)(tmp + i_tmp9));
            S10 = _mm256_load_si256((__m256i *)(tmp + i_tmp10));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpacklo_epi16(S2, S3);
            T2 = _mm256_unpacklo_epi16(S4, S5);
            T3 = _mm256_unpacklo_epi16(S6, S7);
            T4 = _mm256_unpackhi_epi16(S0, S1);
            T5 = _mm256_unpackhi_epi16(S2, S3);
            T6 = _mm256_unpackhi_epi16(S4, S5);
            T7 = _mm256_unpackhi_epi16(S6, S7);

            r0 = _mm256_madd_epi16(T0, mCoefy1);
            r1 = _mm256_madd_epi16(T1, mCoefy2);
            r2 = _mm256_madd_epi16(T2, mCoefy3);
            r3 = _mm256_madd_epi16(T3, mCoefy4);
            r4 = _mm256_madd_epi16(T4, mCoefy1);
            r5 = _mm256_madd_epi16(T5, mCoefy2);
            r6 = _mm256_madd_epi16(T6, mCoefy3);
            r7 = _mm256_madd_epi16(T7, mCoefy4);

            mVal1 = _mm256_add_epi32(r0, r1);
            mVal2 = _mm256_add_epi32(r4, r5);
            mVal1 = _mm256_add_epi32(mVal1, r2);
            mVal2 = _mm256_add_epi32(mVal2, r6);
            mVal1 = _mm256_add_epi32(mVal1, r3);
            mVal2 = _mm256_add_epi32(mVal2, r7);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

            T8 = _mm256_unpacklo_epi16(S8, S9);
            T9 = _mm256_unpackhi_epi16(S8, S9);

            T0 = _mm256_madd_epi16(T1, mCoefy1);
            T1 = _mm256_madd_epi16(T2, mCoefy2);
            T2 = _mm256_madd_epi16(T3, mCoefy3);
            T3 = _mm256_madd_epi16(T8, mCoefy4);
            T4 = _mm256_madd_epi16(T5, mCoefy1);
            T5 = _mm256_madd_epi16(T6, mCoefy2);
            T6 = _mm256_madd_epi16(T7, mCoefy3);
            T7 = _mm256_madd_epi16(T9, mCoefy4);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T4, T5);
            mVal1 = _mm256_add_epi32(mVal1, T2);
            mVal2 = _mm256_add_epi32(mVal2, T6);
            mVal1 = _mm256_add_epi32(mVal1, T3);
            mVal2 = _mm256_add_epi32(mVal2, T7);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst + 2 * i_dst), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

            T0 = _mm256_unpacklo_epi16(S1, S2);
            T1 = _mm256_unpacklo_epi16(S3, S4);
            T2 = _mm256_unpacklo_epi16(S5, S6);
            T3 = _mm256_unpacklo_epi16(S7, S8);
            T4 = _mm256_unpackhi_epi16(S1, S2);
            T5 = _mm256_unpackhi_epi16(S3, S4);
            T6 = _mm256_unpackhi_epi16(S5, S6);
            T7 = _mm256_unpackhi_epi16(S7, S8);

            r0 = _mm256_madd_epi16(T0, mCoefy1);
            r1 = _mm256_madd_epi16(T1, mCoefy2);
            r2 = _mm256_madd_epi16(T2, mCoefy3);
            r3 = _mm256_madd_epi16(T3, mCoefy4);
            r4 = _mm256_madd_epi16(T4, mCoefy1);
            r5 = _mm256_madd_epi16(T5, mCoefy2);
            r6 = _mm256_madd_epi16(T6, mCoefy3);
            r7 = _mm256_madd_epi16(T7, mCoefy4);

            mVal1 = _mm256_add_epi32(r0, r1);
            mVal2 = _mm256_add_epi32(r4, r5);
            mVal1 = _mm256_add_epi32(mVal1, r2);
            mVal2 = _mm256_add_epi32(mVal2, r6);
            mVal1 = _mm256_add_epi32(mVal1, r3);
            mVal2 = _mm256_add_epi32(mVal2, r7);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst + i_dst), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

            T8 = _mm256_unpacklo_epi16(S9, S10);
            T9 = _mm256_unpackhi_epi16(S9, S10);

            T0 = _mm256_madd_epi16(T1, mCoefy1);
            T1 = _mm256_madd_epi16(T2, mCoefy2);
            T2 = _mm256_madd_epi16(T3, mCoefy3);
            T3 = _mm256_madd_epi16(T8, mCoefy4);
            T4 = _mm256_madd_epi16(T5, mCoefy1);
            T5 = _mm256_madd_epi16(T6, mCoefy2);
            T6 = _mm256_madd_epi16(T7, mCoefy3);
            T7 = _mm256_madd_epi16(T9, mCoefy4);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T4, T5);
            mVal1 = _mm256_add_epi32(mVal1, T2);
            mVal2 = _mm256_add_epi32(mVal2, T6);
            mVal1 = _mm256_add_epi32(mVal1, T3);
            mVal2 = _mm256_add_epi32(mVal2, T7);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);

            mVal = _mm256_packs_epi32(mVal1, mVal2);
            _mm_storeu_si128((__m128i *)(dst + 3 * i_dst), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

            tmp += 4 * i_tmp;
            dst += 4 * i_dst;
            height -= 4;
        }
    }
}

void uavs3e_if_hor_ver_luma_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(128 + 7) * 32]);
    s16 *tmp = tmp_res;
    int row, col;
    const int i_tmp = 32;
    //HOR
    {
        __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
        __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);

        __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coef_x);
        __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 2));
        __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 4));
        __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 6));

        __m256i T0, T1, T2, T3, T4, T5, T6, T7;
        __m256i S0, S1, S2, S3;

        src = src - 3 * i_src - 3;

        row = height + 7;
        while (row--) {
            uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
            S0 = _mm256_loadu_si256((__m256i *)(src));
            S1 = _mm256_loadu_si256((__m256i *)(src + 8));
            S2 = _mm256_insertf128_si256(S0, _mm256_castsi256_si128(S1), 0x1);
            S3 = _mm256_insertf128_si256(S1, _mm256_extracti128_si256(S0, 1), 0x0);

            T0 = _mm256_shuffle_epi8(S2, mSwitch1);
            T1 = _mm256_shuffle_epi8(S2, mSwitch2);
            T2 = _mm256_shuffle_epi8(S2, mSwitch3);
            T3 = _mm256_shuffle_epi8(S2, mSwitch4);
            T4 = _mm256_shuffle_epi8(S3, mSwitch1);
            T5 = _mm256_shuffle_epi8(S3, mSwitch2);
            T6 = _mm256_shuffle_epi8(S3, mSwitch3);
            T7 = _mm256_shuffle_epi8(S3, mSwitch4);

            T0 = _mm256_maddubs_epi16(T0, mCoefy1_hor);
            T1 = _mm256_maddubs_epi16(T1, mCoefy2_hor);
            T2 = _mm256_maddubs_epi16(T2, mCoefy3_hor);
            T3 = _mm256_maddubs_epi16(T3, mCoefy4_hor);
            T4 = _mm256_maddubs_epi16(T4, mCoefy1_hor);
            T5 = _mm256_maddubs_epi16(T5, mCoefy2_hor);
            T6 = _mm256_maddubs_epi16(T6, mCoefy3_hor);
            T7 = _mm256_maddubs_epi16(T7, mCoefy4_hor);

            T0 = _mm256_add_epi16(T0, T1);
            T2 = _mm256_add_epi16(T2, T3);
            T4 = _mm256_add_epi16(T4, T5);
            T6 = _mm256_add_epi16(T6, T7);
            T0 = _mm256_add_epi16(T0, T2);
            T4 = _mm256_add_epi16(T4, T6);

            _mm256_store_si256((__m256i *)(tmp), T0);
            _mm256_store_si256((__m256i *)(tmp + 16), T4);

            src += i_src;
            tmp += i_tmp;
        }
    }

    // VER
    {
        const int i_tmp2 = 32 * 2;
        const int i_tmp3 = 32 * 3;
        const int i_tmp4 = 32 * 4;
        const int i_tmp5 = 32 * 5;
        const int i_tmp6 = 32 * 6;
        const int i_tmp7 = 32 * 7;;
        const int i_tmp8 = 32 * 8;
        const int i_tmp9 = 32 * 9;
        const int i_tmp10 = 32 * 10;

        int shift = 12;
        __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
        __m128i mCoefy11 = _mm_set1_epi16(*(s16 *)coef_y);
        __m128i mCoefy22 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
        __m128i mCoefy33 = _mm_set1_epi16(*(s16 *)(coef_y + 4));
        __m128i mCoefy44 = _mm_set1_epi16(*(s16 *)(coef_y + 6));
        __m256i mCoefy1 = _mm256_cvtepi8_epi16(mCoefy11);
        __m256i mCoefy2 = _mm256_cvtepi8_epi16(mCoefy22);
        __m256i mCoefy3 = _mm256_cvtepi8_epi16(mCoefy33);
        __m256i mCoefy4 = _mm256_cvtepi8_epi16(mCoefy44);
        __m256i mVal1, mVal2, mVal;
        __m256i T0, T1, T2, T3, T4, T5, T6, T7, T8, T9;
        __m256i S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10;
        __m256i r0, r1, r2, r3;

        tmp = tmp_res;

        while (height) {
            __m256i r4, r5, r6, r7;
            for (col = 0; col < width; col += 16) {
                S0 = _mm256_load_si256((__m256i *)(tmp + col));
                S1 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp));
                S2 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp2));
                S3 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp3));
                S4 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp4));
                S5 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp5));
                S6 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp6));
                S7 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp7));
                S8 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp8));
                S9 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp9));
                S10 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp10));

                T0 = _mm256_unpacklo_epi16(S0, S1);
                T1 = _mm256_unpacklo_epi16(S2, S3);
                T2 = _mm256_unpacklo_epi16(S4, S5);
                T3 = _mm256_unpacklo_epi16(S6, S7);
                T4 = _mm256_unpackhi_epi16(S0, S1);
                T5 = _mm256_unpackhi_epi16(S2, S3);
                T6 = _mm256_unpackhi_epi16(S4, S5);
                T7 = _mm256_unpackhi_epi16(S6, S7);

                r0 = _mm256_madd_epi16(T0, mCoefy1);
                r1 = _mm256_madd_epi16(T1, mCoefy2);
                r2 = _mm256_madd_epi16(T2, mCoefy3);
                r3 = _mm256_madd_epi16(T3, mCoefy4);
                r4 = _mm256_madd_epi16(T4, mCoefy1);
                r5 = _mm256_madd_epi16(T5, mCoefy2);
                r6 = _mm256_madd_epi16(T6, mCoefy3);
                r7 = _mm256_madd_epi16(T7, mCoefy4);

                mVal1 = _mm256_add_epi32(r0, r1);
                mVal2 = _mm256_add_epi32(r4, r5);
                mVal1 = _mm256_add_epi32(mVal1, r2);
                mVal2 = _mm256_add_epi32(mVal2, r6);
                mVal1 = _mm256_add_epi32(mVal1, r3);
                mVal2 = _mm256_add_epi32(mVal2, r7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

                T8 = _mm256_unpacklo_epi16(S8, S9);
                T9 = _mm256_unpackhi_epi16(S8, S9);

                T0 = _mm256_madd_epi16(T1, mCoefy1);
                T1 = _mm256_madd_epi16(T2, mCoefy2);
                T2 = _mm256_madd_epi16(T3, mCoefy3);
                T3 = _mm256_madd_epi16(T8, mCoefy4);
                T4 = _mm256_madd_epi16(T5, mCoefy1);
                T5 = _mm256_madd_epi16(T6, mCoefy2);
                T6 = _mm256_madd_epi16(T7, mCoefy3);
                T7 = _mm256_madd_epi16(T9, mCoefy4);

                mVal1 = _mm256_add_epi32(T0, T1);
                mVal2 = _mm256_add_epi32(T4, T5);
                mVal1 = _mm256_add_epi32(mVal1, T2);
                mVal2 = _mm256_add_epi32(mVal2, T6);
                mVal1 = _mm256_add_epi32(mVal1, T3);
                mVal2 = _mm256_add_epi32(mVal2, T7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + 2 * i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

                T0 = _mm256_unpacklo_epi16(S1, S2);
                T1 = _mm256_unpacklo_epi16(S3, S4);
                T2 = _mm256_unpacklo_epi16(S5, S6);
                T3 = _mm256_unpacklo_epi16(S7, S8);
                T4 = _mm256_unpackhi_epi16(S1, S2);
                T5 = _mm256_unpackhi_epi16(S3, S4);
                T6 = _mm256_unpackhi_epi16(S5, S6);
                T7 = _mm256_unpackhi_epi16(S7, S8);

                r0 = _mm256_madd_epi16(T0, mCoefy1);
                r1 = _mm256_madd_epi16(T1, mCoefy2);
                r2 = _mm256_madd_epi16(T2, mCoefy3);
                r3 = _mm256_madd_epi16(T3, mCoefy4);
                r4 = _mm256_madd_epi16(T4, mCoefy1);
                r5 = _mm256_madd_epi16(T5, mCoefy2);
                r6 = _mm256_madd_epi16(T6, mCoefy3);
                r7 = _mm256_madd_epi16(T7, mCoefy4);

                mVal1 = _mm256_add_epi32(r0, r1);
                mVal2 = _mm256_add_epi32(r4, r5);
                mVal1 = _mm256_add_epi32(mVal1, r2);
                mVal2 = _mm256_add_epi32(mVal2, r6);
                mVal1 = _mm256_add_epi32(mVal1, r3);
                mVal2 = _mm256_add_epi32(mVal2, r7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

                T8 = _mm256_unpacklo_epi16(S9, S10);
                T9 = _mm256_unpackhi_epi16(S9, S10);

                T0 = _mm256_madd_epi16(T1, mCoefy1);
                T1 = _mm256_madd_epi16(T2, mCoefy2);
                T2 = _mm256_madd_epi16(T3, mCoefy3);
                T3 = _mm256_madd_epi16(T8, mCoefy4);
                T4 = _mm256_madd_epi16(T5, mCoefy1);
                T5 = _mm256_madd_epi16(T6, mCoefy2);
                T6 = _mm256_madd_epi16(T7, mCoefy3);
                T7 = _mm256_madd_epi16(T9, mCoefy4);

                mVal1 = _mm256_add_epi32(T0, T1);
                mVal2 = _mm256_add_epi32(T4, T5);
                mVal1 = _mm256_add_epi32(mVal1, T2);
                mVal2 = _mm256_add_epi32(mVal2, T6);
                mVal1 = _mm256_add_epi32(mVal1, T3);
                mVal2 = _mm256_add_epi32(mVal2, T7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + 3 * i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));
            }
            tmp += 4 * i_tmp;
            dst += 4 * i_dst;
            height -= 4;
        }
    }
}

void uavs3e_if_hor_ver_luma_w32x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(128 + 7) * 128]);
    s16 *tmp = tmp_res;
    int row, col;
    const int i_tmp = width;
    __m256i mVal1, mVal2, mVal;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, T8, T9;
    __m256i S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10;
    __m256i r0, r1, r2, r3;

    //HOR
    {
        __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
        __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);

        __m256i mCoefy1_hor = _mm256_set1_epi16(*(s16 *)coef_x);
        __m256i mCoefy2_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 2));
        __m256i mCoefy3_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 4));
        __m256i mCoefy4_hor = _mm256_set1_epi16(*(s16 *)(coef_x + 6));

        src = src - 3 * i_src - 3;

        row = height + 7;
        while (row--) {
            uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
            for (col = 0; col < width; col += 32) {
                S0 = _mm256_loadu_si256((__m256i *)(src + col));
                S1 = _mm256_loadu_si256((__m256i *)(src + col + 8));
                S2 = _mm256_insertf128_si256(S0, _mm256_castsi256_si128(S1), 0x1);
                S3 = _mm256_insertf128_si256(S1, _mm256_extracti128_si256(S0, 1), 0x0);

                T0 = _mm256_shuffle_epi8(S2, mSwitch1);
                T1 = _mm256_shuffle_epi8(S2, mSwitch2);
                T2 = _mm256_shuffle_epi8(S2, mSwitch3);
                T3 = _mm256_shuffle_epi8(S2, mSwitch4);
                T4 = _mm256_shuffle_epi8(S3, mSwitch1);
                T5 = _mm256_shuffle_epi8(S3, mSwitch2);
                T6 = _mm256_shuffle_epi8(S3, mSwitch3);
                T7 = _mm256_shuffle_epi8(S3, mSwitch4);

                T0 = _mm256_maddubs_epi16(T0, mCoefy1_hor);
                T1 = _mm256_maddubs_epi16(T1, mCoefy2_hor);
                T2 = _mm256_maddubs_epi16(T2, mCoefy3_hor);
                T3 = _mm256_maddubs_epi16(T3, mCoefy4_hor);
                T4 = _mm256_maddubs_epi16(T4, mCoefy1_hor);
                T5 = _mm256_maddubs_epi16(T5, mCoefy2_hor);
                T6 = _mm256_maddubs_epi16(T6, mCoefy3_hor);
                T7 = _mm256_maddubs_epi16(T7, mCoefy4_hor);

                T0 = _mm256_add_epi16(T0, T1);
                T2 = _mm256_add_epi16(T2, T3);
                T4 = _mm256_add_epi16(T4, T5);
                T6 = _mm256_add_epi16(T6, T7);
                T0 = _mm256_add_epi16(T0, T2);
                T4 = _mm256_add_epi16(T4, T6);

                _mm256_store_si256((__m256i *)(tmp + col), T0);
                _mm256_store_si256((__m256i *)(tmp + col + 16), T4);
            }
            src += i_src;
            tmp += i_tmp;
        }
    }

    // VER
    {
        const int i_tmp2 = width << 1;
        const int i_tmp3 = width + i_tmp2;
        const int i_tmp4 = width << 2;
        const int i_tmp5 = i_tmp2 + i_tmp3;
        const int i_tmp6 = i_tmp3 << 1;
        const int i_tmp7 = i_tmp4 + i_tmp3;;
        const int i_tmp8 = width << 3;
        const int i_tmp9 = i_tmp4 + i_tmp5;;
        const int i_tmp10 = i_tmp5 << 1;

        int shift = 12;
        __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
        __m128i mCoefy11 = _mm_set1_epi16(*(s16 *)coef_y);
        __m128i mCoefy22 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
        __m128i mCoefy33 = _mm_set1_epi16(*(s16 *)(coef_y + 4));
        __m128i mCoefy44 = _mm_set1_epi16(*(s16 *)(coef_y + 6));
        __m256i mCoefy1 = _mm256_cvtepi8_epi16(mCoefy11);
        __m256i mCoefy2 = _mm256_cvtepi8_epi16(mCoefy22);
        __m256i mCoefy3 = _mm256_cvtepi8_epi16(mCoefy33);
        __m256i mCoefy4 = _mm256_cvtepi8_epi16(mCoefy44);

        tmp = tmp_res;

        while (height) {
            __m256i r4, r5, r6, r7;
            for (col = 0; col < width; col += 16) {
                S0 = _mm256_load_si256((__m256i *)(tmp + col));
                S1 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp));
                S2 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp2));
                S3 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp3));
                S4 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp4));
                S5 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp5));
                S6 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp6));
                S7 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp7));
                S8 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp8));
                S9 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp9));
                S10 = _mm256_load_si256((__m256i *)(tmp + col + i_tmp10));

                T0 = _mm256_unpacklo_epi16(S0, S1);
                T1 = _mm256_unpacklo_epi16(S2, S3);
                T2 = _mm256_unpacklo_epi16(S4, S5);
                T3 = _mm256_unpacklo_epi16(S6, S7);
                T4 = _mm256_unpackhi_epi16(S0, S1);
                T5 = _mm256_unpackhi_epi16(S2, S3);
                T6 = _mm256_unpackhi_epi16(S4, S5);
                T7 = _mm256_unpackhi_epi16(S6, S7);

                r0 = _mm256_madd_epi16(T0, mCoefy1);
                r1 = _mm256_madd_epi16(T1, mCoefy2);
                r2 = _mm256_madd_epi16(T2, mCoefy3);
                r3 = _mm256_madd_epi16(T3, mCoefy4);
                r4 = _mm256_madd_epi16(T4, mCoefy1);
                r5 = _mm256_madd_epi16(T5, mCoefy2);
                r6 = _mm256_madd_epi16(T6, mCoefy3);
                r7 = _mm256_madd_epi16(T7, mCoefy4);

                mVal1 = _mm256_add_epi32(r0, r1);
                mVal2 = _mm256_add_epi32(r4, r5);
                mVal1 = _mm256_add_epi32(mVal1, r2);
                mVal2 = _mm256_add_epi32(mVal2, r6);
                mVal1 = _mm256_add_epi32(mVal1, r3);
                mVal2 = _mm256_add_epi32(mVal2, r7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

                T8 = _mm256_unpacklo_epi16(S8, S9);
                T9 = _mm256_unpackhi_epi16(S8, S9);

                T0 = _mm256_madd_epi16(T1, mCoefy1);
                T1 = _mm256_madd_epi16(T2, mCoefy2);
                T2 = _mm256_madd_epi16(T3, mCoefy3);
                T3 = _mm256_madd_epi16(T8, mCoefy4);
                T4 = _mm256_madd_epi16(T5, mCoefy1);
                T5 = _mm256_madd_epi16(T6, mCoefy2);
                T6 = _mm256_madd_epi16(T7, mCoefy3);
                T7 = _mm256_madd_epi16(T9, mCoefy4);

                mVal1 = _mm256_add_epi32(T0, T1);
                mVal2 = _mm256_add_epi32(T4, T5);
                mVal1 = _mm256_add_epi32(mVal1, T2);
                mVal2 = _mm256_add_epi32(mVal2, T6);
                mVal1 = _mm256_add_epi32(mVal1, T3);
                mVal2 = _mm256_add_epi32(mVal2, T7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + 2 * i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

                T0 = _mm256_unpacklo_epi16(S1, S2);
                T1 = _mm256_unpacklo_epi16(S3, S4);
                T2 = _mm256_unpacklo_epi16(S5, S6);
                T3 = _mm256_unpacklo_epi16(S7, S8);
                T4 = _mm256_unpackhi_epi16(S1, S2);
                T5 = _mm256_unpackhi_epi16(S3, S4);
                T6 = _mm256_unpackhi_epi16(S5, S6);
                T7 = _mm256_unpackhi_epi16(S7, S8);

                r0 = _mm256_madd_epi16(T0, mCoefy1);
                r1 = _mm256_madd_epi16(T1, mCoefy2);
                r2 = _mm256_madd_epi16(T2, mCoefy3);
                r3 = _mm256_madd_epi16(T3, mCoefy4);
                r4 = _mm256_madd_epi16(T4, mCoefy1);
                r5 = _mm256_madd_epi16(T5, mCoefy2);
                r6 = _mm256_madd_epi16(T6, mCoefy3);
                r7 = _mm256_madd_epi16(T7, mCoefy4);

                mVal1 = _mm256_add_epi32(r0, r1);
                mVal2 = _mm256_add_epi32(r4, r5);
                mVal1 = _mm256_add_epi32(mVal1, r2);
                mVal2 = _mm256_add_epi32(mVal2, r6);
                mVal1 = _mm256_add_epi32(mVal1, r3);
                mVal2 = _mm256_add_epi32(mVal2, r7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));

                T8 = _mm256_unpacklo_epi16(S9, S10);
                T9 = _mm256_unpackhi_epi16(S9, S10);

                T0 = _mm256_madd_epi16(T1, mCoefy1);
                T1 = _mm256_madd_epi16(T2, mCoefy2);
                T2 = _mm256_madd_epi16(T3, mCoefy3);
                T3 = _mm256_madd_epi16(T8, mCoefy4);
                T4 = _mm256_madd_epi16(T5, mCoefy1);
                T5 = _mm256_madd_epi16(T6, mCoefy2);
                T6 = _mm256_madd_epi16(T7, mCoefy3);
                T7 = _mm256_madd_epi16(T9, mCoefy4);

                mVal1 = _mm256_add_epi32(T0, T1);
                mVal2 = _mm256_add_epi32(T4, T5);
                mVal1 = _mm256_add_epi32(mVal1, T2);
                mVal2 = _mm256_add_epi32(mVal2, T6);
                mVal1 = _mm256_add_epi32(mVal1, T3);
                mVal2 = _mm256_add_epi32(mVal2, T7);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                _mm_storeu_si128((__m128i *)(dst + 3 * i_dst + col), _mm_packus_epi16(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1)));
            }
            tmp += 4 * i_tmp;
            dst += 4 * i_dst;
            height -= 4;
        }
    }
}

void uavs3e_if_hor_luma_frame_avx2(const pel *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    s16 *dt0 = dst_tmp[0];
    s16 *dt1 = dst_tmp[1];
    s16 *dt2 = dst_tmp[2];

    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 8);
    __m256i mSwitch2 = _mm256_setr_epi8(2, 3, 4, 5, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9, 10, 2, 3, 4, 5, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9, 10);
    __m256i mSwitch3 = _mm256_setr_epi8(4, 5, 6, 7, 8, 9, 10, 11, 5, 6, 7, 8, 9, 10, 11, 12, 4, 5, 6, 7, 8, 9, 10, 11, 5, 6, 7, 8, 9, 10, 11, 12);
    __m256i mSwitch4 = _mm256_setr_epi8(6, 7, 8, 9, 10, 11, 12, 13, 7, 8, 9, 10, 11, 12, 13, 14, 6, 7, 8, 9, 10, 11, 12, 13, 7, 8, 9, 10, 11, 12, 13, 14);

    __m256i mCoef0 = _mm256_castsi128_si256(_mm_loadl_epi64((__m128i*)coeff[0]));
    __m256i mCoef1 = _mm256_castsi128_si256(_mm_loadl_epi64((__m128i*)coeff[1]));
    __m256i mCoef2 = _mm256_castsi128_si256(_mm_loadl_epi64((__m128i*)coeff[2]));
    __m256i T20, T40, T60, T80, s1, s2, sum, sum_pel, sum_pel1;
    __m256i S20, S40, S60, S80;
    __m256i mAddOffset = _mm256_set1_epi16(32);

    mCoef0 = _mm256_permute4x64_epi64(mCoef0, 0x0);
    mCoef1 = _mm256_permute4x64_epi64(mCoef1, 0x0);
    mCoef2 = _mm256_permute4x64_epi64(mCoef2, 0x0);

    src -= 3;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i += 16) {
            __m256i srcCoeff = _mm256_loadu2_m128i((__m128i*)(src + i + 8), (__m128i*)(src + i));

            S20 = _mm256_shuffle_epi8(srcCoeff, mSwitch1);
            S40 = _mm256_shuffle_epi8(srcCoeff, mSwitch2);
            S60 = _mm256_shuffle_epi8(srcCoeff, mSwitch3);
            S80 = _mm256_shuffle_epi8(srcCoeff, mSwitch4);

            T20 = _mm256_maddubs_epi16(S20, mCoef0);
            T40 = _mm256_maddubs_epi16(S40, mCoef0);
            T60 = _mm256_maddubs_epi16(S60, mCoef0);
            T80 = _mm256_maddubs_epi16(S80, mCoef0);

            s1 = _mm256_hadd_epi16(T20, T40);
            s2 = _mm256_hadd_epi16(T60, T80);
            sum = _mm256_hadd_epi16(s1, s2);
            sum_pel = _mm256_add_epi16(sum, mAddOffset);
            sum_pel = _mm256_srai_epi16(sum_pel, 6);
            _mm256_storeu_si256((__m256i*)&dt0[i], sum);
            _mm_storeu_si128((__m128i*)&d0[i], _mm256_castsi256_si128(sum_pel));

            T20 = _mm256_maddubs_epi16(S20, mCoef1);
            T40 = _mm256_maddubs_epi16(S40, mCoef1);
            T60 = _mm256_maddubs_epi16(S60, mCoef1);
            T80 = _mm256_maddubs_epi16(S80, mCoef1);

            s1 = _mm256_hadd_epi16(T20, T40);
            s2 = _mm256_hadd_epi16(T60, T80);
            sum = _mm256_hadd_epi16(s1, s2);
            sum_pel1 = _mm256_add_epi16(sum, mAddOffset);
            sum_pel1 = _mm256_srai_epi16(sum_pel1, 6);
            _mm256_storeu_si256((__m256i*)&dt1[i], sum);

            sum_pel = _mm256_packus_epi16(sum_pel, sum_pel1);
            sum_pel = _mm256_permute4x64_epi64(sum_pel, 0xD8);
            _mm256_storeu2_m128i((__m128i*)&d1[i], (__m128i*)&d0[i], sum_pel);

            T20 = _mm256_maddubs_epi16(S20, mCoef2);
            T40 = _mm256_maddubs_epi16(S40, mCoef2);
            T60 = _mm256_maddubs_epi16(S60, mCoef2);
            T80 = _mm256_maddubs_epi16(S80, mCoef2);

            s1 = _mm256_hadd_epi16(T20, T40);
            s2 = _mm256_hadd_epi16(T60, T80);
            sum = _mm256_hadd_epi16(s1, s2);
            sum_pel = _mm256_add_epi16(sum, mAddOffset);
            sum_pel = _mm256_srai_epi16(sum_pel, 6);
            sum_pel = _mm256_packus_epi16(sum_pel, sum_pel);
            sum_pel = _mm256_permute4x64_epi64(sum_pel, 0xD8);
            _mm256_storeu_si256((__m256i*)&dt2[i], sum);
            _mm_storeu_si128((__m128i*)&d2[i], _mm256_castsi256_si128(sum_pel));

        }
        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        dt0 += i_dst_tmp;
        dt1 += i_dst_tmp;
        dt2 += i_dst_tmp;
        src += i_src;
    }
}

void uavs3e_if_ver_luma_frame_avx2(const pel *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    __m256i S00, S10, S20, S30;
    __m256i coeff00 = _mm256_set1_epi16(*(s16*)coeff[0]);
    __m256i coeff01 = _mm256_set1_epi16(*(s16*)(coeff[0] + 2));
    __m256i coeff02 = _mm256_set1_epi16(*(s16*)(coeff[0] + 4));
    __m256i coeff03 = _mm256_set1_epi16(*(s16*)(coeff[0] + 6));

    __m256i coeff10 = _mm256_set1_epi16(*(s16*)coeff[1]);
    __m256i coeff11 = _mm256_set1_epi16(*(s16*)(coeff[1] + 2));
    __m256i coeff12 = _mm256_set1_epi16(*(s16*)(coeff[1] + 4));
    __m256i coeff13 = _mm256_set1_epi16(*(s16*)(coeff[1] + 6));

    __m256i coeff20 = _mm256_set1_epi16(*(s16*)coeff[2]);
    __m256i coeff21 = _mm256_set1_epi16(*(s16*)(coeff[2] + 2));
    __m256i coeff22 = _mm256_set1_epi16(*(s16*)(coeff[2] + 4));
    __m256i coeff23 = _mm256_set1_epi16(*(s16*)(coeff[2] + 6));

    __m256i mAddOffset = _mm256_set1_epi16(32);
    __m256i mVal;

    src -= 3 * i_src;

    for (j = 0; j < height; j++) {
        const pel *p = src;
        for (i = 0; i < width; i += 16) {
            __m128i TT00 = _mm_loadu_si128((__m128i*)(p));
            __m128i TT10 = _mm_loadu_si128((__m128i*)(p + i_src));
            __m128i TT20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
            __m128i TT30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));
            __m128i TT40 = _mm_loadu_si128((__m128i*)(p + 4 * i_src));
            __m128i TT50 = _mm_loadu_si128((__m128i*)(p + 5 * i_src));
            __m128i TT60 = _mm_loadu_si128((__m128i*)(p + 6 * i_src));
            __m128i TT70 = _mm_loadu_si128((__m128i*)(p + 7 * i_src));

            __m256i T00 = _mm256_set_m128i(_mm_srli_si128(TT00, 8), TT00);
            __m256i T10 = _mm256_set_m128i(_mm_srli_si128(TT10, 8), TT10);
            __m256i T20 = _mm256_set_m128i(_mm_srli_si128(TT20, 8), TT20);
            __m256i T30 = _mm256_set_m128i(_mm_srli_si128(TT30, 8), TT30);
            __m256i T40 = _mm256_set_m128i(_mm_srli_si128(TT40, 8), TT40);
            __m256i T50 = _mm256_set_m128i(_mm_srli_si128(TT50, 8), TT50);
            __m256i T60 = _mm256_set_m128i(_mm_srli_si128(TT60, 8), TT60);
            __m256i T70 = _mm256_set_m128i(_mm_srli_si128(TT70, 8), TT70);

            S00 = _mm256_unpacklo_epi8(T00, T10);
            S10 = _mm256_unpacklo_epi8(T20, T30);
            S20 = _mm256_unpacklo_epi8(T40, T50);
            S30 = _mm256_unpacklo_epi8(T60, T70);

            T00 = _mm256_maddubs_epi16(S00, coeff00);
            T10 = _mm256_maddubs_epi16(S10, coeff01);
            T20 = _mm256_maddubs_epi16(S20, coeff02);
            T30 = _mm256_maddubs_epi16(S30, coeff03);

            mVal = _mm256_add_epi16(T00, T10);
            mVal = _mm256_add_epi16(mVal, T20);
            mVal = _mm256_add_epi16(mVal, T30);

            mVal = _mm256_add_epi16(mVal, mAddOffset);
            mVal = _mm256_srai_epi16(mVal, 6);
            mVal = _mm256_packus_epi16(mVal, mVal);

            mVal = _mm256_permute4x64_epi64(mVal, 0x8);
            _mm_storeu_si128((__m128i*)&d0[i], _mm256_castsi256_si128(mVal));


            T00 = _mm256_maddubs_epi16(S00, coeff10);
            T10 = _mm256_maddubs_epi16(S10, coeff11);
            T20 = _mm256_maddubs_epi16(S20, coeff12);
            T30 = _mm256_maddubs_epi16(S30, coeff13);

            mVal = _mm256_add_epi16(T00, T10);
            mVal = _mm256_add_epi16(mVal, T20);
            mVal = _mm256_add_epi16(mVal, T30);

            mVal = _mm256_add_epi16(mVal, mAddOffset);
            mVal = _mm256_srai_epi16(mVal, 6);
            mVal = _mm256_packus_epi16(mVal, mVal);

            mVal = _mm256_permute4x64_epi64(mVal, 0x8);
            _mm_storeu_si128((__m128i*)&d1[i], _mm256_castsi256_si128(mVal));

            T00 = _mm256_maddubs_epi16(S00, coeff20);
            T10 = _mm256_maddubs_epi16(S10, coeff21);
            T20 = _mm256_maddubs_epi16(S20, coeff22);
            T30 = _mm256_maddubs_epi16(S30, coeff23);

            mVal = _mm256_add_epi16(T00, T10);
            mVal = _mm256_add_epi16(mVal, T20);
            mVal = _mm256_add_epi16(mVal, T30);

            mVal = _mm256_add_epi16(mVal, mAddOffset);
            mVal = _mm256_srai_epi16(mVal, 6);
            mVal = _mm256_packus_epi16(mVal, mVal);

            mVal = _mm256_permute4x64_epi64(mVal, 0x8);
            _mm_storeu_si128((__m128i*)&d2[i], _mm256_castsi256_si128(mVal));

            p += 16;
        }

        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }

}

void uavs3e_if_ver_luma_frame_ext_avx2(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];

    __m256i mAddOffset = _mm256_set1_epi32(2048);

    __m128i c00 = _mm_set1_epi16(*(s16*)coeff[0]);
    __m128i c01 = _mm_set1_epi16(*(s16*)(coeff[0] + 2));
    __m128i c02 = _mm_set1_epi16(*(s16*)(coeff[0] + 4));
    __m128i c03 = _mm_set1_epi16(*(s16*)(coeff[0] + 6));

    __m128i c10 = _mm_set1_epi16(*(s16*)coeff[1]);
    __m128i c11 = _mm_set1_epi16(*(s16*)(coeff[1] + 2));
    __m128i c12 = _mm_set1_epi16(*(s16*)(coeff[1] + 4));
    __m128i c13 = _mm_set1_epi16(*(s16*)(coeff[1] + 6));

    __m128i c20 = _mm_set1_epi16(*(s16*)coeff[2]);
    __m128i c21 = _mm_set1_epi16(*(s16*)(coeff[2] + 2));
    __m128i c22 = _mm_set1_epi16(*(s16*)(coeff[2] + 4));
    __m128i c23 = _mm_set1_epi16(*(s16*)(coeff[2] + 6));
    __m256i mVal1, mVal2, mVal;

    __m256i coeff00 = _mm256_cvtepi8_epi16(c00);
    __m256i coeff01 = _mm256_cvtepi8_epi16(c01);
    __m256i coeff02 = _mm256_cvtepi8_epi16(c02);
    __m256i coeff03 = _mm256_cvtepi8_epi16(c03);

    __m256i coeff10 = _mm256_cvtepi8_epi16(c10);
    __m256i coeff11 = _mm256_cvtepi8_epi16(c11);
    __m256i coeff12 = _mm256_cvtepi8_epi16(c12);
    __m256i coeff13 = _mm256_cvtepi8_epi16(c13);

    __m256i coeff20 = _mm256_cvtepi8_epi16(c20);
    __m256i coeff21 = _mm256_cvtepi8_epi16(c21);
    __m256i coeff22 = _mm256_cvtepi8_epi16(c22);
    __m256i coeff23 = _mm256_cvtepi8_epi16(c23);

    src -= 3 * i_src;

    for (j = 0; j < height; j++) {
        const s16 *p = src;
        for (i = 0; i < width; i += 16) {
            __m256i S0, S1, S2, S3, S4, S5, S6, S7;
            __m256i T0, T1, T2, T3, T4, T5, T6, T7;
            __m256i T00, T10, T20, T30, T40, T50, T60, T70;

            T00 = _mm256_loadu_si256((__m256i*)(p));
            T10 = _mm256_loadu_si256((__m256i*)(p + i_src));
            S0 = _mm256_unpacklo_epi16(T00, T10);
            S4 = _mm256_unpackhi_epi16(T00, T10);
            T0 = _mm256_madd_epi16(S0, coeff00);
            T4 = _mm256_madd_epi16(S4, coeff00);

            T20 = _mm256_loadu_si256((__m256i*)(p + 2 * i_src));
            T30 = _mm256_loadu_si256((__m256i*)(p + 3 * i_src));
            S1 = _mm256_unpacklo_epi16(T20, T30);
            S5 = _mm256_unpackhi_epi16(T20, T30);
            T1 = _mm256_madd_epi16(S1, coeff01);
            T5 = _mm256_madd_epi16(S5, coeff01);

            T40 = _mm256_loadu_si256((__m256i*)(p + 4 * i_src));
            T50 = _mm256_loadu_si256((__m256i*)(p + 5 * i_src));
            S2 = _mm256_unpacklo_epi16(T40, T50);
            S6 = _mm256_unpackhi_epi16(T40, T50);
            T2 = _mm256_madd_epi16(S2, coeff02);
            T6 = _mm256_madd_epi16(S6, coeff02);

            T60 = _mm256_loadu_si256((__m256i*)(p + 6 * i_src));
            T70 = _mm256_loadu_si256((__m256i*)(p + 7 * i_src));
            S3 = _mm256_unpacklo_epi16(T60, T70);
            S7 = _mm256_unpackhi_epi16(T60, T70);
            T3 = _mm256_madd_epi16(S3, coeff03);
            T7 = _mm256_madd_epi16(S7, coeff03);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal1 = _mm256_add_epi32(mVal1, T2);
            mVal1 = _mm256_add_epi32(mVal1, T3);

            mVal2 = _mm256_add_epi32(T4, T5);
            mVal2 = _mm256_add_epi32(mVal2, T6);
            mVal2 = _mm256_add_epi32(mVal2, T7);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, 12);
            mVal2 = _mm256_srai_epi32(mVal2, 12);
            mVal = _mm256_packs_epi32(mVal1, mVal2);
            mVal = _mm256_packus_epi16(mVal, mVal);

            mVal = _mm256_permute4x64_epi64(mVal, 0x8);
            _mm_storeu_si128((__m128i*)&d0[i], _mm256_castsi256_si128(mVal));

            T0 = _mm256_madd_epi16(S0, coeff10);
            T1 = _mm256_madd_epi16(S1, coeff11);
            T2 = _mm256_madd_epi16(S2, coeff12);
            T3 = _mm256_madd_epi16(S3, coeff13);
            T4 = _mm256_madd_epi16(S4, coeff10);
            T5 = _mm256_madd_epi16(S5, coeff11);
            T6 = _mm256_madd_epi16(S6, coeff12);
            T7 = _mm256_madd_epi16(S7, coeff13);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal1 = _mm256_add_epi32(mVal1, T2);
            mVal1 = _mm256_add_epi32(mVal1, T3);

            mVal2 = _mm256_add_epi32(T4, T5);
            mVal2 = _mm256_add_epi32(mVal2, T6);
            mVal2 = _mm256_add_epi32(mVal2, T7);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, 12);
            mVal2 = _mm256_srai_epi32(mVal2, 12);
            mVal = _mm256_packs_epi32(mVal1, mVal2);
            mVal = _mm256_packus_epi16(mVal, mVal);

            mVal = _mm256_permute4x64_epi64(mVal, 0x8);
            _mm_storeu_si128((__m128i*)&d1[i], _mm256_castsi256_si128(mVal));

            T0 = _mm256_madd_epi16(S0, coeff20);
            T1 = _mm256_madd_epi16(S1, coeff21);
            T2 = _mm256_madd_epi16(S2, coeff22);
            T3 = _mm256_madd_epi16(S3, coeff23);
            T4 = _mm256_madd_epi16(S4, coeff20);
            T5 = _mm256_madd_epi16(S5, coeff21);
            T6 = _mm256_madd_epi16(S6, coeff22);
            T7 = _mm256_madd_epi16(S7, coeff23);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal1 = _mm256_add_epi32(mVal1, T2);
            mVal1 = _mm256_add_epi32(mVal1, T3);

            mVal2 = _mm256_add_epi32(T4, T5);
            mVal2 = _mm256_add_epi32(mVal2, T6);
            mVal2 = _mm256_add_epi32(mVal2, T7);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, 12);
            mVal2 = _mm256_srai_epi32(mVal2, 12);
            mVal = _mm256_packs_epi32(mVal1, mVal2);
            mVal = _mm256_packus_epi16(mVal, mVal);

            mVal = _mm256_permute4x64_epi64(mVal, 0x8);
            _mm_storeu_si128((__m128i*)&d2[i], _mm256_castsi256_si128(mVal));

            p += 16;
        }

        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }
}


#else

void uavs3e_if_cpy_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    int i_src3 = i_src2 + i_src;
    int i_dst3 = i_dst2 + i_dst;
    int i_src4 = i_src << 2;
    int i_dst4 = i_dst << 2;
    __m256i m0, m1, m2, m3;
    while (height > 0) {
        m0 = _mm256_loadu_si256((const __m256i *)(src));
        m1 = _mm256_loadu_si256((const __m256i *)(src + i_src));
        m2 = _mm256_loadu_si256((const __m256i *)(src + i_src2));
        m3 = _mm256_loadu_si256((const __m256i *)(src + i_src3));
        _mm256_storeu_si256((__m256i *)dst, m0);
        _mm256_storeu_si256((__m256i *)(dst + i_dst), m1);
        _mm256_storeu_si256((__m256i *)(dst + i_dst2), m2);
        _mm256_storeu_si256((__m256i *)(dst + i_dst3), m3);
        src += i_src4;
        dst += i_dst4;
        height -= 4;
    }
}

void uavs3e_if_cpy_w32_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    __m256i m0, m1, m2, m3;
    while (height > 0) {
        m0 = _mm256_loadu_si256((const __m256i *)(src));
        m1 = _mm256_loadu_si256((const __m256i *)(src + 16));
        m2 = _mm256_loadu_si256((const __m256i *)(src + i_src));
        m3 = _mm256_loadu_si256((const __m256i *)(src + i_src + 16));
        _mm256_store_si256((__m256i *)dst, m0);
        _mm256_store_si256((__m256i *)(dst + 16), m1);
        _mm256_store_si256((__m256i *)(dst + i_dst), m2);
        _mm256_store_si256((__m256i *)(dst + i_dst + 16), m3);
        src += i_src2;
        dst += i_dst2;
        height -= 2;
    }
}

void uavs3e_if_cpy_w64_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    __m256i m0, m1, m2, m3, m4, m5, m6, m7;
    while (height) {
        m0 = _mm256_loadu_si256((const __m256i *)(src));
        m1 = _mm256_loadu_si256((const __m256i *)(src + 16));
        m2 = _mm256_loadu_si256((const __m256i *)(src + 32));
        m3 = _mm256_loadu_si256((const __m256i *)(src + 48));
        m4 = _mm256_loadu_si256((const __m256i *)(src + i_src));
        m5 = _mm256_loadu_si256((const __m256i *)(src + i_src + 16));
        m6 = _mm256_loadu_si256((const __m256i *)(src + i_src + 32));
        m7 = _mm256_loadu_si256((const __m256i *)(src + i_src + 48));

        _mm256_store_si256((__m256i *)(dst), m0);
        _mm256_store_si256((__m256i *)(dst + 16), m1);
        _mm256_store_si256((__m256i *)(dst + 32), m2);
        _mm256_store_si256((__m256i *)(dst + 48), m3);
        _mm256_store_si256((__m256i *)(dst + i_dst), m4);
        _mm256_store_si256((__m256i *)(dst + i_dst + 16), m5);
        _mm256_store_si256((__m256i *)(dst + i_dst + 32), m6);
        _mm256_store_si256((__m256i *)(dst + i_dst + 48), m7);

        height -= 2;
        src += i_src2;
        dst += i_dst2;
    }
}

void uavs3e_if_cpy_w128_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    __m256i m0, m1, m2, m3, m4, m5, m6, m7;
    while (height) {
        m0 = _mm256_loadu_si256((const __m256i *)(src));
        m1 = _mm256_loadu_si256((const __m256i *)(src + 16));
        m2 = _mm256_loadu_si256((const __m256i *)(src + 32));
        m3 = _mm256_loadu_si256((const __m256i *)(src + 48));
        m4 = _mm256_loadu_si256((const __m256i *)(src + 64));
        m5 = _mm256_loadu_si256((const __m256i *)(src + 80));
        m6 = _mm256_loadu_si256((const __m256i *)(src + 96));
        m7 = _mm256_loadu_si256((const __m256i *)(src + 112));

        _mm256_store_si256((__m256i *)(dst), m0);
        _mm256_store_si256((__m256i *)(dst + 16), m1);
        _mm256_store_si256((__m256i *)(dst + 32), m2);
        _mm256_store_si256((__m256i *)(dst + 48), m3);
        _mm256_store_si256((__m256i *)(dst + 64), m4);
        _mm256_store_si256((__m256i *)(dst + 80), m5);
        _mm256_store_si256((__m256i *)(dst + 96), m6);
        _mm256_store_si256((__m256i *)(dst + 112), m7);

        height--;
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i offset = _mm256_set1_epi32(32);
    s32 *coef = (s32 *)coeff;
    __m128i mCoef0 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);
    __m256i mCoef = _mm256_cvtepi8_epi16(mCoef0);

    src -= 3;

    while (height--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        T0 = _mm256_loadu_si256((__m256i *)(src + 0));
        T1 = _mm256_loadu_si256((__m256i *)(src + 1));
        T2 = _mm256_loadu_si256((__m256i *)(src + 2));
        T3 = _mm256_loadu_si256((__m256i *)(src + 3));
        T4 = _mm256_loadu_si256((__m256i *)(src + 4));
        T5 = _mm256_loadu_si256((__m256i *)(src + 5));
        T6 = _mm256_loadu_si256((__m256i *)(src + 6));
        T7 = _mm256_loadu_si256((__m256i *)(src + 7));

        M0 = _mm256_madd_epi16(T0, mCoef);
        M1 = _mm256_madd_epi16(T1, mCoef);
        M2 = _mm256_madd_epi16(T2, mCoef);
        M3 = _mm256_madd_epi16(T3, mCoef);
        M4 = _mm256_madd_epi16(T4, mCoef);
        M5 = _mm256_madd_epi16(T5, mCoef);
        M6 = _mm256_madd_epi16(T6, mCoef);
        M7 = _mm256_madd_epi16(T7, mCoef);

        M0 = _mm256_hadd_epi32(M0, M1);
        M1 = _mm256_hadd_epi32(M2, M3);
        M2 = _mm256_hadd_epi32(M4, M5);
        M3 = _mm256_hadd_epi32(M6, M7);

        M0 = _mm256_hadd_epi32(M0, M1);
        M1 = _mm256_hadd_epi32(M2, M3);

        M2 = _mm256_add_epi32(M0, offset);
        M3 = _mm256_add_epi32(M1, offset);
        M2 = _mm256_srai_epi32(M2, 6);
        M3 = _mm256_srai_epi32(M3, 6);
        M2 = _mm256_packus_epi32(M2, M3);
        M2 = _mm256_min_epu16(M2, max_pel);

        _mm256_storeu_si256((__m256i *)(dst), M2);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_luma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int col;
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i offset = _mm256_set1_epi32(32);
    s32 *coef = (s32 *)coeff;
    __m128i mCoef0 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);
    __m256i mCoef = _mm256_cvtepi8_epi16(mCoef0);

    src -= 3;

    while (height--) {
        const pel *p_src = src;
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 16) {
            T0 = _mm256_loadu_si256((__m256i *)(p_src + 0));
            T1 = _mm256_loadu_si256((__m256i *)(p_src + 1));
            T2 = _mm256_loadu_si256((__m256i *)(p_src + 2));
            T3 = _mm256_loadu_si256((__m256i *)(p_src + 3));
            T4 = _mm256_loadu_si256((__m256i *)(p_src + 4));
            T5 = _mm256_loadu_si256((__m256i *)(p_src + 5));
            T6 = _mm256_loadu_si256((__m256i *)(p_src + 6));
            T7 = _mm256_loadu_si256((__m256i *)(p_src + 7));

            M0 = _mm256_madd_epi16(T0, mCoef);
            M1 = _mm256_madd_epi16(T1, mCoef);
            M2 = _mm256_madd_epi16(T2, mCoef);
            M3 = _mm256_madd_epi16(T3, mCoef);
            M4 = _mm256_madd_epi16(T4, mCoef);
            M5 = _mm256_madd_epi16(T5, mCoef);
            M6 = _mm256_madd_epi16(T6, mCoef);
            M7 = _mm256_madd_epi16(T7, mCoef);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);
            M2 = _mm256_hadd_epi32(M4, M5);
            M3 = _mm256_hadd_epi32(M6, M7);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);

            M2 = _mm256_add_epi32(M0, offset);
            M3 = _mm256_add_epi32(M1, offset);
            M2 = _mm256_srai_epi32(M2, 6);
            M3 = _mm256_srai_epi32(M3, 6);
            M2 = _mm256_packus_epi32(M2, M3);
            M2 = _mm256_min_epu16(M2, max_pel);

            p_src += 16;
            _mm256_storeu_si256((__m256i *)(dst + col), M2);
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_chroma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coeff));
    __m256i mCoefy2_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coeff + 2)));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    __m256i T0, T1, S0, R0, R1, sum;
    __m128i s0, s1;
    __m128i max_pel = _mm_set1_epi16(max_val);
    src -= 1;

    while (height > 0) {
        s0 = _mm_loadu_si128((__m128i *)(src));
        s1 = _mm_loadu_si128((__m128i *)(src + i_src));

        S0 = _mm256_set_m128i(s1, s0);

        R0 = _mm256_shuffle_epi8(S0, mSwitch1);  
        R1 = _mm256_shuffle_epi8(S0, mSwitch2);

        T0 = _mm256_madd_epi16(R0, mCoefy1_hor);
        T1 = _mm256_madd_epi16(R1, mCoefy2_hor);
        sum = _mm256_add_epi32(T0, T1);

        sum = _mm256_add_epi32(sum, mAddOffset);
        sum = _mm256_srai_epi32(sum, shift);

        s0 = _mm_packus_epi32(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
        s0 = _mm_min_epi16(s0, max_pel);
        s1 = _mm_srli_si128(s0, 8);

        _mm_storel_epi64((__m128i *)(dst), s0);
        _mm_storel_epi64((__m128i *)(dst + i_dst), s1);

        height -= 2;
        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_hor_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coeff));
    __m256i mCoefy2_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coeff + 2)));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, R0, R1, R2, R3, sum0, sum1;
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);

    src -= 1;

    while (height > 0) {
        uavs3e_prefetch(src + i_src * 2, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);
        R0 = _mm256_shuffle_epi8(S2, mSwitch1);
        R1 = _mm256_shuffle_epi8(S2, mSwitch2);
        R2 = _mm256_shuffle_epi8(S3, mSwitch1);
        R3 = _mm256_shuffle_epi8(S3, mSwitch2);
        T0 = _mm256_madd_epi16(R0, mCoefy1_hor);
        T1 = _mm256_madd_epi16(R1, mCoefy2_hor);
        T2 = _mm256_madd_epi16(R2, mCoefy1_hor);
        T3 = _mm256_madd_epi16(R3, mCoefy2_hor);
        sum0 = _mm256_add_epi32(T0, T1);
        sum1 = _mm256_add_epi32(T2, T3);

        sum0 = _mm256_add_epi32(sum0, mAddOffset);
        sum1 = _mm256_add_epi32(sum1, mAddOffset);
        sum0 = _mm256_srai_epi32(sum0, shift);
        sum1 = _mm256_srai_epi32(sum1, shift);

        sum0 = _mm256_packus_epi32(sum0, sum1);
        sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
        sum0 = _mm256_min_epi16(sum0, max_pel);

        _mm_storeu_si128((__m128i *)(dst), _mm256_castsi256_si128(sum0));
        _mm_storeu_si128((__m128i *)(dst + i_dst), _mm256_extracti128_si256(sum0, 1));

        height -= 2;
        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_hor_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coeff));
    __m256i mCoefy2_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coeff + 2)));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, R0, R1, R2, R3, sum0, sum1;
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);

    src -= 1;

    while (height > 0) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);

        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + 8));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);
        R0 = _mm256_shuffle_epi8(S2, mSwitch1);
        R1 = _mm256_shuffle_epi8(S2, mSwitch2);
        R2 = _mm256_shuffle_epi8(S3, mSwitch1);
        R3 = _mm256_shuffle_epi8(S3, mSwitch2);
        T0 = _mm256_madd_epi16(R0, mCoefy1_hor);
        T1 = _mm256_madd_epi16(R1, mCoefy2_hor);
        T2 = _mm256_madd_epi16(R2, mCoefy1_hor);
        T3 = _mm256_madd_epi16(R3, mCoefy2_hor);
        sum0 = _mm256_add_epi32(T0, T1);
        sum1 = _mm256_add_epi32(T2, T3);

        sum0 = _mm256_add_epi32(sum0, mAddOffset);
        sum1 = _mm256_add_epi32(sum1, mAddOffset);
        sum0 = _mm256_srai_epi32(sum0, shift);
        sum1 = _mm256_srai_epi32(sum1, shift);

        sum0 = _mm256_packus_epi32(sum0, sum1);
        sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
        sum0 = _mm256_min_epi16(sum0, max_pel);

        _mm256_storeu_si256((__m256i *)(dst), sum0);

        height--;
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_chroma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int col;
    const int offset = 32;
    const int shift = 6;

    __m256i mCoefy1_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coeff));
    __m256i mCoefy2_hor = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coeff + 2)));
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, R0, R1, R2, R3, sum0, sum1;
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);

    src -= 1;

    while (height--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 16) {
            S0 = _mm256_loadu_si256((__m256i *)(src + col));
            S1 = _mm256_loadu_si256((__m256i *)(src + col + 8));
            S2 = _mm256_permute4x64_epi64(S0, 0x94);
            S3 = _mm256_permute4x64_epi64(S1, 0x94);
            R0 = _mm256_shuffle_epi8(S2, mSwitch1);
            R1 = _mm256_shuffle_epi8(S2, mSwitch2);
            R2 = _mm256_shuffle_epi8(S3, mSwitch1);
            R3 = _mm256_shuffle_epi8(S3, mSwitch2);
            T0 = _mm256_madd_epi16(R0, mCoefy1_hor);
            T1 = _mm256_madd_epi16(R1, mCoefy2_hor);
            T2 = _mm256_madd_epi16(R2, mCoefy1_hor);
            T3 = _mm256_madd_epi16(R3, mCoefy2_hor);
            sum0 = _mm256_add_epi32(T0, T1);
            sum1 = _mm256_add_epi32(T2, T3);

            sum0 = _mm256_add_epi32(sum0, mAddOffset);
            sum1 = _mm256_add_epi32(sum1, mAddOffset);
            sum0 = _mm256_srai_epi32(sum0, shift);
            sum1 = _mm256_srai_epi32(sum1, shift);

            sum0 = _mm256_packus_epi32(sum0, sum1);
            sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
            sum0 = _mm256_min_epi16(sum0, max_pel);

            _mm256_storeu_si256((__m256i *)(dst + col), sum0);
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    const int i_src5 = i_src * 5;
    const int i_src6 = i_src * 6;
    const int i_src7 = i_src * 7;
    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16 *)(coeff + 6));
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i mAddOffset = _mm256_set1_epi32(32);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i N0, N1, N2, N3, N4, N5, N6, N7;
    __m256i coeff00 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff01 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff02 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff03 = _mm256_cvtepi8_epi16(coeff3);

    src -= 3 * i_src;

    while (height--) {
        uavs3e_prefetch(src + 8 * i_src, _MM_HINT_NTA);
        T0 = _mm256_loadu_si256((__m256i *)(src));
        T1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        T2 = _mm256_loadu_si256((__m256i *)(src + i_src2));
        T3 = _mm256_loadu_si256((__m256i *)(src + i_src3));
        T4 = _mm256_loadu_si256((__m256i *)(src + i_src4));
        T5 = _mm256_loadu_si256((__m256i *)(src + i_src5));
        T6 = _mm256_loadu_si256((__m256i *)(src + i_src6));
        T7 = _mm256_loadu_si256((__m256i *)(src + i_src7));

        M0 = _mm256_unpacklo_epi16(T0, T1);
        M1 = _mm256_unpacklo_epi16(T2, T3);
        M2 = _mm256_unpacklo_epi16(T4, T5);
        M3 = _mm256_unpacklo_epi16(T6, T7);
        M4 = _mm256_unpackhi_epi16(T0, T1);
        M5 = _mm256_unpackhi_epi16(T2, T3);
        M6 = _mm256_unpackhi_epi16(T4, T5);
        M7 = _mm256_unpackhi_epi16(T6, T7);

        N0 = _mm256_madd_epi16(M0, coeff00);
        N1 = _mm256_madd_epi16(M1, coeff01);
        N2 = _mm256_madd_epi16(M2, coeff02);
        N3 = _mm256_madd_epi16(M3, coeff03);
        N4 = _mm256_madd_epi16(M4, coeff00);
        N5 = _mm256_madd_epi16(M5, coeff01);
        N6 = _mm256_madd_epi16(M6, coeff02);
        N7 = _mm256_madd_epi16(M7, coeff03);

        N0 = _mm256_add_epi32(N0, N1);
        N1 = _mm256_add_epi32(N2, N3);
        N2 = _mm256_add_epi32(N4, N5);
        N3 = _mm256_add_epi32(N6, N7);

        N0 = _mm256_add_epi32(N0, N1);
        N1 = _mm256_add_epi32(N2, N3);

        N0 = _mm256_add_epi32(N0, mAddOffset);
        N1 = _mm256_add_epi32(N1, mAddOffset);
        N0 = _mm256_srai_epi32(N0, 6);
        N1 = _mm256_srai_epi32(N1, 6);
        N0 = _mm256_packus_epi32(N0, N1);
        N0 = _mm256_min_epu16(N0, max_pel);
        _mm256_storeu_si256((__m256i *)(dst), N0);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int i;
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    const int i_src5 = i_src * 5;
    const int i_src6 = i_src * 6;
    const int i_src7 = i_src * 7;
    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16 *)(coeff + 6));
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i mAddOffset = _mm256_set1_epi32(32);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i N0, N1, N2, N3, N4, N5, N6, N7;
    __m256i coeff00 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff01 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff02 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff03 = _mm256_cvtepi8_epi16(coeff3);

    src -= 3 * i_src;

    while (height--) {
        const pel *p_src = src;
        uavs3e_prefetch(src + 8 * i_src, _MM_HINT_NTA);
        for (i = 0; i < width; i += 16) {
            T0 = _mm256_loadu_si256((__m256i *)(p_src));
            T1 = _mm256_loadu_si256((__m256i *)(p_src + i_src));
            T2 = _mm256_loadu_si256((__m256i *)(p_src + i_src2));
            T3 = _mm256_loadu_si256((__m256i *)(p_src + i_src3));
            T4 = _mm256_loadu_si256((__m256i *)(p_src + i_src4));
            T5 = _mm256_loadu_si256((__m256i *)(p_src + i_src5));
            T6 = _mm256_loadu_si256((__m256i *)(p_src + i_src6));
            T7 = _mm256_loadu_si256((__m256i *)(p_src + i_src7));

            M0 = _mm256_unpacklo_epi16(T0, T1);
            M1 = _mm256_unpacklo_epi16(T2, T3);
            M2 = _mm256_unpacklo_epi16(T4, T5);
            M3 = _mm256_unpacklo_epi16(T6, T7);
            M4 = _mm256_unpackhi_epi16(T0, T1);
            M5 = _mm256_unpackhi_epi16(T2, T3);
            M6 = _mm256_unpackhi_epi16(T4, T5);
            M7 = _mm256_unpackhi_epi16(T6, T7);

            N0 = _mm256_madd_epi16(M0, coeff00);
            N1 = _mm256_madd_epi16(M1, coeff01);
            N2 = _mm256_madd_epi16(M2, coeff02);
            N3 = _mm256_madd_epi16(M3, coeff03);
            N4 = _mm256_madd_epi16(M4, coeff00);
            N5 = _mm256_madd_epi16(M5, coeff01);
            N6 = _mm256_madd_epi16(M6, coeff02);
            N7 = _mm256_madd_epi16(M7, coeff03);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);
            N2 = _mm256_add_epi32(N4, N5);
            N3 = _mm256_add_epi32(N6, N7);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, mAddOffset);
            N1 = _mm256_add_epi32(N1, mAddOffset);
            N0 = _mm256_srai_epi32(N0, 6);
            N1 = _mm256_srai_epi32(N1, 6);
            N0 = _mm256_packus_epi32(N0, N1);
            p_src += 16;
            N0 = _mm256_min_epu16(N0, max_pel);
            _mm256_storeu_si256((__m256i *)(dst + i), N0);
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_chroma_w4_avx2(const pel* src, int i_src, pel* dst, int i_dst, int width, int height, const s8* coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    __m256i coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)coeff));
    __m256i coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)(coeff + 2)));
    __m128i max_pel = _mm_set1_epi16((pel)max_val);
    __m256i T0, T1, mVal;
    __m256i S0, S1, S2, S3;
    __m128i s0, s1, s2, s3, s4;

    src -= i_src;

    while (height) {
        uavs3e_prefetch(src + 5 * i_src, _MM_HINT_NTA);
        uavs3e_prefetch(src + 6 * i_src, _MM_HINT_NTA);
        height -= 2;
        s0 = _mm_loadl_epi64((__m128i*)(src));
        s1 = _mm_loadl_epi64((__m128i*)(src + i_src));
        s2 = _mm_loadl_epi64((__m128i*)(src + i_src2));
        s3 = _mm_loadl_epi64((__m128i*)(src + i_src3));
        s4 = _mm_loadl_epi64((__m128i*)(src + i_src4));

        S0 = _mm256_set_m128i(s1, s0);
        S1 = _mm256_set_m128i(s2, s1);
        S2 = _mm256_set_m128i(s3, s2);
        S3 = _mm256_set_m128i(s4, s3);

        S0 = _mm256_unpacklo_epi16(S0, S1);
        S2 = _mm256_unpacklo_epi16(S2, S3);

        T0 = _mm256_madd_epi16(S0, coeff0);
        T1 = _mm256_madd_epi16(S2, coeff1);

        mVal = _mm256_add_epi32(T0, T1);

        mVal = _mm256_add_epi32(mVal, mAddOffset);
        mVal = _mm256_srai_epi32(mVal, shift);
        s0 = _mm_packus_epi32(_mm256_castsi256_si128(mVal), _mm256_extracti128_si256(mVal, 1));
        s0 = _mm_min_epi16(s0, max_pel);
        s1 = _mm_srli_si128(s0, 8);

        _mm_storel_epi64((__m128i*)(dst), s0);
        _mm_storel_epi64((__m128i*)(dst + i_dst), s1);

        src += 2 * i_src;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_ver_chroma_w8_avx2(const pel* src, int i_src, pel* dst, int i_dst, int width, int height, const s8* coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    __m256i coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)coeff));
    __m256i coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)(coeff + 2)));
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i T0, T1, T2, T3, mVal0, mVal1;
    __m128i s0, s1, s2, s3, s4;
    __m256i S0, S1, S2, S3;

    src -= i_src;

    while (height) {
        uavs3e_prefetch(src + 5 * i_src, _MM_HINT_NTA);
        uavs3e_prefetch(src + 6 * i_src, _MM_HINT_NTA);
        height -= 2;
        s0 = _mm_loadu_si128((__m128i*)(src));
        s1 = _mm_loadu_si128((__m128i*)(src + i_src));
        s2 = _mm_loadu_si128((__m128i*)(src + i_src2));
        s3 = _mm_loadu_si128((__m128i*)(src + i_src3));
        s4 = _mm_loadu_si128((__m128i*)(src + i_src4));

        S0 = _mm256_set_m128i(s1, s0);
        S1 = _mm256_set_m128i(s2, s1);
        S2 = _mm256_set_m128i(s3, s2);
        S3 = _mm256_set_m128i(s4, s3);

        T0 = _mm256_unpacklo_epi16(S0, S1);
        T1 = _mm256_unpackhi_epi16(S0, S1);
        T2 = _mm256_unpacklo_epi16(S2, S3);
        T3 = _mm256_unpackhi_epi16(S2, S3);

        T0 = _mm256_madd_epi16(T0, coeff0);
        T1 = _mm256_madd_epi16(T1, coeff0);
        T2 = _mm256_madd_epi16(T2, coeff1);
        T3 = _mm256_madd_epi16(T3, coeff1);

        mVal0 = _mm256_add_epi32(T0, T2);
        mVal1 = _mm256_add_epi32(T1, T3);

        mVal0 = _mm256_add_epi32(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
        mVal0 = _mm256_srai_epi32(mVal0, shift);
        mVal1 = _mm256_srai_epi32(mVal1, shift);
        mVal0 = _mm256_packus_epi32(mVal0, mVal1);
        mVal0 = _mm256_min_epi16(mVal0, max_pel);

        _mm_storeu_si128((__m128i*)dst, _mm256_castsi256_si128(mVal0));
        _mm_storeu_si128((__m128i*)(dst + i_dst), _mm256_extracti128_si256(mVal0, 1));

        src += 2 * i_src;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_ver_chroma_w16_avx2(const pel* src, int i_src, pel* dst, int i_dst, int width, int height, const s8* coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    const int i_src4 = i_src * 4;
    __m256i coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)coeff));
    __m256i coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)(coeff + 2)));
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal0, mVal1, mVal2, mVal3;
    __m256i S0, S1, S2, S3, S4;

    src -= i_src;

    while (height) {
        uavs3e_prefetch(src + 5 * i_src, _MM_HINT_NTA);
        uavs3e_prefetch(src + 6 * i_src, _MM_HINT_NTA);
        height -= 2;
        S0 = _mm256_loadu_si256((__m256i*)(src));
        S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
        S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
        S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));
        S4 = _mm256_loadu_si256((__m256i*)(src + i_src4));

        T0 = _mm256_unpacklo_epi16(S0, S1);
        T1 = _mm256_unpackhi_epi16(S0, S1);
        T2 = _mm256_unpacklo_epi16(S2, S3);
        T3 = _mm256_unpackhi_epi16(S2, S3);
        T4 = _mm256_unpacklo_epi16(S1, S2);
        T5 = _mm256_unpackhi_epi16(S1, S2);
        T6 = _mm256_unpacklo_epi16(S3, S4);
        T7 = _mm256_unpackhi_epi16(S3, S4);

        T0 = _mm256_madd_epi16(T0, coeff0);
        T1 = _mm256_madd_epi16(T1, coeff0);
        T2 = _mm256_madd_epi16(T2, coeff1);
        T3 = _mm256_madd_epi16(T3, coeff1);
        T4 = _mm256_madd_epi16(T4, coeff0);
        T5 = _mm256_madd_epi16(T5, coeff0);
        T6 = _mm256_madd_epi16(T6, coeff1);
        T7 = _mm256_madd_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi32(T0, T2);
        mVal1 = _mm256_add_epi32(T1, T3);
        mVal2 = _mm256_add_epi32(T4, T6);
        mVal3 = _mm256_add_epi32(T5, T7);

        mVal0 = _mm256_add_epi32(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi32(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi32(mVal0, shift);
        mVal1 = _mm256_srai_epi32(mVal1, shift);
        mVal2 = _mm256_srai_epi32(mVal2, shift);
        mVal3 = _mm256_srai_epi32(mVal3, shift);
        mVal0 = _mm256_packus_epi32(mVal0, mVal1);
        mVal2 = _mm256_packus_epi32(mVal2, mVal3);
        mVal0 = _mm256_min_epi16(mVal0, max_pel);
        mVal2 = _mm256_min_epi16(mVal2, max_pel);

        _mm256_storeu_si256((__m256i*)dst, mVal0);
        _mm256_storeu_si256((__m256i*)(dst + i_dst), mVal2);

        src += 2 * i_src;
        dst += 2 * i_dst;

    }
}

void uavs3e_if_ver_chroma_w32_avx2(const pel* src, int i_src, pel* dst, int i_dst, int width, int height, const s8* coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    __m256i coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)coeff));
    __m256i coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)(coeff + 2)));
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal0, mVal1, mVal2, mVal3;

    src -= i_src;

    while (height--) {
        uavs3e_prefetch(src + 4 * i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i*)(src));
        S4 = _mm256_loadu_si256((__m256i*)(src + 16));
        S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
        S5 = _mm256_loadu_si256((__m256i*)(src + i_src + 16));
        S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
        S6 = _mm256_loadu_si256((__m256i*)(src + i_src2 + 16));
        S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));
        S7 = _mm256_loadu_si256((__m256i*)(src + i_src3 + 16));

        T0 = _mm256_unpacklo_epi16(S0, S1);
        T1 = _mm256_unpacklo_epi16(S2, S3);
        T2 = _mm256_unpackhi_epi16(S0, S1);
        T3 = _mm256_unpackhi_epi16(S2, S3);
        T4 = _mm256_unpacklo_epi16(S4, S5);
        T5 = _mm256_unpacklo_epi16(S6, S7);
        T6 = _mm256_unpackhi_epi16(S4, S5);
        T7 = _mm256_unpackhi_epi16(S6, S7);

        T0 = _mm256_madd_epi16(T0, coeff0);
        T1 = _mm256_madd_epi16(T1, coeff1);
        T2 = _mm256_madd_epi16(T2, coeff0);
        T3 = _mm256_madd_epi16(T3, coeff1);
        T4 = _mm256_madd_epi16(T4, coeff0);
        T5 = _mm256_madd_epi16(T5, coeff1);
        T6 = _mm256_madd_epi16(T6, coeff0);
        T7 = _mm256_madd_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi32(T0, T1);
        mVal1 = _mm256_add_epi32(T2, T3);
        mVal2 = _mm256_add_epi32(T4, T5);
        mVal3 = _mm256_add_epi32(T6, T7);

        mVal0 = _mm256_add_epi32(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi32(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi32(mVal0, shift);
        mVal1 = _mm256_srai_epi32(mVal1, shift);
        mVal2 = _mm256_srai_epi32(mVal2, shift);
        mVal3 = _mm256_srai_epi32(mVal3, shift);
        mVal0 = _mm256_packus_epi32(mVal0, mVal1);
        mVal1 = _mm256_packus_epi32(mVal2, mVal3);

        mVal0 = _mm256_min_epi16(mVal0, max_pel);
        mVal1 = _mm256_min_epi16(mVal1, max_pel);
        _mm256_store_si256((__m256i*)(dst), mVal0);
        _mm256_store_si256((__m256i*)(dst + 16), mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_chroma_w64_avx2(const pel* src, int i_src, pel* dst, int i_dst, int width, int height, const s8* coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;
    __m256i coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)coeff));
    __m256i coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16*)(coeff + 2)));
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);
    __m256i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7, mVal0, mVal1, mVal2, mVal3;

    src -= i_src;

    while (height--) {
        uavs3e_prefetch(src + 4 * i_src, _MM_HINT_NTA);
        S0 = _mm256_loadu_si256((__m256i*)(src));
        S4 = _mm256_loadu_si256((__m256i*)(src + 16));
        S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
        S5 = _mm256_loadu_si256((__m256i*)(src + i_src + 16));
        S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
        S6 = _mm256_loadu_si256((__m256i*)(src + i_src2 + 16));
        S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));
        S7 = _mm256_loadu_si256((__m256i*)(src + i_src3 + 16));

        T0 = _mm256_unpacklo_epi16(S0, S1);
        T1 = _mm256_unpacklo_epi16(S2, S3);
        T2 = _mm256_unpackhi_epi16(S0, S1);
        T3 = _mm256_unpackhi_epi16(S2, S3);
        T4 = _mm256_unpacklo_epi16(S4, S5);
        T5 = _mm256_unpacklo_epi16(S6, S7);
        T6 = _mm256_unpackhi_epi16(S4, S5);
        T7 = _mm256_unpackhi_epi16(S6, S7);

        T0 = _mm256_madd_epi16(T0, coeff0);
        T1 = _mm256_madd_epi16(T1, coeff1);
        T2 = _mm256_madd_epi16(T2, coeff0);
        T3 = _mm256_madd_epi16(T3, coeff1);
        T4 = _mm256_madd_epi16(T4, coeff0);
        T5 = _mm256_madd_epi16(T5, coeff1);
        T6 = _mm256_madd_epi16(T6, coeff0);
        T7 = _mm256_madd_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi32(T0, T1);
        mVal1 = _mm256_add_epi32(T2, T3);
        mVal2 = _mm256_add_epi32(T4, T5);
        mVal3 = _mm256_add_epi32(T6, T7);

        mVal0 = _mm256_add_epi32(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi32(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi32(mVal0, shift);
        mVal1 = _mm256_srai_epi32(mVal1, shift);
        mVal2 = _mm256_srai_epi32(mVal2, shift);
        mVal3 = _mm256_srai_epi32(mVal3, shift);
        mVal0 = _mm256_packus_epi32(mVal0, mVal1);
        mVal1 = _mm256_packus_epi32(mVal2, mVal3);

        mVal0 = _mm256_min_epi16(mVal0, max_pel);
        mVal1 = _mm256_min_epi16(mVal1, max_pel);
        _mm256_store_si256((__m256i*)(dst), mVal0);
        _mm256_store_si256((__m256i*)(dst + 16), mVal1);

        S0 = _mm256_loadu_si256((__m256i*)(src + 32));
        S4 = _mm256_loadu_si256((__m256i*)(src + 48));
        S1 = _mm256_loadu_si256((__m256i*)(src + i_src + 32));
        S5 = _mm256_loadu_si256((__m256i*)(src + i_src + 48));
        S2 = _mm256_loadu_si256((__m256i*)(src + i_src2 + 32));
        S6 = _mm256_loadu_si256((__m256i*)(src + i_src2 + 48));
        S3 = _mm256_loadu_si256((__m256i*)(src + i_src3 + 32));
        S7 = _mm256_loadu_si256((__m256i*)(src + i_src3 + 48));

        T0 = _mm256_unpacklo_epi16(S0, S1);
        T1 = _mm256_unpacklo_epi16(S2, S3);
        T2 = _mm256_unpackhi_epi16(S0, S1);
        T3 = _mm256_unpackhi_epi16(S2, S3);
        T4 = _mm256_unpacklo_epi16(S4, S5);
        T5 = _mm256_unpacklo_epi16(S6, S7);
        T6 = _mm256_unpackhi_epi16(S4, S5);
        T7 = _mm256_unpackhi_epi16(S6, S7);

        T0 = _mm256_madd_epi16(T0, coeff0);
        T1 = _mm256_madd_epi16(T1, coeff1);
        T2 = _mm256_madd_epi16(T2, coeff0);
        T3 = _mm256_madd_epi16(T3, coeff1);
        T4 = _mm256_madd_epi16(T4, coeff0);
        T5 = _mm256_madd_epi16(T5, coeff1);
        T6 = _mm256_madd_epi16(T6, coeff0);
        T7 = _mm256_madd_epi16(T7, coeff1);

        mVal0 = _mm256_add_epi32(T0, T1);
        mVal1 = _mm256_add_epi32(T2, T3);
        mVal2 = _mm256_add_epi32(T4, T5);
        mVal3 = _mm256_add_epi32(T6, T7);

        mVal0 = _mm256_add_epi32(mVal0, mAddOffset);
        mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
        mVal3 = _mm256_add_epi32(mVal3, mAddOffset);
        mVal0 = _mm256_srai_epi32(mVal0, shift);
        mVal1 = _mm256_srai_epi32(mVal1, shift);
        mVal2 = _mm256_srai_epi32(mVal2, shift);
        mVal3 = _mm256_srai_epi32(mVal3, shift);
        mVal0 = _mm256_packus_epi32(mVal0, mVal1);
        mVal1 = _mm256_packus_epi32(mVal2, mVal3);

        mVal0 = _mm256_min_epi16(mVal0, max_pel);
        mVal1 = _mm256_min_epi16(mVal1, max_pel);
        _mm256_store_si256((__m256i*)(dst + 32), mVal0);
        _mm256_store_si256((__m256i*)(dst + 48), mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_ver_luma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(128 + 7) * 128]);
    s16 *tmp = tmp_res;
    int row, i;;
    int add1, shift1;
    int add2, shift2;
    __m128i mCoef0;
    __m256i mCoef, offset;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i N0, N1, N2, N3, N4, N5, N6, N7;
    int i_tmp = width;
    s32 *coef;
    __m128i coeff0, coeff1, coeff2, coeff3;
    __m256i coeff00, coeff01, coeff02, coeff03;
    __m256i max_pel = _mm256_set1_epi16((pel)max_val);

    if (max_val == 255) { // 8 bit_depth
        shift1 = 0;
        shift2 = 12;
    } else { // 10 bit_depth
        shift1 = 2;
        shift2 = 10;
    }

    add1 = (1 << (shift1)) >> 1;
    add2 = 1 << (shift2 - 1);

    src += -3 * i_src - 3;
    coef = (s32 *)coef_x;
    mCoef0 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);
    mCoef = _mm256_cvtepi8_epi16(mCoef0);
    offset = _mm256_set1_epi32(add1);

    row = height + 7;

    while (row--) {
        const pel *p = src;
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (i = 0; i < width; i += 16) {

            T0 = _mm256_loadu_si256((__m256i *)p++);
            T1 = _mm256_loadu_si256((__m256i *)p++);
            T2 = _mm256_loadu_si256((__m256i *)p++);
            T3 = _mm256_loadu_si256((__m256i *)p++);
            T4 = _mm256_loadu_si256((__m256i *)p++);
            T5 = _mm256_loadu_si256((__m256i *)p++);
            T6 = _mm256_loadu_si256((__m256i *)p++);
            T7 = _mm256_loadu_si256((__m256i *)p++);

            M0 = _mm256_madd_epi16(T0, mCoef);
            M1 = _mm256_madd_epi16(T1, mCoef);
            M2 = _mm256_madd_epi16(T2, mCoef);
            M3 = _mm256_madd_epi16(T3, mCoef);
            M4 = _mm256_madd_epi16(T4, mCoef);
            M5 = _mm256_madd_epi16(T5, mCoef);
            M6 = _mm256_madd_epi16(T6, mCoef);
            M7 = _mm256_madd_epi16(T7, mCoef);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);
            M2 = _mm256_hadd_epi32(M4, M5);
            M3 = _mm256_hadd_epi32(M6, M7);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);

            M2 = _mm256_add_epi32(M0, offset);
            M3 = _mm256_add_epi32(M1, offset);
            M2 = _mm256_srai_epi32(M2, shift1);
            M3 = _mm256_srai_epi32(M3, shift1);
            M2 = _mm256_packs_epi32(M2, M3);
            _mm256_store_si256((__m256i *)(tmp + i), M2);

            p += 8;
        }
        tmp += i_tmp;
        src += i_src;
    }

    offset = _mm256_set1_epi32(add2);
    tmp = tmp_res;

    coeff0 = _mm_set1_epi16(*(s16 *)(coef_y));
    coeff1 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    coeff2 = _mm_set1_epi16(*(s16 *)(coef_y + 4));
    coeff3 = _mm_set1_epi16(*(s16 *)(coef_y + 6));
    coeff00 = _mm256_cvtepi8_epi16(coeff0);
    coeff01 = _mm256_cvtepi8_epi16(coeff1);
    coeff02 = _mm256_cvtepi8_epi16(coeff2);
    coeff03 = _mm256_cvtepi8_epi16(coeff3);

    while (height--) {
        const pel *p = (pel *)tmp;
        for (i = 0; i < width; i += 16) {
            T0 = _mm256_load_si256((__m256i *)(p));
            T1 = _mm256_load_si256((__m256i *)(p + i_tmp));
            T2 = _mm256_load_si256((__m256i *)(p + 2 * i_tmp));
            T3 = _mm256_load_si256((__m256i *)(p + 3 * i_tmp));
            T4 = _mm256_load_si256((__m256i *)(p + 4 * i_tmp));
            T5 = _mm256_load_si256((__m256i *)(p + 5 * i_tmp));
            T6 = _mm256_load_si256((__m256i *)(p + 6 * i_tmp));
            T7 = _mm256_load_si256((__m256i *)(p + 7 * i_tmp));

            M0 = _mm256_unpacklo_epi16(T0, T1);
            M1 = _mm256_unpacklo_epi16(T2, T3);
            M2 = _mm256_unpacklo_epi16(T4, T5);
            M3 = _mm256_unpacklo_epi16(T6, T7);
            M4 = _mm256_unpackhi_epi16(T0, T1);
            M5 = _mm256_unpackhi_epi16(T2, T3);
            M6 = _mm256_unpackhi_epi16(T4, T5);
            M7 = _mm256_unpackhi_epi16(T6, T7);

            N0 = _mm256_madd_epi16(M0, coeff00);
            N1 = _mm256_madd_epi16(M1, coeff01);
            N2 = _mm256_madd_epi16(M2, coeff02);
            N3 = _mm256_madd_epi16(M3, coeff03);
            N4 = _mm256_madd_epi16(M4, coeff00);
            N5 = _mm256_madd_epi16(M5, coeff01);
            N6 = _mm256_madd_epi16(M6, coeff02);
            N7 = _mm256_madd_epi16(M7, coeff03);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);
            N2 = _mm256_add_epi32(N4, N5);
            N3 = _mm256_add_epi32(N6, N7);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, offset);
            N1 = _mm256_add_epi32(N1, offset);
            N0 = _mm256_srai_epi32(N0, shift2);
            N1 = _mm256_srai_epi32(N1, shift2);
            N0 = _mm256_packus_epi32(N0, N1);
            N0 = _mm256_min_epu16(N0, max_pel);
            _mm256_storeu_si256((__m256i *)(dst + i), N0);

            p += 16;
        }
        dst += i_dst;
        tmp += i_tmp;
    }

}

void uavs3e_if_hor_ver_chroma_w4_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(32 + 3) * 4]);
    s16 *tmp = tmp_res;
    int row;
    int shift1, shift2;
    int add1, add2;

    __m128i coef0 = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i coef1 = _mm_set1_epi16(*(s16 *)(coef_x + 2));
    __m256i mCoef0 = _mm256_cvtepi8_epi16(coef0);
    __m256i mCoef1 = _mm256_cvtepi8_epi16(coef1);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i T0, T1, S0, sum;
    __m256i mAddOffset;
    __m128i m0;

    if (max_val == 255) { // 8 bit_depth
        shift1 = 0;
        shift2 = 12;
    } else { // 10 bit_depth
        shift1 = 2;
        shift2 = 10;
    }

    add1 = (1 << (shift1)) >> 1;
    add2 = 1 << (shift2 - 1);

    mAddOffset = _mm256_set1_epi32(add1);
    //HOR
    src = src - i_src - 1;
    {
        __m128i s0, t0, t1;
        __m128i m0 = _mm_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
        __m128i m1 = _mm_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
        s0 = _mm_loadu_si128((__m128i *)(src));
        t0 = _mm_shuffle_epi8(s0, m0);
        t1 = _mm_shuffle_epi8(s0, m1);
        t0 = _mm_madd_epi16(t0, _mm256_castsi256_si128(mCoef0));
        t1 = _mm_madd_epi16(t1, _mm256_castsi256_si128(mCoef1));
        t0 = _mm_add_epi32(t0, t1);

        t0 = _mm_add_epi32(t0, _mm256_castsi256_si128(mAddOffset));
        t0 = _mm_srai_epi32(t0, shift1);
        t0 = _mm_packs_epi32(t0, t0);
        _mm_storel_epi64((__m128i *)(tmp), t0);

        src += i_src;
        tmp += 4;
    }
    row = height + 2;
    while (row > 0) {
        S0 = _mm256_set_m128i(_mm_loadu_si128((__m128i *)(src + i_src)), _mm_loadu_si128((__m128i *)(src)));

        T0 = _mm256_shuffle_epi8(S0, mSwitch1);  
        T1 = _mm256_shuffle_epi8(S0, mSwitch2);

        T0 = _mm256_madd_epi16(T0, mCoef0);
        T1 = _mm256_madd_epi16(T1, mCoef1);
        sum = _mm256_add_epi32(T0, T1);

        sum = _mm256_add_epi32(sum, mAddOffset);
        sum = _mm256_srai_epi32(sum, shift1);

        m0 = _mm_packus_epi32(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));

        _mm_store_si128((__m128i *)(tmp), m0);

        row -= 2;
        src += i_src << 1;
        tmp += 8;
    }

    // VER
    tmp = tmp_res;

    {
        __m128i max_val1 = _mm_set1_epi16((pel)max_val);
        __m128i s0, s1, s2, s3, s4, d;
        __m256i coeff0, coeff1, mAddOffset2;
        __m256i S1, S2, S3;

        coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coef_y));
        coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coef_y + 2)));
        mAddOffset2 = _mm256_set1_epi32(add2);

        while (height > 0) {
            s0 = _mm_loadl_epi64((__m128i*)(tmp));
            s1 = _mm_loadl_epi64((__m128i*)(tmp + 4));
            s2 = _mm_loadl_epi64((__m128i*)(tmp + 8));
            s3 = _mm_loadl_epi64((__m128i*)(tmp + 12));
            s4 = _mm_loadl_epi64((__m128i*)(tmp + 16));

            S0 = _mm256_set_m128i(s1, s0);
            S1 = _mm256_set_m128i(s2, s1);
            S2 = _mm256_set_m128i(s3, s2);
            S3 = _mm256_set_m128i(s4, s3);

            S0 = _mm256_unpacklo_epi16(S0, S1);
            S2 = _mm256_unpacklo_epi16(S2, S3);

            T0 = _mm256_madd_epi16(S0, coeff0);
            T1 = _mm256_madd_epi16(S2, coeff1);

            T0 = _mm256_add_epi32(T0, T1);

            T0 = _mm256_add_epi32(T0, mAddOffset2);
            T0 = _mm256_srai_epi32(T0, shift2);

            d = _mm_packus_epi32(_mm256_castsi256_si128(T0), _mm256_extracti128_si256(T0, 1));
            d = _mm_min_epu16(d, max_val1);
            _mm_storel_epi64((__m128i *)dst, d);
            _mm_storel_epi64((__m128i *)(dst + i_dst), _mm_srli_si128(d, 8));

            height -= 2;
            tmp += 8;
            dst += i_dst << 1;
        }
    }
}

void uavs3e_if_hor_ver_chroma_w8_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(64 + 3) * 8]);
    s16 *tmp = tmp_res;
    int row;
    int shift1, shift2;
    int add1, add2;

    __m128i coef0 = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i coef1 = _mm_set1_epi16(*(s16 *)(coef_x + 2));
    __m256i mCoef0 = _mm256_cvtepi8_epi16(coef0);
    __m256i mCoef1 = _mm256_cvtepi8_epi16(coef1);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, sum0, sum1;
    __m256i mAddOffset;

    if (max_val == 255) { // 8 bit_depth
        shift1 = 0;
        shift2 = 12;
    }
    else { // 10 bit_depth
        shift1 = 2;
        shift2 = 10;
    }

    add1 = (1 << (shift1)) >> 1;
    add2 = 1 << (shift2 - 1);

    mAddOffset = _mm256_set1_epi32(add1);
    //HOR
    src = src - i_src - 1;
    {
        __m128i m0;
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        T0 = _mm256_shuffle_epi8(S2, mSwitch1);
        T1 = _mm256_shuffle_epi8(S2, mSwitch2);
        T0 = _mm256_madd_epi16(T0, mCoef0);
        T1 = _mm256_madd_epi16(T1, mCoef1);
        sum0 = _mm256_add_epi32(T0, T1);

        sum0 = _mm256_add_epi32(sum0, mAddOffset);
        sum0 = _mm256_srai_epi32(sum0, shift1);

        m0 = _mm_packus_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));

        _mm_storeu_si128((__m128i *)(tmp), m0);

        src += i_src;
        tmp += 8;
    }
    row = height + 2;
    while (row > 0) {
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + i_src));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);
        T0 = _mm256_shuffle_epi8(S2, mSwitch1);
        T1 = _mm256_shuffle_epi8(S2, mSwitch2);
        T2 = _mm256_shuffle_epi8(S3, mSwitch1);
        T3 = _mm256_shuffle_epi8(S3, mSwitch2);
        T0 = _mm256_madd_epi16(T0, mCoef0);
        T1 = _mm256_madd_epi16(T1, mCoef1);
        T2 = _mm256_madd_epi16(T2, mCoef0);
        T3 = _mm256_madd_epi16(T3, mCoef1);
        sum0 = _mm256_add_epi32(T0, T1);
        sum1 = _mm256_add_epi32(T2, T3);

        sum0 = _mm256_add_epi32(sum0, mAddOffset);
        sum1 = _mm256_add_epi32(sum1, mAddOffset);
        sum0 = _mm256_srai_epi32(sum0, shift1);
        sum1 = _mm256_srai_epi32(sum1, shift1);

        sum0 = _mm256_packus_epi32(sum0, sum1);
        sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);

        _mm256_storeu_si256((__m256i *)(tmp), sum0);

        row -= 2;
        src += i_src << 1;
        tmp += 16;
    }

    // VER
    tmp = tmp_res;

    {
        __m256i max_pel = _mm256_set1_epi16((pel)max_val);
        __m128i s0, s1, s2, s3, s4;
        __m256i coeff0, coeff1, mAddOffset2;
        __m256i mVal0, mVal1;

        coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coef_y));
        coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coef_y + 2)));
        mAddOffset2 = _mm256_set1_epi32(add2);

        while (height > 0) {
            s0 = _mm_loadu_si128((__m128i*)(tmp));
            s1 = _mm_loadu_si128((__m128i*)(tmp + 8));
            s2 = _mm_loadu_si128((__m128i*)(tmp + 16));
            s3 = _mm_loadu_si128((__m128i*)(tmp + 24));
            s4 = _mm_loadu_si128((__m128i*)(tmp + 32));

            height -= 2;
            S0 = _mm256_set_m128i(s1, s0);
            S1 = _mm256_set_m128i(s2, s1);
            S2 = _mm256_set_m128i(s3, s2);
            S3 = _mm256_set_m128i(s4, s3);

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpackhi_epi16(S0, S1);
            T2 = _mm256_unpacklo_epi16(S2, S3);
            T3 = _mm256_unpackhi_epi16(S2, S3);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);

            mVal0 = _mm256_add_epi32(T0, T2);
            mVal1 = _mm256_add_epi32(T1, T3);

            mVal0 = _mm256_add_epi32(mVal0, mAddOffset2);
            mVal1 = _mm256_add_epi32(mVal1, mAddOffset2);
            mVal0 = _mm256_srai_epi32(mVal0, shift2);
            mVal1 = _mm256_srai_epi32(mVal1, shift2);
            mVal0 = _mm256_packus_epi32(mVal0, mVal1);
            mVal0 = _mm256_min_epi16(mVal0, max_pel);

            _mm_storeu_si128((__m128i*)dst, _mm256_castsi256_si128(mVal0));
            _mm_storeu_si128((__m128i*)(dst + i_dst), _mm256_extracti128_si256(mVal0, 1));

            tmp += 16;
            dst += i_dst << 1;
        }
    }
}

void uavs3e_if_hor_ver_chroma_w16_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(64 + 3) * 16]);
    s16 *tmp = tmp_res;
    int row;
    int shift1, shift2;
    int add1, add2;

    __m128i coef0 = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i coef1 = _mm_set1_epi16(*(s16 *)(coef_x + 2));
    __m256i mCoef0 = _mm256_cvtepi8_epi16(coef0);
    __m256i mCoef1 = _mm256_cvtepi8_epi16(coef1);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, sum0, sum1;
    __m256i mAddOffset;

    if (max_val == 255) { // 8 bit_depth
        shift1 = 0;
        shift2 = 12;
    }
    else { // 10 bit_depth
        shift1 = 2;
        shift2 = 10;
    }

    add1 = (1 << (shift1)) >> 1;
    add2 = 1 << (shift2 - 1);

    mAddOffset = _mm256_set1_epi32(add1);
    //HOR
    src = src - i_src - 1;
    row = height + 3;
    while (row > 0) {
        S0 = _mm256_loadu_si256((__m256i *)(src));
        S1 = _mm256_loadu_si256((__m256i *)(src + 8));
        S2 = _mm256_permute4x64_epi64(S0, 0x94);
        S3 = _mm256_permute4x64_epi64(S1, 0x94);
        T0 = _mm256_shuffle_epi8(S2, mSwitch1);
        T1 = _mm256_shuffle_epi8(S2, mSwitch2);
        T2 = _mm256_shuffle_epi8(S3, mSwitch1);
        T3 = _mm256_shuffle_epi8(S3, mSwitch2);
        T0 = _mm256_madd_epi16(T0, mCoef0);
        T1 = _mm256_madd_epi16(T1, mCoef1);
        T2 = _mm256_madd_epi16(T2, mCoef0);
        T3 = _mm256_madd_epi16(T3, mCoef1);
        sum0 = _mm256_add_epi32(T0, T1);
        sum1 = _mm256_add_epi32(T2, T3);

        sum0 = _mm256_add_epi32(sum0, mAddOffset);
        sum1 = _mm256_add_epi32(sum1, mAddOffset);
        sum0 = _mm256_srai_epi32(sum0, shift1);
        sum1 = _mm256_srai_epi32(sum1, shift1);

        sum0 = _mm256_packus_epi32(sum0, sum1);
        sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);

        _mm256_storeu_si256((__m256i *)(tmp), sum0);

        row--;
        src += i_src;
        tmp += 16;
    }

    // VER
    tmp = tmp_res;

    {
        __m256i max_pel = _mm256_set1_epi16((pel)max_val);
        __m256i S4, T4, T5, T6, T7;
        __m256i coeff0, coeff1, mAddOffset2;
        __m256i mVal0, mVal1, mVal2, mVal3;

        coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coef_y));
        coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coef_y + 2)));
        mAddOffset2 = _mm256_set1_epi32(add2);

        while (height > 0) {
            uavs3e_prefetch(tmp + 80, _MM_HINT_NTA);
            height -= 2;
            S0 = _mm256_loadu_si256((__m256i*)(tmp));
            S1 = _mm256_loadu_si256((__m256i*)(tmp + 16));
            S2 = _mm256_loadu_si256((__m256i*)(tmp + 32));
            S3 = _mm256_loadu_si256((__m256i*)(tmp + 48));
            S4 = _mm256_loadu_si256((__m256i*)(tmp + 64));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpackhi_epi16(S0, S1);
            T2 = _mm256_unpacklo_epi16(S2, S3);
            T3 = _mm256_unpackhi_epi16(S2, S3);
            T4 = _mm256_unpacklo_epi16(S1, S2);
            T5 = _mm256_unpackhi_epi16(S1, S2);
            T6 = _mm256_unpacklo_epi16(S3, S4);
            T7 = _mm256_unpackhi_epi16(S3, S4);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);
            T4 = _mm256_madd_epi16(T4, coeff0);
            T5 = _mm256_madd_epi16(T5, coeff0);
            T6 = _mm256_madd_epi16(T6, coeff1);
            T7 = _mm256_madd_epi16(T7, coeff1);

            mVal0 = _mm256_add_epi32(T0, T2);
            mVal1 = _mm256_add_epi32(T1, T3);
            mVal2 = _mm256_add_epi32(T4, T6);
            mVal3 = _mm256_add_epi32(T5, T7);

            mVal0 = _mm256_add_epi32(mVal0, mAddOffset2);
            mVal1 = _mm256_add_epi32(mVal1, mAddOffset2);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset2);
            mVal3 = _mm256_add_epi32(mVal3, mAddOffset2);
            mVal0 = _mm256_srai_epi32(mVal0, shift2);
            mVal1 = _mm256_srai_epi32(mVal1, shift2);
            mVal2 = _mm256_srai_epi32(mVal2, shift2);
            mVal3 = _mm256_srai_epi32(mVal3, shift2);
            mVal0 = _mm256_packus_epi32(mVal0, mVal1);
            mVal2 = _mm256_packus_epi32(mVal2, mVal3);
            mVal0 = _mm256_min_epi16(mVal0, max_pel);
            mVal2 = _mm256_min_epi16(mVal2, max_pel);

            _mm256_storeu_si256((__m256i*)dst, mVal0);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), mVal2);

            tmp += 32;
            dst += i_dst << 1;
        }
    }
}

void uavs3e_if_hor_ver_chroma_w16x_avx2(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_32(s16 tmp_res[(64 + 3) * 64]);
    s16 *tmp = tmp_res;
    int row, col;
    int shift1, shift2;
    int add1, add2;

    __m128i coef0 = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i coef1 = _mm_set1_epi16(*(s16 *)(coef_x + 2));
    __m256i mCoef0 = _mm256_cvtepi8_epi16(coef0);
    __m256i mCoef1 = _mm256_cvtepi8_epi16(coef1);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9, 0, 1, 2, 3, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13, 4, 5, 6, 7, 6, 7, 8, 9, 8, 9, 10, 11, 10, 11, 12, 13);
    __m256i T0, T1, T2, T3, S0, S1, S2, S3, sum0, sum1;
    __m256i mAddOffset;

    if (max_val == 255) { // 8 bit_depth
        shift1 = 0;
        shift2 = 12;
    }
    else { // 10 bit_depth
        shift1 = 2;
        shift2 = 10;
    }

    add1 = (1 << (shift1)) >> 1;
    add2 = 1 << (shift2 - 1);

    mAddOffset = _mm256_set1_epi32(add1);
    //HOR
    src = src - i_src - 1;
    row = height + 3;
    while (row > 0) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 16) {
            S0 = _mm256_loadu_si256((__m256i *)(src + col));
            S1 = _mm256_loadu_si256((__m256i *)(src + col + 8));
            S2 = _mm256_permute4x64_epi64(S0, 0x94);
            S3 = _mm256_permute4x64_epi64(S1, 0x94);
            T0 = _mm256_shuffle_epi8(S2, mSwitch1);
            T1 = _mm256_shuffle_epi8(S2, mSwitch2);
            T2 = _mm256_shuffle_epi8(S3, mSwitch1);
            T3 = _mm256_shuffle_epi8(S3, mSwitch2);
            T0 = _mm256_madd_epi16(T0, mCoef0);
            T1 = _mm256_madd_epi16(T1, mCoef1);
            T2 = _mm256_madd_epi16(T2, mCoef0);
            T3 = _mm256_madd_epi16(T3, mCoef1);
            sum0 = _mm256_add_epi32(T0, T1);
            sum1 = _mm256_add_epi32(T2, T3);

            sum0 = _mm256_add_epi32(sum0, mAddOffset);
            sum1 = _mm256_add_epi32(sum1, mAddOffset);
            sum0 = _mm256_srai_epi32(sum0, shift1);
            sum1 = _mm256_srai_epi32(sum1, shift1);

            sum0 = _mm256_packus_epi32(sum0, sum1);
            sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);

            _mm256_storeu_si256((__m256i *)(tmp + col), sum0);
        }
        row--;
        src += i_src;
        tmp += width;
    }

    // VER
    tmp = tmp_res;

    {
        __m256i max_pel = _mm256_set1_epi16((pel)max_val);
        __m256i coeff0, coeff1, mAddOffset2;
        __m256i mVal0, mVal1;
        int i_tmp = width;
        int i_tmp2 = width << 1;
        int i_tmp3 = i_tmp2 + width;
        coeff0 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)coef_y));
        coeff1 = _mm256_cvtepi8_epi16(_mm_set1_epi16(*(s16 *)(coef_y + 2)));
        mAddOffset2 = _mm256_set1_epi32(add2);

        while (height > 0) {
            uavs3e_prefetch(tmp + 4 * i_tmp, _MM_HINT_NTA);
            height--;
            for (col = 0; col < width; col += 16) {
                S0 = _mm256_loadu_si256((__m256i*)(tmp + col));
                S1 = _mm256_loadu_si256((__m256i*)(tmp + col + i_tmp));
                S2 = _mm256_loadu_si256((__m256i*)(tmp + col + i_tmp2));
                S3 = _mm256_loadu_si256((__m256i*)(tmp + col + i_tmp3));

                T0 = _mm256_unpacklo_epi16(S0, S1);
                T1 = _mm256_unpackhi_epi16(S0, S1);
                T2 = _mm256_unpacklo_epi16(S2, S3);
                T3 = _mm256_unpackhi_epi16(S2, S3);

                T0 = _mm256_madd_epi16(T0, coeff0);
                T1 = _mm256_madd_epi16(T1, coeff0);
                T2 = _mm256_madd_epi16(T2, coeff1);
                T3 = _mm256_madd_epi16(T3, coeff1);

                mVal0 = _mm256_add_epi32(T0, T2);
                mVal1 = _mm256_add_epi32(T1, T3);

                mVal0 = _mm256_add_epi32(mVal0, mAddOffset2);
                mVal1 = _mm256_add_epi32(mVal1, mAddOffset2);
                mVal0 = _mm256_srai_epi32(mVal0, shift2);
                mVal1 = _mm256_srai_epi32(mVal1, shift2);
                mVal0 = _mm256_packus_epi32(mVal0, mVal1);
                mVal0 = _mm256_min_epi16(mVal0, max_pel);

                _mm256_storeu_si256((__m256i*)(dst + col), mVal0);
            }
            tmp += i_tmp;
            dst += i_dst;
        }
    }
}

void uavs3e_if_hor_luma_frame_avx2(const pel *src, int i_src, pel *dst[3], int i_dst, s16 *dst_tmp[3], int i_dst_tmp, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    s16 *dt0 = dst_tmp[0];
    s16 *dt1 = dst_tmp[1];
    s16 *dt2 = dst_tmp[2];
    int shift_tmp = bit_depth - 8;
    int add_tmp = (1 << (shift_tmp)) >> 1;
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel)max_pixel);
    s32 * coef;
    coef = (s32 *)(coeff[0]);
    __m128i mCoef00 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);
    coef = (s32 *)(coeff[1]);
    __m128i mCoef11 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);
    coef = (s32 *)(coeff[2]);
    __m128i mCoef22 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);

    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;

    __m256i offset1 = _mm256_set1_epi32(add_tmp);
    __m256i offset2 = _mm256_set1_epi32(32);

    __m256i mCoef0 = _mm256_cvtepi8_epi16(mCoef00);
    __m256i mCoef1 = _mm256_cvtepi8_epi16(mCoef11);
    __m256i mCoef2 = _mm256_cvtepi8_epi16(mCoef22);

    src -= 3;

    for (j = 0; j < height; j++) {
        const pel *p = src;
        for (i = 0; i < width; i += 16) {

            T0 = _mm256_loadu_si256((__m256i*)p++);
            T1 = _mm256_loadu_si256((__m256i*)p++);
            T2 = _mm256_loadu_si256((__m256i*)p++);
            T3 = _mm256_loadu_si256((__m256i*)p++);
            T4 = _mm256_loadu_si256((__m256i*)p++);
            T5 = _mm256_loadu_si256((__m256i*)p++);
            T6 = _mm256_loadu_si256((__m256i*)p++);
            T7 = _mm256_loadu_si256((__m256i*)p++);

            M0 = _mm256_madd_epi16(T0, mCoef0);
            M1 = _mm256_madd_epi16(T1, mCoef0);
            M2 = _mm256_madd_epi16(T2, mCoef0);
            M3 = _mm256_madd_epi16(T3, mCoef0);
            M4 = _mm256_madd_epi16(T4, mCoef0);
            M5 = _mm256_madd_epi16(T5, mCoef0);
            M6 = _mm256_madd_epi16(T6, mCoef0);
            M7 = _mm256_madd_epi16(T7, mCoef0);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);
            M2 = _mm256_hadd_epi32(M4, M5);
            M3 = _mm256_hadd_epi32(M6, M7);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);

            M2 = _mm256_add_epi32(M0, offset1);
            M3 = _mm256_add_epi32(M1, offset1);
            M2 = _mm256_srai_epi32(M2, shift_tmp);
            M3 = _mm256_srai_epi32(M3, shift_tmp);
            M2 = _mm256_packs_epi32(M2, M3);
            _mm256_storeu_si256((__m256i*)&dt0[i], M2);

            M2 = _mm256_add_epi32(M0, offset2);
            M3 = _mm256_add_epi32(M1, offset2);
            M2 = _mm256_srai_epi32(M2, 6);
            M3 = _mm256_srai_epi32(M3, 6);
            M2 = _mm256_packus_epi32(M2, M3);
            M2 = _mm256_min_epu16(M2, max_val);
            _mm256_storeu_si256((__m256i*)&d0[i], M2);

            M0 = _mm256_madd_epi16(T0, mCoef1);
            M1 = _mm256_madd_epi16(T1, mCoef1);
            M2 = _mm256_madd_epi16(T2, mCoef1);
            M3 = _mm256_madd_epi16(T3, mCoef1);
            M4 = _mm256_madd_epi16(T4, mCoef1);
            M5 = _mm256_madd_epi16(T5, mCoef1);
            M6 = _mm256_madd_epi16(T6, mCoef1);
            M7 = _mm256_madd_epi16(T7, mCoef1);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);
            M2 = _mm256_hadd_epi32(M4, M5);
            M3 = _mm256_hadd_epi32(M6, M7);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);

            M2 = _mm256_add_epi32(M0, offset1);
            M3 = _mm256_add_epi32(M1, offset1);
            M2 = _mm256_srai_epi32(M2, shift_tmp);
            M3 = _mm256_srai_epi32(M3, shift_tmp);
            M2 = _mm256_packs_epi32(M2, M3);
            _mm256_storeu_si256((__m256i*)&dt1[i], M2);

            M2 = _mm256_add_epi32(M0, offset2);
            M3 = _mm256_add_epi32(M1, offset2);
            M2 = _mm256_srai_epi32(M2, 6);
            M3 = _mm256_srai_epi32(M3, 6);
            M2 = _mm256_packus_epi32(M2, M3);
            M2 = _mm256_min_epu16(M2, max_val);
            _mm256_storeu_si256((__m256i*)&d1[i], M2);

            M0 = _mm256_madd_epi16(T0, mCoef2);
            M1 = _mm256_madd_epi16(T1, mCoef2);
            M2 = _mm256_madd_epi16(T2, mCoef2);
            M3 = _mm256_madd_epi16(T3, mCoef2);
            M4 = _mm256_madd_epi16(T4, mCoef2);
            M5 = _mm256_madd_epi16(T5, mCoef2);
            M6 = _mm256_madd_epi16(T6, mCoef2);
            M7 = _mm256_madd_epi16(T7, mCoef2);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);
            M2 = _mm256_hadd_epi32(M4, M5);
            M3 = _mm256_hadd_epi32(M6, M7);

            M0 = _mm256_hadd_epi32(M0, M1);
            M1 = _mm256_hadd_epi32(M2, M3);

            M2 = _mm256_add_epi32(M0, offset1);
            M3 = _mm256_add_epi32(M1, offset1);
            M2 = _mm256_srai_epi32(M2, shift_tmp);
            M3 = _mm256_srai_epi32(M3, shift_tmp);
            M2 = _mm256_packs_epi32(M2, M3);
            _mm256_storeu_si256((__m256i*)&dt2[i], M2);

            M2 = _mm256_add_epi32(M0, offset2);
            M3 = _mm256_add_epi32(M1, offset2);
            M2 = _mm256_srai_epi32(M2, 6);
            M3 = _mm256_srai_epi32(M3, 6);
            M2 = _mm256_packus_epi32(M2, M3);
            M2 = _mm256_min_epu16(M2, max_val);
            _mm256_storeu_si256((__m256i*)&d2[i], M2);
            p += 8;
        }
        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        dt0 += i_dst_tmp;
        dt1 += i_dst_tmp;
        dt2 += i_dst_tmp;
        src += i_src;
    }

}

void uavs3e_if_ver_luma_frame_avx2(const pel *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j;
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel)max_pixel);
    __m256i mAddOffset = _mm256_set1_epi32(32);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i N0, N1, N2, N3, N4, N5, N6, N7;

    __m128i coeff0 = _mm_set1_epi16(*(s16*)coeff[0]);
    __m128i coeff1 = _mm_set1_epi16(*(s16*)(coeff[0] + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16*)(coeff[0] + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16*)(coeff[0] + 6));
    __m256i coeff00 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff01 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff02 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff03 = _mm256_cvtepi8_epi16(coeff3);

    coeff0 = _mm_set1_epi16(*(s16*)coeff[1]);
    coeff1 = _mm_set1_epi16(*(s16*)(coeff[1] + 2));
    __m256i coeff10 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff11 = _mm256_cvtepi8_epi16(coeff1);

    coeff0 = _mm_set1_epi16(*(s16*)coeff[2]);
    coeff1 = _mm_set1_epi16(*(s16*)(coeff[2] + 2));
    coeff2 = _mm_set1_epi16(*(s16*)(coeff[2] + 4));
    coeff3 = _mm_set1_epi16(*(s16*)(coeff[2] + 6));
    __m256i coeff20 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff21 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff22 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff23 = _mm256_cvtepi8_epi16(coeff3);


    src -= 3 * i_src;

    for (j = 0; j < height; j++) {
        const pel *p = src;
        for (i = 0; i < width; i += 16) {
            T0 = _mm256_loadu_si256((__m256i*)(p));
            T1 = _mm256_loadu_si256((__m256i*)(p + i_src));
            T2 = _mm256_loadu_si256((__m256i*)(p + 2 * i_src));
            T3 = _mm256_loadu_si256((__m256i*)(p + 3 * i_src));
            T4 = _mm256_loadu_si256((__m256i*)(p + 4 * i_src));
            T5 = _mm256_loadu_si256((__m256i*)(p + 5 * i_src));
            T6 = _mm256_loadu_si256((__m256i*)(p + 6 * i_src));
            T7 = _mm256_loadu_si256((__m256i*)(p + 7 * i_src));

            M0 = _mm256_unpacklo_epi16(T0, T1);
            M1 = _mm256_unpacklo_epi16(T2, T3);
            M2 = _mm256_unpacklo_epi16(T4, T5);
            M3 = _mm256_unpacklo_epi16(T6, T7);
            M4 = _mm256_unpackhi_epi16(T0, T1);
            M5 = _mm256_unpackhi_epi16(T2, T3);
            M6 = _mm256_unpackhi_epi16(T4, T5);
            M7 = _mm256_unpackhi_epi16(T6, T7);

            N0 = _mm256_madd_epi16(M0, coeff00);
            N1 = _mm256_madd_epi16(M1, coeff01);
            N2 = _mm256_madd_epi16(M2, coeff02);
            N3 = _mm256_madd_epi16(M3, coeff03);
            N4 = _mm256_madd_epi16(M4, coeff00);
            N5 = _mm256_madd_epi16(M5, coeff01);
            N6 = _mm256_madd_epi16(M6, coeff02);
            N7 = _mm256_madd_epi16(M7, coeff03);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);
            N2 = _mm256_add_epi32(N4, N5);
            N3 = _mm256_add_epi32(N6, N7);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, mAddOffset);
            N1 = _mm256_add_epi32(N1, mAddOffset);
            N0 = _mm256_srai_epi32(N0, 6);
            N1 = _mm256_srai_epi32(N1, 6);
            N0 = _mm256_packus_epi32(N0, N1);
            N0 = _mm256_min_epu16(N0, max_val);
            _mm256_storeu_si256((__m256i*)&d0[i], N0);

            N0 = _mm256_madd_epi16(M0, coeff20);
            N1 = _mm256_madd_epi16(M1, coeff21);
            N2 = _mm256_madd_epi16(M2, coeff22);
            N3 = _mm256_madd_epi16(M3, coeff23);
            N4 = _mm256_madd_epi16(M4, coeff20);
            N5 = _mm256_madd_epi16(M5, coeff21);
            N6 = _mm256_madd_epi16(M6, coeff22);
            N7 = _mm256_madd_epi16(M7, coeff23);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);
            N2 = _mm256_add_epi32(N4, N5);
            N3 = _mm256_add_epi32(N6, N7);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, mAddOffset);
            N1 = _mm256_add_epi32(N1, mAddOffset);
            N0 = _mm256_srai_epi32(N0, 6);
            N1 = _mm256_srai_epi32(N1, 6);
            N0 = _mm256_packus_epi32(N0, N1);
            N0 = _mm256_min_epu16(N0, max_val);
            _mm256_storeu_si256((__m256i*)&d2[i], N0);

            T0 = _mm256_add_epi16(T0, T7);
            T1 = _mm256_add_epi16(T1, T6);
            T2 = _mm256_add_epi16(T2, T5);
            T3 = _mm256_add_epi16(T3, T4);

            M0 = _mm256_unpacklo_epi16(T0, T1);
            M1 = _mm256_unpacklo_epi16(T2, T3);
            M2 = _mm256_unpackhi_epi16(T0, T1);
            M3 = _mm256_unpackhi_epi16(T2, T3);

            N0 = _mm256_madd_epi16(M0, coeff10);
            N1 = _mm256_madd_epi16(M1, coeff11);
            N2 = _mm256_madd_epi16(M2, coeff10);
            N3 = _mm256_madd_epi16(M3, coeff11);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, mAddOffset);
            N1 = _mm256_add_epi32(N1, mAddOffset);
            N0 = _mm256_srai_epi32(N0, 6);
            N1 = _mm256_srai_epi32(N1, 6);
            N0 = _mm256_packus_epi32(N0, N1);
            N0 = _mm256_min_epu16(N0, max_val);
            _mm256_storeu_si256((__m256i*)&d1[i], N0);

            p += 16;
        }
        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }
}

void uavs3e_if_ver_luma_frame_ext_avx2(const s16 *src, int i_src, pel *dst[3], int i_dst, int width, int height, s8(*coeff)[8], int bit_depth)
{
    int i, j;
    int shift1 = 20 - bit_depth;
    int add1 = 1 << (shift1 - 1);
    pel *d0 = dst[0];
    pel *d1 = dst[1];
    pel *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel)max_pixel);
    __m256i mAddOffset = _mm256_set1_epi32(add1);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i N0, N1, N2, N3, N4, N5, N6, N7;

    __m128i coeff0 = _mm_set1_epi16(*(s16*)coeff[0]);
    __m128i coeff1 = _mm_set1_epi16(*(s16*)(coeff[0] + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16*)(coeff[0] + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16*)(coeff[0] + 6));
    __m256i coeff00 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff01 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff02 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff03 = _mm256_cvtepi8_epi16(coeff3);

    coeff0 = _mm_set1_epi16(*(s16*)coeff[1]);
    coeff1 = _mm_set1_epi16(*(s16*)(coeff[1] + 2));
    coeff2 = _mm_set1_epi16(*(s16*)(coeff[1] + 4));
    coeff3 = _mm_set1_epi16(*(s16*)(coeff[1] + 6));
    __m256i coeff10 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff11 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff12 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff13 = _mm256_cvtepi8_epi16(coeff3);

    coeff0 = _mm_set1_epi16(*(s16*)coeff[2]);
    coeff1 = _mm_set1_epi16(*(s16*)(coeff[2] + 2));
    coeff2 = _mm_set1_epi16(*(s16*)(coeff[2] + 4));
    coeff3 = _mm_set1_epi16(*(s16*)(coeff[2] + 6));
    __m256i coeff20 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff21 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff22 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff23 = _mm256_cvtepi8_epi16(coeff3);


    src -= 3 * i_src;

    for (j = 0; j < height; j++) {
        const s16 *p = src;
        for (i = 0; i < width; i += 16) {
            T0 = _mm256_loadu_si256((__m256i*)(p));
            T1 = _mm256_loadu_si256((__m256i*)(p + i_src));
            T2 = _mm256_loadu_si256((__m256i*)(p + 2 * i_src));
            T3 = _mm256_loadu_si256((__m256i*)(p + 3 * i_src));
            T4 = _mm256_loadu_si256((__m256i*)(p + 4 * i_src));
            T5 = _mm256_loadu_si256((__m256i*)(p + 5 * i_src));
            T6 = _mm256_loadu_si256((__m256i*)(p + 6 * i_src));
            T7 = _mm256_loadu_si256((__m256i*)(p + 7 * i_src));

            M0 = _mm256_unpacklo_epi16(T0, T1);
            M1 = _mm256_unpacklo_epi16(T2, T3);
            M2 = _mm256_unpacklo_epi16(T4, T5);
            M3 = _mm256_unpacklo_epi16(T6, T7);
            M4 = _mm256_unpackhi_epi16(T0, T1);
            M5 = _mm256_unpackhi_epi16(T2, T3);
            M6 = _mm256_unpackhi_epi16(T4, T5);
            M7 = _mm256_unpackhi_epi16(T6, T7);

            N0 = _mm256_madd_epi16(M0, coeff00);
            N1 = _mm256_madd_epi16(M1, coeff01);
            N2 = _mm256_madd_epi16(M2, coeff02);
            N3 = _mm256_madd_epi16(M3, coeff03);
            N4 = _mm256_madd_epi16(M4, coeff00);
            N5 = _mm256_madd_epi16(M5, coeff01);
            N6 = _mm256_madd_epi16(M6, coeff02);
            N7 = _mm256_madd_epi16(M7, coeff03);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);
            N2 = _mm256_add_epi32(N4, N5);
            N3 = _mm256_add_epi32(N6, N7);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, mAddOffset);
            N1 = _mm256_add_epi32(N1, mAddOffset);
            N0 = _mm256_srai_epi32(N0, shift1);
            N1 = _mm256_srai_epi32(N1, shift1);
            N0 = _mm256_packus_epi32(N0, N1);
            N0 = _mm256_min_epu16(N0, max_val);
            _mm256_storeu_si256((__m256i*)&d0[i], N0);

            N0 = _mm256_madd_epi16(M0, coeff10);
            N1 = _mm256_madd_epi16(M1, coeff11);
            N2 = _mm256_madd_epi16(M2, coeff12);
            N3 = _mm256_madd_epi16(M3, coeff13);
            N4 = _mm256_madd_epi16(M4, coeff10);
            N5 = _mm256_madd_epi16(M5, coeff11);
            N6 = _mm256_madd_epi16(M6, coeff12);
            N7 = _mm256_madd_epi16(M7, coeff13);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);
            N2 = _mm256_add_epi32(N4, N5);
            N3 = _mm256_add_epi32(N6, N7);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, mAddOffset);
            N1 = _mm256_add_epi32(N1, mAddOffset);
            N0 = _mm256_srai_epi32(N0, shift1);
            N1 = _mm256_srai_epi32(N1, shift1);
            N0 = _mm256_packus_epi32(N0, N1);
            N0 = _mm256_min_epu16(N0, max_val);
            _mm256_storeu_si256((__m256i*)&d1[i], N0);

            N0 = _mm256_madd_epi16(M0, coeff20);
            N1 = _mm256_madd_epi16(M1, coeff21);
            N2 = _mm256_madd_epi16(M2, coeff22);
            N3 = _mm256_madd_epi16(M3, coeff23);
            N4 = _mm256_madd_epi16(M4, coeff20);
            N5 = _mm256_madd_epi16(M5, coeff21);
            N6 = _mm256_madd_epi16(M6, coeff22);
            N7 = _mm256_madd_epi16(M7, coeff23);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);
            N2 = _mm256_add_epi32(N4, N5);
            N3 = _mm256_add_epi32(N6, N7);

            N0 = _mm256_add_epi32(N0, N1);
            N1 = _mm256_add_epi32(N2, N3);

            N0 = _mm256_add_epi32(N0, mAddOffset);
            N1 = _mm256_add_epi32(N1, mAddOffset);
            N0 = _mm256_srai_epi32(N0, shift1);
            N1 = _mm256_srai_epi32(N1, shift1);
            N0 = _mm256_packus_epi32(N0, N1);
            N0 = _mm256_min_epu16(N0, max_val);
            _mm256_storeu_si256((__m256i*)&d2[i], N0);

            p += 16;
        }
        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }
}

#endif
