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
#define SLICE_I      1 
#define SLICE_P      2 
#define SLICE_B      3 

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
};

/* rpl structure */
#define MAX_RPLS  32
#define MAX_REFS  17

typedef struct uavs3e_com_rpl_t {
    int slice_type;
    int poc;
    int num;
    int active;
    int ref_poc[MAX_REFS];
    int delta_doi[MAX_REFS];
} com_rpl_t;

#ifdef __cplusplus
}
#endif


#endif // #ifndef _COM_API_H_