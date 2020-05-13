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

#ifndef _COM_API_H_
#define _COM_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define BIT_DEPTH 8

#if (BIT_DEPTH == 8)
    typedef unsigned char     pel; /* pixel type */
#else
    typedef unsigned short    pel; /* pixel type */
#endif

/*****************************************************************************
 * return values and error code
 *****************************************************************************/

#define COM_OK_NO_MORE_FRM              ( 205)      /* no more frames, but it is OK */
#define COM_OK_OUT_NOT_AVAILABLE        ( 204)      /* progress success, but output is not available temporarily */
#define COM_OK                          (   0)
#define COM_ERR                         (  -1)      /* generic error */
#define COM_ERR_INVALID_ARGUMENT        (-101)
#define COM_ERR_OUT_OF_MEMORY           (-102)
#define COM_ERR_UNSUPPORTED             (-103)
#define COM_ERR_UNEXPECTED              (-104)
#define COM_ERR_UNKNOWN               (-32767)      /* unknown error */

/*****************************************************************************
 * slice type
 *****************************************************************************/
#define SLICE_NULL 0
#define SLICE_I    1 
#define SLICE_P    2 
#define SLICE_B    3 

/*****************************************************************************
* rc type
*****************************************************************************/
#define RC_TYPE_NULL 0
#define RC_TYPE_CRF  1
#define RC_TYPE_ABR  2
#define RC_TYPE_CBR  3


/*****************************************************************************
 * image buffer format
 *****************************************************************************/

#define MAX_PLANES 3

typedef struct uavs3e_com_img_t com_img_t;

struct uavs3e_com_img_t {
    int        num_planes;          /* number of plane */
    long long  pts, dts;            /* time-stamps */
    long long  ptr;

    int        width [MAX_PLANES];  /* width (in unit of pixel) */
    int        height[MAX_PLANES];  /* height (in unit of pixel) */
    int        stride[MAX_PLANES];  /* buffer stride (in unit of byte) */
    void      *planes[MAX_PLANES];  /* address of each plane */
    int        pad   [MAX_PLANES];  /* padding size (in unit of pixel) */
    void      *buf   [MAX_PLANES];  /* address of actual allocated buffer */

    /* life cycle management */
    int        refcnt;

	/*for AQ*/
	com_img_t  *list[2];
	int        cucost_done;

	double*    intra_satd;
	double*    propagateCost;

};

#ifdef __cplusplus
}
#endif


#endif // #ifndef _COM_API_H_