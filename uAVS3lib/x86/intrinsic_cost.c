#include "intrinsic.h"

// --------------------------------------------------------------------------------------------------------------------
// SAD
// --------------------------------------------------------------------------------------------------------------------
i32u_t xGetSAD4_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int i_org_x4, i_pred_x4;
    
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20 = _mm_setzero_si128();

    i_org  <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 2);
    i_org_x4 = i_org << 2;
    i_pred_x4 = i_pred << 2;

    while (height--) {
        T00 = _mm_cvtsi32_si128(*(int*)(p_org));
        T01 = _mm_cvtsi32_si128(*(int*)(p_org + i_org));
        T01 = _mm_unpacklo_epi32(T00, T01);
        T02 = _mm_cvtsi32_si128(*(int*)(p_org + (i_org << 1)));
        T03 = _mm_cvtsi32_si128(*(int*)(p_org + 3 * i_org));
        T03 = _mm_unpacklo_epi32(T02, T03);
        T03 = _mm_unpacklo_epi64(T01, T03);

        T10 = _mm_loadl_epi64((__m128i*)(p_pred));
        T11 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred));
        T11 = _mm_unpacklo_epi32(T10, T11);
        T12 = _mm_loadl_epi64((__m128i*)(p_pred + (i_pred << 1)));
        T13 = _mm_loadl_epi64((__m128i*)(p_pred + 3 * i_pred));
        T13 = _mm_unpacklo_epi32(T12, T13);
        T13 = _mm_unpacklo_epi64(T11, T13);

        T20 = _mm_sad_epu8(T03, T13);
        sum0 = _mm_add_epi32(sum0, T20);

        p_org  += i_org_x4;
        p_pred += i_pred_x4;
    }

    sum1 = _mm_shuffle_epi32(sum0, 2);
    sum0 = _mm_add_epi32(sum0, sum1);
    uiSum = _mm_extract_epi32(sum0, 0);

    uiSum <<= skip_lines;
    return uiSum; 
}

i32u_t xGetSAD8_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int  i_org_x2, i_pred_x2;
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21;

    i_org  <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 2);

    i_org_x2  = i_org  << 1;
    i_pred_x2 = i_pred << 1;

    while (height--) {
        T00 = _mm_loadl_epi64((__m128i*)(p_org));
        T01 = _mm_loadl_epi64((__m128i*)(p_org + i_org));
        T01 = _mm_unpacklo_epi64(T00, T01);
        T02 = _mm_loadl_epi64((__m128i*)(p_org + i_org_x2));
        T03 = _mm_loadl_epi64((__m128i*)(p_org + i_org_x2 + i_org));
        T03 = _mm_unpacklo_epi64(T02, T03);

        T10 = _mm_loadl_epi64((__m128i*)(p_pred));
        T11 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred));
        T11 = _mm_unpacklo_epi64(T10, T11);
        T12 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred_x2));
        T13 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred_x2 + i_pred));
        T13 = _mm_unpacklo_epi64(T12, T13);

        T20 = _mm_sad_epu8(T01, T11);
        T21 = _mm_sad_epu8(T03, T13);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T21);

        p_org  += (i_org  << 2);
        p_pred += (i_pred << 2);
    }

    sum0  = _mm_add_epi32(sum0, sum1);
    sum1  = _mm_shuffle_epi32(sum0, 2);
    sum0  = _mm_add_epi32(sum0, sum1);
    uiSum = _mm_cvtsi128_si32(sum0);

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetSAD16_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;

    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21, T22, T23;

    int more_2_lines = height & 0x3;
    i_org  <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 2);

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + i_org));
        T02 = _mm_load_si128((__m128i*)(p_org + (i_org << 1)));
        T03 = _mm_load_si128((__m128i*)(p_org + 3 * i_org));

        T10 = _mm_loadu_si128((__m128i*)(p_pred));
        T11 = _mm_loadu_si128((__m128i*)(p_pred + i_pred));
        T12 = _mm_loadu_si128((__m128i*)(p_pred + (i_pred << 1)));
        T13 = _mm_loadu_si128((__m128i*)(p_pred + 3 * i_pred));

        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T22 = _mm_sad_epu8(T02, T12);
        T23 = _mm_sad_epu8(T03, T13);

        T20 = _mm_packus_epi32(T20, T21);
        T22 = _mm_packus_epi32(T22, T23);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T22);

        p_org  += (i_org  << 2);
        p_pred += (i_pred << 2);
    }

    if (more_2_lines) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + i_org));
        T10 = _mm_loadu_si128((__m128i*)(p_pred));
        T11 = _mm_loadu_si128((__m128i*)(p_pred + i_pred));
        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T20 = _mm_packus_epi32(T20, T21);

        sum0 = _mm_add_epi32(sum0, T20);
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    uiSum = _mm_cvtsi128_si32(sum0);

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetSAD32_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int i_org_x2;
    int i_pred_x2;
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21, T22, T23;

    i_org  <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 1);

    i_org_x2 = i_org << 1;
    i_pred_x2 = i_pred << 1;

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));                  
        T01 = _mm_load_si128((__m128i*)(p_org + 16));
        T02 = _mm_load_si128((__m128i*)(p_org + i_org));     
        T03 = _mm_load_si128((__m128i*)(p_org + i_org + 16));

        T10 = _mm_loadu_si128((__m128i*)(p_pred));                 
        T11 = _mm_loadu_si128((__m128i*)(p_pred + 16));
        T12 = _mm_loadu_si128((__m128i*)(p_pred + i_pred));    
        T13 = _mm_loadu_si128((__m128i*)(p_pred + i_pred + 16));

        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T22 = _mm_sad_epu8(T02, T12);
        T23 = _mm_sad_epu8(T03, T13);

        T20 = _mm_packus_epi32(T20, T21);
        T22 = _mm_packus_epi32(T22, T23);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T22);

        p_org  += i_org_x2;
        p_pred += i_pred_x2;
    }
               
    sum0  = _mm_add_epi32(sum0, sum1);
    sum0  = _mm_hadd_epi32(sum0,sum0);
    sum0  = _mm_hadd_epi32(sum0,sum0);
    uiSum = _mm_cvtsi128_si32(sum0);  

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetSAD64_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;

    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21, T22, T23;

    i_org  <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= skip_lines;

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));                  
        T01 = _mm_load_si128((__m128i*)(p_org + 16));
        T02 = _mm_load_si128((__m128i*)(p_org + 32));     
        T03 = _mm_load_si128((__m128i*)(p_org + 48));

        T10 = _mm_loadu_si128((__m128i*)(p_pred));                 
        T11 = _mm_loadu_si128((__m128i*)(p_pred + 16));
        T12 = _mm_loadu_si128((__m128i*)(p_pred + 32));    
        T13 = _mm_loadu_si128((__m128i*)(p_pred + 48));

        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T22 = _mm_sad_epu8(T02, T12);
        T23 = _mm_sad_epu8(T03, T13);

        T20 = _mm_packus_epi32(T20, T21);
        T22 = _mm_packus_epi32(T22, T23);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T22);

        p_org  += i_org;
        p_pred += i_pred;
    }

    sum0  = _mm_add_epi32(sum0, sum1);
    sum0  = _mm_hadd_epi32(sum0,sum0);                
    sum0  = _mm_hadd_epi32(sum0,sum0);
    uiSum = _mm_cvtsi128_si32(sum0);  

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetSAD32_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int i_org_x2;
    int i_pred_x2;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10, T12;
    __m256i T20, T22;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 1);

    i_org_x2 = i_org << 1;
    i_pred_x2 = i_pred << 1;

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + i_org));

        T10 = _mm256_loadu_si256((__m256i*)(p_pred));
        T12 = _mm256_loadu_si256((__m256i*)(p_pred + i_pred));

        T20 = _mm256_sad_epu8(T00, T10);
        T22 = _mm256_sad_epu8(T02, T12);

        T20 = _mm256_packus_epi32(T20, T22);

        sum0 = _mm256_add_epi32(sum0, T20);

        p_org += i_org_x2;
        p_pred += i_pred_x2;
    }

    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetSAD64_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;

    __m256i sum0 = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10, T12;
    __m256i T20, T22;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= skip_lines;

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + 32));

        T10 = _mm256_loadu_si256((__m256i*)(p_pred));
        T12 = _mm256_loadu_si256((__m256i*)(p_pred + 32));

        T20 = _mm256_sad_epu8(T00, T10);
        T22 = _mm256_sad_epu8(T02, T12);

        T20 = _mm256_packus_epi32(T20, T22);

        sum0 = _mm256_add_epi32(sum0, T20);

        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

// --------------------------------------------------------------------------------------------------------------------
// AVG_SAD
// --------------------------------------------------------------------------------------------------------------------

i32u_t xGetAVGSAD4_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int i_org_x4, i_pred1_x4, i_pred2_x4;

    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20 = _mm_setzero_si128();

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines + 2);
    i_org_x4 = i_org << 2;
    i_pred1_x4 = i_pred1 << 2;
    i_pred2_x4 = i_pred2 << 2;

    while (height--) {
        T00 = _mm_cvtsi32_si128(*(int*)(p_org));
        T01 = _mm_cvtsi32_si128(*(int*)(p_org + i_org));
        T01 = _mm_unpacklo_epi32(T00, T01);
        T02 = _mm_cvtsi32_si128(*(int*)(p_org + (i_org << 1)));
        T03 = _mm_cvtsi32_si128(*(int*)(p_org + 3 * i_org));
        T03 = _mm_unpacklo_epi32(T02, T03);
        T03 = _mm_unpacklo_epi64(T01, T03);

        T10 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1          )), _mm_loadl_epi64((__m128i*)(p_pred2          )));
        T11 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1 + i_pred1)), _mm_loadl_epi64((__m128i*)(p_pred2 + i_pred2)));
        T11 = _mm_unpacklo_epi32(T10, T11);
        T12 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1 + (i_pred1 << 1))), _mm_loadl_epi64((__m128i*)(p_pred2 + (i_pred2 << 1))));
        T13 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1 + 3 * i_pred1   )), _mm_loadl_epi64((__m128i*)(p_pred2 + 3 * i_pred2   )));
        T13 = _mm_unpacklo_epi32(T12, T13);
        T13 = _mm_unpacklo_epi64(T11, T13);

        T20 = _mm_sad_epu8(T03, T13);
        sum0 = _mm_add_epi32(sum0, T20);

        p_org   += i_org_x4;
        p_pred1 += i_pred1_x4;
        p_pred2 += i_pred2_x4;
    }

    sum1 = _mm_shuffle_epi32(sum0, 2);
    sum0 = _mm_add_epi32(sum0, sum1);
    uiSum = _mm_extract_epi32(sum0, 0);

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD8_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int  i_org_x2, i_pred1_x2, i_pred2_x2;
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines + 2);

    i_org_x2 = i_org << 1;
    i_pred1_x2 = i_pred1 << 1;
    i_pred2_x2 = i_pred2 << 1;

    while (height--) {
        T00 = _mm_loadl_epi64((__m128i*)(p_org));
        T01 = _mm_loadl_epi64((__m128i*)(p_org + i_org));
        T01 = _mm_unpacklo_epi64(T00, T01);
        T02 = _mm_loadl_epi64((__m128i*)(p_org + i_org_x2));
        T03 = _mm_loadl_epi64((__m128i*)(p_org + i_org_x2 + i_org));
        T03 = _mm_unpacklo_epi64(T02, T03);

        T10 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1          )), _mm_loadl_epi64((__m128i*)(p_pred2          )));
        T11 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1 + i_pred1)), _mm_loadl_epi64((__m128i*)(p_pred2 + i_pred2)));
        T11 = _mm_unpacklo_epi64(T10, T11);
        T12 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1 + i_pred1_x2          )), _mm_loadl_epi64((__m128i*)(p_pred2 + i_pred2_x2          )));
        T13 = _mm_avg_epu8(_mm_loadl_epi64((__m128i*)(p_pred1 + i_pred1_x2 + i_pred1)), _mm_loadl_epi64((__m128i*)(p_pred2 + i_pred2_x2 + i_pred2)));
        T13 = _mm_unpacklo_epi64(T12, T13);

        T20 = _mm_sad_epu8(T01, T11);
        T21 = _mm_sad_epu8(T03, T13);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T21);

        p_org += (i_org << 2);
        p_pred1 += (i_pred1 << 2);
        p_pred2 += (i_pred2 << 2);
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum1 = _mm_shuffle_epi32(sum0, 2);
    sum0 = _mm_add_epi32(sum0, sum1);
    uiSum = _mm_cvtsi128_si32(sum0);

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD16_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;

    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21, T22, T23;

    int more_2_lines = height & 0x3;
    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines + 2);

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + i_org));
        T02 = _mm_load_si128((__m128i*)(p_org + (i_org << 1)));
        T03 = _mm_load_si128((__m128i*)(p_org + 3 * i_org));

        T10 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1                 )), _mm_loadu_si128((__m128i*)(p_pred2                 )));
        T11 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 +  i_pred1      )), _mm_loadu_si128((__m128i*)(p_pred2 +  i_pred2      )));
        T12 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + (i_pred1 << 1))), _mm_loadu_si128((__m128i*)(p_pred2 + (i_pred2 << 1))));
        T13 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + 3 * i_pred1   )), _mm_loadu_si128((__m128i*)(p_pred2 + 3 * i_pred2   )));

        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T22 = _mm_sad_epu8(T02, T12);
        T23 = _mm_sad_epu8(T03, T13);

        T20 = _mm_packus_epi32(T20, T21);
        T22 = _mm_packus_epi32(T22, T23);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T22);

        p_org += (i_org << 2);
        p_pred1 += (i_pred1 << 2);
        p_pred2 += (i_pred2 << 2);
    }

    if (more_2_lines) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + i_org));
        T10 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1          )), _mm_loadu_si128((__m128i*)(p_pred2          )));
        T11 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + i_pred1)), _mm_loadu_si128((__m128i*)(p_pred2 + i_pred2)));
        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T20 = _mm_packus_epi32(T20, T21);

        sum0 = _mm_add_epi32(sum0, T20);
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    uiSum = _mm_cvtsi128_si32(sum0);

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD32_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int i_org_x2;
    int i_pred1_x2, i_pred2_x2;
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21, T22, T23;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines + 1);

    i_org_x2 = i_org << 1;
    i_pred1_x2 = i_pred1 << 1;
    i_pred2_x2 = i_pred2 << 1;

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + 16));
        T02 = _mm_load_si128((__m128i*)(p_org + i_org));
        T03 = _mm_load_si128((__m128i*)(p_org + i_org + 16));

        T10 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1               )), _mm_loadu_si128((__m128i*)(p_pred2               )));
        T11 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + 16          )), _mm_loadu_si128((__m128i*)(p_pred2 + 16          )));
        T12 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + i_pred1     )), _mm_loadu_si128((__m128i*)(p_pred2 + i_pred2     )));
        T13 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + i_pred1 + 16)), _mm_loadu_si128((__m128i*)(p_pred2 + i_pred2 + 16)));

        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T22 = _mm_sad_epu8(T02, T12);
        T23 = _mm_sad_epu8(T03, T13);

        T20 = _mm_packus_epi32(T20, T21);
        T22 = _mm_packus_epi32(T22, T23);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T22);

        p_org += i_org_x2;
        p_pred1 += i_pred1_x2;
        p_pred2 += i_pred2_x2;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    uiSum = _mm_cvtsi128_si32(sum0);

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD64_sse128(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;

    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;
    __m128i T10, T11, T12, T13;
    __m128i T20, T21, T22, T23;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= skip_lines;

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + 16));
        T02 = _mm_load_si128((__m128i*)(p_org + 32));
        T03 = _mm_load_si128((__m128i*)(p_org + 48));

        T10 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1     )), _mm_loadu_si128((__m128i*)(p_pred2     )));
        T11 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + 16)), _mm_loadu_si128((__m128i*)(p_pred2 + 16)));
        T12 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + 32)), _mm_loadu_si128((__m128i*)(p_pred2 + 32)));
        T13 = _mm_avg_epu8(_mm_loadu_si128((__m128i*)(p_pred1 + 48)), _mm_loadu_si128((__m128i*)(p_pred2 + 48)));

        T20 = _mm_sad_epu8(T00, T10);
        T21 = _mm_sad_epu8(T01, T11);
        T22 = _mm_sad_epu8(T02, T12);
        T23 = _mm_sad_epu8(T03, T13);

        T20 = _mm_packus_epi32(T20, T21);
        T22 = _mm_packus_epi32(T22, T23);

        sum0 = _mm_add_epi32(sum0, T20);
        sum1 = _mm_add_epi32(sum1, T22);

        p_org += i_org;
        p_pred1 += i_pred1;
        p_pred2 += i_pred2;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    uiSum = _mm_cvtsi128_si32(sum0);

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD32_sse256(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    int i_org_x2;
    int i_pred1_x2, i_pred2_x2;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10, T12;
    __m256i T20, T22;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines + 1);

    i_org_x2 = i_org << 1;
    i_pred1_x2 = i_pred1 << 1;
    i_pred2_x2 = i_pred2 << 1;

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + i_org));

        T10 = _mm256_avg_epu8(_mm256_loadu_si256((__m256i*)(p_pred1          )), _mm256_loadu_si256((__m256i*)(p_pred2          )));
        T12 = _mm256_avg_epu8(_mm256_loadu_si256((__m256i*)(p_pred1 + i_pred1)), _mm256_loadu_si256((__m256i*)(p_pred2 + i_pred2)));

        T20 = _mm256_sad_epu8(T00, T10);
        T22 = _mm256_sad_epu8(T02, T12);

        T20 = _mm256_packus_epi32(T20, T22);

        sum0 = _mm256_add_epi32(sum0, T20);

        p_org += i_org_x2;
        p_pred1 += i_pred1_x2;
        p_pred2 += i_pred2_x2;
    }

    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD64_sse256(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;

    __m256i sum0 = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10, T12;
    __m256i T20, T22;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= skip_lines;

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + 32));

        T10 = _mm256_avg_epu8(_mm256_loadu_si256((__m256i*)(p_pred1     )), _mm256_loadu_si256((__m256i*)(p_pred2     )));
        T12 = _mm256_avg_epu8(_mm256_loadu_si256((__m256i*)(p_pred1 + 32)), _mm256_loadu_si256((__m256i*)(p_pred2 + 32)));

        T20 = _mm256_sad_epu8(T00, T10);
        T22 = _mm256_sad_epu8(T02, T12);

        T20 = _mm256_packus_epi32(T20, T22);

        sum0 = _mm256_add_epi32(sum0, T20);

        p_org += i_org;
        p_pred1 += i_pred1;
        p_pred2 += i_pred2;
    }

    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    sum0 = _mm256_hadd_epi32(sum0, _mm256_setzero_si256());
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

// --------------------------------------------------------------------------------------------------------------------
// SAD_x4
// --------------------------------------------------------------------------------------------------------------------

void xGetSAD4_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    int i_org_x4, i_pred_x4;

    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i sum2 = _mm_setzero_si128();
    __m128i sum3 = _mm_setzero_si128();
    __m128i T01, T03;
    __m128i T11, T13;
    __m128i T21, T23;
    __m128i T31, T33;
    __m128i T41, T43;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 2);
    i_org_x4 = i_org << 2;
    i_pred_x4 = i_pred << 2;

    while (height--) {
        T01 = _mm_unpacklo_epi32(_mm_cvtsi32_si128(*(int*)(p_org               )), _mm_cvtsi32_si128(*(int*)(p_org +     i_org)));
        T03 = _mm_unpacklo_epi32(_mm_cvtsi32_si128(*(int*)(p_org + (i_org << 1))), _mm_cvtsi32_si128(*(int*)(p_org + 3 * i_org)));
        T03 = _mm_unpacklo_epi64(T01, T03);

        T11 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred0                )), _mm_loadl_epi64((__m128i*)(pred0 +     i_pred)));
        T13 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred0 + (i_pred << 1))), _mm_loadl_epi64((__m128i*)(pred0 + 3 * i_pred)));
        T13 = _mm_unpacklo_epi64(T11, T13);

        T21 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred1                )), _mm_loadl_epi64((__m128i*)(pred1 +     i_pred)));
        T23 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred1 + (i_pred << 1))), _mm_loadl_epi64((__m128i*)(pred1 + 3 * i_pred)));
        T23 = _mm_unpacklo_epi64(T21, T23);

        T31 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred2                )), _mm_loadl_epi64((__m128i*)(pred2 +     i_pred)));
        T33 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred2 + (i_pred << 1))), _mm_loadl_epi64((__m128i*)(pred2 + 3 * i_pred)));
        T33 = _mm_unpacklo_epi64(T31, T33);

        T41 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred3                )), _mm_loadl_epi64((__m128i*)(pred3 +     i_pred)));
        T43 = _mm_unpacklo_epi32(_mm_loadl_epi64((__m128i*)(pred3 + (i_pred << 1))), _mm_loadl_epi64((__m128i*)(pred3 + 3 * i_pred)));
        T43 = _mm_unpacklo_epi64(T41, T43);

        sum0 = _mm_add_epi32(sum0, _mm_sad_epu8(T03, T13));
        sum1 = _mm_add_epi32(sum1, _mm_sad_epu8(T03, T23));
        sum2 = _mm_add_epi32(sum2, _mm_sad_epu8(T03, T33));
        sum3 = _mm_add_epi32(sum3, _mm_sad_epu8(T03, T43));

        p_org += i_org_x4;
        pred0 += i_pred_x4;
        pred1 += i_pred_x4;
        pred2 += i_pred_x4;
        pred3 += i_pred_x4;
    }

    sum0 = _mm_hadd_epi32(sum0, sum1);
    sum2 = _mm_hadd_epi32(sum2, sum3);
    sum0 = _mm_hadd_epi32(sum0, sum2);

    sad[0] = ((i32u_t)_mm_extract_epi32(sum0, 0)) << skip_lines;
    sad[1] = ((i32u_t)_mm_extract_epi32(sum0, 1)) << skip_lines;
    sad[2] = ((i32u_t)_mm_extract_epi32(sum0, 2)) << skip_lines;
    sad[3] = ((i32u_t)_mm_extract_epi32(sum0, 3)) << skip_lines;
}

void xGetSAD8_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    int  i_org_x2, i_pred_x2;
    int  i_org_x4, i_pred_x4;
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i sum2 = _mm_setzero_si128();
    __m128i sum3 = _mm_setzero_si128();
    __m128i sum4 = _mm_setzero_si128();
    __m128i sum5 = _mm_setzero_si128();
    __m128i sum6 = _mm_setzero_si128();
    __m128i sum7 = _mm_setzero_si128();
    __m128i T01, T03;
    __m128i T11, T13;
    __m128i T21, T23;
    __m128i T31, T33;
    __m128i T41, T43;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 2);

    i_org_x2 = i_org << 1;
    i_pred_x2 = i_pred << 1;
    i_org_x4 = i_org << 2;
    i_pred_x4 = i_pred << 2;

    while (height--) {
        T01 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(p_org           )), _mm_loadl_epi64((__m128i*)(p_org +            i_org)));
        T03 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(p_org + i_org_x2)), _mm_loadl_epi64((__m128i*)(p_org + i_org_x2 + i_org)));

        T11 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred0            )), _mm_loadl_epi64((__m128i*)(pred0 +             i_pred)));
        T13 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred0 + i_pred_x2)), _mm_loadl_epi64((__m128i*)(pred0 + i_pred_x2 + i_pred)));

        T21 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred1            )), _mm_loadl_epi64((__m128i*)(pred1 +             i_pred)));
        T23 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred1 + i_pred_x2)), _mm_loadl_epi64((__m128i*)(pred1 + i_pred_x2 + i_pred)));

        T31 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred2            )), _mm_loadl_epi64((__m128i*)(pred2 +             i_pred)));
        T33 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred2 + i_pred_x2)), _mm_loadl_epi64((__m128i*)(pred2 + i_pred_x2 + i_pred)));

        T41 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred3            )), _mm_loadl_epi64((__m128i*)(pred3 +             i_pred)));
        T43 = _mm_unpacklo_epi64(_mm_loadl_epi64((__m128i*)(pred3 + i_pred_x2)), _mm_loadl_epi64((__m128i*)(pred3 + i_pred_x2 + i_pred)));


        sum0 = _mm_add_epi32(sum0, _mm_sad_epu8(T01, T11));
        sum1 = _mm_add_epi32(sum1, _mm_sad_epu8(T03, T13));
        sum2 = _mm_add_epi32(sum2, _mm_sad_epu8(T01, T21));
        sum3 = _mm_add_epi32(sum3, _mm_sad_epu8(T03, T23));
        sum4 = _mm_add_epi32(sum4, _mm_sad_epu8(T01, T31));
        sum5 = _mm_add_epi32(sum5, _mm_sad_epu8(T03, T33));
        sum6 = _mm_add_epi32(sum6, _mm_sad_epu8(T01, T41));
        sum7 = _mm_add_epi32(sum7, _mm_sad_epu8(T03, T43));

        p_org += i_org_x4;
        pred0 += i_pred_x4;
        pred1 += i_pred_x4;
        pred2 += i_pred_x4;
        pred3 += i_pred_x4;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum2 = _mm_add_epi32(sum2, sum3);
    sum4 = _mm_add_epi32(sum4, sum5);
    sum6 = _mm_add_epi32(sum6, sum7);

    sum0 = _mm_hadd_epi32(sum0, sum2);
    sum4 = _mm_hadd_epi32(sum4, sum6);
    sum0 = _mm_hadd_epi32(sum0, sum4);

    sad[0] = ((i32u_t)_mm_extract_epi32(sum0, 0)) << skip_lines;
    sad[1] = ((i32u_t)_mm_extract_epi32(sum0, 1)) << skip_lines;
    sad[2] = ((i32u_t)_mm_extract_epi32(sum0, 2)) << skip_lines;
    sad[3] = ((i32u_t)_mm_extract_epi32(sum0, 3)) << skip_lines;
}

void xGetSAD16_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    int  i_org_x2, i_pred_x2;
    int  i_org_x3, i_pred_x3;
    int  i_org_x4, i_pred_x4;

    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i sum2 = _mm_setzero_si128();
    __m128i sum3 = _mm_setzero_si128();
    __m128i sum4 = _mm_setzero_si128();
    __m128i sum5 = _mm_setzero_si128();
    __m128i sum6 = _mm_setzero_si128();
    __m128i sum7 = _mm_setzero_si128();

    __m128i T00, T01, T02, T03;

    int more_2_lines = height & 0x3;
    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 2);

    i_org_x2 = i_org << 1;
    i_pred_x2 = i_pred << 1;
    i_org_x3 = i_org + i_org_x2;
    i_pred_x3 = i_pred + i_pred_x2;
    i_org_x4 = i_org << 2;
    i_pred_x4 = i_pred << 2;

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + i_org));
        T02 = _mm_load_si128((__m128i*)(p_org + i_org_x2));
        T03 = _mm_load_si128((__m128i*)(p_org + i_org_x3));

        sum0 = _mm_add_epi32(sum0, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred0            ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred0 + i_pred   )))));
        sum1 = _mm_add_epi32(sum1, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred0 + i_pred_x2))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred0 + i_pred_x3)))));

        sum2 = _mm_add_epi32(sum2, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred1            ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred1 + i_pred   )))));
        sum3 = _mm_add_epi32(sum3, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred1 + i_pred_x2))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred1 + i_pred_x3)))));

        sum4 = _mm_add_epi32(sum4, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred2            ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred2 + i_pred   )))));
        sum5 = _mm_add_epi32(sum5, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred2 + i_pred_x2))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred2 + i_pred_x3)))));

        sum6 = _mm_add_epi32(sum6, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred3            ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred3 + i_pred   )))));
        sum7 = _mm_add_epi32(sum7, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred3 + i_pred_x2))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred3 + i_pred_x3)))));

        p_org += i_org_x4;
        pred0 += i_pred_x4;
        pred1 += i_pred_x4;
        pred2 += i_pred_x4;
        pred3 += i_pred_x4;
    }

    if (more_2_lines) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + i_org));

        sum0 = _mm_add_epi32(sum0, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred0))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred0 + i_pred)))));
        sum2 = _mm_add_epi32(sum2, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred1))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred1 + i_pred)))));
        sum4 = _mm_add_epi32(sum4, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred2))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred2 + i_pred)))));
        sum6 = _mm_add_epi32(sum6, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred3))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred3 + i_pred)))));
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum2 = _mm_add_epi32(sum2, sum3);
    sum4 = _mm_add_epi32(sum4, sum5);
    sum6 = _mm_add_epi32(sum6, sum7);

    sum0 = _mm_hadd_epi32(sum0, sum2);
    sum4 = _mm_hadd_epi32(sum4, sum6);
    sum0 = _mm_hadd_epi32(sum0, sum4);

    sad[0] = ((i32u_t)_mm_extract_epi32(sum0, 0)) << skip_lines;
    sad[1] = ((i32u_t)_mm_extract_epi32(sum0, 1)) << skip_lines;
    sad[2] = ((i32u_t)_mm_extract_epi32(sum0, 2)) << skip_lines;
    sad[3] = ((i32u_t)_mm_extract_epi32(sum0, 3)) << skip_lines;
}

void xGetSAD32_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    int i_org_x2;
    int i_pred_x2;
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i sum2 = _mm_setzero_si128();
    __m128i sum3 = _mm_setzero_si128();
    __m128i sum4 = _mm_setzero_si128();
    __m128i sum5 = _mm_setzero_si128();
    __m128i sum6 = _mm_setzero_si128();
    __m128i sum7 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 1);

    i_org_x2 = i_org << 1;
    i_pred_x2 = i_pred << 1;

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + 16));
        T02 = _mm_load_si128((__m128i*)(p_org + i_org));
        T03 = _mm_load_si128((__m128i*)(p_org + i_org + 16));

        sum0 = _mm_add_epi32(sum0, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred0         ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred0 +          16)))));
        sum1 = _mm_add_epi32(sum1, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred0 + i_pred))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred0 + i_pred + 16)))));

        sum2 = _mm_add_epi32(sum2, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred1         ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred1 +          16)))));
        sum3 = _mm_add_epi32(sum3, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred1 + i_pred))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred1 + i_pred + 16)))));

        sum4 = _mm_add_epi32(sum4, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred2         ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred2 +          16)))));
        sum5 = _mm_add_epi32(sum5, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred2 + i_pred))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred2 + i_pred + 16)))));

        sum6 = _mm_add_epi32(sum6, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred3         ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred3 +          16)))));
        sum7 = _mm_add_epi32(sum7, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred3 + i_pred))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred3 + i_pred + 16)))));

        p_org += i_org_x2;
        pred0 += i_pred_x2;
        pred1 += i_pred_x2;
        pred2 += i_pred_x2;
        pred3 += i_pred_x2;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum2 = _mm_add_epi32(sum2, sum3);
    sum4 = _mm_add_epi32(sum4, sum5);
    sum6 = _mm_add_epi32(sum6, sum7);

    sum0 = _mm_hadd_epi32(sum0, sum2);
    sum4 = _mm_hadd_epi32(sum4, sum6);
    sum0 = _mm_hadd_epi32(sum0, sum4);

    sad[0] = ((i32u_t)_mm_extract_epi32(sum0, 0)) << skip_lines;
    sad[1] = ((i32u_t)_mm_extract_epi32(sum0, 1)) << skip_lines;
    sad[2] = ((i32u_t)_mm_extract_epi32(sum0, 2)) << skip_lines;
    sad[3] = ((i32u_t)_mm_extract_epi32(sum0, 3)) << skip_lines;
}

void xGetSAD64_x4_sse128(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i sum2 = _mm_setzero_si128();
    __m128i sum3 = _mm_setzero_si128();
    __m128i sum4 = _mm_setzero_si128();
    __m128i sum5 = _mm_setzero_si128();
    __m128i sum6 = _mm_setzero_si128();
    __m128i sum7 = _mm_setzero_si128();
    __m128i T00, T01, T02, T03;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= skip_lines;

    while (height--) {
        T00 = _mm_load_si128((__m128i*)(p_org));
        T01 = _mm_load_si128((__m128i*)(p_org + 16));
        T02 = _mm_load_si128((__m128i*)(p_org + 32));
        T03 = _mm_load_si128((__m128i*)(p_org + 48));

        sum0 = _mm_add_epi32(sum0, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred0     ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred0 + 16)))));
        sum1 = _mm_add_epi32(sum1, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred0 + 32))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred0 + 48)))));

        sum2 = _mm_add_epi32(sum2, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred1     ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred1 + 16)))));
        sum3 = _mm_add_epi32(sum3, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred1 + 32))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred1 + 48)))));

        sum4 = _mm_add_epi32(sum4, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred2     ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred2 + 16)))));
        sum5 = _mm_add_epi32(sum5, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred2 + 32))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred2 + 48)))));

        sum6 = _mm_add_epi32(sum6, _mm_packus_epi32(_mm_sad_epu8(T00, _mm_loadu_si128((__m128i*)(pred3     ))), _mm_sad_epu8(T01, _mm_loadu_si128((__m128i*)(pred3 + 16)))));
        sum7 = _mm_add_epi32(sum7, _mm_packus_epi32(_mm_sad_epu8(T02, _mm_loadu_si128((__m128i*)(pred3 + 32))), _mm_sad_epu8(T03, _mm_loadu_si128((__m128i*)(pred3 + 48)))));

        p_org += i_org;
        pred0 += i_pred;
        pred1 += i_pred;
        pred2 += i_pred;
        pred3 += i_pred;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum2 = _mm_add_epi32(sum2, sum3);
    sum4 = _mm_add_epi32(sum4, sum5);
    sum6 = _mm_add_epi32(sum6, sum7);

    sum0 = _mm_hadd_epi32(sum0, sum2);
    sum4 = _mm_hadd_epi32(sum4, sum6);
    sum0 = _mm_hadd_epi32(sum0, sum4);

    sad[0] = ((i32u_t)_mm_extract_epi32(sum0, 0)) << skip_lines;
    sad[1] = ((i32u_t)_mm_extract_epi32(sum0, 1)) << skip_lines;
    sad[2] = ((i32u_t)_mm_extract_epi32(sum0, 2)) << skip_lines;
    sad[3] = ((i32u_t)_mm_extract_epi32(sum0, 3)) << skip_lines;
}

void xGetSAD16_x4_sse256(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    int  i_org_x2, i_pred_x2;
    int  i_org_x3, i_pred_x3;
    int  i_org_x4, i_pred_x4;

    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i ZERO = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;
    __m256i T20, T21;
    __m256i T30, T31;
    __m256i T40, T41;

    int more_2_lines = height & 0x3;
    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 2);

    i_org_x2 = i_org << 1;
    i_pred_x2 = i_pred << 1;
    i_org_x3 = i_org + i_org_x2;
    i_pred_x3 = i_pred + i_pred_x2;
    i_org_x4 = i_org << 2;
    i_pred_x4 = i_pred << 2;

    while (height--) {
        T00 = _mm256_set_m128i(_mm_load_si128((__m128i*)(p_org           )), _mm_load_si128((__m128i*)(p_org + i_org   )));
        T01 = _mm256_set_m128i(_mm_load_si128((__m128i*)(p_org + i_org_x2)), _mm_load_si128((__m128i*)(p_org + i_org_x3)));

        T10 = _mm256_loadu2_m128i(((__m128i*)(pred0            )), ((__m128i*)(pred0 + i_pred   )));
        T11 = _mm256_loadu2_m128i(((__m128i*)(pred0 + i_pred_x2)), ((__m128i*)(pred0 + i_pred_x3)));
        sum0 = _mm256_add_epi32(sum0, _mm256_packus_epi32(_mm256_sad_epu8(T00, T10), _mm256_sad_epu8(T01, T11)));

        T20 = _mm256_loadu2_m128i(((__m128i*)(pred1            )), ((__m128i*)(pred1 + i_pred   )));
        T21 = _mm256_loadu2_m128i(((__m128i*)(pred1 + i_pred_x2)), ((__m128i*)(pred1 + i_pred_x3)));
        sum1 = _mm256_add_epi32(sum1, _mm256_packus_epi32(_mm256_sad_epu8(T00, T20), _mm256_sad_epu8(T01, T21)));

        T30 = _mm256_loadu2_m128i(((__m128i*)(pred2            )), ((__m128i*)(pred2 + i_pred   )));
        T31 = _mm256_loadu2_m128i(((__m128i*)(pred2 + i_pred_x2)), ((__m128i*)(pred2 + i_pred_x3)));
        sum2 = _mm256_add_epi32(sum2, _mm256_packus_epi32(_mm256_sad_epu8(T00, T30), _mm256_sad_epu8(T01, T31)));

        T40 = _mm256_loadu2_m128i(((__m128i*)(pred3            )), ((__m128i*)(pred3 + i_pred   )));
        T41 = _mm256_loadu2_m128i(((__m128i*)(pred3 + i_pred_x2)), ((__m128i*)(pred3 + i_pred_x3)));
        sum3 = _mm256_add_epi32(sum3, _mm256_packus_epi32(_mm256_sad_epu8(T00, T40), _mm256_sad_epu8(T01, T41)));

        p_org += i_org_x4;
        pred0 += i_pred_x4;
        pred1 += i_pred_x4;
        pred2 += i_pred_x4;
        pred3 += i_pred_x4;
    }

    if (more_2_lines) {
        T00 = _mm256_set_m128i(_mm_load_si128((__m128i*)(p_org)), _mm_load_si128((__m128i*)(p_org + i_org)));

        T10 = _mm256_loadu2_m128i(((__m128i*)(pred0)), ((__m128i*)(pred0 + i_pred)));
        sum0 = _mm256_add_epi32(sum0, _mm256_packus_epi32(_mm256_sad_epu8(T00, T10), ZERO));

        T20 = _mm256_loadu2_m128i(((__m128i*)(pred1)), ((__m128i*)(pred1 + i_pred)));
        sum1 = _mm256_add_epi32(sum1, _mm256_packus_epi32(_mm256_sad_epu8(T00, T20), ZERO));

        T30 = _mm256_loadu2_m128i(((__m128i*)(pred2)), ((__m128i*)(pred2 + i_pred)));
        sum2 = _mm256_add_epi32(sum2, _mm256_packus_epi32(_mm256_sad_epu8(T00, T30), ZERO));

        T40 = _mm256_loadu2_m128i(((__m128i*)(pred3)), ((__m128i*)(pred3 + i_pred)));
        sum3 = _mm256_add_epi32(sum3, _mm256_packus_epi32(_mm256_sad_epu8(T00, T40), ZERO));
    }

    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum2 = _mm256_hadd_epi32(sum2, sum3);
    sum0 = _mm256_hadd_epi32(sum0, sum2);

    sad[0] = (((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4))) << skip_lines;
    sad[1] = (((i32u_t)_mm256_extract_epi32(sum0, 1)) + ((i32u_t)_mm256_extract_epi32(sum0, 5))) << skip_lines;
    sad[2] = (((i32u_t)_mm256_extract_epi32(sum0, 2)) + ((i32u_t)_mm256_extract_epi32(sum0, 6))) << skip_lines;
    sad[3] = (((i32u_t)_mm256_extract_epi32(sum0, 3)) + ((i32u_t)_mm256_extract_epi32(sum0, 7))) << skip_lines;
}

void xGetSAD32_x4_sse256(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    int i_org_x2;
    int i_pred_x2;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i T00, T02;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines + 1);

    i_org_x2 = i_org << 1;
    i_pred_x2 = i_pred << 1;

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + i_org));

        sum0 = _mm256_add_epi32(sum0, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred0))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred0 + i_pred)))));
        sum1 = _mm256_add_epi32(sum1, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred1))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred1 + i_pred)))));
        sum2 = _mm256_add_epi32(sum2, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred2))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred2 + i_pred)))));
        sum3 = _mm256_add_epi32(sum3, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred3))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred3 + i_pred)))));

        p_org += i_org_x2;
        pred0 += i_pred_x2;
        pred1 += i_pred_x2;
        pred2 += i_pred_x2;
        pred3 += i_pred_x2;
    }

    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum2 = _mm256_hadd_epi32(sum2, sum3);
    sum0 = _mm256_hadd_epi32(sum0, sum2);

    sad[0] = (((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4))) << skip_lines;
    sad[1] = (((i32u_t)_mm256_extract_epi32(sum0, 1)) + ((i32u_t)_mm256_extract_epi32(sum0, 5))) << skip_lines;
    sad[2] = (((i32u_t)_mm256_extract_epi32(sum0, 2)) + ((i32u_t)_mm256_extract_epi32(sum0, 6))) << skip_lines;
    sad[3] = (((i32u_t)_mm256_extract_epi32(sum0, 3)) + ((i32u_t)_mm256_extract_epi32(sum0, 7))) << skip_lines;
}

void xGetSAD64_x4_sse256(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i T00, T02;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= skip_lines;

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + 32));

        sum0 = _mm256_add_epi32(sum0, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred0))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred0 + 32)))));
        sum1 = _mm256_add_epi32(sum1, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred1))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred1 + 32)))));
        sum2 = _mm256_add_epi32(sum2, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred2))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred2 + 32)))));
        sum3 = _mm256_add_epi32(sum3, _mm256_packus_epi32(_mm256_sad_epu8(T00, _mm256_loadu_si256((__m256i*)(pred3))), _mm256_sad_epu8(T02, _mm256_loadu_si256((__m256i*)(pred3 + 32)))));

        p_org += i_org;
        pred0 += i_pred;
        pred1 += i_pred;
        pred2 += i_pred;
        pred3 += i_pred;
    }

    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum2 = _mm256_hadd_epi32(sum2, sum3);
    sum0 = _mm256_hadd_epi32(sum0, sum2);

    sad[0] = (((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4))) << skip_lines;
    sad[1] = (((i32u_t)_mm256_extract_epi32(sum0, 1)) + ((i32u_t)_mm256_extract_epi32(sum0, 5))) << skip_lines;
    sad[2] = (((i32u_t)_mm256_extract_epi32(sum0, 2)) + ((i32u_t)_mm256_extract_epi32(sum0, 6))) << skip_lines;
    sad[3] = (((i32u_t)_mm256_extract_epi32(sum0, 3)) + ((i32u_t)_mm256_extract_epi32(sum0, 7))) << skip_lines;
}

// --------------------------------------------------------------------------------------------------------------------
// SSE
// --------------------------------------------------------------------------------------------------------------------

i32u_t xGetSSE4_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;

    const __m128i zero = _mm_setzero_si128();
    __m128i sum = _mm_setzero_si128();
    __m128i T00, T01;
    __m128i T10, T11;

    height >>= 1;

    while (height--) {
        T00 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_org)), zero);
        T10 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_org + i_org)), zero);
        T01 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_pred)), zero);
        T11 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_pred + i_pred)), zero);

        T00 = _mm_unpacklo_epi64(T00, T10);
        T01 = _mm_unpacklo_epi64(T01, T11);

        T00 = _mm_sub_epi16(T00, T01);
        T00 = _mm_madd_epi16(T00, T00);

        sum = _mm_add_epi32(sum, T00);

        p_org += (i_org << 1);
        p_pred += (i_pred << 1);
    }

    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    uiSum = _mm_extract_epi32(sum, 0);

    return uiSum;
}

i32u_t xGetSSE8_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;

    const __m128i zero = _mm_setzero_si128();
    __m128i sum = _mm_setzero_si128();
    __m128i T00, T01;

    while (height--) {
        T00 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)(p_org)), zero);
        T01 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)(p_pred)), zero);

        T00 = _mm_sub_epi16(T00, T01);
        T00 = _mm_madd_epi16(T00, T00);
        sum = _mm_add_epi32(sum, T00);

        p_org += i_org;
        p_pred += i_pred;
    }

    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    uiSum = _mm_extract_epi32(sum, 0);

    return uiSum;
}

i32u_t xGetSSE16_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;

    const __m128i zero = _mm_setzero_si128();
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01;
    __m128i T10, T11;

    while (height--) {
        T00 = _mm_loadu_si128((__m128i*)(p_org));
        T01 = _mm_loadu_si128((__m128i*)(p_pred));

        T10 = _mm_unpackhi_epi8(T00, zero);
        T11 = _mm_unpackhi_epi8(T01, zero);

        T00 = _mm_unpacklo_epi8(T00, zero);
        T01 = _mm_unpacklo_epi8(T01, zero);
        

        T00 = _mm_sub_epi16(T00, T01);
        T10 = _mm_sub_epi16(T10, T11);

        T00 = _mm_madd_epi16(T00, T00);
        T10 = _mm_madd_epi16(T10, T10);

        sum0 = _mm_add_epi32(sum0, T00);
        sum1 = _mm_add_epi32(sum1, T10);

        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    uiSum = _mm_extract_epi32(sum0, 0);

    return uiSum;
}

i32u_t xGetSSE32_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    i32u_t  uiCol;
    const __m128i zero = _mm_setzero_si128();
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01;
    __m128i T10, T11;

    while (height--) {
        for (uiCol = 0; uiCol < 32; uiCol += 16) {
            T00 = _mm_loadu_si128((__m128i*)(p_org + uiCol));
            T01 = _mm_loadu_si128((__m128i*)(p_pred + uiCol));

            T10 = _mm_unpackhi_epi8(T00, zero);
            T11 = _mm_unpackhi_epi8(T01, zero);

            T00 = _mm_unpacklo_epi8(T00, zero);
            T01 = _mm_unpacklo_epi8(T01, zero);

            T00 = _mm_sub_epi16(T00, T01);
            T10 = _mm_sub_epi16(T10, T11);

            T00 = _mm_madd_epi16(T00, T00);
            T10 = _mm_madd_epi16(T10, T10);

            sum0 = _mm_add_epi32(sum0, T00);
            sum1 = _mm_add_epi32(sum1, T10);
        }
        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    uiSum = _mm_extract_epi32(sum0, 0);

    return uiSum;
}

i32u_t xGetSSE64_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    i32u_t  uiCol;
    const __m128i zero = _mm_setzero_si128();
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();
    __m128i T00, T01;
    __m128i T10, T11;

    while (height--) {
        for (uiCol = 0; uiCol < 64; uiCol += 16) {
            T00 = _mm_loadu_si128((__m128i*)(p_org + uiCol));
            T01 = _mm_loadu_si128((__m128i*)(p_pred + uiCol));

            T10 = _mm_unpackhi_epi8(T00, zero);
            T11 = _mm_unpackhi_epi8(T01, zero);

            T00 = _mm_unpacklo_epi8(T00, zero);
            T01 = _mm_unpacklo_epi8(T01, zero);

            T00 = _mm_sub_epi16(T00, T01);
            T10 = _mm_sub_epi16(T10, T11);

            T00 = _mm_madd_epi16(T00, T00);
            T10 = _mm_madd_epi16(T10, T10);

            sum0 = _mm_add_epi32(sum0, T00);
            sum1 = _mm_add_epi32(sum1, T10);
        }
        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm_add_epi32(sum0, sum1);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    sum0 = _mm_hadd_epi32(sum0, sum0);
    uiSum = _mm_extract_epi32(sum0, 0);

    return uiSum;
}

i32u_t xGetSSE_Ext_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight)
{
	i32u_t uiSum = 0;
	int uiCol;
	const __m128i zero = _mm_setzero_si128();
	__m128i sum0 = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i T00, T01;
	__m128i T10, T11;

	if (!(iWidth & 15)) {
		for (; iHeight != 0; iHeight--) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 16) {
				T00 = _mm_loadu_si128((__m128i*)(p_org + uiCol));
				T01 = _mm_loadu_si128((__m128i*)(p_pred + uiCol));

				T10 = _mm_unpackhi_epi8(T00, zero);
				T11 = _mm_unpackhi_epi8(T01, zero);

				T00 = _mm_unpacklo_epi8(T00, zero);
				T01 = _mm_unpacklo_epi8(T01, zero);

				T00 = _mm_sub_epi16(T00, T01);
				T10 = _mm_sub_epi16(T10, T11);

				T00 = _mm_madd_epi16(T00, T00);
				T10 = _mm_madd_epi16(T10, T10);

				sum0 = _mm_add_epi32(sum0, T00);
				sum1 = _mm_add_epi32(sum1, T10);
			}
			p_org += i_org;
			p_pred += i_pred;
		}
		sum0 = _mm_add_epi32(sum0, sum1);
		sum0 = _mm_hadd_epi32(sum0, sum0);
		sum0 = _mm_hadd_epi32(sum0, sum0);
		uiSum = _mm_extract_epi32(sum0, 0);
	}
	else if (!(iWidth & 7)) {
		for (; iHeight != 0; iHeight--) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 8)
			{
				T00 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)(p_org + uiCol)), zero);
				T01 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)(p_pred + uiCol)), zero);

				T00 = _mm_sub_epi16(T00, T01);
				T00 = _mm_madd_epi16(T00, T00);
				sum0 = _mm_add_epi32(sum0, T00);
			}
			p_org += i_org;
			p_pred += i_pred;
		}
		sum0 = _mm_hadd_epi32(sum0, sum0);
		sum0 = _mm_hadd_epi32(sum0, sum0);
		uiSum = _mm_extract_epi32(sum0, 0);
	}
	else {
		for (; iHeight != 0; iHeight-=2) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 4)
			{
				T00 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_org + uiCol)), zero);
				T10 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_org + i_org + uiCol)), zero);
				T01 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_pred + uiCol)), zero);
				T11 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_pred + i_pred + uiCol)), zero);

				T00 = _mm_unpacklo_epi64(T00, T10);
				T01 = _mm_unpacklo_epi64(T01, T11);

				T00 = _mm_sub_epi16(T00, T01);
				T00 = _mm_madd_epi16(T00, T00);

				sum0 = _mm_add_epi32(sum0, T00);
			}
			p_org += (i_org << 1);
			p_pred += (i_pred << 1);
		}
		sum0 = _mm_hadd_epi32(sum0, sum0);
		sum0 = _mm_hadd_epi32(sum0, sum0);
		uiSum = _mm_extract_epi32(sum0, 0);
	}

	return uiSum;
}

i64u_t xGetSSE_Psnr_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight)
{
	i64u_t uiSum = 0;
	int uiCol;
	const __m128i zero = _mm_setzero_si128();
	__m128i sum0 = _mm_setzero_si128();
	__m128i T00, T01;
	__m128i T10, T11;

	if (!(iWidth & 15)) {
		for (; iHeight != 0; iHeight--) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 16) {
				T00 = _mm_loadu_si128((__m128i*)(p_org + uiCol));
				T01 = _mm_loadu_si128((__m128i*)(p_pred + uiCol));

				T10 = _mm_unpackhi_epi8(T00, zero);
				T11 = _mm_unpackhi_epi8(T01, zero);

				T00 = _mm_unpacklo_epi8(T00, zero);
				T01 = _mm_unpacklo_epi8(T01, zero);

				T00 = _mm_sub_epi16(T00, T01);
				T10 = _mm_sub_epi16(T10, T11);

				T00 = _mm_madd_epi16(T00, T00);
				T10 = _mm_madd_epi16(T10, T10);

				T00 = _mm_add_epi32(T00, T10);
                T10 = _mm_unpacklo_epi32(T00, zero);
                T00 = _mm_unpackhi_epi32(T00, zero);
                sum0 = _mm_add_epi64(sum0, _mm_add_epi64(T00, T10));
			}
			p_org  += i_org;
			p_pred += i_pred;
		}
		uiSum = _mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1);
	}
	else if (!(iWidth & 7)) {
		for (; iHeight != 0; iHeight--) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 8)
			{
				T00 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)(p_org + uiCol)), zero);
				T01 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)(p_pred + uiCol)), zero);

				T00 = _mm_sub_epi16(T00, T01);
				T00 = _mm_madd_epi16(T00, T00);
                T10 = _mm_unpacklo_epi32(T00, zero);
                T00 = _mm_unpackhi_epi32(T00, zero);
                sum0 = _mm_add_epi64(sum0, _mm_add_epi64(T00, T10));
			}
			p_org += i_org;
			p_pred += i_pred;
		}
		uiSum = _mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1);
	}
	else {
		for (; iHeight != 0; iHeight-=2) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 4)
			{
				T00 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_org + uiCol)), zero);
				T10 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_org + i_org + uiCol)), zero);
				T01 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_pred + uiCol)), zero);
				T11 = _mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int*)(p_pred + i_pred + uiCol)), zero);

				T00 = _mm_unpacklo_epi64(T00, T10);
				T01 = _mm_unpacklo_epi64(T01, T11);

				T00 = _mm_sub_epi16(T00, T01);
				T00 = _mm_madd_epi16(T00, T00);

                T10 = _mm_unpacklo_epi32(T00, zero);
                T00 = _mm_unpackhi_epi32(T00, zero);
                sum0 = _mm_add_epi64(sum0, _mm_add_epi64(T00, T10));
			}
			p_org += (i_org << 1);
			p_pred += (i_pred << 1);
		}
		uiSum = _mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1);
	}

	return uiSum;
}

i32u_t xGetSSE16_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    __m256i sum0 = _mm256_setzero_si256();
    __m256i T00, T01;

    while (height--) {
        T00 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)(p_org)));
        T01 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)(p_pred)));
        T00 = _mm256_sub_epi16(T00, T01);
        T00 = _mm256_madd_epi16(T00, T00);
        sum0 = _mm256_add_epi32(sum0, T00);
        p_org  += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    return ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));
}

i32u_t xGetSSE32_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum;
    const __m256i zero = _mm256_setzero_si256();
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;

    while (height--) {
        T00 = _mm256_loadu_si256((__m256i*)p_org);
        T01 = _mm256_loadu_si256((__m256i*)p_pred);

        T10 = _mm256_unpackhi_epi8(T00, zero);
        T11 = _mm256_unpackhi_epi8(T01, zero);

        T00 = _mm256_unpacklo_epi8(T00, zero);
        T01 = _mm256_unpacklo_epi8(T01, zero);

        T00 = _mm256_sub_epi16(T00, T01);
        T10 = _mm256_sub_epi16(T10, T11);

        T00 = _mm256_madd_epi16(T00, T00);
        T10 = _mm256_madd_epi16(T10, T10);

        sum0 = _mm256_add_epi32(sum0, T00);
        sum1 = _mm256_add_epi32(sum1, T10);
        
        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm256_add_epi32(sum0, sum1);
    sum0 = _mm256_hadd_epi32(sum0, zero);
    sum0 = _mm256_hadd_epi32(sum0, zero);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    return uiSum;
}

i32u_t xGetSSE64_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    const __m256i zero = _mm256_setzero_si256();
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;

    while (height--) {
        T00 = _mm256_loadu_si256((__m256i*)(p_org ));
        T01 = _mm256_loadu_si256((__m256i*)(p_pred));

        T10 = _mm256_unpackhi_epi8(T00, zero);
        T11 = _mm256_unpackhi_epi8(T01, zero);

        T00 = _mm256_unpacklo_epi8(T00, zero);
        T01 = _mm256_unpacklo_epi8(T01, zero);

        T00 = _mm256_sub_epi16(T00, T01);
        T10 = _mm256_sub_epi16(T10, T11);

        T00 = _mm256_madd_epi16(T00, T00);
        T10 = _mm256_madd_epi16(T10, T10);

        sum0 = _mm256_add_epi32(sum0, T00);
        sum1 = _mm256_add_epi32(sum1, T10);

        T00 = _mm256_loadu_si256((__m256i*)(p_org  + 32));
        T01 = _mm256_loadu_si256((__m256i*)(p_pred + 32));

        T10 = _mm256_unpackhi_epi8(T00, zero);
        T11 = _mm256_unpackhi_epi8(T01, zero);

        T00 = _mm256_unpacklo_epi8(T00, zero);
        T01 = _mm256_unpacklo_epi8(T01, zero);

        T00 = _mm256_sub_epi16(T00, T01);
        T10 = _mm256_sub_epi16(T10, T11);

        T00 = _mm256_madd_epi16(T00, T00);
        T10 = _mm256_madd_epi16(T10, T10);

        sum0 = _mm256_add_epi32(sum0, T00);
        sum1 = _mm256_add_epi32(sum1, T10);

        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm256_add_epi32(sum0, sum1);
    sum0 = _mm256_hadd_epi32(sum0, zero);
    sum0 = _mm256_hadd_epi32(sum0, zero);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    return uiSum;
}

// --------------------------------------------------------------------------------------------------------------------
// HADAMARD with step (used in fractional search)
// --------------------------------------------------------------------------------------------------------------------
i32u_t xCalcHAD4x4_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int uiSum = 0;
    int i_org_x2 = i_org << 1;
    int i_pred_x2 = i_pred << 1;

    __m128i T0,T1,T2,T3,T4,T5,T6,T7;
    __m128i M0, M1, M2, M3;
    __m128i sign1 = _mm_set_epi16(1,-1,1,-1,1,-1,1,-1);
    __m128i sign2 = _mm_set_epi16(1,-1,-1,1,1,-1,-1,1);
    __m128i sign3 = _mm_set_epi16(1,1,-1,-1,1,1,-1,-1);

    T0 = _mm_cvtsi32_si128(*(i32u_t*)p_org);
    T2 = _mm_cvtsi32_si128(*(i32u_t*)(p_org + i_org));
    T0 = _mm_unpacklo_epi32(T0, T2);
    T1 = _mm_cvtsi32_si128(*(i32u_t*)p_pred);
    T3 = _mm_cvtsi32_si128(*(i32u_t*)(p_pred + i_pred));
    T1 = _mm_unpacklo_epi32(T1, T3);
    T0 = _mm_cvtepu8_epi16(T0);
    T1 = _mm_cvtepu8_epi16(T1);
    M0 = _mm_sub_epi16(T0, T1);

    T4 = _mm_cvtsi32_si128(*(i32u_t*)(p_org + i_org_x2));
    T6 = _mm_cvtsi32_si128(*(i32u_t*)(p_org + i_org_x2 + i_org));
    T4 = _mm_unpacklo_epi32(T6, T4);
    T5 = _mm_cvtsi32_si128(*(i32u_t*)(p_pred + i_pred_x2));
    T7 = _mm_cvtsi32_si128(*(i32u_t*)(p_pred + i_pred_x2 + i_pred));
    T5 = _mm_unpacklo_epi32(T7, T5);
    T4 = _mm_cvtepu8_epi16(T4);
    T5 = _mm_cvtepu8_epi16(T5);
    M2 = _mm_sub_epi16(T4, T5);

    /*===== hadamard transform =====*/
    T0 = _mm_add_epi16(M0, M2);
    T3 = _mm_sub_epi16(M0, M2);

    T1 = _mm_unpackhi_epi64(T0, T0);
    T2 = _mm_unpackhi_epi64(T3, T3);

    M0 = _mm_add_epi16(T0, T1); //d0, d1, d2, d3
    M1 = _mm_add_epi16(T2, T3);
    M2 = _mm_sub_epi16(T0, T1);
    M3 = _mm_sub_epi16(T3, T2);
  
    /*
    d'[ 0] = d[ 0] + d[ 3] + d[ 1] + d[ 2];
    d'[ 1] = d[ 0] + d[ 3] - d[ 1] - d[ 2];
    d'[ 2] = d[ 1] - d[ 2] + d[ 0] - d[ 3];
    d'[ 3] = d[ 0] - d[ 3] - d[ 1] + d[ 2];
    */
    T0 = _mm_set1_epi16(_mm_extract_epi16(M0, 0));
    T1 = _mm_set1_epi16(_mm_extract_epi16(M0, 1));
    T2 = _mm_set1_epi16(_mm_extract_epi16(M0, 2));
    T3 = _mm_set1_epi16(_mm_extract_epi16(M0, 3));

    T4 = _mm_set1_epi16(_mm_extract_epi16(M1, 0));
    T5 = _mm_set1_epi16(_mm_extract_epi16(M1, 1));
    T6 = _mm_set1_epi16(_mm_extract_epi16(M1, 2));
    T7 = _mm_set1_epi16(_mm_extract_epi16(M1, 3));

    T0 = _mm_unpacklo_epi64(T0, T4);
    T1 = _mm_unpacklo_epi64(T1, T5);
    T2 = _mm_unpacklo_epi64(T2, T6);
    T3 = _mm_unpacklo_epi64(T3, T7);

    T1 = _mm_sign_epi16(T1, sign1);
    T2 = _mm_sign_epi16(T2, sign2);
    T3 = _mm_sign_epi16(T3, sign3);

    T0 = _mm_add_epi16(T0, T1);
    T2 = _mm_add_epi16(T2, T3);
    M0 = _mm_add_epi16(T0, T2);
  
    T0 = _mm_set1_epi16(_mm_extract_epi16(M2, 0));
    T1 = _mm_set1_epi16(_mm_extract_epi16(M2, 1));
    T2 = _mm_set1_epi16(_mm_extract_epi16(M2, 2));
    T3 = _mm_set1_epi16(_mm_extract_epi16(M2, 3));

    T4 = _mm_set1_epi16(_mm_extract_epi16(M3, 0));
    T5 = _mm_set1_epi16(_mm_extract_epi16(M3, 1));
    T6 = _mm_set1_epi16(_mm_extract_epi16(M3, 2));
    T7 = _mm_set1_epi16(_mm_extract_epi16(M3, 3));

    T0 = _mm_unpacklo_epi64(T0, T4);
    T1 = _mm_unpacklo_epi64(T1, T5);
    T2 = _mm_unpacklo_epi64(T2, T6);
    T3 = _mm_unpacklo_epi64(T3, T7);

    T1 = _mm_sign_epi16(T1, sign1);
    T2 = _mm_sign_epi16(T2, sign2);
    T3 = _mm_sign_epi16(T3, sign3);

    T0 = _mm_add_epi16(T0, T1);
    T2 = _mm_add_epi16(T2, T3);
    M2 = _mm_add_epi16(T0, T2);

    T0 = _mm_abs_epi16(M0);
    T1 = _mm_abs_epi16(M2);

    T0 = _mm_add_epi16(T0, T1);
    T0 = _mm_hadd_epi16(T0, T0);
    T0 = _mm_hadd_epi16(T0, T0);
    T0 = _mm_hadd_epi16(T0, T0);

    uiSum = _mm_extract_epi16(T0, 0);

    uiSum = ((uiSum + 1) >> 1);

    return uiSum;
}

i32u_t xCalcHAD8x8_sse128(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int uiSum = 0;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2, M3, M4, M5, M6, M7;
    const __m128i zero = _mm_setzero_si128();
    __m128i sum;
    __m128i sign = _mm_set_epi16(1,1,-1,-1,1,1,-1,-1);

    // horizontal
#define HOR_LINE(i, j) { \
        T0 = _mm_loadl_epi64((__m128i const*)p_org); \
        T1 = _mm_loadl_epi64((__m128i const*)p_pred); \
        T4 = _mm_loadl_epi64((__m128i const*)(p_org + i_org)); \
        T5 = _mm_loadl_epi64((__m128i const*)(p_pred + i_pred)); \
        T0 = _mm_cvtepu8_epi16(T0); \
        T1 = _mm_cvtepu8_epi16(T1); \
        T4 = _mm_cvtepu8_epi16(T4); \
        T5 = _mm_cvtepu8_epi16(T5); \
        T0 = _mm_sub_epi16(T0, T1); \
        T4 = _mm_sub_epi16(T4, T5); \
        T1 = _mm_unpackhi_epi64(T0, T0); \
        T5 = _mm_unpackhi_epi64(T4, T4); \
        T2 = _mm_add_epi16(T0, T1); \
        T3 = _mm_sub_epi16(T0, T1); \
        T6 = _mm_add_epi16(T4, T5); \
        T7 = _mm_sub_epi16(T4, T5); \
        T0 = _mm_unpacklo_epi32(T2, T3); \
        T4 = _mm_unpacklo_epi32(T6, T7); \
        T1 = _mm_unpackhi_epi32(T0, T0); \
        T2 = _mm_unpacklo_epi32(T0, T0); \
        T5 = _mm_unpackhi_epi32(T4, T4); \
        T6 = _mm_unpacklo_epi32(T4, T4); \
        T1 = _mm_sign_epi16(T1, sign); \
        T5 = _mm_sign_epi16(T5, sign); \
        T0 = _mm_add_epi16(T2, T1); \
        T4 = _mm_add_epi16(T6, T5); \
        T1 = _mm_hadd_epi16(T0, T0); \
        T2 = _mm_hsub_epi16(T0, T0); \
        T5 = _mm_hadd_epi16(T4, T4); \
        T6 = _mm_hsub_epi16(T4, T4); \
        M##i = _mm_unpacklo_epi16(T1, T2); \
        M##j = _mm_unpacklo_epi16(T5, T6); \
        p_pred += (i_pred << 1); \
        p_org += (i_org << 1); \
    }

    HOR_LINE(0, 1)
    HOR_LINE(2, 3)
    HOR_LINE(4, 5)
    HOR_LINE(6, 7)
 
#undef HOR_LINE

    // vertical
    T0 = _mm_add_epi16(M0, M4);
    T1 = _mm_add_epi16(M1, M5);
    T2 = _mm_add_epi16(M2, M6);
    T3 = _mm_add_epi16(M3, M7);
    T4 = _mm_sub_epi16(M0, M4);
    T5 = _mm_sub_epi16(M1, M5);
    T6 = _mm_sub_epi16(M2, M6);
    T7 = _mm_sub_epi16(M3, M7);

    M0 = _mm_add_epi16(T0, T2);
    M1 = _mm_add_epi16(T1, T3);
    M2 = _mm_sub_epi16(T0, T2);
    M3 = _mm_sub_epi16(T1, T3);
    M4 = _mm_add_epi16(T4, T6);
    M5 = _mm_add_epi16(T5, T7);
    M6 = _mm_sub_epi16(T4, T6);
    M7 = _mm_sub_epi16(T5, T7);

    T0 = _mm_abs_epi16(_mm_add_epi16(M0, M1));
    T1 = _mm_abs_epi16(_mm_sub_epi16(M0, M1));
    T2 = _mm_abs_epi16(_mm_add_epi16(M2, M3));
    T3 = _mm_abs_epi16(_mm_sub_epi16(M2, M3));
    T4 = _mm_abs_epi16(_mm_add_epi16(M4, M5));
    T5 = _mm_abs_epi16(_mm_sub_epi16(M4, M5));
    T6 = _mm_abs_epi16(_mm_add_epi16(M6, M7));
    T7 = _mm_abs_epi16(_mm_sub_epi16(M6, M7));

    T0 = _mm_add_epi16(T0, T1);
    T2 = _mm_add_epi16(T2, T3);
    T4 = _mm_add_epi16(T4, T5);
    T6 = _mm_add_epi16(T6, T7);

    T0 = _mm_add_epi16(T0, T2);
    T4 = _mm_add_epi16(T4, T6);

    M0 = _mm_cvtepi16_epi32(T0);
    M2 = _mm_cvtepi16_epi32(T4);
    M4 = _mm_unpackhi_epi16(T0, zero);
    M6 = _mm_unpackhi_epi16(T4, zero);

    M0 = _mm_add_epi32(M0, M2);
    M4 = _mm_add_epi32(M4, M6);

    sum = _mm_add_epi32(M0, M4);

    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    uiSum = _mm_cvtsi128_si32(sum);
    uiSum = ((uiSum + 2) >> 2);

    return uiSum;
}

i32u_t xCalcHAD16x16_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int i, uiSum = 0;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    const __m256i zero = _mm256_setzero_si256();
    __m256i sum;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);

    for (i = 0; i < 2; i++) {
        // horizontal
#define HOR_LINE(i, j) { \
    T0 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)p_org )); \
    T1 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)p_pred)); \
    T4 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)(p_org  + i_org ))); \
    T5 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)(p_pred + i_pred))); \
    T0 = _mm256_sub_epi16(T0, T1); \
    T4 = _mm256_sub_epi16(T4, T5); \
    T1 = _mm256_unpackhi_epi64(T0, T0); \
    T5 = _mm256_unpackhi_epi64(T4, T4); \
    T2 = _mm256_add_epi16(T0, T1); \
    T3 = _mm256_sub_epi16(T0, T1); \
    T6 = _mm256_add_epi16(T4, T5); \
    T7 = _mm256_sub_epi16(T4, T5); \
    T0 = _mm256_unpacklo_epi32(T2, T3); \
    T4 = _mm256_unpacklo_epi32(T6, T7); \
    T1 = _mm256_unpackhi_epi32(T0, T0); \
    T2 = _mm256_unpacklo_epi32(T0, T0); \
    T5 = _mm256_unpackhi_epi32(T4, T4); \
    T6 = _mm256_unpacklo_epi32(T4, T4); \
    T1 = _mm256_sign_epi16(T1, sign); \
    T5 = _mm256_sign_epi16(T5, sign); \
    T0 = _mm256_add_epi16(T2, T1); \
    T4 = _mm256_add_epi16(T6, T5); \
    T1 = _mm256_hadd_epi16(T0, T0); \
    T2 = _mm256_hsub_epi16(T0, T0); \
    T5 = _mm256_hadd_epi16(T4, T4); \
    T6 = _mm256_hsub_epi16(T4, T4); \
    M##i = _mm256_unpacklo_epi16(T1, T2); \
    M##j = _mm256_unpacklo_epi16(T5, T6); \
    p_pred += (i_pred << 1); \
    p_org  += (i_org << 1); \
}

        HOR_LINE(0, 1);
        HOR_LINE(2, 3);
        HOR_LINE(4, 5);
        HOR_LINE(6, 7);

#undef HOR_LINE

        // vertical
        T0 = _mm256_add_epi16(M0, M4);
        T1 = _mm256_add_epi16(M1, M5);
        T2 = _mm256_add_epi16(M2, M6);
        T3 = _mm256_add_epi16(M3, M7);
        T4 = _mm256_sub_epi16(M0, M4);
        T5 = _mm256_sub_epi16(M1, M5);
        T6 = _mm256_sub_epi16(M2, M6);
        T7 = _mm256_sub_epi16(M3, M7);

        M0 = _mm256_add_epi16(T0, T2);
        M1 = _mm256_add_epi16(T1, T3);
        M2 = _mm256_sub_epi16(T0, T2);
        M3 = _mm256_sub_epi16(T1, T3);
        M4 = _mm256_add_epi16(T4, T6);
        M5 = _mm256_add_epi16(T5, T7);
        M6 = _mm256_sub_epi16(T4, T6);
        M7 = _mm256_sub_epi16(T5, T7);

        T0 = _mm256_abs_epi16(_mm256_add_epi16(M0, M1));
        T1 = _mm256_abs_epi16(_mm256_sub_epi16(M0, M1));
        T2 = _mm256_abs_epi16(_mm256_add_epi16(M2, M3));
        T3 = _mm256_abs_epi16(_mm256_sub_epi16(M2, M3));
        T4 = _mm256_abs_epi16(_mm256_add_epi16(M4, M5));
        T5 = _mm256_abs_epi16(_mm256_sub_epi16(M4, M5));
        T6 = _mm256_abs_epi16(_mm256_add_epi16(M6, M7));
        T7 = _mm256_abs_epi16(_mm256_sub_epi16(M6, M7));

        T0 = _mm256_add_epi16(T0, T1);
        T2 = _mm256_add_epi16(T2, T3);
        T4 = _mm256_add_epi16(T4, T5);
        T6 = _mm256_add_epi16(T6, T7);

        T0 = _mm256_add_epi16(T0, T2);
        T4 = _mm256_add_epi16(T4, T6);

        M0 = _mm256_unpacklo_epi16(T0, zero);
        M2 = _mm256_unpacklo_epi16(T4, zero);
        M4 = _mm256_unpackhi_epi16(T0, zero);
        M6 = _mm256_unpackhi_epi16(T4, zero);

        M0 = _mm256_add_epi32(M0, M2);
        M4 = _mm256_add_epi32(M4, M6);

        sum = _mm256_add_epi32(M0, M4);

        sum = _mm256_hadd_epi32(sum, sum);
        sum = _mm256_hadd_epi32(sum, sum);


        uiSum += ((((i32u_t)_mm256_extract_epi32(sum, 0)) + 2) >> 2) + ((((i32u_t)_mm256_extract_epi32(sum, 4)) + 2) >> 2);
    }
    return uiSum;
}

i32u_t xCalcHAD32x32_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int i, j, uiSum = 0;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    const __m256i zero = _mm256_setzero_si256();
    __m256i sum;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);
    pel_t *p_org_bak = p_org;
    pel_t *p_pred_bak = p_pred;

    for (i = 0; i < 2; i++) {
        p_org = p_org_bak + i * 16;
        p_pred = p_pred_bak + i * 16;

        for (j = 0; j < 4; j++) {
            // horizontal
#define HOR_LINE(i, j) { \
    T0 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)p_org )); \
    T1 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)p_pred)); \
    T4 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)(p_org  + i_org ))); \
    T5 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)(p_pred + i_pred))); \
    T0 = _mm256_sub_epi16(T0, T1); \
    T4 = _mm256_sub_epi16(T4, T5); \
    T1 = _mm256_unpackhi_epi64(T0, T0); \
    T5 = _mm256_unpackhi_epi64(T4, T4); \
    T2 = _mm256_add_epi16(T0, T1); \
    T3 = _mm256_sub_epi16(T0, T1); \
    T6 = _mm256_add_epi16(T4, T5); \
    T7 = _mm256_sub_epi16(T4, T5); \
    T0 = _mm256_unpacklo_epi32(T2, T3); \
    T4 = _mm256_unpacklo_epi32(T6, T7); \
    T1 = _mm256_unpackhi_epi32(T0, T0); \
    T2 = _mm256_unpacklo_epi32(T0, T0); \
    T5 = _mm256_unpackhi_epi32(T4, T4); \
    T6 = _mm256_unpacklo_epi32(T4, T4); \
    T1 = _mm256_sign_epi16(T1, sign); \
    T5 = _mm256_sign_epi16(T5, sign); \
    T0 = _mm256_add_epi16(T2, T1); \
    T4 = _mm256_add_epi16(T6, T5); \
    T1 = _mm256_hadd_epi16(T0, T0); \
    T2 = _mm256_hsub_epi16(T0, T0); \
    T5 = _mm256_hadd_epi16(T4, T4); \
    T6 = _mm256_hsub_epi16(T4, T4); \
    M##i = _mm256_unpacklo_epi16(T1, T2); \
    M##j = _mm256_unpacklo_epi16(T5, T6); \
    p_pred += (i_pred << 1); \
    p_org  += (i_org << 1); \
}

            HOR_LINE(0, 1);
            HOR_LINE(2, 3);
            HOR_LINE(4, 5);
            HOR_LINE(6, 7);

#undef HOR_LINE

            // vertical
            T0 = _mm256_add_epi16(M0, M4);
            T1 = _mm256_add_epi16(M1, M5);
            T2 = _mm256_add_epi16(M2, M6);
            T3 = _mm256_add_epi16(M3, M7);
            T4 = _mm256_sub_epi16(M0, M4);
            T5 = _mm256_sub_epi16(M1, M5);
            T6 = _mm256_sub_epi16(M2, M6);
            T7 = _mm256_sub_epi16(M3, M7);

            M0 = _mm256_add_epi16(T0, T2);
            M1 = _mm256_add_epi16(T1, T3);
            M2 = _mm256_sub_epi16(T0, T2);
            M3 = _mm256_sub_epi16(T1, T3);
            M4 = _mm256_add_epi16(T4, T6);
            M5 = _mm256_add_epi16(T5, T7);
            M6 = _mm256_sub_epi16(T4, T6);
            M7 = _mm256_sub_epi16(T5, T7);

            T0 = _mm256_abs_epi16(_mm256_add_epi16(M0, M1));
            T1 = _mm256_abs_epi16(_mm256_sub_epi16(M0, M1));
            T2 = _mm256_abs_epi16(_mm256_add_epi16(M2, M3));
            T3 = _mm256_abs_epi16(_mm256_sub_epi16(M2, M3));
            T4 = _mm256_abs_epi16(_mm256_add_epi16(M4, M5));
            T5 = _mm256_abs_epi16(_mm256_sub_epi16(M4, M5));
            T6 = _mm256_abs_epi16(_mm256_add_epi16(M6, M7));
            T7 = _mm256_abs_epi16(_mm256_sub_epi16(M6, M7));

            T0 = _mm256_add_epi16(T0, T1);
            T2 = _mm256_add_epi16(T2, T3);
            T4 = _mm256_add_epi16(T4, T5);
            T6 = _mm256_add_epi16(T6, T7);

            T0 = _mm256_add_epi16(T0, T2);
            T4 = _mm256_add_epi16(T4, T6);

            M0 = _mm256_unpacklo_epi16(T0, zero);
            M2 = _mm256_unpacklo_epi16(T4, zero);
            M4 = _mm256_unpackhi_epi16(T0, zero);
            M6 = _mm256_unpackhi_epi16(T4, zero);

            M0 = _mm256_add_epi32(M0, M2);
            M4 = _mm256_add_epi32(M4, M6);

            sum = _mm256_add_epi32(M0, M4);

            sum = _mm256_hadd_epi32(sum, sum);
            sum = _mm256_hadd_epi32(sum, sum);


            uiSum += ((((i32u_t)_mm256_extract_epi32(sum, 0)) + 2) >> 2) + ((((i32u_t)_mm256_extract_epi32(sum, 4)) + 2) >> 2);
        }
    }
    return uiSum;
}

i32u_t xCalcHAD64x64_sse256(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int i, j, uiSum = 0;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    const __m256i zero = _mm256_setzero_si256();
    __m256i sum;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);
    pel_t *p_org_bak = p_org;
    pel_t *p_pred_bak = p_pred;

    for (i = 0; i < 4; i++) {
        p_org = p_org_bak + i * 16;
        p_pred = p_pred_bak + i * 16;

        for (j = 0; j < 8; j++) {
            // horizontal
#define HOR_LINE(i, j) { \
    T0 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)p_org )); \
    T1 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)p_pred)); \
    T4 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)(p_org  + i_org ))); \
    T5 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i const*)(p_pred + i_pred))); \
    T0 = _mm256_sub_epi16(T0, T1); \
    T4 = _mm256_sub_epi16(T4, T5); \
    T1 = _mm256_unpackhi_epi64(T0, T0); \
    T5 = _mm256_unpackhi_epi64(T4, T4); \
    T2 = _mm256_add_epi16(T0, T1); \
    T3 = _mm256_sub_epi16(T0, T1); \
    T6 = _mm256_add_epi16(T4, T5); \
    T7 = _mm256_sub_epi16(T4, T5); \
    T0 = _mm256_unpacklo_epi32(T2, T3); \
    T4 = _mm256_unpacklo_epi32(T6, T7); \
    T1 = _mm256_unpackhi_epi32(T0, T0); \
    T2 = _mm256_unpacklo_epi32(T0, T0); \
    T5 = _mm256_unpackhi_epi32(T4, T4); \
    T6 = _mm256_unpacklo_epi32(T4, T4); \
    T1 = _mm256_sign_epi16(T1, sign); \
    T5 = _mm256_sign_epi16(T5, sign); \
    T0 = _mm256_add_epi16(T2, T1); \
    T4 = _mm256_add_epi16(T6, T5); \
    T1 = _mm256_hadd_epi16(T0, T0); \
    T2 = _mm256_hsub_epi16(T0, T0); \
    T5 = _mm256_hadd_epi16(T4, T4); \
    T6 = _mm256_hsub_epi16(T4, T4); \
    M##i = _mm256_unpacklo_epi16(T1, T2); \
    M##j = _mm256_unpacklo_epi16(T5, T6); \
    p_pred += (i_pred << 1); \
    p_org  += (i_org << 1); \
}

            HOR_LINE(0, 1);
            HOR_LINE(2, 3);
            HOR_LINE(4, 5);
            HOR_LINE(6, 7);

#undef HOR_LINE

            // vertical
            T0 = _mm256_add_epi16(M0, M4);
            T1 = _mm256_add_epi16(M1, M5);
            T2 = _mm256_add_epi16(M2, M6);
            T3 = _mm256_add_epi16(M3, M7);
            T4 = _mm256_sub_epi16(M0, M4);
            T5 = _mm256_sub_epi16(M1, M5);
            T6 = _mm256_sub_epi16(M2, M6);
            T7 = _mm256_sub_epi16(M3, M7);

            M0 = _mm256_add_epi16(T0, T2);
            M1 = _mm256_add_epi16(T1, T3);
            M2 = _mm256_sub_epi16(T0, T2);
            M3 = _mm256_sub_epi16(T1, T3);
            M4 = _mm256_add_epi16(T4, T6);
            M5 = _mm256_add_epi16(T5, T7);
            M6 = _mm256_sub_epi16(T4, T6);
            M7 = _mm256_sub_epi16(T5, T7);

            T0 = _mm256_abs_epi16(_mm256_add_epi16(M0, M1));
            T1 = _mm256_abs_epi16(_mm256_sub_epi16(M0, M1));
            T2 = _mm256_abs_epi16(_mm256_add_epi16(M2, M3));
            T3 = _mm256_abs_epi16(_mm256_sub_epi16(M2, M3));
            T4 = _mm256_abs_epi16(_mm256_add_epi16(M4, M5));
            T5 = _mm256_abs_epi16(_mm256_sub_epi16(M4, M5));
            T6 = _mm256_abs_epi16(_mm256_add_epi16(M6, M7));
            T7 = _mm256_abs_epi16(_mm256_sub_epi16(M6, M7));

            T0 = _mm256_add_epi16(T0, T1);
            T2 = _mm256_add_epi16(T2, T3);
            T4 = _mm256_add_epi16(T4, T5);
            T6 = _mm256_add_epi16(T6, T7);

            T0 = _mm256_add_epi16(T0, T2);
            T4 = _mm256_add_epi16(T4, T6);

            M0 = _mm256_unpacklo_epi16(T0, zero);
            M2 = _mm256_unpacklo_epi16(T4, zero);
            M4 = _mm256_unpackhi_epi16(T0, zero);
            M6 = _mm256_unpackhi_epi16(T4, zero);

            M0 = _mm256_add_epi32(M0, M2);
            M4 = _mm256_add_epi32(M4, M6);

            sum = _mm256_add_epi32(M0, M4);

            sum = _mm256_hadd_epi32(sum, sum);
            sum = _mm256_hadd_epi32(sum, sum);


            uiSum += ((((i32u_t)_mm256_extract_epi32(sum, 0)) + 2) >> 2) + ((((i32u_t)_mm256_extract_epi32(sum, 4)) + 2) >> 2);
        }
    }
    return uiSum;
}


// --------------------------------------------------------------------------------------------------------------------
// SAD
// --------------------------------------------------------------------------------------------------------------------
i32u_t xGetSAD4_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	int i_org_x2 = i_org << 1;
	int i_pred_x2 = i_pred << 1;
	__m128i T0, T1, T2, T3;
	__m128i zero = _mm_setzero_si128();
	__m128i sum = _mm_setzero_si128();
	height >>= (skip_lines + 1);

	for (; height != 0; height--) {
		T0 = _mm_loadl_epi64((__m128i*)p_org);
		T1 = _mm_loadl_epi64((__m128i*)(p_org + i_org));
		T0 = _mm_unpacklo_epi64(T0, T1);

		T2 = _mm_loadl_epi64((__m128i*)p_pred);
		T3 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred));
		T2 = _mm_unpacklo_epi64(T2, T3);

		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum = _mm_adds_epi16(sum, T0);

		p_org  += i_org_x2;
		p_pred += i_pred_x2;
	}

	T0 = _mm_unpacklo_epi16(sum, zero);
	T1 = _mm_unpackhi_epi16(sum, zero);

	T0 = _mm_add_epi16(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetSAD8_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T0, T1, T2;
	__m128i zero = _mm_setzero_si128();
	__m128i sum = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--) {
		T0 = _mm_loadu_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred);

		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum = _mm_adds_epi16(sum, T0);

		p_org += i_org;
		p_pred += i_pred;
	}

	T0 = _mm_unpacklo_epi16(sum, zero);
	T1 = _mm_unpackhi_epi16(sum, zero);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetSAD16_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T0, T1, T2, T3;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T0 = _mm_load_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred);

		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum1 = _mm_add_epi16(sum1, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 8));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 8));

		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum2 = _mm_add_epi16(sum2, T0);

		p_org += i_org;
		p_pred += i_pred;
	}

	T0 = _mm_unpacklo_epi16(sum1, zero);
	T1 = _mm_unpackhi_epi16(sum1, zero);

	T2 = _mm_unpacklo_epi16(sum2, zero);
	T3 = _mm_unpackhi_epi16(sum2, zero);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetSAD32_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T0, T1, T2, T3;
	__m128i T4, T5, T6, T7;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T0 = _mm_load_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum1 = _mm_add_epi16(sum1, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 8));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 8));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum2 = _mm_add_epi16(sum2, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 16));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 16));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum3 = _mm_add_epi16(sum3, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 24));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 24));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum4 = _mm_add_epi16(sum4, T0);

		p_org += i_org;
		p_pred += i_pred;
	}

	T0 = _mm_unpacklo_epi16(sum1, zero);
	T1 = _mm_unpackhi_epi16(sum1, zero);

	T2 = _mm_unpacklo_epi16(sum2, zero);
	T3 = _mm_unpackhi_epi16(sum2, zero);

	T4 = _mm_unpacklo_epi16(sum3, zero);
	T5 = _mm_unpackhi_epi16(sum3, zero);

	T6 = _mm_unpacklo_epi16(sum4, zero);
	T7 = _mm_unpackhi_epi16(sum4, zero);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T2 = _mm_add_epi32(T4, T5);
	T3 = _mm_add_epi32(T6, T7);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetSAD64_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T0, T1, T2, T3;
	__m128i T4, T5, T6, T7;
	__m128i T8, T9;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();
	__m128i sum5 = _mm_setzero_si128();
	__m128i sum6 = _mm_setzero_si128();
	__m128i sum7 = _mm_setzero_si128();
	__m128i sum8 = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T0 = _mm_load_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum1 = _mm_add_epi16(sum1, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 8));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 8));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum2 = _mm_add_epi16(sum2, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 16));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 16));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum3 = _mm_add_epi16(sum3, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 24));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 24));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum4 = _mm_add_epi16(sum4, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 32));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 32));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum5 = _mm_add_epi16(sum5, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 40));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 40));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum6 = _mm_add_epi16(sum6, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 48));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 48));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum7 = _mm_add_epi16(sum7, T0);

		T0 = _mm_load_si128((__m128i*)(p_org + 56));
		T2 = _mm_loadu_si128((__m128i*)(p_pred + 56));
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum8 = _mm_add_epi16(sum8, T0);

		p_org += i_org;
		p_pred += i_pred;
	}

	T0 = _mm_unpacklo_epi16(sum1, zero);
	T1 = _mm_unpackhi_epi16(sum1, zero);
	T2 = _mm_unpacklo_epi16(sum2, zero);
	T3 = _mm_unpackhi_epi16(sum2, zero);
	T4 = _mm_unpacklo_epi16(sum3, zero);
	T5 = _mm_unpackhi_epi16(sum3, zero);
	T6 = _mm_unpacklo_epi16(sum4, zero);
	T7 = _mm_unpackhi_epi16(sum4, zero);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);
	T2 = _mm_add_epi32(T4, T5);
	T3 = _mm_add_epi32(T6, T7);
	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T8 = _mm_unpacklo_epi16(sum5, zero);
	T9 = _mm_unpackhi_epi16(sum5, zero);
	T2 = _mm_unpacklo_epi16(sum6, zero);
	T3 = _mm_unpackhi_epi16(sum6, zero);
	T4 = _mm_unpacklo_epi16(sum7, zero);
	T5 = _mm_unpackhi_epi16(sum7, zero);
	T6 = _mm_unpacklo_epi16(sum8, zero);
	T7 = _mm_unpackhi_epi16(sum8, zero);

	T8 = _mm_add_epi32(T8, T2);
	T9 = _mm_add_epi32(T9, T3);
	T2 = _mm_add_epi32(T4, T5);
	T3 = _mm_add_epi32(T6, T7);
	T8 = _mm_add_epi32(T8, T2);
	T9 = _mm_add_epi32(T9, T3);

	T0 = _mm_add_epi32(T0, T8);
	T1 = _mm_add_epi32(T1, T9);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetSAD16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i zero = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_loadu_si256((__m256i*)(p_org));
        T10 = _mm256_loadu_si256((__m256i*)(p_pred));

        T00 = _mm256_sub_epi16(T00, T10);
        T00 = _mm256_abs_epi16(T00);
        sum0 = _mm256_add_epi16(sum0, T00);

        p_org += i_org;
        p_pred += i_pred;
    }

    T00 = _mm256_unpacklo_epi16(sum0, zero);
    T02 = _mm256_unpackhi_epi16(sum0, zero);

    sum0 = _mm256_add_epi32(T00, T02);

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetSAD32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i zero = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10, T12;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + 16));

        T10 = _mm256_loadu_si256((__m256i*)(p_pred));
        T12 = _mm256_loadu_si256((__m256i*)(p_pred + 16));

        T00 = _mm256_sub_epi16(T00, T10);
        T00 = _mm256_abs_epi16(T00);
        sum0 = _mm256_add_epi16(sum0, T00);

        T02 = _mm256_sub_epi16(T02, T12);
        T02 = _mm256_abs_epi16(T02);
        sum0 = _mm256_add_epi16(sum0, T02);

        p_org += i_org;
        p_pred += i_pred;
    }

    T00 = _mm256_unpacklo_epi16(sum0, zero);
    T02 = _mm256_unpackhi_epi16(sum0, zero);

    sum0 = _mm256_add_epi32(T00, T02);

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetSAD64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i zero = _mm256_setzero_si256();
    __m256i T00, T01, T02, T03;
    __m256i T10, T11, T12, T13;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T01 = _mm256_load_si256((__m256i*)(p_org + 16));
        T02 = _mm256_load_si256((__m256i*)(p_org + 32));
        T03 = _mm256_load_si256((__m256i*)(p_org + 48));

        T10 = _mm256_loadu_si256((__m256i*)(p_pred));
        T11 = _mm256_loadu_si256((__m256i*)(p_pred + 16));
        T12 = _mm256_loadu_si256((__m256i*)(p_pred + 32));
        T13 = _mm256_loadu_si256((__m256i*)(p_pred + 48));

        T00 = _mm256_sub_epi16(T00, T10);
        T00 = _mm256_abs_epi16(T00);
        sum0 = _mm256_add_epi16(sum0, T00);

        T01 = _mm256_sub_epi16(T01, T11);
        T01 = _mm256_abs_epi16(T01);
        sum1 = _mm256_add_epi16(sum1, T01);

        T02 = _mm256_sub_epi16(T02, T12);
        T02 = _mm256_abs_epi16(T02);
        sum2 = _mm256_add_epi16(sum2, T02);

        T03 = _mm256_sub_epi16(T03, T13);
        T03 = _mm256_abs_epi16(T03);
        sum3 = _mm256_add_epi16(sum3, T03);

        p_org += i_org;
        p_pred += i_pred;
    }

    T00 = _mm256_unpacklo_epi16(sum0, zero);
    T10 = _mm256_unpackhi_epi16(sum0, zero);

    T01 = _mm256_unpacklo_epi16(sum1, zero);
    T11 = _mm256_unpackhi_epi16(sum1, zero);

    T02 = _mm256_unpacklo_epi16(sum2, zero);
    T12 = _mm256_unpackhi_epi16(sum2, zero);

    T03 = _mm256_unpacklo_epi16(sum3, zero);
    T13 = _mm256_unpackhi_epi16(sum3, zero);

    sum0 = _mm256_add_epi32(T00, T10);
    sum1 = _mm256_add_epi32(T01, T11);
    sum2 = _mm256_add_epi32(T02, T12);
    sum3 = _mm256_add_epi32(T03, T13);

    sum0 = _mm256_add_epi32(sum0, sum1);
    sum1 = _mm256_add_epi32(sum2, sum3);
    sum0 = _mm256_add_epi32(sum0, sum1);

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

// --------------------------------------------------------------------------------------------------------------------
// SSE
// --------------------------------------------------------------------------------------------------------------------
i32u_t xGetSSE4_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
	i32u_t uiSum = 0;

	__m128i sum = _mm_setzero_si128();
	__m128i T00, T01;
	__m128i T10, T11;

	height >>= 1;

	while (height--) {
		T00 = _mm_loadl_epi64((__m128i*)p_org);
		T10 = _mm_loadl_epi64((__m128i*)(p_org + i_org));
		T01 = _mm_loadl_epi64((__m128i*)p_pred);
		T11 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred));

		T00 = _mm_unpacklo_epi64(T00, T10);
		T01 = _mm_unpacklo_epi64(T01, T11);

		T00 = _mm_sub_epi16(T00, T01);
		T00 = _mm_madd_epi16(T00, T00);

		sum = _mm_add_epi32(sum, T00);

		p_org += (i_org << 1);
		p_pred += (i_pred << 1);
	}

	sum = _mm_hadd_epi32(sum, sum);
	sum = _mm_hadd_epi32(sum, sum);
	uiSum = _mm_extract_epi32(sum, 0);

	return uiSum;
}

i32u_t xGetSSE8_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
	i32u_t uiSum = 0;

	__m128i sum = _mm_setzero_si128();
	__m128i T00, T01;

	while (height--) {
		T00 = _mm_loadu_si128((__m128i*)p_org);
		T01 = _mm_loadu_si128((__m128i*)p_pred);

		T00 = _mm_sub_epi16(T00, T01);
		T00 = _mm_madd_epi16(T00, T00);
		sum = _mm_add_epi32(sum, T00);

		p_org += i_org;
		p_pred += i_pred;
	}

	sum = _mm_hadd_epi32(sum, sum);
	sum = _mm_hadd_epi32(sum, sum);
	uiSum = _mm_extract_epi32(sum, 0);

	return uiSum;
}

i32u_t xGetSSE16_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
	i32u_t uiSum = 0;

	__m128i sum0 = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i T00, T01;
	__m128i T10, T11;

	while (height--) {
		T00 = _mm_loadu_si128((__m128i*)(p_org));
		T01 = _mm_loadu_si128((__m128i*)(p_pred));

		T10 = _mm_loadu_si128((__m128i*)(p_org + 8));
		T11 = _mm_loadu_si128((__m128i*)(p_pred + 8));

		T00 = _mm_sub_epi16(T00, T01);
		T10 = _mm_sub_epi16(T10, T11);

		T00 = _mm_madd_epi16(T00, T00);
		T10 = _mm_madd_epi16(T10, T10);

		sum0 = _mm_add_epi32(sum0, T00);
		sum1 = _mm_add_epi32(sum1, T10);

		p_org += i_org;
		p_pred += i_pred;
	}

	sum0 = _mm_add_epi32(sum0, sum1);
	sum0 = _mm_hadd_epi32(sum0, sum0);
	sum0 = _mm_hadd_epi32(sum0, sum0);
	uiSum = _mm_extract_epi32(sum0, 0);

	return uiSum;
}

i32u_t xGetSSE32_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
	i32u_t uiSum = 0;

	__m128i sum0 = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i T00, T01;
	__m128i T10, T11;

	while (height--) {

		T00 = _mm_loadu_si128((__m128i*)(p_org));
		T01 = _mm_loadu_si128((__m128i*)(p_pred));

		T10 = _mm_loadu_si128((__m128i*)(p_org + 8));
		T11 = _mm_loadu_si128((__m128i*)(p_pred + 8));

		T00 = _mm_sub_epi16(T00, T01);
		T10 = _mm_sub_epi16(T10, T11);

		T00 = _mm_madd_epi16(T00, T00);
		T10 = _mm_madd_epi16(T10, T10);

		sum0 = _mm_add_epi32(sum0, T00);
		sum1 = _mm_add_epi32(sum1, T10);

		T00 = _mm_loadu_si128((__m128i*)(p_org + 16));
		T01 = _mm_loadu_si128((__m128i*)(p_pred + 16));

		T10 = _mm_loadu_si128((__m128i*)(p_org + 24));
		T11 = _mm_loadu_si128((__m128i*)(p_pred + 24));

		T00 = _mm_sub_epi16(T00, T01);
		T10 = _mm_sub_epi16(T10, T11);

		T00 = _mm_madd_epi16(T00, T00);
		T10 = _mm_madd_epi16(T10, T10);

		sum0 = _mm_add_epi32(sum0, T00);
		sum1 = _mm_add_epi32(sum1, T10);

		p_org += i_org;
		p_pred += i_pred;
	}

	sum0 = _mm_add_epi32(sum0, sum1);
	sum0 = _mm_hadd_epi32(sum0, sum0);
	sum0 = _mm_hadd_epi32(sum0, sum0);
	uiSum = _mm_extract_epi32(sum0, 0);

	return uiSum;
}

i32u_t xGetSSE64_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
	i32u_t uiSum = 0;

	__m128i sum0 = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i T00, T01;
	__m128i T10, T11;

	while (height--) {

		T00 = _mm_loadu_si128((__m128i*)(p_org));
		T01 = _mm_loadu_si128((__m128i*)(p_pred));

		T10 = _mm_loadu_si128((__m128i*)(p_org + 8));
		T11 = _mm_loadu_si128((__m128i*)(p_pred + 8));

		T00 = _mm_sub_epi16(T00, T01);
		T10 = _mm_sub_epi16(T10, T11);

		T00 = _mm_madd_epi16(T00, T00);
		T10 = _mm_madd_epi16(T10, T10);

		sum0 = _mm_add_epi32(sum0, T00);
		sum1 = _mm_add_epi32(sum1, T10);

		T00 = _mm_loadu_si128((__m128i*)(p_org + 16));
		T01 = _mm_loadu_si128((__m128i*)(p_pred + 16));

		T10 = _mm_loadu_si128((__m128i*)(p_org + 24));
		T11 = _mm_loadu_si128((__m128i*)(p_pred + 24));

		T00 = _mm_sub_epi16(T00, T01);
		T10 = _mm_sub_epi16(T10, T11);

		T00 = _mm_madd_epi16(T00, T00);
		T10 = _mm_madd_epi16(T10, T10);

		sum0 = _mm_add_epi32(sum0, T00);
		sum1 = _mm_add_epi32(sum1, T10);

		T00 = _mm_loadu_si128((__m128i*)(p_org + 32));
		T01 = _mm_loadu_si128((__m128i*)(p_pred + 32));

		T10 = _mm_loadu_si128((__m128i*)(p_org + 40));
		T11 = _mm_loadu_si128((__m128i*)(p_pred + 40));

		T00 = _mm_sub_epi16(T00, T01);
		T10 = _mm_sub_epi16(T10, T11);

		T00 = _mm_madd_epi16(T00, T00);
		T10 = _mm_madd_epi16(T10, T10);

		sum0 = _mm_add_epi32(sum0, T00);
		sum1 = _mm_add_epi32(sum1, T10);

		T00 = _mm_loadu_si128((__m128i*)(p_org + 48));
		T01 = _mm_loadu_si128((__m128i*)(p_pred + 48));

		T10 = _mm_loadu_si128((__m128i*)(p_org + 56));
		T11 = _mm_loadu_si128((__m128i*)(p_pred + 56));

		T00 = _mm_sub_epi16(T00, T01);
		T10 = _mm_sub_epi16(T10, T11);

		T00 = _mm_madd_epi16(T00, T00);
		T10 = _mm_madd_epi16(T10, T10);

		sum0 = _mm_add_epi32(sum0, T00);
		sum1 = _mm_add_epi32(sum1, T10);

		p_org += i_org;
		p_pred += i_pred;
	}

	sum0 = _mm_add_epi32(sum0, sum1);
	sum0 = _mm_hadd_epi32(sum0, sum0);
	sum0 = _mm_hadd_epi32(sum0, sum0);
	uiSum = _mm_extract_epi32(sum0, 0);

	return uiSum;
}

i32u_t xGetSSE_Ext_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight)
{
	i32u_t uiSum = 0;
	int uiCol;

	__m128i sum0 = _mm_setzero_si128();
	__m128i T00, T01;
	__m128i T10, T11;

	if (!(iWidth & 7)) {
		for (; iHeight != 0; iHeight--) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 8)
			{
				T00 = _mm_loadu_si128((__m128i*)(p_org + uiCol));
				T01 = _mm_loadu_si128((__m128i*)(p_pred + uiCol));

				T00 = _mm_sub_epi16(T00, T01);
				T00 = _mm_madd_epi16(T00, T00);
				sum0 = _mm_add_epi32(sum0, T00);
			}
			p_org += i_org;
			p_pred += i_pred;
		}
		sum0 = _mm_hadd_epi32(sum0, sum0);
		sum0 = _mm_hadd_epi32(sum0, sum0);
		uiSum = _mm_extract_epi32(sum0, 0);
	}
	else {
		for (; iHeight != 0; iHeight -= 2) {
			for (uiCol = 0; uiCol < iWidth; uiCol += 4)
			{
				T00 = _mm_loadl_epi64((__m128i*)(p_org + uiCol));
				T10 = _mm_loadl_epi64((__m128i*)(p_org + i_org + uiCol));
				T01 = _mm_loadl_epi64((__m128i*)(p_pred + uiCol));
				T11 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred + uiCol));

				T00 = _mm_sub_epi16(T00, T01);
				T00 = _mm_madd_epi16(T00, T00);

				sum0 = _mm_add_epi32(sum0, T00);
			}
			p_org += (i_org << 1);
			p_pred += (i_pred << 1);
		}
		sum0 = _mm_hadd_epi32(sum0, sum0);
		sum0 = _mm_hadd_epi32(sum0, sum0);
		uiSum = _mm_extract_epi32(sum0, 0);
	}

	return uiSum;
}

i64u_t xGetSSE_Psnr_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight)
{
    i64u_t uiSum = 0;
    int uiCol;

    __m128i sum0 = _mm_setzero_si128();
    __m128i zero = _mm_setzero_si128();
    __m128i T00, T01;
    __m128i T10, T11;

    if (!(iWidth & 7)) {
        for (; iHeight != 0; iHeight--) {
            for (uiCol = 0; uiCol < iWidth; uiCol += 8) {
                T00 = _mm_loadu_si128((__m128i*)(p_org + uiCol));
                T01 = _mm_loadu_si128((__m128i*)(p_pred + uiCol));

                T00 = _mm_sub_epi16(T00, T01);
                T00 = _mm_madd_epi16(T00, T00);
                T01 = _mm_unpacklo_epi32(T00, zero);
                T00 = _mm_unpackhi_epi32(T00, zero);
                sum0 = _mm_add_epi64(sum0, _mm_add_epi64(T00, T01));
            }
            p_org += i_org;
            p_pred += i_pred;
        }
        uiSum = _mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1);
    }
    else {
        for (; iHeight != 0; iHeight -= 2) {
            for (uiCol = 0; uiCol < iWidth; uiCol += 4) {
                T00 = _mm_loadl_epi64((__m128i*)(p_org + uiCol));
                T10 = _mm_loadl_epi64((__m128i*)(p_org + i_org + uiCol));
                T01 = _mm_loadl_epi64((__m128i*)(p_pred + uiCol));
                T11 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred + uiCol));

                T00 = _mm_sub_epi16(T00, T01);
                T00 = _mm_madd_epi16(T00, T00);

                T01 = _mm_unpacklo_epi32(T00, zero);
                T00 = _mm_unpackhi_epi32(T00, zero);
                sum0 = _mm_add_epi64(sum0, _mm_add_epi64(T00, T01));
            }
            p_org += (i_org << 1);
            p_pred += (i_pred << 1);
        }
        uiSum = _mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1);
    }

    return uiSum;
}

i32u_t xGetSSE16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;

    __m256i sum0 = _mm256_setzero_si256();
    __m256i T00, T01;

    while (height--) {
        T00 = _mm256_loadu_si256((__m256i*)(p_org));
        T01 = _mm256_loadu_si256((__m256i*)(p_pred));

        T00 = _mm256_sub_epi16(T00, T01);
        T00 = _mm256_madd_epi16(T00, T00);

        sum0 = _mm256_add_epi32(sum0, T00);

        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    return uiSum;
}

i32u_t xGetSSE32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;

    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;

    while (height--) {
        T00 = _mm256_loadu_si256((__m256i*)(p_org));
        T01 = _mm256_loadu_si256((__m256i*)(p_pred));

        T10 = _mm256_loadu_si256((__m256i*)(p_org + 16));
        T11 = _mm256_loadu_si256((__m256i*)(p_pred + 16));

        T00 = _mm256_sub_epi16(T00, T01);
        T00 = _mm256_madd_epi16(T00, T00);

        T10 = _mm256_sub_epi16(T10, T11);
        T10 = _mm256_madd_epi16(T10, T10);

        sum0 = _mm256_add_epi32(sum0, T00);
        sum1 = _mm256_add_epi32(sum1, T10);

        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm256_add_epi32(sum0, sum1);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    return uiSum;
}

i32u_t xGetSSE64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;

    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;
    __m256i T20, T21;
    __m256i T30, T31;

    while (height--) {
        T00 = _mm256_loadu_si256((__m256i*)(p_org));
        T01 = _mm256_loadu_si256((__m256i*)(p_pred));

        T10 = _mm256_loadu_si256((__m256i*)(p_org + 16));
        T11 = _mm256_loadu_si256((__m256i*)(p_pred + 16));

        T20 = _mm256_loadu_si256((__m256i*)(p_org + 32));
        T21 = _mm256_loadu_si256((__m256i*)(p_pred + 32));

        T30 = _mm256_loadu_si256((__m256i*)(p_org + 48));
        T31 = _mm256_loadu_si256((__m256i*)(p_pred + 48));

        T00 = _mm256_sub_epi16(T00, T01);
        T00 = _mm256_madd_epi16(T00, T00);

        T10 = _mm256_sub_epi16(T10, T11);
        T10 = _mm256_madd_epi16(T10, T10);

        T20 = _mm256_sub_epi16(T20, T21);
        T20 = _mm256_madd_epi16(T20, T20);

        T30 = _mm256_sub_epi16(T30, T31);
        T30 = _mm256_madd_epi16(T30, T30);

        sum0 = _mm256_add_epi32(sum0, T00);
        sum1 = _mm256_add_epi32(sum1, T10);
        sum2 = _mm256_add_epi32(sum2, T20);
        sum3 = _mm256_add_epi32(sum3, T30);

        p_org += i_org;
        p_pred += i_pred;
    }

    sum0 = _mm256_add_epi32(sum0, sum1);
    sum1 = _mm256_add_epi32(sum2, sum3);
    sum0 = _mm256_add_epi32(sum0, sum1);

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    return uiSum;
}

// --------------------------------------------------------------------------------------------------------------------
// SAD_x4
// --------------------------------------------------------------------------------------------------------------------
void xGetSAD4_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	int i_org_x2 = i_org << 1;
	int i_pred_x2 = i_pred << 1;
	__m128i T01, T03;
	__m128i T11, T13;
	__m128i T21, T23;
	__m128i T31, T33;
	__m128i T41, T43;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();
	height >>= (skip_lines + 1);

	for (; height != 0; height--)
	{
		T01 = _mm_loadl_epi64((__m128i*)p_org);
		T03 = _mm_loadl_epi64((__m128i*)(p_org + i_org));
		T01 = _mm_unpacklo_epi64(T01, T03);

		T11 = _mm_loadl_epi64((__m128i*)pred0);
		T13 = _mm_loadl_epi64((__m128i*)(pred0 + i_pred));
		T11 = _mm_unpacklo_epi64(T11, T13);

		T21 = _mm_loadl_epi64((__m128i*)pred1);
		T23 = _mm_loadl_epi64((__m128i*)(pred1 + i_pred));
		T21 = _mm_unpacklo_epi64(T21, T23);

		T31 = _mm_loadl_epi64((__m128i*)pred2);
		T33 = _mm_loadl_epi64((__m128i*)(pred2 + i_pred));
		T31 = _mm_unpacklo_epi64(T31, T33);

		T41 = _mm_loadl_epi64((__m128i*)pred3);
		T43 = _mm_loadl_epi64((__m128i*)(pred3 + i_pred));
		T41 = _mm_unpacklo_epi64(T41, T43);

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum1 = _mm_adds_epi16(sum1, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum2 = _mm_adds_epi16(sum2, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum3 = _mm_adds_epi16(sum3, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum4 = _mm_adds_epi16(sum4, T41);

		p_org += i_org_x2;
		pred0 += i_pred_x2;
		pred1 += i_pred_x2;
		pred2 += i_pred_x2;
		pred3 += i_pred_x2;
	}

	T11 = _mm_unpacklo_epi16(sum1, zero);
	T13 = _mm_unpackhi_epi16(sum1, zero);
	T11 = _mm_add_epi32(T11, T13);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[0] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;

	T21 = _mm_unpacklo_epi16(sum2, zero);
	T23 = _mm_unpackhi_epi16(sum2, zero);
	T21 = _mm_add_epi32(T21, T23);
	T21 = _mm_hadd_epi32(T21, T21);
	T21 = _mm_hadd_epi32(T21, T21);
	sad[1] = ((i32u_t)_mm_extract_epi32(T21, 0)) << skip_lines;

	T31 = _mm_unpacklo_epi16(sum3, zero);
	T33 = _mm_unpackhi_epi16(sum3, zero);
	T31 = _mm_add_epi32(T31, T33);
	T31 = _mm_hadd_epi32(T31, T31);
	T31 = _mm_hadd_epi32(T31, T31);
	sad[2] = ((i32u_t)_mm_extract_epi32(T31, 0)) << skip_lines;

	T41 = _mm_unpacklo_epi16(sum4, zero);
	T43 = _mm_unpackhi_epi16(sum4, zero);
	T41 = _mm_add_epi32(T41, T43);
	T41 = _mm_hadd_epi32(T41, T41);
	T41 = _mm_hadd_epi32(T41, T41);
	sad[3] = ((i32u_t)_mm_extract_epi32(T41, 0)) << skip_lines;
}

void xGetSAD8_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T01;
	__m128i T11, T13;
	__m128i T21, T23;
	__m128i T31, T33;
	__m128i T41, T43;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T01 = _mm_loadu_si128((__m128i*)p_org);
		T11 = _mm_loadu_si128((__m128i*)pred0);
		T21 = _mm_loadu_si128((__m128i*)pred1);
		T31 = _mm_loadu_si128((__m128i*)pred2);
		T41 = _mm_loadu_si128((__m128i*)pred3);

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum1 = _mm_adds_epi16(sum1, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum2 = _mm_adds_epi16(sum2, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum3 = _mm_adds_epi16(sum3, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum4 = _mm_adds_epi16(sum4, T41);

		p_org += i_org;
		pred0 += i_pred;
		pred1 += i_pred;
		pred2 += i_pred;
		pred3 += i_pred;
	}

	T11 = _mm_unpacklo_epi16(sum1, zero);
	T13 = _mm_unpackhi_epi16(sum1, zero);
	T11 = _mm_add_epi32(T11, T13);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[0] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;

	T21 = _mm_unpacklo_epi16(sum2, zero);
	T23 = _mm_unpackhi_epi16(sum2, zero);
	T21 = _mm_add_epi32(T21, T23);
	T21 = _mm_hadd_epi32(T21, T21);
	T21 = _mm_hadd_epi32(T21, T21);
	sad[1] = ((i32u_t)_mm_extract_epi32(T21, 0)) << skip_lines;

	T31 = _mm_unpacklo_epi16(sum3, zero);
	T33 = _mm_unpackhi_epi16(sum3, zero);
	T31 = _mm_add_epi32(T31, T33);
	T31 = _mm_hadd_epi32(T31, T31);
	T31 = _mm_hadd_epi32(T31, T31);
	sad[2] = ((i32u_t)_mm_extract_epi32(T31, 0)) << skip_lines;

	T41 = _mm_unpacklo_epi16(sum4, zero);
	T43 = _mm_unpackhi_epi16(sum4, zero);
	T41 = _mm_add_epi32(T41, T43);
	T41 = _mm_hadd_epi32(T41, T41);
	T41 = _mm_hadd_epi32(T41, T41);
	sad[3] = ((i32u_t)_mm_extract_epi32(T41, 0)) << skip_lines;
}

void xGetSAD16_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T01, T03;
	__m128i T11, T13;
	__m128i T21, T23;
	__m128i T31, T33;
	__m128i T41, T43;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();

	__m128i sum5 = _mm_setzero_si128();
	__m128i sum6 = _mm_setzero_si128();
	__m128i sum7 = _mm_setzero_si128();
	__m128i sum8 = _mm_setzero_si128();

	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T01 = _mm_load_si128((__m128i*)p_org);
		T11 = _mm_loadu_si128((__m128i*)pred0);
		T21 = _mm_loadu_si128((__m128i*)pred1);
		T31 = _mm_loadu_si128((__m128i*)pred2);
		T41 = _mm_loadu_si128((__m128i*)pred3);

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum1 = _mm_adds_epi16(sum1, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum2 = _mm_adds_epi16(sum2, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum3 = _mm_adds_epi16(sum3, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum4 = _mm_adds_epi16(sum4, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 8));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 8));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 8));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 8));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 8));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum5 = _mm_adds_epi16(sum5, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum6 = _mm_adds_epi16(sum6, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum7 = _mm_adds_epi16(sum7, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum8 = _mm_adds_epi16(sum8, T41);

		p_org += i_org;
		pred0 += i_pred;
		pred1 += i_pred;
		pred2 += i_pred;
		pred3 += i_pred;
	}

	T11 = _mm_unpacklo_epi16(sum1, zero);
	T13 = _mm_unpackhi_epi16(sum1, zero);
	T01 = _mm_unpacklo_epi16(sum5, zero);
	T03 = _mm_unpackhi_epi16(sum5, zero);
	T11 = _mm_add_epi32(T11, T13);
	T01 = _mm_add_epi32(T01, T03);
	T11 = _mm_add_epi32(T11, T01);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[0] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;

	T21 = _mm_unpacklo_epi16(sum2, zero);
	T23 = _mm_unpackhi_epi16(sum2, zero);
	T01 = _mm_unpacklo_epi16(sum6, zero);
	T03 = _mm_unpackhi_epi16(sum6, zero);
	T01 = _mm_add_epi32(T01, T03);
	T21 = _mm_add_epi32(T21, T23);
	T21 = _mm_add_epi32(T21, T01);
	T21 = _mm_hadd_epi32(T21, T21);
	T21 = _mm_hadd_epi32(T21, T21);
	sad[1] = ((i32u_t)_mm_extract_epi32(T21, 0)) << skip_lines;

	T31 = _mm_unpacklo_epi16(sum3, zero);
	T33 = _mm_unpackhi_epi16(sum3, zero);
	T01 = _mm_unpacklo_epi16(sum7, zero);
	T03 = _mm_unpackhi_epi16(sum7, zero);
	T01 = _mm_add_epi32(T01, T03);
	T31 = _mm_add_epi32(T31, T33);
	T31 = _mm_add_epi32(T31, T01);
	T31 = _mm_hadd_epi32(T31, T31);
	T31 = _mm_hadd_epi32(T31, T31);
	sad[2] = ((i32u_t)_mm_extract_epi32(T31, 0)) << skip_lines;

	T41 = _mm_unpacklo_epi16(sum4, zero);
	T43 = _mm_unpackhi_epi16(sum4, zero);
	T01 = _mm_unpacklo_epi16(sum8, zero);
	T03 = _mm_unpackhi_epi16(sum8, zero);
	T01 = _mm_add_epi32(T01, T03);
	T41 = _mm_add_epi32(T41, T43);
	T41 = _mm_add_epi32(T41, T01);
	T41 = _mm_hadd_epi32(T41, T41);
	T41 = _mm_hadd_epi32(T41, T41);
	sad[3] = ((i32u_t)_mm_extract_epi32(T41, 0)) << skip_lines;
}

void xGetSAD32_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T01, T03;
	__m128i T11, T13;
	__m128i T21, T23;
	__m128i T31, T33;
	__m128i T41, T43;
	__m128i T51, T53;
	__m128i T61, T63;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();

	__m128i sum5 = _mm_setzero_si128();
	__m128i sum6 = _mm_setzero_si128();
	__m128i sum7 = _mm_setzero_si128();
	__m128i sum8 = _mm_setzero_si128();

	__m128i sum9 = _mm_setzero_si128();
	__m128i sum10 = _mm_setzero_si128();
	__m128i sum11 = _mm_setzero_si128();
	__m128i sum12 = _mm_setzero_si128();

	__m128i sum13 = _mm_setzero_si128();
	__m128i sum14 = _mm_setzero_si128();
	__m128i sum15 = _mm_setzero_si128();
	__m128i sum16 = _mm_setzero_si128();

	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T01 = _mm_load_si128((__m128i*)p_org);
		T11 = _mm_loadu_si128((__m128i*)pred0);
		T21 = _mm_loadu_si128((__m128i*)pred1);
		T31 = _mm_loadu_si128((__m128i*)pred2);
		T41 = _mm_loadu_si128((__m128i*)pred3);

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum1 = _mm_adds_epi16(sum1, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum2 = _mm_adds_epi16(sum2, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum3 = _mm_adds_epi16(sum3, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum4 = _mm_adds_epi16(sum4, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 8));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 8));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 8));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 8));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 8));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum5 = _mm_adds_epi16(sum5, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum6 = _mm_adds_epi16(sum6, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum7 = _mm_adds_epi16(sum7, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum8 = _mm_adds_epi16(sum8, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 16));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 16));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 16));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 16));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 16));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum9 = _mm_adds_epi16(sum9, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum10 = _mm_adds_epi16(sum10, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum11 = _mm_adds_epi16(sum11, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum12 = _mm_adds_epi16(sum12, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 24));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 24));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 24));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 24));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 24));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum13 = _mm_adds_epi16(sum13, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum14 = _mm_adds_epi16(sum14, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum15 = _mm_adds_epi16(sum15, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum16 = _mm_adds_epi16(sum16, T41);

		p_org += i_org;
		pred0 += i_pred;
		pred1 += i_pred;
		pred2 += i_pred;
		pred3 += i_pred;
	}

	T11 = _mm_unpacklo_epi16(sum1, zero);
	T13 = _mm_unpackhi_epi16(sum1, zero);
	T01 = _mm_unpacklo_epi16(sum5, zero);
	T03 = _mm_unpackhi_epi16(sum5, zero);
	T51 = _mm_unpacklo_epi16(sum9, zero);
	T53 = _mm_unpackhi_epi16(sum9, zero);
	T61 = _mm_unpacklo_epi16(sum13, zero);
	T63 = _mm_unpackhi_epi16(sum13, zero);
	T11 = _mm_add_epi32(T11, T13);
	T01 = _mm_add_epi32(T01, T03);
	T51 = _mm_add_epi32(T51, T53);
	T61 = _mm_add_epi32(T61, T63);
	T61 = _mm_add_epi32(T61, T51);
	T11 = _mm_add_epi32(T11, T01);
	T11 = _mm_add_epi32(T11, T61);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[0] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;

	T21 = _mm_unpacklo_epi16(sum2, zero);
	T23 = _mm_unpackhi_epi16(sum2, zero);
	T01 = _mm_unpacklo_epi16(sum6, zero);
	T03 = _mm_unpackhi_epi16(sum6, zero);
	T51 = _mm_unpacklo_epi16(sum10, zero);
	T53 = _mm_unpackhi_epi16(sum10, zero);
	T61 = _mm_unpacklo_epi16(sum14, zero);
	T63 = _mm_unpackhi_epi16(sum14, zero);
	T51 = _mm_add_epi32(T51, T53);
	T61 = _mm_add_epi32(T61, T63);
	T61 = _mm_add_epi32(T61, T51);
	T01 = _mm_add_epi32(T01, T03);
	T21 = _mm_add_epi32(T21, T23);
	T21 = _mm_add_epi32(T21, T01);
	T21 = _mm_add_epi32(T21, T61);
	T21 = _mm_hadd_epi32(T21, T21);
	T21 = _mm_hadd_epi32(T21, T21);
	sad[1] = ((i32u_t)_mm_extract_epi32(T21, 0)) << skip_lines;

	T31 = _mm_unpacklo_epi16(sum3, zero);
	T33 = _mm_unpackhi_epi16(sum3, zero);
	T01 = _mm_unpacklo_epi16(sum7, zero);
	T03 = _mm_unpackhi_epi16(sum7, zero);
	T51 = _mm_unpacklo_epi16(sum11, zero);
	T53 = _mm_unpackhi_epi16(sum11, zero);
	T61 = _mm_unpacklo_epi16(sum15, zero);
	T63 = _mm_unpackhi_epi16(sum15, zero);
	T51 = _mm_add_epi32(T51, T53);
	T61 = _mm_add_epi32(T61, T63);
	T61 = _mm_add_epi32(T61, T51);
	T01 = _mm_add_epi32(T01, T03);
	T31 = _mm_add_epi32(T31, T33);
	T31 = _mm_add_epi32(T31, T01);
	T31 = _mm_add_epi32(T31, T61);
	T31 = _mm_hadd_epi32(T31, T31);
	T31 = _mm_hadd_epi32(T31, T31);
	sad[2] = ((i32u_t)_mm_extract_epi32(T31, 0)) << skip_lines;

	T41 = _mm_unpacklo_epi16(sum4, zero);
	T43 = _mm_unpackhi_epi16(sum4, zero);
	T01 = _mm_unpacklo_epi16(sum8, zero);
	T03 = _mm_unpackhi_epi16(sum8, zero);
	T51 = _mm_unpacklo_epi16(sum12, zero);
	T53 = _mm_unpackhi_epi16(sum12, zero);
	T61 = _mm_unpacklo_epi16(sum16, zero);
	T63 = _mm_unpackhi_epi16(sum16, zero);
	T51 = _mm_add_epi32(T51, T53);
	T61 = _mm_add_epi32(T61, T63);
	T61 = _mm_add_epi32(T61, T51);
	T01 = _mm_add_epi32(T01, T03);
	T41 = _mm_add_epi32(T41, T43);
	T41 = _mm_add_epi32(T41, T01);
	T41 = _mm_add_epi32(T41, T61);
	T41 = _mm_hadd_epi32(T41, T41);
	T41 = _mm_hadd_epi32(T41, T41);
	sad[3] = ((i32u_t)_mm_extract_epi32(T41, 0)) << skip_lines;
}

void xGetSAD64_x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred = i_pred << skip_lines;
	__m128i T01;
	__m128i T11, T13;
	__m128i T21;
	__m128i T31;
	__m128i T41;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();

	__m128i sum5 = _mm_setzero_si128();
	__m128i sum6 = _mm_setzero_si128();
	__m128i sum7 = _mm_setzero_si128();
	__m128i sum8 = _mm_setzero_si128();

	__m128i sum9 = _mm_setzero_si128();
	__m128i sum10 = _mm_setzero_si128();
	__m128i sum11 = _mm_setzero_si128();
	__m128i sum12 = _mm_setzero_si128();

	__m128i sum13 = _mm_setzero_si128();
	__m128i sum14 = _mm_setzero_si128();
	__m128i sum15 = _mm_setzero_si128();
	__m128i sum16 = _mm_setzero_si128();

	__m128i sum01 = _mm_setzero_si128();
	__m128i sum02 = _mm_setzero_si128();
	__m128i sum03 = _mm_setzero_si128();
	__m128i sum04 = _mm_setzero_si128();

	__m128i sum05 = _mm_setzero_si128();
	__m128i sum06 = _mm_setzero_si128();
	__m128i sum07 = _mm_setzero_si128();
	__m128i sum08 = _mm_setzero_si128();

	__m128i sum09 = _mm_setzero_si128();
	__m128i sum010 = _mm_setzero_si128();
	__m128i sum011 = _mm_setzero_si128();
	__m128i sum012 = _mm_setzero_si128();

	__m128i sum013 = _mm_setzero_si128();
	__m128i sum014 = _mm_setzero_si128();
	__m128i sum015 = _mm_setzero_si128();
	__m128i sum016 = _mm_setzero_si128();

	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T01 = _mm_load_si128((__m128i*)p_org);
		T11 = _mm_loadu_si128((__m128i*)pred0);
		T21 = _mm_loadu_si128((__m128i*)pred1);
		T31 = _mm_loadu_si128((__m128i*)pred2);
		T41 = _mm_loadu_si128((__m128i*)pred3);

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum1 = _mm_adds_epi16(sum1, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum2 = _mm_adds_epi16(sum2, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum3 = _mm_adds_epi16(sum3, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum4 = _mm_adds_epi16(sum4, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 8));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 8));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 8));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 8));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 8));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum5 = _mm_adds_epi16(sum5, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum6 = _mm_adds_epi16(sum6, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum7 = _mm_adds_epi16(sum7, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum8 = _mm_adds_epi16(sum8, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 16));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 16));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 16));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 16));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 16));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum9 = _mm_adds_epi16(sum9, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum10 = _mm_adds_epi16(sum10, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum11 = _mm_adds_epi16(sum11, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum12 = _mm_adds_epi16(sum12, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 24));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 24));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 24));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 24));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 24));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum13 = _mm_adds_epi16(sum13, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum14 = _mm_adds_epi16(sum14, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum15 = _mm_adds_epi16(sum15, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum16 = _mm_adds_epi16(sum16, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 32));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 32));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 32));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 32));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 32));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum01 = _mm_adds_epi16(sum01, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum02 = _mm_adds_epi16(sum02, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum03 = _mm_adds_epi16(sum03, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum04 = _mm_adds_epi16(sum04, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 40));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 40));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 40));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 40));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 40));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum05 = _mm_adds_epi16(sum05, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum06 = _mm_adds_epi16(sum06, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum07 = _mm_adds_epi16(sum07, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum08 = _mm_adds_epi16(sum08, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 48));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 48));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 48));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 48));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 48));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum09 = _mm_adds_epi16(sum09, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum010 = _mm_adds_epi16(sum010, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum011 = _mm_adds_epi16(sum011, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum012 = _mm_adds_epi16(sum012, T41);

		T01 = _mm_load_si128((__m128i*)(p_org + 56));
		T11 = _mm_loadu_si128((__m128i*)(pred0 + 56));
		T21 = _mm_loadu_si128((__m128i*)(pred1 + 56));
		T31 = _mm_loadu_si128((__m128i*)(pred2 + 56));
		T41 = _mm_loadu_si128((__m128i*)(pred3 + 56));

		T11 = _mm_sub_epi16(T11, T01);
		T11 = _mm_abs_epi16(T11);
		sum013 = _mm_adds_epi16(sum013, T11);

		T21 = _mm_sub_epi16(T21, T01);
		T21 = _mm_abs_epi16(T21);
		sum014 = _mm_adds_epi16(sum014, T21);

		T31 = _mm_sub_epi16(T31, T01);
		T31 = _mm_abs_epi16(T31);
		sum015 = _mm_adds_epi16(sum015, T31);

		T41 = _mm_sub_epi16(T41, T01);
		T41 = _mm_abs_epi16(T41);
		sum016 = _mm_adds_epi16(sum016, T41);

		p_org += i_org;
		pred0 += i_pred;
		pred1 += i_pred;
		pred2 += i_pred;
		pred3 += i_pred;
	}

	T11 = _mm_unpacklo_epi16(sum1, zero);
	T13 = _mm_unpackhi_epi16(sum1, zero);
	sum1 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum5, zero);
	T13 = _mm_unpackhi_epi16(sum5, zero);
	sum5 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum9, zero);
	T13 = _mm_unpackhi_epi16(sum9, zero);
	sum9 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum13, zero);
	T13 = _mm_unpackhi_epi16(sum13, zero);
	sum13 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum01, zero);
	T13 = _mm_unpackhi_epi16(sum01, zero);
	sum01 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum05, zero);
	T13 = _mm_unpackhi_epi16(sum05, zero);
	sum05 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum09, zero);
	T13 = _mm_unpackhi_epi16(sum09, zero);
	sum09 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum013, zero);
	T13 = _mm_unpackhi_epi16(sum013, zero);
	sum013 = _mm_add_epi32(T11, T13);
	sum1 = _mm_add_epi32(sum1, sum01);
	sum5 = _mm_add_epi32(sum5, sum05);
	sum9 = _mm_add_epi32(sum9, sum09);
	sum13 = _mm_add_epi32(sum13, sum013);
	sum1 = _mm_add_epi32(sum1, sum5);
	sum9 = _mm_add_epi32(sum9, sum13);
	T11 = _mm_add_epi32(sum9, sum1);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[0] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;

	T11 = _mm_unpacklo_epi16(sum2, zero);
	T13 = _mm_unpackhi_epi16(sum2, zero);
	sum2 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum6, zero);
	T13 = _mm_unpackhi_epi16(sum6, zero);
	sum6 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum10, zero);
	T13 = _mm_unpackhi_epi16(sum10, zero);
	sum10 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum14, zero);
	T13 = _mm_unpackhi_epi16(sum14, zero);
	sum14 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum02, zero);
	T13 = _mm_unpackhi_epi16(sum02, zero);
	sum02 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum06, zero);
	T13 = _mm_unpackhi_epi16(sum06, zero);
	sum06 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum010, zero);
	T13 = _mm_unpackhi_epi16(sum010, zero);
	sum010 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum014, zero);
	T13 = _mm_unpackhi_epi16(sum014, zero);
	sum014 = _mm_add_epi32(T11, T13);
	sum2 = _mm_add_epi32(sum2, sum02);
	sum6 = _mm_add_epi32(sum6, sum06);
	sum10 = _mm_add_epi32(sum10, sum010);
	sum14 = _mm_add_epi32(sum14, sum014);
	sum2 = _mm_add_epi32(sum2, sum6);
	sum10 = _mm_add_epi32(sum10, sum14);
	T11 = _mm_add_epi32(sum10, sum2);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[1] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;

	T11 = _mm_unpacklo_epi16(sum3, zero);
	T13 = _mm_unpackhi_epi16(sum3, zero);
	sum3 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum7, zero);
	T13 = _mm_unpackhi_epi16(sum7, zero);
	sum7 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum11, zero);
	T13 = _mm_unpackhi_epi16(sum11, zero);
	sum11 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum15, zero);
	T13 = _mm_unpackhi_epi16(sum15, zero);
	sum15 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum03, zero);
	T13 = _mm_unpackhi_epi16(sum03, zero);
	sum03 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum07, zero);
	T13 = _mm_unpackhi_epi16(sum07, zero);
	sum07 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum011, zero);
	T13 = _mm_unpackhi_epi16(sum011, zero);
	sum011 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum015, zero);
	T13 = _mm_unpackhi_epi16(sum015, zero);
	sum015 = _mm_add_epi32(T11, T13);
	sum3 = _mm_add_epi32(sum3, sum03);
	sum7 = _mm_add_epi32(sum7, sum07);
	sum11 = _mm_add_epi32(sum11, sum011);
	sum15 = _mm_add_epi32(sum15, sum015);
	sum3 = _mm_add_epi32(sum3, sum7);
	sum11 = _mm_add_epi32(sum11, sum15);
	T11 = _mm_add_epi32(sum11, sum3);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[2] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;

	T11 = _mm_unpacklo_epi16(sum4, zero);
	T13 = _mm_unpackhi_epi16(sum4, zero);
	sum4 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum8, zero);
	T13 = _mm_unpackhi_epi16(sum8, zero);
	sum8 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum12, zero);
	T13 = _mm_unpackhi_epi16(sum12, zero);
	sum12 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum16, zero);
	T13 = _mm_unpackhi_epi16(sum16, zero);
	sum16 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum04, zero);
	T13 = _mm_unpackhi_epi16(sum04, zero);
	sum04 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum08, zero);
	T13 = _mm_unpackhi_epi16(sum08, zero);
	sum08 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum012, zero);
	T13 = _mm_unpackhi_epi16(sum012, zero);
	sum012 = _mm_add_epi32(T11, T13);
	T11 = _mm_unpacklo_epi16(sum016, zero);
	T13 = _mm_unpackhi_epi16(sum016, zero);
	sum016 = _mm_add_epi32(T11, T13);
	sum4 = _mm_add_epi32(sum4, sum04);
	sum8 = _mm_add_epi32(sum8, sum08);
	sum12 = _mm_add_epi32(sum12, sum012);
	sum16 = _mm_add_epi32(sum16, sum016);
	sum4 = _mm_add_epi32(sum4, sum8);
	sum12 = _mm_add_epi32(sum12, sum16);
	T11 = _mm_add_epi32(sum12, sum4);
	T11 = _mm_hadd_epi32(T11, T11);
	T11 = _mm_hadd_epi32(T11, T11);
	sad[3] = ((i32u_t)_mm_extract_epi32(T11, 0)) << skip_lines;
}

void xGetSAD16_x4_sse256_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i ZERO = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;
    __m256i T20, T21;
    __m256i T30, T31;
    __m256i T40;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_loadu_si256((__m256i*)(p_org));
        T10 = _mm256_loadu_si256((__m256i*)(pred0));
        T20 = _mm256_loadu_si256((__m256i*)(pred1));
        T30 = _mm256_loadu_si256((__m256i*)(pred2));
        T40 = _mm256_loadu_si256((__m256i*)(pred3));

        T10 = _mm256_sub_epi16(T00, T10);
        T10 = _mm256_abs_epi16(T10);
        sum0 = _mm256_add_epi16(sum0, T10);

        T20 = _mm256_sub_epi16(T00, T20);
        T20 = _mm256_abs_epi16(T20);
        sum1 = _mm256_add_epi16(sum1, T20);

        T30 = _mm256_sub_epi16(T00, T30);
        T30 = _mm256_abs_epi16(T30);
        sum2 = _mm256_add_epi16(sum2, T30);

        T40 = _mm256_sub_epi16(T00, T40);
        T40 = _mm256_abs_epi16(T40);
        sum3 = _mm256_add_epi16(sum3, T40);

        p_org += i_org;
        pred0 += i_pred;
        pred1 += i_pred;
        pred2 += i_pred;
        pred3 += i_pred;
    }

    T00 = _mm256_unpacklo_epi16(sum0, ZERO);
    T01 = _mm256_unpackhi_epi16(sum0, ZERO);

    T10 = _mm256_unpacklo_epi16(sum1, ZERO);
    T11 = _mm256_unpackhi_epi16(sum1, ZERO);

    T20 = _mm256_unpacklo_epi16(sum2, ZERO);
    T21 = _mm256_unpackhi_epi16(sum2, ZERO);

    T30 = _mm256_unpacklo_epi16(sum3, ZERO);
    T31 = _mm256_unpackhi_epi16(sum3, ZERO);

    sum0 = _mm256_add_epi32(T00, T01);
    sum1 = _mm256_add_epi32(T10, T11);
    sum2 = _mm256_add_epi32(T20, T21);
    sum3 = _mm256_add_epi32(T30, T31);

    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum1 = _mm256_hadd_epi32(sum2, sum3);
    sum0 = _mm256_hadd_epi32(sum0, sum1);

    sad[0] = (((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4))) << skip_lines;
    sad[1] = (((i32u_t)_mm256_extract_epi32(sum0, 1)) + ((i32u_t)_mm256_extract_epi32(sum0, 5))) << skip_lines;
    sad[2] = (((i32u_t)_mm256_extract_epi32(sum0, 2)) + ((i32u_t)_mm256_extract_epi32(sum0, 6))) << skip_lines;
    sad[3] = (((i32u_t)_mm256_extract_epi32(sum0, 3)) + ((i32u_t)_mm256_extract_epi32(sum0, 7))) << skip_lines;
}

void xGetSAD32_x4_sse256_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i ZERO = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;
    __m256i T20, T21;
    __m256i T30, T31;
    __m256i T40;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T10 = _mm256_loadu_si256((__m256i*)(pred0));
        T20 = _mm256_loadu_si256((__m256i*)(pred1));
        T30 = _mm256_loadu_si256((__m256i*)(pred2));
        T40 = _mm256_loadu_si256((__m256i*)(pred3));

        T10 = _mm256_sub_epi16(T00, T10);
        T10 = _mm256_abs_epi16(T10);
        sum0 = _mm256_add_epi16(sum0, T10);

        T20 = _mm256_sub_epi16(T00, T20);
        T20 = _mm256_abs_epi16(T20);
        sum1 = _mm256_add_epi16(sum1, T20);

        T30 = _mm256_sub_epi16(T00, T30);
        T30 = _mm256_abs_epi16(T30);
        sum2 = _mm256_add_epi16(sum2, T30);

        T40 = _mm256_sub_epi16(T00, T40);
        T40 = _mm256_abs_epi16(T40);
        sum3 = _mm256_add_epi16(sum3, T40);

        T00 = _mm256_load_si256((__m256i*)(p_org + 16));
        T10 = _mm256_loadu_si256((__m256i*)(pred0 + 16));
        T20 = _mm256_loadu_si256((__m256i*)(pred1 + 16));
        T30 = _mm256_loadu_si256((__m256i*)(pred2 + 16));
        T40 = _mm256_loadu_si256((__m256i*)(pred3 + 16));

        T10 = _mm256_sub_epi16(T00, T10);
        T10 = _mm256_abs_epi16(T10);
        sum0 = _mm256_add_epi16(sum0, T10);

        T20 = _mm256_sub_epi16(T00, T20);
        T20 = _mm256_abs_epi16(T20);
        sum1 = _mm256_add_epi16(sum1, T20);

        T30 = _mm256_sub_epi16(T00, T30);
        T30 = _mm256_abs_epi16(T30);
        sum2 = _mm256_add_epi16(sum2, T30);

        T40 = _mm256_sub_epi16(T00, T40);
        T40 = _mm256_abs_epi16(T40);
        sum3 = _mm256_add_epi16(sum3, T40);

        p_org += i_org;
        pred0 += i_pred;
        pred1 += i_pred;
        pred2 += i_pred;
        pred3 += i_pred;
    }

    T00 = _mm256_unpacklo_epi16(sum0, ZERO);
    T01 = _mm256_unpackhi_epi16(sum0, ZERO);

    T10 = _mm256_unpacklo_epi16(sum1, ZERO);
    T11 = _mm256_unpackhi_epi16(sum1, ZERO);

    T20 = _mm256_unpacklo_epi16(sum2, ZERO);
    T21 = _mm256_unpackhi_epi16(sum2, ZERO);

    T30 = _mm256_unpacklo_epi16(sum3, ZERO);
    T31 = _mm256_unpackhi_epi16(sum3, ZERO);

    sum0 = _mm256_add_epi32(T00, T01);
    sum1 = _mm256_add_epi32(T10, T11);
    sum2 = _mm256_add_epi32(T20, T21);
    sum3 = _mm256_add_epi32(T30, T31);

    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum1 = _mm256_hadd_epi32(sum2, sum3);
    sum0 = _mm256_hadd_epi32(sum0, sum1);

    sad[0] = (((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4))) << skip_lines;
    sad[1] = (((i32u_t)_mm256_extract_epi32(sum0, 1)) + ((i32u_t)_mm256_extract_epi32(sum0, 5))) << skip_lines;
    sad[2] = (((i32u_t)_mm256_extract_epi32(sum0, 2)) + ((i32u_t)_mm256_extract_epi32(sum0, 6))) << skip_lines;
    sad[3] = (((i32u_t)_mm256_extract_epi32(sum0, 3)) + ((i32u_t)_mm256_extract_epi32(sum0, 7))) << skip_lines;
}

void xGetSAD64_x4_sse256_10bit(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1, pel_t *pred2, pel_t *pred3, int i_pred, i32u_t sad[4], int height, int skip_lines)
{
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i sum4 = _mm256_setzero_si256();
    __m256i sum5 = _mm256_setzero_si256();
    __m256i sum6 = _mm256_setzero_si256();
    __m256i sum7 = _mm256_setzero_si256();
    __m256i sum8 = _mm256_setzero_si256();
    __m256i sum9 = _mm256_setzero_si256();
    __m256i sum10 = _mm256_setzero_si256();
    __m256i sum11 = _mm256_setzero_si256();
    __m256i sum12 = _mm256_setzero_si256();
    __m256i sum13 = _mm256_setzero_si256();
    __m256i sum14 = _mm256_setzero_si256();
    __m256i sum15 = _mm256_setzero_si256();
    __m256i ZERO = _mm256_setzero_si256();
    __m256i T00, T01;
    __m256i T10, T11;
    __m256i T20, T21;
    __m256i T30, T31;
    __m256i T40;

    i_org <<= skip_lines;
    i_pred <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T10 = _mm256_loadu_si256((__m256i*)(pred0));
        T20 = _mm256_loadu_si256((__m256i*)(pred1));
        T30 = _mm256_loadu_si256((__m256i*)(pred2));
        T40 = _mm256_loadu_si256((__m256i*)(pred3));

        T10 = _mm256_sub_epi16(T00, T10);
        T10 = _mm256_abs_epi16(T10);
        sum0 = _mm256_add_epi16(sum0, T10);

        T20 = _mm256_sub_epi16(T00, T20);
        T20 = _mm256_abs_epi16(T20);
        sum1 = _mm256_add_epi16(sum1, T20);

        T30 = _mm256_sub_epi16(T00, T30);
        T30 = _mm256_abs_epi16(T30);
        sum2 = _mm256_add_epi16(sum2, T30);

        T40 = _mm256_sub_epi16(T00, T40);
        T40 = _mm256_abs_epi16(T40);
        sum3 = _mm256_add_epi16(sum3, T40);

        T00 = _mm256_load_si256((__m256i*)(p_org + 16));
        T10 = _mm256_loadu_si256((__m256i*)(pred0 + 16));
        T20 = _mm256_loadu_si256((__m256i*)(pred1 + 16));
        T30 = _mm256_loadu_si256((__m256i*)(pred2 + 16));
        T40 = _mm256_loadu_si256((__m256i*)(pred3 + 16));

        T10 = _mm256_sub_epi16(T00, T10);
        T10 = _mm256_abs_epi16(T10);
        sum4 = _mm256_add_epi16(sum4, T10);

        T20 = _mm256_sub_epi16(T00, T20);
        T20 = _mm256_abs_epi16(T20);
        sum5 = _mm256_add_epi16(sum5, T20);

        T30 = _mm256_sub_epi16(T00, T30);
        T30 = _mm256_abs_epi16(T30);
        sum6 = _mm256_add_epi16(sum6, T30);

        T40 = _mm256_sub_epi16(T00, T40);
        T40 = _mm256_abs_epi16(T40);
        sum7 = _mm256_add_epi16(sum7, T40);

        T00 = _mm256_load_si256((__m256i*)(p_org + 32));
        T10 = _mm256_loadu_si256((__m256i*)(pred0 + 32));
        T20 = _mm256_loadu_si256((__m256i*)(pred1 + 32));
        T30 = _mm256_loadu_si256((__m256i*)(pred2 + 32));
        T40 = _mm256_loadu_si256((__m256i*)(pred3 + 32));

        T10 = _mm256_sub_epi16(T00, T10);
        T10 = _mm256_abs_epi16(T10);
        sum8 = _mm256_add_epi16(sum8, T10);

        T20 = _mm256_sub_epi16(T00, T20);
        T20 = _mm256_abs_epi16(T20);
        sum9 = _mm256_add_epi16(sum9, T20);

        T30 = _mm256_sub_epi16(T00, T30);
        T30 = _mm256_abs_epi16(T30);
        sum10 = _mm256_add_epi16(sum10, T30);

        T40 = _mm256_sub_epi16(T00, T40);
        T40 = _mm256_abs_epi16(T40);
        sum11 = _mm256_add_epi16(sum11, T40);

        T00 = _mm256_load_si256((__m256i*)(p_org + 48));
        T10 = _mm256_loadu_si256((__m256i*)(pred0 + 48));
        T20 = _mm256_loadu_si256((__m256i*)(pred1 + 48));
        T30 = _mm256_loadu_si256((__m256i*)(pred2 + 48));
        T40 = _mm256_loadu_si256((__m256i*)(pred3 + 48));

        T10 = _mm256_sub_epi16(T00, T10);
        T10 = _mm256_abs_epi16(T10);
        sum12 = _mm256_add_epi16(sum12, T10);

        T20 = _mm256_sub_epi16(T00, T20);
        T20 = _mm256_abs_epi16(T20);
        sum13 = _mm256_add_epi16(sum13, T20);

        T30 = _mm256_sub_epi16(T00, T30);
        T30 = _mm256_abs_epi16(T30);
        sum14 = _mm256_add_epi16(sum14, T30);

        T40 = _mm256_sub_epi16(T00, T40);
        T40 = _mm256_abs_epi16(T40);
        sum15 = _mm256_add_epi16(sum15, T40);

        p_org += i_org;
        pred0 += i_pred;
        pred1 += i_pred;
        pred2 += i_pred;
        pred3 += i_pred;
    }
    //sad0
    T00 = _mm256_unpacklo_epi16(sum0, ZERO);
    T01 = _mm256_unpackhi_epi16(sum0, ZERO);

    T10 = _mm256_unpacklo_epi16(sum4, ZERO);
    T11 = _mm256_unpackhi_epi16(sum4, ZERO);

    T20 = _mm256_unpacklo_epi16(sum8, ZERO);
    T21 = _mm256_unpackhi_epi16(sum8, ZERO);

    T30 = _mm256_unpacklo_epi16(sum12, ZERO);
    T31 = _mm256_unpackhi_epi16(sum12, ZERO);

    sum0 = _mm256_add_epi32(T00, T01);
    sum4 = _mm256_add_epi32(T10, T11);
    sum8 = _mm256_add_epi32(T20, T21);
    sum12 = _mm256_add_epi32(T30, T31);

    sum0 = _mm256_add_epi32(sum0, sum4);
    sum4 = _mm256_add_epi32(sum8, sum12);
    sum0 = _mm256_add_epi32(sum0, sum4);

    //sad1
    T00 = _mm256_unpacklo_epi16(sum1, ZERO);
    T01 = _mm256_unpackhi_epi16(sum1, ZERO);

    T10 = _mm256_unpacklo_epi16(sum5, ZERO);
    T11 = _mm256_unpackhi_epi16(sum5, ZERO);

    T20 = _mm256_unpacklo_epi16(sum9, ZERO);
    T21 = _mm256_unpackhi_epi16(sum9, ZERO);

    T30 = _mm256_unpacklo_epi16(sum13, ZERO);
    T31 = _mm256_unpackhi_epi16(sum13, ZERO);

    sum1 = _mm256_add_epi32(T00, T01);
    sum5 = _mm256_add_epi32(T10, T11);
    sum9 = _mm256_add_epi32(T20, T21);
    sum13 = _mm256_add_epi32(T30, T31);

    sum1 = _mm256_add_epi32(sum1, sum5);
    sum5 = _mm256_add_epi32(sum9, sum13);
    sum1 = _mm256_add_epi32(sum1, sum5);

    //sad2
    T00 = _mm256_unpacklo_epi16(sum2, ZERO);
    T01 = _mm256_unpackhi_epi16(sum2, ZERO);

    T10 = _mm256_unpacklo_epi16(sum6, ZERO);
    T11 = _mm256_unpackhi_epi16(sum6, ZERO);

    T20 = _mm256_unpacklo_epi16(sum10, ZERO);
    T21 = _mm256_unpackhi_epi16(sum10, ZERO);

    T30 = _mm256_unpacklo_epi16(sum14, ZERO);
    T31 = _mm256_unpackhi_epi16(sum14, ZERO);

    sum2 = _mm256_add_epi32(T00, T01);
    sum6 = _mm256_add_epi32(T10, T11);
    sum10 = _mm256_add_epi32(T20, T21);
    sum14 = _mm256_add_epi32(T30, T31);

    sum2 = _mm256_add_epi32(sum2, sum6);
    sum6 = _mm256_add_epi32(sum10, sum14);
    sum2 = _mm256_add_epi32(sum2, sum6);

    //sad3
    T00 = _mm256_unpacklo_epi16(sum3, ZERO);
    T01 = _mm256_unpackhi_epi16(sum3, ZERO);

    T10 = _mm256_unpacklo_epi16(sum7, ZERO);
    T11 = _mm256_unpackhi_epi16(sum7, ZERO);

    T20 = _mm256_unpacklo_epi16(sum11, ZERO);
    T21 = _mm256_unpackhi_epi16(sum11, ZERO);

    T30 = _mm256_unpacklo_epi16(sum15, ZERO);
    T31 = _mm256_unpackhi_epi16(sum15, ZERO);

    sum3 = _mm256_add_epi32(T00, T01);
    sum7 = _mm256_add_epi32(T10, T11);
    sum11 = _mm256_add_epi32(T20, T21);
    sum15 = _mm256_add_epi32(T30, T31);

    sum3 = _mm256_add_epi32(sum3, sum7);
    sum7 = _mm256_add_epi32(sum11, sum15);
    sum3 = _mm256_add_epi32(sum3, sum7);

    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum1 = _mm256_hadd_epi32(sum2, sum3);
    sum0 = _mm256_hadd_epi32(sum0, sum1);

    sad[0] = (((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4))) << skip_lines;
    sad[1] = (((i32u_t)_mm256_extract_epi32(sum0, 1)) + ((i32u_t)_mm256_extract_epi32(sum0, 5))) << skip_lines;
    sad[2] = (((i32u_t)_mm256_extract_epi32(sum0, 2)) + ((i32u_t)_mm256_extract_epi32(sum0, 6))) << skip_lines;
    sad[3] = (((i32u_t)_mm256_extract_epi32(sum0, 3)) + ((i32u_t)_mm256_extract_epi32(sum0, 7))) << skip_lines;
}

// --------------------------------------------------------------------------------------------------------------------
// AVG_SAD
// --------------------------------------------------------------------------------------------------------------------
i32u_t xGetAVGSAD4_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred1 = i_pred1 << skip_lines;
	i_pred2 = i_pred2 << skip_lines;
	int i_org_x2 = i_org << 1;
	int i_pred1_x2 = i_pred1 << 1;
	int i_pred2_x2 = i_pred2 << 1;
	__m128i T0, T1, T2, T3, T4, T5;
	__m128i zero = _mm_setzero_si128();
	__m128i sum = _mm_setzero_si128();
	height >>= (skip_lines + 1);

	while (height--) {
		T0 = _mm_loadl_epi64((__m128i*)p_org);
		T1 = _mm_loadl_epi64((__m128i*)(p_org + i_org));
		T0 = _mm_unpacklo_epi64(T0, T1);

		T2 = _mm_loadl_epi64((__m128i*)p_pred1);
		T3 = _mm_loadl_epi64((__m128i*)(p_pred1 + i_pred1));
		T2 = _mm_unpacklo_epi64(T2, T3);

		T4 = _mm_loadl_epi64((__m128i*)p_pred2);
		T5 = _mm_loadl_epi64((__m128i*)(p_pred2 + i_pred2));
		T4 = _mm_unpacklo_epi64(T4, T5);

		T2 = _mm_avg_epu16(T2, T4);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum = _mm_adds_epi16(sum, T0);

		p_org += i_org_x2;
		p_pred1 += i_pred1_x2;
		p_pred2 += i_pred2_x2;

	}

	T0 = _mm_unpacklo_epi16(sum, zero);
	T1 = _mm_unpackhi_epi16(sum, zero);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;

}

i32u_t xGetAVGSAD8_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred1 = i_pred1 << skip_lines;
	i_pred2 = i_pred2 << skip_lines;
	__m128i T0, T1, T2, T3;
	__m128i zero = _mm_setzero_si128();
	__m128i sum = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T0 = _mm_loadu_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred1);
		T3 = _mm_loadu_si128((__m128i*)p_pred2);

		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum = _mm_adds_epi16(sum, T0);

		p_org += i_org;
		p_pred1 += i_pred1;
		p_pred2 += i_pred2;
	}

	T0 = _mm_unpacklo_epi16(sum, zero);
	T1 = _mm_unpackhi_epi16(sum, zero);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetAVGSAD16_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred1 = i_pred1 << skip_lines;
	i_pred2 = i_pred2 << skip_lines;
	__m128i T0, T1, T2, T3;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T0 = _mm_loadu_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred1);
		T3 = _mm_loadu_si128((__m128i*)p_pred2);

		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum1 = _mm_add_epi16(sum1, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 8));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 8));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 8));

		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum2 = _mm_add_epi16(sum2, T0);

		p_org += i_org;
		p_pred1 += i_pred1;
		p_pred2 += i_pred2;
	}

	T0 = _mm_unpacklo_epi16(sum1, zero);
	T1 = _mm_unpackhi_epi16(sum1, zero);

	T2 = _mm_unpacklo_epi16(sum2, zero);
	T3 = _mm_unpackhi_epi16(sum2, zero);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetAVGSAD32_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred1 = i_pred1 << skip_lines;
	i_pred2 = i_pred2 << skip_lines;
	__m128i T0, T1, T2, T3;
	__m128i T4, T5, T6, T7;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T0 = _mm_loadu_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred1);
		T3 = _mm_loadu_si128((__m128i*)p_pred2);
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum1 = _mm_add_epi16(sum1, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 8));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 8));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 8));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum2 = _mm_add_epi16(sum2, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 16));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 16));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 16));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum3 = _mm_add_epi16(sum3, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 24));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 24));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 24));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum4 = _mm_add_epi16(sum4, T0);

		p_org += i_org;
		p_pred1 += i_pred1;
		p_pred2 += i_pred2;
	}

	T0 = _mm_unpacklo_epi16(sum1, zero);
	T1 = _mm_unpackhi_epi16(sum1, zero);

	T2 = _mm_unpacklo_epi16(sum2, zero);
	T3 = _mm_unpackhi_epi16(sum2, zero);

	T4 = _mm_unpacklo_epi16(sum3, zero);
	T5 = _mm_unpackhi_epi16(sum3, zero);

	T6 = _mm_unpacklo_epi16(sum4, zero);
	T7 = _mm_unpackhi_epi16(sum4, zero);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T2 = _mm_add_epi32(T4, T5);
	T3 = _mm_add_epi32(T6, T7);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetAVGSAD64_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
	i32u_t uiSum = 0;
	i_org = i_org << skip_lines;
	i_pred1 = i_pred1 << skip_lines;
	i_pred2 = i_pred2 << skip_lines;
	__m128i T0, T1, T2, T3;
	__m128i T4, T5, T6, T7;
	__m128i T8, T9;
	__m128i zero = _mm_setzero_si128();
	__m128i sum1 = _mm_setzero_si128();
	__m128i sum2 = _mm_setzero_si128();
	__m128i sum3 = _mm_setzero_si128();
	__m128i sum4 = _mm_setzero_si128();
	__m128i sum5 = _mm_setzero_si128();
	__m128i sum6 = _mm_setzero_si128();
	__m128i sum7 = _mm_setzero_si128();
	__m128i sum8 = _mm_setzero_si128();
	height >>= skip_lines;

	for (; height != 0; height--)
	{
		T0 = _mm_loadu_si128((__m128i*)p_org);
		T2 = _mm_loadu_si128((__m128i*)p_pred1);
		T3 = _mm_loadu_si128((__m128i*)p_pred2);
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum1 = _mm_add_epi16(sum1, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 8));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 8));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 8));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum2 = _mm_add_epi16(sum2, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 16));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 16));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 16));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum3 = _mm_add_epi16(sum3, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 24));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 24));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 24));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum4 = _mm_add_epi16(sum4, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 32));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 32));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 32));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum5 = _mm_add_epi16(sum5, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 40));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 40));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 40));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum6 = _mm_add_epi16(sum6, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 48));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 48));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 48));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum7 = _mm_add_epi16(sum7, T0);

		T0 = _mm_loadu_si128((__m128i*)(p_org + 56));
		T2 = _mm_loadu_si128((__m128i*)(p_pred1 + 56));
		T3 = _mm_loadu_si128((__m128i*)(p_pred2 + 56));
		T2 = _mm_avg_epu16(T2, T3);
		T0 = _mm_sub_epi16(T0, T2);
		T0 = _mm_abs_epi16(T0);
		sum8 = _mm_add_epi16(sum8, T0);

		p_org += i_org;
		p_pred1 += i_pred1;
		p_pred2 += i_pred2;
	}

	T0 = _mm_unpacklo_epi16(sum1, zero);
	T1 = _mm_unpackhi_epi16(sum1, zero);
	T2 = _mm_unpacklo_epi16(sum2, zero);
	T3 = _mm_unpackhi_epi16(sum2, zero);
	T4 = _mm_unpacklo_epi16(sum3, zero);
	T5 = _mm_unpackhi_epi16(sum3, zero);
	T6 = _mm_unpacklo_epi16(sum4, zero);
	T7 = _mm_unpackhi_epi16(sum4, zero);

	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);
	T2 = _mm_add_epi32(T4, T5);
	T3 = _mm_add_epi32(T6, T7);
	T0 = _mm_add_epi32(T0, T2);
	T1 = _mm_add_epi32(T1, T3);

	T8 = _mm_unpacklo_epi16(sum5, zero);
	T9 = _mm_unpackhi_epi16(sum5, zero);
	T2 = _mm_unpacklo_epi16(sum6, zero);
	T3 = _mm_unpackhi_epi16(sum6, zero);
	T4 = _mm_unpacklo_epi16(sum7, zero);
	T5 = _mm_unpackhi_epi16(sum7, zero);
	T6 = _mm_unpacklo_epi16(sum8, zero);
	T7 = _mm_unpackhi_epi16(sum8, zero);

	T8 = _mm_add_epi32(T8, T2);
	T9 = _mm_add_epi32(T9, T3);
	T2 = _mm_add_epi32(T4, T5);
	T3 = _mm_add_epi32(T6, T7);
	T8 = _mm_add_epi32(T8, T2);
	T9 = _mm_add_epi32(T9, T3);

	T0 = _mm_add_epi32(T0, T8);
	T1 = _mm_add_epi32(T1, T9);

	T0 = _mm_add_epi32(T0, T1);
	T0 = _mm_hadd_epi32(T0, T0);
	T0 = _mm_hadd_epi32(T0, T0);
	uiSum = _mm_extract_epi32(T0, 0);

	uiSum <<= skip_lines;
	return uiSum;
}

i32u_t xGetAVGSAD16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i zero = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10;
    __m256i T20;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T10 = _mm256_loadu_si256((__m256i*)(p_pred1));
        T20 = _mm256_loadu_si256((__m256i*)(p_pred2));

        T10 = _mm256_avg_epu16(T10, T20);

        T00 = _mm256_sub_epi16(T00, T10);
        T00 = _mm256_abs_epi16(T00);
        sum0 = _mm256_add_epi16(sum0, T00);

        p_org += i_org;
        p_pred1 += i_pred1;
        p_pred2 += i_pred2;
    }

    T00 = _mm256_unpacklo_epi16(sum0, zero);
    T02 = _mm256_unpackhi_epi16(sum0, zero);

    sum0 = _mm256_add_epi32(T00, T02);

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i zero = _mm256_setzero_si256();
    __m256i T00, T02;
    __m256i T10, T12;
    __m256i T20, T22;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T02 = _mm256_load_si256((__m256i*)(p_org + 16));

        T10 = _mm256_loadu_si256((__m256i*)(p_pred1));
        T12 = _mm256_loadu_si256((__m256i*)(p_pred1 + 16));

        T20 = _mm256_loadu_si256((__m256i*)(p_pred2));
        T22 = _mm256_loadu_si256((__m256i*)(p_pred2 + 16));

        T10 = _mm256_avg_epu16(T10, T20);
        T12 = _mm256_avg_epu16(T12, T22);

        T00 = _mm256_sub_epi16(T00, T10);
        T00 = _mm256_abs_epi16(T00);
        sum0 = _mm256_add_epi16(sum0, T00);

        T02 = _mm256_sub_epi16(T02, T12);
        T02 = _mm256_abs_epi16(T02);
        sum0 = _mm256_add_epi16(sum0, T02);

        p_org += i_org;
        p_pred1 += i_pred1;
        p_pred2 += i_pred2;
    }

    T00 = _mm256_unpacklo_epi16(sum0, zero);
    T02 = _mm256_unpackhi_epi16(sum0, zero);

    sum0 = _mm256_add_epi32(T00, T02);

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

i32u_t xGetAVGSAD64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1, pel_t *p_pred2, int i_pred2, int height, int skip_lines)
{
    i32u_t uiSum = 0;
    __m256i sum0 = _mm256_setzero_si256();
    __m256i sum1 = _mm256_setzero_si256();
    __m256i sum2 = _mm256_setzero_si256();
    __m256i sum3 = _mm256_setzero_si256();
    __m256i zero = _mm256_setzero_si256();
    __m256i T00, T01, T02, T03;
    __m256i T10, T11, T12, T13;
    __m256i T20, T21, T22, T23;

    i_org <<= skip_lines;
    i_pred1 <<= skip_lines;
    i_pred2 <<= skip_lines;
    height >>= (skip_lines);

    while (height--) {
        T00 = _mm256_load_si256((__m256i*)(p_org));
        T01 = _mm256_load_si256((__m256i*)(p_org + 16));
        T02 = _mm256_load_si256((__m256i*)(p_org + 32));
        T03 = _mm256_load_si256((__m256i*)(p_org + 48));

        T10 = _mm256_loadu_si256((__m256i*)(p_pred1));
        T11 = _mm256_loadu_si256((__m256i*)(p_pred1 + 16));
        T12 = _mm256_loadu_si256((__m256i*)(p_pred1 + 32));
        T13 = _mm256_loadu_si256((__m256i*)(p_pred1 + 48));

        T20 = _mm256_loadu_si256((__m256i*)(p_pred2));
        T21 = _mm256_loadu_si256((__m256i*)(p_pred2 + 16));
        T22 = _mm256_loadu_si256((__m256i*)(p_pred2 + 32));
        T23 = _mm256_loadu_si256((__m256i*)(p_pred2 + 48));

        T10 = _mm256_avg_epu16(T10, T20);
        T11 = _mm256_avg_epu16(T11, T21);
        T12 = _mm256_avg_epu16(T12, T22);
        T13 = _mm256_avg_epu16(T13, T23);

        T00 = _mm256_sub_epi16(T00, T10);
        T00 = _mm256_abs_epi16(T00);
        sum0 = _mm256_add_epi16(sum0, T00);

        T01 = _mm256_sub_epi16(T01, T11);
        T01 = _mm256_abs_epi16(T01);
        sum1 = _mm256_add_epi16(sum1, T01);

        T02 = _mm256_sub_epi16(T02, T12);
        T02 = _mm256_abs_epi16(T02);
        sum2 = _mm256_add_epi16(sum2, T02);

        T03 = _mm256_sub_epi16(T03, T13);
        T03 = _mm256_abs_epi16(T03);
        sum3 = _mm256_add_epi16(sum3, T03);

        p_org += i_org;
        p_pred1 += i_pred1;
        p_pred2 += i_pred2;
    }

    T00 = _mm256_unpacklo_epi16(sum0, zero);
    T10 = _mm256_unpackhi_epi16(sum0, zero);

    T01 = _mm256_unpacklo_epi16(sum1, zero);
    T11 = _mm256_unpackhi_epi16(sum1, zero);

    T02 = _mm256_unpacklo_epi16(sum2, zero);
    T12 = _mm256_unpackhi_epi16(sum2, zero);

    T03 = _mm256_unpacklo_epi16(sum3, zero);
    T13 = _mm256_unpackhi_epi16(sum3, zero);

    sum0 = _mm256_add_epi32(T00, T10);
    sum1 = _mm256_add_epi32(T01, T11);
    sum2 = _mm256_add_epi32(T02, T12);
    sum3 = _mm256_add_epi32(T03, T13);

    sum0 = _mm256_add_epi32(sum0, sum1);
    sum1 = _mm256_add_epi32(sum2, sum3);
    sum0 = _mm256_add_epi32(sum0, sum1);

    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    uiSum = ((i32u_t)_mm256_extract_epi32(sum0, 0)) + ((i32u_t)_mm256_extract_epi32(sum0, 4));

    uiSum <<= skip_lines;
    return uiSum;
}

// --------------------------------------------------------------------------------------------------------------------
// HADAMARD with step (used in fractional search)
// --------------------------------------------------------------------------------------------------------------------
i32u_t xCalcHAD4x4_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
	int uiSum = 0;
	int i_org_x2 = i_org << 1;
	int i_pred_x2 = i_pred << 1;

	__m128i T0, T1, T2, T3, T4, T5, T6, T7;
	__m128i M0, M1, M2, M3;
	__m128i sign1 = _mm_set_epi16(1, -1, 1, -1, 1, -1, 1, -1);
	__m128i sign2 = _mm_set_epi16(1, -1, -1, 1, 1, -1, -1, 1);
	__m128i sign3 = _mm_set_epi16(1, 1, -1, -1, 1, 1, -1, -1);

	T0 = _mm_loadl_epi64((__m128i*)p_org);
	T2 = _mm_loadl_epi64((__m128i*)(p_org + i_org));
	T0 = _mm_unpacklo_epi64(T0, T2);
	T1 = _mm_loadl_epi64((__m128i*)p_pred);
	T3 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred));
	T1 = _mm_unpacklo_epi64(T1, T3);
	M0 = _mm_sub_epi16(T0, T1);

	T4 = _mm_loadl_epi64((__m128i*)(p_org + i_org_x2));
	T6 = _mm_loadl_epi64((__m128i*)(p_org + i_org_x2 + i_org));
	T4 = _mm_unpacklo_epi64(T6, T4);
	T5 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred_x2));
	T7 = _mm_loadl_epi64((__m128i*)(p_pred + i_pred_x2 + i_pred));
	T5 = _mm_unpacklo_epi64(T7, T5);
	M2 = _mm_sub_epi16(T4, T5);

	/*===== hadamard transform =====*/
	T0 = _mm_add_epi16(M0, M2);
	T3 = _mm_sub_epi16(M0, M2);

	T1 = _mm_unpackhi_epi64(T0, T0);
	T2 = _mm_unpackhi_epi64(T3, T3);

	M0 = _mm_add_epi16(T0, T1); //d0, d1, d2, d3
	M1 = _mm_add_epi16(T2, T3);
	M2 = _mm_sub_epi16(T0, T1);
	M3 = _mm_sub_epi16(T3, T2);

	/*
	d'[ 0] = d[ 0] + d[ 3] + d[ 1] + d[ 2];
	d'[ 1] = d[ 0] + d[ 3] - d[ 1] - d[ 2];
	d'[ 2] = d[ 1] - d[ 2] + d[ 0] - d[ 3];
	d'[ 3] = d[ 0] - d[ 3] - d[ 1] + d[ 2];
	*/
	T0 = _mm_set1_epi16(_mm_extract_epi16(M0, 0));
	T1 = _mm_set1_epi16(_mm_extract_epi16(M0, 1));
	T2 = _mm_set1_epi16(_mm_extract_epi16(M0, 2));
	T3 = _mm_set1_epi16(_mm_extract_epi16(M0, 3));

	T4 = _mm_set1_epi16(_mm_extract_epi16(M1, 0));
	T5 = _mm_set1_epi16(_mm_extract_epi16(M1, 1));
	T6 = _mm_set1_epi16(_mm_extract_epi16(M1, 2));
	T7 = _mm_set1_epi16(_mm_extract_epi16(M1, 3));

	T0 = _mm_unpacklo_epi64(T0, T4);
	T1 = _mm_unpacklo_epi64(T1, T5);
	T2 = _mm_unpacklo_epi64(T2, T6);
	T3 = _mm_unpacklo_epi64(T3, T7);

	T1 = _mm_sign_epi16(T1, sign1);
	T2 = _mm_sign_epi16(T2, sign2);
	T3 = _mm_sign_epi16(T3, sign3);

	T0 = _mm_add_epi16(T0, T1);
	T2 = _mm_add_epi16(T2, T3);
	M0 = _mm_add_epi16(T0, T2);

	T0 = _mm_set1_epi16(_mm_extract_epi16(M2, 0));
	T1 = _mm_set1_epi16(_mm_extract_epi16(M2, 1));
	T2 = _mm_set1_epi16(_mm_extract_epi16(M2, 2));
	T3 = _mm_set1_epi16(_mm_extract_epi16(M2, 3));

	T4 = _mm_set1_epi16(_mm_extract_epi16(M3, 0));
	T5 = _mm_set1_epi16(_mm_extract_epi16(M3, 1));
	T6 = _mm_set1_epi16(_mm_extract_epi16(M3, 2));
	T7 = _mm_set1_epi16(_mm_extract_epi16(M3, 3));

	T0 = _mm_unpacklo_epi64(T0, T4);
	T1 = _mm_unpacklo_epi64(T1, T5);
	T2 = _mm_unpacklo_epi64(T2, T6);
	T3 = _mm_unpacklo_epi64(T3, T7);

	T1 = _mm_sign_epi16(T1, sign1);
	T2 = _mm_sign_epi16(T2, sign2);
	T3 = _mm_sign_epi16(T3, sign3);

	T0 = _mm_add_epi16(T0, T1);
	T2 = _mm_add_epi16(T2, T3);
	M2 = _mm_add_epi16(T0, T2);

	T0 = _mm_abs_epi16(M0);
	T1 = _mm_abs_epi16(M2);

	T0 = _mm_add_epi16(T0, T1);
	T0 = _mm_hadd_epi16(T0, T0);
	T0 = _mm_hadd_epi16(T0, T0);
	T0 = _mm_hadd_epi16(T0, T0);

	uiSum = _mm_extract_epi16(T0, 0);

	uiSum = ((uiSum + 1) >> 1);

	return uiSum;
}

i32u_t xCalcHAD8x8_sse128_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
	int uiSum = 0;
	__m128i R0, R1, R2, R3;
	__m128i T0, T1, T2, T3, T4, T5, T6, T7;
	__m128i M0, M1, M2, M3, M4, M5, M6, M7;
	__m128i S0, S1, S2, S3, S4, S5, S6, S7;
	__m128i sum;

	R0 = _mm_load_si128((__m128i const*)p_org);
	R1 = _mm_loadu_si128((__m128i const*)p_pred);
	R2 = _mm_load_si128((__m128i const*)(p_org + i_org));
	R3 = _mm_loadu_si128((__m128i const*)(p_pred + i_pred));
	M0 = _mm_sub_epi16(R0, R1);
	M1 = _mm_sub_epi16(R2, R3);
	p_pred += (i_pred << 1);
	p_org += (i_org << 1);


	R0 = _mm_load_si128((__m128i const*)p_org);
	R1 = _mm_loadu_si128((__m128i const*)p_pred);
	R2 = _mm_load_si128((__m128i const*)(p_org + i_org));
	R3 = _mm_loadu_si128((__m128i const*)(p_pred + i_pred));
	M2 = _mm_sub_epi16(R0, R1);
	M3 = _mm_sub_epi16(R2, R3);
	p_pred += (i_pred << 1);
	p_org += (i_org << 1);

	R0 = _mm_load_si128((__m128i const*)p_org);
	R1 = _mm_loadu_si128((__m128i const*)p_pred);
	R2 = _mm_load_si128((__m128i const*)(p_org + i_org));
	R3 = _mm_loadu_si128((__m128i const*)(p_pred + i_pred));
	M4 = _mm_sub_epi16(R0, R1);
	M5 = _mm_sub_epi16(R2, R3);
	p_pred += (i_pred << 1);
	p_org += (i_org << 1);

	R0 = _mm_load_si128((__m128i const*)p_org);
	R1 = _mm_loadu_si128((__m128i const*)p_pred);
	R2 = _mm_load_si128((__m128i const*)(p_org + i_org));
	R3 = _mm_loadu_si128((__m128i const*)(p_pred + i_pred));
	M6 = _mm_sub_epi16(R0, R1);
	M7 = _mm_sub_epi16(R2, R3);
	p_pred += (i_pred << 1);
	p_org += (i_org << 1);

	// vertical
	T0 = _mm_add_epi16(M0, M4);
	T1 = _mm_add_epi16(M1, M5);
	T2 = _mm_add_epi16(M2, M6);
	T3 = _mm_add_epi16(M3, M7);
	T4 = _mm_sub_epi16(M0, M4);
	T5 = _mm_sub_epi16(M1, M5);
	T6 = _mm_sub_epi16(M2, M6);
	T7 = _mm_sub_epi16(M3, M7);

	M0 = _mm_add_epi16(T0, T2);
	M1 = _mm_add_epi16(T1, T3);
	M2 = _mm_sub_epi16(T0, T2);
	M3 = _mm_sub_epi16(T1, T3);
	M4 = _mm_add_epi16(T4, T6);
	M5 = _mm_add_epi16(T5, T7);
	M6 = _mm_sub_epi16(T4, T6);
	M7 = _mm_sub_epi16(T5, T7);

	T0 = _mm_add_epi16(M0, M1);
	T7 = _mm_sub_epi16(M0, M1);
	T3 = _mm_add_epi16(M2, M3);
	T4 = _mm_sub_epi16(M2, M3);
	T1 = _mm_add_epi16(M4, M5);
	T6 = _mm_sub_epi16(M4, M5);
	T2 = _mm_add_epi16(M6, M7);
	T5 = _mm_sub_epi16(M6, M7);

	//transpose
	R0 = _mm_unpacklo_epi16(T0, T1);
	R1 = _mm_unpacklo_epi16(T2, T3);
	R2 = _mm_unpacklo_epi16(T4, T5);
	R3 = _mm_unpacklo_epi16(T6, T7);

	M4 = _mm_unpacklo_epi32(R0, R1);
	M5 = _mm_unpackhi_epi32(R0, R1);
	M6 = _mm_unpacklo_epi32(R2, R3);
	M7 = _mm_unpackhi_epi32(R2, R3);

	M0 = _mm_unpacklo_epi64(M4, M6);
	M1 = _mm_unpackhi_epi64(M4, M6);
	M2 = _mm_unpacklo_epi64(M5, M7);
	M3 = _mm_unpackhi_epi64(M5, M7);

	R0 = _mm_unpackhi_epi16(T0, T1);
	R1 = _mm_unpackhi_epi16(T2, T3);
	R2 = _mm_unpackhi_epi16(T4, T5);
	R3 = _mm_unpackhi_epi16(T6, T7);

	T4 = _mm_unpacklo_epi32(R0, R1);
	T5 = _mm_unpackhi_epi32(R0, R1);
	T6 = _mm_unpacklo_epi32(R2, R3);
	T7 = _mm_unpackhi_epi32(R2, R3);

	M4 = _mm_unpacklo_epi64(T4, T6);
	M5 = _mm_unpackhi_epi64(T4, T6);
	M6 = _mm_unpacklo_epi64(T5, T7);
	M7 = _mm_unpackhi_epi64(T5, T7);

	// vertical
	T0 = _mm_add_epi16(M0, M4);
	T1 = _mm_add_epi16(M1, M5);
	T2 = _mm_add_epi16(M2, M6);
	T3 = _mm_add_epi16(M3, M7);
	T4 = _mm_sub_epi16(M0, M4);
	T5 = _mm_sub_epi16(M1, M5);
	T6 = _mm_sub_epi16(M2, M6);
	T7 = _mm_sub_epi16(M3, M7);

	M0 = _mm_add_epi16(T0, T2);
	M1 = _mm_add_epi16(T1, T3);
	M2 = _mm_sub_epi16(T0, T2);
	M3 = _mm_sub_epi16(T1, T3);
	M4 = _mm_add_epi16(T4, T6);
	M5 = _mm_add_epi16(T5, T7);
	M6 = _mm_sub_epi16(T4, T6);
	M7 = _mm_sub_epi16(T5, T7);

	T0 = _mm_cvtepi16_epi32(M0);
	M0 = _mm_srli_si128(M0, 8);
	T1 = _mm_cvtepi16_epi32(M0);
	T2 = _mm_cvtepi16_epi32(M1);
	M1 = _mm_srli_si128(M1, 8);
	T3 = _mm_cvtepi16_epi32(M1);
	T4 = _mm_cvtepi16_epi32(M2);
	M2 = _mm_srli_si128(M2, 8);
	T5 = _mm_cvtepi16_epi32(M2);
	T6 = _mm_cvtepi16_epi32(M3);
	M3 = _mm_srli_si128(M3, 8);
	T7 = _mm_cvtepi16_epi32(M3);

	S0 = _mm_abs_epi32(_mm_add_epi32(T0, T2));
	S7 = _mm_abs_epi32(_mm_sub_epi32(T0, T2));
	S3 = _mm_abs_epi32(_mm_add_epi32(T1, T3));
	S4 = _mm_abs_epi32(_mm_sub_epi32(T1, T3));
	S1 = _mm_abs_epi32(_mm_add_epi32(T4, T6));
	S6 = _mm_abs_epi32(_mm_sub_epi32(T4, T6));
	S2 = _mm_abs_epi32(_mm_add_epi32(T5, T7));
	S5 = _mm_abs_epi32(_mm_sub_epi32(T5, T7));

	T0 = _mm_cvtepi16_epi32(M4);
	M4 = _mm_srli_si128(M4, 8);
	T1 = _mm_cvtepi16_epi32(M4);
	T2 = _mm_cvtepi16_epi32(M5);
	M5 = _mm_srli_si128(M5, 8);
	T3 = _mm_cvtepi16_epi32(M5);
	T4 = _mm_cvtepi16_epi32(M6);
	M6 = _mm_srli_si128(M6, 8);
	T5 = _mm_cvtepi16_epi32(M6);
	T6 = _mm_cvtepi16_epi32(M7);
	M7 = _mm_srli_si128(M7, 8);
	T7 = _mm_cvtepi16_epi32(M7);

	M0 = _mm_abs_epi32(_mm_add_epi32(T0, T2));
	M7 = _mm_abs_epi32(_mm_sub_epi32(T0, T2));
	M3 = _mm_abs_epi32(_mm_add_epi32(T1, T3));
	M4 = _mm_abs_epi32(_mm_sub_epi32(T1, T3));
	M1 = _mm_abs_epi32(_mm_add_epi32(T4, T6));
	M6 = _mm_abs_epi32(_mm_sub_epi32(T4, T6));
	M2 = _mm_abs_epi32(_mm_add_epi32(T5, T7));
	M5 = _mm_abs_epi32(_mm_sub_epi32(T5, T7));

	T0 = _mm_add_epi32(M0, S0);
	T1 = _mm_add_epi32(M1, S1);
	T2 = _mm_add_epi32(M2, S2);
	T3 = _mm_add_epi32(M3, S3);
	T4 = _mm_add_epi32(M4, S4);
	T5 = _mm_add_epi32(M5, S5);
	T6 = _mm_add_epi32(M6, S6);
	T7 = _mm_add_epi32(M7, S7);

	T0 = _mm_add_epi32(T0, T1);
	T2 = _mm_add_epi32(T2, T3);
	T4 = _mm_add_epi32(T4, T5);
	T6 = _mm_add_epi32(T6, T7);

	T0 = _mm_add_epi32(T0, T2);
	T4 = _mm_add_epi32(T4, T6);

	sum = _mm_add_epi32(T0, T4);

	sum = _mm_hadd_epi32(sum, sum);
	sum = _mm_hadd_epi32(sum, sum);
	uiSum = _mm_cvtsi128_si32(sum);
	uiSum = ((uiSum + 2) >> 2);

	return uiSum;
}

i32u_t xCalcHADs8x8_I_sse128(pel_t *p_org, int i_org)
{
    int uiDC, uiSum = 0;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2, M3, M4, M5, M6, M7;
    const __m128i zero = _mm_setzero_si128();
    __m128i sum;
    __m128i sign = _mm_set_epi16(1, 1, -1, -1, 1, 1, -1, -1);

    // horizontal
#define HOR_LINE(i, j) { \
    T0 = _mm_loadl_epi64((__m128i const*)p_org); \
    T4 = _mm_loadl_epi64((__m128i const*)(p_org + i_org)); \
    T0 = _mm_cvtepu8_epi16(T0); \
    T4 = _mm_cvtepu8_epi16(T4); \
    T1 = _mm_unpackhi_epi64(T0, T0); \
    T5 = _mm_unpackhi_epi64(T4, T4); \
    T2 = _mm_add_epi16(T0, T1); \
    T3 = _mm_sub_epi16(T0, T1); \
    T6 = _mm_add_epi16(T4, T5); \
    T7 = _mm_sub_epi16(T4, T5); \
    T0 = _mm_unpacklo_epi32(T2, T3); \
    T4 = _mm_unpacklo_epi32(T6, T7); \
    T1 = _mm_unpackhi_epi32(T0, T0); \
    T2 = _mm_unpacklo_epi32(T0, T0); \
    T5 = _mm_unpackhi_epi32(T4, T4); \
    T6 = _mm_unpacklo_epi32(T4, T4); \
    T1 = _mm_sign_epi16(T1, sign); \
    T5 = _mm_sign_epi16(T5, sign); \
    T0 = _mm_add_epi16(T2, T1); \
    T4 = _mm_add_epi16(T6, T5); \
    T1 = _mm_hadd_epi16(T0, T0); \
    T2 = _mm_hsub_epi16(T0, T0); \
    T5 = _mm_hadd_epi16(T4, T4); \
    T6 = _mm_hsub_epi16(T4, T4); \
    M##i = _mm_unpacklo_epi16(T1, T2); \
    M##j = _mm_unpacklo_epi16(T5, T6); \
    p_org += (i_org << 1); \
    }

    HOR_LINE(0, 1)
        HOR_LINE(2, 3)
        HOR_LINE(4, 5)
        HOR_LINE(6, 7)

#undef HOR_LINE

    // vertical
    T0 = _mm_add_epi16(M0, M4);
    T1 = _mm_add_epi16(M1, M5);
    T2 = _mm_add_epi16(M2, M6);
    T3 = _mm_add_epi16(M3, M7);
    T4 = _mm_sub_epi16(M0, M4);
    T5 = _mm_sub_epi16(M1, M5);
    T6 = _mm_sub_epi16(M2, M6);
    T7 = _mm_sub_epi16(M3, M7);

    M0 = _mm_add_epi16(T0, T2);
    M1 = _mm_add_epi16(T1, T3);
    M2 = _mm_sub_epi16(T0, T2);
    M3 = _mm_sub_epi16(T1, T3);
    M4 = _mm_add_epi16(T4, T6);
    M5 = _mm_add_epi16(T5, T7);
    M6 = _mm_sub_epi16(T4, T6);
    M7 = _mm_sub_epi16(T5, T7);

    T0 = _mm_abs_epi16(_mm_add_epi16(M0, M1));
    T1 = _mm_abs_epi16(_mm_sub_epi16(M0, M1));
    T2 = _mm_abs_epi16(_mm_add_epi16(M2, M3));
    T3 = _mm_abs_epi16(_mm_sub_epi16(M2, M3));
    T4 = _mm_abs_epi16(_mm_add_epi16(M4, M5));
    T5 = _mm_abs_epi16(_mm_sub_epi16(M4, M5));
    T6 = _mm_abs_epi16(_mm_add_epi16(M6, M7));
    T7 = _mm_abs_epi16(_mm_sub_epi16(M6, M7));

    uiDC = _mm_extract_epi16(T0, 2);

    T0 = _mm_add_epi16(T0, T1);
    T2 = _mm_add_epi16(T2, T3);
    T4 = _mm_add_epi16(T4, T5);
    T6 = _mm_add_epi16(T6, T7);

    T0 = _mm_add_epi16(T0, T2);
    T4 = _mm_add_epi16(T4, T6);

    M0 = _mm_cvtepi16_epi32(T0);
    M2 = _mm_cvtepi16_epi32(T4);
    M4 = _mm_unpackhi_epi16(T0, zero);
    M6 = _mm_unpackhi_epi16(T4, zero);

    M0 = _mm_add_epi32(M0, M2);
    M4 = _mm_add_epi32(M4, M6);

    sum = _mm_add_epi32(M0, M4);

    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    uiSum = _mm_cvtsi128_si32(sum) - uiDC;
    uiSum = ((uiSum + 2) >> 2);

    return uiSum;
}

i32u_t xCalcHADs8x8_I_sse128_10bit(pel_t *p_org, int i_org)
{
    int uiDC, uiSum = 0;
    __m128i R0, R1, R2, R3;
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2, M3, M4, M5, M6, M7;
    __m128i S0, S1, S2, S3, S4, S5, S6, S7;
    __m128i sum;

    M0 = _mm_load_si128((__m128i const*)p_org);
    M1 = _mm_load_si128((__m128i const*)(p_org + i_org));
    p_org += (i_org << 1);

    M2 = _mm_load_si128((__m128i const*)p_org);
    M3 = _mm_load_si128((__m128i const*)(p_org + i_org));
    p_org += (i_org << 1);

    M4 = _mm_load_si128((__m128i const*)p_org);
    M5 = _mm_load_si128((__m128i const*)(p_org + i_org));
    p_org += (i_org << 1);

    M6 = _mm_load_si128((__m128i const*)p_org);
    M7 = _mm_load_si128((__m128i const*)(p_org + i_org));
    p_org += (i_org << 1);

    // vertical
    T0 = _mm_add_epi16(M0, M4);
    T1 = _mm_add_epi16(M1, M5);
    T2 = _mm_add_epi16(M2, M6);
    T3 = _mm_add_epi16(M3, M7);
    T4 = _mm_sub_epi16(M0, M4);
    T5 = _mm_sub_epi16(M1, M5);
    T6 = _mm_sub_epi16(M2, M6);
    T7 = _mm_sub_epi16(M3, M7);

    M0 = _mm_add_epi16(T0, T2);
    M1 = _mm_add_epi16(T1, T3);
    M2 = _mm_sub_epi16(T0, T2);
    M3 = _mm_sub_epi16(T1, T3);
    M4 = _mm_add_epi16(T4, T6);
    M5 = _mm_add_epi16(T5, T7);
    M6 = _mm_sub_epi16(T4, T6);
    M7 = _mm_sub_epi16(T5, T7);

    T0 = _mm_add_epi16(M0, M1);
    T7 = _mm_sub_epi16(M0, M1);
    T3 = _mm_add_epi16(M2, M3);
    T4 = _mm_sub_epi16(M2, M3);
    T1 = _mm_add_epi16(M4, M5);
    T6 = _mm_sub_epi16(M4, M5);
    T2 = _mm_add_epi16(M6, M7);
    T5 = _mm_sub_epi16(M6, M7);

    //transpose
    R0 = _mm_unpacklo_epi16(T0, T1);
    R1 = _mm_unpacklo_epi16(T2, T3);
    R2 = _mm_unpacklo_epi16(T4, T5);
    R3 = _mm_unpacklo_epi16(T6, T7);

    M4 = _mm_unpacklo_epi32(R0, R1);
    M5 = _mm_unpackhi_epi32(R0, R1);
    M6 = _mm_unpacklo_epi32(R2, R3);
    M7 = _mm_unpackhi_epi32(R2, R3);

    M0 = _mm_unpacklo_epi64(M4, M6);
    M1 = _mm_unpackhi_epi64(M4, M6);
    M2 = _mm_unpacklo_epi64(M5, M7);
    M3 = _mm_unpackhi_epi64(M5, M7);

    R0 = _mm_unpackhi_epi16(T0, T1);
    R1 = _mm_unpackhi_epi16(T2, T3);
    R2 = _mm_unpackhi_epi16(T4, T5);
    R3 = _mm_unpackhi_epi16(T6, T7);

    T4 = _mm_unpacklo_epi32(R0, R1);
    T5 = _mm_unpackhi_epi32(R0, R1);
    T6 = _mm_unpacklo_epi32(R2, R3);
    T7 = _mm_unpackhi_epi32(R2, R3);

    M4 = _mm_unpacklo_epi64(T4, T6);
    M5 = _mm_unpackhi_epi64(T4, T6);
    M6 = _mm_unpacklo_epi64(T5, T7);
    M7 = _mm_unpackhi_epi64(T5, T7);

    // vertical
    T0 = _mm_add_epi16(M0, M4);
    T1 = _mm_add_epi16(M1, M5);
    T2 = _mm_add_epi16(M2, M6);
    T3 = _mm_add_epi16(M3, M7);
    T4 = _mm_sub_epi16(M0, M4);
    T5 = _mm_sub_epi16(M1, M5);
    T6 = _mm_sub_epi16(M2, M6);
    T7 = _mm_sub_epi16(M3, M7);

    M0 = _mm_add_epi16(T0, T2);
    M1 = _mm_add_epi16(T1, T3);
    M2 = _mm_sub_epi16(T0, T2);
    M3 = _mm_sub_epi16(T1, T3);
    M4 = _mm_add_epi16(T4, T6);
    M5 = _mm_add_epi16(T5, T7);
    M6 = _mm_sub_epi16(T4, T6);
    M7 = _mm_sub_epi16(T5, T7);

    T0 = _mm_cvtepi16_epi32(M0);
    M0 = _mm_srli_si128(M0, 8);
    T1 = _mm_cvtepi16_epi32(M0);
    T2 = _mm_cvtepi16_epi32(M1);
    M1 = _mm_srli_si128(M1, 8);
    T3 = _mm_cvtepi16_epi32(M1);
    T4 = _mm_cvtepi16_epi32(M2);
    M2 = _mm_srli_si128(M2, 8);
    T5 = _mm_cvtepi16_epi32(M2);
    T6 = _mm_cvtepi16_epi32(M3);
    M3 = _mm_srli_si128(M3, 8);
    T7 = _mm_cvtepi16_epi32(M3);

    S0 = _mm_abs_epi32(_mm_add_epi32(T0, T2));
    S7 = _mm_abs_epi32(_mm_sub_epi32(T0, T2));
    S3 = _mm_abs_epi32(_mm_add_epi32(T1, T3));
    S4 = _mm_abs_epi32(_mm_sub_epi32(T1, T3));
    S1 = _mm_abs_epi32(_mm_add_epi32(T4, T6));
    S6 = _mm_abs_epi32(_mm_sub_epi32(T4, T6));
    S2 = _mm_abs_epi32(_mm_add_epi32(T5, T7));
    S5 = _mm_abs_epi32(_mm_sub_epi32(T5, T7));

    T0 = _mm_cvtepi16_epi32(M4);
    M4 = _mm_srli_si128(M4, 8);
    T1 = _mm_cvtepi16_epi32(M4);
    T2 = _mm_cvtepi16_epi32(M5);
    M5 = _mm_srli_si128(M5, 8);
    T3 = _mm_cvtepi16_epi32(M5);
    T4 = _mm_cvtepi16_epi32(M6);
    M6 = _mm_srli_si128(M6, 8);
    T5 = _mm_cvtepi16_epi32(M6);
    T6 = _mm_cvtepi16_epi32(M7);
    M7 = _mm_srli_si128(M7, 8);
    T7 = _mm_cvtepi16_epi32(M7);

    M0 = _mm_abs_epi32(_mm_add_epi32(T0, T2));
    M7 = _mm_abs_epi32(_mm_sub_epi32(T0, T2));
    M3 = _mm_abs_epi32(_mm_add_epi32(T1, T3));
    M4 = _mm_abs_epi32(_mm_sub_epi32(T1, T3));
    M1 = _mm_abs_epi32(_mm_add_epi32(T4, T6));
    M6 = _mm_abs_epi32(_mm_sub_epi32(T4, T6));
    M2 = _mm_abs_epi32(_mm_add_epi32(T5, T7));
    M5 = _mm_abs_epi32(_mm_sub_epi32(T5, T7));

    M2 = _mm_insert_epi32(M2, 0, 1);
    M3 = _mm_insert_epi32(M3, 0, 1);
    M4 = _mm_insert_epi32(M4, 0, 1);
    M5 = _mm_setzero_si128();
    M6 = _mm_setzero_si128();

    S2 = _mm_insert_epi32(S2, 0, 1);
    S3 = _mm_insert_epi32(S3, 0, 1);
    S4 = _mm_insert_epi32(S4, 0, 1);
    S5 = _mm_insert_epi32(S5, 0, 1);

    uiDC = _mm_extract_epi32(S0, 0);

    T0 = _mm_add_epi32(M0, S0);
    T1 = _mm_add_epi32(M1, S1);
    T2 = _mm_add_epi32(M2, S2);
    T3 = _mm_add_epi32(M3, S3);
    T4 = _mm_add_epi32(M4, S4);
    T5 = _mm_add_epi32(M5, S5);
    T6 = _mm_add_epi32(M6, S6);
    T7 = _mm_add_epi32(M7, S7);

    T0 = _mm_add_epi32(T0, T1);
    T2 = _mm_add_epi32(T2, T3);
    T4 = _mm_add_epi32(T4, T5);
    T6 = _mm_add_epi32(T6, T7);

    T0 = _mm_add_epi32(T0, T2);
    T4 = _mm_add_epi32(T4, T6);

    sum = _mm_add_epi32(T0, T4);

    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    uiSum = _mm_cvtsi128_si32(sum) - uiDC;
    uiSum = ((uiSum + 2) >> 2);

    return uiSum;
}

i32u_t xCalcHAD16x16_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int i, uiSum = 0;
    __m256i R0, R1, R2, R3;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i sum;

    for (i = 0; i < 2; i++) {
        R0 = _mm256_loadu_si256((__m256i const*)p_org);
        R1 = _mm256_loadu_si256((__m256i const*)p_pred);
        R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
        R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
        M0 = _mm256_sub_epi16(R0, R1);
        M1 = _mm256_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);


        R0 = _mm256_loadu_si256((__m256i const*)p_org);
        R1 = _mm256_loadu_si256((__m256i const*)p_pred);
        R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
        R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
        M2 = _mm256_sub_epi16(R0, R1);
        M3 = _mm256_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);

        R0 = _mm256_loadu_si256((__m256i const*)p_org);
        R1 = _mm256_loadu_si256((__m256i const*)p_pred);
        R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
        R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
        M4 = _mm256_sub_epi16(R0, R1);
        M5 = _mm256_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);

        R0 = _mm256_loadu_si256((__m256i const*)p_org);
        R1 = _mm256_loadu_si256((__m256i const*)p_pred);
        R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
        R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
        M6 = _mm256_sub_epi16(R0, R1);
        M7 = _mm256_sub_epi16(R2, R3);
        p_pred += (i_pred << 1);
        p_org += (i_org << 1);

        // vertical
        T0 = _mm256_add_epi16(M0, M4);
        T1 = _mm256_add_epi16(M1, M5);
        T2 = _mm256_add_epi16(M2, M6);
        T3 = _mm256_add_epi16(M3, M7);
        T4 = _mm256_sub_epi16(M0, M4);
        T5 = _mm256_sub_epi16(M1, M5);
        T6 = _mm256_sub_epi16(M2, M6);
        T7 = _mm256_sub_epi16(M3, M7);

        M0 = _mm256_add_epi16(T0, T2);
        M1 = _mm256_add_epi16(T1, T3);
        M2 = _mm256_sub_epi16(T0, T2);
        M3 = _mm256_sub_epi16(T1, T3);
        M4 = _mm256_add_epi16(T4, T6);
        M5 = _mm256_add_epi16(T5, T7);
        M6 = _mm256_sub_epi16(T4, T6);
        M7 = _mm256_sub_epi16(T5, T7);

        T0 = _mm256_add_epi16(M0, M1);
        T7 = _mm256_sub_epi16(M0, M1);
        T3 = _mm256_add_epi16(M2, M3);
        T4 = _mm256_sub_epi16(M2, M3);
        T1 = _mm256_add_epi16(M4, M5);
        T6 = _mm256_sub_epi16(M4, M5);
        T2 = _mm256_add_epi16(M6, M7);
        T5 = _mm256_sub_epi16(M6, M7);

        //transpose
        R0 = _mm256_unpacklo_epi16(T0, T1);
        R1 = _mm256_unpacklo_epi16(T2, T3);
        R2 = _mm256_unpacklo_epi16(T4, T5);
        R3 = _mm256_unpacklo_epi16(T6, T7);

        M4 = _mm256_unpacklo_epi32(R0, R1);
        M5 = _mm256_unpackhi_epi32(R0, R1);
        M6 = _mm256_unpacklo_epi32(R2, R3);
        M7 = _mm256_unpackhi_epi32(R2, R3);

        M0 = _mm256_unpacklo_epi64(M4, M6);
        M1 = _mm256_unpackhi_epi64(M4, M6);
        M2 = _mm256_unpacklo_epi64(M5, M7);
        M3 = _mm256_unpackhi_epi64(M5, M7);

        R0 = _mm256_unpackhi_epi16(T0, T1);
        R1 = _mm256_unpackhi_epi16(T2, T3);
        R2 = _mm256_unpackhi_epi16(T4, T5);
        R3 = _mm256_unpackhi_epi16(T6, T7);

        T4 = _mm256_unpacklo_epi32(R0, R1);
        T5 = _mm256_unpackhi_epi32(R0, R1);
        T6 = _mm256_unpacklo_epi32(R2, R3);
        T7 = _mm256_unpackhi_epi32(R2, R3);

        M4 = _mm256_unpacklo_epi64(T4, T6);
        M5 = _mm256_unpackhi_epi64(T4, T6);
        M6 = _mm256_unpacklo_epi64(T5, T7);
        M7 = _mm256_unpackhi_epi64(T5, T7);

        // vertical
        T0 = _mm256_add_epi16(M0, M4);
        T1 = _mm256_add_epi16(M1, M5);
        T2 = _mm256_add_epi16(M2, M6);
        T3 = _mm256_add_epi16(M3, M7);
        T4 = _mm256_sub_epi16(M0, M4);
        T5 = _mm256_sub_epi16(M1, M5);
        T6 = _mm256_sub_epi16(M2, M6);
        T7 = _mm256_sub_epi16(M3, M7);

        M0 = _mm256_add_epi16(T0, T2);
        M1 = _mm256_add_epi16(T1, T3);
        M2 = _mm256_sub_epi16(T0, T2);
        M3 = _mm256_sub_epi16(T1, T3);
        M4 = _mm256_add_epi16(T4, T6);
        M5 = _mm256_add_epi16(T5, T7);
        M6 = _mm256_sub_epi16(T4, T6);
        M7 = _mm256_sub_epi16(T5, T7);

        T0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M0));
        T1 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M0, 1));
        T2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M1));
        T3 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M1, 1));
        T4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M2));
        T5 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M2, 1));
        T6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M3));
        T7 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M3, 1));

        S0 = _mm256_abs_epi32(_mm256_add_epi32(T0, T2));
        S7 = _mm256_abs_epi32(_mm256_sub_epi32(T0, T2));
        S3 = _mm256_abs_epi32(_mm256_add_epi32(T1, T3));
        S4 = _mm256_abs_epi32(_mm256_sub_epi32(T1, T3));
        S1 = _mm256_abs_epi32(_mm256_add_epi32(T4, T6));
        S6 = _mm256_abs_epi32(_mm256_sub_epi32(T4, T6));
        S2 = _mm256_abs_epi32(_mm256_add_epi32(T5, T7));
        S5 = _mm256_abs_epi32(_mm256_sub_epi32(T5, T7));

        T0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M4));
        T1 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M4, 1));
        T2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M5));
        T3 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M5, 1));
        T4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M6));
        T5 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M6, 1));
        T6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M7));
        T7 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M7, 1));

        M0 = _mm256_abs_epi32(_mm256_add_epi32(T0, T2));
        M7 = _mm256_abs_epi32(_mm256_sub_epi32(T0, T2));
        M3 = _mm256_abs_epi32(_mm256_add_epi32(T1, T3));
        M4 = _mm256_abs_epi32(_mm256_sub_epi32(T1, T3));
        M1 = _mm256_abs_epi32(_mm256_add_epi32(T4, T6));
        M6 = _mm256_abs_epi32(_mm256_sub_epi32(T4, T6));
        M2 = _mm256_abs_epi32(_mm256_add_epi32(T5, T7));
        M5 = _mm256_abs_epi32(_mm256_sub_epi32(T5, T7));

        T0 = _mm256_add_epi32(M0, S0);
        T1 = _mm256_add_epi32(M1, S1);
        T2 = _mm256_add_epi32(M2, S2);
        T3 = _mm256_add_epi32(M3, S3);
        T4 = _mm256_add_epi32(M4, S4);
        T5 = _mm256_add_epi32(M5, S5);
        T6 = _mm256_add_epi32(M6, S6);
        T7 = _mm256_add_epi32(M7, S7);

        T0 = _mm256_add_epi32(T0, T1);
        T2 = _mm256_add_epi32(T2, T3);
        T4 = _mm256_add_epi32(T4, T5);
        T6 = _mm256_add_epi32(T6, T7);

        T0 = _mm256_add_epi32(T0, T6);
        T2 = _mm256_add_epi32(T2, T4);

        sum = _mm256_hadd_epi32(T0, T2);

        sum = _mm256_hadd_epi32(sum, sum);
        uiSum += ((_mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4) + 2) >> 2) + ((_mm256_extract_epi32(sum, 1) + _mm256_extract_epi32(sum, 5) + 2) >> 2);
    }

    return uiSum;
}

i32u_t xCalcHAD32x32_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int i, j, uiSum = 0;
    __m256i R0, R1, R2, R3;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i sum;
    pel_t *p_org_bak = p_org;
    pel_t *p_pred_bak = p_pred;

    for (i = 0; i < 2; i++) {
        p_org = p_org_bak + i * 16;
        p_pred = p_pred_bak + i * 16;

        for (j = 0; j < 4; j++) {
            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M0 = _mm256_sub_epi16(R0, R1);
            M1 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);


            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M2 = _mm256_sub_epi16(R0, R1);
            M3 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);

            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M4 = _mm256_sub_epi16(R0, R1);
            M5 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);

            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M6 = _mm256_sub_epi16(R0, R1);
            M7 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);

            // vertical
            T0 = _mm256_add_epi16(M0, M4);
            T1 = _mm256_add_epi16(M1, M5);
            T2 = _mm256_add_epi16(M2, M6);
            T3 = _mm256_add_epi16(M3, M7);
            T4 = _mm256_sub_epi16(M0, M4);
            T5 = _mm256_sub_epi16(M1, M5);
            T6 = _mm256_sub_epi16(M2, M6);
            T7 = _mm256_sub_epi16(M3, M7);

            M0 = _mm256_add_epi16(T0, T2);
            M1 = _mm256_add_epi16(T1, T3);
            M2 = _mm256_sub_epi16(T0, T2);
            M3 = _mm256_sub_epi16(T1, T3);
            M4 = _mm256_add_epi16(T4, T6);
            M5 = _mm256_add_epi16(T5, T7);
            M6 = _mm256_sub_epi16(T4, T6);
            M7 = _mm256_sub_epi16(T5, T7);

            T0 = _mm256_add_epi16(M0, M1);
            T7 = _mm256_sub_epi16(M0, M1);
            T3 = _mm256_add_epi16(M2, M3);
            T4 = _mm256_sub_epi16(M2, M3);
            T1 = _mm256_add_epi16(M4, M5);
            T6 = _mm256_sub_epi16(M4, M5);
            T2 = _mm256_add_epi16(M6, M7);
            T5 = _mm256_sub_epi16(M6, M7);

            //transpose
            R0 = _mm256_unpacklo_epi16(T0, T1);
            R1 = _mm256_unpacklo_epi16(T2, T3);
            R2 = _mm256_unpacklo_epi16(T4, T5);
            R3 = _mm256_unpacklo_epi16(T6, T7);

            M4 = _mm256_unpacklo_epi32(R0, R1);
            M5 = _mm256_unpackhi_epi32(R0, R1);
            M6 = _mm256_unpacklo_epi32(R2, R3);
            M7 = _mm256_unpackhi_epi32(R2, R3);

            M0 = _mm256_unpacklo_epi64(M4, M6);
            M1 = _mm256_unpackhi_epi64(M4, M6);
            M2 = _mm256_unpacklo_epi64(M5, M7);
            M3 = _mm256_unpackhi_epi64(M5, M7);

            R0 = _mm256_unpackhi_epi16(T0, T1);
            R1 = _mm256_unpackhi_epi16(T2, T3);
            R2 = _mm256_unpackhi_epi16(T4, T5);
            R3 = _mm256_unpackhi_epi16(T6, T7);

            T4 = _mm256_unpacklo_epi32(R0, R1);
            T5 = _mm256_unpackhi_epi32(R0, R1);
            T6 = _mm256_unpacklo_epi32(R2, R3);
            T7 = _mm256_unpackhi_epi32(R2, R3);

            M4 = _mm256_unpacklo_epi64(T4, T6);
            M5 = _mm256_unpackhi_epi64(T4, T6);
            M6 = _mm256_unpacklo_epi64(T5, T7);
            M7 = _mm256_unpackhi_epi64(T5, T7);

            // vertical
            T0 = _mm256_add_epi16(M0, M4);
            T1 = _mm256_add_epi16(M1, M5);
            T2 = _mm256_add_epi16(M2, M6);
            T3 = _mm256_add_epi16(M3, M7);
            T4 = _mm256_sub_epi16(M0, M4);
            T5 = _mm256_sub_epi16(M1, M5);
            T6 = _mm256_sub_epi16(M2, M6);
            T7 = _mm256_sub_epi16(M3, M7);

            M0 = _mm256_add_epi16(T0, T2);
            M1 = _mm256_add_epi16(T1, T3);
            M2 = _mm256_sub_epi16(T0, T2);
            M3 = _mm256_sub_epi16(T1, T3);
            M4 = _mm256_add_epi16(T4, T6);
            M5 = _mm256_add_epi16(T5, T7);
            M6 = _mm256_sub_epi16(T4, T6);
            M7 = _mm256_sub_epi16(T5, T7);

            T0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M0));
            T1 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M0, 1));
            T2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M1));
            T3 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M1, 1));
            T4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M2));
            T5 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M2, 1));
            T6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M3));
            T7 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M3, 1));

            S0 = _mm256_abs_epi32(_mm256_add_epi32(T0, T2));
            S7 = _mm256_abs_epi32(_mm256_sub_epi32(T0, T2));
            S3 = _mm256_abs_epi32(_mm256_add_epi32(T1, T3));
            S4 = _mm256_abs_epi32(_mm256_sub_epi32(T1, T3));
            S1 = _mm256_abs_epi32(_mm256_add_epi32(T4, T6));
            S6 = _mm256_abs_epi32(_mm256_sub_epi32(T4, T6));
            S2 = _mm256_abs_epi32(_mm256_add_epi32(T5, T7));
            S5 = _mm256_abs_epi32(_mm256_sub_epi32(T5, T7));

            T0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M4));
            T1 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M4, 1));
            T2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M5));
            T3 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M5, 1));
            T4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M6));
            T5 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M6, 1));
            T6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M7));
            T7 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M7, 1));

            M0 = _mm256_abs_epi32(_mm256_add_epi32(T0, T2));
            M7 = _mm256_abs_epi32(_mm256_sub_epi32(T0, T2));
            M3 = _mm256_abs_epi32(_mm256_add_epi32(T1, T3));
            M4 = _mm256_abs_epi32(_mm256_sub_epi32(T1, T3));
            M1 = _mm256_abs_epi32(_mm256_add_epi32(T4, T6));
            M6 = _mm256_abs_epi32(_mm256_sub_epi32(T4, T6));
            M2 = _mm256_abs_epi32(_mm256_add_epi32(T5, T7));
            M5 = _mm256_abs_epi32(_mm256_sub_epi32(T5, T7));

            T0 = _mm256_add_epi32(M0, S0);
            T1 = _mm256_add_epi32(M1, S1);
            T2 = _mm256_add_epi32(M2, S2);
            T3 = _mm256_add_epi32(M3, S3);
            T4 = _mm256_add_epi32(M4, S4);
            T5 = _mm256_add_epi32(M5, S5);
            T6 = _mm256_add_epi32(M6, S6);
            T7 = _mm256_add_epi32(M7, S7);

            T0 = _mm256_add_epi32(T0, T1);
            T2 = _mm256_add_epi32(T2, T3);
            T4 = _mm256_add_epi32(T4, T5);
            T6 = _mm256_add_epi32(T6, T7);

            T0 = _mm256_add_epi32(T0, T6);
            T2 = _mm256_add_epi32(T2, T4);

            sum = _mm256_hadd_epi32(T0, T2);

            sum = _mm256_hadd_epi32(sum, sum);
            uiSum += ((_mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4) + 2) >> 2) + ((_mm256_extract_epi32(sum, 1) + _mm256_extract_epi32(sum, 5) + 2) >> 2);
        }
    }

    return uiSum;
}

i32u_t xCalcHAD64x64_sse256_10bit(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int i, j, uiSum = 0;
    __m256i R0, R1, R2, R3;
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i S0, S1, S2, S3, S4, S5, S6, S7;
    __m256i sum;
    pel_t *p_org_bak = p_org;
    pel_t *p_pred_bak = p_pred;

    for (i = 0; i < 4; i++) {
        p_org = p_org_bak + i * 16;
        p_pred = p_pred_bak + i * 16;

        for (j = 0; j < 8; j++) {
            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M0 = _mm256_sub_epi16(R0, R1);
            M1 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);


            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M2 = _mm256_sub_epi16(R0, R1);
            M3 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);

            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M4 = _mm256_sub_epi16(R0, R1);
            M5 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);

            R0 = _mm256_loadu_si256((__m256i const*)p_org);
            R1 = _mm256_loadu_si256((__m256i const*)p_pred);
            R2 = _mm256_loadu_si256((__m256i const*)(p_org + i_org));
            R3 = _mm256_loadu_si256((__m256i const*)(p_pred + i_pred));
            M6 = _mm256_sub_epi16(R0, R1);
            M7 = _mm256_sub_epi16(R2, R3);
            p_pred += (i_pred << 1);
            p_org += (i_org << 1);

            // vertical
            T0 = _mm256_add_epi16(M0, M4);
            T1 = _mm256_add_epi16(M1, M5);
            T2 = _mm256_add_epi16(M2, M6);
            T3 = _mm256_add_epi16(M3, M7);
            T4 = _mm256_sub_epi16(M0, M4);
            T5 = _mm256_sub_epi16(M1, M5);
            T6 = _mm256_sub_epi16(M2, M6);
            T7 = _mm256_sub_epi16(M3, M7);

            M0 = _mm256_add_epi16(T0, T2);
            M1 = _mm256_add_epi16(T1, T3);
            M2 = _mm256_sub_epi16(T0, T2);
            M3 = _mm256_sub_epi16(T1, T3);
            M4 = _mm256_add_epi16(T4, T6);
            M5 = _mm256_add_epi16(T5, T7);
            M6 = _mm256_sub_epi16(T4, T6);
            M7 = _mm256_sub_epi16(T5, T7);

            T0 = _mm256_add_epi16(M0, M1);
            T7 = _mm256_sub_epi16(M0, M1);
            T3 = _mm256_add_epi16(M2, M3);
            T4 = _mm256_sub_epi16(M2, M3);
            T1 = _mm256_add_epi16(M4, M5);
            T6 = _mm256_sub_epi16(M4, M5);
            T2 = _mm256_add_epi16(M6, M7);
            T5 = _mm256_sub_epi16(M6, M7);

            //transpose
            R0 = _mm256_unpacklo_epi16(T0, T1);
            R1 = _mm256_unpacklo_epi16(T2, T3);
            R2 = _mm256_unpacklo_epi16(T4, T5);
            R3 = _mm256_unpacklo_epi16(T6, T7);

            M4 = _mm256_unpacklo_epi32(R0, R1);
            M5 = _mm256_unpackhi_epi32(R0, R1);
            M6 = _mm256_unpacklo_epi32(R2, R3);
            M7 = _mm256_unpackhi_epi32(R2, R3);

            M0 = _mm256_unpacklo_epi64(M4, M6);
            M1 = _mm256_unpackhi_epi64(M4, M6);
            M2 = _mm256_unpacklo_epi64(M5, M7);
            M3 = _mm256_unpackhi_epi64(M5, M7);

            R0 = _mm256_unpackhi_epi16(T0, T1);
            R1 = _mm256_unpackhi_epi16(T2, T3);
            R2 = _mm256_unpackhi_epi16(T4, T5);
            R3 = _mm256_unpackhi_epi16(T6, T7);

            T4 = _mm256_unpacklo_epi32(R0, R1);
            T5 = _mm256_unpackhi_epi32(R0, R1);
            T6 = _mm256_unpacklo_epi32(R2, R3);
            T7 = _mm256_unpackhi_epi32(R2, R3);

            M4 = _mm256_unpacklo_epi64(T4, T6);
            M5 = _mm256_unpackhi_epi64(T4, T6);
            M6 = _mm256_unpacklo_epi64(T5, T7);
            M7 = _mm256_unpackhi_epi64(T5, T7);

            // vertical
            T0 = _mm256_add_epi16(M0, M4);
            T1 = _mm256_add_epi16(M1, M5);
            T2 = _mm256_add_epi16(M2, M6);
            T3 = _mm256_add_epi16(M3, M7);
            T4 = _mm256_sub_epi16(M0, M4);
            T5 = _mm256_sub_epi16(M1, M5);
            T6 = _mm256_sub_epi16(M2, M6);
            T7 = _mm256_sub_epi16(M3, M7);

            M0 = _mm256_add_epi16(T0, T2);
            M1 = _mm256_add_epi16(T1, T3);
            M2 = _mm256_sub_epi16(T0, T2);
            M3 = _mm256_sub_epi16(T1, T3);
            M4 = _mm256_add_epi16(T4, T6);
            M5 = _mm256_add_epi16(T5, T7);
            M6 = _mm256_sub_epi16(T4, T6);
            M7 = _mm256_sub_epi16(T5, T7);

            T0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M0));
            T1 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M0, 1));
            T2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M1));
            T3 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M1, 1));
            T4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M2));
            T5 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M2, 1));
            T6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M3));
            T7 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M3, 1));

            S0 = _mm256_abs_epi32(_mm256_add_epi32(T0, T2));
            S7 = _mm256_abs_epi32(_mm256_sub_epi32(T0, T2));
            S3 = _mm256_abs_epi32(_mm256_add_epi32(T1, T3));
            S4 = _mm256_abs_epi32(_mm256_sub_epi32(T1, T3));
            S1 = _mm256_abs_epi32(_mm256_add_epi32(T4, T6));
            S6 = _mm256_abs_epi32(_mm256_sub_epi32(T4, T6));
            S2 = _mm256_abs_epi32(_mm256_add_epi32(T5, T7));
            S5 = _mm256_abs_epi32(_mm256_sub_epi32(T5, T7));

            T0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M4));
            T1 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M4, 1));
            T2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M5));
            T3 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M5, 1));
            T4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M6));
            T5 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M6, 1));
            T6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(M7));
            T7 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(M7, 1));

            M0 = _mm256_abs_epi32(_mm256_add_epi32(T0, T2));
            M7 = _mm256_abs_epi32(_mm256_sub_epi32(T0, T2));
            M3 = _mm256_abs_epi32(_mm256_add_epi32(T1, T3));
            M4 = _mm256_abs_epi32(_mm256_sub_epi32(T1, T3));
            M1 = _mm256_abs_epi32(_mm256_add_epi32(T4, T6));
            M6 = _mm256_abs_epi32(_mm256_sub_epi32(T4, T6));
            M2 = _mm256_abs_epi32(_mm256_add_epi32(T5, T7));
            M5 = _mm256_abs_epi32(_mm256_sub_epi32(T5, T7));

            T0 = _mm256_add_epi32(M0, S0);
            T1 = _mm256_add_epi32(M1, S1);
            T2 = _mm256_add_epi32(M2, S2);
            T3 = _mm256_add_epi32(M3, S3);
            T4 = _mm256_add_epi32(M4, S4);
            T5 = _mm256_add_epi32(M5, S5);
            T6 = _mm256_add_epi32(M6, S6);
            T7 = _mm256_add_epi32(M7, S7);

            T0 = _mm256_add_epi32(T0, T1);
            T2 = _mm256_add_epi32(T2, T3);
            T4 = _mm256_add_epi32(T4, T5);
            T6 = _mm256_add_epi32(T6, T7);

            T0 = _mm256_add_epi32(T0, T6);
            T2 = _mm256_add_epi32(T2, T4);

            sum = _mm256_hadd_epi32(T0, T2);

            sum = _mm256_hadd_epi32(sum, sum);
            uiSum += ((_mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4) + 2) >> 2) + ((_mm256_extract_epi32(sum, 1) + _mm256_extract_epi32(sum, 5) + 2) >> 2);
        }
    }

    return uiSum;
}


// --------------------------------------------------------------------------------------------------------------------
// block pixel's var
// --------------------------------------------------------------------------------------------------------------------s
i32u_t pixel_var_8_sse128(const pel_t* pix, int i_pix)
{
    i32u_t sum = 0, sqr = 0;
    __m128i SUM = _mm_setzero_si128();
    __m128i SQR = _mm_setzero_si128();
    __m128i zero = _mm_setzero_si128();
    int y;

    for (y = 0; y < 8; y++) {
        __m128i T = _mm_loadl_epi64((const __m128i*)pix);
        T = _mm_unpacklo_epi8(T, zero);
        SUM = _mm_add_epi16(SUM, T);
        T = _mm_madd_epi16(T, T);
        SQR = _mm_add_epi32(SQR, T);
        pix += i_pix;
    }

    SUM = _mm_hadd_epi16(SUM, SUM);
    SUM = _mm_hadd_epi16(SUM, SUM);
    SUM = _mm_hadd_epi16(SUM, SUM);
    sum = _mm_extract_epi16(SUM, 0);

    SQR = _mm_hadd_epi32(SQR, SQR);
    SQR = _mm_hadd_epi32(SQR, SQR);
    sqr = _mm_extract_epi32(SQR, 0);

    return sqr - ((i64s_t)sum * sum >> 6);
}

i32u_t pixel_var_16_sse128(const pel_t* pix, int i_pix)
{
    i32u_t sum = 0, sqr = 0;
    __m128i SUM = _mm_setzero_si128();
    __m128i SQR = _mm_setzero_si128();
    __m128i zero = _mm_setzero_si128();
    int y;

    for (y = 0; y < 16; y++) {
        __m128i T1, T2;
        __m128i T = _mm_loadu_si128((const __m128i*)pix);
        SUM = _mm_add_epi16(SUM, _mm_sad_epu8(T, zero));
        T1 = _mm_unpacklo_epi8(T, zero);
        T2 = _mm_unpackhi_epi8(T, zero);
        T = _mm_add_epi32(_mm_madd_epi16(T1, T1), _mm_madd_epi16(T2, T2));
        SQR = _mm_add_epi32(SQR, T);
        pix += i_pix;
    }

    SUM = _mm_hadd_epi16(SUM, SUM);
    SUM = _mm_hadd_epi16(SUM, SUM);
    SUM = _mm_hadd_epi16(SUM, SUM);
    sum = _mm_extract_epi16(SUM, 0);

    SQR = _mm_hadd_epi32(SQR, SQR);
    SQR = _mm_hadd_epi32(SQR, SQR);
    sqr = _mm_extract_epi32(SQR, 0);

    return sqr - ((i64s_t)sum * sum >> 8);
}
