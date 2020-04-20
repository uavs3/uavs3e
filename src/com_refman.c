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

    com_assert(pic_ref->subpel);

    com_img_addref(pic_ref->img);
}

void com_refm_build_ref_buf(com_pic_manager_t *pm)
{
    com_pic_t **pic = pm->pic;

    /* sort by ptr */
    for (int i = 0; i < pm->cur_num_ref_pics - 1; i++) {
        for (int j = i + 1; j < pm->cur_num_ref_pics; j++) {
            if (pic[i]->ptr > pic[j]->ptr) {
                com_pic_t *pic_t = pic[i];
                pic[i] = pic[j];
                pic[j] = pic_t;
            }
        }
    }
}

int com_refm_create_rpl(com_pic_manager_t *pm, com_pic_header_t *pichdr, com_ref_pic_t(*refp)[REFP_NUM], com_pic_t *top_pic[REFP_NUM], int is_top_level)
{
    int idx_nearest_l0;
    int num0 = 0, num1 = 0;
    com_pic_t *pic_ref_l0[MAX_REFS];
    com_pic_t *pic_ref_l1[MAX_REFS];
    com_rpl_t *rpl;
    com_pic_t **pic_ref = pm->pic;

    if (pm->cur_num_ref_pics == 0) {
        pm->num_refp[REFP_0] = pm->num_refp[REFP_1] = 0;
        pichdr->rpl_l0.num = pichdr->rpl_l0.active = 0;
        pichdr->rpl_l1.num = pichdr->rpl_l1.active = 0;
        return COM_OK;
    }
    for (int i = 0; i < pm->cur_num_ref_pics; i++) {
        com_pic_t *pic = pic_ref[i];
        if (pic->ptr < pichdr->poc) {
            idx_nearest_l0 = i;
        } else {
            break;
        }
    }
    if (is_top_level) {
        for (int i = idx_nearest_l0; i >= 0; i--) {
            if (pic_ref[i]->layer_id < FRM_DEPTH_2) {
                pic_ref_l0[num0++] = pic_ref[i];
            }
        }
        for (int i = idx_nearest_l0; i >= 0; i--) {
            if (pic_ref[i]->layer_id >= FRM_DEPTH_2) {
                pic_ref_l0[num0++] = pic_ref[i];
            }
        }
    } else {
        for (int i = idx_nearest_l0; i >= 0; i--) {
            pic_ref_l0[num0++] = pic_ref[i];
        }
    }
    for (int i = idx_nearest_l0 + 1; i < pm->cur_num_ref_pics; i++) {
        pic_ref_l1[num1++] = pic_ref[i];
    }
    for (int i = 0; num0 < pichdr->rpl_l0.active && i < num1; i++, num0++) {
        pic_ref_l0[num0] = pic_ref_l1[i];
    }
    for (int i = 0; num1 < pichdr->rpl_l1.active && i < num0; i++, num1++) {
        pic_ref_l1[num1] = pic_ref_l0[i];
    }
    for (int i = 0; i < num0; i++) {
        if (pic_ref_l0[i]->layer_id <= FRM_DEPTH_1) {
            top_pic[0] = pic_ref_l0[i];
            break;
        }
    }
    for (int i = 0; i < num1; i++) {
        if (pic_ref_l1[i]->layer_id <= FRM_DEPTH_1) {
            top_pic[1] = pic_ref_l1[i];
            break;
        }
    }

    /********************************************************************************************/
    /* L0 list */
    rpl = &pichdr->rpl_l0;
    rpl->num = num0;
    rpl->active = pm->num_refp[REFP_0] = COM_MIN(rpl->active, num0);

    for (int i = 0; i < num0; i++) {
        com_pic_t *pic = pic_ref_l0[i];
        rpl->delta_doi[i] = (int)(pichdr->dtr % DOI_CYCLE_LENGTH - pic->dtr);
        if (i < rpl->active) {
            set_refp(&refp[i][REFP_0], pic);
        }
    }

    if (pichdr->slice_type == SLICE_P) {
        return COM_OK;
    }

    /********************************************************************************************/
    /* L1 list */
	
    rpl = &pichdr->rpl_l1;
    rpl->num = num1;
    rpl->active = pm->num_refp[REFP_1] = COM_MIN(rpl->active, num1);

    for (int i = 0; i < num1; i++) {
        com_pic_t *pic = pic_ref_l1[i];
        rpl->delta_doi[i] = (int)(pichdr->dtr % DOI_CYCLE_LENGTH - pic->dtr);

        if (i < rpl->active) {
            set_refp(&refp[i][REFP_1], pic);
        }
    }

    return COM_OK;
}

void com_refm_pick_seqhdr_idx(com_seqh_t *seqhdr, com_pic_header_t *pichdr)
{
    int ref_num;
    int *delta_doi;
    int idx;

    pichdr->ref_pic_list_sps_flag[0] = 0;
    pichdr->ref_pic_list_sps_flag[1] = 0;

    ref_num   = pichdr->rpl_l0.num;
    delta_doi = pichdr->rpl_l0.delta_doi;

    for (int i = 0; i < seqhdr->rpls_l0_num; i++) {
        com_rpl_t *rpl = seqhdr->rpls_l0 + i;

        if (ref_num != rpl->num) {
            continue;
        }
        for (idx = 0; idx < rpl->num; idx++) {
            if (rpl->delta_doi[idx] != delta_doi[idx]) {
                break;
            }
        }
        if (idx == ref_num) {
            pichdr->ref_pic_list_sps_flag[0] = 1;
            pichdr->rpl_l0_idx = i;
            break;
        }
    }

    ref_num   = pichdr->rpl_l1.num;
    delta_doi = pichdr->rpl_l1.delta_doi;

    for (int i = 0; i < seqhdr->rpls_l1_num; i++) {
        com_rpl_t *rpl = seqhdr->rpls_l1 + i;

        if (ref_num != rpl->num) {
            continue;
        }
        for (idx = 0; idx < rpl->num; idx++) {
            if (rpl->delta_doi[idx] != delta_doi[idx]) {
                break;
            }
        }
        if (idx == ref_num) {
            pichdr->ref_pic_list_sps_flag[1] = 1;
            pichdr->rpl_l1_idx = i;
            break;
        }
    }
}

com_pic_t *com_refm_find_free_pic(com_pic_manager_t *pm, int b_ref, int *err)
{
    int ret;
    com_pic_t *pic = NULL;

    for (int i = 0; i < pm->max_pb_size; i++) {
        if (pm->pic[i] && !pm->pic[i]->b_ref) {
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
            pic = com_pic_create(pm->pic_width, pm->pic_height, pm->pad_l, pm->pad_c, &ret);
            com_assert_gv(pic != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
        } else {
            com_assert_gv(0, ret, COM_ERR_UNKNOWN, ERR);
        }
    }

    com_img_addref(pic->img);

    if (b_ref) {
        com_subpel_t *subpel = NULL;

        for (int i = 0; i < pm->cur_num_subpels; i++) {
            if (pm->subpel[i] && !pm->subpel[i]->b_used) {
                subpel = pm->subpel[i];
                break;
            }
        }
        if (subpel == NULL) {
            if (pm->cur_num_subpels == pm->max_pb_size) {
                com_assert_gv(0, ret, COM_ERR_UNKNOWN, ERR);
            }
            subpel = pm->subpel[pm->cur_num_subpels] = com_subpel_create(pm->pic_width, pm->pic_height, pm->pad_l, pm->pad_c, &ret);
            com_assert_gv(subpel != NULL, ret, COM_ERR_OUT_OF_MEMORY, ERR);
            pm->cur_num_subpels++;
        }
        subpel->b_used = 1;
        subpel->imgs[0][0] = pic->img;

        pic->subpel = subpel;
    } else {
        pic->subpel = NULL;
    }

    if (err) {
        *err = COM_OK;
    }
    return pic;
ERR:
    if (err) {
        *err = ret;
    }
    if (pic) {
        com_pic_destroy(pic);
    }
    return NULL;
}

void print_pm(com_pic_manager_t *pm, char type)
{
#if 0
    printf("%c refpic/subpel:%2d/%2d(%2d)  ==> ",type, pm->cur_num_ref_pics, pm->cur_num_subpels, pm->max_pb_size);

    for (int i = 0; i < pm->max_pb_size; i++) {
        if (pm->pic[i] == NULL) printf("-");
        else if (pm->pic[i]->b_ref) printf("R");
        else if (1 == com_img_getref(pm->pic[i]->img)) printf("0");
        else printf("*");
    }
    printf("\n");
#endif
}

static void remove_ref_pic(com_pic_manager_t *pm, int idx)
{
    com_pic_t *pic = pm->pic[idx];

    print_pm(pm, ' ');

    pic->b_ref = 0;

    if (pic->subpel && 1 == com_img_getref(pic->img)) {
        pic->subpel->b_used = 0;
    }

    for (int j = idx; j < pm->max_pb_size - 1; j++) {
        pm->pic[j] = pm->pic[j + 1];
    }
    pm->pic[pm->max_pb_size - 1] = pic;

    pm->cur_num_ref_pics--;

    print_pm(pm, '-');
}

void com_refm_remove_ref_pic(com_pic_manager_t *pm, com_pic_header_t *pichdr, com_pic_t *pic, int close_gop, int is_ld)
{
    if (pichdr->slice_type == SLICE_I) {
        pm->ptr_l_i = pic->ptr;

        if (close_gop) {
            for (int i = 0; i < pm->cur_num_ref_pics; i++) {
                com_pic_t *ref = pm->pic[i];
                if (ref && ref->b_ref) {
                    remove_ref_pic(pm, i--);
                }
            }
            pm->ptr_l_l_ip = pm->ptr_l_ip = pic->ptr;
            com_assert(pm->cur_num_ref_pics == 0);
            return;
        }
    } else if (pic->ptr > pm->ptr_l_i) {
        for (int i = 0; i < pm->cur_num_ref_pics; i++) {
            com_pic_t *ref = pm->pic[i];
            if (ref && ref->b_ref && ref->ptr < pm->ptr_l_i) {
                remove_ref_pic(pm, i--);
            }
        }
    }
    if (is_ld) {
        if (pm->cur_num_ref_pics <= MAX_LD_ACTIVE) {
            return;
        }
        for (int i = 0; i < pm->cur_num_ref_pics; i++) {
            com_pic_t * ref = pm->pic[i];
            if (ref && ref->b_ref) {
                if (ref->ptr < pic->ptr - 1 && ref->layer_id >= FRM_DEPTH_2) {
                    remove_ref_pic(pm, i--);
                }
            }
        }
        if (pm->cur_num_ref_pics <= MAX_LD_ACTIVE) {
            return;
        }

        s64 oldest_ref = pic->ptr;
        int oldest_idx = 0;

        for (int i = 0; i < pm->cur_num_ref_pics; i++) {
            com_pic_t * ref = pm->pic[i];
            if (ref && ref->b_ref && ref->ptr < oldest_ref) {
                oldest_ref = ref->ptr;
                oldest_idx = i;
            }
        }
        remove_ref_pic(pm, oldest_idx);
    } else {
        if (pic->layer_id < FRM_DEPTH_2) { // top frames
            for (int i = 0; i < pm->cur_num_ref_pics; i++) {
                com_pic_t * ref = pm->pic[i];
                if (ref && ref->b_ref && ref->ptr < pm->ptr_l_ip && ref->layer_id > FRM_DEPTH_2) {
                    remove_ref_pic(pm, i--);
                }
                if (ref && ref->b_ref && ref->ptr < pm->ptr_l_l_ip) {
                    remove_ref_pic(pm, i--);
                }
            }
            pm->ptr_l_l_ip = pm->ptr_l_ip;
            pm->ptr_l_ip = pic->ptr;
        } else {
            s64 nearest_before_l_l_ip = -1;
            int ref_num_in_cur_subgop = 1;

            for (int i = 0; i < pm->cur_num_ref_pics; i++) {
                com_pic_t * ref = pm->pic[i];
                if (ref && ref->b_ref && ref->ptr < pic->ptr) {
                    if (ref->ptr > pm->ptr_l_l_ip) {
                        if (ref->layer_id >= pic->layer_id) {
                            remove_ref_pic(pm, i--); // delete deeper pic (include current depth) in current subgop
                        }
                        else {
                            ref_num_in_cur_subgop++;
                        }
                    }
                    if (ref->ptr < pm->ptr_l_l_ip && ref->ptr > nearest_before_l_l_ip) {
                        nearest_before_l_l_ip = ref->ptr;
                    }
                }
            }
            for (int i = 0; i < pm->cur_num_ref_pics; i++) {
                com_pic_t * ref = pm->pic[i];
                if (ref && ref->b_ref && ref->ptr < pm->ptr_l_l_ip && (ref->ptr < nearest_before_l_l_ip || ref_num_in_cur_subgop >= 2)) {
                    remove_ref_pic(pm, i--);
                }
            }
            if (!pic->b_ref) {
                for (int i = 0; i < pm->cur_num_ref_pics; i++) {
                    com_pic_t * ref = pm->pic[i];
                    if (ref && ref->b_ref && ref->ptr + 4 < pm->ptr_l_l_ip) {
                        remove_ref_pic(pm, i--);
                    }
                }
            }
            return;
        }
    }
}

int com_refm_insert_rec_pic(com_pic_manager_t *pm, com_pic_t *pic, com_ref_pic_t(*refp)[REFP_NUM])
{
    print_pm(pm, ' ');

    if (pic->b_ref) {
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
        pm->cur_num_ref_pics++;
    } else {
        for (int i = pm->max_pb_size - 1; i >= pm->cur_num_ref_pics; i--) {
            if (pm->pic[i] == NULL) {
                pm->pic[i] = pic;
                pic = NULL;
                break;
            }
        }
        com_assert(pic == NULL);
    }
    print_pm(pm, '+');

    return COM_OK;
}

int com_refm_create(com_pic_manager_t *pm, int max_pb_size, int width, int height)
{
    pm->max_pb_size = max_pb_size;
    pm->ptr_increase = 1;

    pm->pic_width = width;
    pm->pic_height = height;
    pm->pad_l = PIC_PAD_SIZE_L;
    pm->pad_c = PIC_PAD_SIZE_C;

    pm->pic    = com_malloc(sizeof(com_pic_t   *) * max_pb_size);
    pm->subpel = com_malloc(sizeof(com_subpel_t*) * max_pb_size);

    return COM_OK;
}

int com_refm_free(com_pic_manager_t *pm)
{
    int i;

    for (i = 0; i < pm->max_pb_size; i++) {
        if (pm->pic[i]) {
            com_pic_destroy(pm->pic[i]);
        }
        if (pm->subpel[i]) {
            com_subpel_free(pm->subpel[i]);
        }
    }

    com_mfree(pm->pic);
    com_mfree(pm->subpel);

    return COM_OK;
}

