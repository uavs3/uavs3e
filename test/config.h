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

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LOG_LEVEL_0                  0
#define LOG_LEVEL_1                  1
#define LOG_LEVEL_2                  2
#define FRAME_LOGLEVEL              LOG_LEVEL_1

#define CFG_TYPE_MANDATORY       (1 <<0) /* mandatory or not   */
#define CFG_TYPE_NULL            (0 <<1) /* no value           */
#define CFG_TYPE_INTEGER         (10<<1) /* integer type value */
#define CFG_TYPE_STRING          (20<<1) /* string type value  */

#define CFG_TYPE_GET(x)  ((x) & ~CFG_TYPE_MANDATORY)
#define CFG_KEY_NULL                   (127)

typedef struct uavs3e_app_cfg_t {
    char   key;          /* option keyword */
    char   key_long[32]; /* option long keyword */
    int    val_type;    
    void  *val;          
    char   desc[512];    /* description of option */
    char   flag;
} app_cfg_t;

enc_cfg_t   cfg;
static char fn_cfg[256]                 = "\0"; 
static char fn_input[256]               = "\0";
static char fn_output[256]              = "\0";
static char fn_rec[256]                 = "\0"; 
static int  max_frames                  = 0;
static int  t_ds_ratio                  = 1;
static int  g_loglevel                  = LOG_LEVEL_1;

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
        "I-picture period"
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
        'd',  "input_bit_depth", CFG_TYPE_INTEGER,
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
        CFG_KEY_NULL,  "lcu_delta_qp", CFG_TYPE_INTEGER,
        &cfg.dqp_enable,
        "Random qp for lcu, on/off flag"
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
        "rc_max_bitrate (kbps)"
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

static int app_cfg_search_string(app_cfg_t *opts, const char *argv)
{
    int oidx = 0;
    app_cfg_t *o;
    o = opts;
    while (o->key != 0) {
        if (!strcmp(argv, o->key_long)) {
            return oidx;
        }
        oidx++;
        o++;
    }
    return -1;
}

static int app_cfg_search_char(app_cfg_t *ops, const char argv)
{
    int oidx = 0;
    app_cfg_t *o;
    o = ops;
    while (o->key != 0) {
        if (o->key != CFG_KEY_NULL && o->key == argv) {
            return oidx;
        }
        oidx++;
        o++;
    }
    return -1;
}

static int app_cfg_read_value(app_cfg_t *ops, const char *argv)
{
    if (argv == NULL) {
        return -1;
    }
    if (argv[0] == '-' && (argv[1] < '0' || argv[1] > '9')) {
        return -1;
    }
    switch (CFG_TYPE_GET(ops->val_type)) {
    case CFG_TYPE_INTEGER:
        *((int *)ops->val) = atoi(argv);
        break;
    case CFG_TYPE_STRING:
        strcpy((char *)ops->val, argv);
        break;
    default:
        return -1;
    }
    return 0;
}

static int app_print_cfg(app_cfg_t *ops, int idx, char *help)
{
    int optional = 0;
    char vtype[32];
    app_cfg_t *o = ops + idx;
    switch (CFG_TYPE_GET(o->val_type)) {
    case CFG_TYPE_INTEGER:
        strcpy(vtype, "INTEGER");
        break;
    case CFG_TYPE_STRING:
        strcpy(vtype, "STRING");
        break;
    case CFG_TYPE_NULL:
    default:
        strcpy(vtype, "FLAG");
        break;
    }
    optional = !(o->val_type & CFG_TYPE_MANDATORY);
    if (o->key != CFG_KEY_NULL) {
        sprintf(help, "  -%c, --%s [%s]%s\n    : %s", o->key, o->key_long,
                vtype, (optional) ? " (optional)" : "", o->desc);
    } else {
        sprintf(help, "  --%s [%s]%s\n    : %s", o->key_long,
                vtype, (optional) ? " (optional)" : "", o->desc);
    }
    return 0;
}

static int app_parse_cfg_file(FILE *fp, app_cfg_t *ops)
{
    char *parser;
    char line[4096] = "", tag[50] = "", val[4096] = "";
    int oidx;
    while (fgets(line, sizeof(line), fp)) {
        parser = strchr(line, '#');
        if (parser != NULL) {
            *parser = '\0';
        }
        parser = strtok(line, ": \t"); // find the parameter name
        if (parser == NULL) {
            continue;
        }
        strcpy(tag, parser);
        parser = strtok(NULL, ":"); // skip the colon
        parser = strtok(NULL, " \t\n");
        if (parser == NULL) {
            continue;
        }
        strcpy(val, parser);
        oidx = app_cfg_search_string(ops, tag);
        if (oidx < 0) {
            printf("\nError in configuration file: \"%s\" is not a valid command!\n", tag);
            return -1;
        }
        if (CFG_TYPE_GET(ops[oidx].val_type) !=
            CFG_TYPE_NULL) {
            if (app_cfg_read_value(ops + oidx, val)) {
                continue;
            }
        } else {
            *((int *)ops[oidx].val) = 1;
        }
        ops[oidx].flag = 1;
    }
    return 0;
}

static int app_parse_cmd(int argc, const char *argv[], app_cfg_t *ops, int *idx)
{
    int    aidx; /* arg index */
    int    oidx; /* option index */
    aidx = *idx + 1;
    if (aidx >= argc || argv[aidx] == NULL) {
        goto NO_MORE;
    }
    if (argv[aidx][0] != '-') {
        goto ERR;
    }
    if (argv[aidx][1] == '-') {
        /* long option */
        oidx = app_cfg_search_string(ops, argv[aidx] + 2);
        if (oidx < 0) {
            printf("\nError in command: \"%s\" is not a valid command!\n", argv[aidx]);
            goto ERR;
        }
    } else if (strlen(argv[aidx]) == 2) {
        /* short option */
        oidx = app_cfg_search_char(ops, argv[aidx][1]);
        if (oidx < 0) {
            printf("\nError in command: \"%s\" is not a valid command!\n", argv[aidx]);
            goto ERR;
        }
    } else {
        printf("\nError in command: \"%s\" is not a valid command!\n", argv[aidx]);
        goto ERR;
    }
    if (CFG_TYPE_GET(ops[oidx].val_type) != CFG_TYPE_NULL) {
        if (aidx + 1 >= argc) {
            printf("\nError in command: \"%s\" setting is incorrect!\n", argv[aidx]);
            goto ERR;
        }
        if (app_cfg_read_value(ops + oidx, argv[aidx + 1])) {
            printf("\nError in command: \"%s\" setting is incorrect!\n", argv[aidx]);
            goto ERR;
        }
        *idx = *idx + 1;
    } else {
        *((int *)ops[oidx].val) = 1;
    }
    ops[oidx].flag = 1;
    *idx = *idx + 1;
    return ops[oidx].key;
NO_MORE:
    return 0;
ERR:
    return -1;
}

static int app_parse_all(int argc, const char *argv[], app_cfg_t *ops)
{
    int i, ret = 0, idx = 0;
    app_cfg_t *o;
    const char *fname_cfg = NULL;
    FILE *fp;
    /* config file parsing */
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--config")) {
            if (i + 1 < argc) {
                fname_cfg = argv[i + 1];
                break;
            }
        }
    }
    if (fname_cfg) {
        fp = fopen(fname_cfg, "r");
        if (fp == NULL) {
            printf("\nError: Cannot open %s\n", fname_cfg);
            return -1;
        }
        if (app_parse_cfg_file(fp, ops)) {
            fclose(fp);
            return -1; /* config file error */
        }
        fclose(fp);
    }
    /* command line parsing */
    while (1) {
        ret = app_parse_cmd(argc, argv, ops, &idx);
        if (ret <= 0) {
            break;
        }
    }
    /* check mandatory argument */
    o = ops;
    while (o->key != 0) {
        if (o->val_type & CFG_TYPE_MANDATORY) {
            if (o->flag == 0) {
                /* not filled all mandatory argument */
                return o->key;
            }
        }
        o++;
    }
    return ret;
}

#endif /*_APP_CFG_H_ */
