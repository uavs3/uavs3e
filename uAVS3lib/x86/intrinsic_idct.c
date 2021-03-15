#include "intrinsic.h"
#include "../transform.h"

ALIGNED_32(static const coef_t tab_idct_8x8[12][8]) =
{
	{ 44, 38, 44, 38, 44, 38, 44, 38 },
	{ 25, 9, 25, 9, 25, 9, 25, 9 },
	{ 38, -9, 38, -9, 38, -9, 38, -9 },
	{ -44, -25, -44, -25, -44, -25, -44, -25 },
	{ 25, -44, 25, -44, 25, -44, 25, -44 },
	{ 9, 38, 9, 38, 9, 38, 9, 38 },
	{ 9, -25, 9, -25, 9, -25, 9, -25 },
	{ 38, -44, 38, -44, 38, -44, 38, -44 },
	{ 32, 32, 32, 32, 32, 32, 32, 32 },
	{ 32, -32, 32, -32, 32, -32, 32, -32 },
	{ 42, 17, 42, 17, 42, 17, 42, 17 },
	{ 17, -42, 17, -42, 17, -42, 17, -42 }
};

void add_inv_trans_4x4_sec_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	__m128i	tmpSrc0, tmpSrc1, tmpSrc2, tmpSrc3, tmpLoad0, tmpLoad1, tmpLoad2, tmpLoad3;
	__m128i tmpC0, tmpC1, tmpC2, tmpC3;
	__m128i	tmpCoef0, tmpCoef1, tmpCoef2, tmpCoef3;
	__m128i tmpS0, tmpS1, tmpS2, tmpS3, tmpRes0, tmpRes1, tmpRes2, tmpRes3;
	__m128i tmpProduct0, tmpProduct1, tmpProduct2, tmpProduct3, tmpSum0, tmpSum1, tmpSum2, tmpSum3, tmpDst0, tmpDst1, tmpDst2, tmpDst3;
	__m128i m128iS0, m128iS1, P0, P1, P2, P3, D;
	__m128i zero = _mm_setzero_si128();
	__m128i factor, min_val, max_val;
	unsigned int *st0, *st1, *st2, *st3;

	/*---vertical transform first---*/

	//rnd_factor = 16; shift = 5
	factor = _mm_set1_epi32(16);

	//load src data
	tmpLoad0 = _mm_loadu_si128((__m128i*)&src[0]);
	tmpLoad1 = _mm_loadu_si128((__m128i*)&src[4]);
	tmpLoad2 = _mm_loadu_si128((__m128i*)&src[8]);
	tmpLoad3 = _mm_loadu_si128((__m128i*)&src[12]);
	tmpSrc0 = _mm_unpacklo_epi16(tmpLoad0, zero);
	tmpSrc1 = _mm_unpacklo_epi16(tmpLoad1, zero);
	tmpSrc2 = _mm_unpacklo_epi16(tmpLoad2, zero);
	tmpSrc3 = _mm_unpacklo_epi16(tmpLoad3, zero);

	//for i = 0
	//load coef data
	tmpC0 = _mm_set1_epi16(34);							//coef[0][0]
	tmpC1 = _mm_set1_epi16(77);							//coef[1][0]
	tmpC2 = _mm_set1_epi16(79);
	tmpC3 = _mm_set1_epi16(55);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);		
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 5), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 1
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(58);
	tmpC1 = _mm_set1_epi16(69);
	tmpC2 = _mm_set1_epi16(-33);
	tmpC3 = _mm_set1_epi16(-84);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 5), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 2
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(72);
	tmpC1 = _mm_set1_epi16(-7);
	tmpC2 = _mm_set1_epi16(-75);
	tmpC3 = _mm_set1_epi16(73);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 5), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 3
	//load tab_c4_trans data
	tmpC0 = _mm_set1_epi16(81);
	tmpC1 = _mm_set1_epi16(-75);
	tmpC2 = _mm_set1_epi16(58);
	tmpC3 = _mm_set1_epi16(-28);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 5), zero);				//!only low 64bits (4xSHORT) are valid!

	/*---hor transform---*/

	// shift = 14; rnd_factor = 8192
	factor = _mm_set1_epi32(8192);

	//load tab_c4_trans data, a matrix of 4x4
	tmpCoef0 = _mm_set_epi16(0, 81, 0, 72, 0, 58, 0, 34);
	tmpCoef1 = _mm_set_epi16(0, -75, 0, -7, 0, 69, 0, 77);
	tmpCoef2 = _mm_set_epi16(0, 58, 0, -75, 0, -33, 0, 79);
	tmpCoef3 = _mm_set_epi16(0, -28, 0, 73, 0, -84, 0, 55);

	/*-------for j = 0-------*/
	//load src data
    tmpS0 = _mm_set1_epi16(_mm_extract_epi16(tmpRes0, 0));							//tmpRes[0][0]
    tmpS1 = _mm_set1_epi16(_mm_extract_epi16(tmpRes0, 1));							//tmpRes[0][1]
    tmpS2 = _mm_set1_epi16(_mm_extract_epi16(tmpRes0, 2));
    tmpS3 = _mm_set1_epi16(_mm_extract_epi16(tmpRes0, 3));

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
	tmpDst0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 14), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 1-------*/
	//load src data
    tmpS0 = _mm_set1_epi16(_mm_extract_epi16(tmpRes1, 0));							//tmpRes[1][0]
    tmpS1 = _mm_set1_epi16(_mm_extract_epi16(tmpRes1, 1));							//tmpRes[1][1]
    tmpS2 = _mm_set1_epi16(_mm_extract_epi16(tmpRes1, 2));
    tmpS3 = _mm_set1_epi16(_mm_extract_epi16(tmpRes1, 3));
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
	tmpDst1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 14), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 2-------*/
	//load src data
    tmpS0 = _mm_set1_epi16(_mm_extract_epi16(tmpRes2, 0));							//tmpRes[2][0]
    tmpS1 = _mm_set1_epi16(_mm_extract_epi16(tmpRes2, 1));							//tmpRes[2][1]
    tmpS2 = _mm_set1_epi16(_mm_extract_epi16(tmpRes2, 2));
    tmpS3 = _mm_set1_epi16(_mm_extract_epi16(tmpRes2, 3));
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
	tmpDst2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 14), zero);			//!only low 64bits (4xSHORT) are valid!

	/*-------for j = 3-------*/
	//load src data
    tmpS0 = _mm_set1_epi16(_mm_extract_epi16(tmpRes3, 0));							//tmpRes[3][0]
    tmpS1 = _mm_set1_epi16(_mm_extract_epi16(tmpRes3, 1));							//tmpRes[3][1]
    tmpS2 = _mm_set1_epi16(_mm_extract_epi16(tmpRes3, 2));
    tmpS3 = _mm_set1_epi16(_mm_extract_epi16(tmpRes3, 3));
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
	tmpDst3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 14), zero);				//!only low 64bits (4xSHORT) are valid!

	//
	m128iS0 = _mm_unpacklo_epi64(tmpDst0, tmpDst1);
	m128iS1 = _mm_unpacklo_epi64(tmpDst2, tmpDst3);

	//clip
	max_val = _mm_set1_epi16(255);
	min_val = _mm_set1_epi16(-256);
	m128iS0 = _mm_min_epi16(m128iS0, max_val);
	m128iS0 = _mm_max_epi16(m128iS0, min_val);
	m128iS1 = _mm_min_epi16(m128iS1, max_val);
	m128iS1 = _mm_max_epi16(m128iS1, min_val);

	P0 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[0]), zero);
	P1 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred]), zero);
	P2 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred << 1]), zero);
	P3 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 3]), zero);

	P0 = _mm_unpacklo_epi64(P0, P1);
	P2 = _mm_unpacklo_epi64(P2, P3);

	m128iS0 = _mm_add_epi16(m128iS0, P0);
	m128iS1 = _mm_add_epi16(m128iS1, P2);

	D = _mm_packus_epi16(m128iS0, m128iS1);

	st0 = (unsigned int *)&dst[0];
	st1 = (unsigned int *)&dst[i_dst];
	st2 = (unsigned int *)&dst[i_dst << 1];
	st3 = (unsigned int *)&dst[i_dst * 3];

	*st0 = _mm_extract_epi32(D, 0);
	*st1 = _mm_extract_epi32(D, 1);
	*st2 = _mm_extract_epi32(D, 2);
	*st3 = _mm_extract_epi32(D, 3);
}

void add_inv_trans_8x8_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	__m128i m128iS0, m128iS1, m128iS2, m128iS3, m128iS4, m128iS5, m128iS6, m128iS7, m128iAdd, m128Tmp0, m128Tmp1, m128Tmp2, m128Tmp3, E0h, E1h, E2h, E3h, E0l, E1l, E2l, E3l, O0h, O1h, O2h, O3h, O0l, O1l, O2l, O3l, EE0l, EE1l, E00l, E01l, EE0h, EE1h, E00h, E01h;
	__m128i T00, T01, T02, T03, T04, T05, T06, T07;
	__m128i zero = _mm_setzero_si128();
	__m128i max_val,min_val;
	__m128i T10, T11, P0, P1, D;

	m128iAdd = _mm_set1_epi32(16);								// 首次反变换四舍五入的数字

	m128iS1 = _mm_loadu_si128((__m128i*)&src[8]);
	m128iS3 = _mm_loadu_si128((__m128i*)&src[24]);

	m128Tmp0 = _mm_unpacklo_epi16(m128iS1, m128iS3);
	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[0])));
	m128Tmp1 = _mm_unpackhi_epi16(m128iS1, m128iS3);
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[0])));


	m128iS5 = _mm_loadu_si128((__m128i*)&src[40]);
	m128iS7 = _mm_loadu_si128((__m128i*)&src[56]);

	m128Tmp2 = _mm_unpacklo_epi16(m128iS5, m128iS7);
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[1])));
	m128Tmp3 = _mm_unpackhi_epi16(m128iS5, m128iS7);
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[1])));
	O0l = _mm_add_epi32(E1l, E2l);
	O0h = _mm_add_epi32(E1h, E2h);

	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[2])));
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[2])));
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[3])));
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[3])));

	O1l = _mm_add_epi32(E1l, E2l);
	O1h = _mm_add_epi32(E1h, E2h);

	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[4])));
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[4])));
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[5])));
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[5])));
	O2l = _mm_add_epi32(E1l, E2l);
	O2h = _mm_add_epi32(E1h, E2h);

	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[6])));
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[6])));
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[7])));
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[7])));
	O3h = _mm_add_epi32(E1h, E2h);
	O3l = _mm_add_epi32(E1l, E2l);

	/*    -------     */


	m128iS0 = _mm_loadu_si128((__m128i*)&src[0]);
	m128iS4 = _mm_loadu_si128((__m128i*)&src[32]);

	m128Tmp0 = _mm_unpacklo_epi16(m128iS0, m128iS4);
	EE0l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[8])));
	m128Tmp1 = _mm_unpackhi_epi16(m128iS0, m128iS4);
	EE0h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[8])));

	EE1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[9])));
	EE1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[9])));

	/*    -------     */


	m128iS2 = _mm_loadu_si128((__m128i*)&src[16]);
	m128iS6 = _mm_loadu_si128((__m128i*)&src[48]);

	m128Tmp0 = _mm_unpacklo_epi16(m128iS2, m128iS6);
	E00l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[10])));
	m128Tmp1 = _mm_unpackhi_epi16(m128iS2, m128iS6);
	E00h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[10])));
	E01l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[11])));
	E01h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[11])));
	E0l = _mm_add_epi32(EE0l, E00l);
	E0l = _mm_add_epi32(E0l, m128iAdd);
	E0h = _mm_add_epi32(EE0h, E00h);
	E0h = _mm_add_epi32(E0h, m128iAdd);
	E3l = _mm_sub_epi32(EE0l, E00l);
	E3l = _mm_add_epi32(E3l, m128iAdd);
	E3h = _mm_sub_epi32(EE0h, E00h);
	E3h = _mm_add_epi32(E3h, m128iAdd);

	E1l = _mm_add_epi32(EE1l, E01l);
	E1l = _mm_add_epi32(E1l, m128iAdd);
	E1h = _mm_add_epi32(EE1h, E01h);
	E1h = _mm_add_epi32(E1h, m128iAdd);
	E2l = _mm_sub_epi32(EE1l, E01l);
	E2l = _mm_add_epi32(E2l, m128iAdd);
	E2h = _mm_sub_epi32(EE1h, E01h);
	E2h = _mm_add_epi32(E2h, m128iAdd);
	m128iS0 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E0l, O0l), 5), _mm_srai_epi32(_mm_add_epi32(E0h, O0h), 5));			// 首次反变换移位数
	m128iS7 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E0l, O0l), 5), _mm_srai_epi32(_mm_sub_epi32(E0h, O0h), 5));
	m128iS1 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E1l, O1l), 5), _mm_srai_epi32(_mm_add_epi32(E1h, O1h), 5));
	m128iS6 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E1l, O1l), 5), _mm_srai_epi32(_mm_sub_epi32(E1h, O1h), 5));
	m128iS2 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E2l, O2l), 5), _mm_srai_epi32(_mm_add_epi32(E2h, O2h), 5));
	m128iS5 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E2l, O2l), 5), _mm_srai_epi32(_mm_sub_epi32(E2h, O2h), 5));
	m128iS3 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E3l, O3l), 5), _mm_srai_epi32(_mm_add_epi32(E3h, O3h), 5));
	m128iS4 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E3l, O3l), 5), _mm_srai_epi32(_mm_sub_epi32(E3h, O3h), 5));

	/*  Invers matrix   */

	E0l = _mm_unpacklo_epi16(m128iS0, m128iS4);
	E1l = _mm_unpacklo_epi16(m128iS1, m128iS5);
	E2l = _mm_unpacklo_epi16(m128iS2, m128iS6);
	E3l = _mm_unpacklo_epi16(m128iS3, m128iS7);
	O0l = _mm_unpackhi_epi16(m128iS0, m128iS4);
	O1l = _mm_unpackhi_epi16(m128iS1, m128iS5);
	O2l = _mm_unpackhi_epi16(m128iS2, m128iS6);
	O3l = _mm_unpackhi_epi16(m128iS3, m128iS7);
	m128Tmp0 = _mm_unpacklo_epi16(E0l, E2l);
	m128Tmp1 = _mm_unpacklo_epi16(E1l, E3l);
	m128iS0 = _mm_unpacklo_epi16(m128Tmp0, m128Tmp1);
	m128iS1 = _mm_unpackhi_epi16(m128Tmp0, m128Tmp1);
	m128Tmp2 = _mm_unpackhi_epi16(E0l, E2l);
	m128Tmp3 = _mm_unpackhi_epi16(E1l, E3l);
	m128iS2 = _mm_unpacklo_epi16(m128Tmp2, m128Tmp3);
	m128iS3 = _mm_unpackhi_epi16(m128Tmp2, m128Tmp3);
	m128Tmp0 = _mm_unpacklo_epi16(O0l, O2l);
	m128Tmp1 = _mm_unpacklo_epi16(O1l, O3l);
	m128iS4 = _mm_unpacklo_epi16(m128Tmp0, m128Tmp1);
	m128iS5 = _mm_unpackhi_epi16(m128Tmp0, m128Tmp1);
	m128Tmp2 = _mm_unpackhi_epi16(O0l, O2l);
	m128Tmp3 = _mm_unpackhi_epi16(O1l, O3l);
	m128iS6 = _mm_unpacklo_epi16(m128Tmp2, m128Tmp3);
	m128iS7 = _mm_unpackhi_epi16(m128Tmp2, m128Tmp3);

	m128iAdd = _mm_set1_epi32(2048);						//设置四舍五入

	m128Tmp0 = _mm_unpacklo_epi16(m128iS1, m128iS3);
	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[0])));
	m128Tmp1 = _mm_unpackhi_epi16(m128iS1, m128iS3);
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[0])));
	m128Tmp2 = _mm_unpacklo_epi16(m128iS5, m128iS7);
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[1])));
	m128Tmp3 = _mm_unpackhi_epi16(m128iS5, m128iS7);
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[1])));
	O0l = _mm_add_epi32(E1l, E2l);
	O0h = _mm_add_epi32(E1h, E2h);
	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[2])));
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[2])));
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[3])));
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[3])));
	O1l = _mm_add_epi32(E1l, E2l);
	O1h = _mm_add_epi32(E1h, E2h);
	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[4])));
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[4])));
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[5])));
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[5])));
	O2l = _mm_add_epi32(E1l, E2l);
	O2h = _mm_add_epi32(E1h, E2h);
	E1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[6])));
	E1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[6])));
	E2l = _mm_madd_epi16(m128Tmp2, _mm_loadu_si128((__m128i*)(tab_idct_8x8[7])));
	E2h = _mm_madd_epi16(m128Tmp3, _mm_loadu_si128((__m128i*)(tab_idct_8x8[7])));
	O3h = _mm_add_epi32(E1h, E2h);
	O3l = _mm_add_epi32(E1l, E2l);

	m128Tmp0 = _mm_unpacklo_epi16(m128iS0, m128iS4);
	EE0l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[8])));
	m128Tmp1 = _mm_unpackhi_epi16(m128iS0, m128iS4);
	EE0h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[8])));
	EE1l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[9])));
	EE1h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[9])));

	m128Tmp0 = _mm_unpacklo_epi16(m128iS2, m128iS6);
	E00l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[10])));
	m128Tmp1 = _mm_unpackhi_epi16(m128iS2, m128iS6);
	E00h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[10])));
	E01l = _mm_madd_epi16(m128Tmp0, _mm_loadu_si128((__m128i*)(tab_idct_8x8[11])));
	E01h = _mm_madd_epi16(m128Tmp1, _mm_loadu_si128((__m128i*)(tab_idct_8x8[11])));
	E0l = _mm_add_epi32(EE0l, E00l);
	E0l = _mm_add_epi32(E0l, m128iAdd);
	E0h = _mm_add_epi32(EE0h, E00h);
	E0h = _mm_add_epi32(E0h, m128iAdd);
	E3l = _mm_sub_epi32(EE0l, E00l);
	E3l = _mm_add_epi32(E3l, m128iAdd);
	E3h = _mm_sub_epi32(EE0h, E00h);
	E3h = _mm_add_epi32(E3h, m128iAdd);
	E1l = _mm_add_epi32(EE1l, E01l);
	E1l = _mm_add_epi32(E1l, m128iAdd);
	E1h = _mm_add_epi32(EE1h, E01h);
	E1h = _mm_add_epi32(E1h, m128iAdd);
	E2l = _mm_sub_epi32(EE1l, E01l);
	E2l = _mm_add_epi32(E2l, m128iAdd);
	E2h = _mm_sub_epi32(EE1h, E01h);
	E2h = _mm_add_epi32(E2h, m128iAdd);

	m128iS0 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E0l, O0l), 12), _mm_srai_epi32(_mm_add_epi32(E0h, O0h), 12));
	m128iS7 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E0l, O0l), 12), _mm_srai_epi32(_mm_sub_epi32(E0h, O0h), 12));
	m128iS1 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E1l, O1l), 12), _mm_srai_epi32(_mm_add_epi32(E1h, O1h), 12));
	m128iS6 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E1l, O1l), 12), _mm_srai_epi32(_mm_sub_epi32(E1h, O1h), 12));
	m128iS2 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E2l, O2l), 12), _mm_srai_epi32(_mm_add_epi32(E2h, O2h), 12));
	m128iS5 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E2l, O2l), 12), _mm_srai_epi32(_mm_sub_epi32(E2h, O2h), 12));
	m128iS3 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E3l, O3l), 12), _mm_srai_epi32(_mm_add_epi32(E3h, O3h), 12));
	m128iS4 = _mm_packs_epi32(_mm_srai_epi32(_mm_sub_epi32(E3l, O3l), 12), _mm_srai_epi32(_mm_sub_epi32(E3h, O3h), 12));



	// [07 06 05 04 03 02 01 00]
	// [17 16 15 14 13 12 11 10]
	// [27 26 25 24 23 22 21 20]
	// [37 36 35 34 33 32 31 30]
	// [47 46 45 44 43 42 41 40]
	// [57 56 55 54 53 52 51 50]
	// [67 66 65 64 63 62 61 60]
	// [77 76 75 74 73 72 71 70]

	T00 = _mm_unpacklo_epi16(m128iS0, m128iS1);     // [13 03 12 02 11 01 10 00]
	
	T01 = _mm_unpackhi_epi16(m128iS0, m128iS1);     // [17 07 16 06 15 05 14 04]
	T02 = _mm_unpacklo_epi16(m128iS2, m128iS3);     // [33 23 32 22 31 21 30 20]
	
	T03 = _mm_unpackhi_epi16(m128iS2, m128iS3);     // [37 27 36 26 35 25 34 24]
	T04 = _mm_unpacklo_epi16(m128iS4, m128iS5);     // [53 43 52 42 51 41 50 40]
	
	T05 = _mm_unpackhi_epi16(m128iS4, m128iS5);     // [57 47 56 46 55 45 54 44]
	T06 = _mm_unpacklo_epi16(m128iS6, m128iS7);     // [73 63 72 62 71 61 70 60]
	
	T07 = _mm_unpackhi_epi16(m128iS6, m128iS7);     // [77 67 76 66 75 65 74 64]


	m128iS0 = _mm_unpacklo_epi32(T00, T02);                                     // [31 21 11 01 30 20 10 00]
	m128iS1 = _mm_unpackhi_epi32(T00, T02);                                     // [33 23 13 03 32 22 12 02]
	m128iS2 = _mm_unpacklo_epi32(T04, T06);                                     // [71 61 51 41 70 60 50 40]
	m128iS3 = _mm_unpackhi_epi32(T04, T06);                                     // [73 63 53 43 72 62 52 42]	
	m128iS4 = _mm_unpacklo_epi32(T01, T03);                                     // [35 25 15 05 34 24 14 04]
	m128iS5 = _mm_unpackhi_epi32(T01, T03);                                     // [37 27 17 07 36 26 16 06]
	m128iS6 = _mm_unpacklo_epi32(T05, T07);                                     // [75 65 55 45 74 64 54 44]
	m128iS7 = _mm_unpackhi_epi32(T05, T07);                                     // [77 67 57 47 76 66 56 46]



	//clip
	{
		max_val = _mm_set1_epi16(255);
		min_val = _mm_set1_epi16(-256);
		T00 = _mm_min_epi16(T00, max_val);
		T00 = _mm_max_epi16(T00, min_val);
		T01 = _mm_min_epi16(T01, max_val);
		T01 = _mm_max_epi16(T01, min_val);
		T02 = _mm_min_epi16(T02, max_val);
		T02 = _mm_max_epi16(T02, min_val);
		T03 = _mm_min_epi16(T03, max_val);
		T03 = _mm_max_epi16(T03, min_val);
		T04 = _mm_min_epi16(T04, max_val);
		T04 = _mm_max_epi16(T04, min_val);
		T05 = _mm_min_epi16(T05, max_val);
		T05 = _mm_max_epi16(T05, min_val);
		T06 = _mm_min_epi16(T06, max_val);
		T06 = _mm_max_epi16(T06, min_val);
		T07 = _mm_min_epi16(T07, max_val);
		T07 = _mm_max_epi16(T07, min_val);
	}

	{
		T10 = _mm_unpacklo_epi64(m128iS0, m128iS2);                                     // [70 60 50 40 30 20 10 00]
		T11 = _mm_unpackhi_epi64(m128iS0, m128iS2);                                     // [71 61 51 41 31 21 11 01]

		P0 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[0]), zero);
		P1 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred]), zero);

		D = _mm_packus_epi16(_mm_add_epi16(T10, P0), _mm_add_epi16(T11, P1));

		_mm_storel_epi64((__m128i*)&dst[0], D);                   // [70 60 50 40 30 20 10 00]
		_mm_storeh_pi((__m64*)&dst[i_dst], _mm_castsi128_ps(D));  // [71 61 51 41 31 21 11 01]



		T10 = _mm_unpacklo_epi64(m128iS1, m128iS3);                                     // [72 62 52 42 32 22 12 02]
		T11 = _mm_unpackhi_epi64(m128iS1, m128iS3);                                     // [73 63 53 43 33 23 13 03]
		P0 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 2]), zero);
		P1 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 3]), zero);
		D = _mm_packus_epi16(_mm_add_epi16(T10, P0), _mm_add_epi16(T11, P1));
		_mm_storel_epi64((__m128i*)&dst[i_dst*2], D);                   // [70 60 50 40 30 20 10 00]
		_mm_storeh_pi((__m64*)&dst[i_dst*3], _mm_castsi128_ps(D));  // [71 61 51 41 31 21 11 01]




		T10 = _mm_unpacklo_epi64(m128iS4, m128iS6);                                     // [74 64 54 44 34 24 14 04]
		T11 = _mm_unpackhi_epi64(m128iS4, m128iS6);                                     // [75 65 55 45 35 25 15 05]
		P0 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 4]), zero);
		P1 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 5]), zero);
		D = _mm_packus_epi16(_mm_add_epi16(T10, P0), _mm_add_epi16(T11, P1));
		_mm_storel_epi64((__m128i*)&dst[i_dst*4], D);                   // [70 60 50 40 30 20 10 00]
		_mm_storeh_pi((__m64*)&dst[i_dst*5], _mm_castsi128_ps(D));  // [71 61 51 41 31 21 11 01]


		T10 = _mm_unpacklo_epi64(m128iS5, m128iS7);                                     // [76 66 56 46 36 26 16 06]
		T11 = _mm_unpackhi_epi64(m128iS5, m128iS7);                                     // [77 67 57 47 37 27 17 07]
		P0 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 6]), zero);
		P1 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 7]), zero);
		D = _mm_packus_epi16(_mm_add_epi16(T10, P0), _mm_add_epi16(T11, P1));
		_mm_storel_epi64((__m128i*)&dst[i_dst * 6], D);                   // [70 60 50 40 30 20 10 00]
		_mm_storeh_pi((__m64*)&dst[i_dst * 7], _mm_castsi128_ps(D));  // [71 61 51 41 31 21 11 01]

	}
}

void add_inv_trans_16x16_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    __m128i P00, P01, P02, P03, P04, P05, P06, P07, P08, P09, P10, P11, P12, P13, P14, P15;
	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);		//row0 87high - 90low address
	const __m128i c16_p35_p40 = _mm_set1_epi32(0x00230028);
	const __m128i c16_p21_p29 = _mm_set1_epi32(0x0015001D);
	const __m128i c16_p04_p13 = _mm_set1_epi32(0x0004000D);
	const __m128i c16_p29_p43 = _mm_set1_epi32(0x001D002B);		//row1
	const __m128i c16_n21_p04 = _mm_set1_epi32(0xFFEB0004);
	const __m128i c16_n45_n40 = _mm_set1_epi32(0xFFD3FFD8);
	const __m128i c16_n13_n35 = _mm_set1_epi32(0xFFF3FFDD);
	const __m128i c16_p04_p40 = _mm_set1_epi32(0x00040028);		//row2
	const __m128i c16_n43_n35 = _mm_set1_epi32(0xFFD5FFDD);
	const __m128i c16_p29_n13 = _mm_set1_epi32(0x001DFFF3);
	const __m128i c16_p21_p45 = _mm_set1_epi32(0x0015002D);
	const __m128i c16_n21_p35 = _mm_set1_epi32(0xFFEB0023);		//row3
	const __m128i c16_p04_n43 = _mm_set1_epi32(0x0004FFD5);
	const __m128i c16_p13_p45 = _mm_set1_epi32(0x000D002D);
	const __m128i c16_n29_n40 = _mm_set1_epi32(0xFFE3FFD8);
	const __m128i c16_n40_p29 = _mm_set1_epi32(0xFFD8001D);		//row4
	const __m128i c16_p45_n13 = _mm_set1_epi32(0x002DFFF3);
	const __m128i c16_n43_n04 = _mm_set1_epi32(0xFFD5FFFC);
	const __m128i c16_p35_p21 = _mm_set1_epi32(0x00230015);
	const __m128i c16_n45_p21 = _mm_set1_epi32(0xFFD30015);		//row5
	const __m128i c16_p13_p29 = _mm_set1_epi32(0x000D001D);
	const __m128i c16_p35_n43 = _mm_set1_epi32(0x0023FFD5);
	const __m128i c16_n40_p04 = _mm_set1_epi32(0xFFD80004);
	const __m128i c16_n35_p13 = _mm_set1_epi32(0xFFDD000D);		//row6
	const __m128i c16_n40_p45 = _mm_set1_epi32(0xFFD8002D);
	const __m128i c16_p04_p21 = _mm_set1_epi32(0x00040015);
	const __m128i c16_p43_n29 = _mm_set1_epi32(0x002BFFE3);
	const __m128i c16_n13_p04 = _mm_set1_epi32(0xFFF30004);		//row7
	const __m128i c16_n29_p21 = _mm_set1_epi32(0xFFE30015);
	const __m128i c16_n40_p35 = _mm_set1_epi32(0xFFD80023);
	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);

	const __m128i c16_p38_p44 = _mm_set1_epi32(0x0026002C);
	const __m128i c16_p09_p25 = _mm_set1_epi32(0x00090019);
	const __m128i c16_n09_p38 = _mm_set1_epi32(0xFFF70026);
	const __m128i c16_n25_n44 = _mm_set1_epi32(0xFFE7FFD4);
	const __m128i c16_n44_p25 = _mm_set1_epi32(0xFFD40019);
	const __m128i c16_p38_p09 = _mm_set1_epi32(0x00260009);
	const __m128i c16_n25_p09 = _mm_set1_epi32(0xFFE70009);
	const __m128i c16_n44_p38 = _mm_set1_epi32(0xFFD40026);

	const __m128i c16_p17_p42 = _mm_set1_epi32(0x0011002A);
	const __m128i c16_n42_p17 = _mm_set1_epi32(0xFFD60011);

	const __m128i c16_n32_p32 = _mm_set1_epi32(0xFFE00020);
	const __m128i c16_p32_p32 = _mm_set1_epi32(0x00200020);

	__m128i zero = _mm_setzero_si128();

	int i, pass, part;

	__m128i c32_rnd = _mm_set1_epi32(16);								// 第一次四舍五入

	int nShift = 5;

	// DCT1
	__m128i in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2];
	__m128i in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2];
	__m128i res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2];
	__m128i res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2];

	for (i = 0; i < 2; i++)
	{
		const int offset = (i << 3);

		in00[i] = _mm_loadu_si128((const __m128i*)&src[0 * 16 + offset]);	// [07 06 05 04 03 02 01 00]
		in01[i] = _mm_loadu_si128((const __m128i*)&src[1 * 16 + offset]);  // [17 16 15 14 13 12 11 10]
		in02[i] = _mm_loadu_si128((const __m128i*)&src[2 * 16 + offset]); // [27 26 25 24 23 22 21 20]
		in03[i] = _mm_loadu_si128((const __m128i*)&src[3 * 16 + offset]);     // [37 36 35 34 33 32 31 30]
		in04[i] = _mm_loadu_si128((const __m128i*)&src[4 * 16 + offset]);   // [47 46 45 44 43 42 41 40]
		in05[i] = _mm_loadu_si128((const __m128i*)&src[5 * 16 + offset]);     // [57 56 55 54 53 52 51 50]
		in06[i] = _mm_loadu_si128((const __m128i*)&src[6 * 16 + offset]);    // [67 66 65 64 63 62 61 60]
		in07[i] = _mm_loadu_si128((const __m128i*)&src[7 * 16 + offset]);    // [77 76 75 74 73 72 71 70]
		in08[i] = _mm_loadu_si128((const __m128i*)&src[8 * 16 + offset]);
		in09[i] = _mm_loadu_si128((const __m128i*)&src[9 * 16 + offset]);
		in10[i] = _mm_loadu_si128((const __m128i*)&src[10 * 16 + offset]);
		in11[i] = _mm_loadu_si128((const __m128i*)&src[11 * 16 + offset]);
		in12[i] = _mm_loadu_si128((const __m128i*)&src[12 * 16 + offset]);
		in13[i] = _mm_loadu_si128((const __m128i*)&src[13 * 16 + offset]);
		in14[i] = _mm_loadu_si128((const __m128i*)&src[14 * 16 + offset]);
		in15[i] = _mm_loadu_si128((const __m128i*)&src[15 * 16 + offset]);
	}

	for (pass = 0; pass < 2; pass++)
	{
		if (pass == 1)
		{
			c32_rnd = _mm_set1_epi32(2048);				// pass == 1 第二次四舍五入
			nShift = 12;
		}

		for (part = 0; part < 2; part++)
		{
			const __m128i T_00_00A = _mm_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
			const __m128i T_00_00B = _mm_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
			const __m128i T_00_01A = _mm_unpacklo_epi16(in05[part], in07[part]);       // [ ]
			const __m128i T_00_01B = _mm_unpackhi_epi16(in05[part], in07[part]);       // [ ]
			const __m128i T_00_02A = _mm_unpacklo_epi16(in09[part], in11[part]);       // [ ]
			const __m128i T_00_02B = _mm_unpackhi_epi16(in09[part], in11[part]);       // [ ]
			const __m128i T_00_03A = _mm_unpacklo_epi16(in13[part], in15[part]);       // [ ]
			const __m128i T_00_03B = _mm_unpackhi_epi16(in13[part], in15[part]);       // [ ]
			const __m128i T_00_04A = _mm_unpacklo_epi16(in02[part], in06[part]);       // [ ]
			const __m128i T_00_04B = _mm_unpackhi_epi16(in02[part], in06[part]);       // [ ]
			const __m128i T_00_05A = _mm_unpacklo_epi16(in10[part], in14[part]);       // [ ]
			const __m128i T_00_05B = _mm_unpackhi_epi16(in10[part], in14[part]);       // [ ]
			const __m128i T_00_06A = _mm_unpacklo_epi16(in04[part], in12[part]);       // [ ]row
			const __m128i T_00_06B = _mm_unpackhi_epi16(in04[part], in12[part]);       // [ ]
			const __m128i T_00_07A = _mm_unpacklo_epi16(in00[part], in08[part]);       // [83 03 82 02 81 01 81 00] row08 row00
			const __m128i T_00_07B = _mm_unpackhi_epi16(in00[part], in08[part]);       // [87 07 86 06 85 05 84 04]

			__m128i O0A, O1A, O2A, O3A, O4A, O5A, O6A, O7A;
			__m128i O0B, O1B, O2B, O3B, O4B, O5B, O6B, O7B;
			__m128i EO0A, EO1A, EO2A, EO3A;
			__m128i EO0B, EO1B, EO2B, EO3B;
			__m128i EEO0A, EEO1A;
			__m128i EEO0B, EEO1B;
			__m128i EEE0A, EEE1A;
			__m128i EEE0B, EEE1B;
			__m128i T00, T01;
#define COMPUTE_ROW(row0103, row0507, row0911, row1315, c0103, c0507, c0911, c1315, row) \
	T00 = _mm_add_epi32(_mm_madd_epi16(row0103, c0103), _mm_madd_epi16(row0507, c0507)); \
	T01 = _mm_add_epi32(_mm_madd_epi16(row0911, c0911), _mm_madd_epi16(row1315, c1315)); \
	row = _mm_add_epi32(T00, T01);

			COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, O0A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, O1A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, O2A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, O3A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, O4A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, O5A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, O6A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, O7A)

				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, O0B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, O1B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, O2B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, O3B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, O4B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, O5B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, O6B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, O7B)
#undef COMPUTE_ROW



				EO0A = _mm_add_epi32(_mm_madd_epi16(T_00_04A, c16_p38_p44), _mm_madd_epi16(T_00_05A, c16_p09_p25)); // EO0
			EO0B = _mm_add_epi32(_mm_madd_epi16(T_00_04B, c16_p38_p44), _mm_madd_epi16(T_00_05B, c16_p09_p25));
			EO1A = _mm_add_epi32(_mm_madd_epi16(T_00_04A, c16_n09_p38), _mm_madd_epi16(T_00_05A, c16_n25_n44)); // EO1
			EO1B = _mm_add_epi32(_mm_madd_epi16(T_00_04B, c16_n09_p38), _mm_madd_epi16(T_00_05B, c16_n25_n44));
			EO2A = _mm_add_epi32(_mm_madd_epi16(T_00_04A, c16_n44_p25), _mm_madd_epi16(T_00_05A, c16_p38_p09)); // EO2
			EO2B = _mm_add_epi32(_mm_madd_epi16(T_00_04B, c16_n44_p25), _mm_madd_epi16(T_00_05B, c16_p38_p09));
			EO3A = _mm_add_epi32(_mm_madd_epi16(T_00_04A, c16_n25_p09), _mm_madd_epi16(T_00_05A, c16_n44_p38)); // EO3
			EO3B = _mm_add_epi32(_mm_madd_epi16(T_00_04B, c16_n25_p09), _mm_madd_epi16(T_00_05B, c16_n44_p38));


			EEO0A = _mm_madd_epi16(T_00_06A, c16_p17_p42);
			EEO0B = _mm_madd_epi16(T_00_06B, c16_p17_p42);
			EEO1A = _mm_madd_epi16(T_00_06A, c16_n42_p17);
			EEO1B = _mm_madd_epi16(T_00_06B, c16_n42_p17);


			EEE0A = _mm_madd_epi16(T_00_07A, c16_p32_p32);
			EEE0B = _mm_madd_epi16(T_00_07B, c16_p32_p32);
			EEE1A = _mm_madd_epi16(T_00_07A, c16_n32_p32);
			EEE1B = _mm_madd_epi16(T_00_07B, c16_n32_p32);

			{
				const __m128i EE0A = _mm_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
				const __m128i EE0B = _mm_add_epi32(EEE0B, EEO0B);
				const __m128i EE1A = _mm_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
				const __m128i EE1B = _mm_add_epi32(EEE1B, EEO1B);
				const __m128i EE3A = _mm_sub_epi32(EEE0A, EEO0A);          // EE2 = EEE0 - EEO0
				const __m128i EE3B = _mm_sub_epi32(EEE0B, EEO0B);
				const __m128i EE2A = _mm_sub_epi32(EEE1A, EEO1A);          // EE3 = EEE1 - EEO1
				const __m128i EE2B = _mm_sub_epi32(EEE1B, EEO1B);

				const __m128i E0A = _mm_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
				const __m128i E0B = _mm_add_epi32(EE0B, EO0B);
				const __m128i E1A = _mm_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
				const __m128i E1B = _mm_add_epi32(EE1B, EO1B);
				const __m128i E2A = _mm_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
				const __m128i E2B = _mm_add_epi32(EE2B, EO2B);
				const __m128i E3A = _mm_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
				const __m128i E3B = _mm_add_epi32(EE3B, EO3B);
				const __m128i E7A = _mm_sub_epi32(EE0A, EO0A);          // E0 = EE0 - EO0
				const __m128i E7B = _mm_sub_epi32(EE0B, EO0B);
				const __m128i E6A = _mm_sub_epi32(EE1A, EO1A);          // E1 = EE1 - EO1
				const __m128i E6B = _mm_sub_epi32(EE1B, EO1B);
				const __m128i E5A = _mm_sub_epi32(EE2A, EO2A);          // E2 = EE2 - EO2
				const __m128i E5B = _mm_sub_epi32(EE2B, EO2B);
				const __m128i E4A = _mm_sub_epi32(EE3A, EO3A);          // E3 = EE3 - EO3
				const __m128i E4B = _mm_sub_epi32(EE3B, EO3B);

				const __m128i T10A = _mm_add_epi32(E0A, c32_rnd);         // E0 + rnd
				const __m128i T10B = _mm_add_epi32(E0B, c32_rnd);
				const __m128i T11A = _mm_add_epi32(E1A, c32_rnd);         // E1 + rnd
				const __m128i T11B = _mm_add_epi32(E1B, c32_rnd);
				const __m128i T12A = _mm_add_epi32(E2A, c32_rnd);         // E2 + rnd
				const __m128i T12B = _mm_add_epi32(E2B, c32_rnd);
				const __m128i T13A = _mm_add_epi32(E3A, c32_rnd);         // E3 + rnd
				const __m128i T13B = _mm_add_epi32(E3B, c32_rnd);
				const __m128i T14A = _mm_add_epi32(E4A, c32_rnd);         // E4 + rnd
				const __m128i T14B = _mm_add_epi32(E4B, c32_rnd);
				const __m128i T15A = _mm_add_epi32(E5A, c32_rnd);         // E5 + rnd
				const __m128i T15B = _mm_add_epi32(E5B, c32_rnd);
				const __m128i T16A = _mm_add_epi32(E6A, c32_rnd);         // E6 + rnd
				const __m128i T16B = _mm_add_epi32(E6B, c32_rnd);
				const __m128i T17A = _mm_add_epi32(E7A, c32_rnd);         // E7 + rnd
				const __m128i T17B = _mm_add_epi32(E7B, c32_rnd);

				const __m128i T20A = _mm_add_epi32(T10A, O0A);          // E0 + O0 + rnd
				const __m128i T20B = _mm_add_epi32(T10B, O0B);
				const __m128i T21A = _mm_add_epi32(T11A, O1A);          // E1 + O1 + rnd
				const __m128i T21B = _mm_add_epi32(T11B, O1B);
				const __m128i T22A = _mm_add_epi32(T12A, O2A);          // E2 + O2 + rnd
				const __m128i T22B = _mm_add_epi32(T12B, O2B);
				const __m128i T23A = _mm_add_epi32(T13A, O3A);          // E3 + O3 + rnd
				const __m128i T23B = _mm_add_epi32(T13B, O3B);
				const __m128i T24A = _mm_add_epi32(T14A, O4A);          // E4
				const __m128i T24B = _mm_add_epi32(T14B, O4B);
				const __m128i T25A = _mm_add_epi32(T15A, O5A);          // E5
				const __m128i T25B = _mm_add_epi32(T15B, O5B);
				const __m128i T26A = _mm_add_epi32(T16A, O6A);          // E6
				const __m128i T26B = _mm_add_epi32(T16B, O6B);
				const __m128i T27A = _mm_add_epi32(T17A, O7A);          // E7
				const __m128i T27B = _mm_add_epi32(T17B, O7B);
				const __m128i T2FA = _mm_sub_epi32(T10A, O0A);          // E0 - O0 + rnd
				const __m128i T2FB = _mm_sub_epi32(T10B, O0B);
				const __m128i T2EA = _mm_sub_epi32(T11A, O1A);          // E1 - O1 + rnd
				const __m128i T2EB = _mm_sub_epi32(T11B, O1B);
				const __m128i T2DA = _mm_sub_epi32(T12A, O2A);          // E2 - O2 + rnd
				const __m128i T2DB = _mm_sub_epi32(T12B, O2B);
				const __m128i T2CA = _mm_sub_epi32(T13A, O3A);          // E3 - O3 + rnd
				const __m128i T2CB = _mm_sub_epi32(T13B, O3B);
				const __m128i T2BA = _mm_sub_epi32(T14A, O4A);          // E4
				const __m128i T2BB = _mm_sub_epi32(T14B, O4B);
				const __m128i T2AA = _mm_sub_epi32(T15A, O5A);          // E5
				const __m128i T2AB = _mm_sub_epi32(T15B, O5B);
				const __m128i T29A = _mm_sub_epi32(T16A, O6A);          // E6
				const __m128i T29B = _mm_sub_epi32(T16B, O6B);
				const __m128i T28A = _mm_sub_epi32(T17A, O7A);          // E7
				const __m128i T28B = _mm_sub_epi32(T17B, O7B);

				const __m128i T30A = _mm_srai_epi32(T20A, nShift);             // [30 20 10 00]
				const __m128i T30B = _mm_srai_epi32(T20B, nShift);             // [70 60 50 40]
				const __m128i T31A = _mm_srai_epi32(T21A, nShift);             // [31 21 11 01]
				const __m128i T31B = _mm_srai_epi32(T21B, nShift);             // [71 61 51 41]
				const __m128i T32A = _mm_srai_epi32(T22A, nShift);             // [32 22 12 02]
				const __m128i T32B = _mm_srai_epi32(T22B, nShift);             // [72 62 52 42]
				const __m128i T33A = _mm_srai_epi32(T23A, nShift);             // [33 23 13 03]
				const __m128i T33B = _mm_srai_epi32(T23B, nShift);             // [73 63 53 43]
				const __m128i T34A = _mm_srai_epi32(T24A, nShift);             // [33 24 14 04]
				const __m128i T34B = _mm_srai_epi32(T24B, nShift);             // [74 64 54 44]
				const __m128i T35A = _mm_srai_epi32(T25A, nShift);             // [35 25 15 05]
				const __m128i T35B = _mm_srai_epi32(T25B, nShift);             // [75 65 55 45]
				const __m128i T36A = _mm_srai_epi32(T26A, nShift);             // [36 26 16 06]
				const __m128i T36B = _mm_srai_epi32(T26B, nShift);             // [76 66 56 46]
				const __m128i T37A = _mm_srai_epi32(T27A, nShift);             // [37 27 17 07]
				const __m128i T37B = _mm_srai_epi32(T27B, nShift);             // [77 67 57 47]

				const __m128i T38A = _mm_srai_epi32(T28A, nShift);             // [30 20 10 00] x8
				const __m128i T38B = _mm_srai_epi32(T28B, nShift);             // [70 60 50 40]
				const __m128i T39A = _mm_srai_epi32(T29A, nShift);             // [31 21 11 01] x9
				const __m128i T39B = _mm_srai_epi32(T29B, nShift);             // [71 61 51 41]
				const __m128i T3AA = _mm_srai_epi32(T2AA, nShift);             // [32 22 12 02] xA
				const __m128i T3AB = _mm_srai_epi32(T2AB, nShift);             // [72 62 52 42]
				const __m128i T3BA = _mm_srai_epi32(T2BA, nShift);             // [33 23 13 03] xB
				const __m128i T3BB = _mm_srai_epi32(T2BB, nShift);             // [73 63 53 43]
				const __m128i T3CA = _mm_srai_epi32(T2CA, nShift);             // [33 24 14 04] xC
				const __m128i T3CB = _mm_srai_epi32(T2CB, nShift);             // [74 64 54 44]
				const __m128i T3DA = _mm_srai_epi32(T2DA, nShift);             // [35 25 15 05] xD
				const __m128i T3DB = _mm_srai_epi32(T2DB, nShift);             // [75 65 55 45]
				const __m128i T3EA = _mm_srai_epi32(T2EA, nShift);             // [36 26 16 06] xE
				const __m128i T3EB = _mm_srai_epi32(T2EB, nShift);             // [76 66 56 46]
				const __m128i T3FA = _mm_srai_epi32(T2FA, nShift);             // [37 27 17 07] xF
				const __m128i T3FB = _mm_srai_epi32(T2FB, nShift);             // [77 67 57 47]

				res00[part] = _mm_packs_epi32(T30A, T30B);        // [70 60 50 40 30 20 10 00]
				res01[part] = _mm_packs_epi32(T31A, T31B);        // [71 61 51 41 31 21 11 01]
				res02[part] = _mm_packs_epi32(T32A, T32B);        // [72 62 52 42 32 22 12 02]
				res03[part] = _mm_packs_epi32(T33A, T33B);        // [73 63 53 43 33 23 13 03]
				res04[part] = _mm_packs_epi32(T34A, T34B);        // [74 64 54 44 34 24 14 04]
				res05[part] = _mm_packs_epi32(T35A, T35B);        // [75 65 55 45 35 25 15 05]
				res06[part] = _mm_packs_epi32(T36A, T36B);        // [76 66 56 46 36 26 16 06]
				res07[part] = _mm_packs_epi32(T37A, T37B);        // [77 67 57 47 37 27 17 07]

				res08[part] = _mm_packs_epi32(T38A, T38B);        // [A0 ... 80]
				res09[part] = _mm_packs_epi32(T39A, T39B);        // [A1 ... 81]
				res10[part] = _mm_packs_epi32(T3AA, T3AB);        // [A2 ... 82]
				res11[part] = _mm_packs_epi32(T3BA, T3BB);        // [A3 ... 83]
				res12[part] = _mm_packs_epi32(T3CA, T3CB);        // [A4 ... 84]
				res13[part] = _mm_packs_epi32(T3DA, T3DB);        // [A5 ... 85]
				res14[part] = _mm_packs_epi32(T3EA, T3EB);        // [A6 ... 86]
				res15[part] = _mm_packs_epi32(T3FA, T3FB);        // [A7 ... 87]
			}
		}
		//transpose matrix 8x8 16bit.
		{
			__m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
			__m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
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

			TRANSPOSE_8x8_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0])
				TRANSPOSE_8x8_16BIT(res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1])
				TRANSPOSE_8x8_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0])
				TRANSPOSE_8x8_16BIT(res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1])

#undef TRANSPOSE_8x8_16BIT
		}
	}

	//clip
	{
		__m128i max_val = _mm_set1_epi16(255);
		__m128i min_val = _mm_set1_epi16(-256);
		in00[0] = _mm_min_epi16(in00[0], max_val);
		in00[0] = _mm_max_epi16(in00[0], min_val);
		in00[1] = _mm_min_epi16(in00[1], max_val);
		in00[1] = _mm_max_epi16(in00[1], min_val);

		in01[0] = _mm_min_epi16(in01[0], max_val);
		in01[0] = _mm_max_epi16(in01[0], min_val);
		in01[1] = _mm_min_epi16(in01[1], max_val);
		in01[1] = _mm_max_epi16(in01[1], min_val);

		in02[0] = _mm_min_epi16(in02[0], max_val);
		in02[0] = _mm_max_epi16(in02[0], min_val);
		in02[1] = _mm_min_epi16(in02[1], max_val);
		in02[1] = _mm_max_epi16(in02[1], min_val);

		in03[0] = _mm_min_epi16(in03[0], max_val);
		in03[0] = _mm_max_epi16(in03[0], min_val);
		in03[1] = _mm_min_epi16(in03[1], max_val);
		in03[1] = _mm_max_epi16(in03[1], min_val);

		in04[0] = _mm_min_epi16(in04[0], max_val);
		in04[0] = _mm_max_epi16(in04[0], min_val);
		in04[1] = _mm_min_epi16(in04[1], max_val);
		in04[1] = _mm_max_epi16(in04[1], min_val);

		in05[0] = _mm_min_epi16(in05[0], max_val);
		in05[0] = _mm_max_epi16(in05[0], min_val);
		in05[1] = _mm_min_epi16(in05[1], max_val);
		in05[1] = _mm_max_epi16(in05[1], min_val);

		in06[0] = _mm_min_epi16(in06[0], max_val);
		in06[0] = _mm_max_epi16(in06[0], min_val);
		in06[1] = _mm_min_epi16(in06[1], max_val);
		in06[1] = _mm_max_epi16(in06[1], min_val);

		in07[0] = _mm_min_epi16(in07[0], max_val);
		in07[0] = _mm_max_epi16(in07[0], min_val);
		in07[1] = _mm_min_epi16(in07[1], max_val);
		in07[1] = _mm_max_epi16(in07[1], min_val);

		in08[0] = _mm_min_epi16(in08[0], max_val);
		in08[0] = _mm_max_epi16(in08[0], min_val);
		in08[1] = _mm_min_epi16(in08[1], max_val);
		in08[1] = _mm_max_epi16(in08[1], min_val);

		in09[0] = _mm_min_epi16(in09[0], max_val);
		in09[0] = _mm_max_epi16(in09[0], min_val);
		in09[1] = _mm_min_epi16(in09[1], max_val);
		in09[1] = _mm_max_epi16(in09[1], min_val);

		in10[0] = _mm_min_epi16(in10[0], max_val);
		in10[0] = _mm_max_epi16(in10[0], min_val);
		in10[1] = _mm_min_epi16(in10[1], max_val);
		in10[1] = _mm_max_epi16(in10[1], min_val);

		in11[0] = _mm_min_epi16(in11[0], max_val);
		in11[0] = _mm_max_epi16(in11[0], min_val);
		in11[1] = _mm_min_epi16(in11[1], max_val);
		in11[1] = _mm_max_epi16(in11[1], min_val);

		in12[0] = _mm_min_epi16(in12[0], max_val);
		in12[0] = _mm_max_epi16(in12[0], min_val);
		in12[1] = _mm_min_epi16(in12[1], max_val);
		in12[1] = _mm_max_epi16(in12[1], min_val);

		in13[0] = _mm_min_epi16(in13[0], max_val);
		in13[0] = _mm_max_epi16(in13[0], min_val);
		in13[1] = _mm_min_epi16(in13[1], max_val);
		in13[1] = _mm_max_epi16(in13[1], min_val);

		in14[0] = _mm_min_epi16(in14[0], max_val);
		in14[0] = _mm_max_epi16(in14[0], min_val);
		in14[1] = _mm_min_epi16(in14[1], max_val);
		in14[1] = _mm_max_epi16(in14[1], min_val);

		in15[0] = _mm_min_epi16(in15[0], max_val);
		in15[0] = _mm_max_epi16(in15[0], min_val);
		in15[1] = _mm_min_epi16(in15[1], max_val);
		in15[1] = _mm_max_epi16(in15[1], min_val);
	}


	

	P00 = _mm_loadu_si128((const __m128i*)&pred[0]);
	P01 = _mm_loadu_si128((const __m128i*)&pred[1 * i_pred]);
	P02 = _mm_loadu_si128((const __m128i*)&pred[2 * i_pred]);
	P03 = _mm_loadu_si128((const __m128i*)&pred[3 * i_pred]);
	P04 = _mm_loadu_si128((const __m128i*)&pred[4 * i_pred]);
	P05 = _mm_loadu_si128((const __m128i*)&pred[5 * i_pred]);
	P06 = _mm_loadu_si128((const __m128i*)&pred[6 * i_pred]);
	P07 = _mm_loadu_si128((const __m128i*)&pred[7 * i_pred]);
	P08 = _mm_loadu_si128((const __m128i*)&pred[8 * i_pred]);
	P09 = _mm_loadu_si128((const __m128i*)&pred[9 * i_pred]);
	P10 = _mm_loadu_si128((const __m128i*)&pred[10 * i_pred]);
	P11 = _mm_loadu_si128((const __m128i*)&pred[11 * i_pred]);
	P12 = _mm_loadu_si128((const __m128i*)&pred[12 * i_pred]);
	P13 = _mm_loadu_si128((const __m128i*)&pred[13 * i_pred]);
	P14 = _mm_loadu_si128((const __m128i*)&pred[14 * i_pred]);
	P15 = _mm_loadu_si128((const __m128i*)&pred[15 * i_pred]);

	res00[0] = _mm_unpacklo_epi8(P00, zero);
	res00[1] = _mm_unpackhi_epi8(P00, zero);
	res01[0] = _mm_unpacklo_epi8(P01, zero);
	res01[1] = _mm_unpackhi_epi8(P01, zero);
	res02[0] = _mm_unpacklo_epi8(P02, zero);
	res02[1] = _mm_unpackhi_epi8(P02, zero);	
	res03[0] = _mm_unpacklo_epi8(P03, zero);
	res03[1] = _mm_unpackhi_epi8(P03, zero);	
	res04[0] = _mm_unpacklo_epi8(P04, zero);
	res04[1] = _mm_unpackhi_epi8(P04, zero);	
	res05[0] = _mm_unpacklo_epi8(P05, zero);
	res05[1] = _mm_unpackhi_epi8(P05, zero);	
	res06[0] = _mm_unpacklo_epi8(P06, zero);
	res06[1] = _mm_unpackhi_epi8(P06, zero);
	res07[0] = _mm_unpacklo_epi8(P07, zero);
	res07[1] = _mm_unpackhi_epi8(P07, zero);	
	res08[0] = _mm_unpacklo_epi8(P08, zero);
	res08[1] = _mm_unpackhi_epi8(P08, zero);	
	res09[0] = _mm_unpacklo_epi8(P09, zero);
	res09[1] = _mm_unpackhi_epi8(P09, zero);	
	res10[0] = _mm_unpacklo_epi8(P10, zero);
	res10[1] = _mm_unpackhi_epi8(P10, zero);
	res11[0] = _mm_unpacklo_epi8(P11, zero);
	res11[1] = _mm_unpackhi_epi8(P11, zero);
	res12[0] = _mm_unpacklo_epi8(P12, zero);
	res12[1] = _mm_unpackhi_epi8(P12, zero);
	res13[0] = _mm_unpacklo_epi8(P13, zero);
	res13[1] = _mm_unpackhi_epi8(P13, zero);
	res14[0] = _mm_unpacklo_epi8(P14, zero);
	res14[1] = _mm_unpackhi_epi8(P14, zero);
	res15[0] = _mm_unpacklo_epi8(P15, zero);
	res15[1] = _mm_unpackhi_epi8(P15, zero);

	res00[0] = _mm_add_epi16(in00[0], res00[0]);
	res00[1] = _mm_add_epi16(in00[1], res00[1]);
	res01[0] = _mm_add_epi16(in01[0], res01[0]);
	res01[1] = _mm_add_epi16(in01[1], res01[1]);
	res02[0] = _mm_add_epi16(in02[0], res02[0]);
	res02[1] = _mm_add_epi16(in02[1], res02[1]);
	res03[0] = _mm_add_epi16(in03[0], res03[0]);
	res03[1] = _mm_add_epi16(in03[1], res03[1]);
	res04[0] = _mm_add_epi16(in04[0], res04[0]);
	res04[1] = _mm_add_epi16(in04[1], res04[1]);
	res05[0] = _mm_add_epi16(in05[0], res05[0]);
	res05[1] = _mm_add_epi16(in05[1], res05[1]);
	res06[0] = _mm_add_epi16(in06[0], res06[0]);
	res06[1] = _mm_add_epi16(in06[1], res06[1]);
	res07[0] = _mm_add_epi16(in07[0], res07[0]);
	res07[1] = _mm_add_epi16(in07[1], res07[1]);
	res08[0] = _mm_add_epi16(in08[0], res08[0]);
	res08[1] = _mm_add_epi16(in08[1], res08[1]);
	res09[0] = _mm_add_epi16(in09[0], res09[0]);
	res09[1] = _mm_add_epi16(in09[1], res09[1]);
	res10[0] = _mm_add_epi16(in10[0], res10[0]);
	res10[1] = _mm_add_epi16(in10[1], res10[1]);
	res11[0] = _mm_add_epi16(in11[0], res11[0]);
	res11[1] = _mm_add_epi16(in11[1], res11[1]);
	res12[0] = _mm_add_epi16(in12[0], res12[0]);
	res12[1] = _mm_add_epi16(in12[1], res12[1]);
	res13[0] = _mm_add_epi16(in13[0], res13[0]);
	res13[1] = _mm_add_epi16(in13[1], res13[1]);
	res14[0] = _mm_add_epi16(in14[0], res14[0]);
	res14[1] = _mm_add_epi16(in14[1], res14[1]);
	res15[0] = _mm_add_epi16(in15[0], res15[0]);
	res15[1] = _mm_add_epi16(in15[1], res15[1]);

	in00[0] = _mm_packus_epi16(res00[0], res00[1]);
	in01[0] = _mm_packus_epi16(res01[0], res01[1]);
	in02[0] = _mm_packus_epi16(res02[0], res02[1]);
	in03[0] = _mm_packus_epi16(res03[0], res03[1]);
	in04[0] = _mm_packus_epi16(res04[0], res04[1]);
	in05[0] = _mm_packus_epi16(res05[0], res05[1]);
	in06[0] = _mm_packus_epi16(res06[0], res06[1]);
	in07[0] = _mm_packus_epi16(res07[0], res07[1]);
	in08[0] = _mm_packus_epi16(res08[0], res08[1]);
	in09[0] = _mm_packus_epi16(res09[0], res09[1]);
	in10[0] = _mm_packus_epi16(res10[0], res10[1]);
	in11[0] = _mm_packus_epi16(res11[0], res11[1]);
	in12[0] = _mm_packus_epi16(res12[0], res12[1]);
	in13[0] = _mm_packus_epi16(res13[0], res13[1]);
	in14[0] = _mm_packus_epi16(res14[0], res14[1]);
	in15[0] = _mm_packus_epi16(res15[0], res15[1]);

	_mm_storeu_si128((__m128i*)&dst[0], in00[0]);
	_mm_storeu_si128((__m128i*)&dst[1 * i_dst], in01[0]);
	_mm_storeu_si128((__m128i*)&dst[2 * i_dst], in02[0]);
	_mm_storeu_si128((__m128i*)&dst[3 * i_dst], in03[0]);
	_mm_storeu_si128((__m128i*)&dst[4 * i_dst], in04[0]);
	_mm_storeu_si128((__m128i*)&dst[5 * i_dst], in05[0]);
	_mm_storeu_si128((__m128i*)&dst[6 * i_dst], in06[0]);
	_mm_storeu_si128((__m128i*)&dst[7 * i_dst], in07[0]);
	_mm_storeu_si128((__m128i*)&dst[8 * i_dst], in08[0]);
	_mm_storeu_si128((__m128i*)&dst[9 * i_dst], in09[0]);
	_mm_storeu_si128((__m128i*)&dst[10 * i_dst], in10[0]);
	_mm_storeu_si128((__m128i*)&dst[11 * i_dst], in11[0]);
	_mm_storeu_si128((__m128i*)&dst[12 * i_dst], in12[0]);
	_mm_storeu_si128((__m128i*)&dst[13 * i_dst], in13[0]);
	_mm_storeu_si128((__m128i*)&dst[14 * i_dst], in14[0]);
	_mm_storeu_si128((__m128i*)&dst[15 * i_dst], in15[0]);
}

void add_inv_trans_4x4_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	static int count = 0;
	const __m128i c16_p17_p42 = _mm_set1_epi32(0x0011002A);
	const __m128i c16_n42_p17 = _mm_set1_epi32(0xFFD60011);
	const __m128i c16_n32_p32 = _mm_set1_epi32(0xFFE00020);
	const __m128i c16_p32_p32 = _mm_set1_epi32(0x00200020);
	__m128i zero = _mm_setzero_si128();

	__m128i c32_rnd = _mm_set1_epi32(16);								// 第一次四舍五入

	__m128i m128iS0, m128iS1;
	__m128i m128Tmp0, m128Tmp1;
	__m128i E0, E1, O0, O1;
    __m128i P0, P1, P2, P3, D;
    unsigned int *st0, *st1, *st2, *st3;

	m128iS0 = _mm_loadu_si128((__m128i*)&src[0]);
	m128iS1 = _mm_loadu_si128((__m128i*)&src[8]);

	m128Tmp0 = _mm_unpacklo_epi16(m128iS0, m128iS1);
	E0 = _mm_madd_epi16(m128Tmp0, c16_p32_p32);
	E0 = _mm_add_epi32(E0, c32_rnd);
	E1 = _mm_madd_epi16(m128Tmp0, c16_n32_p32);
	E1 = _mm_add_epi32(E1, c32_rnd);


	m128Tmp1 = _mm_unpackhi_epi16(m128iS0, m128iS1);
	O0 = _mm_madd_epi16(m128Tmp1, c16_p17_p42);
	O1 = _mm_madd_epi16(m128Tmp1, c16_n42_p17);


	m128iS0 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E0, O0), 5), _mm_srai_epi32(_mm_sub_epi32(E1, O1), 5));
	m128iS1 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E1, O1), 5), _mm_srai_epi32(_mm_sub_epi32(E0, O0), 5));


	/*inverse*/

	m128Tmp0 = _mm_unpacklo_epi16(m128iS0, m128iS1);
	m128Tmp1 = _mm_unpackhi_epi16(m128iS0, m128iS1);
	m128iS0 = _mm_unpacklo_epi32(m128Tmp0, m128Tmp1);
	m128iS1 = _mm_unpackhi_epi32(m128Tmp0, m128Tmp1);


	/*Second fft*/

	c32_rnd = _mm_set1_epi32(2048);						//设置四舍五入

	m128Tmp0 = _mm_unpacklo_epi16(m128iS0, m128iS1);
	E0 = _mm_madd_epi16(m128Tmp0, c16_p32_p32);
	E0 = _mm_add_epi32(E0, c32_rnd);
	E1 = _mm_madd_epi16(m128Tmp0, c16_n32_p32);
	E1 = _mm_add_epi32(E1, c32_rnd);


	m128Tmp1 = _mm_unpackhi_epi16(m128iS0, m128iS1);
	O0 = _mm_madd_epi16(m128Tmp1, c16_p17_p42);
	O1 = _mm_madd_epi16(m128Tmp1, c16_n42_p17);


	m128iS0 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E0, O0), 12), _mm_srai_epi32(_mm_sub_epi32(E1, O1), 12));
	m128iS1 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(E1, O1), 12), _mm_srai_epi32(_mm_sub_epi32(E0, O0), 12));

	m128Tmp0 = _mm_unpacklo_epi16(m128iS0, m128iS1);
	m128Tmp1 = _mm_unpackhi_epi16(m128iS0, m128iS1);
	m128iS0 = _mm_unpacklo_epi32(m128Tmp0, m128Tmp1);
	m128iS1 = _mm_unpackhi_epi32(m128Tmp0, m128Tmp1);

	//clip
	{
		__m128i max_val = _mm_set1_epi16(255);
		__m128i min_val = _mm_set1_epi16(-256);
		m128iS0 = _mm_min_epi16(m128iS0, max_val);
		m128iS0 = _mm_max_epi16(m128iS0, min_val);
		m128iS1 = _mm_min_epi16(m128iS1, max_val);
		m128iS1 = _mm_max_epi16(m128iS1, min_val);
	}

	P0 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[0]), zero);
	P1 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred]), zero);
	P2 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred << 1]), zero);
	P3 = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i*)&pred[i_pred * 3]), zero);

	P0 = _mm_unpacklo_epi64(P0, P1);
	P2 = _mm_unpacklo_epi64(P2, P3);

	m128iS0 = _mm_add_epi16(m128iS0, P0);
	m128iS1 = _mm_add_epi16(m128iS1, P2);

	D = _mm_packus_epi16(m128iS0, m128iS1);

	st0 = (unsigned int *)&dst[0];
	st1 = (unsigned int *)&dst[i_dst];
	st2 = (unsigned int *)&dst[i_dst << 1];
	st3 = (unsigned int *)&dst[i_dst * 3];

    *st0 = _mm_extract_epi32(D, 0);
    *st1 = _mm_extract_epi32(D, 1);
    *st2 = _mm_extract_epi32(D, 2);
    *st3 = _mm_extract_epi32(D, 3);
}

void add_inv_trans_32x32_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    int k;
    __m128i P00[2], P01[2], P02[2], P03[2], P04[2], P05[2], P06[2], P07[2], P08[2], P09[2], P10[2], P11[2], P12[2], P13[2], P14[2], P15[2], P16[2], P17[2], P18[2], P19[2], P20[2], P21[2], P22[2], P23[2], P24[2], P25[2], P26[2], P27[2], P28[2], P29[2], P30[2], P31[2];

	const __m128i c16_p45_p45 = _mm_set1_epi32(0x002D002D);
	const __m128i c16_p43_p44 = _mm_set1_epi32(0x002B002C);
	const __m128i c16_p39_p41 = _mm_set1_epi32(0x00270029);
	const __m128i c16_p34_p36 = _mm_set1_epi32(0x00220024);
	const __m128i c16_p27_p30 = _mm_set1_epi32(0x001B001E);
	const __m128i c16_p19_p23 = _mm_set1_epi32(0x00130017);
	const __m128i c16_p11_p15 = _mm_set1_epi32(0x000B000F);
	const __m128i c16_p02_p07 = _mm_set1_epi32(0x00020007);
	const __m128i c16_p41_p45 = _mm_set1_epi32(0x0029002D);
	const __m128i c16_p23_p34 = _mm_set1_epi32(0x00170022);
	const __m128i c16_n02_p11 = _mm_set1_epi32(0xFFFE000B);
	const __m128i c16_n27_n15 = _mm_set1_epi32(0xFFE5FFF1);
	const __m128i c16_n43_n36 = _mm_set1_epi32(0xFFD5FFDC);
	const __m128i c16_n44_n45 = _mm_set1_epi32(0xFFD4FFD3);
	const __m128i c16_n30_n39 = _mm_set1_epi32(0xFFE2FFD9);
	const __m128i c16_n07_n19 = _mm_set1_epi32(0xFFF9FFED);
	const __m128i c16_p34_p44 = _mm_set1_epi32(0x0022002C);
	const __m128i c16_n07_p15 = _mm_set1_epi32(0xFFF9000F);
	const __m128i c16_n41_n27 = _mm_set1_epi32(0xFFD7FFE5);
	const __m128i c16_n39_n45 = _mm_set1_epi32(0xFFD9FFD3);
	const __m128i c16_n02_n23 = _mm_set1_epi32(0xFFFEFFE9);
	const __m128i c16_p36_p19 = _mm_set1_epi32(0x00240013);
	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
	const __m128i c16_p11_p30 = _mm_set1_epi32(0x000B001E);
	const __m128i c16_p23_p43 = _mm_set1_epi32(0x0017002B);
	const __m128i c16_n34_n07 = _mm_set1_epi32(0xFFDEFFF9);
	const __m128i c16_n36_n45 = _mm_set1_epi32(0xFFDCFFD3);
	const __m128i c16_p19_n11 = _mm_set1_epi32(0x0013FFF5);
	const __m128i c16_p44_p41 = _mm_set1_epi32(0x002C0029);
	const __m128i c16_n02_p27 = _mm_set1_epi32(0xFFFE001B);
	const __m128i c16_n45_n30 = _mm_set1_epi32(0xFFD3FFE2);
	const __m128i c16_n15_n39 = _mm_set1_epi32(0xFFF1FFD9);
	const __m128i c16_p11_p41 = _mm_set1_epi32(0x000B0029);
	const __m128i c16_n45_n27 = _mm_set1_epi32(0xFFD3FFE5);
	const __m128i c16_p07_n30 = _mm_set1_epi32(0x0007FFE2);
	const __m128i c16_p43_p39 = _mm_set1_epi32(0x002B0027);
	const __m128i c16_n23_p15 = _mm_set1_epi32(0xFFE9000F);
	const __m128i c16_n34_n45 = _mm_set1_epi32(0xFFDEFFD3);
	const __m128i c16_p36_p02 = _mm_set1_epi32(0x00240002);
	const __m128i c16_p19_p44 = _mm_set1_epi32(0x0013002C);
	const __m128i c16_n02_p39 = _mm_set1_epi32(0xFFFE0027);
	const __m128i c16_n36_n41 = _mm_set1_epi32(0xFFDCFFD7);
	const __m128i c16_p43_p07 = _mm_set1_epi32(0x002B0007);
	const __m128i c16_n11_p34 = _mm_set1_epi32(0xFFF50022);
	const __m128i c16_n30_n44 = _mm_set1_epi32(0xFFE2FFD4);
	const __m128i c16_p45_p15 = _mm_set1_epi32(0x002D000F);
	const __m128i c16_n19_p27 = _mm_set1_epi32(0xFFED001B);
	const __m128i c16_n23_n45 = _mm_set1_epi32(0xFFE9FFD3);
	const __m128i c16_n15_p36 = _mm_set1_epi32(0xFFF10024);
	const __m128i c16_n11_n45 = _mm_set1_epi32(0xFFF5FFD3);
	const __m128i c16_p34_p39 = _mm_set1_epi32(0x00220027);
	const __m128i c16_n45_n19 = _mm_set1_epi32(0xFFD3FFED);
	const __m128i c16_p41_n07 = _mm_set1_epi32(0x0029FFF9);
	const __m128i c16_n23_p30 = _mm_set1_epi32(0xFFE9001E);
	const __m128i c16_n02_n44 = _mm_set1_epi32(0xFFFEFFD4);
	const __m128i c16_p27_p43 = _mm_set1_epi32(0x001B002B);
	const __m128i c16_n27_p34 = _mm_set1_epi32(0xFFE50022);
	const __m128i c16_p19_n39 = _mm_set1_epi32(0x0013FFD9);
	const __m128i c16_n11_p43 = _mm_set1_epi32(0xFFF5002B);
	const __m128i c16_p02_n45 = _mm_set1_epi32(0x0002FFD3);
	const __m128i c16_p07_p45 = _mm_set1_epi32(0x0007002D);
	const __m128i c16_n15_n44 = _mm_set1_epi32(0xFFF1FFD4);
	const __m128i c16_p23_p41 = _mm_set1_epi32(0x00170029);
	const __m128i c16_n30_n36 = _mm_set1_epi32(0xFFE2FFDC);
	const __m128i c16_n36_p30 = _mm_set1_epi32(0xFFDC001E);
	const __m128i c16_p41_n23 = _mm_set1_epi32(0x0029FFE9);
	const __m128i c16_n44_p15 = _mm_set1_epi32(0xFFD4000F);
	const __m128i c16_p45_n07 = _mm_set1_epi32(0x002DFFF9);
	const __m128i c16_n45_n02 = _mm_set1_epi32(0xFFD3FFFE);
	const __m128i c16_p43_p11 = _mm_set1_epi32(0x002B000B);
	const __m128i c16_n39_n19 = _mm_set1_epi32(0xFFD9FFED);
	const __m128i c16_p34_p27 = _mm_set1_epi32(0x0022001B);
	const __m128i c16_n43_p27 = _mm_set1_epi32(0xFFD5001B);
	const __m128i c16_p44_n02 = _mm_set1_epi32(0x002CFFFE);
	const __m128i c16_n30_n23 = _mm_set1_epi32(0xFFE2FFE9);
	const __m128i c16_p07_p41 = _mm_set1_epi32(0x00070029);
	const __m128i c16_p19_n45 = _mm_set1_epi32(0x0013FFD3);
	const __m128i c16_n39_p34 = _mm_set1_epi32(0xFFD90022);
	const __m128i c16_p45_n11 = _mm_set1_epi32(0x002DFFF5);
	const __m128i c16_n36_n15 = _mm_set1_epi32(0xFFDCFFF1);
	const __m128i c16_n45_p23 = _mm_set1_epi32(0xFFD30017);
	const __m128i c16_p27_p19 = _mm_set1_epi32(0x001B0013);
	const __m128i c16_p15_n45 = _mm_set1_epi32(0x000FFFD3);
	const __m128i c16_n44_p30 = _mm_set1_epi32(0xFFD4001E);
	const __m128i c16_p34_p11 = _mm_set1_epi32(0x0022000B);
	const __m128i c16_p07_n43 = _mm_set1_epi32(0x0007FFD5);
	const __m128i c16_n41_p36 = _mm_set1_epi32(0xFFD70024);
	const __m128i c16_p39_p02 = _mm_set1_epi32(0x00270002);
	const __m128i c16_n44_p19 = _mm_set1_epi32(0xFFD40013);
	const __m128i c16_n02_p36 = _mm_set1_epi32(0xFFFE0024);
	const __m128i c16_p45_n34 = _mm_set1_epi32(0x002DFFDE);
	const __m128i c16_n15_n23 = _mm_set1_epi32(0xFFF1FFE9);
	const __m128i c16_n39_p43 = _mm_set1_epi32(0xFFD9002B);
	const __m128i c16_p30_p07 = _mm_set1_epi32(0x001E0007);
	const __m128i c16_p27_n45 = _mm_set1_epi32(0x001BFFD3);
	const __m128i c16_n41_p11 = _mm_set1_epi32(0xFFD7000B);
	const __m128i c16_n39_p15 = _mm_set1_epi32(0xFFD9000F);
	const __m128i c16_n30_p45 = _mm_set1_epi32(0xFFE2002D);
	const __m128i c16_p27_p02 = _mm_set1_epi32(0x001B0002);
	const __m128i c16_p41_n44 = _mm_set1_epi32(0x0029FFD4);
	const __m128i c16_n11_n19 = _mm_set1_epi32(0xFFF5FFED);
	const __m128i c16_n45_p36 = _mm_set1_epi32(0xFFD30024);
	const __m128i c16_n07_p34 = _mm_set1_epi32(0xFFF90022);
	const __m128i c16_p43_n23 = _mm_set1_epi32(0x002BFFE9);
	const __m128i c16_n30_p11 = _mm_set1_epi32(0xFFE2000B);
	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);
	const __m128i c16_n19_p36 = _mm_set1_epi32(0xFFED0024);
	const __m128i c16_p23_n02 = _mm_set1_epi32(0x0017FFFE);
	const __m128i c16_p45_n39 = _mm_set1_epi32(0x002DFFD9);
	const __m128i c16_p27_n41 = _mm_set1_epi32(0x001BFFD7);
	const __m128i c16_n15_n07 = _mm_set1_epi32(0xFFF1FFF9);
	const __m128i c16_n44_p34 = _mm_set1_epi32(0xFFD40022);
	const __m128i c16_n19_p07 = _mm_set1_epi32(0xFFED0007);
	const __m128i c16_n39_p30 = _mm_set1_epi32(0xFFD9001E);
	const __m128i c16_n45_p44 = _mm_set1_epi32(0xFFD3002C);
	const __m128i c16_n36_p43 = _mm_set1_epi32(0xFFDC002B);
	const __m128i c16_n15_p27 = _mm_set1_epi32(0xFFF1001B);
	const __m128i c16_p11_p02 = _mm_set1_epi32(0x000B0002);
	const __m128i c16_p34_n23 = _mm_set1_epi32(0x0022FFE9);
	const __m128i c16_p45_n41 = _mm_set1_epi32(0x002DFFD7);
	const __m128i c16_n07_p02 = _mm_set1_epi32(0xFFF90002);
	const __m128i c16_n15_p11 = _mm_set1_epi32(0xFFF1000B);
	const __m128i c16_n23_p19 = _mm_set1_epi32(0xFFE90013);
	const __m128i c16_n30_p27 = _mm_set1_epi32(0xFFE2001B);
	const __m128i c16_n36_p34 = _mm_set1_epi32(0xFFDC0022);
	const __m128i c16_n41_p39 = _mm_set1_epi32(0xFFD70027);
	const __m128i c16_n44_p43 = _mm_set1_epi32(0xFFD4002B);
	const __m128i c16_n45_p45 = _mm_set1_epi32(0xFFD3002D);

	//	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
	const __m128i c16_p35_p40 = _mm_set1_epi32(0x00230028);
	const __m128i c16_p21_p29 = _mm_set1_epi32(0x0015001D);
	const __m128i c16_p04_p13 = _mm_set1_epi32(0x0004000D);
	const __m128i c16_p29_p43 = _mm_set1_epi32(0x001D002B);
	const __m128i c16_n21_p04 = _mm_set1_epi32(0xFFEB0004);
	const __m128i c16_n45_n40 = _mm_set1_epi32(0xFFD3FFD8);
	const __m128i c16_n13_n35 = _mm_set1_epi32(0xFFF3FFDD);
	const __m128i c16_p04_p40 = _mm_set1_epi32(0x00040028);
	const __m128i c16_n43_n35 = _mm_set1_epi32(0xFFD5FFDD);
	const __m128i c16_p29_n13 = _mm_set1_epi32(0x001DFFF3);
	const __m128i c16_p21_p45 = _mm_set1_epi32(0x0015002D);
	const __m128i c16_n21_p35 = _mm_set1_epi32(0xFFEB0023);
	const __m128i c16_p04_n43 = _mm_set1_epi32(0x0004FFD5);
	const __m128i c16_p13_p45 = _mm_set1_epi32(0x000D002D);
	const __m128i c16_n29_n40 = _mm_set1_epi32(0xFFE3FFD8);
	const __m128i c16_n40_p29 = _mm_set1_epi32(0xFFD8001D);
	const __m128i c16_p45_n13 = _mm_set1_epi32(0x002DFFF3);
	const __m128i c16_n43_n04 = _mm_set1_epi32(0xFFD5FFFC);
	const __m128i c16_p35_p21 = _mm_set1_epi32(0x00230015);
	const __m128i c16_n45_p21 = _mm_set1_epi32(0xFFD30015);
	const __m128i c16_p13_p29 = _mm_set1_epi32(0x000D001D);
	const __m128i c16_p35_n43 = _mm_set1_epi32(0x0023FFD5);
	const __m128i c16_n40_p04 = _mm_set1_epi32(0xFFD80004);
	const __m128i c16_n35_p13 = _mm_set1_epi32(0xFFDD000D);
	const __m128i c16_n40_p45 = _mm_set1_epi32(0xFFD8002D);
	const __m128i c16_p04_p21 = _mm_set1_epi32(0x00040015);
	const __m128i c16_p43_n29 = _mm_set1_epi32(0x002BFFE3);
	const __m128i c16_n13_p04 = _mm_set1_epi32(0xFFF30004);
	const __m128i c16_n29_p21 = _mm_set1_epi32(0xFFE30015);
	const __m128i c16_n40_p35 = _mm_set1_epi32(0xFFD80023);
	//	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);


	const __m128i c16_p38_p44 = _mm_set1_epi32(0x0026002C);
	const __m128i c16_p09_p25 = _mm_set1_epi32(0x00090019);
	const __m128i c16_n09_p38 = _mm_set1_epi32(0xFFF70026);
	const __m128i c16_n25_n44 = _mm_set1_epi32(0xFFE7FFD4);

	const __m128i c16_n44_p25 = _mm_set1_epi32(0xFFD40019);
	const __m128i c16_p38_p09 = _mm_set1_epi32(0x00260009);
	const __m128i c16_n25_p09 = _mm_set1_epi32(0xFFE70009);
	const __m128i c16_n44_p38 = _mm_set1_epi32(0xFFD40026);

	const __m128i c16_p17_p42 = _mm_set1_epi32(0x0011002A);
	const __m128i c16_n42_p17 = _mm_set1_epi32(0xFFD60011);

	const __m128i c16_p32_p32 = _mm_set1_epi32(0x00200020);
	const __m128i c16_n32_p32 = _mm_set1_epi32(0xFFE00020);

	__m128i zero = _mm_setzero_si128();
	__m128i c32_rnd = _mm_set1_epi32(16);

	int nShift = 5;
	int i, pass, part;

	// DCT1
	__m128i in00[4], in01[4], in02[4], in03[4], in04[4], in05[4], in06[4], in07[4], in08[4], in09[4], in10[4], in11[4], in12[4], in13[4], in14[4], in15[4];
	__m128i in16[4], in17[4], in18[4], in19[4], in20[4], in21[4], in22[4], in23[4], in24[4], in25[4], in26[4], in27[4], in28[4], in29[4], in30[4], in31[4];
	__m128i res00[4], res01[4], res02[4], res03[4], res04[4], res05[4], res06[4], res07[4], res08[4], res09[4], res10[4], res11[4], res12[4], res13[4], res14[4], res15[4];
	__m128i res16[4], res17[4], res18[4], res19[4], res20[4], res21[4], res22[4], res23[4], res24[4], res25[4], res26[4], res27[4], res28[4], res29[4], res30[4], res31[4];

	for (i = 0; i < 4; i++)
	{
		const int offset = (i << 3);
 
		in00[i] = _mm_loadu_si128((const __m128i*)&src[0 * 32 + offset]);
		in01[i] = _mm_loadu_si128((const __m128i*)&src[1 * 32 + offset]);
		in02[i] = _mm_loadu_si128((const __m128i*)&src[2 * 32 + offset]);
		in03[i] = _mm_loadu_si128((const __m128i*)&src[3 * 32 + offset]);
		in04[i] = _mm_loadu_si128((const __m128i*)&src[4 * 32 + offset]);
		in05[i] = _mm_loadu_si128((const __m128i*)&src[5 * 32 + offset]);
		in06[i] = _mm_loadu_si128((const __m128i*)&src[6 * 32 + offset]);
		in07[i] = _mm_loadu_si128((const __m128i*)&src[7 * 32 + offset]);
		in08[i] = _mm_loadu_si128((const __m128i*)&src[8 * 32 + offset]);
		in09[i] = _mm_loadu_si128((const __m128i*)&src[9 * 32 + offset]);
		in10[i] = _mm_loadu_si128((const __m128i*)&src[10 * 32 + offset]);
		in11[i] = _mm_loadu_si128((const __m128i*)&src[11 * 32 + offset]);
		in12[i] = _mm_loadu_si128((const __m128i*)&src[12 * 32 + offset]);
		in13[i] = _mm_loadu_si128((const __m128i*)&src[13 * 32 + offset]);
		in14[i] = _mm_loadu_si128((const __m128i*)&src[14 * 32 + offset]);
		in15[i] = _mm_loadu_si128((const __m128i*)&src[15 * 32 + offset]);
		in16[i] = _mm_loadu_si128((const __m128i*)&src[16 * 32 + offset]);
		in17[i] = _mm_loadu_si128((const __m128i*)&src[17 * 32 + offset]);
		in18[i] = _mm_loadu_si128((const __m128i*)&src[18 * 32 + offset]);
		in19[i] = _mm_loadu_si128((const __m128i*)&src[19 * 32 + offset]);
		in20[i] = _mm_loadu_si128((const __m128i*)&src[20 * 32 + offset]);
		in21[i] = _mm_loadu_si128((const __m128i*)&src[21 * 32 + offset]);
		in22[i] = _mm_loadu_si128((const __m128i*)&src[22 * 32 + offset]);
		in23[i] = _mm_loadu_si128((const __m128i*)&src[23 * 32 + offset]);
		in24[i] = _mm_loadu_si128((const __m128i*)&src[24 * 32 + offset]);
		in25[i] = _mm_loadu_si128((const __m128i*)&src[25 * 32 + offset]);
		in26[i] = _mm_loadu_si128((const __m128i*)&src[26 * 32 + offset]);
		in27[i] = _mm_loadu_si128((const __m128i*)&src[27 * 32 + offset]);
		in28[i] = _mm_loadu_si128((const __m128i*)&src[28 * 32 + offset]);
		in29[i] = _mm_loadu_si128((const __m128i*)&src[29 * 32 + offset]);
		in30[i] = _mm_loadu_si128((const __m128i*)&src[30 * 32 + offset]);
		in31[i] = _mm_loadu_si128((const __m128i*)&src[31 * 32 + offset]);
	}

	for (pass = 0; pass < 2; pass++)
	{
		if (pass == 1)
		{
			c32_rnd = _mm_set1_epi32(2048);				// pass == 1 第二次四舍五入
			nShift = 12;
		}

		for (part = 0; part < 4; part++)
		{
			const __m128i T_00_00A = _mm_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
			const __m128i T_00_00B = _mm_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
			const __m128i T_00_01A = _mm_unpacklo_epi16(in05[part], in07[part]);       // [ ]
			const __m128i T_00_01B = _mm_unpackhi_epi16(in05[part], in07[part]);       // [ ]
			const __m128i T_00_02A = _mm_unpacklo_epi16(in09[part], in11[part]);       // [ ]
			const __m128i T_00_02B = _mm_unpackhi_epi16(in09[part], in11[part]);       // [ ]
			const __m128i T_00_03A = _mm_unpacklo_epi16(in13[part], in15[part]);       // [ ]
			const __m128i T_00_03B = _mm_unpackhi_epi16(in13[part], in15[part]);       // [ ]
			const __m128i T_00_04A = _mm_unpacklo_epi16(in17[part], in19[part]);       // [ ]
			const __m128i T_00_04B = _mm_unpackhi_epi16(in17[part], in19[part]);       // [ ]
			const __m128i T_00_05A = _mm_unpacklo_epi16(in21[part], in23[part]);       // [ ]
			const __m128i T_00_05B = _mm_unpackhi_epi16(in21[part], in23[part]);       // [ ]
			const __m128i T_00_06A = _mm_unpacklo_epi16(in25[part], in27[part]);       // [ ]
			const __m128i T_00_06B = _mm_unpackhi_epi16(in25[part], in27[part]);       // [ ]
			const __m128i T_00_07A = _mm_unpacklo_epi16(in29[part], in31[part]);       //
			const __m128i T_00_07B = _mm_unpackhi_epi16(in29[part], in31[part]);       // [ ]

			const __m128i T_00_08A = _mm_unpacklo_epi16(in02[part], in06[part]);       // [ ]
			const __m128i T_00_08B = _mm_unpackhi_epi16(in02[part], in06[part]);       // [ ]
			const __m128i T_00_09A = _mm_unpacklo_epi16(in10[part], in14[part]);       // [ ]
			const __m128i T_00_09B = _mm_unpackhi_epi16(in10[part], in14[part]);       // [ ]
			const __m128i T_00_10A = _mm_unpacklo_epi16(in18[part], in22[part]);       // [ ]
			const __m128i T_00_10B = _mm_unpackhi_epi16(in18[part], in22[part]);       // [ ]
			const __m128i T_00_11A = _mm_unpacklo_epi16(in26[part], in30[part]);       // [ ]
			const __m128i T_00_11B = _mm_unpackhi_epi16(in26[part], in30[part]);       // [ ]

			const __m128i T_00_12A = _mm_unpacklo_epi16(in04[part], in12[part]);       // [ ]
			const __m128i T_00_12B = _mm_unpackhi_epi16(in04[part], in12[part]);       // [ ]
			const __m128i T_00_13A = _mm_unpacklo_epi16(in20[part], in28[part]);       // [ ]
			const __m128i T_00_13B = _mm_unpackhi_epi16(in20[part], in28[part]);       // [ ]

			const __m128i T_00_14A = _mm_unpacklo_epi16(in08[part], in24[part]);       //
			const __m128i T_00_14B = _mm_unpackhi_epi16(in08[part], in24[part]);       // [ ]
			const __m128i T_00_15A = _mm_unpacklo_epi16(in00[part], in16[part]);       //
			const __m128i T_00_15B = _mm_unpackhi_epi16(in00[part], in16[part]);       // [ ]

			__m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
			__m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
			__m128i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
			__m128i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
			{
				__m128i T00, T01, T02, T03;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, r1719, r2123, r2527, r2931, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
	T00 = _mm_add_epi32(_mm_madd_epi16(r0103, c0103), _mm_madd_epi16(r0507, c0507)); \
	T01 = _mm_add_epi32(_mm_madd_epi16(r0911, c0911), _mm_madd_epi16(r1315, c1315)); \
	T02 = _mm_add_epi32(_mm_madd_epi16(r1719, c1719), _mm_madd_epi16(r2123, c2123)); \
	T03 = _mm_add_epi32(_mm_madd_epi16(r2527, c2527), _mm_madd_epi16(r2931, c2931)); \
	row = _mm_add_epi32(_mm_add_epi32(T00, T01), _mm_add_epi32(T02, T03));

				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A)

					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B)

#undef COMPUTE_ROW
			}


			{
				__m128i T00, T01;
#define COMPUTE_ROW(row0206, row1014, row1822, row2630, c0206, c1014, c1822, c2630, row) \
	T00 = _mm_add_epi32(_mm_madd_epi16(row0206, c0206), _mm_madd_epi16(row1014, c1014)); \
	T01 = _mm_add_epi32(_mm_madd_epi16(row1822, c1822), _mm_madd_epi16(row2630, c2630)); \
	row = _mm_add_epi32(T00, T01);

				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A)

					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B)
#undef COMPUTE_ROW
			}
			{
				const __m128i EEO0A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_p38_p44), _mm_madd_epi16(T_00_13A, c16_p09_p25));
				const __m128i EEO1A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n09_p38), _mm_madd_epi16(T_00_13A, c16_n25_n44));
				const __m128i EEO2A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n44_p25), _mm_madd_epi16(T_00_13A, c16_p38_p09));
				const __m128i EEO3A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n25_p09), _mm_madd_epi16(T_00_13A, c16_n44_p38));
				const __m128i EEO0B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_p38_p44), _mm_madd_epi16(T_00_13B, c16_p09_p25));
				const __m128i EEO1B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n09_p38), _mm_madd_epi16(T_00_13B, c16_n25_n44));
				const __m128i EEO2B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n44_p25), _mm_madd_epi16(T_00_13B, c16_p38_p09));
				const __m128i EEO3B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n25_p09), _mm_madd_epi16(T_00_13B, c16_n44_p38));

				const __m128i EEEO0A = _mm_madd_epi16(T_00_14A, c16_p17_p42);
				const __m128i EEEO0B = _mm_madd_epi16(T_00_14B, c16_p17_p42);
				const __m128i EEEO1A = _mm_madd_epi16(T_00_14A, c16_n42_p17);
				const __m128i EEEO1B = _mm_madd_epi16(T_00_14B, c16_n42_p17);

				const __m128i EEEE0A = _mm_madd_epi16(T_00_15A, c16_p32_p32);
				const __m128i EEEE0B = _mm_madd_epi16(T_00_15B, c16_p32_p32);
				const __m128i EEEE1A = _mm_madd_epi16(T_00_15A, c16_n32_p32);
				const __m128i EEEE1B = _mm_madd_epi16(T_00_15B, c16_n32_p32);

				const __m128i EEE0A = _mm_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
				const __m128i EEE0B = _mm_add_epi32(EEEE0B, EEEO0B);
				const __m128i EEE1A = _mm_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
				const __m128i EEE1B = _mm_add_epi32(EEEE1B, EEEO1B);
				const __m128i EEE3A = _mm_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
				const __m128i EEE3B = _mm_sub_epi32(EEEE0B, EEEO0B);
				const __m128i EEE2A = _mm_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
				const __m128i EEE2B = _mm_sub_epi32(EEEE1B, EEEO1B);

				const __m128i EE0A = _mm_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
				const __m128i EE0B = _mm_add_epi32(EEE0B, EEO0B);
				const __m128i EE1A = _mm_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
				const __m128i EE1B = _mm_add_epi32(EEE1B, EEO1B);
				const __m128i EE2A = _mm_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
				const __m128i EE2B = _mm_add_epi32(EEE2B, EEO2B);
				const __m128i EE3A = _mm_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
				const __m128i EE3B = _mm_add_epi32(EEE3B, EEO3B);
				const __m128i EE7A = _mm_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
				const __m128i EE7B = _mm_sub_epi32(EEE0B, EEO0B);
				const __m128i EE6A = _mm_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
				const __m128i EE6B = _mm_sub_epi32(EEE1B, EEO1B);
				const __m128i EE5A = _mm_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
				const __m128i EE5B = _mm_sub_epi32(EEE2B, EEO2B);
				const __m128i EE4A = _mm_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
				const __m128i EE4B = _mm_sub_epi32(EEE3B, EEO3B);

				const __m128i E0A = _mm_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
				const __m128i E0B = _mm_add_epi32(EE0B, EO0B);
				const __m128i E1A = _mm_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
				const __m128i E1B = _mm_add_epi32(EE1B, EO1B);
				const __m128i E2A = _mm_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
				const __m128i E2B = _mm_add_epi32(EE2B, EO2B);
				const __m128i E3A = _mm_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
				const __m128i E3B = _mm_add_epi32(EE3B, EO3B);
				const __m128i E4A = _mm_add_epi32(EE4A, EO4A);          // E4 =
				const __m128i E4B = _mm_add_epi32(EE4B, EO4B);
				const __m128i E5A = _mm_add_epi32(EE5A, EO5A);          // E5 =
				const __m128i E5B = _mm_add_epi32(EE5B, EO5B);
				const __m128i E6A = _mm_add_epi32(EE6A, EO6A);          // E6 =
				const __m128i E6B = _mm_add_epi32(EE6B, EO6B);
				const __m128i E7A = _mm_add_epi32(EE7A, EO7A);          // E7 =
				const __m128i E7B = _mm_add_epi32(EE7B, EO7B);
				const __m128i EFA = _mm_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
				const __m128i EFB = _mm_sub_epi32(EE0B, EO0B);
				const __m128i EEA = _mm_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
				const __m128i EEB = _mm_sub_epi32(EE1B, EO1B);
				const __m128i EDA = _mm_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
				const __m128i EDB = _mm_sub_epi32(EE2B, EO2B);
				const __m128i ECA = _mm_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
				const __m128i ECB = _mm_sub_epi32(EE3B, EO3B);
				const __m128i EBA = _mm_sub_epi32(EE4A, EO4A);          // EB =
				const __m128i EBB = _mm_sub_epi32(EE4B, EO4B);
				const __m128i EAA = _mm_sub_epi32(EE5A, EO5A);          // EA =
				const __m128i EAB = _mm_sub_epi32(EE5B, EO5B);
				const __m128i E9A = _mm_sub_epi32(EE6A, EO6A);          // E9 =
				const __m128i E9B = _mm_sub_epi32(EE6B, EO6B);
				const __m128i E8A = _mm_sub_epi32(EE7A, EO7A);          // E8 =
				const __m128i E8B = _mm_sub_epi32(EE7B, EO7B);

				const __m128i T10A = _mm_add_epi32(E0A, c32_rnd);         // E0 + rnd
				const __m128i T10B = _mm_add_epi32(E0B, c32_rnd);
				const __m128i T11A = _mm_add_epi32(E1A, c32_rnd);         // E1 + rnd
				const __m128i T11B = _mm_add_epi32(E1B, c32_rnd);
				const __m128i T12A = _mm_add_epi32(E2A, c32_rnd);         // E2 + rnd
				const __m128i T12B = _mm_add_epi32(E2B, c32_rnd);
				const __m128i T13A = _mm_add_epi32(E3A, c32_rnd);         // E3 + rnd
				const __m128i T13B = _mm_add_epi32(E3B, c32_rnd);
				const __m128i T14A = _mm_add_epi32(E4A, c32_rnd);         // E4 + rnd
				const __m128i T14B = _mm_add_epi32(E4B, c32_rnd);
				const __m128i T15A = _mm_add_epi32(E5A, c32_rnd);         // E5 + rnd
				const __m128i T15B = _mm_add_epi32(E5B, c32_rnd);
				const __m128i T16A = _mm_add_epi32(E6A, c32_rnd);         // E6 + rnd
				const __m128i T16B = _mm_add_epi32(E6B, c32_rnd);
				const __m128i T17A = _mm_add_epi32(E7A, c32_rnd);         // E7 + rnd
				const __m128i T17B = _mm_add_epi32(E7B, c32_rnd);
				const __m128i T18A = _mm_add_epi32(E8A, c32_rnd);         // E8 + rnd
				const __m128i T18B = _mm_add_epi32(E8B, c32_rnd);
				const __m128i T19A = _mm_add_epi32(E9A, c32_rnd);         // E9 + rnd
				const __m128i T19B = _mm_add_epi32(E9B, c32_rnd);
				const __m128i T1AA = _mm_add_epi32(EAA, c32_rnd);         // E10 + rnd
				const __m128i T1AB = _mm_add_epi32(EAB, c32_rnd);
				const __m128i T1BA = _mm_add_epi32(EBA, c32_rnd);         // E11 + rnd
				const __m128i T1BB = _mm_add_epi32(EBB, c32_rnd);
				const __m128i T1CA = _mm_add_epi32(ECA, c32_rnd);         // E12 + rnd
				const __m128i T1CB = _mm_add_epi32(ECB, c32_rnd);
				const __m128i T1DA = _mm_add_epi32(EDA, c32_rnd);         // E13 + rnd
				const __m128i T1DB = _mm_add_epi32(EDB, c32_rnd);
				const __m128i T1EA = _mm_add_epi32(EEA, c32_rnd);         // E14 + rnd
				const __m128i T1EB = _mm_add_epi32(EEB, c32_rnd);
				const __m128i T1FA = _mm_add_epi32(EFA, c32_rnd);         // E15 + rnd
				const __m128i T1FB = _mm_add_epi32(EFB, c32_rnd);

				const __m128i T2_00A = _mm_add_epi32(T10A, O00A);          // E0 + O0 + rnd
				const __m128i T2_00B = _mm_add_epi32(T10B, O00B);
				const __m128i T2_01A = _mm_add_epi32(T11A, O01A);          // E1 + O1 + rnd
				const __m128i T2_01B = _mm_add_epi32(T11B, O01B);
				const __m128i T2_02A = _mm_add_epi32(T12A, O02A);          // E2 + O2 + rnd
				const __m128i T2_02B = _mm_add_epi32(T12B, O02B);
				const __m128i T2_03A = _mm_add_epi32(T13A, O03A);          // E3 + O3 + rnd
				const __m128i T2_03B = _mm_add_epi32(T13B, O03B);
				const __m128i T2_04A = _mm_add_epi32(T14A, O04A);          // E4
				const __m128i T2_04B = _mm_add_epi32(T14B, O04B);
				const __m128i T2_05A = _mm_add_epi32(T15A, O05A);          // E5
				const __m128i T2_05B = _mm_add_epi32(T15B, O05B);
				const __m128i T2_06A = _mm_add_epi32(T16A, O06A);          // E6
				const __m128i T2_06B = _mm_add_epi32(T16B, O06B);
				const __m128i T2_07A = _mm_add_epi32(T17A, O07A);          // E7
				const __m128i T2_07B = _mm_add_epi32(T17B, O07B);
				const __m128i T2_08A = _mm_add_epi32(T18A, O08A);          // E8
				const __m128i T2_08B = _mm_add_epi32(T18B, O08B);
				const __m128i T2_09A = _mm_add_epi32(T19A, O09A);          // E9
				const __m128i T2_09B = _mm_add_epi32(T19B, O09B);
				const __m128i T2_10A = _mm_add_epi32(T1AA, O10A);          // E10
				const __m128i T2_10B = _mm_add_epi32(T1AB, O10B);
				const __m128i T2_11A = _mm_add_epi32(T1BA, O11A);          // E11
				const __m128i T2_11B = _mm_add_epi32(T1BB, O11B);
				const __m128i T2_12A = _mm_add_epi32(T1CA, O12A);          // E12
				const __m128i T2_12B = _mm_add_epi32(T1CB, O12B);
				const __m128i T2_13A = _mm_add_epi32(T1DA, O13A);          // E13
				const __m128i T2_13B = _mm_add_epi32(T1DB, O13B);
				const __m128i T2_14A = _mm_add_epi32(T1EA, O14A);          // E14
				const __m128i T2_14B = _mm_add_epi32(T1EB, O14B);
				const __m128i T2_15A = _mm_add_epi32(T1FA, O15A);          // E15
				const __m128i T2_15B = _mm_add_epi32(T1FB, O15B);
				const __m128i T2_31A = _mm_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
				const __m128i T2_31B = _mm_sub_epi32(T10B, O00B);
				const __m128i T2_30A = _mm_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
				const __m128i T2_30B = _mm_sub_epi32(T11B, O01B);
				const __m128i T2_29A = _mm_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
				const __m128i T2_29B = _mm_sub_epi32(T12B, O02B);
				const __m128i T2_28A = _mm_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
				const __m128i T2_28B = _mm_sub_epi32(T13B, O03B);
				const __m128i T2_27A = _mm_sub_epi32(T14A, O04A);          // E4
				const __m128i T2_27B = _mm_sub_epi32(T14B, O04B);
				const __m128i T2_26A = _mm_sub_epi32(T15A, O05A);          // E5
				const __m128i T2_26B = _mm_sub_epi32(T15B, O05B);
				const __m128i T2_25A = _mm_sub_epi32(T16A, O06A);          // E6
				const __m128i T2_25B = _mm_sub_epi32(T16B, O06B);
				const __m128i T2_24A = _mm_sub_epi32(T17A, O07A);          // E7
				const __m128i T2_24B = _mm_sub_epi32(T17B, O07B);
				const __m128i T2_23A = _mm_sub_epi32(T18A, O08A);          //
				const __m128i T2_23B = _mm_sub_epi32(T18B, O08B);
				const __m128i T2_22A = _mm_sub_epi32(T19A, O09A);          //
				const __m128i T2_22B = _mm_sub_epi32(T19B, O09B);
				const __m128i T2_21A = _mm_sub_epi32(T1AA, O10A);          //
				const __m128i T2_21B = _mm_sub_epi32(T1AB, O10B);
				const __m128i T2_20A = _mm_sub_epi32(T1BA, O11A);          //
				const __m128i T2_20B = _mm_sub_epi32(T1BB, O11B);
				const __m128i T2_19A = _mm_sub_epi32(T1CA, O12A);          //
				const __m128i T2_19B = _mm_sub_epi32(T1CB, O12B);
				const __m128i T2_18A = _mm_sub_epi32(T1DA, O13A);          //
				const __m128i T2_18B = _mm_sub_epi32(T1DB, O13B);
				const __m128i T2_17A = _mm_sub_epi32(T1EA, O14A);          //
				const __m128i T2_17B = _mm_sub_epi32(T1EB, O14B);
				const __m128i T2_16A = _mm_sub_epi32(T1FA, O15A);          //
				const __m128i T2_16B = _mm_sub_epi32(T1FB, O15B);

				const __m128i T3_00A = _mm_srai_epi32(T2_00A, nShift);             // [30 20 10 00]
				const __m128i T3_00B = _mm_srai_epi32(T2_00B, nShift);             // [70 60 50 40]
				const __m128i T3_01A = _mm_srai_epi32(T2_01A, nShift);             // [31 21 11 01]
				const __m128i T3_01B = _mm_srai_epi32(T2_01B, nShift);             // [71 61 51 41]
				const __m128i T3_02A = _mm_srai_epi32(T2_02A, nShift);             // [32 22 12 02]
				const __m128i T3_02B = _mm_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
				const __m128i T3_03A = _mm_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
				const __m128i T3_03B = _mm_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
				const __m128i T3_04A = _mm_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
				const __m128i T3_04B = _mm_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
				const __m128i T3_05A = _mm_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
				const __m128i T3_05B = _mm_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
				const __m128i T3_06A = _mm_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
				const __m128i T3_06B = _mm_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
				const __m128i T3_07A = _mm_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
				const __m128i T3_07B = _mm_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
				const __m128i T3_08A = _mm_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
				const __m128i T3_08B = _mm_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
				const __m128i T3_09A = _mm_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
				const __m128i T3_09B = _mm_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
				const __m128i T3_10A = _mm_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
				const __m128i T3_10B = _mm_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
				const __m128i T3_11A = _mm_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
				const __m128i T3_11B = _mm_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
				const __m128i T3_12A = _mm_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
				const __m128i T3_12B = _mm_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
				const __m128i T3_13A = _mm_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
				const __m128i T3_13B = _mm_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
				const __m128i T3_14A = _mm_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
				const __m128i T3_14B = _mm_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
				const __m128i T3_15A = _mm_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
				const __m128i T3_15B = _mm_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

				const __m128i T3_16A = _mm_srai_epi32(T2_16A, nShift);             // [30 20 10 00]
				const __m128i T3_16B = _mm_srai_epi32(T2_16B, nShift);             // [70 60 50 40]
				const __m128i T3_17A = _mm_srai_epi32(T2_17A, nShift);             // [31 21 11 01]
				const __m128i T3_17B = _mm_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
				const __m128i T3_18A = _mm_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
				const __m128i T3_18B = _mm_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
				const __m128i T3_19A = _mm_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
				const __m128i T3_19B = _mm_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
				const __m128i T3_20A = _mm_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
				const __m128i T3_20B = _mm_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
				const __m128i T3_21A = _mm_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
				const __m128i T3_21B = _mm_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
				const __m128i T3_22A = _mm_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
				const __m128i T3_22B = _mm_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
				const __m128i T3_23A = _mm_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
				const __m128i T3_23B = _mm_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
				const __m128i T3_24A = _mm_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
				const __m128i T3_24B = _mm_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
				const __m128i T3_25A = _mm_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
				const __m128i T3_25B = _mm_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
				const __m128i T3_26A = _mm_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
				const __m128i T3_26B = _mm_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
				const __m128i T3_27A = _mm_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
				const __m128i T3_27B = _mm_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
				const __m128i T3_28A = _mm_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
				const __m128i T3_28B = _mm_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
				const __m128i T3_29A = _mm_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
				const __m128i T3_29B = _mm_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
				const __m128i T3_30A = _mm_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
				const __m128i T3_30B = _mm_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
				const __m128i T3_31A = _mm_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
				const __m128i T3_31B = _mm_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

				res00[part] = _mm_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
				res01[part] = _mm_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
				res02[part] = _mm_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
				res03[part] = _mm_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
				res04[part] = _mm_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
				res05[part] = _mm_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
				res06[part] = _mm_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
				res07[part] = _mm_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
				res08[part] = _mm_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
				res09[part] = _mm_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
				res10[part] = _mm_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
				res11[part] = _mm_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
				res12[part] = _mm_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
				res13[part] = _mm_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
				res14[part] = _mm_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
				res15[part] = _mm_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
				res16[part] = _mm_packs_epi32(T3_16A, T3_16B);
				res17[part] = _mm_packs_epi32(T3_17A, T3_17B);
				res18[part] = _mm_packs_epi32(T3_18A, T3_18B);
				res19[part] = _mm_packs_epi32(T3_19A, T3_19B);
				res20[part] = _mm_packs_epi32(T3_20A, T3_20B);
				res21[part] = _mm_packs_epi32(T3_21A, T3_21B);
				res22[part] = _mm_packs_epi32(T3_22A, T3_22B);
				res23[part] = _mm_packs_epi32(T3_23A, T3_23B);
				res24[part] = _mm_packs_epi32(T3_24A, T3_24B);
				res25[part] = _mm_packs_epi32(T3_25A, T3_25B);
				res26[part] = _mm_packs_epi32(T3_26A, T3_26B);
				res27[part] = _mm_packs_epi32(T3_27A, T3_27B);
				res28[part] = _mm_packs_epi32(T3_28A, T3_28B);
				res29[part] = _mm_packs_epi32(T3_29A, T3_29B);
				res30[part] = _mm_packs_epi32(T3_30A, T3_30B);
				res31[part] = _mm_packs_epi32(T3_31A, T3_31B);
			}
		}
		//transpose matrix 8x8 16bit.
		{
			__m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
			__m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
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

			TRANSPOSE_8x8_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0])
				TRANSPOSE_8x8_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0])
				TRANSPOSE_8x8_16BIT(res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0])
				TRANSPOSE_8x8_16BIT(res00[3], res01[3], res02[3], res03[3], res04[3], res05[3], res06[3], res07[3], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0])

				TRANSPOSE_8x8_16BIT(res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1])
				TRANSPOSE_8x8_16BIT(res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1])
				TRANSPOSE_8x8_16BIT(res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1])
				TRANSPOSE_8x8_16BIT(res08[3], res09[3], res10[3], res11[3], res12[3], res13[3], res14[3], res15[3], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1])

				TRANSPOSE_8x8_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2])
				TRANSPOSE_8x8_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2])
				TRANSPOSE_8x8_16BIT(res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2])
				TRANSPOSE_8x8_16BIT(res16[3], res17[3], res18[3], res19[3], res20[3], res21[3], res22[3], res23[3], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2])

				TRANSPOSE_8x8_16BIT(res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[3], in01[3], in02[3], in03[3], in04[3], in05[3], in06[3], in07[3])
				TRANSPOSE_8x8_16BIT(res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in08[3], in09[3], in10[3], in11[3], in12[3], in13[3], in14[3], in15[3])
				TRANSPOSE_8x8_16BIT(res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2], in16[3], in17[3], in18[3], in19[3], in20[3], in21[3], in22[3], in23[3])
				TRANSPOSE_8x8_16BIT(res24[3], res25[3], res26[3], res27[3], res28[3], res29[3], res30[3], res31[3], in24[3], in25[3], in26[3], in27[3], in28[3], in29[3], in30[3], in31[3])



#undef TRANSPOSE_8x8_16BIT
		}
	}


	//clip
	{
		__m128i max_val = _mm_set1_epi16(255);
		__m128i min_val = _mm_set1_epi16(-256);

		for (k = 0; k < 4; k++)
		{
			in00[k] = _mm_min_epi16(in00[k], max_val);
			in00[k] = _mm_max_epi16(in00[k], min_val);
			in01[k] = _mm_min_epi16(in01[k], max_val);
			in01[k] = _mm_max_epi16(in01[k], min_val);
			in02[k] = _mm_min_epi16(in02[k], max_val);
			in02[k] = _mm_max_epi16(in02[k], min_val);
			in03[k] = _mm_min_epi16(in03[k], max_val);
			in03[k] = _mm_max_epi16(in03[k], min_val);
			in04[k] = _mm_min_epi16(in04[k], max_val);
			in04[k] = _mm_max_epi16(in04[k], min_val);
			in05[k] = _mm_min_epi16(in05[k], max_val);
			in05[k] = _mm_max_epi16(in05[k], min_val);
			in06[k] = _mm_min_epi16(in06[k], max_val);
			in06[k] = _mm_max_epi16(in06[k], min_val);
			in07[k] = _mm_min_epi16(in07[k], max_val);
			in07[k] = _mm_max_epi16(in07[k], min_val);
			in08[k] = _mm_min_epi16(in08[k], max_val);
			in08[k] = _mm_max_epi16(in08[k], min_val);
			in09[k] = _mm_min_epi16(in09[k], max_val);
			in09[k] = _mm_max_epi16(in09[k], min_val);
			in10[k] = _mm_min_epi16(in10[k], max_val);
			in10[k] = _mm_max_epi16(in10[k], min_val);
			in11[k] = _mm_min_epi16(in11[k], max_val);
			in11[k] = _mm_max_epi16(in11[k], min_val);
			in12[k] = _mm_min_epi16(in12[k], max_val);
			in12[k] = _mm_max_epi16(in12[k], min_val);
			in13[k] = _mm_min_epi16(in13[k], max_val);
			in13[k] = _mm_max_epi16(in13[k], min_val);
			in14[k] = _mm_min_epi16(in14[k], max_val);
			in14[k] = _mm_max_epi16(in14[k], min_val);
			in15[k] = _mm_min_epi16(in15[k], max_val);
			in15[k] = _mm_max_epi16(in15[k], min_val);
			in16[k] = _mm_min_epi16(in16[k], max_val);
			in16[k] = _mm_max_epi16(in16[k], min_val);
			in17[k] = _mm_min_epi16(in17[k], max_val);
			in17[k] = _mm_max_epi16(in17[k], min_val);
			in18[k] = _mm_min_epi16(in18[k], max_val);
			in18[k] = _mm_max_epi16(in18[k], min_val);
			in19[k] = _mm_min_epi16(in19[k], max_val);
			in19[k] = _mm_max_epi16(in19[k], min_val);
			in20[k] = _mm_min_epi16(in20[k], max_val);
			in20[k] = _mm_max_epi16(in20[k], min_val);
			in21[k] = _mm_min_epi16(in21[k], max_val);
			in21[k] = _mm_max_epi16(in21[k], min_val);
			in22[k] = _mm_min_epi16(in22[k], max_val);
			in22[k] = _mm_max_epi16(in22[k], min_val);
			in23[k] = _mm_min_epi16(in23[k], max_val);
			in23[k] = _mm_max_epi16(in23[k], min_val);
			in24[k] = _mm_min_epi16(in24[k], max_val);
			in24[k] = _mm_max_epi16(in24[k], min_val);
			in25[k] = _mm_min_epi16(in25[k], max_val);
			in25[k] = _mm_max_epi16(in25[k], min_val);
			in26[k] = _mm_min_epi16(in26[k], max_val);
			in26[k] = _mm_max_epi16(in26[k], min_val);
			in27[k] = _mm_min_epi16(in27[k], max_val);
			in27[k] = _mm_max_epi16(in27[k], min_val);
			in28[k] = _mm_min_epi16(in28[k], max_val);
			in28[k] = _mm_max_epi16(in28[k], min_val);
			in29[k] = _mm_min_epi16(in29[k], max_val);
			in29[k] = _mm_max_epi16(in29[k], min_val);
			in30[k] = _mm_min_epi16(in30[k], max_val);
			in30[k] = _mm_max_epi16(in30[k], min_val);
			in31[k] = _mm_min_epi16(in31[k], max_val);
			in31[k] = _mm_max_epi16(in31[k], min_val);
		}
	}




	// Add

	
	for (k = 0; k < 2; k++)
	{
		int offset = (k << 4);
		P00[k] = _mm_loadu_si128((const __m128i*)&pred[0 + offset]);
		P01[k] = _mm_loadu_si128((const __m128i*)&pred[1 * i_pred + offset]);
		P02[k] = _mm_loadu_si128((const __m128i*)&pred[2 * i_pred + offset]);
		P03[k] = _mm_loadu_si128((const __m128i*)&pred[3 * i_pred + offset]);
		P04[k] = _mm_loadu_si128((const __m128i*)&pred[4 * i_pred + offset]);
		P05[k] = _mm_loadu_si128((const __m128i*)&pred[5 * i_pred + offset]);
		P06[k] = _mm_loadu_si128((const __m128i*)&pred[6 * i_pred + offset]);
		P07[k] = _mm_loadu_si128((const __m128i*)&pred[7 * i_pred + offset]);
		P08[k] = _mm_loadu_si128((const __m128i*)&pred[8 * i_pred + offset]);
		P09[k] = _mm_loadu_si128((const __m128i*)&pred[9 * i_pred + offset]);
		P10[k] = _mm_loadu_si128((const __m128i*)&pred[10 * i_pred + offset]);
		P11[k] = _mm_loadu_si128((const __m128i*)&pred[11 * i_pred + offset]);
		P12[k] = _mm_loadu_si128((const __m128i*)&pred[12 * i_pred + offset]);
		P13[k] = _mm_loadu_si128((const __m128i*)&pred[13 * i_pred + offset]);
		P14[k] = _mm_loadu_si128((const __m128i*)&pred[14 * i_pred + offset]);
		P15[k] = _mm_loadu_si128((const __m128i*)&pred[15 * i_pred + offset]);
		P16[k] = _mm_loadu_si128((const __m128i*)&pred[16 * i_pred + offset]);
		P17[k] = _mm_loadu_si128((const __m128i*)&pred[17 * i_pred + offset]);
		P18[k] = _mm_loadu_si128((const __m128i*)&pred[18 * i_pred + offset]);
		P19[k] = _mm_loadu_si128((const __m128i*)&pred[19 * i_pred + offset]);
		P20[k] = _mm_loadu_si128((const __m128i*)&pred[20 * i_pred + offset]);
		P21[k] = _mm_loadu_si128((const __m128i*)&pred[21 * i_pred + offset]);
		P22[k] = _mm_loadu_si128((const __m128i*)&pred[22 * i_pred + offset]);
		P23[k] = _mm_loadu_si128((const __m128i*)&pred[23 * i_pred + offset]);
		P24[k] = _mm_loadu_si128((const __m128i*)&pred[24 * i_pred + offset]);
		P25[k] = _mm_loadu_si128((const __m128i*)&pred[25 * i_pred + offset]);
		P26[k] = _mm_loadu_si128((const __m128i*)&pred[26 * i_pred + offset]);
		P27[k] = _mm_loadu_si128((const __m128i*)&pred[27 * i_pred + offset]);
		P28[k] = _mm_loadu_si128((const __m128i*)&pred[28 * i_pred + offset]);
		P29[k] = _mm_loadu_si128((const __m128i*)&pred[29 * i_pred + offset]);
		P30[k] = _mm_loadu_si128((const __m128i*)&pred[30 * i_pred + offset]);
		P31[k] = _mm_loadu_si128((const __m128i*)&pred[31 * i_pred + offset]);
	}

	for (k = 0; k < 2; k++)
	{
		int offset = k << 1;
		res00[0 + offset] = _mm_unpacklo_epi8(P00[k], zero);
		res00[1 + offset] = _mm_unpackhi_epi8(P00[k], zero);
		res01[0 + offset] = _mm_unpacklo_epi8(P01[k], zero);
		res01[1 + offset] = _mm_unpackhi_epi8(P01[k], zero);
		res02[0 + offset] = _mm_unpacklo_epi8(P02[k], zero);
		res02[1 + offset] = _mm_unpackhi_epi8(P02[k], zero);
		res03[0 + offset] = _mm_unpacklo_epi8(P03[k], zero);
		res03[1 + offset] = _mm_unpackhi_epi8(P03[k], zero);
		res04[0 + offset] = _mm_unpacklo_epi8(P04[k], zero);
		res04[1 + offset] = _mm_unpackhi_epi8(P04[k], zero);
		res05[0 + offset] = _mm_unpacklo_epi8(P05[k], zero);
		res05[1 + offset] = _mm_unpackhi_epi8(P05[k], zero);
		res06[0 + offset] = _mm_unpacklo_epi8(P06[k], zero);
		res06[1 + offset] = _mm_unpackhi_epi8(P06[k], zero);
		res07[0 + offset] = _mm_unpacklo_epi8(P07[k], zero);
		res07[1 + offset] = _mm_unpackhi_epi8(P07[k], zero);
		res08[0 + offset] = _mm_unpacklo_epi8(P08[k], zero);
		res08[1 + offset] = _mm_unpackhi_epi8(P08[k], zero);
		res09[0 + offset] = _mm_unpacklo_epi8(P09[k], zero);
		res09[1 + offset] = _mm_unpackhi_epi8(P09[k], zero);
		res10[0 + offset] = _mm_unpacklo_epi8(P10[k], zero);
		res10[1 + offset] = _mm_unpackhi_epi8(P10[k], zero);
		res11[0 + offset] = _mm_unpacklo_epi8(P11[k], zero);
		res11[1 + offset] = _mm_unpackhi_epi8(P11[k], zero);
		res12[0 + offset] = _mm_unpacklo_epi8(P12[k], zero);
		res12[1 + offset] = _mm_unpackhi_epi8(P12[k], zero);
		res13[0 + offset] = _mm_unpacklo_epi8(P13[k], zero);
		res13[1 + offset] = _mm_unpackhi_epi8(P13[k], zero);
		res14[0 + offset] = _mm_unpacklo_epi8(P14[k], zero);
		res14[1 + offset] = _mm_unpackhi_epi8(P14[k], zero);
		res15[0 + offset] = _mm_unpacklo_epi8(P15[k], zero);
		res15[1 + offset] = _mm_unpackhi_epi8(P15[k], zero);
		res16[0 + offset] = _mm_unpacklo_epi8(P16[k], zero);
		res16[1 + offset] = _mm_unpackhi_epi8(P16[k], zero);
		res17[0 + offset] = _mm_unpacklo_epi8(P17[k], zero);
		res17[1 + offset] = _mm_unpackhi_epi8(P17[k], zero);
		res18[0 + offset] = _mm_unpacklo_epi8(P18[k], zero);
		res18[1 + offset] = _mm_unpackhi_epi8(P18[k], zero);
		res19[0 + offset] = _mm_unpacklo_epi8(P19[k], zero);
		res19[1 + offset] = _mm_unpackhi_epi8(P19[k], zero);
		res20[0 + offset] = _mm_unpacklo_epi8(P20[k], zero);
		res20[1 + offset] = _mm_unpackhi_epi8(P20[k], zero);
		res21[0 + offset] = _mm_unpacklo_epi8(P21[k], zero);
		res21[1 + offset] = _mm_unpackhi_epi8(P21[k], zero);
		res22[0 + offset] = _mm_unpacklo_epi8(P22[k], zero);
		res22[1 + offset] = _mm_unpackhi_epi8(P22[k], zero);
		res23[0 + offset] = _mm_unpacklo_epi8(P23[k], zero);
		res23[1 + offset] = _mm_unpackhi_epi8(P23[k], zero);
		res24[0 + offset] = _mm_unpacklo_epi8(P24[k], zero);
		res24[1 + offset] = _mm_unpackhi_epi8(P24[k], zero);
		res25[0 + offset] = _mm_unpacklo_epi8(P25[k], zero);
		res25[1 + offset] = _mm_unpackhi_epi8(P25[k], zero);
		res26[0 + offset] = _mm_unpacklo_epi8(P26[k], zero);
		res26[1 + offset] = _mm_unpackhi_epi8(P26[k], zero);
		res27[0 + offset] = _mm_unpacklo_epi8(P27[k], zero);
		res27[1 + offset] = _mm_unpackhi_epi8(P27[k], zero);
		res28[0 + offset] = _mm_unpacklo_epi8(P28[k], zero);
		res28[1 + offset] = _mm_unpackhi_epi8(P28[k], zero);
		res29[0 + offset] = _mm_unpacklo_epi8(P29[k], zero);
		res29[1 + offset] = _mm_unpackhi_epi8(P29[k], zero);
		res30[0 + offset] = _mm_unpacklo_epi8(P30[k], zero);
		res30[1 + offset] = _mm_unpackhi_epi8(P30[k], zero);
		res31[0 + offset] = _mm_unpacklo_epi8(P31[k], zero);
		res31[1 + offset] = _mm_unpackhi_epi8(P31[k], zero);

	}

	for (k = 0; k < 4; k++)
	{
		res00[k] = _mm_add_epi16(in00[k], res00[k]);
		res01[k] = _mm_add_epi16(in01[k], res01[k]);
		res02[k] = _mm_add_epi16(in02[k], res02[k]);
		res03[k] = _mm_add_epi16(in03[k], res03[k]);
		res04[k] = _mm_add_epi16(in04[k], res04[k]);
		res05[k] = _mm_add_epi16(in05[k], res05[k]);
		res06[k] = _mm_add_epi16(in06[k], res06[k]);
		res07[k] = _mm_add_epi16(in07[k], res07[k]);
		res08[k] = _mm_add_epi16(in08[k], res08[k]);
		res09[k] = _mm_add_epi16(in09[k], res09[k]);
		res10[k] = _mm_add_epi16(in10[k], res10[k]);
		res11[k] = _mm_add_epi16(in11[k], res11[k]);
		res12[k] = _mm_add_epi16(in12[k], res12[k]);
		res13[k] = _mm_add_epi16(in13[k], res13[k]);
		res14[k] = _mm_add_epi16(in14[k], res14[k]);
		res15[k] = _mm_add_epi16(in15[k], res15[k]);
		res16[k] = _mm_add_epi16(in16[k], res16[k]);
		res17[k] = _mm_add_epi16(in17[k], res17[k]);
		res18[k] = _mm_add_epi16(in18[k], res18[k]);
		res19[k] = _mm_add_epi16(in19[k], res19[k]);
		res20[k] = _mm_add_epi16(in20[k], res20[k]);
		res21[k] = _mm_add_epi16(in21[k], res21[k]);
		res22[k] = _mm_add_epi16(in22[k], res22[k]);
		res23[k] = _mm_add_epi16(in23[k], res23[k]);
		res24[k] = _mm_add_epi16(in24[k], res24[k]);
		res25[k] = _mm_add_epi16(in25[k], res25[k]);
		res26[k] = _mm_add_epi16(in26[k], res26[k]);
		res27[k] = _mm_add_epi16(in27[k], res27[k]);
		res28[k] = _mm_add_epi16(in28[k], res28[k]);
		res29[k] = _mm_add_epi16(in29[k], res29[k]);
		res30[k] = _mm_add_epi16(in30[k], res30[k]);
		res31[k] = _mm_add_epi16(in31[k], res31[k]);
	}


	for (k = 0; k < 4; k+=2)
	{
		in00[k] = _mm_packus_epi16(res00[k], res00[k + 1]);
		in01[k] = _mm_packus_epi16(res01[k], res01[k + 1]);
		in02[k] = _mm_packus_epi16(res02[k], res02[k + 1]);
		in03[k] = _mm_packus_epi16(res03[k], res03[k + 1]);
		in04[k] = _mm_packus_epi16(res04[k], res04[k + 1]);
		in05[k] = _mm_packus_epi16(res05[k], res05[k + 1]);
		in06[k] = _mm_packus_epi16(res06[k], res06[k + 1]);
		in07[k] = _mm_packus_epi16(res07[k], res07[k + 1]);
		in08[k] = _mm_packus_epi16(res08[k], res08[k + 1]);
		in09[k] = _mm_packus_epi16(res09[k], res09[k + 1]);
		in10[k] = _mm_packus_epi16(res10[k], res10[k + 1]);
		in11[k] = _mm_packus_epi16(res11[k], res11[k + 1]);
		in12[k] = _mm_packus_epi16(res12[k], res12[k + 1]);
		in13[k] = _mm_packus_epi16(res13[k], res13[k + 1]);
		in14[k] = _mm_packus_epi16(res14[k], res14[k + 1]);
		in15[k] = _mm_packus_epi16(res15[k], res15[k + 1]);
		in16[k] = _mm_packus_epi16(res16[k], res16[k + 1]);
		in17[k] = _mm_packus_epi16(res17[k], res17[k + 1]);
		in18[k] = _mm_packus_epi16(res18[k], res18[k + 1]);
		in19[k] = _mm_packus_epi16(res19[k], res19[k + 1]);
		in20[k] = _mm_packus_epi16(res20[k], res20[k + 1]);
		in21[k] = _mm_packus_epi16(res21[k], res21[k + 1]);
		in22[k] = _mm_packus_epi16(res22[k], res22[k + 1]);
		in23[k] = _mm_packus_epi16(res23[k], res23[k + 1]);
		in24[k] = _mm_packus_epi16(res24[k], res24[k + 1]);
		in25[k] = _mm_packus_epi16(res25[k], res25[k + 1]);
		in26[k] = _mm_packus_epi16(res26[k], res26[k + 1]);
		in27[k] = _mm_packus_epi16(res27[k], res27[k + 1]);
		in28[k] = _mm_packus_epi16(res28[k], res28[k + 1]);
		in29[k] = _mm_packus_epi16(res29[k], res29[k + 1]);
		in30[k] = _mm_packus_epi16(res30[k], res30[k + 1]);
		in31[k] = _mm_packus_epi16(res31[k], res31[k + 1]);
	}
	

	for (k = 0; k < 4; k+=2)
	{
		int offset = k <<3;
		_mm_storeu_si128((__m128i*)&dst[0 + offset], in00[k]);
		_mm_storeu_si128((__m128i*)&dst[1 * i_dst + offset], in01[k]);
		_mm_storeu_si128((__m128i*)&dst[2 * i_dst + offset], in02[k]);
		_mm_storeu_si128((__m128i*)&dst[3 * i_dst + offset], in03[k]);
		_mm_storeu_si128((__m128i*)&dst[4 * i_dst + offset], in04[k]);
		_mm_storeu_si128((__m128i*)&dst[5 * i_dst + offset], in05[k]);
		_mm_storeu_si128((__m128i*)&dst[6 * i_dst + offset], in06[k]);
		_mm_storeu_si128((__m128i*)&dst[7 * i_dst + offset], in07[k]);
		_mm_storeu_si128((__m128i*)&dst[8 * i_dst + offset], in08[k]);
		_mm_storeu_si128((__m128i*)&dst[9 * i_dst + offset], in09[k]);

		_mm_storeu_si128((__m128i*)&dst[10 * i_dst + offset], in10[k]);
		_mm_storeu_si128((__m128i*)&dst[11 * i_dst + offset], in11[k]);
		_mm_storeu_si128((__m128i*)&dst[12 * i_dst + offset], in12[k]);
		_mm_storeu_si128((__m128i*)&dst[13 * i_dst + offset], in13[k]);
		_mm_storeu_si128((__m128i*)&dst[14 * i_dst + offset], in14[k]);
		_mm_storeu_si128((__m128i*)&dst[15 * i_dst + offset], in15[k]);
		_mm_storeu_si128((__m128i*)&dst[16 * i_dst + offset], in16[k]);
		_mm_storeu_si128((__m128i*)&dst[17 * i_dst + offset], in17[k]);
		_mm_storeu_si128((__m128i*)&dst[18 * i_dst + offset], in18[k]);
		_mm_storeu_si128((__m128i*)&dst[19 * i_dst + offset], in19[k]);

		_mm_storeu_si128((__m128i*)&dst[20 * i_dst + offset], in20[k]);
		_mm_storeu_si128((__m128i*)&dst[21 * i_dst + offset], in21[k]);
		_mm_storeu_si128((__m128i*)&dst[22 * i_dst + offset], in22[k]);
		_mm_storeu_si128((__m128i*)&dst[23 * i_dst + offset], in23[k]);
		_mm_storeu_si128((__m128i*)&dst[24 * i_dst + offset], in24[k]);
		_mm_storeu_si128((__m128i*)&dst[25 * i_dst + offset], in25[k]);
		_mm_storeu_si128((__m128i*)&dst[26 * i_dst + offset], in26[k]);
		_mm_storeu_si128((__m128i*)&dst[27 * i_dst + offset], in27[k]);
		_mm_storeu_si128((__m128i*)&dst[28 * i_dst + offset], in28[k]);
		_mm_storeu_si128((__m128i*)&dst[29 * i_dst + offset], in29[k]);

		_mm_storeu_si128((__m128i*)&dst[30 * i_dst + offset], in30[k]);
		_mm_storeu_si128((__m128i*)&dst[31 * i_dst + offset], in31[k]);
	}

}

void add_inv_trans_64x64_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    int k;

    __m128i P00[4], P01[4], P02[4], P03[4], P04[4], P05[4], P06[4], P07[4], P08[4], P09[4], P10[4], P11[4], P12[4], P13[4], P14[4], P15[4], P16[4], P17[4], P18[4], P19[4], P20[4], P21[4], P22[4], P23[4], P24[4], P25[4], P26[4], P27[4], P28[4], P29[4];
    __m128i P30[4], P31[4], P32[4], P33[4], P34[4], P35[4], P36[4], P37[4], P38[4], P39[4], P40[4], P41[4], P42[4], P43[4], P44[4], P45[4], P46[4], P47[4], P48[4], P49[4], P50[4], P51[4], P52[4], P53[4], P54[4], P55[4], P56[4], P57[4], P58[4], P59[4];
    __m128i P60[4], P61[4], P62[4], P63[4];
	const __m128i c16_p45_p45 = _mm_set1_epi32(0x002D002D);
	const __m128i c16_p43_p44 = _mm_set1_epi32(0x002B002C);
	const __m128i c16_p39_p41 = _mm_set1_epi32(0x00270029);
	const __m128i c16_p34_p36 = _mm_set1_epi32(0x00220024);
	const __m128i c16_p27_p30 = _mm_set1_epi32(0x001B001E);
	const __m128i c16_p19_p23 = _mm_set1_epi32(0x00130017);
	const __m128i c16_p11_p15 = _mm_set1_epi32(0x000B000F);
	const __m128i c16_p02_p07 = _mm_set1_epi32(0x00020007);
	const __m128i c16_p41_p45 = _mm_set1_epi32(0x0029002D);
	const __m128i c16_p23_p34 = _mm_set1_epi32(0x00170022);
	const __m128i c16_n02_p11 = _mm_set1_epi32(0xFFFE000B);
	const __m128i c16_n27_n15 = _mm_set1_epi32(0xFFE5FFF1);
	const __m128i c16_n43_n36 = _mm_set1_epi32(0xFFD5FFDC);
	const __m128i c16_n44_n45 = _mm_set1_epi32(0xFFD4FFD3);
	const __m128i c16_n30_n39 = _mm_set1_epi32(0xFFE2FFD9);
	const __m128i c16_n07_n19 = _mm_set1_epi32(0xFFF9FFED);
	const __m128i c16_p34_p44 = _mm_set1_epi32(0x0022002C);
	const __m128i c16_n07_p15 = _mm_set1_epi32(0xFFF9000F);
	const __m128i c16_n41_n27 = _mm_set1_epi32(0xFFD7FFE5);
	const __m128i c16_n39_n45 = _mm_set1_epi32(0xFFD9FFD3);
	const __m128i c16_n02_n23 = _mm_set1_epi32(0xFFFEFFE9);
	const __m128i c16_p36_p19 = _mm_set1_epi32(0x00240013);
	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
	const __m128i c16_p11_p30 = _mm_set1_epi32(0x000B001E);
	const __m128i c16_p23_p43 = _mm_set1_epi32(0x0017002B);
	const __m128i c16_n34_n07 = _mm_set1_epi32(0xFFDEFFF9);
	const __m128i c16_n36_n45 = _mm_set1_epi32(0xFFDCFFD3);
	const __m128i c16_p19_n11 = _mm_set1_epi32(0x0013FFF5);
	const __m128i c16_p44_p41 = _mm_set1_epi32(0x002C0029);
	const __m128i c16_n02_p27 = _mm_set1_epi32(0xFFFE001B);
	const __m128i c16_n45_n30 = _mm_set1_epi32(0xFFD3FFE2);
	const __m128i c16_n15_n39 = _mm_set1_epi32(0xFFF1FFD9);
	const __m128i c16_p11_p41 = _mm_set1_epi32(0x000B0029);
	const __m128i c16_n45_n27 = _mm_set1_epi32(0xFFD3FFE5);
	const __m128i c16_p07_n30 = _mm_set1_epi32(0x0007FFE2);
	const __m128i c16_p43_p39 = _mm_set1_epi32(0x002B0027);
	const __m128i c16_n23_p15 = _mm_set1_epi32(0xFFE9000F);
	const __m128i c16_n34_n45 = _mm_set1_epi32(0xFFDEFFD3);
	const __m128i c16_p36_p02 = _mm_set1_epi32(0x00240002);
	const __m128i c16_p19_p44 = _mm_set1_epi32(0x0013002C);
	const __m128i c16_n02_p39 = _mm_set1_epi32(0xFFFE0027);
	const __m128i c16_n36_n41 = _mm_set1_epi32(0xFFDCFFD7);
	const __m128i c16_p43_p07 = _mm_set1_epi32(0x002B0007);
	const __m128i c16_n11_p34 = _mm_set1_epi32(0xFFF50022);
	const __m128i c16_n30_n44 = _mm_set1_epi32(0xFFE2FFD4);
	const __m128i c16_p45_p15 = _mm_set1_epi32(0x002D000F);
	const __m128i c16_n19_p27 = _mm_set1_epi32(0xFFED001B);
	const __m128i c16_n23_n45 = _mm_set1_epi32(0xFFE9FFD3);
	const __m128i c16_n15_p36 = _mm_set1_epi32(0xFFF10024);
	const __m128i c16_n11_n45 = _mm_set1_epi32(0xFFF5FFD3);
	const __m128i c16_p34_p39 = _mm_set1_epi32(0x00220027);
	const __m128i c16_n45_n19 = _mm_set1_epi32(0xFFD3FFED);
	const __m128i c16_p41_n07 = _mm_set1_epi32(0x0029FFF9);
	const __m128i c16_n23_p30 = _mm_set1_epi32(0xFFE9001E);
	const __m128i c16_n02_n44 = _mm_set1_epi32(0xFFFEFFD4);
	const __m128i c16_p27_p43 = _mm_set1_epi32(0x001B002B);
	const __m128i c16_n27_p34 = _mm_set1_epi32(0xFFE50022);
	const __m128i c16_p19_n39 = _mm_set1_epi32(0x0013FFD9);
	const __m128i c16_n11_p43 = _mm_set1_epi32(0xFFF5002B);
	const __m128i c16_p02_n45 = _mm_set1_epi32(0x0002FFD3);
	const __m128i c16_p07_p45 = _mm_set1_epi32(0x0007002D);
	const __m128i c16_n15_n44 = _mm_set1_epi32(0xFFF1FFD4);
	const __m128i c16_p23_p41 = _mm_set1_epi32(0x00170029);
	const __m128i c16_n30_n36 = _mm_set1_epi32(0xFFE2FFDC);
	const __m128i c16_n36_p30 = _mm_set1_epi32(0xFFDC001E);
	const __m128i c16_p41_n23 = _mm_set1_epi32(0x0029FFE9);
	const __m128i c16_n44_p15 = _mm_set1_epi32(0xFFD4000F);
	const __m128i c16_p45_n07 = _mm_set1_epi32(0x002DFFF9);
	const __m128i c16_n45_n02 = _mm_set1_epi32(0xFFD3FFFE);
	const __m128i c16_p43_p11 = _mm_set1_epi32(0x002B000B);
	const __m128i c16_n39_n19 = _mm_set1_epi32(0xFFD9FFED);
	const __m128i c16_p34_p27 = _mm_set1_epi32(0x0022001B);
	const __m128i c16_n43_p27 = _mm_set1_epi32(0xFFD5001B);
	const __m128i c16_p44_n02 = _mm_set1_epi32(0x002CFFFE);
	const __m128i c16_n30_n23 = _mm_set1_epi32(0xFFE2FFE9);
	const __m128i c16_p07_p41 = _mm_set1_epi32(0x00070029);
	const __m128i c16_p19_n45 = _mm_set1_epi32(0x0013FFD3);
	const __m128i c16_n39_p34 = _mm_set1_epi32(0xFFD90022);
	const __m128i c16_p45_n11 = _mm_set1_epi32(0x002DFFF5);
	const __m128i c16_n36_n15 = _mm_set1_epi32(0xFFDCFFF1);
	const __m128i c16_n45_p23 = _mm_set1_epi32(0xFFD30017);
	const __m128i c16_p27_p19 = _mm_set1_epi32(0x001B0013);
	const __m128i c16_p15_n45 = _mm_set1_epi32(0x000FFFD3);
	const __m128i c16_n44_p30 = _mm_set1_epi32(0xFFD4001E);
	const __m128i c16_p34_p11 = _mm_set1_epi32(0x0022000B);
	const __m128i c16_p07_n43 = _mm_set1_epi32(0x0007FFD5);
	const __m128i c16_n41_p36 = _mm_set1_epi32(0xFFD70024);
	const __m128i c16_p39_p02 = _mm_set1_epi32(0x00270002);
	const __m128i c16_n44_p19 = _mm_set1_epi32(0xFFD40013);
	const __m128i c16_n02_p36 = _mm_set1_epi32(0xFFFE0024);
	const __m128i c16_p45_n34 = _mm_set1_epi32(0x002DFFDE);
	const __m128i c16_n15_n23 = _mm_set1_epi32(0xFFF1FFE9);
	const __m128i c16_n39_p43 = _mm_set1_epi32(0xFFD9002B);
	const __m128i c16_p30_p07 = _mm_set1_epi32(0x001E0007);
	const __m128i c16_p27_n45 = _mm_set1_epi32(0x001BFFD3);
	const __m128i c16_n41_p11 = _mm_set1_epi32(0xFFD7000B);
	const __m128i c16_n39_p15 = _mm_set1_epi32(0xFFD9000F);
	const __m128i c16_n30_p45 = _mm_set1_epi32(0xFFE2002D);
	const __m128i c16_p27_p02 = _mm_set1_epi32(0x001B0002);
	const __m128i c16_p41_n44 = _mm_set1_epi32(0x0029FFD4);
	const __m128i c16_n11_n19 = _mm_set1_epi32(0xFFF5FFED);
	const __m128i c16_n45_p36 = _mm_set1_epi32(0xFFD30024);
	const __m128i c16_n07_p34 = _mm_set1_epi32(0xFFF90022);
	const __m128i c16_p43_n23 = _mm_set1_epi32(0x002BFFE9);
	const __m128i c16_n30_p11 = _mm_set1_epi32(0xFFE2000B);
	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);
	const __m128i c16_n19_p36 = _mm_set1_epi32(0xFFED0024);
	const __m128i c16_p23_n02 = _mm_set1_epi32(0x0017FFFE);
	const __m128i c16_p45_n39 = _mm_set1_epi32(0x002DFFD9);
	const __m128i c16_p27_n41 = _mm_set1_epi32(0x001BFFD7);
	const __m128i c16_n15_n07 = _mm_set1_epi32(0xFFF1FFF9);
	const __m128i c16_n44_p34 = _mm_set1_epi32(0xFFD40022);
	const __m128i c16_n19_p07 = _mm_set1_epi32(0xFFED0007);
	const __m128i c16_n39_p30 = _mm_set1_epi32(0xFFD9001E);
	const __m128i c16_n45_p44 = _mm_set1_epi32(0xFFD3002C);
	const __m128i c16_n36_p43 = _mm_set1_epi32(0xFFDC002B);
	const __m128i c16_n15_p27 = _mm_set1_epi32(0xFFF1001B);
	const __m128i c16_p11_p02 = _mm_set1_epi32(0x000B0002);
	const __m128i c16_p34_n23 = _mm_set1_epi32(0x0022FFE9);
	const __m128i c16_p45_n41 = _mm_set1_epi32(0x002DFFD7);
	const __m128i c16_n07_p02 = _mm_set1_epi32(0xFFF90002);
	const __m128i c16_n15_p11 = _mm_set1_epi32(0xFFF1000B);
	const __m128i c16_n23_p19 = _mm_set1_epi32(0xFFE90013);
	const __m128i c16_n30_p27 = _mm_set1_epi32(0xFFE2001B);
	const __m128i c16_n36_p34 = _mm_set1_epi32(0xFFDC0022);
	const __m128i c16_n41_p39 = _mm_set1_epi32(0xFFD70027);
	const __m128i c16_n44_p43 = _mm_set1_epi32(0xFFD4002B);
	const __m128i c16_n45_p45 = _mm_set1_epi32(0xFFD3002D);

	//	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
	const __m128i c16_p35_p40 = _mm_set1_epi32(0x00230028);
	const __m128i c16_p21_p29 = _mm_set1_epi32(0x0015001D);
	const __m128i c16_p04_p13 = _mm_set1_epi32(0x0004000D);
	const __m128i c16_p29_p43 = _mm_set1_epi32(0x001D002B);
	const __m128i c16_n21_p04 = _mm_set1_epi32(0xFFEB0004);
	const __m128i c16_n45_n40 = _mm_set1_epi32(0xFFD3FFD8);
	const __m128i c16_n13_n35 = _mm_set1_epi32(0xFFF3FFDD);
	const __m128i c16_p04_p40 = _mm_set1_epi32(0x00040028);
	const __m128i c16_n43_n35 = _mm_set1_epi32(0xFFD5FFDD);
	const __m128i c16_p29_n13 = _mm_set1_epi32(0x001DFFF3);
	const __m128i c16_p21_p45 = _mm_set1_epi32(0x0015002D);
	const __m128i c16_n21_p35 = _mm_set1_epi32(0xFFEB0023);
	const __m128i c16_p04_n43 = _mm_set1_epi32(0x0004FFD5);
	const __m128i c16_p13_p45 = _mm_set1_epi32(0x000D002D);
	const __m128i c16_n29_n40 = _mm_set1_epi32(0xFFE3FFD8);
	const __m128i c16_n40_p29 = _mm_set1_epi32(0xFFD8001D);
	const __m128i c16_p45_n13 = _mm_set1_epi32(0x002DFFF3);
	const __m128i c16_n43_n04 = _mm_set1_epi32(0xFFD5FFFC);
	const __m128i c16_p35_p21 = _mm_set1_epi32(0x00230015);
	const __m128i c16_n45_p21 = _mm_set1_epi32(0xFFD30015);
	const __m128i c16_p13_p29 = _mm_set1_epi32(0x000D001D);
	const __m128i c16_p35_n43 = _mm_set1_epi32(0x0023FFD5);
	const __m128i c16_n40_p04 = _mm_set1_epi32(0xFFD80004);
	const __m128i c16_n35_p13 = _mm_set1_epi32(0xFFDD000D);
	const __m128i c16_n40_p45 = _mm_set1_epi32(0xFFD8002D);
	const __m128i c16_p04_p21 = _mm_set1_epi32(0x00040015);
	const __m128i c16_p43_n29 = _mm_set1_epi32(0x002BFFE3);
	const __m128i c16_n13_p04 = _mm_set1_epi32(0xFFF30004);
	const __m128i c16_n29_p21 = _mm_set1_epi32(0xFFE30015);
	const __m128i c16_n40_p35 = _mm_set1_epi32(0xFFD80023);
	//	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);


	const __m128i c16_p38_p44 = _mm_set1_epi32(0x0026002C);
	const __m128i c16_p09_p25 = _mm_set1_epi32(0x00090019);
	const __m128i c16_n09_p38 = _mm_set1_epi32(0xFFF70026);
	const __m128i c16_n25_n44 = _mm_set1_epi32(0xFFE7FFD4);

	const __m128i c16_n44_p25 = _mm_set1_epi32(0xFFD40019);
	const __m128i c16_p38_p09 = _mm_set1_epi32(0x00260009);
	const __m128i c16_n25_p09 = _mm_set1_epi32(0xFFE70009);
	const __m128i c16_n44_p38 = _mm_set1_epi32(0xFFD40026);

	const __m128i c16_p17_p42 = _mm_set1_epi32(0x0011002A);
	const __m128i c16_n42_p17 = _mm_set1_epi32(0xFFD60011);

	const __m128i c16_p32_p32 = _mm_set1_epi32(0x00200020);
	const __m128i c16_n32_p32 = _mm_set1_epi32(0xFFE00020);

	__m128i c32_rnd = _mm_set1_epi32(16);
	__m128i zero = _mm_setzero_si128();

	int nShift = 5;
	int i, pass, part;

	// DCT1
	__m128i in00[4], in01[4], in02[4], in03[4], in04[4], in05[4], in06[4], in07[4], in08[4], in09[4], in10[4], in11[4], in12[4], in13[4], in14[4], in15[4];
	__m128i in16[4], in17[4], in18[4], in19[4], in20[4], in21[4], in22[4], in23[4], in24[4], in25[4], in26[4], in27[4], in28[4], in29[4], in30[4], in31[4];
	__m128i res00[4], res01[4], res02[4], res03[4], res04[4], res05[4], res06[4], res07[4], res08[4], res09[4], res10[4], res11[4], res12[4], res13[4], res14[4], res15[4];
	__m128i res16[4], res17[4], res18[4], res19[4], res20[4], res21[4], res22[4], res23[4], res24[4], res25[4], res26[4], res27[4], res28[4], res29[4], res30[4], res31[4];

    //按行 64*64
    __m128i T00[8], T01[8], T02[8], T03[8], T04[8], T05[8], T06[8], T07[8], T08[8], T09[8], T10[8], T11[8], T12[8], T13[8], T14[8], T15[8], T16[8], T17[8], T18[8], T19[8], T20[8], T21[8], T22[8], T23[8], T24[8], T25[8], T26[8], T27[8], T28[8], T29[8], T30[8], T31[8], T32[8], T33[8], T34[8], T35[8], T36[8], T37[8], T38[8], T39[8], T40[8], T41[8], T42[8], T43[8], T44[8], T45[8], T46[8], T47[8], T48[8], T49[8], T50[8], T51[8], T52[8], T53[8], T54[8], T55[8], T56[8], T57[8], T58[8], T59[8], T60[8], T61[8], T62[8], T63[8];

    //按列 16*64
    __m128i V00[8], V01[8], V02[8], V03[8], V04[8], V05[8], V06[8], V07[8], V08[8], V09[8], V10[8], V11[8], V12[8], V13[8], V14[8], V15[8], V16[8], V17[8], V18[8], V19[8], V20[8], V21[8], V22[8], V23[8], V24[8], V25[8], V26[8], V27[8], V28[8], V29[8], V30[8], V31[8], V32[8], V33[8], V34[8], V35[8], V36[8], V37[8], V38[8], V39[8], V40[8], V41[8], V42[8], V43[8], V44[8], V45[8], V46[8], V47[8], V48[8], V49[8], V50[8], V51[8], V52[8], V53[8], V54[8], V55[8], V56[8], V57[8], V58[8], V59[8], V60[8], V61[8], V62[8], V63[8];

    __m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
    __m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;

	for (i = 0; i < 4; i++)
	{
		const int offset = (i << 3);

		in00[i] = _mm_loadu_si128((const __m128i*)&src[0 * 32 + offset]);
		in01[i] = _mm_loadu_si128((const __m128i*)&src[1 * 32 + offset]);
		in02[i] = _mm_loadu_si128((const __m128i*)&src[2 * 32 + offset]);
		in03[i] = _mm_loadu_si128((const __m128i*)&src[3 * 32 + offset]);
		in04[i] = _mm_loadu_si128((const __m128i*)&src[4 * 32 + offset]);
		in05[i] = _mm_loadu_si128((const __m128i*)&src[5 * 32 + offset]);
		in06[i] = _mm_loadu_si128((const __m128i*)&src[6 * 32 + offset]);
		in07[i] = _mm_loadu_si128((const __m128i*)&src[7 * 32 + offset]);
		in08[i] = _mm_loadu_si128((const __m128i*)&src[8 * 32 + offset]);
		in09[i] = _mm_loadu_si128((const __m128i*)&src[9 * 32 + offset]);
		in10[i] = _mm_loadu_si128((const __m128i*)&src[10 * 32 + offset]);
		in11[i] = _mm_loadu_si128((const __m128i*)&src[11 * 32 + offset]);
		in12[i] = _mm_loadu_si128((const __m128i*)&src[12 * 32 + offset]);
		in13[i] = _mm_loadu_si128((const __m128i*)&src[13 * 32 + offset]);
		in14[i] = _mm_loadu_si128((const __m128i*)&src[14 * 32 + offset]);
		in15[i] = _mm_loadu_si128((const __m128i*)&src[15 * 32 + offset]);
		in16[i] = _mm_loadu_si128((const __m128i*)&src[16 * 32 + offset]);
		in17[i] = _mm_loadu_si128((const __m128i*)&src[17 * 32 + offset]);
		in18[i] = _mm_loadu_si128((const __m128i*)&src[18 * 32 + offset]);
		in19[i] = _mm_loadu_si128((const __m128i*)&src[19 * 32 + offset]);
		in20[i] = _mm_loadu_si128((const __m128i*)&src[20 * 32 + offset]);
		in21[i] = _mm_loadu_si128((const __m128i*)&src[21 * 32 + offset]);
		in22[i] = _mm_loadu_si128((const __m128i*)&src[22 * 32 + offset]);
		in23[i] = _mm_loadu_si128((const __m128i*)&src[23 * 32 + offset]);
		in24[i] = _mm_loadu_si128((const __m128i*)&src[24 * 32 + offset]);
		in25[i] = _mm_loadu_si128((const __m128i*)&src[25 * 32 + offset]);
		in26[i] = _mm_loadu_si128((const __m128i*)&src[26 * 32 + offset]);
		in27[i] = _mm_loadu_si128((const __m128i*)&src[27 * 32 + offset]);
		in28[i] = _mm_loadu_si128((const __m128i*)&src[28 * 32 + offset]);
		in29[i] = _mm_loadu_si128((const __m128i*)&src[29 * 32 + offset]);
		in30[i] = _mm_loadu_si128((const __m128i*)&src[30 * 32 + offset]);
		in31[i] = _mm_loadu_si128((const __m128i*)&src[31 * 32 + offset]);
	}

	for (pass = 0; pass < 2; pass++)
	{
		if (pass == 1)
		{
			c32_rnd = _mm_set1_epi32(1024);				// pass == 1 第二次四舍五入
			nShift = 11;
		}

		for (part = 0; part < 4; part++)
		{
			const __m128i T_00_00A = _mm_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
			const __m128i T_00_00B = _mm_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
			const __m128i T_00_01A = _mm_unpacklo_epi16(in05[part], in07[part]);       // [ ]
			const __m128i T_00_01B = _mm_unpackhi_epi16(in05[part], in07[part]);       // [ ]
			const __m128i T_00_02A = _mm_unpacklo_epi16(in09[part], in11[part]);       // [ ]
			const __m128i T_00_02B = _mm_unpackhi_epi16(in09[part], in11[part]);       // [ ]
			const __m128i T_00_03A = _mm_unpacklo_epi16(in13[part], in15[part]);       // [ ]
			const __m128i T_00_03B = _mm_unpackhi_epi16(in13[part], in15[part]);       // [ ]
			const __m128i T_00_04A = _mm_unpacklo_epi16(in17[part], in19[part]);       // [ ]
			const __m128i T_00_04B = _mm_unpackhi_epi16(in17[part], in19[part]);       // [ ]
			const __m128i T_00_05A = _mm_unpacklo_epi16(in21[part], in23[part]);       // [ ]
			const __m128i T_00_05B = _mm_unpackhi_epi16(in21[part], in23[part]);       // [ ]
			const __m128i T_00_06A = _mm_unpacklo_epi16(in25[part], in27[part]);       // [ ]
			const __m128i T_00_06B = _mm_unpackhi_epi16(in25[part], in27[part]);       // [ ]
			const __m128i T_00_07A = _mm_unpacklo_epi16(in29[part], in31[part]);       //
			const __m128i T_00_07B = _mm_unpackhi_epi16(in29[part], in31[part]);       // [ ]

			const __m128i T_00_08A = _mm_unpacklo_epi16(in02[part], in06[part]);       // [ ]
			const __m128i T_00_08B = _mm_unpackhi_epi16(in02[part], in06[part]);       // [ ]
			const __m128i T_00_09A = _mm_unpacklo_epi16(in10[part], in14[part]);       // [ ]
			const __m128i T_00_09B = _mm_unpackhi_epi16(in10[part], in14[part]);       // [ ]
			const __m128i T_00_10A = _mm_unpacklo_epi16(in18[part], in22[part]);       // [ ]
			const __m128i T_00_10B = _mm_unpackhi_epi16(in18[part], in22[part]);       // [ ]
			const __m128i T_00_11A = _mm_unpacklo_epi16(in26[part], in30[part]);       // [ ]
			const __m128i T_00_11B = _mm_unpackhi_epi16(in26[part], in30[part]);       // [ ]

			const __m128i T_00_12A = _mm_unpacklo_epi16(in04[part], in12[part]);       // [ ]
			const __m128i T_00_12B = _mm_unpackhi_epi16(in04[part], in12[part]);       // [ ]
			const __m128i T_00_13A = _mm_unpacklo_epi16(in20[part], in28[part]);       // [ ]
			const __m128i T_00_13B = _mm_unpackhi_epi16(in20[part], in28[part]);       // [ ]

			const __m128i T_00_14A = _mm_unpacklo_epi16(in08[part], in24[part]);       //
			const __m128i T_00_14B = _mm_unpackhi_epi16(in08[part], in24[part]);       // [ ]
			const __m128i T_00_15A = _mm_unpacklo_epi16(in00[part], in16[part]);       //
			const __m128i T_00_15B = _mm_unpackhi_epi16(in00[part], in16[part]);       // [ ]

			__m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
			__m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
			__m128i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
			__m128i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
			{
				__m128i T1, T2, T3, T4;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, r1719, r2123, r2527, r2931, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
	T1 = _mm_add_epi32(_mm_madd_epi16(r0103, c0103), _mm_madd_epi16(r0507, c0507)); \
	T2 = _mm_add_epi32(_mm_madd_epi16(r0911, c0911), _mm_madd_epi16(r1315, c1315)); \
	T3 = _mm_add_epi32(_mm_madd_epi16(r1719, c1719), _mm_madd_epi16(r2123, c2123)); \
	T4 = _mm_add_epi32(_mm_madd_epi16(r2527, c2527), _mm_madd_epi16(r2931, c2931)); \
	row = _mm_add_epi32(_mm_add_epi32(T1, T2), _mm_add_epi32(T3, T4));

				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A)
					COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A)

					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B)
					COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B)

#undef COMPUTE_ROW
			}


			{
				__m128i T1, T2;
#define COMPUTE_ROW(row0206, row1014, row1822, row2630, c0206, c1014, c1822, c2630, row) \
	T1 = _mm_add_epi32(_mm_madd_epi16(row0206, c0206), _mm_madd_epi16(row1014, c1014)); \
	T2 = _mm_add_epi32(_mm_madd_epi16(row1822, c1822), _mm_madd_epi16(row2630, c2630)); \
	row = _mm_add_epi32(T1, T2);

				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A)
					COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A)

					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B)
					COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B)
#undef COMPUTE_ROW
			}
			{
				const __m128i EEO0A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_p38_p44), _mm_madd_epi16(T_00_13A, c16_p09_p25));
				const __m128i EEO1A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n09_p38), _mm_madd_epi16(T_00_13A, c16_n25_n44));
				const __m128i EEO2A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n44_p25), _mm_madd_epi16(T_00_13A, c16_p38_p09));
				const __m128i EEO3A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n25_p09), _mm_madd_epi16(T_00_13A, c16_n44_p38));
				const __m128i EEO0B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_p38_p44), _mm_madd_epi16(T_00_13B, c16_p09_p25));
				const __m128i EEO1B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n09_p38), _mm_madd_epi16(T_00_13B, c16_n25_n44));
				const __m128i EEO2B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n44_p25), _mm_madd_epi16(T_00_13B, c16_p38_p09));
				const __m128i EEO3B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n25_p09), _mm_madd_epi16(T_00_13B, c16_n44_p38));

				const __m128i EEEO0A = _mm_madd_epi16(T_00_14A, c16_p17_p42);
				const __m128i EEEO0B = _mm_madd_epi16(T_00_14B, c16_p17_p42);
				const __m128i EEEO1A = _mm_madd_epi16(T_00_14A, c16_n42_p17);
				const __m128i EEEO1B = _mm_madd_epi16(T_00_14B, c16_n42_p17);

				const __m128i EEEE0A = _mm_madd_epi16(T_00_15A, c16_p32_p32);
				const __m128i EEEE0B = _mm_madd_epi16(T_00_15B, c16_p32_p32);
				const __m128i EEEE1A = _mm_madd_epi16(T_00_15A, c16_n32_p32);
				const __m128i EEEE1B = _mm_madd_epi16(T_00_15B, c16_n32_p32);

				const __m128i EEE0A = _mm_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
				const __m128i EEE0B = _mm_add_epi32(EEEE0B, EEEO0B);
				const __m128i EEE1A = _mm_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
				const __m128i EEE1B = _mm_add_epi32(EEEE1B, EEEO1B);
				const __m128i EEE3A = _mm_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
				const __m128i EEE3B = _mm_sub_epi32(EEEE0B, EEEO0B);
				const __m128i EEE2A = _mm_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
				const __m128i EEE2B = _mm_sub_epi32(EEEE1B, EEEO1B);

				const __m128i EE0A = _mm_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
				const __m128i EE0B = _mm_add_epi32(EEE0B, EEO0B);
				const __m128i EE1A = _mm_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
				const __m128i EE1B = _mm_add_epi32(EEE1B, EEO1B);
				const __m128i EE2A = _mm_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
				const __m128i EE2B = _mm_add_epi32(EEE2B, EEO2B);
				const __m128i EE3A = _mm_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
				const __m128i EE3B = _mm_add_epi32(EEE3B, EEO3B);
				const __m128i EE7A = _mm_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
				const __m128i EE7B = _mm_sub_epi32(EEE0B, EEO0B);
				const __m128i EE6A = _mm_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
				const __m128i EE6B = _mm_sub_epi32(EEE1B, EEO1B);
				const __m128i EE5A = _mm_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
				const __m128i EE5B = _mm_sub_epi32(EEE2B, EEO2B);
				const __m128i EE4A = _mm_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
				const __m128i EE4B = _mm_sub_epi32(EEE3B, EEO3B);

				const __m128i E0A = _mm_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
				const __m128i E0B = _mm_add_epi32(EE0B, EO0B);
				const __m128i E1A = _mm_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
				const __m128i E1B = _mm_add_epi32(EE1B, EO1B);
				const __m128i E2A = _mm_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
				const __m128i E2B = _mm_add_epi32(EE2B, EO2B);
				const __m128i E3A = _mm_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
				const __m128i E3B = _mm_add_epi32(EE3B, EO3B);
				const __m128i E4A = _mm_add_epi32(EE4A, EO4A);          // E4 =
				const __m128i E4B = _mm_add_epi32(EE4B, EO4B);
				const __m128i E5A = _mm_add_epi32(EE5A, EO5A);          // E5 =
				const __m128i E5B = _mm_add_epi32(EE5B, EO5B);
				const __m128i E6A = _mm_add_epi32(EE6A, EO6A);          // E6 =
				const __m128i E6B = _mm_add_epi32(EE6B, EO6B);
				const __m128i E7A = _mm_add_epi32(EE7A, EO7A);          // E7 =
				const __m128i E7B = _mm_add_epi32(EE7B, EO7B);
				const __m128i EFA = _mm_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
				const __m128i EFB = _mm_sub_epi32(EE0B, EO0B);
				const __m128i EEA = _mm_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
				const __m128i EEB = _mm_sub_epi32(EE1B, EO1B);
				const __m128i EDA = _mm_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
				const __m128i EDB = _mm_sub_epi32(EE2B, EO2B);
				const __m128i ECA = _mm_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
				const __m128i ECB = _mm_sub_epi32(EE3B, EO3B);
				const __m128i EBA = _mm_sub_epi32(EE4A, EO4A);          // EB =
				const __m128i EBB = _mm_sub_epi32(EE4B, EO4B);
				const __m128i EAA = _mm_sub_epi32(EE5A, EO5A);          // EA =
				const __m128i EAB = _mm_sub_epi32(EE5B, EO5B);
				const __m128i E9A = _mm_sub_epi32(EE6A, EO6A);          // E9 =
				const __m128i E9B = _mm_sub_epi32(EE6B, EO6B);
				const __m128i E8A = _mm_sub_epi32(EE7A, EO7A);          // E8 =
				const __m128i E8B = _mm_sub_epi32(EE7B, EO7B);

				const __m128i T10A = _mm_add_epi32(E0A, c32_rnd);         // E0 + rnd
				const __m128i T10B = _mm_add_epi32(E0B, c32_rnd);
				const __m128i T11A = _mm_add_epi32(E1A, c32_rnd);         // E1 + rnd
				const __m128i T11B = _mm_add_epi32(E1B, c32_rnd);
				const __m128i T12A = _mm_add_epi32(E2A, c32_rnd);         // E2 + rnd
				const __m128i T12B = _mm_add_epi32(E2B, c32_rnd);
				const __m128i T13A = _mm_add_epi32(E3A, c32_rnd);         // E3 + rnd
				const __m128i T13B = _mm_add_epi32(E3B, c32_rnd);
				const __m128i T14A = _mm_add_epi32(E4A, c32_rnd);         // E4 + rnd
				const __m128i T14B = _mm_add_epi32(E4B, c32_rnd);
				const __m128i T15A = _mm_add_epi32(E5A, c32_rnd);         // E5 + rnd
				const __m128i T15B = _mm_add_epi32(E5B, c32_rnd);
				const __m128i T16A = _mm_add_epi32(E6A, c32_rnd);         // E6 + rnd
				const __m128i T16B = _mm_add_epi32(E6B, c32_rnd);
				const __m128i T17A = _mm_add_epi32(E7A, c32_rnd);         // E7 + rnd
				const __m128i T17B = _mm_add_epi32(E7B, c32_rnd);
				const __m128i T18A = _mm_add_epi32(E8A, c32_rnd);         // E8 + rnd
				const __m128i T18B = _mm_add_epi32(E8B, c32_rnd);
				const __m128i T19A = _mm_add_epi32(E9A, c32_rnd);         // E9 + rnd
				const __m128i T19B = _mm_add_epi32(E9B, c32_rnd);
				const __m128i T1AA = _mm_add_epi32(EAA, c32_rnd);         // E10 + rnd
				const __m128i T1AB = _mm_add_epi32(EAB, c32_rnd);
				const __m128i T1BA = _mm_add_epi32(EBA, c32_rnd);         // E11 + rnd
				const __m128i T1BB = _mm_add_epi32(EBB, c32_rnd);
				const __m128i T1CA = _mm_add_epi32(ECA, c32_rnd);         // E12 + rnd
				const __m128i T1CB = _mm_add_epi32(ECB, c32_rnd);
				const __m128i T1DA = _mm_add_epi32(EDA, c32_rnd);         // E13 + rnd
				const __m128i T1DB = _mm_add_epi32(EDB, c32_rnd);
				const __m128i T1EA = _mm_add_epi32(EEA, c32_rnd);         // E14 + rnd
				const __m128i T1EB = _mm_add_epi32(EEB, c32_rnd);
				const __m128i T1FA = _mm_add_epi32(EFA, c32_rnd);         // E15 + rnd
				const __m128i T1FB = _mm_add_epi32(EFB, c32_rnd);

				const __m128i T2_00A = _mm_add_epi32(T10A, O00A);          // E0 + O0 + rnd
				const __m128i T2_00B = _mm_add_epi32(T10B, O00B);
				const __m128i T2_01A = _mm_add_epi32(T11A, O01A);          // E1 + O1 + rnd
				const __m128i T2_01B = _mm_add_epi32(T11B, O01B);
				const __m128i T2_02A = _mm_add_epi32(T12A, O02A);          // E2 + O2 + rnd
				const __m128i T2_02B = _mm_add_epi32(T12B, O02B);
				const __m128i T2_03A = _mm_add_epi32(T13A, O03A);          // E3 + O3 + rnd
				const __m128i T2_03B = _mm_add_epi32(T13B, O03B);
				const __m128i T2_04A = _mm_add_epi32(T14A, O04A);          // E4
				const __m128i T2_04B = _mm_add_epi32(T14B, O04B);
				const __m128i T2_05A = _mm_add_epi32(T15A, O05A);          // E5
				const __m128i T2_05B = _mm_add_epi32(T15B, O05B);
				const __m128i T2_06A = _mm_add_epi32(T16A, O06A);          // E6
				const __m128i T2_06B = _mm_add_epi32(T16B, O06B);
				const __m128i T2_07A = _mm_add_epi32(T17A, O07A);          // E7
				const __m128i T2_07B = _mm_add_epi32(T17B, O07B);
				const __m128i T2_08A = _mm_add_epi32(T18A, O08A);          // E8
				const __m128i T2_08B = _mm_add_epi32(T18B, O08B);
				const __m128i T2_09A = _mm_add_epi32(T19A, O09A);          // E9
				const __m128i T2_09B = _mm_add_epi32(T19B, O09B);
				const __m128i T2_10A = _mm_add_epi32(T1AA, O10A);          // E10
				const __m128i T2_10B = _mm_add_epi32(T1AB, O10B);
				const __m128i T2_11A = _mm_add_epi32(T1BA, O11A);          // E11
				const __m128i T2_11B = _mm_add_epi32(T1BB, O11B);
				const __m128i T2_12A = _mm_add_epi32(T1CA, O12A);          // E12
				const __m128i T2_12B = _mm_add_epi32(T1CB, O12B);
				const __m128i T2_13A = _mm_add_epi32(T1DA, O13A);          // E13
				const __m128i T2_13B = _mm_add_epi32(T1DB, O13B);
				const __m128i T2_14A = _mm_add_epi32(T1EA, O14A);          // E14
				const __m128i T2_14B = _mm_add_epi32(T1EB, O14B);
				const __m128i T2_15A = _mm_add_epi32(T1FA, O15A);          // E15
				const __m128i T2_15B = _mm_add_epi32(T1FB, O15B);
				const __m128i T2_31A = _mm_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
				const __m128i T2_31B = _mm_sub_epi32(T10B, O00B);
				const __m128i T2_30A = _mm_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
				const __m128i T2_30B = _mm_sub_epi32(T11B, O01B);
				const __m128i T2_29A = _mm_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
				const __m128i T2_29B = _mm_sub_epi32(T12B, O02B);
				const __m128i T2_28A = _mm_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
				const __m128i T2_28B = _mm_sub_epi32(T13B, O03B);
				const __m128i T2_27A = _mm_sub_epi32(T14A, O04A);          // E4
				const __m128i T2_27B = _mm_sub_epi32(T14B, O04B);
				const __m128i T2_26A = _mm_sub_epi32(T15A, O05A);          // E5
				const __m128i T2_26B = _mm_sub_epi32(T15B, O05B);
				const __m128i T2_25A = _mm_sub_epi32(T16A, O06A);          // E6
				const __m128i T2_25B = _mm_sub_epi32(T16B, O06B);
				const __m128i T2_24A = _mm_sub_epi32(T17A, O07A);          // E7
				const __m128i T2_24B = _mm_sub_epi32(T17B, O07B);
				const __m128i T2_23A = _mm_sub_epi32(T18A, O08A);          //
				const __m128i T2_23B = _mm_sub_epi32(T18B, O08B);
				const __m128i T2_22A = _mm_sub_epi32(T19A, O09A);          //
				const __m128i T2_22B = _mm_sub_epi32(T19B, O09B);
				const __m128i T2_21A = _mm_sub_epi32(T1AA, O10A);          //
				const __m128i T2_21B = _mm_sub_epi32(T1AB, O10B);
				const __m128i T2_20A = _mm_sub_epi32(T1BA, O11A);          //
				const __m128i T2_20B = _mm_sub_epi32(T1BB, O11B);
				const __m128i T2_19A = _mm_sub_epi32(T1CA, O12A);          //
				const __m128i T2_19B = _mm_sub_epi32(T1CB, O12B);
				const __m128i T2_18A = _mm_sub_epi32(T1DA, O13A);          //
				const __m128i T2_18B = _mm_sub_epi32(T1DB, O13B);
				const __m128i T2_17A = _mm_sub_epi32(T1EA, O14A);          //
				const __m128i T2_17B = _mm_sub_epi32(T1EB, O14B);
				const __m128i T2_16A = _mm_sub_epi32(T1FA, O15A);          //
				const __m128i T2_16B = _mm_sub_epi32(T1FB, O15B);

				const __m128i T3_00A = _mm_srai_epi32(T2_00A, nShift);             // [30 20 10 00]
				const __m128i T3_00B = _mm_srai_epi32(T2_00B, nShift);             // [70 60 50 40]
				const __m128i T3_01A = _mm_srai_epi32(T2_01A, nShift);             // [31 21 11 01]
				const __m128i T3_01B = _mm_srai_epi32(T2_01B, nShift);             // [71 61 51 41]
				const __m128i T3_02A = _mm_srai_epi32(T2_02A, nShift);             // [32 22 12 02]
				const __m128i T3_02B = _mm_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
				const __m128i T3_03A = _mm_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
				const __m128i T3_03B = _mm_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
				const __m128i T3_04A = _mm_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
				const __m128i T3_04B = _mm_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
				const __m128i T3_05A = _mm_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
				const __m128i T3_05B = _mm_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
				const __m128i T3_06A = _mm_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
				const __m128i T3_06B = _mm_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
				const __m128i T3_07A = _mm_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
				const __m128i T3_07B = _mm_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
				const __m128i T3_08A = _mm_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
				const __m128i T3_08B = _mm_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
				const __m128i T3_09A = _mm_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
				const __m128i T3_09B = _mm_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
				const __m128i T3_10A = _mm_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
				const __m128i T3_10B = _mm_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
				const __m128i T3_11A = _mm_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
				const __m128i T3_11B = _mm_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
				const __m128i T3_12A = _mm_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
				const __m128i T3_12B = _mm_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
				const __m128i T3_13A = _mm_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
				const __m128i T3_13B = _mm_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
				const __m128i T3_14A = _mm_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
				const __m128i T3_14B = _mm_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
				const __m128i T3_15A = _mm_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
				const __m128i T3_15B = _mm_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

				const __m128i T3_16A = _mm_srai_epi32(T2_16A, nShift);             // [30 20 10 00]
				const __m128i T3_16B = _mm_srai_epi32(T2_16B, nShift);             // [70 60 50 40]
				const __m128i T3_17A = _mm_srai_epi32(T2_17A, nShift);             // [31 21 11 01]
				const __m128i T3_17B = _mm_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
				const __m128i T3_18A = _mm_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
				const __m128i T3_18B = _mm_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
				const __m128i T3_19A = _mm_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
				const __m128i T3_19B = _mm_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
				const __m128i T3_20A = _mm_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
				const __m128i T3_20B = _mm_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
				const __m128i T3_21A = _mm_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
				const __m128i T3_21B = _mm_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
				const __m128i T3_22A = _mm_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
				const __m128i T3_22B = _mm_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
				const __m128i T3_23A = _mm_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
				const __m128i T3_23B = _mm_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
				const __m128i T3_24A = _mm_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
				const __m128i T3_24B = _mm_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
				const __m128i T3_25A = _mm_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
				const __m128i T3_25B = _mm_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
				const __m128i T3_26A = _mm_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
				const __m128i T3_26B = _mm_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
				const __m128i T3_27A = _mm_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
				const __m128i T3_27B = _mm_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
				const __m128i T3_28A = _mm_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
				const __m128i T3_28B = _mm_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
				const __m128i T3_29A = _mm_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
				const __m128i T3_29B = _mm_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
				const __m128i T3_30A = _mm_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
				const __m128i T3_30B = _mm_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
				const __m128i T3_31A = _mm_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
				const __m128i T3_31B = _mm_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

				res00[part] = _mm_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
				res01[part] = _mm_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
				res02[part] = _mm_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
				res03[part] = _mm_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
				res04[part] = _mm_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
				res05[part] = _mm_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
				res06[part] = _mm_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
				res07[part] = _mm_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
				res08[part] = _mm_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
				res09[part] = _mm_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
				res10[part] = _mm_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
				res11[part] = _mm_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
				res12[part] = _mm_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
				res13[part] = _mm_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
				res14[part] = _mm_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
				res15[part] = _mm_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
				res16[part] = _mm_packs_epi32(T3_16A, T3_16B);
				res17[part] = _mm_packs_epi32(T3_17A, T3_17B);
				res18[part] = _mm_packs_epi32(T3_18A, T3_18B);
				res19[part] = _mm_packs_epi32(T3_19A, T3_19B);
				res20[part] = _mm_packs_epi32(T3_20A, T3_20B);
				res21[part] = _mm_packs_epi32(T3_21A, T3_21B);
				res22[part] = _mm_packs_epi32(T3_22A, T3_22B);
				res23[part] = _mm_packs_epi32(T3_23A, T3_23B);
				res24[part] = _mm_packs_epi32(T3_24A, T3_24B);
				res25[part] = _mm_packs_epi32(T3_25A, T3_25B);
				res26[part] = _mm_packs_epi32(T3_26A, T3_26B);
				res27[part] = _mm_packs_epi32(T3_27A, T3_27B);
				res28[part] = _mm_packs_epi32(T3_28A, T3_28B);
				res29[part] = _mm_packs_epi32(T3_29A, T3_29B);
				res30[part] = _mm_packs_epi32(T3_30A, T3_30B);
				res31[part] = _mm_packs_epi32(T3_31A, T3_31B);
			}
		}
		//transpose matrix 8x8 16bit.
		{ 
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

			TRANSPOSE_8x8_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0])
				TRANSPOSE_8x8_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0])
				TRANSPOSE_8x8_16BIT(res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0])
				TRANSPOSE_8x8_16BIT(res00[3], res01[3], res02[3], res03[3], res04[3], res05[3], res06[3], res07[3], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0])

				TRANSPOSE_8x8_16BIT(res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1])
				TRANSPOSE_8x8_16BIT(res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1])
				TRANSPOSE_8x8_16BIT(res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1])
				TRANSPOSE_8x8_16BIT(res08[3], res09[3], res10[3], res11[3], res12[3], res13[3], res14[3], res15[3], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1])

				TRANSPOSE_8x8_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2])
				TRANSPOSE_8x8_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2])
				TRANSPOSE_8x8_16BIT(res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2])
				TRANSPOSE_8x8_16BIT(res16[3], res17[3], res18[3], res19[3], res20[3], res21[3], res22[3], res23[3], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2])

				TRANSPOSE_8x8_16BIT(res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[3], in01[3], in02[3], in03[3], in04[3], in05[3], in06[3], in07[3])
				TRANSPOSE_8x8_16BIT(res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in08[3], in09[3], in10[3], in11[3], in12[3], in13[3], in14[3], in15[3])
				TRANSPOSE_8x8_16BIT(res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2], in16[3], in17[3], in18[3], in19[3], in20[3], in21[3], in22[3], in23[3])
				TRANSPOSE_8x8_16BIT(res24[3], res25[3], res26[3], res27[3], res28[3], res29[3], res30[3], res31[3], in24[3], in25[3], in26[3], in27[3], in28[3], in29[3], in30[3], in31[3])



#undef TRANSPOSE_8x8_16BIT
		}
	}


	//clip
	{
		__m128i max_val = _mm_set1_epi16(511);
		__m128i min_val = _mm_set1_epi16(-512);

		for (k = 0; k < 4; k++)
		{
			in00[k] = _mm_min_epi16(in00[k], max_val);
			in00[k] = _mm_max_epi16(in00[k], min_val);
			in01[k] = _mm_min_epi16(in01[k], max_val);
			in01[k] = _mm_max_epi16(in01[k], min_val);
			in02[k] = _mm_min_epi16(in02[k], max_val);
			in02[k] = _mm_max_epi16(in02[k], min_val);
			in03[k] = _mm_min_epi16(in03[k], max_val);
			in03[k] = _mm_max_epi16(in03[k], min_val);
			in04[k] = _mm_min_epi16(in04[k], max_val);
			in04[k] = _mm_max_epi16(in04[k], min_val);
			in05[k] = _mm_min_epi16(in05[k], max_val);
			in05[k] = _mm_max_epi16(in05[k], min_val);
			in06[k] = _mm_min_epi16(in06[k], max_val);
			in06[k] = _mm_max_epi16(in06[k], min_val);
			in07[k] = _mm_min_epi16(in07[k], max_val);
			in07[k] = _mm_max_epi16(in07[k], min_val);
			in08[k] = _mm_min_epi16(in08[k], max_val);
			in08[k] = _mm_max_epi16(in08[k], min_val);
			in09[k] = _mm_min_epi16(in09[k], max_val);
			in09[k] = _mm_max_epi16(in09[k], min_val);
			in10[k] = _mm_min_epi16(in10[k], max_val);
			in10[k] = _mm_max_epi16(in10[k], min_val);
			in11[k] = _mm_min_epi16(in11[k], max_val);
			in11[k] = _mm_max_epi16(in11[k], min_val);
			in12[k] = _mm_min_epi16(in12[k], max_val);
			in12[k] = _mm_max_epi16(in12[k], min_val);
			in13[k] = _mm_min_epi16(in13[k], max_val);
			in13[k] = _mm_max_epi16(in13[k], min_val);
			in14[k] = _mm_min_epi16(in14[k], max_val);
			in14[k] = _mm_max_epi16(in14[k], min_val);
			in15[k] = _mm_min_epi16(in15[k], max_val);
			in15[k] = _mm_max_epi16(in15[k], min_val);
			in16[k] = _mm_min_epi16(in16[k], max_val);
			in16[k] = _mm_max_epi16(in16[k], min_val);
			in17[k] = _mm_min_epi16(in17[k], max_val);
			in17[k] = _mm_max_epi16(in17[k], min_val);
			in18[k] = _mm_min_epi16(in18[k], max_val);
			in18[k] = _mm_max_epi16(in18[k], min_val);
			in19[k] = _mm_min_epi16(in19[k], max_val);
			in19[k] = _mm_max_epi16(in19[k], min_val);
			in20[k] = _mm_min_epi16(in20[k], max_val);
			in20[k] = _mm_max_epi16(in20[k], min_val);
			in21[k] = _mm_min_epi16(in21[k], max_val);
			in21[k] = _mm_max_epi16(in21[k], min_val);
			in22[k] = _mm_min_epi16(in22[k], max_val);
			in22[k] = _mm_max_epi16(in22[k], min_val);
			in23[k] = _mm_min_epi16(in23[k], max_val);
			in23[k] = _mm_max_epi16(in23[k], min_val);
			in24[k] = _mm_min_epi16(in24[k], max_val);
			in24[k] = _mm_max_epi16(in24[k], min_val);
			in25[k] = _mm_min_epi16(in25[k], max_val);
			in25[k] = _mm_max_epi16(in25[k], min_val);
			in26[k] = _mm_min_epi16(in26[k], max_val);
			in26[k] = _mm_max_epi16(in26[k], min_val);
			in27[k] = _mm_min_epi16(in27[k], max_val);
			in27[k] = _mm_max_epi16(in27[k], min_val);
			in28[k] = _mm_min_epi16(in28[k], max_val);
			in28[k] = _mm_max_epi16(in28[k], min_val);
			in29[k] = _mm_min_epi16(in29[k], max_val);
			in29[k] = _mm_max_epi16(in29[k], min_val);
			in30[k] = _mm_min_epi16(in30[k], max_val);
			in30[k] = _mm_max_epi16(in30[k], min_val);
			in31[k] = _mm_min_epi16(in31[k], max_val);
			in31[k] = _mm_max_epi16(in31[k], min_val);
		}
	}

	// Add




	/*--vertical transform--*/
	//32*32, LOAD AND SHIFT
	for (i = 0; i < 4; i++){
		T00[i] = _mm_srai_epi16(in00[i], 1);
		T01[i] = _mm_srai_epi16(in01[i], 1);
		T02[i] = _mm_srai_epi16(in02[i], 1);
		T03[i] = _mm_srai_epi16(in03[i], 1);
		T04[i] = _mm_srai_epi16(in04[i], 1);
		T05[i] = _mm_srai_epi16(in05[i], 1);
		T06[i] = _mm_srai_epi16(in06[i], 1);
		T07[i] = _mm_srai_epi16(in07[i], 1);

		T08[i] = _mm_srai_epi16(in08[i], 1);
		T09[i] = _mm_srai_epi16(in09[i], 1);
		T10[i] = _mm_srai_epi16(in10[i], 1);
		T11[i] = _mm_srai_epi16(in11[i], 1);
		T12[i] = _mm_srai_epi16(in12[i], 1);
		T13[i] = _mm_srai_epi16(in13[i], 1);
		T14[i] = _mm_srai_epi16(in14[i], 1);
		T15[i] = _mm_srai_epi16(in15[i], 1);

		T16[i] = _mm_srai_epi16(in16[i], 1);
		T17[i] = _mm_srai_epi16(in17[i], 1);
		T18[i] = _mm_srai_epi16(in18[i], 1);
		T19[i] = _mm_srai_epi16(in19[i], 1);
		T20[i] = _mm_srai_epi16(in20[i], 1);
		T21[i] = _mm_srai_epi16(in21[i], 1);
		T22[i] = _mm_srai_epi16(in22[i], 1);
		T23[i] = _mm_srai_epi16(in23[i], 1);

		T24[i] = _mm_srai_epi16(in24[i], 1);
		T25[i] = _mm_srai_epi16(in25[i], 1);
		T26[i] = _mm_srai_epi16(in26[i], 1);
		T27[i] = _mm_srai_epi16(in27[i], 1);
		T28[i] = _mm_srai_epi16(in28[i], 1);
		T29[i] = _mm_srai_epi16(in29[i], 1);
		T30[i] = _mm_srai_epi16(in30[i], 1);
		T31[i] = _mm_srai_epi16(in31[i], 1);
	}

	//filter (odd pixel/row)
	for (i = 0; i < 4; i++){
		T32[i] = _mm_srai_epi16(_mm_add_epi16(T00[i], T01[i]), 1);
		T33[i] = _mm_srai_epi16(_mm_add_epi16(T01[i], T02[i]), 1);
		T34[i] = _mm_srai_epi16(_mm_add_epi16(T02[i], T03[i]), 1);
		T35[i] = _mm_srai_epi16(_mm_add_epi16(T03[i], T04[i]), 1);
		T36[i] = _mm_srai_epi16(_mm_add_epi16(T04[i], T05[i]), 1);
		T37[i] = _mm_srai_epi16(_mm_add_epi16(T05[i], T06[i]), 1);
		T38[i] = _mm_srai_epi16(_mm_add_epi16(T06[i], T07[i]), 1);
		T39[i] = _mm_srai_epi16(_mm_add_epi16(T07[i], T08[i]), 1);

		T40[i] = _mm_srai_epi16(_mm_add_epi16(T08[i], T09[i]), 1);
		T41[i] = _mm_srai_epi16(_mm_add_epi16(T09[i], T10[i]), 1);
		T42[i] = _mm_srai_epi16(_mm_add_epi16(T10[i], T11[i]), 1);
		T43[i] = _mm_srai_epi16(_mm_add_epi16(T11[i], T12[i]), 1);
		T44[i] = _mm_srai_epi16(_mm_add_epi16(T12[i], T13[i]), 1);
		T45[i] = _mm_srai_epi16(_mm_add_epi16(T13[i], T14[i]), 1);
		T46[i] = _mm_srai_epi16(_mm_add_epi16(T14[i], T15[i]), 1);
		T47[i] = _mm_srai_epi16(_mm_add_epi16(T15[i], T16[i]), 1);

		T48[i] = _mm_srai_epi16(_mm_add_epi16(T16[i], T17[i]), 1);
		T49[i] = _mm_srai_epi16(_mm_add_epi16(T17[i], T18[i]), 1);
		T50[i] = _mm_srai_epi16(_mm_add_epi16(T18[i], T19[i]), 1);
		T51[i] = _mm_srai_epi16(_mm_add_epi16(T19[i], T20[i]), 1);
		T52[i] = _mm_srai_epi16(_mm_add_epi16(T20[i], T21[i]), 1);
		T53[i] = _mm_srai_epi16(_mm_add_epi16(T21[i], T22[i]), 1);
		T54[i] = _mm_srai_epi16(_mm_add_epi16(T22[i], T23[i]), 1);
		T55[i] = _mm_srai_epi16(_mm_add_epi16(T23[i], T24[i]), 1);

		T56[i] = _mm_srai_epi16(_mm_add_epi16(T24[i], T25[i]), 1);
		T57[i] = _mm_srai_epi16(_mm_add_epi16(T25[i], T26[i]), 1);
		T58[i] = _mm_srai_epi16(_mm_add_epi16(T26[i], T27[i]), 1);
		T59[i] = _mm_srai_epi16(_mm_add_epi16(T27[i], T28[i]), 1);
		T60[i] = _mm_srai_epi16(_mm_add_epi16(T28[i], T29[i]), 1);
		T61[i] = _mm_srai_epi16(_mm_add_epi16(T29[i], T30[i]), 1);
		T62[i] = _mm_srai_epi16(_mm_add_epi16(T30[i], T31[i]), 1);
		T63[i] = _mm_srai_epi16(_mm_add_epi16(T31[i], T31[i]), 1);
	}

	/*--transposition--*/
	//32x64 -> 64x32

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

#define TRANSPOSE_16x16_16BIT(A0_0, A1_0, A2_0, A3_0, A4_0, A5_0, A6_0, A7_0, A8_0, A9_0, A10_0, A11_0, A12_0, A13_0, A14_0, A15_0, A0_1, A1_1, A2_1, A3_1, A4_1, A5_1, A6_1, A7_1, A8_1, A9_1, A10_1, A11_1, A12_1, A13_1, A14_1, A15_1, B0_0, B1_0, B2_0, B3_0, B4_0, B5_0, B6_0, B7_0, B8_0, B9_0, B10_0, B11_0, B12_0, B13_0, B14_0, B15_0, B0_1, B1_1, B2_1, B3_1, B4_1, B5_1, B6_1, B7_1, B8_1, B9_1, B10_1, B11_1, B12_1, B13_1, B14_1, B15_1) \
	TRANSPOSE_8x8_16BIT(A0_0, A1_0, A2_0, A3_0, A4_0, A5_0, A6_0, A7_0, B0_0, B1_0, B2_0, B3_0, B4_0, B5_0, B6_0, B7_0); \
	TRANSPOSE_8x8_16BIT(A8_0, A9_0, A10_0, A11_0, A12_0, A13_0, A14_0, A15_0, B0_1, B1_1, B2_1, B3_1, B4_1, B5_1, B6_1, B7_1); \
	TRANSPOSE_8x8_16BIT(A0_1, A1_1, A2_1, A3_1, A4_1, A5_1, A6_1, A7_1, B8_0, B9_0, B10_0, B11_0, B12_0, B13_0, B14_0, B15_0); \
	TRANSPOSE_8x8_16BIT(A8_1, A9_1, A10_1, A11_1, A12_1, A13_1, A14_1, A15_1, B8_1, B9_1, B10_1, B11_1, B12_1, B13_1, B14_1, B15_1); \





	TRANSPOSE_16x16_16BIT(
		T00[0], T32[0], T01[0], T33[0], T02[0], T34[0], T03[0], T35[0], T04[0], T36[0], T05[0], T37[0], T06[0], T38[0], T07[0], T39[0], T00[1], T32[1], T01[1], T33[1], T02[1], T34[1], T03[1], T35[1], T04[1], T36[1], T05[1], T37[1], T06[1], T38[1], T07[1], T39[1],
		V00[0], V01[0], V02[0], V03[0], V04[0], V05[0], V06[0], V07[0], V08[0], V09[0], V10[0], V11[0], V12[0], V13[0], V14[0], V15[0], V00[1], V01[1], V02[1], V03[1], V04[1], V05[1], V06[1], V07[1], V08[1], V09[1], V10[1], V11[1], V12[1], V13[1], V14[1], V15[1]);
	TRANSPOSE_16x16_16BIT(
		T00[2], T32[2], T01[2], T33[2], T02[2], T34[2], T03[2], T35[2], T04[2], T36[2], T05[2], T37[2], T06[2], T38[2], T07[2], T39[2], T00[3], T32[3], T01[3], T33[3], T02[3], T34[3], T03[3], T35[3], T04[3], T36[3], T05[3], T37[3], T06[3], T38[3], T07[3], T39[3],
		V16[0], V17[0], V18[0], V19[0], V20[0], V21[0], V22[0], V23[0], V24[0], V25[0], V26[0], V27[0], V28[0], V29[0], V30[0], V31[0], V16[1], V17[1], V18[1], V19[1], V20[1], V21[1], V22[1], V23[1], V24[1], V25[1], V26[1], V27[1], V28[1], V29[1], V30[1], V31[1]);

	TRANSPOSE_16x16_16BIT(
		T08[0], T40[0], T09[0], T41[0], T10[0], T42[0], T11[0], T43[0], T12[0], T44[0], T13[0], T45[0], T14[0], T46[0], T15[0], T47[0], T08[1], T40[1], T09[1], T41[1], T10[1], T42[1], T11[1], T43[1], T12[1], T44[1], T13[1], T45[1], T14[1], T46[1], T15[1], T47[1],
		V00[2], V01[2], V02[2], V03[2], V04[2], V05[2], V06[2], V07[2], V08[2], V09[2], V10[2], V11[2], V12[2], V13[2], V14[2], V15[2], V00[3], V01[3], V02[3], V03[3], V04[3], V05[3], V06[3], V07[3], V08[3], V09[3], V10[3], V11[3], V12[3], V13[3], V14[3], V15[3]);
	TRANSPOSE_16x16_16BIT(
		T08[2], T40[2], T09[2], T41[2], T10[2], T42[2], T11[2], T43[2], T12[2], T44[2], T13[2], T45[2], T14[2], T46[2], T15[2], T47[2], T08[3], T40[3], T09[3], T41[3], T10[3], T42[3], T11[3], T43[3], T12[3], T44[3], T13[3], T45[3], T14[3], T46[3], T15[3], T47[3],
		V16[2], V17[2], V18[2], V19[2], V20[2], V21[2], V22[2], V23[2], V24[2], V25[2], V26[2], V27[2], V28[2], V29[2], V30[2], V31[2], V16[3], V17[3], V18[3], V19[3], V20[3], V21[3], V22[3], V23[3], V24[3], V25[3], V26[3], V27[3], V28[3], V29[3], V30[3], V31[3]);

	TRANSPOSE_16x16_16BIT(
		T16[0], T48[0], T17[0], T49[0], T18[0], T50[0], T19[0], T51[0], T20[0], T52[0], T21[0], T53[0], T22[0], T54[0], T23[0], T55[0], T16[1], T48[1], T17[1], T49[1], T18[1], T50[1], T19[1], T51[1], T20[1], T52[1], T21[1], T53[1], T22[1], T54[1], T23[1], T55[1],
		V00[4], V01[4], V02[4], V03[4], V04[4], V05[4], V06[4], V07[4], V08[4], V09[4], V10[4], V11[4], V12[4], V13[4], V14[4], V15[4], V00[5], V01[5], V02[5], V03[5], V04[5], V05[5], V06[5], V07[5], V08[5], V09[5], V10[5], V11[5], V12[5], V13[5], V14[5], V15[5]);
	TRANSPOSE_16x16_16BIT(
		T16[2], T48[2], T17[2], T49[2], T18[2], T50[2], T19[2], T51[2], T20[2], T52[2], T21[2], T53[2], T22[2], T54[2], T23[2], T55[2], T16[3], T48[3], T17[3], T49[3], T18[3], T50[3], T19[3], T51[3], T20[3], T52[3], T21[3], T53[3], T22[3], T54[3], T23[3], T55[3],
		V16[4], V17[4], V18[4], V19[4], V20[4], V21[4], V22[4], V23[4], V24[4], V25[4], V26[4], V27[4], V28[4], V29[4], V30[4], V31[4], V16[5], V17[5], V18[5], V19[5], V20[5], V21[5], V22[5], V23[5], V24[5], V25[5], V26[5], V27[5], V28[5], V29[5], V30[5], V31[5]);

	TRANSPOSE_16x16_16BIT(
		T24[0], T56[0], T25[0], T57[0], T26[0], T58[0], T27[0], T59[0], T28[0], T60[0], T29[0], T61[0], T30[0], T62[0], T31[0], T63[0], T24[1], T56[1], T25[1], T57[1], T26[1], T58[1], T27[1], T59[1], T28[1], T60[1], T29[1], T61[1], T30[1], T62[1], T31[1], T63[1],
		V00[6], V01[6], V02[6], V03[6], V04[6], V05[6], V06[6], V07[6], V08[6], V09[6], V10[6], V11[6], V12[6], V13[6], V14[6], V15[6], V00[7], V01[7], V02[7], V03[7], V04[7], V05[7], V06[7], V07[7], V08[7], V09[7], V10[7], V11[7], V12[7], V13[7], V14[7], V15[7]);
	TRANSPOSE_16x16_16BIT(
		T24[2], T56[2], T25[2], T57[2], T26[2], T58[2], T27[2], T59[2], T28[2], T60[2], T29[2], T61[2], T30[2], T62[2], T31[2], T63[2], T24[3], T56[3], T25[3], T57[3], T26[3], T58[3], T27[3], T59[3], T28[3], T60[3], T29[3], T61[3], T30[3], T62[3], T31[3], T63[3],
		V16[6], V17[6], V18[6], V19[6], V20[6], V21[6], V22[6], V23[6], V24[6], V25[6], V26[6], V27[6], V28[6], V29[6], V30[6], V31[6], V16[7], V17[7], V18[7], V19[7], V20[7], V21[7], V22[7], V23[7], V24[7], V25[7], V26[7], V27[7], V28[7], V29[7], V30[7], V31[7]);

	/*--horizontal transform--*/
	//filter (odd pixel/column)
	for (i = 0; i < 8; i++){
		V32[i] = _mm_srai_epi16(_mm_add_epi16(V00[i], V01[i]), 1);
		V33[i] = _mm_srai_epi16(_mm_add_epi16(V01[i], V02[i]), 1);
		V34[i] = _mm_srai_epi16(_mm_add_epi16(V02[i], V03[i]), 1);
		V35[i] = _mm_srai_epi16(_mm_add_epi16(V03[i], V04[i]), 1);
		V36[i] = _mm_srai_epi16(_mm_add_epi16(V04[i], V05[i]), 1);
		V37[i] = _mm_srai_epi16(_mm_add_epi16(V05[i], V06[i]), 1);
		V38[i] = _mm_srai_epi16(_mm_add_epi16(V06[i], V07[i]), 1);
		V39[i] = _mm_srai_epi16(_mm_add_epi16(V07[i], V08[i]), 1);
		V40[i] = _mm_srai_epi16(_mm_add_epi16(V08[i], V09[i]), 1);
		V41[i] = _mm_srai_epi16(_mm_add_epi16(V09[i], V10[i]), 1);
		V42[i] = _mm_srai_epi16(_mm_add_epi16(V10[i], V11[i]), 1);
		V43[i] = _mm_srai_epi16(_mm_add_epi16(V11[i], V12[i]), 1);
		V44[i] = _mm_srai_epi16(_mm_add_epi16(V12[i], V13[i]), 1);
		V45[i] = _mm_srai_epi16(_mm_add_epi16(V13[i], V14[i]), 1);
		V46[i] = _mm_srai_epi16(_mm_add_epi16(V14[i], V15[i]), 1);
		V47[i] = _mm_srai_epi16(_mm_add_epi16(V15[i], V16[i]), 1);

		V48[i] = _mm_srai_epi16(_mm_add_epi16(V16[i], V17[i]), 1);
		V49[i] = _mm_srai_epi16(_mm_add_epi16(V17[i], V18[i]), 1);
		V50[i] = _mm_srai_epi16(_mm_add_epi16(V18[i], V19[i]), 1);
		V51[i] = _mm_srai_epi16(_mm_add_epi16(V19[i], V20[i]), 1);
		V52[i] = _mm_srai_epi16(_mm_add_epi16(V20[i], V21[i]), 1);
		V53[i] = _mm_srai_epi16(_mm_add_epi16(V21[i], V22[i]), 1);
		V54[i] = _mm_srai_epi16(_mm_add_epi16(V22[i], V23[i]), 1);
		V55[i] = _mm_srai_epi16(_mm_add_epi16(V23[i], V24[i]), 1);
		V56[i] = _mm_srai_epi16(_mm_add_epi16(V24[i], V25[i]), 1);
		V57[i] = _mm_srai_epi16(_mm_add_epi16(V25[i], V26[i]), 1);
		V58[i] = _mm_srai_epi16(_mm_add_epi16(V26[i], V27[i]), 1);
		V59[i] = _mm_srai_epi16(_mm_add_epi16(V27[i], V28[i]), 1);
		V60[i] = _mm_srai_epi16(_mm_add_epi16(V28[i], V29[i]), 1);
		V61[i] = _mm_srai_epi16(_mm_add_epi16(V29[i], V30[i]), 1);
		V62[i] = _mm_srai_epi16(_mm_add_epi16(V30[i], V31[i]), 1);
		V63[i] = _mm_srai_epi16(_mm_add_epi16(V31[i], V31[i]), 1);
	}

	/*--transposition & Store--*/
	//64x64 
	TRANSPOSE_16x16_16BIT(
		V00[0], V32[0], V01[0], V33[0], V02[0], V34[0], V03[0], V35[0], V04[0], V36[0], V05[0], V37[0], V06[0], V38[0], V07[0], V39[0], V00[1], V32[1], V01[1], V33[1], V02[1], V34[1], V03[1], V35[1], V04[1], V36[1], V05[1], V37[1], V06[1], V38[1], V07[1], V39[1],
		T00[0], T01[0], T02[0], T03[0], T04[0], T05[0], T06[0], T07[0], T08[0], T09[0], T10[0], T11[0], T12[0], T13[0], T14[0], T15[0], T00[1], T01[1], T02[1], T03[1], T04[1], T05[1], T06[1], T07[1], T08[1], T09[1], T10[1], T11[1], T12[1], T13[1], T14[1], T15[1]);
	TRANSPOSE_16x16_16BIT(
		V00[2], V32[2], V01[2], V33[2], V02[2], V34[2], V03[2], V35[2], V04[2], V36[2], V05[2], V37[2], V06[2], V38[2], V07[2], V39[2], V00[3], V32[3], V01[3], V33[3], V02[3], V34[3], V03[3], V35[3], V04[3], V36[3], V05[3], V37[3], V06[3], V38[3], V07[3], V39[3],
		T16[0], T17[0], T18[0], T19[0], T20[0], T21[0], T22[0], T23[0], T24[0], T25[0], T26[0], T27[0], T28[0], T29[0], T30[0], T31[0], T16[1], T17[1], T18[1], T19[1], T20[1], T21[1], T22[1], T23[1], T24[1], T25[1], T26[1], T27[1], T28[1], T29[1], T30[1], T31[1]);
	TRANSPOSE_16x16_16BIT(V00[4], V32[4], V01[4], V33[4], V02[4], V34[4], V03[4], V35[4], V04[4], V36[4], V05[4], V37[4], V06[4], V38[4], V07[4], V39[4], V00[5], V32[5], V01[5], V33[5], V02[5], V34[5], V03[5], V35[5], V04[5], V36[5], V05[5], V37[5], V06[5], V38[5], V07[5], V39[5], T32[0], T33[0], T34[0], T35[0], T36[0], T37[0], T38[0], T39[0], T40[0], T41[0], T42[0], T43[0], T44[0], T45[0], T46[0], T47[0], T32[1], T33[1], T34[1], T35[1], T36[1], T37[1], T38[1], T39[1], T40[1], T41[1], T42[1], T43[1], T44[1], T45[1], T46[1], T47[1]);
	TRANSPOSE_16x16_16BIT(V00[6], V32[6], V01[6], V33[6], V02[6], V34[6], V03[6], V35[6], V04[6], V36[6], V05[6], V37[6], V06[6], V38[6], V07[6], V39[6], V00[7], V32[7], V01[7], V33[7], V02[7], V34[7], V03[7], V35[7], V04[7], V36[7], V05[7], V37[7], V06[7], V38[7], V07[7], V39[7], T48[0], T49[0], T50[0], T51[0], T52[0], T53[0], T54[0], T55[0], T56[0], T57[0], T58[0], T59[0], T60[0], T61[0], T62[0], T63[0], T48[1], T49[1], T50[1], T51[1], T52[1], T53[1], T54[1], T55[1], T56[1], T57[1], T58[1], T59[1], T60[1], T61[1], T62[1], T63[1]);

	TRANSPOSE_16x16_16BIT(
		V08[0], V40[0], V09[0], V41[0], V10[0], V42[0], V11[0], V43[0], V12[0], V44[0], V13[0], V45[0], V14[0], V46[0], V15[0], V47[0], V08[1], V40[1], V09[1], V41[1], V10[1], V42[1], V11[1], V43[1], V12[1], V44[1], V13[1], V45[1], V14[1], V46[1], V15[1], V47[1],
		T00[2], T01[2], T02[2], T03[2], T04[2], T05[2], T06[2], T07[2], T08[2], T09[2], T10[2], T11[2], T12[2], T13[2], T14[2], T15[2], T00[3], T01[3], T02[3], T03[3], T04[3], T05[3], T06[3], T07[3], T08[3], T09[3], T10[3], T11[3], T12[3], T13[3], T14[3], T15[3]);
	TRANSPOSE_16x16_16BIT(
		V08[2], V40[2], V09[2], V41[2], V10[2], V42[2], V11[2], V43[2], V12[2], V44[2], V13[2], V45[2], V14[2], V46[2], V15[2], V47[2], V08[3], V40[3], V09[3], V41[3], V10[3], V42[3], V11[3], V43[3], V12[3], V44[3], V13[3], V45[3], V14[3], V46[3], V15[3], V47[3],
		T16[2], T17[2], T18[2], T19[2], T20[2], T21[2], T22[2], T23[2], T24[2], T25[2], T26[2], T27[2], T28[2], T29[2], T30[2], T31[2], T16[3], T17[3], T18[3], T19[3], T20[3], T21[3], T22[3], T23[3], T24[3], T25[3], T26[3], T27[3], T28[3], T29[3], T30[3], T31[3]);
	TRANSPOSE_16x16_16BIT(
		V08[4], V40[4], V09[4], V41[4], V10[4], V42[4], V11[4], V43[4], V12[4], V44[4], V13[4], V45[4], V14[4], V46[4], V15[4], V47[4], V08[5], V40[5], V09[5], V41[5], V10[5], V42[5], V11[5], V43[5], V12[5], V44[5], V13[5], V45[5], V14[5], V46[5], V15[5], V47[5],
		T32[2], T33[2], T34[2], T35[2], T36[2], T37[2], T38[2], T39[2], T40[2], T41[2], T42[2], T43[2], T44[2], T45[2], T46[2], T47[2], T32[3], T33[3], T34[3], T35[3], T36[3], T37[3], T38[3], T39[3], T40[3], T41[3], T42[3], T43[3], T44[3], T45[3], T46[3], T47[3]);
	TRANSPOSE_16x16_16BIT(
		V08[6], V40[6], V09[6], V41[6], V10[6], V42[6], V11[6], V43[6], V12[6], V44[6], V13[6], V45[6], V14[6], V46[6], V15[6], V47[6], V08[7], V40[7], V09[7], V41[7], V10[7], V42[7], V11[7], V43[7], V12[7], V44[7], V13[7], V45[7], V14[7], V46[7], V15[7], V47[7],
		T48[2], T49[2], T50[2], T51[2], T52[2], T53[2], T54[2], T55[2], T56[2], T57[2], T58[2], T59[2], T60[2], T61[2], T62[2], T63[2], T48[3], T49[3], T50[3], T51[3], T52[3], T53[3], T54[3], T55[3], T56[3], T57[3], T58[3], T59[3], T60[3], T61[3], T62[3], T63[3]);

	TRANSPOSE_16x16_16BIT(
		V16[0], V48[0], V17[0], V49[0], V18[0], V50[0], V19[0], V51[0], V20[0], V52[0], V21[0], V53[0], V22[0], V54[0], V23[0], V55[0], V16[1], V48[1], V17[1], V49[1], V18[1], V50[1], V19[1], V51[1], V20[1], V52[1], V21[1], V53[1], V22[1], V54[1], V23[1], V55[1],
		T00[4], T01[4], T02[4], T03[4], T04[4], T05[4], T06[4], T07[4], T08[4], T09[4], T10[4], T11[4], T12[4], T13[4], T14[4], T15[4], T00[5], T01[5], T02[5], T03[5], T04[5], T05[5], T06[5], T07[5], T08[5], T09[5], T10[5], T11[5], T12[5], T13[5], T14[5], T15[5]);
	TRANSPOSE_16x16_16BIT(
		V16[2], V48[2], V17[2], V49[2], V18[2], V50[2], V19[2], V51[2], V20[2], V52[2], V21[2], V53[2], V22[2], V54[2], V23[2], V55[2], V16[3], V48[3], V17[3], V49[3], V18[3], V50[3], V19[3], V51[3], V20[3], V52[3], V21[3], V53[3], V22[3], V54[3], V23[3], V55[3],
		T16[4], T17[4], T18[4], T19[4], T20[4], T21[4], T22[4], T23[4], T24[4], T25[4], T26[4], T27[4], T28[4], T29[4], T30[4], T31[4], T16[5], T17[5], T18[5], T19[5], T20[5], T21[5], T22[5], T23[5], T24[5], T25[5], T26[5], T27[5], T28[5], T29[5], T30[5], T31[5]);
	TRANSPOSE_16x16_16BIT(
		V16[4], V48[4], V17[4], V49[4], V18[4], V50[4], V19[4], V51[4], V20[4], V52[4], V21[4], V53[4], V22[4], V54[4], V23[4], V55[4], V16[5], V48[5], V17[5], V49[5], V18[5], V50[5], V19[5], V51[5], V20[5], V52[5], V21[5], V53[5], V22[5], V54[5], V23[5], V55[5],
		T32[4], T33[4], T34[4], T35[4], T36[4], T37[4], T38[4], T39[4], T40[4], T41[4], T42[4], T43[4], T44[4], T45[4], T46[4], T47[4], T32[5], T33[5], T34[5], T35[5], T36[5], T37[5], T38[5], T39[5], T40[5], T41[5], T42[5], T43[5], T44[5], T45[5], T46[5], T47[5]);
	TRANSPOSE_16x16_16BIT(
		V16[6], V48[6], V17[6], V49[6], V18[6], V50[6], V19[6], V51[6], V20[6], V52[6], V21[6], V53[6], V22[6], V54[6], V23[6], V55[6], V16[7], V48[7], V17[7], V49[7], V18[7], V50[7], V19[7], V51[7], V20[7], V52[7], V21[7], V53[7], V22[7], V54[7], V23[7], V55[7],
		T48[4], T49[4], T50[4], T51[4], T52[4], T53[4], T54[4], T55[4], T56[4], T57[4], T58[4], T59[4], T60[4], T61[4], T62[4], T63[4], T48[5], T49[5], T50[5], T51[5], T52[5], T53[5], T54[5], T55[5], T56[5], T57[5], T58[5], T59[5], T60[5], T61[5], T62[5], T63[5]);

	TRANSPOSE_16x16_16BIT(
		V24[0], V56[0], V25[0], V57[0], V26[0], V58[0], V27[0], V59[0], V28[0], V60[0], V29[0], V61[0], V30[0], V62[0], V31[0], V63[0], V24[1], V56[1], V25[1], V57[1], V26[1], V58[1], V27[1], V59[1], V28[1], V60[1], V29[1], V61[1], V30[1], V62[1], V31[1], V63[1],
		T00[6], T01[6], T02[6], T03[6], T04[6], T05[6], T06[6], T07[6], T08[6], T09[6], T10[6], T11[6], T12[6], T13[6], T14[6], T15[6], T00[7], T01[7], T02[7], T03[7], T04[7], T05[7], T06[7], T07[7], T08[7], T09[7], T10[7], T11[7], T12[7], T13[7], T14[7], T15[7]);
	TRANSPOSE_16x16_16BIT(
		V24[2], V56[2], V25[2], V57[2], V26[2], V58[2], V27[2], V59[2], V28[2], V60[2], V29[2], V61[2], V30[2], V62[2], V31[2], V63[2], V24[3], V56[3], V25[3], V57[3], V26[3], V58[3], V27[3], V59[3], V28[3], V60[3], V29[3], V61[3], V30[3], V62[3], V31[3], V63[3],
		T16[6], T17[6], T18[6], T19[6], T20[6], T21[6], T22[6], T23[6], T24[6], T25[6], T26[6], T27[6], T28[6], T29[6], T30[6], T31[6], T16[7], T17[7], T18[7], T19[7], T20[7], T21[7], T22[7], T23[7], T24[7], T25[7], T26[7], T27[7], T28[7], T29[7], T30[7], T31[7]);
	TRANSPOSE_16x16_16BIT(
		V24[4], V56[4], V25[4], V57[4], V26[4], V58[4], V27[4], V59[4], V28[4], V60[4], V29[4], V61[4], V30[4], V62[4], V31[4], V63[4], V24[5], V56[5], V25[5], V57[5], V26[5], V58[5], V27[5], V59[5], V28[5], V60[5], V29[5], V61[5], V30[5], V62[5], V31[5], V63[5],
		T32[6], T33[6], T34[6], T35[6], T36[6], T37[6], T38[6], T39[6], T40[6], T41[6], T42[6], T43[6], T44[6], T45[6], T46[6], T47[6], T32[7], T33[7], T34[7], T35[7], T36[7], T37[7], T38[7], T39[7], T40[7], T41[7], T42[7], T43[7], T44[7], T45[7], T46[7], T47[7]);
	TRANSPOSE_16x16_16BIT(
		V24[6], V56[6], V25[6], V57[6], V26[6], V58[6], V27[6], V59[6], V28[6], V60[6], V29[6], V61[6], V30[6], V62[6], V31[6], V63[6], V24[7], V56[7], V25[7], V57[7], V26[7], V58[7], V27[7], V59[7], V28[7], V60[7], V29[7], V61[7], V30[7], V62[7], V31[7], V63[7],
		T48[6], T49[6], T50[6], T51[6], T52[6], T53[6], T54[6], T55[6], T56[6], T57[6], T58[6], T59[6], T60[6], T61[6], T62[6], T63[6], T48[7], T49[7], T50[7], T51[7], T52[7], T53[7], T54[7], T55[7], T56[7], T57[7], T58[7], T59[7], T60[7], T61[7], T62[7], T63[7]);


#undef TRANSPOSE_8x8_16BIT
#undef TRANSPOSE_16x16_16BIT


	// Add



	for (k = 0; k < 4; k++)
	{
		int offset = (k << 4);
		P00[k] = _mm_loadu_si128((const __m128i*)&pred[0 + offset]);
		P01[k] = _mm_loadu_si128((const __m128i*)&pred[1 * i_pred + offset]);
		P02[k] = _mm_loadu_si128((const __m128i*)&pred[2 * i_pred + offset]);
		P03[k] = _mm_loadu_si128((const __m128i*)&pred[3 * i_pred + offset]);
		P04[k] = _mm_loadu_si128((const __m128i*)&pred[4 * i_pred + offset]);
		P05[k] = _mm_loadu_si128((const __m128i*)&pred[5 * i_pred + offset]);
		P06[k] = _mm_loadu_si128((const __m128i*)&pred[6 * i_pred + offset]);
		P07[k] = _mm_loadu_si128((const __m128i*)&pred[7 * i_pred + offset]);
		P08[k] = _mm_loadu_si128((const __m128i*)&pred[8 * i_pred + offset]);
		P09[k] = _mm_loadu_si128((const __m128i*)&pred[9 * i_pred + offset]);
		P10[k] = _mm_loadu_si128((const __m128i*)&pred[10 * i_pred + offset]);
		P11[k] = _mm_loadu_si128((const __m128i*)&pred[11 * i_pred + offset]);
		P12[k] = _mm_loadu_si128((const __m128i*)&pred[12 * i_pred + offset]);
		P13[k] = _mm_loadu_si128((const __m128i*)&pred[13 * i_pred + offset]);
		P14[k] = _mm_loadu_si128((const __m128i*)&pred[14 * i_pred + offset]);
		P15[k] = _mm_loadu_si128((const __m128i*)&pred[15 * i_pred + offset]);
		P16[k] = _mm_loadu_si128((const __m128i*)&pred[16 * i_pred + offset]);
		P17[k] = _mm_loadu_si128((const __m128i*)&pred[17 * i_pred + offset]);
		P18[k] = _mm_loadu_si128((const __m128i*)&pred[18 * i_pred + offset]);
		P19[k] = _mm_loadu_si128((const __m128i*)&pred[19 * i_pred + offset]);
		P20[k] = _mm_loadu_si128((const __m128i*)&pred[20 * i_pred + offset]);
		P21[k] = _mm_loadu_si128((const __m128i*)&pred[21 * i_pred + offset]);
		P22[k] = _mm_loadu_si128((const __m128i*)&pred[22 * i_pred + offset]);
		P23[k] = _mm_loadu_si128((const __m128i*)&pred[23 * i_pred + offset]);
		P24[k] = _mm_loadu_si128((const __m128i*)&pred[24 * i_pred + offset]);
		P25[k] = _mm_loadu_si128((const __m128i*)&pred[25 * i_pred + offset]);
		P26[k] = _mm_loadu_si128((const __m128i*)&pred[26 * i_pred + offset]);
		P27[k] = _mm_loadu_si128((const __m128i*)&pred[27 * i_pred + offset]);
		P28[k] = _mm_loadu_si128((const __m128i*)&pred[28 * i_pred + offset]);
		P29[k] = _mm_loadu_si128((const __m128i*)&pred[29 * i_pred + offset]);
		P30[k] = _mm_loadu_si128((const __m128i*)&pred[30 * i_pred + offset]);
		P31[k] = _mm_loadu_si128((const __m128i*)&pred[31 * i_pred + offset]);
		P32[k] = _mm_loadu_si128((const __m128i*)&pred[32 * i_pred + offset]);
		P33[k] = _mm_loadu_si128((const __m128i*)&pred[33 * i_pred + offset]);
		P34[k] = _mm_loadu_si128((const __m128i*)&pred[34 * i_pred + offset]);
		P35[k] = _mm_loadu_si128((const __m128i*)&pred[35 * i_pred + offset]);
		P36[k] = _mm_loadu_si128((const __m128i*)&pred[36 * i_pred + offset]);
		P37[k] = _mm_loadu_si128((const __m128i*)&pred[37 * i_pred + offset]);
		P38[k] = _mm_loadu_si128((const __m128i*)&pred[38 * i_pred + offset]);
		P39[k] = _mm_loadu_si128((const __m128i*)&pred[39 * i_pred + offset]);
		P40[k] = _mm_loadu_si128((const __m128i*)&pred[40 * i_pred + offset]);
		P41[k] = _mm_loadu_si128((const __m128i*)&pred[41 * i_pred + offset]);
		P42[k] = _mm_loadu_si128((const __m128i*)&pred[42 * i_pred + offset]);
		P43[k] = _mm_loadu_si128((const __m128i*)&pred[43 * i_pred + offset]);
		P44[k] = _mm_loadu_si128((const __m128i*)&pred[44 * i_pred + offset]);
		P45[k] = _mm_loadu_si128((const __m128i*)&pred[45 * i_pred + offset]);
		P46[k] = _mm_loadu_si128((const __m128i*)&pred[46 * i_pred + offset]);
		P47[k] = _mm_loadu_si128((const __m128i*)&pred[47 * i_pred + offset]);
		P48[k] = _mm_loadu_si128((const __m128i*)&pred[48 * i_pred + offset]);
		P49[k] = _mm_loadu_si128((const __m128i*)&pred[49 * i_pred + offset]);
		P50[k] = _mm_loadu_si128((const __m128i*)&pred[50 * i_pred + offset]);
		P51[k] = _mm_loadu_si128((const __m128i*)&pred[51 * i_pred + offset]);
		P52[k] = _mm_loadu_si128((const __m128i*)&pred[52 * i_pred + offset]);
		P53[k] = _mm_loadu_si128((const __m128i*)&pred[53 * i_pred + offset]);
		P54[k] = _mm_loadu_si128((const __m128i*)&pred[54 * i_pred + offset]);
		P55[k] = _mm_loadu_si128((const __m128i*)&pred[55 * i_pred + offset]);
		P56[k] = _mm_loadu_si128((const __m128i*)&pred[56 * i_pred + offset]);
		P57[k] = _mm_loadu_si128((const __m128i*)&pred[57 * i_pred + offset]);
		P58[k] = _mm_loadu_si128((const __m128i*)&pred[58 * i_pred + offset]);
		P59[k] = _mm_loadu_si128((const __m128i*)&pred[59 * i_pred + offset]);
		P60[k] = _mm_loadu_si128((const __m128i*)&pred[60 * i_pred + offset]);
		P61[k] = _mm_loadu_si128((const __m128i*)&pred[61 * i_pred + offset]);
		P62[k] = _mm_loadu_si128((const __m128i*)&pred[62 * i_pred + offset]);
		P63[k] = _mm_loadu_si128((const __m128i*)&pred[63 * i_pred + offset]);
	}

	for (k = 0; k < 4; k++)
	{
		int offset = k << 1;
		V00[0 + offset] = _mm_unpacklo_epi8(P00[k], zero);
		V00[1 + offset] = _mm_unpackhi_epi8(P00[k], zero);
		V01[0 + offset] = _mm_unpacklo_epi8(P01[k], zero);
		V01[1 + offset] = _mm_unpackhi_epi8(P01[k], zero);
		V02[0 + offset] = _mm_unpacklo_epi8(P02[k], zero);
		V02[1 + offset] = _mm_unpackhi_epi8(P02[k], zero);
		V03[0 + offset] = _mm_unpacklo_epi8(P03[k], zero);
		V03[1 + offset] = _mm_unpackhi_epi8(P03[k], zero);
		V04[0 + offset] = _mm_unpacklo_epi8(P04[k], zero);
		V04[1 + offset] = _mm_unpackhi_epi8(P04[k], zero);
		V05[0 + offset] = _mm_unpacklo_epi8(P05[k], zero);
		V05[1 + offset] = _mm_unpackhi_epi8(P05[k], zero);
		V06[0 + offset] = _mm_unpacklo_epi8(P06[k], zero);
		V06[1 + offset] = _mm_unpackhi_epi8(P06[k], zero);
		V07[0 + offset] = _mm_unpacklo_epi8(P07[k], zero);
		V07[1 + offset] = _mm_unpackhi_epi8(P07[k], zero);
		V08[0 + offset] = _mm_unpacklo_epi8(P08[k], zero);
		V08[1 + offset] = _mm_unpackhi_epi8(P08[k], zero);
		V09[0 + offset] = _mm_unpacklo_epi8(P09[k], zero);
		V09[1 + offset] = _mm_unpackhi_epi8(P09[k], zero);
		V10[0 + offset] = _mm_unpacklo_epi8(P10[k], zero);
		V10[1 + offset] = _mm_unpackhi_epi8(P10[k], zero);
		V11[0 + offset] = _mm_unpacklo_epi8(P11[k], zero);
		V11[1 + offset] = _mm_unpackhi_epi8(P11[k], zero);
		V12[0 + offset] = _mm_unpacklo_epi8(P12[k], zero);
		V12[1 + offset] = _mm_unpackhi_epi8(P12[k], zero);
		V13[0 + offset] = _mm_unpacklo_epi8(P13[k], zero);
		V13[1 + offset] = _mm_unpackhi_epi8(P13[k], zero);
		V14[0 + offset] = _mm_unpacklo_epi8(P14[k], zero);
		V14[1 + offset] = _mm_unpackhi_epi8(P14[k], zero);
		V15[0 + offset] = _mm_unpacklo_epi8(P15[k], zero);
		V15[1 + offset] = _mm_unpackhi_epi8(P15[k], zero);
		V16[0 + offset] = _mm_unpacklo_epi8(P16[k], zero);
		V16[1 + offset] = _mm_unpackhi_epi8(P16[k], zero);
		V17[0 + offset] = _mm_unpacklo_epi8(P17[k], zero);
		V17[1 + offset] = _mm_unpackhi_epi8(P17[k], zero);
		V18[0 + offset] = _mm_unpacklo_epi8(P18[k], zero);
		V18[1 + offset] = _mm_unpackhi_epi8(P18[k], zero);
		V19[0 + offset] = _mm_unpacklo_epi8(P19[k], zero);
		V19[1 + offset] = _mm_unpackhi_epi8(P19[k], zero);
		V20[0 + offset] = _mm_unpacklo_epi8(P20[k], zero);
		V20[1 + offset] = _mm_unpackhi_epi8(P20[k], zero);
		V21[0 + offset] = _mm_unpacklo_epi8(P21[k], zero);
		V21[1 + offset] = _mm_unpackhi_epi8(P21[k], zero);
		V22[0 + offset] = _mm_unpacklo_epi8(P22[k], zero);
		V22[1 + offset] = _mm_unpackhi_epi8(P22[k], zero);
		V23[0 + offset] = _mm_unpacklo_epi8(P23[k], zero);
		V23[1 + offset] = _mm_unpackhi_epi8(P23[k], zero);
		V24[0 + offset] = _mm_unpacklo_epi8(P24[k], zero);
		V24[1 + offset] = _mm_unpackhi_epi8(P24[k], zero);
		V25[0 + offset] = _mm_unpacklo_epi8(P25[k], zero);
		V25[1 + offset] = _mm_unpackhi_epi8(P25[k], zero);
		V26[0 + offset] = _mm_unpacklo_epi8(P26[k], zero);
		V26[1 + offset] = _mm_unpackhi_epi8(P26[k], zero);
		V27[0 + offset] = _mm_unpacklo_epi8(P27[k], zero);
		V27[1 + offset] = _mm_unpackhi_epi8(P27[k], zero);
		V28[0 + offset] = _mm_unpacklo_epi8(P28[k], zero);
		V28[1 + offset] = _mm_unpackhi_epi8(P28[k], zero);
		V29[0 + offset] = _mm_unpacklo_epi8(P29[k], zero);
		V29[1 + offset] = _mm_unpackhi_epi8(P29[k], zero);


		V30[0 + offset] = _mm_unpacklo_epi8(P30[k], zero);
		V30[1 + offset] = _mm_unpackhi_epi8(P30[k], zero);
		V31[0 + offset] = _mm_unpacklo_epi8(P31[k], zero);
		V31[1 + offset] = _mm_unpackhi_epi8(P31[k], zero);
		V32[0 + offset] = _mm_unpacklo_epi8(P32[k], zero);
		V32[1 + offset] = _mm_unpackhi_epi8(P32[k], zero);
		V33[0 + offset] = _mm_unpacklo_epi8(P33[k], zero);
		V33[1 + offset] = _mm_unpackhi_epi8(P33[k], zero);
		V34[0 + offset] = _mm_unpacklo_epi8(P34[k], zero);
		V34[1 + offset] = _mm_unpackhi_epi8(P34[k], zero);
		V35[0 + offset] = _mm_unpacklo_epi8(P35[k], zero);
		V35[1 + offset] = _mm_unpackhi_epi8(P35[k], zero);
		V36[0 + offset] = _mm_unpacklo_epi8(P36[k], zero);
		V36[1 + offset] = _mm_unpackhi_epi8(P36[k], zero);
		V37[0 + offset] = _mm_unpacklo_epi8(P37[k], zero);
		V37[1 + offset] = _mm_unpackhi_epi8(P37[k], zero);
		V38[0 + offset] = _mm_unpacklo_epi8(P38[k], zero);
		V38[1 + offset] = _mm_unpackhi_epi8(P38[k], zero);
		V39[0 + offset] = _mm_unpacklo_epi8(P39[k], zero);
		V39[1 + offset] = _mm_unpackhi_epi8(P39[k], zero);


		V40[0 + offset] = _mm_unpacklo_epi8(P40[k], zero);
		V40[1 + offset] = _mm_unpackhi_epi8(P40[k], zero);
		V41[0 + offset] = _mm_unpacklo_epi8(P41[k], zero);
		V41[1 + offset] = _mm_unpackhi_epi8(P41[k], zero);
		V42[0 + offset] = _mm_unpacklo_epi8(P42[k], zero);
		V42[1 + offset] = _mm_unpackhi_epi8(P42[k], zero);
		V43[0 + offset] = _mm_unpacklo_epi8(P43[k], zero);
		V43[1 + offset] = _mm_unpackhi_epi8(P43[k], zero);
		V44[0 + offset] = _mm_unpacklo_epi8(P44[k], zero);
		V44[1 + offset] = _mm_unpackhi_epi8(P44[k], zero);
		V45[0 + offset] = _mm_unpacklo_epi8(P45[k], zero);
		V45[1 + offset] = _mm_unpackhi_epi8(P45[k], zero);
		V46[0 + offset] = _mm_unpacklo_epi8(P46[k], zero);
		V46[1 + offset] = _mm_unpackhi_epi8(P46[k], zero);
		V47[0 + offset] = _mm_unpacklo_epi8(P47[k], zero);
		V47[1 + offset] = _mm_unpackhi_epi8(P47[k], zero);
		V48[0 + offset] = _mm_unpacklo_epi8(P48[k], zero);
		V48[1 + offset] = _mm_unpackhi_epi8(P48[k], zero);
		V49[0 + offset] = _mm_unpacklo_epi8(P49[k], zero);
		V49[1 + offset] = _mm_unpackhi_epi8(P49[k], zero);


		V50[0 + offset] = _mm_unpacklo_epi8(P50[k], zero);
		V50[1 + offset] = _mm_unpackhi_epi8(P50[k], zero);
		V51[0 + offset] = _mm_unpacklo_epi8(P51[k], zero);
		V51[1 + offset] = _mm_unpackhi_epi8(P51[k], zero);
		V52[0 + offset] = _mm_unpacklo_epi8(P52[k], zero);
		V52[1 + offset] = _mm_unpackhi_epi8(P52[k], zero);
		V53[0 + offset] = _mm_unpacklo_epi8(P53[k], zero);
		V53[1 + offset] = _mm_unpackhi_epi8(P53[k], zero);
		V54[0 + offset] = _mm_unpacklo_epi8(P54[k], zero);
		V54[1 + offset] = _mm_unpackhi_epi8(P54[k], zero);
		V55[0 + offset] = _mm_unpacklo_epi8(P55[k], zero);
		V55[1 + offset] = _mm_unpackhi_epi8(P55[k], zero);
		V56[0 + offset] = _mm_unpacklo_epi8(P56[k], zero);
		V56[1 + offset] = _mm_unpackhi_epi8(P56[k], zero);
		V57[0 + offset] = _mm_unpacklo_epi8(P57[k], zero);
		V57[1 + offset] = _mm_unpackhi_epi8(P57[k], zero);
		V58[0 + offset] = _mm_unpacklo_epi8(P58[k], zero);
		V58[1 + offset] = _mm_unpackhi_epi8(P58[k], zero);
		V59[0 + offset] = _mm_unpacklo_epi8(P59[k], zero);
		V59[1 + offset] = _mm_unpackhi_epi8(P59[k], zero);


		V60[0 + offset] = _mm_unpacklo_epi8(P60[k], zero);
		V60[1 + offset] = _mm_unpackhi_epi8(P60[k], zero);
		V61[0 + offset] = _mm_unpacklo_epi8(P61[k], zero);
		V61[1 + offset] = _mm_unpackhi_epi8(P61[k], zero);
		V62[0 + offset] = _mm_unpacklo_epi8(P62[k], zero);
		V62[1 + offset] = _mm_unpackhi_epi8(P62[k], zero);
		V63[0 + offset] = _mm_unpacklo_epi8(P63[k], zero);
		V63[1 + offset] = _mm_unpackhi_epi8(P63[k], zero);
	}


	for (k = 0; k < 8; k++)
	{
		T00[k] = _mm_add_epi16(V00[k], T00[k]);
		T01[k] = _mm_add_epi16(V01[k], T01[k]);
		T02[k] = _mm_add_epi16(V02[k], T02[k]);
		T03[k] = _mm_add_epi16(V03[k], T03[k]);
		T04[k] = _mm_add_epi16(V04[k], T04[k]);
		T05[k] = _mm_add_epi16(V05[k], T05[k]);
		T06[k] = _mm_add_epi16(V06[k], T06[k]);
		T07[k] = _mm_add_epi16(V07[k], T07[k]);
		T08[k] = _mm_add_epi16(V08[k], T08[k]);
		T09[k] = _mm_add_epi16(V09[k], T09[k]);
		T10[k] = _mm_add_epi16(V10[k], T10[k]);
		T11[k] = _mm_add_epi16(V11[k], T11[k]);
		T12[k] = _mm_add_epi16(V12[k], T12[k]);
		T13[k] = _mm_add_epi16(V13[k], T13[k]);
		T14[k] = _mm_add_epi16(V14[k], T14[k]);
		T15[k] = _mm_add_epi16(V15[k], T15[k]);
		T16[k] = _mm_add_epi16(V16[k], T16[k]);
		T17[k] = _mm_add_epi16(V17[k], T17[k]);
		T18[k] = _mm_add_epi16(V18[k], T18[k]);
		T19[k] = _mm_add_epi16(V19[k], T19[k]);
		T20[k] = _mm_add_epi16(V20[k], T20[k]);
		T21[k] = _mm_add_epi16(V21[k], T21[k]);
		T22[k] = _mm_add_epi16(V22[k], T22[k]);
		T23[k] = _mm_add_epi16(V23[k], T23[k]);
		T24[k] = _mm_add_epi16(V24[k], T24[k]);
		T25[k] = _mm_add_epi16(V25[k], T25[k]);
		T26[k] = _mm_add_epi16(V26[k], T26[k]);
		T27[k] = _mm_add_epi16(V27[k], T27[k]);
		T28[k] = _mm_add_epi16(V28[k], T28[k]);
		T29[k] = _mm_add_epi16(V29[k], T29[k]);
		
		T30[k] = _mm_add_epi16(V30[k], T30[k]);
		T31[k] = _mm_add_epi16(V31[k], T31[k]);
		T32[k] = _mm_add_epi16(V32[k], T32[k]);
		T33[k] = _mm_add_epi16(V33[k], T33[k]);
		T34[k] = _mm_add_epi16(V34[k], T34[k]);
		T35[k] = _mm_add_epi16(V35[k], T35[k]);
		T36[k] = _mm_add_epi16(V36[k], T36[k]);
		T37[k] = _mm_add_epi16(V37[k], T37[k]);
		T38[k] = _mm_add_epi16(V38[k], T38[k]);
		T39[k] = _mm_add_epi16(V39[k], T39[k]);

		T40[k] = _mm_add_epi16(V40[k], T40[k]);
		T41[k] = _mm_add_epi16(V41[k], T41[k]);
		T42[k] = _mm_add_epi16(V42[k], T42[k]);
		T43[k] = _mm_add_epi16(V43[k], T43[k]);
		T44[k] = _mm_add_epi16(V44[k], T44[k]);
		T45[k] = _mm_add_epi16(V45[k], T45[k]);
		T46[k] = _mm_add_epi16(V46[k], T46[k]);
		T47[k] = _mm_add_epi16(V47[k], T47[k]);
		T48[k] = _mm_add_epi16(V48[k], T48[k]);
		T49[k] = _mm_add_epi16(V49[k], T49[k]);

		T50[k] = _mm_add_epi16(V50[k], T50[k]);
		T51[k] = _mm_add_epi16(V51[k], T51[k]);
		T52[k] = _mm_add_epi16(V52[k], T52[k]);
		T53[k] = _mm_add_epi16(V53[k], T53[k]);
		T54[k] = _mm_add_epi16(V54[k], T54[k]);
		T55[k] = _mm_add_epi16(V55[k], T55[k]);
		T56[k] = _mm_add_epi16(V56[k], T56[k]);
		T57[k] = _mm_add_epi16(V57[k], T57[k]);
		T58[k] = _mm_add_epi16(V58[k], T58[k]);
		T59[k] = _mm_add_epi16(V59[k], T59[k]);

		T60[k] = _mm_add_epi16(V60[k], T60[k]);
		T61[k] = _mm_add_epi16(V61[k], T61[k]);
		T62[k] = _mm_add_epi16(V62[k], T62[k]);
		T63[k] = _mm_add_epi16(V63[k], T63[k]);

	}


	for (k = 0; k < 8; k += 2)
	{
		V00[k] = _mm_packus_epi16(T00[k], T00[k + 1]);
		V01[k] = _mm_packus_epi16(T01[k], T01[k + 1]);
		V02[k] = _mm_packus_epi16(T02[k], T02[k + 1]);
		V03[k] = _mm_packus_epi16(T03[k], T03[k + 1]);
		V04[k] = _mm_packus_epi16(T04[k], T04[k + 1]);
		V05[k] = _mm_packus_epi16(T05[k], T05[k + 1]);
		V06[k] = _mm_packus_epi16(T06[k], T06[k + 1]);
		V07[k] = _mm_packus_epi16(T07[k], T07[k + 1]);
		V08[k] = _mm_packus_epi16(T08[k], T08[k + 1]);
		V09[k] = _mm_packus_epi16(T09[k], T09[k + 1]);
		V10[k] = _mm_packus_epi16(T10[k], T10[k + 1]);
		V11[k] = _mm_packus_epi16(T11[k], T11[k + 1]);
		V12[k] = _mm_packus_epi16(T12[k], T12[k + 1]);
		V13[k] = _mm_packus_epi16(T13[k], T13[k + 1]);
		V14[k] = _mm_packus_epi16(T14[k], T14[k + 1]);
		V15[k] = _mm_packus_epi16(T15[k], T15[k + 1]);
		V16[k] = _mm_packus_epi16(T16[k], T16[k + 1]);
		V17[k] = _mm_packus_epi16(T17[k], T17[k + 1]);
		V18[k] = _mm_packus_epi16(T18[k], T18[k + 1]);
		V19[k] = _mm_packus_epi16(T19[k], T19[k + 1]);
		V20[k] = _mm_packus_epi16(T20[k], T20[k + 1]);
		V21[k] = _mm_packus_epi16(T21[k], T21[k + 1]);
		V22[k] = _mm_packus_epi16(T22[k], T22[k + 1]);
		V23[k] = _mm_packus_epi16(T23[k], T23[k + 1]);
		V24[k] = _mm_packus_epi16(T24[k], T24[k + 1]);
		V25[k] = _mm_packus_epi16(T25[k], T25[k + 1]);
		V26[k] = _mm_packus_epi16(T26[k], T26[k + 1]);
		V27[k] = _mm_packus_epi16(T27[k], T27[k + 1]);
		V28[k] = _mm_packus_epi16(T28[k], T28[k + 1]);
		V29[k] = _mm_packus_epi16(T29[k], T29[k + 1]);
		V30[k] = _mm_packus_epi16(T30[k], T30[k + 1]);
		V31[k] = _mm_packus_epi16(T31[k], T31[k + 1]);
		V32[k] = _mm_packus_epi16(T32[k], T32[k + 1]);
		V33[k] = _mm_packus_epi16(T33[k], T33[k + 1]);
		V34[k] = _mm_packus_epi16(T34[k], T34[k + 1]);
		V35[k] = _mm_packus_epi16(T35[k], T35[k + 1]);
		V36[k] = _mm_packus_epi16(T36[k], T36[k + 1]);
		V37[k] = _mm_packus_epi16(T37[k], T37[k + 1]);
		V38[k] = _mm_packus_epi16(T38[k], T38[k + 1]);
		V39[k] = _mm_packus_epi16(T39[k], T39[k + 1]);
		V40[k] = _mm_packus_epi16(T40[k], T40[k + 1]);
		V41[k] = _mm_packus_epi16(T41[k], T41[k + 1]);
		V42[k] = _mm_packus_epi16(T42[k], T42[k + 1]);
		V43[k] = _mm_packus_epi16(T43[k], T43[k + 1]);
		V44[k] = _mm_packus_epi16(T44[k], T44[k + 1]);
		V45[k] = _mm_packus_epi16(T45[k], T45[k + 1]);
		V46[k] = _mm_packus_epi16(T46[k], T46[k + 1]);
		V47[k] = _mm_packus_epi16(T47[k], T47[k + 1]);
		V48[k] = _mm_packus_epi16(T48[k], T48[k + 1]);
		V49[k] = _mm_packus_epi16(T49[k], T49[k + 1]);
		V50[k] = _mm_packus_epi16(T50[k], T50[k + 1]);
		V51[k] = _mm_packus_epi16(T51[k], T51[k + 1]);
		V52[k] = _mm_packus_epi16(T52[k], T52[k + 1]);
		V53[k] = _mm_packus_epi16(T53[k], T53[k + 1]);
		V54[k] = _mm_packus_epi16(T54[k], T54[k + 1]);
		V55[k] = _mm_packus_epi16(T55[k], T55[k + 1]);
		V56[k] = _mm_packus_epi16(T56[k], T56[k + 1]);
		V57[k] = _mm_packus_epi16(T57[k], T57[k + 1]);
		V58[k] = _mm_packus_epi16(T58[k], T58[k + 1]);
		V59[k] = _mm_packus_epi16(T59[k], T59[k + 1]);
		V60[k] = _mm_packus_epi16(T60[k], T60[k + 1]);
		V61[k] = _mm_packus_epi16(T61[k], T61[k + 1]);
		V62[k] = _mm_packus_epi16(T62[k], T62[k + 1]);
		V63[k] = _mm_packus_epi16(T63[k], T63[k + 1]);
	}


	for (k = 0; k < 8; k += 2)
	{
		int offset = k << 3;
		_mm_storeu_si128((__m128i*)&dst[0 + offset], V00[k]);
		_mm_storeu_si128((__m128i*)&dst[1 * i_dst + offset], V01[k]);
		_mm_storeu_si128((__m128i*)&dst[2 * i_dst + offset], V02[k]);
		_mm_storeu_si128((__m128i*)&dst[3 * i_dst + offset], V03[k]);
		_mm_storeu_si128((__m128i*)&dst[4 * i_dst + offset], V04[k]);
		_mm_storeu_si128((__m128i*)&dst[5 * i_dst + offset], V05[k]);
		_mm_storeu_si128((__m128i*)&dst[6 * i_dst + offset], V06[k]);
		_mm_storeu_si128((__m128i*)&dst[7 * i_dst + offset], V07[k]);
		_mm_storeu_si128((__m128i*)&dst[8 * i_dst + offset], V08[k]);
		_mm_storeu_si128((__m128i*)&dst[9 * i_dst + offset], V09[k]);

		_mm_storeu_si128((__m128i*)&dst[10 * i_dst + offset], V10[k]);
		_mm_storeu_si128((__m128i*)&dst[11 * i_dst + offset], V11[k]);
		_mm_storeu_si128((__m128i*)&dst[12 * i_dst + offset], V12[k]);
		_mm_storeu_si128((__m128i*)&dst[13 * i_dst + offset], V13[k]);
		_mm_storeu_si128((__m128i*)&dst[14 * i_dst + offset], V14[k]);
		_mm_storeu_si128((__m128i*)&dst[15 * i_dst + offset], V15[k]);
		_mm_storeu_si128((__m128i*)&dst[16 * i_dst + offset], V16[k]);
		_mm_storeu_si128((__m128i*)&dst[17 * i_dst + offset], V17[k]);
		_mm_storeu_si128((__m128i*)&dst[18 * i_dst + offset], V18[k]);
		_mm_storeu_si128((__m128i*)&dst[19 * i_dst + offset], V19[k]);

		_mm_storeu_si128((__m128i*)&dst[20 * i_dst + offset], V20[k]);
		_mm_storeu_si128((__m128i*)&dst[21 * i_dst + offset], V21[k]);
		_mm_storeu_si128((__m128i*)&dst[22 * i_dst + offset], V22[k]);
		_mm_storeu_si128((__m128i*)&dst[23 * i_dst + offset], V23[k]);
		_mm_storeu_si128((__m128i*)&dst[24 * i_dst + offset], V24[k]);
		_mm_storeu_si128((__m128i*)&dst[25 * i_dst + offset], V25[k]);
		_mm_storeu_si128((__m128i*)&dst[26 * i_dst + offset], V26[k]);
		_mm_storeu_si128((__m128i*)&dst[27 * i_dst + offset], V27[k]);
		_mm_storeu_si128((__m128i*)&dst[28 * i_dst + offset], V28[k]);
		_mm_storeu_si128((__m128i*)&dst[29 * i_dst + offset], V29[k]);

		_mm_storeu_si128((__m128i*)&dst[30 * i_dst + offset], V30[k]);
		_mm_storeu_si128((__m128i*)&dst[31 * i_dst + offset], V31[k]);
		_mm_storeu_si128((__m128i*)&dst[32 * i_dst + offset], V32[k]);
		_mm_storeu_si128((__m128i*)&dst[33 * i_dst + offset], V33[k]);
		_mm_storeu_si128((__m128i*)&dst[34 * i_dst + offset], V34[k]);
		_mm_storeu_si128((__m128i*)&dst[35 * i_dst + offset], V35[k]);
		_mm_storeu_si128((__m128i*)&dst[36 * i_dst + offset], V36[k]);
		_mm_storeu_si128((__m128i*)&dst[37 * i_dst + offset], V37[k]);
		_mm_storeu_si128((__m128i*)&dst[38 * i_dst + offset], V38[k]);
		_mm_storeu_si128((__m128i*)&dst[39 * i_dst + offset], V39[k]);

		_mm_storeu_si128((__m128i*)&dst[40 * i_dst + offset], V40[k]);
		_mm_storeu_si128((__m128i*)&dst[41 * i_dst + offset], V41[k]);
		_mm_storeu_si128((__m128i*)&dst[42 * i_dst + offset], V42[k]);
		_mm_storeu_si128((__m128i*)&dst[43 * i_dst + offset], V43[k]);
		_mm_storeu_si128((__m128i*)&dst[44 * i_dst + offset], V44[k]);
		_mm_storeu_si128((__m128i*)&dst[45 * i_dst + offset], V45[k]);
		_mm_storeu_si128((__m128i*)&dst[46 * i_dst + offset], V46[k]);
		_mm_storeu_si128((__m128i*)&dst[47 * i_dst + offset], V47[k]);
		_mm_storeu_si128((__m128i*)&dst[48 * i_dst + offset], V48[k]);
		_mm_storeu_si128((__m128i*)&dst[49 * i_dst + offset], V49[k]);

		_mm_storeu_si128((__m128i*)&dst[50 * i_dst + offset], V50[k]);
		_mm_storeu_si128((__m128i*)&dst[51 * i_dst + offset], V51[k]);
		_mm_storeu_si128((__m128i*)&dst[52 * i_dst + offset], V52[k]);
		_mm_storeu_si128((__m128i*)&dst[53 * i_dst + offset], V53[k]);
		_mm_storeu_si128((__m128i*)&dst[54 * i_dst + offset], V54[k]);
		_mm_storeu_si128((__m128i*)&dst[55 * i_dst + offset], V55[k]);
		_mm_storeu_si128((__m128i*)&dst[56 * i_dst + offset], V56[k]);
		_mm_storeu_si128((__m128i*)&dst[57 * i_dst + offset], V57[k]);
		_mm_storeu_si128((__m128i*)&dst[58 * i_dst + offset], V58[k]);
		_mm_storeu_si128((__m128i*)&dst[59 * i_dst + offset], V59[k]);

		_mm_storeu_si128((__m128i*)&dst[60 * i_dst + offset], V60[k]);
		_mm_storeu_si128((__m128i*)&dst[61 * i_dst + offset], V61[k]);
		_mm_storeu_si128((__m128i*)&dst[62 * i_dst + offset], V62[k]);
		_mm_storeu_si128((__m128i*)&dst[63 * i_dst + offset], V63[k]);

	}
}

void add_inv_trans_ext_32x32_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    int k;
    __m128i P00[2], P01[2], P02[2], P03[2], P04[2], P05[2], P06[2], P07[2], P08[2], P09[2], P10[2], P11[2], P12[2], P13[2], P14[2], P15[2], P16[2], P17[2], P18[2], P19[2], P20[2], P21[2], P22[2], P23[2], P24[2], P25[2], P26[2], P27[2], P28[2], P29[2], P30[2], P31[2];

	const __m128i c16_p45_p45 = _mm_set1_epi32(0x002D002D);
	const __m128i c16_p43_p44 = _mm_set1_epi32(0x002B002C);
	const __m128i c16_p39_p41 = _mm_set1_epi32(0x00270029);
	const __m128i c16_p34_p36 = _mm_set1_epi32(0x00220024);
	const __m128i c16_p27_p30 = _mm_set1_epi32(0x001B001E);
	const __m128i c16_p19_p23 = _mm_set1_epi32(0x00130017);
	const __m128i c16_p11_p15 = _mm_set1_epi32(0x000B000F);
	const __m128i c16_p02_p07 = _mm_set1_epi32(0x00020007);
	const __m128i c16_p41_p45 = _mm_set1_epi32(0x0029002D);
	const __m128i c16_p23_p34 = _mm_set1_epi32(0x00170022);
	const __m128i c16_n02_p11 = _mm_set1_epi32(0xFFFE000B);
	const __m128i c16_n27_n15 = _mm_set1_epi32(0xFFE5FFF1);
	const __m128i c16_n43_n36 = _mm_set1_epi32(0xFFD5FFDC);
	const __m128i c16_n44_n45 = _mm_set1_epi32(0xFFD4FFD3);
	const __m128i c16_n30_n39 = _mm_set1_epi32(0xFFE2FFD9);
	const __m128i c16_n07_n19 = _mm_set1_epi32(0xFFF9FFED);
	const __m128i c16_p34_p44 = _mm_set1_epi32(0x0022002C);
	const __m128i c16_n07_p15 = _mm_set1_epi32(0xFFF9000F);
	const __m128i c16_n41_n27 = _mm_set1_epi32(0xFFD7FFE5);
	const __m128i c16_n39_n45 = _mm_set1_epi32(0xFFD9FFD3);
	const __m128i c16_n02_n23 = _mm_set1_epi32(0xFFFEFFE9);
	const __m128i c16_p36_p19 = _mm_set1_epi32(0x00240013);
	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
	const __m128i c16_p11_p30 = _mm_set1_epi32(0x000B001E);
	const __m128i c16_p23_p43 = _mm_set1_epi32(0x0017002B);
	const __m128i c16_n34_n07 = _mm_set1_epi32(0xFFDEFFF9);
	const __m128i c16_n36_n45 = _mm_set1_epi32(0xFFDCFFD3);
	const __m128i c16_p19_n11 = _mm_set1_epi32(0x0013FFF5);
	const __m128i c16_p44_p41 = _mm_set1_epi32(0x002C0029);
	const __m128i c16_n02_p27 = _mm_set1_epi32(0xFFFE001B);
	const __m128i c16_n45_n30 = _mm_set1_epi32(0xFFD3FFE2);
	const __m128i c16_n15_n39 = _mm_set1_epi32(0xFFF1FFD9);
	const __m128i c16_p11_p41 = _mm_set1_epi32(0x000B0029);
	const __m128i c16_n45_n27 = _mm_set1_epi32(0xFFD3FFE5);
	const __m128i c16_p07_n30 = _mm_set1_epi32(0x0007FFE2);
	const __m128i c16_p43_p39 = _mm_set1_epi32(0x002B0027);
	const __m128i c16_n23_p15 = _mm_set1_epi32(0xFFE9000F);
	const __m128i c16_n34_n45 = _mm_set1_epi32(0xFFDEFFD3);
	const __m128i c16_p36_p02 = _mm_set1_epi32(0x00240002);
	const __m128i c16_p19_p44 = _mm_set1_epi32(0x0013002C);
	const __m128i c16_n02_p39 = _mm_set1_epi32(0xFFFE0027);
	const __m128i c16_n36_n41 = _mm_set1_epi32(0xFFDCFFD7);
	const __m128i c16_p43_p07 = _mm_set1_epi32(0x002B0007);
	const __m128i c16_n11_p34 = _mm_set1_epi32(0xFFF50022);
	const __m128i c16_n30_n44 = _mm_set1_epi32(0xFFE2FFD4);
	const __m128i c16_p45_p15 = _mm_set1_epi32(0x002D000F);
	const __m128i c16_n19_p27 = _mm_set1_epi32(0xFFED001B);
	const __m128i c16_n23_n45 = _mm_set1_epi32(0xFFE9FFD3);
	const __m128i c16_n15_p36 = _mm_set1_epi32(0xFFF10024);
	const __m128i c16_n11_n45 = _mm_set1_epi32(0xFFF5FFD3);
	const __m128i c16_p34_p39 = _mm_set1_epi32(0x00220027);
	const __m128i c16_n45_n19 = _mm_set1_epi32(0xFFD3FFED);
	const __m128i c16_p41_n07 = _mm_set1_epi32(0x0029FFF9);
	const __m128i c16_n23_p30 = _mm_set1_epi32(0xFFE9001E);
	const __m128i c16_n02_n44 = _mm_set1_epi32(0xFFFEFFD4);
	const __m128i c16_p27_p43 = _mm_set1_epi32(0x001B002B);
	const __m128i c16_n27_p34 = _mm_set1_epi32(0xFFE50022);
	const __m128i c16_p19_n39 = _mm_set1_epi32(0x0013FFD9);
	const __m128i c16_n11_p43 = _mm_set1_epi32(0xFFF5002B);
	const __m128i c16_p02_n45 = _mm_set1_epi32(0x0002FFD3);
	const __m128i c16_p07_p45 = _mm_set1_epi32(0x0007002D);
	const __m128i c16_n15_n44 = _mm_set1_epi32(0xFFF1FFD4);
	const __m128i c16_p23_p41 = _mm_set1_epi32(0x00170029);
	const __m128i c16_n30_n36 = _mm_set1_epi32(0xFFE2FFDC);
	const __m128i c16_n36_p30 = _mm_set1_epi32(0xFFDC001E);
	const __m128i c16_p41_n23 = _mm_set1_epi32(0x0029FFE9);
	const __m128i c16_n44_p15 = _mm_set1_epi32(0xFFD4000F);
	const __m128i c16_p45_n07 = _mm_set1_epi32(0x002DFFF9);
	const __m128i c16_n45_n02 = _mm_set1_epi32(0xFFD3FFFE);
	const __m128i c16_p43_p11 = _mm_set1_epi32(0x002B000B);
	const __m128i c16_n39_n19 = _mm_set1_epi32(0xFFD9FFED);
	const __m128i c16_p34_p27 = _mm_set1_epi32(0x0022001B);
	const __m128i c16_n43_p27 = _mm_set1_epi32(0xFFD5001B);
	const __m128i c16_p44_n02 = _mm_set1_epi32(0x002CFFFE);
	const __m128i c16_n30_n23 = _mm_set1_epi32(0xFFE2FFE9);
	const __m128i c16_p07_p41 = _mm_set1_epi32(0x00070029);
	const __m128i c16_p19_n45 = _mm_set1_epi32(0x0013FFD3);
	const __m128i c16_n39_p34 = _mm_set1_epi32(0xFFD90022);
	const __m128i c16_p45_n11 = _mm_set1_epi32(0x002DFFF5);
	const __m128i c16_n36_n15 = _mm_set1_epi32(0xFFDCFFF1);
	const __m128i c16_n45_p23 = _mm_set1_epi32(0xFFD30017);
	const __m128i c16_p27_p19 = _mm_set1_epi32(0x001B0013);
	const __m128i c16_p15_n45 = _mm_set1_epi32(0x000FFFD3);
	const __m128i c16_n44_p30 = _mm_set1_epi32(0xFFD4001E);
	const __m128i c16_p34_p11 = _mm_set1_epi32(0x0022000B);
	const __m128i c16_p07_n43 = _mm_set1_epi32(0x0007FFD5);
	const __m128i c16_n41_p36 = _mm_set1_epi32(0xFFD70024);
	const __m128i c16_p39_p02 = _mm_set1_epi32(0x00270002);
	const __m128i c16_n44_p19 = _mm_set1_epi32(0xFFD40013);
	const __m128i c16_n02_p36 = _mm_set1_epi32(0xFFFE0024);
	const __m128i c16_p45_n34 = _mm_set1_epi32(0x002DFFDE);
	const __m128i c16_n15_n23 = _mm_set1_epi32(0xFFF1FFE9);
	const __m128i c16_n39_p43 = _mm_set1_epi32(0xFFD9002B);
	const __m128i c16_p30_p07 = _mm_set1_epi32(0x001E0007);
	const __m128i c16_p27_n45 = _mm_set1_epi32(0x001BFFD3);
	const __m128i c16_n41_p11 = _mm_set1_epi32(0xFFD7000B);
	const __m128i c16_n39_p15 = _mm_set1_epi32(0xFFD9000F);
	const __m128i c16_n30_p45 = _mm_set1_epi32(0xFFE2002D);
	const __m128i c16_p27_p02 = _mm_set1_epi32(0x001B0002);
	const __m128i c16_p41_n44 = _mm_set1_epi32(0x0029FFD4);
	const __m128i c16_n11_n19 = _mm_set1_epi32(0xFFF5FFED);
	const __m128i c16_n45_p36 = _mm_set1_epi32(0xFFD30024);
	const __m128i c16_n07_p34 = _mm_set1_epi32(0xFFF90022);
	const __m128i c16_p43_n23 = _mm_set1_epi32(0x002BFFE9);
	const __m128i c16_n30_p11 = _mm_set1_epi32(0xFFE2000B);
	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);
	const __m128i c16_n19_p36 = _mm_set1_epi32(0xFFED0024);
	const __m128i c16_p23_n02 = _mm_set1_epi32(0x0017FFFE);
	const __m128i c16_p45_n39 = _mm_set1_epi32(0x002DFFD9);
	const __m128i c16_p27_n41 = _mm_set1_epi32(0x001BFFD7);
	const __m128i c16_n15_n07 = _mm_set1_epi32(0xFFF1FFF9);
	const __m128i c16_n44_p34 = _mm_set1_epi32(0xFFD40022);
	const __m128i c16_n19_p07 = _mm_set1_epi32(0xFFED0007);
	const __m128i c16_n39_p30 = _mm_set1_epi32(0xFFD9001E);
	const __m128i c16_n45_p44 = _mm_set1_epi32(0xFFD3002C);
	const __m128i c16_n36_p43 = _mm_set1_epi32(0xFFDC002B);
	const __m128i c16_n15_p27 = _mm_set1_epi32(0xFFF1001B);
	const __m128i c16_p11_p02 = _mm_set1_epi32(0x000B0002);
	const __m128i c16_p34_n23 = _mm_set1_epi32(0x0022FFE9);
	const __m128i c16_p45_n41 = _mm_set1_epi32(0x002DFFD7);
	const __m128i c16_n07_p02 = _mm_set1_epi32(0xFFF90002);
	const __m128i c16_n15_p11 = _mm_set1_epi32(0xFFF1000B);
	const __m128i c16_n23_p19 = _mm_set1_epi32(0xFFE90013);
	const __m128i c16_n30_p27 = _mm_set1_epi32(0xFFE2001B);
	const __m128i c16_n36_p34 = _mm_set1_epi32(0xFFDC0022);
	const __m128i c16_n41_p39 = _mm_set1_epi32(0xFFD70027);
	const __m128i c16_n44_p43 = _mm_set1_epi32(0xFFD4002B);
	const __m128i c16_n45_p45 = _mm_set1_epi32(0xFFD3002D);

	//	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
	const __m128i c16_p35_p40 = _mm_set1_epi32(0x00230028);
	const __m128i c16_p21_p29 = _mm_set1_epi32(0x0015001D);
	const __m128i c16_p04_p13 = _mm_set1_epi32(0x0004000D);
	const __m128i c16_p29_p43 = _mm_set1_epi32(0x001D002B);
	const __m128i c16_n21_p04 = _mm_set1_epi32(0xFFEB0004);
	const __m128i c16_n45_n40 = _mm_set1_epi32(0xFFD3FFD8);
	const __m128i c16_n13_n35 = _mm_set1_epi32(0xFFF3FFDD);
	const __m128i c16_p04_p40 = _mm_set1_epi32(0x00040028);
	const __m128i c16_n43_n35 = _mm_set1_epi32(0xFFD5FFDD);
	const __m128i c16_p29_n13 = _mm_set1_epi32(0x001DFFF3);
	const __m128i c16_p21_p45 = _mm_set1_epi32(0x0015002D);
	const __m128i c16_n21_p35 = _mm_set1_epi32(0xFFEB0023);
	const __m128i c16_p04_n43 = _mm_set1_epi32(0x0004FFD5);
	const __m128i c16_p13_p45 = _mm_set1_epi32(0x000D002D);
	const __m128i c16_n29_n40 = _mm_set1_epi32(0xFFE3FFD8);
	const __m128i c16_n40_p29 = _mm_set1_epi32(0xFFD8001D);
	const __m128i c16_p45_n13 = _mm_set1_epi32(0x002DFFF3);
	const __m128i c16_n43_n04 = _mm_set1_epi32(0xFFD5FFFC);
	const __m128i c16_p35_p21 = _mm_set1_epi32(0x00230015);
	const __m128i c16_n45_p21 = _mm_set1_epi32(0xFFD30015);
	const __m128i c16_p13_p29 = _mm_set1_epi32(0x000D001D);
	const __m128i c16_p35_n43 = _mm_set1_epi32(0x0023FFD5);
	const __m128i c16_n40_p04 = _mm_set1_epi32(0xFFD80004);
	const __m128i c16_n35_p13 = _mm_set1_epi32(0xFFDD000D);
	const __m128i c16_n40_p45 = _mm_set1_epi32(0xFFD8002D);
	const __m128i c16_p04_p21 = _mm_set1_epi32(0x00040015);
	const __m128i c16_p43_n29 = _mm_set1_epi32(0x002BFFE3);
	const __m128i c16_n13_p04 = _mm_set1_epi32(0xFFF30004);
	const __m128i c16_n29_p21 = _mm_set1_epi32(0xFFE30015);
	const __m128i c16_n40_p35 = _mm_set1_epi32(0xFFD80023);
	//	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);


	const __m128i c16_p38_p44 = _mm_set1_epi32(0x0026002C);
	const __m128i c16_p09_p25 = _mm_set1_epi32(0x00090019);
	const __m128i c16_n09_p38 = _mm_set1_epi32(0xFFF70026);
	const __m128i c16_n25_n44 = _mm_set1_epi32(0xFFE7FFD4);

	const __m128i c16_n44_p25 = _mm_set1_epi32(0xFFD40019);
	const __m128i c16_p38_p09 = _mm_set1_epi32(0x00260009);
	const __m128i c16_n25_p09 = _mm_set1_epi32(0xFFE70009);
	const __m128i c16_n44_p38 = _mm_set1_epi32(0xFFD40026);

	const __m128i c16_p17_p42 = _mm_set1_epi32(0x0011002A);
	const __m128i c16_n42_p17 = _mm_set1_epi32(0xFFD60011);

	const __m128i c16_p32_p32 = _mm_set1_epi32(0x00200020);
	const __m128i c16_n32_p32 = _mm_set1_epi32(0xFFE00020);

	__m128i zero = _mm_setzero_si128();
	__m128i c32_rnd = _mm_set1_epi32(16);

	int nShift = 5;
	int i, part;

	// DCT1
	__m128i in00[4], in01[4], in02[4], in03[4], in04[4], in05[4], in06[4], in07[4], in08[4], in09[4], in10[4], in11[4], in12[4], in13[4], in14[4], in15[4];
	__m128i in16[4], in17[4], in18[4], in19[4], in20[4], in21[4], in22[4], in23[4], in24[4], in25[4], in26[4], in27[4], in28[4], in29[4], in30[4], in31[4];
	__m128i res00[4], res01[4], res02[4], res03[4], res04[4], res05[4], res06[4], res07[4], res08[4], res09[4], res10[4], res11[4], res12[4], res13[4], res14[4], res15[4];
	__m128i res16[4], res17[4], res18[4], res19[4], res20[4], res21[4], res22[4], res23[4], res24[4], res25[4], res26[4], res27[4], res28[4], res29[4], res30[4], res31[4];

	for (i = 0; i < 2; i++)
	{
		const int offset = (i << 3);

		in00[i] = _mm_loadu_si128((const __m128i*)&src[0 * 32 + offset]);
		in01[i] = _mm_loadu_si128((const __m128i*)&src[1 * 32 + offset]);
		in02[i] = _mm_loadu_si128((const __m128i*)&src[2 * 32 + offset]);
		in03[i] = _mm_loadu_si128((const __m128i*)&src[3 * 32 + offset]);
		in04[i] = _mm_loadu_si128((const __m128i*)&src[4 * 32 + offset]);
		in05[i] = _mm_loadu_si128((const __m128i*)&src[5 * 32 + offset]);
		in06[i] = _mm_loadu_si128((const __m128i*)&src[6 * 32 + offset]);
		in07[i] = _mm_loadu_si128((const __m128i*)&src[7 * 32 + offset]);
		in08[i] = _mm_loadu_si128((const __m128i*)&src[8 * 32 + offset]);
		in09[i] = _mm_loadu_si128((const __m128i*)&src[9 * 32 + offset]);
		in10[i] = _mm_loadu_si128((const __m128i*)&src[10 * 32 + offset]);
		in11[i] = _mm_loadu_si128((const __m128i*)&src[11 * 32 + offset]);
		in12[i] = _mm_loadu_si128((const __m128i*)&src[12 * 32 + offset]);
		in13[i] = _mm_loadu_si128((const __m128i*)&src[13 * 32 + offset]);
		in14[i] = _mm_loadu_si128((const __m128i*)&src[14 * 32 + offset]);
		in15[i] = _mm_loadu_si128((const __m128i*)&src[15 * 32 + offset]);

        in16[i] = zero; in17[i] = zero; in18[i] = zero; in19[i] = zero;
        in20[i] = zero; in21[i] = zero; in22[i] = zero; in23[i] = zero;
        in24[i] = zero; in25[i] = zero; in26[i] = zero; in27[i] = zero;
        in28[i] = zero; in29[i] = zero; in30[i] = zero; in31[i] = zero;
	}

    for (part = 0; part < 2; part++)
    {
        const __m128i T_00_00A = _mm_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
        const __m128i T_00_00B = _mm_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
        const __m128i T_00_01A = _mm_unpacklo_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_01B = _mm_unpackhi_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_02A = _mm_unpacklo_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_02B = _mm_unpackhi_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_03A = _mm_unpacklo_epi16(in13[part], in15[part]);       // [ ]
        const __m128i T_00_03B = _mm_unpackhi_epi16(in13[part], in15[part]);       // [ ]

        const __m128i T_00_08A = _mm_unpacklo_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_08B = _mm_unpackhi_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_09A = _mm_unpacklo_epi16(in10[part], in14[part]);       // [ ]
        const __m128i T_00_09B = _mm_unpackhi_epi16(in10[part], in14[part]);       // [ ]

        const __m128i T_00_12A = _mm_unpacklo_epi16(in04[part], in12[part]);       // [ ]
        const __m128i T_00_12B = _mm_unpackhi_epi16(in04[part], in12[part]);       // [ ]

        const __m128i T_00_14A = _mm_unpacklo_epi16(in08[part], zero);       //
        const __m128i T_00_14B = _mm_unpackhi_epi16(in08[part], zero);       // [ ]
        const __m128i T_00_15A = _mm_unpacklo_epi16(in00[part], zero);       //
        const __m128i T_00_15B = _mm_unpackhi_epi16(in00[part], zero);       // [ ]

        __m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
        __m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
        __m128i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
        __m128i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
        {
            __m128i T00, T01, T02, T03;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, x1, x2, x3, x4, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
    T00 = _mm_add_epi32(_mm_madd_epi16(r0103, c0103), _mm_madd_epi16(r0507, c0507)); \
    T01 = _mm_add_epi32(_mm_madd_epi16(r0911, c0911), _mm_madd_epi16(r1315, c1315)); \
    T02 = _mm_add_epi32(_mm_madd_epi16(zero, c1719), _mm_madd_epi16(zero, c2123)); \
    T03 = _mm_add_epi32(_mm_madd_epi16(zero, c2527), _mm_madd_epi16(zero, c2931)); \
    row = _mm_add_epi32(_mm_add_epi32(T00, T01), _mm_add_epi32(T02, T03));

            COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A)

                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B)

#undef COMPUTE_ROW
        }


        {
            __m128i T00, T01;
#define COMPUTE_ROW(row0206, row1014, x1, x2, c0206, c1014, c1822, c2630, row) \
    T00 = _mm_add_epi32(_mm_madd_epi16(row0206, c0206), _mm_madd_epi16(row1014, c1014)); \
    T01 = _mm_add_epi32(_mm_madd_epi16(zero, c1822), _mm_madd_epi16(zero, c2630)); \
    row = _mm_add_epi32(T00, T01);

            COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A)

                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B)
#undef COMPUTE_ROW
        }
        {
            const __m128i EEO0A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));
            const __m128i EEO0B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));

            const __m128i EEEO0A = _mm_madd_epi16(T_00_14A, c16_p17_p42);
            const __m128i EEEO0B = _mm_madd_epi16(T_00_14B, c16_p17_p42);
            const __m128i EEEO1A = _mm_madd_epi16(T_00_14A, c16_n42_p17);
            const __m128i EEEO1B = _mm_madd_epi16(T_00_14B, c16_n42_p17);

            const __m128i EEEE0A = _mm_madd_epi16(T_00_15A, c16_p32_p32);
            const __m128i EEEE0B = _mm_madd_epi16(T_00_15B, c16_p32_p32);
            const __m128i EEEE1A = _mm_madd_epi16(T_00_15A, c16_n32_p32);
            const __m128i EEEE1B = _mm_madd_epi16(T_00_15B, c16_n32_p32);

            const __m128i EEE0A = _mm_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
            const __m128i EEE0B = _mm_add_epi32(EEEE0B, EEEO0B);
            const __m128i EEE1A = _mm_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
            const __m128i EEE1B = _mm_add_epi32(EEEE1B, EEEO1B);
            const __m128i EEE3A = _mm_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
            const __m128i EEE3B = _mm_sub_epi32(EEEE0B, EEEO0B);
            const __m128i EEE2A = _mm_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
            const __m128i EEE2B = _mm_sub_epi32(EEEE1B, EEEO1B);

            const __m128i EE0A = _mm_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
            const __m128i EE0B = _mm_add_epi32(EEE0B, EEO0B);
            const __m128i EE1A = _mm_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
            const __m128i EE1B = _mm_add_epi32(EEE1B, EEO1B);
            const __m128i EE2A = _mm_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
            const __m128i EE2B = _mm_add_epi32(EEE2B, EEO2B);
            const __m128i EE3A = _mm_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
            const __m128i EE3B = _mm_add_epi32(EEE3B, EEO3B);
            const __m128i EE7A = _mm_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
            const __m128i EE7B = _mm_sub_epi32(EEE0B, EEO0B);
            const __m128i EE6A = _mm_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
            const __m128i EE6B = _mm_sub_epi32(EEE1B, EEO1B);
            const __m128i EE5A = _mm_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
            const __m128i EE5B = _mm_sub_epi32(EEE2B, EEO2B);
            const __m128i EE4A = _mm_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
            const __m128i EE4B = _mm_sub_epi32(EEE3B, EEO3B);

            const __m128i E0A = _mm_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
            const __m128i E0B = _mm_add_epi32(EE0B, EO0B);
            const __m128i E1A = _mm_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
            const __m128i E1B = _mm_add_epi32(EE1B, EO1B);
            const __m128i E2A = _mm_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
            const __m128i E2B = _mm_add_epi32(EE2B, EO2B);
            const __m128i E3A = _mm_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
            const __m128i E3B = _mm_add_epi32(EE3B, EO3B);
            const __m128i E4A = _mm_add_epi32(EE4A, EO4A);          // E4 =
            const __m128i E4B = _mm_add_epi32(EE4B, EO4B);
            const __m128i E5A = _mm_add_epi32(EE5A, EO5A);          // E5 =
            const __m128i E5B = _mm_add_epi32(EE5B, EO5B);
            const __m128i E6A = _mm_add_epi32(EE6A, EO6A);          // E6 =
            const __m128i E6B = _mm_add_epi32(EE6B, EO6B);
            const __m128i E7A = _mm_add_epi32(EE7A, EO7A);          // E7 =
            const __m128i E7B = _mm_add_epi32(EE7B, EO7B);
            const __m128i EFA = _mm_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
            const __m128i EFB = _mm_sub_epi32(EE0B, EO0B);
            const __m128i EEA = _mm_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
            const __m128i EEB = _mm_sub_epi32(EE1B, EO1B);
            const __m128i EDA = _mm_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
            const __m128i EDB = _mm_sub_epi32(EE2B, EO2B);
            const __m128i ECA = _mm_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
            const __m128i ECB = _mm_sub_epi32(EE3B, EO3B);
            const __m128i EBA = _mm_sub_epi32(EE4A, EO4A);          // EB =
            const __m128i EBB = _mm_sub_epi32(EE4B, EO4B);
            const __m128i EAA = _mm_sub_epi32(EE5A, EO5A);          // EA =
            const __m128i EAB = _mm_sub_epi32(EE5B, EO5B);
            const __m128i E9A = _mm_sub_epi32(EE6A, EO6A);          // E9 =
            const __m128i E9B = _mm_sub_epi32(EE6B, EO6B);
            const __m128i E8A = _mm_sub_epi32(EE7A, EO7A);          // E8 =
            const __m128i E8B = _mm_sub_epi32(EE7B, EO7B);

            const __m128i T10A = _mm_add_epi32(E0A, c32_rnd);         // E0 + rnd
            const __m128i T10B = _mm_add_epi32(E0B, c32_rnd);
            const __m128i T11A = _mm_add_epi32(E1A, c32_rnd);         // E1 + rnd
            const __m128i T11B = _mm_add_epi32(E1B, c32_rnd);
            const __m128i T12A = _mm_add_epi32(E2A, c32_rnd);         // E2 + rnd
            const __m128i T12B = _mm_add_epi32(E2B, c32_rnd);
            const __m128i T13A = _mm_add_epi32(E3A, c32_rnd);         // E3 + rnd
            const __m128i T13B = _mm_add_epi32(E3B, c32_rnd);
            const __m128i T14A = _mm_add_epi32(E4A, c32_rnd);         // E4 + rnd
            const __m128i T14B = _mm_add_epi32(E4B, c32_rnd);
            const __m128i T15A = _mm_add_epi32(E5A, c32_rnd);         // E5 + rnd
            const __m128i T15B = _mm_add_epi32(E5B, c32_rnd);
            const __m128i T16A = _mm_add_epi32(E6A, c32_rnd);         // E6 + rnd
            const __m128i T16B = _mm_add_epi32(E6B, c32_rnd);
            const __m128i T17A = _mm_add_epi32(E7A, c32_rnd);         // E7 + rnd
            const __m128i T17B = _mm_add_epi32(E7B, c32_rnd);
            const __m128i T18A = _mm_add_epi32(E8A, c32_rnd);         // E8 + rnd
            const __m128i T18B = _mm_add_epi32(E8B, c32_rnd);
            const __m128i T19A = _mm_add_epi32(E9A, c32_rnd);         // E9 + rnd
            const __m128i T19B = _mm_add_epi32(E9B, c32_rnd);
            const __m128i T1AA = _mm_add_epi32(EAA, c32_rnd);         // E10 + rnd
            const __m128i T1AB = _mm_add_epi32(EAB, c32_rnd);
            const __m128i T1BA = _mm_add_epi32(EBA, c32_rnd);         // E11 + rnd
            const __m128i T1BB = _mm_add_epi32(EBB, c32_rnd);
            const __m128i T1CA = _mm_add_epi32(ECA, c32_rnd);         // E12 + rnd
            const __m128i T1CB = _mm_add_epi32(ECB, c32_rnd);
            const __m128i T1DA = _mm_add_epi32(EDA, c32_rnd);         // E13 + rnd
            const __m128i T1DB = _mm_add_epi32(EDB, c32_rnd);
            const __m128i T1EA = _mm_add_epi32(EEA, c32_rnd);         // E14 + rnd
            const __m128i T1EB = _mm_add_epi32(EEB, c32_rnd);
            const __m128i T1FA = _mm_add_epi32(EFA, c32_rnd);         // E15 + rnd
            const __m128i T1FB = _mm_add_epi32(EFB, c32_rnd);

            const __m128i T2_00A = _mm_add_epi32(T10A, O00A);          // E0 + O0 + rnd
            const __m128i T2_00B = _mm_add_epi32(T10B, O00B);
            const __m128i T2_01A = _mm_add_epi32(T11A, O01A);          // E1 + O1 + rnd
            const __m128i T2_01B = _mm_add_epi32(T11B, O01B);
            const __m128i T2_02A = _mm_add_epi32(T12A, O02A);          // E2 + O2 + rnd
            const __m128i T2_02B = _mm_add_epi32(T12B, O02B);
            const __m128i T2_03A = _mm_add_epi32(T13A, O03A);          // E3 + O3 + rnd
            const __m128i T2_03B = _mm_add_epi32(T13B, O03B);
            const __m128i T2_04A = _mm_add_epi32(T14A, O04A);          // E4
            const __m128i T2_04B = _mm_add_epi32(T14B, O04B);
            const __m128i T2_05A = _mm_add_epi32(T15A, O05A);          // E5
            const __m128i T2_05B = _mm_add_epi32(T15B, O05B);
            const __m128i T2_06A = _mm_add_epi32(T16A, O06A);          // E6
            const __m128i T2_06B = _mm_add_epi32(T16B, O06B);
            const __m128i T2_07A = _mm_add_epi32(T17A, O07A);          // E7
            const __m128i T2_07B = _mm_add_epi32(T17B, O07B);
            const __m128i T2_08A = _mm_add_epi32(T18A, O08A);          // E8
            const __m128i T2_08B = _mm_add_epi32(T18B, O08B);
            const __m128i T2_09A = _mm_add_epi32(T19A, O09A);          // E9
            const __m128i T2_09B = _mm_add_epi32(T19B, O09B);
            const __m128i T2_10A = _mm_add_epi32(T1AA, O10A);          // E10
            const __m128i T2_10B = _mm_add_epi32(T1AB, O10B);
            const __m128i T2_11A = _mm_add_epi32(T1BA, O11A);          // E11
            const __m128i T2_11B = _mm_add_epi32(T1BB, O11B);
            const __m128i T2_12A = _mm_add_epi32(T1CA, O12A);          // E12
            const __m128i T2_12B = _mm_add_epi32(T1CB, O12B);
            const __m128i T2_13A = _mm_add_epi32(T1DA, O13A);          // E13
            const __m128i T2_13B = _mm_add_epi32(T1DB, O13B);
            const __m128i T2_14A = _mm_add_epi32(T1EA, O14A);          // E14
            const __m128i T2_14B = _mm_add_epi32(T1EB, O14B);
            const __m128i T2_15A = _mm_add_epi32(T1FA, O15A);          // E15
            const __m128i T2_15B = _mm_add_epi32(T1FB, O15B);
            const __m128i T2_31A = _mm_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
            const __m128i T2_31B = _mm_sub_epi32(T10B, O00B);
            const __m128i T2_30A = _mm_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
            const __m128i T2_30B = _mm_sub_epi32(T11B, O01B);
            const __m128i T2_29A = _mm_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
            const __m128i T2_29B = _mm_sub_epi32(T12B, O02B);
            const __m128i T2_28A = _mm_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
            const __m128i T2_28B = _mm_sub_epi32(T13B, O03B);
            const __m128i T2_27A = _mm_sub_epi32(T14A, O04A);          // E4
            const __m128i T2_27B = _mm_sub_epi32(T14B, O04B);
            const __m128i T2_26A = _mm_sub_epi32(T15A, O05A);          // E5
            const __m128i T2_26B = _mm_sub_epi32(T15B, O05B);
            const __m128i T2_25A = _mm_sub_epi32(T16A, O06A);          // E6
            const __m128i T2_25B = _mm_sub_epi32(T16B, O06B);
            const __m128i T2_24A = _mm_sub_epi32(T17A, O07A);          // E7
            const __m128i T2_24B = _mm_sub_epi32(T17B, O07B);
            const __m128i T2_23A = _mm_sub_epi32(T18A, O08A);          //
            const __m128i T2_23B = _mm_sub_epi32(T18B, O08B);
            const __m128i T2_22A = _mm_sub_epi32(T19A, O09A);          //
            const __m128i T2_22B = _mm_sub_epi32(T19B, O09B);
            const __m128i T2_21A = _mm_sub_epi32(T1AA, O10A);          //
            const __m128i T2_21B = _mm_sub_epi32(T1AB, O10B);
            const __m128i T2_20A = _mm_sub_epi32(T1BA, O11A);          //
            const __m128i T2_20B = _mm_sub_epi32(T1BB, O11B);
            const __m128i T2_19A = _mm_sub_epi32(T1CA, O12A);          //
            const __m128i T2_19B = _mm_sub_epi32(T1CB, O12B);
            const __m128i T2_18A = _mm_sub_epi32(T1DA, O13A);          //
            const __m128i T2_18B = _mm_sub_epi32(T1DB, O13B);
            const __m128i T2_17A = _mm_sub_epi32(T1EA, O14A);          //
            const __m128i T2_17B = _mm_sub_epi32(T1EB, O14B);
            const __m128i T2_16A = _mm_sub_epi32(T1FA, O15A);          //
            const __m128i T2_16B = _mm_sub_epi32(T1FB, O15B);

            const __m128i T3_00A = _mm_srai_epi32(T2_00A, nShift);             // [30 20 10 00]
            const __m128i T3_00B = _mm_srai_epi32(T2_00B, nShift);             // [70 60 50 40]
            const __m128i T3_01A = _mm_srai_epi32(T2_01A, nShift);             // [31 21 11 01]
            const __m128i T3_01B = _mm_srai_epi32(T2_01B, nShift);             // [71 61 51 41]
            const __m128i T3_02A = _mm_srai_epi32(T2_02A, nShift);             // [32 22 12 02]
            const __m128i T3_02B = _mm_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
            const __m128i T3_03A = _mm_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
            const __m128i T3_03B = _mm_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
            const __m128i T3_04A = _mm_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
            const __m128i T3_04B = _mm_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
            const __m128i T3_05A = _mm_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
            const __m128i T3_05B = _mm_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
            const __m128i T3_06A = _mm_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
            const __m128i T3_06B = _mm_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
            const __m128i T3_07A = _mm_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
            const __m128i T3_07B = _mm_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
            const __m128i T3_08A = _mm_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
            const __m128i T3_08B = _mm_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
            const __m128i T3_09A = _mm_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
            const __m128i T3_09B = _mm_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
            const __m128i T3_10A = _mm_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
            const __m128i T3_10B = _mm_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
            const __m128i T3_11A = _mm_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
            const __m128i T3_11B = _mm_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
            const __m128i T3_12A = _mm_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
            const __m128i T3_12B = _mm_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
            const __m128i T3_13A = _mm_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
            const __m128i T3_13B = _mm_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
            const __m128i T3_14A = _mm_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
            const __m128i T3_14B = _mm_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
            const __m128i T3_15A = _mm_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
            const __m128i T3_15B = _mm_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

            const __m128i T3_16A = _mm_srai_epi32(T2_16A, nShift);             // [30 20 10 00]
            const __m128i T3_16B = _mm_srai_epi32(T2_16B, nShift);             // [70 60 50 40]
            const __m128i T3_17A = _mm_srai_epi32(T2_17A, nShift);             // [31 21 11 01]
            const __m128i T3_17B = _mm_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
            const __m128i T3_18A = _mm_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
            const __m128i T3_18B = _mm_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
            const __m128i T3_19A = _mm_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
            const __m128i T3_19B = _mm_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
            const __m128i T3_20A = _mm_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
            const __m128i T3_20B = _mm_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
            const __m128i T3_21A = _mm_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
            const __m128i T3_21B = _mm_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
            const __m128i T3_22A = _mm_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
            const __m128i T3_22B = _mm_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
            const __m128i T3_23A = _mm_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
            const __m128i T3_23B = _mm_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
            const __m128i T3_24A = _mm_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
            const __m128i T3_24B = _mm_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
            const __m128i T3_25A = _mm_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
            const __m128i T3_25B = _mm_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
            const __m128i T3_26A = _mm_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
            const __m128i T3_26B = _mm_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
            const __m128i T3_27A = _mm_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
            const __m128i T3_27B = _mm_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
            const __m128i T3_28A = _mm_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
            const __m128i T3_28B = _mm_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
            const __m128i T3_29A = _mm_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
            const __m128i T3_29B = _mm_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
            const __m128i T3_30A = _mm_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
            const __m128i T3_30B = _mm_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
            const __m128i T3_31A = _mm_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
            const __m128i T3_31B = _mm_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

            res00[part] = _mm_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
            res01[part] = _mm_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
            res02[part] = _mm_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
            res03[part] = _mm_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
            res04[part] = _mm_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
            res05[part] = _mm_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
            res06[part] = _mm_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
            res07[part] = _mm_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
            res08[part] = _mm_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
            res09[part] = _mm_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
            res10[part] = _mm_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
            res11[part] = _mm_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
            res12[part] = _mm_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
            res13[part] = _mm_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
            res14[part] = _mm_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
            res15[part] = _mm_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
            res16[part] = _mm_packs_epi32(T3_16A, T3_16B);
            res17[part] = _mm_packs_epi32(T3_17A, T3_17B);
            res18[part] = _mm_packs_epi32(T3_18A, T3_18B);
            res19[part] = _mm_packs_epi32(T3_19A, T3_19B);
            res20[part] = _mm_packs_epi32(T3_20A, T3_20B);
            res21[part] = _mm_packs_epi32(T3_21A, T3_21B);
            res22[part] = _mm_packs_epi32(T3_22A, T3_22B);
            res23[part] = _mm_packs_epi32(T3_23A, T3_23B);
            res24[part] = _mm_packs_epi32(T3_24A, T3_24B);
            res25[part] = _mm_packs_epi32(T3_25A, T3_25B);
            res26[part] = _mm_packs_epi32(T3_26A, T3_26B);
            res27[part] = _mm_packs_epi32(T3_27A, T3_27B);
            res28[part] = _mm_packs_epi32(T3_28A, T3_28B);
            res29[part] = _mm_packs_epi32(T3_29A, T3_29B);
            res30[part] = _mm_packs_epi32(T3_30A, T3_30B);
            res31[part] = _mm_packs_epi32(T3_31A, T3_31B);
        }
    }
    //transpose matrix 8x8 16bit.
    {
        __m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
        __m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
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


        TRANSPOSE_8x8_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0]);
        TRANSPOSE_8x8_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);

        TRANSPOSE_8x8_16BIT(res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1]);
        TRANSPOSE_8x8_16BIT(res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);

        TRANSPOSE_8x8_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2]);
        TRANSPOSE_8x8_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2]);

        TRANSPOSE_8x8_16BIT(res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[3], in01[3], in02[3], in03[3], in04[3], in05[3], in06[3], in07[3]);
        TRANSPOSE_8x8_16BIT(res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in08[3], in09[3], in10[3], in11[3], in12[3], in13[3], in14[3], in15[3]);

#undef TRANSPOSE_8x8_16BIT
    }
	
    c32_rnd = _mm_set1_epi32(2048);				// pass == 1 第二次四舍五入
    nShift = 12;

    for (part = 0; part < 4; part++)
    {
        const __m128i T_00_00A = _mm_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
        const __m128i T_00_00B = _mm_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
        const __m128i T_00_01A = _mm_unpacklo_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_01B = _mm_unpackhi_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_02A = _mm_unpacklo_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_02B = _mm_unpackhi_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_03A = _mm_unpacklo_epi16(in13[part], in15[part]);       // [ ]
        const __m128i T_00_03B = _mm_unpackhi_epi16(in13[part], in15[part]);       // [ ]

        const __m128i T_00_08A = _mm_unpacklo_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_08B = _mm_unpackhi_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_09A = _mm_unpacklo_epi16(in10[part], in14[part]);       // [ ]
        const __m128i T_00_09B = _mm_unpackhi_epi16(in10[part], in14[part]);       // [ ]

        const __m128i T_00_12A = _mm_unpacklo_epi16(in04[part], in12[part]);       // [ ]
        const __m128i T_00_12B = _mm_unpackhi_epi16(in04[part], in12[part]);       // [ ]

        const __m128i T_00_14A = _mm_unpacklo_epi16(in08[part], zero);       //
        const __m128i T_00_14B = _mm_unpackhi_epi16(in08[part], zero);       // [ ]
        const __m128i T_00_15A = _mm_unpacklo_epi16(in00[part], zero);       //
        const __m128i T_00_15B = _mm_unpackhi_epi16(in00[part], zero);       // [ ]

        __m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
        __m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
        __m128i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
        __m128i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
        {
            __m128i T00, T01, T02, T03;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, x1, x2, x3, x4, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
    T00 = _mm_add_epi32(_mm_madd_epi16(r0103, c0103), _mm_madd_epi16(r0507, c0507)); \
    T01 = _mm_add_epi32(_mm_madd_epi16(r0911, c0911), _mm_madd_epi16(r1315, c1315)); \
    T02 = _mm_add_epi32(_mm_madd_epi16(zero, c1719), _mm_madd_epi16(zero, c2123)); \
    T03 = _mm_add_epi32(_mm_madd_epi16(zero, c2527), _mm_madd_epi16(zero, c2931)); \
    row = _mm_add_epi32(_mm_add_epi32(T00, T01), _mm_add_epi32(T02, T03));

            COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A)

                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B)

#undef COMPUTE_ROW
        }


        {
            __m128i T00, T01;
#define COMPUTE_ROW(row0206, row1014, x1, x2, c0206, c1014, c1822, c2630, row) \
    T00 = _mm_add_epi32(_mm_madd_epi16(row0206, c0206), _mm_madd_epi16(row1014, c1014)); \
    T01 = _mm_add_epi32(_mm_madd_epi16(zero, c1822), _mm_madd_epi16(zero, c2630)); \
    row = _mm_add_epi32(T00, T01);

            COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A)

                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B)
#undef COMPUTE_ROW
        }
        {
            const __m128i EEO0A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));
            const __m128i EEO0B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));

            const __m128i EEEO0A = _mm_madd_epi16(T_00_14A, c16_p17_p42);
            const __m128i EEEO0B = _mm_madd_epi16(T_00_14B, c16_p17_p42);
            const __m128i EEEO1A = _mm_madd_epi16(T_00_14A, c16_n42_p17);
            const __m128i EEEO1B = _mm_madd_epi16(T_00_14B, c16_n42_p17);

            const __m128i EEEE0A = _mm_madd_epi16(T_00_15A, c16_p32_p32);
            const __m128i EEEE0B = _mm_madd_epi16(T_00_15B, c16_p32_p32);
            const __m128i EEEE1A = _mm_madd_epi16(T_00_15A, c16_n32_p32);
            const __m128i EEEE1B = _mm_madd_epi16(T_00_15B, c16_n32_p32);

            const __m128i EEE0A = _mm_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
            const __m128i EEE0B = _mm_add_epi32(EEEE0B, EEEO0B);
            const __m128i EEE1A = _mm_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
            const __m128i EEE1B = _mm_add_epi32(EEEE1B, EEEO1B);
            const __m128i EEE3A = _mm_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
            const __m128i EEE3B = _mm_sub_epi32(EEEE0B, EEEO0B);
            const __m128i EEE2A = _mm_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
            const __m128i EEE2B = _mm_sub_epi32(EEEE1B, EEEO1B);

            const __m128i EE0A = _mm_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
            const __m128i EE0B = _mm_add_epi32(EEE0B, EEO0B);
            const __m128i EE1A = _mm_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
            const __m128i EE1B = _mm_add_epi32(EEE1B, EEO1B);
            const __m128i EE2A = _mm_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
            const __m128i EE2B = _mm_add_epi32(EEE2B, EEO2B);
            const __m128i EE3A = _mm_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
            const __m128i EE3B = _mm_add_epi32(EEE3B, EEO3B);
            const __m128i EE7A = _mm_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
            const __m128i EE7B = _mm_sub_epi32(EEE0B, EEO0B);
            const __m128i EE6A = _mm_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
            const __m128i EE6B = _mm_sub_epi32(EEE1B, EEO1B);
            const __m128i EE5A = _mm_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
            const __m128i EE5B = _mm_sub_epi32(EEE2B, EEO2B);
            const __m128i EE4A = _mm_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
            const __m128i EE4B = _mm_sub_epi32(EEE3B, EEO3B);

            const __m128i E0A = _mm_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
            const __m128i E0B = _mm_add_epi32(EE0B, EO0B);
            const __m128i E1A = _mm_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
            const __m128i E1B = _mm_add_epi32(EE1B, EO1B);
            const __m128i E2A = _mm_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
            const __m128i E2B = _mm_add_epi32(EE2B, EO2B);
            const __m128i E3A = _mm_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
            const __m128i E3B = _mm_add_epi32(EE3B, EO3B);
            const __m128i E4A = _mm_add_epi32(EE4A, EO4A);          // E4 =
            const __m128i E4B = _mm_add_epi32(EE4B, EO4B);
            const __m128i E5A = _mm_add_epi32(EE5A, EO5A);          // E5 =
            const __m128i E5B = _mm_add_epi32(EE5B, EO5B);
            const __m128i E6A = _mm_add_epi32(EE6A, EO6A);          // E6 =
            const __m128i E6B = _mm_add_epi32(EE6B, EO6B);
            const __m128i E7A = _mm_add_epi32(EE7A, EO7A);          // E7 =
            const __m128i E7B = _mm_add_epi32(EE7B, EO7B);
            const __m128i EFA = _mm_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
            const __m128i EFB = _mm_sub_epi32(EE0B, EO0B);
            const __m128i EEA = _mm_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
            const __m128i EEB = _mm_sub_epi32(EE1B, EO1B);
            const __m128i EDA = _mm_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
            const __m128i EDB = _mm_sub_epi32(EE2B, EO2B);
            const __m128i ECA = _mm_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
            const __m128i ECB = _mm_sub_epi32(EE3B, EO3B);
            const __m128i EBA = _mm_sub_epi32(EE4A, EO4A);          // EB =
            const __m128i EBB = _mm_sub_epi32(EE4B, EO4B);
            const __m128i EAA = _mm_sub_epi32(EE5A, EO5A);          // EA =
            const __m128i EAB = _mm_sub_epi32(EE5B, EO5B);
            const __m128i E9A = _mm_sub_epi32(EE6A, EO6A);          // E9 =
            const __m128i E9B = _mm_sub_epi32(EE6B, EO6B);
            const __m128i E8A = _mm_sub_epi32(EE7A, EO7A);          // E8 =
            const __m128i E8B = _mm_sub_epi32(EE7B, EO7B);

            const __m128i T10A = _mm_add_epi32(E0A, c32_rnd);         // E0 + rnd
            const __m128i T10B = _mm_add_epi32(E0B, c32_rnd);
            const __m128i T11A = _mm_add_epi32(E1A, c32_rnd);         // E1 + rnd
            const __m128i T11B = _mm_add_epi32(E1B, c32_rnd);
            const __m128i T12A = _mm_add_epi32(E2A, c32_rnd);         // E2 + rnd
            const __m128i T12B = _mm_add_epi32(E2B, c32_rnd);
            const __m128i T13A = _mm_add_epi32(E3A, c32_rnd);         // E3 + rnd
            const __m128i T13B = _mm_add_epi32(E3B, c32_rnd);
            const __m128i T14A = _mm_add_epi32(E4A, c32_rnd);         // E4 + rnd
            const __m128i T14B = _mm_add_epi32(E4B, c32_rnd);
            const __m128i T15A = _mm_add_epi32(E5A, c32_rnd);         // E5 + rnd
            const __m128i T15B = _mm_add_epi32(E5B, c32_rnd);
            const __m128i T16A = _mm_add_epi32(E6A, c32_rnd);         // E6 + rnd
            const __m128i T16B = _mm_add_epi32(E6B, c32_rnd);
            const __m128i T17A = _mm_add_epi32(E7A, c32_rnd);         // E7 + rnd
            const __m128i T17B = _mm_add_epi32(E7B, c32_rnd);
            const __m128i T18A = _mm_add_epi32(E8A, c32_rnd);         // E8 + rnd
            const __m128i T18B = _mm_add_epi32(E8B, c32_rnd);
            const __m128i T19A = _mm_add_epi32(E9A, c32_rnd);         // E9 + rnd
            const __m128i T19B = _mm_add_epi32(E9B, c32_rnd);
            const __m128i T1AA = _mm_add_epi32(EAA, c32_rnd);         // E10 + rnd
            const __m128i T1AB = _mm_add_epi32(EAB, c32_rnd);
            const __m128i T1BA = _mm_add_epi32(EBA, c32_rnd);         // E11 + rnd
            const __m128i T1BB = _mm_add_epi32(EBB, c32_rnd);
            const __m128i T1CA = _mm_add_epi32(ECA, c32_rnd);         // E12 + rnd
            const __m128i T1CB = _mm_add_epi32(ECB, c32_rnd);
            const __m128i T1DA = _mm_add_epi32(EDA, c32_rnd);         // E13 + rnd
            const __m128i T1DB = _mm_add_epi32(EDB, c32_rnd);
            const __m128i T1EA = _mm_add_epi32(EEA, c32_rnd);         // E14 + rnd
            const __m128i T1EB = _mm_add_epi32(EEB, c32_rnd);
            const __m128i T1FA = _mm_add_epi32(EFA, c32_rnd);         // E15 + rnd
            const __m128i T1FB = _mm_add_epi32(EFB, c32_rnd);

            const __m128i T2_00A = _mm_add_epi32(T10A, O00A);          // E0 + O0 + rnd
            const __m128i T2_00B = _mm_add_epi32(T10B, O00B);
            const __m128i T2_01A = _mm_add_epi32(T11A, O01A);          // E1 + O1 + rnd
            const __m128i T2_01B = _mm_add_epi32(T11B, O01B);
            const __m128i T2_02A = _mm_add_epi32(T12A, O02A);          // E2 + O2 + rnd
            const __m128i T2_02B = _mm_add_epi32(T12B, O02B);
            const __m128i T2_03A = _mm_add_epi32(T13A, O03A);          // E3 + O3 + rnd
            const __m128i T2_03B = _mm_add_epi32(T13B, O03B);
            const __m128i T2_04A = _mm_add_epi32(T14A, O04A);          // E4
            const __m128i T2_04B = _mm_add_epi32(T14B, O04B);
            const __m128i T2_05A = _mm_add_epi32(T15A, O05A);          // E5
            const __m128i T2_05B = _mm_add_epi32(T15B, O05B);
            const __m128i T2_06A = _mm_add_epi32(T16A, O06A);          // E6
            const __m128i T2_06B = _mm_add_epi32(T16B, O06B);
            const __m128i T2_07A = _mm_add_epi32(T17A, O07A);          // E7
            const __m128i T2_07B = _mm_add_epi32(T17B, O07B);
            const __m128i T2_08A = _mm_add_epi32(T18A, O08A);          // E8
            const __m128i T2_08B = _mm_add_epi32(T18B, O08B);
            const __m128i T2_09A = _mm_add_epi32(T19A, O09A);          // E9
            const __m128i T2_09B = _mm_add_epi32(T19B, O09B);
            const __m128i T2_10A = _mm_add_epi32(T1AA, O10A);          // E10
            const __m128i T2_10B = _mm_add_epi32(T1AB, O10B);
            const __m128i T2_11A = _mm_add_epi32(T1BA, O11A);          // E11
            const __m128i T2_11B = _mm_add_epi32(T1BB, O11B);
            const __m128i T2_12A = _mm_add_epi32(T1CA, O12A);          // E12
            const __m128i T2_12B = _mm_add_epi32(T1CB, O12B);
            const __m128i T2_13A = _mm_add_epi32(T1DA, O13A);          // E13
            const __m128i T2_13B = _mm_add_epi32(T1DB, O13B);
            const __m128i T2_14A = _mm_add_epi32(T1EA, O14A);          // E14
            const __m128i T2_14B = _mm_add_epi32(T1EB, O14B);
            const __m128i T2_15A = _mm_add_epi32(T1FA, O15A);          // E15
            const __m128i T2_15B = _mm_add_epi32(T1FB, O15B);
            const __m128i T2_31A = _mm_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
            const __m128i T2_31B = _mm_sub_epi32(T10B, O00B);
            const __m128i T2_30A = _mm_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
            const __m128i T2_30B = _mm_sub_epi32(T11B, O01B);
            const __m128i T2_29A = _mm_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
            const __m128i T2_29B = _mm_sub_epi32(T12B, O02B);
            const __m128i T2_28A = _mm_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
            const __m128i T2_28B = _mm_sub_epi32(T13B, O03B);
            const __m128i T2_27A = _mm_sub_epi32(T14A, O04A);          // E4
            const __m128i T2_27B = _mm_sub_epi32(T14B, O04B);
            const __m128i T2_26A = _mm_sub_epi32(T15A, O05A);          // E5
            const __m128i T2_26B = _mm_sub_epi32(T15B, O05B);
            const __m128i T2_25A = _mm_sub_epi32(T16A, O06A);          // E6
            const __m128i T2_25B = _mm_sub_epi32(T16B, O06B);
            const __m128i T2_24A = _mm_sub_epi32(T17A, O07A);          // E7
            const __m128i T2_24B = _mm_sub_epi32(T17B, O07B);
            const __m128i T2_23A = _mm_sub_epi32(T18A, O08A);          //
            const __m128i T2_23B = _mm_sub_epi32(T18B, O08B);
            const __m128i T2_22A = _mm_sub_epi32(T19A, O09A);          //
            const __m128i T2_22B = _mm_sub_epi32(T19B, O09B);
            const __m128i T2_21A = _mm_sub_epi32(T1AA, O10A);          //
            const __m128i T2_21B = _mm_sub_epi32(T1AB, O10B);
            const __m128i T2_20A = _mm_sub_epi32(T1BA, O11A);          //
            const __m128i T2_20B = _mm_sub_epi32(T1BB, O11B);
            const __m128i T2_19A = _mm_sub_epi32(T1CA, O12A);          //
            const __m128i T2_19B = _mm_sub_epi32(T1CB, O12B);
            const __m128i T2_18A = _mm_sub_epi32(T1DA, O13A);          //
            const __m128i T2_18B = _mm_sub_epi32(T1DB, O13B);
            const __m128i T2_17A = _mm_sub_epi32(T1EA, O14A);          //
            const __m128i T2_17B = _mm_sub_epi32(T1EB, O14B);
            const __m128i T2_16A = _mm_sub_epi32(T1FA, O15A);          //
            const __m128i T2_16B = _mm_sub_epi32(T1FB, O15B);

            const __m128i T3_00A = _mm_srai_epi32(T2_00A, nShift);             // [30 20 10 00]
            const __m128i T3_00B = _mm_srai_epi32(T2_00B, nShift);             // [70 60 50 40]
            const __m128i T3_01A = _mm_srai_epi32(T2_01A, nShift);             // [31 21 11 01]
            const __m128i T3_01B = _mm_srai_epi32(T2_01B, nShift);             // [71 61 51 41]
            const __m128i T3_02A = _mm_srai_epi32(T2_02A, nShift);             // [32 22 12 02]
            const __m128i T3_02B = _mm_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
            const __m128i T3_03A = _mm_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
            const __m128i T3_03B = _mm_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
            const __m128i T3_04A = _mm_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
            const __m128i T3_04B = _mm_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
            const __m128i T3_05A = _mm_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
            const __m128i T3_05B = _mm_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
            const __m128i T3_06A = _mm_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
            const __m128i T3_06B = _mm_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
            const __m128i T3_07A = _mm_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
            const __m128i T3_07B = _mm_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
            const __m128i T3_08A = _mm_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
            const __m128i T3_08B = _mm_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
            const __m128i T3_09A = _mm_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
            const __m128i T3_09B = _mm_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
            const __m128i T3_10A = _mm_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
            const __m128i T3_10B = _mm_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
            const __m128i T3_11A = _mm_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
            const __m128i T3_11B = _mm_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
            const __m128i T3_12A = _mm_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
            const __m128i T3_12B = _mm_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
            const __m128i T3_13A = _mm_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
            const __m128i T3_13B = _mm_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
            const __m128i T3_14A = _mm_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
            const __m128i T3_14B = _mm_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
            const __m128i T3_15A = _mm_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
            const __m128i T3_15B = _mm_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

            const __m128i T3_16A = _mm_srai_epi32(T2_16A, nShift);             // [30 20 10 00]
            const __m128i T3_16B = _mm_srai_epi32(T2_16B, nShift);             // [70 60 50 40]
            const __m128i T3_17A = _mm_srai_epi32(T2_17A, nShift);             // [31 21 11 01]
            const __m128i T3_17B = _mm_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
            const __m128i T3_18A = _mm_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
            const __m128i T3_18B = _mm_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
            const __m128i T3_19A = _mm_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
            const __m128i T3_19B = _mm_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
            const __m128i T3_20A = _mm_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
            const __m128i T3_20B = _mm_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
            const __m128i T3_21A = _mm_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
            const __m128i T3_21B = _mm_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
            const __m128i T3_22A = _mm_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
            const __m128i T3_22B = _mm_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
            const __m128i T3_23A = _mm_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
            const __m128i T3_23B = _mm_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
            const __m128i T3_24A = _mm_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
            const __m128i T3_24B = _mm_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
            const __m128i T3_25A = _mm_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
            const __m128i T3_25B = _mm_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
            const __m128i T3_26A = _mm_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
            const __m128i T3_26B = _mm_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
            const __m128i T3_27A = _mm_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
            const __m128i T3_27B = _mm_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
            const __m128i T3_28A = _mm_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
            const __m128i T3_28B = _mm_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
            const __m128i T3_29A = _mm_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
            const __m128i T3_29B = _mm_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
            const __m128i T3_30A = _mm_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
            const __m128i T3_30B = _mm_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
            const __m128i T3_31A = _mm_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
            const __m128i T3_31B = _mm_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

            res00[part] = _mm_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
            res01[part] = _mm_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
            res02[part] = _mm_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
            res03[part] = _mm_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
            res04[part] = _mm_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
            res05[part] = _mm_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
            res06[part] = _mm_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
            res07[part] = _mm_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
            res08[part] = _mm_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
            res09[part] = _mm_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
            res10[part] = _mm_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
            res11[part] = _mm_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
            res12[part] = _mm_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
            res13[part] = _mm_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
            res14[part] = _mm_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
            res15[part] = _mm_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
            res16[part] = _mm_packs_epi32(T3_16A, T3_16B);
            res17[part] = _mm_packs_epi32(T3_17A, T3_17B);
            res18[part] = _mm_packs_epi32(T3_18A, T3_18B);
            res19[part] = _mm_packs_epi32(T3_19A, T3_19B);
            res20[part] = _mm_packs_epi32(T3_20A, T3_20B);
            res21[part] = _mm_packs_epi32(T3_21A, T3_21B);
            res22[part] = _mm_packs_epi32(T3_22A, T3_22B);
            res23[part] = _mm_packs_epi32(T3_23A, T3_23B);
            res24[part] = _mm_packs_epi32(T3_24A, T3_24B);
            res25[part] = _mm_packs_epi32(T3_25A, T3_25B);
            res26[part] = _mm_packs_epi32(T3_26A, T3_26B);
            res27[part] = _mm_packs_epi32(T3_27A, T3_27B);
            res28[part] = _mm_packs_epi32(T3_28A, T3_28B);
            res29[part] = _mm_packs_epi32(T3_29A, T3_29B);
            res30[part] = _mm_packs_epi32(T3_30A, T3_30B);
            res31[part] = _mm_packs_epi32(T3_31A, T3_31B);
        }
    }
    //transpose matrix 8x8 16bit.
    {
        __m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
        __m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
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

        TRANSPOSE_8x8_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0]);
        TRANSPOSE_8x8_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
        TRANSPOSE_8x8_16BIT(res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0]);
        TRANSPOSE_8x8_16BIT(res00[3], res01[3], res02[3], res03[3], res04[3], res05[3], res06[3], res07[3], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0]);

        TRANSPOSE_8x8_16BIT(res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1]);
        TRANSPOSE_8x8_16BIT(res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);
        TRANSPOSE_8x8_16BIT(res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1]);
        TRANSPOSE_8x8_16BIT(res08[3], res09[3], res10[3], res11[3], res12[3], res13[3], res14[3], res15[3], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1]);

        TRANSPOSE_8x8_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2]);
        TRANSPOSE_8x8_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2]);
        TRANSPOSE_8x8_16BIT(res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2]);
        TRANSPOSE_8x8_16BIT(res16[3], res17[3], res18[3], res19[3], res20[3], res21[3], res22[3], res23[3], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2]);

        TRANSPOSE_8x8_16BIT(res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[3], in01[3], in02[3], in03[3], in04[3], in05[3], in06[3], in07[3]);
        TRANSPOSE_8x8_16BIT(res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in08[3], in09[3], in10[3], in11[3], in12[3], in13[3], in14[3], in15[3]);
        TRANSPOSE_8x8_16BIT(res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2], in16[3], in17[3], in18[3], in19[3], in20[3], in21[3], in22[3], in23[3]);
        TRANSPOSE_8x8_16BIT(res24[3], res25[3], res26[3], res27[3], res28[3], res29[3], res30[3], res31[3], in24[3], in25[3], in26[3], in27[3], in28[3], in29[3], in30[3], in31[3]);

#undef TRANSPOSE_8x8_16BIT
    }

	//clip
	{
		__m128i max_val = _mm_set1_epi16(255);
		__m128i min_val = _mm_set1_epi16(-256);

		for (k = 0; k < 4; k++)
		{
			in00[k] = _mm_min_epi16(in00[k], max_val);
			in00[k] = _mm_max_epi16(in00[k], min_val);
			in01[k] = _mm_min_epi16(in01[k], max_val);
			in01[k] = _mm_max_epi16(in01[k], min_val);
			in02[k] = _mm_min_epi16(in02[k], max_val);
			in02[k] = _mm_max_epi16(in02[k], min_val);
			in03[k] = _mm_min_epi16(in03[k], max_val);
			in03[k] = _mm_max_epi16(in03[k], min_val);
			in04[k] = _mm_min_epi16(in04[k], max_val);
			in04[k] = _mm_max_epi16(in04[k], min_val);
			in05[k] = _mm_min_epi16(in05[k], max_val);
			in05[k] = _mm_max_epi16(in05[k], min_val);
			in06[k] = _mm_min_epi16(in06[k], max_val);
			in06[k] = _mm_max_epi16(in06[k], min_val);
			in07[k] = _mm_min_epi16(in07[k], max_val);
			in07[k] = _mm_max_epi16(in07[k], min_val);
			in08[k] = _mm_min_epi16(in08[k], max_val);
			in08[k] = _mm_max_epi16(in08[k], min_val);
			in09[k] = _mm_min_epi16(in09[k], max_val);
			in09[k] = _mm_max_epi16(in09[k], min_val);
			in10[k] = _mm_min_epi16(in10[k], max_val);
			in10[k] = _mm_max_epi16(in10[k], min_val);
			in11[k] = _mm_min_epi16(in11[k], max_val);
			in11[k] = _mm_max_epi16(in11[k], min_val);
			in12[k] = _mm_min_epi16(in12[k], max_val);
			in12[k] = _mm_max_epi16(in12[k], min_val);
			in13[k] = _mm_min_epi16(in13[k], max_val);
			in13[k] = _mm_max_epi16(in13[k], min_val);
			in14[k] = _mm_min_epi16(in14[k], max_val);
			in14[k] = _mm_max_epi16(in14[k], min_val);
			in15[k] = _mm_min_epi16(in15[k], max_val);
			in15[k] = _mm_max_epi16(in15[k], min_val);
			in16[k] = _mm_min_epi16(in16[k], max_val);
			in16[k] = _mm_max_epi16(in16[k], min_val);
			in17[k] = _mm_min_epi16(in17[k], max_val);
			in17[k] = _mm_max_epi16(in17[k], min_val);
			in18[k] = _mm_min_epi16(in18[k], max_val);
			in18[k] = _mm_max_epi16(in18[k], min_val);
			in19[k] = _mm_min_epi16(in19[k], max_val);
			in19[k] = _mm_max_epi16(in19[k], min_val);
			in20[k] = _mm_min_epi16(in20[k], max_val);
			in20[k] = _mm_max_epi16(in20[k], min_val);
			in21[k] = _mm_min_epi16(in21[k], max_val);
			in21[k] = _mm_max_epi16(in21[k], min_val);
			in22[k] = _mm_min_epi16(in22[k], max_val);
			in22[k] = _mm_max_epi16(in22[k], min_val);
			in23[k] = _mm_min_epi16(in23[k], max_val);
			in23[k] = _mm_max_epi16(in23[k], min_val);
			in24[k] = _mm_min_epi16(in24[k], max_val);
			in24[k] = _mm_max_epi16(in24[k], min_val);
			in25[k] = _mm_min_epi16(in25[k], max_val);
			in25[k] = _mm_max_epi16(in25[k], min_val);
			in26[k] = _mm_min_epi16(in26[k], max_val);
			in26[k] = _mm_max_epi16(in26[k], min_val);
			in27[k] = _mm_min_epi16(in27[k], max_val);
			in27[k] = _mm_max_epi16(in27[k], min_val);
			in28[k] = _mm_min_epi16(in28[k], max_val);
			in28[k] = _mm_max_epi16(in28[k], min_val);
			in29[k] = _mm_min_epi16(in29[k], max_val);
			in29[k] = _mm_max_epi16(in29[k], min_val);
			in30[k] = _mm_min_epi16(in30[k], max_val);
			in30[k] = _mm_max_epi16(in30[k], min_val);
			in31[k] = _mm_min_epi16(in31[k], max_val);
			in31[k] = _mm_max_epi16(in31[k], min_val);
		}
	}

	// Add
	for (k = 0; k < 2; k++)
	{
		int offset = (k << 4);
		P00[k] = _mm_loadu_si128((const __m128i*)&pred[0 + offset]);
		P01[k] = _mm_loadu_si128((const __m128i*)&pred[1 * i_pred + offset]);
		P02[k] = _mm_loadu_si128((const __m128i*)&pred[2 * i_pred + offset]);
		P03[k] = _mm_loadu_si128((const __m128i*)&pred[3 * i_pred + offset]);
		P04[k] = _mm_loadu_si128((const __m128i*)&pred[4 * i_pred + offset]);
		P05[k] = _mm_loadu_si128((const __m128i*)&pred[5 * i_pred + offset]);
		P06[k] = _mm_loadu_si128((const __m128i*)&pred[6 * i_pred + offset]);
		P07[k] = _mm_loadu_si128((const __m128i*)&pred[7 * i_pred + offset]);
		P08[k] = _mm_loadu_si128((const __m128i*)&pred[8 * i_pred + offset]);
		P09[k] = _mm_loadu_si128((const __m128i*)&pred[9 * i_pred + offset]);
		P10[k] = _mm_loadu_si128((const __m128i*)&pred[10 * i_pred + offset]);
		P11[k] = _mm_loadu_si128((const __m128i*)&pred[11 * i_pred + offset]);
		P12[k] = _mm_loadu_si128((const __m128i*)&pred[12 * i_pred + offset]);
		P13[k] = _mm_loadu_si128((const __m128i*)&pred[13 * i_pred + offset]);
		P14[k] = _mm_loadu_si128((const __m128i*)&pred[14 * i_pred + offset]);
		P15[k] = _mm_loadu_si128((const __m128i*)&pred[15 * i_pred + offset]);
		P16[k] = _mm_loadu_si128((const __m128i*)&pred[16 * i_pred + offset]);
		P17[k] = _mm_loadu_si128((const __m128i*)&pred[17 * i_pred + offset]);
		P18[k] = _mm_loadu_si128((const __m128i*)&pred[18 * i_pred + offset]);
		P19[k] = _mm_loadu_si128((const __m128i*)&pred[19 * i_pred + offset]);
		P20[k] = _mm_loadu_si128((const __m128i*)&pred[20 * i_pred + offset]);
		P21[k] = _mm_loadu_si128((const __m128i*)&pred[21 * i_pred + offset]);
		P22[k] = _mm_loadu_si128((const __m128i*)&pred[22 * i_pred + offset]);
		P23[k] = _mm_loadu_si128((const __m128i*)&pred[23 * i_pred + offset]);
		P24[k] = _mm_loadu_si128((const __m128i*)&pred[24 * i_pred + offset]);
		P25[k] = _mm_loadu_si128((const __m128i*)&pred[25 * i_pred + offset]);
		P26[k] = _mm_loadu_si128((const __m128i*)&pred[26 * i_pred + offset]);
		P27[k] = _mm_loadu_si128((const __m128i*)&pred[27 * i_pred + offset]);
		P28[k] = _mm_loadu_si128((const __m128i*)&pred[28 * i_pred + offset]);
		P29[k] = _mm_loadu_si128((const __m128i*)&pred[29 * i_pred + offset]);
		P30[k] = _mm_loadu_si128((const __m128i*)&pred[30 * i_pred + offset]);
		P31[k] = _mm_loadu_si128((const __m128i*)&pred[31 * i_pred + offset]);
	}

	for (k = 0; k < 2; k++)
	{
		int offset = k << 1;
		res00[0 + offset] = _mm_unpacklo_epi8(P00[k], zero);
		res00[1 + offset] = _mm_unpackhi_epi8(P00[k], zero);
		res01[0 + offset] = _mm_unpacklo_epi8(P01[k], zero);
		res01[1 + offset] = _mm_unpackhi_epi8(P01[k], zero);
		res02[0 + offset] = _mm_unpacklo_epi8(P02[k], zero);
		res02[1 + offset] = _mm_unpackhi_epi8(P02[k], zero);
		res03[0 + offset] = _mm_unpacklo_epi8(P03[k], zero);
		res03[1 + offset] = _mm_unpackhi_epi8(P03[k], zero);
		res04[0 + offset] = _mm_unpacklo_epi8(P04[k], zero);
		res04[1 + offset] = _mm_unpackhi_epi8(P04[k], zero);
		res05[0 + offset] = _mm_unpacklo_epi8(P05[k], zero);
		res05[1 + offset] = _mm_unpackhi_epi8(P05[k], zero);
		res06[0 + offset] = _mm_unpacklo_epi8(P06[k], zero);
		res06[1 + offset] = _mm_unpackhi_epi8(P06[k], zero);
		res07[0 + offset] = _mm_unpacklo_epi8(P07[k], zero);
		res07[1 + offset] = _mm_unpackhi_epi8(P07[k], zero);
		res08[0 + offset] = _mm_unpacklo_epi8(P08[k], zero);
		res08[1 + offset] = _mm_unpackhi_epi8(P08[k], zero);
		res09[0 + offset] = _mm_unpacklo_epi8(P09[k], zero);
		res09[1 + offset] = _mm_unpackhi_epi8(P09[k], zero);
		res10[0 + offset] = _mm_unpacklo_epi8(P10[k], zero);
		res10[1 + offset] = _mm_unpackhi_epi8(P10[k], zero);
		res11[0 + offset] = _mm_unpacklo_epi8(P11[k], zero);
		res11[1 + offset] = _mm_unpackhi_epi8(P11[k], zero);
		res12[0 + offset] = _mm_unpacklo_epi8(P12[k], zero);
		res12[1 + offset] = _mm_unpackhi_epi8(P12[k], zero);
		res13[0 + offset] = _mm_unpacklo_epi8(P13[k], zero);
		res13[1 + offset] = _mm_unpackhi_epi8(P13[k], zero);
		res14[0 + offset] = _mm_unpacklo_epi8(P14[k], zero);
		res14[1 + offset] = _mm_unpackhi_epi8(P14[k], zero);
		res15[0 + offset] = _mm_unpacklo_epi8(P15[k], zero);
		res15[1 + offset] = _mm_unpackhi_epi8(P15[k], zero);
		res16[0 + offset] = _mm_unpacklo_epi8(P16[k], zero);
		res16[1 + offset] = _mm_unpackhi_epi8(P16[k], zero);
		res17[0 + offset] = _mm_unpacklo_epi8(P17[k], zero);
		res17[1 + offset] = _mm_unpackhi_epi8(P17[k], zero);
		res18[0 + offset] = _mm_unpacklo_epi8(P18[k], zero);
		res18[1 + offset] = _mm_unpackhi_epi8(P18[k], zero);
		res19[0 + offset] = _mm_unpacklo_epi8(P19[k], zero);
		res19[1 + offset] = _mm_unpackhi_epi8(P19[k], zero);
		res20[0 + offset] = _mm_unpacklo_epi8(P20[k], zero);
		res20[1 + offset] = _mm_unpackhi_epi8(P20[k], zero);
		res21[0 + offset] = _mm_unpacklo_epi8(P21[k], zero);
		res21[1 + offset] = _mm_unpackhi_epi8(P21[k], zero);
		res22[0 + offset] = _mm_unpacklo_epi8(P22[k], zero);
		res22[1 + offset] = _mm_unpackhi_epi8(P22[k], zero);
		res23[0 + offset] = _mm_unpacklo_epi8(P23[k], zero);
		res23[1 + offset] = _mm_unpackhi_epi8(P23[k], zero);
		res24[0 + offset] = _mm_unpacklo_epi8(P24[k], zero);
		res24[1 + offset] = _mm_unpackhi_epi8(P24[k], zero);
		res25[0 + offset] = _mm_unpacklo_epi8(P25[k], zero);
		res25[1 + offset] = _mm_unpackhi_epi8(P25[k], zero);
		res26[0 + offset] = _mm_unpacklo_epi8(P26[k], zero);
		res26[1 + offset] = _mm_unpackhi_epi8(P26[k], zero);
		res27[0 + offset] = _mm_unpacklo_epi8(P27[k], zero);
		res27[1 + offset] = _mm_unpackhi_epi8(P27[k], zero);
		res28[0 + offset] = _mm_unpacklo_epi8(P28[k], zero);
		res28[1 + offset] = _mm_unpackhi_epi8(P28[k], zero);
		res29[0 + offset] = _mm_unpacklo_epi8(P29[k], zero);
		res29[1 + offset] = _mm_unpackhi_epi8(P29[k], zero);
		res30[0 + offset] = _mm_unpacklo_epi8(P30[k], zero);
		res30[1 + offset] = _mm_unpackhi_epi8(P30[k], zero);
		res31[0 + offset] = _mm_unpacklo_epi8(P31[k], zero);
		res31[1 + offset] = _mm_unpackhi_epi8(P31[k], zero);

	}

	for (k = 0; k < 4; k++)
	{
		res00[k] = _mm_add_epi16(in00[k], res00[k]);
		res01[k] = _mm_add_epi16(in01[k], res01[k]);
		res02[k] = _mm_add_epi16(in02[k], res02[k]);
		res03[k] = _mm_add_epi16(in03[k], res03[k]);
		res04[k] = _mm_add_epi16(in04[k], res04[k]);
		res05[k] = _mm_add_epi16(in05[k], res05[k]);
		res06[k] = _mm_add_epi16(in06[k], res06[k]);
		res07[k] = _mm_add_epi16(in07[k], res07[k]);
		res08[k] = _mm_add_epi16(in08[k], res08[k]);
		res09[k] = _mm_add_epi16(in09[k], res09[k]);
		res10[k] = _mm_add_epi16(in10[k], res10[k]);
		res11[k] = _mm_add_epi16(in11[k], res11[k]);
		res12[k] = _mm_add_epi16(in12[k], res12[k]);
		res13[k] = _mm_add_epi16(in13[k], res13[k]);
		res14[k] = _mm_add_epi16(in14[k], res14[k]);
		res15[k] = _mm_add_epi16(in15[k], res15[k]);
		res16[k] = _mm_add_epi16(in16[k], res16[k]);
		res17[k] = _mm_add_epi16(in17[k], res17[k]);
		res18[k] = _mm_add_epi16(in18[k], res18[k]);
		res19[k] = _mm_add_epi16(in19[k], res19[k]);
		res20[k] = _mm_add_epi16(in20[k], res20[k]);
		res21[k] = _mm_add_epi16(in21[k], res21[k]);
		res22[k] = _mm_add_epi16(in22[k], res22[k]);
		res23[k] = _mm_add_epi16(in23[k], res23[k]);
		res24[k] = _mm_add_epi16(in24[k], res24[k]);
		res25[k] = _mm_add_epi16(in25[k], res25[k]);
		res26[k] = _mm_add_epi16(in26[k], res26[k]);
		res27[k] = _mm_add_epi16(in27[k], res27[k]);
		res28[k] = _mm_add_epi16(in28[k], res28[k]);
		res29[k] = _mm_add_epi16(in29[k], res29[k]);
		res30[k] = _mm_add_epi16(in30[k], res30[k]);
		res31[k] = _mm_add_epi16(in31[k], res31[k]);
	}


	for (k = 0; k < 4; k+=2)
	{
		in00[k] = _mm_packus_epi16(res00[k], res00[k + 1]);
		in01[k] = _mm_packus_epi16(res01[k], res01[k + 1]);
		in02[k] = _mm_packus_epi16(res02[k], res02[k + 1]);
		in03[k] = _mm_packus_epi16(res03[k], res03[k + 1]);
		in04[k] = _mm_packus_epi16(res04[k], res04[k + 1]);
		in05[k] = _mm_packus_epi16(res05[k], res05[k + 1]);
		in06[k] = _mm_packus_epi16(res06[k], res06[k + 1]);
		in07[k] = _mm_packus_epi16(res07[k], res07[k + 1]);
		in08[k] = _mm_packus_epi16(res08[k], res08[k + 1]);
		in09[k] = _mm_packus_epi16(res09[k], res09[k + 1]);
		in10[k] = _mm_packus_epi16(res10[k], res10[k + 1]);
		in11[k] = _mm_packus_epi16(res11[k], res11[k + 1]);
		in12[k] = _mm_packus_epi16(res12[k], res12[k + 1]);
		in13[k] = _mm_packus_epi16(res13[k], res13[k + 1]);
		in14[k] = _mm_packus_epi16(res14[k], res14[k + 1]);
		in15[k] = _mm_packus_epi16(res15[k], res15[k + 1]);
		in16[k] = _mm_packus_epi16(res16[k], res16[k + 1]);
		in17[k] = _mm_packus_epi16(res17[k], res17[k + 1]);
		in18[k] = _mm_packus_epi16(res18[k], res18[k + 1]);
		in19[k] = _mm_packus_epi16(res19[k], res19[k + 1]);
		in20[k] = _mm_packus_epi16(res20[k], res20[k + 1]);
		in21[k] = _mm_packus_epi16(res21[k], res21[k + 1]);
		in22[k] = _mm_packus_epi16(res22[k], res22[k + 1]);
		in23[k] = _mm_packus_epi16(res23[k], res23[k + 1]);
		in24[k] = _mm_packus_epi16(res24[k], res24[k + 1]);
		in25[k] = _mm_packus_epi16(res25[k], res25[k + 1]);
		in26[k] = _mm_packus_epi16(res26[k], res26[k + 1]);
		in27[k] = _mm_packus_epi16(res27[k], res27[k + 1]);
		in28[k] = _mm_packus_epi16(res28[k], res28[k + 1]);
		in29[k] = _mm_packus_epi16(res29[k], res29[k + 1]);
		in30[k] = _mm_packus_epi16(res30[k], res30[k + 1]);
		in31[k] = _mm_packus_epi16(res31[k], res31[k + 1]);
	}
	

	for (k = 0; k < 4; k+=2)
	{
		int offset = k <<3;
		_mm_storeu_si128((__m128i*)&dst[0 + offset], in00[k]);
		_mm_storeu_si128((__m128i*)&dst[1 * i_dst + offset], in01[k]);
		_mm_storeu_si128((__m128i*)&dst[2 * i_dst + offset], in02[k]);
		_mm_storeu_si128((__m128i*)&dst[3 * i_dst + offset], in03[k]);
		_mm_storeu_si128((__m128i*)&dst[4 * i_dst + offset], in04[k]);
		_mm_storeu_si128((__m128i*)&dst[5 * i_dst + offset], in05[k]);
		_mm_storeu_si128((__m128i*)&dst[6 * i_dst + offset], in06[k]);
		_mm_storeu_si128((__m128i*)&dst[7 * i_dst + offset], in07[k]);
		_mm_storeu_si128((__m128i*)&dst[8 * i_dst + offset], in08[k]);
		_mm_storeu_si128((__m128i*)&dst[9 * i_dst + offset], in09[k]);

		_mm_storeu_si128((__m128i*)&dst[10 * i_dst + offset], in10[k]);
		_mm_storeu_si128((__m128i*)&dst[11 * i_dst + offset], in11[k]);
		_mm_storeu_si128((__m128i*)&dst[12 * i_dst + offset], in12[k]);
		_mm_storeu_si128((__m128i*)&dst[13 * i_dst + offset], in13[k]);
		_mm_storeu_si128((__m128i*)&dst[14 * i_dst + offset], in14[k]);
		_mm_storeu_si128((__m128i*)&dst[15 * i_dst + offset], in15[k]);
		_mm_storeu_si128((__m128i*)&dst[16 * i_dst + offset], in16[k]);
		_mm_storeu_si128((__m128i*)&dst[17 * i_dst + offset], in17[k]);
		_mm_storeu_si128((__m128i*)&dst[18 * i_dst + offset], in18[k]);
		_mm_storeu_si128((__m128i*)&dst[19 * i_dst + offset], in19[k]);

		_mm_storeu_si128((__m128i*)&dst[20 * i_dst + offset], in20[k]);
		_mm_storeu_si128((__m128i*)&dst[21 * i_dst + offset], in21[k]);
		_mm_storeu_si128((__m128i*)&dst[22 * i_dst + offset], in22[k]);
		_mm_storeu_si128((__m128i*)&dst[23 * i_dst + offset], in23[k]);
		_mm_storeu_si128((__m128i*)&dst[24 * i_dst + offset], in24[k]);
		_mm_storeu_si128((__m128i*)&dst[25 * i_dst + offset], in25[k]);
		_mm_storeu_si128((__m128i*)&dst[26 * i_dst + offset], in26[k]);
		_mm_storeu_si128((__m128i*)&dst[27 * i_dst + offset], in27[k]);
		_mm_storeu_si128((__m128i*)&dst[28 * i_dst + offset], in28[k]);
		_mm_storeu_si128((__m128i*)&dst[29 * i_dst + offset], in29[k]);

		_mm_storeu_si128((__m128i*)&dst[30 * i_dst + offset], in30[k]);
		_mm_storeu_si128((__m128i*)&dst[31 * i_dst + offset], in31[k]);
	}
}

void add_inv_trans_ext_64x64_sse128(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    int k;

    __m128i P00[4], P01[4], P02[4], P03[4], P04[4], P05[4], P06[4], P07[4], P08[4], P09[4], P10[4], P11[4], P12[4], P13[4], P14[4], P15[4], P16[4], P17[4], P18[4], P19[4], P20[4], P21[4], P22[4], P23[4], P24[4], P25[4], P26[4], P27[4], P28[4], P29[4];
    __m128i P30[4], P31[4], P32[4], P33[4], P34[4], P35[4], P36[4], P37[4], P38[4], P39[4], P40[4], P41[4], P42[4], P43[4], P44[4], P45[4], P46[4], P47[4], P48[4], P49[4], P50[4], P51[4], P52[4], P53[4], P54[4], P55[4], P56[4], P57[4], P58[4], P59[4];
    __m128i P60[4], P61[4], P62[4], P63[4];
    const __m128i c16_p45_p45 = _mm_set1_epi32(0x002D002D);
    const __m128i c16_p43_p44 = _mm_set1_epi32(0x002B002C);
    const __m128i c16_p39_p41 = _mm_set1_epi32(0x00270029);
    const __m128i c16_p34_p36 = _mm_set1_epi32(0x00220024);
    const __m128i c16_p27_p30 = _mm_set1_epi32(0x001B001E);
    const __m128i c16_p19_p23 = _mm_set1_epi32(0x00130017);
    const __m128i c16_p11_p15 = _mm_set1_epi32(0x000B000F);
    const __m128i c16_p02_p07 = _mm_set1_epi32(0x00020007);
    const __m128i c16_p41_p45 = _mm_set1_epi32(0x0029002D);
    const __m128i c16_p23_p34 = _mm_set1_epi32(0x00170022);
    const __m128i c16_n02_p11 = _mm_set1_epi32(0xFFFE000B);
    const __m128i c16_n27_n15 = _mm_set1_epi32(0xFFE5FFF1);
    const __m128i c16_n43_n36 = _mm_set1_epi32(0xFFD5FFDC);
    const __m128i c16_n44_n45 = _mm_set1_epi32(0xFFD4FFD3);
    const __m128i c16_n30_n39 = _mm_set1_epi32(0xFFE2FFD9);
    const __m128i c16_n07_n19 = _mm_set1_epi32(0xFFF9FFED);
    const __m128i c16_p34_p44 = _mm_set1_epi32(0x0022002C);
    const __m128i c16_n07_p15 = _mm_set1_epi32(0xFFF9000F);
    const __m128i c16_n41_n27 = _mm_set1_epi32(0xFFD7FFE5);
    const __m128i c16_n39_n45 = _mm_set1_epi32(0xFFD9FFD3);
    const __m128i c16_n02_n23 = _mm_set1_epi32(0xFFFEFFE9);
    const __m128i c16_p36_p19 = _mm_set1_epi32(0x00240013);
    const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
    const __m128i c16_p11_p30 = _mm_set1_epi32(0x000B001E);
    const __m128i c16_p23_p43 = _mm_set1_epi32(0x0017002B);
    const __m128i c16_n34_n07 = _mm_set1_epi32(0xFFDEFFF9);
    const __m128i c16_n36_n45 = _mm_set1_epi32(0xFFDCFFD3);
    const __m128i c16_p19_n11 = _mm_set1_epi32(0x0013FFF5);
    const __m128i c16_p44_p41 = _mm_set1_epi32(0x002C0029);
    const __m128i c16_n02_p27 = _mm_set1_epi32(0xFFFE001B);
    const __m128i c16_n45_n30 = _mm_set1_epi32(0xFFD3FFE2);
    const __m128i c16_n15_n39 = _mm_set1_epi32(0xFFF1FFD9);
    const __m128i c16_p11_p41 = _mm_set1_epi32(0x000B0029);
    const __m128i c16_n45_n27 = _mm_set1_epi32(0xFFD3FFE5);
    const __m128i c16_p07_n30 = _mm_set1_epi32(0x0007FFE2);
    const __m128i c16_p43_p39 = _mm_set1_epi32(0x002B0027);
    const __m128i c16_n23_p15 = _mm_set1_epi32(0xFFE9000F);
    const __m128i c16_n34_n45 = _mm_set1_epi32(0xFFDEFFD3);
    const __m128i c16_p36_p02 = _mm_set1_epi32(0x00240002);
    const __m128i c16_p19_p44 = _mm_set1_epi32(0x0013002C);
    const __m128i c16_n02_p39 = _mm_set1_epi32(0xFFFE0027);
    const __m128i c16_n36_n41 = _mm_set1_epi32(0xFFDCFFD7);
    const __m128i c16_p43_p07 = _mm_set1_epi32(0x002B0007);
    const __m128i c16_n11_p34 = _mm_set1_epi32(0xFFF50022);
    const __m128i c16_n30_n44 = _mm_set1_epi32(0xFFE2FFD4);
    const __m128i c16_p45_p15 = _mm_set1_epi32(0x002D000F);
    const __m128i c16_n19_p27 = _mm_set1_epi32(0xFFED001B);
    const __m128i c16_n23_n45 = _mm_set1_epi32(0xFFE9FFD3);
    const __m128i c16_n15_p36 = _mm_set1_epi32(0xFFF10024);
    const __m128i c16_n11_n45 = _mm_set1_epi32(0xFFF5FFD3);
    const __m128i c16_p34_p39 = _mm_set1_epi32(0x00220027);
    const __m128i c16_n45_n19 = _mm_set1_epi32(0xFFD3FFED);
    const __m128i c16_p41_n07 = _mm_set1_epi32(0x0029FFF9);
    const __m128i c16_n23_p30 = _mm_set1_epi32(0xFFE9001E);
    const __m128i c16_n02_n44 = _mm_set1_epi32(0xFFFEFFD4);
    const __m128i c16_p27_p43 = _mm_set1_epi32(0x001B002B);
    const __m128i c16_n27_p34 = _mm_set1_epi32(0xFFE50022);
    const __m128i c16_p19_n39 = _mm_set1_epi32(0x0013FFD9);
    const __m128i c16_n11_p43 = _mm_set1_epi32(0xFFF5002B);
    const __m128i c16_p02_n45 = _mm_set1_epi32(0x0002FFD3);
    const __m128i c16_p07_p45 = _mm_set1_epi32(0x0007002D);
    const __m128i c16_n15_n44 = _mm_set1_epi32(0xFFF1FFD4);
    const __m128i c16_p23_p41 = _mm_set1_epi32(0x00170029);
    const __m128i c16_n30_n36 = _mm_set1_epi32(0xFFE2FFDC);
    const __m128i c16_n36_p30 = _mm_set1_epi32(0xFFDC001E);
    const __m128i c16_p41_n23 = _mm_set1_epi32(0x0029FFE9);
    const __m128i c16_n44_p15 = _mm_set1_epi32(0xFFD4000F);
    const __m128i c16_p45_n07 = _mm_set1_epi32(0x002DFFF9);
    const __m128i c16_n45_n02 = _mm_set1_epi32(0xFFD3FFFE);
    const __m128i c16_p43_p11 = _mm_set1_epi32(0x002B000B);
    const __m128i c16_n39_n19 = _mm_set1_epi32(0xFFD9FFED);
    const __m128i c16_p34_p27 = _mm_set1_epi32(0x0022001B);
    const __m128i c16_n43_p27 = _mm_set1_epi32(0xFFD5001B);
    const __m128i c16_p44_n02 = _mm_set1_epi32(0x002CFFFE);
    const __m128i c16_n30_n23 = _mm_set1_epi32(0xFFE2FFE9);
    const __m128i c16_p07_p41 = _mm_set1_epi32(0x00070029);
    const __m128i c16_p19_n45 = _mm_set1_epi32(0x0013FFD3);
    const __m128i c16_n39_p34 = _mm_set1_epi32(0xFFD90022);
    const __m128i c16_p45_n11 = _mm_set1_epi32(0x002DFFF5);
    const __m128i c16_n36_n15 = _mm_set1_epi32(0xFFDCFFF1);
    const __m128i c16_n45_p23 = _mm_set1_epi32(0xFFD30017);
    const __m128i c16_p27_p19 = _mm_set1_epi32(0x001B0013);
    const __m128i c16_p15_n45 = _mm_set1_epi32(0x000FFFD3);
    const __m128i c16_n44_p30 = _mm_set1_epi32(0xFFD4001E);
    const __m128i c16_p34_p11 = _mm_set1_epi32(0x0022000B);
    const __m128i c16_p07_n43 = _mm_set1_epi32(0x0007FFD5);
    const __m128i c16_n41_p36 = _mm_set1_epi32(0xFFD70024);
    const __m128i c16_p39_p02 = _mm_set1_epi32(0x00270002);
    const __m128i c16_n44_p19 = _mm_set1_epi32(0xFFD40013);
    const __m128i c16_n02_p36 = _mm_set1_epi32(0xFFFE0024);
    const __m128i c16_p45_n34 = _mm_set1_epi32(0x002DFFDE);
    const __m128i c16_n15_n23 = _mm_set1_epi32(0xFFF1FFE9);
    const __m128i c16_n39_p43 = _mm_set1_epi32(0xFFD9002B);
    const __m128i c16_p30_p07 = _mm_set1_epi32(0x001E0007);
    const __m128i c16_p27_n45 = _mm_set1_epi32(0x001BFFD3);
    const __m128i c16_n41_p11 = _mm_set1_epi32(0xFFD7000B);
    const __m128i c16_n39_p15 = _mm_set1_epi32(0xFFD9000F);
    const __m128i c16_n30_p45 = _mm_set1_epi32(0xFFE2002D);
    const __m128i c16_p27_p02 = _mm_set1_epi32(0x001B0002);
    const __m128i c16_p41_n44 = _mm_set1_epi32(0x0029FFD4);
    const __m128i c16_n11_n19 = _mm_set1_epi32(0xFFF5FFED);
    const __m128i c16_n45_p36 = _mm_set1_epi32(0xFFD30024);
    const __m128i c16_n07_p34 = _mm_set1_epi32(0xFFF90022);
    const __m128i c16_p43_n23 = _mm_set1_epi32(0x002BFFE9);
    const __m128i c16_n30_p11 = _mm_set1_epi32(0xFFE2000B);
    const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);
    const __m128i c16_n19_p36 = _mm_set1_epi32(0xFFED0024);
    const __m128i c16_p23_n02 = _mm_set1_epi32(0x0017FFFE);
    const __m128i c16_p45_n39 = _mm_set1_epi32(0x002DFFD9);
    const __m128i c16_p27_n41 = _mm_set1_epi32(0x001BFFD7);
    const __m128i c16_n15_n07 = _mm_set1_epi32(0xFFF1FFF9);
    const __m128i c16_n44_p34 = _mm_set1_epi32(0xFFD40022);
    const __m128i c16_n19_p07 = _mm_set1_epi32(0xFFED0007);
    const __m128i c16_n39_p30 = _mm_set1_epi32(0xFFD9001E);
    const __m128i c16_n45_p44 = _mm_set1_epi32(0xFFD3002C);
    const __m128i c16_n36_p43 = _mm_set1_epi32(0xFFDC002B);
    const __m128i c16_n15_p27 = _mm_set1_epi32(0xFFF1001B);
    const __m128i c16_p11_p02 = _mm_set1_epi32(0x000B0002);
    const __m128i c16_p34_n23 = _mm_set1_epi32(0x0022FFE9);
    const __m128i c16_p45_n41 = _mm_set1_epi32(0x002DFFD7);
    const __m128i c16_n07_p02 = _mm_set1_epi32(0xFFF90002);
    const __m128i c16_n15_p11 = _mm_set1_epi32(0xFFF1000B);
    const __m128i c16_n23_p19 = _mm_set1_epi32(0xFFE90013);
    const __m128i c16_n30_p27 = _mm_set1_epi32(0xFFE2001B);
    const __m128i c16_n36_p34 = _mm_set1_epi32(0xFFDC0022);
    const __m128i c16_n41_p39 = _mm_set1_epi32(0xFFD70027);
    const __m128i c16_n44_p43 = _mm_set1_epi32(0xFFD4002B);
    const __m128i c16_n45_p45 = _mm_set1_epi32(0xFFD3002D);

    //	const __m128i c16_p43_p45 = _mm_set1_epi32(0x002B002D);
    const __m128i c16_p35_p40 = _mm_set1_epi32(0x00230028);
    const __m128i c16_p21_p29 = _mm_set1_epi32(0x0015001D);
    const __m128i c16_p04_p13 = _mm_set1_epi32(0x0004000D);
    const __m128i c16_p29_p43 = _mm_set1_epi32(0x001D002B);
    const __m128i c16_n21_p04 = _mm_set1_epi32(0xFFEB0004);
    const __m128i c16_n45_n40 = _mm_set1_epi32(0xFFD3FFD8);
    const __m128i c16_n13_n35 = _mm_set1_epi32(0xFFF3FFDD);
    const __m128i c16_p04_p40 = _mm_set1_epi32(0x00040028);
    const __m128i c16_n43_n35 = _mm_set1_epi32(0xFFD5FFDD);
    const __m128i c16_p29_n13 = _mm_set1_epi32(0x001DFFF3);
    const __m128i c16_p21_p45 = _mm_set1_epi32(0x0015002D);
    const __m128i c16_n21_p35 = _mm_set1_epi32(0xFFEB0023);
    const __m128i c16_p04_n43 = _mm_set1_epi32(0x0004FFD5);
    const __m128i c16_p13_p45 = _mm_set1_epi32(0x000D002D);
    const __m128i c16_n29_n40 = _mm_set1_epi32(0xFFE3FFD8);
    const __m128i c16_n40_p29 = _mm_set1_epi32(0xFFD8001D);
    const __m128i c16_p45_n13 = _mm_set1_epi32(0x002DFFF3);
    const __m128i c16_n43_n04 = _mm_set1_epi32(0xFFD5FFFC);
    const __m128i c16_p35_p21 = _mm_set1_epi32(0x00230015);
    const __m128i c16_n45_p21 = _mm_set1_epi32(0xFFD30015);
    const __m128i c16_p13_p29 = _mm_set1_epi32(0x000D001D);
    const __m128i c16_p35_n43 = _mm_set1_epi32(0x0023FFD5);
    const __m128i c16_n40_p04 = _mm_set1_epi32(0xFFD80004);
    const __m128i c16_n35_p13 = _mm_set1_epi32(0xFFDD000D);
    const __m128i c16_n40_p45 = _mm_set1_epi32(0xFFD8002D);
    const __m128i c16_p04_p21 = _mm_set1_epi32(0x00040015);
    const __m128i c16_p43_n29 = _mm_set1_epi32(0x002BFFE3);
    const __m128i c16_n13_p04 = _mm_set1_epi32(0xFFF30004);
    const __m128i c16_n29_p21 = _mm_set1_epi32(0xFFE30015);
    const __m128i c16_n40_p35 = _mm_set1_epi32(0xFFD80023);
    //	const __m128i c16_n45_p43 = _mm_set1_epi32(0xFFD3002B);


    const __m128i c16_p38_p44 = _mm_set1_epi32(0x0026002C);
    const __m128i c16_p09_p25 = _mm_set1_epi32(0x00090019);
    const __m128i c16_n09_p38 = _mm_set1_epi32(0xFFF70026);
    const __m128i c16_n25_n44 = _mm_set1_epi32(0xFFE7FFD4);

    const __m128i c16_n44_p25 = _mm_set1_epi32(0xFFD40019);
    const __m128i c16_p38_p09 = _mm_set1_epi32(0x00260009);
    const __m128i c16_n25_p09 = _mm_set1_epi32(0xFFE70009);
    const __m128i c16_n44_p38 = _mm_set1_epi32(0xFFD40026);

    const __m128i c16_p17_p42 = _mm_set1_epi32(0x0011002A);
    const __m128i c16_n42_p17 = _mm_set1_epi32(0xFFD60011);

    const __m128i c16_p32_p32 = _mm_set1_epi32(0x00200020);
    const __m128i c16_n32_p32 = _mm_set1_epi32(0xFFE00020);

    __m128i c32_rnd = _mm_set1_epi32(16);
    __m128i zero = _mm_setzero_si128();

    int nShift = 5;
    int i, part;

    // DCT1
    __m128i in00[4], in01[4], in02[4], in03[4], in04[4], in05[4], in06[4], in07[4], in08[4], in09[4], in10[4], in11[4], in12[4], in13[4], in14[4], in15[4];
    __m128i in16[4], in17[4], in18[4], in19[4], in20[4], in21[4], in22[4], in23[4], in24[4], in25[4], in26[4], in27[4], in28[4], in29[4], in30[4], in31[4];
    __m128i res00[4], res01[4], res02[4], res03[4], res04[4], res05[4], res06[4], res07[4], res08[4], res09[4], res10[4], res11[4], res12[4], res13[4], res14[4], res15[4];
    __m128i res16[4], res17[4], res18[4], res19[4], res20[4], res21[4], res22[4], res23[4], res24[4], res25[4], res26[4], res27[4], res28[4], res29[4], res30[4], res31[4];

    //按行 64*64
    __m128i T00[8], T01[8], T02[8], T03[8], T04[8], T05[8], T06[8], T07[8], T08[8], T09[8], T10[8], T11[8], T12[8], T13[8], T14[8], T15[8], T16[8], T17[8], T18[8], T19[8], T20[8], T21[8], T22[8], T23[8], T24[8], T25[8], T26[8], T27[8], T28[8], T29[8], T30[8], T31[8], T32[8], T33[8], T34[8], T35[8], T36[8], T37[8], T38[8], T39[8], T40[8], T41[8], T42[8], T43[8], T44[8], T45[8], T46[8], T47[8], T48[8], T49[8], T50[8], T51[8], T52[8], T53[8], T54[8], T55[8], T56[8], T57[8], T58[8], T59[8], T60[8], T61[8], T62[8], T63[8];
	 
    //按列 16*64
    __m128i V00[8], V01[8], V02[8], V03[8], V04[8], V05[8], V06[8], V07[8], V08[8], V09[8], V10[8], V11[8], V12[8], V13[8], V14[8], V15[8], V16[8], V17[8], V18[8], V19[8], V20[8], V21[8], V22[8], V23[8], V24[8], V25[8], V26[8], V27[8], V28[8], V29[8], V30[8], V31[8], V32[8], V33[8], V34[8], V35[8], V36[8], V37[8], V38[8], V39[8], V40[8], V41[8], V42[8], V43[8], V44[8], V45[8], V46[8], V47[8], V48[8], V49[8], V50[8], V51[8], V52[8], V53[8], V54[8], V55[8], V56[8], V57[8], V58[8], V59[8], V60[8], V61[8], V62[8], V63[8];

    __m128i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
    __m128i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;

    for (i = 0; i < 2; i++)
    {
        const int offset = (i << 3);

        in00[i] = _mm_loadu_si128((const __m128i*)&src[0 * 32 + offset]);
        in01[i] = _mm_loadu_si128((const __m128i*)&src[1 * 32 + offset]);
        in02[i] = _mm_loadu_si128((const __m128i*)&src[2 * 32 + offset]);
        in03[i] = _mm_loadu_si128((const __m128i*)&src[3 * 32 + offset]);
        in04[i] = _mm_loadu_si128((const __m128i*)&src[4 * 32 + offset]);
        in05[i] = _mm_loadu_si128((const __m128i*)&src[5 * 32 + offset]);
        in06[i] = _mm_loadu_si128((const __m128i*)&src[6 * 32 + offset]);
        in07[i] = _mm_loadu_si128((const __m128i*)&src[7 * 32 + offset]);
        in08[i] = _mm_loadu_si128((const __m128i*)&src[8 * 32 + offset]);
        in09[i] = _mm_loadu_si128((const __m128i*)&src[9 * 32 + offset]);
        in10[i] = _mm_loadu_si128((const __m128i*)&src[10 * 32 + offset]);
        in11[i] = _mm_loadu_si128((const __m128i*)&src[11 * 32 + offset]);
        in12[i] = _mm_loadu_si128((const __m128i*)&src[12 * 32 + offset]);
        in13[i] = _mm_loadu_si128((const __m128i*)&src[13 * 32 + offset]);
        in14[i] = _mm_loadu_si128((const __m128i*)&src[14 * 32 + offset]);
        in15[i] = _mm_loadu_si128((const __m128i*)&src[15 * 32 + offset]);
    }

    for (part = 0; part < 2; part++)
    {
        const __m128i T_00_00A = _mm_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
        const __m128i T_00_00B = _mm_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
        const __m128i T_00_01A = _mm_unpacklo_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_01B = _mm_unpackhi_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_02A = _mm_unpacklo_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_02B = _mm_unpackhi_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_03A = _mm_unpacklo_epi16(in13[part], in15[part]);       // [ ]
        const __m128i T_00_03B = _mm_unpackhi_epi16(in13[part], in15[part]);       // [ ]

        const __m128i T_00_08A = _mm_unpacklo_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_08B = _mm_unpackhi_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_09A = _mm_unpacklo_epi16(in10[part], in14[part]);       // [ ]
        const __m128i T_00_09B = _mm_unpackhi_epi16(in10[part], in14[part]);       // [ ]

        const __m128i T_00_12A = _mm_unpacklo_epi16(in04[part], in12[part]);       // [ ]
        const __m128i T_00_12B = _mm_unpackhi_epi16(in04[part], in12[part]);       // [ ]

        const __m128i T_00_14A = _mm_unpacklo_epi16(in08[part], zero);       //
        const __m128i T_00_14B = _mm_unpackhi_epi16(in08[part], zero);       // [ ]
        const __m128i T_00_15A = _mm_unpacklo_epi16(in00[part], zero);       //
        const __m128i T_00_15B = _mm_unpackhi_epi16(in00[part], zero);       // [ ]

        __m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
        __m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
        __m128i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
        __m128i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
        {
            __m128i T1, T2, T3, T4;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, x1, x2, x3, x4, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
    T1 = _mm_add_epi32(_mm_madd_epi16(r0103, c0103), _mm_madd_epi16(r0507, c0507)); \
    T2 = _mm_add_epi32(_mm_madd_epi16(r0911, c0911), _mm_madd_epi16(r1315, c1315)); \
    T3 = _mm_add_epi32(_mm_madd_epi16(zero, c1719), _mm_madd_epi16(zero, c2123)); \
    T4 = _mm_add_epi32(_mm_madd_epi16(zero, c2527), _mm_madd_epi16(zero, c2931)); \
    row = _mm_add_epi32(_mm_add_epi32(T1, T2), _mm_add_epi32(T3, T4));

            COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A)

                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B)

#undef COMPUTE_ROW
        }


        {
            __m128i T1, T2;
#define COMPUTE_ROW(row0206, row1014, x1, x2, c0206, c1014, c1822, c2630, row) \
    T1 = _mm_add_epi32(_mm_madd_epi16(row0206, c0206), _mm_madd_epi16(row1014, c1014)); \
    T2 = _mm_add_epi32(_mm_madd_epi16(zero, c1822), _mm_madd_epi16(zero, c2630)); \
    row = _mm_add_epi32(T1, T2);

            COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A)

                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B)
#undef COMPUTE_ROW
        }
        {
            const __m128i EEO0A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));
            const __m128i EEO0B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));

            const __m128i EEEO0A = _mm_madd_epi16(T_00_14A, c16_p17_p42);
            const __m128i EEEO0B = _mm_madd_epi16(T_00_14B, c16_p17_p42);
            const __m128i EEEO1A = _mm_madd_epi16(T_00_14A, c16_n42_p17);
            const __m128i EEEO1B = _mm_madd_epi16(T_00_14B, c16_n42_p17);

            const __m128i EEEE0A = _mm_madd_epi16(T_00_15A, c16_p32_p32);
            const __m128i EEEE0B = _mm_madd_epi16(T_00_15B, c16_p32_p32);
            const __m128i EEEE1A = _mm_madd_epi16(T_00_15A, c16_n32_p32);
            const __m128i EEEE1B = _mm_madd_epi16(T_00_15B, c16_n32_p32);

            const __m128i EEE0A = _mm_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
            const __m128i EEE0B = _mm_add_epi32(EEEE0B, EEEO0B);
            const __m128i EEE1A = _mm_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
            const __m128i EEE1B = _mm_add_epi32(EEEE1B, EEEO1B);
            const __m128i EEE3A = _mm_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
            const __m128i EEE3B = _mm_sub_epi32(EEEE0B, EEEO0B);
            const __m128i EEE2A = _mm_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
            const __m128i EEE2B = _mm_sub_epi32(EEEE1B, EEEO1B);

            const __m128i EE0A = _mm_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
            const __m128i EE0B = _mm_add_epi32(EEE0B, EEO0B);
            const __m128i EE1A = _mm_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
            const __m128i EE1B = _mm_add_epi32(EEE1B, EEO1B);
            const __m128i EE2A = _mm_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
            const __m128i EE2B = _mm_add_epi32(EEE2B, EEO2B);
            const __m128i EE3A = _mm_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
            const __m128i EE3B = _mm_add_epi32(EEE3B, EEO3B);
            const __m128i EE7A = _mm_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
            const __m128i EE7B = _mm_sub_epi32(EEE0B, EEO0B);
            const __m128i EE6A = _mm_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
            const __m128i EE6B = _mm_sub_epi32(EEE1B, EEO1B);
            const __m128i EE5A = _mm_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
            const __m128i EE5B = _mm_sub_epi32(EEE2B, EEO2B);
            const __m128i EE4A = _mm_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
            const __m128i EE4B = _mm_sub_epi32(EEE3B, EEO3B);

            const __m128i E0A = _mm_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
            const __m128i E0B = _mm_add_epi32(EE0B, EO0B);
            const __m128i E1A = _mm_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
            const __m128i E1B = _mm_add_epi32(EE1B, EO1B);
            const __m128i E2A = _mm_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
            const __m128i E2B = _mm_add_epi32(EE2B, EO2B);
            const __m128i E3A = _mm_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
            const __m128i E3B = _mm_add_epi32(EE3B, EO3B);
            const __m128i E4A = _mm_add_epi32(EE4A, EO4A);          // E4 =
            const __m128i E4B = _mm_add_epi32(EE4B, EO4B);
            const __m128i E5A = _mm_add_epi32(EE5A, EO5A);          // E5 =
            const __m128i E5B = _mm_add_epi32(EE5B, EO5B);
            const __m128i E6A = _mm_add_epi32(EE6A, EO6A);          // E6 =
            const __m128i E6B = _mm_add_epi32(EE6B, EO6B);
            const __m128i E7A = _mm_add_epi32(EE7A, EO7A);          // E7 =
            const __m128i E7B = _mm_add_epi32(EE7B, EO7B);
            const __m128i EFA = _mm_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
            const __m128i EFB = _mm_sub_epi32(EE0B, EO0B);
            const __m128i EEA = _mm_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
            const __m128i EEB = _mm_sub_epi32(EE1B, EO1B);
            const __m128i EDA = _mm_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
            const __m128i EDB = _mm_sub_epi32(EE2B, EO2B);
            const __m128i ECA = _mm_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
            const __m128i ECB = _mm_sub_epi32(EE3B, EO3B);
            const __m128i EBA = _mm_sub_epi32(EE4A, EO4A);          // EB =
            const __m128i EBB = _mm_sub_epi32(EE4B, EO4B);
            const __m128i EAA = _mm_sub_epi32(EE5A, EO5A);          // EA =
            const __m128i EAB = _mm_sub_epi32(EE5B, EO5B);
            const __m128i E9A = _mm_sub_epi32(EE6A, EO6A);          // E9 =
            const __m128i E9B = _mm_sub_epi32(EE6B, EO6B);
            const __m128i E8A = _mm_sub_epi32(EE7A, EO7A);          // E8 =
            const __m128i E8B = _mm_sub_epi32(EE7B, EO7B);

            const __m128i T10A = _mm_add_epi32(E0A, c32_rnd);         // E0 + rnd
            const __m128i T10B = _mm_add_epi32(E0B, c32_rnd);
            const __m128i T11A = _mm_add_epi32(E1A, c32_rnd);         // E1 + rnd
            const __m128i T11B = _mm_add_epi32(E1B, c32_rnd);
            const __m128i T12A = _mm_add_epi32(E2A, c32_rnd);         // E2 + rnd
            const __m128i T12B = _mm_add_epi32(E2B, c32_rnd);
            const __m128i T13A = _mm_add_epi32(E3A, c32_rnd);         // E3 + rnd
            const __m128i T13B = _mm_add_epi32(E3B, c32_rnd);
            const __m128i T14A = _mm_add_epi32(E4A, c32_rnd);         // E4 + rnd
            const __m128i T14B = _mm_add_epi32(E4B, c32_rnd);
            const __m128i T15A = _mm_add_epi32(E5A, c32_rnd);         // E5 + rnd
            const __m128i T15B = _mm_add_epi32(E5B, c32_rnd);
            const __m128i T16A = _mm_add_epi32(E6A, c32_rnd);         // E6 + rnd
            const __m128i T16B = _mm_add_epi32(E6B, c32_rnd);
            const __m128i T17A = _mm_add_epi32(E7A, c32_rnd);         // E7 + rnd
            const __m128i T17B = _mm_add_epi32(E7B, c32_rnd);
            const __m128i T18A = _mm_add_epi32(E8A, c32_rnd);         // E8 + rnd
            const __m128i T18B = _mm_add_epi32(E8B, c32_rnd);
            const __m128i T19A = _mm_add_epi32(E9A, c32_rnd);         // E9 + rnd
            const __m128i T19B = _mm_add_epi32(E9B, c32_rnd);
            const __m128i T1AA = _mm_add_epi32(EAA, c32_rnd);         // E10 + rnd
            const __m128i T1AB = _mm_add_epi32(EAB, c32_rnd);
            const __m128i T1BA = _mm_add_epi32(EBA, c32_rnd);         // E11 + rnd
            const __m128i T1BB = _mm_add_epi32(EBB, c32_rnd);
            const __m128i T1CA = _mm_add_epi32(ECA, c32_rnd);         // E12 + rnd
            const __m128i T1CB = _mm_add_epi32(ECB, c32_rnd);
            const __m128i T1DA = _mm_add_epi32(EDA, c32_rnd);         // E13 + rnd
            const __m128i T1DB = _mm_add_epi32(EDB, c32_rnd);
            const __m128i T1EA = _mm_add_epi32(EEA, c32_rnd);         // E14 + rnd
            const __m128i T1EB = _mm_add_epi32(EEB, c32_rnd);
            const __m128i T1FA = _mm_add_epi32(EFA, c32_rnd);         // E15 + rnd
            const __m128i T1FB = _mm_add_epi32(EFB, c32_rnd);

            const __m128i T2_00A = _mm_add_epi32(T10A, O00A);          // E0 + O0 + rnd
            const __m128i T2_00B = _mm_add_epi32(T10B, O00B);
            const __m128i T2_01A = _mm_add_epi32(T11A, O01A);          // E1 + O1 + rnd
            const __m128i T2_01B = _mm_add_epi32(T11B, O01B);
            const __m128i T2_02A = _mm_add_epi32(T12A, O02A);          // E2 + O2 + rnd
            const __m128i T2_02B = _mm_add_epi32(T12B, O02B);
            const __m128i T2_03A = _mm_add_epi32(T13A, O03A);          // E3 + O3 + rnd
            const __m128i T2_03B = _mm_add_epi32(T13B, O03B);
            const __m128i T2_04A = _mm_add_epi32(T14A, O04A);          // E4
            const __m128i T2_04B = _mm_add_epi32(T14B, O04B);
            const __m128i T2_05A = _mm_add_epi32(T15A, O05A);          // E5
            const __m128i T2_05B = _mm_add_epi32(T15B, O05B);
            const __m128i T2_06A = _mm_add_epi32(T16A, O06A);          // E6
            const __m128i T2_06B = _mm_add_epi32(T16B, O06B);
            const __m128i T2_07A = _mm_add_epi32(T17A, O07A);          // E7
            const __m128i T2_07B = _mm_add_epi32(T17B, O07B);
            const __m128i T2_08A = _mm_add_epi32(T18A, O08A);          // E8
            const __m128i T2_08B = _mm_add_epi32(T18B, O08B);
            const __m128i T2_09A = _mm_add_epi32(T19A, O09A);          // E9
            const __m128i T2_09B = _mm_add_epi32(T19B, O09B);
            const __m128i T2_10A = _mm_add_epi32(T1AA, O10A);          // E10
            const __m128i T2_10B = _mm_add_epi32(T1AB, O10B);
            const __m128i T2_11A = _mm_add_epi32(T1BA, O11A);          // E11
            const __m128i T2_11B = _mm_add_epi32(T1BB, O11B);
            const __m128i T2_12A = _mm_add_epi32(T1CA, O12A);          // E12
            const __m128i T2_12B = _mm_add_epi32(T1CB, O12B);
            const __m128i T2_13A = _mm_add_epi32(T1DA, O13A);          // E13
            const __m128i T2_13B = _mm_add_epi32(T1DB, O13B);
            const __m128i T2_14A = _mm_add_epi32(T1EA, O14A);          // E14
            const __m128i T2_14B = _mm_add_epi32(T1EB, O14B);
            const __m128i T2_15A = _mm_add_epi32(T1FA, O15A);          // E15
            const __m128i T2_15B = _mm_add_epi32(T1FB, O15B);
            const __m128i T2_31A = _mm_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
            const __m128i T2_31B = _mm_sub_epi32(T10B, O00B);
            const __m128i T2_30A = _mm_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
            const __m128i T2_30B = _mm_sub_epi32(T11B, O01B);
            const __m128i T2_29A = _mm_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
            const __m128i T2_29B = _mm_sub_epi32(T12B, O02B);
            const __m128i T2_28A = _mm_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
            const __m128i T2_28B = _mm_sub_epi32(T13B, O03B);
            const __m128i T2_27A = _mm_sub_epi32(T14A, O04A);          // E4
            const __m128i T2_27B = _mm_sub_epi32(T14B, O04B);
            const __m128i T2_26A = _mm_sub_epi32(T15A, O05A);          // E5
            const __m128i T2_26B = _mm_sub_epi32(T15B, O05B);
            const __m128i T2_25A = _mm_sub_epi32(T16A, O06A);          // E6
            const __m128i T2_25B = _mm_sub_epi32(T16B, O06B);
            const __m128i T2_24A = _mm_sub_epi32(T17A, O07A);          // E7
            const __m128i T2_24B = _mm_sub_epi32(T17B, O07B);
            const __m128i T2_23A = _mm_sub_epi32(T18A, O08A);          //
            const __m128i T2_23B = _mm_sub_epi32(T18B, O08B);
            const __m128i T2_22A = _mm_sub_epi32(T19A, O09A);          //
            const __m128i T2_22B = _mm_sub_epi32(T19B, O09B);
            const __m128i T2_21A = _mm_sub_epi32(T1AA, O10A);          //
            const __m128i T2_21B = _mm_sub_epi32(T1AB, O10B);
            const __m128i T2_20A = _mm_sub_epi32(T1BA, O11A);          //
            const __m128i T2_20B = _mm_sub_epi32(T1BB, O11B);
            const __m128i T2_19A = _mm_sub_epi32(T1CA, O12A);          //
            const __m128i T2_19B = _mm_sub_epi32(T1CB, O12B);
            const __m128i T2_18A = _mm_sub_epi32(T1DA, O13A);          //
            const __m128i T2_18B = _mm_sub_epi32(T1DB, O13B);
            const __m128i T2_17A = _mm_sub_epi32(T1EA, O14A);          //
            const __m128i T2_17B = _mm_sub_epi32(T1EB, O14B);
            const __m128i T2_16A = _mm_sub_epi32(T1FA, O15A);          //
            const __m128i T2_16B = _mm_sub_epi32(T1FB, O15B);

            const __m128i T3_00A = _mm_srai_epi32(T2_00A, nShift);             // [30 20 10 00]
            const __m128i T3_00B = _mm_srai_epi32(T2_00B, nShift);             // [70 60 50 40]
            const __m128i T3_01A = _mm_srai_epi32(T2_01A, nShift);             // [31 21 11 01]
            const __m128i T3_01B = _mm_srai_epi32(T2_01B, nShift);             // [71 61 51 41]
            const __m128i T3_02A = _mm_srai_epi32(T2_02A, nShift);             // [32 22 12 02]
            const __m128i T3_02B = _mm_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
            const __m128i T3_03A = _mm_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
            const __m128i T3_03B = _mm_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
            const __m128i T3_04A = _mm_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
            const __m128i T3_04B = _mm_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
            const __m128i T3_05A = _mm_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
            const __m128i T3_05B = _mm_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
            const __m128i T3_06A = _mm_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
            const __m128i T3_06B = _mm_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
            const __m128i T3_07A = _mm_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
            const __m128i T3_07B = _mm_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
            const __m128i T3_08A = _mm_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
            const __m128i T3_08B = _mm_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
            const __m128i T3_09A = _mm_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
            const __m128i T3_09B = _mm_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
            const __m128i T3_10A = _mm_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
            const __m128i T3_10B = _mm_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
            const __m128i T3_11A = _mm_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
            const __m128i T3_11B = _mm_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
            const __m128i T3_12A = _mm_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
            const __m128i T3_12B = _mm_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
            const __m128i T3_13A = _mm_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
            const __m128i T3_13B = _mm_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
            const __m128i T3_14A = _mm_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
            const __m128i T3_14B = _mm_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
            const __m128i T3_15A = _mm_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
            const __m128i T3_15B = _mm_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

            const __m128i T3_16A = _mm_srai_epi32(T2_16A, nShift);             // [30 20 10 00]
            const __m128i T3_16B = _mm_srai_epi32(T2_16B, nShift);             // [70 60 50 40]
            const __m128i T3_17A = _mm_srai_epi32(T2_17A, nShift);             // [31 21 11 01]
            const __m128i T3_17B = _mm_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
            const __m128i T3_18A = _mm_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
            const __m128i T3_18B = _mm_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
            const __m128i T3_19A = _mm_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
            const __m128i T3_19B = _mm_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
            const __m128i T3_20A = _mm_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
            const __m128i T3_20B = _mm_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
            const __m128i T3_21A = _mm_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
            const __m128i T3_21B = _mm_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
            const __m128i T3_22A = _mm_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
            const __m128i T3_22B = _mm_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
            const __m128i T3_23A = _mm_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
            const __m128i T3_23B = _mm_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
            const __m128i T3_24A = _mm_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
            const __m128i T3_24B = _mm_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
            const __m128i T3_25A = _mm_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
            const __m128i T3_25B = _mm_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
            const __m128i T3_26A = _mm_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
            const __m128i T3_26B = _mm_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
            const __m128i T3_27A = _mm_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
            const __m128i T3_27B = _mm_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
            const __m128i T3_28A = _mm_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
            const __m128i T3_28B = _mm_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
            const __m128i T3_29A = _mm_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
            const __m128i T3_29B = _mm_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
            const __m128i T3_30A = _mm_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
            const __m128i T3_30B = _mm_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
            const __m128i T3_31A = _mm_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
            const __m128i T3_31B = _mm_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

            res00[part] = _mm_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
            res01[part] = _mm_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
            res02[part] = _mm_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
            res03[part] = _mm_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
            res04[part] = _mm_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
            res05[part] = _mm_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
            res06[part] = _mm_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
            res07[part] = _mm_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
            res08[part] = _mm_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
            res09[part] = _mm_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
            res10[part] = _mm_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
            res11[part] = _mm_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
            res12[part] = _mm_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
            res13[part] = _mm_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
            res14[part] = _mm_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
            res15[part] = _mm_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
            res16[part] = _mm_packs_epi32(T3_16A, T3_16B);
            res17[part] = _mm_packs_epi32(T3_17A, T3_17B);
            res18[part] = _mm_packs_epi32(T3_18A, T3_18B);
            res19[part] = _mm_packs_epi32(T3_19A, T3_19B);
            res20[part] = _mm_packs_epi32(T3_20A, T3_20B);
            res21[part] = _mm_packs_epi32(T3_21A, T3_21B);
            res22[part] = _mm_packs_epi32(T3_22A, T3_22B);
            res23[part] = _mm_packs_epi32(T3_23A, T3_23B);
            res24[part] = _mm_packs_epi32(T3_24A, T3_24B);
            res25[part] = _mm_packs_epi32(T3_25A, T3_25B);
            res26[part] = _mm_packs_epi32(T3_26A, T3_26B);
            res27[part] = _mm_packs_epi32(T3_27A, T3_27B);
            res28[part] = _mm_packs_epi32(T3_28A, T3_28B);
            res29[part] = _mm_packs_epi32(T3_29A, T3_29B);
            res30[part] = _mm_packs_epi32(T3_30A, T3_30B);
            res31[part] = _mm_packs_epi32(T3_31A, T3_31B);
        }
    }
    //transpose matrix 8x8 16bit.
    {
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


        TRANSPOSE_8x8_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0]);
        TRANSPOSE_8x8_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);

        TRANSPOSE_8x8_16BIT(res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1]);
        TRANSPOSE_8x8_16BIT(res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);

        TRANSPOSE_8x8_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2]);
        TRANSPOSE_8x8_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2]);

        TRANSPOSE_8x8_16BIT(res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[3], in01[3], in02[3], in03[3], in04[3], in05[3], in06[3], in07[3]);
        TRANSPOSE_8x8_16BIT(res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in08[3], in09[3], in10[3], in11[3], in12[3], in13[3], in14[3], in15[3]);

#undef TRANSPOSE_8x8_16BIT
    }

    c32_rnd = _mm_set1_epi32(1024);	
    nShift = 11;
   
    for (part = 0; part < 4; part++)
    {
        const __m128i T_00_00A = _mm_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
        const __m128i T_00_00B = _mm_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
        const __m128i T_00_01A = _mm_unpacklo_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_01B = _mm_unpackhi_epi16(in05[part], in07[part]);       // [ ]
        const __m128i T_00_02A = _mm_unpacklo_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_02B = _mm_unpackhi_epi16(in09[part], in11[part]);       // [ ]
        const __m128i T_00_03A = _mm_unpacklo_epi16(in13[part], in15[part]);       // [ ]
        const __m128i T_00_03B = _mm_unpackhi_epi16(in13[part], in15[part]);       // [ ]

        const __m128i T_00_08A = _mm_unpacklo_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_08B = _mm_unpackhi_epi16(in02[part], in06[part]);       // [ ]
        const __m128i T_00_09A = _mm_unpacklo_epi16(in10[part], in14[part]);       // [ ]
        const __m128i T_00_09B = _mm_unpackhi_epi16(in10[part], in14[part]);       // [ ]

        const __m128i T_00_12A = _mm_unpacklo_epi16(in04[part], in12[part]);       // [ ]
        const __m128i T_00_12B = _mm_unpackhi_epi16(in04[part], in12[part]);       // [ ]

        const __m128i T_00_14A = _mm_unpacklo_epi16(in08[part], zero);       //
        const __m128i T_00_14B = _mm_unpackhi_epi16(in08[part], zero);       // [ ]
        const __m128i T_00_15A = _mm_unpacklo_epi16(in00[part], zero);       //
        const __m128i T_00_15B = _mm_unpackhi_epi16(in00[part], zero);       // [ ]

        __m128i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
        __m128i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
        __m128i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
        __m128i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
        {
            __m128i T1, T2, T3, T4;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, x1, x2, x3, x4, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
    T1 = _mm_add_epi32(_mm_madd_epi16(r0103, c0103), _mm_madd_epi16(r0507, c0507)); \
    T2 = _mm_add_epi32(_mm_madd_epi16(r0911, c0911), _mm_madd_epi16(r1315, c1315)); \
    T3 = _mm_add_epi32(_mm_madd_epi16(zero, c1719), _mm_madd_epi16(zero, c2123)); \
    T4 = _mm_add_epi32(_mm_madd_epi16(zero, c2527), _mm_madd_epi16(zero, c2931)); \
    row = _mm_add_epi32(_mm_add_epi32(T1, T2), _mm_add_epi32(T3, T4));

            COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A)
                COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A)

                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B)
                COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
                c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B)

#undef COMPUTE_ROW
        }


        {
            __m128i T1, T2;
#define COMPUTE_ROW(row0206, row1014, x1, x2, c0206, c1014, c1822, c2630, row) \
    T1 = _mm_add_epi32(_mm_madd_epi16(row0206, c0206), _mm_madd_epi16(row1014, c1014)); \
    T2 = _mm_add_epi32(_mm_madd_epi16(zero, c1822), _mm_madd_epi16(zero, c2630)); \
    row = _mm_add_epi32(T1, T2);

            COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A)
                COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A)

                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B)
                COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B)
#undef COMPUTE_ROW
        }
        {
            const __m128i EEO0A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3A = _mm_add_epi32(_mm_madd_epi16(T_00_12A, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));
            const __m128i EEO0B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_p38_p44), _mm_madd_epi16(zero, c16_p09_p25));
            const __m128i EEO1B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n09_p38), _mm_madd_epi16(zero, c16_n25_n44));
            const __m128i EEO2B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n44_p25), _mm_madd_epi16(zero, c16_p38_p09));
            const __m128i EEO3B = _mm_add_epi32(_mm_madd_epi16(T_00_12B, c16_n25_p09), _mm_madd_epi16(zero, c16_n44_p38));

            const __m128i EEEO0A = _mm_madd_epi16(T_00_14A, c16_p17_p42);
            const __m128i EEEO0B = _mm_madd_epi16(T_00_14B, c16_p17_p42);
            const __m128i EEEO1A = _mm_madd_epi16(T_00_14A, c16_n42_p17);
            const __m128i EEEO1B = _mm_madd_epi16(T_00_14B, c16_n42_p17);

            const __m128i EEEE0A = _mm_madd_epi16(T_00_15A, c16_p32_p32);
            const __m128i EEEE0B = _mm_madd_epi16(T_00_15B, c16_p32_p32);
            const __m128i EEEE1A = _mm_madd_epi16(T_00_15A, c16_n32_p32);
            const __m128i EEEE1B = _mm_madd_epi16(T_00_15B, c16_n32_p32);

            const __m128i EEE0A = _mm_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
            const __m128i EEE0B = _mm_add_epi32(EEEE0B, EEEO0B);
            const __m128i EEE1A = _mm_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
            const __m128i EEE1B = _mm_add_epi32(EEEE1B, EEEO1B);
            const __m128i EEE3A = _mm_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
            const __m128i EEE3B = _mm_sub_epi32(EEEE0B, EEEO0B);
            const __m128i EEE2A = _mm_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
            const __m128i EEE2B = _mm_sub_epi32(EEEE1B, EEEO1B);

            const __m128i EE0A = _mm_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
            const __m128i EE0B = _mm_add_epi32(EEE0B, EEO0B);
            const __m128i EE1A = _mm_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
            const __m128i EE1B = _mm_add_epi32(EEE1B, EEO1B);
            const __m128i EE2A = _mm_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
            const __m128i EE2B = _mm_add_epi32(EEE2B, EEO2B);
            const __m128i EE3A = _mm_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
            const __m128i EE3B = _mm_add_epi32(EEE3B, EEO3B);
            const __m128i EE7A = _mm_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
            const __m128i EE7B = _mm_sub_epi32(EEE0B, EEO0B);
            const __m128i EE6A = _mm_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
            const __m128i EE6B = _mm_sub_epi32(EEE1B, EEO1B);
            const __m128i EE5A = _mm_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
            const __m128i EE5B = _mm_sub_epi32(EEE2B, EEO2B);
            const __m128i EE4A = _mm_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
            const __m128i EE4B = _mm_sub_epi32(EEE3B, EEO3B);

            const __m128i E0A = _mm_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
            const __m128i E0B = _mm_add_epi32(EE0B, EO0B);
            const __m128i E1A = _mm_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
            const __m128i E1B = _mm_add_epi32(EE1B, EO1B);
            const __m128i E2A = _mm_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
            const __m128i E2B = _mm_add_epi32(EE2B, EO2B);
            const __m128i E3A = _mm_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
            const __m128i E3B = _mm_add_epi32(EE3B, EO3B);
            const __m128i E4A = _mm_add_epi32(EE4A, EO4A);          // E4 =
            const __m128i E4B = _mm_add_epi32(EE4B, EO4B);
            const __m128i E5A = _mm_add_epi32(EE5A, EO5A);          // E5 =
            const __m128i E5B = _mm_add_epi32(EE5B, EO5B);
            const __m128i E6A = _mm_add_epi32(EE6A, EO6A);          // E6 =
            const __m128i E6B = _mm_add_epi32(EE6B, EO6B);
            const __m128i E7A = _mm_add_epi32(EE7A, EO7A);          // E7 =
            const __m128i E7B = _mm_add_epi32(EE7B, EO7B);
            const __m128i EFA = _mm_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
            const __m128i EFB = _mm_sub_epi32(EE0B, EO0B);
            const __m128i EEA = _mm_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
            const __m128i EEB = _mm_sub_epi32(EE1B, EO1B);
            const __m128i EDA = _mm_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
            const __m128i EDB = _mm_sub_epi32(EE2B, EO2B);
            const __m128i ECA = _mm_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
            const __m128i ECB = _mm_sub_epi32(EE3B, EO3B);
            const __m128i EBA = _mm_sub_epi32(EE4A, EO4A);          // EB =
            const __m128i EBB = _mm_sub_epi32(EE4B, EO4B);
            const __m128i EAA = _mm_sub_epi32(EE5A, EO5A);          // EA =
            const __m128i EAB = _mm_sub_epi32(EE5B, EO5B);
            const __m128i E9A = _mm_sub_epi32(EE6A, EO6A);          // E9 =
            const __m128i E9B = _mm_sub_epi32(EE6B, EO6B);
            const __m128i E8A = _mm_sub_epi32(EE7A, EO7A);          // E8 =
            const __m128i E8B = _mm_sub_epi32(EE7B, EO7B);

            const __m128i T10A = _mm_add_epi32(E0A, c32_rnd);         // E0 + rnd
            const __m128i T10B = _mm_add_epi32(E0B, c32_rnd);
            const __m128i T11A = _mm_add_epi32(E1A, c32_rnd);         // E1 + rnd
            const __m128i T11B = _mm_add_epi32(E1B, c32_rnd);
            const __m128i T12A = _mm_add_epi32(E2A, c32_rnd);         // E2 + rnd
            const __m128i T12B = _mm_add_epi32(E2B, c32_rnd);
            const __m128i T13A = _mm_add_epi32(E3A, c32_rnd);         // E3 + rnd
            const __m128i T13B = _mm_add_epi32(E3B, c32_rnd);
            const __m128i T14A = _mm_add_epi32(E4A, c32_rnd);         // E4 + rnd
            const __m128i T14B = _mm_add_epi32(E4B, c32_rnd);
            const __m128i T15A = _mm_add_epi32(E5A, c32_rnd);         // E5 + rnd
            const __m128i T15B = _mm_add_epi32(E5B, c32_rnd);
            const __m128i T16A = _mm_add_epi32(E6A, c32_rnd);         // E6 + rnd
            const __m128i T16B = _mm_add_epi32(E6B, c32_rnd);
            const __m128i T17A = _mm_add_epi32(E7A, c32_rnd);         // E7 + rnd
            const __m128i T17B = _mm_add_epi32(E7B, c32_rnd);
            const __m128i T18A = _mm_add_epi32(E8A, c32_rnd);         // E8 + rnd
            const __m128i T18B = _mm_add_epi32(E8B, c32_rnd);
            const __m128i T19A = _mm_add_epi32(E9A, c32_rnd);         // E9 + rnd
            const __m128i T19B = _mm_add_epi32(E9B, c32_rnd);
            const __m128i T1AA = _mm_add_epi32(EAA, c32_rnd);         // E10 + rnd
            const __m128i T1AB = _mm_add_epi32(EAB, c32_rnd);
            const __m128i T1BA = _mm_add_epi32(EBA, c32_rnd);         // E11 + rnd
            const __m128i T1BB = _mm_add_epi32(EBB, c32_rnd);
            const __m128i T1CA = _mm_add_epi32(ECA, c32_rnd);         // E12 + rnd
            const __m128i T1CB = _mm_add_epi32(ECB, c32_rnd);
            const __m128i T1DA = _mm_add_epi32(EDA, c32_rnd);         // E13 + rnd
            const __m128i T1DB = _mm_add_epi32(EDB, c32_rnd);
            const __m128i T1EA = _mm_add_epi32(EEA, c32_rnd);         // E14 + rnd
            const __m128i T1EB = _mm_add_epi32(EEB, c32_rnd);
            const __m128i T1FA = _mm_add_epi32(EFA, c32_rnd);         // E15 + rnd
            const __m128i T1FB = _mm_add_epi32(EFB, c32_rnd);

            const __m128i T2_00A = _mm_add_epi32(T10A, O00A);          // E0 + O0 + rnd
            const __m128i T2_00B = _mm_add_epi32(T10B, O00B);
            const __m128i T2_01A = _mm_add_epi32(T11A, O01A);          // E1 + O1 + rnd
            const __m128i T2_01B = _mm_add_epi32(T11B, O01B);
            const __m128i T2_02A = _mm_add_epi32(T12A, O02A);          // E2 + O2 + rnd
            const __m128i T2_02B = _mm_add_epi32(T12B, O02B);
            const __m128i T2_03A = _mm_add_epi32(T13A, O03A);          // E3 + O3 + rnd
            const __m128i T2_03B = _mm_add_epi32(T13B, O03B);
            const __m128i T2_04A = _mm_add_epi32(T14A, O04A);          // E4
            const __m128i T2_04B = _mm_add_epi32(T14B, O04B);
            const __m128i T2_05A = _mm_add_epi32(T15A, O05A);          // E5
            const __m128i T2_05B = _mm_add_epi32(T15B, O05B);
            const __m128i T2_06A = _mm_add_epi32(T16A, O06A);          // E6
            const __m128i T2_06B = _mm_add_epi32(T16B, O06B);
            const __m128i T2_07A = _mm_add_epi32(T17A, O07A);          // E7
            const __m128i T2_07B = _mm_add_epi32(T17B, O07B);
            const __m128i T2_08A = _mm_add_epi32(T18A, O08A);          // E8
            const __m128i T2_08B = _mm_add_epi32(T18B, O08B);
            const __m128i T2_09A = _mm_add_epi32(T19A, O09A);          // E9
            const __m128i T2_09B = _mm_add_epi32(T19B, O09B);
            const __m128i T2_10A = _mm_add_epi32(T1AA, O10A);          // E10
            const __m128i T2_10B = _mm_add_epi32(T1AB, O10B);
            const __m128i T2_11A = _mm_add_epi32(T1BA, O11A);          // E11
            const __m128i T2_11B = _mm_add_epi32(T1BB, O11B);
            const __m128i T2_12A = _mm_add_epi32(T1CA, O12A);          // E12
            const __m128i T2_12B = _mm_add_epi32(T1CB, O12B);
            const __m128i T2_13A = _mm_add_epi32(T1DA, O13A);          // E13
            const __m128i T2_13B = _mm_add_epi32(T1DB, O13B);
            const __m128i T2_14A = _mm_add_epi32(T1EA, O14A);          // E14
            const __m128i T2_14B = _mm_add_epi32(T1EB, O14B);
            const __m128i T2_15A = _mm_add_epi32(T1FA, O15A);          // E15
            const __m128i T2_15B = _mm_add_epi32(T1FB, O15B);
            const __m128i T2_31A = _mm_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
            const __m128i T2_31B = _mm_sub_epi32(T10B, O00B);
            const __m128i T2_30A = _mm_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
            const __m128i T2_30B = _mm_sub_epi32(T11B, O01B);
            const __m128i T2_29A = _mm_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
            const __m128i T2_29B = _mm_sub_epi32(T12B, O02B);
            const __m128i T2_28A = _mm_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
            const __m128i T2_28B = _mm_sub_epi32(T13B, O03B);
            const __m128i T2_27A = _mm_sub_epi32(T14A, O04A);          // E4
            const __m128i T2_27B = _mm_sub_epi32(T14B, O04B);
            const __m128i T2_26A = _mm_sub_epi32(T15A, O05A);          // E5
            const __m128i T2_26B = _mm_sub_epi32(T15B, O05B);
            const __m128i T2_25A = _mm_sub_epi32(T16A, O06A);          // E6
            const __m128i T2_25B = _mm_sub_epi32(T16B, O06B);
            const __m128i T2_24A = _mm_sub_epi32(T17A, O07A);          // E7
            const __m128i T2_24B = _mm_sub_epi32(T17B, O07B);
            const __m128i T2_23A = _mm_sub_epi32(T18A, O08A);          //
            const __m128i T2_23B = _mm_sub_epi32(T18B, O08B);
            const __m128i T2_22A = _mm_sub_epi32(T19A, O09A);          //
            const __m128i T2_22B = _mm_sub_epi32(T19B, O09B);
            const __m128i T2_21A = _mm_sub_epi32(T1AA, O10A);          //
            const __m128i T2_21B = _mm_sub_epi32(T1AB, O10B);
            const __m128i T2_20A = _mm_sub_epi32(T1BA, O11A);          //
            const __m128i T2_20B = _mm_sub_epi32(T1BB, O11B);
            const __m128i T2_19A = _mm_sub_epi32(T1CA, O12A);          //
            const __m128i T2_19B = _mm_sub_epi32(T1CB, O12B);
            const __m128i T2_18A = _mm_sub_epi32(T1DA, O13A);          //
            const __m128i T2_18B = _mm_sub_epi32(T1DB, O13B);
            const __m128i T2_17A = _mm_sub_epi32(T1EA, O14A);          //
            const __m128i T2_17B = _mm_sub_epi32(T1EB, O14B);
            const __m128i T2_16A = _mm_sub_epi32(T1FA, O15A);          //
            const __m128i T2_16B = _mm_sub_epi32(T1FB, O15B);

            const __m128i T3_00A = _mm_srai_epi32(T2_00A, nShift);             // [30 20 10 00]
            const __m128i T3_00B = _mm_srai_epi32(T2_00B, nShift);             // [70 60 50 40]
            const __m128i T3_01A = _mm_srai_epi32(T2_01A, nShift);             // [31 21 11 01]
            const __m128i T3_01B = _mm_srai_epi32(T2_01B, nShift);             // [71 61 51 41]
            const __m128i T3_02A = _mm_srai_epi32(T2_02A, nShift);             // [32 22 12 02]
            const __m128i T3_02B = _mm_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
            const __m128i T3_03A = _mm_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
            const __m128i T3_03B = _mm_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
            const __m128i T3_04A = _mm_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
            const __m128i T3_04B = _mm_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
            const __m128i T3_05A = _mm_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
            const __m128i T3_05B = _mm_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
            const __m128i T3_06A = _mm_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
            const __m128i T3_06B = _mm_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
            const __m128i T3_07A = _mm_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
            const __m128i T3_07B = _mm_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
            const __m128i T3_08A = _mm_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
            const __m128i T3_08B = _mm_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
            const __m128i T3_09A = _mm_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
            const __m128i T3_09B = _mm_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
            const __m128i T3_10A = _mm_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
            const __m128i T3_10B = _mm_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
            const __m128i T3_11A = _mm_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
            const __m128i T3_11B = _mm_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
            const __m128i T3_12A = _mm_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
            const __m128i T3_12B = _mm_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
            const __m128i T3_13A = _mm_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
            const __m128i T3_13B = _mm_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
            const __m128i T3_14A = _mm_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
            const __m128i T3_14B = _mm_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
            const __m128i T3_15A = _mm_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
            const __m128i T3_15B = _mm_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

            const __m128i T3_16A = _mm_srai_epi32(T2_16A, nShift);             // [30 20 10 00]
            const __m128i T3_16B = _mm_srai_epi32(T2_16B, nShift);             // [70 60 50 40]
            const __m128i T3_17A = _mm_srai_epi32(T2_17A, nShift);             // [31 21 11 01]
            const __m128i T3_17B = _mm_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
            const __m128i T3_18A = _mm_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
            const __m128i T3_18B = _mm_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
            const __m128i T3_19A = _mm_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
            const __m128i T3_19B = _mm_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
            const __m128i T3_20A = _mm_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
            const __m128i T3_20B = _mm_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
            const __m128i T3_21A = _mm_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
            const __m128i T3_21B = _mm_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
            const __m128i T3_22A = _mm_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
            const __m128i T3_22B = _mm_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
            const __m128i T3_23A = _mm_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
            const __m128i T3_23B = _mm_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
            const __m128i T3_24A = _mm_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
            const __m128i T3_24B = _mm_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
            const __m128i T3_25A = _mm_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
            const __m128i T3_25B = _mm_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
            const __m128i T3_26A = _mm_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
            const __m128i T3_26B = _mm_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
            const __m128i T3_27A = _mm_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
            const __m128i T3_27B = _mm_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
            const __m128i T3_28A = _mm_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
            const __m128i T3_28B = _mm_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
            const __m128i T3_29A = _mm_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
            const __m128i T3_29B = _mm_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
            const __m128i T3_30A = _mm_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
            const __m128i T3_30B = _mm_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
            const __m128i T3_31A = _mm_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
            const __m128i T3_31B = _mm_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

            res00[part] = _mm_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
            res01[part] = _mm_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
            res02[part] = _mm_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
            res03[part] = _mm_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
            res04[part] = _mm_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
            res05[part] = _mm_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
            res06[part] = _mm_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
            res07[part] = _mm_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
            res08[part] = _mm_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
            res09[part] = _mm_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
            res10[part] = _mm_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
            res11[part] = _mm_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
            res12[part] = _mm_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
            res13[part] = _mm_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
            res14[part] = _mm_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
            res15[part] = _mm_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
            res16[part] = _mm_packs_epi32(T3_16A, T3_16B);
            res17[part] = _mm_packs_epi32(T3_17A, T3_17B);
            res18[part] = _mm_packs_epi32(T3_18A, T3_18B);
            res19[part] = _mm_packs_epi32(T3_19A, T3_19B);
            res20[part] = _mm_packs_epi32(T3_20A, T3_20B);
            res21[part] = _mm_packs_epi32(T3_21A, T3_21B);
            res22[part] = _mm_packs_epi32(T3_22A, T3_22B);
            res23[part] = _mm_packs_epi32(T3_23A, T3_23B);
            res24[part] = _mm_packs_epi32(T3_24A, T3_24B);
            res25[part] = _mm_packs_epi32(T3_25A, T3_25B);
            res26[part] = _mm_packs_epi32(T3_26A, T3_26B);
            res27[part] = _mm_packs_epi32(T3_27A, T3_27B);
            res28[part] = _mm_packs_epi32(T3_28A, T3_28B);
            res29[part] = _mm_packs_epi32(T3_29A, T3_29B);
            res30[part] = _mm_packs_epi32(T3_30A, T3_30B);
            res31[part] = _mm_packs_epi32(T3_31A, T3_31B);
        }
    }
    //transpose matrix 8x8 16bit.
    {
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

        TRANSPOSE_8x8_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0]);
        TRANSPOSE_8x8_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
        TRANSPOSE_8x8_16BIT(res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0]);
        TRANSPOSE_8x8_16BIT(res00[3], res01[3], res02[3], res03[3], res04[3], res05[3], res06[3], res07[3], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0]);

        TRANSPOSE_8x8_16BIT(res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1]);
        TRANSPOSE_8x8_16BIT(res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);
        TRANSPOSE_8x8_16BIT(res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1]);
        TRANSPOSE_8x8_16BIT(res08[3], res09[3], res10[3], res11[3], res12[3], res13[3], res14[3], res15[3], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1]);

        TRANSPOSE_8x8_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2]);
        TRANSPOSE_8x8_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2]);
        TRANSPOSE_8x8_16BIT(res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2]);
        TRANSPOSE_8x8_16BIT(res16[3], res17[3], res18[3], res19[3], res20[3], res21[3], res22[3], res23[3], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2]);

        TRANSPOSE_8x8_16BIT(res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[3], in01[3], in02[3], in03[3], in04[3], in05[3], in06[3], in07[3]);
        TRANSPOSE_8x8_16BIT(res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in08[3], in09[3], in10[3], in11[3], in12[3], in13[3], in14[3], in15[3]);
        TRANSPOSE_8x8_16BIT(res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2], in16[3], in17[3], in18[3], in19[3], in20[3], in21[3], in22[3], in23[3]);
        TRANSPOSE_8x8_16BIT(res24[3], res25[3], res26[3], res27[3], res28[3], res29[3], res30[3], res31[3], in24[3], in25[3], in26[3], in27[3], in28[3], in29[3], in30[3], in31[3]);

#undef TRANSPOSE_8x8_16BIT
    }

    //clip
    {
        __m128i max_val = _mm_set1_epi16(511);
        __m128i min_val = _mm_set1_epi16(-512);
   
        for (k = 0; k < 4; k++)
        {
            in00[k] = _mm_min_epi16(in00[k], max_val);
            in00[k] = _mm_max_epi16(in00[k], min_val);
            in01[k] = _mm_min_epi16(in01[k], max_val);
            in01[k] = _mm_max_epi16(in01[k], min_val);
            in02[k] = _mm_min_epi16(in02[k], max_val);
            in02[k] = _mm_max_epi16(in02[k], min_val);
            in03[k] = _mm_min_epi16(in03[k], max_val);
            in03[k] = _mm_max_epi16(in03[k], min_val);
            in04[k] = _mm_min_epi16(in04[k], max_val);
            in04[k] = _mm_max_epi16(in04[k], min_val);
            in05[k] = _mm_min_epi16(in05[k], max_val);
            in05[k] = _mm_max_epi16(in05[k], min_val);
            in06[k] = _mm_min_epi16(in06[k], max_val);
            in06[k] = _mm_max_epi16(in06[k], min_val);
            in07[k] = _mm_min_epi16(in07[k], max_val);
            in07[k] = _mm_max_epi16(in07[k], min_val);
            in08[k] = _mm_min_epi16(in08[k], max_val);
            in08[k] = _mm_max_epi16(in08[k], min_val);
            in09[k] = _mm_min_epi16(in09[k], max_val);
            in09[k] = _mm_max_epi16(in09[k], min_val);
            in10[k] = _mm_min_epi16(in10[k], max_val);
            in10[k] = _mm_max_epi16(in10[k], min_val);
            in11[k] = _mm_min_epi16(in11[k], max_val);
            in11[k] = _mm_max_epi16(in11[k], min_val);
            in12[k] = _mm_min_epi16(in12[k], max_val);
            in12[k] = _mm_max_epi16(in12[k], min_val);
            in13[k] = _mm_min_epi16(in13[k], max_val);
            in13[k] = _mm_max_epi16(in13[k], min_val);
            in14[k] = _mm_min_epi16(in14[k], max_val);
            in14[k] = _mm_max_epi16(in14[k], min_val);
            in15[k] = _mm_min_epi16(in15[k], max_val);
            in15[k] = _mm_max_epi16(in15[k], min_val);
            in16[k] = _mm_min_epi16(in16[k], max_val);
            in16[k] = _mm_max_epi16(in16[k], min_val);
            in17[k] = _mm_min_epi16(in17[k], max_val);
            in17[k] = _mm_max_epi16(in17[k], min_val);
            in18[k] = _mm_min_epi16(in18[k], max_val);
            in18[k] = _mm_max_epi16(in18[k], min_val);
            in19[k] = _mm_min_epi16(in19[k], max_val);
            in19[k] = _mm_max_epi16(in19[k], min_val);
            in20[k] = _mm_min_epi16(in20[k], max_val);
            in20[k] = _mm_max_epi16(in20[k], min_val);
            in21[k] = _mm_min_epi16(in21[k], max_val);
            in21[k] = _mm_max_epi16(in21[k], min_val);
            in22[k] = _mm_min_epi16(in22[k], max_val);
            in22[k] = _mm_max_epi16(in22[k], min_val);
            in23[k] = _mm_min_epi16(in23[k], max_val);
            in23[k] = _mm_max_epi16(in23[k], min_val);
            in24[k] = _mm_min_epi16(in24[k], max_val);
            in24[k] = _mm_max_epi16(in24[k], min_val);
            in25[k] = _mm_min_epi16(in25[k], max_val);
            in25[k] = _mm_max_epi16(in25[k], min_val);
            in26[k] = _mm_min_epi16(in26[k], max_val);
            in26[k] = _mm_max_epi16(in26[k], min_val);
            in27[k] = _mm_min_epi16(in27[k], max_val);
            in27[k] = _mm_max_epi16(in27[k], min_val);
            in28[k] = _mm_min_epi16(in28[k], max_val);
            in28[k] = _mm_max_epi16(in28[k], min_val);
            in29[k] = _mm_min_epi16(in29[k], max_val);
            in29[k] = _mm_max_epi16(in29[k], min_val);
            in30[k] = _mm_min_epi16(in30[k], max_val);
            in30[k] = _mm_max_epi16(in30[k], min_val);
            in31[k] = _mm_min_epi16(in31[k], max_val);
            in31[k] = _mm_max_epi16(in31[k], min_val);
        }
    }

    // Add




    /*--vertical transform--*/
    //32*32, LOAD AND SHIFT
    for (i = 0; i < 4; i++){
        T00[i] = _mm_srai_epi16(in00[i], 1);
        T01[i] = _mm_srai_epi16(in01[i], 1);
        T02[i] = _mm_srai_epi16(in02[i], 1);
        T03[i] = _mm_srai_epi16(in03[i], 1);
        T04[i] = _mm_srai_epi16(in04[i], 1);
        T05[i] = _mm_srai_epi16(in05[i], 1);
        T06[i] = _mm_srai_epi16(in06[i], 1);
        T07[i] = _mm_srai_epi16(in07[i], 1);

        T08[i] = _mm_srai_epi16(in08[i], 1);
        T09[i] = _mm_srai_epi16(in09[i], 1);
        T10[i] = _mm_srai_epi16(in10[i], 1);
        T11[i] = _mm_srai_epi16(in11[i], 1);
        T12[i] = _mm_srai_epi16(in12[i], 1);
        T13[i] = _mm_srai_epi16(in13[i], 1);
        T14[i] = _mm_srai_epi16(in14[i], 1);
        T15[i] = _mm_srai_epi16(in15[i], 1);

        T16[i] = _mm_srai_epi16(in16[i], 1);
        T17[i] = _mm_srai_epi16(in17[i], 1);
        T18[i] = _mm_srai_epi16(in18[i], 1);
        T19[i] = _mm_srai_epi16(in19[i], 1);
        T20[i] = _mm_srai_epi16(in20[i], 1);
        T21[i] = _mm_srai_epi16(in21[i], 1);
        T22[i] = _mm_srai_epi16(in22[i], 1);
        T23[i] = _mm_srai_epi16(in23[i], 1);

        T24[i] = _mm_srai_epi16(in24[i], 1);
        T25[i] = _mm_srai_epi16(in25[i], 1);
        T26[i] = _mm_srai_epi16(in26[i], 1);
        T27[i] = _mm_srai_epi16(in27[i], 1);
        T28[i] = _mm_srai_epi16(in28[i], 1);
        T29[i] = _mm_srai_epi16(in29[i], 1);
        T30[i] = _mm_srai_epi16(in30[i], 1);
        T31[i] = _mm_srai_epi16(in31[i], 1);
    }

    //filter (odd pixel/row)
    for (i = 0; i < 4; i++){
        T32[i] = _mm_srai_epi16(_mm_add_epi16(T00[i], T01[i]), 1);
        T33[i] = _mm_srai_epi16(_mm_add_epi16(T01[i], T02[i]), 1);
        T34[i] = _mm_srai_epi16(_mm_add_epi16(T02[i], T03[i]), 1);
        T35[i] = _mm_srai_epi16(_mm_add_epi16(T03[i], T04[i]), 1);
        T36[i] = _mm_srai_epi16(_mm_add_epi16(T04[i], T05[i]), 1);
        T37[i] = _mm_srai_epi16(_mm_add_epi16(T05[i], T06[i]), 1);
        T38[i] = _mm_srai_epi16(_mm_add_epi16(T06[i], T07[i]), 1);
        T39[i] = _mm_srai_epi16(_mm_add_epi16(T07[i], T08[i]), 1);

        T40[i] = _mm_srai_epi16(_mm_add_epi16(T08[i], T09[i]), 1);
        T41[i] = _mm_srai_epi16(_mm_add_epi16(T09[i], T10[i]), 1);
        T42[i] = _mm_srai_epi16(_mm_add_epi16(T10[i], T11[i]), 1);
        T43[i] = _mm_srai_epi16(_mm_add_epi16(T11[i], T12[i]), 1);
        T44[i] = _mm_srai_epi16(_mm_add_epi16(T12[i], T13[i]), 1);
        T45[i] = _mm_srai_epi16(_mm_add_epi16(T13[i], T14[i]), 1);
        T46[i] = _mm_srai_epi16(_mm_add_epi16(T14[i], T15[i]), 1);
        T47[i] = _mm_srai_epi16(_mm_add_epi16(T15[i], T16[i]), 1);

        T48[i] = _mm_srai_epi16(_mm_add_epi16(T16[i], T17[i]), 1);
        T49[i] = _mm_srai_epi16(_mm_add_epi16(T17[i], T18[i]), 1);
        T50[i] = _mm_srai_epi16(_mm_add_epi16(T18[i], T19[i]), 1);
        T51[i] = _mm_srai_epi16(_mm_add_epi16(T19[i], T20[i]), 1);
        T52[i] = _mm_srai_epi16(_mm_add_epi16(T20[i], T21[i]), 1);
        T53[i] = _mm_srai_epi16(_mm_add_epi16(T21[i], T22[i]), 1);
        T54[i] = _mm_srai_epi16(_mm_add_epi16(T22[i], T23[i]), 1);
        T55[i] = _mm_srai_epi16(_mm_add_epi16(T23[i], T24[i]), 1);

        T56[i] = _mm_srai_epi16(_mm_add_epi16(T24[i], T25[i]), 1);
        T57[i] = _mm_srai_epi16(_mm_add_epi16(T25[i], T26[i]), 1);
        T58[i] = _mm_srai_epi16(_mm_add_epi16(T26[i], T27[i]), 1);
        T59[i] = _mm_srai_epi16(_mm_add_epi16(T27[i], T28[i]), 1);
        T60[i] = _mm_srai_epi16(_mm_add_epi16(T28[i], T29[i]), 1);
        T61[i] = _mm_srai_epi16(_mm_add_epi16(T29[i], T30[i]), 1);
        T62[i] = _mm_srai_epi16(_mm_add_epi16(T30[i], T31[i]), 1);
        T63[i] = _mm_srai_epi16(_mm_add_epi16(T31[i], T31[i]), 1);
    }

    /*--transposition--*/
    //32x64 -> 64x32

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

#define TRANSPOSE_16x16_16BIT(A0_0, A1_0, A2_0, A3_0, A4_0, A5_0, A6_0, A7_0, A8_0, A9_0, A10_0, A11_0, A12_0, A13_0, A14_0, A15_0, A0_1, A1_1, A2_1, A3_1, A4_1, A5_1, A6_1, A7_1, A8_1, A9_1, A10_1, A11_1, A12_1, A13_1, A14_1, A15_1, B0_0, B1_0, B2_0, B3_0, B4_0, B5_0, B6_0, B7_0, B8_0, B9_0, B10_0, B11_0, B12_0, B13_0, B14_0, B15_0, B0_1, B1_1, B2_1, B3_1, B4_1, B5_1, B6_1, B7_1, B8_1, B9_1, B10_1, B11_1, B12_1, B13_1, B14_1, B15_1) \
    TRANSPOSE_8x8_16BIT(A0_0, A1_0, A2_0, A3_0, A4_0, A5_0, A6_0, A7_0, B0_0, B1_0, B2_0, B3_0, B4_0, B5_0, B6_0, B7_0); \
    TRANSPOSE_8x8_16BIT(A8_0, A9_0, A10_0, A11_0, A12_0, A13_0, A14_0, A15_0, B0_1, B1_1, B2_1, B3_1, B4_1, B5_1, B6_1, B7_1); \
    TRANSPOSE_8x8_16BIT(A0_1, A1_1, A2_1, A3_1, A4_1, A5_1, A6_1, A7_1, B8_0, B9_0, B10_0, B11_0, B12_0, B13_0, B14_0, B15_0); \
    TRANSPOSE_8x8_16BIT(A8_1, A9_1, A10_1, A11_1, A12_1, A13_1, A14_1, A15_1, B8_1, B9_1, B10_1, B11_1, B12_1, B13_1, B14_1, B15_1); \





    TRANSPOSE_16x16_16BIT(
        T00[0], T32[0], T01[0], T33[0], T02[0], T34[0], T03[0], T35[0], T04[0], T36[0], T05[0], T37[0], T06[0], T38[0], T07[0], T39[0], T00[1], T32[1], T01[1], T33[1], T02[1], T34[1], T03[1], T35[1], T04[1], T36[1], T05[1], T37[1], T06[1], T38[1], T07[1], T39[1],
        V00[0], V01[0], V02[0], V03[0], V04[0], V05[0], V06[0], V07[0], V08[0], V09[0], V10[0], V11[0], V12[0], V13[0], V14[0], V15[0], V00[1], V01[1], V02[1], V03[1], V04[1], V05[1], V06[1], V07[1], V08[1], V09[1], V10[1], V11[1], V12[1], V13[1], V14[1], V15[1]);
    TRANSPOSE_16x16_16BIT(
        T00[2], T32[2], T01[2], T33[2], T02[2], T34[2], T03[2], T35[2], T04[2], T36[2], T05[2], T37[2], T06[2], T38[2], T07[2], T39[2], T00[3], T32[3], T01[3], T33[3], T02[3], T34[3], T03[3], T35[3], T04[3], T36[3], T05[3], T37[3], T06[3], T38[3], T07[3], T39[3],
        V16[0], V17[0], V18[0], V19[0], V20[0], V21[0], V22[0], V23[0], V24[0], V25[0], V26[0], V27[0], V28[0], V29[0], V30[0], V31[0], V16[1], V17[1], V18[1], V19[1], V20[1], V21[1], V22[1], V23[1], V24[1], V25[1], V26[1], V27[1], V28[1], V29[1], V30[1], V31[1]);

    TRANSPOSE_16x16_16BIT(
        T08[0], T40[0], T09[0], T41[0], T10[0], T42[0], T11[0], T43[0], T12[0], T44[0], T13[0], T45[0], T14[0], T46[0], T15[0], T47[0], T08[1], T40[1], T09[1], T41[1], T10[1], T42[1], T11[1], T43[1], T12[1], T44[1], T13[1], T45[1], T14[1], T46[1], T15[1], T47[1],
        V00[2], V01[2], V02[2], V03[2], V04[2], V05[2], V06[2], V07[2], V08[2], V09[2], V10[2], V11[2], V12[2], V13[2], V14[2], V15[2], V00[3], V01[3], V02[3], V03[3], V04[3], V05[3], V06[3], V07[3], V08[3], V09[3], V10[3], V11[3], V12[3], V13[3], V14[3], V15[3]);
    TRANSPOSE_16x16_16BIT(
        T08[2], T40[2], T09[2], T41[2], T10[2], T42[2], T11[2], T43[2], T12[2], T44[2], T13[2], T45[2], T14[2], T46[2], T15[2], T47[2], T08[3], T40[3], T09[3], T41[3], T10[3], T42[3], T11[3], T43[3], T12[3], T44[3], T13[3], T45[3], T14[3], T46[3], T15[3], T47[3],
        V16[2], V17[2], V18[2], V19[2], V20[2], V21[2], V22[2], V23[2], V24[2], V25[2], V26[2], V27[2], V28[2], V29[2], V30[2], V31[2], V16[3], V17[3], V18[3], V19[3], V20[3], V21[3], V22[3], V23[3], V24[3], V25[3], V26[3], V27[3], V28[3], V29[3], V30[3], V31[3]);

    TRANSPOSE_16x16_16BIT(
        T16[0], T48[0], T17[0], T49[0], T18[0], T50[0], T19[0], T51[0], T20[0], T52[0], T21[0], T53[0], T22[0], T54[0], T23[0], T55[0], T16[1], T48[1], T17[1], T49[1], T18[1], T50[1], T19[1], T51[1], T20[1], T52[1], T21[1], T53[1], T22[1], T54[1], T23[1], T55[1],
        V00[4], V01[4], V02[4], V03[4], V04[4], V05[4], V06[4], V07[4], V08[4], V09[4], V10[4], V11[4], V12[4], V13[4], V14[4], V15[4], V00[5], V01[5], V02[5], V03[5], V04[5], V05[5], V06[5], V07[5], V08[5], V09[5], V10[5], V11[5], V12[5], V13[5], V14[5], V15[5]);
    TRANSPOSE_16x16_16BIT(
        T16[2], T48[2], T17[2], T49[2], T18[2], T50[2], T19[2], T51[2], T20[2], T52[2], T21[2], T53[2], T22[2], T54[2], T23[2], T55[2], T16[3], T48[3], T17[3], T49[3], T18[3], T50[3], T19[3], T51[3], T20[3], T52[3], T21[3], T53[3], T22[3], T54[3], T23[3], T55[3],
        V16[4], V17[4], V18[4], V19[4], V20[4], V21[4], V22[4], V23[4], V24[4], V25[4], V26[4], V27[4], V28[4], V29[4], V30[4], V31[4], V16[5], V17[5], V18[5], V19[5], V20[5], V21[5], V22[5], V23[5], V24[5], V25[5], V26[5], V27[5], V28[5], V29[5], V30[5], V31[5]);

    TRANSPOSE_16x16_16BIT(
        T24[0], T56[0], T25[0], T57[0], T26[0], T58[0], T27[0], T59[0], T28[0], T60[0], T29[0], T61[0], T30[0], T62[0], T31[0], T63[0], T24[1], T56[1], T25[1], T57[1], T26[1], T58[1], T27[1], T59[1], T28[1], T60[1], T29[1], T61[1], T30[1], T62[1], T31[1], T63[1],
        V00[6], V01[6], V02[6], V03[6], V04[6], V05[6], V06[6], V07[6], V08[6], V09[6], V10[6], V11[6], V12[6], V13[6], V14[6], V15[6], V00[7], V01[7], V02[7], V03[7], V04[7], V05[7], V06[7], V07[7], V08[7], V09[7], V10[7], V11[7], V12[7], V13[7], V14[7], V15[7]);
    TRANSPOSE_16x16_16BIT(
        T24[2], T56[2], T25[2], T57[2], T26[2], T58[2], T27[2], T59[2], T28[2], T60[2], T29[2], T61[2], T30[2], T62[2], T31[2], T63[2], T24[3], T56[3], T25[3], T57[3], T26[3], T58[3], T27[3], T59[3], T28[3], T60[3], T29[3], T61[3], T30[3], T62[3], T31[3], T63[3],
        V16[6], V17[6], V18[6], V19[6], V20[6], V21[6], V22[6], V23[6], V24[6], V25[6], V26[6], V27[6], V28[6], V29[6], V30[6], V31[6], V16[7], V17[7], V18[7], V19[7], V20[7], V21[7], V22[7], V23[7], V24[7], V25[7], V26[7], V27[7], V28[7], V29[7], V30[7], V31[7]);

    /*--horizontal transform--*/
    //filter (odd pixel/column)
    for (i = 0; i < 8; i++){
        V32[i] = _mm_srai_epi16(_mm_add_epi16(V00[i], V01[i]), 1);
        V33[i] = _mm_srai_epi16(_mm_add_epi16(V01[i], V02[i]), 1);
        V34[i] = _mm_srai_epi16(_mm_add_epi16(V02[i], V03[i]), 1);
        V35[i] = _mm_srai_epi16(_mm_add_epi16(V03[i], V04[i]), 1);
        V36[i] = _mm_srai_epi16(_mm_add_epi16(V04[i], V05[i]), 1);
        V37[i] = _mm_srai_epi16(_mm_add_epi16(V05[i], V06[i]), 1);
        V38[i] = _mm_srai_epi16(_mm_add_epi16(V06[i], V07[i]), 1);
        V39[i] = _mm_srai_epi16(_mm_add_epi16(V07[i], V08[i]), 1);
        V40[i] = _mm_srai_epi16(_mm_add_epi16(V08[i], V09[i]), 1);
        V41[i] = _mm_srai_epi16(_mm_add_epi16(V09[i], V10[i]), 1);
        V42[i] = _mm_srai_epi16(_mm_add_epi16(V10[i], V11[i]), 1);
        V43[i] = _mm_srai_epi16(_mm_add_epi16(V11[i], V12[i]), 1);
        V44[i] = _mm_srai_epi16(_mm_add_epi16(V12[i], V13[i]), 1);
        V45[i] = _mm_srai_epi16(_mm_add_epi16(V13[i], V14[i]), 1);
        V46[i] = _mm_srai_epi16(_mm_add_epi16(V14[i], V15[i]), 1);
        V47[i] = _mm_srai_epi16(_mm_add_epi16(V15[i], V16[i]), 1);

        V48[i] = _mm_srai_epi16(_mm_add_epi16(V16[i], V17[i]), 1);
        V49[i] = _mm_srai_epi16(_mm_add_epi16(V17[i], V18[i]), 1);
        V50[i] = _mm_srai_epi16(_mm_add_epi16(V18[i], V19[i]), 1);
        V51[i] = _mm_srai_epi16(_mm_add_epi16(V19[i], V20[i]), 1);
        V52[i] = _mm_srai_epi16(_mm_add_epi16(V20[i], V21[i]), 1);
        V53[i] = _mm_srai_epi16(_mm_add_epi16(V21[i], V22[i]), 1);
        V54[i] = _mm_srai_epi16(_mm_add_epi16(V22[i], V23[i]), 1);
        V55[i] = _mm_srai_epi16(_mm_add_epi16(V23[i], V24[i]), 1);
        V56[i] = _mm_srai_epi16(_mm_add_epi16(V24[i], V25[i]), 1);
        V57[i] = _mm_srai_epi16(_mm_add_epi16(V25[i], V26[i]), 1);
        V58[i] = _mm_srai_epi16(_mm_add_epi16(V26[i], V27[i]), 1);
        V59[i] = _mm_srai_epi16(_mm_add_epi16(V27[i], V28[i]), 1);
        V60[i] = _mm_srai_epi16(_mm_add_epi16(V28[i], V29[i]), 1);
        V61[i] = _mm_srai_epi16(_mm_add_epi16(V29[i], V30[i]), 1);
        V62[i] = _mm_srai_epi16(_mm_add_epi16(V30[i], V31[i]), 1);
        V63[i] = _mm_srai_epi16(_mm_add_epi16(V31[i], V31[i]), 1);
    }

    /*--transposition & Store--*/
    //64x64 
    TRANSPOSE_16x16_16BIT(
        V00[0], V32[0], V01[0], V33[0], V02[0], V34[0], V03[0], V35[0], V04[0], V36[0], V05[0], V37[0], V06[0], V38[0], V07[0], V39[0], V00[1], V32[1], V01[1], V33[1], V02[1], V34[1], V03[1], V35[1], V04[1], V36[1], V05[1], V37[1], V06[1], V38[1], V07[1], V39[1],
        T00[0], T01[0], T02[0], T03[0], T04[0], T05[0], T06[0], T07[0], T08[0], T09[0], T10[0], T11[0], T12[0], T13[0], T14[0], T15[0], T00[1], T01[1], T02[1], T03[1], T04[1], T05[1], T06[1], T07[1], T08[1], T09[1], T10[1], T11[1], T12[1], T13[1], T14[1], T15[1]);
    TRANSPOSE_16x16_16BIT(
        V00[2], V32[2], V01[2], V33[2], V02[2], V34[2], V03[2], V35[2], V04[2], V36[2], V05[2], V37[2], V06[2], V38[2], V07[2], V39[2], V00[3], V32[3], V01[3], V33[3], V02[3], V34[3], V03[3], V35[3], V04[3], V36[3], V05[3], V37[3], V06[3], V38[3], V07[3], V39[3],
        T16[0], T17[0], T18[0], T19[0], T20[0], T21[0], T22[0], T23[0], T24[0], T25[0], T26[0], T27[0], T28[0], T29[0], T30[0], T31[0], T16[1], T17[1], T18[1], T19[1], T20[1], T21[1], T22[1], T23[1], T24[1], T25[1], T26[1], T27[1], T28[1], T29[1], T30[1], T31[1]);
    TRANSPOSE_16x16_16BIT(V00[4], V32[4], V01[4], V33[4], V02[4], V34[4], V03[4], V35[4], V04[4], V36[4], V05[4], V37[4], V06[4], V38[4], V07[4], V39[4], V00[5], V32[5], V01[5], V33[5], V02[5], V34[5], V03[5], V35[5], V04[5], V36[5], V05[5], V37[5], V06[5], V38[5], V07[5], V39[5], T32[0], T33[0], T34[0], T35[0], T36[0], T37[0], T38[0], T39[0], T40[0], T41[0], T42[0], T43[0], T44[0], T45[0], T46[0], T47[0], T32[1], T33[1], T34[1], T35[1], T36[1], T37[1], T38[1], T39[1], T40[1], T41[1], T42[1], T43[1], T44[1], T45[1], T46[1], T47[1]);
    TRANSPOSE_16x16_16BIT(V00[6], V32[6], V01[6], V33[6], V02[6], V34[6], V03[6], V35[6], V04[6], V36[6], V05[6], V37[6], V06[6], V38[6], V07[6], V39[6], V00[7], V32[7], V01[7], V33[7], V02[7], V34[7], V03[7], V35[7], V04[7], V36[7], V05[7], V37[7], V06[7], V38[7], V07[7], V39[7], T48[0], T49[0], T50[0], T51[0], T52[0], T53[0], T54[0], T55[0], T56[0], T57[0], T58[0], T59[0], T60[0], T61[0], T62[0], T63[0], T48[1], T49[1], T50[1], T51[1], T52[1], T53[1], T54[1], T55[1], T56[1], T57[1], T58[1], T59[1], T60[1], T61[1], T62[1], T63[1]);

    TRANSPOSE_16x16_16BIT(
        V08[0], V40[0], V09[0], V41[0], V10[0], V42[0], V11[0], V43[0], V12[0], V44[0], V13[0], V45[0], V14[0], V46[0], V15[0], V47[0], V08[1], V40[1], V09[1], V41[1], V10[1], V42[1], V11[1], V43[1], V12[1], V44[1], V13[1], V45[1], V14[1], V46[1], V15[1], V47[1],
        T00[2], T01[2], T02[2], T03[2], T04[2], T05[2], T06[2], T07[2], T08[2], T09[2], T10[2], T11[2], T12[2], T13[2], T14[2], T15[2], T00[3], T01[3], T02[3], T03[3], T04[3], T05[3], T06[3], T07[3], T08[3], T09[3], T10[3], T11[3], T12[3], T13[3], T14[3], T15[3]);
    TRANSPOSE_16x16_16BIT(
        V08[2], V40[2], V09[2], V41[2], V10[2], V42[2], V11[2], V43[2], V12[2], V44[2], V13[2], V45[2], V14[2], V46[2], V15[2], V47[2], V08[3], V40[3], V09[3], V41[3], V10[3], V42[3], V11[3], V43[3], V12[3], V44[3], V13[3], V45[3], V14[3], V46[3], V15[3], V47[3],
        T16[2], T17[2], T18[2], T19[2], T20[2], T21[2], T22[2], T23[2], T24[2], T25[2], T26[2], T27[2], T28[2], T29[2], T30[2], T31[2], T16[3], T17[3], T18[3], T19[3], T20[3], T21[3], T22[3], T23[3], T24[3], T25[3], T26[3], T27[3], T28[3], T29[3], T30[3], T31[3]);
    TRANSPOSE_16x16_16BIT(
        V08[4], V40[4], V09[4], V41[4], V10[4], V42[4], V11[4], V43[4], V12[4], V44[4], V13[4], V45[4], V14[4], V46[4], V15[4], V47[4], V08[5], V40[5], V09[5], V41[5], V10[5], V42[5], V11[5], V43[5], V12[5], V44[5], V13[5], V45[5], V14[5], V46[5], V15[5], V47[5],
        T32[2], T33[2], T34[2], T35[2], T36[2], T37[2], T38[2], T39[2], T40[2], T41[2], T42[2], T43[2], T44[2], T45[2], T46[2], T47[2], T32[3], T33[3], T34[3], T35[3], T36[3], T37[3], T38[3], T39[3], T40[3], T41[3], T42[3], T43[3], T44[3], T45[3], T46[3], T47[3]);
    TRANSPOSE_16x16_16BIT(
        V08[6], V40[6], V09[6], V41[6], V10[6], V42[6], V11[6], V43[6], V12[6], V44[6], V13[6], V45[6], V14[6], V46[6], V15[6], V47[6], V08[7], V40[7], V09[7], V41[7], V10[7], V42[7], V11[7], V43[7], V12[7], V44[7], V13[7], V45[7], V14[7], V46[7], V15[7], V47[7],
        T48[2], T49[2], T50[2], T51[2], T52[2], T53[2], T54[2], T55[2], T56[2], T57[2], T58[2], T59[2], T60[2], T61[2], T62[2], T63[2], T48[3], T49[3], T50[3], T51[3], T52[3], T53[3], T54[3], T55[3], T56[3], T57[3], T58[3], T59[3], T60[3], T61[3], T62[3], T63[3]);

    TRANSPOSE_16x16_16BIT(
        V16[0], V48[0], V17[0], V49[0], V18[0], V50[0], V19[0], V51[0], V20[0], V52[0], V21[0], V53[0], V22[0], V54[0], V23[0], V55[0], V16[1], V48[1], V17[1], V49[1], V18[1], V50[1], V19[1], V51[1], V20[1], V52[1], V21[1], V53[1], V22[1], V54[1], V23[1], V55[1],
        T00[4], T01[4], T02[4], T03[4], T04[4], T05[4], T06[4], T07[4], T08[4], T09[4], T10[4], T11[4], T12[4], T13[4], T14[4], T15[4], T00[5], T01[5], T02[5], T03[5], T04[5], T05[5], T06[5], T07[5], T08[5], T09[5], T10[5], T11[5], T12[5], T13[5], T14[5], T15[5]);
    TRANSPOSE_16x16_16BIT(
        V16[2], V48[2], V17[2], V49[2], V18[2], V50[2], V19[2], V51[2], V20[2], V52[2], V21[2], V53[2], V22[2], V54[2], V23[2], V55[2], V16[3], V48[3], V17[3], V49[3], V18[3], V50[3], V19[3], V51[3], V20[3], V52[3], V21[3], V53[3], V22[3], V54[3], V23[3], V55[3],
        T16[4], T17[4], T18[4], T19[4], T20[4], T21[4], T22[4], T23[4], T24[4], T25[4], T26[4], T27[4], T28[4], T29[4], T30[4], T31[4], T16[5], T17[5], T18[5], T19[5], T20[5], T21[5], T22[5], T23[5], T24[5], T25[5], T26[5], T27[5], T28[5], T29[5], T30[5], T31[5]);
    TRANSPOSE_16x16_16BIT(
        V16[4], V48[4], V17[4], V49[4], V18[4], V50[4], V19[4], V51[4], V20[4], V52[4], V21[4], V53[4], V22[4], V54[4], V23[4], V55[4], V16[5], V48[5], V17[5], V49[5], V18[5], V50[5], V19[5], V51[5], V20[5], V52[5], V21[5], V53[5], V22[5], V54[5], V23[5], V55[5],
        T32[4], T33[4], T34[4], T35[4], T36[4], T37[4], T38[4], T39[4], T40[4], T41[4], T42[4], T43[4], T44[4], T45[4], T46[4], T47[4], T32[5], T33[5], T34[5], T35[5], T36[5], T37[5], T38[5], T39[5], T40[5], T41[5], T42[5], T43[5], T44[5], T45[5], T46[5], T47[5]);
    TRANSPOSE_16x16_16BIT(
        V16[6], V48[6], V17[6], V49[6], V18[6], V50[6], V19[6], V51[6], V20[6], V52[6], V21[6], V53[6], V22[6], V54[6], V23[6], V55[6], V16[7], V48[7], V17[7], V49[7], V18[7], V50[7], V19[7], V51[7], V20[7], V52[7], V21[7], V53[7], V22[7], V54[7], V23[7], V55[7],
        T48[4], T49[4], T50[4], T51[4], T52[4], T53[4], T54[4], T55[4], T56[4], T57[4], T58[4], T59[4], T60[4], T61[4], T62[4], T63[4], T48[5], T49[5], T50[5], T51[5], T52[5], T53[5], T54[5], T55[5], T56[5], T57[5], T58[5], T59[5], T60[5], T61[5], T62[5], T63[5]);

    TRANSPOSE_16x16_16BIT(
        V24[0], V56[0], V25[0], V57[0], V26[0], V58[0], V27[0], V59[0], V28[0], V60[0], V29[0], V61[0], V30[0], V62[0], V31[0], V63[0], V24[1], V56[1], V25[1], V57[1], V26[1], V58[1], V27[1], V59[1], V28[1], V60[1], V29[1], V61[1], V30[1], V62[1], V31[1], V63[1],
        T00[6], T01[6], T02[6], T03[6], T04[6], T05[6], T06[6], T07[6], T08[6], T09[6], T10[6], T11[6], T12[6], T13[6], T14[6], T15[6], T00[7], T01[7], T02[7], T03[7], T04[7], T05[7], T06[7], T07[7], T08[7], T09[7], T10[7], T11[7], T12[7], T13[7], T14[7], T15[7]);
    TRANSPOSE_16x16_16BIT(
        V24[2], V56[2], V25[2], V57[2], V26[2], V58[2], V27[2], V59[2], V28[2], V60[2], V29[2], V61[2], V30[2], V62[2], V31[2], V63[2], V24[3], V56[3], V25[3], V57[3], V26[3], V58[3], V27[3], V59[3], V28[3], V60[3], V29[3], V61[3], V30[3], V62[3], V31[3], V63[3],
        T16[6], T17[6], T18[6], T19[6], T20[6], T21[6], T22[6], T23[6], T24[6], T25[6], T26[6], T27[6], T28[6], T29[6], T30[6], T31[6], T16[7], T17[7], T18[7], T19[7], T20[7], T21[7], T22[7], T23[7], T24[7], T25[7], T26[7], T27[7], T28[7], T29[7], T30[7], T31[7]);
    TRANSPOSE_16x16_16BIT(
        V24[4], V56[4], V25[4], V57[4], V26[4], V58[4], V27[4], V59[4], V28[4], V60[4], V29[4], V61[4], V30[4], V62[4], V31[4], V63[4], V24[5], V56[5], V25[5], V57[5], V26[5], V58[5], V27[5], V59[5], V28[5], V60[5], V29[5], V61[5], V30[5], V62[5], V31[5], V63[5],
        T32[6], T33[6], T34[6], T35[6], T36[6], T37[6], T38[6], T39[6], T40[6], T41[6], T42[6], T43[6], T44[6], T45[6], T46[6], T47[6], T32[7], T33[7], T34[7], T35[7], T36[7], T37[7], T38[7], T39[7], T40[7], T41[7], T42[7], T43[7], T44[7], T45[7], T46[7], T47[7]);
    TRANSPOSE_16x16_16BIT(
        V24[6], V56[6], V25[6], V57[6], V26[6], V58[6], V27[6], V59[6], V28[6], V60[6], V29[6], V61[6], V30[6], V62[6], V31[6], V63[6], V24[7], V56[7], V25[7], V57[7], V26[7], V58[7], V27[7], V59[7], V28[7], V60[7], V29[7], V61[7], V30[7], V62[7], V31[7], V63[7],
        T48[6], T49[6], T50[6], T51[6], T52[6], T53[6], T54[6], T55[6], T56[6], T57[6], T58[6], T59[6], T60[6], T61[6], T62[6], T63[6], T48[7], T49[7], T50[7], T51[7], T52[7], T53[7], T54[7], T55[7], T56[7], T57[7], T58[7], T59[7], T60[7], T61[7], T62[7], T63[7]);


#undef TRANSPOSE_8x8_16BIT
#undef TRANSPOSE_16x16_16BIT


    // Add



    for (k = 0; k < 4; k++)
    {
        int offset = (k << 4);
        P00[k] = _mm_loadu_si128((const __m128i*)&pred[0 + offset]);
        P01[k] = _mm_loadu_si128((const __m128i*)&pred[1 * i_pred + offset]);
        P02[k] = _mm_loadu_si128((const __m128i*)&pred[2 * i_pred + offset]);
        P03[k] = _mm_loadu_si128((const __m128i*)&pred[3 * i_pred + offset]);
        P04[k] = _mm_loadu_si128((const __m128i*)&pred[4 * i_pred + offset]);
        P05[k] = _mm_loadu_si128((const __m128i*)&pred[5 * i_pred + offset]);
        P06[k] = _mm_loadu_si128((const __m128i*)&pred[6 * i_pred + offset]);
        P07[k] = _mm_loadu_si128((const __m128i*)&pred[7 * i_pred + offset]);
        P08[k] = _mm_loadu_si128((const __m128i*)&pred[8 * i_pred + offset]);
        P09[k] = _mm_loadu_si128((const __m128i*)&pred[9 * i_pred + offset]);
        P10[k] = _mm_loadu_si128((const __m128i*)&pred[10 * i_pred + offset]);
        P11[k] = _mm_loadu_si128((const __m128i*)&pred[11 * i_pred + offset]);
        P12[k] = _mm_loadu_si128((const __m128i*)&pred[12 * i_pred + offset]);
        P13[k] = _mm_loadu_si128((const __m128i*)&pred[13 * i_pred + offset]);
        P14[k] = _mm_loadu_si128((const __m128i*)&pred[14 * i_pred + offset]);
        P15[k] = _mm_loadu_si128((const __m128i*)&pred[15 * i_pred + offset]);
        P16[k] = _mm_loadu_si128((const __m128i*)&pred[16 * i_pred + offset]);
        P17[k] = _mm_loadu_si128((const __m128i*)&pred[17 * i_pred + offset]);
        P18[k] = _mm_loadu_si128((const __m128i*)&pred[18 * i_pred + offset]);
        P19[k] = _mm_loadu_si128((const __m128i*)&pred[19 * i_pred + offset]);
        P20[k] = _mm_loadu_si128((const __m128i*)&pred[20 * i_pred + offset]);
        P21[k] = _mm_loadu_si128((const __m128i*)&pred[21 * i_pred + offset]);
        P22[k] = _mm_loadu_si128((const __m128i*)&pred[22 * i_pred + offset]);
        P23[k] = _mm_loadu_si128((const __m128i*)&pred[23 * i_pred + offset]);
        P24[k] = _mm_loadu_si128((const __m128i*)&pred[24 * i_pred + offset]);
        P25[k] = _mm_loadu_si128((const __m128i*)&pred[25 * i_pred + offset]);
        P26[k] = _mm_loadu_si128((const __m128i*)&pred[26 * i_pred + offset]);
        P27[k] = _mm_loadu_si128((const __m128i*)&pred[27 * i_pred + offset]);
        P28[k] = _mm_loadu_si128((const __m128i*)&pred[28 * i_pred + offset]);
        P29[k] = _mm_loadu_si128((const __m128i*)&pred[29 * i_pred + offset]);
        P30[k] = _mm_loadu_si128((const __m128i*)&pred[30 * i_pred + offset]);
        P31[k] = _mm_loadu_si128((const __m128i*)&pred[31 * i_pred + offset]);
        P32[k] = _mm_loadu_si128((const __m128i*)&pred[32 * i_pred + offset]);
        P33[k] = _mm_loadu_si128((const __m128i*)&pred[33 * i_pred + offset]);
        P34[k] = _mm_loadu_si128((const __m128i*)&pred[34 * i_pred + offset]);
        P35[k] = _mm_loadu_si128((const __m128i*)&pred[35 * i_pred + offset]);
        P36[k] = _mm_loadu_si128((const __m128i*)&pred[36 * i_pred + offset]);
        P37[k] = _mm_loadu_si128((const __m128i*)&pred[37 * i_pred + offset]);
        P38[k] = _mm_loadu_si128((const __m128i*)&pred[38 * i_pred + offset]);
        P39[k] = _mm_loadu_si128((const __m128i*)&pred[39 * i_pred + offset]);
        P40[k] = _mm_loadu_si128((const __m128i*)&pred[40 * i_pred + offset]);
        P41[k] = _mm_loadu_si128((const __m128i*)&pred[41 * i_pred + offset]);
        P42[k] = _mm_loadu_si128((const __m128i*)&pred[42 * i_pred + offset]);
        P43[k] = _mm_loadu_si128((const __m128i*)&pred[43 * i_pred + offset]);
        P44[k] = _mm_loadu_si128((const __m128i*)&pred[44 * i_pred + offset]);
        P45[k] = _mm_loadu_si128((const __m128i*)&pred[45 * i_pred + offset]);
        P46[k] = _mm_loadu_si128((const __m128i*)&pred[46 * i_pred + offset]);
        P47[k] = _mm_loadu_si128((const __m128i*)&pred[47 * i_pred + offset]);
        P48[k] = _mm_loadu_si128((const __m128i*)&pred[48 * i_pred + offset]);
        P49[k] = _mm_loadu_si128((const __m128i*)&pred[49 * i_pred + offset]);
        P50[k] = _mm_loadu_si128((const __m128i*)&pred[50 * i_pred + offset]);
        P51[k] = _mm_loadu_si128((const __m128i*)&pred[51 * i_pred + offset]);
        P52[k] = _mm_loadu_si128((const __m128i*)&pred[52 * i_pred + offset]);
        P53[k] = _mm_loadu_si128((const __m128i*)&pred[53 * i_pred + offset]);
        P54[k] = _mm_loadu_si128((const __m128i*)&pred[54 * i_pred + offset]);
        P55[k] = _mm_loadu_si128((const __m128i*)&pred[55 * i_pred + offset]);
        P56[k] = _mm_loadu_si128((const __m128i*)&pred[56 * i_pred + offset]);
        P57[k] = _mm_loadu_si128((const __m128i*)&pred[57 * i_pred + offset]);
        P58[k] = _mm_loadu_si128((const __m128i*)&pred[58 * i_pred + offset]);
        P59[k] = _mm_loadu_si128((const __m128i*)&pred[59 * i_pred + offset]);
        P60[k] = _mm_loadu_si128((const __m128i*)&pred[60 * i_pred + offset]);
        P61[k] = _mm_loadu_si128((const __m128i*)&pred[61 * i_pred + offset]);
        P62[k] = _mm_loadu_si128((const __m128i*)&pred[62 * i_pred + offset]);
        P63[k] = _mm_loadu_si128((const __m128i*)&pred[63 * i_pred + offset]);
    }

    for (k = 0; k < 4; k++)
    {
        int offset = k << 1;
        V00[0 + offset] = _mm_unpacklo_epi8(P00[k], zero);
        V00[1 + offset] = _mm_unpackhi_epi8(P00[k], zero);
        V01[0 + offset] = _mm_unpacklo_epi8(P01[k], zero);
        V01[1 + offset] = _mm_unpackhi_epi8(P01[k], zero);
        V02[0 + offset] = _mm_unpacklo_epi8(P02[k], zero);
        V02[1 + offset] = _mm_unpackhi_epi8(P02[k], zero);
        V03[0 + offset] = _mm_unpacklo_epi8(P03[k], zero);
        V03[1 + offset] = _mm_unpackhi_epi8(P03[k], zero);
        V04[0 + offset] = _mm_unpacklo_epi8(P04[k], zero);
        V04[1 + offset] = _mm_unpackhi_epi8(P04[k], zero);
        V05[0 + offset] = _mm_unpacklo_epi8(P05[k], zero);
        V05[1 + offset] = _mm_unpackhi_epi8(P05[k], zero);
        V06[0 + offset] = _mm_unpacklo_epi8(P06[k], zero);
        V06[1 + offset] = _mm_unpackhi_epi8(P06[k], zero);
        V07[0 + offset] = _mm_unpacklo_epi8(P07[k], zero);
        V07[1 + offset] = _mm_unpackhi_epi8(P07[k], zero);
        V08[0 + offset] = _mm_unpacklo_epi8(P08[k], zero);
        V08[1 + offset] = _mm_unpackhi_epi8(P08[k], zero);
        V09[0 + offset] = _mm_unpacklo_epi8(P09[k], zero);
        V09[1 + offset] = _mm_unpackhi_epi8(P09[k], zero);
        V10[0 + offset] = _mm_unpacklo_epi8(P10[k], zero);
        V10[1 + offset] = _mm_unpackhi_epi8(P10[k], zero);
        V11[0 + offset] = _mm_unpacklo_epi8(P11[k], zero);
        V11[1 + offset] = _mm_unpackhi_epi8(P11[k], zero);
        V12[0 + offset] = _mm_unpacklo_epi8(P12[k], zero);
        V12[1 + offset] = _mm_unpackhi_epi8(P12[k], zero);
        V13[0 + offset] = _mm_unpacklo_epi8(P13[k], zero);
        V13[1 + offset] = _mm_unpackhi_epi8(P13[k], zero);
        V14[0 + offset] = _mm_unpacklo_epi8(P14[k], zero);
        V14[1 + offset] = _mm_unpackhi_epi8(P14[k], zero);
        V15[0 + offset] = _mm_unpacklo_epi8(P15[k], zero);
        V15[1 + offset] = _mm_unpackhi_epi8(P15[k], zero);
        V16[0 + offset] = _mm_unpacklo_epi8(P16[k], zero);
        V16[1 + offset] = _mm_unpackhi_epi8(P16[k], zero);
        V17[0 + offset] = _mm_unpacklo_epi8(P17[k], zero);
        V17[1 + offset] = _mm_unpackhi_epi8(P17[k], zero);
        V18[0 + offset] = _mm_unpacklo_epi8(P18[k], zero);
        V18[1 + offset] = _mm_unpackhi_epi8(P18[k], zero);
        V19[0 + offset] = _mm_unpacklo_epi8(P19[k], zero);
        V19[1 + offset] = _mm_unpackhi_epi8(P19[k], zero);
        V20[0 + offset] = _mm_unpacklo_epi8(P20[k], zero);
        V20[1 + offset] = _mm_unpackhi_epi8(P20[k], zero);
        V21[0 + offset] = _mm_unpacklo_epi8(P21[k], zero);
        V21[1 + offset] = _mm_unpackhi_epi8(P21[k], zero);
        V22[0 + offset] = _mm_unpacklo_epi8(P22[k], zero);
        V22[1 + offset] = _mm_unpackhi_epi8(P22[k], zero);
        V23[0 + offset] = _mm_unpacklo_epi8(P23[k], zero);
        V23[1 + offset] = _mm_unpackhi_epi8(P23[k], zero);
        V24[0 + offset] = _mm_unpacklo_epi8(P24[k], zero);
        V24[1 + offset] = _mm_unpackhi_epi8(P24[k], zero);
        V25[0 + offset] = _mm_unpacklo_epi8(P25[k], zero);
        V25[1 + offset] = _mm_unpackhi_epi8(P25[k], zero);
        V26[0 + offset] = _mm_unpacklo_epi8(P26[k], zero);
        V26[1 + offset] = _mm_unpackhi_epi8(P26[k], zero);
        V27[0 + offset] = _mm_unpacklo_epi8(P27[k], zero);
        V27[1 + offset] = _mm_unpackhi_epi8(P27[k], zero);
        V28[0 + offset] = _mm_unpacklo_epi8(P28[k], zero);
        V28[1 + offset] = _mm_unpackhi_epi8(P28[k], zero);
        V29[0 + offset] = _mm_unpacklo_epi8(P29[k], zero);
        V29[1 + offset] = _mm_unpackhi_epi8(P29[k], zero);


        V30[0 + offset] = _mm_unpacklo_epi8(P30[k], zero);
        V30[1 + offset] = _mm_unpackhi_epi8(P30[k], zero);
        V31[0 + offset] = _mm_unpacklo_epi8(P31[k], zero);
        V31[1 + offset] = _mm_unpackhi_epi8(P31[k], zero);
        V32[0 + offset] = _mm_unpacklo_epi8(P32[k], zero);
        V32[1 + offset] = _mm_unpackhi_epi8(P32[k], zero);
        V33[0 + offset] = _mm_unpacklo_epi8(P33[k], zero);
        V33[1 + offset] = _mm_unpackhi_epi8(P33[k], zero);
        V34[0 + offset] = _mm_unpacklo_epi8(P34[k], zero);
        V34[1 + offset] = _mm_unpackhi_epi8(P34[k], zero);
        V35[0 + offset] = _mm_unpacklo_epi8(P35[k], zero);
        V35[1 + offset] = _mm_unpackhi_epi8(P35[k], zero);
        V36[0 + offset] = _mm_unpacklo_epi8(P36[k], zero);
        V36[1 + offset] = _mm_unpackhi_epi8(P36[k], zero);
        V37[0 + offset] = _mm_unpacklo_epi8(P37[k], zero);
        V37[1 + offset] = _mm_unpackhi_epi8(P37[k], zero);
        V38[0 + offset] = _mm_unpacklo_epi8(P38[k], zero);
        V38[1 + offset] = _mm_unpackhi_epi8(P38[k], zero);
        V39[0 + offset] = _mm_unpacklo_epi8(P39[k], zero);
        V39[1 + offset] = _mm_unpackhi_epi8(P39[k], zero);


        V40[0 + offset] = _mm_unpacklo_epi8(P40[k], zero);
        V40[1 + offset] = _mm_unpackhi_epi8(P40[k], zero);
        V41[0 + offset] = _mm_unpacklo_epi8(P41[k], zero);
        V41[1 + offset] = _mm_unpackhi_epi8(P41[k], zero);
        V42[0 + offset] = _mm_unpacklo_epi8(P42[k], zero);
        V42[1 + offset] = _mm_unpackhi_epi8(P42[k], zero);
        V43[0 + offset] = _mm_unpacklo_epi8(P43[k], zero);
        V43[1 + offset] = _mm_unpackhi_epi8(P43[k], zero);
        V44[0 + offset] = _mm_unpacklo_epi8(P44[k], zero);
        V44[1 + offset] = _mm_unpackhi_epi8(P44[k], zero);
        V45[0 + offset] = _mm_unpacklo_epi8(P45[k], zero);
        V45[1 + offset] = _mm_unpackhi_epi8(P45[k], zero);
        V46[0 + offset] = _mm_unpacklo_epi8(P46[k], zero);
        V46[1 + offset] = _mm_unpackhi_epi8(P46[k], zero);
        V47[0 + offset] = _mm_unpacklo_epi8(P47[k], zero);
        V47[1 + offset] = _mm_unpackhi_epi8(P47[k], zero);
        V48[0 + offset] = _mm_unpacklo_epi8(P48[k], zero);
        V48[1 + offset] = _mm_unpackhi_epi8(P48[k], zero);
        V49[0 + offset] = _mm_unpacklo_epi8(P49[k], zero);
        V49[1 + offset] = _mm_unpackhi_epi8(P49[k], zero);


        V50[0 + offset] = _mm_unpacklo_epi8(P50[k], zero);
        V50[1 + offset] = _mm_unpackhi_epi8(P50[k], zero);
        V51[0 + offset] = _mm_unpacklo_epi8(P51[k], zero);
        V51[1 + offset] = _mm_unpackhi_epi8(P51[k], zero);
        V52[0 + offset] = _mm_unpacklo_epi8(P52[k], zero);
        V52[1 + offset] = _mm_unpackhi_epi8(P52[k], zero);
        V53[0 + offset] = _mm_unpacklo_epi8(P53[k], zero);
        V53[1 + offset] = _mm_unpackhi_epi8(P53[k], zero);
        V54[0 + offset] = _mm_unpacklo_epi8(P54[k], zero);
        V54[1 + offset] = _mm_unpackhi_epi8(P54[k], zero);
        V55[0 + offset] = _mm_unpacklo_epi8(P55[k], zero);
        V55[1 + offset] = _mm_unpackhi_epi8(P55[k], zero);
        V56[0 + offset] = _mm_unpacklo_epi8(P56[k], zero);
        V56[1 + offset] = _mm_unpackhi_epi8(P56[k], zero);
        V57[0 + offset] = _mm_unpacklo_epi8(P57[k], zero);
        V57[1 + offset] = _mm_unpackhi_epi8(P57[k], zero);
        V58[0 + offset] = _mm_unpacklo_epi8(P58[k], zero);
        V58[1 + offset] = _mm_unpackhi_epi8(P58[k], zero);
        V59[0 + offset] = _mm_unpacklo_epi8(P59[k], zero);
        V59[1 + offset] = _mm_unpackhi_epi8(P59[k], zero);


        V60[0 + offset] = _mm_unpacklo_epi8(P60[k], zero);
        V60[1 + offset] = _mm_unpackhi_epi8(P60[k], zero);
        V61[0 + offset] = _mm_unpacklo_epi8(P61[k], zero);
        V61[1 + offset] = _mm_unpackhi_epi8(P61[k], zero);
        V62[0 + offset] = _mm_unpacklo_epi8(P62[k], zero);
        V62[1 + offset] = _mm_unpackhi_epi8(P62[k], zero);
        V63[0 + offset] = _mm_unpacklo_epi8(P63[k], zero);
        V63[1 + offset] = _mm_unpackhi_epi8(P63[k], zero);
    }


    for (k = 0; k < 8; k++)
    {
        T00[k] = _mm_add_epi16(V00[k], T00[k]);
        T01[k] = _mm_add_epi16(V01[k], T01[k]);
        T02[k] = _mm_add_epi16(V02[k], T02[k]);
        T03[k] = _mm_add_epi16(V03[k], T03[k]);
        T04[k] = _mm_add_epi16(V04[k], T04[k]);
        T05[k] = _mm_add_epi16(V05[k], T05[k]);
        T06[k] = _mm_add_epi16(V06[k], T06[k]);
        T07[k] = _mm_add_epi16(V07[k], T07[k]);
        T08[k] = _mm_add_epi16(V08[k], T08[k]);
        T09[k] = _mm_add_epi16(V09[k], T09[k]);
        T10[k] = _mm_add_epi16(V10[k], T10[k]);
        T11[k] = _mm_add_epi16(V11[k], T11[k]);
        T12[k] = _mm_add_epi16(V12[k], T12[k]);
        T13[k] = _mm_add_epi16(V13[k], T13[k]);
        T14[k] = _mm_add_epi16(V14[k], T14[k]);
        T15[k] = _mm_add_epi16(V15[k], T15[k]);
        T16[k] = _mm_add_epi16(V16[k], T16[k]);
        T17[k] = _mm_add_epi16(V17[k], T17[k]);
        T18[k] = _mm_add_epi16(V18[k], T18[k]);
        T19[k] = _mm_add_epi16(V19[k], T19[k]);
        T20[k] = _mm_add_epi16(V20[k], T20[k]);
        T21[k] = _mm_add_epi16(V21[k], T21[k]);
        T22[k] = _mm_add_epi16(V22[k], T22[k]);
        T23[k] = _mm_add_epi16(V23[k], T23[k]);
        T24[k] = _mm_add_epi16(V24[k], T24[k]);
        T25[k] = _mm_add_epi16(V25[k], T25[k]);
        T26[k] = _mm_add_epi16(V26[k], T26[k]);
        T27[k] = _mm_add_epi16(V27[k], T27[k]);
        T28[k] = _mm_add_epi16(V28[k], T28[k]);
        T29[k] = _mm_add_epi16(V29[k], T29[k]);

        T30[k] = _mm_add_epi16(V30[k], T30[k]);
        T31[k] = _mm_add_epi16(V31[k], T31[k]);
        T32[k] = _mm_add_epi16(V32[k], T32[k]);
        T33[k] = _mm_add_epi16(V33[k], T33[k]);
        T34[k] = _mm_add_epi16(V34[k], T34[k]);
        T35[k] = _mm_add_epi16(V35[k], T35[k]);
        T36[k] = _mm_add_epi16(V36[k], T36[k]);
        T37[k] = _mm_add_epi16(V37[k], T37[k]);
        T38[k] = _mm_add_epi16(V38[k], T38[k]);
        T39[k] = _mm_add_epi16(V39[k], T39[k]);

        T40[k] = _mm_add_epi16(V40[k], T40[k]);
        T41[k] = _mm_add_epi16(V41[k], T41[k]);
        T42[k] = _mm_add_epi16(V42[k], T42[k]);
        T43[k] = _mm_add_epi16(V43[k], T43[k]);
        T44[k] = _mm_add_epi16(V44[k], T44[k]);
        T45[k] = _mm_add_epi16(V45[k], T45[k]);
        T46[k] = _mm_add_epi16(V46[k], T46[k]);
        T47[k] = _mm_add_epi16(V47[k], T47[k]);
        T48[k] = _mm_add_epi16(V48[k], T48[k]);
        T49[k] = _mm_add_epi16(V49[k], T49[k]);

        T50[k] = _mm_add_epi16(V50[k], T50[k]);
        T51[k] = _mm_add_epi16(V51[k], T51[k]);
        T52[k] = _mm_add_epi16(V52[k], T52[k]);
        T53[k] = _mm_add_epi16(V53[k], T53[k]);
        T54[k] = _mm_add_epi16(V54[k], T54[k]);
        T55[k] = _mm_add_epi16(V55[k], T55[k]);
        T56[k] = _mm_add_epi16(V56[k], T56[k]);
        T57[k] = _mm_add_epi16(V57[k], T57[k]);
        T58[k] = _mm_add_epi16(V58[k], T58[k]);
        T59[k] = _mm_add_epi16(V59[k], T59[k]);

        T60[k] = _mm_add_epi16(V60[k], T60[k]);
        T61[k] = _mm_add_epi16(V61[k], T61[k]);
        T62[k] = _mm_add_epi16(V62[k], T62[k]);
        T63[k] = _mm_add_epi16(V63[k], T63[k]);

    }


    for (k = 0; k < 8; k += 2)
    {
        V00[k] = _mm_packus_epi16(T00[k], T00[k + 1]);
        V01[k] = _mm_packus_epi16(T01[k], T01[k + 1]);
        V02[k] = _mm_packus_epi16(T02[k], T02[k + 1]);
        V03[k] = _mm_packus_epi16(T03[k], T03[k + 1]);
        V04[k] = _mm_packus_epi16(T04[k], T04[k + 1]);
        V05[k] = _mm_packus_epi16(T05[k], T05[k + 1]);
        V06[k] = _mm_packus_epi16(T06[k], T06[k + 1]);
        V07[k] = _mm_packus_epi16(T07[k], T07[k + 1]);
        V08[k] = _mm_packus_epi16(T08[k], T08[k + 1]);
        V09[k] = _mm_packus_epi16(T09[k], T09[k + 1]);
        V10[k] = _mm_packus_epi16(T10[k], T10[k + 1]);
        V11[k] = _mm_packus_epi16(T11[k], T11[k + 1]);
        V12[k] = _mm_packus_epi16(T12[k], T12[k + 1]);
        V13[k] = _mm_packus_epi16(T13[k], T13[k + 1]);
        V14[k] = _mm_packus_epi16(T14[k], T14[k + 1]);
        V15[k] = _mm_packus_epi16(T15[k], T15[k + 1]);
        V16[k] = _mm_packus_epi16(T16[k], T16[k + 1]);
        V17[k] = _mm_packus_epi16(T17[k], T17[k + 1]);
        V18[k] = _mm_packus_epi16(T18[k], T18[k + 1]);
        V19[k] = _mm_packus_epi16(T19[k], T19[k + 1]);
        V20[k] = _mm_packus_epi16(T20[k], T20[k + 1]);
        V21[k] = _mm_packus_epi16(T21[k], T21[k + 1]);
        V22[k] = _mm_packus_epi16(T22[k], T22[k + 1]);
        V23[k] = _mm_packus_epi16(T23[k], T23[k + 1]);
        V24[k] = _mm_packus_epi16(T24[k], T24[k + 1]);
        V25[k] = _mm_packus_epi16(T25[k], T25[k + 1]);
        V26[k] = _mm_packus_epi16(T26[k], T26[k + 1]);
        V27[k] = _mm_packus_epi16(T27[k], T27[k + 1]);
        V28[k] = _mm_packus_epi16(T28[k], T28[k + 1]);
        V29[k] = _mm_packus_epi16(T29[k], T29[k + 1]);
        V30[k] = _mm_packus_epi16(T30[k], T30[k + 1]);
        V31[k] = _mm_packus_epi16(T31[k], T31[k + 1]);
        V32[k] = _mm_packus_epi16(T32[k], T32[k + 1]);
        V33[k] = _mm_packus_epi16(T33[k], T33[k + 1]);
        V34[k] = _mm_packus_epi16(T34[k], T34[k + 1]);
        V35[k] = _mm_packus_epi16(T35[k], T35[k + 1]);
        V36[k] = _mm_packus_epi16(T36[k], T36[k + 1]);
        V37[k] = _mm_packus_epi16(T37[k], T37[k + 1]);
        V38[k] = _mm_packus_epi16(T38[k], T38[k + 1]);
        V39[k] = _mm_packus_epi16(T39[k], T39[k + 1]);
        V40[k] = _mm_packus_epi16(T40[k], T40[k + 1]);
        V41[k] = _mm_packus_epi16(T41[k], T41[k + 1]);
        V42[k] = _mm_packus_epi16(T42[k], T42[k + 1]);
        V43[k] = _mm_packus_epi16(T43[k], T43[k + 1]);
        V44[k] = _mm_packus_epi16(T44[k], T44[k + 1]);
        V45[k] = _mm_packus_epi16(T45[k], T45[k + 1]);
        V46[k] = _mm_packus_epi16(T46[k], T46[k + 1]);
        V47[k] = _mm_packus_epi16(T47[k], T47[k + 1]);
        V48[k] = _mm_packus_epi16(T48[k], T48[k + 1]);
        V49[k] = _mm_packus_epi16(T49[k], T49[k + 1]);
        V50[k] = _mm_packus_epi16(T50[k], T50[k + 1]);
        V51[k] = _mm_packus_epi16(T51[k], T51[k + 1]);
        V52[k] = _mm_packus_epi16(T52[k], T52[k + 1]);
        V53[k] = _mm_packus_epi16(T53[k], T53[k + 1]);
        V54[k] = _mm_packus_epi16(T54[k], T54[k + 1]);
        V55[k] = _mm_packus_epi16(T55[k], T55[k + 1]);
        V56[k] = _mm_packus_epi16(T56[k], T56[k + 1]);
        V57[k] = _mm_packus_epi16(T57[k], T57[k + 1]);
        V58[k] = _mm_packus_epi16(T58[k], T58[k + 1]);
        V59[k] = _mm_packus_epi16(T59[k], T59[k + 1]);
        V60[k] = _mm_packus_epi16(T60[k], T60[k + 1]);
        V61[k] = _mm_packus_epi16(T61[k], T61[k + 1]);
        V62[k] = _mm_packus_epi16(T62[k], T62[k + 1]);
        V63[k] = _mm_packus_epi16(T63[k], T63[k + 1]);
    }


    for (k = 0; k < 8; k += 2)
    {
        int offset = k << 3;
        _mm_storeu_si128((__m128i*)&dst[0 + offset], V00[k]);
        _mm_storeu_si128((__m128i*)&dst[1 * i_dst + offset], V01[k]);
        _mm_storeu_si128((__m128i*)&dst[2 * i_dst + offset], V02[k]);
        _mm_storeu_si128((__m128i*)&dst[3 * i_dst + offset], V03[k]);
        _mm_storeu_si128((__m128i*)&dst[4 * i_dst + offset], V04[k]);
        _mm_storeu_si128((__m128i*)&dst[5 * i_dst + offset], V05[k]);
        _mm_storeu_si128((__m128i*)&dst[6 * i_dst + offset], V06[k]);
        _mm_storeu_si128((__m128i*)&dst[7 * i_dst + offset], V07[k]);
        _mm_storeu_si128((__m128i*)&dst[8 * i_dst + offset], V08[k]);
        _mm_storeu_si128((__m128i*)&dst[9 * i_dst + offset], V09[k]);

        _mm_storeu_si128((__m128i*)&dst[10 * i_dst + offset], V10[k]);
        _mm_storeu_si128((__m128i*)&dst[11 * i_dst + offset], V11[k]);
        _mm_storeu_si128((__m128i*)&dst[12 * i_dst + offset], V12[k]);
        _mm_storeu_si128((__m128i*)&dst[13 * i_dst + offset], V13[k]);
        _mm_storeu_si128((__m128i*)&dst[14 * i_dst + offset], V14[k]);
        _mm_storeu_si128((__m128i*)&dst[15 * i_dst + offset], V15[k]);
        _mm_storeu_si128((__m128i*)&dst[16 * i_dst + offset], V16[k]);
        _mm_storeu_si128((__m128i*)&dst[17 * i_dst + offset], V17[k]);
        _mm_storeu_si128((__m128i*)&dst[18 * i_dst + offset], V18[k]);
        _mm_storeu_si128((__m128i*)&dst[19 * i_dst + offset], V19[k]);

        _mm_storeu_si128((__m128i*)&dst[20 * i_dst + offset], V20[k]);
        _mm_storeu_si128((__m128i*)&dst[21 * i_dst + offset], V21[k]);
        _mm_storeu_si128((__m128i*)&dst[22 * i_dst + offset], V22[k]);
        _mm_storeu_si128((__m128i*)&dst[23 * i_dst + offset], V23[k]);
        _mm_storeu_si128((__m128i*)&dst[24 * i_dst + offset], V24[k]);
        _mm_storeu_si128((__m128i*)&dst[25 * i_dst + offset], V25[k]);
        _mm_storeu_si128((__m128i*)&dst[26 * i_dst + offset], V26[k]);
        _mm_storeu_si128((__m128i*)&dst[27 * i_dst + offset], V27[k]);
        _mm_storeu_si128((__m128i*)&dst[28 * i_dst + offset], V28[k]);
        _mm_storeu_si128((__m128i*)&dst[29 * i_dst + offset], V29[k]);

        _mm_storeu_si128((__m128i*)&dst[30 * i_dst + offset], V30[k]);
        _mm_storeu_si128((__m128i*)&dst[31 * i_dst + offset], V31[k]);
        _mm_storeu_si128((__m128i*)&dst[32 * i_dst + offset], V32[k]);
        _mm_storeu_si128((__m128i*)&dst[33 * i_dst + offset], V33[k]);
        _mm_storeu_si128((__m128i*)&dst[34 * i_dst + offset], V34[k]);
        _mm_storeu_si128((__m128i*)&dst[35 * i_dst + offset], V35[k]);
        _mm_storeu_si128((__m128i*)&dst[36 * i_dst + offset], V36[k]);
        _mm_storeu_si128((__m128i*)&dst[37 * i_dst + offset], V37[k]);
        _mm_storeu_si128((__m128i*)&dst[38 * i_dst + offset], V38[k]);
        _mm_storeu_si128((__m128i*)&dst[39 * i_dst + offset], V39[k]);

        _mm_storeu_si128((__m128i*)&dst[40 * i_dst + offset], V40[k]);
        _mm_storeu_si128((__m128i*)&dst[41 * i_dst + offset], V41[k]);
        _mm_storeu_si128((__m128i*)&dst[42 * i_dst + offset], V42[k]);
        _mm_storeu_si128((__m128i*)&dst[43 * i_dst + offset], V43[k]);
        _mm_storeu_si128((__m128i*)&dst[44 * i_dst + offset], V44[k]);
        _mm_storeu_si128((__m128i*)&dst[45 * i_dst + offset], V45[k]);
        _mm_storeu_si128((__m128i*)&dst[46 * i_dst + offset], V46[k]);
        _mm_storeu_si128((__m128i*)&dst[47 * i_dst + offset], V47[k]);
        _mm_storeu_si128((__m128i*)&dst[48 * i_dst + offset], V48[k]);
        _mm_storeu_si128((__m128i*)&dst[49 * i_dst + offset], V49[k]);

        _mm_storeu_si128((__m128i*)&dst[50 * i_dst + offset], V50[k]);
        _mm_storeu_si128((__m128i*)&dst[51 * i_dst + offset], V51[k]);
        _mm_storeu_si128((__m128i*)&dst[52 * i_dst + offset], V52[k]);
        _mm_storeu_si128((__m128i*)&dst[53 * i_dst + offset], V53[k]);
        _mm_storeu_si128((__m128i*)&dst[54 * i_dst + offset], V54[k]);
        _mm_storeu_si128((__m128i*)&dst[55 * i_dst + offset], V55[k]);
        _mm_storeu_si128((__m128i*)&dst[56 * i_dst + offset], V56[k]);
        _mm_storeu_si128((__m128i*)&dst[57 * i_dst + offset], V57[k]);
        _mm_storeu_si128((__m128i*)&dst[58 * i_dst + offset], V58[k]);
        _mm_storeu_si128((__m128i*)&dst[59 * i_dst + offset], V59[k]);

        _mm_storeu_si128((__m128i*)&dst[60 * i_dst + offset], V60[k]);
        _mm_storeu_si128((__m128i*)&dst[61 * i_dst + offset], V61[k]);
        _mm_storeu_si128((__m128i*)&dst[62 * i_dst + offset], V62[k]);
        _mm_storeu_si128((__m128i*)&dst[63 * i_dst + offset], V63[k]);

    }
}

void xTr2nd_8_1d_Inv_Hor_sse128(coef_t *src, int i_src)
{
	__m128i	tmpCoef0, tmpCoef1, tmpCoef2, tmpCoef3;
	__m128i tmpS0, tmpS1, tmpS2, tmpS3;
	__m128i tmpProduct0, tmpProduct1, tmpProduct2, tmpProduct3, tmpSum0, tmpSum1, tmpSum2, tmpSum3, tmpDst0, tmpDst1, tmpDst2, tmpDst3;
	__m128i zero = _mm_setzero_si128();
	__m128i factor;

	/*---hor transform---*/

	// shift = 7; rnd_factor = 64
	factor = _mm_set1_epi32(64);

	//load tab_c8_trans data, a matrix of 4x4
	tmpCoef0 = _mm_set_epi16(0, -3, 0, -8, 0, -35, 0, 123);
	tmpCoef1 = _mm_set_epi16(0, 10, 0, 30, 0, -120, 0, -32);
	tmpCoef2 = _mm_set_epi16(0, -22, 0, 123, 0, 25, 0, 14);
	tmpCoef3 = _mm_set_epi16(0, 126, 0, 19, 0, 13, 0, 8);

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
	tmpDst0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);			//!only low 64bits (4xSHORT) are valid!

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
	tmpDst1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);			//!only low 64bits (4xSHORT) are valid!

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
	tmpDst2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);			//!only low 64bits (4xSHORT) are valid!

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
	tmpDst3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//store blk data
	_mm_storel_epi64((__m128i*)&src[0 * i_src], tmpDst0);
	_mm_storel_epi64((__m128i*)&src[1 * i_src], tmpDst1);
	_mm_storel_epi64((__m128i*)&src[2 * i_src], tmpDst2);
	_mm_storel_epi64((__m128i*)&src[3 * i_src], tmpDst3);
}

void xTr2nd_8_1d_Inv_Vert_sse128(coef_t *src, int i_src)
{
	__m128i	tmpSrc0, tmpSrc1, tmpSrc2, tmpSrc3, tmpLoad0, tmpLoad1, tmpLoad2, tmpLoad3;
	__m128i tmpC0, tmpC1, tmpC2, tmpC3;
	__m128i tmpRes0, tmpRes1, tmpRes2, tmpRes3;
	__m128i tmpProduct0, tmpProduct1, tmpProduct2, tmpProduct3, tmpSum0, tmpSum1, tmpSum2, tmpSum3;
	__m128i zero = _mm_setzero_si128();
	__m128i factor;

	/*---vertical transform first---*/

	//rnd_factor = 64; shift = 7
	factor = _mm_set1_epi32(64);

	//load src data
	tmpLoad0 = _mm_loadu_si128((__m128i*)&src[0]);
	tmpLoad1 = _mm_loadu_si128((__m128i*)&src[i_src]);
	tmpLoad2 = _mm_loadu_si128((__m128i*)&src[i_src * 2]);
	tmpLoad3 = _mm_loadu_si128((__m128i*)&src[i_src * 3]);
	tmpSrc0 = _mm_unpacklo_epi16(tmpLoad0, zero);
	tmpSrc1 = _mm_unpacklo_epi16(tmpLoad1, zero);
	tmpSrc2 = _mm_unpacklo_epi16(tmpLoad2, zero);
	tmpSrc3 = _mm_unpacklo_epi16(tmpLoad3, zero);

	//for i = 0
	//load coef data
	tmpC0 = _mm_set1_epi16(123);							//coef[0][0]
	tmpC1 = _mm_set1_epi16(-32);							//coef[1][0]
	tmpC2 = _mm_set1_epi16(14);
	tmpC3 = _mm_set1_epi16(8);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);			
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes0 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 1
	//load coef data
	tmpC0 = _mm_set1_epi16(-35);
	tmpC1 = _mm_set1_epi16(-120);
	tmpC2 = _mm_set1_epi16(25);
	tmpC3 = _mm_set1_epi16(13);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes1 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 2
	//load tab_c8_trans data
	tmpC0 = _mm_set1_epi16(-8);
	tmpC1 = _mm_set1_epi16(30);
	tmpC2 = _mm_set1_epi16(123);
	tmpC3 = _mm_set1_epi16(19);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes2 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//for i = 3
	//load tab_c8_trans data
	tmpC0 = _mm_set1_epi16(-3);
	tmpC1 = _mm_set1_epi16(10);
	tmpC2 = _mm_set1_epi16(-22);
	tmpC3 = _mm_set1_epi16(126);
	//mutiple & add
	tmpProduct0 = _mm_madd_epi16(tmpC0, tmpSrc0);
	tmpProduct1 = _mm_madd_epi16(tmpC1, tmpSrc1);
	tmpProduct2 = _mm_madd_epi16(tmpC2, tmpSrc2);
	tmpProduct3 = _mm_madd_epi16(tmpC3, tmpSrc3);
	//add operation
	tmpSum0 = _mm_add_epi32(tmpProduct0, tmpProduct1);
	tmpSum1 = _mm_add_epi32(tmpProduct2, tmpProduct3);
	tmpSum2 = _mm_add_epi32(tmpSum0, tmpSum1);
	tmpSum3 = _mm_add_epi32(tmpSum2, factor);
	//shift & clip3
	tmpRes3 = _mm_packs_epi32(_mm_srai_epi32(tmpSum3, 7), zero);				//!only low 64bits (4xSHORT) are valid!

	//store blk data
	_mm_storel_epi64((__m128i*)&src[0 * i_src], tmpRes0);
	_mm_storel_epi64((__m128i*)&src[1 * i_src], tmpRes1);
	_mm_storel_epi64((__m128i*)&src[2 * i_src], tmpRes2);
	_mm_storel_epi64((__m128i*)&src[3 * i_src], tmpRes3);
}

void add_inv_trans_16x16_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	__m256i P00, P02, P04, P06, P08, P10, P12, P14;
	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);		//row0 87high - 90low address
	const __m256i c16_p35_p40 = _mm256_set1_epi32(0x00230028);
	const __m256i c16_p21_p29 = _mm256_set1_epi32(0x0015001D);
	const __m256i c16_p04_p13 = _mm256_set1_epi32(0x0004000D);
	const __m256i c16_p29_p43 = _mm256_set1_epi32(0x001D002B);		//row1
	const __m256i c16_n21_p04 = _mm256_set1_epi32(0xFFEB0004);
	const __m256i c16_n45_n40 = _mm256_set1_epi32(0xFFD3FFD8);
	const __m256i c16_n13_n35 = _mm256_set1_epi32(0xFFF3FFDD);
	const __m256i c16_p04_p40 = _mm256_set1_epi32(0x00040028);		//row2
	const __m256i c16_n43_n35 = _mm256_set1_epi32(0xFFD5FFDD);
	const __m256i c16_p29_n13 = _mm256_set1_epi32(0x001DFFF3);
	const __m256i c16_p21_p45 = _mm256_set1_epi32(0x0015002D);
	const __m256i c16_n21_p35 = _mm256_set1_epi32(0xFFEB0023);		//row3
	const __m256i c16_p04_n43 = _mm256_set1_epi32(0x0004FFD5);
	const __m256i c16_p13_p45 = _mm256_set1_epi32(0x000D002D);
	const __m256i c16_n29_n40 = _mm256_set1_epi32(0xFFE3FFD8);
	const __m256i c16_n40_p29 = _mm256_set1_epi32(0xFFD8001D);		//row4
	const __m256i c16_p45_n13 = _mm256_set1_epi32(0x002DFFF3);
	const __m256i c16_n43_n04 = _mm256_set1_epi32(0xFFD5FFFC);
	const __m256i c16_p35_p21 = _mm256_set1_epi32(0x00230015);
	const __m256i c16_n45_p21 = _mm256_set1_epi32(0xFFD30015);		//row5
	const __m256i c16_p13_p29 = _mm256_set1_epi32(0x000D001D);
	const __m256i c16_p35_n43 = _mm256_set1_epi32(0x0023FFD5);
	const __m256i c16_n40_p04 = _mm256_set1_epi32(0xFFD80004);
	const __m256i c16_n35_p13 = _mm256_set1_epi32(0xFFDD000D);		//row6
	const __m256i c16_n40_p45 = _mm256_set1_epi32(0xFFD8002D);
	const __m256i c16_p04_p21 = _mm256_set1_epi32(0x00040015);
	const __m256i c16_p43_n29 = _mm256_set1_epi32(0x002BFFE3);
	const __m256i c16_n13_p04 = _mm256_set1_epi32(0xFFF30004);		//row7
	const __m256i c16_n29_p21 = _mm256_set1_epi32(0xFFE30015);
	const __m256i c16_n40_p35 = _mm256_set1_epi32(0xFFD80023);
	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);

	const __m256i c16_p38_p44 = _mm256_set1_epi32(0x0026002C);
	const __m256i c16_p09_p25 = _mm256_set1_epi32(0x00090019);
	const __m256i c16_n09_p38 = _mm256_set1_epi32(0xFFF70026);
	const __m256i c16_n25_n44 = _mm256_set1_epi32(0xFFE7FFD4);
	const __m256i c16_n44_p25 = _mm256_set1_epi32(0xFFD40019);
	const __m256i c16_p38_p09 = _mm256_set1_epi32(0x00260009);
	const __m256i c16_n25_p09 = _mm256_set1_epi32(0xFFE70009);
	const __m256i c16_n44_p38 = _mm256_set1_epi32(0xFFD40026);

	const __m256i c16_p17_p42 = _mm256_set1_epi32(0x0011002A);
	const __m256i c16_n42_p17 = _mm256_set1_epi32(0xFFD60011);

	const __m256i c16_n32_p32 = _mm256_set1_epi32(0xFFE00020);
	const __m256i c16_p32_p32 = _mm256_set1_epi32(0x00200020);

	__m256i max_val, min_val;
	__m256i zero = _mm256_setzero_si256();
	__m256i c32_rnd = _mm256_set1_epi32(16);								// 第一次四舍五入
	int nShift = 5;

	__m256i in00, in01, in02, in03, in04, in05, in06, in07;
	__m256i in08, in09, in10, in11, in12, in13, in14, in15;
	__m256i res00, res01, res02, res03, res04, res05, res06, res07;
	__m256i res08, res09, res10, res11, res12, res13, res14, res15;



	in00 = _mm256_lddqu_si256((const __m256i*)&src[0 * 16]);	// [07 06 05 04 03 02 01 00]
	in01 = _mm256_lddqu_si256((const __m256i*)&src[1 * 16]);  // [17 16 15 14 13 12 11 10]
	in02 = _mm256_lddqu_si256((const __m256i*)&src[2 * 16]); // [27 26 25 24 23 22 21 20]
	in03 = _mm256_lddqu_si256((const __m256i*)&src[3 * 16]);     // [37 36 35 34 33 32 31 30]
	in04 = _mm256_lddqu_si256((const __m256i*)&src[4 * 16]);   // [47 46 45 44 43 42 41 40]
	in05 = _mm256_lddqu_si256((const __m256i*)&src[5 * 16]);     // [57 56 55 54 53 52 51 50]
	in06 = _mm256_lddqu_si256((const __m256i*)&src[6 * 16]);    // [67 66 65 64 63 62 61 60]
	in07 = _mm256_lddqu_si256((const __m256i*)&src[7 * 16]);    // [77 76 75 74 73 72 71 70]
	in08 = _mm256_lddqu_si256((const __m256i*)&src[8 * 16]);
	in09 = _mm256_lddqu_si256((const __m256i*)&src[9 * 16]);
	in10 = _mm256_lddqu_si256((const __m256i*)&src[10 * 16]);
	in11 = _mm256_lddqu_si256((const __m256i*)&src[11 * 16]);
	in12 = _mm256_lddqu_si256((const __m256i*)&src[12 * 16]);
	in13 = _mm256_lddqu_si256((const __m256i*)&src[13 * 16]);
	in14 = _mm256_lddqu_si256((const __m256i*)&src[14 * 16]);
	in15 = _mm256_lddqu_si256((const __m256i*)&src[15 * 16]);

	//The first IDCT
	{
		const __m256i T_00_00A = _mm256_unpacklo_epi16(in01, in03);       // [33 13 32 12 31 11 30 10]
		const __m256i T_00_00B = _mm256_unpackhi_epi16(in01, in03);       // [37 17 36 16 35 15 34 14]
		const __m256i T_00_01A = _mm256_unpacklo_epi16(in05, in07);       // [ ]
		const __m256i T_00_01B = _mm256_unpackhi_epi16(in05, in07);       // [ ]
		const __m256i T_00_02A = _mm256_unpacklo_epi16(in09, in11);       // [ ]
		const __m256i T_00_02B = _mm256_unpackhi_epi16(in09, in11);       // [ ]
		const __m256i T_00_03A = _mm256_unpacklo_epi16(in13, in15);       // [ ]
		const __m256i T_00_03B = _mm256_unpackhi_epi16(in13, in15);       // [ ]
		const __m256i T_00_04A = _mm256_unpacklo_epi16(in02, in06);       // [ ]
		const __m256i T_00_04B = _mm256_unpackhi_epi16(in02, in06);       // [ ]
		const __m256i T_00_05A = _mm256_unpacklo_epi16(in10, in14);       // [ ]
		const __m256i T_00_05B = _mm256_unpackhi_epi16(in10, in14);       // [ ]
		const __m256i T_00_06A = _mm256_unpacklo_epi16(in04, in12);       // [ ]row
		const __m256i T_00_06B = _mm256_unpackhi_epi16(in04, in12);       // [ ]
		const __m256i T_00_07A = _mm256_unpacklo_epi16(in00, in08);       // [83 03 82 02 81 01 81 00] row08 row00
		const __m256i T_00_07B = _mm256_unpackhi_epi16(in00, in08);       // [87 07 86 06 85 05 84 04]

		__m256i O0A, O1A, O2A, O3A, O4A, O5A, O6A, O7A;
		__m256i O0B, O1B, O2B, O3B, O4B, O5B, O6B, O7B;
		__m256i EO0A, EO1A, EO2A, EO3A;
		__m256i EO0B, EO1B, EO2B, EO3B;
		__m256i EEO0A, EEO1A;
		__m256i EEO0B, EEO1B;
		__m256i EEE0A, EEE1A;
		__m256i EEE0B, EEE1B;

		{
			__m256i T00, T01;
#define COMPUTE_ROW(row0103, row0507, row0911, row1315, c0103, c0507, c0911, c1315, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(row0103, c0103), _mm256_madd_epi16(row0507, c0507)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(row0911, c0911), _mm256_madd_epi16(row1315, c1315)); \
	row = _mm256_add_epi32(T00, T01);

			COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, O0A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, O1A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, O2A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, O3A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, O4A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, O5A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, O6A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, O7A)

				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, O0B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, O1B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, O2B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, O3B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, O4B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, O5B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, O6B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, O7B)
#undef COMPUTE_ROW
		}

		EO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_p38_p44), _mm256_madd_epi16(T_00_05A, c16_p09_p25)); // EO0
		EO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_p38_p44), _mm256_madd_epi16(T_00_05B, c16_p09_p25));
		EO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_n09_p38), _mm256_madd_epi16(T_00_05A, c16_n25_n44)); // EO1
		EO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_n09_p38), _mm256_madd_epi16(T_00_05B, c16_n25_n44));
		EO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_n44_p25), _mm256_madd_epi16(T_00_05A, c16_p38_p09)); // EO2
		EO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_n44_p25), _mm256_madd_epi16(T_00_05B, c16_p38_p09));
		EO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_n25_p09), _mm256_madd_epi16(T_00_05A, c16_n44_p38)); // EO3
		EO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_n25_p09), _mm256_madd_epi16(T_00_05B, c16_n44_p38));

		EEO0A = _mm256_madd_epi16(T_00_06A, c16_p17_p42);
		EEO0B = _mm256_madd_epi16(T_00_06B, c16_p17_p42);
		EEO1A = _mm256_madd_epi16(T_00_06A, c16_n42_p17);
		EEO1B = _mm256_madd_epi16(T_00_06B, c16_n42_p17);

		EEE0A = _mm256_madd_epi16(T_00_07A, c16_p32_p32);
		EEE0B = _mm256_madd_epi16(T_00_07B, c16_p32_p32);
		EEE1A = _mm256_madd_epi16(T_00_07A, c16_n32_p32);
		EEE1B = _mm256_madd_epi16(T_00_07B, c16_n32_p32);
		{
			const __m256i EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
			const __m256i EE0B = _mm256_add_epi32(EEE0B, EEO0B);
			const __m256i EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
			const __m256i EE1B = _mm256_add_epi32(EEE1B, EEO1B);
			const __m256i EE3A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE2 = EEE0 - EEO0
			const __m256i EE3B = _mm256_sub_epi32(EEE0B, EEO0B);
			const __m256i EE2A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE3 = EEE1 - EEO1
			const __m256i EE2B = _mm256_sub_epi32(EEE1B, EEO1B);

			const __m256i E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
			const __m256i E0B = _mm256_add_epi32(EE0B, EO0B);
			const __m256i E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
			const __m256i E1B = _mm256_add_epi32(EE1B, EO1B);
			const __m256i E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
			const __m256i E2B = _mm256_add_epi32(EE2B, EO2B);
			const __m256i E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
			const __m256i E3B = _mm256_add_epi32(EE3B, EO3B);
			const __m256i E7A = _mm256_sub_epi32(EE0A, EO0A);          // E7 = EE0 - EO0
			const __m256i E7B = _mm256_sub_epi32(EE0B, EO0B);
			const __m256i E6A = _mm256_sub_epi32(EE1A, EO1A);          // E6 = EE1 - EO1
			const __m256i E6B = _mm256_sub_epi32(EE1B, EO1B);
			const __m256i E5A = _mm256_sub_epi32(EE2A, EO2A);          // E5 = EE2 - EO2
			const __m256i E5B = _mm256_sub_epi32(EE2B, EO2B);
			const __m256i E4A = _mm256_sub_epi32(EE3A, EO3A);          // E4 = EE3 - EO3
			const __m256i E4B = _mm256_sub_epi32(EE3B, EO3B);

			const __m256i T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
			const __m256i T10B = _mm256_add_epi32(E0B, c32_rnd);
			const __m256i T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
			const __m256i T11B = _mm256_add_epi32(E1B, c32_rnd);
			const __m256i T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
			const __m256i T12B = _mm256_add_epi32(E2B, c32_rnd);
			const __m256i T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
			const __m256i T13B = _mm256_add_epi32(E3B, c32_rnd);
			const __m256i T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
			const __m256i T14B = _mm256_add_epi32(E4B, c32_rnd);
			const __m256i T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
			const __m256i T15B = _mm256_add_epi32(E5B, c32_rnd);
			const __m256i T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
			const __m256i T16B = _mm256_add_epi32(E6B, c32_rnd);
			const __m256i T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
			const __m256i T17B = _mm256_add_epi32(E7B, c32_rnd);

			const __m256i T20A = _mm256_add_epi32(T10A, O0A);          // E0 + O0 + rnd
			const __m256i T20B = _mm256_add_epi32(T10B, O0B);
			const __m256i T21A = _mm256_add_epi32(T11A, O1A);          // E1 + O1 + rnd
			const __m256i T21B = _mm256_add_epi32(T11B, O1B);
			const __m256i T22A = _mm256_add_epi32(T12A, O2A);          // E2 + O2 + rnd
			const __m256i T22B = _mm256_add_epi32(T12B, O2B);
			const __m256i T23A = _mm256_add_epi32(T13A, O3A);          // E3 + O3 + rnd
			const __m256i T23B = _mm256_add_epi32(T13B, O3B);
			const __m256i T24A = _mm256_add_epi32(T14A, O4A);          // E4
			const __m256i T24B = _mm256_add_epi32(T14B, O4B);
			const __m256i T25A = _mm256_add_epi32(T15A, O5A);          // E5
			const __m256i T25B = _mm256_add_epi32(T15B, O5B);
			const __m256i T26A = _mm256_add_epi32(T16A, O6A);          // E6
			const __m256i T26B = _mm256_add_epi32(T16B, O6B);
			const __m256i T27A = _mm256_add_epi32(T17A, O7A);          // E7
			const __m256i T27B = _mm256_add_epi32(T17B, O7B);
			const __m256i T2FA = _mm256_sub_epi32(T10A, O0A);          // E0 - O0 + rnd
			const __m256i T2FB = _mm256_sub_epi32(T10B, O0B);
			const __m256i T2EA = _mm256_sub_epi32(T11A, O1A);          // E1 - O1 + rnd
			const __m256i T2EB = _mm256_sub_epi32(T11B, O1B);
			const __m256i T2DA = _mm256_sub_epi32(T12A, O2A);          // E2 - O2 + rnd
			const __m256i T2DB = _mm256_sub_epi32(T12B, O2B);
			const __m256i T2CA = _mm256_sub_epi32(T13A, O3A);          // E3 - O3 + rnd
			const __m256i T2CB = _mm256_sub_epi32(T13B, O3B);
			const __m256i T2BA = _mm256_sub_epi32(T14A, O4A);          // E4
			const __m256i T2BB = _mm256_sub_epi32(T14B, O4B);
			const __m256i T2AA = _mm256_sub_epi32(T15A, O5A);          // E5
			const __m256i T2AB = _mm256_sub_epi32(T15B, O5B);
			const __m256i T29A = _mm256_sub_epi32(T16A, O6A);          // E6
			const __m256i T29B = _mm256_sub_epi32(T16B, O6B);
			const __m256i T28A = _mm256_sub_epi32(T17A, O7A);          // E7
			const __m256i T28B = _mm256_sub_epi32(T17B, O7B);

			const __m256i T30A = _mm256_srai_epi32(T20A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			const __m256i T30B = _mm256_srai_epi32(T20B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			const __m256i T31A = _mm256_srai_epi32(T21A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			const __m256i T31B = _mm256_srai_epi32(T21B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
			const __m256i T32A = _mm256_srai_epi32(T22A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
			const __m256i T32B = _mm256_srai_epi32(T22B, nShift);             // [72 62 52 42] // This operation make it much slower than 128
			const __m256i T33A = _mm256_srai_epi32(T23A, nShift);             // [33 23 13 03] // This operation make it much slower than 128
			const __m256i T33B = _mm256_srai_epi32(T23B, nShift);             // [73 63 53 43] // This operation make it much slower than 128
			const __m256i T34A = _mm256_srai_epi32(T24A, nShift);             // [33 24 14 04] // This operation make it much slower than 128
			const __m256i T34B = _mm256_srai_epi32(T24B, nShift);             // [74 64 54 44] // This operation make it much slower than 128
			const __m256i T35A = _mm256_srai_epi32(T25A, nShift);             // [35 25 15 05] // This operation make it much slower than 128
			const __m256i T35B = _mm256_srai_epi32(T25B, nShift);             // [75 65 55 45] // This operation make it much slower than 128
			const __m256i T36A = _mm256_srai_epi32(T26A, nShift);             // [36 26 16 06] // This operation make it much slower than 128
			const __m256i T36B = _mm256_srai_epi32(T26B, nShift);             // [76 66 56 46] // This operation make it much slower than 128
			const __m256i T37A = _mm256_srai_epi32(T27A, nShift);             // [37 27 17 07] // This operation make it much slower than 128
			const __m256i T37B = _mm256_srai_epi32(T27B, nShift);             // [77 67 57 47] // This operation make it much slower than 128

			const __m256i T38A = _mm256_srai_epi32(T28A, nShift);             // [30 20 10 00] x8 // This operation make it much slower than 128
			const __m256i T38B = _mm256_srai_epi32(T28B, nShift);             // [70 60 50 40]
			const __m256i T39A = _mm256_srai_epi32(T29A, nShift);             // [31 21 11 01] x9 // This operation make it much slower than 128
			const __m256i T39B = _mm256_srai_epi32(T29B, nShift);             // [71 61 51 41]
			const __m256i T3AA = _mm256_srai_epi32(T2AA, nShift);             // [32 22 12 02] xA // This operation make it much slower than 128
			const __m256i T3AB = _mm256_srai_epi32(T2AB, nShift);             // [72 62 52 42]
			const __m256i T3BA = _mm256_srai_epi32(T2BA, nShift);             // [33 23 13 03] xB // This operation make it much slower than 128
			const __m256i T3BB = _mm256_srai_epi32(T2BB, nShift);             // [73 63 53 43]
			const __m256i T3CA = _mm256_srai_epi32(T2CA, nShift);             // [33 24 14 04] xC // This operation make it much slower than 128
			const __m256i T3CB = _mm256_srai_epi32(T2CB, nShift);             // [74 64 54 44]
			const __m256i T3DA = _mm256_srai_epi32(T2DA, nShift);             // [35 25 15 05] xD // This operation make it much slower than 128
			const __m256i T3DB = _mm256_srai_epi32(T2DB, nShift);             // [75 65 55 45]
			const __m256i T3EA = _mm256_srai_epi32(T2EA, nShift);             // [36 26 16 06] xE // This operation make it much slower than 128
			const __m256i T3EB = _mm256_srai_epi32(T2EB, nShift);             // [76 66 56 46]
			const __m256i T3FA = _mm256_srai_epi32(T2FA, nShift);             // [37 27 17 07] xF // This operation make it much slower than 128
			const __m256i T3FB = _mm256_srai_epi32(T2FB, nShift);             // [77 67 57 47]

			res00 = _mm256_packs_epi32(T30A, T30B);        // [70 60 50 40 30 20 10 00]
			res01 = _mm256_packs_epi32(T31A, T31B);        // [71 61 51 41 31 21 11 01]
			res02 = _mm256_packs_epi32(T32A, T32B);        // [72 62 52 42 32 22 12 02]
			res03 = _mm256_packs_epi32(T33A, T33B);        // [73 63 53 43 33 23 13 03]
			res04 = _mm256_packs_epi32(T34A, T34B);        // [74 64 54 44 34 24 14 04]
			res05 = _mm256_packs_epi32(T35A, T35B);        // [75 65 55 45 35 25 15 05]
			res06 = _mm256_packs_epi32(T36A, T36B);        // [76 66 56 46 36 26 16 06]
			res07 = _mm256_packs_epi32(T37A, T37B);        // [77 67 57 47 37 27 17 07]

			res08 = _mm256_packs_epi32(T38A, T38B);        // [A0 ... 80]
			res09 = _mm256_packs_epi32(T39A, T39B);        // [A1 ... 81]
			res10 = _mm256_packs_epi32(T3AA, T3AB);        // [A2 ... 82]
			res11 = _mm256_packs_epi32(T3BA, T3BB);        // [A3 ... 83]
			res12 = _mm256_packs_epi32(T3CA, T3CB);        // [A4 ... 84]
			res13 = _mm256_packs_epi32(T3DA, T3DB);        // [A5 ... 85]
			res14 = _mm256_packs_epi32(T3EA, T3EB);        // [A6 ... 86]
			res15 = _mm256_packs_epi32(T3FA, T3FB);        // [A7 ... 87]
		}

		//transpose matrix 16x16 16bit.
		{
			__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
	tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
	tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
	tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
	tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
	tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
	tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
	tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
	tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
	O0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	O1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	O2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	O3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	O4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	O5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	O6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	O7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	O8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
	O9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
	O10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
	O11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
	O12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
	O13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
	O14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
	O15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
	tr0_0 = _mm256_unpacklo_epi64(O0, O1); \
	tr0_1 = _mm256_unpacklo_epi64(O2, O3); \
	tr0_2 = _mm256_unpackhi_epi64(O0, O1); \
	tr0_3 = _mm256_unpackhi_epi64(O2, O3); \
	tr0_4 = _mm256_unpacklo_epi64(O4, O5); \
	tr0_5 = _mm256_unpacklo_epi64(O6, O7); \
	tr0_6 = _mm256_unpackhi_epi64(O4, O5); \
	tr0_7 = _mm256_unpackhi_epi64(O6, O7); \
	tr0_8 = _mm256_unpacklo_epi64(O8, O9); \
	tr0_9 = _mm256_unpacklo_epi64(O10, O11); \
	tr0_10 = _mm256_unpackhi_epi64(O8, O9); \
	tr0_11 = _mm256_unpackhi_epi64(O10, O11); \
	tr0_12 = _mm256_unpacklo_epi64(O12, O13); \
	tr0_13 = _mm256_unpacklo_epi64(O14, O15); \
	tr0_14 = _mm256_unpackhi_epi64(O12, O13); \
	tr0_15 = _mm256_unpackhi_epi64(O14, O15); \
	O0 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x20); \
	O1 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x20); \
	O2 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x20); \
	O3 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x20); \
	O4 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x20); \
	O5 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x20); \
	O6 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x20); \
	O7 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x20); \
	O8 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x31); \
	O9 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x31); \
	O10 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x31); \
	O11 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x31); \
	O12 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x31); \
	O13 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x31); \
	O14 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x31); \
	O15 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x31); \

			TRANSPOSE_16x16_16BIT(res00, res01, res02, res03, res04, res05, res06, res07, res08, res09, res10, res11, res12, res13, res14, res15, in00, in01, in02, in03, in04, in05, in06, in07, in08, in09, in10, in11, in12, in13, in14, in15)
#undef TRANSPOSE_16x16_16BIT
		}
	}


	//The second IDCT
	{
		const __m256i T_00_00A = _mm256_unpacklo_epi16(in01, in03);       // [33 13 32 12 31 11 30 10]
		const __m256i T_00_00B = _mm256_unpackhi_epi16(in01, in03);       // [37 17 36 16 35 15 34 14]
		const __m256i T_00_01A = _mm256_unpacklo_epi16(in05, in07);       // [ ]
		const __m256i T_00_01B = _mm256_unpackhi_epi16(in05, in07);       // [ ]
		const __m256i T_00_02A = _mm256_unpacklo_epi16(in09, in11);       // [ ]
		const __m256i T_00_02B = _mm256_unpackhi_epi16(in09, in11);       // [ ]
		const __m256i T_00_03A = _mm256_unpacklo_epi16(in13, in15);       // [ ]
		const __m256i T_00_03B = _mm256_unpackhi_epi16(in13, in15);       // [ ]
		const __m256i T_00_04A = _mm256_unpacklo_epi16(in02, in06);       // [ ]
		const __m256i T_00_04B = _mm256_unpackhi_epi16(in02, in06);       // [ ]
		const __m256i T_00_05A = _mm256_unpacklo_epi16(in10, in14);       // [ ]
		const __m256i T_00_05B = _mm256_unpackhi_epi16(in10, in14);       // [ ]
		const __m256i T_00_06A = _mm256_unpacklo_epi16(in04, in12);       // [ ]row
		const __m256i T_00_06B = _mm256_unpackhi_epi16(in04, in12);       // [ ]
		const __m256i T_00_07A = _mm256_unpacklo_epi16(in00, in08);       // [83 03 82 02 81 01 81 00] row08 row00
		const __m256i T_00_07B = _mm256_unpackhi_epi16(in00, in08);       // [87 07 86 06 85 05 84 04]

		__m256i O0A, O1A, O2A, O3A, O4A, O5A, O6A, O7A;
		__m256i O0B, O1B, O2B, O3B, O4B, O5B, O6B, O7B;
		__m256i EO0A, EO1A, EO2A, EO3A;
		__m256i EO0B, EO1B, EO2B, EO3B;
		__m256i EEO0A, EEO1A;
		__m256i EEO0B, EEO1B;
		__m256i EEE0A, EEE1A;
		__m256i EEE0B, EEE1B;


		c32_rnd = _mm256_set1_epi32(2048);				// pass == 1 第二次四舍五入
		nShift = 12;

		{
			__m256i T00, T01;
#define COMPUTE_ROW(row0103, row0507, row0911, row1315, c0103, c0507, c0911, c1315, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(row0103, c0103), _mm256_madd_epi16(row0507, c0507)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(row0911, c0911), _mm256_madd_epi16(row1315, c1315)); \
	row = _mm256_add_epi32(T00, T01);

			COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, O0A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, O1A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, O2A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, O3A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, O4A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, O5A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, O6A)
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, O7A)

				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, O0B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, O1B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, O2B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, O3B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, O4B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, O5B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, O6B)
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, O7B)
#undef COMPUTE_ROW
		}

		EO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_p38_p44), _mm256_madd_epi16(T_00_05A, c16_p09_p25)); // EO0
		EO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_p38_p44), _mm256_madd_epi16(T_00_05B, c16_p09_p25));
		EO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_n09_p38), _mm256_madd_epi16(T_00_05A, c16_n25_n44)); // EO1
		EO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_n09_p38), _mm256_madd_epi16(T_00_05B, c16_n25_n44));
		EO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_n44_p25), _mm256_madd_epi16(T_00_05A, c16_p38_p09)); // EO2
		EO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_n44_p25), _mm256_madd_epi16(T_00_05B, c16_p38_p09));
		EO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_04A, c16_n25_p09), _mm256_madd_epi16(T_00_05A, c16_n44_p38)); // EO3
		EO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_04B, c16_n25_p09), _mm256_madd_epi16(T_00_05B, c16_n44_p38));

		EEO0A = _mm256_madd_epi16(T_00_06A, c16_p17_p42);
		EEO0B = _mm256_madd_epi16(T_00_06B, c16_p17_p42);
		EEO1A = _mm256_madd_epi16(T_00_06A, c16_n42_p17);
		EEO1B = _mm256_madd_epi16(T_00_06B, c16_n42_p17);

		EEE0A = _mm256_madd_epi16(T_00_07A, c16_p32_p32);
		EEE0B = _mm256_madd_epi16(T_00_07B, c16_p32_p32);
		EEE1A = _mm256_madd_epi16(T_00_07A, c16_n32_p32);
		EEE1B = _mm256_madd_epi16(T_00_07B, c16_n32_p32);
		{
			const __m256i EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
			const __m256i EE0B = _mm256_add_epi32(EEE0B, EEO0B);
			const __m256i EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
			const __m256i EE1B = _mm256_add_epi32(EEE1B, EEO1B);
			const __m256i EE3A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE2 = EEE0 - EEO0
			const __m256i EE3B = _mm256_sub_epi32(EEE0B, EEO0B);
			const __m256i EE2A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE3 = EEE1 - EEO1
			const __m256i EE2B = _mm256_sub_epi32(EEE1B, EEO1B);

			const __m256i E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
			const __m256i E0B = _mm256_add_epi32(EE0B, EO0B);
			const __m256i E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
			const __m256i E1B = _mm256_add_epi32(EE1B, EO1B);
			const __m256i E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
			const __m256i E2B = _mm256_add_epi32(EE2B, EO2B);
			const __m256i E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
			const __m256i E3B = _mm256_add_epi32(EE3B, EO3B);
			const __m256i E7A = _mm256_sub_epi32(EE0A, EO0A);          // E7 = EE0 - EO0
			const __m256i E7B = _mm256_sub_epi32(EE0B, EO0B);
			const __m256i E6A = _mm256_sub_epi32(EE1A, EO1A);          // E6 = EE1 - EO1
			const __m256i E6B = _mm256_sub_epi32(EE1B, EO1B);
			const __m256i E5A = _mm256_sub_epi32(EE2A, EO2A);          // E5 = EE2 - EO2
			const __m256i E5B = _mm256_sub_epi32(EE2B, EO2B);
			const __m256i E4A = _mm256_sub_epi32(EE3A, EO3A);          // E4 = EE3 - EO3
			const __m256i E4B = _mm256_sub_epi32(EE3B, EO3B);

			const __m256i T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
			const __m256i T10B = _mm256_add_epi32(E0B, c32_rnd);
			const __m256i T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
			const __m256i T11B = _mm256_add_epi32(E1B, c32_rnd);
			const __m256i T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
			const __m256i T12B = _mm256_add_epi32(E2B, c32_rnd);
			const __m256i T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
			const __m256i T13B = _mm256_add_epi32(E3B, c32_rnd);
			const __m256i T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
			const __m256i T14B = _mm256_add_epi32(E4B, c32_rnd);
			const __m256i T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
			const __m256i T15B = _mm256_add_epi32(E5B, c32_rnd);
			const __m256i T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
			const __m256i T16B = _mm256_add_epi32(E6B, c32_rnd);
			const __m256i T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
			const __m256i T17B = _mm256_add_epi32(E7B, c32_rnd);

			const __m256i T20A = _mm256_add_epi32(T10A, O0A);          // E0 + O0 + rnd
			const __m256i T20B = _mm256_add_epi32(T10B, O0B);
			const __m256i T21A = _mm256_add_epi32(T11A, O1A);          // E1 + O1 + rnd
			const __m256i T21B = _mm256_add_epi32(T11B, O1B);
			const __m256i T22A = _mm256_add_epi32(T12A, O2A);          // E2 + O2 + rnd
			const __m256i T22B = _mm256_add_epi32(T12B, O2B);
			const __m256i T23A = _mm256_add_epi32(T13A, O3A);          // E3 + O3 + rnd
			const __m256i T23B = _mm256_add_epi32(T13B, O3B);
			const __m256i T24A = _mm256_add_epi32(T14A, O4A);          // E4
			const __m256i T24B = _mm256_add_epi32(T14B, O4B);
			const __m256i T25A = _mm256_add_epi32(T15A, O5A);          // E5
			const __m256i T25B = _mm256_add_epi32(T15B, O5B);
			const __m256i T26A = _mm256_add_epi32(T16A, O6A);          // E6
			const __m256i T26B = _mm256_add_epi32(T16B, O6B);
			const __m256i T27A = _mm256_add_epi32(T17A, O7A);          // E7
			const __m256i T27B = _mm256_add_epi32(T17B, O7B);
			const __m256i T2FA = _mm256_sub_epi32(T10A, O0A);          // E0 - O0 + rnd
			const __m256i T2FB = _mm256_sub_epi32(T10B, O0B);
			const __m256i T2EA = _mm256_sub_epi32(T11A, O1A);          // E1 - O1 + rnd
			const __m256i T2EB = _mm256_sub_epi32(T11B, O1B);
			const __m256i T2DA = _mm256_sub_epi32(T12A, O2A);          // E2 - O2 + rnd
			const __m256i T2DB = _mm256_sub_epi32(T12B, O2B);
			const __m256i T2CA = _mm256_sub_epi32(T13A, O3A);          // E3 - O3 + rnd
			const __m256i T2CB = _mm256_sub_epi32(T13B, O3B);
			const __m256i T2BA = _mm256_sub_epi32(T14A, O4A);          // E4
			const __m256i T2BB = _mm256_sub_epi32(T14B, O4B);
			const __m256i T2AA = _mm256_sub_epi32(T15A, O5A);          // E5
			const __m256i T2AB = _mm256_sub_epi32(T15B, O5B);
			const __m256i T29A = _mm256_sub_epi32(T16A, O6A);          // E6
			const __m256i T29B = _mm256_sub_epi32(T16B, O6B);
			const __m256i T28A = _mm256_sub_epi32(T17A, O7A);          // E7
			const __m256i T28B = _mm256_sub_epi32(T17B, O7B);

			const __m256i T30A = _mm256_srai_epi32(T20A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			const __m256i T30B = _mm256_srai_epi32(T20B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			const __m256i T31A = _mm256_srai_epi32(T21A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			const __m256i T31B = _mm256_srai_epi32(T21B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
			const __m256i T32A = _mm256_srai_epi32(T22A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
			const __m256i T32B = _mm256_srai_epi32(T22B, nShift);             // [72 62 52 42] // This operation make it much slower than 128
			const __m256i T33A = _mm256_srai_epi32(T23A, nShift);             // [33 23 13 03] // This operation make it much slower than 128
			const __m256i T33B = _mm256_srai_epi32(T23B, nShift);             // [73 63 53 43] // This operation make it much slower than 128
			const __m256i T34A = _mm256_srai_epi32(T24A, nShift);             // [33 24 14 04] // This operation make it much slower than 128
			const __m256i T34B = _mm256_srai_epi32(T24B, nShift);             // [74 64 54 44] // This operation make it much slower than 128
			const __m256i T35A = _mm256_srai_epi32(T25A, nShift);             // [35 25 15 05] // This operation make it much slower than 128
			const __m256i T35B = _mm256_srai_epi32(T25B, nShift);             // [75 65 55 45] // This operation make it much slower than 128
			const __m256i T36A = _mm256_srai_epi32(T26A, nShift);             // [36 26 16 06] // This operation make it much slower than 128
			const __m256i T36B = _mm256_srai_epi32(T26B, nShift);             // [76 66 56 46] // This operation make it much slower than 128
			const __m256i T37A = _mm256_srai_epi32(T27A, nShift);             // [37 27 17 07] // This operation make it much slower than 128
			const __m256i T37B = _mm256_srai_epi32(T27B, nShift);             // [77 67 57 47] // This operation make it much slower than 128

			const __m256i T38A = _mm256_srai_epi32(T28A, nShift);             // [30 20 10 00] x8 // This operation make it much slower than 128
			const __m256i T38B = _mm256_srai_epi32(T28B, nShift);             // [70 60 50 40]
			const __m256i T39A = _mm256_srai_epi32(T29A, nShift);             // [31 21 11 01] x9 // This operation make it much slower than 128
			const __m256i T39B = _mm256_srai_epi32(T29B, nShift);             // [71 61 51 41]
			const __m256i T3AA = _mm256_srai_epi32(T2AA, nShift);             // [32 22 12 02] xA // This operation make it much slower than 128
			const __m256i T3AB = _mm256_srai_epi32(T2AB, nShift);             // [72 62 52 42]
			const __m256i T3BA = _mm256_srai_epi32(T2BA, nShift);             // [33 23 13 03] xB // This operation make it much slower than 128
			const __m256i T3BB = _mm256_srai_epi32(T2BB, nShift);             // [73 63 53 43]
			const __m256i T3CA = _mm256_srai_epi32(T2CA, nShift);             // [33 24 14 04] xC // This operation make it much slower than 128
			const __m256i T3CB = _mm256_srai_epi32(T2CB, nShift);             // [74 64 54 44]
			const __m256i T3DA = _mm256_srai_epi32(T2DA, nShift);             // [35 25 15 05] xD // This operation make it much slower than 128
			const __m256i T3DB = _mm256_srai_epi32(T2DB, nShift);             // [75 65 55 45]
			const __m256i T3EA = _mm256_srai_epi32(T2EA, nShift);             // [36 26 16 06] xE // This operation make it much slower than 128
			const __m256i T3EB = _mm256_srai_epi32(T2EB, nShift);             // [76 66 56 46]
			const __m256i T3FA = _mm256_srai_epi32(T2FA, nShift);             // [37 27 17 07] xF // This operation make it much slower than 128
			const __m256i T3FB = _mm256_srai_epi32(T2FB, nShift);             // [77 67 57 47]

			res00 = _mm256_packs_epi32(T30A, T30B);        // [70 60 50 40 30 20 10 00]
			res01 = _mm256_packs_epi32(T31A, T31B);        // [71 61 51 41 31 21 11 01]
			res02 = _mm256_packs_epi32(T32A, T32B);        // [72 62 52 42 32 22 12 02]
			res03 = _mm256_packs_epi32(T33A, T33B);        // [73 63 53 43 33 23 13 03]
			res04 = _mm256_packs_epi32(T34A, T34B);        // [74 64 54 44 34 24 14 04]
			res05 = _mm256_packs_epi32(T35A, T35B);        // [75 65 55 45 35 25 15 05]
			res06 = _mm256_packs_epi32(T36A, T36B);        // [76 66 56 46 36 26 16 06]
			res07 = _mm256_packs_epi32(T37A, T37B);        // [77 67 57 47 37 27 17 07]

			res08 = _mm256_packs_epi32(T38A, T38B);        // [A0 ... 80]
			res09 = _mm256_packs_epi32(T39A, T39B);        // [A1 ... 81]
			res10 = _mm256_packs_epi32(T3AA, T3AB);        // [A2 ... 82]
			res11 = _mm256_packs_epi32(T3BA, T3BB);        // [A3 ... 83]
			res12 = _mm256_packs_epi32(T3CA, T3CB);        // [A4 ... 84]
			res13 = _mm256_packs_epi32(T3DA, T3DB);        // [A5 ... 85]
			res14 = _mm256_packs_epi32(T3EA, T3EB);        // [A6 ... 86]
			res15 = _mm256_packs_epi32(T3FA, T3FB);        // [A7 ... 87]
		}

		//transpose matrix 16x16 16bit.
		//Not the all transpose.
		//Considering the add predict pixel
		{
			__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT_PARTIAL(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
	tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
	tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
	tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
	tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
	tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
	tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
	tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
	tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
	I0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	I1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	I2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	I3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	I4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	I5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	I6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	I7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	I8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
	I9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
	I10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
	I11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
	I12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
	I13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
	I14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
	I15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
	O0 = _mm256_unpacklo_epi64(I0, I1); \
	O1 = _mm256_unpacklo_epi64(I2, I3); \
	O2 = _mm256_unpackhi_epi64(I0, I1); \
	O3 = _mm256_unpackhi_epi64(I2, I3); \
	O4 = _mm256_unpacklo_epi64(I4, I5); \
	O5 = _mm256_unpacklo_epi64(I6, I7); \
	O6 = _mm256_unpackhi_epi64(I4, I5); \
	O7 = _mm256_unpackhi_epi64(I6, I7); \
	O8 = _mm256_unpacklo_epi64(I8, I9); \
	O9 = _mm256_unpacklo_epi64(I10, I11); \
	O10 = _mm256_unpackhi_epi64(I8, I9); \
	O11 = _mm256_unpackhi_epi64(I10, I11); \
	O12 = _mm256_unpacklo_epi64(I12, I13); \
	O13 = _mm256_unpacklo_epi64(I14, I15); \
	O14 = _mm256_unpackhi_epi64(I12, I13); \
	O15 = _mm256_unpackhi_epi64(I14, I15); \

			TRANSPOSE_16x16_16BIT_PARTIAL(res00, res01, res02, res03, res04, res05, res06, res07, res08, res09, res10, res11, res12, res13, res14, res15, in00, in01, in02, in03, in04, in05, in06, in07, in08, in09, in10, in11, in12, in13, in14, in15)
#undef TRANSPOSE_16x16_16BIT
		}
	}

	//clip
	max_val = _mm256_set1_epi16(255);
	min_val = _mm256_set1_epi16(-256);
	in00 = _mm256_min_epi16(in00, max_val);
	in00 = _mm256_max_epi16(in00, min_val);
	in01 = _mm256_min_epi16(in01, max_val);
	in01 = _mm256_max_epi16(in01, min_val);
	in02 = _mm256_min_epi16(in02, max_val);
	in02 = _mm256_max_epi16(in02, min_val);
	in03 = _mm256_min_epi16(in03, max_val);
	in03 = _mm256_max_epi16(in03, min_val);
	in04 = _mm256_min_epi16(in04, max_val);
	in04 = _mm256_max_epi16(in04, min_val);
	in05 = _mm256_min_epi16(in05, max_val);
	in05 = _mm256_max_epi16(in05, min_val);
	in06 = _mm256_min_epi16(in06, max_val);
	in06 = _mm256_max_epi16(in06, min_val);
	in07 = _mm256_min_epi16(in07, max_val);
	in07 = _mm256_max_epi16(in07, min_val);
	in08 = _mm256_min_epi16(in08, max_val);
	in08 = _mm256_max_epi16(in08, min_val);
	in09 = _mm256_min_epi16(in09, max_val);
	in09 = _mm256_max_epi16(in09, min_val);
	in10 = _mm256_min_epi16(in10, max_val);
	in10 = _mm256_max_epi16(in10, min_val);
	in11 = _mm256_min_epi16(in11, max_val);
	in11 = _mm256_max_epi16(in11, min_val);
	in12 = _mm256_min_epi16(in12, max_val);
	in12 = _mm256_max_epi16(in12, min_val);
	in13 = _mm256_min_epi16(in13, max_val);
	in13 = _mm256_max_epi16(in13, min_val);
	in14 = _mm256_min_epi16(in14, max_val);
	in14 = _mm256_max_epi16(in14, min_val);
	in15 = _mm256_min_epi16(in15, max_val);
	in15 = _mm256_max_epi16(in15, min_val);

	P00 = _mm256_loadu2_m128i((const __m128i*)&pred[8 * i_pred], (const __m128i*)&pred[0]);
	P02 = _mm256_loadu2_m128i((const __m128i*)&pred[9 * i_pred], (const __m128i*)&pred[1 * i_pred]);
	P04 = _mm256_loadu2_m128i((const __m128i*)&pred[10 * i_pred], (const __m128i*)&pred[2 * i_pred]);
	P06 = _mm256_loadu2_m128i((const __m128i*)&pred[11 * i_pred], (const __m128i*)&pred[3 * i_pred]);
	P08 = _mm256_loadu2_m128i((const __m128i*)&pred[12 * i_pred], (const __m128i*)&pred[4 * i_pred]);
	P10 = _mm256_loadu2_m128i((const __m128i*)&pred[13 * i_pred], (const __m128i*)&pred[5 * i_pred]);
	P12 = _mm256_loadu2_m128i((const __m128i*)&pred[14 * i_pred], (const __m128i*)&pred[6 * i_pred]);
	P14 = _mm256_loadu2_m128i((const __m128i*)&pred[15 * i_pred], (const __m128i*)&pred[7 * i_pred]);

	res00 = _mm256_unpacklo_epi8(P00, zero);
	res01 = _mm256_unpackhi_epi8(P00, zero);
	res02 = _mm256_unpacklo_epi8(P02, zero);
	res03 = _mm256_unpackhi_epi8(P02, zero);
	res04 = _mm256_unpacklo_epi8(P04, zero);
	res05 = _mm256_unpackhi_epi8(P04, zero);
	res06 = _mm256_unpacklo_epi8(P06, zero);
	res07 = _mm256_unpackhi_epi8(P06, zero);
	res08 = _mm256_unpacklo_epi8(P08, zero);
	res09 = _mm256_unpackhi_epi8(P08, zero);
	res10 = _mm256_unpacklo_epi8(P10, zero);
	res11 = _mm256_unpackhi_epi8(P10, zero);
	res12 = _mm256_unpacklo_epi8(P12, zero);
	res13 = _mm256_unpackhi_epi8(P12, zero);
	res14 = _mm256_unpacklo_epi8(P14, zero);
	res15 = _mm256_unpackhi_epi8(P14, zero);

	res00 = _mm256_add_epi16(in00, res00);
	res01 = _mm256_add_epi16(in01, res01);
	res02 = _mm256_add_epi16(in02, res02);
	res03 = _mm256_add_epi16(in03, res03);
	res04 = _mm256_add_epi16(in04, res04);
	res05 = _mm256_add_epi16(in05, res05);
	res06 = _mm256_add_epi16(in06, res06);
	res07 = _mm256_add_epi16(in07, res07);
	res08 = _mm256_add_epi16(in08, res08);
	res09 = _mm256_add_epi16(in09, res09);
	res10 = _mm256_add_epi16(in10, res10);
	res11 = _mm256_add_epi16(in11, res11);
	res12 = _mm256_add_epi16(in12, res12);
	res13 = _mm256_add_epi16(in13, res13);
	res14 = _mm256_add_epi16(in14, res14);
	res15 = _mm256_add_epi16(in15, res15);

	P00 = _mm256_packus_epi16(res00, res01);
	P02 = _mm256_packus_epi16(res02, res03);
	P04 = _mm256_packus_epi16(res04, res05);
	P06 = _mm256_packus_epi16(res06, res07);
	P08 = _mm256_packus_epi16(res08, res09);
	P10 = _mm256_packus_epi16(res10, res11);
	P12 = _mm256_packus_epi16(res12, res13);
	P14 = _mm256_packus_epi16(res14, res15);


	//_mm256_storeu2_m128i((__m128i*)&dst[8 * i_dst], (__m128i*)&dst[0], P00);
	//_mm256_storeu2_m128i((__m128i*)&dst[9 * i_dst], (__m128i*)&dst[1 * i_dst], P02);
	//_mm256_storeu2_m128i((__m128i*)&dst[10 * i_dst], (__m128i*)&dst[2 * i_dst], P04);
	//_mm256_storeu2_m128i((__m128i*)&dst[11 * i_dst], (__m128i*)&dst[3 * i_dst], P06);
	//_mm256_storeu2_m128i((__m128i*)&dst[12 * i_dst], (__m128i*)&dst[4 * i_dst], P08);
	//_mm256_storeu2_m128i((__m128i*)&dst[13 * i_dst], (__m128i*)&dst[5 * i_dst], P10);
	//_mm256_storeu2_m128i((__m128i*)&dst[14 * i_dst], (__m128i*)&dst[6 * i_dst], P12);
	//_mm256_storeu2_m128i((__m128i*)&dst[15 * i_dst], (__m128i*)&dst[7 * i_dst], P14);



	_mm_storeu_si128((__m128i*)&dst[0], _mm256_castsi256_si128(P00)); _mm_storeu_si128((__m128i*)&dst[8 * i_dst], _mm256_extractf128_si256(P00, 0x1));
	_mm_storeu_si128((__m128i*)&dst[1 * i_dst], _mm256_castsi256_si128(P02)); _mm_storeu_si128((__m128i*)&dst[9 * i_dst], _mm256_extractf128_si256(P02, 0x1));
	_mm_storeu_si128((__m128i*)&dst[2 * i_dst], _mm256_castsi256_si128(P04)); _mm_storeu_si128((__m128i*)&dst[10 * i_dst], _mm256_extractf128_si256(P04, 0x1));
	_mm_storeu_si128((__m128i*)&dst[3 * i_dst], _mm256_castsi256_si128(P06)); _mm_storeu_si128((__m128i*)&dst[11 * i_dst], _mm256_extractf128_si256(P06, 0x1));
	_mm_storeu_si128((__m128i*)&dst[4 * i_dst], _mm256_castsi256_si128(P08)); _mm_storeu_si128((__m128i*)&dst[12 * i_dst], _mm256_extractf128_si256(P08, 0x1));
	_mm_storeu_si128((__m128i*)&dst[5 * i_dst], _mm256_castsi256_si128(P10)); _mm_storeu_si128((__m128i*)&dst[13 * i_dst], _mm256_extractf128_si256(P10, 0x1));
	_mm_storeu_si128((__m128i*)&dst[6 * i_dst], _mm256_castsi256_si128(P12)); _mm_storeu_si128((__m128i*)&dst[14 * i_dst], _mm256_extractf128_si256(P12, 0x1));
	_mm_storeu_si128((__m128i*)&dst[7 * i_dst], _mm256_castsi256_si128(P14)); _mm_storeu_si128((__m128i*)&dst[15 * i_dst], _mm256_extractf128_si256(P14, 0x1));


}

void add_inv_trans_32x32_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	int k, i;
	__m256i P00, P01, P02, P03, P04, P05, P06, P07, P08, P09, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P30, P31;
	__m256i max_val, min_val;
	__m256i EEO0A, EEO1A, EEO2A, EEO3A, EEO0B, EEO1B, EEO2B, EEO3B;
	__m256i EEEO0A, EEEO0B, EEEO1A, EEEO1B;
	__m256i EEEE0A, EEEE0B, EEEE1A, EEEE1B;
	__m256i EEE0A, EEE0B, EEE1A, EEE1B, EEE3A, EEE3B, EEE2A, EEE2B;
	__m256i EE0A, EE0B, EE1A, EE1B, EE2A, EE2B, EE3A, EE3B, EE7A, EE7B, EE6A, EE6B, EE5A, EE5B, EE4A, EE4B;
	__m256i E0A, E0B, E1A, E1B, E2A, E2B, E3A, E3B, E4A, E4B, E5A, E5B, E6A, E6B, E7A, E7B, EFA, EFB, EEA, EEB, EDA, EDB, ECA, ECB, EBA, EBB, EAA, EAB, E9A, E9B, E8A, E8B;
	__m256i T10A, T10B, T11A, T11B, T12A, T12B, T13A, T13B, T14A, T14B, T15A, T15B, T16A, T16B, T17A, T17B, T18A, T18B, T19A, T19B, T1AA, T1AB, T1BA, T1BB, T1CA, T1CB, T1DA, T1DB, T1EA, T1EB, T1FA, T1FB;
	__m256i T2_00A, T2_00B, T2_01A, T2_01B, T2_02A, T2_02B, T2_03A, T2_03B, T2_04A, T2_04B, T2_05A, T2_05B, T2_06A, T2_06B, T2_07A, T2_07B, T2_08A, T2_08B, T2_09A, T2_09B, T2_10A, T2_10B, T2_11A, T2_11B, T2_12A, T2_12B, T2_13A, T2_13B, T2_14A, T2_14B, T2_15A, T2_15B, T2_31A, T2_31B, T2_30A, T2_30B, T2_29A, T2_29B, T2_28A, T2_28B, T2_27A, T2_27B, T2_26A, T2_26B, T2_25A, T2_25B, T2_24A, T2_24B, T2_23A, T2_23B, T2_22A, T2_22B, T2_21A, T2_21B, T2_20A, T2_20B, T2_19A, T2_19B, T2_18A, T2_18B, T2_17A, T2_17B, T2_16A, T2_16B;
	__m256i T3_00A, T3_00B, T3_01A, T3_01B, T3_02A, T3_02B, T3_03A, T3_03B, T3_04A, T3_04B, T3_05A, T3_05B, T3_06A, T3_06B, T3_07A, T3_07B, T3_08A, T3_08B, T3_09A, T3_09B, T3_10A, T3_10B, T3_11A, T3_11B, T3_12A, T3_12B, T3_13A, T3_13B, T3_14A, T3_14B, T3_15A, T3_15B;
	__m256i T3_16A, T3_16B, T3_17A, T3_17B, T3_18A, T3_18B, T3_19A, T3_19B, T3_20A, T3_20B, T3_21A, T3_21B, T3_22A, T3_22B, T3_23A, T3_23B, T3_24A, T3_24B, T3_25A, T3_25B, T3_26A, T3_26B, T3_27A, T3_27B, T3_28A, T3_28B, T3_29A, T3_29B, T3_30A, T3_30B, T3_31A, T3_31B;
	const __m256i c16_p45_p45 = _mm256_set1_epi32(0x002D002D);
	const __m256i c16_p43_p44 = _mm256_set1_epi32(0x002B002C);
	const __m256i c16_p39_p41 = _mm256_set1_epi32(0x00270029);
	const __m256i c16_p34_p36 = _mm256_set1_epi32(0x00220024);
	const __m256i c16_p27_p30 = _mm256_set1_epi32(0x001B001E);
	const __m256i c16_p19_p23 = _mm256_set1_epi32(0x00130017);
	const __m256i c16_p11_p15 = _mm256_set1_epi32(0x000B000F);
	const __m256i c16_p02_p07 = _mm256_set1_epi32(0x00020007);
	const __m256i c16_p41_p45 = _mm256_set1_epi32(0x0029002D);
	const __m256i c16_p23_p34 = _mm256_set1_epi32(0x00170022);
	const __m256i c16_n02_p11 = _mm256_set1_epi32(0xFFFE000B);
	const __m256i c16_n27_n15 = _mm256_set1_epi32(0xFFE5FFF1);
	const __m256i c16_n43_n36 = _mm256_set1_epi32(0xFFD5FFDC);
	const __m256i c16_n44_n45 = _mm256_set1_epi32(0xFFD4FFD3);
	const __m256i c16_n30_n39 = _mm256_set1_epi32(0xFFE2FFD9);
	const __m256i c16_n07_n19 = _mm256_set1_epi32(0xFFF9FFED);
	const __m256i c16_p34_p44 = _mm256_set1_epi32(0x0022002C);
	const __m256i c16_n07_p15 = _mm256_set1_epi32(0xFFF9000F);
	const __m256i c16_n41_n27 = _mm256_set1_epi32(0xFFD7FFE5);
	const __m256i c16_n39_n45 = _mm256_set1_epi32(0xFFD9FFD3);
	const __m256i c16_n02_n23 = _mm256_set1_epi32(0xFFFEFFE9);
	const __m256i c16_p36_p19 = _mm256_set1_epi32(0x00240013);
	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	const __m256i c16_p11_p30 = _mm256_set1_epi32(0x000B001E);
	const __m256i c16_p23_p43 = _mm256_set1_epi32(0x0017002B);
	const __m256i c16_n34_n07 = _mm256_set1_epi32(0xFFDEFFF9);
	const __m256i c16_n36_n45 = _mm256_set1_epi32(0xFFDCFFD3);
	const __m256i c16_p19_n11 = _mm256_set1_epi32(0x0013FFF5);
	const __m256i c16_p44_p41 = _mm256_set1_epi32(0x002C0029);
	const __m256i c16_n02_p27 = _mm256_set1_epi32(0xFFFE001B);
	const __m256i c16_n45_n30 = _mm256_set1_epi32(0xFFD3FFE2);
	const __m256i c16_n15_n39 = _mm256_set1_epi32(0xFFF1FFD9);
	const __m256i c16_p11_p41 = _mm256_set1_epi32(0x000B0029);
	const __m256i c16_n45_n27 = _mm256_set1_epi32(0xFFD3FFE5);
	const __m256i c16_p07_n30 = _mm256_set1_epi32(0x0007FFE2);
	const __m256i c16_p43_p39 = _mm256_set1_epi32(0x002B0027);
	const __m256i c16_n23_p15 = _mm256_set1_epi32(0xFFE9000F);
	const __m256i c16_n34_n45 = _mm256_set1_epi32(0xFFDEFFD3);
	const __m256i c16_p36_p02 = _mm256_set1_epi32(0x00240002);
	const __m256i c16_p19_p44 = _mm256_set1_epi32(0x0013002C);
	const __m256i c16_n02_p39 = _mm256_set1_epi32(0xFFFE0027);
	const __m256i c16_n36_n41 = _mm256_set1_epi32(0xFFDCFFD7);
	const __m256i c16_p43_p07 = _mm256_set1_epi32(0x002B0007);
	const __m256i c16_n11_p34 = _mm256_set1_epi32(0xFFF50022);
	const __m256i c16_n30_n44 = _mm256_set1_epi32(0xFFE2FFD4);
	const __m256i c16_p45_p15 = _mm256_set1_epi32(0x002D000F);
	const __m256i c16_n19_p27 = _mm256_set1_epi32(0xFFED001B);
	const __m256i c16_n23_n45 = _mm256_set1_epi32(0xFFE9FFD3);
	const __m256i c16_n15_p36 = _mm256_set1_epi32(0xFFF10024);
	const __m256i c16_n11_n45 = _mm256_set1_epi32(0xFFF5FFD3);
	const __m256i c16_p34_p39 = _mm256_set1_epi32(0x00220027);
	const __m256i c16_n45_n19 = _mm256_set1_epi32(0xFFD3FFED);
	const __m256i c16_p41_n07 = _mm256_set1_epi32(0x0029FFF9);
	const __m256i c16_n23_p30 = _mm256_set1_epi32(0xFFE9001E);
	const __m256i c16_n02_n44 = _mm256_set1_epi32(0xFFFEFFD4);
	const __m256i c16_p27_p43 = _mm256_set1_epi32(0x001B002B);
	const __m256i c16_n27_p34 = _mm256_set1_epi32(0xFFE50022);
	const __m256i c16_p19_n39 = _mm256_set1_epi32(0x0013FFD9);
	const __m256i c16_n11_p43 = _mm256_set1_epi32(0xFFF5002B);
	const __m256i c16_p02_n45 = _mm256_set1_epi32(0x0002FFD3);
	const __m256i c16_p07_p45 = _mm256_set1_epi32(0x0007002D);
	const __m256i c16_n15_n44 = _mm256_set1_epi32(0xFFF1FFD4);
	const __m256i c16_p23_p41 = _mm256_set1_epi32(0x00170029);
	const __m256i c16_n30_n36 = _mm256_set1_epi32(0xFFE2FFDC);
	const __m256i c16_n36_p30 = _mm256_set1_epi32(0xFFDC001E);
	const __m256i c16_p41_n23 = _mm256_set1_epi32(0x0029FFE9);
	const __m256i c16_n44_p15 = _mm256_set1_epi32(0xFFD4000F);
	const __m256i c16_p45_n07 = _mm256_set1_epi32(0x002DFFF9);
	const __m256i c16_n45_n02 = _mm256_set1_epi32(0xFFD3FFFE);
	const __m256i c16_p43_p11 = _mm256_set1_epi32(0x002B000B);
	const __m256i c16_n39_n19 = _mm256_set1_epi32(0xFFD9FFED);
	const __m256i c16_p34_p27 = _mm256_set1_epi32(0x0022001B);
	const __m256i c16_n43_p27 = _mm256_set1_epi32(0xFFD5001B);
	const __m256i c16_p44_n02 = _mm256_set1_epi32(0x002CFFFE);
	const __m256i c16_n30_n23 = _mm256_set1_epi32(0xFFE2FFE9);
	const __m256i c16_p07_p41 = _mm256_set1_epi32(0x00070029);
	const __m256i c16_p19_n45 = _mm256_set1_epi32(0x0013FFD3);
	const __m256i c16_n39_p34 = _mm256_set1_epi32(0xFFD90022);
	const __m256i c16_p45_n11 = _mm256_set1_epi32(0x002DFFF5);
	const __m256i c16_n36_n15 = _mm256_set1_epi32(0xFFDCFFF1);
	const __m256i c16_n45_p23 = _mm256_set1_epi32(0xFFD30017);
	const __m256i c16_p27_p19 = _mm256_set1_epi32(0x001B0013);
	const __m256i c16_p15_n45 = _mm256_set1_epi32(0x000FFFD3);
	const __m256i c16_n44_p30 = _mm256_set1_epi32(0xFFD4001E);
	const __m256i c16_p34_p11 = _mm256_set1_epi32(0x0022000B);
	const __m256i c16_p07_n43 = _mm256_set1_epi32(0x0007FFD5);
	const __m256i c16_n41_p36 = _mm256_set1_epi32(0xFFD70024);
	const __m256i c16_p39_p02 = _mm256_set1_epi32(0x00270002);
	const __m256i c16_n44_p19 = _mm256_set1_epi32(0xFFD40013);
	const __m256i c16_n02_p36 = _mm256_set1_epi32(0xFFFE0024);
	const __m256i c16_p45_n34 = _mm256_set1_epi32(0x002DFFDE);
	const __m256i c16_n15_n23 = _mm256_set1_epi32(0xFFF1FFE9);
	const __m256i c16_n39_p43 = _mm256_set1_epi32(0xFFD9002B);
	const __m256i c16_p30_p07 = _mm256_set1_epi32(0x001E0007);
	const __m256i c16_p27_n45 = _mm256_set1_epi32(0x001BFFD3);
	const __m256i c16_n41_p11 = _mm256_set1_epi32(0xFFD7000B);
	const __m256i c16_n39_p15 = _mm256_set1_epi32(0xFFD9000F);
	const __m256i c16_n30_p45 = _mm256_set1_epi32(0xFFE2002D);
	const __m256i c16_p27_p02 = _mm256_set1_epi32(0x001B0002);
	const __m256i c16_p41_n44 = _mm256_set1_epi32(0x0029FFD4);
	const __m256i c16_n11_n19 = _mm256_set1_epi32(0xFFF5FFED);
	const __m256i c16_n45_p36 = _mm256_set1_epi32(0xFFD30024);
	const __m256i c16_n07_p34 = _mm256_set1_epi32(0xFFF90022);
	const __m256i c16_p43_n23 = _mm256_set1_epi32(0x002BFFE9);
	const __m256i c16_n30_p11 = _mm256_set1_epi32(0xFFE2000B);
	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);
	const __m256i c16_n19_p36 = _mm256_set1_epi32(0xFFED0024);
	const __m256i c16_p23_n02 = _mm256_set1_epi32(0x0017FFFE);
	const __m256i c16_p45_n39 = _mm256_set1_epi32(0x002DFFD9);
	const __m256i c16_p27_n41 = _mm256_set1_epi32(0x001BFFD7);
	const __m256i c16_n15_n07 = _mm256_set1_epi32(0xFFF1FFF9);
	const __m256i c16_n44_p34 = _mm256_set1_epi32(0xFFD40022);
	const __m256i c16_n19_p07 = _mm256_set1_epi32(0xFFED0007);
	const __m256i c16_n39_p30 = _mm256_set1_epi32(0xFFD9001E);
	const __m256i c16_n45_p44 = _mm256_set1_epi32(0xFFD3002C);
	const __m256i c16_n36_p43 = _mm256_set1_epi32(0xFFDC002B);
	const __m256i c16_n15_p27 = _mm256_set1_epi32(0xFFF1001B);
	const __m256i c16_p11_p02 = _mm256_set1_epi32(0x000B0002);
	const __m256i c16_p34_n23 = _mm256_set1_epi32(0x0022FFE9);
	const __m256i c16_p45_n41 = _mm256_set1_epi32(0x002DFFD7);
	const __m256i c16_n07_p02 = _mm256_set1_epi32(0xFFF90002);
	const __m256i c16_n15_p11 = _mm256_set1_epi32(0xFFF1000B);
	const __m256i c16_n23_p19 = _mm256_set1_epi32(0xFFE90013);
	const __m256i c16_n30_p27 = _mm256_set1_epi32(0xFFE2001B);
	const __m256i c16_n36_p34 = _mm256_set1_epi32(0xFFDC0022);
	const __m256i c16_n41_p39 = _mm256_set1_epi32(0xFFD70027);
	const __m256i c16_n44_p43 = _mm256_set1_epi32(0xFFD4002B);
	const __m256i c16_n45_p45 = _mm256_set1_epi32(0xFFD3002D);

	//	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	const __m256i c16_p35_p40 = _mm256_set1_epi32(0x00230028);
	const __m256i c16_p21_p29 = _mm256_set1_epi32(0x0015001D);
	const __m256i c16_p04_p13 = _mm256_set1_epi32(0x0004000D);
	const __m256i c16_p29_p43 = _mm256_set1_epi32(0x001D002B);
	const __m256i c16_n21_p04 = _mm256_set1_epi32(0xFFEB0004);
	const __m256i c16_n45_n40 = _mm256_set1_epi32(0xFFD3FFD8);
	const __m256i c16_n13_n35 = _mm256_set1_epi32(0xFFF3FFDD);
	const __m256i c16_p04_p40 = _mm256_set1_epi32(0x00040028);
	const __m256i c16_n43_n35 = _mm256_set1_epi32(0xFFD5FFDD);
	const __m256i c16_p29_n13 = _mm256_set1_epi32(0x001DFFF3);
	const __m256i c16_p21_p45 = _mm256_set1_epi32(0x0015002D);
	const __m256i c16_n21_p35 = _mm256_set1_epi32(0xFFEB0023);
	const __m256i c16_p04_n43 = _mm256_set1_epi32(0x0004FFD5);
	const __m256i c16_p13_p45 = _mm256_set1_epi32(0x000D002D);
	const __m256i c16_n29_n40 = _mm256_set1_epi32(0xFFE3FFD8);
	const __m256i c16_n40_p29 = _mm256_set1_epi32(0xFFD8001D);
	const __m256i c16_p45_n13 = _mm256_set1_epi32(0x002DFFF3);
	const __m256i c16_n43_n04 = _mm256_set1_epi32(0xFFD5FFFC);
	const __m256i c16_p35_p21 = _mm256_set1_epi32(0x00230015);
	const __m256i c16_n45_p21 = _mm256_set1_epi32(0xFFD30015);
	const __m256i c16_p13_p29 = _mm256_set1_epi32(0x000D001D);
	const __m256i c16_p35_n43 = _mm256_set1_epi32(0x0023FFD5);
	const __m256i c16_n40_p04 = _mm256_set1_epi32(0xFFD80004);
	const __m256i c16_n35_p13 = _mm256_set1_epi32(0xFFDD000D);
	const __m256i c16_n40_p45 = _mm256_set1_epi32(0xFFD8002D);
	const __m256i c16_p04_p21 = _mm256_set1_epi32(0x00040015);
	const __m256i c16_p43_n29 = _mm256_set1_epi32(0x002BFFE3);
	const __m256i c16_n13_p04 = _mm256_set1_epi32(0xFFF30004);
	const __m256i c16_n29_p21 = _mm256_set1_epi32(0xFFE30015);
	const __m256i c16_n40_p35 = _mm256_set1_epi32(0xFFD80023);
	//	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);

	const __m256i c16_p38_p44 = _mm256_set1_epi32(0x0026002C);
	const __m256i c16_p09_p25 = _mm256_set1_epi32(0x00090019);
	const __m256i c16_n09_p38 = _mm256_set1_epi32(0xFFF70026);
	const __m256i c16_n25_n44 = _mm256_set1_epi32(0xFFE7FFD4);

	const __m256i c16_n44_p25 = _mm256_set1_epi32(0xFFD40019);
	const __m256i c16_p38_p09 = _mm256_set1_epi32(0x00260009);
	const __m256i c16_n25_p09 = _mm256_set1_epi32(0xFFE70009);
	const __m256i c16_n44_p38 = _mm256_set1_epi32(0xFFD40026);

	const __m256i c16_p17_p42 = _mm256_set1_epi32(0x0011002A);
	const __m256i c16_n42_p17 = _mm256_set1_epi32(0xFFD60011);

	const __m256i c16_p32_p32 = _mm256_set1_epi32(0x00200020);
	const __m256i c16_n32_p32 = _mm256_set1_epi32(0xFFE00020);

	__m256i zero = _mm256_setzero_si256();
	__m256i c32_rnd = _mm256_set1_epi32(16);
	int nShift = 5;

	// DCT1
	__m256i in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2];
	__m256i in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2];
	__m256i res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2];
	__m256i res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2];

	int part;
	for (i = 0; i < 2; i++)
	{
		const int offset = (i << 4);
		in00[i] = _mm256_lddqu_si256((const __m256i*)&src[0 * 32 + offset]);
		in01[i] = _mm256_lddqu_si256((const __m256i*)&src[1 * 32 + offset]);
		in02[i] = _mm256_lddqu_si256((const __m256i*)&src[2 * 32 + offset]);
		in03[i] = _mm256_lddqu_si256((const __m256i*)&src[3 * 32 + offset]);
		in04[i] = _mm256_lddqu_si256((const __m256i*)&src[4 * 32 + offset]);
		in05[i] = _mm256_lddqu_si256((const __m256i*)&src[5 * 32 + offset]);
		in06[i] = _mm256_lddqu_si256((const __m256i*)&src[6 * 32 + offset]);
		in07[i] = _mm256_lddqu_si256((const __m256i*)&src[7 * 32 + offset]);
		in08[i] = _mm256_lddqu_si256((const __m256i*)&src[8 * 32 + offset]);
		in09[i] = _mm256_lddqu_si256((const __m256i*)&src[9 * 32 + offset]);
		in10[i] = _mm256_lddqu_si256((const __m256i*)&src[10 * 32 + offset]);
		in11[i] = _mm256_lddqu_si256((const __m256i*)&src[11 * 32 + offset]);
		in12[i] = _mm256_lddqu_si256((const __m256i*)&src[12 * 32 + offset]);
		in13[i] = _mm256_lddqu_si256((const __m256i*)&src[13 * 32 + offset]);
		in14[i] = _mm256_lddqu_si256((const __m256i*)&src[14 * 32 + offset]);
		in15[i] = _mm256_lddqu_si256((const __m256i*)&src[15 * 32 + offset]);
		in16[i] = _mm256_lddqu_si256((const __m256i*)&src[16 * 32 + offset]);
		in17[i] = _mm256_lddqu_si256((const __m256i*)&src[17 * 32 + offset]);
		in18[i] = _mm256_lddqu_si256((const __m256i*)&src[18 * 32 + offset]);
		in19[i] = _mm256_lddqu_si256((const __m256i*)&src[19 * 32 + offset]);
		in20[i] = _mm256_lddqu_si256((const __m256i*)&src[20 * 32 + offset]);
		in21[i] = _mm256_lddqu_si256((const __m256i*)&src[21 * 32 + offset]);
		in22[i] = _mm256_lddqu_si256((const __m256i*)&src[22 * 32 + offset]);
		in23[i] = _mm256_lddqu_si256((const __m256i*)&src[23 * 32 + offset]);
		in24[i] = _mm256_lddqu_si256((const __m256i*)&src[24 * 32 + offset]);
		in25[i] = _mm256_lddqu_si256((const __m256i*)&src[25 * 32 + offset]);
		in26[i] = _mm256_lddqu_si256((const __m256i*)&src[26 * 32 + offset]);
		in27[i] = _mm256_lddqu_si256((const __m256i*)&src[27 * 32 + offset]);
		in28[i] = _mm256_lddqu_si256((const __m256i*)&src[28 * 32 + offset]);
		in29[i] = _mm256_lddqu_si256((const __m256i*)&src[29 * 32 + offset]);
		in30[i] = _mm256_lddqu_si256((const __m256i*)&src[30 * 32 + offset]);
		in31[i] = _mm256_lddqu_si256((const __m256i*)&src[31 * 32 + offset]);
	}

	{
		for (part = 0; part < 2; part++)
		{
			const __m256i T_00_00A = _mm256_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
			const __m256i T_00_00B = _mm256_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
			const __m256i T_00_01A = _mm256_unpacklo_epi16(in05[part], in07[part]);       // [ ]
			const __m256i T_00_01B = _mm256_unpackhi_epi16(in05[part], in07[part]);       // [ ]
			const __m256i T_00_02A = _mm256_unpacklo_epi16(in09[part], in11[part]);       // [ ]
			const __m256i T_00_02B = _mm256_unpackhi_epi16(in09[part], in11[part]);       // [ ]
			const __m256i T_00_03A = _mm256_unpacklo_epi16(in13[part], in15[part]);       // [ ]
			const __m256i T_00_03B = _mm256_unpackhi_epi16(in13[part], in15[part]);       // [ ]
			const __m256i T_00_04A = _mm256_unpacklo_epi16(in17[part], in19[part]);       // [ ]
			const __m256i T_00_04B = _mm256_unpackhi_epi16(in17[part], in19[part]);       // [ ]
			const __m256i T_00_05A = _mm256_unpacklo_epi16(in21[part], in23[part]);       // [ ]
			const __m256i T_00_05B = _mm256_unpackhi_epi16(in21[part], in23[part]);       // [ ]
			const __m256i T_00_06A = _mm256_unpacklo_epi16(in25[part], in27[part]);       // [ ]
			const __m256i T_00_06B = _mm256_unpackhi_epi16(in25[part], in27[part]);       // [ ]
			const __m256i T_00_07A = _mm256_unpacklo_epi16(in29[part], in31[part]);       //
			const __m256i T_00_07B = _mm256_unpackhi_epi16(in29[part], in31[part]);       // [ ]

			const __m256i T_00_08A = _mm256_unpacklo_epi16(in02[part], in06[part]);       // [ ]
			const __m256i T_00_08B = _mm256_unpackhi_epi16(in02[part], in06[part]);       // [ ]
			const __m256i T_00_09A = _mm256_unpacklo_epi16(in10[part], in14[part]);       // [ ]
			const __m256i T_00_09B = _mm256_unpackhi_epi16(in10[part], in14[part]);       // [ ]
			const __m256i T_00_10A = _mm256_unpacklo_epi16(in18[part], in22[part]);       // [ ]
			const __m256i T_00_10B = _mm256_unpackhi_epi16(in18[part], in22[part]);       // [ ]
			const __m256i T_00_11A = _mm256_unpacklo_epi16(in26[part], in30[part]);       // [ ]
			const __m256i T_00_11B = _mm256_unpackhi_epi16(in26[part], in30[part]);       // [ ]

			const __m256i T_00_12A = _mm256_unpacklo_epi16(in04[part], in12[part]);       // [ ]
			const __m256i T_00_12B = _mm256_unpackhi_epi16(in04[part], in12[part]);       // [ ]
			const __m256i T_00_13A = _mm256_unpacklo_epi16(in20[part], in28[part]);       // [ ]
			const __m256i T_00_13B = _mm256_unpackhi_epi16(in20[part], in28[part]);       // [ ]

			const __m256i T_00_14A = _mm256_unpacklo_epi16(in08[part], in24[part]);       //
			const __m256i T_00_14B = _mm256_unpackhi_epi16(in08[part], in24[part]);       // [ ]
			const __m256i T_00_15A = _mm256_unpacklo_epi16(in00[part], in16[part]);       //
			const __m256i T_00_15B = _mm256_unpackhi_epi16(in00[part], in16[part]);       // [ ]

			__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
			__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
			__m256i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
			__m256i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
			{
				__m256i T00, T01, T02, T03;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, r1719, r2123, r2527, r2931, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
					T00 = _mm256_add_epi32(_mm256_madd_epi16(r0103, c0103), _mm256_madd_epi16(r0507, c0507)); \
					T01 = _mm256_add_epi32(_mm256_madd_epi16(r0911, c0911), _mm256_madd_epi16(r1315, c1315)); \
					T02 = _mm256_add_epi32(_mm256_madd_epi16(r1719, c1719), _mm256_madd_epi16(r2123, c2123)); \
					T03 = _mm256_add_epi32(_mm256_madd_epi16(r2527, c2527), _mm256_madd_epi16(r2931, c2931)); \
					row = _mm256_add_epi32(_mm256_add_epi32(T00, T01), _mm256_add_epi32(T02, T03));

				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A);

				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B);

#undef COMPUTE_ROW
			}


			{
				__m256i T00, T01;
#define COMPUTE_ROW(row0206, row1014, row1822, row2630, c0206, c1014, c1822, c2630, row) \
					T00 = _mm256_add_epi32(_mm256_madd_epi16(row0206, c0206), _mm256_madd_epi16(row1014, c1014)); \
					T01 = _mm256_add_epi32(_mm256_madd_epi16(row1822, c1822), _mm256_madd_epi16(row2630, c2630)); \
					row = _mm256_add_epi32(T00, T01);

				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A);

				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B);
#undef COMPUTE_ROW
			}

			EEO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_p38_p44), _mm256_madd_epi16(T_00_13A, c16_p09_p25));
			EEO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n09_p38), _mm256_madd_epi16(T_00_13A, c16_n25_n44));
			EEO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n44_p25), _mm256_madd_epi16(T_00_13A, c16_p38_p09));
			EEO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n25_p09), _mm256_madd_epi16(T_00_13A, c16_n44_p38));
			EEO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_p38_p44), _mm256_madd_epi16(T_00_13B, c16_p09_p25));
			EEO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n09_p38), _mm256_madd_epi16(T_00_13B, c16_n25_n44));
			EEO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n44_p25), _mm256_madd_epi16(T_00_13B, c16_p38_p09));
			EEO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n25_p09), _mm256_madd_epi16(T_00_13B, c16_n44_p38));

			EEEO0A = _mm256_madd_epi16(T_00_14A, c16_p17_p42);
			EEEO0B = _mm256_madd_epi16(T_00_14B, c16_p17_p42);
			EEEO1A = _mm256_madd_epi16(T_00_14A, c16_n42_p17);
			EEEO1B = _mm256_madd_epi16(T_00_14B, c16_n42_p17);

			EEEE0A = _mm256_madd_epi16(T_00_15A, c16_p32_p32);
			EEEE0B = _mm256_madd_epi16(T_00_15B, c16_p32_p32);
			EEEE1A = _mm256_madd_epi16(T_00_15A, c16_n32_p32);
			EEEE1B = _mm256_madd_epi16(T_00_15B, c16_n32_p32);

			EEE0A = _mm256_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
			EEE0B = _mm256_add_epi32(EEEE0B, EEEO0B);
			EEE1A = _mm256_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
			EEE1B = _mm256_add_epi32(EEEE1B, EEEO1B);
			EEE3A = _mm256_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
			EEE3B = _mm256_sub_epi32(EEEE0B, EEEO0B);
			EEE2A = _mm256_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
			EEE2B = _mm256_sub_epi32(EEEE1B, EEEO1B);

			EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
			EE0B = _mm256_add_epi32(EEE0B, EEO0B);
			EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
			EE1B = _mm256_add_epi32(EEE1B, EEO1B);
			EE2A = _mm256_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
			EE2B = _mm256_add_epi32(EEE2B, EEO2B);
			EE3A = _mm256_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
			EE3B = _mm256_add_epi32(EEE3B, EEO3B);
			EE7A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
			EE7B = _mm256_sub_epi32(EEE0B, EEO0B);
			EE6A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
			EE6B = _mm256_sub_epi32(EEE1B, EEO1B);
			EE5A = _mm256_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
			EE5B = _mm256_sub_epi32(EEE2B, EEO2B);
			EE4A = _mm256_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
			EE4B = _mm256_sub_epi32(EEE3B, EEO3B);

			E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
			E0B = _mm256_add_epi32(EE0B, EO0B);
			E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
			E1B = _mm256_add_epi32(EE1B, EO1B);
			E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
			E2B = _mm256_add_epi32(EE2B, EO2B);
			E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
			E3B = _mm256_add_epi32(EE3B, EO3B);
			E4A = _mm256_add_epi32(EE4A, EO4A);          // E4 =
			E4B = _mm256_add_epi32(EE4B, EO4B);
			E5A = _mm256_add_epi32(EE5A, EO5A);          // E5 =
			E5B = _mm256_add_epi32(EE5B, EO5B);
			E6A = _mm256_add_epi32(EE6A, EO6A);          // E6 =
			E6B = _mm256_add_epi32(EE6B, EO6B);
			E7A = _mm256_add_epi32(EE7A, EO7A);          // E7 =
			E7B = _mm256_add_epi32(EE7B, EO7B);
			EFA = _mm256_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
			EFB = _mm256_sub_epi32(EE0B, EO0B);
			EEA = _mm256_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
			EEB = _mm256_sub_epi32(EE1B, EO1B);
			EDA = _mm256_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
			EDB = _mm256_sub_epi32(EE2B, EO2B);
			ECA = _mm256_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
			ECB = _mm256_sub_epi32(EE3B, EO3B);
			EBA = _mm256_sub_epi32(EE4A, EO4A);          // EB =
			EBB = _mm256_sub_epi32(EE4B, EO4B);
			EAA = _mm256_sub_epi32(EE5A, EO5A);          // EA =
			EAB = _mm256_sub_epi32(EE5B, EO5B);
			E9A = _mm256_sub_epi32(EE6A, EO6A);          // E9 =
			E9B = _mm256_sub_epi32(EE6B, EO6B);
			E8A = _mm256_sub_epi32(EE7A, EO7A);          // E8 =
			E8B = _mm256_sub_epi32(EE7B, EO7B);

			T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
			T10B = _mm256_add_epi32(E0B, c32_rnd);
			T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
			T11B = _mm256_add_epi32(E1B, c32_rnd);
			T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
			T12B = _mm256_add_epi32(E2B, c32_rnd);
			T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
			T13B = _mm256_add_epi32(E3B, c32_rnd);
			T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
			T14B = _mm256_add_epi32(E4B, c32_rnd);
			T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
			T15B = _mm256_add_epi32(E5B, c32_rnd);
			T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
			T16B = _mm256_add_epi32(E6B, c32_rnd);
			T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
			T17B = _mm256_add_epi32(E7B, c32_rnd);
			T18A = _mm256_add_epi32(E8A, c32_rnd);         // E8 + rnd
			T18B = _mm256_add_epi32(E8B, c32_rnd);
			T19A = _mm256_add_epi32(E9A, c32_rnd);         // E9 + rnd
			T19B = _mm256_add_epi32(E9B, c32_rnd);
			T1AA = _mm256_add_epi32(EAA, c32_rnd);         // E10 + rnd
			T1AB = _mm256_add_epi32(EAB, c32_rnd);
			T1BA = _mm256_add_epi32(EBA, c32_rnd);         // E11 + rnd
			T1BB = _mm256_add_epi32(EBB, c32_rnd);
			T1CA = _mm256_add_epi32(ECA, c32_rnd);         // E12 + rnd
			T1CB = _mm256_add_epi32(ECB, c32_rnd);
			T1DA = _mm256_add_epi32(EDA, c32_rnd);         // E13 + rnd
			T1DB = _mm256_add_epi32(EDB, c32_rnd);
			T1EA = _mm256_add_epi32(EEA, c32_rnd);         // E14 + rnd
			T1EB = _mm256_add_epi32(EEB, c32_rnd);
			T1FA = _mm256_add_epi32(EFA, c32_rnd);         // E15 + rnd
			T1FB = _mm256_add_epi32(EFB, c32_rnd);

			T2_00A = _mm256_add_epi32(T10A, O00A);          // E0 + O0 + rnd
			T2_00B = _mm256_add_epi32(T10B, O00B);
			T2_01A = _mm256_add_epi32(T11A, O01A);          // E1 + O1 + rnd
			T2_01B = _mm256_add_epi32(T11B, O01B);
			T2_02A = _mm256_add_epi32(T12A, O02A);          // E2 + O2 + rnd
			T2_02B = _mm256_add_epi32(T12B, O02B);
			T2_03A = _mm256_add_epi32(T13A, O03A);          // E3 + O3 + rnd
			T2_03B = _mm256_add_epi32(T13B, O03B);
			T2_04A = _mm256_add_epi32(T14A, O04A);          // E4
			T2_04B = _mm256_add_epi32(T14B, O04B);
			T2_05A = _mm256_add_epi32(T15A, O05A);          // E5
			T2_05B = _mm256_add_epi32(T15B, O05B);
			T2_06A = _mm256_add_epi32(T16A, O06A);          // E6
			T2_06B = _mm256_add_epi32(T16B, O06B);
			T2_07A = _mm256_add_epi32(T17A, O07A);          // E7
			T2_07B = _mm256_add_epi32(T17B, O07B);
			T2_08A = _mm256_add_epi32(T18A, O08A);          // E8
			T2_08B = _mm256_add_epi32(T18B, O08B);
			T2_09A = _mm256_add_epi32(T19A, O09A);          // E9
			T2_09B = _mm256_add_epi32(T19B, O09B);
			T2_10A = _mm256_add_epi32(T1AA, O10A);          // E10
			T2_10B = _mm256_add_epi32(T1AB, O10B);
			T2_11A = _mm256_add_epi32(T1BA, O11A);          // E11
			T2_11B = _mm256_add_epi32(T1BB, O11B);
			T2_12A = _mm256_add_epi32(T1CA, O12A);          // E12
			T2_12B = _mm256_add_epi32(T1CB, O12B);
			T2_13A = _mm256_add_epi32(T1DA, O13A);          // E13
			T2_13B = _mm256_add_epi32(T1DB, O13B);
			T2_14A = _mm256_add_epi32(T1EA, O14A);          // E14
			T2_14B = _mm256_add_epi32(T1EB, O14B);
			T2_15A = _mm256_add_epi32(T1FA, O15A);          // E15
			T2_15B = _mm256_add_epi32(T1FB, O15B);
			T2_31A = _mm256_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
			T2_31B = _mm256_sub_epi32(T10B, O00B);
			T2_30A = _mm256_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
			T2_30B = _mm256_sub_epi32(T11B, O01B);
			T2_29A = _mm256_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
			T2_29B = _mm256_sub_epi32(T12B, O02B);
			T2_28A = _mm256_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
			T2_28B = _mm256_sub_epi32(T13B, O03B);
			T2_27A = _mm256_sub_epi32(T14A, O04A);          // E4
			T2_27B = _mm256_sub_epi32(T14B, O04B);
			T2_26A = _mm256_sub_epi32(T15A, O05A);          // E5
			T2_26B = _mm256_sub_epi32(T15B, O05B);
			T2_25A = _mm256_sub_epi32(T16A, O06A);          // E6
			T2_25B = _mm256_sub_epi32(T16B, O06B);
			T2_24A = _mm256_sub_epi32(T17A, O07A);          // E7
			T2_24B = _mm256_sub_epi32(T17B, O07B);
			T2_23A = _mm256_sub_epi32(T18A, O08A);          //
			T2_23B = _mm256_sub_epi32(T18B, O08B);
			T2_22A = _mm256_sub_epi32(T19A, O09A);          //
			T2_22B = _mm256_sub_epi32(T19B, O09B);
			T2_21A = _mm256_sub_epi32(T1AA, O10A);          //
			T2_21B = _mm256_sub_epi32(T1AB, O10B);
			T2_20A = _mm256_sub_epi32(T1BA, O11A);          //
			T2_20B = _mm256_sub_epi32(T1BB, O11B);
			T2_19A = _mm256_sub_epi32(T1CA, O12A);          //
			T2_19B = _mm256_sub_epi32(T1CB, O12B);
			T2_18A = _mm256_sub_epi32(T1DA, O13A);          //
			T2_18B = _mm256_sub_epi32(T1DB, O13B);
			T2_17A = _mm256_sub_epi32(T1EA, O14A);          //
			T2_17B = _mm256_sub_epi32(T1EB, O14B);
			T2_16A = _mm256_sub_epi32(T1FA, O15A);          //
			T2_16B = _mm256_sub_epi32(T1FB, O15B);

			T3_00A = _mm256_srai_epi32(T2_00A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			T3_00B = _mm256_srai_epi32(T2_00B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			T3_01A = _mm256_srai_epi32(T2_01A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			T3_01B = _mm256_srai_epi32(T2_01B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
			T3_02A = _mm256_srai_epi32(T2_02A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
			T3_02B = _mm256_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
			T3_03A = _mm256_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
			T3_03B = _mm256_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
			T3_04A = _mm256_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
			T3_04B = _mm256_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
			T3_05A = _mm256_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
			T3_05B = _mm256_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
			T3_06A = _mm256_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
			T3_06B = _mm256_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
			T3_07A = _mm256_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
			T3_07B = _mm256_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
			T3_08A = _mm256_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
			T3_08B = _mm256_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
			T3_09A = _mm256_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
			T3_09B = _mm256_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
			T3_10A = _mm256_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
			T3_10B = _mm256_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
			T3_11A = _mm256_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
			T3_11B = _mm256_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
			T3_12A = _mm256_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
			T3_12B = _mm256_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
			T3_13A = _mm256_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
			T3_13B = _mm256_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
			T3_14A = _mm256_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
			T3_14B = _mm256_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
			T3_15A = _mm256_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
			T3_15B = _mm256_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

			T3_16A = _mm256_srai_epi32(T2_16A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			T3_16B = _mm256_srai_epi32(T2_16B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			T3_17A = _mm256_srai_epi32(T2_17A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			T3_17B = _mm256_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
			T3_18A = _mm256_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
			T3_18B = _mm256_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
			T3_19A = _mm256_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
			T3_19B = _mm256_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
			T3_20A = _mm256_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
			T3_20B = _mm256_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
			T3_21A = _mm256_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
			T3_21B = _mm256_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
			T3_22A = _mm256_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
			T3_22B = _mm256_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
			T3_23A = _mm256_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
			T3_23B = _mm256_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
			T3_24A = _mm256_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
			T3_24B = _mm256_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
			T3_25A = _mm256_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
			T3_25B = _mm256_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
			T3_26A = _mm256_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
			T3_26B = _mm256_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
			T3_27A = _mm256_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
			T3_27B = _mm256_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
			T3_28A = _mm256_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
			T3_28B = _mm256_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
			T3_29A = _mm256_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
			T3_29B = _mm256_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
			T3_30A = _mm256_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
			T3_30B = _mm256_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
			T3_31A = _mm256_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
			T3_31B = _mm256_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

			res00[part] = _mm256_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
			res01[part] = _mm256_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
			res02[part] = _mm256_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
			res03[part] = _mm256_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
			res04[part] = _mm256_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
			res05[part] = _mm256_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
			res06[part] = _mm256_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
			res07[part] = _mm256_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
			res08[part] = _mm256_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
			res09[part] = _mm256_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
			res10[part] = _mm256_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
			res11[part] = _mm256_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
			res12[part] = _mm256_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
			res13[part] = _mm256_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
			res14[part] = _mm256_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
			res15[part] = _mm256_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
			res16[part] = _mm256_packs_epi32(T3_16A, T3_16B);
			res17[part] = _mm256_packs_epi32(T3_17A, T3_17B);
			res18[part] = _mm256_packs_epi32(T3_18A, T3_18B);
			res19[part] = _mm256_packs_epi32(T3_19A, T3_19B);
			res20[part] = _mm256_packs_epi32(T3_20A, T3_20B);
			res21[part] = _mm256_packs_epi32(T3_21A, T3_21B);
			res22[part] = _mm256_packs_epi32(T3_22A, T3_22B);
			res23[part] = _mm256_packs_epi32(T3_23A, T3_23B);
			res24[part] = _mm256_packs_epi32(T3_24A, T3_24B);
			res25[part] = _mm256_packs_epi32(T3_25A, T3_25B);
			res26[part] = _mm256_packs_epi32(T3_26A, T3_26B);
			res27[part] = _mm256_packs_epi32(T3_27A, T3_27B);
			res28[part] = _mm256_packs_epi32(T3_28A, T3_28B);
			res29[part] = _mm256_packs_epi32(T3_29A, T3_29B);
			res30[part] = _mm256_packs_epi32(T3_30A, T3_30B);
			res31[part] = _mm256_packs_epi32(T3_31A, T3_31B);

		}

		//transpose 32x32 matrix
		{
			__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
				tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
				tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
				tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
				tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
				tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
				tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
				tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
				tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
				tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
				tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
				tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
				tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
				tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
				tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
				tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
				tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
				O0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
				O1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
				O2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
				O3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
				O4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
				O5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
				O6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
				O7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
				O8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
				O9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
				O10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
				O11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
				O12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
				O13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
				O14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
				O15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
				tr0_0 = _mm256_unpacklo_epi64(O0, O1); \
				tr0_1 = _mm256_unpacklo_epi64(O2, O3); \
				tr0_2 = _mm256_unpackhi_epi64(O0, O1); \
				tr0_3 = _mm256_unpackhi_epi64(O2, O3); \
				tr0_4 = _mm256_unpacklo_epi64(O4, O5); \
				tr0_5 = _mm256_unpacklo_epi64(O6, O7); \
				tr0_6 = _mm256_unpackhi_epi64(O4, O5); \
				tr0_7 = _mm256_unpackhi_epi64(O6, O7); \
				tr0_8 = _mm256_unpacklo_epi64(O8, O9); \
				tr0_9 = _mm256_unpacklo_epi64(O10, O11); \
				tr0_10 = _mm256_unpackhi_epi64(O8, O9); \
				tr0_11 = _mm256_unpackhi_epi64(O10, O11); \
				tr0_12 = _mm256_unpacklo_epi64(O12, O13); \
				tr0_13 = _mm256_unpacklo_epi64(O14, O15); \
				tr0_14 = _mm256_unpackhi_epi64(O12, O13); \
				tr0_15 = _mm256_unpackhi_epi64(O14, O15); \
				O0 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x20); \
				O1 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x20); \
				O2 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x20); \
				O3 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x20); \
				O4 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x20); \
				O5 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x20); \
				O6 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x20); \
				O7 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x20); \
				O8 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x31); \
				O9 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x31); \
				O10 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x31); \
				O11 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x31); \
				O12 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x31); \
				O13 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x31); \
				O14 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x31); \
				O15 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x31); \

			TRANSPOSE_16x16_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
			TRANSPOSE_16x16_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);
			TRANSPOSE_16x16_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0]);
			TRANSPOSE_16x16_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1]);

#undef TRANSPOSE_16x16_16BIT
		}
	}

	c32_rnd = _mm256_set1_epi32(2048);				// pass == 1 第二次四舍五入
	nShift = 12;

	{
		for (part = 0; part < 2; part++)
		{
			const __m256i T_00_00A = _mm256_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
			const __m256i T_00_00B = _mm256_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
			const __m256i T_00_01A = _mm256_unpacklo_epi16(in05[part], in07[part]);       // [ ]
			const __m256i T_00_01B = _mm256_unpackhi_epi16(in05[part], in07[part]);       // [ ]
			const __m256i T_00_02A = _mm256_unpacklo_epi16(in09[part], in11[part]);       // [ ]
			const __m256i T_00_02B = _mm256_unpackhi_epi16(in09[part], in11[part]);       // [ ]
			const __m256i T_00_03A = _mm256_unpacklo_epi16(in13[part], in15[part]);       // [ ]
			const __m256i T_00_03B = _mm256_unpackhi_epi16(in13[part], in15[part]);       // [ ]
			const __m256i T_00_04A = _mm256_unpacklo_epi16(in17[part], in19[part]);       // [ ]
			const __m256i T_00_04B = _mm256_unpackhi_epi16(in17[part], in19[part]);       // [ ]
			const __m256i T_00_05A = _mm256_unpacklo_epi16(in21[part], in23[part]);       // [ ]
			const __m256i T_00_05B = _mm256_unpackhi_epi16(in21[part], in23[part]);       // [ ]
			const __m256i T_00_06A = _mm256_unpacklo_epi16(in25[part], in27[part]);       // [ ]
			const __m256i T_00_06B = _mm256_unpackhi_epi16(in25[part], in27[part]);       // [ ]
			const __m256i T_00_07A = _mm256_unpacklo_epi16(in29[part], in31[part]);       //
			const __m256i T_00_07B = _mm256_unpackhi_epi16(in29[part], in31[part]);       // [ ]

			const __m256i T_00_08A = _mm256_unpacklo_epi16(in02[part], in06[part]);       // [ ]
			const __m256i T_00_08B = _mm256_unpackhi_epi16(in02[part], in06[part]);       // [ ]
			const __m256i T_00_09A = _mm256_unpacklo_epi16(in10[part], in14[part]);       // [ ]
			const __m256i T_00_09B = _mm256_unpackhi_epi16(in10[part], in14[part]);       // [ ]
			const __m256i T_00_10A = _mm256_unpacklo_epi16(in18[part], in22[part]);       // [ ]
			const __m256i T_00_10B = _mm256_unpackhi_epi16(in18[part], in22[part]);       // [ ]
			const __m256i T_00_11A = _mm256_unpacklo_epi16(in26[part], in30[part]);       // [ ]
			const __m256i T_00_11B = _mm256_unpackhi_epi16(in26[part], in30[part]);       // [ ]

			const __m256i T_00_12A = _mm256_unpacklo_epi16(in04[part], in12[part]);       // [ ]
			const __m256i T_00_12B = _mm256_unpackhi_epi16(in04[part], in12[part]);       // [ ]
			const __m256i T_00_13A = _mm256_unpacklo_epi16(in20[part], in28[part]);       // [ ]
			const __m256i T_00_13B = _mm256_unpackhi_epi16(in20[part], in28[part]);       // [ ]

			const __m256i T_00_14A = _mm256_unpacklo_epi16(in08[part], in24[part]);       //
			const __m256i T_00_14B = _mm256_unpackhi_epi16(in08[part], in24[part]);       // [ ]
			const __m256i T_00_15A = _mm256_unpacklo_epi16(in00[part], in16[part]);       //
			const __m256i T_00_15B = _mm256_unpackhi_epi16(in00[part], in16[part]);       // [ ]

			__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
			__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
			__m256i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
			__m256i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
			{
				__m256i T00, T01, T02, T03;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, r1719, r2123, r2527, r2931, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
					T00 = _mm256_add_epi32(_mm256_madd_epi16(r0103, c0103), _mm256_madd_epi16(r0507, c0507)); \
					T01 = _mm256_add_epi32(_mm256_madd_epi16(r0911, c0911), _mm256_madd_epi16(r1315, c1315)); \
					T02 = _mm256_add_epi32(_mm256_madd_epi16(r1719, c1719), _mm256_madd_epi16(r2123, c2123)); \
					T03 = _mm256_add_epi32(_mm256_madd_epi16(r2527, c2527), _mm256_madd_epi16(r2931, c2931)); \
					row = _mm256_add_epi32(_mm256_add_epi32(T00, T01), _mm256_add_epi32(T02, T03));

				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A);

				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B);

#undef COMPUTE_ROW
			}

			{
				__m256i T00, T01;
#define COMPUTE_ROW(row0206, row1014, row1822, row2630, c0206, c1014, c1822, c2630, row) \
					T00 = _mm256_add_epi32(_mm256_madd_epi16(row0206, c0206), _mm256_madd_epi16(row1014, c1014)); \
					T01 = _mm256_add_epi32(_mm256_madd_epi16(row1822, c1822), _mm256_madd_epi16(row2630, c2630)); \
					row = _mm256_add_epi32(T00, T01);

				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A);

				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B);
#undef COMPUTE_ROW
			}

			EEO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_p38_p44), _mm256_madd_epi16(T_00_13A, c16_p09_p25));
			EEO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n09_p38), _mm256_madd_epi16(T_00_13A, c16_n25_n44));
			EEO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n44_p25), _mm256_madd_epi16(T_00_13A, c16_p38_p09));
			EEO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n25_p09), _mm256_madd_epi16(T_00_13A, c16_n44_p38));
			EEO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_p38_p44), _mm256_madd_epi16(T_00_13B, c16_p09_p25));
			EEO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n09_p38), _mm256_madd_epi16(T_00_13B, c16_n25_n44));
			EEO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n44_p25), _mm256_madd_epi16(T_00_13B, c16_p38_p09));
			EEO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n25_p09), _mm256_madd_epi16(T_00_13B, c16_n44_p38));

			EEEO0A = _mm256_madd_epi16(T_00_14A, c16_p17_p42);
			EEEO0B = _mm256_madd_epi16(T_00_14B, c16_p17_p42);
			EEEO1A = _mm256_madd_epi16(T_00_14A, c16_n42_p17);
			EEEO1B = _mm256_madd_epi16(T_00_14B, c16_n42_p17);

			EEEE0A = _mm256_madd_epi16(T_00_15A, c16_p32_p32);
			EEEE0B = _mm256_madd_epi16(T_00_15B, c16_p32_p32);
			EEEE1A = _mm256_madd_epi16(T_00_15A, c16_n32_p32);
			EEEE1B = _mm256_madd_epi16(T_00_15B, c16_n32_p32);

			EEE0A = _mm256_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
			EEE0B = _mm256_add_epi32(EEEE0B, EEEO0B);
			EEE1A = _mm256_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
			EEE1B = _mm256_add_epi32(EEEE1B, EEEO1B);
			EEE3A = _mm256_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
			EEE3B = _mm256_sub_epi32(EEEE0B, EEEO0B);
			EEE2A = _mm256_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
			EEE2B = _mm256_sub_epi32(EEEE1B, EEEO1B);

			EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
			EE0B = _mm256_add_epi32(EEE0B, EEO0B);
			EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
			EE1B = _mm256_add_epi32(EEE1B, EEO1B);
			EE2A = _mm256_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
			EE2B = _mm256_add_epi32(EEE2B, EEO2B);
			EE3A = _mm256_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
			EE3B = _mm256_add_epi32(EEE3B, EEO3B);
			EE7A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
			EE7B = _mm256_sub_epi32(EEE0B, EEO0B);
			EE6A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
			EE6B = _mm256_sub_epi32(EEE1B, EEO1B);
			EE5A = _mm256_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
			EE5B = _mm256_sub_epi32(EEE2B, EEO2B);
			EE4A = _mm256_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
			EE4B = _mm256_sub_epi32(EEE3B, EEO3B);

			E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
			E0B = _mm256_add_epi32(EE0B, EO0B);
			E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
			E1B = _mm256_add_epi32(EE1B, EO1B);
			E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
			E2B = _mm256_add_epi32(EE2B, EO2B);
			E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
			E3B = _mm256_add_epi32(EE3B, EO3B);
			E4A = _mm256_add_epi32(EE4A, EO4A);          // E4 =
			E4B = _mm256_add_epi32(EE4B, EO4B);
			E5A = _mm256_add_epi32(EE5A, EO5A);          // E5 =
			E5B = _mm256_add_epi32(EE5B, EO5B);
			E6A = _mm256_add_epi32(EE6A, EO6A);          // E6 =
			E6B = _mm256_add_epi32(EE6B, EO6B);
			E7A = _mm256_add_epi32(EE7A, EO7A);          // E7 =
			E7B = _mm256_add_epi32(EE7B, EO7B);
			EFA = _mm256_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
			EFB = _mm256_sub_epi32(EE0B, EO0B);
			EEA = _mm256_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
			EEB = _mm256_sub_epi32(EE1B, EO1B);
			EDA = _mm256_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
			EDB = _mm256_sub_epi32(EE2B, EO2B);
			ECA = _mm256_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
			ECB = _mm256_sub_epi32(EE3B, EO3B);
			EBA = _mm256_sub_epi32(EE4A, EO4A);          // EB =
			EBB = _mm256_sub_epi32(EE4B, EO4B);
			EAA = _mm256_sub_epi32(EE5A, EO5A);          // EA =
			EAB = _mm256_sub_epi32(EE5B, EO5B);
			E9A = _mm256_sub_epi32(EE6A, EO6A);          // E9 =
			E9B = _mm256_sub_epi32(EE6B, EO6B);
			E8A = _mm256_sub_epi32(EE7A, EO7A);          // E8 =
			E8B = _mm256_sub_epi32(EE7B, EO7B);

			T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
			T10B = _mm256_add_epi32(E0B, c32_rnd);
			T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
			T11B = _mm256_add_epi32(E1B, c32_rnd);
			T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
			T12B = _mm256_add_epi32(E2B, c32_rnd);
			T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
			T13B = _mm256_add_epi32(E3B, c32_rnd);
			T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
			T14B = _mm256_add_epi32(E4B, c32_rnd);
			T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
			T15B = _mm256_add_epi32(E5B, c32_rnd);
			T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
			T16B = _mm256_add_epi32(E6B, c32_rnd);
			T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
			T17B = _mm256_add_epi32(E7B, c32_rnd);
			T18A = _mm256_add_epi32(E8A, c32_rnd);         // E8 + rnd
			T18B = _mm256_add_epi32(E8B, c32_rnd);
			T19A = _mm256_add_epi32(E9A, c32_rnd);         // E9 + rnd
			T19B = _mm256_add_epi32(E9B, c32_rnd);
			T1AA = _mm256_add_epi32(EAA, c32_rnd);         // E10 + rnd
			T1AB = _mm256_add_epi32(EAB, c32_rnd);
			T1BA = _mm256_add_epi32(EBA, c32_rnd);         // E11 + rnd
			T1BB = _mm256_add_epi32(EBB, c32_rnd);
			T1CA = _mm256_add_epi32(ECA, c32_rnd);         // E12 + rnd
			T1CB = _mm256_add_epi32(ECB, c32_rnd);
			T1DA = _mm256_add_epi32(EDA, c32_rnd);         // E13 + rnd
			T1DB = _mm256_add_epi32(EDB, c32_rnd);
			T1EA = _mm256_add_epi32(EEA, c32_rnd);         // E14 + rnd
			T1EB = _mm256_add_epi32(EEB, c32_rnd);
			T1FA = _mm256_add_epi32(EFA, c32_rnd);         // E15 + rnd
			T1FB = _mm256_add_epi32(EFB, c32_rnd);

			T2_00A = _mm256_add_epi32(T10A, O00A);          // E0 + O0 + rnd
			T2_00B = _mm256_add_epi32(T10B, O00B);
			T2_01A = _mm256_add_epi32(T11A, O01A);          // E1 + O1 + rnd
			T2_01B = _mm256_add_epi32(T11B, O01B);
			T2_02A = _mm256_add_epi32(T12A, O02A);          // E2 + O2 + rnd
			T2_02B = _mm256_add_epi32(T12B, O02B);
			T2_03A = _mm256_add_epi32(T13A, O03A);          // E3 + O3 + rnd
			T2_03B = _mm256_add_epi32(T13B, O03B);
			T2_04A = _mm256_add_epi32(T14A, O04A);          // E4
			T2_04B = _mm256_add_epi32(T14B, O04B);
			T2_05A = _mm256_add_epi32(T15A, O05A);          // E5
			T2_05B = _mm256_add_epi32(T15B, O05B);
			T2_06A = _mm256_add_epi32(T16A, O06A);          // E6
			T2_06B = _mm256_add_epi32(T16B, O06B);
			T2_07A = _mm256_add_epi32(T17A, O07A);          // E7
			T2_07B = _mm256_add_epi32(T17B, O07B);
			T2_08A = _mm256_add_epi32(T18A, O08A);          // E8
			T2_08B = _mm256_add_epi32(T18B, O08B);
			T2_09A = _mm256_add_epi32(T19A, O09A);          // E9
			T2_09B = _mm256_add_epi32(T19B, O09B);
			T2_10A = _mm256_add_epi32(T1AA, O10A);          // E10
			T2_10B = _mm256_add_epi32(T1AB, O10B);
			T2_11A = _mm256_add_epi32(T1BA, O11A);          // E11
			T2_11B = _mm256_add_epi32(T1BB, O11B);
			T2_12A = _mm256_add_epi32(T1CA, O12A);          // E12
			T2_12B = _mm256_add_epi32(T1CB, O12B);
			T2_13A = _mm256_add_epi32(T1DA, O13A);          // E13
			T2_13B = _mm256_add_epi32(T1DB, O13B);
			T2_14A = _mm256_add_epi32(T1EA, O14A);          // E14
			T2_14B = _mm256_add_epi32(T1EB, O14B);
			T2_15A = _mm256_add_epi32(T1FA, O15A);          // E15
			T2_15B = _mm256_add_epi32(T1FB, O15B);
			T2_31A = _mm256_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
			T2_31B = _mm256_sub_epi32(T10B, O00B);
			T2_30A = _mm256_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
			T2_30B = _mm256_sub_epi32(T11B, O01B);
			T2_29A = _mm256_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
			T2_29B = _mm256_sub_epi32(T12B, O02B);
			T2_28A = _mm256_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
			T2_28B = _mm256_sub_epi32(T13B, O03B);
			T2_27A = _mm256_sub_epi32(T14A, O04A);          // E4
			T2_27B = _mm256_sub_epi32(T14B, O04B);
			T2_26A = _mm256_sub_epi32(T15A, O05A);          // E5
			T2_26B = _mm256_sub_epi32(T15B, O05B);
			T2_25A = _mm256_sub_epi32(T16A, O06A);          // E6
			T2_25B = _mm256_sub_epi32(T16B, O06B);
			T2_24A = _mm256_sub_epi32(T17A, O07A);          // E7
			T2_24B = _mm256_sub_epi32(T17B, O07B);
			T2_23A = _mm256_sub_epi32(T18A, O08A);          //
			T2_23B = _mm256_sub_epi32(T18B, O08B);
			T2_22A = _mm256_sub_epi32(T19A, O09A);          //
			T2_22B = _mm256_sub_epi32(T19B, O09B);
			T2_21A = _mm256_sub_epi32(T1AA, O10A);          //
			T2_21B = _mm256_sub_epi32(T1AB, O10B);
			T2_20A = _mm256_sub_epi32(T1BA, O11A);          //
			T2_20B = _mm256_sub_epi32(T1BB, O11B);
			T2_19A = _mm256_sub_epi32(T1CA, O12A);          //
			T2_19B = _mm256_sub_epi32(T1CB, O12B);
			T2_18A = _mm256_sub_epi32(T1DA, O13A);          //
			T2_18B = _mm256_sub_epi32(T1DB, O13B);
			T2_17A = _mm256_sub_epi32(T1EA, O14A);          //
			T2_17B = _mm256_sub_epi32(T1EB, O14B);
			T2_16A = _mm256_sub_epi32(T1FA, O15A);          //
			T2_16B = _mm256_sub_epi32(T1FB, O15B);

			T3_00A = _mm256_srai_epi32(T2_00A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			T3_00B = _mm256_srai_epi32(T2_00B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			T3_01A = _mm256_srai_epi32(T2_01A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			T3_01B = _mm256_srai_epi32(T2_01B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
			T3_02A = _mm256_srai_epi32(T2_02A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
			T3_02B = _mm256_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
			T3_03A = _mm256_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
			T3_03B = _mm256_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
			T3_04A = _mm256_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
			T3_04B = _mm256_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
			T3_05A = _mm256_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
			T3_05B = _mm256_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
			T3_06A = _mm256_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
			T3_06B = _mm256_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
			T3_07A = _mm256_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
			T3_07B = _mm256_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
			T3_08A = _mm256_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
			T3_08B = _mm256_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
			T3_09A = _mm256_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
			T3_09B = _mm256_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
			T3_10A = _mm256_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
			T3_10B = _mm256_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
			T3_11A = _mm256_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
			T3_11B = _mm256_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
			T3_12A = _mm256_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
			T3_12B = _mm256_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
			T3_13A = _mm256_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
			T3_13B = _mm256_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
			T3_14A = _mm256_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
			T3_14B = _mm256_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
			T3_15A = _mm256_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
			T3_15B = _mm256_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

			T3_16A = _mm256_srai_epi32(T2_16A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			T3_16B = _mm256_srai_epi32(T2_16B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			T3_17A = _mm256_srai_epi32(T2_17A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			T3_17B = _mm256_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
			T3_18A = _mm256_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
			T3_18B = _mm256_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
			T3_19A = _mm256_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
			T3_19B = _mm256_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
			T3_20A = _mm256_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
			T3_20B = _mm256_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
			T3_21A = _mm256_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
			T3_21B = _mm256_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
			T3_22A = _mm256_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
			T3_22B = _mm256_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
			T3_23A = _mm256_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
			T3_23B = _mm256_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
			T3_24A = _mm256_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
			T3_24B = _mm256_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
			T3_25A = _mm256_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
			T3_25B = _mm256_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
			T3_26A = _mm256_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
			T3_26B = _mm256_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
			T3_27A = _mm256_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
			T3_27B = _mm256_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
			T3_28A = _mm256_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
			T3_28B = _mm256_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
			T3_29A = _mm256_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
			T3_29B = _mm256_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
			T3_30A = _mm256_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
			T3_30B = _mm256_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
			T3_31A = _mm256_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
			T3_31B = _mm256_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

			res00[part] = _mm256_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
			res01[part] = _mm256_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
			res02[part] = _mm256_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
			res03[part] = _mm256_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
			res04[part] = _mm256_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
			res05[part] = _mm256_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
			res06[part] = _mm256_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
			res07[part] = _mm256_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
			res08[part] = _mm256_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
			res09[part] = _mm256_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
			res10[part] = _mm256_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
			res11[part] = _mm256_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
			res12[part] = _mm256_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
			res13[part] = _mm256_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
			res14[part] = _mm256_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
			res15[part] = _mm256_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
			res16[part] = _mm256_packs_epi32(T3_16A, T3_16B);
			res17[part] = _mm256_packs_epi32(T3_17A, T3_17B);
			res18[part] = _mm256_packs_epi32(T3_18A, T3_18B);
			res19[part] = _mm256_packs_epi32(T3_19A, T3_19B);
			res20[part] = _mm256_packs_epi32(T3_20A, T3_20B);
			res21[part] = _mm256_packs_epi32(T3_21A, T3_21B);
			res22[part] = _mm256_packs_epi32(T3_22A, T3_22B);
			res23[part] = _mm256_packs_epi32(T3_23A, T3_23B);
			res24[part] = _mm256_packs_epi32(T3_24A, T3_24B);
			res25[part] = _mm256_packs_epi32(T3_25A, T3_25B);
			res26[part] = _mm256_packs_epi32(T3_26A, T3_26B);
			res27[part] = _mm256_packs_epi32(T3_27A, T3_27B);
			res28[part] = _mm256_packs_epi32(T3_28A, T3_28B);
			res29[part] = _mm256_packs_epi32(T3_29A, T3_29B);
			res30[part] = _mm256_packs_epi32(T3_30A, T3_30B);
			res31[part] = _mm256_packs_epi32(T3_31A, T3_31B);
		}


		//transpose 32x32 matrix
		//Not the all transpose.
		//Considering the add predict pixel
		{
			__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT_PARTIAL(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
				tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
				tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
				tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
				tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
				tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
				tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
				tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
				tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
				tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
				tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
				tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
				tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
				tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
				tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
				tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
				tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
				I0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
				I1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
				I2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
				I3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
				I4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
				I5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
				I6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
				I7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
				I8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
				I9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
				I10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
				I11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
				I12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
				I13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
				I14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
				I15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
				O0 = _mm256_unpacklo_epi64(I0, I1); \
				O1 = _mm256_unpacklo_epi64(I2, I3); \
				O2 = _mm256_unpackhi_epi64(I0, I1); \
				O3 = _mm256_unpackhi_epi64(I2, I3); \
				O4 = _mm256_unpacklo_epi64(I4, I5); \
				O5 = _mm256_unpacklo_epi64(I6, I7); \
				O6 = _mm256_unpackhi_epi64(I4, I5); \
				O7 = _mm256_unpackhi_epi64(I6, I7); \
				O8 = _mm256_unpacklo_epi64(I8, I9); \
				O9 = _mm256_unpacklo_epi64(I10, I11); \
				O10 = _mm256_unpackhi_epi64(I8, I9); \
				O11 = _mm256_unpackhi_epi64(I10, I11); \
				O12 = _mm256_unpacklo_epi64(I12, I13); \
				O13 = _mm256_unpacklo_epi64(I14, I15); \
				O14 = _mm256_unpackhi_epi64(I12, I13); \
				O15 = _mm256_unpackhi_epi64(I14, I15); \

			TRANSPOSE_16x16_16BIT_PARTIAL(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
			TRANSPOSE_16x16_16BIT_PARTIAL(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);
			TRANSPOSE_16x16_16BIT_PARTIAL(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0]);
			TRANSPOSE_16x16_16BIT_PARTIAL(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1]);

#undef TRANSPOSE_16x16_16BIT

		}

	}


	//clip
	max_val = _mm256_set1_epi16(255);
	min_val = _mm256_set1_epi16(-256);

	for (k = 0; k < 2; k++)
	{
		in00[k] = _mm256_min_epi16(in00[k], max_val);
		in00[k] = _mm256_max_epi16(in00[k], min_val);
		in01[k] = _mm256_min_epi16(in01[k], max_val);
		in01[k] = _mm256_max_epi16(in01[k], min_val);
		in02[k] = _mm256_min_epi16(in02[k], max_val);
		in02[k] = _mm256_max_epi16(in02[k], min_val);
		in03[k] = _mm256_min_epi16(in03[k], max_val);
		in03[k] = _mm256_max_epi16(in03[k], min_val);
		in04[k] = _mm256_min_epi16(in04[k], max_val);
		in04[k] = _mm256_max_epi16(in04[k], min_val);
		in05[k] = _mm256_min_epi16(in05[k], max_val);
		in05[k] = _mm256_max_epi16(in05[k], min_val);
		in06[k] = _mm256_min_epi16(in06[k], max_val);
		in06[k] = _mm256_max_epi16(in06[k], min_val);
		in07[k] = _mm256_min_epi16(in07[k], max_val);
		in07[k] = _mm256_max_epi16(in07[k], min_val);
		in08[k] = _mm256_min_epi16(in08[k], max_val);
		in08[k] = _mm256_max_epi16(in08[k], min_val);
		in09[k] = _mm256_min_epi16(in09[k], max_val);
		in09[k] = _mm256_max_epi16(in09[k], min_val);
		in10[k] = _mm256_min_epi16(in10[k], max_val);
		in10[k] = _mm256_max_epi16(in10[k], min_val);
		in11[k] = _mm256_min_epi16(in11[k], max_val);
		in11[k] = _mm256_max_epi16(in11[k], min_val);
		in12[k] = _mm256_min_epi16(in12[k], max_val);
		in12[k] = _mm256_max_epi16(in12[k], min_val);
		in13[k] = _mm256_min_epi16(in13[k], max_val);
		in13[k] = _mm256_max_epi16(in13[k], min_val);
		in14[k] = _mm256_min_epi16(in14[k], max_val);
		in14[k] = _mm256_max_epi16(in14[k], min_val);
		in15[k] = _mm256_min_epi16(in15[k], max_val);
		in15[k] = _mm256_max_epi16(in15[k], min_val);
		in16[k] = _mm256_min_epi16(in16[k], max_val);
		in16[k] = _mm256_max_epi16(in16[k], min_val);
		in17[k] = _mm256_min_epi16(in17[k], max_val);
		in17[k] = _mm256_max_epi16(in17[k], min_val);
		in18[k] = _mm256_min_epi16(in18[k], max_val);
		in18[k] = _mm256_max_epi16(in18[k], min_val);
		in19[k] = _mm256_min_epi16(in19[k], max_val);
		in19[k] = _mm256_max_epi16(in19[k], min_val);
		in20[k] = _mm256_min_epi16(in20[k], max_val);
		in20[k] = _mm256_max_epi16(in20[k], min_val);
		in21[k] = _mm256_min_epi16(in21[k], max_val);
		in21[k] = _mm256_max_epi16(in21[k], min_val);
		in22[k] = _mm256_min_epi16(in22[k], max_val);
		in22[k] = _mm256_max_epi16(in22[k], min_val);
		in23[k] = _mm256_min_epi16(in23[k], max_val);
		in23[k] = _mm256_max_epi16(in23[k], min_val);
		in24[k] = _mm256_min_epi16(in24[k], max_val);
		in24[k] = _mm256_max_epi16(in24[k], min_val);
		in25[k] = _mm256_min_epi16(in25[k], max_val);
		in25[k] = _mm256_max_epi16(in25[k], min_val);
		in26[k] = _mm256_min_epi16(in26[k], max_val);
		in26[k] = _mm256_max_epi16(in26[k], min_val);
		in27[k] = _mm256_min_epi16(in27[k], max_val);
		in27[k] = _mm256_max_epi16(in27[k], min_val);
		in28[k] = _mm256_min_epi16(in28[k], max_val);
		in28[k] = _mm256_max_epi16(in28[k], min_val);
		in29[k] = _mm256_min_epi16(in29[k], max_val);
		in29[k] = _mm256_max_epi16(in29[k], min_val);
		in30[k] = _mm256_min_epi16(in30[k], max_val);
		in30[k] = _mm256_max_epi16(in30[k], min_val);
		in31[k] = _mm256_min_epi16(in31[k], max_val);
		in31[k] = _mm256_max_epi16(in31[k], min_val);
	}

	// Add
	P00 = _mm256_loadu2_m128i((const __m128i*)&pred[8 * i_pred], (const __m128i*)&pred[0]);
	P01 = _mm256_loadu2_m128i((const __m128i*)&pred[9 * i_pred], (const __m128i*)&pred[1 * i_pred]);
	P02 = _mm256_loadu2_m128i((const __m128i*)&pred[10 * i_pred], (const __m128i*)&pred[2 * i_pred]);
	P03 = _mm256_loadu2_m128i((const __m128i*)&pred[11 * i_pred], (const __m128i*)&pred[3 * i_pred]);
	P04 = _mm256_loadu2_m128i((const __m128i*)&pred[12 * i_pred], (const __m128i*)&pred[4 * i_pred]);
	P05 = _mm256_loadu2_m128i((const __m128i*)&pred[13 * i_pred], (const __m128i*)&pred[5 * i_pred]);
	P06 = _mm256_loadu2_m128i((const __m128i*)&pred[14 * i_pred], (const __m128i*)&pred[6 * i_pred]);
	P07 = _mm256_loadu2_m128i((const __m128i*)&pred[15 * i_pred], (const __m128i*)&pred[7 * i_pred]);

	P08 = _mm256_loadu2_m128i((const __m128i*)&pred[8 * i_pred + 16], (const __m128i*)&pred[0 + 16]);
	P09 = _mm256_loadu2_m128i((const __m128i*)&pred[9 * i_pred + 16], (const __m128i*)&pred[1 * i_pred + 16]);
	P10 = _mm256_loadu2_m128i((const __m128i*)&pred[10 * i_pred + 16], (const __m128i*)&pred[2 * i_pred + 16]);
	P11 = _mm256_loadu2_m128i((const __m128i*)&pred[11 * i_pred + 16], (const __m128i*)&pred[3 * i_pred + 16]);
	P12 = _mm256_loadu2_m128i((const __m128i*)&pred[12 * i_pred + 16], (const __m128i*)&pred[4 * i_pred + 16]);
	P13 = _mm256_loadu2_m128i((const __m128i*)&pred[13 * i_pred + 16], (const __m128i*)&pred[5 * i_pred + 16]);
	P14 = _mm256_loadu2_m128i((const __m128i*)&pred[14 * i_pred + 16], (const __m128i*)&pred[6 * i_pred + 16]);
	P15 = _mm256_loadu2_m128i((const __m128i*)&pred[15 * i_pred + 16], (const __m128i*)&pred[7 * i_pred + 16]);

	P16 = _mm256_loadu2_m128i((const __m128i*)&pred[24 * i_pred], (const __m128i*)&pred[16 * i_pred]);
	P17 = _mm256_loadu2_m128i((const __m128i*)&pred[25 * i_pred], (const __m128i*)&pred[17 * i_pred]);
	P18 = _mm256_loadu2_m128i((const __m128i*)&pred[26 * i_pred], (const __m128i*)&pred[18 * i_pred]);
	P19 = _mm256_loadu2_m128i((const __m128i*)&pred[27 * i_pred], (const __m128i*)&pred[19 * i_pred]);
	P20 = _mm256_loadu2_m128i((const __m128i*)&pred[28 * i_pred], (const __m128i*)&pred[20 * i_pred]);
	P21 = _mm256_loadu2_m128i((const __m128i*)&pred[29 * i_pred], (const __m128i*)&pred[21 * i_pred]);
	P22 = _mm256_loadu2_m128i((const __m128i*)&pred[30 * i_pred], (const __m128i*)&pred[22 * i_pred]);
	P23 = _mm256_loadu2_m128i((const __m128i*)&pred[31 * i_pred], (const __m128i*)&pred[23 * i_pred]);

	P24 = _mm256_loadu2_m128i((const __m128i*)&pred[24 * i_pred + 16], (const __m128i*)&pred[16 * i_pred + 16]);
	P25 = _mm256_loadu2_m128i((const __m128i*)&pred[25 * i_pred + 16], (const __m128i*)&pred[17 * i_pred + 16]);
	P26 = _mm256_loadu2_m128i((const __m128i*)&pred[26 * i_pred + 16], (const __m128i*)&pred[18 * i_pred + 16]);
	P27 = _mm256_loadu2_m128i((const __m128i*)&pred[27 * i_pred + 16], (const __m128i*)&pred[19 * i_pred + 16]);
	P28 = _mm256_loadu2_m128i((const __m128i*)&pred[28 * i_pred + 16], (const __m128i*)&pred[20 * i_pred + 16]);
	P29 = _mm256_loadu2_m128i((const __m128i*)&pred[29 * i_pred + 16], (const __m128i*)&pred[21 * i_pred + 16]);
	P30 = _mm256_loadu2_m128i((const __m128i*)&pred[30 * i_pred + 16], (const __m128i*)&pred[22 * i_pred + 16]);
	P31 = _mm256_loadu2_m128i((const __m128i*)&pred[31 * i_pred + 16], (const __m128i*)&pred[23 * i_pred + 16]);


	res00[0] = _mm256_unpacklo_epi8(P00, zero);
	res01[0] = _mm256_unpackhi_epi8(P00, zero);
	res02[0] = _mm256_unpacklo_epi8(P01, zero);
	res03[0] = _mm256_unpackhi_epi8(P01, zero);
	res04[0] = _mm256_unpacklo_epi8(P02, zero);
	res05[0] = _mm256_unpackhi_epi8(P02, zero);
	res06[0] = _mm256_unpacklo_epi8(P03, zero);
	res07[0] = _mm256_unpackhi_epi8(P03, zero);
	res08[0] = _mm256_unpacklo_epi8(P04, zero);
	res09[0] = _mm256_unpackhi_epi8(P04, zero);
	res10[0] = _mm256_unpacklo_epi8(P05, zero);
	res11[0] = _mm256_unpackhi_epi8(P05, zero);
	res12[0] = _mm256_unpacklo_epi8(P06, zero);
	res13[0] = _mm256_unpackhi_epi8(P06, zero);
	res14[0] = _mm256_unpacklo_epi8(P07, zero);
	res15[0] = _mm256_unpackhi_epi8(P07, zero);

	res00[1] = _mm256_unpacklo_epi8(P08, zero);
	res01[1] = _mm256_unpackhi_epi8(P08, zero);
	res02[1] = _mm256_unpacklo_epi8(P09, zero);
	res03[1] = _mm256_unpackhi_epi8(P09, zero);
	res04[1] = _mm256_unpacklo_epi8(P10, zero);
	res05[1] = _mm256_unpackhi_epi8(P10, zero);
	res06[1] = _mm256_unpacklo_epi8(P11, zero);
	res07[1] = _mm256_unpackhi_epi8(P11, zero);
	res08[1] = _mm256_unpacklo_epi8(P12, zero);
	res09[1] = _mm256_unpackhi_epi8(P12, zero);
	res10[1] = _mm256_unpacklo_epi8(P13, zero);
	res11[1] = _mm256_unpackhi_epi8(P13, zero);
	res12[1] = _mm256_unpacklo_epi8(P14, zero);
	res13[1] = _mm256_unpackhi_epi8(P14, zero);
	res14[1] = _mm256_unpacklo_epi8(P15, zero);
	res15[1] = _mm256_unpackhi_epi8(P15, zero);

	res16[0] = _mm256_unpacklo_epi8(P16, zero);
	res17[0] = _mm256_unpackhi_epi8(P16, zero);
	res18[0] = _mm256_unpacklo_epi8(P17, zero);
	res19[0] = _mm256_unpackhi_epi8(P17, zero);
	res20[0] = _mm256_unpacklo_epi8(P18, zero);
	res21[0] = _mm256_unpackhi_epi8(P18, zero);
	res22[0] = _mm256_unpacklo_epi8(P19, zero);
	res23[0] = _mm256_unpackhi_epi8(P19, zero);
	res24[0] = _mm256_unpacklo_epi8(P20, zero);
	res25[0] = _mm256_unpackhi_epi8(P20, zero);
	res26[0] = _mm256_unpacklo_epi8(P21, zero);
	res27[0] = _mm256_unpackhi_epi8(P21, zero);
	res28[0] = _mm256_unpacklo_epi8(P22, zero);
	res29[0] = _mm256_unpackhi_epi8(P22, zero);
	res30[0] = _mm256_unpacklo_epi8(P23, zero);
	res31[0] = _mm256_unpackhi_epi8(P23, zero);

	res16[1] = _mm256_unpacklo_epi8(P24, zero);
	res17[1] = _mm256_unpackhi_epi8(P24, zero);
	res18[1] = _mm256_unpacklo_epi8(P25, zero);
	res19[1] = _mm256_unpackhi_epi8(P25, zero);
	res20[1] = _mm256_unpacklo_epi8(P26, zero);
	res21[1] = _mm256_unpackhi_epi8(P26, zero);
	res22[1] = _mm256_unpacklo_epi8(P27, zero);
	res23[1] = _mm256_unpackhi_epi8(P27, zero);
	res24[1] = _mm256_unpacklo_epi8(P28, zero);
	res25[1] = _mm256_unpackhi_epi8(P28, zero);
	res26[1] = _mm256_unpacklo_epi8(P29, zero);
	res27[1] = _mm256_unpackhi_epi8(P29, zero);
	res28[1] = _mm256_unpacklo_epi8(P30, zero);
	res29[1] = _mm256_unpackhi_epi8(P30, zero);
	res30[1] = _mm256_unpacklo_epi8(P31, zero);
	res31[1] = _mm256_unpackhi_epi8(P31, zero);

	for (k = 0; k < 2; k++)
	{
		res00[k] = _mm256_add_epi16(in00[k], res00[k]);
		res01[k] = _mm256_add_epi16(in01[k], res01[k]);
		res02[k] = _mm256_add_epi16(in02[k], res02[k]);
		res03[k] = _mm256_add_epi16(in03[k], res03[k]);
		res04[k] = _mm256_add_epi16(in04[k], res04[k]);
		res05[k] = _mm256_add_epi16(in05[k], res05[k]);
		res06[k] = _mm256_add_epi16(in06[k], res06[k]);
		res07[k] = _mm256_add_epi16(in07[k], res07[k]);
		res08[k] = _mm256_add_epi16(in08[k], res08[k]);
		res09[k] = _mm256_add_epi16(in09[k], res09[k]);
		res10[k] = _mm256_add_epi16(in10[k], res10[k]);
		res11[k] = _mm256_add_epi16(in11[k], res11[k]);
		res12[k] = _mm256_add_epi16(in12[k], res12[k]);
		res13[k] = _mm256_add_epi16(in13[k], res13[k]);
		res14[k] = _mm256_add_epi16(in14[k], res14[k]);
		res15[k] = _mm256_add_epi16(in15[k], res15[k]);
		res16[k] = _mm256_add_epi16(in16[k], res16[k]);
		res17[k] = _mm256_add_epi16(in17[k], res17[k]);
		res18[k] = _mm256_add_epi16(in18[k], res18[k]);
		res19[k] = _mm256_add_epi16(in19[k], res19[k]);
		res20[k] = _mm256_add_epi16(in20[k], res20[k]);
		res21[k] = _mm256_add_epi16(in21[k], res21[k]);
		res22[k] = _mm256_add_epi16(in22[k], res22[k]);
		res23[k] = _mm256_add_epi16(in23[k], res23[k]);
		res24[k] = _mm256_add_epi16(in24[k], res24[k]);
		res25[k] = _mm256_add_epi16(in25[k], res25[k]);
		res26[k] = _mm256_add_epi16(in26[k], res26[k]);
		res27[k] = _mm256_add_epi16(in27[k], res27[k]);
		res28[k] = _mm256_add_epi16(in28[k], res28[k]);
		res29[k] = _mm256_add_epi16(in29[k], res29[k]);
		res30[k] = _mm256_add_epi16(in30[k], res30[k]);
		res31[k] = _mm256_add_epi16(in31[k], res31[k]);
	}


	P00 = _mm256_packus_epi16(res00[0], res01[0]);
	P01 = _mm256_packus_epi16(res02[0], res03[0]);
	P02 = _mm256_packus_epi16(res04[0], res05[0]);
	P03 = _mm256_packus_epi16(res06[0], res07[0]);
	P04 = _mm256_packus_epi16(res08[0], res09[0]);
	P05 = _mm256_packus_epi16(res10[0], res11[0]);
	P06 = _mm256_packus_epi16(res12[0], res13[0]);
	P07 = _mm256_packus_epi16(res14[0], res15[0]);

	P08 = _mm256_packus_epi16(res00[1], res01[1]);
	P09 = _mm256_packus_epi16(res02[1], res03[1]);
	P10 = _mm256_packus_epi16(res04[1], res05[1]);
	P11 = _mm256_packus_epi16(res06[1], res07[1]);
	P12 = _mm256_packus_epi16(res08[1], res09[1]);
	P13 = _mm256_packus_epi16(res10[1], res11[1]);
	P14 = _mm256_packus_epi16(res12[1], res13[1]);
	P15 = _mm256_packus_epi16(res14[1], res15[1]);


	P16 = _mm256_packus_epi16(res16[0], res17[0]);
	P17 = _mm256_packus_epi16(res18[0], res19[0]);
	P18 = _mm256_packus_epi16(res20[0], res21[0]);
	P19 = _mm256_packus_epi16(res22[0], res23[0]);
	P20 = _mm256_packus_epi16(res24[0], res25[0]);
	P21 = _mm256_packus_epi16(res26[0], res27[0]);
	P22 = _mm256_packus_epi16(res28[0], res29[0]);
	P23 = _mm256_packus_epi16(res30[0], res31[0]);


	P24 = _mm256_packus_epi16(res16[1], res17[1]);
	P25 = _mm256_packus_epi16(res18[1], res19[1]);
	P26 = _mm256_packus_epi16(res20[1], res21[1]);
	P27 = _mm256_packus_epi16(res22[1], res23[1]);
	P28 = _mm256_packus_epi16(res24[1], res25[1]);
	P29 = _mm256_packus_epi16(res26[1], res27[1]);
	P30 = _mm256_packus_epi16(res28[1], res29[1]);
	P31 = _mm256_packus_epi16(res30[1], res31[1]);

	//_mm256_storeu2_m128i((__m128i*)&dst[8 * i_dst], ( __m128i*)&dst[0], P00);
	//_mm256_storeu2_m128i(( __m128i*)&dst[9 * i_dst], ( __m128i*)&dst[1 * i_dst], P01);
	//_mm256_storeu2_m128i(( __m128i*)&dst[10 * i_dst], ( __m128i*)&dst[2 * i_dst], P02);
	//_mm256_storeu2_m128i(( __m128i*)&dst[11 * i_dst], ( __m128i*)&dst[3 * i_dst], P03);
	//_mm256_storeu2_m128i(( __m128i*)&dst[12 * i_dst], ( __m128i*)&dst[4 * i_dst], P04);
	//_mm256_storeu2_m128i(( __m128i*)&dst[13 * i_dst], ( __m128i*)&dst[5 * i_dst], P05);
	//_mm256_storeu2_m128i(( __m128i*)&dst[14 * i_dst], ( __m128i*)&dst[6 * i_dst], P06);
	//_mm256_storeu2_m128i(( __m128i*)&dst[15 * i_dst], ( __m128i*)&dst[7 * i_dst], P07);

	//_mm256_storeu2_m128i(( __m128i*)&dst[8 * i_dst + 16], ( __m128i*)&dst[0 + 16], P08);
	//_mm256_storeu2_m128i(( __m128i*)&dst[9 * i_dst + 16], ( __m128i*)&dst[1 * i_dst + 16], P09);
	//_mm256_storeu2_m128i(( __m128i*)&dst[10 * i_dst + 16], ( __m128i*)&dst[2 * i_dst + 16], P10);
	//_mm256_storeu2_m128i(( __m128i*)&dst[11 * i_dst + 16], ( __m128i*)&dst[3 * i_dst + 16], P11);
	//_mm256_storeu2_m128i(( __m128i*)&dst[12 * i_dst + 16], ( __m128i*)&dst[4 * i_dst + 16], P12);
	//_mm256_storeu2_m128i(( __m128i*)&dst[13 * i_dst + 16], ( __m128i*)&dst[5 * i_dst + 16], P13);
	//_mm256_storeu2_m128i(( __m128i*)&dst[14 * i_dst + 16], ( __m128i*)&dst[6 * i_dst + 16], P14);
	//_mm256_storeu2_m128i(( __m128i*)&dst[15 * i_dst + 16], ( __m128i*)&dst[7 * i_dst + 16], P15);

	//_mm256_storeu2_m128i(( __m128i*)&dst[24 * i_dst], ( __m128i*)&dst[16 * i_dst], P16);
	//_mm256_storeu2_m128i(( __m128i*)&dst[25 * i_dst], ( __m128i*)&dst[17 * i_dst], P17);
	//_mm256_storeu2_m128i(( __m128i*)&dst[26 * i_dst], ( __m128i*)&dst[18 * i_dst], P18);
	//_mm256_storeu2_m128i(( __m128i*)&dst[27 * i_dst], ( __m128i*)&dst[19 * i_dst], P19);
	//_mm256_storeu2_m128i(( __m128i*)&dst[28 * i_dst], ( __m128i*)&dst[20 * i_dst], P20);
	//_mm256_storeu2_m128i(( __m128i*)&dst[29 * i_dst], ( __m128i*)&dst[21 * i_dst], P21);
	//_mm256_storeu2_m128i(( __m128i*)&dst[30 * i_dst], ( __m128i*)&dst[22 * i_dst], P22);
	//_mm256_storeu2_m128i(( __m128i*)&dst[31 * i_dst], ( __m128i*)&dst[23 * i_dst], P23);

	//_mm256_storeu2_m128i(( __m128i*)&dst[24 * i_dst + 16], ( __m128i*)&dst[16 * i_dst + 16], P24);
	//_mm256_storeu2_m128i(( __m128i*)&dst[25 * i_dst + 16], ( __m128i*)&dst[17 * i_dst + 16], P25);
	//_mm256_storeu2_m128i(( __m128i*)&dst[26 * i_dst + 16], ( __m128i*)&dst[18 * i_dst + 16], P26);
	//_mm256_storeu2_m128i(( __m128i*)&dst[27 * i_dst + 16], ( __m128i*)&dst[19 * i_dst + 16], P27);
	//_mm256_storeu2_m128i(( __m128i*)&dst[28 * i_dst + 16], ( __m128i*)&dst[20 * i_dst + 16], P28);
	//_mm256_storeu2_m128i(( __m128i*)&dst[29 * i_dst + 16], ( __m128i*)&dst[21 * i_dst + 16], P29);
	//_mm256_storeu2_m128i(( __m128i*)&dst[30 * i_dst + 16], ( __m128i*)&dst[22 * i_dst + 16], P30);
	//_mm256_storeu2_m128i(( __m128i*)&dst[31 * i_dst + 16], ( __m128i*)&dst[23 * i_dst + 16], P31);


	_mm_storeu_si128((__m128i*)&dst[0], _mm256_castsi256_si128(P00)); _mm_storeu_si128((__m128i*)&dst[8 * i_dst], _mm256_extractf128_si256(P00, 0x1));
	_mm_storeu_si128((__m128i*)&dst[1 * i_dst], _mm256_castsi256_si128(P01)); _mm_storeu_si128((__m128i*)&dst[9 * i_dst], _mm256_extractf128_si256(P01, 0x1));
	_mm_storeu_si128((__m128i*)&dst[2 * i_dst], _mm256_castsi256_si128(P02)); _mm_storeu_si128((__m128i*)&dst[10 * i_dst], _mm256_extractf128_si256(P02, 0x1));
	_mm_storeu_si128((__m128i*)&dst[3 * i_dst], _mm256_castsi256_si128(P03)); _mm_storeu_si128((__m128i*)&dst[11 * i_dst], _mm256_extractf128_si256(P03, 0x1));
	_mm_storeu_si128((__m128i*)&dst[4 * i_dst], _mm256_castsi256_si128(P04)); _mm_storeu_si128((__m128i*)&dst[12 * i_dst], _mm256_extractf128_si256(P04, 0x1));
	_mm_storeu_si128((__m128i*)&dst[5 * i_dst], _mm256_castsi256_si128(P05)); _mm_storeu_si128((__m128i*)&dst[13 * i_dst], _mm256_extractf128_si256(P05, 0x1));
	_mm_storeu_si128((__m128i*)&dst[6 * i_dst], _mm256_castsi256_si128(P06)); _mm_storeu_si128((__m128i*)&dst[14 * i_dst], _mm256_extractf128_si256(P06, 0x1));
	_mm_storeu_si128((__m128i*)&dst[7 * i_dst], _mm256_castsi256_si128(P07)); _mm_storeu_si128((__m128i*)&dst[15 * i_dst], _mm256_extractf128_si256(P07, 0x1));

	_mm_storeu_si128((__m128i*)&dst[0 + 16], _mm256_castsi256_si128(P08)); _mm_storeu_si128((__m128i*)&dst[8 * i_dst + 16], _mm256_extractf128_si256(P08, 0x1));
	_mm_storeu_si128((__m128i*)&dst[1 * i_dst + 16], _mm256_castsi256_si128(P09)); _mm_storeu_si128((__m128i*)&dst[9 * i_dst + 16], _mm256_extractf128_si256(P09, 0x1));
	_mm_storeu_si128((__m128i*)&dst[2 * i_dst + 16], _mm256_castsi256_si128(P10)); _mm_storeu_si128((__m128i*)&dst[10 * i_dst + 16], _mm256_extractf128_si256(P10, 0x1));
	_mm_storeu_si128((__m128i*)&dst[3 * i_dst + 16], _mm256_castsi256_si128(P11)); _mm_storeu_si128((__m128i*)&dst[11 * i_dst + 16], _mm256_extractf128_si256(P11, 0x1));
	_mm_storeu_si128((__m128i*)&dst[4 * i_dst + 16], _mm256_castsi256_si128(P12)); _mm_storeu_si128((__m128i*)&dst[12 * i_dst + 16], _mm256_extractf128_si256(P12, 0x1));
	_mm_storeu_si128((__m128i*)&dst[5 * i_dst + 16], _mm256_castsi256_si128(P13)); _mm_storeu_si128((__m128i*)&dst[13 * i_dst + 16], _mm256_extractf128_si256(P13, 0x1));
	_mm_storeu_si128((__m128i*)&dst[6 * i_dst + 16], _mm256_castsi256_si128(P14)); _mm_storeu_si128((__m128i*)&dst[14 * i_dst + 16], _mm256_extractf128_si256(P14, 0x1));
	_mm_storeu_si128((__m128i*)&dst[7 * i_dst + 16], _mm256_castsi256_si128(P15)); _mm_storeu_si128((__m128i*)&dst[15 * i_dst + 16], _mm256_extractf128_si256(P15, 0x1));


	_mm_storeu_si128((__m128i*)&dst[16 * i_dst], _mm256_castsi256_si128(P16)); _mm_storeu_si128((__m128i*)&dst[24 * i_dst], _mm256_extractf128_si256(P16, 0x1));
	_mm_storeu_si128((__m128i*)&dst[17 * i_dst], _mm256_castsi256_si128(P17)); _mm_storeu_si128((__m128i*)&dst[25 * i_dst], _mm256_extractf128_si256(P17, 0x1));
	_mm_storeu_si128((__m128i*)&dst[18 * i_dst], _mm256_castsi256_si128(P18)); _mm_storeu_si128((__m128i*)&dst[26 * i_dst], _mm256_extractf128_si256(P18, 0x1));
	_mm_storeu_si128((__m128i*)&dst[19 * i_dst], _mm256_castsi256_si128(P19)); _mm_storeu_si128((__m128i*)&dst[27 * i_dst], _mm256_extractf128_si256(P19, 0x1));
	_mm_storeu_si128((__m128i*)&dst[20 * i_dst], _mm256_castsi256_si128(P20)); _mm_storeu_si128((__m128i*)&dst[28 * i_dst], _mm256_extractf128_si256(P20, 0x1));
	_mm_storeu_si128((__m128i*)&dst[21 * i_dst], _mm256_castsi256_si128(P21)); _mm_storeu_si128((__m128i*)&dst[29 * i_dst], _mm256_extractf128_si256(P21, 0x1));
	_mm_storeu_si128((__m128i*)&dst[22 * i_dst], _mm256_castsi256_si128(P22)); _mm_storeu_si128((__m128i*)&dst[30 * i_dst], _mm256_extractf128_si256(P22, 0x1));
	_mm_storeu_si128((__m128i*)&dst[23 * i_dst], _mm256_castsi256_si128(P23)); _mm_storeu_si128((__m128i*)&dst[31 * i_dst], _mm256_extractf128_si256(P23, 0x1));


	_mm_storeu_si128((__m128i*)&dst[16 * i_dst + 16], _mm256_castsi256_si128(P24)); _mm_storeu_si128((__m128i*)&dst[24 * i_dst + 16], _mm256_extractf128_si256(P24, 0x1));
	_mm_storeu_si128((__m128i*)&dst[17 * i_dst + 16], _mm256_castsi256_si128(P25)); _mm_storeu_si128((__m128i*)&dst[25 * i_dst + 16], _mm256_extractf128_si256(P25, 0x1));
	_mm_storeu_si128((__m128i*)&dst[18 * i_dst + 16], _mm256_castsi256_si128(P26)); _mm_storeu_si128((__m128i*)&dst[26 * i_dst + 16], _mm256_extractf128_si256(P26, 0x1));
	_mm_storeu_si128((__m128i*)&dst[19 * i_dst + 16], _mm256_castsi256_si128(P27)); _mm_storeu_si128((__m128i*)&dst[27 * i_dst + 16], _mm256_extractf128_si256(P27, 0x1));
	_mm_storeu_si128((__m128i*)&dst[20 * i_dst + 16], _mm256_castsi256_si128(P28)); _mm_storeu_si128((__m128i*)&dst[28 * i_dst + 16], _mm256_extractf128_si256(P28, 0x1));
	_mm_storeu_si128((__m128i*)&dst[21 * i_dst + 16], _mm256_castsi256_si128(P29)); _mm_storeu_si128((__m128i*)&dst[29 * i_dst + 16], _mm256_extractf128_si256(P29, 0x1));
	_mm_storeu_si128((__m128i*)&dst[22 * i_dst + 16], _mm256_castsi256_si128(P30)); _mm_storeu_si128((__m128i*)&dst[30 * i_dst + 16], _mm256_extractf128_si256(P30, 0x1));
	_mm_storeu_si128((__m128i*)&dst[23 * i_dst + 16], _mm256_castsi256_si128(P31)); _mm_storeu_si128((__m128i*)&dst[31 * i_dst + 16], _mm256_extractf128_si256(P31, 0x1));

}

void add_inv_trans_ext_32x32_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	int k;
	__m256i P00, P01, P02, P03, P04, P05, P06, P07, P08, P09, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P30, P31;
	__m256i max_val, min_val;
	__m256i EEO0A, EEO1A, EEO2A, EEO3A, EEO0B, EEO1B, EEO2B, EEO3B;
	__m256i EEEO0A, EEEO0B, EEEO1A, EEEO1B;
	__m256i EEEE0A, EEEE0B, EEEE1A, EEEE1B;
	__m256i EEE0A, EEE0B, EEE1A, EEE1B, EEE3A, EEE3B, EEE2A, EEE2B;
	__m256i EE0A, EE0B, EE1A, EE1B, EE2A, EE2B, EE3A, EE3B, EE7A, EE7B, EE6A, EE6B, EE5A, EE5B, EE4A, EE4B;
	__m256i E0A, E0B, E1A, E1B, E2A, E2B, E3A, E3B, E4A, E4B, E5A, E5B, E6A, E6B, E7A, E7B, EFA, EFB, EEA, EEB, EDA, EDB, ECA, ECB, EBA, EBB, EAA, EAB, E9A, E9B, E8A, E8B;
	__m256i T10A, T10B, T11A, T11B, T12A, T12B, T13A, T13B, T14A, T14B, T15A, T15B, T16A, T16B, T17A, T17B, T18A, T18B, T19A, T19B, T1AA, T1AB, T1BA, T1BB, T1CA, T1CB, T1DA, T1DB, T1EA, T1EB, T1FA, T1FB;
	__m256i T2_00A, T2_00B, T2_01A, T2_01B, T2_02A, T2_02B, T2_03A, T2_03B, T2_04A, T2_04B, T2_05A, T2_05B, T2_06A, T2_06B, T2_07A, T2_07B, T2_08A, T2_08B, T2_09A, T2_09B, T2_10A, T2_10B, T2_11A, T2_11B, T2_12A, T2_12B, T2_13A, T2_13B, T2_14A, T2_14B, T2_15A, T2_15B, T2_31A, T2_31B, T2_30A, T2_30B, T2_29A, T2_29B, T2_28A, T2_28B, T2_27A, T2_27B, T2_26A, T2_26B, T2_25A, T2_25B, T2_24A, T2_24B, T2_23A, T2_23B, T2_22A, T2_22B, T2_21A, T2_21B, T2_20A, T2_20B, T2_19A, T2_19B, T2_18A, T2_18B, T2_17A, T2_17B, T2_16A, T2_16B;
	__m256i T3_00A, T3_00B, T3_01A, T3_01B, T3_02A, T3_02B, T3_03A, T3_03B, T3_04A, T3_04B, T3_05A, T3_05B, T3_06A, T3_06B, T3_07A, T3_07B, T3_08A, T3_08B, T3_09A, T3_09B, T3_10A, T3_10B, T3_11A, T3_11B, T3_12A, T3_12B, T3_13A, T3_13B, T3_14A, T3_14B, T3_15A, T3_15B;
	__m256i T3_16A, T3_16B, T3_17A, T3_17B, T3_18A, T3_18B, T3_19A, T3_19B, T3_20A, T3_20B, T3_21A, T3_21B, T3_22A, T3_22B, T3_23A, T3_23B, T3_24A, T3_24B, T3_25A, T3_25B, T3_26A, T3_26B, T3_27A, T3_27B, T3_28A, T3_28B, T3_29A, T3_29B, T3_30A, T3_30B, T3_31A, T3_31B;
	const __m256i c16_p45_p45 = _mm256_set1_epi32(0x002D002D);
	const __m256i c16_p43_p44 = _mm256_set1_epi32(0x002B002C);
	const __m256i c16_p39_p41 = _mm256_set1_epi32(0x00270029);
	const __m256i c16_p34_p36 = _mm256_set1_epi32(0x00220024);
	//	const __m256i c16_p27_p30 = _mm256_set1_epi32(0x001B001E);
	//	const __m256i c16_p19_p23 = _mm256_set1_epi32(0x00130017);
	//	const __m256i c16_p11_p15 = _mm256_set1_epi32(0x000B000F);
	//	const __m256i c16_p02_p07 = _mm256_set1_epi32(0x00020007);
	const __m256i c16_p41_p45 = _mm256_set1_epi32(0x0029002D);
	const __m256i c16_p23_p34 = _mm256_set1_epi32(0x00170022);
	const __m256i c16_n02_p11 = _mm256_set1_epi32(0xFFFE000B);
	const __m256i c16_n27_n15 = _mm256_set1_epi32(0xFFE5FFF1);
	//	const __m256i c16_n43_n36 = _mm256_set1_epi32(0xFFD5FFDC);
	//	const __m256i c16_n44_n45 = _mm256_set1_epi32(0xFFD4FFD3);
	//	const __m256i c16_n30_n39 = _mm256_set1_epi32(0xFFE2FFD9);
	//	const __m256i c16_n07_n19 = _mm256_set1_epi32(0xFFF9FFED);
	const __m256i c16_p34_p44 = _mm256_set1_epi32(0x0022002C);
	const __m256i c16_n07_p15 = _mm256_set1_epi32(0xFFF9000F);
	const __m256i c16_n41_n27 = _mm256_set1_epi32(0xFFD7FFE5);
	const __m256i c16_n39_n45 = _mm256_set1_epi32(0xFFD9FFD3);
	//	const __m256i c16_n02_n23 = _mm256_set1_epi32(0xFFFEFFE9);
	//	const __m256i c16_p36_p19 = _mm256_set1_epi32(0x00240013);
	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	//	const __m256i c16_p11_p30 = _mm256_set1_epi32(0x000B001E);
	const __m256i c16_p23_p43 = _mm256_set1_epi32(0x0017002B);
	const __m256i c16_n34_n07 = _mm256_set1_epi32(0xFFDEFFF9);
	const __m256i c16_n36_n45 = _mm256_set1_epi32(0xFFDCFFD3);
	const __m256i c16_p19_n11 = _mm256_set1_epi32(0x0013FFF5);
	//	const __m256i c16_p44_p41 = _mm256_set1_epi32(0x002C0029);
	//	const __m256i c16_n02_p27 = _mm256_set1_epi32(0xFFFE001B);
	//	const __m256i c16_n45_n30 = _mm256_set1_epi32(0xFFD3FFE2);
	//	const __m256i c16_n15_n39 = _mm256_set1_epi32(0xFFF1FFD9);
	const __m256i c16_p11_p41 = _mm256_set1_epi32(0x000B0029);
	const __m256i c16_n45_n27 = _mm256_set1_epi32(0xFFD3FFE5);
	const __m256i c16_p07_n30 = _mm256_set1_epi32(0x0007FFE2);
	const __m256i c16_p43_p39 = _mm256_set1_epi32(0x002B0027);
	//	const __m256i c16_n23_p15 = _mm256_set1_epi32(0xFFE9000F);
	//	const __m256i c16_n34_n45 = _mm256_set1_epi32(0xFFDEFFD3);
	//	const __m256i c16_p36_p02 = _mm256_set1_epi32(0x00240002);
	//	const __m256i c16_p19_p44 = _mm256_set1_epi32(0x0013002C);
	const __m256i c16_n02_p39 = _mm256_set1_epi32(0xFFFE0027);
	const __m256i c16_n36_n41 = _mm256_set1_epi32(0xFFDCFFD7);
	const __m256i c16_p43_p07 = _mm256_set1_epi32(0x002B0007);
	const __m256i c16_n11_p34 = _mm256_set1_epi32(0xFFF50022);
	//	const __m256i c16_n30_n44 = _mm256_set1_epi32(0xFFE2FFD4);
	//	const __m256i c16_p45_p15 = _mm256_set1_epi32(0x002D000F);
	//	const __m256i c16_n19_p27 = _mm256_set1_epi32(0xFFED001B);
	//	const __m256i c16_n23_n45 = _mm256_set1_epi32(0xFFE9FFD3);
	const __m256i c16_n15_p36 = _mm256_set1_epi32(0xFFF10024);
	const __m256i c16_n11_n45 = _mm256_set1_epi32(0xFFF5FFD3);
	const __m256i c16_p34_p39 = _mm256_set1_epi32(0x00220027);
	const __m256i c16_n45_n19 = _mm256_set1_epi32(0xFFD3FFED);
	//	const __m256i c16_p41_n07 = _mm256_set1_epi32(0x0029FFF9);
	//	const __m256i c16_n23_p30 = _mm256_set1_epi32(0xFFE9001E);
	//	const __m256i c16_n02_n44 = _mm256_set1_epi32(0xFFFEFFD4);
	//	const __m256i c16_p27_p43 = _mm256_set1_epi32(0x001B002B);
	const __m256i c16_n27_p34 = _mm256_set1_epi32(0xFFE50022);
	const __m256i c16_p19_n39 = _mm256_set1_epi32(0x0013FFD9);
	const __m256i c16_n11_p43 = _mm256_set1_epi32(0xFFF5002B);
	const __m256i c16_p02_n45 = _mm256_set1_epi32(0x0002FFD3);
	//	const __m256i c16_p07_p45 = _mm256_set1_epi32(0x0007002D);
	//	const __m256i c16_n15_n44 = _mm256_set1_epi32(0xFFF1FFD4);
	//	const __m256i c16_p23_p41 = _mm256_set1_epi32(0x00170029);
	//	const __m256i c16_n30_n36 = _mm256_set1_epi32(0xFFE2FFDC);
	const __m256i c16_n36_p30 = _mm256_set1_epi32(0xFFDC001E);
	const __m256i c16_p41_n23 = _mm256_set1_epi32(0x0029FFE9);
	const __m256i c16_n44_p15 = _mm256_set1_epi32(0xFFD4000F);
	const __m256i c16_p45_n07 = _mm256_set1_epi32(0x002DFFF9);
	//	const __m256i c16_n45_n02 = _mm256_set1_epi32(0xFFD3FFFE);
	//	const __m256i c16_p43_p11 = _mm256_set1_epi32(0x002B000B);
	//	const __m256i c16_n39_n19 = _mm256_set1_epi32(0xFFD9FFED);
	//	const __m256i c16_p34_p27 = _mm256_set1_epi32(0x0022001B);
	const __m256i c16_n43_p27 = _mm256_set1_epi32(0xFFD5001B);
	const __m256i c16_p44_n02 = _mm256_set1_epi32(0x002CFFFE);
	const __m256i c16_n30_n23 = _mm256_set1_epi32(0xFFE2FFE9);
	const __m256i c16_p07_p41 = _mm256_set1_epi32(0x00070029);
	//	const __m256i c16_p19_n45 = _mm256_set1_epi32(0x0013FFD3);
	//	const __m256i c16_n39_p34 = _mm256_set1_epi32(0xFFD90022);
	//	const __m256i c16_p45_n11 = _mm256_set1_epi32(0x002DFFF5);
	//	const __m256i c16_n36_n15 = _mm256_set1_epi32(0xFFDCFFF1);
	const __m256i c16_n45_p23 = _mm256_set1_epi32(0xFFD30017);
	const __m256i c16_p27_p19 = _mm256_set1_epi32(0x001B0013);
	const __m256i c16_p15_n45 = _mm256_set1_epi32(0x000FFFD3);
	const __m256i c16_n44_p30 = _mm256_set1_epi32(0xFFD4001E);
	//	const __m256i c16_p34_p11 = _mm256_set1_epi32(0x0022000B);
	//	const __m256i c16_p07_n43 = _mm256_set1_epi32(0x0007FFD5);
	//	const __m256i c16_n41_p36 = _mm256_set1_epi32(0xFFD70024);
	//	const __m256i c16_p39_p02 = _mm256_set1_epi32(0x00270002);
	const __m256i c16_n44_p19 = _mm256_set1_epi32(0xFFD40013);
	const __m256i c16_n02_p36 = _mm256_set1_epi32(0xFFFE0024);
	const __m256i c16_p45_n34 = _mm256_set1_epi32(0x002DFFDE);
	const __m256i c16_n15_n23 = _mm256_set1_epi32(0xFFF1FFE9);
	//	const __m256i c16_n39_p43 = _mm256_set1_epi32(0xFFD9002B);
	//	const __m256i c16_p30_p07 = _mm256_set1_epi32(0x001E0007);
	//	const __m256i c16_p27_n45 = _mm256_set1_epi32(0x001BFFD3);
	//	const __m256i c16_n41_p11 = _mm256_set1_epi32(0xFFD7000B);
	const __m256i c16_n39_p15 = _mm256_set1_epi32(0xFFD9000F);
	const __m256i c16_n30_p45 = _mm256_set1_epi32(0xFFE2002D);
	const __m256i c16_p27_p02 = _mm256_set1_epi32(0x001B0002);
	const __m256i c16_p41_n44 = _mm256_set1_epi32(0x0029FFD4);
	//	const __m256i c16_n11_n19 = _mm256_set1_epi32(0xFFF5FFED);
	//	const __m256i c16_n45_p36 = _mm256_set1_epi32(0xFFD30024);
	//	const __m256i c16_n07_p34 = _mm256_set1_epi32(0xFFF90022);
	//	const __m256i c16_p43_n23 = _mm256_set1_epi32(0x002BFFE9);
	const __m256i c16_n30_p11 = _mm256_set1_epi32(0xFFE2000B);
	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);
	const __m256i c16_n19_p36 = _mm256_set1_epi32(0xFFED0024);
	const __m256i c16_p23_n02 = _mm256_set1_epi32(0x0017FFFE);
	//	const __m256i c16_p45_n39 = _mm256_set1_epi32(0x002DFFD9);
	//	const __m256i c16_p27_n41 = _mm256_set1_epi32(0x001BFFD7);
	//	const __m256i c16_n15_n07 = _mm256_set1_epi32(0xFFF1FFF9);
	//	const __m256i c16_n44_p34 = _mm256_set1_epi32(0xFFD40022);
	const __m256i c16_n19_p07 = _mm256_set1_epi32(0xFFED0007);
	const __m256i c16_n39_p30 = _mm256_set1_epi32(0xFFD9001E);
	const __m256i c16_n45_p44 = _mm256_set1_epi32(0xFFD3002C);
	const __m256i c16_n36_p43 = _mm256_set1_epi32(0xFFDC002B);
	//	const __m256i c16_n15_p27 = _mm256_set1_epi32(0xFFF1001B);
	//	const __m256i c16_p11_p02 = _mm256_set1_epi32(0x000B0002);
	//	const __m256i c16_p34_n23 = _mm256_set1_epi32(0x0022FFE9);
	//	const __m256i c16_p45_n41 = _mm256_set1_epi32(0x002DFFD7);
	const __m256i c16_n07_p02 = _mm256_set1_epi32(0xFFF90002);
	const __m256i c16_n15_p11 = _mm256_set1_epi32(0xFFF1000B);
	const __m256i c16_n23_p19 = _mm256_set1_epi32(0xFFE90013);
	const __m256i c16_n30_p27 = _mm256_set1_epi32(0xFFE2001B);
	//	const __m256i c16_n36_p34 = _mm256_set1_epi32(0xFFDC0022);
	//	const __m256i c16_n41_p39 = _mm256_set1_epi32(0xFFD70027);
	//	const __m256i c16_n44_p43 = _mm256_set1_epi32(0xFFD4002B);
	//	const __m256i c16_n45_p45 = _mm256_set1_epi32(0xFFD3002D);

	//	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	const __m256i c16_p35_p40 = _mm256_set1_epi32(0x00230028);
	//	const __m256i c16_p21_p29 = _mm256_set1_epi32(0x0015001D);
	//	const __m256i c16_p04_p13 = _mm256_set1_epi32(0x0004000D);
	const __m256i c16_p29_p43 = _mm256_set1_epi32(0x001D002B);
	const __m256i c16_n21_p04 = _mm256_set1_epi32(0xFFEB0004);
	//	const __m256i c16_n45_n40 = _mm256_set1_epi32(0xFFD3FFD8);
	//	const __m256i c16_n13_n35 = _mm256_set1_epi32(0xFFF3FFDD);
	const __m256i c16_p04_p40 = _mm256_set1_epi32(0x00040028);
	const __m256i c16_n43_n35 = _mm256_set1_epi32(0xFFD5FFDD);
	//	const __m256i c16_p29_n13 = _mm256_set1_epi32(0x001DFFF3);
	//	const __m256i c16_p21_p45 = _mm256_set1_epi32(0x0015002D);
	const __m256i c16_n21_p35 = _mm256_set1_epi32(0xFFEB0023);
	const __m256i c16_p04_n43 = _mm256_set1_epi32(0x0004FFD5);
	//	const __m256i c16_p13_p45 = _mm256_set1_epi32(0x000D002D);
	//	const __m256i c16_n29_n40 = _mm256_set1_epi32(0xFFE3FFD8);
	const __m256i c16_n40_p29 = _mm256_set1_epi32(0xFFD8001D);
	const __m256i c16_p45_n13 = _mm256_set1_epi32(0x002DFFF3);
	//	const __m256i c16_n43_n04 = _mm256_set1_epi32(0xFFD5FFFC);
	//	const __m256i c16_p35_p21 = _mm256_set1_epi32(0x00230015);
	const __m256i c16_n45_p21 = _mm256_set1_epi32(0xFFD30015);
	const __m256i c16_p13_p29 = _mm256_set1_epi32(0x000D001D);
	//	const __m256i c16_p35_n43 = _mm256_set1_epi32(0x0023FFD5);
	//	const __m256i c16_n40_p04 = _mm256_set1_epi32(0xFFD80004);
	const __m256i c16_n35_p13 = _mm256_set1_epi32(0xFFDD000D);
	const __m256i c16_n40_p45 = _mm256_set1_epi32(0xFFD8002D);
	//	const __m256i c16_p04_p21 = _mm256_set1_epi32(0x00040015);
	//	const __m256i c16_p43_n29 = _mm256_set1_epi32(0x002BFFE3);
	const __m256i c16_n13_p04 = _mm256_set1_epi32(0xFFF30004);
	const __m256i c16_n29_p21 = _mm256_set1_epi32(0xFFE30015);
	//	const __m256i c16_n40_p35 = _mm256_set1_epi32(0xFFD80023);
	//	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);

	const __m256i c16_p38_p44 = _mm256_set1_epi32(0x0026002C);
	//	const __m256i c16_p09_p25 = _mm256_set1_epi32(0x00090019);
	const __m256i c16_n09_p38 = _mm256_set1_epi32(0xFFF70026);
	//	const __m256i c16_n25_n44 = _mm256_set1_epi32(0xFFE7FFD4);

	const __m256i c16_n44_p25 = _mm256_set1_epi32(0xFFD40019);
	//	const __m256i c16_p38_p09 = _mm256_set1_epi32(0x00260009);
	const __m256i c16_n25_p09 = _mm256_set1_epi32(0xFFE70009);
	//	const __m256i c16_n44_p38 = _mm256_set1_epi32(0xFFD40026);

	const __m256i c16_p17_p42 = _mm256_set1_epi32(0x0011002A);
	const __m256i c16_n42_p17 = _mm256_set1_epi32(0xFFD60011);

	const __m256i c16_p32_p32 = _mm256_set1_epi32(0x00200020);
	const __m256i c16_n32_p32 = _mm256_set1_epi32(0xFFE00020);

	__m256i zero = _mm256_setzero_si256();
	__m256i c32_rnd = _mm256_set1_epi32(16);
	int nShift = 5;

	// DCT1
	__m256i in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2];
	__m256i in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2];
	__m256i res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2];
	__m256i res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2];

	int part;


	in00[0] = _mm256_lddqu_si256((const __m256i*)&src[0 * 32]);
	in01[0] = _mm256_lddqu_si256((const __m256i*)&src[1 * 32]);
	in02[0] = _mm256_lddqu_si256((const __m256i*)&src[2 * 32]);
	in03[0] = _mm256_lddqu_si256((const __m256i*)&src[3 * 32]);
	in04[0] = _mm256_lddqu_si256((const __m256i*)&src[4 * 32]);
	in05[0] = _mm256_lddqu_si256((const __m256i*)&src[5 * 32]);
	in06[0] = _mm256_lddqu_si256((const __m256i*)&src[6 * 32]);
	in07[0] = _mm256_lddqu_si256((const __m256i*)&src[7 * 32]);
	in08[0] = _mm256_lddqu_si256((const __m256i*)&src[8 * 32]);
	in09[0] = _mm256_lddqu_si256((const __m256i*)&src[9 * 32]);
	in10[0] = _mm256_lddqu_si256((const __m256i*)&src[10 * 32]);
	in11[0] = _mm256_lddqu_si256((const __m256i*)&src[11 * 32]);
	in12[0] = _mm256_lddqu_si256((const __m256i*)&src[12 * 32]);
	in13[0] = _mm256_lddqu_si256((const __m256i*)&src[13 * 32]);
	in14[0] = _mm256_lddqu_si256((const __m256i*)&src[14 * 32]);
	in15[0] = _mm256_lddqu_si256((const __m256i*)&src[15 * 32]);


	// First DCT
	{
		const __m256i T_00_00A = _mm256_unpacklo_epi16(in01[0], in03[0]);       // [33 13 32 12 31 11 30 10]
		const __m256i T_00_00B = _mm256_unpackhi_epi16(in01[0], in03[0]);       // [37 17 36 16 35 15 34 14]
		const __m256i T_00_01A = _mm256_unpacklo_epi16(in05[0], in07[0]);       // [ ]
		const __m256i T_00_01B = _mm256_unpackhi_epi16(in05[0], in07[0]);       // [ ]
		const __m256i T_00_02A = _mm256_unpacklo_epi16(in09[0], in11[0]);       // [ ]
		const __m256i T_00_02B = _mm256_unpackhi_epi16(in09[0], in11[0]);       // [ ]
		const __m256i T_00_03A = _mm256_unpacklo_epi16(in13[0], in15[0]);       // [ ]
		const __m256i T_00_03B = _mm256_unpackhi_epi16(in13[0], in15[0]);       // [ ]

		const __m256i T_00_08A = _mm256_unpacklo_epi16(in02[0], in06[0]);       // [ ]
		const __m256i T_00_08B = _mm256_unpackhi_epi16(in02[0], in06[0]);       // [ ]
		const __m256i T_00_09A = _mm256_unpacklo_epi16(in10[0], in14[0]);       // [ ]
		const __m256i T_00_09B = _mm256_unpackhi_epi16(in10[0], in14[0]);       // [ ]

		const __m256i T_00_12A = _mm256_unpacklo_epi16(in04[0], in12[0]);       // [ ]
		const __m256i T_00_12B = _mm256_unpackhi_epi16(in04[0], in12[0]);       // [ ]

		const __m256i T_00_14A = _mm256_unpacklo_epi16(in08[0], zero);       //
		const __m256i T_00_14B = _mm256_unpackhi_epi16(in08[0], zero);       // [ ]
		const __m256i T_00_15A = _mm256_unpacklo_epi16(in00[0], zero);       //
		const __m256i T_00_15B = _mm256_unpackhi_epi16(in00[0], zero);       // [ ]

		__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
		__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
		__m256i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
		__m256i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
		{
			__m256i T00, T01;
#define COMPUTE_ROW_PARTIAL(r0103, r0507, r0911, r1315, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(r0103, c0103), _mm256_madd_epi16(r0507, c0507)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(r0911, c0911), _mm256_madd_epi16(r1315, c1315)); \
	row = _mm256_add_epi32(_mm256_add_epi32(T00, T01), zero);

			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A);

			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B);

#undef COMPUTE_ROW_PARTIAL
		}


		{
			__m256i T00;
#define COMPUTE_ROW_PARTIAL(row0206, row1014, c0206, c1014, c1822, c2630, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(row0206, c0206), _mm256_madd_epi16(row1014, c1014)); \
	row = _mm256_add_epi32(T00, zero);

			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A);

			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B);
#undef COMPUTE_ROW_PARTIAL
		}

		EEO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_p38_p44), zero);
		EEO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n09_p38), zero);
		EEO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n44_p25), zero);
		EEO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n25_p09), zero);
		EEO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_p38_p44), zero);
		EEO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n09_p38), zero);
		EEO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n44_p25), zero);
		EEO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n25_p09), zero);

		EEEO0A = _mm256_madd_epi16(T_00_14A, c16_p17_p42);
		EEEO0B = _mm256_madd_epi16(T_00_14B, c16_p17_p42);
		EEEO1A = _mm256_madd_epi16(T_00_14A, c16_n42_p17);
		EEEO1B = _mm256_madd_epi16(T_00_14B, c16_n42_p17);

		EEEE0A = _mm256_madd_epi16(T_00_15A, c16_p32_p32);
		EEEE0B = _mm256_madd_epi16(T_00_15B, c16_p32_p32);
		EEEE1A = _mm256_madd_epi16(T_00_15A, c16_n32_p32);
		EEEE1B = _mm256_madd_epi16(T_00_15B, c16_n32_p32);

		EEE0A = _mm256_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
		EEE0B = _mm256_add_epi32(EEEE0B, EEEO0B);
		EEE1A = _mm256_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
		EEE1B = _mm256_add_epi32(EEEE1B, EEEO1B);
		EEE3A = _mm256_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
		EEE3B = _mm256_sub_epi32(EEEE0B, EEEO0B);
		EEE2A = _mm256_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
		EEE2B = _mm256_sub_epi32(EEEE1B, EEEO1B);

		EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
		EE0B = _mm256_add_epi32(EEE0B, EEO0B);
		EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
		EE1B = _mm256_add_epi32(EEE1B, EEO1B);
		EE2A = _mm256_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
		EE2B = _mm256_add_epi32(EEE2B, EEO2B);
		EE3A = _mm256_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
		EE3B = _mm256_add_epi32(EEE3B, EEO3B);
		EE7A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
		EE7B = _mm256_sub_epi32(EEE0B, EEO0B);
		EE6A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
		EE6B = _mm256_sub_epi32(EEE1B, EEO1B);
		EE5A = _mm256_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
		EE5B = _mm256_sub_epi32(EEE2B, EEO2B);
		EE4A = _mm256_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
		EE4B = _mm256_sub_epi32(EEE3B, EEO3B);

		E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
		E0B = _mm256_add_epi32(EE0B, EO0B);
		E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
		E1B = _mm256_add_epi32(EE1B, EO1B);
		E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
		E2B = _mm256_add_epi32(EE2B, EO2B);
		E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
		E3B = _mm256_add_epi32(EE3B, EO3B);
		E4A = _mm256_add_epi32(EE4A, EO4A);          // E4 =
		E4B = _mm256_add_epi32(EE4B, EO4B);
		E5A = _mm256_add_epi32(EE5A, EO5A);          // E5 =
		E5B = _mm256_add_epi32(EE5B, EO5B);
		E6A = _mm256_add_epi32(EE6A, EO6A);          // E6 =
		E6B = _mm256_add_epi32(EE6B, EO6B);
		E7A = _mm256_add_epi32(EE7A, EO7A);          // E7 =
		E7B = _mm256_add_epi32(EE7B, EO7B);
		EFA = _mm256_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
		EFB = _mm256_sub_epi32(EE0B, EO0B);
		EEA = _mm256_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
		EEB = _mm256_sub_epi32(EE1B, EO1B);
		EDA = _mm256_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
		EDB = _mm256_sub_epi32(EE2B, EO2B);
		ECA = _mm256_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
		ECB = _mm256_sub_epi32(EE3B, EO3B);
		EBA = _mm256_sub_epi32(EE4A, EO4A);          // EB =
		EBB = _mm256_sub_epi32(EE4B, EO4B);
		EAA = _mm256_sub_epi32(EE5A, EO5A);          // EA =
		EAB = _mm256_sub_epi32(EE5B, EO5B);
		E9A = _mm256_sub_epi32(EE6A, EO6A);          // E9 =
		E9B = _mm256_sub_epi32(EE6B, EO6B);
		E8A = _mm256_sub_epi32(EE7A, EO7A);          // E8 =
		E8B = _mm256_sub_epi32(EE7B, EO7B);

		T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
		T10B = _mm256_add_epi32(E0B, c32_rnd);
		T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
		T11B = _mm256_add_epi32(E1B, c32_rnd);
		T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
		T12B = _mm256_add_epi32(E2B, c32_rnd);
		T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
		T13B = _mm256_add_epi32(E3B, c32_rnd);
		T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
		T14B = _mm256_add_epi32(E4B, c32_rnd);
		T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
		T15B = _mm256_add_epi32(E5B, c32_rnd);
		T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
		T16B = _mm256_add_epi32(E6B, c32_rnd);
		T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
		T17B = _mm256_add_epi32(E7B, c32_rnd);
		T18A = _mm256_add_epi32(E8A, c32_rnd);         // E8 + rnd
		T18B = _mm256_add_epi32(E8B, c32_rnd);
		T19A = _mm256_add_epi32(E9A, c32_rnd);         // E9 + rnd
		T19B = _mm256_add_epi32(E9B, c32_rnd);
		T1AA = _mm256_add_epi32(EAA, c32_rnd);         // E10 + rnd
		T1AB = _mm256_add_epi32(EAB, c32_rnd);
		T1BA = _mm256_add_epi32(EBA, c32_rnd);         // E11 + rnd
		T1BB = _mm256_add_epi32(EBB, c32_rnd);
		T1CA = _mm256_add_epi32(ECA, c32_rnd);         // E12 + rnd
		T1CB = _mm256_add_epi32(ECB, c32_rnd);
		T1DA = _mm256_add_epi32(EDA, c32_rnd);         // E13 + rnd
		T1DB = _mm256_add_epi32(EDB, c32_rnd);
		T1EA = _mm256_add_epi32(EEA, c32_rnd);         // E14 + rnd
		T1EB = _mm256_add_epi32(EEB, c32_rnd);
		T1FA = _mm256_add_epi32(EFA, c32_rnd);         // E15 + rnd
		T1FB = _mm256_add_epi32(EFB, c32_rnd);

		T2_00A = _mm256_add_epi32(T10A, O00A);          // E0 + O0 + rnd
		T2_00B = _mm256_add_epi32(T10B, O00B);
		T2_01A = _mm256_add_epi32(T11A, O01A);          // E1 + O1 + rnd
		T2_01B = _mm256_add_epi32(T11B, O01B);
		T2_02A = _mm256_add_epi32(T12A, O02A);          // E2 + O2 + rnd
		T2_02B = _mm256_add_epi32(T12B, O02B);
		T2_03A = _mm256_add_epi32(T13A, O03A);          // E3 + O3 + rnd
		T2_03B = _mm256_add_epi32(T13B, O03B);
		T2_04A = _mm256_add_epi32(T14A, O04A);          // E4
		T2_04B = _mm256_add_epi32(T14B, O04B);
		T2_05A = _mm256_add_epi32(T15A, O05A);          // E5
		T2_05B = _mm256_add_epi32(T15B, O05B);
		T2_06A = _mm256_add_epi32(T16A, O06A);          // E6
		T2_06B = _mm256_add_epi32(T16B, O06B);
		T2_07A = _mm256_add_epi32(T17A, O07A);          // E7
		T2_07B = _mm256_add_epi32(T17B, O07B);
		T2_08A = _mm256_add_epi32(T18A, O08A);          // E8
		T2_08B = _mm256_add_epi32(T18B, O08B);
		T2_09A = _mm256_add_epi32(T19A, O09A);          // E9
		T2_09B = _mm256_add_epi32(T19B, O09B);
		T2_10A = _mm256_add_epi32(T1AA, O10A);          // E10
		T2_10B = _mm256_add_epi32(T1AB, O10B);
		T2_11A = _mm256_add_epi32(T1BA, O11A);          // E11
		T2_11B = _mm256_add_epi32(T1BB, O11B);
		T2_12A = _mm256_add_epi32(T1CA, O12A);          // E12
		T2_12B = _mm256_add_epi32(T1CB, O12B);
		T2_13A = _mm256_add_epi32(T1DA, O13A);          // E13
		T2_13B = _mm256_add_epi32(T1DB, O13B);
		T2_14A = _mm256_add_epi32(T1EA, O14A);          // E14
		T2_14B = _mm256_add_epi32(T1EB, O14B);
		T2_15A = _mm256_add_epi32(T1FA, O15A);          // E15
		T2_15B = _mm256_add_epi32(T1FB, O15B);
		T2_31A = _mm256_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
		T2_31B = _mm256_sub_epi32(T10B, O00B);
		T2_30A = _mm256_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
		T2_30B = _mm256_sub_epi32(T11B, O01B);
		T2_29A = _mm256_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
		T2_29B = _mm256_sub_epi32(T12B, O02B);
		T2_28A = _mm256_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
		T2_28B = _mm256_sub_epi32(T13B, O03B);
		T2_27A = _mm256_sub_epi32(T14A, O04A);          // E4
		T2_27B = _mm256_sub_epi32(T14B, O04B);
		T2_26A = _mm256_sub_epi32(T15A, O05A);          // E5
		T2_26B = _mm256_sub_epi32(T15B, O05B);
		T2_25A = _mm256_sub_epi32(T16A, O06A);          // E6
		T2_25B = _mm256_sub_epi32(T16B, O06B);
		T2_24A = _mm256_sub_epi32(T17A, O07A);          // E7
		T2_24B = _mm256_sub_epi32(T17B, O07B);
		T2_23A = _mm256_sub_epi32(T18A, O08A);          //
		T2_23B = _mm256_sub_epi32(T18B, O08B);
		T2_22A = _mm256_sub_epi32(T19A, O09A);          //
		T2_22B = _mm256_sub_epi32(T19B, O09B);
		T2_21A = _mm256_sub_epi32(T1AA, O10A);          //
		T2_21B = _mm256_sub_epi32(T1AB, O10B);
		T2_20A = _mm256_sub_epi32(T1BA, O11A);          //
		T2_20B = _mm256_sub_epi32(T1BB, O11B);
		T2_19A = _mm256_sub_epi32(T1CA, O12A);          //
		T2_19B = _mm256_sub_epi32(T1CB, O12B);
		T2_18A = _mm256_sub_epi32(T1DA, O13A);          //
		T2_18B = _mm256_sub_epi32(T1DB, O13B);
		T2_17A = _mm256_sub_epi32(T1EA, O14A);          //
		T2_17B = _mm256_sub_epi32(T1EB, O14B);
		T2_16A = _mm256_sub_epi32(T1FA, O15A);          //
		T2_16B = _mm256_sub_epi32(T1FB, O15B);

		T3_00A = _mm256_srai_epi32(T2_00A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_00B = _mm256_srai_epi32(T2_00B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_01A = _mm256_srai_epi32(T2_01A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_01B = _mm256_srai_epi32(T2_01B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
		T3_02A = _mm256_srai_epi32(T2_02A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
		T3_02B = _mm256_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
		T3_03A = _mm256_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
		T3_03B = _mm256_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
		T3_04A = _mm256_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
		T3_04B = _mm256_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
		T3_05A = _mm256_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
		T3_05B = _mm256_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
		T3_06A = _mm256_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
		T3_06B = _mm256_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
		T3_07A = _mm256_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
		T3_07B = _mm256_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
		T3_08A = _mm256_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
		T3_08B = _mm256_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
		T3_09A = _mm256_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
		T3_09B = _mm256_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
		T3_10A = _mm256_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
		T3_10B = _mm256_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
		T3_11A = _mm256_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
		T3_11B = _mm256_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
		T3_12A = _mm256_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
		T3_12B = _mm256_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
		T3_13A = _mm256_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
		T3_13B = _mm256_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
		T3_14A = _mm256_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
		T3_14B = _mm256_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
		T3_15A = _mm256_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
		T3_15B = _mm256_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

		T3_16A = _mm256_srai_epi32(T2_16A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_16B = _mm256_srai_epi32(T2_16B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_17A = _mm256_srai_epi32(T2_17A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_17B = _mm256_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
		T3_18A = _mm256_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
		T3_18B = _mm256_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
		T3_19A = _mm256_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
		T3_19B = _mm256_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
		T3_20A = _mm256_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
		T3_20B = _mm256_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
		T3_21A = _mm256_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
		T3_21B = _mm256_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
		T3_22A = _mm256_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
		T3_22B = _mm256_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
		T3_23A = _mm256_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
		T3_23B = _mm256_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
		T3_24A = _mm256_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
		T3_24B = _mm256_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
		T3_25A = _mm256_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
		T3_25B = _mm256_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
		T3_26A = _mm256_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
		T3_26B = _mm256_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
		T3_27A = _mm256_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
		T3_27B = _mm256_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
		T3_28A = _mm256_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
		T3_28B = _mm256_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
		T3_29A = _mm256_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
		T3_29B = _mm256_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
		T3_30A = _mm256_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
		T3_30B = _mm256_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
		T3_31A = _mm256_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
		T3_31B = _mm256_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

		res00[0] = _mm256_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
		res01[0] = _mm256_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
		res02[0] = _mm256_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
		res03[0] = _mm256_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
		res04[0] = _mm256_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
		res05[0] = _mm256_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
		res06[0] = _mm256_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
		res07[0] = _mm256_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
		res08[0] = _mm256_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
		res09[0] = _mm256_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
		res10[0] = _mm256_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
		res11[0] = _mm256_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
		res12[0] = _mm256_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
		res13[0] = _mm256_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
		res14[0] = _mm256_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
		res15[0] = _mm256_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
		res16[0] = _mm256_packs_epi32(T3_16A, T3_16B);
		res17[0] = _mm256_packs_epi32(T3_17A, T3_17B);
		res18[0] = _mm256_packs_epi32(T3_18A, T3_18B);
		res19[0] = _mm256_packs_epi32(T3_19A, T3_19B);
		res20[0] = _mm256_packs_epi32(T3_20A, T3_20B);
		res21[0] = _mm256_packs_epi32(T3_21A, T3_21B);
		res22[0] = _mm256_packs_epi32(T3_22A, T3_22B);
		res23[0] = _mm256_packs_epi32(T3_23A, T3_23B);
		res24[0] = _mm256_packs_epi32(T3_24A, T3_24B);
		res25[0] = _mm256_packs_epi32(T3_25A, T3_25B);
		res26[0] = _mm256_packs_epi32(T3_26A, T3_26B);
		res27[0] = _mm256_packs_epi32(T3_27A, T3_27B);
		res28[0] = _mm256_packs_epi32(T3_28A, T3_28B);
		res29[0] = _mm256_packs_epi32(T3_29A, T3_29B);
		res30[0] = _mm256_packs_epi32(T3_30A, T3_30B);
		res31[0] = _mm256_packs_epi32(T3_31A, T3_31B);

	}


	//transpose 32x32 matrix
	{
		__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
	tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
	tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
	tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
	tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
	tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
	tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
	tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
	tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
	O0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	O1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	O2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	O3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	O4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	O5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	O6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	O7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	O8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
	O9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
	O10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
	O11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
	O12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
	O13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
	O14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
	O15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
	tr0_0 = _mm256_unpacklo_epi64(O0, O1); \
	tr0_1 = _mm256_unpacklo_epi64(O2, O3); \
	tr0_2 = _mm256_unpackhi_epi64(O0, O1); \
	tr0_3 = _mm256_unpackhi_epi64(O2, O3); \
	tr0_4 = _mm256_unpacklo_epi64(O4, O5); \
	tr0_5 = _mm256_unpacklo_epi64(O6, O7); \
	tr0_6 = _mm256_unpackhi_epi64(O4, O5); \
	tr0_7 = _mm256_unpackhi_epi64(O6, O7); \
	tr0_8 = _mm256_unpacklo_epi64(O8, O9); \
	tr0_9 = _mm256_unpacklo_epi64(O10, O11); \
	tr0_10 = _mm256_unpackhi_epi64(O8, O9); \
	tr0_11 = _mm256_unpackhi_epi64(O10, O11); \
	tr0_12 = _mm256_unpacklo_epi64(O12, O13); \
	tr0_13 = _mm256_unpacklo_epi64(O14, O15); \
	tr0_14 = _mm256_unpackhi_epi64(O12, O13); \
	tr0_15 = _mm256_unpackhi_epi64(O14, O15); \
	O0 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x20); \
	O1 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x20); \
	O2 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x20); \
	O3 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x20); \
	O4 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x20); \
	O5 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x20); \
	O6 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x20); \
	O7 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x20); \
	O8 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x31); \
	O9 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x31); \
	O10 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x31); \
	O11 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x31); \
	O12 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x31); \
	O13 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x31); \
	O14 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x31); \
	O15 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x31); \

		TRANSPOSE_16x16_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
		TRANSPOSE_16x16_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);

#undef TRANSPOSE_16x16_16BIT
	}


	// Second IDCT
	c32_rnd = _mm256_set1_epi32(2048);				// pass == 1 第二次四舍五入
	nShift = 12;


	for (part = 0; part < 2; part++)
	{
		const __m256i T_00_00A = _mm256_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
		const __m256i T_00_00B = _mm256_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
		const __m256i T_00_01A = _mm256_unpacklo_epi16(in05[part], in07[part]);       // [ ]
		const __m256i T_00_01B = _mm256_unpackhi_epi16(in05[part], in07[part]);       // [ ]
		const __m256i T_00_02A = _mm256_unpacklo_epi16(in09[part], in11[part]);       // [ ]
		const __m256i T_00_02B = _mm256_unpackhi_epi16(in09[part], in11[part]);       // [ ]
		const __m256i T_00_03A = _mm256_unpacklo_epi16(in13[part], in15[part]);       // [ ]
		const __m256i T_00_03B = _mm256_unpackhi_epi16(in13[part], in15[part]);       // [ ]

		const __m256i T_00_08A = _mm256_unpacklo_epi16(in02[part], in06[part]);       // [ ]
		const __m256i T_00_08B = _mm256_unpackhi_epi16(in02[part], in06[part]);       // [ ]
		const __m256i T_00_09A = _mm256_unpacklo_epi16(in10[part], in14[part]);       // [ ]
		const __m256i T_00_09B = _mm256_unpackhi_epi16(in10[part], in14[part]);       // [ ]

		const __m256i T_00_12A = _mm256_unpacklo_epi16(in04[part], in12[part]);       // [ ]
		const __m256i T_00_12B = _mm256_unpackhi_epi16(in04[part], in12[part]);       // [ ]

		const __m256i T_00_14A = _mm256_unpacklo_epi16(in08[part], zero);       //
		const __m256i T_00_14B = _mm256_unpackhi_epi16(in08[part], zero);       // [ ]
		const __m256i T_00_15A = _mm256_unpacklo_epi16(in00[part], zero);       //
		const __m256i T_00_15B = _mm256_unpackhi_epi16(in00[part], zero);       // [ ]

		__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
		__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
		__m256i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
		__m256i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
		{
			__m256i T00, T01;
#define COMPUTE_ROW_PARTIAL(r0103, r0507, r0911, r1315, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(r0103, c0103), _mm256_madd_epi16(r0507, c0507)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(r0911, c0911), _mm256_madd_epi16(r1315, c1315)); \
	row = _mm256_add_epi32(_mm256_add_epi32(T00, T01), zero);

			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A);

			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B);

#undef COMPUTE_ROW_PARTIAL
		}


		{
			__m256i T00;
#define COMPUTE_ROW_PARTIAL(row0206, row1014, c0206, c1014, c1822, c2630, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(row0206, c0206), _mm256_madd_epi16(row1014, c1014)); \
	row = _mm256_add_epi32(T00, zero);

			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A);

			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B);
#undef COMPUTE_ROW_PARTIAL
		}

		EEO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_p38_p44), zero);
		EEO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n09_p38), zero);
		EEO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n44_p25), zero);
		EEO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n25_p09), zero);
		EEO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_p38_p44), zero);
		EEO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n09_p38), zero);
		EEO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n44_p25), zero);
		EEO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n25_p09), zero);

		EEEO0A = _mm256_madd_epi16(T_00_14A, c16_p17_p42);
		EEEO0B = _mm256_madd_epi16(T_00_14B, c16_p17_p42);
		EEEO1A = _mm256_madd_epi16(T_00_14A, c16_n42_p17);
		EEEO1B = _mm256_madd_epi16(T_00_14B, c16_n42_p17);

		EEEE0A = _mm256_madd_epi16(T_00_15A, c16_p32_p32);
		EEEE0B = _mm256_madd_epi16(T_00_15B, c16_p32_p32);
		EEEE1A = _mm256_madd_epi16(T_00_15A, c16_n32_p32);
		EEEE1B = _mm256_madd_epi16(T_00_15B, c16_n32_p32);

		EEE0A = _mm256_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
		EEE0B = _mm256_add_epi32(EEEE0B, EEEO0B);
		EEE1A = _mm256_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
		EEE1B = _mm256_add_epi32(EEEE1B, EEEO1B);
		EEE3A = _mm256_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
		EEE3B = _mm256_sub_epi32(EEEE0B, EEEO0B);
		EEE2A = _mm256_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
		EEE2B = _mm256_sub_epi32(EEEE1B, EEEO1B);

		EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
		EE0B = _mm256_add_epi32(EEE0B, EEO0B);
		EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
		EE1B = _mm256_add_epi32(EEE1B, EEO1B);
		EE2A = _mm256_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
		EE2B = _mm256_add_epi32(EEE2B, EEO2B);
		EE3A = _mm256_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
		EE3B = _mm256_add_epi32(EEE3B, EEO3B);
		EE7A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
		EE7B = _mm256_sub_epi32(EEE0B, EEO0B);
		EE6A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
		EE6B = _mm256_sub_epi32(EEE1B, EEO1B);
		EE5A = _mm256_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
		EE5B = _mm256_sub_epi32(EEE2B, EEO2B);
		EE4A = _mm256_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
		EE4B = _mm256_sub_epi32(EEE3B, EEO3B);

		E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
		E0B = _mm256_add_epi32(EE0B, EO0B);
		E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
		E1B = _mm256_add_epi32(EE1B, EO1B);
		E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
		E2B = _mm256_add_epi32(EE2B, EO2B);
		E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
		E3B = _mm256_add_epi32(EE3B, EO3B);
		E4A = _mm256_add_epi32(EE4A, EO4A);          // E4 =
		E4B = _mm256_add_epi32(EE4B, EO4B);
		E5A = _mm256_add_epi32(EE5A, EO5A);          // E5 =
		E5B = _mm256_add_epi32(EE5B, EO5B);
		E6A = _mm256_add_epi32(EE6A, EO6A);          // E6 =
		E6B = _mm256_add_epi32(EE6B, EO6B);
		E7A = _mm256_add_epi32(EE7A, EO7A);          // E7 =
		E7B = _mm256_add_epi32(EE7B, EO7B);
		EFA = _mm256_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
		EFB = _mm256_sub_epi32(EE0B, EO0B);
		EEA = _mm256_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
		EEB = _mm256_sub_epi32(EE1B, EO1B);
		EDA = _mm256_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
		EDB = _mm256_sub_epi32(EE2B, EO2B);
		ECA = _mm256_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
		ECB = _mm256_sub_epi32(EE3B, EO3B);
		EBA = _mm256_sub_epi32(EE4A, EO4A);          // EB =
		EBB = _mm256_sub_epi32(EE4B, EO4B);
		EAA = _mm256_sub_epi32(EE5A, EO5A);          // EA =
		EAB = _mm256_sub_epi32(EE5B, EO5B);
		E9A = _mm256_sub_epi32(EE6A, EO6A);          // E9 =
		E9B = _mm256_sub_epi32(EE6B, EO6B);
		E8A = _mm256_sub_epi32(EE7A, EO7A);          // E8 =
		E8B = _mm256_sub_epi32(EE7B, EO7B);

		T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
		T10B = _mm256_add_epi32(E0B, c32_rnd);
		T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
		T11B = _mm256_add_epi32(E1B, c32_rnd);
		T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
		T12B = _mm256_add_epi32(E2B, c32_rnd);
		T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
		T13B = _mm256_add_epi32(E3B, c32_rnd);
		T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
		T14B = _mm256_add_epi32(E4B, c32_rnd);
		T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
		T15B = _mm256_add_epi32(E5B, c32_rnd);
		T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
		T16B = _mm256_add_epi32(E6B, c32_rnd);
		T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
		T17B = _mm256_add_epi32(E7B, c32_rnd);
		T18A = _mm256_add_epi32(E8A, c32_rnd);         // E8 + rnd
		T18B = _mm256_add_epi32(E8B, c32_rnd);
		T19A = _mm256_add_epi32(E9A, c32_rnd);         // E9 + rnd
		T19B = _mm256_add_epi32(E9B, c32_rnd);
		T1AA = _mm256_add_epi32(EAA, c32_rnd);         // E10 + rnd
		T1AB = _mm256_add_epi32(EAB, c32_rnd);
		T1BA = _mm256_add_epi32(EBA, c32_rnd);         // E11 + rnd
		T1BB = _mm256_add_epi32(EBB, c32_rnd);
		T1CA = _mm256_add_epi32(ECA, c32_rnd);         // E12 + rnd
		T1CB = _mm256_add_epi32(ECB, c32_rnd);
		T1DA = _mm256_add_epi32(EDA, c32_rnd);         // E13 + rnd
		T1DB = _mm256_add_epi32(EDB, c32_rnd);
		T1EA = _mm256_add_epi32(EEA, c32_rnd);         // E14 + rnd
		T1EB = _mm256_add_epi32(EEB, c32_rnd);
		T1FA = _mm256_add_epi32(EFA, c32_rnd);         // E15 + rnd
		T1FB = _mm256_add_epi32(EFB, c32_rnd);

		T2_00A = _mm256_add_epi32(T10A, O00A);          // E0 + O0 + rnd
		T2_00B = _mm256_add_epi32(T10B, O00B);
		T2_01A = _mm256_add_epi32(T11A, O01A);          // E1 + O1 + rnd
		T2_01B = _mm256_add_epi32(T11B, O01B);
		T2_02A = _mm256_add_epi32(T12A, O02A);          // E2 + O2 + rnd
		T2_02B = _mm256_add_epi32(T12B, O02B);
		T2_03A = _mm256_add_epi32(T13A, O03A);          // E3 + O3 + rnd
		T2_03B = _mm256_add_epi32(T13B, O03B);
		T2_04A = _mm256_add_epi32(T14A, O04A);          // E4
		T2_04B = _mm256_add_epi32(T14B, O04B);
		T2_05A = _mm256_add_epi32(T15A, O05A);          // E5
		T2_05B = _mm256_add_epi32(T15B, O05B);
		T2_06A = _mm256_add_epi32(T16A, O06A);          // E6
		T2_06B = _mm256_add_epi32(T16B, O06B);
		T2_07A = _mm256_add_epi32(T17A, O07A);          // E7
		T2_07B = _mm256_add_epi32(T17B, O07B);
		T2_08A = _mm256_add_epi32(T18A, O08A);          // E8
		T2_08B = _mm256_add_epi32(T18B, O08B);
		T2_09A = _mm256_add_epi32(T19A, O09A);          // E9
		T2_09B = _mm256_add_epi32(T19B, O09B);
		T2_10A = _mm256_add_epi32(T1AA, O10A);          // E10
		T2_10B = _mm256_add_epi32(T1AB, O10B);
		T2_11A = _mm256_add_epi32(T1BA, O11A);          // E11
		T2_11B = _mm256_add_epi32(T1BB, O11B);
		T2_12A = _mm256_add_epi32(T1CA, O12A);          // E12
		T2_12B = _mm256_add_epi32(T1CB, O12B);
		T2_13A = _mm256_add_epi32(T1DA, O13A);          // E13
		T2_13B = _mm256_add_epi32(T1DB, O13B);
		T2_14A = _mm256_add_epi32(T1EA, O14A);          // E14
		T2_14B = _mm256_add_epi32(T1EB, O14B);
		T2_15A = _mm256_add_epi32(T1FA, O15A);          // E15
		T2_15B = _mm256_add_epi32(T1FB, O15B);
		T2_31A = _mm256_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
		T2_31B = _mm256_sub_epi32(T10B, O00B);
		T2_30A = _mm256_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
		T2_30B = _mm256_sub_epi32(T11B, O01B);
		T2_29A = _mm256_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
		T2_29B = _mm256_sub_epi32(T12B, O02B);
		T2_28A = _mm256_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
		T2_28B = _mm256_sub_epi32(T13B, O03B);
		T2_27A = _mm256_sub_epi32(T14A, O04A);          // E4
		T2_27B = _mm256_sub_epi32(T14B, O04B);
		T2_26A = _mm256_sub_epi32(T15A, O05A);          // E5
		T2_26B = _mm256_sub_epi32(T15B, O05B);
		T2_25A = _mm256_sub_epi32(T16A, O06A);          // E6
		T2_25B = _mm256_sub_epi32(T16B, O06B);
		T2_24A = _mm256_sub_epi32(T17A, O07A);          // E7
		T2_24B = _mm256_sub_epi32(T17B, O07B);
		T2_23A = _mm256_sub_epi32(T18A, O08A);          //
		T2_23B = _mm256_sub_epi32(T18B, O08B);
		T2_22A = _mm256_sub_epi32(T19A, O09A);          //
		T2_22B = _mm256_sub_epi32(T19B, O09B);
		T2_21A = _mm256_sub_epi32(T1AA, O10A);          //
		T2_21B = _mm256_sub_epi32(T1AB, O10B);
		T2_20A = _mm256_sub_epi32(T1BA, O11A);          //
		T2_20B = _mm256_sub_epi32(T1BB, O11B);
		T2_19A = _mm256_sub_epi32(T1CA, O12A);          //
		T2_19B = _mm256_sub_epi32(T1CB, O12B);
		T2_18A = _mm256_sub_epi32(T1DA, O13A);          //
		T2_18B = _mm256_sub_epi32(T1DB, O13B);
		T2_17A = _mm256_sub_epi32(T1EA, O14A);          //
		T2_17B = _mm256_sub_epi32(T1EB, O14B);
		T2_16A = _mm256_sub_epi32(T1FA, O15A);          //
		T2_16B = _mm256_sub_epi32(T1FB, O15B);

		T3_00A = _mm256_srai_epi32(T2_00A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_00B = _mm256_srai_epi32(T2_00B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_01A = _mm256_srai_epi32(T2_01A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_01B = _mm256_srai_epi32(T2_01B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
		T3_02A = _mm256_srai_epi32(T2_02A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
		T3_02B = _mm256_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
		T3_03A = _mm256_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
		T3_03B = _mm256_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
		T3_04A = _mm256_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
		T3_04B = _mm256_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
		T3_05A = _mm256_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
		T3_05B = _mm256_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
		T3_06A = _mm256_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
		T3_06B = _mm256_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
		T3_07A = _mm256_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
		T3_07B = _mm256_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
		T3_08A = _mm256_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
		T3_08B = _mm256_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
		T3_09A = _mm256_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
		T3_09B = _mm256_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
		T3_10A = _mm256_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
		T3_10B = _mm256_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
		T3_11A = _mm256_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
		T3_11B = _mm256_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
		T3_12A = _mm256_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
		T3_12B = _mm256_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
		T3_13A = _mm256_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
		T3_13B = _mm256_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
		T3_14A = _mm256_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
		T3_14B = _mm256_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
		T3_15A = _mm256_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
		T3_15B = _mm256_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

		T3_16A = _mm256_srai_epi32(T2_16A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_16B = _mm256_srai_epi32(T2_16B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_17A = _mm256_srai_epi32(T2_17A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_17B = _mm256_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
		T3_18A = _mm256_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
		T3_18B = _mm256_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
		T3_19A = _mm256_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
		T3_19B = _mm256_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
		T3_20A = _mm256_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
		T3_20B = _mm256_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
		T3_21A = _mm256_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
		T3_21B = _mm256_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
		T3_22A = _mm256_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
		T3_22B = _mm256_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
		T3_23A = _mm256_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
		T3_23B = _mm256_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
		T3_24A = _mm256_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
		T3_24B = _mm256_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
		T3_25A = _mm256_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
		T3_25B = _mm256_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
		T3_26A = _mm256_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
		T3_26B = _mm256_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
		T3_27A = _mm256_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
		T3_27B = _mm256_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
		T3_28A = _mm256_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
		T3_28B = _mm256_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
		T3_29A = _mm256_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
		T3_29B = _mm256_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
		T3_30A = _mm256_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
		T3_30B = _mm256_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
		T3_31A = _mm256_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
		T3_31B = _mm256_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

		res00[part] = _mm256_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
		res01[part] = _mm256_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
		res02[part] = _mm256_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
		res03[part] = _mm256_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
		res04[part] = _mm256_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
		res05[part] = _mm256_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
		res06[part] = _mm256_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
		res07[part] = _mm256_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
		res08[part] = _mm256_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
		res09[part] = _mm256_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
		res10[part] = _mm256_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
		res11[part] = _mm256_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
		res12[part] = _mm256_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
		res13[part] = _mm256_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
		res14[part] = _mm256_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
		res15[part] = _mm256_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
		res16[part] = _mm256_packs_epi32(T3_16A, T3_16B);
		res17[part] = _mm256_packs_epi32(T3_17A, T3_17B);
		res18[part] = _mm256_packs_epi32(T3_18A, T3_18B);
		res19[part] = _mm256_packs_epi32(T3_19A, T3_19B);
		res20[part] = _mm256_packs_epi32(T3_20A, T3_20B);
		res21[part] = _mm256_packs_epi32(T3_21A, T3_21B);
		res22[part] = _mm256_packs_epi32(T3_22A, T3_22B);
		res23[part] = _mm256_packs_epi32(T3_23A, T3_23B);
		res24[part] = _mm256_packs_epi32(T3_24A, T3_24B);
		res25[part] = _mm256_packs_epi32(T3_25A, T3_25B);
		res26[part] = _mm256_packs_epi32(T3_26A, T3_26B);
		res27[part] = _mm256_packs_epi32(T3_27A, T3_27B);
		res28[part] = _mm256_packs_epi32(T3_28A, T3_28B);
		res29[part] = _mm256_packs_epi32(T3_29A, T3_29B);
		res30[part] = _mm256_packs_epi32(T3_30A, T3_30B);
		res31[part] = _mm256_packs_epi32(T3_31A, T3_31B);

	}

	//transpose 32x32 matrix
	//Not the all transpose.
	//Considering the add predict pixel
	{
		__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT_PARTIAL(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
	tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
	tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
	tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
	tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
	tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
	tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
	tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
	tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
	I0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	I1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	I2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	I3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	I4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	I5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	I6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	I7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	I8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
	I9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
	I10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
	I11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
	I12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
	I13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
	I14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
	I15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
	O0 = _mm256_unpacklo_epi64(I0, I1); \
	O1 = _mm256_unpacklo_epi64(I2, I3); \
	O2 = _mm256_unpackhi_epi64(I0, I1); \
	O3 = _mm256_unpackhi_epi64(I2, I3); \
	O4 = _mm256_unpacklo_epi64(I4, I5); \
	O5 = _mm256_unpacklo_epi64(I6, I7); \
	O6 = _mm256_unpackhi_epi64(I4, I5); \
	O7 = _mm256_unpackhi_epi64(I6, I7); \
	O8 = _mm256_unpacklo_epi64(I8, I9); \
	O9 = _mm256_unpacklo_epi64(I10, I11); \
	O10 = _mm256_unpackhi_epi64(I8, I9); \
	O11 = _mm256_unpackhi_epi64(I10, I11); \
	O12 = _mm256_unpacklo_epi64(I12, I13); \
	O13 = _mm256_unpacklo_epi64(I14, I15); \
	O14 = _mm256_unpackhi_epi64(I12, I13); \
	O15 = _mm256_unpackhi_epi64(I14, I15); \

		TRANSPOSE_16x16_16BIT_PARTIAL(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
		TRANSPOSE_16x16_16BIT_PARTIAL(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);
		TRANSPOSE_16x16_16BIT_PARTIAL(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0]);
		TRANSPOSE_16x16_16BIT_PARTIAL(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1]);

#undef TRANSPOSE_16x16_16BIT

	}




	//clip
	max_val = _mm256_set1_epi16(255);
	min_val = _mm256_set1_epi16(-256);

	for (k = 0; k < 2; k++)
	{
		in00[k] = _mm256_min_epi16(in00[k], max_val);
		in00[k] = _mm256_max_epi16(in00[k], min_val);
		in01[k] = _mm256_min_epi16(in01[k], max_val);
		in01[k] = _mm256_max_epi16(in01[k], min_val);
		in02[k] = _mm256_min_epi16(in02[k], max_val);
		in02[k] = _mm256_max_epi16(in02[k], min_val);
		in03[k] = _mm256_min_epi16(in03[k], max_val);
		in03[k] = _mm256_max_epi16(in03[k], min_val);
		in04[k] = _mm256_min_epi16(in04[k], max_val);
		in04[k] = _mm256_max_epi16(in04[k], min_val);
		in05[k] = _mm256_min_epi16(in05[k], max_val);
		in05[k] = _mm256_max_epi16(in05[k], min_val);
		in06[k] = _mm256_min_epi16(in06[k], max_val);
		in06[k] = _mm256_max_epi16(in06[k], min_val);
		in07[k] = _mm256_min_epi16(in07[k], max_val);
		in07[k] = _mm256_max_epi16(in07[k], min_val);
		in08[k] = _mm256_min_epi16(in08[k], max_val);
		in08[k] = _mm256_max_epi16(in08[k], min_val);
		in09[k] = _mm256_min_epi16(in09[k], max_val);
		in09[k] = _mm256_max_epi16(in09[k], min_val);
		in10[k] = _mm256_min_epi16(in10[k], max_val);
		in10[k] = _mm256_max_epi16(in10[k], min_val);
		in11[k] = _mm256_min_epi16(in11[k], max_val);
		in11[k] = _mm256_max_epi16(in11[k], min_val);
		in12[k] = _mm256_min_epi16(in12[k], max_val);
		in12[k] = _mm256_max_epi16(in12[k], min_val);
		in13[k] = _mm256_min_epi16(in13[k], max_val);
		in13[k] = _mm256_max_epi16(in13[k], min_val);
		in14[k] = _mm256_min_epi16(in14[k], max_val);
		in14[k] = _mm256_max_epi16(in14[k], min_val);
		in15[k] = _mm256_min_epi16(in15[k], max_val);
		in15[k] = _mm256_max_epi16(in15[k], min_val);
		in16[k] = _mm256_min_epi16(in16[k], max_val);
		in16[k] = _mm256_max_epi16(in16[k], min_val);
		in17[k] = _mm256_min_epi16(in17[k], max_val);
		in17[k] = _mm256_max_epi16(in17[k], min_val);
		in18[k] = _mm256_min_epi16(in18[k], max_val);
		in18[k] = _mm256_max_epi16(in18[k], min_val);
		in19[k] = _mm256_min_epi16(in19[k], max_val);
		in19[k] = _mm256_max_epi16(in19[k], min_val);
		in20[k] = _mm256_min_epi16(in20[k], max_val);
		in20[k] = _mm256_max_epi16(in20[k], min_val);
		in21[k] = _mm256_min_epi16(in21[k], max_val);
		in21[k] = _mm256_max_epi16(in21[k], min_val);
		in22[k] = _mm256_min_epi16(in22[k], max_val);
		in22[k] = _mm256_max_epi16(in22[k], min_val);
		in23[k] = _mm256_min_epi16(in23[k], max_val);
		in23[k] = _mm256_max_epi16(in23[k], min_val);
		in24[k] = _mm256_min_epi16(in24[k], max_val);
		in24[k] = _mm256_max_epi16(in24[k], min_val);
		in25[k] = _mm256_min_epi16(in25[k], max_val);
		in25[k] = _mm256_max_epi16(in25[k], min_val);
		in26[k] = _mm256_min_epi16(in26[k], max_val);
		in26[k] = _mm256_max_epi16(in26[k], min_val);
		in27[k] = _mm256_min_epi16(in27[k], max_val);
		in27[k] = _mm256_max_epi16(in27[k], min_val);
		in28[k] = _mm256_min_epi16(in28[k], max_val);
		in28[k] = _mm256_max_epi16(in28[k], min_val);
		in29[k] = _mm256_min_epi16(in29[k], max_val);
		in29[k] = _mm256_max_epi16(in29[k], min_val);
		in30[k] = _mm256_min_epi16(in30[k], max_val);
		in30[k] = _mm256_max_epi16(in30[k], min_val);
		in31[k] = _mm256_min_epi16(in31[k], max_val);
		in31[k] = _mm256_max_epi16(in31[k], min_val);
	}

	// Add
	P00 = _mm256_loadu2_m128i((const __m128i*)&pred[8 * i_pred], (const __m128i*)&pred[0]);
	P01 = _mm256_loadu2_m128i((const __m128i*)&pred[9 * i_pred], (const __m128i*)&pred[1 * i_pred]);
	P02 = _mm256_loadu2_m128i((const __m128i*)&pred[10 * i_pred], (const __m128i*)&pred[2 * i_pred]);
	P03 = _mm256_loadu2_m128i((const __m128i*)&pred[11 * i_pred], (const __m128i*)&pred[3 * i_pred]);
	P04 = _mm256_loadu2_m128i((const __m128i*)&pred[12 * i_pred], (const __m128i*)&pred[4 * i_pred]);
	P05 = _mm256_loadu2_m128i((const __m128i*)&pred[13 * i_pred], (const __m128i*)&pred[5 * i_pred]);
	P06 = _mm256_loadu2_m128i((const __m128i*)&pred[14 * i_pred], (const __m128i*)&pred[6 * i_pred]);
	P07 = _mm256_loadu2_m128i((const __m128i*)&pred[15 * i_pred], (const __m128i*)&pred[7 * i_pred]);

	P08 = _mm256_loadu2_m128i((const __m128i*)&pred[8 * i_pred + 16], (const __m128i*)&pred[0 + 16]);
	P09 = _mm256_loadu2_m128i((const __m128i*)&pred[9 * i_pred + 16], (const __m128i*)&pred[1 * i_pred + 16]);
	P10 = _mm256_loadu2_m128i((const __m128i*)&pred[10 * i_pred + 16], (const __m128i*)&pred[2 * i_pred + 16]);
	P11 = _mm256_loadu2_m128i((const __m128i*)&pred[11 * i_pred + 16], (const __m128i*)&pred[3 * i_pred + 16]);
	P12 = _mm256_loadu2_m128i((const __m128i*)&pred[12 * i_pred + 16], (const __m128i*)&pred[4 * i_pred + 16]);
	P13 = _mm256_loadu2_m128i((const __m128i*)&pred[13 * i_pred + 16], (const __m128i*)&pred[5 * i_pred + 16]);
	P14 = _mm256_loadu2_m128i((const __m128i*)&pred[14 * i_pred + 16], (const __m128i*)&pred[6 * i_pred + 16]);
	P15 = _mm256_loadu2_m128i((const __m128i*)&pred[15 * i_pred + 16], (const __m128i*)&pred[7 * i_pred + 16]);

	P16 = _mm256_loadu2_m128i((const __m128i*)&pred[24 * i_pred], (const __m128i*)&pred[16 * i_pred]);
	P17 = _mm256_loadu2_m128i((const __m128i*)&pred[25 * i_pred], (const __m128i*)&pred[17 * i_pred]);
	P18 = _mm256_loadu2_m128i((const __m128i*)&pred[26 * i_pred], (const __m128i*)&pred[18 * i_pred]);
	P19 = _mm256_loadu2_m128i((const __m128i*)&pred[27 * i_pred], (const __m128i*)&pred[19 * i_pred]);
	P20 = _mm256_loadu2_m128i((const __m128i*)&pred[28 * i_pred], (const __m128i*)&pred[20 * i_pred]);
	P21 = _mm256_loadu2_m128i((const __m128i*)&pred[29 * i_pred], (const __m128i*)&pred[21 * i_pred]);
	P22 = _mm256_loadu2_m128i((const __m128i*)&pred[30 * i_pred], (const __m128i*)&pred[22 * i_pred]);
	P23 = _mm256_loadu2_m128i((const __m128i*)&pred[31 * i_pred], (const __m128i*)&pred[23 * i_pred]);

	P24 = _mm256_loadu2_m128i((const __m128i*)&pred[24 * i_pred + 16], (const __m128i*)&pred[16 * i_pred + 16]);
	P25 = _mm256_loadu2_m128i((const __m128i*)&pred[25 * i_pred + 16], (const __m128i*)&pred[17 * i_pred + 16]);
	P26 = _mm256_loadu2_m128i((const __m128i*)&pred[26 * i_pred + 16], (const __m128i*)&pred[18 * i_pred + 16]);
	P27 = _mm256_loadu2_m128i((const __m128i*)&pred[27 * i_pred + 16], (const __m128i*)&pred[19 * i_pred + 16]);
	P28 = _mm256_loadu2_m128i((const __m128i*)&pred[28 * i_pred + 16], (const __m128i*)&pred[20 * i_pred + 16]);
	P29 = _mm256_loadu2_m128i((const __m128i*)&pred[29 * i_pred + 16], (const __m128i*)&pred[21 * i_pred + 16]);
	P30 = _mm256_loadu2_m128i((const __m128i*)&pred[30 * i_pred + 16], (const __m128i*)&pred[22 * i_pred + 16]);
	P31 = _mm256_loadu2_m128i((const __m128i*)&pred[31 * i_pred + 16], (const __m128i*)&pred[23 * i_pred + 16]);


	res00[0] = _mm256_unpacklo_epi8(P00, zero);
	res01[0] = _mm256_unpackhi_epi8(P00, zero);
	res02[0] = _mm256_unpacklo_epi8(P01, zero);
	res03[0] = _mm256_unpackhi_epi8(P01, zero);
	res04[0] = _mm256_unpacklo_epi8(P02, zero);
	res05[0] = _mm256_unpackhi_epi8(P02, zero);
	res06[0] = _mm256_unpacklo_epi8(P03, zero);
	res07[0] = _mm256_unpackhi_epi8(P03, zero);
	res08[0] = _mm256_unpacklo_epi8(P04, zero);
	res09[0] = _mm256_unpackhi_epi8(P04, zero);
	res10[0] = _mm256_unpacklo_epi8(P05, zero);
	res11[0] = _mm256_unpackhi_epi8(P05, zero);
	res12[0] = _mm256_unpacklo_epi8(P06, zero);
	res13[0] = _mm256_unpackhi_epi8(P06, zero);
	res14[0] = _mm256_unpacklo_epi8(P07, zero);
	res15[0] = _mm256_unpackhi_epi8(P07, zero);

	res00[1] = _mm256_unpacklo_epi8(P08, zero);
	res01[1] = _mm256_unpackhi_epi8(P08, zero);
	res02[1] = _mm256_unpacklo_epi8(P09, zero);
	res03[1] = _mm256_unpackhi_epi8(P09, zero);
	res04[1] = _mm256_unpacklo_epi8(P10, zero);
	res05[1] = _mm256_unpackhi_epi8(P10, zero);
	res06[1] = _mm256_unpacklo_epi8(P11, zero);
	res07[1] = _mm256_unpackhi_epi8(P11, zero);
	res08[1] = _mm256_unpacklo_epi8(P12, zero);
	res09[1] = _mm256_unpackhi_epi8(P12, zero);
	res10[1] = _mm256_unpacklo_epi8(P13, zero);
	res11[1] = _mm256_unpackhi_epi8(P13, zero);
	res12[1] = _mm256_unpacklo_epi8(P14, zero);
	res13[1] = _mm256_unpackhi_epi8(P14, zero);
	res14[1] = _mm256_unpacklo_epi8(P15, zero);
	res15[1] = _mm256_unpackhi_epi8(P15, zero);

	res16[0] = _mm256_unpacklo_epi8(P16, zero);
	res17[0] = _mm256_unpackhi_epi8(P16, zero);
	res18[0] = _mm256_unpacklo_epi8(P17, zero);
	res19[0] = _mm256_unpackhi_epi8(P17, zero);
	res20[0] = _mm256_unpacklo_epi8(P18, zero);
	res21[0] = _mm256_unpackhi_epi8(P18, zero);
	res22[0] = _mm256_unpacklo_epi8(P19, zero);
	res23[0] = _mm256_unpackhi_epi8(P19, zero);
	res24[0] = _mm256_unpacklo_epi8(P20, zero);
	res25[0] = _mm256_unpackhi_epi8(P20, zero);
	res26[0] = _mm256_unpacklo_epi8(P21, zero);
	res27[0] = _mm256_unpackhi_epi8(P21, zero);
	res28[0] = _mm256_unpacklo_epi8(P22, zero);
	res29[0] = _mm256_unpackhi_epi8(P22, zero);
	res30[0] = _mm256_unpacklo_epi8(P23, zero);
	res31[0] = _mm256_unpackhi_epi8(P23, zero);

	res16[1] = _mm256_unpacklo_epi8(P24, zero);
	res17[1] = _mm256_unpackhi_epi8(P24, zero);
	res18[1] = _mm256_unpacklo_epi8(P25, zero);
	res19[1] = _mm256_unpackhi_epi8(P25, zero);
	res20[1] = _mm256_unpacklo_epi8(P26, zero);
	res21[1] = _mm256_unpackhi_epi8(P26, zero);
	res22[1] = _mm256_unpacklo_epi8(P27, zero);
	res23[1] = _mm256_unpackhi_epi8(P27, zero);
	res24[1] = _mm256_unpacklo_epi8(P28, zero);
	res25[1] = _mm256_unpackhi_epi8(P28, zero);
	res26[1] = _mm256_unpacklo_epi8(P29, zero);
	res27[1] = _mm256_unpackhi_epi8(P29, zero);
	res28[1] = _mm256_unpacklo_epi8(P30, zero);
	res29[1] = _mm256_unpackhi_epi8(P30, zero);
	res30[1] = _mm256_unpacklo_epi8(P31, zero);
	res31[1] = _mm256_unpackhi_epi8(P31, zero);

	for (k = 0; k < 2; k++)
	{
		res00[k] = _mm256_add_epi16(in00[k], res00[k]);
		res01[k] = _mm256_add_epi16(in01[k], res01[k]);
		res02[k] = _mm256_add_epi16(in02[k], res02[k]);
		res03[k] = _mm256_add_epi16(in03[k], res03[k]);
		res04[k] = _mm256_add_epi16(in04[k], res04[k]);
		res05[k] = _mm256_add_epi16(in05[k], res05[k]);
		res06[k] = _mm256_add_epi16(in06[k], res06[k]);
		res07[k] = _mm256_add_epi16(in07[k], res07[k]);
		res08[k] = _mm256_add_epi16(in08[k], res08[k]);
		res09[k] = _mm256_add_epi16(in09[k], res09[k]);
		res10[k] = _mm256_add_epi16(in10[k], res10[k]);
		res11[k] = _mm256_add_epi16(in11[k], res11[k]);
		res12[k] = _mm256_add_epi16(in12[k], res12[k]);
		res13[k] = _mm256_add_epi16(in13[k], res13[k]);
		res14[k] = _mm256_add_epi16(in14[k], res14[k]);
		res15[k] = _mm256_add_epi16(in15[k], res15[k]);
		res16[k] = _mm256_add_epi16(in16[k], res16[k]);
		res17[k] = _mm256_add_epi16(in17[k], res17[k]);
		res18[k] = _mm256_add_epi16(in18[k], res18[k]);
		res19[k] = _mm256_add_epi16(in19[k], res19[k]);
		res20[k] = _mm256_add_epi16(in20[k], res20[k]);
		res21[k] = _mm256_add_epi16(in21[k], res21[k]);
		res22[k] = _mm256_add_epi16(in22[k], res22[k]);
		res23[k] = _mm256_add_epi16(in23[k], res23[k]);
		res24[k] = _mm256_add_epi16(in24[k], res24[k]);
		res25[k] = _mm256_add_epi16(in25[k], res25[k]);
		res26[k] = _mm256_add_epi16(in26[k], res26[k]);
		res27[k] = _mm256_add_epi16(in27[k], res27[k]);
		res28[k] = _mm256_add_epi16(in28[k], res28[k]);
		res29[k] = _mm256_add_epi16(in29[k], res29[k]);
		res30[k] = _mm256_add_epi16(in30[k], res30[k]);
		res31[k] = _mm256_add_epi16(in31[k], res31[k]);
	}


	P00 = _mm256_packus_epi16(res00[0], res01[0]);
	P01 = _mm256_packus_epi16(res02[0], res03[0]);
	P02 = _mm256_packus_epi16(res04[0], res05[0]);
	P03 = _mm256_packus_epi16(res06[0], res07[0]);
	P04 = _mm256_packus_epi16(res08[0], res09[0]);
	P05 = _mm256_packus_epi16(res10[0], res11[0]);
	P06 = _mm256_packus_epi16(res12[0], res13[0]);
	P07 = _mm256_packus_epi16(res14[0], res15[0]);

	P08 = _mm256_packus_epi16(res00[1], res01[1]);
	P09 = _mm256_packus_epi16(res02[1], res03[1]);
	P10 = _mm256_packus_epi16(res04[1], res05[1]);
	P11 = _mm256_packus_epi16(res06[1], res07[1]);
	P12 = _mm256_packus_epi16(res08[1], res09[1]);
	P13 = _mm256_packus_epi16(res10[1], res11[1]);
	P14 = _mm256_packus_epi16(res12[1], res13[1]);
	P15 = _mm256_packus_epi16(res14[1], res15[1]);


	P16 = _mm256_packus_epi16(res16[0], res17[0]);
	P17 = _mm256_packus_epi16(res18[0], res19[0]);
	P18 = _mm256_packus_epi16(res20[0], res21[0]);
	P19 = _mm256_packus_epi16(res22[0], res23[0]);
	P20 = _mm256_packus_epi16(res24[0], res25[0]);
	P21 = _mm256_packus_epi16(res26[0], res27[0]);
	P22 = _mm256_packus_epi16(res28[0], res29[0]);
	P23 = _mm256_packus_epi16(res30[0], res31[0]);


	P24 = _mm256_packus_epi16(res16[1], res17[1]);
	P25 = _mm256_packus_epi16(res18[1], res19[1]);
	P26 = _mm256_packus_epi16(res20[1], res21[1]);
	P27 = _mm256_packus_epi16(res22[1], res23[1]);
	P28 = _mm256_packus_epi16(res24[1], res25[1]);
	P29 = _mm256_packus_epi16(res26[1], res27[1]);
	P30 = _mm256_packus_epi16(res28[1], res29[1]);
	P31 = _mm256_packus_epi16(res30[1], res31[1]);

	//_mm256_storeu2_m128i(( __m128i*)&dst[8 * i_dst], ( __m128i*)&dst[0], P00);
	//_mm256_storeu2_m128i(( __m128i*)&dst[9 * i_dst], ( __m128i*)&dst[1 * i_dst], P01);
	//_mm256_storeu2_m128i(( __m128i*)&dst[10 * i_dst], ( __m128i*)&dst[2 * i_dst], P02);
	//_mm256_storeu2_m128i(( __m128i*)&dst[11 * i_dst], ( __m128i*)&dst[3 * i_dst], P03);
	//_mm256_storeu2_m128i(( __m128i*)&dst[12 * i_dst], ( __m128i*)&dst[4 * i_dst], P04);
	//_mm256_storeu2_m128i(( __m128i*)&dst[13 * i_dst], ( __m128i*)&dst[5 * i_dst], P05);
	//_mm256_storeu2_m128i(( __m128i*)&dst[14 * i_dst], ( __m128i*)&dst[6 * i_dst], P06);
	//_mm256_storeu2_m128i(( __m128i*)&dst[15 * i_dst], ( __m128i*)&dst[7 * i_dst], P07);

	//_mm256_storeu2_m128i(( __m128i*)&dst[8 * i_dst + 16], ( __m128i*)&dst[0 + 16], P08);
	//_mm256_storeu2_m128i(( __m128i*)&dst[9 * i_dst + 16], ( __m128i*)&dst[1 * i_dst + 16], P09);
	//_mm256_storeu2_m128i(( __m128i*)&dst[10 * i_dst + 16], ( __m128i*)&dst[2 * i_dst + 16], P10);
	//_mm256_storeu2_m128i(( __m128i*)&dst[11 * i_dst + 16], ( __m128i*)&dst[3 * i_dst + 16], P11);
	//_mm256_storeu2_m128i(( __m128i*)&dst[12 * i_dst + 16], ( __m128i*)&dst[4 * i_dst + 16], P12);
	//_mm256_storeu2_m128i(( __m128i*)&dst[13 * i_dst + 16], ( __m128i*)&dst[5 * i_dst + 16], P13);
	//_mm256_storeu2_m128i(( __m128i*)&dst[14 * i_dst + 16], ( __m128i*)&dst[6 * i_dst + 16], P14);
	//_mm256_storeu2_m128i(( __m128i*)&dst[15 * i_dst + 16], ( __m128i*)&dst[7 * i_dst + 16], P15);

	//_mm256_storeu2_m128i(( __m128i*)&dst[24 * i_dst], ( __m128i*)&dst[16 * i_dst], P16);
	//_mm256_storeu2_m128i(( __m128i*)&dst[25 * i_dst], ( __m128i*)&dst[17 * i_dst], P17);
	//_mm256_storeu2_m128i(( __m128i*)&dst[26 * i_dst], ( __m128i*)&dst[18 * i_dst], P18);
	//_mm256_storeu2_m128i(( __m128i*)&dst[27 * i_dst], ( __m128i*)&dst[19 * i_dst], P19);
	//_mm256_storeu2_m128i(( __m128i*)&dst[28 * i_dst], ( __m128i*)&dst[20 * i_dst], P20);
	//_mm256_storeu2_m128i(( __m128i*)&dst[29 * i_dst], ( __m128i*)&dst[21 * i_dst], P21);
	//_mm256_storeu2_m128i(( __m128i*)&dst[30 * i_dst], ( __m128i*)&dst[22 * i_dst], P22);
	//_mm256_storeu2_m128i(( __m128i*)&dst[31 * i_dst], ( __m128i*)&dst[23 * i_dst], P23);

	//_mm256_storeu2_m128i(( __m128i*)&dst[24 * i_dst + 16], ( __m128i*)&dst[16 * i_dst + 16], P24);
	//_mm256_storeu2_m128i(( __m128i*)&dst[25 * i_dst + 16], ( __m128i*)&dst[17 * i_dst + 16], P25);
	//_mm256_storeu2_m128i(( __m128i*)&dst[26 * i_dst + 16], ( __m128i*)&dst[18 * i_dst + 16], P26);
	//_mm256_storeu2_m128i(( __m128i*)&dst[27 * i_dst + 16], ( __m128i*)&dst[19 * i_dst + 16], P27);
	//_mm256_storeu2_m128i(( __m128i*)&dst[28 * i_dst + 16], ( __m128i*)&dst[20 * i_dst + 16], P28);
	//_mm256_storeu2_m128i(( __m128i*)&dst[29 * i_dst + 16], ( __m128i*)&dst[21 * i_dst + 16], P29);
	//_mm256_storeu2_m128i(( __m128i*)&dst[30 * i_dst + 16], ( __m128i*)&dst[22 * i_dst + 16], P30);
	//_mm256_storeu2_m128i(( __m128i*)&dst[31 * i_dst + 16], ( __m128i*)&dst[23 * i_dst + 16], P31);

	_mm_storeu_si128((__m128i*)&dst[0], _mm256_castsi256_si128(P00)); _mm_storeu_si128((__m128i*)&dst[8 * i_dst], _mm256_extractf128_si256(P00, 0x1));
	_mm_storeu_si128((__m128i*)&dst[1 * i_dst], _mm256_castsi256_si128(P01)); _mm_storeu_si128((__m128i*)&dst[9 * i_dst], _mm256_extractf128_si256(P01, 0x1));
	_mm_storeu_si128((__m128i*)&dst[2 * i_dst], _mm256_castsi256_si128(P02)); _mm_storeu_si128((__m128i*)&dst[10 * i_dst], _mm256_extractf128_si256(P02, 0x1));
	_mm_storeu_si128((__m128i*)&dst[3 * i_dst], _mm256_castsi256_si128(P03)); _mm_storeu_si128((__m128i*)&dst[11 * i_dst], _mm256_extractf128_si256(P03, 0x1));
	_mm_storeu_si128((__m128i*)&dst[4 * i_dst], _mm256_castsi256_si128(P04)); _mm_storeu_si128((__m128i*)&dst[12 * i_dst], _mm256_extractf128_si256(P04, 0x1));
	_mm_storeu_si128((__m128i*)&dst[5 * i_dst], _mm256_castsi256_si128(P05)); _mm_storeu_si128((__m128i*)&dst[13 * i_dst], _mm256_extractf128_si256(P05, 0x1));
	_mm_storeu_si128((__m128i*)&dst[6 * i_dst], _mm256_castsi256_si128(P06)); _mm_storeu_si128((__m128i*)&dst[14 * i_dst], _mm256_extractf128_si256(P06, 0x1));
	_mm_storeu_si128((__m128i*)&dst[7 * i_dst], _mm256_castsi256_si128(P07)); _mm_storeu_si128((__m128i*)&dst[15 * i_dst], _mm256_extractf128_si256(P07, 0x1));

	_mm_storeu_si128((__m128i*)&dst[0 + 16], _mm256_castsi256_si128(P08)); _mm_storeu_si128((__m128i*)&dst[8 * i_dst + 16], _mm256_extractf128_si256(P08, 0x1));
	_mm_storeu_si128((__m128i*)&dst[1 * i_dst + 16], _mm256_castsi256_si128(P09)); _mm_storeu_si128((__m128i*)&dst[9 * i_dst + 16], _mm256_extractf128_si256(P09, 0x1));
	_mm_storeu_si128((__m128i*)&dst[2 * i_dst + 16], _mm256_castsi256_si128(P10)); _mm_storeu_si128((__m128i*)&dst[10 * i_dst + 16], _mm256_extractf128_si256(P10, 0x1));
	_mm_storeu_si128((__m128i*)&dst[3 * i_dst + 16], _mm256_castsi256_si128(P11)); _mm_storeu_si128((__m128i*)&dst[11 * i_dst + 16], _mm256_extractf128_si256(P11, 0x1));
	_mm_storeu_si128((__m128i*)&dst[4 * i_dst + 16], _mm256_castsi256_si128(P12)); _mm_storeu_si128((__m128i*)&dst[12 * i_dst + 16], _mm256_extractf128_si256(P12, 0x1));
	_mm_storeu_si128((__m128i*)&dst[5 * i_dst + 16], _mm256_castsi256_si128(P13)); _mm_storeu_si128((__m128i*)&dst[13 * i_dst + 16], _mm256_extractf128_si256(P13, 0x1));
	_mm_storeu_si128((__m128i*)&dst[6 * i_dst + 16], _mm256_castsi256_si128(P14)); _mm_storeu_si128((__m128i*)&dst[14 * i_dst + 16], _mm256_extractf128_si256(P14, 0x1));
	_mm_storeu_si128((__m128i*)&dst[7 * i_dst + 16], _mm256_castsi256_si128(P15)); _mm_storeu_si128((__m128i*)&dst[15 * i_dst + 16], _mm256_extractf128_si256(P15, 0x1));


	_mm_storeu_si128((__m128i*)&dst[16 * i_dst], _mm256_castsi256_si128(P16)); _mm_storeu_si128((__m128i*)&dst[24 * i_dst], _mm256_extractf128_si256(P16, 0x1));
	_mm_storeu_si128((__m128i*)&dst[17 * i_dst], _mm256_castsi256_si128(P17)); _mm_storeu_si128((__m128i*)&dst[25 * i_dst], _mm256_extractf128_si256(P17, 0x1));
	_mm_storeu_si128((__m128i*)&dst[18 * i_dst], _mm256_castsi256_si128(P18)); _mm_storeu_si128((__m128i*)&dst[26 * i_dst], _mm256_extractf128_si256(P18, 0x1));
	_mm_storeu_si128((__m128i*)&dst[19 * i_dst], _mm256_castsi256_si128(P19)); _mm_storeu_si128((__m128i*)&dst[27 * i_dst], _mm256_extractf128_si256(P19, 0x1));
	_mm_storeu_si128((__m128i*)&dst[20 * i_dst], _mm256_castsi256_si128(P20)); _mm_storeu_si128((__m128i*)&dst[28 * i_dst], _mm256_extractf128_si256(P20, 0x1));
	_mm_storeu_si128((__m128i*)&dst[21 * i_dst], _mm256_castsi256_si128(P21)); _mm_storeu_si128((__m128i*)&dst[29 * i_dst], _mm256_extractf128_si256(P21, 0x1));
	_mm_storeu_si128((__m128i*)&dst[22 * i_dst], _mm256_castsi256_si128(P22)); _mm_storeu_si128((__m128i*)&dst[30 * i_dst], _mm256_extractf128_si256(P22, 0x1));
	_mm_storeu_si128((__m128i*)&dst[23 * i_dst], _mm256_castsi256_si128(P23)); _mm_storeu_si128((__m128i*)&dst[31 * i_dst], _mm256_extractf128_si256(P23, 0x1));


	_mm_storeu_si128((__m128i*)&dst[16 * i_dst + 16], _mm256_castsi256_si128(P24)); _mm_storeu_si128((__m128i*)&dst[24 * i_dst + 16], _mm256_extractf128_si256(P24, 0x1));
	_mm_storeu_si128((__m128i*)&dst[17 * i_dst + 16], _mm256_castsi256_si128(P25)); _mm_storeu_si128((__m128i*)&dst[25 * i_dst + 16], _mm256_extractf128_si256(P25, 0x1));
	_mm_storeu_si128((__m128i*)&dst[18 * i_dst + 16], _mm256_castsi256_si128(P26)); _mm_storeu_si128((__m128i*)&dst[26 * i_dst + 16], _mm256_extractf128_si256(P26, 0x1));
	_mm_storeu_si128((__m128i*)&dst[19 * i_dst + 16], _mm256_castsi256_si128(P27)); _mm_storeu_si128((__m128i*)&dst[27 * i_dst + 16], _mm256_extractf128_si256(P27, 0x1));
	_mm_storeu_si128((__m128i*)&dst[20 * i_dst + 16], _mm256_castsi256_si128(P28)); _mm_storeu_si128((__m128i*)&dst[28 * i_dst + 16], _mm256_extractf128_si256(P28, 0x1));
	_mm_storeu_si128((__m128i*)&dst[21 * i_dst + 16], _mm256_castsi256_si128(P29)); _mm_storeu_si128((__m128i*)&dst[29 * i_dst + 16], _mm256_extractf128_si256(P29, 0x1));
	_mm_storeu_si128((__m128i*)&dst[22 * i_dst + 16], _mm256_castsi256_si128(P30)); _mm_storeu_si128((__m128i*)&dst[30 * i_dst + 16], _mm256_extractf128_si256(P30, 0x1));
	_mm_storeu_si128((__m128i*)&dst[23 * i_dst + 16], _mm256_castsi256_si128(P31)); _mm_storeu_si128((__m128i*)&dst[31 * i_dst + 16], _mm256_extractf128_si256(P31, 0x1));



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

void add_inv_trans_64x64_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	int k, i;
	__m256i P00[2], P01[2], P02[2], P03[2], P04[2], P05[2], P06[2], P07[2], P08[2], P09[2], P10[2], P11[2], P12[2], P13[2], P14[2], P15[2], P16[2], P17[2], P18[2], P19[2],
		P20[2], P21[2], P22[2], P23[2], P24[2], P25[2], P26[2], P27[2], P28[2], P29[2], P30[2], P31[2], P32[2], P33[2], P34[2], P35[2], P36[2], P37[2], P38[2], P39[2],
		P40[2], P41[2], P42[2], P43[2], P44[2], P45[2], P46[2], P47[2], P48[2], P49[2], P50[2], P51[2], P52[2], P53[2], P54[2], P55[2], P56[2], P57[2], P58[2], P59[2], P60[2], P61[2], P62[2], P63[2];
	__m256i max_val, min_val;
	__m256i EEO0A, EEO1A, EEO2A, EEO3A, EEO0B, EEO1B, EEO2B, EEO3B;
	__m256i EEEO0A, EEEO0B, EEEO1A, EEEO1B;
	__m256i EEEE0A, EEEE0B, EEEE1A, EEEE1B;
	__m256i EEE0A, EEE0B, EEE1A, EEE1B, EEE3A, EEE3B, EEE2A, EEE2B;
	__m256i EE0A, EE0B, EE1A, EE1B, EE2A, EE2B, EE3A, EE3B, EE7A, EE7B, EE6A, EE6B, EE5A, EE5B, EE4A, EE4B;
	__m256i E0A, E0B, E1A, E1B, E2A, E2B, E3A, E3B, E4A, E4B, E5A, E5B, E6A, E6B, E7A, E7B, EFA, EFB, EEA, EEB, EDA, EDB, ECA, ECB, EBA, EBB, EAA, EAB, E9A, E9B, E8A, E8B;
	__m256i T10A, T10B, T11A, T11B, T12A, T12B, T13A, T13B, T14A, T14B, T15A, T15B, T16A, T16B, T17A, T17B, T18A, T18B, T19A, T19B, T1AA, T1AB, T1BA, T1BB, T1CA, T1CB, T1DA, T1DB, T1EA, T1EB, T1FA, T1FB;
	__m256i T2_00A, T2_00B, T2_01A, T2_01B, T2_02A, T2_02B, T2_03A, T2_03B, T2_04A, T2_04B, T2_05A, T2_05B, T2_06A, T2_06B, T2_07A, T2_07B, T2_08A, T2_08B, T2_09A, T2_09B, T2_10A, T2_10B, T2_11A, T2_11B, T2_12A, T2_12B, T2_13A, T2_13B, T2_14A, T2_14B, T2_15A, T2_15B, T2_31A, T2_31B, T2_30A, T2_30B, T2_29A, T2_29B, T2_28A, T2_28B, T2_27A, T2_27B, T2_26A, T2_26B, T2_25A, T2_25B, T2_24A, T2_24B, T2_23A, T2_23B, T2_22A, T2_22B, T2_21A, T2_21B, T2_20A, T2_20B, T2_19A, T2_19B, T2_18A, T2_18B, T2_17A, T2_17B, T2_16A, T2_16B;
	__m256i T3_00A, T3_00B, T3_01A, T3_01B, T3_02A, T3_02B, T3_03A, T3_03B, T3_04A, T3_04B, T3_05A, T3_05B, T3_06A, T3_06B, T3_07A, T3_07B, T3_08A, T3_08B, T3_09A, T3_09B, T3_10A, T3_10B, T3_11A, T3_11B, T3_12A, T3_12B, T3_13A, T3_13B, T3_14A, T3_14B, T3_15A, T3_15B;
	__m256i T3_16A, T3_16B, T3_17A, T3_17B, T3_18A, T3_18B, T3_19A, T3_19B, T3_20A, T3_20B, T3_21A, T3_21B, T3_22A, T3_22B, T3_23A, T3_23B, T3_24A, T3_24B, T3_25A, T3_25B, T3_26A, T3_26B, T3_27A, T3_27B, T3_28A, T3_28B, T3_29A, T3_29B, T3_30A, T3_30B, T3_31A, T3_31B;
	const __m256i c16_p45_p45 = _mm256_set1_epi32(0x002D002D);
	const __m256i c16_p43_p44 = _mm256_set1_epi32(0x002B002C);
	const __m256i c16_p39_p41 = _mm256_set1_epi32(0x00270029);
	const __m256i c16_p34_p36 = _mm256_set1_epi32(0x00220024);
	const __m256i c16_p27_p30 = _mm256_set1_epi32(0x001B001E);
	const __m256i c16_p19_p23 = _mm256_set1_epi32(0x00130017);
	const __m256i c16_p11_p15 = _mm256_set1_epi32(0x000B000F);
	const __m256i c16_p02_p07 = _mm256_set1_epi32(0x00020007);
	const __m256i c16_p41_p45 = _mm256_set1_epi32(0x0029002D);
	const __m256i c16_p23_p34 = _mm256_set1_epi32(0x00170022);
	const __m256i c16_n02_p11 = _mm256_set1_epi32(0xFFFE000B);
	const __m256i c16_n27_n15 = _mm256_set1_epi32(0xFFE5FFF1);
	const __m256i c16_n43_n36 = _mm256_set1_epi32(0xFFD5FFDC);
	const __m256i c16_n44_n45 = _mm256_set1_epi32(0xFFD4FFD3);
	const __m256i c16_n30_n39 = _mm256_set1_epi32(0xFFE2FFD9);
	const __m256i c16_n07_n19 = _mm256_set1_epi32(0xFFF9FFED);
	const __m256i c16_p34_p44 = _mm256_set1_epi32(0x0022002C);
	const __m256i c16_n07_p15 = _mm256_set1_epi32(0xFFF9000F);
	const __m256i c16_n41_n27 = _mm256_set1_epi32(0xFFD7FFE5);
	const __m256i c16_n39_n45 = _mm256_set1_epi32(0xFFD9FFD3);
	const __m256i c16_n02_n23 = _mm256_set1_epi32(0xFFFEFFE9);
	const __m256i c16_p36_p19 = _mm256_set1_epi32(0x00240013);
	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	const __m256i c16_p11_p30 = _mm256_set1_epi32(0x000B001E);
	const __m256i c16_p23_p43 = _mm256_set1_epi32(0x0017002B);
	const __m256i c16_n34_n07 = _mm256_set1_epi32(0xFFDEFFF9);
	const __m256i c16_n36_n45 = _mm256_set1_epi32(0xFFDCFFD3);
	const __m256i c16_p19_n11 = _mm256_set1_epi32(0x0013FFF5);
	const __m256i c16_p44_p41 = _mm256_set1_epi32(0x002C0029);
	const __m256i c16_n02_p27 = _mm256_set1_epi32(0xFFFE001B);
	const __m256i c16_n45_n30 = _mm256_set1_epi32(0xFFD3FFE2);
	const __m256i c16_n15_n39 = _mm256_set1_epi32(0xFFF1FFD9);
	const __m256i c16_p11_p41 = _mm256_set1_epi32(0x000B0029);
	const __m256i c16_n45_n27 = _mm256_set1_epi32(0xFFD3FFE5);
	const __m256i c16_p07_n30 = _mm256_set1_epi32(0x0007FFE2);
	const __m256i c16_p43_p39 = _mm256_set1_epi32(0x002B0027);
	const __m256i c16_n23_p15 = _mm256_set1_epi32(0xFFE9000F);
	const __m256i c16_n34_n45 = _mm256_set1_epi32(0xFFDEFFD3);
	const __m256i c16_p36_p02 = _mm256_set1_epi32(0x00240002);
	const __m256i c16_p19_p44 = _mm256_set1_epi32(0x0013002C);
	const __m256i c16_n02_p39 = _mm256_set1_epi32(0xFFFE0027);
	const __m256i c16_n36_n41 = _mm256_set1_epi32(0xFFDCFFD7);
	const __m256i c16_p43_p07 = _mm256_set1_epi32(0x002B0007);
	const __m256i c16_n11_p34 = _mm256_set1_epi32(0xFFF50022);
	const __m256i c16_n30_n44 = _mm256_set1_epi32(0xFFE2FFD4);
	const __m256i c16_p45_p15 = _mm256_set1_epi32(0x002D000F);
	const __m256i c16_n19_p27 = _mm256_set1_epi32(0xFFED001B);
	const __m256i c16_n23_n45 = _mm256_set1_epi32(0xFFE9FFD3);
	const __m256i c16_n15_p36 = _mm256_set1_epi32(0xFFF10024);
	const __m256i c16_n11_n45 = _mm256_set1_epi32(0xFFF5FFD3);
	const __m256i c16_p34_p39 = _mm256_set1_epi32(0x00220027);
	const __m256i c16_n45_n19 = _mm256_set1_epi32(0xFFD3FFED);
	const __m256i c16_p41_n07 = _mm256_set1_epi32(0x0029FFF9);
	const __m256i c16_n23_p30 = _mm256_set1_epi32(0xFFE9001E);
	const __m256i c16_n02_n44 = _mm256_set1_epi32(0xFFFEFFD4);
	const __m256i c16_p27_p43 = _mm256_set1_epi32(0x001B002B);
	const __m256i c16_n27_p34 = _mm256_set1_epi32(0xFFE50022);
	const __m256i c16_p19_n39 = _mm256_set1_epi32(0x0013FFD9);
	const __m256i c16_n11_p43 = _mm256_set1_epi32(0xFFF5002B);
	const __m256i c16_p02_n45 = _mm256_set1_epi32(0x0002FFD3);
	const __m256i c16_p07_p45 = _mm256_set1_epi32(0x0007002D);
	const __m256i c16_n15_n44 = _mm256_set1_epi32(0xFFF1FFD4);
	const __m256i c16_p23_p41 = _mm256_set1_epi32(0x00170029);
	const __m256i c16_n30_n36 = _mm256_set1_epi32(0xFFE2FFDC);
	const __m256i c16_n36_p30 = _mm256_set1_epi32(0xFFDC001E);
	const __m256i c16_p41_n23 = _mm256_set1_epi32(0x0029FFE9);
	const __m256i c16_n44_p15 = _mm256_set1_epi32(0xFFD4000F);
	const __m256i c16_p45_n07 = _mm256_set1_epi32(0x002DFFF9);
	const __m256i c16_n45_n02 = _mm256_set1_epi32(0xFFD3FFFE);
	const __m256i c16_p43_p11 = _mm256_set1_epi32(0x002B000B);
	const __m256i c16_n39_n19 = _mm256_set1_epi32(0xFFD9FFED);
	const __m256i c16_p34_p27 = _mm256_set1_epi32(0x0022001B);
	const __m256i c16_n43_p27 = _mm256_set1_epi32(0xFFD5001B);
	const __m256i c16_p44_n02 = _mm256_set1_epi32(0x002CFFFE);
	const __m256i c16_n30_n23 = _mm256_set1_epi32(0xFFE2FFE9);
	const __m256i c16_p07_p41 = _mm256_set1_epi32(0x00070029);
	const __m256i c16_p19_n45 = _mm256_set1_epi32(0x0013FFD3);
	const __m256i c16_n39_p34 = _mm256_set1_epi32(0xFFD90022);
	const __m256i c16_p45_n11 = _mm256_set1_epi32(0x002DFFF5);
	const __m256i c16_n36_n15 = _mm256_set1_epi32(0xFFDCFFF1);
	const __m256i c16_n45_p23 = _mm256_set1_epi32(0xFFD30017);
	const __m256i c16_p27_p19 = _mm256_set1_epi32(0x001B0013);
	const __m256i c16_p15_n45 = _mm256_set1_epi32(0x000FFFD3);
	const __m256i c16_n44_p30 = _mm256_set1_epi32(0xFFD4001E);
	const __m256i c16_p34_p11 = _mm256_set1_epi32(0x0022000B);
	const __m256i c16_p07_n43 = _mm256_set1_epi32(0x0007FFD5);
	const __m256i c16_n41_p36 = _mm256_set1_epi32(0xFFD70024);
	const __m256i c16_p39_p02 = _mm256_set1_epi32(0x00270002);
	const __m256i c16_n44_p19 = _mm256_set1_epi32(0xFFD40013);
	const __m256i c16_n02_p36 = _mm256_set1_epi32(0xFFFE0024);
	const __m256i c16_p45_n34 = _mm256_set1_epi32(0x002DFFDE);
	const __m256i c16_n15_n23 = _mm256_set1_epi32(0xFFF1FFE9);
	const __m256i c16_n39_p43 = _mm256_set1_epi32(0xFFD9002B);
	const __m256i c16_p30_p07 = _mm256_set1_epi32(0x001E0007);
	const __m256i c16_p27_n45 = _mm256_set1_epi32(0x001BFFD3);
	const __m256i c16_n41_p11 = _mm256_set1_epi32(0xFFD7000B);
	const __m256i c16_n39_p15 = _mm256_set1_epi32(0xFFD9000F);
	const __m256i c16_n30_p45 = _mm256_set1_epi32(0xFFE2002D);
	const __m256i c16_p27_p02 = _mm256_set1_epi32(0x001B0002);
	const __m256i c16_p41_n44 = _mm256_set1_epi32(0x0029FFD4);
	const __m256i c16_n11_n19 = _mm256_set1_epi32(0xFFF5FFED);
	const __m256i c16_n45_p36 = _mm256_set1_epi32(0xFFD30024);
	const __m256i c16_n07_p34 = _mm256_set1_epi32(0xFFF90022);
	const __m256i c16_p43_n23 = _mm256_set1_epi32(0x002BFFE9);
	const __m256i c16_n30_p11 = _mm256_set1_epi32(0xFFE2000B);
	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);
	const __m256i c16_n19_p36 = _mm256_set1_epi32(0xFFED0024);
	const __m256i c16_p23_n02 = _mm256_set1_epi32(0x0017FFFE);
	const __m256i c16_p45_n39 = _mm256_set1_epi32(0x002DFFD9);
	const __m256i c16_p27_n41 = _mm256_set1_epi32(0x001BFFD7);
	const __m256i c16_n15_n07 = _mm256_set1_epi32(0xFFF1FFF9);
	const __m256i c16_n44_p34 = _mm256_set1_epi32(0xFFD40022);
	const __m256i c16_n19_p07 = _mm256_set1_epi32(0xFFED0007);
	const __m256i c16_n39_p30 = _mm256_set1_epi32(0xFFD9001E);
	const __m256i c16_n45_p44 = _mm256_set1_epi32(0xFFD3002C);
	const __m256i c16_n36_p43 = _mm256_set1_epi32(0xFFDC002B);
	const __m256i c16_n15_p27 = _mm256_set1_epi32(0xFFF1001B);
	const __m256i c16_p11_p02 = _mm256_set1_epi32(0x000B0002);
	const __m256i c16_p34_n23 = _mm256_set1_epi32(0x0022FFE9);
	const __m256i c16_p45_n41 = _mm256_set1_epi32(0x002DFFD7);
	const __m256i c16_n07_p02 = _mm256_set1_epi32(0xFFF90002);
	const __m256i c16_n15_p11 = _mm256_set1_epi32(0xFFF1000B);
	const __m256i c16_n23_p19 = _mm256_set1_epi32(0xFFE90013);
	const __m256i c16_n30_p27 = _mm256_set1_epi32(0xFFE2001B);
	const __m256i c16_n36_p34 = _mm256_set1_epi32(0xFFDC0022);
	const __m256i c16_n41_p39 = _mm256_set1_epi32(0xFFD70027);
	const __m256i c16_n44_p43 = _mm256_set1_epi32(0xFFD4002B);
	const __m256i c16_n45_p45 = _mm256_set1_epi32(0xFFD3002D);

	//	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	const __m256i c16_p35_p40 = _mm256_set1_epi32(0x00230028);
	const __m256i c16_p21_p29 = _mm256_set1_epi32(0x0015001D);
	const __m256i c16_p04_p13 = _mm256_set1_epi32(0x0004000D);
	const __m256i c16_p29_p43 = _mm256_set1_epi32(0x001D002B);
	const __m256i c16_n21_p04 = _mm256_set1_epi32(0xFFEB0004);
	const __m256i c16_n45_n40 = _mm256_set1_epi32(0xFFD3FFD8);
	const __m256i c16_n13_n35 = _mm256_set1_epi32(0xFFF3FFDD);
	const __m256i c16_p04_p40 = _mm256_set1_epi32(0x00040028);
	const __m256i c16_n43_n35 = _mm256_set1_epi32(0xFFD5FFDD);
	const __m256i c16_p29_n13 = _mm256_set1_epi32(0x001DFFF3);
	const __m256i c16_p21_p45 = _mm256_set1_epi32(0x0015002D);
	const __m256i c16_n21_p35 = _mm256_set1_epi32(0xFFEB0023);
	const __m256i c16_p04_n43 = _mm256_set1_epi32(0x0004FFD5);
	const __m256i c16_p13_p45 = _mm256_set1_epi32(0x000D002D);
	const __m256i c16_n29_n40 = _mm256_set1_epi32(0xFFE3FFD8);
	const __m256i c16_n40_p29 = _mm256_set1_epi32(0xFFD8001D);
	const __m256i c16_p45_n13 = _mm256_set1_epi32(0x002DFFF3);
	const __m256i c16_n43_n04 = _mm256_set1_epi32(0xFFD5FFFC);
	const __m256i c16_p35_p21 = _mm256_set1_epi32(0x00230015);
	const __m256i c16_n45_p21 = _mm256_set1_epi32(0xFFD30015);
	const __m256i c16_p13_p29 = _mm256_set1_epi32(0x000D001D);
	const __m256i c16_p35_n43 = _mm256_set1_epi32(0x0023FFD5);
	const __m256i c16_n40_p04 = _mm256_set1_epi32(0xFFD80004);
	const __m256i c16_n35_p13 = _mm256_set1_epi32(0xFFDD000D);
	const __m256i c16_n40_p45 = _mm256_set1_epi32(0xFFD8002D);
	const __m256i c16_p04_p21 = _mm256_set1_epi32(0x00040015);
	const __m256i c16_p43_n29 = _mm256_set1_epi32(0x002BFFE3);
	const __m256i c16_n13_p04 = _mm256_set1_epi32(0xFFF30004);
	const __m256i c16_n29_p21 = _mm256_set1_epi32(0xFFE30015);
	const __m256i c16_n40_p35 = _mm256_set1_epi32(0xFFD80023);
	//	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);

	const __m256i c16_p38_p44 = _mm256_set1_epi32(0x0026002C);
	const __m256i c16_p09_p25 = _mm256_set1_epi32(0x00090019);
	const __m256i c16_n09_p38 = _mm256_set1_epi32(0xFFF70026);
	const __m256i c16_n25_n44 = _mm256_set1_epi32(0xFFE7FFD4);

	const __m256i c16_n44_p25 = _mm256_set1_epi32(0xFFD40019);
	const __m256i c16_p38_p09 = _mm256_set1_epi32(0x00260009);
	const __m256i c16_n25_p09 = _mm256_set1_epi32(0xFFE70009);
	const __m256i c16_n44_p38 = _mm256_set1_epi32(0xFFD40026);

	const __m256i c16_p17_p42 = _mm256_set1_epi32(0x0011002A);
	const __m256i c16_n42_p17 = _mm256_set1_epi32(0xFFD60011);

	const __m256i c16_p32_p32 = _mm256_set1_epi32(0x00200020);
	const __m256i c16_n32_p32 = _mm256_set1_epi32(0xFFE00020);

	__m256i zero = _mm256_setzero_si256();
	__m256i c32_rnd = _mm256_set1_epi32(16);
	int nShift = 5;

	// DCT1
	__m256i in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2];
	__m256i in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2];
	__m256i res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2];
	__m256i res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2];

	int pass, part;
	for (i = 0; i < 2; i++)
	{
		const int offset = (i << 4);
		in00[i] = _mm256_lddqu_si256((const __m256i*)&src[0 * 32 + offset]);
		in01[i] = _mm256_lddqu_si256((const __m256i*)&src[1 * 32 + offset]);
		in02[i] = _mm256_lddqu_si256((const __m256i*)&src[2 * 32 + offset]);
		in03[i] = _mm256_lddqu_si256((const __m256i*)&src[3 * 32 + offset]);
		in04[i] = _mm256_lddqu_si256((const __m256i*)&src[4 * 32 + offset]);
		in05[i] = _mm256_lddqu_si256((const __m256i*)&src[5 * 32 + offset]);
		in06[i] = _mm256_lddqu_si256((const __m256i*)&src[6 * 32 + offset]);
		in07[i] = _mm256_lddqu_si256((const __m256i*)&src[7 * 32 + offset]);
		in08[i] = _mm256_lddqu_si256((const __m256i*)&src[8 * 32 + offset]);
		in09[i] = _mm256_lddqu_si256((const __m256i*)&src[9 * 32 + offset]);
		in10[i] = _mm256_lddqu_si256((const __m256i*)&src[10 * 32 + offset]);
		in11[i] = _mm256_lddqu_si256((const __m256i*)&src[11 * 32 + offset]);
		in12[i] = _mm256_lddqu_si256((const __m256i*)&src[12 * 32 + offset]);
		in13[i] = _mm256_lddqu_si256((const __m256i*)&src[13 * 32 + offset]);
		in14[i] = _mm256_lddqu_si256((const __m256i*)&src[14 * 32 + offset]);
		in15[i] = _mm256_lddqu_si256((const __m256i*)&src[15 * 32 + offset]);
		in16[i] = _mm256_lddqu_si256((const __m256i*)&src[16 * 32 + offset]);
		in17[i] = _mm256_lddqu_si256((const __m256i*)&src[17 * 32 + offset]);
		in18[i] = _mm256_lddqu_si256((const __m256i*)&src[18 * 32 + offset]);
		in19[i] = _mm256_lddqu_si256((const __m256i*)&src[19 * 32 + offset]);
		in20[i] = _mm256_lddqu_si256((const __m256i*)&src[20 * 32 + offset]);
		in21[i] = _mm256_lddqu_si256((const __m256i*)&src[21 * 32 + offset]);
		in22[i] = _mm256_lddqu_si256((const __m256i*)&src[22 * 32 + offset]);
		in23[i] = _mm256_lddqu_si256((const __m256i*)&src[23 * 32 + offset]);
		in24[i] = _mm256_lddqu_si256((const __m256i*)&src[24 * 32 + offset]);
		in25[i] = _mm256_lddqu_si256((const __m256i*)&src[25 * 32 + offset]);
		in26[i] = _mm256_lddqu_si256((const __m256i*)&src[26 * 32 + offset]);
		in27[i] = _mm256_lddqu_si256((const __m256i*)&src[27 * 32 + offset]);
		in28[i] = _mm256_lddqu_si256((const __m256i*)&src[28 * 32 + offset]);
		in29[i] = _mm256_lddqu_si256((const __m256i*)&src[29 * 32 + offset]);
		in30[i] = _mm256_lddqu_si256((const __m256i*)&src[30 * 32 + offset]);
		in31[i] = _mm256_lddqu_si256((const __m256i*)&src[31 * 32 + offset]);
	}

	for (pass = 0; pass < 2; pass++)
	{
		if (pass == 1)
		{
			c32_rnd = _mm256_set1_epi32(1024);				// pass == 1 第二次四舍五入
			nShift = 11;
		}

		for (part = 0; part < 2; part++)
		{
			const __m256i T_00_00A = _mm256_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
			const __m256i T_00_00B = _mm256_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
			const __m256i T_00_01A = _mm256_unpacklo_epi16(in05[part], in07[part]);       // [ ]
			const __m256i T_00_01B = _mm256_unpackhi_epi16(in05[part], in07[part]);       // [ ]
			const __m256i T_00_02A = _mm256_unpacklo_epi16(in09[part], in11[part]);       // [ ]
			const __m256i T_00_02B = _mm256_unpackhi_epi16(in09[part], in11[part]);       // [ ]
			const __m256i T_00_03A = _mm256_unpacklo_epi16(in13[part], in15[part]);       // [ ]
			const __m256i T_00_03B = _mm256_unpackhi_epi16(in13[part], in15[part]);       // [ ]
			const __m256i T_00_04A = _mm256_unpacklo_epi16(in17[part], in19[part]);       // [ ]
			const __m256i T_00_04B = _mm256_unpackhi_epi16(in17[part], in19[part]);       // [ ]
			const __m256i T_00_05A = _mm256_unpacklo_epi16(in21[part], in23[part]);       // [ ]
			const __m256i T_00_05B = _mm256_unpackhi_epi16(in21[part], in23[part]);       // [ ]
			const __m256i T_00_06A = _mm256_unpacklo_epi16(in25[part], in27[part]);       // [ ]
			const __m256i T_00_06B = _mm256_unpackhi_epi16(in25[part], in27[part]);       // [ ]
			const __m256i T_00_07A = _mm256_unpacklo_epi16(in29[part], in31[part]);       //
			const __m256i T_00_07B = _mm256_unpackhi_epi16(in29[part], in31[part]);       // [ ]

			const __m256i T_00_08A = _mm256_unpacklo_epi16(in02[part], in06[part]);       // [ ]
			const __m256i T_00_08B = _mm256_unpackhi_epi16(in02[part], in06[part]);       // [ ]
			const __m256i T_00_09A = _mm256_unpacklo_epi16(in10[part], in14[part]);       // [ ]
			const __m256i T_00_09B = _mm256_unpackhi_epi16(in10[part], in14[part]);       // [ ]
			const __m256i T_00_10A = _mm256_unpacklo_epi16(in18[part], in22[part]);       // [ ]
			const __m256i T_00_10B = _mm256_unpackhi_epi16(in18[part], in22[part]);       // [ ]
			const __m256i T_00_11A = _mm256_unpacklo_epi16(in26[part], in30[part]);       // [ ]
			const __m256i T_00_11B = _mm256_unpackhi_epi16(in26[part], in30[part]);       // [ ]

			const __m256i T_00_12A = _mm256_unpacklo_epi16(in04[part], in12[part]);       // [ ]
			const __m256i T_00_12B = _mm256_unpackhi_epi16(in04[part], in12[part]);       // [ ]
			const __m256i T_00_13A = _mm256_unpacklo_epi16(in20[part], in28[part]);       // [ ]
			const __m256i T_00_13B = _mm256_unpackhi_epi16(in20[part], in28[part]);       // [ ]

			const __m256i T_00_14A = _mm256_unpacklo_epi16(in08[part], in24[part]);       //
			const __m256i T_00_14B = _mm256_unpackhi_epi16(in08[part], in24[part]);       // [ ]
			const __m256i T_00_15A = _mm256_unpacklo_epi16(in00[part], in16[part]);       //
			const __m256i T_00_15B = _mm256_unpackhi_epi16(in00[part], in16[part]);       // [ ]

			__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
			__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
			__m256i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
			__m256i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
			{
				__m256i T00, T01, T02, T03;
#define COMPUTE_ROW(r0103, r0507, r0911, r1315, r1719, r2123, r2527, r2931, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(r0103, c0103), _mm256_madd_epi16(r0507, c0507)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(r0911, c0911), _mm256_madd_epi16(r1315, c1315)); \
	T02 = _mm256_add_epi32(_mm256_madd_epi16(r1719, c1719), _mm256_madd_epi16(r2123, c2123)); \
	T03 = _mm256_add_epi32(_mm256_madd_epi16(r2527, c2527), _mm256_madd_epi16(r2931, c2931)); \
	row = _mm256_add_epi32(_mm256_add_epi32(T00, T01), _mm256_add_epi32(T02, T03));

				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A);
				COMPUTE_ROW(T_00_00A, T_00_01A, T_00_02A, T_00_03A, T_00_04A, T_00_05A, T_00_06A, T_00_07A, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A);

				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B);
				COMPUTE_ROW(T_00_00B, T_00_01B, T_00_02B, T_00_03B, T_00_04B, T_00_05B, T_00_06B, T_00_07B, \
					c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B);

#undef COMPUTE_ROW
			}


			{
				__m256i T00, T01;
#define COMPUTE_ROW(row0206, row1014, row1822, row2630, c0206, c1014, c1822, c2630, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(row0206, c0206), _mm256_madd_epi16(row1014, c1014)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(row1822, c1822), _mm256_madd_epi16(row2630, c2630)); \
	row = _mm256_add_epi32(T00, T01);

				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A);
				COMPUTE_ROW(T_00_08A, T_00_09A, T_00_10A, T_00_11A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A);

				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B);
				COMPUTE_ROW(T_00_08B, T_00_09B, T_00_10B, T_00_11B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B);
#undef COMPUTE_ROW
			}

			EEO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_p38_p44), _mm256_madd_epi16(T_00_13A, c16_p09_p25));
			EEO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n09_p38), _mm256_madd_epi16(T_00_13A, c16_n25_n44));
			EEO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n44_p25), _mm256_madd_epi16(T_00_13A, c16_p38_p09));
			EEO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n25_p09), _mm256_madd_epi16(T_00_13A, c16_n44_p38));
			EEO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_p38_p44), _mm256_madd_epi16(T_00_13B, c16_p09_p25));
			EEO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n09_p38), _mm256_madd_epi16(T_00_13B, c16_n25_n44));
			EEO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n44_p25), _mm256_madd_epi16(T_00_13B, c16_p38_p09));
			EEO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n25_p09), _mm256_madd_epi16(T_00_13B, c16_n44_p38));

			EEEO0A = _mm256_madd_epi16(T_00_14A, c16_p17_p42);
			EEEO0B = _mm256_madd_epi16(T_00_14B, c16_p17_p42);
			EEEO1A = _mm256_madd_epi16(T_00_14A, c16_n42_p17);
			EEEO1B = _mm256_madd_epi16(T_00_14B, c16_n42_p17);

			EEEE0A = _mm256_madd_epi16(T_00_15A, c16_p32_p32);
			EEEE0B = _mm256_madd_epi16(T_00_15B, c16_p32_p32);
			EEEE1A = _mm256_madd_epi16(T_00_15A, c16_n32_p32);
			EEEE1B = _mm256_madd_epi16(T_00_15B, c16_n32_p32);

			EEE0A = _mm256_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
			EEE0B = _mm256_add_epi32(EEEE0B, EEEO0B);
			EEE1A = _mm256_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
			EEE1B = _mm256_add_epi32(EEEE1B, EEEO1B);
			EEE3A = _mm256_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
			EEE3B = _mm256_sub_epi32(EEEE0B, EEEO0B);
			EEE2A = _mm256_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
			EEE2B = _mm256_sub_epi32(EEEE1B, EEEO1B);

			EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
			EE0B = _mm256_add_epi32(EEE0B, EEO0B);
			EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
			EE1B = _mm256_add_epi32(EEE1B, EEO1B);
			EE2A = _mm256_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
			EE2B = _mm256_add_epi32(EEE2B, EEO2B);
			EE3A = _mm256_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
			EE3B = _mm256_add_epi32(EEE3B, EEO3B);
			EE7A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
			EE7B = _mm256_sub_epi32(EEE0B, EEO0B);
			EE6A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
			EE6B = _mm256_sub_epi32(EEE1B, EEO1B);
			EE5A = _mm256_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
			EE5B = _mm256_sub_epi32(EEE2B, EEO2B);
			EE4A = _mm256_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
			EE4B = _mm256_sub_epi32(EEE3B, EEO3B);

			E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
			E0B = _mm256_add_epi32(EE0B, EO0B);
			E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
			E1B = _mm256_add_epi32(EE1B, EO1B);
			E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
			E2B = _mm256_add_epi32(EE2B, EO2B);
			E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
			E3B = _mm256_add_epi32(EE3B, EO3B);
			E4A = _mm256_add_epi32(EE4A, EO4A);          // E4 =
			E4B = _mm256_add_epi32(EE4B, EO4B);
			E5A = _mm256_add_epi32(EE5A, EO5A);          // E5 =
			E5B = _mm256_add_epi32(EE5B, EO5B);
			E6A = _mm256_add_epi32(EE6A, EO6A);          // E6 =
			E6B = _mm256_add_epi32(EE6B, EO6B);
			E7A = _mm256_add_epi32(EE7A, EO7A);          // E7 =
			E7B = _mm256_add_epi32(EE7B, EO7B);
			EFA = _mm256_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
			EFB = _mm256_sub_epi32(EE0B, EO0B);
			EEA = _mm256_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
			EEB = _mm256_sub_epi32(EE1B, EO1B);
			EDA = _mm256_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
			EDB = _mm256_sub_epi32(EE2B, EO2B);
			ECA = _mm256_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
			ECB = _mm256_sub_epi32(EE3B, EO3B);
			EBA = _mm256_sub_epi32(EE4A, EO4A);          // EB =
			EBB = _mm256_sub_epi32(EE4B, EO4B);
			EAA = _mm256_sub_epi32(EE5A, EO5A);          // EA =
			EAB = _mm256_sub_epi32(EE5B, EO5B);
			E9A = _mm256_sub_epi32(EE6A, EO6A);          // E9 =
			E9B = _mm256_sub_epi32(EE6B, EO6B);
			E8A = _mm256_sub_epi32(EE7A, EO7A);          // E8 =
			E8B = _mm256_sub_epi32(EE7B, EO7B);

			T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
			T10B = _mm256_add_epi32(E0B, c32_rnd);
			T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
			T11B = _mm256_add_epi32(E1B, c32_rnd);
			T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
			T12B = _mm256_add_epi32(E2B, c32_rnd);
			T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
			T13B = _mm256_add_epi32(E3B, c32_rnd);
			T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
			T14B = _mm256_add_epi32(E4B, c32_rnd);
			T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
			T15B = _mm256_add_epi32(E5B, c32_rnd);
			T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
			T16B = _mm256_add_epi32(E6B, c32_rnd);
			T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
			T17B = _mm256_add_epi32(E7B, c32_rnd);
			T18A = _mm256_add_epi32(E8A, c32_rnd);         // E8 + rnd
			T18B = _mm256_add_epi32(E8B, c32_rnd);
			T19A = _mm256_add_epi32(E9A, c32_rnd);         // E9 + rnd
			T19B = _mm256_add_epi32(E9B, c32_rnd);
			T1AA = _mm256_add_epi32(EAA, c32_rnd);         // E10 + rnd
			T1AB = _mm256_add_epi32(EAB, c32_rnd);
			T1BA = _mm256_add_epi32(EBA, c32_rnd);         // E11 + rnd
			T1BB = _mm256_add_epi32(EBB, c32_rnd);
			T1CA = _mm256_add_epi32(ECA, c32_rnd);         // E12 + rnd
			T1CB = _mm256_add_epi32(ECB, c32_rnd);
			T1DA = _mm256_add_epi32(EDA, c32_rnd);         // E13 + rnd
			T1DB = _mm256_add_epi32(EDB, c32_rnd);
			T1EA = _mm256_add_epi32(EEA, c32_rnd);         // E14 + rnd
			T1EB = _mm256_add_epi32(EEB, c32_rnd);
			T1FA = _mm256_add_epi32(EFA, c32_rnd);         // E15 + rnd
			T1FB = _mm256_add_epi32(EFB, c32_rnd);

			T2_00A = _mm256_add_epi32(T10A, O00A);          // E0 + O0 + rnd
			T2_00B = _mm256_add_epi32(T10B, O00B);
			T2_01A = _mm256_add_epi32(T11A, O01A);          // E1 + O1 + rnd
			T2_01B = _mm256_add_epi32(T11B, O01B);
			T2_02A = _mm256_add_epi32(T12A, O02A);          // E2 + O2 + rnd
			T2_02B = _mm256_add_epi32(T12B, O02B);
			T2_03A = _mm256_add_epi32(T13A, O03A);          // E3 + O3 + rnd
			T2_03B = _mm256_add_epi32(T13B, O03B);
			T2_04A = _mm256_add_epi32(T14A, O04A);          // E4
			T2_04B = _mm256_add_epi32(T14B, O04B);
			T2_05A = _mm256_add_epi32(T15A, O05A);          // E5
			T2_05B = _mm256_add_epi32(T15B, O05B);
			T2_06A = _mm256_add_epi32(T16A, O06A);          // E6
			T2_06B = _mm256_add_epi32(T16B, O06B);
			T2_07A = _mm256_add_epi32(T17A, O07A);          // E7
			T2_07B = _mm256_add_epi32(T17B, O07B);
			T2_08A = _mm256_add_epi32(T18A, O08A);          // E8
			T2_08B = _mm256_add_epi32(T18B, O08B);
			T2_09A = _mm256_add_epi32(T19A, O09A);          // E9
			T2_09B = _mm256_add_epi32(T19B, O09B);
			T2_10A = _mm256_add_epi32(T1AA, O10A);          // E10
			T2_10B = _mm256_add_epi32(T1AB, O10B);
			T2_11A = _mm256_add_epi32(T1BA, O11A);          // E11
			T2_11B = _mm256_add_epi32(T1BB, O11B);
			T2_12A = _mm256_add_epi32(T1CA, O12A);          // E12
			T2_12B = _mm256_add_epi32(T1CB, O12B);
			T2_13A = _mm256_add_epi32(T1DA, O13A);          // E13
			T2_13B = _mm256_add_epi32(T1DB, O13B);
			T2_14A = _mm256_add_epi32(T1EA, O14A);          // E14
			T2_14B = _mm256_add_epi32(T1EB, O14B);
			T2_15A = _mm256_add_epi32(T1FA, O15A);          // E15
			T2_15B = _mm256_add_epi32(T1FB, O15B);
			T2_31A = _mm256_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
			T2_31B = _mm256_sub_epi32(T10B, O00B);
			T2_30A = _mm256_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
			T2_30B = _mm256_sub_epi32(T11B, O01B);
			T2_29A = _mm256_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
			T2_29B = _mm256_sub_epi32(T12B, O02B);
			T2_28A = _mm256_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
			T2_28B = _mm256_sub_epi32(T13B, O03B);
			T2_27A = _mm256_sub_epi32(T14A, O04A);          // E4
			T2_27B = _mm256_sub_epi32(T14B, O04B);
			T2_26A = _mm256_sub_epi32(T15A, O05A);          // E5
			T2_26B = _mm256_sub_epi32(T15B, O05B);
			T2_25A = _mm256_sub_epi32(T16A, O06A);          // E6
			T2_25B = _mm256_sub_epi32(T16B, O06B);
			T2_24A = _mm256_sub_epi32(T17A, O07A);          // E7
			T2_24B = _mm256_sub_epi32(T17B, O07B);
			T2_23A = _mm256_sub_epi32(T18A, O08A);          //
			T2_23B = _mm256_sub_epi32(T18B, O08B);
			T2_22A = _mm256_sub_epi32(T19A, O09A);          //
			T2_22B = _mm256_sub_epi32(T19B, O09B);
			T2_21A = _mm256_sub_epi32(T1AA, O10A);          //
			T2_21B = _mm256_sub_epi32(T1AB, O10B);
			T2_20A = _mm256_sub_epi32(T1BA, O11A);          //
			T2_20B = _mm256_sub_epi32(T1BB, O11B);
			T2_19A = _mm256_sub_epi32(T1CA, O12A);          //
			T2_19B = _mm256_sub_epi32(T1CB, O12B);
			T2_18A = _mm256_sub_epi32(T1DA, O13A);          //
			T2_18B = _mm256_sub_epi32(T1DB, O13B);
			T2_17A = _mm256_sub_epi32(T1EA, O14A);          //
			T2_17B = _mm256_sub_epi32(T1EB, O14B);
			T2_16A = _mm256_sub_epi32(T1FA, O15A);          //
			T2_16B = _mm256_sub_epi32(T1FB, O15B);

			T3_00A = _mm256_srai_epi32(T2_00A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			T3_00B = _mm256_srai_epi32(T2_00B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			T3_01A = _mm256_srai_epi32(T2_01A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			T3_01B = _mm256_srai_epi32(T2_01B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
			T3_02A = _mm256_srai_epi32(T2_02A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
			T3_02B = _mm256_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
			T3_03A = _mm256_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
			T3_03B = _mm256_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
			T3_04A = _mm256_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
			T3_04B = _mm256_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
			T3_05A = _mm256_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
			T3_05B = _mm256_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
			T3_06A = _mm256_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
			T3_06B = _mm256_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
			T3_07A = _mm256_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
			T3_07B = _mm256_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
			T3_08A = _mm256_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
			T3_08B = _mm256_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
			T3_09A = _mm256_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
			T3_09B = _mm256_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
			T3_10A = _mm256_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
			T3_10B = _mm256_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
			T3_11A = _mm256_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
			T3_11B = _mm256_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
			T3_12A = _mm256_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
			T3_12B = _mm256_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
			T3_13A = _mm256_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
			T3_13B = _mm256_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
			T3_14A = _mm256_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
			T3_14B = _mm256_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
			T3_15A = _mm256_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
			T3_15B = _mm256_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

			T3_16A = _mm256_srai_epi32(T2_16A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
			T3_16B = _mm256_srai_epi32(T2_16B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
			T3_17A = _mm256_srai_epi32(T2_17A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
			T3_17B = _mm256_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
			T3_18A = _mm256_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
			T3_18B = _mm256_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
			T3_19A = _mm256_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
			T3_19B = _mm256_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
			T3_20A = _mm256_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
			T3_20B = _mm256_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
			T3_21A = _mm256_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
			T3_21B = _mm256_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
			T3_22A = _mm256_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
			T3_22B = _mm256_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
			T3_23A = _mm256_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
			T3_23B = _mm256_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
			T3_24A = _mm256_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
			T3_24B = _mm256_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
			T3_25A = _mm256_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
			T3_25B = _mm256_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
			T3_26A = _mm256_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
			T3_26B = _mm256_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
			T3_27A = _mm256_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
			T3_27B = _mm256_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
			T3_28A = _mm256_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
			T3_28B = _mm256_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
			T3_29A = _mm256_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
			T3_29B = _mm256_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
			T3_30A = _mm256_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
			T3_30B = _mm256_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
			T3_31A = _mm256_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
			T3_31B = _mm256_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

			res00[part] = _mm256_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
			res01[part] = _mm256_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
			res02[part] = _mm256_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
			res03[part] = _mm256_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
			res04[part] = _mm256_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
			res05[part] = _mm256_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
			res06[part] = _mm256_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
			res07[part] = _mm256_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
			res08[part] = _mm256_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
			res09[part] = _mm256_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
			res10[part] = _mm256_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
			res11[part] = _mm256_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
			res12[part] = _mm256_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
			res13[part] = _mm256_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
			res14[part] = _mm256_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
			res15[part] = _mm256_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
			res16[part] = _mm256_packs_epi32(T3_16A, T3_16B);
			res17[part] = _mm256_packs_epi32(T3_17A, T3_17B);
			res18[part] = _mm256_packs_epi32(T3_18A, T3_18B);
			res19[part] = _mm256_packs_epi32(T3_19A, T3_19B);
			res20[part] = _mm256_packs_epi32(T3_20A, T3_20B);
			res21[part] = _mm256_packs_epi32(T3_21A, T3_21B);
			res22[part] = _mm256_packs_epi32(T3_22A, T3_22B);
			res23[part] = _mm256_packs_epi32(T3_23A, T3_23B);
			res24[part] = _mm256_packs_epi32(T3_24A, T3_24B);
			res25[part] = _mm256_packs_epi32(T3_25A, T3_25B);
			res26[part] = _mm256_packs_epi32(T3_26A, T3_26B);
			res27[part] = _mm256_packs_epi32(T3_27A, T3_27B);
			res28[part] = _mm256_packs_epi32(T3_28A, T3_28B);
			res29[part] = _mm256_packs_epi32(T3_29A, T3_29B);
			res30[part] = _mm256_packs_epi32(T3_30A, T3_30B);
			res31[part] = _mm256_packs_epi32(T3_31A, T3_31B);

		}

		//transpose 32x32 matrix
		{
			__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
	tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
	tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
	tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
	tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
	tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
	tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
	tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
	tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
	O0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	O1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	O2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	O3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	O4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	O5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	O6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	O7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	O8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
	O9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
	O10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
	O11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
	O12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
	O13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
	O14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
	O15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
	tr0_0 = _mm256_unpacklo_epi64(O0, O1); \
	tr0_1 = _mm256_unpacklo_epi64(O2, O3); \
	tr0_2 = _mm256_unpackhi_epi64(O0, O1); \
	tr0_3 = _mm256_unpackhi_epi64(O2, O3); \
	tr0_4 = _mm256_unpacklo_epi64(O4, O5); \
	tr0_5 = _mm256_unpacklo_epi64(O6, O7); \
	tr0_6 = _mm256_unpackhi_epi64(O4, O5); \
	tr0_7 = _mm256_unpackhi_epi64(O6, O7); \
	tr0_8 = _mm256_unpacklo_epi64(O8, O9); \
	tr0_9 = _mm256_unpacklo_epi64(O10, O11); \
	tr0_10 = _mm256_unpackhi_epi64(O8, O9); \
	tr0_11 = _mm256_unpackhi_epi64(O10, O11); \
	tr0_12 = _mm256_unpacklo_epi64(O12, O13); \
	tr0_13 = _mm256_unpacklo_epi64(O14, O15); \
	tr0_14 = _mm256_unpackhi_epi64(O12, O13); \
	tr0_15 = _mm256_unpackhi_epi64(O14, O15); \
	O0 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x20); \
	O1 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x20); \
	O2 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x20); \
	O3 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x20); \
	O4 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x20); \
	O5 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x20); \
	O6 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x20); \
	O7 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x20); \
	O8 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x31); \
	O9 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x31); \
	O10 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x31); \
	O11 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x31); \
	O12 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x31); \
	O13 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x31); \
	O14 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x31); \
	O15 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x31); \

			TRANSPOSE_16x16_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
			TRANSPOSE_16x16_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);
			TRANSPOSE_16x16_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0]);
			TRANSPOSE_16x16_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1]);

#undef TRANSPOSE_16x16_16BIT
		}
	}

	//clip
	max_val = _mm256_set1_epi16(511);
	min_val = _mm256_set1_epi16(-512);

	for (k = 0; k < 2; k++)
	{
		in00[k] = _mm256_min_epi16(in00[k], max_val);
		in00[k] = _mm256_max_epi16(in00[k], min_val);
		in01[k] = _mm256_min_epi16(in01[k], max_val);
		in01[k] = _mm256_max_epi16(in01[k], min_val);
		in02[k] = _mm256_min_epi16(in02[k], max_val);
		in02[k] = _mm256_max_epi16(in02[k], min_val);
		in03[k] = _mm256_min_epi16(in03[k], max_val);
		in03[k] = _mm256_max_epi16(in03[k], min_val);
		in04[k] = _mm256_min_epi16(in04[k], max_val);
		in04[k] = _mm256_max_epi16(in04[k], min_val);
		in05[k] = _mm256_min_epi16(in05[k], max_val);
		in05[k] = _mm256_max_epi16(in05[k], min_val);
		in06[k] = _mm256_min_epi16(in06[k], max_val);
		in06[k] = _mm256_max_epi16(in06[k], min_val);
		in07[k] = _mm256_min_epi16(in07[k], max_val);
		in07[k] = _mm256_max_epi16(in07[k], min_val);
		in08[k] = _mm256_min_epi16(in08[k], max_val);
		in08[k] = _mm256_max_epi16(in08[k], min_val);
		in09[k] = _mm256_min_epi16(in09[k], max_val);
		in09[k] = _mm256_max_epi16(in09[k], min_val);
		in10[k] = _mm256_min_epi16(in10[k], max_val);
		in10[k] = _mm256_max_epi16(in10[k], min_val);
		in11[k] = _mm256_min_epi16(in11[k], max_val);
		in11[k] = _mm256_max_epi16(in11[k], min_val);
		in12[k] = _mm256_min_epi16(in12[k], max_val);
		in12[k] = _mm256_max_epi16(in12[k], min_val);
		in13[k] = _mm256_min_epi16(in13[k], max_val);
		in13[k] = _mm256_max_epi16(in13[k], min_val);
		in14[k] = _mm256_min_epi16(in14[k], max_val);
		in14[k] = _mm256_max_epi16(in14[k], min_val);
		in15[k] = _mm256_min_epi16(in15[k], max_val);
		in15[k] = _mm256_max_epi16(in15[k], min_val);
		in16[k] = _mm256_min_epi16(in16[k], max_val);
		in16[k] = _mm256_max_epi16(in16[k], min_val);
		in17[k] = _mm256_min_epi16(in17[k], max_val);
		in17[k] = _mm256_max_epi16(in17[k], min_val);
		in18[k] = _mm256_min_epi16(in18[k], max_val);
		in18[k] = _mm256_max_epi16(in18[k], min_val);
		in19[k] = _mm256_min_epi16(in19[k], max_val);
		in19[k] = _mm256_max_epi16(in19[k], min_val);
		in20[k] = _mm256_min_epi16(in20[k], max_val);
		in20[k] = _mm256_max_epi16(in20[k], min_val);
		in21[k] = _mm256_min_epi16(in21[k], max_val);
		in21[k] = _mm256_max_epi16(in21[k], min_val);
		in22[k] = _mm256_min_epi16(in22[k], max_val);
		in22[k] = _mm256_max_epi16(in22[k], min_val);
		in23[k] = _mm256_min_epi16(in23[k], max_val);
		in23[k] = _mm256_max_epi16(in23[k], min_val);
		in24[k] = _mm256_min_epi16(in24[k], max_val);
		in24[k] = _mm256_max_epi16(in24[k], min_val);
		in25[k] = _mm256_min_epi16(in25[k], max_val);
		in25[k] = _mm256_max_epi16(in25[k], min_val);
		in26[k] = _mm256_min_epi16(in26[k], max_val);
		in26[k] = _mm256_max_epi16(in26[k], min_val);
		in27[k] = _mm256_min_epi16(in27[k], max_val);
		in27[k] = _mm256_max_epi16(in27[k], min_val);
		in28[k] = _mm256_min_epi16(in28[k], max_val);
		in28[k] = _mm256_max_epi16(in28[k], min_val);
		in29[k] = _mm256_min_epi16(in29[k], max_val);
		in29[k] = _mm256_max_epi16(in29[k], min_val);
		in30[k] = _mm256_min_epi16(in30[k], max_val);
		in30[k] = _mm256_max_epi16(in30[k], min_val);
		in31[k] = _mm256_min_epi16(in31[k], max_val);
		in31[k] = _mm256_max_epi16(in31[k], min_val);
	}


	//	int i;
    {
        __m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
        __m256i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
        __m256i	t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;

        //按行 64*64
        __m256i T00[4], T01[4], T02[4], T03[4], T04[4], T05[4], T06[4], T07[4], T08[4], T09[4], T10[4], T11[4], T12[4], T13[4], T14[4], T15[4], T16[4], T17[4], T18[4], T19[4], T20[4], T21[4], T22[4], T23[4], T24[4], T25[4], T26[4], T27[4], T28[4], T29[4], T30[4], T31[4], T32[4], T33[4], T34[4], T35[4], T36[4], T37[4], T38[4], T39[4], T40[4], T41[4], T42[4], T43[4], T44[4], T45[4], T46[4], T47[4], T48[4], T49[4], T50[4], T51[4], T52[4], T53[4], T54[4], T55[4], T56[4], T57[4], T58[4], T59[4], T60[4], T61[4], T62[4], T63[4];

        //按列 64*64
        __m256i V00[4], V01[4], V02[4], V03[4], V04[4], V05[4], V06[4], V07[4], V08[4], V09[4], V10[4], V11[4], V12[4], V13[4], V14[4], V15[4], V16[4], V17[4], V18[4], V19[4], V20[4], V21[4], V22[4], V23[4], V24[4], V25[4], V26[4], V27[4], V28[4], V29[4], V30[4], V31[4], V32[4], V33[4], V34[4], V35[4], V36[4], V37[4], V38[4], V39[4], V40[4], V41[4], V42[4], V43[4], V44[4], V45[4], V46[4], V47[4], V48[4], V49[4], V50[4], V51[4], V52[4], V53[4], V54[4], V55[4], V56[4], V57[4], V58[4], V59[4], V60[4], V61[4], V62[4], V63[4];

        /*--vertical transform--*/
        //32*32, LOAD AND SHIFT
        for (i = 0; i < 2; i++){
            T00[i] = _mm256_srai_epi16(in00[i], 1);
            T01[i] = _mm256_srai_epi16(in01[i], 1);
            T02[i] = _mm256_srai_epi16(in02[i], 1);
            T03[i] = _mm256_srai_epi16(in03[i], 1);
            T04[i] = _mm256_srai_epi16(in04[i], 1);
            T05[i] = _mm256_srai_epi16(in05[i], 1);
            T06[i] = _mm256_srai_epi16(in06[i], 1);
            T07[i] = _mm256_srai_epi16(in07[i], 1);
            T08[i] = _mm256_srai_epi16(in08[i], 1);
            T09[i] = _mm256_srai_epi16(in09[i], 1);

            T10[i] = _mm256_srai_epi16(in10[i], 1);
            T11[i] = _mm256_srai_epi16(in11[i], 1);
            T12[i] = _mm256_srai_epi16(in12[i], 1);
            T13[i] = _mm256_srai_epi16(in13[i], 1);
            T14[i] = _mm256_srai_epi16(in14[i], 1);
            T15[i] = _mm256_srai_epi16(in15[i], 1);
            T16[i] = _mm256_srai_epi16(in16[i], 1);
            T17[i] = _mm256_srai_epi16(in17[i], 1);
            T18[i] = _mm256_srai_epi16(in18[i], 1);
            T19[i] = _mm256_srai_epi16(in19[i], 1);

            T20[i] = _mm256_srai_epi16(in20[i], 1);
            T21[i] = _mm256_srai_epi16(in21[i], 1);
            T22[i] = _mm256_srai_epi16(in22[i], 1);
            T23[i] = _mm256_srai_epi16(in23[i], 1);
            T24[i] = _mm256_srai_epi16(in24[i], 1);
            T25[i] = _mm256_srai_epi16(in25[i], 1);
            T26[i] = _mm256_srai_epi16(in26[i], 1);
            T27[i] = _mm256_srai_epi16(in27[i], 1);
            T28[i] = _mm256_srai_epi16(in28[i], 1);
            T29[i] = _mm256_srai_epi16(in29[i], 1);

            T30[i] = _mm256_srai_epi16(in30[i], 1);
            T31[i] = _mm256_srai_epi16(in31[i], 1);
        }

        //filter (odd pixel/row)
        for (i = 0; i < 2; i++){
            T32[i] = _mm256_srai_epi16(_mm256_add_epi16(T00[i], T01[i]), 1);
            T33[i] = _mm256_srai_epi16(_mm256_add_epi16(T01[i], T02[i]), 1);
            T34[i] = _mm256_srai_epi16(_mm256_add_epi16(T02[i], T03[i]), 1);
            T35[i] = _mm256_srai_epi16(_mm256_add_epi16(T03[i], T04[i]), 1);
            T36[i] = _mm256_srai_epi16(_mm256_add_epi16(T04[i], T05[i]), 1);
            T37[i] = _mm256_srai_epi16(_mm256_add_epi16(T05[i], T06[i]), 1);
            T38[i] = _mm256_srai_epi16(_mm256_add_epi16(T06[i], T07[i]), 1);
            T39[i] = _mm256_srai_epi16(_mm256_add_epi16(T07[i], T08[i]), 1);

            T40[i] = _mm256_srai_epi16(_mm256_add_epi16(T08[i], T09[i]), 1);
            T41[i] = _mm256_srai_epi16(_mm256_add_epi16(T09[i], T10[i]), 1);
            T42[i] = _mm256_srai_epi16(_mm256_add_epi16(T10[i], T11[i]), 1);
            T43[i] = _mm256_srai_epi16(_mm256_add_epi16(T11[i], T12[i]), 1);
            T44[i] = _mm256_srai_epi16(_mm256_add_epi16(T12[i], T13[i]), 1);
            T45[i] = _mm256_srai_epi16(_mm256_add_epi16(T13[i], T14[i]), 1);
            T46[i] = _mm256_srai_epi16(_mm256_add_epi16(T14[i], T15[i]), 1);
            T47[i] = _mm256_srai_epi16(_mm256_add_epi16(T15[i], T16[i]), 1);

            T48[i] = _mm256_srai_epi16(_mm256_add_epi16(T16[i], T17[i]), 1);
            T49[i] = _mm256_srai_epi16(_mm256_add_epi16(T17[i], T18[i]), 1);
            T50[i] = _mm256_srai_epi16(_mm256_add_epi16(T18[i], T19[i]), 1);
            T51[i] = _mm256_srai_epi16(_mm256_add_epi16(T19[i], T20[i]), 1);
            T52[i] = _mm256_srai_epi16(_mm256_add_epi16(T20[i], T21[i]), 1);
            T53[i] = _mm256_srai_epi16(_mm256_add_epi16(T21[i], T22[i]), 1);
            T54[i] = _mm256_srai_epi16(_mm256_add_epi16(T22[i], T23[i]), 1);
            T55[i] = _mm256_srai_epi16(_mm256_add_epi16(T23[i], T24[i]), 1);

            T56[i] = _mm256_srai_epi16(_mm256_add_epi16(T24[i], T25[i]), 1);
            T57[i] = _mm256_srai_epi16(_mm256_add_epi16(T25[i], T26[i]), 1);
            T58[i] = _mm256_srai_epi16(_mm256_add_epi16(T26[i], T27[i]), 1);
            T59[i] = _mm256_srai_epi16(_mm256_add_epi16(T27[i], T28[i]), 1);
            T60[i] = _mm256_srai_epi16(_mm256_add_epi16(T28[i], T29[i]), 1);
            T61[i] = _mm256_srai_epi16(_mm256_add_epi16(T29[i], T30[i]), 1);
            T62[i] = _mm256_srai_epi16(_mm256_add_epi16(T30[i], T31[i]), 1);
            T63[i] = _mm256_srai_epi16(_mm256_add_epi16(T31[i], T31[i]), 1);
        }

        /*--transposition--*/
        //32x64 -> 64x32
        TRANSPOSE_16x16_16BIT_m256i(T00[0], T32[0], T01[0], T33[0], T02[0], T34[0], T03[0], T35[0], T04[0], T36[0], T05[0], T37[0], T06[0], T38[0], T07[0], T39[0], V00[0], V01[0], V02[0], V03[0], V04[0], V05[0], V06[0], V07[0], V08[0], V09[0], V10[0], V11[0], V12[0], V13[0], V14[0], V15[0]);
        TRANSPOSE_16x16_16BIT_m256i(T08[0], T40[0], T09[0], T41[0], T10[0], T42[0], T11[0], T43[0], T12[0], T44[0], T13[0], T45[0], T14[0], T46[0], T15[0], T47[0], V00[1], V01[1], V02[1], V03[1], V04[1], V05[1], V06[1], V07[1], V08[1], V09[1], V10[1], V11[1], V12[1], V13[1], V14[1], V15[1]);
        TRANSPOSE_16x16_16BIT_m256i(T16[0], T48[0], T17[0], T49[0], T18[0], T50[0], T19[0], T51[0], T20[0], T52[0], T21[0], T53[0], T22[0], T54[0], T23[0], T55[0], V00[2], V01[2], V02[2], V03[2], V04[2], V05[2], V06[2], V07[2], V08[2], V09[2], V10[2], V11[2], V12[2], V13[2], V14[2], V15[2]);
        TRANSPOSE_16x16_16BIT_m256i(T24[0], T56[0], T25[0], T57[0], T26[0], T58[0], T27[0], T59[0], T28[0], T60[0], T29[0], T61[0], T30[0], T62[0], T31[0], T63[0], V00[3], V01[3], V02[3], V03[3], V04[3], V05[3], V06[3], V07[3], V08[3], V09[3], V10[3], V11[3], V12[3], V13[3], V14[3], V15[3]);

        TRANSPOSE_16x16_16BIT_m256i(T00[1], T32[1], T01[1], T33[1], T02[1], T34[1], T03[1], T35[1], T04[1], T36[1], T05[1], T37[1], T06[1], T38[1], T07[1], T39[1], V16[0], V17[0], V18[0], V19[0], V20[0], V21[0], V22[0], V23[0], V24[0], V25[0], V26[0], V27[0], V28[0], V29[0], V30[0], V31[0]);
        TRANSPOSE_16x16_16BIT_m256i(T08[1], T40[1], T09[1], T41[1], T10[1], T42[1], T11[1], T43[1], T12[1], T44[1], T13[1], T45[1], T14[1], T46[1], T15[1], T47[1], V16[1], V17[1], V18[1], V19[1], V20[1], V21[1], V22[1], V23[1], V24[1], V25[1], V26[1], V27[1], V28[1], V29[1], V30[1], V31[1]);
        TRANSPOSE_16x16_16BIT_m256i(T16[1], T48[1], T17[1], T49[1], T18[1], T50[1], T19[1], T51[1], T20[1], T52[1], T21[1], T53[1], T22[1], T54[1], T23[1], T55[1], V16[2], V17[2], V18[2], V19[2], V20[2], V21[2], V22[2], V23[2], V24[2], V25[2], V26[2], V27[2], V28[2], V29[2], V30[2], V31[2]);
        TRANSPOSE_16x16_16BIT_m256i(T24[1], T56[1], T25[1], T57[1], T26[1], T58[1], T27[1], T59[1], T28[1], T60[1], T29[1], T61[1], T30[1], T62[1], T31[1], T63[1], V16[3], V17[3], V18[3], V19[3], V20[3], V21[3], V22[3], V23[3], V24[3], V25[3], V26[3], V27[3], V28[3], V29[3], V30[3], V31[3]);

        /*--horizontal transform--*/
        //filter (odd pixel/column)
        for (i = 0; i < 4; i++){
            V32[i] = _mm256_srai_epi16(_mm256_add_epi16(V00[i], V01[i]), 1);
            V33[i] = _mm256_srai_epi16(_mm256_add_epi16(V01[i], V02[i]), 1);
            V34[i] = _mm256_srai_epi16(_mm256_add_epi16(V02[i], V03[i]), 1);
            V35[i] = _mm256_srai_epi16(_mm256_add_epi16(V03[i], V04[i]), 1);
            V36[i] = _mm256_srai_epi16(_mm256_add_epi16(V04[i], V05[i]), 1);
            V37[i] = _mm256_srai_epi16(_mm256_add_epi16(V05[i], V06[i]), 1);
            V38[i] = _mm256_srai_epi16(_mm256_add_epi16(V06[i], V07[i]), 1);
            V39[i] = _mm256_srai_epi16(_mm256_add_epi16(V07[i], V08[i]), 1);
            V40[i] = _mm256_srai_epi16(_mm256_add_epi16(V08[i], V09[i]), 1);
            V41[i] = _mm256_srai_epi16(_mm256_add_epi16(V09[i], V10[i]), 1);
            V42[i] = _mm256_srai_epi16(_mm256_add_epi16(V10[i], V11[i]), 1);
            V43[i] = _mm256_srai_epi16(_mm256_add_epi16(V11[i], V12[i]), 1);
            V44[i] = _mm256_srai_epi16(_mm256_add_epi16(V12[i], V13[i]), 1);
            V45[i] = _mm256_srai_epi16(_mm256_add_epi16(V13[i], V14[i]), 1);
            V46[i] = _mm256_srai_epi16(_mm256_add_epi16(V14[i], V15[i]), 1);
            V47[i] = _mm256_srai_epi16(_mm256_add_epi16(V15[i], V16[i]), 1);

            V48[i] = _mm256_srai_epi16(_mm256_add_epi16(V16[i], V17[i]), 1);
            V49[i] = _mm256_srai_epi16(_mm256_add_epi16(V17[i], V18[i]), 1);
            V50[i] = _mm256_srai_epi16(_mm256_add_epi16(V18[i], V19[i]), 1);
            V51[i] = _mm256_srai_epi16(_mm256_add_epi16(V19[i], V20[i]), 1);
            V52[i] = _mm256_srai_epi16(_mm256_add_epi16(V20[i], V21[i]), 1);
            V53[i] = _mm256_srai_epi16(_mm256_add_epi16(V21[i], V22[i]), 1);
            V54[i] = _mm256_srai_epi16(_mm256_add_epi16(V22[i], V23[i]), 1);
            V55[i] = _mm256_srai_epi16(_mm256_add_epi16(V23[i], V24[i]), 1);
            V56[i] = _mm256_srai_epi16(_mm256_add_epi16(V24[i], V25[i]), 1);
            V57[i] = _mm256_srai_epi16(_mm256_add_epi16(V25[i], V26[i]), 1);
            V58[i] = _mm256_srai_epi16(_mm256_add_epi16(V26[i], V27[i]), 1);
            V59[i] = _mm256_srai_epi16(_mm256_add_epi16(V27[i], V28[i]), 1);
            V60[i] = _mm256_srai_epi16(_mm256_add_epi16(V28[i], V29[i]), 1);
            V61[i] = _mm256_srai_epi16(_mm256_add_epi16(V29[i], V30[i]), 1);
            V62[i] = _mm256_srai_epi16(_mm256_add_epi16(V30[i], V31[i]), 1);
            V63[i] = _mm256_srai_epi16(_mm256_add_epi16(V31[i], V31[i]), 1);
        }

        /*--transposition & Store--*/
        //64x64 
        TRANSPOSE_16x16_16BIT_m256i(V00[0], V32[0], V01[0], V33[0], V02[0], V34[0], V03[0], V35[0], V04[0], V36[0], V05[0], V37[0], V06[0], V38[0], V07[0], V39[0], T00[0], T01[0], T02[0], T03[0], T04[0], T05[0], T06[0], T07[0], T08[0], T09[0], T10[0], T11[0], T12[0], T13[0], T14[0], T15[0]);
        TRANSPOSE_16x16_16BIT_m256i(V00[1], V32[1], V01[1], V33[1], V02[1], V34[1], V03[1], V35[1], V04[1], V36[1], V05[1], V37[1], V06[1], V38[1], V07[1], V39[1], T16[0], T17[0], T18[0], T19[0], T20[0], T21[0], T22[0], T23[0], T24[0], T25[0], T26[0], T27[0], T28[0], T29[0], T30[0], T31[0]);
        TRANSPOSE_16x16_16BIT_m256i(V00[2], V32[2], V01[2], V33[2], V02[2], V34[2], V03[2], V35[2], V04[2], V36[2], V05[2], V37[2], V06[2], V38[2], V07[2], V39[2], T32[0], T33[0], T34[0], T35[0], T36[0], T37[0], T38[0], T39[0], T40[0], T41[0], T42[0], T43[0], T44[0], T45[0], T46[0], T47[0]);
        TRANSPOSE_16x16_16BIT_m256i(V00[3], V32[3], V01[3], V33[3], V02[3], V34[3], V03[3], V35[3], V04[3], V36[3], V05[3], V37[3], V06[3], V38[3], V07[3], V39[3], T48[0], T49[0], T50[0], T51[0], T52[0], T53[0], T54[0], T55[0], T56[0], T57[0], T58[0], T59[0], T60[0], T61[0], T62[0], T63[0]);

        TRANSPOSE_16x16_16BIT_m256i(V08[0], V40[0], V09[0], V41[0], V10[0], V42[0], V11[0], V43[0], V12[0], V44[0], V13[0], V45[0], V14[0], V46[0], V15[0], V47[0], T00[1], T01[1], T02[1], T03[1], T04[1], T05[1], T06[1], T07[1], T08[1], T09[1], T10[1], T11[1], T12[1], T13[1], T14[1], T15[1]);
        TRANSPOSE_16x16_16BIT_m256i(V08[1], V40[1], V09[1], V41[1], V10[1], V42[1], V11[1], V43[1], V12[1], V44[1], V13[1], V45[1], V14[1], V46[1], V15[1], V47[1], T16[1], T17[1], T18[1], T19[1], T20[1], T21[1], T22[1], T23[1], T24[1], T25[1], T26[1], T27[1], T28[1], T29[1], T30[1], T31[1]);
        TRANSPOSE_16x16_16BIT_m256i(V08[2], V40[2], V09[2], V41[2], V10[2], V42[2], V11[2], V43[2], V12[2], V44[2], V13[2], V45[2], V14[2], V46[2], V15[2], V47[2], T32[1], T33[1], T34[1], T35[1], T36[1], T37[1], T38[1], T39[1], T40[1], T41[1], T42[1], T43[1], T44[1], T45[1], T46[1], T47[1]);
        TRANSPOSE_16x16_16BIT_m256i(V08[3], V40[3], V09[3], V41[3], V10[3], V42[3], V11[3], V43[3], V12[3], V44[3], V13[3], V45[3], V14[3], V46[3], V15[3], V47[3], T48[1], T49[1], T50[1], T51[1], T52[1], T53[1], T54[1], T55[1], T56[1], T57[1], T58[1], T59[1], T60[1], T61[1], T62[1], T63[1]);

        TRANSPOSE_16x16_16BIT_m256i(V16[0], V48[0], V17[0], V49[0], V18[0], V50[0], V19[0], V51[0], V20[0], V52[0], V21[0], V53[0], V22[0], V54[0], V23[0], V55[0], T00[2], T01[2], T02[2], T03[2], T04[2], T05[2], T06[2], T07[2], T08[2], T09[2], T10[2], T11[2], T12[2], T13[2], T14[2], T15[2]);
        TRANSPOSE_16x16_16BIT_m256i(V16[1], V48[1], V17[1], V49[1], V18[1], V50[1], V19[1], V51[1], V20[1], V52[1], V21[1], V53[1], V22[1], V54[1], V23[1], V55[1], T16[2], T17[2], T18[2], T19[2], T20[2], T21[2], T22[2], T23[2], T24[2], T25[2], T26[2], T27[2], T28[2], T29[2], T30[2], T31[2]);
        TRANSPOSE_16x16_16BIT_m256i(V16[2], V48[2], V17[2], V49[2], V18[2], V50[2], V19[2], V51[2], V20[2], V52[2], V21[2], V53[2], V22[2], V54[2], V23[2], V55[2], T32[2], T33[2], T34[2], T35[2], T36[2], T37[2], T38[2], T39[2], T40[2], T41[2], T42[2], T43[2], T44[2], T45[2], T46[2], T47[2]);
        TRANSPOSE_16x16_16BIT_m256i(V16[3], V48[3], V17[3], V49[3], V18[3], V50[3], V19[3], V51[3], V20[3], V52[3], V21[3], V53[3], V22[3], V54[3], V23[3], V55[3], T48[2], T49[2], T50[2], T51[2], T52[2], T53[2], T54[2], T55[2], T56[2], T57[2], T58[2], T59[2], T60[2], T61[2], T62[2], T63[2]);

        TRANSPOSE_16x16_16BIT_m256i(V24[0], V56[0], V25[0], V57[0], V26[0], V58[0], V27[0], V59[0], V28[0], V60[0], V29[0], V61[0], V30[0], V62[0], V31[0], V63[0], T00[3], T01[3], T02[3], T03[3], T04[3], T05[3], T06[3], T07[3], T08[3], T09[3], T10[3], T11[3], T12[3], T13[3], T14[3], T15[3]);
        TRANSPOSE_16x16_16BIT_m256i(V24[1], V56[1], V25[1], V57[1], V26[1], V58[1], V27[1], V59[1], V28[1], V60[1], V29[1], V61[1], V30[1], V62[1], V31[1], V63[1], T16[3], T17[3], T18[3], T19[3], T20[3], T21[3], T22[3], T23[3], T24[3], T25[3], T26[3], T27[3], T28[3], T29[3], T30[3], T31[3]);
        TRANSPOSE_16x16_16BIT_m256i(V24[2], V56[2], V25[2], V57[2], V26[2], V58[2], V27[2], V59[2], V28[2], V60[2], V29[2], V61[2], V30[2], V62[2], V31[2], V63[2], T32[3], T33[3], T34[3], T35[3], T36[3], T37[3], T38[3], T39[3], T40[3], T41[3], T42[3], T43[3], T44[3], T45[3], T46[3], T47[3]);
        TRANSPOSE_16x16_16BIT_m256i(V24[3], V56[3], V25[3], V57[3], V26[3], V58[3], V27[3], V59[3], V28[3], V60[3], V29[3], V61[3], V30[3], V62[3], V31[3], V63[3], T48[3], T49[3], T50[3], T51[3], T52[3], T53[3], T54[3], T55[3], T56[3], T57[3], T58[3], T59[3], T60[3], T61[3], T62[3], T63[3]);

        for (k = 0; k < 2; k++)
        {
            int offset = (k << 5);
            P00[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[0 + offset]), 0xD8);
            P01[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[1 * i_pred + offset]), 0xD8);
            P02[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[2 * i_pred + offset]), 0xD8);
            P03[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[3 * i_pred + offset]), 0xD8);
            P04[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[4 * i_pred + offset]), 0xD8);
            P05[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[5 * i_pred + offset]), 0xD8);
            P06[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[6 * i_pred + offset]), 0xD8);
            P07[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[7 * i_pred + offset]), 0xD8);
            P08[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[8 * i_pred + offset]), 0xD8);
            P09[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[9 * i_pred + offset]), 0xD8);

            P10[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[10 * i_pred + offset]), 0xD8);
            P11[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[11 * i_pred + offset]), 0xD8);
            P12[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[12 * i_pred + offset]), 0xD8);
            P13[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[13 * i_pred + offset]), 0xD8);
            P14[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[14 * i_pred + offset]), 0xD8);
            P15[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[15 * i_pred + offset]), 0xD8);
            P16[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[16 * i_pred + offset]), 0xD8);
            P17[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[17 * i_pred + offset]), 0xD8);
            P18[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[18 * i_pred + offset]), 0xD8);
            P19[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[19 * i_pred + offset]), 0xD8);


            P20[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[20 * i_pred + offset]), 0xD8);
            P21[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[21 * i_pred + offset]), 0xD8);
            P22[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[22 * i_pred + offset]), 0xD8);
            P23[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[23 * i_pred + offset]), 0xD8);
            P24[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[24 * i_pred + offset]), 0xD8);
            P25[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[25 * i_pred + offset]), 0xD8);
            P26[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[26 * i_pred + offset]), 0xD8);
            P27[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[27 * i_pred + offset]), 0xD8);
            P28[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[28 * i_pred + offset]), 0xD8);
            P29[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[29 * i_pred + offset]), 0xD8);

            P30[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[30 * i_pred + offset]), 0xD8);
            P31[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[31 * i_pred + offset]), 0xD8);
            P32[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[32 * i_pred + offset]), 0xD8);
            P33[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[33 * i_pred + offset]), 0xD8);
            P34[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[34 * i_pred + offset]), 0xD8);
            P35[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[35 * i_pred + offset]), 0xD8);
            P36[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[36 * i_pred + offset]), 0xD8);
            P37[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[37 * i_pred + offset]), 0xD8);
            P38[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[38 * i_pred + offset]), 0xD8);
            P39[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[39 * i_pred + offset]), 0xD8);

            P40[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[40 * i_pred + offset]), 0xD8);
            P41[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[41 * i_pred + offset]), 0xD8);
            P42[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[42 * i_pred + offset]), 0xD8);
            P43[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[43 * i_pred + offset]), 0xD8);
            P44[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[44 * i_pred + offset]), 0xD8);
            P45[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[45 * i_pred + offset]), 0xD8);
            P46[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[46 * i_pred + offset]), 0xD8);
            P47[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[47 * i_pred + offset]), 0xD8);
            P48[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[48 * i_pred + offset]), 0xD8);
            P49[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[49 * i_pred + offset]), 0xD8);

            P50[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[50 * i_pred + offset]), 0xD8);
            P51[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[51 * i_pred + offset]), 0xD8);
            P52[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[52 * i_pred + offset]), 0xD8);
            P53[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[53 * i_pred + offset]), 0xD8);
            P54[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[54 * i_pred + offset]), 0xD8);
            P55[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[55 * i_pred + offset]), 0xD8);
            P56[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[56 * i_pred + offset]), 0xD8);
            P57[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[57 * i_pred + offset]), 0xD8);
            P58[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[58 * i_pred + offset]), 0xD8);
            P59[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[59 * i_pred + offset]), 0xD8);

            P60[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[60 * i_pred + offset]), 0xD8);
            P61[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[61 * i_pred + offset]), 0xD8);
            P62[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[62 * i_pred + offset]), 0xD8);
            P63[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[63 * i_pred + offset]), 0xD8);
        }



        for (k = 0; k < 2; k++)
        {
            int offset = k << 1;

            V00[0 + offset] = _mm256_unpacklo_epi8(P00[k], zero);
            V00[1 + offset] = _mm256_unpackhi_epi8(P00[k], zero);
            V01[0 + offset] = _mm256_unpacklo_epi8(P01[k], zero);
            V01[1 + offset] = _mm256_unpackhi_epi8(P01[k], zero);
            V02[0 + offset] = _mm256_unpacklo_epi8(P02[k], zero);
            V02[1 + offset] = _mm256_unpackhi_epi8(P02[k], zero);
            V03[0 + offset] = _mm256_unpacklo_epi8(P03[k], zero);
            V03[1 + offset] = _mm256_unpackhi_epi8(P03[k], zero);
            V04[0 + offset] = _mm256_unpacklo_epi8(P04[k], zero);
            V04[1 + offset] = _mm256_unpackhi_epi8(P04[k], zero);
            V05[0 + offset] = _mm256_unpacklo_epi8(P05[k], zero);
            V05[1 + offset] = _mm256_unpackhi_epi8(P05[k], zero);
            V06[0 + offset] = _mm256_unpacklo_epi8(P06[k], zero);
            V06[1 + offset] = _mm256_unpackhi_epi8(P06[k], zero);
            V07[0 + offset] = _mm256_unpacklo_epi8(P07[k], zero);
            V07[1 + offset] = _mm256_unpackhi_epi8(P07[k], zero);
            V08[0 + offset] = _mm256_unpacklo_epi8(P08[k], zero);
            V08[1 + offset] = _mm256_unpackhi_epi8(P08[k], zero);
            V09[0 + offset] = _mm256_unpacklo_epi8(P09[k], zero);
            V09[1 + offset] = _mm256_unpackhi_epi8(P09[k], zero);
            V10[0 + offset] = _mm256_unpacklo_epi8(P10[k], zero);
            V10[1 + offset] = _mm256_unpackhi_epi8(P10[k], zero);
            V11[0 + offset] = _mm256_unpacklo_epi8(P11[k], zero);
            V11[1 + offset] = _mm256_unpackhi_epi8(P11[k], zero);
            V12[0 + offset] = _mm256_unpacklo_epi8(P12[k], zero);
            V12[1 + offset] = _mm256_unpackhi_epi8(P12[k], zero);
            V13[0 + offset] = _mm256_unpacklo_epi8(P13[k], zero);
            V13[1 + offset] = _mm256_unpackhi_epi8(P13[k], zero);
            V14[0 + offset] = _mm256_unpacklo_epi8(P14[k], zero);
            V14[1 + offset] = _mm256_unpackhi_epi8(P14[k], zero);
            V15[0 + offset] = _mm256_unpacklo_epi8(P15[k], zero);
            V15[1 + offset] = _mm256_unpackhi_epi8(P15[k], zero);
            V16[0 + offset] = _mm256_unpacklo_epi8(P16[k], zero);
            V16[1 + offset] = _mm256_unpackhi_epi8(P16[k], zero);
            V17[0 + offset] = _mm256_unpacklo_epi8(P17[k], zero);
            V17[1 + offset] = _mm256_unpackhi_epi8(P17[k], zero);
            V18[0 + offset] = _mm256_unpacklo_epi8(P18[k], zero);
            V18[1 + offset] = _mm256_unpackhi_epi8(P18[k], zero);
            V19[0 + offset] = _mm256_unpacklo_epi8(P19[k], zero);
            V19[1 + offset] = _mm256_unpackhi_epi8(P19[k], zero);
            V20[0 + offset] = _mm256_unpacklo_epi8(P20[k], zero);
            V20[1 + offset] = _mm256_unpackhi_epi8(P20[k], zero);
            V21[0 + offset] = _mm256_unpacklo_epi8(P21[k], zero);
            V21[1 + offset] = _mm256_unpackhi_epi8(P21[k], zero);
            V22[0 + offset] = _mm256_unpacklo_epi8(P22[k], zero);
            V22[1 + offset] = _mm256_unpackhi_epi8(P22[k], zero);
            V23[0 + offset] = _mm256_unpacklo_epi8(P23[k], zero);
            V23[1 + offset] = _mm256_unpackhi_epi8(P23[k], zero);
            V24[0 + offset] = _mm256_unpacklo_epi8(P24[k], zero);
            V24[1 + offset] = _mm256_unpackhi_epi8(P24[k], zero);
            V25[0 + offset] = _mm256_unpacklo_epi8(P25[k], zero);
            V25[1 + offset] = _mm256_unpackhi_epi8(P25[k], zero);
            V26[0 + offset] = _mm256_unpacklo_epi8(P26[k], zero);
            V26[1 + offset] = _mm256_unpackhi_epi8(P26[k], zero);
            V27[0 + offset] = _mm256_unpacklo_epi8(P27[k], zero);
            V27[1 + offset] = _mm256_unpackhi_epi8(P27[k], zero);
            V28[0 + offset] = _mm256_unpacklo_epi8(P28[k], zero);
            V28[1 + offset] = _mm256_unpackhi_epi8(P28[k], zero);
            V29[0 + offset] = _mm256_unpacklo_epi8(P29[k], zero);
            V29[1 + offset] = _mm256_unpackhi_epi8(P29[k], zero);

            V30[0 + offset] = _mm256_unpacklo_epi8(P30[k], zero);
            V30[1 + offset] = _mm256_unpackhi_epi8(P30[k], zero);
            V31[0 + offset] = _mm256_unpacklo_epi8(P31[k], zero);
            V31[1 + offset] = _mm256_unpackhi_epi8(P31[k], zero);
            V32[0 + offset] = _mm256_unpacklo_epi8(P32[k], zero);
            V32[1 + offset] = _mm256_unpackhi_epi8(P32[k], zero);
            V33[0 + offset] = _mm256_unpacklo_epi8(P33[k], zero);
            V33[1 + offset] = _mm256_unpackhi_epi8(P33[k], zero);
            V34[0 + offset] = _mm256_unpacklo_epi8(P34[k], zero);
            V34[1 + offset] = _mm256_unpackhi_epi8(P34[k], zero);
            V35[0 + offset] = _mm256_unpacklo_epi8(P35[k], zero);
            V35[1 + offset] = _mm256_unpackhi_epi8(P35[k], zero);
            V36[0 + offset] = _mm256_unpacklo_epi8(P36[k], zero);
            V36[1 + offset] = _mm256_unpackhi_epi8(P36[k], zero);
            V37[0 + offset] = _mm256_unpacklo_epi8(P37[k], zero);
            V37[1 + offset] = _mm256_unpackhi_epi8(P37[k], zero);
            V38[0 + offset] = _mm256_unpacklo_epi8(P38[k], zero);
            V38[1 + offset] = _mm256_unpackhi_epi8(P38[k], zero);
            V39[0 + offset] = _mm256_unpacklo_epi8(P39[k], zero);
            V39[1 + offset] = _mm256_unpackhi_epi8(P39[k], zero);

            V40[0 + offset] = _mm256_unpacklo_epi8(P40[k], zero);
            V40[1 + offset] = _mm256_unpackhi_epi8(P40[k], zero);
            V41[0 + offset] = _mm256_unpacklo_epi8(P41[k], zero);
            V41[1 + offset] = _mm256_unpackhi_epi8(P41[k], zero);
            V42[0 + offset] = _mm256_unpacklo_epi8(P42[k], zero);
            V42[1 + offset] = _mm256_unpackhi_epi8(P42[k], zero);
            V43[0 + offset] = _mm256_unpacklo_epi8(P43[k], zero);
            V43[1 + offset] = _mm256_unpackhi_epi8(P43[k], zero);
            V44[0 + offset] = _mm256_unpacklo_epi8(P44[k], zero);
            V44[1 + offset] = _mm256_unpackhi_epi8(P44[k], zero);
            V45[0 + offset] = _mm256_unpacklo_epi8(P45[k], zero);
            V45[1 + offset] = _mm256_unpackhi_epi8(P45[k], zero);
            V46[0 + offset] = _mm256_unpacklo_epi8(P46[k], zero);
            V46[1 + offset] = _mm256_unpackhi_epi8(P46[k], zero);
            V47[0 + offset] = _mm256_unpacklo_epi8(P47[k], zero);
            V47[1 + offset] = _mm256_unpackhi_epi8(P47[k], zero);
            V48[0 + offset] = _mm256_unpacklo_epi8(P48[k], zero);
            V48[1 + offset] = _mm256_unpackhi_epi8(P48[k], zero);
            V49[0 + offset] = _mm256_unpacklo_epi8(P49[k], zero);
            V49[1 + offset] = _mm256_unpackhi_epi8(P49[k], zero);

            V50[0 + offset] = _mm256_unpacklo_epi8(P50[k], zero);
            V50[1 + offset] = _mm256_unpackhi_epi8(P50[k], zero);
            V51[0 + offset] = _mm256_unpacklo_epi8(P51[k], zero);
            V51[1 + offset] = _mm256_unpackhi_epi8(P51[k], zero);
            V52[0 + offset] = _mm256_unpacklo_epi8(P52[k], zero);
            V52[1 + offset] = _mm256_unpackhi_epi8(P52[k], zero);
            V53[0 + offset] = _mm256_unpacklo_epi8(P53[k], zero);
            V53[1 + offset] = _mm256_unpackhi_epi8(P53[k], zero);
            V54[0 + offset] = _mm256_unpacklo_epi8(P54[k], zero);
            V54[1 + offset] = _mm256_unpackhi_epi8(P54[k], zero);
            V55[0 + offset] = _mm256_unpacklo_epi8(P55[k], zero);
            V55[1 + offset] = _mm256_unpackhi_epi8(P55[k], zero);
            V56[0 + offset] = _mm256_unpacklo_epi8(P56[k], zero);
            V56[1 + offset] = _mm256_unpackhi_epi8(P56[k], zero);
            V57[0 + offset] = _mm256_unpacklo_epi8(P57[k], zero);
            V57[1 + offset] = _mm256_unpackhi_epi8(P57[k], zero);
            V58[0 + offset] = _mm256_unpacklo_epi8(P58[k], zero);
            V58[1 + offset] = _mm256_unpackhi_epi8(P58[k], zero);
            V59[0 + offset] = _mm256_unpacklo_epi8(P59[k], zero);
            V59[1 + offset] = _mm256_unpackhi_epi8(P59[k], zero);

            V60[0 + offset] = _mm256_unpacklo_epi8(P60[k], zero);
            V60[1 + offset] = _mm256_unpackhi_epi8(P60[k], zero);
            V61[0 + offset] = _mm256_unpacklo_epi8(P61[k], zero);
            V61[1 + offset] = _mm256_unpackhi_epi8(P61[k], zero);
            V62[0 + offset] = _mm256_unpacklo_epi8(P62[k], zero);
            V62[1 + offset] = _mm256_unpackhi_epi8(P62[k], zero);
            V63[0 + offset] = _mm256_unpacklo_epi8(P63[k], zero);
            V63[1 + offset] = _mm256_unpackhi_epi8(P63[k], zero);
        }


        for (k = 0; k < 4; k++)
        {
            T00[k] = _mm256_add_epi16(V00[k], T00[k]);
            T01[k] = _mm256_add_epi16(V01[k], T01[k]);
            T02[k] = _mm256_add_epi16(V02[k], T02[k]);
            T03[k] = _mm256_add_epi16(V03[k], T03[k]);
            T04[k] = _mm256_add_epi16(V04[k], T04[k]);
            T05[k] = _mm256_add_epi16(V05[k], T05[k]);
            T06[k] = _mm256_add_epi16(V06[k], T06[k]);
            T07[k] = _mm256_add_epi16(V07[k], T07[k]);
            T08[k] = _mm256_add_epi16(V08[k], T08[k]);
            T09[k] = _mm256_add_epi16(V09[k], T09[k]);
            T10[k] = _mm256_add_epi16(V10[k], T10[k]);
            T11[k] = _mm256_add_epi16(V11[k], T11[k]);
            T12[k] = _mm256_add_epi16(V12[k], T12[k]);
            T13[k] = _mm256_add_epi16(V13[k], T13[k]);
            T14[k] = _mm256_add_epi16(V14[k], T14[k]);
            T15[k] = _mm256_add_epi16(V15[k], T15[k]);
            T16[k] = _mm256_add_epi16(V16[k], T16[k]);
            T17[k] = _mm256_add_epi16(V17[k], T17[k]);
            T18[k] = _mm256_add_epi16(V18[k], T18[k]);
            T19[k] = _mm256_add_epi16(V19[k], T19[k]);
            T20[k] = _mm256_add_epi16(V20[k], T20[k]);
            T21[k] = _mm256_add_epi16(V21[k], T21[k]);
            T22[k] = _mm256_add_epi16(V22[k], T22[k]);
            T23[k] = _mm256_add_epi16(V23[k], T23[k]);
            T24[k] = _mm256_add_epi16(V24[k], T24[k]);
            T25[k] = _mm256_add_epi16(V25[k], T25[k]);
            T26[k] = _mm256_add_epi16(V26[k], T26[k]);
            T27[k] = _mm256_add_epi16(V27[k], T27[k]);
            T28[k] = _mm256_add_epi16(V28[k], T28[k]);
            T29[k] = _mm256_add_epi16(V29[k], T29[k]);

            T30[k] = _mm256_add_epi16(V30[k], T30[k]);
            T31[k] = _mm256_add_epi16(V31[k], T31[k]);
            T32[k] = _mm256_add_epi16(V32[k], T32[k]);
            T33[k] = _mm256_add_epi16(V33[k], T33[k]);
            T34[k] = _mm256_add_epi16(V34[k], T34[k]);
            T35[k] = _mm256_add_epi16(V35[k], T35[k]);
            T36[k] = _mm256_add_epi16(V36[k], T36[k]);
            T37[k] = _mm256_add_epi16(V37[k], T37[k]);
            T38[k] = _mm256_add_epi16(V38[k], T38[k]);
            T39[k] = _mm256_add_epi16(V39[k], T39[k]);

            T40[k] = _mm256_add_epi16(V40[k], T40[k]);
            T41[k] = _mm256_add_epi16(V41[k], T41[k]);
            T42[k] = _mm256_add_epi16(V42[k], T42[k]);
            T43[k] = _mm256_add_epi16(V43[k], T43[k]);
            T44[k] = _mm256_add_epi16(V44[k], T44[k]);
            T45[k] = _mm256_add_epi16(V45[k], T45[k]);
            T46[k] = _mm256_add_epi16(V46[k], T46[k]);
            T47[k] = _mm256_add_epi16(V47[k], T47[k]);
            T48[k] = _mm256_add_epi16(V48[k], T48[k]);
            T49[k] = _mm256_add_epi16(V49[k], T49[k]);

            T50[k] = _mm256_add_epi16(V50[k], T50[k]);
            T51[k] = _mm256_add_epi16(V51[k], T51[k]);
            T52[k] = _mm256_add_epi16(V52[k], T52[k]);
            T53[k] = _mm256_add_epi16(V53[k], T53[k]);
            T54[k] = _mm256_add_epi16(V54[k], T54[k]);
            T55[k] = _mm256_add_epi16(V55[k], T55[k]);
            T56[k] = _mm256_add_epi16(V56[k], T56[k]);
            T57[k] = _mm256_add_epi16(V57[k], T57[k]);
            T58[k] = _mm256_add_epi16(V58[k], T58[k]);
            T59[k] = _mm256_add_epi16(V59[k], T59[k]);

            T60[k] = _mm256_add_epi16(V60[k], T60[k]);
            T61[k] = _mm256_add_epi16(V61[k], T61[k]);
            T62[k] = _mm256_add_epi16(V62[k], T62[k]);
            T63[k] = _mm256_add_epi16(V63[k], T63[k]);

        }

        for (k = 0; k < 4; k += 2)
        {
            V00[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T00[k], T00[k + 1]), 0xD8);
            V01[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T01[k], T01[k + 1]), 0xD8);
            V02[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T02[k], T02[k + 1]), 0xD8);
            V03[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T03[k], T03[k + 1]), 0xD8);
            V04[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T04[k], T04[k + 1]), 0xD8);
            V05[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T05[k], T05[k + 1]), 0xD8);
            V06[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T06[k], T06[k + 1]), 0xD8);
            V07[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T07[k], T07[k + 1]), 0xD8);
            V08[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T08[k], T08[k + 1]), 0xD8);
            V09[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T09[k], T09[k + 1]), 0xD8);
            V10[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T10[k], T10[k + 1]), 0xD8);
            V11[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T11[k], T11[k + 1]), 0xD8);
            V12[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T12[k], T12[k + 1]), 0xD8);
            V13[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T13[k], T13[k + 1]), 0xD8);
            V14[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T14[k], T14[k + 1]), 0xD8);
            V15[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T15[k], T15[k + 1]), 0xD8);
            V16[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T16[k], T16[k + 1]), 0xD8);
            V17[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T17[k], T17[k + 1]), 0xD8);
            V18[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T18[k], T18[k + 1]), 0xD8);
            V19[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T19[k], T19[k + 1]), 0xD8);
            V20[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T20[k], T20[k + 1]), 0xD8);
            V21[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T21[k], T21[k + 1]), 0xD8);
            V22[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T22[k], T22[k + 1]), 0xD8);
            V23[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T23[k], T23[k + 1]), 0xD8);
            V24[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T24[k], T24[k + 1]), 0xD8);
            V25[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T25[k], T25[k + 1]), 0xD8);
            V26[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T26[k], T26[k + 1]), 0xD8);
            V27[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T27[k], T27[k + 1]), 0xD8);
            V28[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T28[k], T28[k + 1]), 0xD8);
            V29[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T29[k], T29[k + 1]), 0xD8);
            V30[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T30[k], T30[k + 1]), 0xD8);
            V31[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T31[k], T31[k + 1]), 0xD8);
            V32[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T32[k], T32[k + 1]), 0xD8);
            V33[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T33[k], T33[k + 1]), 0xD8);
            V34[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T34[k], T34[k + 1]), 0xD8);
            V35[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T35[k], T35[k + 1]), 0xD8);
            V36[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T36[k], T36[k + 1]), 0xD8);
            V37[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T37[k], T37[k + 1]), 0xD8);
            V38[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T38[k], T38[k + 1]), 0xD8);
            V39[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T39[k], T39[k + 1]), 0xD8);
            V40[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T40[k], T40[k + 1]), 0xD8);
            V41[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T41[k], T41[k + 1]), 0xD8);
            V42[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T42[k], T42[k + 1]), 0xD8);
            V43[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T43[k], T43[k + 1]), 0xD8);
            V44[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T44[k], T44[k + 1]), 0xD8);
            V45[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T45[k], T45[k + 1]), 0xD8);
            V46[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T46[k], T46[k + 1]), 0xD8);
            V47[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T47[k], T47[k + 1]), 0xD8);
            V48[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T48[k], T48[k + 1]), 0xD8);
            V49[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T49[k], T49[k + 1]), 0xD8);
            V50[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T50[k], T50[k + 1]), 0xD8);
            V51[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T51[k], T51[k + 1]), 0xD8);
            V52[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T52[k], T52[k + 1]), 0xD8);
            V53[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T53[k], T53[k + 1]), 0xD8);
            V54[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T54[k], T54[k + 1]), 0xD8);
            V55[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T55[k], T55[k + 1]), 0xD8);
            V56[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T56[k], T56[k + 1]), 0xD8);
            V57[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T57[k], T57[k + 1]), 0xD8);
            V58[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T58[k], T58[k + 1]), 0xD8);
            V59[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T59[k], T59[k + 1]), 0xD8);
            V60[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T60[k], T60[k + 1]), 0xD8);
            V61[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T61[k], T61[k + 1]), 0xD8);
            V62[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T62[k], T62[k + 1]), 0xD8);
            V63[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T63[k], T63[k + 1]), 0xD8);
        }


        for (k = 0; k < 4; k += 2)
        {
            int offset = k << 4;
            _mm256_storeu_si256((__m256i*)&dst[0 + offset], V00[k]);
            _mm256_storeu_si256((__m256i*)&dst[1 * i_dst + offset], V01[k]);
            _mm256_storeu_si256((__m256i*)&dst[2 * i_dst + offset], V02[k]);
            _mm256_storeu_si256((__m256i*)&dst[3 * i_dst + offset], V03[k]);
            _mm256_storeu_si256((__m256i*)&dst[4 * i_dst + offset], V04[k]);
            _mm256_storeu_si256((__m256i*)&dst[5 * i_dst + offset], V05[k]);
            _mm256_storeu_si256((__m256i*)&dst[6 * i_dst + offset], V06[k]);
            _mm256_storeu_si256((__m256i*)&dst[7 * i_dst + offset], V07[k]);
            _mm256_storeu_si256((__m256i*)&dst[8 * i_dst + offset], V08[k]);
            _mm256_storeu_si256((__m256i*)&dst[9 * i_dst + offset], V09[k]);

            _mm256_storeu_si256((__m256i*)&dst[10 * i_dst + offset], V10[k]);
            _mm256_storeu_si256((__m256i*)&dst[11 * i_dst + offset], V11[k]);
            _mm256_storeu_si256((__m256i*)&dst[12 * i_dst + offset], V12[k]);
            _mm256_storeu_si256((__m256i*)&dst[13 * i_dst + offset], V13[k]);
            _mm256_storeu_si256((__m256i*)&dst[14 * i_dst + offset], V14[k]);
            _mm256_storeu_si256((__m256i*)&dst[15 * i_dst + offset], V15[k]);
            _mm256_storeu_si256((__m256i*)&dst[16 * i_dst + offset], V16[k]);
            _mm256_storeu_si256((__m256i*)&dst[17 * i_dst + offset], V17[k]);
            _mm256_storeu_si256((__m256i*)&dst[18 * i_dst + offset], V18[k]);
            _mm256_storeu_si256((__m256i*)&dst[19 * i_dst + offset], V19[k]);

            _mm256_storeu_si256((__m256i*)&dst[20 * i_dst + offset], V20[k]);
            _mm256_storeu_si256((__m256i*)&dst[21 * i_dst + offset], V21[k]);
            _mm256_storeu_si256((__m256i*)&dst[22 * i_dst + offset], V22[k]);
            _mm256_storeu_si256((__m256i*)&dst[23 * i_dst + offset], V23[k]);
            _mm256_storeu_si256((__m256i*)&dst[24 * i_dst + offset], V24[k]);
            _mm256_storeu_si256((__m256i*)&dst[25 * i_dst + offset], V25[k]);
            _mm256_storeu_si256((__m256i*)&dst[26 * i_dst + offset], V26[k]);
            _mm256_storeu_si256((__m256i*)&dst[27 * i_dst + offset], V27[k]);
            _mm256_storeu_si256((__m256i*)&dst[28 * i_dst + offset], V28[k]);
            _mm256_storeu_si256((__m256i*)&dst[29 * i_dst + offset], V29[k]);

            _mm256_storeu_si256((__m256i*)&dst[30 * i_dst + offset], V30[k]);
            _mm256_storeu_si256((__m256i*)&dst[31 * i_dst + offset], V31[k]);
            _mm256_storeu_si256((__m256i*)&dst[32 * i_dst + offset], V32[k]);
            _mm256_storeu_si256((__m256i*)&dst[33 * i_dst + offset], V33[k]);
            _mm256_storeu_si256((__m256i*)&dst[34 * i_dst + offset], V34[k]);
            _mm256_storeu_si256((__m256i*)&dst[35 * i_dst + offset], V35[k]);
            _mm256_storeu_si256((__m256i*)&dst[36 * i_dst + offset], V36[k]);
            _mm256_storeu_si256((__m256i*)&dst[37 * i_dst + offset], V37[k]);
            _mm256_storeu_si256((__m256i*)&dst[38 * i_dst + offset], V38[k]);
            _mm256_storeu_si256((__m256i*)&dst[39 * i_dst + offset], V39[k]);

            _mm256_storeu_si256((__m256i*)&dst[40 * i_dst + offset], V40[k]);
            _mm256_storeu_si256((__m256i*)&dst[41 * i_dst + offset], V41[k]);
            _mm256_storeu_si256((__m256i*)&dst[42 * i_dst + offset], V42[k]);
            _mm256_storeu_si256((__m256i*)&dst[43 * i_dst + offset], V43[k]);
            _mm256_storeu_si256((__m256i*)&dst[44 * i_dst + offset], V44[k]);
            _mm256_storeu_si256((__m256i*)&dst[45 * i_dst + offset], V45[k]);
            _mm256_storeu_si256((__m256i*)&dst[46 * i_dst + offset], V46[k]);
            _mm256_storeu_si256((__m256i*)&dst[47 * i_dst + offset], V47[k]);
            _mm256_storeu_si256((__m256i*)&dst[48 * i_dst + offset], V48[k]);
            _mm256_storeu_si256((__m256i*)&dst[49 * i_dst + offset], V49[k]);

            _mm256_storeu_si256((__m256i*)&dst[50 * i_dst + offset], V50[k]);
            _mm256_storeu_si256((__m256i*)&dst[51 * i_dst + offset], V51[k]);
            _mm256_storeu_si256((__m256i*)&dst[52 * i_dst + offset], V52[k]);
            _mm256_storeu_si256((__m256i*)&dst[53 * i_dst + offset], V53[k]);
            _mm256_storeu_si256((__m256i*)&dst[54 * i_dst + offset], V54[k]);
            _mm256_storeu_si256((__m256i*)&dst[55 * i_dst + offset], V55[k]);
            _mm256_storeu_si256((__m256i*)&dst[56 * i_dst + offset], V56[k]);
            _mm256_storeu_si256((__m256i*)&dst[57 * i_dst + offset], V57[k]);
            _mm256_storeu_si256((__m256i*)&dst[58 * i_dst + offset], V58[k]);
            _mm256_storeu_si256((__m256i*)&dst[59 * i_dst + offset], V59[k]);

            _mm256_storeu_si256((__m256i*)&dst[60 * i_dst + offset], V60[k]);
            _mm256_storeu_si256((__m256i*)&dst[61 * i_dst + offset], V61[k]);
            _mm256_storeu_si256((__m256i*)&dst[62 * i_dst + offset], V62[k]);
            _mm256_storeu_si256((__m256i*)&dst[63 * i_dst + offset], V63[k]);

        }
    }
}

void add_inv_trans_ext_64x64_sse256(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
	int k, i;
	__m256i P00[2], P01[2], P02[2], P03[2], P04[2], P05[2], P06[2], P07[2], P08[2], P09[2], P10[2], P11[2], P12[2], P13[2], P14[2], P15[2], P16[2], P17[2], P18[2], P19[2],
		P20[2], P21[2], P22[2], P23[2], P24[2], P25[2], P26[2], P27[2], P28[2], P29[2], P30[2], P31[2], P32[2], P33[2], P34[2], P35[2], P36[2], P37[2], P38[2], P39[2],
		P40[2], P41[2], P42[2], P43[2], P44[2], P45[2], P46[2], P47[2], P48[2], P49[2], P50[2], P51[2], P52[2], P53[2], P54[2], P55[2], P56[2], P57[2], P58[2], P59[2], P60[2], P61[2], P62[2], P63[2];
	__m256i max_val, min_val;
	__m256i EEO0A, EEO1A, EEO2A, EEO3A, EEO0B, EEO1B, EEO2B, EEO3B;
	__m256i EEEO0A, EEEO0B, EEEO1A, EEEO1B;
	__m256i EEEE0A, EEEE0B, EEEE1A, EEEE1B;
	__m256i EEE0A, EEE0B, EEE1A, EEE1B, EEE3A, EEE3B, EEE2A, EEE2B;
	__m256i EE0A, EE0B, EE1A, EE1B, EE2A, EE2B, EE3A, EE3B, EE7A, EE7B, EE6A, EE6B, EE5A, EE5B, EE4A, EE4B;
	__m256i E0A, E0B, E1A, E1B, E2A, E2B, E3A, E3B, E4A, E4B, E5A, E5B, E6A, E6B, E7A, E7B, EFA, EFB, EEA, EEB, EDA, EDB, ECA, ECB, EBA, EBB, EAA, EAB, E9A, E9B, E8A, E8B;
	__m256i T10A, T10B, T11A, T11B, T12A, T12B, T13A, T13B, T14A, T14B, T15A, T15B, T16A, T16B, T17A, T17B, T18A, T18B, T19A, T19B, T1AA, T1AB, T1BA, T1BB, T1CA, T1CB, T1DA, T1DB, T1EA, T1EB, T1FA, T1FB;
	__m256i T2_00A, T2_00B, T2_01A, T2_01B, T2_02A, T2_02B, T2_03A, T2_03B, T2_04A, T2_04B, T2_05A, T2_05B, T2_06A, T2_06B, T2_07A, T2_07B, T2_08A, T2_08B, T2_09A, T2_09B, T2_10A, T2_10B, T2_11A, T2_11B, T2_12A, T2_12B, T2_13A, T2_13B, T2_14A, T2_14B, T2_15A, T2_15B, T2_31A, T2_31B, T2_30A, T2_30B, T2_29A, T2_29B, T2_28A, T2_28B, T2_27A, T2_27B, T2_26A, T2_26B, T2_25A, T2_25B, T2_24A, T2_24B, T2_23A, T2_23B, T2_22A, T2_22B, T2_21A, T2_21B, T2_20A, T2_20B, T2_19A, T2_19B, T2_18A, T2_18B, T2_17A, T2_17B, T2_16A, T2_16B;
	__m256i T3_00A, T3_00B, T3_01A, T3_01B, T3_02A, T3_02B, T3_03A, T3_03B, T3_04A, T3_04B, T3_05A, T3_05B, T3_06A, T3_06B, T3_07A, T3_07B, T3_08A, T3_08B, T3_09A, T3_09B, T3_10A, T3_10B, T3_11A, T3_11B, T3_12A, T3_12B, T3_13A, T3_13B, T3_14A, T3_14B, T3_15A, T3_15B;
	__m256i T3_16A, T3_16B, T3_17A, T3_17B, T3_18A, T3_18B, T3_19A, T3_19B, T3_20A, T3_20B, T3_21A, T3_21B, T3_22A, T3_22B, T3_23A, T3_23B, T3_24A, T3_24B, T3_25A, T3_25B, T3_26A, T3_26B, T3_27A, T3_27B, T3_28A, T3_28B, T3_29A, T3_29B, T3_30A, T3_30B, T3_31A, T3_31B;
	const __m256i c16_p45_p45 = _mm256_set1_epi32(0x002D002D);
	const __m256i c16_p43_p44 = _mm256_set1_epi32(0x002B002C);
	const __m256i c16_p39_p41 = _mm256_set1_epi32(0x00270029);
	const __m256i c16_p34_p36 = _mm256_set1_epi32(0x00220024);
	//	const __m256i c16_p27_p30 = _mm256_set1_epi32(0x001B001E);
	//	const __m256i c16_p19_p23 = _mm256_set1_epi32(0x00130017);
	//	const __m256i c16_p11_p15 = _mm256_set1_epi32(0x000B000F);
	//	const __m256i c16_p02_p07 = _mm256_set1_epi32(0x00020007);
	const __m256i c16_p41_p45 = _mm256_set1_epi32(0x0029002D);
	const __m256i c16_p23_p34 = _mm256_set1_epi32(0x00170022);
	const __m256i c16_n02_p11 = _mm256_set1_epi32(0xFFFE000B);
	const __m256i c16_n27_n15 = _mm256_set1_epi32(0xFFE5FFF1);
	//	const __m256i c16_n43_n36 = _mm256_set1_epi32(0xFFD5FFDC);
	//	const __m256i c16_n44_n45 = _mm256_set1_epi32(0xFFD4FFD3);
	//	const __m256i c16_n30_n39 = _mm256_set1_epi32(0xFFE2FFD9);
	//	const __m256i c16_n07_n19 = _mm256_set1_epi32(0xFFF9FFED);
	const __m256i c16_p34_p44 = _mm256_set1_epi32(0x0022002C);
	const __m256i c16_n07_p15 = _mm256_set1_epi32(0xFFF9000F);
	const __m256i c16_n41_n27 = _mm256_set1_epi32(0xFFD7FFE5);
	const __m256i c16_n39_n45 = _mm256_set1_epi32(0xFFD9FFD3);
	//	const __m256i c16_n02_n23 = _mm256_set1_epi32(0xFFFEFFE9);
	//	const __m256i c16_p36_p19 = _mm256_set1_epi32(0x00240013);
	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	//	const __m256i c16_p11_p30 = _mm256_set1_epi32(0x000B001E);
	const __m256i c16_p23_p43 = _mm256_set1_epi32(0x0017002B);
	const __m256i c16_n34_n07 = _mm256_set1_epi32(0xFFDEFFF9);
	const __m256i c16_n36_n45 = _mm256_set1_epi32(0xFFDCFFD3);
	const __m256i c16_p19_n11 = _mm256_set1_epi32(0x0013FFF5);
	//	const __m256i c16_p44_p41 = _mm256_set1_epi32(0x002C0029);
	//	const __m256i c16_n02_p27 = _mm256_set1_epi32(0xFFFE001B);
	//	const __m256i c16_n45_n30 = _mm256_set1_epi32(0xFFD3FFE2);
	//	const __m256i c16_n15_n39 = _mm256_set1_epi32(0xFFF1FFD9);
	const __m256i c16_p11_p41 = _mm256_set1_epi32(0x000B0029);
	const __m256i c16_n45_n27 = _mm256_set1_epi32(0xFFD3FFE5);
	const __m256i c16_p07_n30 = _mm256_set1_epi32(0x0007FFE2);
	const __m256i c16_p43_p39 = _mm256_set1_epi32(0x002B0027);
	//	const __m256i c16_n23_p15 = _mm256_set1_epi32(0xFFE9000F);
	//	const __m256i c16_n34_n45 = _mm256_set1_epi32(0xFFDEFFD3);
	//	const __m256i c16_p36_p02 = _mm256_set1_epi32(0x00240002);
	//	const __m256i c16_p19_p44 = _mm256_set1_epi32(0x0013002C);
	const __m256i c16_n02_p39 = _mm256_set1_epi32(0xFFFE0027);
	const __m256i c16_n36_n41 = _mm256_set1_epi32(0xFFDCFFD7);
	const __m256i c16_p43_p07 = _mm256_set1_epi32(0x002B0007);
	const __m256i c16_n11_p34 = _mm256_set1_epi32(0xFFF50022);
	//	const __m256i c16_n30_n44 = _mm256_set1_epi32(0xFFE2FFD4);
	//	const __m256i c16_p45_p15 = _mm256_set1_epi32(0x002D000F);
	//	const __m256i c16_n19_p27 = _mm256_set1_epi32(0xFFED001B);
	//	const __m256i c16_n23_n45 = _mm256_set1_epi32(0xFFE9FFD3);
	const __m256i c16_n15_p36 = _mm256_set1_epi32(0xFFF10024);
	const __m256i c16_n11_n45 = _mm256_set1_epi32(0xFFF5FFD3);
	const __m256i c16_p34_p39 = _mm256_set1_epi32(0x00220027);
	const __m256i c16_n45_n19 = _mm256_set1_epi32(0xFFD3FFED);
	//	const __m256i c16_p41_n07 = _mm256_set1_epi32(0x0029FFF9);
	//	const __m256i c16_n23_p30 = _mm256_set1_epi32(0xFFE9001E);
	//	const __m256i c16_n02_n44 = _mm256_set1_epi32(0xFFFEFFD4);
	//	const __m256i c16_p27_p43 = _mm256_set1_epi32(0x001B002B);
	const __m256i c16_n27_p34 = _mm256_set1_epi32(0xFFE50022);
	const __m256i c16_p19_n39 = _mm256_set1_epi32(0x0013FFD9);
	const __m256i c16_n11_p43 = _mm256_set1_epi32(0xFFF5002B);
	const __m256i c16_p02_n45 = _mm256_set1_epi32(0x0002FFD3);
	//	const __m256i c16_p07_p45 = _mm256_set1_epi32(0x0007002D);
	//	const __m256i c16_n15_n44 = _mm256_set1_epi32(0xFFF1FFD4);
	//	const __m256i c16_p23_p41 = _mm256_set1_epi32(0x00170029);
	//	const __m256i c16_n30_n36 = _mm256_set1_epi32(0xFFE2FFDC);
	const __m256i c16_n36_p30 = _mm256_set1_epi32(0xFFDC001E);
	const __m256i c16_p41_n23 = _mm256_set1_epi32(0x0029FFE9);
	const __m256i c16_n44_p15 = _mm256_set1_epi32(0xFFD4000F);
	const __m256i c16_p45_n07 = _mm256_set1_epi32(0x002DFFF9);
	//	const __m256i c16_n45_n02 = _mm256_set1_epi32(0xFFD3FFFE);
	//	const __m256i c16_p43_p11 = _mm256_set1_epi32(0x002B000B);
	//	const __m256i c16_n39_n19 = _mm256_set1_epi32(0xFFD9FFED);
	//	const __m256i c16_p34_p27 = _mm256_set1_epi32(0x0022001B);
	const __m256i c16_n43_p27 = _mm256_set1_epi32(0xFFD5001B);
	const __m256i c16_p44_n02 = _mm256_set1_epi32(0x002CFFFE);
	const __m256i c16_n30_n23 = _mm256_set1_epi32(0xFFE2FFE9);
	const __m256i c16_p07_p41 = _mm256_set1_epi32(0x00070029);
	//	const __m256i c16_p19_n45 = _mm256_set1_epi32(0x0013FFD3);
	//	const __m256i c16_n39_p34 = _mm256_set1_epi32(0xFFD90022);
	//	const __m256i c16_p45_n11 = _mm256_set1_epi32(0x002DFFF5);
	//	const __m256i c16_n36_n15 = _mm256_set1_epi32(0xFFDCFFF1);
	const __m256i c16_n45_p23 = _mm256_set1_epi32(0xFFD30017);
	const __m256i c16_p27_p19 = _mm256_set1_epi32(0x001B0013);
	const __m256i c16_p15_n45 = _mm256_set1_epi32(0x000FFFD3);
	const __m256i c16_n44_p30 = _mm256_set1_epi32(0xFFD4001E);
	//	const __m256i c16_p34_p11 = _mm256_set1_epi32(0x0022000B);
	//	const __m256i c16_p07_n43 = _mm256_set1_epi32(0x0007FFD5);
	//	const __m256i c16_n41_p36 = _mm256_set1_epi32(0xFFD70024);
	//	const __m256i c16_p39_p02 = _mm256_set1_epi32(0x00270002);
	const __m256i c16_n44_p19 = _mm256_set1_epi32(0xFFD40013);
	const __m256i c16_n02_p36 = _mm256_set1_epi32(0xFFFE0024);
	const __m256i c16_p45_n34 = _mm256_set1_epi32(0x002DFFDE);
	const __m256i c16_n15_n23 = _mm256_set1_epi32(0xFFF1FFE9);
	//	const __m256i c16_n39_p43 = _mm256_set1_epi32(0xFFD9002B);
	//	const __m256i c16_p30_p07 = _mm256_set1_epi32(0x001E0007);
	//	const __m256i c16_p27_n45 = _mm256_set1_epi32(0x001BFFD3);
	//	const __m256i c16_n41_p11 = _mm256_set1_epi32(0xFFD7000B);
	const __m256i c16_n39_p15 = _mm256_set1_epi32(0xFFD9000F);
	const __m256i c16_n30_p45 = _mm256_set1_epi32(0xFFE2002D);
	const __m256i c16_p27_p02 = _mm256_set1_epi32(0x001B0002);
	const __m256i c16_p41_n44 = _mm256_set1_epi32(0x0029FFD4);
	//	const __m256i c16_n11_n19 = _mm256_set1_epi32(0xFFF5FFED);
	//	const __m256i c16_n45_p36 = _mm256_set1_epi32(0xFFD30024);
	//	const __m256i c16_n07_p34 = _mm256_set1_epi32(0xFFF90022);
	//	const __m256i c16_p43_n23 = _mm256_set1_epi32(0x002BFFE9);
	const __m256i c16_n30_p11 = _mm256_set1_epi32(0xFFE2000B);
	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);
	const __m256i c16_n19_p36 = _mm256_set1_epi32(0xFFED0024);
	const __m256i c16_p23_n02 = _mm256_set1_epi32(0x0017FFFE);
	//	const __m256i c16_p45_n39 = _mm256_set1_epi32(0x002DFFD9);
	//	const __m256i c16_p27_n41 = _mm256_set1_epi32(0x001BFFD7);
	//	const __m256i c16_n15_n07 = _mm256_set1_epi32(0xFFF1FFF9);
	//	const __m256i c16_n44_p34 = _mm256_set1_epi32(0xFFD40022);
	const __m256i c16_n19_p07 = _mm256_set1_epi32(0xFFED0007);
	const __m256i c16_n39_p30 = _mm256_set1_epi32(0xFFD9001E);
	const __m256i c16_n45_p44 = _mm256_set1_epi32(0xFFD3002C);
	const __m256i c16_n36_p43 = _mm256_set1_epi32(0xFFDC002B);
	//	const __m256i c16_n15_p27 = _mm256_set1_epi32(0xFFF1001B);
	//	const __m256i c16_p11_p02 = _mm256_set1_epi32(0x000B0002);
	//	const __m256i c16_p34_n23 = _mm256_set1_epi32(0x0022FFE9);
	//	const __m256i c16_p45_n41 = _mm256_set1_epi32(0x002DFFD7);
	const __m256i c16_n07_p02 = _mm256_set1_epi32(0xFFF90002);
	const __m256i c16_n15_p11 = _mm256_set1_epi32(0xFFF1000B);
	const __m256i c16_n23_p19 = _mm256_set1_epi32(0xFFE90013);
	const __m256i c16_n30_p27 = _mm256_set1_epi32(0xFFE2001B);
	//	const __m256i c16_n36_p34 = _mm256_set1_epi32(0xFFDC0022);
	//	const __m256i c16_n41_p39 = _mm256_set1_epi32(0xFFD70027);
	//	const __m256i c16_n44_p43 = _mm256_set1_epi32(0xFFD4002B);
	//	const __m256i c16_n45_p45 = _mm256_set1_epi32(0xFFD3002D);

	//	const __m256i c16_p43_p45 = _mm256_set1_epi32(0x002B002D);
	const __m256i c16_p35_p40 = _mm256_set1_epi32(0x00230028);
	//	const __m256i c16_p21_p29 = _mm256_set1_epi32(0x0015001D);
	//	const __m256i c16_p04_p13 = _mm256_set1_epi32(0x0004000D);
	const __m256i c16_p29_p43 = _mm256_set1_epi32(0x001D002B);
	const __m256i c16_n21_p04 = _mm256_set1_epi32(0xFFEB0004);
	//	const __m256i c16_n45_n40 = _mm256_set1_epi32(0xFFD3FFD8);
	//	const __m256i c16_n13_n35 = _mm256_set1_epi32(0xFFF3FFDD);
	const __m256i c16_p04_p40 = _mm256_set1_epi32(0x00040028);
	const __m256i c16_n43_n35 = _mm256_set1_epi32(0xFFD5FFDD);
	//	const __m256i c16_p29_n13 = _mm256_set1_epi32(0x001DFFF3);
	//	const __m256i c16_p21_p45 = _mm256_set1_epi32(0x0015002D);
	const __m256i c16_n21_p35 = _mm256_set1_epi32(0xFFEB0023);
	const __m256i c16_p04_n43 = _mm256_set1_epi32(0x0004FFD5);
	//	const __m256i c16_p13_p45 = _mm256_set1_epi32(0x000D002D);
	//	const __m256i c16_n29_n40 = _mm256_set1_epi32(0xFFE3FFD8);
	const __m256i c16_n40_p29 = _mm256_set1_epi32(0xFFD8001D);
	const __m256i c16_p45_n13 = _mm256_set1_epi32(0x002DFFF3);
	//	const __m256i c16_n43_n04 = _mm256_set1_epi32(0xFFD5FFFC);
	//	const __m256i c16_p35_p21 = _mm256_set1_epi32(0x00230015);
	const __m256i c16_n45_p21 = _mm256_set1_epi32(0xFFD30015);
	const __m256i c16_p13_p29 = _mm256_set1_epi32(0x000D001D);
	//	const __m256i c16_p35_n43 = _mm256_set1_epi32(0x0023FFD5);
	//	const __m256i c16_n40_p04 = _mm256_set1_epi32(0xFFD80004);
	const __m256i c16_n35_p13 = _mm256_set1_epi32(0xFFDD000D);
	const __m256i c16_n40_p45 = _mm256_set1_epi32(0xFFD8002D);
	//	const __m256i c16_p04_p21 = _mm256_set1_epi32(0x00040015);
	//	const __m256i c16_p43_n29 = _mm256_set1_epi32(0x002BFFE3);
	const __m256i c16_n13_p04 = _mm256_set1_epi32(0xFFF30004);
	const __m256i c16_n29_p21 = _mm256_set1_epi32(0xFFE30015);
	//	const __m256i c16_n40_p35 = _mm256_set1_epi32(0xFFD80023);
	//	const __m256i c16_n45_p43 = _mm256_set1_epi32(0xFFD3002B);

	const __m256i c16_p38_p44 = _mm256_set1_epi32(0x0026002C);
	//	const __m256i c16_p09_p25 = _mm256_set1_epi32(0x00090019);
	const __m256i c16_n09_p38 = _mm256_set1_epi32(0xFFF70026);
	//	const __m256i c16_n25_n44 = _mm256_set1_epi32(0xFFE7FFD4);

	const __m256i c16_n44_p25 = _mm256_set1_epi32(0xFFD40019);
	//	const __m256i c16_p38_p09 = _mm256_set1_epi32(0x00260009);
	const __m256i c16_n25_p09 = _mm256_set1_epi32(0xFFE70009);
	//	const __m256i c16_n44_p38 = _mm256_set1_epi32(0xFFD40026);

	const __m256i c16_p17_p42 = _mm256_set1_epi32(0x0011002A);
	const __m256i c16_n42_p17 = _mm256_set1_epi32(0xFFD60011);

	const __m256i c16_p32_p32 = _mm256_set1_epi32(0x00200020);
	const __m256i c16_n32_p32 = _mm256_set1_epi32(0xFFE00020);

	__m256i zero = _mm256_setzero_si256();
	__m256i c32_rnd = _mm256_set1_epi32(16);
	int nShift = 5;

	// DCT1
	__m256i in00[2], in01[2], in02[2], in03[2], in04[2], in05[2], in06[2], in07[2], in08[2], in09[2], in10[2], in11[2], in12[2], in13[2], in14[2], in15[2];
	__m256i in16[2], in17[2], in18[2], in19[2], in20[2], in21[2], in22[2], in23[2], in24[2], in25[2], in26[2], in27[2], in28[2], in29[2], in30[2], in31[2];
	__m256i res00[2], res01[2], res02[2], res03[2], res04[2], res05[2], res06[2], res07[2], res08[2], res09[2], res10[2], res11[2], res12[2], res13[2], res14[2], res15[2];
	__m256i res16[2], res17[2], res18[2], res19[2], res20[2], res21[2], res22[2], res23[2], res24[2], res25[2], res26[2], res27[2], res28[2], res29[2], res30[2], res31[2];

	int part;


	in00[0] = _mm256_lddqu_si256((const __m256i*)&src[0 * 32]);
	in01[0] = _mm256_lddqu_si256((const __m256i*)&src[1 * 32]);
	in02[0] = _mm256_lddqu_si256((const __m256i*)&src[2 * 32]);
	in03[0] = _mm256_lddqu_si256((const __m256i*)&src[3 * 32]);
	in04[0] = _mm256_lddqu_si256((const __m256i*)&src[4 * 32]);
	in05[0] = _mm256_lddqu_si256((const __m256i*)&src[5 * 32]);
	in06[0] = _mm256_lddqu_si256((const __m256i*)&src[6 * 32]);
	in07[0] = _mm256_lddqu_si256((const __m256i*)&src[7 * 32]);
	in08[0] = _mm256_lddqu_si256((const __m256i*)&src[8 * 32]);
	in09[0] = _mm256_lddqu_si256((const __m256i*)&src[9 * 32]);
	in10[0] = _mm256_lddqu_si256((const __m256i*)&src[10 * 32]);
	in11[0] = _mm256_lddqu_si256((const __m256i*)&src[11 * 32]);
	in12[0] = _mm256_lddqu_si256((const __m256i*)&src[12 * 32]);
	in13[0] = _mm256_lddqu_si256((const __m256i*)&src[13 * 32]);
	in14[0] = _mm256_lddqu_si256((const __m256i*)&src[14 * 32]);
	in15[0] = _mm256_lddqu_si256((const __m256i*)&src[15 * 32]);


	// First DCT
	{
		const __m256i T_00_00A = _mm256_unpacklo_epi16(in01[0], in03[0]);       // [33 13 32 12 31 11 30 10]
		const __m256i T_00_00B = _mm256_unpackhi_epi16(in01[0], in03[0]);       // [37 17 36 16 35 15 34 14]
		const __m256i T_00_01A = _mm256_unpacklo_epi16(in05[0], in07[0]);       // [ ]
		const __m256i T_00_01B = _mm256_unpackhi_epi16(in05[0], in07[0]);       // [ ]
		const __m256i T_00_02A = _mm256_unpacklo_epi16(in09[0], in11[0]);       // [ ]
		const __m256i T_00_02B = _mm256_unpackhi_epi16(in09[0], in11[0]);       // [ ]
		const __m256i T_00_03A = _mm256_unpacklo_epi16(in13[0], in15[0]);       // [ ]
		const __m256i T_00_03B = _mm256_unpackhi_epi16(in13[0], in15[0]);       // [ ]

		const __m256i T_00_08A = _mm256_unpacklo_epi16(in02[0], in06[0]);       // [ ]
		const __m256i T_00_08B = _mm256_unpackhi_epi16(in02[0], in06[0]);       // [ ]
		const __m256i T_00_09A = _mm256_unpacklo_epi16(in10[0], in14[0]);       // [ ]
		const __m256i T_00_09B = _mm256_unpackhi_epi16(in10[0], in14[0]);       // [ ]

		const __m256i T_00_12A = _mm256_unpacklo_epi16(in04[0], in12[0]);       // [ ]
		const __m256i T_00_12B = _mm256_unpackhi_epi16(in04[0], in12[0]);       // [ ]

		const __m256i T_00_14A = _mm256_unpacklo_epi16(in08[0], zero);       //
		const __m256i T_00_14B = _mm256_unpackhi_epi16(in08[0], zero);       // [ ]
		const __m256i T_00_15A = _mm256_unpacklo_epi16(in00[0], zero);       //
		const __m256i T_00_15B = _mm256_unpackhi_epi16(in00[0], zero);       // [ ]

		__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
		__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
		__m256i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
		__m256i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
		{
			__m256i T00, T01;
#define COMPUTE_ROW_PARTIAL(r0103, r0507, r0911, r1315, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(r0103, c0103), _mm256_madd_epi16(r0507, c0507)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(r0911, c0911), _mm256_madd_epi16(r1315, c1315)); \
	row = _mm256_add_epi32(_mm256_add_epi32(T00, T01), zero);

			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A);

			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B);

#undef COMPUTE_ROW_PARTIAL
		}


		{
			__m256i T00;
#define COMPUTE_ROW_PARTIAL(row0206, row1014, c0206, c1014, c1822, c2630, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(row0206, c0206), _mm256_madd_epi16(row1014, c1014)); \
	row = _mm256_add_epi32(T00, zero);

			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A);

			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B);
#undef COMPUTE_ROW_PARTIAL
		}

		EEO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_p38_p44), zero);
		EEO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n09_p38), zero);
		EEO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n44_p25), zero);
		EEO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n25_p09), zero);
		EEO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_p38_p44), zero);
		EEO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n09_p38), zero);
		EEO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n44_p25), zero);
		EEO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n25_p09), zero);

		EEEO0A = _mm256_madd_epi16(T_00_14A, c16_p17_p42);
		EEEO0B = _mm256_madd_epi16(T_00_14B, c16_p17_p42);
		EEEO1A = _mm256_madd_epi16(T_00_14A, c16_n42_p17);
		EEEO1B = _mm256_madd_epi16(T_00_14B, c16_n42_p17);

		EEEE0A = _mm256_madd_epi16(T_00_15A, c16_p32_p32);
		EEEE0B = _mm256_madd_epi16(T_00_15B, c16_p32_p32);
		EEEE1A = _mm256_madd_epi16(T_00_15A, c16_n32_p32);
		EEEE1B = _mm256_madd_epi16(T_00_15B, c16_n32_p32);

		EEE0A = _mm256_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
		EEE0B = _mm256_add_epi32(EEEE0B, EEEO0B);
		EEE1A = _mm256_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
		EEE1B = _mm256_add_epi32(EEEE1B, EEEO1B);
		EEE3A = _mm256_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
		EEE3B = _mm256_sub_epi32(EEEE0B, EEEO0B);
		EEE2A = _mm256_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
		EEE2B = _mm256_sub_epi32(EEEE1B, EEEO1B);

		EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
		EE0B = _mm256_add_epi32(EEE0B, EEO0B);
		EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
		EE1B = _mm256_add_epi32(EEE1B, EEO1B);
		EE2A = _mm256_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
		EE2B = _mm256_add_epi32(EEE2B, EEO2B);
		EE3A = _mm256_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
		EE3B = _mm256_add_epi32(EEE3B, EEO3B);
		EE7A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
		EE7B = _mm256_sub_epi32(EEE0B, EEO0B);
		EE6A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
		EE6B = _mm256_sub_epi32(EEE1B, EEO1B);
		EE5A = _mm256_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
		EE5B = _mm256_sub_epi32(EEE2B, EEO2B);
		EE4A = _mm256_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
		EE4B = _mm256_sub_epi32(EEE3B, EEO3B);

		E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
		E0B = _mm256_add_epi32(EE0B, EO0B);
		E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
		E1B = _mm256_add_epi32(EE1B, EO1B);
		E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
		E2B = _mm256_add_epi32(EE2B, EO2B);
		E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
		E3B = _mm256_add_epi32(EE3B, EO3B);
		E4A = _mm256_add_epi32(EE4A, EO4A);          // E4 =
		E4B = _mm256_add_epi32(EE4B, EO4B);
		E5A = _mm256_add_epi32(EE5A, EO5A);          // E5 =
		E5B = _mm256_add_epi32(EE5B, EO5B);
		E6A = _mm256_add_epi32(EE6A, EO6A);          // E6 =
		E6B = _mm256_add_epi32(EE6B, EO6B);
		E7A = _mm256_add_epi32(EE7A, EO7A);          // E7 =
		E7B = _mm256_add_epi32(EE7B, EO7B);
		EFA = _mm256_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
		EFB = _mm256_sub_epi32(EE0B, EO0B);
		EEA = _mm256_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
		EEB = _mm256_sub_epi32(EE1B, EO1B);
		EDA = _mm256_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
		EDB = _mm256_sub_epi32(EE2B, EO2B);
		ECA = _mm256_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
		ECB = _mm256_sub_epi32(EE3B, EO3B);
		EBA = _mm256_sub_epi32(EE4A, EO4A);          // EB =
		EBB = _mm256_sub_epi32(EE4B, EO4B);
		EAA = _mm256_sub_epi32(EE5A, EO5A);          // EA =
		EAB = _mm256_sub_epi32(EE5B, EO5B);
		E9A = _mm256_sub_epi32(EE6A, EO6A);          // E9 =
		E9B = _mm256_sub_epi32(EE6B, EO6B);
		E8A = _mm256_sub_epi32(EE7A, EO7A);          // E8 =
		E8B = _mm256_sub_epi32(EE7B, EO7B);

		T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
		T10B = _mm256_add_epi32(E0B, c32_rnd);
		T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
		T11B = _mm256_add_epi32(E1B, c32_rnd);
		T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
		T12B = _mm256_add_epi32(E2B, c32_rnd);
		T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
		T13B = _mm256_add_epi32(E3B, c32_rnd);
		T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
		T14B = _mm256_add_epi32(E4B, c32_rnd);
		T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
		T15B = _mm256_add_epi32(E5B, c32_rnd);
		T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
		T16B = _mm256_add_epi32(E6B, c32_rnd);
		T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
		T17B = _mm256_add_epi32(E7B, c32_rnd);
		T18A = _mm256_add_epi32(E8A, c32_rnd);         // E8 + rnd
		T18B = _mm256_add_epi32(E8B, c32_rnd);
		T19A = _mm256_add_epi32(E9A, c32_rnd);         // E9 + rnd
		T19B = _mm256_add_epi32(E9B, c32_rnd);
		T1AA = _mm256_add_epi32(EAA, c32_rnd);         // E10 + rnd
		T1AB = _mm256_add_epi32(EAB, c32_rnd);
		T1BA = _mm256_add_epi32(EBA, c32_rnd);         // E11 + rnd
		T1BB = _mm256_add_epi32(EBB, c32_rnd);
		T1CA = _mm256_add_epi32(ECA, c32_rnd);         // E12 + rnd
		T1CB = _mm256_add_epi32(ECB, c32_rnd);
		T1DA = _mm256_add_epi32(EDA, c32_rnd);         // E13 + rnd
		T1DB = _mm256_add_epi32(EDB, c32_rnd);
		T1EA = _mm256_add_epi32(EEA, c32_rnd);         // E14 + rnd
		T1EB = _mm256_add_epi32(EEB, c32_rnd);
		T1FA = _mm256_add_epi32(EFA, c32_rnd);         // E15 + rnd
		T1FB = _mm256_add_epi32(EFB, c32_rnd);

		T2_00A = _mm256_add_epi32(T10A, O00A);          // E0 + O0 + rnd
		T2_00B = _mm256_add_epi32(T10B, O00B);
		T2_01A = _mm256_add_epi32(T11A, O01A);          // E1 + O1 + rnd
		T2_01B = _mm256_add_epi32(T11B, O01B);
		T2_02A = _mm256_add_epi32(T12A, O02A);          // E2 + O2 + rnd
		T2_02B = _mm256_add_epi32(T12B, O02B);
		T2_03A = _mm256_add_epi32(T13A, O03A);          // E3 + O3 + rnd
		T2_03B = _mm256_add_epi32(T13B, O03B);
		T2_04A = _mm256_add_epi32(T14A, O04A);          // E4
		T2_04B = _mm256_add_epi32(T14B, O04B);
		T2_05A = _mm256_add_epi32(T15A, O05A);          // E5
		T2_05B = _mm256_add_epi32(T15B, O05B);
		T2_06A = _mm256_add_epi32(T16A, O06A);          // E6
		T2_06B = _mm256_add_epi32(T16B, O06B);
		T2_07A = _mm256_add_epi32(T17A, O07A);          // E7
		T2_07B = _mm256_add_epi32(T17B, O07B);
		T2_08A = _mm256_add_epi32(T18A, O08A);          // E8
		T2_08B = _mm256_add_epi32(T18B, O08B);
		T2_09A = _mm256_add_epi32(T19A, O09A);          // E9
		T2_09B = _mm256_add_epi32(T19B, O09B);
		T2_10A = _mm256_add_epi32(T1AA, O10A);          // E10
		T2_10B = _mm256_add_epi32(T1AB, O10B);
		T2_11A = _mm256_add_epi32(T1BA, O11A);          // E11
		T2_11B = _mm256_add_epi32(T1BB, O11B);
		T2_12A = _mm256_add_epi32(T1CA, O12A);          // E12
		T2_12B = _mm256_add_epi32(T1CB, O12B);
		T2_13A = _mm256_add_epi32(T1DA, O13A);          // E13
		T2_13B = _mm256_add_epi32(T1DB, O13B);
		T2_14A = _mm256_add_epi32(T1EA, O14A);          // E14
		T2_14B = _mm256_add_epi32(T1EB, O14B);
		T2_15A = _mm256_add_epi32(T1FA, O15A);          // E15
		T2_15B = _mm256_add_epi32(T1FB, O15B);
		T2_31A = _mm256_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
		T2_31B = _mm256_sub_epi32(T10B, O00B);
		T2_30A = _mm256_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
		T2_30B = _mm256_sub_epi32(T11B, O01B);
		T2_29A = _mm256_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
		T2_29B = _mm256_sub_epi32(T12B, O02B);
		T2_28A = _mm256_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
		T2_28B = _mm256_sub_epi32(T13B, O03B);
		T2_27A = _mm256_sub_epi32(T14A, O04A);          // E4
		T2_27B = _mm256_sub_epi32(T14B, O04B);
		T2_26A = _mm256_sub_epi32(T15A, O05A);          // E5
		T2_26B = _mm256_sub_epi32(T15B, O05B);
		T2_25A = _mm256_sub_epi32(T16A, O06A);          // E6
		T2_25B = _mm256_sub_epi32(T16B, O06B);
		T2_24A = _mm256_sub_epi32(T17A, O07A);          // E7
		T2_24B = _mm256_sub_epi32(T17B, O07B);
		T2_23A = _mm256_sub_epi32(T18A, O08A);          //
		T2_23B = _mm256_sub_epi32(T18B, O08B);
		T2_22A = _mm256_sub_epi32(T19A, O09A);          //
		T2_22B = _mm256_sub_epi32(T19B, O09B);
		T2_21A = _mm256_sub_epi32(T1AA, O10A);          //
		T2_21B = _mm256_sub_epi32(T1AB, O10B);
		T2_20A = _mm256_sub_epi32(T1BA, O11A);          //
		T2_20B = _mm256_sub_epi32(T1BB, O11B);
		T2_19A = _mm256_sub_epi32(T1CA, O12A);          //
		T2_19B = _mm256_sub_epi32(T1CB, O12B);
		T2_18A = _mm256_sub_epi32(T1DA, O13A);          //
		T2_18B = _mm256_sub_epi32(T1DB, O13B);
		T2_17A = _mm256_sub_epi32(T1EA, O14A);          //
		T2_17B = _mm256_sub_epi32(T1EB, O14B);
		T2_16A = _mm256_sub_epi32(T1FA, O15A);          //
		T2_16B = _mm256_sub_epi32(T1FB, O15B);

		T3_00A = _mm256_srai_epi32(T2_00A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_00B = _mm256_srai_epi32(T2_00B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_01A = _mm256_srai_epi32(T2_01A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_01B = _mm256_srai_epi32(T2_01B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
		T3_02A = _mm256_srai_epi32(T2_02A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
		T3_02B = _mm256_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
		T3_03A = _mm256_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
		T3_03B = _mm256_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
		T3_04A = _mm256_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
		T3_04B = _mm256_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
		T3_05A = _mm256_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
		T3_05B = _mm256_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
		T3_06A = _mm256_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
		T3_06B = _mm256_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
		T3_07A = _mm256_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
		T3_07B = _mm256_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
		T3_08A = _mm256_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
		T3_08B = _mm256_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
		T3_09A = _mm256_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
		T3_09B = _mm256_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
		T3_10A = _mm256_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
		T3_10B = _mm256_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
		T3_11A = _mm256_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
		T3_11B = _mm256_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
		T3_12A = _mm256_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
		T3_12B = _mm256_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
		T3_13A = _mm256_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
		T3_13B = _mm256_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
		T3_14A = _mm256_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
		T3_14B = _mm256_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
		T3_15A = _mm256_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
		T3_15B = _mm256_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

		T3_16A = _mm256_srai_epi32(T2_16A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_16B = _mm256_srai_epi32(T2_16B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_17A = _mm256_srai_epi32(T2_17A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_17B = _mm256_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
		T3_18A = _mm256_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
		T3_18B = _mm256_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
		T3_19A = _mm256_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
		T3_19B = _mm256_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
		T3_20A = _mm256_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
		T3_20B = _mm256_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
		T3_21A = _mm256_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
		T3_21B = _mm256_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
		T3_22A = _mm256_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
		T3_22B = _mm256_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
		T3_23A = _mm256_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
		T3_23B = _mm256_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
		T3_24A = _mm256_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
		T3_24B = _mm256_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
		T3_25A = _mm256_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
		T3_25B = _mm256_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
		T3_26A = _mm256_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
		T3_26B = _mm256_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
		T3_27A = _mm256_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
		T3_27B = _mm256_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
		T3_28A = _mm256_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
		T3_28B = _mm256_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
		T3_29A = _mm256_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
		T3_29B = _mm256_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
		T3_30A = _mm256_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
		T3_30B = _mm256_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
		T3_31A = _mm256_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
		T3_31B = _mm256_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

		res00[0] = _mm256_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
		res01[0] = _mm256_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
		res02[0] = _mm256_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
		res03[0] = _mm256_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
		res04[0] = _mm256_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
		res05[0] = _mm256_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
		res06[0] = _mm256_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
		res07[0] = _mm256_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
		res08[0] = _mm256_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
		res09[0] = _mm256_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
		res10[0] = _mm256_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
		res11[0] = _mm256_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
		res12[0] = _mm256_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
		res13[0] = _mm256_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
		res14[0] = _mm256_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
		res15[0] = _mm256_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
		res16[0] = _mm256_packs_epi32(T3_16A, T3_16B);
		res17[0] = _mm256_packs_epi32(T3_17A, T3_17B);
		res18[0] = _mm256_packs_epi32(T3_18A, T3_18B);
		res19[0] = _mm256_packs_epi32(T3_19A, T3_19B);
		res20[0] = _mm256_packs_epi32(T3_20A, T3_20B);
		res21[0] = _mm256_packs_epi32(T3_21A, T3_21B);
		res22[0] = _mm256_packs_epi32(T3_22A, T3_22B);
		res23[0] = _mm256_packs_epi32(T3_23A, T3_23B);
		res24[0] = _mm256_packs_epi32(T3_24A, T3_24B);
		res25[0] = _mm256_packs_epi32(T3_25A, T3_25B);
		res26[0] = _mm256_packs_epi32(T3_26A, T3_26B);
		res27[0] = _mm256_packs_epi32(T3_27A, T3_27B);
		res28[0] = _mm256_packs_epi32(T3_28A, T3_28B);
		res29[0] = _mm256_packs_epi32(T3_29A, T3_29B);
		res30[0] = _mm256_packs_epi32(T3_30A, T3_30B);
		res31[0] = _mm256_packs_epi32(T3_31A, T3_31B);

	}


	//transpose 32x32 matrix
	{
		__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
	tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
	tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
	tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
	tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
	tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
	tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
	tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
	tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
	O0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	O1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	O2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	O3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	O4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	O5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	O6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	O7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	O8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
	O9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
	O10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
	O11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
	O12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
	O13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
	O14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
	O15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
	tr0_0 = _mm256_unpacklo_epi64(O0, O1); \
	tr0_1 = _mm256_unpacklo_epi64(O2, O3); \
	tr0_2 = _mm256_unpackhi_epi64(O0, O1); \
	tr0_3 = _mm256_unpackhi_epi64(O2, O3); \
	tr0_4 = _mm256_unpacklo_epi64(O4, O5); \
	tr0_5 = _mm256_unpacklo_epi64(O6, O7); \
	tr0_6 = _mm256_unpackhi_epi64(O4, O5); \
	tr0_7 = _mm256_unpackhi_epi64(O6, O7); \
	tr0_8 = _mm256_unpacklo_epi64(O8, O9); \
	tr0_9 = _mm256_unpacklo_epi64(O10, O11); \
	tr0_10 = _mm256_unpackhi_epi64(O8, O9); \
	tr0_11 = _mm256_unpackhi_epi64(O10, O11); \
	tr0_12 = _mm256_unpacklo_epi64(O12, O13); \
	tr0_13 = _mm256_unpacklo_epi64(O14, O15); \
	tr0_14 = _mm256_unpackhi_epi64(O12, O13); \
	tr0_15 = _mm256_unpackhi_epi64(O14, O15); \
	O0 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x20); \
	O1 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x20); \
	O2 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x20); \
	O3 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x20); \
	O4 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x20); \
	O5 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x20); \
	O6 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x20); \
	O7 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x20); \
	O8 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x31); \
	O9 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x31); \
	O10 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x31); \
	O11 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x31); \
	O12 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x31); \
	O13 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x31); \
	O14 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x31); \
	O15 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x31); \

		TRANSPOSE_16x16_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
		TRANSPOSE_16x16_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);

#undef TRANSPOSE_16x16_16BIT
	}


	// Second IDCT
	c32_rnd = _mm256_set1_epi32(1024);				// pass == 1 第二次四舍五入
	nShift = 11;


	for (part = 0; part < 2; part++)
	{
		const __m256i T_00_00A = _mm256_unpacklo_epi16(in01[part], in03[part]);       // [33 13 32 12 31 11 30 10]
		const __m256i T_00_00B = _mm256_unpackhi_epi16(in01[part], in03[part]);       // [37 17 36 16 35 15 34 14]
		const __m256i T_00_01A = _mm256_unpacklo_epi16(in05[part], in07[part]);       // [ ]
		const __m256i T_00_01B = _mm256_unpackhi_epi16(in05[part], in07[part]);       // [ ]
		const __m256i T_00_02A = _mm256_unpacklo_epi16(in09[part], in11[part]);       // [ ]
		const __m256i T_00_02B = _mm256_unpackhi_epi16(in09[part], in11[part]);       // [ ]
		const __m256i T_00_03A = _mm256_unpacklo_epi16(in13[part], in15[part]);       // [ ]
		const __m256i T_00_03B = _mm256_unpackhi_epi16(in13[part], in15[part]);       // [ ]

		const __m256i T_00_08A = _mm256_unpacklo_epi16(in02[part], in06[part]);       // [ ]
		const __m256i T_00_08B = _mm256_unpackhi_epi16(in02[part], in06[part]);       // [ ]
		const __m256i T_00_09A = _mm256_unpacklo_epi16(in10[part], in14[part]);       // [ ]
		const __m256i T_00_09B = _mm256_unpackhi_epi16(in10[part], in14[part]);       // [ ]

		const __m256i T_00_12A = _mm256_unpacklo_epi16(in04[part], in12[part]);       // [ ]
		const __m256i T_00_12B = _mm256_unpackhi_epi16(in04[part], in12[part]);       // [ ]

		const __m256i T_00_14A = _mm256_unpacklo_epi16(in08[part], zero);       //
		const __m256i T_00_14B = _mm256_unpackhi_epi16(in08[part], zero);       // [ ]
		const __m256i T_00_15A = _mm256_unpacklo_epi16(in00[part], zero);       //
		const __m256i T_00_15B = _mm256_unpackhi_epi16(in00[part], zero);       // [ ]

		__m256i O00A, O01A, O02A, O03A, O04A, O05A, O06A, O07A, O08A, O09A, O10A, O11A, O12A, O13A, O14A, O15A;
		__m256i O00B, O01B, O02B, O03B, O04B, O05B, O06B, O07B, O08B, O09B, O10B, O11B, O12B, O13B, O14B, O15B;
		__m256i EO0A, EO1A, EO2A, EO3A, EO4A, EO5A, EO6A, EO7A;
		__m256i EO0B, EO1B, EO2B, EO3B, EO4B, EO5B, EO6B, EO7B;
		{
			__m256i T00, T01;
#define COMPUTE_ROW_PARTIAL(r0103, r0507, r0911, r1315, c0103, c0507, c0911, c1315, c1719, c2123, c2527, c2931, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(r0103, c0103), _mm256_madd_epi16(r0507, c0507)); \
	T01 = _mm256_add_epi32(_mm256_madd_epi16(r0911, c0911), _mm256_madd_epi16(r1315, c1315)); \
	row = _mm256_add_epi32(_mm256_add_epi32(T00, T01), zero);

			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14A);
			COMPUTE_ROW_PARTIAL(T_00_00A, T_00_01A, T_00_02A, T_00_03A, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15A);

			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p45_p45, c16_p43_p44, c16_p39_p41, c16_p34_p36, c16_p27_p30, c16_p19_p23, c16_p11_p15, c16_p02_p07, O00B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p41_p45, c16_p23_p34, c16_n02_p11, c16_n27_n15, c16_n43_n36, c16_n44_n45, c16_n30_n39, c16_n07_n19, O01B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p34_p44, c16_n07_p15, c16_n41_n27, c16_n39_n45, c16_n02_n23, c16_p36_p19, c16_p43_p45, c16_p11_p30, O02B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p23_p43, c16_n34_n07, c16_n36_n45, c16_p19_n11, c16_p44_p41, c16_n02_p27, c16_n45_n30, c16_n15_n39, O03B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_p11_p41, c16_n45_n27, c16_p07_n30, c16_p43_p39, c16_n23_p15, c16_n34_n45, c16_p36_p02, c16_p19_p44, O04B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n02_p39, c16_n36_n41, c16_p43_p07, c16_n11_p34, c16_n30_n44, c16_p45_p15, c16_n19_p27, c16_n23_n45, O05B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n15_p36, c16_n11_n45, c16_p34_p39, c16_n45_n19, c16_p41_n07, c16_n23_p30, c16_n02_n44, c16_p27_p43, O06B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n27_p34, c16_p19_n39, c16_n11_p43, c16_p02_n45, c16_p07_p45, c16_n15_n44, c16_p23_p41, c16_n30_n36, O07B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n36_p30, c16_p41_n23, c16_n44_p15, c16_p45_n07, c16_n45_n02, c16_p43_p11, c16_n39_n19, c16_p34_p27, O08B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n43_p27, c16_p44_n02, c16_n30_n23, c16_p07_p41, c16_p19_n45, c16_n39_p34, c16_p45_n11, c16_n36_n15, O09B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n45_p23, c16_p27_p19, c16_p15_n45, c16_n44_p30, c16_p34_p11, c16_p07_n43, c16_n41_p36, c16_p39_p02, O10B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n44_p19, c16_n02_p36, c16_p45_n34, c16_n15_n23, c16_n39_p43, c16_p30_p07, c16_p27_n45, c16_n41_p11, O11B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n39_p15, c16_n30_p45, c16_p27_p02, c16_p41_n44, c16_n11_n19, c16_n45_p36, c16_n07_p34, c16_p43_n23, O12B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n30_p11, c16_n45_p43, c16_n19_p36, c16_p23_n02, c16_p45_n39, c16_p27_n41, c16_n15_n07, c16_n44_p34, O13B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n19_p07, c16_n39_p30, c16_n45_p44, c16_n36_p43, c16_n15_p27, c16_p11_p02, c16_p34_n23, c16_p45_n41, O14B);
			COMPUTE_ROW_PARTIAL(T_00_00B, T_00_01B, T_00_02B, T_00_03B, \
				c16_n07_p02, c16_n15_p11, c16_n23_p19, c16_n30_p27, c16_n36_p34, c16_n41_p39, c16_n44_p43, c16_n45_p45, O15B);

#undef COMPUTE_ROW_PARTIAL
		}


		{
			__m256i T00;
#define COMPUTE_ROW_PARTIAL(row0206, row1014, c0206, c1014, c1822, c2630, row) \
	T00 = _mm256_add_epi32(_mm256_madd_epi16(row0206, c0206), _mm256_madd_epi16(row1014, c1014)); \
	row = _mm256_add_epi32(T00, zero);

			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6A);
			COMPUTE_ROW_PARTIAL(T_00_08A, T_00_09A, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7A);

			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p43_p45, c16_p35_p40, c16_p21_p29, c16_p04_p13, EO0B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p29_p43, c16_n21_p04, c16_n45_n40, c16_n13_n35, EO1B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_p04_p40, c16_n43_n35, c16_p29_n13, c16_p21_p45, EO2B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n21_p35, c16_p04_n43, c16_p13_p45, c16_n29_n40, EO3B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n40_p29, c16_p45_n13, c16_n43_n04, c16_p35_p21, EO4B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n45_p21, c16_p13_p29, c16_p35_n43, c16_n40_p04, EO5B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n35_p13, c16_n40_p45, c16_p04_p21, c16_p43_n29, EO6B);
			COMPUTE_ROW_PARTIAL(T_00_08B, T_00_09B, c16_n13_p04, c16_n29_p21, c16_n40_p35, c16_n45_p43, EO7B);
#undef COMPUTE_ROW_PARTIAL
		}

		EEO0A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_p38_p44), zero);
		EEO1A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n09_p38), zero);
		EEO2A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n44_p25), zero);
		EEO3A = _mm256_add_epi32(_mm256_madd_epi16(T_00_12A, c16_n25_p09), zero);
		EEO0B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_p38_p44), zero);
		EEO1B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n09_p38), zero);
		EEO2B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n44_p25), zero);
		EEO3B = _mm256_add_epi32(_mm256_madd_epi16(T_00_12B, c16_n25_p09), zero);

		EEEO0A = _mm256_madd_epi16(T_00_14A, c16_p17_p42);
		EEEO0B = _mm256_madd_epi16(T_00_14B, c16_p17_p42);
		EEEO1A = _mm256_madd_epi16(T_00_14A, c16_n42_p17);
		EEEO1B = _mm256_madd_epi16(T_00_14B, c16_n42_p17);

		EEEE0A = _mm256_madd_epi16(T_00_15A, c16_p32_p32);
		EEEE0B = _mm256_madd_epi16(T_00_15B, c16_p32_p32);
		EEEE1A = _mm256_madd_epi16(T_00_15A, c16_n32_p32);
		EEEE1B = _mm256_madd_epi16(T_00_15B, c16_n32_p32);

		EEE0A = _mm256_add_epi32(EEEE0A, EEEO0A);          // EEE0 = EEEE0 + EEEO0
		EEE0B = _mm256_add_epi32(EEEE0B, EEEO0B);
		EEE1A = _mm256_add_epi32(EEEE1A, EEEO1A);          // EEE1 = EEEE1 + EEEO1
		EEE1B = _mm256_add_epi32(EEEE1B, EEEO1B);
		EEE3A = _mm256_sub_epi32(EEEE0A, EEEO0A);          // EEE2 = EEEE0 - EEEO0
		EEE3B = _mm256_sub_epi32(EEEE0B, EEEO0B);
		EEE2A = _mm256_sub_epi32(EEEE1A, EEEO1A);          // EEE3 = EEEE1 - EEEO1
		EEE2B = _mm256_sub_epi32(EEEE1B, EEEO1B);

		EE0A = _mm256_add_epi32(EEE0A, EEO0A);          // EE0 = EEE0 + EEO0
		EE0B = _mm256_add_epi32(EEE0B, EEO0B);
		EE1A = _mm256_add_epi32(EEE1A, EEO1A);          // EE1 = EEE1 + EEO1
		EE1B = _mm256_add_epi32(EEE1B, EEO1B);
		EE2A = _mm256_add_epi32(EEE2A, EEO2A);          // EE2 = EEE0 + EEO0
		EE2B = _mm256_add_epi32(EEE2B, EEO2B);
		EE3A = _mm256_add_epi32(EEE3A, EEO3A);          // EE3 = EEE1 + EEO1
		EE3B = _mm256_add_epi32(EEE3B, EEO3B);
		EE7A = _mm256_sub_epi32(EEE0A, EEO0A);          // EE7 = EEE0 - EEO0
		EE7B = _mm256_sub_epi32(EEE0B, EEO0B);
		EE6A = _mm256_sub_epi32(EEE1A, EEO1A);          // EE6 = EEE1 - EEO1
		EE6B = _mm256_sub_epi32(EEE1B, EEO1B);
		EE5A = _mm256_sub_epi32(EEE2A, EEO2A);          // EE5 = EEE0 - EEO0
		EE5B = _mm256_sub_epi32(EEE2B, EEO2B);
		EE4A = _mm256_sub_epi32(EEE3A, EEO3A);          // EE4 = EEE1 - EEO1
		EE4B = _mm256_sub_epi32(EEE3B, EEO3B);

		E0A = _mm256_add_epi32(EE0A, EO0A);          // E0 = EE0 + EO0
		E0B = _mm256_add_epi32(EE0B, EO0B);
		E1A = _mm256_add_epi32(EE1A, EO1A);          // E1 = EE1 + EO1
		E1B = _mm256_add_epi32(EE1B, EO1B);
		E2A = _mm256_add_epi32(EE2A, EO2A);          // E2 = EE2 + EO2
		E2B = _mm256_add_epi32(EE2B, EO2B);
		E3A = _mm256_add_epi32(EE3A, EO3A);          // E3 = EE3 + EO3
		E3B = _mm256_add_epi32(EE3B, EO3B);
		E4A = _mm256_add_epi32(EE4A, EO4A);          // E4 =
		E4B = _mm256_add_epi32(EE4B, EO4B);
		E5A = _mm256_add_epi32(EE5A, EO5A);          // E5 =
		E5B = _mm256_add_epi32(EE5B, EO5B);
		E6A = _mm256_add_epi32(EE6A, EO6A);          // E6 =
		E6B = _mm256_add_epi32(EE6B, EO6B);
		E7A = _mm256_add_epi32(EE7A, EO7A);          // E7 =
		E7B = _mm256_add_epi32(EE7B, EO7B);
		EFA = _mm256_sub_epi32(EE0A, EO0A);          // EF = EE0 - EO0
		EFB = _mm256_sub_epi32(EE0B, EO0B);
		EEA = _mm256_sub_epi32(EE1A, EO1A);          // EE = EE1 - EO1
		EEB = _mm256_sub_epi32(EE1B, EO1B);
		EDA = _mm256_sub_epi32(EE2A, EO2A);          // ED = EE2 - EO2
		EDB = _mm256_sub_epi32(EE2B, EO2B);
		ECA = _mm256_sub_epi32(EE3A, EO3A);          // EC = EE3 - EO3
		ECB = _mm256_sub_epi32(EE3B, EO3B);
		EBA = _mm256_sub_epi32(EE4A, EO4A);          // EB =
		EBB = _mm256_sub_epi32(EE4B, EO4B);
		EAA = _mm256_sub_epi32(EE5A, EO5A);          // EA =
		EAB = _mm256_sub_epi32(EE5B, EO5B);
		E9A = _mm256_sub_epi32(EE6A, EO6A);          // E9 =
		E9B = _mm256_sub_epi32(EE6B, EO6B);
		E8A = _mm256_sub_epi32(EE7A, EO7A);          // E8 =
		E8B = _mm256_sub_epi32(EE7B, EO7B);

		T10A = _mm256_add_epi32(E0A, c32_rnd);         // E0 + rnd
		T10B = _mm256_add_epi32(E0B, c32_rnd);
		T11A = _mm256_add_epi32(E1A, c32_rnd);         // E1 + rnd
		T11B = _mm256_add_epi32(E1B, c32_rnd);
		T12A = _mm256_add_epi32(E2A, c32_rnd);         // E2 + rnd
		T12B = _mm256_add_epi32(E2B, c32_rnd);
		T13A = _mm256_add_epi32(E3A, c32_rnd);         // E3 + rnd
		T13B = _mm256_add_epi32(E3B, c32_rnd);
		T14A = _mm256_add_epi32(E4A, c32_rnd);         // E4 + rnd
		T14B = _mm256_add_epi32(E4B, c32_rnd);
		T15A = _mm256_add_epi32(E5A, c32_rnd);         // E5 + rnd
		T15B = _mm256_add_epi32(E5B, c32_rnd);
		T16A = _mm256_add_epi32(E6A, c32_rnd);         // E6 + rnd
		T16B = _mm256_add_epi32(E6B, c32_rnd);
		T17A = _mm256_add_epi32(E7A, c32_rnd);         // E7 + rnd
		T17B = _mm256_add_epi32(E7B, c32_rnd);
		T18A = _mm256_add_epi32(E8A, c32_rnd);         // E8 + rnd
		T18B = _mm256_add_epi32(E8B, c32_rnd);
		T19A = _mm256_add_epi32(E9A, c32_rnd);         // E9 + rnd
		T19B = _mm256_add_epi32(E9B, c32_rnd);
		T1AA = _mm256_add_epi32(EAA, c32_rnd);         // E10 + rnd
		T1AB = _mm256_add_epi32(EAB, c32_rnd);
		T1BA = _mm256_add_epi32(EBA, c32_rnd);         // E11 + rnd
		T1BB = _mm256_add_epi32(EBB, c32_rnd);
		T1CA = _mm256_add_epi32(ECA, c32_rnd);         // E12 + rnd
		T1CB = _mm256_add_epi32(ECB, c32_rnd);
		T1DA = _mm256_add_epi32(EDA, c32_rnd);         // E13 + rnd
		T1DB = _mm256_add_epi32(EDB, c32_rnd);
		T1EA = _mm256_add_epi32(EEA, c32_rnd);         // E14 + rnd
		T1EB = _mm256_add_epi32(EEB, c32_rnd);
		T1FA = _mm256_add_epi32(EFA, c32_rnd);         // E15 + rnd
		T1FB = _mm256_add_epi32(EFB, c32_rnd);

		T2_00A = _mm256_add_epi32(T10A, O00A);          // E0 + O0 + rnd
		T2_00B = _mm256_add_epi32(T10B, O00B);
		T2_01A = _mm256_add_epi32(T11A, O01A);          // E1 + O1 + rnd
		T2_01B = _mm256_add_epi32(T11B, O01B);
		T2_02A = _mm256_add_epi32(T12A, O02A);          // E2 + O2 + rnd
		T2_02B = _mm256_add_epi32(T12B, O02B);
		T2_03A = _mm256_add_epi32(T13A, O03A);          // E3 + O3 + rnd
		T2_03B = _mm256_add_epi32(T13B, O03B);
		T2_04A = _mm256_add_epi32(T14A, O04A);          // E4
		T2_04B = _mm256_add_epi32(T14B, O04B);
		T2_05A = _mm256_add_epi32(T15A, O05A);          // E5
		T2_05B = _mm256_add_epi32(T15B, O05B);
		T2_06A = _mm256_add_epi32(T16A, O06A);          // E6
		T2_06B = _mm256_add_epi32(T16B, O06B);
		T2_07A = _mm256_add_epi32(T17A, O07A);          // E7
		T2_07B = _mm256_add_epi32(T17B, O07B);
		T2_08A = _mm256_add_epi32(T18A, O08A);          // E8
		T2_08B = _mm256_add_epi32(T18B, O08B);
		T2_09A = _mm256_add_epi32(T19A, O09A);          // E9
		T2_09B = _mm256_add_epi32(T19B, O09B);
		T2_10A = _mm256_add_epi32(T1AA, O10A);          // E10
		T2_10B = _mm256_add_epi32(T1AB, O10B);
		T2_11A = _mm256_add_epi32(T1BA, O11A);          // E11
		T2_11B = _mm256_add_epi32(T1BB, O11B);
		T2_12A = _mm256_add_epi32(T1CA, O12A);          // E12
		T2_12B = _mm256_add_epi32(T1CB, O12B);
		T2_13A = _mm256_add_epi32(T1DA, O13A);          // E13
		T2_13B = _mm256_add_epi32(T1DB, O13B);
		T2_14A = _mm256_add_epi32(T1EA, O14A);          // E14
		T2_14B = _mm256_add_epi32(T1EB, O14B);
		T2_15A = _mm256_add_epi32(T1FA, O15A);          // E15
		T2_15B = _mm256_add_epi32(T1FB, O15B);
		T2_31A = _mm256_sub_epi32(T10A, O00A);          // E0 - O0 + rnd
		T2_31B = _mm256_sub_epi32(T10B, O00B);
		T2_30A = _mm256_sub_epi32(T11A, O01A);          // E1 - O1 + rnd
		T2_30B = _mm256_sub_epi32(T11B, O01B);
		T2_29A = _mm256_sub_epi32(T12A, O02A);          // E2 - O2 + rnd
		T2_29B = _mm256_sub_epi32(T12B, O02B);
		T2_28A = _mm256_sub_epi32(T13A, O03A);          // E3 - O3 + rnd
		T2_28B = _mm256_sub_epi32(T13B, O03B);
		T2_27A = _mm256_sub_epi32(T14A, O04A);          // E4
		T2_27B = _mm256_sub_epi32(T14B, O04B);
		T2_26A = _mm256_sub_epi32(T15A, O05A);          // E5
		T2_26B = _mm256_sub_epi32(T15B, O05B);
		T2_25A = _mm256_sub_epi32(T16A, O06A);          // E6
		T2_25B = _mm256_sub_epi32(T16B, O06B);
		T2_24A = _mm256_sub_epi32(T17A, O07A);          // E7
		T2_24B = _mm256_sub_epi32(T17B, O07B);
		T2_23A = _mm256_sub_epi32(T18A, O08A);          //
		T2_23B = _mm256_sub_epi32(T18B, O08B);
		T2_22A = _mm256_sub_epi32(T19A, O09A);          //
		T2_22B = _mm256_sub_epi32(T19B, O09B);
		T2_21A = _mm256_sub_epi32(T1AA, O10A);          //
		T2_21B = _mm256_sub_epi32(T1AB, O10B);
		T2_20A = _mm256_sub_epi32(T1BA, O11A);          //
		T2_20B = _mm256_sub_epi32(T1BB, O11B);
		T2_19A = _mm256_sub_epi32(T1CA, O12A);          //
		T2_19B = _mm256_sub_epi32(T1CB, O12B);
		T2_18A = _mm256_sub_epi32(T1DA, O13A);          //
		T2_18B = _mm256_sub_epi32(T1DB, O13B);
		T2_17A = _mm256_sub_epi32(T1EA, O14A);          //
		T2_17B = _mm256_sub_epi32(T1EB, O14B);
		T2_16A = _mm256_sub_epi32(T1FA, O15A);          //
		T2_16B = _mm256_sub_epi32(T1FB, O15B);

		T3_00A = _mm256_srai_epi32(T2_00A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_00B = _mm256_srai_epi32(T2_00B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_01A = _mm256_srai_epi32(T2_01A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_01B = _mm256_srai_epi32(T2_01B, nShift);             // [71 61 51 41] // This operation make it much slower than 128
		T3_02A = _mm256_srai_epi32(T2_02A, nShift);             // [32 22 12 02] // This operation make it much slower than 128
		T3_02B = _mm256_srai_epi32(T2_02B, nShift);             // [72 62 52 42]
		T3_03A = _mm256_srai_epi32(T2_03A, nShift);             // [33 23 13 03]
		T3_03B = _mm256_srai_epi32(T2_03B, nShift);             // [73 63 53 43]
		T3_04A = _mm256_srai_epi32(T2_04A, nShift);             // [33 24 14 04]
		T3_04B = _mm256_srai_epi32(T2_04B, nShift);             // [74 64 54 44]
		T3_05A = _mm256_srai_epi32(T2_05A, nShift);             // [35 25 15 05]
		T3_05B = _mm256_srai_epi32(T2_05B, nShift);             // [75 65 55 45]
		T3_06A = _mm256_srai_epi32(T2_06A, nShift);             // [36 26 16 06]
		T3_06B = _mm256_srai_epi32(T2_06B, nShift);             // [76 66 56 46]
		T3_07A = _mm256_srai_epi32(T2_07A, nShift);             // [37 27 17 07]
		T3_07B = _mm256_srai_epi32(T2_07B, nShift);             // [77 67 57 47]
		T3_08A = _mm256_srai_epi32(T2_08A, nShift);             // [30 20 10 00] x8
		T3_08B = _mm256_srai_epi32(T2_08B, nShift);             // [70 60 50 40]
		T3_09A = _mm256_srai_epi32(T2_09A, nShift);             // [31 21 11 01] x9
		T3_09B = _mm256_srai_epi32(T2_09B, nShift);             // [71 61 51 41]
		T3_10A = _mm256_srai_epi32(T2_10A, nShift);             // [32 22 12 02] xA
		T3_10B = _mm256_srai_epi32(T2_10B, nShift);             // [72 62 52 42]
		T3_11A = _mm256_srai_epi32(T2_11A, nShift);             // [33 23 13 03] xB
		T3_11B = _mm256_srai_epi32(T2_11B, nShift);             // [73 63 53 43]
		T3_12A = _mm256_srai_epi32(T2_12A, nShift);             // [33 24 14 04] xC
		T3_12B = _mm256_srai_epi32(T2_12B, nShift);             // [74 64 54 44]
		T3_13A = _mm256_srai_epi32(T2_13A, nShift);             // [35 25 15 05] xD
		T3_13B = _mm256_srai_epi32(T2_13B, nShift);             // [75 65 55 45]
		T3_14A = _mm256_srai_epi32(T2_14A, nShift);             // [36 26 16 06] xE
		T3_14B = _mm256_srai_epi32(T2_14B, nShift);             // [76 66 56 46]
		T3_15A = _mm256_srai_epi32(T2_15A, nShift);             // [37 27 17 07] xF
		T3_15B = _mm256_srai_epi32(T2_15B, nShift);             // [77 67 57 47]

		T3_16A = _mm256_srai_epi32(T2_16A, nShift);             // [30 20 10 00] // This operation make it much slower than 128
		T3_16B = _mm256_srai_epi32(T2_16B, nShift);             // [70 60 50 40] // This operation make it much slower than 128
		T3_17A = _mm256_srai_epi32(T2_17A, nShift);             // [31 21 11 01] // This operation make it much slower than 128
		T3_17B = _mm256_srai_epi32(T2_17B, nShift);             // [71 61 51 41]
		T3_18A = _mm256_srai_epi32(T2_18A, nShift);             // [32 22 12 02]
		T3_18B = _mm256_srai_epi32(T2_18B, nShift);             // [72 62 52 42]
		T3_19A = _mm256_srai_epi32(T2_19A, nShift);             // [33 23 13 03]
		T3_19B = _mm256_srai_epi32(T2_19B, nShift);             // [73 63 53 43]
		T3_20A = _mm256_srai_epi32(T2_20A, nShift);             // [33 24 14 04]
		T3_20B = _mm256_srai_epi32(T2_20B, nShift);             // [74 64 54 44]
		T3_21A = _mm256_srai_epi32(T2_21A, nShift);             // [35 25 15 05]
		T3_21B = _mm256_srai_epi32(T2_21B, nShift);             // [75 65 55 45]
		T3_22A = _mm256_srai_epi32(T2_22A, nShift);             // [36 26 16 06]
		T3_22B = _mm256_srai_epi32(T2_22B, nShift);             // [76 66 56 46]
		T3_23A = _mm256_srai_epi32(T2_23A, nShift);             // [37 27 17 07]
		T3_23B = _mm256_srai_epi32(T2_23B, nShift);             // [77 67 57 47]
		T3_24A = _mm256_srai_epi32(T2_24A, nShift);             // [30 20 10 00] x8
		T3_24B = _mm256_srai_epi32(T2_24B, nShift);             // [70 60 50 40]
		T3_25A = _mm256_srai_epi32(T2_25A, nShift);             // [31 21 11 01] x9
		T3_25B = _mm256_srai_epi32(T2_25B, nShift);             // [71 61 51 41]
		T3_26A = _mm256_srai_epi32(T2_26A, nShift);             // [32 22 12 02] xA
		T3_26B = _mm256_srai_epi32(T2_26B, nShift);             // [72 62 52 42]
		T3_27A = _mm256_srai_epi32(T2_27A, nShift);             // [33 23 13 03] xB
		T3_27B = _mm256_srai_epi32(T2_27B, nShift);             // [73 63 53 43]
		T3_28A = _mm256_srai_epi32(T2_28A, nShift);             // [33 24 14 04] xC
		T3_28B = _mm256_srai_epi32(T2_28B, nShift);             // [74 64 54 44]
		T3_29A = _mm256_srai_epi32(T2_29A, nShift);             // [35 25 15 05] xD
		T3_29B = _mm256_srai_epi32(T2_29B, nShift);             // [75 65 55 45]
		T3_30A = _mm256_srai_epi32(T2_30A, nShift);             // [36 26 16 06] xE
		T3_30B = _mm256_srai_epi32(T2_30B, nShift);             // [76 66 56 46]
		T3_31A = _mm256_srai_epi32(T2_31A, nShift);             // [37 27 17 07] xF
		T3_31B = _mm256_srai_epi32(T2_31B, nShift);             // [77 67 57 47]

		res00[part] = _mm256_packs_epi32(T3_00A, T3_00B);        // [70 60 50 40 30 20 10 00]
		res01[part] = _mm256_packs_epi32(T3_01A, T3_01B);        // [71 61 51 41 31 21 11 01]
		res02[part] = _mm256_packs_epi32(T3_02A, T3_02B);        // [72 62 52 42 32 22 12 02]
		res03[part] = _mm256_packs_epi32(T3_03A, T3_03B);        // [73 63 53 43 33 23 13 03]
		res04[part] = _mm256_packs_epi32(T3_04A, T3_04B);        // [74 64 54 44 34 24 14 04]
		res05[part] = _mm256_packs_epi32(T3_05A, T3_05B);        // [75 65 55 45 35 25 15 05]
		res06[part] = _mm256_packs_epi32(T3_06A, T3_06B);        // [76 66 56 46 36 26 16 06]
		res07[part] = _mm256_packs_epi32(T3_07A, T3_07B);        // [77 67 57 47 37 27 17 07]
		res08[part] = _mm256_packs_epi32(T3_08A, T3_08B);        // [A0 ... 80]
		res09[part] = _mm256_packs_epi32(T3_09A, T3_09B);        // [A1 ... 81]
		res10[part] = _mm256_packs_epi32(T3_10A, T3_10B);        // [A2 ... 82]
		res11[part] = _mm256_packs_epi32(T3_11A, T3_11B);        // [A3 ... 83]
		res12[part] = _mm256_packs_epi32(T3_12A, T3_12B);        // [A4 ... 84]
		res13[part] = _mm256_packs_epi32(T3_13A, T3_13B);        // [A5 ... 85]
		res14[part] = _mm256_packs_epi32(T3_14A, T3_14B);        // [A6 ... 86]
		res15[part] = _mm256_packs_epi32(T3_15A, T3_15B);        // [A7 ... 87]
		res16[part] = _mm256_packs_epi32(T3_16A, T3_16B);
		res17[part] = _mm256_packs_epi32(T3_17A, T3_17B);
		res18[part] = _mm256_packs_epi32(T3_18A, T3_18B);
		res19[part] = _mm256_packs_epi32(T3_19A, T3_19B);
		res20[part] = _mm256_packs_epi32(T3_20A, T3_20B);
		res21[part] = _mm256_packs_epi32(T3_21A, T3_21B);
		res22[part] = _mm256_packs_epi32(T3_22A, T3_22B);
		res23[part] = _mm256_packs_epi32(T3_23A, T3_23B);
		res24[part] = _mm256_packs_epi32(T3_24A, T3_24B);
		res25[part] = _mm256_packs_epi32(T3_25A, T3_25B);
		res26[part] = _mm256_packs_epi32(T3_26A, T3_26B);
		res27[part] = _mm256_packs_epi32(T3_27A, T3_27B);
		res28[part] = _mm256_packs_epi32(T3_28A, T3_28B);
		res29[part] = _mm256_packs_epi32(T3_29A, T3_29B);
		res30[part] = _mm256_packs_epi32(T3_30A, T3_30B);
		res31[part] = _mm256_packs_epi32(T3_31A, T3_31B);

	}

	//transpose 32x32 matrix
	{
		__m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7, tr0_8, tr0_9, tr0_10, tr0_11, tr0_12, tr0_13, tr0_14, tr0_15;
#define TRANSPOSE_16x16_16BIT(I0, I1, I2, I3, I4, I5, I6, I7, I8, I9, I10, I11, I12, I13, I14, I15, O0, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10, O11, O12, O13, O14, O15) \
	tr0_0 = _mm256_unpacklo_epi16(I0, I1); \
	tr0_1 = _mm256_unpacklo_epi16(I2, I3); \
	tr0_2 = _mm256_unpacklo_epi16(I4, I5); \
	tr0_3 = _mm256_unpacklo_epi16(I6, I7); \
	tr0_4 = _mm256_unpacklo_epi16(I8, I9); \
	tr0_5 = _mm256_unpacklo_epi16(I10, I11); \
	tr0_6 = _mm256_unpacklo_epi16(I12, I13); \
	tr0_7 = _mm256_unpacklo_epi16(I14, I15); \
	tr0_8 = _mm256_unpackhi_epi16(I0, I1); \
	tr0_9 = _mm256_unpackhi_epi16(I2, I3); \
	tr0_10 = _mm256_unpackhi_epi16(I4, I5); \
	tr0_11 = _mm256_unpackhi_epi16(I6, I7); \
	tr0_12 = _mm256_unpackhi_epi16(I8, I9); \
	tr0_13 = _mm256_unpackhi_epi16(I10, I11); \
	tr0_14 = _mm256_unpackhi_epi16(I12, I13); \
	tr0_15 = _mm256_unpackhi_epi16(I14, I15); \
	O0 = _mm256_unpacklo_epi32(tr0_0, tr0_1); \
	O1 = _mm256_unpacklo_epi32(tr0_2, tr0_3); \
	O2 = _mm256_unpacklo_epi32(tr0_4, tr0_5); \
	O3 = _mm256_unpacklo_epi32(tr0_6, tr0_7); \
	O4 = _mm256_unpackhi_epi32(tr0_0, tr0_1); \
	O5 = _mm256_unpackhi_epi32(tr0_2, tr0_3); \
	O6 = _mm256_unpackhi_epi32(tr0_4, tr0_5); \
	O7 = _mm256_unpackhi_epi32(tr0_6, tr0_7); \
	O8 = _mm256_unpacklo_epi32(tr0_8, tr0_9); \
	O9 = _mm256_unpacklo_epi32(tr0_10, tr0_11); \
	O10 = _mm256_unpacklo_epi32(tr0_12, tr0_13); \
	O11 = _mm256_unpacklo_epi32(tr0_14, tr0_15); \
	O12 = _mm256_unpackhi_epi32(tr0_8, tr0_9); \
	O13 = _mm256_unpackhi_epi32(tr0_10, tr0_11); \
	O14 = _mm256_unpackhi_epi32(tr0_12, tr0_13); \
	O15 = _mm256_unpackhi_epi32(tr0_14, tr0_15); \
	tr0_0 = _mm256_unpacklo_epi64(O0, O1); \
	tr0_1 = _mm256_unpacklo_epi64(O2, O3); \
	tr0_2 = _mm256_unpackhi_epi64(O0, O1); \
	tr0_3 = _mm256_unpackhi_epi64(O2, O3); \
	tr0_4 = _mm256_unpacklo_epi64(O4, O5); \
	tr0_5 = _mm256_unpacklo_epi64(O6, O7); \
	tr0_6 = _mm256_unpackhi_epi64(O4, O5); \
	tr0_7 = _mm256_unpackhi_epi64(O6, O7); \
	tr0_8 = _mm256_unpacklo_epi64(O8, O9); \
	tr0_9 = _mm256_unpacklo_epi64(O10, O11); \
	tr0_10 = _mm256_unpackhi_epi64(O8, O9); \
	tr0_11 = _mm256_unpackhi_epi64(O10, O11); \
	tr0_12 = _mm256_unpacklo_epi64(O12, O13); \
	tr0_13 = _mm256_unpacklo_epi64(O14, O15); \
	tr0_14 = _mm256_unpackhi_epi64(O12, O13); \
	tr0_15 = _mm256_unpackhi_epi64(O14, O15); \
	O0 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x20); \
	O1 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x20); \
	O2 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x20); \
	O3 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x20); \
	O4 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x20); \
	O5 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x20); \
	O6 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x20); \
	O7 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x20); \
	O8 = _mm256_permute2x128_si256(tr0_0, tr0_1, 0x31); \
	O9 = _mm256_permute2x128_si256(tr0_2, tr0_3, 0x31); \
	O10 = _mm256_permute2x128_si256(tr0_4, tr0_5, 0x31); \
	O11 = _mm256_permute2x128_si256(tr0_6, tr0_7, 0x31); \
	O12 = _mm256_permute2x128_si256(tr0_8, tr0_9, 0x31); \
	O13 = _mm256_permute2x128_si256(tr0_10, tr0_11, 0x31); \
	O14 = _mm256_permute2x128_si256(tr0_12, tr0_13, 0x31); \
	O15 = _mm256_permute2x128_si256(tr0_14, tr0_15, 0x31); \

		TRANSPOSE_16x16_16BIT(res00[0], res01[0], res02[0], res03[0], res04[0], res05[0], res06[0], res07[0], res08[0], res09[0], res10[0], res11[0], res12[0], res13[0], res14[0], res15[0], in00[0], in01[0], in02[0], in03[0], in04[0], in05[0], in06[0], in07[0], in08[0], in09[0], in10[0], in11[0], in12[0], in13[0], in14[0], in15[0]);
		TRANSPOSE_16x16_16BIT(res16[0], res17[0], res18[0], res19[0], res20[0], res21[0], res22[0], res23[0], res24[0], res25[0], res26[0], res27[0], res28[0], res29[0], res30[0], res31[0], in00[1], in01[1], in02[1], in03[1], in04[1], in05[1], in06[1], in07[1], in08[1], in09[1], in10[1], in11[1], in12[1], in13[1], in14[1], in15[1]);
		TRANSPOSE_16x16_16BIT(res00[1], res01[1], res02[1], res03[1], res04[1], res05[1], res06[1], res07[1], res08[1], res09[1], res10[1], res11[1], res12[1], res13[1], res14[1], res15[1], in16[0], in17[0], in18[0], in19[0], in20[0], in21[0], in22[0], in23[0], in24[0], in25[0], in26[0], in27[0], in28[0], in29[0], in30[0], in31[0]);
		TRANSPOSE_16x16_16BIT(res16[1], res17[1], res18[1], res19[1], res20[1], res21[1], res22[1], res23[1], res24[1], res25[1], res26[1], res27[1], res28[1], res29[1], res30[1], res31[1], in16[1], in17[1], in18[1], in19[1], in20[1], in21[1], in22[1], in23[1], in24[1], in25[1], in26[1], in27[1], in28[1], in29[1], in30[1], in31[1]);

#undef TRANSPOSE_16x16_16BIT
	}

	//clip
	max_val = _mm256_set1_epi16(511);
	min_val = _mm256_set1_epi16(-512);

	for (k = 0; k < 2; k++)
	{
		in00[k] = _mm256_min_epi16(in00[k], max_val);
		in00[k] = _mm256_max_epi16(in00[k], min_val);
		in01[k] = _mm256_min_epi16(in01[k], max_val);
		in01[k] = _mm256_max_epi16(in01[k], min_val);
		in02[k] = _mm256_min_epi16(in02[k], max_val);
		in02[k] = _mm256_max_epi16(in02[k], min_val);
		in03[k] = _mm256_min_epi16(in03[k], max_val);
		in03[k] = _mm256_max_epi16(in03[k], min_val);
		in04[k] = _mm256_min_epi16(in04[k], max_val);
		in04[k] = _mm256_max_epi16(in04[k], min_val);
		in05[k] = _mm256_min_epi16(in05[k], max_val);
		in05[k] = _mm256_max_epi16(in05[k], min_val);
		in06[k] = _mm256_min_epi16(in06[k], max_val);
		in06[k] = _mm256_max_epi16(in06[k], min_val);
		in07[k] = _mm256_min_epi16(in07[k], max_val);
		in07[k] = _mm256_max_epi16(in07[k], min_val);
		in08[k] = _mm256_min_epi16(in08[k], max_val);
		in08[k] = _mm256_max_epi16(in08[k], min_val);
		in09[k] = _mm256_min_epi16(in09[k], max_val);
		in09[k] = _mm256_max_epi16(in09[k], min_val);
		in10[k] = _mm256_min_epi16(in10[k], max_val);
		in10[k] = _mm256_max_epi16(in10[k], min_val);
		in11[k] = _mm256_min_epi16(in11[k], max_val);
		in11[k] = _mm256_max_epi16(in11[k], min_val);
		in12[k] = _mm256_min_epi16(in12[k], max_val);
		in12[k] = _mm256_max_epi16(in12[k], min_val);
		in13[k] = _mm256_min_epi16(in13[k], max_val);
		in13[k] = _mm256_max_epi16(in13[k], min_val);
		in14[k] = _mm256_min_epi16(in14[k], max_val);
		in14[k] = _mm256_max_epi16(in14[k], min_val);
		in15[k] = _mm256_min_epi16(in15[k], max_val);
		in15[k] = _mm256_max_epi16(in15[k], min_val);
		in16[k] = _mm256_min_epi16(in16[k], max_val);
		in16[k] = _mm256_max_epi16(in16[k], min_val);
		in17[k] = _mm256_min_epi16(in17[k], max_val);
		in17[k] = _mm256_max_epi16(in17[k], min_val);
		in18[k] = _mm256_min_epi16(in18[k], max_val);
		in18[k] = _mm256_max_epi16(in18[k], min_val);
		in19[k] = _mm256_min_epi16(in19[k], max_val);
		in19[k] = _mm256_max_epi16(in19[k], min_val);
		in20[k] = _mm256_min_epi16(in20[k], max_val);
		in20[k] = _mm256_max_epi16(in20[k], min_val);
		in21[k] = _mm256_min_epi16(in21[k], max_val);
		in21[k] = _mm256_max_epi16(in21[k], min_val);
		in22[k] = _mm256_min_epi16(in22[k], max_val);
		in22[k] = _mm256_max_epi16(in22[k], min_val);
		in23[k] = _mm256_min_epi16(in23[k], max_val);
		in23[k] = _mm256_max_epi16(in23[k], min_val);
		in24[k] = _mm256_min_epi16(in24[k], max_val);
		in24[k] = _mm256_max_epi16(in24[k], min_val);
		in25[k] = _mm256_min_epi16(in25[k], max_val);
		in25[k] = _mm256_max_epi16(in25[k], min_val);
		in26[k] = _mm256_min_epi16(in26[k], max_val);
		in26[k] = _mm256_max_epi16(in26[k], min_val);
		in27[k] = _mm256_min_epi16(in27[k], max_val);
		in27[k] = _mm256_max_epi16(in27[k], min_val);
		in28[k] = _mm256_min_epi16(in28[k], max_val);
		in28[k] = _mm256_max_epi16(in28[k], min_val);
		in29[k] = _mm256_min_epi16(in29[k], max_val);
		in29[k] = _mm256_max_epi16(in29[k], min_val);
		in30[k] = _mm256_min_epi16(in30[k], max_val);
		in30[k] = _mm256_max_epi16(in30[k], min_val);
		in31[k] = _mm256_min_epi16(in31[k], max_val);
		in31[k] = _mm256_max_epi16(in31[k], min_val);
	}


	//	int i;
    {
        __m256i tr0_0, tr0_1, tr0_2, tr0_3, tr0_4, tr0_5, tr0_6, tr0_7;
        __m256i tr1_0, tr1_1, tr1_2, tr1_3, tr1_4, tr1_5, tr1_6, tr1_7;
        __m256i	t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;

        //按行 64*64
        __m256i T00[4], T01[4], T02[4], T03[4], T04[4], T05[4], T06[4], T07[4], T08[4], T09[4], T10[4], T11[4], T12[4], T13[4], T14[4], T15[4], T16[4], T17[4], T18[4], T19[4], T20[4], T21[4], T22[4], T23[4], T24[4], T25[4], T26[4], T27[4], T28[4], T29[4], T30[4], T31[4], T32[4], T33[4], T34[4], T35[4], T36[4], T37[4], T38[4], T39[4], T40[4], T41[4], T42[4], T43[4], T44[4], T45[4], T46[4], T47[4], T48[4], T49[4], T50[4], T51[4], T52[4], T53[4], T54[4], T55[4], T56[4], T57[4], T58[4], T59[4], T60[4], T61[4], T62[4], T63[4];

        //按列 64*64
        __m256i V00[4], V01[4], V02[4], V03[4], V04[4], V05[4], V06[4], V07[4], V08[4], V09[4], V10[4], V11[4], V12[4], V13[4], V14[4], V15[4], V16[4], V17[4], V18[4], V19[4], V20[4], V21[4], V22[4], V23[4], V24[4], V25[4], V26[4], V27[4], V28[4], V29[4], V30[4], V31[4], V32[4], V33[4], V34[4], V35[4], V36[4], V37[4], V38[4], V39[4], V40[4], V41[4], V42[4], V43[4], V44[4], V45[4], V46[4], V47[4], V48[4], V49[4], V50[4], V51[4], V52[4], V53[4], V54[4], V55[4], V56[4], V57[4], V58[4], V59[4], V60[4], V61[4], V62[4], V63[4];

        /*--vertical transform--*/
        //32*32, LOAD AND SHIFT
        for (i = 0; i < 2; i++){
            T00[i] = _mm256_srai_epi16(in00[i], 1);
            T01[i] = _mm256_srai_epi16(in01[i], 1);
            T02[i] = _mm256_srai_epi16(in02[i], 1);
            T03[i] = _mm256_srai_epi16(in03[i], 1);
            T04[i] = _mm256_srai_epi16(in04[i], 1);
            T05[i] = _mm256_srai_epi16(in05[i], 1);
            T06[i] = _mm256_srai_epi16(in06[i], 1);
            T07[i] = _mm256_srai_epi16(in07[i], 1);
            T08[i] = _mm256_srai_epi16(in08[i], 1);
            T09[i] = _mm256_srai_epi16(in09[i], 1);

            T10[i] = _mm256_srai_epi16(in10[i], 1);
            T11[i] = _mm256_srai_epi16(in11[i], 1);
            T12[i] = _mm256_srai_epi16(in12[i], 1);
            T13[i] = _mm256_srai_epi16(in13[i], 1);
            T14[i] = _mm256_srai_epi16(in14[i], 1);
            T15[i] = _mm256_srai_epi16(in15[i], 1);
            T16[i] = _mm256_srai_epi16(in16[i], 1);
            T17[i] = _mm256_srai_epi16(in17[i], 1);
            T18[i] = _mm256_srai_epi16(in18[i], 1);
            T19[i] = _mm256_srai_epi16(in19[i], 1);

            T20[i] = _mm256_srai_epi16(in20[i], 1);
            T21[i] = _mm256_srai_epi16(in21[i], 1);
            T22[i] = _mm256_srai_epi16(in22[i], 1);
            T23[i] = _mm256_srai_epi16(in23[i], 1);
            T24[i] = _mm256_srai_epi16(in24[i], 1);
            T25[i] = _mm256_srai_epi16(in25[i], 1);
            T26[i] = _mm256_srai_epi16(in26[i], 1);
            T27[i] = _mm256_srai_epi16(in27[i], 1);
            T28[i] = _mm256_srai_epi16(in28[i], 1);
            T29[i] = _mm256_srai_epi16(in29[i], 1);

            T30[i] = _mm256_srai_epi16(in30[i], 1);
            T31[i] = _mm256_srai_epi16(in31[i], 1);
        }

        //filter (odd pixel/row)
        for (i = 0; i < 2; i++){
            T32[i] = _mm256_srai_epi16(_mm256_add_epi16(T00[i], T01[i]), 1);
            T33[i] = _mm256_srai_epi16(_mm256_add_epi16(T01[i], T02[i]), 1);
            T34[i] = _mm256_srai_epi16(_mm256_add_epi16(T02[i], T03[i]), 1);
            T35[i] = _mm256_srai_epi16(_mm256_add_epi16(T03[i], T04[i]), 1);
            T36[i] = _mm256_srai_epi16(_mm256_add_epi16(T04[i], T05[i]), 1);
            T37[i] = _mm256_srai_epi16(_mm256_add_epi16(T05[i], T06[i]), 1);
            T38[i] = _mm256_srai_epi16(_mm256_add_epi16(T06[i], T07[i]), 1);
            T39[i] = _mm256_srai_epi16(_mm256_add_epi16(T07[i], T08[i]), 1);

            T40[i] = _mm256_srai_epi16(_mm256_add_epi16(T08[i], T09[i]), 1);
            T41[i] = _mm256_srai_epi16(_mm256_add_epi16(T09[i], T10[i]), 1);
            T42[i] = _mm256_srai_epi16(_mm256_add_epi16(T10[i], T11[i]), 1);
            T43[i] = _mm256_srai_epi16(_mm256_add_epi16(T11[i], T12[i]), 1);
            T44[i] = _mm256_srai_epi16(_mm256_add_epi16(T12[i], T13[i]), 1);
            T45[i] = _mm256_srai_epi16(_mm256_add_epi16(T13[i], T14[i]), 1);
            T46[i] = _mm256_srai_epi16(_mm256_add_epi16(T14[i], T15[i]), 1);
            T47[i] = _mm256_srai_epi16(_mm256_add_epi16(T15[i], T16[i]), 1);

            T48[i] = _mm256_srai_epi16(_mm256_add_epi16(T16[i], T17[i]), 1);
            T49[i] = _mm256_srai_epi16(_mm256_add_epi16(T17[i], T18[i]), 1);
            T50[i] = _mm256_srai_epi16(_mm256_add_epi16(T18[i], T19[i]), 1);
            T51[i] = _mm256_srai_epi16(_mm256_add_epi16(T19[i], T20[i]), 1);
            T52[i] = _mm256_srai_epi16(_mm256_add_epi16(T20[i], T21[i]), 1);
            T53[i] = _mm256_srai_epi16(_mm256_add_epi16(T21[i], T22[i]), 1);
            T54[i] = _mm256_srai_epi16(_mm256_add_epi16(T22[i], T23[i]), 1);
            T55[i] = _mm256_srai_epi16(_mm256_add_epi16(T23[i], T24[i]), 1);

            T56[i] = _mm256_srai_epi16(_mm256_add_epi16(T24[i], T25[i]), 1);
            T57[i] = _mm256_srai_epi16(_mm256_add_epi16(T25[i], T26[i]), 1);
            T58[i] = _mm256_srai_epi16(_mm256_add_epi16(T26[i], T27[i]), 1);
            T59[i] = _mm256_srai_epi16(_mm256_add_epi16(T27[i], T28[i]), 1);
            T60[i] = _mm256_srai_epi16(_mm256_add_epi16(T28[i], T29[i]), 1);
            T61[i] = _mm256_srai_epi16(_mm256_add_epi16(T29[i], T30[i]), 1);
            T62[i] = _mm256_srai_epi16(_mm256_add_epi16(T30[i], T31[i]), 1);
            T63[i] = _mm256_srai_epi16(_mm256_add_epi16(T31[i], T31[i]), 1);
        }

        /*--transposition--*/
        //32x64 -> 64x32
        TRANSPOSE_16x16_16BIT_m256i(T00[0], T32[0], T01[0], T33[0], T02[0], T34[0], T03[0], T35[0], T04[0], T36[0], T05[0], T37[0], T06[0], T38[0], T07[0], T39[0], V00[0], V01[0], V02[0], V03[0], V04[0], V05[0], V06[0], V07[0], V08[0], V09[0], V10[0], V11[0], V12[0], V13[0], V14[0], V15[0]);
        TRANSPOSE_16x16_16BIT_m256i(T08[0], T40[0], T09[0], T41[0], T10[0], T42[0], T11[0], T43[0], T12[0], T44[0], T13[0], T45[0], T14[0], T46[0], T15[0], T47[0], V00[1], V01[1], V02[1], V03[1], V04[1], V05[1], V06[1], V07[1], V08[1], V09[1], V10[1], V11[1], V12[1], V13[1], V14[1], V15[1]);
        TRANSPOSE_16x16_16BIT_m256i(T16[0], T48[0], T17[0], T49[0], T18[0], T50[0], T19[0], T51[0], T20[0], T52[0], T21[0], T53[0], T22[0], T54[0], T23[0], T55[0], V00[2], V01[2], V02[2], V03[2], V04[2], V05[2], V06[2], V07[2], V08[2], V09[2], V10[2], V11[2], V12[2], V13[2], V14[2], V15[2]);
        TRANSPOSE_16x16_16BIT_m256i(T24[0], T56[0], T25[0], T57[0], T26[0], T58[0], T27[0], T59[0], T28[0], T60[0], T29[0], T61[0], T30[0], T62[0], T31[0], T63[0], V00[3], V01[3], V02[3], V03[3], V04[3], V05[3], V06[3], V07[3], V08[3], V09[3], V10[3], V11[3], V12[3], V13[3], V14[3], V15[3]);

        TRANSPOSE_16x16_16BIT_m256i(T00[1], T32[1], T01[1], T33[1], T02[1], T34[1], T03[1], T35[1], T04[1], T36[1], T05[1], T37[1], T06[1], T38[1], T07[1], T39[1], V16[0], V17[0], V18[0], V19[0], V20[0], V21[0], V22[0], V23[0], V24[0], V25[0], V26[0], V27[0], V28[0], V29[0], V30[0], V31[0]);
        TRANSPOSE_16x16_16BIT_m256i(T08[1], T40[1], T09[1], T41[1], T10[1], T42[1], T11[1], T43[1], T12[1], T44[1], T13[1], T45[1], T14[1], T46[1], T15[1], T47[1], V16[1], V17[1], V18[1], V19[1], V20[1], V21[1], V22[1], V23[1], V24[1], V25[1], V26[1], V27[1], V28[1], V29[1], V30[1], V31[1]);
        TRANSPOSE_16x16_16BIT_m256i(T16[1], T48[1], T17[1], T49[1], T18[1], T50[1], T19[1], T51[1], T20[1], T52[1], T21[1], T53[1], T22[1], T54[1], T23[1], T55[1], V16[2], V17[2], V18[2], V19[2], V20[2], V21[2], V22[2], V23[2], V24[2], V25[2], V26[2], V27[2], V28[2], V29[2], V30[2], V31[2]);
        TRANSPOSE_16x16_16BIT_m256i(T24[1], T56[1], T25[1], T57[1], T26[1], T58[1], T27[1], T59[1], T28[1], T60[1], T29[1], T61[1], T30[1], T62[1], T31[1], T63[1], V16[3], V17[3], V18[3], V19[3], V20[3], V21[3], V22[3], V23[3], V24[3], V25[3], V26[3], V27[3], V28[3], V29[3], V30[3], V31[3]);

        /*--horizontal transform--*/
        //filter (odd pixel/column)
        for (i = 0; i < 4; i++){
            V32[i] = _mm256_srai_epi16(_mm256_add_epi16(V00[i], V01[i]), 1);
            V33[i] = _mm256_srai_epi16(_mm256_add_epi16(V01[i], V02[i]), 1);
            V34[i] = _mm256_srai_epi16(_mm256_add_epi16(V02[i], V03[i]), 1);
            V35[i] = _mm256_srai_epi16(_mm256_add_epi16(V03[i], V04[i]), 1);
            V36[i] = _mm256_srai_epi16(_mm256_add_epi16(V04[i], V05[i]), 1);
            V37[i] = _mm256_srai_epi16(_mm256_add_epi16(V05[i], V06[i]), 1);
            V38[i] = _mm256_srai_epi16(_mm256_add_epi16(V06[i], V07[i]), 1);
            V39[i] = _mm256_srai_epi16(_mm256_add_epi16(V07[i], V08[i]), 1);
            V40[i] = _mm256_srai_epi16(_mm256_add_epi16(V08[i], V09[i]), 1);
            V41[i] = _mm256_srai_epi16(_mm256_add_epi16(V09[i], V10[i]), 1);
            V42[i] = _mm256_srai_epi16(_mm256_add_epi16(V10[i], V11[i]), 1);
            V43[i] = _mm256_srai_epi16(_mm256_add_epi16(V11[i], V12[i]), 1);
            V44[i] = _mm256_srai_epi16(_mm256_add_epi16(V12[i], V13[i]), 1);
            V45[i] = _mm256_srai_epi16(_mm256_add_epi16(V13[i], V14[i]), 1);
            V46[i] = _mm256_srai_epi16(_mm256_add_epi16(V14[i], V15[i]), 1);
            V47[i] = _mm256_srai_epi16(_mm256_add_epi16(V15[i], V16[i]), 1);

            V48[i] = _mm256_srai_epi16(_mm256_add_epi16(V16[i], V17[i]), 1);
            V49[i] = _mm256_srai_epi16(_mm256_add_epi16(V17[i], V18[i]), 1);
            V50[i] = _mm256_srai_epi16(_mm256_add_epi16(V18[i], V19[i]), 1);
            V51[i] = _mm256_srai_epi16(_mm256_add_epi16(V19[i], V20[i]), 1);
            V52[i] = _mm256_srai_epi16(_mm256_add_epi16(V20[i], V21[i]), 1);
            V53[i] = _mm256_srai_epi16(_mm256_add_epi16(V21[i], V22[i]), 1);
            V54[i] = _mm256_srai_epi16(_mm256_add_epi16(V22[i], V23[i]), 1);
            V55[i] = _mm256_srai_epi16(_mm256_add_epi16(V23[i], V24[i]), 1);
            V56[i] = _mm256_srai_epi16(_mm256_add_epi16(V24[i], V25[i]), 1);
            V57[i] = _mm256_srai_epi16(_mm256_add_epi16(V25[i], V26[i]), 1);
            V58[i] = _mm256_srai_epi16(_mm256_add_epi16(V26[i], V27[i]), 1);
            V59[i] = _mm256_srai_epi16(_mm256_add_epi16(V27[i], V28[i]), 1);
            V60[i] = _mm256_srai_epi16(_mm256_add_epi16(V28[i], V29[i]), 1);
            V61[i] = _mm256_srai_epi16(_mm256_add_epi16(V29[i], V30[i]), 1);
            V62[i] = _mm256_srai_epi16(_mm256_add_epi16(V30[i], V31[i]), 1);
            V63[i] = _mm256_srai_epi16(_mm256_add_epi16(V31[i], V31[i]), 1);
        }

        /*--transposition & Store--*/
        //64x64 
        TRANSPOSE_16x16_16BIT_m256i(V00[0], V32[0], V01[0], V33[0], V02[0], V34[0], V03[0], V35[0], V04[0], V36[0], V05[0], V37[0], V06[0], V38[0], V07[0], V39[0], T00[0], T01[0], T02[0], T03[0], T04[0], T05[0], T06[0], T07[0], T08[0], T09[0], T10[0], T11[0], T12[0], T13[0], T14[0], T15[0]);
        TRANSPOSE_16x16_16BIT_m256i(V00[1], V32[1], V01[1], V33[1], V02[1], V34[1], V03[1], V35[1], V04[1], V36[1], V05[1], V37[1], V06[1], V38[1], V07[1], V39[1], T16[0], T17[0], T18[0], T19[0], T20[0], T21[0], T22[0], T23[0], T24[0], T25[0], T26[0], T27[0], T28[0], T29[0], T30[0], T31[0]);
        TRANSPOSE_16x16_16BIT_m256i(V00[2], V32[2], V01[2], V33[2], V02[2], V34[2], V03[2], V35[2], V04[2], V36[2], V05[2], V37[2], V06[2], V38[2], V07[2], V39[2], T32[0], T33[0], T34[0], T35[0], T36[0], T37[0], T38[0], T39[0], T40[0], T41[0], T42[0], T43[0], T44[0], T45[0], T46[0], T47[0]);
        TRANSPOSE_16x16_16BIT_m256i(V00[3], V32[3], V01[3], V33[3], V02[3], V34[3], V03[3], V35[3], V04[3], V36[3], V05[3], V37[3], V06[3], V38[3], V07[3], V39[3], T48[0], T49[0], T50[0], T51[0], T52[0], T53[0], T54[0], T55[0], T56[0], T57[0], T58[0], T59[0], T60[0], T61[0], T62[0], T63[0]);

        TRANSPOSE_16x16_16BIT_m256i(V08[0], V40[0], V09[0], V41[0], V10[0], V42[0], V11[0], V43[0], V12[0], V44[0], V13[0], V45[0], V14[0], V46[0], V15[0], V47[0], T00[1], T01[1], T02[1], T03[1], T04[1], T05[1], T06[1], T07[1], T08[1], T09[1], T10[1], T11[1], T12[1], T13[1], T14[1], T15[1]);
        TRANSPOSE_16x16_16BIT_m256i(V08[1], V40[1], V09[1], V41[1], V10[1], V42[1], V11[1], V43[1], V12[1], V44[1], V13[1], V45[1], V14[1], V46[1], V15[1], V47[1], T16[1], T17[1], T18[1], T19[1], T20[1], T21[1], T22[1], T23[1], T24[1], T25[1], T26[1], T27[1], T28[1], T29[1], T30[1], T31[1]);
        TRANSPOSE_16x16_16BIT_m256i(V08[2], V40[2], V09[2], V41[2], V10[2], V42[2], V11[2], V43[2], V12[2], V44[2], V13[2], V45[2], V14[2], V46[2], V15[2], V47[2], T32[1], T33[1], T34[1], T35[1], T36[1], T37[1], T38[1], T39[1], T40[1], T41[1], T42[1], T43[1], T44[1], T45[1], T46[1], T47[1]);
        TRANSPOSE_16x16_16BIT_m256i(V08[3], V40[3], V09[3], V41[3], V10[3], V42[3], V11[3], V43[3], V12[3], V44[3], V13[3], V45[3], V14[3], V46[3], V15[3], V47[3], T48[1], T49[1], T50[1], T51[1], T52[1], T53[1], T54[1], T55[1], T56[1], T57[1], T58[1], T59[1], T60[1], T61[1], T62[1], T63[1]);

        TRANSPOSE_16x16_16BIT_m256i(V16[0], V48[0], V17[0], V49[0], V18[0], V50[0], V19[0], V51[0], V20[0], V52[0], V21[0], V53[0], V22[0], V54[0], V23[0], V55[0], T00[2], T01[2], T02[2], T03[2], T04[2], T05[2], T06[2], T07[2], T08[2], T09[2], T10[2], T11[2], T12[2], T13[2], T14[2], T15[2]);
        TRANSPOSE_16x16_16BIT_m256i(V16[1], V48[1], V17[1], V49[1], V18[1], V50[1], V19[1], V51[1], V20[1], V52[1], V21[1], V53[1], V22[1], V54[1], V23[1], V55[1], T16[2], T17[2], T18[2], T19[2], T20[2], T21[2], T22[2], T23[2], T24[2], T25[2], T26[2], T27[2], T28[2], T29[2], T30[2], T31[2]);
        TRANSPOSE_16x16_16BIT_m256i(V16[2], V48[2], V17[2], V49[2], V18[2], V50[2], V19[2], V51[2], V20[2], V52[2], V21[2], V53[2], V22[2], V54[2], V23[2], V55[2], T32[2], T33[2], T34[2], T35[2], T36[2], T37[2], T38[2], T39[2], T40[2], T41[2], T42[2], T43[2], T44[2], T45[2], T46[2], T47[2]);
        TRANSPOSE_16x16_16BIT_m256i(V16[3], V48[3], V17[3], V49[3], V18[3], V50[3], V19[3], V51[3], V20[3], V52[3], V21[3], V53[3], V22[3], V54[3], V23[3], V55[3], T48[2], T49[2], T50[2], T51[2], T52[2], T53[2], T54[2], T55[2], T56[2], T57[2], T58[2], T59[2], T60[2], T61[2], T62[2], T63[2]);

        TRANSPOSE_16x16_16BIT_m256i(V24[0], V56[0], V25[0], V57[0], V26[0], V58[0], V27[0], V59[0], V28[0], V60[0], V29[0], V61[0], V30[0], V62[0], V31[0], V63[0], T00[3], T01[3], T02[3], T03[3], T04[3], T05[3], T06[3], T07[3], T08[3], T09[3], T10[3], T11[3], T12[3], T13[3], T14[3], T15[3]);
        TRANSPOSE_16x16_16BIT_m256i(V24[1], V56[1], V25[1], V57[1], V26[1], V58[1], V27[1], V59[1], V28[1], V60[1], V29[1], V61[1], V30[1], V62[1], V31[1], V63[1], T16[3], T17[3], T18[3], T19[3], T20[3], T21[3], T22[3], T23[3], T24[3], T25[3], T26[3], T27[3], T28[3], T29[3], T30[3], T31[3]);
        TRANSPOSE_16x16_16BIT_m256i(V24[2], V56[2], V25[2], V57[2], V26[2], V58[2], V27[2], V59[2], V28[2], V60[2], V29[2], V61[2], V30[2], V62[2], V31[2], V63[2], T32[3], T33[3], T34[3], T35[3], T36[3], T37[3], T38[3], T39[3], T40[3], T41[3], T42[3], T43[3], T44[3], T45[3], T46[3], T47[3]);
        TRANSPOSE_16x16_16BIT_m256i(V24[3], V56[3], V25[3], V57[3], V26[3], V58[3], V27[3], V59[3], V28[3], V60[3], V29[3], V61[3], V30[3], V62[3], V31[3], V63[3], T48[3], T49[3], T50[3], T51[3], T52[3], T53[3], T54[3], T55[3], T56[3], T57[3], T58[3], T59[3], T60[3], T61[3], T62[3], T63[3]);

        for (k = 0; k < 2; k++)
        {
            int offset = (k << 5);
            P00[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[0 + offset]), 0xD8);
            P01[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[1 * i_pred + offset]), 0xD8);
            P02[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[2 * i_pred + offset]), 0xD8);
            P03[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[3 * i_pred + offset]), 0xD8);
            P04[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[4 * i_pred + offset]), 0xD8);
            P05[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[5 * i_pred + offset]), 0xD8);
            P06[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[6 * i_pred + offset]), 0xD8);
            P07[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[7 * i_pred + offset]), 0xD8);
            P08[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[8 * i_pred + offset]), 0xD8);
            P09[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[9 * i_pred + offset]), 0xD8);

            P10[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[10 * i_pred + offset]), 0xD8);
            P11[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[11 * i_pred + offset]), 0xD8);
            P12[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[12 * i_pred + offset]), 0xD8);
            P13[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[13 * i_pred + offset]), 0xD8);
            P14[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[14 * i_pred + offset]), 0xD8);
            P15[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[15 * i_pred + offset]), 0xD8);
            P16[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[16 * i_pred + offset]), 0xD8);
            P17[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[17 * i_pred + offset]), 0xD8);
            P18[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[18 * i_pred + offset]), 0xD8);
            P19[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[19 * i_pred + offset]), 0xD8);


            P20[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[20 * i_pred + offset]), 0xD8);
            P21[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[21 * i_pred + offset]), 0xD8);
            P22[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[22 * i_pred + offset]), 0xD8);
            P23[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[23 * i_pred + offset]), 0xD8);
            P24[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[24 * i_pred + offset]), 0xD8);
            P25[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[25 * i_pred + offset]), 0xD8);
            P26[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[26 * i_pred + offset]), 0xD8);
            P27[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[27 * i_pred + offset]), 0xD8);
            P28[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[28 * i_pred + offset]), 0xD8);
            P29[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[29 * i_pred + offset]), 0xD8);

            P30[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[30 * i_pred + offset]), 0xD8);
            P31[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[31 * i_pred + offset]), 0xD8);
            P32[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[32 * i_pred + offset]), 0xD8);
            P33[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[33 * i_pred + offset]), 0xD8);
            P34[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[34 * i_pred + offset]), 0xD8);
            P35[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[35 * i_pred + offset]), 0xD8);
            P36[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[36 * i_pred + offset]), 0xD8);
            P37[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[37 * i_pred + offset]), 0xD8);
            P38[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[38 * i_pred + offset]), 0xD8);
            P39[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[39 * i_pred + offset]), 0xD8);

            P40[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[40 * i_pred + offset]), 0xD8);
            P41[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[41 * i_pred + offset]), 0xD8);
            P42[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[42 * i_pred + offset]), 0xD8);
            P43[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[43 * i_pred + offset]), 0xD8);
            P44[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[44 * i_pred + offset]), 0xD8);
            P45[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[45 * i_pred + offset]), 0xD8);
            P46[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[46 * i_pred + offset]), 0xD8);
            P47[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[47 * i_pred + offset]), 0xD8);
            P48[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[48 * i_pred + offset]), 0xD8);
            P49[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[49 * i_pred + offset]), 0xD8);

            P50[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[50 * i_pred + offset]), 0xD8);
            P51[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[51 * i_pred + offset]), 0xD8);
            P52[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[52 * i_pred + offset]), 0xD8);
            P53[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[53 * i_pred + offset]), 0xD8);
            P54[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[54 * i_pred + offset]), 0xD8);
            P55[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[55 * i_pred + offset]), 0xD8);
            P56[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[56 * i_pred + offset]), 0xD8);
            P57[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[57 * i_pred + offset]), 0xD8);
            P58[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[58 * i_pred + offset]), 0xD8);
            P59[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[59 * i_pred + offset]), 0xD8);

            P60[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[60 * i_pred + offset]), 0xD8);
            P61[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[61 * i_pred + offset]), 0xD8);
            P62[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[62 * i_pred + offset]), 0xD8);
            P63[k] = _mm256_permute4x64_epi64(_mm256_lddqu_si256((const __m256i*)&pred[63 * i_pred + offset]), 0xD8);
        }



        for (k = 0; k < 2; k++)
        {
            int offset = k << 1;

            V00[0 + offset] = _mm256_unpacklo_epi8(P00[k], zero);
            V00[1 + offset] = _mm256_unpackhi_epi8(P00[k], zero);
            V01[0 + offset] = _mm256_unpacklo_epi8(P01[k], zero);
            V01[1 + offset] = _mm256_unpackhi_epi8(P01[k], zero);
            V02[0 + offset] = _mm256_unpacklo_epi8(P02[k], zero);
            V02[1 + offset] = _mm256_unpackhi_epi8(P02[k], zero);
            V03[0 + offset] = _mm256_unpacklo_epi8(P03[k], zero);
            V03[1 + offset] = _mm256_unpackhi_epi8(P03[k], zero);
            V04[0 + offset] = _mm256_unpacklo_epi8(P04[k], zero);
            V04[1 + offset] = _mm256_unpackhi_epi8(P04[k], zero);
            V05[0 + offset] = _mm256_unpacklo_epi8(P05[k], zero);
            V05[1 + offset] = _mm256_unpackhi_epi8(P05[k], zero);
            V06[0 + offset] = _mm256_unpacklo_epi8(P06[k], zero);
            V06[1 + offset] = _mm256_unpackhi_epi8(P06[k], zero);
            V07[0 + offset] = _mm256_unpacklo_epi8(P07[k], zero);
            V07[1 + offset] = _mm256_unpackhi_epi8(P07[k], zero);
            V08[0 + offset] = _mm256_unpacklo_epi8(P08[k], zero);
            V08[1 + offset] = _mm256_unpackhi_epi8(P08[k], zero);
            V09[0 + offset] = _mm256_unpacklo_epi8(P09[k], zero);
            V09[1 + offset] = _mm256_unpackhi_epi8(P09[k], zero);
            V10[0 + offset] = _mm256_unpacklo_epi8(P10[k], zero);
            V10[1 + offset] = _mm256_unpackhi_epi8(P10[k], zero);
            V11[0 + offset] = _mm256_unpacklo_epi8(P11[k], zero);
            V11[1 + offset] = _mm256_unpackhi_epi8(P11[k], zero);
            V12[0 + offset] = _mm256_unpacklo_epi8(P12[k], zero);
            V12[1 + offset] = _mm256_unpackhi_epi8(P12[k], zero);
            V13[0 + offset] = _mm256_unpacklo_epi8(P13[k], zero);
            V13[1 + offset] = _mm256_unpackhi_epi8(P13[k], zero);
            V14[0 + offset] = _mm256_unpacklo_epi8(P14[k], zero);
            V14[1 + offset] = _mm256_unpackhi_epi8(P14[k], zero);
            V15[0 + offset] = _mm256_unpacklo_epi8(P15[k], zero);
            V15[1 + offset] = _mm256_unpackhi_epi8(P15[k], zero);
            V16[0 + offset] = _mm256_unpacklo_epi8(P16[k], zero);
            V16[1 + offset] = _mm256_unpackhi_epi8(P16[k], zero);
            V17[0 + offset] = _mm256_unpacklo_epi8(P17[k], zero);
            V17[1 + offset] = _mm256_unpackhi_epi8(P17[k], zero);
            V18[0 + offset] = _mm256_unpacklo_epi8(P18[k], zero);
            V18[1 + offset] = _mm256_unpackhi_epi8(P18[k], zero);
            V19[0 + offset] = _mm256_unpacklo_epi8(P19[k], zero);
            V19[1 + offset] = _mm256_unpackhi_epi8(P19[k], zero);
            V20[0 + offset] = _mm256_unpacklo_epi8(P20[k], zero);
            V20[1 + offset] = _mm256_unpackhi_epi8(P20[k], zero);
            V21[0 + offset] = _mm256_unpacklo_epi8(P21[k], zero);
            V21[1 + offset] = _mm256_unpackhi_epi8(P21[k], zero);
            V22[0 + offset] = _mm256_unpacklo_epi8(P22[k], zero);
            V22[1 + offset] = _mm256_unpackhi_epi8(P22[k], zero);
            V23[0 + offset] = _mm256_unpacklo_epi8(P23[k], zero);
            V23[1 + offset] = _mm256_unpackhi_epi8(P23[k], zero);
            V24[0 + offset] = _mm256_unpacklo_epi8(P24[k], zero);
            V24[1 + offset] = _mm256_unpackhi_epi8(P24[k], zero);
            V25[0 + offset] = _mm256_unpacklo_epi8(P25[k], zero);
            V25[1 + offset] = _mm256_unpackhi_epi8(P25[k], zero);
            V26[0 + offset] = _mm256_unpacklo_epi8(P26[k], zero);
            V26[1 + offset] = _mm256_unpackhi_epi8(P26[k], zero);
            V27[0 + offset] = _mm256_unpacklo_epi8(P27[k], zero);
            V27[1 + offset] = _mm256_unpackhi_epi8(P27[k], zero);
            V28[0 + offset] = _mm256_unpacklo_epi8(P28[k], zero);
            V28[1 + offset] = _mm256_unpackhi_epi8(P28[k], zero);
            V29[0 + offset] = _mm256_unpacklo_epi8(P29[k], zero);
            V29[1 + offset] = _mm256_unpackhi_epi8(P29[k], zero);

            V30[0 + offset] = _mm256_unpacklo_epi8(P30[k], zero);
            V30[1 + offset] = _mm256_unpackhi_epi8(P30[k], zero);
            V31[0 + offset] = _mm256_unpacklo_epi8(P31[k], zero);
            V31[1 + offset] = _mm256_unpackhi_epi8(P31[k], zero);
            V32[0 + offset] = _mm256_unpacklo_epi8(P32[k], zero);
            V32[1 + offset] = _mm256_unpackhi_epi8(P32[k], zero);
            V33[0 + offset] = _mm256_unpacklo_epi8(P33[k], zero);
            V33[1 + offset] = _mm256_unpackhi_epi8(P33[k], zero);
            V34[0 + offset] = _mm256_unpacklo_epi8(P34[k], zero);
            V34[1 + offset] = _mm256_unpackhi_epi8(P34[k], zero);
            V35[0 + offset] = _mm256_unpacklo_epi8(P35[k], zero);
            V35[1 + offset] = _mm256_unpackhi_epi8(P35[k], zero);
            V36[0 + offset] = _mm256_unpacklo_epi8(P36[k], zero);
            V36[1 + offset] = _mm256_unpackhi_epi8(P36[k], zero);
            V37[0 + offset] = _mm256_unpacklo_epi8(P37[k], zero);
            V37[1 + offset] = _mm256_unpackhi_epi8(P37[k], zero);
            V38[0 + offset] = _mm256_unpacklo_epi8(P38[k], zero);
            V38[1 + offset] = _mm256_unpackhi_epi8(P38[k], zero);
            V39[0 + offset] = _mm256_unpacklo_epi8(P39[k], zero);
            V39[1 + offset] = _mm256_unpackhi_epi8(P39[k], zero);

            V40[0 + offset] = _mm256_unpacklo_epi8(P40[k], zero);
            V40[1 + offset] = _mm256_unpackhi_epi8(P40[k], zero);
            V41[0 + offset] = _mm256_unpacklo_epi8(P41[k], zero);
            V41[1 + offset] = _mm256_unpackhi_epi8(P41[k], zero);
            V42[0 + offset] = _mm256_unpacklo_epi8(P42[k], zero);
            V42[1 + offset] = _mm256_unpackhi_epi8(P42[k], zero);
            V43[0 + offset] = _mm256_unpacklo_epi8(P43[k], zero);
            V43[1 + offset] = _mm256_unpackhi_epi8(P43[k], zero);
            V44[0 + offset] = _mm256_unpacklo_epi8(P44[k], zero);
            V44[1 + offset] = _mm256_unpackhi_epi8(P44[k], zero);
            V45[0 + offset] = _mm256_unpacklo_epi8(P45[k], zero);
            V45[1 + offset] = _mm256_unpackhi_epi8(P45[k], zero);
            V46[0 + offset] = _mm256_unpacklo_epi8(P46[k], zero);
            V46[1 + offset] = _mm256_unpackhi_epi8(P46[k], zero);
            V47[0 + offset] = _mm256_unpacklo_epi8(P47[k], zero);
            V47[1 + offset] = _mm256_unpackhi_epi8(P47[k], zero);
            V48[0 + offset] = _mm256_unpacklo_epi8(P48[k], zero);
            V48[1 + offset] = _mm256_unpackhi_epi8(P48[k], zero);
            V49[0 + offset] = _mm256_unpacklo_epi8(P49[k], zero);
            V49[1 + offset] = _mm256_unpackhi_epi8(P49[k], zero);

            V50[0 + offset] = _mm256_unpacklo_epi8(P50[k], zero);
            V50[1 + offset] = _mm256_unpackhi_epi8(P50[k], zero);
            V51[0 + offset] = _mm256_unpacklo_epi8(P51[k], zero);
            V51[1 + offset] = _mm256_unpackhi_epi8(P51[k], zero);
            V52[0 + offset] = _mm256_unpacklo_epi8(P52[k], zero);
            V52[1 + offset] = _mm256_unpackhi_epi8(P52[k], zero);
            V53[0 + offset] = _mm256_unpacklo_epi8(P53[k], zero);
            V53[1 + offset] = _mm256_unpackhi_epi8(P53[k], zero);
            V54[0 + offset] = _mm256_unpacklo_epi8(P54[k], zero);
            V54[1 + offset] = _mm256_unpackhi_epi8(P54[k], zero);
            V55[0 + offset] = _mm256_unpacklo_epi8(P55[k], zero);
            V55[1 + offset] = _mm256_unpackhi_epi8(P55[k], zero);
            V56[0 + offset] = _mm256_unpacklo_epi8(P56[k], zero);
            V56[1 + offset] = _mm256_unpackhi_epi8(P56[k], zero);
            V57[0 + offset] = _mm256_unpacklo_epi8(P57[k], zero);
            V57[1 + offset] = _mm256_unpackhi_epi8(P57[k], zero);
            V58[0 + offset] = _mm256_unpacklo_epi8(P58[k], zero);
            V58[1 + offset] = _mm256_unpackhi_epi8(P58[k], zero);
            V59[0 + offset] = _mm256_unpacklo_epi8(P59[k], zero);
            V59[1 + offset] = _mm256_unpackhi_epi8(P59[k], zero);

            V60[0 + offset] = _mm256_unpacklo_epi8(P60[k], zero);
            V60[1 + offset] = _mm256_unpackhi_epi8(P60[k], zero);
            V61[0 + offset] = _mm256_unpacklo_epi8(P61[k], zero);
            V61[1 + offset] = _mm256_unpackhi_epi8(P61[k], zero);
            V62[0 + offset] = _mm256_unpacklo_epi8(P62[k], zero);
            V62[1 + offset] = _mm256_unpackhi_epi8(P62[k], zero);
            V63[0 + offset] = _mm256_unpacklo_epi8(P63[k], zero);
            V63[1 + offset] = _mm256_unpackhi_epi8(P63[k], zero);
        }


        for (k = 0; k < 4; k++)
        {
            T00[k] = _mm256_add_epi16(V00[k], T00[k]);
            T01[k] = _mm256_add_epi16(V01[k], T01[k]);
            T02[k] = _mm256_add_epi16(V02[k], T02[k]);
            T03[k] = _mm256_add_epi16(V03[k], T03[k]);
            T04[k] = _mm256_add_epi16(V04[k], T04[k]);
            T05[k] = _mm256_add_epi16(V05[k], T05[k]);
            T06[k] = _mm256_add_epi16(V06[k], T06[k]);
            T07[k] = _mm256_add_epi16(V07[k], T07[k]);
            T08[k] = _mm256_add_epi16(V08[k], T08[k]);
            T09[k] = _mm256_add_epi16(V09[k], T09[k]);
            T10[k] = _mm256_add_epi16(V10[k], T10[k]);
            T11[k] = _mm256_add_epi16(V11[k], T11[k]);
            T12[k] = _mm256_add_epi16(V12[k], T12[k]);
            T13[k] = _mm256_add_epi16(V13[k], T13[k]);
            T14[k] = _mm256_add_epi16(V14[k], T14[k]);
            T15[k] = _mm256_add_epi16(V15[k], T15[k]);
            T16[k] = _mm256_add_epi16(V16[k], T16[k]);
            T17[k] = _mm256_add_epi16(V17[k], T17[k]);
            T18[k] = _mm256_add_epi16(V18[k], T18[k]);
            T19[k] = _mm256_add_epi16(V19[k], T19[k]);
            T20[k] = _mm256_add_epi16(V20[k], T20[k]);
            T21[k] = _mm256_add_epi16(V21[k], T21[k]);
            T22[k] = _mm256_add_epi16(V22[k], T22[k]);
            T23[k] = _mm256_add_epi16(V23[k], T23[k]);
            T24[k] = _mm256_add_epi16(V24[k], T24[k]);
            T25[k] = _mm256_add_epi16(V25[k], T25[k]);
            T26[k] = _mm256_add_epi16(V26[k], T26[k]);
            T27[k] = _mm256_add_epi16(V27[k], T27[k]);
            T28[k] = _mm256_add_epi16(V28[k], T28[k]);
            T29[k] = _mm256_add_epi16(V29[k], T29[k]);

            T30[k] = _mm256_add_epi16(V30[k], T30[k]);
            T31[k] = _mm256_add_epi16(V31[k], T31[k]);
            T32[k] = _mm256_add_epi16(V32[k], T32[k]);
            T33[k] = _mm256_add_epi16(V33[k], T33[k]);
            T34[k] = _mm256_add_epi16(V34[k], T34[k]);
            T35[k] = _mm256_add_epi16(V35[k], T35[k]);
            T36[k] = _mm256_add_epi16(V36[k], T36[k]);
            T37[k] = _mm256_add_epi16(V37[k], T37[k]);
            T38[k] = _mm256_add_epi16(V38[k], T38[k]);
            T39[k] = _mm256_add_epi16(V39[k], T39[k]);

            T40[k] = _mm256_add_epi16(V40[k], T40[k]);
            T41[k] = _mm256_add_epi16(V41[k], T41[k]);
            T42[k] = _mm256_add_epi16(V42[k], T42[k]);
            T43[k] = _mm256_add_epi16(V43[k], T43[k]);
            T44[k] = _mm256_add_epi16(V44[k], T44[k]);
            T45[k] = _mm256_add_epi16(V45[k], T45[k]);
            T46[k] = _mm256_add_epi16(V46[k], T46[k]);
            T47[k] = _mm256_add_epi16(V47[k], T47[k]);
            T48[k] = _mm256_add_epi16(V48[k], T48[k]);
            T49[k] = _mm256_add_epi16(V49[k], T49[k]);

            T50[k] = _mm256_add_epi16(V50[k], T50[k]);
            T51[k] = _mm256_add_epi16(V51[k], T51[k]);
            T52[k] = _mm256_add_epi16(V52[k], T52[k]);
            T53[k] = _mm256_add_epi16(V53[k], T53[k]);
            T54[k] = _mm256_add_epi16(V54[k], T54[k]);
            T55[k] = _mm256_add_epi16(V55[k], T55[k]);
            T56[k] = _mm256_add_epi16(V56[k], T56[k]);
            T57[k] = _mm256_add_epi16(V57[k], T57[k]);
            T58[k] = _mm256_add_epi16(V58[k], T58[k]);
            T59[k] = _mm256_add_epi16(V59[k], T59[k]);

            T60[k] = _mm256_add_epi16(V60[k], T60[k]);
            T61[k] = _mm256_add_epi16(V61[k], T61[k]);
            T62[k] = _mm256_add_epi16(V62[k], T62[k]);
            T63[k] = _mm256_add_epi16(V63[k], T63[k]);

        }

        for (k = 0; k < 4; k += 2)
        {
            V00[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T00[k], T00[k + 1]), 0xD8);
            V01[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T01[k], T01[k + 1]), 0xD8);
            V02[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T02[k], T02[k + 1]), 0xD8);
            V03[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T03[k], T03[k + 1]), 0xD8);
            V04[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T04[k], T04[k + 1]), 0xD8);
            V05[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T05[k], T05[k + 1]), 0xD8);
            V06[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T06[k], T06[k + 1]), 0xD8);
            V07[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T07[k], T07[k + 1]), 0xD8);
            V08[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T08[k], T08[k + 1]), 0xD8);
            V09[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T09[k], T09[k + 1]), 0xD8);
            V10[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T10[k], T10[k + 1]), 0xD8);
            V11[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T11[k], T11[k + 1]), 0xD8);
            V12[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T12[k], T12[k + 1]), 0xD8);
            V13[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T13[k], T13[k + 1]), 0xD8);
            V14[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T14[k], T14[k + 1]), 0xD8);
            V15[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T15[k], T15[k + 1]), 0xD8);
            V16[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T16[k], T16[k + 1]), 0xD8);
            V17[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T17[k], T17[k + 1]), 0xD8);
            V18[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T18[k], T18[k + 1]), 0xD8);
            V19[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T19[k], T19[k + 1]), 0xD8);
            V20[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T20[k], T20[k + 1]), 0xD8);
            V21[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T21[k], T21[k + 1]), 0xD8);
            V22[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T22[k], T22[k + 1]), 0xD8);
            V23[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T23[k], T23[k + 1]), 0xD8);
            V24[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T24[k], T24[k + 1]), 0xD8);
            V25[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T25[k], T25[k + 1]), 0xD8);
            V26[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T26[k], T26[k + 1]), 0xD8);
            V27[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T27[k], T27[k + 1]), 0xD8);
            V28[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T28[k], T28[k + 1]), 0xD8);
            V29[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T29[k], T29[k + 1]), 0xD8);
            V30[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T30[k], T30[k + 1]), 0xD8);
            V31[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T31[k], T31[k + 1]), 0xD8);
            V32[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T32[k], T32[k + 1]), 0xD8);
            V33[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T33[k], T33[k + 1]), 0xD8);
            V34[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T34[k], T34[k + 1]), 0xD8);
            V35[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T35[k], T35[k + 1]), 0xD8);
            V36[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T36[k], T36[k + 1]), 0xD8);
            V37[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T37[k], T37[k + 1]), 0xD8);
            V38[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T38[k], T38[k + 1]), 0xD8);
            V39[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T39[k], T39[k + 1]), 0xD8);
            V40[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T40[k], T40[k + 1]), 0xD8);
            V41[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T41[k], T41[k + 1]), 0xD8);
            V42[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T42[k], T42[k + 1]), 0xD8);
            V43[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T43[k], T43[k + 1]), 0xD8);
            V44[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T44[k], T44[k + 1]), 0xD8);
            V45[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T45[k], T45[k + 1]), 0xD8);
            V46[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T46[k], T46[k + 1]), 0xD8);
            V47[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T47[k], T47[k + 1]), 0xD8);
            V48[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T48[k], T48[k + 1]), 0xD8);
            V49[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T49[k], T49[k + 1]), 0xD8);
            V50[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T50[k], T50[k + 1]), 0xD8);
            V51[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T51[k], T51[k + 1]), 0xD8);
            V52[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T52[k], T52[k + 1]), 0xD8);
            V53[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T53[k], T53[k + 1]), 0xD8);
            V54[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T54[k], T54[k + 1]), 0xD8);
            V55[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T55[k], T55[k + 1]), 0xD8);
            V56[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T56[k], T56[k + 1]), 0xD8);
            V57[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T57[k], T57[k + 1]), 0xD8);
            V58[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T58[k], T58[k + 1]), 0xD8);
            V59[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T59[k], T59[k + 1]), 0xD8);
            V60[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T60[k], T60[k + 1]), 0xD8);
            V61[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T61[k], T61[k + 1]), 0xD8);
            V62[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T62[k], T62[k + 1]), 0xD8);
            V63[k] = _mm256_permute4x64_epi64(_mm256_packus_epi16(T63[k], T63[k + 1]), 0xD8);
        }


        for (k = 0; k < 4; k += 2)
        {
            int offset = k << 4;
            _mm256_storeu_si256((__m256i*)&dst[0 + offset], V00[k]);
            _mm256_storeu_si256((__m256i*)&dst[1 * i_dst + offset], V01[k]);
            _mm256_storeu_si256((__m256i*)&dst[2 * i_dst + offset], V02[k]);
            _mm256_storeu_si256((__m256i*)&dst[3 * i_dst + offset], V03[k]);
            _mm256_storeu_si256((__m256i*)&dst[4 * i_dst + offset], V04[k]);
            _mm256_storeu_si256((__m256i*)&dst[5 * i_dst + offset], V05[k]);
            _mm256_storeu_si256((__m256i*)&dst[6 * i_dst + offset], V06[k]);
            _mm256_storeu_si256((__m256i*)&dst[7 * i_dst + offset], V07[k]);
            _mm256_storeu_si256((__m256i*)&dst[8 * i_dst + offset], V08[k]);
            _mm256_storeu_si256((__m256i*)&dst[9 * i_dst + offset], V09[k]);

            _mm256_storeu_si256((__m256i*)&dst[10 * i_dst + offset], V10[k]);
            _mm256_storeu_si256((__m256i*)&dst[11 * i_dst + offset], V11[k]);
            _mm256_storeu_si256((__m256i*)&dst[12 * i_dst + offset], V12[k]);
            _mm256_storeu_si256((__m256i*)&dst[13 * i_dst + offset], V13[k]);
            _mm256_storeu_si256((__m256i*)&dst[14 * i_dst + offset], V14[k]);
            _mm256_storeu_si256((__m256i*)&dst[15 * i_dst + offset], V15[k]);
            _mm256_storeu_si256((__m256i*)&dst[16 * i_dst + offset], V16[k]);
            _mm256_storeu_si256((__m256i*)&dst[17 * i_dst + offset], V17[k]);
            _mm256_storeu_si256((__m256i*)&dst[18 * i_dst + offset], V18[k]);
            _mm256_storeu_si256((__m256i*)&dst[19 * i_dst + offset], V19[k]);

            _mm256_storeu_si256((__m256i*)&dst[20 * i_dst + offset], V20[k]);
            _mm256_storeu_si256((__m256i*)&dst[21 * i_dst + offset], V21[k]);
            _mm256_storeu_si256((__m256i*)&dst[22 * i_dst + offset], V22[k]);
            _mm256_storeu_si256((__m256i*)&dst[23 * i_dst + offset], V23[k]);
            _mm256_storeu_si256((__m256i*)&dst[24 * i_dst + offset], V24[k]);
            _mm256_storeu_si256((__m256i*)&dst[25 * i_dst + offset], V25[k]);
            _mm256_storeu_si256((__m256i*)&dst[26 * i_dst + offset], V26[k]);
            _mm256_storeu_si256((__m256i*)&dst[27 * i_dst + offset], V27[k]);
            _mm256_storeu_si256((__m256i*)&dst[28 * i_dst + offset], V28[k]);
            _mm256_storeu_si256((__m256i*)&dst[29 * i_dst + offset], V29[k]);

            _mm256_storeu_si256((__m256i*)&dst[30 * i_dst + offset], V30[k]);
            _mm256_storeu_si256((__m256i*)&dst[31 * i_dst + offset], V31[k]);
            _mm256_storeu_si256((__m256i*)&dst[32 * i_dst + offset], V32[k]);
            _mm256_storeu_si256((__m256i*)&dst[33 * i_dst + offset], V33[k]);
            _mm256_storeu_si256((__m256i*)&dst[34 * i_dst + offset], V34[k]);
            _mm256_storeu_si256((__m256i*)&dst[35 * i_dst + offset], V35[k]);
            _mm256_storeu_si256((__m256i*)&dst[36 * i_dst + offset], V36[k]);
            _mm256_storeu_si256((__m256i*)&dst[37 * i_dst + offset], V37[k]);
            _mm256_storeu_si256((__m256i*)&dst[38 * i_dst + offset], V38[k]);
            _mm256_storeu_si256((__m256i*)&dst[39 * i_dst + offset], V39[k]);

            _mm256_storeu_si256((__m256i*)&dst[40 * i_dst + offset], V40[k]);
            _mm256_storeu_si256((__m256i*)&dst[41 * i_dst + offset], V41[k]);
            _mm256_storeu_si256((__m256i*)&dst[42 * i_dst + offset], V42[k]);
            _mm256_storeu_si256((__m256i*)&dst[43 * i_dst + offset], V43[k]);
            _mm256_storeu_si256((__m256i*)&dst[44 * i_dst + offset], V44[k]);
            _mm256_storeu_si256((__m256i*)&dst[45 * i_dst + offset], V45[k]);
            _mm256_storeu_si256((__m256i*)&dst[46 * i_dst + offset], V46[k]);
            _mm256_storeu_si256((__m256i*)&dst[47 * i_dst + offset], V47[k]);
            _mm256_storeu_si256((__m256i*)&dst[48 * i_dst + offset], V48[k]);
            _mm256_storeu_si256((__m256i*)&dst[49 * i_dst + offset], V49[k]);

            _mm256_storeu_si256((__m256i*)&dst[50 * i_dst + offset], V50[k]);
            _mm256_storeu_si256((__m256i*)&dst[51 * i_dst + offset], V51[k]);
            _mm256_storeu_si256((__m256i*)&dst[52 * i_dst + offset], V52[k]);
            _mm256_storeu_si256((__m256i*)&dst[53 * i_dst + offset], V53[k]);
            _mm256_storeu_si256((__m256i*)&dst[54 * i_dst + offset], V54[k]);
            _mm256_storeu_si256((__m256i*)&dst[55 * i_dst + offset], V55[k]);
            _mm256_storeu_si256((__m256i*)&dst[56 * i_dst + offset], V56[k]);
            _mm256_storeu_si256((__m256i*)&dst[57 * i_dst + offset], V57[k]);
            _mm256_storeu_si256((__m256i*)&dst[58 * i_dst + offset], V58[k]);
            _mm256_storeu_si256((__m256i*)&dst[59 * i_dst + offset], V59[k]);

            _mm256_storeu_si256((__m256i*)&dst[60 * i_dst + offset], V60[k]);
            _mm256_storeu_si256((__m256i*)&dst[61 * i_dst + offset], V61[k]);
            _mm256_storeu_si256((__m256i*)&dst[62 * i_dst + offset], V62[k]);
            _mm256_storeu_si256((__m256i*)&dst[63 * i_dst + offset], V63[k]);
        }
    }
}

void uavs3e_itrans_dct8_pb4_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)
{
    int i = 0;
    char *iT = com_tbl_tm4[DCT8][0];
    __m256i factor = _mm256_set1_epi32(1 << (shift - 1));
    __m256i s0, s1, s2, s3;	//源数据
    __m256i c0, c1, c2, c3;	//系数矩阵
    __m256i zero = _mm256_setzero_si256();
    __m256i e0, e1, e2, e3;
    __m256i v0, v1, v2, v3;
    __m256i r0;
    __m256i mask = _mm256_set_epi64x(0, 0, -1, -1);

    for (i; i < line; i++)
    {
        //load coef data
        //char->coef_t
        c0 = _mm256_loadu_si256((__m256i*)(iT));
        c0 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c0));
        c1 = _mm256_loadu_si256((__m256i*)(iT + 4));
        c1 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c1));
        c2 = _mm256_loadu_si256((__m256i*)(iT + 8));
        c2 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c2));
        c3 = _mm256_loadu_si256((__m256i*)(iT + 12));
        c3 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c3));
        c0 = _mm256_unpacklo_epi16(c0, zero);
        c1 = _mm256_unpacklo_epi16(c1, zero);
        c2 = _mm256_unpacklo_epi16(c2, zero);
        c3 = _mm256_unpacklo_epi16(c3, zero);

        //load src
        s0 = _mm256_set1_epi16(coeff[0]);
        s1 = _mm256_set1_epi16(coeff[line]);
        s2 = _mm256_set1_epi16(coeff[2 * line]);
        s3 = _mm256_set1_epi16(coeff[3 * line]);

        v0 = _mm256_madd_epi16(s0, c0);
        v1 = _mm256_madd_epi16(s1, c1);
        v2 = _mm256_madd_epi16(s2, c2);
        v3 = _mm256_madd_epi16(s3, c3);

        e0 = _mm256_add_epi32(v0, v1);
        e1 = _mm256_add_epi32(v2, v3);
        e2 = _mm256_add_epi32(e0, e1);
        e3 = _mm256_add_epi32(e2, factor);

        r0 = _mm256_packs_epi32(_mm256_srai_epi32(e3, shift), zero);
        _mm256_maskstore_epi64((i64s_t*)block, mask, r0);
        coeff++;
        block += 4;
    }
}

void uavs3e_itrans_dct8_pb8_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)
{
    int i;
    char *iT = com_tbl_tm8[DCT8][0];
    __m256i s0, s1, s2, s3, s4, s5, s6, s7;
    __m256i c0, c1, c2, c3, c4, c5, c6, c7;
    __m256i e0, e1, e2, e3, e4, e5, e6;
    __m256i r0, r1, r2;
    __m256i zero = _mm256_setzero_si256();
    __m256i rnd_factor = _mm256_set1_epi32(1 << (shift - 1));
    __m256i mask = _mm256_set_epi64x(0, 0, -1, -1);

    for (i = 0; i < line; i++)
    {
        s0 = _mm256_set1_epi32(coeff[0]);
        s1 = _mm256_set1_epi32(coeff[line]);
        s2 = _mm256_set1_epi32(coeff[2 * line]);
        s3 = _mm256_set1_epi32(coeff[3 * line]);
        s4 = _mm256_set1_epi32(coeff[4 * line]);
        s5 = _mm256_set1_epi32(coeff[5 * line]);
        s6 = _mm256_set1_epi32(coeff[6 * line]);
        s7 = _mm256_set1_epi32(coeff[7 * line]);

        c0 = _mm256_loadu_si256((__m256i*)iT);
        c0 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c0));
        c1 = _mm256_loadu_si256((__m256i*)(iT + 8));
        c1 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c1));
        c2 = _mm256_loadu_si256((__m256i*)(iT + 16));
        c2 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c2));
        c3 = _mm256_loadu_si256((__m256i*)(iT + 24));
        c3 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c3));
        c4 = _mm256_loadu_si256((__m256i*)(iT + 32));
        c4 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c4));
        c5 = _mm256_loadu_si256((__m256i*)(iT + 40));
        c5 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c5));
        c6 = _mm256_loadu_si256((__m256i*)(iT + 48));
        c6 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c6));
        c7 = _mm256_loadu_si256((__m256i*)(iT + 56));
        c7 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c7));

        e0 = _mm256_add_epi32(_mm256_mullo_epi32(s0, c0), _mm256_mullo_epi32(s1, c1));
        e1 = _mm256_add_epi32(_mm256_mullo_epi32(s2, c2), _mm256_mullo_epi32(s3, c3));
        e2 = _mm256_add_epi32(_mm256_mullo_epi32(s4, c4), _mm256_mullo_epi32(s5, c5));
        e3 = _mm256_add_epi32(_mm256_mullo_epi32(s6, c6), _mm256_mullo_epi32(s7, c7));
        e4 = _mm256_add_epi32(e0, e1);
        e5 = _mm256_add_epi32(e2, e3);
        e6 = _mm256_add_epi32(e4, e5);

        r0 = _mm256_add_epi32(e6, rnd_factor);
        r1 = _mm256_packs_epi32(_mm256_srai_epi32(r0, shift), zero);
        r2 = _mm256_permute4x64_epi64(r1, 0x08);
        _mm256_maskstore_epi64((i64s_t*)block, mask, r2);

        coeff++;
        block += 8;
    }
}

void uavs3e_itrans_dct8_pb16_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)
{
    int i, j, k;
    char *iT = com_tbl_tm16[DCT8][0];
    __m256i s0, s1, s2, s3, s4, s5, s6, s7;
    __m256i c0, c1, c2, c3, c4, c5, c6, c7;
    __m256i e0, e1, e2, e3, e4, e5, e6;
    __m256i r0, r1, r2;
    __m256i mask = _mm256_set_epi64x(0, 0, -1, -1);
    __m256i zero = _mm256_setzero_si256();
    __m256i rnd_factor = _mm256_set1_epi32(1 << (shift - 1));
    for (i = 0; i < line; i++)
    {
        char *pb16 = iT;
        for (j = 0; j < 2; j++)
        {
            __m256i v0 = zero;
            for (k = 0; k < 2; k++)
            {
                s0 = _mm256_set1_epi32(coeff[0]);
                s1 = _mm256_set1_epi32(coeff[line]);
                s2 = _mm256_set1_epi32(coeff[2 * line]);
                s3 = _mm256_set1_epi32(coeff[3 * line]);
                s4 = _mm256_set1_epi32(coeff[4 * line]);
                s5 = _mm256_set1_epi32(coeff[5 * line]);
                s6 = _mm256_set1_epi32(coeff[6 * line]);
                s7 = _mm256_set1_epi32(coeff[7 * line]);

                c0 = _mm256_loadu_si256((__m256i*)pb16);
                c0 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c0));
                c1 = _mm256_loadu_si256((__m256i*)(pb16 + 16));
                c1 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c1));
                c2 = _mm256_loadu_si256((__m256i*)(pb16 + 32));
                c2 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c2));
                c3 = _mm256_loadu_si256((__m256i*)(pb16 + 48));
                c3 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c3));
                c4 = _mm256_loadu_si256((__m256i*)(pb16 + 64));
                c4 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c4));
                c5 = _mm256_loadu_si256((__m256i*)(pb16 + 80));
                c5 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c5));
                c6 = _mm256_loadu_si256((__m256i*)(pb16 + 96));
                c6 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c6));
                c7 = _mm256_loadu_si256((__m256i*)(pb16 + 112));
                c7 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c7));

                e0 = _mm256_add_epi32(_mm256_mullo_epi32(s0, c0), _mm256_mullo_epi32(s1, c1));
                e1 = _mm256_add_epi32(_mm256_mullo_epi32(s2, c2), _mm256_mullo_epi32(s3, c3));
                e2 = _mm256_add_epi32(_mm256_mullo_epi32(s4, c4), _mm256_mullo_epi32(s5, c5));
                e3 = _mm256_add_epi32(_mm256_mullo_epi32(s6, c6), _mm256_mullo_epi32(s7, c7));
                e4 = _mm256_add_epi32(e0, e1);
                e5 = _mm256_add_epi32(e2, e3);
                e6 = _mm256_add_epi32(e4, e5);

                v0 = _mm256_add_epi32(v0, e6);

                pb16 += 128;
                coeff += (8 * line);
            }
            r0 = _mm256_add_epi32(v0, rnd_factor);
            r1 = _mm256_packs_epi32(_mm256_srai_epi32(r0, shift), zero);
            r2 = _mm256_permute4x64_epi64(r1, 0x08);
            _mm256_maskstore_epi64((i64s_t*)block, mask, r2);
            coeff -= (16 * line);
            pb16 -= 248;
            block += 8;
        }
        coeff++;
    }
}

void uavs3e_itrans_dst7_pb4_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)
{
    int i = 0;
    char *iT = com_tbl_tm4[DST7][0];
    __m256i factor = _mm256_set1_epi32(1 << (shift - 1));
    __m256i s0, s1, s2, s3;	//源数据
    __m256i c0, c1, c2, c3;	//系数矩阵
    __m256i zero = _mm256_setzero_si256();
    __m256i e0, e1, e2, e3;
    __m256i v0, v1, v2, v3;
    __m256i r0;
    __m256i mask = _mm256_set_epi64x(0, 0, -1, -1);

    for (i; i < line; i++)
    {
        //load coef data
        //char->coef_t
        c0 = _mm256_loadu_si256((__m256i*)(iT));
        c0 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c0));
        c1 = _mm256_loadu_si256((__m256i*)(iT + 4));
        c1 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c1));
        c2 = _mm256_loadu_si256((__m256i*)(iT + 8));
        c2 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c2));
        c3 = _mm256_loadu_si256((__m256i*)(iT + 12));
        c3 = _mm256_cvtepi8_epi16(_mm256_castsi256_si128(c3));
        c0 = _mm256_unpacklo_epi16(c0, zero);
        c1 = _mm256_unpacklo_epi16(c1, zero);
        c2 = _mm256_unpacklo_epi16(c2, zero);
        c3 = _mm256_unpacklo_epi16(c3, zero);

        //load src
        s0 = _mm256_set1_epi16(coeff[0]);
        s1 = _mm256_set1_epi16(coeff[line]);
        s2 = _mm256_set1_epi16(coeff[2 * line]);
        s3 = _mm256_set1_epi16(coeff[3 * line]);

        v0 = _mm256_madd_epi16(s0, c0);
        v1 = _mm256_madd_epi16(s1, c1);
        v2 = _mm256_madd_epi16(s2, c2);
        v3 = _mm256_madd_epi16(s3, c3);

        e0 = _mm256_add_epi32(v0, v1);
        e1 = _mm256_add_epi32(v2, v3);
        e2 = _mm256_add_epi32(e0, e1);
        e3 = _mm256_add_epi32(e2, factor);

        r0 = _mm256_packs_epi32(_mm256_srai_epi32(e3, shift), zero);
        _mm256_maskstore_epi64((i64s_t*)block, mask, r0);
        coeff++;
        block += 4;
    }
}

void uavs3e_itrans_dst7_pb8_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)
{
    int i;
    char *iT = com_tbl_tm8[DST7][0];
    __m256i s0, s1, s2, s3, s4, s5, s6, s7;
    __m256i c0, c1, c2, c3, c4, c5, c6, c7;
    __m256i e0, e1, e2, e3, e4, e5, e6;
    __m256i r0, r1, r2;
    __m256i zero = _mm256_setzero_si256();
    __m256i rnd_factor = _mm256_set1_epi32(1 << (shift - 1));
    __m256i mask = _mm256_set_epi64x(0, 0, -1, -1);

    for (i = 0; i < line; i++)
    {
        s0 = _mm256_set1_epi32(coeff[0]);
        s1 = _mm256_set1_epi32(coeff[line]);
        s2 = _mm256_set1_epi32(coeff[2 * line]);
        s3 = _mm256_set1_epi32(coeff[3 * line]);
        s4 = _mm256_set1_epi32(coeff[4 * line]);
        s5 = _mm256_set1_epi32(coeff[5 * line]);
        s6 = _mm256_set1_epi32(coeff[6 * line]);
        s7 = _mm256_set1_epi32(coeff[7 * line]);

        c0 = _mm256_loadu_si256((__m256i*)iT);
        c0 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c0));
        c1 = _mm256_loadu_si256((__m256i*)(iT + 8));
        c1 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c1));
        c2 = _mm256_loadu_si256((__m256i*)(iT + 16));
        c2 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c2));
        c3 = _mm256_loadu_si256((__m256i*)(iT + 24));
        c3 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c3));
        c4 = _mm256_loadu_si256((__m256i*)(iT + 32));
        c4 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c4));
        c5 = _mm256_loadu_si256((__m256i*)(iT + 40));
        c5 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c5));
        c6 = _mm256_loadu_si256((__m256i*)(iT + 48));
        c6 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c6));
        c7 = _mm256_loadu_si256((__m256i*)(iT + 56));
        c7 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c7));

        e0 = _mm256_add_epi32(_mm256_mullo_epi32(s0, c0), _mm256_mullo_epi32(s1, c1));
        e1 = _mm256_add_epi32(_mm256_mullo_epi32(s2, c2), _mm256_mullo_epi32(s3, c3));
        e2 = _mm256_add_epi32(_mm256_mullo_epi32(s4, c4), _mm256_mullo_epi32(s5, c5));
        e3 = _mm256_add_epi32(_mm256_mullo_epi32(s6, c6), _mm256_mullo_epi32(s7, c7));
        e4 = _mm256_add_epi32(e0, e1);
        e5 = _mm256_add_epi32(e2, e3);
        e6 = _mm256_add_epi32(e4, e5);

        r0 = _mm256_add_epi32(e6, rnd_factor);
        r1 = _mm256_packs_epi32(_mm256_srai_epi32(r0, shift), zero);
        r2 = _mm256_permute4x64_epi64(r1, 0x08);
        _mm256_maskstore_epi64((i64s_t*)block, mask, r2);

        coeff++;
        block += 8;
    }
}

void uavs3e_itrans_dst7_pb16_avx2(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)
{
    int i, j, k;
    char *iT = com_tbl_tm16[DST7][0];
    __m256i s0, s1, s2, s3, s4, s5, s6, s7;
    __m256i c0, c1, c2, c3, c4, c5, c6, c7;
    __m256i e0, e1, e2, e3, e4, e5, e6;
    __m256i r0, r1, r2;
    __m256i mask = _mm256_set_epi64x(0, 0, -1, -1);
    __m256i zero = _mm256_setzero_si256();
    __m256i rnd_factor = _mm256_set1_epi32(1 << (shift - 1));
    for (i = 0; i < line; i++)
    {
        char *pb16 = iT;
        for (j = 0; j < 2; j++)
        {
            __m256i v0 = zero;
            for (k = 0; k < 2; k++)
            {
                s0 = _mm256_set1_epi32(coeff[0]);
                s1 = _mm256_set1_epi32(coeff[line]);
                s2 = _mm256_set1_epi32(coeff[2 * line]);
                s3 = _mm256_set1_epi32(coeff[3 * line]);
                s4 = _mm256_set1_epi32(coeff[4 * line]);
                s5 = _mm256_set1_epi32(coeff[5 * line]);
                s6 = _mm256_set1_epi32(coeff[6 * line]);
                s7 = _mm256_set1_epi32(coeff[7 * line]);

                c0 = _mm256_loadu_si256((__m256i*)pb16);
                c0 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c0));
                c1 = _mm256_loadu_si256((__m256i*)(pb16 + 16));
                c1 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c1));
                c2 = _mm256_loadu_si256((__m256i*)(pb16 + 32));
                c2 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c2));
                c3 = _mm256_loadu_si256((__m256i*)(pb16 + 48));
                c3 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c3));
                c4 = _mm256_loadu_si256((__m256i*)(pb16 + 64));
                c4 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c4));
                c5 = _mm256_loadu_si256((__m256i*)(pb16 + 80));
                c5 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c5));
                c6 = _mm256_loadu_si256((__m256i*)(pb16 + 96));
                c6 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c6));
                c7 = _mm256_loadu_si256((__m256i*)(pb16 + 112));
                c7 = _mm256_cvtepi8_epi32(_mm256_castsi256_si128(c7));

                e0 = _mm256_add_epi32(_mm256_mullo_epi32(s0, c0), _mm256_mullo_epi32(s1, c1));
                e1 = _mm256_add_epi32(_mm256_mullo_epi32(s2, c2), _mm256_mullo_epi32(s3, c3));
                e2 = _mm256_add_epi32(_mm256_mullo_epi32(s4, c4), _mm256_mullo_epi32(s5, c5));
                e3 = _mm256_add_epi32(_mm256_mullo_epi32(s6, c6), _mm256_mullo_epi32(s7, c7));
                e4 = _mm256_add_epi32(e0, e1);
                e5 = _mm256_add_epi32(e2, e3);
                e6 = _mm256_add_epi32(e4, e5);

                v0 = _mm256_add_epi32(v0, e6);

                pb16 += 128;
                coeff += (8 * line);
            }
            r0 = _mm256_add_epi32(v0, rnd_factor);
            r1 = _mm256_packs_epi32(_mm256_srai_epi32(r0, shift), zero);
            r2 = _mm256_permute4x64_epi64(r1, 0x08);
            _mm256_maskstore_epi64((i64s_t*)block, mask, r2);
            coeff -= (16 * line);
            pb16 -= 248;
            block += 8;
        }
        coeff++;
    }
}
