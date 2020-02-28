/**************************************************************************************
 * Copyright (C) 2018-2019 uavs3e project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Open-Intelligence Open Source License V1.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Open-Intelligence Open Source License V1.1 for more details.
 *
 * You should have received a copy of the Open-Intelligence Open Source License V1.1
 * along with this program; if not, you can download it on:
 * http://www.aitisa.org.cn/uploadfile/2018/0910/20180910031548314.pdf
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>

#ifdef _WIN32
#include <IO.H>
#endif
#include "uavs3e.h"
#include "config.h"


static int  frame_to_be_encoded;

#define S_IREAD        0000400         /* read  permission, owner */
#define S_IWRITE       0000200         /* write permission, owner */

#ifndef _WIN32
#if defined(__APPLE__)
#define _lseeki64 lseek
#else
#define _lseeki64 lseek64
#endif
#define _read read
#define _write write
#define _open open
#define _close close

#define O_BINARY 0x0
#endif

/* printf function ************************************************************/

void print_log(int i_level, const char *psz_fmt, ...)
{
    if (i_level <= g_loglevel) {
        va_list arg;
        va_start(arg, psz_fmt);
        vfprintf(stderr, psz_fmt, arg);
        va_end(arg);
    }
}

/****************************************************************************/
#if defined(WIN32) || defined(WIN64)
#include <windows.h>

typedef DWORD time_clk_t;
#define CLK_PER_SEC     (1000)
#define CLK_PER_MSEC    (1)
#define app_get_time()  clock()

#elif defined(__GNUC__) || defined(ANDROID)
#include <time.h>
#include <sys/time.h>
typedef unsigned long time_clk_t;
#define CLK_PER_SEC     (10000)
#define CLK_PER_MSEC    (10)
static time_clk_t app_get_time(void)
{
    time_clk_t t;
    t = (time_clk_t)(clock()) * 10000L / CLK_PER_SEC;
    return t;
}
#else
#error THIS PLATFORM CANNOT SUPPORT CLOCK.
#endif

#define clock_2_msec(clk) \
    ((int)((clk + (CLK_PER_MSEC/2))/CLK_PER_MSEC))

#define CLIP3(n,min,max) (((n)>(max))? (max) : (((n)<(min))? (min) : (n)))


static void print_usage(void)
{
    int i;
    char str[1024];
    print_log(0, "< Usage >\n");
    for (i = 0; i < ((int)(sizeof(options) / sizeof(options[0])) - 1); i++) {
        if (app_print_cfg(options, i, str) < 0) {
            return;
        }
        print_log(0, "%s\n", str);
    }
}

static int skip_frames(FILE *fp, com_img_t *img, int num_skipped_frames, int bit_depth)
{
    int y_size, u_size, v_size, skipped_size;
    int f_w = img->width[0];
    int f_h = img->height[0];

    if (num_skipped_frames <= 0) {
        return 0;
    }

    int scale = (bit_depth == 10 ? 2 : 1);
    y_size = f_w * f_h * scale;
    u_size = v_size = (f_w >> 1) * (f_h >> 1) * scale;
    skipped_size = (y_size + u_size + v_size) * num_skipped_frames;
    fseek(fp, skipped_size, SEEK_CUR);

    return 0;
}

static int write_image(int fd, com_img_t *img, int bit_depth, long long idx)
{
    unsigned char *p8;
    int i, j;
    int scale = (bit_depth == 10 ? 2 : 1);
    int frm_size;

    frm_size = img->width[0] * img->height[0] * 3 / 2 * scale;

    _lseeki64(fd, frm_size * idx, SEEK_SET);

    for (i = 0; i < 3; i++) {
        p8 = (unsigned char *)img->planes[i];
        for (j = 0; j < img->height[i]; j++) {
            _write(fd, p8, img->width[i] * scale);
            p8 += img->stride[i];
        }
    }
    return 0;
}

static void img_cvt_short_2_char(com_img_t *imgb_dst, com_img_t *imgb_src, int shift)
{
    int i, j, k, t0, add;
    short          *s;
    unsigned char *d;
    add = (shift > 0) ? (1 << (shift - 1)) : 0;
    for (i = 0; i < 3; i++) {
        s = imgb_src->planes[i];
        d = imgb_dst->planes[i];
        for (j = 0; j < imgb_src->height[i]; j++) {
            for (k = 0; k < imgb_src->width[i]; k++) {
                t0 = ((s[k] + add) >> shift);
                d[k] = (unsigned char)(CLIP3(t0, 0, 255));
            }
            s = (short *)(((unsigned char *)s) + imgb_src->stride[i]);
            d = d + imgb_dst->stride[i];
        }
    }
}

static void cvt_rec_2_output(com_img_t *dst, com_img_t *src, int bit_depth)
{
    if (bit_depth == 10 || BIT_DEPTH == 8) {
        int scale = (bit_depth == 10 ? 2 : 1);
        for (int i = 0; i < src->num_planes; i++) {
            unsigned char *s, *d;
            s = (unsigned char *)src->planes[i];
            d = (unsigned char *)dst->planes[i];

            for (int j = 0; j < src->height[i]; j++) {
                memcpy(d, s, scale * src->width[i]);
                s += src->stride[i];
                d += dst->stride[i];
            }
        }
    }
    else { // the output reconstructed file is 8-bit storage for 8-bit depth
        img_cvt_short_2_char(dst, src, 0);
    }
    dst->pts = src->pts;
}

static void image_free(com_img_t *img)
{
    int i;
    for (i = 0; i < MAX_PLANES; i++) {
        if (img->buf[i]) {
            free(img->buf[i]);
        }
    }
    free(img);
}

com_img_t *image_create(int width, int height, int bitDepth)
{
    int i;
    com_img_t *img;
    img = (com_img_t *)malloc(sizeof(com_img_t));
    if (img == NULL) {
        print_log(0, "cannot create image buffer\n");
        return NULL;
    }
    memset(img, 0, sizeof(com_img_t));

    for (i = 0; i < 3; i++) {
        img->width[i] = width;
        img->stride[i] = width * (bitDepth == 8 ? 1 : 2);
        img->height[i] = height;
        img->planes[i] = img->buf[i] = malloc(img->stride[i] * height);

        if (img->planes[i] == NULL) {
            print_log(0, "cannot allocate picture buffer\n");
            return NULL;
        }
        if (i == 0) {
            width = (width + 1) >> 1;
            height = (height + 1) >> 1;
        }
    }
    img->num_planes = 3;

    return img;
}

static int read_image(FILE *fp, com_img_t *img, int hor_size, int ver_size, int bit_depth_input, int bit_depth_internal)
{
    int scale = (bit_depth_input == 10) ? 2 : 1;
    int bit_shift = bit_depth_internal - bit_depth_input;
    const short minval = 0;
    const short maxval = (1 << bit_depth_internal) - 1;

    const int num_comp = 3;

    for (int comp = 0; comp < num_comp; comp++) {
        int padding_w = (img->width[0] - hor_size) >> (comp > 0 ? 1 : 0);
        int padding_h = (img->height[0] - ver_size) >> (comp > 0 ? 1 : 0);
        int size_byte = ((hor_size * ver_size) >> (comp > 0 ? 2 : 0)) * scale;
        int size_buff = (img->width[0] * img->height[0]) >> (comp > 0 ? 2 : 0);

        unsigned char *buf = malloc(size_byte);

        if (fread(buf, 1, size_byte, fp) != (unsigned)size_byte) {
            return -1;
        }

        pel *dst = (pel *)img->planes[comp];

        if (bit_shift < 0 && BIT_DEPTH == 8) {
            int rounding = 1 << ((-bit_shift) - 1);

            for (int y = 0; y < img->height[comp] - padding_h; y++) {
                int ind_src = y * (img->width[comp] - padding_w);
                int ind_dst = y * img->width[comp];
                for (int x = 0; x < img->width[comp] - padding_w; x++) {
                    int val = (buf[(ind_src + x) * 2] | (buf[(ind_src + x) * 2 + 1] << 8));
                    dst[ind_dst + x] = CLIP3((val + rounding) >> (-bit_shift), minval, maxval);
                }
                for (int x = img->width[comp] - padding_w; x < img->width[comp]; x++) { //padding right
                    dst[ind_dst + x] = dst[ind_dst + x - 1];
                }
            }
            for (int y = img->height[comp] - padding_h; y < img->height[comp]; y++) { //padding bottom
                int ind_dst = y * img->width[comp];
                for (int x = 0; x < img->width[comp]; x++) {
                    int i_dst = ind_dst + x;
                    dst[i_dst] = dst[i_dst - img->width[comp]];
                }
            }
        }
        else { // BIT_DEPTH == 10 || bit_depth_internal >= bit_depth_input
            if (bit_depth_input == 10) {
                for (int y = 0; y < img->height[comp] - padding_h; y++) {
                    int ind_src = y * (img->width[comp] - padding_w);
                    int ind_dst = y * img->width[comp];
                    for (int x = 0; x < img->width[comp] - padding_w; x++) {
                        dst[ind_dst + x] = (buf[(ind_src + x) * 2] | (buf[(ind_src + x) * 2 + 1] << 8));

                    }
                    for (int x = img->width[comp] - padding_w; x < img->width[comp]; x++) { //padding right
                        dst[ind_dst + x] = dst[ind_dst + x - 1];
                    }
                }
                for (int y = img->height[comp] - padding_h; y < img->height[comp]; y++) { //padding bottom
                    int ind_dst = y * img->width[comp];
                    for (int x = 0; x < img->width[comp]; x++) {
                        int i_dst = ind_dst + x;
                        dst[i_dst] = dst[i_dst - img->width[comp]];
                    }
                }
            }
            else {
                for (int y = 0; y < img->height[comp] - padding_h; y++) {
                    int ind_src = y * (img->width[comp] - padding_w);
                    int ind_dst = y * img->width[comp];
                    for (int x = 0; x < img->width[comp] - padding_w; x++) {
                        dst[ind_dst + x] = buf[ind_src + x];
                    }
                    for (int x = img->width[comp] - padding_w; x < img->width[comp]; x++) { //padding right
                        dst[ind_dst + x] = dst[ind_dst + x - 1];
                    }
                }
                for (int y = img->height[comp] - padding_h; y < img->height[comp]; y++) { //padding bottom
                    int ind_dst = y * img->width[comp];
                    for (int x = 0; x < img->width[comp]; x++) {
                        int i_dst = ind_dst + x;
                        dst[i_dst] = dst[i_dst - img->width[comp]];
                    }
                }
            }

            if (bit_shift > 0) {
                for (int i = 0; i < size_buff; i++) {
                    dst[i] = dst[i] << bit_shift;
                }
            }
            else if (bit_shift < 0) {
                int rounding = 1 << ((-bit_shift) - 1);
                for (int i = 0; i < size_buff; i++) {
                    dst[i] = CLIP3((dst[i] + rounding) >> (-bit_shift), minval, maxval);
                }
            }
        }
        free(buf);
    }
    return 0;
}

static void print_stat_header(void)
{
    if (g_loglevel < FRAME_LOGLEVEL) {
        return;
    }
    printf("--------------------------------------------------------------------------------------\n");
    printf("  Input YUV file           : %s \n", fn_input);
    if (cfg_flags[CONFIG_FNAME_OUT]) {
        printf("  Output bitstream         : %s \n", fn_output);
    }
    if (cfg_flags[CONFIG_FNAME_REC]) {
        printf("  Output YUV file          : %s \n", fn_rec);
    }
    printf("--------------------------------------------------------------------------------------\n");
    printf("    POC  QP   PSNR-Y   PSNR-U   PSNR-V   Bits    EncT(ms)  Ext_info  Ref. List\n");
    printf("--------------------------------------------------------------------------------------\n");
    fflush(stdout);
}

static void print_config(void *h, enc_cfg_t param)
{
    printf("--------------------------------------------------------------------------------------\n");
    printf("< Sequence's Info >\n");
    printf("\tresolution input         : %d x %d\n", param.horizontal_size, param.vertical_size);
    printf("\tresolution coding        : %d x %d\n", param.pic_width, param.pic_height);
    printf("\tbitdepth input           : %d\n", param.bit_depth_input);
    printf("\tbitdepth coding          : %d\n", param.bit_depth_internal);
    printf("\tframe rate               : %d / %d\n", param.fps_num, param.fps_den);
    printf("\tintra picture period     : %d\n", param.i_period);
    printf("\thierarchical GOP         : %d\n", !param.disable_hgop);
    printf("\tmax b frames             : %d\n", param.max_b_frames);
    printf("\tsignature                : %d\n", param.use_pic_sign);

    printf("\n< Parallel Info >\n");
    printf("\tWPP threads              : %d\n", param.wpp_threads);
    printf("\tframe threads            : %d\n", param.frm_threads);

    printf("\n< RC Info >\n");
    printf("\tRC type                  : %d (0: CQP, 1: CRF, 2: ABR, 3: CBR)\n", param.rc_type);

    if (param.rc_type == RC_TYPE_NULL) {
        printf("\tqp                       : %d\n", param.qp);
    } else if (param.rc_type == RC_TYPE_CRF) {
        printf("\tcrf                      : %d\n", param.rc_crf);
        printf("\tmax_bitrate              : %d\n", param.rc_max_bitrate);
        printf("\tqp range                 : %d-%d\n", param.rc_min_qp, param.rc_max_qp);
    } else  if (param.rc_type == RC_TYPE_ABR || param.rc_type == RC_TYPE_CBR) {
        printf("\tbitrate                  : %d\n", param.rc_bitrate);
        printf("\tmax_bitrate              : %d\n", param.rc_max_bitrate);
        printf("\tqp range                 : %d-%d\n", param.rc_min_qp, param.rc_max_qp);
    }
    printf("\tdqp_enable               : %d\n", param.dqp_enable);
    printf("\tqp_offset_cb             : %d\n", param.qp_offset_cb);
    printf("\tqp_offset_cr             : %d\n", param.qp_offset_cr);

    //printf split configure
    printf("\n< CU split CFG >\n");
    printf("\tctu_size:        %d\n", param.ctu_size);
    printf("\tmin_cu_size:     %d\n", param.min_cu_size);
    printf("\tmax_part_ratio:  %d\n", param.max_part_ratio);
    printf("\tmax_split_times: %d\n", param.max_split_times);
    printf("\tmin_qt_size:     %d\n", param.min_qt_size);
    printf("\tmax_bt_size:     %d\n", param.max_bt_size);
    printf("\tmax_eqt_size:    %d\n", param.max_eqt_size);
    printf("\tmax_dt_size:     %d\n", param.max_dt_size);

    // printf tool configurations
    printf("\n< Tool CFG >\n");

    //loop filter
    printf("\tLoop Filter:  deblock: %d, ", param.use_deblock);
    printf("sao: %d, ", param.sao_enable);
    printf("alf: %d, ", param.alf_enable);
    printf("cross_patch: %d, ", param.filter_cross_patch);
    printf("\n");

    //inter
    printf("\tInter: AMVR(%d) ", param.amvr_enable);
    printf("HMVP_NUM(%d) ", param.num_of_hmvp);
    printf("AFFINE(%d) ", param.affine_enable);
    printf("SMVD(%d) ", param.smvd_enable);
    printf("UMVE(%d) ", param.umve_enable);
    printf("EMVR(%d) ", param.emvr_enable);
    printf("\n");

    //intra
    printf("\tIntra: TSCPM(%d) ", param.tscpm_enable);
    printf("IPF(%d) ", param.ipf_flag);
    printf("DT(%d) ", param.dt_enable);
    printf("\n");

    //transform 
    printf("\tTransform: PBT(%d) ", param.pbt_enable);
    printf("SECTrans(%d) ", param.sectrans_enable);
    printf("\n");

    //quant
    printf("\tQuant: WeightedQuant: %d, ", param.wq_enable);
    printf("\n");

    fflush(stdout);
}

static void find_psnr(com_img_t *org, com_img_t *rec, double psnr[3], int bit_depth)
{
    double sum[3], mse[3];
    pel *o, *r;
    int i, j, k;
    int peak_val = (bit_depth == 8) ? 255 : 1023;
    for (i = 0; i < org->num_planes; i++) {
        o       = (pel *)org->planes[i];
        r       = (pel *)rec->planes[i];
        sum[i] = 0;
        for (j = 0; j < org->height[i]; j++) {
            for (k = 0; k < org->width[i]; k++) {
                sum[i] += (o[k] - r[k]) * (o[k] - r[k]);
            }
            o = (pel *)((unsigned char *)o + org->stride[i]);
            r = (pel *)((unsigned char *)r + rec->stride[i]);
        }
        mse[i] = sum[i] / (org->width[i] * org->height[i]);
        // psnr[i] = (mse[i] == 0.0) ? 100. : fabs(10 * log10(((255 * 255 * 16) / mse[i])));
        psnr[i] = (mse[i] == 0.0) ? 100. : fabs(10 * log10(((peak_val * peak_val) / mse[i])));
    }
}

void print_psnr(enc_stat_t *stat, double *psnr, int bitrate, time_clk_t clk_end)
{
    char  type;

    int i, j;

    switch (stat->type) {
    case SLICE_I:
        type = 'I';
        break;
    case SLICE_P:
        type = 'P';
        break;
    case SLICE_B:
        type = 'B';
        break;
    default:
        type = 'U';
        break;
    }

    print_log(1, "%5lld(%c)%3d%9.4f%9.4f%9.4f%8d%9d ", \
            stat->poc, type, stat->qp, psnr[0], psnr[1], psnr[2], \
            bitrate, clock_2_msec(clk_end));

    print_log(1, " [%s] ", stat->ext_info);

    for (i = 0; i < 2; i++) {
        print_log(1, "[L%d ", i);
        for (j = 0; j < stat->refpic_num[i]; j++) {
            print_log(1, "%lld ", stat->refpic[i][j]);
        }
        print_log(1, "] ");
    }

    print_log(1, "\n");
    fflush(stdout);
}

void enc_cfg_init(enc_cfg_t *cfg)
{
    memset(cfg, 0, sizeof(enc_cfg_t));

    cfg->qp                  =  23;
    cfg->fps_num             =  50;
    cfg->fps_den             =   1;
    cfg->i_period            =  48;
    cfg->bit_depth_input     =   8;
    cfg->bit_depth_internal  =   8;
    cfg->use_pic_sign        =   0;
    cfg->max_b_frames        =  15;
    cfg->disable_hgop        =   0;
    cfg->amvr_enable         =   1;
    cfg->affine_enable       =   1;
    cfg->smvd_enable         =   1;
    cfg->use_deblock         =   1;
    cfg->num_of_hmvp         =   8;
    cfg->ipf_flag            =   1;
    cfg->tscpm_enable        =   1;
    cfg->umve_enable         =   1;
    cfg->emvr_enable         =   1;
    cfg->dt_enable           =   1;
    cfg->wq_enable           =   0;
    cfg->sao_enable          =   1;
    cfg->alf_enable          =   1;
    cfg->sectrans_enable     =   1;
    cfg->pbt_enable          =   1;
    cfg->dqp_enable          =   0;
    cfg->chroma_format       =   1;
    cfg->rpls_l0_cfg_num     =   0;
    cfg->rpls_l1_cfg_num     =   0;
    cfg->filter_cross_patch  =   1;
    cfg->colocated_patch     =   0;
    cfg->ctu_size            = 128;
    cfg->min_cu_size         =   4;
    cfg->max_part_ratio      =   8;
    cfg->max_split_times     =   6;
    cfg->min_qt_size         =   8;
    cfg->max_bt_size         = 128;
    cfg->max_eqt_size        =  64;
    cfg->max_dt_size         =  64;
    cfg->qp_offset_cb        =   0;
    cfg->qp_offset_cr        =   0;
    cfg->wpp_threads         =   1;
    cfg->frm_threads         =   1;
    cfg->rc_type             =   0;
}

int main(int argc, const char **argv)
{
    int                finished = 0;
    FILE               *fpi = NULL;
    FILE               *fpo = NULL;
    void               *h;
    com_img_t          *img_enc = NULL; 
    com_img_t          *img_rec = NULL; 
    enc_stat_t         stat;
    char               enc_ext_info[2048] = "";
    int                i, ret, size;
    time_clk_t         time_start, total_time;
    long long          frame_cnt;
    int                num_encoded_frames = 0;
    double             bitrate;
    double             psnr[3] = {0,};
    double             psnr_avg[3] = {0,};
    com_img_t          *tmp_img = NULL;
    int rec_fd = 0;

    enc_cfg_init(&cfg);

    srand((unsigned int)(time(NULL)));

    /* parse options */
    if (app_parse_all(argc, argv, options) < 0) {
        print_log(0, "Configuration error, please refer to the usage.\n");
        print_usage();
        return -1;
    }
    if (parse_rpl_cfg(&cfg)) {
        print_usage();
        return -1;
    }

    fpi = fopen(fn_input,  "rb");
    if (fpi == NULL) {
        print_log(0, "cannot open original file (%s)\n", fn_input);
        print_usage();
        return -1;
    }
    if (cfg_flags[CONFIG_FNAME_OUT]) {
        fpo = fopen(fn_output, "wb");
        if (fpo == NULL) {
            print_log(0, "cannot open stream file (%s)\n", fn_input);
            print_usage();
            return -1;
        }
    }
    if (cfg_flags[CONFIG_FNAME_REC]) {
        rec_fd = _open(fn_rec, O_WRONLY | O_CREAT | O_BINARY | O_TRUNC, S_IREAD | S_IWRITE);
        if (rec_fd == 0) {
            print_log(0, "cannot open original file (%s)\n", fn_input);
            print_usage();
            return -1;
        }
    }

    frame_to_be_encoded = (max_frames + t_ds_ratio - 1) / t_ds_ratio;

    /* create encoder */
    h = uavs3e_create(&cfg, NULL);

    if (h == NULL) {
        print_log(0, "cannot create encoder\n");
        return -1;
    }

    tmp_img = image_create(cfg.pic_width, cfg.pic_height, cfg.bit_depth_internal);

    print_config(h, cfg);
    print_stat_header();

    stat.ext_info = enc_ext_info;
    stat.ext_info_buf_size = sizeof(enc_ext_info);

    bitrate = 0;

    /* encode Sequence Header if needed **************************************/
    total_time = 0;
    frame_cnt  = 0;

    /* encode pictures *******************************************************/
    while (1) {
        com_img_t *img_enc = NULL;

        if (finished == 0) {
            /* get encoding buffer */
            if (COM_OK != uavs3e_get_img(h, &img_enc)) {
                print_log(0, "Cannot get original image buffer\n");
                return -1;
            }
            /* read original image */
            if (frame_cnt == frame_to_be_encoded || read_image(fpi, img_enc, cfg.horizontal_size, cfg.vertical_size, cfg.bit_depth_input, cfg.bit_depth_internal)) {
                print_log(2, "Entering bumping process...\n");
                finished = 1;
                img_enc = NULL;
            } else {
                img_enc->pts = frame_cnt++;
                skip_frames(fpi, img_enc, t_ds_ratio - 1, cfg.bit_depth_input);
            }
        }
        /* encoding */
        time_start = app_get_time();

        ret = uavs3e_enc(h, &stat, img_enc);

        time_clk_t now = app_get_time(); 
        time_clk_t time_dur = now - time_start;
        total_time += time_dur;

        /* store bitstream */
        if (ret == COM_OK_OUT_NOT_AVAILABLE) {
            continue;
        } else if (ret == COM_OK) {
            if (fpo && stat.bytes > 0) {
                fwrite(stat.buf, 1, stat.bytes, fpo);
            }
            /* get reconstructed image */
            size = sizeof(com_img_t **);
            img_rec = stat.rec_img;
            num_encoded_frames++;

            if (ret < 0) {
                print_log(0, "failed to get reconstruction image\n");
                return -1;
            }

            /* calculate PSNR */
            psnr[0] = psnr[1] = psnr[2] = 0;
            find_psnr(stat.org_img, img_rec, psnr, cfg.bit_depth_internal);

            /* store reconstructed image to list only for writing out */
            if (rec_fd) {
                cvt_rec_2_output(tmp_img, img_rec, cfg.bit_depth_internal);

                if (cfg_flags[CONFIG_FNAME_REC]) {
                    if (write_image(rec_fd, tmp_img, cfg.bit_depth_internal, img_rec->pts)) {
                        print_log(0, "cannot write reconstruction image\n");
                    }
                }

            }

            print_psnr(&stat, psnr, (stat.bytes - stat.user_bytes) << 3, time_dur);
            bitrate += (stat.bytes - stat.user_bytes);

            for (i = 0; i < 3; i++) {
                psnr_avg[i] += psnr[i];
            }
        } else if (ret == COM_OK_NO_MORE_FRM) {
            break;
        } else {
            print_log(2, "uavs3e_enc() err: %d\n", ret);
            return -1;
        }
    }

    char end_code[4] = { 0, 0, 1, 0xB1 };
    fwrite(end_code, 1, 4, fpo);

    print_log(1, "===============================================================================\n");
    psnr_avg[0] /= frame_cnt;
    psnr_avg[1] /= frame_cnt;
    psnr_avg[2] /= frame_cnt;

    print_log(1, "  PSNR Y(dB)       : %-5.4f\n", psnr_avg[0]);
    print_log(1, "  PSNR U(dB)       : %-5.4f\n", psnr_avg[1]);
    print_log(1, "  PSNR V(dB)       : %-5.4f\n", psnr_avg[2]);

    print_log(1, "  Total bits(bits) : %-.0f\n", bitrate * 8);
    bitrate *= (cfg.fps_num / cfg.fps_den * 8);

    bitrate /= frame_cnt;
    bitrate /= 1000;
    print_log(1, "  bitrate(kbps)    : %-5.4f\n", bitrate);
    print_log(1, "===============================================================================\n");
    print_log(1, "Encoded frame count               = %d\n", (int)frame_cnt);
    print_log(1, "Total encoding time               = %.3f msec,",
            (float)clock_2_msec(total_time));
    print_log(1, " %.3f sec\n", (float)(clock_2_msec(total_time) / 1000.0));
    print_log(1, "Average encoding time for a frame = %.3f msec\n",
            (float)clock_2_msec(total_time) / frame_cnt);
    print_log(1, "Average encoding speed            = %.5f frames/sec\n",
            ((float)frame_cnt * 1000) / ((float)clock_2_msec(total_time)));
    print_log(1, "===============================================================================\n");
    fflush(stdout);

#if 1
    FILE *fp = fopen("psnr.txt", "a+");
    fprintf(fp, "%s %.4f %.4f %.4f %.4f    %.5f\n", fn_output, bitrate, psnr_avg[0], psnr_avg[1], psnr_avg[2], 
                                                   ((float)frame_cnt * 1000) / ((float)clock_2_msec(total_time)));
    fclose(fp);
#endif

    uavs3e_free(h);

    if (fpi) {
        fclose(fpi);
    }
    if (fpo) {
        fclose(fpo);
    }
    if (rec_fd) {
        _close(rec_fd);
    }

    image_free(tmp_img);

    getchar();

    return 0;
}
