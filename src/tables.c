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

#include "tables.h"

tab_u8 tbl_sao_eo_offset_map[8] = { 3, 1, 0, 2, 4, 5, 6, 7 };

tab_s8 tbl_sao_bound_clip[NUM_SAO_OFFSET][3] = {
    //EO
    { -1, 6, 7 }, //low bound, upper bound, threshold
    { 0, 1, 1 },
    { 0, 0, 0 },
    { -1, 0, 1 },
    { -6, 1, 7 },
    { -7, 7, 7 } //BO
};

tab_u8 tbl_refi_bits[17][16] = {
    {0,                                                             },
    {0,                                                             },
    {1,  1,                                                         },
    {1,  2,  2,                                                     },
    {1,  2,  3,  3,                                                 },
    {1,  2,  3,  4,  4,                                             },
    {1,  2,  3,  4,  5,  5,                                         },
    {1,  2,  3,  4,  5,  6,  6,                                     },
    {1,  2,  3,  4,  5,  6,  7,  7,                                 },
    {1,  2,  3,  4,  5,  6,  7,  8,  8,                             },
    {1,  2,  3,  4,  5,  6,  7,  8,  9,  9,                         },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 10,                     },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 11,                 },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 12,             },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 13,         },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14,     },
    {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 15, },
};
