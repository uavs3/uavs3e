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

#include "define.h"

int bs_flush(bs_t *bs)
{
    int bytes = ((32 - bs->leftbits + 7) >> 3);
    
    while (bytes--) {
        *bs->cur++ = (bs->code >> 24) & 0xFF;
        bs->code <<= 8;
    }
    bs->leftbits = 32;

    return 0;
}

void bs_init(bs_t *bs, u8 *buf, u8 *buftmp, int size)
{
    bs->start     = buf;
    bs->cur       = buf;
    bs->buftmp    = buftmp;
    bs->end       = buf + size - 1;
    bs->code      = 0;
    bs->leftbits  = 32;
}

int bs_write1(bs_t *bs, int val)
{
    bs->leftbits--;
    bs->code |= ((val & 0x1) << bs->leftbits);
    if (bs->leftbits == 0) {
        assert(bs->cur <= bs->end);
        bs_flush(bs);
        bs->code = 0;
        bs->leftbits = 32;
    }
    return 0;
}

int bs_write(bs_t *bs, u32 val, int len)  /* len(1 ~ 32) */
{
    int leftbits;

    leftbits = bs->leftbits;
    val <<= (32 - len);
    bs->code |= (val >> (32 - leftbits));
    if (len < leftbits) {
        bs->leftbits -= len;
    } else {
        assert(bs->cur + 4 <= bs->end);
        bs->leftbits = 0;
        bs_flush(bs);
        bs->code = (val << leftbits);
        bs->leftbits = 32 - (len - leftbits);
    }
    return 0;
}

void bs_write_ue(bs_t *bs, u32 val)
{
    int   len_i, len_c, info, nn;
    u32  code;
    nn = ((val + 1) >> 1);
    for (len_i = 0; len_i < 16 && nn != 0; len_i++) {
        nn >>= 1;
    }
    info = val + 1 - (1 << len_i);
    code = (1 << len_i) | ((info) & ((1 << len_i) - 1));
    len_c = (len_i << 1) + 1;
    bs_write(bs, code, len_c);
}

void bs_write_se(bs_t *bs, int val)
{
    bs_write_ue(bs, val <= 0 ? (-val * 2) : (val * 2 - 1));
}
