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
