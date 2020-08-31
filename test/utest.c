/**************************************************************************************
 * Copyright (c) 2018-2020 ["Peking University Shenzhen Graduate School",
 *   "Peng Cheng Laboratory", and "Guangdong Bohua UHD Innovation Corporation"]
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes the software uAVS3d developed by
 *    Peking University Shenzhen Graduate School, Peng Cheng Laboratory
 *    and Guangdong Bohua UHD Innovation Corporation.
 * 4. Neither the name of the organizations (Peking University Shenzhen Graduate School,
 *    Peng Cheng Laboratory and Guangdong Bohua UHD Innovation Corporation) nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include <io.h>
#endif
#include "uavs3e.h"
#include "config.h"

#define S_IREAD        0000400         /* read  permission, owner */
#define S_IWRITE       0000200         /* write permission, owner */

#define LOG_LEVEL_0                  0
#define LOG_LEVEL_1                  1
#define LOG_LEVEL_2                  2
#define FRAME_LOGLEVEL              LOG_LEVEL_1

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

enc_cfg_t   cfg;
static char fn_cfg   [256] = "\0";
static char fn_input [256] = "\0";
static char fn_output[256] = "\0";
static char fn_rec   [256] = "\0";
static int  max_frames = 0;
static int  t_ds_ratio = 1;
static int  g_loglevel = LOG_LEVEL_1;
static int  frame_to_be_encoded;

static app_cfg_t options[] = {
    {
        CFG_KEY_NULL, "config", CFG_TYPE_STRING,
        fn_cfg,
        "file name of configuration"
        ,0 
    },
    {
        'i', "input", CFG_TYPE_STRING | CFG_TYPE_MANDATORY,
        fn_input,
        "file name of input video"
        ,0 
    },
    {
        'o', "output", CFG_TYPE_STRING,
        fn_output,
        "file name of output bitstream"
        ,0 
    },
    {
        'r', "recon", CFG_TYPE_STRING,
        fn_rec,
        "file name of reconstructed video"
        ,0 
    },
    {
        'w',  "width", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg.horizontal_size,
        "pixel width of input video"
        ,0 
    },
    {
        'h',  "height", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg.vertical_size,
        "pixel height of input video"
        ,0
    },
    {
        'q',  "qp", CFG_TYPE_INTEGER,
        &cfg.qp,
        "QP value (0~63 for 8bit input, -16~63 for 10bit input)"
        ,0 
    },
    {
        'p',  "i_period", CFG_TYPE_INTEGER,
        &cfg.i_period,
        "I-picture period (0: Only one I frame)"
        ,0 
    },
    {
        'g',  "max_b_frames", CFG_TYPE_INTEGER,
        &cfg.max_b_frames,
        "Number of maximum B frames (1,3,7,15)\n"
        ,0 },
    {
        'f',  "frames", CFG_TYPE_INTEGER,
        &max_frames,
        "maximum number of frames to be encoded"
        ,0 
    },
    {
        's',  "signature", CFG_TYPE_INTEGER,
        &cfg.use_pic_sign,
        "embed picture signature (HASH) for conformance checking in decoding"
        "\t 0: disable\n"
        "\t 1: enable\n"
        ,0 
    },
    {
        'v',  "verbose", CFG_TYPE_INTEGER,
        &g_loglevel,
        "verbose level\n"
        "\t 0: no message\n"
        "\t 1: frame-level messages (default)\n"
        "\t 2: all messages\n"
        ,0 
    },
    {
        'd',  "input_bit_depth", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg.bit_depth_input,
        "input bitdepth (8(default), 10) "
        ,0 
    },
    {
        CFG_KEY_NULL,  "fps_num", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg.fps_num,
        "frame rate (Hz), numerator"
        ,0 
    },
    {
        CFG_KEY_NULL,  "fps_den", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg.fps_den,
        "frame rate (Hz), denominator"
        ,0 
    },
    {
        CFG_KEY_NULL,  "internal_bit_depth", CFG_TYPE_INTEGER,
        &cfg.bit_depth_internal,
        "output bitdepth (8, 10)(default: same as input bitdpeth) "
        ,0
    },
    {
        CFG_KEY_NULL,  "close_gop", CFG_TYPE_INTEGER,
        &cfg.close_gop,
        "use close gop"
        ,0
    },
    {
        CFG_KEY_NULL,  "scenecut", CFG_TYPE_INTEGER,
        &cfg.scenecut,
        "scenecut threshold (0~100, 0:off)"
        ,0
    },
    {
        CFG_KEY_NULL,  "schistogram", CFG_TYPE_INTEGER,
        &cfg.scenecut_histogram,
        "M5582 histogram-based scenecut detection(0:off, 1:on)"
        ,0
    },
    {
        CFG_KEY_NULL,  "lookahead", CFG_TYPE_INTEGER,
        &cfg.lookahead,
        "size of lookahead window"
        ,0
    },
    {
        CFG_KEY_NULL,  "adaptive_gop", CFG_TYPE_INTEGER,
        &cfg.adaptive_gop,
        "adaptive length of sub-gop"
        ,0
    },
    {
        CFG_KEY_NULL,  "chroma_dqp", CFG_TYPE_INTEGER,
        &cfg.chroma_dqp,
        "adaptive frame-level delta QP of chroma"
        ,0
    },
    {
        CFG_KEY_NULL,  "qp_offset_cb", CFG_TYPE_INTEGER,
        &cfg.qp_offset_cb,
        "qp offset for cb, disable:0 (default)"
        ,0
    },
    {
        CFG_KEY_NULL,  "qp_offset_cr", CFG_TYPE_INTEGER,
        &cfg.qp_offset_cr,
        "qp offset for cr, disable:0 (default)"
        ,0 
    },
    {
        CFG_KEY_NULL,  "speed_level", CFG_TYPE_INTEGER,
        &cfg.speed_level,
        "Level of coding speed"
        ,0 
    },
    {
        CFG_KEY_NULL,  "wpp_threads", CFG_TYPE_INTEGER,
        &cfg.wpp_threads,
        "Number of threads for WPP"
        ,0 
    },
    {
        CFG_KEY_NULL,  "frm_threads", CFG_TYPE_INTEGER,
        &cfg.frm_threads,
        "Number of threads for Frame"
        ,0 
    },
    {
        CFG_KEY_NULL,  "adaptive_dqp", CFG_TYPE_INTEGER,
        &cfg.adaptive_dqp,
        "Variance  based Adaptive delta QP of LCU"
        ,0 
    },
    {
        CFG_KEY_NULL,  "amvr", CFG_TYPE_INTEGER,
        &cfg.amvr_enable,
        "amvr on/off flag"
        ,0
    },
    {
        CFG_KEY_NULL,  "ipf", CFG_TYPE_INTEGER,
        &cfg.ipf_flag,
        "Intra prediction filter on/off flag"
        ,0
    },
    {
        CFG_KEY_NULL, "wq_enable", CFG_TYPE_INTEGER,
        &cfg.wq_enable,
        "Weight Quant on/off, disable: 0(default), enable: 1"
        ,0
    },
    {
        CFG_KEY_NULL, "seq_wq_mode", CFG_TYPE_INTEGER,
        &cfg.seq_wq_mode,
        "Seq Weight Quant (0: default, 1: User Define)"
        ,0
    },
    {
        CFG_KEY_NULL, "seq_wq_user", CFG_TYPE_STRING,
        &cfg.seq_wq_user,
        "User Defined Seq WQ Matrix"
        ,0 
    },
    {
        CFG_KEY_NULL, "pic_wq_data_idx", CFG_TYPE_INTEGER,
        &cfg.pic_wq_data_idx,
        "Pic level WQ data index, 0(default):refer to seq_header,  1:derived by WQ parameter,  2:load from pichdr"
        ,0 
    },
    {
        CFG_KEY_NULL, "pic_wq_user", CFG_TYPE_STRING,
        &cfg.pic_wq_user,
        "User Defined Pic WQ Matrix"
        ,0
    },
    {
        CFG_KEY_NULL, "wq_param", CFG_TYPE_INTEGER,
        &cfg.wq_param,
        "WQ Param (0=Default, 1=UnDetailed, 2=Detailed)"
        ,0 
    },
    {
        CFG_KEY_NULL, "wq_model", CFG_TYPE_INTEGER,
        &cfg.wq_model,
        "WQ Model (0~2, default:0)"
        ,0
    },
    {
        CFG_KEY_NULL, "wq_param_detailed", CFG_TYPE_STRING,
        &cfg.wq_param_detailed,
        "default:[64,49,53,58,58,64]"
        ,0
    },
    {
        CFG_KEY_NULL, "wq_param_undetailed", CFG_TYPE_STRING,
        &cfg.wq_param_undetailed,
        "default:[67,71,71,80,80,106]"
        ,0 
    },
    {
        CFG_KEY_NULL,  "hmvp", CFG_TYPE_INTEGER,
        &cfg.num_of_hmvp,
        "number of hmvp skip candidates (default: 8, disable: 0)"
        ,0 
    },
    {
        CFG_KEY_NULL,  "tscpm", CFG_TYPE_INTEGER,
        &cfg.tscpm_enable,
        "tscpm on/off flag"
        ,0
    },
    {
        CFG_KEY_NULL,  "umve", CFG_TYPE_INTEGER,
        &cfg.umve_enable,
        "umve on/off flag"
        ,0 
    },
    {
        CFG_KEY_NULL,  "emvr", CFG_TYPE_INTEGER,
        &cfg.emvr_enable,
        "emvr on/off flag"
        ,0 
    },
    {
        CFG_KEY_NULL,  "affine", CFG_TYPE_INTEGER,
        &cfg.affine_enable,
        "affine on/off flag"
        ,0
    },
    {
        CFG_KEY_NULL,  "smvd", CFG_TYPE_INTEGER,
        &cfg.smvd_enable,
        "smvd on/off flag (on: 1, off: 0, default: 1)"
        ,0
    },
    {
        CFG_KEY_NULL,  "dt_intra", CFG_TYPE_INTEGER,
        &cfg.dt_enable,
        "dt_intra on/off flag (on: 1, off: 0, default: 1)"
        ,0
    },
    {
        CFG_KEY_NULL,  "sectrans", CFG_TYPE_INTEGER,
        &cfg.sectrans_enable,
        "second transform (on: 1, off: 0, default: 1)"
        ,0
    },
    {
        CFG_KEY_NULL,  "pbt", CFG_TYPE_INTEGER,
        &cfg.pbt_enable,
        "pbt on/off flag (on: 1, off: 0, default: 1)"
        ,0 
    },
    {
        CFG_KEY_NULL, "deblock", CFG_TYPE_INTEGER,
        &cfg.use_deblock,
        "deblock on/off flag (on: 1, off: 0, default: 1)"
        ,0
    },
    {
        CFG_KEY_NULL, "sao", CFG_TYPE_INTEGER,
        &cfg.sao_enable,
        "sao on/off flag (on: 1, off: 0, default: 1)"
        ,0
    },
    {
        CFG_KEY_NULL, "alf", CFG_TYPE_INTEGER,
        &cfg.alf_enable,
        "alf on/off flag (on: 1, off: 0, default: 1)"
        ,0
    },
    {
        CFG_KEY_NULL, "cross_patch_loopfilter", CFG_TYPE_INTEGER,
        &cfg.filter_cross_patch,
        "loop_filter_across_patch_flag(1:cross;0:non cross)"
        ,0
    },
    {
        CFG_KEY_NULL, "colocated_patch", CFG_TYPE_INTEGER,
        &cfg.colocated_patch,
        "if the MV out of the patch boundary,0:non use colocated;1:use colocated"
        ,0 
    },
    {
        CFG_KEY_NULL, "patch_width", CFG_TYPE_INTEGER,
        &cfg.patch_width,
        "when uniform is 1,the nember of LCU in horizon in a patch"
        ,0 
    },
    {
        CFG_KEY_NULL, "patch_height", CFG_TYPE_INTEGER,
        &cfg.patch_height,
        "when uniform is 1,the nember of LCU in vertical in a patch"
        ,0 
    },
    {
        CFG_KEY_NULL, "TemporalSubsampleRatio", CFG_TYPE_INTEGER,
        &t_ds_ratio,
        "Subsampling Ratio (default: 8 for AI, 1 for RA and LD)"
        ,0
    },
    {
        CFG_KEY_NULL, "ctu_size", CFG_TYPE_INTEGER,
        &cfg.ctu_size,
        "ctu_size (default: 128; allowed values: 32, 64, 128)"
        ,0
    },
    {
        CFG_KEY_NULL, "min_cu_size", CFG_TYPE_INTEGER,
        &cfg.min_cu_size,
        "min_cu_size (default: 4; allowed values: 4)"
        ,0
    },
    {
        CFG_KEY_NULL, "max_part_ratio", CFG_TYPE_INTEGER,
        &cfg.max_part_ratio,
        "max_part_ratio (default:8; allowed values: 8)"
        ,0
    },
    {
        CFG_KEY_NULL, "max_split_times", CFG_TYPE_INTEGER,
        &cfg.max_split_times,
        "max_split_times (default: 6, allowed values: 6)"
        ,0 
    },
    {
        CFG_KEY_NULL, "min_qt_size", CFG_TYPE_INTEGER,
        &cfg.min_qt_size,
        "min_qt_size (default: 8, allowed values: 64, 128)"
        ,0 
    },
    {
        CFG_KEY_NULL, "max_bt_size", CFG_TYPE_INTEGER,
        &cfg.max_bt_size,
        "max_bt_size (default: 128, allowed values: 4, 8, 16, 32, 64, 128)"
        ,0
    },
    {
        CFG_KEY_NULL, "max_eqt_size", CFG_TYPE_INTEGER,
        &cfg.max_eqt_size,
        "max_eqt_size (default: 64, allowed values: 8, 16, 32, 64)"
        ,0
    },
    {
        CFG_KEY_NULL, "max_dt_size", CFG_TYPE_INTEGER,
        &cfg.max_dt_size,
        "max_dt_size (default: 64, allowed values: 16, 32, 64)"
        ,0
    },
    {
        CFG_KEY_NULL, "rc_type", CFG_TYPE_INTEGER,
        &cfg.rc_type,
        "rc_type (default: 0-CQP, allowed values: 1-CRF, 2-ABR, 3-CBR)"
        ,0
    },
    {
        CFG_KEY_NULL, "rc_bitrate", CFG_TYPE_INTEGER,
        &cfg.rc_bitrate,
        "rc_bitrate (kbps)"
        ,0 
    },
    {
        CFG_KEY_NULL, "rc_max_bitrate", CFG_TYPE_INTEGER,
        &cfg.rc_max_bitrate,
        "rc_max_bitrate (kbps), 0 is disable"
        ,0
    },
    {
        CFG_KEY_NULL, "rc_min_qp", CFG_TYPE_INTEGER,
        &cfg.rc_min_qp,
        "rc_min_qp (default: 0)"
        ,0
    },
    {
        CFG_KEY_NULL, "rc_max_qp", CFG_TYPE_INTEGER,
        &cfg.rc_max_qp,
        "rc_max_qp (default: 63)"
        ,0
    },
    {
        CFG_KEY_NULL, "rc_crf", CFG_TYPE_INTEGER,
        &cfg.rc_crf,
        "crf value (0 - 63)"
        ,0 
    },
    { 0, "", CFG_TYPE_NULL, NULL, "" , 0 } /* termination */
};

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

static int skip_frames(int fd, com_img_t *img, int num_skipped_frames, int bit_depth)
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
    _lseeki64(fd, skipped_size, SEEK_CUR);

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

static int read_image(int fd, com_img_t *img, int hor_size, int ver_size, int bit_depth_input, int bit_depth_internal, long long frm_idx)
{
    int scale = (bit_depth_input == 10) ? 2 : 1;
    int bit_shift = bit_depth_internal - bit_depth_input;
    const short minval = 0;
    const short maxval = (1 << bit_depth_internal) - 1;
    const int num_comp = 3;
    int total_size = ((hor_size * ver_size) * 3 / 2) * scale;
    int seek_offset = 0;

    //if (frm_idx > 13 && frm_idx < 16) { // scenecut test
    //    seek_offset = 200;
    //}

    _lseeki64(fd, total_size * (frm_idx + seek_offset), SEEK_SET);

    for (int comp = 0; comp < num_comp; comp++) {
        int padding_w = (img->width[0] - hor_size) >> (comp > 0 ? 1 : 0);
        int padding_h = (img->height[0] - ver_size) >> (comp > 0 ? 1 : 0);
        int size_byte = ((hor_size * ver_size) >> (comp > 0 ? 2 : 0)) * scale;
        int size_buff = (img->width[0] * img->height[0]) >> (comp > 0 ? 2 : 0);

        unsigned char *buf = malloc(size_byte);

        if (_read(fd, buf, size_byte) != (unsigned)size_byte) {
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
    printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
    printf("  Input YUV file           : %s \n", fn_input);
    if (strlen(fn_output) != 0) {
        printf("  Output bitstream         : %s \n", fn_output);
    }
    if (strlen(fn_rec) != 0) {
        printf("  Output YUV file          : %s \n", fn_rec);
    }
    printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
    printf("    POC | QP |  PSNR-Y  PSNR-U  PSNR-V| SSIM-Y SSIM-U SSIM-V|   Bits |  Time |        Ref. List      | Ext_info\n");
    fflush(stdout);
}

static void print_config(void *h, enc_cfg_t param)
{
    printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
    printf("< Sequence's Info >\n");
    printf("\tresolution input         : %d x %d\n", param.horizontal_size, param.vertical_size);
    printf("\tresolution coding        : %d x %d\n", param.pic_width, param.pic_height);
    printf("\tbitdepth input           : %d\n", param.bit_depth_input);
    printf("\tbitdepth coding          : %d\n", param.bit_depth_internal);
    printf("\tframe rate               : %d / %d\n", param.fps_num, param.fps_den);
    printf("\tintra picture period     : %d\n", param.i_period);
    printf("\tclose_gop                : %d\n", param.close_gop);
    printf("\tmax b frames             : %d\n", param.max_b_frames);
    printf("\tsignature                : %d\n", param.use_pic_sign);

    printf("\n< LookAhead Info >\n");
    printf("\tlookahead                : %d\n", param.lookahead);
    printf("\tscenecut                 : %d\n", param.scenecut);
    printf("\tschistogram              : %d\n", param.scenecut_histogram);
    printf("\tadaptive_gop             : %d\n", param.adaptive_gop);

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
    printf("\tQuant: WeightedQuant: %d ", param.wq_enable);
    printf("\n");

    //encoder-side tools
    printf("\tENC-Side Tools: chroma_qp(%d) ", param.chroma_dqp);
    printf("AQ(%d) ", param.adaptive_dqp);

    printf("\n");

    //speed-up tools
    printf("\tSpeed_level: %d", param.speed_level);

    printf("\n");

    fflush(stdout);
}

void print_psnr(enc_stat_t *stat, double *psnr, double *ssim, int bitrate, time_clk_t clk_end)
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

    print_log(1, "%5lld(%c)|%4.1f|%8.4f%8.4f%8.4f|%7.4f%7.4f%7.4f|%8d|%7d|", \
            stat->poc, type, stat->qp, psnr[0], psnr[1], psnr[2], ssim[0], ssim[1], ssim[2],\
            bitrate, clock_2_msec(clk_end));

    for (i = 0; i < 2; i++) {
        print_log(1, "L%d ", i);
        for (j = 0; j < stat->refpic_num[i]; j++) {
            print_log(1, "%3lld ", stat->refpic[i][j]);
        }
        for (; j < 2; j++) {
            print_log(1, "    ");
        }
        print_log(1, "|");
    }

    print_log(1, "[%s] ", stat->ext_info);

    print_log(1, "\n");
    fflush(stdout);
}

int main(int argc, const char **argv)
{
    int                finished = 0;
    int                fdi = 0;
    int                fdo = 0;
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
    double             psnr[3], ssim[3];
    double             psnr_avg[3] = { 0 };
    double             ssim_avg[3] = { 0 };
    com_img_t          *tmp_img = NULL;
    int fd_rec = 0;

    uavs3e_load_default_cfg(&cfg);

    srand((unsigned int)(time(NULL)));

    /* parse options */
    if (app_parse_all(argc, argv, options) < 0) {
        print_log(0, "Configuration error, please refer to the usage.\n");
        print_usage();
        return -1;
    }

    fdi = _open(fn_input, O_BINARY | O_RDONLY);

    if (fdi <= 0) {
        print_log(0, "cannot open original file (%s)\n", fn_input);
        print_usage();
        return -1;
    }
    if (strlen(fn_output) != 0) {
        fdo = _open(fn_output, O_WRONLY | O_CREAT | O_BINARY | O_TRUNC, S_IREAD | S_IWRITE);

        if (fdo <= 0) {
            print_log(0, "cannot open stream file (%s)\n", fn_output);
            print_usage();
            return -1;
        }
    }
    if (strlen(fn_rec) != 0) {
        fd_rec = _open(fn_rec, O_WRONLY | O_CREAT | O_BINARY | O_TRUNC, S_IREAD | S_IWRITE);
        if (fd_rec <= 0) {
            print_log(0, "cannot open original file (%s)\n", fn_input);
            print_usage();
            return -1;
        }
    }

    if (max_frames) {
        frame_to_be_encoded = (max_frames + t_ds_ratio - 1) / t_ds_ratio;
    } else {
        frame_to_be_encoded = -1;
    }

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
            if (frame_cnt == frame_to_be_encoded || read_image(fdi, img_enc, cfg.horizontal_size, cfg.vertical_size, cfg.bit_depth_input, cfg.bit_depth_internal, frame_cnt)) {
                print_log(2, "Entering bumping process...\n");
                finished = 1;
                img_enc = NULL;
            } else {
                img_enc->pts = frame_cnt++;
                skip_frames(fdi, img_enc, t_ds_ratio - 1, cfg.bit_depth_input);
            }
        }
        /* encoding */
        time_start = app_get_time();
        stat.insert_idr = 0;

        ret = uavs3e_enc(h, &stat, img_enc);

        time_clk_t now = app_get_time(); 
        time_clk_t time_dur = now - time_start;
        total_time += time_dur;

        /* store bitstream */
        if (ret == COM_OK_OUT_NOT_AVAILABLE) {
            continue;
        } else if (ret == COM_OK) {
            if (fdo > 0 && stat.bytes > 0) {
                _write(fdo, stat.buf, stat.bytes);
            }
            /* get reconstructed image */
            size = sizeof(com_img_t **);
            img_rec = stat.rec_img;
            num_encoded_frames++;

            if (ret < 0) {
                print_log(0, "failed to get reconstruction image\n");
                return -1;
            }

            /* calculate PSNR & SSIM */
            uavs3e_find_psnr(stat.org_img, img_rec, psnr, cfg.bit_depth_internal);
            uavs3e_find_ssim(stat.org_img, img_rec, ssim, cfg.bit_depth_internal);

            /* store reconstructed image to list only for writing out */
            if (fd_rec > 0) {
                cvt_rec_2_output(tmp_img, img_rec, cfg.bit_depth_internal);

                if (write_image(fd_rec, tmp_img, cfg.bit_depth_internal, img_rec->pts)) {
                    print_log(0, "cannot write reconstruction image\n");
                }
            }

            print_psnr(&stat, psnr, ssim, (stat.bytes - stat.user_bytes) << 3, time_dur);

            bitrate += (stat.bytes - stat.user_bytes);

            for (i = 0; i < 3; i++) {
                psnr_avg[i] += psnr[i];
                ssim_avg[i] += ssim[i];
            }

        } else if (ret == COM_OK_NO_MORE_FRM) {
            break;
        } else {
            print_log(2, "uavs3e_enc() err: %d\n", ret);
            return -1;
        }
    }

    char end_code[4] = { 0, 0, 1, 0xB1 };

    if (fdo > 0) {
        _write(fdo, end_code, 4);
    }

    print_log(1, "\n\n===============================================================================\n");
    psnr_avg[0] /= frame_cnt;
    psnr_avg[1] /= frame_cnt;
    psnr_avg[2] /= frame_cnt;
    ssim_avg[0] /= frame_cnt;
    ssim_avg[1] /= frame_cnt;
    ssim_avg[2] /= frame_cnt;

    print_log(1, "  PSNR Y(dB)       : %-5.4f\n", psnr_avg[0]);
    print_log(1, "  PSNR U(dB)       : %-5.4f\n", psnr_avg[1]);
    print_log(1, "  PSNR V(dB)       : %-5.4f\n", psnr_avg[2]);
    print_log(1, "  SSIM Y(dB)       : %-5.4f\n", ssim_avg[0]);
    print_log(1, "  SSIM U(dB)       : %-5.4f\n", ssim_avg[1]);
    print_log(1, "  SSIM V(dB)       : %-5.4f\n", ssim_avg[2]);

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
    fprintf(fp, "%s    %.4f %.4f %.4f %.4f    %.4f %.4f %.4f    %.5f\n", fn_output, bitrate, 
                                                    psnr_avg[0], psnr_avg[1], psnr_avg[2],
                                                    ssim_avg[0], ssim_avg[1], ssim_avg[2],
                                                   ((float)frame_cnt * 1000) / ((float)clock_2_msec(total_time)));
    fclose(fp);
#endif

    uavs3e_free(h);

    if (fdi > 0) {
        _close(fdi);
    }
    if (fdo > 0) {
        _close(fdo);
    }
    if (fd_rec > 0) {
        _close(fd_rec);
    }

    image_free(tmp_img);

    //getchar();

    return 0;
}
