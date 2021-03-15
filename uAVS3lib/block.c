#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include "defines.h"

#include "intra-prediction.h"
#include "global.h"

#include "vlc.h"
#include "block.h"
#include "codingUnit.h"
#include "rdoq.h"

tab_i32s_t tab_max_coef_factor[2][5] = {
    { 3968, 1920, 435, 85, 14 }, // square
    {    0,  845, 244, 56,  0 }  // non-square
};

// quantize coefficients to zero compulsively, except for left-top 1/4 area
int quant_ext(coef_t *curr_blk, int size, int Q, int qp_const, int shift)
{
    int i, j;
    int nz = 0; 
    int half_size = size / 2;

    for (i = 0; i < half_size; i++) {
        for (j = 0; j < half_size; j++) {
            coef_t val = curr_blk[j];
            int temp = (absm(val) * Q + qp_const) >> shift;
            curr_blk[j] = (coef_t)(val >= 0 ? temp : -temp);
            nz += temp;
        }
        memset(curr_blk + half_size, 0, sizeof(coef_t)* half_size);
        curr_blk += size;
    }
    memset(curr_blk, 0, sizeof(coef_t)*size * half_size);

    return nz;
}

int quant_normal(coef_t *curr_blk, int coef_num, int Q, int qp_const, int shift)
{
    int i;
    int nz = 0;

    for (i = 0; i < coef_num; i++) {
        coef_t val = curr_blk[i];
        int temp = (absm(val) * Q + qp_const) >> shift;
        curr_blk[i] = (coef_t)(val >= 0 ? temp : -temp);
        nz += temp;
    }
    return nz;
}

int pre_quant(coef_t *curr_blk, i16u_t *abs_blk, int len, int Q_threshold)
{
    int val;
    int i;
    int nz = 0;

    for (i = 0; i < len; i++) {
        val = absm(curr_blk[i]);
        abs_blk[i] = (val - 1 > Q_threshold ? val : 0);
        nz |= abs_blk[i];
    }
    return nz;
}

void inv_quant_normal(coef_t *src, coef_t *dst, int coef_num, int QPI, int shift)
{
    int i, j;
    int add = 1 << (shift - 1);
    coef_t *p = src;

    memset(dst, 0, sizeof(coef_t)* coef_num);

    for (i = 0; i < coef_num; i += 4, p += 4) {
        if (p[0] | p[1] | p[2] | p[3]) {
            for (j = 0; j < 4; j++) {
                if (p[j]) {
                    int temp = (p[j] * QPI + add) >> shift;
                    dst[i + j] = (coef_t)Clip3(-32768, 32767, temp);
                }
            }
        }
    }
}

void inv_quant_normal_ext(coef_t *src, coef_t *dst, int size, int QPI, int shift)
{
    int i, j;
    int add = 1 << (shift - 1);
    int half_size = size >> 1;

    memset(dst, 0, sizeof(coef_t)* size * size);

    for (i = 0; i < half_size; i++) {
        for (j = 0; j < half_size; j++) {
            if (src[j]) {
                int temp = (src[j] * QPI + add) >> shift;
                dst[j] = (coef_t)Clip3(-32768, 32767, temp);
            }
        }
        src += size;
        dst += size;
    }
}

int wq_quant_ext(coef_t *curr_blk, i16s_t *wq_blk, int width, int height, int Q, int qp_const, int shift)
{
    int i, j;
    int nz = 0;
    int half_w = width / 2;
    int half_h = height / 2;

    qp_const >>= 6;
    shift -= 6;

    for (i = 0; i < half_h; i++) {
        for (j = 0; j < half_w; j++) {
            coef_t val = curr_blk[j];
            int temp = (absm(val) * Q / wq_blk[j] + qp_const) >> shift;
            curr_blk[j] = (coef_t)(val >= 0 ? temp : -temp);
            nz += (temp) ? 1 : 0;
        }
        memset(curr_blk + half_w, 0, sizeof(coef_t)* half_w);
        curr_blk += width;
        wq_blk   += width;
    }
    memset(curr_blk, 0, sizeof(coef_t)*width * half_h);

    return nz;
}

int wq_quant_normal(coef_t *curr_blk, i16s_t *wq_blk, int coef_num, int Q, int qp_const, int shift)
{
    int i;
    int nz = 0;

    qp_const >>= 6;
    shift -= 6;

    for (i = 0; i < coef_num; i++) {
        coef_t val = curr_blk[i];
        int temp = (absm(val) * Q / wq_blk[i] + qp_const) >> shift;
        curr_blk[i] = (coef_t)(val >= 0 ? temp : -temp);
        nz += (temp) ? 1 : 0;
    }
    return nz;
}

int wq_pre_quant(coef_t *curr_blk, i16u_t *abs_blk, i16s_t *wq_blk, int len, int Q_threshold)
{
    int val;
    int i;
    int nz = 0;

    for (i = 0; i < len; i++) {
        val = absm(curr_blk[i]);
        abs_blk[i] = (val - 1 > ((Q_threshold * wq_blk[i] + 32) >> 6) ? val : 0);
        nz |= abs_blk[i];
    }
    return nz;
}

void wq_inv_quant_normal(coef_t *src, coef_t *dst, i16s_t *wq_blk, int coef_num, int QPI, int shift)
{
    int i, j;
    int add = 1 << (shift - 1);
    coef_t *p = src;

    memset(dst, 0, sizeof(coef_t)* coef_num);

    for (i = 0; i < coef_num; i += 4, p += 4, wq_blk += 4) {
        if (p[0] | p[1] | p[2] | p[3]) {
            for (j = 0; j < 4; j++) {
                if (p[j]) {
                    int temp;
                    temp = (p[j] * wq_blk[j]) >> 2;
                    temp = (temp * QPI) >> 4;
                    temp = (temp + add) >> shift;
                    dst[i + j] = (coef_t)Clip3(-32768, 32767, temp);
                }
            }
        }
    }
}

void wq_inv_quant_normal_ext(coef_t *src, coef_t *dst, i16s_t *wq_blk, int width, int height, int QPI, int shift)
{
    int i, j;
    int add = 1 << (shift - 1);
    int half_w = width >> 1;
    int half_h = height >> 1;

    memset(dst, 0, sizeof(coef_t)* width * height);

    for (i = 0; i < half_h; i++) {
        for (j = 0; j < half_w; j++) {
            if (src[j]) {
                int temp;
                temp = (src[j] * wq_blk[j]) >> 2;
                temp = (temp * QPI) >> 4;
                temp = (temp + add) >> shift;
                dst[j] = (coef_t)Clip3(-32768, 32767, temp);
            }
        }
        src += width;
        dst += width;
        wq_blk += width;
    }
}

static __inline int pre_check_quant(coef_t *curr_blk, i16u_t *abs_blk, int len, int threshold, int use_wq, i16s_t *wq_blk)
{
#if USE_WQ
    if (use_wq) 
        return g_funs_handle.wq_pre_quant(curr_blk, abs_blk, wq_blk, len, threshold);
    else 
#endif
        return g_funs_handle.pre_quant(curr_blk, abs_blk, len, threshold);
}

int quant_blk(avs3_enc_t *h, aec_t *aec, int qp, int mode, coef_t *curr_blk, int tu_bitsize, cu_t *cu, int plane_id, int intraPredMode)
{
    int nz;
    int iSize;
    const cfg_param_t *input = h->input;
    int shift = 15 + LIMIT_BIT - (input->bit_depth + 1) - tu_bitsize;
    i16s_t *wq_blk = NULL;

    if (plane_id) {
        iSize = 1 << tu_bitsize;
    } else{
        iSize = h->tu_q_size;
    } 

#if USE_WQ
    wq_blk = h->wquant_matrix[0][tab_log2[iSize] - 2];
#endif

    if (h->analyzer.use_rdoq) {
        nz = rdoq_quant_block(h, h->type, qp, h->analyzer.rdoq_lambda, mode > 3, curr_blk, tu_bitsize, tu_bitsize, plane_id, h->input->bit_depth);
    } else {
        int intra = (mode > 3) ? 1 : 0;
        int qp_const = intra ? (1 << shift) * 10 / 31 : (1 << shift) * 10 / 62;

#if USE_WQ
        if (input->use_wquant) {
            if (CHECK_FAST_TRQ(iSize, h->type)) {
                nz = g_funs_handle.wq_quant_ext(curr_blk, wq_blk, iSize, iSize, tab_q_tab[qp], qp_const, shift);
            } else {
                nz = g_funs_handle.wq_quant(curr_blk, wq_blk, iSize * iSize, tab_q_tab[qp], qp_const, shift);
            }
        } else 
#endif
        {
            if (CHECK_FAST_TRQ(iSize, h->type)) {
                nz = g_funs_handle.quant_ext(curr_blk, iSize, tab_q_tab[qp], qp_const, shift);
            } else {
                nz = g_funs_handle.quant(curr_blk, iSize * iSize, tab_q_tab[qp], qp_const, shift);
            }
        }
    }

    return nz;
}

void inv_quant_blk(avs3_enc_t *h, int qp, coef_t *src, int i_src, coef_t *dst, int i_dst, int uiBitSize,  int isChroma)
{
    int shift, QPI;
    int iSize;
    i16s_t *wq_blk = NULL;
    const cfg_param_t *input = h->input;
    shift = tab_iq_shift[qp] + (input->bit_depth + 1) + uiBitSize - LIMIT_BIT;
    QPI = tab_iq_tab[qp];

    if (isChroma) {
        iSize = 1 << uiBitSize;
    } else {
        iSize = h->tu_q_size;
    }

#if USE_WQ
    wq_blk = h->wquant_matrix[0][tab_log2[iSize] - 2];
#endif

#if USE_WQ
    if (input->use_wquant) {
        if (!h->analyzer.use_rdoq && (CHECK_FAST_TRQ(iSize, h->type))) {
            g_funs_handle.wq_inv_quant_ext(src, dst, wq_blk, iSize, iSize, QPI, shift);
        } else {
            g_funs_handle.wq_inv_quant(src, dst, wq_blk, iSize * iSize, QPI, shift);
        }
    } else 
#endif
    {
        if (!h->analyzer.use_rdoq && (CHECK_FAST_TRQ(iSize, h->type))) {
            g_funs_handle.inv_quant_ext(src, dst, iSize, QPI, shift);
        } else {
            g_funs_handle.inv_quant(src, dst, iSize * iSize, QPI, shift);
        }
    }
}

int add_sign(coef_t *dat, i16u_t *abs_val, int len) 
{
    int i, nz = 0;
    for (i = 0; i < len; i++) {
        dat[i] = (dat[i] > 0) ? abs_val[i] : -abs_val[i];
        nz |= dat[i];
    }
    return nz;
}

int get_cg_bits(coef_t *coef)
{
    int i;
    int bits = 0;

    for (i = 0; i < 16; i++) {
        int level = abs(coef[i]);
        bits += level + !!level;
    }

    return bits;
}


void zero_cg_check4(coef_t *coef, i64u_t *cg_mask)
{
    i64u_t *p = (i64u_t*)coef;
    *cg_mask = p[0] | p[1] | p[2] | p[3];
}

void zero_cg_check8(coef_t *coef, i64u_t *cg_mask)
{
    i64u_t *p = (i64u_t*)coef;
    cg_mask[0] = p[0] | p[2] | p[4] | p[6];
    cg_mask[1] = p[1] | p[3] | p[5] | p[7];
    p += 8;
    cg_mask[2] = p[0] | p[2] | p[4] | p[6];
    cg_mask[3] = p[1] | p[3] | p[5] | p[7];
}

void zero_cg_check16(coef_t *coef, i64u_t *cg_mask)
{
    i64u_t *p = (i64u_t*)coef;
    cg_mask[0] = p[0] | p[4] | p[8] | p[12];
    cg_mask[1] = p[1] | p[5] | p[9] | p[13];
    cg_mask[2] = p[2] | p[6] | p[10] | p[14];
    cg_mask[3] = p[3] | p[7] | p[11] | p[15];
    p += 16;
    cg_mask[4] = p[0] | p[4] | p[8] | p[12];
    cg_mask[5] = p[1] | p[5] | p[9] | p[13];
    cg_mask[6] = p[2] | p[6] | p[10] | p[14];
    cg_mask[7] = p[3] | p[7] | p[11] | p[15];
    p += 16;
    cg_mask[8] = p[0] | p[4] | p[8] | p[12];
    cg_mask[9] = p[1] | p[5] | p[9] | p[13];
    cg_mask[10] = p[2] | p[6] | p[10] | p[14];
    cg_mask[11] = p[3] | p[7] | p[11] | p[15];
    p += 16;
    cg_mask[12] = p[0] | p[4] | p[8] | p[12];
    cg_mask[13] = p[1] | p[5] | p[9] | p[13];
    cg_mask[14] = p[2] | p[6] | p[10] | p[14];
    cg_mask[15] = p[3] | p[7] | p[11] | p[15];
}

void zero_cg_check32(coef_t *coef, i64u_t *cg_mask)
{
    i64u_t *p = (i64u_t*)coef;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
    cg_mask[0] = p[0] | p[8] | p[16] | p[24];
    cg_mask[1] = p[1] | p[9] | p[17] | p[25];
    cg_mask[2] = p[2] | p[10] | p[18] | p[26];
    cg_mask[3] = p[3] | p[11] | p[19] | p[27];
    cg_mask[4] = p[4] | p[12] | p[20] | p[28];
    cg_mask[5] = p[5] | p[13] | p[21] | p[29];
    cg_mask[6] = p[6] | p[14] | p[22] | p[30];
    cg_mask[7] = p[7] | p[15] | p[23] | p[31];
    cg_mask += 8;
    p += 32;
}

int pre_quant_rdoq(i16s_t *coef, int num, int q_value, int q_bits, int err_scale, i32u_t* abs_coef, i32u_t* abs_level, long long *uncoded_err)
{
    int q_bits_add = 1 << (q_bits - 1);
    int last_nz = -1;
    int threshold = ((1 << (q_bits - 1)) - 1) / q_value + 1;

    for (int i = 0; i < num; i++) {
        int abs_value = abs(coef[i]);

        if (abs_value < threshold) {
            abs_level[i] = 0;
        } else {
            i32u_t level_double = abs_value * q_value;
            long long err = (level_double * (long long)err_scale) >> ERR_SCALE_PRECISION_BITS;

            abs_level[i] = (i32u_t)((level_double + q_bits_add) >> q_bits);
            abs_coef[i] = level_double;
            uncoded_err[i] = err * err;

            last_nz = i;
        }
    }

    return last_nz;
}

void com_funs_init_quant()
{
    g_funs_handle.quant         = quant_normal;
    g_funs_handle.quant_ext     = quant_ext;
    g_funs_handle.inv_quant     = inv_quant_normal;
    g_funs_handle.inv_quant_ext = inv_quant_normal_ext;
    g_funs_handle.pre_quant     = pre_quant;

    g_funs_handle.wq_quant         = wq_quant_normal;
    g_funs_handle.wq_quant_ext     = wq_quant_ext;
    g_funs_handle.wq_inv_quant     = wq_inv_quant_normal;
    g_funs_handle.wq_inv_quant_ext = wq_inv_quant_normal_ext;
    g_funs_handle.wq_pre_quant     = wq_pre_quant;

    g_funs_handle.add_sign = add_sign;

    g_funs_handle.get_cg_bits = get_cg_bits;

    g_funs_handle.zero_cg_check[0] = zero_cg_check4;
    g_funs_handle.zero_cg_check[1] = zero_cg_check8;
    g_funs_handle.zero_cg_check[2] = zero_cg_check16;
    g_funs_handle.zero_cg_check[3] = zero_cg_check32;

    g_funs_handle.pre_quant_rdoq = pre_quant_rdoq;
}
