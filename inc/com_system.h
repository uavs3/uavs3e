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

#ifndef _COM_PORT_H_
#define _COM_PORT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "com_thread.h"

/*****************************************************************************
 * basic types
 *****************************************************************************/
#if defined(WIN32) || defined(WIN64)
typedef __int8                     s8;
typedef unsigned __int8            u8;
typedef __int16                    s16;
typedef unsigned __int16           u16;
typedef __int32                    s32;
typedef unsigned __int32           u32;
typedef __int64                    s64;
typedef unsigned __int64           u64;
#elif defined(__GNUC__)
#include <stdint.h>
typedef int8_t                     s8;
typedef uint8_t                    u8;
typedef int16_t                    s16;
typedef uint16_t                   u16;
typedef int32_t                    s32;
typedef uint32_t                   u32;
typedef int64_t                    s64;
typedef uint64_t                   u64;
#else
typedef signed char                s8;
typedef unsigned char              u8;
typedef signed short               s16;
typedef unsigned short             u16;
typedef signed int                 s32;
typedef unsigned int               u32;
#if defined(X86_64) && !defined(_MSC_VER) /* for 64bit-Linux */
typedef signed long                s64;
typedef unsigned long              u64;
#else
typedef signed long long           s64;
typedef unsigned long long         u64;
#endif
#endif

typedef  const  u8 tab_u8;
typedef  const  s8 tab_s8;
typedef  const u16 tab_u16;
typedef  const s16 tab_s16;
typedef  const u32 tab_u32;
typedef  const s32 tab_s32;

#ifndef NULL
#define NULL                      (void*)0
#endif

typedef int BOOL;
#define TRUE  1
#define FALSE 0

/*****************************************************************************
 * limit constant
 *****************************************************************************/
#define COM_UINT16_MAX          ((u16)0xFFFF)
#define COM_UINT16_MIN          ((u16)0x0)
#define COM_INT16_MAX           ((s16)0x7FFF)
#define COM_INT16_MIN           ((s16)0x8000)

#define COM_UINT_MAX            ((u32)0xFFFFFFFF)
#define COM_UINT_MIN            ((u32)0x0)
#define COM_INT_MAX             ((int)0x7FFFFFFF)
#define COM_INT_MIN             ((int)0x80000000)

#define COM_UINT32_MAX          ((u32)0xFFFFFFFF)
#define COM_UINT32_MIN          ((u32)0x0)
#define COM_INT32_MAX           ((s32)0x7FFFFFFF)
#define COM_INT32_MIN           ((s32)0x80000000)

#define COM_UINT64_MAX          ((u64)0xFFFFFFFFFFFFFFFFL)
#define COM_UINT64_MIN          ((u64)0x0L)
#define COM_INT64_MAX           ((s64)0x7FFFFFFFFFFFFFFFL)
#define COM_INT64_MIN           ((s64)0x8000000000000000L)

#define COM_INT18_MAX           ((s32)(131071))
#define COM_INT18_MIN           ((s32)(-131072))

/*****************************************************************************
 * inline defines
 *****************************************************************************/

#ifdef __GNUC__
#    define AVS3_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > x || __GNUC__ == x && __GNUC_MINOR__ >= y)
#else
#    define AVS3_GCC_VERSION_AT_LEAST(x,y) 0
#endif

#define avs3_inline inline

#if AVS3_GCC_VERSION_AT_LEAST(3,1)
#    define avs3_always_inline __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#    define avs3_always_inline __forceinline
#else
#    define avs3_always_inline inline
#endif

#if AVS3_GCC_VERSION_AT_LEAST(3,1)
#    define avs3_no_inline __attribute__((noinline))
#elif defined(_MSC_VER)
#    define avs3_no_inline __declspec(noinline)
#else
#    define avs3_no_inline
#endif


/*****************************************************************************
 * memory operations
 *****************************************************************************/
#define com_malloc(size)          uavs3e_align_malloc((size))
#define com_mfree(m)              if(m){uavs3e_align_free(m); (m) = NULL; }

#define com_mcpy(dst,src,size)    memcpy((dst), (src), (size))
#define com_mset(dst,v,size)      memset((dst), (v), (size))
#define com_mset_x64a(dst,v,size) memset((dst), (v), (size))
#define com_mset_x128(dst,v,size) memset((dst), (v), (size))
#define com_mcmp(dst,src,size)    memcmp((dst), (src), (size))

#define ALIGN_SHIFT  5
#define ALIGN_BASIC (1 << ALIGN_SHIFT)
#define ALIGN_MASK (ALIGN_BASIC - 1)
#define ALIGN_POINTER(x) (x + ALIGN_MASK - (((intptr_t)x + ALIGN_MASK) & ((intptr_t)ALIGN_MASK)))

#define GIVE_BUFFER(d, p, s) { (d) = (void*)(p); p += s; p = ALIGN_POINTER(p); }
#define GIVE_BUFFERV(d, p, s, v) { (d) = (void*)(p); p += s; p = ALIGN_POINTER(p); memset(d, v, s); }

/* ---------------------------------------------------------------------------
* date align
*/
#if defined(_WIN32) && !defined(__GNUC__)
#define DECLARE_ALIGNED(var, n) __declspec(align(n)) var
#else
#define DECLARE_ALIGNED(var, n) var __attribute__((aligned (n)))
#endif
#define ALIGNED_32(var)    DECLARE_ALIGNED(var, 32)
#define ALIGNED_16(var)    DECLARE_ALIGNED(var, 16)
#define ALIGNED_8(var)     DECLARE_ALIGNED(var,  8)
#define ALIGNED_4(var)     DECLARE_ALIGNED(var,  4)
#define COM_ALIGN(val, align)   ((((val) + (align) - 1) / (align)) * (align))

#define uavs3e_prefetch(a,b) _mm_prefetch((const char*)a,b)

typedef union uavs3e_union16_t {
    u16 i;
    u8  c[2];
} union16_t;

typedef union uavs3e_union32_t {
    u32 i;
    u16 b[2];
    u8  c[4];
} union32_t;

typedef union uavs3e_union64_t {
    u64 i;
    u32 a[2];
    u16 b[4];
    u8  c[8];
} union64_t;

typedef struct uavs3e_uint128_t {
    u64    i[2];
} uint128_t;

typedef union uavs3e_union128_t {
    uint128_t i;
    u64 a[2];
    u32 b[4];
    u16 c[8];
    u8  d[16];
} union128_t;

#define M16(src)                (((union16_t *)(src))->i)
#define M32(src)                (((union32_t *)(src))->i)
#define M64(src)                (((union64_t *)(src))->i)
#define M128(src)               (((union128_t*)(src))->i)

#define CP16(dst,src)           M16 (dst) = M16 (src)
#define CP32(dst,src)           M32 (dst) = M32 (src)
#define CP64(dst,src)           M64 (dst) = M64 (src)
#define CP128(dst,src)          M128(dst) = M128(src)


/*****************************************************************************
 *  assert
 *****************************************************************************/

#include <assert.h>
#define com_assert(x) \
    {if(!(x)){assert(0);}}
#define com_assert_r(x) \
    {if(!(x)){assert(0); return;}}
#define com_assert_rv(x,r) \
    {if(!(x)){assert(0); return (r);}}
#define com_assert_g(x,g) \
    {if(!(x)){assert(0); goto g;}}
#define com_assert_gv(x,r,v,g) \
    {if(!(x)){assert(0); (r)=(v); goto g;}}


/*****************************************************************************
 *  basic operation
 *****************************************************************************/

#define COM_ABS(a)                     abs(a)
#define COM_ABS64(a)                   (((a)^((a)>>63)) - ((a)>>63))
#define COM_ABS32(a)                   (((a)^((a)>>31)) - ((a)>>31))
#define COM_ABS16(a)                   (((a)^((a)>>15)) - ((a)>>15))

#define COM_MAX(a,b)                   (((a) > (b)) ? (a) : (b))
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))
#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))
#define COM_CLIP(n,min,max)            (((n)>(max))? (max) : (((n)<(min))? (min) : (n)))

#define COM_SIGN(x)                    (((x) < 0) ? -1 : 1)
#define COM_SIGN_GET(val)              ((val<0)? 1: 0)
#define COM_SIGN_SET(val, sign)        ((sign)? -val : val)
#define COM_SIGN_GET16(val)            (((val)>>15) & 1)
#define COM_SIGN_SET16(val, sign)      (((val) ^ ((s16)((sign)<<15)>>15)) + (sign))

#define CONV_LOG2(v)                   (com_tbl_log2[v])

#if defined(_MSC_VER)
#define COM_LOG2F(x) (logf((float)(x)) * 1.44269504088896405f)
#define COM_LOG2(x) (log((double)(x)) * 1.4426950408889640513713538072172)
#else
#define COM_LOG2F(x) log2f(x)
#define COM_LOG2(x)  log2(x)
#endif


#endif /* _COM_PORT_H_ */