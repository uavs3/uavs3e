#include "intrinsic.h"
#include <assert.h>

#if defined(_MSC_VER) 
#pragma warning(disable: 4100)  // unreferenced formal parameter
#endif

void cpy_pel_I420_to_uchar_YUY2_sse128(const pel_t *srcy, const pel_t *srcu, const pel_t *srcv, int i_src, int i_srcc, uchar_t *dst, int i_dst, int width, int height)
{
    int i, j, l;
    const pel_t *src1 = srcy + i_src;
    uchar_t     *dst1 = dst + i_dst;

    __m128i TY00, TY01, TY10, TY11, TU, TV, TC0, TC1;
    __m128i TD00, TD01, TD10, TD11;

    if (!(width & 31)) {
        for (i = 0; i < height; i += 2) {
            for (j = 0, l = 0; j < width - 31; j += 32, l += 64) {
                TU = _mm_load_si128((const __m128i*)(srcu + (j >> 1)));
                TV = _mm_load_si128((const __m128i*)(srcv + (j >> 1)));

                TC0 = _mm_unpacklo_epi8(TU, TV);
                TY00 = _mm_load_si128((const __m128i*)(srcy + j));
                TY10 = _mm_load_si128((const __m128i*)(src1 + j));

                TD00 = _mm_unpacklo_epi8(TY00, TC0);
                TD01 = _mm_unpackhi_epi8(TY00, TC0);
                TD10 = _mm_unpacklo_epi8(TY10, TC0);
                TD11 = _mm_unpackhi_epi8(TY10, TC0);

                _mm_storeu_si128((__m128i*)(dst + l), TD00);
                _mm_storeu_si128((__m128i*)(dst + l + 16), TD01);
                _mm_storeu_si128((__m128i*)(dst1 + l), TD10);
                _mm_storeu_si128((__m128i*)(dst1 + l + 16), TD11);

                TC1 = _mm_unpackhi_epi8(TU, TV);
                TY01 = _mm_load_si128((const __m128i*)(srcy + j + 16));
                TY11 = _mm_load_si128((const __m128i*)(src1 + j + 16));

                TD00 = _mm_unpacklo_epi8(TY01, TC1);
                TD01 = _mm_unpackhi_epi8(TY01, TC1);
                TD10 = _mm_unpacklo_epi8(TY11, TC1);
                TD11 = _mm_unpackhi_epi8(TY11, TC1);

                _mm_storeu_si128((__m128i*)(dst + l + 32), TD00);
                _mm_storeu_si128((__m128i*)(dst + l + 48), TD01);
                _mm_storeu_si128((__m128i*)(dst1 + l + 32), TD10);
                _mm_storeu_si128((__m128i*)(dst1 + l + 48), TD11);
            }
            srcy += 2 * i_src;
            src1 += 2 * i_src;
            dst  += 2 * i_dst;
            dst1 += 2 * i_dst;
            srcu += i_srcc;
            srcv += i_srcc;
        }
    } else {
        for (i = 0; i < height; i += 2) {
            for (j = 0, l = 0; j < width - 15; j += 16, l += 32) {
                TU = _mm_loadl_epi64((const __m128i*)(srcu + (j >> 1)));
                TV = _mm_loadl_epi64((const __m128i*)(srcv + (j >> 1)));

                TC0 = _mm_unpacklo_epi8(TU, TV);
                TY00 = _mm_load_si128((const __m128i*)(srcy + j));
                TY10 = _mm_load_si128((const __m128i*)(src1 + j));

                TD00 = _mm_unpacklo_epi8(TY00, TC0);
                TD01 = _mm_unpackhi_epi8(TY00, TC0);
                TD10 = _mm_unpacklo_epi8(TY10, TC0);
                TD11 = _mm_unpackhi_epi8(TY10, TC0);

                _mm_storeu_si128((__m128i*)(dst  + l     ), TD00);
                _mm_storeu_si128((__m128i*)(dst  + l + 16), TD01);
                _mm_storeu_si128((__m128i*)(dst1 + l     ), TD10);
                _mm_storeu_si128((__m128i*)(dst1 + l + 16), TD11);
            }

            if (j < width) {
                assert(j + 8 == width);
                TU = _mm_loadl_epi64((const __m128i*)(srcu + (j >> 1)));
                TV = _mm_loadl_epi64((const __m128i*)(srcv + (j >> 1)));

                TC0 = _mm_unpacklo_epi8(TU, TV);
                TY00 = _mm_load_si128((const __m128i*)(srcy + j));
                TY10 = _mm_load_si128((const __m128i*)(src1 + j));

                TD00 = _mm_unpacklo_epi8(TY00, TC0);
                TD10 = _mm_unpacklo_epi8(TY10, TC0);

                _mm_storeu_si128((__m128i*)(dst  + l), TD00);
                _mm_storeu_si128((__m128i*)(dst1 + l), TD10);
            }

            srcy += 2 * i_src;
            src1 += 2 * i_src;
            dst += 2 * i_dst;
            dst1 += 2 * i_dst;
            srcu += i_srcc;
            srcv += i_srcc;
        }
    }
}

void avg_pel_0_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i, j;
    __m128i S1, S2, D;
    __m128i mask = _mm_load_si128((const __m128i*)intrinsic_mask[(width & 15) - 1]);

    for (i = 0; i < height; i++) {
        for (j = 0; j < width - 15; j += 16) {
            S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
            S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
            D = _mm_avg_epu8(S1, S2);
            _mm_storeu_si128((__m128i*)(dst + j), D);
        }

        S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
        S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
        D = _mm_avg_epu8(S1, S2);
        _mm_maskmoveu_si128(D, mask, (char_t*)&dst[j]);

        src1 += i_src1;
        src2 += i_src2;
        dst += i_dst;
    }
}

void avg_pel_2_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m128i S1, S2, D;
    i16s_t *s1 = (i16s_t *)src1;
    i16s_t *s2 = (i16s_t *)src2;
    i16s_t *d  = (i16s_t *)dst;

    int i_s1 = i_src1 >> 1;
    int i_s2 = i_src2 >> 1;
    int i_d  = i_dst  >> 1;

    for (i = 0; i < height; i += 4) {
        S1 = _mm_setr_epi16(s1[0], s1[i_s1], s1[i_s1 << 1], s1[(i_s1 << 1) + i_s1], 0, 0, 0, 0);
        S2 = _mm_setr_epi16(s2[0], s2[i_s2], s2[i_s2 << 1], s2[(i_s2 << 1) + i_s2], 0, 0, 0, 0);
        D = _mm_avg_epu8(S1, S2);
        *d = _mm_extract_epi16(D, 0);
        d += i_d;
        *d = _mm_extract_epi16(D, 1);
        d += i_d;
        *d = _mm_extract_epi16(D, 2);
        d += i_d;
        *d = _mm_extract_epi16(D, 3);
        d += i_d;

        s1 += i_src1 << 1;
        s2 += i_src2 << 1;
    }
}

void avg_pel_4_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m128i S1, S2, D;
    int *s1 = (int *)src1;
    int *s2 = (int *)src2;
    int *d  = (int *)dst;

    if (height == 2) {
        S1 = _mm_setr_epi32(s1[0], s1[i_src1 >> 2], 0, 0);
        S2 = _mm_setr_epi32(s2[0], s2[i_src2 >> 2], 0, 0);
        D = _mm_avg_epu8(S1, S2);
        d[         0] = _mm_extract_epi32(D, 0);
        d[i_dst >> 2] = _mm_extract_epi32(D, 1);
    } else {
        int i_s1 = i_src1 >> 2;
        int i_s2 = i_src2 >> 2;
        int i_d  = i_dst  >> 2;

        for (i = 0; i < height; i += 4) {
            S1 = _mm_setr_epi32(s1[0], s1[i_s1], s1[i_s1 << 1], s1[(i_s1 << 1) + i_s1]);
            S2 = _mm_setr_epi32(s2[0], s2[i_s2], s2[i_s2 << 1], s2[(i_s2 << 1) + i_s2]);
            D = _mm_avg_epu8(S1, S2);
            *d = _mm_extract_epi32(D, 0);
            d += i_d;
            *d = _mm_extract_epi32(D, 1);
            d += i_d;
            *d = _mm_extract_epi32(D, 2);
            d += i_d;
            *d = _mm_extract_epi32(D, 3);
            d += i_d;

            s1 += i_src1;
            s2 += i_src2;
        }
    }
}

void avg_pel_8_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m128i S1, S2, D;

    for (i = 0; i < height; i++) {
        S1 = _mm_loadl_epi64((const __m128i*)src1);
        S2 = _mm_loadl_epi64((const __m128i*)src2);
        D = _mm_avg_epu8(S1, S2);
        _mm_storel_epi64((__m128i*)dst, D);

        src1 += i_src1;
        src2 += i_src2;
        dst += i_dst;
    }
}

void avg_pel_12_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m128i S1, S2, D;
    __m128i mask = _mm_load_si128((const __m128i*)intrinsic_mask[11]);

    for (i = 0; i < height; i++) {
        S1 = _mm_loadu_si128((const __m128i*)src1);
        S2 = _mm_loadu_si128((const __m128i*)src2);
        D  = _mm_avg_epu8(S1, S2);
        _mm_maskmoveu_si128(D, mask, (char_t*)dst);

        src1 += i_src1;
        src2 += i_src2;
        dst += i_dst;
    }
}

void avg_pel_16_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m128i S1, S2, D;

    for (i = 0; i < height; i++) {
        S1 = _mm_loadu_si128((const __m128i*)src1);
        S2 = _mm_loadu_si128((const __m128i*)src2);
        D  = _mm_avg_epu8(S1, S2);
        _mm_storeu_si128((__m128i*)dst, D);

        src1 += i_src1;
        src2 += i_src2;
        dst += i_dst;
    }
}

void avg_pel_24_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m128i S1, S2, D;

    for (i = 0; i < height; i++) {
        S1 = _mm_loadu_si128((const __m128i*)src1);
        S2 = _mm_loadu_si128((const __m128i*)src2);
        D  = _mm_avg_epu8(S1, S2);
        _mm_storeu_si128((__m128i*)dst, D);

        S1 = _mm_loadl_epi64((const __m128i*)(src1 + 16));
        S2 = _mm_loadl_epi64((const __m128i*)(src2 + 16));
        D  = _mm_avg_epu8(S1, S2);
        _mm_storel_epi64((__m128i*)(dst + 16), D);

        src1 += i_src1;
        src2 += i_src2;
        dst  += i_dst;
    }
}

void avg_pel_x16_sse128(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i, j;
    __m128i S1, S2, D;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j += 16) {
            S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
            S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
            D  = _mm_avg_epu8(S1, S2);
            _mm_storeu_si128((__m128i*)(dst + j), D);
        }
        src1 += i_src1;
        src2 += i_src2;
        dst += i_dst;
    }
}

void avg_pel_x16_sse256(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    __m256i S1, S2, S3, S4, D, D1;

    if (width == 32) {
        int i_src1x2 = i_src1 << 1;
        int i_src2x2 = i_src2 << 1;
        int i_dstx2 = i_dst << 1;
        height >>= 1;

        while (height--) {
            S1 = _mm256_loadu_si256((const __m256i*)(src1));
            S2 = _mm256_loadu_si256((const __m256i*)(src2));
            D = _mm256_avg_epu8(S1, S2);
            _mm256_storeu_si256((__m256i*)(dst), D);

            S3 = _mm256_loadu_si256((const __m256i*)(src1 + i_src1));
            S4 = _mm256_loadu_si256((const __m256i*)(src2 + i_src2));
            D1 = _mm256_avg_epu8(S3, S4);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), D1);

            src1 += i_src1x2;
            src2 += i_src2x2;
            dst  += i_dstx2;
        }
    } else {
        assert(width == 64);
        while (height--) {
            S1 = _mm256_loadu_si256((const __m256i*)(src1));
            S2 = _mm256_loadu_si256((const __m256i*)(src2));
            D = _mm256_avg_epu8(S1, S2);
            _mm256_storeu_si256((__m256i*)(dst), D);

            S1 = _mm256_loadu_si256((const __m256i*)(src1 + 32));
            S2 = _mm256_loadu_si256((const __m256i*)(src2 + 32));
            D = _mm256_avg_epu8(S1, S2);
            _mm256_storeu_si256((__m256i*)(dst + 32), D);
            
            src1 += i_src1;
            src2 += i_src2;
            dst  += i_dst;
        }
    }
}

void avg_pel_1d_sse128(pel_t *dst, pel_t *src1, pel_t *src2, int len)
{
    int j;
    __m128i S1, S2, D;

    assert((len & 15) == 0);
  
    for (j = 0; j < len; j += 16) {
        S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
        S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
        D = _mm_avg_epu8(S1, S2);
        _mm_storeu_si128((__m128i*)(dst + j), D);
    }
}

void avg_pel_1d_sse256(pel_t *dst, pel_t *src1, pel_t *src2, int len)
{
    int j;
    

    assert((len & 15) == 0);

    if (len == 16) {
        __m128i S1, S2, D;
        S1 = _mm_loadu_si128((const __m128i*)(src1));
        S2 = _mm_loadu_si128((const __m128i*)(src2));
        D = _mm_avg_epu8(S1, S2);
        _mm_storeu_si128((__m128i*)(dst), D);
    } else {
        __m256i S1, S2, D;

        assert((len & 31) == 0);

        for (j = 0; j < len; j += 32) {
            S1 = _mm256_loadu_si256((const __m256i*)(src1 + j));
            S2 = _mm256_loadu_si256((const __m256i*)(src2 + j));
            D = _mm256_avg_epu8(S1, S2);
            _mm256_storeu_si256((__m256i*)(dst + j), D);
        }
    }
    
}

void padding_rows_sse128(pel_t *src, int i_src, int width, int height, int start, int rows, int pad)
{
    int i, j;
    pel_t *p, *p1, *p2;

    start = max(start, 0);
    rows  = min(rows, height - start);

    p = src + start * i_src;

    for (i = 0; i < rows; i++) {
        __m128i Val1 = _mm_set1_epi8((char)p[0]);
        __m128i Val2 = _mm_set1_epi8((char)p[width - 1]);
        p1 = p - pad;
        p2 = p + width;
        for (j = 0; j < pad - 15; j += 16) {
            _mm_storeu_si128((__m128i*)(p1 + j), Val1);
            _mm_storeu_si128((__m128i*)(p2 + j), Val2);
        }
        if (pad - j > 7) {
            _mm_storel_epi64((__m128i*)(p1 + j), Val1);
            _mm_storel_epi64((__m128i*)(p2 + j), Val2);
            j += 8;
        }
        if (pad - j) {
            assert(pad - j == 4);
            *(i32u_t*)(p1 + j) = _mm_cvtsi128_si32(Val1);
            *(i32u_t*)(p2 + j) = _mm_cvtsi128_si32(Val2);
        }
        p += i_src;
    }

    if (start == 0) {
        p = src - pad;
        for (i = 1; i <= pad; i++) {
            memcpy(p - i_src * i, p, (width + 2 * pad) * sizeof(pel_t));
        }
    }

    if (start + rows == height) {
        p = src + i_src * (height - 1) - pad;
        for (i = 1; i <= pad; i++) {
            memcpy(p + i_src * i, p, (width + 2 * pad) * sizeof(pel_t));
        }
    }
}

void padding_rows_sse128_10bit(pel_t *src, int i_src, int width, int height, int start, int rows, int pad)
{
    int i, j;
    pel_t *p, *p1, *p2;

    start = max(start, 0);
    rows = min(rows, height - start);

    p = src + start * i_src;

    for (i = 0; i < rows; i++) {
        __m128i Val1 = _mm_set1_epi16(p[0        ]);
        __m128i Val2 = _mm_set1_epi16(p[width - 1]);
        p1 = p - pad;
        p2 = p + width;
        for (j = 0; j < pad - 7; j += 8) {
            _mm_storeu_si128((__m128i*)(p1 + j), Val1);
            _mm_storeu_si128((__m128i*)(p2 + j), Val2);
        }
        if (pad - j > 3) {
            _mm_storel_epi64((__m128i*)(p1 + j), Val1);
            _mm_storel_epi64((__m128i*)(p2 + j), Val2);
            j += 4;
        }
        if (pad - j) {
            *(i32u_t*)(p1 + j) = _mm_cvtsi128_si32(Val1);
            *(i32u_t*)(p2 + j) = _mm_cvtsi128_si32(Val2);
        }
        p += i_src;
    }

    if (start == 0) {
        p = src - pad;
        for (i = 1; i <= pad; i++) {
            memcpy(p - i_src * i, p, (width + 2 * pad) * sizeof(pel_t));
        }
    }

    if (start + rows == height) {
        p = src + i_src * (height - 1) - pad;
        for (i = 1; i <= pad; i++) {
            memcpy(p + i_src * i, p, (width + 2 * pad) * sizeof(pel_t));
        }
    }
}

void com_mem_cpy16_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    intptr_t check = (intptr_t)src | (intptr_t)dst | i_src | i_dst; 

    if (check & 0xF) {
        while (height--) {
            _mm_storeu_si128((__m128i*)(dst), _mm_loadu_si128((__m128i*)(src)));
            src += i_src;
            dst += i_dst;
        }
    } else {
        while (height--) {
            _mm_store_si128((__m128i*)(dst), _mm_load_si128((__m128i*)(src)));
            src += i_src;
            dst += i_dst;
        }
    }
}

void com_mem_cpy32_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    uchar_t *s = (uchar_t*)src;
    uchar_t *d = (uchar_t*)dst;
    i_src *= sizeof(pel_t);
    i_dst *= sizeof(pel_t);

    while (height--) {
        _mm_storeu_si128((__m128i*)(d     ), _mm_loadu_si128((__m128i*)(s     )));
        _mm_storeu_si128((__m128i*)(d + 16), _mm_loadu_si128((__m128i*)(s + 16)));
        s += i_src;
        d += i_dst;
    }
}

void com_mem_cpy64_sse128(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    uchar_t *s = (uchar_t*)src;
    uchar_t *d = (uchar_t*)dst;
    i_src *= sizeof(pel_t);
    i_dst *= sizeof(pel_t);

    while (height--) {
        _mm_storeu_si128((__m128i*)(d     ), _mm_loadu_si128((__m128i*)(s     )));
        _mm_storeu_si128((__m128i*)(d + 16), _mm_loadu_si128((__m128i*)(s + 16)));
        _mm_storeu_si128((__m128i*)(d + 32), _mm_loadu_si128((__m128i*)(s + 32)));
        _mm_storeu_si128((__m128i*)(d + 48), _mm_loadu_si128((__m128i*)(s + 48)));
        s += i_src;
        d += i_dst;
    }
}

void com_mem_cpy64_sse128_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    uchar_t *s = (uchar_t*)src;
    uchar_t *d = (uchar_t*)dst;
    i_src *= sizeof(pel_t);
    i_dst *= sizeof(pel_t);

    while (height--) {
        _mm_storeu_si128((__m128i*)(d      ), _mm_loadu_si128((__m128i*)(s      )));
        _mm_storeu_si128((__m128i*)(d +  16), _mm_loadu_si128((__m128i*)(s +  16)));
        _mm_storeu_si128((__m128i*)(d +  32), _mm_loadu_si128((__m128i*)(s +  32)));
        _mm_storeu_si128((__m128i*)(d +  48), _mm_loadu_si128((__m128i*)(s +  48)));
        _mm_storeu_si128((__m128i*)(d +  64), _mm_loadu_si128((__m128i*)(s +  64)));
        _mm_storeu_si128((__m128i*)(d +  80), _mm_loadu_si128((__m128i*)(s +  80)));
        _mm_storeu_si128((__m128i*)(d +  96), _mm_loadu_si128((__m128i*)(s +  96)));
        _mm_storeu_si128((__m128i*)(d + 112), _mm_loadu_si128((__m128i*)(s + 112)));
        s += i_src;
        d += i_dst;
    }
}

void com_mem_cpy32_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    intptr_t check = (intptr_t)src | (intptr_t)dst | i_src | i_dst; 

    if (check & 0x1F) {
        while (height--) {
            _mm256_storeu_si256((__m256i*)(dst), _mm256_loadu_si256((__m256i*)(src)));
            src += i_src;
            dst += i_dst;
        }
    } else {
        while (height--) {
            _mm256_store_si256((__m256i*)(dst), _mm256_load_si256((__m256i*)(src)));
            src += i_src;
            dst += i_dst;
        }
    }
}

void com_mem_cpy64_sse256(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    uchar_t *s = (uchar_t*)src;
    uchar_t *d = (uchar_t*)dst;
    i_src *= sizeof(pel_t);
    i_dst *= sizeof(pel_t);

    while (height--) {
        _mm256_storeu_si256((__m256i*)(d     ), _mm256_loadu_si256((__m256i*)(s     )));
        _mm256_storeu_si256((__m256i*)(d + 32), _mm256_loadu_si256((__m256i*)(s + 32)));
        s += i_src;
        d += i_dst;
    }
}

void com_mem_cpy64_sse256_10bit(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    uchar_t *s = (uchar_t*)src;
    uchar_t *d = (uchar_t*)dst;
    i_src *= sizeof(pel_t);
    i_dst *= sizeof(pel_t);

    while (height--) {
        _mm256_storeu_si256((__m256i*)(d     ), _mm256_loadu_si256((__m256i*)(s     )));
        _mm256_storeu_si256((__m256i*)(d + 32), _mm256_loadu_si256((__m256i*)(s + 32)));
        _mm256_storeu_si256((__m256i*)(d + 64), _mm256_loadu_si256((__m256i*)(s + 64)));
        _mm256_storeu_si256((__m256i*)(d + 96), _mm256_loadu_si256((__m256i*)(s + 96)));
        s += i_src;
        d += i_dst;
    }
}

double calc_ave_sse128(pel_t* p_org, int width, int height, int iDownScale, int bit_depth)
{
    int i, j;
    int widhig = width*(height / iDownScale); // only downscale height

    int tmpSum_sse = 0;
    __m128i row1, row1High, row1Low, row2, row2High, row2Low;
    __m128i zero = _mm_setzero_si128();
    int num_16blk;

    if (width % 16 == 0){ // 16x16~64x64
        num_16blk = width / 16;
        for (i = 0; i < num_16blk; i++){
            row1 = _mm_lddqu_si128((__m128i const*)(p_org + 16 * i));
            row1Low = _mm_unpacklo_epi8(row1, zero);
            row1High = _mm_unpackhi_epi8(row1, zero);
            for (j = iDownScale; j < height; j += iDownScale){
                row2 = _mm_lddqu_si128((__m128i const*)(p_org + j * CACHE_STRIDE + 16 * i));
                row2Low = _mm_unpacklo_epi8(row2, zero);
                row2High = _mm_unpackhi_epi8(row2, zero);
                row1Low = _mm_add_epi16(row1Low, row2Low);
                row1High = _mm_add_epi16(row1High, row2High);
            }

            row1 = _mm_add_epi16(row1Low, row1High);
            row1 = _mm_hadd_epi16(row1, row1);
            row1 = _mm_hadd_epi16(row1, row1);
            row1 = _mm_hadd_epi16(row1, row1);
            tmpSum_sse += _mm_extract_epi16(row1, 0);
        }
    }
    else if (width == 8 || width == 4){ // 8x8
        row1 = _mm_lddqu_si128((__m128i const*)p_org);
        row1Low = _mm_unpacklo_epi8(row1, zero);
        for (j = iDownScale; j < height; j += iDownScale){
            row2 = _mm_lddqu_si128((__m128i const*)(p_org + j * CACHE_STRIDE));
            row2Low = _mm_unpacklo_epi8(row2, zero);
            row1Low = _mm_add_epi16(row1Low, row2Low);
        }

        for (j = 0; j < width; j++) {
            tmpSum_sse += _mm_extract_epi16(row1Low, 0);
            row1Low = _mm_srli_si128(row1Low, 2);
        }
    }
    else
        assert(0);

    return (double)tmpSum_sse / widhig;
}

double calc_var_sse128(pel_t* p_org, int width, int height, int iDownScale, double ave, int bit_depth)
{
    int i, j, N;
    int i_org = CACHE_STRIDE;
    double tmpSum = 0;
    __m128i T0, T1, sum = _mm_setzero_si128();
    __m128i zero = _mm_setzero_si128();

    height /= iDownScale;
    i_org *= iDownScale;
    N = width * height;

    if (width & 15) {
        for (j = 0; j < height; j++){
            T0 = _mm_loadl_epi64((const __m128i*)p_org);
            T0 = _mm_unpacklo_epi8(T0, zero);
            T0 = _mm_madd_epi16(T0, T0);
            sum = _mm_add_epi32(sum, T0);
            p_org += i_org;
        }
        sum = _mm_hadd_epi32(sum, sum);
        if (width == 8) {
            sum= _mm_hadd_epi32(sum, sum);
        }
        tmpSum = _mm_extract_epi32(sum, 0);
    } else {
        for (j = 0; j < height; j++){
            for (i = 0; i < width; i += 16){
                T0 = _mm_load_si128((const __m128i*)(p_org + i));

                T1 = _mm_unpackhi_epi8(T0, zero);
                T0 = _mm_unpacklo_epi8(T0, zero);

                T0 = _mm_madd_epi16(T0, T0);
                T1 = _mm_madd_epi16(T1, T1);

                sum = _mm_add_epi32(sum, T0);
                sum = _mm_add_epi32(sum, T1);
            }
            p_org += i_org;
        }
        sum = _mm_hadd_epi32(sum, sum);
        sum = _mm_hadd_epi32(sum, sum);
        tmpSum = _mm_extract_epi32(sum, 0);
    }

    return tmpSum / N - ave * ave;
}

//////////////////////////////////////////////////////////////////////////////////////
//  avg_pel
//////////////////////////////////////////////////////////////////////////////////////
void avg_pel_0_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i, j;
	__m128i S1, S2, D;
	__m128i mask = _mm_load_si128((const __m128i*)intrinsic_mask_10bit[(width & 7) - 1]);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width - 7; j += 8) {
			S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
			S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
			D = _mm_avg_epu16(S1, S2);
			_mm_storeu_si128((__m128i*)(dst + j), D);
		}
		if (j < width){
			S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
			S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
			D = _mm_avg_epu16(S1, S2);
			_mm_maskmoveu_si128(D, mask, (char_t*)&dst[j]);
		}
		src1 += i_src1;
		src2 += i_src2;
		dst += i_dst;
	}
}

void avg_pel_2_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i;
	__m128i S1, S2, D;
	i32s_t *s1 = (i32s_t *)src1;
	i32s_t *s2 = (i32s_t *)src2;
	i32s_t *d = (i32s_t *)dst;

	int i_s1 = i_src1 >> 1;
	int i_s2 = i_src2 >> 1;
	int i_d = i_dst >> 1;

	for (i = 0; i < height; i += 4) {
		S1 = _mm_setr_epi32(s1[0], s1[i_s1], s1[i_s1 << 1], s1[(i_s1 << 1) + i_s1]);
		S2 = _mm_setr_epi32(s2[0], s2[i_s2], s2[i_s2 << 1], s2[(i_s2 << 1) + i_s2]);
		D = _mm_avg_epu8(S1, S2);
		*d = _mm_extract_epi32(D, 0);
		d += i_d;
		*d = _mm_extract_epi32(D, 1);
		d += i_d;
		*d = _mm_extract_epi32(D, 2);
		d += i_d;
		*d = _mm_extract_epi32(D, 3);
		d += i_d;

		s1 += i_src1 << 1;
		s2 += i_src2 << 1;
	}
}

void avg_pel_4_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i;
	__m128i S1, S2, S3, S4;

	for (i = 0; i < height; i += 2) {
		S1 = _mm_loadl_epi64((__m128i*)src1);
		src1 += i_src1;
		S2 = _mm_loadl_epi64((__m128i*)src1);
		S1 = _mm_unpacklo_epi64(S1, S2);

		S3 = _mm_loadl_epi64((__m128i*)src2);
		src2 += i_src2;
		S4 = _mm_loadl_epi64((__m128i*)src2);
		S3 = _mm_unpacklo_epi64(S3, S4);

		S1 = _mm_avg_epu16(S1, S3);
		_mm_storel_epi64((__m128i*)dst, S1);
		S1 = _mm_srli_si128(S1, 8);
		dst += i_dst;
		_mm_storel_epi64((__m128i*)dst, S1);

		src1 += i_src1;
		src2 += i_src2;
		dst += i_dst;
	}
}

void avg_pel_8_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i;
	__m128i S1, S2, D;

	for (i = 0; i < height; i++) {
		S1 = _mm_loadu_si128((const __m128i*)src1);
		S2 = _mm_loadu_si128((const __m128i*)src2);
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)dst, D);

		src1 += i_src1;
		src2 += i_src2;
		dst += i_dst;
	}
}

void avg_pel_12_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i;
	__m128i S1, S2, D;

	for (i = 0; i < height; i++) {
		S1 = _mm_loadu_si128((const __m128i*)src1);
		S2 = _mm_loadu_si128((const __m128i*)src2);
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)dst, D);

		S1 = _mm_loadl_epi64((const __m128i*)(src1 + 8));
		S2 = _mm_loadl_epi64((const __m128i*)(src2 + 8));
		D = _mm_avg_epu16(S1, S2);
		_mm_storel_epi64((__m128i*)(dst + 8), D);

		src1 += i_src1;
		src2 += i_src2;
		dst += i_dst;
	}
}

void avg_pel_16_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i;
	__m128i S1, S2, D;

	for (i = 0; i < height; i++) {
		S1 = _mm_loadu_si128((const __m128i*)src1);
		S2 = _mm_loadu_si128((const __m128i*)src2);
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)dst, D);

		S1 = _mm_loadu_si128((const __m128i*)(src1 + 8));
		S2 = _mm_loadu_si128((const __m128i*)(src2 + 8));
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)(dst + 8), D);

		src1 += i_src1;
		src2 += i_src2;
		dst += i_dst;
	}
}

void avg_pel_24_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i;
	__m128i S1, S2, D;

	for (i = 0; i < height; i++) {
		S1 = _mm_loadu_si128((const __m128i*)src1);
		S2 = _mm_loadu_si128((const __m128i*)src2);
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)dst, D);

		S1 = _mm_loadu_si128((const __m128i*)(src1 + 8));
		S2 = _mm_loadu_si128((const __m128i*)(src2 + 8));
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)(dst + 8), D);

		S1 = _mm_loadu_si128((const __m128i*)(src1 + 16));
		S2 = _mm_loadu_si128((const __m128i*)(src2 + 16));
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)(dst + 16), D);

		src1 += i_src1;
		src2 += i_src2;
		dst += i_dst;
	}
}

void avg_pel_x16_sse128_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
	int i, j;
	__m128i S1, S2, D;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j += 8){
			S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
			S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
			D = _mm_avg_epu16(S1, S2);
			_mm_storeu_si128((__m128i*)(dst + j), D);
		}

		src1 += i_src1;
		src2 += i_src2;
		dst += i_dst;
	}
}

void avg_pel_1d_sse128_10bit(pel_t *dst, pel_t *src1, pel_t *src2, int len)
{
	int j;
	__m128i S1, S2, D;

	assert((len & 15) == 0);

	for (j = 0; j < len; j += 8) {
		S1 = _mm_loadu_si128((const __m128i*)(src1 + j));
		S2 = _mm_loadu_si128((const __m128i*)(src2 + j));
		D = _mm_avg_epu16(S1, S2);
		_mm_storeu_si128((__m128i*)(dst + j), D);
	}
}

void avg_pel_16_sse256_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m256i S1, S2, D;

    for (i = 0; i < height; i++) {
        S1 = _mm256_loadu_si256((const __m256i*)src1);
        S2 = _mm256_loadu_si256((const __m256i*)src2);
        D = _mm256_avg_epu16(S1, S2);
        _mm256_storeu_si256((__m256i*)dst, D);

        src1 += i_src1;
        src2 += i_src2;
        dst  += i_dst;
    }
}

void avg_pel_x16_sse256_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i, j;
    __m256i S1, S2, D;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j += 16) {
            S1 = _mm256_loadu_si256((const __m256i*)(src1 + j));
            S2 = _mm256_loadu_si256((const __m256i*)(src2 + j));
            D = _mm256_avg_epu16(S1, S2);
            _mm256_storeu_si256((__m256i*)(dst + j), D);
        }
        src1 += i_src1;
        src2 += i_src2;
        dst += i_dst;
    }
}

void avg_pel_1d_sse256_10bit(pel_t *dst, pel_t *src1, pel_t *src2, int len)
{
    int j;

    assert((len & 7) == 0);

    if (len == 8) {
        __m128i S1, S2, D;
        S1 = _mm_loadu_si128((const __m128i*)(src1));
        S2 = _mm_loadu_si128((const __m128i*)(src2));
        D = _mm_avg_epu16(S1, S2);
        _mm_storeu_si128((__m128i*)(dst), D);
    } else {
        __m256i S1, S2, D;

        assert((len & 15) == 0);

        for (j = 0; j < len; j += 16) {
            S1 = _mm256_loadu_si256((const __m256i*)(src1 + j));
            S2 = _mm256_loadu_si256((const __m256i*)(src2 + j));
            D = _mm256_avg_epu16(S1, S2);
            _mm256_storeu_si256((__m256i*)(dst + j), D);
        }
    }
}

double calc_ave_sse128_10bit(pel_t* p_org, int width, int height, int iDownScale, int bit_depth)
{
    int i, j, k, N;
    int i_org = CACHE_STRIDE;
    int pel_sum = 0;
    __m128i sum16, sum32 = _mm_setzero_si128();
    __m128i zero = _mm_setzero_si128();

    height /= iDownScale;
    i_org *= iDownScale;
    N = width * height;

    if (width & 7) {
        assert(width == 4);

        sum16 = _mm_setzero_si128();
        for (i = 0; i < height; i++) {
            sum16 = _mm_add_epi16(sum16, _mm_loadl_epi64((const __m128i*)p_org));
            p_org += i_org;
        }
        sum32 = _mm_cvtepu16_epi32(sum16);

        sum32 = _mm_hadd_epi32(sum32, sum32);
        sum32 = _mm_hadd_epi32(sum32, sum32);
        pel_sum = _mm_extract_epi32(sum32, 0);
    } else {
        for (i = 0; i < height - 8; i += 8) {
            sum16 = _mm_setzero_si128();
            for (k = 0; k < 8; k++) {
                for (j = 0; j < width; j += 8) {
                    sum16 = _mm_add_epi16(sum16, _mm_load_si128((const __m128i*)(p_org + j)));
                }
                p_org += i_org;
            }
            sum32 = _mm_add_epi32(sum32, _mm_cvtepu16_epi32(sum16));
            sum32 = _mm_add_epi32(sum32, _mm_unpackhi_epi16(sum16, zero));
        }

        sum16 = _mm_setzero_si128();
        for (; i < height; i++) {
            for (j = 0; j < width; j += 8) {
                sum16 = _mm_add_epi16(sum16, _mm_load_si128((const __m128i*)(p_org + j)));
            }
            p_org += i_org;
        }
        sum32 = _mm_add_epi32(sum32, _mm_cvtepu16_epi32(sum16));
        sum32 = _mm_add_epi32(sum32, _mm_unpackhi_epi16(sum16, zero));

        sum32 = _mm_hadd_epi32(sum32, sum32);
        sum32 = _mm_hadd_epi32(sum32, sum32);
        pel_sum = _mm_extract_epi32(sum32, 0);
    }

    return ((double)pel_sum) / N / (1 << (bit_depth - 8));
}

double calc_var_sse128_10bit(pel_t* p_org, int width, int height, int iDownScale, double ave, int bit_depth)
{
    int i, j, N;
    int i_org = CACHE_STRIDE;
    double tmpSum = 0;
    __m128i T0, sum = _mm_setzero_si128();

    height /= iDownScale;
    i_org *= iDownScale;
    N = width * height;

    ave *= 1 << (bit_depth - 8);

    if (width & 7) {
        assert(width == 4);
        for (j = 0; j < height; j++){
            for (i = 0; i < width; i += 8){
                T0 = _mm_loadl_epi64((const __m128i*)(p_org + i));
                T0 = _mm_madd_epi16(T0, T0);
                sum = _mm_add_epi32(sum, T0);
            }
            p_org += i_org;
        }
        sum = _mm_hadd_epi32(sum, sum);
        tmpSum = _mm_extract_epi32(sum, 0);
    } else {
        for (j = 0; j < height; j++){
            for (i = 0; i < width; i += 8){
                T0 = _mm_load_si128((const __m128i*)(p_org + i));
                T0 = _mm_madd_epi16(T0, T0);
                sum = _mm_add_epi32(sum, T0);
            }
            p_org += i_org;
        }
        sum = _mm_hadd_epi32(sum, sum);
        sum = _mm_hadd_epi32(sum, sum);
        tmpSum = _mm_extract_epi32(sum, 0);
    }

    tmpSum = tmpSum / N - ave*ave;

    return tmpSum / (1 << (bit_depth - 8)) / (1 << (bit_depth - 8));
}

void pix_sub_b4_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    i32u_t *s1 = (i32u_t *)org;
    i32u_t *s2 = (i32u_t *)pred;
    int i_s1 = i_org  >> 2;
    int i_s2 = i_pred >> 2;

    __m128i S1 = _mm_setr_epi32(s1[0], s1[i_s1], s1[i_s1 << 1], s1[(i_s1 << 1) + i_s1]);
    __m128i S2 = _mm_setr_epi32(s2[0], s2[i_s2], s2[i_s2 << 1], s2[(i_s2 << 1) + i_s2]);
    
    __m256i T1 = _mm256_cvtepu8_epi16(S1);
    __m256i T2 = _mm256_cvtepu8_epi16(S2);

    _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(T1, T2));
}

void pix_sub_b8_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    for (int i = 0; i < 4; i++) {
        __m128i S1 = _mm_unpacklo_epi64(_mm_loadl_epi64((const __m128i*) org), _mm_loadl_epi64((const __m128i*)(org  + i_org )));
        __m128i S2 = _mm_unpacklo_epi64(_mm_loadl_epi64((const __m128i*)pred), _mm_loadl_epi64((const __m128i*)(pred + i_pred)));

        __m256i T1 = _mm256_cvtepu8_epi16(S1);
        __m256i T2 = _mm256_cvtepu8_epi16(S2);

        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(T1, T2));

        dst += 16;
        org += i_org << 1;
        pred += i_pred << 1;
    }
}

void pix_sub_b16_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    for (int i = 0; i < 16; i++) {
        __m128i S1 = _mm_loadu_si128((const __m128i*)org);
        __m128i S2 = _mm_loadu_si128((const __m128i*)pred);

        __m256i T1 = _mm256_cvtepu8_epi16(S1);
        __m256i T2 = _mm256_cvtepu8_epi16(S2);

        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(T1, T2));

        dst += 16;
        org += i_org;
        pred += i_pred;
    }
}

void pix_sub_b32_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    for (int i = 0; i < 32; i++) {
        __m128i S1 = _mm_loadu_si128((const __m128i*)org);
        __m128i S2 = _mm_loadu_si128((const __m128i*)pred);
        __m128i S3 = _mm_loadu_si128((const __m128i*)(org  + 16));
        __m128i S4 = _mm_loadu_si128((const __m128i*)(pred + 16));

        __m256i T1 = _mm256_cvtepu8_epi16(S1);
        __m256i T2 = _mm256_cvtepu8_epi16(S2);
        __m256i T3 = _mm256_cvtepu8_epi16(S3);
        __m256i T4 = _mm256_cvtepu8_epi16(S4);

        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(T1, T2));
        dst  += 16;
        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(T3, T4));
        dst += 16;

        org  += i_org;
        pred += i_pred;
    }
}

void pix_add_b4_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    i32u_t *s1 = (i32u_t *)pred;
    int i_s1 = i_pred >> 2;

    __m128i S1 = _mm_setr_epi32(s1[0], s1[i_s1], s1[i_s1 << 1], s1[(i_s1 << 1) + i_s1]);

    __m256i T1 = _mm256_cvtepu8_epi16(S1);
    __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);

    T1 = _mm256_add_epi16(T1, T2);
    T1 = _mm256_packus_epi16(T1, T1);
    
    *(i32u_t *)dst = _mm256_extract_epi32(T1, 0);
    dst += i_dst;
    *(i32u_t *)dst = _mm256_extract_epi32(T1, 1);
    dst += i_dst;
    *(i32u_t *)dst = _mm256_extract_epi32(T1, 4);
    dst += i_dst;
    *(i32u_t *)dst = _mm256_extract_epi32(T1, 5);
    dst += i_dst;
}

void pix_add_b8_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    for (int i = 0; i < 4; i++) {
        __m128i S1 = _mm_unpacklo_epi64(_mm_loadl_epi64((const __m128i*)pred), _mm_loadl_epi64((const __m128i*)(pred + i_pred)));

        __m256i T1 = _mm256_cvtepu8_epi16(S1);
        __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);

        T1 = _mm256_add_epi16(T1, T2);
        T1 = _mm256_packus_epi16(T1, T1);

        *(i64u_t *)dst = _mm256_extract_epi64(T1, 0);
        dst += i_dst;
        *(i64u_t *)dst = _mm256_extract_epi64(T1, 2);
        dst += i_dst;

        resi += 16;
        pred += i_pred << 1;
    }
}

void pix_add_b16_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    for (int i = 0; i < 16; i++) {
        __m256i T1 = _mm256_cvtepu8_epi16(_mm_loadu_si128((const __m128i*)pred));
        __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);

        T1 = _mm256_add_epi16(T1, T2);
        T1 = _mm256_packus_epi16(T1, T1);

        *(i64u_t *)dst = _mm256_extract_epi64(T1, 0);
        *(i64u_t *)(dst + 8) = _mm256_extract_epi64(T1, 2);
        dst += i_dst;

        resi += 16;
        pred += i_pred;
    }
}

void pix_add_b32_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    for (int i = 0; i < 32; i++) {
        __m256i T1 = _mm256_cvtepu8_epi16(_mm_loadu_si128((const __m128i*)pred));
        __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);
        resi += 16;

        __m256i T3 = _mm256_cvtepu8_epi16(_mm_loadu_si128((const __m128i*)(pred + 16)));
        __m256i T4 = _mm256_loadu_si256((const __m256i*)resi);
        resi += 16;

        T1 = _mm256_add_epi16(T1, T2);
        T1 = _mm256_packus_epi16(T1, T1);

        T3 = _mm256_add_epi16(T3, T4);
        T3 = _mm256_packus_epi16(T3, T3);

        *(i64u_t *) dst       = _mm256_extract_epi64(T1, 0);
        *(i64u_t *)(dst + 8)  = _mm256_extract_epi64(T1, 2);
        *(i64u_t *)(dst + 16) = _mm256_extract_epi64(T3, 0);
        *(i64u_t *)(dst + 24) = _mm256_extract_epi64(T3, 2);

        dst  += i_dst;
        pred += i_pred;
    }
}

void pix_sub_b4_10bit_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    i64u_t *s1 = (i64u_t *)org;
    i64u_t *s2 = (i64u_t *)pred;
    int i_s1 = i_org  >> 2;
    int i_s2 = i_pred >> 2;

    __m256i S1 = _mm256_setr_epi64x(s1[0], s1[i_s1], s1[i_s1 << 1], s1[(i_s1 << 1) + i_s1]);
    __m256i S2 = _mm256_setr_epi64x(s2[0], s2[i_s2], s2[i_s2 << 1], s2[(i_s2 << 1) + i_s2]);

    _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(S1, S2));
}

void pix_sub_b8_10bit_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    for (int i = 0; i < 4; i++) {
        __m256i S1 = _mm256_loadu2_m128i(((const __m128i*)( org + i_org) ), ((const __m128i*)org ));
        __m256i S2 = _mm256_loadu2_m128i(((const __m128i*)(pred + i_pred)), ((const __m128i*)pred));

        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(S1, S2));

        dst += 16;
        org += i_org << 1;
        pred += i_pred << 1;
    }
}

void pix_sub_b16_10bit_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    for (int i = 0; i < 16; i++) {
        __m256i S1 = _mm256_loadu_si256((const __m256i*)org);
        __m256i S2 = _mm256_loadu_si256((const __m256i*)pred);

        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(S1, S2));

        dst += 16;
        org += i_org;
        pred += i_pred;
    }
}

void pix_sub_b32_10bit_sse256(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    for (int i = 0; i < 32; i++) {
        __m256i T1 = _mm256_loadu_si256((const __m256i*)org);
        __m256i T2 = _mm256_loadu_si256((const __m256i*)pred);
        __m256i T3 = _mm256_loadu_si256((const __m256i*)(org  + 16));
        __m256i T4 = _mm256_loadu_si256((const __m256i*)(pred + 16));

        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(T1, T2));
        dst  += 16;
        _mm256_store_si256((__m256i*)dst, _mm256_sub_epi16(T3, T4));
        dst += 16;

        org  += i_org;
        pred += i_pred;
    }
}

void pix_add_b4_10bit_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    i64u_t *s1 = (i64u_t *)pred;
    int i_s1 = i_pred >> 2;
    short vmax = (1 << (bit_depth)) - 1;
    __m256i max_val = _mm256_set1_epi16(vmax);
    __m256i min_val = _mm256_setzero_si256();
     
    __m256i T1 = _mm256_setr_epi64x(s1[0], s1[i_s1], s1[i_s1 << 1], s1[(i_s1 << 1) + i_s1]);
    __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);

    T1 = _mm256_add_epi16(T1, T2);
    T1 = _mm256_min_epi16(T1, max_val);
    T1 = _mm256_max_epi16(T1, min_val);

    *(i64u_t *)dst = _mm256_extract_epi64(T1, 0);
    dst += i_dst;
    *(i64u_t *)dst = _mm256_extract_epi64(T1, 1);
    dst += i_dst;
    *(i64u_t *)dst = _mm256_extract_epi64(T1, 2);
    dst += i_dst;
    *(i64u_t *)dst = _mm256_extract_epi64(T1, 3);
    dst += i_dst;
}

void pix_add_b8_10bit_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    short vmax = (1 << (bit_depth)) - 1;
    __m256i max_val = _mm256_set1_epi16(vmax);
    __m256i min_val = _mm256_setzero_si256();

    for (int i = 0; i < 4; i++) {
        __m256i T1 = _mm256_loadu2_m128i(((const __m128i*)(pred + i_pred)), ((const __m128i*)pred));
        __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);

        T1 = _mm256_add_epi16(T1, T2);
        T1 = _mm256_min_epi16(T1, max_val);
        T1 = _mm256_max_epi16(T1, min_val);

        _mm256_storeu2_m128i((__m128i*)(dst + i_dst), (__m128i*)dst, T1);

        dst += i_dst << 1;
        resi += 16;
        pred += i_pred << 1;
    }
}

void pix_add_b16_10bit_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    short vmax = (1 << (bit_depth)) - 1;
    __m256i max_val = _mm256_set1_epi16(vmax);
    __m256i min_val = _mm256_setzero_si256();

    for (int i = 0; i < 16; i++) {
        __m256i T1 = _mm256_loadu_si256((const __m256i*)pred);
        __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);

        T1 = _mm256_add_epi16(T1, T2);
        T1 = _mm256_min_epi16(T1, max_val);
        T1 = _mm256_max_epi16(T1, min_val);

        _mm256_storeu_si256((__m256i*)dst, T1);

        dst += i_dst;
        resi += 16;
        pred += i_pred;
    }
}

void pix_add_b32_10bit_sse256(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth)
{
    short vmax = (1 << (bit_depth)) - 1;
    __m256i max_val = _mm256_set1_epi16(vmax);
    __m256i min_val = _mm256_setzero_si256();

    for (int i = 0; i < 32; i++) {
        __m256i T1 = _mm256_loadu_si256((const __m256i*)pred);
        __m256i T2 = _mm256_loadu_si256((const __m256i*)resi);
        resi += 16;
        
        __m256i T3 = _mm256_loadu_si256((const __m256i*)(pred + 16));
        __m256i T4 = _mm256_loadu_si256((const __m256i*)resi);
        resi += 16;

        T1 = _mm256_add_epi16(T1, T2);
        T1 = _mm256_min_epi16(T1, max_val);
        T1 = _mm256_max_epi16(T1, min_val);

        T3 = _mm256_add_epi16(T3, T4);
        T3 = _mm256_min_epi16(T3, max_val);
        T3 = _mm256_max_epi16(T3, min_val);

        _mm256_storeu_si256((__m256i*)dst, T1);
        _mm256_storeu_si256((__m256i*)(dst + 16), T3);

        dst  += i_dst;
        pred += i_pred;
    }
}