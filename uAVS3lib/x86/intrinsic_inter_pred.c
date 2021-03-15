#include "intrinsic.h"

#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4127) // conditional expression is constant

/* 8bit compiler */

void com_if_filter_hor_4_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row, col;
    const i16s_t offset = 32;
    const int shift = 6;

    __m128i mAddOffset = _mm_set1_epi16(offset);

    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6);
    __m128i mSwitch2 = _mm_setr_epi8(4, 5, 6, 7, 5, 6, 7, 8, 6, 7, 8, 9, 7, 8, 9, 10);
    __m128i mCoef = _mm_set1_epi32(*(i32s_t*)coeff);
    __m128i mask = _mm_loadu_si128((__m128i*)(intrinsic_mask[(width & 7) - 1]));

    src -= 1;

    for (row = 0; row < height; row++) {
        __m128i mT20, mT40, mSum, mVal;

        for (col = 0; col < width - 7; col += 8) {

            __m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
            
            mT20 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
            mT40 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);
       
            mSum = _mm_hadd_epi16(mT20, mT40);
            mVal = _mm_add_epi16(mSum, mAddOffset);

            mVal = _mm_srai_epi16(mVal, shift);
			mVal = _mm_packus_epi16(mVal, mVal);

			_mm_storel_epi64((__m128i*)&dst[col], mVal);
        }

        if (col < width) { 

            __m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));

            mT20 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
            mT40 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);

            mSum = _mm_hadd_epi16(mT20, mT40);
            mVal = _mm_add_epi16(mSum, mAddOffset);

            mVal = _mm_srai_epi16(mVal, shift);
			mVal = _mm_packus_epi16(mVal, mVal);

            _mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
        }

        src += i_src;
        dst += i_dst;
    }
}

void com_if_filter_hor_4_w16_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row, col;
    const int offset = 32;
    const int shift = 6;

    __m256i mCoef = _mm256_set1_epi32(*(i32s_t*)coeff);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6, 0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 5, 6, 7, 8, 6, 7, 8, 9, 7, 8, 9, 10, 4, 5, 6, 7, 5, 6, 7, 8, 6, 7, 8, 9, 7, 8, 9, 10);
    __m256i mAddOffset = _mm256_set1_epi16((short)offset);
    __m256i T0, T1, S, S0, sum;
    __m256i mask16 = _mm256_setr_epi32(-1, -1, -1, -1, 0, 0, 0, 0);
    src -= 1;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col += 16) {
            S = _mm256_loadu_si256((__m256i*)(src + col));
            S0 = _mm256_permute4x64_epi64(S, 0x94);
            T0 = _mm256_maddubs_epi16(_mm256_shuffle_epi8(S0, mSwitch1), mCoef);
            T1 = _mm256_maddubs_epi16(_mm256_shuffle_epi8(S0, mSwitch2), mCoef);
            sum = _mm256_hadd_epi16(T0, T1);

            sum = _mm256_add_epi16(sum, mAddOffset);
            sum = _mm256_srai_epi16(sum, shift);
            sum = _mm256_packus_epi16(sum, sum);
            sum = _mm256_permute4x64_epi64(sum, 0xd8);

            _mm256_maskstore_epi32((int*)(dst + col), mask16, sum);
        }
        src += i_src;
        dst += i_dst;
    }
}

void com_if_filter_ver_4_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row, col;
    const i16s_t offset = 32;
    const int shift = 6;
    int bsym = (coeff[1] == coeff[2]);
    __m128i mAddOffset = _mm_set1_epi16(offset);

    pel_t const *p;

    __m128i mask = _mm_loadu_si128((__m128i*)(intrinsic_mask[(width & 7) - 1]));

    src -= i_src;

    if (bsym) {
        __m128i coeff0 = _mm_set1_epi8(coeff[0]);
        __m128i coeff1 = _mm_set1_epi8(coeff[1]);
        __m128i mVal;

        for (row = 0; row < height; row++) {
            p = src;
            for (col = 0; col < width - 7; col += 8) {
                __m128i T00 = _mm_loadu_si128((__m128i*)(p));
                __m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
                __m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
                __m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

                T00 = _mm_unpacklo_epi8(T00, T30);
                T10 = _mm_unpacklo_epi8(T10, T20);

                T00 = _mm_maddubs_epi16(T00, coeff0);
                T10 = _mm_maddubs_epi16(T10, coeff1);

                mVal = _mm_add_epi16(T00, T10);

                mVal = _mm_add_epi16(mVal, mAddOffset);
                mVal = _mm_srai_epi16(mVal, shift);
                mVal = _mm_packus_epi16(mVal, mVal);

                _mm_storel_epi64((__m128i*)&dst[col], mVal);

                p += 8;
            }

            if (col < width) { // store either 1, 2, 3, 4, 5, 6, or 7 8-bit results in dst
                __m128i T00 = _mm_loadu_si128((__m128i*)(p));
                __m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
                __m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
                __m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

                T00 = _mm_unpacklo_epi8(T00, T30);
                T10 = _mm_unpacklo_epi8(T10, T20);

                T00 = _mm_maddubs_epi16(T00, coeff0);
                T10 = _mm_maddubs_epi16(T10, coeff1);

                mVal = _mm_add_epi16(T00, T10);

                mVal = _mm_add_epi16(mVal, mAddOffset);
                mVal = _mm_srai_epi16(mVal, shift);
                mVal = _mm_packus_epi16(mVal, mVal);

                _mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
            }

            src += i_src;
            dst += i_dst;
        }
    }
    else {
        __m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coeff);
        __m128i coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff + 2));
        __m128i mVal;
        for (row = 0; row < height; row++) {
            p = src;
            for (col = 0; col < width - 7; col += 8) {
                __m128i T00 = _mm_loadu_si128((__m128i*)(p));
                __m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
                __m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
                __m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

                T00 = _mm_unpacklo_epi8(T00, T10);
                T10 = _mm_unpacklo_epi8(T20, T30);

                T00 = _mm_maddubs_epi16(T00, coeff0);
                T10 = _mm_maddubs_epi16(T10, coeff1);

                mVal = _mm_add_epi16(T00, T10);

                mVal = _mm_add_epi16(mVal, mAddOffset);
                mVal = _mm_srai_epi16(mVal, shift);
                mVal = _mm_packus_epi16(mVal, mVal);

                _mm_storel_epi64((__m128i*)&dst[col], mVal);

                p += 8;
            }

            if (col < width) { // store either 1, 2, 3, 4, 5, 6, or 7 8-bit results in dst
                __m128i T00 = _mm_loadu_si128((__m128i*)(p));
                __m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
                __m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
                __m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

                T00 = _mm_unpacklo_epi8(T00, T10);
                T10 = _mm_unpacklo_epi8(T20, T30);

                T00 = _mm_maddubs_epi16(T00, coeff0);
                T10 = _mm_maddubs_epi16(T10, coeff1);

                mVal = _mm_add_epi16(T00, T10);

                mVal = _mm_add_epi16(mVal, mAddOffset);
                mVal = _mm_srai_epi16(mVal, shift);
                mVal = _mm_packus_epi16(mVal, mVal);

                _mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
            }

            src += i_src;
            dst += i_dst;
        }
    }
}

void com_if_filter_ver_4_w16_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row;
    const int offset = 32;
    const int shift = 6;
    int bsym = (coeff[1] == coeff[2]);
    __m256i mAddOffset = _mm256_set1_epi16((short)offset);
    const int i_src2 = i_src << 1;
    const int i_src3 = i_src + i_src2;
    const int i_src4 = i_src << 2;

    src -= i_src;

    if (bsym) {
        __m256i coeff0 = _mm256_set1_epi8(coeff[0]);
        __m256i coeff1 = _mm256_set1_epi8(coeff[1]);
        __m256i T0, T1, T2, T3, mVal1, mVal2;
        __m256i R0, R1, R2, R3;
        for (row = 0; row < height; row += 2) {
            __m128i S0 = _mm_loadu_si128((__m128i*)(src));
            __m128i S1 = _mm_loadu_si128((__m128i*)(src + i_src));
            __m128i S2 = _mm_loadu_si128((__m128i*)(src + i_src2));
            __m128i S3 = _mm_loadu_si128((__m128i*)(src + i_src3));
            __m128i S4 = _mm_loadu_si128((__m128i*)(src + i_src4));

            R0 = _mm256_set_m128i(S0, S1);
            R1 = _mm256_set_m128i(S1, S2);
            R2 = _mm256_set_m128i(S2, S3);
            R3 = _mm256_set_m128i(S3, S4);

            T0 = _mm256_unpacklo_epi8(R0, R3);
            T1 = _mm256_unpackhi_epi8(R0, R3);
            T2 = _mm256_unpacklo_epi8(R1, R2);
            T3 = _mm256_unpackhi_epi8(R1, R2);

            T0 = _mm256_maddubs_epi16(T0, coeff0);
            T1 = _mm256_maddubs_epi16(T1, coeff0);
            T2 = _mm256_maddubs_epi16(T2, coeff1);
            T3 = _mm256_maddubs_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi16(T0, T2);
            mVal2 = _mm256_add_epi16(T1, T3);

            mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi16(mVal1, shift);
            mVal2 = _mm256_srai_epi16(mVal2, shift);
            mVal1 = _mm256_packus_epi16(mVal1, mVal2);

            _mm256_storeu2_m128i((__m128i*)dst, (__m128i*)(dst + i_dst), mVal1);

            src += i_src2;
            dst += 2 * i_dst;
        }
    }
    else {
        __m256i coeff0 = _mm256_set1_epi16(*(i16s_t*)coeff);
        __m256i coeff1 = _mm256_set1_epi16(*(i16s_t*)(coeff + 2));
        __m256i T0, T1, T2, T3, mVal1, mVal2;
        __m256i R0, R1, R2, R3;
        for (row = 0; row < height; row += 2) {
            __m128i S0 = _mm_loadu_si128((__m128i*)(src));
            __m128i S1 = _mm_loadu_si128((__m128i*)(src + i_src));
            __m128i S2 = _mm_loadu_si128((__m128i*)(src + i_src2));
            __m128i S3 = _mm_loadu_si128((__m128i*)(src + i_src3));
            __m128i S4 = _mm_loadu_si128((__m128i*)(src + i_src4));

            R0 = _mm256_set_m128i(S0, S1);
            R1 = _mm256_set_m128i(S1, S2);
            R2 = _mm256_set_m128i(S2, S3);
            R3 = _mm256_set_m128i(S3, S4);

            T0 = _mm256_unpacklo_epi8(R0, R1);
            T1 = _mm256_unpackhi_epi8(R0, R1);
            T2 = _mm256_unpacklo_epi8(R2, R3);
            T3 = _mm256_unpackhi_epi8(R2, R3);

            T0 = _mm256_maddubs_epi16(T0, coeff0);
            T1 = _mm256_maddubs_epi16(T1, coeff0);
            T2 = _mm256_maddubs_epi16(T2, coeff1);
            T3 = _mm256_maddubs_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi16(T0, T2);
            mVal2 = _mm256_add_epi16(T1, T3);

            mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi16(mVal1, shift);
            mVal2 = _mm256_srai_epi16(mVal2, shift);
            mVal1 = _mm256_packus_epi16(mVal1, mVal2);

            _mm256_storeu2_m128i((__m128i*)dst, (__m128i*)(dst + i_dst), mVal1);

            src += i_src2;
            dst += 2 * i_dst;
        }
    }
}

void com_if_filter_ver_4_w32_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row;
    const int offset = 32;
    const int shift = 6;
    int bsym = (coeff[1] == coeff[2]);
    __m256i mAddOffset = _mm256_set1_epi16((short)offset);
    const int i_src2 = i_src * 2;
    const int i_src3 = i_src * 3;

    src -= i_src;

    if (bsym)
    {
        __m256i coeff0 = _mm256_set1_epi8(coeff[0]);
        __m256i coeff1 = _mm256_set1_epi8(coeff[1]);
        __m256i S0, S1, S2, S3;
        __m256i T0, T1, T2, T3, mVal1, mVal2;
        for (row = 0; row < height; row++)
        {
            S0 = _mm256_loadu_si256((__m256i*)(src));
            S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
            S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
            S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));

            T0 = _mm256_unpacklo_epi8(S0, S3);
            T1 = _mm256_unpacklo_epi8(S1, S2);
            T2 = _mm256_unpackhi_epi8(S0, S3);
            T3 = _mm256_unpackhi_epi8(S1, S2);

            T0 = _mm256_maddubs_epi16(T0, coeff0);
            T1 = _mm256_maddubs_epi16(T1, coeff1);
            T2 = _mm256_maddubs_epi16(T2, coeff0);
            T3 = _mm256_maddubs_epi16(T3, coeff1);
            mVal1 = _mm256_add_epi16(T0, T1);
            mVal2 = _mm256_add_epi16(T2, T3);

            mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi16(mVal1, shift);
            mVal2 = _mm256_srai_epi16(mVal2, shift);
            mVal1 = _mm256_packus_epi16(mVal1, mVal2);
            _mm256_storeu_si256((__m256i*)(dst), mVal1);
            src += i_src;
            dst += i_dst;
        }
    }
    else
    {
        __m256i coeff0 = _mm256_set1_epi16(*(i16s_t*)coeff);
        __m256i coeff1 = _mm256_set1_epi16(*(i16s_t*)(coeff + 2));
        __m256i S0, S1, S2, S3;
        __m256i T0, T1, T2, T3, mVal1, mVal2;
        for (row = 0; row < height; row++)
        {
            S0 = _mm256_loadu_si256((__m256i*)(src));
            S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
            S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
            S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));

            T0 = _mm256_unpacklo_epi8(S0, S1);
            T1 = _mm256_unpacklo_epi8(S2, S3);
            T2 = _mm256_unpackhi_epi8(S0, S1);
            T3 = _mm256_unpackhi_epi8(S2, S3);

            T0 = _mm256_maddubs_epi16(T0, coeff0);
            T1 = _mm256_maddubs_epi16(T1, coeff1);
            T2 = _mm256_maddubs_epi16(T2, coeff0);
            T3 = _mm256_maddubs_epi16(T3, coeff1);
            mVal1 = _mm256_add_epi16(T0, T1);
            mVal2 = _mm256_add_epi16(T2, T3);

            mVal1 = _mm256_add_epi16(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi16(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi16(mVal1, shift);
            mVal2 = _mm256_srai_epi16(mVal2, shift);
            mVal1 = _mm256_packus_epi16(mVal1, mVal2);
            _mm256_storeu_si256((__m256i*)(dst), mVal1);

            src += i_src;
            dst += i_dst;
        }
    }
}

void com_if_filter_hor_ver_4_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth)
{
    int row, col;
    int shift;
    i16s_t const *p;
 
    int b_sym_y = (coef_y[1] == coef_y[6]);

    ALIGNED_16(i16s_t tmp_res[(32 + 3) * 32]);
    i16s_t *tmp = tmp_res;
    const int i_tmp = 32;

    __m128i mAddOffset;

    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5,  6);
    __m128i mSwitch2 = _mm_setr_epi8(4, 5, 6, 7, 5, 6, 7, 8, 6, 7, 8, 9, 7, 8, 9, 10);

    __m128i mCoefx = _mm_set1_epi32(*(i32s_t*)coef_x);

    __m128i mask = _mm_loadu_si128((__m128i*)(intrinsic_mask[(width & 7) - 1]));

    // HOR
    src = src - 1 * i_src - 1;
    
    if (width > 4) {
        for (row = -1; row < height + 2; row++) {
            __m128i mT0, mT1, mVal;
            for (col = 0; col < width; col += 8) {
                __m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
                mT0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoefx);
                mT1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoefx);

                mVal = _mm_hadd_epi16(mT0, mT1);
                _mm_store_si128((__m128i*)&tmp[col], mVal);
            }
            src += i_src;
            tmp += i_tmp;
        }
    } else {
        for (row = -1; row < height + 2; row++) {
            __m128i mSrc = _mm_loadu_si128((__m128i*)src);
            __m128i mT0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoefx);
            __m128i mVal = _mm_hadd_epi16(mT0, mT0);
            _mm_storel_epi64((__m128i*)tmp, mVal);
            src += i_src;
            tmp += i_tmp;
        }
    }
    

    // VER
    shift = 12;
    mAddOffset = _mm_set1_epi32(1 << 11);

    tmp = tmp_res;
    if (b_sym_y) {
        __m128i mCoefy1 = _mm_set1_epi16(coef_y[0]);
        __m128i mCoefy2 = _mm_set1_epi16(coef_y[1]);
        __m128i mVal1, mVal2, mVal;

		for (row = 0; row < height; row++) {
			p = tmp;
			for (col = 0; col < width - 7; col += 8) {
				__m128i T00 = _mm_loadu_si128((__m128i*)(p));
				__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_tmp));
				__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_tmp));
				__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_tmp));

				__m128i M0 = _mm_unpacklo_epi16(T00, T30);
				__m128i M1 = _mm_unpacklo_epi16(T10, T20);
				__m128i M2 = _mm_unpackhi_epi16(T00, T30);
				__m128i M3 = _mm_unpackhi_epi16(T10, T20);

				M0 = _mm_madd_epi16(M0, mCoefy1);
				M1 = _mm_madd_epi16(M1, mCoefy2);
				M2 = _mm_madd_epi16(M2, mCoefy1);
				M3 = _mm_madd_epi16(M3, mCoefy2);

				mVal1 = _mm_add_epi32(M0, M1);
				mVal2 = _mm_add_epi32(M2, M3);

				mVal1 = _mm_add_epi32(mVal1, mAddOffset);
				mVal2 = _mm_add_epi32(mVal2, mAddOffset);
				mVal1 = _mm_srai_epi32(mVal1, shift);
				mVal2 = _mm_srai_epi32(mVal2, shift);
				mVal = _mm_packs_epi32(mVal1, mVal2);
				mVal = _mm_packus_epi16(mVal, mVal);

				_mm_storel_epi64((__m128i*)&dst[col], mVal);

				p += 8;
			}

			if (col < width) { // store either 1, 2, 3, 4, 5, 6, or 7 8-bit results in dst
				__m128i T00 = _mm_loadu_si128((__m128i*)(p));
				__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_tmp));
				__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_tmp));
				__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_tmp));

				__m128i M0 = _mm_unpacklo_epi16(T00, T30);
				__m128i M1 = _mm_unpacklo_epi16(T10, T20);
				__m128i M2 = _mm_unpackhi_epi16(T00, T30);
				__m128i M3 = _mm_unpackhi_epi16(T10, T20);

				M0 = _mm_madd_epi16(M0, mCoefy1);
				M1 = _mm_madd_epi16(M1, mCoefy2);
				M2 = _mm_madd_epi16(M2, mCoefy1);
				M3 = _mm_madd_epi16(M3, mCoefy2);

				mVal1 = _mm_add_epi32(M0, M1);
				mVal2 = _mm_add_epi32(M2, M3);

				mVal1 = _mm_add_epi32(mVal1, mAddOffset);
				mVal2 = _mm_add_epi32(mVal2, mAddOffset);
				mVal1 = _mm_srai_epi32(mVal1, shift);
				mVal2 = _mm_srai_epi32(mVal2, shift);
				mVal = _mm_packs_epi32(mVal1, mVal2);
				mVal = _mm_packus_epi16(mVal, mVal);

				_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
			}

			tmp += i_tmp;
			dst += i_dst;
		}
    } else {
        __m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coef_y);
        __m128i coeff1 = _mm_set1_epi16(*(i16s_t*)(coef_y + 2));
		__m128i mVal1, mVal2, mVal;
		coeff0 = _mm_cvtepi8_epi16(coeff0);
		coeff1 = _mm_cvtepi8_epi16(coeff1);
		
		for (row = 0; row < height; row++) {
			p = tmp;
			for (col = 0; col < width - 7; col += 8) {
				__m128i T00 = _mm_loadu_si128((__m128i*)(p));
				__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_tmp));
				__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_tmp));
				__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_tmp));

				__m128i M0 = _mm_unpacklo_epi16(T00, T10);
				__m128i M1 = _mm_unpacklo_epi16(T20, T30);
				__m128i M2 = _mm_unpackhi_epi16(T00, T10);
				__m128i M3 = _mm_unpackhi_epi16(T20, T30);

				M0 = _mm_madd_epi16(M0, coeff0);
				M1 = _mm_madd_epi16(M1, coeff1);
				M2 = _mm_madd_epi16(M2, coeff0);
				M3 = _mm_madd_epi16(M3, coeff1);

				mVal1 = _mm_add_epi32(M0, M1);
				mVal2 = _mm_add_epi32(M2, M3);

				mVal1 = _mm_add_epi32(mVal1, mAddOffset);
				mVal2 = _mm_add_epi32(mVal2, mAddOffset);
				mVal1 = _mm_srai_epi32(mVal1, shift);
				mVal2 = _mm_srai_epi32(mVal2, shift);
				mVal = _mm_packs_epi32(mVal1, mVal2);
				mVal = _mm_packus_epi16(mVal, mVal);

				_mm_storel_epi64((__m128i*)&dst[col], mVal);

				p += 8;
			}

			if (col < width) { // store either 1, 2, 3, 4, 5, 6, or 7 8-bit results in dst
				__m128i T00 = _mm_loadu_si128((__m128i*)(p));
				__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_tmp));
				__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_tmp));
				__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_tmp));

				__m128i M0 = _mm_unpacklo_epi16(T00, T10);
				__m128i M1 = _mm_unpacklo_epi16(T20, T30);
				__m128i M2 = _mm_unpackhi_epi16(T00, T10);
				__m128i M3 = _mm_unpackhi_epi16(T20, T30);

				M0 = _mm_madd_epi16(M0, coeff0);
				M1 = _mm_madd_epi16(M1, coeff1);
				M2 = _mm_madd_epi16(M2, coeff0);
				M3 = _mm_madd_epi16(M3, coeff1);

				mVal1 = _mm_add_epi32(M0, M1);
				mVal2 = _mm_add_epi32(M2, M3);

				mVal1 = _mm_add_epi32(mVal1, mAddOffset);
				mVal2 = _mm_add_epi32(mVal2, mAddOffset);
				mVal1 = _mm_srai_epi32(mVal1, shift);
				mVal2 = _mm_srai_epi32(mVal2, shift);
				mVal = _mm_packs_epi32(mVal1, mVal2);
				mVal = _mm_packus_epi16(mVal, mVal);

				_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
			}

			tmp += i_tmp;
			dst += i_dst;
		}
	}
}

void com_if_filter_hor_ver_4_w16_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth)
{
    ALIGNED_32(i16s_t tmp_res[(32 + 3) * 32]);
    i16s_t *tmp = tmp_res;
    const int i_tmp = 32;
    const int i_tmp2 = 64;
    const int i_tmp3 = 96;

    int row, col;
    int bsymy = (coef_y[1] == coef_y[6]);
    int shift = 12;
    __m256i mAddOffset = _mm256_set1_epi32(1 << 11);
    __m256i mCoef = _mm256_set1_epi32(*(i32s_t*)coef_x);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6, 0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 5, 6, 7, 8, 6, 7, 8, 9, 7, 8, 9, 10, 4, 5, 6, 7, 5, 6, 7, 8, 6, 7, 8, 9, 7, 8, 9, 10);
    __m256i T0, T1, S, S0, R1, R2, sum;

    //HOR
    src = src - i_src - 1;

    for (row = -1; row < height + 2; row++) {
        for (col = 0; col < width; col += 16)  {
            S = _mm256_loadu_si256((__m256i*)(src + col));
            S0 = _mm256_permute4x64_epi64(S, 0x94);
            R1 = _mm256_shuffle_epi8(S0, mSwitch1);
            R2 = _mm256_shuffle_epi8(S0, mSwitch2);
            T0 = _mm256_maddubs_epi16(R1, mCoef);
            T1 = _mm256_maddubs_epi16(R2, mCoef);
            sum = _mm256_hadd_epi16(T0, T1);
            _mm256_store_si256((__m256i*)(tmp + col), sum);
        }
        src += i_src;
        tmp += i_tmp;
    }

    // VER
    tmp = tmp_res;
    if (bsymy)
    {
        __m256i mCoefy1 = _mm256_set1_epi16(coef_y[0]);
        __m256i mCoefy2 = _mm256_set1_epi16(coef_y[1]);
        __m256i mVal1, mVal2, mVal;
        __m256i T2, T3, S1, S2, S3;


        for (row = 0; row < height; row++)
        {
            for (col = 0; col < width; col += 16)
            {
                S0 = _mm256_load_si256((__m256i*)(tmp + col));
                S1 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp));
                S2 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp2));
                S3 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp3));

                T0 = _mm256_unpacklo_epi16(S0, S3);
                T1 = _mm256_unpacklo_epi16(S1, S2);
                T2 = _mm256_unpackhi_epi16(S0, S3);
                T3 = _mm256_unpackhi_epi16(S1, S2);

                T0 = _mm256_madd_epi16(T0, mCoefy1);
                T1 = _mm256_madd_epi16(T1, mCoefy2);
                T2 = _mm256_madd_epi16(T2, mCoefy1);
                T3 = _mm256_madd_epi16(T3, mCoefy2);

                mVal1 = _mm256_add_epi32(T0, T1);
                mVal2 = _mm256_add_epi32(T2, T3);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                mVal = _mm256_packus_epi16(mVal, mVal);

                mVal = _mm256_permute4x64_epi64(mVal, 0xd8);
                _mm_storeu_si128((__m128i*)(dst + col), _mm256_castsi256_si128(mVal));
            }
            tmp += i_tmp;
            dst += i_dst;
        }
    }
    else
    {
        __m128i mCoefy11 = _mm_set1_epi16(*(i16s_t*)coef_y);
        __m128i mCoefy22 = _mm_set1_epi16(*(i16s_t*)(coef_y + 2));
        __m256i mVal1, mVal2, mVal;
        __m256i T2, T3, S1, S2, S3;

        __m256i mCoefy1 = _mm256_cvtepi8_epi16(mCoefy11);
        __m256i mCoefy2 = _mm256_cvtepi8_epi16(mCoefy22);

        for (row = 0; row < height; row++)
        {
            for (col = 0; col < width; col += 16)
            {
                S0 = _mm256_load_si256((__m256i*)(tmp + col));
                S1 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp));
                S2 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp2));
                S3 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp3));

                T0 = _mm256_unpacklo_epi16(S0, S1);
                T1 = _mm256_unpacklo_epi16(S2, S3);
                T2 = _mm256_unpackhi_epi16(S0, S1);
                T3 = _mm256_unpackhi_epi16(S2, S3);

                T0 = _mm256_madd_epi16(T0, mCoefy1);
                T1 = _mm256_madd_epi16(T1, mCoefy2);
                T2 = _mm256_madd_epi16(T2, mCoefy1);
                T3 = _mm256_madd_epi16(T3, mCoefy2);

                mVal1 = _mm256_add_epi32(T0, T1);
                mVal2 = _mm256_add_epi32(T2, T3);

                mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
                mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
                mVal1 = _mm256_srai_epi32(mVal1, shift);
                mVal2 = _mm256_srai_epi32(mVal2, shift);

                mVal = _mm256_packs_epi32(mVal1, mVal2);
                mVal = _mm256_packus_epi16(mVal, mVal);

                mVal = _mm256_permute4x64_epi64(mVal, 0xd8);
                _mm_storeu_si128((__m128i*)(dst + col), _mm256_castsi256_si128(mVal));
            }
            tmp += i_tmp;
            dst += i_dst;
        }
    }
}

void com_if_filter_hor_8_sse128(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
	int i, j;
	pel_t *d0 = dst[0];
	pel_t *d1 = dst[1];
	pel_t *d2 = dst[2];
    i16s_t *dt0 = dst_tmp[0];
    i16s_t *dt1 = dst_tmp[1];
    i16s_t *dt2 = dst_tmp[2];

	__m128i mSwitch1 = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 8);
	__m128i mSwitch2 = _mm_setr_epi8(2, 3, 4, 5, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9, 10);
	__m128i mSwitch3 = _mm_setr_epi8(4, 5, 6, 7, 8, 9, 10, 11, 5, 6, 7, 8, 9, 10, 11, 12);
	__m128i mSwitch4 = _mm_setr_epi8(6, 7, 8, 9, 10, 11, 12, 13, 7, 8, 9, 10, 11, 12, 13, 14);

	__m128i mCoef0 = _mm_loadl_epi64((__m128i*)coeff[0]);
	__m128i mCoef1 = _mm_loadl_epi64((__m128i*)coeff[1]);
	__m128i mCoef2 = _mm_loadl_epi64((__m128i*)coeff[2]);
    __m128i T20, T40, T60, T80, s1, s2, sum, sum_pel;
    __m128i S20, S40, S60, S80;
    __m128i mAddOffset = _mm_set1_epi16(32);

	mCoef0 = _mm_unpacklo_epi64(mCoef0, mCoef0);
	mCoef1 = _mm_unpacklo_epi64(mCoef1, mCoef1);
	mCoef2 = _mm_unpacklo_epi64(mCoef2, mCoef2);

	src -= 3;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i += 8) {
			__m128i srcCoeff = _mm_loadu_si128((__m128i*)(src + i));

			S20 = _mm_shuffle_epi8(srcCoeff, mSwitch1);
			S40 = _mm_shuffle_epi8(srcCoeff, mSwitch2);
			S60 = _mm_shuffle_epi8(srcCoeff, mSwitch3);
			S80 = _mm_shuffle_epi8(srcCoeff, mSwitch4);

			T20 = _mm_maddubs_epi16(S20, mCoef0);
			T40 = _mm_maddubs_epi16(S40, mCoef0);
			T60 = _mm_maddubs_epi16(S60, mCoef0);
			T80 = _mm_maddubs_epi16(S80, mCoef0);

			s1 = _mm_hadd_epi16(T20, T40);
			s2 = _mm_hadd_epi16(T60, T80);
			sum = _mm_hadd_epi16(s1, s2);
            sum_pel = _mm_add_epi16(sum, mAddOffset);
            sum_pel = _mm_srai_epi16(sum_pel, 6);
            sum_pel = _mm_packus_epi16(sum_pel, sum_pel);
			_mm_storeu_si128((__m128i*)&dt0[i], sum);
            _mm_storel_epi64((__m128i*)&d0[i], sum_pel);

			T20 = _mm_maddubs_epi16(S20, mCoef1);
			T40 = _mm_maddubs_epi16(S40, mCoef1);
			T60 = _mm_maddubs_epi16(S60, mCoef1);
			T80 = _mm_maddubs_epi16(S80, mCoef1);

			s1 = _mm_hadd_epi16(T20, T40);
			s2 = _mm_hadd_epi16(T60, T80);
			sum = _mm_hadd_epi16(s1, s2);
            sum_pel = _mm_add_epi16(sum, mAddOffset);
            sum_pel = _mm_srai_epi16(sum_pel, 6);
            sum_pel = _mm_packus_epi16(sum_pel, sum_pel);
			_mm_storeu_si128((__m128i*)&dt1[i], sum);
            _mm_storel_epi64((__m128i*)&d1[i], sum_pel);

			T20 = _mm_maddubs_epi16(S20, mCoef2);
			T40 = _mm_maddubs_epi16(S40, mCoef2);
			T60 = _mm_maddubs_epi16(S60, mCoef2);
			T80 = _mm_maddubs_epi16(S80, mCoef2);

			s1 = _mm_hadd_epi16(T20, T40);
			s2 = _mm_hadd_epi16(T60, T80);
			sum = _mm_hadd_epi16(s1, s2);
            sum_pel = _mm_add_epi16(sum, mAddOffset);
            sum_pel = _mm_srai_epi16(sum_pel, 6);
            sum_pel = _mm_packus_epi16(sum_pel, sum_pel);
			_mm_storeu_si128((__m128i*)&dt2[i], sum);
            _mm_storel_epi64((__m128i*)&d2[i], sum_pel);
		}
		d0  += i_dst;
		d1  += i_dst;
		d2  += i_dst;
        dt0 += i_dst_tmp;
        dt1 += i_dst_tmp;
        dt2 += i_dst_tmp;
		src += i_src;
	}
}

void com_if_filter_hor_8_sse256(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    i16s_t *dt0 = dst_tmp[0];
    i16s_t *dt1 = dst_tmp[1];
    i16s_t *dt2 = dst_tmp[2];

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

void com_if_filter_ver_8_sse128(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
	int i, j;
	pel_t *d0 = dst[0];
	pel_t *d1 = dst[1];
	pel_t *d2 = dst[2];
	__m128i S00, S10, S20, S30;
	__m128i coeff00 = _mm_set1_epi16(*(i16s_t*)coeff[0]);
	__m128i coeff01 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 2));
	__m128i coeff02 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 4));
	__m128i coeff03 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 6));

	__m128i coeff10 = _mm_set1_epi16(*(i16s_t*)coeff[1]);
	__m128i coeff11 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 2));
	__m128i coeff12 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 4));
	__m128i coeff13 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 6));

	__m128i coeff20 = _mm_set1_epi16(*(i16s_t*)coeff[2]);
	__m128i coeff21 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 2));
	__m128i coeff22 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 4));
	__m128i coeff23 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 6));

	__m128i mAddOffset = _mm_set1_epi16(32);
	__m128i mVal;
	src -= 3 * i_src;

	for (j = 0; j < height; j++) 
	{
		const pel_t *p = src;
		for (i = 0; i < width; i += 8) 
		{
			__m128i T00 = _mm_loadl_epi64((__m128i*)(p));
            __m128i T10 = _mm_loadl_epi64((__m128i*)(p + i_src));
            __m128i T20 = _mm_loadl_epi64((__m128i*)(p + 2 * i_src));
            __m128i T30 = _mm_loadl_epi64((__m128i*)(p + 3 * i_src));
            __m128i T40 = _mm_loadl_epi64((__m128i*)(p + 4 * i_src));
            __m128i T50 = _mm_loadl_epi64((__m128i*)(p + 5 * i_src));
            __m128i T60 = _mm_loadl_epi64((__m128i*)(p + 6 * i_src));
            __m128i T70 = _mm_loadl_epi64((__m128i*)(p + 7 * i_src));

			S00 = _mm_unpacklo_epi8(T00, T10);
			S10 = _mm_unpacklo_epi8(T20, T30);
			S20 = _mm_unpacklo_epi8(T40, T50);
			S30 = _mm_unpacklo_epi8(T60, T70);

			T00 = _mm_maddubs_epi16(S00, coeff00);
			T10 = _mm_maddubs_epi16(S10, coeff01);
			T20 = _mm_maddubs_epi16(S20, coeff02);
			T30 = _mm_maddubs_epi16(S30, coeff03);

			mVal = _mm_add_epi16(T00, T10);
			mVal = _mm_add_epi16(mVal, T20);
			mVal = _mm_add_epi16(mVal, T30);

			mVal = _mm_add_epi16(mVal, mAddOffset);
			mVal = _mm_srai_epi16(mVal, 6);
			mVal = _mm_packus_epi16(mVal, mVal);

			_mm_storel_epi64((__m128i*)&d0[i], mVal);

			T00 = _mm_maddubs_epi16(S00, coeff10);
			T10 = _mm_maddubs_epi16(S10, coeff11);
			T20 = _mm_maddubs_epi16(S20, coeff12);
			T30 = _mm_maddubs_epi16(S30, coeff13);

			mVal = _mm_add_epi16(T00, T10);
			mVal = _mm_add_epi16(mVal, T20);
			mVal = _mm_add_epi16(mVal, T30);

			mVal = _mm_add_epi16(mVal, mAddOffset);
			mVal = _mm_srai_epi16(mVal, 6);
			mVal = _mm_packus_epi16(mVal, mVal);

			_mm_storel_epi64((__m128i*)&d1[i], mVal);

			T00 = _mm_maddubs_epi16(S00, coeff20);
			T10 = _mm_maddubs_epi16(S10, coeff21);
			T20 = _mm_maddubs_epi16(S20, coeff22);
			T30 = _mm_maddubs_epi16(S30, coeff23);

			mVal = _mm_add_epi16(T00, T10);
			mVal = _mm_add_epi16(mVal, T20);
			mVal = _mm_add_epi16(mVal, T30);

			mVal = _mm_add_epi16(mVal, mAddOffset);
			mVal = _mm_srai_epi16(mVal, 6);
			mVal = _mm_packus_epi16(mVal, mVal);

			_mm_storel_epi64((__m128i*)&d2[i], mVal);

			p += 8;
		}

		d0 += i_dst;
		d1 += i_dst;
		d2 += i_dst;
		src += i_src;
	}

}

void com_if_filter_ver_8_sse256(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    __m256i S00, S10, S20, S30;
    __m256i coeff00 = _mm256_set1_epi16(*(i16s_t*)coeff[0]);
    __m256i coeff01 = _mm256_set1_epi16(*(i16s_t*)(coeff[0] + 2));
    __m256i coeff02 = _mm256_set1_epi16(*(i16s_t*)(coeff[0] + 4));
    __m256i coeff03 = _mm256_set1_epi16(*(i16s_t*)(coeff[0] + 6));

    __m256i coeff10 = _mm256_set1_epi16(*(i16s_t*)coeff[1]);
    __m256i coeff11 = _mm256_set1_epi16(*(i16s_t*)(coeff[1] + 2));
    __m256i coeff12 = _mm256_set1_epi16(*(i16s_t*)(coeff[1] + 4));
    __m256i coeff13 = _mm256_set1_epi16(*(i16s_t*)(coeff[1] + 6));

    __m256i coeff20 = _mm256_set1_epi16(*(i16s_t*)coeff[2]);
    __m256i coeff21 = _mm256_set1_epi16(*(i16s_t*)(coeff[2] + 2));
    __m256i coeff22 = _mm256_set1_epi16(*(i16s_t*)(coeff[2] + 4));
    __m256i coeff23 = _mm256_set1_epi16(*(i16s_t*)(coeff[2] + 6));

    __m256i mAddOffset = _mm256_set1_epi16(32);
    __m256i mVal;

    src -= 3 * i_src;

    for (j = 0; j < height; j++) {
        const pel_t *p = src;
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

void com_if_filter_ver_8_ext_sse128(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
	int i, j;
	pel_t *d0 = dst[0];
	pel_t *d1 = dst[1];
	pel_t *d2 = dst[2];

	__m128i mAddOffset = _mm_set1_epi32(2048);

	__m128i coeff00 = _mm_set1_epi16(*(i16s_t*)coeff[0]);
	__m128i coeff01 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 2));
	__m128i coeff02 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 4));
	__m128i coeff03 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 6));

	__m128i coeff10 = _mm_set1_epi16(*(i16s_t*)coeff[1]);
	__m128i coeff11 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 2));
	__m128i coeff12 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 4));
	__m128i coeff13 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 6));

	__m128i coeff20 = _mm_set1_epi16(*(i16s_t*)coeff[2]);
	__m128i coeff21 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 2));
	__m128i coeff22 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 4));
	__m128i coeff23 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 6));
	__m128i mVal1, mVal2, mVal;
	coeff00 = _mm_cvtepi8_epi16(coeff00);
	coeff01 = _mm_cvtepi8_epi16(coeff01);
	coeff02 = _mm_cvtepi8_epi16(coeff02);
	coeff03 = _mm_cvtepi8_epi16(coeff03);

	coeff10 = _mm_cvtepi8_epi16(coeff10);
	coeff11 = _mm_cvtepi8_epi16(coeff11);
	coeff12 = _mm_cvtepi8_epi16(coeff12);
	coeff13 = _mm_cvtepi8_epi16(coeff13);

	coeff20 = _mm_cvtepi8_epi16(coeff20);
	coeff21 = _mm_cvtepi8_epi16(coeff21);
	coeff22 = _mm_cvtepi8_epi16(coeff22);
	coeff23 = _mm_cvtepi8_epi16(coeff23);

	src -= 3 * i_src;

	for (j = 0; j < height; j++) 
	{
		const i16s_t *p = src;
		for (i = 0; i < width; i += 8) 
		{
			__m128i T00 = _mm_loadu_si128((__m128i*)(p));
			__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
			__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
			__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));
			__m128i T40 = _mm_loadu_si128((__m128i*)(p + 4 * i_src));
			__m128i T50 = _mm_loadu_si128((__m128i*)(p + 5 * i_src));
			__m128i T60 = _mm_loadu_si128((__m128i*)(p + 6 * i_src));
			__m128i T70 = _mm_loadu_si128((__m128i*)(p + 7 * i_src));

			__m128i S0 = _mm_unpacklo_epi16(T00, T10);
			__m128i S1 = _mm_unpacklo_epi16(T20, T30);
			__m128i S2 = _mm_unpacklo_epi16(T40, T50);
			__m128i S3 = _mm_unpacklo_epi16(T60, T70);
			__m128i S4 = _mm_unpackhi_epi16(T00, T10);
			__m128i S5 = _mm_unpackhi_epi16(T20, T30);
			__m128i S6 = _mm_unpackhi_epi16(T40, T50);
			__m128i S7 = _mm_unpackhi_epi16(T60, T70);

			__m128i T0 = _mm_madd_epi16(S0, coeff00);
			__m128i T1 = _mm_madd_epi16(S1, coeff01);
			__m128i T2 = _mm_madd_epi16(S2, coeff02);
			__m128i T3 = _mm_madd_epi16(S3, coeff03);
			__m128i T4 = _mm_madd_epi16(S4, coeff00);
			__m128i T5 = _mm_madd_epi16(S5, coeff01);
			__m128i T6 = _mm_madd_epi16(S6, coeff02);
			__m128i T7 = _mm_madd_epi16(S7, coeff03);

			mVal1 = _mm_add_epi32(T0, T1);
			mVal1 = _mm_add_epi32(mVal1, T2);
			mVal1 = _mm_add_epi32(mVal1, T3);

			mVal2 = _mm_add_epi32(T4, T5);
			mVal2 = _mm_add_epi32(mVal2, T6);
			mVal2 = _mm_add_epi32(mVal2, T7);

			mVal1 = _mm_add_epi32(mVal1, mAddOffset);
			mVal2 = _mm_add_epi32(mVal2, mAddOffset);
			mVal1 = _mm_srai_epi32(mVal1, 12);
			mVal2 = _mm_srai_epi32(mVal2, 12);
			mVal = _mm_packs_epi32(mVal1, mVal2);
			mVal = _mm_packus_epi16(mVal, mVal);

			_mm_storel_epi64((__m128i*)&d0[i], mVal);

			T0 = _mm_madd_epi16(S0, coeff10);
			T1 = _mm_madd_epi16(S1, coeff11);
			T2 = _mm_madd_epi16(S2, coeff12);
			T3 = _mm_madd_epi16(S3, coeff13);
			T4 = _mm_madd_epi16(S4, coeff10);
			T5 = _mm_madd_epi16(S5, coeff11);
			T6 = _mm_madd_epi16(S6, coeff12);
			T7 = _mm_madd_epi16(S7, coeff13);

			mVal1 = _mm_add_epi32(T0, T1);
			mVal1 = _mm_add_epi32(mVal1, T2);
			mVal1 = _mm_add_epi32(mVal1, T3);

			mVal2 = _mm_add_epi32(T4, T5);
			mVal2 = _mm_add_epi32(mVal2, T6);
			mVal2 = _mm_add_epi32(mVal2, T7);

			mVal1 = _mm_add_epi32(mVal1, mAddOffset);
			mVal2 = _mm_add_epi32(mVal2, mAddOffset);
			mVal1 = _mm_srai_epi32(mVal1, 12);
			mVal2 = _mm_srai_epi32(mVal2, 12);
			mVal = _mm_packs_epi32(mVal1, mVal2);
			mVal = _mm_packus_epi16(mVal, mVal);

			_mm_storel_epi64((__m128i*)&d1[i], mVal);

			T0 = _mm_madd_epi16(S0, coeff20);
			T1 = _mm_madd_epi16(S1, coeff21);
			T2 = _mm_madd_epi16(S2, coeff22);
			T3 = _mm_madd_epi16(S3, coeff23);
			T4 = _mm_madd_epi16(S4, coeff20);
			T5 = _mm_madd_epi16(S5, coeff21);
			T6 = _mm_madd_epi16(S6, coeff22);
			T7 = _mm_madd_epi16(S7, coeff23);

			mVal1 = _mm_add_epi32(T0, T1);
			mVal1 = _mm_add_epi32(mVal1, T2);
			mVal1 = _mm_add_epi32(mVal1, T3);

			mVal2 = _mm_add_epi32(T4, T5);
			mVal2 = _mm_add_epi32(mVal2, T6);
			mVal2 = _mm_add_epi32(mVal2, T7);

			mVal1 = _mm_add_epi32(mVal1, mAddOffset);
			mVal2 = _mm_add_epi32(mVal2, mAddOffset);
			mVal1 = _mm_srai_epi32(mVal1, 12);
			mVal2 = _mm_srai_epi32(mVal2, 12);
			mVal = _mm_packs_epi32(mVal1, mVal2);
			mVal = _mm_packus_epi16(mVal, mVal);

			_mm_storel_epi64((__m128i*)&d2[i], mVal);

			p += 8;
		}

		d0 += i_dst;
		d1 += i_dst;
		d2 += i_dst;
		src += i_src;
	}
}

void com_if_filter_ver_8_ext_sse256(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];

    __m256i mAddOffset = _mm256_set1_epi32(2048);

    __m128i c00 = _mm_set1_epi16(*(i16s_t*)coeff[0]);
    __m128i c01 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 2));
    __m128i c02 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 4));
    __m128i c03 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 6));

    __m128i c10 = _mm_set1_epi16(*(i16s_t*)coeff[1]);
    __m128i c11 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 2));
    __m128i c12 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 4));
    __m128i c13 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 6));

    __m128i c20 = _mm_set1_epi16(*(i16s_t*)coeff[2]);
    __m128i c21 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 2));
    __m128i c22 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 4));
    __m128i c23 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 6));
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
        const i16s_t *p = src;
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


/* 10bit compiler */

void com_if_filter_hor_4_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
	int row, col;
	const i16s_t offset = 32;
	const int shift = 6;
	int max_pixel = (1 << bit_depth) - 1;

	__m128i mAddOffset = _mm_set1_epi32(offset);

	__m128i mSwitch1 = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 8, 9);
	__m128i mSwitch2 = _mm_setr_epi8(4, 5, 6, 7, 8, 9, 10, 11, 6, 7, 8, 9, 10, 11, 12, 13);
	__m128i mCoef = _mm_set1_epi32(*(i32s_t*)coeff);
	__m128i max_val = _mm_set1_epi16((pel_t)max_pixel);

	mCoef = _mm_cvtepi8_epi16(mCoef);
	src -= 1;

	if (!(width & 7)){
		for (row = 0; row < height; row++) {
			__m128i mT20, mT40, mVal, mT10, mT30;

			for (col = 0; col < width; col += 8) {
				__m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
				mT20 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
				mT40 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);

				mSrc = _mm_loadu_si128((__m128i*)(src + col + 4));
				mT10 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
				mT30 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);

				mT20 = _mm_hadd_epi32(mT20, mT40);
				mT10 = _mm_hadd_epi32(mT10, mT30);

				mT20 = _mm_add_epi32(mT20, mAddOffset);
				mT10 = _mm_add_epi32(mT10, mAddOffset);
				mT20 = _mm_srai_epi32(mT20, shift);
				mT10 = _mm_srai_epi32(mT10, shift);

				mVal = _mm_packus_epi32(mT20, mT10);
				mVal = _mm_min_epu16(mVal, max_val);
				_mm_storeu_si128((__m128i*)&dst[col], mVal);
			}

			src += i_src;
			dst += i_dst;
		}
	}
	else{
		__m128i mask = _mm_loadu_si128((__m128i*)(intrinsic_mask_10bit[(width & 7) - 1]));
		for (row = 0; row < height; row++) {
			__m128i mT20, mT40, mVal, mT10, mT30;

			for (col = 0; col < width - 7; col += 8) {
				__m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
				mT20 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
				mT40 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);

				mSrc = _mm_loadu_si128((__m128i*)(src + col + 4));
				mT10 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
				mT30 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);

				mT20 = _mm_hadd_epi32(mT20, mT40);
				mT10 = _mm_hadd_epi32(mT10, mT30);

				mT20 = _mm_add_epi32(mT20, mAddOffset);
				mT10 = _mm_add_epi32(mT10, mAddOffset);
				mT20 = _mm_srai_epi32(mT20, shift);
				mT10 = _mm_srai_epi32(mT10, shift);

				mVal = _mm_packus_epi32(mT20, mT10);
				mVal = _mm_min_epu16(mVal, max_val);
				_mm_storeu_si128((__m128i*)&dst[col], mVal);
			}


			if (width - col > 4){
				__m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
				mT20 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
				mT40 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);

				mSrc = _mm_loadu_si128((__m128i*)(src + col + 4));
				mT10 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
				mT30 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);

				mT20 = _mm_hadd_epi32(mT20, mT40);
				mT10 = _mm_hadd_epi32(mT10, mT30);

				mT20 = _mm_add_epi32(mT20, mAddOffset);
				mT10 = _mm_add_epi32(mT10, mAddOffset);
				mT20 = _mm_srai_epi32(mT20, shift);
				mT10 = _mm_srai_epi32(mT10, shift);

				mVal = _mm_packus_epi32(mT20, mT10);
			}
			else
			{
				__m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
				mT20 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef);
				mT40 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef);
				mT20 = _mm_hadd_epi32(mT20, mT40);

				mT20 = _mm_add_epi32(mT20, mAddOffset);
				mT20 = _mm_srai_epi32(mT20, shift);

				mVal = _mm_packus_epi32(mT20, mT20);

			}
			mVal = _mm_min_epu16(mVal, max_val);
			_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);

			src += i_src;
			dst += i_dst;
		}
	}
}

void com_if_filter_hor_4_w8_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row, col;
    const int offset = 32;
    const int shift = 6;

    __m128i mCoef1 = _mm_set1_epi32(*(i32s_t*)coeff);
    __m256i mCoef = _mm256_cvtepi8_epi16(mCoef1);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 8, 9, 10, 11, 6, 7, 8, 9, 10, 11, 12, 13, 4, 5, 6, 7, 8, 9, 10, 11, 6, 7, 8, 9, 10, 11, 12, 13);
    __m256i mAddOffset = _mm256_set1_epi32((i16s_t)offset);
    __m256i T0, T1, S, S0, sum;
    src -= 1;
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel_t)max_pixel);

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col += 8) {
            S = _mm256_loadu_si256((__m256i*)(src + col));
            S0 = _mm256_permute4x64_epi64(S, 0x94);
            T0 = _mm256_madd_epi16(_mm256_shuffle_epi8(S0, mSwitch1), mCoef);
            T1 = _mm256_madd_epi16(_mm256_shuffle_epi8(S0, mSwitch2), mCoef);
            sum = _mm256_hadd_epi32(T0, T1);

            sum = _mm256_add_epi32(sum, mAddOffset);
            sum = _mm256_srai_epi32(sum, shift);
            sum = _mm256_packus_epi32(sum, sum);
            sum = _mm256_permute4x64_epi64(sum, 0xd8);

            sum = _mm256_min_epu16(sum, max_val);
            _mm_storeu_si128((__m128i*)(dst + col), _mm256_castsi256_si128(sum));
        }
        src += i_src;
        dst += i_dst;
    }
}

void com_if_filter_ver_4_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
	int row, col;
	const i16s_t offset = 32;
	const int shift = 6;
	int bsym = (coeff[1] == coeff[2]);
	__m128i mAddOffset = _mm_set1_epi32(offset);
	int max_pixel = (1 << bit_depth) - 1;

	pel_t const *p;
	__m128i max_val = _mm_set1_epi16((pel_t)max_pixel);

	src -= i_src;

	if (bsym) {
		if (!(width & 7)){
			__m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coeff);
			coeff0 = _mm_cvtepi8_epi16(coeff0);
			__m128i mVal;

			for (row = 0; row < height; row++) {
				p = src;
				for (col = 0; col < width; col += 8) {
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					T00 = _mm_add_epi16(T00, T30);
					T20 = _mm_add_epi16(T10, T20);

					T10 = _mm_unpackhi_epi16(T00, T20);
					T00 = _mm_unpacklo_epi16(T00, T20);

					T00 = _mm_madd_epi16(T00, coeff0);
					T10 = _mm_madd_epi16(T10, coeff0);

					T00 = _mm_add_epi32(T00, mAddOffset);
					T00 = _mm_srai_epi32(T00, shift);
					T10 = _mm_add_epi32(T10, mAddOffset);
					T10 = _mm_srai_epi32(T10, shift);

					mVal = _mm_packus_epi32(T00, T10);
					mVal = _mm_min_epu16(mVal, max_val);
					_mm_storeu_si128((__m128i*)&dst[col], mVal);

					p += 8;
				}

				src += i_src;
				dst += i_dst;
			}
		}
		else
		{
			__m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coeff);
			coeff0 = _mm_cvtepi8_epi16(coeff0);
			__m128i mVal;
			__m128i mask = _mm_loadu_si128((__m128i*)(intrinsic_mask_10bit[(width & 7) - 1]));
			for (row = 0; row < height; row++) {
				p = src;
				for (col = 0; col < width - 7; col += 8) {
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					T00 = _mm_add_epi16(T00, T30);
					T20 = _mm_add_epi16(T10, T20);

					T10 = _mm_unpackhi_epi16(T00, T20);
					T00 = _mm_unpacklo_epi16(T00, T20);

					T00 = _mm_madd_epi16(T00, coeff0);
					T10 = _mm_madd_epi16(T10, coeff0);

					T00 = _mm_add_epi32(T00, mAddOffset);
					T00 = _mm_srai_epi32(T00, shift);
					T10 = _mm_add_epi32(T10, mAddOffset);
					T10 = _mm_srai_epi32(T10, shift);

					mVal = _mm_packus_epi32(T00, T10);
					mVal = _mm_min_epu16(mVal, max_val);
					_mm_storeu_si128((__m128i*)&dst[col], mVal);

					p += 8;
				}

				if (width - col > 4) {
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					T00 = _mm_add_epi16(T00, T30);
					T20 = _mm_add_epi16(T10, T20);

					T10 = _mm_unpackhi_epi16(T00, T20);
					T00 = _mm_unpacklo_epi16(T00, T20);

					T00 = _mm_madd_epi16(T00, coeff0);
					T10 = _mm_madd_epi16(T10, coeff0);

					T00 = _mm_add_epi32(T00, mAddOffset);
					T00 = _mm_srai_epi32(T00, shift);
					T10 = _mm_add_epi32(T10, mAddOffset);
					T10 = _mm_srai_epi32(T10, shift);

					mVal = _mm_packus_epi32(T00, T10);
					mVal = _mm_min_epu16(mVal, max_val);
					_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);

				}
				else
				{
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					T00 = _mm_add_epi16(T00, T30);
					T20 = _mm_add_epi16(T10, T20);

					T00 = _mm_unpacklo_epi16(T00, T20);
					T00 = _mm_madd_epi16(T00, coeff0);

					T00 = _mm_add_epi32(T00, mAddOffset);
					T00 = _mm_srai_epi32(T00, shift);

					mVal = _mm_packus_epi32(T00, T00);
					mVal = _mm_min_epu16(mVal, max_val);
					_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
				}

				src += i_src;
				dst += i_dst;
			}
		}
	}
	else {
		if (!(width & 7)){
			__m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coeff);
			__m128i coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff + 2));
			coeff0 = _mm_cvtepi8_epi16(coeff0);
			coeff1 = _mm_cvtepi8_epi16(coeff1);
			__m128i mVal;
			__m128i M0, M1, M2, M3;
			for (row = 0; row < height; row++) {
				p = src;
				for (col = 0; col < width; col += 8) {
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					M0 = _mm_unpacklo_epi16(T00, T10);
					M1 = _mm_unpacklo_epi16(T20, T30);
					M2 = _mm_unpackhi_epi16(T00, T10);
					M3 = _mm_unpackhi_epi16(T20, T30);

					M0 = _mm_madd_epi16(M0, coeff0);
					M1 = _mm_madd_epi16(M1, coeff1);
					M2 = _mm_madd_epi16(M2, coeff0);
					M3 = _mm_madd_epi16(M3, coeff1);

					M0 = _mm_add_epi32(M0, M1);
					M2 = _mm_add_epi32(M2, M3);

					M0 = _mm_add_epi32(M0, mAddOffset);
					M2 = _mm_add_epi32(M2, mAddOffset);
					M0 = _mm_srai_epi32(M0, shift);
					M2 = _mm_srai_epi32(M2, shift);

					mVal = _mm_packus_epi32(M0, M2);
					mVal = _mm_min_epu16(mVal, max_val);
					_mm_storeu_si128((__m128i*)&dst[col], mVal);

					p += 8;
				}

				src += i_src;
				dst += i_dst;

			}
		}
		else{
			__m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coeff);
			__m128i coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff + 2));
			coeff0 = _mm_cvtepi8_epi16(coeff0);
			coeff1 = _mm_cvtepi8_epi16(coeff1);
			__m128i mVal;
			__m128i mask = _mm_loadu_si128((__m128i*)(intrinsic_mask_10bit[(width & 7) - 1]));
			__m128i M0, M1, M2, M3;
			for (row = 0; row < height; row++) {
				p = src;
				for (col = 0; col < width - 7; col += 8) {
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					M0 = _mm_unpacklo_epi16(T00, T10);
					M1 = _mm_unpacklo_epi16(T20, T30);
					M2 = _mm_unpackhi_epi16(T00, T10);
					M3 = _mm_unpackhi_epi16(T20, T30);

					M0 = _mm_madd_epi16(M0, coeff0);
					M1 = _mm_madd_epi16(M1, coeff1);
					M2 = _mm_madd_epi16(M2, coeff0);
					M3 = _mm_madd_epi16(M3, coeff1);

					M0 = _mm_add_epi32(M0, M1);
					M2 = _mm_add_epi32(M2, M3);

					M0 = _mm_add_epi32(M0, mAddOffset);
					M2 = _mm_add_epi32(M2, mAddOffset);
					M0 = _mm_srai_epi32(M0, shift);
					M2 = _mm_srai_epi32(M2, shift);

					mVal = _mm_packus_epi32(M0, M2);
					mVal = _mm_min_epu16(mVal, max_val);
					_mm_storeu_si128((__m128i*)&dst[col], mVal);

					p += 8;
				}

				if (width - col > 4) {
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					M0 = _mm_unpacklo_epi16(T00, T10);
					M1 = _mm_unpacklo_epi16(T20, T30);
					M2 = _mm_unpackhi_epi16(T00, T10);
					M3 = _mm_unpackhi_epi16(T20, T30);

					M0 = _mm_madd_epi16(M0, coeff0);
					M1 = _mm_madd_epi16(M1, coeff1);
					M2 = _mm_madd_epi16(M2, coeff0);
					M3 = _mm_madd_epi16(M3, coeff1);

					M0 = _mm_add_epi32(M0, M1);
					M2 = _mm_add_epi32(M2, M3);

					M0 = _mm_add_epi32(M0, mAddOffset);
					M2 = _mm_add_epi32(M2, mAddOffset);
					M0 = _mm_srai_epi32(M0, shift);
					M2 = _mm_srai_epi32(M2, shift);

					mVal = _mm_packus_epi32(M0, M2);
					mVal = _mm_min_epu16(mVal, max_val);

					_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
				}
				else
				{
					__m128i T00 = _mm_loadu_si128((__m128i*)(p));
					__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_src));
					__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
					__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));

					M0 = _mm_unpacklo_epi16(T00, T10);
					M1 = _mm_unpacklo_epi16(T20, T30);

					M0 = _mm_madd_epi16(M0, coeff0);
					M1 = _mm_madd_epi16(M1, coeff1);

					M0 = _mm_add_epi32(M0, M1);

					M0 = _mm_add_epi32(M0, mAddOffset);
					M0 = _mm_srai_epi32(M0, shift);

					mVal = _mm_packus_epi32(M0, M0);
					mVal = _mm_min_epu16(mVal, max_val);

					_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);
				}

				src += i_src;
				dst += i_dst;

			}
		}
	}
}

void com_if_filter_ver_4_w16_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row;
    const int offset = 32;
    const int shift = 6;
    int bsym = (coeff[1] == coeff[2]);
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    const int i_src2 = i_src << 1;
    const int i_src3 = i_src + i_src2;
    const int i_src4 = i_src << 2;

    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel_t)max_pixel);

    src -= i_src;

    if (bsym) {
        __m128i coeff1 = _mm_set1_epi16(((i16s_t*)coeff)[0]);
        __m256i coeff0 = _mm256_cvtepi8_epi16(coeff1);
        __m256i T0, T1, T2, T3, mVal1, mVal2, mVal3, mVal4;
        __m256i R0, R1, R2, R3;
        for (row = 0; row < height; row += 2) {
            __m256i S0 = _mm256_loadu_si256((__m256i*)(src));
            __m256i S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
            __m256i S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
            __m256i S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));
            __m256i S4 = _mm256_loadu_si256((__m256i*)(src + i_src4));

            R0 = _mm256_add_epi16(S0, S3);
            R1 = _mm256_add_epi16(S1, S2);
            R2 = _mm256_add_epi16(S1, S4);
            R3 = _mm256_add_epi16(S2, S3);

            T0 = _mm256_unpacklo_epi16(R0, R1);
            T1 = _mm256_unpackhi_epi16(R0, R1);
            T2 = _mm256_unpacklo_epi16(R2, R3);
            T3 = _mm256_unpackhi_epi16(R2, R3);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff0);
            T3 = _mm256_madd_epi16(T3, coeff0);

            mVal1 = _mm256_add_epi32(T0, mAddOffset);
            mVal2 = _mm256_add_epi32(T1, mAddOffset);
            mVal3 = _mm256_add_epi32(T2, mAddOffset);
            mVal4 = _mm256_add_epi32(T3, mAddOffset);

            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal3 = _mm256_srai_epi32(mVal3, shift);
            mVal4 = _mm256_srai_epi32(mVal4, shift);

            mVal1 = _mm256_packus_epi32(mVal1, mVal2);
            mVal3 = _mm256_packus_epi32(mVal3, mVal4);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            mVal3 = _mm256_min_epu16(mVal3, max_val);
            _mm256_storeu_si256((__m256i*)dst, mVal1);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), mVal3);

            src += i_src2;
            dst += 2 * i_dst;
        }
    }
    else {
        __m128i coeff00 = _mm_set1_epi16(*(i16s_t*)coeff);
        __m128i coeff11 = _mm_set1_epi16(*(i16s_t*)(coeff + 2));
        __m256i coeff0 = _mm256_cvtepi8_epi16(coeff00);
        __m256i coeff1 = _mm256_cvtepi8_epi16(coeff11);
        __m256i T0, T1, T2, T3, mVal1, mVal2;
        for (row = 0; row < height; row += 2) {
            __m256i S0 = _mm256_loadu_si256((__m256i*)(src));
            __m256i S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
            __m256i S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
            __m256i S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));
            __m256i S4 = _mm256_loadu_si256((__m256i*)(src + i_src4));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpackhi_epi16(S0, S1);
            T2 = _mm256_unpacklo_epi16(S2, S3);
            T3 = _mm256_unpackhi_epi16(S2, S3);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi32(T0, T2);
            mVal2 = _mm256_add_epi32(T1, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal1 = _mm256_packus_epi32(mVal1, mVal2);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            _mm256_storeu_si256((__m256i*)dst, mVal1);

            T0 = _mm256_unpacklo_epi16(S1, S2);
            T1 = _mm256_unpackhi_epi16(S1, S2);
            T2 = _mm256_unpacklo_epi16(S3, S4);
            T3 = _mm256_unpackhi_epi16(S3, S4);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi32(T0, T2);
            mVal2 = _mm256_add_epi32(T1, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal1 = _mm256_packus_epi32(mVal1, mVal2);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), mVal1);

            src += i_src2;
            dst += 2 * i_dst;
        }
    }
}

void com_if_filter_ver_4_w32_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coeff, int bit_depth)
{
    int row;
    const int offset = 32;
    const int shift = 6;
    int bsym = (coeff[1] == coeff[2]);
    __m256i mAddOffset = _mm256_set1_epi32(offset);
    const int i_src2 = i_src << 1;
    const int i_src3 = i_src + i_src2;
    const int i_src4 = i_src << 2;

    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel_t)max_pixel);

    src -= i_src;

    if (bsym) {
        __m128i coeff1 = _mm_set1_epi16(((i16s_t*)coeff)[0]);
        __m256i coeff0 = _mm256_cvtepi8_epi16(coeff1);
        __m256i T0, T1, T2, T3, mVal1, mVal2, mVal3, mVal4;
        __m256i R0, R1, R2, R3;
        for (row = 0; row < height; row += 2) {
            const pel_t *src1 = src + 16;
            __m256i S0 = _mm256_loadu_si256((__m256i*)(src));
            __m256i S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
            __m256i S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
            __m256i S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));
            __m256i S4 = _mm256_loadu_si256((__m256i*)(src + i_src4));

            R0 = _mm256_add_epi16(S0, S3);
            R1 = _mm256_add_epi16(S1, S2);
            R2 = _mm256_add_epi16(S1, S4);
            R3 = _mm256_add_epi16(S2, S3);

            T0 = _mm256_unpacklo_epi16(R0, R1);
            T1 = _mm256_unpackhi_epi16(R0, R1);
            T2 = _mm256_unpacklo_epi16(R2, R3);
            T3 = _mm256_unpackhi_epi16(R2, R3);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff0);
            T3 = _mm256_madd_epi16(T3, coeff0);

            mVal1 = _mm256_add_epi32(T0, mAddOffset);
            mVal2 = _mm256_add_epi32(T1, mAddOffset);
            mVal3 = _mm256_add_epi32(T2, mAddOffset);
            mVal4 = _mm256_add_epi32(T3, mAddOffset);

            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal3 = _mm256_srai_epi32(mVal3, shift);
            mVal4 = _mm256_srai_epi32(mVal4, shift);

            mVal1 = _mm256_packus_epi32(mVal1, mVal2);
            mVal3 = _mm256_packus_epi32(mVal3, mVal4);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            mVal3 = _mm256_min_epu16(mVal3, max_val);
            _mm256_storeu_si256((__m256i*)dst, mVal1);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), mVal3);

            S0 = _mm256_loadu_si256((__m256i*)(src1));
            S1 = _mm256_loadu_si256((__m256i*)(src1 + i_src));
            S2 = _mm256_loadu_si256((__m256i*)(src1 + i_src2));
            S3 = _mm256_loadu_si256((__m256i*)(src1 + i_src3));
            S4 = _mm256_loadu_si256((__m256i*)(src1 + i_src4));

            R0 = _mm256_add_epi16(S0, S3);
            R1 = _mm256_add_epi16(S1, S2);
            R2 = _mm256_add_epi16(S1, S4);
            R3 = _mm256_add_epi16(S2, S3);

            T0 = _mm256_unpacklo_epi16(R0, R1);
            T1 = _mm256_unpackhi_epi16(R0, R1);
            T2 = _mm256_unpacklo_epi16(R2, R3);
            T3 = _mm256_unpackhi_epi16(R2, R3);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff0);
            T3 = _mm256_madd_epi16(T3, coeff0);

            mVal1 = _mm256_add_epi32(T0, mAddOffset);
            mVal2 = _mm256_add_epi32(T1, mAddOffset);
            mVal3 = _mm256_add_epi32(T2, mAddOffset);
            mVal4 = _mm256_add_epi32(T3, mAddOffset);

            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal3 = _mm256_srai_epi32(mVal3, shift);
            mVal4 = _mm256_srai_epi32(mVal4, shift);

            mVal1 = _mm256_packus_epi32(mVal1, mVal2);
            mVal3 = _mm256_packus_epi32(mVal3, mVal4);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            mVal3 = _mm256_min_epu16(mVal3, max_val);
            _mm256_storeu_si256((__m256i*)(dst + 16), mVal1);
            _mm256_storeu_si256((__m256i*)(dst + i_dst + 16), mVal3);

            src += i_src2;
            dst += 2 * i_dst;
        }
    }
    else {
        __m128i coeff00 = _mm_set1_epi16(*(i16s_t*)coeff);
        __m128i coeff11 = _mm_set1_epi16(*(i16s_t*)(coeff + 2));
        __m256i coeff0 = _mm256_cvtepi8_epi16(coeff00);
        __m256i coeff1 = _mm256_cvtepi8_epi16(coeff11);
        __m256i T0, T1, T2, T3, mVal1, mVal2;
        for (row = 0; row < height; row += 2) {
            const pel_t *src1 = src + 16;
            __m256i S0 = _mm256_loadu_si256((__m256i*)(src));
            __m256i S1 = _mm256_loadu_si256((__m256i*)(src + i_src));
            __m256i S2 = _mm256_loadu_si256((__m256i*)(src + i_src2));
            __m256i S3 = _mm256_loadu_si256((__m256i*)(src + i_src3));
            __m256i S4 = _mm256_loadu_si256((__m256i*)(src + i_src4));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpackhi_epi16(S0, S1);
            T2 = _mm256_unpacklo_epi16(S2, S3);
            T3 = _mm256_unpackhi_epi16(S2, S3);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi32(T0, T2);
            mVal2 = _mm256_add_epi32(T1, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal1 = _mm256_packus_epi32(mVal1, mVal2);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            _mm256_storeu_si256((__m256i*)dst, mVal1);

            T0 = _mm256_unpacklo_epi16(S1, S2);
            T1 = _mm256_unpackhi_epi16(S1, S2);
            T2 = _mm256_unpacklo_epi16(S3, S4);
            T3 = _mm256_unpackhi_epi16(S3, S4);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi32(T0, T2);
            mVal2 = _mm256_add_epi32(T1, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal1 = _mm256_packus_epi32(mVal1, mVal2);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), mVal1);

            S0 = _mm256_loadu_si256((__m256i*)(src1));
            S1 = _mm256_loadu_si256((__m256i*)(src1 + i_src));
            S2 = _mm256_loadu_si256((__m256i*)(src1 + i_src2));
            S3 = _mm256_loadu_si256((__m256i*)(src1 + i_src3));
            S4 = _mm256_loadu_si256((__m256i*)(src1 + i_src4));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpackhi_epi16(S0, S1);
            T2 = _mm256_unpacklo_epi16(S2, S3);
            T3 = _mm256_unpackhi_epi16(S2, S3);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi32(T0, T2);
            mVal2 = _mm256_add_epi32(T1, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal1 = _mm256_packus_epi32(mVal1, mVal2);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            _mm256_storeu_si256((__m256i*)(dst + 16), mVal1);

            T0 = _mm256_unpacklo_epi16(S1, S2);
            T1 = _mm256_unpackhi_epi16(S1, S2);
            T2 = _mm256_unpacklo_epi16(S3, S4);
            T3 = _mm256_unpackhi_epi16(S3, S4);

            T0 = _mm256_madd_epi16(T0, coeff0);
            T1 = _mm256_madd_epi16(T1, coeff0);
            T2 = _mm256_madd_epi16(T2, coeff1);
            T3 = _mm256_madd_epi16(T3, coeff1);

            mVal1 = _mm256_add_epi32(T0, T2);
            mVal2 = _mm256_add_epi32(T1, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift);
            mVal2 = _mm256_srai_epi32(mVal2, shift);
            mVal1 = _mm256_packus_epi32(mVal1, mVal2);

            mVal1 = _mm256_min_epu16(mVal1, max_val);
            _mm256_storeu_si256((__m256i*)(dst + i_dst + 16), mVal1);

            src += i_src2;
            dst += 2 * i_dst;
        }
    }
}

void com_if_filter_hor_ver_4_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth)
{
	int row, col;
	i16s_t const *p;

	int shift1 = bit_depth - 8;
	int shift2 = 20 - bit_depth;
	int add1 = (1 << (shift1)) >> 1;
	int add2 = 1 << (shift2 - 1);

	ALIGNED_16(i16s_t tmp_res[(32 + 3) * 32]);
	i16s_t *tmp = tmp_res;
	const int i_tmp = 32;

	__m128i mAddOffset1 = _mm_set1_epi32(add1);
	__m128i mAddOffset2 = _mm_set1_epi32(add2);

	__m128i mSwitch1 = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 8, 9);
	__m128i mSwitch2 = _mm_setr_epi8(4, 5, 6, 7, 8, 9, 10, 11, 6, 7, 8, 9, 10, 11, 12, 13);

	__m128i mCoefx = _mm_set1_epi32(*(i32s_t*)coef_x);
	mCoefx = _mm_cvtepi8_epi16(mCoefx);

	__m128i mask = _mm_loadu_si128((__m128i*)(intrinsic_mask_10bit[(width & 7) - 1]));
	int max_pixel = (1 << bit_depth) - 1;
	__m128i max_val = _mm_set1_epi16((pel_t)max_pixel);

	// HOR
	src = src - 1 * i_src - 1;

	if (width > 4) {
		for (row = -1; row < height + 2; row++) {
			__m128i mT0, mT1, mVal, mT2, mT3;
			for (col = 0; col < width; col += 8) {
				__m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
				mT0 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoefx);
				mT1 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoefx);

				mSrc = _mm_loadu_si128((__m128i*)(src + col + 4));
				mT2 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoefx);
				mT3 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoefx);

				mT0 = _mm_hadd_epi32(mT0, mT1);
				mT2 = _mm_hadd_epi32(mT2, mT3);

				mT0 = _mm_add_epi32(mT0, mAddOffset1);
				mT2 = _mm_add_epi32(mT2, mAddOffset1);
				mT0 = _mm_srai_epi32(mT0, shift1);
				mT2 = _mm_srai_epi32(mT2, shift1);

				mVal = _mm_packs_epi32(mT0, mT2);
				_mm_store_si128((__m128i*)&tmp[col], mVal);
			}
			src += i_src;
			tmp += i_tmp;
		}
	}
	else {
		for (row = -1; row < height + 2; row++) {
			__m128i mSrc = _mm_loadu_si128((__m128i*)(src));
			__m128i mT0 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoefx);
			__m128i mT1 = _mm_madd_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoefx);

			mT0 = _mm_hadd_epi32(mT0, mT1);
			mT0 = _mm_add_epi32(mT0, mAddOffset1);
			mT0 = _mm_srai_epi32(mT0, shift1);

			mT0 = _mm_hadd_epi16(mT0, mT0);
			_mm_storel_epi64((__m128i*)tmp, mT0);
			src += i_src;
			tmp += i_tmp;
		}
	}


	// VER

	tmp = tmp_res;

	__m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coef_y);
	__m128i coeff1 = _mm_set1_epi16(*(i16s_t*)(coef_y + 2));
	__m128i mVal;
	coeff0 = _mm_cvtepi8_epi16(coeff0);
	coeff1 = _mm_cvtepi8_epi16(coeff1);
	__m128i M0, M1, M2, M3;
	if (!(width & 7)){
		for (row = 0; row < height; row++) {
			p = tmp;
			for (col = 0; col < width; col += 8) {
				__m128i T00 = _mm_loadu_si128((__m128i*)(p));
				__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_tmp));
				__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_tmp));
				__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_tmp));

				M0 = _mm_unpacklo_epi16(T00, T10);
				M1 = _mm_unpacklo_epi16(T20, T30);
				M2 = _mm_unpackhi_epi16(T00, T10);
				M3 = _mm_unpackhi_epi16(T20, T30);

				M0 = _mm_madd_epi16(M0, coeff0);
				M1 = _mm_madd_epi16(M1, coeff1);
				M2 = _mm_madd_epi16(M2, coeff0);
				M3 = _mm_madd_epi16(M3, coeff1);

				M0 = _mm_add_epi32(M0, M1);
				M2 = _mm_add_epi32(M2, M3);

				M0 = _mm_add_epi32(M0, mAddOffset2);
				M2 = _mm_add_epi32(M2, mAddOffset2);
				M0 = _mm_srai_epi32(M0, shift2);
				M2 = _mm_srai_epi32(M2, shift2);

				mVal = _mm_packus_epi32(M0, M2);
				mVal = _mm_min_epu16(mVal, max_val);
				_mm_storeu_si128((__m128i*)&dst[col], mVal);

				p += 8;
			}

			tmp += i_tmp;
			dst += i_dst;
		}
	}
	else{
		for (row = 0; row < height; row++) {
			p = tmp;
			for (col = 0; col < width - 7; col += 8) {
				__m128i T00 = _mm_loadu_si128((__m128i*)(p));
				__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_tmp));
				__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_tmp));
				__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_tmp));

				M0 = _mm_unpacklo_epi16(T00, T10);
				M1 = _mm_unpacklo_epi16(T20, T30);
				M2 = _mm_unpackhi_epi16(T00, T10);
				M3 = _mm_unpackhi_epi16(T20, T30);

				M0 = _mm_madd_epi16(M0, coeff0);
				M1 = _mm_madd_epi16(M1, coeff1);
				M2 = _mm_madd_epi16(M2, coeff0);
				M3 = _mm_madd_epi16(M3, coeff1);

				M0 = _mm_add_epi32(M0, M1);
				M2 = _mm_add_epi32(M2, M3);

				M0 = _mm_add_epi32(M0, mAddOffset2);
				M2 = _mm_add_epi32(M2, mAddOffset2);
				M0 = _mm_srai_epi32(M0, shift2);
				M2 = _mm_srai_epi32(M2, shift2);

				mVal = _mm_packus_epi32(M0, M2);
				mVal = _mm_min_epu16(mVal, max_val);
				_mm_storeu_si128((__m128i*)&dst[col], mVal);

				p += 8;
			}

			__m128i T00 = _mm_loadu_si128((__m128i*)(p));
			__m128i T10 = _mm_loadu_si128((__m128i*)(p + i_tmp));
			__m128i T20 = _mm_loadu_si128((__m128i*)(p + 2 * i_tmp));
			__m128i T30 = _mm_loadu_si128((__m128i*)(p + 3 * i_tmp));

			M0 = _mm_unpacklo_epi16(T00, T10);
			M1 = _mm_unpacklo_epi16(T20, T30);
			M2 = _mm_unpackhi_epi16(T00, T10);
			M3 = _mm_unpackhi_epi16(T20, T30);

			M0 = _mm_madd_epi16(M0, coeff0);
			M1 = _mm_madd_epi16(M1, coeff1);
			M2 = _mm_madd_epi16(M2, coeff0);
			M3 = _mm_madd_epi16(M3, coeff1);

			M0 = _mm_add_epi32(M0, M1);
			M2 = _mm_add_epi32(M2, M3);

			M0 = _mm_add_epi32(M0, mAddOffset2);
			M2 = _mm_add_epi32(M2, mAddOffset2);
			M0 = _mm_srai_epi32(M0, shift2);
			M2 = _mm_srai_epi32(M2, shift2);

			mVal = _mm_packus_epi32(M0, M2);
			mVal = _mm_min_epu16(mVal, max_val);
			_mm_maskmoveu_si128(mVal, mask, (char_t*)&dst[col]);


			tmp += i_tmp;
			dst += i_dst;
		}
	}
}

void com_if_filter_hor_ver_4_w16_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, const char_t *coef_x, const char_t *coef_y, int bit_depth)
{
    ALIGNED_32(i16s_t tmp_res[(32 + 3) * 32]);
    i16s_t *tmp = tmp_res;
    const int i_tmp = 32;
    const int i_tmp2 = 64;
    const int i_tmp3 = 96;
    __m256i mask8 = _mm256_setr_epi32(-1, -1, -1, -1, 0, 0, 0, 0);
    int row, col;

    int shift1 = bit_depth - 8;
    int shift2 = 20 - bit_depth;
    int add1 = (1 << (shift1)) >> 1;
    int add2 = 1 << (shift2 - 1);
    __m256i mAddOffset = _mm256_set1_epi32(add1);
    __m128i mCoef1 = _mm_set1_epi32(*(i32s_t*)coef_x);
    __m256i mCoef = _mm256_cvtepi8_epi16(mCoef1);
    __m256i mSwitch1 = _mm256_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 8, 9);
    __m256i mSwitch2 = _mm256_setr_epi8(4, 5, 6, 7, 8, 9, 10, 11, 6, 7, 8, 9, 10, 11, 12, 13, 4, 5, 6, 7, 8, 9, 10, 11, 6, 7, 8, 9, 10, 11, 12, 13);
    __m256i T0, T1, S, S0, sum;
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel_t)max_pixel);

    //HOR
    src = src - i_src - 1;

    for (row = -1; row < height + 2; row++) {
        for (col = 0; col < width; col += 8) {
            S = _mm256_loadu_si256((__m256i*)(src + col));
            S0 = _mm256_permute4x64_epi64(S, 0x94);
            T0 = _mm256_madd_epi16(_mm256_shuffle_epi8(S0, mSwitch1), mCoef);
            T1 = _mm256_madd_epi16(_mm256_shuffle_epi8(S0, mSwitch2), mCoef);
            sum = _mm256_hadd_epi32(T0, T1);

            sum = _mm256_add_epi32(sum, mAddOffset);
            sum = _mm256_srai_epi32(sum, shift1);
            sum = _mm256_packs_epi32(sum, sum);
            sum = _mm256_permute4x64_epi64(sum, 0xd8);

            _mm256_maskstore_epi32((i32s_t*)(tmp + col), mask8, sum);
        }
        src += i_src;
        tmp += i_tmp;
    }

    // VER
    tmp = tmp_res;
    mAddOffset = _mm256_set1_epi32(add2);

    __m128i mCoefy11 = _mm_set1_epi16(*(i16s_t*)coef_y);
    __m128i mCoefy22 = _mm_set1_epi16(*(i16s_t*)(coef_y + 2));
    __m256i mVal1, mVal2, mVal;
    __m256i T2, T3, S1, S2, S3;

    __m256i mCoefy1 = _mm256_cvtepi8_epi16(mCoefy11);
    __m256i mCoefy2 = _mm256_cvtepi8_epi16(mCoefy22);

    for (row = 0; row < height; row++)
    {
        for (col = 0; col < width; col += 16)
        {
            S0 = _mm256_load_si256((__m256i*)(tmp + col));
            S1 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp));
            S2 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp2));
            S3 = _mm256_load_si256((__m256i*)(tmp + col + i_tmp3));

            T0 = _mm256_unpacklo_epi16(S0, S1);
            T1 = _mm256_unpacklo_epi16(S2, S3);
            T2 = _mm256_unpackhi_epi16(S0, S1);
            T3 = _mm256_unpackhi_epi16(S2, S3);

            T0 = _mm256_madd_epi16(T0, mCoefy1);
            T1 = _mm256_madd_epi16(T1, mCoefy2);
            T2 = _mm256_madd_epi16(T2, mCoefy1);
            T3 = _mm256_madd_epi16(T3, mCoefy2);

            mVal1 = _mm256_add_epi32(T0, T1);
            mVal2 = _mm256_add_epi32(T2, T3);

            mVal1 = _mm256_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm256_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm256_srai_epi32(mVal1, shift2);
            mVal2 = _mm256_srai_epi32(mVal2, shift2);

            mVal = _mm256_packus_epi32(mVal1, mVal2);
            mVal = _mm256_min_epu16(mVal, max_val);
            _mm256_storeu_si256((__m256i*)(dst + col), mVal);
        }
        tmp += i_tmp;
        dst += i_dst;
    }

}

void com_if_filter_hor_8_sse128_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
	int i, j;
	pel_t *d0 = dst[0];
	pel_t *d1 = dst[1];
	pel_t *d2 = dst[2];
	i16s_t *dt0 = dst_tmp[0];
	i16s_t *dt1 = dst_tmp[1];
	i16s_t *dt2 = dst_tmp[2];
	int shift_tmp = bit_depth - 8;
	int add_tmp = (1 << (shift_tmp)) >> 1;
	int max_pixel = (1 << bit_depth) - 1;
	__m128i max_val = _mm_set1_epi16((pel_t)max_pixel);

	__m128i mCoef0 = _mm_loadl_epi64((__m128i*)coeff[0]);
	__m128i mCoef1 = _mm_loadl_epi64((__m128i*)coeff[1]);
	__m128i mCoef2 = _mm_loadl_epi64((__m128i*)coeff[2]);

	__m128i T0, T1, T2, T3, T4, T5, T6, T7;
	__m128i M0, M1, M2, M3, M4, M5, M6, M7;

	__m128i offset1 = _mm_set1_epi32(add_tmp);
	__m128i offset2 = _mm_set1_epi32(32);

	mCoef0 = _mm_cvtepi8_epi16(mCoef0);
	mCoef1 = _mm_cvtepi8_epi16(mCoef1);
	mCoef2 = _mm_cvtepi8_epi16(mCoef2);

	src -= 3;

	for (j = 0; j < height; j++) {
		const pel_t *p = src;
		for (i = 0; i < width; i+=8) {

			T0 = _mm_loadu_si128((__m128i*)p++);
			T1 = _mm_loadu_si128((__m128i*)p++);
			T2 = _mm_loadu_si128((__m128i*)p++);
			T3 = _mm_loadu_si128((__m128i*)p++);
			T4 = _mm_loadu_si128((__m128i*)p++);
			T5 = _mm_loadu_si128((__m128i*)p++);
			T6 = _mm_loadu_si128((__m128i*)p++);
			T7 = _mm_loadu_si128((__m128i*)p++);

			M0 = _mm_madd_epi16(T0, mCoef0);
			M1 = _mm_madd_epi16(T1, mCoef0);
			M2 = _mm_madd_epi16(T2, mCoef0);
			M3 = _mm_madd_epi16(T3, mCoef0);
			M4 = _mm_madd_epi16(T4, mCoef0);
			M5 = _mm_madd_epi16(T5, mCoef0);
			M6 = _mm_madd_epi16(T6, mCoef0);
			M7 = _mm_madd_epi16(T7, mCoef0);

			M0 = _mm_hadd_epi32(M0, M1);
			M1 = _mm_hadd_epi32(M2, M3);
			M2 = _mm_hadd_epi32(M4, M5);
			M3 = _mm_hadd_epi32(M6, M7);

			M0 = _mm_hadd_epi32(M0, M1);
			M1 = _mm_hadd_epi32(M2, M3);

			M2 = _mm_add_epi32(M0, offset1);
			M3 = _mm_add_epi32(M1, offset1);
			M2 = _mm_srai_epi32(M2, shift_tmp);
			M3 = _mm_srai_epi32(M3, shift_tmp);
			M2 = _mm_packs_epi32(M2, M3);
			_mm_storeu_si128((__m128i*)&dt0[i], M2);

			M2 = _mm_add_epi32(M0, offset2);
			M3 = _mm_add_epi32(M1, offset2);
			M2 = _mm_srai_epi32(M2, 6);
			M3 = _mm_srai_epi32(M3, 6);
			M2 = _mm_packus_epi32(M2, M3);
			M2 = _mm_min_epu16(M2, max_val);
			_mm_storeu_si128((__m128i*)&d0[i], M2);

			M0 = _mm_madd_epi16(T0, mCoef1);
			M1 = _mm_madd_epi16(T1, mCoef1);
			M2 = _mm_madd_epi16(T2, mCoef1);
			M3 = _mm_madd_epi16(T3, mCoef1);
			M4 = _mm_madd_epi16(T4, mCoef1);
			M5 = _mm_madd_epi16(T5, mCoef1);
			M6 = _mm_madd_epi16(T6, mCoef1);
			M7 = _mm_madd_epi16(T7, mCoef1);

			M0 = _mm_hadd_epi32(M0, M1);
			M1 = _mm_hadd_epi32(M2, M3);
			M2 = _mm_hadd_epi32(M4, M5);
			M3 = _mm_hadd_epi32(M6, M7);

			M0 = _mm_hadd_epi32(M0, M1);
			M1 = _mm_hadd_epi32(M2, M3);

			M2 = _mm_add_epi32(M0, offset1);
			M3 = _mm_add_epi32(M1, offset1);
			M2 = _mm_srai_epi32(M2, shift_tmp);
			M3 = _mm_srai_epi32(M3, shift_tmp);
			M2 = _mm_packs_epi32(M2, M3);
			_mm_storeu_si128((__m128i*)&dt1[i], M2);

			M2 = _mm_add_epi32(M0, offset2);
			M3 = _mm_add_epi32(M1, offset2);
			M2 = _mm_srai_epi32(M2, 6);
			M3 = _mm_srai_epi32(M3, 6);
			M2 = _mm_packus_epi32(M2, M3);
			M2 = _mm_min_epu16(M2, max_val);
			_mm_storeu_si128((__m128i*)&d1[i], M2);

			M0 = _mm_madd_epi16(T0, mCoef2);
			M1 = _mm_madd_epi16(T1, mCoef2);
			M2 = _mm_madd_epi16(T2, mCoef2);
			M3 = _mm_madd_epi16(T3, mCoef2);
			M4 = _mm_madd_epi16(T4, mCoef2);
			M5 = _mm_madd_epi16(T5, mCoef2);
			M6 = _mm_madd_epi16(T6, mCoef2);
			M7 = _mm_madd_epi16(T7, mCoef2);

			M0 = _mm_hadd_epi32(M0, M1);
			M1 = _mm_hadd_epi32(M2, M3);
			M2 = _mm_hadd_epi32(M4, M5);
			M3 = _mm_hadd_epi32(M6, M7);

			M0 = _mm_hadd_epi32(M0, M1);
			M1 = _mm_hadd_epi32(M2, M3);

			M2 = _mm_add_epi32(M0, offset1);
			M3 = _mm_add_epi32(M1, offset1);
			M2 = _mm_srai_epi32(M2, shift_tmp);
			M3 = _mm_srai_epi32(M3, shift_tmp);
			M2 = _mm_packs_epi32(M2, M3);
			_mm_storeu_si128((__m128i*)&dt2[i], M2);

			M2 = _mm_add_epi32(M0, offset2);
			M3 = _mm_add_epi32(M1, offset2);
			M2 = _mm_srai_epi32(M2, 6);
			M3 = _mm_srai_epi32(M3, 6);
			M2 = _mm_packus_epi32(M2, M3);
			M2 = _mm_min_epu16(M2, max_val);
			_mm_storeu_si128((__m128i*)&d2[i], M2);
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

void com_if_filter_hor_8_sse256_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, i16s_t *dst_tmp[3], int i_dst_tmp, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    i16s_t *dt0 = dst_tmp[0];
    i16s_t *dt1 = dst_tmp[1];
    i16s_t *dt2 = dst_tmp[2];
    int shift_tmp = bit_depth - 8;
    int add_tmp = (1 << (shift_tmp)) >> 1;
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel_t)max_pixel);
    i32s_t * coef;
    coef = (i32s_t *)(coeff[0]);
    __m128i mCoef00 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);
    coef = (i32s_t *)(coeff[1]);
    __m128i mCoef11 = _mm_setr_epi32(coef[0], coef[1], coef[0], coef[1]);
    coef = (i32s_t *)(coeff[2]);
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
        const pel_t *p = src;
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

void com_if_filter_ver_8_sse128_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
	int i, j;
	pel_t *d0 = dst[0];
	pel_t *d1 = dst[1];
	pel_t *d2 = dst[2];
	int max_pixel = (1 << bit_depth) - 1;
	__m128i max_val = _mm_set1_epi16((pel_t)max_pixel);
	__m128i mAddOffset = _mm_set1_epi32(32);
	__m128i T0, T1, T2, T3, T4, T5, T6, T7;
	__m128i M0, M1, M2, M3, M4, M5, M6, M7;
	__m128i N0, N1, N2, N3, N4, N5, N6, N7;

	__m128i coeff00 = _mm_set1_epi16(*(i16s_t*)coeff[0]);
	__m128i coeff01 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 2));
	__m128i coeff02 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 4));
	__m128i coeff03 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 6));
	coeff00 = _mm_cvtepi8_epi16(coeff00);
	coeff01 = _mm_cvtepi8_epi16(coeff01);
	coeff02 = _mm_cvtepi8_epi16(coeff02);
	coeff03 = _mm_cvtepi8_epi16(coeff03);

	__m128i coeff10 = _mm_set1_epi16(*(i16s_t*)coeff[1]);
	__m128i coeff11 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 2));
	coeff10 = _mm_cvtepi8_epi16(coeff10);
	coeff11 = _mm_cvtepi8_epi16(coeff11);

	__m128i coeff20 = _mm_set1_epi16(*(i16s_t*)coeff[2]);
	__m128i coeff21 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 2));
	__m128i coeff22 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 4));
	__m128i coeff23 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 6));	
	coeff20 = _mm_cvtepi8_epi16(coeff20);
	coeff21 = _mm_cvtepi8_epi16(coeff21);
	coeff22 = _mm_cvtepi8_epi16(coeff22);
	coeff23 = _mm_cvtepi8_epi16(coeff23);


	src -= 3 * i_src;

	for (j = 0; j < height; j++) {
		const pel_t *p = src;
		for (i = 0; i < width; i+=8) {
			T0 = _mm_loadu_si128((__m128i*)(p));
			T1 = _mm_loadu_si128((__m128i*)(p + i_src));
			T2 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
			T3 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));
			T4 = _mm_loadu_si128((__m128i*)(p + 4 * i_src));
			T5 = _mm_loadu_si128((__m128i*)(p + 5 * i_src));
			T6 = _mm_loadu_si128((__m128i*)(p + 6 * i_src));
			T7 = _mm_loadu_si128((__m128i*)(p + 7 * i_src));

			M0 = _mm_unpacklo_epi16(T0, T1);
			M1 = _mm_unpacklo_epi16(T2, T3);
			M2 = _mm_unpacklo_epi16(T4, T5);
			M3 = _mm_unpacklo_epi16(T6, T7);
			M4 = _mm_unpackhi_epi16(T0, T1);
			M5 = _mm_unpackhi_epi16(T2, T3);
			M6 = _mm_unpackhi_epi16(T4, T5);
			M7 = _mm_unpackhi_epi16(T6, T7);

			N0 = _mm_madd_epi16(M0, coeff00);
			N1 = _mm_madd_epi16(M1, coeff01);
			N2 = _mm_madd_epi16(M2, coeff02);
			N3 = _mm_madd_epi16(M3, coeff03);
			N4 = _mm_madd_epi16(M4, coeff00);
			N5 = _mm_madd_epi16(M5, coeff01);
			N6 = _mm_madd_epi16(M6, coeff02);
			N7 = _mm_madd_epi16(M7, coeff03);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);
			N2 = _mm_add_epi32(N4, N5);
			N3 = _mm_add_epi32(N6, N7);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);

			N0 = _mm_add_epi32(N0, mAddOffset);
			N1 = _mm_add_epi32(N1, mAddOffset);
			N0 = _mm_srai_epi32(N0, 6);
			N1 = _mm_srai_epi32(N1, 6);
			N0 = _mm_packus_epi32(N0, N1);
			N0 = _mm_min_epu16(N0, max_val);
			_mm_storeu_si128((__m128i*)&d0[i], N0);
			
			N0 = _mm_madd_epi16(M0, coeff20);
			N1 = _mm_madd_epi16(M1, coeff21);
			N2 = _mm_madd_epi16(M2, coeff22);
			N3 = _mm_madd_epi16(M3, coeff23);
			N4 = _mm_madd_epi16(M4, coeff20);
			N5 = _mm_madd_epi16(M5, coeff21);
			N6 = _mm_madd_epi16(M6, coeff22);
			N7 = _mm_madd_epi16(M7, coeff23);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);
			N2 = _mm_add_epi32(N4, N5);
			N3 = _mm_add_epi32(N6, N7);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);

			N0 = _mm_add_epi32(N0, mAddOffset);
			N1 = _mm_add_epi32(N1, mAddOffset);
			N0 = _mm_srai_epi32(N0, 6);
			N1 = _mm_srai_epi32(N1, 6);
			N0 = _mm_packus_epi32(N0, N1);
			N0 = _mm_min_epu16(N0, max_val);
			_mm_storeu_si128((__m128i*)&d2[i], N0);

			T0 = _mm_add_epi16(T0, T7);
			T1 = _mm_add_epi16(T1, T6);
			T2 = _mm_add_epi16(T2, T5);
			T3 = _mm_add_epi16(T3, T4);

			M0 = _mm_unpacklo_epi16(T0, T1);
			M1 = _mm_unpacklo_epi16(T2, T3);
			M2 = _mm_unpackhi_epi16(T0, T1);
			M3 = _mm_unpackhi_epi16(T2, T3);

			N0 = _mm_madd_epi16(M0, coeff10);
			N1 = _mm_madd_epi16(M1, coeff11);
			N2 = _mm_madd_epi16(M2, coeff10);
			N3 = _mm_madd_epi16(M3, coeff11);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);

			N0 = _mm_add_epi32(N0, mAddOffset);
			N1 = _mm_add_epi32(N1, mAddOffset);
			N0 = _mm_srai_epi32(N0, 6);
			N1 = _mm_srai_epi32(N1, 6);
			N0 = _mm_packus_epi32(N0, N1);
			N0 = _mm_min_epu16(N0, max_val);
			_mm_storeu_si128((__m128i*)&d1[i], N0);

			p += 8;
		}
		d0 += i_dst;
		d1 += i_dst;
		d2 += i_dst;
		src += i_src;
	}
}

void com_if_filter_ver_8_sse256_10bit(const pel_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j;
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16((pel_t)max_pixel);
    __m256i mAddOffset = _mm256_set1_epi32(32);
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i N0, N1, N2, N3, N4, N5, N6, N7;

    __m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coeff[0]);
    __m128i coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 2));
    __m128i coeff2 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 4));
    __m128i coeff3 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 6));
    __m256i coeff00 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff01 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff02 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff03 = _mm256_cvtepi8_epi16(coeff3);

    coeff0 = _mm_set1_epi16(*(i16s_t*)coeff[1]);
    coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 2));
    __m256i coeff10 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff11 = _mm256_cvtepi8_epi16(coeff1);

    coeff0 = _mm_set1_epi16(*(i16s_t*)coeff[2]);
    coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 2));
    coeff2 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 4));
    coeff3 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 6));
    __m256i coeff20 = _mm256_cvtepi8_epi16(coeff0);
    __m256i coeff21 = _mm256_cvtepi8_epi16(coeff1);
    __m256i coeff22 = _mm256_cvtepi8_epi16(coeff2);
    __m256i coeff23 = _mm256_cvtepi8_epi16(coeff3);


    src -= 3 * i_src;

    for (j = 0; j < height; j++) {
        const pel_t *p = src;
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

void com_if_filter_ver_8_ext_sse128_10bit(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
	int i, j;
	int shift1 = 20 - bit_depth;
	int add1 = 1 << (shift1 - 1);
	pel_t *d0 = dst[0];
	pel_t *d1 = dst[1];
	pel_t *d2 = dst[2];
	int max_pixel = (1 << bit_depth) - 1;
	__m128i max_val = _mm_set1_epi16((pel_t)max_pixel);
	__m128i mAddOffset = _mm_set1_epi32(add1);
	__m128i T0, T1, T2, T3, T4, T5, T6, T7;
	__m128i M0, M1, M2, M3, M4, M5, M6, M7;
	__m128i N0, N1, N2, N3, N4, N5, N6, N7;

	__m128i coeff00 = _mm_set1_epi16(*(i16s_t*)coeff[0]);
	__m128i coeff01 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 2));
	__m128i coeff02 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 4));
	__m128i coeff03 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 6));
	coeff00 = _mm_cvtepi8_epi16(coeff00);
	coeff01 = _mm_cvtepi8_epi16(coeff01);
	coeff02 = _mm_cvtepi8_epi16(coeff02);
	coeff03 = _mm_cvtepi8_epi16(coeff03);

	__m128i coeff10 = _mm_set1_epi16(*(i16s_t*)coeff[1]);
	__m128i coeff11 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 2));
	__m128i coeff12 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 4));
	__m128i coeff13 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 6));
	coeff10 = _mm_cvtepi8_epi16(coeff10);
	coeff11 = _mm_cvtepi8_epi16(coeff11);
	coeff12 = _mm_cvtepi8_epi16(coeff12);
	coeff13 = _mm_cvtepi8_epi16(coeff13);

	__m128i coeff20 = _mm_set1_epi16(*(i16s_t*)coeff[2]);
	__m128i coeff21 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 2));
	__m128i coeff22 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 4));
	__m128i coeff23 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 6));
	coeff20 = _mm_cvtepi8_epi16(coeff20);
	coeff21 = _mm_cvtepi8_epi16(coeff21);
	coeff22 = _mm_cvtepi8_epi16(coeff22);
	coeff23 = _mm_cvtepi8_epi16(coeff23);


	src -= 3 * i_src;

	for (j = 0; j < height; j++) {
		const i16s_t *p = src;
		for (i = 0; i < width; i += 8) {
			T0 = _mm_loadu_si128((__m128i*)(p));
			T1 = _mm_loadu_si128((__m128i*)(p + i_src));
			T2 = _mm_loadu_si128((__m128i*)(p + 2 * i_src));
			T3 = _mm_loadu_si128((__m128i*)(p + 3 * i_src));
			T4 = _mm_loadu_si128((__m128i*)(p + 4 * i_src));
			T5 = _mm_loadu_si128((__m128i*)(p + 5 * i_src));
			T6 = _mm_loadu_si128((__m128i*)(p + 6 * i_src));
			T7 = _mm_loadu_si128((__m128i*)(p + 7 * i_src));

			M0 = _mm_unpacklo_epi16(T0, T1);
			M1 = _mm_unpacklo_epi16(T2, T3);
			M2 = _mm_unpacklo_epi16(T4, T5);
			M3 = _mm_unpacklo_epi16(T6, T7);
			M4 = _mm_unpackhi_epi16(T0, T1);
			M5 = _mm_unpackhi_epi16(T2, T3);
			M6 = _mm_unpackhi_epi16(T4, T5);
			M7 = _mm_unpackhi_epi16(T6, T7);

			N0 = _mm_madd_epi16(M0, coeff00);
			N1 = _mm_madd_epi16(M1, coeff01);
			N2 = _mm_madd_epi16(M2, coeff02);
			N3 = _mm_madd_epi16(M3, coeff03);
			N4 = _mm_madd_epi16(M4, coeff00);
			N5 = _mm_madd_epi16(M5, coeff01);
			N6 = _mm_madd_epi16(M6, coeff02);
			N7 = _mm_madd_epi16(M7, coeff03);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);
			N2 = _mm_add_epi32(N4, N5);
			N3 = _mm_add_epi32(N6, N7);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);

			N0 = _mm_add_epi32(N0, mAddOffset);
			N1 = _mm_add_epi32(N1, mAddOffset);
			N0 = _mm_srai_epi32(N0, shift1);
			N1 = _mm_srai_epi32(N1, shift1);
			N0 = _mm_packus_epi32(N0, N1);
			N0 = _mm_min_epu16(N0, max_val);
			_mm_storeu_si128((__m128i*)&d0[i], N0);

			N0 = _mm_madd_epi16(M0, coeff10);
			N1 = _mm_madd_epi16(M1, coeff11);
			N2 = _mm_madd_epi16(M2, coeff12);
			N3 = _mm_madd_epi16(M3, coeff13);
			N4 = _mm_madd_epi16(M4, coeff10);
			N5 = _mm_madd_epi16(M5, coeff11);
			N6 = _mm_madd_epi16(M6, coeff12);
			N7 = _mm_madd_epi16(M7, coeff13);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);
			N2 = _mm_add_epi32(N4, N5);
			N3 = _mm_add_epi32(N6, N7);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);

			N0 = _mm_add_epi32(N0, mAddOffset);
			N1 = _mm_add_epi32(N1, mAddOffset);
			N0 = _mm_srai_epi32(N0, shift1);
			N1 = _mm_srai_epi32(N1, shift1);
			N0 = _mm_packus_epi32(N0, N1);
			N0 = _mm_min_epu16(N0, max_val);
			_mm_storeu_si128((__m128i*)&d1[i], N0);

			N0 = _mm_madd_epi16(M0, coeff20);
			N1 = _mm_madd_epi16(M1, coeff21);
			N2 = _mm_madd_epi16(M2, coeff22);
			N3 = _mm_madd_epi16(M3, coeff23);
			N4 = _mm_madd_epi16(M4, coeff20);
			N5 = _mm_madd_epi16(M5, coeff21);
			N6 = _mm_madd_epi16(M6, coeff22);
			N7 = _mm_madd_epi16(M7, coeff23);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);
			N2 = _mm_add_epi32(N4, N5);
			N3 = _mm_add_epi32(N6, N7);

			N0 = _mm_add_epi32(N0, N1);
			N1 = _mm_add_epi32(N2, N3);

			N0 = _mm_add_epi32(N0, mAddOffset);
			N1 = _mm_add_epi32(N1, mAddOffset);
			N0 = _mm_srai_epi32(N0, shift1);
			N1 = _mm_srai_epi32(N1, shift1);
			N0 = _mm_packus_epi32(N0, N1);
			N0 = _mm_min_epu16(N0, max_val);
			_mm_storeu_si128((__m128i*)&d2[i], N0);

			p += 8;
		}
		d0 += i_dst;
		d1 += i_dst;
		d2 += i_dst;
		src += i_src;
	}
}

void com_if_filter_ver_8_ext_sse256_10bit(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
	int i, j;
	int shift1 = 20 - bit_depth;
	int add1 = 1 << (shift1 - 1);
	pel_t *d0 = dst[0];
	pel_t *d1 = dst[1];
	pel_t *d2 = dst[2];
	int max_pixel = (1 << bit_depth) - 1;
	__m256i max_val = _mm256_set1_epi16((pel_t)max_pixel);
	__m256i mAddOffset = _mm256_set1_epi32(add1);
	__m256i T0, T1, T2, T3, T4, T5, T6, T7;
	__m256i M0, M1, M2, M3, M4, M5, M6, M7;
	__m256i N0, N1, N2, N3, N4, N5, N6, N7;

	__m128i coeff0 = _mm_set1_epi16(*(i16s_t*)coeff[0]);
	__m128i coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 2));
	__m128i coeff2 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 4));
	__m128i coeff3 = _mm_set1_epi16(*(i16s_t*)(coeff[0] + 6));
	__m256i coeff00 = _mm256_cvtepi8_epi16(coeff0);
	__m256i coeff01 = _mm256_cvtepi8_epi16(coeff1);
	__m256i coeff02 = _mm256_cvtepi8_epi16(coeff2);
	__m256i coeff03 = _mm256_cvtepi8_epi16(coeff3);

	coeff0 = _mm_set1_epi16(*(i16s_t*)coeff[1]);
	coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 2));
	coeff2 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 4));
	coeff3 = _mm_set1_epi16(*(i16s_t*)(coeff[1] + 6));
	__m256i coeff10 = _mm256_cvtepi8_epi16(coeff0);
	__m256i coeff11 = _mm256_cvtepi8_epi16(coeff1);
	__m256i coeff12 = _mm256_cvtepi8_epi16(coeff2);
	__m256i coeff13 = _mm256_cvtepi8_epi16(coeff3);

	coeff0 = _mm_set1_epi16(*(i16s_t*)coeff[2]);
	coeff1 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 2));
	coeff2 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 4));
	coeff3 = _mm_set1_epi16(*(i16s_t*)(coeff[2] + 6));
	__m256i coeff20 = _mm256_cvtepi8_epi16(coeff0);
	__m256i coeff21 = _mm256_cvtepi8_epi16(coeff1);
	__m256i coeff22 = _mm256_cvtepi8_epi16(coeff2);
	__m256i coeff23 = _mm256_cvtepi8_epi16(coeff3);


	src -= 3 * i_src;

	for (j = 0; j < height; j++) {
		const i16s_t *p = src;
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

