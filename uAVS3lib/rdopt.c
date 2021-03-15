#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>


#include "block.h"
#include "vlc.h"
#include "codingUnit.h"
#include "global.h"
#include "AEC.h"
#include "commonVariables.h"
#include "transform.h"
#include "intra-prediction.h"
#include "common.h"

int intra_enc_pu_luma(avs3_enc_t *h, cu_t *cu, aec_t *aec, int uiBitSize, int *nz, int ipmode)
{
    coef_t *p_coef = h->cu_coefs.coef_y;
    int i_coef = h->tu_q_size;
    int i_pred = h->pu_size;
    pel_t *p_pred = h->pred_intra_luma[ipmode];
    pel_t *p_org = h->p_org[0] + (h->pu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->pu_pix_x - h->lcu_pix_x;
    pel_t *p_rec = h->p_rec[0] + (h->pu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->pu_pix_x - h->lcu_pix_x;

    *nz = 0;

    transform_blk(h, 0, p_org, CACHE_STRIDE, p_pred, i_pred, p_coef, i_coef, uiBitSize, cu, 0);

    if (quant_blk(h, aec, cu->QP, 4, p_coef, uiBitSize, cu, 0, ipmode)) { // if not zero block
        ALIGNED_16(coef_t tmp_coef[MAX_CU_SIZE * MAX_CU_SIZE]);
        *nz = 1;
        inv_quant_blk(h, cu->QP, p_coef, i_coef, tmp_coef, i_coef, uiBitSize, 0);
        inv_transform(h, 0, tmp_coef, i_coef, p_pred, i_pred, p_rec, CACHE_STRIDE, uiBitSize, cu, 0);
    } else { // if zero block
        g_funs_handle.cpy_pel[h->pu_size >> 1](p_pred, i_pred, p_rec, CACHE_STRIDE, h->pu_size, h->pu_size);
    }

    return g_funs_handle.cost_ssd[uiBitSize - 2](p_org, CACHE_STRIDE, p_rec, CACHE_STRIDE, h->pu_size);
}


double intra_pu_rdcost(avs3_enc_t *h, aec_t *aec_up, cu_t *cu, int uiBitSize, int *nonzero, int ipmode, double lambda, const int *mostProbableMode, int *distortion, double mcost)
{
    *distortion = intra_enc_pu_luma(h, cu, aec_up, uiBitSize, nonzero, ipmode);

    if (*distortion < mcost) {
        aec_t cs_tmp;
        aec_t *aec = &cs_tmp;
        int bit_pos ;
        int mode_val = (mostProbableMode[0] == ipmode) ? -2 : ((mostProbableMode[1] == ipmode) ? -1 : (ipmode < mostProbableMode[0] ? ipmode : (ipmode < mostProbableMode[1] ? ipmode - 1 : ipmode - 2)));

        copy_coding_state_hdr(&cs_tmp, aec_up);

        bit_pos = arienco_bits_written(aec);

        writeIntraPredMode(aec, mode_val);

        if (*distortion + lambda * (arienco_bits_written(aec) - bit_pos) < mcost) {
            if (*nonzero) {
                int max_bits = (int)((mcost - *distortion) / lambda) - (arienco_bits_written(aec) - bit_pos);
                write_coeffs(h, aec, h->cu_coefs.coef_y, cu, uiBitSize, 1, max_bits);
            }
            return *distortion + lambda * (arienco_bits_written(aec) - bit_pos);
        } else {
            return MAX_COST;
        }
    } else {
        return MAX_COST;
    }
}

// get up and left intra prediction direction and refer to them as MPM candidates (only for mode prediction)
void intra_pu_gen_mpm(avs3_enc_t *h, int *mostProbableMode)
{
    const cfg_param_t *input = h->input;
    int upMode;
    int leftMode;
    int b8x = h->pu_pix_x >> 3;
    int b8y = h->pu_pix_y >> 3;
    int b8_stride = input->b8_stride;
    char_t *p_ipredmode = h->ipredmode + b8y * b8_stride + b8x;

    if (h->pu_pix_y > h->cu_pix_y || h->cu_available_up) {
        upMode = p_ipredmode[-b8_stride];
    } else {
        upMode = 0;
    }
    
    leftMode = p_ipredmode[-1];
    upMode = (upMode < 0) ? DC_PRED : upMode;
    leftMode = (leftMode < 0) ? DC_PRED : leftMode;

    mostProbableMode[0] = min(upMode, leftMode);
    mostProbableMode[1] = max(upMode, leftMode);

    if (mostProbableMode[0] == mostProbableMode[1]) {
        mostProbableMode[0] = DC_PRED;
        mostProbableMode[1] = (mostProbableMode[1] == DC_PRED) ? BI_PRED : mostProbableMode[1];
    }
}

#define CHECK_ONE_INTRA_PRED_MODE(mode, num) {                                                                                                          \
    int uiModebits, uiSad;                                                                                                                              \
    double uicost;                                                                                                                                      \
    pel_t *pred = h->pred_intra_luma[mode];                                                                                                  \
    intra_pred_pu_luma(EP, pred, h->pu_size, mode, avaliable_up, avaliable_left, h->pu_size, h->pu_size, input->bit_depth);                             \
    uiSad = g_funs_handle.cost_blk_satd[stad_bitsize](p_org, CACHE_STRIDE, pred, h->pu_size);                                \
    uiModebits = (mostProbableMode[0] == mode || mostProbableMode[1] == mode) ? 2 : 6;                                                                  \
    uicost = (double)uiSad + (double)uiModebits * lambda;                                                                                               \
    com_update_cand_list(mode, uicost, num, CandModeList, CandCostList);                                                                                \
}

void intra_pu_luma_rmd(avs3_enc_t *h, cu_t *cu, int iNumCandFullRD, pel_t *EP, int avaliable_up, int avaliable_left, double lambda, const int *mostProbableMode, int *CandModeList, double *CandCostList)
{
    int i;
    const cfg_param_t *input = h->input;
    pel_t *p_org = h->p_org[0] + (h->pu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->pu_pix_x - h->lcu_pix_x;
    int fast_rmd = (h->type == B_FRM);
    int j;
    int stad_bitsize = h->cu_bitsize - 2;
    int pRMDTab4Step[9] = { 0, 1, 2, 8, 12, 16, 20, 24, 28 }; // 9 modes
    int NumCand4StepIn = 9;
    int NumCand4StepOut = fast_rmd ? 4 : 6;

    int pRMDTab2Step[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // at most 2*NumCand4StepOut modes
    int NumCand2StepIn = 0;
    int NumCand2StepOut = fast_rmd ? 1 : 2; // two candidates except "0, 1, 2"

    int pRMDTab1Step[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // at most 4 modes
    int NumCand1StepIn = 0;

    int pRMDIdx;

    // 4-step
    for (pRMDIdx = 0; pRMDIdx < NumCand4StepIn; pRMDIdx++) {
        CHECK_ONE_INTRA_PRED_MODE(pRMDTab4Step[pRMDIdx], pRMDIdx);
    }
    if (fast_rmd && CandModeList[0] < 2 && CandModeList[1] < 2) { // only the strong direction needs the further directional mode exploiting.
        return;
    }

    // 2-step
    j = 0;
    for (pRMDIdx = 0; pRMDIdx < NumCand4StepOut; pRMDIdx++){
        int imode4step = CandModeList[pRMDIdx];

        if (imode4step >= 6){
            for (i = 0; i < NumCand2StepIn; i++){
                if (imode4step - 2 == pRMDTab2Step[i]) {
                    break;
                }
            }
            if (i == NumCand2StepIn && imode4step - 2 >= 3){
                pRMDTab2Step[NumCand2StepIn++] = imode4step - 2;
            }
            for (i = 0; i < NumCand2StepIn; i++){
                if (imode4step + 2 == pRMDTab2Step[i]) {
                    break;
                }
            }
            if (i == NumCand2StepIn && imode4step + 2 <= 32){
                pRMDTab2Step[NumCand2StepIn++] = imode4step + 2;
            }
        }
    }
    for (pRMDIdx = 0; pRMDIdx < NumCand2StepIn; pRMDIdx++) {
        CHECK_ONE_INTRA_PRED_MODE(pRMDTab2Step[pRMDIdx], max(NumCand2StepIn + 1, iNumCandFullRD));
    }

    if (SPEED_LEVEL(6, input->speed_level) && CandModeList[0] <= 2) { // only the strong direction needs the further directional mode exploiting.
        return;
    }

    // 1-step
    j = 0;
    for (pRMDIdx = 0; pRMDIdx < NumCand2StepOut;){
        int imode2step = CandModeList[j++];
        if (j >= NUM_MODE_FULL_RD)
            break;
        if (imode2step >= 4){
            pRMDIdx++;
            if (NumCand1StepIn == 0){
                pRMDTab1Step[NumCand1StepIn++] = imode2step - 1;
                pRMDTab1Step[NumCand1StepIn++] = imode2step + 1;
            }
            else{
                for (i = 0; i < NumCand1StepIn; i++){
                    if (imode2step - 1 == pRMDTab1Step[i]) {
                        break;
                    }
                }
                if (i == NumCand1StepIn){
                    pRMDTab1Step[NumCand1StepIn++] = imode2step - 1;
                }
                for (i = 0; i < NumCand1StepIn; i++){
                    if (imode2step + 1 == pRMDTab1Step[i]) {
                        break;
                    }
                }
                if (i == NumCand1StepIn){
                    pRMDTab1Step[NumCand1StepIn++] = imode2step + 1;
                }
            }
        }
    }
    for (pRMDIdx = 0; pRMDIdx < NumCand1StepIn; pRMDIdx++) {
        if (pRMDTab1Step[pRMDIdx] <= 32) {
            CHECK_ONE_INTRA_PRED_MODE(pRMDTab1Step[pRMDIdx], iNumCandFullRD);
        }
    }
}

int intra_pu_luma_rdo(avs3_enc_t *h, cu_t *cu, aec_t *aec, int iNumCandFullRD, int uiBitSize, double lambda, const int *mostProbableMode, const int *CandModeList, double* CandModeCost, int *best_dist, double mcost, double *rdcost)
{
    const cfg_param_t *input = h->input;

    double min_rdcost = mcost;
    coef_t coef_for_intra_luma[MAX_CU_SIZE * MAX_CU_SIZE];
    coef_t *p_cur_coef;
    pel_t rec8x8[MAX_CU_SIZE * MAX_CU_SIZE];

    pel_t *p_rec;
    int best_ipmode = 0;
    int c_nz, nonzero = 1;
    double cost;
    int imode, i, j;
    int distortion;
	int pic_pix_x = h->pu_pix_x;
	int pic_pix_y = h->pu_pix_y;
    int b8x = h->pu_pix_x >> 3;
    int b8y = h->pu_pix_y >> 3;
	int b8s = h->pu_size >> 3;
    int b8_stride = input->b8_stride;
    char_t *p_ipredmode = h->ipredmode + b8y * b8_stride + b8x;

    p_cur_coef = h->cu_coefs.coef_y;
    *best_dist = MAX_COST;

    // full RDO
    for (imode = 0; imode < iNumCandFullRD; imode++) {
        int ipmode = CandModeList[imode];
        cu->ipred_mode_real = (char_t)ipmode;

        if (CandModeCost[imode] > CandModeCost[0] * 2 && SPEED_LEVEL(6, input->speed_level)) {
            break;
        }

        if ((cost = intra_pu_rdcost(h, aec, cu, uiBitSize, &c_nz, ipmode, lambda, mostProbableMode, &distortion, min_rdcost)) < min_rdcost) {
            p_rec = h->p_rec[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
            g_funs_handle.cpy_pel[h->pu_size >> 1](p_rec, CACHE_STRIDE, rec8x8, h->pu_size, h->pu_size, h->pu_size);
            if (c_nz) {
                memcpy(coef_for_intra_luma, p_cur_coef, sizeof(coef_t)* h->tu_q_size * h->tu_q_size);
            }
            *best_dist = distortion;
            nonzero = c_nz;
            min_rdcost = cost;
            best_ipmode = ipmode;
        }
    }

    *rdcost = min_rdcost;

    if (min_rdcost < mcost) {
        for (j = 0; j < b8s; j++) {
            for (i = 0; i < b8s; i++) {
                p_ipredmode[i] = (char_t)best_ipmode;
            }
            p_ipredmode += b8_stride;
        }

        cu->ipred_mode = (char_t)((mostProbableMode[0] == best_ipmode) ? -2 : ((mostProbableMode[1] == best_ipmode) ? -1 : (best_ipmode < mostProbableMode[0] ? best_ipmode : (best_ipmode < mostProbableMode[1] ? best_ipmode - 1 : best_ipmode - 2))));
        cu->ipred_mode_real = (char_t)best_ipmode;

        p_rec = h->p_rec[0] + (pic_pix_y - h->lcu_pix_y) * CACHE_STRIDE + pic_pix_x - h->lcu_pix_x;
        g_funs_handle.cpy_pel[h->pu_size >> 1](rec8x8, h->pu_size, p_rec, CACHE_STRIDE, h->pu_size, h->pu_size);
        if (nonzero) {
            memcpy(p_cur_coef, coef_for_intra_luma, sizeof(coef_t)* h->pu_size * h->pu_size);
        }
        return nonzero;
    } else {
        *best_dist = MAX_COST;
        return 0;
    }
}

/*
*************************************************************************
* Function:Store cu_t parameters
* Input:
* Output:
* Return:
* Attention:
*************************************************************************
*/
void store_cu_best_info(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, int uiBitSize, int mode)
{
    int i, pu_num;
    const cfg_param_t *input = h->input;
    cu_t* bst_cu = &bst->bst_cu;

    *bst_cu = *cu;

    if (B_INTRA(mode) || !mode) {
        pu_num = 1;
    } else {
        pu_num = tab_inter_pu_num[mode];
    }

    for (i = 0; i < pu_num; i++) {
        int b8idx;
        mv_info_t *p_mv_info = bst->mode_mv[i];
        init_pu_pos(h, mode, cu->mdirect_mode, uiBitSize, i);
        b8idx = h->pu_b8_y * input->b8_stride + h->pu_b8_x;

        p_mv_info[0].r = h->p_cur_frm->refbuf[b8idx];
        CP32(p_mv_info[0].mv, h->p_cur_frm->mvbuf[b8idx]);

        p_mv_info[1].r = h->refbuf_bw[b8idx];
        CP32(p_mv_info[1].mv, h->mvbuf_bw[b8idx]);
    }
}

/*
*************************************************************************
* Function:8x8 Intra mode decision for an cu_t
* Input:
* Output:
* Return:
* Attention:
*************************************************************************
*/

int intra_luma_mode_decision(avs3_enc_t *h, cu_t *cu, aec_t *aec, int uiBitSize, double lambda, double min_rdcost, double *iLumaDist)
{
    int imode;
    int mostProbableMode[2];
    double rdcost;
    int cbp = 0;
    int distortion;
    int    CandModeList[NUM_MODE_FULL_RD];
    double CandCostList[NUM_MODE_FULL_RD];

    pel_t edgepixels[1024];
    pel_t *EP = edgepixels + ((1 << uiBitSize) * 2) + 4;
    int block_avaliable_up, block_avaliable_left;

    static tab_char_t iFullRDNumTab[5] = { 3, 3, 2, 2, 1 };
    int iNumCandFullRD = iFullRDNumTab[uiBitSize - 2];
 
    *iLumaDist = 0;
    
    // MPM generation
    intra_pu_gen_mpm(h, mostProbableMode);

    // do intra prediction
    intra_prepare_edge_luma(h, cu, EP, uiBitSize, &block_avaliable_up, &block_avaliable_left);

    // rough mode decision
    for (imode = 0; imode < NUM_MODE_FULL_RD; imode++) {
        CandModeList[imode] = DC_PRED;
        CandCostList[imode] = MAX_COST;
    }
    intra_pu_luma_rmd(h, cu, iNumCandFullRD, EP, block_avaliable_up, block_avaliable_left, lambda, mostProbableMode, CandModeList, CandCostList);

    // full RDO
    if (intra_pu_luma_rdo(h, cu, aec, iNumCandFullRD, uiBitSize, lambda, mostProbableMode, CandModeList, CandCostList, &distortion, min_rdcost, &rdcost)) {
        cbp = 15;
    }
 
    *iLumaDist = distortion;

    return cbp;
}

/** Obtain the ref and mv information for inter prediction according to
*   current mode and direction, and store them in corresponding buffers.
* \param h
* \param cu
* \returns Void
*/
void dump_pu_mv_ref(avs3_enc_t *h, cu_t *cu, int pu_idx, int b8)
{
    const cfg_param_t *input = h->input;
    me_info_t *me = &h->analyzer.me_info;
    int md_direct = cu->mdirect_mode;

    int b8x = h->pu_pix_x  >> 3;
    int b8y = h->pu_pix_y  >> 3;
    int b8_stride = input->b8_stride;

    char_t *fw_ref, *bw_ref;
    i16s_t(*fw_mv)[2], (*bw_mv)[2];

    char_t ref1, ref2;
    i16s_t *mv1, *mv2;
    i16s_t tmv[3] = { 0, 0, 0 };

    int mode = cu->cuType;
    int pdir = cu->b8pdir;
    int bx = b8 % 2;
    int by = b8 / 2;
	int offset = b8y * b8_stride + b8x;
    ref1 = ref2 = 0;
    mv1 = mv2 = tmv;

    if (IS_INTRA(cu)) {
        ref1 = ref2 = -1;
    } else {
        i16s_t *fmv_array;

        /* set ref */
        if (!mode) {
            if (h->type == B_FRM) {
                if (md_direct == DS_BACKWARD) {
                    ref1 = -1;
                }
                if (md_direct == DS_FORWARD) {
                    ref2 = -1;
                }
            } else {
                ref2 = -1;
            }
        } else {
            if (h->type == B_FRM) {
                if (me->best8x8pdir[mode] == FORWARD) {
                    ref2 = -1;
                }
                if (me->best8x8pdir[mode] == BACKWARD) {
                    ref1 = -1;
                }
            } else {
                ref1 = me->best8x8ref[mode];
                ref2 = -1;
            }
        }

        /* set mv */
        fmv_array = (mode ? (pdir == BID ? me->allBidMv : me->allFwMv) : me->allFwMv)[mode][by][bx][ref1];

        if (pdir != BACKWARD) {
            if (md_direct != 0) { // special Skip/Direct mode, first mv
                mv1 = me->skip_fw_mv[md_direct];
            } else { // normal mode, first mv
                mv1 = fmv_array;
            }
        }

        if (h->type == B_FRM && pdir != FORWARD) {
            if (md_direct != 0) { // special B direct
                mv2 = me->skip_bw_mv[md_direct];
            } else { // normal BID
                mv2 = me->allBwMv[mode][by][bx][0];
            }
        }
    }

    fw_ref = h->p_cur_frm->refbuf + offset;
    bw_ref = h->refbuf_bw         + offset;
    fw_mv  = h->p_cur_frm->mvbuf  + offset;
    bw_mv  = h->mvbuf_bw          + offset;

    fw_ref[0] = ref1;
    bw_ref[0] = ref2;

    //CP32(fw_mv[0], mv1);
    //CP32(bw_mv[0], mv2);
    memcpy(fw_mv[0], mv1, 4);
    memcpy(bw_mv[0], mv2, 4);
}

void prepare_cu_inter_info(avs3_enc_t *h, cu_t *cu, int uiBitSize)
{
    me_info_t *me = &h->analyzer.me_info;
    int i;
    int bframe  = (h->type == B_FRM);

    int mode = cu->cuType;
    
    int pu_num;

    if (IS_INTRA(cu)) {
        cu->b8pdir = -1;
    } else if (mode) {
        cu->b8pdir = me->best8x8pdir[mode];
    } else { // Skip or Direct
        if (bframe) {
            int dir = cu->mdirect_mode;
            if (dir == 0) {
                cu->b8pdir = BID;
            } else if (dir == DS_BID) {
                cu->b8pdir = BID;
            } else if (dir == DS_BACKWARD) {
                cu->b8pdir = BACKWARD;
            } else if (dir == DS_FORWARD) {
                cu->b8pdir = FORWARD;
            }
        } else {
            cu->b8pdir = 0;
        }
    } 

    if (!mode) {
        pu_num = 1;
    } else {
        pu_num = tab_inter_pu_num[mode];
    }

    for (i = 0; i < pu_num; i++) {
        init_pu_pos(h, mode, cu->mdirect_mode, uiBitSize, i);
        dump_pu_mv_ref(h, cu, i, tab_inter_pu_2_blk_idx[cu->cuType][i]);
    }

    if (!IS_INTRA(cu) && mode) {
        get_cu_mvd(h, cu, uiBitSize);
    }
}

void analyze_cu_intra_mode(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, aec_t *aec, int uiBitSize, double lambda, double  *min_rdcost)
{
    const cfg_param_t *input = h->input;
    double iLumaDist;
    cu->cuType = I16MB;
    h->pu_pix_x = h->tu_pix_x = h->cu_pix_x;
    h->pu_pix_y = h->tu_pix_y = h->cu_pix_y;
    h->pu_size  = h->tu_size  = 1 << uiBitSize;
    h->tu_q_size = h->tu_size == 64 ? 32 : h->tu_size;

    prepare_cu_inter_info(h, cu, uiBitSize);
    
    cu->trans_size = 0;

    // intra luma mode decision
    cu->cbp = intra_luma_mode_decision(h, cu, aec, uiBitSize, lambda, *min_rdcost, &iLumaDist);

    // intra chroma mode decision
    if (iLumaDist < *min_rdcost) {
        double rdcost = MAX_COST;

        // intra chroma prediction
        intra_pred_chroma(h, uiBitSize - 1, cu->ipred_mode_real, DM_PRED_C);
        cu->ipred_mode_c = DM_PRED_C;

        // calculate the ssd amount of y,u,v
        rdcost = iLumaDist + chroma_residual_coding(h, cu, aec, uiBitSize - 1, 0, 1);

        if (*min_rdcost > rdcost && ((h->type & I_FRM) || rdcost < h->analyzer.cu_threshold * 5)) {
            aec_t cs_tmp;
            copy_coding_state_hdr(&cs_tmp, aec);
            rdcost += write_one_cu_rdo(h, &cs_tmp, cu, uiBitSize, lambda, *min_rdcost - rdcost);
            if (rdcost < *min_rdcost) {
                *min_rdcost = rdcost;
                bst->rd_cost = rdcost;
                store_cu_best_info(h, cu, bst, uiBitSize, I16MB);
            }
        }
    }
}

void inter_rdcost_one_cu(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, aec_t *aec, int uiBitSize, double lambda, int mode, double *mcost, int need_pred_luma)
{
    double rdcost1 = MAX_COST, rdcost2 = MAX_COST;
    double min_cost = *mcost;
    int dist_chroma;
    int cu_size = 1 << uiBitSize;
    int T2Nx2N_cbp = 15;
    int i, pu_num;
    int md_direct = cu->mdirect_mode;

    cu->cuType = mode;
    cu->cbp = 0;

    prepare_cu_inter_info(h, cu, uiBitSize);

    if (!mode) {
        pu_num = 1;
    } else {
        pu_num = tab_inter_pu_num[mode];
    }
    for (i = 0; i < pu_num; i++) {
        init_pu_inter_pos(h, mode, cu->mdirect_mode, uiBitSize, i);
        inter_pred_one_pu(h, cu, i, need_pred_luma);
    }
 
    dist_chroma = chroma_residual_coding(h, cu, aec, uiBitSize - 1, 0, 1);

    if (dist_chroma < min_cost) {
        aec_t cs_tmp;

        // 2Nx2N Transform Unit
        cu->trans_size = 0;
        rdcost1 = dist_chroma + inter_luma_coding(h, cu, aec, uiBitSize, 0, 1);
        T2Nx2N_cbp = cu->cbp;

        if (rdcost1 < min_cost) {
            copy_coding_state_hdr(&cs_tmp, aec);
            rdcost1 += write_one_cu_rdo(h, &cs_tmp, cu, uiBitSize, lambda, min_cost - rdcost1);
            min_cost = min(min_cost, rdcost1);
        }
         
        // NxN Transform Unit 
        if (h->type == B_FRM && (T2Nx2N_cbp & 0xF) && uiBitSize > 3 && 0) {
            cu->trans_size = 1;
            rdcost2 = dist_chroma + inter_luma_coding(h, cu, aec, uiBitSize, 0, 1);

            if (rdcost2 < min_cost) {
                copy_coding_state_hdr(&cs_tmp, aec);
                rdcost2 += write_one_cu_rdo(h, &cs_tmp, cu, uiBitSize, lambda, min_cost - rdcost2);
                min_cost = min(min_cost, rdcost2);
            }
        }

        // All Zero
        if (cu->cbp && T2Nx2N_cbp && (!mode || h->type == B_FRM)) {
            int cu_c_size = cu_size >> 1;
            int org_offset = (h->cu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->cu_pix_x - h->lcu_pix_x;
            double rdcost3 = g_funs_handle.cost_ssd[uiBitSize - 2](h->p_org[0] + org_offset, CACHE_STRIDE, (mode ? h->pred_inter_luma : h->pred_inter_luma_skip[md_direct]), cu_size, cu_size);

            org_offset >>= 1;
            rdcost3 += g_funs_handle.cost_ssd[uiBitSize - 3](h->p_org[1] + org_offset, CACHE_STRIDE, (mode ? h->pred_inter_chroma[0] : h->pred_inter_chroma_skip[md_direct][0]), cu_c_size, cu_c_size);
            rdcost3 += g_funs_handle.cost_ssd[uiBitSize - 3](h->p_org[2] + org_offset, CACHE_STRIDE, (mode ? h->pred_inter_chroma[1] : h->pred_inter_chroma_skip[md_direct][1]), cu_c_size, cu_c_size);

            if (rdcost3 < min_cost) {
                int cbp_bak = cu->cbp;
                cu->cbp = 0;

                copy_coding_state_hdr(&cs_tmp, aec);
                rdcost3 += write_one_cu_rdo(h, &cs_tmp, cu, uiBitSize, lambda, min_cost - rdcost3);

                if (rdcost3 < min_cost) {
                    min_cost = rdcost3;
                    chroma_residual_coding(h, cu, aec, uiBitSize - 1, 1, 0);
                    inter_luma_coding(h, cu, aec, uiBitSize, 1, 0);
                } else if (min_cost < *mcost && rdcost2 < rdcost1) {
                    cu->cbp = cbp_bak;
                }
            }
        }

        if (min_cost < *mcost) {
            *mcost = min_cost;
            bst->rd_cost = min_cost;

            if (rdcost1 == min_cost) {
                cu->trans_size = 0;
                cu->cbp = T2Nx2N_cbp;
                store_cu_best_info(h, cu, bst, uiBitSize, mode);
            } else if (rdcost2 == min_cost) {
                cu->trans_size = (cu->cbp & 0xF) ? 1 : 0;
                store_cu_best_info(h, cu, bst, uiBitSize, mode);
            } else {
                cu->trans_size = 0;
                store_cu_best_info(h, cu, bst, uiBitSize, mode);
            }
        }
    }
}

void inter_rdcost_one_cu_allzero(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, aec_t *aec, int uiBitSize, double lambda, int mode, double *mcost, int need_pred_luma)
{
    double min_cost = *mcost;
    int cu_size = 1 << uiBitSize;
    int i, pu_num;
    int md_direct = cu->mdirect_mode;

    cu->cuType = mode;
    cu->cbp = 0;
    cu->trans_size = 0;

    prepare_cu_inter_info(h, cu, uiBitSize);

    if (!mode) {
        pu_num = 1;
    } else {
        pu_num = tab_inter_pu_num[mode];
    }
    for (i = 0; i < pu_num; i++) {
        init_pu_inter_pos(h, mode, cu->mdirect_mode, uiBitSize, i);
        inter_pred_one_pu(h, cu, i, need_pred_luma);
    }
  
    if (chroma_residual_coding(h, cu, aec, uiBitSize - 1, 1, 1) < min_cost) {
        aec_t cs_tmp;
        int cu_c_size = cu_size >> 1;
        int org_offset = (h->cu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->cu_pix_x - h->lcu_pix_x;
        double rdcost = g_funs_handle.cost_ssd[uiBitSize - 2](h->p_org[0] + org_offset, CACHE_STRIDE, (mode ? h->pred_inter_luma : h->pred_inter_luma_skip[md_direct]), cu_size, cu_size);

        org_offset >>= 1;
        rdcost += g_funs_handle.cost_ssd[uiBitSize - 3](h->p_org[1] + org_offset, CACHE_STRIDE, (mode ? h->pred_inter_chroma[0] : h->pred_inter_chroma_skip[md_direct][0]), cu_c_size, cu_c_size);
        rdcost += g_funs_handle.cost_ssd[uiBitSize - 3](h->p_org[2] + org_offset, CACHE_STRIDE, (mode ? h->pred_inter_chroma[1] : h->pred_inter_chroma_skip[md_direct][1]), cu_c_size, cu_c_size);

        if (rdcost < min_cost) {
            copy_coding_state_hdr(&cs_tmp, aec);
            rdcost += write_one_cu_rdo(h, &cs_tmp, cu, uiBitSize, lambda, min_cost - rdcost);

            if (rdcost < min_cost) {
                *mcost = rdcost;
                bst->rd_cost = rdcost;
                inter_luma_coding(h, cu, aec, uiBitSize, 1, 0);
                store_cu_best_info(h, cu, bst, uiBitSize, mode);
            }
        }
    }
}

int inter_check_skip(avs3_enc_t *h, cu_t *cu, int uiBitSize)
{
    int cu_size = 1 << uiBitSize;
    int org_offset = (h->cu_pix_y - h->lcu_pix_y) * CACHE_STRIDE + h->cu_pix_x - h->lcu_pix_x;
    int md_direct = cu->mdirect_mode;
    pel_t *pred = h->pred_inter_luma_skip[md_direct];

    cu->cuType = 0;

    prepare_cu_inter_info(h, cu, uiBitSize);
    init_pu_inter_pos(h, 0, cu->mdirect_mode, uiBitSize, 0);
    inter_pred_one_pu_luma(h, cu, pred, 0);

    return g_funs_handle.cost_blk_satd[uiBitSize - 2](h->p_org[0] + org_offset, CACHE_STRIDE, pred, cu_size);
}

/*
*************************************************************************
* Function:Set stored cu_t parameters
* Input:
* Output:
* Return:
* Attention:
*************************************************************************
*/
void revert_large_cu_param(avs3_enc_t *h, best_mode_t *bst, int uiBitSize, unsigned int uiPositionInPic)
{
    const cfg_param_t *input = h->input;
    int    i, j, k;
    cu_t  *bst_cu = &bst->bst_cu;
    int    mode = bst_cu->cuType;
	int    b8x = uiPositionInPic % input->pic_width_in_mcu;
	int    b8y = uiPositionInPic / input->pic_width_in_mcu;
    int    pix_x = b8x << 3;
    int    pix_y = b8y << 3;
    int    pix_x_in_lcu = h->cu_pix_x - h->lcu_pix_x;
    int    pix_y_in_lcu = h->cu_pix_y - h->lcu_pix_y;

    cu_t *tmpMB;

    int WidthInLCU = ((input->img_width >> LCU_SIZE_IN_BITS) + (input->img_width % LCU_SIZE ? 1 : 0));
    int currSMB_nr = (pix_y >> LCU_SIZE_IN_BITS) * WidthInLCU + (pix_x >> LCU_SIZE_IN_BITS);

    pel_t *rec, *recu, *recv;
    int b8_stride = input->b8_stride;
    char_t *p_ipredmode;
    char_t *fw_ref, *bw_ref;
    char_t *pdir, *psize;
    int pu_num;
    int cu_size = 1 << uiBitSize;
    int val_psize = cu_size;
	int b8s = 1 << (uiBitSize - 3);

    //===== reconstruction values =====
    if (bst_cu->cbp & 0xF) {
        coef_t *p_coef = h->all_quant_coefs[currSMB_nr].coef_y + (tab_b8_xy_cvto_zigzag[pix_y_in_lcu >> 3][pix_x_in_lcu >> 3] << 6);
        memcpy(p_coef, bst->coef_y, cu_size * cu_size * sizeof(coef_t));
    }
    
    rec = h->p_rec[0] + (pix_y - h->lcu_pix_y) * CACHE_STRIDE + pix_x - h->lcu_pix_x;
    g_funs_handle.cpy_pel[cu_size >> 1](bst->rec_y, cu_size, rec, CACHE_STRIDE, cu_size, cu_size);

    cu_size >>= 1;

    if (bst_cu->cbp & 0x10) {
        coef_t *p_coef = h->all_quant_coefs[currSMB_nr].coef_u + (tab_b8_xy_cvto_zigzag[pix_y_in_lcu >> 3][pix_x_in_lcu >> 3] << 4);
        memcpy(p_coef, bst->coef_u, cu_size * cu_size * sizeof(coef_t));
    }
    if (bst_cu->cbp & 0x20) {
        coef_t *p_coef = h->all_quant_coefs[currSMB_nr].coef_v + (tab_b8_xy_cvto_zigzag[pix_y_in_lcu >> 3][pix_x_in_lcu >> 3] << 4);
        memcpy(p_coef, bst->coef_v, cu_size * cu_size * sizeof(coef_t));
    }

    recu = h->p_rec[1] + (((pix_y - h->lcu_pix_y) * CACHE_STRIDE + pix_x - h->lcu_pix_x) >> 1);
    recv = h->p_rec[2] + (((pix_y - h->lcu_pix_y) * CACHE_STRIDE + pix_x - h->lcu_pix_x) >> 1);
    g_funs_handle.cpy_pel[cu_size >> 1](bst->rec_u, cu_size, recu, CACHE_STRIDE, cu_size, cu_size);
    g_funs_handle.cpy_pel[cu_size >> 1](bst->rec_v, cu_size, recv, CACHE_STRIDE, cu_size, cu_size);

    //===============   cbp and mode   ===============
    tmpMB = &h->cu_data[uiPositionInPic];

    for (j = 0; j < (1 << (uiBitSize - 3)); j++) {     //uiBitSize 4:1 5:2x2 6 4x4
        for (i = 0; i < (1 << (uiBitSize - 3)); i++) {
            tmpMB[i] = *bst_cu;
        }
        tmpMB += input->pic_width_in_mcu;
    }

    if (B_INTRA(mode)) {
        char_t ipred = bst_cu->ipred_mode_real;
		int offset = b8y * b8_stride + b8x;

        p_ipredmode = h->ipredmode + b8y * b8_stride + b8x;
        for (j = 0; j < b8s - 1; j++) {
            p_ipredmode[b8s - 1] = ipred;
            p_ipredmode += b8_stride;
        }
        for (i = 0; i < b8s; i++) {
            p_ipredmode[i] = ipred;
        }
         
        fw_ref = h->p_cur_frm->refbuf + offset;
        bw_ref = h->refbuf_bw         + offset;
        pdir   = h->pdir              + offset;
        psize  = h->size              + offset;

        for (j = 0; j < b8s; j++) {
            for (i = 0; i < b8s; i++) {
                fw_ref[i] = -1;
                bw_ref[i] = -1;
                pdir  [i] = -1;
                psize [i] = val_psize;
            }
            fw_ref += b8_stride;
            bw_ref += b8_stride;
            pdir   += b8_stride;
            psize  += b8_stride; 
        }
    } else {
        char_t pdir_val = bst_cu->b8pdir;

        p_ipredmode = h->ipredmode + b8y * b8_stride + b8x;
        for (j = 0; j < b8s - 1; j++) {
            p_ipredmode[b8s - 1] = -1;
            p_ipredmode += b8_stride;
        }
        for (i = 0; i < b8s; i++) {
            p_ipredmode[i] = -1;
        }

        pu_num = !mode ? 1 : tab_inter_pu_num[mode];

        for (k = 0; k < pu_num; k++) {
            int offset;
            i16s_t(*fw_mv)[2], (*bw_mv)[2];
            mv_info_t *fw_info = &bst->mode_mv[k][0];
            mv_info_t *bw_info = &bst->mode_mv[k][1];
            

            init_pu_pos(h, mode, bst_cu->mdirect_mode, uiBitSize, k);

			offset = h->pu_b8_y * b8_stride + h->pu_b8_x;

            fw_ref = h->p_cur_frm->refbuf + offset;
            fw_mv  = h->p_cur_frm->mvbuf  + offset;
            bw_ref = h->refbuf_bw         + offset;
            bw_mv  = h->mvbuf_bw          + offset;
            pdir   = h->pdir              + offset;
            psize  = h->size              + offset;

            b8s = h->pu_size >> 3;

            for (j = 0; j < b8s; j++) {
                for (i = 0; i < b8s; i++) {
                    pdir[i] = pdir_val;
                    psize[i] = val_psize;
                    fw_ref[i] = fw_info->r;
                    CP32(fw_mv[i], fw_info->mv);

                    bw_ref[i] = bw_info->r;
                    CP32(bw_mv[i], bw_info->mv);
                }
                fw_mv  += b8_stride;
                bw_mv  += b8_stride;
                fw_ref += b8_stride;
                bw_ref += b8_stride;
                pdir   += b8_stride;
                psize  += b8_stride;
            }
        }
        if (mode) {
            memcpy(h->cu_data[uiPositionInPic].mvd, bst_cu->mvd, sizeof(bst_cu->mvd[0]) * pu_num);
        }
    }
}

int motion_estimation(avs3_enc_t *h, me_info_t *me, int uiBitSize, int mode)  
{
    int satd_cost = 0;
    int fw_mcost, bw_mcost = MAX_COST;
    int ref, mcost;
    int best_fw_ref = 0, best_pdir = FORWARD;

    h->analyzer.me_method = (uiBitSize == 3) ? IME_HEX : IME_TZ;

    if (SPEED_LEVEL(6, h->input->speed_level)) {
        me->b_fast_fme = 1;
    } else {
        me->b_fast_fme = (h->curr_RPS.layer > 1 || uiBitSize == 3);
    }

    init_pu_inter_pos(h, mode, 0, uiBitSize, 0);

    if (h->type == B_FRM) {
        bw_mcost = pu_motion_search(h, me, -1, uiBitSize);
        fw_mcost = pu_motion_search(h, me,  0, uiBitSize);
        best_fw_ref = 0;
    } else {
        int min_mcost = MAX_COST;
        int max_ref = h->refs_num;

        for (ref = 0; ref < max_ref; ref++) {
            mcost = pu_motion_search(h, me, ref, uiBitSize) + (h->refs_num > 1 ? REF_COST_FWD(ref) : 0);

            if (mcost < min_mcost) {
                min_mcost = mcost;
                best_fw_ref = ref;
            }
        }
        fw_mcost = min_mcost;
    }
    
    if (h->type == B_FRM) {
        int bid_mcost = MAX_COST;
        if (uiBitSize > 3 || mode == P2NX2N) {
            pu_motion_search_b(h, me, mode, uiBitSize, &bid_mcost, min(fw_mcost, bw_mcost));
        }
        if (fw_mcost <= bw_mcost && fw_mcost <= bid_mcost) {
            best_pdir = FORWARD;
            satd_cost += fw_mcost;
        } else if (bw_mcost <= fw_mcost && bw_mcost <= bid_mcost) {
            best_pdir = BACKWARD;
            satd_cost += bw_mcost;
            best_fw_ref = 0;
        } else {
            best_pdir = BID;
            satd_cost += bid_mcost;
        }
    } else {
        best_pdir = FORWARD;
        satd_cost += fw_mcost;
    }
    me->best8x8ref    [mode]    = (char_t)best_fw_ref;
    me->best8x8pdir   [mode]    = (char_t)best_pdir;

    return satd_cost;
}

double analyze_cu_skip_mode(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, aec_t *aec, int uiBitSize, double *skip_satd)
{
    analyzer_t *a = &h->analyzer;
    int dir;
    double mcost = MAX_COST;

    if (h->type == B_FRM) {
        int bst_dir;
        int satd, min_satd = MAX_COST;

        get_col_bskip_mv(h, uiBitSize);
        get_mhp_bskip_mv(h, uiBitSize);

        for (dir = 0; dir < 4; dir++) {
            cu->mdirect_mode = dir;
            satd = inter_check_skip(h, cu, uiBitSize);
            if (satd < min_satd) {
                min_satd = satd;
                bst_dir = dir;
            }
        }
        *skip_satd = satd;
        cu->mdirect_mode = bst_dir;
        inter_rdcost_one_cu(h, cu, bst, aec, uiBitSize, a->lambda_mode, 0, &mcost, 0);
    } else {
        *skip_satd = MAX_COST;
        get_col_pskip_mv(h, uiBitSize);
        inter_rdcost_one_cu(h, cu, bst, aec, uiBitSize, a->lambda_mode, 0, &mcost, 1);
    }

    return mcost;
}

void analyze_cu_inter_mode(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, aec_t *aec, int mode, double *min_rdcost, int uiBitSize, int force_all_zero)
{
    const cfg_param_t *input = h->input;
    analyzer_t *a = &h->analyzer;
    me_info_t *me = &a->me_info;
    cu_t *bst_cu = &bst->bst_cu;

    cu->ipred_mode_c = DC_PRED_C;
    cu->mdirect_mode = 0;

    if (force_all_zero) {
        inter_rdcost_one_cu_allzero(h, cu, bst, aec, uiBitSize, a->lambda_mode, mode, min_rdcost, 1);
    } else {
        inter_rdcost_one_cu(h, cu, bst, aec, uiBitSize, a->lambda_mode, mode, min_rdcost, 1);
    }
}


void statistic_cu_vertds(avs3_enc_t *h, int uiBitSize, int iDownScale, int pix_y, int pix_x, double *var_2Nx2N, double *var_ave, double *var_var, int stat_mask)
{
    double ave_2Nx2N = 0, ave_NxN[4], var_NxN[4];
    double tmpAveVar = 0;
    int blk;

    double *ave = ave_NxN;
    double *var = var_NxN;

    pel_t *p_org;
    int width, height, widhig;
    int bit_depth = h->input->bit_depth;

    width = 1 << uiBitSize;
    height = 1 << uiBitSize;
    widhig = width*(height / iDownScale); // only downscale height
    p_org = h->p_org[0] + (pix_y - h->lcu_pix_y) * CACHE_STRIDE + pix_x - h->lcu_pix_x;

    // average (2Nx2N)
    if (stat_mask & 0x3){
        ave_2Nx2N = g_funs_handle.calc_blk_ave(p_org, width, height, iDownScale, bit_depth);

        // variance (2Nx2N)
        if (stat_mask & 0x1){
            *var_2Nx2N = g_funs_handle.calc_blk_var(p_org, width, height, iDownScale, ave_2Nx2N, bit_depth);
        }
    }

    if (iDownScale > 1){
        iDownScale = iDownScale / 2;
    }
    width = 1 << (uiBitSize - 1);
    height = 1 << (uiBitSize - 1);
    widhig = ((width)*(height / iDownScale));

    // averages (4 parts)
    if (stat_mask & 0x6){
        for (blk = 0; blk < 4; blk++){
            int blkx = blk % 2;
            int blky = blk / 2;
            p_org = h->p_org[0] + (pix_y - h->lcu_pix_y + (blky * width)) * CACHE_STRIDE + pix_x - h->lcu_pix_x + (blkx * width);

            // SSE version
            ave[blk] = g_funs_handle.calc_blk_ave(p_org, width, height, iDownScale, bit_depth);
        }
    }

    if (stat_mask & 0x2){
        // average's variance
        for (blk = 0; blk < 4; blk++){
            (*var_ave) += (ave[blk] - ave_2Nx2N)*(ave[blk] - ave_2Nx2N);
        }
        (*var_ave) /= 4;
    }

    if (stat_mask & 0x4){
        // variance (4 parts)
        for (blk = 0; blk < 4; blk++){
            int blkx = blk % 2;
            int blky = blk / 2;
            p_org = h->p_org[0] + (pix_y - h->lcu_pix_y + (blky * width)) * CACHE_STRIDE + pix_x - h->lcu_pix_x + (blkx * width);
            var[blk] = g_funs_handle.calc_blk_var(p_org, width, height, iDownScale, ave[blk], bit_depth);
            tmpAveVar += var[blk];
        }
        tmpAveVar /= 4;

        // variance's variance
        for (blk = 0; blk < 4; blk++){
            (*var_var) += (var[blk] - tmpAveVar)*(var[blk] - tmpAveVar);
        }
        (*var_var) /= 4;
    }
}

void intra_depth_early_skip(avs3_enc_t *h, int uiBitSize, int *uiMaxBitSize, int *uiMinBitSize, int out_of_pic, int pix_y_InPic_start, int pix_x_InPic_start, double *r_var_var, int QpI)
{
    double var_2Nx2N = 0;
    double var_ave_NxN = 0;
    double var_var_NxN = 0;
    int iDownScale;

    if (*uiMaxBitSize <= *uiMinBitSize) { // to avoid skipping all CU sizes
        return;
    }

    if (!out_of_pic){
        int qpidx = Clip3(1, 52, QpI) - 1;
        int img_width = h->input->img_width;
        if (uiBitSize == 6){
            // conduct decision
            double var_var_th;

            // 52 Qp points
            static const double var_var_ths_clsAB[52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.36142, 240.939, 486.554, 743.891, 1018.63, 1316.47, 1643.08, 2004.15, 2405.37, 2852.41, 3350.97, 3906.73, 4525.37, 5212.58, 5974.04, 6815.43, 7742.45, 8760.77, 9876.08, 11094.1, 12420.4, 13860.8, 15420.9, 17106.4, 18923.1, 20876.5 }; // more conservative thresholds
            static const double var_var_ths_clsFCD[52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.071655, 59.0982, 79.1279, 79.1279, 79.1279, 79.1279, 79.1279, 79.1279, 79.1279, 79.1279, 79.1279, 93.4319, 253.125, 479.747, 782.929, 1172.3, 1657.49, 2248.13, 2953.84, 3784.26, 4749.02, 5857.75, 7120.07, 8545.62, 10144, 11924.9 };

            iDownScale = 4;
            statistic_cu_vertds(h, uiBitSize, iDownScale, pix_y_InPic_start, pix_x_InPic_start, &var_2Nx2N, &var_ave_NxN, &var_var_NxN, 0x4);

            if (img_width > 1600){
                var_var_th = var_var_ths_clsAB[qpidx];
            } else{
                var_var_th = var_var_ths_clsFCD[qpidx];
            }

            if (var_var_NxN > var_var_th) {
                *uiMaxBitSize = 5;
            }
        } else if (uiBitSize == 5){
            // conduct decision
            double var_var_th;

            // 52 Qp points
            static const double var_var_ths_clsAB[52] = { 19607.6, 19878.3, 20179, 20513.2, 20884.4, 21297, 21755.3, 22264.6, 22830.5, 23459.3, 24157.9, 24934.2, 25796.7, 26755, 27819.9, 29003, 30317.6, 31778.3, 33401.2, 35204.5, 37208.2, 39434.4, 41908.1, 44656.6, 47710.5, 51103.7, 54873.9, 59063, 62844.3, 66250.8, 69315.5, 72071.3, 74551.4, 76788.6, 78816.2, 80666.9, 82374, 83970.3, 85488.9, 86962.8, 88425.1, 89908.7, 91446.6, 93071.9, 94817.6, 96716.7, 98802.2, 101107, 103664, 106507, 109668, 113181 };
            static const double var_var_ths_clsFCD[52] = { 2476.9, 2505.84, 2537.99, 2573.72, 2613.42, 2657.53, 2706.54, 2761, 2821.51, 2888.74, 2963.44, 3046.44, 3138.66, 3241.13, 3354.99, 3481.5, 3622.06, 3778.24, 3951.77, 4144.59, 4358.83, 4596.87, 4861.37, 5155.25, 5481.78, 5844.6, 6247.73, 6695.65, 7137.9, 7574.47, 8005.37, 8430.6, 8620.48, 8896.56, 9258.83, 9707.29, 10242, 10862.8, 11891.4, 13098, 14482.7, 16045.5, 17786.3, 19705.2, 21802.1, 24077.1, 26530.2, 29161.3, 31970.5, 34957.7, 38123, 41466.3 };

            iDownScale = 2;
            statistic_cu_vertds(h, uiBitSize, iDownScale, pix_y_InPic_start, pix_x_InPic_start, &var_2Nx2N, &var_ave_NxN, &var_var_NxN, 0x4);

            if (img_width > 1600){
                var_var_th = var_var_ths_clsAB[qpidx];
            } else{
                var_var_th = var_var_ths_clsFCD[qpidx];
            }

            if (var_var_NxN > var_var_th){
                *uiMaxBitSize = 4;
            }
        } else if (uiBitSize == 4){
            if (img_width < 1000) { // only skip 16x16 CUs for small videos
                // conduct decision
                // four Qp points
                //double var_var_ths[4] = { 26389.5, 46740.8, 88585.9, 183965 };

                // 52 Qp points
                static const double var_var_ths[52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252.639, 913.172, 1647.1, 2462.57, 3368.65, 4375.41, 5494.03, 6736.93, 8117.94, 9652.4, 11357.4, 13251.7, 15356.6, 17695.4, 20294, 23181.3, 26389.5, 29954.1, 33689, 37676.7, 41999.8, 46740.8, 51982.3, 57806.7, 64296.8, 71535, 79603.8, 88585.9, 98563.7, 109620, 121837, 135297, 150084, 166279, 183965, 203225, 224141, 246795, 271271, 297651, 326017, 356452 };

                iDownScale = 2;
                statistic_cu_vertds(h, uiBitSize, iDownScale, pix_y_InPic_start, pix_x_InPic_start, &var_2Nx2N, &var_ave_NxN, &var_var_NxN, 0x4);

                if (var_var_NxN > var_var_ths[qpidx]){
                    *uiMaxBitSize = 3;
                }
            }
            //else{ // transform var_var_NxN of large videos to pruning operation
            //    iDownScale = 2;
            //    statistic_cu_vertds(h, uiBitSize, iDownScale, pix_y_InPic_start, pix_x_InPic_start, &var_2Nx2N, &var_ave_NxN, &var_var_NxN, 0x6);
            //}
        }
        *r_var_var = var_var_NxN;
    }
}

void intra_depth_early_prune(avs3_enc_t *h, int uiBitSize, int *uiMaxBitSize, int *uiMinBitSize, int out_of_pic, double var_var_NxN, double rdcost, int QpI)
{
    if (*uiMaxBitSize <= *uiMinBitSize) // to avoid skipping all CU sizes
        return;

    if (!out_of_pic){
        int qpidx = Clip3(1, 52, QpI) - 1;

        int img_width = h->input->img_width;
        if (uiBitSize == 5){
            // conduct decision
            double var_var_th;

            // 52 Qp points
            static const double var_var_ths_clsAB[52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.743985, 9.68235, 19.6139, 30.6489, 42.91, 53.5645, 63.1399, 72.1639, 81.1643, 90.6686, 101.205, 113.3, 127.482, 144.279, 164.218, 187.827, 215.634, 248.166, 285.951, 329.516, 379.39, 436.1, 500.173, 572.138, 652.522, 741.852, 840.657, 949.463, 1068.8 };
            static const double var_var_ths_clsFCD[52] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0819724, 2.68857, 5.58479, 5.58479, 5.58479, 5.58479, 5.58479, 21.6749, 54.7645, 99.8834, 157.92, 229.762, 316.298, 418.415, 537.002, 672.947, 827.139, 1000.46, 1193.81, 1408.07, 1644.13, 1902.87, 2185.19, 2491.97, 2824.1, 3182.47, 3567.96, 3981.47 };

            if (img_width > 1600){
                var_var_th = var_var_ths_clsAB[qpidx];
                if (var_var_NxN < var_var_th * 2) {
                    *uiMinBitSize = 5;
                }
            } else{
                //rdcost_th = rdcost_ths_clsFCD[qpidx];
                var_var_th = var_var_ths_clsFCD[qpidx];
                //if (var_var_NxN < var_var_th || rdcost < rdcost_th) {
                if (var_var_NxN < var_var_th * 2) {
                    *uiMinBitSize = 5;
                }
            }
        } else if (uiBitSize == 4){
            // conduct decision
            double rdcost_th;

            // 52 Qp points
            static const double rdcost_ths_clsAB[52] = { 411.031, 419.602, 429.125, 439.707, 451.464, 464.528, 479.043, 495.171, 513.091, 533.002, 555.125, 579.706, 607.019, 637.367, 671.086, 708.552, 750.181, 796.436, 847.83, 904.934, 968.383, 1038.88, 1117.21, 1204.25, 1300.96, 1408.41, 1527.8, 1660.46, 1811.23, 1983.09, 2179.02, 2402, 2655, 2941, 3262.97, 3623.9, 4026.76, 4474.52, 4970.17, 5516.67, 6117.02, 6774.18, 7491.12, 8270.84, 9116.3, 10030.5, 11016.4, 12076.9, 13215.1, 14434, 15736.4, 17125.4 };
            static const double rdcost_ths_clsF[52] = { 202.886, 210.2, 218.327, 227.356, 237.389, 248.536, 260.922, 274.684, 289.975, 306.966, 325.844, 346.82, 370.126, 396.022, 424.795, 456.766, 492.288, 531.758, 575.613, 624.341, 678.483, 738.64, 805.482, 879.751, 962.272, 1053.96, 1155.84, 1269.04, 1384.59, 1508.1, 1645.17, 1801.39, 1982.36, 2193.68, 2440.96, 2729.77, 3065.74, 3454.45, 3901.5, 4412.5, 4993.04, 5648.71, 6385.13, 7207.88, 8122.56, 9134.78, 10250.1, 11474.2, 12812.6, 14271, 15854.8, 17569.8 };

            if (img_width > 1600){
                //var_var_th = var_var_ths_clsAB[qpidx];
                rdcost_th = rdcost_ths_clsAB[qpidx];
                //if (var_var_NxN < var_var_th || rdcost < rdcost_th) {
                if (rdcost < rdcost_th * 2) {
                    *uiMinBitSize = 4;
                }
            }
            else{
                rdcost_th = rdcost_ths_clsF[qpidx];
                if (rdcost < rdcost_th * 2) {
                    *uiMinBitSize = 4;
                }
            }
        }
    }
}

avs3_no_inline double analyze_one_cu_pb_frm(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, aec_t *aec, int uiBitSize)
{
    analyzer_t *a = &h->analyzer;
    double min_rdcost = MAX_COST;
    const cfg_param_t *input = h->input;
    double inter_satd;
    double skip_satd;
    int force_zero;

    cu->mdirect_mode = 0;
    cu->ipred_mode_c = DC_PRED_C; 

    min_rdcost = analyze_cu_skip_mode(h, cu, bst, aec, uiBitSize, &skip_satd);

    if (SPEED_LEVEL(6, input->speed_level) && min_rdcost < a->cu_threshold * 0.7 && bst->bst_cu.cbp == 0) {
        return min_rdcost;
    }

    inter_satd = motion_estimation(h, &a->me_info, uiBitSize, P2NX2N);

    if (!SPEED_LEVEL(6, input->speed_level) || inter_satd + a->lambda_mode < skip_satd) {
        force_zero = (bst->bst_cu.cbp & 0xF) == 0;
        analyze_cu_inter_mode(h, cu, bst, aec, P2NX2N, &min_rdcost, uiBitSize, force_zero);
    }

    if (h->curr_RPS.referd_by_others && bst->bst_cu.cbp) {
        if (uiBitSize == 3 || !SPEED_LEVEL(6, input->speed_level)) {
            analyze_cu_intra_mode(h, cu, bst, aec, uiBitSize, a->lambda_mode, &min_rdcost);
        } else if (h->curr_RPS.layer < 2 || uiBitSize == 4) {
            double var_var_NxN = 0;
            double threshold = (uiBitSize == 4 ? 41371 : 7900);

            statistic_cu_vertds(h, uiBitSize, 1, h->cu_pix_y, h->cu_pix_x, NULL, NULL, &var_var_NxN, 0x4);

            if (var_var_NxN < threshold) {
                analyze_cu_intra_mode(h, cu, bst, aec, uiBitSize, a->lambda_mode, &min_rdcost);
            }
        }
    }
    return min_rdcost;
}

double analyze_one_cu_i_frm(avs3_enc_t *h, cu_t *cu, best_mode_t *bst, aec_t *aec, int uiBitSize, double min_rdcost)
{
    analyzer_t *a = &h->analyzer;
    analyze_cu_intra_mode(h, cu, bst, aec, uiBitSize, a->lambda_mode, &min_rdcost);
    return min_rdcost;
}

double encode_one_cu(avs3_enc_t *h, best_mode_t *bst, aec_t *aec, int uiBitSize, double *est_bits)
{
    cu_t* cu = &bst->bst_cu;
    int mode = cu->cuType;
    analyzer_t *a = &h->analyzer;
    double cost = 0;
    int cu_size = 1 << uiBitSize;

    int pix_x   = h->cu_pix_x;
    int pix_c_x = pix_x >> 1;
    int pix_y   = h->cu_pix_y;
    int pix_c_y = pix_y >> 1;
    pel_t *rec, *recu, *recv;
    int back_cbp = cu->cbp;
    int i;
    double bits_cost;

    h->analyzer.use_rdoq = 1; // RDOQ is enabled for encoding final parameters 

    cu->cbp = 0;

    if (B_INTRA(mode)) {
        int ipredmode, nz;
        pel_t edgepixels[1024];
        pel_t *EP = edgepixels + ((1 << (uiBitSize + 1)) * 2) + 4;

        ipredmode = cu->ipred_mode_real;
        cu->trans_size = 0;
        h->pu_pix_x = h->tu_pix_x = h->cu_pix_x;
        h->pu_pix_y = h->tu_pix_y = h->cu_pix_y;
        h->pu_size  = h->tu_size  = 1 << uiBitSize;
        h->tu_q_size = h->tu_size == 64 ? 32 : h->tu_size;

        h->tu_available_left = h->cu_pix_x > 0;
        h->tu_available_up = h->cu_available_up;

        cost += intra_enc_pu_luma(h, cu, aec, uiBitSize, &nz, ipredmode);

        if (nz) {
            cu->cbp = 15;
        }

        cost += chroma_residual_coding(h, cu, aec, uiBitSize - 1, 0, 1);
        bits_cost = write_one_cu_rdo(h, aec, cu, uiBitSize, a->lambda_mode, MAX_COST);
        cost += bits_cost;
    } else {
        int pu_num;
        int force_allzero = (back_cbp == 0 ? 1 : 0);
        int b8_stride = h->input->b8_stride;

        if (!mode) {
            pu_num = 1;
        } else {
            pu_num = tab_inter_pu_num[mode];
        }

        for (i = 0; i < pu_num; i++) {
            int b8idx;
            mv_info_t *p_mv_info = bst->mode_mv[i];

            init_pu_inter_pos(h, mode, cu->mdirect_mode, uiBitSize, i);

            b8idx = (h->pu_pix_y >> 3) * b8_stride + (h->pu_pix_x >> 3);

            h->p_cur_frm->refbuf[b8idx] = p_mv_info[0].r;
            h->refbuf_bw        [b8idx] = p_mv_info[1].r;

            CP32(h->p_cur_frm->mvbuf[b8idx], p_mv_info[0].mv);
            CP32(h->mvbuf_bw        [b8idx], p_mv_info[1].mv);
        }

        cost += inter_luma_coding(h, cu, aec, uiBitSize, force_allzero, 1);
        cost += chroma_residual_coding(h, cu, aec, uiBitSize - 1, force_allzero, 1);
        bits_cost = write_one_cu_rdo(h, aec, cu, uiBitSize, a->lambda_mode, MAX_COST);
        cost += bits_cost;
    }

    *est_bits = bits_cost / a->lambda_mode;

    /* store coef and rec */
    if ((cu->cbp & 0xF) == 0xF) {
        memcpy(bst->coef_y, h->cu_coefs.coef_y, cu_size * cu_size * sizeof(coef_t));
    } else if (cu->cbp & 0xF) {
        uchar_t *d = (uchar_t*)bst->coef_y;
        uchar_t *s = (uchar_t*)h->cu_coefs.coef_y;
        int size = cu_size * cu_size * sizeof(coef_t) >> 2;

        for (i = 0; i < 4; i++) {
            if (cu->cbp & (1 << i)) {
                memcpy(d, s, size);
            }
            d += size;
            s += size;
        }
    }
    if (cu->cbp & 0x10) {
        memcpy(bst->coef_u, h->cu_coefs.coef_u, cu_size * cu_size * sizeof(coef_t) >> 2);
    }
    if (cu->cbp & 0x20) {
        memcpy(bst->coef_v, h->cu_coefs.coef_v, cu_size * cu_size * sizeof(coef_t) >> 2);
    }

    //--- reconstructed blocks ----
    rec = h->p_rec[0] + (pix_y - h->lcu_pix_y) * CACHE_STRIDE + pix_x - h->lcu_pix_x;

    g_funs_handle.cpy_pel[cu_size >> 1](rec, CACHE_STRIDE, bst->rec_y, cu_size, cu_size, cu_size);

    recu = h->p_rec[1] + (pix_c_y - h->lcu_pix_y / 2) * CACHE_STRIDE + pix_c_x - h->lcu_pix_x / 2;
    recv = h->p_rec[2] + (pix_c_y - h->lcu_pix_y / 2) * CACHE_STRIDE + pix_c_x - h->lcu_pix_x / 2;

    cu_size >>= 1;

    g_funs_handle.cpy_pel[cu_size >> 1](recu, CACHE_STRIDE, bst->rec_u, cu_size, cu_size, cu_size);
    g_funs_handle.cpy_pel[cu_size >> 1](recv, CACHE_STRIDE, bst->rec_v, cu_size, cu_size, cu_size);

    return cost;
}
 
double cdd_get_threshold(avs3_enc_t *h, int uiBitSize, double lambda)
{
    double threshold;
    const cfg_param_t *input = h->input;
    int adj = 1 << (input->bit_depth - 8);
    double Q_Step;

    if (h->type & I_FRM) {
        return 0;
    }

    Q_Step = adj * pow(2.0, (5.661 * log(lambda) + 13.131) / 8.0);

    if (SPEED_LEVEL(6, input->speed_level)) {
        switch (uiBitSize) {
        case 3:
            threshold = 48 * Q_Step;
            threshold *= 5.0;
            break;
        case 4:
            threshold = 149 * Q_Step;
            threshold *= 3.0;
            break;
        case 5:
            threshold = 408 * Q_Step;
            threshold *= 1.8;
            break;
        case 6:
            threshold = 1320 * Q_Step;
            threshold *= 1.2;
            break;
        default:
            threshold = MAX_COST;
            break;
        }
    } else {
        switch (uiBitSize) {
        case 3:
            threshold = 40 * Q_Step + 100;
            threshold *= 5.0;
            break;
        case 4:
            threshold = 124 * Q_Step + 366;
            threshold *= 3.0;
            break;
        case 5:
            threshold = 340 * Q_Step + 1600;
            threshold *= 1.8;
            break;
        case 6:
            threshold = 1100 * Q_Step + 4500;
            threshold *= 1.2;
            break;
        default:
            threshold = MAX_COST;
            break;
        }
    }

    return threshold;
}

double analyze_cu_tree(avs3_enc_t *h, aec_t *aec, int uiBitSize, int uiMaxBitSize, int uiMinBitSize, int uiPositionInPic, int qp, double lambda, double mcost, double *est_bits)
{
    int i;
    const cfg_param_t *input = h->input;
    analyzer_t *a = &h->analyzer;
    aec_t large_cs;
    int pix_x_InPic_start = (uiPositionInPic % input->pic_width_in_mcu) << 3;
    int pix_y_InPic_start = (uiPositionInPic / input->pic_width_in_mcu) << 3;
    int pix_x_InPic_end  = ((uiPositionInPic % input->pic_width_in_mcu) << 3) + (1 << uiBitSize);
    int pix_y_InPic_end  = ((uiPositionInPic / input->pic_width_in_mcu) << 3) + (1 << uiBitSize);
    int out_of_pic = (pix_x_InPic_end > input->img_width) || (pix_y_InPic_end > input->img_height);
    int early_terminate = 0;
    int cu_blk_size = (1 << (uiBitSize)) * (1 << (uiBitSize));
    double small_rd_cost = MAX_COST;
    double large_cu_cost = MAX_COST;
    double bak_mcost = mcost;
    int blksize = 1 << uiBitSize;
    static float scu_cost_th[4] = { 0.5f, 0.75f, 1.0f, 1.0f }; 

    best_mode_t best_info;
    double var_var_NxN = MAX_COST;
    cu_t cu;
    double large_est_bits;
    double small_est_bits;

    ALIGNED_64(coef_t bst_coefs[MAX_CU_SIZE * MAX_CU_SIZE * 3 / 2]);
    ALIGNED_64(pel_t  bst_recs [MAX_CU_SIZE * MAX_CU_SIZE * 3 / 2]);

    init_cu_pos(h, uiPositionInPic, uiBitSize);
    Init_Curr_codingUnit(h, &cu, uiBitSize, uiPositionInPic, qp, lambda);

    best_info.coef_y = bst_coefs;
    best_info.coef_u = bst_coefs + cu_blk_size;
    best_info.coef_v = bst_coefs + cu_blk_size + cu_blk_size / 4;
    best_info.rec_y  = bst_recs;
    best_info.rec_u  = bst_recs  + cu_blk_size;
    best_info.rec_v  = bst_recs  + cu_blk_size + cu_blk_size / 4;

    best_info.bst_cu.cbp = 0xFF;
    best_info.bst_cu.cuType = -1;

    if (h->type & I_FRM) {
        intra_depth_early_skip(h, uiBitSize, &uiMaxBitSize, &uiMinBitSize, out_of_pic, pix_y_InPic_start, pix_x_InPic_start, &var_var_NxN, cu.QP);
    }

    if (!out_of_pic && uiBitSize <= uiMaxBitSize) {
        int split_bits = 0;

        h->analyzer.use_rdoq = 0;

        best_info.bst_cu.QP = cu.QP;

        a->cu_threshold = cdd_get_threshold(h, uiBitSize, h->analyzer.lambda_mode);

        copy_coding_state_hdr(&large_cs, aec);

        lbac_enc_split_mode(h, &large_cs, NO_SPLIT,
            blksize, blksize, LCU_SIZE_IN_BITS - uiBitSize, 0, pix_x_InPic_start, pix_y_InPic_start);

        if (h->type & I_FRM) {
            analyze_one_cu_i_frm(h, &cu, &best_info, &large_cs, uiBitSize, mcost);
            intra_depth_early_prune(h, uiBitSize, &uiMaxBitSize, &uiMinBitSize, out_of_pic, var_var_NxN, mcost, cu.QP);
        } else {
            analyze_one_cu_pb_frm(h, &cu, &best_info, &large_cs, uiBitSize);
        }

        if (best_info.bst_cu.cuType != -1) {
            double cost = split_bits * a->lambda_mode;
            cu_t *bst_cu = &best_info.bst_cu;

            if (bst_cu->cbp == 0 && (bst_cu->cuType < I16MB)) {
                pel_t *predy, *predu, *predv;

                if (bst_cu->cuType) {
                    predy = h->pred_inter_luma;
                    predu = h->pred_inter_chroma[0];
                    predv = h->pred_inter_chroma[1];
                } else {
                    predy = h->pred_inter_luma_skip[bst_cu->mdirect_mode];
                    predu = h->pred_inter_chroma_skip[bst_cu->mdirect_mode][0];
                    predv = h->pred_inter_chroma_skip[bst_cu->mdirect_mode][1];
                }

                cu_blk_size *= sizeof(pel_t);
                memcpy(best_info.rec_y, predy, cu_blk_size);
                memcpy(best_info.rec_u, predu, cu_blk_size >> 2);
                memcpy(best_info.rec_v, predv, cu_blk_size >> 2);

                large_cu_cost = cost + best_info.rd_cost;

                large_est_bits = write_one_cu_rdo(h, &large_cs, &best_info.bst_cu, uiBitSize, a->lambda_mode, MAX_COST) / a->lambda_mode;
            } else {
                large_cu_cost = cost + encode_one_cu(h, &best_info, &large_cs, uiBitSize, &large_est_bits);
            }
        }

        mcost = min(mcost, large_cu_cost);

        early_terminate |= uiBitSize <= uiMinBitSize;

        if (!early_terminate && best_info.bst_cu.cuType != -1) {
            int b_skip = (best_info.bst_cu.cuType == 0 && best_info.bst_cu.cbp == 0);
            double threshold = a->cu_threshold * input->speed_adj_rate;

            early_terminate |= !(h->type & I_FRM) && large_cu_cost > a->cu_threshold * 12; // Skip Noisy Blocks

            if (SPEED_LEVEL(7, input->speed_level) && uiBitSize == 4) {
                threshold *= 2;
            }

            early_terminate |= large_cu_cost < threshold;
            
            early_terminate |= b_skip && large_cu_cost < threshold * 1.5;
 
            if (h->type == B_FRM && uiBitSize == 4) {
                if (h->curr_RPS.layer == 2) {
                    early_terminate |= large_cu_cost < threshold * 1.2;
                }
                if (h->curr_RPS.layer == 3) {
                    early_terminate |= large_cu_cost < threshold * 2.0;
                }
            }
        }
    }

    if (!early_terminate) {
        double split_cost = 0;
        double tmp_est_bits;

        small_rd_cost = 0;
        small_est_bits = 0;

        int bak_len = arienco_bits_written(aec);

        lbac_enc_split_mode(h, aec, SPLIT_QUAD,
            blksize, blksize, LCU_SIZE_IN_BITS - uiBitSize, 0, pix_x_InPic_start, pix_y_InPic_start);

        split_cost = a->lambda_mode * (arienco_bits_written(aec) - bak_len);

        for (i = 0; i < 4; i++) {
            int mb_x  = (i &  1) << (uiBitSize - 3 - 1);
            int mb_y  = (i >> 1) << (uiBitSize - 3 - 1);
            int pos   = uiPositionInPic + mb_y * input->pic_width_in_mcu + mb_x;
            int pos_x = pix_x_InPic_start + (mb_x << 3);
            int pos_y = pix_y_InPic_start + (mb_y << 3);
            double threold_cost = scu_cost_th[i] * mcost;

            if (pos_x >= input->img_width || pos_y >= input->img_height) { // check the starting position
                continue;
            }
     
            small_rd_cost += analyze_cu_tree(h, aec, uiBitSize - 1, uiMaxBitSize, uiMinBitSize, pos, qp, a->lambda_mode, mcost - small_rd_cost, &tmp_est_bits);
            small_est_bits += tmp_est_bits;

            if (small_rd_cost + split_cost >= threold_cost) {
                small_rd_cost = MAX_COST;
                break;
            }
        }
        small_rd_cost += split_cost;
    }

    if (bak_mcost > min(large_cu_cost, small_rd_cost)) {
        if (!out_of_pic) {
            init_cu_pos(h, uiPositionInPic, uiBitSize);
            if (large_cu_cost <= small_rd_cost) {
                revert_large_cu_param(h, &best_info, uiBitSize, uiPositionInPic);
                reset_coding_state_hdr(&large_cs);
                *est_bits = large_est_bits;
                return large_cu_cost;
            } else {
                *est_bits = small_est_bits;
                return small_rd_cost;
            }
        } else {
            *est_bits = small_est_bits;
            return small_rd_cost;
        }
    } else {
        return bak_mcost + 1;
    }
}