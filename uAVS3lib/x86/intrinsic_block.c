#include "intrinsic.h"
#include "../block.h"

int quant_normal_sse128(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift)
{
	int i;

	__m128i temp, ntemp, val0, valhi, vallo, val1, val2, coef;
	__m128i q = _mm_set1_epi16((short)Q);
	__m128i offset = _mm_set1_epi32(qp_const);
	__m128i zero = _mm_setzero_si128();
	__m128i mask;
	__m128i sum = _mm_setzero_si128();

	for (i = 0; i < coef_num; i += 8) {
		val0  = _mm_load_si128((__m128i *)(curr_blk + i));
        mask  = _mm_cmplt_epi16(val0, zero);
        vallo = _mm_mullo_epi16(val0, q);
        valhi = _mm_mulhi_epi16(val0, q);
		val1 = _mm_unpacklo_epi16(vallo, valhi);
		val2 = _mm_unpackhi_epi16(vallo, valhi);
        val1 = _mm_abs_epi32(val1);
        val2 = _mm_abs_epi32(val2);
		val1 = _mm_add_epi32(val1, offset);
		val2 = _mm_add_epi32(val2, offset);
		val1 = _mm_srli_epi32(val1, shift);
		val2 = _mm_srli_epi32(val2, shift);
		temp = _mm_packs_epi32(val1, val2);
		sum = _mm_add_epi32(sum, val1);
		ntemp = _mm_sub_epi16(zero, temp);
		sum = _mm_add_epi32(sum, val2);
		coef = _mm_blendv_epi8(temp, ntemp, mask);
		_mm_store_si128((__m128i *)(curr_blk + i), coef);
	}
    return !_mm_testz_si128(sum, _mm_set1_epi16(-1));
}

int quant_normal_sse256(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift)
{
    int i;

    __m256i temp, ntemp, val0, valhi, vallo, val1, val2, coef;
    __m256i q = _mm256_set1_epi16((short)Q);
    __m256i offset = _mm256_set1_epi32(qp_const);
    __m256i zero = _mm256_setzero_si256();
    __m256i mask;
    __m256i sum = _mm256_setzero_si256();

    for (i = 0; i < coef_num; i += 16) {
        val0 = _mm256_load_si256((__m256i *)(curr_blk + i));
        mask = _mm256_cmpgt_epi16(zero, val0);
        vallo = _mm256_mullo_epi16(val0, q);
        valhi = _mm256_mulhi_epi16(val0, q);
        val1 = _mm256_unpacklo_epi16(vallo, valhi);
        val2 = _mm256_unpackhi_epi16(vallo, valhi);
        val1 = _mm256_abs_epi32(val1);
        val2 = _mm256_abs_epi32(val2);
        val1 = _mm256_add_epi32(val1, offset);
        val2 = _mm256_add_epi32(val2, offset);
        val1 = _mm256_srli_epi32(val1, shift);
        val2 = _mm256_srli_epi32(val2, shift);
        temp = _mm256_packs_epi32(val1, val2);
        sum = _mm256_add_epi32(sum, val1);
        ntemp = _mm256_sub_epi16(zero, temp);
        sum = _mm256_add_epi32(sum, val2);
        coef = _mm256_blendv_epi8(temp, ntemp, mask);
        _mm256_store_si256((__m256i *)(curr_blk + i), coef);
    }
    return !_mm256_testz_si256(sum, _mm256_set1_epi16(-1));
}

int quant_ext_sse128(coef_t *curr_blk, int size, int Q, int qp_const, int shift)
{
    int i, j;
    int half_size = size / 2;
    __m128i temp, ntemp, val0, valhi, vallo, val1, val2, coef;
    __m128i q = _mm_set1_epi16((short)Q);
    __m128i offset = _mm_set1_epi32(qp_const);
    __m128i zero = _mm_setzero_si128();
    __m128i mask;
    __m128i sum = _mm_setzero_si128();

    for (i = 0; i < half_size; i++) {
        for (j = 0; j < half_size; j += 8) {
            val0 = _mm_loadu_si128((__m128i *)(curr_blk + j));
            mask = _mm_cmplt_epi16(val0, zero);
            vallo = _mm_mullo_epi16(val0, q);
            valhi = _mm_mulhi_epi16(val0, q);
            val1 = _mm_unpacklo_epi16(vallo, valhi);
            val2 = _mm_unpackhi_epi16(vallo, valhi);
            val1 = _mm_abs_epi32(val1);
            val2 = _mm_abs_epi32(val2);
            val1 = _mm_add_epi32(val1, offset);
            val2 = _mm_add_epi32(val2, offset);
            val1 = _mm_srli_epi32(val1, shift);
            val2 = _mm_srli_epi32(val2, shift);
            temp = _mm_packs_epi32(val1, val2);
            sum = _mm_add_epi32(sum, val1);
            ntemp = _mm_sub_epi16(zero, temp);
            sum = _mm_add_epi32(sum, val2);
            coef = _mm_blendv_epi8(temp, ntemp, mask);
            _mm_store_si128((__m128i *)(curr_blk + j), coef);
        }
        curr_blk += size;
    }

    return !_mm_testz_si128(sum, _mm_set1_epi16(-1));
}

int quant_ext_sse256(coef_t *curr_blk, int size, int Q, int qp_const, int shift)
{
    int i;
    __m256i temp, ntemp, val0, valhi, vallo, val1, val2, coef;
    __m256i q = _mm256_set1_epi16((short)Q);
    __m256i offset = _mm256_set1_epi32(qp_const);
    __m256i zero = _mm256_setzero_si256();
    __m256i mask;
    __m256i sum = _mm256_setzero_si256();


    for (i = 0; i < 16; i++) {
        val0 = _mm256_loadu_si256((__m256i *)(curr_blk));
        mask = _mm256_cmpgt_epi16(zero, val0);
        vallo = _mm256_mullo_epi16(val0, q);
        valhi = _mm256_mulhi_epi16(val0, q);
        val1 = _mm256_unpacklo_epi16(vallo, valhi);
        val2 = _mm256_unpackhi_epi16(vallo, valhi);
        val1 = _mm256_abs_epi32(val1);
        val2 = _mm256_abs_epi32(val2);
        val1 = _mm256_add_epi32(val1, offset);
        val2 = _mm256_add_epi32(val2, offset);
        val1 = _mm256_srli_epi32(val1, shift);
        val2 = _mm256_srli_epi32(val2, shift);
        temp = _mm256_packs_epi32(val1, val2);
        sum = _mm256_add_epi32(sum, val1);
        ntemp = _mm256_sub_epi16(zero, temp);
        sum = _mm256_add_epi32(sum, val2);
        coef = _mm256_blendv_epi8(temp, ntemp, mask);
        _mm256_store_si256((__m256i *)(curr_blk), coef);
     
        curr_blk += size;
    }

    return !_mm256_testz_si256(sum, _mm256_set1_epi16(-1));
}

void inv_quant_normal_sse128(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift)
{
    int i;

    __m128i S1, S2, D1, D2;
    __m128i T1 = _mm_set1_epi32(QPI);
    __m128i T2 = _mm_set1_epi32(1 << (shift - 1));
    __m128i ZERO = _mm_setzero_si128();
    __m128i mask_all  = _mm_set1_epi16(-1);
    __m128i mask_low  = _mm_set_epi32(-1, -1,  0,  0);
    __m128i mask_high = _mm_set_epi32( 0,  0, -1, -1);

    for (i = 0; i < coef_num; i += 8) {
        S1 = _mm_load_si128((const __m128i*)(src + i));

        if (!_mm_testz_si128(S1, mask_all)) {
            S2 = _mm_srli_si128(S1, 8);
            S1 = _mm_cvtepi16_epi32(S1);
            D1 = _mm_mullo_epi32(S1, T1);
            D1 = _mm_add_epi32(D1, T2);
            D1 = _mm_srai_epi32(D1, shift);

            S2 = _mm_cvtepi16_epi32(S2);
            D2 = _mm_mullo_epi32(S2, T1);
            D2 = _mm_add_epi32(D2, T2);
            D2 = _mm_srai_epi32(D2, shift);

            D1 = _mm_packs_epi32(D1, D2);
            _mm_store_si128((__m128i*)(dst + i), D1);
        }
        else if (!_mm_testz_si128(S1, mask_low)) {
            S1 = _mm_cvtepi16_epi32(S1);
            D1 = _mm_mullo_epi32(S1, T1);
            D1 = _mm_add_epi32(D1, T2);
            D1 = _mm_srai_epi32(D1, shift);
            D1 = _mm_packs_epi32(D1, D1);
            _mm_storel_epi64((__m128i*)(dst + i    ),   D1);
            _mm_storel_epi64((__m128i*)(dst + i + 4), ZERO);
        }
        else if (!_mm_testz_si128(S1, mask_high)) {
            S2 = _mm_srli_si128(S1, 8);
            S2 = _mm_cvtepi16_epi32(S2);
            D2 = _mm_mullo_epi32(S2, T1);
            D2 = _mm_add_epi32(D2, T2);
            D2 = _mm_srai_epi32(D2, shift);

            D2 = _mm_packs_epi32(D2, D2);
            _mm_storel_epi64((__m128i*)(dst + i    ), ZERO);
            _mm_storel_epi64((__m128i*)(dst + i + 4),   D2);
        }
        else {
            _mm_store_si128((__m128i*)(dst + i), ZERO);
        }
    } 
}

void inv_quant_normal_sse256(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift)
{
    int i;
    __m256i S1, D1;
    __m256i T1 = _mm256_set1_epi32(QPI);
    __m256i T2 = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < coef_num; i += 8) {
        S1 = _mm256_cvtepi16_epi32(_mm_load_si128((const __m128i*)(src + i)));
        D1 = _mm256_mullo_epi32(S1, T1);
        D1 = _mm256_add_epi32(D1, T2);
        D1 = _mm256_srai_epi32(D1, shift);
        D1 = _mm256_packs_epi32(D1, D1);
        _mm_storel_epi64((__m128i*)(dst + i    ), _mm256_castsi256_si128(D1));
        _mm_storel_epi64((__m128i*)(dst + i + 4), _mm256_extracti128_si256(D1, 1));
    }
}

void inv_quant_ext_sse128(coef_t *src, coef_t *dst, int size, int QPI, int shift)
{
    int i, j;

    __m128i S1, S2, D1, D2;
    __m128i T1 = _mm_set1_epi32(QPI);
    __m128i T2 = _mm_set1_epi32(1 << (shift - 1));
    __m128i ZERO = _mm_setzero_si128();
    __m128i mask_all = _mm_set1_epi16(-1);
    __m128i mask_low = _mm_set_epi32(-1, -1, 0, 0);
    __m128i mask_high = _mm_set_epi32(0, 0, -1, -1);

    for (j = 0; j < 16; j++) {
        for (i = 0; i < 16; i += 8) {
            S1 = _mm_loadu_si128((const __m128i*)(src + i));

            if (!_mm_testz_si128(S1, mask_all)) {
                S2 = _mm_srli_si128(S1, 8);
                S1 = _mm_cvtepi16_epi32(S1);
                D1 = _mm_mullo_epi32(S1, T1);
                D1 = _mm_add_epi32(D1, T2);
                D1 = _mm_srai_epi32(D1, shift);

                S2 = _mm_cvtepi16_epi32(S2);
                D2 = _mm_mullo_epi32(S2, T1);
                D2 = _mm_add_epi32(D2, T2);
                D2 = _mm_srai_epi32(D2, shift);

                D1 = _mm_packs_epi32(D1, D2);
                _mm_store_si128((__m128i*)(dst + i), D1);
            }
            else if (!_mm_testz_si128(S1, mask_low)) {
                S1 = _mm_cvtepi16_epi32(S1);
                D1 = _mm_mullo_epi32(S1, T1);
                D1 = _mm_add_epi32(D1, T2);
                D1 = _mm_srai_epi32(D1, shift);
                D1 = _mm_packs_epi32(D1, D1);
                _mm_storel_epi64((__m128i*)(dst + i), D1);
                _mm_storel_epi64((__m128i*)(dst + i + 4), ZERO);
            }
            else if (!_mm_testz_si128(S1, mask_high)) {
                S2 = _mm_srli_si128(S1, 8);
                S2 = _mm_cvtepi16_epi32(S2);
                D2 = _mm_mullo_epi32(S2, T1);
                D2 = _mm_add_epi32(D2, T2);
                D2 = _mm_srai_epi32(D2, shift);

                D2 = _mm_packs_epi32(D2, D2);
                _mm_storel_epi64((__m128i*)(dst + i), ZERO);
                _mm_storel_epi64((__m128i*)(dst + i + 4), D2);
            }
            else {
                _mm_store_si128((__m128i*)(dst + i), ZERO);
            }
        }
        for (; i < 32; i += 8) {
            _mm_store_si128((__m128i*)(dst + i), ZERO);
        }
        dst += 32;
        src += 32;
    }

    for (i = 0; i < 512; i += 8) {
        _mm_store_si128((__m128i*)(dst + i), ZERO);
    }
}

int add_sign_sse128(coef_t *dat, i16u_t *abs_val, int len)
{
    int i;

    __m128i T1, T2, Mask, D;
    __m128i zero = _mm_setzero_si128();
    __m128i NZ = zero;

    for (i = 0; i < len; i += 8) {
        T1 = _mm_load_si128((const __m128i*)(abs_val + i));
        Mask = _mm_load_si128((const __m128i*)(dat + i));
        NZ = _mm_or_si128(T1, NZ);
        T2 = _mm_sub_epi16(zero, T1);
        Mask = _mm_cmpgt_epi16(Mask, zero);
        D = _mm_blendv_epi8(T2, T1, Mask);
        _mm_store_si128((__m128i*)(dat + i), D);
    }

    return !_mm_testz_si128(NZ, _mm_set1_epi16(-1));
}

int add_sign_sse256(coef_t *dat, i16u_t *abs_val, int len)
{
    int i;

    __m256i T1, T2, Mask, D;
    __m256i zero = _mm256_setzero_si256();
    __m256i NZ = zero;

    for (i = 0; i < len; i += 16) {
        T1 = _mm256_load_si256((const __m256i*)(abs_val + i));
        Mask = _mm256_load_si256((const __m256i*)(dat + i));
        NZ = _mm256_or_si256(T1, NZ);
        T2 = _mm256_sub_epi16(zero, T1);
        Mask = _mm256_cmpgt_epi16(Mask, zero);
        D = _mm256_blendv_epi8(T2, T1, Mask);
        _mm256_store_si256((__m256i*)(dat + i), D);
    }

    return !_mm256_testz_si256(NZ, _mm256_set1_epi16(-1));
}

int pre_quant_sse128(coef_t *curr_blk, i16u_t *abs_blk, int len, int Q_threshold)
{
    int i;

    __m128i T, ABS, ABS_SUB1, Mask;
    __m128i NZ = _mm_setzero_si128();
    __m128i THR = _mm_set1_epi16((short)Q_threshold);
    __m128i ONE = _mm_set1_epi16(1);

    for (i = 0; i < len; i += 8) {
        T = _mm_load_si128((const __m128i*)(curr_blk + i));
        ABS = _mm_abs_epi16(T);
        ABS_SUB1 = _mm_subs_epu16(ABS, ONE);
        Mask = _mm_cmpgt_epi16(ABS_SUB1, THR);
        ABS = _mm_and_si128(ABS, Mask);
        _mm_store_si128((__m128i*)(abs_blk + i), ABS);
        NZ = _mm_or_si128(Mask, NZ);
    }

    return !_mm_testz_si128(NZ, _mm_set1_epi16(-1));
}

int pre_quant_sse256(coef_t *curr_blk, i16u_t *abs_blk, int len, int Q_threshold)
{
    int i;

    __m256i T, ABS, ABS_SUB1, Mask;
    __m256i NZ = _mm256_setzero_si256();
    __m256i THR = _mm256_set1_epi16((short)Q_threshold);
    __m256i ONE = _mm256_set1_epi16(1);

    for (i = 0; i < len; i += 16) {
        T = _mm256_loadu_si256((const __m256i*)(curr_blk + i));
        ABS = _mm256_abs_epi16(T);
        ABS_SUB1 = _mm256_subs_epu16(ABS, ONE);
        Mask = _mm256_cmpgt_epi16(ABS_SUB1, THR);
        ABS = _mm256_and_si256(ABS, Mask);
        _mm256_store_si256((__m256i*)(abs_blk + i), ABS);
        NZ = _mm256_or_si256(Mask, NZ);
    }

    return !_mm256_testz_si256(NZ, _mm256_set1_epi16(-1));
}

int get_cg_bits_sse128(coef_t *coef)
{
    __m128i sum = _mm_set1_epi16(2);
    __m128i zero = _mm_setzero_si128();
    __m128i T1 = _mm_load_si128((__m128i*)coef);
    __m128i T2 = _mm_load_si128((__m128i*)(coef + 8));
    __m128i T3, T4, T5, T6;

    T5 = _mm_abs_epi16(T1);
    T6 = _mm_abs_epi16(T2);
    T3 = _mm_cmpeq_epi16(T1, zero);
    T4 = _mm_cmpeq_epi16(T2, zero);
    T1 = _mm_add_epi16(T5, T6);
    T3 = _mm_add_epi16(T3, T4);
    sum = _mm_add_epi16(sum, T1);
    sum = _mm_add_epi16(sum, T3);

    sum = _mm_hadd_epi16(sum, sum);
    sum = _mm_hadd_epi16(sum, sum);
    sum = _mm_hadd_epi16(sum, sum);

    return _mm_extract_epi16(sum, 0);
}

void zero_cg_check8_sse128(coef_t *coef, i64u_t *cg_mask)
{
    __m128i T1, T2, T3, T4, T5, T6, T7, T8;

    T1 = _mm_load_si128((__m128i*)(coef));
    T2 = _mm_load_si128((__m128i*)(coef + 8));
    T3 = _mm_load_si128((__m128i*)(coef + 16));
    T4 = _mm_load_si128((__m128i*)(coef + 24));
    T5 = _mm_load_si128((__m128i*)(coef + 32));
    T6 = _mm_load_si128((__m128i*)(coef + 40));
    T7 = _mm_load_si128((__m128i*)(coef + 48));
    T8 = _mm_load_si128((__m128i*)(coef + 56));

    T1 = _mm_or_si128(T1, T2);
    T3 = _mm_or_si128(T3, T4);
    T5 = _mm_or_si128(T5, T6);
    T7 = _mm_or_si128(T7, T8);

    T1 = _mm_or_si128(T1, T3);
    T5 = _mm_or_si128(T5, T7);

    _mm_store_si128((__m128i*)(cg_mask), T1);
    _mm_store_si128((__m128i*)(cg_mask + 2), T5);
}

void zero_cg_check16_sse256(coef_t *coef, i64u_t *cg_mask)
{
    __m256i T1, T2, T3, T4, T5, T6, T7, T8;

    T1 = _mm256_load_si256((__m256i*)(coef));
    T2 = _mm256_load_si256((__m256i*)(coef + 16));
    T3 = _mm256_load_si256((__m256i*)(coef + 32));
    T4 = _mm256_load_si256((__m256i*)(coef + 48));
    T5 = _mm256_load_si256((__m256i*)(coef + 64));
    T6 = _mm256_load_si256((__m256i*)(coef + 80));
    T7 = _mm256_load_si256((__m256i*)(coef + 96));
    T8 = _mm256_load_si256((__m256i*)(coef + 112));

    T1 = _mm256_or_si256(T1, T2);
    T3 = _mm256_or_si256(T3, T4);
    T5 = _mm256_or_si256(T5, T6);
    T7 = _mm256_or_si256(T7, T8);

    T1 = _mm256_or_si256(T1, T3);
    T5 = _mm256_or_si256(T5, T7);

    _mm256_store_si256((__m256i*)(cg_mask), T1);
    _mm256_store_si256((__m256i*)(cg_mask + 4), T5);

    coef += 128;
    cg_mask += 8;

    T1 = _mm256_load_si256((__m256i*)(coef));
    T2 = _mm256_load_si256((__m256i*)(coef + 16));
    T3 = _mm256_load_si256((__m256i*)(coef + 32));
    T4 = _mm256_load_si256((__m256i*)(coef + 48));
    T5 = _mm256_load_si256((__m256i*)(coef + 64));
    T6 = _mm256_load_si256((__m256i*)(coef + 80));
    T7 = _mm256_load_si256((__m256i*)(coef + 96));
    T8 = _mm256_load_si256((__m256i*)(coef + 112));

    T1 = _mm256_or_si256(T1, T2);
    T3 = _mm256_or_si256(T3, T4);
    T5 = _mm256_or_si256(T5, T6);
    T7 = _mm256_or_si256(T7, T8);

    T1 = _mm256_or_si256(T1, T3);
    T5 = _mm256_or_si256(T5, T7);

    _mm256_store_si256((__m256i*)(cg_mask), T1);
    _mm256_store_si256((__m256i*)(cg_mask + 4), T5);
}

void zero_cg_check32_sse256(coef_t *coef, i64u_t *cg_mask)
{
    int i;
    for (i = 0; i < 8; i++) {
        __m256i T1, T2, T3, T4, T5, T6, T7, T8;

        T1 = _mm256_load_si256((__m256i*)(coef));
        T2 = _mm256_load_si256((__m256i*)(coef + 16));
        T3 = _mm256_load_si256((__m256i*)(coef + 32));
        T4 = _mm256_load_si256((__m256i*)(coef + 48));
        T5 = _mm256_load_si256((__m256i*)(coef + 64));
        T6 = _mm256_load_si256((__m256i*)(coef + 80));
        T7 = _mm256_load_si256((__m256i*)(coef + 96));
        T8 = _mm256_load_si256((__m256i*)(coef + 112));

        T1 = _mm256_or_si256(T1, T3);
        T5 = _mm256_or_si256(T5, T7);
        T2 = _mm256_or_si256(T2, T4);
        T6 = _mm256_or_si256(T6, T8);

        T1 = _mm256_or_si256(T1, T5);
        T2 = _mm256_or_si256(T2, T6);

        _mm256_store_si256((__m256i*)(cg_mask), T1);
        _mm256_store_si256((__m256i*)(cg_mask + 4), T2);

        coef += 128;
        cg_mask += 8;
    }
}

int get_nz_num_sse256(coef_t *p_coef, int num_coeff)
{
    __m256i CNT = _mm256_setzero_si256();
    __m256i zero = _mm256_setzero_si256();

    for (int i = 0; i < num_coeff; i += 16) {
        __m256i T = _mm256_load_si256((const __m256i*)p_coef);
        T = _mm256_cmpeq_epi16(T, zero);
        CNT = _mm256_add_epi16(CNT, T);
        p_coef += 16;
    }

    CNT = _mm256_hadds_epi16(CNT, CNT);
    CNT = _mm256_hadds_epi16(CNT, CNT);
    CNT = _mm256_hadds_epi16(CNT, CNT);

    i16s_t num = _mm256_extract_epi16(CNT, 0);
    num += _mm256_extract_epi16(CNT, 8);

    return num_coeff + num;
}

int pre_quant_rdoq_sse256(i16s_t *coef, int num, int q_value, int q_bits, int err_scale, i32u_t* abs_coef, i32u_t* abs_level, long long *uncoded_err)
{
    int q_bits_add = 1 << (q_bits - 1);
    int threshold = ((1 << (q_bits - 1)) - 1) / q_value + 1;

    __m256i ADD     = _mm256_set1_epi32(q_bits_add);
    __m256i ES      = _mm256_set1_epi32(err_scale);
    __m256i THRD    = _mm256_set1_epi16(threshold - 1);
    __m256i Q       = _mm256_set1_epi32(q_value);
    __m256i MaskALL = _mm256_set1_epi32(-1);
    __m256i ZERO    = _mm256_setzero_si256();

    i16s_t *c    = coef;
    i32u_t *absc = abs_coef;
    i32u_t *absl = abs_level;
    i64s_t *uerr = uncoded_err;

    int last_nz;
    int last_nz_grp = -1;

    for (int i = 0; i < num; i += 16) {
        __m256i T = _mm256_abs_epi16(_mm256_load_si256((const __m256i*)c));
        __m256i M = _mm256_cmpgt_epi16(T, THRD);

        T = _mm256_and_si256(T, M);
        
        int allzero = _mm256_testz_si256(T, MaskALL);

        if (allzero) {
            _mm256_store_si256((__m256i*)(absl    ), ZERO);
            _mm256_store_si256((__m256i*)(absl + 8), ZERO);

            c    += 16;
            absc += 16;
            absl += 16;
            uerr += 16;
            continue;
        }
        last_nz_grp = i;

        __m256i S1 = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(T, 0));
        __m256i S2 = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(T, 1));

        S1 = _mm256_mullo_epi32(S1, Q);
        S2 = _mm256_mullo_epi32(S2, Q);
        _mm256_store_si256((__m256i*)(absc    ), S1);
        _mm256_store_si256((__m256i*)(absc + 8), S2);

        __m256i L1 = _mm256_srli_epi32(_mm256_add_epi32(S1, ADD), q_bits);
        __m256i L2 = _mm256_srli_epi32(_mm256_add_epi32(S2, ADD), q_bits);
        _mm256_store_si256((__m256i*)(absl    ), L1);
        _mm256_store_si256((__m256i*)(absl + 8), L2);

        __m256i E1 = _mm256_mul_epu32(S1, ES);
        __m256i E2 = _mm256_mul_epu32(_mm256_srli_epi64(S1, 32), ES);
        __m256i E3 = _mm256_mul_epu32(S2, ES);
        __m256i E4 = _mm256_mul_epu32(_mm256_srli_epi64(S2, 32), ES);

        E1 = _mm256_srli_epi64(E1, ERR_SCALE_PRECISION_BITS);
        E2 = _mm256_srli_epi64(E2, ERR_SCALE_PRECISION_BITS);
        E3 = _mm256_srli_epi64(E3, ERR_SCALE_PRECISION_BITS);
        E4 = _mm256_srli_epi64(E4, ERR_SCALE_PRECISION_BITS);

        E1 = _mm256_mul_epu32(E1, E1);
        E2 = _mm256_mul_epu32(E2, E2);
        E3 = _mm256_mul_epu32(E3, E3);
        E4 = _mm256_mul_epu32(E4, E4);

        __m256i T1 = _mm256_unpacklo_epi64(E1, E2);
        __m256i T2 = _mm256_unpackhi_epi64(E1, E2);
        __m256i T3 = _mm256_unpacklo_epi64(E3, E4);
        __m256i T4 = _mm256_unpackhi_epi64(E3, E4);

        _mm256_storeu2_m128i((__m128i*)(uerr +  4), (__m128i*)(uerr     ), T1);
        _mm256_storeu2_m128i((__m128i*)(uerr +  6), (__m128i*)(uerr +  2), T2);
        _mm256_storeu2_m128i((__m128i*)(uerr + 12), (__m128i*)(uerr +  8), T3);
        _mm256_storeu2_m128i((__m128i*)(uerr + 14), (__m128i*)(uerr + 10), T4);

        c    += 16;
        absc += 16;
        absl += 16;
        uerr += 16;
    }

    if (last_nz_grp >= 0) {
        last_nz = last_nz_grp;

        for (int i = 0; i < 16; i++) {
            if (abs_level[last_nz_grp + i]) {
                last_nz = last_nz_grp + i;
            }
        }
    } else {
        last_nz = -1;
    }

    return last_nz;
}
