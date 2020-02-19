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

#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

#include "com_system.h"

typedef struct uavs3e_bs_t {
    u32              code;       /* 32-bits buffer */
    int              leftbits;   /* bits left in 32-bits buffer */
    u8              *cur;        /* writing position */
    u8              *end;        /* buffer end */
    u8              *start;      /* buffer begin */
    u8              *buftmp;     /* buffer for demulation */
} bs_t;

#define BS_GET_BYTES(bs)    (int)((bs)->cur - (bs)->start)

void bs_init    (bs_t *bs, u8 *buf, u8 *buftmp, int size);
int  bs_flush   (bs_t *bs);
int  bs_write1  (bs_t *bs, int val);
int  bs_write   (bs_t *bs, u32 val, int len);
void bs_write_ue(bs_t *bs, u32 val);
void bs_write_se(bs_t *bs, int val);


#endif /* _BITSTREAM_H_ */