#include "define.h"

#define UNIT_SIZE_LOG2 4
#define UNIT_SIZE (1 << UNIT_SIZE_LOG2)
#define UNIT_WIDX (UNIT_SIZE_LOG2 - MIN_CU_LOG2)

static void get_ipred_neighbor(pel *dst, int x, int y, int w, int h, int pic_width, int pic_height, pel *src, int s_src, int bit_depth)
{
    pel *srcT  = src - s_src;
    pel *srcTL = srcT - 1;
    pel *srcL  = src - 1;
    pel *left  = dst - 1;
    pel *up    = dst + 1;
    int default_val = 1 << (bit_depth - 1);
    int pad_range = COM_MAX(w, h) * 4 + 4;

    if (y) {
        int size = COM_MIN(2 * w, pic_width - x);
        com_mcpy(up, srcT, size * sizeof(pel));
        up += size, srcT += size;
        com_mset_pel(up, up[-1], pad_range - size);
    } else {
        com_mset_pel(up, default_val, pad_range);
    }
    if (x) {
        pel *src = srcL;
        int size = COM_MIN(2 * h, pic_height - y);

        for (int i = 0; i < size; ++i) {
            *left-- = *src;
            src += s_src;
        }
        int pads = pad_range - size;
        com_mset_pel(left - pads + 1, left[1], pad_range - size);
    } else {
        com_mset_pel(left - pad_range + 1, default_val, pad_range);
    }
    if (x && y) {
        dst[0] = srcTL[0];
    } else if (y) {
        dst[0] = srcTL[1];
    } else if (x) {
        dst[0] = srcL[0];
    } else {
        dst[0] = default_val;
    }
}

double loka_estimate_coding_cost(inter_search_t *pi, com_img_t *img_org, com_img_t **ref_l0, com_img_t **ref_l1, int num_ref[2], int bit_depth)
{
    const int    base_qp = 32;
    const double base_lambda = 1.43631 * pow(2.0, (base_qp - 16.0) / 4.0);

    int pic_width  = img_org->width [0];
    int pic_height = img_org->height[0];

    double total_cost = 0;
    int i_org = img_org->stride[0];
    com_pic_t pic = { 0 };
    com_subpel_t subpel;

    pic.subpel = &subpel;

    pi->ptr       = img_org->ptr;
    pi->curr_mvr  = 2;
    pi->lambda_mv = (u32)floor(65536.0 * sqrt(base_lambda));
    pi->i_org     = i_org;
    pi->fast_me   = 0;

#define WRITE_REC_PIC 0

#if WRITE_REC_PIC
    static pel *buf = NULL;
    if (buf == NULL) buf = malloc(pic_width * pic_height);
    static FILE *fp = NULL;
    if (fp == NULL) fp = fopen("preprocess.yuv", "wb");
    static FILE *fp_org = NULL;
    if (fp_org == NULL) fp_org = fopen("preprocess_org.yuv", "wb");
#endif

    for (int y = 0; y < pic_height - UNIT_SIZE + 1; y += UNIT_SIZE) {
        for (int x = 0; x < pic_width - UNIT_SIZE + 1; x += UNIT_SIZE) {
            ALIGNED_32(pel pred_buf[MAX_CU_DIM]);
            u64 min_cost = COM_UINT64_MAX;
            pel *org = (pel*)img_org->planes[0] + y * img_org->stride[0] + x;

            pi->org = org;

            for (int lidx = 0; lidx < 2; lidx++) { // uni-prediction (L0 or L1)
                pi->num_refp = num_ref[lidx];

                for (int refi = 0; refi < num_ref[lidx]; refi++) {
                    com_img_t *ref_img = (lidx ? ref_l1 : ref_l0)[refi];
                    pic.ptr = ref_img->ptr;
                    pic.img = ref_img;
                    pic.stride_luma = ref_img->stride[0];
                    pic.y = ref_img->planes[0];
                    pic.subpel->imgs[0][0] = ref_img;

                    com_pic_t *ref_pic = pi->ref_pic = &pic;
                    s16 mv[2], mvp[2] = { 0 };

                    me_search_tz(pi, x, y, UNIT_SIZE, UNIT_SIZE, pic_width, pic_height, refi, lidx, mvp, mv, 0);
                    com_mc_blk_luma(ref_pic, pred_buf, UNIT_SIZE, (x << 2) + mv[MV_X], (y << 2) + mv[MV_Y], UNIT_SIZE, UNIT_SIZE, UNIT_WIDX, pi->max_coord[MV_X], pi->max_coord[MV_Y], (1 << bit_depth) - 1, 0);

                    u32 cost = com_had(UNIT_SIZE, UNIT_SIZE, org, pred_buf, i_org, UNIT_SIZE, bit_depth);
                    if (cost < min_cost) {
#if WRITE_REC_PIC 
                        uavs3e_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                        min_cost = cost;

                    }
                }
            }

            pel nb_buf[INTRA_NEIB_SIZE];
            get_ipred_neighbor(nb_buf + INTRA_NEIB_MID, x, y, UNIT_SIZE, UNIT_SIZE, pic_width, pic_height, org, i_org, bit_depth);

            int avaliable_nb = (x ? AVAIL_LE : 0) | (y ? AVAIL_UP : 0) | ((x && y) ? AVAIL_UP_LE : 0);
            static tab_s8 ipm_tab[] = { 0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32 };

            for (int i = 0; i < sizeof(ipm_tab); i++) {
                com_intra_pred(nb_buf + INTRA_NEIB_MID, pred_buf, ipm_tab[i], UNIT_SIZE, UNIT_SIZE, bit_depth, avaliable_nb, 0);
                u32 cost = com_had(UNIT_SIZE, UNIT_SIZE, org, pred_buf, i_org, UNIT_SIZE, bit_depth);

                if (cost < min_cost) {
#if WRITE_REC_PIC 
                    uavs3e_funs_handle.ipcpy[UNIT_WIDX](pred_buf, UNIT_SIZE, buf + y * pic_width + x, pic_width, UNIT_SIZE, UNIT_SIZE);
#endif
                    min_cost = cost;
                }
            }
            total_cost += min_cost;
        }        
    }

#if WRITE_REC_PIC
    fwrite(buf, 1, pic_width * pic_height, fp);
    fwrite(img_org->planes[0], 1, pic_width * pic_height, fp_org);
#endif

#undef WRITE_REC_PIC

    return total_cost / (pic_width / UNIT_SIZE) / (pic_height / UNIT_SIZE) / UNIT_SIZE / UNIT_SIZE;
}