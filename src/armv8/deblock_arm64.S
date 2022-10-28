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

#include "def_arm64.S"

#if defined(__arm64__)

#if !COMPILE_10BIT
//8bit未修改
/*****************************************************************************************************
*  void uavs3e_deblock_ver_luma_arm64(pel *SrcPtr, int stride, int Alpha, int Beta, int flt_flag)
*  SrcPtr->x0, stride->x1, Alpha->x2, Beta->x3, flt_flag->x4
******************************************************************************************************/
function uavs3e_deblock_ver_luma_arm64

    and x1, x1, #0xffffffff
    // prepare data
    dup v25.8h, w2              // save Alpha
    dup v24.8h, w3              // save Beta
    sub x0, x0, #4              // src-4
    mov x2, x0                  // src-4

    // prepare flt_flag[0] & flt_flag[1]
    and w5, w4, #1
    lsr w4, w4, #8
    neg w5, w5
    and w6, w4, #1
    neg w6, w6

    dup   v31.2S, w5            // v31.4s: flt_flag[0] flt_flag[0] flt_flag[1] flt_flag[1]
    ins   v31.S[2], w6
    ins   v31.S[3], w6

    sub x5, x1, #4
    ld4 {v0.B, v1.B, v2.B, v3.B}[0], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[0], [x0], x5
    ld4 {v0.B, v1.B, v2.B, v3.B}[1], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[1], [x0], x5
    ld4 {v0.B, v1.B, v2.B, v3.B}[2], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[2], [x0], x5
    ld4 {v0.B, v1.B, v2.B, v3.B}[3], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[3], [x0], x5
    ld4 {v0.B, v1.B, v2.B, v3.B}[4], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[4], [x0], x5
    ld4 {v0.B, v1.B, v2.B, v3.B}[5], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[5], [x0], x5
    ld4 {v0.B, v1.B, v2.B, v3.B}[6], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[6], [x0], x5
    ld4 {v0.B, v1.B, v2.B, v3.B}[7], [x0], #4
    ld4 {v4.B, v5.B, v6.B, v7.B}[7], [x0]

    uxtl v0.8h, v0.8b       // L3
    uxtl v1.8h, v1.8b       // L2
    uxtl v2.8h, v2.8b       // L1
    uxtl v3.8h, v3.8b       // L0
    uxtl v4.8h, v4.8b       // R0
    uxtl v5.8h, v5.8b       // R1
    uxtl v6.8h, v6.8b       // R2
    uxtl v7.8h, v7.8b       // R3

    movi v22.8h, #2
    uabd v16.8h, v2.8h, v3.8h       // COM_ABS(L1 - L0)
    uabd v17.8h, v4.8h, v5.8h       // COM_ABS(R0 - R1)
    cmhi v18.8h, v24.8h, v16.8h     // COM_ABS(L1 - L0) < Beta
    cmhi v19.8h, v24.8h, v17.8h     // COM_ABS(R0 - R1) < Beta
    and  v18.16b, v18.16b, v22.16b  // FlatnessL
    and  v19.16b, v19.16b, v22.16b  // FlatnessR

    uabd v20.8h, v1.8h, v3.8h       // COM_ABS(L2 - L0)
    uabd v21.8h, v4.8h, v6.8h       // COM_ABS(R0 - R2)
    cmhi v20.8h, v24.8h, v20.8h     // COM_ABS(L2 - L0) < Beta
    cmhi v21.8h, v24.8h, v21.8h     // COM_ABS(R0 - R2) < Beta

    movi v23.8h, #1
    uabd v26.8h, v4.8h, v3.8h       // ABS(R0 - L0)
    cmhi v27.8h, v25.8h, v26.8h     // ABS(R0 - L0) < Alpha
    ushr v28.8h, v24.8h, #2         // Beta/4

    and  v20.16b, v20.16b, v23.16b
    and  v21.16b, v21.16b, v23.16b
    add  v29.8h, v18.8h, v20.8h     // FlatnessL++ (saved)
    add  v19.8h, v19.8h, v21.8h     // FlatnessR++

    add  v30.8h, v19.8h, v29.8h     // flt = FlatnessL + FlatnessR (saved)

    cmge v16.8h, v28.8h, v16.8h     // COM_ABS(L1 - L0) <= Beta / 4
    cmge v17.8h, v28.8h, v17.8h     // COM_ABS(R0 - R1) <= Beta / 4
    and  v16.16b, v16.16b, v17.16b  // (COM_ABS(L1 - L0) <= Beta / 4) && (COM_ABS(R1 - R0) <= Beta / 4)

    cmeq v17.8h, v2.8h, v3.8h       // L0 == L1
    cmeq v18.8h, v4.8h, v5.8h       // R1 == R0
    uabd v19.8h, v2.8h, v5.8h       // COM_ABS(L1 - R1)

    and  v16.16b, v16.16b, v27.16b  // v16 && ABS(R0 - L0) < Alpha
    and  v17.16b, v17.16b, v18.16b  // (R1 == R0) && (L0 == L1)
    cmeq v18.8h, v29.8h, v22.8h     // FlatnessL(v29) == 2(v22)
    cmhi v19.8h, v24.8h, v19.8h     // COM_ABS(L1 - R1) < Beta

    // flt == 6
    movi v26.8h, #6
    movi v20.8h, #0                 // fs = 0
    movi v21.8h, #1
    movi v23.8h, #3
    movi v24.8h, #4
    movi v25.8h, #5
    cmeq v27.8h, v30.8h, v26.8h     // flt == 6
    bif  v24.16b, v23.16b, v16.16b
    bit  v20.16b, v24.16b, v27.16b  // fs34
    movi v24.8h, #4

    // flt == 5
    movi v16.8h, #2             // v16 = 2
    cmeq v27.8h, v30.8h, v25.8h     // flt == 5
    bit  v16.16b, v23.16b, v17.16b  // fs23
    bit  v20.16b, v16.16b, v27.16b  // fs234

    // flt == 4
    cmeq v27.8h, v30.8h, v24.8h     // flt == 4
    bit  v21.16b, v22.16b, v18.16b  // fs12
    bit  v20.16b, v21.16b, v27.16b  // fs1234
    movi v21.8h, #1                 // reset v21 = 1

    // flt == 3
    movi v17.8h, #0
    cmeq v27.8h, v30.8h, v23.8h     // flt == 3
    bit  v17.16b, v21.16b, v19.16b  // fs01
    bit  v20.16b, v17.16b, v27.16b  // fs01234

    and  v21.16b, v31.16b, v20.16b  // fs & flt_flag

    // fs == 1
    add v16.8h, v3.8h, v4.8h        // L0 + R0 (saved)
    shl v26.8h, v3.8h, #1           // L0 << 1 (saved)
    shl v27.8h, v4.8h, #1           // R0 << 1 (saved)
    add v17.8h, v26.8h, v16.8h      // L0 + (L0 << 1) + R0
    add v18.8h, v27.8h, v16.8h      // L0 + (R0 << 1) + R0

    movi  v19.8h, #1
    urshr v30.8h, v17.8h, #2
    urshr v31.8h, v18.8h, #2
    cmeq  v17.8h, v21.8h, v19.8h    // if fs == 1
    bif   v30.16b, v3.16b, v17.16b
    bif   v31.16b, v4.16b, v17.16b

    // fs == 2
    shl   v16.8h, v16.8h, #1        // (L0 + R0)<<1 (saved)
    add   v17.8h, v2.8h, v4.8h      // L1 + R0
    add   v18.8h, v3.8h, v5.8h      // L0 + R1
    shl   v19.8h, v2.8h, #1         // L1<<1
    shl   v20.8h, v5.8h, #1         // R1<<1
    add   v17.8h, v16.8h, v17.8h    // (L0+R0)<<1 + L1 + R0
    add   v18.8h, v16.8h, v18.8h    // (L0+R0)<<1 + L0 + R1
    shl   v22.8h, v3.8h, #3         // L0<<3
    shl   v23.8h, v4.8h, #3         // R0<<3
    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1
    add   v17.8h, v17.8h, v22.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3
    add   v18.8h, v18.8h, v23.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3
    urshr v28.8h, v17.8h, #4        // ((L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3)>>4
    urshr v29.8h, v18.8h, #4        // ((L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3)>>4
    movi  v17.8h, #2
    cmeq  v17.8h, v21.8h, v17.8h    // fs == 2
    bit   v30.16b, v28.16b, v17.16b
    bit   v31.16b, v29.16b, v17.16b

    // fs == 3; update src[0] src[-1]
    shl   v16.8h, v16.8h, #1        // (L0+R0)<<2 --> V16
    shl   v17.8h, v2.8h, #2         // L1<<2
    shl   v18.8h, v5.8h, #2         // R1<<2
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h      // L1 + R2

    add   v17.8h, v17.8h, v26.8h    // L0<<1 + L1<<2
    add   v18.8h, v18.8h, v27.8h    // R0<<1 + R1<<2
    add   v19.8h, v16.8h, v19.8h    // (L0+R0)<<2 + L2+R1
    add   v20.8h, v16.8h, v20.8h    // (L0+R0)<<2 + L1+R2

    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<2 + L2+R1 + L0<<1+L1<<2
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<2 + L1+R2 + R0<<1+R1<<2

    movi  v23.8h, #3
    urshr v17.8h, v17.8h, #4        // ((L0+R0)<<2 + (L2 + R1) + L0<<1+L1<<2) >> 4
    urshr v18.8h, v18.8h, #4        // ((L0+R0)<<2 + (L1 + R2) + R0<<1+R1<<2) >> 4

    cmeq  v25.8h, v21.8h, v23.8h    // fs == 3 (saved)
    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // fs == 3; calculate src[-2] src[1]
    add   v17.8h, v1.8h, v4.8h      // L2+R0
    add   v18.8h, v6.8h, v3.8h      // R2+L0
    shl   v19.8h, v1.8h, #1         // L2<<1
    shl   v20.8h, v6.8h, #1         // R2<<1
    shl   v22.8h, v2.8h, #3         // L1<<3
    shl   v23.8h, v5.8h, #3         // R1<<3

    add   v17.8h, v17.8h, v19.8h    // L2+R0+L2<<1
    add   v18.8h, v18.8h, v20.8h    // R2+L0+R2<<1

    shl   v19.8h, v3.8h, #2         // L0<<2
    shl   v20.8h, v4.8h, #2         // R0<<2
    add   v17.8h, v17.8h, v22.8h    // (L2+R0+L2<<1) + (L1<<3)
    add   v18.8h, v18.8h, v23.8h    // (R2+L0+R2<<1) + (R1<<3)

    add   v17.8h, v17.8h, v19.8h    // ((L2+R0+L2<<1) + (L1<<3)) + L0<<2
    add   v18.8h, v18.8h, v20.8h    //

    urshr v28.8h, v17.8h, #4
    urshr v29.8h, v18.8h, #4
    bif   v28.16b, v2.16b, v25.16b
    bif   v29.16b, v5.16b, v25.16b  // update L1_dst and R1_dst

    // fs == 4
    movi v17.8h, #4
    cmeq v25.8h, v21.8h, v17.8h
    mov  v19.d[0], v25.d[1]
    addp v24.2s, v25.2s, v19.2s
    mov  w6, v24.s[0]
    mov  w7, v24.s[1]
    orr  w6, w6, w7
    cmp  w6, #0
    beq  deblock_ver_filtered

    // if fs == 4 exist
    // calculate L0 and R0
    shl   v16.8h, v16.8h, #1        // (R0+L0)<<3 (saved)
    shl   v17.8h, v2.8h, #3         // L1<<3
    shl   v18.8h, v5.8h, #3         //
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h
    add   v17.8h, v17.8h, v26.8h    // L1*8 + L0*2
    add   v18.8h, v18.8h, v27.8h
    shl   v22.8h, v19.8h, #1        // (L2 + R1)*2
    shl   v23.8h, v20.8h, #1
    add   v17.8h, v17.8h, v16.8h    // (L1*8 + L0*2) + ((R0+L0)*8)
    add   v18.8h, v18.8h, v16.8h
    add   v19.8h, v19.8h, v22.8h    // (L2 + R1)*3
    add   v20.8h, v20.8h, v23.8h

    add   v17.8h, v17.8h, v19.8h
    add   v18.8h, v18.8h, v20.8h
    urshr v17.8h, v17.8h, #5        // ((L1*8 + L0*2) + ((R0+L0)*8) + (L2 + R1)*3)>>5
    urshr v18.8h, v18.8h, #5

    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // calculate L1 and R1
    add   v21.8h, v1.8h, v2.8h      // L2 + L1
    add   v22.8h, v6.8h, v5.8h
    add   v19.8h, v2.8h, v4.8h      // L1 + R0
    add   v20.8h, v5.8h, v3.8h
    add   v17.8h, v21.8h, v3.8h     // (L2+L1) + L0
    add   v18.8h, v22.8h, v4.8h
    add   v19.8h, v19.8h, v27.8h    // (L1+R0) + R0*2
    add   v20.8h, v20.8h, v26.8h
    shl   v17.8h, v17.8h, #2        // (L0 + L1 + L2)*4
    shl   v18.8h, v18.8h, #2
    add   v17.8h, v17.8h, v19.8h    // L1+R0 + R0*2 + (L0 + L1 + L2)*4
    add   v18.8h, v18.8h, v20.8h

    srshr v17.8h, v17.8h, #4        //
    srshr v18.8h, v18.8h, #4
    bit   v28.16b, v17.16b, v25.16b //
    bit   v29.16b, v18.16b, v25.16b

    // calculate L2 and R2
    add   v16.8h, v3.8h, v4.8h      // L0 + R0
    add   v17.8h, v21.8h, v0.8h     // L1 + L2 + L3
    add   v18.8h, v22.8h, v7.8h
    shl   v17.8h, v17.8h, #1        // (L1 + L2 + L3)*2
    shl   v18.8h, v18.8h, #1
    add   v17.8h, v17.8h, v16.8h
    add   v18.8h, v18.8h, v16.8h

    urshr v17.8h, v17.8h, #3        // ((L1 + L2 + L3)*2 + L0 + R0 + 4)>>3
    urshr v18.8h, v18.8h, #3        //

deblock_ver_filtered:
    bif   v17.16b, v1.16b, v25.16b
    bif   v18.16b, v6.16b, v25.16b

    xtn  v0.8b, v0.8h
    xtn  v1.8b, v17.8h
    xtn  v2.8b, v28.8h
    xtn  v3.8b, v30.8h
    xtn  v4.8b, v31.8h
    xtn  v5.8b, v29.8h
    xtn  v6.8b, v18.8h
    xtn  v7.8b, v7.8h

    st4 {V0.B, V1.B, V2.B, V3.B}[0], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[0], [x2], x5
    st4 {V0.B, V1.B, V2.B, V3.B}[1], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[1], [x2], x5
    st4 {V0.B, V1.B, V2.B, V3.B}[2], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[2], [x2], x5
    st4 {V0.B, V1.B, V2.B, V3.B}[3], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[3], [x2], x5
    st4 {V0.B, V1.B, V2.B, V3.B}[4], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[4], [x2], x5
    st4 {V0.B, V1.B, V2.B, V3.B}[5], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[5], [x2], x5
    st4 {V0.B, V1.B, V2.B, V3.B}[6], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[6], [x2], x5
    st4 {V0.B, V1.B, V2.B, V3.B}[7], [x2], #4
    st4 {V4.B, V5.B, V6.B, V7.B}[7], [x2]

    ret


/*****************************************************************************************************
 *  void uavs3e_deblock_ver_chroma_arm64(pel *srcu, pel *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag);
 *  SrcPtrU->x0, SrcPtrV->x1, stride->x2, alpha_u->x3, beta_u->x4, alpha_v->x5, beta_v->x6, flt_flag->x7
 ******************************************************************************************************/
function uavs3e_deblock_ver_chroma_arm64

    and x2, x2, #0xffffffff

    add w8, w3, w5, lsl #16
    add w9, w4, w6, lsl #16

    dup v25.4s, w8                  // v25.8h: alphau alphav alphau alphav alphau alphav alphau alphav
    dup v24.4s, w9                  // v24.8h: betau, betav

    lsr w9, w7, #1
    lsr w8, w7, #9
    and w9, w9, #1
    and w10, w8, #1
    neg w9, w9                      // flag0
    neg w10, w10                      // flag1

    dup v26.2s, w9
    dup v27.2s, w10                  // v26: flag0 flag0 flag1 flag1
    mov v26.d[1], v27.d[0]

    sub x0, x0, #3                  // srcu -= 3*sizeof(pel)
    sub x1, x1, #3                  // srcv -= 3*sizeof(pel)
    sub x5, x2, #4
    mov x3, x0
    mov x4, x1

    ld4 {v0.b, v1.b, v2.b, v3.b}[0], [x0], #4
    ld2 {v4.b, v5.b}[0], [x0], x5
    ld4 {v0.b, v1.b, v2.b, v3.b}[1], [x1], #4
    ld2 {v4.b, v5.b}[1], [x1], x5
    ld4 {v0.b, v1.b, v2.b, v3.b}[2], [x0], #4
    ld2 {v4.b, v5.b}[2], [x0], x5
    ld4 {v0.b, v1.b, v2.b, v3.b}[3], [x1], #4
    ld2 {v4.b, v5.b}[3], [x1], x5
    ld4 {v0.b, v1.b, v2.b, v3.b}[4], [x0], #4
    ld2 {v4.b, v5.b}[4], [x0], x5
    ld4 {v0.b, v1.b, v2.b, v3.b}[5], [x1], #4
    ld2 {v4.b, v5.b}[5], [x1], x5
    ld4 {v0.b, v1.b, v2.b, v3.b}[6], [x0], #4
    ld2 {v4.b, v5.b}[6], [x0], x5
    ld4 {v0.b, v1.b, v2.b, v3.b}[7], [x1], #4
    ld2 {v4.b, v5.b}[7], [x1]

    uxtl v5.8h, v5.8b               // R2
    uxtl v4.8h, v4.8b               // R1
    uxtl v3.8h, v3.8b               // R0
    uxtl v2.8h, v2.8b               // L0
    uxtl v1.8h, v1.8b               // L1
    uxtl v0.8h, v0.8b               // L2

    uabd v16.8h, v2.8h , v1.8h      // abs(L0-L1)
    uabd v17.8h, v3.8h , v4.8h      // abs(R0-R1)
    uabd v29.8h, v3.8h , v2.8h      // abs(R0-L0)
    uabd v30.8h, v0.8h , v2.8h      // abs(L2-L0)
    uabd v31.8h, v3.8h , v5.8h      // abs(R0-R2)

    cmhi v18.8h, v24.8h, v16.8h     // abs(L0-L1) < beta
    cmhi v19.8h, v24.8h, v17.8h     // abs(R0-R1) < beta
    ushr v28.8h, v24.8h, #2         // beta/4
    cmhi v29.8h, v25.8h, v29.8h     // abs(R0-L0) < alpha
    cmhi v30.8h, v24.8h, v30.8h     // abs(L2-L0) < beta
    cmhi v31.8h, v24.8h, v31.8h     // abs(R2-R0) < beta
    cmge v16.8h, v28.8h, v16.8h     // abs(L0-L1) <= beta/4
    cmge v17.8h, v28.8h, v17.8h     // abs(R0-R1) <= beta/4

    and  v18.16b, v18.16b, v19.16b  // abs(L0-L1) < beta && abs(R0-R1) < beta
    and  v30.16b, v30.16b, v31.16b  // abs(L2-L0) < beta && abs(R2-R0) < beta
    and  v16.16b, v16.16b, v17.16b  // abs(L0-L1) <= beta/4 && abs(R0-R1) <= beta/4

    and  v31.16b, v18.16b, v26.16b  // flt0: mask L0, R0
    and  v30.16b, v30.16b, v29.16b  //
    and  v16.16b, v31.16b, v16.16b  //
    and  v30.16b, v30.16b, v16.16b  // flt1: mask L1, R1

    // filter L0, R0
    shl  v24.8h, v3.8h, #1          // R0 * 2
    shl  v25.8h, v2.8h, #1
    add  v18.8h, v2.8h, v0.8h       // L0 + L2
    add  v19.8h, v3.8h, v5.8h
    shl  v20.8h, v1.8h, #3          // L1 * 8
    shl  v21.8h, v4.8h, #3
    add  v16.8h, v24.8h, v20.8h     // (R0 * 2) + (L1 * 8)
    add  v17.8h, v25.8h, v21.8h
    shl  v22.8h, v18.8h, #1         // (L0 + L2)*2
    shl  v23.8h, v19.8h, #1
    add  v16.8h, v16.8h, v18.8h     // (L0 + L2) + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v19.8h
    add  v16.8h, v16.8h, v22.8h     // (L0 + L2)*3 + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v23.8h

    urshr v16.8h, v16.8h, #4
    urshr v17.8h, v17.8h, #4

    add  v18.8h, v1.8h, v3.8h       // L1 + R0
    add  v19.8h, v4.8h, v2.8h
    shl  v20.8h, v2.8h, #3          // L0 * 8
    shl  v21.8h, v3.8h, #3
    shl  v22.8h, v18.8h, #1         // (L1 + R0)*2
    shl  v23.8h, v19.8h, #1
    add  v20.8h, v20.8h, v25.8h     // L0 * 10
    add  v21.8h, v21.8h, v24.8h
    add  v18.8h, v18.8h, v22.8h     // (L1 + R0)*3
    add  v19.8h, v19.8h, v23.8h
    add  v18.8h, v18.8h, v20.8h     // (L1 + R0)*3 + L0*10
    add  v19.8h, v19.8h, v21.8h

    urshr v18.8h, v18.8h, #4
    urshr v19.8h, v19.8h, #4

    bit  v1.16b, v16.16b, v30.16b   // L1
    bit  v2.16b, v18.16b, v31.16b   // L0
    bit  v3.16b, v19.16b, v31.16b   // R0
    bit  v4.16b, v17.16b, v30.16b   // R1

    //store L1, L0, R0, R1
    xtn v0.8b, v0.8h
    xtn v1.8b, v1.8h
    xtn v2.8b, v2.8h
    xtn v3.8b, v3.8h
    xtn v4.8b, v4.8h
    xtn v5.8b, v5.8h

    st4 {v0.b, v1.b, v2.b, v3.b}[0], [x3], #4
    st2 {v4.b, v5.b}[0], [x3], x5
    st4 {v0.b, v1.b, v2.b, v3.b}[1], [x4], #4
    st2 {v4.b, v5.b}[1], [x4], x5
    st4 {v0.b, v1.b, v2.b, v3.b}[2], [x3], #4
    st2 {v4.b, v5.b}[2], [x3], x5
    st4 {v0.b, v1.b, v2.b, v3.b}[3], [x4], #4
    st2 {v4.b, v5.b}[3], [x4], x5
    st4 {v0.b, v1.b, v2.b, v3.b}[4], [x3], #4
    st2 {v4.b, v5.b}[4], [x3], x5
    st4 {v0.b, v1.b, v2.b, v3.b}[5], [x4], #4
    st2 {v4.b, v5.b}[5], [x4], x5
    st4 {v0.b, v1.b, v2.b, v3.b}[6], [x3], #4
    st2 {v4.b, v5.b}[6], [x3], x5
    st4 {v0.b, v1.b, v2.b, v3.b}[7], [x4], #4
    st2 {v4.b, v5.b}[7], [x4]
    ret

/*****************************************************************************************************
 *  void uavs3e_deblock_hor_luma_arm64(pel *SrcPtr, int stride, int Alpha, int Beta, int flt_flag)
 *  SrcPtr->x0, stride->x1, Alpha->x2, Beta->x3, flt_flag->x4
 ******************************************************************************************************/
function uavs3e_deblock_hor_luma_arm64

    and x1, x1, #0xffffffff
    // prepare data
    dup v25.8h, w2          // save Alpha
    dup v24.8h, w3          // save Beta
    sub x0, x0, x1, lsl #2 // src - 4*stride
    add x2, x0, x1         // src - 3*stride

    // prepare flt_flag[0] & flt_flag[1]
    and w5, w4, #1
    neg w5, w5
    lsr w4, w4, #8
    and w6, w4, #1
    neg w6, w6

    dup v31.2s, w5          // save flt_flag[1] in vector $V31.4S[0-1]
    ins v31.s[2], w6
    ins v31.s[3], w6        // save flt_flag[0] in vector $V31.4S[2-3]

    ld1 {v0.8b}, [x0], x1   // L3
    ld1 {v1.8b}, [x0], x1   // L2
    ld1 {v2.8b}, [x0], x1   // L1
    ld1 {v3.8b}, [x0], x1   // L0
    ld1 {v4.8b}, [x0], x1   // R0
    ld1 {v5.8b}, [x0], x1   // R1
    ld1 {v6.8b}, [x0], x1   // R2
    ld1 {v7.8b}, [x0]       // R3

    uxtl v0.8h, v0.8b       // L3
    uxtl v1.8h, v1.8b       // L2
    uxtl v2.8h, v2.8b       // L1
    uxtl v3.8h, v3.8b       // L0
    uxtl v4.8h, v4.8b       // R0
    uxtl v5.8h, v5.8b       // R1
    uxtl v6.8h, v6.8b       // R2
    uxtl v7.8h, v7.8b       // R3

    movi v22.8h, #2
    uabd v16.8h, v2.8h, v3.8h       // COM_ABS(L1 - L0)
    uabd v17.8h, v4.8h, v5.8h       // COM_ABS(R0 - R1)
    cmhi v18.8h, v24.8h, v16.8h     // COM_ABS(L1 - L0) < Beta
    cmhi v19.8h, v24.8h, v17.8h     // COM_ABS(R0 - R1) < Beta
    and  v18.16b, v18.16b, v22.16b  // FlatnessL
    and  v19.16b, v19.16b, v22.16b  // FlatnessR

    uabd v20.8h, v1.8h, v3.8h       // COM_ABS(L2 - L0)
    uabd v21.8h, v4.8h, v6.8h       // COM_ABS(R0 - R2)
    cmhi v20.8h, v24.8h, v20.8h     // COM_ABS(L2 - L0) < Beta
    cmhi v21.8h, v24.8h, v21.8h     // COM_ABS(R0 - R2) < Beta

    movi v23.8h, #1
    uabd v26.8h, v4.8h, v3.8h       // ABS(R0 - L0)
    cmhi v27.8h, v25.8h, v26.8h     // ABS(R0 - L0) < Alpha
    ushr v28.8h, v24.8h, #2         // Beta/4

    and  v20.16b, v20.16b, v23.16b
    and  v21.16b, v21.16b, v23.16b
    add  v29.8h, v18.8h, v20.8h     // FlatnessL++ (saved)
    add  v19.8h, v19.8h, v21.8h     // FlatnessR++

    add  v30.8h, v19.8h, v29.8h     // flt = FlatnessL + FlatnessR (saved)

    cmge v16.8h, v28.8h, v16.8h     // COM_ABS(L1 - L0) <= Beta / 4
    cmge v17.8h, v28.8h, v17.8h     // COM_ABS(R0 - R1) <= Beta / 4
    and  v16.16b, v16.16b, v17.16b  // (COM_ABS(L1 - L0) <= Beta / 4) && (COM_ABS(R1 - R0) <= Beta / 4)

    cmeq v17.8h, v2.8h, v3.8h       // L0 == L1
    cmeq v18.8h, v4.8h, v5.8h       // R1 == R0
    uabd v19.8h, v2.8h, v5.8h       // COM_ABS(L1 - R1)

    and  v16.16b, v16.16b, v27.16b  // v16 && ABS(R0 - L0) < Alpha
    and  v17.16b, v17.16b, v18.16b  // (R1 == R0) && (L0 == L1)
    cmeq v18.8h, v29.8h, v22.8h     // FlatnessL(v29) == 2(v22)
    cmhi v19.8h, v24.8h, v19.8h     // COM_ABS(L1 - R1) < Beta

    // flt == 6
    movi v26.8h, #6
    movi v20.8h, #0                 // fs = 0
    movi v21.8h, #1
    movi v23.8h, #3
    movi v24.8h, #4
    movi v25.8h, #5
    cmeq v27.8h, v30.8h, v26.8h     // flt == 6
    bif  v24.16b, v23.16b, v16.16b
    bit  v20.16b, v24.16b, v27.16b  // fs34
    movi v24.8h, #4

    // flt == 5
    movi  v16.8h, #2             // v16 = 2
    cmeq v27.8h, v30.8h, v25.8h     // flt == 5
    bit  v16.16b, v23.16b, v17.16b  // fs23
    bit  v20.16b, v16.16b, v27.16b  // fs234

    // flt == 4
    cmeq v27.8h, v30.8h, v24.8h     // flt == 4
    bit  v21.16b, v22.16b, v18.16b  // fs12
    bit  v20.16b, v21.16b, v27.16b  // fs1234
    movi v21.8h, #1                 // reset v21 = 1

    // flt == 3
    movi v17.8h, #0
    cmeq v27.8h, v30.8h, v23.8h     // flt == 3
    bit  v17.16b, v21.16b, v19.16b  // fs01
    bit  v20.16b, v17.16b, v27.16b  // fs01234

    and  v21.16b, v31.16b, v20.16b  // fs & flt_flag

    // fs == 1
    add v16.8h, v3.8h, v4.8h        // L0 + R0 (saved)
    shl v26.8h, v3.8h, #1           // L0 << 1 (saved)
    shl v27.8h, v4.8h, #1           // R0 << 1 (saved)
    add v17.8h, v26.8h, v16.8h      // L0 + (L0 << 1) + R0
    add v18.8h, v27.8h, v16.8h      // L0 + (R0 << 1) + R0

    movi  v19.8h, #1
    urshr v30.8h, v17.8h, #2
    urshr v31.8h, v18.8h, #2
    cmeq  v17.8h, v21.8h, v19.8h    // if fs == 1
    bif   v30.16b, v3.16b, v17.16b
    bif   v31.16b, v4.16b, v17.16b

    // fs == 2
    shl   v16.8h, v16.8h, #1        // (L0 + R0)<<1 (saved)
    add   v17.8h, v2.8h, v4.8h      // L1 + R0
    add   v18.8h, v3.8h, v5.8h      // L0 + R1
    shl   v19.8h, v2.8h, #1         // L1<<1
    shl   v20.8h, v5.8h, #1         // R1<<1
    add   v17.8h, v16.8h, v17.8h    // (L0+R0)<<1 + L1 + R0
    add   v18.8h, v16.8h, v18.8h    // (L0+R0)<<1 + L0 + R1
    shl   v22.8h, v3.8h, #3         // L0<<3
    shl   v23.8h, v4.8h, #3         // R0<<3
    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1
    add   v17.8h, v17.8h, v22.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3
    add   v18.8h, v18.8h, v23.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3
    urshr v28.8h, v17.8h, #4        // ((L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3)>>4
    urshr v29.8h, v18.8h, #4        // ((L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3)>>4
    movi  v17.8h, #2
    cmeq  v17.8h, v21.8h, v17.8h    // fs == 2
    bit   v30.16b, v28.16b, v17.16b
    bit   v31.16b, v29.16b, v17.16b

    // fs == 3; update src[0] src[-1]
    shl   v16.8h, v16.8h, #1        // (L0+R0)<<2 --> V16
    shl   v17.8h, v2.8h, #2         // L1<<2
    shl   v18.8h, v5.8h, #2         // R1<<2
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h      // L1 + R2

    add   v17.8h, v17.8h, v26.8h    // L0<<1 + L1<<2
    add   v18.8h, v18.8h, v27.8h    // R0<<1 + R1<<2
    add   v19.8h, v16.8h, v19.8h    // (L0+R0)<<2 + L2+R1
    add   v20.8h, v16.8h, v20.8h    // (L0+R0)<<2 + L1+R2

    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<2 + L2+R1 + L0<<1+L1<<2
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<2 + L1+R2 + R0<<1+R1<<2

    movi  v23.8h, #3
    urshr v17.8h, v17.8h, #4        // ((L0+R0)<<2 + (L2 + R1) + L0<<1+L1<<2) >> 4
    urshr v18.8h, v18.8h, #4        // ((L0+R0)<<2 + (L1 + R2) + R0<<1+R1<<2) >> 4

    cmeq  v25.8h, v21.8h, v23.8h    // fs == 3 (saved)
    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // fs == 3; calculate src[-2] src[1]
    add   v17.8h, v1.8h, v4.8h      // L2+R0
    add   v18.8h, v6.8h, v3.8h      // R2+L0
    shl   v19.8h, v1.8h, #1         // L2<<1
    shl   v20.8h, v6.8h, #1         // R2<<1
    shl   v22.8h, v2.8h, #3         // L1<<3
    shl   v23.8h, v5.8h, #3         // R1<<3

    add   v17.8h, v17.8h, v19.8h    // L2+R0+L2<<1
    add   v18.8h, v18.8h, v20.8h    // R2+L0+R2<<1

    shl   v19.8h, v3.8h, #2         // L0<<2
    shl   v20.8h, v4.8h, #2         // R0<<2
    add   v17.8h, v17.8h, v22.8h    // (L2+R0+L2<<1) + (L1<<3)
    add   v18.8h, v18.8h, v23.8h    // (R2+L0+R2<<1) + (R1<<3)

    add   v17.8h, v17.8h, v19.8h    // ((L2+R0+L2<<1) + (L1<<3)) + L0<<2
    add   v18.8h, v18.8h, v20.8h    //

    urshr v28.8h, v17.8h, #4
    urshr v29.8h, v18.8h, #4
    bif   v28.16b, v2.16b, v25.16b
    bif   v29.16b, v5.16b, v25.16b  // update L1_dst and R1_dst

    // fs == 4
    movi v17.8h, #4
    cmeq v25.8h, v21.8h, v17.8h
    mov  v19.d[0], v25.d[1]
    addp v24.2s, v25.2s, v19.2s
    mov  w6, v24.s[0]
    mov  w7, v24.s[1]
    orr  w6, w6, w7
    cmp  w6, #0
    beq  deblock_hor_filtered

    // if fs == 4 exist
    // calculate L0 and R0
    shl   v16.8h, v16.8h, #1        // (R0+L0)<<3 (saved)
    shl   v17.8h, v2.8h, #3         // L1<<3
    shl   v18.8h, v5.8h, #3         //
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h
    add   v17.8h, v17.8h, v26.8h    // L1*8 + L0*2
    add   v18.8h, v18.8h, v27.8h
    shl   v22.8h, v19.8h, #1        // (L2 + R1)*2
    shl   v23.8h, v20.8h, #1
    add   v17.8h, v17.8h, v16.8h    // (L1*8 + L0*2) + ((R0+L0)*8)
    add   v18.8h, v18.8h, v16.8h
    add   v19.8h, v19.8h, v22.8h    // (L2 + R1)*3
    add   v20.8h, v20.8h, v23.8h

    add   v17.8h, v17.8h, v19.8h
    add   v18.8h, v18.8h, v20.8h
    urshr v17.8h, v17.8h, #5        // ((L1*8 + L0*2) + ((R0+L0)*8) + (L2 + R1)*3)>>5
    urshr v18.8h, v18.8h, #5

    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // calculate L1 and R1
    add   v21.8h, v1.8h, v2.8h      // L2 + L1
    add   v22.8h, v6.8h, v5.8h
    add   v19.8h, v2.8h, v4.8h      // L1 + R0
    add   v20.8h, v5.8h, v3.8h
    add   v17.8h, v21.8h, v3.8h     // (L2+L1) + L0
    add   v18.8h, v22.8h, v4.8h
    add   v19.8h, v19.8h, v27.8h    // (L1+R0) + R0*2
    add   v20.8h, v20.8h, v26.8h
    shl   v17.8h, v17.8h, #2        // (L0 + L1 + L2)*4
    shl   v18.8h, v18.8h, #2
    add   v17.8h, v17.8h, v19.8h    // L1+R0 + R0*2 + (L0 + L1 + L2)*4
    add   v18.8h, v18.8h, v20.8h

    srshr v17.8h, v17.8h, #4        //
    srshr v18.8h, v18.8h, #4
    bit   v28.16b, v17.16b, v25.16b //
    bit   v29.16b, v18.16b, v25.16b

    // calculate L2 and R2
    add   v16.8h, v3.8h, v4.8h      // L0 + R0
    add   v17.8h, v21.8h, v0.8h     // L1 + L2 + L3
    add   v18.8h, v22.8h, v7.8h
    shl   v17.8h, v17.8h, #1        // (L1 + L2 + L3)*2
    shl   v18.8h, v18.8h, #1
    add   v17.8h, v17.8h, v16.8h
    add   v18.8h, v18.8h, v16.8h

    urshr v17.8h, v17.8h, #3        // ((L1 + L2 + L3)*2 + L0 + R0 + 4)>>3
    urshr v18.8h, v18.8h, #3        //

    bif   v17.16b, v1.16b, v25.16b
    bif   v18.16b, v6.16b, v25.16b

    xtn  v0.8b, v17.8h
    xtn  v1.8b, v28.8h
    xtn  v2.8b, v30.8h
    xtn  v3.8b, v31.8h
    xtn  v4.8b, v29.8h
    xtn  v5.8b, v18.8h
    st1 {v0.8b}, [x2], x1
    st1 {v1.8b}, [x2], x1
    st1 {v2.8b}, [x2], x1
    st1 {v3.8b}, [x2], x1
    st1 {v4.8b}, [x2], x1
    st1 {v5.8b}, [x2]

    b deblock_hor_end

deblock_hor_filtered:
    add x2, x2, x1
    xtn v1.8b, v28.8h
    xtn v2.8b, v30.8h
    xtn v3.8b, v31.8h
    xtn v4.8b, v29.8h

    st1 {v1.8b}, [x2], x1
    st1 {v2.8b}, [x2], x1
    st1 {v3.8b}, [x2], x1
    st1 {v4.8b}, [x2]

deblock_hor_end:
    ret

/*****************************************************************************************************
 *  void uavs3e_deblock_hor_chroma_arm64(pel *srcu, pel *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag)
 *  srcu->x0, srcv->x1, stride->x2, alpha_u->x3, beta_u->x4, alpha_v->x5, beta_v->x6, flt_flag->x7
 ******************************************************************************************************/
function uavs3e_deblock_hor_chroma_arm64
    sub sp, sp, #96
    add x11, sp, #64
    st1 {v8.2d, v9.2d, v10.2d, v11.2d}, [sp]
    st1 {v12.2d, v13.2d}, [x11]

    and x2, x2, #0xffffffff

    add w3, w3, w5, lsl #16
    add w4, w4, w6, lsl #16
    dup v25.4s, w3                  // v25: alphau alphav alphau alphav alphau alphav alphau alphav
    dup v24.4s, w4                  // v24: betau betav betau betav betau betav betau betav

    lsr w8, w7, #1
    lsr w7, w7, #9
    and w6, w8, #1
    and w7, w7, #1
    neg w6, w6                      // flag0
    neg w7, w7                      // flag1

    dup v6.4h, w6
    dup v7.4h, w7                   // v6: flag0 flag0 flag1 flag1
    mov v6.d[1], v7.d[0]

    lsl x3, x2, #1                  // x3 -> 2*stride
    sub x0, x0, x3                  // srcu - 2*stride
    sub x4, x0, x2                  // srcu - 3*stride
    sub x1, x1, x3                  // srcv - 2*stride
    sub x5, x1, x2                  // srcv - 3*stride

    //load u
    ld1 {v0.s}[0], [x4], x2           // L2
    ld1 {v1.s}[0], [x4], x2           // L1
    ld1 {v2.s}[0], [x4], x2           // L0
    ld1 {v3.s}[0], [x4], x2           // R0
    ld1 {v4.s}[0], [x4], x2           // R1
    ld1 {v5.s}[0], [x4]               // R2

    //load v
    ld1 {v8.s}[0], [x5], x2           // L2
    ld1 {v9.s}[0], [x5], x2           // L1
    ld1 {v10.s}[0], [x5], x2           // L0
    ld1 {v11.s}[0], [x5], x2           // R0
    ld1 {v12.s}[0], [x5], x2           // R1
    ld1 {v13.s}[0], [x5]               // R2

    //uv交织
    zip1 v0.8b, v0.8b, v8.8b
    zip1 v1.8b, v1.8b, v9.8b
    zip1 v2.8b, v2.8b, v10.8b
    zip1 v3.8b, v3.8b, v11.8b
    zip1 v4.8b, v4.8b, v12.8b
    zip1 v5.8b, v5.8b, v13.8b

    uxtl v0.8h, v0.8b               // L2
    uxtl v1.8h, v1.8b               // L1
    uxtl v2.8h, v2.8b               // L0
    uxtl v3.8h, v3.8b               // R0
    uxtl v4.8h, v4.8b               // R1
    uxtl v5.8h, v5.8b               // R2

    uabd v16.8h, v2.8h , v1.8h      // abs(L0-L1)
    uabd v17.8h, v3.8h , v4.8h      // abs(R0-R1)
    uabd v29.8h, v3.8h , v2.8h      // abs(R0-L0)
    uabd v30.8h, v0.8h , v2.8h      // abs(L2-L0)
    uabd v31.8h, v3.8h , v5.8h      // abs(R0-R2)

    cmhi v18.8h, v24.8h, v16.8h     // abs(L0-L1) < beta
    cmhi v19.8h, v24.8h, v17.8h     // abs(R0-R1) < beta
    ushr v28.8h, v24.8h, #2         // beta/4
    cmhi v29.8h, v25.8h, v29.8h     // abs(R0-L0) < alpha
    cmhi v30.8h, v24.8h, v30.8h     // abs(L2-L0) < beta
    cmhi v31.8h, v24.8h, v31.8h     // abs(R2-R0) < beta
    cmge v16.8h, v28.8h, v16.8h     // abs(L0-L1) <= beta/4
    cmge v17.8h, v28.8h, v17.8h     // abs(R0-R1) <= beta/4

    and  v18.16b, v18.16b, v19.16b  // abs(L0-L1) < beta && abs(R0-R1) < beta
    and  v30.16b, v30.16b, v31.16b  // abs(L2-L0) < beta && abs(R2-R0) < beta
    and  v16.16b, v16.16b, v17.16b  // abs(L0-L1) <= beta/4 && abs(R0-R1) <= beta/4

    and  v31.16b, v18.16b, v6.16b   // flt0: mask L0, R0
    and  v30.16b, v30.16b, v29.16b  //
    and  v16.16b, v31.16b, v16.16b  //
    and  v30.16b, v30.16b, v16.16b  // flt1: mask L1, R1

    // filter L0, R0
    shl  v24.8h, v3.8h, #1          // R0 * 2
    shl  v25.8h, v2.8h, #1
    add  v18.8h, v2.8h, v0.8h       // L0 + L2
    add  v19.8h, v3.8h, v5.8h
    shl  v20.8h, v1.8h, #3          // L1 * 8
    shl  v21.8h, v4.8h, #3
    add  v16.8h, v24.8h, v20.8h     // (R0 * 2) + (L1 * 8)
    add  v17.8h, v25.8h, v21.8h
    shl  v22.8h, v18.8h, #1         // (L0 + L2)*2
    shl  v23.8h, v19.8h, #1
    add  v16.8h, v16.8h, v18.8h     // (L0 + L2) + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v19.8h
    add  v16.8h, v16.8h, v22.8h     // (L0 + L2)*3 + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v23.8h

    urshr v16.8h, v16.8h, #4
    urshr v17.8h, v17.8h, #4

    add  v18.8h, v1.8h, v3.8h       // L1 + R0
    add  v19.8h, v4.8h, v2.8h
    shl  v20.8h, v2.8h, #3          // L0 * 8
    shl  v21.8h, v3.8h, #3
    shl  v22.8h, v18.8h, #1         // (L1 + R0)*2
    shl  v23.8h, v19.8h, #1
    add  v20.8h, v20.8h, v25.8h     // L0 * 10
    add  v21.8h, v21.8h, v24.8h
    add  v18.8h, v18.8h, v22.8h     // (L1 + R0)*3
    add  v19.8h, v19.8h, v23.8h
    add  v18.8h, v18.8h, v20.8h     // (L1 + R0)*3 + L0*10
    add  v19.8h, v19.8h, v21.8h

    urshr v18.8h, v18.8h, #4
    urshr v19.8h, v19.8h, #4

    bit  v1.16b, v16.16b, v30.16b   // L1
    bit  v2.16b, v18.16b, v31.16b   // L0
    bit  v3.16b, v19.16b, v31.16b   // R0
    bit  v4.16b, v17.16b, v30.16b   // R1

    //store L1,L0,R0,R1
    xtn v1.8b, v1.8h
    xtn v2.8b, v2.8h
    xtn v3.8b, v3.8h
    xtn v4.8b, v4.8h

    uzp2 v8.8b, v1.8b, v1.8b        //v
    uzp1 v1.8b, v1.8b, v1.8b        //u
    uzp2 v9.8b, v2.8b, v2.8b        //v
    uzp1 v2.8b, v2.8b, v2.8b        //u
    uzp2 v10.8b, v3.8b, v3.8b       //v
    uzp1 v3.8b, v3.8b, v3.8b        //u
    uzp2 v11.8b, v4.8b, v4.8b       //v
    uzp1 v4.8b, v4.8b, v4.8b        //u

    st1 {v1.s}[0], [x0], x2
    st1 {v2.s}[0], [x0], x2
    st1 {v3.s}[0], [x0], x2
    st1 {v4.s}[0], [x0]

    st1 {v8.s}[0], [x1], x2
    st1 {v9.s}[0], [x1], x2
    st1 {v10.s}[0], [x1], x2
    st1 {v11.s}[0], [x1]

    ld1 {v8.2d, v9.2d, v10.2d, v11.2d}, [sp], #64
    ld1 {v12.2d, v13.2d}, [sp], #32


    ret

#else // COMPILE_10BIT == 1

/*****************************************************************************************************
 *  void uavs3e_deblock_hor_luma_arm64(pel *SrcPtr, int stride, int Alpha, int Beta, int flt_flag)
 *  SrcPtr->x0, stride->x1, Alpha->x2, Beta->x3, flt_flag->x4
 ******************************************************************************************************/
function uavs3e_deblock_hor_luma_arm64

    and x1, x1, #0xffffffff
    lsl x1, x1, #1
    // prepare data
    dup v25.8h, w2          // save Alpha
    dup v24.8h, w3          // save Beta
    sub x0, x0, x1, lsl #2  // src - 4*stride
    add x2, x0, x1          // src - 3*stride

    // prepare flt_flag[0] & flt_flag[1]
    and w5, w4, #1
    neg w5, w5
    lsr w4, w4, #8
    and w6, w4, #1
    neg w6, w6

    dup v31.2s, w5          // flt_flag
    ins v31.s[2], w6
    ins v31.s[3], w6

    ld1 {v0.8h}, [x0], x1   // L3
    ld1 {v1.8h}, [x0], x1   // L2
    ld1 {v2.8h}, [x0], x1   // L1
    ld1 {v3.8h}, [x0], x1   // L0
    ld1 {v4.8h}, [x0], x1   // R0
    ld1 {v5.8h}, [x0], x1   // R1
    ld1 {v6.8h}, [x0], x1   // R2
    ld1 {v7.8h}, [x0]       // R3

    movi v22.8h, #2
    uabd v16.8h, v2.8h, v3.8h       // COM_ABS(L1 - L0)
    uabd v17.8h, v4.8h, v5.8h       // COM_ABS(R0 - R1)
    cmhi v18.8h, v24.8h, v16.8h     // COM_ABS(L1 - L0) < Beta
    cmhi v19.8h, v24.8h, v17.8h     // COM_ABS(R0 - R1) < Beta
    and  v18.16b, v18.16b, v22.16b  // FlatnessL
    and  v19.16b, v19.16b, v22.16b  // FlatnessR

    uabd v20.8h, v1.8h, v3.8h       // COM_ABS(L2 - L0)
    uabd v21.8h, v4.8h, v6.8h       // COM_ABS(R0 - R2)
    cmhi v20.8h, v24.8h, v20.8h     // COM_ABS(L2 - L0) < Beta
    cmhi v21.8h, v24.8h, v21.8h     // COM_ABS(R0 - R2) < Beta

    movi v23.8h, #1
    uabd v26.8h, v4.8h, v3.8h       // ABS(R0 - L0)
    cmhi v27.8h, v25.8h, v26.8h     // ABS(R0 - L0) < Alpha
    ushr v28.8h, v24.8h, #2         // Beta/4

    and  v20.16b, v20.16b, v23.16b
    and  v21.16b, v21.16b, v23.16b
    add  v29.8h, v18.8h, v20.8h     // FlatnessL++ (saved)
    add  v19.8h, v19.8h, v21.8h     // FlatnessR++

    add  v30.8h, v19.8h, v29.8h     // flt = FlatnessL + FlatnessR (saved)

    cmge v16.8h, v28.8h, v16.8h     // COM_ABS(L1 - L0) <= Beta / 4
    cmge v17.8h, v28.8h, v17.8h     // COM_ABS(R0 - R1) <= Beta / 4
    and  v16.16b, v16.16b, v17.16b  // (COM_ABS(L1 - L0) <= Beta / 4) && (COM_ABS(R1 - R0) <= Beta / 4)

    cmeq v17.8h, v2.8h, v3.8h       // L0 == L1
    cmeq v18.8h, v4.8h, v5.8h       // R1 == R0
    uabd v19.8h, v2.8h, v5.8h       // COM_ABS(L1 - R1)

    and  v16.16b, v16.16b, v27.16b  // v16 && ABS(R0 - L0) < Alpha
    and  v17.16b, v17.16b, v18.16b  // (R1 == R0) && (L0 == L1)
    cmeq v18.8h, v29.8h, v22.8h     // FlatnessL(v29) == 2(v22)
    cmhi v19.8h, v24.8h, v19.8h     // COM_ABS(L1 - R1) < Beta

    // flt == 6
    movi v26.8h, #6
    movi v20.8h, #0                 // fs = 0
    movi v21.8h, #1
    movi v23.8h, #3
    movi v24.8h, #4
    movi v25.8h, #5
    cmeq v27.8h, v30.8h, v26.8h     // flt == 6
    bif  v24.16b, v23.16b, v16.16b
    bit  v20.16b, v24.16b, v27.16b  // fs34
    movi v24.8h, #4

    // flt == 5
    movi  v16.8h, #2             // v16 = 2
    cmeq v27.8h, v30.8h, v25.8h     // flt == 5
    bit  v16.16b, v23.16b, v17.16b  // fs23
    bit  v20.16b, v16.16b, v27.16b  // fs234

    // flt == 4
    cmeq v27.8h, v30.8h, v24.8h     // flt == 4
    bit  v21.16b, v22.16b, v18.16b  // fs12
    bit  v20.16b, v21.16b, v27.16b  // fs1234
    movi v21.8h, #1                 // reset v21 = 1

    // flt == 3
    movi v17.8h, #0
    cmeq v27.8h, v30.8h, v23.8h     // flt == 3
    bit  v17.16b, v21.16b, v19.16b  // fs01
    bit  v20.16b, v17.16b, v27.16b  // fs01234

    and  v21.16b, v31.16b, v20.16b  // fs & flt_flag

    // fs == 1
    add v16.8h, v3.8h, v4.8h        // L0 + R0 (saved)
    shl v26.8h, v3.8h, #1           // L0 << 1 (saved)
    shl v27.8h, v4.8h, #1           // R0 << 1 (saved)
    add v17.8h, v26.8h, v16.8h      // L0 + (L0 << 1) + R0
    add v18.8h, v27.8h, v16.8h      // L0 + (R0 << 1) + R0

    movi  v19.8h, #1
    urshr v30.8h, v17.8h, #2
    urshr v31.8h, v18.8h, #2
    cmeq  v17.8h, v21.8h, v19.8h    // if fs == 1
    bif   v30.16b, v3.16b, v17.16b
    bif   v31.16b, v4.16b, v17.16b

    // fs == 2
    shl   v16.8h, v16.8h, #1        // (L0 + R0)<<1 (saved)
    add   v17.8h, v2.8h, v4.8h      // L1 + R0
    add   v18.8h, v3.8h, v5.8h      // L0 + R1
    shl   v19.8h, v2.8h, #1         // L1<<1
    shl   v20.8h, v5.8h, #1         // R1<<1
    add   v17.8h, v16.8h, v17.8h    // (L0+R0)<<1 + L1 + R0
    add   v18.8h, v16.8h, v18.8h    // (L0+R0)<<1 + L0 + R1
    shl   v22.8h, v3.8h, #3         // L0<<3
    shl   v23.8h, v4.8h, #3         // R0<<3
    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1
    add   v17.8h, v17.8h, v22.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3
    add   v18.8h, v18.8h, v23.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3
    urshr v28.8h, v17.8h, #4        // ((L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3)>>4
    urshr v29.8h, v18.8h, #4        // ((L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3)>>4
    movi  v17.8h, #2
    cmeq  v17.8h, v21.8h, v17.8h    // fs == 2
    bit   v30.16b, v28.16b, v17.16b
    bit   v31.16b, v29.16b, v17.16b

    // fs == 3; update src[0] src[-1]
    shl   v16.8h, v16.8h, #1        // (L0+R0)<<2 --> V16
    shl   v17.8h, v2.8h, #2         // L1<<2
    shl   v18.8h, v5.8h, #2         // R1<<2
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h      // L1 + R2

    add   v17.8h, v17.8h, v26.8h    // L0<<1 + L1<<2
    add   v18.8h, v18.8h, v27.8h    // R0<<1 + R1<<2
    add   v19.8h, v16.8h, v19.8h    // (L0+R0)<<2 + L2+R1
    add   v20.8h, v16.8h, v20.8h    // (L0+R0)<<2 + L1+R2

    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<2 + L2+R1 + L0<<1+L1<<2
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<2 + L1+R2 + R0<<1+R1<<2

    movi  v23.8h, #3
    urshr v17.8h, v17.8h, #4        // ((L0+R0)<<2 + (L2 + R1) + L0<<1+L1<<2) >> 4
    urshr v18.8h, v18.8h, #4        // ((L0+R0)<<2 + (L1 + R2) + R0<<1+R1<<2) >> 4

    cmeq  v25.8h, v21.8h, v23.8h    // fs == 3 (saved)
    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // fs == 3; calculate src[-2] src[1]
    add   v17.8h, v1.8h, v4.8h      // L2+R0
    add   v18.8h, v6.8h, v3.8h      // R2+L0
    shl   v19.8h, v1.8h, #1         // L2<<1
    shl   v20.8h, v6.8h, #1         // R2<<1
    shl   v22.8h, v2.8h, #3         // L1<<3
    shl   v23.8h, v5.8h, #3         // R1<<3

    add   v17.8h, v17.8h, v19.8h    // L2+R0+L2<<1
    add   v18.8h, v18.8h, v20.8h    // R2+L0+R2<<1

    shl   v19.8h, v3.8h, #2         // L0<<2
    shl   v20.8h, v4.8h, #2         // R0<<2
    add   v17.8h, v17.8h, v22.8h    // (L2+R0+L2<<1) + (L1<<3)
    add   v18.8h, v18.8h, v23.8h    // (R2+L0+R2<<1) + (R1<<3)

    add   v17.8h, v17.8h, v19.8h    // ((L2+R0+L2<<1) + (L1<<3)) + L0<<2
    add   v18.8h, v18.8h, v20.8h    //

    urshr v28.8h, v17.8h, #4
    urshr v29.8h, v18.8h, #4
    bif   v28.16b, v2.16b, v25.16b
    bif   v29.16b, v5.16b, v25.16b  // update L1_dst and R1_dst

    // fs == 4
    movi v17.8h, #4
    cmeq v25.8h, v21.8h, v17.8h
    mov  v19.d[0], v25.d[1]
    addp v24.2s, v25.2s, v19.2s
    mov  w6, v24.s[0]
    mov  w7, v24.s[1]
    orr  w6, w6, w7
    cmp  w6, #0
    beq  deblock_hor_filtered

    // if fs == 4 exist
    // calculate L0 and R0
    shl   v16.8h, v16.8h, #1        // (R0+L0)<<3 (saved)
    shl   v17.8h, v2.8h, #3         // L1<<3
    shl   v18.8h, v5.8h, #3         //
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h
    add   v17.8h, v17.8h, v26.8h    // L1*8 + L0*2
    add   v18.8h, v18.8h, v27.8h
    shl   v22.8h, v19.8h, #1        // (L2 + R1)*2
    shl   v23.8h, v20.8h, #1
    add   v17.8h, v17.8h, v16.8h    // (L1*8 + L0*2) + ((R0+L0)*8)
    add   v18.8h, v18.8h, v16.8h
    add   v19.8h, v19.8h, v22.8h    // (L2 + R1)*3
    add   v20.8h, v20.8h, v23.8h

    add   v17.8h, v17.8h, v19.8h
    add   v18.8h, v18.8h, v20.8h
    urshr v17.8h, v17.8h, #5        // ((L1*8 + L0*2) + ((R0+L0)*8) + (L2 + R1)*3)>>5
    urshr v18.8h, v18.8h, #5

    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // calculate L1 and R1
    add   v21.8h, v1.8h, v2.8h      // L2 + L1
    add   v22.8h, v6.8h, v5.8h
    add   v19.8h, v2.8h, v4.8h      // L1 + R0
    add   v20.8h, v5.8h, v3.8h
    add   v17.8h, v21.8h, v3.8h     // (L2+L1) + L0
    add   v18.8h, v22.8h, v4.8h
    add   v19.8h, v19.8h, v27.8h    // (L1+R0) + R0*2
    add   v20.8h, v20.8h, v26.8h
    shl   v17.8h, v17.8h, #2        // (L0 + L1 + L2)*4
    shl   v18.8h, v18.8h, #2
    add   v17.8h, v17.8h, v19.8h    // L1+R0 + R0*2 + (L0 + L1 + L2)*4
    add   v18.8h, v18.8h, v20.8h

    srshr v17.8h, v17.8h, #4        //
    srshr v18.8h, v18.8h, #4
    bit   v28.16b, v17.16b, v25.16b //
    bit   v29.16b, v18.16b, v25.16b

    // calculate L2 and R2
    add   v16.8h, v3.8h, v4.8h      // L0 + R0
    add   v17.8h, v21.8h, v0.8h     // L1 + L2 + L3
    add   v18.8h, v22.8h, v7.8h
    shl   v17.8h, v17.8h, #1        // (L1 + L2 + L3)*2
    shl   v18.8h, v18.8h, #1
    add   v17.8h, v17.8h, v16.8h
    add   v18.8h, v18.8h, v16.8h

    urshr v17.8h, v17.8h, #3        // ((L1 + L2 + L3)*2 + L0 + R0 + 4)>>3
    urshr v18.8h, v18.8h, #3        //

    bif   v17.16b, v1.16b, v25.16b
    bif   v18.16b, v6.16b, v25.16b

    st1 {v17.8h}, [x2], x1
    st1 {v28.8h}, [x2], x1
    st1 {v30.8h}, [x2], x1
    st1 {v31.8h}, [x2], x1
    st1 {v29.8h}, [x2], x1
    st1 {v18.8h}, [x2]

    b deblock_hor_end

deblock_hor_filtered:
    add x2, x2, x1
    st1 {v28.8h}, [x2], x1
    st1 {v30.8h}, [x2], x1
    st1 {v31.8h}, [x2], x1
    st1 {v29.8h}, [x2]

deblock_hor_end:
    ret

/*****************************************************************************************************
 *  void uavs3e_deblock_hor_chroma_arm64(pel *srcu, pel *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag)
 *  srcu->x0, srcv->x1, stride->x2, alpha_u->x3, beta_u->x4, alpha_v->x5, beta_v->x6, flt_flag->x7
 ******************************************************************************************************/
function uavs3e_deblock_hor_chroma_arm64

    sub sp, sp, #96
    add x11, sp, #64
    st1 {v8.2d, v9.2d, v10.2d, v11.2d}, [sp]
    st1 {v12.2d, v13.2d}, [x11]

    and x2, x2, #0xffffffff

    add w3, w3, w5, lsl #16
    add w4, w4, w6, lsl #16
    dup v25.4s, w3                  // v25: alphau alphav alphau alphav alphau alphav alphau alphav
    dup v24.4s, w4                  // v24: betau betav betau betav betau betav betau betav

    lsl x2, x2, #1

    lsr w8, w7, #1
    lsr w7, w7, #9
    and w6, w8, #1
    and w7, w7, #1
    neg w6, w6                      // flag0
    neg w7, w7                      // flag1

    dup v6.4h, w6
    dup v7.4h, w7                   // v6: flag0 flag0 flag1 flag1
    mov v6.d[1], v7.d[0]

    lsl x3, x2, #1                  // x3 -> 2*stride
    sub x0, x0, x3                  // srcu - 2*stride
    sub x4, x0, x2                  // srcu - 3*stride
    sub x1, x1, x3                  // srcv - 2*stride
    sub x5, x1, x2                  // srcv - 3*stride

    //load u
    ld1 {v0.d}[0], [x4], x2           // L2
    ld1 {v1.d}[0], [x4], x2           // L1
    ld1 {v2.d}[0], [x4], x2           // L0
    ld1 {v3.d}[0], [x4], x2           // R0
    ld1 {v4.d}[0], [x4], x2           // R1
    ld1 {v5.d}[0], [x4]               // R2

    //load v
    ld1 {v8.d}[0], [x5], x2           // L2
    ld1 {v9.d}[0], [x5], x2           // L1
    ld1 {v10.d}[0], [x5], x2           // L0
    ld1 {v11.d}[0], [x5], x2           // R0
    ld1 {v12.d}[0], [x5], x2           // R1
    ld1 {v13.d}[0], [x5]               // R2

    //uv交织
    zip1 v0.8h, v0.8h, v8.8h
    zip1 v1.8h, v1.8h, v9.8h
    zip1 v2.8h, v2.8h, v10.8h
    zip1 v3.8h, v3.8h, v11.8h
    zip1 v4.8h, v4.8h, v12.8h
    zip1 v5.8h, v5.8h, v13.8h

    uabd v16.8h, v2.8h , v1.8h      // abs(L0-L1)
    uabd v17.8h, v3.8h , v4.8h      // abs(R0-R1)
    uabd v29.8h, v3.8h , v2.8h      // abs(R0-L0)
    uabd v30.8h, v0.8h , v2.8h      // abs(L2-L0)
    uabd v31.8h, v3.8h , v5.8h      // abs(R0-R2)

    cmhi v18.8h, v24.8h, v16.8h     // abs(L0-L1) < beta
    cmhi v19.8h, v24.8h, v17.8h     // abs(R0-R1) < beta
    ushr v28.8h, v24.8h, #2         // beta/4
    cmhi v29.8h, v25.8h, v29.8h     // abs(R0-L0) < alpha
    cmhi v30.8h, v24.8h, v30.8h     // abs(L2-L0) < beta
    cmhi v31.8h, v24.8h, v31.8h     // abs(R2-R0) < beta
    cmge v16.8h, v28.8h, v16.8h     // abs(L0-L1) <= beta/4
    cmge v17.8h, v28.8h, v17.8h     // abs(R0-R1) <= beta/4

    and  v18.16b, v18.16b, v19.16b  // abs(L0-L1) < beta && abs(R0-R1) < beta
    and  v30.16b, v30.16b, v31.16b  // abs(L2-L0) < beta && abs(R2-R0) < beta
    and  v16.16b, v16.16b, v17.16b  // abs(L0-L1) <= beta/4 && abs(R0-R1) <= beta/4

    and  v31.16b, v18.16b, v6.16b   // flt0: mask L0, R0
    and  v30.16b, v30.16b, v29.16b  //
    and  v16.16b, v31.16b, v16.16b  //
    and  v30.16b, v30.16b, v16.16b  // flt1: mask L1, R1

    // filter L0, R0
    shl  v24.8h, v3.8h, #1          // R0 * 2
    shl  v25.8h, v2.8h, #1
    add  v18.8h, v2.8h, v0.8h       // L0 + L2
    add  v19.8h, v3.8h, v5.8h
    shl  v20.8h, v1.8h, #3          // L1 * 8
    shl  v21.8h, v4.8h, #3
    add  v16.8h, v24.8h, v20.8h     // (R0 * 2) + (L1 * 8)
    add  v17.8h, v25.8h, v21.8h
    shl  v22.8h, v18.8h, #1         // (L0 + L2)*2
    shl  v23.8h, v19.8h, #1
    add  v16.8h, v16.8h, v18.8h     // (L0 + L2) + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v19.8h
    add  v16.8h, v16.8h, v22.8h     // (L0 + L2)*3 + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v23.8h

    urshr v16.8h, v16.8h, #4
    urshr v17.8h, v17.8h, #4

    add  v18.8h, v1.8h, v3.8h       // L1 + R0
    add  v19.8h, v4.8h, v2.8h
    shl  v20.8h, v2.8h, #3          // L0 * 8
    shl  v21.8h, v3.8h, #3
    shl  v22.8h, v18.8h, #1         // (L1 + R0)*2
    shl  v23.8h, v19.8h, #1
    add  v20.8h, v20.8h, v25.8h     // L0 * 10
    add  v21.8h, v21.8h, v24.8h
    add  v18.8h, v18.8h, v22.8h     // (L1 + R0)*3
    add  v19.8h, v19.8h, v23.8h
    add  v18.8h, v18.8h, v20.8h     // (L1 + R0)*3 + L0*10
    add  v19.8h, v19.8h, v21.8h

    urshr v18.8h, v18.8h, #4
    urshr v19.8h, v19.8h, #4

    bit  v1.16b, v16.16b, v30.16b   // L1
    bit  v2.16b, v18.16b, v31.16b   // L0
    bit  v3.16b, v19.16b, v31.16b   // R0
    bit  v4.16b, v17.16b, v30.16b   // R1

    uzp2 v8.8h, v1.8h, v1.8h        //v
    uzp1 v1.8h, v1.8h, v1.8h        //u
    uzp2 v9.8h, v2.8h, v2.8h        //v
    uzp1 v2.8h, v2.8h, v2.8h        //u
    uzp2 v10.8h, v3.8h, v3.8h       //v
    uzp1 v3.8h, v3.8h, v3.8h        //u
    uzp2 v11.8h, v4.8h, v4.8h       //v
    uzp1 v4.8h, v4.8h, v4.8h        //u

    st1 {v1.2s}, [x0], x2
    st1 {v2.2s}, [x0], x2
    st1 {v3.2s}, [x0], x2
    st1 {v4.2s}, [x0]

    st1 {v8.2s}, [x1], x2
    st1 {v9.2s}, [x1], x2
    st1 {v10.2s}, [x1], x2
    st1 {v11.2s}, [x1]

    ld1 {v8.2d, v9.2d, v10.2d, v11.2d}, [sp], #64
    ld1 {v12.2d, v13.2d}, [sp], #32

    ret

/*****************************************************************************************************
*  void uavs3e_deblock_ver_luma_arm64(pel *SrcPtr, int stride, int Alpha, int Beta, int flt_flag)
*  SrcPtr->x0, stride->x1, Alpha->x2, Beta->x3, flt_flag->x4
******************************************************************************************************/
function uavs3e_deblock_ver_luma_arm64

    and x1, x1, #0xffffffff
    lsl x1, x1, #1

    // prepare data
    dup v25.8h, w2              // save Alpha
    dup v24.8h, w3              // save Beta
    sub x0, x0, #8              // src-4
    mov x2, x0                  // src-4

    // prepare flt_flag[0] & flt_flag[1]
    and w5, w4, #1
    lsr w4, w4, #8
    neg w5, w5
    and w6, w4, #1
    neg w6, w6

    dup   v31.2s, w5
    ins   v31.s[2], w6
    ins   v31.s[3], w6

    sub x5, x1, #8
    ld4 {v0.H, v1.H, v2.H, v3.H}[0], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[0], [x0], x5
    ld4 {v0.H, v1.H, v2.H, v3.H}[1], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[1], [x0], x5
    ld4 {v0.H, v1.H, v2.H, v3.H}[2], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[2], [x0], x5
    ld4 {v0.H, v1.H, v2.H, v3.H}[3], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[3], [x0], x5
    ld4 {v0.H, v1.H, v2.H, v3.H}[4], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[4], [x0], x5
    ld4 {v0.H, v1.H, v2.H, v3.H}[5], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[5], [x0], x5
    ld4 {v0.H, v1.H, v2.H, v3.H}[6], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[6], [x0], x5
    ld4 {v0.H, v1.H, v2.H, v3.H}[7], [x0], #8
    ld4 {v4.H, v5.H, v6.H, v7.H}[7], [x0]

    // v0.8h: L3
    // v1.8h: L2
    // v2.8h: L1
    // v3.8h: L0
    // v4.8h: R0
    // v5.8h: R1
    // v6.8h: R2
    // v7.8h: R3

    movi v22.8h, #2
    uabd v16.8h, v2.8h, v3.8h       // COM_ABS(L1 - L0)
    uabd v17.8h, v4.8h, v5.8h       // COM_ABS(R0 - R1)
    cmhi v18.8h, v24.8h, v16.8h     // COM_ABS(L1 - L0) < Beta
    cmhi v19.8h, v24.8h, v17.8h     // COM_ABS(R0 - R1) < Beta
    and  v18.16b, v18.16b, v22.16b  // FlatnessL
    and  v19.16b, v19.16b, v22.16b  // FlatnessR

    uabd v20.8h, v1.8h, v3.8h       // COM_ABS(L2 - L0)
    uabd v21.8h, v4.8h, v6.8h       // COM_ABS(R0 - R2)
    cmhi v20.8h, v24.8h, v20.8h     // COM_ABS(L2 - L0) < Beta
    cmhi v21.8h, v24.8h, v21.8h     // COM_ABS(R0 - R2) < Beta

    movi v23.8h, #1
    uabd v26.8h, v4.8h, v3.8h       // ABS(R0 - L0)
    cmhi v27.8h, v25.8h, v26.8h     // ABS(R0 - L0) < Alpha
    ushr v28.8h, v24.8h, #2         // Beta/4

    and  v20.16b, v20.16b, v23.16b
    and  v21.16b, v21.16b, v23.16b
    add  v29.8h, v18.8h, v20.8h     // FlatnessL++ (saved)
    add  v19.8h, v19.8h, v21.8h     // FlatnessR++

    add  v30.8h, v19.8h, v29.8h     // flt = FlatnessL + FlatnessR (saved)

    cmge v16.8h, v28.8h, v16.8h     // COM_ABS(L1 - L0) <= Beta / 4
    cmge v17.8h, v28.8h, v17.8h     // COM_ABS(R0 - R1) <= Beta / 4
    and  v16.16b, v16.16b, v17.16b  // (COM_ABS(L1 - L0) <= Beta / 4) && (COM_ABS(R1 - R0) <= Beta / 4)

    cmeq v17.8h, v2.8h, v3.8h       // L0 == L1
    cmeq v18.8h, v4.8h, v5.8h       // R1 == R0
    uabd v19.8h, v2.8h, v5.8h       // COM_ABS(L1 - R1)

    and  v16.16b, v16.16b, v27.16b  // v16 && ABS(R0 - L0) < Alpha
    and  v17.16b, v17.16b, v18.16b  // (R1 == R0) && (L0 == L1)
    cmeq v18.8h, v29.8h, v22.8h     // FlatnessL(v29) == 2(v22)
    cmhi v19.8h, v24.8h, v19.8h     // COM_ABS(L1 - R1) < Beta

    // flt == 6
    movi v26.8h, #6
    movi v20.8h, #0                 // fs = 0
    movi v21.8h, #1
    movi v23.8h, #3
    movi v24.8h, #4
    movi v25.8h, #5
    cmeq v27.8h, v30.8h, v26.8h     // flt == 6
    bif  v24.16b, v23.16b, v16.16b
    bit  v20.16b, v24.16b, v27.16b  // fs34
    movi v24.8h, #4

    // flt == 5
    movi v16.8h, #2             // v16 = 2
    cmeq v27.8h, v30.8h, v25.8h     // flt == 5
    bit  v16.16b, v23.16b, v17.16b  // fs23
    bit  v20.16b, v16.16b, v27.16b  // fs234

    // flt == 4
    cmeq v27.8h, v30.8h, v24.8h     // flt == 4
    bit  v21.16b, v22.16b, v18.16b  // fs12
    bit  v20.16b, v21.16b, v27.16b  // fs1234
    movi v21.8h, #1                 // reset v21 = 1

    // flt == 3
    movi v17.8h, #0
    cmeq v27.8h, v30.8h, v23.8h     // flt == 3
    bit  v17.16b, v21.16b, v19.16b  // fs01
    bit  v20.16b, v17.16b, v27.16b  // fs01234

    and  v21.16b, v31.16b, v20.16b  // fs & flt_flag

    // fs == 1
    add v16.8h, v3.8h, v4.8h        // L0 + R0 (saved)
    shl v26.8h, v3.8h, #1           // L0 << 1 (saved)
    shl v27.8h, v4.8h, #1           // R0 << 1 (saved)
    add v17.8h, v26.8h, v16.8h      // L0 + (L0 << 1) + R0
    add v18.8h, v27.8h, v16.8h      // L0 + (R0 << 1) + R0

    movi  v19.8h, #1
    urshr v30.8h, v17.8h, #2
    urshr v31.8h, v18.8h, #2
    cmeq  v17.8h, v21.8h, v19.8h    // if fs == 1
    bif   v30.16b, v3.16b, v17.16b
    bif   v31.16b, v4.16b, v17.16b

    // fs == 2
    shl   v16.8h, v16.8h, #1        // (L0 + R0)<<1 (saved)
    add   v17.8h, v2.8h, v4.8h      // L1 + R0
    add   v18.8h, v3.8h, v5.8h      // L0 + R1
    shl   v19.8h, v2.8h, #1         // L1<<1
    shl   v20.8h, v5.8h, #1         // R1<<1
    add   v17.8h, v16.8h, v17.8h    // (L0+R0)<<1 + L1 + R0
    add   v18.8h, v16.8h, v18.8h    // (L0+R0)<<1 + L0 + R1
    shl   v22.8h, v3.8h, #3         // L0<<3
    shl   v23.8h, v4.8h, #3         // R0<<3
    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1
    add   v17.8h, v17.8h, v22.8h    // (L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3
    add   v18.8h, v18.8h, v23.8h    // (L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3
    urshr v28.8h, v17.8h, #4        // ((L0+R0)<<1 + L1 + R0 + L1<<1 + L0<<3)>>4
    urshr v29.8h, v18.8h, #4        // ((L0+R0)<<1 + L0 + R1 + R1<<1 + R0<<3)>>4
    movi  v17.8h, #2
    cmeq  v17.8h, v21.8h, v17.8h    // fs == 2
    bit   v30.16b, v28.16b, v17.16b
    bit   v31.16b, v29.16b, v17.16b

    // fs == 3; update src[0] src[-1]
    shl   v16.8h, v16.8h, #1        // (L0+R0)<<2 --> V16
    shl   v17.8h, v2.8h, #2         // L1<<2
    shl   v18.8h, v5.8h, #2         // R1<<2
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h      // L1 + R2

    add   v17.8h, v17.8h, v26.8h    // L0<<1 + L1<<2
    add   v18.8h, v18.8h, v27.8h    // R0<<1 + R1<<2
    add   v19.8h, v16.8h, v19.8h    // (L0+R0)<<2 + L2+R1
    add   v20.8h, v16.8h, v20.8h    // (L0+R0)<<2 + L1+R2

    add   v17.8h, v17.8h, v19.8h    // (L0+R0)<<2 + L2+R1 + L0<<1+L1<<2
    add   v18.8h, v18.8h, v20.8h    // (L0+R0)<<2 + L1+R2 + R0<<1+R1<<2

    movi  v23.8h, #3
    urshr v17.8h, v17.8h, #4        // ((L0+R0)<<2 + (L2 + R1) + L0<<1+L1<<2) >> 4
    urshr v18.8h, v18.8h, #4        // ((L0+R0)<<2 + (L1 + R2) + R0<<1+R1<<2) >> 4

    cmeq  v25.8h, v21.8h, v23.8h    // fs == 3 (saved)
    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // fs == 3; calculate src[-2] src[1]
    add   v17.8h, v1.8h, v4.8h      // L2+R0
    add   v18.8h, v6.8h, v3.8h      // R2+L0
    shl   v19.8h, v1.8h, #1         // L2<<1
    shl   v20.8h, v6.8h, #1         // R2<<1
    shl   v22.8h, v2.8h, #3         // L1<<3
    shl   v23.8h, v5.8h, #3         // R1<<3

    add   v17.8h, v17.8h, v19.8h    // L2+R0+L2<<1
    add   v18.8h, v18.8h, v20.8h    // R2+L0+R2<<1

    shl   v19.8h, v3.8h, #2         // L0<<2
    shl   v20.8h, v4.8h, #2         // R0<<2
    add   v17.8h, v17.8h, v22.8h    // (L2+R0+L2<<1) + (L1<<3)
    add   v18.8h, v18.8h, v23.8h    // (R2+L0+R2<<1) + (R1<<3)

    add   v17.8h, v17.8h, v19.8h    // ((L2+R0+L2<<1) + (L1<<3)) + L0<<2
    add   v18.8h, v18.8h, v20.8h    //

    urshr v28.8h, v17.8h, #4
    urshr v29.8h, v18.8h, #4
    bif   v28.16b, v2.16b, v25.16b
    bif   v29.16b, v5.16b, v25.16b  // update L1_dst and R1_dst

    // fs == 4
    movi v17.8h, #4
    cmeq v25.8h, v21.8h, v17.8h
    mov  v19.d[0], v25.d[1]
    addp v24.2s, v25.2s, v19.2s
    mov  w6, v24.s[0]
    mov  w7, v24.s[1]
    orr  w6, w6, w7
    cmp  w6, #0
    beq  deblock_ver_filtered

    // if fs == 4 exist
    // calculate L0 and R0
    shl   v16.8h, v16.8h, #1        // (R0+L0)<<3 (saved)
    shl   v17.8h, v2.8h, #3         // L1<<3
    shl   v18.8h, v5.8h, #3         //
    add   v19.8h, v1.8h, v5.8h      // L2 + R1
    add   v20.8h, v2.8h, v6.8h
    add   v17.8h, v17.8h, v26.8h    // L1*8 + L0*2
    add   v18.8h, v18.8h, v27.8h
    shl   v22.8h, v19.8h, #1        // (L2 + R1)*2
    shl   v23.8h, v20.8h, #1
    add   v17.8h, v17.8h, v16.8h    // (L1*8 + L0*2) + ((R0+L0)*8)
    add   v18.8h, v18.8h, v16.8h
    add   v19.8h, v19.8h, v22.8h    // (L2 + R1)*3
    add   v20.8h, v20.8h, v23.8h

    add   v17.8h, v17.8h, v19.8h
    add   v18.8h, v18.8h, v20.8h
    urshr v17.8h, v17.8h, #5        // ((L1*8 + L0*2) + ((R0+L0)*8) + (L2 + R1)*3)>>5
    urshr v18.8h, v18.8h, #5

    bit   v30.16b, v17.16b, v25.16b // update L0
    bit   v31.16b, v18.16b, v25.16b

    // calculate L1 and R1
    add   v21.8h, v1.8h, v2.8h      // L2 + L1
    add   v22.8h, v6.8h, v5.8h
    add   v19.8h, v2.8h, v4.8h      // L1 + R0
    add   v20.8h, v5.8h, v3.8h
    add   v17.8h, v21.8h, v3.8h     // (L2+L1) + L0
    add   v18.8h, v22.8h, v4.8h
    add   v19.8h, v19.8h, v27.8h    // (L1+R0) + R0*2
    add   v20.8h, v20.8h, v26.8h
    shl   v17.8h, v17.8h, #2        // (L0 + L1 + L2)*4
    shl   v18.8h, v18.8h, #2
    add   v17.8h, v17.8h, v19.8h    // L1+R0 + R0*2 + (L0 + L1 + L2)*4
    add   v18.8h, v18.8h, v20.8h

    srshr v17.8h, v17.8h, #4        //
    srshr v18.8h, v18.8h, #4
    bit   v28.16b, v17.16b, v25.16b //
    bit   v29.16b, v18.16b, v25.16b

    // calculate L2 and R2
    add   v16.8h, v3.8h, v4.8h      // L0 + R0
    add   v17.8h, v21.8h, v0.8h     // L1 + L2 + L3
    add   v18.8h, v22.8h, v7.8h
    shl   v17.8h, v17.8h, #1        // (L1 + L2 + L3)*2
    shl   v18.8h, v18.8h, #1
    add   v17.8h, v17.8h, v16.8h
    add   v18.8h, v18.8h, v16.8h

    urshr v17.8h, v17.8h, #3        // ((L1 + L2 + L3)*2 + L0 + R0 + 4)>>3
    urshr v18.8h, v18.8h, #3        //

deblock_ver_filtered:
    bif   v17.16b, v1.16b, v25.16b
    bif   v18.16b, v6.16b, v25.16b

    mov v1.16b, v17.16b
    mov v2.16b, v28.16b
    mov v3.16b, v30.16b
    mov v4.16b, v31.16b
    mov v5.16b, v29.16b
    mov v6.16b, v18.16b

    st4 {v0.H, v1.H, v2.H, v3.H}[0], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[0], [x2], x5
    st4 {v0.H, v1.H, v2.H, v3.H}[1], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[1], [x2], x5
    st4 {v0.H, v1.H, v2.H, v3.H}[2], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[2], [x2], x5
    st4 {v0.H, v1.H, v2.H, v3.H}[3], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[3], [x2], x5
    st4 {v0.H, v1.H, v2.H, v3.H}[4], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[4], [x2], x5
    st4 {v0.H, v1.H, v2.H, v3.H}[5], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[5], [x2], x5
    st4 {v0.H, v1.H, v2.H, v3.H}[6], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[6], [x2], x5
    st4 {v0.H, v1.H, v2.H, v3.H}[7], [x2], #8
    st4 {v4.H, v5.H, v6.H, v7.H}[7], [x2]

    ret


/*****************************************************************************************************
 *  void uavs3e_deblock_ver_chroma_arm64(pel *srcu, pel *srcv, int stride, int alpha_u, int beta_u, int alpha_v, int beta_v, int flt_flag);
 *  SrcPtrU->x0, SrcPtrU->x1, stride->x2, alpha_u->x3, beta_u->x4, alpha_v->x5, beta_v->x6, flt_flag->x7
 ******************************************************************************************************/
function uavs3e_deblock_ver_chroma_arm64

    and x2, x2, #0xffffffff

    add w8, w3, w5, lsl #16     //alpha_uv交织
    add w9, w4, w6, lsl #16     //beta_uv交织

    lsl x2, x2, #1

    dup v25.4s, w8                  // v25.8h: alphau alphav alphau alphav alphau alphav alphau alphav
    dup v24.4s, w9                  // v24.8h: betau, betav

    lsr w9, w7, #1
    lsr w8, w7, #9
    and w9, w9, #1
    and w10, w8, #1
    neg w9, w9                        // flag0
    neg w10, w10                      // flag1

    dup v26.2s, w9
    dup v27.2s, w10                  // v26: flag0 flag0 flag1 flag1
    mov v26.d[1], v27.d[0]

    sub x0, x0, #6                  // srcu -= 3*sizeof(pel)
    sub x1, x1, #6                  // srcv -= 3*sizeof(pel)
    sub x5, x2, #8
    mov x3, x0
    mov x4, x1

    //src[-3]:src[2] --> l2, l1, l0, r0, r1, r2
    ld4 {v0.h, v1.h, v2.h, v3.h}[0], [x0], #8
    ld2 {v4.h, v5.h}[0], [x0], x5
    ld4 {v0.h, v1.h, v2.h, v3.h}[1], [x1], #8
    ld2 {v4.h, v5.h}[1], [x1], x5
    ld4 {v0.h, v1.h, v2.h, v3.h}[2], [x0], #8
    ld2 {v4.h, v5.h}[2], [x0], x5
    ld4 {v0.h, v1.h, v2.h, v3.h}[3], [x1], #8
    ld2 {v4.h, v5.h}[3], [x1], x5
    ld4 {v0.h, v1.h, v2.h, v3.h}[4], [x0], #8
    ld2 {v4.h, v5.h}[4], [x0], x5
    ld4 {v0.h, v1.h, v2.h, v3.h}[5], [x1], #8
    ld2 {v4.h, v5.h}[5], [x1], x5
    ld4 {v0.h, v1.h, v2.h, v3.h}[6], [x0], #8
    ld2 {v4.h, v5.h}[6], [x0], x5
    ld4 {v0.h, v1.h, v2.h, v3.h}[7], [x1], #8
    ld2 {v4.h, v5.h}[7], [x1]

    // v5.8h : R2
    // v4.8h : R1
    // v3.8h : R0
    // v2.8h : L0
    // v1.8h : L1
    // v0.8h : L2

    uabd v16.8h, v2.8h , v1.8h      // abs(L0-L1)
    uabd v17.8h, v3.8h , v4.8h      // abs(R0-R1)
    uabd v29.8h, v3.8h , v2.8h      // abs(R0-L0)
    uabd v30.8h, v0.8h , v2.8h      // abs(L2-L0)
    uabd v31.8h, v3.8h , v5.8h      // abs(R0-R2)

    cmhi v18.8h, v24.8h, v16.8h     // abs(L0-L1) < beta
    cmhi v19.8h, v24.8h, v17.8h     // abs(R0-R1) < beta
    ushr v28.8h, v24.8h, #2         // beta/4
    cmhi v29.8h, v25.8h, v29.8h     // abs(R0-L0) < alpha
    cmhi v30.8h, v24.8h, v30.8h     // abs(L2-L0) < beta
    cmhi v31.8h, v24.8h, v31.8h     // abs(R2-R0) < beta
    cmge v16.8h, v28.8h, v16.8h     // abs(L0-L1) <= beta/4
    cmge v17.8h, v28.8h, v17.8h     // abs(R0-R1) <= beta/4

    and  v18.16b, v18.16b, v19.16b  // abs(L0-L1) < beta && abs(R0-R1) < beta
    and  v30.16b, v30.16b, v31.16b  // abs(L2-L0) < beta && abs(R2-R0) < beta
    and  v16.16b, v16.16b, v17.16b  // abs(L0-L1) <= beta/4 && abs(R0-R1) <= beta/4

    and  v31.16b, v18.16b, v26.16b  // flt0: mask L0, R0
    and  v30.16b, v30.16b, v29.16b  //
    and  v16.16b, v31.16b, v16.16b  //
    and  v30.16b, v30.16b, v16.16b  // flt1: mask L1, R1

    // filter L0, R0
    shl  v24.8h, v3.8h, #1          // R0 * 2
    shl  v25.8h, v2.8h, #1
    add  v18.8h, v2.8h, v0.8h       // L0 + L2
    add  v19.8h, v3.8h, v5.8h
    shl  v20.8h, v1.8h, #3          // L1 * 8
    shl  v21.8h, v4.8h, #3
    add  v16.8h, v24.8h, v20.8h     // (R0 * 2) + (L1 * 8)
    add  v17.8h, v25.8h, v21.8h
    shl  v22.8h, v18.8h, #1         // (L0 + L2)*2
    shl  v23.8h, v19.8h, #1
    add  v16.8h, v16.8h, v18.8h     // (L0 + L2) + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v19.8h
    add  v16.8h, v16.8h, v22.8h     // (L0 + L2)*3 + (R0 * 2) + (L1 * 8)
    add  v17.8h, v17.8h, v23.8h

    urshr v16.8h, v16.8h, #4
    urshr v17.8h, v17.8h, #4

    add  v18.8h, v1.8h, v3.8h       // L1 + R0
    add  v19.8h, v4.8h, v2.8h
    shl  v20.8h, v2.8h, #3          // L0 * 8
    shl  v21.8h, v3.8h, #3
    shl  v22.8h, v18.8h, #1         // (L1 + R0)*2
    shl  v23.8h, v19.8h, #1
    add  v20.8h, v20.8h, v25.8h     // L0 * 10
    add  v21.8h, v21.8h, v24.8h
    add  v18.8h, v18.8h, v22.8h     // (L1 + R0)*3
    add  v19.8h, v19.8h, v23.8h
    add  v18.8h, v18.8h, v20.8h     // (L1 + R0)*3 + L0*10
    add  v19.8h, v19.8h, v21.8h

    urshr v18.8h, v18.8h, #4
    urshr v19.8h, v19.8h, #4

    bit  v1.16b, v16.16b, v30.16b   // L1
    bit  v2.16b, v18.16b, v31.16b   // L0
    bit  v3.16b, v19.16b, v31.16b   // R0
    bit  v4.16b, v17.16b, v30.16b   // R1

    //store L1, L0, R0, R1

    st4 {v0.h, v1.h, v2.h, v3.h}[0], [x3], #8
    st2 {v4.h, v5.h}[0], [x3], x5
    st4 {v0.h, v1.h, v2.h, v3.h}[1], [x4], #8
    st2 {v4.h, v5.h}[1], [x4], x5
    st4 {v0.h, v1.h, v2.h, v3.h}[2], [x3], #8
    st2 {v4.h, v5.h}[2], [x3], x5
    st4 {v0.h, v1.h, v2.h, v3.h}[3], [x4], #8
    st2 {v4.h, v5.h}[3], [x4], x5
    st4 {v0.h, v1.h, v2.h, v3.h}[4], [x3], #8
    st2 {v4.h, v5.h}[4], [x3], x5
    st4 {v0.h, v1.h, v2.h, v3.h}[5], [x4], #8
    st2 {v4.h, v5.h}[5], [x4], x5
    st4 {v0.h, v1.h, v2.h, v3.h}[6], [x3], #8
    st2 {v4.h, v5.h}[6], [x3], x5
    st4 {v0.h, v1.h, v2.h, v3.h}[7], [x4], #8
    st2 {v4.h, v5.h}[7], [x4]

    ret

#endif  // COMPILE_10BIT

#endif
