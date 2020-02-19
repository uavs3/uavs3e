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
