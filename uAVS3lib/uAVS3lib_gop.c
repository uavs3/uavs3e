#include <math.h>
#include "uAVS3lib_gop.h"
#include "global.h"
#include "commonVariables.h"
#include "AEC.h"
#include "image.h"
#include "header.h"
#include "vlc.h"
#include "wquant.h"
#include "tools/uratectrl.h"
#include "tools/preprocess.h"

typedef struct avs3gop avs3gop_t;

typedef struct avs3_gop_frm_t {
    i64s_t pts;
    i64s_t dts;
    int  frm_type;
    int  frm_size;
} avs3_gop_frm_t;

typedef struct avs3gop_enc_t {
    void *enc;
    char *strm_buf;
    int buf_size;
    int cur_size;
    avs3gop_t *top;
} avs3gop_enc_t;

struct avs3gop {
    int    objs;
    avs3gop_enc_t *obj_list;
    strm_out_t strm_out;
    rec_out_t  rec_out;
    void *priv_data;
    int active_objs;
    int obj_idx;
    cfg_param_t input;
    int segment_size;
    i64s_t total_frms;
    int frms_in_seg;
};

void avs3gop_rec_callback(void *priv_data, image_t *rec, image_t *org, image_t *trace, int frm_type, long long idx, int bits, double qp, int time, signed char* ext_info)
{
    avs3gop_enc_t *h = (avs3gop_enc_t*)priv_data;
    avs3gop_t *g = h->top;

    if (g->rec_out) {
        g->rec_out(g->priv_data, rec, org, trace, frm_type, idx, bits, qp, time, ext_info);
    }
}

void avs3gop_strm_callback(void* priv_data, unsigned char *buf, int len, long long pts, long long dts, int type)
{
    avs3gop_enc_t *h = (avs3gop_enc_t*)priv_data;
    avs3_gop_frm_t frm;
    frm.pts = pts;
    frm.dts = dts;
    frm.frm_type = type;
    frm.frm_size = len;

    if (h->cur_size + len + sizeof(frm) > h->buf_size) {
        char *tmp_buf;
        h->buf_size = h->cur_size + len + sizeof(frm) + 1024 * 1024;
        tmp_buf = com_malloc(h->buf_size, 0);

        if (h->cur_size) {
            memcpy(tmp_buf, h->strm_buf, h->cur_size);
        }
        if (h->strm_buf) {
            com_free(h->strm_buf);
        }
        h->strm_buf = tmp_buf;
    }

    memcpy(h->strm_buf + h->cur_size, &frm, sizeof(frm)); h->cur_size += sizeof(frm);
    memcpy(h->strm_buf + h->cur_size, buf, len); h->cur_size += len;
}

void* avs3gop_lib_create(cfg_param_t *input, strm_out_t strm_callbak, rec_out_t rec_callback, void *priv_data, int objs, void **masks)
{
    int i;
    avs3gop_t *g = com_malloc(sizeof(avs3gop_t), 1);

    g->input        = *input;
    g->priv_data    = priv_data;
    g->strm_out     = strm_callbak;
    g->rec_out      = rec_callback;
    g->active_objs  = 0;
    g->obj_idx      = 0;
    g->segment_size = input->intra_period * 8;
    g->total_frms   = 0;
    g->frms_in_seg  = 0;
    g->objs         = objs;
    g->obj_list     = com_malloc(sizeof(avs3gop_enc_t) * objs, 1);

#if defined(__GNUC__)
    cpu_set_t mask_bak;
    CPU_ZERO(&mask_bak);
    pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask_bak);
#endif
    for (i = 0; i < objs; i++) {
        const int GOP_STRM_BUF = 10 * 1024 * 1024;
        avs3gop_enc_t *e = &g->obj_list[i];
        cfg_param_t tmp_input = *input;

#if defined(__GNUC__)
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), (cpu_set_t*)masks[i]);
#endif
        e->enc = avs3_lib_create(&tmp_input, avs3gop_strm_callback, avs3gop_rec_callback, input->intra_period * 8 + 3, &g->obj_list[i]);
        e->buf_size = GOP_STRM_BUF;
        e->cur_size = 0;
        e->strm_buf = com_malloc(GOP_STRM_BUF, 0);
        e->top = g;
    }
#if defined(__GNUC__)
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask_bak);
#endif
    return g;
}

void finish_one_seg(avs3gop_t *g, avs3gop_enc_t *obj)
{
    avs3_lib_reset(obj->enc, &g->input, obj);
    char *start = obj->strm_buf;

    while (start < obj->strm_buf + obj->cur_size) {
        avs3_gop_frm_t *frm = (avs3_gop_frm_t *)start;
        start += sizeof(avs3_gop_frm_t);
        g->strm_out(g->priv_data, start, frm->frm_size, frm->pts, frm->dts, frm->frm_type);
        start += frm->frm_size;
    }
    obj->cur_size = 0;
}

void avs3gop_lib_encode(void *handle, int bflush, int output_seq_end)
{
    avs3gop_t *g = (avs3gop_t *)handle;
    avs3gop_enc_t *obj = g->obj_list + g->obj_idx;

    if (!bflush) {
        if (g->frms_in_seg == 0) {
            if (g->active_objs == g->objs) {
                finish_one_seg(g, obj);
            }
            g->active_objs = COM_MIN(g->active_objs + 1, g->objs);
        }
        avs3_lib_encode(obj->enc, 0, 0);
        g->frms_in_seg++;
        g->total_frms++;

        if (g->frms_in_seg == g->segment_size) {
            avs3_lib_encode(obj->enc, 1, 1);
            g->obj_idx = (g->obj_idx + 1) % g->objs;
            g->frms_in_seg = 0;
        }
    } else {
        if (g->frms_in_seg) {
            avs3_lib_encode(obj->enc, 1, 1);
            g->obj_idx = (g->obj_idx + 1) % g->objs;
        }
        if (g->active_objs == g->objs) {
            while (g->active_objs--) {
                finish_one_seg(g, g->obj_list + g->obj_idx);
                g->obj_idx = (g->obj_idx + 1) % g->objs;
            }
        } else {
            int i;
            for (i = 0; i < g->obj_idx; i++) {
                finish_one_seg(g, g->obj_list + i);
            }
        }
    }
}

void avs3gop_lib_free(void *handle)
{
    int i;
    avs3gop_t *g = (avs3gop_t *)handle;
 
    for (i = 0; i < g->objs; i++) {
        avs3_lib_encode(g->obj_list[i].enc, 2, 1);
        avs3_lib_free(g->obj_list[i].enc);
        com_free(g->obj_list[i].strm_buf);
    }
    com_free(g->obj_list);
}

image_t *avs3gop_lib_imgbuf(void *handle)
{
    avs3gop_t *g = (avs3gop_t *)handle;
    return avs3_lib_imgbuf(g->obj_list[g->obj_idx].enc);
}

void avs3gop_lib_speed_adjust(void *handle, double adj)
{
    int i;
    avs3gop_t *g = (avs3gop_t *)handle;

    for (i = 0; i < g->objs; i++) {
        avs3_lib_speed_adjust(g->obj_list[i].enc, adj);
    }
}
