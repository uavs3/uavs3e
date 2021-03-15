#include "common.h"
#include "commonVariables.h"

#include <sys/timeb.h>

funs_handle_t g_funs_handle;

tab_uchar_t tab_inter_pu_num[MAXMODE] = { 4, 1, 1 };

tab_uchar_t tab_inter_pu_2_blk_idx[MAXMODE][4] = {
    { 0, 1, 2, 3 },    // 0. SKIPDIRECT
    { 0, 0, 0, 0 },    // 1. P2NX2N
    { 0, 0, 0, 0 }     // 2. I16MB
};

/* PU pixel's info, based on 8x8 cu, for inter mode */
const int cst_log_level = 2;

static void cavs_log_default(int i_level, const char_t *psz_fmt, va_list arg)
{
    char_t *psz_prefix;

    switch (i_level) {
    case COM_LOG_ERROR:
        psz_prefix = "error";
        break;
    case COM_LOG_WARNING:
        psz_prefix = "warning";
        break;
    case COM_LOG_INFO:
        psz_prefix = "info";
        break;
    case COM_LOG_DEBUG:
        psz_prefix = "debug";
        break;
    default:
        psz_prefix = "unknown";
        break;
    }

    fprintf(stderr, "[%s]: ", psz_prefix);
    vfprintf(stderr, psz_fmt, arg);

    if (i_level == COM_LOG_ERROR) {
        assert(0);
    }
}

void com_log(int i_level, const char_t *psz_fmt, ...)
{
    if (i_level <= cst_log_level) {
        va_list arg;
        va_start(arg, psz_fmt);
        cavs_log_default(i_level, psz_fmt, arg);
        va_end(arg);
    }
}

void *com_malloc(int i_size, int inited)
{
    int mask = ALIGN_BASIC - 1;
    uchar_t *align_buf;
    uchar_t *buf = (uchar_t *)malloc(i_size + mask + sizeof(void **));

    if (buf) {
        align_buf = buf + mask + sizeof(void **);
        align_buf -= (intptr_t)align_buf & mask;
        *(((void **)align_buf) - 1) = buf;
        if (inited) {
            memset(align_buf, 0, i_size);
        }
        return align_buf;
    } else {
        com_log(COM_LOG_ERROR, "malloc of size %d failed\n", i_size);
        return NULL;
    }
    
}

void com_free(void *p)
{
    if (p) {
        free(*(((void **)p) - 1));
    } else {
        com_log(COM_LOG_WARNING, "free a NULL pointer\n");
    }
}

void init_cu_pos(avs3_enc_t *h, int mcu_idx, int bit_size)
{
    const cfg_param_t *input = h->input;
    int mcu_x = mcu_idx % input->pic_width_in_mcu;
    int mcu_y = mcu_idx / input->pic_width_in_mcu;
    h->cu_pix_x = mcu_x * 8;
    h->cu_pix_y = mcu_y * 8;
    h->cu_bitsize = bit_size;
    h->cu_available_up = h->cu_pix_y > h->lcu_pix_y ? 1 : h->slice_nr[h->lcu_y] == h->slice_nr[h->lcu_y - 1];
}

void init_pu_inter_pos(avs3_enc_t *h, int mode, int md_direct, int cu_bitsize, int blk)
{
    int size = (mode + 1) << (cu_bitsize - 1);
    
    h->pu_pix_x = h->cu_pix_x + (blk &  1) * size;
    h->pu_pix_y = h->cu_pix_y + (blk >> 1) * size;
    h->pu_size = size;
    
    if (!mode) {
        h->pu_size <<= 1;
    }

    h->pu_b8_x = h->pu_pix_x >> 3;
    h->pu_b8_y = h->pu_pix_y >> 3;
}

void init_pu_pos(avs3_enc_t *h, int mode, int md_direct, int bit_size, int blk)
{
    if (B_INTRA(mode)) {
        h->pu_pix_x = h->cu_pix_x;
        h->pu_pix_y = h->cu_pix_y;
        h->pu_size = 1 << bit_size;
    } else {
        int size = (mode + 1) << (bit_size - 1);

        h->pu_pix_x = h->cu_pix_x + (blk  & 1) * size;
        h->pu_pix_y = h->cu_pix_y + (blk >> 1) * size;
        h->pu_size = size;

        if (!mode) {
            h->pu_size <<= 1;
        }
    }
    h->pu_b8_x = h->pu_pix_x >> 3;
    h->pu_b8_y = h->pu_pix_y >> 3;
}

void init_tu_pos(avs3_enc_t *h, int mode, int cu_bitsize, int trans_size, int blk)
{
    int blk_x, blk_y;

    if (trans_size == 0) {
        h->tu_pix_x = h->cu_pix_x;
        h->tu_pix_y = h->cu_pix_y;
        h->tu_size = 1 << cu_bitsize;
    } else {
        blk_x = (blk % 2) << (cu_bitsize - 1);
        blk_y = (blk / 2) << (cu_bitsize - 1);
        h->tu_pix_x = h->cu_pix_x + blk_x;
        h->tu_pix_y = h->cu_pix_y + blk_y;
        h->tu_size = 1 << (cu_bitsize - 1);
    }

    if (h->tu_size == 64) {
        h->tu_q_size = h->tu_size / 2;
    } else {
        h->tu_q_size = h->tu_size;
    }
}

int image_create(image_t *p_img, int width, int height, int b_org, int planes)
{
    int num_pix_luma = 0;
    int pel_size = 0;
    int i_alloc_width, i_alloc_height;
    int i_offset_y, i_offset_uv;
    pel_t *p_buf;

    if (b_org) {
        i_alloc_width  = width;
        i_alloc_height = height;
        i_offset_y = i_offset_uv = 0;
    } else {
        i_alloc_width  = (IMG_PAD_SIZE + width  + IMG_PAD_SIZE + 15) >> 4 << 4;
        i_alloc_height = (IMG_PAD_SIZE + height + IMG_PAD_SIZE);
        i_offset_y = i_alloc_width * IMG_PAD_SIZE + IMG_PAD_SIZE;
        i_offset_uv = (i_alloc_width * IMG_PAD_SIZE / 4) + (IMG_PAD_SIZE / 2);
    }
    num_pix_luma = i_alloc_width * i_alloc_height;

    if (planes == 1) {
        pel_size = num_pix_luma;
    } else if (planes == 3) {
        pel_size = num_pix_luma * 3 / 2;
    } else {
        assert(0);
    }

    p_img->width = width;
    p_img->height = height;

    p_img->plane[3] = p_buf = com_malloc((pel_size + 32) * sizeof(pel_t), 1); // more 32 bytes for assembler

    p_img->plane[0] = p_buf + i_offset_y;
    p_img->i_stride[0] = i_alloc_width;

    if (planes == 3) {
        p_img->plane[1] = p_buf + num_pix_luma + i_offset_uv;
        p_img->plane[2] = p_buf + num_pix_luma + i_offset_uv + num_pix_luma / 4;
        p_img->i_stride[1] = p_img->i_stride[2] = i_alloc_width / 2;
    }

    p_img->i_stride[3] = pel_size;

    if (b_org) {
        int lcu_rows = (height + LCU_SIZE - 1) / LCU_SIZE;
        p_img->inter_cpp_list = com_malloc(sizeof(double) * lcu_rows, 0);
        p_img->intra_cpp_list = com_malloc(sizeof(double) * lcu_rows, 0);
    } else {
        p_img->inter_cpp_list = NULL;
        p_img->intra_cpp_list = NULL;
    }

    return (pel_size + 32) * sizeof(pel_t);
}

void image_free(image_t *p_img)
{
    if (p_img) {
        if (p_img->plane[3]) {
            com_free(p_img->plane[3]);
        }
        if (p_img->inter_cpp_list) {
            com_free(p_img->inter_cpp_list);
        }
        if (p_img->intra_cpp_list) {
            com_free(p_img->intra_cpp_list);
        }
        memset(p_img, 0, sizeof(image_t));
    }
}

void image_copy(image_t *p_dst, image_t *p_src)
{
    if (p_src->i_stride[0] == p_dst->i_stride[0]) { 
        memcpy(p_dst->plane[3], p_src->plane[3], p_dst->i_stride[3] * sizeof(pel_t));
    } else {
        com_log(COM_LOG_ERROR, "image_copy failed!\n");
    }
}

void image_copy_rows(image_t *p_dst, image_t *p_src, int start, int rows)
{
    if (p_src->i_stride[0] == p_dst->i_stride[0]) {
        int i_stride;
        
        i_stride = p_src->i_stride[0];
        memcpy(p_dst->plane[0] + start * i_stride, p_src->plane[0] + start * i_stride, rows * i_stride * sizeof(pel_t));

        i_stride = p_src->i_stride[1];
        rows >>= 1;
        start >>= 1;
        memcpy(p_dst->plane[1] + start * i_stride, p_src->plane[1] + start * i_stride, rows * i_stride * sizeof(pel_t));
        memcpy(p_dst->plane[2] + start * i_stride, p_src->plane[2] + start * i_stride, rows * i_stride * sizeof(pel_t));

    } else {
        com_log(COM_LOG_ERROR, "image_copy failed!\n");
    }
}

void image_copy_lcu_row_for_sao(avs3_enc_t *h, image_t *p_dst, image_t *p_src, int lcu_y)
{
    const cfg_param_t *input = h->input;
    int i_stride;
    int lcu_pix_y = lcu_y * LCU_SIZE;
    int b_first_row = (lcu_y == 0) ||  (!LOOPFLT_CROSS_SLICE && (lcu_y % input->lcu_rows_in_slice == 0));
    int b_last_row  = (lcu_y == h->input->pic_height_in_lcu - 1) || (!LOOPFLT_CROSS_SLICE && ((lcu_y + 1) % input->lcu_rows_in_slice == 0));
    int start = b_first_row ? lcu_pix_y : lcu_pix_y - 3;
    int end = b_last_row ? lcu_pix_y + LCU_SIZE : lcu_pix_y + LCU_SIZE - 3;
    int rows = end - start;

    i_stride = p_src->i_stride[0];
    memcpy(p_dst->plane[0] + start * i_stride, p_src->plane[0] + start * i_stride, rows * i_stride * sizeof(pel_t));

    i_stride = p_src->i_stride[1];
    lcu_pix_y >>= 1;
    start = b_first_row ? lcu_pix_y : lcu_pix_y - 3;
    end = b_last_row ? lcu_pix_y + LCU_SIZE / 2 : lcu_pix_y + LCU_SIZE / 2 - 3;
    rows = end - start;

    memcpy(p_dst->plane[1] + start * i_stride, p_src->plane[1] + start * i_stride, rows * i_stride * sizeof(pel_t));
    memcpy(p_dst->plane[2] + start * i_stride, p_src->plane[2] + start * i_stride, rows * i_stride * sizeof(pel_t));
}

void image_padding(image_t *img, int pad)
{
    int width   = img->width;
    int height  = img->height;
    int widthc  = width  >> 1;
    int heightc = height >> 1;
    int padc    = pad    >> 1;
    g_funs_handle.padding_rows(img->plane[0], img->i_stride[0], width, height, 0, height, pad);
    g_funs_handle.padding_rows(img->plane[1], img->i_stride[1], widthc, heightc, 0, heightc, padc);
    g_funs_handle.padding_rows(img->plane[2], img->i_stride[2], widthc, heightc, 0, heightc, padc);
}

void image_padding_lcu_row(avs3_enc_t *h, image_t *img, int lcu_y, int pad)
{
    int width = img->width;
    int height = img->height;
    int widthc = width >> 1;
    int heightc = height >> 1;
    int padc = pad >> 1;

    int lcu_pix_y = lcu_y * LCU_SIZE;
    int b_last_row = (lcu_y == h->input->pic_height_in_lcu - 1);

    int start = lcu_pix_y ? lcu_pix_y - 4 : 0;
    int end = b_last_row ? lcu_pix_y + LCU_SIZE : lcu_pix_y + LCU_SIZE - 4;
    int rows = end - start;

    g_funs_handle.padding_rows(img->plane[0], img->i_stride[0], width, height, start, rows, pad);

    lcu_pix_y >>= 1;
    start = lcu_pix_y ? lcu_pix_y - 4 : 0;
    end = b_last_row ? lcu_pix_y + LCU_SIZE / 2 : lcu_pix_y + LCU_SIZE / 2 - 4;
    rows = end - start;

    g_funs_handle.padding_rows(img->plane[1], img->i_stride[1], widthc, heightc, start, rows, padc);
    g_funs_handle.padding_rows(img->plane[2], img->i_stride[2], widthc, heightc, start, rows, padc);
}

void com_update_cand_list(int uiMode, double uiCost, int uiFullCandNum, int *CandModeList, double *CandCostList)
{
    int idx = uiFullCandNum;

    while (idx && uiCost < CandCostList[idx - 1]) {
        CandModeList[idx] = CandModeList[idx - 1];
        CandCostList[idx] = CandCostList[idx - 1];
        idx--;
    }
    CandModeList[idx] = uiMode;
    CandCostList[idx] = uiCost;
}