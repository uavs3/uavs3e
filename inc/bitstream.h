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
*    This product includes software developed by the <organization>.
* 4. Neither the name of the <organization> nor the
*    names of its contributors may be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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