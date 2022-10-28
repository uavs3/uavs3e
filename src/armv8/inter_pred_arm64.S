#include "def_arm64.S"
#if defined(__arm64__)

#if !COMPILE_10BIT

//void uavs3e_if_cpy_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w4_arm64

if_cpy_w4_loop_y:
    ld1 {v0.S}[0], [x0], x1
    ld1 {v0.S}[1], [x0], x1
    ld1 {v0.S}[2], [x0], x1
    ld1 {v0.S}[3], [x0], x1
    st1 {v0.S}[0], [x2], x3
    st1 {v0.S}[1], [x2], x3
    subs w5, w5, #4
    st1 {v0.S}[2], [x2], x3
    st1 {v0.S}[3], [x2], x3

    bgt if_cpy_w4_loop_y

    ret

//void uavs3e_if_cpy_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w8_arm64

if_cpy_w8_loop_y:
    ld1 {v0.D}[0], [x0], x1
    ld1 {v0.D}[1], [x0], x1
    ld1 {v1.D}[0], [x0], x1
    ld1 {v1.D}[1], [x0], x1
    subs w5, w5, #4
    st1 {v0.D}[0], [x2], x3
    st1 {v0.D}[1], [x2], x3
    st1 {v1.D}[0], [x2], x3
    st1 {v1.D}[1], [x2], x3
    bgt if_cpy_w8_loop_y

    ret

//void uavs3e_if_cpy_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w16_arm64

if_cpy_w16_loop_y:
    ld1 {v0.2D}, [x0], x1
    ld1 {v1.2D}, [x0], x1
    ld1 {v2.2D}, [x0], x1
    ld1 {v3.2D}, [x0], x1

    subs w5, w5, #4
    st1 {v0.2D}, [x2], x3
    st1 {v1.2D}, [x2], x3
    st1 {v2.2D}, [x2], x3
    st1 {v3.2D}, [x2], x3
    bgt if_cpy_w16_loop_y

    ret

//void uavs3e_if_cpy_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w32_arm64

if_cpy_w32_loop_y:
    ld1 {v0.2d, v1.2d}, [x0], x1
    ld1 {v2.2d, v3.2d}, [x0], x1
    ld1 {v4.2d, v5.2d}, [x0], x1
    ld1 {v6.2d, v7.2d}, [x0], x1

    subs w5, w5, #4
    st1 {v0.2d, v1.2d}, [x2], x3
    st1 {v2.2d, v3.2d}, [x2], x3
    st1 {v4.2d, v5.2d}, [x2], x3
    st1 {v6.2d, v7.2d}, [x2], x3
    bgt if_cpy_w32_loop_y

    ret

//void uavs3e_if_cpy_w64_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w64_arm64

if_cpy_w64_loop_y:
    ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], x1
    ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x0], x1
    ld1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x0], x1
    ld1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x0], x1

    subs w5, w5, #4
    st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x2], x3
    st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x2], x3
    st1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x2], x3
    st1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x2], x3
    bgt if_cpy_w64_loop_y

    ret

//void uavs3e_if_cpy_w128_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w128_arm64
    sub x1, x1, #64
    sub x3, x3, #64
if_cpy_w128_loop_y:
    ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #64
    ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x0], x1
    ld1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x0], #64
    ld1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x0], x1

    subs w5, w5, #2
    st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x2], #64
    st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x2], x3
    st1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x2], #64
    st1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x2], x3
    bgt if_cpy_w128_loop_y

    ret

//void uavs3e_if_hor_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_chroma_w8_arm64

    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, #1                  // src - 2

if_hor_chroma_w8_loop_y:
    ld1 {v20.8b, v21.8b}, [x0], x1  // src[x-2]
    ld1 {v22.8b, v23.8b}, [x0], x1
    ext v17.8b, v20.8b, v21.8b, #1  // src[x]
    ext v18.8b, v20.8b, v21.8b, #2  // src[x+1]
    ext v19.8b, v20.8b, v21.8b, #3  // src[x+2]
    ext v24.8b, v22.8b, v23.8b, #1  // src[x]
    ext v25.8b, v22.8b, v23.8b, #2  // src[x+1]
    ext v26.8b, v22.8b, v23.8b, #3  // src[x+2]

    ld1 {v28.8b, v29.8b}, [x0], x1
    ld1 {v30.8b, v31.8b}, [x0], x1

    umull v5.8h, v17.8b, v1.8b
    umlsl v5.8h, v20.8b, v0.8b
    umlal v5.8h, v18.8b, v2.8b
    umlsl v5.8h, v19.8b, v3.8b

    umull v6.8h, v24.8b, v1.8b
    umlsl v6.8h, v22.8b, v0.8b
    umlal v6.8h, v25.8b, v2.8b
    umlsl v6.8h, v26.8b, v3.8b

    sqrshrun v5.8b, v5.8h, #6
    sqrshrun v6.8b, v6.8h, #6

    ext v17.8b, v28.8b, v29.8b, #1   // src[x]
    ext v18.8b, v28.8b, v29.8b, #2   // src[x+1]
    ext v19.8b, v28.8b, v29.8b, #3   // src[x+2]
    ext v24.8b, v30.8b, v31.8b, #1   // src[x]
    ext v25.8b, v30.8b, v31.8b, #2   // src[x+1]
    ext v26.8b, v30.8b, v31.8b, #3   // src[x+2]

    st1 {v5.8b}, [x2], x3
    st1 {v6.8b}, [x2], x3

    umull v16.8h, v17.8b, v1.8b
    umlsl v16.8h, v28.8b, v0.8b
    umlal v16.8h, v18.8b, v2.8b
    umlsl v16.8h, v19.8b, v3.8b

    umull v17.8h, v24.8b, v1.8b
    umlsl v17.8h, v30.8b, v0.8b
    umlal v17.8h, v25.8b, v2.8b
    umlsl v17.8h, v26.8b, v3.8b

    sqrshrun v16.8b, v16.8h, #6
    sqrshrun v17.8b, v17.8h, #6
    subs w5, w5, #4
    st1 {v16.8b}, [x2], x3
    st1 {v17.8b}, [x2], x3

    bgt if_hor_chroma_w8_loop_y

    ret

//void uavs3e_if_hor_chroma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_chroma_w16_arm64

    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, #1                      // src - 2

if_hor_chroma_w16_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], x1    // src[x-2]
    ld1 {v22.16b, v23.16b}, [x0], x1
    ext v17.16b, v20.16b, v21.16b, #1   // src[x]
    ext v18.16b, v20.16b, v21.16b, #2   // src[x+1]
    ext v19.16b, v20.16b, v21.16b, #3   // src[x+2]
    ext v24.16b, v22.16b, v23.16b, #1   // src[x]
    ext v25.16b, v22.16b, v23.16b, #2   // src[x+1]
    ext v26.16b, v22.16b, v23.16b, #3   // src[x+2]

    umull  v27.8h, v17.8b, v1.8b
    umull2 v28.8h, v17.16b, v1.16b
    umull  v29.8h, v24.8b, v1.8b
    umull2 v30.8h, v24.16b, v1.16b
    umlsl  v27.8h, v20.8b, v0.8b
    umlsl2 v28.8h, v20.16b, v0.16b
    umlsl  v29.8h, v22.8b, v0.8b
    umlsl2 v30.8h, v22.16b, v0.16b
    umlal  v27.8h, v18.8b, v2.8b
    umlal2 v28.8h, v18.16b, v2.16b
    umlal  v29.8h, v25.8b, v2.8b
    umlal2 v30.8h, v25.16b, v2.16b
    umlsl  v27.8h, v19.8b, v3.8b
    umlsl2 v28.8h, v19.16b, v3.16b
    umlsl  v29.8h, v26.8b, v3.8b
    umlsl2 v30.8h, v26.16b, v3.16b

    sqrshrun v27.8b, v27.8h, #6
    sqrshrun v28.8b, v28.8h, #6
    sqrshrun v29.8b, v29.8h, #6
    sqrshrun v30.8b, v30.8h, #6

    subs w5, w5, #2
    st1 {v27.8b, v28.8b}, [x2], x3
    st1 {v29.8b, v30.8b}, [x2], x3

    //--------------------------------
    // loop control
    //--------------------------------
    bgt if_hor_chroma_w16_loop_y

    ret

//void uavs3e_if_hor_chroma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_chroma_w32_arm64

    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, #1                      // src - 2
    sub x1, x1, #32
if_hor_chroma_w32_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], #32   // src[x-2]
    ld1 {v22.16b}, [x0], x1
    ext v17.16b, v20.16b, v21.16b, #1   // src[x]
    ext v18.16b, v20.16b, v21.16b, #2   // src[x+1]
    ext v19.16b, v20.16b, v21.16b, #3   // src[x+2]
    ext v23.16b, v21.16b, v22.16b, #1   // src[x]
    ext v24.16b, v21.16b, v22.16b, #2   // src[x+1]
    ext v25.16b, v21.16b, v22.16b, #3   // src[x+2]

    umull  v4.8h, v17.8b, v1.8b
    umull2 v5.8h, v17.16b, v1.16b
    umlsl  v4.8h, v20.8b, v0.8b
    umlsl2 v5.8h, v20.16b, v0.16b
    umlal  v4.8h, v18.8b, v2.8b
    umlal2 v5.8h, v18.16b, v2.16b
    umlsl  v4.8h, v19.8b, v3.8b
    umlsl2 v5.8h, v19.16b, v3.16b

    umull  v6.8h, v23.8b, v1.8b
    umull2 v7.8h, v23.16b, v1.16b
    umlsl  v6.8h, v21.8b, v0.8b
    umlsl2 v7.8h, v21.16b, v0.16b
    umlal  v6.8h, v24.8b, v2.8b
    umlal2 v7.8h, v24.16b, v2.16b
    umlsl  v6.8h, v25.8b, v3.8b
    umlsl2 v7.8h, v25.16b, v3.16b

    sqrshrun v4.8b, v4.8h, #6
    sqrshrun v5.8b, v5.8h, #6
    sqrshrun v6.8b, v6.8h, #6
    sqrshrun v7.8b, v7.8h, #6

    subs w5, w5, #1
    st1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x2], x3
    bgt if_hor_chroma_w32_loop_y

    ret

//void uavs3e_if_hor_chroma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_chroma_w32x_arm64

    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, #1                      // src - 1
if_hor_chroma_w32x_loop_y:
    mov w9, w4
    mov x10, x0
    mov x11, x2
if_hor_chroma_w32x_loop_x:
    ld1 {v20.16b, v21.16b}, [x10], #32  // src[x-2]
    ld1 {v22.16b}, [x10]

    ext v17.16b, v20.16b, v21.16b, #1   // src[x]
    ext v18.16b, v20.16b, v21.16b, #2   // src[x+1]
    ext v19.16b, v20.16b, v21.16b, #3   // src[x+2]
    ext v23.16b, v21.16b, v22.16b, #1   // src[x]
    ext v24.16b, v21.16b, v22.16b, #2   // src[x+1]
    ext v25.16b, v21.16b, v22.16b, #3   // src[x+2]

    umull  v4.8h, v17.8b, v1.8b
    umull2 v5.8h, v17.16b, v1.16b
    umlsl  v4.8h, v20.8b, v0.8b
    umlsl2 v5.8h, v20.16b, v0.16b
    umlal  v4.8h, v18.8b, v2.8b
    umlal2 v5.8h, v18.16b, v2.16b
    umlsl  v4.8h, v19.8b, v3.8b
    umlsl2 v5.8h, v19.16b, v3.16b

    umull  v6.8h, v23.8b, v1.8b
    umull2 v7.8h, v23.16b, v1.16b
    umlsl  v6.8h, v21.8b, v0.8b
    umlsl2 v7.8h, v21.16b, v0.16b
    umlal  v6.8h, v24.8b, v2.8b
    umlal2 v7.8h, v24.16b, v2.16b
    umlsl  v6.8h, v25.8b, v3.8b
    umlsl2 v7.8h, v25.16b, v3.16b

    sqrshrun v4.8b, v4.8h, #6
    sqrshrun v5.8b, v5.8h, #6
    sqrshrun v6.8b, v6.8h, #6
    sqrshrun v7.8b, v7.8h, #6

    subs w9, w9, #32
    st1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x11], #32
    bgt if_hor_chroma_w32x_loop_x

    subs w5, w5, #1
    add x0, x0, x1
    add x2, x2, x3
    bgt if_hor_chroma_w32x_loop_y

    ret

//void uavs3e_if_hor_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w4_arm64

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                          // x-3
if_hor_luma_w4_loop_y:
    ld1 {v22.16b}, [x0], x1                 // src[x-3]
    ld1 {v23.16b}, [x0], x1
    ld1 {v24.16b}, [x0], x1                 // src[x-3]
    ld1 {v25.16b}, [x0], x1
    zip1 v20.16b, v22.16b, v23.16b
    zip2 v21.16b, v22.16b, v23.16b
    zip1 v22.16b, v24.16b, v25.16b
    zip2 v23.16b, v24.16b, v25.16b

    ext v24.16b, v20.16b, v21.16b, #2
    ext v25.16b, v20.16b, v21.16b, #4
    umull v17.8h, v24.8b, v1.8b
    ext v26.16b, v20.16b, v21.16b, #6
    umlsl v17.8h, v20.8b, v0.8b
    ext v27.16b, v20.16b, v21.16b, #8
    umlsl v17.8h, v25.8b, v2.8b
    ext v28.16b, v20.16b, v21.16b, #10
    umlal v17.8h, v26.8b, v3.8b
    ext v29.16b, v20.16b, v21.16b, #12
    umlal v17.8h, v27.8b, v4.8b
    ext v30.16b, v20.16b, v21.16b, #14
    umlsl v17.8h, v28.8b, v5.8b
    ext v24.16b, v22.16b, v23.16b, #2
    umlal v17.8h, v29.8b, v6.8b
    ext v25.16b, v22.16b, v23.16b, #4
    umull v18.8h, v24.8b, v1.8b
    umlsl v17.8h, v30.8b, v7.8b
    umlsl v18.8h, v22.8b, v0.8b
    ext v26.16b, v22.16b, v23.16b, #6
    umlsl v18.8h, v25.8b, v2.8b
    ext v27.16b, v22.16b, v23.16b, #8
    umlal v18.8h, v26.8b, v3.8b
    ext v28.16b, v22.16b, v23.16b, #10
    umlal v18.8h, v27.8b, v4.8b
    ext v29.16b, v22.16b, v23.16b, #12
    umlsl v18.8h, v28.8b, v5.8b
    ext v30.16b, v22.16b, v23.16b, #14
    umlal v18.8h, v29.8b, v6.8b
    sqrshrun v17.8b, v17.8h, #6
    umlsl v18.8h, v30.8b, v7.8b

    //(sum + 32) >> 6
    sqrshrun v18.8b, v18.8h, #6
    uzp1 v16.8b, v17.8b, v17.8b
    uzp2 v17.8b, v17.8b, v17.8b
    uzp1 v19.8b, v18.8b, v18.8b
    uzp2 v18.8b, v18.8b, v18.8b

    st1 {v16.s}[0], [x2], x3
    st1 {v17.s}[0], [x2], x3
    subs w5, w5, #4
    st1 {v19.s}[0], [x2], x3
    st1 {v18.s}[0], [x2], x3
    bgt if_hor_luma_w4_loop_y

    ret

//void uavs3e_if_hor_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w8_arm64

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b

    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                      // x-3
if_hor_luma_w8_loop_y:
    ld1 {v20.8b, v21.8b}, [x0], x1      // src[x-3]
    ld1 {v22.8b, v23.8b}, [x0], x1
    ext v24.8b, v20.8b, v21.8b, #1
    ext v25.8b, v20.8b, v21.8b, #2
    ext v26.8b, v20.8b, v21.8b, #3
    umull v17.8h, v24.8b, v1.8b
    ext v27.8b, v20.8b, v21.8b, #4
    umlsl v17.8h, v20.8b, v0.8b
    ext v28.8b, v20.8b, v21.8b, #5
    umlsl v17.8h, v25.8b, v2.8b
    ext v29.8b, v20.8b, v21.8b, #6
    umlal v17.8h, v26.8b, v3.8b
    ext v30.8b, v20.8b, v21.8b, #7
    umlal v17.8h, v27.8b, v4.8b
    ext v24.8b, v22.8b, v23.8b, #1
    umlsl v17.8h, v28.8b, v5.8b
    ext v25.8b, v22.8b, v23.8b, #2
    umull v18.8h, v24.8b, v1.8b
    umlal v17.8h, v29.8b, v6.8b
    umlsl v18.8h, v22.8b, v0.8b
    ext v26.8b, v22.8b, v23.8b, #3
    umlsl v18.8h, v25.8b, v2.8b
    umlsl v17.8h, v30.8b, v7.8b
    ext v27.8b, v22.8b, v23.8b, #4
    umlal v18.8h, v26.8b, v3.8b
    ext v28.8b, v22.8b, v23.8b, #5
    umlal v18.8h, v27.8b, v4.8b
    ext v29.8b, v22.8b, v23.8b, #6
    umlsl v18.8h, v28.8b, v5.8b
    ext v30.8b, v22.8b, v23.8b, #7
    umlal v18.8h, v29.8b, v6.8b
    sqrshrun v17.8b, v17.8h, #6
    umlsl v18.8h, v30.8b, v7.8b

    //(sum + 32) >> 6
    sqrshrun v18.8b, v18.8h, #6
    subs w5, w5, #2
    st1 {v17.8b}, [x2], x3
    st1 {v18.8b}, [x2], x3
    bgt if_hor_luma_w8_loop_y

    ret

//void uavs3e_if_hor_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w16_arm64

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b

    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                      // x-3
if_hor_luma_w16_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], x1    // src[x-3]
    ld1 {v22.16b, v23.16b}, [x0], x1

    ext v24.16b, v20.16b, v21.16b, #1
    ext v25.16b, v20.16b, v21.16b, #2
    ext v26.16b, v20.16b, v21.16b, #3
    ext v27.16b, v20.16b, v21.16b, #4
    ext v28.16b, v20.16b, v21.16b, #5
    ext v29.16b, v20.16b, v21.16b, #6
    ext v30.16b, v20.16b, v21.16b, #7

    umull  v17.8h, v24.8b, v1.8b
    umull2 v18.8h, v24.16b, v1.16b
    umlsl  v17.8h, v20.8b, v0.8b
    umlsl2 v18.8h, v20.16b, v0.16b
    umlsl  v17.8h, v25.8b, v2.8b
    umlsl2 v18.8h, v25.16b, v2.16b
    umlal  v17.8h, v26.8b, v3.8b
    umlal2 v18.8h, v26.16b, v3.16b
    umlal  v17.8h, v27.8b, v4.8b
    umlal2 v18.8h, v27.16b, v4.16b
    umlsl  v17.8h, v28.8b, v5.8b
    umlsl2 v18.8h, v28.16b, v5.16b
    umlal  v17.8h, v29.8b, v6.8b
    umlal2 v18.8h, v29.16b, v6.16b
    umlsl  v17.8h, v30.8b, v7.8b
    umlsl2 v18.8h, v30.16b, v7.16b

    ext v24.16b, v22.16b, v23.16b, #1
    ext v25.16b, v22.16b, v23.16b, #2
    ext v26.16b, v22.16b, v23.16b, #3
    ext v27.16b, v22.16b, v23.16b, #4
    ext v28.16b, v22.16b, v23.16b, #5
    ext v29.16b, v22.16b, v23.16b, #6
    ext v30.16b, v22.16b, v23.16b, #7

    //(sum + 32) >> 6
    sqrshrun v16.8b, v17.8h, #6
    sqrshrun v17.8b, v18.8h, #6

    umull  v18.8h, v24.8b, v1.8b
    umull2 v19.8h, v24.16b, v1.16b
    umlsl  v18.8h, v22.8b, v0.8b
    umlsl2 v19.8h, v22.16b, v0.16b
    umlsl  v18.8h, v25.8b, v2.8b
    umlsl2 v19.8h, v25.16b, v2.16b
    umlal  v18.8h, v26.8b, v3.8b
    umlal2 v19.8h, v26.16b, v3.16b
    umlal  v18.8h, v27.8b, v4.8b
    umlal2 v19.8h, v27.16b, v4.16b
    umlsl  v18.8h, v28.8b, v5.8b
    umlsl2 v19.8h, v28.16b, v5.16b
    umlal  v18.8h, v29.8b, v6.8b
    umlal2 v19.8h, v29.16b, v6.16b
    umlsl  v18.8h, v30.8b, v7.8b
    umlsl2 v19.8h, v30.16b, v7.16b

    st1 {v16.8b, v17.8b}, [x2], x3

    sqrshrun v18.8b, v18.8h, #6
    sqrshrun v19.8b, v19.8h, #6

    subs w5, w5, #2
    st1 {v18.8b, v19.8b}, [x2], x3
    bgt if_hor_luma_w16_loop_y

    ret

//void uavs3e_if_hor_luma_w32_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w32_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b

    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                          // x-3
    sub x1, x1, #32
if_hor_luma_w32_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], #32       // src[x-3]
    ld1 {v29.16b}, [x0], x1

    ext v22.16b, v20.16b, v21.16b, #1
    ext v23.16b, v20.16b, v21.16b, #2
    ext v24.16b, v20.16b, v21.16b, #3
    ext v25.16b, v20.16b, v21.16b, #4
    ext v26.16b, v20.16b, v21.16b, #5
    ext v27.16b, v20.16b, v21.16b, #6
    ext v28.16b, v20.16b, v21.16b, #7

    ext v12.16b, v21.16b, v29.16b, #1
    ext v13.16b, v21.16b, v29.16b, #2
    ext v14.16b, v21.16b, v29.16b, #3
    ext v15.16b, v21.16b, v29.16b, #4
    ext v16.16b, v21.16b, v29.16b, #5
    ext v17.16b, v21.16b, v29.16b, #6
    ext v18.16b, v21.16b, v29.16b, #7

    umull  v19.8h, v22.8b, v1.8b
    umull2 v29.8h, v22.16b, v1.16b
    umlsl  v19.8h, v20.8b, v0.8b
    umlsl2 v29.8h, v20.16b, v0.16b
    umlsl  v19.8h, v23.8b, v2.8b
    umlsl2 v29.8h, v23.16b, v2.16b
    umlal  v19.8h, v24.8b, v3.8b
    umlal2 v29.8h, v24.16b, v3.16b
    umlal  v19.8h, v25.8b, v4.8b
    umlal2 v29.8h, v25.16b, v4.16b
    umlsl  v19.8h, v26.8b, v5.8b
    umlsl2 v29.8h, v26.16b, v5.16b
    umlal  v19.8h, v27.8b, v6.8b
    umlal2 v29.8h, v27.16b, v6.16b
    umlsl  v19.8h, v28.8b, v7.8b
    umlsl2 v29.8h, v28.16b, v7.16b

    umull  v30.8h, v12.8b, v1.8b
    umull2 v31.8h, v12.16b, v1.16b
    umlsl  v30.8h, v21.8b, v0.8b
    umlsl2 v31.8h, v21.16b, v0.16b
    umlsl  v30.8h, v13.8b, v2.8b
    umlsl2 v31.8h, v13.16b, v2.16b
    umlal  v30.8h, v14.8b, v3.8b
    umlal2 v31.8h, v14.16b, v3.16b
    umlal  v30.8h, v15.8b, v4.8b
    umlal2 v31.8h, v15.16b, v4.16b
    umlsl  v30.8h, v16.8b, v5.8b
    umlsl2 v31.8h, v16.16b, v5.16b
    umlal  v30.8h, v17.8b, v6.8b
    umlal2 v31.8h, v17.16b, v6.16b
    umlsl  v30.8h, v18.8b, v7.8b
    umlsl2 v31.8h, v18.16b, v7.16b

    //(sum + 32) >> 6
    sqrshrun v28.8b, v19.8h, #6
    sqrshrun v29.8b, v29.8h, #6
    sqrshrun v30.8b, v30.8h, #6
    sqrshrun v31.8b, v31.8h, #6

    subs w5, w5, #1
    st1 {v28.8b, v29.8b, v30.8b, v31.8b}, [x2], x3
    bgt if_hor_luma_w32_loop_y

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
    ret

//void uavs3e_if_hor_luma_w32x_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w32x_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b

    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                      // x-3
if_hor_luma_w32x_loop_y:
    mov w9, w4
    mov x10, x0
    mov x11, x2
if_hor_luma_w32x_loop_x:
    ld1 {v20.16b, v21.16b}, [x10], #32  // src[x-3]
    ld1 {v29.16b}, [x10]

    ext v22.16b, v20.16b, v21.16b, #1
    ext v23.16b, v20.16b, v21.16b, #2
    ext v24.16b, v20.16b, v21.16b, #3
    ext v25.16b, v20.16b, v21.16b, #4
    ext v26.16b, v20.16b, v21.16b, #5
    ext v27.16b, v20.16b, v21.16b, #6
    ext v28.16b, v20.16b, v21.16b, #7

    ext v12.16b, v21.16b, v29.16b, #1
    ext v13.16b, v21.16b, v29.16b, #2
    ext v14.16b, v21.16b, v29.16b, #3
    ext v15.16b, v21.16b, v29.16b, #4
    ext v16.16b, v21.16b, v29.16b, #5
    ext v17.16b, v21.16b, v29.16b, #6
    ext v18.16b, v21.16b, v29.16b, #7

    umull  v19.8h, v22.8b, v1.8b
    umull2 v29.8h, v22.16b, v1.16b
    umlsl  v19.8h, v20.8b, v0.8b
    umlsl2 v29.8h, v20.16b, v0.16b
    umlsl  v19.8h, v23.8b, v2.8b
    umlsl2 v29.8h, v23.16b, v2.16b
    umlal  v19.8h, v24.8b, v3.8b
    umlal2 v29.8h, v24.16b, v3.16b
    umlal  v19.8h, v25.8b, v4.8b
    umlal2 v29.8h, v25.16b, v4.16b
    umlsl  v19.8h, v26.8b, v5.8b
    umlsl2 v29.8h, v26.16b, v5.16b
    umlal  v19.8h, v27.8b, v6.8b
    umlal2 v29.8h, v27.16b, v6.16b
    umlsl  v19.8h, v28.8b, v7.8b
    umlsl2 v29.8h, v28.16b, v7.16b

    umull  v30.8h, v12.8b, v1.8b
    umull2 v31.8h, v12.16b, v1.16b
    umlsl  v30.8h, v21.8b, v0.8b
    umlsl2 v31.8h, v21.16b, v0.16b
    umlsl  v30.8h, v13.8b, v2.8b
    umlsl2 v31.8h, v13.16b, v2.16b
    umlal  v30.8h, v14.8b, v3.8b
    umlal2 v31.8h, v14.16b, v3.16b
    umlal  v30.8h, v15.8b, v4.8b
    umlal2 v31.8h, v15.16b, v4.16b
    umlsl  v30.8h, v16.8b, v5.8b
    umlsl2 v31.8h, v16.16b, v5.16b
    umlal  v30.8h, v17.8b, v6.8b
    umlal2 v31.8h, v17.16b, v6.16b
    umlsl  v30.8h, v18.8b, v7.8b
    umlsl2 v31.8h, v18.16b, v7.16b

    //(sum + 32) >> 6
    sqrshrun v28.8b, v19.8h, #6
    sqrshrun v29.8b, v29.8h, #6
    sqrshrun v30.8b, v30.8h, #6
    sqrshrun v31.8b, v31.8h, #6

    subs w9, w9, #32
    st1 {v28.8b, v29.8b, v30.8b, v31.8b}, [x11], #32
    bgt if_hor_luma_w32x_loop_x

    subs w5, w5, #1
    add x0, x0, x1
    add x2, x2, x3
    bgt if_hor_luma_w32x_loop_y

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
    ret

//void uavs3e_if_ver_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w8_arm64
    // load coeff
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, x1                  // src - i_src
if_ver_chroma_w8_loop_y:
    ld1 {v16.8b}, [x0], x1          // src-i_src
    ld1 {v17.8b}, [x0], x1          // src
    ld1 {v18.8b}, [x0], x1          // src+i_src
    ld1 {v19.8b}, [x0], x1          // src+2*i_src
    mov x10, x0
    ld1 {v20.8b}, [x0], x1
    ld1 {v21.8b}, [x0], x1
    ld1 {v22.8b}, [x0], x1

    umull v24.8h, v17.8b, v1.8b
    umull v25.8h, v18.8b, v1.8b
    umlsl v24.8h, v16.8b, v0.8b
    umlsl v25.8h, v17.8b, v0.8b
    umlal v24.8h, v18.8b, v2.8b
    umlal v25.8h, v19.8b, v2.8b
    umlsl v24.8h, v19.8b, v3.8b
    umlsl v25.8h, v20.8b, v3.8b

    umull v26.8h, v19.8b, v1.8b
    umull v27.8h, v20.8b, v1.8b
    umlsl v26.8h, v18.8b, v0.8b
    umlsl v27.8h, v19.8b, v0.8b
    umlal v26.8h, v20.8b, v2.8b
    umlal v27.8h, v21.8b, v2.8b
    umlsl v26.8h, v21.8b, v3.8b
    umlsl v27.8h, v22.8b, v3.8b

    //(sum + 32) >> 6
    sqrshrun v24.8b, v24.8h, #6
    sqrshrun v25.8b, v25.8h, #6
    sqrshrun v26.8b, v26.8h, #6
    sqrshrun v27.8b, v27.8h, #6

    st1 {v24.8b}, [x2], x3
    subs w5, w5, #4
    mov  x0, x10
    st1 {v25.8b}, [x2], x3
    st1 {v26.8b}, [x2], x3
    st1 {v27.8b}, [x2], x3
    bgt if_ver_chroma_w8_loop_y

    ret

//void uavs3e_if_ver_chroma_w16_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w16_arm64

    // load coeff
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, x1
if_ver_chroma_w16_loop_y:
    ld1 {v16.16b}, [x0], x1         // src-i_src
    ld1 {v17.16b}, [x0], x1         // src
    ld1 {v18.16b}, [x0], x1         // src+i_src
    ld1 {v19.16b}, [x0], x1         // src+2*i_src
    mov x10, x0
    ld1 {v20.16b}, [x0], x1
    ld1 {v21.16b}, [x0], x1
    ld1 {v31.16b}, [x0]

    umull  v22.8h, v17.8b, v1.8b
    umull2 v23.8h, v17.16b, v1.16b
    umull  v24.8h, v18.8b, v1.8b
    umull2 v25.8h, v18.16b, v1.16b
    umlsl  v22.8h, v16.8b, v0.8b
    umlsl2 v23.8h, v16.16b, v0.16b
    umlsl  v24.8h, v17.8b, v0.8b
    umlsl2 v25.8h, v17.16b, v0.16b
    umlal  v22.8h, v18.8b, v2.8b
    umlal2 v23.8h, v18.16b, v2.16b
    umlal  v24.8h, v19.8b, v2.8b
    umlal2 v25.8h, v19.16b, v2.16b
    umlsl  v22.8h, v19.8b, v3.8b
    umlsl2 v23.8h, v19.16b, v3.16b
    umlsl  v24.8h, v20.8b, v3.8b
    umlsl2 v25.8h, v20.16b, v3.16b

    umull  v26.8h, v19.8b, v1.8b
    umull2 v27.8h, v19.16b, v1.16b
    umull  v28.8h, v20.8b, v1.8b
    umull2 v29.8h, v20.16b, v1.16b
    umlsl  v26.8h, v18.8b, v0.8b
    umlsl2 v27.8h, v18.16b, v0.16b
    umlsl  v28.8h, v19.8b, v0.8b
    umlsl2 v29.8h, v19.16b, v0.16b
    umlal  v26.8h, v20.8b, v2.8b
    umlal2 v27.8h, v20.16b, v2.16b
    umlal  v28.8h, v21.8b, v2.8b
    umlal2 v29.8h, v21.16b, v2.16b
    umlsl  v26.8h, v21.8b, v3.8b
    umlsl2 v27.8h, v21.16b, v3.16b
    umlsl  v28.8h, v31.8b, v3.8b
    umlsl2 v29.8h, v31.16b, v3.16b

    //(sum + 32) >> 6
    sqrshrun v22.8b, v22.8h, #6
    sqrshrun v23.8b, v23.8h, #6
    sqrshrun v24.8b, v24.8h, #6
    sqrshrun v25.8b, v25.8h, #6
    sqrshrun v26.8b, v26.8h, #6
    sqrshrun v27.8b, v27.8h, #6
    sqrshrun v28.8b, v28.8h, #6
    sqrshrun v29.8b, v29.8h, #6

    subs w5, w5, #4
    mov x0, x10
    st1 {v22.8b, v23.8b}, [x2], x3
    st1 {v24.8b, v25.8b}, [x2], x3
    st1 {v26.8b, v27.8b}, [x2], x3
    st1 {v28.8b, v29.8b}, [x2], x3
    bgt if_ver_chroma_w16_loop_y

    ret

//void uavs3e_if_ver_chroma_w32_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w32_arm64

    // load coeff
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, x1
if_ver_chroma_w32_loop_y:
    ld1 {v16.16b, v17.16b}, [x0], x1        // src + x - i_src
    ld1 {v18.16b, v19.16b}, [x0], x1        // src + x
    mov x10, x0
    ld1 {v20.16b, v21.16b}, [x0], x1        // src + x + i_src
    ld1 {v22.16b, v23.16b}, [x0], x1        // src + x + 2*i_src
    ld1 {v4.16b, v5.16b}, [x0]

    umull  v24.8h, v18.8b, v1.8b
    umull2 v25.8h, v18.16b, v1.16b
    umull  v26.8h, v19.8b, v1.8b
    umull2 v27.8h, v19.16b, v1.16b
    umlsl  v24.8h, v16.8b, v0.8b
    umlsl2 v25.8h, v16.16b, v0.16b
    umlsl  v26.8h, v17.8b, v0.8b
    umlsl2 v27.8h, v17.16b, v0.16b
    umlal  v24.8h, v20.8b, v2.8b
    umlal2 v25.8h, v20.16b, v2.16b
    umlal  v26.8h, v21.8b, v2.8b
    umlal2 v27.8h, v21.16b, v2.16b
    umlsl  v24.8h, v22.8b, v3.8b
    umlsl2 v25.8h, v22.16b, v3.16b
    umlsl  v26.8h, v23.8b, v3.8b
    umlsl2 v27.8h, v23.16b, v3.16b

    umull  v28.8h, v20.8b, v1.8b
    umull2 v29.8h, v20.16b, v1.16b
    umull  v30.8h, v21.8b, v1.8b
    umull2 v31.8h, v21.16b, v1.16b
    umlsl  v28.8h, v18.8b, v0.8b
    umlsl2 v29.8h, v18.16b, v0.16b
    umlsl  v30.8h, v19.8b, v0.8b
    umlsl2 v31.8h, v19.16b, v0.16b
    umlal  v28.8h, v22.8b, v2.8b
    umlal2 v29.8h, v22.16b, v2.16b
    umlal  v30.8h, v23.8b, v2.8b
    umlal2 v31.8h, v23.16b, v2.16b
    umlsl  v28.8h, v4.8b, v3.8b
    umlsl2 v29.8h, v4.16b, v3.16b
    umlsl  v30.8h, v5.8b, v3.8b
    umlsl2 v31.8h, v5.16b, v3.16b

    //(sum + 32) >> 6
    sqrshrun v24.8b, v24.8h, #6
    sqrshrun v25.8b, v25.8h, #6
    sqrshrun v26.8b, v26.8h, #6
    sqrshrun v27.8b, v27.8h, #6
    sqrshrun v28.8b, v28.8h, #6
    sqrshrun v29.8b, v29.8h, #6
    sqrshrun v30.8b, v30.8h, #6
    sqrshrun v31.8b, v31.8h, #6

    mov x0, x10
    subs w5, w5, #2
    st1 {v24.8b, v25.8b, v26.8b, v27.8b}, [x2], x3
    st1 {v28.8b, v29.8b, v30.8b, v31.8b}, [x2], x3
    bgt if_ver_chroma_w32_loop_y

    ret

//void uavs3e_if_ver_chroma_w64_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w64_arm64

    // load coeff
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, x1
    sub x3, x3, #32
if_ver_chroma_w64_loop_y:
    ld1 {v16.16b, v17.16b, v18.16b, v19.16b}, [x0], x1  // src - i_src
    mov x10, x0
    ld1 {v20.16b, v21.16b, v22.16b, v23.16b}, [x0], x1  // src
    ld1 {v24.16b, v25.16b, v26.16b, v27.16b}, [x0], x1  // src + i_src
    ld1 {v28.16b, v29.16b, v30.16b, v31.16b}, [x0]      // src + 2*i_src

    umull  v4.8h, v20.8b, v1.8b
    umull2 v5.8h, v20.16b, v1.16b
    umull  v6.8h, v21.8b, v1.8b
    umull2 v7.8h, v21.16b, v1.16b
    umlsl  v4.8h, v16.8b, v0.8b
    umlsl2 v5.8h, v16.16b, v0.16b
    umlsl  v6.8h, v17.8b, v0.8b
    umlsl2 v7.8h, v17.16b, v0.16b
    umlal  v4.8h, v24.8b, v2.8b
    umlal2 v5.8h, v24.16b, v2.16b
    umlal  v6.8h, v25.8b, v2.8b
    umlal2 v7.8h, v25.16b, v2.16b
    umlsl  v4.8h, v28.8b, v3.8b
    umlsl2 v5.8h, v28.16b, v3.16b
    umlsl  v6.8h, v29.8b, v3.8b
    umlsl2 v7.8h, v29.16b, v3.16b

    umull  v16.8h, v22.8b, v1.8b
    umull2 v17.8h, v22.16b, v1.16b
    umull  v20.8h, v23.8b, v1.8b
    umull2 v21.8h, v23.16b, v1.16b
    umlsl  v16.8h, v18.8b, v0.8b
    umlsl2 v17.8h, v18.16b, v0.16b
    umlsl  v20.8h, v19.8b, v0.8b
    umlsl2 v21.8h, v19.16b, v0.16b
    umlal  v16.8h, v26.8b, v2.8b
    umlal2 v17.8h, v26.16b, v2.16b
    umlal  v20.8h, v27.8b, v2.8b
    umlal2 v21.8h, v27.16b, v2.16b
    umlsl  v16.8h, v30.8b, v3.8b
    umlsl2 v17.8h, v30.16b, v3.16b
    umlsl  v20.8h, v31.8b, v3.8b
    umlsl2 v21.8h, v31.16b, v3.16b

    //(sum + 32) >> 6
    sqrshrun v4.8b, v4.8h, #6
    sqrshrun v5.8b, v5.8h, #6
    sqrshrun v6.8b, v6.8h, #6
    sqrshrun v7.8b, v7.8h, #6
    sqrshrun v24.8b, v16.8h, #6
    sqrshrun v25.8b, v17.8h, #6
    sqrshrun v26.8b, v20.8h, #6
    sqrshrun v27.8b, v21.8h, #6

    subs w5, w5, #1
    mov x0, x10     // src += i_src
    st1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x2], #32
    st1 {v24.8b, v25.8b, v26.8b, v27.8b}, [x2], x3
    bgt if_ver_chroma_w64_loop_y

    ret

//void uavs3e_if_ver_chroma_w128_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w128_arm64
    // load coeff
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]

    sub x0, x0, x1
    sub x3, x3, #96
if_ver_chroma_w128_loop_y:
    add x10, x0, #64
    ld1 {v16.16b, v17.16b, v18.16b, v19.16b}, [x0], x1  // src - i_src
    ld1 {v20.16b, v21.16b, v22.16b, v23.16b}, [x0], x1  // src
    ld1 {v24.16b, v25.16b, v26.16b, v27.16b}, [x0], x1  // src + i_src
    ld1 {v28.16b, v29.16b, v30.16b, v31.16b}, [x0]      // src + 2*i_src

    umull  v4.8h, v20.8b, v1.8b
    umull2 v5.8h, v20.16b, v1.16b
    umull  v6.8h, v21.8b, v1.8b
    umull2 v7.8h, v21.16b, v1.16b
    umlsl  v4.8h, v16.8b, v0.8b
    umlsl2 v5.8h, v16.16b, v0.16b
    umlsl  v6.8h, v17.8b, v0.8b
    umlsl2 v7.8h, v17.16b, v0.16b
    umlal  v4.8h, v24.8b, v2.8b
    umlal2 v5.8h, v24.16b, v2.16b
    umlal  v6.8h, v25.8b, v2.8b
    umlal2 v7.8h, v25.16b, v2.16b
    umlsl  v4.8h, v28.8b, v3.8b
    umlsl2 v5.8h, v28.16b, v3.16b
    umlsl  v6.8h, v29.8b, v3.8b
    umlsl2 v7.8h, v29.16b, v3.16b

    umull  v16.8h, v22.8b, v1.8b
    umull2 v17.8h, v22.16b, v1.16b
    umull  v20.8h, v23.8b, v1.8b
    umull2 v21.8h, v23.16b, v1.16b
    umlsl  v16.8h, v18.8b, v0.8b
    umlsl2 v17.8h, v18.16b, v0.16b
    umlsl  v20.8h, v19.8b, v0.8b
    umlsl2 v21.8h, v19.16b, v0.16b
    umlal  v16.8h, v26.8b, v2.8b
    umlal2 v17.8h, v26.16b, v2.16b
    umlal  v20.8h, v27.8b, v2.8b
    umlal2 v21.8h, v27.16b, v2.16b
    umlsl  v16.8h, v30.8b, v3.8b
    umlsl2 v17.8h, v30.16b, v3.16b
    umlsl  v20.8h, v31.8b, v3.8b
    umlsl2 v21.8h, v31.16b, v3.16b

    //(sum + 32) >> 6
    sqrshrun v4.8b, v4.8h, #6
    sqrshrun v5.8b, v5.8h, #6
    sqrshrun v6.8b, v6.8h, #6
    sqrshrun v7.8b, v7.8h, #6
    sqrshrun v24.8b, v16.8h, #6
    sqrshrun v25.8b, v17.8h, #6
    sqrshrun v26.8b, v20.8h, #6
    sqrshrun v27.8b, v21.8h, #6

    st1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x2], #32
    st1 {v24.8b, v25.8b, v26.8b, v27.8b}, [x2], #32

    ld1 {v16.16b, v17.16b, v18.16b, v19.16b}, [x10], x1 // src - i_src
    mov x0, x10
    ld1 {v20.16b, v21.16b, v22.16b, v23.16b}, [x10], x1
    ld1 {v24.16b, v25.16b, v26.16b, v27.16b}, [x10], x1
    ld1 {v28.16b, v29.16b, v30.16b, v31.16b}, [x10]
    sub x0, x0, #64

    umull  v4.8h, v20.8b, v1.8b
    umull2 v5.8h, v20.16b, v1.16b
    umull  v6.8h, v21.8b, v1.8b
    umull2 v7.8h, v21.16b, v1.16b
    umlsl  v4.8h, v16.8b, v0.8b
    umlsl2 v5.8h, v16.16b, v0.16b
    umlsl  v6.8h, v17.8b, v0.8b
    umlsl2 v7.8h, v17.16b, v0.16b
    umlal  v4.8h, v24.8b, v2.8b
    umlal2 v5.8h, v24.16b, v2.16b
    umlal  v6.8h, v25.8b, v2.8b
    umlal2 v7.8h, v25.16b, v2.16b
    umlsl  v4.8h, v28.8b, v3.8b
    umlsl2 v5.8h, v28.16b, v3.16b
    umlsl  v6.8h, v29.8b, v3.8b
    umlsl2 v7.8h, v29.16b, v3.16b

    umull  v16.8h, v22.8b, v1.8b
    umull2 v17.8h, v22.16b, v1.16b
    umull  v20.8h, v23.8b, v1.8b
    umull2 v21.8h, v23.16b, v1.16b
    umlsl  v16.8h, v18.8b, v0.8b
    umlsl2 v17.8h, v18.16b, v0.16b
    umlsl  v20.8h, v19.8b, v0.8b
    umlsl2 v21.8h, v19.16b, v0.16b
    umlal  v16.8h, v26.8b, v2.8b
    umlal2 v17.8h, v26.16b, v2.16b
    umlal  v20.8h, v27.8b, v2.8b
    umlal2 v21.8h, v27.16b, v2.16b
    umlsl  v16.8h, v30.8b, v3.8b
    umlsl2 v17.8h, v30.16b, v3.16b
    umlsl  v20.8h, v31.8b, v3.8b
    umlsl2 v21.8h, v31.16b, v3.16b

    //(sum + 32) >> 6
    sqrshrun v4.8b, v4.8h, #6
    sqrshrun v5.8b, v5.8h, #6
    sqrshrun v6.8b, v6.8h, #6
    sqrshrun v7.8b, v7.8h, #6
    sqrshrun v24.8b, v16.8h, #6
    sqrshrun v25.8b, v17.8h, #6
    sqrshrun v26.8b, v20.8h, #6
    sqrshrun v27.8b, v21.8h, #6

    subs w5, w5, #1
    st1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x2], #32
    st1 {v24.8b, v25.8b, v26.8b, v27.8b}, [x2], x3
    bgt if_ver_chroma_w128_loop_y

    ret

//void uavs3e_if_ver_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w4_arm64

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src

if_ver_luma_w4_loop_y:
    ld1 {v16.s}[0], [x0], x1           // x-3*i_src
    ld1 {v17.s}[0], [x0], x1           // x-2*i_src
    ld1 {v18.s}[0], [x0], x1           // x-i_src
    ld1 {v19.s}[0], [x0], x1           // x
    mov x10, x0
    ld1 {v20.s}[0], [x0], x1           // x+i_src
    ld1 {v21.s}[0], [x0], x1           // x+2*i_src
    ld1 {v22.s}[0], [x0], x1           // x+3*i_src
    ld1 {v23.s}[0], [x0], x1           // x+4*i_src
    ld1 {v24.s}[0], [x0], x1
    ld1 {v25.s}[0], [x0], x1
    ld1 {v26.s}[0], [x0]

    zip1 v16.2s, v16.2s, v17.2s
    zip1 v17.2s, v17.2s, v18.2s
    zip1 v18.2s, v18.2s, v19.2s
    zip1 v19.2s, v19.2s, v20.2s
    zip1 v20.2s, v20.2s, v21.2s
    zip1 v21.2s, v21.2s, v22.2s
    zip1 v22.2s, v22.2s, v23.2s
    zip1 v23.2s, v23.2s, v24.2s
    zip1 v24.2s, v24.2s, v25.2s
    zip1 v25.2s, v25.2s, v26.2s

    umull v27.8h, v17.8b, v1.8b
    umlsl v27.8h, v16.8b, v0.8b
    umlsl v27.8h, v18.8b, v2.8b
    umlal v27.8h, v19.8b, v3.8b
    umlal v27.8h, v20.8b, v4.8b
    umlsl v27.8h, v21.8b, v5.8b
    umlal v27.8h, v22.8b, v6.8b
    umlsl v27.8h, v23.8b, v7.8b

    umull v28.8h, v19.8b, v1.8b
    umlsl v28.8h, v18.8b, v0.8b
    umlsl v28.8h, v20.8b, v2.8b
    umlal v28.8h, v21.8b, v3.8b
    umlal v28.8h, v22.8b, v4.8b
    umlsl v28.8h, v23.8b, v5.8b
    umlal v28.8h, v24.8b, v6.8b
    umlsl v28.8h, v25.8b, v7.8b

    //(sum + 32) >> 6
    sqrshrun v27.8b, v27.8h, #6
    sqrshrun v28.8b, v28.8h, #6

    subs w5, w5, #4
    mov  x0, x10
    st1 {v27.s}[0], [x2], x3
    st1 {v27.s}[1], [x2], x3
    st1 {v28.s}[0], [x2], x3
    st1 {v28.s}[1], [x2], x3
    bgt if_ver_luma_w4_loop_y

    ret

//void uavs3e_if_ver_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w8_arm64

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src
if_ver_luma_w8_loop_y:
    ld1 {v16.8b}, [x0], x1             // x-3*i_src
    ld1 {v17.8b}, [x0], x1             // x-2*i_src
    ld1 {v18.8b}, [x0], x1             // x-i_src
    ld1 {v19.8b}, [x0], x1             // x
    mov x10, x0
    ld1 {v20.8b}, [x0], x1             // x+i_src
    ld1 {v21.8b}, [x0], x1             // x+2*i_src
    ld1 {v22.8b}, [x0], x1             // x+3*i_src
    ld1 {v23.8b}, [x0], x1             // x+4*i_src
    ld1 {v24.8b}, [x0], x1
    ld1 {v30.8b}, [x0], x1
    ld1 {v31.8b}, [x0]

    umull v25.8h, v17.8b, v1.8b
    umull v26.8h, v18.8b, v1.8b
    umlsl v25.8h, v16.8b, v0.8b
    umlsl v26.8h, v17.8b, v0.8b
    umlsl v25.8h, v18.8b, v2.8b
    umlsl v26.8h, v19.8b, v2.8b
    umlal v25.8h, v19.8b, v3.8b
    umlal v26.8h, v20.8b, v3.8b
    umlal v25.8h, v20.8b, v4.8b
    umlal v26.8h, v21.8b, v4.8b
    umlsl v25.8h, v21.8b, v5.8b
    umlsl v26.8h, v22.8b, v5.8b
    umlal v25.8h, v22.8b, v6.8b
    umlal v26.8h, v23.8b, v6.8b
    umlsl v25.8h, v23.8b, v7.8b
    umlsl v26.8h, v24.8b, v7.8b

    umull v27.8h, v19.8b, v1.8b
    umull v28.8h, v20.8b, v1.8b
    umlsl v27.8h, v18.8b, v0.8b
    umlsl v28.8h, v19.8b, v0.8b
    umlsl v27.8h, v20.8b, v2.8b
    umlsl v28.8h, v21.8b, v2.8b
    umlal v27.8h, v21.8b, v3.8b
    umlal v28.8h, v22.8b, v3.8b
    umlal v27.8h, v22.8b, v4.8b
    umlal v28.8h, v23.8b, v4.8b
    umlsl v27.8h, v23.8b, v5.8b
    umlsl v28.8h, v24.8b, v5.8b
    umlal v27.8h, v24.8b, v6.8b
    umlal v28.8h, v30.8b, v6.8b
    umlsl v27.8h, v30.8b, v7.8b
    umlsl v28.8h, v31.8b, v7.8b

    //(sum + 32) >> 6
    sqrshrun v25.8b, v25.8h, #6
    sqrshrun v26.8b, v26.8h, #6
    sqrshrun v27.8b, v27.8h, #6
    sqrshrun v28.8b, v28.8h, #6

    st1 {v25.8b}, [x2], x3
    st1 {v26.8b}, [x2], x3
    subs w5, w5, #4
    mov  x0, x10
    st1 {v27.8b}, [x2], x3
    st1 {v28.8b}, [x2], x3
    bgt if_ver_luma_w8_loop_y

    ret

//void uavs3e_if_ver_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w16_arm64

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src
if_ver_luma_w16_loop_y:
    ld1 {v16.16b}, [x0], x1            // x-3*i_src
    ld1 {v17.16b}, [x0], x1            // x-2*i_src
    ld1 {v18.16b}, [x0], x1            // x-i_src
    ld1 {v19.16b}, [x0], x1            // x
    mov x10, x0
    ld1 {v20.16b}, [x0], x1            // x+i_src
    ld1 {v21.16b}, [x0], x1            // x+2*i_src
    ld1 {v22.16b}, [x0], x1            // x+3*i_src
    ld1 {v23.16b}, [x0], x1            // x+4*i_src
    ld1 {v24.16b}, [x0], x1
    ld1 {v25.16b}, [x0], x1
    ld1 {v26.16b}, [x0]

    umull  v28.8h, v17.8b, v1.8b
    umull2 v29.8h, v17.16b, v1.16b
    umull  v30.8h, v18.8b, v1.8b
    umull2 v31.8h, v18.16b, v1.16b
    umlsl  v28.8h, v16.8b, v0.8b
    umlsl2 v29.8h, v16.16b, v0.16b
    umlsl  v30.8h, v17.8b, v0.8b
    umlsl2 v31.8h, v17.16b, v0.16b
    umlsl  v28.8h, v18.8b, v2.8b
    umlsl2 v29.8h, v18.16b, v2.16b
    umlsl  v30.8h, v19.8b, v2.8b
    umlsl2 v31.8h, v19.16b, v2.16b
    umlal  v28.8h, v19.8b, v3.8b
    umlal2 v29.8h, v19.16b, v3.16b
    umlal  v30.8h, v20.8b, v3.8b
    umlal2 v31.8h, v20.16b, v3.16b
    umlal  v28.8h, v20.8b, v4.8b
    umlal2 v29.8h, v20.16b, v4.16b
    umlal  v30.8h, v21.8b, v4.8b
    umlal2 v31.8h, v21.16b, v4.16b
    umlsl  v28.8h, v21.8b, v5.8b
    umlsl2 v29.8h, v21.16b, v5.16b
    umlsl  v30.8h, v22.8b, v5.8b
    umlsl2 v31.8h, v22.16b, v5.16b
    umlal  v28.8h, v22.8b, v6.8b
    umlal2 v29.8h, v22.16b, v6.16b
    umlal  v30.8h, v23.8b, v6.8b
    umlal2 v31.8h, v23.16b, v6.16b
    umlsl  v28.8h, v23.8b, v7.8b
    umlsl2 v29.8h, v23.16b, v7.16b
    umlsl  v30.8h, v24.8b, v7.8b
    umlsl2 v31.8h, v24.16b, v7.16b

    sqrshrun v28.8b, v28.8h, #6
    sqrshrun v29.8b, v29.8h, #6
    sqrshrun v30.8b, v30.8h, #6
    sqrshrun v31.8b, v31.8h, #6

    st1 {v28.8b, v29.8b}, [x2], x3
    st1 {v30.8b, v31.8b}, [x2], x3

    umull  v16.8h, v19.8b, v1.8b
    umull2 v17.8h, v19.16b, v1.16b
    umull  v28.8h, v20.8b, v1.8b
    umull2 v29.8h, v20.16b, v1.16b
    umlsl  v16.8h, v18.8b, v0.8b
    umlsl2 v17.8h, v18.16b, v0.16b
    umlsl  v28.8h, v19.8b, v0.8b
    umlsl2 v29.8h, v19.16b, v0.16b
    umlsl  v16.8h, v20.8b, v2.8b
    umlsl2 v17.8h, v20.16b, v2.16b
    umlsl  v28.8h, v21.8b, v2.8b
    umlsl2 v29.8h, v21.16b, v2.16b
    umlal  v16.8h, v21.8b, v3.8b
    umlal2 v17.8h, v21.16b, v3.16b
    umlal  v28.8h, v22.8b, v3.8b
    umlal2 v29.8h, v22.16b, v3.16b
    umlal  v16.8h, v22.8b, v4.8b
    umlal2 v17.8h, v22.16b, v4.16b
    umlal  v28.8h, v23.8b, v4.8b
    umlal2 v29.8h, v23.16b, v4.16b
    umlsl  v16.8h, v23.8b, v5.8b
    umlsl2 v17.8h, v23.16b, v5.16b
    umlsl  v28.8h, v24.8b, v5.8b
    umlsl2 v29.8h, v24.16b, v5.16b
    umlal  v16.8h, v24.8b, v6.8b
    umlal2 v17.8h, v24.16b, v6.16b
    umlal  v28.8h, v25.8b, v6.8b
    umlal2 v29.8h, v25.16b, v6.16b
    umlsl  v16.8h, v25.8b, v7.8b
    umlsl2 v17.8h, v25.16b, v7.16b
    umlsl  v28.8h, v26.8b, v7.8b
    umlsl2 v29.8h, v26.16b, v7.16b

    sqrshrun v16.8b, v16.8h, #6
    sqrshrun v17.8b, v17.8h, #6
    sqrshrun v28.8b, v28.8h, #6
    sqrshrun v29.8b, v29.8h, #6

    mov x0, x10                     // src += i_src
    subs w5, w5, #4
    st1 {v16.8b, v17.8b}, [x2], x3
    st1 {v28.8b, v29.8b}, [x2], x3
    bgt if_ver_luma_w16_loop_y

    ret

//void uavs3e_if_ver_luma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w32_arm64
    sub sp, sp, #96
    st1 {v10.2d, v11.2d, v12.2d, v13.2d}, [sp], #64
    st1 {v14.2d, v15.2d}, [sp]
    sub sp, sp, #64

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src
if_ver_luma_w32_loop_y:
    ld1 {v16.16b, v17.16b}, [x0], x1    // x-3*i_src
    ld1 {v18.16b, v19.16b}, [x0], x1    // x-2*i_src
    mov x10, x0
    ld1 {v20.16b, v21.16b}, [x0], x1    // x-i_src
    ld1 {v22.16b, v23.16b}, [x0], x1    // x
    ld1 {v24.16b, v25.16b}, [x0], x1    // x+i_src
    ld1 {v26.16b, v27.16b}, [x0], x1    // x+2*i_src
    ld1 {v28.16b, v29.16b}, [x0], x1    // x+3*i_src
    ld1 {v30.16b, v31.16b}, [x0], x1    // x+4*i_src

    umull  v12.8h, v18.8b, v1.8b
    umull2 v13.8h, v18.16b, v1.16b
    umull  v14.8h, v19.8b, v1.8b
    umull2 v15.8h, v19.16b, v1.16b
    umlsl  v12.8h, v16.8b, v0.8b
    umlsl2 v13.8h, v16.16b, v0.16b
    umlsl  v14.8h, v17.8b, v0.8b
    umlsl2 v15.8h, v17.16b, v0.16b
    umlsl  v12.8h, v20.8b, v2.8b
    umlsl2 v13.8h, v20.16b, v2.16b
    umlsl  v14.8h, v21.8b, v2.8b
    umlsl2 v15.8h, v21.16b, v2.16b
    umlal  v12.8h, v22.8b, v3.8b
    umlal2 v13.8h, v22.16b, v3.16b
    umlal  v14.8h, v23.8b, v3.8b
    umlal2 v15.8h, v23.16b, v3.16b
    umlal  v12.8h, v24.8b, v4.8b
    umlal2 v13.8h, v24.16b, v4.16b
    umlal  v14.8h, v25.8b, v4.8b
    umlal2 v15.8h, v25.16b, v4.16b
    umlsl  v12.8h, v26.8b, v5.8b
    umlsl2 v13.8h, v26.16b, v5.16b
    umlsl  v14.8h, v27.8b, v5.8b
    umlsl2 v15.8h, v27.16b, v5.16b
    umlal  v12.8h, v28.8b, v6.8b
    umlal2 v13.8h, v28.16b, v6.16b
    umlal  v14.8h, v29.8b, v6.8b
    umlal2 v15.8h, v29.16b, v6.16b
    umlsl  v12.8h, v30.8b, v7.8b
    umlsl2 v13.8h, v30.16b, v7.16b
    umlsl  v14.8h, v31.8b, v7.8b
    umlsl2 v15.8h, v31.16b, v7.16b

    //(sum + 32) >> 6
    sqrshrun v12.8b, v12.8h, #6
    sqrshrun v13.8b, v13.8h, #6
    sqrshrun v14.8b, v14.8h, #6
    sqrshrun v15.8b, v15.8h, #6
    st1 {v12.8b, v13.8b, v14.8b, v15.8b}, [x2], x3

    ld1 {v16.16b, v17.16b}, [x0]

    umull  v10.8h, v20.8b, v1.8b
    umull2 v11.8h, v20.16b, v1.16b
    umull  v12.8h, v21.8b, v1.8b
    umull2 v13.8h, v21.16b, v1.16b
    umlsl  v10.8h, v18.8b, v0.8b
    umlsl2 v11.8h, v18.16b, v0.16b
    umlsl  v12.8h, v19.8b, v0.8b
    umlsl2 v13.8h, v19.16b, v0.16b
    umlsl  v10.8h, v22.8b, v2.8b
    umlsl2 v11.8h, v22.16b, v2.16b
    umlsl  v12.8h, v23.8b, v2.8b
    umlsl2 v13.8h, v23.16b, v2.16b
    umlal  v10.8h, v24.8b, v3.8b
    umlal2 v11.8h, v24.16b, v3.16b
    umlal  v12.8h, v25.8b, v3.8b
    umlal2 v13.8h, v25.16b, v3.16b
    umlal  v10.8h, v26.8b, v4.8b
    umlal2 v11.8h, v26.16b, v4.16b
    umlal  v12.8h, v27.8b, v4.8b
    umlal2 v13.8h, v27.16b, v4.16b
    umlsl  v10.8h, v28.8b, v5.8b
    umlsl2 v11.8h, v28.16b, v5.16b
    umlsl  v12.8h, v29.8b, v5.8b
    umlsl2 v13.8h, v29.16b, v5.16b
    umlal  v10.8h, v30.8b, v6.8b
    umlal2 v11.8h, v30.16b, v6.16b
    umlal  v12.8h, v31.8b, v6.8b
    umlal2 v13.8h, v31.16b, v6.16b
    umlsl  v10.8h, v16.8b, v7.8b
    umlsl2 v11.8h, v16.16b, v7.16b
    umlsl  v12.8h, v17.8b, v7.8b
    umlsl2 v13.8h, v17.16b, v7.16b

    //(sum + 32) >> 6
    sqrshrun v10.8b, v10.8h, #6
    sqrshrun v11.8b, v11.8h, #6
    sqrshrun v12.8b, v12.8h, #6
    sqrshrun v13.8b, v13.8h, #6

    subs w5, w5, #2
    mov x0, x10

    st1 {v10.8b, v11.8b, v12.8b, v13.8b}, [x2], x3
    bgt if_ver_luma_w32_loop_y

    ld1 {v10.2d, v11.2d, v12.2d, v13.2d}, [sp], #64
    ld1 {v14.2d, v15.2d}, [sp], #32
    ret

//void uavs3e_if_ver_luma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w32x_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src
if_ver_luma_w32x_loop_y:
    mov x9, #0
    mov x11, x2
if_ver_luma_w32x_loop_x:
    add x10, x0, x9
    ld1 {v16.16b, v17.16b}, [x10], x1    // x-3*i_src
    ld1 {v18.16b, v19.16b}, [x10], x1    // x-2*i_src
    ld1 {v20.16b, v21.16b}, [x10], x1    // x-i_src
    ld1 {v22.16b, v23.16b}, [x10], x1    // x
    ld1 {v24.16b, v25.16b}, [x10], x1    // x+i_src
    ld1 {v26.16b, v27.16b}, [x10], x1    // x+2*i_src
    ld1 {v28.16b, v29.16b}, [x10], x1    // x+3*i_src
    ld1 {v30.16b, v31.16b}, [x10], x1    // x+4*i_src

    umull  v12.8h, v18.8b, v1.8b
    umull2 v13.8h, v18.16b, v1.16b
    umull  v14.8h, v19.8b, v1.8b
    umull2 v15.8h, v19.16b, v1.16b
    umlsl  v12.8h, v16.8b, v0.8b
    umlsl2 v13.8h, v16.16b, v0.16b
    umlsl  v14.8h, v17.8b, v0.8b
    umlsl2 v15.8h, v17.16b, v0.16b
    umlsl  v12.8h, v20.8b, v2.8b
    umlsl2 v13.8h, v20.16b, v2.16b
    umlsl  v14.8h, v21.8b, v2.8b
    umlsl2 v15.8h, v21.16b, v2.16b
    umlal  v12.8h, v22.8b, v3.8b
    umlal2 v13.8h, v22.16b, v3.16b
    umlal  v14.8h, v23.8b, v3.8b
    umlal2 v15.8h, v23.16b, v3.16b
    umlal  v12.8h, v24.8b, v4.8b
    umlal2 v13.8h, v24.16b, v4.16b
    umlal  v14.8h, v25.8b, v4.8b
    umlal2 v15.8h, v25.16b, v4.16b
    umlsl  v12.8h, v26.8b, v5.8b
    umlsl2 v13.8h, v26.16b, v5.16b
    umlsl  v14.8h, v27.8b, v5.8b
    umlsl2 v15.8h, v27.16b, v5.16b
    umlal  v12.8h, v28.8b, v6.8b
    umlal2 v13.8h, v28.16b, v6.16b
    umlal  v14.8h, v29.8b, v6.8b
    umlal2 v15.8h, v29.16b, v6.16b
    umlsl  v12.8h, v30.8b, v7.8b
    umlsl2 v13.8h, v30.16b, v7.16b
    umlsl  v14.8h, v31.8b, v7.8b
    umlsl2 v15.8h, v31.16b, v7.16b

    add w9, w9, #32
    //(sum + 32) >> 6
    sqrshrun v12.8b, v12.8h, #6
    sqrshrun v13.8b, v13.8h, #6
    sqrshrun v14.8b, v14.8h, #6
    sqrshrun v15.8b, v15.8h, #6
    cmp w9, w4
    st1 {v12.8b, v13.8b, v14.8b, v15.8b}, [x11], #32
    blt if_ver_luma_w32x_loop_x

    subs w5, w5, #1
    add x0, x0, x1                      //src += i_src
    add x2, x2, x3                      //dst += i_dst
    bgt if_ver_luma_w32x_loop_y

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64

    ret

//void uavs3e_if_hor_ver_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w8_arm64

//--------------------------------
// HOR first 3 rows
//--------------------------------
    sub x0, x0, x1                      // src -= i_src
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]
    sub x0, x0, #1                      // x - 1 UV

    //the first three rows
    ld1 {v4.8b, v5.8b}, [x0], x1        // src[x-1]
    ld1 {v6.8b, v7.8b}, [x0], x1
    ld1 {v20.8b, v21.8b}, [x0], x1

    ext v22.8b, v4.8b, v5.8b, #1        // src[x]
    ext v23.8b, v4.8b, v5.8b, #2        // src[x+1]
    ext v24.8b, v4.8b, v5.8b, #3        // src[x+2]

    ext v25.8b, v6.8b, v7.8b, #1        // src[x]
    ext v26.8b, v6.8b, v7.8b, #2        // src[x+1]
    ext v27.8b, v6.8b, v7.8b, #3        // src[x+2]

    ext v28.8b, v20.8b, v21.8b, #1      // src[x]
    ext v29.8b, v20.8b, v21.8b, #2      // src[x+1]
    ext v30.8b, v20.8b, v21.8b, #3      // src[x+2]

    umull v16.8h, v22.8b, v1.8b
    umull v17.8h, v25.8b, v1.8b
    umull v18.8h, v28.8b, v1.8b
    umlsl v16.8h, v4.8b, v0.8b
    umlsl v17.8h, v6.8b, v0.8b
    umlsl v18.8h, v20.8b, v0.8b
    umlal v16.8h, v23.8b, v2.8b
    umlal v17.8h, v26.8b, v2.8b
    umlal v18.8h, v29.8b, v2.8b
    umlsl v16.8h, v24.8b, v3.8b
    umlsl v17.8h, v27.8b, v3.8b
    umlsl v18.8h, v30.8b, v3.8b

    ld1 {v7.s}[0], [x7]                 // load coeff
    sxtl v7.8h, v7.8b                   // 8bit to 16bit

if_hor_ver_chroma_w8_loop_y:
    ld1 {v24.8b, v25.8b}, [x0], x1      // src[x-1]
    ld1 {v26.8b, v27.8b}, [x0], x1      // src[x-1]
    ld1 {v28.8b, v29.8b}, [x0], x1
    ld1 {v30.8b, v31.8b}, [x0], x1

    ext v4.8b, v24.8b, v25.8b, #1       // src[x]
    ext v5.8b, v24.8b, v25.8b, #2       // src[x+1]
    ext v6.8b, v24.8b, v25.8b, #3       // src[x+2]

    ext v23.8b, v26.8b, v27.8b, #1      // src[x]
    ext v25.8b, v26.8b, v27.8b, #2      // src[x+1]
    ext v27.8b, v26.8b, v27.8b, #3      // src[x+2]

    umull v19.8h, v4.8b , v1.8b
    umull v20.8h, v23.8b, v1.8b
    umlsl v19.8h, v24.8b, v0.8b
    umlsl v20.8h, v26.8b, v0.8b
    umlal v19.8h, v5.8b , v2.8b
    umlal v20.8h, v25.8b, v2.8b
    umlsl v19.8h, v6.8b , v3.8b
    umlsl v20.8h, v27.8b, v3.8b

    ext v4.8b, v28.8b, v29.8b, #1       // src[x]
    ext v5.8b, v28.8b, v29.8b, #2       // src[x+1]
    ext v6.8b, v28.8b, v29.8b, #3       // src[x+2]

    ext v23.8b, v30.8b, v31.8b, #1      // src[x]
    ext v25.8b, v30.8b, v31.8b, #2      // src[x+1]
    ext v27.8b, v30.8b, v31.8b, #3      // src[x+2]

    umull v21.8h, v4.8b , v1.8b
    umull v22.8h, v23.8b, v1.8b
    umlsl v21.8h, v28.8b, v0.8b
    umlsl v22.8h, v30.8b, v0.8b
    umlal v21.8h, v5.8b , v2.8b
    umlal v22.8h, v25.8b, v2.8b
    umlsl v21.8h, v6.8b , v3.8b
    umlsl v22.8h, v27.8b, v3.8b

    smull  v24.4s, v16.4h, v7.h[0]
    smull2 v25.4s, v16.8h, v7.h[0]
    smull  v26.4s, v17.4h, v7.h[0]
    smull2 v27.4s, v17.8h, v7.h[0]
    smlal  v24.4s, v17.4h, v7.h[1]
    smlal2 v25.4s, v17.8h, v7.h[1]
    smlal  v26.4s, v18.4h, v7.h[1]
    smlal2 v27.4s, v18.8h, v7.h[1]
    smlal  v24.4s, v18.4h, v7.h[2]
    smlal2 v25.4s, v18.8h, v7.h[2]
    smlal  v26.4s, v19.4h, v7.h[2]
    smlal2 v27.4s, v19.8h, v7.h[2]
    smlal  v24.4s, v19.4h, v7.h[3]
    smlal2 v25.4s, v19.8h, v7.h[3]
    smlal  v26.4s, v20.4h, v7.h[3]
    smlal2 v27.4s, v20.8h, v7.h[3]

    smull  v28.4s, v18.4h, v7.h[0]
    smull2 v29.4s, v18.8h, v7.h[0]
    smull  v30.4s, v19.4h, v7.h[0]
    smull2 v31.4s, v19.8h, v7.h[0]
    smlal  v28.4s, v19.4h, v7.h[1]
    smlal2 v29.4s, v19.8h, v7.h[1]
    smlal  v30.4s, v20.4h, v7.h[1]
    smlal2 v31.4s, v20.8h, v7.h[1]
    smlal  v28.4s, v20.4h, v7.h[2]
    smlal2 v29.4s, v20.8h, v7.h[2]
    smlal  v30.4s, v21.4h, v7.h[2]
    smlal2 v31.4s, v21.8h, v7.h[2]
    smlal  v28.4s, v21.4h, v7.h[3]
    smlal2 v29.4s, v21.8h, v7.h[3]
    smlal  v30.4s, v22.4h, v7.h[3]
    smlal2 v31.4s, v22.8h, v7.h[3]

    mov v16.16b, v20.16b
    mov v17.16b, v21.16b
    mov v18.16b, v22.16b

    rshrn  v24.4h, v24.4s, #12
    rshrn2 v24.8h, v25.4s, #12
    rshrn  v25.4h, v26.4s, #12
    rshrn2 v25.8h, v27.4s, #12
    rshrn  v26.4h, v28.4s, #12
    rshrn2 v26.8h, v29.4s, #12
    rshrn  v27.4h, v30.4s, #12
    rshrn2 v27.8h, v31.4s, #12

    sqxtun v24.8b, v24.8h               // 16bit to 8bit
    sqxtun v25.8b, v25.8h
    sqxtun v26.8b, v26.8h               // 16bit to 8bit
    sqxtun v27.8b, v27.8h

    subs w5, w5, #4
    st1 {v24.8b}, [x2], x3
    st1 {v25.8b}, [x2], x3
    st1 {v26.8b}, [x2], x3
    st1 {v27.8b}, [x2], x3
    bgt if_hor_ver_chroma_w8_loop_y

    ret

//void uavs3e_if_hor_ver_chroma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8n 0- *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w16_arm64

    // align (x17)
    // x17-->tmp
    ldr x15, =2176
    sub x17, sp, x15                        // (64 + 4)*16*sizeof(short)
    sub x0, x0, x1                          // src += -1 * i_src;
    mov sp, x17

//--------------------------------
// HOR
//--------------------------------
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]
    sub x0, x0, #1                          // x - 1

    //the first three rows
    ld1 {v16.16b, v17.16b}, [x0], x1       // src[x-1]
    ld1 {v18.16b, v19.16b}, [x0], x1       // src[x-1]
    ld1 {v20.16b, v21.16b}, [x0], x1

    ext v22.16b, v16.16b, v17.16b, #1       // src[x]
    ext v23.16b, v16.16b, v17.16b, #2       // src[x+1]
    ext v24.16b, v16.16b, v17.16b, #3       // src[x+2]

    ext v25.16b, v18.16b, v19.16b, #1
    ext v26.16b, v18.16b, v19.16b, #2
    ext v27.16b, v18.16b, v19.16b, #3

    ext v28.16b, v20.16b, v21.16b, #1
    ext v29.16b, v20.16b, v21.16b, #2
    ext v30.16b, v20.16b, v21.16b, #3

    umull  v4.8h, v22.8b, v1.8b
    umull2 v5.8h, v22.16b, v1.16b
    umlsl  v4.8h, v16.8b, v0.8b
    umlsl2 v5.8h, v16.16b, v0.16b
    umlal  v4.8h, v23.8b, v2.8b
    umlal2 v5.8h, v23.16b, v2.16b
    umlsl  v4.8h, v24.8b, v3.8b
    umlsl2 v5.8h, v24.16b, v3.16b

    umull  v6.8h, v25.8b, v1.8b
    umull2 v7.8h, v25.16b, v1.16b
    umlsl  v6.8h, v18.8b, v0.8b
    umlsl2 v7.8h, v18.16b, v0.16b
    umlal  v6.8h, v26.8b, v2.8b
    umlal2 v7.8h, v26.16b, v2.16b
    umlsl  v6.8h, v27.8b, v3.8b
    umlsl2 v7.8h, v27.16b, v3.16b

    umull  v16.8h, v28.8b, v1.8b
    umull2 v17.8h, v28.16b, v1.16b
    umlsl  v16.8h, v20.8b, v0.8b
    umlsl2 v17.8h, v20.16b, v0.16b
    umlal  v16.8h, v29.8b, v2.8b
    umlal2 v17.8h, v29.16b, v2.16b
    umlsl  v16.8h, v30.8b, v3.8b
    umlsl2 v17.8h, v30.16b, v3.16b

    st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x17], #64
    st1 {v16.2d, v17.2d}, [x17], #32

    mov w8, w5
if_hor_ver_chroma_w16_hor_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], x1        // src[x-1]
    ld1 {v22.16b, v23.16b}, [x0], x1

    ext v17.16b, v20.16b, v21.16b, #1       // src[x]
    ext v18.16b, v20.16b, v21.16b, #2       // src[x+1]
    ext v19.16b, v20.16b, v21.16b, #3       // src[x+2]
    ext v29.16b, v22.16b, v23.16b, #1       // src[x]
    ext v30.16b, v22.16b, v23.16b, #2       // src[x+1]
    ext v31.16b, v22.16b, v23.16b, #3       // src[x+2]

    umull  v24.8h, v17.8b, v1.8b
    umull2 v25.8h, v17.16b, v1.16b
    umull  v26.8h, v29.8b, v1.8b
    umull2 v27.8h, v29.16b, v1.16b
    umlsl  v24.8h, v20.8b, v0.8b
    umlsl2 v25.8h, v20.16b, v0.16b
    umlsl  v26.8h, v22.8b, v0.8b
    umlsl2 v27.8h, v22.16b, v0.16b
    umlal  v24.8h, v18.8b, v2.8b
    umlal2 v25.8h, v18.16b, v2.16b
    umlal  v26.8h, v30.8b, v2.8b
    umlal2 v27.8h, v30.16b, v2.16b
    umlsl  v24.8h, v19.8b, v3.8b
    umlsl2 v25.8h, v19.16b, v3.16b
    umlsl  v26.8h, v31.8b, v3.8b
    umlsl2 v27.8h, v31.16b, v3.16b

    subs w8, w8, #2
    st1 {v24.2d, v25.2d, v26.2d, v27.2d}, [x17], #64
    bgt if_hor_ver_chroma_w16_hor_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit
if_hor_ver_chroma_w16_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v2.8h, v3.8h}, [x17]

    smull  v24.4s, v16.4h, v0.h[0]
    smull2 v25.4s, v16.8h, v0.h[0]
    smull  v26.4s, v17.4h, v0.h[0]
    smull2 v27.4s, v17.8h, v0.h[0]
    smlal  v24.4s, v18.4h, v0.h[1]
    smlal2 v25.4s, v18.8h, v0.h[1]
    smlal  v26.4s, v19.4h, v0.h[1]
    smlal2 v27.4s, v19.8h, v0.h[1]
    smlal  v24.4s, v20.4h, v0.h[2]
    smlal2 v25.4s, v20.8h, v0.h[2]
    smlal  v26.4s, v21.4h, v0.h[2]
    smlal2 v27.4s, v21.8h, v0.h[2]
    smlal  v24.4s, v22.4h, v0.h[3]
    smlal2 v25.4s, v22.8h, v0.h[3]
    smlal  v26.4s, v23.4h, v0.h[3]
    smlal2 v27.4s, v23.8h, v0.h[3]

    smull  v28.4s, v18.4h, v0.h[0]
    smull2 v29.4s, v18.8h, v0.h[0]
    smull  v30.4s, v19.4h, v0.h[0]
    smull2 v31.4s, v19.8h, v0.h[0]
    smlal  v28.4s, v20.4h, v0.h[1]
    smlal2 v29.4s, v20.8h, v0.h[1]
    smlal  v30.4s, v21.4h, v0.h[1]
    smlal2 v31.4s, v21.8h, v0.h[1]
    smlal  v28.4s, v22.4h, v0.h[2]
    smlal2 v29.4s, v22.8h, v0.h[2]
    smlal  v30.4s, v23.4h, v0.h[2]
    smlal2 v31.4s, v23.8h, v0.h[2]
    smlal  v28.4s, v2.4h, v0.h[3]
    smlal2 v29.4s, v2.8h, v0.h[3]
    smlal  v30.4s, v3.4h, v0.h[3]
    smlal2 v31.4s, v3.8h, v0.h[3]

    rshrn  v24.4h, v24.4s, #12
    rshrn2 v24.8h, v25.4s, #12
    rshrn  v25.4h, v26.4s, #12
    rshrn2 v25.8h, v27.4s, #12
    rshrn  v26.4h, v28.4s, #12
    rshrn2 v26.8h, v29.4s, #12
    rshrn  v27.4h, v30.4s, #12
    rshrn2 v27.8h, v31.4s, #12

    sqxtun v24.8b, v24.8h                   // 16bit to 8bit
    sqxtun v25.8b, v25.8h
    sqxtun v26.8b, v26.8h
    sqxtun v27.8b, v27.8h

    subs w5, w5, #2
    mov x17, x10                            // tmp += 64;
    st1 {v24.8b, v25.8b}, [x2], x3
    st1 {v26.8b, v27.8b}, [x2], x3
    bgt if_hor_ver_chroma_w16_ver_loop_y

    add sp, sp, x15                         // (64 + 4)*16*sizeof(short)

    ret

//void uavs3e_if_hor_ver_chroma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8n 0- *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w32_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]

    // align (x17)
    // x17-->tmp
    ldr x15, =4352
    sub x17, sp, x15                        // (64 + 4)*32*sizeof(short)

    sub x0, x0, x1                          // src -= i_src;
    lsl x16, x4, #1

    mov sp, x17

    //--------------------------------
    // HOR
    //--------------------------------
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]
    sub x0, x0, #1                          // x - 1 UV
    sub x1, x1, #32

    add w8, w5, #3                          // height + 3
if_hor_ver_chroma_w32_hor_loop_y:
    ld1 {v16.16b, v17.16b}, [x0], #32       // src[x-1]
    ld1 {v18.16b}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #1       // src[x]
    ext v20.16b, v16.16b, v17.16b, #2       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #3       // src[x+2]
    ext v22.16b, v17.16b, v18.16b, #1
    ext v23.16b, v17.16b, v18.16b, #2
    ext v24.16b, v17.16b, v18.16b, #3

    umull  v28.8h, v19.8b, v1.8b
    umull2 v29.8h, v19.16b, v1.16b
    umlsl  v28.8h, v16.8b, v0.8b
    umlsl2 v29.8h, v16.16b, v0.16b
    umlal  v28.8h, v20.8b, v2.8b
    umlal2 v29.8h, v20.16b, v2.16b
    umlsl  v28.8h, v21.8b, v3.8b
    umlsl2 v29.8h, v21.16b, v3.16b

    umull  v30.8h, v22.8b, v1.8b
    umull2 v31.8h, v22.16b, v1.16b
    umlsl  v30.8h, v17.8b, v0.8b
    umlsl2 v31.8h, v17.16b, v0.16b
    umlal  v30.8h, v23.8b, v2.8b
    umlal2 v31.8h, v23.16b, v2.16b
    umlsl  v30.8h, v24.8b, v3.8b
    umlsl2 v31.8h, v24.16b, v3.16b

    subs w8, w8, #1
    st1 {v28.2d, v29.2d, v30.2d, v31.2d}, [x17], #64
    bgt if_hor_ver_chroma_w32_hor_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w32_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64        // x-i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    mov x10, x17
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64
    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [x17]

    smull  v4.4s, v16.4h, v0.h[0]
    smull2 v5.4s, v16.8h, v0.h[0]
    smull  v6.4s, v17.4h, v0.h[0]
    smull2 v7.4s, v17.8h, v0.h[0]
    smlal  v4.4s, v20.4h, v0.h[1]
    smlal2 v5.4s, v20.8h, v0.h[1]
    smlal  v6.4s, v21.4h, v0.h[1]
    smlal2 v7.4s, v21.8h, v0.h[1]
    smlal  v4.4s, v24.4h, v0.h[2]
    smlal2 v5.4s, v24.8h, v0.h[2]
    smlal  v6.4s, v25.4h, v0.h[2]
    smlal2 v7.4s, v25.8h, v0.h[2]
    smlal  v4.4s, v28.4h, v0.h[3]
    smlal2 v5.4s, v28.8h, v0.h[3]
    smlal  v6.4s, v29.4h, v0.h[3]
    smlal2 v7.4s, v29.8h, v0.h[3]

    rshrn  v4.4h, v4.4s, #12
    rshrn2 v4.8h, v5.4s, #12
    rshrn  v5.4h, v6.4s, #12
    rshrn2 v5.8h, v7.4s, #12

    sqxtun v4.8b, v4.8h                 // 16bit to 8bit
    sqxtun v5.8b, v5.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v22.4h, v0.h[1]
    smlal2 v3.4s, v22.8h, v0.h[1]
    smlal  v6.4s, v23.4h, v0.h[1]
    smlal2 v7.4s, v23.8h, v0.h[1]
    smlal  v2.4s, v26.4h, v0.h[2]
    smlal2 v3.4s, v26.8h, v0.h[2]
    smlal  v6.4s, v27.4h, v0.h[2]
    smlal2 v7.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v30.4h, v0.h[3]
    smlal2 v3.4s, v30.8h, v0.h[3]
    smlal  v6.4s, v31.4h, v0.h[3]
    smlal2 v7.4s, v31.8h, v0.h[3]

    rshrn  v2.4h, v2.4s, #12
    rshrn2 v2.8h, v3.4s, #12
    rshrn  v3.4h, v6.4s, #12
    rshrn2 v3.8h, v7.4s, #12

    sqxtun v6.8b, v2.8h                 // 16bit to 8bit
    sqxtun v7.8b, v3.8h

    st1 {v4.4h, v5.4h, v6.4h, v7.4h}, [x2], x3

    smull  v4.4s, v20.4h, v0.h[0]
    smull2 v5.4s, v20.8h, v0.h[0]
    smull  v6.4s, v21.4h, v0.h[0]
    smull2 v7.4s, v21.8h, v0.h[0]
    smlal  v4.4s, v24.4h, v0.h[1]
    smlal2 v5.4s, v24.8h, v0.h[1]
    smlal  v6.4s, v25.4h, v0.h[1]
    smlal2 v7.4s, v25.8h, v0.h[1]
    smlal  v4.4s, v28.4h, v0.h[2]
    smlal2 v5.4s, v28.8h, v0.h[2]
    smlal  v6.4s, v29.4h, v0.h[2]
    smlal2 v7.4s, v29.8h, v0.h[2]
    smlal  v4.4s, v12.4h, v0.h[3]
    smlal2 v5.4s, v12.8h, v0.h[3]
    smlal  v6.4s, v13.4h, v0.h[3]
    smlal2 v7.4s, v13.8h, v0.h[3]

    rshrn  v4.4h, v4.4s, #12
    rshrn2 v4.8h, v5.4s, #12
    rshrn  v5.4h, v6.4s, #12
    rshrn2 v5.8h, v7.4s, #12

    sqxtun v4.8b, v4.8h                 // 16bit to 8bit
    sqxtun v5.8b, v5.8h

    smull  v2.4s, v22.4h, v0.h[0]
    smull2 v3.4s, v22.8h, v0.h[0]
    smull  v6.4s, v23.4h, v0.h[0]
    smull2 v7.4s, v23.8h, v0.h[0]
    smlal  v2.4s, v26.4h, v0.h[1]
    smlal2 v3.4s, v26.8h, v0.h[1]
    smlal  v6.4s, v27.4h, v0.h[1]
    smlal2 v7.4s, v27.8h, v0.h[1]
    smlal  v2.4s, v30.4h, v0.h[2]
    smlal2 v3.4s, v30.8h, v0.h[2]
    smlal  v6.4s, v31.4h, v0.h[2]
    smlal2 v7.4s, v31.8h, v0.h[2]
    smlal  v2.4s, v14.4h, v0.h[3]
    smlal2 v3.4s, v14.8h, v0.h[3]
    smlal  v6.4s, v15.4h, v0.h[3]
    smlal2 v7.4s, v15.8h, v0.h[3]

    rshrn  v2.4h, v2.4s, #12
    rshrn2 v2.8h, v3.4s, #12
    rshrn  v3.4h, v6.4s, #12
    rshrn2 v3.8h, v7.4s, #12

    sqxtun v6.8b, v2.8h                 // 16bit to 8bit
    sqxtun v7.8b, v3.8h

    subs w5, w5, #2
    mov x17, x10
    st1 {v4.4h, v5.4h, v6.4h, v7.4h}, [x2], x3
    bgt if_hor_ver_chroma_w32_ver_loop_y

    add sp, sp, x15                     // (128 + 4)*64*2*sizeof(short)

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
ret

//void uavs3e_if_hor_ver_chroma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8n 0- *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w32x_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]
    // align (x17)
    // x17-->tmp
    mov x15, #68
    lsl x15, x15, #8
    sub x17, sp, x15                        // (64 + 4)*128*sizeof(short)

    sub x0, x0, x1                          // src -= i_src;
    lsl x16, x4, #1

    mov sp, x17
    //--------------------------------
    // HOR
    //--------------------------------
    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    dup v0.16b, v3.b[0]
    dup v1.16b, v3.b[1]
    dup v2.16b, v3.b[2]
    dup v3.16b, v3.b[3]
    sub x0, x0, #1                          // x - 1 UV

    add w8, w5, #3
if_hor_ver_chroma_w32x_hor_loop_y:
    mov w9, w4
    mov x10, x0
    mov x11, x17
if_hor_ver_chroma_w32x_hor_loop_x:
    ld1 {v16.16b, v17.16b}, [x10], #32      // src[x-1]
    ld1 {v18.16b}, [x10]

    ext v19.16b, v16.16b, v17.16b, #1       // src[x]
    ext v20.16b, v16.16b, v17.16b, #2       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #3       // src[x+2]
    ext v22.16b, v17.16b, v18.16b, #1
    ext v23.16b, v17.16b, v18.16b, #2
    ext v24.16b, v17.16b, v18.16b, #3

    umull  v28.8h, v19.8b, v1.8b
    umull2 v29.8h, v19.16b, v1.16b
    umlsl  v28.8h, v16.8b, v0.8b
    umlsl2 v29.8h, v16.16b, v0.16b
    umlal  v28.8h, v20.8b, v2.8b
    umlal2 v29.8h, v20.16b, v2.16b
    umlsl  v28.8h, v21.8b, v3.8b
    umlsl2 v29.8h, v21.16b, v3.16b

    umull  v30.8h, v22.8b, v1.8b
    umull2 v31.8h, v22.16b, v1.16b
    umlsl  v30.8h, v17.8b, v0.8b
    umlsl2 v31.8h, v17.16b, v0.16b
    umlal  v30.8h, v23.8b, v2.8b
    umlal2 v31.8h, v23.16b, v2.16b
    umlsl  v30.8h, v24.8b, v3.8b
    umlsl2 v31.8h, v24.16b, v3.16b

    subs w9, w9, #32
    st1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x11], #64
    bgt if_hor_ver_chroma_w32x_hor_loop_x

    subs w8, w8, #1
    add x0, x0, x1
    add x17, x17, x16
    bgt if_hor_ver_chroma_w32x_hor_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w32x_ver_loop_y:
    mov x9, #0
    mov x11, x2
if_hor_ver_chroma_w32x_ver_loop_x:
    add x10, x17, x9
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16        // x-i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10]

    smull  v4.4s, v16.4h, v0.h[0]
    smull2 v5.4s, v16.8h, v0.h[0]
    smull  v6.4s, v17.4h, v0.h[0]
    smull2 v7.4s, v17.8h, v0.h[0]
    smlal  v4.4s, v20.4h, v0.h[1]
    smlal2 v5.4s, v20.8h, v0.h[1]
    smlal  v6.4s, v21.4h, v0.h[1]
    smlal2 v7.4s, v21.8h, v0.h[1]
    smlal  v4.4s, v24.4h, v0.h[2]
    smlal2 v5.4s, v24.8h, v0.h[2]
    smlal  v6.4s, v25.4h, v0.h[2]
    smlal2 v7.4s, v25.8h, v0.h[2]
    smlal  v4.4s, v28.4h, v0.h[3]
    smlal2 v5.4s, v28.8h, v0.h[3]
    smlal  v6.4s, v29.4h, v0.h[3]
    smlal2 v7.4s, v29.8h, v0.h[3]

    rshrn  v4.4h, v4.4s, #12
    rshrn2 v4.8h, v5.4s, #12
    rshrn  v5.4h, v6.4s, #12
    rshrn2 v5.8h, v7.4s, #12

    sqxtun v4.8b, v4.8h                   // 16bit to 8bit
    sqxtun v5.8b, v5.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v22.4h, v0.h[1]
    smlal2 v3.4s, v22.8h, v0.h[1]
    smlal  v6.4s, v23.4h, v0.h[1]
    smlal2 v7.4s, v23.8h, v0.h[1]
    smlal  v2.4s, v26.4h, v0.h[2]
    smlal2 v3.4s, v26.8h, v0.h[2]
    smlal  v6.4s, v27.4h, v0.h[2]
    smlal2 v7.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v30.4h, v0.h[3]
    smlal2 v3.4s, v30.8h, v0.h[3]
    smlal  v6.4s, v31.4h, v0.h[3]
    smlal2 v7.4s, v31.8h, v0.h[3]

    rshrn  v2.4h, v2.4s, #12
    rshrn2 v2.8h, v3.4s, #12
    rshrn  v3.4h, v6.4s, #12
    rshrn2 v3.8h, v7.4s, #12

    sqxtun v6.8b, v2.8h                   // 16bit to 8bit
    sqxtun v7.8b, v3.8h

    add w9, w9, #64
    st1 {v4.4h, v5.4h, v6.4h, v7.4h}, [x11], #32
    //--------------------------------
    // loop control
    //--------------------------------
    cmp w9, w16
    blt if_hor_ver_chroma_w32x_ver_loop_x

    subs w5, w5, #1
    add x17, x17, x16                       // tmp += i_tmp;
    add x2, x2, x3                          // dst += i_dst

    bgt if_hor_ver_chroma_w32x_ver_loop_y

    add sp, sp, x15                     // (64 + 4)*64*2*sizeof(short)

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
ret


//void uavs3e_if_hor_ver_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w4_arm64
    sub sp, sp, #16
    st1 {v8.2d}, [sp]

    sub x0, x0, x1, lsl #1              // src += -3 * i_src;
    ld1 {v7.d}[0], [x6]
    ld1 {v8.d}[0], [x7]                 // load coeff
    sub x0, x0, x1

    abs  v7.8b, v7.8b
    sxtl v8.8h, v8.8b                   // 8bit to 16bit

    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                      // src -= 3
//--------------------------------
// HOR first 3 rows
//--------------------------------
    ld1 {v16.16b}, [x0], x1             // src[x-3]
    ld1 {v17.16b}, [x0], x1
    ld1 {v18.16b}, [x0], x1

    zip1 v20.16b, v16.16b, v16.16b      // 00, 10, 01, 11 ...
    zip2 v21.16b, v16.16b, v16.16b      // 08, 18, 09, 19 ...
    zip1 v22.16b, v17.16b, v18.16b      // 20, 30, 21, 31 ...
    zip2 v23.16b, v17.16b, v18.16b      // 28, 38, 29, 39 ...

    zip1 v16.8h, v20.8h, v22.8h         // 00, 10, 20, 30, 01, 11, 21, 31 ...
    zip2 v17.8h, v20.8h, v22.8h         // 04, 14, 24, 34 ...
    zip1 v18.8h, v21.8h, v23.8h         // 08, 18, 28, 38 ...

    ext v24.16b, v16.16b, v17.16b, #4
    ext v26.16b, v16.16b, v17.16b, #12

    ext v27.16b, v17.16b, v18.16b, #4
    ext v29.16b, v17.16b, v18.16b, #12

    ld1 {v20.16b}, [x0], x1             // src[x-3]
    ld1 {v21.16b}, [x0], x1
    ld1 {v22.16b}, [x0], x1
    ld1 {v23.16b}, [x0], x1

    umull  v30.8h, v24.8b, v1.8b
    umull2 v31.8h, v24.16b, v1.16b
    umlsl  v30.8h, v16.8b, v0.8b
    umlsl2 v31.8h, v16.16b, v0.16b
    umlsl2 v30.8h, v16.16b, v2.16b
    umlsl  v31.8h, v17.8b, v2.8b
    umlal  v30.8h, v26.8b, v3.8b
    umlal2 v31.8h, v26.16b, v3.16b
    umlal  v30.8h, v17.8b, v4.8b
    umlal2 v31.8h, v17.16b, v4.16b
    umlsl  v30.8h, v27.8b, v5.8b
    umlsl2 v31.8h, v27.16b, v5.16b
    umlal2 v30.8h, v17.16b, v6.16b
    umlal  v31.8h, v18.8b, v6.8b
    umlsl  v30.8h, v29.8b, v7.8b        // 00, 10, 20, 30, 01, 11, 21, 31
    umlsl2 v31.8h, v29.16b, v7.16b      // 02, 12, 22, 32, 03, 13, 23, 33

    uzp1 v18.8h, v30.8h, v31.8h
    uzp2 v19.8h, v30.8h, v31.8h
    uzp1 v16.8h, v18.8h, v19.8h
    uzp2 v17.8h, v18.8h, v19.8h

//--------------------------------
// HOR rows[3-6]
//--------------------------------
    zip1 v24.16b, v20.16b, v21.16b      // 00, 10, 01, 11 ...
    zip2 v25.16b, v20.16b, v21.16b      // 08, 18, 09, 19 ...
    zip1 v26.16b, v22.16b, v23.16b      // 20, 30, 21, 31 ...
    zip2 v27.16b, v22.16b, v23.16b      // 28, 38, 29, 39 ...

    zip1 v20.8h, v24.8h, v26.8h         // 00, 10, 20, 30, 01, 11, 21, 31 ...
    zip2 v21.8h, v24.8h, v26.8h         // 04, 14, 24, 34 ...
    zip1 v22.8h, v25.8h, v27.8h         // 08, 18, 28, 38 ...

    ext v24.16b, v20.16b, v21.16b, #4
    ext v26.16b, v20.16b, v21.16b, #12

    ext v27.16b, v21.16b, v22.16b, #4
    ext v29.16b, v21.16b, v22.16b, #12

    umull  v30.8h, v24.8b, v1.8b
    umull2 v31.8h, v24.16b, v1.16b
    umlsl  v30.8h, v20.8b, v0.8b
    umlsl2 v31.8h, v20.16b, v0.16b
    umlsl2 v30.8h, v20.16b, v2.16b
    umlsl  v31.8h, v21.8b, v2.8b
    umlal  v30.8h, v26.8b, v3.8b
    umlal2 v31.8h, v26.16b, v3.16b
    umlal  v30.8h, v21.8b, v4.8b
    umlal2 v31.8h, v21.16b, v4.16b
    umlsl  v30.8h, v27.8b, v5.8b
    umlsl2 v31.8h, v27.16b, v5.16b
    umlal2 v30.8h, v21.16b, v6.16b
    umlal  v31.8h, v22.8b, v6.8b
    umlsl  v30.8h, v29.8b, v7.8b        // 00, 10, 20, 30, 01, 11, 21, 31
    umlsl2 v31.8h, v29.16b, v7.16b      // 02, 12, 22, 32, 03, 13, 23, 33

    uzp1 v22.8h, v30.8h, v31.8h
    uzp2 v23.8h, v30.8h, v31.8h
    uzp1 v18.8h, v22.8h, v23.8h
    uzp2 v19.8h, v22.8h, v23.8h

    // load coeffs
if_hor_ver_luma_w4_loop_y:
    ld1 {v20.16b}, [x0], x1             // src[x-3]
    ld1 {v21.16b}, [x0], x1
    ld1 {v22.16b}, [x0], x1
    ld1 {v23.16b}, [x0], x1

    zip1 v24.16b, v20.16b, v21.16b      // 00, 10, 01, 11 ...
    zip2 v25.16b, v20.16b, v21.16b      // 08, 18, 09, 19 ...
    zip1 v26.16b, v22.16b, v23.16b      // 20, 30, 21, 31 ...
    zip2 v27.16b, v22.16b, v23.16b      // 28, 38, 29, 39 ...

    zip1 v20.8h, v24.8h, v26.8h         // 00, 10, 20, 30, 01, 11, 21, 31 ...
    zip2 v21.8h, v24.8h, v26.8h         // 04, 14, 24, 34 ...
    zip1 v22.8h, v25.8h, v27.8h         // 08, 18, 28, 38 ...

    ext v24.16b, v20.16b, v21.16b, #4
    ext v26.16b, v20.16b, v21.16b, #12

    ext v27.16b, v21.16b, v22.16b, #4
    ext v29.16b, v21.16b, v22.16b, #12

    umull  v30.8h, v24.8b, v1.8b
    umull2 v31.8h, v24.16b, v1.16b
    umlsl  v30.8h, v20.8b, v0.8b
    umlsl2 v31.8h, v20.16b, v0.16b
    umlsl2 v30.8h, v20.16b, v2.16b
    umlsl  v31.8h, v21.8b, v2.8b
    umlal  v30.8h, v26.8b, v3.8b
    umlal2 v31.8h, v26.16b, v3.16b
    umlal  v30.8h, v21.8b, v4.8b
    umlal2 v31.8h, v21.16b, v4.16b
    umlsl  v30.8h, v27.8b, v5.8b
    umlsl2 v31.8h, v27.16b, v5.16b
    umlal2 v30.8h, v21.16b, v6.16b
    umlal  v31.8h, v22.8b, v6.8b
    umlsl  v30.8h, v29.8b, v7.8b        // 00, 10, 20, 30, 01, 11, 21, 31
    umlsl2 v31.8h, v29.16b, v7.16b      // 02, 12, 22, 32, 03, 13, 23, 33

    uzp1 v22.8h, v30.8h, v31.8h
    uzp2 v23.8h, v30.8h, v31.8h
    uzp1 v20.8h, v22.8h, v23.8h
    uzp2 v21.8h, v22.8h, v23.8h

    smull2 v27.4s, v16.8h, v8.h[0]
    smull  v28.4s, v17.4h, v8.h[0]
    smull2 v29.4s, v17.8h, v8.h[0]
    smull  v30.4s, v18.4h, v8.h[0]

    smlal  v27.4s, v17.4h, v8.h[1]
    smlal2 v28.4s, v17.8h, v8.h[1]
    smlal  v29.4s, v18.4h, v8.h[1]
    smlal2 v30.4s, v18.8h, v8.h[1]

    smlal2 v27.4s, v17.8h, v8.h[2]
    smlal  v28.4s, v18.4h, v8.h[2]
    smlal2 v29.4s, v18.8h, v8.h[2]
    smlal  v30.4s, v19.4h, v8.h[2]

    smlal  v27.4s, v18.4h, v8.h[3]
    smlal2 v28.4s, v18.8h, v8.h[3]
    smlal  v29.4s, v19.4h, v8.h[3]
    smlal2 v30.4s, v19.8h, v8.h[3]

    smlal2 v27.4s, v18.8h, v8.h[4]
    smlal  v28.4s, v19.4h, v8.h[4]
    smlal2 v29.4s, v19.8h, v8.h[4]
    smlal  v30.4s, v20.4h, v8.h[4]

    smlal  v27.4s, v19.4h, v8.h[5]
    smlal2 v28.4s, v19.8h, v8.h[5]
    smlal  v29.4s, v20.4h, v8.h[5]
    smlal2 v30.4s, v20.8h, v8.h[5]

    smlal2 v27.4s, v19.8h, v8.h[6]
    smlal  v28.4s, v20.4h, v8.h[6]
    smlal2 v29.4s, v20.8h, v8.h[6]
    smlal  v30.4s, v21.4h, v8.h[6]

    smlal  v27.4s, v20.4h, v8.h[7]
    smlal2 v28.4s, v20.8h, v8.h[7]
    smlal  v29.4s, v21.4h, v8.h[7]
    smlal2 v30.4s, v21.8h, v8.h[7]

    mov v16.16b, v18.16b
    mov v17.16b, v19.16b
    mov v18.16b, v20.16b
    mov v19.16b, v21.16b

    rshrn v27.4h, v27.4s, #12
    rshrn v28.4h, v28.4s, #12
    rshrn v29.4h, v29.4s, #12
    rshrn v30.4h, v30.4s, #12

    sqxtun v27.8b, v27.8h               // 16bit to 8bit
    sqxtun v28.8b, v28.8h
    sqxtun v29.8b, v29.8h
    sqxtun v30.8b, v30.8h

    subs w5, w5, #4
    st1 {v27.s}[0], [x2], x3
    st1 {v28.s}[0], [x2], x3
    st1 {v29.s}[0], [x2], x3
    st1 {v30.s}[0], [x2], x3
    bgt if_hor_ver_luma_w4_loop_y

    ld1 {v8.2d}, [sp], #16
    ret

//void uavs3e_if_hor_ver_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w8_arm64

    // x17-->tmp
    mov x15, #72                            // (64 + 8) * 8 * sizeof(short)
    lsl x15, x15, #4
    sub x17, sp, x15

    sub x0, x0, x1, lsl #1                  // src -= 3 * i_src;
    mov sp,  x17
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                          // src -= 3
//--------------------------------
// HOR
//--------------------------------
    ld1 {v22.8b, v23.8b}, [x0], x1
    ext v24.8b, v22.8b, v23.8b, #1
    ext v25.8b, v22.8b, v23.8b, #2
    ext v26.8b, v22.8b, v23.8b, #3
    ext v27.8b, v22.8b, v23.8b, #4
    ext v28.8b, v22.8b, v23.8b, #5
    ext v29.8b, v22.8b, v23.8b, #6
    ext v30.8b, v22.8b, v23.8b, #7

    umull v17.8h, v24.8b, v1.8b
    umlsl v17.8h, v22.8b, v0.8b
    umlsl v17.8h, v25.8b, v2.8b
    umlal v17.8h, v26.8b, v3.8b
    umlal v17.8h, v27.8b, v4.8b
    umlsl v17.8h, v28.8b, v5.8b
    umlal v17.8h, v29.8b, v6.8b
    umlsl v17.8h, v30.8b, v7.8b

    st1 {v17.8h}, [x17], #16

    add w8, w5, #6
if_hor_ver_luma_w8_hor_loop_y:
    ld1 {v20.16b}, [x0], x1                 // src[x-3]
    ld1 {v21.16b}, [x0], x1
    zip1 v22.16b, v20.16b, v21.16b
    zip2 v23.16b, v20.16b, v21.16b

    ext v24.16b, v22.16b, v23.16b, #2
    ext v25.16b, v22.16b, v23.16b, #4
    ext v26.16b, v22.16b, v23.16b, #6
    //ext v27.16b, v22.16b, v23.16b, #8
    ext v28.16b, v22.16b, v23.16b, #10
    ext v29.16b, v22.16b, v23.16b, #12
    ext v30.16b, v22.16b, v23.16b, #14

    umull v17.8h, v24.8b, v1.8b
    umull2 v18.8h, v24.16b, v1.16b
    umlsl v17.8h, v22.8b, v0.8b
    umlsl2 v18.8h, v22.16b, v0.16b
    umlsl v17.8h, v25.8b, v2.8b
    umlsl2 v18.8h, v25.16b, v2.16b
    umlal v17.8h, v26.8b, v3.8b
    umlal2 v18.8h, v26.16b, v3.16b
    umlal2 v17.8h, v22.16b, v4.16b
    umlal v18.8h, v23.8b, v4.8b
    umlsl v17.8h, v28.8b, v5.8b
    umlsl2 v18.8h, v28.16b, v5.16b
    umlal v17.8h, v29.8b, v6.8b
    umlal2 v18.8h, v29.16b, v6.16b
    umlsl v17.8h, v30.8b, v7.8b
    umlsl2 v18.8h, v30.16b, v7.16b

    uzp1 v24.8h, v17.8h, v18.8h
    uzp2 v25.8h, v17.8h, v18.8h

    subs w8, w8, #2
    st1 {v24.8h, v25.8h}, [x17], #32
    bgt if_hor_ver_luma_w8_hor_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------

    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit
if_hor_ver_luma_w8_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v24.8h, v25.8h}, [x17], #32
    ld1 {v31.8h}, [x17]

    smull  v26.4s, v16.4h, v0.h[0]
    smull2 v27.4s, v16.8h, v0.h[0]
    smull  v28.4s, v17.4h, v0.h[0]
    smull2 v29.4s, v17.8h, v0.h[0]

    smlal  v26.4s, v17.4h, v0.h[1]
    smlal2 v27.4s, v17.8h, v0.h[1]
    smlal  v28.4s, v18.4h, v0.h[1]
    smlal2 v29.4s, v18.8h, v0.h[1]

    smlal  v26.4s, v18.4h, v0.h[2]
    smlal2 v27.4s, v18.8h, v0.h[2]
    smlal  v28.4s, v19.4h, v0.h[2]
    smlal2 v29.4s, v19.8h, v0.h[2]

    smlal  v26.4s, v19.4h, v0.h[3]
    smlal2 v27.4s, v19.8h, v0.h[3]
    smlal  v28.4s, v20.4h, v0.h[3]
    smlal2 v29.4s, v20.8h, v0.h[3]

    smlal  v26.4s, v20.4h, v0.h[4]
    smlal2 v27.4s, v20.8h, v0.h[4]
    smlal  v28.4s, v21.4h, v0.h[4]
    smlal2 v29.4s, v21.8h, v0.h[4]

    smlal  v26.4s, v21.4h, v0.h[5]
    smlal2 v27.4s, v21.8h, v0.h[5]
    smlal  v28.4s, v22.4h, v0.h[5]
    smlal2 v29.4s, v22.8h, v0.h[5]

    smlal  v26.4s, v22.4h, v0.h[6]
    smlal2 v27.4s, v22.8h, v0.h[6]
    smlal  v28.4s, v23.4h, v0.h[6]
    smlal2 v29.4s, v23.8h, v0.h[6]
    smlal  v26.4s, v23.4h, v0.h[7]
    smlal2 v27.4s, v23.8h, v0.h[7]
    smlal  v28.4s, v24.4h, v0.h[7]
    smlal2 v29.4s, v24.8h, v0.h[7]

    smull  v4.4s, v18.4h, v0.h[0]
    smull2 v5.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v4.4s, v19.4h, v0.h[1]
    smlal2 v5.4s, v19.8h, v0.h[1]
    smlal  v6.4s, v20.4h, v0.h[1]
    smlal2 v7.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v20.4h, v0.h[2]
    smlal2 v5.4s, v20.8h, v0.h[2]
    smlal  v6.4s, v21.4h, v0.h[2]
    smlal2 v7.4s, v21.8h, v0.h[2]
    smlal  v4.4s, v21.4h, v0.h[3]
    smlal2 v5.4s, v21.8h, v0.h[3]
    smlal  v6.4s, v22.4h, v0.h[3]
    smlal2 v7.4s, v22.8h, v0.h[3]
    smlal  v4.4s, v22.4h, v0.h[4]
    smlal2 v5.4s, v22.8h, v0.h[4]
    smlal  v6.4s, v23.4h, v0.h[4]
    smlal2 v7.4s, v23.8h, v0.h[4]
    smlal  v4.4s, v23.4h, v0.h[5]
    smlal2 v5.4s, v23.8h, v0.h[5]
    smlal  v6.4s, v24.4h, v0.h[5]
    smlal2 v7.4s, v24.8h, v0.h[5]
    smlal  v4.4s, v24.4h, v0.h[6]
    smlal2 v5.4s, v24.8h, v0.h[6]
    smlal  v6.4s, v25.4h, v0.h[6]
    smlal2 v7.4s, v25.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[7]
    smlal2 v5.4s, v25.8h, v0.h[7]
    smlal  v6.4s, v31.4h, v0.h[7]
    smlal2 v7.4s, v31.8h, v0.h[7]

    rshrn  v26.4h, v26.4s, #12
    rshrn2 v26.8h, v27.4s, #12
    rshrn  v27.4h, v28.4s, #12
    rshrn2 v27.8h, v29.4s, #12
    rshrn  v28.4h, v4.4s, #12
    rshrn2 v28.8h, v5.4s, #12
    rshrn  v29.4h, v6.4s, #12
    rshrn2 v29.8h, v7.4s, #12

    sqxtun v26.8b, v26.8h               // 16bit to 8bit
    sqxtun v27.8b, v27.8h
    sqxtun v28.8b, v28.8h
    sqxtun v29.8b, v29.8h

    subs w5, w5, #4
    mov x17, x10
    st1 {v26.8b}, [x2], x3
    st1 {v27.8b}, [x2], x3
    st1 {v28.8b}, [x2], x3
    st1 {v29.8b}, [x2], x3
    bgt if_hor_ver_luma_w8_ver_loop_y

    add sp, sp, x15                         // (64 + 8) * 64 * sizeof(short)

    ret

//void uavs3e_if_hor_ver_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w16_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]

    // x17-->tmp
    mov x15, #136                            // (128 + 8) * 16 * sizeof(short)
    lsl x15, x15, #5
    sub x17, sp, x15

    sub x0, x0, x1, lsl #1                  // src += -3 * i_src;
    mov sp, x17
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    mov x16, #32
    sub x0, x0, #3                          // src -= 3
//--------------------------------
// HOR
//--------------------------------
    ld1 {v20.16b, v21.16b}, [x0], x1           // src[x-3]

    ext v22.16b, v20.16b, v21.16b, #1
    ext v23.16b, v20.16b, v21.16b, #2
    ext v24.16b, v20.16b, v21.16b, #3
    ext v25.16b, v20.16b, v21.16b, #4
    ext v26.16b, v20.16b, v21.16b, #5
    ext v27.16b, v20.16b, v21.16b, #6
    ext v28.16b, v20.16b, v21.16b, #7

    umull  v17.8h, v22.8b, v1.8b
    umull2 v18.8h, v22.16b, v1.16b
    umlsl  v17.8h, v20.8b, v0.8b
    umlsl2 v18.8h, v20.16b, v0.16b
    umlsl  v17.8h, v23.8b, v2.8b
    umlsl2 v18.8h, v23.16b, v2.16b
    umlal  v17.8h, v24.8b, v3.8b
    umlal2 v18.8h, v24.16b, v3.16b
    umlal  v17.8h, v25.8b, v4.8b
    umlal2 v18.8h, v25.16b, v4.16b
    umlsl  v17.8h, v26.8b, v5.8b
    umlsl2 v18.8h, v26.16b, v5.16b
    umlal  v17.8h, v27.8b, v6.8b
    umlal2 v18.8h, v27.16b, v6.16b
    umlsl  v17.8h, v28.8b, v7.8b
    umlsl2 v18.8h, v28.16b, v7.16b

    st1 {v17.8h, v18.8h}, [x17], #32

    add w8, w5, #6
if_hor_ver_luma_w16_hor_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], x1           // src[x-3]
    ld1 {v30.16b, v31.16b}, [x0], x1

    ext v22.16b, v20.16b, v21.16b, #1
    ext v23.16b, v20.16b, v21.16b, #2
    ext v24.16b, v20.16b, v21.16b, #3
    ext v25.16b, v20.16b, v21.16b, #4
    ext v26.16b, v20.16b, v21.16b, #5
    ext v27.16b, v20.16b, v21.16b, #6
    ext v28.16b, v20.16b, v21.16b, #7

    ext v12.16b, v30.16b, v31.16b, #1
    ext v13.16b, v30.16b, v31.16b, #2
    ext v14.16b, v30.16b, v31.16b, #3
    ext v15.16b, v30.16b, v31.16b, #4
    ext v21.16b, v30.16b, v31.16b, #5
    ext v29.16b, v30.16b, v31.16b, #6
    ext v31.16b, v30.16b, v31.16b, #7

    umull  v16.8h, v22.8b, v1.8b
    umull2 v17.8h, v22.16b, v1.16b
    umull  v18.8h, v12.8b, v1.8b
    umull2 v19.8h, v12.16b, v1.16b
    umlsl  v16.8h, v20.8b, v0.8b
    umlsl2 v17.8h, v20.16b, v0.16b
    umlsl  v18.8h, v30.8b, v0.8b
    umlsl2 v19.8h, v30.16b, v0.16b
    umlsl  v16.8h, v23.8b, v2.8b
    umlsl2 v17.8h, v23.16b, v2.16b
    umlsl  v18.8h, v13.8b, v2.8b
    umlsl2 v19.8h, v13.16b, v2.16b
    umlal  v16.8h, v24.8b, v3.8b
    umlal2 v17.8h, v24.16b, v3.16b
    umlal  v18.8h, v14.8b, v3.8b
    umlal2 v19.8h, v14.16b, v3.16b
    umlal  v16.8h, v25.8b, v4.8b
    umlal2 v17.8h, v25.16b, v4.16b
    umlal  v18.8h, v15.8b, v4.8b
    umlal2 v19.8h, v15.16b, v4.16b
    umlsl  v16.8h, v26.8b, v5.8b
    umlsl2 v17.8h, v26.16b, v5.16b
    umlsl  v18.8h, v21.8b, v5.8b
    umlsl2 v19.8h, v21.16b, v5.16b
    umlal  v16.8h, v27.8b, v6.8b
    umlal2 v17.8h, v27.16b, v6.16b
    umlal  v18.8h, v29.8b, v6.8b
    umlal2 v19.8h, v29.16b, v6.16b
    umlsl  v16.8h, v28.8b, v7.8b
    umlsl2 v17.8h, v28.16b, v7.16b
    umlsl  v18.8h, v31.8b, v7.8b
    umlsl2 v19.8h, v31.16b, v7.16b

    subs w8, w8, #2
    st1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    bgt if_hor_ver_luma_w16_hor_loop_y

//--------------------------------
// VER
//--------------------------------

    mov x10, sp                     // tmp
    // load coeffs
    ld1 {v0.d}[0], [x7]             // load coeff
    sxtl v0.8h, v0.8b               // 8bit to 16bit
if_hor_ver_luma_w16_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], #64
    mov x17, x10
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], #64
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], #64
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10], #64

    smull  v6.4s, v16.4h, v0.h[0]
    smull2 v7.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smlal  v6.4s, v18.4h, v0.h[1]
    smlal2 v7.4s, v18.8h, v0.h[1]
    smlal  v4.4s, v19.4h, v0.h[1]
    smlal2 v5.4s, v19.8h, v0.h[1]

    smlal  v6.4s, v20.4h, v0.h[2]
    smlal2 v7.4s, v20.8h, v0.h[2]
    smlal  v4.4s, v21.4h, v0.h[2]
    smlal2 v5.4s, v21.8h, v0.h[2]
    smlal  v6.4s, v22.4h, v0.h[3]
    smlal2 v7.4s, v22.8h, v0.h[3]
    smlal  v4.4s, v23.4h, v0.h[3]
    smlal2 v5.4s, v23.8h, v0.h[3]

    smlal  v6.4s, v24.4h, v0.h[4]
    smlal2 v7.4s, v24.8h, v0.h[4]
    smlal  v4.4s, v25.4h, v0.h[4]
    smlal2 v5.4s, v25.8h, v0.h[4]
    smlal  v6.4s, v26.4h, v0.h[5]
    smlal2 v7.4s, v26.8h, v0.h[5]
    smlal  v4.4s, v27.4h, v0.h[5]
    smlal2 v5.4s, v27.8h, v0.h[5]

    ld1 {v16.8h, v17.8h}, [x10]         // x+4*i_src

    smlal  v6.4s, v28.4h, v0.h[6]
    smlal2 v7.4s, v28.8h, v0.h[6]
    smlal  v4.4s, v29.4h, v0.h[6]
    smlal2 v5.4s, v29.8h, v0.h[6]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v4.4s, v31.4h, v0.h[7]
    smlal2 v5.4s, v31.8h, v0.h[7]

    rshrn  v6.4h, v6.4s, #12
    rshrn2 v6.8h, v7.4s, #12
    rshrn  v7.4h, v4.4s, #12
    rshrn2 v7.8h, v5.4s, #12
    sqxtun v6.8b, v6.8h
    sqxtun v7.8b, v7.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v4.4s, v19.4h, v0.h[0]
    smull2 v5.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]

    st1 {v6.8b, v7.8b}, [x2], x3

    smlal  v2.4s, v22.4h, v0.h[2]
    smlal2 v3.4s, v22.8h, v0.h[2]
    smlal  v4.4s, v23.4h, v0.h[2]
    smlal2 v5.4s, v23.8h, v0.h[2]
    smlal  v2.4s, v24.4h, v0.h[3]
    smlal2 v3.4s, v24.8h, v0.h[3]
    smlal  v4.4s, v25.4h, v0.h[3]
    smlal2 v5.4s, v25.8h, v0.h[3]

    smlal  v2.4s, v26.4h, v0.h[4]
    smlal2 v3.4s, v26.8h, v0.h[4]
    smlal  v4.4s, v27.4h, v0.h[4]
    smlal2 v5.4s, v27.8h, v0.h[4]
    smlal  v2.4s, v28.4h, v0.h[5]
    smlal2 v3.4s, v28.8h, v0.h[5]
    smlal  v4.4s, v29.4h, v0.h[5]
    smlal2 v5.4s, v29.8h, v0.h[5]

    smlal  v2.4s, v30.4h, v0.h[6]
    smlal2 v3.4s, v30.8h, v0.h[6]
    smlal  v4.4s, v31.4h, v0.h[6]
    smlal2 v5.4s, v31.8h, v0.h[6]
    smlal  v2.4s, v16.4h, v0.h[7]
    smlal2 v3.4s, v16.8h, v0.h[7]
    smlal  v4.4s, v17.4h, v0.h[7]
    smlal2 v5.4s, v17.8h, v0.h[7]

    rshrn  v2.4h, v2.4s, #12
    rshrn2 v2.8h, v3.4s, #12
    rshrn  v3.4h, v4.4s, #12
    rshrn2 v3.8h, v5.4s, #12
    sqxtun v2.8b, v2.8h
    sqxtun v3.8b, v3.8h

    subs w5, w5, #2
    mov  x10, x17
    st1 {v2.8b, v3.8b}, [x2], x3
    bgt if_hor_ver_luma_w16_ver_loop_y

    add sp, sp, x15                     // (128 + 8) * 16 * sizeof(short)
    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64

    ret

//void uavs3e_if_hor_ver_luma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w32_arm64
    sub sp, sp, #32
    st1 {v8.2d, v9.2d}, [sp]

    // x17-->tmp
    mov x15, #136                            // (128 + 8) * 128 * sizeof(short)
    lsl x15, x15, #8
    sub x17, sp, x15

    sub x0, x0, x1, lsl #1                  // src += -3 * i_src;
    mov sp,  x17
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    sub x0, x0, #3                          // src -= 3
//--------------------------------
// HOR
//--------------------------------
    sub x1, x1, #32
    add w8, w5, #7
if_hor_ver_luma_w32_hor_loop_y:
    ld1 {v16.16b, v17.16b}, [x0], #32
    ld1 {v18.16b}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #1
    ext v20.16b, v16.16b, v17.16b, #2
    ext v21.16b, v16.16b, v17.16b, #3
    ext v22.16b, v16.16b, v17.16b, #4
    ext v23.16b, v16.16b, v17.16b, #5
    ext v24.16b, v16.16b, v17.16b, #6
    ext v25.16b, v16.16b, v17.16b, #7

    umull  v26.8h, v19.8b, v1.8b
    umull2 v27.8h, v19.16b, v1.16b
    umlsl  v26.8h, v16.8b, v0.8b
    umlsl2 v27.8h, v16.16b, v0.16b
    umlsl  v26.8h, v20.8b, v2.8b
    umlsl2 v27.8h, v20.16b, v2.16b
    umlal  v26.8h, v21.8b, v3.8b
    umlal2 v27.8h, v21.16b, v3.16b
    umlal  v26.8h, v22.8b, v4.8b
    umlal2 v27.8h, v22.16b, v4.16b
    umlsl  v26.8h, v23.8b, v5.8b
    umlsl2 v27.8h, v23.16b, v5.16b
    umlal  v26.8h, v24.8b, v6.8b
    umlal2 v27.8h, v24.16b, v6.16b
    umlsl  v26.8h, v25.8b, v7.8b
    umlsl2 v27.8h, v25.16b, v7.16b

    ext v19.16b, v17.16b, v18.16b, #1
    ext v20.16b, v17.16b, v18.16b, #2
    ext v21.16b, v17.16b, v18.16b, #3
    ext v22.16b, v17.16b, v18.16b, #4
    ext v23.16b, v17.16b, v18.16b, #5
    ext v24.16b, v17.16b, v18.16b, #6
    ext v25.16b, v17.16b, v18.16b, #7

    umull  v28.8h, v19.8b, v1.8b
    umull2 v29.8h, v19.16b, v1.16b
    umlsl  v28.8h, v17.8b, v0.8b
    umlsl2 v29.8h, v17.16b, v0.16b
    umlsl  v28.8h, v20.8b, v2.8b
    umlsl2 v29.8h, v20.16b, v2.16b
    umlal  v28.8h, v21.8b, v3.8b
    umlal2 v29.8h, v21.16b, v3.16b
    umlal  v28.8h, v22.8b, v4.8b
    umlal2 v29.8h, v22.16b, v4.16b
    umlsl  v28.8h, v23.8b, v5.8b
    umlsl2 v29.8h, v23.16b, v5.16b
    umlal  v28.8h, v24.8b, v6.8b
    umlal2 v29.8h, v24.16b, v6.16b
    umlsl  v28.8h, v25.8b, v7.8b
    umlsl2 v29.8h, v25.16b, v7.16b

    subs w8, w8, #1
    st1 {v26.8h, v27.8h, v28.8h, v29.8h}, [x17], #64
    bgt if_hor_ver_luma_w32_hor_loop_y

    mov x17, sp
//--------------------------------
// VER
//--------------------------------
    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

if_hor_ver_luma_w32_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64    // x-3*i_src
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64    // x-2*i_src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64    // x-i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64    // x
    smull  v2.4s, v16.4h, v0.h[0]
    smull2 v3.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smull  v6.4s, v18.4h, v0.h[0]
    smull2 v7.4s, v18.8h, v0.h[0]
    smull  v8.4s, v19.4h, v0.h[0]
    smull2 v9.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]
    smlal  v6.4s, v22.4h, v0.h[1]
    smlal2 v7.4s, v22.8h, v0.h[1]
    smlal  v8.4s, v23.4h, v0.h[1]
    smlal2 v9.4s, v23.8h, v0.h[1]

    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64    // x+i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64    // x+2*i_src

    smlal  v2.4s, v24.4h, v0.h[2]
    smlal2 v3.4s, v24.8h, v0.h[2]
    smlal  v4.4s, v25.4h, v0.h[2]
    smlal2 v5.4s, v25.8h, v0.h[2]
    smlal  v6.4s, v26.4h, v0.h[2]
    smlal2 v7.4s, v26.8h, v0.h[2]
    smlal  v8.4s, v27.4h, v0.h[2]
    smlal2 v9.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v28.4h, v0.h[3]
    smlal2 v3.4s, v28.8h, v0.h[3]
    smlal  v4.4s, v29.4h, v0.h[3]
    smlal2 v5.4s, v29.8h, v0.h[3]
    smlal  v6.4s, v30.4h, v0.h[3]
    smlal2 v7.4s, v30.8h, v0.h[3]
    smlal  v8.4s, v31.4h, v0.h[3]
    smlal2 v9.4s, v31.8h, v0.h[3]

    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64    // x+3*i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17]         // x+4*i_src

    smlal  v2.4s, v16.4h, v0.h[4]
    smlal2 v3.4s, v16.8h, v0.h[4]
    smlal  v4.4s, v17.4h, v0.h[4]
    smlal2 v5.4s, v17.8h, v0.h[4]
    smlal  v6.4s, v18.4h, v0.h[4]
    smlal2 v7.4s, v18.8h, v0.h[4]
    smlal  v8.4s, v19.4h, v0.h[4]
    smlal2 v9.4s, v19.8h, v0.h[4]
    smlal  v2.4s, v20.4h, v0.h[5]
    smlal2 v3.4s, v20.8h, v0.h[5]
    smlal  v4.4s, v21.4h, v0.h[5]
    smlal2 v5.4s, v21.8h, v0.h[5]
    smlal  v6.4s, v22.4h, v0.h[5]
    smlal2 v7.4s, v22.8h, v0.h[5]
    smlal  v8.4s, v23.4h, v0.h[5]
    smlal2 v9.4s, v23.8h, v0.h[5]

    smlal  v2.4s, v24.4h, v0.h[6]
    smlal2 v3.4s, v24.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[6]
    smlal2 v5.4s, v25.8h, v0.h[6]
    smlal  v6.4s, v26.4h, v0.h[6]
    smlal2 v7.4s, v26.8h, v0.h[6]
    smlal  v8.4s, v27.4h, v0.h[6]
    smlal2 v9.4s, v27.8h, v0.h[6]
    smlal  v2.4s, v28.4h, v0.h[7]
    smlal2 v3.4s, v28.8h, v0.h[7]
    smlal  v4.4s, v29.4h, v0.h[7]
    smlal2 v5.4s, v29.8h, v0.h[7]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v8.4s, v31.4h, v0.h[7]
    smlal2 v9.4s, v31.8h, v0.h[7]

    mov x17, x10
    rshrn  v2.4h, v2.4s, #12
    rshrn2 v2.8h, v3.4s, #12
    rshrn  v3.4h, v4.4s, #12
    rshrn2 v3.8h, v5.4s, #12
    rshrn  v4.4h, v6.4s, #12
    rshrn2 v4.8h, v7.4s, #12
    rshrn  v5.4h, v8.4s, #12
    rshrn2 v5.8h, v9.4s, #12

    sqxtun v2.8b, v2.8h
    sqxtun v3.8b, v3.8h
    sqxtun v4.8b, v4.8h
    sqxtun v5.8b, v5.8h

    subs w5, w5, #1
    st1 {v2.8b, v3.8b, v4.8b, v5.8b}, [x2], x3
    bgt if_hor_ver_luma_w32_ver_loop_y

    add sp, sp, x15                     // (128 + 8) * 128 * sizeof(short)

    ld1 {v8.2d, v9.2d}, [sp], #32
    ret

//void uavs3e_if_hor_ver_luma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w32x_arm64
    sub sp, sp, #32
    st1 {v8.8h, v9.8h}, [sp]

    // x17-->tmp
    mov x15, #136                           // (128 + 8) * 128 * sizeof(short)
    lsl x15, x15, #8
    sub x17, sp, x15

    sub x0, x0, x1, lsl #1                  // src += -3 * i_src;
    mov sp, x17
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    abs v7.8b, v18.8b
    dup v0.16b, v7.b[0]
    dup v1.16b, v7.b[1]
    dup v2.16b, v7.b[2]
    dup v3.16b, v7.b[3]
    dup v4.16b, v7.b[4]
    dup v5.16b, v7.b[5]
    dup v6.16b, v7.b[6]
    dup v7.16b, v7.b[7]

    lsl x16, x4, #1                         // i_tmp = width * sizeof(short)
    sub x0, x0, #3                          // src -= 3
//--------------------------------
// HOR
//--------------------------------
    add w8, w5, #7
if_hor_ver_luma_w32x_hor_loop_y:
    mov x9, #0
    mov x10, x0
    mov x11, x17
if_hor_ver_luma_w32x_hor_loop_x:
    ld1 {v16.16b, v17.16b}, [x10], #32      // src[x-3]
    ld1 {v18.16b}, [x10]

    ext v19.16b, v16.16b, v17.16b, #1
    ext v20.16b, v16.16b, v17.16b, #2
    ext v21.16b, v16.16b, v17.16b, #3
    ext v22.16b, v16.16b, v17.16b, #4
    ext v23.16b, v16.16b, v17.16b, #5
    ext v24.16b, v16.16b, v17.16b, #6
    ext v25.16b, v16.16b, v17.16b, #7

    umull  v26.8h, v19.8b, v1.8b
    umull2 v27.8h, v19.16b, v1.16b
    umlsl  v26.8h, v16.8b, v0.8b
    umlsl2 v27.8h, v16.16b, v0.16b
    umlsl  v26.8h, v20.8b, v2.8b
    umlsl2 v27.8h, v20.16b, v2.16b
    umlal  v26.8h, v21.8b, v3.8b
    umlal2 v27.8h, v21.16b, v3.16b
    umlal  v26.8h, v22.8b, v4.8b
    umlal2 v27.8h, v22.16b, v4.16b
    umlsl  v26.8h, v23.8b, v5.8b
    umlsl2 v27.8h, v23.16b, v5.16b
    umlal  v26.8h, v24.8b, v6.8b
    umlal2 v27.8h, v24.16b, v6.16b
    umlsl  v26.8h, v25.8b, v7.8b
    umlsl2 v27.8h, v25.16b, v7.16b

    ext v19.16b, v17.16b, v18.16b, #1
    ext v20.16b, v17.16b, v18.16b, #2
    ext v21.16b, v17.16b, v18.16b, #3
    ext v22.16b, v17.16b, v18.16b, #4
    ext v23.16b, v17.16b, v18.16b, #5
    ext v24.16b, v17.16b, v18.16b, #6
    ext v25.16b, v17.16b, v18.16b, #7

    umull  v28.8h, v19.8b, v1.8b
    umull2 v29.8h, v19.16b, v1.16b
    umlsl  v28.8h, v17.8b, v0.8b
    umlsl2 v29.8h, v17.16b, v0.16b
    umlsl  v28.8h, v20.8b, v2.8b
    umlsl2 v29.8h, v20.16b, v2.16b
    umlal  v28.8h, v21.8b, v3.8b
    umlal2 v29.8h, v21.16b, v3.16b
    umlal  v28.8h, v22.8b, v4.8b
    umlal2 v29.8h, v22.16b, v4.16b
    umlsl  v28.8h, v23.8b, v5.8b
    umlsl2 v29.8h, v23.16b, v5.16b
    umlal  v28.8h, v24.8b, v6.8b
    umlal2 v29.8h, v24.16b, v6.16b
    umlsl  v28.8h, v25.8b, v7.8b
    umlsl2 v29.8h, v25.16b, v7.16b

    add x9, x9, #32
    st1 {v26.8h, v27.8h, v28.8h, v29.8h}, [x11], #64

//--------------------------------
// loop control
//--------------------------------
    cmp x9, x4
    blt if_hor_ver_luma_w32x_hor_loop_x

    subs w8, w8, #1                     // height--
    add x0, x0, x1
    add x17, x17, x16
    bgt if_hor_ver_luma_w32x_hor_loop_y

    mov x17, sp
//--------------------------------
// VER
//--------------------------------
    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

    mov w8, #0
if_hor_ver_luma_w32x_ver_loop_y:
    mov x9, #0
    mov x11, x2
if_hor_ver_luma_w32x_ver_loop_x:
    add x10, x17, x9
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16    // x-3*i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16    // x-2*i_src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16    // x-i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10], x16    // x
    smull  v2.4s, v16.4h, v0.h[0]
    smull2 v3.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smull  v6.4s, v18.4h, v0.h[0]
    smull2 v7.4s, v18.8h, v0.h[0]
    smull  v8.4s, v19.4h, v0.h[0]
    smull2 v9.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]
    smlal  v6.4s, v22.4h, v0.h[1]
    smlal2 v7.4s, v22.8h, v0.h[1]
    smlal  v8.4s, v23.4h, v0.h[1]
    smlal2 v9.4s, v23.8h, v0.h[1]

    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16    // x+i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16    // x+2*i_src

    smlal  v2.4s, v24.4h, v0.h[2]
    smlal2 v3.4s, v24.8h, v0.h[2]
    smlal  v4.4s, v25.4h, v0.h[2]
    smlal2 v5.4s, v25.8h, v0.h[2]
    smlal  v6.4s, v26.4h, v0.h[2]
    smlal2 v7.4s, v26.8h, v0.h[2]
    smlal  v8.4s, v27.4h, v0.h[2]
    smlal2 v9.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v28.4h, v0.h[3]
    smlal2 v3.4s, v28.8h, v0.h[3]
    smlal  v4.4s, v29.4h, v0.h[3]
    smlal2 v5.4s, v29.8h, v0.h[3]
    smlal  v6.4s, v30.4h, v0.h[3]
    smlal2 v7.4s, v30.8h, v0.h[3]
    smlal  v8.4s, v31.4h, v0.h[3]
    smlal2 v9.4s, v31.8h, v0.h[3]

    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16    // x+3*i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10]         // x+4*i_src

    smlal  v2.4s, v16.4h, v0.h[4]
    smlal2 v3.4s, v16.8h, v0.h[4]
    smlal  v4.4s, v17.4h, v0.h[4]
    smlal2 v5.4s, v17.8h, v0.h[4]
    smlal  v6.4s, v18.4h, v0.h[4]
    smlal2 v7.4s, v18.8h, v0.h[4]
    smlal  v8.4s, v19.4h, v0.h[4]
    smlal2 v9.4s, v19.8h, v0.h[4]
    smlal  v2.4s, v20.4h, v0.h[5]
    smlal2 v3.4s, v20.8h, v0.h[5]
    smlal  v4.4s, v21.4h, v0.h[5]
    smlal2 v5.4s, v21.8h, v0.h[5]
    smlal  v6.4s, v22.4h, v0.h[5]
    smlal2 v7.4s, v22.8h, v0.h[5]
    smlal  v8.4s, v23.4h, v0.h[5]
    smlal2 v9.4s, v23.8h, v0.h[5]

    smlal  v2.4s, v24.4h, v0.h[6]
    smlal2 v3.4s, v24.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[6]
    smlal2 v5.4s, v25.8h, v0.h[6]
    smlal  v6.4s, v26.4h, v0.h[6]
    smlal2 v7.4s, v26.8h, v0.h[6]
    smlal  v8.4s, v27.4h, v0.h[6]
    smlal2 v9.4s, v27.8h, v0.h[6]
    smlal  v2.4s, v28.4h, v0.h[7]
    smlal2 v3.4s, v28.8h, v0.h[7]
    smlal  v4.4s, v29.4h, v0.h[7]
    smlal2 v5.4s, v29.8h, v0.h[7]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v8.4s, v31.4h, v0.h[7]
    smlal2 v9.4s, v31.8h, v0.h[7]

    rshrn  v2.4h, v2.4s, #12
    rshrn2 v2.8h, v3.4s, #12
    rshrn  v3.4h, v4.4s, #12
    rshrn2 v3.8h, v5.4s, #12
    rshrn  v4.4h, v6.4s, #12
    rshrn2 v4.8h, v7.4s, #12
    rshrn  v5.4h, v8.4s, #12
    rshrn2 v5.8h, v9.4s, #12

    sqxtun v2.8b, v2.8h
    sqxtun v3.8b, v3.8h
    sqxtun v4.8b, v4.8h
    sqxtun v5.8b, v5.8h

    add x9, x9, #64
    st1 {v2.8b, v3.8b, v4.8b, v5.8b}, [x11], #32

//--------------------------------
// loop control
//--------------------------------
    cmp x9, x16
    blt if_hor_ver_luma_w32x_ver_loop_x

    subs w5, w5, #1
    add x17, x17, x16                   // src += i_src
    add x2, x2, x3                      // dst += i_dst
    bgt if_hor_ver_luma_w32x_ver_loop_y

    add sp, sp, x15                     // (128 + 8) * 128 * sizeof(short)

    ld1 {v8.8h, v9.8h}, [sp], #32
    ret

#else   //10bit
//void uavs3e_if_cpy_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w4_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
if_cpy_w4_loop_y:
    ld1 {v0.4h}, [x0], x1
    ld1 {v1.4h}, [x0], x1
    ld1 {v2.4h}, [x0], x1
    ld1 {v3.4h}, [x0], x1
    st1 {v0.4h}, [x2], x3
    st1 {v1.4h}, [x2], x3
    subs w5, w5, #4
    st1 {v2.4h}, [x2], x3
    st1 {v3.4h}, [x2], x3
    bgt if_cpy_w4_loop_y

    ret

//void uavs3e_if_cpy_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
if_cpy_w8_loop_y:
    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1

    subs w5, w5, #4
    st1 {v0.8h}, [x2], x3
    st1 {v1.8h}, [x2], x3
    st1 {v2.8h}, [x2], x3
    st1 {v3.8h}, [x2], x3
    bgt if_cpy_w8_loop_y

    ret

//void uavs3e_if_cpy_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w16_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
if_cpy_w16_loop_y:
    ld1 {v0.2d, v1.2d}, [x0], x1
    ld1 {v2.2d, v3.2d}, [x0], x1
    ld1 {v4.2d, v5.2d}, [x0], x1
    ld1 {v6.2d, v7.2d}, [x0], x1

    subs w5, w5, #4
    st1 {v0.2d, v1.2d}, [x2], x3
    st1 {v2.2d, v3.2d}, [x2], x3
    st1 {v4.2d, v5.2d}, [x2], x3
    st1 {v6.2d, v7.2d}, [x2], x3
    bgt if_cpy_w16_loop_y

    ret

//void uavs3e_if_cpy_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w32_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
if_cpy_w32_loop_y:
    ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], x1
    ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x0], x1
    ld1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x0], x1
    ld1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x0], x1

    subs w5, w5, #4
    st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x2], x3
    st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x2], x3
    st1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x2], x3
    st1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x2], x3
    bne if_cpy_w32_loop_y

    ret

//void uavs3e_if_cpy_w64_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w64_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    sub x1, x1, #64
    sub x3, x3, #64
if_cpy_w64_loop_y:
    ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #64
    ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x0], x1
    ld1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x0], #64
    ld1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x0], x1

    subs w5, w5, #2
    st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x2], #64
    st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x2], x3
    st1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x2], #64
    st1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x2], x3
    bgt if_cpy_w64_loop_y

    ret

//void uavs3e_if_cpy_w128_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5
function uavs3e_if_cpy_w128_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    sub x1, x1, #192
    sub x3, x3, #192
if_cpy_w128_loop_y:
    ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #64
    ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x0], #64
    ld1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x0], #64
    ld1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x0], x1

    subs w5, w5, #1
    st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x2], #64
    st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [x2], #64
    st1 {v18.2d, v19.2d, v20.2d, v21.2d}, [x2], #64
    st1 {v22.2d, v23.2d, v24.2d, v25.2d}, [x2], x3
    bgt if_cpy_w128_loop_y

    ret


//void uavs3e_if_hor_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w4_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.4h, v7.h[0]
    dup v1.4h, v7.h[1]
    dup v2.4h, v7.h[2]
    dup v3.4h, v7.h[3]
    dup v4.4h, v7.h[4]
    dup v5.4h, v7.h[5]
    dup v6.4h, v7.h[6]
    dup v7.4h, v7.h[7]

    sub x0, x0, #6                      // x-3
    sub x1, x1, #16
    dup v31.8h, w7                      // max_val
if_hor_luma_w4_loop_y:
    ld1 {v20.4h, v21.4h}, [x0], #16     // src[x-3]
    ld1 {v16.4h}, [x0], x1
    ld1 {v22.4h, v23.4h}, [x0], #16
    ld1 {v19.4h}, [x0], x1

    ext v24.8b, v20.8b, v21.8b, #2      // src[x-2]
    ext v25.8b, v20.8b, v21.8b, #4      // src[x-1]
    ext v26.8b, v20.8b, v21.8b, #6      // src[x]
    umull v17.4s, v24.4h, v1.4h
    // v21 : src[x+1]
    umlsl v17.4s, v20.4h, v0.4h
    ext v28.8b, v21.8b, v16.8b, #2      // src[x+2]
    umlsl v17.4s, v25.4h, v2.4h
    ext v29.8b, v21.8b, v16.8b, #4      // src[x+3]
    umlal v17.4s, v26.4h, v3.4h
    ext v30.8b, v21.8b, v16.8b, #6      // src[x+4]
    umlal v17.4s, v21.4h, v4.4h
    ext v24.8b, v22.8b, v23.8b, #2
    umlsl v17.4s, v28.4h, v5.4h
    ext v25.8b, v22.8b, v23.8b, #4
    umull v18.4s, v24.4h, v1.4h
    umlal v17.4s, v29.4h, v6.4h
    umlsl v18.4s, v22.4h, v0.4h
    ext v26.8b, v22.8b, v23.8b, #6
    umlsl v18.4s, v25.4h, v2.4h
    umlsl v17.4s, v30.4h, v7.4h
    // v23 : src[x+1+i_src]
    umlal v18.4s, v26.4h, v3.4h
    ext v28.8b, v23.8b, v19.8b, #2
    umlal v18.4s, v23.4h, v4.4h
    ext v29.8b, v23.8b, v19.8b, #4
    umlsl v18.4s, v28.4h, v5.4h
    ext v30.8b, v23.8b, v19.8b, #6
    umlal v18.4s, v29.4h, v6.4h
    sqrshrun v17.4h, v17.4s, #6
    umlsl v18.4s, v30.4h, v7.4h

    //(sum + 32) >> 6
    sqrshrun v18.4h, v18.4s, #6

    umin v17.4h, v17.4h, v31.4h
    umin v18.4h, v18.4h, v31.4h

    subs w5, w5, #2
    st1 {v17.4h}, [x2], x3
    st1 {v18.4h}, [x2], x3
    bgt if_hor_luma_w4_loop_y

    ret

//void uavs3e_if_hor_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b

    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, #6                      // x-3
    dup v31.8h, w7                      // max_val
if_hor_luma_w8_loop_y:
    ld1 {v20.8h, v21.8h}, [x0], x1      // src[x-3]
    ld1 {v22.8h, v23.8h}, [x0], x1

    ext v24.16b, v20.16b, v21.16b, #2
    ext v25.16b, v20.16b, v21.16b, #4
    ext v26.16b, v20.16b, v21.16b, #6
    ext v27.16b, v20.16b, v21.16b, #8
    ext v28.16b, v20.16b, v21.16b, #10
    ext v29.16b, v20.16b, v21.16b, #12
    ext v30.16b, v20.16b, v21.16b, #14

    umull  v17.4s, v24.4h, v1.4h
    umull2 v18.4s, v24.8h, v1.8h
    umlsl  v17.4s, v20.4h, v0.4h
    umlsl2 v18.4s, v20.8h, v0.8h
    umlsl  v17.4s, v25.4h, v2.4h
    umlsl2 v18.4s, v25.8h, v2.8h
    umlal  v17.4s, v26.4h, v3.4h
    umlal2 v18.4s, v26.8h, v3.8h
    umlal  v17.4s, v27.4h, v4.4h
    umlal2 v18.4s, v27.8h, v4.8h
    umlsl  v17.4s, v28.4h, v5.4h
    umlsl2 v18.4s, v28.8h, v5.8h
    umlal  v17.4s, v29.4h, v6.4h
    umlal2 v18.4s, v29.8h, v6.8h
    umlsl  v17.4s, v30.4h, v7.4h
    umlsl2 v18.4s, v30.8h, v7.8h

    ext v24.16b, v22.16b, v23.16b, #2
    ext v25.16b, v22.16b, v23.16b, #4
    ext v26.16b, v22.16b, v23.16b, #6
    ext v27.16b, v22.16b, v23.16b, #8
    ext v28.16b, v22.16b, v23.16b, #10
    ext v29.16b, v22.16b, v23.16b, #12
    ext v30.16b, v22.16b, v23.16b, #14

    //(sum + 32) >> 6
    sqrshrun  v16.4h, v17.4s, #6
    sqrshrun2 v16.8h, v18.4s, #6

    umull  v18.4s, v24.4h, v1.4h
    umull2 v19.4s, v24.8h, v1.8h
    umlsl  v18.4s, v22.4h, v0.4h
    umlsl2 v19.4s, v22.8h, v0.8h
    umlsl  v18.4s, v25.4h, v2.4h
    umlsl2 v19.4s, v25.8h, v2.8h
    umlal  v18.4s, v26.4h, v3.4h
    umlal2 v19.4s, v26.8h, v3.8h
    umlal  v18.4s, v27.4h, v4.4h
    umlal2 v19.4s, v27.8h, v4.8h
    umlsl  v18.4s, v28.4h, v5.4h
    umlsl2 v19.4s, v28.8h, v5.8h
    umlal  v18.4s, v29.4h, v6.4h
    umlal2 v19.4s, v29.8h, v6.8h
    umlsl  v18.4s, v30.4h, v7.4h
    umlsl2 v19.4s, v30.8h, v7.8h

    sqrshrun  v18.4h, v18.4s, #6
    sqrshrun2 v18.8h, v19.4s, #6

    umin v16.8h, v16.8h, v31.8h
    umin v18.8h, v18.8h, v31.8h

    subs w5, w5, #2
    st1 {v16.8h}, [x2], x3
    st1 {v18.8h}, [x2], x3
    bgt if_hor_luma_w8_loop_y

    ret

//void uavs3e_if_hor_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w16_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]
    sub sp, sp, #16
    st1 {v11.2d}, [sp]
    
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, #6                      // x-3
    sub x1, x1, #32
    dup v11.8h, w7
if_hor_luma_w16_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], #32   // src[x-3]
    ld1 {v29.16b}, [x0], x1

    ext v22.16b, v20.16b, v21.16b, #2
    ext v23.16b, v20.16b, v21.16b, #4
    ext v24.16b, v20.16b, v21.16b, #6
    ext v25.16b, v20.16b, v21.16b, #8
    ext v26.16b, v20.16b, v21.16b, #10
    ext v27.16b, v20.16b, v21.16b, #12
    ext v28.16b, v20.16b, v21.16b, #14

    ext v12.16b, v21.16b, v29.16b, #2
    ext v13.16b, v21.16b, v29.16b, #4
    ext v14.16b, v21.16b, v29.16b, #6
    ext v15.16b, v21.16b, v29.16b, #8
    ext v16.16b, v21.16b, v29.16b, #10
    ext v17.16b, v21.16b, v29.16b, #12
    ext v18.16b, v21.16b, v29.16b, #14

    umull  v19.4s, v22.4h, v1.4h
    umull2 v29.4s, v22.8h, v1.8h
    umull  v30.4s, v12.4h, v1.4h
    umull2 v31.4s, v12.8h, v1.8h
    umlsl  v19.4s, v20.4h, v0.4h
    umlsl2 v29.4s, v20.8h, v0.8h
    umlsl  v30.4s, v21.4h, v0.4h
    umlsl2 v31.4s, v21.8h, v0.8h
    umlsl  v19.4s, v23.4h, v2.4h
    umlsl2 v29.4s, v23.8h, v2.8h
    umlsl  v30.4s, v13.4h, v2.4h
    umlsl2 v31.4s, v13.8h, v2.8h
    umlal  v19.4s, v24.4h, v3.4h
    umlal2 v29.4s, v24.8h, v3.8h
    umlal  v30.4s, v14.4h, v3.4h
    umlal2 v31.4s, v14.8h, v3.8h
    umlal  v19.4s, v25.4h, v4.4h
    umlal2 v29.4s, v25.8h, v4.8h
    umlal  v30.4s, v15.4h, v4.4h
    umlal2 v31.4s, v15.8h, v4.8h
    umlsl  v19.4s, v26.4h, v5.4h
    umlsl2 v29.4s, v26.8h, v5.8h
    umlsl  v30.4s, v16.4h, v5.4h
    umlsl2 v31.4s, v16.8h, v5.8h
    umlal  v19.4s, v27.4h, v6.4h
    umlal2 v29.4s, v27.8h, v6.8h
    umlal  v30.4s, v17.4h, v6.4h
    umlal2 v31.4s, v17.8h, v6.8h
    umlsl  v19.4s, v28.4h, v7.4h
    umlsl2 v29.4s, v28.8h, v7.8h
    umlsl  v30.4s, v18.4h, v7.4h
    umlsl2 v31.4s, v18.8h, v7.8h

    //(sum + 32) >> 6
    sqrshrun  v28.4h, v19.4s, #6
    sqrshrun2 v28.8h, v29.4s, #6
    sqrshrun  v30.4h, v30.4s, #6
    sqrshrun2 v30.8h, v31.4s, #6

    umin v28.8h, v28.8h, v11.8h
    umin v29.8h, v30.8h, v11.8h

    subs w5, w5, #1

    st1 {v28.8h, v29.8h}, [x2], x3
    bgt if_hor_luma_w16_loop_y

    ld1 {v11.2d}, [sp], #16
    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64

    ret

//void uavs3e_if_hor_luma_w16x_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_luma_w16x_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]
    sub sp, sp, #16
    st1 {v11.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b

    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, #6                      // x-3
    dup v11.8h, w7                      // max_pix
if_hor_luma_w16x_loop_y:
    mov w9, w4
    mov x10, x0
    mov x11, x2
if_hor_luma_w16x_loop_x:
    ld1 {v20.16b, v21.16b}, [x10], #32  // src[x-3]
    ld1 {v29.16b}, [x10]

    ext v22.16b, v20.16b, v21.16b, #2
    ext v23.16b, v20.16b, v21.16b, #4
    ext v24.16b, v20.16b, v21.16b, #6
    ext v25.16b, v20.16b, v21.16b, #8
    ext v26.16b, v20.16b, v21.16b, #10
    ext v27.16b, v20.16b, v21.16b, #12
    ext v28.16b, v20.16b, v21.16b, #14

    ext v12.16b, v21.16b, v29.16b, #2
    ext v13.16b, v21.16b, v29.16b, #4
    ext v14.16b, v21.16b, v29.16b, #6
    ext v15.16b, v21.16b, v29.16b, #8
    ext v16.16b, v21.16b, v29.16b, #10
    ext v17.16b, v21.16b, v29.16b, #12
    ext v18.16b, v21.16b, v29.16b, #14

    umull  v19.4s, v22.4h, v1.4h
    umull2 v29.4s, v22.8h, v1.8h
    umull  v30.4s, v12.4h, v1.4h
    umull2 v31.4s, v12.8h, v1.8h
    umlsl  v19.4s, v20.4h, v0.4h
    umlsl2 v29.4s, v20.8h, v0.8h
    umlsl  v30.4s, v21.4h, v0.4h
    umlsl2 v31.4s, v21.8h, v0.8h
    umlsl  v19.4s, v23.4h, v2.4h
    umlsl2 v29.4s, v23.8h, v2.8h
    umlsl  v30.4s, v13.4h, v2.4h
    umlsl2 v31.4s, v13.8h, v2.8h
    umlal  v19.4s, v24.4h, v3.4h
    umlal2 v29.4s, v24.8h, v3.8h
    umlal  v30.4s, v14.4h, v3.4h
    umlal2 v31.4s, v14.8h, v3.8h
    umlal  v19.4s, v25.4h, v4.4h
    umlal2 v29.4s, v25.8h, v4.8h
    umlal  v30.4s, v15.4h, v4.4h
    umlal2 v31.4s, v15.8h, v4.8h
    umlsl  v19.4s, v26.4h, v5.4h
    umlsl2 v29.4s, v26.8h, v5.8h
    umlsl  v30.4s, v16.4h, v5.4h
    umlsl2 v31.4s, v16.8h, v5.8h
    umlal  v19.4s, v27.4h, v6.4h
    umlal2 v29.4s, v27.8h, v6.8h
    umlal  v30.4s, v17.4h, v6.4h
    umlal2 v31.4s, v17.8h, v6.8h
    umlsl  v19.4s, v28.4h, v7.4h
    umlsl2 v29.4s, v28.8h, v7.8h
    umlsl  v30.4s, v18.4h, v7.4h
    umlsl2 v31.4s, v18.8h, v7.8h

    //(sum + 32) >> 6
    sqrshrun  v28.4h, v19.4s, #6
    sqrshrun2 v28.8h, v29.4s, #6
    sqrshrun  v30.4h, v30.4s, #6
    sqrshrun2 v30.8h, v31.4s, #6

    umin v28.8h, v28.8h, v11.8h
    umin v29.8h, v30.8h, v11.8h
    subs w9, w9, #16
    st1 {v28.8h, v29.8h}, [x11], #32
    bgt if_hor_luma_w16x_loop_x

    subs w5, w5, #1
    add x0, x0, x1
    add x2, x2, x3
    bgt if_hor_luma_w16x_loop_y

    ld1 {v11.2d}, [sp], #16
    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64

    ret

//void uavs3e_if_hor_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_chroma_w8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v4.s}[0], [x6]
    abs  v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]

    sub x0, x0, #2                      // src - 1
    dup v31.8h, w7                      // max_val

if_hor_chroma_w8_loop_y:
    ld1 {v20.8h, v21.8h}, [x0], x1      // src[x-1]
    ld1 {v22.8h, v23.8h}, [x0], x1
    ext v17.16b, v20.16b, v21.16b, #2   // src[x]
    ext v18.16b, v20.16b, v21.16b, #4   // src[x+1]
    ext v19.16b, v20.16b, v21.16b, #6   // src[x+2]
    ext v24.16b, v22.16b, v23.16b, #2   // src[x]
    ext v25.16b, v22.16b, v23.16b, #4   // src[x+1]
    ext v26.16b, v22.16b, v23.16b, #6   // src[x+2]

    umull  v4.4s, v17.4h, v1.4h
    umull2 v5.4s, v17.8h, v1.8h
    umlsl  v4.4s, v20.4h, v0.4h
    umlsl2 v5.4s, v20.8h, v0.8h
    umlal  v4.4s, v18.4h, v2.4h
    umlal2 v5.4s, v18.8h, v2.8h
    umlsl  v4.4s, v19.4h, v3.4h
    umlsl2 v5.4s, v19.8h, v3.8h

    umull  v6.4s, v24.4h, v1.4h
    umull2 v7.4s, v24.8h, v1.8h
    umlsl  v6.4s, v22.4h, v0.4h
    umlsl2 v7.4s, v22.8h, v0.8h
    umlal  v6.4s, v25.4h, v2.4h
    umlal2 v7.4s, v25.8h, v2.8h
    umlsl  v6.4s, v26.4h, v3.4h
    umlsl2 v7.4s, v26.8h, v3.8h

    sqrshrun v4.4h, v4.4s, #6       // saturate s32 -> u16
    sqrshrun v5.4h, v5.4s, #6
    sqrshrun v6.4h, v6.4s, #6
    sqrshrun v7.4h, v7.4s, #6

    umin v4.4h, v4.4h, v31.4h
    umin v5.4h, v5.4h, v31.4h
    umin v6.4h, v6.4h, v31.4h
    umin v7.4h, v7.4h, v31.4h

    subs w5, w5, #2
    st1 {v4.4h, v5.4h}, [x2], x3
    st1 {v6.4h, v7.4h}, [x2], x3
    bgt if_hor_chroma_w8_loop_y

    ret

//void uavs3e_if_hor_chroma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_chroma_w16_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]

    sub x0, x0, #2                      // src - 1
    sub x1, x1, #32
    dup v31.8h, w7                      // max_val
if_hor_chroma_w16_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], #32     // src[x-1]
    ld1 {v18.8h}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #2   // src[x]
    ext v20.16b, v16.16b, v17.16b, #4   // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6   // src[x+2]
    ext v22.16b, v17.16b, v18.16b, #2   // src[x+8]
    ext v23.16b, v17.16b, v18.16b, #4   // src[x+9]
    ext v24.16b, v17.16b, v18.16b, #6   // src[x+10]

    umull  v4.4s, v19.4h, v1.4h
    umull2 v5.4s, v19.8h, v1.8h
    umull  v6.4s, v22.4h, v1.4h
    umull2 v7.4s, v22.8h, v1.8h
    umlsl  v4.4s, v16.4h, v0.4h
    umlsl2 v5.4s, v16.8h, v0.8h
    umlsl  v6.4s, v17.4h, v0.4h
    umlsl2 v7.4s, v17.8h, v0.8h
    umlal  v4.4s, v20.4h, v2.4h
    umlal2 v5.4s, v20.8h, v2.8h
    umlal  v6.4s, v23.4h, v2.4h
    umlal2 v7.4s, v23.8h, v2.8h
    umlsl  v4.4s, v21.4h, v3.4h
    umlsl2 v5.4s, v21.8h, v3.8h
    umlsl  v6.4s, v24.4h, v3.4h
    umlsl2 v7.4s, v24.8h, v3.8h

    sqrshrun v4.4h, v4.4s, #6           // saturate s32 -> u16
    sqrshrun v5.4h, v5.4s, #6
    sqrshrun v6.4h, v6.4s, #6
    sqrshrun v7.4h, v7.4s, #6

    umin v4.4h, v4.4h, v31.4h
    umin v5.4h, v5.4h, v31.4h
    umin v6.4h, v6.4h, v31.4h
    umin v7.4h, v7.4h, v31.4h

    subs w5, w5, #1
    st1 {v4.4h, v5.4h, v6.4h, v7.4h}, [x2], x3
    bgt if_hor_chroma_w16_loop_y

    ret

//void uavs3e_if_hor_chroma_w16x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_hor_chroma_w16x_arm64

    lsl x1, x1, #1
    lsl x3, x3, #1

    ld1 {v4.s}[0], [x6]
    abs v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]

    sub x0, x0, #2                      // src - 1
    dup v31.8h, w7                      // max_val
if_hor_chroma_w16x_loop_y:
    mov w9, w4
    mov x10, x0
    mov x11, x2
if_hor_chroma_w16x_loop_x:
    ld1 {v16.8h, v17.8h}, [x10], #32     // src[x-1]
    ld1 {v18.8h}, [x10]

    ext v19.16b, v16.16b, v17.16b, #2   // src[x]
    ext v20.16b, v16.16b, v17.16b, #4   // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6   // src[x+2]
    ext v22.16b, v17.16b, v18.16b, #2   // src[x+8]
    ext v23.16b, v17.16b, v18.16b, #4   // src[x+9]
    ext v24.16b, v17.16b, v18.16b, #6   // src[x+10]

    umull  v4.4s, v19.4h, v1.4h
    umull2 v5.4s, v19.8h, v1.8h
    umull  v6.4s, v22.4h, v1.4h
    umull2 v7.4s, v22.8h, v1.8h
    umlsl  v4.4s, v16.4h, v0.4h
    umlsl2 v5.4s, v16.8h, v0.8h
    umlsl  v6.4s, v17.4h, v0.4h
    umlsl2 v7.4s, v17.8h, v0.8h
    umlal  v4.4s, v20.4h, v2.4h
    umlal2 v5.4s, v20.8h, v2.8h
    umlal  v6.4s, v23.4h, v2.4h
    umlal2 v7.4s, v23.8h, v2.8h
    umlsl  v4.4s, v21.4h, v3.4h
    umlsl2 v5.4s, v21.8h, v3.8h
    umlsl  v6.4s, v24.4h, v3.4h
    umlsl2 v7.4s, v24.8h, v3.8h

    sqrshrun v4.4h, v4.4s, #6           // saturate s32 -> u16
    sqrshrun v5.4h, v5.4s, #6
    sqrshrun v6.4h, v6.4s, #6
    sqrshrun v7.4h, v7.4s, #6

    umin v4.4h, v4.4h, v31.4h
    umin v5.4h, v5.4h, v31.4h
    umin v6.4h, v6.4h, v31.4h
    umin v7.4h, v7.4h, v31.4h

    subs w9, w9, #16
    st1 {v4.4h, v5.4h, v6.4h, v7.4h}, [x11], #32
    bgt if_hor_chroma_w16x_loop_x

    subs w5, w5, #1
    add x0, x0, x1
    add x2, x2, x3
    bgt if_hor_chroma_w16x_loop_y

    ret

//void uavs3e_if_ver_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w4_arm64
    sub sp, sp, #16
    st1 {v8.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src
    dup v8.8h, w7
if_ver_luma_w4_loop_y:
    ld1 {v16.4h}, [x0], x1             // x-3*i_src
    ld1 {v17.4h}, [x0], x1             // x-2*i_src
    ld1 {v18.4h}, [x0], x1             // x-i_src
    ld1 {v19.4h}, [x0], x1             // x
    mov x10, x0
    ld1 {v20.4h}, [x0], x1             // x+i_src
    ld1 {v21.4h}, [x0], x1             // x+2*i_src
    ld1 {v22.4h}, [x0], x1             // x+3*i_src
    ld1 {v23.4h}, [x0], x1             // x+4*i_src
    ld1 {v24.4h}, [x0], x1
    ld1 {v30.4h}, [x0], x1
    ld1 {v31.4h}, [x0]

    umull v25.4s, v17.4h, v1.4h
    umull v26.4s, v18.4h, v1.4h
    umlsl v25.4s, v16.4h, v0.4h
    umlsl v26.4s, v17.4h, v0.4h
    umlsl v25.4s, v18.4h, v2.4h
    umlsl v26.4s, v19.4h, v2.4h
    umlal v25.4s, v19.4h, v3.4h
    umlal v26.4s, v20.4h, v3.4h
    umlal v25.4s, v20.4h, v4.4h
    umlal v26.4s, v21.4h, v4.4h
    umlsl v25.4s, v21.4h, v5.4h
    umlsl v26.4s, v22.4h, v5.4h
    umlal v25.4s, v22.4h, v6.4h
    umlal v26.4s, v23.4h, v6.4h
    umlsl v25.4s, v23.4h, v7.4h
    umlsl v26.4s, v24.4h, v7.4h

    umull v27.4s, v19.4h, v1.4h
    umull v28.4s, v20.4h, v1.4h
    umlsl v27.4s, v18.4h, v0.4h
    umlsl v28.4s, v19.4h, v0.4h
    umlsl v27.4s, v20.4h, v2.4h
    umlsl v28.4s, v21.4h, v2.4h
    umlal v27.4s, v21.4h, v3.4h
    umlal v28.4s, v22.4h, v3.4h
    umlal v27.4s, v22.4h, v4.4h
    umlal v28.4s, v23.4h, v4.4h
    umlsl v27.4s, v23.4h, v5.4h
    umlsl v28.4s, v24.4h, v5.4h
    umlal v27.4s, v24.4h, v6.4h
    umlal v28.4s, v30.4h, v6.4h
    umlsl v27.4s, v30.4h, v7.4h
    umlsl v28.4s, v31.4h, v7.4h

    //(sum + 32) >> 6
    sqrshrun  v25.4h, v25.4s, #6
    sqrshrun2 v25.8h, v26.4s, #6
    sqrshrun  v26.4h, v27.4s, #6
    sqrshrun2 v26.8h, v28.4s, #6
    
    umin v25.8h, v25.8h, v8.8h
    umin v26.8h, v26.8h, v8.8h

    subs w5, w5, #4
    mov x0, x10
    st1 {v25.d}[0], [x2], x3
    st1 {v25.d}[1], [x2], x3
    st1 {v26.d}[0], [x2], x3
    st1 {v26.d}[1], [x2], x3
    bgt if_ver_luma_w4_loop_y

    ld1 {v8.2d}, [sp], #16

    ret

//void uavs3e_if_ver_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w8_arm64
    sub sp, sp, #16
    st1 {v8.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src

    dup v8.8h, w7
if_ver_luma_w8_loop_y:
    ld1 {v16.8h}, [x0], x1            // x-3*i_src
    ld1 {v17.8h}, [x0], x1            // x-2*i_src
    ld1 {v18.8h}, [x0], x1            // x-i_src
    ld1 {v19.8h}, [x0], x1            // x
    mov x10, x0
    ld1 {v20.8h}, [x0], x1            // x+i_src
    ld1 {v21.8h}, [x0], x1            // x+2*i_src
    ld1 {v22.8h}, [x0], x1            // x+3*i_src
    ld1 {v23.8h}, [x0], x1            // x+4*i_src
    ld1 {v24.8h}, [x0], x1
    ld1 {v25.8h}, [x0], x1
    ld1 {v26.8h}, [x0]

    umull  v28.4s, v17.4h, v1.4h
    umull2 v29.4s, v17.8h, v1.8h
    umull  v30.4s, v18.4h, v1.4h
    umull2 v31.4s, v18.8h, v1.8h
    umlsl  v28.4s, v16.4h, v0.4h
    umlsl2 v29.4s, v16.8h, v0.8h
    umlsl  v30.4s, v17.4h, v0.4h
    umlsl2 v31.4s, v17.8h, v0.8h
    umlsl  v28.4s, v18.4h, v2.4h
    umlsl2 v29.4s, v18.8h, v2.8h
    umlsl  v30.4s, v19.4h, v2.4h
    umlsl2 v31.4s, v19.8h, v2.8h
    umlal  v28.4s, v19.4h, v3.4h
    umlal2 v29.4s, v19.8h, v3.8h
    umlal  v30.4s, v20.4h, v3.4h
    umlal2 v31.4s, v20.8h, v3.8h
    umlal  v28.4s, v20.4h, v4.4h
    umlal2 v29.4s, v20.8h, v4.8h
    umlal  v30.4s, v21.4h, v4.4h
    umlal2 v31.4s, v21.8h, v4.8h
    umlsl  v28.4s, v21.4h, v5.4h
    umlsl2 v29.4s, v21.8h, v5.8h
    umlsl  v30.4s, v22.4h, v5.4h
    umlsl2 v31.4s, v22.8h, v5.8h
    umlal  v28.4s, v22.4h, v6.4h
    umlal2 v29.4s, v22.8h, v6.8h
    umlal  v30.4s, v23.4h, v6.4h
    umlal2 v31.4s, v23.8h, v6.8h
    umlsl  v28.4s, v23.4h, v7.4h
    umlsl2 v29.4s, v23.8h, v7.8h
    umlsl  v30.4s, v24.4h, v7.4h
    umlsl2 v31.4s, v24.8h, v7.8h

    sqrshrun  v28.4h, v28.4s, #6
    sqrshrun2 v28.8h, v29.4s, #6
    sqrshrun  v29.4h, v30.4s, #6
    sqrshrun2 v29.8h, v31.4s, #6

    umin v28.8h, v28.8h, v8.8h
    umin v29.8h, v29.8h, v8.8h

    st1 {v28.8h}, [x2], x3
    st1 {v29.8h}, [x2], x3

    umull  v16.4s, v19.4h, v1.4h
    umull2 v17.4s, v19.8h, v1.8h
    umull  v28.4s, v20.4h, v1.4h
    umull2 v29.4s, v20.8h, v1.8h
    umlsl  v16.4s, v18.4h, v0.4h
    umlsl2 v17.4s, v18.8h, v0.8h
    umlsl  v28.4s, v19.4h, v0.4h
    umlsl2 v29.4s, v19.8h, v0.8h
    umlsl  v16.4s, v20.4h, v2.4h
    umlsl2 v17.4s, v20.8h, v2.8h
    umlsl  v28.4s, v21.4h, v2.4h
    umlsl2 v29.4s, v21.8h, v2.8h
    umlal  v16.4s, v21.4h, v3.4h
    umlal2 v17.4s, v21.8h, v3.8h
    umlal  v28.4s, v22.4h, v3.4h
    umlal2 v29.4s, v22.8h, v3.8h
    umlal  v16.4s, v22.4h, v4.4h
    umlal2 v17.4s, v22.8h, v4.8h
    umlal  v28.4s, v23.4h, v4.4h
    umlal2 v29.4s, v23.8h, v4.8h
    umlsl  v16.4s, v23.4h, v5.4h
    umlsl2 v17.4s, v23.8h, v5.8h
    umlsl  v28.4s, v24.4h, v5.4h
    umlsl2 v29.4s, v24.8h, v5.8h
    umlal  v16.4s, v24.4h, v6.4h
    umlal2 v17.4s, v24.8h, v6.8h
    umlal  v28.4s, v25.4h, v6.4h
    umlal2 v29.4s, v25.8h, v6.8h
    umlsl  v16.4s, v25.4h, v7.4h
    umlsl2 v17.4s, v25.8h, v7.8h
    umlsl  v28.4s, v26.4h, v7.4h
    umlsl2 v29.4s, v26.8h, v7.8h

    sqrshrun  v16.4h, v16.4s, #6
    sqrshrun2 v16.8h, v17.4s, #6
    sqrshrun  v17.4h, v28.4s, #6
    sqrshrun2 v17.8h, v29.4s, #6

    mov x0, x10                     // src += 4*i_src

    umin v16.8h, v16.8h, v8.8h
    umin v17.8h, v17.8h, v8.8h

    subs w5, w5, #4
    st1 {v16.8h}, [x2], x3
    st1 {v17.8h}, [x2], x3
    bgt if_ver_luma_w8_loop_y

    ld1 {v8.2d}, [sp], #16

    ret

//void uavs3e_if_ver_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w16_arm64
    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src

    dup v8.8h, w7
if_ver_luma_w16_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], x1    // x-3*i_src
    ld1 {v18.8h, v19.8h}, [x0], x1    // x-2*i_src
    mov x10, x0
    ld1 {v20.8h, v21.8h}, [x0], x1    // x-i_src
    ld1 {v22.8h, v23.8h}, [x0], x1    // x
    ld1 {v24.8h, v25.8h}, [x0], x1    // x+i_src
    ld1 {v26.8h, v27.8h}, [x0], x1    // x+2*i_src
    ld1 {v28.8h, v29.8h}, [x0], x1    // x+3*i_src
    ld1 {v30.8h, v31.8h}, [x0], x1    // x+4*i_src

    umull  v12.4s, v18.4h, v1.4h
    umull2 v13.4s, v18.8h, v1.8h
    umull  v14.4s, v19.4h, v1.4h
    umull2 v15.4s, v19.8h, v1.8h
    umlsl  v12.4s, v16.4h, v0.4h
    umlsl2 v13.4s, v16.8h, v0.8h
    umlsl  v14.4s, v17.4h, v0.4h
    umlsl2 v15.4s, v17.8h, v0.8h
    umlsl  v12.4s, v20.4h, v2.4h
    umlsl2 v13.4s, v20.8h, v2.8h
    umlsl  v14.4s, v21.4h, v2.4h
    umlsl2 v15.4s, v21.8h, v2.8h
    umlal  v12.4s, v22.4h, v3.4h
    umlal2 v13.4s, v22.8h, v3.8h
    umlal  v14.4s, v23.4h, v3.4h
    umlal2 v15.4s, v23.8h, v3.8h
    umlal  v12.4s, v24.4h, v4.4h
    umlal2 v13.4s, v24.8h, v4.8h
    umlal  v14.4s, v25.4h, v4.4h
    umlal2 v15.4s, v25.8h, v4.8h
    umlsl  v12.4s, v26.4h, v5.4h
    umlsl2 v13.4s, v26.8h, v5.8h
    umlsl  v14.4s, v27.4h, v5.4h
    umlsl2 v15.4s, v27.8h, v5.8h
    umlal  v12.4s, v28.4h, v6.4h
    umlal2 v13.4s, v28.8h, v6.8h
    umlal  v14.4s, v29.4h, v6.4h
    umlal2 v15.4s, v29.8h, v6.8h
    umlsl  v12.4s, v30.4h, v7.4h
    umlsl2 v13.4s, v30.8h, v7.8h
    umlsl  v14.4s, v31.4h, v7.4h
    umlsl2 v15.4s, v31.8h, v7.8h

    //(sum + 32) >> 6
    sqrshrun  v12.4h, v12.4s, #6
    sqrshrun2 v12.8h, v13.4s, #6
    sqrshrun  v13.4h, v14.4s, #6
    sqrshrun2 v13.8h, v15.4s, #6

    umin v12.8h, v12.8h, v8.8h
    umin v13.8h, v13.8h, v8.8h

    st1 {v12.8h, v13.8h}, [x2], x3

    ld1 {v16.8h, v17.8h}, [x0]

    umull  v10.4s, v20.4h, v1.4h
    umull2 v11.4s, v20.8h, v1.8h
    umull  v12.4s, v21.4h, v1.4h
    umull2 v13.4s, v21.8h, v1.8h
    umlsl  v10.4s, v18.4h, v0.4h
    umlsl2 v11.4s, v18.8h, v0.8h
    umlsl  v12.4s, v19.4h, v0.4h
    umlsl2 v13.4s, v19.8h, v0.8h
    umlsl  v10.4s, v22.4h, v2.4h
    umlsl2 v11.4s, v22.8h, v2.8h
    umlsl  v12.4s, v23.4h, v2.4h
    umlsl2 v13.4s, v23.8h, v2.8h
    umlal  v10.4s, v24.4h, v3.4h
    umlal2 v11.4s, v24.8h, v3.8h
    umlal  v12.4s, v25.4h, v3.4h
    umlal2 v13.4s, v25.8h, v3.8h
    umlal  v10.4s, v26.4h, v4.4h
    umlal2 v11.4s, v26.8h, v4.8h
    umlal  v12.4s, v27.4h, v4.4h
    umlal2 v13.4s, v27.8h, v4.8h
    umlsl  v10.4s, v28.4h, v5.4h
    umlsl2 v11.4s, v28.8h, v5.8h
    umlsl  v12.4s, v29.4h, v5.4h
    umlsl2 v13.4s, v29.8h, v5.8h
    umlal  v10.4s, v30.4h, v6.4h
    umlal2 v11.4s, v30.8h, v6.8h
    umlal  v12.4s, v31.4h, v6.4h
    umlal2 v13.4s, v31.8h, v6.8h
    umlsl  v10.4s, v16.4h, v7.4h
    umlsl2 v11.4s, v16.8h, v7.8h
    umlsl  v12.4s, v17.4h, v7.4h
    umlsl2 v13.4s, v17.8h, v7.8h

    //(sum + 32) >> 6
    sqrshrun  v10.4h, v10.4s, #6
    sqrshrun2 v10.8h, v11.4s, #6
    sqrshrun  v11.4h, v12.4s, #6
    sqrshrun2 v11.8h, v13.4s, #6

    umin v10.8h, v10.8h, v8.8h
    umin v11.8h, v11.8h, v8.8h

    mov x0, x10                     // src += i_src
    subs w5, w5, #2
    st1 {v10.8h, v11.8h}, [x2], x3
    bgt if_ver_luma_w16_loop_y

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64

    ret

//void uavs3e_if_ver_luma_w16x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_luma_w16x_arm64
    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x4, x4, #1

    // load coeffs
    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, x1, lsl #1
    sub x0, x0, x1                      // src - 3*i_src

    dup v8.8h, w7
if_ver_luma_w16x_loop_y:
    mov x9, #0
    mov x11, x2
if_ver_luma_w16x_loop_x:
    add x10, x0, x9
    ld1 {v16.8h, v17.8h}, [x10], x1    // x-3*i_src
    ld1 {v18.8h, v19.8h}, [x10], x1    // x-2*i_src
    ld1 {v20.8h, v21.8h}, [x10], x1    // x-i_src
    ld1 {v22.8h, v23.8h}, [x10], x1    // x
    ld1 {v24.8h, v25.8h}, [x10], x1    // x+i_src
    ld1 {v26.8h, v27.8h}, [x10], x1    // x+2*i_src
    ld1 {v28.8h, v29.8h}, [x10], x1    // x+3*i_src
    ld1 {v30.8h, v31.8h}, [x10]        // x+4*i_src

    umull  v12.4s, v18.4h, v1.4h
    umull2 v13.4s, v18.8h, v1.8h
    umull  v14.4s, v19.4h, v1.4h
    umull2 v15.4s, v19.8h, v1.8h
    umlsl  v12.4s, v16.4h, v0.4h
    umlsl2 v13.4s, v16.8h, v0.8h
    umlsl  v14.4s, v17.4h, v0.4h
    umlsl2 v15.4s, v17.8h, v0.8h
    umlsl  v12.4s, v20.4h, v2.4h
    umlsl2 v13.4s, v20.8h, v2.8h
    umlsl  v14.4s, v21.4h, v2.4h
    umlsl2 v15.4s, v21.8h, v2.8h
    umlal  v12.4s, v22.4h, v3.4h
    umlal2 v13.4s, v22.8h, v3.8h
    umlal  v14.4s, v23.4h, v3.4h
    umlal2 v15.4s, v23.8h, v3.8h
    umlal  v12.4s, v24.4h, v4.4h
    umlal2 v13.4s, v24.8h, v4.8h
    umlal  v14.4s, v25.4h, v4.4h
    umlal2 v15.4s, v25.8h, v4.8h
    umlsl  v12.4s, v26.4h, v5.4h
    umlsl2 v13.4s, v26.8h, v5.8h
    umlsl  v14.4s, v27.4h, v5.4h
    umlsl2 v15.4s, v27.8h, v5.8h
    umlal  v12.4s, v28.4h, v6.4h
    umlal2 v13.4s, v28.8h, v6.8h
    umlal  v14.4s, v29.4h, v6.4h
    umlal2 v15.4s, v29.8h, v6.8h
    umlsl  v12.4s, v30.4h, v7.4h
    umlsl2 v13.4s, v30.8h, v7.8h
    umlsl  v14.4s, v31.4h, v7.4h
    umlsl2 v15.4s, v31.8h, v7.8h

    //(sum + 32) >> 6
    sqrshrun  v12.4h, v12.4s, #6
    sqrshrun2 v12.8h, v13.4s, #6
    sqrshrun  v13.4h, v14.4s, #6
    sqrshrun2 v13.8h, v15.4s, #6

    umin v12.8h, v12.8h, v8.8h
    umin v13.8h, v13.8h, v8.8h

    add w9, w9, #32
    st1 {v12.8h, v13.8h}, [x11], #32

    //--------------------------------
    // loop control
    //--------------------------------
    cmp w9, w4
    blt if_ver_luma_w16x_loop_x

    subs w5, w5, #1
    add x0, x0, x1                      // src += i_src
    add x2, x2, x3                      // dst += i_dst
    bgt if_ver_luma_w16x_loop_y

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64
    ret


//void uavs3e_if_ver_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, s8 const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1

    // load coeff
    ld1 {v4.s}[0], [x6]
    abs  v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]

    sub x0, x0, x1                  // src - i_src
    dup v30.8h, w7
if_ver_chroma_w8_loop_y:
    ld1 {v16.8h}, [x0], x1         // src-i_src
    ld1 {v17.8h}, [x0], x1         // src
    ld1 {v18.8h}, [x0], x1         // src+i_src
    ld1 {v19.8h}, [x0], x1         // src+2*i_src
    mov x10, x0
    ld1 {v20.8h}, [x0], x1
    ld1 {v21.8h}, [x0], x1
    ld1 {v31.8h}, [x0]

    umull  v22.4s, v17.4h, v1.4h
    umull2 v23.4s, v17.8h, v1.8h
    umull  v24.4s, v18.4h, v1.4h
    umull2 v25.4s, v18.8h, v1.8h
    umlsl  v22.4s, v16.4h, v0.4h
    umlsl2 v23.4s, v16.8h, v0.8h
    umlsl  v24.4s, v17.4h, v0.4h
    umlsl2 v25.4s, v17.8h, v0.8h
    umlal  v22.4s, v18.4h, v2.4h
    umlal2 v23.4s, v18.8h, v2.8h
    umlal  v24.4s, v19.4h, v2.4h
    umlal2 v25.4s, v19.8h, v2.8h
    umlsl  v22.4s, v19.4h, v3.4h
    umlsl2 v23.4s, v19.8h, v3.8h
    umlsl  v24.4s, v20.4h, v3.4h
    umlsl2 v25.4s, v20.8h, v3.8h

    umull  v26.4s, v19.4h, v1.4h
    umull2 v27.4s, v19.8h, v1.8h
    umull  v28.4s, v20.4h, v1.4h
    umull2 v29.4s, v20.8h, v1.8h
    umlsl  v26.4s, v18.4h, v0.4h
    umlsl2 v27.4s, v18.8h, v0.8h
    umlsl  v28.4s, v19.4h, v0.4h
    umlsl2 v29.4s, v19.8h, v0.8h
    umlal  v26.4s, v20.4h, v2.4h
    umlal2 v27.4s, v20.8h, v2.8h
    umlal  v28.4s, v21.4h, v2.4h
    umlal2 v29.4s, v21.8h, v2.8h
    umlsl  v26.4s, v21.4h, v3.4h
    umlsl2 v27.4s, v21.8h, v3.8h
    umlsl  v28.4s, v31.4h, v3.4h
    umlsl2 v29.4s, v31.8h, v3.8h

    //(sum + 32) >> 6
    sqrshrun  v22.4h, v22.4s, #6
    sqrshrun2 v22.8h, v23.4s, #6
    sqrshrun  v23.4h, v24.4s, #6
    sqrshrun2 v23.8h, v25.4s, #6
    sqrshrun  v24.4h, v26.4s, #6
    sqrshrun2 v24.8h, v27.4s, #6
    sqrshrun  v25.4h, v28.4s, #6
    sqrshrun2 v25.8h, v29.4s, #6

    umin v22.8h, v22.8h, v30.8h
    umin v23.8h, v23.8h, v30.8h
    umin v24.8h, v24.8h, v30.8h
    umin v25.8h, v25.8h, v30.8h

    st1 {v22.8h}, [x2], x3
    subs w5, w5, #4
    mov x0, x10
    st1 {v23.8h}, [x2], x3
    st1 {v24.8h}, [x2], x3
    st1 {v25.8h}, [x2], x3

    bgt if_ver_chroma_w8_loop_y

    ret

//void uavs3e_if_ver_chroma_w16_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w16_arm64

    lsl x1, x1, #1
    lsl x3, x3, #1

    // load coeff
    ld1 {v4.s}[0], [x6]
    abs  v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]

    sub x0, x0, x1
    dup v6.8h, w7
if_ver_chroma_w16_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], x1        // src + x - i_src
    ld1 {v18.8h, v19.8h}, [x0], x1        // src + x
    mov x10, x0
    ld1 {v20.8h, v21.8h}, [x0], x1        // src + x + i_src
    ld1 {v22.8h, v23.8h}, [x0], x1        // src + x + 2*i_src
    ld1 {v4.8h, v5.8h}, [x0]

    umull  v24.4s, v18.4h, v1.4h
    umull2 v25.4s, v18.8h, v1.8h
    umull  v26.4s, v19.4h, v1.4h
    umull2 v27.4s, v19.8h, v1.8h
    umlsl  v24.4s, v16.4h, v0.4h
    umlsl2 v25.4s, v16.8h, v0.8h
    umlsl  v26.4s, v17.4h, v0.4h
    umlsl2 v27.4s, v17.8h, v0.8h
    umlal  v24.4s, v20.4h, v2.4h
    umlal2 v25.4s, v20.8h, v2.8h
    umlal  v26.4s, v21.4h, v2.4h
    umlal2 v27.4s, v21.8h, v2.8h
    umlsl  v24.4s, v22.4h, v3.4h
    umlsl2 v25.4s, v22.8h, v3.8h
    umlsl  v26.4s, v23.4h, v3.4h
    umlsl2 v27.4s, v23.8h, v3.8h

    umull  v28.4s, v20.4h, v1.4h
    umull2 v29.4s, v20.8h, v1.8h
    umull  v30.4s, v21.4h, v1.4h
    umull2 v31.4s, v21.8h, v1.8h
    umlsl  v28.4s, v18.4h, v0.4h
    umlsl2 v29.4s, v18.8h, v0.8h
    umlsl  v30.4s, v19.4h, v0.4h
    umlsl2 v31.4s, v19.8h, v0.8h
    umlal  v28.4s, v22.4h, v2.4h
    umlal2 v29.4s, v22.8h, v2.8h
    umlal  v30.4s, v23.4h, v2.4h
    umlal2 v31.4s, v23.8h, v2.8h
    umlsl  v28.4s, v4.4h, v3.4h
    umlsl2 v29.4s, v4.8h, v3.8h
    umlsl  v30.4s, v5.4h, v3.4h
    umlsl2 v31.4s, v5.8h, v3.8h

    //(sum + 32) >> 6
    sqrshrun  v24.4h, v24.4s, #6
    sqrshrun2 v24.8h, v25.4s, #6
    sqrshrun  v25.4h, v26.4s, #6
    sqrshrun2 v25.8h, v27.4s, #6
    sqrshrun  v26.4h, v28.4s, #6
    sqrshrun2 v26.8h, v29.4s, #6
    sqrshrun  v27.4h, v30.4s, #6
    sqrshrun2 v27.8h, v31.4s, #6

    umin v24.8h, v24.8h, v6.8h
    umin v25.8h, v25.8h, v6.8h
    umin v26.8h, v26.8h, v6.8h
    umin v27.8h, v27.8h, v6.8h

    subs w5, w5, #2
    mov x0, x10
    st1 {v24.8h, v25.8h}, [x2], x3
    st1 {v26.8h, v27.8h}, [x2], x3
    bgt if_ver_chroma_w16_loop_y

    ret

//void uavs3e_if_ver_chroma_w32_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w32_arm64
    sub sp, sp, #16
    st1 {v8.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // load coeff
    ld1 {v4.s}[0], [x6]
    abs  v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]

    dup v8.8h, w7
    sub x0, x0, x1
if_ver_chroma_w32_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x0], x1  // src - i_src
    mov x10, x0
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x0], x1  // src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x0], x1  // src + i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x0]      // src + 2*i_src

    umull  v4.4s, v20.4h, v1.4h
    umull2 v5.4s, v20.8h, v1.8h
    umull  v6.4s, v21.4h, v1.4h
    umull2 v7.4s, v21.8h, v1.8h
    umlsl  v4.4s, v16.4h, v0.4h
    umlsl2 v5.4s, v16.8h, v0.8h
    umlsl  v6.4s, v17.4h, v0.4h
    umlsl2 v7.4s, v17.8h, v0.8h
    umlal  v4.4s, v24.4h, v2.4h
    umlal2 v5.4s, v24.8h, v2.8h
    umlal  v6.4s, v25.4h, v2.4h
    umlal2 v7.4s, v25.8h, v2.8h
    umlsl  v4.4s, v28.4h, v3.4h
    umlsl2 v5.4s, v28.8h, v3.8h
    umlsl  v6.4s, v29.4h, v3.4h
    umlsl2 v7.4s, v29.8h, v3.8h

    umull  v16.4s, v22.4h, v1.4h
    umull2 v17.4s, v22.8h, v1.8h
    umull  v20.4s, v23.4h, v1.4h
    umull2 v21.4s, v23.8h, v1.8h
    umlsl  v16.4s, v18.4h, v0.4h
    umlsl2 v17.4s, v18.8h, v0.8h
    umlsl  v20.4s, v19.4h, v0.4h
    umlsl2 v21.4s, v19.8h, v0.8h
    umlal  v16.4s, v26.4h, v2.4h
    umlal2 v17.4s, v26.8h, v2.8h
    umlal  v20.4s, v27.4h, v2.4h
    umlal2 v21.4s, v27.8h, v2.8h
    umlsl  v16.4s, v30.4h, v3.4h
    umlsl2 v17.4s, v30.8h, v3.8h
    umlsl  v20.4s, v31.4h, v3.4h
    umlsl2 v21.4s, v31.8h, v3.8h

    //(sum + 32) >> 6
    sqrshrun  v4.4h, v4.4s, #6
    sqrshrun2 v4.8h, v5.4s, #6
    sqrshrun  v5.4h, v6.4s, #6
    sqrshrun2 v5.8h, v7.4s, #6
    sqrshrun  v6.4h, v16.4s, #6
    sqrshrun2 v6.8h, v17.4s, #6
    sqrshrun  v7.4h, v20.4s, #6
    sqrshrun2 v7.8h, v21.4s, #6

    umin v4.8h, v4.8h, v8.8h
    umin v5.8h, v5.8h, v8.8h
    umin v6.8h, v6.8h, v8.8h
    umin v7.8h, v7.8h, v8.8h

    mov x0, x10
    subs w5, w5, #1
    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x2], x3
    bgt if_ver_chroma_w32_loop_y
    ld1 {v8.2d}, [sp], #16

    ret

//void uavs3e_if_ver_chroma_w32x_arm64(const pel_t *src, int i_src, pel_t *dst, int i_dst, int width, int height, char_t const *coeff, int max_val);
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff->x6, max_val->x7
function uavs3e_if_ver_chroma_w32x_arm64
    sub sp, sp, #16
    st1 {v8.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x4, x4, #1

    // load coeff
    ld1 {v4.s}[0], [x6]
    abs  v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]

    dup v8.8h, w7
    sub x0, x0, x1
    sub x3, x3, x4
if_ver_chroma_w32x_loop_y:
    mov x11, #0             // x = 0
if_ver_chroma_w32x_loop_x:
    add x10, x0, x11
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x1  // src - i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x1  // src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x1  // src + i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10]      // src + 2*i_src

    umull  v4.4s, v20.4h, v1.4h
    umull2 v5.4s, v20.8h, v1.8h
    umull  v6.4s, v21.4h, v1.4h
    umull2 v7.4s, v21.8h, v1.8h
    umlsl  v4.4s, v16.4h, v0.4h
    umlsl2 v5.4s, v16.8h, v0.8h
    umlsl  v6.4s, v17.4h, v0.4h
    umlsl2 v7.4s, v17.8h, v0.8h
    umlal  v4.4s, v24.4h, v2.4h
    umlal2 v5.4s, v24.8h, v2.8h
    umlal  v6.4s, v25.4h, v2.4h
    umlal2 v7.4s, v25.8h, v2.8h
    umlsl  v4.4s, v28.4h, v3.4h
    umlsl2 v5.4s, v28.8h, v3.8h
    umlsl  v6.4s, v29.4h, v3.4h
    umlsl2 v7.4s, v29.8h, v3.8h

    umull  v16.4s, v22.4h, v1.4h
    umull2 v17.4s, v22.8h, v1.8h
    umull  v20.4s, v23.4h, v1.4h
    umull2 v21.4s, v23.8h, v1.8h
    umlsl  v16.4s, v18.4h, v0.4h
    umlsl2 v17.4s, v18.8h, v0.8h
    umlsl  v20.4s, v19.4h, v0.4h
    umlsl2 v21.4s, v19.8h, v0.8h
    umlal  v16.4s, v26.4h, v2.4h
    umlal2 v17.4s, v26.8h, v2.8h
    umlal  v20.4s, v27.4h, v2.4h
    umlal2 v21.4s, v27.8h, v2.8h
    umlsl  v16.4s, v30.4h, v3.4h
    umlsl2 v17.4s, v30.8h, v3.8h
    umlsl  v20.4s, v31.4h, v3.4h
    umlsl2 v21.4s, v31.8h, v3.8h

    //(sum + 32) >> 6
    sqrshrun  v4.4h, v4.4s, #6
    sqrshrun2 v4.8h, v5.4s, #6
    sqrshrun  v5.4h, v6.4s, #6
    sqrshrun2 v5.8h, v7.4s, #6
    sqrshrun  v6.4h, v16.4s, #6
    sqrshrun2 v6.8h, v17.4s, #6
    sqrshrun  v7.4h, v20.4s, #6
    sqrshrun2 v7.8h, v21.4s, #6

    umin v4.8h, v4.8h, v8.8h
    umin v5.8h, v5.8h, v8.8h
    umin v6.8h, v6.8h, v8.8h
    umin v7.8h, v7.8h, v8.8h

    add x11, x11, #64
    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x2], #64

    cmp x11, x4
    blt if_ver_chroma_w32x_loop_x

    subs w5, w5, #1
    add x0, x0, x1     // src += i_src
    add x2, x2, x3
    bgt if_ver_chroma_w32x_loop_y
    ld1 {v8.2d}, [sp], #16

    ret

//void uavs3e_if_hor_ver_luma_w4_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val->x8
function uavs3e_if_hor_ver_luma_w4_arm64

    ldr w8, [sp]                        // w8 = max_val

    lsl x1, x1, #1
    lsl x3, x3, #1

    // x17-->tmp
    ldr x15, =320                       // (32 + 8) * 4 * sizeof(short)
    sub x0, x0, x1, lsl #1              // src += -3 * i_src;
    sub x17, sp, x15
    sub x0, x0, x1
    mov sp,  x17

    ld1 {v18.d}[0], [x6]
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, #6                      // src -= 3
    sub x1, x1, #16

    cmp w8, #255
    bne if_hor_ver_luma_w4_hor_10bit

//--------------------------------
// HOR
//--------------------------------
if_hor_ver_luma_w4_hor:

    ld1 {v22.8b, v23.8b}, [x0], #16     // src[x-3]
    ld1 {v16.4h}, [x0], x1
    ext v24.8b, v22.8b, v23.8b, #2      // src[x-2]
    ext v25.8b, v22.8b, v23.8b, #4      // src[x-1]
    ext v26.8b, v22.8b, v23.8b, #6      // src[x]
    // v23 : src[x+1]
    ext v28.8b, v23.8b, v16.8b, #2
    ext v29.8b, v23.8b, v16.8b, #4
    ext v30.8b, v23.8b, v16.8b, #6

    mul v17.4h, v24.4h, v1.4h
    mls v17.4h, v22.4h, v0.4h
    mls v17.4h, v25.4h, v2.4h
    mla v17.4h, v26.4h, v3.4h
    mla v17.4h, v23.4h, v4.4h
    mls v17.4h, v28.4h, v5.4h
    mla v17.4h, v29.4h, v6.4h
    mls v17.4h, v30.4h, v7.4h

    st1 {v17.4h}, [x17], #8

    add w9, w5, #6                      // loop height+6 times
if_hor_ver_luma_w4_hor_loop_y:
    ld1 {v20.8h}, [x0], #16             // src[x-3]
    ld1 {v16.4h}, [x0], x1
    ld1 {v21.8h}, [x0], #16
    ld1 {v17.4h}, [x0], x1
    zip1 v22.8h, v20.8h, v21.8h
    zip2 v23.8h, v20.8h, v21.8h
    zip1 v20.8h, v16.8h, v17.8h

    ext v24.16b, v22.16b, v23.16b, #4   // src[x-2]
    ext v25.16b, v22.16b, v23.16b, #8   // src[x-1]
    ext v26.16b, v22.16b, v23.16b, #12  // src[x]
    // v23: src[x+1]
    ext v28.16b, v23.16b, v20.16b, #4
    ext v29.16b, v23.16b, v20.16b, #8
    ext v30.16b, v23.16b, v20.16b, #12

    mul v17.8h, v24.8h, v1.8h
    mls v17.8h, v22.8h, v0.8h
    mls v17.8h, v25.8h, v2.8h
    mla v17.8h, v26.8h, v3.8h
    mla v17.8h, v23.8h, v4.8h
    mls v17.8h, v28.8h, v5.8h
    mla v17.8h, v29.8h, v6.8h
    mls v17.8h, v30.8h, v7.8h

    uzp1 v24.8h, v17.8h, v17.8h
    uzp2 v25.8h, v17.8h, v17.8h

    subs w9, w9, #2
    st1 {v24.4h, v25.4h}, [x17], #16
    bgt if_hor_ver_luma_w4_hor_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------

    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit
    dup v31.8h, w8
if_hor_ver_luma_w4_ver_loop_y:
    mov x10, x17
    ld1 {v16.4h, v17.4h, v18.4h, v19.4h}, [x10], #32
    smull v27.4s, v16.4h, v0.h[0]
    smull v28.4s, v17.4h, v0.h[0]
    smull v29.4s, v18.4h, v0.h[0]
    smull v30.4s, v19.4h, v0.h[0]
    mov x17, x10
    ld1 {v20.4h, v21.4h, v22.4h, v23.4h}, [x10], #32
    smlal v27.4s, v17.4h, v0.h[1]
    smlal v28.4s, v18.4h, v0.h[1]
    smlal v29.4s, v19.4h, v0.h[1]
    smlal v30.4s, v20.4h, v0.h[1]

    smlal v27.4s, v18.4h, v0.h[2]
    smlal v28.4s, v19.4h, v0.h[2]
    smlal v29.4s, v20.4h, v0.h[2]
    smlal v30.4s, v21.4h, v0.h[2]

    smlal v27.4s, v19.4h, v0.h[3]
    smlal v28.4s, v20.4h, v0.h[3]
    smlal v29.4s, v21.4h, v0.h[3]
    smlal v30.4s, v22.4h, v0.h[3]

    smlal v27.4s, v20.4h, v0.h[4]
    smlal v28.4s, v21.4h, v0.h[4]
    smlal v29.4s, v22.4h, v0.h[4]
    smlal v30.4s, v23.4h, v0.h[4]

    ld1 {v24.4h, v25.4h}, [x10], #16
    smlal v27.4s, v21.4h, v0.h[5]
    smlal v28.4s, v22.4h, v0.h[5]
    smlal v29.4s, v23.4h, v0.h[5]
    smlal v30.4s, v24.4h, v0.h[5]

    smlal v27.4s, v22.4h, v0.h[6]
    smlal v28.4s, v23.4h, v0.h[6]
    smlal v29.4s, v24.4h, v0.h[6]
    smlal v30.4s, v25.4h, v0.h[6]

    ld1 {v26.d}[0], [x10]
    smlal v27.4s, v23.4h, v0.h[7]
    smlal v28.4s, v24.4h, v0.h[7]
    smlal v29.4s, v25.4h, v0.h[7]
    smlal v30.4s, v26.4h, v0.h[7]

    sqrshrun  v27.4h, v27.4s, #12
    sqrshrun2 v27.8h, v28.4s, #12
    sqrshrun  v28.4h, v29.4s, #12
    sqrshrun2 v28.8h, v30.4s, #12

    umin v27.8h, v27.8h, v31.8h
    umin v28.8h, v28.8h, v31.8h

    subs w5, w5, #4
    st1 {v27.d}[0], [x2], x3
    st1 {v27.d}[1], [x2], x3
    st1 {v28.d}[0], [x2], x3
    st1 {v28.d}[1], [x2], x3
    bgt if_hor_ver_luma_w4_ver_loop_y
    b   if_hor_ver_luma_w4_end

if_hor_ver_luma_w4_hor_10bit:
    ld1 {v22.8b, v23.8b}, [x0], #16     // src[x-3]
    ld1 {v16.4h}, [x0], x1
    ext v24.8b, v22.8b, v23.8b, #2      // src[x-2]
    ext v25.8b, v22.8b, v23.8b, #4      // src[x-1]
    ext v26.8b, v22.8b, v23.8b, #6      // src[x]
    // v23 : src[x+1]
    ext v28.8b, v23.8b, v16.8b, #2
    ext v29.8b, v23.8b, v16.8b, #4
    ext v30.8b, v23.8b, v16.8b, #6

    umull v17.4s, v24.4h, v1.4h
    umlsl v17.4s, v22.4h, v0.4h
    umlsl v17.4s, v25.4h, v2.4h
    umlal v17.4s, v26.4h, v3.4h
    umlal v17.4s, v23.4h, v4.4h
    umlsl v17.4s, v28.4h, v5.4h
    umlal v17.4s, v29.4h, v6.4h
    umlsl v17.4s, v30.4h, v7.4h

    rshrn v17.4h, v17.4s, #2
    st1 {v17.4h}, [x17], #8

    add w9, w5, #6                      // loop height+6 times
if_hor_ver_luma_w4_hor_10bit_loop_y:
    ld1 {v20.8h}, [x0], #16             // src[x-3]
    ld1 {v16.4h}, [x0], x1
    ld1 {v21.8h}, [x0], #16
    ld1 {v17.4h}, [x0], x1
    zip1 v22.8h, v20.8h, v21.8h
    zip2 v23.8h, v20.8h, v21.8h
    zip1 v20.8h, v16.8h, v17.8h

    ext v24.16b, v22.16b, v23.16b, #4   // src[x-2]
    ext v25.16b, v22.16b, v23.16b, #8   // src[x-1]
    ext v26.16b, v22.16b, v23.16b, #12  // src[x]
    // v23: src[x+1]
    ext v28.16b, v23.16b, v20.16b, #4
    ext v29.16b, v23.16b, v20.16b, #8
    ext v30.16b, v23.16b, v20.16b, #12

    umull  v17.4s, v24.4h, v1.4h
    umull2 v18.4s, v24.8h, v1.8h
    umlsl  v17.4s, v22.4h, v0.4h
    umlsl2 v18.4s, v22.8h, v0.8h
    umlsl  v17.4s, v25.4h, v2.4h
    umlsl2 v18.4s, v25.8h, v2.8h
    umlal  v17.4s, v26.4h, v3.4h
    umlal2 v18.4s, v26.8h, v3.8h
    umlal  v17.4s, v23.4h, v4.4h
    umlal2 v18.4s, v23.8h, v4.8h
    umlsl  v17.4s, v28.4h, v5.4h
    umlsl2 v18.4s, v28.8h, v5.8h
    umlal  v17.4s, v29.4h, v6.4h
    umlal2 v18.4s, v29.8h, v6.8h
    umlsl  v17.4s, v30.4h, v7.4h
    umlsl2 v18.4s, v30.8h, v7.8h

    rshrn  v17.4h, v17.4s, #2
    rshrn2 v17.8h, v18.4s, #2

    uzp1 v24.8h, v17.8h, v17.8h
    uzp2 v25.8h, v17.8h, v17.8h

    subs w9, w9, #2
    st1 {v24.4h, v25.4h}, [x17], #16
    bgt if_hor_ver_luma_w4_hor_10bit_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------

    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit
    dup v31.8h, w8
if_hor_ver_luma_w4_ver_10bit_loop_y:
    mov x10, x17
    ld1 {v16.4h, v17.4h, v18.4h, v19.4h}, [x10], #32
    smull v27.4s, v16.4h, v0.h[0]
    smull v28.4s, v17.4h, v0.h[0]
    smull v29.4s, v18.4h, v0.h[0]
    smull v30.4s, v19.4h, v0.h[0]
    mov x17, x10
    ld1 {v20.4h, v21.4h, v22.4h, v23.4h}, [x10], #32
    smlal v27.4s, v17.4h, v0.h[1]
    smlal v28.4s, v18.4h, v0.h[1]
    smlal v29.4s, v19.4h, v0.h[1]
    smlal v30.4s, v20.4h, v0.h[1]

    smlal v27.4s, v18.4h, v0.h[2]
    smlal v28.4s, v19.4h, v0.h[2]
    smlal v29.4s, v20.4h, v0.h[2]
    smlal v30.4s, v21.4h, v0.h[2]

    smlal v27.4s, v19.4h, v0.h[3]
    smlal v28.4s, v20.4h, v0.h[3]
    smlal v29.4s, v21.4h, v0.h[3]
    smlal v30.4s, v22.4h, v0.h[3]

    smlal v27.4s, v20.4h, v0.h[4]
    smlal v28.4s, v21.4h, v0.h[4]
    smlal v29.4s, v22.4h, v0.h[4]
    smlal v30.4s, v23.4h, v0.h[4]

    ld1 {v24.4h, v25.4h}, [x10], #16
    smlal v27.4s, v21.4h, v0.h[5]
    smlal v28.4s, v22.4h, v0.h[5]
    smlal v29.4s, v23.4h, v0.h[5]
    smlal v30.4s, v24.4h, v0.h[5]

    smlal v27.4s, v22.4h, v0.h[6]
    smlal v28.4s, v23.4h, v0.h[6]
    smlal v29.4s, v24.4h, v0.h[6]
    smlal v30.4s, v25.4h, v0.h[6]

    ld1 {v26.d}[0], [x10]
    smlal v27.4s, v23.4h, v0.h[7]
    smlal v28.4s, v24.4h, v0.h[7]
    smlal v29.4s, v25.4h, v0.h[7]
    smlal v30.4s, v26.4h, v0.h[7]

    sqrshrun  v27.4h, v27.4s, #10
    sqrshrun2 v27.8h, v28.4s, #10
    sqrshrun  v28.4h, v29.4s, #10
    sqrshrun2 v28.8h, v30.4s, #10

    umin v27.8h, v27.8h, v31.8h
    umin v28.8h, v28.8h, v31.8h

    subs w5, w5, #4
    st1 {v27.d}[0], [x2], x3
    st1 {v27.d}[1], [x2], x3
    st1 {v28.d}[0], [x2], x3
    st1 {v28.d}[1], [x2], x3
    bgt if_hor_ver_luma_w4_ver_10bit_loop_y

if_hor_ver_luma_w4_end:
    add sp, sp, x15                     // (32 + 8) * 4 * sizeof(short)

    ret

//void uavs3e_if_hor_ver_luma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w8_arm64

    ldr w8, [sp]                            // w8 = max_val
    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // x17-->tmp
    mov x15, #72                            // (64 + 8) * 8 * sizeof(short)
    lsl x15, x15, #4
    sub x17, sp, x15
    mov sp,  x17

    sub x0, x0, x1, lsl #1                  // src += -3 * i_src;
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    dup  v11.8h, w8
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, #6                          // src -= 3
    cmp w8, #255
    bgt if_hor_ver_luma_w8_10bit

//--------------------------------
// HOR
//--------------------------------
    ld1 {v22.8h, v23.8h}, [x0], x1
    ext v24.16b, v22.16b, v23.16b, #2
    ext v25.16b, v22.16b, v23.16b, #4
    ext v26.16b, v22.16b, v23.16b, #6
    ext v27.16b, v22.16b, v23.16b, #8
    ext v28.16b, v22.16b, v23.16b, #10
    ext v29.16b, v22.16b, v23.16b, #12
    ext v30.16b, v22.16b, v23.16b, #14

    mul v17.8h, v24.8h, v1.8h
    mls v17.8h, v22.8h, v0.8h
    mls v17.8h, v25.8h, v2.8h
    mla v17.8h, v26.8h, v3.8h
    mla v17.8h, v27.8h, v4.8h
    mls v17.8h, v28.8h, v5.8h
    mla v17.8h, v29.8h, v6.8h
    mls v17.8h, v30.8h, v7.8h

    st1 {v17.8h}, [x17], #16

    add w8, w5, #6
if_hor_ver_luma_w8_hor_loop_y:
    ld1 {v20.8h, v21.8h}, [x0], x1           // src[x-3]
    ld1 {v30.8h, v31.8h}, [x0], x1

    ext v22.16b, v20.16b, v21.16b, #2
    ext v23.16b, v20.16b, v21.16b, #4
    ext v24.16b, v20.16b, v21.16b, #6
    ext v25.16b, v20.16b, v21.16b, #8
    ext v26.16b, v20.16b, v21.16b, #10
    ext v27.16b, v20.16b, v21.16b, #12
    ext v28.16b, v20.16b, v21.16b, #14

    ext v12.16b, v30.16b, v31.16b, #2
    ext v13.16b, v30.16b, v31.16b, #4
    ext v14.16b, v30.16b, v31.16b, #6
    ext v15.16b, v30.16b, v31.16b, #8
    ext v21.16b, v30.16b, v31.16b, #10
    ext v29.16b, v30.16b, v31.16b, #12
    ext v31.16b, v30.16b, v31.16b, #14

    mul v16.8h, v22.8h, v1.8h
    mul v17.8h, v12.8h, v1.8h
    mls v16.8h, v20.8h, v0.8h
    mls v17.8h, v30.8h, v0.8h
    mls v16.8h, v23.8h, v2.8h
    mls v17.8h, v13.8h, v2.8h
    mla v16.8h, v24.8h, v3.8h
    mla v17.8h, v14.8h, v3.8h
    mla v16.8h, v25.8h, v4.8h
    mla v17.8h, v15.8h, v4.8h
    mls v16.8h, v26.8h, v5.8h
    mls v17.8h, v21.8h, v5.8h
    mla v16.8h, v27.8h, v6.8h
    mla v17.8h, v29.8h, v6.8h
    mls v16.8h, v28.8h, v7.8h
    mls v17.8h, v31.8h, v7.8h

    subs w8, w8, #2
    st1 {v16.8h, v17.8h}, [x17], #32
    bgt if_hor_ver_luma_w8_hor_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------

    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

if_hor_ver_luma_w8_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v24.8h, v25.8h}, [x17], #32
    ld1 {v31.8h}, [x17]

    smull  v26.4s, v16.4h, v0.h[0]
    smull2 v27.4s, v16.8h, v0.h[0]
    smull  v28.4s, v17.4h, v0.h[0]
    smull2 v29.4s, v17.8h, v0.h[0]

    smlal  v26.4s, v17.4h, v0.h[1]
    smlal2 v27.4s, v17.8h, v0.h[1]
    smlal  v28.4s, v18.4h, v0.h[1]
    smlal2 v29.4s, v18.8h, v0.h[1]

    smlal  v26.4s, v18.4h, v0.h[2]
    smlal2 v27.4s, v18.8h, v0.h[2]
    smlal  v28.4s, v19.4h, v0.h[2]
    smlal2 v29.4s, v19.8h, v0.h[2]

    smlal  v26.4s, v19.4h, v0.h[3]
    smlal2 v27.4s, v19.8h, v0.h[3]
    smlal  v28.4s, v20.4h, v0.h[3]
    smlal2 v29.4s, v20.8h, v0.h[3]

    smlal  v26.4s, v20.4h, v0.h[4]
    smlal2 v27.4s, v20.8h, v0.h[4]
    smlal  v28.4s, v21.4h, v0.h[4]
    smlal2 v29.4s, v21.8h, v0.h[4]

    smlal  v26.4s, v21.4h, v0.h[5]
    smlal2 v27.4s, v21.8h, v0.h[5]
    smlal  v28.4s, v22.4h, v0.h[5]
    smlal2 v29.4s, v22.8h, v0.h[5]

    smlal  v26.4s, v22.4h, v0.h[6]
    smlal2 v27.4s, v22.8h, v0.h[6]
    smlal  v28.4s, v23.4h, v0.h[6]
    smlal2 v29.4s, v23.8h, v0.h[6]
    smlal  v26.4s, v23.4h, v0.h[7]
    smlal2 v27.4s, v23.8h, v0.h[7]
    smlal  v28.4s, v24.4h, v0.h[7]
    smlal2 v29.4s, v24.8h, v0.h[7]

    smull  v4.4s, v18.4h, v0.h[0]
    smull2 v5.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v4.4s, v19.4h, v0.h[1]
    smlal2 v5.4s, v19.8h, v0.h[1]
    smlal  v6.4s, v20.4h, v0.h[1]
    smlal2 v7.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v20.4h, v0.h[2]
    smlal2 v5.4s, v20.8h, v0.h[2]
    smlal  v6.4s, v21.4h, v0.h[2]
    smlal2 v7.4s, v21.8h, v0.h[2]
    smlal  v4.4s, v21.4h, v0.h[3]
    smlal2 v5.4s, v21.8h, v0.h[3]
    smlal  v6.4s, v22.4h, v0.h[3]
    smlal2 v7.4s, v22.8h, v0.h[3]
    smlal  v4.4s, v22.4h, v0.h[4]
    smlal2 v5.4s, v22.8h, v0.h[4]
    smlal  v6.4s, v23.4h, v0.h[4]
    smlal2 v7.4s, v23.8h, v0.h[4]
    smlal  v4.4s, v23.4h, v0.h[5]
    smlal2 v5.4s, v23.8h, v0.h[5]
    smlal  v6.4s, v24.4h, v0.h[5]
    smlal2 v7.4s, v24.8h, v0.h[5]
    smlal  v4.4s, v24.4h, v0.h[6]
    smlal2 v5.4s, v24.8h, v0.h[6]
    smlal  v6.4s, v25.4h, v0.h[6]
    smlal2 v7.4s, v25.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[7]
    smlal2 v5.4s, v25.8h, v0.h[7]
    smlal  v6.4s, v31.4h, v0.h[7]
    smlal2 v7.4s, v31.8h, v0.h[7]

    sqrshrun  v26.4h, v26.4s, #12
    sqrshrun2 v26.8h, v27.4s, #12
    sqrshrun  v27.4h, v28.4s, #12
    sqrshrun2 v27.8h, v29.4s, #12
    sqrshrun  v28.4h, v4.4s, #12
    sqrshrun2 v28.8h, v5.4s, #12
    sqrshrun  v29.4h, v6.4s, #12
    sqrshrun2 v29.8h, v7.4s, #12

    umin v26.8h, v26.8h, v11.8h
    umin v27.8h, v27.8h, v11.8h
    umin v28.8h, v28.8h, v11.8h
    umin v29.8h, v29.8h, v11.8h

    subs w5, w5, #4
    mov  x17, x10
    st1 {v26.8h}, [x2], x3
    st1 {v27.8h}, [x2], x3
    st1 {v28.8h}, [x2], x3
    st1 {v29.8h}, [x2], x3
    bgt if_hor_ver_luma_w8_ver_loop_y
    b   if_hor_ver_luma_w8_end

if_hor_ver_luma_w8_10bit:
//--------------------------------
// HOR
//--------------------------------
    ld1 {v22.8h, v23.8h}, [x0], x1
    ext v24.16b, v22.16b, v23.16b, #2
    ext v25.16b, v22.16b, v23.16b, #4
    ext v26.16b, v22.16b, v23.16b, #6
    ext v27.16b, v22.16b, v23.16b, #8
    ext v28.16b, v22.16b, v23.16b, #10
    ext v29.16b, v22.16b, v23.16b, #12
    ext v30.16b, v22.16b, v23.16b, #14

    umull  v17.4s, v24.4h, v1.4h
    umull2 v18.4s, v24.8h, v1.8h
    umlsl  v17.4s, v22.4h, v0.4h
    umlsl2 v18.4s, v22.8h, v0.8h
    umlsl  v17.4s, v25.4h, v2.4h
    umlsl2 v18.4s, v25.8h, v2.8h
    umlal  v17.4s, v26.4h, v3.4h
    umlal2 v18.4s, v26.8h, v3.8h
    umlal  v17.4s, v27.4h, v4.4h
    umlal2 v18.4s, v27.8h, v4.8h
    umlsl  v17.4s, v28.4h, v5.4h
    umlsl2 v18.4s, v28.8h, v5.8h
    umlal  v17.4s, v29.4h, v6.4h
    umlal2 v18.4s, v29.8h, v6.8h
    umlsl  v17.4s, v30.4h, v7.4h
    umlsl2 v18.4s, v30.8h, v7.8h

    rshrn  v17.4h, v17.4s, #2
    rshrn2 v17.8h, v18.4s, #2

    st1 {v17.8h}, [x17], #16

    add w8, w5, #6
if_hor_ver_luma_w8_hor_10bit_loop_y:
    ld1 {v20.16b, v21.16b}, [x0], x1           // src[x-3]
    ld1 {v30.16b, v31.16b}, [x0], x1

    ext v22.16b, v20.16b, v21.16b, #2
    ext v23.16b, v20.16b, v21.16b, #4
    ext v24.16b, v20.16b, v21.16b, #6
    ext v25.16b, v20.16b, v21.16b, #8
    ext v26.16b, v20.16b, v21.16b, #10
    ext v27.16b, v20.16b, v21.16b, #12
    ext v28.16b, v20.16b, v21.16b, #14

    ext v12.16b, v30.16b, v31.16b, #2
    ext v13.16b, v30.16b, v31.16b, #4
    ext v14.16b, v30.16b, v31.16b, #6
    ext v15.16b, v30.16b, v31.16b, #8
    ext v21.16b, v30.16b, v31.16b, #10
    ext v29.16b, v30.16b, v31.16b, #12
    ext v31.16b, v30.16b, v31.16b, #14

    umull  v16.4s, v22.4h, v1.4h
    umull2 v17.4s, v22.8h, v1.8h
    umull  v18.4s, v12.4h, v1.4h
    umull2 v19.4s, v12.8h, v1.8h
    umlsl  v16.4s, v20.4h, v0.4h
    umlsl2 v17.4s, v20.8h, v0.8h
    umlsl  v18.4s, v30.4h, v0.4h
    umlsl2 v19.4s, v30.8h, v0.8h
    umlsl  v16.4s, v23.4h, v2.4h
    umlsl2 v17.4s, v23.8h, v2.8h
    umlsl  v18.4s, v13.4h, v2.4h
    umlsl2 v19.4s, v13.8h, v2.8h
    umlal  v16.4s, v24.4h, v3.4h
    umlal2 v17.4s, v24.8h, v3.8h
    umlal  v18.4s, v14.4h, v3.4h
    umlal2 v19.4s, v14.8h, v3.8h
    umlal  v16.4s, v25.4h, v4.4h
    umlal2 v17.4s, v25.8h, v4.8h
    umlal  v18.4s, v15.4h, v4.4h
    umlal2 v19.4s, v15.8h, v4.8h
    umlsl  v16.4s, v26.4h, v5.4h
    umlsl2 v17.4s, v26.8h, v5.8h
    umlsl  v18.4s, v21.4h, v5.4h
    umlsl2 v19.4s, v21.8h, v5.8h
    umlal  v16.4s, v27.4h, v6.4h
    umlal2 v17.4s, v27.8h, v6.8h
    umlal  v18.4s, v29.4h, v6.4h
    umlal2 v19.4s, v29.8h, v6.8h
    umlsl  v16.4s, v28.4h, v7.4h
    umlsl2 v17.4s, v28.8h, v7.8h
    umlsl  v18.4s, v31.4h, v7.4h
    umlsl2 v19.4s, v31.8h, v7.8h

    rshrn  v16.4h, v16.4s, #2
    rshrn2 v16.8h, v17.4s, #2
    rshrn  v17.4h, v18.4s, #2
    rshrn2 v17.8h, v19.4s, #2

    subs w8, w8, #2
    st1 {v16.8h, v17.8h}, [x17], #32
    bgt if_hor_ver_luma_w8_hor_10bit_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------

    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

if_hor_ver_luma_w8_ver_10bit_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v24.8h, v25.8h}, [x17], #32
    ld1 {v31.8h}, [x17]

    smull  v26.4s, v16.4h, v0.h[0]
    smull2 v27.4s, v16.8h, v0.h[0]
    smull  v28.4s, v17.4h, v0.h[0]
    smull2 v29.4s, v17.8h, v0.h[0]

    smlal  v26.4s, v17.4h, v0.h[1]
    smlal2 v27.4s, v17.8h, v0.h[1]
    smlal  v28.4s, v18.4h, v0.h[1]
    smlal2 v29.4s, v18.8h, v0.h[1]

    smlal  v26.4s, v18.4h, v0.h[2]
    smlal2 v27.4s, v18.8h, v0.h[2]
    smlal  v28.4s, v19.4h, v0.h[2]
    smlal2 v29.4s, v19.8h, v0.h[2]

    smlal  v26.4s, v19.4h, v0.h[3]
    smlal2 v27.4s, v19.8h, v0.h[3]
    smlal  v28.4s, v20.4h, v0.h[3]
    smlal2 v29.4s, v20.8h, v0.h[3]

    smlal  v26.4s, v20.4h, v0.h[4]
    smlal2 v27.4s, v20.8h, v0.h[4]
    smlal  v28.4s, v21.4h, v0.h[4]
    smlal2 v29.4s, v21.8h, v0.h[4]

    smlal  v26.4s, v21.4h, v0.h[5]
    smlal2 v27.4s, v21.8h, v0.h[5]
    smlal  v28.4s, v22.4h, v0.h[5]
    smlal2 v29.4s, v22.8h, v0.h[5]

    smlal  v26.4s, v22.4h, v0.h[6]
    smlal2 v27.4s, v22.8h, v0.h[6]
    smlal  v28.4s, v23.4h, v0.h[6]
    smlal2 v29.4s, v23.8h, v0.h[6]
    smlal  v26.4s, v23.4h, v0.h[7]
    smlal2 v27.4s, v23.8h, v0.h[7]
    smlal  v28.4s, v24.4h, v0.h[7]
    smlal2 v29.4s, v24.8h, v0.h[7]

    smull  v4.4s, v18.4h, v0.h[0]
    smull2 v5.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v4.4s, v19.4h, v0.h[1]
    smlal2 v5.4s, v19.8h, v0.h[1]
    smlal  v6.4s, v20.4h, v0.h[1]
    smlal2 v7.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v20.4h, v0.h[2]
    smlal2 v5.4s, v20.8h, v0.h[2]
    smlal  v6.4s, v21.4h, v0.h[2]
    smlal2 v7.4s, v21.8h, v0.h[2]
    smlal  v4.4s, v21.4h, v0.h[3]
    smlal2 v5.4s, v21.8h, v0.h[3]
    smlal  v6.4s, v22.4h, v0.h[3]
    smlal2 v7.4s, v22.8h, v0.h[3]
    smlal  v4.4s, v22.4h, v0.h[4]
    smlal2 v5.4s, v22.8h, v0.h[4]
    smlal  v6.4s, v23.4h, v0.h[4]
    smlal2 v7.4s, v23.8h, v0.h[4]
    smlal  v4.4s, v23.4h, v0.h[5]
    smlal2 v5.4s, v23.8h, v0.h[5]
    smlal  v6.4s, v24.4h, v0.h[5]
    smlal2 v7.4s, v24.8h, v0.h[5]
    smlal  v4.4s, v24.4h, v0.h[6]
    smlal2 v5.4s, v24.8h, v0.h[6]
    smlal  v6.4s, v25.4h, v0.h[6]
    smlal2 v7.4s, v25.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[7]
    smlal2 v5.4s, v25.8h, v0.h[7]
    smlal  v6.4s, v31.4h, v0.h[7]
    smlal2 v7.4s, v31.8h, v0.h[7]

    sqrshrun  v26.4h, v26.4s, #10
    sqrshrun2 v26.8h, v27.4s, #10
    sqrshrun  v27.4h, v28.4s, #10
    sqrshrun2 v27.8h, v29.4s, #10
    sqrshrun  v28.4h, v4.4s, #10
    sqrshrun2 v28.8h, v5.4s, #10
    sqrshrun  v29.4h, v6.4s, #10
    sqrshrun2 v29.8h, v7.4s, #10

    umin v26.8h, v26.8h, v11.8h
    umin v27.8h, v27.8h, v11.8h
    umin v28.8h, v28.8h, v11.8h
    umin v29.8h, v29.8h, v11.8h

    subs w5, w5, #4
    mov x17, x10
    st1 {v26.8h}, [x2], x3
    st1 {v27.8h}, [x2], x3
    st1 {v28.8h}, [x2], x3
    st1 {v29.8h}, [x2], x3
    bgt if_hor_ver_luma_w8_ver_10bit_loop_y

if_hor_ver_luma_w8_end:
    add sp, sp, x15

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64

    ret

//void uavs3e_if_hor_ver_luma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w16_arm64
    ldr w8, [sp]                            // max_val

    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // x17-->tmp
    mov x15, #136                           // (128 + 8) * 16 * sizeof(short)
    lsl x15, x15, #5
    sub x17, sp, x15
    mov sp,  x17

    sub x0, x0, x1, lsl #1                  // src += -3 * i_src;
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    dup v14.8h, w8

    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    mov x16, #32
    sub x0, x0, #6                          // src -= 3
    cmp w8, #255
    bgt if_hor_ver_luma_w16_10bit

//--------------------------------
// HOR
//--------------------------------
    sub x1, x1, #32
    
    add w8, w5, #7
if_hor_ver_luma_w16_hor_loop_y:
    ld1 {v20.8h, v21.8h}, [x0], #32           // src[x-3]
    ld1 {v31.8h}, [x0], x1

    ext v22.16b, v20.16b, v21.16b, #2
    ext v23.16b, v20.16b, v21.16b, #4
    ext v24.16b, v20.16b, v21.16b, #6
    ext v25.16b, v20.16b, v21.16b, #8
    ext v26.16b, v20.16b, v21.16b, #10
    ext v27.16b, v20.16b, v21.16b, #12
    ext v28.16b, v20.16b, v21.16b, #14

    ext v16.16b, v21.16b, v31.16b, #2
    ext v17.16b, v21.16b, v31.16b, #4
    ext v18.16b, v21.16b, v31.16b, #6
    ext v19.16b, v21.16b, v31.16b, #8
    ext v29.16b, v21.16b, v31.16b, #10
    ext v30.16b, v21.16b, v31.16b, #12
    ext v31.16b, v21.16b, v31.16b, #14

    mul v10.8h, v22.8h, v1.8h
    mul v11.8h, v16.8h, v1.8h
    mls v10.8h, v20.8h, v0.8h
    mls v11.8h, v21.8h, v0.8h
    mls v10.8h, v23.8h, v2.8h
    mls v11.8h, v17.8h, v2.8h
    mla v10.8h, v24.8h, v3.8h
    mla v11.8h, v18.8h, v3.8h
    mla v10.8h, v25.8h, v4.8h
    mla v11.8h, v19.8h, v4.8h
    mls v10.8h, v26.8h, v5.8h
    mls v11.8h, v29.8h, v5.8h
    mla v10.8h, v27.8h, v6.8h
    mla v11.8h, v30.8h, v6.8h
    mls v10.8h, v28.8h, v7.8h
    mls v11.8h, v31.8h, v7.8h

    subs w8, w8, #1
    st1 {v10.8h, v11.8h}, [x17], #32
    bgt if_hor_ver_luma_w16_hor_loop_y

//--------------------------------
// VER
//--------------------------------

    // load coeffs
    ld1 {v0.d}[0], [x7]             // load coeff
    sxtl v0.8h, v0.8b               // 8bit to 16bit

    mov x17, sp                     // tmp
if_hor_ver_luma_w16_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64

    smull  v6.4s, v16.4h, v0.h[0]
    smull2 v7.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smlal  v6.4s, v18.4h, v0.h[1]
    smlal2 v7.4s, v18.8h, v0.h[1]
    smlal  v4.4s, v19.4h, v0.h[1]
    smlal2 v5.4s, v19.8h, v0.h[1]

    smlal  v6.4s, v20.4h, v0.h[2]
    smlal2 v7.4s, v20.8h, v0.h[2]
    smlal  v4.4s, v21.4h, v0.h[2]
    smlal2 v5.4s, v21.8h, v0.h[2]
    smlal  v6.4s, v22.4h, v0.h[3]
    smlal2 v7.4s, v22.8h, v0.h[3]
    smlal  v4.4s, v23.4h, v0.h[3]
    smlal2 v5.4s, v23.8h, v0.h[3]

    smlal  v6.4s, v24.4h, v0.h[4]
    smlal2 v7.4s, v24.8h, v0.h[4]
    smlal  v4.4s, v25.4h, v0.h[4]
    smlal2 v5.4s, v25.8h, v0.h[4]
    smlal  v6.4s, v26.4h, v0.h[5]
    smlal2 v7.4s, v26.8h, v0.h[5]
    smlal  v4.4s, v27.4h, v0.h[5]
    smlal2 v5.4s, v27.8h, v0.h[5]

    ld1 {v16.8h, v17.8h}, [x17]         // x+4*i_src

    smlal  v6.4s, v28.4h, v0.h[6]
    smlal2 v7.4s, v28.8h, v0.h[6]
    smlal  v4.4s, v29.4h, v0.h[6]
    smlal2 v5.4s, v29.8h, v0.h[6]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v4.4s, v31.4h, v0.h[7]
    smlal2 v5.4s, v31.8h, v0.h[7]

    sqrshrun  v6.4h, v6.4s, #12
    sqrshrun2 v6.8h, v7.4s, #12
    sqrshrun  v7.4h, v4.4s, #12
    sqrshrun2 v7.8h, v5.4s, #12
    umin v6.8h, v6.8h, v14.8h
    umin v7.8h, v7.8h, v14.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v4.4s, v19.4h, v0.h[0]
    smull2 v5.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]

    st1 {v6.8h, v7.8h}, [x2], x3

    smlal  v2.4s, v22.4h, v0.h[2]
    smlal2 v3.4s, v22.8h, v0.h[2]
    smlal  v4.4s, v23.4h, v0.h[2]
    smlal2 v5.4s, v23.8h, v0.h[2]
    smlal  v2.4s, v24.4h, v0.h[3]
    smlal2 v3.4s, v24.8h, v0.h[3]
    smlal  v4.4s, v25.4h, v0.h[3]
    smlal2 v5.4s, v25.8h, v0.h[3]

    smlal  v2.4s, v26.4h, v0.h[4]
    smlal2 v3.4s, v26.8h, v0.h[4]
    smlal  v4.4s, v27.4h, v0.h[4]
    smlal2 v5.4s, v27.8h, v0.h[4]
    smlal  v2.4s, v28.4h, v0.h[5]
    smlal2 v3.4s, v28.8h, v0.h[5]
    smlal  v4.4s, v29.4h, v0.h[5]
    smlal2 v5.4s, v29.8h, v0.h[5]

    smlal  v2.4s, v30.4h, v0.h[6]
    smlal2 v3.4s, v30.8h, v0.h[6]
    smlal  v4.4s, v31.4h, v0.h[6]
    smlal2 v5.4s, v31.8h, v0.h[6]
    smlal  v2.4s, v16.4h, v0.h[7]
    smlal2 v3.4s, v16.8h, v0.h[7]
    smlal  v4.4s, v17.4h, v0.h[7]
    smlal2 v5.4s, v17.8h, v0.h[7]

    sqrshrun  v2.4h, v2.4s, #12
    sqrshrun2 v2.8h, v3.4s, #12
    sqrshrun  v3.4h, v4.4s, #12
    sqrshrun2 v3.8h, v5.4s, #12
    umin v2.8h, v2.8h, v14.8h
    umin v3.8h, v3.8h, v14.8h

    subs w5, w5, #2
    mov  x17, x10
    st1 {v2.8h, v3.8h}, [x2], x3
    bgt if_hor_ver_luma_w16_ver_loop_y
    b   if_hor_ver_luma_w16_end

if_hor_ver_luma_w16_10bit:
    sub x1, x1, #32
    add w8, w5, #7
if_hor_ver_luma_w16_hor_10bit_loop_y:
    ld1 {v20.8h, v21.8h}, [x0], #32           // src[x-3]
    ld1 {v31.8h}, [x0], x1

    ext v22.16b, v20.16b, v21.16b, #2
    ext v23.16b, v20.16b, v21.16b, #4
    ext v24.16b, v20.16b, v21.16b, #6
    ext v25.16b, v20.16b, v21.16b, #8
    ext v26.16b, v20.16b, v21.16b, #10
    ext v27.16b, v20.16b, v21.16b, #12
    ext v28.16b, v20.16b, v21.16b, #14

    ext v16.16b, v21.16b, v31.16b, #2
    ext v17.16b, v21.16b, v31.16b, #4
    ext v18.16b, v21.16b, v31.16b, #6
    ext v19.16b, v21.16b, v31.16b, #8
    ext v29.16b, v21.16b, v31.16b, #10
    ext v30.16b, v21.16b, v31.16b, #12
    ext v31.16b, v21.16b, v31.16b, #14

    umull v10.4s, v22.4h, v1.4h
    umull v11.4s, v16.4h, v1.4h
    umlsl v10.4s, v20.4h, v0.4h
    umlsl v11.4s, v21.4h, v0.4h
    umlsl v10.4s, v23.4h, v2.4h
    umlsl v11.4s, v17.4h, v2.4h
    umlal v10.4s, v24.4h, v3.4h
    umlal v11.4s, v18.4h, v3.4h
    umlal v10.4s, v25.4h, v4.4h
    umlal v11.4s, v19.4h, v4.4h
    umlsl v10.4s, v26.4h, v5.4h
    umlsl v11.4s, v29.4h, v5.4h
    umlal v10.4s, v27.4h, v6.4h
    umlal v11.4s, v30.4h, v6.4h
    umlsl v10.4s, v28.4h, v7.4h
    umlsl v11.4s, v31.4h, v7.4h

    umull2 v12.4s, v22.8h, v1.8h
    umull2 v13.4s, v16.8h, v1.8h
    umlsl2 v12.4s, v20.8h, v0.8h
    umlsl2 v13.4s, v21.8h, v0.8h
    umlsl2 v12.4s, v23.8h, v2.8h
    umlsl2 v13.4s, v17.8h, v2.8h
    umlal2 v12.4s, v24.8h, v3.8h
    umlal2 v13.4s, v18.8h, v3.8h
    umlal2 v12.4s, v25.8h, v4.8h
    umlal2 v13.4s, v19.8h, v4.8h
    umlsl2 v12.4s, v26.8h, v5.8h
    umlsl2 v13.4s, v29.8h, v5.8h
    umlal2 v12.4s, v27.8h, v6.8h
    umlal2 v13.4s, v30.8h, v6.8h
    umlsl2 v12.4s, v28.8h, v7.8h
    umlsl2 v13.4s, v31.8h, v7.8h

    rshrn  v10.4h, v10.4s, #2
    rshrn2 v10.8h, v12.4s, #2
    rshrn  v11.4h, v11.4s, #2
    rshrn2 v11.8h, v13.4s, #2

    subs w8, w8, #1
    st1 {v10.8h, v11.8h}, [x17], #32
    bgt if_hor_ver_luma_w16_hor_10bit_loop_y

//--------------------------------
// VER
//--------------------------------

    // load coeffs
    ld1 {v0.d}[0], [x7]             // load coeff
    sxtl v0.8h, v0.8b               // 8bit to 16bit

    mov x17, sp                     // tmp
if_hor_ver_luma_w16_ver_10bit_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64

    smull  v6.4s, v16.4h, v0.h[0]
    smull2 v7.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smlal  v6.4s, v18.4h, v0.h[1]
    smlal2 v7.4s, v18.8h, v0.h[1]
    smlal  v4.4s, v19.4h, v0.h[1]
    smlal2 v5.4s, v19.8h, v0.h[1]

    smlal  v6.4s, v20.4h, v0.h[2]
    smlal2 v7.4s, v20.8h, v0.h[2]
    smlal  v4.4s, v21.4h, v0.h[2]
    smlal2 v5.4s, v21.8h, v0.h[2]
    smlal  v6.4s, v22.4h, v0.h[3]
    smlal2 v7.4s, v22.8h, v0.h[3]
    smlal  v4.4s, v23.4h, v0.h[3]
    smlal2 v5.4s, v23.8h, v0.h[3]

    smlal  v6.4s, v24.4h, v0.h[4]
    smlal2 v7.4s, v24.8h, v0.h[4]
    smlal  v4.4s, v25.4h, v0.h[4]
    smlal2 v5.4s, v25.8h, v0.h[4]
    smlal  v6.4s, v26.4h, v0.h[5]
    smlal2 v7.4s, v26.8h, v0.h[5]
    smlal  v4.4s, v27.4h, v0.h[5]
    smlal2 v5.4s, v27.8h, v0.h[5]

    ld1 {v16.8h, v17.8h}, [x17]         // x+4*i_src

    smlal  v6.4s, v28.4h, v0.h[6]
    smlal2 v7.4s, v28.8h, v0.h[6]
    smlal  v4.4s, v29.4h, v0.h[6]
    smlal2 v5.4s, v29.8h, v0.h[6]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v4.4s, v31.4h, v0.h[7]
    smlal2 v5.4s, v31.8h, v0.h[7]

    sqrshrun  v6.4h, v6.4s, #10
    sqrshrun2 v6.8h, v7.4s, #10
    sqrshrun  v7.4h, v4.4s, #10
    sqrshrun2 v7.8h, v5.4s, #10
    umin v6.8h, v6.8h, v14.8h
    umin v7.8h, v7.8h, v14.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v4.4s, v19.4h, v0.h[0]
    smull2 v5.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]

    st1 {v6.8h, v7.8h}, [x2], x3

    smlal  v2.4s, v22.4h, v0.h[2]
    smlal2 v3.4s, v22.8h, v0.h[2]
    smlal  v4.4s, v23.4h, v0.h[2]
    smlal2 v5.4s, v23.8h, v0.h[2]
    smlal  v2.4s, v24.4h, v0.h[3]
    smlal2 v3.4s, v24.8h, v0.h[3]
    smlal  v4.4s, v25.4h, v0.h[3]
    smlal2 v5.4s, v25.8h, v0.h[3]

    smlal  v2.4s, v26.4h, v0.h[4]
    smlal2 v3.4s, v26.8h, v0.h[4]
    smlal  v4.4s, v27.4h, v0.h[4]
    smlal2 v5.4s, v27.8h, v0.h[4]
    smlal  v2.4s, v28.4h, v0.h[5]
    smlal2 v3.4s, v28.8h, v0.h[5]
    smlal  v4.4s, v29.4h, v0.h[5]
    smlal2 v5.4s, v29.8h, v0.h[5]

    smlal  v2.4s, v30.4h, v0.h[6]
    smlal2 v3.4s, v30.8h, v0.h[6]
    smlal  v4.4s, v31.4h, v0.h[6]
    smlal2 v5.4s, v31.8h, v0.h[6]
    smlal  v2.4s, v16.4h, v0.h[7]
    smlal2 v3.4s, v16.8h, v0.h[7]
    smlal  v4.4s, v17.4h, v0.h[7]
    smlal2 v5.4s, v17.8h, v0.h[7]

    sqrshrun  v2.4h, v2.4s, #10
    sqrshrun2 v2.8h, v3.4s, #10
    sqrshrun  v3.4h, v4.4s, #10
    sqrshrun2 v3.8h, v5.4s, #10
    umin v2.8h, v2.8h, v14.8h
    umin v3.8h, v3.8h, v14.8h

    subs w5, w5, #2
    mov  x17, x10
    st1 {v2.8h, v3.8h}, [x2], x3
    bgt if_hor_ver_luma_w16_ver_10bit_loop_y

if_hor_ver_luma_w16_end:
    add sp, sp, x15

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64

    ret

//void uavs3e_if_hor_ver_luma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w32_arm64
    ldr w8, [sp]

    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // x17-->tmp
    mov x15, #136                           // (128 + 8) * 128 * sizeof(short)
    lsl x15, x15, #8
    sub x17, sp, x15
    mov sp,  x17

    sub x0, x0, x1, lsl #1                  // src += -3 * i_src;
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    dup v10.8h, w8
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    sub x0, x0, #6                          // src -= 3
    cmp w8, #255
    bgt if_hor_ver_luma_w32_10bit

//--------------------------------
// HOR
//--------------------------------
    add w8, w5, #7
    sub x1, x1, #64
if_hor_ver_luma_w32_hor_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], #32
    ld1 {v30.8h, v31.8h}, [x0], #32
    ld1 {v18.8h}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #2
    ext v20.16b, v16.16b, v17.16b, #4
    ext v21.16b, v16.16b, v17.16b, #6
    ext v22.16b, v16.16b, v17.16b, #8
    ext v23.16b, v16.16b, v17.16b, #10
    ext v24.16b, v16.16b, v17.16b, #12
    ext v25.16b, v16.16b, v17.16b, #14

    mul v26.8h, v19.8h, v1.8h
    mls v26.8h, v16.8h, v0.8h
    mls v26.8h, v20.8h, v2.8h
    mla v26.8h, v21.8h, v3.8h
    mla v26.8h, v22.8h, v4.8h
    mls v26.8h, v23.8h, v5.8h
    mla v26.8h, v24.8h, v6.8h
    mls v26.8h, v25.8h, v7.8h

    ext v19.16b, v17.16b, v30.16b, #2
    ext v20.16b, v17.16b, v30.16b, #4
    ext v21.16b, v17.16b, v30.16b, #6
    ext v22.16b, v17.16b, v30.16b, #8
    ext v23.16b, v17.16b, v30.16b, #10
    ext v24.16b, v17.16b, v30.16b, #12
    ext v25.16b, v17.16b, v30.16b, #14

    mul v27.8h, v19.8h, v1.8h
    mls v27.8h, v17.8h, v0.8h
    mls v27.8h, v20.8h, v2.8h
    mla v27.8h, v21.8h, v3.8h
    mla v27.8h, v22.8h, v4.8h
    mls v27.8h, v23.8h, v5.8h
    mla v27.8h, v24.8h, v6.8h
    mls v27.8h, v25.8h, v7.8h

    ext v19.16b, v30.16b, v31.16b, #2
    ext v20.16b, v30.16b, v31.16b, #4
    ext v21.16b, v30.16b, v31.16b, #6
    ext v22.16b, v30.16b, v31.16b, #8
    ext v23.16b, v30.16b, v31.16b, #10
    ext v24.16b, v30.16b, v31.16b, #12
    ext v25.16b, v30.16b, v31.16b, #14

    mul v28.8h, v19.8h, v1.8h
    mls v28.8h, v30.8h, v0.8h
    mls v28.8h, v20.8h, v2.8h
    mla v28.8h, v21.8h, v3.8h
    mla v28.8h, v22.8h, v4.8h
    mls v28.8h, v23.8h, v5.8h
    mla v28.8h, v24.8h, v6.8h
    mls v28.8h, v25.8h, v7.8h

    ext v19.16b, v31.16b, v18.16b, #2
    ext v20.16b, v31.16b, v18.16b, #4
    ext v21.16b, v31.16b, v18.16b, #6
    ext v22.16b, v31.16b, v18.16b, #8
    ext v23.16b, v31.16b, v18.16b, #10
    ext v24.16b, v31.16b, v18.16b, #12
    ext v25.16b, v31.16b, v18.16b, #14

    mul v29.8h, v19.8h, v1.8h
    mls v29.8h, v31.8h, v0.8h
    mls v29.8h, v20.8h, v2.8h
    mla v29.8h, v21.8h, v3.8h
    mla v29.8h, v22.8h, v4.8h
    mls v29.8h, v23.8h, v5.8h
    mla v29.8h, v24.8h, v6.8h
    mls v29.8h, v25.8h, v7.8h

    subs w8, w8, #1
    st1 {v26.8h, v27.8h, v28.8h, v29.8h}, [x17], #64
    bgt if_hor_ver_luma_w32_hor_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------
    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

if_hor_ver_luma_w32_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64    // x-3*i_src
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64    // x-2*i_src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64    // x-i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64    // x
    smull  v2.4s, v16.4h, v0.h[0]
    smull2 v3.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smull  v6.4s, v18.4h, v0.h[0]
    smull2 v7.4s, v18.8h, v0.h[0]
    smull  v8.4s, v19.4h, v0.h[0]
    smull2 v9.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]
    smlal  v6.4s, v22.4h, v0.h[1]
    smlal2 v7.4s, v22.8h, v0.h[1]
    smlal  v8.4s, v23.4h, v0.h[1]
    smlal2 v9.4s, v23.8h, v0.h[1]

    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64    // x+i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64    // x+2*i_src

    smlal  v2.4s, v24.4h, v0.h[2]
    smlal2 v3.4s, v24.8h, v0.h[2]
    smlal  v4.4s, v25.4h, v0.h[2]
    smlal2 v5.4s, v25.8h, v0.h[2]
    smlal  v6.4s, v26.4h, v0.h[2]
    smlal2 v7.4s, v26.8h, v0.h[2]
    smlal  v8.4s, v27.4h, v0.h[2]
    smlal2 v9.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v28.4h, v0.h[3]
    smlal2 v3.4s, v28.8h, v0.h[3]
    smlal  v4.4s, v29.4h, v0.h[3]
    smlal2 v5.4s, v29.8h, v0.h[3]
    smlal  v6.4s, v30.4h, v0.h[3]
    smlal2 v7.4s, v30.8h, v0.h[3]
    smlal  v8.4s, v31.4h, v0.h[3]
    smlal2 v9.4s, v31.8h, v0.h[3]

    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64    // x+3*i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17]         // x+4*i_src

    smlal  v2.4s, v16.4h, v0.h[4]
    smlal2 v3.4s, v16.8h, v0.h[4]
    smlal  v4.4s, v17.4h, v0.h[4]
    smlal2 v5.4s, v17.8h, v0.h[4]
    smlal  v6.4s, v18.4h, v0.h[4]
    smlal2 v7.4s, v18.8h, v0.h[4]
    smlal  v8.4s, v19.4h, v0.h[4]
    smlal2 v9.4s, v19.8h, v0.h[4]
    smlal  v2.4s, v20.4h, v0.h[5]
    smlal2 v3.4s, v20.8h, v0.h[5]
    smlal  v4.4s, v21.4h, v0.h[5]
    smlal2 v5.4s, v21.8h, v0.h[5]
    smlal  v6.4s, v22.4h, v0.h[5]
    smlal2 v7.4s, v22.8h, v0.h[5]
    smlal  v8.4s, v23.4h, v0.h[5]
    smlal2 v9.4s, v23.8h, v0.h[5]

    smlal  v2.4s, v24.4h, v0.h[6]
    smlal2 v3.4s, v24.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[6]
    smlal2 v5.4s, v25.8h, v0.h[6]
    smlal  v6.4s, v26.4h, v0.h[6]
    smlal2 v7.4s, v26.8h, v0.h[6]
    smlal  v8.4s, v27.4h, v0.h[6]
    smlal2 v9.4s, v27.8h, v0.h[6]
    smlal  v2.4s, v28.4h, v0.h[7]
    smlal2 v3.4s, v28.8h, v0.h[7]
    smlal  v4.4s, v29.4h, v0.h[7]
    smlal2 v5.4s, v29.8h, v0.h[7]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v8.4s, v31.4h, v0.h[7]
    smlal2 v9.4s, v31.8h, v0.h[7]

    mov x17, x10
    sqrshrun  v2.4h, v2.4s, #12
    sqrshrun2 v2.8h, v3.4s, #12
    sqrshrun  v3.4h, v4.4s, #12
    sqrshrun2 v3.8h, v5.4s, #12
    sqrshrun  v4.4h, v6.4s, #12
    sqrshrun2 v4.8h, v7.4s, #12
    sqrshrun  v5.4h, v8.4s, #12
    sqrshrun2 v5.8h, v9.4s, #12

    umin v2.8h, v2.8h, v10.8h
    umin v3.8h, v3.8h, v10.8h
    umin v4.8h, v4.8h, v10.8h
    umin v5.8h, v5.8h, v10.8h

    subs w5, w5, #1
    st1 {v2.8h, v3.8h, v4.8h, v5.8h}, [x2], x3
    bgt if_hor_ver_luma_w32_ver_loop_y
    b   if_hor_ver_luma_w32_end

if_hor_ver_luma_w32_10bit:
    add w8, w5, #7
    sub x1, x1, #64
if_hor_ver_luma_w32_hor_10bit_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], #32
    ld1 {v30.8h, v31.8h}, [x0], #32
    ld1 {v18.8h}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #2
    ext v20.16b, v16.16b, v17.16b, #4
    ext v21.16b, v16.16b, v17.16b, #6
    ext v22.16b, v16.16b, v17.16b, #8
    ext v23.16b, v16.16b, v17.16b, #10
    ext v24.16b, v16.16b, v17.16b, #12
    ext v25.16b, v16.16b, v17.16b, #14

    umull  v26.4s, v19.4h, v1.4h
    umull2 v27.4s, v19.8h, v1.8h
    umlsl  v26.4s, v16.4h, v0.4h
    umlsl2 v27.4s, v16.8h, v0.8h
    umlsl  v26.4s, v20.4h, v2.4h
    umlsl2 v27.4s, v20.8h, v2.8h
    umlal  v26.4s, v21.4h, v3.4h
    umlal2 v27.4s, v21.8h, v3.8h
    umlal  v26.4s, v22.4h, v4.4h
    umlal2 v27.4s, v22.8h, v4.8h
    umlsl  v26.4s, v23.4h, v5.4h
    umlsl2 v27.4s, v23.8h, v5.8h
    umlal  v26.4s, v24.4h, v6.4h
    umlal2 v27.4s, v24.8h, v6.8h
    umlsl  v26.4s, v25.4h, v7.4h
    umlsl2 v27.4s, v25.8h, v7.8h

    ext v19.16b, v17.16b, v30.16b, #2
    ext v20.16b, v17.16b, v30.16b, #4
    ext v21.16b, v17.16b, v30.16b, #6
    ext v22.16b, v17.16b, v30.16b, #8
    ext v23.16b, v17.16b, v30.16b, #10
    ext v24.16b, v17.16b, v30.16b, #12
    ext v25.16b, v17.16b, v30.16b, #14

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umlsl  v28.4s, v17.4h, v0.4h
    umlsl2 v29.4s, v17.8h, v0.8h
    umlsl  v28.4s, v20.4h, v2.4h
    umlsl2 v29.4s, v20.8h, v2.8h
    umlal  v28.4s, v21.4h, v3.4h
    umlal2 v29.4s, v21.8h, v3.8h
    umlal  v28.4s, v22.4h, v4.4h
    umlal2 v29.4s, v22.8h, v4.8h
    umlsl  v28.4s, v23.4h, v5.4h
    umlsl2 v29.4s, v23.8h, v5.8h
    umlal  v28.4s, v24.4h, v6.4h
    umlal2 v29.4s, v24.8h, v6.8h
    umlsl  v28.4s, v25.4h, v7.4h
    umlsl2 v29.4s, v25.8h, v7.8h

    rshrn  v26.4h, v26.4s, #2
    rshrn2 v26.8h, v27.4s, #2
    rshrn  v27.4h, v28.4s, #2
    rshrn2 v27.8h, v29.4s, #2

    st1 {v26.8h, v27.8h}, [x17], #32

    ext v19.16b, v30.16b, v31.16b, #2
    ext v20.16b, v30.16b, v31.16b, #4
    ext v21.16b, v30.16b, v31.16b, #6
    ext v22.16b, v30.16b, v31.16b, #8
    ext v23.16b, v30.16b, v31.16b, #10
    ext v24.16b, v30.16b, v31.16b, #12
    ext v25.16b, v30.16b, v31.16b, #14

    umull  v26.4s, v19.4h, v1.4h
    umull2 v27.4s, v19.8h, v1.8h
    umlsl  v26.4s, v30.4h, v0.4h
    umlsl2 v27.4s, v30.8h, v0.8h
    umlsl  v26.4s, v20.4h, v2.4h
    umlsl2 v27.4s, v20.8h, v2.8h
    umlal  v26.4s, v21.4h, v3.4h
    umlal2 v27.4s, v21.8h, v3.8h
    umlal  v26.4s, v22.4h, v4.4h
    umlal2 v27.4s, v22.8h, v4.8h
    umlsl  v26.4s, v23.4h, v5.4h
    umlsl2 v27.4s, v23.8h, v5.8h
    umlal  v26.4s, v24.4h, v6.4h
    umlal2 v27.4s, v24.8h, v6.8h
    umlsl  v26.4s, v25.4h, v7.4h
    umlsl2 v27.4s, v25.8h, v7.8h

    ext v19.16b, v31.16b, v18.16b, #2
    ext v20.16b, v31.16b, v18.16b, #4
    ext v21.16b, v31.16b, v18.16b, #6
    ext v22.16b, v31.16b, v18.16b, #8
    ext v23.16b, v31.16b, v18.16b, #10
    ext v24.16b, v31.16b, v18.16b, #12
    ext v25.16b, v31.16b, v18.16b, #14

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umlsl  v28.4s, v31.4h, v0.4h
    umlsl2 v29.4s, v31.8h, v0.8h
    umlsl  v28.4s, v20.4h, v2.4h
    umlsl2 v29.4s, v20.8h, v2.8h
    umlal  v28.4s, v21.4h, v3.4h
    umlal2 v29.4s, v21.8h, v3.8h
    umlal  v28.4s, v22.4h, v4.4h
    umlal2 v29.4s, v22.8h, v4.8h
    umlsl  v28.4s, v23.4h, v5.4h
    umlsl2 v29.4s, v23.8h, v5.8h
    umlal  v28.4s, v24.4h, v6.4h
    umlal2 v29.4s, v24.8h, v6.8h
    umlsl  v28.4s, v25.4h, v7.4h
    umlsl2 v29.4s, v25.8h, v7.8h

    rshrn  v26.4h, v26.4s, #2
    rshrn2 v26.8h, v27.4s, #2
    rshrn  v27.4h, v28.4s, #2
    rshrn2 v27.8h, v29.4s, #2

    subs w8, w8, #1
    st1 {v26.8h, v27.8h}, [x17], #32
    bgt if_hor_ver_luma_w32_hor_10bit_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------
    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

if_hor_ver_luma_w32_ver_10bit_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64    // x-3*i_src
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64    // x-2*i_src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64    // x-i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64    // x
    smull  v2.4s, v16.4h, v0.h[0]
    smull2 v3.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smull  v6.4s, v18.4h, v0.h[0]
    smull2 v7.4s, v18.8h, v0.h[0]
    smull  v8.4s, v19.4h, v0.h[0]
    smull2 v9.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]
    smlal  v6.4s, v22.4h, v0.h[1]
    smlal2 v7.4s, v22.8h, v0.h[1]
    smlal  v8.4s, v23.4h, v0.h[1]
    smlal2 v9.4s, v23.8h, v0.h[1]

    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64    // x+i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64    // x+2*i_src

    smlal  v2.4s, v24.4h, v0.h[2]
    smlal2 v3.4s, v24.8h, v0.h[2]
    smlal  v4.4s, v25.4h, v0.h[2]
    smlal2 v5.4s, v25.8h, v0.h[2]
    smlal  v6.4s, v26.4h, v0.h[2]
    smlal2 v7.4s, v26.8h, v0.h[2]
    smlal  v8.4s, v27.4h, v0.h[2]
    smlal2 v9.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v28.4h, v0.h[3]
    smlal2 v3.4s, v28.8h, v0.h[3]
    smlal  v4.4s, v29.4h, v0.h[3]
    smlal2 v5.4s, v29.8h, v0.h[3]
    smlal  v6.4s, v30.4h, v0.h[3]
    smlal2 v7.4s, v30.8h, v0.h[3]
    smlal  v8.4s, v31.4h, v0.h[3]
    smlal2 v9.4s, v31.8h, v0.h[3]

    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64    // x+3*i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17]         // x+4*i_src

    smlal  v2.4s, v16.4h, v0.h[4]
    smlal2 v3.4s, v16.8h, v0.h[4]
    smlal  v4.4s, v17.4h, v0.h[4]
    smlal2 v5.4s, v17.8h, v0.h[4]
    smlal  v6.4s, v18.4h, v0.h[4]
    smlal2 v7.4s, v18.8h, v0.h[4]
    smlal  v8.4s, v19.4h, v0.h[4]
    smlal2 v9.4s, v19.8h, v0.h[4]
    smlal  v2.4s, v20.4h, v0.h[5]
    smlal2 v3.4s, v20.8h, v0.h[5]
    smlal  v4.4s, v21.4h, v0.h[5]
    smlal2 v5.4s, v21.8h, v0.h[5]
    smlal  v6.4s, v22.4h, v0.h[5]
    smlal2 v7.4s, v22.8h, v0.h[5]
    smlal  v8.4s, v23.4h, v0.h[5]
    smlal2 v9.4s, v23.8h, v0.h[5]

    smlal  v2.4s, v24.4h, v0.h[6]
    smlal2 v3.4s, v24.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[6]
    smlal2 v5.4s, v25.8h, v0.h[6]
    smlal  v6.4s, v26.4h, v0.h[6]
    smlal2 v7.4s, v26.8h, v0.h[6]
    smlal  v8.4s, v27.4h, v0.h[6]
    smlal2 v9.4s, v27.8h, v0.h[6]
    smlal  v2.4s, v28.4h, v0.h[7]
    smlal2 v3.4s, v28.8h, v0.h[7]
    smlal  v4.4s, v29.4h, v0.h[7]
    smlal2 v5.4s, v29.8h, v0.h[7]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v8.4s, v31.4h, v0.h[7]
    smlal2 v9.4s, v31.8h, v0.h[7]

    mov x17, x10
    sqrshrun  v2.4h, v2.4s, #10
    sqrshrun2 v2.8h, v3.4s, #10
    sqrshrun  v3.4h, v4.4s, #10
    sqrshrun2 v3.8h, v5.4s, #10
    sqrshrun  v4.4h, v6.4s, #10
    sqrshrun2 v4.8h, v7.4s, #10
    sqrshrun  v5.4h, v8.4s, #10
    sqrshrun2 v5.8h, v9.4s, #10

    umin v2.8h, v2.8h, v10.8h
    umin v3.8h, v3.8h, v10.8h
    umin v4.8h, v4.8h, v10.8h
    umin v5.8h, v5.8h, v10.8h

    subs w5, w5, #1
    st1 {v2.8h, v3.8h, v4.8h, v5.8h}, [x2], x3
    bgt if_hor_ver_luma_w32_ver_10bit_loop_y

if_hor_ver_luma_w32_end:
    add sp, sp, x15

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64
    ret

//void uavs3e_if_hor_ver_luma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_luma_w32x_arm64
    ldr w8, [sp]

    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // x17-->tmp
    mov x15, #136                           // (128 + 8) * 128 * sizeof(short)
    lsl x15, x15, #8
    sub x17, sp, x15
    mov sp,  x17

    sub x0, x0, x1, lsl #1                  // src += -3 * i_src;
    sub x0, x0, x1

    ld1 {v18.d}[0], [x6]
    dup  v10.8h, w8
    abs  v7.8b, v18.8b
    uxtl v7.8h, v7.8b
    dup v0.8h, v7.h[0]
    dup v1.8h, v7.h[1]
    dup v2.8h, v7.h[2]
    dup v3.8h, v7.h[3]
    dup v4.8h, v7.h[4]
    dup v5.8h, v7.h[5]
    dup v6.8h, v7.h[6]
    dup v7.8h, v7.h[7]

    lsl x16, x4, #1                         // i_tmp = width * sizeof(short)
    sub x0, x0, #6                          // src -= 3

    cmp w8, #255
    bgt if_hor_ver_luma_w32x_10bit

//--------------------------------
// HOR
//--------------------------------
    add w8, w5, #7
if_hor_ver_luma_w32x_hor_loop_y:
    mov x9, x4
    mov x10, x0
    mov x11, x17
if_hor_ver_luma_w32x_hor_loop_x:
    ld1 {v16.16b, v17.16b}, [x10], #32      // src[x-3]
    ld1 {v30.16b, v31.16b}, [x10], #32
    ld1 {v18.16b}, [x10]

    ext v19.16b, v16.16b, v17.16b, #2
    ext v20.16b, v16.16b, v17.16b, #4
    ext v21.16b, v16.16b, v17.16b, #6
    ext v22.16b, v16.16b, v17.16b, #8
    ext v23.16b, v16.16b, v17.16b, #10
    ext v24.16b, v16.16b, v17.16b, #12
    ext v25.16b, v16.16b, v17.16b, #14

    mul v26.8h, v19.8h, v1.8h
    mls v26.8h, v16.8h, v0.8h
    mls v26.8h, v20.8h, v2.8h
    mla v26.8h, v21.8h, v3.8h
    mla v26.8h, v22.8h, v4.8h
    mls v26.8h, v23.8h, v5.8h
    mla v26.8h, v24.8h, v6.8h
    mls v26.8h, v25.8h, v7.8h

    ext v19.16b, v17.16b, v30.16b, #2
    ext v20.16b, v17.16b, v30.16b, #4
    ext v21.16b, v17.16b, v30.16b, #6
    ext v22.16b, v17.16b, v30.16b, #8
    ext v23.16b, v17.16b, v30.16b, #10
    ext v24.16b, v17.16b, v30.16b, #12
    ext v25.16b, v17.16b, v30.16b, #14

    mul v27.8h, v19.8h, v1.8h
    mls v27.8h, v17.8h, v0.8h
    mls v27.8h, v20.8h, v2.8h
    mla v27.8h, v21.8h, v3.8h
    mla v27.8h, v22.8h, v4.8h
    mls v27.8h, v23.8h, v5.8h
    mla v27.8h, v24.8h, v6.8h
    mls v27.8h, v25.8h, v7.8h

    ext v19.16b, v30.16b, v31.16b, #2
    ext v20.16b, v30.16b, v31.16b, #4
    ext v21.16b, v30.16b, v31.16b, #6
    ext v22.16b, v30.16b, v31.16b, #8
    ext v23.16b, v30.16b, v31.16b, #10
    ext v24.16b, v30.16b, v31.16b, #12
    ext v25.16b, v30.16b, v31.16b, #14

    mul v28.8h, v19.8h, v1.8h
    mls v28.8h, v30.8h, v0.8h
    mls v28.8h, v20.8h, v2.8h
    mla v28.8h, v21.8h, v3.8h
    mla v28.8h, v22.8h, v4.8h
    mls v28.8h, v23.8h, v5.8h
    mla v28.8h, v24.8h, v6.8h
    mls v28.8h, v25.8h, v7.8h

    ext v19.16b, v31.16b, v18.16b, #2
    ext v20.16b, v31.16b, v18.16b, #4
    ext v21.16b, v31.16b, v18.16b, #6
    ext v22.16b, v31.16b, v18.16b, #8
    ext v23.16b, v31.16b, v18.16b, #10
    ext v24.16b, v31.16b, v18.16b, #12
    ext v25.16b, v31.16b, v18.16b, #14

    mul v29.8h, v19.8h, v1.8h
    mls v29.8h, v31.8h, v0.8h
    mls v29.8h, v20.8h, v2.8h
    mla v29.8h, v21.8h, v3.8h
    mla v29.8h, v22.8h, v4.8h
    mls v29.8h, v23.8h, v5.8h
    mla v29.8h, v24.8h, v6.8h
    mls v29.8h, v25.8h, v7.8h

    subs x9, x9, #32
    st1 {v26.8h, v27.8h, v28.8h, v29.8h}, [x11], #64
    bgt if_hor_ver_luma_w32x_hor_loop_x

    subs w8, w8, #1
    add x0, x0, x1
    add x17, x17, x16
    bgt if_hor_ver_luma_w32x_hor_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------
    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

if_hor_ver_luma_w32x_ver_loop_y:
    mov x9, #0
    mov x11, x2
if_hor_ver_luma_w32x_ver_loop_x:
    add x10, x17, x9, lsl #1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16    // x-3*i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16    // x-2*i_src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16    // x-i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10], x16    // x
    smull  v2.4s, v16.4h, v0.h[0]
    smull2 v3.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smull  v6.4s, v18.4h, v0.h[0]
    smull2 v7.4s, v18.8h, v0.h[0]
    smull  v8.4s, v19.4h, v0.h[0]
    smull2 v9.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]
    smlal  v6.4s, v22.4h, v0.h[1]
    smlal2 v7.4s, v22.8h, v0.h[1]
    smlal  v8.4s, v23.4h, v0.h[1]
    smlal2 v9.4s, v23.8h, v0.h[1]

    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16    // x+i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16    // x+2*i_src

    smlal  v2.4s, v24.4h, v0.h[2]
    smlal2 v3.4s, v24.8h, v0.h[2]
    smlal  v4.4s, v25.4h, v0.h[2]
    smlal2 v5.4s, v25.8h, v0.h[2]
    smlal  v6.4s, v26.4h, v0.h[2]
    smlal2 v7.4s, v26.8h, v0.h[2]
    smlal  v8.4s, v27.4h, v0.h[2]
    smlal2 v9.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v28.4h, v0.h[3]
    smlal2 v3.4s, v28.8h, v0.h[3]
    smlal  v4.4s, v29.4h, v0.h[3]
    smlal2 v5.4s, v29.8h, v0.h[3]
    smlal  v6.4s, v30.4h, v0.h[3]
    smlal2 v7.4s, v30.8h, v0.h[3]
    smlal  v8.4s, v31.4h, v0.h[3]
    smlal2 v9.4s, v31.8h, v0.h[3]

    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16    // x+3*i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10]         // x+4*i_src

    smlal  v2.4s, v16.4h, v0.h[4]
    smlal2 v3.4s, v16.8h, v0.h[4]
    smlal  v4.4s, v17.4h, v0.h[4]
    smlal2 v5.4s, v17.8h, v0.h[4]
    smlal  v6.4s, v18.4h, v0.h[4]
    smlal2 v7.4s, v18.8h, v0.h[4]
    smlal  v8.4s, v19.4h, v0.h[4]
    smlal2 v9.4s, v19.8h, v0.h[4]
    smlal  v2.4s, v20.4h, v0.h[5]
    smlal2 v3.4s, v20.8h, v0.h[5]
    smlal  v4.4s, v21.4h, v0.h[5]
    smlal2 v5.4s, v21.8h, v0.h[5]
    smlal  v6.4s, v22.4h, v0.h[5]
    smlal2 v7.4s, v22.8h, v0.h[5]
    smlal  v8.4s, v23.4h, v0.h[5]
    smlal2 v9.4s, v23.8h, v0.h[5]

    smlal  v2.4s, v24.4h, v0.h[6]
    smlal2 v3.4s, v24.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[6]
    smlal2 v5.4s, v25.8h, v0.h[6]
    smlal  v6.4s, v26.4h, v0.h[6]
    smlal2 v7.4s, v26.8h, v0.h[6]
    smlal  v8.4s, v27.4h, v0.h[6]
    smlal2 v9.4s, v27.8h, v0.h[6]
    smlal  v2.4s, v28.4h, v0.h[7]
    smlal2 v3.4s, v28.8h, v0.h[7]
    smlal  v4.4s, v29.4h, v0.h[7]
    smlal2 v5.4s, v29.8h, v0.h[7]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v8.4s, v31.4h, v0.h[7]
    smlal2 v9.4s, v31.8h, v0.h[7]

    sqrshrun  v2.4h, v2.4s, #12
    sqrshrun2 v2.8h, v3.4s, #12
    sqrshrun  v3.4h, v4.4s, #12
    sqrshrun2 v3.8h, v5.4s, #12
    sqrshrun  v4.4h, v6.4s, #12
    sqrshrun2 v4.8h, v7.4s, #12
    sqrshrun  v5.4h, v8.4s, #12
    sqrshrun2 v5.8h, v9.4s, #12

    umin v2.8h, v2.8h, v10.8h
    umin v3.8h, v3.8h, v10.8h
    umin v4.8h, v4.8h, v10.8h
    umin v5.8h, v5.8h, v10.8h

    add x9, x9, #32
    st1 {v2.8h, v3.8h, v4.8h, v5.8h}, [x11], #64

//--------------------------------
// loop control
//--------------------------------
    cmp x9, x4
    blt if_hor_ver_luma_w32x_ver_loop_x

    subs w5, w5, #1
    add x17, x17, x16                   // src += i_src
    add x2, x2, x3                      // dst += i_dst
    bgt if_hor_ver_luma_w32x_ver_loop_y
    b   if_hor_ver_luma_w32x_end

if_hor_ver_luma_w32x_10bit:
//--------------------------------
// HOR
//--------------------------------
    add w8, w5, #7
if_hor_ver_luma_w32x_hor_10bit_loop_y:
    mov x9, x4
    mov x10, x0
    mov x11, x17
if_hor_ver_luma_w32x_hor_10bit_loop_x:
    ld1 {v16.16b, v17.16b}, [x10], #32      // src[x-3]
    ld1 {v30.16b, v31.16b}, [x10], #32
    ld1 {v18.16b}, [x10]

    ext v19.16b, v16.16b, v17.16b, #2
    ext v20.16b, v16.16b, v17.16b, #4
    ext v21.16b, v16.16b, v17.16b, #6
    ext v22.16b, v16.16b, v17.16b, #8
    ext v23.16b, v16.16b, v17.16b, #10
    ext v24.16b, v16.16b, v17.16b, #12
    ext v25.16b, v16.16b, v17.16b, #14

    umull  v26.4s, v19.4h, v1.4h
    umull2 v27.4s, v19.8h, v1.8h
    umlsl  v26.4s, v16.4h, v0.4h
    umlsl2 v27.4s, v16.8h, v0.8h
    umlsl  v26.4s, v20.4h, v2.4h
    umlsl2 v27.4s, v20.8h, v2.8h
    umlal  v26.4s, v21.4h, v3.4h
    umlal2 v27.4s, v21.8h, v3.8h
    umlal  v26.4s, v22.4h, v4.4h
    umlal2 v27.4s, v22.8h, v4.8h
    umlsl  v26.4s, v23.4h, v5.4h
    umlsl2 v27.4s, v23.8h, v5.8h
    umlal  v26.4s, v24.4h, v6.4h
    umlal2 v27.4s, v24.8h, v6.8h
    umlsl  v26.4s, v25.4h, v7.4h
    umlsl2 v27.4s, v25.8h, v7.8h

    ext v19.16b, v17.16b, v30.16b, #2
    ext v20.16b, v17.16b, v30.16b, #4
    ext v21.16b, v17.16b, v30.16b, #6
    ext v22.16b, v17.16b, v30.16b, #8
    ext v23.16b, v17.16b, v30.16b, #10
    ext v24.16b, v17.16b, v30.16b, #12
    ext v25.16b, v17.16b, v30.16b, #14

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umlsl  v28.4s, v17.4h, v0.4h
    umlsl2 v29.4s, v17.8h, v0.8h
    umlsl  v28.4s, v20.4h, v2.4h
    umlsl2 v29.4s, v20.8h, v2.8h
    umlal  v28.4s, v21.4h, v3.4h
    umlal2 v29.4s, v21.8h, v3.8h
    umlal  v28.4s, v22.4h, v4.4h
    umlal2 v29.4s, v22.8h, v4.8h
    umlsl  v28.4s, v23.4h, v5.4h
    umlsl2 v29.4s, v23.8h, v5.8h
    umlal  v28.4s, v24.4h, v6.4h
    umlal2 v29.4s, v24.8h, v6.8h
    umlsl  v28.4s, v25.4h, v7.4h
    umlsl2 v29.4s, v25.8h, v7.8h

    rshrn  v26.4h, v26.4s, #2
    rshrn2 v26.8h, v27.4s, #2
    rshrn  v27.4h, v28.4s, #2
    rshrn2 v27.8h, v29.4s, #2

    st1 {v26.8h, v27.8h}, [x11], #32

    ext v19.16b, v30.16b, v31.16b, #2
    ext v20.16b, v30.16b, v31.16b, #4
    ext v21.16b, v30.16b, v31.16b, #6
    ext v22.16b, v30.16b, v31.16b, #8
    ext v23.16b, v30.16b, v31.16b, #10
    ext v24.16b, v30.16b, v31.16b, #12
    ext v25.16b, v30.16b, v31.16b, #14

    umull  v26.4s, v19.4h, v1.4h
    umull2 v27.4s, v19.8h, v1.8h
    umlsl  v26.4s, v30.4h, v0.4h
    umlsl2 v27.4s, v30.8h, v0.8h
    umlsl  v26.4s, v20.4h, v2.4h
    umlsl2 v27.4s, v20.8h, v2.8h
    umlal  v26.4s, v21.4h, v3.4h
    umlal2 v27.4s, v21.8h, v3.8h
    umlal  v26.4s, v22.4h, v4.4h
    umlal2 v27.4s, v22.8h, v4.8h
    umlsl  v26.4s, v23.4h, v5.4h
    umlsl2 v27.4s, v23.8h, v5.8h
    umlal  v26.4s, v24.4h, v6.4h
    umlal2 v27.4s, v24.8h, v6.8h
    umlsl  v26.4s, v25.4h, v7.4h
    umlsl2 v27.4s, v25.8h, v7.8h

    ext v19.16b, v31.16b, v18.16b, #2
    ext v20.16b, v31.16b, v18.16b, #4
    ext v21.16b, v31.16b, v18.16b, #6
    ext v22.16b, v31.16b, v18.16b, #8
    ext v23.16b, v31.16b, v18.16b, #10
    ext v24.16b, v31.16b, v18.16b, #12
    ext v25.16b, v31.16b, v18.16b, #14

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umlsl  v28.4s, v31.4h, v0.4h
    umlsl2 v29.4s, v31.8h, v0.8h
    umlsl  v28.4s, v20.4h, v2.4h
    umlsl2 v29.4s, v20.8h, v2.8h
    umlal  v28.4s, v21.4h, v3.4h
    umlal2 v29.4s, v21.8h, v3.8h
    umlal  v28.4s, v22.4h, v4.4h
    umlal2 v29.4s, v22.8h, v4.8h
    umlsl  v28.4s, v23.4h, v5.4h
    umlsl2 v29.4s, v23.8h, v5.8h
    umlal  v28.4s, v24.4h, v6.4h
    umlal2 v29.4s, v24.8h, v6.8h
    umlsl  v28.4s, v25.4h, v7.4h
    umlsl2 v29.4s, v25.8h, v7.8h

    rshrn  v26.4h, v26.4s, #2
    rshrn2 v26.8h, v27.4s, #2
    rshrn  v27.4h, v28.4s, #2
    rshrn2 v27.8h, v29.4s, #2

    subs x9, x9, #32
    st1 {v26.8h, v27.8h}, [x11], #32
    bgt if_hor_ver_luma_w32x_hor_10bit_loop_x

    subs w8, w8, #1
    add x0, x0, x1
    add x17, x17, x16
    bgt if_hor_ver_luma_w32x_hor_10bit_loop_y

    mov x17, sp

//--------------------------------
// VER
//--------------------------------
    // load coeffs
    ld1 {v0.d}[0], [x7]                 // load coeff
    sxtl v0.8h, v0.8b                   // 8bit to 16bit

if_hor_ver_luma_w32x_ver_10bit_loop_y:
    mov x9, #0
    mov x11, x2
if_hor_ver_luma_w32x_ver_10bit_loop_x:
    add x10, x17, x9, lsl #1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16    // x-3*i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16    // x-2*i_src
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16    // x-i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10], x16    // x
    smull  v2.4s, v16.4h, v0.h[0]
    smull2 v3.4s, v16.8h, v0.h[0]
    smull  v4.4s, v17.4h, v0.h[0]
    smull2 v5.4s, v17.8h, v0.h[0]
    smull  v6.4s, v18.4h, v0.h[0]
    smull2 v7.4s, v18.8h, v0.h[0]
    smull  v8.4s, v19.4h, v0.h[0]
    smull2 v9.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v20.4h, v0.h[1]
    smlal2 v3.4s, v20.8h, v0.h[1]
    smlal  v4.4s, v21.4h, v0.h[1]
    smlal2 v5.4s, v21.8h, v0.h[1]
    smlal  v6.4s, v22.4h, v0.h[1]
    smlal2 v7.4s, v22.8h, v0.h[1]
    smlal  v8.4s, v23.4h, v0.h[1]
    smlal2 v9.4s, v23.8h, v0.h[1]

    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16    // x+i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16    // x+2*i_src

    smlal  v2.4s, v24.4h, v0.h[2]
    smlal2 v3.4s, v24.8h, v0.h[2]
    smlal  v4.4s, v25.4h, v0.h[2]
    smlal2 v5.4s, v25.8h, v0.h[2]
    smlal  v6.4s, v26.4h, v0.h[2]
    smlal2 v7.4s, v26.8h, v0.h[2]
    smlal  v8.4s, v27.4h, v0.h[2]
    smlal2 v9.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v28.4h, v0.h[3]
    smlal2 v3.4s, v28.8h, v0.h[3]
    smlal  v4.4s, v29.4h, v0.h[3]
    smlal2 v5.4s, v29.8h, v0.h[3]
    smlal  v6.4s, v30.4h, v0.h[3]
    smlal2 v7.4s, v30.8h, v0.h[3]
    smlal  v8.4s, v31.4h, v0.h[3]
    smlal2 v9.4s, v31.8h, v0.h[3]

    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16    // x+3*i_src
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10]         // x+4*i_src

    smlal  v2.4s, v16.4h, v0.h[4]
    smlal2 v3.4s, v16.8h, v0.h[4]
    smlal  v4.4s, v17.4h, v0.h[4]
    smlal2 v5.4s, v17.8h, v0.h[4]
    smlal  v6.4s, v18.4h, v0.h[4]
    smlal2 v7.4s, v18.8h, v0.h[4]
    smlal  v8.4s, v19.4h, v0.h[4]
    smlal2 v9.4s, v19.8h, v0.h[4]
    smlal  v2.4s, v20.4h, v0.h[5]
    smlal2 v3.4s, v20.8h, v0.h[5]
    smlal  v4.4s, v21.4h, v0.h[5]
    smlal2 v5.4s, v21.8h, v0.h[5]
    smlal  v6.4s, v22.4h, v0.h[5]
    smlal2 v7.4s, v22.8h, v0.h[5]
    smlal  v8.4s, v23.4h, v0.h[5]
    smlal2 v9.4s, v23.8h, v0.h[5]

    smlal  v2.4s, v24.4h, v0.h[6]
    smlal2 v3.4s, v24.8h, v0.h[6]
    smlal  v4.4s, v25.4h, v0.h[6]
    smlal2 v5.4s, v25.8h, v0.h[6]
    smlal  v6.4s, v26.4h, v0.h[6]
    smlal2 v7.4s, v26.8h, v0.h[6]
    smlal  v8.4s, v27.4h, v0.h[6]
    smlal2 v9.4s, v27.8h, v0.h[6]
    smlal  v2.4s, v28.4h, v0.h[7]
    smlal2 v3.4s, v28.8h, v0.h[7]
    smlal  v4.4s, v29.4h, v0.h[7]
    smlal2 v5.4s, v29.8h, v0.h[7]
    smlal  v6.4s, v30.4h, v0.h[7]
    smlal2 v7.4s, v30.8h, v0.h[7]
    smlal  v8.4s, v31.4h, v0.h[7]
    smlal2 v9.4s, v31.8h, v0.h[7]

    sqrshrun  v2.4h, v2.4s, #10
    sqrshrun2 v2.8h, v3.4s, #10
    sqrshrun  v3.4h, v4.4s, #10
    sqrshrun2 v3.8h, v5.4s, #10
    sqrshrun  v4.4h, v6.4s, #10
    sqrshrun2 v4.8h, v7.4s, #10
    sqrshrun  v5.4h, v8.4s, #10
    sqrshrun2 v5.8h, v9.4s, #10

    umin v2.8h, v2.8h, v10.8h
    umin v3.8h, v3.8h, v10.8h
    umin v4.8h, v4.8h, v10.8h
    umin v5.8h, v5.8h, v10.8h

    add x9, x9, #32
    st1 {v2.8h, v3.8h, v4.8h, v5.8h}, [x11], #64

//--------------------------------
// loop control
//--------------------------------
    cmp x9, x4
    blt if_hor_ver_luma_w32x_ver_10bit_loop_x

    subs w5, w5, #1
    add x17, x17, x16                   // src += i_src
    add x2, x2, x3                      // dst += i_dst
    bgt if_hor_ver_luma_w32x_ver_10bit_loop_y

if_hor_ver_luma_w32x_end:
    add sp, sp, x15

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64
    ret


//void uavs3e_if_hor_ver_chroma_w8_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w8_arm64
    ldr w8, [sp]

    sub sp, sp, #16
    st1 {v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // align (x17)
    // x17-->tmp
    mov x15, #36                            // #68
    lsl x15, x15, #4
    sub x17, sp, x15                        // (32 + 4)*8*sizeof(short)
    mov sp, x17
    sub x0, x0, x1                          // src += -1 * i_src;

//--------------------------------
// HOR
//--------------------------------
    ld1 {v4.s}[0], [x6]
    dup  v15.8h, w8
    abs  v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]
    sub x0, x0, #2                          // x - 1 UV

    cmp w8, #255
    bgt if_hor_ver_chroma_w8_10bit

    //the first three rows
    ld1 {v16.8h, v17.8h}, [x0], x1          // src[x-1]
    ld1 {v18.8h, v19.8h}, [x0], x1          // src[x-1]
    ld1 {v20.8h, v21.8h}, [x0], x1

    ext v22.16b, v16.16b, v17.16b, #2       // src[x]
    ext v23.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v24.16b, v16.16b, v17.16b, #6       // src[x+2]

    ext v25.16b, v18.16b, v19.16b, #2
    ext v26.16b, v18.16b, v19.16b, #4
    ext v27.16b, v18.16b, v19.16b, #6

    ext v28.16b, v20.16b, v21.16b, #2
    ext v29.16b, v20.16b, v21.16b, #4
    ext v30.16b, v20.16b, v21.16b, #6

    mul v4.8h, v22.8h, v1.8h
    mul v5.8h, v25.8h, v1.8h
    mul v6.8h, v28.8h, v1.8h
    mls v4.8h, v16.8h, v0.8h
    mls v5.8h, v18.8h, v0.8h
    mls v6.8h, v20.8h, v0.8h
    mla v4.8h, v23.8h, v2.8h
    mla v5.8h, v26.8h, v2.8h
    mla v6.8h, v29.8h, v2.8h
    mls v4.8h, v24.8h, v3.8h
    mls v5.8h, v27.8h, v3.8h
    mls v6.8h, v30.8h, v3.8h

    st1 {v4.8h, v5.8h}, [x17], #32
    st1 {v6.8h}, [x17], #16

    // the next height rows
    mov w8, w5
if_hor_ver_chroma_w8_hor_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], x1       // src[x-1]
    ld1 {v18.8h, v19.8h}, [x0], x1       // src[x-1]
    ld1 {v20.8h, v21.8h}, [x0], x1
    ld1 {v22.8h, v23.8h}, [x0], x1

    ext v24.16b, v16.16b, v17.16b, #2       // src[x]
    ext v25.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v26.16b, v16.16b, v17.16b, #6      // src[x+2]

    ext v27.16b, v18.16b, v19.16b, #2       // src[x]
    ext v28.16b, v18.16b, v19.16b, #4       // src[x+1]
    ext v29.16b, v18.16b, v19.16b, #6      // src[x+2]

    ext v30.16b, v20.16b, v21.16b, #2       // src[x]
    ext v31.16b, v20.16b, v21.16b, #4       // src[x+1]
    ext v17.16b, v20.16b, v21.16b, #6      // src[x+2]

    ext v19.16b, v22.16b, v23.16b, #2       // src[x]
    ext v21.16b, v22.16b, v23.16b, #4       // src[x+1]
    ext v23.16b, v22.16b, v23.16b, #6      // src[x+2]

    mul v4.8h, v24.8h, v1.8h
    mul v5.8h, v27.8h, v1.8h
    mul v6.8h, v30.8h, v1.8h
    mul v7.8h, v19.8h, v1.8h
    mls v4.8h, v16.8h, v0.8h
    mls v5.8h, v18.8h, v0.8h
    mls v6.8h, v20.8h, v0.8h
    mls v7.8h, v22.8h, v0.8h
    mla v4.8h, v25.8h, v2.8h
    mla v5.8h, v28.8h, v2.8h
    mla v6.8h, v31.8h, v2.8h
    mla v7.8h, v21.8h, v2.8h
    mls v4.8h, v26.8h, v3.8h
    mls v5.8h, v29.8h, v3.8h
    mls v6.8h, v17.8h, v3.8h
    mls v7.8h, v23.8h, v3.8h

    subs w8, w8, #4
    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x17], #64
    bgt if_hor_ver_chroma_w8_hor_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w8_ver_loop_y:
    ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [x17], #64    // x-i_src
    ld1 {v20.2d, v21.2d, v22.2d, v23.2d}, [x17]
    mov x10, x17

    smull  v24.4s, v16.4h, v0.h[0]
    smull2 v25.4s, v16.8h, v0.h[0]
    smull  v26.4s, v17.4h, v0.h[0]
    smull2 v27.4s, v17.8h, v0.h[0]
    smlal  v24.4s, v17.4h, v0.h[1]
    smlal2 v25.4s, v17.8h, v0.h[1]
    smlal  v26.4s, v18.4h, v0.h[1]
    smlal2 v27.4s, v18.8h, v0.h[1]
    smlal  v24.4s, v18.4h, v0.h[2]
    smlal2 v25.4s, v18.8h, v0.h[2]
    smlal  v26.4s, v19.4h, v0.h[2]
    smlal2 v27.4s, v19.8h, v0.h[2]
    smlal  v24.4s, v19.4h, v0.h[3]
    smlal2 v25.4s, v19.8h, v0.h[3]
    smlal  v26.4s, v20.4h, v0.h[3]
    smlal2 v27.4s, v20.8h, v0.h[3]

    smull  v28.4s, v18.4h, v0.h[0]
    smull2 v29.4s, v18.8h, v0.h[0]
    smull  v30.4s, v19.4h, v0.h[0]
    smull2 v31.4s, v19.8h, v0.h[0]
    smlal  v28.4s, v19.4h, v0.h[1]
    smlal2 v29.4s, v19.8h, v0.h[1]
    smlal  v30.4s, v20.4h, v0.h[1]
    smlal2 v31.4s, v20.8h, v0.h[1]
    smlal  v28.4s, v20.4h, v0.h[2]
    smlal2 v29.4s, v20.8h, v0.h[2]
    smlal  v30.4s, v21.4h, v0.h[2]
    smlal2 v31.4s, v21.8h, v0.h[2]
    smlal  v28.4s, v21.4h, v0.h[3]
    smlal2 v29.4s, v21.8h, v0.h[3]
    smlal  v30.4s, v22.4h, v0.h[3]
    smlal2 v31.4s, v22.8h, v0.h[3]

    sqrshrun  v24.4h, v24.4s, #12
    sqrshrun2 v24.8h, v25.4s, #12
    sqrshrun  v25.4h, v26.4s, #12
    sqrshrun2 v25.8h, v27.4s, #12
    sqrshrun  v26.4h, v28.4s, #12
    sqrshrun2 v26.8h, v29.4s, #12
    sqrshrun  v27.4h, v30.4s, #12
    sqrshrun2 v27.8h, v31.4s, #12

    umin v24.8h, v24.8h, v15.8h
    umin v25.8h, v25.8h, v15.8h
    umin v26.8h, v26.8h, v15.8h
    umin v27.8h, v27.8h, v15.8h

    subs w5, w5, #4
    mov x17, x10
    st1 {v24.8h}, [x2], x3
    st1 {v25.8h}, [x2], x3
    st1 {v26.8h}, [x2], x3
    st1 {v27.8h}, [x2], x3
    bgt if_hor_ver_chroma_w8_ver_loop_y
    b   if_hor_ver_chroma_w8_end

if_hor_ver_chroma_w8_10bit:

    //the first three rows
    ld1 {v16.8h, v17.8h}, [x0], x1       // src[x-1]
    ld1 {v18.8h, v19.8h}, [x0], x1       // src[x-1]
    ld1 {v20.8h, v21.8h}, [x0], x1

    ext v22.16b, v16.16b, v17.16b, #2      // src[x]
    ext v23.16b, v16.16b, v17.16b, #4      // src[x+1]
    ext v24.16b, v16.16b, v17.16b, #6      // src[x+2]

    ext v25.16b, v18.16b, v19.16b, #2
    ext v26.16b, v18.16b, v19.16b, #4
    ext v27.16b, v18.16b, v19.16b, #6

    ext v28.16b, v20.16b, v21.16b, #2
    ext v29.16b, v20.16b, v21.16b, #4
    ext v30.16b, v20.16b, v21.16b, #6

    umull  v4.4s, v22.4h, v1.4h
    umull2 v5.4s, v22.8h, v1.8h
    umlsl  v4.4s, v16.4h, v0.4h
    umlsl2 v5.4s, v16.8h, v0.8h
    umlal  v4.4s, v23.4h, v2.4h
    umlal2 v5.4s, v23.8h, v2.8h
    umlsl  v4.4s, v24.4h, v3.4h
    umlsl2 v5.4s, v24.8h, v3.8h

    umull  v6.4s, v25.4h, v1.4h
    umull2 v7.4s, v25.8h, v1.8h
    umlsl  v6.4s, v18.4h, v0.4h
    umlsl2 v7.4s, v18.8h, v0.8h
    umlal  v6.4s, v26.4h, v2.4h
    umlal2 v7.4s, v26.8h, v2.8h
    umlsl  v6.4s, v27.4h, v3.4h
    umlsl2 v7.4s, v27.8h, v3.8h

    umull  v16.4s, v28.4h, v1.4h
    umull2 v17.4s, v28.8h, v1.8h
    umlsl  v16.4s, v20.4h, v0.4h
    umlsl2 v17.4s, v20.8h, v0.8h
    umlal  v16.4s, v29.4h, v2.4h
    umlal2 v17.4s, v29.8h, v2.8h
    umlsl  v16.4s, v30.4h, v3.4h
    umlsl2 v17.4s, v30.8h, v3.8h

    rshrn  v4.4h, v4.4s, #2
    rshrn2 v4.8h, v5.4s, #2
    rshrn  v5.4h, v6.4s, #2
    rshrn2 v5.8h, v7.4s, #2
    rshrn  v6.4h, v16.4s, #2
    rshrn2 v6.8h, v17.4s, #2

    st1 {v4.8h, v5.8h}, [x17], #32
    st1 {v6.8h}, [x17], #16

    // the next height rows
    mov w8, w5
if_hor_ver_chroma_w8_hor_10bit_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], x1          // src[x-1]
    ld1 {v18.8h, v19.8h}, [x0], x1          // src[x-1]
    ld1 {v20.8h, v21.8h}, [x0], x1
    ld1 {v22.8h, v23.8h}, [x0], x1

    ext v24.16b, v16.16b, v17.16b, #2       // src[x]
    ext v25.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v26.16b, v16.16b, v17.16b, #6       // src[x+2]

    ext v27.16b, v18.16b, v19.16b, #2       // src[x]
    ext v28.16b, v18.16b, v19.16b, #4       // src[x+1]
    ext v29.16b, v18.16b, v19.16b, #6      // src[x+2]

    ext v30.16b, v20.16b, v21.16b, #2       // src[x]
    ext v31.16b, v20.16b, v21.16b, #4       // src[x+1]
    ext v17.16b, v20.16b, v21.16b, #6       // src[x+2]

    ext v19.16b, v22.16b, v23.16b, #2       // src[x]
    ext v21.16b, v22.16b, v23.16b, #4       // src[x+1]
    ext v23.16b, v22.16b, v23.16b, #6       // src[x+2]

    umull  v4.4s, v24.4h, v1.4h
    umull  v5.4s, v27.4h, v1.4h
    umull2 v6.4s, v24.8h, v1.8h
    umull2 v7.4s, v27.8h, v1.8h
    umlsl  v4.4s, v16.4h, v0.4h
    umlsl  v5.4s, v18.4h, v0.4h
    umlsl2 v6.4s, v16.8h, v0.8h
    umlsl2 v7.4s, v18.8h, v0.8h
    umlal  v4.4s, v25.4h, v2.4h
    umlal  v5.4s, v28.4h, v2.4h
    umlal2 v6.4s, v25.8h, v2.8h
    umlal2 v7.4s, v28.8h, v2.8h
    umlsl  v4.4s, v26.4h, v3.4h
    umlsl  v5.4s, v29.4h, v3.4h
    umlsl2 v6.4s, v26.8h, v3.8h
    umlsl2 v7.4s, v29.8h, v3.8h

    rshrn  v4.4h, v4.4s, #2
    rshrn2 v4.8h, v6.4s, #2
    rshrn  v5.4h, v5.4s, #2
    rshrn2 v5.8h, v7.4s, #2

    st1 {v4.8h, v5.8h}, [x17], #32

    umull  v4.4s, v30.4h, v1.4h
    umull  v5.4s, v19.4h, v1.4h
    umull2 v6.4s, v30.8h, v1.8h
    umull2 v7.4s, v19.8h, v1.8h
    umlsl  v4.4s, v20.4h, v0.4h
    umlsl  v5.4s, v22.4h, v0.4h
    umlsl2 v6.4s, v20.8h, v0.8h
    umlsl2 v7.4s, v22.8h, v0.8h
    umlal  v4.4s, v31.4h, v2.4h
    umlal  v5.4s, v21.4h, v2.4h
    umlal2 v6.4s, v31.8h, v2.8h
    umlal2 v7.4s, v21.8h, v2.8h
    umlsl  v4.4s, v17.4h, v3.4h
    umlsl  v5.4s, v23.4h, v3.4h
    umlsl2 v6.4s, v17.8h, v3.8h
    umlsl2 v7.4s, v23.8h, v3.8h

    rshrn  v4.4h, v4.4s, #2
    rshrn2 v4.8h, v6.4s, #2
    rshrn  v5.4h, v5.4s, #2
    rshrn2 v5.8h, v7.4s, #2

    subs w8, w8, #4

    st1 {v4.8h, v5.8h}, [x17], #32
    bgt if_hor_ver_chroma_w8_hor_10bit_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w8_ver_10bit_loop_y:
    ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [x17], #64    // x-i_src
    ld1 {v20.2d, v21.2d, v22.2d, v23.2d}, [x17]
    mov x10, x17

    smull  v24.4s, v16.4h, v0.h[0]
    smull2 v25.4s, v16.8h, v0.h[0]
    smull  v26.4s, v17.4h, v0.h[0]
    smull2 v27.4s, v17.8h, v0.h[0]
    smlal  v24.4s, v17.4h, v0.h[1]
    smlal2 v25.4s, v17.8h, v0.h[1]
    smlal  v26.4s, v18.4h, v0.h[1]
    smlal2 v27.4s, v18.8h, v0.h[1]
    smlal  v24.4s, v18.4h, v0.h[2]
    smlal2 v25.4s, v18.8h, v0.h[2]
    smlal  v26.4s, v19.4h, v0.h[2]
    smlal2 v27.4s, v19.8h, v0.h[2]
    smlal  v24.4s, v19.4h, v0.h[3]
    smlal2 v25.4s, v19.8h, v0.h[3]
    smlal  v26.4s, v20.4h, v0.h[3]
    smlal2 v27.4s, v20.8h, v0.h[3]

    smull  v28.4s, v18.4h, v0.h[0]
    smull2 v29.4s, v18.8h, v0.h[0]
    smull  v30.4s, v19.4h, v0.h[0]
    smull2 v31.4s, v19.8h, v0.h[0]
    smlal  v28.4s, v19.4h, v0.h[1]
    smlal2 v29.4s, v19.8h, v0.h[1]
    smlal  v30.4s, v20.4h, v0.h[1]
    smlal2 v31.4s, v20.8h, v0.h[1]
    smlal  v28.4s, v20.4h, v0.h[2]
    smlal2 v29.4s, v20.8h, v0.h[2]
    smlal  v30.4s, v21.4h, v0.h[2]
    smlal2 v31.4s, v21.8h, v0.h[2]
    smlal  v28.4s, v21.4h, v0.h[3]
    smlal2 v29.4s, v21.8h, v0.h[3]
    smlal  v30.4s, v22.4h, v0.h[3]
    smlal2 v31.4s, v22.8h, v0.h[3]

    sqrshrun  v24.4h, v24.4s, #10
    sqrshrun2 v24.8h, v25.4s, #10
    sqrshrun  v25.4h, v26.4s, #10
    sqrshrun2 v25.8h, v27.4s, #10
    sqrshrun  v26.4h, v28.4s, #10
    sqrshrun2 v26.8h, v29.4s, #10
    sqrshrun  v27.4h, v30.4s, #10
    sqrshrun2 v27.8h, v31.4s, #10

    umin v24.8h, v24.8h, v15.8h
    umin v25.8h, v25.8h, v15.8h
    umin v26.8h, v26.8h, v15.8h
    umin v27.8h, v27.8h, v15.8h

    subs w5, w5, #4
    mov x17, x10
    st1 {v24.8h}, [x2], x3
    st1 {v25.8h}, [x2], x3
    st1 {v26.8h}, [x2], x3
    st1 {v27.8h}, [x2], x3
    bgt if_hor_ver_chroma_w8_ver_10bit_loop_y
if_hor_ver_chroma_w8_end:
    add sp, sp, x15                     // (32 + 4)*8*sizeof(short)
    ld1 {v15.2d}, [sp], #16

    ret

//void uavs3e_if_hor_ver_chroma_w16_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8n 0- *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w16_arm64
    ldr w8, [sp]

    sub sp, sp, #16
    st1 {v15.2d}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1

    // align (x17)
    // x17-->tmp
    mov x15, #68                            // #68
    lsl x15, x15, #5
    sub x17, sp, x15                        // (64 + 4)*16*sizeof(short)
    mov sp, x17

    sub x0, x0, x1                          // src += -1 * i_src;
//--------------------------------
// HOR
//--------------------------------
    ld1 {v4.s}[0], [x6]
    dup v15.8h, w8
    abs v3.8b, v4.8b
    uxtl v3.8h, v3.8b

    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]
    sub x0, x0, #2                          // x - 1 UV
    sub x1, x1, #32

    cmp w8, #255
    bgt if_hor_ver_chroma_w16_10bit

    add w8, w5, #3
if_hor_ver_chroma_w16_hor_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], #32         // src[x-1]
    ld1 {v18.8h}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #2       // src[x]
    ext v20.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6       // src[x+2]
    ext v22.16b, v17.16b, v18.16b, #2
    ext v23.16b, v17.16b, v18.16b, #4
    ext v24.16b, v17.16b, v18.16b, #6

    mul v28.8h, v19.8h, v1.8h
    mul v29.8h, v22.8h, v1.8h
    mls v28.8h, v16.8h, v0.8h
    mls v29.8h, v17.8h, v0.8h
    mla v28.8h, v20.8h, v2.8h
    mla v29.8h, v23.8h, v2.8h
    mls v28.8h, v21.8h, v3.8h
    mls v29.8h, v24.8h, v3.8h

    subs w8, w8, #1
    st1 {v28.8h, v29.8h}, [x17], #32
    bgt if_hor_ver_chroma_w16_hor_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w16_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v2.8h, v3.8h}, [x17]

    smull  v24.4s, v16.4h, v0.h[0]
    smull2 v25.4s, v16.8h, v0.h[0]
    smull  v26.4s, v17.4h, v0.h[0]
    smull2 v27.4s, v17.8h, v0.h[0]
    smlal  v24.4s, v18.4h, v0.h[1]
    smlal2 v25.4s, v18.8h, v0.h[1]
    smlal  v26.4s, v19.4h, v0.h[1]
    smlal2 v27.4s, v19.8h, v0.h[1]
    smlal  v24.4s, v20.4h, v0.h[2]
    smlal2 v25.4s, v20.8h, v0.h[2]
    smlal  v26.4s, v21.4h, v0.h[2]
    smlal2 v27.4s, v21.8h, v0.h[2]
    smlal  v24.4s, v22.4h, v0.h[3]
    smlal2 v25.4s, v22.8h, v0.h[3]
    smlal  v26.4s, v23.4h, v0.h[3]
    smlal2 v27.4s, v23.8h, v0.h[3]

    smull  v28.4s, v18.4h, v0.h[0]
    smull2 v29.4s, v18.8h, v0.h[0]
    smull  v30.4s, v19.4h, v0.h[0]
    smull2 v31.4s, v19.8h, v0.h[0]
    smlal  v28.4s, v20.4h, v0.h[1]
    smlal2 v29.4s, v20.8h, v0.h[1]
    smlal  v30.4s, v21.4h, v0.h[1]
    smlal2 v31.4s, v21.8h, v0.h[1]
    smlal  v28.4s, v22.4h, v0.h[2]
    smlal2 v29.4s, v22.8h, v0.h[2]
    smlal  v30.4s, v23.4h, v0.h[2]
    smlal2 v31.4s, v23.8h, v0.h[2]
    smlal  v28.4s, v2.4h, v0.h[3]
    smlal2 v29.4s, v2.8h, v0.h[3]
    smlal  v30.4s, v3.4h, v0.h[3]
    smlal2 v31.4s, v3.8h, v0.h[3]

    sqrshrun  v24.4h, v24.4s, #12
    sqrshrun2 v24.8h, v25.4s, #12
    sqrshrun  v25.4h, v26.4s, #12
    sqrshrun2 v25.8h, v27.4s, #12
    sqrshrun  v26.4h, v28.4s, #12
    sqrshrun2 v26.8h, v29.4s, #12
    sqrshrun  v27.4h, v30.4s, #12
    sqrshrun2 v27.8h, v31.4s, #12

    umin v24.8h, v24.8h, v15.8h
    umin v25.8h, v25.8h, v15.8h
    umin v26.8h, v26.8h, v15.8h
    umin v27.8h, v27.8h, v15.8h

    subs w5, w5, #2
    mov x17, x10                            // tmp += 64;
    st1 {v24.8h, v25.8h}, [x2], x3
    st1 {v26.8h, v27.8h}, [x2], x3
    bgt if_hor_ver_chroma_w16_ver_loop_y
    b   if_hor_ver_chroma_w16_end

if_hor_ver_chroma_w16_10bit:
    add w8, w5, #3
if_hor_ver_chroma_w16_hor_10bit_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], #32         // src[x-1]
    ld1 {v18.8h}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #2       // src[x]
    ext v20.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6       // src[x+2]
    ext v22.16b, v17.16b, v18.16b, #2
    ext v23.16b, v17.16b, v18.16b, #4
    ext v24.16b, v17.16b, v18.16b, #6

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umlsl  v28.4s, v16.4h, v0.4h
    umlsl2 v29.4s, v16.8h, v0.8h
    umlal  v28.4s, v20.4h, v2.4h
    umlal2 v29.4s, v20.8h, v2.8h
    umlsl  v28.4s, v21.4h, v3.4h
    umlsl2 v29.4s, v21.8h, v3.8h

    umull  v30.4s, v22.4h, v1.4h
    umull2 v31.4s, v22.8h, v1.8h
    umlsl  v30.4s, v17.4h, v0.4h
    umlsl2 v31.4s, v17.8h, v0.8h
    umlal  v30.4s, v23.4h, v2.4h
    umlal2 v31.4s, v23.8h, v2.8h
    umlsl  v30.4s, v24.4h, v3.4h
    umlsl2 v31.4s, v24.8h, v3.8h

    rshrn  v28.4h, v28.4s, #2
    rshrn2 v28.8h, v29.4s, #2
    rshrn  v29.4h, v30.4s, #2
    rshrn2 v29.8h, v31.4s, #2

    subs w8, w8, #1
    st1 {v28.8h, v29.8h}, [x17], #32
    bgt if_hor_ver_chroma_w16_hor_10bit_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w16_ver_10bit_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64
    mov x10, x17
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    ld1 {v2.8h, v3.8h}, [x17]

    smull  v24.4s, v16.4h, v0.h[0]
    smull2 v25.4s, v16.8h, v0.h[0]
    smull  v26.4s, v17.4h, v0.h[0]
    smull2 v27.4s, v17.8h, v0.h[0]
    smlal  v24.4s, v18.4h, v0.h[1]
    smlal2 v25.4s, v18.8h, v0.h[1]
    smlal  v26.4s, v19.4h, v0.h[1]
    smlal2 v27.4s, v19.8h, v0.h[1]
    smlal  v24.4s, v20.4h, v0.h[2]
    smlal2 v25.4s, v20.8h, v0.h[2]
    smlal  v26.4s, v21.4h, v0.h[2]
    smlal2 v27.4s, v21.8h, v0.h[2]
    smlal  v24.4s, v22.4h, v0.h[3]
    smlal2 v25.4s, v22.8h, v0.h[3]
    smlal  v26.4s, v23.4h, v0.h[3]
    smlal2 v27.4s, v23.8h, v0.h[3]

    smull  v28.4s, v18.4h, v0.h[0]
    smull2 v29.4s, v18.8h, v0.h[0]
    smull  v30.4s, v19.4h, v0.h[0]
    smull2 v31.4s, v19.8h, v0.h[0]
    smlal  v28.4s, v20.4h, v0.h[1]
    smlal2 v29.4s, v20.8h, v0.h[1]
    smlal  v30.4s, v21.4h, v0.h[1]
    smlal2 v31.4s, v21.8h, v0.h[1]
    smlal  v28.4s, v22.4h, v0.h[2]
    smlal2 v29.4s, v22.8h, v0.h[2]
    smlal  v30.4s, v23.4h, v0.h[2]
    smlal2 v31.4s, v23.8h, v0.h[2]
    smlal  v28.4s, v2.4h, v0.h[3]
    smlal2 v29.4s, v2.8h, v0.h[3]
    smlal  v30.4s, v3.4h, v0.h[3]
    smlal2 v31.4s, v3.8h, v0.h[3]

    sqrshrun  v24.4h, v24.4s, #10
    sqrshrun2 v24.8h, v25.4s, #10
    sqrshrun  v25.4h, v26.4s, #10
    sqrshrun2 v25.8h, v27.4s, #10
    sqrshrun  v26.4h, v28.4s, #10
    sqrshrun2 v26.8h, v29.4s, #10
    sqrshrun  v27.4h, v30.4s, #10
    sqrshrun2 v27.8h, v31.4s, #10

    umin v24.8h, v24.8h, v15.8h
    umin v25.8h, v25.8h, v15.8h
    umin v26.8h, v26.8h, v15.8h
    umin v27.8h, v27.8h, v15.8h

    subs w5, w5, #2
    mov x17, x10                            // tmp += 64;
    st1 {v24.8h, v25.8h}, [x2], x3
    st1 {v26.8h, v27.8h}, [x2], x3
    bgt if_hor_ver_chroma_w16_ver_10bit_loop_y

if_hor_ver_chroma_w16_end:
    add sp, sp, x15                     // (64 + 4)*16*sizeof(short)
    ld1 {v15.2d}, [sp], #16

    ret

//void uavs3e_if_hor_ver_chroma_w32_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8 *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w32_arm64
    ldr w8, [sp]
    lsl x1, x1, #1
    lsl x3, x3, #1

    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    // align (x17)
    // x17-->tmp
    mov x15, #68
    lsl x15, x15, #6
    sub x17, sp, x15                        // (64 + 4)*32*sizeof(short)
    mov sp, x17

    sub x0, x0, x1                          // src += -1 * i_src;
    lsl x16, x4, #1

    //--------------------------------
    // HOR
    //--------------------------------
    ld1 {v4.s}[0], [x6]
    dup  v11.8h, w8
    abs  v3.8b, v4.8b
    uxtl v3.8h, v3.8b
    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]
    sub x0, x0, #2                          // x - 1 UV
    sub x1, x1, #64

    cmp w8, #255
    bgt if_hor_ver_chroma_w32_10bit

    add w8, w5, #3
if_hor_ver_chroma_w32_hor_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], #32         // src[x-1]
    ld1 {v26.8h, v27.8h}, [x0], #32
    ld1 {v18.8h}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #2       // src[x]
    ext v20.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6       // src[x+2]
    ext v22.16b, v17.16b, v26.16b, #2
    ext v23.16b, v17.16b, v26.16b, #4
    ext v24.16b, v17.16b, v26.16b, #6

    mul v28.8h, v19.8h, v1.8h
    mul v29.8h, v22.8h, v1.8h
    mls v28.8h, v16.8h, v0.8h
    mls v29.8h, v17.8h, v0.8h
    mla v28.8h, v20.8h, v2.8h
    mla v29.8h, v23.8h, v2.8h
    mls v28.8h, v21.8h, v3.8h
    mls v29.8h, v24.8h, v3.8h

    ext v19.16b, v26.16b, v27.16b, #2       // src[x+16]
    ext v20.16b, v26.16b, v27.16b, #4       // src[x+17]
    ext v21.16b, v26.16b, v27.16b, #6       // src[x+18]
    ext v22.16b, v27.16b, v18.16b, #2
    ext v23.16b, v27.16b, v18.16b, #4
    ext v24.16b, v27.16b, v18.16b, #6

    mul v30.8h, v19.8h, v1.8h
    mul v31.8h, v22.8h, v1.8h
    mls v30.8h, v26.8h, v0.8h
    mls v31.8h, v27.8h, v0.8h
    mla v30.8h, v20.8h, v2.8h
    mla v31.8h, v23.8h, v2.8h
    mls v30.8h, v21.8h, v3.8h
    mls v31.8h, v24.8h, v3.8h

    subs w8, w8, #1
    st1 {v28.2d, v29.2d, v30.2d, v31.2d}, [x17], #64
    bgt if_hor_ver_chroma_w32_hor_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w32_ver_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64        // x-i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    mov x10, x17
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64
    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [x17]

    smull  v4.4s, v16.4h, v0.h[0]
    smull2 v5.4s, v16.8h, v0.h[0]
    smull  v6.4s, v17.4h, v0.h[0]
    smull2 v7.4s, v17.8h, v0.h[0]
    smlal  v4.4s, v20.4h, v0.h[1]
    smlal2 v5.4s, v20.8h, v0.h[1]
    smlal  v6.4s, v21.4h, v0.h[1]
    smlal2 v7.4s, v21.8h, v0.h[1]
    smlal  v4.4s, v24.4h, v0.h[2]
    smlal2 v5.4s, v24.8h, v0.h[2]
    smlal  v6.4s, v25.4h, v0.h[2]
    smlal2 v7.4s, v25.8h, v0.h[2]
    smlal  v4.4s, v28.4h, v0.h[3]
    smlal2 v5.4s, v28.8h, v0.h[3]
    smlal  v6.4s, v29.4h, v0.h[3]
    smlal2 v7.4s, v29.8h, v0.h[3]

    sqrshrun  v4.4h, v4.4s, #12
    sqrshrun2 v4.8h, v5.4s, #12
    sqrshrun  v5.4h, v6.4s, #12
    sqrshrun2 v5.8h, v7.4s, #12

    umin   v4.8h, v4.8h, v11.8h
    umin   v5.8h, v5.8h, v11.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v22.4h, v0.h[1]
    smlal2 v3.4s, v22.8h, v0.h[1]
    smlal  v6.4s, v23.4h, v0.h[1]
    smlal2 v7.4s, v23.8h, v0.h[1]
    smlal  v2.4s, v26.4h, v0.h[2]
    smlal2 v3.4s, v26.8h, v0.h[2]
    smlal  v6.4s, v27.4h, v0.h[2]
    smlal2 v7.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v30.4h, v0.h[3]
    smlal2 v3.4s, v30.8h, v0.h[3]
    smlal  v6.4s, v31.4h, v0.h[3]
    smlal2 v7.4s, v31.8h, v0.h[3]

    sqrshrun  v2.4h, v2.4s, #12
    sqrshrun2 v2.8h, v3.4s, #12
    sqrshrun  v3.4h, v6.4s, #12
    sqrshrun2 v3.8h, v7.4s, #12

    umin v6.8h, v2.8h, v11.8h
    umin v7.8h, v3.8h, v11.8h

    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x2], x3

    smull  v4.4s, v20.4h, v0.h[0]
    smull2 v5.4s, v20.8h, v0.h[0]
    smull  v6.4s, v21.4h, v0.h[0]
    smull2 v7.4s, v21.8h, v0.h[0]
    smlal  v4.4s, v24.4h, v0.h[1]
    smlal2 v5.4s, v24.8h, v0.h[1]
    smlal  v6.4s, v25.4h, v0.h[1]
    smlal2 v7.4s, v25.8h, v0.h[1]
    smlal  v4.4s, v28.4h, v0.h[2]
    smlal2 v5.4s, v28.8h, v0.h[2]
    smlal  v6.4s, v29.4h, v0.h[2]
    smlal2 v7.4s, v29.8h, v0.h[2]
    smlal  v4.4s, v12.4h, v0.h[3]
    smlal2 v5.4s, v12.8h, v0.h[3]
    smlal  v6.4s, v13.4h, v0.h[3]
    smlal2 v7.4s, v13.8h, v0.h[3]

    sqrshrun  v4.4h, v4.4s, #12
    sqrshrun2 v4.8h, v5.4s, #12
    sqrshrun  v5.4h, v6.4s, #12
    sqrshrun2 v5.8h, v7.4s, #12

    umin   v4.8h, v4.8h, v11.8h
    umin   v5.8h, v5.8h, v11.8h

    smull  v2.4s, v22.4h, v0.h[0]
    smull2 v3.4s, v22.8h, v0.h[0]
    smull  v6.4s, v23.4h, v0.h[0]
    smull2 v7.4s, v23.8h, v0.h[0]
    smlal  v2.4s, v26.4h, v0.h[1]
    smlal2 v3.4s, v26.8h, v0.h[1]
    smlal  v6.4s, v27.4h, v0.h[1]
    smlal2 v7.4s, v27.8h, v0.h[1]
    smlal  v2.4s, v30.4h, v0.h[2]
    smlal2 v3.4s, v30.8h, v0.h[2]
    smlal  v6.4s, v31.4h, v0.h[2]
    smlal2 v7.4s, v31.8h, v0.h[2]
    smlal  v2.4s, v14.4h, v0.h[3]
    smlal2 v3.4s, v14.8h, v0.h[3]
    smlal  v6.4s, v15.4h, v0.h[3]
    smlal2 v7.4s, v15.8h, v0.h[3]

    sqrshrun  v2.4h, v2.4s, #12
    sqrshrun2 v2.8h, v3.4s, #12
    sqrshrun  v3.4h, v6.4s, #12
    sqrshrun2 v3.8h, v7.4s, #12

    umin v6.8h, v2.8h, v11.8h
    umin v7.8h, v3.8h, v11.8h

    subs w5, w5, #2
    mov x17, x10
    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x2], x3
    bgt if_hor_ver_chroma_w32_ver_loop_y
    b   if_hor_ver_chroma_w32_end

if_hor_ver_chroma_w32_10bit:

    add w8, w5, #3
if_hor_ver_chroma_w32_hor_10bit_loop_y:
    ld1 {v16.8h, v17.8h}, [x0], #32         // src[x-1]
    ld1 {v26.8h, v27.8h}, [x0], #32
    ld1 {v18.8h}, [x0], x1

    ext v19.16b, v16.16b, v17.16b, #2       // src[x]
    ext v20.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6       // src[x+2]
    ext v22.16b, v17.16b, v26.16b, #2
    ext v23.16b, v17.16b, v26.16b, #4
    ext v24.16b, v17.16b, v26.16b, #6

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umull  v30.4s, v22.4h, v1.4h
    umull2 v31.4s, v22.8h, v1.8h
    umlsl  v28.4s, v16.4h, v0.4h
    umlsl2 v29.4s, v16.8h, v0.8h
    umlsl  v30.4s, v17.4h, v0.4h
    umlsl2 v31.4s, v17.8h, v0.8h
    umlal  v28.4s, v20.4h, v2.4h
    umlal2 v29.4s, v20.8h, v2.8h
    umlal  v30.4s, v23.4h, v2.4h
    umlal2 v31.4s, v23.8h, v2.8h
    umlsl  v28.4s, v21.4h, v3.4h
    umlsl2 v29.4s, v21.8h, v3.8h
    umlsl  v30.4s, v24.4h, v3.4h
    umlsl2 v31.4s, v24.8h, v3.8h

    rshrn  v28.4h, v28.4s, #2
    rshrn2 v28.8h, v29.4s, #2
    rshrn  v29.4h, v30.4s, #2
    rshrn2 v29.8h, v31.4s, #2

    st1 {v28.8h, v29.8h}, [x17], #32

    ext v19.16b, v26.16b, v27.16b, #2       // src[x]
    ext v20.16b, v26.16b, v27.16b, #4       // src[x+1]
    ext v21.16b, v26.16b, v27.16b, #6       // src[x+2]
    ext v22.16b, v27.16b, v18.16b, #2
    ext v23.16b, v27.16b, v18.16b, #4
    ext v24.16b, v27.16b, v18.16b, #6

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umlsl  v28.4s, v26.4h, v0.4h
    umlsl2 v29.4s, v26.8h, v0.8h
    umlal  v28.4s, v20.4h, v2.4h
    umlal2 v29.4s, v20.8h, v2.8h
    umlsl  v28.4s, v21.4h, v3.4h
    umlsl2 v29.4s, v21.8h, v3.8h

    umull  v30.4s, v22.4h, v1.4h
    umull2 v31.4s, v22.8h, v1.8h
    umlsl  v30.4s, v27.4h, v0.4h
    umlsl2 v31.4s, v27.8h, v0.8h
    umlal  v30.4s, v23.4h, v2.4h
    umlal2 v31.4s, v23.8h, v2.8h
    umlsl  v30.4s, v24.4h, v3.4h
    umlsl2 v31.4s, v24.8h, v3.8h

    rshrn  v28.4h, v28.4s, #2
    rshrn2 v28.8h, v29.4s, #2
    rshrn  v29.4h, v30.4s, #2
    rshrn2 v29.8h, v31.4s, #2

    subs w8, w8, #1

    st1 {v28.8h, v29.8h}, [x17], #32
    bgt if_hor_ver_chroma_w32_hor_10bit_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w32_ver_10bit_loop_y:
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x17], #64        // x-i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x17], #64
    mov x10, x17
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x17], #64
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x17], #64
    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [x17]

    smull  v4.4s, v16.4h, v0.h[0]
    smull2 v5.4s, v16.8h, v0.h[0]
    smull  v6.4s, v17.4h, v0.h[0]
    smull2 v7.4s, v17.8h, v0.h[0]
    smlal  v4.4s, v20.4h, v0.h[1]
    smlal2 v5.4s, v20.8h, v0.h[1]
    smlal  v6.4s, v21.4h, v0.h[1]
    smlal2 v7.4s, v21.8h, v0.h[1]
    smlal  v4.4s, v24.4h, v0.h[2]
    smlal2 v5.4s, v24.8h, v0.h[2]
    smlal  v6.4s, v25.4h, v0.h[2]
    smlal2 v7.4s, v25.8h, v0.h[2]
    smlal  v4.4s, v28.4h, v0.h[3]
    smlal2 v5.4s, v28.8h, v0.h[3]
    smlal  v6.4s, v29.4h, v0.h[3]
    smlal2 v7.4s, v29.8h, v0.h[3]

    sqrshrun  v4.4h, v4.4s, #10
    sqrshrun2 v4.8h, v5.4s, #10
    sqrshrun  v5.4h, v6.4s, #10
    sqrshrun2 v5.8h, v7.4s, #10

    umin   v4.8h, v4.8h, v11.8h
    umin   v5.8h, v5.8h, v11.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v22.4h, v0.h[1]
    smlal2 v3.4s, v22.8h, v0.h[1]
    smlal  v6.4s, v23.4h, v0.h[1]
    smlal2 v7.4s, v23.8h, v0.h[1]
    smlal  v2.4s, v26.4h, v0.h[2]
    smlal2 v3.4s, v26.8h, v0.h[2]
    smlal  v6.4s, v27.4h, v0.h[2]
    smlal2 v7.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v30.4h, v0.h[3]
    smlal2 v3.4s, v30.8h, v0.h[3]
    smlal  v6.4s, v31.4h, v0.h[3]
    smlal2 v7.4s, v31.8h, v0.h[3]

    sqrshrun  v2.4h, v2.4s, #10
    sqrshrun2 v2.8h, v3.4s, #10
    sqrshrun  v3.4h, v6.4s, #10
    sqrshrun2 v3.8h, v7.4s, #10

    umin v6.8h, v2.8h, v11.8h
    umin v7.8h, v3.8h, v11.8h

    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x2], x3

    smull  v4.4s, v20.4h, v0.h[0]
    smull2 v5.4s, v20.8h, v0.h[0]
    smull  v6.4s, v21.4h, v0.h[0]
    smull2 v7.4s, v21.8h, v0.h[0]
    smlal  v4.4s, v24.4h, v0.h[1]
    smlal2 v5.4s, v24.8h, v0.h[1]
    smlal  v6.4s, v25.4h, v0.h[1]
    smlal2 v7.4s, v25.8h, v0.h[1]
    smlal  v4.4s, v28.4h, v0.h[2]
    smlal2 v5.4s, v28.8h, v0.h[2]
    smlal  v6.4s, v29.4h, v0.h[2]
    smlal2 v7.4s, v29.8h, v0.h[2]
    smlal  v4.4s, v12.4h, v0.h[3]
    smlal2 v5.4s, v12.8h, v0.h[3]
    smlal  v6.4s, v13.4h, v0.h[3]
    smlal2 v7.4s, v13.8h, v0.h[3]

    sqrshrun  v4.4h, v4.4s, #10
    sqrshrun2 v4.8h, v5.4s, #10
    sqrshrun  v5.4h, v6.4s, #10
    sqrshrun2 v5.8h, v7.4s, #10

    umin   v4.8h, v4.8h, v11.8h
    umin   v5.8h, v5.8h, v11.8h

    smull  v2.4s, v22.4h, v0.h[0]
    smull2 v3.4s, v22.8h, v0.h[0]
    smull  v6.4s, v23.4h, v0.h[0]
    smull2 v7.4s, v23.8h, v0.h[0]
    smlal  v2.4s, v26.4h, v0.h[1]
    smlal2 v3.4s, v26.8h, v0.h[1]
    smlal  v6.4s, v27.4h, v0.h[1]
    smlal2 v7.4s, v27.8h, v0.h[1]
    smlal  v2.4s, v30.4h, v0.h[2]
    smlal2 v3.4s, v30.8h, v0.h[2]
    smlal  v6.4s, v31.4h, v0.h[2]
    smlal2 v7.4s, v31.8h, v0.h[2]
    smlal  v2.4s, v14.4h, v0.h[3]
    smlal2 v3.4s, v14.8h, v0.h[3]
    smlal  v6.4s, v15.4h, v0.h[3]
    smlal2 v7.4s, v15.8h, v0.h[3]

    sqrshrun  v2.4h, v2.4s, #10
    sqrshrun2 v2.8h, v3.4s, #10
    sqrshrun  v3.4h, v6.4s, #10
    sqrshrun2 v3.8h, v7.4s, #10

    umin v6.8h, v2.8h, v11.8h
    umin v7.8h, v3.8h, v11.8h

    subs w5, w5, #2
    mov x17, x10
    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x2], x3
    bgt if_hor_ver_chroma_w32_ver_10bit_loop_y

if_hor_ver_chroma_w32_end:
    add sp, sp, x15                     // (128 + 4)*64*2*sizeof(short)

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64

    ret

//void uavs3e_if_hor_ver_chroma_w32x_arm64(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff_h, const s8n 0- *coeff_v, int max_val)
//src->x0, i_src->x1, dst->x2, i_dst->x3, width->x4, height->x5, coeff_h->x6, coeff_v->x7, max_val = 255
function uavs3e_if_hor_ver_chroma_w32x_arm64
    ldr w8, [sp]
    lsl x1, x1, #1
    lsl x3, x3, #1

    sub sp, sp, #64
    st1 {v8.2d - v11.2d}, [sp]
    sub sp, sp, #64
    st1 {v12.2d - v15.2d}, [sp]

    // align (x17)
    // x17-->tmp
    mov x15, #68
    lsl x15, x15, #8
    sub x17, sp, x15                        // (64 + 4)*128*sizeof(short)
    mov sp, x17

    sub x0, x0, x1                          // src += -1 * i_src;
    lsl x16, x4, #1

    //--------------------------------
    // HOR
    //--------------------------------
    ld1 {v4.s}[0], [x6]
    dup v11.8h, w8
    abs v3.8b, v4.8b
    uxtl v3.8h, v3.8b

    dup v0.8h, v3.h[0]
    dup v1.8h, v3.h[1]
    dup v2.8h, v3.h[2]
    dup v3.8h, v3.h[3]
    sub x0, x0, #2                          // x - 1 UV

    cmp w8, #255
    bgt hor_ver_chroma_w32x_10bit

    add w8, w5, #3
if_hor_ver_chroma_w32x_hor_loop_y:
    mov x9, x4
    mov x10, x0
    mov x11, x17
if_hor_ver_chroma_w32x_hor_loop_x:
    ld1 {v16.8h, v17.8h}, [x10], #32        // src[x-1]
    ld1 {v26.8h, v27.8h}, [x10], #32
    ld1 {v18.8h}, [x10]

    ext v19.16b, v16.16b, v17.16b, #2       // src[x]
    ext v20.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6       // src[x+2]
    ext v22.16b, v17.16b, v26.16b, #2
    ext v23.16b, v17.16b, v26.16b, #4
    ext v24.16b, v17.16b, v26.16b, #6

    mul v28.8h, v19.8h, v1.8h
    mul v29.8h, v22.8h, v1.8h
    mls v28.8h, v16.8h, v0.8h
    mls v29.8h, v17.8h, v0.8h
    mla v28.8h, v20.8h, v2.8h
    mla v29.8h, v23.8h, v2.8h
    mls v28.8h, v21.8h, v3.8h
    mls v29.8h, v24.8h, v3.8h

    ext v19.16b, v26.16b, v27.16b, #2       // src[x+16]
    ext v20.16b, v26.16b, v27.16b, #4       // src[x+17]
    ext v21.16b, v26.16b, v27.16b, #6       // src[x+18]
    ext v22.16b, v27.16b, v18.16b, #2
    ext v23.16b, v27.16b, v18.16b, #4
    ext v24.16b, v27.16b, v18.16b, #6

    mul v30.8h, v19.8h, v1.8h
    mul v31.8h, v22.8h, v1.8h
    mls v30.8h, v26.8h, v0.8h
    mls v31.8h, v27.8h, v0.8h
    mla v30.8h, v20.8h, v2.8h
    mla v31.8h, v23.8h, v2.8h
    mls v30.8h, v21.8h, v3.8h
    mls v31.8h, v24.8h, v3.8h

    subs w9, w9, #32
    st1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x11], #64
    bgt if_hor_ver_chroma_w32x_hor_loop_x

    subs w8, w8, #1
    add x0, x0, x1
    add x17, x17, x16
    bgt if_hor_ver_chroma_w32x_hor_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w32x_ver_loop_y:
    mov x9, #0
    mov x11, x2
if_hor_ver_chroma_w32x_ver_loop_x:
    add x10, x17, x9, lsl #1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16        // x-i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10]

    smull  v4.4s, v16.4h, v0.h[0]
    smull2 v5.4s, v16.8h, v0.h[0]
    smull  v6.4s, v17.4h, v0.h[0]
    smull2 v7.4s, v17.8h, v0.h[0]
    smlal  v4.4s, v20.4h, v0.h[1]
    smlal2 v5.4s, v20.8h, v0.h[1]
    smlal  v6.4s, v21.4h, v0.h[1]
    smlal2 v7.4s, v21.8h, v0.h[1]
    smlal  v4.4s, v24.4h, v0.h[2]
    smlal2 v5.4s, v24.8h, v0.h[2]
    smlal  v6.4s, v25.4h, v0.h[2]
    smlal2 v7.4s, v25.8h, v0.h[2]
    smlal  v4.4s, v28.4h, v0.h[3]
    smlal2 v5.4s, v28.8h, v0.h[3]
    smlal  v6.4s, v29.4h, v0.h[3]
    smlal2 v7.4s, v29.8h, v0.h[3]

    sqrshrun  v4.4h, v4.4s, #12
    sqrshrun2 v4.8h, v5.4s, #12
    sqrshrun  v5.4h, v6.4s, #12
    sqrshrun2 v5.8h, v7.4s, #12

    umin v4.8h, v4.8h, v11.8h
    umin v5.8h, v5.8h, v11.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v22.4h, v0.h[1]
    smlal2 v3.4s, v22.8h, v0.h[1]
    smlal  v6.4s, v23.4h, v0.h[1]
    smlal2 v7.4s, v23.8h, v0.h[1]
    smlal  v2.4s, v26.4h, v0.h[2]
    smlal2 v3.4s, v26.8h, v0.h[2]
    smlal  v6.4s, v27.4h, v0.h[2]
    smlal2 v7.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v30.4h, v0.h[3]
    smlal2 v3.4s, v30.8h, v0.h[3]
    smlal  v6.4s, v31.4h, v0.h[3]
    smlal2 v7.4s, v31.8h, v0.h[3]

    sqrshrun  v2.4h, v2.4s, #12
    sqrshrun2 v2.8h, v3.4s, #12
    sqrshrun  v3.4h, v6.4s, #12
    sqrshrun2 v3.8h, v7.4s, #12

    umin v6.8h, v2.8h, v11.8h
    umin v7.8h, v3.8h, v11.8h

    add w9, w9, #32
    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x11], #64
    //--------------------------------
    // loop control
    //--------------------------------
    cmp w9, w4
    blt if_hor_ver_chroma_w32x_ver_loop_x

    subs w5, w5, #1
    add x17, x17, x16                       // tmp += i_tmp;
    add x2, x2, x3                          // dst += i_dst
    bgt if_hor_ver_chroma_w32x_ver_loop_y
    b   hor_ver_chroma_w32x_end

hor_ver_chroma_w32x_10bit:
    add w8, w5, #3
if_hor_ver_chroma_w32x_hor_10bit_loop_y:
    mov x9, x4
    mov x10, x0
    mov x11, x17
if_hor_ver_chroma_w32x_hor_10bit_loop_x:
    ld1 {v16.8h, v17.8h}, [x10], #32        // src[x-1]
    ld1 {v26.8h, v27.8h}, [x10], #32
    ld1 {v18.8h}, [x10]

    ext v19.16b, v16.16b, v17.16b, #2       // src[x]
    ext v20.16b, v16.16b, v17.16b, #4       // src[x+1]
    ext v21.16b, v16.16b, v17.16b, #6      // src[x+2]
    ext v22.16b, v17.16b, v26.16b, #2
    ext v23.16b, v17.16b, v26.16b, #4
    ext v24.16b, v17.16b, v26.16b, #6

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umull  v30.4s, v22.4h, v1.4h
    umull2 v31.4s, v22.8h, v1.8h
    umlsl  v28.4s, v16.4h, v0.4h
    umlsl2 v29.4s, v16.8h, v0.8h
    umlsl  v30.4s, v17.4h, v0.4h
    umlsl2 v31.4s, v17.8h, v0.8h
    umlal  v28.4s, v20.4h, v2.4h
    umlal2 v29.4s, v20.8h, v2.8h
    umlal  v30.4s, v23.4h, v2.4h
    umlal2 v31.4s, v23.8h, v2.8h
    umlsl  v28.4s, v21.4h, v3.4h
    umlsl2 v29.4s, v21.8h, v3.8h
    umlsl  v30.4s, v24.4h, v3.4h
    umlsl2 v31.4s, v24.8h, v3.8h

    rshrn  v28.4h, v28.4s, #2
    rshrn2 v28.8h, v29.4s, #2
    rshrn  v29.4h, v30.4s, #2
    rshrn2 v29.8h, v31.4s, #2

    st1 {v28.8h, v29.8h}, [x11], #32

    ext v19.16b, v26.16b, v27.16b, #2       // src[x]
    ext v20.16b, v26.16b, v27.16b, #4       // src[x+1]
    ext v21.16b, v26.16b, v27.16b, #6       // src[x+2]
    ext v22.16b, v27.16b, v18.16b, #2
    ext v23.16b, v27.16b, v18.16b, #4
    ext v24.16b, v27.16b, v18.16b, #6

    umull  v28.4s, v19.4h, v1.4h
    umull2 v29.4s, v19.8h, v1.8h
    umlsl  v28.4s, v26.4h, v0.4h
    umlsl2 v29.4s, v26.8h, v0.8h
    umlal  v28.4s, v20.4h, v2.4h
    umlal2 v29.4s, v20.8h, v2.8h
    umlsl  v28.4s, v21.4h, v3.4h
    umlsl2 v29.4s, v21.8h, v3.8h

    umull  v30.4s, v22.4h, v1.4h
    umull2 v31.4s, v22.8h, v1.8h
    umlsl  v30.4s, v27.4h, v0.4h
    umlsl2 v31.4s, v27.8h, v0.8h
    umlal  v30.4s, v23.4h, v2.4h
    umlal2 v31.4s, v23.8h, v2.8h
    umlsl  v30.4s, v24.4h, v3.4h
    umlsl2 v31.4s, v24.8h, v3.8h

    rshrn  v28.4h, v28.4s, #2
    rshrn2 v28.8h, v29.4s, #2
    rshrn  v29.4h, v30.4s, #2
    rshrn2 v29.8h, v31.4s, #2

    subs w9, w9, #32
    st1 {v28.8h, v29.8h}, [x11], #32
    bgt if_hor_ver_chroma_w32x_hor_10bit_loop_x

    subs w8, w8, #1
    add x0, x0, x1
    add x17, x17, x16
    bgt if_hor_ver_chroma_w32x_hor_10bit_loop_y

//--------------------------------
// VER
//--------------------------------
    mov x17, sp

    ld1 {v0.s}[0], [x7]                     // load coeff
    sxtl v0.8h, v0.8b                       // 8bit to 16bit

if_hor_ver_chroma_w32x_ver_10bit_loop_y:
    mov x9, #0
    mov x11, x2
if_hor_ver_chroma_w32x_ver_10bit_loop_x:
    add x10, x17, x9, lsl #1
    ld1 {v16.8h, v17.8h, v18.8h, v19.8h}, [x10], x16        // x-i_src
    ld1 {v20.8h, v21.8h, v22.8h, v23.8h}, [x10], x16
    ld1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x10], x16
    ld1 {v28.8h, v29.8h, v30.8h, v31.8h}, [x10]

    smull  v4.4s, v16.4h, v0.h[0]
    smull2 v5.4s, v16.8h, v0.h[0]
    smull  v6.4s, v17.4h, v0.h[0]
    smull2 v7.4s, v17.8h, v0.h[0]
    smlal  v4.4s, v20.4h, v0.h[1]
    smlal2 v5.4s, v20.8h, v0.h[1]
    smlal  v6.4s, v21.4h, v0.h[1]
    smlal2 v7.4s, v21.8h, v0.h[1]
    smlal  v4.4s, v24.4h, v0.h[2]
    smlal2 v5.4s, v24.8h, v0.h[2]
    smlal  v6.4s, v25.4h, v0.h[2]
    smlal2 v7.4s, v25.8h, v0.h[2]
    smlal  v4.4s, v28.4h, v0.h[3]
    smlal2 v5.4s, v28.8h, v0.h[3]
    smlal  v6.4s, v29.4h, v0.h[3]
    smlal2 v7.4s, v29.8h, v0.h[3]

    sqrshrun  v4.4h, v4.4s, #10
    sqrshrun2 v4.8h, v5.4s, #10
    sqrshrun  v5.4h, v6.4s, #10
    sqrshrun2 v5.8h, v7.4s, #10

    umin v4.8h, v4.8h, v11.8h
    umin v5.8h, v5.8h, v11.8h

    smull  v2.4s, v18.4h, v0.h[0]
    smull2 v3.4s, v18.8h, v0.h[0]
    smull  v6.4s, v19.4h, v0.h[0]
    smull2 v7.4s, v19.8h, v0.h[0]
    smlal  v2.4s, v22.4h, v0.h[1]
    smlal2 v3.4s, v22.8h, v0.h[1]
    smlal  v6.4s, v23.4h, v0.h[1]
    smlal2 v7.4s, v23.8h, v0.h[1]
    smlal  v2.4s, v26.4h, v0.h[2]
    smlal2 v3.4s, v26.8h, v0.h[2]
    smlal  v6.4s, v27.4h, v0.h[2]
    smlal2 v7.4s, v27.8h, v0.h[2]
    smlal  v2.4s, v30.4h, v0.h[3]
    smlal2 v3.4s, v30.8h, v0.h[3]
    smlal  v6.4s, v31.4h, v0.h[3]
    smlal2 v7.4s, v31.8h, v0.h[3]

    sqrshrun  v2.4h, v2.4s, #10
    sqrshrun2 v2.8h, v3.4s, #10
    sqrshrun  v3.4h, v6.4s, #10
    sqrshrun2 v3.8h, v7.4s, #10

    umin v6.8h, v2.8h, v11.8h
    umin v7.8h, v3.8h, v11.8h

    add w9, w9, #32
    st1 {v4.8h, v5.8h, v6.8h, v7.8h}, [x11], #64
    //--------------------------------
    // loop control
    //--------------------------------
    cmp w9, w4
    blt if_hor_ver_chroma_w32x_ver_10bit_loop_x

    subs w5, w5, #1
    add x17, x17, x16                       // tmp += i_tmp;
    add x2, x2, x3                          // dst += i_dst
    bgt if_hor_ver_chroma_w32x_ver_10bit_loop_y

hor_ver_chroma_w32x_end:
    add sp, sp, x15                     // (64 + 4)*64*2*sizeof(short)

    ld1 {v12.2d - v15.2d}, [sp], #64
    ld1 {v8.2d - v11.2d}, [sp], #64

    ret

#endif

#endif
