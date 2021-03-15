#include "intrinsic.h"

#pragma warning(disable: 4101) // unreferenced local variable

void sub_trans_4x4_sec_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	__m128i in[4], O00, O01, O02, O03, P00, P01, P02, P03;
	__m128i zero = _mm_setzero_si128();
	__m128i tmpC0, tmpC1, tmpC2, tmpC3, factor;
	__m128i	tmpCoef0, tmpCoef1, tmpCoef2, tmpCoef3;
	__m128i tmpS0, tmpS1, tmpS2, tmpS3, tmpRes0, tmpRes1, tmpRes2, tmpRes3;
	__m128i tmpProduct0, tmpProduct1, tmpProduct2, tmpProduct3, tmpSum0, tmpSum1, tmpSum2, tmpSum3, tmpDst0, tmpDst1, tmpDst2, tmpDst3, tmpDst4;

	//load org data & pred data
	O00 = _mm_loadl_epi64((__m128i*)&org[0]); //low 32 bits is valid!
	O01 = _mm_loadl_epi64((__m128i*)&org[i_org]);
	O02 = _mm_loadl_epi64((__m128i*)&org[i_org << 1]);
	O03 = _mm_loadl_epi64((__m128i*)&org[3 * i_org]);
	O00 = _mm_unpacklo_epi8(O00, zero); //low 64 bits is valid!
	O01 = _mm_unpacklo_epi8(O01, zero);
	O02 = _mm_unpacklo_epi8(O02, zero);
	O03 = _mm_unpacklo_epi8(O03, zero);
	P00 = _mm_loadl_epi64((__m128i*)&pred[0]);
	P01 = _mm_loadl_epi64((__m128i*)&pred[i_pred]);
	P02 = _mm_loadl_epi64((__m128i*)&pred[i_pred << 1]);
	P03 = _mm_loadl_epi64((__m128i*)&pred[3 * i_pred]);
	P00 = _mm_unpacklo_epi8(P00, zero);
	P01 = _mm_unpacklo_epi8(P01, zero);
	P02 = _mm_unpacklo_epi8(P02, zero);
	P03 = _mm_unpacklo_epi8(P03, zero);
	in[0] = _mm_sub_epi16(O00, P00);
	in[1] = _mm_sub_epi16(O01, P01);
	in[2] = _mm_sub_epi16(O02, P02);
	in[3] = _mm_sub_epi16(O03, P03);

	/*---hor transform---*/
	// shift = 1; rnd_factor = 1
	factor = _mm_set1_epi32(1);

	//load tab_c4_trans data, a matrix of 4x4
	tmpCoef0 = _mm_set_epi16(0, 55, 0, 79, 0, 77, 0, 34);
	tmpCoef1 = _mm_set_epi16(0, -84, 0, -33, 0, 69, 0, 58);
	tmpCoef2 = _mm_set_epi16(0, 73, 0, -75, 0, -7, 0, 72);
	tmpCoef3 = _mm_set_epi16(0, -28, 0, 58, 0, -75, 0, 81);

	/*-------for j = 0-------*/
	//load src data
	tmpS0 = _mm_set1_epi16(_mm_extract_epi16(in[0], 0));							//tmpRes[0][0]
	tmpS1 = _mm_set1_epi16(_mm_extract_epi16(in[0], 1));							//tmpRes[0][1]
	tmpS2 = _mm_set1_epi16(_mm_extract_epi16(in[0], 2));
	tmpS3 = _mm_set1_epi16(_mm_extract_epi16(in[0], 3));
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 1), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 1-------*/
	//load src data
    tmpS0 = _mm_set1_epi16(_mm_extract_epi16(in[1], 0));							//tmpRes[1][0]
    tmpS1 = _mm_set1_epi16(_mm_extract_epi16(in[1], 1));							//tmpRes[1][1]
    tmpS2 = _mm_set1_epi16(_mm_extract_epi16(in[1], 2));
    tmpS3 = _mm_set1_epi16(_mm_extract_epi16(in[1], 3));
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 1), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 2-------*/
	//load src data
    tmpS0 = _mm_set1_epi16(_mm_extract_epi16(in[2], 0));							//tmpRes[2][0]
    tmpS1 = _mm_set1_epi16(_mm_extract_epi16(in[2], 1));							//tmpRes[2][1]
    tmpS2 = _mm_set1_epi16(_mm_extract_epi16(in[2], 2));
    tmpS3 = _mm_set1_epi16(_mm_extract_epi16(in[2], 3));
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 1), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 3-------*/
	//load src data
    tmpS0 = _mm_set1_epi16(_mm_extract_epi16(in[3], 0));							//tmpRes[3][0]
    tmpS1 = _mm_set1_epi16(_mm_extract_epi16(in[3], 1));							//tmpRes[3][1]
    tmpS2 = _mm_set1_epi16(_mm_extract_epi16(in[3], 2));
    tmpS3 = _mm_set1_epi16(_mm_extract_epi16(in[3], 3));
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 1), zero);				//!only low 64bits (4xSHORT) are valid!

	/*---vertical transform---*/

	//rnd_factor = 128; shift = 8
	factor = _mm_set1_epi32(128);
	tmpRes0 = _mm_unpacklo_epi16(tmpRes0, zero);
	tmpRes1 = _mm_unpacklo_epi16(tmpRes1, zero);
	tmpRes2 = _mm_unpacklo_epi16(tmpRes2, zero);
	tmpRes3 = _mm_unpacklo_epi16(tmpRes3, zero);

	//for i = 0
	//load coef data
	tmpC0 = _mm_set1_epi16(34);							//coef[0][0]
	tmpC1 = _mm_set1_epi16(58);							//coef[1][0]
	tmpC2 = _mm_set1_epi16(72);
	tmpC3 = _mm_set1_epi16(81);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpRes0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpRes1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpRes2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpRes3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);			
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 8), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 1
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(77);
	tmpC1 = _mm_set1_epi16(69);
	tmpC2 = _mm_set1_epi16(-7);
	tmpC3 = _mm_set1_epi16(-75);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpRes0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpRes1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpRes2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpRes3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 8), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 2
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(79);
	tmpC1 = _mm_set1_epi16(-33);
	tmpC2 = _mm_set1_epi16(-75);
	tmpC3 = _mm_set1_epi16(58);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpRes0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpRes1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpRes2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpRes3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 8), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 3
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(55);
	tmpC1 = _mm_set1_epi16(-84);
	tmpC2 = _mm_set1_epi16(73);
	tmpC3 = _mm_set1_epi16(-28);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpRes0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpRes1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpRes2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpRes3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 8), zero);				//!only low 64bits (4xSHORT) are valid!

	_mm_storel_epi64((__m128i*)&dst[0], tmpDst0);
	_mm_storel_epi64((__m128i*)&dst[4], tmpDst1);
	_mm_storel_epi64((__m128i*)&dst[8], tmpDst2);
	_mm_storel_epi64((__m128i*)&dst[12], tmpDst3);
}

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// dct 4x4 //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
#define pair_set_epi16(a, b) \
  _mm_set_epi16(b, a, b, a, b, a, b, a)

ALIGNED_32(static const coef_t tab_dct_4[][8]) =
{
	{ 32, 32, 32, 32, 32, 32, 32, 32 },
	{ 42, 17, -17, -42, 42, 17, -17, -42 },
	{ 32, -32, -32, 32, 32, -32, -32, 32, },
	{ 17, -42, 42, -17, 17, -42, 42, -17 }
};

void sub_trans_4x4_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	const __m128i k_p32_p32 = _mm_set1_epi16(32);
	const __m128i k_p32_m32 = pair_set_epi16(32, -32);
	const __m128i k_p17_p42 = pair_set_epi16(17, 42);
	const __m128i k_m42_p17 = pair_set_epi16(-42, 17);
	const __m128i k_ROUNDING2 = _mm_set1_epi32(64);
	__m128i in[4];
	__m128i tr0, tr1, tr00, tr01;
	__m128i r0, r1, r2, r3, t0, t2, u0, u1, u2, u3, v0, v1, v2, v3, w0, w1, w2, w3;
	__m128i res0, res1;
	__m128i tmpZero = _mm_setzero_si128();
	int shift2 = 7;
	__m128i O00, O01, O02, O03, P00, P01, P02, P03;

	//load org data & pred data
	O00 = _mm_loadl_epi64((__m128i*)&org[0 * i_org]); //low 32 bits is valid!
	O01 = _mm_loadl_epi64((__m128i*)&org[1 * i_org]);
	O02 = _mm_loadl_epi64((__m128i*)&org[2 * i_org]);
	O03 = _mm_loadl_epi64((__m128i*)&org[3 * i_org]);
	O00 = _mm_unpacklo_epi8(O00, tmpZero); //low 64 bits is valid!
	O01 = _mm_unpacklo_epi8(O01, tmpZero);
	O02 = _mm_unpacklo_epi8(O02, tmpZero);
	O03 = _mm_unpacklo_epi8(O03, tmpZero);
	P00 = _mm_loadl_epi64((__m128i*)&pred[0 * i_pred]);
	P01 = _mm_loadl_epi64((__m128i*)&pred[1 * i_pred]);
	P02 = _mm_loadl_epi64((__m128i*)&pred[2 * i_pred]);
	P03 = _mm_loadl_epi64((__m128i*)&pred[3 * i_pred]);
	P00 = _mm_unpacklo_epi8(P00, tmpZero); 
	P01 = _mm_unpacklo_epi8(P01, tmpZero);
	P02 = _mm_unpacklo_epi8(P02, tmpZero);
	P03 = _mm_unpacklo_epi8(P03, tmpZero);
	in[0] = _mm_sub_epi16(O00, P00);
	in[1] = _mm_sub_epi16(O01, P01);
	in[2] = _mm_sub_epi16(O02, P02);
	in[3] = _mm_sub_epi16(O03, P03);

	//transpose input data, low 64 bit is valid
	tr00 = _mm_unpacklo_epi64(in[0], in[2]);	//in[0] in[2]
	tr01 = _mm_unpacklo_epi64(in[1], in[3]);	//in[1] in[3]
	tr0 = _mm_unpacklo_epi16(tr00, tr01);
	tr1 = _mm_unpackhi_epi16(tr00, tr01);
	in[0] = _mm_unpacklo_epi32(tr0, tr1);
	in[2] = _mm_unpackhi_epi32(tr0, tr1);
	in[1] = _mm_unpackhi_epi64(in[0], in[0]);
	in[3] = _mm_unpackhi_epi64(in[2], in[2]);

	///// DCT1 //////
	//E[]&O[], low 64 bit is valid
	r0 = _mm_add_epi16(in[0], in[3]);	//E0
	r1 = _mm_add_epi16(in[1], in[2]);	//E1
	r2 = _mm_sub_epi16(in[1], in[2]);	//O1
	r3 = _mm_sub_epi16(in[0], in[3]);   //O0

	t0 = _mm_unpacklo_epi16(r0, r1);
	t2 = _mm_unpacklo_epi16(r2, r3);
	u0 = _mm_madd_epi16(t0, k_p32_p32);
	u2 = _mm_madd_epi16(t0, k_p32_m32);
	u1 = _mm_madd_epi16(t2, k_p17_p42);
	u3 = _mm_madd_epi16(t2, k_m42_p17);

	res0 = _mm_packs_epi32(u0, u1);	//out[0] out[1]
	res1 = _mm_packs_epi32(u2, u3);	//out[2] out[3]

	///// DCT2 //////
	r0 = _mm_madd_epi16(res0, _mm_load_si128((__m128i*)tab_dct_4[0]));
	r1 = _mm_madd_epi16(res1, _mm_load_si128((__m128i*)tab_dct_4[0]));
	u0 = _mm_hadd_epi32(r0, r1);

	r0 = _mm_madd_epi16(res0, _mm_load_si128((__m128i*)tab_dct_4[1]));
	r1 = _mm_madd_epi16(res1, _mm_load_si128((__m128i*)tab_dct_4[1]));
	u1 = _mm_hadd_epi32(r0, r1);

	r0 = _mm_madd_epi16(res0, _mm_load_si128((__m128i*)tab_dct_4[2]));
	r1 = _mm_madd_epi16(res1, _mm_load_si128((__m128i*)tab_dct_4[2]));
	u2 = _mm_hadd_epi32(r0, r1);

	r0 = _mm_madd_epi16(res0, _mm_load_si128((__m128i*)tab_dct_4[3]));
	r1 = _mm_madd_epi16(res1, _mm_load_si128((__m128i*)tab_dct_4[3]));
	u3 = _mm_hadd_epi32(r0, r1);

	//clip
	v0 = _mm_add_epi32(u0, k_ROUNDING2);
	v1 = _mm_add_epi32(u1, k_ROUNDING2);
	v2 = _mm_add_epi32(u2, k_ROUNDING2);
	v3 = _mm_add_epi32(u3, k_ROUNDING2);
	w0 = _mm_srai_epi32(v0, shift2);
	w1 = _mm_srai_epi32(v1, shift2);
	w2 = _mm_srai_epi32(v2, shift2);
	w3 = _mm_srai_epi32(v3, shift2);
	res0 = _mm_packs_epi32(w0, w1);
	res1 = _mm_packs_epi32(w2, w3);

	//store
	_mm_storeu_si128((__m128i *)(dst + 0 * 4), res0);
	_mm_storeu_si128((__m128i *)(dst + 2 * 4), res1);
}

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// dct 8x8 //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
ALIGNED_32(static const coef_t tab_dct_8_1[][8])=
{
	{ 32, 32, 32, 32, 32, 32, 32, 32 },
	{ 44, 38, 25, 9, -9, -25, -38, -44 },
	{ 42, 17, -17, -42, -42, -17, 17, 42 },
	{ 38, -9, -44, -25, 25, 44, 9, -38 },
	{ 32, -32, -32, 32, 32, -32, -32, 32 },
	{ 25, -44, 9, 38, -38, -9, 44, -25 },
	{ 17, -42, 42, -17, -17, 42, -42, 17 },
	{ 9, -25, 38, -44, 44, -38, 25, -9 }
};

void sub_trans_8x8_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	int shift1 = 1;
	int shift2 = 8;
	const __m128i k_ROUNDING1 = _mm_set1_epi32(1);
	const __m128i k_ROUNDING2 = _mm_set1_epi32(128);
	__m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
	__m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
	__m128i in0, in1, in2, in3, in4, in5, in6, in7;
	__m128i res0, res1, res2, res3, res4, res5, res6, res7;
	__m128i r0, r1, r2, r3, t0, t1, t2, t3;
	__m128i q0, q1, q2, q3, q4, q5, q6, q7, u0, u1, u2, u3, u4, u5, u6, u7, v0, v1, v2, v3, v4, v5, v6, v7, w0, w1, w2, w3, w4, w5, w6, w7;
	__m128i O00, O01, O02, O03, O04, O05, O06, O07;
	__m128i P00, P01, P02, P03, P04, P05, P06, P07;
	__m128i tmpZero = _mm_setzero_si128();
	const __m128i k_p32_p32 = _mm_set1_epi16(32);
	const __m128i k_p32_m32 = pair_set_epi16(32, -32);
	const __m128i k_p42_p17 = pair_set_epi16(42, 17);
	const __m128i k_p17_m42 = pair_set_epi16(17, -42);
	const __m128i k_p44_p38 = pair_set_epi16(44, 38);
	const __m128i k_p25_p9 = pair_set_epi16(25, 9);
	const __m128i k_p38_m9 = pair_set_epi16(38, -9);
	const __m128i k_m44_m25 = pair_set_epi16(-44, -25);
	const __m128i k_p25_m44 = pair_set_epi16(25, -44);
	const __m128i k_p9_p38 = pair_set_epi16(9, 38);
	const __m128i k_p9_m25 = pair_set_epi16(9, -25);
	const __m128i k_p38_m44 = pair_set_epi16(38, -44);

	//load data
	O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org]);
	O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org]);
	O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org]);
	O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org]);
	O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org]);
	O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org]);
	O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org]);
	O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org]);
	O00 = _mm_unpacklo_epi8(O00, tmpZero);
	O01 = _mm_unpacklo_epi8(O01, tmpZero);
	O02 = _mm_unpacklo_epi8(O02, tmpZero);
	O03 = _mm_unpacklo_epi8(O03, tmpZero);
	O04 = _mm_unpacklo_epi8(O04, tmpZero);
	O05 = _mm_unpacklo_epi8(O05, tmpZero);
	O06 = _mm_unpacklo_epi8(O06, tmpZero);
	O07 = _mm_unpacklo_epi8(O07, tmpZero);

	P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred]);
	P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred]);
	P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred]);
	P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred]);
	P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred]);
	P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred]);
	P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred]);
	P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred]);
	P00 = _mm_unpacklo_epi8(P00, tmpZero);
	P01 = _mm_unpacklo_epi8(P01, tmpZero);
	P02 = _mm_unpacklo_epi8(P02, tmpZero);
	P03 = _mm_unpacklo_epi8(P03, tmpZero);
	P04 = _mm_unpacklo_epi8(P04, tmpZero);
	P05 = _mm_unpacklo_epi8(P05, tmpZero);
	P06 = _mm_unpacklo_epi8(P06, tmpZero);
	P07 = _mm_unpacklo_epi8(P07, tmpZero);

	in0 = _mm_sub_epi16(O00, P00);
	in1 = _mm_sub_epi16(O01, P01);
	in2 = _mm_sub_epi16(O02, P02);
	in3 = _mm_sub_epi16(O03, P03);
	in4 = _mm_sub_epi16(O04, P04);
	in5 = _mm_sub_epi16(O05, P05);
	in6 = _mm_sub_epi16(O06, P06);
	in7 = _mm_sub_epi16(O07, P07);

	//DCT1
#define TRANSPOSE_8x8_16BIT(I0, I1, I2, I3, I4, I5, I6, I7) \
    tr0_0 = _mm_unpacklo_epi16(I0, I1); \
    tr0_1 = _mm_unpacklo_epi16(I2, I3); \
    tr0_2 = _mm_unpackhi_epi16(I0, I1); \
    tr0_3 = _mm_unpackhi_epi16(I2, I3); \
    tr0_4 = _mm_unpacklo_epi16(I4, I5); \
    tr0_5 = _mm_unpacklo_epi16(I6, I7); \
    tr0_6 = _mm_unpackhi_epi16(I4, I5); \
    tr0_7 = _mm_unpackhi_epi16(I6, I7); \
    tr1_0 = _mm_unpacklo_epi32(tr0_0, tr0_1); \
    tr1_1 = _mm_unpacklo_epi32(tr0_2, tr0_3); \
    tr1_2 = _mm_unpackhi_epi32(tr0_0, tr0_1); \
    tr1_3 = _mm_unpackhi_epi32(tr0_2, tr0_3); \
    tr1_4 = _mm_unpacklo_epi32(tr0_4, tr0_5); \
    tr1_5 = _mm_unpacklo_epi32(tr0_6, tr0_7); \
    tr1_6 = _mm_unpackhi_epi32(tr0_4, tr0_5); \
    tr1_7 = _mm_unpackhi_epi32(tr0_6, tr0_7); \
    I0 = _mm_unpacklo_epi64(tr1_0, tr1_4); \
    I1 = _mm_unpackhi_epi64(tr1_0, tr1_4); \
    I2 = _mm_unpacklo_epi64(tr1_2, tr1_6); \
    I3 = _mm_unpackhi_epi64(tr1_2, tr1_6); \
    I4 = _mm_unpacklo_epi64(tr1_1, tr1_5); \
    I5 = _mm_unpackhi_epi64(tr1_1, tr1_5); \
    I6 = _mm_unpacklo_epi64(tr1_3, tr1_7); \
    I7 = _mm_unpackhi_epi64(tr1_3, tr1_7); \

	TRANSPOSE_8x8_16BIT(in0, in1, in2, in3, in4, in5, in6, in7)
#undef TRANSPOSE_8x8_16BIT

		q0 = _mm_add_epi16(in0, in7);	//E0
	q1 = _mm_add_epi16(in1, in6);	//E1
	q2 = _mm_add_epi16(in2, in5);	//E2
	q3 = _mm_add_epi16(in3, in4);	//E3
	q4 = _mm_sub_epi16(in0, in7);	//O0
	q5 = _mm_sub_epi16(in1, in6);	//O1
	q6 = _mm_sub_epi16(in2, in5);	//O2
	q7 = _mm_sub_epi16(in3, in4);	//O3

	//even lines
	r0 = _mm_add_epi16(q0, q3);	//EE0
	r1 = _mm_add_epi16(q1, q2);	//EE1
	r2 = _mm_sub_epi16(q0, q3);	//EO0
	r3 = _mm_sub_epi16(q1, q2);	//EO1

	t0 = _mm_unpacklo_epi16(r0, r1);	//interleave EE0 & EE1
	t1 = _mm_unpackhi_epi16(r0, r1);
	t2 = _mm_unpacklo_epi16(r2, r3);	//interleave EO0 & EO1
	t3 = _mm_unpackhi_epi16(r2, r3);
	u0 = _mm_madd_epi16(t0, k_p32_p32);
	u1 = _mm_madd_epi16(t1, k_p32_p32);
	u2 = _mm_madd_epi16(t0, k_p32_m32);
	u3 = _mm_madd_epi16(t1, k_p32_m32);
	u4 = _mm_madd_epi16(t2, k_p42_p17);
	u5 = _mm_madd_epi16(t3, k_p42_p17);
	u6 = _mm_madd_epi16(t2, k_p17_m42);
	u7 = _mm_madd_epi16(t3, k_p17_m42);

	v0 = _mm_add_epi32(u0, k_ROUNDING1);
	v1 = _mm_add_epi32(u1, k_ROUNDING1);
	v2 = _mm_add_epi32(u2, k_ROUNDING1);
	v3 = _mm_add_epi32(u3, k_ROUNDING1);
	v4 = _mm_add_epi32(u4, k_ROUNDING1);
	v5 = _mm_add_epi32(u5, k_ROUNDING1);
	v6 = _mm_add_epi32(u6, k_ROUNDING1);
	v7 = _mm_add_epi32(u7, k_ROUNDING1);
	w0 = _mm_srai_epi32(v0, shift1);
	w1 = _mm_srai_epi32(v1, shift1);
	w2 = _mm_srai_epi32(v2, shift1);
	w3 = _mm_srai_epi32(v3, shift1);
	w4 = _mm_srai_epi32(v4, shift1);
	w5 = _mm_srai_epi32(v5, shift1);
	w6 = _mm_srai_epi32(v6, shift1);
	w7 = _mm_srai_epi32(v7, shift1);

	res0 = _mm_packs_epi32(w0, w1);
	res4 = _mm_packs_epi32(w2, w3);
	res2 = _mm_packs_epi32(w4, w5);
	res6 = _mm_packs_epi32(w6, w7);

	// odd lines
	t0 = _mm_unpacklo_epi16(q4, q5);	//interleave O0 & O1
	t1 = _mm_unpackhi_epi16(q4, q5);
	t2 = _mm_unpacklo_epi16(q6, q7);	//interleave O2 & O3
	t3 = _mm_unpackhi_epi16(q6, q7);

	//line 1
	u0 = _mm_madd_epi16(t0, k_p44_p38);
	u1 = _mm_madd_epi16(t1, k_p44_p38);
	u2 = _mm_madd_epi16(t2, k_p25_p9);
	u3 = _mm_madd_epi16(t3, k_p25_p9);
	v0 = _mm_add_epi32(u0, u2);
	v1 = _mm_add_epi32(u1, u3);
	v0 = _mm_add_epi32(v0, k_ROUNDING1);
	v1 = _mm_add_epi32(v1, k_ROUNDING1);
	w0 = _mm_srai_epi32(v0, shift1);
	w1 = _mm_srai_epi32(v1, shift1);
	res1 = _mm_packs_epi32(w0, w1);

	//line 3
	u0 = _mm_madd_epi16(t0, k_p38_m9);
	u1 = _mm_madd_epi16(t1, k_p38_m9);
	u2 = _mm_madd_epi16(t2, k_m44_m25);
	u3 = _mm_madd_epi16(t3, k_m44_m25);
	v0 = _mm_add_epi32(u0, u2);
	v1 = _mm_add_epi32(u1, u3);
	v0 = _mm_add_epi32(v0, k_ROUNDING1);
	v1 = _mm_add_epi32(v1, k_ROUNDING1);
	w0 = _mm_srai_epi32(v0, shift1);
	w1 = _mm_srai_epi32(v1, shift1);
	res3 = _mm_packs_epi32(w0, w1);

	//line 5
	u0 = _mm_madd_epi16(t0, k_p25_m44);
	u1 = _mm_madd_epi16(t1, k_p25_m44);
	u2 = _mm_madd_epi16(t2, k_p9_p38);
	u3 = _mm_madd_epi16(t3, k_p9_p38);
	v0 = _mm_add_epi32(u0, u2);
	v1 = _mm_add_epi32(u1, u3);
	v0 = _mm_add_epi32(v0, k_ROUNDING1);
	v1 = _mm_add_epi32(v1, k_ROUNDING1);
	w0 = _mm_srai_epi32(v0, shift1);
	w1 = _mm_srai_epi32(v1, shift1);
	res5 = _mm_packs_epi32(w0, w1);

	//line 7
	u0 = _mm_madd_epi16(t0, k_p9_m25);
	u1 = _mm_madd_epi16(t1, k_p9_m25);
	u2 = _mm_madd_epi16(t2, k_p38_m44);
	u3 = _mm_madd_epi16(t3, k_p38_m44);
	v0 = _mm_add_epi32(u0, u2);
	v1 = _mm_add_epi32(u1, u3);
	v0 = _mm_add_epi32(v0, k_ROUNDING1);
	v1 = _mm_add_epi32(v1, k_ROUNDING1);
	w0 = _mm_srai_epi32(v0, shift1);
	w1 = _mm_srai_epi32(v1, shift1);
	res7 = _mm_packs_epi32(w0, w1);

	//DCT2
#define MAKE_ODD(tab)\
	q0 = _mm_madd_epi16(res0, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	q1 = _mm_madd_epi16(res1, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	q2 = _mm_madd_epi16(res2, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	q3 = _mm_madd_epi16(res3, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	q4 = _mm_madd_epi16(res4, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	q5 = _mm_madd_epi16(res5, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	q6 = _mm_madd_epi16(res6, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	q7 = _mm_madd_epi16(res7, _mm_load_si128((__m128i*)tab_dct_8_1[tab]));\
	r0 = _mm_hadd_epi32(q0, q1);\
	r1 = _mm_hadd_epi32(q2, q3);\
	r2 = _mm_hadd_epi32(q4, q5);\
	r3 = _mm_hadd_epi32(q6, q7);\
	v0 = _mm_hadd_epi32(r0, r1);\
	v1 = _mm_hadd_epi32(r2, r3);\
	v0 = _mm_add_epi32(v0, k_ROUNDING2);\
	v1 = _mm_add_epi32(v1, k_ROUNDING2);\
	w0 = _mm_srai_epi32(v0, shift2);\
	w1 = _mm_srai_epi32(v1, shift2);\
	t0 = _mm_packs_epi32(w0, w1);\
	_mm_storeu_si128((__m128i *)(dst + tab * 8), t0);

	MAKE_ODD(0);
	MAKE_ODD(1);
	MAKE_ODD(2);
	MAKE_ODD(3);
	MAKE_ODD(4);
	MAKE_ODD(5);
	MAKE_ODD(6);
	MAKE_ODD(7);
#undef MAKE_ODD
}

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// dct 16x16 ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
ALIGNED_32(static const coef_t tab_dct_8[][8]) =
{
	{ 0x0100, 0x0F0E, 0x0706, 0x0908, 0x0302, 0x0D0C, 0x0504, 0x0B0A },

	{ 32, 32, 32, 32, 32, 32, 32, 32 },
	{ 32, -32, 32, -32, 32, -32, 32, -32 },
	{ 42, 17, 42, 17, 42, 17, 42, 17 },
	{ 17, -42, 17, -42, 17, -42, 17, -42 },
	{ 44, 9, 38, 25, 44, 9, 38, 25 },
	{ 38, -25, -9, -44, 38, -25, -9, -44 },
	{ 25, 38, -44, 9, 25, 38, -44, 9 },
	{ 9, -44, -25, 38, 9, -44, -25, 38 },
};

ALIGNED_32(static const coef_t tab_dct_16_0[][8]) =
{
	{ 0x0F0E, 0x0D0C, 0x0B0A, 0x0908, 0x0706, 0x0504, 0x0302, 0x0100 },  // 0
	{ 0x0100, 0x0F0E, 0x0706, 0x0908, 0x0302, 0x0D0C, 0x0504, 0x0B0A },  // 1
	{ 0x0100, 0x0706, 0x0302, 0x0504, 0x0F0E, 0x0908, 0x0D0C, 0x0B0A },  // 2
	{ 0x0F0E, 0x0908, 0x0D0C, 0x0B0A, 0x0100, 0x0706, 0x0302, 0x0504 },  // 3
};

ALIGNED_32(static const coef_t tab_dct_8_256i[][16]) =
{
	{ 0x0100, 0x0F0E, 0x0706, 0x0908, 0x0302, 0x0D0C, 0x0504, 0x0B0A, 0x0100, 0x0F0E, 0x0706, 0x0908, 0x0302, 0x0D0C, 0x0504, 0x0B0A },

	{ 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, },
	{ 32, -32, 32, -32, 32, -32, 32, -32, 32, -32, 32, -32, 32, -32, 32, -32 },
	{ 42, 17, 42, 17, 42, 17, 42, 17, 42, 17, 42, 17, 42, 17, 42, 17 },
	{ 17, -42, 17, -42, 17, -42, 17, -42, 17, -42, 17, -42, 17, -42, 17, -42 },
	{ 44, 9, 38, 25, 44, 9, 38, 25, 44, 9, 38, 25, 44, 9, 38, 25 },
	{ 38, -25, -9, -44, 38, -25, -9, -44, 38, -25, -9, -44, 38, -25, -9, -44 },
	{ 25, 38, -44, 9, 25, 38, -44, 9, 25, 38, -44, 9, 25, 38, -44, 9 },
	{ 9, -44, -25, 38, 9, -44, -25, 38, 9, -44, -25, 38, 9, -44, -25, 38 },
};

ALIGNED_32(static const coef_t tab_dct_16_0_256i[][16]) =
{
	{ 0x0F0E, 0x0D0C, 0x0B0A, 0x0908, 0x0706, 0x0504, 0x0302, 0x0100, 0x0F0E, 0x0D0C, 0x0B0A, 0x0908, 0x0706, 0x0504, 0x0302, 0x0100 },  // 0
	{ 0x0100, 0x0F0E, 0x0706, 0x0908, 0x0302, 0x0D0C, 0x0504, 0x0B0A, 0x0100, 0x0F0E, 0x0706, 0x0908, 0x0302, 0x0D0C, 0x0504, 0x0B0A },  // 1
	{ 0x0100, 0x0706, 0x0302, 0x0504, 0x0F0E, 0x0908, 0x0D0C, 0x0B0A, 0x0100, 0x0706, 0x0302, 0x0504, 0x0F0E, 0x0908, 0x0D0C, 0x0B0A },  // 2
	{ 0x0F0E, 0x0908, 0x0D0C, 0x0B0A, 0x0100, 0x0706, 0x0302, 0x0504, 0x0F0E, 0x0908, 0x0D0C, 0x0B0A, 0x0100, 0x0706, 0x0302, 0x0504 },  // 3
};

ALIGNED_32(static const coef_t tab_dct_16_1[][8]) =
{
	{ 45, 43, 40, 35, 29, 21, 13, 4 },  //  0
	{ 43, 29, 4, -21, -40, -45, -35, -13 },  //  1
	{ 40, 4, -35, -43, -13, 29, 45, 21 },  //  2
	{ 35, -21, -43, 4, 45, 13, -40, -29 },  //  3
	{ 29, -40, -13, 45, -4, -43, 21, 35 },  //  4
	{ 21, -45, 29, 13, -43, 35, 4, -40 },  //  5
	{ 13, -35, 45, -40, 21, 4, -29, 43 },  //  6
	{ 4, -13, 21, -29, 35, -40, 43, -45 },  //  7
	{ 42, 42, -42, -42, 17, 17, -17, -17 },  //  8
	{ 17, 17, -17, -17, -42, -42, 42, 42 },  //  9
	{ 44, 44, 9, 9, 38, 38, 25, 25 },  // 10
	{ 38, 38, -25, -25, -9, -9, -44, -44 },  // 11
	{ 25, 25, 38, 38, -44, -44, 9, 9 },  // 12
	{ 9, 9, -44, -44, -25, -25, 38, 38 },  // 13
#define MAKE_COEF(a0, a1, a2, a3, a4, a5, a6, a7) \
		    { (a0), -(a0), (a3), -(a3), (a1), -(a1), (a2), -(a2) \
		    }, \
		    { (a7), -(a7), (a4), -(a4), (a6), -(a6), (a5), -(a5) },

	MAKE_COEF(45, 43, 40, 35, 29, 21, 13, 4)
	MAKE_COEF(43, 29, 4, -21, -40, -45, -35, -13)
	MAKE_COEF(40, 4, -35, -43, -13, 29, 45, 21)
	MAKE_COEF(35, -21, -43, 4, 45, 13, -40, -29)
	MAKE_COEF(29, -40, -13, 45, -4, -43, 21, 35)
	MAKE_COEF(21, -45, 29, 13, -43, 35, 4, -40)
	MAKE_COEF(13, -35, 45, -40, 21, 4, -29, 43)
	MAKE_COEF(4, -13, 21, -29, 35, -40, 43, -45)
#undef MAKE_COEF
};

ALIGNED_32(static const coef_t tab_dct_16_1_256i[][16]) =
{
	{ 45, 43, 40, 35, 29, 21, 13, 4, 45, 43, 40, 35, 29, 21, 13, 4 },  //  0
	{ 43, 29, 4, -21, -40, -45, -35, -13, 43, 29, 4, -21, -40, -45, -35, -13 },  //  1
	{ 40, 4, -35, -43, -13, 29, 45, 21, 40, 4, -35, -43, -13, 29, 45, 21 },  //  2
	{ 35, -21, -43, 4, 45, 13, -40, -29, 35, -21, -43, 4, 45, 13, -40, -29 },  //  3
	{ 29, -40, -13, 45, -4, -43, 21, 35, 29, -40, -13, 45, -4, -43, 21, 35 },  //  4
	{ 21, -45, 29, 13, -43, 35, 4, -40, 21, -45, 29, 13, -43, 35, 4, -40 },  //  5
	{ 13, -35, 45, -40, 21, 4, -29, 43, 13, -35, 45, -40, 21, 4, -29, 43 },  //  6
	{ 4, -13, 21, -29, 35, -40, 43, -45, 4, -13, 21, -29, 35, -40, 43, -45 },  //  7
	{ 42, 42, -42, -42, 17, 17, -17, -17, 42, 42, -42, -42, 17, 17, -17, -17 },  //  8
	{ 17, 17, -17, -17, -42, -42, 42, 42, 17, 17, -17, -17, -42, -42, 42, 42 },  //  9
	{ 44, 44, 9, 9, 38, 38, 25, 25, 44, 44, 9, 9, 38, 38, 25, 25 },  // 10
	{ 38, 38, -25, -25, -9, -9, -44, -44, 38, 38, -25, -25, -9, -9, -44, -44 },  // 11
	{ 25, 25, 38, 38, -44, -44, 9, 9, 25, 25, 38, 38, -44, -44, 9, 9 },  // 12
	{ 9, 9, -44, -44, -25, -25, 38, 38, 9, 9, -44, -44, -25, -25, 38, 38 },  // 13
#define MAKE_COEF(a0, a1, a2, a3, a4, a5, a6, a7) \
	{ (a0), -(a0), (a3), -(a3), (a1), -(a1), (a2), -(a2), (a0), -(a0), (a3), -(a3), (a1), -(a1), (a2), -(a2) \
	}, \
	{ (a7), -(a7), (a4), -(a4), (a6), -(a6), (a5), -(a5), (a7), -(a7), (a4), -(a4), (a6), -(a6), (a5), -(a5) },

	MAKE_COEF(45, 43, 40, 35, 29, 21, 13, 4)
	MAKE_COEF(43, 29, 4, -21, -40, -45, -35, -13)
	MAKE_COEF(40, 4, -35, -43, -13, 29, 45, 21)
	MAKE_COEF(35, -21, -43, 4, 45, 13, -40, -29)
	MAKE_COEF(29, -40, -13, 45, -4, -43, 21, 35)
	MAKE_COEF(21, -45, 29, 13, -43, 35, 4, -40)
	MAKE_COEF(13, -35, 45, -40, 21, 4, -29, 43)
	MAKE_COEF(4, -13, 21, -29, 35, -40, 43, -45)
#undef MAKE_COEF
};

void sub_trans_16x16_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift)
{
	// Const
	__m128i c_2 = _mm_set1_epi32(2);	//shift2 = 9
	__m128i c_256 = _mm_set1_epi32(256);	//shift2 = 9
	int shift1 = 2;
	int shift2 = 9;
	int i;

	__m128i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m128i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m128i T10, T11, T12, T13, T14, T15, T16, T17;
	__m128i T20, T21, T22, T23, T24, T25, T26, T27;
	__m128i T30, T31, T32, T33, T34, T35, T36, T37;
	__m128i T40, T41, T42, T43, T44, T45, T46, T47; 
	__m128i T50, T51, T52, T53;
	__m128i T60, T61, T62, T63, T64, T65, T66, T67;
	__m128i TT10, TT11, TT12, TT13, TT14, TT15, TT16, TT17;
	__m128i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m128i TT30, TT31, TT32, TT33;
	__m128i TT40, TT41;
	__m128i T70;

	__m128i O00, O01, O02, O03, O04, O05, O06, O07;
	__m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m128i P00, P01, P02, P03, P04, P05, P06, P07;
	__m128i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m128i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;

	__m128i im[16][2];
	__m128i tmpZero = _mm_setzero_si128();

	// DCT1
	for (i = 0; i < 16 / 8; i ++)
	{
		//input data
		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org]);

        org += i_org << 3;

		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);

		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred]);

        pred += i_pred << 3;

		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);

		T00A = _mm_sub_epi16(O00A, P00A);
		T00B = _mm_sub_epi16(O00B, P00B);
		T01A = _mm_sub_epi16(O01A, P01A);
		T01B = _mm_sub_epi16(O01B, P01B);
		T02A = _mm_sub_epi16(O02A, P02A);
		T02B = _mm_sub_epi16(O02B, P02B);
		T03A = _mm_sub_epi16(O03A, P03A);
		T03B = _mm_sub_epi16(O03B, P03B);
		T04A = _mm_sub_epi16(O04A, P04A);
		T04B = _mm_sub_epi16(O04B, P04B);
		T05A = _mm_sub_epi16(O05A, P05A);
		T05B = _mm_sub_epi16(O05B, P05B);
		T06A = _mm_sub_epi16(O06A, P06A);
		T06B = _mm_sub_epi16(O06B, P06B);
		T07A = _mm_sub_epi16(O07A, P07A);
		T07B = _mm_sub_epi16(O07B, P07B);

		//shuffle
		T00B = _mm_shuffle_epi8(T00B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T01B = _mm_shuffle_epi8(T01B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02B = _mm_shuffle_epi8(T02B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03B = _mm_shuffle_epi8(T03B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T04B = _mm_shuffle_epi8(T04B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T05B = _mm_shuffle_epi8(T05B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T06B = _mm_shuffle_epi8(T06B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T07B = _mm_shuffle_epi8(T07B, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		T10 = _mm_add_epi16(T00A, T00B);
		T11 = _mm_add_epi16(T01A, T01B);
		T12 = _mm_add_epi16(T02A, T02B);
		T13 = _mm_add_epi16(T03A, T03B);
		T14 = _mm_add_epi16(T04A, T04B);
		T15 = _mm_add_epi16(T05A, T05B);
		T16 = _mm_add_epi16(T06A, T06B);
		T17 = _mm_add_epi16(T07A, T07B);

		T20 = _mm_sub_epi16(T00A, T00B);
		T21 = _mm_sub_epi16(T01A, T01B);
		T22 = _mm_sub_epi16(T02A, T02B);
		T23 = _mm_sub_epi16(T03A, T03B);
		T24 = _mm_sub_epi16(T04A, T04B);
		T25 = _mm_sub_epi16(T05A, T05B);
		T26 = _mm_sub_epi16(T06A, T06B);
		T27 = _mm_sub_epi16(T07A, T07B);

		T30 = _mm_shuffle_epi8(T10, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T31 = _mm_shuffle_epi8(T11, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T32 = _mm_shuffle_epi8(T12, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T33 = _mm_shuffle_epi8(T13, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T34 = _mm_shuffle_epi8(T14, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T35 = _mm_shuffle_epi8(T15, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T36 = _mm_shuffle_epi8(T16, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T37 = _mm_shuffle_epi8(T17, _mm_load_si128((__m128i*)tab_dct_16_0[1]));

		T40 = _mm_hadd_epi16(T30, T31);
		T41 = _mm_hadd_epi16(T32, T33);
		T42 = _mm_hadd_epi16(T34, T35);
		T43 = _mm_hadd_epi16(T36, T37);
		T44 = _mm_hsub_epi16(T30, T31);
		T45 = _mm_hsub_epi16(T32, T33);
		T46 = _mm_hsub_epi16(T34, T35);
		T47 = _mm_hsub_epi16(T36, T37);

		T50 = _mm_hadd_epi16(T40, T41);
		T51 = _mm_hadd_epi16(T42, T43);
		T52 = _mm_hsub_epi16(T40, T41);
		T53 = _mm_hsub_epi16(T42, T43);

		T60 = _mm_madd_epi16(T50, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T61 = _mm_madd_epi16(T51, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[0][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[0 * 16 + i], T70);

		T60 = _mm_madd_epi16(T50, _mm_load_si128((__m128i*)tab_dct_8[2]));
		T61 = _mm_madd_epi16(T51, _mm_load_si128((__m128i*)tab_dct_8[2]));
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[8][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[8 * 16 + i], T70);

		T60 = _mm_madd_epi16(T52, _mm_load_si128((__m128i*)tab_dct_8[3]));
		T61 = _mm_madd_epi16(T53, _mm_load_si128((__m128i*)tab_dct_8[3]));
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[4][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[4 * 16 + i], T70);

		T60 = _mm_madd_epi16(T52, _mm_load_si128((__m128i*)tab_dct_8[4]));
		T61 = _mm_madd_epi16(T53, _mm_load_si128((__m128i*)tab_dct_8[4]));
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[12][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[12 * 16 + i], T70);

		T60 = _mm_madd_epi16(T44, _mm_load_si128((__m128i*)tab_dct_8[5]));
		T61 = _mm_madd_epi16(T45, _mm_load_si128((__m128i*)tab_dct_8[5]));
		T62 = _mm_madd_epi16(T46, _mm_load_si128((__m128i*)tab_dct_8[5]));
		T63 = _mm_madd_epi16(T47, _mm_load_si128((__m128i*)tab_dct_8[5]));
		T60 = _mm_hadd_epi32(T60, T61);
		T61 = _mm_hadd_epi32(T62, T63);
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[2][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[2 * 16 + i], T70);

		T60 = _mm_madd_epi16(T44, _mm_load_si128((__m128i*)tab_dct_8[6]));
		T61 = _mm_madd_epi16(T45, _mm_load_si128((__m128i*)tab_dct_8[6]));
		T62 = _mm_madd_epi16(T46, _mm_load_si128((__m128i*)tab_dct_8[6]));
		T63 = _mm_madd_epi16(T47, _mm_load_si128((__m128i*)tab_dct_8[6]));
		T60 = _mm_hadd_epi32(T60, T61);
		T61 = _mm_hadd_epi32(T62, T63);
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[6][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[6 * 16 + i], T70);

		T60 = _mm_madd_epi16(T44, _mm_load_si128((__m128i*)tab_dct_8[7]));
		T61 = _mm_madd_epi16(T45, _mm_load_si128((__m128i*)tab_dct_8[7]));
		T62 = _mm_madd_epi16(T46, _mm_load_si128((__m128i*)tab_dct_8[7]));
		T63 = _mm_madd_epi16(T47, _mm_load_si128((__m128i*)tab_dct_8[7]));
		T60 = _mm_hadd_epi32(T60, T61);
		T61 = _mm_hadd_epi32(T62, T63);
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[10][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[10 * 16 + i], T70);

		T60 = _mm_madd_epi16(T44, _mm_load_si128((__m128i*)tab_dct_8[8]));
		T61 = _mm_madd_epi16(T45, _mm_load_si128((__m128i*)tab_dct_8[8]));
		T62 = _mm_madd_epi16(T46, _mm_load_si128((__m128i*)tab_dct_8[8]));
		T63 = _mm_madd_epi16(T47, _mm_load_si128((__m128i*)tab_dct_8[8]));
		T60 = _mm_hadd_epi32(T60, T61);
		T61 = _mm_hadd_epi32(T62, T63);
		T60 = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1);
		T61 = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1);
		T70 = _mm_packs_epi32(T60, T61);
		im[14][i] = T70;
//		_mm_store_si128((__m128i*)&tmp[14 * 16 + i], T70);

#define MAKE_ODD(tab, dstPos) \
    T60  = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T61  = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T62  = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T63  = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T64  = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T65  = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T66  = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T67  = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
    T62  = _mm_hadd_epi32(T64, T65); \
    T63  = _mm_hadd_epi32(T66, T67); \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
    T60  = _mm_srai_epi32(_mm_add_epi32(T60, c_2), shift1); \
    T61  = _mm_srai_epi32(_mm_add_epi32(T61, c_2), shift1); \
    T70  = _mm_packs_epi32(T60, T61); \
	im[dstPos][i] = T70;
//    _mm_store_si128((__m128i*)&tmp[(dstPos) * 16 + i], T70);

		MAKE_ODD(0, 1);
		MAKE_ODD(1, 3);
		MAKE_ODD(2, 5);
		MAKE_ODD(3, 7);
		MAKE_ODD(4, 9);
		MAKE_ODD(5, 11);
		MAKE_ODD(6, 13);
		MAKE_ODD(7, 15);
#undef MAKE_ODD
	}

	// DCT2
	for (i = 0; i < 16; i += 8)
	{
		T00A = im[(i + 0)][0];
		T00B = im[(i + 0)][1];
		T01A = im[(i + 1)][0];
		T01B = im[(i + 1)][1];
		T02A = im[(i + 2)][0];
		T02B = im[(i + 2)][1];
		T03A = im[(i + 3)][0];
		T03B = im[(i + 3)][1];
		T04A = im[(i + 4)][0];
		T04B = im[(i + 4)][1];
		T05A = im[(i + 5)][0];
		T05B = im[(i + 5)][1];
		T06A = im[(i + 6)][0];
		T06B = im[(i + 6)][1];
		T07A = im[(i + 7)][0];
		T07B = im[(i + 7)][1];

		T00A = _mm_shuffle_epi8(T00A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T00B = _mm_shuffle_epi8(T00B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));
		T01A = _mm_shuffle_epi8(T01A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T01B = _mm_shuffle_epi8(T01B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));
		T02A = _mm_shuffle_epi8(T02A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T02B = _mm_shuffle_epi8(T02B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));
		T03A = _mm_shuffle_epi8(T03A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T03B = _mm_shuffle_epi8(T03B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));

		T04A = _mm_shuffle_epi8(T04A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T04B = _mm_shuffle_epi8(T04B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));
		T05A = _mm_shuffle_epi8(T05A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T05B = _mm_shuffle_epi8(T05B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));
		T06A = _mm_shuffle_epi8(T06A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T06B = _mm_shuffle_epi8(T06B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));
		T07A = _mm_shuffle_epi8(T07A, _mm_load_si128((__m128i*)tab_dct_16_0[2]));
		T07B = _mm_shuffle_epi8(T07B, _mm_load_si128((__m128i*)tab_dct_16_0[3]));

		T10 = _mm_unpacklo_epi16(T00A, T00B);
		T11 = _mm_unpackhi_epi16(T00A, T00B);
		T12 = _mm_unpacklo_epi16(T01A, T01B);
		T13 = _mm_unpackhi_epi16(T01A, T01B);
		T14 = _mm_unpacklo_epi16(T02A, T02B);
		T15 = _mm_unpackhi_epi16(T02A, T02B);
		T16 = _mm_unpacklo_epi16(T03A, T03B);
		T17 = _mm_unpackhi_epi16(T03A, T03B);

		TT10 = _mm_unpacklo_epi16(T04A, T04B);
		TT11 = _mm_unpackhi_epi16(T04A, T04B);
		TT12 = _mm_unpacklo_epi16(T05A, T05B);
		TT13 = _mm_unpackhi_epi16(T05A, T05B);
		TT14 = _mm_unpacklo_epi16(T06A, T06B);
		TT15 = _mm_unpackhi_epi16(T06A, T06B);
		TT16 = _mm_unpacklo_epi16(T07A, T07B);
		TT17 = _mm_unpackhi_epi16(T07A, T07B);

		T20 = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T21 = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T22 = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T23 = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T24 = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T25 = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T26 = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T27 = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_8[1]));

		TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_8[1]));
		TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_8[1]));
		TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_8[1]));
		TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_8[1]));
		TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_8[1]));
		TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_8[1]));
		TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_8[1]));
		TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_8[1]));

		T30 = _mm_add_epi32(T20, T21);
		T31 = _mm_add_epi32(T22, T23);
		T32 = _mm_add_epi32(T24, T25);
		T33 = _mm_add_epi32(T26, T27);

		TT30 = _mm_add_epi32(TT20, TT21);
		TT31 = _mm_add_epi32(TT22, TT23);
		TT32 = _mm_add_epi32(TT24, TT25);
		TT33 = _mm_add_epi32(TT26, TT27);

		T30 = _mm_hadd_epi32(T30, T31);
		T31 = _mm_hadd_epi32(T32, T33);

		TT30 = _mm_hadd_epi32(TT30, TT31);
		TT31 = _mm_hadd_epi32(TT32, TT33);

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hsub_epi32(T30, T31);
		T40 = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2);
		T41 = _mm_srai_epi32(_mm_add_epi32(T41, c_256), shift2);

		TT40 = _mm_hadd_epi32(TT30, TT31);
		TT41 = _mm_hsub_epi32(TT30, TT31);
		TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2);
		TT41 = _mm_srai_epi32(_mm_add_epi32(TT41, c_256), shift2);

		T70 = _mm_packs_epi32(T40, TT40);
		_mm_storeu_si128((__m128i*)&dst[0 * 16 + i], T70);
		T70 = _mm_packs_epi32(T41, TT41);
		_mm_storeu_si128((__m128i*)&dst[8 * 16 + i], T70);

		T20 = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T21 = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T22 = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T23 = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T24 = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T25 = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T26 = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T27 = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_16_1[8]));

		TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_16_1[8]));

		T30 = _mm_add_epi32(T20, T21);
		T31 = _mm_add_epi32(T22, T23);
		T32 = _mm_add_epi32(T24, T25);
		T33 = _mm_add_epi32(T26, T27);

		TT30 = _mm_add_epi32(TT20, TT21);
		TT31 = _mm_add_epi32(TT22, TT23);
		TT32 = _mm_add_epi32(TT24, TT25);
		TT33 = _mm_add_epi32(TT26, TT27);

		T30 = _mm_hadd_epi32(T30, T31);
		T31 = _mm_hadd_epi32(T32, T33);

		TT30 = _mm_hadd_epi32(TT30, TT31);
		TT31 = _mm_hadd_epi32(TT32, TT33);

		T40 = _mm_hadd_epi32(T30, T31);
		T40 = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2);

		TT40 = _mm_hadd_epi32(TT30, TT31);
		TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2);
		T70 = _mm_packs_epi32(T40, TT40);
		_mm_storeu_si128((__m128i*)&dst[4 * 16 + i], T70);

		T20 = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T21 = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T22 = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T23 = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T24 = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T25 = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T26 = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T27 = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_16_1[9]));

		TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_16_1[9]));

		T30 = _mm_add_epi32(T20, T21);
		T31 = _mm_add_epi32(T22, T23);
		T32 = _mm_add_epi32(T24, T25);
		T33 = _mm_add_epi32(T26, T27);

		TT30 = _mm_add_epi32(TT20, TT21);
		TT31 = _mm_add_epi32(TT22, TT23);
		TT32 = _mm_add_epi32(TT24, TT25);
		TT33 = _mm_add_epi32(TT26, TT27);

		T30 = _mm_hadd_epi32(T30, T31);
		T31 = _mm_hadd_epi32(T32, T33);

		TT30 = _mm_hadd_epi32(TT30, TT31);
		TT31 = _mm_hadd_epi32(TT32, TT33);

		T40 = _mm_hadd_epi32(T30, T31);
		T40 = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2);
		TT40 = _mm_hadd_epi32(TT30, TT31);
		TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2);
		T70 = _mm_packs_epi32(T40, TT40);
		_mm_storeu_si128((__m128i*)&dst[12 * 16 + i], T70);

		T20 = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		T21 = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		T22 = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		T23 = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		T24 = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		T25 = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		T26 = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		T27 = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_16_1[10]));

		TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_16_1[10]));
		TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_16_1[10]));

		T30 = _mm_sub_epi32(T20, T21);
		T31 = _mm_sub_epi32(T22, T23);
		T32 = _mm_sub_epi32(T24, T25);
		T33 = _mm_sub_epi32(T26, T27);

		TT30 = _mm_sub_epi32(TT20, TT21);
		TT31 = _mm_sub_epi32(TT22, TT23);
		TT32 = _mm_sub_epi32(TT24, TT25);
		TT33 = _mm_sub_epi32(TT26, TT27);

		T30 = _mm_hadd_epi32(T30, T31);
		T31 = _mm_hadd_epi32(T32, T33);

		TT30 = _mm_hadd_epi32(TT30, TT31);
		TT31 = _mm_hadd_epi32(TT32, TT33);

		T40 = _mm_hadd_epi32(T30, T31);
		T40 = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2);
		TT40 = _mm_hadd_epi32(TT30, TT31);
		TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2);
		T70 = _mm_packs_epi32(T40, TT40);
		_mm_storeu_si128((__m128i*)&dst[2 * 16 + i], T70);

		T20 = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		T21 = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		T22 = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		T23 = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		T24 = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		T25 = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		T26 = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		T27 = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_16_1[11]));

		TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_16_1[11]));
		TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_16_1[11]));

		T30 = _mm_sub_epi32(T20, T21);
		T31 = _mm_sub_epi32(T22, T23);
		T32 = _mm_sub_epi32(T24, T25);
		T33 = _mm_sub_epi32(T26, T27);

		TT30 = _mm_sub_epi32(TT20, TT21);
		TT31 = _mm_sub_epi32(TT22, TT23);
		TT32 = _mm_sub_epi32(TT24, TT25);
		TT33 = _mm_sub_epi32(TT26, TT27);

		T30 = _mm_hadd_epi32(T30, T31);
		T31 = _mm_hadd_epi32(T32, T33);

		TT30 = _mm_hadd_epi32(TT30, TT31);
		TT31 = _mm_hadd_epi32(TT32, TT33);

		T40 = _mm_hadd_epi32(T30, T31);
		T40 = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2);
		TT40 = _mm_hadd_epi32(TT30, TT31);
		TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2);
		T70 = _mm_packs_epi32(T40, TT40);
		_mm_storeu_si128((__m128i*)&dst[6 * 16 + i], T70);

		T20 = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		T21 = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		T22 = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		T23 = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		T24 = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		T25 = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		T26 = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		T27 = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_16_1[12]));

		TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_16_1[12]));
		TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_16_1[12]));

		T30 = _mm_sub_epi32(T20, T21);
		T31 = _mm_sub_epi32(T22, T23);
		T32 = _mm_sub_epi32(T24, T25);
		T33 = _mm_sub_epi32(T26, T27);

		TT30 = _mm_sub_epi32(TT20, TT21);
		TT31 = _mm_sub_epi32(TT22, TT23);
		TT32 = _mm_sub_epi32(TT24, TT25);
		TT33 = _mm_sub_epi32(TT26, TT27);

		T30 = _mm_hadd_epi32(T30, T31);
		T31 = _mm_hadd_epi32(T32, T33);

		TT30 = _mm_hadd_epi32(TT30, TT31);
		TT31 = _mm_hadd_epi32(TT32, TT33);

		T40 = _mm_hadd_epi32(T30, T31);
		T40 = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2);
		TT40 = _mm_hadd_epi32(TT30, TT31);
		TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2);
		T70 = _mm_packs_epi32(T40, TT40);
		_mm_storeu_si128((__m128i*)&dst[10 * 16 + i], T70);

		T20 = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		T21 = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		T22 = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		T23 = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		T24 = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		T25 = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		T26 = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		T27 = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_16_1[13]));

		TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_16_1[13]));
		TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_16_1[13]));

		T30 = _mm_sub_epi32(T20, T21);
		T31 = _mm_sub_epi32(T22, T23);
		T32 = _mm_sub_epi32(T24, T25);
		T33 = _mm_sub_epi32(T26, T27);

		TT30 = _mm_sub_epi32(TT20, TT21);
		TT31 = _mm_sub_epi32(TT22, TT23);
		TT32 = _mm_sub_epi32(TT24, TT25);
		TT33 = _mm_sub_epi32(TT26, TT27);

		T30 = _mm_hadd_epi32(T30, T31);
		T31 = _mm_hadd_epi32(T32, T33);

		TT30 = _mm_hadd_epi32(TT30, TT31);
		TT31 = _mm_hadd_epi32(TT32, TT33);

		T40 = _mm_hadd_epi32(T30, T31);
		T40 = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2);
		TT40 = _mm_hadd_epi32(TT30, TT31);
		TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2);
		T70 = _mm_packs_epi32(T40, TT40);
		_mm_storeu_si128((__m128i*)&dst[14 * 16 + i], T70);

#define MAKE_ODD(tab, dstPos) \
		    T20  = _mm_madd_epi16(T10, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)]));       /* [*O2_0 *O1_0 *O3_0 *O0_0] */ \
		    T21  = _mm_madd_epi16(T11, _mm_load_si128((__m128i*)tab_dct_16_1[(tab) + 1]));   /* [*O5_0 *O6_0 *O4_0 *O7_0] */ \
		    T22  = _mm_madd_epi16(T12, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
		    T23  = _mm_madd_epi16(T13, _mm_load_si128((__m128i*)tab_dct_16_1[(tab) + 1])); \
		    T24  = _mm_madd_epi16(T14, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
		    T25  = _mm_madd_epi16(T15, _mm_load_si128((__m128i*)tab_dct_16_1[(tab) + 1])); \
		    T26  = _mm_madd_epi16(T16, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
		    T27  = _mm_madd_epi16(T17, _mm_load_si128((__m128i*)tab_dct_16_1[(tab) + 1])); \
		        \
		    T30  = _mm_add_epi32(T20, T21); \
		    T31  = _mm_add_epi32(T22, T23); \
		    T32  = _mm_add_epi32(T24, T25); \
		    T33  = _mm_add_epi32(T26, T27); \
		        \
		    T30  = _mm_hadd_epi32(T30, T31); \
		    T31  = _mm_hadd_epi32(T32, T33); \
		        \
		    T40  = _mm_hadd_epi32(T30, T31); \
		    T40  = _mm_srai_epi32(_mm_add_epi32(T40, c_256), shift2); \
				\
			TT20 = _mm_madd_epi16(TT10, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)]));\
			TT21 = _mm_madd_epi16(TT11, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)+1]));\
			TT22 = _mm_madd_epi16(TT12, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
			TT23 = _mm_madd_epi16(TT13, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)+1])); \
			TT24 = _mm_madd_epi16(TT14, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
			TT25 = _mm_madd_epi16(TT15, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)+1])); \
			TT26 = _mm_madd_epi16(TT16, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)])); \
			TT27 = _mm_madd_epi16(TT17, _mm_load_si128((__m128i*)tab_dct_16_1[(tab)+1])); \
			\
			TT30 = _mm_add_epi32(TT20, TT21); \
			TT31 = _mm_add_epi32(TT22, TT23); \
			TT32 = _mm_add_epi32(TT24, TT25); \
			TT33 = _mm_add_epi32(TT26, TT27); \
			\
			TT30 = _mm_hadd_epi32(TT30, TT31); \
			TT31 = _mm_hadd_epi32(TT32, TT33); \
			\
			TT40 = _mm_hadd_epi32(TT30, TT31); \
			TT40 = _mm_srai_epi32(_mm_add_epi32(TT40, c_256), shift2); \
			T70 = _mm_packs_epi32(T40, TT40);\
		    _mm_storeu_si128((__m128i*)&dst[(dstPos) * 16 + i], T70);

		MAKE_ODD(14, 1);
		MAKE_ODD(16, 3);
		MAKE_ODD(18, 5);
		MAKE_ODD(20, 7);
		MAKE_ODD(22, 9);
		MAKE_ODD(24, 11);
		MAKE_ODD(26, 13);
		MAKE_ODD(28, 15);
#undef MAKE_ODD
	}
}

void sub_trans_16x16_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift)
{
	// Const
	__m256i c_2 = _mm256_set1_epi32(2);	//shift2 = 9
	__m256i c_256 = _mm256_set1_epi32(256);	//shift2 = 9
	int shift1 = 2;
	int shift2 = 9;

	__m256i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m256i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m256i T10, T11, T12, T13, T14, T15, T16, T17;
	__m256i T20, T21, T22, T23, T24, T25, T26, T27;
	__m256i T30, T31, T32, T33, T34, T35, T36, T37;
	__m256i T40, T41, T42, T43, T44, T45, T46, T47;
	__m256i T50, T51, T52, T53;
	__m256i T60, T61, T62, T63, T64, T65, T66, T67;
	__m256i TT10, TT11, TT12, TT13, TT14, TT15, TT16, TT17;
	__m256i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m256i TT30, TT31, TT32, TT33;
	__m256i TT40, TT41;
	__m256i T70;

	__m256i O00, O01, O02, O03, O04, O05, O06, O07, O08, O09, O10, O11, O12, O13, O14, O15;
	__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m256i P00, P01, P02, P03, P04, P05, P06, P07, P08, P09, P10, P11, P12, P13, P14, P15;
	__m256i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m256i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;

	__m256i im[16];
	__m256i tmpZero = _mm256_setzero_si256();
	__m256i TAB16_0, TAB16_1, TAB8_1, TAB8_5, TAB8_6, TAB8_7, TAB8_8, TAB16_0_2, TAB16_0_3, TAB16_1_8, TAB16_1_9, TAB16_1_10, TAB16_1_11, TAB16_1_12, TAB16_1_13;

	// DCT1
	//input data
    O00 = _mm256_loadu2_m128i((const __m128i*)&org[8 * i_org], (const __m128i*)&org[0]);
    O01 = _mm256_loadu2_m128i((const __m128i*)&org[9 * i_org], (const __m128i*)&org[1 * i_org]);
    O02 = _mm256_loadu2_m128i((const __m128i*)&org[10 * i_org], (const __m128i*)&org[2 * i_org]);
    O03 = _mm256_loadu2_m128i((const __m128i*)&org[11 * i_org], (const __m128i*)&org[3 * i_org]);
    O04 = _mm256_loadu2_m128i((const __m128i*)&org[12 * i_org], (const __m128i*)&org[4 * i_org]);
    O05 = _mm256_loadu2_m128i((const __m128i*)&org[13 * i_org], (const __m128i*)&org[5 * i_org]);
    O06 = _mm256_loadu2_m128i((const __m128i*)&org[14 * i_org], (const __m128i*)&org[6 * i_org]);
    O07 = _mm256_loadu2_m128i((const __m128i*)&org[15 * i_org], (const __m128i*)&org[7 * i_org]);

    P00 = _mm256_loadu2_m128i((const __m128i*)&pred[8 * i_pred], (const __m128i*)&pred[0]);
    P01 = _mm256_loadu2_m128i((const __m128i*)&pred[9 * i_pred], (const __m128i*)&pred[1 * i_pred]);
    P02 = _mm256_loadu2_m128i((const __m128i*)&pred[10 * i_pred], (const __m128i*)&pred[2 * i_pred]);
    P03 = _mm256_loadu2_m128i((const __m128i*)&pred[11 * i_pred], (const __m128i*)&pred[3 * i_pred]);
    P04 = _mm256_loadu2_m128i((const __m128i*)&pred[12 * i_pred], (const __m128i*)&pred[4 * i_pred]);
    P05 = _mm256_loadu2_m128i((const __m128i*)&pred[13 * i_pred], (const __m128i*)&pred[5 * i_pred]);
    P06 = _mm256_loadu2_m128i((const __m128i*)&pred[14 * i_pred], (const __m128i*)&pred[6 * i_pred]);
    P07 = _mm256_loadu2_m128i((const __m128i*)&pred[15 * i_pred], (const __m128i*)&pred[7 * i_pred]);

	O00A = _mm256_unpacklo_epi8(O00, tmpZero);
	O00B = _mm256_unpackhi_epi8(O00, tmpZero);
	O01A = _mm256_unpacklo_epi8(O01, tmpZero);
	O01B = _mm256_unpackhi_epi8(O01, tmpZero);
	O02A = _mm256_unpacklo_epi8(O02, tmpZero);
	O02B = _mm256_unpackhi_epi8(O02, tmpZero);
	O03A = _mm256_unpacklo_epi8(O03, tmpZero);
	O03B = _mm256_unpackhi_epi8(O03, tmpZero);
	O04A = _mm256_unpacklo_epi8(O04, tmpZero);
	O04B = _mm256_unpackhi_epi8(O04, tmpZero);
	O05A = _mm256_unpacklo_epi8(O05, tmpZero);
	O05B = _mm256_unpackhi_epi8(O05, tmpZero);
	O06A = _mm256_unpacklo_epi8(O06, tmpZero);
	O06B = _mm256_unpackhi_epi8(O06, tmpZero);
	O07A = _mm256_unpacklo_epi8(O07, tmpZero);
	O07B = _mm256_unpackhi_epi8(O07, tmpZero);

	P00A = _mm256_unpacklo_epi8(P00, tmpZero);
	P00B = _mm256_unpackhi_epi8(P00, tmpZero);
	P01A = _mm256_unpacklo_epi8(P01, tmpZero);
	P01B = _mm256_unpackhi_epi8(P01, tmpZero);
	P02A = _mm256_unpacklo_epi8(P02, tmpZero);
	P02B = _mm256_unpackhi_epi8(P02, tmpZero);
	P03A = _mm256_unpacklo_epi8(P03, tmpZero);
	P03B = _mm256_unpackhi_epi8(P03, tmpZero);
	P04A = _mm256_unpacklo_epi8(P04, tmpZero);
	P04B = _mm256_unpackhi_epi8(P04, tmpZero);
	P05A = _mm256_unpacklo_epi8(P05, tmpZero);
	P05B = _mm256_unpackhi_epi8(P05, tmpZero);
	P06A = _mm256_unpacklo_epi8(P06, tmpZero);
	P06B = _mm256_unpackhi_epi8(P06, tmpZero);
	P07A = _mm256_unpacklo_epi8(P07, tmpZero);
	P07B = _mm256_unpackhi_epi8(P07, tmpZero);

	T00A = _mm256_sub_epi16(O00A, P00A);
	T00B = _mm256_sub_epi16(O00B, P00B);
	T01A = _mm256_sub_epi16(O01A, P01A);
	T01B = _mm256_sub_epi16(O01B, P01B);
	T02A = _mm256_sub_epi16(O02A, P02A);
	T02B = _mm256_sub_epi16(O02B, P02B);
	T03A = _mm256_sub_epi16(O03A, P03A);
	T03B = _mm256_sub_epi16(O03B, P03B);
	T04A = _mm256_sub_epi16(O04A, P04A);
	T04B = _mm256_sub_epi16(O04B, P04B);
	T05A = _mm256_sub_epi16(O05A, P05A);
	T05B = _mm256_sub_epi16(O05B, P05B);
	T06A = _mm256_sub_epi16(O06A, P06A);
	T06B = _mm256_sub_epi16(O06B, P06B);
	T07A = _mm256_sub_epi16(O07A, P07A);
	T07B = _mm256_sub_epi16(O07B, P07B);

	//shuffle
	TAB16_0 = _mm256_loadu_si256((__m256i*)tab_dct_16_0_256i[0]);
	T00B = _mm256_shuffle_epi8(T00B, TAB16_0);
	T01B = _mm256_shuffle_epi8(T01B, TAB16_0);
	T02B = _mm256_shuffle_epi8(T02B, TAB16_0);
	T03B = _mm256_shuffle_epi8(T03B, TAB16_0);
	T04B = _mm256_shuffle_epi8(T04B, TAB16_0);
	T05B = _mm256_shuffle_epi8(T05B, TAB16_0);
	T06B = _mm256_shuffle_epi8(T06B, TAB16_0);
	T07B = _mm256_shuffle_epi8(T07B, TAB16_0);

	T10 = _mm256_add_epi16(T00A, T00B);
	T11 = _mm256_add_epi16(T01A, T01B);
	T12 = _mm256_add_epi16(T02A, T02B);
	T13 = _mm256_add_epi16(T03A, T03B);
	T14 = _mm256_add_epi16(T04A, T04B);
	T15 = _mm256_add_epi16(T05A, T05B);
	T16 = _mm256_add_epi16(T06A, T06B);
	T17 = _mm256_add_epi16(T07A, T07B);

	T20 = _mm256_sub_epi16(T00A, T00B);
	T21 = _mm256_sub_epi16(T01A, T01B);
	T22 = _mm256_sub_epi16(T02A, T02B);
	T23 = _mm256_sub_epi16(T03A, T03B);
	T24 = _mm256_sub_epi16(T04A, T04B);
	T25 = _mm256_sub_epi16(T05A, T05B);
	T26 = _mm256_sub_epi16(T06A, T06B);
	T27 = _mm256_sub_epi16(T07A, T07B);

	TAB16_1 = _mm256_loadu_si256((__m256i*)tab_dct_16_0_256i[1]);
	T30 = _mm256_shuffle_epi8(T10, TAB16_1);
	T31 = _mm256_shuffle_epi8(T11, TAB16_1);
	T32 = _mm256_shuffle_epi8(T12, TAB16_1);
	T33 = _mm256_shuffle_epi8(T13, TAB16_1);
	T34 = _mm256_shuffle_epi8(T14, TAB16_1);
	T35 = _mm256_shuffle_epi8(T15, TAB16_1);
	T36 = _mm256_shuffle_epi8(T16, TAB16_1);
	T37 = _mm256_shuffle_epi8(T17, TAB16_1);

	T40 = _mm256_hadd_epi16(T30, T31);
	T41 = _mm256_hadd_epi16(T32, T33);
	T42 = _mm256_hadd_epi16(T34, T35);
	T43 = _mm256_hadd_epi16(T36, T37);
	T44 = _mm256_hsub_epi16(T30, T31);
	T45 = _mm256_hsub_epi16(T32, T33);
	T46 = _mm256_hsub_epi16(T34, T35);
	T47 = _mm256_hsub_epi16(T36, T37);

	T50 = _mm256_hadd_epi16(T40, T41);
	T51 = _mm256_hadd_epi16(T42, T43);
	T52 = _mm256_hsub_epi16(T40, T41);
	T53 = _mm256_hsub_epi16(T42, T43);

	T60 = _mm256_madd_epi16(T50, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[1]));
	T61 = _mm256_madd_epi16(T51, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[1]));
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[0] = T70;

	T60 = _mm256_madd_epi16(T50, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[2]));
	T61 = _mm256_madd_epi16(T51, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[2]));
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[8] = T70;

	T60 = _mm256_madd_epi16(T52, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[3]));
	T61 = _mm256_madd_epi16(T53, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[3]));
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[4] = T70;

	T60 = _mm256_madd_epi16(T52, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[4]));
	T61 = _mm256_madd_epi16(T53, _mm256_loadu_si256((__m256i*)tab_dct_8_256i[4]));
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[12] = T70;

	TAB8_5 = _mm256_loadu_si256((__m256i*)tab_dct_8_256i[5]);
	T60 = _mm256_madd_epi16(T44, TAB8_5);
	T61 = _mm256_madd_epi16(T45, TAB8_5);
	T62 = _mm256_madd_epi16(T46, TAB8_5);
	T63 = _mm256_madd_epi16(T47, TAB8_5);
	T60 = _mm256_hadd_epi32(T60, T61);
	T61 = _mm256_hadd_epi32(T62, T63);
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[2] = T70;

	TAB8_6 = _mm256_loadu_si256((__m256i*)tab_dct_8_256i[6]);
	T60 = _mm256_madd_epi16(T44, TAB8_6);
	T61 = _mm256_madd_epi16(T45, TAB8_6);
	T62 = _mm256_madd_epi16(T46, TAB8_6);
	T63 = _mm256_madd_epi16(T47, TAB8_6);
	T60 = _mm256_hadd_epi32(T60, T61);
	T61 = _mm256_hadd_epi32(T62, T63);
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[6] = T70;

	TAB8_7 = _mm256_loadu_si256((__m256i*)tab_dct_8_256i[7]);
	T60 = _mm256_madd_epi16(T44, TAB8_7);
	T61 = _mm256_madd_epi16(T45, TAB8_7);
	T62 = _mm256_madd_epi16(T46, TAB8_7);
	T63 = _mm256_madd_epi16(T47, TAB8_7);
	T60 = _mm256_hadd_epi32(T60, T61);
	T61 = _mm256_hadd_epi32(T62, T63);
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[10] = T70;

	TAB8_8 = _mm256_loadu_si256((__m256i*)tab_dct_8_256i[8]);
	T60 = _mm256_madd_epi16(T44, TAB8_8);
	T61 = _mm256_madd_epi16(T45, TAB8_8);
	T62 = _mm256_madd_epi16(T46, TAB8_8);
	T63 = _mm256_madd_epi16(T47, TAB8_8);
	T60 = _mm256_hadd_epi32(T60, T61);
	T61 = _mm256_hadd_epi32(T62, T63);
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1);
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1);
	T70 = _mm256_packs_epi32(T60, T61);
	im[14] = T70;

#define MAKE_ODD(tab, dstPos) \
	T60 = _mm256_madd_epi16(T20, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T61 = _mm256_madd_epi16(T21, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T62 = _mm256_madd_epi16(T22, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T63 = _mm256_madd_epi16(T23, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T64 = _mm256_madd_epi16(T24, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T65 = _mm256_madd_epi16(T25, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T66 = _mm256_madd_epi16(T26, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T67 = _mm256_madd_epi16(T27, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	T62 = _mm256_hadd_epi32(T64, T65); \
	T63 = _mm256_hadd_epi32(T66, T67); \
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_2), shift1); \
	T61 = _mm256_srai_epi32(_mm256_add_epi32(T61, c_2), shift1); \
	T70 = _mm256_packs_epi32(T60, T61); \
	im[dstPos] = T70;

	MAKE_ODD(0, 1);
	MAKE_ODD(1, 3);
	MAKE_ODD(2, 5);
	MAKE_ODD(3, 7);
	MAKE_ODD(4, 9);
	MAKE_ODD(5, 11);
	MAKE_ODD(6, 13);
	MAKE_ODD(7, 15);
#undef MAKE_ODD

	// DCT2
	T00A = _mm256_permute2x128_si256(im[0], im[8], 0x20);
	T00B = _mm256_permute2x128_si256(im[0], im[8], 0x31);
	T01A = _mm256_permute2x128_si256(im[1], im[9], 0x20);
	T01B = _mm256_permute2x128_si256(im[1], im[9], 0x31);
	T02A = _mm256_permute2x128_si256(im[2], im[10], 0x20);
	T02B = _mm256_permute2x128_si256(im[2], im[10], 0x31);
	T03A = _mm256_permute2x128_si256(im[3], im[11], 0x20);
	T03B = _mm256_permute2x128_si256(im[3], im[11], 0x31);
	T04A = _mm256_permute2x128_si256(im[4], im[12], 0x20);
	T04B = _mm256_permute2x128_si256(im[4], im[12], 0x31);
	T05A = _mm256_permute2x128_si256(im[5], im[13], 0x20);
	T05B = _mm256_permute2x128_si256(im[5], im[13], 0x31);
	T06A = _mm256_permute2x128_si256(im[6], im[14], 0x20);
	T06B = _mm256_permute2x128_si256(im[6], im[14], 0x31);
	T07A = _mm256_permute2x128_si256(im[7], im[15], 0x20);
	T07B = _mm256_permute2x128_si256(im[7], im[15], 0x31);

	TAB16_0_2 = _mm256_loadu_si256((__m256i*)tab_dct_16_0_256i[2]);
	TAB16_0_3 = _mm256_loadu_si256((__m256i*)tab_dct_16_0_256i[3]);
	T00A = _mm256_shuffle_epi8(T00A, TAB16_0_2);
	T00B = _mm256_shuffle_epi8(T00B, TAB16_0_3);
	T01A = _mm256_shuffle_epi8(T01A, TAB16_0_2);
	T01B = _mm256_shuffle_epi8(T01B, TAB16_0_3);
	T02A = _mm256_shuffle_epi8(T02A, TAB16_0_2);
	T02B = _mm256_shuffle_epi8(T02B, TAB16_0_3);
	T03A = _mm256_shuffle_epi8(T03A, TAB16_0_2);
	T03B = _mm256_shuffle_epi8(T03B, TAB16_0_3);

	T04A = _mm256_shuffle_epi8(T04A, TAB16_0_2);
	T04B = _mm256_shuffle_epi8(T04B, TAB16_0_3);
	T05A = _mm256_shuffle_epi8(T05A, TAB16_0_2);
	T05B = _mm256_shuffle_epi8(T05B, TAB16_0_3);
	T06A = _mm256_shuffle_epi8(T06A, TAB16_0_2);
	T06B = _mm256_shuffle_epi8(T06B, TAB16_0_3);
	T07A = _mm256_shuffle_epi8(T07A, TAB16_0_2);
	T07B = _mm256_shuffle_epi8(T07B, TAB16_0_3);

	T10 = _mm256_unpacklo_epi16(T00A, T00B);
	T11 = _mm256_unpackhi_epi16(T00A, T00B);
	T12 = _mm256_unpacklo_epi16(T01A, T01B);
	T13 = _mm256_unpackhi_epi16(T01A, T01B);
	T14 = _mm256_unpacklo_epi16(T02A, T02B);
	T15 = _mm256_unpackhi_epi16(T02A, T02B);
	T16 = _mm256_unpacklo_epi16(T03A, T03B);
	T17 = _mm256_unpackhi_epi16(T03A, T03B);

	TT10 = _mm256_unpacklo_epi16(T04A, T04B);
	TT11 = _mm256_unpackhi_epi16(T04A, T04B);
	TT12 = _mm256_unpacklo_epi16(T05A, T05B);
	TT13 = _mm256_unpackhi_epi16(T05A, T05B);
	TT14 = _mm256_unpacklo_epi16(T06A, T06B);
	TT15 = _mm256_unpackhi_epi16(T06A, T06B);
	TT16 = _mm256_unpacklo_epi16(T07A, T07B);
	TT17 = _mm256_unpackhi_epi16(T07A, T07B);

	TAB8_1 = _mm256_loadu_si256((__m256i*)tab_dct_8_256i[1]);
	T20 = _mm256_madd_epi16(T10, TAB8_1);
	T21 = _mm256_madd_epi16(T11, TAB8_1);
	T22 = _mm256_madd_epi16(T12, TAB8_1);
	T23 = _mm256_madd_epi16(T13, TAB8_1);
	T24 = _mm256_madd_epi16(T14, TAB8_1);
	T25 = _mm256_madd_epi16(T15, TAB8_1);
	T26 = _mm256_madd_epi16(T16, TAB8_1);
	T27 = _mm256_madd_epi16(T17, TAB8_1);

	TT20 = _mm256_madd_epi16(TT10, TAB8_1);
	TT21 = _mm256_madd_epi16(TT11, TAB8_1);
	TT22 = _mm256_madd_epi16(TT12, TAB8_1);
	TT23 = _mm256_madd_epi16(TT13, TAB8_1);
	TT24 = _mm256_madd_epi16(TT14, TAB8_1);
	TT25 = _mm256_madd_epi16(TT15, TAB8_1);
	TT26 = _mm256_madd_epi16(TT16, TAB8_1);
	TT27 = _mm256_madd_epi16(TT17, TAB8_1);

	T30 = _mm256_add_epi32(T20, T21);
	T31 = _mm256_add_epi32(T22, T23);
	T32 = _mm256_add_epi32(T24, T25);
	T33 = _mm256_add_epi32(T26, T27);

	TT30 = _mm256_add_epi32(TT20, TT21);
	TT31 = _mm256_add_epi32(TT22, TT23);
	TT32 = _mm256_add_epi32(TT24, TT25);
	TT33 = _mm256_add_epi32(TT26, TT27);

	T30 = _mm256_hadd_epi32(T30, T31);
	T31 = _mm256_hadd_epi32(T32, T33);

	TT30 = _mm256_hadd_epi32(TT30, TT31);
	TT31 = _mm256_hadd_epi32(TT32, TT33);

	T40 = _mm256_hadd_epi32(T30, T31);
	T41 = _mm256_hsub_epi32(T30, T31);
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2);
	T41 = _mm256_srai_epi32(_mm256_add_epi32(T41, c_256), shift2);

	TT40 = _mm256_hadd_epi32(TT30, TT31);
	TT41 = _mm256_hsub_epi32(TT30, TT31);
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2);
	TT41 = _mm256_srai_epi32(_mm256_add_epi32(TT41, c_256), shift2);

	T70 = _mm256_packs_epi32(T40, TT40);
	_mm256_storeu_si256((__m256i*)&dst[0 * 16], T70);
	T70 = _mm256_packs_epi32(T41, TT41);
	_mm256_storeu_si256((__m256i*)&dst[8 * 16], T70);

	TAB16_1_8 = _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[8]);
	T20 = _mm256_madd_epi16(T10, TAB16_1_8);
	T21 = _mm256_madd_epi16(T11, TAB16_1_8);
	T22 = _mm256_madd_epi16(T12, TAB16_1_8);
	T23 = _mm256_madd_epi16(T13, TAB16_1_8);
	T24 = _mm256_madd_epi16(T14, TAB16_1_8);
	T25 = _mm256_madd_epi16(T15, TAB16_1_8);
	T26 = _mm256_madd_epi16(T16, TAB16_1_8);
	T27 = _mm256_madd_epi16(T17, TAB16_1_8);

	TT20 = _mm256_madd_epi16(TT10, TAB16_1_8);
	TT21 = _mm256_madd_epi16(TT11, TAB16_1_8);
	TT22 = _mm256_madd_epi16(TT12, TAB16_1_8);
	TT23 = _mm256_madd_epi16(TT13, TAB16_1_8);
	TT24 = _mm256_madd_epi16(TT14, TAB16_1_8);
	TT25 = _mm256_madd_epi16(TT15, TAB16_1_8);
	TT26 = _mm256_madd_epi16(TT16, TAB16_1_8);
	TT27 = _mm256_madd_epi16(TT17, TAB16_1_8);

	T30 = _mm256_add_epi32(T20, T21);
	T31 = _mm256_add_epi32(T22, T23);
	T32 = _mm256_add_epi32(T24, T25);
	T33 = _mm256_add_epi32(T26, T27);

	TT30 = _mm256_add_epi32(TT20, TT21);
	TT31 = _mm256_add_epi32(TT22, TT23);
	TT32 = _mm256_add_epi32(TT24, TT25);
	TT33 = _mm256_add_epi32(TT26, TT27);

	T30 = _mm256_hadd_epi32(T30, T31);
	T31 = _mm256_hadd_epi32(T32, T33);

	TT30 = _mm256_hadd_epi32(TT30, TT31);
	TT31 = _mm256_hadd_epi32(TT32, TT33);

	T40 = _mm256_hadd_epi32(T30, T31);
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2);

	TT40 = _mm256_hadd_epi32(TT30, TT31);
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2);
	T70 = _mm256_packs_epi32(T40, TT40);
	_mm256_storeu_si256((__m256i*)&dst[4 * 16], T70);

	TAB16_1_9 = _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[9]);
	T20 = _mm256_madd_epi16(T10, TAB16_1_9);
	T21 = _mm256_madd_epi16(T11, TAB16_1_9);
	T22 = _mm256_madd_epi16(T12, TAB16_1_9);
	T23 = _mm256_madd_epi16(T13, TAB16_1_9);
	T24 = _mm256_madd_epi16(T14, TAB16_1_9);
	T25 = _mm256_madd_epi16(T15, TAB16_1_9);
	T26 = _mm256_madd_epi16(T16, TAB16_1_9);
	T27 = _mm256_madd_epi16(T17, TAB16_1_9);

	TT20 = _mm256_madd_epi16(TT10, TAB16_1_9);
	TT21 = _mm256_madd_epi16(TT11, TAB16_1_9);
	TT22 = _mm256_madd_epi16(TT12, TAB16_1_9);
	TT23 = _mm256_madd_epi16(TT13, TAB16_1_9);
	TT24 = _mm256_madd_epi16(TT14, TAB16_1_9);
	TT25 = _mm256_madd_epi16(TT15, TAB16_1_9);
	TT26 = _mm256_madd_epi16(TT16, TAB16_1_9);
	TT27 = _mm256_madd_epi16(TT17, TAB16_1_9);

	T30 = _mm256_add_epi32(T20, T21);
	T31 = _mm256_add_epi32(T22, T23);
	T32 = _mm256_add_epi32(T24, T25);
	T33 = _mm256_add_epi32(T26, T27);

	TT30 = _mm256_add_epi32(TT20, TT21);
	TT31 = _mm256_add_epi32(TT22, TT23);
	TT32 = _mm256_add_epi32(TT24, TT25);
	TT33 = _mm256_add_epi32(TT26, TT27);

	T30 = _mm256_hadd_epi32(T30, T31);
	T31 = _mm256_hadd_epi32(T32, T33);

	TT30 = _mm256_hadd_epi32(TT30, TT31);
	TT31 = _mm256_hadd_epi32(TT32, TT33);

	T40 = _mm256_hadd_epi32(T30, T31);
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2);
	TT40 = _mm256_hadd_epi32(TT30, TT31);
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2);
	T70 = _mm256_packs_epi32(T40, TT40);
	_mm256_storeu_si256((__m256i*)&dst[12 * 16], T70);

	TAB16_1_10 = _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[10]);
	T20 = _mm256_madd_epi16(T10, TAB16_1_10);
	T21 = _mm256_madd_epi16(T11, TAB16_1_10);
	T22 = _mm256_madd_epi16(T12, TAB16_1_10);
	T23 = _mm256_madd_epi16(T13, TAB16_1_10);
	T24 = _mm256_madd_epi16(T14, TAB16_1_10);
	T25 = _mm256_madd_epi16(T15, TAB16_1_10);
	T26 = _mm256_madd_epi16(T16, TAB16_1_10);
	T27 = _mm256_madd_epi16(T17, TAB16_1_10);

	TT20 = _mm256_madd_epi16(TT10, TAB16_1_10);
	TT21 = _mm256_madd_epi16(TT11, TAB16_1_10);
	TT22 = _mm256_madd_epi16(TT12, TAB16_1_10);
	TT23 = _mm256_madd_epi16(TT13, TAB16_1_10);
	TT24 = _mm256_madd_epi16(TT14, TAB16_1_10);
	TT25 = _mm256_madd_epi16(TT15, TAB16_1_10);
	TT26 = _mm256_madd_epi16(TT16, TAB16_1_10);
	TT27 = _mm256_madd_epi16(TT17, TAB16_1_10);

	T30 = _mm256_sub_epi32(T20, T21);
	T31 = _mm256_sub_epi32(T22, T23);
	T32 = _mm256_sub_epi32(T24, T25);
	T33 = _mm256_sub_epi32(T26, T27);

	TT30 = _mm256_sub_epi32(TT20, TT21);
	TT31 = _mm256_sub_epi32(TT22, TT23);
	TT32 = _mm256_sub_epi32(TT24, TT25);
	TT33 = _mm256_sub_epi32(TT26, TT27);

	T30 = _mm256_hadd_epi32(T30, T31);
	T31 = _mm256_hadd_epi32(T32, T33);

	TT30 = _mm256_hadd_epi32(TT30, TT31);
	TT31 = _mm256_hadd_epi32(TT32, TT33);

	T40 = _mm256_hadd_epi32(T30, T31);
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2);
	TT40 = _mm256_hadd_epi32(TT30, TT31);
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2);
	T70 = _mm256_packs_epi32(T40, TT40);
	_mm256_storeu_si256((__m256i*)&dst[2 * 16], T70);

	TAB16_1_11 = _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[11]);
	T20 = _mm256_madd_epi16(T10, TAB16_1_11);
	T21 = _mm256_madd_epi16(T11, TAB16_1_11);
	T22 = _mm256_madd_epi16(T12, TAB16_1_11);
	T23 = _mm256_madd_epi16(T13, TAB16_1_11);
	T24 = _mm256_madd_epi16(T14, TAB16_1_11);
	T25 = _mm256_madd_epi16(T15, TAB16_1_11);
	T26 = _mm256_madd_epi16(T16, TAB16_1_11);
	T27 = _mm256_madd_epi16(T17, TAB16_1_11);

	TT20 = _mm256_madd_epi16(TT10, TAB16_1_11);
	TT21 = _mm256_madd_epi16(TT11, TAB16_1_11);
	TT22 = _mm256_madd_epi16(TT12, TAB16_1_11);
	TT23 = _mm256_madd_epi16(TT13, TAB16_1_11);
	TT24 = _mm256_madd_epi16(TT14, TAB16_1_11);
	TT25 = _mm256_madd_epi16(TT15, TAB16_1_11);
	TT26 = _mm256_madd_epi16(TT16, TAB16_1_11);
	TT27 = _mm256_madd_epi16(TT17, TAB16_1_11);

	T30 = _mm256_sub_epi32(T20, T21);
	T31 = _mm256_sub_epi32(T22, T23);
	T32 = _mm256_sub_epi32(T24, T25);
	T33 = _mm256_sub_epi32(T26, T27);

	TT30 = _mm256_sub_epi32(TT20, TT21);
	TT31 = _mm256_sub_epi32(TT22, TT23);
	TT32 = _mm256_sub_epi32(TT24, TT25);
	TT33 = _mm256_sub_epi32(TT26, TT27);

	T30 = _mm256_hadd_epi32(T30, T31);
	T31 = _mm256_hadd_epi32(T32, T33);

	TT30 = _mm256_hadd_epi32(TT30, TT31);
	TT31 = _mm256_hadd_epi32(TT32, TT33);

	T40 = _mm256_hadd_epi32(T30, T31);
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2);
	TT40 = _mm256_hadd_epi32(TT30, TT31);
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2);
	T70 = _mm256_packs_epi32(T40, TT40);
	_mm256_storeu_si256((__m256i*)&dst[6 * 16], T70);

	TAB16_1_12 = _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[12]);
	T20 = _mm256_madd_epi16(T10, TAB16_1_12);
	T21 = _mm256_madd_epi16(T11, TAB16_1_12);
	T22 = _mm256_madd_epi16(T12, TAB16_1_12);
	T23 = _mm256_madd_epi16(T13, TAB16_1_12);
	T24 = _mm256_madd_epi16(T14, TAB16_1_12);
	T25 = _mm256_madd_epi16(T15, TAB16_1_12);
	T26 = _mm256_madd_epi16(T16, TAB16_1_12);
	T27 = _mm256_madd_epi16(T17, TAB16_1_12);

	TT20 = _mm256_madd_epi16(TT10, TAB16_1_12);
	TT21 = _mm256_madd_epi16(TT11, TAB16_1_12);
	TT22 = _mm256_madd_epi16(TT12, TAB16_1_12);
	TT23 = _mm256_madd_epi16(TT13, TAB16_1_12);
	TT24 = _mm256_madd_epi16(TT14, TAB16_1_12);
	TT25 = _mm256_madd_epi16(TT15, TAB16_1_12);
	TT26 = _mm256_madd_epi16(TT16, TAB16_1_12);
	TT27 = _mm256_madd_epi16(TT17, TAB16_1_12);

	T30 = _mm256_sub_epi32(T20, T21);
	T31 = _mm256_sub_epi32(T22, T23);
	T32 = _mm256_sub_epi32(T24, T25);
	T33 = _mm256_sub_epi32(T26, T27);

	TT30 = _mm256_sub_epi32(TT20, TT21);
	TT31 = _mm256_sub_epi32(TT22, TT23);
	TT32 = _mm256_sub_epi32(TT24, TT25);
	TT33 = _mm256_sub_epi32(TT26, TT27);

	T30 = _mm256_hadd_epi32(T30, T31);
	T31 = _mm256_hadd_epi32(T32, T33);

	TT30 = _mm256_hadd_epi32(TT30, TT31);
	TT31 = _mm256_hadd_epi32(TT32, TT33);

	T40 = _mm256_hadd_epi32(T30, T31);
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2);
	TT40 = _mm256_hadd_epi32(TT30, TT31);
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2);
	T70 = _mm256_packs_epi32(T40, TT40);
	_mm256_storeu_si256((__m256i*)&dst[10 * 16], T70);

	TAB16_1_13 = _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[13]);
	T20 = _mm256_madd_epi16(T10, TAB16_1_13);
	T21 = _mm256_madd_epi16(T11, TAB16_1_13);
	T22 = _mm256_madd_epi16(T12, TAB16_1_13);
	T23 = _mm256_madd_epi16(T13, TAB16_1_13);
	T24 = _mm256_madd_epi16(T14, TAB16_1_13);
	T25 = _mm256_madd_epi16(T15, TAB16_1_13);
	T26 = _mm256_madd_epi16(T16, TAB16_1_13);
	T27 = _mm256_madd_epi16(T17, TAB16_1_13);

	TT20 = _mm256_madd_epi16(TT10, TAB16_1_13);
	TT21 = _mm256_madd_epi16(TT11, TAB16_1_13);
	TT22 = _mm256_madd_epi16(TT12, TAB16_1_13);
	TT23 = _mm256_madd_epi16(TT13, TAB16_1_13);
	TT24 = _mm256_madd_epi16(TT14, TAB16_1_13);
	TT25 = _mm256_madd_epi16(TT15, TAB16_1_13);
	TT26 = _mm256_madd_epi16(TT16, TAB16_1_13);
	TT27 = _mm256_madd_epi16(TT17, TAB16_1_13);

	T30 = _mm256_sub_epi32(T20, T21);
	T31 = _mm256_sub_epi32(T22, T23);
	T32 = _mm256_sub_epi32(T24, T25);
	T33 = _mm256_sub_epi32(T26, T27);

	TT30 = _mm256_sub_epi32(TT20, TT21);
	TT31 = _mm256_sub_epi32(TT22, TT23);
	TT32 = _mm256_sub_epi32(TT24, TT25);
	TT33 = _mm256_sub_epi32(TT26, TT27);

	T30 = _mm256_hadd_epi32(T30, T31);
	T31 = _mm256_hadd_epi32(T32, T33);

	TT30 = _mm256_hadd_epi32(TT30, TT31);
	TT31 = _mm256_hadd_epi32(TT32, TT33);

	T40 = _mm256_hadd_epi32(T30, T31);
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2);
	TT40 = _mm256_hadd_epi32(TT30, TT31);
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2);
	T70 = _mm256_packs_epi32(T40, TT40);
	_mm256_storeu_si256((__m256i*)&dst[14 * 16], T70);

#define MAKE_ODD(tab, dstPos) \
	T20 = _mm256_madd_epi16(T10, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)]));       /* [*O2_0 *O1_0 *O3_0 *O0_0] */ \
	T21 = _mm256_madd_epi16(T11, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1]));   /* [*O5_0 *O6_0 *O4_0 *O7_0] */ \
	T22 = _mm256_madd_epi16(T12, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T23 = _mm256_madd_epi16(T13, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1])); \
	T24 = _mm256_madd_epi16(T14, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T25 = _mm256_madd_epi16(T15, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1])); \
	T26 = _mm256_madd_epi16(T16, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	T27 = _mm256_madd_epi16(T17, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1])); \
	\
	T30 = _mm256_add_epi32(T20, T21); \
	T31 = _mm256_add_epi32(T22, T23); \
	T32 = _mm256_add_epi32(T24, T25); \
	T33 = _mm256_add_epi32(T26, T27); \
	\
	T30 = _mm256_hadd_epi32(T30, T31); \
	T31 = _mm256_hadd_epi32(T32, T33); \
	\
	T40 = _mm256_hadd_epi32(T30, T31); \
	T40 = _mm256_srai_epi32(_mm256_add_epi32(T40, c_256), shift2); \
	\
	TT20 = _mm256_madd_epi16(TT10, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	TT21 = _mm256_madd_epi16(TT11, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1])); \
	TT22 = _mm256_madd_epi16(TT12, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	TT23 = _mm256_madd_epi16(TT13, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1])); \
	TT24 = _mm256_madd_epi16(TT14, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	TT25 = _mm256_madd_epi16(TT15, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1])); \
	TT26 = _mm256_madd_epi16(TT16, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)])); \
	TT27 = _mm256_madd_epi16(TT17, _mm256_loadu_si256((__m256i*)tab_dct_16_1_256i[(tab)+1])); \
	\
	TT30 = _mm256_add_epi32(TT20, TT21); \
	TT31 = _mm256_add_epi32(TT22, TT23); \
	TT32 = _mm256_add_epi32(TT24, TT25); \
	TT33 = _mm256_add_epi32(TT26, TT27); \
	\
	TT30 = _mm256_hadd_epi32(TT30, TT31); \
	TT31 = _mm256_hadd_epi32(TT32, TT33); \
	\
	TT40 = _mm256_hadd_epi32(TT30, TT31); \
	TT40 = _mm256_srai_epi32(_mm256_add_epi32(TT40, c_256), shift2); \
	T70 = _mm256_packs_epi32(T40, TT40); \
	_mm256_storeu_si256((__m256i*)&dst[(dstPos)* 16], T70);

	MAKE_ODD(14, 1);
	MAKE_ODD(16, 3);
	MAKE_ODD(18, 5);
	MAKE_ODD(20, 7);
	MAKE_ODD(22, 9);
	MAKE_ODD(24, 11);
	MAKE_ODD(26, 13);
	MAKE_ODD(28, 15);
#undef MAKE_ODD
}

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// dct 32x32 ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
ALIGNED_32(static const coef_t tab_dct_32_0[][8]) =
{
	{ 0x0F0E, 0x0100, 0x0908, 0x0706, 0x0D0C, 0x0302, 0x0B0A, 0x0504 },  // 0
};

ALIGNED_32(static const coef_t tab_dct_32_0_256i[][16]) =
{
	{ 0x0F0E, 0x0100, 0x0908, 0x0706, 0x0D0C, 0x0302, 0x0B0A, 0x0504, 0x0F0E, 0x0100, 0x0908, 0x0706, 0x0D0C, 0x0302, 0x0B0A, 0x0504 },  // 0
};

ALIGNED_32(static const coef_t tab_dct_32_1[][8]) =
{
	{ 44, -44, 9, -9, 38, -38, 25, -25 },          //  0
	{ 38, -38, -25, 25, -9, 9, -44, 44 },          //  1
	{ 25, -25, 38, -38, -44, 44, 9, -9 },          //  2
	{ 9, -9, -44, 44, -25, 25, 38, -38 },          //  3

#define MAKE_COEF8(a0, a1, a2, a3, a4, a5, a6, a7) \
				    { (a0), (a7), (a3), (a4), (a1), (a6), (a2), (a5) \
				    }, \

	MAKE_COEF8(45, 43, 40, 35, 29, 21, 13, 4)
	MAKE_COEF8(43, 29, 4, -21, -40, -45, -35, -13)
	MAKE_COEF8(40, 4, -35, -43, -13, 29, 45, 21)
	MAKE_COEF8(35, -21, -43, 4, 45, 13, -40, -29)
	MAKE_COEF8(29, -40, -13, 45, -4, -43, 21, 35)
	MAKE_COEF8(21, -45, 29, 13, -43, 35, 4, -40)
	MAKE_COEF8(13, -35, 45, -40, 21, 4, -29, 43)
	MAKE_COEF8(4, -13, 21, -29, 35, -40, 43, -45)
#undef MAKE_COEF8

#define MAKE_COEF16(a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14, a15) \
			    { (a00), (a07), (a03), (a04), (a01), (a06), (a02), (a05) }, \
			    { (a15), (a08), (a12), (a11), (a14), (a09), (a13), (a10) },

	MAKE_COEF16(45, 45, 44, 43, 41, 39, 36, 34, 30, 27, 23, 19, 15, 11, 7, 2)    // 12
	MAKE_COEF16(45, 41, 34, 23, 11, -2, -15, -27, -36, -43, -45, -44, -39, -30, -19, -7)    // 14
	MAKE_COEF16(44, 34, 15, -7, -27, -41, -45, -39, -23, -2, 19, 36, 45, 43, 30, 11)    // 16
	MAKE_COEF16(43, 23, -7, -34, -45, -36, -11, 19, 41, 44, 27, -2, -30, -45, -39, -15)    // 18
	MAKE_COEF16(41, 11, -27, -45, -30, 7, 39, 43, 15, -23, -45, -34, 2, 36, 44, 19)    // 20
	MAKE_COEF16(39, -2, -41, -36, 7, 43, 34, -11, -44, -30, 15, 45, 27, -19, -45, -23)    // 22
	MAKE_COEF16(36, -15, -45, -11, 39, 34, -19, -45, -7, 41, 30, -23, -44, -2, 43, 27)    // 24
	MAKE_COEF16(34, -27, -39, 19, 43, -11, -45, 2, 45, 7, -44, -15, 41, 23, -36, -30)    // 26
	MAKE_COEF16(30, -36, -23, 41, 15, -44, -7, 45, -2, -45, 11, 43, -19, -39, 27, 34)    // 28
	MAKE_COEF16(27, -43, -2, 44, -23, -30, 41, 7, -45, 19, 34, -39, -11, 45, -15, -36)    // 30
	MAKE_COEF16(23, -45, 19, 27, -45, 15, 30, -44, 11, 34, -43, 7, 36, -41, 2, 39)    // 32
	MAKE_COEF16(19, -44, 36, -2, -34, 45, -23, -15, 43, -39, 7, 30, -45, 27, 11, -41)    // 34
	MAKE_COEF16(15, -39, 45, -30, 2, 27, -44, 41, -19, -11, 36, -45, 34, -7, -23, 43)    // 36
	MAKE_COEF16(11, -30, 43, -45, 36, -19, -2, 23, -39, 45, -41, 27, -7, -15, 34, -44)    // 38
	MAKE_COEF16(7, -19, 30, -39, 44, -45, 43, -36, 27, -15, 2, 11, -23, 34, -41, 45)    // 40
	MAKE_COEF16(2, -7, 11, -15, 19, -23, 27, -30, 34, -36, 39, -41, 43, -44, 45, -45)    // 42
#undef MAKE_COEF16

	{
		32, 32, 32, 32, 32, 32, 32, 32
	},		// 44

	{ 32, 32, -32, -32, -32, -32, 32, 32 },  // 45

	{ 42, 42, 17, 17, -17, -17, -42, -42 },  // 46
	{ -42, -42, -17, -17, 17, 17, 42, 42 },  // 47

	{ 17, 17, -42, -42, 42, 42, -17, -17 },  // 48
	{ -17, -17, 42, 42, -42, -42, 17, 17 },  // 49

#define MAKE_COEF16(a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14, a15) \
						    { (a00), (a00), (a01), (a01), (a02), (a02), (a03), (a03) }, \
						    { (a04), (a04), (a05), (a05), (a06), (a06), (a07), (a07) }, \
						    { (a08), (a08), (a09), (a09), (a10), (a10), (a11), (a11) }, \
						    { (a12), (a12), (a13), (a13), (a14), (a14), (a15), (a15) },

	MAKE_COEF16(44, 38, 25, 9, -9, -25, -38, -44, -44, -38, -25, -9, 9, 25, 38, 44) // 25
	MAKE_COEF16(38, -9, -44, -25, 25, 44, 9, -38, -38, 9, 44, 25, -25, -44, -9, 38) // 54

	// TODO: convert below table here
#undef MAKE_COEF16

		{ 25, 25, -44, -44, 9, 9, 38, 38},		// 58
		{ -38, -38, -9, -9, 44, 44, -25, -25 },  // 59
		{ -25, -25, 44, 44, -9, -9, -38, -38 },  // 60
		{ 38, 38, 9, 9, -44, -44, 25, 25 },  // 61

		{ 9, 9, -25, -25, 38, 38, -44, -44 },  // 62
		{ 44, 44, -38, -38, 25, 25, -9, -9 },  // 63
		{ -9, -9, 25, 25, -38, -38, 44, 44 },  // 64
		{ -44, -44, 38, 38, -25, -25, 9, 9 },  // 65

		{ 45, 45, 43, 43, 40, 40, 35, 35 },  // 66
		{ 29, 29, 21, 21, 13, 13, 4, 4 },  // 67
		{ -4, -4, -13, -13, -21, -21, -29, -29 },  // 68
		{ -35, -35, -40, -40, -43, -43, -45, -45 },  // 69

		{ 43, 43, 29, 29, 4, 4, -21, -21 },  // 70
		{ -40, -40, -45, -45, -35, -35, -13, -13 },  // 71
		{ 13, 13, 35, 35, 45, 45, 40, 40 },  // 72
		{ 21, 21, -4, -4, -29, -29, -43, -43 },  // 73

		{ 40, 40, 4, 4, -35, -35, -43, -43 },  // 74
		{ -13, -13, 29, 29, 45, 45, 21, 21 },  // 75
		{ -21, -21, -45, -45, -29, -29, 13, 13 },  // 76
		{ 43, 43, 35, 35, -4, -4, -40, -40 },  // 77

		{ 35, 35, -21, -21, -43, -43, 4, 4 },  // 78
		{ 45, 45, 13, 13, -40, -40, -29, -29 },  // 79
		{ 29, 29, 40, 40, -13, -13, -45, -45 },  // 80
		{ -4, -4, 43, 43, 21, 21, -35, -35 },  // 81

		{ 29, 29, -40, -40, -13, -13, 45, 45 },  // 82
		{ -4, -4, -43, -43, 21, 21, 35, 35 },  // 83
		{ -35, -35, -21, -21, 43, 43, 4, 4 },  // 84
		{ -45, -45, 13, 13, 40, 40, -29, -29 },  // 85

		{ 21, 21, -45, -45, 29, 29, 13, 13 },  // 86
		{ -43, -43, 35, 35, 4, 4, -40, -40 },  // 87
		{ 40, 40, -4, -4, -35, -35, 43, 43 },  // 88
		{ -13, -13, -29, -29, 45, 45, -21, -21 },  // 89

		{ 13, 13, -35, -35, 45, 45, -40, -40 },  // 90
		{ 21, 21, 4, 4, -29, -29, 43, 43 },  // 91
		{ -43, -43, 29, 29, -4, -4, -21, -21 },  // 92
		{ 40, 40, -45, -45, 35, 35, -13, -13 },  // 93

		{ 4, 4, -13, -13, 21, 21, -29, -29 },  // 94
		{ 35, 35, -40, -40, 43, 43, -45, -45 },  // 95
		{ 45, 45, -43, -43, 40, 40, -35, -35 },  // 96
		{ 29, 29, -21, -21, 13, 13, -4, -4 },  // 97

#define MAKE_COEF16(a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14, a15) \
									    { (a00), -(a00), (a01), -(a01), (a02), -(a02), (a03), -(a03) }, \
									    { (a04), -(a04), (a05), -(a05), (a06), -(a06), (a07), -(a07) }, \
									    { (a08), -(a08), (a09), -(a09), (a10), -(a10), (a11), -(a11) }, \
									    { (a12), -(a12), (a13), -(a13), (a14), -(a14), (a15), -(a15) },

		MAKE_COEF16(45, 45, 44, 43, 41, 39, 36, 34, 30, 27, 23, 19, 15, 11, 7, 2)    // 98
		MAKE_COEF16(45, 41, 34, 23, 11, -2, -15, -27, -36, -43, -45, -44, -39, -30, -19, -7)     //102
		MAKE_COEF16(44, 34, 15, -7, -27, -41, -45, -39, -23, -2, 19, 36, 45, 43, 30, 11)     //106
		MAKE_COEF16(43, 23, -7, -34, -45, -36, -11, 19, +41, 44, 27, -2, -30, -45, -39, -15)     //110
		MAKE_COEF16(41, 11, -27, -45, -30, 7, 39, 43, +15, -23, -45, -34, 2, 36, 44, 19)     //114
		MAKE_COEF16(39, -2, -41, -36, 7, 43, 34, -11, -44, -30, 15, 45, 27, -19, -45, -23)     //118
		MAKE_COEF16(36, -15, -45, -11, 39, 34, -19, -45, -7, 41, 30, -23, -44, -2, 43, 27)     //122
		MAKE_COEF16(34, -27, -39, 19, 43, -11, -45, 2, +45, 7, -44, -15, 41, 23, -36, -30)     //126
		MAKE_COEF16(30, -36, -23, 41, 15, -44, -7, 45, -2, -45, 11, 43, -19, -39, 27, 34)     //130
		MAKE_COEF16(27, -43, -2, 44, -23, -30, 41, 7, -45, 19, 34, -39, -11, 45, -15, -36)     //134
		MAKE_COEF16(23, -45, 19, 27, -45, 15, 30, -44, +11, 34, -43, 7, 36, -41, 2, 39)     //138
		MAKE_COEF16(19, -44, 36, -2, -34, 45, -23, -15, +43, -39, 7, 30, -45, 27, 11, -41)     //142
		MAKE_COEF16(15, -39, 45, -30, 2, 27, -44, 41, -19, -11, 36, -45, 34, -7, -23, 43)     //146
		MAKE_COEF16(11, -30, 43, -45, 36, -19, -2, 23, -39, 45, -41, 27, -7, -15, 34, -44)     //150
		MAKE_COEF16(7, -19, 30, -39, 44, -45, 43, -36, +27, -15, 2, 11, -23, 34, -41, 45)     //154
		MAKE_COEF16(2, -7, 11, -15, 19, -23, 27, -30, +34, -36, 39, -41, 43, -44, 45, -45)     //158

#undef MAKE_COEF16
};

ALIGNED_32(static const coef_t tab_dct_32_1_256i[][16]) =
{
	{ 44, -44, 9, -9, 38, -38, 25, -25, 44, -44, 9, -9, 38, -38, 25, -25 },          //  0
	{ 38, -38, -25, 25, -9, 9, -44, 44, 38, -38, -25, 25, -9, 9, -44, 44 },          //  1
	{ 25, -25, 38, -38, -44, 44, 9, -9, 25, -25, 38, -38, -44, 44, 9, -9 },          //  2
	{ 9, -9, -44, 44, -25, 25, 38, -38, 9, -9, -44, 44, -25, 25, 38, -38 },          //  3

#define MAKE_COEF8(a0, a1, a2, a3, a4, a5, a6, a7) \
	{ (a0), (a7), (a3), (a4), (a1), (a6), (a2), (a5), (a0), (a7), (a3), (a4), (a1), (a6), (a2), (a5) }, \

	MAKE_COEF8(45, 43, 40, 35, 29, 21, 13, 4)
	MAKE_COEF8(43, 29, 4, -21, -40, -45, -35, -13)
	MAKE_COEF8(40, 4, -35, -43, -13, 29, 45, 21)
	MAKE_COEF8(35, -21, -43, 4, 45, 13, -40, -29)
	MAKE_COEF8(29, -40, -13, 45, -4, -43, 21, 35)
	MAKE_COEF8(21, -45, 29, 13, -43, 35, 4, -40)
	MAKE_COEF8(13, -35, 45, -40, 21, 4, -29, 43)
	MAKE_COEF8(4, -13, 21, -29, 35, -40, 43, -45)
#undef MAKE_COEF8

#define MAKE_COEF16(a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14, a15) \
	{(a00), (a07), (a03), (a04), (a01), (a06), (a02), (a05), (a00), (a07), (a03), (a04), (a01), (a06), (a02), (a05)}, \
	{(a15), (a08), (a12), (a11), (a14), (a09), (a13), (a10), (a15), (a08), (a12), (a11), (a14), (a09), (a13), (a10)}, \

	MAKE_COEF16(45, 45, 44, 43, 41, 39, 36, 34, 30, 27, 23, 19, 15, 11, 7, 2)    // 12
	MAKE_COEF16(45, 41, 34, 23, 11, -2, -15, -27, -36, -43, -45, -44, -39, -30, -19, -7)    // 14
	MAKE_COEF16(44, 34, 15, -7, -27, -41, -45, -39, -23, -2, 19, 36, 45, 43, 30, 11)    // 16
	MAKE_COEF16(43, 23, -7, -34, -45, -36, -11, 19, 41, 44, 27, -2, -30, -45, -39, -15)    // 18
	MAKE_COEF16(41, 11, -27, -45, -30, 7, 39, 43, 15, -23, -45, -34, 2, 36, 44, 19)    // 20
	MAKE_COEF16(39, -2, -41, -36, 7, 43, 34, -11, -44, -30, 15, 45, 27, -19, -45, -23)    // 22
	MAKE_COEF16(36, -15, -45, -11, 39, 34, -19, -45, -7, 41, 30, -23, -44, -2, 43, 27)    // 24
	MAKE_COEF16(34, -27, -39, 19, 43, -11, -45, 2, 45, 7, -44, -15, 41, 23, -36, -30)    // 26
	MAKE_COEF16(30, -36, -23, 41, 15, -44, -7, 45, -2, -45, 11, 43, -19, -39, 27, 34)    // 28
	MAKE_COEF16(27, -43, -2, 44, -23, -30, 41, 7, -45, 19, 34, -39, -11, 45, -15, -36)    // 30
	MAKE_COEF16(23, -45, 19, 27, -45, 15, 30, -44, 11, 34, -43, 7, 36, -41, 2, 39)    // 32
	MAKE_COEF16(19, -44, 36, -2, -34, 45, -23, -15, 43, -39, 7, 30, -45, 27, 11, -41)    // 34
	MAKE_COEF16(15, -39, 45, -30, 2, 27, -44, 41, -19, -11, 36, -45, 34, -7, -23, 43)    // 36
	MAKE_COEF16(11, -30, 43, -45, 36, -19, -2, 23, -39, 45, -41, 27, -7, -15, 34, -44)    // 38
	MAKE_COEF16(7, -19, 30, -39, 44, -45, 43, -36, 27, -15, 2, 11, -23, 34, -41, 45)    // 40
	MAKE_COEF16(2, -7, 11, -15, 19, -23, 27, -30, 34, -36, 39, -41, 43, -44, 45, -45)    // 42
#undef MAKE_COEF16

	{
		32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
	},		// 44

	{ 32, 32, -32, -32, -32, -32, 32, 32, 32, 32, -32, -32, -32, -32, 32, 32 },  // 45

	{ 42, 42, 17, 17, -17, -17, -42, -42, 42, 42, 17, 17, -17, -17, -42, -42 },  // 46
	{ -42, -42, -17, -17, 17, 17, 42, 42, -42, -42, -17, -17, 17, 17, 42, 42 },  // 47

	{ 17, 17, -42, -42, 42, 42, -17, -17, 17, 17, -42, -42, 42, 42, -17, -17 },  // 48
	{ -17, -17, 42, 42, -42, -42, 17, 17, -17, -17, 42, 42, -42, -42, 17, 17 },  // 49

#define MAKE_COEF16(a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14, a15) \
	{ (a00), (a00), (a01), (a01), (a02), (a02), (a03), (a03), (a00), (a00), (a01), (a01), (a02), (a02), (a03), (a03) }, \
	{ (a04), (a04), (a05), (a05), (a06), (a06), (a07), (a07), (a04), (a04), (a05), (a05), (a06), (a06), (a07), (a07) }, \
	{ (a08), (a08), (a09), (a09), (a10), (a10), (a11), (a11), (a08), (a08), (a09), (a09), (a10), (a10), (a11), (a11) }, \
	{ (a12), (a12), (a13), (a13), (a14), (a14), (a15), (a15), (a12), (a12), (a13), (a13), (a14), (a14), (a15), (a15) },

	MAKE_COEF16(44, 38, 25, 9, -9, -25, -38, -44, -44, -38, -25, -9, 9, 25, 38, 44) // 25
	MAKE_COEF16(38, -9, -44, -25, 25, 44, 9, -38, -38, 9, 44, 25, -25, -44, -9, 38) // 54

	// TODO: convert below table here
#undef MAKE_COEF16

		{
		25, 25, -44, -44, 9, 9, 38, 38, 25, 25, -44, -44, 9, 9, 38, 38
	},		// 58
	{ -38, -38, -9, -9, 44, 44, -25, -25, -38, -38, -9, -9, 44, 44, -25, -25 },  // 59
	{ -25, -25, 44, 44, -9, -9, -38, -38, -25, -25, 44, 44, -9, -9, -38, -38 },  // 60
	{ 38, 38, 9, 9, -44, -44, 25, 25, 38, 38, 9, 9, -44, -44, 25, 25 },  // 61

	{ 9, 9, -25, -25, 38, 38, -44, -44, 9, 9, -25, -25, 38, 38, -44, -44 },  // 62
	{ 44, 44, -38, -38, 25, 25, -9, -9, 44, 44, -38, -38, 25, 25, -9, -9 },  // 63
	{ -9, -9, 25, 25, -38, -38, 44, 44, -9, -9, 25, 25, -38, -38, 44, 44 },  // 64
	{ -44, -44, 38, 38, -25, -25, 9, 9, -44, -44, 38, 38, -25, -25, 9, 9 },  // 65

	{ 45, 45, 43, 43, 40, 40, 35, 35, 45, 45, 43, 43, 40, 40, 35, 35 },  // 66
	{ 29, 29, 21, 21, 13, 13, 4, 4, 29, 29, 21, 21, 13, 13, 4, 4 },  // 67
	{ -4, -4, -13, -13, -21, -21, -29, -29, -4, -4, -13, -13, -21, -21, -29, -29 },  // 68
	{ -35, -35, -40, -40, -43, -43, -45, -45, -35, -35, -40, -40, -43, -43, -45, -45 },  // 69

	{ 43, 43, 29, 29, 4, 4, -21, -21, 43, 43, 29, 29, 4, 4, -21, -21 },  // 70
	{ -40, -40, -45, -45, -35, -35, -13, -13, -40, -40, -45, -45, -35, -35, -13, -13 },  // 71
	{ 13, 13, 35, 35, 45, 45, 40, 40, 13, 13, 35, 35, 45, 45, 40, 40 },  // 72
	{ 21, 21, -4, -4, -29, -29, -43, -43, 21, 21, -4, -4, -29, -29, -43, -43 },  // 73

	{ 40, 40, 4, 4, -35, -35, -43, -43, 40, 40, 4, 4, -35, -35, -43, -43 },  // 74
	{ -13, -13, 29, 29, 45, 45, 21, 21, -13, -13, 29, 29, 45, 45, 21, 21 },  // 75
	{ -21, -21, -45, -45, -29, -29, 13, 13, -21, -21, -45, -45, -29, -29, 13, 13 },  // 76
	{ 43, 43, 35, 35, -4, -4, -40, -40, 43, 43, 35, 35, -4, -4, -40, -40 },  // 77

	{ 35, 35, -21, -21, -43, -43, 4, 4, 35, 35, -21, -21, -43, -43, 4, 4 },  // 78
	{ 45, 45, 13, 13, -40, -40, -29, -29, 45, 45, 13, 13, -40, -40, -29, -29 },  // 79
	{ 29, 29, 40, 40, -13, -13, -45, -45, 29, 29, 40, 40, -13, -13, -45, -45 },  // 80
	{ -4, -4, 43, 43, 21, 21, -35, -35, -4, -4, 43, 43, 21, 21, -35, -35 },  // 81

	{ 29, 29, -40, -40, -13, -13, 45, 45, 29, 29, -40, -40, -13, -13, 45, 45 },  // 82
	{ -4, -4, -43, -43, 21, 21, 35, 35, -4, -4, -43, -43, 21, 21, 35, 35 },  // 83
	{ -35, -35, -21, -21, 43, 43, 4, 4, -35, -35, -21, -21, 43, 43, 4, 4 },  // 84
	{ -45, -45, 13, 13, 40, 40, -29, -29, -45, -45, 13, 13, 40, 40, -29, -29 },  // 85

	{ 21, 21, -45, -45, 29, 29, 13, 13, 21, 21, -45, -45, 29, 29, 13, 13 },  // 86
	{ -43, -43, 35, 35, 4, 4, -40, -40, -43, -43, 35, 35, 4, 4, -40, -40 },  // 87
	{ 40, 40, -4, -4, -35, -35, 43, 43, 40, 40, -4, -4, -35, -35, 43, 43 },  // 88
	{ -13, -13, -29, -29, 45, 45, -21, -21, -13, -13, -29, -29, 45, 45, -21, -21 },  // 89

	{ 13, 13, -35, -35, 45, 45, -40, -40, 13, 13, -35, -35, 45, 45, -40, -40 },  // 90
	{ 21, 21, 4, 4, -29, -29, 43, 43, 21, 21, 4, 4, -29, -29, 43, 43 },  // 91
	{ -43, -43, 29, 29, -4, -4, -21, -21, -43, -43, 29, 29, -4, -4, -21, -21 },  // 92
	{ 40, 40, -45, -45, 35, 35, -13, -13, 40, 40, -45, -45, 35, 35, -13, -13 },  // 93

	{ 4, 4, -13, -13, 21, 21, -29, -29, 4, 4, -13, -13, 21, 21, -29, -29 },  // 94
	{ 35, 35, -40, -40, 43, 43, -45, -45, 35, 35, -40, -40, 43, 43, -45, -45 },  // 95
	{ 45, 45, -43, -43, 40, 40, -35, -35, 45, 45, -43, -43, 40, 40, -35, -35 },  // 96
	{ 29, 29, -21, -21, 13, 13, -4, -4, 29, 29, -21, -21, 13, 13, -4, -4 },  // 97

#define MAKE_COEF16(a00, a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14, a15) \
	{ (a00), -(a00), (a01), -(a01), (a02), -(a02), (a03), -(a03), (a00), -(a00), (a01), -(a01), (a02), -(a02), (a03), -(a03) }, \
	{ (a04), -(a04), (a05), -(a05), (a06), -(a06), (a07), -(a07), (a04), -(a04), (a05), -(a05), (a06), -(a06), (a07), -(a07) }, \
	{ (a08), -(a08), (a09), -(a09), (a10), -(a10), (a11), -(a11), (a08), -(a08), (a09), -(a09), (a10), -(a10), (a11), -(a11) }, \
	{ (a12), -(a12), (a13), -(a13), (a14), -(a14), (a15), -(a15), (a12), -(a12), (a13), -(a13), (a14), -(a14), (a15), -(a15) },

	MAKE_COEF16(45, 45, 44, 43, 41, 39, 36, 34, 30, 27, 23, 19, 15, 11, 7, 2)    // 98
	MAKE_COEF16(45, 41, 34, 23, 11, -2, -15, -27, -36, -43, -45, -44, -39, -30, -19, -7)     //102
	MAKE_COEF16(44, 34, 15, -7, -27, -41, -45, -39, -23, -2, 19, 36, 45, 43, 30, 11)     //106
	MAKE_COEF16(43, 23, -7, -34, -45, -36, -11, 19, +41, 44, 27, -2, -30, -45, -39, -15)     //110
	MAKE_COEF16(41, 11, -27, -45, -30, 7, 39, 43, +15, -23, -45, -34, 2, 36, 44, 19)     //114
	MAKE_COEF16(39, -2, -41, -36, 7, 43, 34, -11, -44, -30, 15, 45, 27, -19, -45, -23)     //118
	MAKE_COEF16(36, -15, -45, -11, 39, 34, -19, -45, -7, 41, 30, -23, -44, -2, 43, 27)     //122
	MAKE_COEF16(34, -27, -39, 19, 43, -11, -45, 2, +45, 7, -44, -15, 41, 23, -36, -30)     //126
	MAKE_COEF16(30, -36, -23, 41, 15, -44, -7, 45, -2, -45, 11, 43, -19, -39, 27, 34)     //130
	MAKE_COEF16(27, -43, -2, 44, -23, -30, 41, 7, -45, 19, 34, -39, -11, 45, -15, -36)     //134
	MAKE_COEF16(23, -45, 19, 27, -45, 15, 30, -44, +11, 34, -43, 7, 36, -41, 2, 39)     //138
	MAKE_COEF16(19, -44, 36, -2, -34, 45, -23, -15, +43, -39, 7, 30, -45, 27, 11, -41)     //142
	MAKE_COEF16(15, -39, 45, -30, 2, 27, -44, 41, -19, -11, 36, -45, 34, -7, -23, 43)     //146
	MAKE_COEF16(11, -30, 43, -45, 36, -19, -2, 23, -39, 45, -41, 27, -7, -15, 34, -44)     //150
	MAKE_COEF16(7, -19, 30, -39, 44, -45, 43, -36, +27, -15, 2, 11, -23, 34, -41, 45)     //154
	MAKE_COEF16(2, -7, 11, -15, 19, -23, 27, -30, +34, -36, 39, -41, 43, -44, 45, -45)     //158

#undef MAKE_COEF16
};

void sub_trans_32x32_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	// Const
	__m128i c_4 = _mm_set1_epi32(4);
	__m128i c_512 = _mm_set1_epi32(512);
	int shift1 = 3;
	int shift2 = 10;
	int i;

	__m128i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m128i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m128i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m128i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m128i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m128i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m128i T20, T21, T22, T23, T24, T25, T26, T27;
	__m128i T30, T31, T32, T33, T34, T35, T36, T37;
	__m128i T40, T41, T42, T43, T44, T45, T46, T47;
	__m128i T50, T51, T52, T53;
	__m128i T60, T61, T62, T63, T64, T65, T66, T67;
	__m128i im[32][4];

	__m128i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m128i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m128i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m128i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m128i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m128i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m128i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m128i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m128i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m128i TT50, TT51, TT52, TT53;
	__m128i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m128i tResult;

	__m128i O00, O01, O02, O03, O04, O05, O06, O07;
	__m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m128i P00, P01, P02, P03, P04, P05, P06, P07;
	__m128i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m128i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m128i tmpZero = _mm_setzero_si128();
    __m128i tabDCT16, tabDCT32;

	// DCT1
	for (i = 0; i < 32 / 8; i++)
	{
		//load data
		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org]);

		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);

		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred]);

		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);

		T00A = _mm_sub_epi16(O00A, P00A);
		T00B = _mm_sub_epi16(O00B, P00B);
		T01A = _mm_sub_epi16(O01A, P01A);
		T01B = _mm_sub_epi16(O01B, P01B);
		T02A = _mm_sub_epi16(O02A, P02A);
		T02B = _mm_sub_epi16(O02B, P02B);
		T03A = _mm_sub_epi16(O03A, P03A);
		T03B = _mm_sub_epi16(O03B, P03B);
		T04A = _mm_sub_epi16(O04A, P04A);
		T04B = _mm_sub_epi16(O04B, P04B);
		T05A = _mm_sub_epi16(O05A, P05A);
		T05B = _mm_sub_epi16(O05B, P05B);
		T06A = _mm_sub_epi16(O06A, P06A);
		T06B = _mm_sub_epi16(O06B, P06B);
		T07A = _mm_sub_epi16(O07A, P07A);
		T07B = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 16]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 16]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 16]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 16]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 16]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 16]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 16]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 16]);

		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);

		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 16]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 16]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 16]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 16]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 16]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 16]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 16]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 16]);

        org += i_org << 3;
        pred += i_pred << 3;

		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);

		T00C = _mm_sub_epi16(O00A, P00A);
		T00D = _mm_sub_epi16(O00B, P00B);
		T01C = _mm_sub_epi16(O01A, P01A);
		T01D = _mm_sub_epi16(O01B, P01B);
		T02C = _mm_sub_epi16(O02A, P02A);
		T02D = _mm_sub_epi16(O02B, P02B);
		T03C = _mm_sub_epi16(O03A, P03A);
		T03D = _mm_sub_epi16(O03B, P03B);
		T04C = _mm_sub_epi16(O04A, P04A);
		T04D = _mm_sub_epi16(O04B, P04B);
		T05C = _mm_sub_epi16(O05A, P05A);
		T05D = _mm_sub_epi16(O05B, P05B);
		T06C = _mm_sub_epi16(O06A, P06A);
		T06D = _mm_sub_epi16(O06B, P06B);
		T07C = _mm_sub_epi16(O07A, P07A);
		T07D = _mm_sub_epi16(O07B, P07B);
		//
        tabDCT16 = _mm_load_si128((__m128i*)tab_dct_16_0[1]);
        tabDCT32 = _mm_load_si128((__m128i*)tab_dct_32_0[0]);
        T00A = _mm_shuffle_epi8(T00A, tabDCT16);    // [05 02 06 01 04 03 07 00]
        T00B = _mm_shuffle_epi8(T00B, tabDCT32);    // [10 13 09 14 11 12 08 15]
		T00C = _mm_shuffle_epi8(T00C, tabDCT16);    // [21 18 22 17 20 19 23 16]
		T00D = _mm_shuffle_epi8(T00D, tabDCT32);    // [26 29 25 30 27 28 24 31]
		T01A = _mm_shuffle_epi8(T01A, tabDCT16);
		T01B = _mm_shuffle_epi8(T01B, tabDCT32);
		T01C = _mm_shuffle_epi8(T01C, tabDCT16);
		T01D = _mm_shuffle_epi8(T01D, tabDCT32);
		T02A = _mm_shuffle_epi8(T02A, tabDCT16);
		T02B = _mm_shuffle_epi8(T02B, tabDCT32);
		T02C = _mm_shuffle_epi8(T02C, tabDCT16);
		T02D = _mm_shuffle_epi8(T02D, tabDCT32);
		T03A = _mm_shuffle_epi8(T03A, tabDCT16);
		T03B = _mm_shuffle_epi8(T03B, tabDCT32);
		T03C = _mm_shuffle_epi8(T03C, tabDCT16);
		T03D = _mm_shuffle_epi8(T03D, tabDCT32);
		T04A = _mm_shuffle_epi8(T04A, tabDCT16);
		T04B = _mm_shuffle_epi8(T04B, tabDCT32);
		T04C = _mm_shuffle_epi8(T04C, tabDCT16);
		T04D = _mm_shuffle_epi8(T04D, tabDCT32);
		T05A = _mm_shuffle_epi8(T05A, tabDCT16);
		T05B = _mm_shuffle_epi8(T05B, tabDCT32);
		T05C = _mm_shuffle_epi8(T05C, tabDCT16);
		T05D = _mm_shuffle_epi8(T05D, tabDCT32);
		T06A = _mm_shuffle_epi8(T06A, tabDCT16);
		T06B = _mm_shuffle_epi8(T06B, tabDCT32);
		T06C = _mm_shuffle_epi8(T06C, tabDCT16);
		T06D = _mm_shuffle_epi8(T06D, tabDCT32);
		T07A = _mm_shuffle_epi8(T07A, tabDCT16);
		T07B = _mm_shuffle_epi8(T07B, tabDCT32);
		T07C = _mm_shuffle_epi8(T07C, tabDCT16);
		T07D = _mm_shuffle_epi8(T07D, tabDCT32);

		T10A = _mm_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm_add_epi16(T01A, T01D);
		T11B = _mm_add_epi16(T01B, T01C);
		T12A = _mm_add_epi16(T02A, T02D);
		T12B = _mm_add_epi16(T02B, T02C);
		T13A = _mm_add_epi16(T03A, T03D);
		T13B = _mm_add_epi16(T03B, T03C);
		T14A = _mm_add_epi16(T04A, T04D);
		T14B = _mm_add_epi16(T04B, T04C);
		T15A = _mm_add_epi16(T05A, T05D);
		T15B = _mm_add_epi16(T05B, T05C);
		T16A = _mm_add_epi16(T06A, T06D);
		T16B = _mm_add_epi16(T06B, T06C);
		T17A = _mm_add_epi16(T07A, T07D);
		T17B = _mm_add_epi16(T07B, T07C);

		T00A = _mm_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm_sub_epi16(T01A, T01D);
		T01B = _mm_sub_epi16(T01B, T01C);
		T02A = _mm_sub_epi16(T02A, T02D);
		T02B = _mm_sub_epi16(T02B, T02C);
		T03A = _mm_sub_epi16(T03A, T03D);
		T03B = _mm_sub_epi16(T03B, T03C);
		T04A = _mm_sub_epi16(T04A, T04D);
		T04B = _mm_sub_epi16(T04B, T04C);
		T05A = _mm_sub_epi16(T05A, T05D);
		T05B = _mm_sub_epi16(T05B, T05C);
		T06A = _mm_sub_epi16(T06A, T06D);
		T06B = _mm_sub_epi16(T06B, T06C);
		T07A = _mm_sub_epi16(T07A, T07D);
		T07B = _mm_sub_epi16(T07B, T07C);

		T20 = _mm_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm_add_epi16(T11A, T11B);
		T22 = _mm_add_epi16(T12A, T12B);
		T23 = _mm_add_epi16(T13A, T13B);
		T24 = _mm_add_epi16(T14A, T14B);
		T25 = _mm_add_epi16(T15A, T15B);
		T26 = _mm_add_epi16(T16A, T16B);
		T27 = _mm_add_epi16(T17A, T17B);

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_8[1]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[0][i] = T60;

		T50 = _mm_hsub_epi32(T40, T41);
		T51 = _mm_hsub_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[16][i] = T60;

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_16_1[8]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[8][i] = T60;

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_16_1[9]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[24][i] = T60;

#define MAKE_ODD(tab, dstPos) \
    T30  = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T32  = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T34  = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T36  = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
        \
    T40  = _mm_hadd_epi32(T30, T31); \
    T41  = _mm_hadd_epi32(T32, T33); \
    T42  = _mm_hadd_epi32(T34, T35); \
    T43  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);
		MAKE_ODD(2, 20);
		MAKE_ODD(3, 28);

		T20 = _mm_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm_sub_epi16(T11A, T11B);
		T22 = _mm_sub_epi16(T12A, T12B);
		T23 = _mm_sub_epi16(T13A, T13B);
		T24 = _mm_sub_epi16(T14A, T14B);
		T25 = _mm_sub_epi16(T15A, T15B);
		T26 = _mm_sub_epi16(T16A, T16B);
		T27 = _mm_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);
		MAKE_ODD(8, 18);
		MAKE_ODD(9, 22);
		MAKE_ODD(10, 26);
		MAKE_ODD(11, 30);
#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
    T20  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T21  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T22  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T23  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T24  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T25  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T26  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T27  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T30  = _mm_madd_epi16(T04A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T04B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T32  = _mm_madd_epi16(T05A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T05B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T34  = _mm_madd_epi16(T06A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T06B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T36  = _mm_madd_epi16(T07A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T07B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
        \
    T40  = _mm_hadd_epi32(T20, T21); \
    T41  = _mm_hadd_epi32(T22, T23); \
    T42  = _mm_hadd_epi32(T24, T25); \
    T43  = _mm_hadd_epi32(T26, T27); \
    T44  = _mm_hadd_epi32(T30, T31); \
    T45  = _mm_hadd_epi32(T32, T33); \
    T46  = _mm_hadd_epi32(T34, T35); \
    T47  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T52  = _mm_hadd_epi32(T44, T45); \
    T53  = _mm_hadd_epi32(T46, T47); \
        \
    T50  = _mm_hadd_epi32(T50, T51); \
    T51  = _mm_hadd_epi32(T52, T53); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);
		MAKE_ODD(28, 17);
		MAKE_ODD(30, 19);
		MAKE_ODD(32, 21);
		MAKE_ODD(34, 23);
		MAKE_ODD(36, 25);
		MAKE_ODD(38, 27);
		MAKE_ODD(40, 29);
		MAKE_ODD(42, 31);

#undef MAKE_ODD
	}

	// DCT2
	for (i = 0; i < 32 / 8; i++)
	{
		T00A = im[i * 8 + 0][0];    // [07 06 05 04 03 02 01 00]
		T00B = im[i * 8 + 0][1];    // [15 14 13 12 11 10 09 08]
		T00C = im[i * 8 + 0][2];    // [23 22 21 20 19 18 17 16]
		T00D = im[i * 8 + 0][3];    // [31 30 29 28 27 26 25 24]
		T01A = im[i * 8 + 1][0];
		T01B = im[i * 8 + 1][1];
		T01C = im[i * 8 + 1][2];
		T01D = im[i * 8 + 1][3];
		T02A = im[i * 8 + 2][0];
		T02B = im[i * 8 + 2][1];
		T02C = im[i * 8 + 2][2];
		T02D = im[i * 8 + 2][3];
		T03A = im[i * 8 + 3][0];
		T03B = im[i * 8 + 3][1];
		T03C = im[i * 8 + 3][2];
		T03D = im[i * 8 + 3][3];

		TT00A = im[i * 8 + 4][0];
		TT00B = im[i * 8 + 4][1];
		TT00C = im[i * 8 + 4][2];
		TT00D = im[i * 8 + 4][3];
		TT01A = im[i * 8 + 5][0];
		TT01B = im[i * 8 + 5][1];
		TT01C = im[i * 8 + 5][2];
		TT01D = im[i * 8 + 5][3];
		TT02A = im[i * 8 + 6][0];
		TT02B = im[i * 8 + 6][1];
		TT02C = im[i * 8 + 6][2];
		TT02D = im[i * 8 + 6][3];
		TT03A = im[i * 8 + 7][0];
		TT03B = im[i * 8 + 7][1];
		TT03C = im[i * 8 + 7][2];
		TT03D = im[i * 8 + 7][3];

		T00C = _mm_shuffle_epi8(T00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [16 17 18 19 20 21 22 23]
		T00D = _mm_shuffle_epi8(T00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [24 25 26 27 28 29 30 31]
		T01C = _mm_shuffle_epi8(T01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T01D = _mm_shuffle_epi8(T01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02C = _mm_shuffle_epi8(T02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02D = _mm_shuffle_epi8(T02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03C = _mm_shuffle_epi8(T03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03D = _mm_shuffle_epi8(T03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		TT00C = _mm_shuffle_epi8(TT00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT00D = _mm_shuffle_epi8(TT00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01C = _mm_shuffle_epi8(TT01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01D = _mm_shuffle_epi8(TT01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02C = _mm_shuffle_epi8(TT02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02D = _mm_shuffle_epi8(TT02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03C = _mm_shuffle_epi8(TT03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03D = _mm_shuffle_epi8(TT03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		T10A = _mm_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
		T10B = _mm_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
		T00A = _mm_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
		T00B = _mm_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
		T11A = _mm_unpacklo_epi16(T01A, T01D);
		T11B = _mm_unpackhi_epi16(T01A, T01D);
		T01A = _mm_unpacklo_epi16(T01B, T01C);
		T01B = _mm_unpackhi_epi16(T01B, T01C);
		T12A = _mm_unpacklo_epi16(T02A, T02D);
		T12B = _mm_unpackhi_epi16(T02A, T02D);
		T02A = _mm_unpacklo_epi16(T02B, T02C);
		T02B = _mm_unpackhi_epi16(T02B, T02C);
		T13A = _mm_unpacklo_epi16(T03A, T03D);
		T13B = _mm_unpackhi_epi16(T03A, T03D);
		T03A = _mm_unpacklo_epi16(T03B, T03C);
		T03B = _mm_unpackhi_epi16(T03B, T03C);

		TT10A = _mm_unpacklo_epi16(TT00A, TT00D);
		TT10B = _mm_unpackhi_epi16(TT00A, TT00D);
		TT00A = _mm_unpacklo_epi16(TT00B, TT00C);
		TT00B = _mm_unpackhi_epi16(TT00B, TT00C);
		TT11A = _mm_unpacklo_epi16(TT01A, TT01D);
		TT11B = _mm_unpackhi_epi16(TT01A, TT01D);
		TT01A = _mm_unpacklo_epi16(TT01B, TT01C);
		TT01B = _mm_unpackhi_epi16(TT01B, TT01C);
		TT12A = _mm_unpacklo_epi16(TT02A, TT02D);
		TT12B = _mm_unpackhi_epi16(TT02A, TT02D);
		TT02A = _mm_unpacklo_epi16(TT02B, TT02C);
		TT02B = _mm_unpackhi_epi16(TT02B, TT02C);
		TT13A = _mm_unpacklo_epi16(TT03A, TT03D);
		TT13B = _mm_unpackhi_epi16(TT03A, TT03D);
		TT03A = _mm_unpacklo_epi16(TT03B, TT03C);
		TT03B = _mm_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
    T20  = _mm_madd_epi16(T10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T21  = _mm_madd_epi16(T10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T22  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T23  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T24  = _mm_madd_epi16(T11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T25  = _mm_madd_epi16(T11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T26  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T27  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T30  = _mm_madd_epi16(T12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T31  = _mm_madd_epi16(T12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T32  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T33  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T34  = _mm_madd_epi16(T13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T35  = _mm_madd_epi16(T13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T36  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T37  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    T60  = _mm_hadd_epi32(T20, T21); \
    T61  = _mm_hadd_epi32(T22, T23); \
    T62  = _mm_hadd_epi32(T24, T25); \
    T63  = _mm_hadd_epi32(T26, T27); \
    T64  = _mm_hadd_epi32(T30, T31); \
    T65  = _mm_hadd_epi32(T32, T33); \
    T66  = _mm_hadd_epi32(T34, T35); \
    T67  = _mm_hadd_epi32(T36, T37); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
    T62  = _mm_hadd_epi32(T64, T65); \
    T63  = _mm_hadd_epi32(T66, T67); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
        \
    T60  = _mm_srai_epi32(_mm_add_epi32(T60, c_512), shift2); \
		\
	TT20  = _mm_madd_epi16(TT10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT21  = _mm_madd_epi16(TT10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT22  = _mm_madd_epi16(TT00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT23  = _mm_madd_epi16(TT00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT24  = _mm_madd_epi16(TT11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT25  = _mm_madd_epi16(TT11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT26  = _mm_madd_epi16(TT01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT27  = _mm_madd_epi16(TT01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT30  = _mm_madd_epi16(TT12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT31  = _mm_madd_epi16(TT12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT32  = _mm_madd_epi16(TT02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT33  = _mm_madd_epi16(TT02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT34  = _mm_madd_epi16(TT13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT35  = _mm_madd_epi16(TT13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT36  = _mm_madd_epi16(TT03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT37  = _mm_madd_epi16(TT03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    TT60  = _mm_hadd_epi32(TT20, TT21); \
    TT61  = _mm_hadd_epi32(TT22, TT23); \
    TT62  = _mm_hadd_epi32(TT24, TT25); \
    TT63  = _mm_hadd_epi32(TT26, TT27); \
    TT64  = _mm_hadd_epi32(TT30, TT31); \
    TT65  = _mm_hadd_epi32(TT32, TT33); \
    TT66  = _mm_hadd_epi32(TT34, TT35); \
    TT67  = _mm_hadd_epi32(TT36, TT37); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
    TT62  = _mm_hadd_epi32(TT64, TT65); \
    TT63  = _mm_hadd_epi32(TT66, TT67); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
        \
    TT60  = _mm_srai_epi32(_mm_add_epi32(TT60, c_512), shift2); \
		\
	tResult = _mm_packs_epi32(T60, TT60);\
    _mm_storeu_si128((__m128i*)&dst[(dstPos) * 32 + (i * 8) + 0], tResult); \

		MAKE_ODD(44, 44, 44, 44, 0);
		MAKE_ODD(45, 45, 45, 45, 16);
		MAKE_ODD(46, 47, 46, 47, 8);
		MAKE_ODD(48, 49, 48, 49, 24);

		MAKE_ODD(50, 51, 52, 53, 4);
		MAKE_ODD(54, 55, 56, 57, 12);
		MAKE_ODD(58, 59, 60, 61, 20);
		MAKE_ODD(62, 63, 64, 65, 28);

		MAKE_ODD(66, 67, 68, 69, 2);
		MAKE_ODD(70, 71, 72, 73, 6);
		MAKE_ODD(74, 75, 76, 77, 10);
		MAKE_ODD(78, 79, 80, 81, 14);

		MAKE_ODD(82, 83, 84, 85, 18);
		MAKE_ODD(86, 87, 88, 89, 22);
		MAKE_ODD(90, 91, 92, 93, 26);
		MAKE_ODD(94, 95, 96, 97, 30);

		MAKE_ODD(98, 99, 100, 101, 1);
		MAKE_ODD(102, 103, 104, 105, 3);
		MAKE_ODD(106, 107, 108, 109, 5);
		MAKE_ODD(110, 111, 112, 113, 7);
		MAKE_ODD(114, 115, 116, 117, 9);
		MAKE_ODD(118, 119, 120, 121, 11);
		MAKE_ODD(122, 123, 124, 125, 13);
		MAKE_ODD(126, 127, 128, 129, 15);
		MAKE_ODD(130, 131, 132, 133, 17);
		MAKE_ODD(134, 135, 136, 137, 19);
		MAKE_ODD(138, 139, 140, 141, 21);
		MAKE_ODD(142, 143, 144, 145, 23);
		MAKE_ODD(146, 147, 148, 149, 25);
		MAKE_ODD(150, 151, 152, 153, 27);
		MAKE_ODD(154, 155, 156, 157, 29);
		MAKE_ODD(158, 159, 160, 161, 31);
#undef MAKE_ODD
	}
}

void sub_trans_32x32_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	// Const
	__m256i c_4 = _mm256_set1_epi32(4);
	__m256i c_512 = _mm256_set1_epi32(512);
	int shift1 = 3;
	int shift2 = 10;
	int i;

	__m256i T00, T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15;
	__m256i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m256i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m256i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m256i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m256i OO00A, OO01A, OO02A, OO03A, OO04A, OO05A, OO06A, OO07A;
	__m256i OO00B, OO01B, OO02B, OO03B, OO04B, OO05B, OO06B, OO07B;
	__m256i PP00A, PP01A, PP02A, PP03A, PP04A, PP05A, PP06A, PP07A;
	__m256i PP00B, PP01B, PP02B, PP03B, PP04B, PP05B, PP06B, PP07B;
	__m256i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m256i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m256i T20, T21, T22, T23, T24, T25, T26, T27;
	__m256i T30, T31, T32, T33, T34, T35, T36, T37;
	__m256i T40, T41, T42, T43, T44, T45, T46, T47;
	__m256i T50, T51, T52, T53;
	__m256i T60, T61, T62, T63, T64, T65, T66, T67;
	__m256i im[32][4];

	__m256i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m256i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m256i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m256i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m256i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m256i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m256i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m256i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m256i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m256i TT50, TT51, TT52, TT53;
	__m256i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m256i tResult;

	__m256i O00, O01, O02, O03, O04, O05, O06, O07, O08, O09, O10, O11, O12, O13, O14, O15;
	__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m256i P00, P01, P02, P03, P04, P05, P06, P07, P08, P09, P10, P11, P12, P13, P14, P15;
	__m256i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m256i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m256i tmpZero = _mm256_setzero_si256();
	__m256i TAB16_0_1, TAB32_0_0, TAB16_0_0;

	// DCT1
	for (i = 0; i < 2; i++)
	{
		//load data
		O00 = _mm256_loadu_si256((__m256i*)&org[0 * i_org]);	//256 bits are valid , 8*32
		O01 = _mm256_loadu_si256((__m256i*)&org[1 * i_org]);
		O02 = _mm256_loadu_si256((__m256i*)&org[2 * i_org]);
		O03 = _mm256_loadu_si256((__m256i*)&org[3 * i_org]);
		O04 = _mm256_loadu_si256((__m256i*)&org[4 * i_org]);
		O05 = _mm256_loadu_si256((__m256i*)&org[5 * i_org]);
		O06 = _mm256_loadu_si256((__m256i*)&org[6 * i_org]);
		O07 = _mm256_loadu_si256((__m256i*)&org[7 * i_org]);
		O08 = _mm256_loadu_si256((__m256i*)&org[8 * i_org]);
		O09 = _mm256_loadu_si256((__m256i*)&org[9 * i_org]);
		O10 = _mm256_loadu_si256((__m256i*)&org[10 * i_org]);
		O11 = _mm256_loadu_si256((__m256i*)&org[11 * i_org]);
		O12 = _mm256_loadu_si256((__m256i*)&org[12 * i_org]);
		O13 = _mm256_loadu_si256((__m256i*)&org[13 * i_org]);
		O14 = _mm256_loadu_si256((__m256i*)&org[14 * i_org]);
		O15 = _mm256_loadu_si256((__m256i*)&org[15 * i_org]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[0 * i_pred]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[1 * i_pred]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[2 * i_pred]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[3 * i_pred]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[4 * i_pred]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[5 * i_pred]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[6 * i_pred]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[7 * i_pred]);
		P08 = _mm256_loadu_si256((__m256i*)&pred[8 * i_pred]);
		P09 = _mm256_loadu_si256((__m256i*)&pred[9 * i_pred]);
		P10 = _mm256_loadu_si256((__m256i*)&pred[10 * i_pred]);
		P11 = _mm256_loadu_si256((__m256i*)&pred[11 * i_pred]);
		P12 = _mm256_loadu_si256((__m256i*)&pred[12 * i_pred]);
		P13 = _mm256_loadu_si256((__m256i*)&pred[13 * i_pred]);
		P14 = _mm256_loadu_si256((__m256i*)&pred[14 * i_pred]);
		P15 = _mm256_loadu_si256((__m256i*)&pred[15 * i_pred]);

		OO00A = _mm256_permute2x128_si256(O00, O08, 0x20);
		OO01A = _mm256_permute2x128_si256(O01, O09, 0x20);
		OO02A = _mm256_permute2x128_si256(O02, O10, 0x20);
		OO03A = _mm256_permute2x128_si256(O03, O11, 0x20);
		OO04A = _mm256_permute2x128_si256(O04, O12, 0x20);
		OO05A = _mm256_permute2x128_si256(O05, O13, 0x20);
		OO06A = _mm256_permute2x128_si256(O06, O14, 0x20);
		OO07A = _mm256_permute2x128_si256(O07, O15, 0x20); 
		PP00A = _mm256_permute2x128_si256(P00, P08, 0x20);
		PP01A = _mm256_permute2x128_si256(P01, P09, 0x20);
		PP02A = _mm256_permute2x128_si256(P02, P10, 0x20);
		PP03A = _mm256_permute2x128_si256(P03, P11, 0x20);
		PP04A = _mm256_permute2x128_si256(P04, P12, 0x20);
		PP05A = _mm256_permute2x128_si256(P05, P13, 0x20);
		PP06A = _mm256_permute2x128_si256(P06, P14, 0x20);
		PP07A = _mm256_permute2x128_si256(P07, P15, 0x20);

		O00A = _mm256_unpacklo_epi8(OO00A, tmpZero);
		O00B = _mm256_unpackhi_epi8(OO00A, tmpZero);
		O01A = _mm256_unpacklo_epi8(OO01A, tmpZero);
		O01B = _mm256_unpackhi_epi8(OO01A, tmpZero);
		O02A = _mm256_unpacklo_epi8(OO02A, tmpZero);
		O02B = _mm256_unpackhi_epi8(OO02A, tmpZero);
		O03A = _mm256_unpacklo_epi8(OO03A, tmpZero);
		O03B = _mm256_unpackhi_epi8(OO03A, tmpZero);
		O04A = _mm256_unpacklo_epi8(OO04A, tmpZero);
		O04B = _mm256_unpackhi_epi8(OO04A, tmpZero);
		O05A = _mm256_unpacklo_epi8(OO05A, tmpZero);
		O05B = _mm256_unpackhi_epi8(OO05A, tmpZero);
		O06A = _mm256_unpacklo_epi8(OO06A, tmpZero);
		O06B = _mm256_unpackhi_epi8(OO06A, tmpZero);
		O07A = _mm256_unpacklo_epi8(OO07A, tmpZero);
		O07B = _mm256_unpackhi_epi8(OO07A, tmpZero);
		P00A = _mm256_unpacklo_epi8(PP00A, tmpZero);
		P00B = _mm256_unpackhi_epi8(PP00A, tmpZero);
		P01A = _mm256_unpacklo_epi8(PP01A, tmpZero);
		P01B = _mm256_unpackhi_epi8(PP01A, tmpZero);
		P02A = _mm256_unpacklo_epi8(PP02A, tmpZero);
		P02B = _mm256_unpackhi_epi8(PP02A, tmpZero);
		P03A = _mm256_unpacklo_epi8(PP03A, tmpZero);
		P03B = _mm256_unpackhi_epi8(PP03A, tmpZero);
		P04A = _mm256_unpacklo_epi8(PP04A, tmpZero);
		P04B = _mm256_unpackhi_epi8(PP04A, tmpZero);
		P05A = _mm256_unpacklo_epi8(PP05A, tmpZero);
		P05B = _mm256_unpackhi_epi8(PP05A, tmpZero);
		P06A = _mm256_unpacklo_epi8(PP06A, tmpZero);
		P06B = _mm256_unpackhi_epi8(PP06A, tmpZero);
		P07A = _mm256_unpacklo_epi8(PP07A, tmpZero);
		P07B = _mm256_unpackhi_epi8(PP07A, tmpZero);

		T00A = _mm256_sub_epi16(O00A, P00A);
		T00B = _mm256_sub_epi16(O00B, P00B);
		T01A = _mm256_sub_epi16(O01A, P01A);
		T01B = _mm256_sub_epi16(O01B, P01B);
		T02A = _mm256_sub_epi16(O02A, P02A);
		T02B = _mm256_sub_epi16(O02B, P02B);
		T03A = _mm256_sub_epi16(O03A, P03A);
		T03B = _mm256_sub_epi16(O03B, P03B);
		T04A = _mm256_sub_epi16(O04A, P04A);
		T04B = _mm256_sub_epi16(O04B, P04B);
		T05A = _mm256_sub_epi16(O05A, P05A);
		T05B = _mm256_sub_epi16(O05B, P05B);
		T06A = _mm256_sub_epi16(O06A, P06A);
		T06B = _mm256_sub_epi16(O06B, P06B);
		T07A = _mm256_sub_epi16(O07A, P07A);
		T07B = _mm256_sub_epi16(O07B, P07B);

		OO00B = _mm256_permute2x128_si256(O00, O08, 0x31);
		OO01B = _mm256_permute2x128_si256(O01, O09, 0x31);
		OO02B = _mm256_permute2x128_si256(O02, O10, 0x31);
		OO03B = _mm256_permute2x128_si256(O03, O11, 0x31);
		OO04B = _mm256_permute2x128_si256(O04, O12, 0x31);
		OO05B = _mm256_permute2x128_si256(O05, O13, 0x31);
		OO06B = _mm256_permute2x128_si256(O06, O14, 0x31);
		OO07B = _mm256_permute2x128_si256(O07, O15, 0x31);
		PP00B = _mm256_permute2x128_si256(P00, P08, 0x31);
		PP01B = _mm256_permute2x128_si256(P01, P09, 0x31);
		PP02B = _mm256_permute2x128_si256(P02, P10, 0x31);
		PP03B = _mm256_permute2x128_si256(P03, P11, 0x31);
		PP04B = _mm256_permute2x128_si256(P04, P12, 0x31);
		PP05B = _mm256_permute2x128_si256(P05, P13, 0x31);
		PP06B = _mm256_permute2x128_si256(P06, P14, 0x31);
		PP07B = _mm256_permute2x128_si256(P07, P15, 0x31);
		
		O00A = _mm256_unpacklo_epi8(OO00B, tmpZero);
		O00B = _mm256_unpackhi_epi8(OO00B, tmpZero);
		O01A = _mm256_unpacklo_epi8(OO01B, tmpZero);
		O01B = _mm256_unpackhi_epi8(OO01B, tmpZero);
		O02A = _mm256_unpacklo_epi8(OO02B, tmpZero);
		O02B = _mm256_unpackhi_epi8(OO02B, tmpZero);
		O03A = _mm256_unpacklo_epi8(OO03B, tmpZero);
		O03B = _mm256_unpackhi_epi8(OO03B, tmpZero);
		O04A = _mm256_unpacklo_epi8(OO04B, tmpZero);
		O04B = _mm256_unpackhi_epi8(OO04B, tmpZero);
		O05A = _mm256_unpacklo_epi8(OO05B, tmpZero);
		O05B = _mm256_unpackhi_epi8(OO05B, tmpZero);
		O06A = _mm256_unpacklo_epi8(OO06B, tmpZero);
		O06B = _mm256_unpackhi_epi8(OO06B, tmpZero);
		O07A = _mm256_unpacklo_epi8(OO07B, tmpZero);
		O07B = _mm256_unpackhi_epi8(OO07B, tmpZero);
		P00A = _mm256_unpacklo_epi8(PP00B, tmpZero);
		P00B = _mm256_unpackhi_epi8(PP00B, tmpZero);
		P01A = _mm256_unpacklo_epi8(PP01B, tmpZero);
		P01B = _mm256_unpackhi_epi8(PP01B, tmpZero);
		P02A = _mm256_unpacklo_epi8(PP02B, tmpZero);
		P02B = _mm256_unpackhi_epi8(PP02B, tmpZero);
		P03A = _mm256_unpacklo_epi8(PP03B, tmpZero);
		P03B = _mm256_unpackhi_epi8(PP03B, tmpZero);
		P04A = _mm256_unpacklo_epi8(PP04B, tmpZero);
		P04B = _mm256_unpackhi_epi8(PP04B, tmpZero);
		P05A = _mm256_unpacklo_epi8(PP05B, tmpZero);
		P05B = _mm256_unpackhi_epi8(PP05B, tmpZero);
		P06A = _mm256_unpacklo_epi8(PP06B, tmpZero);
		P06B = _mm256_unpackhi_epi8(PP06B, tmpZero);
		P07A = _mm256_unpacklo_epi8(PP07B, tmpZero);
		P07B = _mm256_unpackhi_epi8(PP07B, tmpZero);
		T00C = _mm256_sub_epi16(O00A, P00A);
		T00D = _mm256_sub_epi16(O00B, P00B);
		T01C = _mm256_sub_epi16(O01A, P01A);
		T01D = _mm256_sub_epi16(O01B, P01B);
		T02C = _mm256_sub_epi16(O02A, P02A);
		T02D = _mm256_sub_epi16(O02B, P02B);
		T03C = _mm256_sub_epi16(O03A, P03A);
		T03D = _mm256_sub_epi16(O03B, P03B);
		T04C = _mm256_sub_epi16(O04A, P04A);
		T04D = _mm256_sub_epi16(O04B, P04B);
		T05C = _mm256_sub_epi16(O05A, P05A);
		T05D = _mm256_sub_epi16(O05B, P05B);
		T06C = _mm256_sub_epi16(O06A, P06A);
		T06D = _mm256_sub_epi16(O06B, P06B);
		T07C = _mm256_sub_epi16(O07A, P07A);
		T07D = _mm256_sub_epi16(O07B, P07B);
		org += i_org << 4;
		pred += i_pred << 4;

		//
		TAB16_0_1 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[1]);
		TAB32_0_0 = _mm256_load_si256((__m256i*)tab_dct_32_0_256i[0]);
		T00A = _mm256_shuffle_epi8(T00A, TAB16_0_1);    // [05 02 06 01 04 03 07 00]
		T00B = _mm256_shuffle_epi8(T00B, TAB32_0_0);    // [10 13 09 14 11 12 08 15]
		T00C = _mm256_shuffle_epi8(T00C, TAB16_0_1);    // [21 18 22 17 20 19 23 16]
		T00D = _mm256_shuffle_epi8(T00D, TAB32_0_0);    // [26 29 25 30 27 28 24 31]
		T01A = _mm256_shuffle_epi8(T01A, TAB16_0_1);
		T01B = _mm256_shuffle_epi8(T01B, TAB32_0_0);
		T01C = _mm256_shuffle_epi8(T01C, TAB16_0_1);
		T01D = _mm256_shuffle_epi8(T01D, TAB32_0_0);
		T02A = _mm256_shuffle_epi8(T02A, TAB16_0_1);
		T02B = _mm256_shuffle_epi8(T02B, TAB32_0_0);
		T02C = _mm256_shuffle_epi8(T02C, TAB16_0_1);
		T02D = _mm256_shuffle_epi8(T02D, TAB32_0_0);
		T03A = _mm256_shuffle_epi8(T03A, TAB16_0_1);
		T03B = _mm256_shuffle_epi8(T03B, TAB32_0_0);
		T03C = _mm256_shuffle_epi8(T03C, TAB16_0_1);
		T03D = _mm256_shuffle_epi8(T03D, TAB32_0_0);
		T04A = _mm256_shuffle_epi8(T04A, TAB16_0_1);
		T04B = _mm256_shuffle_epi8(T04B, TAB32_0_0);
		T04C = _mm256_shuffle_epi8(T04C, TAB16_0_1);
		T04D = _mm256_shuffle_epi8(T04D, TAB32_0_0);
		T05A = _mm256_shuffle_epi8(T05A, TAB16_0_1);
		T05B = _mm256_shuffle_epi8(T05B, TAB32_0_0);
		T05C = _mm256_shuffle_epi8(T05C, TAB16_0_1);
		T05D = _mm256_shuffle_epi8(T05D, TAB32_0_0);
		T06A = _mm256_shuffle_epi8(T06A, TAB16_0_1);
		T06B = _mm256_shuffle_epi8(T06B, TAB32_0_0);
		T06C = _mm256_shuffle_epi8(T06C, TAB16_0_1);
		T06D = _mm256_shuffle_epi8(T06D, TAB32_0_0);
		T07A = _mm256_shuffle_epi8(T07A, TAB16_0_1);
		T07B = _mm256_shuffle_epi8(T07B, TAB32_0_0);
		T07C = _mm256_shuffle_epi8(T07C, TAB16_0_1);
		T07D = _mm256_shuffle_epi8(T07D, TAB32_0_0);

		T10A = _mm256_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm256_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm256_add_epi16(T01A, T01D);
		T11B = _mm256_add_epi16(T01B, T01C);
		T12A = _mm256_add_epi16(T02A, T02D);
		T12B = _mm256_add_epi16(T02B, T02C);
		T13A = _mm256_add_epi16(T03A, T03D);
		T13B = _mm256_add_epi16(T03B, T03C);
		T14A = _mm256_add_epi16(T04A, T04D);
		T14B = _mm256_add_epi16(T04B, T04C);
		T15A = _mm256_add_epi16(T05A, T05D);
		T15B = _mm256_add_epi16(T05B, T05C);
		T16A = _mm256_add_epi16(T06A, T06D);
		T16B = _mm256_add_epi16(T06B, T06C);
		T17A = _mm256_add_epi16(T07A, T07D);
		T17B = _mm256_add_epi16(T07B, T07C);

		T00A = _mm256_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm256_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm256_sub_epi16(T01A, T01D);
		T01B = _mm256_sub_epi16(T01B, T01C);
		T02A = _mm256_sub_epi16(T02A, T02D);
		T02B = _mm256_sub_epi16(T02B, T02C);
		T03A = _mm256_sub_epi16(T03A, T03D);
		T03B = _mm256_sub_epi16(T03B, T03C);
		T04A = _mm256_sub_epi16(T04A, T04D);
		T04B = _mm256_sub_epi16(T04B, T04C);
		T05A = _mm256_sub_epi16(T05A, T05D);
		T05B = _mm256_sub_epi16(T05B, T05C);
		T06A = _mm256_sub_epi16(T06A, T06D);
		T06B = _mm256_sub_epi16(T06B, T06C);
		T07A = _mm256_sub_epi16(T07A, T07D);
		T07B = _mm256_sub_epi16(T07B, T07C);

		T20 = _mm256_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm256_add_epi16(T11A, T11B);
		T22 = _mm256_add_epi16(T12A, T12B);
		T23 = _mm256_add_epi16(T13A, T13B);
		T24 = _mm256_add_epi16(T14A, T14B);
		T25 = _mm256_add_epi16(T15A, T15B);
		T26 = _mm256_add_epi16(T16A, T16B);
		T27 = _mm256_add_epi16(T17A, T17B);

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[0][i] = T60;

		T50 = _mm256_hsub_epi32(T40, T41);
		T51 = _mm256_hsub_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[16][i] = T60;

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[8][i] = T60;

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[24][i] = T60;

#define MAKE_ODD(tab, dstPos) \
	T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	\
	T40 = _mm256_hadd_epi32(T30, T31); \
	T41 = _mm256_hadd_epi32(T32, T33); \
	T42 = _mm256_hadd_epi32(T34, T35); \
	T43 = _mm256_hadd_epi32(T36, T37); \
	\
	T50 = _mm256_hadd_epi32(T40, T41); \
	T51 = _mm256_hadd_epi32(T42, T43); \
	T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1); \
	T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1); \
	T60 = _mm256_packs_epi32(T50, T51); \
	im[(dstPos)][i] = T60;

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);
		MAKE_ODD(2, 20);
		MAKE_ODD(3, 28);

		T20 = _mm256_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm256_sub_epi16(T11A, T11B);
		T22 = _mm256_sub_epi16(T12A, T12B);
		T23 = _mm256_sub_epi16(T13A, T13B);
		T24 = _mm256_sub_epi16(T14A, T14B);
		T25 = _mm256_sub_epi16(T15A, T15B);
		T26 = _mm256_sub_epi16(T16A, T16B);
		T27 = _mm256_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);
		MAKE_ODD(8, 18);
		MAKE_ODD(9, 22);
		MAKE_ODD(10, 26);
		MAKE_ODD(11, 30);
#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
	T20 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T21 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T22 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T23 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T24 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T25 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T26 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T27 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T30 = _mm256_madd_epi16(T04A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T31 = _mm256_madd_epi16(T04B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T32 = _mm256_madd_epi16(T05A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T33 = _mm256_madd_epi16(T05B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T34 = _mm256_madd_epi16(T06A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T35 = _mm256_madd_epi16(T06B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T36 = _mm256_madd_epi16(T07A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T37 = _mm256_madd_epi16(T07B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	\
	T40 = _mm256_hadd_epi32(T20, T21); \
	T41 = _mm256_hadd_epi32(T22, T23); \
	T42 = _mm256_hadd_epi32(T24, T25); \
	T43 = _mm256_hadd_epi32(T26, T27); \
	T44 = _mm256_hadd_epi32(T30, T31); \
	T45 = _mm256_hadd_epi32(T32, T33); \
	T46 = _mm256_hadd_epi32(T34, T35); \
	T47 = _mm256_hadd_epi32(T36, T37); \
	\
	T50 = _mm256_hadd_epi32(T40, T41); \
	T51 = _mm256_hadd_epi32(T42, T43); \
	T52 = _mm256_hadd_epi32(T44, T45); \
	T53 = _mm256_hadd_epi32(T46, T47); \
	\
	T50 = _mm256_hadd_epi32(T50, T51); \
	T51 = _mm256_hadd_epi32(T52, T53); \
	T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1); \
	T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1); \
	T60 = _mm256_packs_epi32(T50, T51); \
	im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);
		MAKE_ODD(28, 17);
		MAKE_ODD(30, 19);
		MAKE_ODD(32, 21);
		MAKE_ODD(34, 23);
		MAKE_ODD(36, 25);
		MAKE_ODD(38, 27);
		MAKE_ODD(40, 29);
		MAKE_ODD(42, 31);

#undef MAKE_ODD
	}

	// DCT2
	for (i = 0; i < 2; i++)
	{
		T00A = _mm256_permute2x128_si256(im[i * 16 + 0][0], im[i * 16 + 8][0], 0x20);
		T00B = _mm256_permute2x128_si256(im[i * 16 + 0][0], im[i * 16 + 8][0], 0x31);
		T01A = _mm256_permute2x128_si256(im[i * 16 + 1][0], im[i * 16 + 9][0], 0x20);
		T01B = _mm256_permute2x128_si256(im[i * 16 + 1][0], im[i * 16 + 9][0], 0x31);
		T02A = _mm256_permute2x128_si256(im[i * 16 + 2][0], im[i * 16 + 10][0], 0x20);
		T02B = _mm256_permute2x128_si256(im[i * 16 + 2][0], im[i * 16 + 10][0], 0x31);
		T03A = _mm256_permute2x128_si256(im[i * 16 + 3][0], im[i * 16 + 11][0], 0x20);
		T03B = _mm256_permute2x128_si256(im[i * 16 + 3][0], im[i * 16 + 11][0], 0x31);
		TT00A = _mm256_permute2x128_si256(im[i * 16 + 4][0], im[i * 16 + 12][0], 0x20);
		TT00B = _mm256_permute2x128_si256(im[i * 16 + 4][0], im[i * 16 + 12][0], 0x31);
		TT01A = _mm256_permute2x128_si256(im[i * 16 + 5][0], im[i * 16 + 13][0], 0x20);
		TT01B = _mm256_permute2x128_si256(im[i * 16 + 5][0], im[i * 16 + 13][0], 0x31);
		TT02A = _mm256_permute2x128_si256(im[i * 16 + 6][0], im[i * 16 + 14][0], 0x20);
		TT02B = _mm256_permute2x128_si256(im[i * 16 + 6][0], im[i * 16 + 14][0], 0x31);
		TT03A = _mm256_permute2x128_si256(im[i * 16 + 7][0], im[i * 16 + 15][0], 0x20);
		TT03B = _mm256_permute2x128_si256(im[i * 16 + 7][0], im[i * 16 + 15][0], 0x31);
		T00C = _mm256_permute2x128_si256(im[i * 16 + 0][1], im[i * 16 + 8][1], 0x20);
		T00D = _mm256_permute2x128_si256(im[i * 16 + 0][1], im[i * 16 + 8][1], 0x31);
		T01C = _mm256_permute2x128_si256(im[i * 16 + 1][1], im[i * 16 + 9][1], 0x20);
		T01D = _mm256_permute2x128_si256(im[i * 16 + 1][1], im[i * 16 + 9][1], 0x31);
		T02C = _mm256_permute2x128_si256(im[i * 16 + 2][1], im[i * 16 + 10][1], 0x20);
		T02D = _mm256_permute2x128_si256(im[i * 16 + 2][1], im[i * 16 + 10][1], 0x31);
		T03C = _mm256_permute2x128_si256(im[i * 16 + 3][1], im[i * 16 + 11][1], 0x20);
		T03D = _mm256_permute2x128_si256(im[i * 16 + 3][1], im[i * 16 + 11][1], 0x31);
		TT00C = _mm256_permute2x128_si256(im[i * 16 + 4][1], im[i * 16 + 12][1], 0x20);
		TT00D = _mm256_permute2x128_si256(im[i * 16 + 4][1], im[i * 16 + 12][1], 0x31);
		TT01C = _mm256_permute2x128_si256(im[i * 16 + 5][1], im[i * 16 + 13][1], 0x20);
		TT01D = _mm256_permute2x128_si256(im[i * 16 + 5][1], im[i * 16 + 13][1], 0x31);
		TT02C = _mm256_permute2x128_si256(im[i * 16 + 6][1], im[i * 16 + 14][1], 0x20);
		TT02D = _mm256_permute2x128_si256(im[i * 16 + 6][1], im[i * 16 + 14][1], 0x31);
		TT03C = _mm256_permute2x128_si256(im[i * 16 + 7][1], im[i * 16 + 15][1], 0x20);
		TT03D = _mm256_permute2x128_si256(im[i * 16 + 7][1], im[i * 16 + 15][1], 0x31);

		TAB16_0_0 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[0]);
		T00C = _mm256_shuffle_epi8(T00C, TAB16_0_0);    // [16 17 18 19 20 21 22 23]
		T00D = _mm256_shuffle_epi8(T00D, TAB16_0_0);    // [24 25 26 27 28 29 30 31]
		T01C = _mm256_shuffle_epi8(T01C, TAB16_0_0);
		T01D = _mm256_shuffle_epi8(T01D, TAB16_0_0);
		T02C = _mm256_shuffle_epi8(T02C, TAB16_0_0);
		T02D = _mm256_shuffle_epi8(T02D, TAB16_0_0);
		T03C = _mm256_shuffle_epi8(T03C, TAB16_0_0);
		T03D = _mm256_shuffle_epi8(T03D, TAB16_0_0);

		TT00C = _mm256_shuffle_epi8(TT00C, TAB16_0_0);
		TT00D = _mm256_shuffle_epi8(TT00D, TAB16_0_0);
		TT01C = _mm256_shuffle_epi8(TT01C, TAB16_0_0);
		TT01D = _mm256_shuffle_epi8(TT01D, TAB16_0_0);
		TT02C = _mm256_shuffle_epi8(TT02C, TAB16_0_0);
		TT02D = _mm256_shuffle_epi8(TT02D, TAB16_0_0);
		TT03C = _mm256_shuffle_epi8(TT03C, TAB16_0_0);
		TT03D = _mm256_shuffle_epi8(TT03D, TAB16_0_0);

		T10A = _mm256_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
		T10B = _mm256_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
		T00A = _mm256_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
		T00B = _mm256_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
		T11A = _mm256_unpacklo_epi16(T01A, T01D);
		T11B = _mm256_unpackhi_epi16(T01A, T01D);
		T01A = _mm256_unpacklo_epi16(T01B, T01C);
		T01B = _mm256_unpackhi_epi16(T01B, T01C);
		T12A = _mm256_unpacklo_epi16(T02A, T02D);
		T12B = _mm256_unpackhi_epi16(T02A, T02D);
		T02A = _mm256_unpacklo_epi16(T02B, T02C);
		T02B = _mm256_unpackhi_epi16(T02B, T02C);
		T13A = _mm256_unpacklo_epi16(T03A, T03D);
		T13B = _mm256_unpackhi_epi16(T03A, T03D);
		T03A = _mm256_unpacklo_epi16(T03B, T03C);
		T03B = _mm256_unpackhi_epi16(T03B, T03C);

		TT10A = _mm256_unpacklo_epi16(TT00A, TT00D);
		TT10B = _mm256_unpackhi_epi16(TT00A, TT00D);
		TT00A = _mm256_unpacklo_epi16(TT00B, TT00C);
		TT00B = _mm256_unpackhi_epi16(TT00B, TT00C);
		TT11A = _mm256_unpacklo_epi16(TT01A, TT01D);
		TT11B = _mm256_unpackhi_epi16(TT01A, TT01D);
		TT01A = _mm256_unpacklo_epi16(TT01B, TT01C);
		TT01B = _mm256_unpackhi_epi16(TT01B, TT01C);
		TT12A = _mm256_unpacklo_epi16(TT02A, TT02D);
		TT12B = _mm256_unpackhi_epi16(TT02A, TT02D);
		TT02A = _mm256_unpacklo_epi16(TT02B, TT02C);
		TT02B = _mm256_unpackhi_epi16(TT02B, TT02C);
		TT13A = _mm256_unpacklo_epi16(TT03A, TT03D);
		TT13B = _mm256_unpackhi_epi16(TT03A, TT03D);
		TT03A = _mm256_unpacklo_epi16(TT03B, TT03C);
		TT03B = _mm256_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
	T20 = _mm256_madd_epi16(T10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T21 = _mm256_madd_epi16(T10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T22 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T23 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T24 = _mm256_madd_epi16(T11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T25 = _mm256_madd_epi16(T11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T26 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T27 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T30 = _mm256_madd_epi16(T12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T31 = _mm256_madd_epi16(T12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T32 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T33 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T34 = _mm256_madd_epi16(T13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T35 = _mm256_madd_epi16(T13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T36 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T37 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	T60 = _mm256_hadd_epi32(T20, T21); \
	T61 = _mm256_hadd_epi32(T22, T23); \
	T62 = _mm256_hadd_epi32(T24, T25); \
	T63 = _mm256_hadd_epi32(T26, T27); \
	T64 = _mm256_hadd_epi32(T30, T31); \
	T65 = _mm256_hadd_epi32(T32, T33); \
	T66 = _mm256_hadd_epi32(T34, T35); \
	T67 = _mm256_hadd_epi32(T36, T37); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	T62 = _mm256_hadd_epi32(T64, T65); \
	T63 = _mm256_hadd_epi32(T66, T67); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	\
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_512), shift2); \
	\
	TT20 = _mm256_madd_epi16(TT10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT21 = _mm256_madd_epi16(TT10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT22 = _mm256_madd_epi16(TT00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT23 = _mm256_madd_epi16(TT00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT24 = _mm256_madd_epi16(TT11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT25 = _mm256_madd_epi16(TT11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT26 = _mm256_madd_epi16(TT01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT27 = _mm256_madd_epi16(TT01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT30 = _mm256_madd_epi16(TT12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT31 = _mm256_madd_epi16(TT12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT32 = _mm256_madd_epi16(TT02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT33 = _mm256_madd_epi16(TT02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT34 = _mm256_madd_epi16(TT13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT35 = _mm256_madd_epi16(TT13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT36 = _mm256_madd_epi16(TT03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT37 = _mm256_madd_epi16(TT03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	TT60 = _mm256_hadd_epi32(TT20, TT21); \
	TT61 = _mm256_hadd_epi32(TT22, TT23); \
	TT62 = _mm256_hadd_epi32(TT24, TT25); \
	TT63 = _mm256_hadd_epi32(TT26, TT27); \
	TT64 = _mm256_hadd_epi32(TT30, TT31); \
	TT65 = _mm256_hadd_epi32(TT32, TT33); \
	TT66 = _mm256_hadd_epi32(TT34, TT35); \
	TT67 = _mm256_hadd_epi32(TT36, TT37); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	TT62 = _mm256_hadd_epi32(TT64, TT65); \
	TT63 = _mm256_hadd_epi32(TT66, TT67); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	\
	TT60 = _mm256_srai_epi32(_mm256_add_epi32(TT60, c_512), shift2); \
	\
	tResult = _mm256_packs_epi32(T60, TT60); \
	_mm256_storeu_si256((__m256i*)&dst[(dstPos)* 32 + (i * 16) + 0], tResult); \

		MAKE_ODD(44, 44, 44, 44, 0);
		MAKE_ODD(45, 45, 45, 45, 16);
		MAKE_ODD(46, 47, 46, 47, 8);
		MAKE_ODD(48, 49, 48, 49, 24);

		MAKE_ODD(50, 51, 52, 53, 4);
		MAKE_ODD(54, 55, 56, 57, 12);
		MAKE_ODD(58, 59, 60, 61, 20);
		MAKE_ODD(62, 63, 64, 65, 28);

		MAKE_ODD(66, 67, 68, 69, 2);
		MAKE_ODD(70, 71, 72, 73, 6);
		MAKE_ODD(74, 75, 76, 77, 10);
		MAKE_ODD(78, 79, 80, 81, 14);

		MAKE_ODD(82, 83, 84, 85, 18);
		MAKE_ODD(86, 87, 88, 89, 22);
		MAKE_ODD(90, 91, 92, 93, 26);
		MAKE_ODD(94, 95, 96, 97, 30);

		MAKE_ODD(98, 99, 100, 101, 1);
		MAKE_ODD(102, 103, 104, 105, 3);
		MAKE_ODD(106, 107, 108, 109, 5);
		MAKE_ODD(110, 111, 112, 113, 7);
		MAKE_ODD(114, 115, 116, 117, 9);
		MAKE_ODD(118, 119, 120, 121, 11);
		MAKE_ODD(122, 123, 124, 125, 13);
		MAKE_ODD(126, 127, 128, 129, 15);
		MAKE_ODD(130, 131, 132, 133, 17);
		MAKE_ODD(134, 135, 136, 137, 19);
		MAKE_ODD(138, 139, 140, 141, 21);
		MAKE_ODD(142, 143, 144, 145, 23);
		MAKE_ODD(146, 147, 148, 149, 25);
		MAKE_ODD(150, 151, 152, 153, 27);
		MAKE_ODD(154, 155, 156, 157, 29);
		MAKE_ODD(158, 159, 160, 161, 31);
#undef MAKE_ODD
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// dct 64x64 & 64x16 & 16x64/////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#define TRANSPOSE_8x8_16BIT(I0, I1, I2, I3, I4, I5, I6, I7, O0, O1, O2, O3, O4, O5, O6, O7) \
    tr0_0 = _mm_unpacklo_epi16(I0, I1); \
    tr0_1 = _mm_unpacklo_epi16(I2, I3); \
    tr0_2 = _mm_unpackhi_epi16(I0, I1); \
    tr0_3 = _mm_unpackhi_epi16(I2, I3); \
    tr0_4 = _mm_unpacklo_epi16(I4, I5); \
    tr0_5 = _mm_unpacklo_epi16(I6, I7); \
    tr0_6 = _mm_unpackhi_epi16(I4, I5); \
    tr0_7 = _mm_unpackhi_epi16(I6, I7); \
    tr1_0 = _mm_unpacklo_epi32(tr0_0, tr0_1); \
    tr1_1 = _mm_unpacklo_epi32(tr0_2, tr0_3); \
    tr1_2 = _mm_unpackhi_epi32(tr0_0, tr0_1); \
    tr1_3 = _mm_unpackhi_epi32(tr0_2, tr0_3); \
    tr1_4 = _mm_unpacklo_epi32(tr0_4, tr0_5); \
    tr1_5 = _mm_unpacklo_epi32(tr0_6, tr0_7); \
    tr1_6 = _mm_unpackhi_epi32(tr0_4, tr0_5); \
    tr1_7 = _mm_unpackhi_epi32(tr0_6, tr0_7); \
    O0 = _mm_unpacklo_epi64(tr1_0, tr1_4); \
    O1 = _mm_unpackhi_epi64(tr1_0, tr1_4); \
    O2 = _mm_unpacklo_epi64(tr1_2, tr1_6); \
    O3 = _mm_unpackhi_epi64(tr1_2, tr1_6); \
    O4 = _mm_unpacklo_epi64(tr1_1, tr1_5); \
    O5 = _mm_unpackhi_epi64(tr1_1, tr1_5); \
    O6 = _mm_unpacklo_epi64(tr1_3, tr1_7); \
    O7 = _mm_unpackhi_epi64(tr1_3, tr1_7); \

void sub_trans_64x64_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	int i;
	resi_t resi[32 * 32];
    resi_t *temp_resi;
	__m128i T[64][8], V[64][8];
	__m128i O00, O01, O02, O03, O04, O05, O06, O07;
	__m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m128i P00, P01, P02, P03, P04, P05, P06, P07;
	__m128i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m128i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
	__m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
	__m128i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m128i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m128i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m128i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m128i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m128i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m128i T20, T21, T22, T23, T24, T25, T26, T27;
	__m128i T30, T31, T32, T33, T34, T35, T36, T37;
	__m128i T40, T41, T42, T43, T44, T45, T46, T47;
	__m128i T50, T51, T52, T53;
	__m128i T60, T61, T62, T63, T64, T65, T66, T67;
	__m128i im[32][4];
	__m128i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m128i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m128i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m128i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m128i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m128i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m128i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m128i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m128i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m128i TT50, TT51, TT52, TT53;
	__m128i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m128i tResult;

	__m128i tmpZero = _mm_setzero_si128();
	__m128i k1 = _mm_set1_epi16(1);
	__m128i k2 = _mm_set1_epi16(2);
	__m128i tt;
	__m128i c_8 = _mm_set1_epi32(8);
	__m128i c_512 = _mm_set1_epi32(512);
	int shift1 = 4;
	int shift2 = 10;

	//// pix sub & wavelet ////
	for (i = 0; i < 8; i++){
		//load data
		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org]);
		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred]);
		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][0] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][1] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][0] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][1] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][0] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][1] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][0] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][1] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][0] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][1] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][0] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][1] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][0] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][1] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][0] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][1] = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 16]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 16]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 16]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 16]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 16]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 16]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 16]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 16]);
		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 16]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 16]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 16]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 16]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 16]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 16]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 16]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 16]);
		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][2] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][3] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][2] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][3] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][2] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][3] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][2] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][3] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][2] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][3] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][2] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][3] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][2] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][3] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][2] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][3] = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 32]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 32]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 32]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 32]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 32]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 32]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 32]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 32]);
		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 32]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 32]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 32]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 32]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 32]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 32]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 32]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 32]);
		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][4] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][5] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][4] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][5] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][4] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][5] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][4] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][5] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][4] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][5] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][4] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][5] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][4] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][5] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][4] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][5] = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 48]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 48]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 48]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 48]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 48]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 48]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 48]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 48]);
        O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 48]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 48]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 48]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 48]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 48]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 48]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 48]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 48]);

        org  += i_org << 3;
        pred += i_pred << 3;

		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][6] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][7] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][6] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][7] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][6] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][7] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][6] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][7] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][6] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][7] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][6] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][7] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][6] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][7] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][6] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][7] = _mm_sub_epi16(O07B, P07B);

		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][0], T[i * 8 + 1][0], T[i * 8 + 2][0], T[i * 8 + 3][0], T[i * 8 + 4][0], T[i * 8 + 5][0], T[i * 8 + 6][0], T[i * 8 + 7][0], V[0][i], V[1][i], V[2][i], V[3][i], V[4][i], V[5][i], V[6][i], V[7][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][1], T[i * 8 + 1][1], T[i * 8 + 2][1], T[i * 8 + 3][1], T[i * 8 + 4][1], T[i * 8 + 5][1], T[i * 8 + 6][1], T[i * 8 + 7][1], V[8][i], V[9][i], V[10][i], V[11][i], V[12][i], V[13][i], V[14][i], V[15][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][2], T[i * 8 + 1][2], T[i * 8 + 2][2], T[i * 8 + 3][2], T[i * 8 + 4][2], T[i * 8 + 5][2], T[i * 8 + 6][2], T[i * 8 + 7][2], V[16][i], V[17][i], V[18][i], V[19][i], V[20][i], V[21][i], V[22][i], V[23][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][3], T[i * 8 + 1][3], T[i * 8 + 2][3], T[i * 8 + 3][3], T[i * 8 + 4][3], T[i * 8 + 5][3], T[i * 8 + 6][3], T[i * 8 + 7][3], V[24][i], V[25][i], V[26][i], V[27][i], V[28][i], V[29][i], V[30][i], V[31][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][4], T[i * 8 + 1][4], T[i * 8 + 2][4], T[i * 8 + 3][4], T[i * 8 + 4][4], T[i * 8 + 5][4], T[i * 8 + 6][4], T[i * 8 + 7][4], V[32][i], V[33][i], V[34][i], V[35][i], V[36][i], V[37][i], V[38][i], V[39][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][5], T[i * 8 + 1][5], T[i * 8 + 2][5], T[i * 8 + 3][5], T[i * 8 + 4][5], T[i * 8 + 5][5], T[i * 8 + 6][5], T[i * 8 + 7][5], V[40][i], V[41][i], V[42][i], V[43][i], V[44][i], V[45][i], V[46][i], V[47][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][6], T[i * 8 + 1][6], T[i * 8 + 2][6], T[i * 8 + 3][6], T[i * 8 + 4][6], T[i * 8 + 5][6], T[i * 8 + 6][6], T[i * 8 + 7][6], V[48][i], V[49][i], V[50][i], V[51][i], V[52][i], V[53][i], V[54][i], V[55][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][7], T[i * 8 + 1][7], T[i * 8 + 2][7], T[i * 8 + 3][7], T[i * 8 + 4][7], T[i * 8 + 5][7], T[i * 8 + 6][7], T[i * 8 + 7][7], V[56][i], V[57][i], V[58][i], V[59][i], V[60][i], V[61][i], V[62][i], V[63][i]);

		//filter H
		tt = _mm_srai_epi16(_mm_add_epi16(V[0][i], V[2][i]), 1);
		V[1][i] = _mm_sub_epi16(V[1][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[2][i], V[4][i]), 1);
		V[3][i] = _mm_sub_epi16(V[3][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[4][i], V[6][i]), 1);
		V[5][i] = _mm_sub_epi16(V[5][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[6][i], V[8][i]), 1);
		V[7][i] = _mm_sub_epi16(V[7][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[8][i], V[10][i]), 1);
		V[9][i] = _mm_sub_epi16(V[9][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[10][i], V[12][i]), 1);
		V[11][i] = _mm_sub_epi16(V[11][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[12][i], V[14][i]), 1);
		V[13][i] = _mm_sub_epi16(V[13][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[14][i], V[16][i]), 1);
		V[15][i] = _mm_sub_epi16(V[15][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[16][i], V[18][i]), 1);
		V[17][i] = _mm_sub_epi16(V[17][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[18][i], V[20][i]), 1);
		V[19][i] = _mm_sub_epi16(V[19][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[20][i], V[22][i]), 1);
		V[21][i] = _mm_sub_epi16(V[21][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[22][i], V[24][i]), 1);
		V[23][i] = _mm_sub_epi16(V[23][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[24][i], V[26][i]), 1);
		V[25][i] = _mm_sub_epi16(V[25][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[26][i], V[28][i]), 1);
		V[27][i] = _mm_sub_epi16(V[27][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[28][i], V[30][i]), 1);
		V[29][i] = _mm_sub_epi16(V[29][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[30][i], V[32][i]), 1);
		V[31][i] = _mm_sub_epi16(V[31][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[32][i], V[34][i]), 1);
		V[33][i] = _mm_sub_epi16(V[33][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[34][i], V[36][i]), 1);
		V[35][i] = _mm_sub_epi16(V[35][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[36][i], V[38][i]), 1);
		V[37][i] = _mm_sub_epi16(V[37][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[38][i], V[40][i]), 1);
		V[39][i] = _mm_sub_epi16(V[39][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[40][i], V[42][i]), 1);
		V[41][i] = _mm_sub_epi16(V[41][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[42][i], V[44][i]), 1);
		V[43][i] = _mm_sub_epi16(V[43][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[44][i], V[46][i]), 1);
		V[45][i] = _mm_sub_epi16(V[45][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[46][i], V[48][i]), 1);
		V[47][i] = _mm_sub_epi16(V[47][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[48][i], V[50][i]), 1);
		V[49][i] = _mm_sub_epi16(V[49][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[50][i], V[52][i]), 1);
		V[51][i] = _mm_sub_epi16(V[51][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[52][i], V[54][i]), 1);
		V[53][i] = _mm_sub_epi16(V[53][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[54][i], V[56][i]), 1);
		V[55][i] = _mm_sub_epi16(V[55][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[56][i], V[58][i]), 1);
		V[57][i] = _mm_sub_epi16(V[57][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[58][i], V[60][i]), 1);
		V[59][i] = _mm_sub_epi16(V[59][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[60][i], V[62][i]), 1);
		V[61][i] = _mm_sub_epi16(V[61][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[62][i], V[62][i]), 1);
		V[63][i] = _mm_sub_epi16(V[63][i], tt);

		//filter L
		tt = _mm_add_epi16(_mm_add_epi16(V[1][i], V[1][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[0][i] = _mm_add_epi16(V[0][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[1][i], V[3][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[2][i] = _mm_add_epi16(V[2][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[3][i], V[5][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[4][i] = _mm_add_epi16(V[4][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[5][i], V[7][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[6][i] = _mm_add_epi16(V[6][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[7][i], V[9][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[8][i] = _mm_add_epi16(V[8][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[9][i], V[11][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[10][i] = _mm_add_epi16(V[10][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[11][i], V[13][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[12][i] = _mm_add_epi16(V[12][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[13][i], V[15][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[14][i] = _mm_add_epi16(V[14][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[15][i], V[17][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[16][i] = _mm_add_epi16(V[16][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[17][i], V[19][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[18][i] = _mm_add_epi16(V[18][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[19][i], V[21][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[20][i] = _mm_add_epi16(V[20][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[21][i], V[23][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[22][i] = _mm_add_epi16(V[22][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[23][i], V[25][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[24][i] = _mm_add_epi16(V[24][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[25][i], V[27][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[26][i] = _mm_add_epi16(V[26][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[27][i], V[29][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[28][i] = _mm_add_epi16(V[28][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[29][i], V[31][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[30][i] = _mm_add_epi16(V[30][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[31][i], V[33][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[32][i] = _mm_add_epi16(V[32][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[33][i], V[35][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[34][i] = _mm_add_epi16(V[34][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[35][i], V[37][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[36][i] = _mm_add_epi16(V[36][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[37][i], V[39][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[38][i] = _mm_add_epi16(V[38][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[39][i], V[41][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[40][i] = _mm_add_epi16(V[40][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[41][i], V[43][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[42][i] = _mm_add_epi16(V[42][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[43][i], V[45][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[44][i] = _mm_add_epi16(V[44][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[45][i], V[47][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[46][i] = _mm_add_epi16(V[46][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[47][i], V[49][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[48][i] = _mm_add_epi16(V[48][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[49][i], V[51][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[50][i] = _mm_add_epi16(V[50][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[51][i], V[53][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[52][i] = _mm_add_epi16(V[52][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[53][i], V[55][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[54][i] = _mm_add_epi16(V[54][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[55][i], V[57][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[56][i] = _mm_add_epi16(V[56][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[57][i], V[59][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[58][i] = _mm_add_epi16(V[58][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[59][i], V[61][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[60][i] = _mm_add_epi16(V[60][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[61][i], V[63][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[62][i] = _mm_add_epi16(V[62][i], tt);

		TRANSPOSE_8x8_16BIT(V[0][i], V[2][i], V[4][i], V[6][i], V[8][i], V[10][i], V[12][i], V[14][i], T[i * 8 + 0][0], T[i * 8 + 1][0], T[i * 8 + 2][0], T[i * 8 + 3][0], T[i * 8 + 4][0], T[i * 8 + 5][0], T[i * 8 + 6][0], T[i * 8 + 7][0]);
		TRANSPOSE_8x8_16BIT(V[16][i], V[18][i], V[20][i], V[22][i], V[24][i], V[26][i], V[28][i], V[30][i], T[i * 8 + 0][1], T[i * 8 + 1][1], T[i * 8 + 2][1], T[i * 8 + 3][1], T[i * 8 + 4][1], T[i * 8 + 5][1], T[i * 8 + 6][1], T[i * 8 + 7][1]);
		TRANSPOSE_8x8_16BIT(V[32][i], V[34][i], V[36][i], V[38][i], V[40][i], V[42][i], V[44][i], V[46][i], T[i * 8 + 0][2], T[i * 8 + 1][2], T[i * 8 + 2][2], T[i * 8 + 3][2], T[i * 8 + 4][2], T[i * 8 + 5][2], T[i * 8 + 6][2], T[i * 8 + 7][2]);
		TRANSPOSE_8x8_16BIT(V[48][i], V[50][i], V[52][i], V[54][i], V[56][i], V[58][i], V[60][i], V[62][i], T[i * 8 + 0][3], T[i * 8 + 1][3], T[i * 8 + 2][3], T[i * 8 + 3][3], T[i * 8 + 4][3], T[i * 8 + 5][3], T[i * 8 + 6][3], T[i * 8 + 7][3]);
	}

	for (i = 0; i < 4; i++){
		tt = _mm_srai_epi16(_mm_add_epi16(T[0][i], T[2][i]), 1);
		T[1][i] = _mm_sub_epi16(T[1][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[2][i], T[4][i]), 1);
		T[3][i] = _mm_sub_epi16(T[3][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[4][i], T[6][i]), 1);
		T[5][i] = _mm_sub_epi16(T[5][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[6][i], T[8][i]), 1);
		T[7][i] = _mm_sub_epi16(T[7][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[8][i], T[10][i]), 1);
		T[9][i] = _mm_sub_epi16(T[9][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[10][i], T[12][i]), 1);
		T[11][i] = _mm_sub_epi16(T[11][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[12][i], T[14][i]), 1);
		T[13][i] = _mm_sub_epi16(T[13][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[14][i], T[16][i]), 1);
		T[15][i] = _mm_sub_epi16(T[15][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[16][i], T[18][i]), 1);
		T[17][i] = _mm_sub_epi16(T[17][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[18][i], T[20][i]), 1);
		T[19][i] = _mm_sub_epi16(T[19][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[20][i], T[22][i]), 1);
		T[21][i] = _mm_sub_epi16(T[21][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[22][i], T[24][i]), 1);
		T[23][i] = _mm_sub_epi16(T[23][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[24][i], T[26][i]), 1);
		T[25][i] = _mm_sub_epi16(T[25][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[26][i], T[28][i]), 1);
		T[27][i] = _mm_sub_epi16(T[27][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[28][i], T[30][i]), 1);
		T[29][i] = _mm_sub_epi16(T[29][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[30][i], T[32][i]), 1);
		T[31][i] = _mm_sub_epi16(T[31][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[32][i], T[34][i]), 1);
		T[33][i] = _mm_sub_epi16(T[33][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[34][i], T[36][i]), 1);
		T[35][i] = _mm_sub_epi16(T[35][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[36][i], T[38][i]), 1);
		T[37][i] = _mm_sub_epi16(T[37][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[38][i], T[40][i]), 1);
		T[39][i] = _mm_sub_epi16(T[39][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[40][i], T[42][i]), 1);
		T[41][i] = _mm_sub_epi16(T[41][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[42][i], T[44][i]), 1);
		T[43][i] = _mm_sub_epi16(T[43][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[44][i], T[46][i]), 1);
		T[45][i] = _mm_sub_epi16(T[45][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[46][i], T[48][i]), 1);
		T[47][i] = _mm_sub_epi16(T[47][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[48][i], T[50][i]), 1);
		T[49][i] = _mm_sub_epi16(T[49][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[50][i], T[52][i]), 1);
		T[51][i] = _mm_sub_epi16(T[51][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[52][i], T[54][i]), 1);
		T[53][i] = _mm_sub_epi16(T[53][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[54][i], T[56][i]), 1);
		T[55][i] = _mm_sub_epi16(T[55][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[56][i], T[58][i]), 1);
		T[57][i] = _mm_sub_epi16(T[57][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[58][i], T[60][i]), 1);
		T[59][i] = _mm_sub_epi16(T[59][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[60][i], T[62][i]), 1);
		T[61][i] = _mm_sub_epi16(T[61][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[62][i], T[62][i]), 1);
		T[63][i] = _mm_sub_epi16(T[63][i], tt);

		tt = _mm_add_epi16(_mm_add_epi16(T[1][i], T[1][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[0][i] = _mm_slli_epi16(T[0][i], 1);
		T[0][i] = _mm_add_epi16(T[0][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[1][i], T[3][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[2][i] = _mm_slli_epi16(T[2][i], 1);
		T[2][i] = _mm_add_epi16(T[2][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[3][i], T[5][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[4][i] = _mm_slli_epi16(T[4][i], 1);
		T[4][i] = _mm_add_epi16(T[4][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[5][i], T[7][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[6][i] = _mm_slli_epi16(T[6][i], 1);
		T[6][i] = _mm_add_epi16(T[6][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[7][i], T[9][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[8][i] = _mm_slli_epi16(T[8][i], 1);
		T[8][i] = _mm_add_epi16(T[8][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[9][i], T[11][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[10][i] = _mm_slli_epi16(T[10][i], 1);
		T[10][i] = _mm_add_epi16(T[10][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[11][i], T[13][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[12][i] = _mm_slli_epi16(T[12][i], 1);
		T[12][i] = _mm_add_epi16(T[12][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[13][i], T[15][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[14][i] = _mm_slli_epi16(T[14][i], 1);
		T[14][i] = _mm_add_epi16(T[14][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[15][i], T[17][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[16][i] = _mm_slli_epi16(T[16][i], 1);
		T[16][i] = _mm_add_epi16(T[16][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[17][i], T[19][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[18][i] = _mm_slli_epi16(T[18][i], 1);
		T[18][i] = _mm_add_epi16(T[18][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[19][i], T[21][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[20][i] = _mm_slli_epi16(T[20][i], 1);
		T[20][i] = _mm_add_epi16(T[20][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[21][i], T[23][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[22][i] = _mm_slli_epi16(T[22][i], 1);
		T[22][i] = _mm_add_epi16(T[22][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[23][i], T[25][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[24][i] = _mm_slli_epi16(T[24][i], 1);
		T[24][i] = _mm_add_epi16(T[24][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[25][i], T[27][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[26][i] = _mm_slli_epi16(T[26][i], 1);
		T[26][i] = _mm_add_epi16(T[26][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[27][i], T[29][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[28][i] = _mm_slli_epi16(T[28][i], 1);
		T[28][i] = _mm_add_epi16(T[28][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[29][i], T[31][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[30][i] = _mm_slli_epi16(T[30][i], 1);
		T[30][i] = _mm_add_epi16(T[30][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[31][i], T[33][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[32][i] = _mm_slli_epi16(T[32][i], 1);
		T[32][i] = _mm_add_epi16(T[32][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[33][i], T[35][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[34][i] = _mm_slli_epi16(T[34][i], 1);
		T[34][i] = _mm_add_epi16(T[34][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[35][i], T[37][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[36][i] = _mm_slli_epi16(T[36][i], 1);
		T[36][i] = _mm_add_epi16(T[36][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[37][i], T[39][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[38][i] = _mm_slli_epi16(T[38][i], 1);
		T[38][i] = _mm_add_epi16(T[38][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[39][i], T[41][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[40][i] = _mm_slli_epi16(T[40][i], 1);
		T[40][i] = _mm_add_epi16(T[40][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[41][i], T[43][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[42][i] = _mm_slli_epi16(T[42][i], 1);
		T[42][i] = _mm_add_epi16(T[42][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[43][i], T[45][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[44][i] = _mm_slli_epi16(T[44][i], 1);
		T[44][i] = _mm_add_epi16(T[44][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[45][i], T[47][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[46][i] = _mm_slli_epi16(T[46][i], 1);
		T[46][i] = _mm_add_epi16(T[46][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[47][i], T[49][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[48][i] = _mm_slli_epi16(T[48][i], 1);
		T[48][i] = _mm_add_epi16(T[48][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[49][i], T[51][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[50][i] = _mm_slli_epi16(T[50][i], 1);
		T[50][i] = _mm_add_epi16(T[50][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[51][i], T[53][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[52][i] = _mm_slli_epi16(T[52][i], 1);
		T[52][i] = _mm_add_epi16(T[52][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[53][i], T[55][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[54][i] = _mm_slli_epi16(T[54][i], 1);
		T[54][i] = _mm_add_epi16(T[54][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[55][i], T[57][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[56][i] = _mm_slli_epi16(T[56][i], 1);
		T[56][i] = _mm_add_epi16(T[56][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[57][i], T[59][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[58][i] = _mm_slli_epi16(T[58][i], 1);
		T[58][i] = _mm_add_epi16(T[58][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[59][i], T[61][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[60][i] = _mm_slli_epi16(T[60][i], 1);
		T[60][i] = _mm_add_epi16(T[60][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[61][i], T[63][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[62][i] = _mm_slli_epi16(T[62][i], 1);
		T[62][i] = _mm_add_epi16(T[62][i], tt);

        temp_resi = resi + (i << 3);

		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 0), T[0][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 1), T[2][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 2), T[4][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 3), T[6][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 4), T[8][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 5), T[10][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 6), T[12][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 7), T[14][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 8), T[16][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 9), T[18][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 10), T[20][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 11), T[22][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 12), T[24][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 13), T[26][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 14), T[28][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 15), T[30][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 16), T[32][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 17), T[34][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 18), T[36][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 19), T[38][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 20), T[40][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 21), T[42][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 22), T[44][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 23), T[46][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 24), T[48][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 25), T[50][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 26), T[52][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 27), T[54][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 28), T[56][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 29), T[58][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 30), T[60][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 31), T[62][i]);
	}

	///// DCT /////

	// DCT1
	for (i = 0; i < 32 / 8; i++)
	{
		//load data
        temp_resi = resi + (i << 8);

		T00A = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 +  0]);    // [07 06 05 04 03 02 01 00]
		T00B = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 +  8]);    // [15 14 13 12 11 10 09 08]
		T00C = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 + 16]);    // [23 22 21 20 19 18 17 16]
		T00D = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 + 24]);    // [31 30 29 28 27 26 25 24]
		T01A = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 +  0]);
		T01B = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 +  8]);
		T01C = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 + 16]);
		T01D = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 + 24]);
		T02A = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 +  0]);
		T02B = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 +  8]);
		T02C = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 + 16]);
		T02D = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 + 24]);
		T03A = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 +  0]);
		T03B = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 +  8]);
		T03C = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 + 16]);
		T03D = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 + 24]);
		T04A = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 +  0]);
		T04B = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 +  8]);
		T04C = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 + 16]);
		T04D = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 + 24]);
		T05A = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 +  0]);
		T05B = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 +  8]);
		T05C = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 + 16]);
		T05D = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 + 24]);
		T06A = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 +  0]);
		T06B = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 +  8]);
		T06C = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 + 16]);
		T06D = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 + 24]);
		T07A = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 +  0]);
		T07B = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 +  8]);
		T07C = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 + 16]);
		T07D = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 + 24]);
		//
		T00A = _mm_shuffle_epi8(T00A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));    // [05 02 06 01 04 03 07 00]
		T00B = _mm_shuffle_epi8(T00B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));    // [10 13 09 14 11 12 08 15]
		T00C = _mm_shuffle_epi8(T00C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));    // [21 18 22 17 20 19 23 16]
		T00D = _mm_shuffle_epi8(T00D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));    // [26 29 25 30 27 28 24 31]
		T01A = _mm_shuffle_epi8(T01A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T01B = _mm_shuffle_epi8(T01B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T01C = _mm_shuffle_epi8(T01C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T01D = _mm_shuffle_epi8(T01D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T02A = _mm_shuffle_epi8(T02A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T02B = _mm_shuffle_epi8(T02B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T02C = _mm_shuffle_epi8(T02C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T02D = _mm_shuffle_epi8(T02D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T03A = _mm_shuffle_epi8(T03A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T03B = _mm_shuffle_epi8(T03B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T03C = _mm_shuffle_epi8(T03C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T03D = _mm_shuffle_epi8(T03D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T04A = _mm_shuffle_epi8(T04A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T04B = _mm_shuffle_epi8(T04B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T04C = _mm_shuffle_epi8(T04C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T04D = _mm_shuffle_epi8(T04D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T05A = _mm_shuffle_epi8(T05A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T05B = _mm_shuffle_epi8(T05B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T05C = _mm_shuffle_epi8(T05C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T05D = _mm_shuffle_epi8(T05D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T06A = _mm_shuffle_epi8(T06A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T06B = _mm_shuffle_epi8(T06B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T06C = _mm_shuffle_epi8(T06C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T06D = _mm_shuffle_epi8(T06D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T07A = _mm_shuffle_epi8(T07A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T07B = _mm_shuffle_epi8(T07B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T07C = _mm_shuffle_epi8(T07C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T07D = _mm_shuffle_epi8(T07D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));

		T10A = _mm_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm_add_epi16(T01A, T01D);
		T11B = _mm_add_epi16(T01B, T01C);
		T12A = _mm_add_epi16(T02A, T02D);
		T12B = _mm_add_epi16(T02B, T02C);
		T13A = _mm_add_epi16(T03A, T03D);
		T13B = _mm_add_epi16(T03B, T03C);
		T14A = _mm_add_epi16(T04A, T04D);
		T14B = _mm_add_epi16(T04B, T04C);
		T15A = _mm_add_epi16(T05A, T05D);
		T15B = _mm_add_epi16(T05B, T05C);
		T16A = _mm_add_epi16(T06A, T06D);
		T16B = _mm_add_epi16(T06B, T06C);
		T17A = _mm_add_epi16(T07A, T07D);
		T17B = _mm_add_epi16(T07B, T07C);

		T00A = _mm_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm_sub_epi16(T01A, T01D);
		T01B = _mm_sub_epi16(T01B, T01C);
		T02A = _mm_sub_epi16(T02A, T02D);
		T02B = _mm_sub_epi16(T02B, T02C);
		T03A = _mm_sub_epi16(T03A, T03D);
		T03B = _mm_sub_epi16(T03B, T03C);
		T04A = _mm_sub_epi16(T04A, T04D);
		T04B = _mm_sub_epi16(T04B, T04C);
		T05A = _mm_sub_epi16(T05A, T05D);
		T05B = _mm_sub_epi16(T05B, T05C);
		T06A = _mm_sub_epi16(T06A, T06D);
		T06B = _mm_sub_epi16(T06B, T06C);
		T07A = _mm_sub_epi16(T07A, T07D);
		T07B = _mm_sub_epi16(T07B, T07C);

		T20 = _mm_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm_add_epi16(T11A, T11B);
		T22 = _mm_add_epi16(T12A, T12B);
		T23 = _mm_add_epi16(T13A, T13B);
		T24 = _mm_add_epi16(T14A, T14B);
		T25 = _mm_add_epi16(T15A, T15B);
		T26 = _mm_add_epi16(T16A, T16B);
		T27 = _mm_add_epi16(T17A, T17B);

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_8[1]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[0][i] = T60;

		T50 = _mm_hsub_epi32(T40, T41);
		T51 = _mm_hsub_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[16][i] = T60;

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_16_1[8]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[8][i] = T60;

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_16_1[9]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_16_1[9]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[24][i] = T60;

#define MAKE_ODD(tab, dstPos) \
    T30  = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T32  = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T34  = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T36  = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
        \
    T40  = _mm_hadd_epi32(T30, T31); \
    T41  = _mm_hadd_epi32(T32, T33); \
    T42  = _mm_hadd_epi32(T34, T35); \
    T43  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);
		MAKE_ODD(2, 20);
		MAKE_ODD(3, 28);

		T20 = _mm_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm_sub_epi16(T11A, T11B);
		T22 = _mm_sub_epi16(T12A, T12B);
		T23 = _mm_sub_epi16(T13A, T13B);
		T24 = _mm_sub_epi16(T14A, T14B);
		T25 = _mm_sub_epi16(T15A, T15B);
		T26 = _mm_sub_epi16(T16A, T16B);
		T27 = _mm_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);
		MAKE_ODD(8, 18);
		MAKE_ODD(9, 22);
		MAKE_ODD(10, 26);
		MAKE_ODD(11, 30);
#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
    T20  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T21  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T22  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T23  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T24  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T25  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T26  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T27  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T30  = _mm_madd_epi16(T04A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T04B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T32  = _mm_madd_epi16(T05A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T05B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T34  = _mm_madd_epi16(T06A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T06B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T36  = _mm_madd_epi16(T07A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T07B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
        \
    T40  = _mm_hadd_epi32(T20, T21); \
    T41  = _mm_hadd_epi32(T22, T23); \
    T42  = _mm_hadd_epi32(T24, T25); \
    T43  = _mm_hadd_epi32(T26, T27); \
    T44  = _mm_hadd_epi32(T30, T31); \
    T45  = _mm_hadd_epi32(T32, T33); \
    T46  = _mm_hadd_epi32(T34, T35); \
    T47  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T52  = _mm_hadd_epi32(T44, T45); \
    T53  = _mm_hadd_epi32(T46, T47); \
        \
    T50  = _mm_hadd_epi32(T50, T51); \
    T51  = _mm_hadd_epi32(T52, T53); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);
		MAKE_ODD(28, 17);
		MAKE_ODD(30, 19);
		MAKE_ODD(32, 21);
		MAKE_ODD(34, 23);
		MAKE_ODD(36, 25);
		MAKE_ODD(38, 27);
		MAKE_ODD(40, 29);
		MAKE_ODD(42, 31);

#undef MAKE_ODD
	}

	// DCT2
	for (i = 0; i < 32 / 8; i++)
	{
        int idx = i << 3;

		T00A = im[idx + 0][0];    // [07 06 05 04 03 02 01 00]
		T00B = im[idx + 0][1];    // [15 14 13 12 11 10 09 08]
		T00C = im[idx + 0][2];    // [23 22 21 20 19 18 17 16]
		T00D = im[idx + 0][3];    // [31 30 29 28 27 26 25 24]
		T01A = im[idx + 1][0];
		T01B = im[idx + 1][1];
		T01C = im[idx + 1][2];
		T01D = im[idx + 1][3];
		T02A = im[idx + 2][0];
		T02B = im[idx + 2][1];
		T02C = im[idx + 2][2];
		T02D = im[idx + 2][3];
		T03A = im[idx + 3][0];
		T03B = im[idx + 3][1];
		T03C = im[idx + 3][2];
		T03D = im[idx + 3][3];

		TT00A = im[idx + 4][0];
		TT00B = im[idx + 4][1];
		TT00C = im[idx + 4][2];
		TT00D = im[idx + 4][3];
		TT01A = im[idx + 5][0];
		TT01B = im[idx + 5][1];
		TT01C = im[idx + 5][2];
		TT01D = im[idx + 5][3];
		TT02A = im[idx + 6][0];
		TT02B = im[idx + 6][1];
		TT02C = im[idx + 6][2];
		TT02D = im[idx + 6][3];
		TT03A = im[idx + 7][0];
		TT03B = im[idx + 7][1];
		TT03C = im[idx + 7][2];
		TT03D = im[idx + 7][3];

		T00C = _mm_shuffle_epi8(T00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [16 17 18 19 20 21 22 23]
		T00D = _mm_shuffle_epi8(T00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [24 25 26 27 28 29 30 31]
		T01C = _mm_shuffle_epi8(T01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T01D = _mm_shuffle_epi8(T01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02C = _mm_shuffle_epi8(T02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02D = _mm_shuffle_epi8(T02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03C = _mm_shuffle_epi8(T03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03D = _mm_shuffle_epi8(T03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		TT00C = _mm_shuffle_epi8(TT00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT00D = _mm_shuffle_epi8(TT00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01C = _mm_shuffle_epi8(TT01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01D = _mm_shuffle_epi8(TT01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02C = _mm_shuffle_epi8(TT02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02D = _mm_shuffle_epi8(TT02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03C = _mm_shuffle_epi8(TT03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03D = _mm_shuffle_epi8(TT03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		T10A = _mm_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
		T10B = _mm_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
		T00A = _mm_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
		T00B = _mm_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
		T11A = _mm_unpacklo_epi16(T01A, T01D);
		T11B = _mm_unpackhi_epi16(T01A, T01D);
		T01A = _mm_unpacklo_epi16(T01B, T01C);
		T01B = _mm_unpackhi_epi16(T01B, T01C);
		T12A = _mm_unpacklo_epi16(T02A, T02D);
		T12B = _mm_unpackhi_epi16(T02A, T02D);
		T02A = _mm_unpacklo_epi16(T02B, T02C);
		T02B = _mm_unpackhi_epi16(T02B, T02C);
		T13A = _mm_unpacklo_epi16(T03A, T03D);
		T13B = _mm_unpackhi_epi16(T03A, T03D);
		T03A = _mm_unpacklo_epi16(T03B, T03C);
		T03B = _mm_unpackhi_epi16(T03B, T03C);

		TT10A = _mm_unpacklo_epi16(TT00A, TT00D);
		TT10B = _mm_unpackhi_epi16(TT00A, TT00D);
		TT00A = _mm_unpacklo_epi16(TT00B, TT00C);
		TT00B = _mm_unpackhi_epi16(TT00B, TT00C);
		TT11A = _mm_unpacklo_epi16(TT01A, TT01D);
		TT11B = _mm_unpackhi_epi16(TT01A, TT01D);
		TT01A = _mm_unpacklo_epi16(TT01B, TT01C);
		TT01B = _mm_unpackhi_epi16(TT01B, TT01C);
		TT12A = _mm_unpacklo_epi16(TT02A, TT02D);
		TT12B = _mm_unpackhi_epi16(TT02A, TT02D);
		TT02A = _mm_unpacklo_epi16(TT02B, TT02C);
		TT02B = _mm_unpackhi_epi16(TT02B, TT02C);
		TT13A = _mm_unpacklo_epi16(TT03A, TT03D);
		TT13B = _mm_unpackhi_epi16(TT03A, TT03D);
		TT03A = _mm_unpacklo_epi16(TT03B, TT03C);
		TT03B = _mm_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
    T20  = _mm_madd_epi16(T10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T21  = _mm_madd_epi16(T10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T22  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T23  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T24  = _mm_madd_epi16(T11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T25  = _mm_madd_epi16(T11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T26  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T27  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T30  = _mm_madd_epi16(T12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T31  = _mm_madd_epi16(T12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T32  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T33  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T34  = _mm_madd_epi16(T13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T35  = _mm_madd_epi16(T13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T36  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T37  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    T60  = _mm_hadd_epi32(T20, T21); \
    T61  = _mm_hadd_epi32(T22, T23); \
    T62  = _mm_hadd_epi32(T24, T25); \
    T63  = _mm_hadd_epi32(T26, T27); \
    T64  = _mm_hadd_epi32(T30, T31); \
    T65  = _mm_hadd_epi32(T32, T33); \
    T66  = _mm_hadd_epi32(T34, T35); \
    T67  = _mm_hadd_epi32(T36, T37); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
    T62  = _mm_hadd_epi32(T64, T65); \
    T63  = _mm_hadd_epi32(T66, T67); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
        \
    T60  = _mm_srai_epi32(_mm_add_epi32(T60, c_512), shift2); \
		\
	TT20  = _mm_madd_epi16(TT10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT21  = _mm_madd_epi16(TT10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT22  = _mm_madd_epi16(TT00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT23  = _mm_madd_epi16(TT00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT24  = _mm_madd_epi16(TT11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT25  = _mm_madd_epi16(TT11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT26  = _mm_madd_epi16(TT01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT27  = _mm_madd_epi16(TT01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT30  = _mm_madd_epi16(TT12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT31  = _mm_madd_epi16(TT12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT32  = _mm_madd_epi16(TT02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT33  = _mm_madd_epi16(TT02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT34  = _mm_madd_epi16(TT13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT35  = _mm_madd_epi16(TT13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT36  = _mm_madd_epi16(TT03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT37  = _mm_madd_epi16(TT03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    TT60  = _mm_hadd_epi32(TT20, TT21); \
    TT61  = _mm_hadd_epi32(TT22, TT23); \
    TT62  = _mm_hadd_epi32(TT24, TT25); \
    TT63  = _mm_hadd_epi32(TT26, TT27); \
    TT64  = _mm_hadd_epi32(TT30, TT31); \
    TT65  = _mm_hadd_epi32(TT32, TT33); \
    TT66  = _mm_hadd_epi32(TT34, TT35); \
    TT67  = _mm_hadd_epi32(TT36, TT37); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
    TT62  = _mm_hadd_epi32(TT64, TT65); \
    TT63  = _mm_hadd_epi32(TT66, TT67); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
        \
    TT60  = _mm_srai_epi32(_mm_add_epi32(TT60, c_512), shift2); \
		\
	tResult = _mm_packs_epi32(T60, TT60);\
    _mm_storeu_si128((__m128i*)&dst[((dstPos) << 5) + idx + 0], tResult); \

		MAKE_ODD(44, 44, 44, 44, 0);
		MAKE_ODD(45, 45, 45, 45, 16);
		MAKE_ODD(46, 47, 46, 47, 8);
		MAKE_ODD(48, 49, 48, 49, 24);

		MAKE_ODD(50, 51, 52, 53, 4);
		MAKE_ODD(54, 55, 56, 57, 12);
		MAKE_ODD(58, 59, 60, 61, 20);
		MAKE_ODD(62, 63, 64, 65, 28);

		MAKE_ODD(66, 67, 68, 69, 2);
		MAKE_ODD(70, 71, 72, 73, 6);
		MAKE_ODD(74, 75, 76, 77, 10);
		MAKE_ODD(78, 79, 80, 81, 14);

		MAKE_ODD(82, 83, 84, 85, 18);
		MAKE_ODD(86, 87, 88, 89, 22);
		MAKE_ODD(90, 91, 92, 93, 26);
		MAKE_ODD(94, 95, 96, 97, 30);

		MAKE_ODD(98, 99, 100, 101, 1);
		MAKE_ODD(102, 103, 104, 105, 3);
		MAKE_ODD(106, 107, 108, 109, 5);
		MAKE_ODD(110, 111, 112, 113, 7);
		MAKE_ODD(114, 115, 116, 117, 9);
		MAKE_ODD(118, 119, 120, 121, 11);
		MAKE_ODD(122, 123, 124, 125, 13);
		MAKE_ODD(126, 127, 128, 129, 15);
		MAKE_ODD(130, 131, 132, 133, 17);
		MAKE_ODD(134, 135, 136, 137, 19);
		MAKE_ODD(138, 139, 140, 141, 21);
		MAKE_ODD(142, 143, 144, 145, 23);
		MAKE_ODD(146, 147, 148, 149, 25);
		MAKE_ODD(150, 151, 152, 153, 27);
		MAKE_ODD(154, 155, 156, 157, 29);
		MAKE_ODD(158, 159, 160, 161, 31);
#undef MAKE_ODD
	}
}

#define TRANSPOSE_8x8_16BIT_m256i(I0, I1, I2, I3, I4, I5, I6, I7, O0, O1, O2, O3, O4, O5, O6, O7) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_3 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_4 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_5 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_6 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_7 = _mm256_unpackhi_epi16(I6, I7); \
	tr1_0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	tr1_1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	tr1_2 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	tr1_3 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	tr1_4 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	tr1_5 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	tr1_6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	tr1_7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	O0 = _mm256_unpacklo_epi64(tr1_0, tr1_4); \
	O1 = _mm256_unpackhi_epi64(tr1_0, tr1_4); \
	O2 = _mm256_unpacklo_epi64(tr1_2, tr1_6); \
	O3 = _mm256_unpackhi_epi64(tr1_2, tr1_6); \
	O4 = _mm256_unpacklo_epi64(tr1_1, tr1_5); \
	O5 = _mm256_unpackhi_epi64(tr1_1, tr1_5); \
	O6 = _mm256_unpacklo_epi64(tr1_3, tr1_7); \
	O7 = _mm256_unpackhi_epi64(tr1_3, tr1_7); \

#define TRANSPOSE_16x16_16BIT_m256i(I0,	I1,	I2,	I3,	I4,	I5,	I6,	I7,	I8,	I9,	I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11,	O12, O13, O14, O15) \
	TRANSPOSE_8x8_16BIT_m256i(I0, I1, I2, I3, I4, I5, I6, I7, t0, t1, t2, t3, t4, t5, t6, t7); \
	TRANSPOSE_8x8_16BIT_m256i(I8, I9, I10, I11, I12, I13, I14, I15, t8, t9, t10, t11, t12, t13, t14, t15); \
	O0 = _mm256_permute2x128_si256(t0, t8, 0x20); \
	O1 = _mm256_permute2x128_si256(t1, t9, 0x20); \
	O2 = _mm256_permute2x128_si256(t2, t10, 0x20); \
	O3 = _mm256_permute2x128_si256(t3, t11, 0x20); \
	O4 = _mm256_permute2x128_si256(t4, t12, 0x20); \
	O5 = _mm256_permute2x128_si256(t5, t13, 0x20); \
	O6 = _mm256_permute2x128_si256(t6, t14, 0x20); \
	O7 = _mm256_permute2x128_si256(t7, t15, 0x20); \
	O8 = _mm256_permute2x128_si256(t0, t8, 0x31); \
	O9 = _mm256_permute2x128_si256(t1, t9, 0x31); \
	O10 = _mm256_permute2x128_si256(t2, t10, 0x31); \
	O11 = _mm256_permute2x128_si256(t3, t11, 0x31); \
	O12 = _mm256_permute2x128_si256(t4, t12, 0x31); \
	O13 = _mm256_permute2x128_si256(t5, t13, 0x31); \
	O14 = _mm256_permute2x128_si256(t6, t14, 0x31); \
	O15 = _mm256_permute2x128_si256(t7, t15, 0x31); \

void sub_trans_64x64_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	int i;
	resi_t resi[32 * 32];
	resi_t *temp_resi;
	__m256i T[64][4], V[64][4];
	__m256i O00, O01, O02, O03, O04, O05, O06, O07;
	__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m256i P00, P01, P02, P03, P04, P05, P06, P07;
	__m256i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m256i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
	__m256i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
	__m256i	t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
	__m256i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m256i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m256i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m256i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m256i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m256i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m256i T20, T21, T22, T23, T24, T25, T26, T27;
	__m256i T30, T31, T32, T33, T34, T35, T36, T37;
	__m256i T40, T41, T42, T43, T44, T45, T46, T47;
	__m256i T50, T51, T52, T53;
	__m256i T60, T61, T62, T63, T64, T65, T66, T67;
	__m256i im[32][4];
	__m256i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m256i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m256i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m256i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m256i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m256i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m256i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m256i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m256i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m256i TT50, TT51, TT52, TT53;
	__m256i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m256i tResult;

	__m256i TAB16_0_1, TAB32_0_0, TAB16_0_0;
	__m256i tmpZero = _mm256_setzero_si256();
	__m256i k1 = _mm256_set1_epi16(1);
	__m256i k2 = _mm256_set1_epi16(2);
	__m256i tt;
	__m256i c_8 = _mm256_set1_epi32(8);
	__m256i c_512 = _mm256_set1_epi32(512);
	int shift1 = 4;
	int shift2 = 10;

	//// pix sub & wavelet ////
	for (i = 0; i < 4; i++){
		//load data
		O00 = _mm256_loadu_si256((__m256i*)&org[0 * i_org]);	//8x32
		O01 = _mm256_loadu_si256((__m256i*)&org[1 * i_org]);
		O02 = _mm256_loadu_si256((__m256i*)&org[2 * i_org]);
		O03 = _mm256_loadu_si256((__m256i*)&org[3 * i_org]);
		O04 = _mm256_loadu_si256((__m256i*)&org[4 * i_org]);
		O05 = _mm256_loadu_si256((__m256i*)&org[5 * i_org]);
		O06 = _mm256_loadu_si256((__m256i*)&org[6 * i_org]);
		O07 = _mm256_loadu_si256((__m256i*)&org[7 * i_org]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[0 * i_pred]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[1 * i_pred]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[2 * i_pred]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[3 * i_pred]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[4 * i_pred]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[5 * i_pred]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[6 * i_pred]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[7 * i_pred]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216); 
		O02 = _mm256_permute4x64_epi64(O02, 216); 
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216); 
		O05 = _mm256_permute4x64_epi64(O05, 216); 
		O06 = _mm256_permute4x64_epi64(O06, 216); 
		O07 = _mm256_permute4x64_epi64(O07, 216); 
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);	//A B
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);	//C D
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);

		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 0][0] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 0][1] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 1][0] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 1][1] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 2][0] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 2][1] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 3][0] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 3][1] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 4][0] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 4][1] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 5][0] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 5][1] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 6][0] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 6][1] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 7][0] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 7][1] = _mm256_sub_epi16(O07B, P07B);

		O00 = _mm256_loadu_si256((__m256i*)&org[0 * i_org + 32]);
		O01 = _mm256_loadu_si256((__m256i*)&org[1 * i_org + 32]);
		O02 = _mm256_loadu_si256((__m256i*)&org[2 * i_org + 32]);
		O03 = _mm256_loadu_si256((__m256i*)&org[3 * i_org + 32]);
		O04 = _mm256_loadu_si256((__m256i*)&org[4 * i_org + 32]);
		O05 = _mm256_loadu_si256((__m256i*)&org[5 * i_org + 32]);
		O06 = _mm256_loadu_si256((__m256i*)&org[6 * i_org + 32]);
		O07 = _mm256_loadu_si256((__m256i*)&org[7 * i_org + 32]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[0 * i_pred + 32]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[1 * i_pred + 32]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[2 * i_pred + 32]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[3 * i_pred + 32]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[4 * i_pred + 32]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[5 * i_pred + 32]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[6 * i_pred + 32]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[7 * i_pred + 32]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216);
		O02 = _mm256_permute4x64_epi64(O02, 216);
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216);
		O05 = _mm256_permute4x64_epi64(O05, 216);
		O06 = _mm256_permute4x64_epi64(O06, 216);
		O07 = _mm256_permute4x64_epi64(O07, 216);
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);
		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 0][2] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 0][3] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 1][2] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 1][3] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 2][2] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 2][3] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 3][2] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 3][3] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 4][2] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 4][3] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 5][2] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 5][3] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 6][2] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 6][3] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 7][2] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 7][3] = _mm256_sub_epi16(O07B, P07B);

		O00 = _mm256_loadu_si256((__m256i*)&org[8 * i_org]);	//8x32
		O01 = _mm256_loadu_si256((__m256i*)&org[9 * i_org]);
		O02 = _mm256_loadu_si256((__m256i*)&org[10 * i_org]);
		O03 = _mm256_loadu_si256((__m256i*)&org[11 * i_org]);
		O04 = _mm256_loadu_si256((__m256i*)&org[12 * i_org]);
		O05 = _mm256_loadu_si256((__m256i*)&org[13 * i_org]);
		O06 = _mm256_loadu_si256((__m256i*)&org[14 * i_org]);
		O07 = _mm256_loadu_si256((__m256i*)&org[15 * i_org]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[8 * i_pred]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[9 * i_pred]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[10 * i_pred]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[11 * i_pred]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[12 * i_pred]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[13 * i_pred]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[14 * i_pred]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[15 * i_pred]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216);
		O02 = _mm256_permute4x64_epi64(O02, 216);
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216);
		O05 = _mm256_permute4x64_epi64(O05, 216);
		O06 = _mm256_permute4x64_epi64(O06, 216);
		O07 = _mm256_permute4x64_epi64(O07, 216);
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);	//A B
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);	//C D
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);

		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 8][0] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 8][1] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 9][0] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 9][1] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 10][0] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 10][1] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 11][0] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 11][1] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 12][0] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 12][1] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 13][0] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 13][1] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 14][0] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 14][1] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 15][0] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 15][1] = _mm256_sub_epi16(O07B, P07B);

		O00 = _mm256_loadu_si256((__m256i*)&org[8 * i_org + 32]);
		O01 = _mm256_loadu_si256((__m256i*)&org[9 * i_org + 32]);
		O02 = _mm256_loadu_si256((__m256i*)&org[10 * i_org + 32]);
		O03 = _mm256_loadu_si256((__m256i*)&org[11 * i_org + 32]);
		O04 = _mm256_loadu_si256((__m256i*)&org[12 * i_org + 32]);
		O05 = _mm256_loadu_si256((__m256i*)&org[13 * i_org + 32]);
		O06 = _mm256_loadu_si256((__m256i*)&org[14 * i_org + 32]);
		O07 = _mm256_loadu_si256((__m256i*)&org[15 * i_org + 32]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[8 * i_pred + 32]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[9 * i_pred + 32]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[10 * i_pred + 32]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[11 * i_pred + 32]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[12 * i_pred + 32]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[13 * i_pred + 32]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[14 * i_pred + 32]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[15 * i_pred + 32]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216);
		O02 = _mm256_permute4x64_epi64(O02, 216);
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216);
		O05 = _mm256_permute4x64_epi64(O05, 216);
		O06 = _mm256_permute4x64_epi64(O06, 216);
		O07 = _mm256_permute4x64_epi64(O07, 216);
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);
		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 8][2] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 8][3] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 9][2] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 9][3] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 10][2] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 10][3] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 11][2] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 11][3] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 12][2] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 12][3] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 13][2] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 13][3] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 14][2] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 14][3] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 15][2] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 15][3] = _mm256_sub_epi16(O07B, P07B);

		org += i_org << 4;
		pred += i_pred << 4;

		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][0], T[i * 16 + 1][0], T[i * 16 + 2][0], T[i * 16 + 3][0], T[i * 16 + 4][0], T[i * 16 + 5][0], T[i * 16 + 6][0], T[i * 16 + 7][0], T[i * 16 + 8][0], T[i * 16 + 9][0], T[i * 16 + 10][0], T[i * 16 + 11][0], T[i * 16 + 12][0], T[i * 16 + 13][0], T[i * 16 + 14][0], T[i * 16 + 15][0], V[0][i], V[1][i], V[2][i], V[3][i], V[4][i], V[5][i], V[6][i], V[7][i], V[8][i], V[9][i], V[10][i], V[11][i], V[12][i], V[13][i], V[14][i], V[15][i]);
		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][1], T[i * 16 + 1][1], T[i * 16 + 2][1], T[i * 16 + 3][1], T[i * 16 + 4][1], T[i * 16 + 5][1], T[i * 16 + 6][1], T[i * 16 + 7][1], T[i * 16 + 8][1], T[i * 16 + 9][1], T[i * 16 + 10][1], T[i * 16 + 11][1], T[i * 16 + 12][1], T[i * 16 + 13][1], T[i * 16 + 14][1], T[i * 16 + 15][1], V[16][i], V[17][i], V[18][i], V[19][i], V[20][i], V[21][i], V[22][i], V[23][i], V[24][i], V[25][i], V[26][i], V[27][i], V[28][i], V[29][i], V[30][i], V[31][i]);
		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][2], T[i * 16 + 1][2], T[i * 16 + 2][2], T[i * 16 + 3][2], T[i * 16 + 4][2], T[i * 16 + 5][2], T[i * 16 + 6][2], T[i * 16 + 7][2], T[i * 16 + 8][2], T[i * 16 + 9][2], T[i * 16 + 10][2], T[i * 16 + 11][2], T[i * 16 + 12][2], T[i * 16 + 13][2], T[i * 16 + 14][2], T[i * 16 + 15][2], V[32][i], V[33][i], V[34][i], V[35][i], V[36][i], V[37][i], V[38][i], V[39][i], V[40][i], V[41][i], V[42][i], V[43][i], V[44][i], V[45][i], V[46][i], V[47][i]);
		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][3], T[i * 16 + 1][3], T[i * 16 + 2][3], T[i * 16 + 3][3], T[i * 16 + 4][3], T[i * 16 + 5][3], T[i * 16 + 6][3], T[i * 16 + 7][3], T[i * 16 + 8][3], T[i * 16 + 9][3], T[i * 16 + 10][3], T[i * 16 + 11][3], T[i * 16 + 12][3], T[i * 16 + 13][3], T[i * 16 + 14][3], T[i * 16 + 15][3], V[48][i], V[49][i], V[50][i], V[51][i], V[52][i], V[53][i], V[54][i], V[55][i], V[56][i], V[57][i], V[58][i], V[59][i], V[60][i], V[61][i], V[62][i], V[63][i]);

		//filter H
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[0][i], V[2][i]), 1);
		V[1][i] = _mm256_sub_epi16(V[1][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[2][i], V[4][i]), 1);
		V[3][i] = _mm256_sub_epi16(V[3][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[4][i], V[6][i]), 1);
		V[5][i] = _mm256_sub_epi16(V[5][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[6][i], V[8][i]), 1);
		V[7][i] = _mm256_sub_epi16(V[7][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[8][i], V[10][i]), 1);
		V[9][i] = _mm256_sub_epi16(V[9][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[10][i], V[12][i]), 1);
		V[11][i] = _mm256_sub_epi16(V[11][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[12][i], V[14][i]), 1);
		V[13][i] = _mm256_sub_epi16(V[13][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[14][i], V[16][i]), 1);
		V[15][i] = _mm256_sub_epi16(V[15][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[16][i], V[18][i]), 1);
		V[17][i] = _mm256_sub_epi16(V[17][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[18][i], V[20][i]), 1);
		V[19][i] = _mm256_sub_epi16(V[19][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[20][i], V[22][i]), 1);
		V[21][i] = _mm256_sub_epi16(V[21][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[22][i], V[24][i]), 1);
		V[23][i] = _mm256_sub_epi16(V[23][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[24][i], V[26][i]), 1);
		V[25][i] = _mm256_sub_epi16(V[25][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[26][i], V[28][i]), 1);
		V[27][i] = _mm256_sub_epi16(V[27][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[28][i], V[30][i]), 1);
		V[29][i] = _mm256_sub_epi16(V[29][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[30][i], V[32][i]), 1);
		V[31][i] = _mm256_sub_epi16(V[31][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[32][i], V[34][i]), 1);
		V[33][i] = _mm256_sub_epi16(V[33][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[34][i], V[36][i]), 1);
		V[35][i] = _mm256_sub_epi16(V[35][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[36][i], V[38][i]), 1);
		V[37][i] = _mm256_sub_epi16(V[37][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[38][i], V[40][i]), 1);
		V[39][i] = _mm256_sub_epi16(V[39][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[40][i], V[42][i]), 1);
		V[41][i] = _mm256_sub_epi16(V[41][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[42][i], V[44][i]), 1);
		V[43][i] = _mm256_sub_epi16(V[43][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[44][i], V[46][i]), 1);
		V[45][i] = _mm256_sub_epi16(V[45][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[46][i], V[48][i]), 1);
		V[47][i] = _mm256_sub_epi16(V[47][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[48][i], V[50][i]), 1);
		V[49][i] = _mm256_sub_epi16(V[49][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[50][i], V[52][i]), 1);
		V[51][i] = _mm256_sub_epi16(V[51][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[52][i], V[54][i]), 1);
		V[53][i] = _mm256_sub_epi16(V[53][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[54][i], V[56][i]), 1);
		V[55][i] = _mm256_sub_epi16(V[55][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[56][i], V[58][i]), 1);
		V[57][i] = _mm256_sub_epi16(V[57][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[58][i], V[60][i]), 1);
		V[59][i] = _mm256_sub_epi16(V[59][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[60][i], V[62][i]), 1);
		V[61][i] = _mm256_sub_epi16(V[61][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[62][i], V[62][i]), 1);
		V[63][i] = _mm256_sub_epi16(V[63][i], tt);

		//filter L
		tt = _mm256_add_epi16(_mm256_add_epi16(V[1][i], V[1][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[0][i] = _mm256_add_epi16(V[0][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[1][i], V[3][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[2][i] = _mm256_add_epi16(V[2][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[3][i], V[5][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[4][i] = _mm256_add_epi16(V[4][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[5][i], V[7][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[6][i] = _mm256_add_epi16(V[6][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[7][i], V[9][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[8][i] = _mm256_add_epi16(V[8][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[9][i], V[11][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[10][i] = _mm256_add_epi16(V[10][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[11][i], V[13][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[12][i] = _mm256_add_epi16(V[12][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[13][i], V[15][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[14][i] = _mm256_add_epi16(V[14][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[15][i], V[17][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[16][i] = _mm256_add_epi16(V[16][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[17][i], V[19][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[18][i] = _mm256_add_epi16(V[18][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[19][i], V[21][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[20][i] = _mm256_add_epi16(V[20][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[21][i], V[23][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[22][i] = _mm256_add_epi16(V[22][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[23][i], V[25][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[24][i] = _mm256_add_epi16(V[24][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[25][i], V[27][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[26][i] = _mm256_add_epi16(V[26][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[27][i], V[29][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[28][i] = _mm256_add_epi16(V[28][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[29][i], V[31][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[30][i] = _mm256_add_epi16(V[30][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[31][i], V[33][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[32][i] = _mm256_add_epi16(V[32][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[33][i], V[35][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[34][i] = _mm256_add_epi16(V[34][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[35][i], V[37][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[36][i] = _mm256_add_epi16(V[36][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[37][i], V[39][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[38][i] = _mm256_add_epi16(V[38][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[39][i], V[41][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[40][i] = _mm256_add_epi16(V[40][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[41][i], V[43][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[42][i] = _mm256_add_epi16(V[42][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[43][i], V[45][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[44][i] = _mm256_add_epi16(V[44][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[45][i], V[47][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[46][i] = _mm256_add_epi16(V[46][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[47][i], V[49][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[48][i] = _mm256_add_epi16(V[48][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[49][i], V[51][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[50][i] = _mm256_add_epi16(V[50][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[51][i], V[53][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[52][i] = _mm256_add_epi16(V[52][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[53][i], V[55][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[54][i] = _mm256_add_epi16(V[54][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[55][i], V[57][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[56][i] = _mm256_add_epi16(V[56][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[57][i], V[59][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[58][i] = _mm256_add_epi16(V[58][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[59][i], V[61][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[60][i] = _mm256_add_epi16(V[60][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[61][i], V[63][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[62][i] = _mm256_add_epi16(V[62][i], tt);
		
		TRANSPOSE_16x16_16BIT_m256i(V[0][i], V[2][i], V[4][i], V[6][i], V[8][i], V[10][i], V[12][i], V[14][i], V[16][i], V[18][i], V[20][i], V[22][i], V[24][i], V[26][i], V[28][i], V[30][i], T[i * 16 + 0][0], T[i * 16 + 1][0], T[i * 16 + 2][0], T[i * 16 + 3][0], T[i * 16 + 4][0], T[i * 16 + 5][0], T[i * 16 + 6][0], T[i * 16 + 7][0], T[i * 16 + 8][0], T[i * 16 + 9][0], T[i * 16 + 10][0], T[i * 16 + 11][0], T[i * 16 + 12][0], T[i * 16 + 13][0], T[i * 16 + 14][0], T[i * 16 + 15][0]);
		TRANSPOSE_16x16_16BIT_m256i(V[32][i], V[34][i], V[36][i], V[38][i], V[40][i], V[42][i], V[44][i], V[46][i], V[48][i], V[50][i], V[52][i], V[54][i], V[56][i], V[58][i], V[60][i], V[62][i], T[i * 16 + 0][1], T[i * 16 + 1][1], T[i * 16 + 2][1], T[i * 16 + 3][1], T[i * 16 + 4][1], T[i * 16 + 5][1], T[i * 16 + 6][1], T[i * 16 + 7][1], T[i * 16 + 8][1], T[i * 16 + 9][1], T[i * 16 + 10][1], T[i * 16 + 11][1], T[i * 16 + 12][1], T[i * 16 + 13][1], T[i * 16 + 14][1], T[i * 16 + 15][1]);
	}

	for (i = 0; i < 2; i++){
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[0][i], T[2][i]), 1);
		T[1][i] = _mm256_sub_epi16(T[1][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[2][i], T[4][i]), 1);
		T[3][i] = _mm256_sub_epi16(T[3][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[4][i], T[6][i]), 1);
		T[5][i] = _mm256_sub_epi16(T[5][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[6][i], T[8][i]), 1);
		T[7][i] = _mm256_sub_epi16(T[7][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[8][i], T[10][i]), 1);
		T[9][i] = _mm256_sub_epi16(T[9][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[10][i], T[12][i]), 1);
		T[11][i] = _mm256_sub_epi16(T[11][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[12][i], T[14][i]), 1);
		T[13][i] = _mm256_sub_epi16(T[13][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[14][i], T[16][i]), 1);
		T[15][i] = _mm256_sub_epi16(T[15][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[16][i], T[18][i]), 1);
		T[17][i] = _mm256_sub_epi16(T[17][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[18][i], T[20][i]), 1);
		T[19][i] = _mm256_sub_epi16(T[19][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[20][i], T[22][i]), 1);
		T[21][i] = _mm256_sub_epi16(T[21][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[22][i], T[24][i]), 1);
		T[23][i] = _mm256_sub_epi16(T[23][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[24][i], T[26][i]), 1);
		T[25][i] = _mm256_sub_epi16(T[25][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[26][i], T[28][i]), 1);
		T[27][i] = _mm256_sub_epi16(T[27][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[28][i], T[30][i]), 1);
		T[29][i] = _mm256_sub_epi16(T[29][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[30][i], T[32][i]), 1);
		T[31][i] = _mm256_sub_epi16(T[31][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[32][i], T[34][i]), 1);
		T[33][i] = _mm256_sub_epi16(T[33][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[34][i], T[36][i]), 1);
		T[35][i] = _mm256_sub_epi16(T[35][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[36][i], T[38][i]), 1);
		T[37][i] = _mm256_sub_epi16(T[37][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[38][i], T[40][i]), 1);
		T[39][i] = _mm256_sub_epi16(T[39][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[40][i], T[42][i]), 1);
		T[41][i] = _mm256_sub_epi16(T[41][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[42][i], T[44][i]), 1);
		T[43][i] = _mm256_sub_epi16(T[43][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[44][i], T[46][i]), 1);
		T[45][i] = _mm256_sub_epi16(T[45][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[46][i], T[48][i]), 1);
		T[47][i] = _mm256_sub_epi16(T[47][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[48][i], T[50][i]), 1);
		T[49][i] = _mm256_sub_epi16(T[49][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[50][i], T[52][i]), 1);
		T[51][i] = _mm256_sub_epi16(T[51][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[52][i], T[54][i]), 1);
		T[53][i] = _mm256_sub_epi16(T[53][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[54][i], T[56][i]), 1);
		T[55][i] = _mm256_sub_epi16(T[55][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[56][i], T[58][i]), 1);
		T[57][i] = _mm256_sub_epi16(T[57][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[58][i], T[60][i]), 1);
		T[59][i] = _mm256_sub_epi16(T[59][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[60][i], T[62][i]), 1);
		T[61][i] = _mm256_sub_epi16(T[61][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[62][i], T[62][i]), 1);
		T[63][i] = _mm256_sub_epi16(T[63][i], tt);

		tt = _mm256_add_epi16(_mm256_add_epi16(T[1][i], T[1][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[0][i] = _mm256_slli_epi16(T[0][i], 1);
		T[0][i] = _mm256_add_epi16(T[0][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[1][i], T[3][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[2][i] = _mm256_slli_epi16(T[2][i], 1);
		T[2][i] = _mm256_add_epi16(T[2][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[3][i], T[5][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[4][i] = _mm256_slli_epi16(T[4][i], 1);
		T[4][i] = _mm256_add_epi16(T[4][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[5][i], T[7][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[6][i] = _mm256_slli_epi16(T[6][i], 1);
		T[6][i] = _mm256_add_epi16(T[6][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[7][i], T[9][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[8][i] = _mm256_slli_epi16(T[8][i], 1);
		T[8][i] = _mm256_add_epi16(T[8][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[9][i], T[11][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[10][i] = _mm256_slli_epi16(T[10][i], 1);
		T[10][i] = _mm256_add_epi16(T[10][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[11][i], T[13][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[12][i] = _mm256_slli_epi16(T[12][i], 1);
		T[12][i] = _mm256_add_epi16(T[12][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[13][i], T[15][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[14][i] = _mm256_slli_epi16(T[14][i], 1);
		T[14][i] = _mm256_add_epi16(T[14][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[15][i], T[17][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[16][i] = _mm256_slli_epi16(T[16][i], 1);
		T[16][i] = _mm256_add_epi16(T[16][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[17][i], T[19][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[18][i] = _mm256_slli_epi16(T[18][i], 1);
		T[18][i] = _mm256_add_epi16(T[18][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[19][i], T[21][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[20][i] = _mm256_slli_epi16(T[20][i], 1);
		T[20][i] = _mm256_add_epi16(T[20][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[21][i], T[23][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[22][i] = _mm256_slli_epi16(T[22][i], 1);
		T[22][i] = _mm256_add_epi16(T[22][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[23][i], T[25][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[24][i] = _mm256_slli_epi16(T[24][i], 1);
		T[24][i] = _mm256_add_epi16(T[24][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[25][i], T[27][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[26][i] = _mm256_slli_epi16(T[26][i], 1);
		T[26][i] = _mm256_add_epi16(T[26][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[27][i], T[29][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[28][i] = _mm256_slli_epi16(T[28][i], 1);
		T[28][i] = _mm256_add_epi16(T[28][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[29][i], T[31][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[30][i] = _mm256_slli_epi16(T[30][i], 1);
		T[30][i] = _mm256_add_epi16(T[30][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[31][i], T[33][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[32][i] = _mm256_slli_epi16(T[32][i], 1);
		T[32][i] = _mm256_add_epi16(T[32][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[33][i], T[35][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[34][i] = _mm256_slli_epi16(T[34][i], 1);
		T[34][i] = _mm256_add_epi16(T[34][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[35][i], T[37][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[36][i] = _mm256_slli_epi16(T[36][i], 1);
		T[36][i] = _mm256_add_epi16(T[36][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[37][i], T[39][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[38][i] = _mm256_slli_epi16(T[38][i], 1);
		T[38][i] = _mm256_add_epi16(T[38][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[39][i], T[41][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[40][i] = _mm256_slli_epi16(T[40][i], 1);
		T[40][i] = _mm256_add_epi16(T[40][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[41][i], T[43][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[42][i] = _mm256_slli_epi16(T[42][i], 1);
		T[42][i] = _mm256_add_epi16(T[42][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[43][i], T[45][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[44][i] = _mm256_slli_epi16(T[44][i], 1);
		T[44][i] = _mm256_add_epi16(T[44][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[45][i], T[47][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[46][i] = _mm256_slli_epi16(T[46][i], 1);
		T[46][i] = _mm256_add_epi16(T[46][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[47][i], T[49][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[48][i] = _mm256_slli_epi16(T[48][i], 1);
		T[48][i] = _mm256_add_epi16(T[48][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[49][i], T[51][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[50][i] = _mm256_slli_epi16(T[50][i], 1);
		T[50][i] = _mm256_add_epi16(T[50][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[51][i], T[53][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[52][i] = _mm256_slli_epi16(T[52][i], 1);
		T[52][i] = _mm256_add_epi16(T[52][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[53][i], T[55][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[54][i] = _mm256_slli_epi16(T[54][i], 1);
		T[54][i] = _mm256_add_epi16(T[54][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[55][i], T[57][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[56][i] = _mm256_slli_epi16(T[56][i], 1);
		T[56][i] = _mm256_add_epi16(T[56][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[57][i], T[59][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[58][i] = _mm256_slli_epi16(T[58][i], 1);
		T[58][i] = _mm256_add_epi16(T[58][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[59][i], T[61][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[60][i] = _mm256_slli_epi16(T[60][i], 1);
		T[60][i] = _mm256_add_epi16(T[60][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[61][i], T[63][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[62][i] = _mm256_slli_epi16(T[62][i], 1);
		T[62][i] = _mm256_add_epi16(T[62][i], tt);

		//temp_resi = resi + (i << 4);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 0), T[0][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 1), T[2][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 2), T[4][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 3), T[6][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 4), T[8][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 5), T[10][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 6), T[12][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 7), T[14][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 8), T[16][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 9), T[18][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 10), T[20][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 11), T[22][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 12), T[24][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 13), T[26][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 14), T[28][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 15), T[30][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 16), T[32][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 17), T[34][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 18), T[36][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 19), T[38][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 20), T[40][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 21), T[42][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 22), T[44][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 23), T[46][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 24), T[48][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 25), T[50][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 26), T[52][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 27), T[54][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 28), T[56][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 29), T[58][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 30), T[60][i]);
		//_mm256_storeu_si256((__m256i *)(temp_resi + 32 * 31), T[62][i]);
	}

	///// DCT /////

	// DCT1
	for (i = 0; i < 2; i++)
	{
		T00A = _mm256_permute2x128_si256(T[0 + 32 * i][0], T[16 + 32 * i][0], 0x20);
		T00B = _mm256_permute2x128_si256(T[0 + 32 * i][0], T[16 + 32 * i][0], 0x31);
		T01A = _mm256_permute2x128_si256(T[2 + 32 * i][0], T[18 + 32 * i][0], 0x20);
		T01B = _mm256_permute2x128_si256(T[2 + 32 * i][0], T[18 + 32 * i][0], 0x31);
		T02A = _mm256_permute2x128_si256(T[4 + 32 * i][0], T[20 + 32 * i][0], 0x20);
		T02B = _mm256_permute2x128_si256(T[4 + 32 * i][0], T[20 + 32 * i][0], 0x31);
		T03A = _mm256_permute2x128_si256(T[6 + 32 * i][0], T[22 + 32 * i][0], 0x20);
		T03B = _mm256_permute2x128_si256(T[6 + 32 * i][0], T[22 + 32 * i][0], 0x31);
		T04A = _mm256_permute2x128_si256(T[8 + 32 * i][0], T[24 + 32 * i][0], 0x20);
		T04B = _mm256_permute2x128_si256(T[8 + 32 * i][0], T[24 + 32 * i][0], 0x31);
		T05A = _mm256_permute2x128_si256(T[10 + 32 * i][0], T[26 + 32 * i][0], 0x20);
		T05B = _mm256_permute2x128_si256(T[10 + 32 * i][0], T[26 + 32 * i][0], 0x31);
		T06A = _mm256_permute2x128_si256(T[12 + 32 * i][0], T[28 + 32 * i][0], 0x20);
		T06B = _mm256_permute2x128_si256(T[12 + 32 * i][0], T[28 + 32 * i][0], 0x31);
		T07A = _mm256_permute2x128_si256(T[14 + 32 * i][0], T[30 + 32 * i][0], 0x20);
		T07B = _mm256_permute2x128_si256(T[14 + 32 * i][0], T[30 + 32 * i][0], 0x31);

		T00C = _mm256_permute2x128_si256(T[0 + 32 * i][1], T[16 + 32 * i][1], 0x20);
		T00D = _mm256_permute2x128_si256(T[0 + 32 * i][1], T[16 + 32 * i][1], 0x31);
		T01C = _mm256_permute2x128_si256(T[2 + 32 * i][1], T[18 + 32 * i][1], 0x20);
		T01D = _mm256_permute2x128_si256(T[2 + 32 * i][1], T[18 + 32 * i][1], 0x31);
		T02C = _mm256_permute2x128_si256(T[4 + 32 * i][1], T[20 + 32 * i][1], 0x20);
		T02D = _mm256_permute2x128_si256(T[4 + 32 * i][1], T[20 + 32 * i][1], 0x31);
		T03C = _mm256_permute2x128_si256(T[6 + 32 * i][1], T[22 + 32 * i][1], 0x20);
		T03D = _mm256_permute2x128_si256(T[6 + 32 * i][1], T[22 + 32 * i][1], 0x31);
		T04C = _mm256_permute2x128_si256(T[8 + 32 * i][1], T[24 + 32 * i][1], 0x20);
		T04D = _mm256_permute2x128_si256(T[8 + 32 * i][1], T[24 + 32 * i][1], 0x31);
		T05C = _mm256_permute2x128_si256(T[10 + 32 * i][1], T[26 + 32 * i][1], 0x20);
		T05D = _mm256_permute2x128_si256(T[10 + 32 * i][1], T[26 + 32 * i][1], 0x31);
		T06C = _mm256_permute2x128_si256(T[12 + 32 * i][1], T[28 + 32 * i][1], 0x20);
		T06D = _mm256_permute2x128_si256(T[12 + 32 * i][1], T[28 + 32 * i][1], 0x31);
		T07C = _mm256_permute2x128_si256(T[14 + 32 * i][1], T[30 + 32 * i][1], 0x20);
		T07D = _mm256_permute2x128_si256(T[14 + 32 * i][1], T[30 + 32 * i][1], 0x31);

		org += i_org << 4;
		pred += i_pred << 4;

		//
		TAB16_0_1 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[1]);
		TAB32_0_0 = _mm256_load_si256((__m256i*)tab_dct_32_0_256i[0]);
		T00A = _mm256_shuffle_epi8(T00A, TAB16_0_1);    // [05 02 06 01 04 03 07 00]
		T00B = _mm256_shuffle_epi8(T00B, TAB32_0_0);    // [10 13 09 14 11 12 08 15]
		T00C = _mm256_shuffle_epi8(T00C, TAB16_0_1);    // [21 18 22 17 20 19 23 16]
		T00D = _mm256_shuffle_epi8(T00D, TAB32_0_0);    // [26 29 25 30 27 28 24 31]
		T01A = _mm256_shuffle_epi8(T01A, TAB16_0_1);
		T01B = _mm256_shuffle_epi8(T01B, TAB32_0_0);
		T01C = _mm256_shuffle_epi8(T01C, TAB16_0_1);
		T01D = _mm256_shuffle_epi8(T01D, TAB32_0_0);
		T02A = _mm256_shuffle_epi8(T02A, TAB16_0_1);
		T02B = _mm256_shuffle_epi8(T02B, TAB32_0_0);
		T02C = _mm256_shuffle_epi8(T02C, TAB16_0_1);
		T02D = _mm256_shuffle_epi8(T02D, TAB32_0_0);
		T03A = _mm256_shuffle_epi8(T03A, TAB16_0_1);
		T03B = _mm256_shuffle_epi8(T03B, TAB32_0_0);
		T03C = _mm256_shuffle_epi8(T03C, TAB16_0_1);
		T03D = _mm256_shuffle_epi8(T03D, TAB32_0_0);
		T04A = _mm256_shuffle_epi8(T04A, TAB16_0_1);
		T04B = _mm256_shuffle_epi8(T04B, TAB32_0_0);
		T04C = _mm256_shuffle_epi8(T04C, TAB16_0_1);
		T04D = _mm256_shuffle_epi8(T04D, TAB32_0_0);
		T05A = _mm256_shuffle_epi8(T05A, TAB16_0_1);
		T05B = _mm256_shuffle_epi8(T05B, TAB32_0_0);
		T05C = _mm256_shuffle_epi8(T05C, TAB16_0_1);
		T05D = _mm256_shuffle_epi8(T05D, TAB32_0_0);
		T06A = _mm256_shuffle_epi8(T06A, TAB16_0_1);
		T06B = _mm256_shuffle_epi8(T06B, TAB32_0_0);
		T06C = _mm256_shuffle_epi8(T06C, TAB16_0_1);
		T06D = _mm256_shuffle_epi8(T06D, TAB32_0_0);
		T07A = _mm256_shuffle_epi8(T07A, TAB16_0_1);
		T07B = _mm256_shuffle_epi8(T07B, TAB32_0_0);
		T07C = _mm256_shuffle_epi8(T07C, TAB16_0_1);
		T07D = _mm256_shuffle_epi8(T07D, TAB32_0_0);

		T10A = _mm256_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm256_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm256_add_epi16(T01A, T01D);
		T11B = _mm256_add_epi16(T01B, T01C);
		T12A = _mm256_add_epi16(T02A, T02D);
		T12B = _mm256_add_epi16(T02B, T02C);
		T13A = _mm256_add_epi16(T03A, T03D);
		T13B = _mm256_add_epi16(T03B, T03C);
		T14A = _mm256_add_epi16(T04A, T04D);
		T14B = _mm256_add_epi16(T04B, T04C);
		T15A = _mm256_add_epi16(T05A, T05D);
		T15B = _mm256_add_epi16(T05B, T05C);
		T16A = _mm256_add_epi16(T06A, T06D);
		T16B = _mm256_add_epi16(T06B, T06C);
		T17A = _mm256_add_epi16(T07A, T07D);
		T17B = _mm256_add_epi16(T07B, T07C);

		T00A = _mm256_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm256_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm256_sub_epi16(T01A, T01D);
		T01B = _mm256_sub_epi16(T01B, T01C);
		T02A = _mm256_sub_epi16(T02A, T02D);
		T02B = _mm256_sub_epi16(T02B, T02C);
		T03A = _mm256_sub_epi16(T03A, T03D);
		T03B = _mm256_sub_epi16(T03B, T03C);
		T04A = _mm256_sub_epi16(T04A, T04D);
		T04B = _mm256_sub_epi16(T04B, T04C);
		T05A = _mm256_sub_epi16(T05A, T05D);
		T05B = _mm256_sub_epi16(T05B, T05C);
		T06A = _mm256_sub_epi16(T06A, T06D);
		T06B = _mm256_sub_epi16(T06B, T06C);
		T07A = _mm256_sub_epi16(T07A, T07D);
		T07B = _mm256_sub_epi16(T07B, T07C);

		T20 = _mm256_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm256_add_epi16(T11A, T11B);
		T22 = _mm256_add_epi16(T12A, T12B);
		T23 = _mm256_add_epi16(T13A, T13B);
		T24 = _mm256_add_epi16(T14A, T14B);
		T25 = _mm256_add_epi16(T15A, T15B);
		T26 = _mm256_add_epi16(T16A, T16B);
		T27 = _mm256_add_epi16(T17A, T17B);

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[0][i] = T60;

		T50 = _mm256_hsub_epi32(T40, T41);
		T51 = _mm256_hsub_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[16][i] = T60;

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[8][i] = T60;

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[9]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[24][i] = T60;

#define MAKE_ODD(tab, dstPos) \
	T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	\
	T40 = _mm256_hadd_epi32(T30, T31); \
	T41 = _mm256_hadd_epi32(T32, T33); \
	T42 = _mm256_hadd_epi32(T34, T35); \
	T43 = _mm256_hadd_epi32(T36, T37); \
	\
	T50 = _mm256_hadd_epi32(T40, T41); \
	T51 = _mm256_hadd_epi32(T42, T43); \
	T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1); \
	T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1); \
	T60 = _mm256_packs_epi32(T50, T51); \
	im[(dstPos)][i] = T60;

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);
		MAKE_ODD(2, 20);
		MAKE_ODD(3, 28);

		T20 = _mm256_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm256_sub_epi16(T11A, T11B);
		T22 = _mm256_sub_epi16(T12A, T12B);
		T23 = _mm256_sub_epi16(T13A, T13B);
		T24 = _mm256_sub_epi16(T14A, T14B);
		T25 = _mm256_sub_epi16(T15A, T15B);
		T26 = _mm256_sub_epi16(T16A, T16B);
		T27 = _mm256_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);
		MAKE_ODD(8, 18);
		MAKE_ODD(9, 22);
		MAKE_ODD(10, 26);
		MAKE_ODD(11, 30);
#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
	T20 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T21 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T22 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T23 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T24 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T25 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T26 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T27 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T30 = _mm256_madd_epi16(T04A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T31 = _mm256_madd_epi16(T04B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T32 = _mm256_madd_epi16(T05A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T33 = _mm256_madd_epi16(T05B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T34 = _mm256_madd_epi16(T06A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T35 = _mm256_madd_epi16(T06B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T36 = _mm256_madd_epi16(T07A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T37 = _mm256_madd_epi16(T07B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	\
	T40 = _mm256_hadd_epi32(T20, T21); \
	T41 = _mm256_hadd_epi32(T22, T23); \
	T42 = _mm256_hadd_epi32(T24, T25); \
	T43 = _mm256_hadd_epi32(T26, T27); \
	T44 = _mm256_hadd_epi32(T30, T31); \
	T45 = _mm256_hadd_epi32(T32, T33); \
	T46 = _mm256_hadd_epi32(T34, T35); \
	T47 = _mm256_hadd_epi32(T36, T37); \
	\
	T50 = _mm256_hadd_epi32(T40, T41); \
	T51 = _mm256_hadd_epi32(T42, T43); \
	T52 = _mm256_hadd_epi32(T44, T45); \
	T53 = _mm256_hadd_epi32(T46, T47); \
	\
	T50 = _mm256_hadd_epi32(T50, T51); \
	T51 = _mm256_hadd_epi32(T52, T53); \
	T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1); \
	T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1); \
	T60 = _mm256_packs_epi32(T50, T51); \
	im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);
		MAKE_ODD(28, 17);
		MAKE_ODD(30, 19);
		MAKE_ODD(32, 21);
		MAKE_ODD(34, 23);
		MAKE_ODD(36, 25);
		MAKE_ODD(38, 27);
		MAKE_ODD(40, 29);
		MAKE_ODD(42, 31);

#undef MAKE_ODD
	}

	// DCT2
	for (i = 0; i < 2; i++)
	{
		T00A = _mm256_permute2x128_si256(im[i * 16 + 0][0], im[i * 16 + 8][0], 0x20);
		T00B = _mm256_permute2x128_si256(im[i * 16 + 0][0], im[i * 16 + 8][0], 0x31);
		T01A = _mm256_permute2x128_si256(im[i * 16 + 1][0], im[i * 16 + 9][0], 0x20);
		T01B = _mm256_permute2x128_si256(im[i * 16 + 1][0], im[i * 16 + 9][0], 0x31);
		T02A = _mm256_permute2x128_si256(im[i * 16 + 2][0], im[i * 16 + 10][0], 0x20);
		T02B = _mm256_permute2x128_si256(im[i * 16 + 2][0], im[i * 16 + 10][0], 0x31);
		T03A = _mm256_permute2x128_si256(im[i * 16 + 3][0], im[i * 16 + 11][0], 0x20);
		T03B = _mm256_permute2x128_si256(im[i * 16 + 3][0], im[i * 16 + 11][0], 0x31);
		TT00A = _mm256_permute2x128_si256(im[i * 16 + 4][0], im[i * 16 + 12][0], 0x20);
		TT00B = _mm256_permute2x128_si256(im[i * 16 + 4][0], im[i * 16 + 12][0], 0x31);
		TT01A = _mm256_permute2x128_si256(im[i * 16 + 5][0], im[i * 16 + 13][0], 0x20);
		TT01B = _mm256_permute2x128_si256(im[i * 16 + 5][0], im[i * 16 + 13][0], 0x31);
		TT02A = _mm256_permute2x128_si256(im[i * 16 + 6][0], im[i * 16 + 14][0], 0x20);
		TT02B = _mm256_permute2x128_si256(im[i * 16 + 6][0], im[i * 16 + 14][0], 0x31);
		TT03A = _mm256_permute2x128_si256(im[i * 16 + 7][0], im[i * 16 + 15][0], 0x20);
		TT03B = _mm256_permute2x128_si256(im[i * 16 + 7][0], im[i * 16 + 15][0], 0x31);
		T00C = _mm256_permute2x128_si256(im[i * 16 + 0][1], im[i * 16 + 8][1], 0x20);
		T00D = _mm256_permute2x128_si256(im[i * 16 + 0][1], im[i * 16 + 8][1], 0x31);
		T01C = _mm256_permute2x128_si256(im[i * 16 + 1][1], im[i * 16 + 9][1], 0x20);
		T01D = _mm256_permute2x128_si256(im[i * 16 + 1][1], im[i * 16 + 9][1], 0x31);
		T02C = _mm256_permute2x128_si256(im[i * 16 + 2][1], im[i * 16 + 10][1], 0x20);
		T02D = _mm256_permute2x128_si256(im[i * 16 + 2][1], im[i * 16 + 10][1], 0x31);
		T03C = _mm256_permute2x128_si256(im[i * 16 + 3][1], im[i * 16 + 11][1], 0x20);
		T03D = _mm256_permute2x128_si256(im[i * 16 + 3][1], im[i * 16 + 11][1], 0x31);
		TT00C = _mm256_permute2x128_si256(im[i * 16 + 4][1], im[i * 16 + 12][1], 0x20);
		TT00D = _mm256_permute2x128_si256(im[i * 16 + 4][1], im[i * 16 + 12][1], 0x31);
		TT01C = _mm256_permute2x128_si256(im[i * 16 + 5][1], im[i * 16 + 13][1], 0x20);
		TT01D = _mm256_permute2x128_si256(im[i * 16 + 5][1], im[i * 16 + 13][1], 0x31);
		TT02C = _mm256_permute2x128_si256(im[i * 16 + 6][1], im[i * 16 + 14][1], 0x20);
		TT02D = _mm256_permute2x128_si256(im[i * 16 + 6][1], im[i * 16 + 14][1], 0x31);
		TT03C = _mm256_permute2x128_si256(im[i * 16 + 7][1], im[i * 16 + 15][1], 0x20);
		TT03D = _mm256_permute2x128_si256(im[i * 16 + 7][1], im[i * 16 + 15][1], 0x31);

		TAB16_0_0 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[0]);
		T00C = _mm256_shuffle_epi8(T00C, TAB16_0_0);    // [16 17 18 19 20 21 22 23]
		T00D = _mm256_shuffle_epi8(T00D, TAB16_0_0);    // [24 25 26 27 28 29 30 31]
		T01C = _mm256_shuffle_epi8(T01C, TAB16_0_0);
		T01D = _mm256_shuffle_epi8(T01D, TAB16_0_0);
		T02C = _mm256_shuffle_epi8(T02C, TAB16_0_0);
		T02D = _mm256_shuffle_epi8(T02D, TAB16_0_0);
		T03C = _mm256_shuffle_epi8(T03C, TAB16_0_0);
		T03D = _mm256_shuffle_epi8(T03D, TAB16_0_0);

		TT00C = _mm256_shuffle_epi8(TT00C, TAB16_0_0);
		TT00D = _mm256_shuffle_epi8(TT00D, TAB16_0_0);
		TT01C = _mm256_shuffle_epi8(TT01C, TAB16_0_0);
		TT01D = _mm256_shuffle_epi8(TT01D, TAB16_0_0);
		TT02C = _mm256_shuffle_epi8(TT02C, TAB16_0_0);
		TT02D = _mm256_shuffle_epi8(TT02D, TAB16_0_0);
		TT03C = _mm256_shuffle_epi8(TT03C, TAB16_0_0);
		TT03D = _mm256_shuffle_epi8(TT03D, TAB16_0_0);

		T10A = _mm256_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
		T10B = _mm256_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
		T00A = _mm256_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
		T00B = _mm256_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
		T11A = _mm256_unpacklo_epi16(T01A, T01D);
		T11B = _mm256_unpackhi_epi16(T01A, T01D);
		T01A = _mm256_unpacklo_epi16(T01B, T01C);
		T01B = _mm256_unpackhi_epi16(T01B, T01C);
		T12A = _mm256_unpacklo_epi16(T02A, T02D);
		T12B = _mm256_unpackhi_epi16(T02A, T02D);
		T02A = _mm256_unpacklo_epi16(T02B, T02C);
		T02B = _mm256_unpackhi_epi16(T02B, T02C);
		T13A = _mm256_unpacklo_epi16(T03A, T03D);
		T13B = _mm256_unpackhi_epi16(T03A, T03D);
		T03A = _mm256_unpacklo_epi16(T03B, T03C);
		T03B = _mm256_unpackhi_epi16(T03B, T03C);

		TT10A = _mm256_unpacklo_epi16(TT00A, TT00D);
		TT10B = _mm256_unpackhi_epi16(TT00A, TT00D);
		TT00A = _mm256_unpacklo_epi16(TT00B, TT00C);
		TT00B = _mm256_unpackhi_epi16(TT00B, TT00C);
		TT11A = _mm256_unpacklo_epi16(TT01A, TT01D);
		TT11B = _mm256_unpackhi_epi16(TT01A, TT01D);
		TT01A = _mm256_unpacklo_epi16(TT01B, TT01C);
		TT01B = _mm256_unpackhi_epi16(TT01B, TT01C);
		TT12A = _mm256_unpacklo_epi16(TT02A, TT02D);
		TT12B = _mm256_unpackhi_epi16(TT02A, TT02D);
		TT02A = _mm256_unpacklo_epi16(TT02B, TT02C);
		TT02B = _mm256_unpackhi_epi16(TT02B, TT02C);
		TT13A = _mm256_unpacklo_epi16(TT03A, TT03D);
		TT13B = _mm256_unpackhi_epi16(TT03A, TT03D);
		TT03A = _mm256_unpacklo_epi16(TT03B, TT03C);
		TT03B = _mm256_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
	T20 = _mm256_madd_epi16(T10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T21 = _mm256_madd_epi16(T10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T22 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T23 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T24 = _mm256_madd_epi16(T11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T25 = _mm256_madd_epi16(T11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T26 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T27 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T30 = _mm256_madd_epi16(T12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T31 = _mm256_madd_epi16(T12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T32 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T33 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T34 = _mm256_madd_epi16(T13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T35 = _mm256_madd_epi16(T13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T36 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T37 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	T60 = _mm256_hadd_epi32(T20, T21); \
	T61 = _mm256_hadd_epi32(T22, T23); \
	T62 = _mm256_hadd_epi32(T24, T25); \
	T63 = _mm256_hadd_epi32(T26, T27); \
	T64 = _mm256_hadd_epi32(T30, T31); \
	T65 = _mm256_hadd_epi32(T32, T33); \
	T66 = _mm256_hadd_epi32(T34, T35); \
	T67 = _mm256_hadd_epi32(T36, T37); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	T62 = _mm256_hadd_epi32(T64, T65); \
	T63 = _mm256_hadd_epi32(T66, T67); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	\
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_512), shift2); \
	\
	TT20 = _mm256_madd_epi16(TT10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT21 = _mm256_madd_epi16(TT10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT22 = _mm256_madd_epi16(TT00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT23 = _mm256_madd_epi16(TT00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT24 = _mm256_madd_epi16(TT11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT25 = _mm256_madd_epi16(TT11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT26 = _mm256_madd_epi16(TT01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT27 = _mm256_madd_epi16(TT01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT30 = _mm256_madd_epi16(TT12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT31 = _mm256_madd_epi16(TT12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT32 = _mm256_madd_epi16(TT02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT33 = _mm256_madd_epi16(TT02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT34 = _mm256_madd_epi16(TT13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT35 = _mm256_madd_epi16(TT13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT36 = _mm256_madd_epi16(TT03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT37 = _mm256_madd_epi16(TT03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	TT60 = _mm256_hadd_epi32(TT20, TT21); \
	TT61 = _mm256_hadd_epi32(TT22, TT23); \
	TT62 = _mm256_hadd_epi32(TT24, TT25); \
	TT63 = _mm256_hadd_epi32(TT26, TT27); \
	TT64 = _mm256_hadd_epi32(TT30, TT31); \
	TT65 = _mm256_hadd_epi32(TT32, TT33); \
	TT66 = _mm256_hadd_epi32(TT34, TT35); \
	TT67 = _mm256_hadd_epi32(TT36, TT37); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	TT62 = _mm256_hadd_epi32(TT64, TT65); \
	TT63 = _mm256_hadd_epi32(TT66, TT67); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	\
	TT60 = _mm256_srai_epi32(_mm256_add_epi32(TT60, c_512), shift2); \
	\
	tResult = _mm256_packs_epi32(T60, TT60); \
	_mm256_storeu_si256((__m256i*)&dst[(dstPos)* 32 + (i * 16) + 0], tResult); \

		MAKE_ODD(44, 44, 44, 44, 0);
		MAKE_ODD(45, 45, 45, 45, 16);
		MAKE_ODD(46, 47, 46, 47, 8);
		MAKE_ODD(48, 49, 48, 49, 24);

		MAKE_ODD(50, 51, 52, 53, 4);
		MAKE_ODD(54, 55, 56, 57, 12);
		MAKE_ODD(58, 59, 60, 61, 20);
		MAKE_ODD(62, 63, 64, 65, 28);

		MAKE_ODD(66, 67, 68, 69, 2);
		MAKE_ODD(70, 71, 72, 73, 6);
		MAKE_ODD(74, 75, 76, 77, 10);
		MAKE_ODD(78, 79, 80, 81, 14);

		MAKE_ODD(82, 83, 84, 85, 18);
		MAKE_ODD(86, 87, 88, 89, 22);
		MAKE_ODD(90, 91, 92, 93, 26);
		MAKE_ODD(94, 95, 96, 97, 30);

		MAKE_ODD(98, 99, 100, 101, 1);
		MAKE_ODD(102, 103, 104, 105, 3);
		MAKE_ODD(106, 107, 108, 109, 5);
		MAKE_ODD(110, 111, 112, 113, 7);
		MAKE_ODD(114, 115, 116, 117, 9);
		MAKE_ODD(118, 119, 120, 121, 11);
		MAKE_ODD(122, 123, 124, 125, 13);
		MAKE_ODD(126, 127, 128, 129, 15);
		MAKE_ODD(130, 131, 132, 133, 17);
		MAKE_ODD(134, 135, 136, 137, 19);
		MAKE_ODD(138, 139, 140, 141, 21);
		MAKE_ODD(142, 143, 144, 145, 23);
		MAKE_ODD(146, 147, 148, 149, 25);
		MAKE_ODD(150, 151, 152, 153, 27);
		MAKE_ODD(154, 155, 156, 157, 29);
		MAKE_ODD(158, 159, 160, 161, 31);
#undef MAKE_ODD
	}
}

void sub_trans_ext_32x32_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	// Const
	__m128i c_4 = _mm_set1_epi32(4);
	__m128i c_512 = _mm_set1_epi32(512);
	int shift1 = 3;
	int shift2 = 10;
	int i;

	__m128i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m128i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m128i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m128i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m128i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m128i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m128i T20, T21, T22, T23, T24, T25, T26, T27;
	__m128i T30, T31, T32, T33, T34, T35, T36, T37;
	__m128i T40, T41, T42, T43, T44, T45, T46, T47;
	__m128i T50, T51, T52, T53;
	__m128i T60, T61, T62, T63, T64, T65, T66, T67;
	__m128i im[16][4];

	__m128i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m128i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m128i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m128i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m128i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m128i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m128i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m128i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m128i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m128i TT50, TT51, TT52, TT53;
	__m128i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m128i tResult;

	__m128i O00, O01, O02, O03, O04, O05, O06, O07;
	__m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m128i P00, P01, P02, P03, P04, P05, P06, P07;
	__m128i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m128i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m128i tmpZero = _mm_setzero_si128();

	// DCT1
	for (i = 0; i < 32 / 8; i++)
	{
		//load data
		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org]);

		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);

		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred]);

		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);

		T00A = _mm_sub_epi16(O00A, P00A);
		T00B = _mm_sub_epi16(O00B, P00B);
		T01A = _mm_sub_epi16(O01A, P01A);
		T01B = _mm_sub_epi16(O01B, P01B);
		T02A = _mm_sub_epi16(O02A, P02A);
		T02B = _mm_sub_epi16(O02B, P02B);
		T03A = _mm_sub_epi16(O03A, P03A);
		T03B = _mm_sub_epi16(O03B, P03B);
		T04A = _mm_sub_epi16(O04A, P04A);
		T04B = _mm_sub_epi16(O04B, P04B);
		T05A = _mm_sub_epi16(O05A, P05A);
		T05B = _mm_sub_epi16(O05B, P05B);
		T06A = _mm_sub_epi16(O06A, P06A);
		T06B = _mm_sub_epi16(O06B, P06B);
		T07A = _mm_sub_epi16(O07A, P07A);
		T07B = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 16]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 16]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 16]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 16]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 16]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 16]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 16]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 16]);

		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);

		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 16]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 16]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 16]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 16]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 16]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 16]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 16]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 16]);

        org += i_org << 3;
        pred += i_pred << 3;

		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);

		T00C = _mm_sub_epi16(O00A, P00A);
		T00D = _mm_sub_epi16(O00B, P00B);
		T01C = _mm_sub_epi16(O01A, P01A);
		T01D = _mm_sub_epi16(O01B, P01B);
		T02C = _mm_sub_epi16(O02A, P02A);
		T02D = _mm_sub_epi16(O02B, P02B);
		T03C = _mm_sub_epi16(O03A, P03A);
		T03D = _mm_sub_epi16(O03B, P03B);
		T04C = _mm_sub_epi16(O04A, P04A);
		T04D = _mm_sub_epi16(O04B, P04B);
		T05C = _mm_sub_epi16(O05A, P05A);
		T05D = _mm_sub_epi16(O05B, P05B);
		T06C = _mm_sub_epi16(O06A, P06A);
		T06D = _mm_sub_epi16(O06B, P06B);
		T07C = _mm_sub_epi16(O07A, P07A);
		T07D = _mm_sub_epi16(O07B, P07B);
		//
		T00A = _mm_shuffle_epi8(T00A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));    // [05 02 06 01 04 03 07 00]
		T00B = _mm_shuffle_epi8(T00B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));    // [10 13 09 14 11 12 08 15]
		T00C = _mm_shuffle_epi8(T00C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));    // [21 18 22 17 20 19 23 16]
		T00D = _mm_shuffle_epi8(T00D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));    // [26 29 25 30 27 28 24 31]
		T01A = _mm_shuffle_epi8(T01A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T01B = _mm_shuffle_epi8(T01B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T01C = _mm_shuffle_epi8(T01C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T01D = _mm_shuffle_epi8(T01D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T02A = _mm_shuffle_epi8(T02A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T02B = _mm_shuffle_epi8(T02B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T02C = _mm_shuffle_epi8(T02C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T02D = _mm_shuffle_epi8(T02D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T03A = _mm_shuffle_epi8(T03A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T03B = _mm_shuffle_epi8(T03B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T03C = _mm_shuffle_epi8(T03C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T03D = _mm_shuffle_epi8(T03D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T04A = _mm_shuffle_epi8(T04A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T04B = _mm_shuffle_epi8(T04B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T04C = _mm_shuffle_epi8(T04C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T04D = _mm_shuffle_epi8(T04D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T05A = _mm_shuffle_epi8(T05A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T05B = _mm_shuffle_epi8(T05B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T05C = _mm_shuffle_epi8(T05C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T05D = _mm_shuffle_epi8(T05D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T06A = _mm_shuffle_epi8(T06A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T06B = _mm_shuffle_epi8(T06B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T06C = _mm_shuffle_epi8(T06C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T06D = _mm_shuffle_epi8(T06D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T07A = _mm_shuffle_epi8(T07A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T07B = _mm_shuffle_epi8(T07B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T07C = _mm_shuffle_epi8(T07C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T07D = _mm_shuffle_epi8(T07D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));

		T10A = _mm_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm_add_epi16(T01A, T01D);
		T11B = _mm_add_epi16(T01B, T01C);
		T12A = _mm_add_epi16(T02A, T02D);
		T12B = _mm_add_epi16(T02B, T02C);
		T13A = _mm_add_epi16(T03A, T03D);
		T13B = _mm_add_epi16(T03B, T03C);
		T14A = _mm_add_epi16(T04A, T04D);
		T14B = _mm_add_epi16(T04B, T04C);
		T15A = _mm_add_epi16(T05A, T05D);
		T15B = _mm_add_epi16(T05B, T05C);
		T16A = _mm_add_epi16(T06A, T06D);
		T16B = _mm_add_epi16(T06B, T06C);
		T17A = _mm_add_epi16(T07A, T07D);
		T17B = _mm_add_epi16(T07B, T07C);

		T00A = _mm_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm_sub_epi16(T01A, T01D);
		T01B = _mm_sub_epi16(T01B, T01C);
		T02A = _mm_sub_epi16(T02A, T02D);
		T02B = _mm_sub_epi16(T02B, T02C);
		T03A = _mm_sub_epi16(T03A, T03D);
		T03B = _mm_sub_epi16(T03B, T03C);
		T04A = _mm_sub_epi16(T04A, T04D);
		T04B = _mm_sub_epi16(T04B, T04C);
		T05A = _mm_sub_epi16(T05A, T05D);
		T05B = _mm_sub_epi16(T05B, T05C);
		T06A = _mm_sub_epi16(T06A, T06D);
		T06B = _mm_sub_epi16(T06B, T06C);
		T07A = _mm_sub_epi16(T07A, T07D);
		T07B = _mm_sub_epi16(T07B, T07C);

		T20 = _mm_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm_add_epi16(T11A, T11B);
		T22 = _mm_add_epi16(T12A, T12B);
		T23 = _mm_add_epi16(T13A, T13B);
		T24 = _mm_add_epi16(T14A, T14B);
		T25 = _mm_add_epi16(T15A, T15B);
		T26 = _mm_add_epi16(T16A, T16B);
		T27 = _mm_add_epi16(T17A, T17B);

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_8[1]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[0][i] = T60;

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_16_1[8]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[8][i] = T60;

#define MAKE_ODD(tab, dstPos) \
    T30  = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T32  = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T34  = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T36  = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
        \
    T40  = _mm_hadd_epi32(T30, T31); \
    T41  = _mm_hadd_epi32(T32, T33); \
    T42  = _mm_hadd_epi32(T34, T35); \
    T43  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);

		T20 = _mm_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm_sub_epi16(T11A, T11B);
		T22 = _mm_sub_epi16(T12A, T12B);
		T23 = _mm_sub_epi16(T13A, T13B);
		T24 = _mm_sub_epi16(T14A, T14B);
		T25 = _mm_sub_epi16(T15A, T15B);
		T26 = _mm_sub_epi16(T16A, T16B);
		T27 = _mm_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);
#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
    T20  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T21  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T22  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T23  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T24  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T25  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T26  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T27  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T30  = _mm_madd_epi16(T04A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T04B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T32  = _mm_madd_epi16(T05A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T05B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T34  = _mm_madd_epi16(T06A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T06B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T36  = _mm_madd_epi16(T07A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T07B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
        \
    T40  = _mm_hadd_epi32(T20, T21); \
    T41  = _mm_hadd_epi32(T22, T23); \
    T42  = _mm_hadd_epi32(T24, T25); \
    T43  = _mm_hadd_epi32(T26, T27); \
    T44  = _mm_hadd_epi32(T30, T31); \
    T45  = _mm_hadd_epi32(T32, T33); \
    T46  = _mm_hadd_epi32(T34, T35); \
    T47  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T52  = _mm_hadd_epi32(T44, T45); \
    T53  = _mm_hadd_epi32(T46, T47); \
        \
    T50  = _mm_hadd_epi32(T50, T51); \
    T51  = _mm_hadd_epi32(T52, T53); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_4), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_4), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);

#undef MAKE_ODD
	}

	// DCT2
	for (i = 0; i < 2; i++)
	{
		T00A = im[i * 8 + 0][0];    // [07 06 05 04 03 02 01 00]
		T00B = im[i * 8 + 0][1];    // [15 14 13 12 11 10 09 08]
		T00C = im[i * 8 + 0][2];    // [23 22 21 20 19 18 17 16]
		T00D = im[i * 8 + 0][3];    // [31 30 29 28 27 26 25 24]
		T01A = im[i * 8 + 1][0];
		T01B = im[i * 8 + 1][1];
		T01C = im[i * 8 + 1][2];
		T01D = im[i * 8 + 1][3];
		T02A = im[i * 8 + 2][0];
		T02B = im[i * 8 + 2][1];
		T02C = im[i * 8 + 2][2];
		T02D = im[i * 8 + 2][3];
		T03A = im[i * 8 + 3][0];
		T03B = im[i * 8 + 3][1];
		T03C = im[i * 8 + 3][2];
		T03D = im[i * 8 + 3][3];

		TT00A = im[i * 8 + 4][0];
		TT00B = im[i * 8 + 4][1];
		TT00C = im[i * 8 + 4][2];
		TT00D = im[i * 8 + 4][3];
		TT01A = im[i * 8 + 5][0];
		TT01B = im[i * 8 + 5][1];
		TT01C = im[i * 8 + 5][2];
		TT01D = im[i * 8 + 5][3];
		TT02A = im[i * 8 + 6][0];
		TT02B = im[i * 8 + 6][1];
		TT02C = im[i * 8 + 6][2];
		TT02D = im[i * 8 + 6][3];
		TT03A = im[i * 8 + 7][0];
		TT03B = im[i * 8 + 7][1];
		TT03C = im[i * 8 + 7][2];
		TT03D = im[i * 8 + 7][3];

		T00C = _mm_shuffle_epi8(T00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [16 17 18 19 20 21 22 23]
		T00D = _mm_shuffle_epi8(T00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [24 25 26 27 28 29 30 31]
		T01C = _mm_shuffle_epi8(T01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T01D = _mm_shuffle_epi8(T01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02C = _mm_shuffle_epi8(T02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02D = _mm_shuffle_epi8(T02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03C = _mm_shuffle_epi8(T03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03D = _mm_shuffle_epi8(T03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		TT00C = _mm_shuffle_epi8(TT00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT00D = _mm_shuffle_epi8(TT00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01C = _mm_shuffle_epi8(TT01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01D = _mm_shuffle_epi8(TT01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02C = _mm_shuffle_epi8(TT02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02D = _mm_shuffle_epi8(TT02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03C = _mm_shuffle_epi8(TT03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03D = _mm_shuffle_epi8(TT03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		T10A = _mm_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
		T10B = _mm_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
		T00A = _mm_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
		T00B = _mm_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
		T11A = _mm_unpacklo_epi16(T01A, T01D);
		T11B = _mm_unpackhi_epi16(T01A, T01D);
		T01A = _mm_unpacklo_epi16(T01B, T01C);
		T01B = _mm_unpackhi_epi16(T01B, T01C);
		T12A = _mm_unpacklo_epi16(T02A, T02D);
		T12B = _mm_unpackhi_epi16(T02A, T02D);
		T02A = _mm_unpacklo_epi16(T02B, T02C);
		T02B = _mm_unpackhi_epi16(T02B, T02C);
		T13A = _mm_unpacklo_epi16(T03A, T03D);
		T13B = _mm_unpackhi_epi16(T03A, T03D);
		T03A = _mm_unpacklo_epi16(T03B, T03C);
		T03B = _mm_unpackhi_epi16(T03B, T03C);

		TT10A = _mm_unpacklo_epi16(TT00A, TT00D);
		TT10B = _mm_unpackhi_epi16(TT00A, TT00D);
		TT00A = _mm_unpacklo_epi16(TT00B, TT00C);
		TT00B = _mm_unpackhi_epi16(TT00B, TT00C);
		TT11A = _mm_unpacklo_epi16(TT01A, TT01D);
		TT11B = _mm_unpackhi_epi16(TT01A, TT01D);
		TT01A = _mm_unpacklo_epi16(TT01B, TT01C);
		TT01B = _mm_unpackhi_epi16(TT01B, TT01C);
		TT12A = _mm_unpacklo_epi16(TT02A, TT02D);
		TT12B = _mm_unpackhi_epi16(TT02A, TT02D);
		TT02A = _mm_unpacklo_epi16(TT02B, TT02C);
		TT02B = _mm_unpackhi_epi16(TT02B, TT02C);
		TT13A = _mm_unpacklo_epi16(TT03A, TT03D);
		TT13B = _mm_unpackhi_epi16(TT03A, TT03D);
		TT03A = _mm_unpacklo_epi16(TT03B, TT03C);
		TT03B = _mm_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
    T20  = _mm_madd_epi16(T10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T21  = _mm_madd_epi16(T10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T22  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T23  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T24  = _mm_madd_epi16(T11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T25  = _mm_madd_epi16(T11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T26  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T27  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T30  = _mm_madd_epi16(T12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T31  = _mm_madd_epi16(T12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T32  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T33  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T34  = _mm_madd_epi16(T13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T35  = _mm_madd_epi16(T13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T36  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T37  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    T60  = _mm_hadd_epi32(T20, T21); \
    T61  = _mm_hadd_epi32(T22, T23); \
    T62  = _mm_hadd_epi32(T24, T25); \
    T63  = _mm_hadd_epi32(T26, T27); \
    T64  = _mm_hadd_epi32(T30, T31); \
    T65  = _mm_hadd_epi32(T32, T33); \
    T66  = _mm_hadd_epi32(T34, T35); \
    T67  = _mm_hadd_epi32(T36, T37); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
    T62  = _mm_hadd_epi32(T64, T65); \
    T63  = _mm_hadd_epi32(T66, T67); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
        \
    T60  = _mm_srai_epi32(_mm_add_epi32(T60, c_512), shift2); \
		\
	TT20  = _mm_madd_epi16(TT10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT21  = _mm_madd_epi16(TT10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT22  = _mm_madd_epi16(TT00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT23  = _mm_madd_epi16(TT00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT24  = _mm_madd_epi16(TT11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT25  = _mm_madd_epi16(TT11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT26  = _mm_madd_epi16(TT01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT27  = _mm_madd_epi16(TT01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT30  = _mm_madd_epi16(TT12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT31  = _mm_madd_epi16(TT12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT32  = _mm_madd_epi16(TT02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT33  = _mm_madd_epi16(TT02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT34  = _mm_madd_epi16(TT13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT35  = _mm_madd_epi16(TT13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT36  = _mm_madd_epi16(TT03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT37  = _mm_madd_epi16(TT03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    TT60  = _mm_hadd_epi32(TT20, TT21); \
    TT61  = _mm_hadd_epi32(TT22, TT23); \
    TT62  = _mm_hadd_epi32(TT24, TT25); \
    TT63  = _mm_hadd_epi32(TT26, TT27); \
    TT64  = _mm_hadd_epi32(TT30, TT31); \
    TT65  = _mm_hadd_epi32(TT32, TT33); \
    TT66  = _mm_hadd_epi32(TT34, TT35); \
    TT67  = _mm_hadd_epi32(TT36, TT37); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
    TT62  = _mm_hadd_epi32(TT64, TT65); \
    TT63  = _mm_hadd_epi32(TT66, TT67); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
        \
    TT60  = _mm_srai_epi32(_mm_add_epi32(TT60, c_512), shift2); \
		\
	tResult = _mm_packs_epi32(T60, TT60);\
    _mm_storeu_si128((__m128i*)&dst[(dstPos) * 32 + (i * 8) + 0], tResult); \

		MAKE_ODD(44, 44, 44, 44, 0);
		MAKE_ODD(46, 47, 46, 47, 8);

		MAKE_ODD(50, 51, 52, 53, 4);
		MAKE_ODD(54, 55, 56, 57, 12);

		MAKE_ODD(66, 67, 68, 69, 2);
		MAKE_ODD(70, 71, 72, 73, 6);
		MAKE_ODD(74, 75, 76, 77, 10);
		MAKE_ODD(78, 79, 80, 81, 14);


		MAKE_ODD(98, 99, 100, 101, 1);
		MAKE_ODD(102, 103, 104, 105, 3);
		MAKE_ODD(106, 107, 108, 109, 5);
		MAKE_ODD(110, 111, 112, 113, 7);
		MAKE_ODD(114, 115, 116, 117, 9);
		MAKE_ODD(118, 119, 120, 121, 11);
		MAKE_ODD(122, 123, 124, 125, 13);
		MAKE_ODD(126, 127, 128, 129, 15);

#undef MAKE_ODD
	}

    for (i = 0; i < 16; i++) {
        _mm_storeu_si128((__m128i*)(dst + 16), tmpZero);
        _mm_storeu_si128((__m128i*)(dst + 24), tmpZero);
        dst += 32;
    }
    for (i = 16; i < 32; i++) {
        _mm_storeu_si128((__m128i*)(dst +  0), tmpZero);
        _mm_storeu_si128((__m128i*)(dst +  8), tmpZero);
        _mm_storeu_si128((__m128i*)(dst + 16), tmpZero);
        _mm_storeu_si128((__m128i*)(dst + 24), tmpZero);
        dst += 32;
    }
}

void sub_trans_ext_64x64_sse128(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	int i;
	resi_t resi[32 * 32];
    resi_t *temp_resi;
	__m128i T[64][8], V[64][8];
	__m128i O00, O01, O02, O03, O04, O05, O06, O07;
	__m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m128i P00, P01, P02, P03, P04, P05, P06, P07;
	__m128i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m128i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
	__m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
	__m128i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m128i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m128i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m128i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m128i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m128i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m128i T20, T21, T22, T23, T24, T25, T26, T27;
	__m128i T30, T31, T32, T33, T34, T35, T36, T37;
	__m128i T40, T41, T42, T43, T44, T45, T46, T47;
	__m128i T50, T51, T52, T53;
	__m128i T60, T61, T62, T63, T64, T65, T66, T67;
	__m128i im[16][4];
	__m128i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m128i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m128i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m128i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m128i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m128i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m128i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m128i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m128i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m128i TT50, TT51, TT52, TT53;
	__m128i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m128i tResult;

	__m128i tmpZero = _mm_setzero_si128();
	__m128i k1 = _mm_set1_epi16(1);
	__m128i k2 = _mm_set1_epi16(2);
	__m128i tt;
	__m128i c_8 = _mm_set1_epi32(8);
	__m128i c_512 = _mm_set1_epi32(512);
	int shift1 = 4;
	int shift2 = 10;

	//// pix sub & wavelet ////
	for (i = 0; i < 8; i++){
		//load data
		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org]);
		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred]);
		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][0] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][1] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][0] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][1] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][0] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][1] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][0] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][1] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][0] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][1] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][0] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][1] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][0] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][1] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][0] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][1] = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 16]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 16]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 16]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 16]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 16]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 16]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 16]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 16]);
		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 16]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 16]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 16]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 16]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 16]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 16]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 16]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 16]);
		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][2] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][3] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][2] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][3] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][2] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][3] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][2] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][3] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][2] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][3] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][2] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][3] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][2] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][3] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][2] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][3] = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 32]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 32]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 32]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 32]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 32]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 32]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 32]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 32]);
		O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 32]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 32]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 32]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 32]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 32]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 32]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 32]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 32]);
		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][4] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][5] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][4] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][5] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][4] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][5] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][4] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][5] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][4] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][5] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][4] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][5] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][4] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][5] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][4] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][5] = _mm_sub_epi16(O07B, P07B);

		O00 = _mm_loadu_si128((__m128i*)&org[0 * i_org + 48]);
		O01 = _mm_loadu_si128((__m128i*)&org[1 * i_org + 48]);
		O02 = _mm_loadu_si128((__m128i*)&org[2 * i_org + 48]);
		O03 = _mm_loadu_si128((__m128i*)&org[3 * i_org + 48]);
		O04 = _mm_loadu_si128((__m128i*)&org[4 * i_org + 48]);
		O05 = _mm_loadu_si128((__m128i*)&org[5 * i_org + 48]);
		O06 = _mm_loadu_si128((__m128i*)&org[6 * i_org + 48]);
		O07 = _mm_loadu_si128((__m128i*)&org[7 * i_org + 48]);
        O00A = _mm_unpacklo_epi8(O00, tmpZero);
		O00B = _mm_unpackhi_epi8(O00, tmpZero);
		O01A = _mm_unpacklo_epi8(O01, tmpZero);
		O01B = _mm_unpackhi_epi8(O01, tmpZero);
		O02A = _mm_unpacklo_epi8(O02, tmpZero);
		O02B = _mm_unpackhi_epi8(O02, tmpZero);
		O03A = _mm_unpacklo_epi8(O03, tmpZero);
		O03B = _mm_unpackhi_epi8(O03, tmpZero);
		O04A = _mm_unpacklo_epi8(O04, tmpZero);
		O04B = _mm_unpackhi_epi8(O04, tmpZero);
		O05A = _mm_unpacklo_epi8(O05, tmpZero);
		O05B = _mm_unpackhi_epi8(O05, tmpZero);
		O06A = _mm_unpacklo_epi8(O06, tmpZero);
		O06B = _mm_unpackhi_epi8(O06, tmpZero);
		O07A = _mm_unpacklo_epi8(O07, tmpZero);
		O07B = _mm_unpackhi_epi8(O07, tmpZero);
		P00 = _mm_loadu_si128((__m128i*)&pred[0 * i_pred + 48]);
		P01 = _mm_loadu_si128((__m128i*)&pred[1 * i_pred + 48]);
		P02 = _mm_loadu_si128((__m128i*)&pred[2 * i_pred + 48]);
		P03 = _mm_loadu_si128((__m128i*)&pred[3 * i_pred + 48]);
		P04 = _mm_loadu_si128((__m128i*)&pred[4 * i_pred + 48]);
		P05 = _mm_loadu_si128((__m128i*)&pred[5 * i_pred + 48]);
		P06 = _mm_loadu_si128((__m128i*)&pred[6 * i_pred + 48]);
		P07 = _mm_loadu_si128((__m128i*)&pred[7 * i_pred + 48]);

        org  += i_org << 3;
        pred += i_pred << 3;

		P00A = _mm_unpacklo_epi8(P00, tmpZero);
		P00B = _mm_unpackhi_epi8(P00, tmpZero);
		P01A = _mm_unpacklo_epi8(P01, tmpZero);
		P01B = _mm_unpackhi_epi8(P01, tmpZero);
		P02A = _mm_unpacklo_epi8(P02, tmpZero);
		P02B = _mm_unpackhi_epi8(P02, tmpZero);
		P03A = _mm_unpacklo_epi8(P03, tmpZero);
		P03B = _mm_unpackhi_epi8(P03, tmpZero);
		P04A = _mm_unpacklo_epi8(P04, tmpZero);
		P04B = _mm_unpackhi_epi8(P04, tmpZero);
		P05A = _mm_unpacklo_epi8(P05, tmpZero);
		P05B = _mm_unpackhi_epi8(P05, tmpZero);
		P06A = _mm_unpacklo_epi8(P06, tmpZero);
		P06B = _mm_unpackhi_epi8(P06, tmpZero);
		P07A = _mm_unpacklo_epi8(P07, tmpZero);
		P07B = _mm_unpackhi_epi8(P07, tmpZero);
		T[i * 8 + 0][6] = _mm_sub_epi16(O00A, P00A);
		T[i * 8 + 0][7] = _mm_sub_epi16(O00B, P00B);
		T[i * 8 + 1][6] = _mm_sub_epi16(O01A, P01A);
		T[i * 8 + 1][7] = _mm_sub_epi16(O01B, P01B);
		T[i * 8 + 2][6] = _mm_sub_epi16(O02A, P02A);
		T[i * 8 + 2][7] = _mm_sub_epi16(O02B, P02B);
		T[i * 8 + 3][6] = _mm_sub_epi16(O03A, P03A);
		T[i * 8 + 3][7] = _mm_sub_epi16(O03B, P03B);
		T[i * 8 + 4][6] = _mm_sub_epi16(O04A, P04A);
		T[i * 8 + 4][7] = _mm_sub_epi16(O04B, P04B);
		T[i * 8 + 5][6] = _mm_sub_epi16(O05A, P05A);
		T[i * 8 + 5][7] = _mm_sub_epi16(O05B, P05B);
		T[i * 8 + 6][6] = _mm_sub_epi16(O06A, P06A);
		T[i * 8 + 6][7] = _mm_sub_epi16(O06B, P06B);
		T[i * 8 + 7][6] = _mm_sub_epi16(O07A, P07A);
		T[i * 8 + 7][7] = _mm_sub_epi16(O07B, P07B);

		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][0], T[i * 8 + 1][0], T[i * 8 + 2][0], T[i * 8 + 3][0], T[i * 8 + 4][0], T[i * 8 + 5][0], T[i * 8 + 6][0], T[i * 8 + 7][0], V[0][i], V[1][i], V[2][i], V[3][i], V[4][i], V[5][i], V[6][i], V[7][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][1], T[i * 8 + 1][1], T[i * 8 + 2][1], T[i * 8 + 3][1], T[i * 8 + 4][1], T[i * 8 + 5][1], T[i * 8 + 6][1], T[i * 8 + 7][1], V[8][i], V[9][i], V[10][i], V[11][i], V[12][i], V[13][i], V[14][i], V[15][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][2], T[i * 8 + 1][2], T[i * 8 + 2][2], T[i * 8 + 3][2], T[i * 8 + 4][2], T[i * 8 + 5][2], T[i * 8 + 6][2], T[i * 8 + 7][2], V[16][i], V[17][i], V[18][i], V[19][i], V[20][i], V[21][i], V[22][i], V[23][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][3], T[i * 8 + 1][3], T[i * 8 + 2][3], T[i * 8 + 3][3], T[i * 8 + 4][3], T[i * 8 + 5][3], T[i * 8 + 6][3], T[i * 8 + 7][3], V[24][i], V[25][i], V[26][i], V[27][i], V[28][i], V[29][i], V[30][i], V[31][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][4], T[i * 8 + 1][4], T[i * 8 + 2][4], T[i * 8 + 3][4], T[i * 8 + 4][4], T[i * 8 + 5][4], T[i * 8 + 6][4], T[i * 8 + 7][4], V[32][i], V[33][i], V[34][i], V[35][i], V[36][i], V[37][i], V[38][i], V[39][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][5], T[i * 8 + 1][5], T[i * 8 + 2][5], T[i * 8 + 3][5], T[i * 8 + 4][5], T[i * 8 + 5][5], T[i * 8 + 6][5], T[i * 8 + 7][5], V[40][i], V[41][i], V[42][i], V[43][i], V[44][i], V[45][i], V[46][i], V[47][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][6], T[i * 8 + 1][6], T[i * 8 + 2][6], T[i * 8 + 3][6], T[i * 8 + 4][6], T[i * 8 + 5][6], T[i * 8 + 6][6], T[i * 8 + 7][6], V[48][i], V[49][i], V[50][i], V[51][i], V[52][i], V[53][i], V[54][i], V[55][i]);
		TRANSPOSE_8x8_16BIT(T[i * 8 + 0][7], T[i * 8 + 1][7], T[i * 8 + 2][7], T[i * 8 + 3][7], T[i * 8 + 4][7], T[i * 8 + 5][7], T[i * 8 + 6][7], T[i * 8 + 7][7], V[56][i], V[57][i], V[58][i], V[59][i], V[60][i], V[61][i], V[62][i], V[63][i]);

		//filter H
		tt = _mm_srai_epi16(_mm_add_epi16(V[0][i], V[2][i]), 1);
		V[1][i] = _mm_sub_epi16(V[1][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[2][i], V[4][i]), 1);
		V[3][i] = _mm_sub_epi16(V[3][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[4][i], V[6][i]), 1);
		V[5][i] = _mm_sub_epi16(V[5][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[6][i], V[8][i]), 1);
		V[7][i] = _mm_sub_epi16(V[7][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[8][i], V[10][i]), 1);
		V[9][i] = _mm_sub_epi16(V[9][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[10][i], V[12][i]), 1);
		V[11][i] = _mm_sub_epi16(V[11][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[12][i], V[14][i]), 1);
		V[13][i] = _mm_sub_epi16(V[13][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[14][i], V[16][i]), 1);
		V[15][i] = _mm_sub_epi16(V[15][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[16][i], V[18][i]), 1);
		V[17][i] = _mm_sub_epi16(V[17][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[18][i], V[20][i]), 1);
		V[19][i] = _mm_sub_epi16(V[19][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[20][i], V[22][i]), 1);
		V[21][i] = _mm_sub_epi16(V[21][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[22][i], V[24][i]), 1);
		V[23][i] = _mm_sub_epi16(V[23][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[24][i], V[26][i]), 1);
		V[25][i] = _mm_sub_epi16(V[25][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[26][i], V[28][i]), 1);
		V[27][i] = _mm_sub_epi16(V[27][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[28][i], V[30][i]), 1);
		V[29][i] = _mm_sub_epi16(V[29][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[30][i], V[32][i]), 1);
		V[31][i] = _mm_sub_epi16(V[31][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[32][i], V[34][i]), 1);
		V[33][i] = _mm_sub_epi16(V[33][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[34][i], V[36][i]), 1);
		V[35][i] = _mm_sub_epi16(V[35][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[36][i], V[38][i]), 1);
		V[37][i] = _mm_sub_epi16(V[37][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[38][i], V[40][i]), 1);
		V[39][i] = _mm_sub_epi16(V[39][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[40][i], V[42][i]), 1);
		V[41][i] = _mm_sub_epi16(V[41][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[42][i], V[44][i]), 1);
		V[43][i] = _mm_sub_epi16(V[43][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[44][i], V[46][i]), 1);
		V[45][i] = _mm_sub_epi16(V[45][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[46][i], V[48][i]), 1);
		V[47][i] = _mm_sub_epi16(V[47][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[48][i], V[50][i]), 1);
		V[49][i] = _mm_sub_epi16(V[49][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[50][i], V[52][i]), 1);
		V[51][i] = _mm_sub_epi16(V[51][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[52][i], V[54][i]), 1);
		V[53][i] = _mm_sub_epi16(V[53][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[54][i], V[56][i]), 1);
		V[55][i] = _mm_sub_epi16(V[55][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[56][i], V[58][i]), 1);
		V[57][i] = _mm_sub_epi16(V[57][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[58][i], V[60][i]), 1);
		V[59][i] = _mm_sub_epi16(V[59][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[60][i], V[62][i]), 1);
		V[61][i] = _mm_sub_epi16(V[61][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(V[62][i], V[62][i]), 1);
		V[63][i] = _mm_sub_epi16(V[63][i], tt);

		//filter L
		tt = _mm_add_epi16(_mm_add_epi16(V[1][i], V[1][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[0][i] = _mm_add_epi16(V[0][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[1][i], V[3][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[2][i] = _mm_add_epi16(V[2][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[3][i], V[5][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[4][i] = _mm_add_epi16(V[4][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[5][i], V[7][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[6][i] = _mm_add_epi16(V[6][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[7][i], V[9][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[8][i] = _mm_add_epi16(V[8][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[9][i], V[11][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[10][i] = _mm_add_epi16(V[10][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[11][i], V[13][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[12][i] = _mm_add_epi16(V[12][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[13][i], V[15][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[14][i] = _mm_add_epi16(V[14][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[15][i], V[17][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[16][i] = _mm_add_epi16(V[16][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[17][i], V[19][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[18][i] = _mm_add_epi16(V[18][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[19][i], V[21][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[20][i] = _mm_add_epi16(V[20][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[21][i], V[23][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[22][i] = _mm_add_epi16(V[22][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[23][i], V[25][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[24][i] = _mm_add_epi16(V[24][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[25][i], V[27][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[26][i] = _mm_add_epi16(V[26][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[27][i], V[29][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[28][i] = _mm_add_epi16(V[28][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[29][i], V[31][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[30][i] = _mm_add_epi16(V[30][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[31][i], V[33][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[32][i] = _mm_add_epi16(V[32][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[33][i], V[35][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[34][i] = _mm_add_epi16(V[34][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[35][i], V[37][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[36][i] = _mm_add_epi16(V[36][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[37][i], V[39][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[38][i] = _mm_add_epi16(V[38][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[39][i], V[41][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[40][i] = _mm_add_epi16(V[40][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[41][i], V[43][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[42][i] = _mm_add_epi16(V[42][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[43][i], V[45][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[44][i] = _mm_add_epi16(V[44][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[45][i], V[47][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[46][i] = _mm_add_epi16(V[46][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[47][i], V[49][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[48][i] = _mm_add_epi16(V[48][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[49][i], V[51][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[50][i] = _mm_add_epi16(V[50][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[51][i], V[53][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[52][i] = _mm_add_epi16(V[52][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[53][i], V[55][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[54][i] = _mm_add_epi16(V[54][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[55][i], V[57][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[56][i] = _mm_add_epi16(V[56][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[57][i], V[59][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[58][i] = _mm_add_epi16(V[58][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[59][i], V[61][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[60][i] = _mm_add_epi16(V[60][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(V[61][i], V[63][i]), k2);
		tt = _mm_srai_epi16(tt, 2);
		V[62][i] = _mm_add_epi16(V[62][i], tt);

		TRANSPOSE_8x8_16BIT(V[0][i], V[2][i], V[4][i], V[6][i], V[8][i], V[10][i], V[12][i], V[14][i], T[i * 8 + 0][0], T[i * 8 + 1][0], T[i * 8 + 2][0], T[i * 8 + 3][0], T[i * 8 + 4][0], T[i * 8 + 5][0], T[i * 8 + 6][0], T[i * 8 + 7][0]);
		TRANSPOSE_8x8_16BIT(V[16][i], V[18][i], V[20][i], V[22][i], V[24][i], V[26][i], V[28][i], V[30][i], T[i * 8 + 0][1], T[i * 8 + 1][1], T[i * 8 + 2][1], T[i * 8 + 3][1], T[i * 8 + 4][1], T[i * 8 + 5][1], T[i * 8 + 6][1], T[i * 8 + 7][1]);
		TRANSPOSE_8x8_16BIT(V[32][i], V[34][i], V[36][i], V[38][i], V[40][i], V[42][i], V[44][i], V[46][i], T[i * 8 + 0][2], T[i * 8 + 1][2], T[i * 8 + 2][2], T[i * 8 + 3][2], T[i * 8 + 4][2], T[i * 8 + 5][2], T[i * 8 + 6][2], T[i * 8 + 7][2]);
		TRANSPOSE_8x8_16BIT(V[48][i], V[50][i], V[52][i], V[54][i], V[56][i], V[58][i], V[60][i], V[62][i], T[i * 8 + 0][3], T[i * 8 + 1][3], T[i * 8 + 2][3], T[i * 8 + 3][3], T[i * 8 + 4][3], T[i * 8 + 5][3], T[i * 8 + 6][3], T[i * 8 + 7][3]);
	}

	for (i = 0; i < 4; i++){
		tt = _mm_srai_epi16(_mm_add_epi16(T[0][i], T[2][i]), 1);
		T[1][i] = _mm_sub_epi16(T[1][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[2][i], T[4][i]), 1);
		T[3][i] = _mm_sub_epi16(T[3][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[4][i], T[6][i]), 1);
		T[5][i] = _mm_sub_epi16(T[5][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[6][i], T[8][i]), 1);
		T[7][i] = _mm_sub_epi16(T[7][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[8][i], T[10][i]), 1);
		T[9][i] = _mm_sub_epi16(T[9][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[10][i], T[12][i]), 1);
		T[11][i] = _mm_sub_epi16(T[11][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[12][i], T[14][i]), 1);
		T[13][i] = _mm_sub_epi16(T[13][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[14][i], T[16][i]), 1);
		T[15][i] = _mm_sub_epi16(T[15][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[16][i], T[18][i]), 1);
		T[17][i] = _mm_sub_epi16(T[17][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[18][i], T[20][i]), 1);
		T[19][i] = _mm_sub_epi16(T[19][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[20][i], T[22][i]), 1);
		T[21][i] = _mm_sub_epi16(T[21][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[22][i], T[24][i]), 1);
		T[23][i] = _mm_sub_epi16(T[23][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[24][i], T[26][i]), 1);
		T[25][i] = _mm_sub_epi16(T[25][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[26][i], T[28][i]), 1);
		T[27][i] = _mm_sub_epi16(T[27][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[28][i], T[30][i]), 1);
		T[29][i] = _mm_sub_epi16(T[29][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[30][i], T[32][i]), 1);
		T[31][i] = _mm_sub_epi16(T[31][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[32][i], T[34][i]), 1);
		T[33][i] = _mm_sub_epi16(T[33][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[34][i], T[36][i]), 1);
		T[35][i] = _mm_sub_epi16(T[35][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[36][i], T[38][i]), 1);
		T[37][i] = _mm_sub_epi16(T[37][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[38][i], T[40][i]), 1);
		T[39][i] = _mm_sub_epi16(T[39][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[40][i], T[42][i]), 1);
		T[41][i] = _mm_sub_epi16(T[41][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[42][i], T[44][i]), 1);
		T[43][i] = _mm_sub_epi16(T[43][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[44][i], T[46][i]), 1);
		T[45][i] = _mm_sub_epi16(T[45][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[46][i], T[48][i]), 1);
		T[47][i] = _mm_sub_epi16(T[47][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[48][i], T[50][i]), 1);
		T[49][i] = _mm_sub_epi16(T[49][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[50][i], T[52][i]), 1);
		T[51][i] = _mm_sub_epi16(T[51][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[52][i], T[54][i]), 1);
		T[53][i] = _mm_sub_epi16(T[53][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[54][i], T[56][i]), 1);
		T[55][i] = _mm_sub_epi16(T[55][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[56][i], T[58][i]), 1);
		T[57][i] = _mm_sub_epi16(T[57][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[58][i], T[60][i]), 1);
		T[59][i] = _mm_sub_epi16(T[59][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[60][i], T[62][i]), 1);
		T[61][i] = _mm_sub_epi16(T[61][i], tt);
		tt = _mm_srai_epi16(_mm_add_epi16(T[62][i], T[62][i]), 1);
		T[63][i] = _mm_sub_epi16(T[63][i], tt);

		tt = _mm_add_epi16(_mm_add_epi16(T[1][i], T[1][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[0][i] = _mm_slli_epi16(T[0][i], 1);
		T[0][i] = _mm_add_epi16(T[0][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[1][i], T[3][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[2][i] = _mm_slli_epi16(T[2][i], 1);
		T[2][i] = _mm_add_epi16(T[2][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[3][i], T[5][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[4][i] = _mm_slli_epi16(T[4][i], 1);
		T[4][i] = _mm_add_epi16(T[4][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[5][i], T[7][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[6][i] = _mm_slli_epi16(T[6][i], 1);
		T[6][i] = _mm_add_epi16(T[6][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[7][i], T[9][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[8][i] = _mm_slli_epi16(T[8][i], 1);
		T[8][i] = _mm_add_epi16(T[8][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[9][i], T[11][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[10][i] = _mm_slli_epi16(T[10][i], 1);
		T[10][i] = _mm_add_epi16(T[10][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[11][i], T[13][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[12][i] = _mm_slli_epi16(T[12][i], 1);
		T[12][i] = _mm_add_epi16(T[12][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[13][i], T[15][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[14][i] = _mm_slli_epi16(T[14][i], 1);
		T[14][i] = _mm_add_epi16(T[14][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[15][i], T[17][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[16][i] = _mm_slli_epi16(T[16][i], 1);
		T[16][i] = _mm_add_epi16(T[16][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[17][i], T[19][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[18][i] = _mm_slli_epi16(T[18][i], 1);
		T[18][i] = _mm_add_epi16(T[18][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[19][i], T[21][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[20][i] = _mm_slli_epi16(T[20][i], 1);
		T[20][i] = _mm_add_epi16(T[20][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[21][i], T[23][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[22][i] = _mm_slli_epi16(T[22][i], 1);
		T[22][i] = _mm_add_epi16(T[22][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[23][i], T[25][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[24][i] = _mm_slli_epi16(T[24][i], 1);
		T[24][i] = _mm_add_epi16(T[24][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[25][i], T[27][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[26][i] = _mm_slli_epi16(T[26][i], 1);
		T[26][i] = _mm_add_epi16(T[26][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[27][i], T[29][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[28][i] = _mm_slli_epi16(T[28][i], 1);
		T[28][i] = _mm_add_epi16(T[28][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[29][i], T[31][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[30][i] = _mm_slli_epi16(T[30][i], 1);
		T[30][i] = _mm_add_epi16(T[30][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[31][i], T[33][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[32][i] = _mm_slli_epi16(T[32][i], 1);
		T[32][i] = _mm_add_epi16(T[32][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[33][i], T[35][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[34][i] = _mm_slli_epi16(T[34][i], 1);
		T[34][i] = _mm_add_epi16(T[34][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[35][i], T[37][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[36][i] = _mm_slli_epi16(T[36][i], 1);
		T[36][i] = _mm_add_epi16(T[36][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[37][i], T[39][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[38][i] = _mm_slli_epi16(T[38][i], 1);
		T[38][i] = _mm_add_epi16(T[38][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[39][i], T[41][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[40][i] = _mm_slli_epi16(T[40][i], 1);
		T[40][i] = _mm_add_epi16(T[40][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[41][i], T[43][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[42][i] = _mm_slli_epi16(T[42][i], 1);
		T[42][i] = _mm_add_epi16(T[42][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[43][i], T[45][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[44][i] = _mm_slli_epi16(T[44][i], 1);
		T[44][i] = _mm_add_epi16(T[44][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[45][i], T[47][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[46][i] = _mm_slli_epi16(T[46][i], 1);
		T[46][i] = _mm_add_epi16(T[46][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[47][i], T[49][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[48][i] = _mm_slli_epi16(T[48][i], 1);
		T[48][i] = _mm_add_epi16(T[48][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[49][i], T[51][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[50][i] = _mm_slli_epi16(T[50][i], 1);
		T[50][i] = _mm_add_epi16(T[50][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[51][i], T[53][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[52][i] = _mm_slli_epi16(T[52][i], 1);
		T[52][i] = _mm_add_epi16(T[52][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[53][i], T[55][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[54][i] = _mm_slli_epi16(T[54][i], 1);
		T[54][i] = _mm_add_epi16(T[54][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[55][i], T[57][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[56][i] = _mm_slli_epi16(T[56][i], 1);
		T[56][i] = _mm_add_epi16(T[56][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[57][i], T[59][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[58][i] = _mm_slli_epi16(T[58][i], 1);
		T[58][i] = _mm_add_epi16(T[58][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[59][i], T[61][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[60][i] = _mm_slli_epi16(T[60][i], 1);
		T[60][i] = _mm_add_epi16(T[60][i], tt);
		tt = _mm_add_epi16(_mm_add_epi16(T[61][i], T[63][i]), k1);
		tt = _mm_srai_epi16(tt, 1);
		T[62][i] = _mm_slli_epi16(T[62][i], 1);
		T[62][i] = _mm_add_epi16(T[62][i], tt);

        temp_resi = resi + (i << 3);

		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 0), T[0][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 1), T[2][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 2), T[4][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 3), T[6][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 4), T[8][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 5), T[10][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 6), T[12][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 7), T[14][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 8), T[16][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 9), T[18][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 10), T[20][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 11), T[22][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 12), T[24][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 13), T[26][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 14), T[28][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 15), T[30][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 16), T[32][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 17), T[34][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 18), T[36][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 19), T[38][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 20), T[40][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 21), T[42][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 22), T[44][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 23), T[46][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 24), T[48][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 25), T[50][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 26), T[52][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 27), T[54][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 28), T[56][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 29), T[58][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 30), T[60][i]);
		_mm_storeu_si128((__m128i *)(temp_resi + 32 * 31), T[62][i]);
	}

	///// DCT /////

	// DCT1
	for (i = 0; i < 32 / 8; i++)
	{
		//load data
        temp_resi = resi + (i << 8);

		T00A = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 +  0]);    // [07 06 05 04 03 02 01 00]
		T00B = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 +  8]);    // [15 14 13 12 11 10 09 08]
		T00C = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 + 16]);    // [23 22 21 20 19 18 17 16]
		T00D = _mm_loadu_si128((__m128i*)&temp_resi[0 * 32 + 24]);    // [31 30 29 28 27 26 25 24]
		T01A = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 +  0]);
		T01B = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 +  8]);
		T01C = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 + 16]);
		T01D = _mm_loadu_si128((__m128i*)&temp_resi[1 * 32 + 24]);
		T02A = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 +  0]);
		T02B = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 +  8]);
		T02C = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 + 16]);
		T02D = _mm_loadu_si128((__m128i*)&temp_resi[2 * 32 + 24]);
		T03A = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 +  0]);
		T03B = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 +  8]);
		T03C = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 + 16]);
		T03D = _mm_loadu_si128((__m128i*)&temp_resi[3 * 32 + 24]);
		T04A = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 +  0]);
		T04B = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 +  8]);
		T04C = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 + 16]);
		T04D = _mm_loadu_si128((__m128i*)&temp_resi[4 * 32 + 24]);
		T05A = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 +  0]);
		T05B = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 +  8]);
		T05C = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 + 16]);
		T05D = _mm_loadu_si128((__m128i*)&temp_resi[5 * 32 + 24]);
		T06A = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 +  0]);
		T06B = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 +  8]);
		T06C = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 + 16]);
		T06D = _mm_loadu_si128((__m128i*)&temp_resi[6 * 32 + 24]);
		T07A = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 +  0]);
		T07B = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 +  8]);
		T07C = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 + 16]);
		T07D = _mm_loadu_si128((__m128i*)&temp_resi[7 * 32 + 24]);
		//
		T00A = _mm_shuffle_epi8(T00A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));    // [05 02 06 01 04 03 07 00]
		T00B = _mm_shuffle_epi8(T00B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));    // [10 13 09 14 11 12 08 15]
		T00C = _mm_shuffle_epi8(T00C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));    // [21 18 22 17 20 19 23 16]
		T00D = _mm_shuffle_epi8(T00D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));    // [26 29 25 30 27 28 24 31]
		T01A = _mm_shuffle_epi8(T01A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T01B = _mm_shuffle_epi8(T01B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T01C = _mm_shuffle_epi8(T01C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T01D = _mm_shuffle_epi8(T01D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T02A = _mm_shuffle_epi8(T02A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T02B = _mm_shuffle_epi8(T02B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T02C = _mm_shuffle_epi8(T02C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T02D = _mm_shuffle_epi8(T02D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T03A = _mm_shuffle_epi8(T03A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T03B = _mm_shuffle_epi8(T03B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T03C = _mm_shuffle_epi8(T03C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T03D = _mm_shuffle_epi8(T03D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T04A = _mm_shuffle_epi8(T04A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T04B = _mm_shuffle_epi8(T04B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T04C = _mm_shuffle_epi8(T04C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T04D = _mm_shuffle_epi8(T04D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T05A = _mm_shuffle_epi8(T05A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T05B = _mm_shuffle_epi8(T05B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T05C = _mm_shuffle_epi8(T05C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T05D = _mm_shuffle_epi8(T05D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T06A = _mm_shuffle_epi8(T06A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T06B = _mm_shuffle_epi8(T06B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T06C = _mm_shuffle_epi8(T06C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T06D = _mm_shuffle_epi8(T06D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T07A = _mm_shuffle_epi8(T07A, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T07B = _mm_shuffle_epi8(T07B, _mm_load_si128((__m128i*)tab_dct_32_0[0]));
		T07C = _mm_shuffle_epi8(T07C, _mm_load_si128((__m128i*)tab_dct_16_0[1]));
		T07D = _mm_shuffle_epi8(T07D, _mm_load_si128((__m128i*)tab_dct_32_0[0]));

		T10A = _mm_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm_add_epi16(T01A, T01D);
		T11B = _mm_add_epi16(T01B, T01C);
		T12A = _mm_add_epi16(T02A, T02D);
		T12B = _mm_add_epi16(T02B, T02C);
		T13A = _mm_add_epi16(T03A, T03D);
		T13B = _mm_add_epi16(T03B, T03C);
		T14A = _mm_add_epi16(T04A, T04D);
		T14B = _mm_add_epi16(T04B, T04C);
		T15A = _mm_add_epi16(T05A, T05D);
		T15B = _mm_add_epi16(T05B, T05C);
		T16A = _mm_add_epi16(T06A, T06D);
		T16B = _mm_add_epi16(T06B, T06C);
		T17A = _mm_add_epi16(T07A, T07D);
		T17B = _mm_add_epi16(T07B, T07C);

		T00A = _mm_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm_sub_epi16(T01A, T01D);
		T01B = _mm_sub_epi16(T01B, T01C);
		T02A = _mm_sub_epi16(T02A, T02D);
		T02B = _mm_sub_epi16(T02B, T02C);
		T03A = _mm_sub_epi16(T03A, T03D);
		T03B = _mm_sub_epi16(T03B, T03C);
		T04A = _mm_sub_epi16(T04A, T04D);
		T04B = _mm_sub_epi16(T04B, T04C);
		T05A = _mm_sub_epi16(T05A, T05D);
		T05B = _mm_sub_epi16(T05B, T05C);
		T06A = _mm_sub_epi16(T06A, T06D);
		T06B = _mm_sub_epi16(T06B, T06C);
		T07A = _mm_sub_epi16(T07A, T07D);
		T07B = _mm_sub_epi16(T07B, T07C);

		T20 = _mm_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm_add_epi16(T11A, T11B);
		T22 = _mm_add_epi16(T12A, T12B);
		T23 = _mm_add_epi16(T13A, T13B);
		T24 = _mm_add_epi16(T14A, T14B);
		T25 = _mm_add_epi16(T15A, T15B);
		T26 = _mm_add_epi16(T16A, T16B);
		T27 = _mm_add_epi16(T17A, T17B);

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_8[1]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_8[1]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[0][i] = T60;

		T30 = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T31 = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T32 = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T33 = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T34 = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T35 = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T36 = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_16_1[8]));
		T37 = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_16_1[8]));

		T40 = _mm_hadd_epi32(T30, T31);
		T41 = _mm_hadd_epi32(T32, T33);
		T42 = _mm_hadd_epi32(T34, T35);
		T43 = _mm_hadd_epi32(T36, T37);

		T50 = _mm_hadd_epi32(T40, T41);
		T51 = _mm_hadd_epi32(T42, T43);
		T50 = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1);
		T51 = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1);
		T60 = _mm_packs_epi32(T50, T51);
		im[8][i] = T60;

#define MAKE_ODD(tab, dstPos) \
    T30  = _mm_madd_epi16(T20, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T21, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T32  = _mm_madd_epi16(T22, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T23, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T34  = _mm_madd_epi16(T24, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T25, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T36  = _mm_madd_epi16(T26, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T27, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
        \
    T40  = _mm_hadd_epi32(T30, T31); \
    T41  = _mm_hadd_epi32(T32, T33); \
    T42  = _mm_hadd_epi32(T34, T35); \
    T43  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);

		T20 = _mm_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm_sub_epi16(T11A, T11B);
		T22 = _mm_sub_epi16(T12A, T12B);
		T23 = _mm_sub_epi16(T13A, T13B);
		T24 = _mm_sub_epi16(T14A, T14B);
		T25 = _mm_sub_epi16(T15A, T15B);
		T26 = _mm_sub_epi16(T16A, T16B);
		T27 = _mm_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);
#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
    T20  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T21  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T22  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T23  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T24  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T25  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T26  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T27  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T30  = _mm_madd_epi16(T04A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T31  = _mm_madd_epi16(T04B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T32  = _mm_madd_epi16(T05A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T33  = _mm_madd_epi16(T05B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T34  = _mm_madd_epi16(T06A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T35  = _mm_madd_epi16(T06B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
    T36  = _mm_madd_epi16(T07A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab)])); \
    T37  = _mm_madd_epi16(T07B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab) + 1])); \
        \
    T40  = _mm_hadd_epi32(T20, T21); \
    T41  = _mm_hadd_epi32(T22, T23); \
    T42  = _mm_hadd_epi32(T24, T25); \
    T43  = _mm_hadd_epi32(T26, T27); \
    T44  = _mm_hadd_epi32(T30, T31); \
    T45  = _mm_hadd_epi32(T32, T33); \
    T46  = _mm_hadd_epi32(T34, T35); \
    T47  = _mm_hadd_epi32(T36, T37); \
        \
    T50  = _mm_hadd_epi32(T40, T41); \
    T51  = _mm_hadd_epi32(T42, T43); \
    T52  = _mm_hadd_epi32(T44, T45); \
    T53  = _mm_hadd_epi32(T46, T47); \
        \
    T50  = _mm_hadd_epi32(T50, T51); \
    T51  = _mm_hadd_epi32(T52, T53); \
    T50  = _mm_srai_epi32(_mm_add_epi32(T50, c_8), shift1); \
    T51  = _mm_srai_epi32(_mm_add_epi32(T51, c_8), shift1); \
    T60  = _mm_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);

#undef MAKE_ODD
	}

	// DCT2
	for (i = 0; i < 2; i++)
	{
        int idx = i << 3;

		T00A = im[idx + 0][0];    // [07 06 05 04 03 02 01 00]
		T00B = im[idx + 0][1];    // [15 14 13 12 11 10 09 08]
		T00C = im[idx + 0][2];    // [23 22 21 20 19 18 17 16]
		T00D = im[idx + 0][3];    // [31 30 29 28 27 26 25 24]
		T01A = im[idx + 1][0];
		T01B = im[idx + 1][1];
		T01C = im[idx + 1][2];
		T01D = im[idx + 1][3];
		T02A = im[idx + 2][0];
		T02B = im[idx + 2][1];
		T02C = im[idx + 2][2];
		T02D = im[idx + 2][3];
		T03A = im[idx + 3][0];
		T03B = im[idx + 3][1];
		T03C = im[idx + 3][2];
		T03D = im[idx + 3][3];

		TT00A = im[idx + 4][0];
		TT00B = im[idx + 4][1];
		TT00C = im[idx + 4][2];
		TT00D = im[idx + 4][3];
		TT01A = im[idx + 5][0];
		TT01B = im[idx + 5][1];
		TT01C = im[idx + 5][2];
		TT01D = im[idx + 5][3];
		TT02A = im[idx + 6][0];
		TT02B = im[idx + 6][1];
		TT02C = im[idx + 6][2];
		TT02D = im[idx + 6][3];
		TT03A = im[idx + 7][0];
		TT03B = im[idx + 7][1];
		TT03C = im[idx + 7][2];
		TT03D = im[idx + 7][3];

		T00C = _mm_shuffle_epi8(T00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [16 17 18 19 20 21 22 23]
		T00D = _mm_shuffle_epi8(T00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));    // [24 25 26 27 28 29 30 31]
		T01C = _mm_shuffle_epi8(T01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T01D = _mm_shuffle_epi8(T01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02C = _mm_shuffle_epi8(T02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T02D = _mm_shuffle_epi8(T02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03C = _mm_shuffle_epi8(T03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		T03D = _mm_shuffle_epi8(T03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		TT00C = _mm_shuffle_epi8(TT00C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT00D = _mm_shuffle_epi8(TT00D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01C = _mm_shuffle_epi8(TT01C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT01D = _mm_shuffle_epi8(TT01D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02C = _mm_shuffle_epi8(TT02C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT02D = _mm_shuffle_epi8(TT02D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03C = _mm_shuffle_epi8(TT03C, _mm_load_si128((__m128i*)tab_dct_16_0[0]));
		TT03D = _mm_shuffle_epi8(TT03D, _mm_load_si128((__m128i*)tab_dct_16_0[0]));

		T10A = _mm_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
		T10B = _mm_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
		T00A = _mm_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
		T00B = _mm_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
		T11A = _mm_unpacklo_epi16(T01A, T01D);
		T11B = _mm_unpackhi_epi16(T01A, T01D);
		T01A = _mm_unpacklo_epi16(T01B, T01C);
		T01B = _mm_unpackhi_epi16(T01B, T01C);
		T12A = _mm_unpacklo_epi16(T02A, T02D);
		T12B = _mm_unpackhi_epi16(T02A, T02D);
		T02A = _mm_unpacklo_epi16(T02B, T02C);
		T02B = _mm_unpackhi_epi16(T02B, T02C);
		T13A = _mm_unpacklo_epi16(T03A, T03D);
		T13B = _mm_unpackhi_epi16(T03A, T03D);
		T03A = _mm_unpacklo_epi16(T03B, T03C);
		T03B = _mm_unpackhi_epi16(T03B, T03C);

		TT10A = _mm_unpacklo_epi16(TT00A, TT00D);
		TT10B = _mm_unpackhi_epi16(TT00A, TT00D);
		TT00A = _mm_unpacklo_epi16(TT00B, TT00C);
		TT00B = _mm_unpackhi_epi16(TT00B, TT00C);
		TT11A = _mm_unpacklo_epi16(TT01A, TT01D);
		TT11B = _mm_unpackhi_epi16(TT01A, TT01D);
		TT01A = _mm_unpacklo_epi16(TT01B, TT01C);
		TT01B = _mm_unpackhi_epi16(TT01B, TT01C);
		TT12A = _mm_unpacklo_epi16(TT02A, TT02D);
		TT12B = _mm_unpackhi_epi16(TT02A, TT02D);
		TT02A = _mm_unpacklo_epi16(TT02B, TT02C);
		TT02B = _mm_unpackhi_epi16(TT02B, TT02C);
		TT13A = _mm_unpacklo_epi16(TT03A, TT03D);
		TT13B = _mm_unpackhi_epi16(TT03A, TT03D);
		TT03A = _mm_unpacklo_epi16(TT03B, TT03C);
		TT03B = _mm_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
    T20  = _mm_madd_epi16(T10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T21  = _mm_madd_epi16(T10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T22  = _mm_madd_epi16(T00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T23  = _mm_madd_epi16(T00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T24  = _mm_madd_epi16(T11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T25  = _mm_madd_epi16(T11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T26  = _mm_madd_epi16(T01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T27  = _mm_madd_epi16(T01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T30  = _mm_madd_epi16(T12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T31  = _mm_madd_epi16(T12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T32  = _mm_madd_epi16(T02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T33  = _mm_madd_epi16(T02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    T34  = _mm_madd_epi16(T13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    T35  = _mm_madd_epi16(T13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    T36  = _mm_madd_epi16(T03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    T37  = _mm_madd_epi16(T03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    T60  = _mm_hadd_epi32(T20, T21); \
    T61  = _mm_hadd_epi32(T22, T23); \
    T62  = _mm_hadd_epi32(T24, T25); \
    T63  = _mm_hadd_epi32(T26, T27); \
    T64  = _mm_hadd_epi32(T30, T31); \
    T65  = _mm_hadd_epi32(T32, T33); \
    T66  = _mm_hadd_epi32(T34, T35); \
    T67  = _mm_hadd_epi32(T36, T37); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
    T62  = _mm_hadd_epi32(T64, T65); \
    T63  = _mm_hadd_epi32(T66, T67); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
    T61  = _mm_hadd_epi32(T62, T63); \
        \
    T60  = _mm_hadd_epi32(T60, T61); \
        \
    T60  = _mm_srai_epi32(_mm_add_epi32(T60, c_512), shift2); \
		\
	TT20  = _mm_madd_epi16(TT10A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT21  = _mm_madd_epi16(TT10B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT22  = _mm_madd_epi16(TT00A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT23  = _mm_madd_epi16(TT00B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT24  = _mm_madd_epi16(TT11A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT25  = _mm_madd_epi16(TT11B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT26  = _mm_madd_epi16(TT01A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT27  = _mm_madd_epi16(TT01B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT30  = _mm_madd_epi16(TT12A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT31  = _mm_madd_epi16(TT12B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT32  = _mm_madd_epi16(TT02A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT33  = _mm_madd_epi16(TT02B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
    TT34  = _mm_madd_epi16(TT13A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab0)])); \
    TT35  = _mm_madd_epi16(TT13B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab1)])); \
    TT36  = _mm_madd_epi16(TT03A, _mm_load_si128((__m128i*)tab_dct_32_1[(tab2)])); \
    TT37  = _mm_madd_epi16(TT03B, _mm_load_si128((__m128i*)tab_dct_32_1[(tab3)])); \
        \
    TT60  = _mm_hadd_epi32(TT20, TT21); \
    TT61  = _mm_hadd_epi32(TT22, TT23); \
    TT62  = _mm_hadd_epi32(TT24, TT25); \
    TT63  = _mm_hadd_epi32(TT26, TT27); \
    TT64  = _mm_hadd_epi32(TT30, TT31); \
    TT65  = _mm_hadd_epi32(TT32, TT33); \
    TT66  = _mm_hadd_epi32(TT34, TT35); \
    TT67  = _mm_hadd_epi32(TT36, TT37); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
    TT62  = _mm_hadd_epi32(TT64, TT65); \
    TT63  = _mm_hadd_epi32(TT66, TT67); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
    TT61  = _mm_hadd_epi32(TT62, TT63); \
        \
    TT60  = _mm_hadd_epi32(TT60, TT61); \
        \
    TT60  = _mm_srai_epi32(_mm_add_epi32(TT60, c_512), shift2); \
		\
	tResult = _mm_packs_epi32(T60, TT60);\
    _mm_storeu_si128((__m128i*)&dst[((dstPos) << 5) + idx + 0], tResult); \

		MAKE_ODD(44, 44, 44, 44, 0);
		MAKE_ODD(46, 47, 46, 47, 8);

		MAKE_ODD(50, 51, 52, 53, 4);
		MAKE_ODD(54, 55, 56, 57, 12);

		MAKE_ODD(66, 67, 68, 69, 2);
		MAKE_ODD(70, 71, 72, 73, 6);
		MAKE_ODD(74, 75, 76, 77, 10);
		MAKE_ODD(78, 79, 80, 81, 14);

		MAKE_ODD(98, 99, 100, 101, 1);
		MAKE_ODD(102, 103, 104, 105, 3);
		MAKE_ODD(106, 107, 108, 109, 5);
		MAKE_ODD(110, 111, 112, 113, 7);
		MAKE_ODD(114, 115, 116, 117, 9);
		MAKE_ODD(118, 119, 120, 121, 11);
		MAKE_ODD(122, 123, 124, 125, 13);
		MAKE_ODD(126, 127, 128, 129, 15);
#undef MAKE_ODD
	}

    for (i = 0; i < 16; i++) {
        _mm_storeu_si128((__m128i*)(dst + 16), tmpZero);
        _mm_storeu_si128((__m128i*)(dst + 24), tmpZero);
        dst += 32;
    }
    for (i = 16; i < 32; i++) {
        _mm_storeu_si128((__m128i*)(dst +  0), tmpZero);
        _mm_storeu_si128((__m128i*)(dst +  8), tmpZero);
        _mm_storeu_si128((__m128i*)(dst + 16), tmpZero);
        _mm_storeu_si128((__m128i*)(dst + 24), tmpZero);
        dst += 32;
    }
}

void sub_trans_ext_32x32_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	// Const
	__m256i c_4 = _mm256_set1_epi32(4);
	__m256i c_512 = _mm256_set1_epi32(512);
	int shift1 = 3;
	int shift2 = 10;
	int i;

	__m256i T00, T01, T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15;
	__m256i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m256i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m256i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m256i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m256i OO00A, OO01A, OO02A, OO03A, OO04A, OO05A, OO06A, OO07A;
	__m256i OO00B, OO01B, OO02B, OO03B, OO04B, OO05B, OO06B, OO07B;
	__m256i PP00A, PP01A, PP02A, PP03A, PP04A, PP05A, PP06A, PP07A;
	__m256i PP00B, PP01B, PP02B, PP03B, PP04B, PP05B, PP06B, PP07B;
	__m256i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m256i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m256i T20, T21, T22, T23, T24, T25, T26, T27;
	__m256i T30, T31, T32, T33, T34, T35, T36, T37;
	__m256i T40, T41, T42, T43, T44, T45, T46, T47;
	__m256i T50, T51, T52, T53;
	__m256i T60, T61, T62, T63, T64, T65, T66, T67;
	__m256i im[16][4];

	__m256i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m256i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m256i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m256i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m256i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m256i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m256i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m256i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m256i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m256i TT50, TT51, TT52, TT53;
	__m256i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m256i tResult;

	__m256i O00, O01, O02, O03, O04, O05, O06, O07, O08, O09, O10, O11, O12, O13, O14, O15;
	__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m256i P00, P01, P02, P03, P04, P05, P06, P07, P08, P09, P10, P11, P12, P13, P14, P15;
	__m256i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m256i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m256i tmpZero = _mm256_setzero_si256();
	__m256i TAB16_0_1, TAB32_0_0, TAB16_0_0;

	// DCT1
	for (i = 0; i < 2; i++)
	{
		//load data
		O00 = _mm256_loadu_si256((__m256i*)&org[0 * i_org]);	//256 bits are valid , 8*32
		O01 = _mm256_loadu_si256((__m256i*)&org[1 * i_org]);
		O02 = _mm256_loadu_si256((__m256i*)&org[2 * i_org]);
		O03 = _mm256_loadu_si256((__m256i*)&org[3 * i_org]);
		O04 = _mm256_loadu_si256((__m256i*)&org[4 * i_org]);
		O05 = _mm256_loadu_si256((__m256i*)&org[5 * i_org]);
		O06 = _mm256_loadu_si256((__m256i*)&org[6 * i_org]);
		O07 = _mm256_loadu_si256((__m256i*)&org[7 * i_org]);
		O08 = _mm256_loadu_si256((__m256i*)&org[8 * i_org]);
		O09 = _mm256_loadu_si256((__m256i*)&org[9 * i_org]);
		O10 = _mm256_loadu_si256((__m256i*)&org[10 * i_org]);
		O11 = _mm256_loadu_si256((__m256i*)&org[11 * i_org]);
		O12 = _mm256_loadu_si256((__m256i*)&org[12 * i_org]);
		O13 = _mm256_loadu_si256((__m256i*)&org[13 * i_org]);
		O14 = _mm256_loadu_si256((__m256i*)&org[14 * i_org]);
		O15 = _mm256_loadu_si256((__m256i*)&org[15 * i_org]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[0 * i_pred]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[1 * i_pred]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[2 * i_pred]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[3 * i_pred]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[4 * i_pred]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[5 * i_pred]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[6 * i_pred]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[7 * i_pred]);
		P08 = _mm256_loadu_si256((__m256i*)&pred[8 * i_pred]);
		P09 = _mm256_loadu_si256((__m256i*)&pred[9 * i_pred]);
		P10 = _mm256_loadu_si256((__m256i*)&pred[10 * i_pred]);
		P11 = _mm256_loadu_si256((__m256i*)&pred[11 * i_pred]);
		P12 = _mm256_loadu_si256((__m256i*)&pred[12 * i_pred]);
		P13 = _mm256_loadu_si256((__m256i*)&pred[13 * i_pred]);
		P14 = _mm256_loadu_si256((__m256i*)&pred[14 * i_pred]);
		P15 = _mm256_loadu_si256((__m256i*)&pred[15 * i_pred]);

		OO00A = _mm256_permute2x128_si256(O00, O08, 0x20);
		OO01A = _mm256_permute2x128_si256(O01, O09, 0x20);
		OO02A = _mm256_permute2x128_si256(O02, O10, 0x20);
		OO03A = _mm256_permute2x128_si256(O03, O11, 0x20);
		OO04A = _mm256_permute2x128_si256(O04, O12, 0x20);
		OO05A = _mm256_permute2x128_si256(O05, O13, 0x20);
		OO06A = _mm256_permute2x128_si256(O06, O14, 0x20);
		OO07A = _mm256_permute2x128_si256(O07, O15, 0x20);
		PP00A = _mm256_permute2x128_si256(P00, P08, 0x20);
		PP01A = _mm256_permute2x128_si256(P01, P09, 0x20);
		PP02A = _mm256_permute2x128_si256(P02, P10, 0x20);
		PP03A = _mm256_permute2x128_si256(P03, P11, 0x20);
		PP04A = _mm256_permute2x128_si256(P04, P12, 0x20);
		PP05A = _mm256_permute2x128_si256(P05, P13, 0x20);
		PP06A = _mm256_permute2x128_si256(P06, P14, 0x20);
		PP07A = _mm256_permute2x128_si256(P07, P15, 0x20);

		O00A = _mm256_unpacklo_epi8(OO00A, tmpZero);
		O00B = _mm256_unpackhi_epi8(OO00A, tmpZero);
		O01A = _mm256_unpacklo_epi8(OO01A, tmpZero);
		O01B = _mm256_unpackhi_epi8(OO01A, tmpZero);
		O02A = _mm256_unpacklo_epi8(OO02A, tmpZero);
		O02B = _mm256_unpackhi_epi8(OO02A, tmpZero);
		O03A = _mm256_unpacklo_epi8(OO03A, tmpZero);
		O03B = _mm256_unpackhi_epi8(OO03A, tmpZero);
		O04A = _mm256_unpacklo_epi8(OO04A, tmpZero);
		O04B = _mm256_unpackhi_epi8(OO04A, tmpZero);
		O05A = _mm256_unpacklo_epi8(OO05A, tmpZero);
		O05B = _mm256_unpackhi_epi8(OO05A, tmpZero);
		O06A = _mm256_unpacklo_epi8(OO06A, tmpZero);
		O06B = _mm256_unpackhi_epi8(OO06A, tmpZero);
		O07A = _mm256_unpacklo_epi8(OO07A, tmpZero);
		O07B = _mm256_unpackhi_epi8(OO07A, tmpZero);
		P00A = _mm256_unpacklo_epi8(PP00A, tmpZero);
		P00B = _mm256_unpackhi_epi8(PP00A, tmpZero);
		P01A = _mm256_unpacklo_epi8(PP01A, tmpZero);
		P01B = _mm256_unpackhi_epi8(PP01A, tmpZero);
		P02A = _mm256_unpacklo_epi8(PP02A, tmpZero);
		P02B = _mm256_unpackhi_epi8(PP02A, tmpZero);
		P03A = _mm256_unpacklo_epi8(PP03A, tmpZero);
		P03B = _mm256_unpackhi_epi8(PP03A, tmpZero);
		P04A = _mm256_unpacklo_epi8(PP04A, tmpZero);
		P04B = _mm256_unpackhi_epi8(PP04A, tmpZero);
		P05A = _mm256_unpacklo_epi8(PP05A, tmpZero);
		P05B = _mm256_unpackhi_epi8(PP05A, tmpZero);
		P06A = _mm256_unpacklo_epi8(PP06A, tmpZero);
		P06B = _mm256_unpackhi_epi8(PP06A, tmpZero);
		P07A = _mm256_unpacklo_epi8(PP07A, tmpZero);
		P07B = _mm256_unpackhi_epi8(PP07A, tmpZero);

		T00A = _mm256_sub_epi16(O00A, P00A);
		T00B = _mm256_sub_epi16(O00B, P00B);
		T01A = _mm256_sub_epi16(O01A, P01A);
		T01B = _mm256_sub_epi16(O01B, P01B);
		T02A = _mm256_sub_epi16(O02A, P02A);
		T02B = _mm256_sub_epi16(O02B, P02B);
		T03A = _mm256_sub_epi16(O03A, P03A);
		T03B = _mm256_sub_epi16(O03B, P03B);
		T04A = _mm256_sub_epi16(O04A, P04A);
		T04B = _mm256_sub_epi16(O04B, P04B);
		T05A = _mm256_sub_epi16(O05A, P05A);
		T05B = _mm256_sub_epi16(O05B, P05B);
		T06A = _mm256_sub_epi16(O06A, P06A);
		T06B = _mm256_sub_epi16(O06B, P06B);
		T07A = _mm256_sub_epi16(O07A, P07A);
		T07B = _mm256_sub_epi16(O07B, P07B);

		OO00B = _mm256_permute2x128_si256(O00, O08, 0x31);
		OO01B = _mm256_permute2x128_si256(O01, O09, 0x31);
		OO02B = _mm256_permute2x128_si256(O02, O10, 0x31);
		OO03B = _mm256_permute2x128_si256(O03, O11, 0x31);
		OO04B = _mm256_permute2x128_si256(O04, O12, 0x31);
		OO05B = _mm256_permute2x128_si256(O05, O13, 0x31);
		OO06B = _mm256_permute2x128_si256(O06, O14, 0x31);
		OO07B = _mm256_permute2x128_si256(O07, O15, 0x31);
		PP00B = _mm256_permute2x128_si256(P00, P08, 0x31);
		PP01B = _mm256_permute2x128_si256(P01, P09, 0x31);
		PP02B = _mm256_permute2x128_si256(P02, P10, 0x31);
		PP03B = _mm256_permute2x128_si256(P03, P11, 0x31);
		PP04B = _mm256_permute2x128_si256(P04, P12, 0x31);
		PP05B = _mm256_permute2x128_si256(P05, P13, 0x31);
		PP06B = _mm256_permute2x128_si256(P06, P14, 0x31);
		PP07B = _mm256_permute2x128_si256(P07, P15, 0x31);

		O00A = _mm256_unpacklo_epi8(OO00B, tmpZero);
		O00B = _mm256_unpackhi_epi8(OO00B, tmpZero);
		O01A = _mm256_unpacklo_epi8(OO01B, tmpZero);
		O01B = _mm256_unpackhi_epi8(OO01B, tmpZero);
		O02A = _mm256_unpacklo_epi8(OO02B, tmpZero);
		O02B = _mm256_unpackhi_epi8(OO02B, tmpZero);
		O03A = _mm256_unpacklo_epi8(OO03B, tmpZero);
		O03B = _mm256_unpackhi_epi8(OO03B, tmpZero);
		O04A = _mm256_unpacklo_epi8(OO04B, tmpZero);
		O04B = _mm256_unpackhi_epi8(OO04B, tmpZero);
		O05A = _mm256_unpacklo_epi8(OO05B, tmpZero);
		O05B = _mm256_unpackhi_epi8(OO05B, tmpZero);
		O06A = _mm256_unpacklo_epi8(OO06B, tmpZero);
		O06B = _mm256_unpackhi_epi8(OO06B, tmpZero);
		O07A = _mm256_unpacklo_epi8(OO07B, tmpZero);
		O07B = _mm256_unpackhi_epi8(OO07B, tmpZero);
		P00A = _mm256_unpacklo_epi8(PP00B, tmpZero);
		P00B = _mm256_unpackhi_epi8(PP00B, tmpZero);
		P01A = _mm256_unpacklo_epi8(PP01B, tmpZero);
		P01B = _mm256_unpackhi_epi8(PP01B, tmpZero);
		P02A = _mm256_unpacklo_epi8(PP02B, tmpZero);
		P02B = _mm256_unpackhi_epi8(PP02B, tmpZero);
		P03A = _mm256_unpacklo_epi8(PP03B, tmpZero);
		P03B = _mm256_unpackhi_epi8(PP03B, tmpZero);
		P04A = _mm256_unpacklo_epi8(PP04B, tmpZero);
		P04B = _mm256_unpackhi_epi8(PP04B, tmpZero);
		P05A = _mm256_unpacklo_epi8(PP05B, tmpZero);
		P05B = _mm256_unpackhi_epi8(PP05B, tmpZero);
		P06A = _mm256_unpacklo_epi8(PP06B, tmpZero);
		P06B = _mm256_unpackhi_epi8(PP06B, tmpZero);
		P07A = _mm256_unpacklo_epi8(PP07B, tmpZero);
		P07B = _mm256_unpackhi_epi8(PP07B, tmpZero);
		T00C = _mm256_sub_epi16(O00A, P00A);
		T00D = _mm256_sub_epi16(O00B, P00B);
		T01C = _mm256_sub_epi16(O01A, P01A);
		T01D = _mm256_sub_epi16(O01B, P01B);
		T02C = _mm256_sub_epi16(O02A, P02A);
		T02D = _mm256_sub_epi16(O02B, P02B);
		T03C = _mm256_sub_epi16(O03A, P03A);
		T03D = _mm256_sub_epi16(O03B, P03B);
		T04C = _mm256_sub_epi16(O04A, P04A);
		T04D = _mm256_sub_epi16(O04B, P04B);
		T05C = _mm256_sub_epi16(O05A, P05A);
		T05D = _mm256_sub_epi16(O05B, P05B);
		T06C = _mm256_sub_epi16(O06A, P06A);
		T06D = _mm256_sub_epi16(O06B, P06B);
		T07C = _mm256_sub_epi16(O07A, P07A);
		T07D = _mm256_sub_epi16(O07B, P07B);
		org += i_org << 4;
		pred += i_pred << 4;

		//
		TAB16_0_1 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[1]);
		TAB32_0_0 = _mm256_load_si256((__m256i*)tab_dct_32_0_256i[0]);
		T00A = _mm256_shuffle_epi8(T00A, TAB16_0_1);    // [05 02 06 01 04 03 07 00]
		T00B = _mm256_shuffle_epi8(T00B, TAB32_0_0);    // [10 13 09 14 11 12 08 15]
		T00C = _mm256_shuffle_epi8(T00C, TAB16_0_1);    // [21 18 22 17 20 19 23 16]
		T00D = _mm256_shuffle_epi8(T00D, TAB32_0_0);    // [26 29 25 30 27 28 24 31]
		T01A = _mm256_shuffle_epi8(T01A, TAB16_0_1);
		T01B = _mm256_shuffle_epi8(T01B, TAB32_0_0);
		T01C = _mm256_shuffle_epi8(T01C, TAB16_0_1);
		T01D = _mm256_shuffle_epi8(T01D, TAB32_0_0);
		T02A = _mm256_shuffle_epi8(T02A, TAB16_0_1);
		T02B = _mm256_shuffle_epi8(T02B, TAB32_0_0);
		T02C = _mm256_shuffle_epi8(T02C, TAB16_0_1);
		T02D = _mm256_shuffle_epi8(T02D, TAB32_0_0);
		T03A = _mm256_shuffle_epi8(T03A, TAB16_0_1);
		T03B = _mm256_shuffle_epi8(T03B, TAB32_0_0);
		T03C = _mm256_shuffle_epi8(T03C, TAB16_0_1);
		T03D = _mm256_shuffle_epi8(T03D, TAB32_0_0);
		T04A = _mm256_shuffle_epi8(T04A, TAB16_0_1);
		T04B = _mm256_shuffle_epi8(T04B, TAB32_0_0);
		T04C = _mm256_shuffle_epi8(T04C, TAB16_0_1);
		T04D = _mm256_shuffle_epi8(T04D, TAB32_0_0);
		T05A = _mm256_shuffle_epi8(T05A, TAB16_0_1);
		T05B = _mm256_shuffle_epi8(T05B, TAB32_0_0);
		T05C = _mm256_shuffle_epi8(T05C, TAB16_0_1);
		T05D = _mm256_shuffle_epi8(T05D, TAB32_0_0);
		T06A = _mm256_shuffle_epi8(T06A, TAB16_0_1);
		T06B = _mm256_shuffle_epi8(T06B, TAB32_0_0);
		T06C = _mm256_shuffle_epi8(T06C, TAB16_0_1);
		T06D = _mm256_shuffle_epi8(T06D, TAB32_0_0);
		T07A = _mm256_shuffle_epi8(T07A, TAB16_0_1);
		T07B = _mm256_shuffle_epi8(T07B, TAB32_0_0);
		T07C = _mm256_shuffle_epi8(T07C, TAB16_0_1);
		T07D = _mm256_shuffle_epi8(T07D, TAB32_0_0);

		T10A = _mm256_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm256_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm256_add_epi16(T01A, T01D);
		T11B = _mm256_add_epi16(T01B, T01C);
		T12A = _mm256_add_epi16(T02A, T02D);
		T12B = _mm256_add_epi16(T02B, T02C);
		T13A = _mm256_add_epi16(T03A, T03D);
		T13B = _mm256_add_epi16(T03B, T03C);
		T14A = _mm256_add_epi16(T04A, T04D);
		T14B = _mm256_add_epi16(T04B, T04C);
		T15A = _mm256_add_epi16(T05A, T05D);
		T15B = _mm256_add_epi16(T05B, T05C);
		T16A = _mm256_add_epi16(T06A, T06D);
		T16B = _mm256_add_epi16(T06B, T06C);
		T17A = _mm256_add_epi16(T07A, T07D);
		T17B = _mm256_add_epi16(T07B, T07C);

		T00A = _mm256_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm256_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm256_sub_epi16(T01A, T01D);
		T01B = _mm256_sub_epi16(T01B, T01C);
		T02A = _mm256_sub_epi16(T02A, T02D);
		T02B = _mm256_sub_epi16(T02B, T02C);
		T03A = _mm256_sub_epi16(T03A, T03D);
		T03B = _mm256_sub_epi16(T03B, T03C);
		T04A = _mm256_sub_epi16(T04A, T04D);
		T04B = _mm256_sub_epi16(T04B, T04C);
		T05A = _mm256_sub_epi16(T05A, T05D);
		T05B = _mm256_sub_epi16(T05B, T05C);
		T06A = _mm256_sub_epi16(T06A, T06D);
		T06B = _mm256_sub_epi16(T06B, T06C);
		T07A = _mm256_sub_epi16(T07A, T07D);
		T07B = _mm256_sub_epi16(T07B, T07C);

		T20 = _mm256_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm256_add_epi16(T11A, T11B);
		T22 = _mm256_add_epi16(T12A, T12B);
		T23 = _mm256_add_epi16(T13A, T13B);
		T24 = _mm256_add_epi16(T14A, T14B);
		T25 = _mm256_add_epi16(T15A, T15B);
		T26 = _mm256_add_epi16(T16A, T16B);
		T27 = _mm256_add_epi16(T17A, T17B);

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[0][i] = T60;

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[8][i] = T60;

#define MAKE_ODD(tab, dstPos) \
	T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	\
	T40 = _mm256_hadd_epi32(T30, T31); \
	T41 = _mm256_hadd_epi32(T32, T33); \
	T42 = _mm256_hadd_epi32(T34, T35); \
	T43 = _mm256_hadd_epi32(T36, T37); \
	\
	T50 = _mm256_hadd_epi32(T40, T41); \
	T51 = _mm256_hadd_epi32(T42, T43); \
	T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1); \
	T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1); \
	T60 = _mm256_packs_epi32(T50, T51); \
	im[(dstPos)][i] = T60;

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);

		T20 = _mm256_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm256_sub_epi16(T11A, T11B);
		T22 = _mm256_sub_epi16(T12A, T12B);
		T23 = _mm256_sub_epi16(T13A, T13B);
		T24 = _mm256_sub_epi16(T14A, T14B);
		T25 = _mm256_sub_epi16(T15A, T15B);
		T26 = _mm256_sub_epi16(T16A, T16B);
		T27 = _mm256_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);
#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
	T20 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T21 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T22 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T23 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T24 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T25 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T26 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T27 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T30 = _mm256_madd_epi16(T04A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T31 = _mm256_madd_epi16(T04B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T32 = _mm256_madd_epi16(T05A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T33 = _mm256_madd_epi16(T05B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T34 = _mm256_madd_epi16(T06A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T35 = _mm256_madd_epi16(T06B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T36 = _mm256_madd_epi16(T07A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T37 = _mm256_madd_epi16(T07B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	\
	T40 = _mm256_hadd_epi32(T20, T21); \
	T41 = _mm256_hadd_epi32(T22, T23); \
	T42 = _mm256_hadd_epi32(T24, T25); \
	T43 = _mm256_hadd_epi32(T26, T27); \
	T44 = _mm256_hadd_epi32(T30, T31); \
	T45 = _mm256_hadd_epi32(T32, T33); \
	T46 = _mm256_hadd_epi32(T34, T35); \
	T47 = _mm256_hadd_epi32(T36, T37); \
	\
	T50 = _mm256_hadd_epi32(T40, T41); \
	T51 = _mm256_hadd_epi32(T42, T43); \
	T52 = _mm256_hadd_epi32(T44, T45); \
	T53 = _mm256_hadd_epi32(T46, T47); \
	\
	T50 = _mm256_hadd_epi32(T50, T51); \
	T51 = _mm256_hadd_epi32(T52, T53); \
	T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_4), shift1); \
	T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_4), shift1); \
	T60 = _mm256_packs_epi32(T50, T51); \
	im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);

#undef MAKE_ODD
	}

	// DCT2
	T00A = _mm256_permute2x128_si256(im[0][0], im[8][0], 0x20);
	T00B = _mm256_permute2x128_si256(im[0][0], im[8][0], 0x31);
	T01A = _mm256_permute2x128_si256(im[1][0], im[9][0], 0x20);
	T01B = _mm256_permute2x128_si256(im[1][0], im[9][0], 0x31);
	T02A = _mm256_permute2x128_si256(im[2][0], im[10][0], 0x20);
	T02B = _mm256_permute2x128_si256(im[2][0], im[10][0], 0x31);
	T03A = _mm256_permute2x128_si256(im[3][0], im[11][0], 0x20);
	T03B = _mm256_permute2x128_si256(im[3][0], im[11][0], 0x31);
	TT00A = _mm256_permute2x128_si256(im[4][0], im[12][0], 0x20);
	TT00B = _mm256_permute2x128_si256(im[4][0], im[12][0], 0x31);
	TT01A = _mm256_permute2x128_si256(im[5][0], im[13][0], 0x20);
	TT01B = _mm256_permute2x128_si256(im[5][0], im[13][0], 0x31);
	TT02A = _mm256_permute2x128_si256(im[6][0], im[14][0], 0x20);
	TT02B = _mm256_permute2x128_si256(im[6][0], im[14][0], 0x31);
	TT03A = _mm256_permute2x128_si256(im[7][0], im[15][0], 0x20);
	TT03B = _mm256_permute2x128_si256(im[7][0], im[15][0], 0x31);
	T00C = _mm256_permute2x128_si256(im[0][1], im[8][1], 0x20);
	T00D = _mm256_permute2x128_si256(im[0][1], im[8][1], 0x31);
	T01C = _mm256_permute2x128_si256(im[1][1], im[9][1], 0x20);
	T01D = _mm256_permute2x128_si256(im[1][1], im[9][1], 0x31);
	T02C = _mm256_permute2x128_si256(im[2][1], im[10][1], 0x20);
	T02D = _mm256_permute2x128_si256(im[2][1], im[10][1], 0x31);
	T03C = _mm256_permute2x128_si256(im[3][1], im[11][1], 0x20);
	T03D = _mm256_permute2x128_si256(im[3][1], im[11][1], 0x31);
	TT00C = _mm256_permute2x128_si256(im[4][1], im[12][1], 0x20);
	TT00D = _mm256_permute2x128_si256(im[4][1], im[12][1], 0x31);
	TT01C = _mm256_permute2x128_si256(im[5][1], im[13][1], 0x20);
	TT01D = _mm256_permute2x128_si256(im[5][1], im[13][1], 0x31);
	TT02C = _mm256_permute2x128_si256(im[6][1], im[14][1], 0x20);
	TT02D = _mm256_permute2x128_si256(im[6][1], im[14][1], 0x31);
	TT03C = _mm256_permute2x128_si256(im[7][1], im[15][1], 0x20);
	TT03D = _mm256_permute2x128_si256(im[7][1], im[15][1], 0x31);

	TAB16_0_0 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[0]);
	T00C = _mm256_shuffle_epi8(T00C, TAB16_0_0);    // [16 17 18 19 20 21 22 23]
	T00D = _mm256_shuffle_epi8(T00D, TAB16_0_0);    // [24 25 26 27 28 29 30 31]
	T01C = _mm256_shuffle_epi8(T01C, TAB16_0_0);
	T01D = _mm256_shuffle_epi8(T01D, TAB16_0_0);
	T02C = _mm256_shuffle_epi8(T02C, TAB16_0_0);
	T02D = _mm256_shuffle_epi8(T02D, TAB16_0_0);
	T03C = _mm256_shuffle_epi8(T03C, TAB16_0_0);
	T03D = _mm256_shuffle_epi8(T03D, TAB16_0_0);

	TT00C = _mm256_shuffle_epi8(TT00C, TAB16_0_0);
	TT00D = _mm256_shuffle_epi8(TT00D, TAB16_0_0);
	TT01C = _mm256_shuffle_epi8(TT01C, TAB16_0_0);
	TT01D = _mm256_shuffle_epi8(TT01D, TAB16_0_0);
	TT02C = _mm256_shuffle_epi8(TT02C, TAB16_0_0);
	TT02D = _mm256_shuffle_epi8(TT02D, TAB16_0_0);
	TT03C = _mm256_shuffle_epi8(TT03C, TAB16_0_0);
	TT03D = _mm256_shuffle_epi8(TT03D, TAB16_0_0);

	T10A = _mm256_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
	T10B = _mm256_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
	T00A = _mm256_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
	T00B = _mm256_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
	T11A = _mm256_unpacklo_epi16(T01A, T01D);
	T11B = _mm256_unpackhi_epi16(T01A, T01D);
	T01A = _mm256_unpacklo_epi16(T01B, T01C);
	T01B = _mm256_unpackhi_epi16(T01B, T01C);
	T12A = _mm256_unpacklo_epi16(T02A, T02D);
	T12B = _mm256_unpackhi_epi16(T02A, T02D);
	T02A = _mm256_unpacklo_epi16(T02B, T02C);
	T02B = _mm256_unpackhi_epi16(T02B, T02C);
	T13A = _mm256_unpacklo_epi16(T03A, T03D);
	T13B = _mm256_unpackhi_epi16(T03A, T03D);
	T03A = _mm256_unpacklo_epi16(T03B, T03C);
	T03B = _mm256_unpackhi_epi16(T03B, T03C);

	TT10A = _mm256_unpacklo_epi16(TT00A, TT00D);
	TT10B = _mm256_unpackhi_epi16(TT00A, TT00D);
	TT00A = _mm256_unpacklo_epi16(TT00B, TT00C);
	TT00B = _mm256_unpackhi_epi16(TT00B, TT00C);
	TT11A = _mm256_unpacklo_epi16(TT01A, TT01D);
	TT11B = _mm256_unpackhi_epi16(TT01A, TT01D);
	TT01A = _mm256_unpacklo_epi16(TT01B, TT01C);
	TT01B = _mm256_unpackhi_epi16(TT01B, TT01C);
	TT12A = _mm256_unpacklo_epi16(TT02A, TT02D);
	TT12B = _mm256_unpackhi_epi16(TT02A, TT02D);
	TT02A = _mm256_unpacklo_epi16(TT02B, TT02C);
	TT02B = _mm256_unpackhi_epi16(TT02B, TT02C);
	TT13A = _mm256_unpacklo_epi16(TT03A, TT03D);
	TT13B = _mm256_unpackhi_epi16(TT03A, TT03D);
	TT03A = _mm256_unpacklo_epi16(TT03B, TT03C);
	TT03B = _mm256_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
	T20 = _mm256_madd_epi16(T10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T21 = _mm256_madd_epi16(T10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T22 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T23 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T24 = _mm256_madd_epi16(T11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T25 = _mm256_madd_epi16(T11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T26 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T27 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T30 = _mm256_madd_epi16(T12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T31 = _mm256_madd_epi16(T12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T32 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T33 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T34 = _mm256_madd_epi16(T13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T35 = _mm256_madd_epi16(T13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T36 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T37 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	T60 = _mm256_hadd_epi32(T20, T21); \
	T61 = _mm256_hadd_epi32(T22, T23); \
	T62 = _mm256_hadd_epi32(T24, T25); \
	T63 = _mm256_hadd_epi32(T26, T27); \
	T64 = _mm256_hadd_epi32(T30, T31); \
	T65 = _mm256_hadd_epi32(T32, T33); \
	T66 = _mm256_hadd_epi32(T34, T35); \
	T67 = _mm256_hadd_epi32(T36, T37); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	T62 = _mm256_hadd_epi32(T64, T65); \
	T63 = _mm256_hadd_epi32(T66, T67); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	\
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_512), shift2); \
	\
	TT20 = _mm256_madd_epi16(TT10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT21 = _mm256_madd_epi16(TT10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT22 = _mm256_madd_epi16(TT00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT23 = _mm256_madd_epi16(TT00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT24 = _mm256_madd_epi16(TT11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT25 = _mm256_madd_epi16(TT11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT26 = _mm256_madd_epi16(TT01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT27 = _mm256_madd_epi16(TT01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT30 = _mm256_madd_epi16(TT12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT31 = _mm256_madd_epi16(TT12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT32 = _mm256_madd_epi16(TT02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT33 = _mm256_madd_epi16(TT02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT34 = _mm256_madd_epi16(TT13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT35 = _mm256_madd_epi16(TT13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT36 = _mm256_madd_epi16(TT03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT37 = _mm256_madd_epi16(TT03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	TT60 = _mm256_hadd_epi32(TT20, TT21); \
	TT61 = _mm256_hadd_epi32(TT22, TT23); \
	TT62 = _mm256_hadd_epi32(TT24, TT25); \
	TT63 = _mm256_hadd_epi32(TT26, TT27); \
	TT64 = _mm256_hadd_epi32(TT30, TT31); \
	TT65 = _mm256_hadd_epi32(TT32, TT33); \
	TT66 = _mm256_hadd_epi32(TT34, TT35); \
	TT67 = _mm256_hadd_epi32(TT36, TT37); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	TT62 = _mm256_hadd_epi32(TT64, TT65); \
	TT63 = _mm256_hadd_epi32(TT66, TT67); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	\
	TT60 = _mm256_srai_epi32(_mm256_add_epi32(TT60, c_512), shift2); \
	\
	tResult = _mm256_packs_epi32(T60, TT60); \
	_mm256_storeu_si256((__m256i*)&dst[(dstPos)* 32], tResult); \

	MAKE_ODD(44, 44, 44, 44, 0);
	MAKE_ODD(46, 47, 46, 47, 8);

	MAKE_ODD(50, 51, 52, 53, 4);
	MAKE_ODD(54, 55, 56, 57, 12);

	MAKE_ODD(66, 67, 68, 69, 2);
	MAKE_ODD(70, 71, 72, 73, 6);
	MAKE_ODD(74, 75, 76, 77, 10);
	MAKE_ODD(78, 79, 80, 81, 14);

	MAKE_ODD(98, 99, 100, 101, 1);
	MAKE_ODD(102, 103, 104, 105, 3);
	MAKE_ODD(106, 107, 108, 109, 5);
	MAKE_ODD(110, 111, 112, 113, 7);
	MAKE_ODD(114, 115, 116, 117, 9);
	MAKE_ODD(118, 119, 120, 121, 11);
	MAKE_ODD(122, 123, 124, 125, 13);
	MAKE_ODD(126, 127, 128, 129, 15);
#undef MAKE_ODD

	for (i = 0; i < 16; i++) {
		_mm256_storeu_si256((__m256i*)(dst + 16), tmpZero);
		dst += 32;
	}
	for (i = 16; i < 32; i++) {
		_mm256_storeu_si256((__m256i*)(dst), tmpZero);
		_mm256_storeu_si256((__m256i*)(dst + 16), tmpZero);
		dst += 32;
	}
}

void sub_trans_ext_64x64_sse256(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift){
	int i;
	resi_t resi[32 * 32];
	resi_t *temp_resi;
	__m256i T[64][4], V[64][4];
	__m256i O00, O01, O02, O03, O04, O05, O06, O07;
	__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A;
	__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B;
	__m256i P00, P01, P02, P03, P04, P05, P06, P07;
	__m256i P00A, P01A, P02A, P03A, P04A, P05A, P06A, P07A;
	__m256i P00B, P01B, P02B, P03B, P04B, P05B, P06B, P07B;
	__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
	__m256i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
	__m256i	t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
	__m256i T00A, T01A, T02A, T03A, T04A, T05A, T06A, T07A;
	__m256i T00B, T01B, T02B, T03B, T04B, T05B, T06B, T07B;
	__m256i T00C, T01C, T02C, T03C, T04C, T05C, T06C, T07C;
	__m256i T00D, T01D, T02D, T03D, T04D, T05D, T06D, T07D;
	__m256i T10A, T11A, T12A, T13A, T14A, T15A, T16A, T17A;
	__m256i T10B, T11B, T12B, T13B, T14B, T15B, T16B, T17B;
	__m256i T20, T21, T22, T23, T24, T25, T26, T27;
	__m256i T30, T31, T32, T33, T34, T35, T36, T37;
	__m256i T40, T41, T42, T43, T44, T45, T46, T47;
	__m256i T50, T51, T52, T53;
	__m256i T60, T61, T62, T63, T64, T65, T66, T67;
	__m256i im[16][4];
	__m256i TT00A, TT01A, TT02A, TT03A, TT04A, TT05A, TT06A, TT07A;
	__m256i TT00B, TT01B, TT02B, TT03B, TT04B, TT05B, TT06B, TT07B;
	__m256i TT00C, TT01C, TT02C, TT03C, TT04C, TT05C, TT06C, TT07C;
	__m256i TT00D, TT01D, TT02D, TT03D, TT04D, TT05D, TT06D, TT07D;
	__m256i TT10A, TT11A, TT12A, TT13A, TT14A, TT15A, TT16A, TT17A;
	__m256i TT10B, TT11B, TT12B, TT13B, TT14B, TT15B, TT16B, TT17B;
	__m256i TT20, TT21, TT22, TT23, TT24, TT25, TT26, TT27;
	__m256i TT30, TT31, TT32, TT33, TT34, TT35, TT36, TT37;
	__m256i TT40, TT41, TT42, TT43, TT44, TT45, TT46, TT47;
	__m256i TT50, TT51, TT52, TT53;
	__m256i TT60, TT61, TT62, TT63, TT64, TT65, TT66, TT67;
	__m256i tResult;

	__m256i TAB16_0_1, TAB32_0_0, TAB16_0_0;
	__m256i tmpZero = _mm256_setzero_si256();
	__m256i k1 = _mm256_set1_epi16(1);
	__m256i k2 = _mm256_set1_epi16(2);
	__m256i tt;
	__m256i c_8 = _mm256_set1_epi32(8);
	__m256i c_512 = _mm256_set1_epi32(512);
	int shift1 = 4;
	int shift2 = 10;

	//// pix sub & wavelet ////
	for (i = 0; i < 4; i++){
		//load data
		O00 = _mm256_loadu_si256((__m256i*)&org[0 * i_org]);	//8x32
		O01 = _mm256_loadu_si256((__m256i*)&org[1 * i_org]);
		O02 = _mm256_loadu_si256((__m256i*)&org[2 * i_org]);
		O03 = _mm256_loadu_si256((__m256i*)&org[3 * i_org]);
		O04 = _mm256_loadu_si256((__m256i*)&org[4 * i_org]);
		O05 = _mm256_loadu_si256((__m256i*)&org[5 * i_org]);
		O06 = _mm256_loadu_si256((__m256i*)&org[6 * i_org]);
		O07 = _mm256_loadu_si256((__m256i*)&org[7 * i_org]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[0 * i_pred]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[1 * i_pred]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[2 * i_pred]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[3 * i_pred]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[4 * i_pred]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[5 * i_pred]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[6 * i_pred]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[7 * i_pred]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216);
		O02 = _mm256_permute4x64_epi64(O02, 216);
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216);
		O05 = _mm256_permute4x64_epi64(O05, 216);
		O06 = _mm256_permute4x64_epi64(O06, 216);
		O07 = _mm256_permute4x64_epi64(O07, 216);
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);	//A B
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);	//C D
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);

		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 0][0] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 0][1] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 1][0] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 1][1] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 2][0] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 2][1] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 3][0] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 3][1] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 4][0] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 4][1] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 5][0] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 5][1] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 6][0] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 6][1] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 7][0] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 7][1] = _mm256_sub_epi16(O07B, P07B);

		O00 = _mm256_loadu_si256((__m256i*)&org[0 * i_org + 32]);
		O01 = _mm256_loadu_si256((__m256i*)&org[1 * i_org + 32]);
		O02 = _mm256_loadu_si256((__m256i*)&org[2 * i_org + 32]);
		O03 = _mm256_loadu_si256((__m256i*)&org[3 * i_org + 32]);
		O04 = _mm256_loadu_si256((__m256i*)&org[4 * i_org + 32]);
		O05 = _mm256_loadu_si256((__m256i*)&org[5 * i_org + 32]);
		O06 = _mm256_loadu_si256((__m256i*)&org[6 * i_org + 32]);
		O07 = _mm256_loadu_si256((__m256i*)&org[7 * i_org + 32]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[0 * i_pred + 32]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[1 * i_pred + 32]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[2 * i_pred + 32]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[3 * i_pred + 32]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[4 * i_pred + 32]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[5 * i_pred + 32]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[6 * i_pred + 32]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[7 * i_pred + 32]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216);
		O02 = _mm256_permute4x64_epi64(O02, 216);
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216);
		O05 = _mm256_permute4x64_epi64(O05, 216);
		O06 = _mm256_permute4x64_epi64(O06, 216);
		O07 = _mm256_permute4x64_epi64(O07, 216);
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);
		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 0][2] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 0][3] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 1][2] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 1][3] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 2][2] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 2][3] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 3][2] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 3][3] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 4][2] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 4][3] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 5][2] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 5][3] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 6][2] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 6][3] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 7][2] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 7][3] = _mm256_sub_epi16(O07B, P07B);

		O00 = _mm256_loadu_si256((__m256i*)&org[8 * i_org]);	//8x32
		O01 = _mm256_loadu_si256((__m256i*)&org[9 * i_org]);
		O02 = _mm256_loadu_si256((__m256i*)&org[10 * i_org]);
		O03 = _mm256_loadu_si256((__m256i*)&org[11 * i_org]);
		O04 = _mm256_loadu_si256((__m256i*)&org[12 * i_org]);
		O05 = _mm256_loadu_si256((__m256i*)&org[13 * i_org]);
		O06 = _mm256_loadu_si256((__m256i*)&org[14 * i_org]);
		O07 = _mm256_loadu_si256((__m256i*)&org[15 * i_org]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[8 * i_pred]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[9 * i_pred]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[10 * i_pred]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[11 * i_pred]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[12 * i_pred]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[13 * i_pred]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[14 * i_pred]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[15 * i_pred]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216);
		O02 = _mm256_permute4x64_epi64(O02, 216);
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216);
		O05 = _mm256_permute4x64_epi64(O05, 216);
		O06 = _mm256_permute4x64_epi64(O06, 216);
		O07 = _mm256_permute4x64_epi64(O07, 216);
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);	//A B
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);	//C D
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);

		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 8][0] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 8][1] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 9][0] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 9][1] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 10][0] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 10][1] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 11][0] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 11][1] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 12][0] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 12][1] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 13][0] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 13][1] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 14][0] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 14][1] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 15][0] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 15][1] = _mm256_sub_epi16(O07B, P07B);

		O00 = _mm256_loadu_si256((__m256i*)&org[8 * i_org + 32]);
		O01 = _mm256_loadu_si256((__m256i*)&org[9 * i_org + 32]);
		O02 = _mm256_loadu_si256((__m256i*)&org[10 * i_org + 32]);
		O03 = _mm256_loadu_si256((__m256i*)&org[11 * i_org + 32]);
		O04 = _mm256_loadu_si256((__m256i*)&org[12 * i_org + 32]);
		O05 = _mm256_loadu_si256((__m256i*)&org[13 * i_org + 32]);
		O06 = _mm256_loadu_si256((__m256i*)&org[14 * i_org + 32]);
		O07 = _mm256_loadu_si256((__m256i*)&org[15 * i_org + 32]);
		P00 = _mm256_loadu_si256((__m256i*)&pred[8 * i_pred + 32]);
		P01 = _mm256_loadu_si256((__m256i*)&pred[9 * i_pred + 32]);
		P02 = _mm256_loadu_si256((__m256i*)&pred[10 * i_pred + 32]);
		P03 = _mm256_loadu_si256((__m256i*)&pred[11 * i_pred + 32]);
		P04 = _mm256_loadu_si256((__m256i*)&pred[12 * i_pred + 32]);
		P05 = _mm256_loadu_si256((__m256i*)&pred[13 * i_pred + 32]);
		P06 = _mm256_loadu_si256((__m256i*)&pred[14 * i_pred + 32]);
		P07 = _mm256_loadu_si256((__m256i*)&pred[15 * i_pred + 32]);

		O00 = _mm256_permute4x64_epi64(O00, 216); //1101 1000
		O01 = _mm256_permute4x64_epi64(O01, 216);
		O02 = _mm256_permute4x64_epi64(O02, 216);
		O03 = _mm256_permute4x64_epi64(O03, 216);
		O04 = _mm256_permute4x64_epi64(O04, 216);
		O05 = _mm256_permute4x64_epi64(O05, 216);
		O06 = _mm256_permute4x64_epi64(O06, 216);
		O07 = _mm256_permute4x64_epi64(O07, 216);
		P00 = _mm256_permute4x64_epi64(P00, 216); //1101 1000
		P01 = _mm256_permute4x64_epi64(P01, 216);
		P02 = _mm256_permute4x64_epi64(P02, 216);
		P03 = _mm256_permute4x64_epi64(P03, 216);
		P04 = _mm256_permute4x64_epi64(P04, 216);
		P05 = _mm256_permute4x64_epi64(P05, 216);
		P06 = _mm256_permute4x64_epi64(P06, 216);
		P07 = _mm256_permute4x64_epi64(P07, 216);

		O00A = _mm256_unpacklo_epi8(O00, tmpZero);
		O00B = _mm256_unpackhi_epi8(O00, tmpZero);
		O01A = _mm256_unpacklo_epi8(O01, tmpZero);
		O01B = _mm256_unpackhi_epi8(O01, tmpZero);
		O02A = _mm256_unpacklo_epi8(O02, tmpZero);
		O02B = _mm256_unpackhi_epi8(O02, tmpZero);
		O03A = _mm256_unpacklo_epi8(O03, tmpZero);
		O03B = _mm256_unpackhi_epi8(O03, tmpZero);
		O04A = _mm256_unpacklo_epi8(O04, tmpZero);
		O04B = _mm256_unpackhi_epi8(O04, tmpZero);
		O05A = _mm256_unpacklo_epi8(O05, tmpZero);
		O05B = _mm256_unpackhi_epi8(O05, tmpZero);
		O06A = _mm256_unpacklo_epi8(O06, tmpZero);
		O06B = _mm256_unpackhi_epi8(O06, tmpZero);
		O07A = _mm256_unpacklo_epi8(O07, tmpZero);
		O07B = _mm256_unpackhi_epi8(O07, tmpZero);
		P00A = _mm256_unpacklo_epi8(P00, tmpZero);
		P00B = _mm256_unpackhi_epi8(P00, tmpZero);
		P01A = _mm256_unpacklo_epi8(P01, tmpZero);
		P01B = _mm256_unpackhi_epi8(P01, tmpZero);
		P02A = _mm256_unpacklo_epi8(P02, tmpZero);
		P02B = _mm256_unpackhi_epi8(P02, tmpZero);
		P03A = _mm256_unpacklo_epi8(P03, tmpZero);
		P03B = _mm256_unpackhi_epi8(P03, tmpZero);
		P04A = _mm256_unpacklo_epi8(P04, tmpZero);
		P04B = _mm256_unpackhi_epi8(P04, tmpZero);
		P05A = _mm256_unpacklo_epi8(P05, tmpZero);
		P05B = _mm256_unpackhi_epi8(P05, tmpZero);
		P06A = _mm256_unpacklo_epi8(P06, tmpZero);
		P06B = _mm256_unpackhi_epi8(P06, tmpZero);
		P07A = _mm256_unpacklo_epi8(P07, tmpZero);
		P07B = _mm256_unpackhi_epi8(P07, tmpZero);

		T[i * 16 + 8][2] = _mm256_sub_epi16(O00A, P00A);
		T[i * 16 + 8][3] = _mm256_sub_epi16(O00B, P00B);
		T[i * 16 + 9][2] = _mm256_sub_epi16(O01A, P01A);
		T[i * 16 + 9][3] = _mm256_sub_epi16(O01B, P01B);
		T[i * 16 + 10][2] = _mm256_sub_epi16(O02A, P02A);
		T[i * 16 + 10][3] = _mm256_sub_epi16(O02B, P02B);
		T[i * 16 + 11][2] = _mm256_sub_epi16(O03A, P03A);
		T[i * 16 + 11][3] = _mm256_sub_epi16(O03B, P03B);
		T[i * 16 + 12][2] = _mm256_sub_epi16(O04A, P04A);
		T[i * 16 + 12][3] = _mm256_sub_epi16(O04B, P04B);
		T[i * 16 + 13][2] = _mm256_sub_epi16(O05A, P05A);
		T[i * 16 + 13][3] = _mm256_sub_epi16(O05B, P05B);
		T[i * 16 + 14][2] = _mm256_sub_epi16(O06A, P06A);
		T[i * 16 + 14][3] = _mm256_sub_epi16(O06B, P06B);
		T[i * 16 + 15][2] = _mm256_sub_epi16(O07A, P07A);
		T[i * 16 + 15][3] = _mm256_sub_epi16(O07B, P07B);

		org += i_org << 4;
		pred += i_pred << 4;

		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][0], T[i * 16 + 1][0], T[i * 16 + 2][0], T[i * 16 + 3][0], T[i * 16 + 4][0], T[i * 16 + 5][0], T[i * 16 + 6][0], T[i * 16 + 7][0], T[i * 16 + 8][0], T[i * 16 + 9][0], T[i * 16 + 10][0], T[i * 16 + 11][0], T[i * 16 + 12][0], T[i * 16 + 13][0], T[i * 16 + 14][0], T[i * 16 + 15][0], V[0][i], V[1][i], V[2][i], V[3][i], V[4][i], V[5][i], V[6][i], V[7][i], V[8][i], V[9][i], V[10][i], V[11][i], V[12][i], V[13][i], V[14][i], V[15][i]);
		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][1], T[i * 16 + 1][1], T[i * 16 + 2][1], T[i * 16 + 3][1], T[i * 16 + 4][1], T[i * 16 + 5][1], T[i * 16 + 6][1], T[i * 16 + 7][1], T[i * 16 + 8][1], T[i * 16 + 9][1], T[i * 16 + 10][1], T[i * 16 + 11][1], T[i * 16 + 12][1], T[i * 16 + 13][1], T[i * 16 + 14][1], T[i * 16 + 15][1], V[16][i], V[17][i], V[18][i], V[19][i], V[20][i], V[21][i], V[22][i], V[23][i], V[24][i], V[25][i], V[26][i], V[27][i], V[28][i], V[29][i], V[30][i], V[31][i]);
		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][2], T[i * 16 + 1][2], T[i * 16 + 2][2], T[i * 16 + 3][2], T[i * 16 + 4][2], T[i * 16 + 5][2], T[i * 16 + 6][2], T[i * 16 + 7][2], T[i * 16 + 8][2], T[i * 16 + 9][2], T[i * 16 + 10][2], T[i * 16 + 11][2], T[i * 16 + 12][2], T[i * 16 + 13][2], T[i * 16 + 14][2], T[i * 16 + 15][2], V[32][i], V[33][i], V[34][i], V[35][i], V[36][i], V[37][i], V[38][i], V[39][i], V[40][i], V[41][i], V[42][i], V[43][i], V[44][i], V[45][i], V[46][i], V[47][i]);
		TRANSPOSE_16x16_16BIT_m256i(T[i * 16 + 0][3], T[i * 16 + 1][3], T[i * 16 + 2][3], T[i * 16 + 3][3], T[i * 16 + 4][3], T[i * 16 + 5][3], T[i * 16 + 6][3], T[i * 16 + 7][3], T[i * 16 + 8][3], T[i * 16 + 9][3], T[i * 16 + 10][3], T[i * 16 + 11][3], T[i * 16 + 12][3], T[i * 16 + 13][3], T[i * 16 + 14][3], T[i * 16 + 15][3], V[48][i], V[49][i], V[50][i], V[51][i], V[52][i], V[53][i], V[54][i], V[55][i], V[56][i], V[57][i], V[58][i], V[59][i], V[60][i], V[61][i], V[62][i], V[63][i]);

		//filter H
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[0][i], V[2][i]), 1);
		V[1][i] = _mm256_sub_epi16(V[1][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[2][i], V[4][i]), 1);
		V[3][i] = _mm256_sub_epi16(V[3][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[4][i], V[6][i]), 1);
		V[5][i] = _mm256_sub_epi16(V[5][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[6][i], V[8][i]), 1);
		V[7][i] = _mm256_sub_epi16(V[7][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[8][i], V[10][i]), 1);
		V[9][i] = _mm256_sub_epi16(V[9][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[10][i], V[12][i]), 1);
		V[11][i] = _mm256_sub_epi16(V[11][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[12][i], V[14][i]), 1);
		V[13][i] = _mm256_sub_epi16(V[13][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[14][i], V[16][i]), 1);
		V[15][i] = _mm256_sub_epi16(V[15][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[16][i], V[18][i]), 1);
		V[17][i] = _mm256_sub_epi16(V[17][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[18][i], V[20][i]), 1);
		V[19][i] = _mm256_sub_epi16(V[19][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[20][i], V[22][i]), 1);
		V[21][i] = _mm256_sub_epi16(V[21][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[22][i], V[24][i]), 1);
		V[23][i] = _mm256_sub_epi16(V[23][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[24][i], V[26][i]), 1);
		V[25][i] = _mm256_sub_epi16(V[25][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[26][i], V[28][i]), 1);
		V[27][i] = _mm256_sub_epi16(V[27][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[28][i], V[30][i]), 1);
		V[29][i] = _mm256_sub_epi16(V[29][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[30][i], V[32][i]), 1);
		V[31][i] = _mm256_sub_epi16(V[31][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[32][i], V[34][i]), 1);
		V[33][i] = _mm256_sub_epi16(V[33][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[34][i], V[36][i]), 1);
		V[35][i] = _mm256_sub_epi16(V[35][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[36][i], V[38][i]), 1);
		V[37][i] = _mm256_sub_epi16(V[37][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[38][i], V[40][i]), 1);
		V[39][i] = _mm256_sub_epi16(V[39][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[40][i], V[42][i]), 1);
		V[41][i] = _mm256_sub_epi16(V[41][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[42][i], V[44][i]), 1);
		V[43][i] = _mm256_sub_epi16(V[43][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[44][i], V[46][i]), 1);
		V[45][i] = _mm256_sub_epi16(V[45][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[46][i], V[48][i]), 1);
		V[47][i] = _mm256_sub_epi16(V[47][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[48][i], V[50][i]), 1);
		V[49][i] = _mm256_sub_epi16(V[49][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[50][i], V[52][i]), 1);
		V[51][i] = _mm256_sub_epi16(V[51][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[52][i], V[54][i]), 1);
		V[53][i] = _mm256_sub_epi16(V[53][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[54][i], V[56][i]), 1);
		V[55][i] = _mm256_sub_epi16(V[55][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[56][i], V[58][i]), 1);
		V[57][i] = _mm256_sub_epi16(V[57][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[58][i], V[60][i]), 1);
		V[59][i] = _mm256_sub_epi16(V[59][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[60][i], V[62][i]), 1);
		V[61][i] = _mm256_sub_epi16(V[61][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(V[62][i], V[62][i]), 1);
		V[63][i] = _mm256_sub_epi16(V[63][i], tt);

		//filter L
		tt = _mm256_add_epi16(_mm256_add_epi16(V[1][i], V[1][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[0][i] = _mm256_add_epi16(V[0][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[1][i], V[3][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[2][i] = _mm256_add_epi16(V[2][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[3][i], V[5][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[4][i] = _mm256_add_epi16(V[4][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[5][i], V[7][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[6][i] = _mm256_add_epi16(V[6][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[7][i], V[9][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[8][i] = _mm256_add_epi16(V[8][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[9][i], V[11][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[10][i] = _mm256_add_epi16(V[10][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[11][i], V[13][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[12][i] = _mm256_add_epi16(V[12][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[13][i], V[15][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[14][i] = _mm256_add_epi16(V[14][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[15][i], V[17][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[16][i] = _mm256_add_epi16(V[16][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[17][i], V[19][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[18][i] = _mm256_add_epi16(V[18][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[19][i], V[21][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[20][i] = _mm256_add_epi16(V[20][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[21][i], V[23][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[22][i] = _mm256_add_epi16(V[22][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[23][i], V[25][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[24][i] = _mm256_add_epi16(V[24][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[25][i], V[27][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[26][i] = _mm256_add_epi16(V[26][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[27][i], V[29][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[28][i] = _mm256_add_epi16(V[28][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[29][i], V[31][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[30][i] = _mm256_add_epi16(V[30][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[31][i], V[33][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[32][i] = _mm256_add_epi16(V[32][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[33][i], V[35][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[34][i] = _mm256_add_epi16(V[34][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[35][i], V[37][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[36][i] = _mm256_add_epi16(V[36][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[37][i], V[39][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[38][i] = _mm256_add_epi16(V[38][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[39][i], V[41][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[40][i] = _mm256_add_epi16(V[40][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[41][i], V[43][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[42][i] = _mm256_add_epi16(V[42][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[43][i], V[45][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[44][i] = _mm256_add_epi16(V[44][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[45][i], V[47][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[46][i] = _mm256_add_epi16(V[46][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[47][i], V[49][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[48][i] = _mm256_add_epi16(V[48][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[49][i], V[51][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[50][i] = _mm256_add_epi16(V[50][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[51][i], V[53][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[52][i] = _mm256_add_epi16(V[52][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[53][i], V[55][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[54][i] = _mm256_add_epi16(V[54][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[55][i], V[57][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[56][i] = _mm256_add_epi16(V[56][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[57][i], V[59][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[58][i] = _mm256_add_epi16(V[58][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[59][i], V[61][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[60][i] = _mm256_add_epi16(V[60][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(V[61][i], V[63][i]), k2);
		tt = _mm256_srai_epi16(tt, 2);
		V[62][i] = _mm256_add_epi16(V[62][i], tt);

		TRANSPOSE_16x16_16BIT_m256i(V[0][i], V[2][i], V[4][i], V[6][i], V[8][i], V[10][i], V[12][i], V[14][i], V[16][i], V[18][i], V[20][i], V[22][i], V[24][i], V[26][i], V[28][i], V[30][i], T[i * 16 + 0][0], T[i * 16 + 1][0], T[i * 16 + 2][0], T[i * 16 + 3][0], T[i * 16 + 4][0], T[i * 16 + 5][0], T[i * 16 + 6][0], T[i * 16 + 7][0], T[i * 16 + 8][0], T[i * 16 + 9][0], T[i * 16 + 10][0], T[i * 16 + 11][0], T[i * 16 + 12][0], T[i * 16 + 13][0], T[i * 16 + 14][0], T[i * 16 + 15][0]);
		TRANSPOSE_16x16_16BIT_m256i(V[32][i], V[34][i], V[36][i], V[38][i], V[40][i], V[42][i], V[44][i], V[46][i], V[48][i], V[50][i], V[52][i], V[54][i], V[56][i], V[58][i], V[60][i], V[62][i], T[i * 16 + 0][1], T[i * 16 + 1][1], T[i * 16 + 2][1], T[i * 16 + 3][1], T[i * 16 + 4][1], T[i * 16 + 5][1], T[i * 16 + 6][1], T[i * 16 + 7][1], T[i * 16 + 8][1], T[i * 16 + 9][1], T[i * 16 + 10][1], T[i * 16 + 11][1], T[i * 16 + 12][1], T[i * 16 + 13][1], T[i * 16 + 14][1], T[i * 16 + 15][1]);
	}

	for (i = 0; i < 2; i++){
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[0][i], T[2][i]), 1);
		T[1][i] = _mm256_sub_epi16(T[1][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[2][i], T[4][i]), 1);
		T[3][i] = _mm256_sub_epi16(T[3][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[4][i], T[6][i]), 1);
		T[5][i] = _mm256_sub_epi16(T[5][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[6][i], T[8][i]), 1);
		T[7][i] = _mm256_sub_epi16(T[7][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[8][i], T[10][i]), 1);
		T[9][i] = _mm256_sub_epi16(T[9][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[10][i], T[12][i]), 1);
		T[11][i] = _mm256_sub_epi16(T[11][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[12][i], T[14][i]), 1);
		T[13][i] = _mm256_sub_epi16(T[13][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[14][i], T[16][i]), 1);
		T[15][i] = _mm256_sub_epi16(T[15][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[16][i], T[18][i]), 1);
		T[17][i] = _mm256_sub_epi16(T[17][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[18][i], T[20][i]), 1);
		T[19][i] = _mm256_sub_epi16(T[19][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[20][i], T[22][i]), 1);
		T[21][i] = _mm256_sub_epi16(T[21][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[22][i], T[24][i]), 1);
		T[23][i] = _mm256_sub_epi16(T[23][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[24][i], T[26][i]), 1);
		T[25][i] = _mm256_sub_epi16(T[25][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[26][i], T[28][i]), 1);
		T[27][i] = _mm256_sub_epi16(T[27][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[28][i], T[30][i]), 1);
		T[29][i] = _mm256_sub_epi16(T[29][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[30][i], T[32][i]), 1);
		T[31][i] = _mm256_sub_epi16(T[31][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[32][i], T[34][i]), 1);
		T[33][i] = _mm256_sub_epi16(T[33][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[34][i], T[36][i]), 1);
		T[35][i] = _mm256_sub_epi16(T[35][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[36][i], T[38][i]), 1);
		T[37][i] = _mm256_sub_epi16(T[37][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[38][i], T[40][i]), 1);
		T[39][i] = _mm256_sub_epi16(T[39][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[40][i], T[42][i]), 1);
		T[41][i] = _mm256_sub_epi16(T[41][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[42][i], T[44][i]), 1);
		T[43][i] = _mm256_sub_epi16(T[43][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[44][i], T[46][i]), 1);
		T[45][i] = _mm256_sub_epi16(T[45][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[46][i], T[48][i]), 1);
		T[47][i] = _mm256_sub_epi16(T[47][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[48][i], T[50][i]), 1);
		T[49][i] = _mm256_sub_epi16(T[49][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[50][i], T[52][i]), 1);
		T[51][i] = _mm256_sub_epi16(T[51][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[52][i], T[54][i]), 1);
		T[53][i] = _mm256_sub_epi16(T[53][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[54][i], T[56][i]), 1);
		T[55][i] = _mm256_sub_epi16(T[55][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[56][i], T[58][i]), 1);
		T[57][i] = _mm256_sub_epi16(T[57][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[58][i], T[60][i]), 1);
		T[59][i] = _mm256_sub_epi16(T[59][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[60][i], T[62][i]), 1);
		T[61][i] = _mm256_sub_epi16(T[61][i], tt);
		tt = _mm256_srai_epi16(_mm256_add_epi16(T[62][i], T[62][i]), 1);
		T[63][i] = _mm256_sub_epi16(T[63][i], tt);

		tt = _mm256_add_epi16(_mm256_add_epi16(T[1][i], T[1][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[0][i] = _mm256_slli_epi16(T[0][i], 1);
		T[0][i] = _mm256_add_epi16(T[0][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[1][i], T[3][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[2][i] = _mm256_slli_epi16(T[2][i], 1);
		T[2][i] = _mm256_add_epi16(T[2][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[3][i], T[5][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[4][i] = _mm256_slli_epi16(T[4][i], 1);
		T[4][i] = _mm256_add_epi16(T[4][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[5][i], T[7][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[6][i] = _mm256_slli_epi16(T[6][i], 1);
		T[6][i] = _mm256_add_epi16(T[6][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[7][i], T[9][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[8][i] = _mm256_slli_epi16(T[8][i], 1);
		T[8][i] = _mm256_add_epi16(T[8][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[9][i], T[11][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[10][i] = _mm256_slli_epi16(T[10][i], 1);
		T[10][i] = _mm256_add_epi16(T[10][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[11][i], T[13][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[12][i] = _mm256_slli_epi16(T[12][i], 1);
		T[12][i] = _mm256_add_epi16(T[12][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[13][i], T[15][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[14][i] = _mm256_slli_epi16(T[14][i], 1);
		T[14][i] = _mm256_add_epi16(T[14][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[15][i], T[17][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[16][i] = _mm256_slli_epi16(T[16][i], 1);
		T[16][i] = _mm256_add_epi16(T[16][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[17][i], T[19][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[18][i] = _mm256_slli_epi16(T[18][i], 1);
		T[18][i] = _mm256_add_epi16(T[18][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[19][i], T[21][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[20][i] = _mm256_slli_epi16(T[20][i], 1);
		T[20][i] = _mm256_add_epi16(T[20][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[21][i], T[23][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[22][i] = _mm256_slli_epi16(T[22][i], 1);
		T[22][i] = _mm256_add_epi16(T[22][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[23][i], T[25][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[24][i] = _mm256_slli_epi16(T[24][i], 1);
		T[24][i] = _mm256_add_epi16(T[24][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[25][i], T[27][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[26][i] = _mm256_slli_epi16(T[26][i], 1);
		T[26][i] = _mm256_add_epi16(T[26][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[27][i], T[29][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[28][i] = _mm256_slli_epi16(T[28][i], 1);
		T[28][i] = _mm256_add_epi16(T[28][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[29][i], T[31][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[30][i] = _mm256_slli_epi16(T[30][i], 1);
		T[30][i] = _mm256_add_epi16(T[30][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[31][i], T[33][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[32][i] = _mm256_slli_epi16(T[32][i], 1);
		T[32][i] = _mm256_add_epi16(T[32][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[33][i], T[35][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[34][i] = _mm256_slli_epi16(T[34][i], 1);
		T[34][i] = _mm256_add_epi16(T[34][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[35][i], T[37][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[36][i] = _mm256_slli_epi16(T[36][i], 1);
		T[36][i] = _mm256_add_epi16(T[36][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[37][i], T[39][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[38][i] = _mm256_slli_epi16(T[38][i], 1);
		T[38][i] = _mm256_add_epi16(T[38][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[39][i], T[41][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[40][i] = _mm256_slli_epi16(T[40][i], 1);
		T[40][i] = _mm256_add_epi16(T[40][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[41][i], T[43][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[42][i] = _mm256_slli_epi16(T[42][i], 1);
		T[42][i] = _mm256_add_epi16(T[42][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[43][i], T[45][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[44][i] = _mm256_slli_epi16(T[44][i], 1);
		T[44][i] = _mm256_add_epi16(T[44][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[45][i], T[47][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[46][i] = _mm256_slli_epi16(T[46][i], 1);
		T[46][i] = _mm256_add_epi16(T[46][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[47][i], T[49][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[48][i] = _mm256_slli_epi16(T[48][i], 1);
		T[48][i] = _mm256_add_epi16(T[48][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[49][i], T[51][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[50][i] = _mm256_slli_epi16(T[50][i], 1);
		T[50][i] = _mm256_add_epi16(T[50][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[51][i], T[53][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[52][i] = _mm256_slli_epi16(T[52][i], 1);
		T[52][i] = _mm256_add_epi16(T[52][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[53][i], T[55][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[54][i] = _mm256_slli_epi16(T[54][i], 1);
		T[54][i] = _mm256_add_epi16(T[54][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[55][i], T[57][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[56][i] = _mm256_slli_epi16(T[56][i], 1);
		T[56][i] = _mm256_add_epi16(T[56][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[57][i], T[59][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[58][i] = _mm256_slli_epi16(T[58][i], 1);
		T[58][i] = _mm256_add_epi16(T[58][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[59][i], T[61][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[60][i] = _mm256_slli_epi16(T[60][i], 1);
		T[60][i] = _mm256_add_epi16(T[60][i], tt);
		tt = _mm256_add_epi16(_mm256_add_epi16(T[61][i], T[63][i]), k1);
		tt = _mm256_srai_epi16(tt, 1);
		T[62][i] = _mm256_slli_epi16(T[62][i], 1);
		T[62][i] = _mm256_add_epi16(T[62][i], tt);
	}

	///// DCT /////

	// DCT1
	for (i = 0; i < 2; i++)
	{
		T00A = _mm256_permute2x128_si256(T[0 + 32 * i][0], T[16 + 32 * i][0], 0x20);
		T00B = _mm256_permute2x128_si256(T[0 + 32 * i][0], T[16 + 32 * i][0], 0x31);
		T01A = _mm256_permute2x128_si256(T[2 + 32 * i][0], T[18 + 32 * i][0], 0x20);
		T01B = _mm256_permute2x128_si256(T[2 + 32 * i][0], T[18 + 32 * i][0], 0x31);
		T02A = _mm256_permute2x128_si256(T[4 + 32 * i][0], T[20 + 32 * i][0], 0x20);
		T02B = _mm256_permute2x128_si256(T[4 + 32 * i][0], T[20 + 32 * i][0], 0x31);
		T03A = _mm256_permute2x128_si256(T[6 + 32 * i][0], T[22 + 32 * i][0], 0x20);
		T03B = _mm256_permute2x128_si256(T[6 + 32 * i][0], T[22 + 32 * i][0], 0x31);
		T04A = _mm256_permute2x128_si256(T[8 + 32 * i][0], T[24 + 32 * i][0], 0x20);
		T04B = _mm256_permute2x128_si256(T[8 + 32 * i][0], T[24 + 32 * i][0], 0x31);
		T05A = _mm256_permute2x128_si256(T[10 + 32 * i][0], T[26 + 32 * i][0], 0x20);
		T05B = _mm256_permute2x128_si256(T[10 + 32 * i][0], T[26 + 32 * i][0], 0x31);
		T06A = _mm256_permute2x128_si256(T[12 + 32 * i][0], T[28 + 32 * i][0], 0x20);
		T06B = _mm256_permute2x128_si256(T[12 + 32 * i][0], T[28 + 32 * i][0], 0x31);
		T07A = _mm256_permute2x128_si256(T[14 + 32 * i][0], T[30 + 32 * i][0], 0x20);
		T07B = _mm256_permute2x128_si256(T[14 + 32 * i][0], T[30 + 32 * i][0], 0x31);

		T00C = _mm256_permute2x128_si256(T[0 + 32 * i][1], T[16 + 32 * i][1], 0x20);
		T00D = _mm256_permute2x128_si256(T[0 + 32 * i][1], T[16 + 32 * i][1], 0x31);
		T01C = _mm256_permute2x128_si256(T[2 + 32 * i][1], T[18 + 32 * i][1], 0x20);
		T01D = _mm256_permute2x128_si256(T[2 + 32 * i][1], T[18 + 32 * i][1], 0x31);
		T02C = _mm256_permute2x128_si256(T[4 + 32 * i][1], T[20 + 32 * i][1], 0x20);
		T02D = _mm256_permute2x128_si256(T[4 + 32 * i][1], T[20 + 32 * i][1], 0x31);
		T03C = _mm256_permute2x128_si256(T[6 + 32 * i][1], T[22 + 32 * i][1], 0x20);
		T03D = _mm256_permute2x128_si256(T[6 + 32 * i][1], T[22 + 32 * i][1], 0x31);
		T04C = _mm256_permute2x128_si256(T[8 + 32 * i][1], T[24 + 32 * i][1], 0x20);
		T04D = _mm256_permute2x128_si256(T[8 + 32 * i][1], T[24 + 32 * i][1], 0x31);
		T05C = _mm256_permute2x128_si256(T[10 + 32 * i][1], T[26 + 32 * i][1], 0x20);
		T05D = _mm256_permute2x128_si256(T[10 + 32 * i][1], T[26 + 32 * i][1], 0x31);
		T06C = _mm256_permute2x128_si256(T[12 + 32 * i][1], T[28 + 32 * i][1], 0x20);
		T06D = _mm256_permute2x128_si256(T[12 + 32 * i][1], T[28 + 32 * i][1], 0x31);
		T07C = _mm256_permute2x128_si256(T[14 + 32 * i][1], T[30 + 32 * i][1], 0x20);
		T07D = _mm256_permute2x128_si256(T[14 + 32 * i][1], T[30 + 32 * i][1], 0x31);

		org += i_org << 4;
		pred += i_pred << 4;

		//
		TAB16_0_1 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[1]);
		TAB32_0_0 = _mm256_load_si256((__m256i*)tab_dct_32_0_256i[0]);
		T00A = _mm256_shuffle_epi8(T00A, TAB16_0_1);    // [05 02 06 01 04 03 07 00]
		T00B = _mm256_shuffle_epi8(T00B, TAB32_0_0);    // [10 13 09 14 11 12 08 15]
		T00C = _mm256_shuffle_epi8(T00C, TAB16_0_1);    // [21 18 22 17 20 19 23 16]
		T00D = _mm256_shuffle_epi8(T00D, TAB32_0_0);    // [26 29 25 30 27 28 24 31]
		T01A = _mm256_shuffle_epi8(T01A, TAB16_0_1);
		T01B = _mm256_shuffle_epi8(T01B, TAB32_0_0);
		T01C = _mm256_shuffle_epi8(T01C, TAB16_0_1);
		T01D = _mm256_shuffle_epi8(T01D, TAB32_0_0);
		T02A = _mm256_shuffle_epi8(T02A, TAB16_0_1);
		T02B = _mm256_shuffle_epi8(T02B, TAB32_0_0);
		T02C = _mm256_shuffle_epi8(T02C, TAB16_0_1);
		T02D = _mm256_shuffle_epi8(T02D, TAB32_0_0);
		T03A = _mm256_shuffle_epi8(T03A, TAB16_0_1);
		T03B = _mm256_shuffle_epi8(T03B, TAB32_0_0);
		T03C = _mm256_shuffle_epi8(T03C, TAB16_0_1);
		T03D = _mm256_shuffle_epi8(T03D, TAB32_0_0);
		T04A = _mm256_shuffle_epi8(T04A, TAB16_0_1);
		T04B = _mm256_shuffle_epi8(T04B, TAB32_0_0);
		T04C = _mm256_shuffle_epi8(T04C, TAB16_0_1);
		T04D = _mm256_shuffle_epi8(T04D, TAB32_0_0);
		T05A = _mm256_shuffle_epi8(T05A, TAB16_0_1);
		T05B = _mm256_shuffle_epi8(T05B, TAB32_0_0);
		T05C = _mm256_shuffle_epi8(T05C, TAB16_0_1);
		T05D = _mm256_shuffle_epi8(T05D, TAB32_0_0);
		T06A = _mm256_shuffle_epi8(T06A, TAB16_0_1);
		T06B = _mm256_shuffle_epi8(T06B, TAB32_0_0);
		T06C = _mm256_shuffle_epi8(T06C, TAB16_0_1);
		T06D = _mm256_shuffle_epi8(T06D, TAB32_0_0);
		T07A = _mm256_shuffle_epi8(T07A, TAB16_0_1);
		T07B = _mm256_shuffle_epi8(T07B, TAB32_0_0);
		T07C = _mm256_shuffle_epi8(T07C, TAB16_0_1);
		T07D = _mm256_shuffle_epi8(T07D, TAB32_0_0);

		T10A = _mm256_add_epi16(T00A, T00D);   // [E05 E02 E06 E01 E04 E03 E07 E00]
		T10B = _mm256_add_epi16(T00B, T00C);   // [E10 E13 E09 E14 E11 E12 E08 E15]
		T11A = _mm256_add_epi16(T01A, T01D);
		T11B = _mm256_add_epi16(T01B, T01C);
		T12A = _mm256_add_epi16(T02A, T02D);
		T12B = _mm256_add_epi16(T02B, T02C);
		T13A = _mm256_add_epi16(T03A, T03D);
		T13B = _mm256_add_epi16(T03B, T03C);
		T14A = _mm256_add_epi16(T04A, T04D);
		T14B = _mm256_add_epi16(T04B, T04C);
		T15A = _mm256_add_epi16(T05A, T05D);
		T15B = _mm256_add_epi16(T05B, T05C);
		T16A = _mm256_add_epi16(T06A, T06D);
		T16B = _mm256_add_epi16(T06B, T06C);
		T17A = _mm256_add_epi16(T07A, T07D);
		T17B = _mm256_add_epi16(T07B, T07C);

		T00A = _mm256_sub_epi16(T00A, T00D);   // [O05 O02 O06 O01 O04 O03 O07 O00]
		T00B = _mm256_sub_epi16(T00B, T00C);   // [O10 O13 O09 O14 O11 O12 O08 O15]
		T01A = _mm256_sub_epi16(T01A, T01D);
		T01B = _mm256_sub_epi16(T01B, T01C);
		T02A = _mm256_sub_epi16(T02A, T02D);
		T02B = _mm256_sub_epi16(T02B, T02C);
		T03A = _mm256_sub_epi16(T03A, T03D);
		T03B = _mm256_sub_epi16(T03B, T03C);
		T04A = _mm256_sub_epi16(T04A, T04D);
		T04B = _mm256_sub_epi16(T04B, T04C);
		T05A = _mm256_sub_epi16(T05A, T05D);
		T05B = _mm256_sub_epi16(T05B, T05C);
		T06A = _mm256_sub_epi16(T06A, T06D);
		T06B = _mm256_sub_epi16(T06B, T06C);
		T07A = _mm256_sub_epi16(T07A, T07D);
		T07B = _mm256_sub_epi16(T07B, T07C);

		T20 = _mm256_add_epi16(T10A, T10B);   // [EE5 EE2 EE6 EE1 EE4 EE3 EE7 EE0]
		T21 = _mm256_add_epi16(T11A, T11B);
		T22 = _mm256_add_epi16(T12A, T12B);
		T23 = _mm256_add_epi16(T13A, T13B);
		T24 = _mm256_add_epi16(T14A, T14B);
		T25 = _mm256_add_epi16(T15A, T15B);
		T26 = _mm256_add_epi16(T16A, T16B);
		T27 = _mm256_add_epi16(T17A, T17B);

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_8_256i[1]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
		im[0][i] = T60;

		T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));
		T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_16_1_256i[8]));

		T40 = _mm256_hadd_epi32(T30, T31);
		T41 = _mm256_hadd_epi32(T32, T33);
		T42 = _mm256_hadd_epi32(T34, T35);
		T43 = _mm256_hadd_epi32(T36, T37);

		T50 = _mm256_hadd_epi32(T40, T41);
		T51 = _mm256_hadd_epi32(T42, T43);
		T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1);
		T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1);
		T60 = _mm256_packs_epi32(T50, T51);
        im[8][i] = T60;

#define MAKE_ODD(tab, dstPos) { \
    T30 = _mm256_madd_epi16(T20, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T31 = _mm256_madd_epi16(T21, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T32 = _mm256_madd_epi16(T22, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T33 = _mm256_madd_epi16(T23, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T34 = _mm256_madd_epi16(T24, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T35 = _mm256_madd_epi16(T25, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T36 = _mm256_madd_epi16(T26, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T37 = _mm256_madd_epi16(T27, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
    T40 = _mm256_hadd_epi32(T30, T31); \
    T41 = _mm256_hadd_epi32(T32, T33); \
    T42 = _mm256_hadd_epi32(T34, T35); \
    T43 = _mm256_hadd_epi32(T36, T37); \
    T50 = _mm256_hadd_epi32(T40, T41); \
    T51 = _mm256_hadd_epi32(T42, T43); \
    T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1); \
    T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1); \
    T60 = _mm256_packs_epi32(T50, T51); \
    im[(dstPos)][i] = T60; \
}

		MAKE_ODD(0, 4);
		MAKE_ODD(1, 12);

		T20 = _mm256_sub_epi16(T10A, T10B);   // [EO5 EO2 EO6 EO1 EO4 EO3 EO7 EO0]
		T21 = _mm256_sub_epi16(T11A, T11B);
		T22 = _mm256_sub_epi16(T12A, T12B);
		T23 = _mm256_sub_epi16(T13A, T13B);
		T24 = _mm256_sub_epi16(T14A, T14B);
		T25 = _mm256_sub_epi16(T15A, T15B);
		T26 = _mm256_sub_epi16(T16A, T16B);
		T27 = _mm256_sub_epi16(T17A, T17B);

		MAKE_ODD(4, 2);
		MAKE_ODD(5, 6);
		MAKE_ODD(6, 10);
		MAKE_ODD(7, 14);

#undef MAKE_ODD

#define MAKE_ODD(tab, dstPos) \
	T20 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T21 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T22 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T23 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T24 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T25 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T26 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T27 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T30 = _mm256_madd_epi16(T04A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T31 = _mm256_madd_epi16(T04B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T32 = _mm256_madd_epi16(T05A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T33 = _mm256_madd_epi16(T05B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T34 = _mm256_madd_epi16(T06A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T35 = _mm256_madd_epi16(T06B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	T36 = _mm256_madd_epi16(T07A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)])); \
	T37 = _mm256_madd_epi16(T07B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab)+1])); \
	\
	T40 = _mm256_hadd_epi32(T20, T21); \
	T41 = _mm256_hadd_epi32(T22, T23); \
	T42 = _mm256_hadd_epi32(T24, T25); \
	T43 = _mm256_hadd_epi32(T26, T27); \
	T44 = _mm256_hadd_epi32(T30, T31); \
	T45 = _mm256_hadd_epi32(T32, T33); \
	T46 = _mm256_hadd_epi32(T34, T35); \
	T47 = _mm256_hadd_epi32(T36, T37); \
	\
	T50 = _mm256_hadd_epi32(T40, T41); \
	T51 = _mm256_hadd_epi32(T42, T43); \
	T52 = _mm256_hadd_epi32(T44, T45); \
	T53 = _mm256_hadd_epi32(T46, T47); \
	\
	T50 = _mm256_hadd_epi32(T50, T51); \
	T51 = _mm256_hadd_epi32(T52, T53); \
	T50 = _mm256_srai_epi32(_mm256_add_epi32(T50, c_8), shift1); \
	T51 = _mm256_srai_epi32(_mm256_add_epi32(T51, c_8), shift1); \
	T60 = _mm256_packs_epi32(T50, T51); \
	im[(dstPos)][i] = T60;

		MAKE_ODD(12, 1);
		MAKE_ODD(14, 3);
		MAKE_ODD(16, 5);
		MAKE_ODD(18, 7);
		MAKE_ODD(20, 9);
		MAKE_ODD(22, 11);
		MAKE_ODD(24, 13);
		MAKE_ODD(26, 15);

#undef MAKE_ODD
	}

	// DCT2
	T00A = _mm256_permute2x128_si256(im[0][0], im[8][0], 0x20);
	T00B = _mm256_permute2x128_si256(im[0][0], im[8][0], 0x31);
	T01A = _mm256_permute2x128_si256(im[1][0], im[9][0], 0x20);
	T01B = _mm256_permute2x128_si256(im[1][0], im[9][0], 0x31);
	T02A = _mm256_permute2x128_si256(im[2][0], im[10][0], 0x20);
	T02B = _mm256_permute2x128_si256(im[2][0], im[10][0], 0x31);
	T03A = _mm256_permute2x128_si256(im[3][0], im[11][0], 0x20);
	T03B = _mm256_permute2x128_si256(im[3][0], im[11][0], 0x31);
	TT00A = _mm256_permute2x128_si256(im[4][0], im[12][0], 0x20);
	TT00B = _mm256_permute2x128_si256(im[4][0], im[12][0], 0x31);
	TT01A = _mm256_permute2x128_si256(im[5][0], im[13][0], 0x20);
	TT01B = _mm256_permute2x128_si256(im[5][0], im[13][0], 0x31);
	TT02A = _mm256_permute2x128_si256(im[6][0], im[14][0], 0x20);
	TT02B = _mm256_permute2x128_si256(im[6][0], im[14][0], 0x31);
	TT03A = _mm256_permute2x128_si256(im[7][0], im[15][0], 0x20);
	TT03B = _mm256_permute2x128_si256(im[7][0], im[15][0], 0x31);
	T00C = _mm256_permute2x128_si256(im[0][1], im[8][1], 0x20);
	T00D = _mm256_permute2x128_si256(im[0][1], im[8][1], 0x31);
	T01C = _mm256_permute2x128_si256(im[1][1], im[9][1], 0x20);
	T01D = _mm256_permute2x128_si256(im[1][1], im[9][1], 0x31);
	T02C = _mm256_permute2x128_si256(im[2][1], im[10][1], 0x20);
	T02D = _mm256_permute2x128_si256(im[2][1], im[10][1], 0x31);
	T03C = _mm256_permute2x128_si256(im[3][1], im[11][1], 0x20);
	T03D = _mm256_permute2x128_si256(im[3][1], im[11][1], 0x31);
	TT00C = _mm256_permute2x128_si256(im[4][1], im[12][1], 0x20);
	TT00D = _mm256_permute2x128_si256(im[4][1], im[12][1], 0x31);
	TT01C = _mm256_permute2x128_si256(im[5][1], im[13][1], 0x20);
	TT01D = _mm256_permute2x128_si256(im[5][1], im[13][1], 0x31);
	TT02C = _mm256_permute2x128_si256(im[6][1], im[14][1], 0x20);
	TT02D = _mm256_permute2x128_si256(im[6][1], im[14][1], 0x31);
	TT03C = _mm256_permute2x128_si256(im[7][1], im[15][1], 0x20);
	TT03D = _mm256_permute2x128_si256(im[7][1], im[15][1], 0x31);

	TAB16_0_0 = _mm256_load_si256((__m256i*)tab_dct_16_0_256i[0]);
	T00C = _mm256_shuffle_epi8(T00C, TAB16_0_0);    // [16 17 18 19 20 21 22 23]
	T00D = _mm256_shuffle_epi8(T00D, TAB16_0_0);    // [24 25 26 27 28 29 30 31]
	T01C = _mm256_shuffle_epi8(T01C, TAB16_0_0);
	T01D = _mm256_shuffle_epi8(T01D, TAB16_0_0);
	T02C = _mm256_shuffle_epi8(T02C, TAB16_0_0);
	T02D = _mm256_shuffle_epi8(T02D, TAB16_0_0);
	T03C = _mm256_shuffle_epi8(T03C, TAB16_0_0);
	T03D = _mm256_shuffle_epi8(T03D, TAB16_0_0);

	TT00C = _mm256_shuffle_epi8(TT00C, TAB16_0_0);
	TT00D = _mm256_shuffle_epi8(TT00D, TAB16_0_0);
	TT01C = _mm256_shuffle_epi8(TT01C, TAB16_0_0);
	TT01D = _mm256_shuffle_epi8(TT01D, TAB16_0_0);
	TT02C = _mm256_shuffle_epi8(TT02C, TAB16_0_0);
	TT02D = _mm256_shuffle_epi8(TT02D, TAB16_0_0);
	TT03C = _mm256_shuffle_epi8(TT03C, TAB16_0_0);
	TT03D = _mm256_shuffle_epi8(TT03D, TAB16_0_0);

	T10A = _mm256_unpacklo_epi16(T00A, T00D);  // [28 03 29 02 30 01 31 00]
	T10B = _mm256_unpackhi_epi16(T00A, T00D);  // [24 07 25 06 26 05 27 04]
	T00A = _mm256_unpacklo_epi16(T00B, T00C);  // [20 11 21 10 22 09 23 08]
	T00B = _mm256_unpackhi_epi16(T00B, T00C);  // [16 15 17 14 18 13 19 12]
	T11A = _mm256_unpacklo_epi16(T01A, T01D);
	T11B = _mm256_unpackhi_epi16(T01A, T01D);
	T01A = _mm256_unpacklo_epi16(T01B, T01C);
	T01B = _mm256_unpackhi_epi16(T01B, T01C);
	T12A = _mm256_unpacklo_epi16(T02A, T02D);
	T12B = _mm256_unpackhi_epi16(T02A, T02D);
	T02A = _mm256_unpacklo_epi16(T02B, T02C);
	T02B = _mm256_unpackhi_epi16(T02B, T02C);
	T13A = _mm256_unpacklo_epi16(T03A, T03D);
	T13B = _mm256_unpackhi_epi16(T03A, T03D);
	T03A = _mm256_unpacklo_epi16(T03B, T03C);
	T03B = _mm256_unpackhi_epi16(T03B, T03C);

	TT10A = _mm256_unpacklo_epi16(TT00A, TT00D);
	TT10B = _mm256_unpackhi_epi16(TT00A, TT00D);
	TT00A = _mm256_unpacklo_epi16(TT00B, TT00C);
	TT00B = _mm256_unpackhi_epi16(TT00B, TT00C);
	TT11A = _mm256_unpacklo_epi16(TT01A, TT01D);
	TT11B = _mm256_unpackhi_epi16(TT01A, TT01D);
	TT01A = _mm256_unpacklo_epi16(TT01B, TT01C);
	TT01B = _mm256_unpackhi_epi16(TT01B, TT01C);
	TT12A = _mm256_unpacklo_epi16(TT02A, TT02D);
	TT12B = _mm256_unpackhi_epi16(TT02A, TT02D);
	TT02A = _mm256_unpacklo_epi16(TT02B, TT02C);
	TT02B = _mm256_unpackhi_epi16(TT02B, TT02C);
	TT13A = _mm256_unpacklo_epi16(TT03A, TT03D);
	TT13B = _mm256_unpackhi_epi16(TT03A, TT03D);
	TT03A = _mm256_unpacklo_epi16(TT03B, TT03C);
	TT03B = _mm256_unpackhi_epi16(TT03B, TT03C);

#define MAKE_ODD(tab0, tab1, tab2, tab3, dstPos) \
	T20 = _mm256_madd_epi16(T10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T21 = _mm256_madd_epi16(T10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T22 = _mm256_madd_epi16(T00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T23 = _mm256_madd_epi16(T00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T24 = _mm256_madd_epi16(T11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T25 = _mm256_madd_epi16(T11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T26 = _mm256_madd_epi16(T01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T27 = _mm256_madd_epi16(T01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T30 = _mm256_madd_epi16(T12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T31 = _mm256_madd_epi16(T12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T32 = _mm256_madd_epi16(T02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T33 = _mm256_madd_epi16(T02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	T34 = _mm256_madd_epi16(T13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	T35 = _mm256_madd_epi16(T13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	T36 = _mm256_madd_epi16(T03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	T37 = _mm256_madd_epi16(T03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	T60 = _mm256_hadd_epi32(T20, T21); \
	T61 = _mm256_hadd_epi32(T22, T23); \
	T62 = _mm256_hadd_epi32(T24, T25); \
	T63 = _mm256_hadd_epi32(T26, T27); \
	T64 = _mm256_hadd_epi32(T30, T31); \
	T65 = _mm256_hadd_epi32(T32, T33); \
	T66 = _mm256_hadd_epi32(T34, T35); \
	T67 = _mm256_hadd_epi32(T36, T37); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	T62 = _mm256_hadd_epi32(T64, T65); \
	T63 = _mm256_hadd_epi32(T66, T67); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	T61 = _mm256_hadd_epi32(T62, T63); \
	\
	T60 = _mm256_hadd_epi32(T60, T61); \
	\
	T60 = _mm256_srai_epi32(_mm256_add_epi32(T60, c_512), shift2); \
	\
	TT20 = _mm256_madd_epi16(TT10A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT21 = _mm256_madd_epi16(TT10B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT22 = _mm256_madd_epi16(TT00A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT23 = _mm256_madd_epi16(TT00B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT24 = _mm256_madd_epi16(TT11A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT25 = _mm256_madd_epi16(TT11B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT26 = _mm256_madd_epi16(TT01A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT27 = _mm256_madd_epi16(TT01B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT30 = _mm256_madd_epi16(TT12A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT31 = _mm256_madd_epi16(TT12B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT32 = _mm256_madd_epi16(TT02A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT33 = _mm256_madd_epi16(TT02B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	TT34 = _mm256_madd_epi16(TT13A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab0)])); \
	TT35 = _mm256_madd_epi16(TT13B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab1)])); \
	TT36 = _mm256_madd_epi16(TT03A, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab2)])); \
	TT37 = _mm256_madd_epi16(TT03B, _mm256_load_si256((__m256i*)tab_dct_32_1_256i[(tab3)])); \
	\
	TT60 = _mm256_hadd_epi32(TT20, TT21); \
	TT61 = _mm256_hadd_epi32(TT22, TT23); \
	TT62 = _mm256_hadd_epi32(TT24, TT25); \
	TT63 = _mm256_hadd_epi32(TT26, TT27); \
	TT64 = _mm256_hadd_epi32(TT30, TT31); \
	TT65 = _mm256_hadd_epi32(TT32, TT33); \
	TT66 = _mm256_hadd_epi32(TT34, TT35); \
	TT67 = _mm256_hadd_epi32(TT36, TT37); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	TT62 = _mm256_hadd_epi32(TT64, TT65); \
	TT63 = _mm256_hadd_epi32(TT66, TT67); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	TT61 = _mm256_hadd_epi32(TT62, TT63); \
	\
	TT60 = _mm256_hadd_epi32(TT60, TT61); \
	\
	TT60 = _mm256_srai_epi32(_mm256_add_epi32(TT60, c_512), shift2); \
	\
	tResult = _mm256_packs_epi32(T60, TT60); \
	_mm256_storeu_si256((__m256i*)&dst[(dstPos)* 32], tResult); \

	MAKE_ODD(44, 44, 44, 44, 0);
	MAKE_ODD(46, 47, 46, 47, 8);

	MAKE_ODD(50, 51, 52, 53, 4);
	MAKE_ODD(54, 55, 56, 57, 12);

	MAKE_ODD(66, 67, 68, 69, 2);
	MAKE_ODD(70, 71, 72, 73, 6);
	MAKE_ODD(74, 75, 76, 77, 10);
	MAKE_ODD(78, 79, 80, 81, 14);

	MAKE_ODD(98, 99, 100, 101, 1);
	MAKE_ODD(102, 103, 104, 105, 3);
	MAKE_ODD(106, 107, 108, 109, 5);
	MAKE_ODD(110, 111, 112, 113, 7);
	MAKE_ODD(114, 115, 116, 117, 9);
	MAKE_ODD(118, 119, 120, 121, 11);
	MAKE_ODD(122, 123, 124, 125, 13);
	MAKE_ODD(126, 127, 128, 129, 15);
#undef MAKE_ODD

	for (i = 0; i < 16; i++) {
		_mm256_storeu_si256((__m256i*)(dst + 16), tmpZero);
		dst += 32;
	}
	for (i = 16; i < 32; i++) {
		_mm256_storeu_si256((__m256i*)(dst), tmpZero);
		_mm256_storeu_si256((__m256i*)(dst + 16), tmpZero);
		dst += 32;
	}
}

void xTr2nd_8_1d_Hor_sse128(coef_t *src, int i_src){
	__m128i zero = _mm_setzero_si128();
	__m128i factor;
	__m128i	tmpCoef0, tmpCoef1, tmpCoef2, tmpCoef3;
	__m128i tmpS0, tmpS1, tmpS2, tmpS3, tmpRes0, tmpRes1, tmpRes2, tmpRes3;
	__m128i tmpProduct0, tmpProduct1, tmpProduct2, tmpProduct3, tmpSum0, tmpSum1, tmpSum2, tmpSum3;

	/*---hor transform---*/
	// shift = 7; rnd_factor = 64
	factor = _mm_set1_epi32(64);

	//load tab_c4_trans data, a matrix of 4x4
	tmpCoef0 = _mm_set_epi16(0, 8, 0, 14, 0, -32, 0, 123);
	tmpCoef1 = _mm_set_epi16(0, 13, 0, 25, 0, -120, 0, -35);
	tmpCoef2 = _mm_set_epi16(0, 19, 0, 123, 0, 30, 0, -8);
	tmpCoef3 = _mm_set_epi16(0, 126, 0, -22, 0, 10, 0, -3);

	/*-------for j = 0-------*/
	//load src data
	tmpS0 = _mm_set1_epi16(src[0]);
	tmpS1 = _mm_set1_epi16(src[1]);
	tmpS2 = _mm_set1_epi16(src[2]);
	tmpS3 = _mm_set1_epi16(src[3]);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 1-------*/
	//load src data
	tmpS0 = _mm_set1_epi16(src[0 + i_src]);
	tmpS1 = _mm_set1_epi16(src[1 + i_src]);
	tmpS2 = _mm_set1_epi16(src[2 + i_src]);
	tmpS3 = _mm_set1_epi16(src[3 + i_src]);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 2-------*/
	//load src data
	tmpS0 = _mm_set1_epi16(src[0 + 2 * i_src]);
	tmpS1 = _mm_set1_epi16(src[1 + 2 * i_src]);
	tmpS2 = _mm_set1_epi16(src[2 + 2 * i_src]);
	tmpS3 = _mm_set1_epi16(src[3 + 2 * i_src]);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 3-------*/
	//load src data
	tmpS0 = _mm_set1_epi16(src[0 + 3 * i_src]);
	tmpS1 = _mm_set1_epi16(src[1 + 3 * i_src]);
	tmpS2 = _mm_set1_epi16(src[2 + 3 * i_src]);
	tmpS3 = _mm_set1_epi16(src[3 + 3 * i_src]);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpCoef0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpCoef1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpCoef2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpCoef3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift operation
	tmpRes3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	_mm_storel_epi64((__m128i*)&src[0], tmpRes0);
	_mm_storel_epi64((__m128i*)&src[i_src], tmpRes1);
	_mm_storel_epi64((__m128i*)&src[2 * i_src], tmpRes2);
	_mm_storel_epi64((__m128i*)&src[3 * i_src], tmpRes3);
}

void xTr2nd_8_1d_Vert_sse128(coef_t *src, int i_src){
	__m128i zero = _mm_setzero_si128();
	__m128i tmpC0, tmpC1, tmpC2, tmpC3, factor;
	__m128i tmpS0, tmpS1, tmpS2, tmpS3, tmpL0, tmpL1, tmpL2, tmpL3;
	__m128i tmpProduct0, tmpProduct1, tmpProduct2, tmpProduct3, tmpSum0, tmpSum1, tmpSum2, tmpSum3, tmpDst0, tmpDst1, tmpDst2, tmpDst3;

	/*---vertical transform---*/

	//rnd_factor = 64; shift = 7
	factor = _mm_set1_epi32(64);

	tmpL0 = _mm_loadu_si128((__m128i*)&src[0]);
	tmpL1 = _mm_loadu_si128((__m128i*)&src[i_src]);
	tmpL2 = _mm_loadu_si128((__m128i*)&src[i_src * 2]);
	tmpL3 = _mm_loadu_si128((__m128i*)&src[i_src * 3]);
	tmpS0 = _mm_unpacklo_epi16(tmpL0, zero);
	tmpS1 = _mm_unpacklo_epi16(tmpL1, zero);
	tmpS2 = _mm_unpacklo_epi16(tmpL2, zero);
	tmpS3 = _mm_unpacklo_epi16(tmpL3, zero);

	//for i = 0
	//load coef data
	tmpC0 = _mm_set1_epi16(123);							//coef[0][0]
	tmpC1 = _mm_set1_epi16(-35);							//coef[1][0]
	tmpC2 = _mm_set1_epi16(-8);
	tmpC3 = _mm_set1_epi16(-3);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);			
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 1
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(-32);
	tmpC1 = _mm_set1_epi16(-120);
	tmpC2 = _mm_set1_epi16(30);
	tmpC3 = _mm_set1_epi16(10);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 2
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(14);
	tmpC1 = _mm_set1_epi16(25);
	tmpC2 = _mm_set1_epi16(123);
	tmpC3 = _mm_set1_epi16(-22);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 3
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(8);
	tmpC1 = _mm_set1_epi16(13);
	tmpC2 = _mm_set1_epi16(19);
	tmpC3 = _mm_set1_epi16(126);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpS0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpS1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpS2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpS3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpDst3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	_mm_storel_epi64((__m128i*)&src[0], tmpDst0);
	_mm_storel_epi64((__m128i*)&src[i_src], tmpDst1);
	_mm_storel_epi64((__m128i*)&src[2 * i_src], tmpDst2);
	_mm_storel_epi64((__m128i*)&src[3 * i_src], tmpDst3);
}

ALIGNED_32(static const short tab_dct8_pb4_coeffs_256i[][16]) = {
    // iT[2]+iT[3], iT[1], iT[2], iT[3]
    { 42, 37, 27, 15, 42, 37, 27, 15, 42, 37, 27, 15, 42, 37, 27, 15 },
    // iT[1], 0, -iT[1], -iT[1]
    { 37, 0, -37, -37, 37, 0, -37, -37, 37, 0, -37, -37, 37, 0, -37, -37 },
    // iT[2], -iT[1], -iT[3], iT[2]+iT[3]
    { 27, -37, -15, 42, 27, -37, -15, 42, 27, -37, -15, 42, 27, -37, -15, 42 },
    // iT[3], -iT[1], iT[2]+iT[3], -iT[2]
    { 15, -37, 42, -27, 15, -37, 42, -27, 15, -37, 42, -27, 15, -37, 42, -27 }
};

ALIGNED_32(static const short tab_dct8_pb8_coeffs_256i[][16]) = {
    { 44, 42, 39, 35, 30, 23, 16, 8, 44, 42, 39, 35, 30, 23, 16, 8 },
    { 42, 30, 8, -16, -35, -44, -39, -23, 42, 30, 8, -16, -35, -44, -39, -23 },
    { 39, 8, -30, -44, -23, 16, 42, 35, 39, 8, -30, -44, -23, 16, 42, 35 },
    { 35, -16, -44, -8, 39, 30, -23, -42, 35, -16, -44, -8, 39, 30, -23, -42 },
    { 30, -35, -23, 39, 16, -42, -8, 44, 30, -35, -23, 39, 16, -42, -8, 44 },
    { 23, -44, 16, 30, -42, 8, 35, -39, 23, -44, 16, 30, -42, 8, 35, -39 },
    { 16, -39, 42, -23, -8, 35, -44, 30, 16, -39, 42, -23, -8, 35, -44, 30 },
    { 8, -23, 35, -42, 44, -39, 30, -16, 8, -23, 35, -42, 44, -39, 30, -16 }
};

ALIGNED_32(static const short tab_dct8_pb16_coeffs_256i[][16]) = {
    { 45, 44, 43, 42, 41, 39, 36, 34, 31, 28, 24, 20, 17, 13, 8, 4 },
    { 44, 41, 34, 24, 13, 0, -13, -24, -34, -41, -44, -44, -41, -34, -24, -13 },
    { 43, 34, 17, -4, -24, -39, -45, -41, -28, -8, 13, 31, 42, 44, 36, 20 },
    { 42, 24, -4, -31, -44, -39, -17, 13, 36, 45, 34, 8, -20, -41, -43, -28 },
    { 41, 13, -24, -44, -34, 0, 34, 44, 24, -13, -41, -41, -13, 24, 44, 34 },
    { 39, 0, -39, -39, 0, 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, -39 },
    { 36, -13, -45, -17, 34, 39, -8, -44, -20, 31, 41, -4, -43, -24, 28, 42 },
    { 34, -24, -41, 13, 44, 0, -44, -13, 41, 24, -34, -34, 24, 41, -13, -44 },
    { 31, -34, -28, 36, 24, -39, -20, 41, 17, -42, -13, 43, 8, -44, -4, 45 },
    { 28, -41, -8, 45, -13, -39, 31, 24, -42, -4, 44, -17, -36, 34, 20, -43 },
    { 24, -44, 13, 34, -41, 0, 41, -34, -13, 44, -24, -24, 44, -13, -34, 41 },
    { 20, -44, 31, 8, -41, 39, -4, -34, 43, -17, -24, 45, -28, -13, 42, -36 },
    { 17, -41, 42, -20, -13, 39, -43, 24, 8, -36, 44, -28, -4, 34, -45, 31 },
    { 13, -34, 44, -41, 24, 0, -24, 41, -44, 34, -13, -13, 34, -44, 41, -24 },
    { 8, -24, 36, -43, 44, -39, 28, -13, -4, 20, -34, 42, -45, 41, -31, 17 },
    { 4, -13, 20, -28, 34, -39, 42, -44, 45, -43, 41, -36, 31, -24, 17, -8 }
};

ALIGNED_32(static const short tab_dst7_pb4_coeffs_256i[][16]) = {
    // iT[0], iT[1], iT[2], iT[0]+iT[1]
    { 15, 27, 37, 42, 15, 27, 37, 42, 15, 27, 37, 42, 15, 27, 37, 42 },
    // iT[2], iT[2], 0, -iT[1]
    { 37, 37, 0, -37, 37, 37, 0, -37, 37, 37, 0, -37, 37, 37, 0, -37 },
    // iT[0]+iT[1], -iT[0], -iT[2], iT[1]
    { 42, -15, -37, 27, 42, -15, -37, 27, 42, -15, -37, 27, 42, -15, -37, 27 },
    // iT[1], -iT[0]-iT[1], iT[2], -iT[0]
    { 27, -42, 37, -15, 27, -42, 37, -15, 27, -42, 37, -15, 27, -42, 37, -15 }
};

ALIGNED_32(static const short tab_dst7_pb8_coeffs_256i[][16]) = {
    { 8, 16, 23, 30, 35, 39, 42, 44, 8, 16, 23, 30, 35, 39, 42, 44 },
    { 23, 39, 44, 35, 16, -8, -30, -42, 23, 39, 44, 35, 16, -8, -30, -42 },
    { 35, 42, 16, -23, -44, -30, 8, 39, 35, 42, 16, -23, -44, -30, 8, 39 },
    { 42, 23, -30, -39, 8, 44, 16, -35, 42, 23, -30, -39, 8, 44, 16, -35 },
    { 44, -8, -42, 16, 39, -23, -35, 30, 44, -8, -42, 16, 39, -23, -35, 30 },
    { 39, -35, -8, 42, -30, -16, 44, -23, 39, -35, -8, 42, -30, -16, 44, -23 },
    { 30, -44, 35, -8, -23, 42, -39, 16, 30, -44, 35, -8, -23, 42, -39, 16 },
    { 16, -30, 39, -44, 42, -35, 23, -8, 16, -30, 39, -44, 42, -35, 23, -8 }
};

ALIGNED_32(static const short tab_dst7_pb16_coeffs_256i[][16]) = {
    { 4, 8, 13, 17, 20, 24, 28, 31, 34, 36, 39, 41, 42, 43, 44, 45, },
    { 13, 24, 34, 41, 44, 44, 41, 34, 24, 13, 0, -13, -24, -34, -41, -44, },
    { 20, 36, 44, 42, 31, 13, -8, -28, -41, -45, -39, -24, -4, 17, 34, 43, },
    { 28, 43, 41, 20, -8, -34, -45, -36, -13, 17, 39, 44, 31, 4, -24, -42, },
    { 34, 44, 24, -13, -41, -41, -13, 24, 44, 34, 0, -34, -44, -24, 13, 41, },
    { 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, },
    { 42, 28, -24, -43, -4, 41, 31, -20, -44, -8, 39, 34, -17, -45, -13, 36, },
    { 44, 13, -41, -24, 34, 34, -24, -41, 13, 44, 0, -44, -13, 41, 24, -34, },
    { 45, -4, -44, 8, 43, -13, -42, 17, 41, -20, -39, 24, 36, -28, -34, 31, },
    { 43, -20, -34, 36, 17, -44, 4, 42, -24, -31, 39, 13, -45, 8, 41, -28, },
    { 41, -34, -13, 44, -24, -24, 44, -13, -34, 41, 0, -41, 34, 13, -44, 24, },
    { 36, -42, 13, 28, -45, 24, 17, -43, 34, 4, -39, 41, -8, -31, 44, -20, },
    { 31, -45, 34, -4, -28, 44, -36, 8, 24, -43, 39, -13, -20, 42, -41, 17, },
    { 24, -41, 44, -34, 13, 13, -34, 44, -41, 24, 0, -24, 41, -44, 34, -13, },
    { 17, -31, 41, -45, 42, -34, 20, -4, -13, 28, -39, 44, -43, 36, -24, 8, },
    { 8, -17, 24, -31, 36, -41, 43, -45, 44, -42, 39, -34, 28, -20, 13, -4, }
};

void trans_dct8_pb4_avx2(short *src, short *dst, int shift)
{
    int i = 0;
    __m256i s0;
    __m256i c0, c1, c2, c3;
    __m256i v0, v1, v2, v3;
    __m256i off;
    __m128i d0, d1, d2, d3;

    if (shift) {
        off = _mm256_set1_epi32(1 << (shift - 1));
    } else {
        off = _mm256_set1_epi32(0);
    }

    c0 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[0]));
    c1 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[1]));
    c2 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[2]));
    c3 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[3]));

    s0 = _mm256_loadu_si256((__m256i*)(src));   // src[0][0-3], src[1][0-3], src[2][0-3], src[3][0-3]

    v0 = _mm256_madd_epi16(s0, c0);
    v1 = _mm256_madd_epi16(s0, c1);
    v2 = _mm256_madd_epi16(s0, c2);
    v3 = _mm256_madd_epi16(s0, c3);

    v0 = _mm256_hadd_epi32(v0, v1);
    v2 = _mm256_hadd_epi32(v2, v3);

    v0 = _mm256_add_epi32(v0, off);
    v2 = _mm256_add_epi32(v2, off);

    v0 = _mm256_srai_epi32(v0, shift);          // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
    v2 = _mm256_srai_epi32(v2, shift);

    v0 = _mm256_packs_epi32(v0, v2);            // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]

    d0 = _mm256_castsi256_si128(v0);
    d1 = _mm256_extracti128_si256(v0, 1);

    d2 = _mm_unpacklo_epi32(d0, d1);            // dst[0][0-3], dst[1][0-3]
    d3 = _mm_unpackhi_epi32(d0, d1);
    d0 = _mm_srli_si128(d2, 8);
    d1 = _mm_srli_si128(d3, 8);
    _mm_storel_epi64((__m128i*)(dst), d2);
    _mm_storel_epi64((__m128i*)(dst + 4), d0);
    _mm_storel_epi64((__m128i*)(dst + 4 * 2), d3);
    _mm_storel_epi64((__m128i*)(dst + 4 * 3), d1);
}

void trans_dct8_pb8_avx2(short *src, short *dst, int shift)
{
    __m256i s0, s1;
    __m256i c[8];
    __m256i v[16];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j;

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < 8; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dct8_pb8_coeffs_256i[i]));
    }

    for (i = 0; i < 8; i += 4) {
        s0 = _mm256_loadu2_m128i((__m128i *)(src + 16), (__m128i *)(src));      // src[0][0-7], src[2][0-7]
        s1 = _mm256_loadu2_m128i((__m128i *)(src + 24), (__m128i *)(src + 8));  // src[1][0-7], src[3][0-7]

        for (j = 0; j < 8; j++) {
            v[j] = _mm256_madd_epi16(s0, c[j]);
            v[j + 8] = _mm256_madd_epi16(s1, c[j]);
        }

        v[0] = _mm256_hadd_epi32(v[0], v[8]);
        v[1] = _mm256_hadd_epi32(v[1], v[9]);
        v[2] = _mm256_hadd_epi32(v[2], v[10]);
        v[3] = _mm256_hadd_epi32(v[3], v[11]);
        v[4] = _mm256_hadd_epi32(v[4], v[12]);
        v[5] = _mm256_hadd_epi32(v[5], v[13]);
        v[6] = _mm256_hadd_epi32(v[6], v[14]);
        v[7] = _mm256_hadd_epi32(v[7], v[15]);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);
        v[2] = _mm256_hadd_epi32(v[2], v[3]);
        v[4] = _mm256_hadd_epi32(v[4], v[5]);
        v[6] = _mm256_hadd_epi32(v[6], v[7]);

        v[0] = _mm256_add_epi32(v[0], off);
        v[2] = _mm256_add_epi32(v[2], off);
        v[4] = _mm256_add_epi32(v[4], off);
        v[6] = _mm256_add_epi32(v[6], off);

        v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
        v[2] = _mm256_srai_epi32(v[2], shift);
        v[4] = _mm256_srai_epi32(v[4], shift);
        v[6] = _mm256_srai_epi32(v[6], shift);

        v[0] = _mm256_packs_epi32(v[0], v[2]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
        v[4] = _mm256_packs_epi32(v[4], v[6]);

        d4 = _mm256_castsi256_si128(v[0]);
        d5 = _mm256_extracti128_si256(v[0], 1);
        d6 = _mm256_castsi256_si128(v[4]);
        d7 = _mm256_extracti128_si256(v[4], 1);

        d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
        d1 = _mm_unpackhi_epi32(d4, d5);
        d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
        d3 = _mm_unpackhi_epi32(d6, d7);
        d4 = _mm_srli_si128(d0, 8);
        d5 = _mm_srli_si128(d1, 8);
        d6 = _mm_srli_si128(d2, 8);
        d7 = _mm_srli_si128(d3, 8);
        _mm_storel_epi64((__m128i*)(dst), d0);
        _mm_storel_epi64((__m128i*)(dst + 8), d4);
        _mm_storel_epi64((__m128i*)(dst + 8 * 2), d1);
        _mm_storel_epi64((__m128i*)(dst + 8 * 3), d5);
        _mm_storel_epi64((__m128i*)(dst + 8 * 4), d2);
        _mm_storel_epi64((__m128i*)(dst + 8 * 5), d6);
        _mm_storel_epi64((__m128i*)(dst + 8 * 6), d3);
        _mm_storel_epi64((__m128i*)(dst + 8 * 7), d7);

        src += 8 * 4;
        dst += 4;
    }
}

void trans_dct8_pb16_avx2(short *src, short *dst, int shift)
{
    __m256i s0, s1, s2, s3;
    __m256i c[16];
    __m256i v[32], t[8];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j, k;

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < 16; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dct8_pb16_coeffs_256i[i]));
    }

    for (i = 0; i < 16; i += 4)
    {
        short* pdst = dst;

        s0 = _mm256_loadu_si256((__m256i *)(src));          // src[0][0-15]
        s1 = _mm256_loadu_si256((__m256i *)(src + 16));     // src[1][0-15]
        s2 = _mm256_loadu_si256((__m256i *)(src + 32));
        s3 = _mm256_loadu_si256((__m256i *)(src + 48));

        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                int cid = k + j * 8;
                v[k] = _mm256_madd_epi16(s0, c[cid]);
                v[k + 8] = _mm256_madd_epi16(s1, c[cid]);
                v[k + 16] = _mm256_madd_epi16(s2, c[cid]);
                v[k + 24] = _mm256_madd_epi16(s3, c[cid]);
            }

            v[0] = _mm256_hadd_epi32(v[0], v[8]);
            v[1] = _mm256_hadd_epi32(v[1], v[9]);
            v[2] = _mm256_hadd_epi32(v[2], v[10]);
            v[3] = _mm256_hadd_epi32(v[3], v[11]);
            v[4] = _mm256_hadd_epi32(v[4], v[12]);
            v[5] = _mm256_hadd_epi32(v[5], v[13]);
            v[6] = _mm256_hadd_epi32(v[6], v[14]);
            v[7] = _mm256_hadd_epi32(v[7], v[15]);

            v[16] = _mm256_hadd_epi32(v[16], v[24]);
            v[17] = _mm256_hadd_epi32(v[17], v[25]);
            v[18] = _mm256_hadd_epi32(v[18], v[26]);
            v[19] = _mm256_hadd_epi32(v[19], v[27]);
            v[20] = _mm256_hadd_epi32(v[20], v[28]);
            v[21] = _mm256_hadd_epi32(v[21], v[29]);
            v[22] = _mm256_hadd_epi32(v[22], v[30]);
            v[23] = _mm256_hadd_epi32(v[23], v[31]);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);
            v[2] = _mm256_hadd_epi32(v[2], v[3]);
            v[4] = _mm256_hadd_epi32(v[4], v[5]);
            v[6] = _mm256_hadd_epi32(v[6], v[7]);

            v[16] = _mm256_hadd_epi32(v[16], v[17]);
            v[18] = _mm256_hadd_epi32(v[18], v[19]);
            v[20] = _mm256_hadd_epi32(v[20], v[21]);
            v[22] = _mm256_hadd_epi32(v[22], v[23]);

            t[0] = _mm256_permute2x128_si256(v[0], v[16], 0x20);
            t[1] = _mm256_permute2x128_si256(v[0], v[16], 0x31);
            t[2] = _mm256_permute2x128_si256(v[2], v[18], 0x20);
            t[3] = _mm256_permute2x128_si256(v[2], v[18], 0x31);
            t[4] = _mm256_permute2x128_si256(v[4], v[20], 0x20);
            t[5] = _mm256_permute2x128_si256(v[4], v[20], 0x31);
            t[6] = _mm256_permute2x128_si256(v[6], v[22], 0x20);
            t[7] = _mm256_permute2x128_si256(v[6], v[22], 0x31);

            v[0] = _mm256_add_epi32(t[0], t[1]);
            v[1] = _mm256_add_epi32(t[2], t[3]);
            v[2] = _mm256_add_epi32(t[4], t[5]);
            v[3] = _mm256_add_epi32(t[6], t[7]);

            v[0] = _mm256_add_epi32(v[0], off);
            v[1] = _mm256_add_epi32(v[1], off);
            v[2] = _mm256_add_epi32(v[2], off);
            v[3] = _mm256_add_epi32(v[3], off);

            v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
            v[1] = _mm256_srai_epi32(v[1], shift);
            v[2] = _mm256_srai_epi32(v[2], shift);
            v[3] = _mm256_srai_epi32(v[3], shift);

            v[0] = _mm256_packs_epi32(v[0], v[1]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
            v[2] = _mm256_packs_epi32(v[2], v[3]);

            d4 = _mm256_castsi256_si128(v[0]);
            d5 = _mm256_extracti128_si256(v[0], 1);
            d6 = _mm256_castsi256_si128(v[2]);
            d7 = _mm256_extracti128_si256(v[2], 1);

            d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
            d1 = _mm_unpackhi_epi32(d4, d5);
            d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
            d3 = _mm_unpackhi_epi32(d6, d7);
            d4 = _mm_srli_si128(d0, 8);
            d5 = _mm_srli_si128(d1, 8);
            d6 = _mm_srli_si128(d2, 8);
            d7 = _mm_srli_si128(d3, 8);
            _mm_storel_epi64((__m128i*)(pdst), d0);
            _mm_storel_epi64((__m128i*)(pdst + 16), d4);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 2), d1);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 3), d5);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 4), d2);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 5), d6);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 6), d3);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 7), d7);

            pdst += 16 * 8;
        }
        src += 16 * 4;
        dst += 4;
    }
}

void trans_dst7_pb4_avx2(short *src, short *dst, int shift)
{
    int i = 0;
    __m256i s0;
    __m256i c0, c1, c2, c3;
    __m256i v0, v1, v2, v3;
    __m256i off;
    __m128i d0, d1, d2, d3;

    if (shift) {
        off = _mm256_set1_epi32(1 << (shift - 1));
    }
    else {
        off = _mm256_set1_epi32(0);
    }

    c0 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[0]));
    c1 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[1]));
    c2 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[2]));
    c3 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[3]));

    s0 = _mm256_loadu_si256((__m256i*)(src));   // src[0][0-3], src[1][0-3], src[2][0-3], src[3][0-3]

    v0 = _mm256_madd_epi16(s0, c0);
    v1 = _mm256_madd_epi16(s0, c1);
    v2 = _mm256_madd_epi16(s0, c2);
    v3 = _mm256_madd_epi16(s0, c3);

    v0 = _mm256_hadd_epi32(v0, v1);
    v2 = _mm256_hadd_epi32(v2, v3);

    v0 = _mm256_add_epi32(v0, off);
    v2 = _mm256_add_epi32(v2, off);

    v0 = _mm256_srai_epi32(v0, shift);          // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
    v2 = _mm256_srai_epi32(v2, shift);

    v0 = _mm256_packs_epi32(v0, v2);            // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]

    d0 = _mm256_castsi256_si128(v0);
    d1 = _mm256_extracti128_si256(v0, 1);

    d2 = _mm_unpacklo_epi32(d0, d1);            // dst[0][0-3], dst[1][0-3]
    d3 = _mm_unpackhi_epi32(d0, d1);
    d0 = _mm_srli_si128(d2, 8);
    d1 = _mm_srli_si128(d3, 8);
    _mm_storel_epi64((__m128i*)(dst), d2);
    _mm_storel_epi64((__m128i*)(dst + 4), d0);
    _mm_storel_epi64((__m128i*)(dst + 4 * 2), d3);
    _mm_storel_epi64((__m128i*)(dst + 4 * 3), d1); 
}

void trans_dst7_pb8_avx2(short *src, short *dst, int shift)
{
    __m256i s0, s1;
    __m256i c[8];
    __m256i v[16];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j;

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < 8; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dst7_pb8_coeffs_256i[i]));
    }

    for (i = 0; i < 8; i += 4)
    {
        s0 = _mm256_loadu2_m128i((__m128i *)(src + 16), (__m128i *)(src));      // src[0][0-7], src[2][0-7]
        s1 = _mm256_loadu2_m128i((__m128i *)(src + 24), (__m128i *)(src + 8));  // src[1][0-7], src[3][0-7]

        for (j = 0; j < 8; j++) {
            v[j] = _mm256_madd_epi16(s0, c[j]);
            v[j + 8] = _mm256_madd_epi16(s1, c[j]);
        }

        v[0] = _mm256_hadd_epi32(v[0], v[8]);
        v[1] = _mm256_hadd_epi32(v[1], v[9]);
        v[2] = _mm256_hadd_epi32(v[2], v[10]);
        v[3] = _mm256_hadd_epi32(v[3], v[11]);
        v[4] = _mm256_hadd_epi32(v[4], v[12]);
        v[5] = _mm256_hadd_epi32(v[5], v[13]);
        v[6] = _mm256_hadd_epi32(v[6], v[14]);
        v[7] = _mm256_hadd_epi32(v[7], v[15]);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);
        v[2] = _mm256_hadd_epi32(v[2], v[3]);
        v[4] = _mm256_hadd_epi32(v[4], v[5]);
        v[6] = _mm256_hadd_epi32(v[6], v[7]);

        v[0] = _mm256_add_epi32(v[0], off);
        v[2] = _mm256_add_epi32(v[2], off);
        v[4] = _mm256_add_epi32(v[4], off);
        v[6] = _mm256_add_epi32(v[6], off);

        v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
        v[2] = _mm256_srai_epi32(v[2], shift);
        v[4] = _mm256_srai_epi32(v[4], shift);
        v[6] = _mm256_srai_epi32(v[6], shift);

        v[0] = _mm256_packs_epi32(v[0], v[2]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
        v[4] = _mm256_packs_epi32(v[4], v[6]);

        d4 = _mm256_castsi256_si128(v[0]);
        d5 = _mm256_extracti128_si256(v[0], 1);
        d6 = _mm256_castsi256_si128(v[4]);
        d7 = _mm256_extracti128_si256(v[4], 1);

        d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
        d1 = _mm_unpackhi_epi32(d4, d5);
        d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
        d3 = _mm_unpackhi_epi32(d6, d7);
        d4 = _mm_srli_si128(d0, 8);
        d5 = _mm_srli_si128(d1, 8);
        d6 = _mm_srli_si128(d2, 8);
        d7 = _mm_srli_si128(d3, 8);
        _mm_storel_epi64((__m128i*)(dst), d0);
        _mm_storel_epi64((__m128i*)(dst + 8), d4);
        _mm_storel_epi64((__m128i*)(dst + 8 * 2), d1);
        _mm_storel_epi64((__m128i*)(dst + 8 * 3), d5);
        _mm_storel_epi64((__m128i*)(dst + 8 * 4), d2);
        _mm_storel_epi64((__m128i*)(dst + 8 * 5), d6);
        _mm_storel_epi64((__m128i*)(dst + 8 * 6), d3);
        _mm_storel_epi64((__m128i*)(dst + 8 * 7), d7);

        src += 8 * 4;
        dst += 4;
    }
}

void trans_dst7_pb16_avx2(short *src, short *dst, int shift)
{
    __m256i s0, s1, s2, s3;
    __m256i c[16];
    __m256i v[32], t[8];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j, k;

    for (i = 0; i < 16; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dst7_pb16_coeffs_256i[i]));
    }

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < 16; i += 4)
    {
        short* pdst = dst;

        s0 = _mm256_loadu_si256((__m256i *)(src));          // src[0][0-15]
        s1 = _mm256_loadu_si256((__m256i *)(src + 16));     // src[1][0-15]
        s2 = _mm256_loadu_si256((__m256i *)(src + 32));
        s3 = _mm256_loadu_si256((__m256i *)(src + 48));

        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                int cid = k + j * 8;
                v[k] = _mm256_madd_epi16(s0, c[cid]);
                v[k + 8] = _mm256_madd_epi16(s1, c[cid]);
                v[k + 16] = _mm256_madd_epi16(s2, c[cid]);
                v[k + 24] = _mm256_madd_epi16(s3, c[cid]);
            }

            v[0] = _mm256_hadd_epi32(v[0], v[8]);
            v[1] = _mm256_hadd_epi32(v[1], v[9]);
            v[2] = _mm256_hadd_epi32(v[2], v[10]);
            v[3] = _mm256_hadd_epi32(v[3], v[11]);
            v[4] = _mm256_hadd_epi32(v[4], v[12]);
            v[5] = _mm256_hadd_epi32(v[5], v[13]);
            v[6] = _mm256_hadd_epi32(v[6], v[14]);
            v[7] = _mm256_hadd_epi32(v[7], v[15]);

            v[16] = _mm256_hadd_epi32(v[16], v[24]);
            v[17] = _mm256_hadd_epi32(v[17], v[25]);
            v[18] = _mm256_hadd_epi32(v[18], v[26]);
            v[19] = _mm256_hadd_epi32(v[19], v[27]);
            v[20] = _mm256_hadd_epi32(v[20], v[28]);
            v[21] = _mm256_hadd_epi32(v[21], v[29]);
            v[22] = _mm256_hadd_epi32(v[22], v[30]);
            v[23] = _mm256_hadd_epi32(v[23], v[31]);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);
            v[2] = _mm256_hadd_epi32(v[2], v[3]);
            v[4] = _mm256_hadd_epi32(v[4], v[5]);
            v[6] = _mm256_hadd_epi32(v[6], v[7]);

            v[16] = _mm256_hadd_epi32(v[16], v[17]);
            v[18] = _mm256_hadd_epi32(v[18], v[19]);
            v[20] = _mm256_hadd_epi32(v[20], v[21]);
            v[22] = _mm256_hadd_epi32(v[22], v[23]);

            t[0] = _mm256_permute2x128_si256(v[0], v[16], 0x20);
            t[1] = _mm256_permute2x128_si256(v[0], v[16], 0x31);
            t[2] = _mm256_permute2x128_si256(v[2], v[18], 0x20);
            t[3] = _mm256_permute2x128_si256(v[2], v[18], 0x31);
            t[4] = _mm256_permute2x128_si256(v[4], v[20], 0x20);
            t[5] = _mm256_permute2x128_si256(v[4], v[20], 0x31);
            t[6] = _mm256_permute2x128_si256(v[6], v[22], 0x20);
            t[7] = _mm256_permute2x128_si256(v[6], v[22], 0x31);

            v[0] = _mm256_add_epi32(t[0], t[1]);
            v[1] = _mm256_add_epi32(t[2], t[3]);
            v[2] = _mm256_add_epi32(t[4], t[5]);
            v[3] = _mm256_add_epi32(t[6], t[7]);

            v[0] = _mm256_add_epi32(v[0], off);
            v[1] = _mm256_add_epi32(v[1], off);
            v[2] = _mm256_add_epi32(v[2], off);
            v[3] = _mm256_add_epi32(v[3], off);

            v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
            v[1] = _mm256_srai_epi32(v[1], shift);
            v[2] = _mm256_srai_epi32(v[2], shift);
            v[3] = _mm256_srai_epi32(v[3], shift);

            v[0] = _mm256_packs_epi32(v[0], v[1]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
            v[2] = _mm256_packs_epi32(v[2], v[3]);

            d4 = _mm256_castsi256_si128(v[0]);
            d5 = _mm256_extracti128_si256(v[0], 1);
            d6 = _mm256_castsi256_si128(v[2]);
            d7 = _mm256_extracti128_si256(v[2], 1);

            d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
            d1 = _mm_unpackhi_epi32(d4, d5);
            d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
            d3 = _mm_unpackhi_epi32(d6, d7);
            d4 = _mm_srli_si128(d0, 8);
            d5 = _mm_srli_si128(d1, 8);
            d6 = _mm_srli_si128(d2, 8);
            d7 = _mm_srli_si128(d3, 8);
            _mm_storel_epi64((__m128i*)(pdst), d0);
            _mm_storel_epi64((__m128i*)(pdst + 16), d4);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 2), d1);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 3), d5);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 4), d2);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 5), d6);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 6), d3);
            _mm_storel_epi64((__m128i*)(pdst + 16 * 7), d7);

            pdst += 16 * 8;
        }
        src += 16 * 4;
        dst += 4;
    }
}
