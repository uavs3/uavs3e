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

#ifndef _APP_CFG_H_
#define _APP_CFG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CFG_TYPE_MANDATORY       (1 <<0) /* mandatory or not   */
#define CFG_TYPE_NULL            (0 <<1) /* no value           */
#define CFG_TYPE_INTEGER         (10<<1) /* integer type value */
#define CFG_TYPE_STRING          (20<<1) /* string type value  */

#define CFG_TYPE_GET(x)  ((x) & ~CFG_TYPE_MANDATORY)
#define CFG_KEY_NULL                   (127)

typedef struct app_cfg_t {
    char   key;          /* option keyword */
    char   key_long[32]; /* option long keyword */
    int    val_type;    
    void  *val;          
    char   desc[512];    /* description of option */
    char   flag;
} app_cfg_t;

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
                printf("%s[-%c] is mandatory!\n", o->key_long, o->key);
                return -1;
            }
        }
        o++;
    }
    return ret;
}

#endif /*_APP_CFG_H_ */
