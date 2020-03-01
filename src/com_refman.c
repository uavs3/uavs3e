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

#include "com_modules.h"

#define IS_REF(pic) ((pic)->b_ref)

static void set_refp(com_ref_pic_t *refp, com_pic_t *pic_ref)
{
    refp->pic      = pic_ref;
    refp->ptr      = pic_ref->ptr;
    refp->map_mv   = pic_ref->map_mv;
    refp->map_refi = pic_ref->map_refi;
    refp->list_ptr = pic_ref->list_ptr;

    com_img_addref(pic_ref->img);
}

void com_refm_build_ref_buf(com_pic_manager_t *pm)
{
    int cnt = 0;
    com_pic_t **pic = pm->pic;
    com_pic_t **pic_ref = pm->pic_ref;

    memset(pic_ref, 0, sizeof(com_pic_t*) * MAX_REFS);

    for (int i = 0; i < pm->max_pb_size; i++) {
        if (pic[i] && IS_REF(pic[i])) {
            pic_ref[cnt++] = pic[i];
        }
    }

    /* sort by ptr */
    for (int i = 0; i < cnt - 1; i++) {
        for (int j = i + 1; j < cnt; j++) {
            if (pic_ref[i]->ptr < pic_ref[j]->ptr) {
                com_pic_t *pic_t = pic_ref[i];
                pic_ref[i] = pic_ref[j];
                pic_ref[j] = pic_t;
            }
        }
    }
}

int com_refm_get_active_ref(com_pic_manager_t *pm, com_pic_header_t *pichdr, com_ref_pic_t(*refp)[REFP_NUM])
{
    if ((pichdr->slice_type == SLICE_I) && (pichdr->poc == 0)) {
        return COM_OK;
    }

    if (pichdr->slice_type != SLICE_I) {
        com_assert_rv(pm->cur_num_ref_pics > 0, COM_ERR_UNEXPECTED);
    }
    for (int i = 0; i < MAX_REFS; i++) {
        refp[i][REFP_0].pic = refp[i][REFP_1].pic = NULL;
    }
    pm->num_refp[REFP_0] = pm->num_refp[REFP_1] = 0;

    /********************************************************************************************/
    /* L0 list */
    
    for (int i = 0; i < MAX_REFS; i++) {
        int j = 0;
        s64 refPicPoc = pichdr->poc - pichdr->rpl_l0.ref_poc[i];
  
        if (pichdr->rpl_l0.ref_poc[i] <= 0 && i >= pichdr->rpl_l0.active) {
            continue;
        }

        while (j < pm->cur_num_ref_pics && pm->pic_ref[j]->ptr != refPicPoc) j++;

        if (i < pichdr->rpl_l0.active) {
            com_assert_rv(j < pm->cur_num_ref_pics && pm->pic_ref[j]->ptr == refPicPoc, COM_ERR);
            set_refp(&refp[i][REFP_0], pm->pic_ref[j]);
            pm->num_refp[REFP_0]++;
        }

        if (j < pm->cur_num_ref_pics && pm->pic_ref[j]->ptr == refPicPoc) {
            int diff = (int)(pichdr->dtr % DOI_CYCLE_LENGTH - pm->pic_ref[j]->dtr);

            if (diff != pichdr->rpl_l0.delta_doi[i]) { // make sure delta doi of RPL0 correct,in case of last incomplete GOP
                pichdr->ref_pic_list_sps_flag[0] = 0;
                pichdr->rpl_l0.delta_doi[i] = diff;
            }
        }
    }

    if (pichdr->slice_type == SLICE_P) {
        return COM_OK;
    }

    /********************************************************************************************/
    /* L1 list */
	
    for (int i = 0; i < MAX_REFS; i++) {
        int j = 0;
        s64 refPicPoc = pichdr->poc - pichdr->rpl_l1.ref_poc[i];
      
        if (pichdr->rpl_l1.ref_poc[i] <= 0 && i >= pichdr->rpl_l1.active) {
            continue;
        }

        while (j < pm->cur_num_ref_pics && pm->pic_ref[j]->ptr != refPicPoc) j++;

        if (i < pichdr->rpl_l1.active) {
            com_assert_rv(j < pm->cur_num_ref_pics && pm->pic_ref[j]->ptr == refPicPoc, COM_ERR);
            set_refp(&refp[i][REFP_1], pm->pic_ref[j]);
            pm->num_refp[REFP_1]++;
        }

        if (j < pm->cur_num_ref_pics && pm->pic_ref[j]->ptr == refPicPoc) {
            int diff = (int)(pichdr->dtr % DOI_CYCLE_LENGTH - pm->pic_ref[j]->dtr); 

            if (diff != pichdr->rpl_l1.delta_doi[i]) { //make sure delta doi of RPL0 correct
                pichdr->ref_pic_list_sps_flag[1] = 0;
                pichdr->rpl_l1.delta_doi[i] = diff;
            }
        }
    }

    return COM_OK;
}

com_pic_t *com_refm_find_free_pic(com_pic_manager_t *pm, int *err)
{
    int ret;
    com_pic_t *pic = NULL;

    for (int i = 0; i < pm->max_pb_size; i++) {
        if (pm->pic[i] != NULL && !IS_REF(pm->pic[i])) {
            com_img_t *img = pm->pic[i]->img;
            com_assert(img != NULL);

            if (1 == com_img_getref(img)) {
                pic = pm->pic[i];

                for (; i < pm->max_pb_size - 1; i++) {
                    pm->pic[i] = pm->pic[i + 1];
                }
                pm->pic[pm->max_pb_size - 1] = NULL;
                break;
            }
        }
    }

    if (pic == NULL) {
        int cur_pb_size = 0;
        for (int i = 0; i < pm->max_pb_size; i++) {
            if (pm->pic[i]) {
                cur_pb_size++;
            }
        }
        if (cur_pb_size < pm->max_pb_size) {
            pic = com_picbuf_create(pm->pic_width, pm->pic_height, pm->pad_l, pm->pad_c, &ret);
            com_assert_gv(pic != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        } else {
            com_assert_gv(0, ret, COM_ERR_UNKNOWN, ERR);
        }
    }

    com_img_addref(pic->img);

    if (err) {
        *err = COM_OK;
    }
    return pic;
ERR:
    if (err) {
        *err = ret;
    }
    if (pic) {
        com_picbuf_free(pic);
    }
    return NULL;
}

void print_pm(com_pic_manager_t *pm, char type)
{
#if 0
    printf("%c pb:%2d(%2d)  ==> ",type, pm->cur_num_ref_pics, pm->max_pb_size);

    for (int i = 0; i < pm->max_pb_size; i++) {
        if (pm->pic[i] == NULL) printf("-");
        else if (pm->pic[i]->b_ref) printf("R");
        else if (1 == com_img_getref(pm->pic[i]->img)) printf("0");
        else printf("*");
    }
    printf("\n");
#endif
}

int com_refm_mark_ref_pic(com_pic_manager_t *pm, com_pic_header_t *pichdr)
{
    for (int i = 0; i < pm->cur_num_ref_pics; i++) {
        com_pic_t * pic = pm->pic[i];
        if (pic && IS_REF(pic)) {
            int in_rpl = 0;
            for (int j = 0; j < pichdr->rpl_l0.num; j++) {
                if (pic->ptr == (pichdr->poc - pichdr->rpl_l0.ref_poc[j])) { //NOTE: we need to put POC also in com_pic_t
                    in_rpl = 1;
                    break;
                }
            }
            if (!in_rpl) {
                for (int j = 0; j < pichdr->rpl_l1.num; j++) {
                    if (pic->ptr == (pichdr->poc - pichdr->rpl_l1.ref_poc[j])) { //NOTE: we need to put POC also in com_pic_t
                        in_rpl = 1;
                        break;
                    }
                }
            }
            if (!in_rpl) {
                print_pm(pm, ' ');
                pic->b_ref = 0;

                for (int j = i; j < pm->max_pb_size - 1; j++) {
                    pm->pic[j] = pm->pic[j + 1];
                }
                pm->pic[pm->max_pb_size - 1] = pic;

                pm->cur_num_ref_pics--;
                i--;       
                print_pm(pm, '-');
            }
        }
    }

    return COM_OK;
}

int com_refm_insert_rec_pic(com_pic_manager_t *pm, com_pic_t *pic, int slice_type, s64 ptr, s64 dtr, com_ref_pic_t(*refp)[REFP_NUM])
{
    pic->ptr = ptr;
    pic->dtr = (s32)(dtr % DOI_CYCLE_LENGTH); 

    print_pm(pm, ' ');

    for (int i = 0; i < pm->num_refp[REFP_0]; i++) {
        pic->list_ptr[i] = refp[i][REFP_0].ptr;
    }
    if (pm->pic[pm->cur_num_ref_pics]) {
        for (int i = pm->cur_num_ref_pics + 1; i < pm->max_pb_size; i++) {
            if (pm->pic[i] == NULL) {
                pm->pic[i] = pm->pic[pm->cur_num_ref_pics];
                pm->pic[pm->cur_num_ref_pics] = NULL;
                break;
            }
        }
    }

    com_assert(pm->pic[pm->cur_num_ref_pics] == NULL);

    pm->pic[pm->cur_num_ref_pics] = pic;

    pm->cur_num_ref_pics += pic->b_ref;

    print_pm(pm, '+');

    return COM_OK;
}

int com_refm_create(com_pic_manager_t *pm, int max_pb_size, int max_num_ref_pics, int width, int height)
{
    if (max_num_ref_pics > MAX_REFS) {
        return COM_ERR_UNSUPPORTED;
    }
    pm->max_num_ref_pics = max_num_ref_pics;
    pm->max_pb_size = max_pb_size;
    pm->ptr_increase = 1;

    pm->pic_width = width;
    pm->pic_height = height;
    pm->pad_l = PIC_PAD_SIZE_L;
    pm->pad_c = PIC_PAD_SIZE_C;

    pm->pic = com_malloc(sizeof(com_pic_t*) * max_pb_size);

    return COM_OK;
}

int com_refm_free(com_pic_manager_t *pm)
{
    int i;

    for (i = 0; i < pm->max_pb_size; i++) {
        if (pm->pic[i]) {
            com_picbuf_free(pm->pic[i]);
            pm->pic[i] = NULL;
        }
    }

    pm->cur_num_ref_pics = 0;

    com_mfree(pm->pic);

    return COM_OK;
}

