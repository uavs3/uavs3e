#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "commonVariables.h"
#include "commonStructures.h"
#include "loop-filter.h"

tab_i32s_t tab_saoclip[NUM_SAO_OFFSET][3] = {
    { -1, 6, 7}, 
    {  0, 1, 1},
    {  0, 0, 0},
    { -1, 0, 1},
    { -6, 1, 7},
    { -7, 7, 7} 
};

void cpy_sao_param_blk(SAOBlkParam *saopara_dst, SAOBlkParam *saopara_src)
{
    memcpy(saopara_dst, saopara_src, sizeof(SAOBlkParam)* NUM_SAO_COMPONENTS);
}

void getMergeNeighbor(avs3_enc_t *h, int lcu_x, int lcu_y, int input_MaxsizeInBit, 
    SAOBlkParam(*rec_saoBlkParam)[NUM_SAO_COMPONENTS], int *MergeAvail, 
    SAOBlkParam sao_merge_param[][NUM_SAO_COMPONENTS])
{
    const cfg_param_t *input = h->input;
    int mb_y = lcu_y << (LCU_SIZE_IN_BITS - 3);
    int mb_x = lcu_x << (LCU_SIZE_IN_BITS - 3);
    int lcu_idx = lcu_y * input->pic_width_in_lcu + lcu_x;
    int pic_mb_width = input->img_width >> 3;
    int mb_nr;
    int mergeup_avail, mergeleft_avail;
    int width_in_smb;
    SAOBlkParam *sao_left_param;
    SAOBlkParam *sao_up_param;
    mb_nr = mb_y * pic_mb_width + mb_x;
    width_in_smb = (input->img_width % (1 << input_MaxsizeInBit)) ? (input->img_width / (1 << input_MaxsizeInBit) + 1) : (input->img_width / (1 << input_MaxsizeInBit));

    mergeup_avail      = (mb_y == 0) ? 0 : h->slice_nr[lcu_y] == h->slice_nr[lcu_y - 1];
    mergeleft_avail    = (mb_x == 0) ? 0 : 1;
    
    if (mergeleft_avail) {
        sao_left_param =  rec_saoBlkParam[lcu_idx -  1] ;
        cpy_sao_param_blk(sao_merge_param[SAO_MERGE_LEFT], sao_left_param);
    }
    if (mergeup_avail) {
        sao_up_param = rec_saoBlkParam[lcu_idx - width_in_smb];
        cpy_sao_param_blk(sao_merge_param[SAO_MERGE_ABOVE], sao_up_param);
    }
    MergeAvail[SAO_MERGE_LEFT] = mergeleft_avail;
    MergeAvail[SAO_MERGE_ABOVE] = mergeup_avail;
}

void SAO_on_block(void *handle, void *sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height, int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down)
{
    avs3_enc_t *h = (avs3_enc_t *)handle;
    SAOBlkParam *saoBlkParam = (SAOBlkParam *)sao_data;
    int type;
    int start_x, end_x, start_y, end_y;
    int start_x_r0, end_x_r0, start_x_r, end_x_r, start_x_rn, end_x_rn;
    int x, y;
    pel_t *src, *dst;
    pel_t *src_base, *dst_base;
    int i_src, i_dst;
    char_t leftsign, rightsign, upsign, downsign;
    int diff;
    char_t signupline[65];
    char_t *signupline1;
    int reg = 0;
    int edgetype;
    int max_pixel = (1 << h->input->bit_depth) - 1;
    int bit_depth = h->input->bit_depth;
    int smb_available_upleft = (smb_available_up && smb_available_left);
    int smb_available_upright = (smb_available_up && smb_available_right);
    int smb_available_leftdown = (smb_available_down && smb_available_left);
    int smb_available_rightdwon = (smb_available_down && smb_available_right);

    i_src = h->img_sao.i_stride [compIdx];
    i_dst = h->img_rec->i_stride[compIdx];
    src = h->img_sao.plane [compIdx] + pix_y * i_src + pix_x;
    dst = h->img_rec->plane[compIdx] + pix_y * i_dst + pix_x;

    type = saoBlkParam->typeIdc;

    switch (type) {
    case SAO_TYPE_EO_0:
        end_y = lcu_pix_height;
        start_x = smb_available_left ? 0 : 1;
        end_x = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);

        for (y = 0; y < end_y; y++) {
            diff = src[start_x] - src[start_x - 1];
            leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            for (x = start_x; x < end_x; x++) {
                diff = src[x] - src[x + 1];
                rightsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = leftsign + rightsign;
                leftsign = -rightsign;
                dst[x] = avs3_pixel_clip(src[x] + saoBlkParam->offset[edgetype + 2], max_pixel);
            }
            src += i_src;
            dst += i_dst;
        }

        break;
    case SAO_TYPE_EO_90:
        end_x = lcu_pix_width;
        start_y = smb_available_up ? 0 : 1;
        end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);

        src_base = src + start_y * i_src;
        dst_base = dst + start_y * i_dst;

        for (x = 0; x < end_x; x++) {
            src = src_base + x;
            dst = dst_base + x;
            diff = src[0] - src[-i_src];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            for (y = start_y; y < end_y; y++) {
                diff = src[0] - src[i_src];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + upsign;
                upsign = -downsign;
                dst[0] = avs3_pixel_clip(src[0] + saoBlkParam->offset[edgetype + 2], max_pixel);
                src += i_src;
                dst += i_dst;
            }
        }

        break;
    case SAO_TYPE_EO_135:
        start_x_r0 = smb_available_upleft ? 0 : 1;
        end_x_r0 = smb_available_up ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;
        start_x_r = smb_available_left ? 0 : 1;
        end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
        start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
        end_x_rn = smb_available_rightdwon ? lcu_pix_width : (lcu_pix_width - 1);

        //init the line buffer
        for (x = start_x_r + 1; x < end_x_r + 1; x++) {
            diff = src[x + i_src] - src[x - 1];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            signupline[x] = upsign;
        }
        //first row
        for (x = start_x_r0; x < end_x_r0; x++) {
            diff = src[x] - src[x - 1 - i_src];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = upsign - signupline[x + 1];
            dst[x] = avs3_pixel_clip(src[x] + saoBlkParam->offset[edgetype + 2], max_pixel);
        }

        dst += i_dst;
        src += i_src;
        //middle rows
        for (y = 1; y < lcu_pix_height - 1; y++) {
            for (x = start_x_r; x < end_x_r; x++) {
                if (x == start_x_r) {
                    diff = src[x] - src[x - 1 - i_src];
                    upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    signupline[x] = upsign;
                }
                diff = src[x] - src[x + 1 + i_src];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + signupline[x];
                dst[x] = avs3_pixel_clip(src[x] + saoBlkParam->offset[edgetype + 2], max_pixel);
                signupline[x] = (char_t)reg;
                reg = -downsign;
            }
            src += i_src;
            dst += i_dst;
        }
        //last row
        if (lcu_pix_height == 0) {
            src -= i_src * 2;
            dst -= i_dst * 2;
        }
        for (x = start_x_rn; x < end_x_rn; x++) {
            if (x == start_x_r) {
                diff = src[x] - src[x - 1 - i_src];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                signupline[x] = upsign;
            }
            diff = src[x] - src[x + 1 + i_src];
            downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = downsign + signupline[x];
            dst[x] = avs3_pixel_clip(src[x] + saoBlkParam->offset[edgetype + 2], max_pixel);
        }

        break;
    case SAO_TYPE_EO_45:
        start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
        end_x_r0 = smb_available_upright ? lcu_pix_width : (lcu_pix_width - 1);
        start_x_r = smb_available_left ? 0 : 1;
        end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
        start_x_rn = smb_available_leftdown ? 0 : 1;
        end_x_rn = smb_available_down ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;

        //init the line buffer
        signupline1 = signupline + 1;
        for (x = start_x_r - 1; x < end_x_r - 1; x++) {
            diff = src[x + i_src] - src[x + 1];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            signupline1[x] = upsign;
        }
        //first row
        for (x = start_x_r0; x < end_x_r0; x++) {
            diff = src[x] - src[x + 1 - i_src];
            upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = upsign - signupline1[x - 1];
            dst[x] = avs3_pixel_clip(src[x] + saoBlkParam->offset[edgetype + 2], max_pixel);
        }
        dst += i_dst;
        src += i_src;

        //middle rows
        for (y = 1; y < lcu_pix_height - 1; y++) {
            for (x = start_x_r; x < end_x_r; x++) {
                if (x == end_x_r - 1) {
                    diff = src[x] - src[x + 1 - i_src];
                    upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    signupline1[x] = upsign;
                }
                diff = src[x] - src[x - 1 + i_src];
                downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                edgetype = downsign + signupline1[x];
                dst[x] = avs3_pixel_clip(src[x] + saoBlkParam->offset[edgetype + 2], max_pixel);
                signupline1[x - 1] = -downsign;
            }
            dst += i_dst;
            src += i_src;
        }

        if (lcu_pix_height == 0) {
            src -= i_src * 2;
            dst -= i_dst * 2;
        }

        for (x = start_x_rn; x < end_x_rn; x++) {
            if (x == end_x_r - 1) {
                diff = src[x] - src[x + 1 - i_src];
                upsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                signupline1[x] = upsign;
            }
            diff = src[x] - src[x - 1 + i_src];
            downsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
            edgetype = downsign + signupline1[x];
            dst[x] = avs3_pixel_clip(src[x] + saoBlkParam->offset[edgetype + 2], max_pixel);
        }

        break;
    default:
        printf("Not a supported SAO types\n");
        assert(0);
        exit(-1);

    }
}


void SAO_on_smb(avs3_enc_t *h, int lcu_x, int lcu_y, SAOBlkParam *saoBlkParam)
{
    sao_enc_t *sao = &h->sao_enc;
    const cfg_param_t* input = h->input;
    int pix_y = lcu_y * LCU_SIZE;
    int pix_x = lcu_x * LCU_SIZE;
    int lcu_pix_width  = lcu_x == input->pic_width_in_lcu - 1 ? input->img_width - lcu_x * LCU_SIZE : LCU_SIZE;
    int lcu_pix_height = lcu_y == input->pic_height_in_lcu - 1 ? input->img_height - lcu_y * LCU_SIZE : LCU_SIZE;
    int isAboveAvail = pix_y ? 1 : 0;
    int isBelowAvail = pix_y + lcu_pix_height < input->img_height;
    int isLeftAvail  = pix_x ? 1 : 0;
    int isRightAvail = pix_x + lcu_pix_width  < input->img_width;
    int isFltSliceBorder = isAboveAvail;
    int *sli_nr = h->slice_nr + lcu_y;

    int x_offset = 0, width_offset = 0, height_offset = 0;
    int blk_x, blk_y, blk_width, blk_height;

    if ((saoBlkParam[SAO_Y].typeIdc == -1) && (saoBlkParam[SAO_Cb].typeIdc == -1) && (saoBlkParam[SAO_Cr].typeIdc == -1)) {
        return;
    }

    if (!LOOPFLT_CROSS_SLICE && isFltSliceBorder && sli_nr[0] != sli_nr[-1]) {
        isFltSliceBorder = 0;
    }
    
    if (isLeftAvail) {
        x_offset -= 4;
        width_offset += 4;
    }
    if (isRightAvail) {
        width_offset -= 4;
    }
    if (isBelowAvail) {
        height_offset -= 4;
    }

    blk_width  = width_offset  + lcu_pix_width;
    blk_height = height_offset + lcu_pix_height;
    blk_x = x_offset + pix_x;
    blk_y = pix_y;

    if (saoBlkParam[0].typeIdc != -1) {
        g_funs_handle.sao_flt[1](h, &(saoBlkParam[0]), 0, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail);
    }

    blk_width  = width_offset + (lcu_pix_width >> 1);
    blk_height = height_offset + (lcu_pix_height >> 1);
    blk_x = x_offset + (pix_x >> 1);
    blk_y = pix_y >> 1;
    
    if (saoBlkParam[1].typeIdc != -1) {
        g_funs_handle.sao_flt[1](h, &(saoBlkParam[1]), 1, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail);
    }
    if (saoBlkParam[2].typeIdc != -1) {
        g_funs_handle.sao_flt[1](h, &(saoBlkParam[2]), 2, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, isFltSliceBorder, isBelowAvail);
    }

    if (isAboveAvail) {
        blk_width = width_offset + lcu_pix_width;
        blk_height = 4;
        blk_x = x_offset + pix_x;
        blk_y = pix_y - 4;

        if (saoBlkParam[0].typeIdc != -1) {
            g_funs_handle.sao_flt[1](h, &(saoBlkParam[0]), 0, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, 1, isFltSliceBorder);
        }

        blk_width = width_offset + (lcu_pix_width >> 1);
        blk_height = 4;
        blk_x = x_offset + (pix_x >> 1);
        blk_y = (pix_y >> 1) - 4;

        if (saoBlkParam[1].typeIdc != -1) {
            g_funs_handle.sao_flt[1](h, &(saoBlkParam[1]), 1, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, 1, isFltSliceBorder);
        }
        if (saoBlkParam[2].typeIdc != -1) {
            g_funs_handle.sao_flt[1](h, &(saoBlkParam[2]), 2, blk_y, blk_x, blk_height, blk_width, isLeftAvail, isRightAvail, 1, isFltSliceBorder);
        }
    }
}


