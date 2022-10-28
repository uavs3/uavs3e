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
//void uavs3e_get_sad_4_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_4_arm64

    movi v16.16b, #0
get_sad_4_y:
    //load org
    ld1 {v0.s}[0], [x0], x1
    ld1 {v0.s}[1], [x0], x1
    ld1 {v0.s}[2], [x0], x1
    ld1 {v0.s}[3], [x0], x1
    //load pred
    ld1 {v1.s}[0], [x2], x3
    ld1 {v1.s}[1], [x2], x3
    ld1 {v1.s}[2], [x2], x3
    ld1 {v1.s}[3], [x2], x3
    uabdl v2.8h, v0.8b, v1.8b
    uabdl2 v3.8h, v0.16b, v1.16b
    add v0.8h, v2.8h, v3.8h
    add v16.8h, v16.8h, v0.8h
    subs w4, w4, #4
    bgt get_sad_4_y

    uaddlp v16.4s, v16.8h
    addp v16.4s, v16.4s, v16.4s
    addp v16.4s, v16.4s, v16.4s
    mov x0, #0
    umov w0, v16.s[0]

    ret

//void uavs3e_get_sad_8_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_8_arm64
    movi v16.16b, #0

get_sad_8_y:
    //load org
    ld1 {v0.8b}, [x0], x1
    ld1 {v1.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v3.8b}, [x0], x1
    //load pred
    ld1 {v4.8b}, [x2], x3
    ld1 {v5.8b}, [x2], x3
    ld1 {v6.8b}, [x2], x3
    ld1 {v7.8b}, [x2], x3
    uabdl v0.8h, v0.8b, v4.8b
    uabdl v1.8h, v1.8b, v5.8b
    uabdl v2.8h, v2.8b, v6.8b
    uabdl v3.8h, v3.8b, v7.8b
    add v0.8h, v0.8h, v1.8h
    add v2.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v2.8h
    add v16.8h, v16.8h, v0.8h

    subs w4, w4, #4
    bgt get_sad_8_y
    uaddlp v16.4s, v16.8h
    addp v16.4s, v16.4s, v16.4s
    addp v16.4s, v16.4s, v16.4s

    mov x0, #0
    umov w0, v16.s[0]

ret

//void uavs3e_get_sad_16_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_16_arm64
    movi v16.16b, #0
    movi v17.16b, #0
    movi v18.16b, #0
    movi v19.16b, #0

get_sad_16_y:
    ld1 {v0.8b, v1.8b}, [x0], x1
    ld1 {v2.8b, v3.8b}, [x0], x1
    //load pred
    ld1 {v4.8b, v5.8b}, [x2], x3
    ld1 {v6.8b, v7.8b}, [x2], x3

    uabdl v0.8h, v0.8b, v4.8b
    uabdl v1.8h, v1.8b, v5.8b
    uabdl v2.8h, v2.8b, v6.8b
    uabdl v3.8h, v3.8b, v7.8b
    add v16.8h, v16.8h, v0.8h
    add v17.8h, v17.8h, v1.8h
    add v18.8h, v18.8h, v2.8h
    add v19.8h, v19.8h, v3.8h

    subs w4, w4, #2
    bgt get_sad_16_y

    uaddlp v16.4s, v16.8h
    uaddlp v17.4s, v17.8h
    uaddlp v18.4s, v18.8h
    uaddlp v19.4s, v19.8h
    addp v16.4s, v16.4s, v17.4s
    addp v18.4s, v18.4s, v19.4s
    addp v16.4s, v16.4s, v18.4s
    addp v16.4s, v16.4s, v16.4s
    addp v16.4s, v16.4s, v16.4s

    mov x0, #0
    umov w0, v16.s[0]

    ret

//void uavs3e_get_sad_32_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_32_arm64
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0
    movi v28.16b, #0
    movi v29.16b, #0
    movi v30.16b, #0
    movi v31.16b, #0

get_sad_32_y:
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x0], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x2], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x2], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b
    add v24.8h, v24.8h, v0.8h
    add v25.8h, v25.8h, v1.8h
    add v26.8h, v26.8h, v2.8h
    add v27.8h, v27.8h, v3.8h
    add v28.8h, v28.8h, v4.8h
    add v29.8h, v29.8h, v5.8h
    add v30.8h, v30.8h, v6.8h
    add v31.8h, v31.8h, v7.8h

    subs w4, w4, #2
    bgt get_sad_32_y

    uaddlp v24.4s, v24.8h
    uaddlp v25.4s, v25.8h
    uaddlp v26.4s, v26.8h
    uaddlp v27.4s, v27.8h
    uaddlp v28.4s, v28.8h
    uaddlp v29.4s, v29.8h
    uaddlp v30.4s, v30.8h
    uaddlp v31.4s, v31.8h
    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v27.4s
    addp v28.4s, v28.4s, v29.4s
    addp v30.4s, v30.4s, v31.4s
    addp v24.4s, v24.4s, v26.4s
    addp v28.4s, v28.4s, v30.4s
    addp v24.4s, v24.4s, v28.4s
    addp v24.4s, v24.4s, v24.4s
    addp v24.4s, v24.4s, v24.4s

    mov x0, #0
    umov w0, v24.s[0]

    ret

//void uavs3e_get_sad_64_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_64_arm64
    sub sp, sp, #64
    st1 {v8.8h - v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h - v15.8h}, [sp]
    movi v24.16b, #0

get_sad_64_y:

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x0], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x2], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x2], x3

    uabdl v8.8h, v0.8b, v16.8b
    uabdl v9.8h, v1.8b, v17.8b
    uabdl v10.8h, v2.8b, v18.8b
    uabdl v11.8h, v3.8b, v19.8b
    uabdl v12.8h, v4.8b, v20.8b
    uabdl v13.8h, v5.8b, v21.8b
    uabdl v14.8h, v6.8b, v22.8b
    uabdl v15.8h, v7.8b, v23.8b

    uabdl2 v0.8h, v0.16b, v16.16b
    uabdl2 v1.8h, v1.16b, v17.16b
    uabdl2 v2.8h, v2.16b, v18.16b
    uabdl2 v3.8h, v3.16b, v19.16b
    uabdl2 v4.8h, v4.16b, v20.16b
    uabdl2 v5.8h, v5.16b, v21.16b
    uabdl2 v6.8h, v6.16b, v22.16b
    uabdl2 v7.8h, v7.16b, v23.16b

    add v0.8h, v0.8h, v8.8h
    add v1.8h, v1.8h, v9.8h
    add v2.8h, v2.8h, v10.8h
    add v3.8h, v3.8h, v11.8h
    add v4.8h, v4.8h, v12.8h
    add v5.8h, v5.8h, v13.8h
    add v6.8h, v6.8h, v14.8h
    add v7.8h, v7.8h, v15.8h

    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v24.4s, v24.4s, v0.4s

    subs w4, w4, #2
    bgt get_sad_64_y
    addp v24.4s, v24.4s, v24.4s
    addp v24.4s, v24.4s, v24.4s

    mov x0, #0
    umov w0, v24.s[0]

    ld1 {v12.8h - v15.8h}, [sp], #64
    ld1 {v8.8h - v11.8h}, [sp], #64

    ret

//void uavs3e_get_sad_128_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_128_arm64
    sub sp, sp, #64
    st1 {v8.8h - v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h - v15.8h}, [sp]
    lsl x7, x1, #1      //2 * i_org
    lsl x8, x3, #1      //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0

get_sad_128_y:
    mov x5, x0
    mov x6, x2

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabdl v8.8h, v0.8b, v16.8b
    uabdl v9.8h, v1.8b, v17.8b
    uabdl v10.8h, v2.8b, v18.8b
    uabdl v11.8h, v3.8b, v19.8b
    uabdl v12.8h, v4.8b, v20.8b
    uabdl v13.8h, v5.8b, v21.8b
    uabdl v14.8h, v6.8b, v22.8b
    uabdl v15.8h, v7.8b, v23.8b
    
    uabdl2 v0.8h, v0.16b, v16.16b
    uabdl2 v1.8h, v1.16b, v17.16b
    uabdl2 v2.8h, v2.16b, v18.16b
    uabdl2 v3.8h, v3.16b, v19.16b
    uabdl2 v4.8h, v4.16b, v20.16b
    uabdl2 v5.8h, v5.16b, v21.16b
    uabdl2 v6.8h, v6.16b, v22.16b
    uabdl2 v7.8h, v7.16b, v23.16b
    
    add v0.8h, v0.8h, v8.8h
    add v1.8h, v1.8h, v9.8h
    add v2.8h, v2.8h, v10.8h
    add v3.8h, v3.8h, v11.8h
    add v4.8h, v4.8h, v12.8h
    add v5.8h, v5.8h, v13.8h
    add v6.8h, v6.8h, v14.8h
    add v7.8h, v7.8h, v15.8h
    
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v24.4s, v24.4s, v0.4s

    add x5, x0, #64
    add x6, x2, #64

    //load p_org + 64
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabdl v8.8h, v0.8b, v16.8b
    uabdl v9.8h, v1.8b, v17.8b
    uabdl v10.8h, v2.8b, v18.8b
    uabdl v11.8h, v3.8b, v19.8b
    uabdl v12.8h, v4.8b, v20.8b
    uabdl v13.8h, v5.8b, v21.8b
    uabdl v14.8h, v6.8b, v22.8b
    uabdl v15.8h, v7.8b, v23.8b
    
    uabdl2 v0.8h, v0.16b, v16.16b
    uabdl2 v1.8h, v1.16b, v17.16b
    uabdl2 v2.8h, v2.16b, v18.16b
    uabdl2 v3.8h, v3.16b, v19.16b
    uabdl2 v4.8h, v4.16b, v20.16b
    uabdl2 v5.8h, v5.16b, v21.16b
    uabdl2 v6.8h, v6.16b, v22.16b
    uabdl2 v7.8h, v7.16b, v23.16b
    
    add v0.8h, v0.8h, v8.8h
    add v1.8h, v1.8h, v9.8h
    add v2.8h, v2.8h, v10.8h
    add v3.8h, v3.8h, v11.8h
    add v4.8h, v4.8h, v12.8h
    add v5.8h, v5.8h, v13.8h
    add v6.8h, v6.8h, v14.8h
    add v7.8h, v7.8h, v15.8h
    
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v25.4s, v25.4s, v0.4s

    add x0, x0, x7
    add x2, x2, x8

    subs w4, w4, #2
    bgt get_sad_128_y

    add v24.4s, v24.4s, v25.4s
    addp v24.4s, v24.4s, v24.4s
    addp v24.4s, v24.4s, v24.4s

    mov x0, #0
    umov w0, v24.s[0]

    ld1 {v12.8h - v15.8h}, [sp], #64
    ld1 {v8.8h - v11.8h}, [sp], #64

    ret

//void uavs3e_get_ssd_4_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_4_arm64
    movi v16.16b, #0

get_ssd_4_y:
    //load p_org
    ld1 {v0.s}[0], [x0], x1
    ld1 {v1.s}[0], [x0], x1
    ld1 {v2.s}[0], [x0], x1
    ld1 {v3.s}[0], [x0], x1
    //load pred
    ld1 {v4.s}[0], [x2], x3
    ld1 {v5.s}[0], [x2], x3
    ld1 {v6.s}[0], [x2], x3
    ld1 {v7.s}[0], [x2], x3

    uabdl v0.8h, v0.8b, v4.8b
    uabdl v1.8h, v1.8b, v5.8b
    uabdl v2.8h, v2.8b, v6.8b
    uabdl v3.8h, v3.8b, v7.8b

    umlal v16.4s, v0.4h, v0.4h
    umlal v16.4s, v1.4h, v1.4h
    umlal v16.4s, v2.4h, v2.4h
    umlal v16.4s, v3.4h, v3.4h

    subs w4, w4, #4
    bgt get_ssd_4_y

    uaddlp v16.2d, v16.4s
    addp v16.2d, v16.2d, v16.2d
    mov x0, v16.d[0]

ret

//void uavs3e_get_ssd_8_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_8_arm64
    movi v16.16b, #0

get_ssd_8_y:
    //load p_org
    ld1 {v0.8b}, [x0], x1
    ld1 {v1.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v3.8b}, [x0], x1
    //load pred
    ld1 {v4.8b}, [x2], x3
    ld1 {v5.8b}, [x2], x3
    ld1 {v6.8b}, [x2], x3
    ld1 {v7.8b}, [x2], x3

    uabdl v0.8h, v0.8b, v4.8b
    uabdl v1.8h, v1.8b, v5.8b
    uabdl v2.8h, v2.8b, v6.8b
    uabdl v3.8h, v3.8b, v7.8b

    umlal v16.4s, v0.4h, v0.4h
    umlal2 v16.4s, v0.8h, v0.8h
    umlal v16.4s, v1.4h, v1.4h
    umlal2 v16.4s, v1.8h, v1.8h
    umlal v16.4s, v2.4h, v2.4h
    umlal2 v16.4s, v2.8h, v2.8h
    umlal v16.4s, v3.4h, v3.4h
    umlal2 v16.4s, v3.8h, v3.8h

    subs w4, w4, #4
    bgt get_ssd_8_y

    uaddlp v16.2d, v16.4s
    addp v16.2d, v16.2d, v16.2d
    mov x0, v16.d[0]

ret

//void uavs3e_get_ssd_16_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_16_arm64
    movi v24.16b, #0

get_ssd_16_y:
    //load p_org
    ld1 {v0.8b, v1.8b}, [x0], x1
    ld1 {v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b}, [x0], x1
    ld1 {v6.8b, v7.8b}, [x0], x1
    //load pred
    ld1 {v16.8b, v17.8b}, [x2], x3
    ld1 {v18.8b, v19.8b}, [x2], x3
    ld1 {v20.8b, v21.8b}, [x2], x3
    ld1 {v22.8b, v23.8b}, [x2], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    subs w4, w4, #4
    bgt get_ssd_16_y

    uaddlp v24.2d, v24.4s
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_ssd_32_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_32_arm64
    movi v24.16b, #0

get_ssd_32_y:
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x0], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x2], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x2], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    subs w4, w4, #2
    bgt get_ssd_32_y

    uaddlp v24.2d, v24.4s
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_ssd_64_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_64_arm64
    lsl x5, x1, #1      //2 * i_org
    lsl x6, x3, #1      //2 * i_pred
    movi v24.16b, #0


get_ssd_64_y:
    mov x7, x0
    mov x8, x2

    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x7], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x7], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x8], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x8], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x7, x0, #32
    add x8, x2, #32
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x7], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x7], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x8], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x8], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x0, x0, x5
    add x2, x2, x6

    subs w4, w4, #2
    bgt get_ssd_64_y

    uaddlp v24.2d, v24.4s
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_ssd_128_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_128_arm64
    lsl x5, x1, #1      //2 * i_org
    lsl x6, x3, #1      //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0


get_ssd_128_y:
    mov x7, x0
    mov x8, x2

    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x7], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x7], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x8], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x8], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x7, x0, #32
    add x8, x2, #32
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x7], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x7], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x8], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x8], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x7, x0, #64
    add x8, x2, #64

    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x7], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x7], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x8], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x8], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v25.4s, v0.4h, v0.4h
    umlal2 v25.4s, v0.8h, v0.8h
    umlal v25.4s, v1.4h, v1.4h
    umlal2 v25.4s, v1.8h, v1.8h
    umlal v25.4s, v2.4h, v2.4h
    umlal2 v25.4s, v2.8h, v2.8h
    umlal v25.4s, v3.4h, v3.4h
    umlal2 v25.4s, v3.8h, v3.8h
    umlal v25.4s, v4.4h, v4.4h
    umlal2 v25.4s, v4.8h, v4.8h
    umlal v25.4s, v5.4h, v5.4h
    umlal2 v25.4s, v5.8h, v5.8h
    umlal v25.4s, v6.4h, v6.4h
    umlal2 v25.4s, v6.8h, v6.8h
    umlal v25.4s, v7.4h, v7.4h
    umlal2 v25.4s, v7.8h, v7.8h

    add x7, x0, #96
    add x8, x2, #96
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x7], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x7], x1
    //load pred
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x8], x3
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x8], x3

    uabdl v0.8h, v0.8b, v16.8b
    uabdl v1.8h, v1.8b, v17.8b
    uabdl v2.8h, v2.8b, v18.8b
    uabdl v3.8h, v3.8b, v19.8b
    uabdl v4.8h, v4.8b, v20.8b
    uabdl v5.8h, v5.8b, v21.8b
    uabdl v6.8h, v6.8b, v22.8b
    uabdl v7.8h, v7.8b, v23.8b

    umlal v25.4s, v0.4h, v0.4h
    umlal2 v25.4s, v0.8h, v0.8h
    umlal v25.4s, v1.4h, v1.4h
    umlal2 v25.4s, v1.8h, v1.8h
    umlal v25.4s, v2.4h, v2.4h
    umlal2 v25.4s, v2.8h, v2.8h
    umlal v25.4s, v3.4h, v3.4h
    umlal2 v25.4s, v3.8h, v3.8h
    umlal v25.4s, v4.4h, v4.4h
    umlal2 v25.4s, v4.8h, v4.8h
    umlal v25.4s, v5.4h, v5.4h
    umlal2 v25.4s, v5.8h, v5.8h
    umlal v25.4s, v6.4h, v6.4h
    umlal2 v25.4s, v6.8h, v6.8h
    umlal v25.4s, v7.4h, v7.4h
    umlal2 v25.4s, v7.8h, v7.8h

    add x0, x0, x5
    add x2, x2, x6

    subs w4, w4, #2
    bgt get_ssd_128_y

    uaddl v26.2d, v24.2s, v25.2s
    uaddl2 v27.2d, v24.4s, v25.4s
    add v24.2d, v26.2d, v27.2d
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_sad_x3_4_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_4_arm64
    movi v16.16b, #0
    movi v17.16b, #0
    movi v18.16b, #0

get_sad_x3_4_y:
    //load p_org
    ld1 {v0.s}[0], [x0], x1
    ld1 {v0.s}[1], [x0], x1
    ld1 {v1.s}[0], [x0], x1
    ld1 {v1.s}[1], [x0], x1
    //load pred
    ld1 {v2.s}[0], [x2], x5
    ld1 {v2.s}[1], [x2], x5
    ld1 {v3.s}[0], [x2], x5
    ld1 {v3.s}[1], [x2], x5
    ld1 {v4.s}[0], [x3], x5
    ld1 {v4.s}[1], [x3], x5
    ld1 {v5.s}[0], [x3], x5
    ld1 {v5.s}[1], [x3], x5
    ld1 {v6.s}[0], [x4], x5
    ld1 {v6.s}[1], [x4], x5
    ld1 {v7.s}[0], [x4], x5
    ld1 {v7.s}[1], [x4], x5
    //abs
    uabdl v2.8h, v0.8b, v2.8b
    uabdl v3.8h, v1.8b, v3.8b
    uabdl v4.8h, v0.8b, v4.8b
    uabdl v5.8h, v1.8b, v5.8b
    uabdl v6.8h, v0.8b, v6.8b
    uabdl v7.8h, v1.8b, v7.8b

    add v2.8h, v2.8h, v3.8h
    add v4.8h, v4.8h, v5.8h
    add v6.8h, v6.8h, v7.8h
    add v16.8h, v16.8h, v2.8h
    add v17.8h, v17.8h, v4.8h
    add v18.8h, v18.8h, v6.8h
    subs w7, w7, #4
    bgt get_sad_x3_4_y

    uaddlp v16.4s, v16.8h
    uaddlp v17.4s, v17.8h
    uaddlp v18.4s, v18.8h
    addp v16.4s, v16.4s, v17.4s
    addp v18.4s, v18.4s, v18.4s
    addp v16.4s, v16.4s, v18.4s

    st1 {v16.d}[0], [x6], #8
    st1 {v16.s}[2], [x6]

    ret

//void uavs3e_get_sad_x3_8_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_8_arm64
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_8_y:
    //load p_org
    ld1 {v0.8b}, [x0], x1
    ld1 {v1.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v3.8b}, [x0], x1
    //load pred
    ld1 {v4.8b}, [x2], x5
    ld1 {v5.8b}, [x2], x5
    ld1 {v6.8b}, [x2], x5
    ld1 {v7.8b}, [x2], x5
    ld1 {v16.8b}, [x3], x5
    ld1 {v17.8b}, [x3], x5
    ld1 {v18.8b}, [x3], x5
    ld1 {v19.8b}, [x3], x5
    ld1 {v20.8b}, [x4], x5
    ld1 {v21.8b}, [x4], x5
    ld1 {v22.8b}, [x4], x5
    ld1 {v23.8b}, [x4], x5
    //abs
    uabdl v4.8h, v0.8b, v4.8b
    uabdl v5.8h, v1.8b, v5.8b
    uabdl v6.8h, v2.8b, v6.8b
    uabdl v7.8h, v3.8b, v7.8b
    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v0.8b, v20.8b
    uabdl v21.8h, v1.8b, v21.8b
    uabdl v22.8h, v2.8b, v22.8b
    uabdl v23.8h, v3.8b, v23.8b

    add v4.8h, v4.8h, v5.8h
    add v5.8h, v6.8h, v7.8h
    add v4.8h, v4.8h, v5.8h
    add v24.8h, v24.8h, v4.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    add v25.8h, v25.8h, v16.8h
    add v20.8h, v20.8h, v21.8h
    add v21.8h, v22.8h, v23.8h
    add v20.8h, v20.8h, v21.8h
    add v26.8h, v26.8h, v20.8h

    subs w7, w7, #4
    bgt get_sad_x3_8_y

    uaddlp v24.4s, v24.8h
    uaddlp v25.4s, v25.8h
    uaddlp v26.4s, v26.8h
    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]

ret

//void uavs3e_get_sad_x3_16_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_16_arm64
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_16_y:
    //load p_org
    ld1 {v0.8b, v1.8b}, [x0], x1
    ld1 {v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b}, [x0], x1
    ld1 {v6.8b, v7.8b}, [x0], x1
    //load pred0
    ld1 {v16.8b, v17.8b}, [x2], x5
    ld1 {v18.8b, v19.8b}, [x2], x5
    ld1 {v20.8b, v21.8b}, [x2], x5
    ld1 {v22.8b, v23.8b}, [x2], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b}, [x3], x5
    ld1 {v18.8b, v19.8b}, [x3], x5
    ld1 {v20.8b, v21.8b}, [x3], x5
    ld1 {v22.8b, v23.8b}, [x3], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b}, [x4], x5
    ld1 {v18.8b, v19.8b}, [x4], x5
    ld1 {v20.8b, v21.8b}, [x4], x5
    ld1 {v22.8b, v23.8b}, [x4], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    subs w7, w7, #4
    bgt get_sad_x3_16_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
ret

//void uavs3e_get_sad_x3_32_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_32_arm64
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_32_y:
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x0], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x2], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x2], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x3], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x3], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x4], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x4], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    subs w7, w7, #2
    bgt get_sad_x3_32_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
ret


//void uavs3e_get_sad_x3_64_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_64_arm64
    lsl x12, x1, #1     //2 * i_org
    lsl x13, x5, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_64_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #32
    add x9, x2, #32
    add x10, x3, #32
    add x11, x4, #32
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x0, x0, x12
    add x2, x2, x13
    add x3, x3, x13
    add x4, x4, x13

    subs w7, w7, #2
    bgt get_sad_x3_64_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
    ret

//void uavs3e_get_sad_x3_128_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_128_arm64
    lsl x12, x1, #1     //2 * i_org
    lsl x13, x5, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_128_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #32
    add x9, x2, #32
    add x10, x3, #32
    add x11, x4, #32
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #64
    add x9, x2, #64
    add x10, x3, #64
    add x11, x4, #64
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #96
    add x9, x2, #96
    add x10, x3, #96
    add x11, x4, #96
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x5
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x5

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x0, x0, x12
    add x2, x2, x13
    add x3, x3, x13
    add x4, x4, x13

    subs w7, w7, #2
    bgt get_sad_x3_128_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
ret

//void uavs3e_get_sad_x4_4_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_4_arm64
    ldr x8, [sp]
    movi v18.16b, #0
    movi v19.16b, #0
    movi v20.16b, #0
    movi v21.16b, #0

get_sad_x4_4_y:
    //load p_org
    ld1 {v0.s}[0], [x0], x1
    ld1 {v0.s}[1], [x0], x1
    ld1 {v1.s}[0], [x0], x1
    ld1 {v1.s}[1], [x0], x1
    //load pred
    ld1 {v2.s}[0], [x2], x6
    ld1 {v2.s}[1], [x2], x6
    ld1 {v3.s}[0], [x2], x6
    ld1 {v3.s}[1], [x2], x6
    ld1 {v4.s}[0], [x3], x6
    ld1 {v4.s}[1], [x3], x6
    ld1 {v5.s}[0], [x3], x6
    ld1 {v5.s}[1], [x3], x6
    ld1 {v6.s}[0], [x4], x6
    ld1 {v6.s}[1], [x4], x6
    ld1 {v7.s}[0], [x4], x6
    ld1 {v7.s}[1], [x4], x6
    ld1 {v16.s}[0], [x5], x6
    ld1 {v16.s}[1], [x5], x6
    ld1 {v17.s}[0], [x5], x6
    ld1 {v17.s}[1], [x5], x6

    //abs
    uabdl v2.8h, v0.8b, v2.8b
    uabdl v3.8h, v1.8b, v3.8b
    uabdl v4.8h, v0.8b, v4.8b
    uabdl v5.8h, v1.8b, v5.8b
    uabdl v6.8h, v0.8b, v6.8b
    uabdl v7.8h, v1.8b, v7.8b
    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b

    add v2.8h, v2.8h, v3.8h
    add v4.8h, v4.8h, v5.8h
    add v6.8h, v6.8h, v7.8h
    add v16.8h, v16.8h, v17.8h
    add v18.8h, v18.8h, v2.8h
    add v19.8h, v19.8h, v4.8h
    add v20.8h, v20.8h, v6.8h
    add v21.8h, v21.8h, v16.8h

    subs w8, w8, #4
    bgt get_sad_x4_4_y

    uaddlp v18.4s, v18.8h
    uaddlp v19.4s, v19.8h
    uaddlp v20.4s, v20.8h
    uaddlp v21.4s, v21.8h
    addp v18.4s, v18.4s, v19.4s
    addp v20.4s, v20.4s, v21.4s
    addp v18.4s, v18.4s, v20.4s

    st1 {v18.4s}, [x7]
    ret

//void uavs3e_get_sad_x4_8_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_8_arm64
    ldr x8, [sp]
    movi v28.16b, #0
    movi v29.16b, #0
    movi v30.16b, #0
    movi v31.16b, #0

get_sad_x4_8_y:
    //load p_org
    ld1 {v0.8b}, [x0], x1
    ld1 {v1.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v3.8b}, [x0], x1
    //load pred
    ld1 {v4.8b}, [x2], x6
    ld1 {v5.8b}, [x2], x6
    ld1 {v6.8b}, [x2], x6
    ld1 {v7.8b}, [x2], x6
    ld1 {v16.8b}, [x3], x6
    ld1 {v17.8b}, [x3], x6
    ld1 {v18.8b}, [x3], x6
    ld1 {v19.8b}, [x3], x6
    ld1 {v20.8b}, [x4], x6
    ld1 {v21.8b}, [x4], x6
    ld1 {v22.8b}, [x4], x6
    ld1 {v23.8b}, [x4], x6
    ld1 {v24.8b}, [x5], x6
    ld1 {v25.8b}, [x5], x6
    ld1 {v26.8b}, [x5], x6
    ld1 {v27.8b}, [x5], x6

    //abs
    uabdl v4.8h, v0.8b, v4.8b
    uabdl v5.8h, v1.8b, v5.8b
    uabdl v6.8h, v2.8b, v6.8b
    uabdl v7.8h, v3.8b, v7.8b
    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v0.8b, v20.8b
    uabdl v21.8h, v1.8b, v21.8b
    uabdl v22.8h, v2.8b, v22.8b
    uabdl v23.8h, v3.8b, v23.8b
    uabdl v24.8h, v0.8b, v24.8b
    uabdl v25.8h, v1.8b, v25.8b
    uabdl v26.8h, v2.8b, v26.8b
    uabdl v27.8h, v3.8b, v27.8b

    add v4.8h, v4.8h, v5.8h
    add v6.8h, v6.8h, v7.8h
    add v4.8h, v4.8h, v6.8h
    add v28.8h, v28.8h, v4.8h
    add v16.8h, v16.8h, v17.8h
    add v18.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v18.8h
    add v29.8h, v29.8h, v16.8h
    add v20.8h, v20.8h, v21.8h
    add v22.8h, v22.8h, v23.8h
    add v20.8h, v20.8h, v22.8h
    add v30.8h, v30.8h, v20.8h
    add v24.8h, v24.8h, v25.8h
    add v26.8h, v26.8h, v27.8h
    add v24.8h, v24.8h, v26.8h
    add v31.8h, v31.8h, v24.8h

    subs w8, w8, #4
    bgt get_sad_x4_8_y

    uaddlp v28.4s, v28.8h
    uaddlp v29.4s, v29.8h
    uaddlp v30.4s, v30.8h
    uaddlp v31.4s, v31.8h
    addp v28.4s, v28.4s, v29.4s
    addp v30.4s, v30.4s, v31.4s
    addp v28.4s, v28.4s, v30.4s

    st1 {v28.4s}, [x7]

ret

//void uavs3e_get_sad_x4_16_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_16_arm64
    ldr x8, [sp]
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_16_y:
    //load p_org
    ld1 {v0.8b, v1.8b}, [x0], x1
    ld1 {v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b}, [x0], x1
    ld1 {v6.8b, v7.8b}, [x0], x1

    //load pred0
    ld1 {v16.8b, v17.8b}, [x2], x6
    ld1 {v18.8b, v19.8b}, [x2], x6
    ld1 {v20.8b, v21.8b}, [x2], x6
    ld1 {v22.8b, v23.8b}, [x2], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b}, [x3], x6
    ld1 {v18.8b, v19.8b}, [x3], x6
    ld1 {v20.8b, v21.8b}, [x3], x6
    ld1 {v22.8b, v23.8b}, [x3], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b}, [x4], x6
    ld1 {v18.8b, v19.8b}, [x4], x6
    ld1 {v20.8b, v21.8b}, [x4], x6
    ld1 {v22.8b, v23.8b}, [x4], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b}, [x5], x6
    ld1 {v18.8b, v19.8b}, [x5], x6
    ld1 {v20.8b, v21.8b}, [x5], x6
    ld1 {v22.8b, v23.8b}, [x5], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    subs w8, w8, #4
    bgt get_sad_x4_16_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]
ret

//void uavs3e_get_sad_x4_32_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[3]->x7, height->[x8]
function uavs3e_get_sad_x4_32_arm64
    ldr x8, [sp]
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_32_y:
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x0], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x2], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x2], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x3], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x3], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x4], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x4], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x5], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x5], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    subs w8, w8, #2
    bgt get_sad_x4_32_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]

ret


//void uavs3e_get_sad_x4_64_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[4], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_64_arm64
    ldr x15, [sp]
    lsl x13, x1, #1     //2 * i_org
    lsl x14, x6, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_64_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    mov x12, x5
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x12], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x12], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #32
    add x9, x2, #32
    add x10, x3, #32
    add x11, x4, #32
    add x12, x5, #32
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x12], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x12], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x0, x0, x13
    add x2, x2, x14
    add x3, x3, x14
    add x4, x4, x14
    add x5, x5, x14

    subs w15, w15, #2
    bgt get_sad_x4_64_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]
ret

//void uavs3e_get_sad_x4_128_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[4], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_128_arm64
    ldr x15, [sp]
    lsl x13, x1, #1     //2 * i_org
    lsl x14, x6, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_128_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    mov x12, x5
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x12], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x12], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #32
    add x9, x2, #32
    add x10, x3, #32
    add x11, x4, #32
    add x12, x5, #32
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x12], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x12], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #64
    add x9, x2, #64
    add x10, x3, #64
    add x11, x4, #64
    add x12, x5, #64
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x12], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x12], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #96
    add x9, x2, #96
    add x10, x3, #96
    add x11, x4, #96
    add x12, x5, #96
    //load p_org
    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x8], x1
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x8], x1
    //load pred0
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x9], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x9], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x10], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x10], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x11], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x11], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8b, v17.8b, v18.8b, v19.8b}, [x12], x6
    ld1 {v20.8b, v21.8b, v22.8b, v23.8b}, [x12], x6

    uabdl v16.8h, v0.8b, v16.8b
    uabdl v17.8h, v1.8b, v17.8b
    uabdl v18.8h, v2.8b, v18.8b
    uabdl v19.8h, v3.8b, v19.8b
    uabdl v20.8h, v4.8b, v20.8b
    uabdl v21.8h, v5.8b, v21.8b
    uabdl v22.8h, v6.8b, v22.8b
    uabdl v23.8h, v7.8b, v23.8b

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x0, x0, x13
    add x2, x2, x14
    add x3, x3, x14
    add x4, x4, x14
    add x5, x5, x14

    subs w15, w15, #2
    bgt get_sad_x4_128_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]

ret

//void uavs3e_had_4x4_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_4x4_arm64

    ld1 {v0.s}[0], [x0], x1
    ld1 {v0.s}[1], [x0], x1
    ld1 {v2.s}[0], [x0], x1
    ld1 {v2.s}[1], [x0], x1

    ld1 {v1.s}[0], [x2], x3
    ld1 {v1.s}[1], [x2], x3
    ld1 {v3.s}[0], [x2], x3
    ld1 {v3.s}[1], [x2], x3

    uxtl v0.8h, v0.8b
    uxtl v1.8h, v1.8b
    uxtl v2.8h, v2.8b
    uxtl v3.8h, v3.8b

    sub v0.8h, v0.8h, v1.8h
    sub v1.8h, v2.8h, v3.8h

    uzp1 v2.8h, v0.8h, v1.8h    //d0, d2, d4, d6
    uzp2 v3.8h, v0.8h, v1.8h    //d1, d3, d5, d7

    add v0.8h, v2.8h, v3.8h     //d0 + d1
    sub v1.8h, v2.8h, v3.8h     //d0 - d1

    trn1 v2.8h, v0.8h, v1.8h    //d0 + d1, d0 - d1, d4 + d5, d4 - d5
    trn2 v3.8h, v0.8h, v1.8h    //d2 + d3, d2 - d3, d6 + d7, d6 - d7

    add v0.8h, v2.8h, v3.8h     //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v2.8h, v3.8h     //d0 + d1 - d2 - d3, d0 - d1 - d2 + d3

    trn1 v2.4s, v0.4s, v1.4s    //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - d2 - d3, d0 - d1 - d2 + d3
    trn2 v3.4s, v0.4s, v1.4s    //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - d6 - d7, d4 - d5 - d6 + d7

    add v0.8h, v2.8h, v3.8h
    sub v1.8h, v2.8h, v3.8h

    trn1 v2.2d, v0.2d, v1.2d
    trn2 v3.2d, v0.2d, v1.2d

    add v0.8h, v2.8h, v3.8h
    sub v1.8h, v2.8h, v3.8h

    abs v0.8h, v0.8h
    abs v1.8h, v1.8h

    uaddl v2.4s, v0.4h, v1.4h
    uaddl2 v3.4s, v0.8h, v1.8h
    add v0.4s, v2.4s, v3.4s
    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    add x0, x0, #1
    lsr x0, x0, #1

ret

//void uavs3e_had_8x8_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_8x8_arm64
    ld1 {v0.8b}, [x0], x1
    ld1 {v1.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v3.8b}, [x0], x1
    ld1 {v4.8b}, [x0], x1
    ld1 {v5.8b}, [x0], x1
    ld1 {v6.8b}, [x0], x1
    ld1 {v7.8b}, [x0], x1

    ld1 {v16.8b}, [x2], x3
    ld1 {v17.8b}, [x2], x3
    ld1 {v18.8b}, [x2], x3
    ld1 {v19.8b}, [x2], x3
    ld1 {v20.8b}, [x2], x3
    ld1 {v21.8b}, [x2], x3
    ld1 {v22.8b}, [x2], x3
    ld1 {v23.8b}, [x2], x3

    uxtl v0.8h, v0.8b
    uxtl v1.8h, v1.8b
    uxtl v2.8h, v2.8b
    uxtl v3.8h, v3.8b
    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b

    uxtl v16.8h, v16.8b
    uxtl v17.8h, v17.8b
    uxtl v18.8h, v18.8b
    uxtl v19.8h, v19.8b
    uxtl v20.8h, v20.8b
    uxtl v21.8h, v21.8b
    uxtl v22.8h, v22.8b
    uxtl v23.8h, v23.8b

    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h

    uzp1 v16.8h, v0.8h, v1.8h       //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v17.8h, v0.8h, v1.8h       //d1, d3, d5, d7,
    uzp1 v18.8h, v2.8h, v3.8h       //d16, d18, d20, d22,
    uzp2 v19.8h, v2.8h, v3.8h       //d17, d19, d21, d23,
    uzp1 v20.8h, v4.8h, v5.8h       //d32, d34, d36, d38,
    uzp2 v21.8h, v4.8h, v5.8h       //d33, d35, d37, d39,
    uzp1 v22.8h, v6.8h, v7.8h       //d48, d50, d52, d54,
    uzp2 v23.8h, v6.8h, v7.8h       //d49, d51, d53, d55,

    add v0.8h, v16.8h, v17.8h       //d0 + d1, d2 + d3,
    sub v1.8h, v16.8h, v17.8h       //d0 - d1, d2 - d3,
    add v2.8h, v18.8h, v19.8h       //d16 + d17, d18 + d19
    sub v3.8h, v18.8h, v19.8h       //d16 - d17, d18 - d19
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    trn1 v16.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1, d4 + d5, d4 - d5
    trn2 v17.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3, d6 + d7, d5 - d7
    trn1 v18.8h, v2.8h, v3.8h
    trn2 v19.8h, v2.8h, v3.8h
    trn1 v20.8h, v4.8h, v5.8h
    trn2 v21.8h, v4.8h, v5.8h
    trn1 v22.8h, v6.8h, v7.8h
    trn2 v23.8h, v6.8h, v7.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v18.8h, v19.8h       //d16 + d17 + d18 + d19, d16 - d17 + (d18 - d19)
    sub v3.8h, v18.8h, v19.8h       //d16 + d17 - d18 + d19, d16 - d17 - (d18 - d19)
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    trn1 v16.4s, v0.4s, v1.4s       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v17.4s, v0.4s, v1.4s       //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v18.4s, v2.4s, v3.4s
    trn2 v19.4s, v2.4s, v3.4s
    trn1 v20.4s, v4.4s, v5.4s
    trn2 v21.4s, v4.4s, v5.4s
    trn1 v22.4s, v6.4s, v7.4s
    trn2 v23.4s, v6.4s, v7.4s

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7, d0 - d1 + d2 - d3 + d4 - d5 + d6 - d7
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v18.8h, v19.8h       //d16 + d17 + d18 + d19 + d20 + d21 + d22 + d23
    sub v3.8h, v18.8h, v19.8h       //d16 + d17 + d18 + d19 - (d20 + d21 + d22 + d23)
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    trn1 v16.2d, v0.2d, v1.2d
    trn2 v17.2d, v0.2d, v1.2d
    trn1 v18.2d, v2.2d, v3.2d
    trn2 v19.2d, v2.2d, v3.2d
    trn1 v20.2d, v4.2d, v5.2d
    trn2 v21.2d, v4.2d, v5.2d
    trn1 v22.2d, v6.2d, v7.2d
    trn2 v23.2d, v6.2d, v7.2d

    add v0.8h, v16.8h, v17.8h
    sub v1.8h, v16.8h, v17.8h
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    add v16.8h, v0.8h, v2.8h
    sub v17.8h, v0.8h, v2.8h
    add v18.8h, v1.8h, v3.8h
    sub v19.8h, v1.8h, v3.8h
    add v20.8h, v4.8h, v6.8h
    sub v21.8h, v4.8h, v6.8h
    add v22.8h, v5.8h, v7.8h
    sub v23.8h, v5.8h, v7.8h

    saddl v0.4s, v16.4h, v20.4h
    saddl2 v1.4s, v16.8h, v20.8h
    saddl v2.4s, v17.4h, v21.4h
    saddl2 v3.4s, v17.8h, v21.8h
    saddl v4.4s, v18.4h, v22.4h
    saddl2 v5.4s, v18.8h, v22.8h
    saddl v6.4s, v19.4h, v23.4h
    saddl2 v7.4s, v19.8h, v23.8h
    ssubl v24.4s, v16.4h, v20.4h
    ssubl2 v25.4s, v16.8h, v20.8h
    ssubl v26.4s, v17.4h, v21.4h
    ssubl2 v27.4s, v17.8h, v21.8h
    ssubl v28.4s, v18.4h, v22.4h
    ssubl2 v29.4s, v18.8h, v22.8h
    ssubl v30.4s, v19.4h, v23.4h
    ssubl2 v31.4s, v19.8h, v23.8h

    abs v0.4s, v0.4s
    abs v1.4s, v1.4s
    abs v2.4s, v2.4s
    abs v3.4s, v3.4s
    abs v4.4s, v4.4s
    abs v5.4s, v5.4s
    abs v6.4s, v6.4s
    abs v7.4s, v7.4s
    abs v24.4s, v24.4s
    abs v25.4s, v25.4s
    abs v26.4s, v26.4s
    abs v27.4s, v27.4s
    abs v28.4s, v28.4s
    abs v29.4s, v29.4s
    abs v30.4s, v30.4s
    abs v31.4s, v31.4s

    add v0.4s, v0.4s, v24.4s
    add v1.4s, v1.4s, v25.4s
    add v2.4s, v2.4s, v26.4s
    add v3.4s, v3.4s, v27.4s
    add v4.4s, v4.4s, v28.4s
    add v5.4s, v5.4s, v29.4s
    add v6.4s, v6.4s, v30.4s
    add v7.4s, v7.4s, v31.4s

    add v0.4s, v0.4s, v1.4s
    add v2.4s, v2.4s, v3.4s
    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v0.4s, v0.4s, v2.4s
    add v4.4s, v4.4s, v6.4s
    add v0.4s, v0.4s, v4.4s

    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    add x0, x0, #2
    lsr x0, x0, #2

ret

//void uavs3e_had_8x4_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_8x4_arm64
    ld1 {v0.8b}, [x0], x1
    ld1 {v1.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v3.8b}, [x0], x1

    ld1 {v4.8b}, [x2], x3
    ld1 {v5.8b}, [x2], x3
    ld1 {v6.8b}, [x2], x3
    ld1 {v7.8b}, [x2], x3

    uxtl v0.8h, v0.8b
    uxtl v1.8h, v1.8b
    uxtl v2.8h, v2.8b
    uxtl v3.8h, v3.8b
    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b

    sub v0.8h, v0.8h, v4.8h
    sub v1.8h, v1.8h, v5.8h
    sub v2.8h, v2.8h, v6.8h
    sub v3.8h, v3.8h, v7.8h

    uzp1 v4.8h, v0.8h, v1.8h       //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v5.8h, v0.8h, v1.8h       //d1, d3, d5, d7, d9, d11, d13, d15
    uzp1 v6.8h, v2.8h, v3.8h       //d16, d18, d20, d22, d24, d26, d28, d30
    uzp2 v7.8h, v2.8h, v3.8h       //d17, d19, d21, d23, d25, d27, d29, d31


    add v0.8h, v4.8h, v5.8h        //d0 + d1, d2 + d3,
    sub v1.8h, v4.8h, v5.8h        //d0 - d1, d2 - d3,
    add v2.8h, v6.8h, v7.8h        //d16 + d17, d18 + d19
    sub v3.8h, v6.8h, v7.8h        //d16 - d17, d18 - d19


    trn1 v4.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1, d4 + d5, d4 - d5
    trn2 v5.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3, d6 + d7, d5 - d7
    trn1 v6.8h, v2.8h, v3.8h       //d16 + d17, d16 - d17
    trn2 v7.8h, v2.8h, v3.8h       //d18 + d19, d18 - d19

    add v0.8h, v4.8h, v5.8h        //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v4.8h, v5.8h        //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v6.8h, v7.8h        //d16 + d17 + d18 + d19, d16 - d17 + (d18 - d19)
    sub v3.8h, v6.8h, v7.8h        //d16 + d17 - d18 + d19, d16 - d17 - (d18 - d19)

    trn1 v4.4s, v0.4s, v1.4s      //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v5.4s, v0.4s, v1.4s      //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v6.4s, v2.4s, v3.4s
    trn2 v7.4s, v2.4s, v3.4s

    add v0.8h, v4.8h, v5.8h        //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7, d0 - d1 + d2 - d3 + d4 - d5 + d6 - d7
    sub v1.8h, v4.8h, v5.8h        //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v6.8h, v7.8h        //d16 + d17 + d18 + d19 + d20 + d21 + d22 + d23
    sub v3.8h, v6.8h, v7.8h        //d16 + d17 + d18 + d19 - (d20 + d21 + d22 + d23)

    trn1 v4.2d, v0.2d, v1.2d       //d0...7
    trn2 v5.2d, v0.2d, v1.2d       //d8...15
    trn1 v6.2d, v2.2d, v3.2d       //d16...d23
    trn2 v7.2d, v2.2d, v3.2d       //d24...d31

    add v0.8h, v4.8h, v6.8h
    add v1.8h, v5.8h, v7.8h
    sub v2.8h, v4.8h, v6.8h
    sub v3.8h, v5.8h, v7.8h

    add v4.8h, v0.8h, v1.8h
    sub v5.8h, v0.8h, v1.8h
    add v6.8h, v2.8h, v3.8h
    sub v7.8h, v2.8h, v3.8h

    abs v0.8h, v4.8h
    abs v1.8h, v5.8h
    abs v2.8h, v6.8h
    abs v3.8h, v7.8h

    uaddl v4.4s, v0.4h, v1.4h
    uaddl2 v5.4s, v0.8h, v1.8h
    uaddl v6.4s, v2.4h, v3.4h
    uaddl2 v7.4s, v2.8h, v3.8h

    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v0.4s, v4.4s, v6.4s

    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    mov x1, #32
    ucvtf d0, x1
    fsqrt d0, d0
    ucvtf d1, x0
    fdiv d0, d1, d0
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0

    ret

//void uavs3e_had_4x8_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_4x8_arm64
    ld1 {v0.s}[0], [x0], x1
    ld1 {v0.s}[1], [x0], x1
    ld1 {v1.s}[0], [x0], x1
    ld1 {v1.s}[1], [x0], x1
    ld1 {v2.s}[0], [x0], x1
    ld1 {v2.s}[1], [x0], x1
    ld1 {v3.s}[0], [x0], x1
    ld1 {v3.s}[1], [x0], x1

    ld1 {v4.s}[0], [x2], x3
    ld1 {v4.s}[1], [x2], x3
    ld1 {v5.s}[0], [x2], x3
    ld1 {v5.s}[1], [x2], x3
    ld1 {v6.s}[0], [x2], x3
    ld1 {v6.s}[1], [x2], x3
    ld1 {v7.s}[0], [x2], x3
    ld1 {v7.s}[1], [x2], x3

    uxtl v0.8h, v0.8b
    uxtl v1.8h, v1.8b
    uxtl v2.8h, v2.8b
    uxtl v3.8h, v3.8b
    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b

    sub v0.8h, v0.8h, v4.8h
    sub v1.8h, v1.8h, v5.8h
    sub v2.8h, v2.8h, v6.8h
    sub v3.8h, v3.8h, v7.8h

    uzp1 v4.8h, v0.8h, v1.8h        //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v5.8h, v0.8h, v1.8h        //d1, d3, d5, d7, d9, d11, d13, d15
    uzp1 v6.8h, v2.8h, v3.8h        //d16, d18, d20, d22, d24, d26, d28, d30
    uzp2 v7.8h, v2.8h, v3.8h        //d17, d19, d21, d23, d25, d27, d29, d31

    add v0.8h, v4.8h, v5.8h         //d0 + d1, d2 + d3
    sub v1.8h, v4.8h, v5.8h         //d0 - d1, d2 - d3
    add v2.8h, v6.8h, v7.8h         //d16 + d17, d18 + d19
    sub v3.8h, v6.8h, v7.8h         //d16 - d17, d18 - d19

    trn1 v4.8h, v0.8h, v1.8h        //d0 + d1, d0 - d1
    trn2 v5.8h, v0.8h, v1.8h        //d2 + d3, d2 - d3
    trn1 v6.8h, v2.8h, v3.8h        //d16 + d17, d16 - d17
    trn2 v7.8h, v2.8h, v3.8h        //d18 + d19, d18 - d19

    add v0.8h, v4.8h, v5.8h         //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v4.8h, v5.8h         //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v6.8h, v7.8h         //d16 + d17 + d18 + d19
    sub v3.8h, v6.8h, v7.8h         //d16 + d17 - (d18 + d19)

    trn1 v4.4s, v0.4s, v1.4s        //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v5.4s, v0.4s, v1.4s        //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v6.4s, v2.4s, v3.4s
    trn2 v7.4s, v2.4s, v3.4s

    add v0.8h, v4.8h, v6.8h
    add v1.8h, v5.8h, v7.8h
    sub v2.8h, v4.8h, v6.8h
    sub v3.8h, v5.8h, v7.8h

    trn1 v4.2d, v0.2d, v1.2d
    trn2 v5.2d, v0.2d, v1.2d
    trn1 v6.2d, v2.2d, v3.2d
    trn2 v7.2d, v2.2d, v3.2d

    add v0.8h, v4.8h, v5.8h
    sub v1.8h, v4.8h, v5.8h
    add v2.8h, v6.8h, v7.8h
    sub v3.8h, v6.8h, v7.8h

    trn1 v4.2d, v0.2d, v1.2d
    trn2 v5.2d, v0.2d, v1.2d
    trn1 v6.2d, v2.2d, v3.2d
    trn2 v7.2d, v2.2d, v3.2d

    add v0.8h, v4.8h, v5.8h
    sub v1.8h, v4.8h, v5.8h
    add v2.8h, v6.8h, v7.8h
    sub v3.8h, v6.8h, v7.8h

    abs v0.8h, v0.8h
    abs v1.8h, v1.8h
    abs v2.8h, v2.8h
    abs v3.8h, v3.8h

    uaddl v4.4s, v0.4h, v1.4h
    uaddl2 v5.4s, v0.8h, v1.8h
    uaddl v6.4s, v2.4h, v3.4h
    uaddl2 v7.4s, v2.8h, v3.8h

    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v0.4s, v4.4s, v6.4s

    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    mov x1, #32
    ucvtf d0, x1
    fsqrt d0, d0
    ucvtf d1, x0
    fdiv d0, d1, d0
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0
    ret

//void uavs3e_had_8x16_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_8x16_arm64
    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    ld1 {v0.8b}, [x0], x1
    ld1 {v1.8b}, [x0], x1
    ld1 {v2.8b}, [x0], x1
    ld1 {v3.8b}, [x0], x1
    ld1 {v4.8b}, [x0], x1
    ld1 {v5.8b}, [x0], x1
    ld1 {v6.8b}, [x0], x1
    ld1 {v7.8b}, [x0], x1
    ld1 {v8.8b}, [x0], x1
    ld1 {v9.8b}, [x0], x1
    ld1 {v10.8b}, [x0], x1
    ld1 {v11.8b}, [x0], x1
    ld1 {v12.8b}, [x0], x1
    ld1 {v13.8b}, [x0], x1
    ld1 {v14.8b}, [x0], x1
    ld1 {v15.8b}, [x0], x1

    ld1 {v16.8b}, [x2], x3
    ld1 {v17.8b}, [x2], x3
    ld1 {v18.8b}, [x2], x3
    ld1 {v19.8b}, [x2], x3
    ld1 {v20.8b}, [x2], x3
    ld1 {v21.8b}, [x2], x3
    ld1 {v22.8b}, [x2], x3
    ld1 {v23.8b}, [x2], x3
    ld1 {v24.8b}, [x2], x3
    ld1 {v25.8b}, [x2], x3
    ld1 {v26.8b}, [x2], x3
    ld1 {v27.8b}, [x2], x3
    ld1 {v28.8b}, [x2], x3
    ld1 {v29.8b}, [x2], x3
    ld1 {v30.8b}, [x2], x3
    ld1 {v31.8b}, [x2], x3

    uxtl v0.8h, v0.8b
    uxtl v1.8h, v1.8b
    uxtl v2.8h, v2.8b
    uxtl v3.8h, v3.8b
    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b
    uxtl v8.8h, v8.8b
    uxtl v9.8h, v9.8b
    uxtl v10.8h, v10.8b
    uxtl v11.8h, v11.8b
    uxtl v12.8h, v12.8b
    uxtl v13.8h, v13.8b
    uxtl v14.8h, v14.8b
    uxtl v15.8h, v15.8b

    uxtl v16.8h, v16.8b
    uxtl v17.8h, v17.8b
    uxtl v18.8h, v18.8b
    uxtl v19.8h, v19.8b
    uxtl v20.8h, v20.8b
    uxtl v21.8h, v21.8b
    uxtl v22.8h, v22.8b
    uxtl v23.8h, v23.8b
    uxtl v24.8h, v24.8b
    uxtl v25.8h, v25.8b
    uxtl v26.8h, v26.8b
    uxtl v27.8h, v27.8b
    uxtl v28.8h, v28.8b
    uxtl v29.8h, v29.8b
    uxtl v30.8h, v30.8b
    uxtl v31.8h, v31.8b

    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h
    sub v8.8h, v8.8h, v24.8h
    sub v9.8h, v9.8h, v25.8h
    sub v10.8h, v10.8h, v26.8h
    sub v11.8h, v11.8h, v27.8h
    sub v12.8h, v12.8h, v28.8h
    sub v13.8h, v13.8h, v29.8h
    sub v14.8h, v14.8h, v30.8h
    sub v15.8h, v15.8h, v31.8h

    uzp1 v16.8h, v0.8h, v1.8h       //d0, d2, d4, d6
    uzp2 v17.8h, v0.8h, v1.8h       //d1, d3, d5, d7
    uzp1 v18.8h, v2.8h, v3.8h       //d16, d18, d20, d22
    uzp2 v19.8h, v2.8h, v3.8h       //d17, d19, d21, d23
    uzp1 v20.8h, v4.8h, v5.8h
    uzp2 v21.8h, v4.8h, v5.8h
    uzp1 v22.8h, v6.8h, v7.8h
    uzp2 v23.8h, v6.8h, v7.8h
    uzp1 v24.8h, v8.8h, v9.8h
    uzp2 v25.8h, v8.8h, v9.8h
    uzp1 v26.8h, v10.8h, v11.8h
    uzp2 v27.8h, v10.8h, v11.8h
    uzp1 v28.8h, v12.8h, v13.8h
    uzp2 v29.8h, v12.8h, v13.8h
    uzp1 v30.8h, v14.8h, v15.8h
    uzp2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1, d2 + d3
    sub v1.8h, v16.8h, v17.8h       //d0 - d1, d2 - d3
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1
    trn2 v17.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3
    trn1 v18.8h, v2.8h, v3.8h
    trn2 v19.8h, v2.8h, v3.8h
    trn1 v20.8h, v4.8h, v5.8h
    trn2 v21.8h, v4.8h, v5.8h
    trn1 v22.8h, v6.8h, v7.8h
    trn2 v23.8h, v6.8h, v7.8h
    trn1 v24.8h, v8.8h, v9.8h
    trn2 v25.8h, v8.8h, v9.8h
    trn1 v26.8h, v10.8h, v11.8h
    trn2 v27.8h, v10.8h, v11.8h
    trn1 v28.8h, v12.8h, v13.8h
    trn2 v29.8h, v12.8h, v13.8h
    trn1 v30.8h, v14.8h, v15.8h
    trn2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.4s, v0.4s, v1.4s       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v17.4s, v0.4s, v1.4s       //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v18.4s, v2.4s, v3.4s
    trn2 v19.4s, v2.4s, v3.4s
    trn1 v20.4s, v4.4s, v5.4s
    trn2 v21.4s, v4.4s, v5.4s
    trn1 v22.4s, v6.4s, v7.4s
    trn2 v23.4s, v6.4s, v7.4s
    trn1 v24.4s, v8.4s, v9.4s
    trn2 v25.4s, v8.4s, v9.4s
    trn1 v26.4s, v10.4s, v11.4s
    trn2 v27.4s, v10.4s, v11.4s
    trn1 v28.4s, v12.4s, v13.4s
    trn2 v29.4s, v12.4s, v13.4s
    trn1 v30.4s, v14.4s, v15.4s
    trn2 v31.4s, v14.4s, v15.4s

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.2d, v0.2d, v1.2d
    trn2 v17.2d, v0.2d, v1.2d
    trn1 v18.2d, v2.2d, v3.2d
    trn2 v19.2d, v2.2d, v3.2d
    trn1 v20.2d, v4.2d, v5.2d
    trn2 v21.2d, v4.2d, v5.2d
    trn1 v22.2d, v6.2d, v7.2d
    trn2 v23.2d, v6.2d, v7.2d
    trn1 v24.2d, v8.2d, v9.2d
    trn2 v25.2d, v8.2d, v9.2d
    trn1 v26.2d, v10.2d, v11.2d
    trn2 v27.2d, v10.2d, v11.2d
    trn1 v28.2d, v12.2d, v13.2d
    trn2 v29.2d, v12.2d, v13.2d
    trn1 v30.2d, v14.2d, v15.2d
    trn2 v31.2d, v14.2d, v15.2d

    add v0.8h, v16.8h, v24.8h
    add v1.8h, v17.8h, v25.8h
    add v2.8h, v18.8h, v26.8h
    add v3.8h, v19.8h, v27.8h
    add v4.8h, v20.8h, v28.8h
    add v5.8h, v21.8h, v29.8h
    add v6.8h, v22.8h, v30.8h
    add v7.8h, v23.8h, v31.8h
    sub v8.8h, v16.8h, v24.8h
    sub v9.8h, v17.8h, v25.8h
    sub v10.8h, v18.8h, v26.8h
    sub v11.8h, v19.8h, v27.8h
    sub v12.8h, v20.8h, v28.8h
    sub v13.8h, v21.8h, v29.8h
    sub v14.8h, v22.8h, v30.8h
    sub v15.8h, v23.8h, v31.8h

    add v16.8h, v0.8h, v4.8h
    add v17.8h, v1.8h, v5.8h
    add v18.8h, v2.8h, v6.8h
    add v19.8h, v3.8h, v7.8h
    sub v20.8h, v0.8h, v4.8h
    sub v21.8h, v1.8h, v5.8h
    sub v22.8h, v2.8h, v6.8h
    sub v23.8h, v3.8h, v7.8h
    add v24.8h, v8.8h, v12.8h
    add v25.8h, v9.8h, v13.8h
    add v26.8h, v10.8h, v14.8h
    add v27.8h, v11.8h, v15.8h
    sub v28.8h, v8.8h, v12.8h
    sub v29.8h, v9.8h, v13.8h
    sub v30.8h, v10.8h, v14.8h
    sub v31.8h, v11.8h, v15.8h

    saddl v0.4s, v16.4h, v18.4h
    saddl2 v1.4s, v16.8h, v18.8h
    saddl v2.4s, v17.4h, v19.4h
    saddl2 v3.4s, v17.8h, v19.8h
    ssubl v4.4s, v16.4h, v18.4h
    ssubl2 v5.4s, v16.8h, v18.8h
    ssubl v6.4s, v17.4h, v19.4h
    ssubl2 v7.4s, v17.8h, v19.8h
    saddl v8.4s, v20.4h, v22.4h
    saddl2 v9.4s, v20.8h, v22.8h
    saddl v10.4s, v21.4h, v23.4h
    saddl2 v11.4s, v21.8h, v23.8h
    ssubl v12.4s, v20.4h, v22.4h
    ssubl2 v13.4s, v20.8h, v22.8h
    ssubl v14.4s, v21.4h, v23.4h
    ssubl2 v15.4s, v21.8h, v23.8h
    saddl v16.4s, v24.4h, v26.4h
    saddl2 v17.4s, v24.8h, v26.8h
    saddl v18.4s, v25.4h, v27.4h
    saddl2 v19.4s, v25.8h, v27.8h
    ssubl v20.4s, v24.4h, v26.4h
    ssubl2 v21.4s, v24.8h, v26.8h
    ssubl v22.4s, v25.4h, v27.4h
    ssubl2 v23.4s, v25.8h, v27.8h
    saddl v24.4s, v28.4h, v30.4h
    saddl2 v25.4s, v28.8h, v30.8h
    saddl v26.4s, v29.4h, v31.4h
    saddl2 v27.4s, v29.8h, v31.8h
    sub sp, sp, #64
    st1 {v24.8h, v25.8h, v26.8h, v27.8h}, [sp]
    ssubl v24.4s, v28.4h, v30.4h
    ssubl2 v25.4s, v28.8h, v30.8h
    ssubl v26.4s, v29.4h, v31.4h
    ssubl2 v27.4s, v29.8h, v31.8h
    mov v28.16b, v24.16b
    mov v29.16b, v25.16b
    mov v30.16b, v26.16b
    mov v31.16b, v27.16b

    add v24.4s, v0.4s, v2.4s
    add v25.4s, v1.4s, v3.4s
    sub v26.4s, v0.4s, v2.4s
    sub v27.4s, v1.4s, v3.4s
    mov v0.16b, v24.16b
    mov v1.16b, v25.16b
    mov v2.16b, v26.16b
    mov v3.16b, v27.16b
    add v24.4s, v4.4s, v6.4s
    add v25.4s, v5.4s, v7.4s
    sub v26.4s, v4.4s, v6.4s
    sub v27.4s, v5.4s, v7.4s
    mov v4.16b, v24.16b
    mov v5.16b, v25.16b
    mov v6.16b, v26.16b
    mov v7.16b, v27.16b
    add v24.4s, v8.4s, v10.4s
    add v25.4s, v9.4s, v11.4s
    sub v26.4s, v8.4s, v10.4s
    sub v27.4s, v9.4s, v11.4s
    mov v8.16b, v24.16b
    mov v9.16b, v25.16b
    mov v10.16b, v26.16b
    mov v11.16b, v27.16b
    add v24.4s, v12.4s, v14.4s
    add v25.4s, v13.4s, v15.4s
    sub v26.4s, v12.4s, v14.4s
    sub v27.4s, v13.4s, v15.4s
    mov v12.16b, v24.16b
    mov v13.16b, v25.16b
    mov v14.16b, v26.16b
    mov v15.16b, v27.16b
    add v24.4s, v16.4s, v18.4s
    add v25.4s, v17.4s, v19.4s
    sub v26.4s, v16.4s, v18.4s
    sub v27.4s, v17.4s, v19.4s
    mov v16.16b, v24.16b
    mov v17.16b, v25.16b
    mov v18.16b, v26.16b
    mov v19.16b, v27.16b
    add v24.4s, v20.4s, v22.4s
    add v25.4s, v21.4s, v23.4s
    sub v26.4s, v20.4s, v22.4s
    sub v27.4s, v21.4s, v23.4s
    mov v20.16b, v24.16b
    mov v21.16b, v25.16b
    mov v22.16b, v26.16b
    mov v23.16b, v27.16b
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [sp]
    st1 {v0.8h, v1.8h, v2.8h, v3.8h}, [sp]
    add v0.4s, v24.4s, v26.4s
    add v1.4s, v25.4s, v27.4s
    sub v2.4s, v24.4s, v26.4s
    sub v3.4s, v25.4s, v27.4s
    mov v24.16b, v0.16b
    mov v25.16b, v1.16b
    mov v26.16b, v2.16b
    mov v27.16b, v3.16b
    add v0.4s, v28.4s, v30.4s
    add v1.4s, v29.4s, v31.4s
    sub v2.4s, v28.4s, v30.4s
    sub v3.4s, v29.4s, v31.4s
    mov v28.16b, v0.16b
    mov v29.16b, v1.16b
    mov v30.16b, v2.16b
    mov v31.16b, v3.16b
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [sp], #64

    abs v0.4s, v0.4s
    abs v1.4s, v1.4s
    abs v2.4s, v2.4s
    abs v3.4s, v3.4s
    abs v4.4s, v4.4s
    abs v5.4s, v5.4s
    abs v6.4s, v6.4s
    abs v7.4s, v7.4s
    abs v8.4s, v8.4s
    abs v9.4s, v9.4s
    abs v10.4s, v10.4s
    abs v11.4s, v11.4s
    abs v12.4s, v12.4s
    abs v13.4s, v13.4s
    abs v14.4s, v14.4s
    abs v15.4s, v15.4s
    abs v16.4s, v16.4s
    abs v17.4s, v17.4s
    abs v18.4s, v18.4s
    abs v19.4s, v19.4s
    abs v20.4s, v20.4s
    abs v21.4s, v21.4s
    abs v22.4s, v22.4s
    abs v23.4s, v23.4s
    abs v24.4s, v24.4s
    abs v25.4s, v25.4s
    abs v26.4s, v26.4s
    abs v27.4s, v27.4s
    abs v28.4s, v28.4s
    abs v29.4s, v29.4s
    abs v30.4s, v30.4s
    abs v31.4s, v31.4s

    add v0.4s, v0.4s, v1.4s
    add v2.4s, v2.4s, v3.4s
    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v8.4s, v8.4s, v9.4s
    add v10.4s, v10.4s, v11.4s
    add v12.4s, v12.4s, v13.4s
    add v14.4s, v14.4s, v15.4s
    add v16.4s, v16.4s, v17.4s
    add v18.4s, v18.4s, v19.4s
    add v20.4s, v20.4s, v21.4s
    add v22.4s, v22.4s, v23.4s
    add v24.4s, v24.4s, v25.4s
    add v26.4s, v26.4s, v27.4s
    add v28.4s, v28.4s, v29.4s
    add v30.4s, v30.4s, v31.4s
    add v0.4s, v0.4s, v2.4s
    add v1.4s, v4.4s, v6.4s
    add v2.4s, v8.4s, v10.4s
    add v3.4s, v12.4s, v14.4s
    add v4.4s, v16.4s, v18.4s
    add v5.4s, v20.4s, v22.4s
    add v6.4s, v24.4s, v26.4s
    add v7.4s, v28.4s, v30.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v2.4s, v4.4s, v5.4s
    add v3.4s, v6.4s, v7.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v0.4s, v0.4s, v1.4s
    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x1, #128
    ucvtf d1, x1
    fsqrt d1, d1
    mov x0, #0
    umov w0, v0.s[0]
    ucvtf d0, x0
    fdiv d0, d0, d1
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0

    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64
    ret

//void uavs3e_had_16x8_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_16x8_arm64
    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    ld1 {v0.8b, v1.8b}, [x0], x1
    ld1 {v2.8b, v3.8b}, [x0], x1
    ld1 {v4.8b, v5.8b}, [x0], x1
    ld1 {v6.8b, v7.8b}, [x0], x1
    ld1 {v8.8b, v9.8b}, [x0], x1
    ld1 {v10.8b, v11.8b}, [x0], x1
    ld1 {v12.8b, v13.8b}, [x0], x1
    ld1 {v14.8b, v15.8b}, [x0], x1

    ld1 {v16.8b, v17.8b}, [x2], x3
    ld1 {v18.8b, v19.8b}, [x2], x3
    ld1 {v20.8b, v21.8b}, [x2], x3
    ld1 {v22.8b, v23.8b}, [x2], x3
    ld1 {v24.8b, v25.8b}, [x2], x3
    ld1 {v26.8b, v27.8b}, [x2], x3
    ld1 {v28.8b, v29.8b}, [x2], x3
    ld1 {v30.8b, v31.8b}, [x2], x3

    uxtl v0.8h, v0.8b
    uxtl v1.8h, v1.8b
    uxtl v2.8h, v2.8b
    uxtl v3.8h, v3.8b
    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b
    uxtl v8.8h, v8.8b
    uxtl v9.8h, v9.8b
    uxtl v10.8h, v10.8b
    uxtl v11.8h, v11.8b
    uxtl v12.8h, v12.8b
    uxtl v13.8h, v13.8b
    uxtl v14.8h, v14.8b
    uxtl v15.8h, v15.8b
    
    uxtl v16.8h, v16.8b
    uxtl v17.8h, v17.8b
    uxtl v18.8h, v18.8b
    uxtl v19.8h, v19.8b
    uxtl v20.8h, v20.8b
    uxtl v21.8h, v21.8b
    uxtl v22.8h, v22.8b
    uxtl v23.8h, v23.8b
    uxtl v24.8h, v24.8b
    uxtl v25.8h, v25.8b
    uxtl v26.8h, v26.8b
    uxtl v27.8h, v27.8b
    uxtl v28.8h, v28.8b
    uxtl v29.8h, v29.8b
    uxtl v30.8h, v30.8b
    uxtl v31.8h, v31.8b

    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h
    sub v8.8h, v8.8h, v24.8h
    sub v9.8h, v9.8h, v25.8h
    sub v10.8h, v10.8h, v26.8h
    sub v11.8h, v11.8h, v27.8h
    sub v12.8h, v12.8h, v28.8h
    sub v13.8h, v13.8h, v29.8h
    sub v14.8h, v14.8h, v30.8h
    sub v15.8h, v15.8h, v31.8h

    uzp1 v16.8h, v0.8h, v1.8h       //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v17.8h, v0.8h, v1.8h       //d1, d3, d5, d7, d9, d11, d13, d15
    uzp1 v18.8h, v2.8h, v3.8h       //d16, d18, d20, d22
    uzp2 v19.8h, v2.8h, v3.8h       //d17, d19, d21, d23
    uzp1 v20.8h, v4.8h, v5.8h
    uzp2 v21.8h, v4.8h, v5.8h
    uzp1 v22.8h, v6.8h, v7.8h
    uzp2 v23.8h, v6.8h, v7.8h
    uzp1 v24.8h, v8.8h, v9.8h
    uzp2 v25.8h, v8.8h, v9.8h
    uzp1 v26.8h, v10.8h, v11.8h
    uzp2 v27.8h, v10.8h, v11.8h
    uzp1 v28.8h, v12.8h, v13.8h
    uzp2 v29.8h, v12.8h, v13.8h
    uzp1 v30.8h, v14.8h, v15.8h
    uzp2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1, d2 + d3
    sub v1.8h, v16.8h, v17.8h       //d0 - d1, d2 - d3
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1
    trn2 v17.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3
    trn1 v18.8h, v2.8h, v3.8h
    trn2 v19.8h, v2.8h, v3.8h
    trn1 v20.8h, v4.8h, v5.8h
    trn2 v21.8h, v4.8h, v5.8h
    trn1 v22.8h, v6.8h, v7.8h
    trn2 v23.8h, v6.8h, v7.8h
    trn1 v24.8h, v8.8h, v9.8h
    trn2 v25.8h, v8.8h, v9.8h
    trn1 v26.8h, v10.8h, v11.8h
    trn2 v27.8h, v10.8h, v11.8h
    trn1 v28.8h, v12.8h, v13.8h
    trn2 v29.8h, v12.8h, v13.8h
    trn1 v30.8h, v14.8h, v15.8h
    trn2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.4s, v0.4s, v1.4s       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v17.4s, v0.4s, v1.4s       //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v18.4s, v2.4s, v3.4s
    trn2 v19.4s, v2.4s, v3.4s
    trn1 v20.4s, v4.4s, v5.4s
    trn2 v21.4s, v4.4s, v5.4s
    trn1 v22.4s, v6.4s, v7.4s
    trn2 v23.4s, v6.4s, v7.4s
    trn1 v24.4s, v8.4s, v9.4s
    trn2 v25.4s, v8.4s, v9.4s
    trn1 v26.4s, v10.4s, v11.4s
    trn2 v27.4s, v10.4s, v11.4s
    trn1 v28.4s, v12.4s, v13.4s
    trn2 v29.4s, v12.4s, v13.4s
    trn1 v30.4s, v14.4s, v15.4s
    trn2 v31.4s, v14.4s, v15.4s

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.2d, v0.2d, v1.2d       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7
    trn2 v17.2d, v0.2d, v1.2d       //d8 + d9 + d10 + d11 + d12 + d13 + d14 + d15
    trn1 v18.2d, v2.2d, v3.2d
    trn2 v19.2d, v2.2d, v3.2d
    trn1 v20.2d, v4.2d, v5.2d
    trn2 v21.2d, v4.2d, v5.2d
    trn1 v22.2d, v6.2d, v7.2d
    trn2 v23.2d, v6.2d, v7.2d
    trn1 v24.2d, v8.2d, v9.2d
    trn2 v25.2d, v8.2d, v9.2d
    trn1 v26.2d, v10.2d, v11.2d
    trn2 v27.2d, v10.2d, v11.2d
    trn1 v28.2d, v12.2d, v13.2d
    trn2 v29.2d, v12.2d, v13.2d
    trn1 v30.2d, v14.2d, v15.2d
    trn2 v31.2d, v14.2d, v15.2d

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10 + d11 + d12 + d13 + d14 + d15
    sub v1.8h, v16.8h, v17.8h
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    add v16.8h, v0.8h, v8.8h
    add v17.8h, v1.8h, v9.8h
    add v18.8h, v2.8h, v10.8h
    add v19.8h, v3.8h, v11.8h
    add v20.8h, v4.8h, v12.8h
    add v21.8h, v5.8h, v13.8h
    add v22.8h, v6.8h, v14.8h
    add v23.8h, v7.8h, v15.8h
    sub v24.8h, v0.8h, v8.8h
    sub v25.8h, v1.8h, v9.8h
    sub v26.8h, v2.8h, v10.8h
    sub v27.8h, v3.8h, v11.8h
    sub v28.8h, v4.8h, v12.8h
    sub v29.8h, v5.8h, v13.8h
    sub v30.8h, v6.8h, v14.8h
    sub v31.8h, v7.8h, v15.8h

    saddl v0.4s, v16.4h, v20.4h
    saddl2 v1.4s, v16.8h, v20.8h
    saddl v2.4s, v17.4h, v21.4h
    saddl2 v3.4s, v17.8h, v21.8h
    saddl v4.4s, v18.4h, v22.4h
    saddl2 v5.4s, v18.8h, v22.8h
    saddl v6.4s, v19.4h, v23.4h
    saddl2 v7.4s, v19.8h, v23.8h
    ssubl v8.4s, v16.4h, v20.4h
    ssubl2 v9.4s, v16.8h, v20.8h
    ssubl v10.4s, v17.4h, v21.4h
    ssubl2 v11.4s, v17.8h, v21.8h
    ssubl v12.4s, v18.4h, v22.4h
    ssubl2 v13.4s, v18.8h, v22.8h
    ssubl v14.4s, v19.4h, v23.4h
    ssubl2 v15.4s, v19.8h, v23.8h
    saddl v16.4s, v24.4h, v28.4h
    saddl2 v17.4s, v24.8h, v28.8h
    saddl v18.4s, v25.4h, v29.4h
    saddl2 v19.4s, v25.8h, v29.8h
    saddl v20.4s, v26.4h, v30.4h
    saddl2 v21.4s, v26.8h, v30.8h
    saddl v22.4s, v27.4h, v31.4h
    saddl2 v23.4s, v27.8h, v31.8h
    sub sp, sp, #64
    st1 {v20.4s, v21.4s, v22.4s, v23.4s}, [sp]
    ssubl v20.4s, v24.4h, v28.4h
    ssubl2 v21.4s, v24.8h, v28.8h
    ssubl v22.4s, v25.4h, v29.4h
    ssubl2 v23.4s, v25.8h, v29.8h
    ssubl v24.4s, v26.4h, v30.4h
    ssubl2 v25.4s, v26.8h, v30.8h
    ssubl v26.4s, v27.4h, v31.4h
    ssubl2 v27.4s, v27.8h, v31.8h
    mov v31.16b, v27.16b
    mov v30.16b, v26.16b
    mov v29.16b, v25.16b
    mov v28.16b, v24.16b
    mov v27.16b, v23.16b
    mov v26.16b, v22.16b
    mov v25.16b, v21.16b
    mov v24.16b, v20.16b

    sub sp, sp, #64
    st1 {v24.4s, v25.4s, v26.4s, v27.4s}, [sp]
    add v20.4s, v0.4s, v4.4s
    add v21.4s, v1.4s, v5.4s
    add v22.4s, v2.4s, v6.4s
    add v23.4s, v3.4s, v7.4s
    sub v24.4s, v0.4s, v4.4s
    sub v25.4s, v1.4s, v5.4s
    sub v26.4s, v2.4s, v6.4s
    sub v27.4s, v3.4s, v7.4s
    mov v0.16b, v20.16b
    mov v1.16b, v21.16b
    mov v2.16b, v22.16b
    mov v3.16b, v23.16b
    mov v4.16b, v24.16b
    mov v5.16b, v25.16b
    mov v6.16b, v26.16b
    mov v7.16b, v27.16b
    ld1 {v24.4s, v25.4s, v26.4s, v27.4s}, [sp], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [sp]
    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [sp]
    sub sp, sp, #64
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [sp]
    add v0.4s, v8.4s, v12.4s
    add v1.4s, v9.4s, v13.4s
    add v2.4s, v10.4s, v14.4s
    add v3.4s, v11.4s, v15.4s
    sub v4.4s, v8.4s, v12.4s
    sub v5.4s, v9.4s, v13.4s
    sub v6.4s, v10.4s, v14.4s
    sub v7.4s, v11.4s, v15.4s
    mov v8.16b, v0.16b
    mov v9.16b, v1.16b
    mov v10.16b, v2.16b
    mov v11.16b, v3.16b
    mov v12.16b, v4.16b
    mov v13.16b, v5.16b
    mov v14.16b, v6.16b
    mov v15.16b, v7.16b
    add v0.4s, v16.4s, v20.4s
    add v1.4s, v17.4s, v21.4s
    add v2.4s, v18.4s, v22.4s
    add v3.4s, v19.4s, v23.4s
    sub v4.4s, v16.4s, v20.4s
    sub v5.4s, v17.4s, v21.4s
    sub v6.4s, v18.4s, v22.4s
    sub v7.4s, v19.4s, v23.4s
    mov v16.16b, v0.16b
    mov v17.16b, v1.16b
    mov v18.16b, v2.16b
    mov v19.16b, v3.16b
    mov v20.16b, v4.16b
    mov v21.16b, v5.16b
    mov v22.16b, v6.16b
    mov v23.16b, v7.16b
    add v0.4s, v24.4s, v28.4s
    add v1.4s, v25.4s, v29.4s
    add v2.4s, v26.4s, v30.4s
    add v3.4s, v27.4s, v31.4s
    sub v4.4s, v24.4s, v28.4s
    sub v5.4s, v25.4s, v29.4s
    sub v6.4s, v26.4s, v30.4s
    sub v7.4s, v27.4s, v31.4s
    mov v24.16b, v0.16b
    mov v25.16b, v1.16b
    mov v26.16b, v2.16b
    mov v27.16b, v3.16b
    mov v28.16b, v4.16b
    mov v29.16b, v5.16b
    mov v30.16b, v6.16b
    mov v31.16b, v7.16b
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [sp], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [sp], #64

    abs v0.4s, v0.4s
    abs v1.4s, v1.4s
    abs v2.4s, v2.4s
    abs v3.4s, v3.4s
    abs v4.4s, v4.4s
    abs v5.4s, v5.4s
    abs v6.4s, v6.4s
    abs v7.4s, v7.4s
    abs v8.4s, v8.4s
    abs v9.4s, v9.4s
    abs v10.4s, v10.4s
    abs v11.4s, v11.4s
    abs v12.4s, v12.4s
    abs v13.4s, v13.4s
    abs v14.4s, v14.4s
    abs v15.4s, v15.4s
    abs v16.4s, v16.4s
    abs v17.4s, v17.4s
    abs v18.4s, v18.4s
    abs v19.4s, v19.4s
    abs v20.4s, v20.4s
    abs v21.4s, v21.4s
    abs v22.4s, v22.4s
    abs v23.4s, v23.4s
    abs v24.4s, v24.4s
    abs v25.4s, v25.4s
    abs v26.4s, v26.4s
    abs v27.4s, v27.4s
    abs v28.4s, v28.4s
    abs v29.4s, v29.4s
    abs v30.4s, v30.4s
    abs v31.4s, v31.4s

    add v0.4s, v0.4s, v1.4s
    add v2.4s, v2.4s, v3.4s
    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v8.4s, v8.4s, v9.4s
    add v10.4s, v10.4s, v11.4s
    add v12.4s, v12.4s, v13.4s
    add v14.4s, v14.4s, v15.4s
    add v16.4s, v16.4s, v17.4s
    add v18.4s, v18.4s, v19.4s
    add v20.4s, v20.4s, v21.4s
    add v22.4s, v22.4s, v23.4s
    add v24.4s, v24.4s, v25.4s
    add v26.4s, v26.4s, v27.4s
    add v28.4s, v28.4s, v29.4s
    add v30.4s, v30.4s, v31.4s
    add v0.4s, v0.4s, v2.4s
    add v1.4s, v4.4s, v6.4s
    add v2.4s, v8.4s, v10.4s
    add v3.4s, v12.4s, v14.4s
    add v4.4s, v16.4s, v18.4s
    add v5.4s, v20.4s, v22.4s
    add v6.4s, v24.4s, v26.4s
    add v7.4s, v28.4s, v30.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v2.4s, v4.4s, v5.4s
    add v3.4s, v6.4s, v7.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v0.4s, v0.4s, v1.4s
    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x1, #128
    ucvtf d1, x1
    fsqrt d1, d1
    mov x0, #0
    umov w0, v0.s[0]
    ucvtf d0, x0
    fdiv d0, d0, d1
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0

    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64
    ret

#else
//void uavs3e_get_sad_4_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_4_arm64
    lsl w1, w1, #1
    lsl w3, w3, #1

    movi v16.16b, #0
get_sad_4_y:
    //load org
    ld1 {v0.d}[0], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v1.d}[0], [x0], x1
    ld1 {v1.d}[1], [x0], x1
    //load pred
    ld1 {v2.d}[0], [x2], x3
    ld1 {v2.d}[1], [x2], x3
    ld1 {v3.d}[0], [x2], x3
    ld1 {v3.d}[1], [x2], x3
    uabd v0.8h, v0.8h, v2.8h
    uabd v1.8h, v1.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    add v16.8h, v16.8h, v0.8h
    subs w4, w4, #4
    bgt get_sad_4_y

    uaddlp v16.4s, v16.8h
    addp v16.4s, v16.4s, v16.4s
    addp v16.4s, v16.4s, v16.4s
    mov x0, #0
    umov w0, v16.s[0]

    ret

//void uavs3e_get_sad_8_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    movi v16.16b, #0

get_sad_8_y:
    //load org
    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1
    //load pred
    ld1 {v4.8h}, [x2], x3
    ld1 {v5.8h}, [x2], x3
    ld1 {v6.8h}, [x2], x3
    ld1 {v7.8h}, [x2], x3
    uabd v0.8h, v0.8h, v4.8h
    uabd v1.8h, v1.8h, v5.8h
    uabd v2.8h, v2.8h, v6.8h
    uabd v3.8h, v3.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v2.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v2.8h
    add v16.8h, v16.8h, v0.8h

    subs w4, w4, #4
    bgt get_sad_8_y
    uaddlp v16.4s, v16.8h
    addp v16.4s, v16.4s, v16.4s
    addp v16.4s, v16.4s, v16.4s

    mov x0, #0
    umov w0, v16.s[0]

ret

//void uavs3e_get_sad_16_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_16_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    movi v16.16b, #0
    movi v17.16b, #0
    movi v18.16b, #0
    movi v19.16b, #0

get_sad_16_y:
    ld1 {v0.8h, v1.8h}, [x0], x1
    ld1 {v2.8h, v3.8h}, [x0], x1
    //load pred
    ld1 {v4.8h, v5.8h}, [x2], x3
    ld1 {v6.8h, v7.8h}, [x2], x3

    uabd v0.8h, v0.8h, v4.8h
    uabd v1.8h, v1.8h, v5.8h
    uabd v2.8h, v2.8h, v6.8h
    uabd v3.8h, v3.8h, v7.8h
    add v16.8h, v16.8h, v0.8h
    add v17.8h, v17.8h, v1.8h
    add v18.8h, v18.8h, v2.8h
    add v19.8h, v19.8h, v3.8h

    subs w4, w4, #2
    bgt get_sad_16_y

    uaddlp v16.4s, v16.8h
    uaddlp v17.4s, v17.8h
    uaddlp v18.4s, v18.8h
    uaddlp v19.4s, v19.8h
    addp v16.4s, v16.4s, v17.4s
    addp v18.4s, v18.4s, v19.4s
    addp v16.4s, v16.4s, v18.4s
    addp v16.4s, v16.4s, v16.4s
    addp v16.4s, v16.4s, v16.4s

    mov x0, #0
    umov w0, v16.s[0]

    ret

//void uavs3e_get_sad_32_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_32_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0
    movi v28.16b, #0
    movi v29.16b, #0
    movi v30.16b, #0
    movi v31.16b, #0

get_sad_32_y:
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x0], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x2], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x2], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h
    add v24.8h, v24.8h, v0.8h
    add v25.8h, v25.8h, v1.8h
    add v26.8h, v26.8h, v2.8h
    add v27.8h, v27.8h, v3.8h
    add v28.8h, v28.8h, v4.8h
    add v29.8h, v29.8h, v5.8h
    add v30.8h, v30.8h, v6.8h
    add v31.8h, v31.8h, v7.8h

    subs w4, w4, #2
    bgt get_sad_32_y

    uaddlp v24.4s, v24.8h
    uaddlp v25.4s, v25.8h
    uaddlp v26.4s, v26.8h
    uaddlp v27.4s, v27.8h
    uaddlp v28.4s, v28.8h
    uaddlp v29.4s, v29.8h
    uaddlp v30.4s, v30.8h
    uaddlp v31.4s, v31.8h
    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v27.4s
    addp v28.4s, v28.4s, v29.4s
    addp v30.4s, v30.4s, v31.4s
    addp v24.4s, v24.4s, v26.4s
    addp v28.4s, v28.4s, v30.4s
    addp v24.4s, v24.4s, v28.4s
    addp v24.4s, v24.4s, v24.4s
    addp v24.4s, v24.4s, v24.4s

    mov x0, #0
    umov w0, v24.s[0]

    ret

//void uavs3e_get_sad_64_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_64_arm64
    lsl x1, x1, #1      //i_org
    lsl x3, x3, #1
    lsl x7, x1, #1      //2 * i_org
    lsl x8, x3, #1
    movi v24.16b, #0
    movi v25.16b, #0

get_sad_64_y:
    mov x5, x0
    mov x6, x2

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v24.4s, v24.4s, v0.4s

    add x5, x0, #64
    add x6, x2, #64

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v25.4s, v25.4s, v0.4s

    add x0, x0, x7
    add x2, x2, x8

    subs w4, w4, #2
    bgt get_sad_64_y
    add v24.4s, v24.4s, v25.4s
    addp v24.4s, v24.4s, v24.4s
    addp v24.4s, v24.4s, v24.4s

    mov x0, #0
    umov w0, v24.s[0]

    ret

//void uavs3e_get_sad_128_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_sad_128_arm64
    lsl x1, x1, #1      //i_org
    lsl x3, x3, #1      //i_pred
    lsl x7, x1, #1      //2 * i_org
    lsl x8, x3, #1      //i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_128_y:
    mov x5, x0
    mov x6, x2

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v24.4s, v24.4s, v0.4s

    add x5, x0, #64
    add x6, x2, #64

    //load p_org + 64
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v25.4s, v25.4s, v0.4s

    add x5, x0, #128
    add x6, x2, #128

    //load p_org + 128
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v26.4s, v26.4s, v0.4s

    add x5, x0, #192
    add x6, x2, #192

    //load p_org + 196
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x5], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x5], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x6], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x6], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v2.8h, v4.8h, v5.8h
    add v3.8h, v6.8h, v7.8h
    add v0.8h, v0.8h, v1.8h
    add v1.8h, v2.8h, v3.8h
    add v0.8h, v0.8h, v1.8h
    uaddlp v0.4s, v0.8h
    add v27.4s, v27.4s, v0.4s

    add x0, x0, x7
    add x2, x2, x8

    subs w4, w4, #2
    bgt get_sad_128_y

    add v24.4s, v24.4s, v25.4s
    add v26.4s, v26.4s, v27.4s
    add v24.4s, v24.4s, v26.4s
    addp v24.4s, v24.4s, v24.4s
    addp v24.4s, v24.4s, v24.4s

    mov x0, #0
    umov w0, v24.s[0]

    ret

//void uavs3e_get_ssd_4_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_4_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    movi v16.16b, #0

get_ssd_4_y:
    //load p_org
    ld1 {v0.4h}, [x0], x1
    ld1 {v1.4h}, [x0], x1
    ld1 {v2.4h}, [x0], x1
    ld1 {v3.4h}, [x0], x1
    //load pred
    ld1 {v4.4h}, [x2], x3
    ld1 {v5.4h}, [x2], x3
    ld1 {v6.4h}, [x2], x3
    ld1 {v7.4h}, [x2], x3

    uabd v0.4h, v0.4h, v4.4h
    uabd v1.4h, v1.4h, v5.4h
    uabd v2.4h, v2.4h, v6.4h
    uabd v3.4h, v3.4h, v7.4h

    umlal v16.4s, v0.4h, v0.4h
    umlal v16.4s, v1.4h, v1.4h
    umlal v16.4s, v2.4h, v2.4h
    umlal v16.4s, v3.4h, v3.4h

    subs w4, w4, #4
    bgt get_ssd_4_y

    uaddlp v16.2d, v16.4s
    addp v16.2d, v16.2d, v16.2d
    mov x0, v16.d[0]

ret

//void uavs3e_get_ssd_8_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    movi v16.16b, #0

get_ssd_8_y:
    //load p_org
    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1
    //load pred
    ld1 {v4.8h}, [x2], x3
    ld1 {v5.8h}, [x2], x3
    ld1 {v6.8h}, [x2], x3
    ld1 {v7.8h}, [x2], x3

    uabd v0.8h, v0.8h, v4.8h
    uabd v1.8h, v1.8h, v5.8h
    uabd v2.8h, v2.8h, v6.8h
    uabd v3.8h, v3.8h, v7.8h

    umlal v16.4s, v0.4h, v0.4h
    umlal2 v16.4s, v0.8h, v0.8h
    umlal v16.4s, v1.4h, v1.4h
    umlal2 v16.4s, v1.8h, v1.8h
    umlal v16.4s, v2.4h, v2.4h
    umlal2 v16.4s, v2.8h, v2.8h
    umlal v16.4s, v3.4h, v3.4h
    umlal2 v16.4s, v3.8h, v3.8h

    subs w4, w4, #4
    bgt get_ssd_8_y

    uaddlp v16.2d, v16.4s
    addp v16.2d, v16.2d, v16.2d
    mov x0, v16.d[0]

ret

//void uavs3e_get_ssd_16_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_16_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    movi v24.16b, #0

get_ssd_16_y:
    //load p_org
    ld1 {v0.8h, v1.8h}, [x0], x1
    ld1 {v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h}, [x0], x1
    ld1 {v6.8h, v7.8h}, [x0], x1
    //load pred
    ld1 {v16.8h, v17.8h}, [x2], x3
    ld1 {v18.8h, v19.8h}, [x2], x3
    ld1 {v20.8h, v21.8h}, [x2], x3
    ld1 {v22.8h, v23.8h}, [x2], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    subs w4, w4, #4
    bgt get_ssd_16_y

    uaddlp v24.2d, v24.4s
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_ssd_32_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_32_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    movi v24.16b, #0

get_ssd_32_y:
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x0], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x2], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x2], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    subs w4, w4, #2
    bgt get_ssd_32_y

    uaddlp v24.2d, v24.4s
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_ssd_64_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_64_arm64
    lsl x1, x1, #1      //i_org
    lsl x3, x3, #1      //i_pred
    lsl x5, x1, #1      //2 * i_org
    lsl x6, x3, #1      //2 * i_pred
    movi v24.16b, #0


get_ssd_64_y:
    mov x7, x0
    mov x8, x2

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x7], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x7], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x8], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x8], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x7, x0, #64
    add x8, x2, #64
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x7], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x7], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x8], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x8], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x0, x0, x5
    add x2, x2, x6

    subs w4, w4, #2
    bgt get_ssd_64_y

    uaddlp v24.2d, v24.4s
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_ssd_128_arm64(pel *p_org, int i_org, pel *pred, int i_pred, int height)
//*p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, height->x4
function uavs3e_get_ssd_128_arm64
    lsl x1, x1, #1      //i_org
    lsl x3, x3, #1      //i_pred
    lsl x5, x1, #1      //2 * i_org
    lsl x6, x3, #1      //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0


get_ssd_128_y:
    mov x7, x0
    mov x8, x2

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x7], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x7], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x8], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x8], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x7, x0, #64
    add x8, x2, #64
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x7], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x7], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x8], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x8], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v24.4s, v0.4h, v0.4h
    umlal2 v24.4s, v0.8h, v0.8h
    umlal v24.4s, v1.4h, v1.4h
    umlal2 v24.4s, v1.8h, v1.8h
    umlal v24.4s, v2.4h, v2.4h
    umlal2 v24.4s, v2.8h, v2.8h
    umlal v24.4s, v3.4h, v3.4h
    umlal2 v24.4s, v3.8h, v3.8h
    umlal v24.4s, v4.4h, v4.4h
    umlal2 v24.4s, v4.8h, v4.8h
    umlal v24.4s, v5.4h, v5.4h
    umlal2 v24.4s, v5.8h, v5.8h
    umlal v24.4s, v6.4h, v6.4h
    umlal2 v24.4s, v6.8h, v6.8h
    umlal v24.4s, v7.4h, v7.4h
    umlal2 v24.4s, v7.8h, v7.8h

    add x7, x0, #128
    add x8, x2, #128

    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x7], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x7], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x8], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x8], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v25.4s, v0.4h, v0.4h
    umlal2 v25.4s, v0.8h, v0.8h
    umlal v25.4s, v1.4h, v1.4h
    umlal2 v25.4s, v1.8h, v1.8h
    umlal v25.4s, v2.4h, v2.4h
    umlal2 v25.4s, v2.8h, v2.8h
    umlal v25.4s, v3.4h, v3.4h
    umlal2 v25.4s, v3.8h, v3.8h
    umlal v25.4s, v4.4h, v4.4h
    umlal2 v25.4s, v4.8h, v4.8h
    umlal v25.4s, v5.4h, v5.4h
    umlal2 v25.4s, v5.8h, v5.8h
    umlal v25.4s, v6.4h, v6.4h
    umlal2 v25.4s, v6.8h, v6.8h
    umlal v25.4s, v7.4h, v7.4h
    umlal2 v25.4s, v7.8h, v7.8h

    add x7, x0, #192
    add x8, x2, #192
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x7], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x7], x1
    //load pred
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x8], x3
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x8], x3

    uabd v0.8h, v0.8h, v16.8h
    uabd v1.8h, v1.8h, v17.8h
    uabd v2.8h, v2.8h, v18.8h
    uabd v3.8h, v3.8h, v19.8h
    uabd v4.8h, v4.8h, v20.8h
    uabd v5.8h, v5.8h, v21.8h
    uabd v6.8h, v6.8h, v22.8h
    uabd v7.8h, v7.8h, v23.8h

    umlal v25.4s, v0.4h, v0.4h
    umlal2 v25.4s, v0.8h, v0.8h
    umlal v25.4s, v1.4h, v1.4h
    umlal2 v25.4s, v1.8h, v1.8h
    umlal v25.4s, v2.4h, v2.4h
    umlal2 v25.4s, v2.8h, v2.8h
    umlal v25.4s, v3.4h, v3.4h
    umlal2 v25.4s, v3.8h, v3.8h
    umlal v25.4s, v4.4h, v4.4h
    umlal2 v25.4s, v4.8h, v4.8h
    umlal v25.4s, v5.4h, v5.4h
    umlal2 v25.4s, v5.8h, v5.8h
    umlal v25.4s, v6.4h, v6.4h
    umlal2 v25.4s, v6.8h, v6.8h
    umlal v25.4s, v7.4h, v7.4h
    umlal2 v25.4s, v7.8h, v7.8h

    add x0, x0, x5
    add x2, x2, x6

    subs w4, w4, #2
    bgt get_ssd_128_y

    uaddl v26.2d, v24.2s, v25.2s
    uaddl2 v27.2d, v24.4s, v25.4s
    add v24.2d, v26.2d, v27.2d
    addp v24.2d, v24.2d, v24.2d
    mov x0, v24.d[0]

ret

//void uavs3e_get_sad_x3_4_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_4_arm64
    lsl x1, x1, #1
    lsl x5, x5, #1
    movi v16.16b, #0
    movi v17.16b, #0
    movi v18.16b, #0

get_sad_x3_4_y:
    //load p_org
    ld1 {v0.d}[0], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v1.d}[0], [x0], x1
    ld1 {v1.d}[1], [x0], x1
    //load pred
    ld1 {v2.d}[0], [x2], x5
    ld1 {v2.d}[1], [x2], x5
    ld1 {v3.d}[0], [x2], x5
    ld1 {v3.d}[1], [x2], x5
    ld1 {v4.d}[0], [x3], x5
    ld1 {v4.d}[1], [x3], x5
    ld1 {v5.d}[0], [x3], x5
    ld1 {v5.d}[1], [x3], x5
    ld1 {v6.d}[0], [x4], x5
    ld1 {v6.d}[1], [x4], x5
    ld1 {v7.d}[0], [x4], x5
    ld1 {v7.d}[1], [x4], x5
    //abs
    uabd v2.8h, v0.8h, v2.8h
    uabd v3.8h, v1.8h, v3.8h
    uabd v4.8h, v0.8h, v4.8h
    uabd v5.8h, v1.8h, v5.8h
    uabd v6.8h, v0.8h, v6.8h
    uabd v7.8h, v1.8h, v7.8h

    add v2.8h, v2.8h, v3.8h
    add v4.8h, v4.8h, v5.8h
    add v6.8h, v6.8h, v7.8h
    add v16.8h, v16.8h, v2.8h
    add v17.8h, v17.8h, v4.8h
    add v18.8h, v18.8h, v6.8h
    subs w7, w7, #4
    bgt get_sad_x3_4_y

    uaddlp v16.4s, v16.8h
    uaddlp v17.4s, v17.8h
    uaddlp v18.4s, v18.8h
    addp v16.4s, v16.4s, v17.4s
    addp v18.4s, v18.4s, v18.4s
    addp v16.4s, v16.4s, v18.4s

    st1 {v16.d}[0], [x6], #8
    st1 {v16.s}[2], [x6]

    ret

//void uavs3e_get_sad_x3_8_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_8_arm64
    lsl x1, x1, #1
    lsl x5, x5, #1
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_8_y:
    //load p_org
    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1
    //load pred
    ld1 {v4.8h}, [x2], x5
    ld1 {v5.8h}, [x2], x5
    ld1 {v6.8h}, [x2], x5
    ld1 {v7.8h}, [x2], x5
    ld1 {v16.8h}, [x3], x5
    ld1 {v17.8h}, [x3], x5
    ld1 {v18.8h}, [x3], x5
    ld1 {v19.8h}, [x3], x5
    ld1 {v20.8h}, [x4], x5
    ld1 {v21.8h}, [x4], x5
    ld1 {v22.8h}, [x4], x5
    ld1 {v23.8h}, [x4], x5
    //abs
    uabd v4.8h, v0.8h, v4.8h
    uabd v5.8h, v1.8h, v5.8h
    uabd v6.8h, v2.8h, v6.8h
    uabd v7.8h, v3.8h, v7.8h
    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v0.8h, v20.8h
    uabd v21.8h, v1.8h, v21.8h
    uabd v22.8h, v2.8h, v22.8h
    uabd v23.8h, v3.8h, v23.8h

    add v4.8h, v4.8h, v5.8h
    add v5.8h, v6.8h, v7.8h
    add v4.8h, v4.8h, v5.8h
    add v24.8h, v24.8h, v4.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    add v25.8h, v25.8h, v16.8h
    add v20.8h, v20.8h, v21.8h
    add v21.8h, v22.8h, v23.8h
    add v20.8h, v20.8h, v21.8h
    add v26.8h, v26.8h, v20.8h

    subs w7, w7, #4
    bgt get_sad_x3_8_y

    uaddlp v24.4s, v24.8h
    uaddlp v25.4s, v25.8h
    uaddlp v26.4s, v26.8h
    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]

ret

//void uavs3e_get_sad_x3_16_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_16_arm64
    lsl x1, x1, #1
    lsl x5, x5, #1
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_16_y:
    //load p_org
    ld1 {v0.8h, v1.8h}, [x0], x1
    ld1 {v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h}, [x0], x1
    ld1 {v6.8h, v7.8h}, [x0], x1
    //load pred0
    ld1 {v16.8h, v17.8h}, [x2], x5
    ld1 {v18.8h, v19.8h}, [x2], x5
    ld1 {v20.8h, v21.8h}, [x2], x5
    ld1 {v22.8h, v23.8h}, [x2], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h}, [x3], x5
    ld1 {v18.8h, v19.8h}, [x3], x5
    ld1 {v20.8h, v21.8h}, [x3], x5
    ld1 {v22.8h, v23.8h}, [x3], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h}, [x4], x5
    ld1 {v18.8h, v19.8h}, [x4], x5
    ld1 {v20.8h, v21.8h}, [x4], x5
    ld1 {v22.8h, v23.8h}, [x4], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    subs w7, w7, #4
    bgt get_sad_x3_16_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
ret

//void uavs3e_get_sad_x3_32_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_32_arm64
    lsl x1, x1, #1
    lsl x5, x5, #1
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_32_y:
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x0], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x2], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x2], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x3], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x3], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x4], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x4], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    subs w7, w7, #2
    bgt get_sad_x3_32_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
ret


//void uavs3e_get_sad_x3_64_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_64_arm64
    lsl x1, x1, #1
    lsl x5, x5, #1
    lsl x12, x1, #1     //2 * i_org
    lsl x13, x5, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_64_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #64
    add x9, x2, #64
    add x10, x3, #64
    add x11, x4, #64
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x0, x0, x12
    add x2, x2, x13
    add x3, x3, x13
    add x4, x4, x13

    subs w7, w7, #2
    bgt get_sad_x3_64_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
    ret

//void uavs3e_get_sad_x3_128_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, pred0->x2, pred1->x3, pred2->x4, i_pred->x5, sad[3]->x6, height->x7
function uavs3e_get_sad_x3_128_arm64
    lsl x1, x1, #1
    lsl x5, x5, #1
    lsl x12, x1, #1     //2 * i_org
    lsl x13, x5, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0

get_sad_x3_128_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #64
    add x9, x2, #64
    add x10, x3, #64
    add x11, x4, #64
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #128
    add x9, x2, #128
    add x10, x3, #128
    add x11, x4, #128
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x8, x0, #192
    add x9, x2, #192
    add x10, x3, #192
    add x11, x4, #192
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x5
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x5

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    add x0, x0, x12
    add x2, x2, x13
    add x3, x3, x13
    add x4, x4, x13

    subs w7, w7, #2
    bgt get_sad_x3_128_y

    addp v24.4s, v24.4s, v25.4s
    addp v26.4s, v26.4s, v26.4s
    addp v24.4s, v24.4s, v26.4s

    st1 {v24.d}[0], [x6], #8
    st1 {v24.s}[2], [x6]
ret

//void uavs3e_get_sad_x4_4_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_4_arm64
    ldr x8, [sp]
    lsl x1, x1, #1
    lsl x6, x6, #1
    movi v18.16b, #0
    movi v19.16b, #0
    movi v20.16b, #0
    movi v21.16b, #0

get_sad_x4_4_y:
    //load p_org
    ld1 {v0.d}[0], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v1.d}[0], [x0], x1
    ld1 {v1.d}[1], [x0], x1
    //load pred
    ld1 {v2.d}[0], [x2], x6
    ld1 {v2.d}[1], [x2], x6
    ld1 {v3.d}[0], [x2], x6
    ld1 {v3.d}[1], [x2], x6
    ld1 {v4.d}[0], [x3], x6
    ld1 {v4.d}[1], [x3], x6
    ld1 {v5.d}[0], [x3], x6
    ld1 {v5.d}[1], [x3], x6
    ld1 {v6.d}[0], [x4], x6
    ld1 {v6.d}[1], [x4], x6
    ld1 {v7.d}[0], [x4], x6
    ld1 {v7.d}[1], [x4], x6
    ld1 {v16.d}[0], [x5], x6
    ld1 {v16.d}[1], [x5], x6
    ld1 {v17.d}[0], [x5], x6
    ld1 {v17.d}[1], [x5], x6

    //abs
    uabd v2.8h, v0.8h, v2.8h
    uabd v3.8h, v1.8h, v3.8h
    uabd v4.8h, v0.8h, v4.8h
    uabd v5.8h, v1.8h, v5.8h
    uabd v6.8h, v0.8h, v6.8h
    uabd v7.8h, v1.8h, v7.8h
    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h

    add v2.8h, v2.8h, v3.8h
    add v4.8h, v4.8h, v5.8h
    add v6.8h, v6.8h, v7.8h
    add v16.8h, v16.8h, v17.8h
    add v18.8h, v18.8h, v2.8h
    add v19.8h, v19.8h, v4.8h
    add v20.8h, v20.8h, v6.8h
    add v21.8h, v21.8h, v16.8h

    subs w8, w8, #4
    bgt get_sad_x4_4_y

    uaddlp v18.4s, v18.8h
    uaddlp v19.4s, v19.8h
    uaddlp v20.4s, v20.8h
    uaddlp v21.4s, v21.8h
    addp v18.4s, v18.4s, v19.4s
    addp v20.4s, v20.4s, v21.4s
    addp v18.4s, v18.4s, v20.4s

    st1 {v18.4s}, [x7]
    ret

//void uavs3e_get_sad_x4_8_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_8_arm64
    ldr x8, [sp]
    lsl x1, x1, #1
    lsl x6, x6, #1
    movi v28.16b, #0
    movi v29.16b, #0
    movi v30.16b, #0
    movi v31.16b, #0

get_sad_x4_8_y:
    //load p_org
    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1
    //load pred
    ld1 {v4.8h}, [x2], x6
    ld1 {v5.8h}, [x2], x6
    ld1 {v6.8h}, [x2], x6
    ld1 {v7.8h}, [x2], x6
    ld1 {v16.8h}, [x3], x6
    ld1 {v17.8h}, [x3], x6
    ld1 {v18.8h}, [x3], x6
    ld1 {v19.8h}, [x3], x6
    ld1 {v20.8h}, [x4], x6
    ld1 {v21.8h}, [x4], x6
    ld1 {v22.8h}, [x4], x6
    ld1 {v23.8h}, [x4], x6
    ld1 {v24.8h}, [x5], x6
    ld1 {v25.8h}, [x5], x6
    ld1 {v26.8h}, [x5], x6
    ld1 {v27.8h}, [x5], x6

    //abs
    uabd v4.8h, v0.8h, v4.8h
    uabd v5.8h, v1.8h, v5.8h
    uabd v6.8h, v2.8h, v6.8h
    uabd v7.8h, v3.8h, v7.8h
    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v0.8h, v20.8h
    uabd v21.8h, v1.8h, v21.8h
    uabd v22.8h, v2.8h, v22.8h
    uabd v23.8h, v3.8h, v23.8h
    uabd v24.8h, v0.8h, v24.8h
    uabd v25.8h, v1.8h, v25.8h
    uabd v26.8h, v2.8h, v26.8h
    uabd v27.8h, v3.8h, v27.8h

    add v4.8h, v4.8h, v5.8h
    add v6.8h, v6.8h, v7.8h
    add v4.8h, v4.8h, v6.8h
    add v28.8h, v28.8h, v4.8h
    add v16.8h, v16.8h, v17.8h
    add v18.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v18.8h
    add v29.8h, v29.8h, v16.8h
    add v20.8h, v20.8h, v21.8h
    add v22.8h, v22.8h, v23.8h
    add v20.8h, v20.8h, v22.8h
    add v30.8h, v30.8h, v20.8h
    add v24.8h, v24.8h, v25.8h
    add v26.8h, v26.8h, v27.8h
    add v24.8h, v24.8h, v26.8h
    add v31.8h, v31.8h, v24.8h

    subs w8, w8, #4
    bgt get_sad_x4_8_y

    uaddlp v28.4s, v28.8h
    uaddlp v29.4s, v29.8h
    uaddlp v30.4s, v30.8h
    uaddlp v31.4s, v31.8h
    addp v28.4s, v28.4s, v29.4s
    addp v30.4s, v30.4s, v31.4s
    addp v28.4s, v28.4s, v30.4s

    st1 {v28.4s}, [x7]

ret

//void uavs3e_get_sad_x4_16_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_16_arm64
    ldr x8, [sp]
    lsl x1, x1, #1
    lsl x6, x6, #1
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_16_y:
    //load p_org
    ld1 {v0.8h, v1.8h}, [x0], x1
    ld1 {v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h}, [x0], x1
    ld1 {v6.8h, v7.8h}, [x0], x1

    //load pred0
    ld1 {v16.8h, v17.8h}, [x2], x6
    ld1 {v18.8h, v19.8h}, [x2], x6
    ld1 {v20.8h, v21.8h}, [x2], x6
    ld1 {v22.8h, v23.8h}, [x2], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h}, [x3], x6
    ld1 {v18.8h, v19.8h}, [x3], x6
    ld1 {v20.8h, v21.8h}, [x3], x6
    ld1 {v22.8h, v23.8h}, [x3], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h}, [x4], x6
    ld1 {v18.8h, v19.8h}, [x4], x6
    ld1 {v20.8h, v21.8h}, [x4], x6
    ld1 {v22.8h, v23.8h}, [x4], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h}, [x5], x6
    ld1 {v18.8h, v19.8h}, [x5], x6
    ld1 {v20.8h, v21.8h}, [x5], x6
    ld1 {v22.8h, v23.8h}, [x5], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    subs w8, w8, #4
    bgt get_sad_x4_16_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]
ret

//void uavs3e_get_sad_x4_32_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[3], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[3]->x7, height->[x8]
function uavs3e_get_sad_x4_32_arm64
    ldr x8, [sp]
    lsl x1, x1, #1
    lsl x6, x6, #1
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_32_y:
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x0], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x2], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x2], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x3], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x3], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x4], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x4], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x5], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x5], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    subs w8, w8, #2
    bgt get_sad_x4_32_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]

ret


//void uavs3e_get_sad_x4_64_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[4], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_64_arm64
    ldr x15, [sp]
    lsl x1, x1, #1
    lsl x6, x6, #1
    lsl x13, x1, #1     //2 * i_org
    lsl x14, x6, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_64_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    mov x12, x5
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x12], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x12], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #64
    add x9, x2, #64
    add x10, x3, #64
    add x11, x4, #64
    add x12, x5, #64
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x12], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x12], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x0, x0, x13
    add x2, x2, x14
    add x3, x3, x14
    add x4, x4, x14
    add x5, x5, x14

    subs w15, w15, #2
    bgt get_sad_x4_64_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]
ret

//void uavs3e_get_sad_x4_128_arm64(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel, *pred3, int i_pred, u32 sad[4], int height)
//*p_org->x0, i_org->x1, *pred0->x2, *pred1->x3, *pred2->x4, *pred3->x5, i_pred->x6, sad[4]->x7, height->[x8]
function uavs3e_get_sad_x4_128_arm64
    ldr x15, [sp]
    lsl x1, x1, #1
    lsl x6, x6, #1
    lsl x13, x1, #1     //2 * i_org
    lsl x14, x6, #1     //2 * i_pred
    movi v24.16b, #0
    movi v25.16b, #0
    movi v26.16b, #0
    movi v27.16b, #0

get_sad_x4_128_y:
    mov x8, x0
    mov x9, x2
    mov x10, x3
    mov x11, x4
    mov x12, x5
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x12], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x12], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #64
    add x9, x2, #64
    add x10, x3, #64
    add x11, x4, #64
    add x12, x5, #64
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x12], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x12], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #128
    add x9, x2, #128
    add x10, x3, #128
    add x11, x4, #128
    add x12, x5, #128
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x12], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x12], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x8, x0, #192
    add x9, x2, #192
    add x10, x3, #192
    add x11, x4, #192
    add x12, x5, #192
    //load p_org
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x8], x1
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x8], x1
    //load pred0
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x9], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x9], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v24.4s, v24.4s, v16.4s

    //load pred1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v25.4s, v25.4s, v16.4s

    //load pred2
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x11], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x11], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v26.4s, v26.4s, v16.4s

    //load pred3
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x12], x6
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x12], x6

    uabd v16.8h, v0.8h, v16.8h
    uabd v17.8h, v1.8h, v17.8h
    uabd v18.8h, v2.8h, v18.8h
    uabd v19.8h, v3.8h, v19.8h
    uabd v20.8h, v4.8h, v20.8h
    uabd v21.8h, v5.8h, v21.8h
    uabd v22.8h, v6.8h, v22.8h
    uabd v23.8h, v7.8h, v23.8h

    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v18.8h, v20.8h, v21.8h
    add v19.8h, v22.8h, v23.8h
    add v16.8h, v16.8h, v17.8h
    add v17.8h, v18.8h, v19.8h
    add v16.8h, v16.8h, v17.8h
    uaddlp v16.4s, v16.8h
    add v27.4s, v27.4s, v16.4s

    add x0, x0, x13
    add x2, x2, x14
    add x3, x3, x14
    add x4, x4, x14
    add x5, x5, x14

    subs w15, w15, #2
    bgt get_sad_x4_128_y

    addp v24.4s, v24.4s, v25.4s
    addp v25.4s, v26.4s, v27.4s
    addp v24.4s, v24.4s, v25.4s

    st1 {v24.4s}, [x7]

ret

//void uavs3e_had_4x4_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_4x4_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v0.d}[0], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v2.d}[0], [x0], x1
    ld1 {v2.d}[1], [x0], x1

    ld1 {v1.d}[0], [x2], x3
    ld1 {v1.d}[1], [x2], x3
    ld1 {v3.d}[0], [x2], x3
    ld1 {v3.d}[1], [x2], x3

    sub v0.8h, v0.8h, v1.8h
    sub v1.8h, v2.8h, v3.8h

    uzp1 v2.8h, v0.8h, v1.8h    //d0, d2, d4, d6
    uzp2 v3.8h, v0.8h, v1.8h    //d1, d3, d5, d7

    add v0.8h, v2.8h, v3.8h     //d0 + d1
    sub v1.8h, v2.8h, v3.8h     //d0 - d1

    trn1 v2.8h, v0.8h, v1.8h    //d0 + d1, d0 - d1, d4 + d5, d4 - d5
    trn2 v3.8h, v0.8h, v1.8h    //d2 + d3, d2 - d3, d6 + d7, d6 - d7

    add v0.8h, v2.8h, v3.8h     //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v2.8h, v3.8h     //d0 + d1 - d2 - d3, d0 - d1 - d2 + d3

    trn1 v2.4s, v0.4s, v1.4s    //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - d2 - d3, d0 - d1 - d2 + d3
    trn2 v3.4s, v0.4s, v1.4s    //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - d6 - d7, d4 - d5 - d6 + d7

    add v0.8h, v2.8h, v3.8h
    sub v1.8h, v2.8h, v3.8h

    trn1 v2.2d, v0.2d, v1.2d
    trn2 v3.2d, v0.2d, v1.2d

    add v0.8h, v2.8h, v3.8h
    sub v1.8h, v2.8h, v3.8h

    abs v0.8h, v0.8h
    abs v1.8h, v1.8h

    uaddl v2.4s, v0.4h, v1.4h
    uaddl2 v3.4s, v0.8h, v1.8h
    add v0.4s, v2.4s, v3.4s
    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    add x0, x0, #1
    lsr x0, x0, #1

ret

//void uavs3e_had_8x8_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_8x8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1
    ld1 {v4.8h}, [x0], x1
    ld1 {v5.8h}, [x0], x1
    ld1 {v6.8h}, [x0], x1
    ld1 {v7.8h}, [x0], x1

    ld1 {v16.8h}, [x2], x3
    ld1 {v17.8h}, [x2], x3
    ld1 {v18.8h}, [x2], x3
    ld1 {v19.8h}, [x2], x3
    ld1 {v20.8h}, [x2], x3
    ld1 {v21.8h}, [x2], x3
    ld1 {v22.8h}, [x2], x3
    ld1 {v23.8h}, [x2], x3

    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h

    uzp1 v16.8h, v0.8h, v1.8h       //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v17.8h, v0.8h, v1.8h       //d1, d3, d5, d7,
    uzp1 v18.8h, v2.8h, v3.8h       //d16, d18, d20, d22,
    uzp2 v19.8h, v2.8h, v3.8h       //d17, d19, d21, d23,
    uzp1 v20.8h, v4.8h, v5.8h       //d32, d34, d36, d38,
    uzp2 v21.8h, v4.8h, v5.8h       //d33, d35, d37, d39,
    uzp1 v22.8h, v6.8h, v7.8h       //d48, d50, d52, d54,
    uzp2 v23.8h, v6.8h, v7.8h       //d49, d51, d53, d55,

    add v0.8h, v16.8h, v17.8h       //d0 + d1, d2 + d3,
    sub v1.8h, v16.8h, v17.8h       //d0 - d1, d2 - d3,
    add v2.8h, v18.8h, v19.8h       //d16 + d17, d18 + d19
    sub v3.8h, v18.8h, v19.8h       //d16 - d17, d18 - d19
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    trn1 v16.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1, d4 + d5, d4 - d5
    trn2 v17.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3, d6 + d7, d5 - d7
    trn1 v18.8h, v2.8h, v3.8h
    trn2 v19.8h, v2.8h, v3.8h
    trn1 v20.8h, v4.8h, v5.8h
    trn2 v21.8h, v4.8h, v5.8h
    trn1 v22.8h, v6.8h, v7.8h
    trn2 v23.8h, v6.8h, v7.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v18.8h, v19.8h       //d16 + d17 + d18 + d19, d16 - d17 + (d18 - d19)
    sub v3.8h, v18.8h, v19.8h       //d16 + d17 - d18 + d19, d16 - d17 - (d18 - d19)
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    trn1 v16.4s, v0.4s, v1.4s       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v17.4s, v0.4s, v1.4s       //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v18.4s, v2.4s, v3.4s
    trn2 v19.4s, v2.4s, v3.4s
    trn1 v20.4s, v4.4s, v5.4s
    trn2 v21.4s, v4.4s, v5.4s
    trn1 v22.4s, v6.4s, v7.4s
    trn2 v23.4s, v6.4s, v7.4s

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7, d0 - d1 + d2 - d3 + d4 - d5 + d6 - d7
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v18.8h, v19.8h       //d16 + d17 + d18 + d19 + d20 + d21 + d22 + d23
    sub v3.8h, v18.8h, v19.8h       //d16 + d17 + d18 + d19 - (d20 + d21 + d22 + d23)
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    trn1 v16.2d, v0.2d, v1.2d
    trn2 v17.2d, v0.2d, v1.2d
    trn1 v18.2d, v2.2d, v3.2d
    trn2 v19.2d, v2.2d, v3.2d
    trn1 v20.2d, v4.2d, v5.2d
    trn2 v21.2d, v4.2d, v5.2d
    trn1 v22.2d, v6.2d, v7.2d
    trn2 v23.2d, v6.2d, v7.2d

    add v0.8h, v16.8h, v17.8h
    sub v1.8h, v16.8h, v17.8h
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h

    add v16.8h, v0.8h, v2.8h
    sub v17.8h, v0.8h, v2.8h
    add v18.8h, v1.8h, v3.8h
    sub v19.8h, v1.8h, v3.8h
    add v20.8h, v4.8h, v6.8h
    sub v21.8h, v4.8h, v6.8h
    add v22.8h, v5.8h, v7.8h
    sub v23.8h, v5.8h, v7.8h

    saddl v0.4s, v16.4h, v20.4h
    saddl2 v1.4s, v16.8h, v20.8h
    saddl v2.4s, v17.4h, v21.4h
    saddl2 v3.4s, v17.8h, v21.8h
    saddl v4.4s, v18.4h, v22.4h
    saddl2 v5.4s, v18.8h, v22.8h
    saddl v6.4s, v19.4h, v23.4h
    saddl2 v7.4s, v19.8h, v23.8h
    ssubl v24.4s, v16.4h, v20.4h
    ssubl2 v25.4s, v16.8h, v20.8h
    ssubl v26.4s, v17.4h, v21.4h
    ssubl2 v27.4s, v17.8h, v21.8h
    ssubl v28.4s, v18.4h, v22.4h
    ssubl2 v29.4s, v18.8h, v22.8h
    ssubl v30.4s, v19.4h, v23.4h
    ssubl2 v31.4s, v19.8h, v23.8h

    abs v0.4s, v0.4s
    abs v1.4s, v1.4s
    abs v2.4s, v2.4s
    abs v3.4s, v3.4s
    abs v4.4s, v4.4s
    abs v5.4s, v5.4s
    abs v6.4s, v6.4s
    abs v7.4s, v7.4s
    abs v24.4s, v24.4s
    abs v25.4s, v25.4s
    abs v26.4s, v26.4s
    abs v27.4s, v27.4s
    abs v28.4s, v28.4s
    abs v29.4s, v29.4s
    abs v30.4s, v30.4s
    abs v31.4s, v31.4s

    add v0.4s, v0.4s, v24.4s
    add v1.4s, v1.4s, v25.4s
    add v2.4s, v2.4s, v26.4s
    add v3.4s, v3.4s, v27.4s
    add v4.4s, v4.4s, v28.4s
    add v5.4s, v5.4s, v29.4s
    add v6.4s, v6.4s, v30.4s
    add v7.4s, v7.4s, v31.4s

    add v0.4s, v0.4s, v1.4s
    add v2.4s, v2.4s, v3.4s
    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v0.4s, v0.4s, v2.4s
    add v4.4s, v4.4s, v6.4s
    add v0.4s, v0.4s, v4.4s

    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    add x0, x0, #2
    lsr x0, x0, #2

ret

//void uavs3e_had_8x4_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_8x4_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1

    ld1 {v4.8h}, [x2], x3
    ld1 {v5.8h}, [x2], x3
    ld1 {v6.8h}, [x2], x3
    ld1 {v7.8h}, [x2], x3

    sub v0.8h, v0.8h, v4.8h
    sub v1.8h, v1.8h, v5.8h
    sub v2.8h, v2.8h, v6.8h
    sub v3.8h, v3.8h, v7.8h

    uzp1 v4.8h, v0.8h, v1.8h       //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v5.8h, v0.8h, v1.8h       //d1, d3, d5, d7, d9, d11, d13, d15
    uzp1 v6.8h, v2.8h, v3.8h       //d16, d18, d20, d22, d24, d26, d28, d30
    uzp2 v7.8h, v2.8h, v3.8h       //d17, d19, d21, d23, d25, d27, d29, d31


    add v0.8h, v4.8h, v5.8h        //d0 + d1, d2 + d3,
    sub v1.8h, v4.8h, v5.8h        //d0 - d1, d2 - d3,
    add v2.8h, v6.8h, v7.8h        //d16 + d17, d18 + d19
    sub v3.8h, v6.8h, v7.8h        //d16 - d17, d18 - d19


    trn1 v4.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1, d4 + d5, d4 - d5
    trn2 v5.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3, d6 + d7, d5 - d7
    trn1 v6.8h, v2.8h, v3.8h       //d16 + d17, d16 - d17
    trn2 v7.8h, v2.8h, v3.8h       //d18 + d19, d18 - d19

    add v0.8h, v4.8h, v5.8h        //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v4.8h, v5.8h        //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v6.8h, v7.8h        //d16 + d17 + d18 + d19, d16 - d17 + (d18 - d19)
    sub v3.8h, v6.8h, v7.8h        //d16 + d17 - d18 + d19, d16 - d17 - (d18 - d19)

    trn1 v4.4s, v0.4s, v1.4s      //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v5.4s, v0.4s, v1.4s      //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v6.4s, v2.4s, v3.4s
    trn2 v7.4s, v2.4s, v3.4s

    add v0.8h, v4.8h, v5.8h        //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7, d0 - d1 + d2 - d3 + d4 - d5 + d6 - d7
    sub v1.8h, v4.8h, v5.8h        //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v6.8h, v7.8h        //d16 + d17 + d18 + d19 + d20 + d21 + d22 + d23
    sub v3.8h, v6.8h, v7.8h        //d16 + d17 + d18 + d19 - (d20 + d21 + d22 + d23)

    trn1 v4.2d, v0.2d, v1.2d       //d0...7
    trn2 v5.2d, v0.2d, v1.2d       //d8...15
    trn1 v6.2d, v2.2d, v3.2d       //d16...d23
    trn2 v7.2d, v2.2d, v3.2d       //d24...d31

    add v0.8h, v4.8h, v6.8h
    add v1.8h, v5.8h, v7.8h
    sub v2.8h, v4.8h, v6.8h
    sub v3.8h, v5.8h, v7.8h

    add v4.8h, v0.8h, v1.8h
    sub v5.8h, v0.8h, v1.8h
    add v6.8h, v2.8h, v3.8h
    sub v7.8h, v2.8h, v3.8h

    abs v0.8h, v4.8h
    abs v1.8h, v5.8h
    abs v2.8h, v6.8h
    abs v3.8h, v7.8h

    uaddl v4.4s, v0.4h, v1.4h
    uaddl2 v5.4s, v0.8h, v1.8h
    uaddl v6.4s, v2.4h, v3.4h
    uaddl2 v7.4s, v2.8h, v3.8h

    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v0.4s, v4.4s, v6.4s

    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    mov x1, #32
    ucvtf d0, x1
    fsqrt d0, d0
    ucvtf d1, x0
    fdiv d0, d1, d0
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0

    ret

//void uavs3e_had_4x8_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_4x8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v0.d}[0], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v1.d}[0], [x0], x1
    ld1 {v1.d}[1], [x0], x1
    ld1 {v2.d}[0], [x0], x1
    ld1 {v2.d}[1], [x0], x1
    ld1 {v3.d}[0], [x0], x1
    ld1 {v3.d}[1], [x0], x1

    ld1 {v4.d}[0], [x2], x3
    ld1 {v4.d}[1], [x2], x3
    ld1 {v5.d}[0], [x2], x3
    ld1 {v5.d}[1], [x2], x3
    ld1 {v6.d}[0], [x2], x3
    ld1 {v6.d}[1], [x2], x3
    ld1 {v7.d}[0], [x2], x3
    ld1 {v7.d}[1], [x2], x3

    sub v0.8h, v0.8h, v4.8h
    sub v1.8h, v1.8h, v5.8h
    sub v2.8h, v2.8h, v6.8h
    sub v3.8h, v3.8h, v7.8h

    uzp1 v4.8h, v0.8h, v1.8h        //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v5.8h, v0.8h, v1.8h        //d1, d3, d5, d7, d9, d11, d13, d15
    uzp1 v6.8h, v2.8h, v3.8h        //d16, d18, d20, d22, d24, d26, d28, d30
    uzp2 v7.8h, v2.8h, v3.8h        //d17, d19, d21, d23, d25, d27, d29, d31

    add v0.8h, v4.8h, v5.8h         //d0 + d1, d2 + d3
    sub v1.8h, v4.8h, v5.8h         //d0 - d1, d2 - d3
    add v2.8h, v6.8h, v7.8h         //d16 + d17, d18 + d19
    sub v3.8h, v6.8h, v7.8h         //d16 - d17, d18 - d19

    trn1 v4.8h, v0.8h, v1.8h        //d0 + d1, d0 - d1
    trn2 v5.8h, v0.8h, v1.8h        //d2 + d3, d2 - d3
    trn1 v6.8h, v2.8h, v3.8h        //d16 + d17, d16 - d17
    trn2 v7.8h, v2.8h, v3.8h        //d18 + d19, d18 - d19

    add v0.8h, v4.8h, v5.8h         //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v4.8h, v5.8h         //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v6.8h, v7.8h         //d16 + d17 + d18 + d19
    sub v3.8h, v6.8h, v7.8h         //d16 + d17 - (d18 + d19)

    trn1 v4.4s, v0.4s, v1.4s        //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v5.4s, v0.4s, v1.4s        //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v6.4s, v2.4s, v3.4s
    trn2 v7.4s, v2.4s, v3.4s

    add v0.8h, v4.8h, v6.8h
    add v1.8h, v5.8h, v7.8h
    sub v2.8h, v4.8h, v6.8h
    sub v3.8h, v5.8h, v7.8h

    trn1 v4.2d, v0.2d, v1.2d
    trn2 v5.2d, v0.2d, v1.2d
    trn1 v6.2d, v2.2d, v3.2d
    trn2 v7.2d, v2.2d, v3.2d

    add v0.8h, v4.8h, v5.8h
    sub v1.8h, v4.8h, v5.8h
    add v2.8h, v6.8h, v7.8h
    sub v3.8h, v6.8h, v7.8h

    trn1 v4.2d, v0.2d, v1.2d
    trn2 v5.2d, v0.2d, v1.2d
    trn1 v6.2d, v2.2d, v3.2d
    trn2 v7.2d, v2.2d, v3.2d

    add v0.8h, v4.8h, v5.8h
    sub v1.8h, v4.8h, v5.8h
    add v2.8h, v6.8h, v7.8h
    sub v3.8h, v6.8h, v7.8h

    abs v0.8h, v0.8h
    abs v1.8h, v1.8h
    abs v2.8h, v2.8h
    abs v3.8h, v3.8h

    uaddl v4.4s, v0.4h, v1.4h
    uaddl2 v5.4s, v0.8h, v1.8h
    uaddl v6.4s, v2.4h, v3.4h
    uaddl2 v7.4s, v2.8h, v3.8h

    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v0.4s, v4.4s, v6.4s

    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x0, #0
    umov w0, v0.s[0]
    mov x1, #32
    ucvtf d0, x1
    fsqrt d0, d0
    ucvtf d1, x0
    fdiv d0, d1, d0
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0
    ret

//void uavs3e_had_8x16_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_8x16_arm64
    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1
    ld1 {v4.8h}, [x0], x1
    ld1 {v5.8h}, [x0], x1
    ld1 {v6.8h}, [x0], x1
    ld1 {v7.8h}, [x0], x1
    ld1 {v8.8h}, [x0], x1
    ld1 {v9.8h}, [x0], x1
    ld1 {v10.8h}, [x0], x1
    ld1 {v11.8h}, [x0], x1
    ld1 {v12.8h}, [x0], x1
    ld1 {v13.8h}, [x0], x1
    ld1 {v14.8h}, [x0], x1
    ld1 {v15.8h}, [x0], x1

    ld1 {v16.8h}, [x2], x3
    ld1 {v17.8h}, [x2], x3
    ld1 {v18.8h}, [x2], x3
    ld1 {v19.8h}, [x2], x3
    ld1 {v20.8h}, [x2], x3
    ld1 {v21.8h}, [x2], x3
    ld1 {v22.8h}, [x2], x3
    ld1 {v23.8h}, [x2], x3
    ld1 {v24.8h}, [x2], x3
    ld1 {v25.8h}, [x2], x3
    ld1 {v26.8h}, [x2], x3
    ld1 {v27.8h}, [x2], x3
    ld1 {v28.8h}, [x2], x3
    ld1 {v29.8h}, [x2], x3
    ld1 {v30.8h}, [x2], x3
    ld1 {v31.8h}, [x2], x3

    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h
    sub v8.8h, v8.8h, v24.8h
    sub v9.8h, v9.8h, v25.8h
    sub v10.8h, v10.8h, v26.8h
    sub v11.8h, v11.8h, v27.8h
    sub v12.8h, v12.8h, v28.8h
    sub v13.8h, v13.8h, v29.8h
    sub v14.8h, v14.8h, v30.8h
    sub v15.8h, v15.8h, v31.8h

    uzp1 v16.8h, v0.8h, v1.8h       //d0, d2, d4, d6
    uzp2 v17.8h, v0.8h, v1.8h       //d1, d3, d5, d7
    uzp1 v18.8h, v2.8h, v3.8h       //d16, d18, d20, d22
    uzp2 v19.8h, v2.8h, v3.8h       //d17, d19, d21, d23
    uzp1 v20.8h, v4.8h, v5.8h
    uzp2 v21.8h, v4.8h, v5.8h
    uzp1 v22.8h, v6.8h, v7.8h
    uzp2 v23.8h, v6.8h, v7.8h
    uzp1 v24.8h, v8.8h, v9.8h
    uzp2 v25.8h, v8.8h, v9.8h
    uzp1 v26.8h, v10.8h, v11.8h
    uzp2 v27.8h, v10.8h, v11.8h
    uzp1 v28.8h, v12.8h, v13.8h
    uzp2 v29.8h, v12.8h, v13.8h
    uzp1 v30.8h, v14.8h, v15.8h
    uzp2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1, d2 + d3
    sub v1.8h, v16.8h, v17.8h       //d0 - d1, d2 - d3
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1
    trn2 v17.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3
    trn1 v18.8h, v2.8h, v3.8h
    trn2 v19.8h, v2.8h, v3.8h
    trn1 v20.8h, v4.8h, v5.8h
    trn2 v21.8h, v4.8h, v5.8h
    trn1 v22.8h, v6.8h, v7.8h
    trn2 v23.8h, v6.8h, v7.8h
    trn1 v24.8h, v8.8h, v9.8h
    trn2 v25.8h, v8.8h, v9.8h
    trn1 v26.8h, v10.8h, v11.8h
    trn2 v27.8h, v10.8h, v11.8h
    trn1 v28.8h, v12.8h, v13.8h
    trn2 v29.8h, v12.8h, v13.8h
    trn1 v30.8h, v14.8h, v15.8h
    trn2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.4s, v0.4s, v1.4s       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v17.4s, v0.4s, v1.4s       //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v18.4s, v2.4s, v3.4s
    trn2 v19.4s, v2.4s, v3.4s
    trn1 v20.4s, v4.4s, v5.4s
    trn2 v21.4s, v4.4s, v5.4s
    trn1 v22.4s, v6.4s, v7.4s
    trn2 v23.4s, v6.4s, v7.4s
    trn1 v24.4s, v8.4s, v9.4s
    trn2 v25.4s, v8.4s, v9.4s
    trn1 v26.4s, v10.4s, v11.4s
    trn2 v27.4s, v10.4s, v11.4s
    trn1 v28.4s, v12.4s, v13.4s
    trn2 v29.4s, v12.4s, v13.4s
    trn1 v30.4s, v14.4s, v15.4s
    trn2 v31.4s, v14.4s, v15.4s

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.2d, v0.2d, v1.2d
    trn2 v17.2d, v0.2d, v1.2d
    trn1 v18.2d, v2.2d, v3.2d
    trn2 v19.2d, v2.2d, v3.2d
    trn1 v20.2d, v4.2d, v5.2d
    trn2 v21.2d, v4.2d, v5.2d
    trn1 v22.2d, v6.2d, v7.2d
    trn2 v23.2d, v6.2d, v7.2d
    trn1 v24.2d, v8.2d, v9.2d
    trn2 v25.2d, v8.2d, v9.2d
    trn1 v26.2d, v10.2d, v11.2d
    trn2 v27.2d, v10.2d, v11.2d
    trn1 v28.2d, v12.2d, v13.2d
    trn2 v29.2d, v12.2d, v13.2d
    trn1 v30.2d, v14.2d, v15.2d
    trn2 v31.2d, v14.2d, v15.2d

    add v0.8h, v16.8h, v24.8h
    add v1.8h, v17.8h, v25.8h
    add v2.8h, v18.8h, v26.8h
    add v3.8h, v19.8h, v27.8h
    add v4.8h, v20.8h, v28.8h
    add v5.8h, v21.8h, v29.8h
    add v6.8h, v22.8h, v30.8h
    add v7.8h, v23.8h, v31.8h
    sub v8.8h, v16.8h, v24.8h
    sub v9.8h, v17.8h, v25.8h
    sub v10.8h, v18.8h, v26.8h
    sub v11.8h, v19.8h, v27.8h
    sub v12.8h, v20.8h, v28.8h
    sub v13.8h, v21.8h, v29.8h
    sub v14.8h, v22.8h, v30.8h
    sub v15.8h, v23.8h, v31.8h

    add v16.8h, v0.8h, v4.8h
    add v17.8h, v1.8h, v5.8h
    add v18.8h, v2.8h, v6.8h
    add v19.8h, v3.8h, v7.8h
    sub v20.8h, v0.8h, v4.8h
    sub v21.8h, v1.8h, v5.8h
    sub v22.8h, v2.8h, v6.8h
    sub v23.8h, v3.8h, v7.8h
    add v24.8h, v8.8h, v12.8h
    add v25.8h, v9.8h, v13.8h
    add v26.8h, v10.8h, v14.8h
    add v27.8h, v11.8h, v15.8h
    sub v28.8h, v8.8h, v12.8h
    sub v29.8h, v9.8h, v13.8h
    sub v30.8h, v10.8h, v14.8h
    sub v31.8h, v11.8h, v15.8h

    saddl v0.4s, v16.4h, v18.4h
    saddl2 v1.4s, v16.8h, v18.8h
    saddl v2.4s, v17.4h, v19.4h
    saddl2 v3.4s, v17.8h, v19.8h
    ssubl v4.4s, v16.4h, v18.4h
    ssubl2 v5.4s, v16.8h, v18.8h
    ssubl v6.4s, v17.4h, v19.4h
    ssubl2 v7.4s, v17.8h, v19.8h
    saddl v8.4s, v20.4h, v22.4h
    saddl2 v9.4s, v20.8h, v22.8h
    saddl v10.4s, v21.4h, v23.4h
    saddl2 v11.4s, v21.8h, v23.8h
    ssubl v12.4s, v20.4h, v22.4h
    ssubl2 v13.4s, v20.8h, v22.8h
    ssubl v14.4s, v21.4h, v23.4h
    ssubl2 v15.4s, v21.8h, v23.8h
    saddl v16.4s, v24.4h, v26.4h
    saddl2 v17.4s, v24.8h, v26.8h
    saddl v18.4s, v25.4h, v27.4h
    saddl2 v19.4s, v25.8h, v27.8h
    ssubl v20.4s, v24.4h, v26.4h
    ssubl2 v21.4s, v24.8h, v26.8h
    ssubl v22.4s, v25.4h, v27.4h
    ssubl2 v23.4s, v25.8h, v27.8h
    saddl v24.4s, v28.4h, v30.4h
    saddl2 v25.4s, v28.8h, v30.8h
    saddl v26.4s, v29.4h, v31.4h
    saddl2 v27.4s, v29.8h, v31.8h
    sub sp, sp, #64
    st1 {v24.8h, v25.8h, v26.8h, v27.8h}, [sp]
    ssubl v24.4s, v28.4h, v30.4h
    ssubl2 v25.4s, v28.8h, v30.8h
    ssubl v26.4s, v29.4h, v31.4h
    ssubl2 v27.4s, v29.8h, v31.8h
    mov v28.16b, v24.16b
    mov v29.16b, v25.16b
    mov v30.16b, v26.16b
    mov v31.16b, v27.16b

    add v24.4s, v0.4s, v2.4s
    add v25.4s, v1.4s, v3.4s
    sub v26.4s, v0.4s, v2.4s
    sub v27.4s, v1.4s, v3.4s
    mov v0.16b, v24.16b
    mov v1.16b, v25.16b
    mov v2.16b, v26.16b
    mov v3.16b, v27.16b
    add v24.4s, v4.4s, v6.4s
    add v25.4s, v5.4s, v7.4s
    sub v26.4s, v4.4s, v6.4s
    sub v27.4s, v5.4s, v7.4s
    mov v4.16b, v24.16b
    mov v5.16b, v25.16b
    mov v6.16b, v26.16b
    mov v7.16b, v27.16b
    add v24.4s, v8.4s, v10.4s
    add v25.4s, v9.4s, v11.4s
    sub v26.4s, v8.4s, v10.4s
    sub v27.4s, v9.4s, v11.4s
    mov v8.16b, v24.16b
    mov v9.16b, v25.16b
    mov v10.16b, v26.16b
    mov v11.16b, v27.16b
    add v24.4s, v12.4s, v14.4s
    add v25.4s, v13.4s, v15.4s
    sub v26.4s, v12.4s, v14.4s
    sub v27.4s, v13.4s, v15.4s
    mov v12.16b, v24.16b
    mov v13.16b, v25.16b
    mov v14.16b, v26.16b
    mov v15.16b, v27.16b
    add v24.4s, v16.4s, v18.4s
    add v25.4s, v17.4s, v19.4s
    sub v26.4s, v16.4s, v18.4s
    sub v27.4s, v17.4s, v19.4s
    mov v16.16b, v24.16b
    mov v17.16b, v25.16b
    mov v18.16b, v26.16b
    mov v19.16b, v27.16b
    add v24.4s, v20.4s, v22.4s
    add v25.4s, v21.4s, v23.4s
    sub v26.4s, v20.4s, v22.4s
    sub v27.4s, v21.4s, v23.4s
    mov v20.16b, v24.16b
    mov v21.16b, v25.16b
    mov v22.16b, v26.16b
    mov v23.16b, v27.16b
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [sp]
    st1 {v0.8h, v1.8h, v2.8h, v3.8h}, [sp]
    add v0.4s, v24.4s, v26.4s
    add v1.4s, v25.4s, v27.4s
    sub v2.4s, v24.4s, v26.4s
    sub v3.4s, v25.4s, v27.4s
    mov v24.16b, v0.16b
    mov v25.16b, v1.16b
    mov v26.16b, v2.16b
    mov v27.16b, v3.16b
    add v0.4s, v28.4s, v30.4s
    add v1.4s, v29.4s, v31.4s
    sub v2.4s, v28.4s, v30.4s
    sub v3.4s, v29.4s, v31.4s
    mov v28.16b, v0.16b
    mov v29.16b, v1.16b
    mov v30.16b, v2.16b
    mov v31.16b, v3.16b
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [sp], #64

    abs v0.4s, v0.4s
    abs v1.4s, v1.4s
    abs v2.4s, v2.4s
    abs v3.4s, v3.4s
    abs v4.4s, v4.4s
    abs v5.4s, v5.4s
    abs v6.4s, v6.4s
    abs v7.4s, v7.4s
    abs v8.4s, v8.4s
    abs v9.4s, v9.4s
    abs v10.4s, v10.4s
    abs v11.4s, v11.4s
    abs v12.4s, v12.4s
    abs v13.4s, v13.4s
    abs v14.4s, v14.4s
    abs v15.4s, v15.4s
    abs v16.4s, v16.4s
    abs v17.4s, v17.4s
    abs v18.4s, v18.4s
    abs v19.4s, v19.4s
    abs v20.4s, v20.4s
    abs v21.4s, v21.4s
    abs v22.4s, v22.4s
    abs v23.4s, v23.4s
    abs v24.4s, v24.4s
    abs v25.4s, v25.4s
    abs v26.4s, v26.4s
    abs v27.4s, v27.4s
    abs v28.4s, v28.4s
    abs v29.4s, v29.4s
    abs v30.4s, v30.4s
    abs v31.4s, v31.4s

    add v0.4s, v0.4s, v1.4s
    add v2.4s, v2.4s, v3.4s
    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v8.4s, v8.4s, v9.4s
    add v10.4s, v10.4s, v11.4s
    add v12.4s, v12.4s, v13.4s
    add v14.4s, v14.4s, v15.4s
    add v16.4s, v16.4s, v17.4s
    add v18.4s, v18.4s, v19.4s
    add v20.4s, v20.4s, v21.4s
    add v22.4s, v22.4s, v23.4s
    add v24.4s, v24.4s, v25.4s
    add v26.4s, v26.4s, v27.4s
    add v28.4s, v28.4s, v29.4s
    add v30.4s, v30.4s, v31.4s
    add v0.4s, v0.4s, v2.4s
    add v1.4s, v4.4s, v6.4s
    add v2.4s, v8.4s, v10.4s
    add v3.4s, v12.4s, v14.4s
    add v4.4s, v16.4s, v18.4s
    add v5.4s, v20.4s, v22.4s
    add v6.4s, v24.4s, v26.4s
    add v7.4s, v28.4s, v30.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v2.4s, v4.4s, v5.4s
    add v3.4s, v6.4s, v7.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v0.4s, v0.4s, v1.4s
    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x1, #128
    ucvtf d1, x1
    fsqrt d1, d1
    mov x0, #0
    umov w0, v0.s[0]
    ucvtf d0, x0
    fdiv d0, d0, d1
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0

    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64
    ret

//void uavs3e_had_16x8_arm64(pel *org, int s_org, pel *cur, int s_cur)
//*org->x0, s_org->x1, *cur->x2, s_cur->x3
function uavs3e_had_16x8_arm64
    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v0.8h, v1.8h}, [x0], x1
    ld1 {v2.8h, v3.8h}, [x0], x1
    ld1 {v4.8h, v5.8h}, [x0], x1
    ld1 {v6.8h, v7.8h}, [x0], x1
    ld1 {v8.8h, v9.8h}, [x0], x1
    ld1 {v10.8h, v11.8h}, [x0], x1
    ld1 {v12.8h, v13.8h}, [x0], x1
    ld1 {v14.8h, v15.8h}, [x0], x1

    ld1 {v16.8h, v17.8h}, [x2], x3
    ld1 {v18.8h, v19.8h}, [x2], x3
    ld1 {v20.8h, v21.8h}, [x2], x3
    ld1 {v22.8h, v23.8h}, [x2], x3
    ld1 {v24.8h, v25.8h}, [x2], x3
    ld1 {v26.8h, v27.8h}, [x2], x3
    ld1 {v28.8h, v29.8h}, [x2], x3
    ld1 {v30.8h, v31.8h}, [x2], x3

    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h
    sub v8.8h, v8.8h, v24.8h
    sub v9.8h, v9.8h, v25.8h
    sub v10.8h, v10.8h, v26.8h
    sub v11.8h, v11.8h, v27.8h
    sub v12.8h, v12.8h, v28.8h
    sub v13.8h, v13.8h, v29.8h
    sub v14.8h, v14.8h, v30.8h
    sub v15.8h, v15.8h, v31.8h

    uzp1 v16.8h, v0.8h, v1.8h       //d0, d2, d4, d6, d8, d10, d12, d14
    uzp2 v17.8h, v0.8h, v1.8h       //d1, d3, d5, d7, d9, d11, d13, d15
    uzp1 v18.8h, v2.8h, v3.8h       //d16, d18, d20, d22
    uzp2 v19.8h, v2.8h, v3.8h       //d17, d19, d21, d23
    uzp1 v20.8h, v4.8h, v5.8h
    uzp2 v21.8h, v4.8h, v5.8h
    uzp1 v22.8h, v6.8h, v7.8h
    uzp2 v23.8h, v6.8h, v7.8h
    uzp1 v24.8h, v8.8h, v9.8h
    uzp2 v25.8h, v8.8h, v9.8h
    uzp1 v26.8h, v10.8h, v11.8h
    uzp2 v27.8h, v10.8h, v11.8h
    uzp1 v28.8h, v12.8h, v13.8h
    uzp2 v29.8h, v12.8h, v13.8h
    uzp1 v30.8h, v14.8h, v15.8h
    uzp2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1, d2 + d3
    sub v1.8h, v16.8h, v17.8h       //d0 - d1, d2 - d3
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.8h, v0.8h, v1.8h       //d0 + d1, d0 - d1
    trn2 v17.8h, v0.8h, v1.8h       //d2 + d3, d2 - d3
    trn1 v18.8h, v2.8h, v3.8h
    trn2 v19.8h, v2.8h, v3.8h
    trn1 v20.8h, v4.8h, v5.8h
    trn2 v21.8h, v4.8h, v5.8h
    trn1 v22.8h, v6.8h, v7.8h
    trn2 v23.8h, v6.8h, v7.8h
    trn1 v24.8h, v8.8h, v9.8h
    trn2 v25.8h, v8.8h, v9.8h
    trn1 v26.8h, v10.8h, v11.8h
    trn2 v27.8h, v10.8h, v11.8h
    trn1 v28.8h, v12.8h, v13.8h
    trn2 v29.8h, v12.8h, v13.8h
    trn1 v30.8h, v14.8h, v15.8h
    trn2 v31.8h, v14.8h, v15.8h

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.4s, v0.4s, v1.4s       //d0 + d1 + d2 + d3, d0 - d1 + d2 - d3, d0 + d1 - (d2 + d3), d0 - d1 - (d2 - d3)
    trn2 v17.4s, v0.4s, v1.4s       //d4 + d5 + d6 + d7, d4 - d5 + d6 - d7, d4 + d5 - (d6 + d7), d4 - d5 - (d6 - d7)
    trn1 v18.4s, v2.4s, v3.4s
    trn2 v19.4s, v2.4s, v3.4s
    trn1 v20.4s, v4.4s, v5.4s
    trn2 v21.4s, v4.4s, v5.4s
    trn1 v22.4s, v6.4s, v7.4s
    trn2 v23.4s, v6.4s, v7.4s
    trn1 v24.4s, v8.4s, v9.4s
    trn2 v25.4s, v8.4s, v9.4s
    trn1 v26.4s, v10.4s, v11.4s
    trn2 v27.4s, v10.4s, v11.4s
    trn1 v28.4s, v12.4s, v13.4s
    trn2 v29.4s, v12.4s, v13.4s
    trn1 v30.4s, v14.4s, v15.4s
    trn2 v31.4s, v14.4s, v15.4s

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7
    sub v1.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 - (d4 + d5 + d6 + d7)
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    trn1 v16.2d, v0.2d, v1.2d       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7
    trn2 v17.2d, v0.2d, v1.2d       //d8 + d9 + d10 + d11 + d12 + d13 + d14 + d15
    trn1 v18.2d, v2.2d, v3.2d
    trn2 v19.2d, v2.2d, v3.2d
    trn1 v20.2d, v4.2d, v5.2d
    trn2 v21.2d, v4.2d, v5.2d
    trn1 v22.2d, v6.2d, v7.2d
    trn2 v23.2d, v6.2d, v7.2d
    trn1 v24.2d, v8.2d, v9.2d
    trn2 v25.2d, v8.2d, v9.2d
    trn1 v26.2d, v10.2d, v11.2d
    trn2 v27.2d, v10.2d, v11.2d
    trn1 v28.2d, v12.2d, v13.2d
    trn2 v29.2d, v12.2d, v13.2d
    trn1 v30.2d, v14.2d, v15.2d
    trn2 v31.2d, v14.2d, v15.2d

    add v0.8h, v16.8h, v17.8h       //d0 + d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10 + d11 + d12 + d13 + d14 + d15
    sub v1.8h, v16.8h, v17.8h
    add v2.8h, v18.8h, v19.8h
    sub v3.8h, v18.8h, v19.8h
    add v4.8h, v20.8h, v21.8h
    sub v5.8h, v20.8h, v21.8h
    add v6.8h, v22.8h, v23.8h
    sub v7.8h, v22.8h, v23.8h
    add v8.8h, v24.8h, v25.8h
    sub v9.8h, v24.8h, v25.8h
    add v10.8h, v26.8h, v27.8h
    sub v11.8h, v26.8h, v27.8h
    add v12.8h, v28.8h, v29.8h
    sub v13.8h, v28.8h, v29.8h
    add v14.8h, v30.8h, v31.8h
    sub v15.8h, v30.8h, v31.8h

    add v16.8h, v0.8h, v8.8h
    add v17.8h, v1.8h, v9.8h
    add v18.8h, v2.8h, v10.8h
    add v19.8h, v3.8h, v11.8h
    add v20.8h, v4.8h, v12.8h
    add v21.8h, v5.8h, v13.8h
    add v22.8h, v6.8h, v14.8h
    add v23.8h, v7.8h, v15.8h
    sub v24.8h, v0.8h, v8.8h
    sub v25.8h, v1.8h, v9.8h
    sub v26.8h, v2.8h, v10.8h
    sub v27.8h, v3.8h, v11.8h
    sub v28.8h, v4.8h, v12.8h
    sub v29.8h, v5.8h, v13.8h
    sub v30.8h, v6.8h, v14.8h
    sub v31.8h, v7.8h, v15.8h

    saddl v0.4s, v16.4h, v20.4h
    saddl2 v1.4s, v16.8h, v20.8h
    saddl v2.4s, v17.4h, v21.4h
    saddl2 v3.4s, v17.8h, v21.8h
    saddl v4.4s, v18.4h, v22.4h
    saddl2 v5.4s, v18.8h, v22.8h
    saddl v6.4s, v19.4h, v23.4h
    saddl2 v7.4s, v19.8h, v23.8h
    ssubl v8.4s, v16.4h, v20.4h
    ssubl2 v9.4s, v16.8h, v20.8h
    ssubl v10.4s, v17.4h, v21.4h
    ssubl2 v11.4s, v17.8h, v21.8h
    ssubl v12.4s, v18.4h, v22.4h
    ssubl2 v13.4s, v18.8h, v22.8h
    ssubl v14.4s, v19.4h, v23.4h
    ssubl2 v15.4s, v19.8h, v23.8h
    saddl v16.4s, v24.4h, v28.4h
    saddl2 v17.4s, v24.8h, v28.8h
    saddl v18.4s, v25.4h, v29.4h
    saddl2 v19.4s, v25.8h, v29.8h
    saddl v20.4s, v26.4h, v30.4h
    saddl2 v21.4s, v26.8h, v30.8h
    saddl v22.4s, v27.4h, v31.4h
    saddl2 v23.4s, v27.8h, v31.8h
    sub sp, sp, #64
    st1 {v20.4s, v21.4s, v22.4s, v23.4s}, [sp]
    ssubl v20.4s, v24.4h, v28.4h
    ssubl2 v21.4s, v24.8h, v28.8h
    ssubl v22.4s, v25.4h, v29.4h
    ssubl2 v23.4s, v25.8h, v29.8h
    ssubl v24.4s, v26.4h, v30.4h
    ssubl2 v25.4s, v26.8h, v30.8h
    ssubl v26.4s, v27.4h, v31.4h
    ssubl2 v27.4s, v27.8h, v31.8h
    mov v31.16b, v27.16b
    mov v30.16b, v26.16b
    mov v29.16b, v25.16b
    mov v28.16b, v24.16b
    mov v27.16b, v23.16b
    mov v26.16b, v22.16b
    mov v25.16b, v21.16b
    mov v24.16b, v20.16b

    sub sp, sp, #64
    st1 {v24.4s, v25.4s, v26.4s, v27.4s}, [sp]
    add v20.4s, v0.4s, v4.4s
    add v21.4s, v1.4s, v5.4s
    add v22.4s, v2.4s, v6.4s
    add v23.4s, v3.4s, v7.4s
    sub v24.4s, v0.4s, v4.4s
    sub v25.4s, v1.4s, v5.4s
    sub v26.4s, v2.4s, v6.4s
    sub v27.4s, v3.4s, v7.4s
    mov v0.16b, v20.16b
    mov v1.16b, v21.16b
    mov v2.16b, v22.16b
    mov v3.16b, v23.16b
    mov v4.16b, v24.16b
    mov v5.16b, v25.16b
    mov v6.16b, v26.16b
    mov v7.16b, v27.16b
    ld1 {v24.4s, v25.4s, v26.4s, v27.4s}, [sp], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [sp]
    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [sp]
    sub sp, sp, #64
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [sp]
    add v0.4s, v8.4s, v12.4s
    add v1.4s, v9.4s, v13.4s
    add v2.4s, v10.4s, v14.4s
    add v3.4s, v11.4s, v15.4s
    sub v4.4s, v8.4s, v12.4s
    sub v5.4s, v9.4s, v13.4s
    sub v6.4s, v10.4s, v14.4s
    sub v7.4s, v11.4s, v15.4s
    mov v8.16b, v0.16b
    mov v9.16b, v1.16b
    mov v10.16b, v2.16b
    mov v11.16b, v3.16b
    mov v12.16b, v4.16b
    mov v13.16b, v5.16b
    mov v14.16b, v6.16b
    mov v15.16b, v7.16b
    add v0.4s, v16.4s, v20.4s
    add v1.4s, v17.4s, v21.4s
    add v2.4s, v18.4s, v22.4s
    add v3.4s, v19.4s, v23.4s
    sub v4.4s, v16.4s, v20.4s
    sub v5.4s, v17.4s, v21.4s
    sub v6.4s, v18.4s, v22.4s
    sub v7.4s, v19.4s, v23.4s
    mov v16.16b, v0.16b
    mov v17.16b, v1.16b
    mov v18.16b, v2.16b
    mov v19.16b, v3.16b
    mov v20.16b, v4.16b
    mov v21.16b, v5.16b
    mov v22.16b, v6.16b
    mov v23.16b, v7.16b
    add v0.4s, v24.4s, v28.4s
    add v1.4s, v25.4s, v29.4s
    add v2.4s, v26.4s, v30.4s
    add v3.4s, v27.4s, v31.4s
    sub v4.4s, v24.4s, v28.4s
    sub v5.4s, v25.4s, v29.4s
    sub v6.4s, v26.4s, v30.4s
    sub v7.4s, v27.4s, v31.4s
    mov v24.16b, v0.16b
    mov v25.16b, v1.16b
    mov v26.16b, v2.16b
    mov v27.16b, v3.16b
    mov v28.16b, v4.16b
    mov v29.16b, v5.16b
    mov v30.16b, v6.16b
    mov v31.16b, v7.16b
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [sp], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [sp], #64

    abs v0.4s, v0.4s
    abs v1.4s, v1.4s
    abs v2.4s, v2.4s
    abs v3.4s, v3.4s
    abs v4.4s, v4.4s
    abs v5.4s, v5.4s
    abs v6.4s, v6.4s
    abs v7.4s, v7.4s
    abs v8.4s, v8.4s
    abs v9.4s, v9.4s
    abs v10.4s, v10.4s
    abs v11.4s, v11.4s
    abs v12.4s, v12.4s
    abs v13.4s, v13.4s
    abs v14.4s, v14.4s
    abs v15.4s, v15.4s
    abs v16.4s, v16.4s
    abs v17.4s, v17.4s
    abs v18.4s, v18.4s
    abs v19.4s, v19.4s
    abs v20.4s, v20.4s
    abs v21.4s, v21.4s
    abs v22.4s, v22.4s
    abs v23.4s, v23.4s
    abs v24.4s, v24.4s
    abs v25.4s, v25.4s
    abs v26.4s, v26.4s
    abs v27.4s, v27.4s
    abs v28.4s, v28.4s
    abs v29.4s, v29.4s
    abs v30.4s, v30.4s
    abs v31.4s, v31.4s

    add v0.4s, v0.4s, v1.4s
    add v2.4s, v2.4s, v3.4s
    add v4.4s, v4.4s, v5.4s
    add v6.4s, v6.4s, v7.4s
    add v8.4s, v8.4s, v9.4s
    add v10.4s, v10.4s, v11.4s
    add v12.4s, v12.4s, v13.4s
    add v14.4s, v14.4s, v15.4s
    add v16.4s, v16.4s, v17.4s
    add v18.4s, v18.4s, v19.4s
    add v20.4s, v20.4s, v21.4s
    add v22.4s, v22.4s, v23.4s
    add v24.4s, v24.4s, v25.4s
    add v26.4s, v26.4s, v27.4s
    add v28.4s, v28.4s, v29.4s
    add v30.4s, v30.4s, v31.4s
    add v0.4s, v0.4s, v2.4s
    add v1.4s, v4.4s, v6.4s
    add v2.4s, v8.4s, v10.4s
    add v3.4s, v12.4s, v14.4s
    add v4.4s, v16.4s, v18.4s
    add v5.4s, v20.4s, v22.4s
    add v6.4s, v24.4s, v26.4s
    add v7.4s, v28.4s, v30.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v2.4s, v4.4s, v5.4s
    add v3.4s, v6.4s, v7.4s
    add v0.4s, v0.4s, v1.4s
    add v1.4s, v2.4s, v3.4s
    add v0.4s, v0.4s, v1.4s
    addp v0.4s, v0.4s, v0.4s
    addp v0.4s, v0.4s, v0.4s

    mov x1, #128
    ucvtf d1, x1
    fsqrt d1, d1
    mov x0, #0
    umov w0, v0.s[0]
    ucvtf d0, x0
    fdiv d0, d0, d1
    fmov d1, #2.0
    fmul d0, d0, d1
    fcvtms x0, d0

    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64
    ret
#endif
#endif
