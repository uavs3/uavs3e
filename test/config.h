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
    int   *flag;         /* flag to setting or not */
    void  *val;          
    char   desc[512];    /* description of option */
} app_cfg_t;

typedef enum _CONFIG_S {
    CONFIG_FNAME_CFG,
    CONFIG_FNAME_INP,
    CONFIG_FNAME_OUT,
    CONFIG_FNAME_REC,
    CONFIG_WIDTH_INP,
    CONFIG_HEIGHT_INP,
    CONFIG_QP,
    CONFIG_FPS_NUM,
    CONFIG_FPS_DEN,
    CONFIG_IPERIOD,
    CONFIG_MAX_FRM_NUM,
    CONFIG_USE_PIC_SIGN,
    CONFIG_VERBOSE,
    CONFIG_MAX_B_FRAMES,
    CONFIG_DISABLE_HGOP,
    CONFIG_CLOSE_GOP,
    CONFIG_OUT_BIT_DEPTH,
    CONFIG_IN_BIT_DEPTH,
    CONFIG_QP_OFFSET_CB,
    CONFIG_QP_OFFSET_CR,
    CONFIG_DELTA_QP,
    CONFIG_IPCM,
    CONFIG_AMVR,
    CONFIG_IPF,
    CONFIG_AFFINE,
    CONFIG_SMVD,
    CONFIG_DT_INTRA,
    CONFIG_PBT,
    CONFIG_DEBLOCK,
    CONFIG_SAO,
    CONFIG_ALF,
    CONFIG_WQ_ENABLE,
    CONFIG_SEQ_WQ_MODE,
    CONFIG_SEQ_WQ_FILE,
    CONFIG_PIC_WQ_DATA_IDX,
    CONFIG_PIC_WQ_FILE,
    CONFIG_WQ_PARAM,
    CONFIG_WQ_PARAM_MODEL,
    CONFIG_WQ_PARAM_DETAILED,
    CONFIG_WQ_PARAM_UNDETAILED,

    CONFIG_HMVP,
    CONFIG_TSCPM,
    CONFIG_UMVE,
    CONFIG_EMVR,
    CONFIG_SECTRANS,

    CONFIG_CROSS_PATCH_LOCONFIG_FILTER,
    CONFIG_PATCH_REF_COLOCATED,
    CONFIG_PATCH_WIDTH,
    CONFIG_PATCH_HEIGHT,
    CONFIG_PATCH_COLUMNS,
    CONFIG_PATCH_ROWS,
    CONFIG_COLUMN_WIDTH,
    CONFIG_ROW_HEIGHT,
    CONFIG_TEMP_SUB_RATIO,
    CONFIG_CTU_SZE,
    CONFIG_MIN_CU_SIZE,
    CONFIG_MAX_PART_RATIO,
    CONFIG_MAX_SPLIT_TIMES,
    CONFIG_MIN_QT_SIZE,
    CONFIG_MAX_BT_SIZE,
    CONFIG_MAX_EQT_SIZE,
    CONFIG_MAX_DT_SIZE,
    CONFIG_WPP_NUM,
    CONFIG_THD_FRM_NUM,
    CONFIG_RC_TYPE,
    CONFIG_RC_BITRATE,
    CONFIG_RC_MAX_BITRATE,
    CONFIG_RC_CRF,
    CONFIG_RC_MIN_QP,
    CONFIG_RC_MAX_QP,
    // ...
    CONFIG_MAX,
} CONFIG_S;

enc_cfg_t   cfg;
static char fn_cfg[256]                 = "\0"; 
static char fn_input[256]               = "\0";
static char fn_output[256]              = "\0";
static char fn_rec[256]                 = "\0"; 
static int  max_frames                  = 0;
static int  t_ds_ratio                  = 1;
static int  g_loglevel                  = LOG_LEVEL_1;
static int  cfg_flags[CONFIG_MAX]       = { 0 };

static app_cfg_t options[] = {
    {
        CFG_KEY_NULL, "config", CFG_TYPE_STRING,
        &cfg_flags[CONFIG_FNAME_CFG], fn_cfg,
        "file name of configuration"
    },
    {
        'i', "input", CFG_TYPE_STRING | CFG_TYPE_MANDATORY,
        &cfg_flags[CONFIG_FNAME_INP], fn_input,
        "file name of input video"
    },
    {
        'o', "output", CFG_TYPE_STRING,
        &cfg_flags[CONFIG_FNAME_OUT], fn_output,
        "file name of output bitstream"
    },
    {
        'r', "recon", CFG_TYPE_STRING,
        &cfg_flags[CONFIG_FNAME_REC], fn_rec,
        "file name of reconstructed video"
    },
    {
        'w',  "width", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg_flags[CONFIG_WIDTH_INP], &cfg.horizontal_size,
        "pixel width of input video"
    },
    {
        'h',  "height", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg_flags[CONFIG_HEIGHT_INP], &cfg.vertical_size,
        "pixel height of input video"
    },
    {
        'q',  "qp", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_QP], &cfg.qp,
        "QP value (0~63 for 8bit input, -16~63 for 10bit input)"
    },
    {
        'p',  "i_period", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_IPERIOD], &cfg.i_period,
        "I-picture period"
    },
    {
        'g',  "max_b_frames", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MAX_B_FRAMES], &cfg.max_b_frames,
        "Number of maximum B frames (1,3,7,15)\n"
    },
    {
        'f',  "frames", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MAX_FRM_NUM], &max_frames,
        "maximum number of frames to be encoded"
    },
    {
        's',  "signature", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_USE_PIC_SIGN], &cfg.use_pic_sign,
        "embed picture signature (HASH) for conformance checking in decoding"
        "\t 0: disable\n"
        "\t 1: enable\n"
    },
    {
        'v',  "verbose", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_VERBOSE], &g_loglevel,
        "verbose level\n"
        "\t 0: no message\n"
        "\t 1: frame-level messages (default)\n"
        "\t 2: all messages\n"
    },
    {
        'd',  "input_bit_depth", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_IN_BIT_DEPTH], &cfg.bit_depth_input,
        "input bitdepth (8(default), 10) "
    },
    {
        CFG_KEY_NULL,  "fps_num", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg_flags[CONFIG_FPS_NUM], &cfg.fps_num,
        "frame rate (Hz), numerator"
    },
    {
        CFG_KEY_NULL,  "fps_den", CFG_TYPE_INTEGER | CFG_TYPE_MANDATORY,
        &cfg_flags[CONFIG_FPS_DEN], &cfg.fps_den,
        "frame rate (Hz), denominator"
    },
    {
        CFG_KEY_NULL,  "internal_bit_depth", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_OUT_BIT_DEPTH], &cfg.bit_depth_internal,
        "output bitdepth (8, 10)(default: same as input bitdpeth) "
    },
    {
        CFG_KEY_NULL,  "close_gop", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_CLOSE_GOP], &cfg.close_gop,
        "use close gop"
    },
    {
        CFG_KEY_NULL,  "qp_offset_cb", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_QP_OFFSET_CB], &cfg.qp_offset_cb,
        "qp offset for cb, disable:0 (default)"
    },
    {
        CFG_KEY_NULL,  "qp_offset_cr", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_QP_OFFSET_CR], &cfg.qp_offset_cr,
        "qp offset for cr, disable:0 (default)"
    },
    {
        CFG_KEY_NULL,  "wpp_threads", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_WPP_NUM], &cfg.wpp_threads,
        "Number of threads for WPP"
    },
    {
        CFG_KEY_NULL,  "frm_threads", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_THD_FRM_NUM], &cfg.frm_threads,
        "Number of threads for Frame"
    },
    {
        CFG_KEY_NULL,  "lcu_delta_qp", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_DELTA_QP], &cfg.dqp_enable,
        "Random qp for lcu, on/off flag"
    },
    {
        CFG_KEY_NULL,  "amvr", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_AMVR], &cfg.amvr_enable,
        "amvr on/off flag"
    },
    {
        CFG_KEY_NULL,  "ipf", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_IPF], &cfg.ipf_flag,
        "Intra prediction filter on/off flag"
    },

    {
        CFG_KEY_NULL, "wq_enable", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_WQ_ENABLE], &cfg.wq_enable,
        "Weight Quant on/off, disable: 0(default), enable: 1"
    },
    {
        CFG_KEY_NULL, "seq_wq_mode", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_SEQ_WQ_MODE], &cfg.seq_wq_mode,
        "Seq Weight Quant (0: default, 1: User Define)"
    },
    {
        CFG_KEY_NULL, "seq_wq_user", CFG_TYPE_STRING,
        &cfg_flags[CONFIG_SEQ_WQ_FILE], &cfg.seq_wq_user,
        "User Defined Seq WQ Matrix"
    },
    {
        CFG_KEY_NULL, "pic_wq_data_idx", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_PIC_WQ_DATA_IDX], &cfg.pic_wq_data_idx,
        "Pic level WQ data index, 0(default):refer to seq_header,  1:derived by WQ parameter,  2:load from pichdr"
    },
    {
        CFG_KEY_NULL, "pic_wq_user", CFG_TYPE_STRING,
        &cfg_flags[CONFIG_PIC_WQ_FILE], &cfg.pic_wq_user,
        "User Defined Pic WQ Matrix"
    },
    {
        CFG_KEY_NULL, "wq_param", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_WQ_PARAM], &cfg.wq_param,
        "WQ Param (0=Default, 1=UnDetailed, 2=Detailed)"
    },
    {
        CFG_KEY_NULL, "wq_model", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_WQ_PARAM_MODEL], &cfg.wq_model,
        "WQ Model (0~2, default:0)"
    },
    {
        CFG_KEY_NULL, "wq_param_detailed", CFG_TYPE_STRING,
        &cfg_flags[CONFIG_WQ_PARAM_DETAILED], &cfg.wq_param_detailed,
        "default:[64,49,53,58,58,64]"
    },
    {
        CFG_KEY_NULL, "wq_param_undetailed", CFG_TYPE_STRING,
        &cfg_flags[CONFIG_WQ_PARAM_UNDETAILED], &cfg.wq_param_undetailed,
        "default:[67,71,71,80,80,106]"
    },
    {
        CFG_KEY_NULL,  "hmvp", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_HMVP], &cfg.num_of_hmvp,
        "number of hmvp skip candidates (default: 8, disable: 0)"
    },
    {
        CFG_KEY_NULL,  "tscpm", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_TSCPM], &cfg.tscpm_enable,
        "tscpm on/off flag"
    },
    {
        CFG_KEY_NULL,  "umve", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_UMVE], &cfg.umve_enable,
        "umve on/off flag"
    },
    {
        CFG_KEY_NULL,  "emvr", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_EMVR], &cfg.emvr_enable,
        "emvr on/off flag"
    },
    {
        CFG_KEY_NULL,  "affine", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_AFFINE], &cfg.affine_enable,
        "affine on/off flag"
    },
    {
        CFG_KEY_NULL,  "smvd", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_SMVD], &cfg.smvd_enable,
        "smvd on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        CFG_KEY_NULL,  "dt_intra", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_DT_INTRA], &cfg.dt_enable,
        "dt_intra on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        CFG_KEY_NULL,  "sectrans", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_SECTRANS], &cfg.sectrans_enable,
        "second transform (on: 1, off: 0, default: 1)"
    },
    {
        CFG_KEY_NULL,  "pbt", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_PBT], &cfg.pbt_enable,
        "pbt on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        CFG_KEY_NULL, "deblock", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_DEBLOCK], &cfg.use_deblock,
        "deblock on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        CFG_KEY_NULL, "sao", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_SAO], &cfg.sao_enable,
        "sao on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        CFG_KEY_NULL, "alf", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_ALF], &cfg.alf_enable,
        "alf on/off flag (on: 1, off: 0, default: 1)"
    },
    {
        CFG_KEY_NULL, "cross_patch_loopfilter", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_CROSS_PATCH_LOCONFIG_FILTER], &cfg.filter_cross_patch,
        "loop_filter_across_patch_flag(1:cross;0:non cross)"
    },
    {
        CFG_KEY_NULL, "colocated_patch", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_PATCH_REF_COLOCATED], &cfg.colocated_patch,
        "if the MV out of the patch boundary,0:non use colocated;1:use colocated"
    },
    {
        CFG_KEY_NULL, "patch_width", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_PATCH_WIDTH], &cfg.patch_width,
        "when uniform is 1,the nember of LCU in horizon in a patch"
    },
    {
        CFG_KEY_NULL, "patch_height", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_PATCH_HEIGHT], &cfg.patch_height,
        "when uniform is 1,the nember of LCU in vertical in a patch"
    },
    {
        CFG_KEY_NULL, "TemporalSubsampleRatio", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_TEMP_SUB_RATIO], &t_ds_ratio,
        "Subsampling Ratio (default: 8 for AI, 1 for RA and LD)"
    },
    {
        CFG_KEY_NULL, "ctu_size", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_CTU_SZE], &cfg.ctu_size,
        "ctu_size (default: 128; allowed values: 32, 64, 128)"
    },
    {
        CFG_KEY_NULL, "min_cu_size", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MIN_CU_SIZE], &cfg.min_cu_size,
        "min_cu_size (default: 4; allowed values: 4)"
    },
    {
        CFG_KEY_NULL, "max_part_ratio", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MAX_PART_RATIO], &cfg.max_part_ratio,
        "max_part_ratio (default:8; allowed values: 8)"
    },
    {
        CFG_KEY_NULL, "max_split_times", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MAX_SPLIT_TIMES], &cfg.max_split_times,
        "max_split_times (default: 6, allowed values: 6)"
    },
    {
        CFG_KEY_NULL, "min_qt_size", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MIN_QT_SIZE], &cfg.min_qt_size,
        "min_qt_size (default: 8, allowed values: 64, 128)"
    },
    {
        CFG_KEY_NULL, "max_bt_size", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MAX_BT_SIZE], &cfg.max_bt_size,
        "max_bt_size (default: 128, allowed values: 4, 8, 16, 32, 64, 128)"
    },
    {
        CFG_KEY_NULL, "max_eqt_size", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MAX_EQT_SIZE], &cfg.max_eqt_size,
        "max_eqt_size (default: 64, allowed values: 8, 16, 32, 64)"
    },
    {
        CFG_KEY_NULL, "max_dt_size", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_MAX_DT_SIZE], &cfg.max_dt_size,
        "max_dt_size (default: 64, allowed values: 16, 32, 64)"
    },
    {
        CFG_KEY_NULL, "rc_type", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_RC_TYPE], &cfg.rc_type,
        "rc_type (default: 0-CQP, allowed values: 1-CRF, 2-ABR, 3-CBR)"
    },
    {
        CFG_KEY_NULL, "rc_bitrate", CFG_TYPE_INTEGER,
        &cfg_flags[CONFIG_RC_BITRATE], &cfg.rc_bitrate,
        "rc_bitrate (kbps)"
     },
    {
       CFG_KEY_NULL, "rc_max_bitrate", CFG_TYPE_INTEGER,
       &cfg_flags[CONFIG_RC_MAX_BITRATE], &cfg.rc_max_bitrate,
       "rc_max_bitrate (kbps)"
    },
    {
       CFG_KEY_NULL, "rc_min_qp", CFG_TYPE_INTEGER,
       &cfg_flags[CONFIG_RC_MIN_QP], &cfg.rc_min_qp,
       "rc_min_qp (default: 0)"
    },
    {
       CFG_KEY_NULL, "rc_max_qp", CFG_TYPE_INTEGER,
       &cfg_flags[CONFIG_RC_MAX_QP], &cfg.rc_max_qp,
       "rc_max_qp (default: 63)"
    },
    {
       CFG_KEY_NULL, "rc_crf", CFG_TYPE_INTEGER,
       &cfg_flags[CONFIG_RC_CRF], &cfg.rc_crf,
       "crf value (0 - 63)"
    },
    {0, "", CFG_TYPE_NULL, NULL, NULL, ""} /* termination */
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
        *ops[oidx].flag = 1;
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
    *ops[oidx].flag = 1;
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
            if (*o->flag == 0) {
                /* not filled all mandatory argument */
                return o->key;
            }
        }
        o++;
    }
    return ret;
}

#endif /*_APP_CFG_H_ */
