#include "commonVariables.h"

static void com_mem_cpy(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height)
{
    while (height--) {
        memcpy(dst, src, sizeof(pel_t)* width);
        src += i_src;
        dst += i_dst;
    }
}

#if defined(_MSC_VER)  
#pragma warning(disable: 4100)
#endif

static void padding_rows(pel_t *src, int i_src, int width, int height, int start, int rows, int pad)
{
    int i, j;
    pel_t *p;

    start = max(start, 0);
    rows = min(rows, height - start);

    if (start + rows == height) {
        rows += pad;
        p = src + i_src * (height - 1);
        for (i = 1; i <= pad; i++) {
            memcpy(p + i_src * i, p, width * sizeof(pel_t));
        }
    }

    if (start == 0) {
        start = -pad;
        rows += pad;
        p = src;
        for (i = 1; i <= pad; i++) {
            memcpy(p - i_src * i, p, width * sizeof(pel_t));
        }
    }

    p = src + start * i_src;

    // left & right
    for (i = 0; i < rows; i++) {
        for (j = 0; j < pad; j++) {
            p[-pad + j] = p[0];
            p[width + j] = p[width - 1];
        }
        p += i_src;
    }
}


void avg_pel(pel_t *dst, int i_dst, pel_t *src1, int i_src1, pel_t *src2, int i_src2, int width, int height)
{
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            dst[j] = (src1[j] + src2[j] + 1) >> 1;
        }
        dst += i_dst;
        src1 += i_src1;
        src2 += i_src2;
    }
}

void avg_pel_1d(pel_t *dst, pel_t *src1, pel_t *src2, int len)
{
    int j;
    for (j = 0; j < len; j++) {
        dst[j] = (src1[j] + src2[j] + 1) >> 1;
    }
}

// block average only by downscaling vertically
double calc_blk_ave_vertds(pel_t* p_org, int width, int height, int iDownScale, int bit_depth)
{
    int i, j;
    int widhig = width*(height / iDownScale); // only downscale height

    int tmpSum = 0;
    for (j = 0; j < height; j += iDownScale){
        for (i = 0; i < width; i += 1){
            tmpSum += p_org[j*CACHE_STRIDE + i];
        }
    }

    return ((double)tmpSum) / widhig / (1 << (bit_depth - 8));
}

// block variance only by downscaling vertically
double calc_blk_var_vertds(pel_t* p_org, int width, int height, int iDownScale, double ave, int bit_depth)
{
    int i, j;
    int widhig = width*(height / iDownScale); // only downscale height
    double tmpSum = 0;
    ave *= 1 << (bit_depth - 8);

    for (j = 0; j < height; j += iDownScale){
        for (i = 0; i < width; i += 1){
            tmpSum += p_org[j*CACHE_STRIDE + i] * p_org[j*CACHE_STRIDE + i];
        }
    }

    tmpSum = tmpSum / widhig - ave*ave;

    return tmpSum / (1 << (bit_depth - 8)) / (1 << (bit_depth - 8));
}

void com_funs_init_pixel_opt()
{
    int i;

    g_funs_handle.padding_rows = padding_rows;

    for (i = 0; i < 33; i++) {
        g_funs_handle.avg_pel[i] = avg_pel;
        g_funs_handle.cpy_pel[i] = com_mem_cpy;
    }

    g_funs_handle.avg_pel_1d = avg_pel_1d;

    g_funs_handle.calc_blk_ave = calc_blk_ave_vertds;
    g_funs_handle.calc_blk_var = calc_blk_var_vertds;
}