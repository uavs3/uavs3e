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

#define ROUND(a)  (((a) < 0)? (int)((a) - 0.5) : (int)((a) + 0.5))
#define REG              0.0001
#define REG_SQR          0.0000001

#define Clip_post(high,val) ((val > high)? high: val)

static void copyToImage(pel *pDest, pel *pSrc, int stride_in, int img_height, int img_width, int formatShift)
{
    int height = img_height >> formatShift;
    int width  = img_width  >> formatShift;
    pel *psrc  = pSrc;
    pel *pdst  = pDest;

    for (int j = 0; j < height; j++) {
        memcpy(pdst, psrc, width * sizeof(pel));
        pdst = pdst + stride_in;
        psrc = psrc + stride_in;
    }
}

static void copyOneAlfBlk(pel *picDst, pel *picSrc, int stride, int ypos, int xpos, int height, int width, int isAboveAvail, int isBelowAvail)
{
    int posOffset = (ypos * stride) + xpos;
    pel *pelDst;
    pel *pelSrc;
    int j;
    int startPos = isAboveAvail ? (ypos - 4) : ypos;
    int endPos = isBelowAvail ? (ypos + height - 4) : ypos + height;
    posOffset = (startPos * stride) + xpos;
    pelDst = picDst + posOffset;
    pelSrc = picSrc + posOffset;
    for (j = startPos; j < endPos; j++) {
        memcpy(pelDst, pelSrc, sizeof(pel)*width);
        pelDst += stride;
        pelSrc += stride;
    }
}

static long long cal_lcu_ssd(enc_alf_var_t *Enc_ALF, pel *pOrg, int i_org, pel *pCmp, int iWidth, int iHeight, int iStride)
{
    if (iWidth < 256 && CONV_LOG2(iWidth) != -1) {
        int widx = CONV_LOG2(iWidth) - MIN_CU_LOG2;
        return uavs3e_funs_handle.cost_ssd[widx](pOrg, i_org, pCmp, iStride, iHeight);
    } else {
        long long uiSSD = 0;

        for (int y = 0; y < iHeight; y++) {
            for (int x = 0; x < iWidth; x++) {
                int iTemp = pOrg[x] - pCmp[x];
                uiSSD += iTemp * iTemp;
            }
            pOrg += i_org;
            pCmp += iStride;
        }
        return uiSSD;
    }
}

static long long cal_alf_lcu_dist(enc_pic_t *ep, int compIdx, int lcuAddr, int ctuYPos, int ctuXPos, int ctuHeight,
    int ctuWidth, BOOL isAboveAvail, pel *picSrc, int i_src, pel *picCmp, int stride)
{
    if (compIdx == Y_C) {
        pel *pelCmp = picCmp + ctuYPos * stride + ctuXPos;
        pel *pelSrc = picSrc + ctuYPos * i_src  + ctuXPos;
        return cal_lcu_ssd(&ep->Enc_ALF, pelSrc, i_src, pelCmp, ctuWidth, ctuHeight, stride);
    } else {
        ctuYPos   >>= 1;
        ctuXPos   >>= 1;
        ctuHeight >>= 1;
        ctuWidth  >>= 1;

        if (isAboveAvail) {
            ctuYPos -= 4;
        }
        pel *pelCmp = picCmp + ctuYPos * stride + ctuXPos;
        pel *pelSrc = picSrc + ctuYPos * i_src  + ctuXPos;
        return cal_lcu_ssd(&ep->Enc_ALF, pelSrc, i_src, pelCmp, ctuWidth, ctuHeight, stride);
    }
}

static void mergeFrom(enc_alf_corr_t *dst, enc_alf_corr_t *src, int *mergeTable)
{
    switch (dst->componentID) {
    case U_C:
    case V_C: {
        double(*srcE)[ALF_MAX_NUM_COEF] = src->ECorr[0];
        double(*dstE)[ALF_MAX_NUM_COEF] = dst->ECorr[0];
        double *srcy = src->yCorr[0];
        double *dsty = dst->yCorr[0];

        for (int j = 0; j < ALF_MAX_NUM_COEF; j++) {
            for (int i = 0; i < ALF_MAX_NUM_COEF; i++) {
                dstE[j][i] += srcE[j][i];
            }
            dsty[j] += srcy[j];
        }
        break;
    }
    case Y_C: {
        for (int varInd = 0; varInd < NO_VAR_BINS; varInd++) {
            int filtIdx = (mergeTable == NULL) ? (0) : (mergeTable[varInd]);
            double(*srcE)[ALF_MAX_NUM_COEF] = src->ECorr[varInd];
            double(*dstE)[ALF_MAX_NUM_COEF] = dst->ECorr[filtIdx];
            double *srcy = src->yCorr[varInd];
            double *dsty = dst->yCorr[filtIdx];

            for (int j = 0; j < ALF_MAX_NUM_COEF; j++) {
                for (int i = 0; i < ALF_MAX_NUM_COEF; i++) {
                    dstE[j][i] += srcE[j][i];
                }
                dsty[j] += srcy[j];
            }
        }
        break;
    }
    default: {
        printf("not a legal component ID\n");
        assert(0);
        exit(-1);
    }
    }
}

static void predictALFCoeff(int(*coeff)[ALF_MAX_NUM_COEF], int numCoef, int numFilters)
{
    int g, pred, sum, i;
    for (g = 0; g < numFilters; g++) {
        sum = 0;
        for (i = 0; i < numCoef - 1; i++) {
            sum += (2 * coeff[g][i]);
        }
        pred = (1 << ALF_NUM_BIT_SHIFT) - (sum);
        coeff[g][numCoef - 1] = coeff[g][numCoef - 1] - pred;
    }
}

static void deriveBoundaryAvail(enc_pic_t *ep, int numLCUInPicWidth, int numLCUInPicHeight, int ctu, BOOL *isAboveAvail, BOOL *isBelowAvail)
{
    int numLCUsInFrame = numLCUInPicHeight * numLCUInPicWidth;
    int lcuHeight = 1 << ep->info.log2_max_cuwh;
    int lcuWidth = lcuHeight;
    int i_scu = ep->info.i_scu;
    int pic_x = (ctu % numLCUInPicWidth) * lcuWidth;
    int pic_y = (ctu / numLCUInPicWidth) * lcuHeight;
    int mb_x = pic_x / MIN_CU_SIZE;
    int mb_y = pic_y / MIN_CU_SIZE;
    int mb_nr = mb_y * i_scu + mb_x;
 
    *isAboveAvail = (ctu >= numLCUInPicWidth);
    *isBelowAvail = (ctu < numLCUsInFrame - numLCUInPicWidth);

    s8 *map_patch = ep->map.map_patch;
    int cuCurr = (mb_nr);
    int cuAbove = *isAboveAvail ? (mb_nr - i_scu) : -1;

    if (!ep->info.sqh.filter_cross_patch) {
        *isAboveAvail = FALSE;

        int curSliceNr = map_patch[cuCurr];

        if (cuAbove != -1) {
            int neighorSliceNr = map_patch[cuAbove];
            if (curSliceNr == neighorSliceNr) {
                *isAboveAvail = TRUE;
            }
        }
    }
}

/*
*************************************************************************
* Function: Calculate the correlation matrix for each LCU
* Input:
*  skipCUBoundaries  : Boundary skip flag
*           compIdx  : Image component index
*            lcuAddr : The address of current LCU
* (ctuXPos,ctuYPos)  : The LCU position
*          isXXAvail : The Available of neighboring LCU
*            pPicOrg : The original image buffer
*            pPicSrc : The distortion image buffer
*           stride : The width of image buffer
* Output:
* Return:
*************************************************************************
*/
static void getStatisticsOneLCU(int varInd, int ctuYPos, int ctuXPos, int ctuHeight, int ctuWidth
    , BOOL isAboveAvail, BOOL isBelowAvail, enc_alf_corr_t *lcu_alfCorr, com_pic_t *pic_org, com_pic_t *pic_rec)
{
    enc_alf_corr_t *alfCorr = lcu_alfCorr;
    uavs3e_funs_handle.alf_calc(pic_org->y, pic_org->stride_luma, pic_rec->y, pic_rec->stride_luma, ctuXPos, ctuYPos, ctuWidth, ctuHeight, alfCorr->ECorr[varInd], alfCorr->yCorr[varInd], isAboveAvail, isBelowAvail);

    ctuYPos   >>= 1;
    ctuXPos   >>= 1;
    ctuHeight >>= 1;
    ctuWidth  >>= 1;

    alfCorr = lcu_alfCorr + 1;
    uavs3e_funs_handle.alf_calc(pic_org->u, pic_org->stride_chroma, pic_rec->u, pic_rec->stride_chroma, ctuXPos, ctuYPos, ctuWidth, ctuHeight, alfCorr->ECorr[0], alfCorr->yCorr[0], isAboveAvail, isBelowAvail);

    alfCorr = lcu_alfCorr + 2;
    uavs3e_funs_handle.alf_calc(pic_org->v, pic_org->stride_chroma, pic_rec->v, pic_rec->stride_chroma, ctuXPos, ctuYPos, ctuWidth, ctuHeight, alfCorr->ECorr[0], alfCorr->yCorr[0], isAboveAvail, isBelowAvail);
}


/*
*************************************************************************
* Function: Calculate the correlation matrix for image
* Input:
*             h  : CONTEXT used for encoding process
*     pic_alf_Org  : picture of the original image
*     pic_alf_Rec  : picture of the the ALF input image
*           stride : The stride of Y component of the ALF input picture
*           lambda : The lambda value in the ALF-RD decision
* Output:
* Return:
*************************************************************************
*/
static void alf_get_statistics(enc_pic_t *ep, com_pic_t *pic_org, com_pic_t *pic_rec)
{
    enc_alf_var_t *Enc_ALF = &ep->Enc_ALF;
    int lcu_size           =  ep->info.max_cuwh;
    int img_height         =  ep->info.pic_height;
    int img_width          =  ep->info.pic_width;
    int numLCUInPicWidth   =  ep->info.pic_width_in_lcu;
    int numLCUInPicHeight  =  ep->info.pic_height_in_lcu;

    int NumCUInFrame = numLCUInPicHeight * numLCUInPicWidth;

    for (int ctu = 0; ctu < NumCUInFrame; ctu++) {
        int varIdx    = ep->alf_var_map[ctu];
        int ctuYPos   = (ctu / numLCUInPicWidth) * lcu_size;
        int ctuXPos   = (ctu % numLCUInPicWidth) * lcu_size;
        int ctuHeight = (ctuYPos + lcu_size > img_height) ? (img_height - ctuYPos) : lcu_size;
        int ctuWidth  = (ctuXPos + lcu_size > img_width ) ? (img_width  - ctuXPos) : lcu_size;
        BOOL  isAboveAvail, isBelowAvail;

        memset(&Enc_ALF->m_alfCorr[ctu], 0, sizeof(enc_alf_corr_t) * N_C);

        deriveBoundaryAvail(ep, numLCUInPicWidth, numLCUInPicHeight, ctu, &isAboveAvail, &isBelowAvail);

        getStatisticsOneLCU(varIdx, ctuYPos, ctuXPos, ctuHeight, ctuWidth, isAboveAvail, isBelowAvail, Enc_ALF->m_alfCorr[ctu], pic_org, pic_rec);
    }
}

static unsigned int uvlcBitrateEstimate(int val)
{
    unsigned int length = 1;
    val++;
    assert(val);
    while (1 != val) {
        val >>= 1;
        length += 2;
    }
    return ((length >> 1) + ((length + 1) >> 1));
}

static unsigned int svlcBitrateEsitmate(int val)
{
    return uvlcBitrateEstimate((val <= 0) ? (-val << 1) : ((val << 1) - 1));
}

static unsigned int filterCoeffBitrateEstimate(int *coeff)
{
    unsigned int  bitrate = 0;
    int i;
    for (i = 0; i < (int)ALF_MAX_NUM_COEF; i++) {
        bitrate += (svlcBitrateEsitmate(coeff[i]));
    }
    return bitrate;
}

static unsigned int ALFParamBitrateEstimate(com_alf_pic_param_t *alfParam)
{
    unsigned int  bitrate = 0; //alf enabled flag
    int noFilters, g;
    if (alfParam->alf_flag == 1) {
        if (alfParam->componentID == Y_C) {
            noFilters = alfParam->filters_per_group - 1;
            bitrate += uvlcBitrateEstimate(noFilters);
            bitrate += (4 * noFilters);
        }
        for (g = 0; g < alfParam->filters_per_group; g++) {
            bitrate += filterCoeffBitrateEstimate(alfParam->coeffmulti[g]);
        }
    }
    return bitrate;
}

static unsigned int estimateALFBitrateInPicHeader(com_alf_pic_param_t *alfPicParam)
{
    //CXCTBD please help to check if the implementation is consistent with syntax coding
    int compIdx;
    unsigned int bitrate = 3; // pic_alf_enabled_flag[0,1,2]
    if (alfPicParam[0].alf_flag == 1 || alfPicParam[1].alf_flag == 1 || alfPicParam[2].alf_flag == 1) {
        for (compIdx = 0; compIdx < N_C; compIdx++) {
            bitrate += ALFParamBitrateEstimate(&alfPicParam[compIdx]);
        }
    }
    return bitrate;
}

static long long xFastFiltDistEstimation(enc_alf_var_t *Enc_ALF, double ppdE[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF], double *pdy, int *piCoeff, int iFiltLength)
{
    double pdcoeff[ALF_MAX_NUM_COEF];
    double dDist = 0;

    for (int i = 0; i < iFiltLength; i++) {
        pdcoeff[i] = (double)piCoeff[i] / (double)(1 << ((int)ALF_NUM_BIT_SHIFT));
    }
    for (int i = 0; i < iFiltLength; i++) {
        double dsum = ((double)ppdE[i][i]) * pdcoeff[i];
        for (int j = i + 1; j < iFiltLength; j++) {
            dsum += (double)(2 * ppdE[i][j]) * pdcoeff[j];
        }
        dDist += ((dsum - 2.0 * pdy[i]) * pdcoeff[i]);
    }
    if (dDist < 0) {
        return -((long long)(-dDist + 0.5));
    } else { 
        return  ((long long)( dDist + 0.5));
    }
}

static long long estimate_alf_lcu_dist(enc_alf_var_t *Enc_ALF, enc_alf_corr_t *alfCorr, int(*coeffSet)[ALF_MAX_NUM_COEF], int filterSetSize, int *mergeTable)
{
    enc_alf_corr_t alfMerged;
    long long iDist = 0;
    memset(&alfMerged, 0, sizeof(enc_alf_corr_t));
    mergeFrom(&alfMerged, alfCorr, mergeTable);

    for (int f = 0; f < filterSetSize; f++) {
        iDist += xFastFiltDistEstimation(Enc_ALF, alfMerged.ECorr[f], alfMerged.yCorr[f], coeffSet ? coeffSet[f] : Enc_ALF->m_coeffNoFilter, ALF_MAX_NUM_COEF);
    }
    return iDist;
}

static void filterOneCTB(enc_alf_var_t *Enc_ALF, pel *pRest, pel *pDec, int varIdx, int stride, int compIdx, int bit_depth, com_alf_pic_param_t *alfParam, int ctuYPos, int ctuHeight,
    int ctuXPos, int ctuWidth, int(*coeffSet)[ALF_MAX_NUM_COEF], BOOL isAboveAvail, BOOL isBelowAvail)
{
    //half size of 7x7cross+ 3x3square
    int  formatShift = (compIdx == Y_C) ? 0 : 1;
    int ypos, height, xpos, width;
    //derive CTB start positions, width, and height. If the boundary is not available, skip boundary samples.
    ypos = (ctuYPos >> formatShift);
    height = (ctuHeight >> formatShift);
    xpos = (ctuXPos >> formatShift);
    width = (ctuWidth >> formatShift);
    int *coef = (compIdx != Y_C) ? coeffSet[0] : coeffSet[Enc_ALF->m_varIndTab[varIdx]];

    if (isAboveAvail) {
        ypos -= 4;
        height += 4;
    }
    if (isBelowAvail) {
        height -= 4;
    }

    pRest += ypos * stride + xpos;
    pDec += ypos * stride + xpos;

    uavs3e_funs_handle.alf(pRest, stride, pDec, stride, width, height, coef, bit_depth);
    uavs3e_funs_handle.alf_fix(pRest, stride, pDec, stride, width, height, coef, bit_depth);
}


/*
*************************************************************************
* Function: ALF On/Off decision for LCU
*************************************************************************
*/
static double alf_decision_all_lcu(enc_pic_t *ep, lbac_t *lbac, com_alf_pic_param_t *alfPictureParam
                            , double lambda
                            , BOOL isRDOEstimate
                            , enc_alf_corr_t (*alfCorr)[N_C]
                            , pel *imgY_org, pel **imgUV_org, int i_org, pel *imgY_Dec, pel **imgUV_Dec, pel *imgY_Res, pel **imgUV_Res
                            , int stride)
{
    enc_alf_var_t *Enc_ALF = &ep->Enc_ALF;
    BOOL isAboveAvail, isBelowAvail;

    long long  distBestPic[N_C] = { 0 };
    double     rateBestPic[N_C] = { 0 };

    double lambda_luma    = lambda;
    double lambda_chroma  = lambda; //todo: lambda is not correct
    int lcu_size          = ep->info.max_cuwh;
    int img_height        = ep->info.pic_height;
    int img_width         = ep->info.pic_width;
    int numLCUInPicWidth  = ep->info.pic_width_in_lcu;
    int numLCUInPicHeight = ep->info.pic_height_in_lcu;
    int NumCUsInFrame     = numLCUInPicHeight * numLCUInPicWidth;

    lbac_t alf_sbac;
    lbac_copy(&alf_sbac, lbac);
    int coefs[N_C][NO_VAR_BINS][ALF_MAX_NUM_COEF];

    for (int compIdx = 0; compIdx < N_C; compIdx++) {
        com_alf_recon_coef(&alfPictureParam[compIdx], coefs[compIdx]);
    }

    memset(Enc_ALF->m_varIndTab, 0, NO_VAR_BINS * sizeof(int));

    if (alfPictureParam[0].filters_per_group > 1) {
        for (int i = 1; i < NO_VAR_BINS; ++i) {
            if (alfPictureParam[0].filterPattern[i]) {
                Enc_ALF->m_varIndTab[i] = Enc_ALF->m_varIndTab[i - 1] + 1;
            } else {
                Enc_ALF->m_varIndTab[i] = Enc_ALF->m_varIndTab[i - 1];
            }
        }
    }

    for (int ctu = 0; ctu < NumCUsInFrame; ctu++) {
        int ctuYPos   = (ctu / numLCUInPicWidth) * lcu_size;
        int ctuXPos   = (ctu % numLCUInPicWidth) * lcu_size;
        int ctuHeight = (ctuYPos + lcu_size > img_height) ? (img_height - ctuYPos) : lcu_size;
        int ctuWidth  = (ctuXPos + lcu_size  > img_width) ? (img_width  - ctuXPos) : lcu_size;

        if (ctu > 0) {
            int prev_ctuYPos = ((ctu - 1) / numLCUInPicWidth) * lcu_size;
            int prev_ctuXPos = ((ctu - 1) % numLCUInPicWidth) * lcu_size;
            s8 *map_patch = ep->map.map_patch;
            int curr_mb_nr = (ctuYPos >> MIN_CU_LOG2) * (img_width >> MIN_CU_LOG2) + (ctuXPos >> MIN_CU_LOG2);
            int prev_mb_nr = (prev_ctuYPos >> MIN_CU_LOG2) * (img_width >> MIN_CU_LOG2) + (prev_ctuXPos >> MIN_CU_LOG2);
            int ctuCurr_sn = map_patch[curr_mb_nr];
            int ctuPrev_sn = map_patch[prev_mb_nr];
            if (ctuCurr_sn != ctuPrev_sn) {
                lbac_reset(&alf_sbac);    // init lbac for alf rdo
            }
        }

        deriveBoundaryAvail(ep, numLCUInPicWidth, numLCUInPicHeight, ctu, &isAboveAvail, &isBelowAvail);

        for (int compIdx = 0; compIdx < N_C; compIdx++) {
            pel *org = NULL, *pDec = NULL, *pRest = NULL;
            int formatShift = 0, stride_in = 0;
            long long distEnc;

            if (!isRDOEstimate) {
                formatShift = (compIdx == Y_C) ? 0 : 1;
                stride_in   = (compIdx == Y_C) ? (stride) : (stride >> 1);
                org         = (compIdx == Y_C) ? imgY_org : imgUV_org[compIdx - U_C];
                pDec        = (compIdx == Y_C) ? imgY_Dec : imgUV_Dec[compIdx - U_C];
                pRest       = (compIdx == Y_C) ? imgY_Res : imgUV_Res[compIdx - U_C];
            }
            if (alfPictureParam[compIdx].alf_flag == 0) {
                Enc_ALF->m_AlfLCUEnabled[ctu][compIdx] = FALSE;
                continue;
            }
            if (isRDOEstimate) {
                distEnc = estimate_alf_lcu_dist(Enc_ALF, &alfCorr[ctu][compIdx], coefs[compIdx], alfPictureParam[compIdx].filters_per_group, Enc_ALF->m_varIndTab)
                        - estimate_alf_lcu_dist(Enc_ALF, &alfCorr[ctu][compIdx], NULL, 1, NULL);
            } else {
                int i_org_plane = (compIdx == Y_C) ? (i_org) : (i_org >> 1);

                filterOneCTB(Enc_ALF, pRest, pDec, ep->alf_var_map[ctu], stride_in, compIdx, ep->info.bit_depth_internal, &alfPictureParam[compIdx], ctuYPos, ctuHeight, ctuXPos, ctuWidth, coefs[compIdx], isAboveAvail, isBelowAvail);

                distEnc  = cal_alf_lcu_dist(ep, compIdx, ctu, ctuYPos, ctuXPos, ctuHeight, ctuWidth, isAboveAvail, org, i_org_plane, pRest, stride_in);
                distEnc -= cal_alf_lcu_dist(ep, compIdx, ctu, ctuYPos, ctuXPos, ctuHeight, ctuWidth, isAboveAvail, org, i_org_plane, pDec,  stride_in);
            }
            lbac_t alf_lbac_off;
            lbac_copy(&alf_lbac_off, &alf_sbac);

            // ALF ON
            double rateEnc = lbac_get_bits(&alf_sbac);
            lbac_enc_alf_flag(&alf_sbac, NULL, 1);
            rateEnc = lbac_get_bits(&alf_sbac) - rateEnc;
            double costEnc = (double)distEnc + (compIdx == 0 ? lambda_luma : lambda_chroma) * rateEnc;

            //ALF OFF
            double rateOff = lbac_get_bits(&alf_lbac_off);
            lbac_enc_alf_flag(&alf_lbac_off, NULL, 0);
            rateOff = lbac_get_bits(&alf_lbac_off) - rateOff;
            double costOff = (compIdx == 0 ? lambda_luma : lambda_chroma) * rateOff;

            if (costEnc >= costOff) {
                Enc_ALF->m_AlfLCUEnabled[ctu][compIdx] = FALSE;

                if (!isRDOEstimate) {
                    copyOneAlfBlk(pRest, pDec, stride_in, ctuYPos >> formatShift, ctuXPos >> formatShift, ctuHeight >> formatShift, ctuWidth >> formatShift, isAboveAvail, isBelowAvail);
                }
                lbac_copy(&alf_sbac, &alf_lbac_off);
            } else {
                Enc_ALF->m_AlfLCUEnabled[ctu][compIdx] = TRUE;
            }

            rateBestPic[compIdx] += (Enc_ALF->m_AlfLCUEnabled[ctu][compIdx] ? rateEnc : rateOff);
            distBestPic[compIdx] += (Enc_ALF->m_AlfLCUEnabled[ctu][compIdx] ? distEnc : 0);
        } 
    } 

    for (int compIdx = 0; compIdx < N_C; compIdx++) {
        if (alfPictureParam[compIdx].alf_flag == 1) {
            double costAlfOn = (double)distBestPic[compIdx] + (compIdx == 0 ? lambda_luma : lambda_chroma) * (rateBestPic[compIdx] +
                        (double)(ALFParamBitrateEstimate(&alfPictureParam[compIdx])));

            if (costAlfOn >= 0) {
                alfPictureParam[compIdx].alf_flag = 0;
                for (int n = 0; n < NumCUsInFrame; n++) {
                    Enc_ALF->m_AlfLCUEnabled[n][compIdx] = FALSE;
                }
                if (!isRDOEstimate) {
                    int  formatShift = (compIdx == Y_C) ? 0 : 1;
                    pel *pDec        = (compIdx == Y_C) ? imgY_Dec : imgUV_Dec[compIdx - U_C];
                    pel *pRest       = (compIdx == Y_C) ? imgY_Res : imgUV_Res[compIdx - U_C];
                    int  stride_in   = (compIdx == Y_C) ? (stride) : (stride >> 1);
                    copyToImage(pRest, pDec, stride_in, img_height, img_width, formatShift);
                }
            }
        }
    }
    double bestCost = 0;
    for (int compIdx = 0; compIdx < N_C; compIdx++) {
        if (alfPictureParam[compIdx].alf_flag == 1) {
            bestCost += (double)distBestPic[compIdx] + (compIdx == 0 ? lambda_luma : lambda_chroma) * (rateBestPic[compIdx]);
        }
    }

    return bestCost;
}

static void add_corr_data(enc_alf_corr_t *A, enc_alf_corr_t *B, enc_alf_corr_t *C)
{
    int numCoef = ALF_MAX_NUM_COEF;
    int maxNumGroups = NO_VAR_BINS;
    int numGroups;
    int g, j, i;
    if (A->componentID >= 0) {
        numGroups = (A->componentID == Y_C) ? (maxNumGroups) : (1);
        for (g = 0; g < numGroups; g++) {
            for (j = 0; j < numCoef; j++) {
                C->yCorr[g][j] = A->yCorr[g][j] + B->yCorr[g][j];
                for (i = 0; i < numCoef; i++) {
                    C->ECorr[g][j][i] = A->ECorr[g][j][i] + B->ECorr[g][j][i];
                }
            }
        }
    }
}

static void accumulate_lcu_corr(enc_pic_t *ep, enc_alf_corr_t *alf_corr, enc_alf_corr_t (*alfCorSrcLCU)[N_C], BOOL useAllLCUs)
{
    enc_alf_var_t *Enc_ALF = &ep->Enc_ALF;
    int numLCUInPicWidth   =  ep->info.pic_width_in_lcu;
    int numLCUInPicHeight  =  ep->info.pic_height_in_lcu;
    int NumCUInFrame = numLCUInPicHeight * numLCUInPicWidth;

    memset(alf_corr, 0, sizeof(enc_alf_corr_t) * 3);

    for (int compIdx = 0; compIdx < N_C; compIdx++) {
        enc_alf_corr_t *corr = &alf_corr[compIdx];
   
        if (!useAllLCUs) {
            int numStatLCU = 0;
            for (int addr = 0; addr < NumCUInFrame; addr++) {
                if (Enc_ALF->m_AlfLCUEnabled[addr][compIdx]) {
                    numStatLCU++;
                    break;
                }
            }
            if (numStatLCU == 0) {
                useAllLCUs = TRUE;
            }
        }
        for (int addr = 0; addr < (int)NumCUInFrame; addr++) {
            if (useAllLCUs || Enc_ALF->m_AlfLCUEnabled[addr][compIdx]) {
                add_corr_data(&alfCorSrcLCU[addr][compIdx], corr, corr);
            }
        }
    }
}

static void xcodeFiltCoeff(int *varIndTab, int numFilters, com_alf_pic_param_t *alfParam)
{
    int filterPattern[NO_VAR_BINS], i;
    memset(filterPattern, 0, NO_VAR_BINS * sizeof(int));
    alfParam->filters_per_group = numFilters;

    if (alfParam->filters_per_group > 1) {
        for (i = 1; i < NO_VAR_BINS; ++i) {
            if (varIndTab[i] != varIndTab[i - 1]) {
                filterPattern[i] = 1;
            }
        }
    }
    memcpy(alfParam->filterPattern, filterPattern, NO_VAR_BINS * sizeof(int));
    predictALFCoeff(alfParam->coeffmulti, ALF_MAX_NUM_COEF, alfParam->filters_per_group);
}

static tab_u8 tbl_weights_shape_1sym[ALF_MAX_NUM_COEF + 1] = {
    2,
    2,
    2, 2, 2,
    2, 2, 2, 1,
    1
};

static void xFilterCoefQuickSort(double *coef_data, int *coef_num, int upper, int lower)
{
    double mid, tmp_data;
    int i, j, tmp_num;
    i = upper;
    j = lower;
    mid = coef_data[(lower + upper) >> 1];
    do {
        while (coef_data[i] < mid) {
            i++;
        }
        while (mid < coef_data[j]) {
            j--;
        }
        if (i <= j) {
            tmp_data = coef_data[i];
            tmp_num = coef_num[i];
            coef_data[i] = coef_data[j];
            coef_num[i] = coef_num[j];
            coef_data[j] = tmp_data;
            coef_num[j] = tmp_num;
            i++;
            j--;
        }
    } while (i <= j);
    if (upper < j) {
        xFilterCoefQuickSort(coef_data, coef_num, upper, j);
    }
    if (i < lower) {
        xFilterCoefQuickSort(coef_data, coef_num, i, lower);
    }
}

static void xQuantFilterCoef(double *h, int *qh)
{
    int i, N;
    int max_value, min_value;
    double dbl_total_gain;
    int total_gain, q_total_gain;
    int upper, lower;
    double *dh;
    int  *nc;
    tab_u8 *pFiltMag;
    N = (int)ALF_MAX_NUM_COEF;
    pFiltMag = tbl_weights_shape_1sym;
    dh = (double *)malloc(N * sizeof(double));
    nc = (int *)malloc(N * sizeof(int));
    max_value = (1 << (1 + ALF_NUM_BIT_SHIFT)) - 1;
    min_value = 0 - (1 << (1 + ALF_NUM_BIT_SHIFT));
    dbl_total_gain = 0.0;
    q_total_gain = 0;
    for (i = 0; i < N; i++) {
        if (h[i] >= 0.0) {
            qh[i] = (int)(h[i] * (1 << ALF_NUM_BIT_SHIFT) + 0.5);
        } else {
            qh[i] = -(int)(-h[i] * (1 << ALF_NUM_BIT_SHIFT) + 0.5);
        }
        dh[i] = (double)qh[i] / (double)(1 << ALF_NUM_BIT_SHIFT) - h[i];
        dh[i] *= pFiltMag[i];
        dbl_total_gain += h[i] * pFiltMag[i];
        q_total_gain += qh[i] * pFiltMag[i];
        nc[i] = i;
    }
    // modification of quantized filter coefficients
    total_gain = (int)(dbl_total_gain * (1 << ALF_NUM_BIT_SHIFT) + 0.5);
    if (q_total_gain != total_gain) {
        xFilterCoefQuickSort(dh, nc, 0, N - 1);
        if (q_total_gain > total_gain) {
            upper = N - 1;
            while (q_total_gain > total_gain + 1) {
                i = nc[upper % N];
                qh[i]--;
                q_total_gain -= pFiltMag[i];
                upper--;
            }
            if (q_total_gain == total_gain + 1) {
                if (dh[N - 1] > 0) {
                    qh[N - 1]--;
                } else {
                    i = nc[upper % N];
                    qh[i]--;
                    qh[N - 1]++;
                }
            }
        } else if (q_total_gain < total_gain) {
            lower = 0;
            while (q_total_gain < total_gain - 1) {
                i = nc[lower % N];
                qh[i]++;
                q_total_gain += pFiltMag[i];
                lower++;
            }
            if (q_total_gain == total_gain - 1) {
                if (dh[N - 1] < 0) {
                    qh[N - 1]++;
                } else {
                    i = nc[lower % N];
                    qh[i]++;
                    qh[N - 1]--;
                }
            }
        }
    }
    // set of filter coefficients
    for (i = 0; i < N; i++) {
        qh[i] = COM_MAX(min_value, COM_MIN(max_value, qh[i]));
    }
    com_alf_check_coef(qh, N);
    free(dh);
    dh = NULL;
    free(nc);
    nc = NULL;
}

static double xfindBestCoeffCodMethod(int(*filterCoeffSymQuant)[ALF_MAX_NUM_COEF], int sqrFiltLength, int filters_per_fr,
                               double errorForce0CoeffTab[NO_VAR_BINS][2], double lambda)
{
    int coeffBits, i;
    double error = 0, lagrangian;
    int coeffmulti[NO_VAR_BINS][ALF_MAX_NUM_COEF];
    int g;

    for (g = 0; g < filters_per_fr; g++) {
        for (i = 0; i < sqrFiltLength; i++) {
            coeffmulti[g][i] = filterCoeffSymQuant[g][i];
        }
    }
    predictALFCoeff(coeffmulti, sqrFiltLength, filters_per_fr);
    coeffBits = 0;
    for (g = 0; g < filters_per_fr; g++) {
        coeffBits += filterCoeffBitrateEstimate(coeffmulti[g]);
    }
    for (i = 0; i < filters_per_fr; i++) {
        error += errorForce0CoeffTab[i][1];
    }
    lagrangian = error + lambda * coeffBits;

    return (lagrangian);
}

static int gnsCholeskyDec(double (*inpMatr)[ALF_MAX_NUM_COEF], double outMatr[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF], int noEq)
{
    int i, j, k;     /* Looping Variables */
    double scale;       /* scaling factor for each row */
    double invDiag[ALF_MAX_NUM_COEF];  /* Vector of the inverse of diagonal entries of outMatr */
    //  Cholesky decomposition starts
    for (i = 0; i < noEq; i++) {
        for (j = i; j < noEq; j++) {
            /* Compute the scaling factor */
            scale = inpMatr[i][j];
            if (i > 0) {
                for (k = i - 1; k >= 0; k--) {
                    scale -= outMatr[k][j] * outMatr[k][i];
                }
            }
            /* Compute i'th row of outMatr */
            if (i == j) {
                if (scale <= REG_SQR) {  // if(scale <= 0 )  /* If inpMatr is singular */
                    return 0;
                }
                else {
                    /* Normal operation */
                    invDiag[i] = 1.0 / (outMatr[i][i] = sqrt(scale));
                }
            }
            else {
                outMatr[i][j] = scale * invDiag[i]; /* Upper triangular part          */
                outMatr[j][i] = 0.0;              /* Lower triangular part set to 0 */
            }
        }
    }
    return 1; /* Signal that Cholesky factorization is successfully performed */
}

static void gnsTransposeBacksubstitution(double U[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF], double rhs[], double x[], int order)
{
    int i, j;             /* Looping variables */
    double sum;              /* Holds backsubstitution from already handled rows */
    /* Backsubstitution starts */
    x[0] = rhs[0] / U[0][0];               /* First row of U'                   */
    for (i = 1; i < order; i++) {
        /* For the rows 1..order-1           */
        for (j = 0, sum = 0.0; j < i; j++) { /* Backsubst already solved unknowns */
            sum += x[j] * U[j][i];
        }
        x[i] = (rhs[i] - sum) / U[i][i];       /* i'th component of solution vect.  */
    }
}

static void gnsBacksubstitution(double R[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF], double z[ALF_MAX_NUM_COEF], int R_size, double A[ALF_MAX_NUM_COEF])
{
    int i, j;
    double sum;
    R_size--;
    A[R_size] = z[R_size] / R[R_size][R_size];
    for (i = R_size - 1; i >= 0; i--) {
        for (j = i + 1, sum = 0.0; j <= R_size; j++) {
            sum += R[i][j] * A[j];
        }
        A[i] = (z[i] - sum) / R[i][i];
    }
}

static int gnsSolveByChol(double(*LHS)[ALF_MAX_NUM_COEF], double *rhs, double *x, int noEq)
{
    double aux[ALF_MAX_NUM_COEF];     /* Auxiliary vector */
    double U[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF];    /* Upper triangular Cholesky factor of LHS */
    int  i, singular;          /* Looping variable */
    assert(noEq > 0);
    /* The equation to be solved is LHSx = rhs */
    /* Compute upper triangular U such that U'*U = LHS */
    if (gnsCholeskyDec(LHS, U, noEq)) { /* If Cholesky decomposition has been successful */
        singular = 1;
        /* Now, the equation is  U'*U*x = rhs, where U is upper triangular
        * Solve U'*aux = rhs for aux
        */
        gnsTransposeBacksubstitution(U, rhs, aux, noEq);
        /* The equation is now U*x = aux, solve it for x (new motion coefficients) */
        gnsBacksubstitution(U, aux, noEq, x);
    }
    else { /* LHS was singular */
        singular = 0;
        /* Regularize LHS */
        for (i = 0; i < noEq; i++) {
            LHS[i][i] += REG;
        }
        /* Compute upper triangular U such that U'*U = regularized LHS */
        singular = gnsCholeskyDec(LHS, U, noEq);
        if (singular == 1) {
            /* Solve  U'*aux = rhs for aux */
            gnsTransposeBacksubstitution(U, rhs, aux, noEq);
            /* Solve U*x = aux for x */
            gnsBacksubstitution(U, aux, noEq, x);
        }
        else {
            x[0] = 1.0;
            for (i = 1; i < noEq; i++) {
                x[i] = 0.0;
            }
        }
    }
    return singular;
}

static double calculateErrorAbs(double(*A)[ALF_MAX_NUM_COEF], double *b, int size)
{
    int i;
    double error, sum;
    double c[ALF_MAX_NUM_COEF];
    gnsSolveByChol(A, b, c, size);
    sum = 0;
    for (i = 0; i < size; i++) {
        sum += c[i] * b[i];
    }
    error = - sum;
    return error;
}

static double mergeFiltersGreedy(enc_alf_var_t *Enc_ALF, double (*yGlobalSeq)[ALF_MAX_NUM_COEF], double (*EGlobalSeq)[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF],
                          int intervalBest[NO_VAR_BINS][2],
                          double error_tab[NO_VAR_BINS], double error_comb_tab[NO_VAR_BINS],
                          int indexList[NO_VAR_BINS], int available[NO_VAR_BINS],
                          int sqrFiltLength, int noIntervals)
{
    int first, ind, ind1, ind2, i, j, bestToMerge;
    double error, error1, error2, errorMin;

    if (noIntervals == NO_VAR_BINS) {
        for (ind = 0; ind < NO_VAR_BINS; ind++) {
            indexList[ind] = ind;
            available[ind] = 1;
 
            memcpy(Enc_ALF->m_y_merged[ind], yGlobalSeq[ind], sizeof(double)*sqrFiltLength);
            for (i = 0; i < sqrFiltLength; i++) {
                memcpy(Enc_ALF->m_E_merged[ind][i], EGlobalSeq[ind][i], sizeof(double)*sqrFiltLength);
            }
        }
        for (ind = 0; ind < NO_VAR_BINS; ind++) {
            error_tab[ind] = calculateErrorAbs(Enc_ALF->m_E_merged[ind], Enc_ALF->m_y_merged[ind], sqrFiltLength);
        }
        for (ind = 0; ind < NO_VAR_BINS - 1; ind++) {
            ind1 = indexList[ind];
            ind2 = indexList[ind + 1];
            error1 = error_tab[ind1];
            error2 = error_tab[ind2];
  
            for (i = 0; i < sqrFiltLength; i++) {
                Enc_ALF->m_y_temp[i] = Enc_ALF->m_y_merged[ind1][i] + Enc_ALF->m_y_merged[ind2][i];
                for (j = 0; j < sqrFiltLength; j++) {
                    Enc_ALF->m_E_temp[i][j] = Enc_ALF->m_E_merged[ind1][i][j] + Enc_ALF->m_E_merged[ind2][i][j];
                }
            }
            error_comb_tab[ind1] = calculateErrorAbs(Enc_ALF->m_E_temp, Enc_ALF->m_y_temp, sqrFiltLength) - error1 - error2;
        }
    } else {
        errorMin = 0;
        first = 1;
        bestToMerge = 0;
        for (ind = 0; ind < noIntervals; ind++) {
            error = error_comb_tab[indexList[ind]];
            if ((error < errorMin || first == 1)) {
                errorMin = error;
                bestToMerge = ind;
                first = 0;
            }
        }
        ind1 = indexList[bestToMerge];
        ind2 = indexList[bestToMerge + 1];

        for (i = 0; i < sqrFiltLength; i++) {
            Enc_ALF->m_y_merged[ind1][i] += Enc_ALF->m_y_merged[ind2][i];
            for (j = 0; j < sqrFiltLength; j++) {
                Enc_ALF->m_E_merged[ind1][i][j] += Enc_ALF->m_E_merged[ind2][i][j];
            }
        }
        available[ind2] = 0;
        //update error tables
        error_tab[ind1] = error_comb_tab[ind1] + error_tab[ind1] + error_tab[ind2];
        if (indexList[bestToMerge] > 0) {
            ind1 = indexList[bestToMerge - 1];
            ind2 = indexList[bestToMerge];
            error1 = error_tab[ind1];
            error2 = error_tab[ind2];

            for (i = 0; i < sqrFiltLength; i++) {
                Enc_ALF->m_y_temp[i] = Enc_ALF->m_y_merged[ind1][i] + Enc_ALF->m_y_merged[ind2][i];
                for (j = 0; j < sqrFiltLength; j++) {
                    Enc_ALF->m_E_temp[i][j] = Enc_ALF->m_E_merged[ind1][i][j] + Enc_ALF->m_E_merged[ind2][i][j];
                }
            }
            error_comb_tab[ind1] = calculateErrorAbs(Enc_ALF->m_E_temp, Enc_ALF->m_y_temp, sqrFiltLength) - error1 - error2;
        }
        if (indexList[bestToMerge + 1] < NO_VAR_BINS - 1) {
            ind1 = indexList[bestToMerge];
            ind2 = indexList[bestToMerge + 2];
            error1 = error_tab[ind1];
            error2 = error_tab[ind2];

            for (i = 0; i < sqrFiltLength; i++) {
                Enc_ALF->m_y_temp[i] = Enc_ALF->m_y_merged[ind1][i] + Enc_ALF->m_y_merged[ind2][i];
                for (j = 0; j < sqrFiltLength; j++) {
                    Enc_ALF->m_E_temp[i][j] = Enc_ALF->m_E_merged[ind1][i][j] + Enc_ALF->m_E_merged[ind2][i][j];
                }
            }
            error_comb_tab[ind1] = calculateErrorAbs(Enc_ALF->m_E_temp, Enc_ALF->m_y_temp, sqrFiltLength) - error1 - error2;
        }
        ind = 0;
        for (i = 0; i < NO_VAR_BINS; i++) {
            if (available[i] == 1) {
                indexList[ind] = i;
                ind++;
            }
        }
    }
    errorMin = 0;
    for (ind = 0; ind < noIntervals; ind++) {
        errorMin += error_tab[indexList[ind]];
    }
    for (ind = 0; ind < noIntervals - 1; ind++) {
        intervalBest[ind][0] = indexList[ind];
        intervalBest[ind][1] = indexList[ind + 1] - 1;
    }
    intervalBest[noIntervals - 1][0] = indexList[noIntervals - 1];
    intervalBest[noIntervals - 1][1] = NO_VAR_BINS - 1;
    return (errorMin);
}

static void add_A(double (*Amerged)[ALF_MAX_NUM_COEF], double (*A)[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF], int start, int stop, int size)
{
    int i, j, ind;          /* Looping variable */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            Amerged[i][j] = 0;
            for (ind = start; ind <= stop; ind++) {
                Amerged[i][j] += A[ind][i][j];
            }
        }
    }
}

static void add_b(double *bmerged, double (*b)[ALF_MAX_NUM_COEF], int start, int stop, int size)
{
    int i, ind;          /* Looping variable */
    for (i = 0; i < size; i++) {
        bmerged[i] = 0;
        for (ind = start; ind <= stop; ind++) {
            bmerged[i] += b[ind][i];
        }
    }
}

static void roundFiltCoeff(int *FilterCoeffQuan, double *FilterCoeff, int sqrFiltLength, int factor)
{
    int i;
    double diff;
    int diffInt, sign;
    for (i = 0; i < sqrFiltLength; i++) {
        sign = (FilterCoeff[i] > 0) ? 1 : -1;
        diff = FilterCoeff[i] * sign;
        diffInt = (int)(diff * (double)factor + 0.5);
        FilterCoeffQuan[i] = diffInt * sign;
    }
}

static double calculateErrorCoeffProvided(double (*A)[ALF_MAX_NUM_COEF], double *b, double *c, int size)
{
    int i, j;
    double error, sum = 0;
    error = 0;
    for (i = 0; i < size; i++) { //diagonal
        sum = 0;
        for (j = i + 1; j < size; j++) {
            sum += (A[j][i] + A[i][j]) * c[j];
        }
        error += (A[i][i] * c[i] + sum - 2 * b[i]) * c[i];
    }
    return error;
}

static double alf_quant_int_flt_coef(double *filterCoeff, int *filterCoeffQuant, double (*E)[ALF_MAX_NUM_COEF], double *y, int sqrFiltLength)
{
    double error;
    int filterCoeffQuantMod[ALF_MAX_NUM_COEF];
    int factor = (1 << ((int)ALF_NUM_BIT_SHIFT));
    int i;
    int quantCoeffSum, minInd, targetCoeffSumInt, k, diff;
    double targetCoeffSum, errMin;

    gnsSolveByChol(E, y, filterCoeff, sqrFiltLength);
    targetCoeffSum = 0;
    for (i = 0; i < sqrFiltLength; i++) {
        targetCoeffSum += (tbl_weights_shape_1sym[i] * filterCoeff[i] * factor);
    }
    targetCoeffSumInt = ROUND(targetCoeffSum);
    roundFiltCoeff(filterCoeffQuant, filterCoeff, sqrFiltLength, factor);
    quantCoeffSum = 0;
    for (i = 0; i < sqrFiltLength; i++) {
        quantCoeffSum += tbl_weights_shape_1sym[i] * filterCoeffQuant[i];
    }
    while (quantCoeffSum != targetCoeffSumInt) {
        if (quantCoeffSum > targetCoeffSumInt) {
            diff = quantCoeffSum - targetCoeffSumInt;
            errMin = 0;
            minInd = -1;
            for (k = 0; k < sqrFiltLength; k++) {
                if (tbl_weights_shape_1sym[k] <= diff) {
                    for (i = 0; i < sqrFiltLength; i++) {
                        filterCoeffQuantMod[i] = filterCoeffQuant[i];
                    }
                    filterCoeffQuantMod[k]--;
                    for (i = 0; i < sqrFiltLength; i++) {
                        filterCoeff[i] = (double)filterCoeffQuantMod[i] / (double)factor;
                    }
                    error = calculateErrorCoeffProvided(E, y, filterCoeff, sqrFiltLength);
                    if (error < errMin || minInd == -1) {
                        errMin = error;
                        minInd = k;
                    }
                } // if (weights(k)<=diff)
            } // for (k=0; k<sqrFiltLength; k++)
            filterCoeffQuant[minInd]--;
        } else {
            diff = targetCoeffSumInt - quantCoeffSum;
            errMin = 0;
            minInd = -1;
            for (k = 0; k < sqrFiltLength; k++) {
                if (tbl_weights_shape_1sym[k] <= diff) {
                    for (i = 0; i < sqrFiltLength; i++) {
                        filterCoeffQuantMod[i] = filterCoeffQuant[i];
                    }
                    filterCoeffQuantMod[k]++;
                    for (i = 0; i < sqrFiltLength; i++) {
                        filterCoeff[i] = (double)filterCoeffQuantMod[i] / (double)factor;
                    }
                    error = calculateErrorCoeffProvided(E, y, filterCoeff, sqrFiltLength);
                    if (error < errMin || minInd == -1) {
                        errMin = error;
                        minInd = k;
                    }
                } // if (weights(k)<=diff)
            } // for (k=0; k<sqrFiltLength; k++)
            filterCoeffQuant[minInd]++;
        }
        quantCoeffSum = 0;
        for (i = 0; i < sqrFiltLength; i++) {
            quantCoeffSum += tbl_weights_shape_1sym[i] * filterCoeffQuant[i];
        }
    }
    com_alf_check_coef(filterCoeffQuant, sqrFiltLength);
    for (i = 0; i < sqrFiltLength; i++) {
        filterCoeff[i] = (double)filterCoeffQuant[i] / (double)factor;
    }
    error = calculateErrorCoeffProvided(E, y, filterCoeff, sqrFiltLength);

    return (error);
}

static double alf_find_coef(enc_alf_var_t *Enc_ALF, double(*EGlobalSeq)[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF], double(*yGlobalSeq)[ALF_MAX_NUM_COEF], int(*filterCoeffSeq)[ALF_MAX_NUM_COEF],
    int(*filterCoeffQuantSeq)[ALF_MAX_NUM_COEF], int intervalBest[NO_VAR_BINS][2], int varIndTab[NO_VAR_BINS], int sqrFiltLength,
    int filters_per_fr, double errorTabForce0Coeff[NO_VAR_BINS][2])
{
    int filterCoeffQuant[ALF_MAX_NUM_COEF];
    double filterCoeff[ALF_MAX_NUM_COEF];
    double error;
    int k, filtNo;

    error = 0;
    for (filtNo = 0; filtNo < filters_per_fr; filtNo++) {
        add_A(Enc_ALF->m_E_temp, EGlobalSeq, intervalBest[filtNo][0], intervalBest[filtNo][1], sqrFiltLength);
        add_b(Enc_ALF->m_y_temp, yGlobalSeq, intervalBest[filtNo][0], intervalBest[filtNo][1], sqrFiltLength);

        // Find coeffcients
        errorTabForce0Coeff[filtNo][1] = alf_quant_int_flt_coef(filterCoeff, filterCoeffQuant, Enc_ALF->m_E_temp, Enc_ALF->m_y_temp, sqrFiltLength);
        errorTabForce0Coeff[filtNo][0] = 0;
        error += errorTabForce0Coeff[filtNo][1];
        for (k = 0; k < sqrFiltLength; k++) {
            filterCoeffSeq[filtNo][k] = filterCoeffQuant[k];
            filterCoeffQuantSeq[filtNo][k] = filterCoeffQuant[k];
        }
    }
    for (filtNo = 0; filtNo < filters_per_fr; filtNo++) {
        for (k = intervalBest[filtNo][0]; k <= intervalBest[filtNo][1]; k++) {
            varIndTab[k] = filtNo;
        }
    }
    return (error);
}

static void alf_find_best_flt_var(enc_alf_var_t *Enc_ALF, double(*ySym)[ALF_MAX_NUM_COEF], double(*ESym)[ALF_MAX_NUM_COEF][ALF_MAX_NUM_COEF], int(*filterCoeffSym)[ALF_MAX_NUM_COEF],
    int *filters_per_fr_best, int varIndTab[], double lambda_val, int numMaxFilters)
{
    int filterCoeffSymQuant[NO_VAR_BINS][ALF_MAX_NUM_COEF];
    int filters_per_fr, firstFilt, interval[NO_VAR_BINS][2], intervalBest[NO_VAR_BINS][2];
    double  lagrangian, lagrangianMin;
    int sqrFiltLength;
    double errorForce0CoeffTab[NO_VAR_BINS][2];
    double error_tab[NO_VAR_BINS], error_comb_tab[NO_VAR_BINS];
    int indexList[NO_VAR_BINS], available[NO_VAR_BINS];

    sqrFiltLength = (int)ALF_MAX_NUM_COEF;

    // zero all variables
    memset(varIndTab, 0, sizeof(int)*NO_VAR_BINS);
    for (int i = 0; i < NO_VAR_BINS; i++) {
        memset(filterCoeffSym[i], 0, sizeof(int)*ALF_MAX_NUM_COEF);
        memset(filterCoeffSymQuant[i], 0, sizeof(int)*ALF_MAX_NUM_COEF);
    }
    firstFilt = 1;
    lagrangianMin = 0;
    filters_per_fr = NO_VAR_BINS;
    while (filters_per_fr >= 1) {
        mergeFiltersGreedy(Enc_ALF, ySym, ESym, interval, error_tab, error_comb_tab, indexList, available, sqrFiltLength, filters_per_fr);

        alf_find_coef(Enc_ALF, ESym, ySym, filterCoeffSym, filterCoeffSymQuant, interval,
            varIndTab, sqrFiltLength, filters_per_fr, errorForce0CoeffTab);
        lagrangian = xfindBestCoeffCodMethod(filterCoeffSymQuant, sqrFiltLength, filters_per_fr,
            errorForce0CoeffTab, lambda_val);
        if (lagrangian < lagrangianMin || firstFilt == 1 || filters_per_fr == numMaxFilters) {
            firstFilt = 0;
            lagrangianMin = lagrangian;
            (*filters_per_fr_best) = filters_per_fr;
            memcpy(intervalBest, interval, NO_VAR_BINS * 2 * sizeof(int));
        }
        filters_per_fr--;
    }
    alf_find_coef(Enc_ALF, ESym, ySym, filterCoeffSym, filterCoeffSymQuant, intervalBest,
        varIndTab, sqrFiltLength, (*filters_per_fr_best), errorForce0CoeffTab);
    if (*filters_per_fr_best == 1) {
        memset(varIndTab, 0, sizeof(int)*NO_VAR_BINS);
    }
}

static void alf_derive_flt_coef(enc_alf_var_t *Enc_ALF, int compIdx, enc_alf_corr_t *alfCorr, com_alf_pic_param_t *alfFiltParam, int maxNumFilters, double lambda)
{
    switch (compIdx) {
    case Y_C: {
        int numFilters;
        int lambdaForMerge = (int)lambda;
        memset(Enc_ALF->m_varIndTab, 0, sizeof(int)*NO_VAR_BINS);
        alf_find_best_flt_var(Enc_ALF, alfCorr->yCorr, alfCorr->ECorr, alfFiltParam->coeffmulti, &numFilters, Enc_ALF->m_varIndTab, lambdaForMerge, maxNumFilters);
        xcodeFiltCoeff(Enc_ALF->m_varIndTab, numFilters, alfFiltParam);
        break;
    }
    case U_C:
    case V_C: {
        double coef[ALF_MAX_NUM_COEF];
        alfFiltParam->filters_per_group = 1;
        gnsSolveByChol(alfCorr->ECorr[0], alfCorr->yCorr[0], coef, ALF_MAX_NUM_COEF);
        xQuantFilterCoef(coef, alfFiltParam->coeffmulti[0]);
        predictALFCoeff(alfFiltParam->coeffmulti, ALF_MAX_NUM_COEF, alfFiltParam->filters_per_group);
        break;
    }
    default: {
        printf("Not a legal component ID\n");
        assert(0);
        exit(-1);
    }
    }
}

static void alf_decide_pic_param(enc_alf_var_t *Enc_ALF, com_alf_pic_param_t *alfPictureParam, enc_alf_corr_t *alfCorr, double lambdaLuma)
{
    //todo: chroma need different lambdas? lambdaWeighting needed?

    for (int compIdx = 0; compIdx < N_C; compIdx++) {
        com_alf_pic_param_t *alfParam = &alfPictureParam[compIdx];
        enc_alf_corr_t *picCorr = &alfCorr[compIdx];
        alfParam->alf_flag = 1;
        alf_derive_flt_coef(Enc_ALF, compIdx, picCorr, alfParam, NO_VAR_BINS, lambdaLuma);
    }
}

static void alf_get_filter_coefs(enc_pic_t *ep, lbac_t *lbac, com_alf_pic_param_t *alfPictureParam, double lambda)
{
    enc_alf_var_t *Enc_ALF = &ep->Enc_ALF;
    enc_alf_corr_t alfPicCorr[N_C];
    com_alf_pic_param_t tmp_alf_param[N_C];
    double costMin = 1.7e+308;

    for (int compIdx = 0; compIdx < N_C; compIdx++) {
        tmp_alf_param[compIdx].alf_flag = 0;
        tmp_alf_param[compIdx].filters_per_group = 1;
        tmp_alf_param[compIdx].componentID = compIdx;
    }
    memset(alfPicCorr, 0, sizeof(alfPicCorr));

    accumulate_lcu_corr(ep, alfPicCorr, Enc_ALF->m_alfCorr, TRUE);
    alf_decide_pic_param(Enc_ALF, tmp_alf_param, alfPicCorr, lambda);

    for (int i = 0; i < ALF_REDESIGN_ITERATION; i++) {
        if (i != 0) {
            accumulate_lcu_corr(ep, alfPicCorr, Enc_ALF->m_alfCorr, FALSE);
            alf_decide_pic_param(Enc_ALF, tmp_alf_param, alfPicCorr, lambda);
        }
        double cost = alf_decision_all_lcu(ep, lbac, tmp_alf_param, lambda, TRUE, Enc_ALF->m_alfCorr, NULL, NULL, 0, NULL, NULL, NULL, NULL, 0)
                    + estimateALFBitrateInPicHeader(tmp_alf_param) * lambda;

        if (cost < costMin) {
            costMin = cost;
            for (int compIdx = 0; compIdx < N_C; compIdx++) {
                com_alf_copy_param(&alfPictureParam[compIdx], &tmp_alf_param[compIdx]);
            }
        }
    }
}

int uavs3e_alf_frame(enc_pic_t *ep, com_pic_t *pic_rec, com_pic_t *pic_org, double lambda)
{
    lbac_t lbac_alf;
    lbac_t *lbac = &lbac_alf;
    com_pic_t *alf_rec = ep->pic_alf_Rec;
    com_alf_pic_param_t *alf_param = ep->Enc_ALF.m_alfPictureParam;

    pel *pResY = pic_rec->y;
    pel *pDecY = alf_rec->y;
    pel *pOrgY = pic_org->y;

    pel *pResUV[2] = { pic_rec->u, pic_rec->v };
    pel *pDecUV[2] = { alf_rec->u, alf_rec->v };
    pel *pOrgUV[2] = { pic_org->u, pic_org->v };

    alf_rec->stride_luma   = pic_rec->stride_luma;
    alf_rec->stride_chroma = pic_rec->stride_chroma;

    com_alf_copy_frm(alf_rec, pic_rec);

    lbac_reset(lbac);
    lbac_ctx_init(&lbac->h);

    lambda *= (ep->info.bit_depth_internal == 10) ? ep->info.qp_offset_bit_depth : 1;

    alf_get_statistics(ep, pic_org, alf_rec);
    alf_get_filter_coefs(ep, lbac, alf_param, lambda);
    alf_decision_all_lcu(ep, lbac, alf_param, lambda, FALSE, NULL, pOrgY, pOrgUV, pic_org->stride_luma, pDecY, pDecUV, pResY, pResUV, pic_rec->stride_luma);

    for (int i = 0; i < N_C; i++) {
        ep->pic_alf_on[i] = alf_param[i].alf_flag;
    }
    return COM_OK;
}

