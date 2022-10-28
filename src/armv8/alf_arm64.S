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

#define SIMPLIFIED_ALF_ARM64 1

#if SIMPLIFIED_ALF_ARM64
/********************************************************************************************************************************************
 *  void uavs3e_alf_filter_block_arm64(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
 *  dst->x0, i_dst->x1, src->x2, i_src->x3, lcu_width->x4, lcu_height->x5, coef->x6
 ********************************************************************************************************************************************/
function uavs3e_alf_filter_block_arm64

    //x19-x28 are callee-saved registers
    stp x19, x20, [sp, #-16]
    stp x21, x22, [sp, #-32]
    stp x23, x24, [sp, #-48]
    sub sp, sp, #48

    ld1 {v2.4s, v3.4s}, [x6]        // load coef[0-7]
    xtn v0.4h, v2.4s
    xtn v1.4h, v3.4s
    add x6, x6, #32
    ld1 {v4.2s}, [x6]               // load coef[8]
    xtn v2.4h, v4.4s

    mov w8, #0                      // w8 : i = startPos
    sub w15, w5, #1                 // w15: lcu_height - 1
    sub w19, w5, #3                 // lcu_height - 3

alf_arm64_loop_y:
    sub x9 , x2, x3                 // imgPad2 = src - i_src;
    add x10, x2, x3                 // imgPad1 = src + i_src;
    sub x11, x2, x3, lsl #1         // imgPad4 = src - 2*i_src;
    add x12, x2, x3, lsl #1         // imgPad3 = src + 2*i_src;
    sub x13, x11, x3                // imgPad6 = src - 3*i_src;
    add x14, x12, x3                // imgPad5 = src + 3*i_src;

    cmp w8, #3
    bge alf_arm64_y_ge_3
    cmp w8, #1
    beq alf_arm64_y_eq_1
    bgt alf_arm64_y_eq_2
    mov x9, x2                      // i == 0
alf_arm64_y_eq_1:
    mov x11, x9                     // i == 1
alf_arm64_y_eq_2:
    mov x13, x11                    // i == 2

    b alf_arm64_y_lt_h_minus_3

alf_arm64_y_ge_3:
    cmp w8, w19
    blt alf_arm64_y_lt_h_minus_3
    beq alf_arm64_y_eq_h_minus_3
    cmp w8, w15
    blt alf_arm64_y_eq_h_minus_2
    mov x10, x2                     // i == lcu_height - 1
alf_arm64_y_eq_h_minus_2:
    mov x12, x10                    // i == lcu_height - 2
alf_arm64_y_eq_h_minus_3:
    mov x14, x12                    // i == lcu_height - 3

alf_arm64_y_lt_h_minus_3:

    mov x20, #0                     // j = 0
alf_arm64_loop_x:
    add x21, x13, x20
    add x22, x14, x20
    add x23, x11, x20
    add x24, x12, x20

    ld1 {v3.16b}, [x21]
    ld1 {v4.16b}, [x22]
    ld1 {v5.16b}, [x23]
    ld1 {v6.16b}, [x24]

    add x21, x9 , x20
    add x22, x10, x20
    sub x23, x21, #1
    sub x24, x22, #1

    uaddl  v18.8h, v3.8b , v4.8b
    uaddl2 v19.8h, v3.16b, v4.16b
    uaddl  v20.8h, v5.8b , v6.8b
    uaddl2 v21.8h, v5.16b, v6.16b

    ld1 {v3.16b, v4.16b}, [x23]         // load imgPad2[j-1]
    ld1 {v5.16b, v6.16b}, [x24]         // load imgPad1[j-1]

    mul   v16.8h, v18.8h, v0.h[0]       // pixelInt  = coef[0] * (imgPad5[j] + imgPad6[j]);
    mul   v17.8h, v19.8h, v0.h[0]
    mla   v16.8h, v20.8h, v0.h[1]       // pixelInt += coef[1] * (imgPad3[j] + imgPad4[j]);
    mla   v17.8h, v21.8h, v0.h[1]

    ext v18.16b, v3.16b, v4.16b, #2     // imgPad2[j+1]
    ext v19.16b, v5.16b, v6.16b, #2     // imgPad1[j+1]
    ext v4.16b, v3.16b, v4.16b, #1      // imgPad2[j]
    ext v6.16b, v5.16b, v6.16b, #1      // imgPad1[j]

    add x21, x2, x20
    sub x22, x21, #3

    uaddl  v20.8h, v19.8b , v3.8b
    uaddl2 v21.8h, v19.16b, v3.16b
    uaddl  v22.8h, v4.8b , v6.8b
    uaddl2 v23.8h, v4.16b, v6.16b
    uaddl  v24.8h, v5.8b , v18.8b
    uaddl2 v25.8h, v5.16b, v18.16b

    ld1 {v3.16b, v4.16b}, [x22]         // load imgPad[j-3]

    mla v16.8h, v20.8h, v0.h[2]         // pixelInt += coef[2] * (imgPad1[j + 1] + imgPad2[j - 1])
    mla v17.8h, v21.8h, v0.h[2]
    mla v16.8h, v22.8h, v0.h[3]         // pixelInt += coef[3] * (imgPad1[j] + imgPad2[j])
    mla v17.8h, v23.8h, v0.h[3]
    mla v16.8h, v24.8h, v1.h[0]         // pixelInt += coef[4] * (imgPad1[j - 1] + imgPad2[j + 1])
    mla v17.8h, v25.8h, v1.h[0]

    ext v5.16b, v3.16b, v4.16b, #1      // imgPad[j-2]
    ext v6.16b, v3.16b, v4.16b, #2      // imgPad[j-1]
    ext v7.16b, v3.16b, v4.16b, #3      // imgPad[j]
    ext v18.16b, v3.16b, v4.16b, #4     // imgPad[j+1]
    ext v19.16b, v3.16b, v4.16b, #5     // imgPad[j+2]
    ext v20.16b, v3.16b, v4.16b, #6     // imgPad[j+3]

    uaddl  v22.8h, v20.8b , v3.8b
    uaddl2 v23.8h, v20.16b, v3.16b
    uaddl  v24.8h, v19.8b , v5.8b
    uaddl2 v25.8h, v19.16b, v5.16b

    uaddl  v26.8h, v18.8b , v6.8b
    uaddl2 v27.8h, v18.16b, v6.16b

    uxtl   v28.8h, v7.8b
    uxtl2  v29.8h, v7.16b

    mla   v16.8h, v22.8h, v1.h[1]       // pixelInt += coef[5] * (imgPad[j + 3] + imgPad[j - 3])
    mla   v17.8h, v23.8h, v1.h[1]
    mla   v16.8h, v24.8h, v1.h[2]       // pixelInt += coef[6] * (imgPad[j + 2] + imgPad[j - 2])
    mla   v17.8h, v25.8h, v1.h[2]
    mla   v16.8h, v26.8h, v1.h[3]       // pixelInt += coef[7] * (imgPad[j + 1] + imgPad[j - 1])
    mla   v17.8h, v27.8h, v1.h[3]
    mla   v16.8h, v28.8h, v2.h[0]       // pixelInt += coef[8] * (imgPad[j])
    mla   v17.8h, v29.8h, v2.h[0]

    add   x21, x0, x20
    sqrshrun v16.8b,  v16.8h,  #6
    sqrshrun v17.8b,  v17.8h,  #6

    add   x20, x20, #16
    st1   {v16.8b, v17.8b}, [x21]       // store imgRes[j]

    cmp   x20, x4
    blt   alf_arm64_loop_x

    add   w8, w8, #1
    add   x0, x0, x1
    add   x2, x2, x3
    cmp   w8, w5
    blt   alf_arm64_loop_y

    add sp, sp, #48
    ldp x19, x20, [sp, #-16]
    ldp x21, x22, [sp, #-32]
    ldp x23, x24, [sp, #-48]

ret

#else // SIMPLIFIED_ALF_ARM64 == 0

/********************************************************************************************************************************************
 *  void uavs3e_alf_filter_block_arm64(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth);
 *  dst->x0, i_dst->x1, src->x2, i_src->x3, lcu_width->x4, lcu_height->x5, coef->x6
 ********************************************************************************************************************************************/
function uavs3e_alf_filter_block_arm64

    //x19-x28 are callee-saved registers
    stp x19, x20, [sp, #-16]
    stp x21, x22, [sp, #-32]
    stp x23, x24, [sp, #-48]
    sub sp, sp, #48

    ld1 {v2.4s, v3.4s}, [x6]        // load coef[0-7]
    xtn v0.4h, v2.4s
    xtn v1.4h, v3.4s
    add x6, x6, #32
    ld1 {v4.2s}, [x6]               // load coef[8]
    xtn v2.4h, v4.4s

    mov w8, #0                      // w8 : i = startPos
    sub w15, w5, #1                 // w15: lcu_height - 1
    sub w19, w5, #3                 // lcu_height - 3

alf_arm64_loop_y:
    sub x9 , x2, x3                 // imgPad2 = src - i_src;
    add x10, x2, x3                 // imgPad1 = src + i_src;
    sub x11, x2, x3, lsl #1         // imgPad4 = src - 2*i_src;
    add x12, x2, x3, lsl #1         // imgPad3 = src + 2*i_src;
    sub x13, x11, x3                // imgPad6 = src - 3*i_src;
    add x14, x12, x3                // imgPad5 = src + 3*i_src;

    cmp w8, #3
    bge alf_arm64_y_ge_3
    cmp w8, #1
    beq alf_arm64_y_eq_1
    bgt alf_arm64_y_eq_2
    mov x9, x2                      // i == 0
alf_arm64_y_eq_1:
    mov x11, x9                     // i == 1
alf_arm64_y_eq_2:
    mov x13, x11                    // i == 2

    b alf_arm64_y_lt_h_minus_3

alf_arm64_y_ge_3:
    cmp w8, w19
    blt alf_arm64_y_lt_h_minus_3
    beq alf_arm64_y_eq_h_minus_3
    cmp w8, w15
    blt alf_arm64_y_eq_h_minus_2
    mov x10, x2                     // i == lcu_height - 1
alf_arm64_y_eq_h_minus_2:
    mov x12, x10                    // i == lcu_height - 2
alf_arm64_y_eq_h_minus_3:
    mov x14, x12                    // i == lcu_height - 3

alf_arm64_y_lt_h_minus_3:

    mov x20, #0                     // j = 0
alf_arm64_loop_x:
    add x21, x13, x20
    add x22, x14, x20
    add x23, x11, x20
    add x24, x12, x20

    ld1 {v3.8b}, [x21]
    ld1 {v4.8b}, [x22]
    ld1 {v5.8b}, [x23]
    ld1 {v6.8b}, [x24]

    add x21, x9 , x20
    add x22, x10, x20
    sub x21, x21, #1
    sub x22, x22, #1

    uaddl  v3.8h, v3.8b, v4.8b
    uaddl  v5.8h, v5.8b, v6.8b
    smull  v16.4s, v3.4h, v0.h[0]   // pixelInt  = coef[0] * (imgPad5[j] + imgPad6[j]);
    smull2 v17.4s, v3.8h, v0.h[0]
    smlal  v16.4s, v5.4h, v0.h[1]   // pixelInt += coef[1] * (imgPad3[j] + imgPad4[j]);
    smlal2 v17.4s, v5.8h, v0.h[1]

    ld1 {v3.8b, v4.8b}, [x21]       // load imgPad2[j-1]
    ld1 {v5.8b, v6.8b}, [x22]       // load imgPad1[j-1]

    ext v18.8b, v3.8b, v4.8b, #2    // imgPad2[j+1]
    ext v19.8b, v5.8b, v6.8b, #2    // imgPad1[j+1]
    ext v4.8b, v3.8b, v4.8b, #1     // imgPad2[j]
    ext v6.8b, v5.8b, v6.8b, #1     // imgPad1[j]

    uaddl v20.8h, v19.8b, v3.8b
    uaddl v21.8h, v4.8b, v6.8b
    uaddl v22.8h, v5.8b, v18.8b

    add x21, x2, x20
    sub x22, x21, #3

    smlal  v16.4s, v20.4h, v0.h[2]  // pixelInt += coef[2] * (imgPad1[j + 1] + imgPad2[j - 1])
    smlal2 v17.4s, v20.8h, v0.h[2]
    smlal  v16.4s, v21.4h, v0.h[3]  // pixelInt += coef[3] * (imgPad1[j] + imgPad2[j])
    smlal2 v17.4s, v21.8h, v0.h[3]
    smlal  v16.4s, v22.4h, v1.h[0]  // pixelInt += coef[4] * (imgPad1[j - 1] + imgPad2[j + 1])
    smlal2 v17.4s, v22.8h, v1.h[0]

    ld1 {v3.8b, v4.8b}, [x22]       // load imgPad[j-3]

    ext v5.8b, v3.8b, v4.8b, #1     // imgPad[j-2]
    ext v6.8b, v3.8b, v4.8b, #2     // imgPad[j-1]
    ext v7.8b, v3.8b, v4.8b, #3     // imgPad[j]
    ext v22.8b, v3.8b, v4.8b, #4    // imgPad[j+1]
    ext v18.8b, v3.8b, v4.8b, #5    // imgPad[j+2]
    ext v19.8b, v3.8b, v4.8b, #6    // imgPad[j+3]

    uaddl v20.8h, v19.8b, v3.8b
    uaddl v21.8h, v18.8b, v5.8b

    smlal  v16.4s, v20.4h, v1.h[1]  // pixelInt += coef[5] * (imgPad[j + 3] + imgPad[j - 3])
    smlal2 v17.4s, v20.8h, v1.h[1]
    smlal  v16.4s, v21.4h, v1.h[2]  // pixelInt += coef[6] * (imgPad[j + 2] + imgPad[j - 2])
    smlal2 v17.4s, v21.8h, v1.h[2]

    uaddl v20.8h, v22.8b, v6.8b
    uxtl  v21.8h, v7.8b
    smlal v16.4s, v20.4h, v1.h[3]   // pixelInt += coef[7] * (imgPad[j + 1] + imgPad[j - 1])
    smlal2 v17.4s, v20.8h, v1.h[3]
    smlal v16.4s, v21.4h, v2.h[0]   // pixelInt += coef[8] * (imgPad[j])
    smlal2 v17.4s, v21.8h, v2.h[0]

    add   x21, x0, x20
    rshrn  v16.4h, v16.4s, #6
    rshrn2 v16.8h, v17.4s, #6
    sqxtun v16.8b, v16.8h

    add   x20, x20, #8
    st1   {v16.8b}, [x21]           // store imgRes[j]

    cmp   x20, x4
    blt   alf_arm64_loop_x

    add   w8, w8, #1
    add   x0, x0, x1
    add   x2, x2, x3
    cmp   w8, w5
    blt   alf_arm64_loop_y

    add sp, sp, #48
    ldp x19, x20, [sp, #-16]
    ldp x21, x22, [sp, #-32]
    ldp x23, x24, [sp, #-48]

    ret

#endif  // SIMPLIFIED_ALF_ARM64

#else

/********************************************************************************************************************************************
 *  void uavs3e_alf_filter_block_arm64(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int bit_depth);
 *  dst->x0, i_dst->x1, src->x2, i_src->x3, lcu_width->x4, lcu_height->x5, coef->x6, bit_depth->x7
 ********************************************************************************************************************************************/
function uavs3e_alf_filter_block_arm64

    //x19-x28 are callee-saved registers
    stp x19, x20, [sp, #-16]
    stp x21, x22, [sp, #-32]
    stp x23, x24, [sp, #-48]
    sub sp, sp, #48

    ld1 {v2.4s, v3.4s}, [x6], #32   // load coef[0-7]

    lsl x1, x1, #1                  // i_dst *= sizeof(pel)
    lsl x3, x3, #1
    lsl x4, x4, #1                  // lcu_width *= sizeof(pel)

    ld1 {v4.2s}, [x6]               // load coef[8]

    mov w9, #1

    xtn v0.4h, v2.4s
    xtn v1.4h, v3.4s
    xtn v2.4h, v4.4s

    lsl w9, w9, w7
    sub w9, w9, #1
    dup v31.8h, w9

    mov w8, #0                      // w8 : i = startPos
    sub w15, w5, #1                 // w15: lcu_height - 1
    sub w19, w5, #3                 // lcu_height - 3

alf_arm64_loop_y:
    sub x9 , x2, x3                 // imgPad2 = src - i_src;
    add x10, x2, x3                 // imgPad1 = src + i_src;
    sub x11, x2, x3, lsl #1         // imgPad4 = src - 2*i_src;
    add x12, x2, x3, lsl #1         // imgPad3 = src + 2*i_src;
    sub x13, x11, x3                // imgPad6 = src - 3*i_src;
    add x14, x12, x3                // imgPad5 = src + 3*i_src;

    cmp w8, #3
    bge alf_arm64_y_ge_3
    cmp w8, #1
    beq alf_arm64_y_eq_1
    bgt alf_arm64_y_eq_2
    mov x9, x2                      // i == 0
alf_arm64_y_eq_1:
    mov x11, x9                     // i == 1
alf_arm64_y_eq_2:
    mov x13, x11                    // i == 2

    b alf_arm64_y_lt_h_minus_3

alf_arm64_y_ge_3:
    cmp w8, w19
    blt alf_arm64_y_lt_h_minus_3
    beq alf_arm64_y_eq_h_minus_3
    cmp w8, w15
    blt alf_arm64_y_eq_h_minus_2
    mov x10, x2                     // i == lcu_height - 1
alf_arm64_y_eq_h_minus_2:
    mov x12, x10                    // i == lcu_height - 2
alf_arm64_y_eq_h_minus_3:
    mov x14, x12                    // i == lcu_height - 3

alf_arm64_y_lt_h_minus_3:

    mov x20, #0                     // j = 0
alf_arm64_loop_x:
    add x21, x13, x20
    add x22, x14, x20
    add x23, x11, x20
    add x24, x12, x20

    ld1 {v3.8h}, [x21]
    ld1 {v4.8h}, [x22]
    ld1 {v5.8h}, [x23]
    ld1 {v6.8h}, [x24]

    add v3.8h, v3.8h, v4.8h
    add v5.8h, v5.8h, v6.8h
    smull  v16.4s, v3.4h, v0.h[0]   // pixelInt  = coef[0] * (imgPad5[j] + imgPad6[j]);
    smull2 v17.4s, v3.8h, v0.h[0]
    smlal  v16.4s, v5.4h, v0.h[1]   // pixelInt += coef[1] * (imgPad3[j] + imgPad4[j]);
    smlal2 v17.4s, v5.8h, v0.h[1]

    add x21, x9 , x20
    add x22, x10, x20
    sub x23, x21, #2
    sub x24, x22, #2
    ld1 {v5.8h}, [x21]              // load imgPad2[j]
    ld1 {v6.8h}, [x22]              // load imgPad1[j]
    ld1 {v3.8h}, [x23]              // load imgPad2[j-1]
    ld1 {v4.8h}, [x24]              // load imgPad1[j-1]

    add x24, x22, #2
    add x23, x21, #2
    ld1 {v19.8h}, [x24]             // load imgPad1[j+1]
    ld1 {v18.8h}, [x23]             // load imgPad2[j+1]

    add v20.8h, v19.8h, v3.8h
    add v21.8h, v5.8h, v6.8h
    add v22.8h, v4.8h, v18.8h

    smlal  v16.4s, v20.4h, v0.h[2]  // pixelInt += coef[2] * (imgPad1[j + 1] + imgPad2[j - 1])
    smlal2 v17.4s, v20.8h, v0.h[2]
    smlal  v16.4s, v21.4h, v0.h[3]  // pixelInt += coef[3] * (imgPad1[j] + imgPad2[j])
    smlal2 v17.4s, v21.8h, v0.h[3]
    smlal  v16.4s, v22.4h, v1.h[0]  // pixelInt += coef[4] * (imgPad1[j - 1] + imgPad2[j + 1])
    smlal2 v17.4s, v22.8h, v1.h[0]

    add x21, x2, x20
    sub x22, x21, #6
    add x23, x21, #10
    ld1 {v3.8h}, [x22]              // load imgPad[j-3]
    ld1 {v4.8h}, [x23]

    ext v5.16b, v3.16b, v4.16b, #2      // imgPad[j-2]
    ext v6.16b, v3.16b, v4.16b, #4      // imgPad[j-1]
    ext v7.16b, v3.16b, v4.16b, #6      // imgPad[j]
    ext v22.16b, v3.16b, v4.16b, #8     // imgPad[j+1]
    ext v18.16b, v3.16b, v4.16b, #10    // imgPad[j+2]
    ext v19.16b, v3.16b, v4.16b, #12    // imgPad[j+3]

    add v20.8h, v19.8h, v3.8h
    add v21.8h, v18.8h, v5.8h

    smlal  v16.4s, v20.4h, v1.h[1]      // pixelInt += coef[5] * (imgPad[j + 3] + imgPad[j - 3])
    smlal2 v17.4s, v20.8h, v1.h[1]
    smlal  v16.4s, v21.4h, v1.h[2]      // pixelInt += coef[6] * (imgPad[j + 2] + imgPad[j - 2])
    smlal2 v17.4s, v21.8h, v1.h[2]

    add    v20.8h, v22.8h, v6.8h

    smlal  v16.4s, v20.4h, v1.h[3]      // pixelInt += coef[7] * (imgPad[j + 1] + imgPad[j - 1])
    smlal2 v17.4s, v20.8h, v1.h[3]
    smlal  v16.4s, v7.4h, v2.h[0]      // pixelInt += coef[8] * (imgPad[j])
    smlal2 v17.4s, v7.8h, v2.h[0]

    add   x21, x0, x20
    sqrshrun  v16.4h, v16.4s, #6
    sqrshrun2 v16.8h, v17.4s, #6
    
    umin v16.8h, v16.8h, v31.8h

    add   x20, x20, #16
    st1   {v16.8h}, [x21]               // store imgRes[j]

    cmp   x20, x4
    blt   alf_arm64_loop_x

    add   w8, w8, #1
    add   x0, x0, x1
    add   x2, x2, x3
    cmp   w8, w5
    blt   alf_arm64_loop_y

    add sp, sp, #48
    ldp x19, x20, [sp, #-16]
    ldp x21, x22, [sp, #-32]
    ldp x23, x24, [sp, #-48]

    ret

#endif      // COMPILE_10BIT

#endif      // defined(__arm64__)
