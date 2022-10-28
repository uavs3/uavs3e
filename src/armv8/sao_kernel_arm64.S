#include "def_arm64.S"

#if defined(__arm64__)

#if !COMPILE_10BIT
/***********************************************************************************************************************************
*  void uavs3e_sao_eo_0_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_x, int end_x, int mb_height, char_t* mask)
*  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_x->x5, end_x->x6, mb_height->x7, mask->x8
************************************************************************************************************************************/
function uavs3e_sao_eo_0_arm64
    ldr x8, [sp]                // mask

    // ------- load mask -------
    sub x10, x6, x5
    and x9, x10, #15
    add x12, x8, x9, lsl #4
    ld1 {v31.4s}, [x12]         // mask + (((end_x - start_x) & 0x0f) - 1)*16

    sub w10, w6, w9             // end_x_16 = end_x - ((end_x - start_x) & 0x0f)

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]         // offset[0-3]
    ldr  w9, [x4, #16]          // offset[4]
    mov  x8, #0
    movi v2.4s, #0
    mov  v2.s[0], w9
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h           // v0.8b: offset[0-4]

    movi v1.16b, #2

loop_y_eo_0:

    mov x9, x5                  // x = start_x
loop_x_eo_0:

    add  x12, x0, x9
    sub  x13, x12, #1
    add  x14, x12, #1
    
    ld1  {v21.16b}, [x12]       // src[x]
    ld1  {v20.16b}, [x13]       // src[x-1]
    ld1  {v22.16b}, [x14]       // src[x+1]

    // leftsign & rightsign
    umin v23.16b, v20.16b, v21.16b
    umin v24.16b, v21.16b, v22.16b

    cmeq v25.16b, v23.16b, v20.16b
    cmeq v26.16b, v23.16b, v21.16b
    cmeq v27.16b, v24.16b, v21.16b
    cmeq v28.16b, v24.16b, v22.16b

    sub  v20.16b, v26.16b, v25.16b      // leftsign
    sub  v22.16b, v27.16b, v28.16b      // rightsign

    // get edgetype
    add v20.16b, v22.16b, v20.16b       // edgetype

    uxtl  v23.8h, v21.8b
    uxtl2 v24.8h, v21.16b               // src[x]

    add v20.16b, v20.16b, v1.16b        // edgetype+2
    tbl v22.16b, {v0.16b}, v20.16b      // offset

    saddw  v23.8h, v23.8h, v22.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v24.8h, v22.16b      // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h

    add x12, x1, x9
    cmp x9, x10
    beq maskmove_eo_0
    add x9, x9, #16
    st1 {v20.16b}, [x12]
    cmp x9, x6
    blt loop_x_eo_0
    b   loop_x_eo_0_end

maskmove_eo_0:
    // maskmove
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v31.16b
    st1 {v20.16b}, [x12]

loop_x_eo_0_end:
    subs w7, w7, #1
    add x0, x0, x2
    add x1, x1, x3
    bne loop_y_eo_0

    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_0_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_x, int end_x, int mb_height, char_t* mask)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_x->x5, end_x->x6, mb_height->x7,
 *  mask->x8
 ************************************************************************************************************************************/
function uavs3e_sao_eo_0_chroma_arm64
    ldr     x8, [sp]                        // mask

    sub     x10, x6, x5
    lsr     x10, x10, #1
    and     x9 , x10, #15
    add     x12, x8, x9, lsl#4
    ld1     {v25.4s}, [x12]                 //-- load mask: mask + (((end_x - start_x) & 0x0f) - 1)*16
    sxtl    v17.8h, v25.8b
    sxtl2   v18.8h, v25.16b

    sub     x10, x6, x9, lsl #1             //-- x10 = end_x_16 = end_x - ((end_x - start_x) & 0x0f)

    mov     w8, #0x00ff
    dup     v31.8h, w8                      //-- mask_uv: for uv interlace

    and     v17.16b, v17.16b, v31.16b       //-- mask for last cols
    and     v18.16b, v18.16b, v31.16b

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]                  //-- load offset[0-3]
    ldr     w9, [x4, #16]                   //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w9
    xtn     v0.8b, v0.8h                    //-- convert int32 to byte

    movi    v1.16b , #2                     //-- constant(save)

loop_y_eo_0_chroma:
    mov     x9, x5                          //-- x = start_x
loop_x_eo_0_chroma:
    add     x12, x0 , x9
    sub     x13, x12, #2
    add     x14, x12 , #2
    ld1     {v4.8h, v5.8h}, [x12]           //-- load src[x]
    ld1     {v6.8h, v7.8h}, [x13]           //-- load src[x-2]
    ld1     {v2.8h, v3.8h}, [x14]           //-- load src[x+2]
    xtn     v21.8b, v4.8h                   //-- delete the other chroma
    xtn2    v21.16b, v5.8h                  //-- src[x]
    xtn     v20.8b , v6.8h
    xtn2    v20.16b, v7.8h                  //-- src[x-2]
    xtn     v22.8b , v2.8h
    xtn2    v22.16b, v3.8h

    umin    v23.16b, v20.16b, v21.16b
    umin    v26.16b, v21.16b, v22.16b
    cmeq    v24.16b, v23.16b, v20.16b
    cmeq    v25.16b, v23.16b, v21.16b
    cmeq    v27.16b, v26.16b, v21.16b
    cmeq    v28.16b, v26.16b, v22.16b
    sub     v20.16b, v25.16b, v24.16b       //-- leftsign
    sub     v22.16b, v27.16b, v28.16b       //-- rightsign

    add     v20.16b, v22.16b, v20.16b       //-- edgetype
    add     v20.16b, v20.16b, v1.16b        //-- generate look-up indexs
    tbl     v22.16b, {v0.16b}, v20.16b      //-- get offset

    uxtl    v23.8h, v21.8b                  //-- src[x] low 8 samples
    uxtl2   v24.8h, v21.16b

    add     x12, x1, x9                     //-- dst + x

    saddw   v23.8h, v23.8h, v22.8b
    saddw2  v24.8h, v24.8h, v22.16b

    ld1     {v4.8h, v5.8h}, [x12]           //-- load 32 pixels from dst+x
    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h                 //-- results

    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b

    cmp     x9, x10
    beq     maskmove_eo_0_chroma
    bif     v21.16b, v4.16b, v31.16b
    bif     v22.16b, v5.16b, v31.16b
    add     x9, x9, #32
    st1     {v21.16b, v22.16b}, [x12]
    cmp     x9, x6
    blt     loop_x_eo_0_chroma
    b       loop_x_eo_0_chroma_end

maskmove_eo_0_chroma:
    //--- maskmove
    bif    v21.16b, v4.16b, v17.16b
    bif    v22.16b, v5.16b, v18.16b
    st1    {v21.16b, v22.16b}, [x12]

loop_x_eo_0_chroma_end:
    subs   x7, x7, #1                       //-- mb_height--
    add    x0, x0, x2                       //-- src += src_stride
    add    x1, x1, x3                       //-- dst += dst_stride
    bgt    loop_y_eo_0_chroma

    ret


/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_90_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_y, int end_y, int mb_width);
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_y->x5, end_y->x6, mb_width->x7
 ************************************************************************************************************************************/
function uavs3e_sao_eo_90_arm64

    mul x8, x2, x5
    mul x9, x3, x5
    add x0, x0, x8
    add x1, x1, x9              // update x0 and x2

    sub x10, x7, #15            // end_x_16 = mb_width - 15

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]         // offset[0-3]
    ldr  w9, [x4, #16]          // offset[4]
    mov  x8, #0
    movi v2.4s, #0
    mov  v2.s[0], w9
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h           // offset[0-4]

    movi v1.16b, #2

    sub  w8, w6, w5             // loop_y = end_y - start_y

    mov  w12, #0
    mov  w9, #-1
    cmp  w7, #4
    beq  set_mask_width_4
    movi v30.2d, #-1
    mov  v30.s[3], w12          //  mask="-1 repeat 12, 0, 0, 0, 0"
    b    loop_y_eo_90

set_mask_width_4:

    movi v30.4s, #0
    mov  v30.s[0], w9           // mask="-1, -1, -1, -1, 0 repeat 12"

loop_y_eo_90:

    mov  x9, #0                 // x = 0

loop_x_eo_90:
    add  x12, x0, x9            // x12
    sub  x13, x12, x2
    add  x14, x12, x2
    ld1  {v21.16b}, [x12]       // src[x]
    ld1  {v20.16b}, [x13]       // src[x-src_stride]
    ld1  {v22.16b}, [x14]       // src[x+src_stride]

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             // $v20.16b the result = offset+src[x]

    add x12, x1, x9
    cmp x9, x10
    bge maskmove_eo_90
    st1 {v20.16b}, [x12]
    add x9, x9, #16
    cmp x9, x7
    blt loop_x_eo_90
    b   loop_x_eo_90_end

maskmove_eo_90:
    // maskmove
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

loop_x_eo_90_end:
    subs w8, w8, #1                     // loop_y--
    add x0, x0, x2
    add x1, x1, x3
    bgt loop_y_eo_90

    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_90_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_y, int end_y, int mb_width, int bit_depth);
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_y->x5, end_y->x6, mb_width->x7
 ************************************************************************************************************************************/
function uavse_sao_eo_90_chroma_arm64

    mul     x8, x2, x5
    mul     x9, x3, x5
    add     x0, x0, x8                  //-- src -= start_y*src_stride
    add     x1, x1, x9                  //-- dst -= start_y*dst_stride

    sub     x10, x7, #31                //-- x10 = end_x_16 = mb_width - 31

    mov     w8, #0x00ff
    dup     v31.8h, w8

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]              //-- load offset[0-3]
    ldr     w9 , [x4, #16]              //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w9
    xtn     v0.8b, v0.8h                //-- convert int32 to byte

    movi    v1.16b, #2                  //-- constant(save)

    sub     w8, w6, w5                  //-- y = end_y - start_y

    cmp     x7 , #8
    beq     set_mask_chroma_width_4
    movi    v18.16b, #255               //-- mask="-1 repeat 12, 0, 0, 0, 0"
    movi    v19.4s, #0
    mov     v19.d[0], v18.d[0]
    b       loop_y_eo_90_chroma
set_mask_chroma_width_4:
    movi    v18.8b, #255
    movi    v19.4s, #0
    mov     v18.d[1], v19.d[0]          //-- mask="-1, -1, -1, -1, 0 repeat 12"

loop_y_eo_90_chroma:
    mov     x9, #0                      //-- x = 0
loop_x_eo_90_chroma:
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    ld1     {v2.8h, v3.8h}, [x12]       //-- load src[x]    (save)
    ld1     {v4.8h, v5.8h}, [x13]       //-- load src[x - src_stride]
    ld1     {v6.8h, v7.8h}, [x14]       //-- load src[x + src_stride]
    xtn     v21.8b , v2.8h
    xtn2    v21.16b, v3.8h
    xtn     v20.8b , v4.8h
    xtn2    v20.16b, v5.8h
    xtn     v22.8b , v6.8h
    xtn2    v22.16b, v7.8h

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add     x12, x1, x9                 //-- dst+x
    cmp     x9, x10
    bge     maskmove_eo_90_chroma
    ld1     {v2.8h, v3.8h}, [x12]
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    add     x9, x9, #32
    bif     v21.16b, v2.16b, v31.16b
    bif     v22.16b, v3.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    cmp     x9, x7
    blt     loop_x_eo_90_chroma
    b       loop_x_eo_90_chroma_end
maskmove_eo_90_chroma:
    //--- maskmove
    ld1     {v2.8h, v3.8h}, [x12]
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    bif     v21.16b, v2.16b, v18.16b
    bif     v22.16b, v3.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_eo_90_chroma_end:
    subs    w8, w8, #1                  //-- y--
    add     x0, x0, x2                  //-- src+=src_stride
    add     x1, x1, x3                  //-- dst+=dst_stride
    bgt     loop_y_eo_90_chroma

    ret


/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_135_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int bit_depth, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, start_x_r0->x7,
 *  end_x_r0->x8, start_x_r->x9, end_x_r->x10, start_x_rn->x11, end_x_rn->x12
 ************************************************************************************************************************************/
function uavs3e_sao_eo_135_arm64
    // get start_x_r0 and end_x_r0
#if defined(__APPLE__)
    ldp w8, w9, [sp]
#else
    ldp x8, x9, [sp]
#endif
    sxtw x8, w8                         // start_x_r0
    sxtw x9, w9                         // end_x_r0

    // get end_x_r0_16
    sub x11, x9, x8
    and x11, x11, #15
    sub x10, x9, x11                    // end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]                 // offset[0-3]
    ldr  w11, [x4, #16]                 // offset[4]
    movi v2.4s, #0
    mov  v2.s[0], w11
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h                   // offset[0-4]

    movi v1.16b, #2

    // ------- first row -------
    mov  x11, x8                        // x = start_x_r0

test_loop_x_eo_135_r0:

    cmp x11, x9
    bge test_loop_x_eo_135_end_r0

    add x12, x0, x11
    sub x13, x12, x2
    add x14, x12, x2
    sub x13, x13, #1
    add x14, x14, #1
    ld1 {v21.16b}, [x12]                // src[x]
    ld1 {v20.16b}, [x13]                // src[x-src_stride-1]
    ld1 {v22.16b}, [x14]                // src[x+src_stride+1]

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add x12, x1, x11
    cmp x11, x10
    bge test_maskmove_eo_135_r0
    st1 {v20.16b}, [x12]
    add x11, x11, #16
    b   test_loop_x_eo_135_r0

test_maskmove_eo_135_r0:
    sub x7, x9, x10
    add x7, x5, x7, lsl #4              // offset = 16*rownum
    ld1 {v30.4s}, [x7]                  // load mask_r0
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

test_loop_x_eo_135_end_r0:
    add x0, x0, x2                      // src+=src_stride
    add x1, x1, x3                      // dst+=dst_stride

    // ------- middle rows -------
    // get param
#if defined(__APPLE__)
    ldp w7, w8, [sp, #8]
#else
    ldp x7, x8, [sp, #16]
#endif
    sxtw x7, w7                         // start_x_r
    sxtw x8, w8                         // end_x_r

    sub x9, x8, x7
    and x9, x9, #15
    add x12, x5, x9, lsl #4
    ld1 {v30.4s}, [x12]                 // mask_r

    sub x10, x8, x9                     // end_x_r_16

    sub x11, x6, #2                     // y = mb_height - 2

test_loop_y_eo_135_r:

    mov x9, x7                          // x = start_x_r

test_loop_x_eo_135_r:
    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    sub x13, x13, #1
    add x14, x14, #1
    ld1 {v21.16b}, [x12]                // src[x]
    ld1 {v20.16b}, [x13]                // src[x-src_stride-1]
    ld1 {v22.16b}, [x14]                // src[x+src_stride+1]

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_135_r
    add x9, x9, #16
    st1 {v20.16b}, [x12]
    cmp x9, x8
    blt test_loop_x_eo_135_r
    b   test_loop_x_eo_135_end_r

test_maskmove_eo_135_r:
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

test_loop_x_eo_135_end_r:
    subs x11, x11, #1                   // y--
    add x0, x0, x2                      // src += src_stride
    add x1, x1, x3                      // dst += dst_stride
    bgt test_loop_y_eo_135_r

// ------- last row -------
#if defined(__APPLE__)
    ldp w6, w7, [sp, #16]
#else
    ldp x6, x7, [sp, #32]
#endif
    sxtw x6, w6                         // start_x_rn
    sxtw x7, w7                         // end_x_rn

    sub x8, x7, x6
    and x8, x8, #15
    sub x10, x7, x8                     // end_x_rn_16

    mov x9, x6                          // x = start_x_rn

test_loop_x_eo_135_rn:
    cmp x9, x7
    bge test_loop_x_eo_135_end_rn

    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    sub x13, x13, #1
    add x14, x14, #1
    ld1 {v21.16b}, [x12]                // src[x]
    ld1 {v20.16b}, [x13]                // src[x-src_stride-1]
    ld1 {v22.16b}, [x14]                // src[x+src_stride+1]

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_135_rn
    st1 {v20.16b}, [x12]
    add x9, x9, #16
    b   test_loop_x_eo_135_rn

test_maskmove_eo_135_rn:
    sub x6, x7, x10
    add x6, x5, x6, lsl #4              // offset = 16*rownum
    ld1 {v30.4s}, [x6]                  // load mask_r0
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

test_loop_x_eo_135_end_rn:

    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_135_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int bit_depth, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, start_x_r0->x7,
 *  end_x_r0->x8, start_x_r->x13, end_x_r->x14, start_x_rn->x15, end_x_rn->x16
 ************************************************************************************************************************************/
function uavs3e_sao_eo_135_chroma_arm64

#if defined(__APPLE__)
    ldp     w8, w9, [sp]
#else
    ldp     x8, x9, [sp]
#endif
    sxtw    x8, w8                          // start_x_r0
    sxtw    x9, w9                          // end_x_r0

    sub     x11, x9, x8
    and     x11, x11, #31
    sub     x10, x9, x11                    //-- end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f);

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]                  //-- load offset[0-3]
    ldr     w11, [x4, #16]                  //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w11
    xtn     v0.8b, v0.8h                    //-- convert int32 to byte

    mov     w11, #0x00ff
    dup     v31.8h, w11

    movi    v1.16b , #2                     //-- constant(save)

//---------------------first row-------------------------
    mov     x11, x8                         //-- x = start_x_r0
loop_x_eo_135_chroma_r0:
    cmp     x11, x9
    bge     loop_x_eo_135_chroma_end_r0
    add     x12, x0, x11
    sub     x13, x12, x2
    add     x14, x12, x2
    sub     x13, x13, #2
    add     x14, x14, #2

    ld1     {v2.8h, v3.8h}, [x12]       //-- load src[x] (save)
    ld1     {v4.8h, v5.8h}, [x13]       //-- load src[x-src_stride-2]
    ld1     {v6.8h, v7.8h}, [x14]       //-- load src[x+src_stride+2]

    xtn     v21.8b , v2.8h
    xtn2    v21.16b, v3.8h
    xtn     v20.8b , v4.8h
    xtn2    v20.16b, v5.8h
    xtn     v22.8b , v6.8h
    xtn2    v22.16b, v7.8h

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add     x12, x1, x11                 //-- dst+x
    cmp     x11, x10
    bge     maskmove_eo_135_chroma_r0

    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    add     x11, x11, #32
    b       loop_x_eo_135_chroma_r0
maskmove_eo_135_chroma_r0:
    sub     x7, x9, x10
    add     x7, x5, x7, lsl #3              //-- offset = 16*rowid
    ld1     {v25.4s}, [x7]                  //-- load mask_r0
    ld1     {v3.8h, v4.8h}, [x12]
    sxtl    v18.8h, v25.8b
    sxtl2   v19.8h, v25.16b
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v3.16b, v18.16b
    bif     v22.16b, v4.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_eo_135_chroma_end_r0:
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride

//--------------------------------middle rows--------------------------------
#if defined(__APPLE__)
    ldp     w7 , w8, [sp, #8]               //-- x7=start_x_r; x8=end_x_r
#else
    ldp     x7 , x8, [sp, #16]              //-- x7=start_x_r; x8=end_x_r
#endif
    sxtw    x7 , w7
    sxtw    x8 , w8

    sub     x9 , x8, x7
    and     x9 , x9, #31
    add     x12, x5, x9, lsl #3
    ld1     {v25.16b}, [x12]                //-- mask_r
    sxtl    v18.8h, v25.8b
    sxtl2   v19.8h, v25.16b

    sub     x10, x8, x9                     //-- end_x_r_16

    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    sub     x11, x6, #2                     //-- y = mb_height - 2
loop_y_eo_135_chroma_r:
    mov     x9, x7                          //-- x = start_x_r
loop_x_eo_135_chroma_r:
    add     x12, x0, x9
    sub     x13, x12, x2
    add     x14, x12, x2
    sub     x13, x13, #2
    add     x14, x14, #2

    ld1     {v2.8h, v3.8h}, [x12]       //-- load src[x] (save)
    ld1     {v4.8h, v5.8h}, [x13]       //-- load src[x-src_stride-2]
    ld1     {v6.8h, v7.8h}, [x14]       //-- load src[x+src_stride+2]

    xtn     v21.8b , v2.8h
    xtn2    v21.16b, v3.8h
    xtn     v20.8b , v4.8h
    xtn2    v20.16b, v5.8h
    xtn     v22.8b , v6.8h
    xtn2    v22.16b, v7.8h

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add     x12, x1, x9                 //-- dst+x
    cmp     x9, x10
    bge     maskmove_eo_135_chroma_r
    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    add     x9, x9, #32
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    cmp     x9, x8
    blt     loop_x_eo_135_chroma_r
    b       loop_x_eo_135_chroma_end_r
maskmove_eo_135_chroma_r:
    //--- maskmove
    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    bif     v21.16b, v3.16b, v18.16b
    bif     v22.16b, v4.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_eo_135_chroma_end_r:
    subs    x11, x11, #1                    //-- y--
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride
    bgt     loop_y_eo_135_chroma_r

//---------------------------------last row--------------------------------
#if defined(__APPLE__)
    ldp     w6, w7, [sp, #16]              //-- x6=start_x_rn; x7=end_x_rn
#else
    ldp     x6, x7, [sp, #32]              //-- x6=start_x_rn; x7=end_x_rn
#endif
    sxtw    x7, w7
    sxtw    x6, w6
    sub     x8 , x7, x6
    and     x8 , x8, #31
    sub     x10, x7, x8                     //-- x10=end_x_rn_16

    mov     x9 , x6                         //-- x = start_x_rn
loop_x_eo_135_chroma_rn:
    cmp     x9 , x7
    bge     loop_x_eo_135_chroma_end_rn
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    sub     x13, x13, #2
    add     x14, x14, #2

    ld1     {v2.8h, v3.8h}, [x12]       //-- load src[x] (save)
    ld1     {v4.8h, v5.8h}, [x13]       //-- load src[x-src_stride-2]
    ld1     {v6.8h, v7.8h}, [x14]       //-- load src[x+src_stride+2]

    xtn     v21.8b , v2.8h
    xtn2    v21.16b, v3.8h
    xtn     v20.8b , v4.8h
    xtn2    v20.16b, v5.8h
    xtn     v22.8b , v6.8h
    xtn2    v22.16b, v7.8h

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add     x12, x1, x9                 //-- dst+x
    cmp     x9, x10
    bge     maskmove_eo_135_chroma_rn
    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    add     x9, x9, #32
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.16b, v22.16b}, [x12]

    b       loop_x_eo_135_chroma_rn
maskmove_eo_135_chroma_rn:
    sub     x6, x7, x10
    add     x6, x5, x6, lsl #3              //-- offset = 16*rownum
    ld1     {v25.16b}, [x6]                 //-- load mask_rn
    ld1     {v3.8h, v4.8h}, [x12]
    sxtl    v18.8h, v25.8b
    sxtl2   v19.8h, v25.16b
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v3.16b, v18.16b
    bif     v22.16b, v4.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]
loop_x_eo_135_chroma_end_rn:
    ret


/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_45_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, start_x_r0->x7,
 *  end_x_r0->x8, start_x_r->x9, end_x_r->x10, start_x_rn->x11, end_x_rn->x12
 ************************************************************************************************************************************/
function uavs3e_sao_eo_45_arm64
#if defined(__APPLE__)
    ldp w8, w9, [sp]
#else
    ldp x8, x9, [sp]
#endif
    sxtw x8, w8                         // start_x_r0
    sxtw x9, w9                         // end_x_r0

    // get end_x_r0_16
    sub x11, x9, x8
    and x11, x11, #15
    sub x10, x9, x11                    // end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]                 // offset[0-3]
    ldr  w11, [x4, #16]                 // offset[4]
    movi v2.4s, #0
    mov  v2.s[0], w11
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h                   // offset[0-4]

    movi v1.16b, #2

    // ------- first row -------
    mov  x11, x8                         // x = start_x_r0

test_loop_x_eo_45_r0:

    cmp x11, x9
    bge test_loop_x_eo_45_end_r0
    add x12, x0, x11
    sub x13, x12, x2
    add x14, x12, x2
    add x13, x13, #1
    sub x14, x14, #1
    ld1 {v21.16b}, [x12]                // src[x]
    ld1 {v20.16b}, [x13]                // src[x-src_stride+1]
    ld1 {v22.16b}, [x14]                // src[x+src_stride-1]

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add x12, x1, x11
    cmp x11, x10
    bge test_maskmove_eo_45_r0
    st1 {v20.16b}, [x12]
    add x11, x11, #16
    b   test_loop_x_eo_45_r0

test_maskmove_eo_45_r0:
    sub x7, x9, x10
    add x7, x5, x7, lsl #4              // offset = 16*rownum
    ld1 {v30.4s}, [x7]                  // load mask_r0
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

test_loop_x_eo_45_end_r0:
    add x0, x0, x2                      // src+=src_stride
    add x1, x1, x3                      // dst+=dst_stride

    // ------- middle rows -------
    // get param
#if defined(__APPLE__)
    ldp w7, w8, [sp, #8]                // x7 start_x_r; x8 end_x_r
#else
    ldp x7, x8, [sp, #16]               // x7 start_x_r; x8 end_x_r
#endif
    sxtw x7, w7
    sxtw x8, w8

    sub x9, x8, x7
    and x9, x9, #15
    add x12, x5, x9, lsl #4
    ld1 {v30.4s}, [x12]                 // mask_r

    sub x10, x8, x9                     // end_x_r_16

    sub x11, x6, #2                     // y = mb_height - 2

test_loop_y_eo_45_r:

    mov x9, x7                          // x = start_x_r

test_loop_x_eo_45_r:

    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    add x13, x13, #1
    sub x14, x14, #1
    ld1 {v21.16b}, [x12]                // src[x]
    ld1 {v20.16b}, [x13]                // src[x-src_stride+1]
    ld1 {v22.16b}, [x14]                // src[x+src_stride-1]

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_45_r
    add x9, x9, #16
    st1 {v20.16b}, [x12]
    cmp x9, x8
    blt test_loop_x_eo_45_r
    b   test_loop_x_eo_45_end_r

test_maskmove_eo_45_r:
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

test_loop_x_eo_45_end_r:
    subs x11, x11, #1
    add x0, x0, x2                      // src+=src_stride
    add x1, x1, x3                      // dst+=dst_stride
    bgt test_loop_y_eo_45_r

    // ------- last row -------
#if defined(__APPLE__)
    ldp w6, w7, [sp, #16]               // $x6 start_x_rn; $x7 end_x_rn
#else
    ldp x6, x7, [sp, #32]               // $x6 start_x_rn; $x7 end_x_rn
#endif
    sxtw x6, w6
    sxtw x7, w7

    sub x8, x7, x6
    and x8, x8, #15
    sub x10, x7, x8                     // end_x_rn_16

    mov x9, x6                          // x = start_x_rn

test_loop_x_eo_45_rn:
    cmp x9, x7
    bge test_loop_x_eo_45_end_rn
    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    add x13, x13, #1
    sub x14, x14, #1
    ld1 {v21.16b}, [x12]                // src[x]
    ld1 {v20.16b}, [x13]                // src[x-src_stride+1]
    ld1 {v22.16b}, [x14]                // src[x+src_stride-1]

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b     //-- leftsign
    sub   v22.16b, v27.16b, v28.16b     //-- rightsign

    add     v20.16b, v22.16b, v20.16b   // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b             // src[x]

    add     v20.16b, v20.16b, v1.16b    // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b  // offset

    saddw   v23.8h, v23.8h, v22.8b      // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b     // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h             //-- results

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_45_rn
    st1 {v20.16b}, [x12]
    add x9, x9, #16
    b   test_loop_x_eo_45_rn

test_maskmove_eo_45_rn:
    sub x6, x7, x10
    add x6, x5, x6, lsl #4              // offset = 16*rownum
    ld1 {v30.4s}, [x6]                  // load mask_r0
    ld1 {v22.16b}, [x12]                // load 16 pixels from dst+x
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

test_loop_x_eo_45_end_rn:

ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_45_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, start_x_r0->x7,
 *  end_x_r0->x8, start_x_r->x9, end_x_r->x10, start_x_rn->x11, end_x_rn->x12
 ************************************************************************************************************************************/
function uavs3e_sao_eo_45_chroma_arm64
#if defined(__APPLE__)
    ldp     w8, w9, [sp]
#else
    ldp     x8, x9, [sp]
#endif
    sxtw    x8, w8                          // start_x_r0
    sxtw    x9, w9                          // end_x_r0

    sub     x11, x9, x8
    and     x11, x11, #31
    sub     x10, x9, x11                    //-- end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f);

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]                  //-- load offset[0-3]
    ldr     w11, [x4, #16]                  //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w11
    xtn     v0.8b, v0.8h                    //-- convert int32 to byte

    mov     w11, #0x00ff
    dup     v31.8h, w11

    movi    v1.16b, #2                      //-- constant(save)

//---------------------first row-------------------------
    mov     x11, x8                         //-- x = start_x_r0
loop_x_eo_45_chroma_r0:
    cmp     x11, x9
    bge     loop_x_eo_45_chroma_end_r0
    add     x12, x0, x11
    sub     x13, x12, x2
    add     x14, x12, x2
    add     x13, x13, #2
    sub     x14, x14, #2
    ld1     {v2.8h, v3.8h}, [x12]           //-- load src[x] (save)
    ld1     {v4.8h, v5.8h}, [x13]           //-- load src[x-src_stride+2]
    ld1     {v6.8h, v7.8h}, [x14]           //-- load src[x+src_stride-2]
    xtn     v21.8b , v2.8h
    xtn2    v21.16b, v3.8h
    xtn     v20.8b , v4.8h
    xtn2    v20.16b, v5.8h
    xtn     v22.8b , v6.8h
    xtn2    v22.16b, v7.8h

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b         //-- leftsign
    sub   v22.16b, v27.16b, v28.16b         //-- rightsign

    add     v20.16b, v22.16b, v20.16b       // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b                 // src[x]

    add     v20.16b, v20.16b, v1.16b        // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b      // offset

    saddw   v23.8h, v23.8h, v22.8b          // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b         // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h                 //-- results

    add     x12, x1, x11                    //-- dst+x
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b

    cmp     x11, x10
    bge     maskmove_eo_45_chroma_r0
    ld1     {v3.8h, v4.8h}, [x12]
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    add     x11, x11, #32
    b       loop_x_eo_45_chroma_r0
maskmove_eo_45_chroma_r0:
    sub     x7, x9, x10
    add     x7, x5, x7, lsl #3              //-- offset = 16*rownum
    ld1     {v25.8h}, [x7]                  //-- load mask_r0
    ld1     {v4.8h, v5.8h}, [x12]
    sxtl    v18.8h, v25.8b
    sxtl2   v19.8h, v25.16b
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v4.16b, v18.16b
    bif     v22.16b, v5.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_eo_45_chroma_end_r0:
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride

//--------------------------------middle rows--------------------------------
#if defined(__APPLE__)
    ldp     w7 , w8, [sp, #8]               //-- x7=start_x_r; x8=end_x_r
#else
    ldp     x7 , x8, [sp, #16]              //-- x7=start_x_r; x8=end_x_r
#endif
    sxtw    x7 , w7
    sxtw    x8 , w8

    sub     x9 , x8, x7
    and     x9 , x9, #31
    add     x12, x5, x9, lsl #3
    ld1     {v25.8h}, [x12]                 //-- mask_r

    sub     x10, x8, x9                     //-- x10=end_x_r_16

    sxtl    v18.8h, v25.8b
    sxtl2   v19.8h, v25.16b

    sub     x11, x6, #2                     //-- y = mb_height - 2
    
    and    v18.16b, v18.16b, v31.16b
    and    v19.16b, v19.16b, v31.16b
loop_y_eo_45_chroma_r:
    mov     x9, x7                          //-- x = start_x_r
loop_x_eo_45_chroma_r:
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    add     x13, x13, #2
    sub     x14, x14, #2
    ld1     {v2.8h, v3.8h}, [x12]           //-- load src[x] (save)
    ld1     {v4.8h, v5.8h}, [x13]           //-- load src[x-src_stride+2]
    ld1     {v6.8h, v7.8h}, [x14]           //-- load src[x+src_stride-2]
    xtn     v21.8b , v2.8h
    xtn2    v21.16b, v3.8h
    xtn     v20.8b , v4.8h
    xtn2    v20.16b, v5.8h
    xtn     v22.8b , v6.8h
    xtn2    v22.16b, v7.8h

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b         //-- leftsign
    sub   v22.16b, v27.16b, v28.16b         //-- rightsign

    add     v20.16b, v22.16b, v20.16b       // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b                 // src[x]

    add     v20.16b, v20.16b, v1.16b        // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b      // offset

    saddw   v23.8h, v23.8h, v22.8b          // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b         // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h                 //-- results

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    ld1     {v4.8h, v5.8h}, [x12]
    bge     maskmove_eo_45_chroma_r
    bif     v21.16b, v4.16b, v31.16b
    bif     v22.16b, v5.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    add     x9, x9, #32
    cmp     x9, x8
    blt     loop_x_eo_45_chroma_r
    b       loop_x_eo_45_chroma_end_r
maskmove_eo_45_chroma_r:
    //--- maskmove
    bif    v21.16b, v4.16b, v18.16b
    bif    v22.16b, v5.16b, v19.16b
    st1    {v21.8h, v22.8h}, [x12]

loop_x_eo_45_chroma_end_r:
    subs   x11, x11, #1                     //-- y--
    add    x0, x0, x2                       //-- src+=src_stride
    add    x1, x1, x3                       //-- dst+=dst_stride
    bgt    loop_y_eo_45_chroma_r

//---------------------------------last row--------------------------------
#if defined(__APPLE__)
    ldp     w6 , w7, [sp, #16]              //-- x6=start_x_rn; x7=end_x_rn
#else
    ldp     x6 , x7, [sp, #32]              //-- x6=start_x_rn; x7=end_x_rn
#endif
    sxtw    x7 , w7
    sxtw    x6 , w6

    sub     x8 , x7, x6
    and     x8 , x8, #31
    sub     x10, x7, x8                     //-- x10=end_x_rn_16

    mov     x9 , x6                         //-- x = start_x_rn
loop_x_eo_45_chroma_rn:
    cmp     x9 , x7
    bge     loop_x_eo_45_chroma_end_rn
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    add     x13, x13, #2
    sub     x14, x14, #2
    ld1     {v2.8h, v3.8h}, [x12]           //-- load src[x] (save)
    ld1     {v4.8h, v5.8h}, [x13]           //-- load src[x-src_stride+2]
    ld1     {v6.8h, v7.8h}, [x14]           //-- load src[x+src_stride-2]
    xtn     v21.8b , v2.8h
    xtn2    v21.16b, v3.8h
    xtn     v20.8b , v4.8h
    xtn2    v20.16b, v5.8h
    xtn     v22.8b , v6.8h
    xtn2    v22.16b, v7.8h

    // get leftsign & rightsign
    umin  v23.16b, v20.16b, v21.16b
    umin  v26.16b, v21.16b, v22.16b
    cmeq  v24.16b, v23.16b, v20.16b
    cmeq  v25.16b, v23.16b, v21.16b
    cmeq  v27.16b, v26.16b, v21.16b
    cmeq  v28.16b, v26.16b, v22.16b
    sub   v20.16b, v25.16b, v24.16b         //-- leftsign
    sub   v22.16b, v27.16b, v28.16b         //-- rightsign

    add     v20.16b, v22.16b, v20.16b       // edgetype

    uxtl    v23.8h, v21.8b
    uxtl2   v24.8h, v21.16b                 // src[x]

    add     v20.16b, v20.16b, v1.16b        // edgetype+2
    tbl     v22.16b, {v0.16b}, v20.16b      // offset

    saddw   v23.8h, v23.8h, v22.8b          // offset+src[x] low 8 samples
    saddw2  v24.8h, v24.8h, v22.16b         // offset+src[x] high 8 samples

    sqxtun  v20.8b, v23.8h
    sqxtun2 v20.16b, v24.8h                 //-- results

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    ld1     {v4.8h, v5.8h}, [x12]
    bge     maskmove_eo_45_chroma_rn
    bif     v21.16b, v4.16b, v31.16b
    bif     v22.16b, v5.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    add     x9, x9, #32
    b       loop_x_eo_45_chroma_rn
maskmove_eo_45_chroma_rn:
    sub     x6, x7 , x10
    add     x6, x5 , x6, lsl #3             //-- offset = 16*rownum
    ld1     {v25.8h}, [x6]                  //-- load mask_rn
    sxtl    v18.8h, v25.8b
    sxtl2   v19.8h, v25.16b
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v4.16b, v18.16b
    bif     v22.16b, v5.16b, v19.16b
    st1     {v21.16b, v22.16b}, [x12]
loop_x_eo_45_chroma_end_rn:
    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_bo_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int *bind_ids, int mb_width, int mb_height)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, bind_ids->x5, mb_width->x6, mb_height->x7
 ************************************************************************************************************************************/
function uavs3e_sao_bo_arm64

    ldr w9 , [x5]
    ldr w10, [x5, #4]
    ldr w11, [x5, #8]
    ldr w12, [x5, #12]

    ld1  {v18.4s}, [x4]             // load offsets

    dup v0.16b, w9
    dup v1.16b, w10
    dup v2.16b, w11
    dup v3.16b, w12

    mov w9, v18.s[0]
    mov w10, v18.s[1]
    mov w11, v18.s[2]
    mov w12, v18.s[3]

    and w13, w6, #31

    dup v4.16b, w9                  // offset[0]
    dup v5.16b, w10                 // offset[1]
    dup v6.16b, w11                 // offset[2]
    dup v7.16b, w12                 // offset[3]

    cmp  w13, #0                    // mb_width % 32 == 0
    beq  sao_bo_w32x_y

    sub  x10, x6, #4                // mb_width - 4
    and  x11, x10, #15              // (mb_width - 4) % 16
    mov  w12, #0
    mov  w9, #-1
    
    sub  x10, x6, #15               // mb_width - 15
    cmp  w11, #0
    beq  set_mask_4_bo
    movi v30.2d, #-1
    mov  v30.s[3], w12              // mask="-1 repeat 12, 0, 0, 0, 0"
    b    loop_y_bo

set_mask_4_bo:

    movi v30.2d, #0
    mov  v30.s[0], w9               // mask="-1, -1, -1, -1, 0 repeat 12"

loop_y_bo:

    mov x9, #0                      // x = 0
loop_x_bo:
    add  x12, x0, x9
    ld1  {v23.16b}, [x12]           // src[x]
    ushr v18.16b, v23.16b, #3

    cmeq v19.16b, v18.16b, v0.16b
    cmeq v20.16b, v18.16b, v1.16b
    cmeq v21.16b, v18.16b, v2.16b
    cmeq v22.16b, v18.16b, v3.16b

    and  v19.16b, v19.16b, v4.16b
    and  v20.16b, v20.16b, v5.16b
    and  v21.16b, v21.16b, v6.16b
    and  v22.16b, v22.16b, v7.16b

    orr  v19.16b, v19.16b, v20.16b
    orr  v21.16b, v21.16b, v22.16b

    uxtl  v22.8h, v23.8b
    uxtl2 v23.8h, v23.16b

    orr  v19.16b, v19.16b, v21.16b  // get offsets

    saddw  v20.8h, v22.8h, v19.8b
    saddw2 v21.8h, v23.8h, v19.16b

    sqxtun  v20.8b, v20.8h
    sqxtun2 v20.16b, v21.8h         // get results

    add x12, x1, x9                 // dst+x
    cmp x9, x10
    bge maskmove_bo
    add x9, x9, #16
    st1 {v20.16b}, [x12]
    cmp x9, x6
    blt loop_x_bo
    b   loop_x_bo_end

maskmove_bo:
    ld1 {v22.16b}, [x12]
    bif v20.16b, v22.16b, v30.16b
    st1 {v20.16b}, [x12]

loop_x_bo_end:
    subs w7, w7, #1
    add x0, x0, x2
    add x1, x1, x3
    bgt loop_y_bo
    b   sao_bo_end

sao_bo_w32x_y:
    mov x9, x6                      // x = 0
    mov x11, x0
    mov x12, x1
sao_bo_w32x_x:
    ld1  {v16.16b, v17.16b}, [x11]  // src[x]
    ushr v18.16b, v16.16b, #3
    ushr v23.16b, v17.16b, #3

    cmeq v19.16b, v18.16b, v0.16b
    cmeq v20.16b, v18.16b, v1.16b
    cmeq v21.16b, v18.16b, v2.16b
    cmeq v22.16b, v18.16b, v3.16b

    cmeq v24.16b, v23.16b, v0.16b
    cmeq v25.16b, v23.16b, v1.16b
    cmeq v26.16b, v23.16b, v2.16b
    cmeq v27.16b, v23.16b, v3.16b

    and  v19.16b, v19.16b, v4.16b
    and  v20.16b, v20.16b, v5.16b
    and  v21.16b, v21.16b, v6.16b
    and  v22.16b, v22.16b, v7.16b
    
    and  v24.16b, v24.16b, v4.16b
    and  v25.16b, v25.16b, v5.16b
    and  v26.16b, v26.16b, v6.16b
    and  v27.16b, v27.16b, v7.16b

    orr  v19.16b, v19.16b, v20.16b
    orr  v21.16b, v21.16b, v22.16b
    orr  v24.16b, v24.16b, v25.16b
    orr  v26.16b, v26.16b, v27.16b

    uxtl  v22.8h, v16.8b
    uxtl2 v23.8h, v16.16b
    uxtl  v28.8h, v17.8b
    uxtl2 v29.8h, v17.16b

    orr  v19.16b, v19.16b, v21.16b  // get offsets
    orr  v24.16b, v24.16b, v26.16b

    saddw  v20.8h, v22.8h, v19.8b
    saddw2 v21.8h, v23.8h, v19.16b
    saddw  v22.8h, v28.8h, v24.8b
    saddw2 v23.8h, v29.8h, v24.16b

    sqxtun  v18.8b , v20.8h
    sqxtun2 v18.16b, v21.8h         // get results
    sqxtun  v19.8b , v22.8h
    sqxtun2 v19.16b, v23.8h

    subs x9, x9, #32
    add  x11, x11, #32
    st1 {v18.16b, v19.16b}, [x12]
    add  x12, x12, #32
    bgt sao_bo_w32x_x

    subs w7, w7, #1
    add x0, x0, x2
    add x1, x1, x3
    bgt sao_bo_w32x_y

sao_bo_end:
    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_bo_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int *bind_ids, int mb_width, int mb_height)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, bind_ids->x5, mb_width->x6,
 *  mb_height->x7 ************************************************************************************************************************************/
function uavs3e_sao_bo_chroma_arm64
    ldr w9 , [x5]
    ldr w10, [x5, #4]
    ldr w11, [x5, #8]
    ldr w12, [x5, #12]

    ld1 {v19.4s}, [x4]

    dup v0.16b, w9
    dup v1.16b, w10
    dup v2.16b, w11
    dup v3.16b, w12

    mov     w9, #0x00ff
    dup     v18.8h, w9                      //-- mask_uv

    sub     x11, x6, #8                     //-- mb_width - 8

    xtn     v19.4h, v19.4s
    xtn     v19.8b, v19.8h
    dup     v4.16b, v19.b[0]                //-- offset[0]
    dup     v5.16b, v19.b[1]                //-- offset[1]
    dup     v6.16b, v19.b[2]                //-- offset[2]
    dup     v7.16b, v19.b[3]                //-- offset[3]
    and     w11, w11, #31

    sub     x10, x6, #31                    //-- mb_width - 31
    cmp     w11, #0                         //-- (mb_width - 8)%32 == 0 ?
    movi    v24.16b, #0xff
    movi    v25.4s, #0
    beq     set_mask_width_8_bo_chroma
    mov     v25.d[0], v24.d[0]              //-- mask="-1 repeat 12, 0, 0, 0, 0"
    b       loop_y_bo_chroma
set_mask_width_8_bo_chroma:
    mov     v24.d[1], v24.d[0]              //-- mask="-1, -1, -1, -1, 0 repeat 12"

loop_y_bo_chroma:
    and     v24.16b, v24.16b, v18.16b
    and     v25.16b, v25.16b, v18.16b

    mov     x9, #0                          //-- x = 0
loop_x_bo_chroma:
    add     x12, x0 , x9
    ld1     {v19.8h, v20.8h}, [x12]         //-- load src[x]    (save)
    xtn     v23.8b, v19.8h
    xtn2    v23.16b, v20.8h
    ushr    v22.16b, v23.16b, #3

    cmeq    v19.16b , v22.16b, v0.16b
    cmeq    v20.16b, v22.16b, v1.16b
    cmeq    v21.16b, v22.16b, v2.16b
    cmeq    v22.16b, v22.16b, v3.16b

    and     v19.16b , v19.16b , v4.16b
    and     v20.16b, v20.16b, v5.16b
    and     v21.16b, v21.16b, v6.16b
    and     v22.16b, v22.16b, v7.16b

    orr     v19.16b, v19.16b , v20.16b
    orr     v21.16b, v21.16b, v22.16b
    orr     v19.16b, v19.16b , v21.16b      //-- get offsets

    uxtl    v22.8h, v23.8b                  //-- src[x] low 8 samples
    uxtl2   v23.8h, v23.16b

    saddw  v20.8h, v22.8h, v19.8b
    saddw2 v21.8h, v23.8h, v19.16b

    sqxtun  v20.8b, v20.8h
    sqxtun2 v20.16b, v21.8h                 //-- results

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    uxtl    v21.8h, v20.8b
    uxtl2   v22.8h, v20.16b
    ld1     {v19.8h, v20.8h}, [x12]
    bge     maskmove_bo_chroma
    bif     v21.16b, v19.16b, v18.16b
    bif     v22.16b, v20.16b, v18.16b
    add     x9, x9, #32
    st1     {v21.8h, v22.8h}, [x12]
    cmp     x9, x6
    blt     loop_x_bo_chroma
    b       loop_x_bo_chroma_end
maskmove_bo_chroma:
    //--- maskmove
    bif     v21.16b, v19.16b, v24.16b
    bif     v22.16b, v20.16b, v25.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_bo_chroma_end:
    subs    x7, x7, #1                      //-- y--
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride
    bgt     loop_y_bo_chroma

    ret
#else // COMPILE_10BIT == 1

/***********************************************************************************************************************************
*  void uavs3e_sao_eo_0_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_x, int end_x, int mb_height, pel* mask, int bit_depth)
*  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_x->x5, end_x->x6, mb_height->x7, mask->x8, bit_depth->w9
************************************************************************************************************************************/
function uavs3e_sao_eo_0_arm64
    ldr x8, [sp]                // mask
    ldr w9, [sp, #8]            // bit_depth

    // ------- load mask -------
    sub x10, x6, x5
    and x11, x10, #15
    add x12, x8, x11, lsl #5
    ld1 {v30.8h, v31.8h}, [x12] // mask + ((end_x - start_x) & 0x0f)*16*sizeof(pel)

    sub w10, w6, w11            // end_x_16 = end_x - ((end_x - start_x) & 0x0f)

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]         // offset[0-3]
    ldr  w11, [x4, #16]         // offset[4]
    mov  x8, #0
    movi v2.4s, #0
    mov  v2.s[0], w11
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h           // v0.8b: offset[0-4]

    mov w11, #1
    movi v1.16b, #2
    lsl w11, w11, w9

    lsl x2, x2, #1
    lsl x3, x3, #1
    lsl x5, x5, #1
    lsl x6, x6, #1
    lsl x10, x10, #1

    sub w11, w11, #1            // max_pel

    movi v6.8h, #0              // min_pel
    dup  v7.8h, w11             // max_pel

loop_y_eo_0:

    mov x9, x5                  // x = start_x
loop_x_eo_0:

    add  x12, x0, x9
    sub  x13, x12, #2
    add  x14, x12, #2

    ld1  {v16.8h, v17.8h}, [x13]        // src[x-1]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+1]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v22.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v22.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v22.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x9
    cmp x9, x10
    beq maskmove_eo_0
    add x9, x9, #32
    st1 {v23.8h, v24.8h}, [x12]
    cmp x9, x6
    blt loop_x_eo_0
    b   loop_x_eo_0_end

maskmove_eo_0:
    // maskmove
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

loop_x_eo_0_end:
    subs w7, w7, #1
    add x0, x0, x2
    add x1, x1, x3
    bgt loop_y_eo_0

    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_0_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_x, int end_x, int mb_height, char_t* mask, int bit_depth)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_x->x5, end_x->x6, mb_height->x7,
 *  mask->x8
 ************************************************************************************************************************************/
function uavs3e_sao_eo_0_chroma_arm64
    ldr     x8, [sp]                        // mask
    ldr     w9, [sp, #8]                    // bit_depth
    mov     w13, #1

    sub     x10, x6, x5
    and     x11, x10, #15
    add     x12, x8, x11, lsl#4
    ld1     {v25.8h}, [x12]                 //-- load mask: mask + (((end_x - start_x) & 0x07))*16*sizeof(pel)
    lsl     w13, w13, w9

    sxtl    v16.4s, v25.4h
    sxtl2   v17.4s, v25.8h

    mov     w8, #0x0000ffff
    sub     x10, x6, x11                    //-- x10 = end_x_16 = end_x - ((end_x - start_x) & 0x0f)
    sub     w13, w13, #1
    dup     v31.4s, w8                      //-- mask_uv: for uv interlace

    and     v16.16b, v16.16b, v31.16b       //-- mask for last cols
    and     v17.16b, v17.16b, v31.16b
    movi    v18.8h, #0                      //-- min_pel
    dup     v19.8h, w13                     //-- max_pel

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]                  //-- load offset[0-3]
    ldr     w9, [x4, #16]                   //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w9
    xtn     v0.8b, v0.8h                    //-- convert int32 to byte

    lsl     x2, x2, #1
    lsl     x3, x3, #1
    lsl     x5, x5, #1
    lsl     x6, x6, #1
    lsl     x10, x10, #1

    movi    v1.16b , #2                     //-- constant(save)

loop_y_eo_0_chroma:
    mov     x9, x5                          //-- x = start_x
loop_x_eo_0_chroma:
    add     x12, x0 , x9
    sub     x13, x12, #4
    add     x14, x12, #4
    ld1     {v6.4s, v7.4s}, [x13]           //-- load src[x-2]
    ld1     {v4.4s, v5.4s}, [x12]           //-- load src[x]
    ld1     {v2.4s, v3.4s}, [x14]           //-- load src[x+2]
    xtn     v20.4h, v6.4s                   //-- delete the other chroma
    xtn2    v20.8h, v7.4s                   //-- src[x-2]
    xtn     v21.4h, v4.4s
    xtn2    v21.8h, v5.4s                   //-- src[x]
    xtn     v22.4h, v2.4s
    xtn2    v22.8h, v3.4s

    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h          //-- leftsign
    sub     v22.8h, v27.8h, v28.8h          //-- rightsign

    add     v20.8h, v22.8h, v20.8h          //-- edgetype
    xtn     v20.8b, v20.8h

    add     v20.8b, v20.8b, v1.8b           //-- generate look-up indexs
    tbl     v22.8b, {v0.16b}, v20.8b        //-- get offset

    add     x12, x1, x9                     //-- dst + x

    saddw   v23.8h, v21.8h, v22.8b

    ld1     {v4.8h, v5.8h}, [x12]           //-- load 16 pixels from dst+x
    smax    v23.8h, v23.8h, v18.8h
    smin    v23.8h, v23.8h, v19.8h

    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h

    cmp     x9, x10
    beq     maskmove_eo_0_chroma
    bif     v21.16b, v4.16b, v31.16b
    bif     v22.16b, v5.16b, v31.16b
    add     x9, x9, #32
    st1     {v21.8h, v22.8h}, [x12]
    cmp     x9, x6
    blt     loop_x_eo_0_chroma
    b       loop_x_eo_0_chroma_end

maskmove_eo_0_chroma:
    //--- maskmove
    bif    v21.16b, v4.16b, v16.16b
    bif    v22.16b, v5.16b, v17.16b
    st1    {v21.8h, v22.8h}, [x12]

loop_x_eo_0_chroma_end:
    subs   x7, x7, #1                       //-- y++
    add    x0, x0, x2                       //-- src += src_stride
    add    x1, x1, x3                       //-- dst += dst_stride
    bgt    loop_y_eo_0_chroma

    ret


/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_90_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_y, int end_y, int mb_width, int bit_depth);
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_y->x5, end_y->x6, mb_width->x7, bit_depth->w8
 ************************************************************************************************************************************/
function uavs3e_sao_eo_90_arm64
    ldr w8, [sp]

    mov x9, #1

    lsl x2, x2, #1
    lsl x3, x3, #1
    lsl x7, x7, #1
    lsl w9, w9, w8

    mul x10, x2, x5
    mul x11, x3, x5
    add x0, x0, x10
    add x1, x1, x11             // dst += start_y * dst_stride

    sub x10, x7, #30            // end_x_16 = (mb_width - 15)*sizeof(pel)
    sub w11, w9, #1

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]         // offset[0-3]
    ldr  w9, [x4, #16]          // offset[4]
    mov  x8, #0
    movi v2.4s, #0
    mov  v2.s[0], w9
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h           // offset[0-4]

    movi v1.16b, #2
    movi v6.8h, #0
    dup  v7.8h, w11

    sub  w8, w6, w5             // y = end_y - start_y

    mov  x9, #-1
    cmp  w7, #8
    beq  set_mask_width_4
    movi v30.16b, #255
    movi v31.4s, #0
    mov  v31.d[0], x9           // v31.8h: "-1, -1, -1, -1, 0, 0, 0, 0"
    b    loop_y_eo_90

set_mask_width_4:
    movi v30.4s, #0
    mov  v30.d[0], x9           // v30.8h: "-1, -1, -1, -1, 0, 0, 0, 0"
    movi v31.4s, #0
loop_y_eo_90:

    mov  x9, #0                 // x = 0

loop_x_eo_90:
    add  x12, x0, x9            // x12
    sub  x13, x12, x2
    add  x14, x12, x2
    ld1  {v16.8h, v17.8h}, [x13]        // src[x-src_stride]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+src_stride]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v25.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v25.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v25.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x9
    cmp x9, x10
    bge maskmove_eo_90
    st1 {v23.8h, v24.8h}, [x12]
    add x9, x9, #32
    cmp x9, x7
    blt loop_x_eo_90
    b   loop_x_eo_90_end

maskmove_eo_90:
    // maskmove
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

loop_x_eo_90_end:
    subs w8, w8, #1
    add x0, x0, x2
    add x1, x1, x3
    bgt loop_y_eo_90

ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_90_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int start_y, int end_y, int mb_width, int bit_depth);
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, start_y->x5, end_y->x6, mb_width->x7
 ************************************************************************************************************************************/
function uavs3e_sao_eo_90_chroma_arm64
    ldr     w11, [sp]                   //-- bit_depth

    lsl     w2, w2, #1
    lsl     w3, w3, #1
    lsl     w7, w7, #1
    mov     w13, #1

    mul     x8, x2, x5
    mul     x9, x3, x5
    add     x0, x0, x8                  //-- src -= start_y*src_stride
    add     x1, x1, x9                  //-- dst -= start_y*dst_stride

    lsl     w13, w13, w11

    sub     x10, x7, #16                //-- mb_width - 8
    sub     w13, w13, #1

    mov     w8, #0x0000ffff
    dup     v31.4s, w8

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]              //-- load offset[0-3]
    ldr     w9 , [x4, #16]              //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w9
    xtn     v0.8b, v0.8h                //-- convert int32 to byte

    movi    v1.16b, #2                  //-- constant(save)
    movi    v29.8h, #0                  //-- min_pel
    dup     v30.8h, w13                 //-- max_pel

    sub     w8, w6, w5                  //-- y = start_y

loop_y_eo_90_chroma:
    mov     x9, #0                      //-- x = 0
loop_x_eo_90_chroma:
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    ld1     {v2.4s, v3.4s}, [x12]       //-- load src[x](save)
    ld1     {v4.4s, v5.4s}, [x13]       //-- load src[x - src_stride]
    ld1     {v6.4s, v7.4s}, [x14]       //-- load src[x + src_stride]
    xtn     v21.4h, v2.4s
    xtn2    v21.8h, v3.4s
    xtn     v20.4h, v4.4s
    xtn2    v20.8h, v5.4s
    xtn     v22.4h, v6.4s
    xtn2    v22.8h, v7.4s

    // get leftsign & rightsign
    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h      //-- leftsign
    sub     v22.8h, v27.8h, v28.8h      //-- rightsign

    add     v20.8h, v22.8h, v20.8h      // edgetype

    xtn     v20.8b, v20.8h
    add     v20.8b, v20.8b, v1.8b       // edgetype+2
    tbl     v22.8b, {v0.16b}, v20.8b    // offset

    saddw   v23.8h, v21.8h, v22.8b      // offset+src[x]

    smax    v23.8h, v23.8h, v29.8h
    smin    v23.8h, v23.8h, v30.8h

    add     x12, x1, x9                 //-- dst+x
    cmp     x9, x10
    bge     maskmove_eo_90_chroma
    ld1     {v2.8h, v3.8h}, [x12]
    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h
    add     x9, x9, #32
    bif     v21.16b, v2.16b, v31.16b
    bif     v22.16b, v3.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    cmp     x9, x7
    blt     loop_x_eo_90_chroma
    b       loop_x_eo_90_chroma_end
maskmove_eo_90_chroma:
    //--- maskmove
    ld1     {v2.8h}, [x12]
    uxtl    v21.4s, v23.4h
    bif     v21.16b, v2.16b, v31.16b
    st1     {v21.8h}, [x12]

loop_x_eo_90_chroma_end:
    subs    w8, w8, #1                  //-- y++
    add     x0, x0, x2                  //-- src+=src_stride
    add     x1, x1, x3                  //-- dst+=dst_stride
    bgt     loop_y_eo_90_chroma

    ret


/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_135_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int bit_depth, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, bit_depth->x7
 ************************************************************************************************************************************/
function uavs3e_sao_eo_135_arm64
#if defined(__APPLE__)
    ldp w8, w9, [sp]
#else
    ldp x8, x9, [sp]                // start_x_r0 and end_x_r0
#endif

    sxtw x8, w8                     // start_x_r0
    sxtw x9, w9                     // end_x_r0

    // get end_x_r0_16
    sub x11, x9, x8
    and x11, x11, #15
    sub x10, x9, x11                // end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f

    mov x12, #1

    lsl x2, x2, #1
    lsl x3, x3, #1
    lsl x8, x8, #1
    lsl x9, x9, #1
    lsl x10, x10, #1
    lsl w12, w12, w7

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]             // offset[0-3]
    ldr  w11, [x4, #16]             // offset[4]
    movi v2.4s, #0
    mov  v2.s[0], w11
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h               // offset[0-4]

    sub w12, w12, #1
    movi v1.16b, #2
    movi v6.8h, #0                  // min_pel
    dup  v7.8h, w12                 // max_pel

    // ------- first row -------
    mov x11, x8                     // x = start_x_r0

test_loop_x_eo_135_r0:

    cmp x11, x9
    bge test_loop_x_eo_135_end_r0

    add x12, x0, x11
    sub x13, x12, x2
    add x14, x12, x2
    sub x13, x13, #2
    add x14, x14, #2
    ld1  {v16.8h, v17.8h}, [x13]        // src[x-src_stride-1]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+src_stride+1]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v25.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v25.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v25.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x11
    cmp x11, x10
    bge test_maskmove_eo_135_r0
    st1 {v23.8h, v24.8h}, [x12]
    add x11, x11, #32
    b   test_loop_x_eo_135_r0

test_maskmove_eo_135_r0:
    sub x7, x9, x10
    add x7, x5, x7, lsl #4              // offset = 16*rownum
    ld1 {v30.4s, v31.4s}, [x7]          // load mask_r0
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

test_loop_x_eo_135_end_r0:
    add x0, x0, x2                      // src+=src_stride
    add x1, x1, x3                      // dst+=dst_stride

    // ------- middle rows -------
    // get param
#if defined(__APPLE__)
    ldp w7, w8, [sp, #8]
#else
    ldp x7, x8, [sp, #16]
#endif
    sxtw x7, w7                         // x7 start_x_r
    sxtw x8, w8                         // x8 end_x_r

    sub x9, x8, x7
    and x9, x9, #15
    add x12, x5, x9, lsl #5
    ld1 {v30.4s, v31.4s}, [x12]         // mask_r

    sub x10, x8, x9                     // end_x_r_16
    lsl x7, x7, #1
    lsl x8, x8, #1
    lsl x10, x10, #1

    sub x11, x6, #2                     // y = mb_height - 2

test_loop_y_eo_135_r:

    mov x9, x7                          // x = start_x_r

test_loop_x_eo_135_r:
    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    sub x13, x13, #2
    add x14, x14, #2
    ld1  {v16.8h, v17.8h}, [x13]        // src[x-src_stride-1]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+src_stride+1]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v25.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v25.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v25.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_135_r
    add x9, x9, #32
    st1 {v23.8h, v24.8h}, [x12]
    cmp x9, x8
    blt test_loop_x_eo_135_r
    b   test_loop_x_eo_135_end_r

test_maskmove_eo_135_r:
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

test_loop_x_eo_135_end_r:
    subs x11, x11, #1
    add x0, x0, x2                      // src += src_stride
    add x1, x1, x3                      // dst += dst_stride
    bgt test_loop_y_eo_135_r

// ------- last row -------
#if defined(__APPLE__)
    ldp w6, w7, [sp, #16]
#else
    ldp x6, x7, [sp, #32]
#endif
    sxtw x6, w6                         // start_x_rn
    sxtw x7, w7                         // end_x_rn

    sub x8, x7, x6
    and x8, x8, #15
    sub x10, x7, x8                     // end_x_rn_16

    lsl x6, x6, #1
    lsl x7, x7, #1
    lsl x10, x10, #1

    mov x9, x6                          // x = start_x_rn

test_loop_x_eo_135_rn:
    cmp x9, x7
    bge test_loop_x_eo_135_end_rn

    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    sub x13, x13, #2
    add x14, x14, #2
    ld1  {v16.8h, v17.8h}, [x13]        // src[x-src_stride-1]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+src_stride+1]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v25.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v25.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v25.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_135_rn
    st1 {v23.8h, v24.8h}, [x12]
    add x9, x9, #32
    b   test_loop_x_eo_135_rn

test_maskmove_eo_135_rn:
    sub x6, x7, x10
    add x6, x5, x6, lsl #4              // offset = 16*rownum
    ld1 {v30.4s, v31.4s}, [x6]          // load mask_r0
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

test_loop_x_eo_135_end_rn:

    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_135_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int bit_depth, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, bit_depth->x7
 ************************************************************************************************************************************/
function uavs3e_sao_eo_135_chroma_arm64
#if defined(__APPLE__)
    ldp w8, w9, [sp]
#else
    ldp x8, x9, [sp]                        // start_x_r0 and end_x_r0
#endif

    mov     w13, #1
    sxtw    x8, w8                          // start_x_r0
    sxtw    x9, w9                          // end_x_r0

    lsl     w13, w13, w7
    sub     x11, x9, x8
    and     x11, x11, #15
    sub     x10, x9, x11                    //-- end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f);
    sub     w13, w13, #1

    lsl     x2, x2, #1
    lsl     x3, x3, #1
    lsl     x8, x8, #1
    lsl     x9, x9, #1
    lsl     x10, x10, #1

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]                  //-- load offset[0-3]
    ldr     w11, [x4, #16]                  //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w11
    xtn     v0.8b, v0.8h                    //-- convert int32 to byte

    mov     w12, #0x0000ffff
    movi    v29.8h, #0
    dup     v31.4s, w12
    dup     v30.8h, w13
    movi    v1.16b , #2                     //-- constant(save)

//---------------------first row-------------------------
    mov     x11, x8                         //-- x = start_x_r0
loop_x_eo_135_chroma_r0:
    cmp     x11, x9
    bge     loop_x_eo_135_chroma_end_r0
    add     x12, x0, x11
    sub     x13, x12, x2
    add     x14, x12, x2
    sub     x13, x13, #4
    add     x14, x14, #4

    ld1     {v2.4s, v3.4s}, [x12]           //-- load src[x](save)
    ld1     {v4.4s, v5.4s}, [x13]           //-- load src[x - src_stride - 2]
    ld1     {v6.4s, v7.4s}, [x14]           //-- load src[x + src_stride + 2]
    xtn     v21.4h, v2.4s
    xtn2    v21.8h, v3.4s
    xtn     v20.4h, v4.4s
    xtn2    v20.8h, v5.4s
    xtn     v22.4h, v6.4s
    xtn2    v22.8h, v7.4s

    // get leftsign & rightsign
    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h          //-- leftsign
    sub     v22.8h, v27.8h, v28.8h          //-- rightsign

    add     v20.8h, v22.8h, v20.8h          // edgetype

    xtn     v20.8b, v20.8h
    add     v20.8b, v20.8b, v1.8b           // edgetype+2
    tbl     v22.8b, {v0.16b}, v20.8b        // offset

    saddw   v23.8h, v21.8h, v22.8b          // offset+src[x]

    smax    v23.8h, v23.8h, v29.8h
    smin    v23.8h, v23.8h, v30.8h

    add     x12, x1, x11                    //-- dst+x
    cmp     x11, x10
    bge     maskmove_eo_135_chroma_r0

    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    add     x11, x11, #32
    b       loop_x_eo_135_chroma_r0
maskmove_eo_135_chroma_r0:
    sub     x7, x9, x10
    add     x7, x5, x7, lsl #3              //-- offset = 16*rowid*sizeof(pel)
    ld1     {v25.8h}, [x7]                  //-- load mask_r0
    ld1     {v3.8h, v4.8h}, [x12]
    sxtl    v18.4s, v25.4h
    sxtl2   v19.4s, v25.8h
    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v3.16b, v18.16b
    bif     v22.16b, v4.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_eo_135_chroma_end_r0:
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride

//--------------------------------middle rows--------------------------------
#if defined(__APPLE__)
    ldp     w7 , w8, [sp, #8]               //-- x7=start_x_r; x8=end_x_r
#else
    ldp     x7 , x8, [sp, #16]              //-- x7=start_x_r; x8=end_x_r
#endif
    sxtw    x7 , w7
    sxtw    x8 , w8

    sub     x9 , x8, x7
    and     x9 , x9, #15
    add     x12, x5, x9, lsl #4
    ld1     {v25.8h}, [x12]                 //-- mask_r
    sxtl    v18.4s, v25.4h
    sxtl2   v19.4s, v25.8h

    sub     x10, x8, x9                     //-- end_x_r_16

    lsl     x7, x7, #1
    lsl     x8, x8, #1
    lsl     x10, x10, #1

    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b

    sub     x11, x6, #2                     //-- y = mb_height - 2
loop_y_eo_135_chroma_r:
    mov     x9, x7                          //-- x = start_x_r
loop_x_eo_135_chroma_r:
    add     x12, x0, x9
    sub     x13, x12, x2
    add     x14, x12, x2
    sub     x13, x13, #4
    add     x14, x14, #4

    ld1     {v2.4s, v3.4s}, [x12]           //-- load src[x](save)
    ld1     {v4.4s, v5.4s}, [x13]           //-- load src[x - src_stride - 2]
    ld1     {v6.4s, v7.4s}, [x14]           //-- load src[x + src_stride + 2]
    xtn     v21.4h, v2.4s
    xtn2    v21.8h, v3.4s
    xtn     v20.4h, v4.4s
    xtn2    v20.8h, v5.4s
    xtn     v22.4h, v6.4s
    xtn2    v22.8h, v7.4s

    // get leftsign & rightsign
    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h          //-- leftsign
    sub     v22.8h, v27.8h, v28.8h          //-- rightsign

    add     v20.8h, v22.8h, v20.8h          // edgetype

    xtn     v20.8b, v20.8h
    add     v20.8b, v20.8b, v1.8b           // edgetype+2
    tbl     v22.8b, {v0.16b}, v20.8b        // offset

    saddw   v23.8h, v21.8h, v22.8b          // offset+src[x]

    smax    v23.8h, v23.8h, v29.8h
    smin    v23.8h, v23.8h, v30.8h

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    bge     maskmove_eo_135_chroma_r
    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h
    add     x9, x9, #32
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    cmp     x9, x8
    blt     loop_x_eo_135_chroma_r
    b       loop_x_eo_135_chroma_end_r
maskmove_eo_135_chroma_r:
    //--- maskmove
    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h
    bif     v21.16b, v3.16b, v18.16b
    bif     v22.16b, v4.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_eo_135_chroma_end_r:
    subs    x11, x11, #1                    //-- y++
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride
    bgt     loop_y_eo_135_chroma_r

//---------------------------------last row--------------------------------
#if defined(__APPLE__)
    ldp     w6, w7, [sp, #16]              //-- x6=start_x_rn; x7=end_x_rn
#else
    ldp     x6, x7, [sp, #32]              //-- x6=start_x_rn; x7=end_x_rn
#endif
    sxtw    x7, w7
    sxtw    x6, w6
    sub     x8 , x7, x6
    and     x8 , x8, #15
    sub     x10, x7, x8                     //-- end_x_rn_16

    lsl     x7, x7, #1
    lsl     x6, x6, #1
    lsl     x10, x10, #1

    mov     x9 , x6                         //-- x = start_x_rn
loop_x_eo_135_chroma_rn:
    cmp     x9 , x7
    bge     loop_x_eo_135_chroma_end_rn
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    sub     x13, x13, #4
    add     x14, x14, #4

    ld1     {v2.4s, v3.4s}, [x12]           //-- load src[x](save)
    ld1     {v4.4s, v5.4s}, [x13]           //-- load src[x - src_stride - 2]
    ld1     {v6.4s, v7.4s}, [x14]           //-- load src[x + src_stride + 2]
    xtn     v21.4h, v2.4s
    xtn2    v21.8h, v3.4s
    xtn     v20.4h, v4.4s
    xtn2    v20.8h, v5.4s
    xtn     v22.4h, v6.4s
    xtn2    v22.8h, v7.4s

    // get leftsign & rightsign
    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h          //-- leftsign
    sub     v22.8h, v27.8h, v28.8h          //-- rightsign

    add     v20.8h, v22.8h, v20.8h          // edgetype

    xtn     v20.8b, v20.8h
    add     v20.8b, v20.8b, v1.8b           // edgetype+2
    tbl     v22.8b, {v0.16b}, v20.8b        // offset

    saddw   v23.8h, v21.8h, v22.8b          // offset+src[x]

    smax    v23.8h, v23.8h, v29.8h
    smin    v23.8h, v23.8h, v30.8h

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    bge     maskmove_eo_135_chroma_rn
    ld1     {v3.8h, v4.8h}, [x12]
    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h
    add     x9, x9, #32
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.16b, v22.16b}, [x12]

    b       loop_x_eo_135_chroma_rn
maskmove_eo_135_chroma_rn:
    sub     x6, x7, x10
    add     x6, x5, x6, lsl #3              //-- offset = 16*rownum
    ld1     {v25.8h}, [x6]                  //-- load mask_rn
    ld1     {v3.8h, v4.8h}, [x12]
    sxtl    v18.4s, v25.4h
    sxtl2   v19.4s, v25.8h
    uxtl    v21.4s, v23.4h
    uxtl2   v22.4s, v23.8h
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v3.16b, v18.16b
    bif     v22.16b, v4.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]
loop_x_eo_135_chroma_end_rn:
    ret


/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_45_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int bit_depth, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, bit_depth->x7
 ************************************************************************************************************************************/
function uavs3e_sao_eo_45_arm64
#if defined(__APPLE__)
    ldp w8, w9, [sp]
#else
    ldp x8, x9, [sp]                // start_x_r0 and end_x_r0
#endif

    mov w12, #1

    sxtw x8, w8                     // start_x_r0
    sxtw x9, w9                     // end_x_r0

    // get end_x_r0_16
    sub x11, x9, x8
    and x11, x11, #15
    sub x10, x9, x11                // end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f

    lsl w12, w12, w7
    lsl x2, x2, #1
    lsl x3, x3, #1
    lsl x8, x8, #1
    lsl x9, x9, #1
    lsl x10, x10, #1
    sub w12, w12, #1

    // ------- set offset table -------
    ld1  {v20.4s}, [x4]             // offset[0-3]
    ldr  w11, [x4, #16]             // offset[4]
    movi v2.4s, #0
    mov  v2.s[0], w11
    xtn  v0.4h, v20.4s
    xtn2 v0.8h, v2.4s
    xtn  v0.8b, v0.8h               // offset[0-4]

    movi v1.16b, #2
    movi v6.8h, #0
    dup  v7.8h, w12

    // ------- first row -------
    mov  x11, x8                     // x = start_x_r0

test_loop_x_eo_45_r0:

    cmp x11, x9
    bge test_loop_x_eo_45_end_r0
    add x12, x0, x11
    sub x13, x12, x2
    add x14, x12, x2
    add x13, x13, #2
    sub x14, x14, #2
    ld1  {v16.8h, v17.8h}, [x13]        // src[x-src_stride+1]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+src_stride-1]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v25.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v25.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v25.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x11
    cmp x11, x10
    bge test_maskmove_eo_45_r0
    st1 {v23.8h, v24.8h}, [x12]
    add x11, x11, #32
    b   test_loop_x_eo_45_r0

test_maskmove_eo_45_r0:
    sub x7, x9, x10
    add x7, x5, x7, lsl #4              // offset = 16*rownum
    ld1 {v30.4s, v31.4s}, [x7]          // load mask_r0
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

test_loop_x_eo_45_end_r0:
    add x0, x0, x2                      // src+=src_stride
    add x1, x1, x3                      // dst+=dst_stride

    // ------- middle rows -------
    // get param
#if defined(__APPLE__)
    ldp w7, w8, [sp, #8]                // x7 start_x_r; x8 end_x_r
#else
    ldp x7, x8, [sp, #16]               // x7 start_x_r; x8 end_x_r
#endif
    sxtw x7, w7
    sxtw x8, w8

    sub x9, x8, x7
    and x9, x9, #15
    add x12, x5, x9, lsl #5
    ld1 {v30.4s, v31.4s}, [x12]         // mask_r

    sub x10, x8, x9                     // end_x_r_16

    sub x11, x6, #2                     // y = mb_height - 2

    lsl x7, x7, #1
    lsl x8, x8, #1
    lsl x10, x10, #1

test_loop_y_eo_45_r:
    mov x9, x7                          // x = start_x_r

test_loop_x_eo_45_r:
    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    add x13, x13, #2
    sub x14, x14, #2
    ld1  {v16.8h, v17.8h}, [x13]        // src[x-src_stride+1]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+src_stride-1]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v25.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v25.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v25.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_45_r
    add x9, x9, #32
    st1 {v23.8h, v24.8h}, [x12]
    cmp x9, x8
    blt test_loop_x_eo_45_r
    b   test_loop_x_eo_45_end_r

test_maskmove_eo_45_r:
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

test_loop_x_eo_45_end_r:
    subs x11, x11, #1
    add x0, x0, x2                      // src+=src_stride
    add x1, x1, x3                      // dst+=dst_stride
    bgt test_loop_y_eo_45_r

    // ------- last row -------
#if defined(__APPLE__)
    ldp w6, w7, [sp, #16]
#else
    ldp x6, x7, [sp, #32]
#endif
    sxtw x6, w6                         // start_x_rn
    sxtw x7, w7                         // end_x_rn

    sub x8, x7, x6
    and x8, x8, #15
    sub x10, x7, x8                     // end_x_rn_16

    lsl x6, x6, #1
    lsl x7, x7, #1
    lsl x10, x10, #1

    mov x9, x6                          // x = start_x_rn

test_loop_x_eo_45_rn:
    cmp x9, x7
    bge test_loop_x_eo_45_end_rn
    add x12, x0, x9
    sub x13, x12, x2
    add x14, x12, x2
    add x13, x13, #2
    sub x14, x14, #2
    ld1  {v16.8h, v17.8h}, [x13]        // src[x-src_stride+1]
    ld1  {v18.8h, v19.8h}, [x12]        // src[x]
    ld1  {v20.8h, v21.8h}, [x14]        // src[x+src_stride-1]

    // leftsign & rightsign
    umin v2.8h, v16.8h, v18.8h
    umin v3.8h, v17.8h, v19.8h
    umin v4.8h, v18.8h, v20.8h
    umin v5.8h, v19.8h, v21.8h

    cmeq v22.8h, v2.8h, v16.8h
    cmeq v23.8h, v2.8h, v18.8h
    cmeq v24.8h, v3.8h, v17.8h
    cmeq v25.8h, v3.8h, v19.8h
    cmeq v26.8h, v4.8h, v18.8h
    cmeq v27.8h, v4.8h, v20.8h
    cmeq v28.8h, v5.8h, v19.8h
    cmeq v29.8h, v5.8h, v21.8h

    sub  v16.8h, v23.8h, v22.8h         // leftsign
    sub  v17.8h, v25.8h, v24.8h
    sub  v20.8h, v26.8h, v27.8h         // rightsign
    sub  v21.8h, v28.8h, v29.8h

    // get edgetype
    add v16.8h, v16.8h, v20.8h          // edgetype
    add v17.8h, v17.8h, v21.8h

    xtn  v16.8b, v16.8h
    xtn2 v16.16b, v17.8h

    add v16.16b, v16.16b, v1.16b        // edgetype+2

    tbl v25.16b, {v0.16b}, v16.16b      // offset

    saddw  v23.8h, v18.8h, v25.8b       // offset+src[x] low 8 samples
    saddw2 v24.8h, v19.8h, v25.16b      // offset+src[x] high 8 samples

    smax v23.8h, v23.8h, v6.8h
    smax v24.8h, v24.8h, v6.8h
    smin v23.8h, v23.8h, v7.8h
    smin v24.8h, v24.8h, v7.8h

    add x12, x1, x9
    cmp x9, x10
    bge test_maskmove_eo_45_rn
    st1 {v23.8h, v24.8h}, [x12]
    add x9, x9, #32
    b   test_loop_x_eo_45_rn

test_maskmove_eo_45_rn:
    sub x6, x7, x10
    add x6, x5, x6, lsl #4              // offset = 16*rownum
    ld1 {v30.4s, v31.4s}, [x6]          // load mask_r0
    ld1 {v21.8h, v22.8h}, [x12]         // load 16 pixels from dst+x
    bif v23.16b, v21.16b, v30.16b
    bif v24.16b, v22.16b, v31.16b
    st1 {v23.8h, v24.8h}, [x12]

test_loop_x_eo_45_end_rn:

    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_eo_45_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, char_t* mask, int mb_height, int bit_depth, int start_x_r0,
 *  int end_x_r0, int start_x_r, int end_x_r, int start_x_rn, int end_x_rn)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, mask->x5, mb_height->x6, bit_depth->x7
 ************************************************************************************************************************************/
function uavs3e_sao_eo_45_chroma_arm64
#if defined(__APPLE__)
    ldp w8, w9, [sp]
#else
    ldp x8, x9, [sp]                        // start_x_r0 and end_x_r0
#endif

    mov     w12, #1

    sxtw    x8, w8                          // start_x_r0
    sxtw    x9, w9                          // end_x_r0

    lsl     w12, w12, w7
    sub     x11, x9, x8
    and     x11, x11, #15
    sub     x10, x9, x11                    //-- end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f);
    sub     w12, w12, #1

//------- set offset table: v0 -----------
    ld1     {v20.4s}, [x4]                  //-- load offset[0-3]
    ldr     w11, [x4, #16]                  //-- load offset4
    xtn     v0.4h, v20.4s
    mov     v0.h[4], w11
    xtn     v0.8b, v0.8h                    //-- convert int32 to byte

    lsl     x2, x2, #1
    lsl     x3, x3, #1
    lsl     x8, x8, #1
    lsl     x9, x9, #1
    lsl     x10, x10, #1

    mov     w11, #0x0000ffff

    movi    v29.8h, #0                      //-- min_pel
    dup     v30.8h, w12                     //-- max_pel
    dup     v31.4s, w11

    movi    v1.16b, #2                      //-- constant(save)

//---------------------first row-------------------------
    mov     x11, x8                         //-- x = start_x_r0
loop_x_eo_45_chroma_r0:
    cmp     x11, x9
    bge     loop_x_eo_45_chroma_end_r0
    add     x12, x0, x11
    sub     x13, x12, x2
    add     x14, x12, x2
    add     x13, x13, #4
    sub     x14, x14, #4
    ld1     {v2.4s, v3.4s}, [x12]           //-- load src[x] (save)
    ld1     {v4.4s, v5.4s}, [x13]           //-- load src[x-src_stride+2]
    ld1     {v6.4s, v7.4s}, [x14]           //-- load src[x+src_stride-2]
    xtn     v21.4h, v2.4s
    xtn2    v21.8h, v3.4s
    xtn     v20.4h, v4.4s
    xtn2    v20.8h, v5.4s
    xtn     v22.4h, v6.4s
    xtn2    v22.8h, v7.4s

    // get leftsign & rightsign
    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h          //-- leftsign
    sub     v22.8h, v27.8h, v28.8h          //-- rightsign

    add     v20.8h, v22.8h, v20.8h          // edgetype

    xtn     v20.8b, v20.8h

    add     v20.8b, v20.8b, v1.8b           // edgetype+2
    tbl     v22.8b, {v0.16b}, v20.8b        // offset

    saddw   v20.8h, v21.8h, v22.8b          // offset+src[x]

    smax    v20.8h, v20.8h, v29.8h
    smin    v20.8h, v20.8h, v30.8h

    add     x12, x1, x11                    //-- dst+x
    uxtl    v21.4s, v20.4h
    uxtl2   v22.4s, v20.8h

    cmp     x11, x10
    bge     maskmove_eo_45_chroma_r0
    ld1     {v3.8h, v4.8h}, [x12]
    bif     v21.16b, v3.16b, v31.16b
    bif     v22.16b, v4.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    add     x11, x11, #32
    b       loop_x_eo_45_chroma_r0
maskmove_eo_45_chroma_r0:
    sub     x7, x9, x10
    add     x7, x5, x7, lsl #3              //-- offset = 16*rownum
    ld1     {v25.8h}, [x7]                  //-- load mask_r0
    ld1     {v4.8h, v5.8h}, [x12]
    sxtl    v18.4s, v25.4h
    sxtl2   v19.4s, v25.8h
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v4.16b, v18.16b
    bif     v22.16b, v5.16b, v19.16b
    st1     {v21.8h, v22.8h}, [x12]

loop_x_eo_45_chroma_end_r0:
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride

//--------------------------------middle rows--------------------------------
#if defined(__APPLE__)
    ldp     w7 , w8, [sp, #8]               //-- x7=start_x_r; x8=end_x_r
#else
    ldp     x7 , x8, [sp, #16]              //-- x7=start_x_r; x8=end_x_r
#endif
    sxtw    x7 , w7
    sxtw    x8 , w8

    sub     x9 , x8, x7
    and     x9 , x9, #15
    add     x12, x5, x9, lsl #4
    ld1     {v25.8h}, [x12]                 //-- mask_r

    sub     x10, x8, x9                     //-- end_x_r_16

    sxtl    v18.4s, v25.4h
    sxtl2   v19.4s, v25.8h

    lsl     x7, x7, #1
    lsl     x8, x8, #1
    lsl     x10, x10, #1

    and     v18.16b, v18.16b, v31.16b       //-- mask_r
    and     v19.16b, v19.16b, v31.16b

    sub     x11, x6, #2                     //-- y = mb_height - 2
loop_y_eo_45_chroma_r:
    mov     x9, x7                          //-- x = start_x_r
loop_x_eo_45_chroma_r:
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    add     x13, x13, #4
    sub     x14, x14, #4
    ld1     {v2.4s, v3.4s}, [x12]           //-- load src[x] (save)
    ld1     {v4.4s, v5.4s}, [x13]           //-- load src[x-src_stride+2]
    ld1     {v6.4s, v7.4s}, [x14]           //-- load src[x+src_stride-2]
    xtn     v21.4h, v2.4s
    xtn2    v21.8h, v3.4s
    xtn     v20.4h, v4.4s
    xtn2    v20.8h, v5.4s
    xtn     v22.4h, v6.4s
    xtn2    v22.8h, v7.4s

    // get leftsign & rightsign
    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h          //-- leftsign
    sub     v22.8h, v27.8h, v28.8h          //-- rightsign

    add     v20.8h, v22.8h, v20.8h          // edgetype

    xtn     v20.8b, v20.8h

    add     v20.8b, v20.8b, v1.8b           // edgetype+2
    tbl     v22.8b, {v0.16b}, v20.8b        // offset

    saddw   v20.8h, v21.8h, v22.8b          // offset+src[x]

    smax    v20.8h, v20.8h, v29.8h
    smin    v20.8h, v20.8h, v30.8h

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    uxtl    v23.4s, v20.4h
    uxtl2   v24.4s, v20.8h
    ld1     {v4.8h, v5.8h}, [x12]
    bge     maskmove_eo_45_chroma_r
    bif     v23.16b, v4.16b, v31.16b
    bif     v24.16b, v5.16b, v31.16b
    st1     {v23.8h, v24.8h}, [x12]
    add     x9, x9, #32
    cmp     x9, x8
    blt     loop_x_eo_45_chroma_r
    b       loop_x_eo_45_chroma_end_r
maskmove_eo_45_chroma_r:
    //--- maskmove
    bif    v23.16b, v4.16b, v18.16b
    bif    v24.16b, v5.16b, v19.16b
    st1    {v23.8h, v24.8h}, [x12]

loop_x_eo_45_chroma_end_r:
    subs   x11, x11, #1                     //-- y--
    add    x0, x0, x2                       //-- src+=src_stride
    add    x1, x1, x3                       //-- dst+=dst_stride
    bgt    loop_y_eo_45_chroma_r

//---------------------------------last row--------------------------------
#if defined(__APPLE__)
    ldp     w6 , w7, [sp, #16]              //-- x6=start_x_rn; x7=end_x_rn
#else
    ldp     x6 , x7, [sp, #32]              //-- x6=start_x_rn; x7=end_x_rn
#endif
    sxtw    x7 , w7
    sxtw    x6 , w6

    sub     x8 , x7, x6
    and     x8 , x8, #15
    sub     x10, x7, x8                     //-- end_x_rn_16

    lsl     x7, x7, #1
    lsl     x6, x6, #1
    lsl     x10, x10, #1
    mov     x9 , x6                         //-- x = start_x_rn
loop_x_eo_45_chroma_rn:
    cmp     x9 , x7
    bge     loop_x_eo_45_chroma_end_rn
    add     x12, x0 , x9
    sub     x13, x12, x2
    add     x14, x12, x2
    add     x13, x13, #4
    sub     x14, x14, #4
    ld1     {v2.4s, v3.4s}, [x12]           //-- load src[x] (save)
    ld1     {v4.4s, v5.4s}, [x13]           //-- load src[x-src_stride+2]
    ld1     {v6.4s, v7.4s}, [x14]           //-- load src[x+src_stride-2]
    xtn     v21.4h, v2.4s
    xtn2    v21.8h, v3.4s
    xtn     v20.4h, v4.4s
    xtn2    v20.8h, v5.4s
    xtn     v22.4h, v6.4s
    xtn2    v22.8h, v7.4s

    // get leftsign & rightsign
    umin    v23.8h, v20.8h, v21.8h
    umin    v26.8h, v21.8h, v22.8h
    cmeq    v24.8h, v23.8h, v20.8h
    cmeq    v25.8h, v23.8h, v21.8h
    cmeq    v27.8h, v26.8h, v21.8h
    cmeq    v28.8h, v26.8h, v22.8h
    sub     v20.8h, v25.8h, v24.8h          //-- leftsign
    sub     v22.8h, v27.8h, v28.8h          //-- rightsign

    add     v20.8h, v22.8h, v20.8h          // edgetype

    xtn     v20.8b, v20.8h

    add     v20.8b, v20.8b, v1.8b           // edgetype+2
    tbl     v22.8b, {v0.16b}, v20.8b        // offset

    saddw   v20.8h, v21.8h, v22.8b          // offset+src[x]

    smax    v20.8h, v20.8h, v29.8h
    smin    v20.8h, v20.8h, v30.8h

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    uxtl    v21.4s, v20.4h
    uxtl2   v22.4s, v20.8h
    ld1     {v4.8h, v5.8h}, [x12]
    bge     maskmove_eo_45_chroma_rn
    bif     v21.16b, v4.16b, v31.16b
    bif     v22.16b, v5.16b, v31.16b
    st1     {v21.8h, v22.8h}, [x12]
    add     x9, x9, #32
    b       loop_x_eo_45_chroma_rn
maskmove_eo_45_chroma_rn:
    sub     x6, x7 , x10
    add     x6, x5 , x6, lsl #3             //-- offset = 16*rownum
    ld1     {v25.8h}, [x6]                  //-- load mask_rn
    sxtl    v18.4s, v25.4h
    sxtl2   v19.4s, v25.8h
    and     v18.16b, v18.16b, v31.16b
    and     v19.16b, v19.16b, v31.16b
    bif     v21.16b, v4.16b, v18.16b
    bif     v22.16b, v5.16b, v19.16b
    st1     {v21.16b, v22.16b}, [x12]
loop_x_eo_45_chroma_end_rn:
    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_bo_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int *bind_ids, int mb_width, int mb_height, int bit_depth)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, bind_ids->x5, mb_width->x6, mb_height->x7, bit_depth->x8
 ************************************************************************************************************************************/
function uavs3e_sao_bo_arm64
    ldr w8 , [sp]                   // bit_depth
    ldr w9 , [x5]
    ldr w10, [x5, #4]
    ldr w11, [x5, #8]
    ldr w12, [x5, #12]

    sub w14, w8, #5                 // shift = bit_depth - 5
    mov w13, #1

    neg w14, w14
    ld1 {v18.4s}, [x4]             // load offsets
    dup v29.8h, w14
    lsl w13, w13, w8

    dup v0.8h, w9
    dup v1.8h, w10
    dup v2.8h, w11
    dup v3.8h, w12

    sub w13, w13, #1
    mov w9, v18.s[0]
    mov w10, v18.s[1]
    mov w11, v18.s[2]
    mov w12, v18.s[3]

    dup v4.8h, w9                   // offset[0]
    dup v5.8h, w10                  // offset[1]
    dup v6.8h, w11                  // offset[2]
    dup v7.8h, w12                  // offset[3]

    movi v30.8h, #0                 // min_pel
    dup  v31.8h, w13                // max_pel

    lsl  x2, x2, #1
    lsl  x3, x3, #1

    and  w9, w6, #15
    lsl  x6, x6, #1

    cmp  w9, #0                     // width%16 == 0?
    beq  sao_bo_w16x_y

    sub  x10, x6, #8                // mb_width - 4

sao_bo_y:
    mov x9, #0                      // x = mb_width
    mov x11, x0
    mov x12, x1
sao_bo_x:
    ld1  {v16.8h}, [x11]            // src[x]
    ushl v18.8h, v16.8h, v29.8h

    cmeq v20.8h, v18.8h, v0.8h
    cmeq v21.8h, v18.8h, v1.8h
    cmeq v22.8h, v18.8h, v2.8h
    cmeq v23.8h, v18.8h, v3.8h

    and  v20.16b, v20.16b, v4.16b
    and  v21.16b, v21.16b, v5.16b
    and  v22.16b, v22.16b, v6.16b
    and  v23.16b, v23.16b, v7.16b

    orr  v20.16b, v20.16b, v21.16b
    orr  v22.16b, v22.16b, v23.16b

    orr  v20.16b, v20.16b, v22.16b  // get offsets

    add v16.8h, v16.8h, v20.8h

    smax v16.8h, v16.8h, v30.8h
    smin v16.8h, v16.8h, v31.8h

    cmp x9, x10
    bge maskmove_bo
    add x9, x9, #16
    add x11, x11, #16
    st1 {v16.8h}, [x12]
    add x12, x12, #16
    cmp x9, x6
    blt sao_bo_x
    b   sao_bo_x_end

maskmove_bo:
    st1 {v16.4h}, [x12]

sao_bo_x_end:
    subs w7, w7, #1
    add x0, x0, x2
    add x1, x1, x3
    bgt sao_bo_y
    b   sao_bo_end

sao_bo_w16x_y:
    mov x11, x0
    mov x12, x1
    mov x9, x6                      // x = mb_width
sao_bo_w16x_x:
    ld1  {v16.8h, v17.8h}, [x11]    // src[x]
    ushl v18.8h, v16.8h, v29.8h
    ushl v19.8h, v17.8h, v29.8h

    cmeq v20.8h, v18.8h, v0.8h
    cmeq v21.8h, v18.8h, v1.8h
    cmeq v22.8h, v18.8h, v2.8h
    cmeq v23.8h, v18.8h, v3.8h

    cmeq v24.8h, v19.8h, v0.8h
    cmeq v25.8h, v19.8h, v1.8h
    cmeq v26.8h, v19.8h, v2.8h
    cmeq v27.8h, v19.8h, v3.8h

    and  v20.16b, v20.16b, v4.16b
    and  v21.16b, v21.16b, v5.16b
    and  v22.16b, v22.16b, v6.16b
    and  v23.16b, v23.16b, v7.16b

    and  v24.16b, v24.16b, v4.16b
    and  v25.16b, v25.16b, v5.16b
    and  v26.16b, v26.16b, v6.16b
    and  v27.16b, v27.16b, v7.16b

    orr  v20.16b, v20.16b, v21.16b
    orr  v22.16b, v22.16b, v23.16b
    orr  v24.16b, v24.16b, v25.16b
    orr  v26.16b, v26.16b, v27.16b

    orr  v20.16b, v20.16b, v22.16b  // get offsets
    orr  v24.16b, v24.16b, v26.16b

    add v16.8h, v16.8h, v20.8h
    add v17.8h, v17.8h, v24.8h

    smax v16.8h, v16.8h, v30.8h
    smax v17.8h, v17.8h, v30.8h
    smin v16.8h, v16.8h, v31.8h
    smin v17.8h, v17.8h, v31.8h

    subs x9, x9, #32
    add  x11, x11, #32

    st1 {v16.8h, v17.8h}, [x12]
    add  x12, x12, #32
    bgt sao_bo_w16x_x

    subs w7, w7, #1
    add x0, x0, x2
    add x1, x1, x3
    bgt sao_bo_w16x_y

sao_bo_end:
    ret

/***********************************************************************************************************************************
 *  void uavs3e_sao_bo_chroma_arm64(pel_t* src, pel_t* dst, int src_stride, int dst_stride, int* offset, int *bind_ids, int mb_width, int mb_height, int bit_depth)
 *  src->x0, dst->x1, src_stride->x2, dst_stride->x3, offset->x4, bind_ids->x5, mb_width->x6, mb_height->x7
 ************************************************************************************************************************************/
function uavs3e_sao_bo_chroma_arm64
    ldr     w8 , [sp]                       // bit_depth
    ldr     w9 , [x5]
    ldr     w10, [x5, #4]
    ldr     w11, [x5, #8]
    ldr     w12, [x5, #12]

    mov     w13, #1
    sub     w14, w8, #5                     // shift = bit_depth - 5
    ld1     {v19.4s}, [x4]

    neg     w14, w14
    lsl     w13, w13, w8
    dup     v0.8h, w9
    dup     v1.8h, w10
    dup     v2.8h, w11
    dup     v3.8h, w12
    sub     w13, w13, #1

    mov     w9, #0x0000ffff
    dup     v29.8h, w14
    movi    v30.8h, #0                      //-- min_pel
    dup     v31.8h, w13                     //-- max_pel
    dup     v18.4s, w9                      //-- mask_uv

    xtn     v19.4h, v19.4s
    dup     v4.8h, v19.h[0]                 //-- offset[0]
    dup     v5.8h, v19.h[1]                 //-- offset[1]
    dup     v6.8h, v19.h[2]                 //-- offset[2]
    dup     v7.8h, v19.h[3]                 //-- offset[3]

    lsl     x6, x6, #1
    lsl     x2, x2, #1
    lsl     x3, x3, #1
    sub     x10, x6, #16                    //-- mb_width - 8

loop_y_bo_chroma:
    mov     x11, x0
    mov     x9, #0                          //-- x = 0
loop_x_bo_chroma:
    ld1     {v19.8h, v20.8h}, [x11]         //-- load src[x]    (save)
    xtn     v23.4h, v19.4s
    xtn2    v23.8h, v20.4s
    ushl    v22.8h, v23.8h, v29.8h

    cmeq    v19.8h, v22.8h, v0.8h
    cmeq    v20.8h, v22.8h, v1.8h
    cmeq    v21.8h, v22.8h, v2.8h
    cmeq    v22.8h, v22.8h, v3.8h

    and     v19.16b, v19.16b, v4.16b
    and     v20.16b, v20.16b, v5.16b
    and     v21.16b, v21.16b, v6.16b
    and     v22.16b, v22.16b, v7.16b

    orr     v19.16b, v19.16b, v20.16b
    orr     v21.16b, v21.16b, v22.16b
    orr     v19.16b, v19.16b, v21.16b       //-- get offsets

    add     v20.8h, v23.8h, v19.8h
    smax    v20.8h, v20.8h, v30.8h
    smin    v20.8h, v20.8h, v31.8h

    add     x12, x1, x9                     //-- dst+x
    cmp     x9, x10
    bge     maskmove_bo_chroma
    ld1     {v16.8h, v17.8h}, [x12]
    uxtl    v21.4s, v20.4h
    uxtl2   v22.4s, v20.8h
    bif     v21.16b, v16.16b, v18.16b
    bif     v22.16b, v17.16b, v18.16b
    add     x9, x9, #32
    add     x11, x11, #32
    st1     {v21.8h, v22.8h}, [x12]
    cmp     x9, x6
    blt     loop_x_bo_chroma
    b       loop_x_bo_chroma_end
maskmove_bo_chroma:
    //--- maskmove
    ld1     {v19.8h}, [x12]
    uxtl    v20.4s, v20.4h
    bif     v20.16b, v19.16b, v18.16b
    st1     {v20.8h}, [x12]

loop_x_bo_chroma_end:
    subs    x7, x7, #1                      //-- y--
    add     x0, x0, x2                      //-- src+=src_stride
    add     x1, x1, x3                      //-- dst+=dst_stride
    bgt     loop_y_bo_chroma

    ret

#endif  // COMPILE_10BIT

#endif
