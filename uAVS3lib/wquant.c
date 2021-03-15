#include "wquant.h"

tab_i16s_t tab_WqMDefault4x4[16] = {
    64, 64, 64, 68,
    64, 64, 68, 72,
    64, 68, 76, 80,
    72, 76, 84, 96
};

tab_i16s_t tab_WqMDefault8x8[64] = {
     64,  64,  64,  64,  68,  68,  72,  76,
     64,  64,  64,  68,  72,  76,  84,  92,
     64,  64,  68,  72,  76,  80,  88, 100,
     64,  68,  72,  80,  84,  92, 100, 112,
     68,  72,  80,  84,  92, 104, 112, 128,
     76,  80,  84,  92, 104, 116, 132, 152,
     96, 100, 104, 116, 124, 140, 164, 188,
    104, 108, 116, 128, 152, 172, 192, 216
};

void cal_all_wq_matrix(i16s_t *matrix[3][4], i16s_t *m4, i16s_t *m8)
{
    int i, j;

    memcpy(matrix[0][0], m4, 16 * sizeof(i16s_t));
    memcpy(matrix[0][1], m8, 64 * sizeof(i16s_t));

    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            matrix[0][2][(i << 4) + j] = m8[((i >> 1) & 7) * 8 + ((j >> 1) & 7)];
        }
    }

    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            matrix[0][3][(i << 5) + j] = m8[((i >> 2) & 7) * 8 + ((j >> 2) & 7)];
        }
    }

    memcpy(matrix[1][1], matrix[0][1],  16 * sizeof(i16s_t));
    memcpy(matrix[1][2], matrix[0][2],  64 * sizeof(i16s_t));
    memcpy(matrix[1][3], matrix[0][3], 256 * sizeof(i16s_t));

    for (i = 0; i < 8; i++) {
        memcpy(matrix[2][1] + i * 2, matrix[0][1] + i * 8, 2 * sizeof(i16s_t));
    }
    for (i = 0; i < 16; i++) {
        memcpy(matrix[2][2] + i * 4, matrix[0][2] + i * 16, 4 * sizeof(i16s_t));
    }
    for (i = 0; i < 32; i++) {
        memcpy(matrix[2][3] + i * 8, matrix[0][3] + i * 32, 8 * sizeof(i16s_t));
    }
}