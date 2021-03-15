#include "commonVariables.h"


// ====================================================================================================================
// Distortion functions
// ====================================================================================================================

#define DEFINE_SAD(w) \
static i32u_t xGetSAD##w(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height, int skip_lines) {  \
    int i, iSubStep = (1 << skip_lines);                                                                    \
    i32u_t uiSum = 0;                                                                                       \
    i_org *= iSubStep;                                                                                      \
    i_pred *= iSubStep;                                                                                     \
    for (; height != 0; height -= iSubStep) {                                                               \
        for (i = 0; i < w; i++) {                                                                           \
            uiSum += abs(p_org[i] - p_pred[i]);                                                             \
        }                                                                                                   \
        p_org += i_org;                                                                                     \
        p_pred += i_pred;                                                                                   \
    }                                                                                                       \
    uiSum <<= skip_lines;                                                                                   \
    return uiSum;                                                                                           \
}

DEFINE_SAD(4)
DEFINE_SAD(8)
DEFINE_SAD(12)
DEFINE_SAD(16)
DEFINE_SAD(24)
DEFINE_SAD(32)
DEFINE_SAD(48)
DEFINE_SAD(64)

#define DEFINE_SAD_X4(w) \
static void xGetSAD##w##_x4(pel_t *p_org, int i_org, pel_t *pred0, pel_t *pred1,                            \
                                                     pel_t *pred2, pel_t *pred3,                            \
                                                  int i_pred, i32u_t sad[4], int height, int skip_lines) {  \
    int i, iSubStep = (1 << skip_lines);                                                                    \
    i32u_t uiSum0 = 0, uiSum1 = 0, uiSum2 = 0, uiSum3 = 0;                                                  \
    i_org *= iSubStep;                                                                                      \
    i_pred *= iSubStep;                                                                                     \
    for (; height != 0; height -= iSubStep) {                                                               \
        for (i = 0; i < w; i++) {                                                                           \
            uiSum0 += abs(p_org[i] - pred0[i]);                                                             \
            uiSum1 += abs(p_org[i] - pred1[i]);                                                             \
            uiSum2 += abs(p_org[i] - pred2[i]);                                                             \
            uiSum3 += abs(p_org[i] - pred3[i]);                                                             \
        }                                                                                                   \
        p_org += i_org;                                                                                     \
        pred0 += i_pred;                                                                                    \
        pred1 += i_pred;                                                                                    \
        pred2 += i_pred;                                                                                    \
        pred3 += i_pred;                                                                                    \
    }                                                                                                       \
    sad[0] = uiSum0 << skip_lines;                                                                          \
    sad[1] = uiSum1 << skip_lines;                                                                          \
    sad[2] = uiSum2 << skip_lines;                                                                          \
    sad[3] = uiSum3 << skip_lines;                                                                          \
}

DEFINE_SAD_X4(4)
DEFINE_SAD_X4(8)
DEFINE_SAD_X4(12)
DEFINE_SAD_X4(16)
DEFINE_SAD_X4(24)
DEFINE_SAD_X4(32)
DEFINE_SAD_X4(48)
DEFINE_SAD_X4(64)

#define DEFINE_AVG_SAD(w) \
static i32u_t xGetAVGSAD##w(pel_t *p_org, int i_org, pel_t *p_pred1, int i_pred1,                               \
                                                     pel_t *p_pred2, int i_pred2, int height, int skip_lines) { \
    int i, iSubStep = (1 << skip_lines);                                                                        \
    i32u_t uiSum = 0;                                                                                           \
    i_org *= iSubStep;                                                                                          \
    i_pred1 *= iSubStep;                                                                                        \
    i_pred2 *= iSubStep;                                                                                        \
    for (; height != 0; height -= iSubStep) {                                                                   \
        for (i = 0; i < w; i++) {                                                                               \
            uiSum += abs(p_org[i] - ((p_pred1[i] + p_pred2[i] + 1) >> 1));                                      \
        }                                                                                                       \
        p_org += i_org;                                                                                         \
        p_pred1 += i_pred1;                                                                                     \
        p_pred2 += i_pred2;                                                                                     \
    }                                                                                                           \
    uiSum <<= skip_lines;                                                                                       \
    return uiSum;                                                                                               \
}

DEFINE_AVG_SAD(4)
DEFINE_AVG_SAD(8)
DEFINE_AVG_SAD(16)
DEFINE_AVG_SAD(32)
DEFINE_AVG_SAD(64)

// --------------------------------------------------------------------------------------------------------------------
// SSE
// --------------------------------------------------------------------------------------------------------------------

static i32u_t xGetSSE4(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    int  iTemp;

    for (; height != 0; height--) {

        iTemp = p_org[0] - p_pred[0];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[1] - p_pred[1];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[2] - p_pred[2];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[3] - p_pred[3];
        uiSum += (iTemp * iTemp);

        p_org += i_org;
        p_pred += i_pred;
    }

    return (uiSum);
}

static i32u_t xGetSSE8(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    int  iTemp;

    for (; height != 0; height--) {
        iTemp = p_org[0] - p_pred[0];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[1] - p_pred[1];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[2] - p_pred[2];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[3] - p_pred[3];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[4] - p_pred[4];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[5] - p_pred[5];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[6] - p_pred[6];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[7] - p_pred[7];
        uiSum += (iTemp * iTemp);

        p_org += i_org;
        p_pred += i_pred;
    }

    return (uiSum);
}

static i32u_t xGetSSE16(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    int  iTemp;

    for (; height != 0; height--) {

        iTemp = p_org[0] - p_pred[0];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[1] - p_pred[1];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[2] - p_pred[2];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[3] - p_pred[3];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[4] - p_pred[4];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[5] - p_pred[5];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[6] - p_pred[6];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[7] - p_pred[7];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[8] - p_pred[8];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[9] - p_pred[9];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[10] - p_pred[10];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[11] - p_pred[11];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[12] - p_pred[12];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[13] - p_pred[13];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[14] - p_pred[14];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[15] - p_pred[15];
        uiSum += (iTemp * iTemp);

        p_org += i_org;
        p_pred += i_pred;
    }

    return (uiSum);
}

static i32u_t xGetSSE32(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    int  iTemp;

    for (; height != 0; height--) {

        iTemp = p_org[0] - p_pred[0];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[1] - p_pred[1];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[2] - p_pred[2];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[3] - p_pred[3];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[4] - p_pred[4];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[5] - p_pred[5];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[6] - p_pred[6];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[7] - p_pred[7];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[8] - p_pred[8];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[9] - p_pred[9];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[10] - p_pred[10];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[11] - p_pred[11];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[12] - p_pred[12];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[13] - p_pred[13];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[14] - p_pred[14];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[15] - p_pred[15];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[16] - p_pred[16];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[17] - p_pred[17];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[18] - p_pred[18];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[19] - p_pred[19];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[20] - p_pred[20];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[21] - p_pred[21];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[22] - p_pred[22];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[23] - p_pred[23];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[24] - p_pred[24];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[25] - p_pred[25];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[26] - p_pred[26];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[27] - p_pred[27];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[28] - p_pred[28];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[29] - p_pred[29];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[30] - p_pred[30];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[31] - p_pred[31];
        uiSum += (iTemp * iTemp);

        p_org += i_org;
        p_pred += i_pred;
    }

    return (uiSum);
}

static i32u_t xGetSSE64(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int height)
{
    i32u_t uiSum = 0;
    int  iTemp;

    for (; height != 0; height--) {
        iTemp = p_org[0] - p_pred[0];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[1] - p_pred[1];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[2] - p_pred[2];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[3] - p_pred[3];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[4] - p_pred[4];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[5] - p_pred[5];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[6] - p_pred[6];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[7] - p_pred[7];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[8] - p_pred[8];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[9] - p_pred[9];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[10] - p_pred[10];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[11] - p_pred[11];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[12] - p_pred[12];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[13] - p_pred[13];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[14] - p_pred[14];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[15] - p_pred[15];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[16] - p_pred[16];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[17] - p_pred[17];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[18] - p_pred[18];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[19] - p_pred[19];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[20] - p_pred[20];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[21] - p_pred[21];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[22] - p_pred[22];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[23] - p_pred[23];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[24] - p_pred[24];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[25] - p_pred[25];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[26] - p_pred[26];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[27] - p_pred[27];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[28] - p_pred[28];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[29] - p_pred[29];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[30] - p_pred[30];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[31] - p_pred[31];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[32] - p_pred[32];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[33] - p_pred[33];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[34] - p_pred[34];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[35] - p_pred[35];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[36] - p_pred[36];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[37] - p_pred[37];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[38] - p_pred[38];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[39] - p_pred[39];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[40] - p_pred[40];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[41] - p_pred[41];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[42] - p_pred[42];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[43] - p_pred[43];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[44] - p_pred[44];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[45] - p_pred[45];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[46] - p_pred[46];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[47] - p_pred[47];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[48] - p_pred[48];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[49] - p_pred[49];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[50] - p_pred[50];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[51] - p_pred[51];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[52] - p_pred[52];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[53] - p_pred[53];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[54] - p_pred[54];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[55] - p_pred[55];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[56] - p_pred[56];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[57] - p_pred[57];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[58] - p_pred[58];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[59] - p_pred[59];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[60] - p_pred[60];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[61] - p_pred[61];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[62] - p_pred[62];
        uiSum += (iTemp * iTemp);
        iTemp = p_org[63] - p_pred[63];
        uiSum += (iTemp * iTemp);

        p_org += i_org;
        p_pred += i_pred;
    }

    return (uiSum);
}

static i32u_t xGetSSE_Ext(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight)
{
    i32u_t uiSSD = 0;
    int x, y;

    int iTemp;

    for (y = 0; y < iHeight; y++) {
        for (x = 0; x < iWidth; x++) {
            iTemp = p_org[x] - p_pred[x];
            uiSSD += iTemp * iTemp;
        }
        p_org += i_org;
        p_pred += i_pred;
    }

    return uiSSD;;
}

static i64u_t xGetSSE_Psnr(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred, int iWidth, int iHeight)
{
    i64u_t uiSSD = 0;
    int x, y;

    int iTemp;

    for (y = 0; y < iHeight; y++) {
        for (x = 0; x < iWidth; x++) {
            iTemp = p_org[x] - p_pred[x];
            uiSSD += iTemp * iTemp;
        }
        p_org  += i_org;
        p_pred += i_pred;
    }

    return uiSSD;;
}


// --------------------------------------------------------------------------------------------------------------------
// HADAMARD with step (used in fractional search)
// --------------------------------------------------------------------------------------------------------------------
static i32u_t xCalcHAD4x4(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int k, uiSum = 0, diff[16], m[16], d[16];

    for (k = 0; k < 16; k += 4) {
        diff[k + 0] = p_org[0] - p_pred[0];
        diff[k + 1] = p_org[1] - p_pred[1];
        diff[k + 2] = p_org[2] - p_pred[2];
        diff[k + 3] = p_org[3] - p_pred[3];

        p_pred += i_pred;
        p_org += i_org;
    }

    /*===== hadamard transform =====*/
    m[0] = diff[0] + diff[12];
    m[1] = diff[1] + diff[13];
    m[2] = diff[2] + diff[14];
    m[3] = diff[3] + diff[15];
    m[4] = diff[4] + diff[8];
    m[5] = diff[5] + diff[9];
    m[6] = diff[6] + diff[10];
    m[7] = diff[7] + diff[11];
    m[8] = diff[4] - diff[8];
    m[9] = diff[5] - diff[9];
    m[10] = diff[6] - diff[10];
    m[11] = diff[7] - diff[11];
    m[12] = diff[0] - diff[12];
    m[13] = diff[1] - diff[13];
    m[14] = diff[2] - diff[14];
    m[15] = diff[3] - diff[15];

    d[0] = m[0] + m[4];
    d[1] = m[1] + m[5];
    d[2] = m[2] + m[6];
    d[3] = m[3] + m[7];
    d[4] = m[8] + m[12];
    d[5] = m[9] + m[13];
    d[6] = m[10] + m[14];
    d[7] = m[11] + m[15];
    d[8] = m[0] - m[4];
    d[9] = m[1] - m[5];
    d[10] = m[2] - m[6];
    d[11] = m[3] - m[7];
    d[12] = m[12] - m[8];
    d[13] = m[13] - m[9];
    d[14] = m[14] - m[10];
    d[15] = m[15] - m[11];

    m[0] = d[0] + d[3];
    m[1] = d[1] + d[2];
    m[2] = d[1] - d[2];
    m[3] = d[0] - d[3];
    m[4] = d[4] + d[7];
    m[5] = d[5] + d[6];
    m[6] = d[5] - d[6];
    m[7] = d[4] - d[7];
    m[8] = d[8] + d[11];
    m[9] = d[9] + d[10];
    m[10] = d[9] - d[10];
    m[11] = d[8] - d[11];
    m[12] = d[12] + d[15];
    m[13] = d[13] + d[14];
    m[14] = d[13] - d[14];
    m[15] = d[12] - d[15];

    d[0] = m[0] + m[1];
    d[1] = m[0] - m[1];
    d[2] = m[2] + m[3];
    d[3] = m[3] - m[2];
    d[4] = m[4] + m[5];
    d[5] = m[4] - m[5];
    d[6] = m[6] + m[7];
    d[7] = m[7] - m[6];
    d[8] = m[8] + m[9];
    d[9] = m[8] - m[9];
    d[10] = m[10] + m[11];
    d[11] = m[11] - m[10];
    d[12] = m[12] + m[13];
    d[13] = m[12] - m[13];
    d[14] = m[14] + m[15];
    d[15] = m[15] - m[14];

    for (k = 0; k < 16; ++k) {
        uiSum += abs(d[k]);
    }
    uiSum = ((uiSum + 1) >> 1);

    return uiSum;
}

static i32u_t xCalcHAD8x8(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    int k, i, j, jj, uiSum = 0;
    int diff[64], m1[8][8], m2[8][8], m3[8][8];

    for (k = 0; k < 64; k += 8) {
        diff[k + 0] = p_org[0] - p_pred[0];
        diff[k + 1] = p_org[1] - p_pred[1];
        diff[k + 2] = p_org[2] - p_pred[2];
        diff[k + 3] = p_org[3] - p_pred[3];
        diff[k + 4] = p_org[4] - p_pred[4];
        diff[k + 5] = p_org[5] - p_pred[5];
        diff[k + 6] = p_org[6] - p_pred[6];
        diff[k + 7] = p_org[7] - p_pred[7];

        p_pred += i_pred;
        p_org  += i_org;
    }

    //horizontal
    for (j = 0; j < 8; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj    ] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj    ] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];

        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];

        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }

    //vertical
    for (i = 0; i < 8; i++) {
        m3[0][i] = m2[0][i] + m2[4][i];
        m3[1][i] = m2[1][i] + m2[5][i];
        m3[2][i] = m2[2][i] + m2[6][i];
        m3[3][i] = m2[3][i] + m2[7][i];
        m3[4][i] = m2[0][i] - m2[4][i];
        m3[5][i] = m2[1][i] - m2[5][i];
        m3[6][i] = m2[2][i] - m2[6][i];
        m3[7][i] = m2[3][i] - m2[7][i];

        m1[0][i] = m3[0][i] + m3[2][i];
        m1[1][i] = m3[1][i] + m3[3][i];
        m1[2][i] = m3[0][i] - m3[2][i];
        m1[3][i] = m3[1][i] - m3[3][i];
        m1[4][i] = m3[4][i] + m3[6][i];
        m1[5][i] = m3[5][i] + m3[7][i];
        m1[6][i] = m3[4][i] - m3[6][i];
        m1[7][i] = m3[5][i] - m3[7][i];

        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            uiSum += abs(m2[i][j]);
        }
    }

    uiSum = ((uiSum + 2) >> 2);

    return uiSum;
}

static i32u_t xCalcHADs8x8_I(pel_t *p_org, int i_org)
{
    int k, i, j, jj;
    int diff[64], m1[8][8], m2[8][8], m3[8][8], iSumHad = 0;

    for (k = 0; k < 64; k += 8)
    {
        diff[k + 0] = p_org[0];
        diff[k + 1] = p_org[1];
        diff[k + 2] = p_org[2];
        diff[k + 3] = p_org[3];
        diff[k + 4] = p_org[4];
        diff[k + 5] = p_org[5];
        diff[k + 6] = p_org[6];
        diff[k + 7] = p_org[7];

        p_org += i_org;
    }

    //horizontal
    for (j = 0; j < 8; j++)
    {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];

        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];

        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }

    //vertical
    for (i = 0; i < 8; i++)
    {
        m3[0][i] = m2[0][i] + m2[4][i];
        m3[1][i] = m2[1][i] + m2[5][i];
        m3[2][i] = m2[2][i] + m2[6][i];
        m3[3][i] = m2[3][i] + m2[7][i];
        m3[4][i] = m2[0][i] - m2[4][i];
        m3[5][i] = m2[1][i] - m2[5][i];
        m3[6][i] = m2[2][i] - m2[6][i];
        m3[7][i] = m2[3][i] - m2[7][i];

        m1[0][i] = m3[0][i] + m3[2][i];
        m1[1][i] = m3[1][i] + m3[3][i];
        m1[2][i] = m3[0][i] - m3[2][i];
        m1[3][i] = m3[1][i] - m3[3][i];
        m1[4][i] = m3[4][i] + m3[6][i];
        m1[5][i] = m3[5][i] + m3[7][i];
        m1[6][i] = m3[4][i] - m3[6][i];
        m1[7][i] = m3[5][i] - m3[7][i];

        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }

#if COMPILE_10BIT
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            iSumHad += abs(m2[i][j]);
        }
    }
#else
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            iSumHad += abs(m2[i][j]);
        }
}
#endif
    iSumHad -= abs(m2[0][0]);
    iSumHad = (iSumHad + 2) >> 2;
    return(iSumHad);
}

static i32u_t xCalcHAD8x16(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    return xCalcHAD8x8(p_org,     i_org, p_pred,     i_pred) +
           xCalcHAD8x8(p_org + 8, i_org, p_pred + 8, i_pred);
}

static i32u_t xCalcHAD16x8(pel_t *p_org, int i_org, pel_t *p_pred, int i_pred)
{
    return xCalcHAD8x8(p_org,             i_org, p_pred,              i_pred) +
           xCalcHAD8x8(p_org + 8 * i_org, i_org, p_pred + 8 * i_pred, i_pred);
}

static i32u_t  xCalcHAD8x2(pel_t *org, int i_org, pel_t * rec, int i_rec)
{
    int satd = 0;
    int i, j, jj;
    int diff[257], m1[2][8], m2[2][8];

    for (j = 0; j < 8; j++) {
        for (i = 0; i < 2; i++) {
            diff[j * 2 + i] = org[i] - rec[i];
        }
        org += i_org;
        rec += i_rec;
    }

    //horizontal
    for (j = 0; j < 2; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];

        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];

        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }

    //vertical
    for (i = 0; i < 8; i++) {
        m1[0][i] = m2[0][i] + m2[1][i];
        m1[1][i] = m2[0][i] - m2[1][i];
    }

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 8; i++) {
            satd += (abs(m1[j][i]));
        }
    }

    satd = ((satd + 1) >> 1);

    return satd;
}

static i32u_t  xCalcHAD2x8(pel_t *org, int i_org, pel_t * rec, int i_rec)
{
    int satd = 0;
    int i, j, jj;
    int diff[257], m1[8][2], m2[8][2];

    // Hadamard8x8
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 8; i++) {
            diff[j * 8 + i] = org[i] - rec[i];
        }
        org += i_org;
        rec += i_rec;
    }


    //horizontal
    for (j = 0; j < 8; j++) {
        jj = j << 1;
        m1[j][0] = diff[jj] + diff[jj + 1];
        m1[j][1] = diff[jj] - diff[jj + 1];
    }

    //vertical
    for (i = 0; i < 2; i++) {
        m2[0][i] = m1[0][i] + m1[4][i];
        m2[1][i] = m1[1][i] + m1[5][i];
        m2[2][i] = m1[2][i] + m1[6][i];
        m2[3][i] = m1[3][i] + m1[7][i];
        m2[4][i] = m1[0][i] - m1[4][i];
        m2[5][i] = m1[1][i] - m1[5][i];
        m2[6][i] = m1[2][i] - m1[6][i];
        m2[7][i] = m1[3][i] - m1[7][i];

        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m1[4][i] = m2[4][i] + m2[6][i];
        m1[5][i] = m2[5][i] + m2[7][i];
        m1[6][i] = m2[4][i] - m2[6][i];
        m1[7][i] = m2[5][i] - m2[7][i];

        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }

    for (j = 0; j < 8; j++) {
        for (i = 0; i < 2; i++) {
            satd += (abs(m2[j][i]));
        }
    }

    satd = ((satd + 1) >> 1);

    return satd;
}

static i32u_t  xCalcHAD16x4(pel_t *org, int i_org, pel_t * rec, int i_rec)
{
    int satd = 0;
    int i, j, jj;
    int diff[257], m1[4][16], m2[4][16];

    // Hadamard8x8
    for (j = 0; j < 16; j++) {
        for (i = 0; i < 4; i++) {
            diff[j * 4 + i] = org[i] - rec[i];
        }
        org += i_org;
        rec += i_rec;
    }


    //horizontal
    for (j = 0; j < 4; j++) {
        jj = j << 4;

        m2[j][0] = diff[jj] + diff[jj + 8];
        m2[j][1] = diff[jj + 1] + diff[jj + 9];
        m2[j][2] = diff[jj + 2] + diff[jj + 10];
        m2[j][3] = diff[jj + 3] + diff[jj + 11];
        m2[j][4] = diff[jj + 4] + diff[jj + 12];
        m2[j][5] = diff[jj + 5] + diff[jj + 13];
        m2[j][6] = diff[jj + 6] + diff[jj + 14];
        m2[j][7] = diff[jj + 7] + diff[jj + 15];
        m2[j][8] = diff[jj] - diff[jj + 8];
        m2[j][9] = diff[jj + 1] - diff[jj + 9];
        m2[j][10] = diff[jj + 2] - diff[jj + 10];
        m2[j][11] = diff[jj + 3] - diff[jj + 11];
        m2[j][12] = diff[jj + 4] - diff[jj + 12];
        m2[j][13] = diff[jj + 5] - diff[jj + 13];
        m2[j][14] = diff[jj + 6] - diff[jj + 14];
        m2[j][15] = diff[jj + 7] - diff[jj + 15];

        m1[j][0] = m2[j][0] + m2[j][4];
        m1[j][1] = m2[j][1] + m2[j][5];
        m1[j][2] = m2[j][2] + m2[j][6];
        m1[j][3] = m2[j][3] + m2[j][7];
        m1[j][4] = m2[j][0] - m2[j][4];
        m1[j][5] = m2[j][1] - m2[j][5];
        m1[j][6] = m2[j][2] - m2[j][6];
        m1[j][7] = m2[j][3] - m2[j][7];
        m1[j][8] = m2[j][8] + m2[j][12];
        m1[j][9] = m2[j][9] + m2[j][13];
        m1[j][10] = m2[j][10] + m2[j][14];
        m1[j][11] = m2[j][11] + m2[j][15];
        m1[j][12] = m2[j][8] - m2[j][12];
        m1[j][13] = m2[j][9] - m2[j][13];
        m1[j][14] = m2[j][10] - m2[j][14];
        m1[j][15] = m2[j][11] - m2[j][15];

        m2[j][0] = m1[j][0] + m1[j][2];
        m2[j][1] = m1[j][1] + m1[j][3];
        m2[j][2] = m1[j][0] - m1[j][2];
        m2[j][3] = m1[j][1] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][6];
        m2[j][5] = m1[j][5] + m1[j][7];
        m2[j][6] = m1[j][4] - m1[j][6];
        m2[j][7] = m1[j][5] - m1[j][7];
        m2[j][8] = m1[j][8] + m1[j][10];
        m2[j][9] = m1[j][9] + m1[j][11];
        m2[j][10] = m1[j][8] - m1[j][10];
        m2[j][11] = m1[j][9] - m1[j][11];
        m2[j][12] = m1[j][12] + m1[j][14];
        m2[j][13] = m1[j][13] + m1[j][15];
        m2[j][14] = m1[j][12] - m1[j][14];
        m2[j][15] = m1[j][13] - m1[j][15];

        m1[j][0] = m2[j][0] + m2[j][1];
        m1[j][1] = m2[j][0] - m2[j][1];
        m1[j][2] = m2[j][2] + m2[j][3];
        m1[j][3] = m2[j][2] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][5];
        m1[j][5] = m2[j][4] - m2[j][5];
        m1[j][6] = m2[j][6] + m2[j][7];
        m1[j][7] = m2[j][6] - m2[j][7];
        m1[j][8] = m2[j][8] + m2[j][9];
        m1[j][9] = m2[j][8] - m2[j][9];
        m1[j][10] = m2[j][10] + m2[j][11];
        m1[j][11] = m2[j][10] - m2[j][11];
        m1[j][12] = m2[j][12] + m2[j][13];
        m1[j][13] = m2[j][12] - m2[j][13];
        m1[j][14] = m2[j][14] + m2[j][15];
        m1[j][15] = m2[j][14] - m2[j][15];
    }

    //vertical
    for (i = 0; i < 16; i++) {
        m2[0][i] = m1[0][i] + m1[2][i];
        m2[1][i] = m1[1][i] + m1[3][i];
        m2[2][i] = m1[0][i] - m1[2][i];
        m2[3][i] = m1[1][i] - m1[3][i];

        m1[0][i] = m2[0][i] + m2[1][i];
        m1[1][i] = m2[0][i] - m2[1][i];
        m1[2][i] = m2[2][i] + m2[3][i];
        m1[3][i] = m2[2][i] - m2[3][i];
    }


    for (j = 0; j < 4; j++) {
        for (i = 0; i < 16; i++) {
            satd += (abs(m1[j][i]));
        }
    }

    satd = ((satd + 2) >> 2);

    return satd;
}

static i32u_t  xCalcHAD4x16(pel_t *org, int i_org, pel_t * rec, int i_rec)
{
    int satd = 0;
    int i, j, jj;
    int diff[257], m1[16][4], m2[16][4], m3[16][4];

    // Hadamard8x8
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 16; i++) {
            diff[j * 16 + i] = org[i] - rec[i];
        }
        org += i_org;
        rec += i_rec;
    }

    //horizontal
    for (j = 0; j < 16; j++) {
        jj = j << 2;
        m2[j][0] = diff[jj] + diff[jj + 2];
        m2[j][1] = diff[jj + 1] + diff[jj + 3];
        m2[j][2] = diff[jj] - diff[jj + 2];
        m2[j][3] = diff[jj + 1] - diff[jj + 3];

        m1[j][0] = m2[j][0] + m2[j][1];
        m1[j][1] = m2[j][0] - m2[j][1];
        m1[j][2] = m2[j][2] + m2[j][3];
        m1[j][3] = m2[j][2] - m2[j][3];
    }

    //vertical
    for (i = 0; i < 4; i++) {
        m2[0][i] = m1[0][i] + m1[8][i];
        m2[1][i] = m1[1][i] + m1[9][i];
        m2[2][i] = m1[2][i] + m1[10][i];
        m2[3][i] = m1[3][i] + m1[11][i];
        m2[4][i] = m1[4][i] + m1[12][i];
        m2[5][i] = m1[5][i] + m1[13][i];
        m2[6][i] = m1[6][i] + m1[14][i];
        m2[7][i] = m1[7][i] + m1[15][i];
        m2[8][i] = m1[0][i] - m1[8][i];
        m2[9][i] = m1[1][i] - m1[9][i];
        m2[10][i] = m1[2][i] - m1[10][i];
        m2[11][i] = m1[3][i] - m1[11][i];
        m2[12][i] = m1[4][i] - m1[12][i];
        m2[13][i] = m1[5][i] - m1[13][i];
        m2[14][i] = m1[6][i] - m1[14][i];
        m2[15][i] = m1[7][i] - m1[15][i];

        m3[0][i] = m2[0][i] + m2[4][i];
        m3[1][i] = m2[1][i] + m2[5][i];
        m3[2][i] = m2[2][i] + m2[6][i];
        m3[3][i] = m2[3][i] + m2[7][i];
        m3[4][i] = m2[0][i] - m2[4][i];
        m3[5][i] = m2[1][i] - m2[5][i];
        m3[6][i] = m2[2][i] - m2[6][i];
        m3[7][i] = m2[3][i] - m2[7][i];
        m3[8][i] = m2[8][i] + m2[12][i];
        m3[9][i] = m2[9][i] + m2[13][i];
        m3[10][i] = m2[10][i] + m2[14][i];
        m3[11][i] = m2[11][i] + m2[15][i];
        m3[12][i] = m2[8][i] - m2[12][i];
        m3[13][i] = m2[9][i] - m2[13][i];
        m3[14][i] = m2[10][i] - m2[14][i];
        m3[15][i] = m2[11][i] - m2[15][i];

        m1[0][i] = m3[0][i] + m3[2][i];
        m1[1][i] = m3[1][i] + m3[3][i];
        m1[2][i] = m3[0][i] - m3[2][i];
        m1[3][i] = m3[1][i] - m3[3][i];
        m1[4][i] = m3[4][i] + m3[6][i];
        m1[5][i] = m3[5][i] + m3[7][i];
        m1[6][i] = m3[4][i] - m3[6][i];
        m1[7][i] = m3[5][i] - m3[7][i];
        m1[8][i] = m3[8][i] + m3[10][i];
        m1[9][i] = m3[9][i] + m3[11][i];
        m1[10][i] = m3[8][i] - m3[10][i];
        m1[11][i] = m3[9][i] - m3[11][i];
        m1[12][i] = m3[12][i] + m3[14][i];
        m1[13][i] = m3[13][i] + m3[15][i];
        m1[14][i] = m3[12][i] - m3[14][i];
        m1[15][i] = m3[13][i] - m3[15][i];

        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
        m2[8][i] = m1[8][i] + m1[9][i];
        m2[9][i] = m1[8][i] - m1[9][i];
        m2[10][i] = m1[10][i] + m1[11][i];
        m2[11][i] = m1[10][i] - m1[11][i];
        m2[12][i] = m1[12][i] + m1[13][i];
        m2[13][i] = m1[12][i] - m1[13][i];
        m2[14][i] = m1[14][i] + m1[15][i];
        m2[15][i] = m1[14][i] - m1[15][i];
    }


    for (j = 0; j < 16; j++) {
        for (i = 0; i < 4; i++) {
            satd += (abs(m2[j][i]));
        }
    }

    satd = ((satd + 2) >> 2);

    return satd;
}

#if defined(_MSC_VER) 
#pragma warning(disable: 4100)  // unreferenced formal parameter
#endif

i32u_t find_blk_satd16(pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    int satd = 0;
    satd += g_funs_handle.cost_blk_satd[1](org    , i_org, pred    , i_pred);
    satd += g_funs_handle.cost_blk_satd[1](org + 8, i_org, pred + 8, i_pred);

    org  += i_org  << 3;
    pred += i_pred << 3;

    satd += g_funs_handle.cost_blk_satd[1](org    , i_org, pred    , i_pred);
    satd += g_funs_handle.cost_blk_satd[1](org + 8, i_org, pred + 8, i_pred);
    return satd;
}


i32u_t find_blk_satd32(pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    int satd = 0;
    int height = 4;

    while (height--) {
        satd += g_funs_handle.cost_blk_satd[1](org     , i_org, pred     , i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org +  8, i_org, pred +  8, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 16, i_org, pred + 16, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 24, i_org, pred + 24, i_pred);
        org  += i_org  << 3;
        pred += i_pred << 3;
    }

    return satd;
}

i32u_t find_blk_satd64(pel_t *org, int i_org, pel_t *pred, int i_pred)
{
    int satd = 0;
    int height = 8;

    while (height--) {
        satd += g_funs_handle.cost_blk_satd[1](org, i_org, pred, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 8, i_org, pred + 8, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 16, i_org, pred + 16, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 24, i_org, pred + 24, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 32, i_org, pred + 32, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 40, i_org, pred + 40, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 48, i_org, pred + 48, i_pred);
        satd += g_funs_handle.cost_blk_satd[1](org + 56, i_org, pred + 56, i_pred);
        org  += i_org  << 3;
        pred += i_pred << 3;
    }

    return satd;
}

#define XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)

static void ssim_4x4x2_core(const pel_t *pix1, int stride1, const pel_t *pix2, int stride2, int sums[2][4])
{
    int x, y, z;
    for (z = 0; z < 2; z++) {
        int s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (y = 0; y < 4; y++) {
            for (x = 0; x < 4; x++) {
                int a = pix1[x + y*stride1];
                int b = pix2[x + y*stride2];
                s1 += a;
                s2 += b;
                ss += a*a;
                ss += b*b;
                s12 += a*b;
            }
        }
        sums[z][0] = s1;
        sums[z][1] = s2;
        sums[z][2] = ss;
        sums[z][3] = s12;
        pix1 += 4;
        pix2 += 4;
    }
}

static float ssim_end1( int s1, int s2, int ss, int s12 )
{
/* Maximum value for 10-bit is: ss*64 = (2^10-1)^2*16*4*64 = 4286582784, which will overflow in some cases.
 * s1*s1, s2*s2, and s1*s2 also obtain this value for edge cases: ((2^10-1)*16*4)^2 = 4286582784.
 * Maximum value for 9-bit is: ss*64 = (2^9-1)^2*16*4*64 = 1069551616, which will not overflow. */
#if COMPILE_10BIT
#define PIXEL_MAX 1023
#else
#define PIXEL_MAX 255
#endif

    static const float ssim_c1 = (float)(.01 * .01 * PIXEL_MAX * PIXEL_MAX * 64);
    static const float ssim_c2 = (float)(.03 * .03 * PIXEL_MAX * PIXEL_MAX * 64 * 63);

    float fs1 = (float)s1;
    float fs2 = (float)s2;
    float fss = (float)ss;
    float fs12 = (float)s12;
    float vars = (float)(fss * 64 - fs1 * fs1 - fs2 * fs2);
    float covar = (float)(fs12 * 64 - fs1 * fs2);
    return (float)(2 * fs1 * fs2 + ssim_c1) * (float)(2 * covar + ssim_c2) / ((float)(fs1 * fs1 + fs2 * fs2 + ssim_c1) * (float)(vars + ssim_c2));

}

static float ssim_end4(int sum0[5][4], int sum1[5][4], int width)
{
    float ssim = 0.0;
    int i;
    for (i = 0; i < width; i++)
        ssim += ssim_end1(sum0[i][0] + sum0[i + 1][0] + sum1[i][0] + sum1[i + 1][0],
                          sum0[i][1] + sum0[i + 1][1] + sum1[i][1] + sum1[i + 1][1],
                          sum0[i][2] + sum0[i + 1][2] + sum1[i][2] + sum1[i + 1][2],
                          sum0[i][3] + sum0[i + 1][3] + sum1[i][3] + sum1[i + 1][3]);
    return ssim;
}

float avs3_pixel_ssim_wxh(pel_t *pix1, int stride1, pel_t *pix2, int stride2, int width, int height, int *cnt)
{
#define MAX_PIC_WIDTH 8192

    static int buf[2 * (MAX_PIC_WIDTH / 4 + 3)][4];
    int x, y, z = 0;
    float ssim = 0.0;
    int(*sum0)[4] = buf;
    int(*sum1)[4] = sum0 + (width >> 2) + 3;
    
    width >>= 2;
    height >>= 2;

    for (y = 1; y < height; y++) {
        for (; z <= y; z++) {
            XCHG(void*, sum0, sum1);
            for (x = 0; x < width; x += 2)
                g_funs_handle.ssim_4x4x2_core(&pix1[4 * (x + z*stride1)], stride1, &pix2[4 * (x + z*stride2)], stride2, &sum0[x]);
        }
        for (x = 0; x < width - 1; x += 4)
            ssim += g_funs_handle.ssim_end4(sum0 + x, sum1 + x, min(4, width - x - 1));
    }
    *cnt = (height - 1) * (width - 1);
    return ssim;
}

void com_funs_init_rdcost()
{
    g_funs_handle.cost_ssd[0] = xGetSSE4;
    g_funs_handle.cost_ssd[1] = xGetSSE8;
    g_funs_handle.cost_ssd[2] = xGetSSE16;
    g_funs_handle.cost_ssd[3] = xGetSSE32;
    g_funs_handle.cost_ssd[4] = xGetSSE64;
    g_funs_handle.cost_ssd_ext = xGetSSE_Ext;
    g_funs_handle.cost_ssd_psnr = xGetSSE_Psnr;
    g_funs_handle.ssim_4x4x2_core = ssim_4x4x2_core;
    g_funs_handle.ssim_end4 = ssim_end4;

    g_funs_handle.cost_sad[ 1] = xGetSAD4;
    g_funs_handle.cost_sad[ 2] = xGetSAD8;
    g_funs_handle.cost_sad[ 3] = xGetSAD12;
    g_funs_handle.cost_sad[ 4] = xGetSAD16;
    g_funs_handle.cost_sad[ 6] = xGetSAD24;
    g_funs_handle.cost_sad[ 8] = xGetSAD32;
    g_funs_handle.cost_sad[12] = xGetSAD48;
    g_funs_handle.cost_sad[16] = xGetSAD64;

    g_funs_handle.cost_avg_sad[ 1] = xGetAVGSAD4;
    g_funs_handle.cost_avg_sad[ 2] = xGetAVGSAD8;
    g_funs_handle.cost_avg_sad[ 4] = xGetAVGSAD16;
    g_funs_handle.cost_avg_sad[ 8] = xGetAVGSAD32;
    g_funs_handle.cost_avg_sad[16] = xGetAVGSAD64;
    
    g_funs_handle.cost_sad_x4[ 1] = xGetSAD4_x4;
    g_funs_handle.cost_sad_x4[ 2] = xGetSAD8_x4;
    g_funs_handle.cost_sad_x4[ 3] = xGetSAD12_x4;
    g_funs_handle.cost_sad_x4[ 4] = xGetSAD16_x4;
    g_funs_handle.cost_sad_x4[ 6] = xGetSAD24_x4;
    g_funs_handle.cost_sad_x4[ 8] = xGetSAD32_x4;
    g_funs_handle.cost_sad_x4[12] = xGetSAD48_x4;
    g_funs_handle.cost_sad_x4[16] = xGetSAD64_x4;

    g_funs_handle.cost_blk_satd[0] = xCalcHAD4x4;
    g_funs_handle.cost_blk_satd[1] = xCalcHAD8x8;
    g_funs_handle.cost_blk_satd[2] = find_blk_satd16;
    g_funs_handle.cost_blk_satd[3] = find_blk_satd32;
    g_funs_handle.cost_blk_satd[4] = find_blk_satd64;

    g_funs_handle.cost_satd_i = xCalcHADs8x8_I;
}
