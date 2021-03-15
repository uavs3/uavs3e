#include"transform.h"
#include <memory.h>

#define absm(A) ((A)<(0) ? (-(A)):(A))

static __inline coef_t clip_coef(int x)
{
    return (coef_t)min(max(x, -32768), 32767);
}

#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))


#define LOT_MAX_WLT_TAP 2 

char com_tbl_tm2[3][2][2];
char com_tbl_tm4[3][4][4];
char com_tbl_tm8[3][8][8];
char com_tbl_tm16[3][16][16];
char com_tbl_tm32[3][32][32];
char com_tbl_tm64[3][64][64];

double com_tbl_sqrt[2];

tab_i32s_t tab_trans_core_4[4][4] = {
    {  32,    32,     32,     32 },
    {  42,    17,    -17,    -42 },
    {  32,   -32,    -32,     32 },
    {  17,   -42,     42,    -17 }
};

tab_i32s_t tab_trans_core_8[8][8] = {
    {  32,    32,     32,     32,     32,     32,     32,     32    },
    {  44,    38,     25,      9,     -9,    -25,    -38,    -44    },
    {  42,    17,    -17,    -42,    -42,    -17,     17,     42    },
    {  38,    -9,    -44,    -25,     25,     44,      9,    -38    },
    {  32,   -32,    -32,     32,     32,    -32,    -32,     32    },
    {  25,   -44,      9,     38,    -38,     -9,     44,    -25    },
    {  17,   -42,     42,    -17,    -17,     42,    -42,     17    },
    {   9,   -25,     38,    -44,     44,    -38,     25,     -9    }
};

tab_i32s_t tab_trans_core_16[16][16] = {
    {  32,    32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32    },
    {  45,    43,     40,     35,     29,     21,     13,      4,     -4,    -13,    -21,    -29,    -35,    -40,    -43,    -45    },
    {  44,    38,     25,      9,     -9,    -25,    -38,    -44,    -44,    -38,    -25,     -9,      9,     25,     38,     44    },
    {  43,    29,      4,    -21,    -40,    -45,    -35,    -13,     13,     35,     45,     40,     21,     -4,    -29,    -43    },
    {  42,    17,    -17,    -42,    -42,    -17,     17,     42,     42,     17,    -17,    -42,    -42,    -17,     17,     42    },
    {  40,     4,    -35,    -43,    -13,     29,     45,     21,    -21,    -45,    -29,     13,     43,     35,     -4,    -40    },
    {  38,    -9,    -44,    -25,     25,     44,      9,    -38,    -38,      9,     44,     25,    -25,    -44,     -9,     38    },
    {  35,   -21,    -43,      4,     45,     13,    -40,    -29,     29,     40,    -13,    -45,     -4,     43,     21,    -35    },
    {  32,   -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32    },
    {  29,   -40,    -13,     45,     -4,    -43,     21,     35,    -35,    -21,     43,      4,    -45,     13,     40,    -29    },
    {  25,   -44,      9,     38,    -38,     -9,     44,    -25,    -25,     44,     -9,    -38,     38,      9,    -44,     25    },
    {  21,   -45,     29,     13,    -43,     35,      4,    -40,     40,     -4,    -35,     43,    -13,    -29,     45,    -21    },
    {  17,   -42,     42,    -17,    -17,     42,    -42,     17,     17,    -42,     42,    -17,    -17,     42,    -42,     17    },
    {  13,   -35,     45,    -40,     21,      4,    -29,     43,    -43,     29,     -4,    -21,     40,    -45,     35,    -13    },
    {   9,   -25,     38,    -44,     44,    -38,     25,     -9,     -9,     25,    -38,     44,    -44,     38,    -25,      9    },
    {   4,   -13,     21,    -29,     35,    -40,     43,    -45,     45,    -43,     40,    -35,     29,    -21,     13,     -4    }
};

tab_i32s_t tab_trans_core_32[32][32] = {
    {  32,    32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32,     32    },
    {  45,    45,     44,     43,     41,     39,     36,     34,     30,     27,     23,     19,     15,     11,      7,      2,     -2,     -7,    -11,    -15,    -19,    -23,    -27,    -30,    -34,    -36,    -39,    -41,    -43,    -44,    -45,    -45    },
    {  45,    43,     40,     35,     29,     21,     13,      4,     -4,    -13,    -21,    -29,    -35,    -40,    -43,    -45,    -45,    -43,    -40,    -35,    -29,    -21,    -13,     -4,      4,     13,     21,     29,     35,     40,     43,     45    },
    {  45,    41,     34,     23,     11,     -2,    -15,    -27,    -36,    -43,    -45,    -44,    -39,    -30,    -19,     -7,      7,     19,     30,     39,     44,     45,     43,     36,     27,     15,      2,    -11,    -23,    -34,    -41,    -45    },
    {  44,    38,     25,      9,     -9,    -25,    -38,    -44,    -44,    -38,    -25,     -9,      9,     25,     38,     44,     44,     38,     25,      9,     -9,    -25,    -38,    -44,    -44,    -38,    -25,     -9,      9,     25,     38,     44    },
    {  44,    34,     15,     -7,    -27,    -41,    -45,    -39,    -23,     -2,     19,     36,     45,     43,     30,     11,    -11,    -30,    -43,    -45,    -36,    -19,      2,     23,     39,     45,     41,     27,      7,    -15,    -34,    -44    },
    {  43,    29,      4,    -21,    -40,    -45,    -35,    -13,     13,     35,     45,     40,     21,     -4,    -29,    -43,    -43,    -29,     -4,     21,     40,     45,     35,     13,    -13,    -35,    -45,    -40,    -21,      4,     29,     43    },
    {  43,    23,     -7,    -34,    -45,    -36,    -11,     19,     41,     44,     27,     -2,    -30,    -45,    -39,    -15,     15,     39,     45,     30,      2,    -27,    -44,    -41,    -19,     11,     36,     45,     34,      7,    -23,    -43    },
    {  42,    17,    -17,    -42,    -42,    -17,     17,     42,     42,     17,    -17,    -42,    -42,    -17,     17,     42,     42,     17,    -17,    -42,    -42,    -17,     17,     42,     42,     17,    -17,    -42,    -42,    -17,     17,     42    },
    {  41,    11,    -27,    -45,    -30,      7,     39,     43,     15,    -23,    -45,    -34,      2,     36,     44,     19,    -19,    -44,    -36,     -2,     34,     45,     23,    -15,    -43,    -39,     -7,     30,     45,     27,    -11,    -41    },
    {  40,     4,    -35,    -43,    -13,     29,     45,     21,    -21,    -45,    -29,     13,     43,     35,     -4,    -40,    -40,     -4,     35,     43,     13,    -29,    -45,    -21,     21,     45,     29,    -13,    -43,    -35,      4,     40    },
    {  39,    -2,    -41,    -36,      7,     43,     34,    -11,    -44,    -30,     15,     45,     27,    -19,    -45,    -23,     23,     45,     19,    -27,    -45,    -15,     30,     44,     11,    -34,    -43,     -7,     36,     41,      2,    -39    },
    {  38,    -9,    -44,    -25,     25,     44,      9,    -38,    -38,      9,     44,     25,    -25,    -44,     -9,     38,     38,     -9,    -44,    -25,     25,     44,      9,    -38,    -38,      9,     44,     25,    -25,    -44,     -9,     38    },
    {  36,   -15,    -45,    -11,     39,     34,    -19,    -45,     -7,     41,     30,    -23,    -44,     -2,     43,     27,    -27,    -43,      2,     44,     23,    -30,    -41,      7,     45,     19,    -34,    -39,     11,     45,     15,    -36    },
    {  35,   -21,    -43,      4,     45,     13,    -40,    -29,     29,     40,    -13,    -45,     -4,     43,     21,    -35,    -35,     21,     43,     -4,    -45,    -13,     40,     29,    -29,    -40,     13,     45,      4,    -43,    -21,     35    },
    {  34,   -27,    -39,     19,     43,    -11,    -45,      2,     45,      7,    -44,    -15,     41,     23,    -36,    -30,     30,     36,    -23,    -41,     15,     44,     -7,    -45,     -2,     45,     11,    -43,    -19,     39,     27,    -34    },
    {  32,   -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32,     32,    -32,    -32,     32    },
    {  30,   -36,    -23,     41,     15,    -44,     -7,     45,     -2,    -45,     11,     43,    -19,    -39,     27,     34,    -34,    -27,     39,     19,    -43,    -11,     45,      2,    -45,      7,     44,    -15,    -41,     23,     36,    -30    },
    {  29,   -40,    -13,     45,     -4,    -43,     21,     35,    -35,    -21,     43,      4,    -45,     13,     40,    -29,    -29,     40,     13,    -45,      4,     43,    -21,    -35,     35,     21,    -43,     -4,     45,    -13,    -40,     29    },
    {  27,   -43,     -2,     44,    -23,    -30,     41,      7,    -45,     19,     34,    -39,    -11,     45,    -15,    -36,     36,     15,    -45,     11,     39,    -34,    -19,     45,     -7,    -41,     30,     23,    -44,      2,     43,    -27    },
    {  25,   -44,      9,     38,    -38,     -9,     44,    -25,    -25,     44,     -9,    -38,     38,      9,    -44,     25,     25,    -44,      9,     38,    -38,     -9,     44,    -25,    -25,     44,     -9,    -38,     38,      9,    -44,     25    },
    {  23,   -45,     19,     27,    -45,     15,     30,    -44,     11,     34,    -43,      7,     36,    -41,      2,     39,    -39,     -2,     41,    -36,     -7,     43,    -34,    -11,     44,    -30,    -15,     45,    -27,    -19,     45,    -23    },
    {  21,   -45,     29,     13,    -43,     35,      4,    -40,     40,     -4,    -35,     43,    -13,    -29,     45,    -21,    -21,     45,    -29,    -13,     43,    -35,     -4,     40,    -40,      4,     35,    -43,     13,     29,    -45,     21    },
    {  19,   -44,     36,     -2,    -34,     45,    -23,    -15,     43,    -39,      7,     30,    -45,     27,     11,    -41,     41,    -11,    -27,     45,    -30,     -7,     39,    -43,     15,     23,    -45,     34,      2,    -36,     44,    -19    },
    {  17,   -42,     42,    -17,    -17,     42,    -42,     17,     17,    -42,     42,    -17,    -17,     42,    -42,     17,     17,    -42,     42,    -17,    -17,     42,    -42,     17,     17,    -42,     42,    -17,    -17,     42,    -42,     17    },
    {  15,   -39,     45,    -30,      2,     27,    -44,     41,    -19,    -11,     36,    -45,     34,     -7,    -23,     43,    -43,     23,      7,    -34,     45,    -36,     11,     19,    -41,     44,    -27,     -2,     30,    -45,     39,    -15    },
    {  13,   -35,     45,    -40,     21,      4,    -29,     43,    -43,     29,     -4,    -21,     40,    -45,     35,    -13,    -13,     35,    -45,     40,    -21,     -4,     29,    -43,     43,    -29,      4,     21,    -40,     45,    -35,     13    },
    {  11,   -30,     43,    -45,     36,    -19,     -2,     23,    -39,     45,    -41,     27,     -7,    -15,     34,    -44,     44,    -34,     15,      7,    -27,     41,    -45,     39,    -23,      2,     19,    -36,     45,    -43,     30,    -11    },
    {   9,   -25,     38,    -44,     44,    -38,     25,     -9,     -9,     25,    -38,     44,    -44,     38,    -25,      9,      9,    -25,     38,    -44,     44,    -38,     25,     -9,     -9,     25,    -38,     44,    -44,     38,    -25,      9    },
    {   7,   -19,     30,    -39,     44,    -45,     43,    -36,     27,    -15,      2,     11,    -23,     34,    -41,     45,    -45,     41,    -34,     23,    -11,     -2,     15,    -27,     36,    -43,     45,    -44,     39,    -30,     19,     -7    },
    {   4,   -13,     21,    -29,     35,    -40,     43,    -45,     45,    -43,     40,    -35,     29,    -21,     13,     -4,     -4,     13,    -21,     29,    -35,     40,    -43,     45,    -45,     43,    -40,     35,    -29,     21,    -13,      4    },
    {   2,    -7,     11,    -15,     19,    -23,     27,    -30,     34,    -36,     39,    -41,     43,    -44,     45,    -45,     45,    -45,     44,    -43,     41,    -39,     36,    -34,     30,    -27,     23,    -19,     15,    -11,      7,     -2    }
};

tab_i16u_t tab_q_tab[80] = {
    32768, 29775, 27554, 25268, 23170, 21247, 19369, 17770,
    16302, 15024, 13777, 12634, 11626, 10624,  9742,  8958,
     8192,  7512,  6889,  6305,  5793,  5303,  4878,  4467,
     4091,  3756,  3444,  3161,  2894,  2654,  2435,  2235,
     2048,  1878,  1722,  1579,  1449,  1329,  1218,  1117,
     1024,   939,   861,   790,   724,   664,   609,   558,
      512,   470,   430,   395,   362,   332,   304,   279,
      256,   235,   215,   197,   181,   166,   152,   140,
      128,   116,   108,    99,    91,    83,    76,    69,
       64,    59,    54,    49,    45,    41,    38,    35
};

tab_i16u_t tab_iq_tab[80] = {
    32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
    32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933,
    65535, 35734, 38968, 42577, 46341, 50617, 55027, 60097,
    32809, 35734, 38968, 42454, 46382, 50576, 55109, 60056,
    65535, 35734, 38968, 42495, 46320, 50515, 55109, 60076,
    65535, 35744, 38968, 42495, 46341, 50535, 55099, 60087,
    65535, 35734, 38973, 42500, 46341, 50535, 55109, 60097,
    32771, 35734, 38965, 42497, 46341, 50535, 55109, 60099,
    32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
    32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933
};

tab_i16s_t tab_iq_shift[80] = {
    15, 15, 15, 15, 15, 15, 15, 15,
    14, 14, 14, 14, 14, 14, 14, 14,
    14, 13, 13, 13, 13, 13, 13, 13,
    12, 12, 12, 12, 12, 12, 12, 12,
    12, 11, 11, 11, 11, 11, 11, 11,
    11, 10, 10, 10, 10, 10, 10, 10,
    10,  9,  9,  9,  9,  9,  9,  9,
     8,  8,  8,  8,  8,  8,  8,  8,
     7,  7,  7,  7,  7,  7,  7,  7,
     6,  6,  6,  6,  6,  6,  6,  6
};

tab_i16s_t tab_c4_trans[4][4] = {
    {    34,    58,    72,     81,},
    {    77,    69,    -7,    -75,},
    {    79,   -33,   -75,     58,},
    {    55,   -84,    73,    -28,}
};

tab_i16s_t tab_c8_trans[4][4] = {
    {   123,   -35,    -8,    -3,},
    {   -32,  -120,    30,    10,},
    {    14,    25,   123,   -22,},
    {     8,    13,    19,   126,},
};


#define PIX_SUB_DEFINE(s) \
static void pix_sub_##s(resi_t *dst, pel_t *org, int i_org, pel_t *pred, int i_pred) { \
    int i, j;    \
    for (i = 0; i < s; i++) { \
        for (j = 0; j < s; j++) { \
            dst[j] = org[j] - pred[j]; \
        } \
        dst += s; \
        org += i_org; \
        pred += i_pred; \
    } \
}

PIX_SUB_DEFINE(4)
PIX_SUB_DEFINE(8)
PIX_SUB_DEFINE(16)
PIX_SUB_DEFINE(32)

#define PIX_ADD_DEFINE(s) \
static void pix_add_##s(pel_t *dst, int i_dst, pel_t *pred, int i_pred, resi_t *resi, int bit_depth) { \
    int i, j; \
    int max_pixel = (1 << bit_depth) - 1; \
    for (i = 0; i < s; i++) { \
        for (j = 0; j < s; j++) { \
            dst[j] = avs3_pixel_clip(resi[j] + pred[j], max_pixel); \
        } \
        dst += i_dst; \
        resi += s; \
        pred += i_pred; \
    } \
}

PIX_ADD_DEFINE(4)
PIX_ADD_DEFINE(8)
PIX_ADD_DEFINE(16)
PIX_ADD_DEFINE(32)

/////////////////////////////////////////////////////////////////////////////
/// function definition
/////////////////////////////////////////////////////////////////////////////
static void partialButterfly4x4(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift)
{
    int j;
    int E[4], O[4];
    int add = shift ? (1 << (shift - 1)) : 0;

    for (j = 0; j < 4; j++) {
        E[0] = tab_trans_core_4[0][0] * src[0] + tab_trans_core_4[2][0] * src[2];
        E[1] = tab_trans_core_4[2][0] * src[0] - tab_trans_core_4[0][0] * src[2];
        E[2] = tab_trans_core_4[1][0] * src[0] - tab_trans_core_4[3][0] * src[2];
        E[3] = tab_trans_core_4[3][0] * src[0] + tab_trans_core_4[1][0] * src[2];
        O[0] = tab_trans_core_4[1][0] * src[1] + tab_trans_core_4[3][0] * src[3];
        O[1] = tab_trans_core_4[3][0] * src[1] - tab_trans_core_4[1][0] * src[3];
        O[2] = tab_trans_core_4[0][0] * src[1] - tab_trans_core_4[2][0] * src[3];
        O[3] = tab_trans_core_4[2][0] * src[1] + tab_trans_core_4[0][0] * src[3];
        src += i_src;
        dst[0 * i_dst] = clip_coef((E[0] + O[3] + add) >> shift);
        dst[2 * i_dst] = clip_coef((E[1] - O[2] + add) >> shift);
        dst[1 * i_dst] = clip_coef((E[2] + O[1] + add) >> shift);
        dst[3 * i_dst] = clip_coef((E[3] - O[0] + add) >> shift);
        dst++;
    }
}

static void partialButterfly8x8(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift)
{
    int j, k;
    int E[4], O[4];
    int EE[2], EO[2];
    int add = 1 << (shift - 1);

    for (j = 0; j < 8; j++) {
        /* E and O*/
        for (k = 0; k < 4; k++) {
            E[k] = src[k] + src[7 - k];
            O[k] = src[k] - src[7 - k];
        }
        src += i_src;

        /* EE and EO */
        EE[0] = E[0] + E[3];
        EO[0] = E[0] - E[3];
        EE[1] = E[1] + E[2];
        EO[1] = E[1] - E[2];

        dst[0 * i_dst] = clip_coef((tab_trans_core_8[0][0] * EE[0] + tab_trans_core_8[0][1] * EE[1] + add) >> shift);
        dst[4 * i_dst] = clip_coef((tab_trans_core_8[4][0] * EE[0] + tab_trans_core_8[4][1] * EE[1] + add) >> shift);
        dst[2 * i_dst] = clip_coef((tab_trans_core_8[2][0] * EO[0] + tab_trans_core_8[2][1] * EO[1] + add) >> shift);
        dst[6 * i_dst] = clip_coef((tab_trans_core_8[6][0] * EO[0] + tab_trans_core_8[6][1] * EO[1] + add) >> shift);

        dst[1 * i_dst] = clip_coef((tab_trans_core_8[1][0] * O[0] + tab_trans_core_8[1][1] * O[1] + tab_trans_core_8[1][2] * O[2] + tab_trans_core_8[1][3] * O[3] + add) >> shift);
        dst[3 * i_dst] = clip_coef((tab_trans_core_8[3][0] * O[0] + tab_trans_core_8[3][1] * O[1] + tab_trans_core_8[3][2] * O[2] + tab_trans_core_8[3][3] * O[3] + add) >> shift);
        dst[5 * i_dst] = clip_coef((tab_trans_core_8[5][0] * O[0] + tab_trans_core_8[5][1] * O[1] + tab_trans_core_8[5][2] * O[2] + tab_trans_core_8[5][3] * O[3] + add) >> shift);
        dst[7 * i_dst] = clip_coef((tab_trans_core_8[7][0] * O[0] + tab_trans_core_8[7][1] * O[1] + tab_trans_core_8[7][2] * O[2] + tab_trans_core_8[7][3] * O[3] + add) >> shift);
        dst++;
    }
}

static void partialButterfly16x16(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift)
{
    int j, k;
    int E[8], O[8];
    int EE[4], EO[4];
    int EEE[2], EEO[2];
    int add = 1 << (shift - 1);

    for (j = 0; j < 16; j++) {
        /* E and O*/
        for (k = 0; k < 8; k++) {
            E[k] = src[k] + src[15 - k];
            O[k] = src[k] - src[15 - k];
        }
        src += i_src;
        /* EE and EO */
        for (k = 0; k < 4; k++) {
            EE[k] = E[k] + E[7 - k];
            EO[k] = E[k] - E[7 - k];
        }
        /* EEE and EEO */
        EEE[0] = EE[0] + EE[3];
        EEO[0] = EE[0] - EE[3];
        EEE[1] = EE[1] + EE[2];
        EEO[1] = EE[1] - EE[2];

        dst[0 * i_dst] = clip_coef((tab_trans_core_16[0][0] * EEE[0] + tab_trans_core_16[0][1] * EEE[1] + add) >> shift);
        dst[8 * i_dst] = clip_coef((tab_trans_core_16[8][0] * EEE[0] + tab_trans_core_16[8][1] * EEE[1] + add) >> shift);
        dst[4 * i_dst] = clip_coef((tab_trans_core_16[4][0] * EEO[0] + tab_trans_core_16[4][1] * EEO[1] + add) >> shift);
        dst[12 * i_dst] = clip_coef((tab_trans_core_16[12][0] * EEO[0] + tab_trans_core_16[12][1] * EEO[1] + add) >> shift);

        for (k = 2; k < 16; k += 4) {
            dst[k* i_dst] = clip_coef((tab_trans_core_16[k][0] * EO[0] + tab_trans_core_16[k][1] * EO[1] + tab_trans_core_16[k][2] * EO[2] + tab_trans_core_16[k][3] * EO[3] + add) >> shift);
        }

        for (k = 1; k < 16; k += 2) {
            dst[k * i_dst] = clip_coef((tab_trans_core_16[k][0] * O[0] + tab_trans_core_16[k][1] * O[1] + tab_trans_core_16[k][2] * O[2] + tab_trans_core_16[k][3] * O[3] +
                tab_trans_core_16[k][4] * O[4] + tab_trans_core_16[k][5] * O[5] + tab_trans_core_16[k][6] * O[6] + tab_trans_core_16[k][7] * O[7] + add) >> shift);
        }
        dst++;
    }
}

static void partialButterfly32x32(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift)
{
    int j, k;
    int E[16], O[16];
    int EE[8], EO[8];
    int EEE[4], EEO[4];
    int EEEE[2], EEEO[2];
    int add = 1 << (shift - 1);

    for (j = 0; j < 32; j++) {
        /* E and O*/
        for (k = 0; k < 16; k++) {
            E[k] = src[k] + src[31 - k];
            O[k] = src[k] - src[31 - k];
        }
        src += i_src;
        /* EE and EO */
        for (k = 0; k < 8; k++) {
            EE[k] = E[k] + E[15 - k];
            EO[k] = E[k] - E[15 - k];
        }
        /* EEE and EEO */
        for (k = 0; k < 4; k++) {
            EEE[k] = EE[k] + EE[7 - k];
            EEO[k] = EE[k] - EE[7 - k];
        }
        /* EEEE and EEEO */
        EEEE[0] = EEE[0] + EEE[3];
        EEEO[0] = EEE[0] - EEE[3];
        EEEE[1] = EEE[1] + EEE[2];
        EEEO[1] = EEE[1] - EEE[2];

        dst[0 * i_dst] = clip_coef((tab_trans_core_32[0][0] * EEEE[0] + tab_trans_core_32[0][1] * EEEE[1] + add) >> shift);
        dst[16 * i_dst] = clip_coef((tab_trans_core_32[16][0] * EEEE[0] + tab_trans_core_32[16][1] * EEEE[1] + add) >> shift);
        dst[8 * i_dst] = clip_coef((tab_trans_core_32[8][0] * EEEO[0] + tab_trans_core_32[8][1] * EEEO[1] + add) >> shift);
        dst[24 * i_dst] = clip_coef((tab_trans_core_32[24][0] * EEEO[0] + tab_trans_core_32[24][1] * EEEO[1] + add) >> shift);
        for (k = 4; k < 32; k += 8) {
            dst[k* i_dst] = clip_coef((tab_trans_core_32[k][0] * EEO[0] + tab_trans_core_32[k][1] * EEO[1] + tab_trans_core_32[k][2] * EEO[2] + tab_trans_core_32[k][3] * EEO[3] + add) >> shift);
        }
        for (k = 2; k < 32; k += 4) {
            dst[k* i_dst] = clip_coef((tab_trans_core_32[k][0] * EO[0] + tab_trans_core_32[k][1] * EO[1] + tab_trans_core_32[k][2] * EO[2] + tab_trans_core_32[k][3] * EO[3] +
                tab_trans_core_32[k][4] * EO[4] + tab_trans_core_32[k][5] * EO[5] + tab_trans_core_32[k][6] * EO[6] + tab_trans_core_32[k][7] * EO[7] + add) >> shift);
        }
        for (k = 1; k < 32; k += 2) {
            dst[k * i_dst] = clip_coef((tab_trans_core_32[k][0] * O[0] + tab_trans_core_32[k][1] * O[1] + tab_trans_core_32[k][2] * O[2] + tab_trans_core_32[k][3] * O[3] +
                tab_trans_core_32[k][4] * O[4] + tab_trans_core_32[k][5] * O[5] + tab_trans_core_32[k][6] * O[6] + tab_trans_core_32[k][7] * O[7] +
                tab_trans_core_32[k][8] * O[8] + tab_trans_core_32[k][9] * O[9] + tab_trans_core_32[k][10] * O[10] + tab_trans_core_32[k][11] * O[11] +
                tab_trans_core_32[k][12] * O[12] + tab_trans_core_32[k][13] * O[13] + tab_trans_core_32[k][14] * O[14] + tab_trans_core_32[k][15] * O[15] + add) >> shift);
        }
        dst++;
    }
}

static void partialButterflyInverse4x4(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift, int bit_depth)
{
    int j;
    int E[2], O[2];
    int max_val = (1 << (bit_depth - 1)) - 1;
    int min_val = -max_val - 1;
    int add = 1 << (shift - 1);

    for (j = 0; j < 4; j++) {
        E[0] = tab_trans_core_4[0][0] * src[0 * i_src] + tab_trans_core_4[2][0] * src[2 * i_src];
        E[1] = tab_trans_core_4[2][0] * src[0 * i_src] - tab_trans_core_4[0][0] * src[2 * i_src];
        O[0] = tab_trans_core_4[1][0] * src[1 * i_src] + tab_trans_core_4[3][0] * src[3 * i_src];
        O[1] = tab_trans_core_4[3][0] * src[1 * i_src] - tab_trans_core_4[1][0] * src[3 * i_src];
        src++;
        dst[0] = (resi_t)Clip3(min_val, max_val, (E[0] + O[0] + add) >> shift);
        dst[2] = (resi_t)Clip3(min_val, max_val, (E[1] - O[1] + add) >> shift);
        dst[1] = (resi_t)Clip3(min_val, max_val, (E[1] + O[1] + add) >> shift);
        dst[3] = (resi_t)Clip3(min_val, max_val, (E[0] - O[0] + add) >> shift);
        dst += i_dst;
    }
}

static void partialButterflyInverse8x8(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift, int bit_depth)
{
    int j, k;
    int E[4], O[4];
    int EE[2], EO[2];
    int max_val = (1 << (bit_depth - 1)) - 1;
    int min_val = -max_val - 1;
    int add = 1 << (shift - 1);

    for (j = 0; j < 8; j++) {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k = 0; k < 4; k++) {
            O[k] = tab_trans_core_8[ 1][k] * src[1 * i_src] + tab_trans_core_8[ 3][k] * src[3 * i_src] + tab_trans_core_8[ 5][k] * src[5 * i_src] + tab_trans_core_8[ 7][k] * src[7 * i_src];
        }

        EO[0] = tab_trans_core_8[2][0] * src[ 2  * i_src] + tab_trans_core_8[6][0] * src[ 6  * i_src];
        EO[1] = tab_trans_core_8[2][1] * src[ 2  * i_src] + tab_trans_core_8[6][1] * src[ 6  * i_src];
        EE[0] = tab_trans_core_8[0][0] * src[ 0  * i_src] + tab_trans_core_8[4][0] * src[ 4  * i_src];
        EE[1] = tab_trans_core_8[0][1] * src[ 0  * i_src] + tab_trans_core_8[4][1] * src[ 4  * i_src];
        src++;
        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        E[0] = EE[0] + EO[0];
        E[3] = EE[0] - EO[0];
        E[1] = EE[1] + EO[1];
        E[2] = EE[1] - EO[1];
        for (k = 0; k < 4; k++) {
            dst[ k   ] = (resi_t)Clip3(min_val, max_val, (E[k] + O[k] + add) >> shift);
            dst[k + 4] = (resi_t)Clip3(min_val, max_val, (E[3 - k] - O[3 - k] + add) >> shift);
        }
        dst += i_dst;
    }
}

static void partialButterflyInverse16x16(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift, int bit_depth)
{
    int j, k;
    int E[8], O[8];
    int EE[4], EO[4];
    int EEE[2], EEO[2];
    int max_val = (1 << (bit_depth - 1)) - 1;
    int min_val = -max_val - 1;
    int add = 1 << (shift - 1);

    for (j = 0; j < 16; j++) {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k = 0; k < 8; k++) {
            O[k] = tab_trans_core_16[ 1][k] * src[ 1  * i_src] + tab_trans_core_16[ 3][k] * src[ 3  * i_src] + tab_trans_core_16[ 5][k] * src[ 5  * i_src] + tab_trans_core_16[ 7][k] * src[ 7  * i_src] +
                   tab_trans_core_16[ 9][k] * src[ 9  * i_src] + tab_trans_core_16[11][k] * src[ 11  * i_src] + tab_trans_core_16[13][k] * src[ 13  * i_src] + tab_trans_core_16[15][k] * src[15  * i_src];
        }
        for (k = 0; k < 4; k++) {
            EO[k] = tab_trans_core_16[ 2][k] * src[ 2  * i_src] + tab_trans_core_16[ 6][k] * src[ 6  * i_src] + tab_trans_core_16[10][k] * src[10  * i_src] + tab_trans_core_16[14][k] * src[14  * i_src];
        }
        EEO[0] = tab_trans_core_16[4][0] * src[ 4  * i_src] + tab_trans_core_16[12][0] * src[ 12  * i_src];
        EEE[0] = tab_trans_core_16[0][0] * src[ 0  * i_src] + tab_trans_core_16[ 8][0] * src[ 8   * i_src];
        EEO[1] = tab_trans_core_16[4][1] * src[ 4  * i_src] + tab_trans_core_16[12][1] * src[ 12  * i_src];
        EEE[1] = tab_trans_core_16[0][1] * src[ 0  * i_src] + tab_trans_core_16[ 8][1] * src[ 8   * i_src];
        src++;
        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        for (k = 0; k < 2; k++) {
            EE[k] = EEE[k] + EEO[k];
            EE[k + 2] = EEE[1 - k] - EEO[1 - k];
        }
        for (k = 0; k < 4; k++) {
            E[k] = EE[k] + EO[k];
            E[k + 4] = EE[3 - k] - EO[3 - k];
        }
        for (k = 0; k < 8; k++) {
            dst[k] = (resi_t)Clip3(min_val, max_val, (E[k] + O[k] + add) >> shift);
            dst[k + 8] = (resi_t)Clip3(min_val, max_val, (E[7 - k] - O[7 - k] + add) >> shift);
        }
        dst += i_dst;
    }
}

static void partialButterflyInverse32x32(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift, int bit_depth)
{
    int j, k;
    int E[16], O[16];
    int EE[8], EO[8];
    int EEE[4], EEO[4];
    int EEEE[2], EEEO[2];
    int max_val = (1 << (bit_depth - 1)) - 1;
    int min_val = -max_val - 1;
    int add = 1 << (shift - 1);

    for (j = 0; j < 32; j++) {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k = 0; k < 16; k++) {
            O[k] = tab_trans_core_32[ 1][k] * src[ 1  * i_src] + tab_trans_core_32[ 3][k] * src[ 3   * i_src] + tab_trans_core_32[ 5][k] * src[ 5   * i_src] + tab_trans_core_32[ 7][k] * src[ 7  * i_src] +
                   tab_trans_core_32[ 9][k] * src[ 9   * i_src] + tab_trans_core_32[11][k] * src[ 11  * i_src] + tab_trans_core_32[13][k] * src[ 13  * i_src] + tab_trans_core_32[15][k] * src[ 15  * i_src] +
                   tab_trans_core_32[17][k] * src[ 17  * i_src] + tab_trans_core_32[19][k] * src[ 19  * i_src] + tab_trans_core_32[21][k] * src[ 21  * i_src] + tab_trans_core_32[23][k] * src[ 23  * i_src] +
                   tab_trans_core_32[25][k] * src[ 25  * i_src] + tab_trans_core_32[27][k] * src[ 27  * i_src] + tab_trans_core_32[29][k] * src[ 29  * i_src] + tab_trans_core_32[31][k] * src[ 31  * i_src];
        }
        for (k = 0; k < 8; k++) {
            EO[k] = tab_trans_core_32[ 2][k] * src[ 2  * i_src] + tab_trans_core_32[ 6][k] * src[ 6  * i_src] + tab_trans_core_32[10][k] * src[ 10  * i_src] + tab_trans_core_32[14][k] * src[ 14  * i_src] +
                    tab_trans_core_32[18][k] * src[ 18  * i_src] + tab_trans_core_32[22][k] * src[ 22  * i_src] + tab_trans_core_32[26][k] * src[ 26  * i_src] + tab_trans_core_32[30][k] * src[ 30  * i_src];
        }
        for (k = 0; k < 4; k++) {
            EEO[k] = tab_trans_core_32[4][k] * src[ 4  * i_src] + tab_trans_core_32[12][k] * src[ 12  * i_src] + tab_trans_core_32[20][k] * src[ 20  * i_src] + tab_trans_core_32[28][k] * src[ 28  * i_src];
        }
        EEEO[0] = tab_trans_core_32[8][0] * src[ 8  * i_src] + tab_trans_core_32[24][0] * src[ 24  * i_src];
        EEEO[1] = tab_trans_core_32[8][1] * src[ 8  * i_src] + tab_trans_core_32[24][1] * src[ 24  * i_src];
        EEEE[0] = tab_trans_core_32[0][0] * src[ 0  * i_src] + tab_trans_core_32[16][0] * src[ 16  * i_src];
        EEEE[1] = tab_trans_core_32[0][1] * src[ 0  * i_src] + tab_trans_core_32[16][1] * src[ 16  * i_src];
        src++;

        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        EEE[0] = EEEE[0] + EEEO[0];
        EEE[3] = EEEE[0] - EEEO[0];
        EEE[1] = EEEE[1] + EEEO[1];
        EEE[2] = EEEE[1] - EEEO[1];
        for (k = 0; k < 4; k++) {
            EE[k] = EEE[k] + EEO[k];
            EE[k + 4] = EEE[3 - k] - EEO[3 - k];
        }
        for (k = 0; k < 8; k++) {
            E[k] = EE[k] + EO[k];
            E[k + 8] = EE[7 - k] - EO[7 - k];
        }
        for (k = 0; k < 16; k++) {
            dst[k] = (resi_t)Clip3(min_val, max_val, (E[k] + O[k] + add) >> shift);
            dst[k + 16] = (resi_t)Clip3(min_val, max_val, (E[15 - k] - O[15 - k] + add) >> shift);
        }
        dst += i_dst;
    }
}

static void xTr2nd_8_1d_Hor(coef_t *src, int i_src)
{
    int i, j , k, sum;
    int rnd_factor;
    int tmpSrc[4][4];

    rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c8_trans[i][k] * tmpSrc[j][k];
            }
            src[j* i_src + i] = (coef_t)Clip3(-32768, 32767, sum >> 7);
        }
    }
}

static void xTr2nd_8_1d_Vert(coef_t *src, int i_src)
{
    int i, j , k, sum;
    int rnd_factor;
    int tmpSrc[4][4];

    rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c8_trans[i][k] * tmpSrc[k][j];
            }
            src[i* i_src + j] = (coef_t)Clip3(-32768, 32767, sum >> 7);
        }
    }
}

static void xTr2nd_8_1d_Inv_Vert(coef_t *src, int i_src)
{
    int i, j , k, sum;
    int rnd_factor;
    int tmpSrc[4][4];

    rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c8_trans[k][i] * tmpSrc[k][j];
            }
            src[i * i_src + j] = (coef_t)Clip3(-32768, 32767, sum >> 7);
        }
    }
}

static void xTr2nd_8_1d_Inv_Hor(coef_t *src, int i_src)
{
    int i, j , k, sum;
    int rnd_factor;
    int tmpSrc[4][4];

    rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c8_trans[k][i] * tmpSrc[j][k];
            }
            src[j* i_src + i] = (coef_t)Clip3(-32768, 32767, sum >> 7);
        }
    }
}

static void xCTr_4_1d_Hor(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift)
{
    int i, j , k, sum;
    int rnd_factor = shift == 0 ? 0 : 1 << (shift - 1);
    int tmpSrc[4][4];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c4_trans[i][k] * tmpSrc[j][k];
            }
            dst[j * i_dst + i] = (coef_t)Clip3(-32768, 32767, sum >> shift);
        }
    }
}

static void xCTr_4_1d_Vert(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift)
{
    int i, j , k, sum;
    int rnd_factor = shift == 0 ? 0 : 1 << (shift - 1);
    int tmpSrc[4][4];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i* i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c4_trans[i][k] * tmpSrc[k][j];
            }
            dst[i* i_dst + j] = (coef_t)Clip3(-32768, 32767, sum >> shift);
        }
    }
}

static void xCTr_4_1d_Inv_Vert(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift)
{
    int i, j , k, sum;
    int rnd_factor = shift == 0 ? 0 : 1 << (shift - 1);
    int tmpSrc[4][4];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c4_trans[k][i] * tmpSrc[k][j];
            }
            dst[i* i_dst + j] = (coef_t)Clip3(-32768, 32767, sum >> shift);
        }
    }
}

static void xCTr_4_1d_Inv_Hor(coef_t *src, int i_src, coef_t *dst, int i_dst, int shift, int bit_depth)
{
    int i, j , k, sum;
    int rnd_factor = shift == 0 ? 0 : 1 << (shift - 1);
    int tmpSrc[4][4];
    int min_pixel = -(1 << bit_depth);
    int max_pixel = (1 << bit_depth) - 1;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += tab_c4_trans[k][i] * tmpSrc[j][k];
            }
            dst[j* i_dst + i] = (coef_t)Clip3(min_pixel, max_pixel, sum >> shift);
        }
    }
}

static void sub_trans_4x4(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    g_funs_handle.pix_sub[0](resi, org, i_org, pred, i_pred);
    partialButterfly4x4(resi, 4, tmp, 4, shift);
    partialButterfly4x4(tmp, 4, dst, 4, 7);
}

static void sub_trans_4x4_sec(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    g_funs_handle.pix_sub[0](resi, org, i_org, pred, i_pred);
    xCTr_4_1d_Hor(resi, 4, tmp, 4, shift + 1);
    xCTr_4_1d_Vert(tmp, 4, dst, 4, 8);
}

static void sub_trans_8x8(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    g_funs_handle.pix_sub[1](resi, org, i_org, pred, i_pred);
    partialButterfly8x8(resi, 8, tmp, 8, shift);
    partialButterfly8x8(tmp, 8, dst, 8, 8);
}

static void sub_trans_16x16(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    g_funs_handle.pix_sub[2](resi, org, i_org, pred, i_pred);
    partialButterfly16x16(resi, 16, tmp, 16, shift);
    partialButterfly16x16(tmp, 16, dst, 16, 9);
}

static void sub_trans_32x32(pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int shift)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    g_funs_handle.pix_sub[3](resi, org, i_org, pred, i_pred);
    partialButterfly32x32(resi, 32, tmp, 32, shift);
    partialButterfly32x32(tmp, 32, dst, 32, 10);
}

static void add_inv_trans_4x4(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    partialButterflyInverse4x4(src, 4, tmp, 4, 5, 16);
    partialButterflyInverse4x4(tmp, 4, resi, 4, 20 - bit_depth, bit_depth + 1);
    g_funs_handle.pix_add[0](dst, i_dst, pred, i_pred, resi, bit_depth);
}

static void add_inv_trans_4x4_sec(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    xCTr_4_1d_Inv_Vert(src, 4, tmp, 4, 5);
    xCTr_4_1d_Inv_Hor(tmp, 4, resi, 4, 22 - bit_depth, bit_depth + 1);
    g_funs_handle.pix_add[0](dst, i_dst, pred, i_pred, resi, bit_depth);
}

static void add_inv_trans_8x8(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    partialButterflyInverse8x8(src, 8, tmp, 8, 5, 16);
    partialButterflyInverse8x8(tmp, 8, resi, 8, 20 - bit_depth, bit_depth + 1);
    g_funs_handle.pix_add[1](dst, i_dst, pred, i_pred, resi, bit_depth);
}

static void add_inv_trans_16x16(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    partialButterflyInverse16x16(src, 16, tmp, 16, 5, 16);
    partialButterflyInverse16x16(tmp, 16, resi, 16, 20 - bit_depth, bit_depth + 1);
    g_funs_handle.pix_add[2](dst, i_dst, pred, i_pred, resi, bit_depth);
}

static void add_inv_trans_32x32(coef_t *src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    ALIGNED_32(coef_t tmp[LCU_SIZE * LCU_SIZE]);
    ALIGNED_32(resi_t resi[LCU_SIZE * LCU_SIZE]);
    partialButterflyInverse32x32(src, 32, tmp, 32, 5, 16);
    partialButterflyInverse32x32(tmp, 32, resi, 32, 20 - bit_depth, bit_depth + 1);
    g_funs_handle.pix_add[3](dst, i_dst, pred, i_pred, resi, bit_depth);
}

static int com_get_forward_trans_shift(int log2_size, int type, int bit_depth)
{
    assert(log2_size <= 6);
    return (type == 0) ? (log2_size + bit_depth - 10) : (log2_size + 5);
}

void tu_trans(int idx, int tu_bitsize, pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int bit_depth)
{
    ALIGNED_32(coef_t coef_temp[32 * 32]);
    ALIGNED_32(coef_t resi[32 * 32]);
    coef_t *p = resi;
    int size = 1 << tu_bitsize;

    g_funs_handle.pix_sub[tu_bitsize - 2](resi, org, i_org, pred, i_pred);

    int shift1 = com_get_forward_trans_shift(tu_bitsize, 0, bit_depth);
    int shift2 = com_get_forward_trans_shift(tu_bitsize, 1, bit_depth);
    g_funs_handle.trans_dct8_dst7[idx  & 1][tu_bitsize - 1](resi, coef_temp, shift1);
    g_funs_handle.trans_dct8_dst7[idx >> 1][tu_bitsize - 1](coef_temp, dst, shift2);
}

void transform_blk(avs3_enc_t *h, int blkidx, pel_t *org, int i_org, pel_t *pred, int i_pred, coef_t *dst, int i_dst, int tu_bitsize, cu_t *cu, int b_chroma)
{
    int sec_trans = h->input->tools.use_secT;
    int shift = h->input->bit_depth + tu_bitsize - 10;

    int is_tu = blkidx > 0;
    blkidx--;

    if (is_tu) {
        tu_trans(blkidx, tu_bitsize, org, i_org, pred, i_pred, dst, h->input->bit_depth);
    } else {
        if (tu_bitsize == 2 && IS_INTRA(cu) && (!b_chroma) && sec_trans) {
            g_funs_handle.sub_trans[0](org, i_org, pred, i_pred, dst, shift);
        }
        else if (!h->analyzer.use_rdoq && (CHECK_FAST_TRQ(1 << tu_bitsize, h->type))) {
            g_funs_handle.sub_trans_ext[tu_bitsize - 1](org, i_org, pred, i_pred, dst, shift);
        }
        else {
            g_funs_handle.sub_trans[tu_bitsize - 1](org, i_org, pred, i_pred, dst, shift);
        }
    }

    if (IS_INTRA(cu) && sec_trans && (!b_chroma) && tu_bitsize >= 3) {
        int vt, ht;
        int uiMode = cu->ipred_mode_real;
        int block_available_up = h->tu_available_up;
        int block_available_left = h->tu_available_left;
        vt = (uiMode >= 0 && uiMode <= 23);
        ht = (uiMode >= 13 && uiMode <= 32) || (uiMode >= 0 && uiMode <= 2);
        vt = vt && block_available_up;
        ht = ht && block_available_left;
        if (vt) {
            g_funs_handle.trans_2nd_Ver(dst, i_dst);
        }
        if (ht) {
            g_funs_handle.trans_2nd_Hor(dst, i_dst);
        }
    }
}

static int get_inv_trans_shift(int log2_size, int type, int bit_depth)
{
    return ((type == 0) ? 5 : (15 + 5 - bit_depth));
}

void tu_inv_trans(int idx, int tu_bitsize, coef_t *coef, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int bit_depth)
{
    ALIGNED_32(coef_t coef_temp[32 * 32]);
    ALIGNED_32(coef_t resi[32 * 32]);
    int size = 1 << tu_bitsize;
    int shift1 = get_inv_trans_shift(tu_bitsize, 0, bit_depth);
    int shift2 = get_inv_trans_shift(tu_bitsize, 1, bit_depth);

    int max_tr_val = (1 << 15) - 1;
    int min_tr_val = -(1 << 15);
    g_funs_handle.itrans_dct8_dst7[idx >> 1][tu_bitsize - 1](coef, coef_temp, shift1, 1 << tu_bitsize, max_tr_val, min_tr_val);

    max_tr_val = (1 << bit_depth) - 1;
    min_tr_val = -(1 << bit_depth);
    g_funs_handle.itrans_dct8_dst7[idx & 1][tu_bitsize - 1](coef_temp, resi, shift2, 1 << tu_bitsize, max_tr_val, min_tr_val);
    g_funs_handle.pix_add[tu_bitsize - 2](dst, i_dst, pred, i_pred, resi, bit_depth);
}

void inv_transform(avs3_enc_t *h, int blkidx, coef_t *src, int i_src, pel_t *pred, int i_pred, pel_t *dst, int i_dst, int tu_bitsize, cu_t *cu, int b_chroma)
{
    int sec_trans = h->input->tools.use_secT;

    int is_tu = blkidx > 0;
    blkidx--;

    if (IS_INTRA(cu) && sec_trans && (!b_chroma) && tu_bitsize >= 3) {
        int vt, ht;
        int uiMode = cu->ipred_mode_real;
        int block_available_up = h->tu_available_up;
        int block_available_left = h->tu_available_left;
        vt = (uiMode >= 0 && uiMode <= 23);
        ht = (uiMode >= 13 && uiMode <= 32) || (uiMode >= 0 && uiMode <= 2);
        vt = vt && block_available_up;
        ht = ht && block_available_left;
        if (ht) {
            g_funs_handle.inv_trans_2nd_Hor(src, i_src);
        }
        if (vt) {
            g_funs_handle.inv_trans_2nd_Ver(src, i_src);
        }
    }

    if (is_tu) {
        tu_inv_trans(blkidx, tu_bitsize, src, pred, i_pred, dst, i_dst, h->input->bit_depth);
    } else {
        if (tu_bitsize == 2 && IS_INTRA(cu) && (!b_chroma) && sec_trans) {
            g_funs_handle.add_inv_trans[0](src, pred, i_pred, dst, i_dst, h->input->bit_depth);
        }
        else if (!h->analyzer.use_rdoq && (CHECK_FAST_TRQ(1 << tu_bitsize, h->type))) {
            g_funs_handle.add_inv_trans_ext[tu_bitsize - 1](src, pred, i_pred, dst, i_dst, h->input->bit_depth);
        }
        else {
            g_funs_handle.add_inv_trans[tu_bitsize - 1](src, pred, i_pred, dst, i_dst, h->input->bit_depth);
        }
    }
}

/******************   DCT-8   ******************************************/

static void tx_dct8_pb4(coef_t *src, coef_t *dst, int shift)  // input src, output dst
{
    int i;
    int rnd_factor = 1 << (shift - 1);

    char_t *iT = com_tbl_tm4[DCT8][0];

    for (i = 0; i < 4; i++) {
        dst[0 * 4] = (coef_t)(((iT[2] + iT[3]) * src[0] + iT[1] * src[1] + iT[2] * src[2] + iT[3] * src[3] + rnd_factor) >> shift);
        dst[1 * 4] = (coef_t)((iT[1] * src[0] + (-iT[1]) * src[2] + (-iT[1]) * src[3] + rnd_factor) >> shift);
        dst[2 * 4] = (coef_t)((iT[2] * src[0] + (-iT[1]) * src[1] + (-iT[3]) * src[2] + (iT[2] + iT[3]) * src[3] + rnd_factor) >> shift);
        dst[3 * 4] = (coef_t)((iT[3] * src[0] + (-iT[1]) * src[1] + (iT[2] + iT[3]) * src[2] + (-iT[2]) * src[3] + rnd_factor) >> shift);

        src += 4;
        dst++;
    }
}

static void tx_dct8_pb8(coef_t *src, coef_t *dst, int shift)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 8;
    char_t *iT;
    coef_t *pCoef;

    const int  cutoff = uiTrSize;

    for (i = 0; i < 8; i++) {
        pCoef = dst;
        iT = com_tbl_tm8[DCT8][0];

        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (coef_t)((iSum + rnd_factor) >> shift);
            pCoef += 8;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

static void tx_dct8_pb16(coef_t *src, coef_t *dst, int shift)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 16;
    char_t *iT;
    coef_t *pCoef;

    const int  cutoff = uiTrSize;

    for (i = 0; i < 16; i++) {
        pCoef = dst;
        iT = com_tbl_tm16[DCT8][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (coef_t)((iSum + rnd_factor) >> shift);
            pCoef += 16;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

/******************   DST-7   ******************************************/

static void tx_dst7_pb4(coef_t *src, coef_t *dst, int shift)  // input src, output dst
{
    int i;
    int rnd_factor = 1 << (shift - 1);

    char_t *iT = com_tbl_tm4[DST7][0];

    for (i = 0; i < 4; i++) {
        dst[0 * 4] = (coef_t)((iT[0] * src[0] + iT[1] * src[1] + iT[2] * src[2] + (iT[0] + iT[1]) * src[3] + rnd_factor) >> shift);
        dst[1 * 4] = (coef_t)((iT[2] * (src[0] + src[1] - src[3]) + rnd_factor) >> shift);
        dst[2 * 4] = (coef_t)(((iT[0] + iT[1]) * src[0] + (-iT[0]) * src[1] + (-iT[2]) * src[2] + iT[1] * src[3] + rnd_factor) >> shift);
        dst[3 * 4] = (coef_t)((iT[1] * src[0] + (-iT[0] - iT[1]) * src[1] + iT[2] * src[2] + (-iT[0]) * src[3] + rnd_factor) >> shift);

        src += 4;
        dst++;
    }
}

static void tx_dst7_pb8(coef_t *src, coef_t *dst, int shift)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 8;
    char_t *iT;
    coef_t *pCoef;

    const int  cutoff = uiTrSize;
    for (i = 0; i < 8; i++) {
        pCoef = dst;
        iT = com_tbl_tm8[DST7][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (coef_t)((iSum + rnd_factor) >> shift);
            pCoef += 8;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

static void tx_dst7_pb16(coef_t *src, coef_t *dst, int shift)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 16;
    char_t *iT;
    coef_t *pCoef;

    const int  cutoff = uiTrSize;
    for (i = 0; i < 16; i++) {
        pCoef = dst;
        iT = com_tbl_tm16[DST7][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (coef_t)((iSum + rnd_factor) >> shift);
            pCoef += 16;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

void com_dct_coef_create()
{
    int i, c = 2;
    const double PI = 3.14159265358979323846;

    for (i = 0; i < 6; i++) {
        char_t *iT = NULL;
        const double s = sqrt((double)c) * 32;
        switch (i) {
        case 0:
            iT = com_tbl_tm2[0][0];
            break;
        case 1:
            iT = com_tbl_tm4[0][0];
            break;
        case 2:
            iT = com_tbl_tm8[0][0];
            break;
        case 3:
            iT = com_tbl_tm16[0][0];
            break;
        case 4:
            iT = com_tbl_tm32[0][0];
            break;
        case 5:
            iT = com_tbl_tm64[0][0];
            break;
        default:
            exit(0);
            break;
        }

        for (int k = 0; k < c; k++) {
            for (int n = 0; n < c; n++) {
                double w0, v;

                // DCT-II
                w0 = k == 0 ? sqrt(0.5) : 1;
                v = cos(PI * (n + 0.5) * k / c) * w0 * sqrt(2.0 / c);
                iT[DCT2 *c *c + k * c + n] = (char_t)(s * v + (v > 0 ? 0.5 : -0.5));

                // DCT-VIII
                v = cos(PI * (k + 0.5) * (n + 0.5) / (c + 0.5)) * sqrt(2.0 / (c + 0.5));
                iT[DCT8 *c *c + k * c + n] = (char_t)(s * v + (v > 0 ? 0.5 : -0.5));

                // DST-VII
                v = sin(PI * (k + 0.5) * (n + 1) / (c + 0.5)) * sqrt(2.0 / (c + 0.5));
                iT[DST7 *c *c + k * c + n] = (char_t)(s * v + (v > 0 ? 0.5 : -0.5));

            }
        }
        c <<= 1;
    }

    com_tbl_sqrt[0] = sqrt(4.0 * 8.0);
    com_tbl_sqrt[1] = sqrt(16.0 * 8.0);
}

/******************   DCT-8   ******************************************/

static void itx_dct8_pb4(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)  // input tmp, output block
{
    int i;
    int rnd_factor = 1 << (shift - 1);

    char_t *iT = com_tbl_tm4[DCT8][0];

    int c[4];
    const int  reducedLine = line;
    for (i = 0; i < reducedLine; i++) {
        // Intermediate Variables
        c[0] = coeff[0 * line] + coeff[3 * line];
        c[1] = coeff[2 * line] + coeff[0 * line];
        c[2] = coeff[3 * line] - coeff[2 * line];
        c[3] = iT[1] * coeff[1 * line];

        block[0] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[3] * c[0] + iT[2] * c[1] + c[3] + rnd_factor) >> shift);
        block[1] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[1] * (coeff[0 * line] - coeff[2 * line] - coeff[3 * line]) + rnd_factor) >> shift);
        block[2] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[3] * c[2] + iT[2] * c[0] - c[3] + rnd_factor) >> shift);
        block[3] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[3] * c[1] - iT[2] * c[2] - c[3] + rnd_factor) >> shift);

        block += 4;
        coeff++;
    }
}

static void itx_dct8_pb8(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 8;
    char_t *iT = com_tbl_tm8[DCT8][0];
    const int  reducedLine = line;
    const int  cutoff = 8;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dct8_pb16(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 16;
    char_t *iT = com_tbl_tm16[DCT8][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

/******************   DST-7   ******************************************/
static void itx_dst7_pb4(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)  // input tmp, output block
{
    int i, c[4];
    int rnd_factor = 1 << (shift - 1);
    char_t *iT = com_tbl_tm4[DST7][0];
    const int  reducedLine = line;

    for (i = 0; i < reducedLine; i++) {
        // Intermediate Variables

        c[0] = coeff[0 * line] + coeff[2 * line];
        c[1] = coeff[2 * line] + coeff[3 * line];
        c[2] = coeff[0 * line] - coeff[3 * line];
        c[3] = iT[2] * coeff[1 * line];

        block[0] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[0] * c[0] + iT[1] * c[1] + c[3] + rnd_factor) >> shift);
        block[1] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[1] * c[2] - iT[0] * c[1] + c[3] + rnd_factor) >> shift);
        block[2] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[2] * (coeff[0 * line] - coeff[2 * line] + coeff[3 * line]) + rnd_factor) >> shift);
        block[3] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (iT[1] * c[0] + iT[0] * c[2] - c[3] + rnd_factor) >> shift);

        block += 4;
        coeff++;
    }
}

static void itx_dst7_pb8(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 8;
    char_t *iT = com_tbl_tm8[DST7][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dst7_pb16(coef_t *coeff, coef_t *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 16;
    char_t *iT = com_tbl_tm16[DST7][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (coef_t)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static int get_nz_num(coef_t *p_coef, int num_coeff)
{
    int num_sig = 0;

    for (int i = 0; i < num_coeff; i++) {
        num_sig += !!p_coef[i];
    }

    return num_sig;
}

void com_funs_init_transform()
{
    com_dct_coef_create();

    g_funs_handle.add_inv_trans[0] = add_inv_trans_4x4_sec;
    g_funs_handle.add_inv_trans[1] = add_inv_trans_4x4;
    g_funs_handle.add_inv_trans[2] = add_inv_trans_8x8;
    g_funs_handle.add_inv_trans[3] = add_inv_trans_16x16;
    g_funs_handle.add_inv_trans[4] = add_inv_trans_32x32;
    g_funs_handle.inv_trans_2nd_Hor = xTr2nd_8_1d_Inv_Hor;
    g_funs_handle.inv_trans_2nd_Ver = xTr2nd_8_1d_Inv_Vert;

    g_funs_handle.sub_trans[0] = sub_trans_4x4_sec;
    g_funs_handle.sub_trans[1] = sub_trans_4x4;
    g_funs_handle.sub_trans[2] = sub_trans_8x8;
    g_funs_handle.sub_trans[3] = sub_trans_16x16;
    g_funs_handle.sub_trans[4] = sub_trans_32x32;
    g_funs_handle.trans_2nd_Hor = xTr2nd_8_1d_Hor;
    g_funs_handle.trans_2nd_Ver = xTr2nd_8_1d_Vert;

    g_funs_handle.add_inv_trans_ext[4] = add_inv_trans_32x32;
    g_funs_handle.sub_trans_ext[4] = sub_trans_32x32;

    g_funs_handle.pix_sub[0] = pix_sub_4;
    g_funs_handle.pix_add[0] = pix_add_4;
    g_funs_handle.pix_sub[1] = pix_sub_8;
    g_funs_handle.pix_add[1] = pix_add_8;
    g_funs_handle.pix_sub[2] = pix_sub_16;
    g_funs_handle.pix_add[2] = pix_add_16;
    g_funs_handle.pix_sub[3] = pix_sub_32;
    g_funs_handle.pix_add[3] = pix_add_32;

    g_funs_handle.itrans_dct8_dst7[0][1] = itx_dct8_pb4;
    g_funs_handle.itrans_dct8_dst7[0][2] = itx_dct8_pb8;
    g_funs_handle.itrans_dct8_dst7[0][3] = itx_dct8_pb16;

    g_funs_handle.itrans_dct8_dst7[1][1] = itx_dst7_pb4;
    g_funs_handle.itrans_dct8_dst7[1][2] = itx_dst7_pb8;
    g_funs_handle.itrans_dct8_dst7[1][3] = itx_dst7_pb16;

    g_funs_handle.trans_dct8_dst7[0][1] = tx_dct8_pb4;
    g_funs_handle.trans_dct8_dst7[0][2] = tx_dct8_pb8;
    g_funs_handle.trans_dct8_dst7[0][3] = tx_dct8_pb16;

    g_funs_handle.trans_dct8_dst7[1][1] = tx_dst7_pb4;
    g_funs_handle.trans_dct8_dst7[1][2] = tx_dst7_pb8;
    g_funs_handle.trans_dct8_dst7[1][3] = tx_dst7_pb16;

    g_funs_handle.get_nz_num = get_nz_num;
}