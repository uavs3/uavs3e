#include "intrinsic.h"

#if ENABLE_AVX512

#include <zmmintrin.h>

int quant_normal_sse512(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift)
{
    int i;
    __m512i S1, D1;
    __m512i T1 = _mm512_set1_epi32(Q);
    __m512i T2 = _mm512_set1_epi32(qp_const);
    __m512i zero = _mm512_setzero_si512();
    __mmask16  mask;
    __m512i sum = _mm512_setzero_si512();

    for (i = 0; i < coef_num; i += 16) {
        S1 = _mm512_cvtepi16_epi32(_mm256_load_si256((const __m256i*)(curr_blk + i)));
        mask = _mm512_cmpgt_epi32_mask(zero, S1);
        S1 = _mm512_abs_epi32(S1);
        D1 = _mm512_mullo_epi32(S1, T1);
        D1 = _mm512_add_epi32(D1, T2);
        D1 = _mm512_srai_epi32(D1, shift);
        sum = _mm512_add_epi32(sum, D1);
        S1 = _mm512_sub_epi32(zero, D1);
        D1 = _mm512_mask_blend_epi32(mask, D1, S1);
        _mm256_store_si256((__m256i*)(curr_blk + i), _mm512_cvtepi32_epi16(D1));
    }
    return _mm512_test_epi32_mask(sum, sum);
}

void inv_quant_normal_sse512(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift)
{
    int i;
    __m512i S1, D1;
    __m512i T1 = _mm512_set1_epi32(QPI);
    __m512i T2 = _mm512_set1_epi32(1 << (shift - 1));

    for (i = 0; i < coef_num; i += 16) {
        S1 = _mm512_cvtepi16_epi32(_mm256_load_si256((const __m256i*)(src + i)));
        D1 = _mm512_mullo_epi32(S1, T1);
        D1 = _mm512_add_epi32(D1, T2);
        D1 = _mm512_srai_epi32(D1, shift);
        _mm256_store_si256((__m256i*)(dst + i), _mm512_cvtepi32_epi16(D1));
    }
}

void xGetSAD32_x4_sse512_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    __m512i sum0 = _mm512_setzero_si512();
    __m512i sum1 = _mm512_setzero_si512();
    __m512i sum2 = _mm512_setzero_si512();
    __m512i sum3 = _mm512_setzero_si512();
    __m512i ZERO = _mm512_setzero_si512();
    __m512i T00, T01;
    __m512i T10, T11;
    __m512i T20, T21;
    __m512i T30, T31;
    __m512i T40;
    __m256i R0, R1, R2, R3;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm512_load_si512((__m512i*)(p_org));
        T10 = _mm512_loadu_si512((__m512i*)(pred0));
        T20 = _mm512_loadu_si512((__m512i*)(pred1));
        T30 = _mm512_loadu_si512((__m512i*)(pred2));
        T40 = _mm512_loadu_si512((__m512i*)(pred3));

        T10 = _mm512_sub_epi16(T00, T10);
        T10 = _mm512_abs_epi16(T10);
        sum0 = _mm512_add_epi16(sum0, T10);

        T20 = _mm512_sub_epi16(T00, T20);
        T20 = _mm512_abs_epi16(T20);
        sum1 = _mm512_add_epi16(sum1, T20);

        T30 = _mm512_sub_epi16(T00, T30);
        T30 = _mm512_abs_epi16(T30);
        sum2 = _mm512_add_epi16(sum2, T30);

        T40 = _mm512_sub_epi16(T00, T40);
        T40 = _mm512_abs_epi16(T40);
        sum3 = _mm512_add_epi16(sum3, T40);

        p_org += i_org;
        pred0 += i_pred;
        pred1 += i_pred;
        pred2 += i_pred;
        pred3 += i_pred;
    }

    T00 = _mm512_unpacklo_epi16(sum0, ZERO);
    T01 = _mm512_unpackhi_epi16(sum0, ZERO);

    T10 = _mm512_unpacklo_epi16(sum1, ZERO);
    T11 = _mm512_unpackhi_epi16(sum1, ZERO);

    T20 = _mm512_unpacklo_epi16(sum2, ZERO);
    T21 = _mm512_unpackhi_epi16(sum2, ZERO);

    T30 = _mm512_unpacklo_epi16(sum3, ZERO);
    T31 = _mm512_unpackhi_epi16(sum3, ZERO);

    sum0 = _mm512_add_epi32(T00, T01);
    sum1 = _mm512_add_epi32(T10, T11);
    sum2 = _mm512_add_epi32(T20, T21);
    sum3 = _mm512_add_epi32(T30, T31);

    R0 = _mm256_add_epi32(_mm512_castsi512_si256(sum0), _mm512_extracti64x4_epi64(sum0, 1));
    R1 = _mm256_add_epi32(_mm512_castsi512_si256(sum1), _mm512_extracti64x4_epi64(sum1, 1));
    R2 = _mm256_add_epi32(_mm512_castsi512_si256(sum2), _mm512_extracti64x4_epi64(sum2, 1));
    R3 = _mm256_add_epi32(_mm512_castsi512_si256(sum3), _mm512_extracti64x4_epi64(sum3, 1));

    R0 = _mm256_hadd_epi32(R0, R1);
    R1 = _mm256_hadd_epi32(R2, R3);
    R0 = _mm256_hadd_epi32(R0, R1);

    sad[0] = (((i32u_t)_mm256_extract_epi32(R0, 0)) + ((i32u_t)_mm256_extract_epi32(R0, 4))) << skip_lines;
    sad[1] = (((i32u_t)_mm256_extract_epi32(R0, 1)) + ((i32u_t)_mm256_extract_epi32(R0, 5))) << skip_lines;
    sad[2] = (((i32u_t)_mm256_extract_epi32(R0, 2)) + ((i32u_t)_mm256_extract_epi32(R0, 6))) << skip_lines;
    sad[3] = (((i32u_t)_mm256_extract_epi32(R0, 3)) + ((i32u_t)_mm256_extract_epi32(R0, 7))) << skip_lines;
}

i32u_t xGetSAD32_sse512_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    __m512i sum0 = _mm512_setzero_si512();
    __m512i ZERO = _mm512_setzero_si512();
    __m512i T00, T10;
    __m256i R0;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm512_load_si512((__m512i*)(p_org));
        T10 = _mm512_loadu_si512((__m512i*)(p_pred));

        T10 = _mm512_sub_epi16(T00, T10);
        T10 = _mm512_abs_epi16(T10);
        sum0 = _mm512_add_epi16(sum0, T10);

        p_org += i_org;
        p_pred += i_pred;
    }

    T00 = _mm512_unpacklo_epi16(sum0, ZERO);
    T10 = _mm512_unpackhi_epi16(sum0, ZERO);

    sum0 = _mm512_add_epi32(T00, T10);

    R0 = _mm256_add_epi32(_mm512_castsi512_si256(sum0), _mm512_extracti64x4_epi64(sum0, 1));

    R0 = _mm256_hadd_epi32(R0, R0);
    R0 = _mm256_hadd_epi32(R0, R0);

    return (((i32u_t)_mm256_extract_epi32(R0, 0)) + ((i32u_t)_mm256_extract_epi32(R0, 4))) << skip_lines;
}

i32u_t xCalcHAD16x16_sse512_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int uiSum = 0;

    __m512i R0, R1, R2, R3;
    __m512i T0, T1, T2, T3, T4, T5, T6, T7;
    __m512i M0, M1, M2, M3, M4, M5, M6, M7;
    __m512i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i sum0, sum1;
    pel_t * p_org1 = p_org + i_org * 8;
    pel_t * p_pred1 = p_pred + i_pred * 8;

    R0 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_org)), _mm256_loadu_si256((__m256i const*)p_org1), 1);
    R1 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_pred)), _mm256_loadu_si256((__m256i const*)p_pred1), 1);
    R2 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_org + i_org))), _mm256_loadu_si256((__m256i const*)(p_org1 + i_org)), 1);
    R3 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_pred + i_pred))), _mm256_loadu_si256((__m256i const*)(p_pred1 + i_pred)), 1);
    M0 = _mm512_sub_epi16(R0, R1);
    M1 = _mm512_sub_epi16(R2, R3);
    p_pred += (i_pred << 1);
    p_org += (i_org << 1);
    p_pred1 += (i_pred << 1);
    p_org1 += (i_org << 1);

    R0 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_org)), _mm256_loadu_si256((__m256i const*)p_org1), 1);
    R1 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_pred)), _mm256_loadu_si256((__m256i const*)p_pred1), 1);
    R2 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_org + i_org))), _mm256_loadu_si256((__m256i const*)(p_org1 + i_org)), 1);
    R3 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_pred + i_pred))), _mm256_loadu_si256((__m256i const*)(p_pred1 + i_pred)), 1);
    M2 = _mm512_sub_epi16(R0, R1);
    M3 = _mm512_sub_epi16(R2, R3);
    p_pred += (i_pred << 1);
    p_org += (i_org << 1);
    p_pred1 += (i_pred << 1);
    p_org1 += (i_org << 1);

    R0 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_org)), _mm256_loadu_si256((__m256i const*)p_org1), 1);
    R1 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_pred)), _mm256_loadu_si256((__m256i const*)p_pred1), 1);
    R2 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_org + i_org))), _mm256_loadu_si256((__m256i const*)(p_org1 + i_org)), 1);
    R3 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_pred + i_pred))), _mm256_loadu_si256((__m256i const*)(p_pred1 + i_pred)), 1);
    M4 = _mm512_sub_epi16(R0, R1);
    M5 = _mm512_sub_epi16(R2, R3);
    p_pred += (i_pred << 1);
    p_org += (i_org << 1);
    p_pred1 += (i_pred << 1);
    p_org1 += (i_org << 1);

    R0 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_org)), _mm256_loadu_si256((__m256i const*)p_org1), 1);
    R1 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)p_pred)), _mm256_loadu_si256((__m256i const*)p_pred1), 1);
    R2 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_org + i_org))), _mm256_loadu_si256((__m256i const*)(p_org1 + i_org)), 1);
    R3 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((__m256i const*)(p_pred + i_pred))), _mm256_loadu_si256((__m256i const*)(p_pred1 + i_pred)), 1);
    M6 = _mm512_sub_epi16(R0, R1);
    M7 = _mm512_sub_epi16(R2, R3);

    // vertical
    T0 = _mm512_add_epi16(M0, M4);
    T1 = _mm512_add_epi16(M1, M5);
    T2 = _mm512_add_epi16(M2, M6);
    T3 = _mm512_add_epi16(M3, M7);
    T4 = _mm512_sub_epi16(M0, M4);
    T5 = _mm512_sub_epi16(M1, M5);
    T6 = _mm512_sub_epi16(M2, M6);
    T7 = _mm512_sub_epi16(M3, M7);

    M0 = _mm512_add_epi16(T0, T2);
    M1 = _mm512_add_epi16(T1, T3);
    M2 = _mm512_sub_epi16(T0, T2);
    M3 = _mm512_sub_epi16(T1, T3);
    M4 = _mm512_add_epi16(T4, T6);
    M5 = _mm512_add_epi16(T5, T7);
    M6 = _mm512_sub_epi16(T4, T6);
    M7 = _mm512_sub_epi16(T5, T7);

    T0 = _mm512_add_epi16(M0, M1);
    T7 = _mm512_sub_epi16(M0, M1);
    T3 = _mm512_add_epi16(M2, M3);
    T4 = _mm512_sub_epi16(M2, M3);
    T1 = _mm512_add_epi16(M4, M5);
    T6 = _mm512_sub_epi16(M4, M5);
    T2 = _mm512_add_epi16(M6, M7);
    T5 = _mm512_sub_epi16(M6, M7);

    //transpose
    R0 = _mm512_unpacklo_epi16(T0, T1);
    R1 = _mm512_unpacklo_epi16(T2, T3);
    R2 = _mm512_unpacklo_epi16(T4, T5);
    R3 = _mm512_unpacklo_epi16(T6, T7);

    M4 = _mm512_unpacklo_epi32(R0, R1);
    M5 = _mm512_unpackhi_epi32(R0, R1);
    M6 = _mm512_unpacklo_epi32(R2, R3);
    M7 = _mm512_unpackhi_epi32(R2, R3);

    M0 = _mm512_unpacklo_epi64(M4, M6);
    M1 = _mm512_unpackhi_epi64(M4, M6);
    M2 = _mm512_unpacklo_epi64(M5, M7);
    M3 = _mm512_unpackhi_epi64(M5, M7);

    R0 = _mm512_unpackhi_epi16(T0, T1);
    R1 = _mm512_unpackhi_epi16(T2, T3);
    R2 = _mm512_unpackhi_epi16(T4, T5);
    R3 = _mm512_unpackhi_epi16(T6, T7);

    T4 = _mm512_unpacklo_epi32(R0, R1);
    T5 = _mm512_unpackhi_epi32(R0, R1);
    T6 = _mm512_unpacklo_epi32(R2, R3);
    T7 = _mm512_unpackhi_epi32(R2, R3);

    M4 = _mm512_unpacklo_epi64(T4, T6);
    M5 = _mm512_unpackhi_epi64(T4, T6);
    M6 = _mm512_unpacklo_epi64(T5, T7);
    M7 = _mm512_unpackhi_epi64(T5, T7);

    // vertical
    T0 = _mm512_add_epi16(M0, M4);
    T1 = _mm512_add_epi16(M1, M5);
    T2 = _mm512_add_epi16(M2, M6);
    T3 = _mm512_add_epi16(M3, M7);
    T4 = _mm512_sub_epi16(M0, M4);
    T5 = _mm512_sub_epi16(M1, M5);
    T6 = _mm512_sub_epi16(M2, M6);
    T7 = _mm512_sub_epi16(M3, M7);

    M0 = _mm512_add_epi16(T0, T2);
    M1 = _mm512_add_epi16(T1, T3);
    M2 = _mm512_sub_epi16(T0, T2);
    M3 = _mm512_sub_epi16(T1, T3);
    M4 = _mm512_add_epi16(T4, T6);
    M5 = _mm512_add_epi16(T5, T7);
    M6 = _mm512_sub_epi16(T4, T6);
    M7 = _mm512_sub_epi16(T5, T7);

    T0 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M0));
    T1 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M0, 1));
    T2 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M1));
    T3 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M1, 1));
    T4 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M2));
    T5 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M2, 1));
    T6 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M3));
    T7 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M3, 1));

    S0 = _mm512_abs_epi32(_mm512_add_epi32(T0, T2));
    S7 = _mm512_abs_epi32(_mm512_sub_epi32(T0, T2));
    S3 = _mm512_abs_epi32(_mm512_add_epi32(T1, T3));
    S4 = _mm512_abs_epi32(_mm512_sub_epi32(T1, T3));
    S1 = _mm512_abs_epi32(_mm512_add_epi32(T4, T6));
    S6 = _mm512_abs_epi32(_mm512_sub_epi32(T4, T6));
    S2 = _mm512_abs_epi32(_mm512_add_epi32(T5, T7));
    S5 = _mm512_abs_epi32(_mm512_sub_epi32(T5, T7));

    T0 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M4));
    T1 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M4, 1));
    T2 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M5));
    T3 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M5, 1));
    T4 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M6));
    T5 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M6, 1));
    T6 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M7));
    T7 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M7, 1));

    M0 = _mm512_abs_epi32(_mm512_add_epi32(T0, T2));
    M7 = _mm512_abs_epi32(_mm512_sub_epi32(T0, T2));
    M3 = _mm512_abs_epi32(_mm512_add_epi32(T1, T3));
    M4 = _mm512_abs_epi32(_mm512_sub_epi32(T1, T3));
    M1 = _mm512_abs_epi32(_mm512_add_epi32(T4, T6));
    M6 = _mm512_abs_epi32(_mm512_sub_epi32(T4, T6));
    M2 = _mm512_abs_epi32(_mm512_add_epi32(T5, T7));
    M5 = _mm512_abs_epi32(_mm512_sub_epi32(T5, T7));

    T0 = _mm512_add_epi32(M0, S0);
    T1 = _mm512_add_epi32(M1, S1);
    T2 = _mm512_add_epi32(M2, S2);
    T3 = _mm512_add_epi32(M3, S3);
    T4 = _mm512_add_epi32(M4, S4);
    T5 = _mm512_add_epi32(M5, S5);
    T6 = _mm512_add_epi32(M6, S6);
    T7 = _mm512_add_epi32(M7, S7);

    T0 = _mm512_add_epi32(T0, T1);
    T2 = _mm512_add_epi32(T2, T3);
    T4 = _mm512_add_epi32(T4, T5);
    T6 = _mm512_add_epi32(T6, T7);

    T0 = _mm512_add_epi32(T0, T6);
    T2 = _mm512_add_epi32(T2, T4);

    sum0 = _mm256_hadd_epi32(_mm512_castsi512_si256(T0), _mm512_castsi512_si256(T2));
    sum1 = _mm256_hadd_epi32(_mm512_extracti64x4_epi64(T0, 1), _mm512_extracti64x4_epi64(T2, 1));

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum1 = _mm256_hadd_epi32(sum1, sum1);
    uiSum += ((_mm256_extract_epi32(sum0, 0) + _mm256_extract_epi32(sum0, 4) + 2) >> 2) +
        ((_mm256_extract_epi32(sum0, 1) + _mm256_extract_epi32(sum0, 5) + 2) >> 2);
    uiSum += ((_mm256_extract_epi32(sum1, 0) + _mm256_extract_epi32(sum1, 4) + 2) >> 2) +
        ((_mm256_extract_epi32(sum1, 1) + _mm256_extract_epi32(sum1, 5) + 2) >> 2);

    return uiSum;
}

i32u_t xCalcHAD32x32_sse512_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int i, uiSum = 0;
    for (i = 0; i < 4; i++) {
        __m512i R0, R1, R2, R3;
        __m512i T0, T1, T2, T3, T4, T5, T6, T7;
        __m512i M0, M1, M2, M3, M4, M5, M6, M7;
        __m512i S0, S1, S2, S3, S4, S5, S6, S7;
        __m256i sum0, sum1;

        R0 = _mm512_loadu_si512((__m512i const*)p_org);
        R1 = _mm512_loadu_si512((__m512i const*)p_pred);
        R2 = _mm512_loadu_si512((__m512i const*)(p_org + i_org));
        R3 = _mm512_loadu_si512((__m512i const*)(p_pred + i_pred));
        M0 = _mm512_sub_epi16(R0, R1);
        M1 = _mm512_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);


        R0 = _mm512_loadu_si512((__m512i const*)p_org);
        R1 = _mm512_loadu_si512((__m512i const*)p_pred);
        R2 = _mm512_loadu_si512((__m512i const*)(p_org + i_org));
        R3 = _mm512_loadu_si512((__m512i const*)(p_pred + i_pred));
        M2 = _mm512_sub_epi16(R0, R1);
        M3 = _mm512_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);

        R0 = _mm512_loadu_si512((__m512i const*)p_org);
        R1 = _mm512_loadu_si512((__m512i const*)p_pred);
        R2 = _mm512_loadu_si512((__m512i const*)(p_org + i_org));
        R3 = _mm512_loadu_si512((__m512i const*)(p_pred + i_pred));
        M4 = _mm512_sub_epi16(R0, R1);
        M5 = _mm512_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);

        R0 = _mm512_loadu_si512((__m512i const*)p_org);
        R1 = _mm512_loadu_si512((__m512i const*)p_pred);
        R2 = _mm512_loadu_si512((__m512i const*)(p_org + i_org));
        R3 = _mm512_loadu_si512((__m512i const*)(p_pred + i_pred));
        M6 = _mm512_sub_epi16(R0, R1);
        M7 = _mm512_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);

        // vertical
        T0 = _mm512_add_epi16(M0, M4);
        T1 = _mm512_add_epi16(M1, M5);
        T2 = _mm512_add_epi16(M2, M6);
        T3 = _mm512_add_epi16(M3, M7);
        T4 = _mm512_sub_epi16(M0, M4);
        T5 = _mm512_sub_epi16(M1, M5);
        T6 = _mm512_sub_epi16(M2, M6);
        T7 = _mm512_sub_epi16(M3, M7);

        M0 = _mm512_add_epi16(T0, T2);
        M1 = _mm512_add_epi16(T1, T3);
        M2 = _mm512_sub_epi16(T0, T2);
        M3 = _mm512_sub_epi16(T1, T3);
        M4 = _mm512_add_epi16(T4, T6);
        M5 = _mm512_add_epi16(T5, T7);
        M6 = _mm512_sub_epi16(T4, T6);
        M7 = _mm512_sub_epi16(T5, T7);

        T0 = _mm512_add_epi16(M0, M1);
        T7 = _mm512_sub_epi16(M0, M1);
        T3 = _mm512_add_epi16(M2, M3);
        T4 = _mm512_sub_epi16(M2, M3);
        T1 = _mm512_add_epi16(M4, M5);
        T6 = _mm512_sub_epi16(M4, M5);
        T2 = _mm512_add_epi16(M6, M7);
        T5 = _mm512_sub_epi16(M6, M7);

        //transpose
        R0 = _mm512_unpacklo_epi16(T0, T1);
        R1 = _mm512_unpacklo_epi16(T2, T3);
        R2 = _mm512_unpacklo_epi16(T4, T5);
        R3 = _mm512_unpacklo_epi16(T6, T7);

        M4 = _mm512_unpacklo_epi32(R0, R1);
        M5 = _mm512_unpackhi_epi32(R0, R1);
        M6 = _mm512_unpacklo_epi32(R2, R3);
        M7 = _mm512_unpackhi_epi32(R2, R3);

        M0 = _mm512_unpacklo_epi64(M4, M6);
        M1 = _mm512_unpackhi_epi64(M4, M6);
        M2 = _mm512_unpacklo_epi64(M5, M7);
        M3 = _mm512_unpackhi_epi64(M5, M7);

        R0 = _mm512_unpackhi_epi16(T0, T1);
        R1 = _mm512_unpackhi_epi16(T2, T3);
        R2 = _mm512_unpackhi_epi16(T4, T5);
        R3 = _mm512_unpackhi_epi16(T6, T7);

        T4 = _mm512_unpacklo_epi32(R0, R1);
        T5 = _mm512_unpackhi_epi32(R0, R1);
        T6 = _mm512_unpacklo_epi32(R2, R3);
        T7 = _mm512_unpackhi_epi32(R2, R3);

        M4 = _mm512_unpacklo_epi64(T4, T6);
        M5 = _mm512_unpackhi_epi64(T4, T6);
        M6 = _mm512_unpacklo_epi64(T5, T7);
        M7 = _mm512_unpackhi_epi64(T5, T7);

        // vertical
        T0 = _mm512_add_epi16(M0, M4);
        T1 = _mm512_add_epi16(M1, M5);
        T2 = _mm512_add_epi16(M2, M6);
        T3 = _mm512_add_epi16(M3, M7);
        T4 = _mm512_sub_epi16(M0, M4);
        T5 = _mm512_sub_epi16(M1, M5);
        T6 = _mm512_sub_epi16(M2, M6);
        T7 = _mm512_sub_epi16(M3, M7);

        M0 = _mm512_add_epi16(T0, T2);
        M1 = _mm512_add_epi16(T1, T3);
        M2 = _mm512_sub_epi16(T0, T2);
        M3 = _mm512_sub_epi16(T1, T3);
        M4 = _mm512_add_epi16(T4, T6);
        M5 = _mm512_add_epi16(T5, T7);
        M6 = _mm512_sub_epi16(T4, T6);
        M7 = _mm512_sub_epi16(T5, T7);

        T0 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M0));
        T1 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M0, 1));
        T2 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M1));
        T3 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M1, 1));
        T4 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M2));
        T5 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M2, 1));
        T6 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M3));
        T7 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M3, 1));

        S0 = _mm512_abs_epi32(_mm512_add_epi32(T0, T2));
        S7 = _mm512_abs_epi32(_mm512_sub_epi32(T0, T2));
        S3 = _mm512_abs_epi32(_mm512_add_epi32(T1, T3));
        S4 = _mm512_abs_epi32(_mm512_sub_epi32(T1, T3));
        S1 = _mm512_abs_epi32(_mm512_add_epi32(T4, T6));
        S6 = _mm512_abs_epi32(_mm512_sub_epi32(T4, T6));
        S2 = _mm512_abs_epi32(_mm512_add_epi32(T5, T7));
        S5 = _mm512_abs_epi32(_mm512_sub_epi32(T5, T7));

        T0 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M4));
        T1 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M4, 1));
        T2 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M5));
        T3 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M5, 1));
        T4 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M6));
        T5 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M6, 1));
        T6 = _mm512_cvtepi16_epi32(_mm512_castsi512_si256(M7));
        T7 = _mm512_cvtepi16_epi32(_mm512_extracti64x4_epi64(M7, 1));

        M0 = _mm512_abs_epi32(_mm512_add_epi32(T0, T2));
        M7 = _mm512_abs_epi32(_mm512_sub_epi32(T0, T2));
        M3 = _mm512_abs_epi32(_mm512_add_epi32(T1, T3));
        M4 = _mm512_abs_epi32(_mm512_sub_epi32(T1, T3));
        M1 = _mm512_abs_epi32(_mm512_add_epi32(T4, T6));
        M6 = _mm512_abs_epi32(_mm512_sub_epi32(T4, T6));
        M2 = _mm512_abs_epi32(_mm512_add_epi32(T5, T7));
        M5 = _mm512_abs_epi32(_mm512_sub_epi32(T5, T7));

        T0 = _mm512_add_epi32(M0, S0);
        T1 = _mm512_add_epi32(M1, S1);
        T2 = _mm512_add_epi32(M2, S2);
        T3 = _mm512_add_epi32(M3, S3);
        T4 = _mm512_add_epi32(M4, S4);
        T5 = _mm512_add_epi32(M5, S5);
        T6 = _mm512_add_epi32(M6, S6);
        T7 = _mm512_add_epi32(M7, S7);

        T0 = _mm512_add_epi32(T0, T1);
        T2 = _mm512_add_epi32(T2, T3);
        T4 = _mm512_add_epi32(T4, T5);
        T6 = _mm512_add_epi32(T6, T7);

        T0 = _mm512_add_epi32(T0, T6);
        T2 = _mm512_add_epi32(T2, T4);

        sum0 = _mm256_hadd_epi32(_mm512_castsi512_si256(T0), _mm512_castsi512_si256(T2));
        sum1 = _mm256_hadd_epi32(_mm512_extracti64x4_epi64(T0, 1), _mm512_extracti64x4_epi64(T2, 1));

        sum0 = _mm256_hadd_epi32(sum0, sum0);
        sum1 = _mm256_hadd_epi32(sum1, sum1);
        uiSum += ((_mm256_extract_epi32(sum0, 0) + _mm256_extract_epi32(sum0, 4) + 2) >> 2) +
            ((_mm256_extract_epi32(sum0, 1) + _mm256_extract_epi32(sum0, 5) + 2) >> 2);
        uiSum += ((_mm256_extract_epi32(sum1, 0) + _mm256_extract_epi32(sum1, 4) + 2) >> 2) +
            ((_mm256_extract_epi32(sum1, 1) + _mm256_extract_epi32(sum1, 5) + 2) >> 2);

    }

    return uiSum;
}

void com_if_filter_ver_8_ext_sse512_10bit(const i16s_t *src, int i_src, pel_t *dst[3], int i_dst, int width, int height, tab_char_t(*coeff)[8], int bit_depth)
{
    int i, j;
    int shift1 = 20 - bit_depth;
    int add1 = 1 << (shift1 - 1);
    pel_t *d0 = dst[0];
    pel_t *d1 = dst[1];
    pel_t *d2 = dst[2];
    int max_pixel = (1 << bit_depth) - 1;
    __m512i max_val = _mm512_set1_epi16((pel_t)max_pixel);
    __m512i mAddOffset = _mm512_set1_epi32(add1);
    __m512i T0, T1, T2, T3, T4, T5, T6, T7;
    __m512i M0, M1, M2, M3, M4, M5, M6, M7;
    __m512i N0, N1, N2, N3, N4, N5, N6, N7;

    __m256i coeff0 = _mm256_set1_epi16(*(i16s_t*)coeff[0]);
    __m256i coeff1 = _mm256_set1_epi16(*(i16s_t*)(coeff[0] + 2));
    __m256i coeff2 = _mm256_set1_epi16(*(i16s_t*)(coeff[0] + 4));
    __m256i coeff3 = _mm256_set1_epi16(*(i16s_t*)(coeff[0] + 6));
    __m512i coeff00 = _mm512_cvtepi8_epi16(coeff0);
    __m512i coeff01 = _mm512_cvtepi8_epi16(coeff1);
    __m512i coeff02 = _mm512_cvtepi8_epi16(coeff2);
    __m512i coeff03 = _mm512_cvtepi8_epi16(coeff3);

    coeff0 = _mm256_set1_epi16(*(i16s_t*)coeff[1]);
    coeff1 = _mm256_set1_epi16(*(i16s_t*)(coeff[1] + 2));
    coeff2 = _mm256_set1_epi16(*(i16s_t*)(coeff[1] + 4));
    coeff3 = _mm256_set1_epi16(*(i16s_t*)(coeff[1] + 6));
    __m512i coeff10 = _mm512_cvtepi8_epi16(coeff0);
    __m512i coeff11 = _mm512_cvtepi8_epi16(coeff1);
    __m512i coeff12 = _mm512_cvtepi8_epi16(coeff2);
    __m512i coeff13 = _mm512_cvtepi8_epi16(coeff3);

    coeff0 = _mm256_set1_epi16(*(i16s_t*)coeff[2]);
    coeff1 = _mm256_set1_epi16(*(i16s_t*)(coeff[2] + 2));
    coeff2 = _mm256_set1_epi16(*(i16s_t*)(coeff[2] + 4));
    coeff3 = _mm256_set1_epi16(*(i16s_t*)(coeff[2] + 6));
    __m512i coeff20 = _mm512_cvtepi8_epi16(coeff0);
    __m512i coeff21 = _mm512_cvtepi8_epi16(coeff1);
    __m512i coeff22 = _mm512_cvtepi8_epi16(coeff2);
    __m512i coeff23 = _mm512_cvtepi8_epi16(coeff3);


    src -= 3 * i_src;

    for (j = 0; j < height; j++) {
        const i16s_t *p = src;
        for (i = 0; i < width; i += 32) {
            T0 = _mm512_loadu_si512((__m512i*)(p));
            T1 = _mm512_loadu_si512((__m512i*)(p + i_src));
            T2 = _mm512_loadu_si512((__m512i*)(p + 2 * i_src));
            T3 = _mm512_loadu_si512((__m512i*)(p + 3 * i_src));
            T4 = _mm512_loadu_si512((__m512i*)(p + 4 * i_src));
            T5 = _mm512_loadu_si512((__m512i*)(p + 5 * i_src));
            T6 = _mm512_loadu_si512((__m512i*)(p + 6 * i_src));
            T7 = _mm512_loadu_si512((__m512i*)(p + 7 * i_src));

            M0 = _mm512_unpacklo_epi16(T0, T1);
            M1 = _mm512_unpacklo_epi16(T2, T3);
            M2 = _mm512_unpacklo_epi16(T4, T5);
            M3 = _mm512_unpacklo_epi16(T6, T7);
            M4 = _mm512_unpackhi_epi16(T0, T1);
            M5 = _mm512_unpackhi_epi16(T2, T3);
            M6 = _mm512_unpackhi_epi16(T4, T5);
            M7 = _mm512_unpackhi_epi16(T6, T7);

            N0 = _mm512_madd_epi16(M0, coeff00);
            N1 = _mm512_madd_epi16(M1, coeff01);
            N2 = _mm512_madd_epi16(M2, coeff02);
            N3 = _mm512_madd_epi16(M3, coeff03);
            N4 = _mm512_madd_epi16(M4, coeff00);
            N5 = _mm512_madd_epi16(M5, coeff01);
            N6 = _mm512_madd_epi16(M6, coeff02);
            N7 = _mm512_madd_epi16(M7, coeff03);

            N0 = _mm512_add_epi32(N0, N1);
            N1 = _mm512_add_epi32(N2, N3);
            N2 = _mm512_add_epi32(N4, N5);
            N3 = _mm512_add_epi32(N6, N7);

            N0 = _mm512_add_epi32(N0, N1);
            N1 = _mm512_add_epi32(N2, N3);

            N0 = _mm512_add_epi32(N0, mAddOffset);
            N1 = _mm512_add_epi32(N1, mAddOffset);
            N0 = _mm512_srai_epi32(N0, shift1);
            N1 = _mm512_srai_epi32(N1, shift1);
            N0 = _mm512_packus_epi32(N0, N1);
            N0 = _mm512_min_epu16(N0, max_val);
            _mm512_storeu_si512((__m512i*)&d0[i], N0);

            N0 = _mm512_madd_epi16(M0, coeff10);
            N1 = _mm512_madd_epi16(M1, coeff11);
            N2 = _mm512_madd_epi16(M2, coeff12);
            N3 = _mm512_madd_epi16(M3, coeff13);
            N4 = _mm512_madd_epi16(M4, coeff10);
            N5 = _mm512_madd_epi16(M5, coeff11);
            N6 = _mm512_madd_epi16(M6, coeff12);
            N7 = _mm512_madd_epi16(M7, coeff13);

            N0 = _mm512_add_epi32(N0, N1);
            N1 = _mm512_add_epi32(N2, N3);
            N2 = _mm512_add_epi32(N4, N5);
            N3 = _mm512_add_epi32(N6, N7);

            N0 = _mm512_add_epi32(N0, N1);
            N1 = _mm512_add_epi32(N2, N3);

            N0 = _mm512_add_epi32(N0, mAddOffset);
            N1 = _mm512_add_epi32(N1, mAddOffset);
            N0 = _mm512_srai_epi32(N0, shift1);
            N1 = _mm512_srai_epi32(N1, shift1);
            N0 = _mm512_packus_epi32(N0, N1);
            N0 = _mm512_min_epu16(N0, max_val);
            _mm512_storeu_si512((__m512i*)&d1[i], N0);

            N0 = _mm512_madd_epi16(M0, coeff20);
            N1 = _mm512_madd_epi16(M1, coeff21);
            N2 = _mm512_madd_epi16(M2, coeff22);
            N3 = _mm512_madd_epi16(M3, coeff23);
            N4 = _mm512_madd_epi16(M4, coeff20);
            N5 = _mm512_madd_epi16(M5, coeff21);
            N6 = _mm512_madd_epi16(M6, coeff22);
            N7 = _mm512_madd_epi16(M7, coeff23);

            N0 = _mm512_add_epi32(N0, N1);
            N1 = _mm512_add_epi32(N2, N3);
            N2 = _mm512_add_epi32(N4, N5);
            N3 = _mm512_add_epi32(N6, N7);

            N0 = _mm512_add_epi32(N0, N1);
            N1 = _mm512_add_epi32(N2, N3);

            N0 = _mm512_add_epi32(N0, mAddOffset);
            N1 = _mm512_add_epi32(N1, mAddOffset);
            N0 = _mm512_srai_epi32(N0, shift1);
            N1 = _mm512_srai_epi32(N1, shift1);
            N0 = _mm512_packus_epi32(N0, N1);
            N0 = _mm512_min_epu16(N0, max_val);
            _mm512_storeu_si512((__m512i*)&d2[i], N0);

            p += 32;
        }
        d0 += i_dst;
        d1 += i_dst;
        d2 += i_dst;
        src += i_src;
    }
}

void avg_pel_16_sse512_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    __m512i S1, S2, D;
    int i_src1_x2 = i_src1 << 1;
    int i_src2_x2 = i_src2 << 1;
    int i_dst_x2 = i_dst << 1;

    height >>= 1;

    while (height--) {
        S1 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((const __m256i*)src1)), _mm256_loadu_si256((const __m256i*)(src1 + i_src1)), 1);
        S2 = _mm512_inserti64x4(_mm512_castsi256_si512(_mm256_loadu_si256((const __m256i*)src2)), _mm256_loadu_si256((const __m256i*)(src2 + i_src2)), 1);
        D = _mm512_avg_epu16(S1, S2);
        _mm256_storeu_si256((__m256i*)dst, _mm512_castsi512_si256(D));
        _mm256_storeu_si256((__m256i*)(dst + i_dst), _mm512_extracti64x4_epi64(D, 1));

        src1 += i_src1_x2;
        src2 += i_src2_x2;
        dst += i_dst_x2;
    }
}

void avg_pel_32_sse512_10bit(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i;
    __m512i S1, S2, D;

    for (i = 0; i < height; i++) {
        S1 = _mm512_loadu_si512((const __m512i*)(src1));
        S2 = _mm512_loadu_si512((const __m512i*)(src2));
        D = _mm512_avg_epu16(S1, S2);
        _mm512_storeu_si512((__m512i*)(dst), D);
        src1 += i_src1;
        src2 += i_src2;
        dst += i_dst;
    }
}

#endif