#include "def_arm64.S"
#if defined(__arm64__)

#if !COMPILE_10BIT

/******************************************************************************************************
*  void uavs3e_recon_w4_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7, int bit_depth->
******************************************************************************************************/
function uavs3e_recon_w4_arm64
    ldr w8, [sp]

    //max_val = (1 << bit_depth) - 1;
    mov w9, #1
    lsl w9, w9, w8
    sub w9, w9, #1
    mov w10, #0

    cmp x7, #0
    beq cbf_zero_w4

recon_w4_loopx:
    //load *resi
    //ld4 {v0.d,v1.d,v2.d,v3.d}[0], [x0], #32
    ld1 {v0.d}[0], [x0], #8
    ld1 {v1.d}[0], [x0], #8
    ld1 {v2.d}[0], [x0], #8
    ld1 {v3.d}[0], [x0], #8
    //load *pred
    ld1 {v4.8b}, [x1], x2
    ld1 {v5.8b}, [x1], x2
    ld1 {v6.8b}, [x1], x2
    ld1 {v7.8b}, [x1], x2

    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b

    sqadd v0.8h, v0.8h, v4.8h
    sqadd v1.8h, v1.8h, v5.8h
    sqadd v2.8h, v2.8h, v6.8h
    sqadd v3.8h, v3.8h, v7.8h

    //clip
    dup v4.4h, w9   //max_val
    dup v5.4h, w10  //0
    smin v0.4h, v0.4h, v4.4h
    smax v0.4h, v0.4h, v5.4h
    smin v1.4h, v1.4h, v4.4h
    smax v1.4h, v1.4h, v5.4h
    smin v2.4h, v2.4h, v4.4h
    smax v2.4h, v2.4h, v5.4h
    smin v3.4h, v3.4h, v4.4h
    smax v3.4h, v3.4h, v5.4h

    xtn v0.8b, v0.8h
    xtn v1.8b, v1.8h
    xtn v2.8b, v2.8h
    xtn v3.8b, v3.8h

    //store to blk
    st1 {v0.s}[0], [x5], x6
    st1 {v1.s}[0], [x5], x6
    st1 {v2.s}[0], [x5], x6
    st1 {v3.s}[0], [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w4_loopx
    b recon_w4_end

cbf_zero_w4:
    //load pred
    ld1 {v4.s}[0], [x1], x2
    ld1 {v5.s}[0], [x1], x2
    ld1 {v6.s}[0], [x1], x2
    ld1 {v7.s}[0], [x1], x2

    st1 {v4.s}[0], [x5], x6
    st1 {v5.s}[0], [x5], x6
    st1 {v6.s}[0], [x5], x6
    st1 {v7.s}[0], [x5], x6
    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w4

recon_w4_end:

    ret


/******************************************************************************************************
*  void uavs3e_recon_w8_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7
******************************************************************************************************/
function uavs3e_recon_w8_arm64

    //max_val = (1 << bit_depth) - 1;
    mov w9, #1
    lsl w9, w9, #8
    sub w9, w9, #1
    mov w10, #0

    cmp x7, #0
    beq cbf_zero_w8

recon_w8_loopx:
    //load *resi
    //ld4 {v0.d,v1.d,v2.d,v3.d}[0], [x0], #32
    ld1 {v0.8h}, [x0], #16
    ld1 {v1.8h}, [x0], #16
    ld1 {v2.8h}, [x0], #16
    ld1 {v3.8h}, [x0], #16
    //load *pred
    ld1 {v4.8b}, [x1], x2
    ld1 {v5.8b}, [x1], x2
    ld1 {v6.8b}, [x1], x2
    ld1 {v7.8b}, [x1], x2

    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b

    sqadd v0.8h, v0.8h, v4.8h
    sqadd v1.8h, v1.8h, v5.8h
    sqadd v2.8h, v2.8h, v6.8h
    sqadd v3.8h, v3.8h, v7.8h

    //clip
    dup v4.8h, w9   //max_val
    dup v5.8h, w10  //0
    smin v0.8h, v0.8h, v4.8h
    smax v0.8h, v0.8h, v5.8h
    smin v1.8h, v1.8h, v4.8h
    smax v1.8h, v1.8h, v5.8h
    smin v2.8h, v2.8h, v4.8h
    smax v2.8h, v2.8h, v5.8h
    smin v3.8h, v3.8h, v4.8h
    smax v3.8h, v3.8h, v5.8h

    xtn v0.8b, v0.8h
    xtn v1.8b, v1.8h
    xtn v2.8b, v2.8h
    xtn v3.8b, v3.8h

    //store to blk
    st1 {v0.d}[0], [x5], x6
    st1 {v1.d}[0], [x5], x6
    st1 {v2.d}[0], [x5], x6
    st1 {v3.d}[0], [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w8_loopx
    b recon_w8_end

cbf_zero_w8:
    //
    ld1 {v4.8b}, [x1], x2
    ld1 {v5.8b}, [x1], x2
    ld1 {v6.8b}, [x1], x2
    ld1 {v7.8b}, [x1], x2
    st1 {v4.d}[0], [x5], x6
    st1 {v5.d}[0], [x5], x6
    st1 {v6.d}[0], [x5], x6
    st1 {v7.d}[0], [x5], x6
    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w8

recon_w8_end:

    ret

/******************************************************************************************************
*  void uavs3e_recon_w16_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7
******************************************************************************************************/
function uavs3e_recon_w16_arm64

    //max_val = (1 << bit_depth) - 1;
    mov w9, #1
    lsl w9, w9, #8
    sub w9, w9, #1      //max_val
    mov w10, #0         //0

    cmp x7, #0
    beq cbf_zero_w16

recon_w16_loopx:
    //load *resi
    //ld4 {v0.d,v1.d,v2.d,v3.d}[0], [x0], #32
    ld1 {v0.8h,v1.8h}, [x0], #32
    ld1 {v2.8h,v3.8h}, [x0], #32
    ld1 {v4.8h,v5.8h}, [x0], #32
    ld1 {v6.8h,v7.8h}, [x0], #32
    //load *pred

    ld1 {v16.8b,v17.8b}, [x1], x2
    ld1 {v18.8b,v19.8b}, [x1], x2
    ld1 {v20.8b,v21.8b}, [x1], x2
    ld1 {v22.8b,v23.8b}, [x1], x2

    uxtl v16.8h, v16.8b
    uxtl v17.8h, v17.8b
    uxtl v18.8h, v18.8b
    uxtl v19.8h, v19.8b
    uxtl v20.8h, v20.8b
    uxtl v21.8h, v21.8b
    uxtl v22.8h, v22.8b
    uxtl v23.8h, v23.8b

    sqadd v0.8h, v0.8h, v16.8h
    sqadd v1.8h, v1.8h, v17.8h
    sqadd v2.8h, v2.8h, v18.8h
    sqadd v3.8h, v3.8h, v19.8h
    sqadd v4.8h, v4.8h, v20.8h
    sqadd v5.8h, v5.8h, v21.8h
    sqadd v6.8h, v6.8h, v22.8h
    sqadd v7.8h, v7.8h, v23.8h

    //clip
    dup v16.8h, w9   //max_val
    dup v17.8h, w10  //0
    smin v0.8h, v0.8h, v16.8h
    smax v0.8h, v0.8h, v17.8h
    smin v1.8h, v1.8h, v16.8h
    smax v1.8h, v1.8h, v17.8h
    smin v2.8h, v2.8h, v16.8h
    smax v2.8h, v2.8h, v17.8h
    smin v3.8h, v3.8h, v16.8h
    smax v3.8h, v3.8h, v17.8h
    smin v4.8h, v4.8h, v16.8h
    smax v4.8h, v4.8h, v17.8h
    smin v5.8h, v5.8h, v16.8h
    smax v5.8h, v5.8h, v17.8h
    smin v6.8h, v6.8h, v16.8h
    smax v6.8h, v6.8h, v17.8h
    smin v7.8h, v7.8h, v16.8h
    smax v7.8h, v7.8h, v17.8h

    xtn v0.8b, v0.8h
    xtn v1.8b, v1.8h
    xtn v2.8b, v2.8h
    xtn v3.8b, v3.8h
    xtn v4.8b, v4.8h
    xtn v5.8b, v5.8h
    xtn v6.8b, v6.8h
    xtn v7.8b, v7.8h

    //store to blk
    st1 {v0.8b,v1.8b}, [x5], x6
    st1 {v2.8b,v3.8b}, [x5], x6
    st1 {v4.8b,v5.8b}, [x5], x6
    st1 {v6.8b,v7.8b}, [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w16_loopx
    b recon_w16_end

cbf_zero_w16:
    //
    ld1 {v4.16b}, [x1], x2
    ld1 {v5.16b}, [x1], x2
    ld1 {v6.16b}, [x1], x2
    ld1 {v7.16b}, [x1], x2

    st1 {v4.16b}, [x5], x6
    st1 {v5.16b}, [x5], x6
    st1 {v6.16b}, [x5], x6
    st1 {v7.16b}, [x5], x6
    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w16

recon_w16_end:

    ret

/******************************************************************************************************
*  void uavs3e_recon_w32_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7
******************************************************************************************************/
function uavs3e_recon_w32_arm64

    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    //max_val = (1 << bit_depth) - 1;
    mov w9, #1
    lsl w9, w9, #8
    sub w9, w9, #1
    mov w10, #0

    cmp x7, #0
    beq cbf_zero_w32

recon_w32_loopx:
    //load *resi
    //ld4 {v0.d,v1.d,v2.d,v3.d}[0], [x0], #32
    ld1 {v0.8h, v1.8h, v2.8h, v3.8h},      [x0], #64
    ld1 {v4.8h, v5.8h, v6.8h, v7.8h},      [x0], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h},    [x0], #64
    ld1 {v12.8h, v13.8h, v14.8h, v15.8h},  [x0], #64
    //load *pred
    ld1 {v16.8b,v17.8b,v18.8b,v19.8b}, [x1], x2
    ld1 {v20.8b,v21.8b,v22.8b,v23.8b}, [x1], x2
    ld1 {v24.8b,v25.8b,v26.8b,v27.8b}, [x1], x2
    ld1 {v28.8b,v29.8b,v30.8b,v31.8b}, [x1], x2

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

    sqadd v0.8h, v0.8h, v16.8h
    sqadd v1.8h, v1.8h, v17.8h
    sqadd v2.8h, v2.8h, v18.8h
    sqadd v3.8h, v3.8h, v19.8h
    sqadd v4.8h, v4.8h, v20.8h
    sqadd v5.8h, v5.8h, v21.8h
    sqadd v6.8h, v6.8h, v22.8h
    sqadd v7.8h, v7.8h, v23.8h
    sqadd v8.8h, v8.8h, v24.8h
    sqadd v9.8h, v9.8h, v25.8h
    sqadd v10.8h, v10.8h, v26.8h
    sqadd v11.8h, v11.8h, v27.8h
    sqadd v12.8h, v12.8h, v28.8h
    sqadd v13.8h, v13.8h, v29.8h
    sqadd v14.8h, v14.8h, v30.8h
    sqadd v15.8h, v15.8h, v31.8h

    //clip
    dup v16.8h, w9   //max_val
    dup v17.8h, w10  //0
    smin v0.8h, v0.8h, v16.8h
    smax v0.8h, v0.8h, v17.8h
    smin v1.8h, v1.8h, v16.8h
    smax v1.8h, v1.8h, v17.8h
    smin v2.8h, v2.8h, v16.8h
    smax v2.8h, v2.8h, v17.8h
    smin v3.8h, v3.8h, v16.8h
    smax v3.8h, v3.8h, v17.8h
    smin v4.8h, v4.8h, v16.8h
    smax v4.8h, v4.8h, v17.8h
    smin v5.8h, v5.8h, v16.8h
    smax v5.8h, v5.8h, v17.8h
    smin v6.8h, v6.8h, v16.8h
    smax v6.8h, v6.8h, v17.8h
    smin v7.8h, v7.8h, v16.8h
    smax v7.8h, v7.8h, v17.8h
    smin v8.8h, v8.8h, v16.8h
    smax v8.8h, v8.8h, v17.8h
    smin v9.8h, v9.8h, v16.8h
    smax v9.8h, v9.8h, v17.8h
    smin v10.8h, v10.8h, v16.8h
    smax v10.8h, v10.8h, v17.8h
    smin v11.8h, v11.8h, v16.8h
    smax v11.8h, v11.8h, v17.8h
    smin v12.8h, v12.8h, v16.8h
    smax v12.8h, v12.8h, v17.8h
    smin v13.8h, v13.8h, v16.8h
    smax v13.8h, v13.8h, v17.8h
    smin v14.8h, v14.8h, v16.8h
    smax v14.8h, v14.8h, v17.8h
    smin v15.8h, v15.8h, v16.8h
    smax v15.8h, v15.8h, v17.8h

    xtn v0.8b, v0.8h
    xtn v1.8b, v1.8h
    xtn v2.8b, v2.8h
    xtn v3.8b, v3.8h
    xtn v4.8b, v4.8h
    xtn v5.8b, v5.8h
    xtn v6.8b, v6.8h
    xtn v7.8b, v7.8h
    xtn v8.8b, v8.8h
    xtn v9.8b, v9.8h
    xtn v10.8b, v10.8h
    xtn v11.8b, v11.8h
    xtn v12.8b, v12.8h
    xtn v13.8b, v13.8h
    xtn v14.8b, v14.8h
    xtn v15.8b, v15.8h

    //store to blk
    st1 {v0.8b,v1.8b,v2.8b,v3.8b}, [x5], x6
    st1 {v4.8b,v5.8b,v6.8b,v7.8b}, [x5], x6
    st1 {v8.8b,v9.8b,v10.8b,v11.8b}, [x5], x6
    st1 {v12.8b,v13.8b,v14.8b,v15.8b}, [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w32_loopx
    b recon_w32_end

cbf_zero_w32:
    //
    ld1 {v4.16b,v5.16b}, [x1], x2
    ld1 {v6.16b,v7.16b}, [x1], x2
    ld1 {v8.16b,v9.16b}, [x1], x2
    ld1 {v10.16b,v11.16b}, [x1], x2
    st1 {v4.16b,v5.16b}, [x5], x6
    st1 {v6.16b,v7.16b}, [x5], x6
    st1 {v8.16b,v9.16b}, [x5], x6
    st1 {v10.16b,v11.16b}, [x5], x6
    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w32

recon_w32_end:
    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64
    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_4_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_4_arm64

    lsl x5, x5, #1

diff_w4_loopx:

    //load *org
    //ld4 {v0.4h,v1.4h,v2.4h,v3.4h}, [x0], x1
    ld1 {v0.s}[0], [x0], x1
    ld1 {v0.s}[1], [x0], x1
    ld1 {v0.s}[2], [x0], x1
    ld1 {v0.s}[3], [x0], x1

    //load *pred
    //ld1 {v4.4h,v5.4h,v6.4h,v7.4h,}, [x2], x3
    ld1 {v1.s}[0], [x2], x3
    ld1 {v1.s}[1], [x2], x3
    ld1 {v1.s}[2], [x2], x3
    ld1 {v1.s}[3], [x2], x3

    uxtl v2.8h, v0.8b
    uxtl2 v3.8h, v0.16b
    uxtl v4.8h, v1.8b
    uxtl2 v5.8h, v1.16b

    //减
    sub v0.8h, v2.8h, v4.8h
    sub v1.8h, v3.8h, v5.8h

//    ssubl v2.8h,v0.8b,v1.8b
//    ssubl2 v3.8h,v0.16b,v1.16b


    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.d}[0], [x4], x5
    st1 {v0.d}[1], [x4], x5
    st1 {v1.d}[0], [x4], x5
    st1 {v1.d}[1], [x4], x5


//    //p_resi[i] = p_org[i] - p_pred[i]
//    st1 {v2.d}[0], [x4], x5
//    st1 {v2.d}[1], [x4], x5
//    st1 {v3.d}[0], [x4], x5
//    st1 {v3.d}[1], [x4], x5

    sub x6,x6,#4
    cmp x6,#0
    bgt diff_w4_loopx

    ret


/******************************************************************************************************
*  void uavs3e_pel_diff_8_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_8_arm64

    lsl x5, x5, #1

diff_w8_loopx:

    //load *org
    //ld4 {v0.4h,v1.4h,v2.4h,v3.4h}, [x0], x1
    ld1 {v0.d}[0], [x0], x1
    ld1 {v1.d}[0], [x0], x1
    ld1 {v2.d}[0], [x0], x1
    ld1 {v3.d}[0], [x0], x1

    //load *pred
    //ld1 {v4.4h,v5.4h,v6.4h,v7.4h,}, [x2], x3
    ld1 {v4.d}[0], [x2], x3
    ld1 {v5.d}[0], [x2], x3
    ld1 {v6.d}[0], [x2], x3
    ld1 {v7.d}[0], [x2], x3

    uxtl v0.8h, v0.8b
    uxtl v1.8h, v1.8b
    uxtl v2.8h, v2.8b
    uxtl v3.8h, v3.8b
    uxtl v4.8h, v4.8b
    uxtl v5.8h, v5.8b
    uxtl v6.8h, v6.8b
    uxtl v7.8h, v7.8b


    //减
    sub v0.8h, v0.8h, v4.8h
    sub v1.8h, v1.8h, v5.8h
    sub v2.8h, v2.8h, v6.8h
    sub v3.8h, v3.8h, v7.8h

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h}, [x4], x5
    st1 {v1.8h}, [x4], x5
    st1 {v2.8h}, [x4], x5
    st1 {v3.8h}, [x4], x5

    sub x6,x6,#4
    cmp x6,#0
    bgt diff_w8_loopx

    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_16_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_16_arm64

    lsl x5, x5, #1

diff_w16_loopx:

    //load *org
    //ld4 {v0.4h,v1.4h,v2.4h,v3.4h}, [x0], x1
    ld1 {v0.8b,v1.8b}, [x0], x1
    ld1 {v2.8b,v3.8b}, [x0], x1
    ld1 {v4.8b,v5.8b}, [x0], x1
    ld1 {v6.8b,v7.8b}, [x0], x1

    //load *pred
    //ld1 {v4.4h,v5.4h,v6.4h,v7.4h,}, [x2], x3
    ld1 {v16.8b,v17.8b}, [x2], x3
    ld1 {v18.8b,v19.8b}, [x2], x3
    ld1 {v20.8b,v21.8b}, [x2], x3
    ld1 {v22.8b,v23.8b}, [x2], x3

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

    //减
    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h


    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h,v1.8h}, [x4], x5
    st1 {v2.8h,v3.8h}, [x4], x5
    st1 {v4.8h,v5.8h}, [x4], x5
    st1 {v6.8h,v7.8h}, [x4], x5

    sub x6,x6,#4
    cmp x6,#0
    bgt diff_w16_loopx
    b diff_w16_end

diff_w16_end:

    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_32_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_32_arm64

    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    lsl x5, x5, #1

diff_w32_loopx:

    //load *org
    //ld4 {v0.8h-v15.8h}, [x0], x1
    ld1 {v0.8b,v1.8b,v2.8b,v3.8b}, [x0], x1
    ld1 {v4.8b,v5.8b,v6.8b,v7.8b}, [x0], x1
    ld1 {v8.8b,v9.8b,v10.8b,v11.8b}, [x0], x1
    ld1 {v12.8b,v13.8b,v14.8b,v15.8b}, [x0], x1

    //load *pred
    //ld1 {v16.8h-v31.8h}, [x2], x3
    ld1 {v16.8b,v17.8b,v18.8b,v19.8b}, [x2], x3
    ld1 {v20.8b,v21.8b,v22.8b,v23.8b}, [x2], x3
    ld1 {v24.8b,v25.8b,v26.8b,v27.8b}, [x2], x3
    ld1 {v28.8b,v29.8b,v30.8b,v31.8b}, [x2], x3

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

    //减
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

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h,v1.8h,v2.8h,v3.8h}, [x4], x5
    st1 {v4.8h,v5.8h,v6.8h,v7.8h}, [x4], x5
    st1 {v8.8h,v9.8h,v10.8h,v11.8h}, [x4], x5
    st1 {v12.8h,v13.8h,v14.8h,v15.8h}, [x4], x5

    sub x6,x6,#4
    cmp x6,#0
    bgt diff_w32_loopx
    b diff_w32_end

diff_w32_end:

    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64

    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_64_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_64_arm64

    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    lsl x5, x5, #1

    sub x1, x1, #32
    sub x3, x3, #32
    sub x5, x5, #64

diff_w64_loopx:

    //load *org
    //ld4 {v0.8h-v15.8h}, [x0], x1
    ld1 {v0.8b,v1.8b,v2.8b,v3.8b}, [x0], #32
    ld1 {v4.8b,v5.8b,v6.8b,v7.8b}, [x0], x1
    ld1 {v8.8b,v9.8b,v10.8b,v11.8b}, [x0], #32
    ld1 {v12.8b,v13.8b,v14.8b,v15.8b}, [x0], x1

    //load *pred
    //ld1 {v16.8h-v31.8h}, [x2], x3
    ld1 {v16.8b,v17.8b,v18.8b,v19.8b}, [x2], #32
    ld1 {v20.8b,v21.8b,v22.8b,v23.8b}, [x2], x3
    ld1 {v24.8b,v25.8b,v26.8b,v27.8b}, [x2], #32
    ld1 {v28.8b,v29.8b,v30.8b,v31.8b}, [x2], x3

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

    //减
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

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h,v1.8h,v2.8h,v3.8h}, [x4], #64
    st1 {v4.8h,v5.8h,v6.8h,v7.8h}, [x4], x5
    st1 {v8.8h,v9.8h,v10.8h,v11.8h}, [x4], #64
    st1 {v12.8h,v13.8h,v14.8h,v15.8h}, [x4], x5

    sub x6,x6,#2
    cmp x6,#0
    bgt diff_w64_loopx
    b diff_w64_end

diff_w64_end:

    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64

    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_128_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_128_arm64

    sub sp, sp, #64
    st1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp]

    lsl x5, x5, #1

    sub x1, x1, #96
    sub x3, x3, #96
    sub x5, x5, #192

diff_w128_loopx:

    //load *org
    //ld4 {v0.8h-v15.8h}, [x0], x1
    ld1 {v0.8b,v1.8b,v2.8b,v3.8b}, [x0], #32
    ld1 {v4.8b,v5.8b,v6.8b,v7.8b}, [x0], #32
    ld1 {v8.8b,v9.8b,v10.8b,v11.8b}, [x0], #32
    ld1 {v12.8b,v13.8b,v14.8b,v15.8b}, [x0], x1

    //load *pred
    //ld1 {v16.8h-v31.8h}, [x2], x3
    ld1 {v16.8b,v17.8b,v18.8b,v19.8b}, [x2], #32
    ld1 {v20.8b,v21.8b,v22.8b,v23.8b}, [x2], #32
    ld1 {v24.8b,v25.8b,v26.8b,v27.8b}, [x2], #32
    ld1 {v28.8b,v29.8b,v30.8b,v31.8b}, [x2], x3

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

    //减
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

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h,v1.8h,v2.8h,v3.8h}, [x4], #64
    st1 {v4.8h,v5.8h,v6.8h,v7.8h}, [x4], #64
    st1 {v8.8h,v9.8h,v10.8h,v11.8h}, [x4], #64
    st1 {v12.8h,v13.8h,v14.8h,v15.8h}, [x4], x5

    sub x6,x6,#1
    cmp x6,#0
    bgt diff_w128_loopx
    b diff_w128_end

diff_w128_end:

    ld1 {v12.8h, v13.8h, v14.8h, v15.8h}, [sp], #64
    ld1 {v8.8h, v9.8h, v10.8h, v11.8h}, [sp], #64

    ret

#else

/******************************************************************************************************
*  void uavs3e_recon_w4_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7, int bit_depth->
******************************************************************************************************/
function uavs3e_recon_w4_arm64
    //max_val = (1 << bit_depth) - 1;
    mov w9, #1
    lsl w9, w9, #10      //10
    sub w9, w9, #1      //max_val
    mov w10, #0

    lsl x2, x2, #1
    lsl x6, x6, #1

    //if (cbf == 0)
    cmp x7, #0
    beq cbf_zero_w4

recon_w4_loopx:
    //load *resi
    ld1 {v0.8h, v1.8h}, [x0], #32

    //load *pred
    ld1 {v2.d}[0], [x1], x2
    ld1 {v2.d}[1], [x1], x2
    ld1 {v3.d}[0], [x1], x2
    ld1 {v3.d}[1], [x1], x2

    //加
    add v0.8h, v0.8h, v2.8h
    add v1.8h, v1.8h, v3.8h

    //clip
    dup v2.8h, w9   //max_val
    dup v3.8h, w10  //0
    smin v0.8h, v0.8h, v2.8h
    smax v0.8h, v0.8h, v3.8h
    smin v1.8h, v1.8h, v2.8h
    smax v1.8h, v1.8h, v3.8h

    //store to blk
    st1 {v0.d}[0], [x5], x6
    st1 {v0.d}[1], [x5], x6
    st1 {v1.d}[0], [x5], x6
    st1 {v1.d}[1], [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w4_loopx
    b recon_w4_end

cbf_zero_w4:
    //load pred
    ld1 {v4.4h}, [x1], x2
    ld1 {v5.4h}, [x1], x2
    ld1 {v6.4h}, [x1], x2
    ld1 {v7.4h}, [x1], x2

    st1 {v4.d}[0], [x5], x6
    st1 {v5.d}[0], [x5], x6
    st1 {v6.d}[0], [x5], x6
    st1 {v7.d}[0], [x5], x6
    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w4

recon_w4_end:

    ret

/******************************************************************************************************
*  void uavs3e_recon_w8_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7, int bit_depth->
******************************************************************************************************/
function uavs3e_recon_w8_arm64
    //max_val = (1 << bit_depth) - 1;
    mov w9, #1
    lsl w9, w9, #10
    sub w9, w9, #1
    mov w10, #0

    lsl x2, x2, #1
    lsl x6, x6, #1

    cmp x7, #0
    beq cbf_zero_w8

recon_w8_loopx:
    //load *resi
    ld1 {v0.8h - v3.8h}, [x0], #64

    //load *pred
    ld1 {v4.8h}, [x1], x2
    ld1 {v5.8h}, [x1], x2
    ld1 {v6.8h}, [x1], x2
    ld1 {v7.8h}, [x1], x2

    add v0.8h, v0.8h, v4.8h
    add v1.8h, v1.8h, v5.8h
    add v2.8h, v2.8h, v6.8h
    add v3.8h, v3.8h, v7.8h

    //clip
    dup v4.8h, w9   //max_val
    dup v5.8h, w10  //0
    smin v0.8h, v0.8h, v4.8h
    smax v0.8h, v0.8h, v5.8h
    smin v1.8h, v1.8h, v4.8h
    smax v1.8h, v1.8h, v5.8h
    smin v2.8h, v2.8h, v4.8h
    smax v2.8h, v2.8h, v5.8h
    smin v3.8h, v3.8h, v4.8h
    smax v3.8h, v3.8h, v5.8h

    //store to blk
    st1 {v0.8h}, [x5], x6
    st1 {v1.8h}, [x5], x6
    st1 {v2.8h}, [x5], x6
    st1 {v3.8h}, [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w8_loopx
    b recon_w8_end

cbf_zero_w8:

    ld1 {v4.8h}, [x1], x2
    ld1 {v5.8h}, [x1], x2
    ld1 {v6.8h}, [x1], x2
    ld1 {v7.8h}, [x1], x2

    st1 {v4.2d}, [x5], x6
    st1 {v5.2d}, [x5], x6
    st1 {v6.2d}, [x5], x6
    st1 {v7.2d}, [x5], x6
    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w8

recon_w8_end:
    ret

/******************************************************************************************************
*  void uavs3e_recon_w16_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7, int bit_depth->
******************************************************************************************************/
function uavs3e_recon_w16_arm64
    //max_val = (1 << bit_depth) - 1;
    //bit_depth = 10
    mov w9, #1
    lsl w9, w9, #10
    sub w9, w9, #1
    mov w10, #0

    lsl x2, x2, #1
    lsl x6, x6, #1

    cmp x7, #0
    beq cbf_zero_w16

recon_w16_loopx:
    ld1 {v0.8h, v1.8h}, [x0], #32
    ld1 {v2.8h, v3.8h}, [x0], #32
    ld1 {v4.8h, v5.8h}, [x0], #32
    ld1 {v6.8h, v7.8h}, [x0], #32

    //load *pred
    ld1 {v16.8h, v17.8h}, [x1], x2
    ld1 {v18.8h, v19.8h}, [x1], x2
    ld1 {v20.8h, v21.8h}, [x1], x2
    ld1 {v22.8h, v23.8h}, [x1], x2

    add v0.8h, v0.8h, v16.8h
    add v1.8h, v1.8h, v17.8h
    add v2.8h, v2.8h, v18.8h
    add v3.8h, v3.8h, v19.8h
    add v4.8h, v4.8h, v20.8h
    add v5.8h, v5.8h, v21.8h
    add v6.8h, v6.8h, v22.8h
    add v7.8h, v7.8h, v23.8h

    //clip
    dup v16.8h, w9   //max_val
    dup v17.8h, w10  //0
    smin v0.8h, v0.8h, v16.8h
    smax v0.8h, v0.8h, v17.8h
    smin v1.8h, v1.8h, v16.8h
    smax v1.8h, v1.8h, v17.8h
    smin v2.8h, v2.8h, v16.8h
    smax v2.8h, v2.8h, v17.8h
    smin v3.8h, v3.8h, v16.8h
    smax v3.8h, v3.8h, v17.8h
    smin v4.8h, v4.8h, v16.8h
    smax v4.8h, v4.8h, v17.8h
    smin v5.8h, v5.8h, v16.8h
    smax v5.8h, v5.8h, v17.8h
    smin v6.8h, v6.8h, v16.8h
    smax v6.8h, v6.8h, v17.8h
    smin v7.8h, v7.8h, v16.8h
    smax v7.8h, v7.8h, v17.8h

    //store to blk
    st1 {v0.8h, v1.8h}, [x5], x6
    st1 {v2.8h, v3.8h}, [x5], x6
    st1 {v4.8h, v5.8h}, [x5], x6
    st1 {v6.8h, v7.8h}, [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w16_loopx
    b recon_w16_end

cbf_zero_w16:

    ld1 {v0.8h,v1.8h}, [x1], x2
    ld1 {v2.8h,v3.8h}, [x1], x2
    ld1 {v4.8h,v5.8h}, [x1], x2
    ld1 {v6.8h,v7.8h}, [x1], x2

    st1 {v0.8h,v1.8h}, [x5], x6
    st1 {v2.8h,v3.8h}, [x5], x6
    st1 {v4.8h,v5.8h}, [x5], x6
    st1 {v6.8h,v7.8h}, [x5], x6
    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w16

recon_w16_end:
    ret

/******************************************************************************************************
*  void uavs3e_recon_w32_arm64(s16 *resi, pel *pred, int i_pred, int width, int height, pel *rec, int i_rec, int cbf, int bit_depth)
*  resi->x0, pred->x1, i_pred->x2, width->x3, height->x4, rec->x5, i_rec->x6, int cbf->x7, int bit_depth->
******************************************************************************************************/
function uavs3e_recon_w32_arm64
    sub sp, sp, #64
    st1 {v8.8h - v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h - v15.8h}, [sp]

    //max_val = (1 << bit_depth) - 1;
    mov w9, #1
    lsl w9, w9, #10
    sub w9, w9, #1
    mov w10, #0

    lsl x2, x2, #1
    lsl x6, x6, #1

    cmp x7, #0
    beq cbf_zero_w32

recon_w32_loopx:
    //load *resi
    ld1 {v0.8h - v3.8h}, [x0], #64
    ld1 {v4.8h - v7.8h}, [x0], #64
    ld1 {v8.8h - v11.8h}, [x0], #64
    ld1 {v12.8h - v15.8h}, [x0], #64

    //load *pred
    ld1 {v16.8h - v19.8h}, [x1], x2
    ld1 {v20.8h - v23.8h}, [x1], x2
    ld1 {v24.8h - v27.8h}, [x1], x2
    ld1 {v28.8h - v31.8h}, [x1], x2

    add v0.8h, v0.8h, v16.8h
    add v1.8h, v1.8h, v17.8h
    add v2.8h, v2.8h, v18.8h
    add v3.8h, v3.8h, v19.8h
    add v4.8h, v4.8h, v20.8h
    add v5.8h, v5.8h, v21.8h
    add v6.8h, v6.8h, v22.8h
    add v7.8h, v7.8h, v23.8h
    add v8.8h, v8.8h, v24.8h
    add v9.8h, v9.8h, v25.8h
    add v10.8h, v10.8h, v26.8h
    add v11.8h, v11.8h, v27.8h
    add v12.8h, v12.8h, v28.8h
    add v13.8h, v13.8h, v29.8h
    add v14.8h, v14.8h, v30.8h
    add v15.8h, v15.8h, v31.8h

    //clip
    dup v16.8h, w9   //max_val
    dup v17.8h, w10  //0
    smin v0.8h, v0.8h, v16.8h
    smax v0.8h, v0.8h, v17.8h
    smin v1.8h, v1.8h, v16.8h
    smax v1.8h, v1.8h, v17.8h
    smin v2.8h, v2.8h, v16.8h
    smax v2.8h, v2.8h, v17.8h
    smin v3.8h, v3.8h, v16.8h
    smax v3.8h, v3.8h, v17.8h
    smin v4.8h, v4.8h, v16.8h
    smax v4.8h, v4.8h, v17.8h
    smin v5.8h, v5.8h, v16.8h
    smax v5.8h, v5.8h, v17.8h
    smin v6.8h, v6.8h, v16.8h
    smax v6.8h, v6.8h, v17.8h
    smin v7.8h, v7.8h, v16.8h
    smax v7.8h, v7.8h, v17.8h
    smin v8.8h, v8.8h, v16.8h
    smax v8.8h, v8.8h, v17.8h
    smin v9.8h, v9.8h, v16.8h
    smax v9.8h, v9.8h, v17.8h
    smin v10.8h, v10.8h, v16.8h
    smax v10.8h, v10.8h, v17.8h
    smin v11.8h, v11.8h, v16.8h
    smax v11.8h, v11.8h, v17.8h
    smin v12.8h, v12.8h, v16.8h
    smax v12.8h, v12.8h, v17.8h
    smin v13.8h, v13.8h, v16.8h
    smax v13.8h, v13.8h, v17.8h
    smin v14.8h, v14.8h, v16.8h
    smax v14.8h, v14.8h, v17.8h
    smin v15.8h, v15.8h, v16.8h
    smax v15.8h, v15.8h, v17.8h

    //store to blk
    st1 {v0.8h - v3.8h}, [x5], x6
    st1 {v4.8h - v7.8h}, [x5], x6
    st1 {v8.8h - v11.8h}, [x5], x6
    st1 {v12.8h - v15.8h}, [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt recon_w32_loopx
    b recon_w32_end

cbf_zero_w32:
    ld1 {v4.8h - v7.8h}, [x1], x2
    ld1 {v8.8h - v11.8h}, [x1], x2
    ld1 {v12.8h - v15.8h}, [x1], x2
    ld1 {v16.8h - v19.8h}, [x1], x2

    st1 {v4.8h - v7.8h}, [x5], x6
    st1 {v8.8h - v11.8h}, [x5], x6
    st1 {v12.8h - v15.8h}, [x5], x6
    st1 {v16.8h - v19.8h}, [x5], x6

    sub x4, x4, #4
    cmp x4, #0
    bgt cbf_zero_w32

recon_w32_end:
    ld1 {v12.8h - v15.8h}, [sp], #64
    ld1 {v8.8h - v11.8h}, [sp], #64
    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_4_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/
function uavs3e_pel_diff_4_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x5, x5, #1

diff_w4_loopx:
    //load *org
    ld1 {v0.d}[0], [x0], x1
    ld1 {v0.d}[1], [x0], x1
    ld1 {v1.d}[0], [x0], x1
    ld1 {v1.d}[1], [x0], x1

    //load *pred
    ld1 {v2.d}[0], [x2], x3
    ld1 {v2.d}[1], [x2], x3
    ld1 {v3.d}[0], [x2], x3
    ld1 {v3.d}[1], [x2], x3

    sub v0.8h, v0.8h, v2.8h
    sub v1.8h, v1.8h, v3.8h

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.d}[0], [x4], x5
    st1 {v0.d}[1], [x4], x5
    st1 {v1.d}[0], [x4], x5
    st1 {v1.d}[1], [x4], x5

    subs x6, x6, #4
    bgt diff_w4_loopx
    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_8_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/
function uavs3e_pel_diff_8_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x5, x5, #1

diff_w8_loopx:
    //load *org
    ld1 {v0.8h}, [x0], x1
    ld1 {v1.8h}, [x0], x1
    ld1 {v2.8h}, [x0], x1
    ld1 {v3.8h}, [x0], x1

    //load *pred
    ld1 {v4.8h}, [x2], x3
    ld1 {v5.8h}, [x2], x3
    ld1 {v6.8h}, [x2], x3
    ld1 {v7.8h}, [x2], x3

    sub v0.8h, v0.8h, v4.8h
    sub v1.8h, v1.8h, v5.8h
    sub v2.8h, v2.8h, v6.8h
    sub v3.8h, v3.8h, v7.8h

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h}, [x4], x5
    st1 {v1.8h}, [x4], x5
    st1 {v2.8h}, [x4], x5
    st1 {v3.8h}, [x4], x5

    subs x6, x6, #4
    bgt diff_w8_loopx
    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_16_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_16_arm64
    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x5, x5, #1

diff_w16_loopx:

    //load *org
    ld1 {v0.8h,v1.8h}, [x0], x1
    ld1 {v2.8h,v3.8h}, [x0], x1
    ld1 {v4.8h,v5.8h}, [x0], x1
    ld1 {v6.8h,v7.8h}, [x0], x1

    //load *pred
    ld1 {v16.8h,v17.8h}, [x2], x3
    ld1 {v18.8h,v19.8h}, [x2], x3
    ld1 {v20.8h,v21.8h}, [x2], x3
    ld1 {v22.8h,v23.8h}, [x2], x3

    sub v0.8h, v0.8h, v16.8h
    sub v1.8h, v1.8h, v17.8h
    sub v2.8h, v2.8h, v18.8h
    sub v3.8h, v3.8h, v19.8h
    sub v4.8h, v4.8h, v20.8h
    sub v5.8h, v5.8h, v21.8h
    sub v6.8h, v6.8h, v22.8h
    sub v7.8h, v7.8h, v23.8h

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h,v1.8h}, [x4], x5
    st1 {v2.8h,v3.8h}, [x4], x5
    st1 {v4.8h,v5.8h}, [x4], x5
    st1 {v6.8h,v7.8h}, [x4], x5

    subs x6, x6, #4
    bgt diff_w16_loopx
    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_32_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_32_arm64
    sub sp, sp, #64
    st1 {v8.8h - v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h - v15.8h}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x5, x5, #1

diff_w32_loopx:
    //load *org
    ld1 {v0.8h - v3.8h}, [x0], x1
    ld1 {v4.8h - v7.8h}, [x0], x1
    ld1 {v8.8h - v11.8h}, [x0], x1
    ld1 {v12.8h - v15.8h}, [x0], x1

    //load *pred
    ld1 {v16.8h - v19.8h}, [x2], x3
    ld1 {v20.8h - v23.8h}, [x2], x3
    ld1 {v24.8h - v27.8h}, [x2], x3
    ld1 {v28.8h - v31.8h}, [x2], x3

    //减
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

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h - v3.8h}, [x4], x5
    st1 {v4.8h - v7.8h}, [x4], x5
    st1 {v8.8h - v11.8h}, [x4], x5
    st1 {v12.8h - v15.8h}, [x4], x5

    subs x6, x6, #4
    bgt diff_w32_loopx

    ld1 {v12.8h - v15.8h}, [sp], #64
    ld1 {v8.8h - v11.8h}, [sp], #64

    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_64_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/

function uavs3e_pel_diff_64_arm64
    sub sp, sp, #64
    st1 {v8.8h - v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h - v15.8h}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x5, x5, #1

    sub x1, x1, #64
    sub x3, x3, #64
    sub x5, x5, #64

diff_w64_loopx:
    //load *org
    ld1 {v0.8h - v3.8h}, [x0], #64
    ld1 {v4.8h - v7.8h}, [x0], x1
    ld1 {v8.8h - v11.8h}, [x0], #64
    ld1 {v12.8h - v15.8h}, [x0], x1

    //load *pred
    ld1 {v16.8h - v19.8h}, [x2], #64
    ld1 {v20.8h - v23.8h}, [x2], x3
    ld1 {v24.8h - v27.8h}, [x2], #64
    ld1 {v28.8h - v31.8h}, [x2], x3

    //减
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

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h - v3.8h}, [x4], #64
    st1 {v4.8h - v7.8h}, [x4], x5
    st1 {v8.8h - v11.8h}, [x4], #64
    st1 {v12.8h - v15.8h}, [x4], x5

    subs x6, x6, #2
    bgt diff_w64_loopx

    ld1 {v12.8h - v15.8h}, [sp], #64
    ld1 {v8.8h - v11.8h}, [sp], #64
    ret

/******************************************************************************************************
*  void uavs3e_pel_diff_128_arm64(pel *p_org, int i_org, pel *p_pred, int i_pred, s16 *p_resi, int i_resi, int height)
*  p_org->x0, i_org->x1, p_pred->x2, i_pred->x3, p_resi->x4, i_resi->x5, height>x6->
******************************************************************************************************/
function uavs3e_pel_diff_128_arm64
    sub sp, sp, #64
    st1 {v8.8h - v11.8h}, [sp]
    sub sp, sp, #64
    st1 {v12.8h - v15.8h}, [sp]

    lsl x1, x1, #1
    lsl x3, x3, #1
    lsl x5, x5, #1
    sub x1, x1, #192
    sub x3, x3, #192
    sub x5, x5, #192

diff_w128_loopx:
    //load *org
    ld1 {v0.8h - v3.8h}, [x0], #64
    ld1 {v4.8h - v7.8h}, [x0], #64
    ld1 {v8.8h - v11.8h}, [x0], #64
    ld1 {v12.8h - v15.8h}, [x0], x1

    //load *pred
    ld1 {v16.8h - v19.8h}, [x2], #64
    ld1 {v20.8h - v23.8h}, [x2], #64
    ld1 {v24.8h - v27.8h}, [x2], #64
    ld1 {v28.8h - v31.8h}, [x2], x3

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

    //p_resi[i] = p_org[i] - p_pred[i]
    st1 {v0.8h - v3.8h}, [x4], #64
    st1 {v4.8h - v7.8h}, [x4], #64
    st1 {v8.8h - v11.8h}, [x4], #64
    st1 {v12.8h - v15.8h}, [x4], x5

    subs x6, x6, #1
    bgt diff_w128_loopx

    ld1 {v12.8h - v15.8h}, [sp], #64
    ld1 {v8.8h - v11.8h}, [sp], #64
    ret

// void uavs3e_pel_avrg_4_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height)
// dst->x0, i_dst->x1, src1->x2, src2->x3, height->x4
function uavs3e_pel_avrg_4_arm64
    lsl x1, x1, #1

avg_pel_w4_y:
    ld1 {v0.8h, v1.8h}, [x2], #32
    ld1 {v2.8h, v3.8h}, [x3], #32

    urhadd v0.8h, v0.8h, v2.8h
    urhadd v1.8h, v1.8h, v3.8h

    st1  {v0.d}[0], [x0], x1
    st1  {v0.d}[1], [x0], x1
    st1  {v1.d}[0], [x0], x1
    st1  {v1.d}[1], [x0], x1

    subs w4, w4, #4
    bgt avg_pel_w4_y

    ret


// void uavs3e_pel_avrg_8_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
// dst->x0, i_dst->x1, src1->x2, src2->x3, height->x4
function uavs3e_pel_avrg_8_arm64
    lsl x1, x1, #1

avg_pel_w8_y:
    ld1 {v0.8h - v3.8h}, [x2], #64
    ld1 {v4.8h - v7.8h}, [x3], #64

    urhadd v0.8h, v0.8h, v4.8h
    urhadd v1.8h, v1.8h, v5.8h
    urhadd v2.8h, v2.8h, v6.8h
    urhadd v3.8h, v3.8h, v7.8h

    subs w4, w4, #4
    st1  {v0.8h}, [x0], x1
    st1  {v1.8h}, [x0], x1
    st1  {v2.8h}, [x0], x1
    st1  {v3.8h}, [x0], x1

    bgt avg_pel_w8_y

    ret

// void uavs3e_pel_avrg_16_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
// dst->x0, i_dst->x1, src1->x2, src2->x3, height->x4
function uavs3e_pel_avrg_16_arm64
    lsl x1, x1, #1
avg_pel_w16_y:
    ld1 {v0.8h - v3.8h}, [x2], #64
    ld1 {v4.8h - v7.8h}, [x2], #64

    ld1 {v20.8h - v23.8h}, [x3], #64
    ld1 {v24.8h - v27.8h}, [x3], #64

    urhadd v0.8h, v0.8h, v20.8h
    urhadd v1.8h, v1.8h, v21.8h
    urhadd v2.8h, v2.8h, v22.8h
    urhadd v3.8h, v3.8h, v23.8h
    urhadd v4.8h, v4.8h, v24.8h
    urhadd v5.8h, v5.8h, v25.8h
    urhadd v6.8h, v6.8h, v26.8h
    urhadd v7.8h, v7.8h, v27.8h

    subs w4, w4, #4
    st1  {v0.8h, v1.8h}, [x0], x1
    st1  {v2.8h, v3.8h}, [x0], x1
    st1  {v4.8h, v5.8h}, [x0], x1
    st1  {v6.8h, v7.8h}, [x0], x1

    bgt avg_pel_w16_y

    ret

// void uavs3e_pel_avrg_32_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
// dst->x0, i_dst->x1, src1->x2, src2->x3, height->x4
function uavs3e_pel_avrg_32_arm64
    lsl x1, x1, #1
avg_pel_w32_y:
    ld1 {v0.8h - v3.8h}, [x2], #64
    ld1 {v4.8h - v7.8h}, [x2], #64

    ld1 {v20.8h - v23.8h}, [x3], #64
    ld1 {v24.8h - v27.8h}, [x3], #64

    urhadd v0.8h, v0.8h, v20.8h
    urhadd v1.8h, v1.8h, v21.8h
    urhadd v2.8h, v2.8h, v22.8h
    urhadd v3.8h, v3.8h, v23.8h
    urhadd v4.8h, v4.8h, v24.8h
    urhadd v5.8h, v5.8h, v25.8h
    urhadd v6.8h, v6.8h, v26.8h
    urhadd v7.8h, v7.8h, v27.8h

    subs w4, w4, #2
    st1  {v0.8h - v3.8h}, [x0], x1
    st1  {v4.8h - v7.8h}, [x0], x1
    bgt avg_pel_w32_y

    ret

// void uavs3e_pel_avrg_64_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
// dst->x0, i_dst->x1, src1->x2, src2->x3, height->x4
function uavs3e_pel_avrg_64_arm64
    lsl x1, x1, #1
    sub x1, x1, #64
avg_pel_w64_y:
    ld1 {v0.8h - v3.8h}, [x2], #64
    ld1 {v4.8h - v7.8h}, [x2], #64

    ld1 {v20.8h - v23.8h}, [x3], #64
    ld1 {v24.8h - v27.8h}, [x3], #64

    urhadd v0.8h, v0.8h, v20.8h
    urhadd v1.8h, v1.8h, v21.8h
    urhadd v2.8h, v2.8h, v22.8h
    urhadd v3.8h, v3.8h, v23.8h
    urhadd v4.8h, v4.8h, v24.8h
    urhadd v5.8h, v5.8h, v25.8h
    urhadd v6.8h, v6.8h, v26.8h
    urhadd v7.8h, v7.8h, v27.8h

    subs w4, w4, #1
    st1  {v0.8h - v3.8h}, [x0], #64
    st1  {v4.8h - v7.8h}, [x0], x1
    bgt avg_pel_w64_y

    ret


// void uavs3e_pel_avrg_128_arm64(pel *dst, int i_dst, pel *src1, pel *src2, int height);
// dst->x0, i_dst->x1, src1->x2, src2->x3, height->x4
function uavs3e_pel_avrg_128_arm64
    lsl x1, x1, #1
    sub x1, x1, #192

avg_pel_w128_y:
    ld1 {v0.8h - v3.8h}, [x2], #64
    ld1 {v4.8h - v7.8h}, [x2], #64

    ld1 {v20.8h - v23.8h}, [x3], #64
    ld1 {v24.8h - v27.8h}, [x3], #64

    urhadd v0.8h, v0.8h, v20.8h
    urhadd v1.8h, v1.8h, v21.8h
    urhadd v2.8h, v2.8h, v22.8h
    urhadd v3.8h, v3.8h, v23.8h
    urhadd v4.8h, v4.8h, v24.8h
    urhadd v5.8h, v5.8h, v25.8h
    urhadd v6.8h, v6.8h, v26.8h
    urhadd v7.8h, v7.8h, v27.8h

    st1 {v0.8h - v3.8h}, [x0], #64
    st1 {v4.8h - v7.8h}, [x0], #64

    ld1 {v16.8h - v19.8h}, [x2], #64
    ld1 {v28.8h - v31.8h}, [x2], #64

    ld1 {v20.8h - v23.8h}, [x3], #64
    ld1 {v24.8h - v27.8h}, [x3], #64

    urhadd v16.8h, v16.8h, v20.8h
    urhadd v17.8h, v17.8h, v21.8h
    urhadd v18.8h, v18.8h, v22.8h
    urhadd v19.8h, v19.8h, v23.8h
    urhadd v28.8h, v28.8h, v24.8h
    urhadd v29.8h, v29.8h, v25.8h
    urhadd v30.8h, v30.8h, v26.8h
    urhadd v31.8h, v31.8h, v27.8h

    st1  {v16.8h - v19.8h}, [x0], #64
    st1  {v28.8h - v31.8h}, [x0], x1

    subs w4, w4, #1
    bgt avg_pel_w128_y

    ret
#endif

#endif
