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

#include "com_modules.h"

static tab_u8 ALPHA_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  2,  2,  2,
    2,  2,  2,  3,  3,  3,  3,  4,
    4,  4,  5,  5,  6,  6,  7,  7,
    8,  9,  10, 10, 11, 12, 13, 15,
    16, 17, 19, 21, 23, 25, 27, 29,
    32, 35, 38, 41, 45, 49, 54, 59
};
static tab_u8  BETA_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  2,  2,
    2,  2,  2,  2,  3,  3,  3,  3,
    4,  4,  5,  5,  5,  6,  6,  7,
    8,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22,
    23, 23, 24, 24, 25, 25, 26, 27
};


void com_df_clear_edge(int x, int y, int w, int h, u8 *map_edge, int i_scu)
{
    map_edge += (y >> MIN_CU_LOG2) * i_scu;

    for (int b4_y = (y >> MIN_CU_LOG2); b4_y < ((y + h) >> MIN_CU_LOG2); b4_y++) {
        memset(map_edge, 0, (x + w) >> MIN_CU_LOG2);
        map_edge += i_scu;
    }
}

#define pack16to32_mask2(x,y) (((x) << 16)|((y) & 0x7FFF))
#define CHECK_MVRANGE(pkt) (((pkt + pkt_mv_min) | (pkt_mv_max - pkt)) & 0x80004000)

static int avs3_always_inline skip_filter(com_map_t *map, com_ref_pic_t refp[MAX_REFS][REFP_NUM], int scup, int offset)
{
    com_scu_t MbP = map->map_scu[scup];
    com_scu_t MbQ = map->map_scu[scup + offset];
    com_pic_t *p_pic0, *p_pic1, *q_pic0, *q_pic1;
    com_scu_t mask = { 0 };
    const u32 pkt_mv_min = pack16to32_mask2(3, 3);
    const u32 pkt_mv_max = pkt_mv_min | 0x8000;

    mask.intra = 1;
    mask.cbf = 1;

    assert(sizeof(u8) == sizeof(com_scu_t));

    if ((*(u8*)&MbP | *(u8*)&MbQ) & *(u8*)&mask) {
        return 0;
    }

    s8 *refiP = map->map_refi[scup];
    s8 *refiQ = map->map_refi[scup + offset];

    if (M16(refiP) == M16(refiQ)) {
        s16(*mvP)[MV_D] = map->map_mv[scup];
        s16(*mvQ)[MV_D] = map->map_mv[scup + offset];

        if (REFI_IS_VALID(refiP[REFP_0])) {
            int mv_diff_pkt = ((M32(mvP[REFP_0]) | 0x8000) - (M32(mvQ[REFP_0]) & 0xFFFF7FFF)) & 0xFFFF7FFF;
            if (CHECK_MVRANGE(mv_diff_pkt)) {
                return 0;
            }
        }
        if (REFI_IS_VALID(refiP[REFP_1])) {
            int mv_diff_pkt = ((M32(mvP[REFP_1]) | 0x8000) - (M32(mvQ[REFP_1]) & 0xFFFF7FFF)) & 0xFFFF7FFF;
            if (CHECK_MVRANGE(mv_diff_pkt)) {
                return 0;
            }
        }
        return 1;
    }

    p_pic0 = REFI_IS_VALID(refiP[REFP_0]) ? refp[refiP[REFP_0]][REFP_0].pic : NULL;
    p_pic1 = REFI_IS_VALID(refiP[REFP_1]) ? refp[refiP[REFP_1]][REFP_1].pic : NULL;
    q_pic0 = REFI_IS_VALID(refiQ[REFP_0]) ? refp[refiQ[REFP_0]][REFP_0].pic : NULL;
    q_pic1 = REFI_IS_VALID(refiQ[REFP_1]) ? refp[refiQ[REFP_1]][REFP_1].pic : NULL;

    if (p_pic0 == q_pic1 && p_pic1 == q_pic0) {
        s16(*mvP)[MV_D] = map->map_mv[scup];
        s16(*mvQ)[MV_D] = map->map_mv[scup + offset];

        if (REFI_IS_VALID(refiP[REFP_0])) {
            int mv_diff_pkt = ((M32(mvP[REFP_0]) | 0x8000) - (M32(mvQ[REFP_1]) & 0xFFFF7FFF)) & 0xFFFF7FFF;
            if (CHECK_MVRANGE(mv_diff_pkt)) {
                return 0;
            }
        }
        if (REFI_IS_VALID(refiP[REFP_1])) {
            int mv_diff_pkt = ((M32(mvP[REFP_1]) | 0x8000) - (M32(mvQ[REFP_0]) & 0xFFFF7FFF)) & 0xFFFF7FFF;
            if (CHECK_MVRANGE(mv_diff_pkt)) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

static void avs3_always_inline com_df_set_edge_hor_inline(u8 *edge, com_map_t *map, int i_scu, com_ref_pic_t refp[MAX_REFS][REFP_NUM], int scup, int w, int edgecondition)
{
    edgecondition <<= 2;

    while (w--) {
        *edge++ |= skip_filter(map, refp, scup++, -i_scu) ? 0 : edgecondition;
    }
}

static void avs3_always_inline com_df_set_edge_ver_inline(u8 *edge, int i_edge, com_map_t *map, int i_scu, com_ref_pic_t refp[MAX_REFS][REFP_NUM], int scup, int h, int edgecondition)
{
    while (h--) {
        *edge |= skip_filter(map, refp, scup, -1) ? 0 : edgecondition;
        edge += i_edge;
        scup += i_scu;
    }
}

void com_df_set_edge(com_info_t *info, com_map_t *map, u8 *edge, int i_edge, com_ref_pic_t refp[MAX_REFS][REFP_NUM], int x, int y, int cuw, int cuh, int cud, int cup, BOOL b_recurse)
{
    s8 split_mode = NO_SPLIT;
    int lcu_idx;
    int edge_type = 0;

    lcu_idx = (x >> info->log2_max_cuwh) + (y >> info->log2_max_cuwh) * info->pic_width_in_lcu;

    if (b_recurse) {
        com_get_split_mode(&split_mode, cud, cup, cuw, cuh, info->max_cuwh, (map->map_split + lcu_idx * info->cus_in_lcu));
    }

    if (split_mode != NO_SPLIT) {
        com_split_struct_t split_struct;
        com_split_get_part_structure(split_mode, x, y, cuw, cuh, cup, cud, info->log2_max_cuwh - MIN_CU_LOG2, &split_struct);
        for (int part_num = 0; part_num < split_struct.part_count; ++part_num) {
            int cur_part_num = part_num;
            int sub_cuw = split_struct.width[cur_part_num];
            int sub_cuh = split_struct.height[cur_part_num];
            int x_pos = split_struct.x_pos[cur_part_num];
            int y_pos = split_struct.y_pos[cur_part_num];
            int offset = PEL2SCU(y_pos - y) * i_edge + PEL2SCU(x_pos - x);

            if (x_pos < info->pic_width && y_pos < info->pic_height) {
                com_df_set_edge(info, map, edge + offset, i_edge, refp, x_pos, y_pos, sub_cuw, sub_cuh, split_struct.cud, split_struct.cup[cur_part_num], b_recurse);
            }
        }
    } else {
        // luma tb filtering due to intra_DT and PBT
        int scu_x = PEL2SCU(x);
        int scu_y = PEL2SCU(y);
        int scu_w = PEL2SCU(cuw);
        int scu_h = PEL2SCU(cuh);
        int i_scu = info->i_scu;
        int scup = scu_y * i_scu + scu_x;
        part_size_t tb_part = map->map_scu[scup].tbpart;
        const int GRID_MASK = (LOOPFILTER_GRID >> MIN_CU_LOG2) - 1;
        s8 *patch_idx = map->map_patch;

#define SET_HOR_EDGE(dy,flag)       \
    if (((scu_y + dy) & GRID_MASK) == 0) {  \
        com_df_set_edge_hor_inline(edge + (dy) * i_edge, map, i_scu, refp, (scu_y + dy)* i_scu + scu_x, scu_w, flag); \
    }
#define SET_VER_EDGE(dx,flag)       \
    if (((scu_x + dx) & GRID_MASK) == 0) {  \
        com_df_set_edge_ver_inline(edge + (dx), i_edge, map, i_scu, refp, scu_y * i_scu + (scu_x + dx), scu_h, flag); \
    }
        switch (tb_part) {
        case SIZE_2NxhN:
            SET_HOR_EDGE(scu_h / 4, EDGE_TYPE_LUMA);
            SET_HOR_EDGE(scu_h / 2, EDGE_TYPE_LUMA);
            SET_HOR_EDGE(scu_h / 4 * 3, EDGE_TYPE_LUMA);
            break;
        case SIZE_hNx2N:
            SET_VER_EDGE(scu_w / 4, EDGE_TYPE_LUMA);
            SET_VER_EDGE(scu_w / 2, EDGE_TYPE_LUMA);
            SET_VER_EDGE(scu_w / 4 * 3, EDGE_TYPE_LUMA);
            break;
        case SIZE_NxN:
            SET_HOR_EDGE(scu_h / 2, EDGE_TYPE_LUMA);
            SET_VER_EDGE(scu_w / 2, EDGE_TYPE_LUMA);
            break;
        default:
            break;
        }
        if (scu_y && (scu_y & GRID_MASK) == 0 && (info->sqh.filter_cross_patch || patch_idx[scup] == patch_idx[scup - i_scu])) {
            SET_HOR_EDGE(0, EDGE_TYPE_ALL);  // UP
        }
        if (scu_x && (scu_x & GRID_MASK) == 0 && (info->sqh.filter_cross_patch || patch_idx[scup] == patch_idx[scup - 1])) {
            SET_VER_EDGE(0, EDGE_TYPE_ALL);  // LEFT
        }
    }
}

static void avs3_always_inline com_df_cal_y_param(com_pic_header_t *pichdr, int QPP, int QPQ, int QPOFFS, int shift, int *alpha, int *beta)
{
    int QP = (QPP + QPQ + 1 - QPOFFS - QPOFFS) >> 1;
    *alpha = ALPHA_TABLE[COM_CLIP3(0, MAX_QUANT_BASE, QP + pichdr->alpha_c_offset)] << shift;
    *beta  = BETA_TABLE [COM_CLIP3(0, MAX_QUANT_BASE, QP + pichdr->beta_offset   )] << shift;
}

static void avs3_always_inline com_df_cal_c_param(com_pic_header_t *pichdr, int QPP, int QPQ, int QPOFFS, int shift, int *alpha, int *beta, int delta)
{
    int QP;
    int c_p_QPuv = QPP + delta - QPOFFS;
    int c_q_QPuv = QPQ + delta - QPOFFS;

    if (c_p_QPuv >= 0) {
        c_p_QPuv = com_tbl_qp_chroma_ajudst[COM_MIN(MAX_QUANT_BASE, c_p_QPuv)];
    }
    if (c_q_QPuv >= 0) {
        c_q_QPuv = com_tbl_qp_chroma_ajudst[COM_MIN(MAX_QUANT_BASE, c_q_QPuv)];
    }
    c_p_QPuv = COM_MAX(0, c_p_QPuv);
    c_q_QPuv = COM_MAX(0, c_q_QPuv);

    QP = (c_p_QPuv + c_q_QPuv + 1) >> 1;

    *alpha = ALPHA_TABLE[COM_CLIP3(0, MAX_QUANT_BASE, QP + pichdr->alpha_c_offset)] << shift;
    *beta = BETA_TABLE[COM_CLIP3(0, MAX_QUANT_BASE, QP + pichdr->beta_offset)] << shift;
}

void com_df_lcu(com_info_t *info, com_pic_header_t *pichdr, com_map_t *map, com_pic_t *pic, int lcu_x, int lcu_y)
{
    int i_scu = info->i_scu;
    int i_lcu = info->pic_width_in_lcu;
    int log2_lcu = info->log2_max_cuwh;
    com_seqh_t *seqh = &info->sqh;
    int lcu_size = info->max_cuwh;
    int pix_y;
    int s_l = pic->stride_luma;
    int s_c = pic->stride_chroma;
    int shift = info->bit_depth_internal - 8;
    int alpha_hor_y, beta_hor_y, alpha_ver_y, beta_ver_y;
    int alpha_hor_u, beta_hor_u, alpha_ver_u, beta_ver_u;
    int alpha_hor_v, beta_hor_v, alpha_ver_v, beta_ver_v;
    int start_x = lcu_x * lcu_size;
    int start_y = lcu_y * lcu_size;
    int end_x = COM_MIN(start_x + lcu_size, info->pic_width);
    int end_y = COM_MIN(start_y + lcu_size, info->pic_height);
    pel *src_l = pic->y + start_y * s_l;
    pel *src_u = pic->u + start_y / 2 * s_c;
    pel *src_v = pic->v + start_y / 2 * s_c;
    int QPOFFS = info->qp_offset_bit_depth;
    u8 *pQP = &map->map_qp[(start_y >> info->log2_max_cuwh) * info->pic_width_in_lcu + (start_x >> info->log2_max_cuwh)];
    int QP         = pQP[0];
    int QP_left    = pQP[-1];
    int QP_top     = pQP[-info->pic_width_in_lcu];
    int QP_topleft = pQP[-info->pic_width_in_lcu - 1];
    int edge;

    // luma deblock
    for (pix_y = start_y; pix_y < end_y; pix_y += LOOPFILTER_GRID, src_l += s_l * LOOPFILTER_GRID) {
        int pix_x = start_x;
        u8 *edge_flag = map->map_edge + (pix_y >> MIN_CU_LOG2) * i_scu + (pix_x >> MIN_CU_LOG2);

        if (pix_x) {
            if (pix_y == start_y) {
                com_df_cal_y_param(pichdr, QP_left, QP_topleft, QPOFFS, shift, &alpha_hor_y, &beta_hor_y);
            } else {
                com_df_cal_y_param(pichdr, QP_left, QP_left, QPOFFS, shift, &alpha_hor_y, &beta_hor_y);
            }
            com_df_cal_y_param(pichdr, QP, QP_left,    QPOFFS, shift, &alpha_ver_y, &beta_ver_y);

            edge = ((edge_flag[0]) | ((edge_flag[i_scu]) << 8)) & 0x0303;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[0](src_l + pix_x, s_l, alpha_ver_y, beta_ver_y, edge);
            }
            edge = (edge_flag[-1] >> 2) & 0x03;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[1](src_l + pix_x - MIN_CU_SIZE, s_l, alpha_hor_y, beta_hor_y, edge);
            }
        }
        pix_x += LOOPFILTER_GRID;
        edge_flag += 2;

        if (pix_y == start_y) {
            com_df_cal_y_param(pichdr, QP, QP_top, QPOFFS, shift, &alpha_hor_y, &beta_hor_y);
        } else {
            com_df_cal_y_param(pichdr, QP, QP, QPOFFS, shift, &alpha_hor_y, &beta_hor_y);
        }

        com_df_cal_y_param(pichdr, QP, QP, QPOFFS, shift, &alpha_ver_y, &beta_ver_y);

        for (; pix_x < end_x; pix_x += LOOPFILTER_GRID, edge_flag += 2) {
            edge = ((edge_flag[0]) | ((edge_flag[i_scu]) << 8)) & 0x0303;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[0](src_l + pix_x, s_l, alpha_ver_y, beta_ver_y, edge);
            }
            edge = (M16(edge_flag - 2) >> 2) & 0x0303;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[1](src_l + pix_x - LOOPFILTER_GRID, s_l, alpha_hor_y, beta_hor_y, edge);
            }
        }
        edge = (M16(edge_flag - 2) >> 2) & (end_x == info->pic_width ? 0x0303 : 0x03);
        if (edge) {
            uavs3e_funs_handle.deblock_luma[1](src_l + pix_x - LOOPFILTER_GRID, s_l, alpha_hor_y, beta_hor_y, edge);
        }
    }

    // chroma deblock
    start_x >>= 1;
    end_x   >>= 1;
    start_y >>= 1;
    end_y   >>= 1;

    for (pix_y = start_y; pix_y < end_y; pix_y += LOOPFILTER_GRID) {
        int pix_x = start_x;
        u8 *edge_flag = map->map_edge + (pix_y >> (MIN_CU_LOG2 - 1)) * i_scu + (pix_x >> (MIN_CU_LOG2 - 1));

        if (pix_x) {
            if (pix_y == start_y) {
                com_df_cal_c_param(pichdr, QP_left, QP_topleft, QPOFFS, shift, &alpha_hor_u, &beta_hor_u, pichdr->chroma_quant_param_delta_cb);
                com_df_cal_c_param(pichdr, QP_left, QP_topleft, QPOFFS, shift, &alpha_hor_v, &beta_hor_v, pichdr->chroma_quant_param_delta_cr);
            } else {
                com_df_cal_c_param(pichdr, QP_left, QP_left, QPOFFS, shift, &alpha_hor_u, &beta_hor_u, pichdr->chroma_quant_param_delta_cb);
                com_df_cal_c_param(pichdr, QP_left, QP_left, QPOFFS, shift, &alpha_hor_v, &beta_hor_v, pichdr->chroma_quant_param_delta_cr);
            }
            com_df_cal_c_param(pichdr, QP, QP_left, QPOFFS, shift, &alpha_ver_u, &beta_ver_u, pichdr->chroma_quant_param_delta_cb);
            com_df_cal_c_param(pichdr, QP, QP_left, QPOFFS, shift, &alpha_ver_v, &beta_ver_v, pichdr->chroma_quant_param_delta_cr);

            edge = ((edge_flag[0]) | ((edge_flag[i_scu]) << 8)) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[0](src_u + pix_x, src_v + pix_x, s_c, alpha_ver_u, beta_ver_u, alpha_ver_v, beta_ver_v, edge);
            }
            edge = ((edge_flag[i_scu * 2]) | ((edge_flag[i_scu * 3]) << 8)) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[0](src_u + pix_x + s_c * MIN_CU_SIZE, src_v + pix_x + s_c * MIN_CU_SIZE, s_c, alpha_ver_u, beta_ver_u, alpha_ver_v, beta_ver_v, edge);
            }
            edge = (M16(edge_flag - 2) >> 2) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[1](src_u + pix_x - MIN_CU_SIZE, src_v + pix_x - MIN_CU_SIZE, s_c, alpha_hor_u, beta_hor_u, alpha_hor_v, beta_hor_v, edge);
            }
        }
        pix_x += LOOPFILTER_GRID;
        edge_flag += 4;

        if (pix_y == start_y) {
            com_df_cal_c_param(pichdr, QP, QP_top, QPOFFS, shift, &alpha_hor_u, &beta_hor_u, pichdr->chroma_quant_param_delta_cb);
            com_df_cal_c_param(pichdr, QP, QP_top, QPOFFS, shift, &alpha_hor_v, &beta_hor_v, pichdr->chroma_quant_param_delta_cr);
        } else {
            com_df_cal_c_param(pichdr, QP, QP, QPOFFS, shift, &alpha_hor_u, &beta_hor_u, pichdr->chroma_quant_param_delta_cb);
            com_df_cal_c_param(pichdr, QP, QP, QPOFFS, shift, &alpha_hor_v, &beta_hor_v, pichdr->chroma_quant_param_delta_cr);
        }

        com_df_cal_c_param(pichdr, QP, QP, QPOFFS, shift, &alpha_ver_u, &beta_ver_u, pichdr->chroma_quant_param_delta_cb);
        com_df_cal_c_param(pichdr, QP, QP, QPOFFS, shift, &alpha_ver_v, &beta_ver_v, pichdr->chroma_quant_param_delta_cr);

        for (; pix_x < end_x; pix_x += LOOPFILTER_GRID) {
            edge = ((edge_flag[0]) | ((edge_flag[i_scu]) << 8)) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[0](src_u + pix_x, src_v + pix_x, s_c, alpha_ver_u, beta_ver_u, alpha_ver_v, beta_ver_v, edge);
            }
            edge = ((edge_flag[i_scu * 2]) | ((edge_flag[i_scu * 3]) << 8)) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[0](src_u + pix_x + s_c * MIN_CU_SIZE, src_v + pix_x + s_c * MIN_CU_SIZE, s_c, alpha_ver_u, beta_ver_u, alpha_ver_v, beta_ver_v, edge);
            }
            edge = (M16(edge_flag - 4) >> 2) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[1](src_u + pix_x - LOOPFILTER_GRID, src_v + pix_x - LOOPFILTER_GRID, s_c, alpha_hor_u, beta_hor_u, alpha_hor_v, beta_hor_v, edge);
            }
            edge = (M16(edge_flag - 2) >> 2) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[1](src_u + pix_x - MIN_CU_SIZE, src_v + pix_x - MIN_CU_SIZE, s_c, alpha_hor_u, beta_hor_u, alpha_hor_v, beta_hor_v, edge);
            }
            edge_flag += 4;
        }
        edge = (M16(edge_flag - 4) >> 2) & 0x0202;
        if (edge) {
            uavs3e_funs_handle.deblock_chroma[1](src_u + pix_x - LOOPFILTER_GRID, src_v + pix_x - LOOPFILTER_GRID, s_c, alpha_hor_u, beta_hor_u, alpha_hor_v, beta_hor_v, edge);
        }
        if (end_x == info->pic_width / 2) {
            edge = (M16(edge_flag - 2) >> 2) & 0x0202;
            if (edge) {
                uavs3e_funs_handle.deblock_chroma[1](src_u + pix_x - MIN_CU_SIZE, src_v + pix_x - MIN_CU_SIZE, s_c, alpha_hor_u, beta_hor_u, alpha_hor_v, beta_hor_v, edge);
            }
        }
        src_u += s_c * LOOPFILTER_GRID;
        src_v += s_c * LOOPFILTER_GRID;
    }
}

void com_df_rdo_luma(com_info_t *info, com_pic_header_t *pichdr, com_map_t *map, u8 *edge, int i_edge, pel *src, int i_src, int pix_x, int pix_y, int cuw, int cuh)
{
    int i_scu = info->i_scu;
    com_seqh_t *seqh = &info->sqh;
    int shift = info->bit_depth_internal - 8;
    int alpha_hor_y, beta_hor_y, alpha_ver_y, beta_ver_y;
    int QPOFFS  = info->qp_offset_bit_depth;
    pel *p = src;

    // todo: support Delta QP
    int QP = map->map_qp[(pix_y >> info->log2_max_cuwh) * info->pic_width_in_lcu + (pix_x >> info->log2_max_cuwh)];
    com_df_cal_y_param(pichdr, QP, QP, QPOFFS, shift, &alpha_hor_y, &beta_hor_y);
    com_df_cal_y_param(pichdr, QP, QP, QPOFFS, shift, &alpha_ver_y, &beta_ver_y);

    for (pix_y = 0; pix_y < cuh - LOOPFILTER_GRID + 1; pix_y += LOOPFILTER_GRID) {
        u8 *edge_flag = edge + PEL2SCU(pix_y) * i_edge;

        for (pix_x = 0; pix_x < cuw; pix_x += MIN_CU_SIZE) {
            int edge = ((edge_flag[0]) | ((edge_flag[i_edge]) << 8)) & 0x0303;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[0](p + pix_x, i_src, alpha_ver_y, beta_ver_y, edge);
            }
            edge_flag++;
        }
        p += i_src * LOOPFILTER_GRID;
    }
    if (pix_y < cuh) {
        u8 *edge_flag = edge + PEL2SCU(pix_y) * i_edge;
        for (pix_x = 0; pix_x < cuw; pix_x += MIN_CU_SIZE) {
            int edge = edge_flag[0] & 0x03;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[0](p + pix_x, i_src, alpha_ver_y, beta_ver_y, edge);
            }
            edge_flag++;
        }
    }
    p = src;

    for (pix_y = 0; pix_y < cuh; pix_y += MIN_CU_SIZE) {
        u8 *edge_flag = edge + PEL2SCU(pix_y) * i_edge;
        for (pix_x = 0; pix_x < cuw - LOOPFILTER_GRID + 1; pix_x += LOOPFILTER_GRID) {
            int edge = (M16(edge_flag) >> 2) & 0x0303;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[1](p + pix_x, i_src, alpha_hor_y, beta_hor_y, edge);
            }
            edge_flag += 2;
        }
        if (pix_x < cuw) {
            int edge = (*edge_flag >> 2) & 0x03;
            if (edge) {
                uavs3e_funs_handle.deblock_luma[1](p + pix_x, i_src, alpha_hor_y, beta_hor_y, edge);
            }
        }
        p += i_src * MIN_CU_SIZE;
    }
}

static void deblock_edge_luma_ver(pel *src, int stride, int Alpha, int Beta, int edge_flag)
{
    int i, line_size = 8;

    if ((edge_flag & 0x0101) != 0x0101) {
        line_size = 4;
    }
    if (!(edge_flag & 0x1)) {
        src += 4 * stride;
    }

    for (i = 0; i < line_size; i++, src += stride) {
        int L3 = src[-4];
        int L2 = src[-3];
        int L1 = src[-2];
        int L0 = src[-1];
        int R0 = src[ 0];
        int R1 = src[ 1];
        int R2 = src[ 2];
        int R3 = src[ 3];
        int AbsDelta = COM_ABS(R0 - L0);
        int FlatnessL = (COM_ABS(L1 - L0) < Beta) ? 2 : 0;
        int FlatnessR = (COM_ABS(R0 - R1) < Beta) ? 2 : 0;
        int fs;

        if (COM_ABS(L2 - L0) < Beta) {
            FlatnessL++;
        }
        if (COM_ABS(R0 - R2) < Beta) {
            FlatnessR++;
        }

        switch (FlatnessL + FlatnessR) {
        case 6:
            fs = (COM_ABS(R0 - R1) <= Beta / 4 && COM_ABS(L0 - L1) <= Beta / 4 && AbsDelta < Alpha) ? 4 : 3;
            break;
        case 5:
            fs = (R1 == R0 && L0 == L1) ? 3 : 2;
            break;
        case 4:
            fs = (FlatnessL == 2) ? 2 : 1;
            break;
        case 3:
            fs = (COM_ABS(L1 - R1) < Beta) ? 1 : 0;
            break;
        default:
            fs = 0;
            break;
        }

        switch (fs) {
        case 4:
            src[-3] = (pel)((((L3 + L2 + L1) << 1) + L0 + R0 + 4) >> 3);                          //L2
            src[-2] = (pel)((((L2 + L0 + L1) << 2) + L1 + R0 * 3 + 8) >> 4);                      //L1
            src[-1] = (pel)(((L2 + R1) * 3 + ((L1 + L0 + R0) << 3) + (L0 << 1) + 16) >> 5);       //L0
            src[ 0] = (pel)(((R2 + L1) * 3 + ((R1 + R0 + L0) << 3) + (R0 << 1) + 16) >> 5);       //R0
            src[ 1] = (pel)((((R2 + R1 + R0) << 2) + R1 + L0 * 3 + 8) >> 4);                      //R1
            src[ 2] = (pel)((((R3 + R2 + R1) << 1) + R0 + L0 + 4) >> 3);                          //R2
            break;
        case 3:
            src[-2] = (pel)((L2 * 3 + L1 * 8 + L0 * 4 + R0 + 8) >> 4);                           //L1
            src[-1] = (pel)((L2 + (L1 << 2) + (L0 << 2) + (L0 << 1) + (R0 << 2) + R1 + 8) >> 4); //L0
            src[ 0] = (pel)((L1 + (L0 << 2) + (R0 << 2) + (R0 << 1) + (R1 << 2) + R2 + 8) >> 4); //R0
            src[ 1] = (pel)((R2 * 3 + R1 * 8 + R0 * 4 + L0 + 8) >> 4);                           //R1
            break;
        case 2:
            src[-1] = (pel)(((L1 << 1) + L1 + (L0 << 3) + (L0 << 1) + (R0 << 1) + R0 + 8) >> 4);
            src[ 0] = (pel)(((L0 << 1) + L0 + (R0 << 3) + (R0 << 1) + (R1 << 1) + R1 + 8) >> 4);
            break;
        case 1:
            src[-1] = (pel)((L0 * 3 + R0 + 2) >> 2);
            src[ 0] = (pel)((R0 * 3 + L0 + 2) >> 2);
            break;
        default:
            break;
        }
    }
}

static void deblock_edge_luma_hor(pel *src, int stride, int Alpha, int Beta, int edge_flag)
{
    int i, line_size = 8;
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int inc4 = inc + inc3;

    if ((edge_flag & 0x0101) != 0x0101) {
        line_size = 4;
    }
    if (!(edge_flag & 0x1)) {
        src += 4;
    }

    for (i = 0; i < line_size; i++, src++) {
        int L3 = src[-inc4];
        int L2 = src[-inc3];
        int L1 = src[-inc2];
        int L0 = src[-inc];
        int R0 = src[0];
        int R1 = src[inc];
        int R2 = src[inc2];
        int R3 = src[inc3];
        int AbsDelta = COM_ABS(R0 - L0);
        int FlatnessL = (COM_ABS(L1 - L0) < Beta) ? 2 : 0;
        int FlatnessR = (COM_ABS(R0 - R1) < Beta) ? 2 : 0;
        int fs;

        if (COM_ABS(L2 - L0) < Beta) {
            FlatnessL++;
        }
        if (COM_ABS(R0 - R2) < Beta) {
            FlatnessR++;
        }

        switch (FlatnessL + FlatnessR) {
        case 6:
            fs = (COM_ABS(R0 - R1) <= Beta / 4 && COM_ABS(L0 - L1) <= Beta / 4 && AbsDelta < Alpha) ? 4 : 3;
            break;
        case 5:
            fs = (R1 == R0 && L0 == L1) ? 3 : 2;
            break;
        case 4:
            fs = (FlatnessL == 2) ? 2 : 1;
            break;
        case 3:
            fs = (COM_ABS(L1 - R1) < Beta) ? 1 : 0;
            break;
        default:
            fs = 0;
            break;
        }

        switch (fs) {
        case 4:
            src[-inc3] = (pel)((((L3 + L2 + L1) << 1) + L0 + R0 + 4) >> 3);                         //L2
            src[-inc2] = (pel)((((L2 + L0 + L1) << 2) + L1 + R0 * 3 + 8) >> 4);                     //L1
            src[-inc ] = (pel)(((L2 + R1) * 3 + ((L1 + L0 + R0) << 3) + (L0 << 1) + 16) >> 5);      //L0
            src[ 0   ] = (pel)(((R2 + L1) * 3 + ((R1 + R0 + L0) << 3) + (R0 << 1) + 16) >> 5);      //R0
            src[ inc ] = (pel)((((R2 + R1 + R0) << 2) + R1 + L0 * 3 + 8) >> 4);                     //R1
            src[ inc2] = (pel)((((R3 + R2 + R1) << 1) + R0 + L0 + 4) >> 3);                         //R2
            break;
        case 3:
            src[-inc2] = (pel)((L2 * 3 + L1 * 8 + L0 * 4 + R0 + 8) >> 4);                           //L1
            src[-inc ] = (pel)((L2 + (L1 << 2) + (L0 << 2) + (L0 << 1) + (R0 << 2) + R1 + 8) >> 4); //L0
            src[ 0   ] = (pel)((L1 + (L0 << 2) + (R0 << 2) + (R0 << 1) + (R1 << 2) + R2 + 8) >> 4); //R0
            src[ inc ] = (pel)((R2 * 3 + R1 * 8 + R0 * 4 + L0 + 8) >> 4);                           //R2
            break;
        case 2:
            src[-inc] = (pel)(((L1 << 1) + L1 + (L0 << 3) + (L0 << 1) + (R0 << 1) + R0 + 8) >> 4);
            src[0] = (pel)(((L0 << 1) + L0 + (R0 << 3) + (R0 << 1) + (R1 << 1) + R1 + 8) >> 4);
            break;
        case 1:
            src[-inc] = (pel)((L0 * 3 + R0 + 2) >> 2);
            src[0] = (pel)((R0 * 3 + L0 + 2) >> 2);
            break;
        default:
            break;
        }
    }
}

static void deblock_edge_chro_ver(pel *srcu, pel *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int edge_flag)
{
    int i, line_size = 4;
    int alpha[2] = { alpha_u, alpha_v };
    int beta[2] = { beta_u, beta_v };
    pel *p_src;
    int uv;

    if ((edge_flag & 0x0202) != 0x0202) {
        line_size = 2;
    }
    if (!(edge_flag & 0x2)) {
        srcu += stride << 1;
        srcv += stride << 1;
    }

    for (uv = 0; uv < 2; uv++) {
        p_src = uv ? srcv : srcu;
        for (i = 0; i < line_size; i++, p_src += stride) {
            int L2 = p_src[-3];
            int L1 = p_src[-2];
            int L0 = p_src[-1];
            int R0 = p_src[0];
            int R1 = p_src[1];
            int R2 = p_src[2];
            int delta_m = COM_ABS(R0 - L0);
            int delta_l = COM_ABS(L1 - L0);
            int delta_r = COM_ABS(R0 - R1);

            if ((delta_l < beta[uv]) && (delta_r < beta[uv])) {
                p_src[-1] = (pel)((L1 * 3 + L0 * 10 + R0 * 3 + 8) >> 4);                  // L0
                p_src[0] = (pel)((R1 * 3 + R0 * 10 + L0 * 3 + 8) >> 4);                  // R0
                if ((COM_ABS(L2 - L0) < beta[uv]) && (COM_ABS(R2 - R0) < beta[uv]) &&
                    delta_r <= beta[uv] / 4 && delta_l <= beta[uv] / 4 && delta_m < alpha[uv]) {
                    p_src[-2] = (pel)((L2 * 3 + L1 * 8 + L0 * 3 + R0 * 2 + 8) >> 4);      // L1
                    p_src[1] = (pel)((R2 * 3 + R1 * 8 + R0 * 3 + L0 * 2 + 8) >> 4);      // R1
                }
            }
        }
    }
}

static void deblock_edge_chro_hor(pel *srcu, pel *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int edge_flag)
{
    int i, line_size = 4;
    int inc = stride;
    int inc2 = inc << 1;
    int inc3 = inc + inc2;
    int alpha[2] = { alpha_u, alpha_v };
    int beta[2] = { beta_u, beta_v };
    pel *p_src;
    int uv;

    if ((edge_flag & 0x0202) != 0x0202) {
        line_size = 2;
    }
    if (!(edge_flag & 0x2)) {
        srcu += 2;
        srcv += 2;
    }

    for (uv = 0; uv < 2; uv++) {
        p_src = uv ? srcv : srcu;
        for (i = 0; i < line_size; i++, p_src += 1) {
            int L2 = p_src[-inc3];
            int L1 = p_src[-inc2];
            int L0 = p_src[-inc];
            int R0 = p_src[0];
            int R1 = p_src[inc];
            int R2 = p_src[inc2];
            int delta_m = COM_ABS(R0 - L0);
            int delta_l = COM_ABS(L1 - L0);
            int delta_r = COM_ABS(R0 - R1);

            if ((delta_l < beta[uv]) && (delta_r < beta[uv])) {
                p_src[-inc] = (pel)((L1 * 3 + L0 * 10 + R0 * 3 + 8) >> 4);                  // L0
                p_src[0] = (pel)((R1 * 3 + R0 * 10 + L0 * 3 + 8) >> 4);                     // R0
                if ((COM_ABS(L2 - L0) < beta[uv]) && (COM_ABS(R2 - R0) < beta[uv]) &&
                    delta_r <= beta[uv] / 4 && delta_l <= beta[uv] / 4 && delta_m < alpha[uv]) {
                    p_src[-inc2] = (pel)((L2 * 3 + L1 * 8 + L0 * 3 + R0 * 2 + 8) >> 4);     // L1
                    p_src[inc] = (pel)((R2 * 3 + R1 * 8 + R0 * 3 + L0 * 2 + 8) >> 4);       // R1
                }
            }
        }
    }
}

void uavs3e_funs_init_deblock_c()
{
    uavs3e_funs_handle.deblock_luma[0] = deblock_edge_luma_ver;
    uavs3e_funs_handle.deblock_luma[1] = deblock_edge_luma_hor;
    uavs3e_funs_handle.deblock_chroma[0] = deblock_edge_chro_ver;
    uavs3e_funs_handle.deblock_chroma[1] = deblock_edge_chro_hor;
}