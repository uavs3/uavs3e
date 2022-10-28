#include "def_arm64.S"

#if defined(__arm64__)

//*************************************************************************************************
//void uavs3e_itrans_dct2_h4_w4_arm64(s16 *src, s16 *dst, int bit_depth);
//x0->coeff blk, 16 bit
//x1->resi blk, 16 bit
//x2->bit_depth
//*************************************************************************************************
function uavs3e_itrans_dct2_h4_w4_arm64
    mov w8, #1
    lsl w8, w8, w2
    sub w4, w2, #20                 // -shift = bit_depth - 20
    sub w9, w8, #1                  // max_pel = (1<<bit_depth) - 1
    neg w8, w8                      // min_pel = -(1<<bit_depth)
    dup v31.4s, w4                  // for left shift

	//load parameters
	ld1	{v0.4h, v1.4h, v2.4h, v3.4h},	[x0]
    mov w5, #32
    mov w6, #42
    mov w7, #17
    mov v4.h[0], w5
	mov v4.h[2], w6
	mov v4.h[3], w7

	/*---first butterfly---*/
	//O[0]
	smull v16.4s, v1.4h, v4.h[2]
	smlal v16.4s, v3.4h, v4.h[3]
	//O[1]
	smull v17.4s, v1.4h, v4.h[3]
	smlsl v17.4s, v3.4h, v4.h[2]
	//E[0]
	smull v18.4s, v0.4h, v4.h[0]
	smlal v18.4s, v2.4h, v4.h[0]
	//E[1]
	smull v19.4s, v0.4h, v4.h[0]
	smlsl v19.4s, v2.4h, v4.h[0]

	sqadd v20.4s, v16.4s, v18.4s
    sqadd v21.4s, v19.4s, v17.4s
    sqsub v22.4s, v19.4s, v17.4s
    sqsub v23.4s, v18.4s, v16.4s

	sqrshrn v0.4h, v20.4s, #5
	sqrshrn v1.4h, v21.4s, #5
	sqrshrn v2.4h, v22.4s, #5
	sqrshrn v3.4h, v23.4s, #5

	//transpose
    trn1 v5.2s, v0.2s, v2.2s
	trn2 v7.2s, v0.2s, v2.2s
	trn1 v6.2s, v1.2s, v3.2s
	trn2 v18.2s, v1.2s, v3.2s
	trn1 v0.4h, v5.4h, v6.4h
	trn2 v1.4h, v5.4h, v6.4h
	trn1 v2.4h, v7.4h, v18.4h
	trn2 v3.4h, v7.4h, v18.4h

	/*---second butterfly---*/
	//O[0]
	smull v16.4s, v1.4h, v4.H[2]
	smlal v16.4s, v3.4h, v4.H[3]
	//O[1]
	smull v17.4s, v1.4h, v4.H[3]
	smlsl v17.4s, v3.4h, v4.H[2]
	//E[0]
	smull v18.4s, v0.4h, v4.H[0]
	smlal v18.4s, v2.4h, v4.H[0]
	//E[1]
	smull v19.4s, v0.4h, v4.H[0]
	smlsl v19.4s, v2.4h, v4.H[0]

	sqadd v20.4s, v16.4s, v18.4s
    sqadd v21.4s, v19.4s, v17.4s
    sqsub v22.4s, v19.4s, v17.4s
    sqsub v23.4s, v18.4s, v16.4s

    srshl v0.4s, v20.4s, v31.4s
    srshl v1.4s, v21.4s, v31.4s
    srshl v2.4s, v22.4s, v31.4s
    srshl v3.4s, v23.4s, v31.4s

    sqxtn v0.4h, v0.4s
    sqxtn v1.4h, v1.4s
    sqxtn v2.4h, v2.4s
    sqxtn v3.4h, v3.4s

    //transpose
    trn1 v5.2s, v0.2s, v2.2s
    trn2 v7.2s, v0.2s, v2.2s
    trn1 v6.2s, v1.2s, v3.2s
    trn2 v18.2s, v1.2s, v3.2s
    trn1 v0.4h, v5.4h, v6.4h
    trn2 v1.4h, v5.4h, v6.4h
    trn1 v2.4h, v7.4h, v18.4h
    trn2 v3.4h, v7.4h, v18.4h

    //clip
	dup v5.4h, w8               //minval
	dup v6.4h, w9               //maxval
	smin v0.4h, v0.4h, v6.4h
	smax v0.4h, v0.4h, v5.4h
	smin v1.4h, v1.4h, v6.4h
	smax v1.4h, v1.4h, v5.4h
	smin v2.4h, v2.4h, v6.4h
	smax v2.4h, v2.4h, v5.4h
	smin v3.4h, v3.4h, v6.4h
	smax v3.4h, v3.4h, v5.4h

	/*---add & store to blk---*/

    st1 {v0.4h, v1.4h, v2.4h, v3.4h}, [x1]

ret

//*************************************************************************************************
//void dct2_butterfly_h4_arm64(s16 *src, s16 *dst, int width, int shift, int bit_depth);
//x0: coeff blk, 16 bit
//x1: resi blk, 16 bit
//x2: blk width
//x3: shift
//x4: bit_depth
//*************************************************************************************************
function dct2_butterfly_h4_arm64
    //transcode coeffs
    mov w5, #32
    mov w6, #42
    mov w7, #17
    mov v4.h[0], w5
    mov v4.h[2], w6
    mov v4.h[3], w7

    mov w8, #1
    lsl w8, w8, w4
    sub w5, w4, #20                 // -shift = bit_depth - 20
    sub w9, w8, #1                  // max_pel = (1<<bit_depth) - 1
    neg w8, w8                      // min_pel = -(1<<bit_depth)
    dup v31.4s, w5                  // for left shift
    dup v5.4h, w8                   // min_pel
    dup v6.4h, w9                   // max_pel

    mov x8, #0                      // i = 0
    lsl x2, x2, #1                  // width * sizeof(s16)
dct2_h4_loopx:
    add x9, x0, x8
    ld1 {v0.4h}, [x9], x2
    ld1 {v1.4h}, [x9], x2
    ld1 {v2.4h}, [x9], x2
    ld1 {v3.4h}, [x9], x2

    //O[0]
    smull v16.4s, v1.4h, v4.h[2]
    smlal v16.4s, v3.4h, v4.h[3]
    //O[1]
    smull v17.4s, v1.4h, v4.h[3]
    smlsl v17.4s, v3.4h, v4.h[2]
    //E[0]
    smull v18.4s, v0.4h, v4.h[0]
    smlal v18.4s, v2.4h, v4.h[0]
    //E[1]
    smull v19.4s, v0.4h, v4.h[0]
    smlsl v19.4s, v2.4h, v4.h[0]

    sqadd v20.4s, v16.4s, v18.4s
    sqadd v21.4s, v19.4s, v17.4s
    sqsub v22.4s, v19.4s, v17.4s
    sqsub v23.4s, v18.4s, v16.4s

    cmp w4, #15
    bne dct2_h4_2nd_shift_clip
    sqrshrn v0.4h, v20.4s, #5
    sqrshrn v1.4h, v21.4s, #5
    sqrshrn v2.4h, v22.4s, #5
    sqrshrn v3.4h, v23.4s, #5
    b dct2_h4_store

dct2_h4_2nd_shift_clip:    // second transform
    srshl v0.4s, v20.4s, v31.4s
    srshl v1.4s, v21.4s, v31.4s
    srshl v2.4s, v22.4s, v31.4s
    srshl v3.4s, v23.4s, v31.4s

    sqxtn v0.4h, v0.4s
    sqxtn v1.4h, v1.4s
    sqxtn v2.4h, v2.4s
    sqxtn v3.4h, v3.4s

    smin v0.4h, v0.4h, v6.4h
    smax v0.4h, v0.4h, v5.4h
    smin v1.4h, v1.4h, v6.4h
    smax v1.4h, v1.4h, v5.4h
    smin v2.4h, v2.4h, v6.4h
    smax v2.4h, v2.4h, v5.4h
    smin v3.4h, v3.4h, v6.4h
    smax v3.4h, v3.4h, v5.4h

dct2_h4_store:
    //transpose
    trn1 v20.2s, v0.2s, v2.2s
    trn2 v22.2s, v0.2s, v2.2s
    trn1 v21.2s, v1.2s, v3.2s
    trn2 v23.2s, v1.2s, v3.2s
    trn1 v0.4h, v20.4h, v21.4h
    trn2 v1.4h, v20.4h, v21.4h
    trn1 v2.4h, v22.4h, v23.4h
    trn2 v3.4h, v22.4h, v23.4h

    add x8, x8, #8      // i += 4 * sizeof(s16)

    st1 {v0.4h, v1.4h, v2.4h, v3.4h}, [x1], #32

    cmp x8, x2
    blt dct2_h4_loopx

    ret


//************************************************************************
//void dct2_butterfly_h8_arm64(s16 *src, int i_src, s16 *dst, int width, int shift, int bit_depth);
//x0: coeff blk, 16 bit
//x1: i_src
//x2: resi blk, 16 bit
//x3: blk width
//x4: shift
//x5: bit_depth
//************************************************************************

function dct2_butterfly_h8_arm64

    sub sp, sp, #16
    st1 {v8.8h}, [sp]   // protect v8

    // set transform coeffs
    mov w9, #32
    mov w6, #17
    mov w7, #42
    mov v8.h[0], w9
    mov v8.h[1], w9
    mov v8.h[2], w6
    mov v8.h[3], w7
    mov w9, #9
    mov w6, #25
    mov w7, #38
    mov w8, #44
    mov v8.h[4], w9
    mov v8.h[5], w6
    mov v8.h[6], w7
    mov v8.h[7], w8

    mov w8, #1
    lsl w8, w8, w5
    sub w6, w5, #20             // -shift = bit_depth - 20
    sub w9, w8, #1              // max_pel = (1<<bit_depth) - 1
    neg w8, w8                  // min_pel = -(1<<bit_depth)
    dup v31.4s, w6              // for left shift
    dup v25.8h, w8              // min_pel
    dup v26.8h, w9              // max_pel

    mov x8, #0                  // i = 0
    lsl x1, x1, #1              // i_src *= sizeof(s16)
    lsl x3, x3, #1              // width *= sizeof(s16)
dct2_h8_loopx:
    add x9, x0, x8
    ld1 {v0.4h}, [x9], x1
    ld1 {v1.4h}, [x9], x1
    ld1 {v2.4h}, [x9], x1
    ld1 {v3.4h}, [x9], x1
    ld1 {v4.4h}, [x9], x1
    ld1 {v5.4h}, [x9], x1
    ld1 {v6.4h}, [x9], x1
    ld1 {v7.4h}, [x9], x1

    //E[0]
    smull v16.4s, v0.4h, v8.H[0]
    smlal v16.4s, v2.4h, v8.H[3]
    smlal v16.4s, v4.4h, v8.H[0]
    smlal v16.4s, v6.4h, v8.H[2]
    //E[1]
    smull v17.4s, v0.4h, v8.H[0]
    smlal v17.4s, v2.4h, v8.H[2]
    smlsl v17.4s, v4.4h, v8.H[0]
    smlsl v17.4s, v6.4h, v8.H[3]
    //E[2]
    smull v18.4s, v0.4h, v8.H[0]
    smlsl v18.4s, v2.4h, v8.H[2]
    smlsl v18.4s, v4.4h, v8.H[0]
    smlal v18.4s, v6.4h, v8.H[3]
    //E[3]
    smull v19.4s, v0.4h, v8.H[0]
    smlsl v19.4s, v2.4h, v8.H[3]
    smlal v19.4s, v4.4h, v8.H[0]
    smlsl v19.4s, v6.4h, v8.H[2]

    //O[0]
    smull v20.4s, v1.4h, v8.H[7]
    smlal v20.4s, v3.4h, v8.H[6]
    smlal v20.4s, v5.4h, v8.H[5]
    smlal v20.4s, v7.4h, v8.H[4]
    //O[1]
    smull v21.4s, v1.4h, v8.H[6]
    smlsl v21.4s, v3.4h, v8.H[4]
    smlsl v21.4s, v5.4h, v8.H[7]
    smlsl v21.4s, v7.4h, v8.H[5]
    //O[2]
    smull v22.4s, v1.4h, v8.H[5]
    smlsl v22.4s, v3.4h, v8.H[7]
    smlal v22.4s, v5.4h, v8.H[4]
    smlal v22.4s, v7.4h, v8.H[6]
    //O[3]
    smull v23.4s, v1.4h, v8.H[4]
    smlsl v23.4s, v3.4h, v8.H[5]
    smlal v23.4s, v5.4h, v8.H[6]
    smlsl v23.4s, v7.4h, v8.H[7]

    //CALCULATE DST
    add v0.4s, v16.4s, v20.4s   //DST[0]
    add v1.4s, v17.4s, v21.4s   //DST[1]
    add v2.4s, v18.4s, v22.4s   //DST[2]
    add v3.4s, v19.4s, v23.4s   //DST[3]
    sub v4.4s, v19.4s, v23.4s   //DST[4]
    sub v5.4s, v18.4s, v22.4s   //DST[5]
    sub v6.4s, v17.4s, v21.4s   //DST[6]
    sub v7.4s, v16.4s, v20.4s   //DST[7]

    cmp w5, #15
    bne dct2_h8_2nd_shift_clip
    sqrshrn v0.4h, v0.4s, #5
    sqrshrn v1.4h, v1.4s, #5
    sqrshrn v2.4h, v2.4s, #5
    sqrshrn v3.4h, v3.4s, #5
    sqrshrn v4.4h, v4.4s, #5
    sqrshrn v5.4h, v5.4s, #5
    sqrshrn v6.4h, v6.4s, #5
    sqrshrn v7.4h, v7.4s, #5

    trn1 v16.2s, v0.2s, v2.2s
    trn1 v17.2s, v1.2s, v3.2s
    trn2 v18.2s, v0.2s, v2.2s
    trn2 v19.2s, v1.2s, v3.2s
    trn1 v20.2s, v4.2s, v6.2s
    trn1 v21.2s, v5.2s, v7.2s
    trn2 v22.2s, v4.2s, v6.2s
    trn2 v23.2s, v5.2s, v7.2s

    trn1 v0.4h, v16.4h, v17.4h
    trn2 v1.4h, v16.4h, v17.4h
    trn1 v2.4h, v18.4h, v19.4h
    trn2 v3.4h, v18.4h, v19.4h
    trn1 v4.4h, v20.4h, v21.4h
    trn2 v5.4h, v20.4h, v21.4h
    trn1 v6.4h, v22.4h, v23.4h
    trn2 v7.4h, v22.4h, v23.4h

    trn1 v0.2d, v0.2d, v4.2d
    trn1 v1.2d, v1.2d, v5.2d
    trn1 v2.2d, v2.2d, v6.2d
    trn1 v3.2d, v3.2d, v7.2d
    b dct2_h8_store
dct2_h8_2nd_shift_clip:
    srshl v0.4s, v0.4s, v31.4s
    srshl v1.4s, v1.4s, v31.4s
    srshl v2.4s, v2.4s, v31.4s
    srshl v3.4s, v3.4s, v31.4s
    srshl v4.4s, v4.4s, v31.4s
    srshl v5.4s, v5.4s, v31.4s
    srshl v6.4s, v6.4s, v31.4s
    srshl v7.4s, v7.4s, v31.4s

    sqxtn v0.4h, v0.4s
    sqxtn v1.4h, v1.4s
    sqxtn v2.4h, v2.4s
    sqxtn v3.4h, v3.4s
    sqxtn v4.4h, v4.4s
    sqxtn v5.4h, v5.4s
    sqxtn v6.4h, v6.4s
    sqxtn v7.4h, v7.4s

    trn1 v16.2s, v0.2s, v2.2s
    trn1 v17.2s, v1.2s, v3.2s
    trn2 v18.2s, v0.2s, v2.2s
    trn2 v19.2s, v1.2s, v3.2s
    trn1 v20.2s, v4.2s, v6.2s
    trn1 v21.2s, v5.2s, v7.2s
    trn2 v22.2s, v4.2s, v6.2s
    trn2 v23.2s, v5.2s, v7.2s

    trn1 v0.4h, v16.4h, v17.4h
    trn2 v1.4h, v16.4h, v17.4h
    trn1 v2.4h, v18.4h, v19.4h
    trn2 v3.4h, v18.4h, v19.4h
    trn1 v4.4h, v20.4h, v21.4h
    trn2 v5.4h, v20.4h, v21.4h
    trn1 v6.4h, v22.4h, v23.4h
    trn2 v7.4h, v22.4h, v23.4h

    trn1 v0.2d, v0.2d, v4.2d
    trn1 v1.2d, v1.2d, v5.2d
    trn1 v2.2d, v2.2d, v6.2d
    trn1 v3.2d, v3.2d, v7.2d

    smax v0.8h, v0.8h, v25.8h
    smax v1.8h, v1.8h, v25.8h
    smax v2.8h, v2.8h, v25.8h
    smax v3.8h, v3.8h, v25.8h
    smin v0.8h, v0.8h, v26.8h
    smin v1.8h, v1.8h, v26.8h
    smin v2.8h, v2.8h, v26.8h
    smin v3.8h, v3.8h, v26.8h

dct2_h8_store:
    add x8, x8, #8      // i += 4 * sizeof(s16)

    st1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x2], #64

    cmp x8, x3
    blt dct2_h8_loopx

    ld1 {v8.8h}, [sp]   // protect v8
    add sp, sp, #16

    ret

//************************************************************************
//void dct2_butterfly_h16_arm64(s16 *src, int i_src, s16 *dst, int width, int shift, int bit_depth);
//x0: coeff blk, 16 bit
//x1: i_src
//x2: resi blk, 16 bit
//x3: blk width
//x4: shift
//x5: bit_depth
//************************************************************************

function dct2_butterfly_h16_arm64

    //protect v8,v9
    sub sp, sp, #32
    st1 {v8.8h, v9.8h}, [sp]

    mov w13 , #9
    mov w6 , #25
    mov w7 , #38
    mov w8 , #44
    mov w9 , #32
    mov w10, #32
    mov w11, #17
    mov w12, #42
    mov v8.h[0], w13
    mov v8.h[1], w6
    mov v8.h[2], w7
    mov v8.h[3], w8
    mov v8.h[4], w9
    mov v8.h[5], w10
    mov v8.h[6], w11
    mov v8.h[7], w12

    mov w13 , #4
    mov w6 , #13
    mov w7 , #21
    mov w8 , #29
    mov w9 , #35
    mov w10, #40
    mov w11, #43
    mov w12, #45
    mov v9.h[0], w13
    mov v9.h[1], w6
    mov v9.h[2], w7
    mov v9.h[3], w8
    mov v9.h[4], w9
    mov v9.h[5], w10
    mov v9.h[6], w11
    mov v9.h[7], w12

    cmp w5, #15
    bne dct2_h16_2nd_transform

    lsl x9, x1, #2                  // i_src * 2 * sizeof(s16)
    mov x8, #0                      // i = 0
    lsl x1, x1, #1                  // i_src * sizeof(s16)
    lsl x3, x3, #1                  // width * 2
dct2_h16_1st_loopx:
    add x10, x0, x8
    ld1 {v0.4h}, [x10], x9          //SRC[0*i_src]
    ld1 {v1.4h}, [x10], x9          //SRC[2*i_src]
    ld1 {v2.4h}, [x10], x9          //SRC[4*i_src]
    ld1 {v3.4h}, [x10], x9          //SRC[6*i_src]
    ld1 {v4.4h}, [x10], x9          //SRC[8*i_src]
    ld1 {v5.4h}, [x10], x9          //SRC[10*i_src]
    ld1 {v6.4h}, [x10], x9          //SRC[12*i_src]
    ld1 {v7.4h}, [x10], x9          //SRC[14*i_src]

    smull v16.4s, v1.4h, v8.H[3]
    smlal v16.4s, v3.4h, v8.H[2]
    smlal v16.4s, v5.4h, v8.H[1]
    smlal v16.4s, v7.4h, v8.H[0]    //EO[0]

    smull v17.4s, v1.4h, v8.H[2]
    smlsl v17.4s, v3.4h, v8.H[0]
    smlsl v17.4s, v5.4h, v8.H[3]
    smlsl v17.4s, v7.4h, v8.H[1]    //EO[1]

    smull v18.4s, v1.4h, v8.H[1]
    smlsl v18.4s, v3.4h, v8.H[3]
    smlal v18.4s, v5.4h, v8.H[0]
    smlal v18.4s, v7.4h, v8.H[2]    //EO[2]

    smull v19.4s, v1.4h, v8.H[0]
    smlsl v19.4s, v3.4h, v8.H[1]
    smlal v19.4s, v5.4h, v8.H[2]
    smlsl v19.4s, v7.4h, v8.H[3]    //EO[3]

    smull v20.4s, v2.4h, v8.H[7]
    smlal v20.4s, v6.4h, v8.H[6]    //EEO[0]
    smull v21.4s, v0.4h, v8.H[4]
    smlal v21.4s, v4.4h, v8.H[4]    //EEE[0]
    smull v22.4s, v2.4h, v8.H[6]
    smlsl v22.4s, v6.4h, v8.H[7]    //EEO[1]
    smull v23.4s, v0.4h, v8.H[4]
    smlsl v23.4s, v4.4h, v8.H[4]    //EEE[1]

    add v24.4s, v21.4s, v20.4s      //EE[0]
    add v25.4s, v23.4s, v22.4s      //EE[1]
    sub v26.4s, v23.4s, v22.4s      //EE[2]
    sub v27.4s, v21.4s, v20.4s      //EE[3]

    add v20.4s, v24.4s, v16.4s      //E[0]
    add v21.4s, v25.4s, v17.4s      //E[1]
    add v22.4s, v26.4s, v18.4s      //E[2]
    add v23.4s, v27.4s, v19.4s      //E[3]
    sub v28.4s, v27.4s, v19.4s      //E[4]
    sub v29.4s, v26.4s, v18.4s      //E[5]
    sub v30.4s, v25.4s, v17.4s      //E[6]
    sub v31.4s, v24.4s, v16.4s      //E[7]

    add x10, x0, x8
    add x10, x10, x1
    ld1 {v0.4h}, [x10], x9          //SRC[1*LINE]
    ld1 {v1.4h}, [x10], x9          //SRC[3*LINE]
    ld1 {v2.4h}, [x10], x9          //SRC[5*LINE]
    ld1 {v3.4h}, [x10], x9          //SRC[7*LINE]
    ld1 {v4.4h}, [x10], x9          //SRC[9*LINE]
    ld1 {v5.4h}, [x10], x9          //SRC[11*LINE]
    ld1 {v6.4h}, [x10], x9          //SRC[13*LINE]
    ld1 {v7.4h}, [x10], x9          //SRC[15*LINE]

    //O[0]
    smull v16.4s, v0.4h, v9.h[7]
    smlal v16.4s, v1.4h, v9.h[6]
    smlal v16.4s, v2.4h, v9.h[5]
    smlal v16.4s, v3.4h, v9.h[4]
    smlal v16.4s, v4.4h, v9.h[3]
    smlal v16.4s, v5.4h, v9.h[2]
    smlal v16.4s, v6.4h, v9.h[1]
    smlal v16.4s, v7.4h, v9.h[0]
    //O[1]
    smull v17.4s, v0.4h, v9.h[6]
    smlal v17.4s, v1.4h, v9.h[3]
    smlal v17.4s, v2.4h, v9.h[0]
    smlsl v17.4s, v3.4h, v9.h[2]
    smlsl v17.4s, v4.4h, v9.h[5]
    smlsl v17.4s, v5.4h, v9.h[7]
    smlsl v17.4s, v6.4h, v9.h[4]
    smlsl v17.4s, v7.4h, v9.h[1]
    //O[2]
    smull v18.4s, v0.4h, v9.h[5]
    smlal v18.4s, v1.4h, v9.h[0]
    smlsl v18.4s, v2.4h, v9.h[4]
    smlsl v18.4s, v3.4h, v9.h[6]
    smlsl v18.4s, v4.4h, v9.h[1]
    smlal v18.4s, v5.4h, v9.h[3]
    smlal v18.4s, v6.4h, v9.h[7]
    smlal v18.4s, v7.4h, v9.h[2]
    //O[3]
    smull v19.4s, v0.4h, v9.h[4]
    smlsl v19.4s, v1.4h, v9.h[2]
    smlsl v19.4s, v2.4h, v9.h[6]
    smlal v19.4s, v3.4h, v9.h[0]
    smlal v19.4s, v4.4h, v9.h[7]
    smlal v19.4s, v5.4h, v9.h[1]
    smlsl v19.4s, v6.4h, v9.h[5]
    smlsl v19.4s, v7.4h, v9.h[3]
    //O[4]
    smull v24.4s, v0.4h, v9.h[3]
    smlsl v24.4s, v1.4h, v9.h[5]
    smlsl v24.4s, v2.4h, v9.h[1]
    smlal v24.4s, v3.4h, v9.h[7]
    smlsl v24.4s, v4.4h, v9.h[0]
    smlsl v24.4s, v5.4h, v9.h[6]
    smlal v24.4s, v6.4h, v9.h[2]
    smlal v24.4s, v7.4h, v9.h[4]
    //O[5]
    smull v25.4s, v0.4h, v9.h[2]
    smlsl v25.4s, v1.4h, v9.h[7]
    smlal v25.4s, v2.4h, v9.h[3]
    smlal v25.4s, v3.4h, v9.h[1]
    smlsl v25.4s, v4.4h, v9.h[6]
    smlal v25.4s, v5.4h, v9.h[4]
    smlal v25.4s, v6.4h, v9.h[0]
    smlsl v25.4s, v7.4h, v9.h[5]
    //O[6]
    smull v26.4s, v0.4h, v9.h[1]
    smlsl v26.4s, v1.4h, v9.h[4]
    smlal v26.4s, v2.4h, v9.h[7]
    smlsl v26.4s, v3.4h, v9.h[5]
    smlal v26.4s, v4.4h, v9.h[2]
    smlal v26.4s, v5.4h, v9.h[0]
    smlsl v26.4s, v6.4h, v9.h[3]
    smlal v26.4s, v7.4h, v9.h[6]
    //O[7]
    smull v27.4s, v0.4h, v9.h[0]
    smlsl v27.4s, v1.4h, v9.h[1]
    smlal v27.4s, v2.4h, v9.h[2]
    smlsl v27.4s, v3.4h, v9.h[3]
    smlal v27.4s, v4.4h, v9.h[4]
    smlsl v27.4s, v5.4h, v9.h[5]
    smlal v27.4s, v6.4h, v9.h[6]
    smlsl v27.4s, v7.4h, v9.h[7]

    add v0.4s, v16.4s, v20.4s       //DST[0]
    add v1.4s, v17.4s, v21.4s       //DST[1]
    add v2.4s, v18.4s, v22.4s       //DST[2]
    add v3.4s, v19.4s, v23.4s       //DST[3]
    sub v7.4s, v20.4s, v16.4s       //DST[15]
    sub v6.4s, v21.4s, v17.4s       //DST[14]
    sub v5.4s, v22.4s, v18.4s       //DST[13]
    sub v4.4s, v23.4s, v19.4s       //DST[12]

    sqrshrn v0.4h, v0.4s, #5
    sqrshrn v1.4h, v1.4s, #5
    sqrshrn v2.4h, v2.4s, #5
    sqrshrn v3.4h, v3.4s, #5
    sqrshrn v4.4h, v4.4s, #5
    sqrshrn v5.4h, v5.4s, #5
    sqrshrn v6.4h, v6.4s, #5
    sqrshrn v7.4h, v7.4s, #5

    trn1 v16.2s, v0.2s, v2.2s
    trn2 v18.2s, v0.2s, v2.2s
    trn1 v17.2s, v1.2s, v3.2s
    trn2 v19.2s, v1.2s, v3.2s
    trn1 v0.4h, v16.4h, v17.4h
    trn2 v1.4h, v16.4h, v17.4h
    trn1 v2.4h, v18.4h, v19.4h
    trn2 v3.4h, v18.4h, v19.4h

    trn1 v16.2s, v4.2s, v6.2s
    trn2 v18.2s, v4.2s, v6.2s
    trn1 v17.2s, v5.2s, v7.2s
    trn2 v19.2s, v5.2s, v7.2s
    trn1 v4.4h, v16.4h, v17.4h
    trn2 v5.4h, v16.4h, v17.4h
    trn1 v6.4h, v18.4h, v19.4h
    trn2 v7.4h, v18.4h, v19.4h

    mov x11, x2
    add x12, x2, #24
    mov x13, #32
    st1 {v0.4h}, [x11], x13
    st1 {v4.4h}, [x12], x13
    st1 {v1.4h}, [x11], x13
    st1 {v5.4h}, [x12], x13
    st1 {v2.4h}, [x11], x13
    st1 {v6.4h}, [x12], x13
    st1 {v3.4h}, [x11]
    st1 {v7.4h}, [x12]

    add v0.4s, v28.4s, v24.4s       //DST[4]
    add v1.4s, v29.4s, v25.4s       //DST[5]
    add v2.4s, v30.4s, v26.4s       //DST[6]
    add v3.4s, v31.4s, v27.4s       //DST[7]
    sub v7.4s, v28.4s, v24.4s       //DST[11]
    sub v6.4s, v29.4s, v25.4s       //DST[10]
    sub v5.4s, v30.4s, v26.4s       //DST[9]
    sub v4.4s, v31.4s, v27.4s       //DST[8]

    sqrshrn v0.4h, v0.4s, #5
    sqrshrn v1.4h, v1.4s, #5
    sqrshrn v2.4h, v2.4s, #5
    sqrshrn v3.4h, v3.4s, #5
    sqrshrn v4.4h, v4.4s, #5
    sqrshrn v5.4h, v5.4s, #5
    sqrshrn v6.4h, v6.4s, #5
    sqrshrn v7.4h, v7.4s, #5

    trn1 v24.2s, v0.2s, v2.2s
    trn2 v26.2s, v0.2s, v2.2s
    trn1 v25.2s, v1.2s, v3.2s
    trn2 v27.2s, v1.2s, v3.2s
    trn1 v0.4h, v24.4h, v25.4h
    trn2 v1.4h, v24.4h, v25.4h
    trn1 v2.4h, v26.4h, v27.4h
    trn2 v3.4h, v26.4h, v27.4h

    trn1 v24.2s, v4.2s, v6.2s
    trn2 v26.2s, v4.2s, v6.2s
    trn1 v25.2s, v5.2s, v7.2s
    trn2 v27.2s, v5.2s, v7.2s
    trn1 v4.4h, v24.4h, v25.4h
    trn2 v5.4h, v24.4h, v25.4h
    trn1 v6.4h, v26.4h, v27.4h
    trn2 v7.4h, v26.4h, v27.4h

    add x11, x2, #8
    trn1 v0.2d, v0.2d, v4.2d
    trn1 v1.2d, v1.2d, v5.2d
    trn1 v2.2d, v2.2d, v6.2d
    trn1 v3.2d, v3.2d, v7.2d

    st1 {v0.8h}, [x11], x13
    st1 {v1.8h}, [x11], x13
    st1 {v2.8h}, [x11], x13
    st1 {v3.8h}, [x11]
    add x8, x8, #8
    add x2, x2, #128                // next 4*16*sizeof(s16)
    cmp x8, x3
    blt dct2_h16_1st_loopx

    ld1 {v8.8h, v9.8h}, [sp], #32
    b   dct2_h16_end
dct2_h16_2nd_transform:

    sub sp, sp, #48
    add x7, sp, #16
    st1 {v10.8h, v11.8h}, [x7]
    st1 {v12.8h}, [sp]

    mov w8, #1
    lsl w8, w8, w5
    sub w6, w5, #20                 // -shift = bit_depth - 20
    sub w9, w8, #1                  // max_pel = (1<<bit_depth) - 1
    neg w8, w8                      // min_pel = -(1<<bit_depth)
    dup v12.4s, w6                  // for left shift
    dup v10.8h, w8                  // min_pel
    dup v11.8h, w9                  // max_pel

    lsl x9, x1, #2                  // i_src * 2 * sizeof(s16)
    mov x8, #0                      // i = 0
    lsl x1, x1, #1                  // i_src * sizeof(s16)
    lsl x3, x3, #1
dct2_h16_2nd_loopx:
    add x10, x0, x8
    ld1 {v0.4h}, [x10], x9          //SRC[0*LINE]
    ld1 {v1.4h}, [x10], x9          //SRC[2*LINE]
    ld1 {v2.4h}, [x10], x9          //SRC[4*LINE]
    ld1 {v3.4h}, [x10], x9          //SRC[6*LINE]
    ld1 {v4.4h}, [x10], x9          //SRC[8*LINE]
    ld1 {v5.4h}, [x10], x9          //SRC[10*LINE]
    ld1 {v6.4h}, [x10], x9          //SRC[12*LINE]
    ld1 {v7.4h}, [x10], x9          //SRC[14*LINE]

    smull v16.4s, v1.4h, v8.h[3]
    smlal v16.4s, v3.4h, v8.h[2]
    smlal v16.4s, v5.4h, v8.h[1]
    smlal v16.4s, v7.4h, v8.h[0]    //EO[0]

    smull v17.4s, v1.4h, v8.h[2]
    smlsl v17.4s, v3.4h, v8.h[0]
    smlsl v17.4s, v5.4h, v8.h[3]
    smlsl v17.4s, v7.4h, v8.h[1]    //EO[1]

    smull v18.4s, v1.4h, v8.h[1]
    smlsl v18.4s, v3.4h, v8.h[3]
    smlal v18.4s, v5.4h, v8.h[0]
    smlal v18.4s, v7.4h, v8.h[2]    //EO[2]

    smull v19.4s, v1.4h, v8.h[0]
    smlsl v19.4s, v3.4h, v8.h[1]
    smlal v19.4s, v5.4h, v8.h[2]
    smlsl v19.4s, v7.4h, v8.h[3]    //EO[3]

    smull v20.4s, v2.4h, v8.h[7]
    smlal v20.4s, v6.4h, v8.h[6]    //EEO[0]
    smull v21.4s, v0.4h, v8.h[4]
    smlal v21.4s, v4.4h, v8.h[4]    //EEE[0]
    smull v22.4s, v2.4h, v8.h[6]
    smlsl v22.4s, v6.4h, v8.h[7]    //EEO[1]
    smull v23.4s, v0.4h, v8.h[4]
    smlsl v23.4s, v4.4h, v8.h[4]    //EEE[1]

    add v24.4s, v21.4s, v20.4s      //EE[0]
    add v25.4s, v23.4s, v22.4s      //EE[1]
    sub v26.4s, v23.4s, v22.4s      //EE[2]
    sub v27.4s, v21.4s, v20.4s      //EE[3]

    add v20.4s, v24.4s, v16.4s      //E[0]
    add v21.4s, v25.4s, v17.4s      //E[1]
    add v22.4s, v26.4s, v18.4s      //E[2]
    add v23.4s, v27.4s, v19.4s      //E[3]
    sub v28.4s, v27.4s, v19.4s      //E[4]
    sub v29.4s, v26.4s, v18.4s      //E[5]
    sub v30.4s, v25.4s, v17.4s      //E[6]
    sub v31.4s, v24.4s, v16.4s      //E[7]

    add x10, x0, x8
    add x10, x10, x1
    ld1 {v0.4h}, [x10], x9          //SRC[1*LINE]
    ld1 {v1.4h}, [x10], x9          //SRC[3*LINE]
    ld1 {v2.4h}, [x10], x9          //SRC[5*LINE]
    ld1 {v3.4h}, [x10], x9          //SRC[7*LINE]
    ld1 {v4.4h}, [x10], x9          //SRC[9*LINE]
    ld1 {v5.4h}, [x10], x9          //SRC[11*LINE]
    ld1 {v6.4h}, [x10], x9          //SRC[13*LINE]
    ld1 {v7.4h}, [x10], x9          //SRC[15*LINE]

    //O[0]
    smull v16.4s, v0.4h, v9.h[7]
    smlal v16.4s, v1.4h, v9.h[6]
    smlal v16.4s, v2.4h, v9.h[5]
    smlal v16.4s, v3.4h, v9.h[4]
    smlal v16.4s, v4.4h, v9.h[3]
    smlal v16.4s, v5.4h, v9.h[2]
    smlal v16.4s, v6.4h, v9.h[1]
    smlal v16.4s, v7.4h, v9.h[0]
    //O[1]
    smull v17.4s, v0.4h, v9.h[6]
    smlal v17.4s, v1.4h, v9.h[3]
    smlal v17.4s, v2.4h, v9.h[0]
    smlsl v17.4s, v3.4h, v9.h[2]
    smlsl v17.4s, v4.4h, v9.h[5]
    smlsl v17.4s, v5.4h, v9.h[7]
    smlsl v17.4s, v6.4h, v9.h[4]
    smlsl v17.4s, v7.4h, v9.h[1]
    //O[2]
    smull v18.4s, v0.4h, v9.h[5]
    smlal v18.4s, v1.4h, v9.h[0]
    smlsl v18.4s, v2.4h, v9.h[4]
    smlsl v18.4s, v3.4h, v9.h[6]
    smlsl v18.4s, v4.4h, v9.h[1]
    smlal v18.4s, v5.4h, v9.h[3]
    smlal v18.4s, v6.4h, v9.h[7]
    smlal v18.4s, v7.4h, v9.h[2]
    //O[3]
    smull v19.4s, v0.4h, v9.h[4]
    smlsl v19.4s, v1.4h, v9.h[2]
    smlsl v19.4s, v2.4h, v9.h[6]
    smlal v19.4s, v3.4h, v9.h[0]
    smlal v19.4s, v4.4h, v9.h[7]
    smlal v19.4s, v5.4h, v9.h[1]
    smlsl v19.4s, v6.4h, v9.h[5]
    smlsl v19.4s, v7.4h, v9.h[3]
    //O[4]
    smull v24.4s, v0.4h, v9.h[3]
    smlsl v24.4s, v1.4h, v9.h[5]
    smlsl v24.4s, v2.4h, v9.h[1]
    smlal v24.4s, v3.4h, v9.h[7]
    smlsl v24.4s, v4.4h, v9.h[0]
    smlsl v24.4s, v5.4h, v9.h[6]
    smlal v24.4s, v6.4h, v9.h[2]
    smlal v24.4s, v7.4h, v9.h[4]
    //O[5]
    smull v25.4s, v0.4h, v9.h[2]
    smlsl v25.4s, v1.4h, v9.h[7]
    smlal v25.4s, v2.4h, v9.h[3]
    smlal v25.4s, v3.4h, v9.h[1]
    smlsl v25.4s, v4.4h, v9.h[6]
    smlal v25.4s, v5.4h, v9.h[4]
    smlal v25.4s, v6.4h, v9.h[0]
    smlsl v25.4s, v7.4h, v9.h[5]
    //O[6]
    smull v26.4s, v0.4h, v9.h[1]
    smlsl v26.4s, v1.4h, v9.h[4]
    smlal v26.4s, v2.4h, v9.h[7]
    smlsl v26.4s, v3.4h, v9.h[5]
    smlal v26.4s, v4.4h, v9.h[2]
    smlal v26.4s, v5.4h, v9.h[0]
    smlsl v26.4s, v6.4h, v9.h[3]
    smlal v26.4s, v7.4h, v9.h[6]
    //O[7]
    smull v27.4s, v0.4h, v9.h[0]
    smlsl v27.4s, v1.4h, v9.h[1]
    smlal v27.4s, v2.4h, v9.h[2]
    smlsl v27.4s, v3.4h, v9.h[3]
    smlal v27.4s, v4.4h, v9.h[4]
    smlsl v27.4s, v5.4h, v9.h[5]
    smlal v27.4s, v6.4h, v9.h[6]
    smlsl v27.4s, v7.4h, v9.h[7]

    add v0.4s, v16.4s, v20.4s       //DST[0]
    add v1.4s, v17.4s, v21.4s       //DST[1]
    add v2.4s, v18.4s, v22.4s       //DST[2]
    add v3.4s, v19.4s, v23.4s       //DST[3]
    sub v7.4s, v20.4s, v16.4s       //DST[15]
    sub v6.4s, v21.4s, v17.4s       //DST[14]
    sub v5.4s, v22.4s, v18.4s       //DST[13]
    sub v4.4s, v23.4s, v19.4s       //DST[12]

#if !COMPILE_10BIT
    sqrshrn v0.4h, v0.4s, #12
    sqrshrn v1.4h, v1.4s, #12
    sqrshrn v2.4h, v2.4s, #12
    sqrshrn v3.4h, v3.4s, #12
    sqrshrn v4.4h, v4.4s, #12
    sqrshrn v5.4h, v5.4s, #12
    sqrshrn v6.4h, v6.4s, #12
    sqrshrn v7.4h, v7.4s, #12
#else
    srshl v0.4s, v0.4s, v12.4s
    srshl v1.4s, v1.4s, v12.4s
    srshl v2.4s, v2.4s, v12.4s
    srshl v3.4s, v3.4s, v12.4s
    srshl v4.4s, v4.4s, v12.4s
    srshl v5.4s, v5.4s, v12.4s
    srshl v6.4s, v6.4s, v12.4s
    srshl v7.4s, v7.4s, v12.4s

    sqxtn v0.4h, v0.4s
    sqxtn v1.4h, v1.4s
    sqxtn v2.4h, v2.4s
    sqxtn v3.4h, v3.4s
    sqxtn v4.4h, v4.4s
    sqxtn v5.4h, v5.4s
    sqxtn v6.4h, v6.4s
    sqxtn v7.4h, v7.4s
#endif

    trn1 v16.2s, v0.2s, v2.2s
    trn2 v18.2s, v0.2s, v2.2s
    trn1 v17.2s, v1.2s, v3.2s
    trn2 v19.2s, v1.2s, v3.2s
    trn1 v0.4h, v16.4h, v17.4h
    trn2 v1.4h, v16.4h, v17.4h
    trn1 v2.4h, v18.4h, v19.4h
    trn2 v3.4h, v18.4h, v19.4h

    trn1 v16.2s, v4.2s, v6.2s
    trn2 v18.2s, v4.2s, v6.2s
    trn1 v17.2s, v5.2s, v7.2s
    trn2 v19.2s, v5.2s, v7.2s
    trn1 v4.4h, v16.4h, v17.4h
    trn2 v5.4h, v16.4h, v17.4h
    trn1 v6.4h, v18.4h, v19.4h
    trn2 v7.4h, v18.4h, v19.4h

    add v16.4s, v28.4s, v24.4s       //DST[4]
    add v17.4s, v29.4s, v25.4s       //DST[5]
    add v18.4s, v30.4s, v26.4s       //DST[6]
    add v19.4s, v31.4s, v27.4s       //DST[7]
    sub v23.4s, v28.4s, v24.4s       //DST[11]
    sub v22.4s, v29.4s, v25.4s       //DST[10]
    sub v21.4s, v30.4s, v26.4s       //DST[9]
    sub v20.4s, v31.4s, v27.4s       //DST[8]

#if !COMPILE_10BIT
    sqrshrn v16.4h, v16.4s, #12
    sqrshrn v17.4h, v17.4s, #12
    sqrshrn v18.4h, v18.4s, #12
    sqrshrn v19.4h, v19.4s, #12
    sqrshrn v20.4h, v20.4s, #12
    sqrshrn v21.4h, v21.4s, #12
    sqrshrn v22.4h, v22.4s, #12
    sqrshrn v23.4h, v23.4s, #12
#else
    srshl v16.4s, v16.4s, v12.4s
    srshl v17.4s, v17.4s, v12.4s
    srshl v18.4s, v18.4s, v12.4s
    srshl v19.4s, v19.4s, v12.4s
    srshl v20.4s, v20.4s, v12.4s
    srshl v21.4s, v21.4s, v12.4s
    srshl v22.4s, v22.4s, v12.4s
    srshl v23.4s, v23.4s, v12.4s

    sqxtn v16.4h, v16.4s
    sqxtn v17.4h, v17.4s
    sqxtn v18.4h, v18.4s
    sqxtn v19.4h, v19.4s
    sqxtn v20.4h, v20.4s
    sqxtn v21.4h, v21.4s
    sqxtn v22.4h, v22.4s
    sqxtn v23.4h, v23.4s
#endif

    trn1 v24.2s, v16.2s, v18.2s
    trn2 v26.2s, v16.2s, v18.2s
    trn1 v25.2s, v17.2s, v19.2s
    trn2 v27.2s, v17.2s, v19.2s
    trn1 v16.4h, v24.4h, v25.4h
    trn2 v17.4h, v24.4h, v25.4h
    trn1 v18.4h, v26.4h, v27.4h
    trn2 v19.4h, v26.4h, v27.4h

    trn1 v24.2s, v20.2s, v22.2s
    trn2 v26.2s, v20.2s, v22.2s
    trn1 v25.2s, v21.2s, v23.2s
    trn2 v27.2s, v21.2s, v23.2s
    trn1 v20.4h, v24.4h, v25.4h
    trn2 v21.4h, v24.4h, v25.4h
    trn1 v22.4h, v26.4h, v27.4h
    trn2 v23.4h, v26.4h, v27.4h

    trn1 v24.2d, v0.2d, v16.2d
    trn1 v26.2d, v1.2d, v17.2d
    trn1 v28.2d, v2.2d, v18.2d
    trn1 v30.2d, v3.2d, v19.2d

    trn1 v25.2d, v20.2d, v4.2d
    trn1 v27.2d, v21.2d, v5.2d
    trn1 v29.2d, v22.2d, v6.2d
    trn1 v31.2d, v23.2d, v7.2d

    smax v0.8h, v0.8h, v10.8h
    smax v1.8h, v1.8h, v10.8h
    smax v2.8h, v2.8h, v10.8h
    smax v3.8h, v3.8h, v10.8h
    smax v4.8h, v4.8h, v10.8h
    smax v5.8h, v5.8h, v10.8h
    smax v6.8h, v6.8h, v10.8h
    smax v7.8h, v7.8h, v10.8h

    smin v0.8h, v0.8h, v11.8h
    smin v1.8h, v1.8h, v11.8h
    smin v2.8h, v2.8h, v11.8h
    smin v3.8h, v3.8h, v11.8h
    smin v4.8h, v4.8h, v11.8h
    smin v5.8h, v5.8h, v11.8h
    smin v6.8h, v6.8h, v11.8h
    smin v7.8h, v7.8h, v11.8h

    add x8, x8, #8
    st1 {v24.2d, v25.2d, v26.2d, v27.2d}, [x2], #64
    st1 {v28.2d, v29.2d, v30.2d, v31.2d}, [x2], #64
    cmp x8, x3
    blt dct2_h16_2nd_loopx

    ld1 {v12.8h}, [sp], #16
    ld1 {v10.8h, v11.8h}, [sp], #32
    ld1 {v8.8h, v9.8h}, [sp], #32

dct2_h16_end:
    ret

//**********************************************************
// input:
//   x10: src
//   x12: i_src*8
//   x6: i_src*4
//   x7: i_src*2
//   x2: i_src
//   v8, v9, v14, v15: dct2 coeffs
// output:
//   v0-v7: O[0]-O[7]
//   sp[0-127]: O[8]-O[15]
//   sp[128-383]: E[0]-E[15]
//**********************************************************
.macro dct2_h32_w4_calcu_E_O_arm64

    ld1 {v0.4h}, [x10], x12         // src[ 0       ]
    ld1 {v1.4h}, [x10], x12         // src[ 8*i_src  ]
    ld1 {v2.4h}, [x10], x12         // src[ 16*i_src ]
    ld1 {v3.4h}, [x10], x12         // src[ 24*i_src ]

    smull v4.4s, v1.4h, v14.H[3]
    smull v5.4s, v1.4h, v14.H[2]
    smull v6.4s, v0.4h, v14.H[0]
    smull v7.4s, v0.4h, v14.H[0]
    smlal v4.4s, v3.4h, v14.H[2]    // EEEO[0]
    smlsl v5.4s, v3.4h, v14.H[3]    // EEEO[1]
    smlal v6.4s, v2.4h, v14.H[0]    // EEEE[0]
    smlsl v7.4s, v2.4h, v14.H[0]    // EEEE[1]

    add v16.4s, v6.4s, v4.4s        // EEE[0]
    add v17.4s, v7.4s, v5.4s        // EEE[1]
    sub v18.4s, v7.4s, v5.4s        // EEE[2]
    sub v19.4s, v6.4s, v4.4s        // EEE[3]

    // CALCULATE EEO
    add x10, x0, x8
    add x10, x10, x6
    ld1 {v0.4h}, [x10], x12         // src[ 4*line  ]
    ld1 {v1.4h}, [x10], x12         // src[ 12*line  ]
    ld1 {v2.4h}, [x10], x12         // src[ 20*line ]
    ld1 {v3.4h}, [x10], x12         // src[ 28*line ]

    smull v20.4s, v0.4h, v14.H[7]
    smull v21.4s, v0.4h, v14.H[6]
    smull v22.4s, v0.4h, v14.H[5]
    smull v23.4s, v0.4h, v14.H[4]

    smlal v20.4s, v1.4h, v14.H[6]
    smlsl v21.4s, v1.4h, v14.H[4]
    smlsl v22.4s, v1.4h, v14.H[7]
    smlsl v23.4s, v1.4h, v14.H[5]

    smlal v20.4s, v2.4h, v14.H[5]
    smlsl v21.4s, v2.4h, v14.H[7]
    smlal v22.4s, v2.4h, v14.H[4]
    smlal v23.4s, v2.4h, v14.H[6]

    smlal v20.4s, v3.4h, v14.H[4]   // EEO[0]
    smlsl v21.4s, v3.4h, v14.H[5]   // EEO[1]
    smlal v22.4s, v3.4h, v14.H[6]   // EEO[2]
    smlsl v23.4s, v3.4h, v14.H[7]   // EEO[3]

    // CALCULATE EE
    add v24.4s, v16.4s, v20.4s      // EE[0]
    add v25.4s, v17.4s, v21.4s      // EE[1]
    add v26.4s, v18.4s, v22.4s      // EE[2]
    add v27.4s, v19.4s, v23.4s      // EE[3]
    sub v28.4s, v19.4s, v23.4s      // EE[4]
    sub v29.4s, v18.4s, v22.4s      // EE[5]
    sub v30.4s, v17.4s, v21.4s      // EE[6]
    sub v31.4s, v16.4s, v20.4s      // EE[7]

    //CALCULATE EO
    add x10, x0, x8
    add x10, x10, x7                // src + 2*i_src

    ld1 {v0.4h}, [x10], X6          // src[ 2*line ]
    ld1 {v1.4h}, [x10], X6          // src[ 6*line ]
    ld1 {v2.4h}, [x10], X6          // src[10*line ]
    ld1 {v3.4h}, [x10], X6          // src[14*line ]
    ld1 {v4.4h}, [x10], X6          // src[18*line ]
    ld1 {v5.4h}, [x10], X6          // src[22*line ]
    ld1 {v6.4h}, [x10], X6          // src[26*line ]
    ld1 {v7.4h}, [x10], X6          // src[30*line ]

    smull v16.4s, v0.4h, v15.H[0]
    smull v17.4s, v0.4h, v15.H[1]
    smull v18.4s, v0.4h, v15.H[2]
    smull v19.4s, v0.4h, v15.H[3]

    smlsl v16.4s, v1.4h, v15.H[1]
    smlsl v17.4s, v1.4h, v15.H[4]
    smlsl v18.4s, v1.4h, v15.H[7]
    smlsl v19.4s, v1.4h, v15.H[5]

    smlal v16.4s, v2.4h, v15.H[2]
    smlal v17.4s, v2.4h, v15.H[7]
    smlal v18.4s, v2.4h, v15.H[3]
    smlsl v19.4s, v2.4h, v15.H[1]

    smlsl v16.4s, v3.4h, v15.H[3]
    smlsl v17.4s, v3.4h, v15.H[5]
    smlal v18.4s, v3.4h, v15.H[1]
    smlal v19.4s, v3.4h, v15.H[7]

    smlal v16.4s, v4.4h, v15.H[4]
    smlal v17.4s, v4.4h, v15.H[2]
    smlsl v18.4s, v4.4h, v15.H[6]
    smlsl v19.4s, v4.4h, v15.H[0]

    smlsl v16.4s, v5.4h, v15.H[5]
    smlal v17.4s, v5.4h, v15.H[0]
    smlal v18.4s, v5.4h, v15.H[4]
    smlsl v19.4s, v5.4h, v15.H[6]

    smlal v16.4s, v6.4h, v15.H[6]
    smlsl v17.4s, v6.4h, v15.H[3]
    smlal v18.4s, v6.4h, v15.H[0]
    smlal v19.4s, v6.4h, v15.H[2]

    smlsl v16.4s, v7.4h, v15.H[7]   // EO[7]
    smlal v17.4s, v7.4h, v15.H[6]   // EO[6]
    smlsl v18.4s, v7.4h, v15.H[5]   // EO[5]
    smlal v19.4s, v7.4h, v15.H[4]   // EO[4]

    smull v20.4s, v0.4h, v15.H[4]
    smull v21.4s, v0.4h, v15.H[5]
    smull v22.4s, v0.4h, v15.H[6]
    smull v23.4s, v0.4h, v15.H[7]

    smlsl v20.4s, v1.4h, v15.H[2]
    smlal v21.4s, v1.4h, v15.H[0]
    smlal v22.4s, v1.4h, v15.H[3]
    smlal v23.4s, v1.4h, v15.H[6]

    smlsl v20.4s, v2.4h, v15.H[6]
    smlsl v21.4s, v2.4h, v15.H[4]
    smlal v22.4s, v2.4h, v15.H[0]
    smlal v23.4s, v2.4h, v15.H[5]

    smlal v20.4s, v3.4h, v15.H[0]
    smlsl v21.4s, v3.4h, v15.H[6]
    smlsl v22.4s, v3.4h, v15.H[2]
    smlal v23.4s, v3.4h, v15.H[4]

    smlal v20.4s, v4.4h, v15.H[7]
    smlsl v21.4s, v4.4h, v15.H[1]
    smlsl v22.4s, v4.4h, v15.H[5]
    smlal v23.4s, v4.4h, v15.H[3]

    smlal v20.4s, v5.4h, v15.H[1]
    smlal v21.4s, v5.4h, v15.H[3]
    smlsl v22.4s, v5.4h, v15.H[7]
    smlal v23.4s, v5.4h, v15.H[2]

    smlsl v20.4s, v6.4h, v15.H[5]
    smlal v21.4s, v6.4h, v15.H[7]
    smlsl v22.4s, v6.4h, v15.H[4]
    smlal v23.4s, v6.4h, v15.H[1]

    smlsl v20.4s, v7.4h, v15.H[3]   // EO[7]
    smlal v21.4s, v7.4h, v15.H[2]   // EO[6]
    smlsl v22.4s, v7.4h, v15.H[1]   // EO[5]
    smlal v23.4s, v7.4h, v15.H[0]   // EO[4]

    add v0.4s, v24.4s, v23.4s       // E[0]
    add v1.4s, v25.4s, v22.4s       // E[1]
    add v2.4s, v26.4s, v21.4s       // E[2]
    add v3.4s, v27.4s, v20.4s       // E[3]
    add v4.4s, v28.4s, v19.4s       // E[4]
    add v5.4s, v29.4s, v18.4s       // E[5]
    add v6.4s, v30.4s, v17.4s       // E[6]
    add v7.4s, v31.4s, v16.4s       // E[7]

    add x9, sp, #128
    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x9], #64     // save E[0-7]
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x9], #64

    sub v0.4s, v31.4s, v16.4s       // E[8]
    sub v1.4s, v30.4s, v17.4s       // E[9]
    sub v2.4s, v29.4s, v18.4s       // E[10]
    sub v3.4s, v28.4s, v19.4s       // E[11]
    sub v4.4s, v27.4s, v20.4s       // E[12]
    sub v5.4s, v26.4s, v21.4s       // E[13]
    sub v6.4s, v25.4s, v22.4s       // E[14]
    sub v7.4s, v24.4s, v23.4s       // E[15]

    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x9], #64     // save E[8-15]
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x9], #64

    //CALCULATE O
    add x10, x0, x8
    add x10, x10, x1                // src + i_src

    ld1 {v16.4h}, [x10], x7         //src[ 1*line ]
    ld1 {v17.4h}, [x10], x7         //src[ 3*line ]
    ld1 {v18.4h}, [x10], x7         //src[ 5*line ]
    ld1 {v19.4h}, [x10], x7         //src[ 7*line ]
    ld1 {v20.4h}, [x10], x7         //src[ 9*line ]
    ld1 {v21.4h}, [x10], x7         //src[11*line ]
    ld1 {v22.4h}, [x10], x7         //src[13*line ]
    ld1 {v23.4h}, [x10], x7         //src[15*line ]
    ld1 {v24.4h}, [x10], x7         //src[17*line ]
    ld1 {v25.4h}, [x10], x7         //src[19*line ]
    ld1 {v26.4h}, [x10], x7         //src[21*line ]
    ld1 {v27.4h}, [x10], x7         //src[23*line ]
    ld1 {v28.4h}, [x10], x7         //src[25*line ]
    ld1 {v29.4h}, [x10], x7         //src[27*line ]
    ld1 {v30.4h}, [x10], x7         //src[29*line ]
    ld1 {v31.4h}, [x10], x7         //src[31*line ]

    //O[15]
    smull v7.4s, v16.4h, v8.H[0]
    smlsl v7.4s, v17.4h, v8.H[1]
    smlal v7.4s, v18.4h, v8.H[2]
    smlsl v7.4s, v19.4h, v8.H[3]

    smlal v7.4s, v20.4h, v8.H[4]
    smlsl v7.4s, v21.4h, v8.H[5]
    smlal v7.4s, v22.4h, v8.H[6]
    smlsl v7.4s, v23.4h, v8.H[7]

    smlal v7.4s, v24.4h, v9.H[0]
    smlsl v7.4s, v25.4h, v9.H[1]
    smlal v7.4s, v26.4h, v9.H[2]
    smlsl v7.4s, v27.4h, v9.H[3]

    smlal v7.4s, v28.4h, v9.H[4]
    smlsl v7.4s, v29.4h, v9.H[5]
    smlal v7.4s, v30.4h, v9.H[6]
    smlsl v7.4s, v31.4h, v9.H[7]

    //O[14]
    smull v6.4s, v16.4h, v8.H[1]
    smlsl v6.4s, v17.4h, v8.H[4]
    smlal v6.4s, v18.4h, v8.H[7]
    smlsl v6.4s, v19.4h, v9.H[2]

    smlal v6.4s, v20.4h, v9.H[5]
    smlsl v6.4s, v21.4h, v9.H[7]
    smlal v6.4s, v22.4h, v9.H[4]
    smlsl v6.4s, v23.4h, v9.H[1]

    smlal v6.4s, v24.4h, v8.H[6]
    smlsl v6.4s, v25.4h, v8.H[3]
    smlal v6.4s, v26.4h, v8.H[0]
    smlal v6.4s, v27.4h, v8.H[2]

    smlsl v6.4s, v28.4h, v8.H[5]
    smlal v6.4s, v29.4h, v9.H[0]
    smlsl v6.4s, v30.4h, v9.H[3]
    smlal v6.4s, v31.4h, v9.H[7]

    //O[13]
    smull v5.4s, v16.4h, v8.H[2]
    smlsl v5.4s, v17.4h, v8.H[7]
    smlal v5.4s, v18.4h, v9.H[4]
    smlsl v5.4s, v19.4h, v9.H[7]

    smlal v5.4s, v20.4h, v9.H[1]
    smlsl v5.4s, v21.4h, v8.H[4]
    smlsl v5.4s, v22.4h, v8.H[0]
    smlal v5.4s, v23.4h, v8.H[5]

    smlsl v5.4s, v24.4h, v9.H[2]
    smlal v5.4s, v25.4h, v9.H[6]
    smlsl v5.4s, v26.4h, v9.H[3]
    smlal v5.4s, v27.4h, v8.H[6]

    smlsl v5.4s, v28.4h, v8.H[1]
    smlsl v5.4s, v29.4h, v8.H[3]
    smlal v5.4s, v30.4h, v9.H[0]
    smlsl v5.4s, v31.4h, v9.H[5]

    //O[12]
    smull v4.4s, v16.4h, v8.H[3]
    smlsl v4.4s, v17.4h, v9.H[2]
    smlal v4.4s, v18.4h, v9.H[7]
    smlsl v4.4s, v19.4h, v8.H[7]

    smlal v4.4s, v20.4h, v8.H[0]
    smlal v4.4s, v21.4h, v8.H[6]
    smlsl v4.4s, v22.4h, v9.H[5]
    smlal v4.4s, v23.4h, v9.H[3]

    smlsl v4.4s, v24.4h, v8.H[4]
    smlsl v4.4s, v25.4h, v8.H[2]
    smlal v4.4s, v26.4h, v9.H[1]
    smlsl v4.4s, v27.4h, v9.H[7]

    smlal v4.4s, v28.4h, v9.H[0]
    smlsl v4.4s, v29.4h, v8.H[1]
    smlsl v4.4s, v30.4h, v8.H[5]
    smlal v4.4s, v31.4h, v9.H[4]

    //O[11]
    smull v3.4s, v16.4h, v8.H[4]
    smlsl v3.4s, v17.4h, v9.H[5]
    smlal v3.4s, v18.4h, v9.H[1]
    smlsl v3.4s, v19.4h, v8.H[0]

    smlsl v3.4s, v20.4h, v9.H[0]
    smlal v3.4s, v21.4h, v9.H[7]
    smlsl v3.4s, v22.4h, v8.H[5]
    smlsl v3.4s, v23.4h, v8.H[3]

    smlal v3.4s, v24.4h, v9.H[4]
    smlsl v3.4s, v25.4h, v9.H[2]
    smlal v3.4s, v26.4h, v8.H[1]
    smlal v3.4s, v27.4h, v8.H[7]

    smlsl v3.4s, v28.4h, v9.H[7]
    smlal v3.4s, v29.4h, v8.H[6]
    smlal v3.4s, v30.4h, v8.H[2]
    smlsl v3.4s, v31.4h, v9.H[3]

    //O[10]
    smull v2.4s, v16.4h, v8.H[5]
    smlsl v2.4s, v17.4h, v9.H[7]
    smlal v2.4s, v18.4h, v8.H[4]
    smlal v2.4s, v19.4h, v8.H[6]

    smlsl v2.4s, v20.4h, v9.H[7]
    smlal v2.4s, v21.4h, v8.H[3]
    smlal v2.4s, v22.4h, v8.H[7]
    smlsl v2.4s, v23.4h, v9.H[5]

    smlal v2.4s, v24.4h, v8.H[2]
    smlal v2.4s, v25.4h, v9.H[0]
    smlsl v2.4s, v26.4h, v9.H[4]
    smlal v2.4s, v27.4h, v8.H[1]

    smlal v2.4s, v28.4h, v9.H[1]
    smlsl v2.4s, v29.4h, v9.H[3]
    smlal v2.4s, v30.4h, v8.H[0]
    smlal v2.4s, v31.4h, v9.H[2]

    //O[9]
    smull v1.4s, v16.4h, v8.H[6]
    smlsl v1.4s, v17.4h, v9.H[4]
    smlsl v1.4s, v18.4h, v8.H[0]
    smlal v1.4s, v19.4h, v9.H[5]

    smlsl v1.4s, v20.4h, v8.H[5]
    smlsl v1.4s, v21.4h, v8.H[7]
    smlal v1.4s, v22.4h, v9.H[3]
    smlal v1.4s, v23.4h, v8.H[1]

    smlsl v1.4s, v24.4h, v9.H[7]
    smlal v1.4s, v25.4h, v8.H[4]
    smlal v1.4s, v26.4h, v9.H[0]
    smlsl v1.4s, v27.4h, v9.H[2]

    smlsl v1.4s, v28.4h, v8.H[2]
    smlal v1.4s, v29.4h, v9.H[7]
    smlsl v1.4s, v30.4h, v8.H[3]
    smlsl v1.4s, v31.4h, v9.H[1]

    //O[8]
    smull v0.4s, v16.4h, v8.H[7]
    smlsl v0.4s, v17.4h, v9.H[1]
    smlsl v0.4s, v18.4h, v8.H[5]
    smlal v0.4s, v19.4h, v9.H[3]

    smlal v0.4s, v20.4h, v8.H[3]
    smlsl v0.4s, v21.4h, v9.H[5]
    smlsl v0.4s, v22.4h, v8.H[1]
    smlal v0.4s, v23.4h, v9.H[7]

    smlsl v0.4s, v24.4h, v8.H[0]
    smlsl v0.4s, v25.4h, v9.H[7]
    smlal v0.4s, v26.4h, v8.H[2]
    smlal v0.4s, v27.4h, v9.H[4]

    smlsl v0.4s, v28.4h, v8.H[4]
    smlsl v0.4s, v29.4h, v9.H[2]
    smlal v0.4s, v30.4h, v8.H[6]
    smlal v0.4s, v31.4h, v9.H[0]

    // STORE O[8] - O[15]
    mov x9, sp
    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x9], #64 //STORE O[8] - O[11]
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x9], #64 //STORE O[12] - O[15]

    //O[7]
    smull v7.4s, v16.4h, v9.H[0]
    smlsl v7.4s, v17.4h, v8.H[6]
    smlsl v7.4s, v18.4h, v9.H[2]
    smlal v7.4s, v19.4h, v8.H[4]

    smlal v7.4s, v20.4h, v9.H[4]
    smlsl v7.4s, v21.4h, v8.H[2]
    smlsl v7.4s, v22.4h, v9.H[7]
    smlal v7.4s, v23.4h, v8.H[0]

    smlal v7.4s, v24.4h, v9.H[7]
    smlal v7.4s, v25.4h, v8.H[1]
    smlsl v7.4s, v26.4h, v9.H[5]
    smlsl v7.4s, v27.4h, v8.H[3]

    smlal v7.4s, v28.4h, v9.H[3]
    smlal v7.4s, v29.4h, v8.H[5]
    smlsl v7.4s, v30.4h, v9.H[1]
    smlsl v7.4s, v31.4h, v8.H[7]

    //O[6]
    smull v6.4s, v16.4h, v9.H[1]
    smlsl v6.4s, v17.4h, v8.H[3]
    smlsl v6.4s, v18.4h, v9.H[7]
    smlsl v6.4s, v19.4h, v8.H[2]

    smlal v6.4s, v20.4h, v9.H[2]
    smlal v6.4s, v21.4h, v9.H[0]
    smlsl v6.4s, v22.4h, v8.H[4]
    smlsl v6.4s, v23.4h, v9.H[7]

    smlsl v6.4s, v24.4h, v8.H[1]
    smlal v6.4s, v25.4h, v9.H[3]
    smlal v6.4s, v26.4h, v8.H[7]
    smlsl v6.4s, v27.4h, v8.H[5]

    smlsl v6.4s, v28.4h, v9.H[5]
    smlsl v6.4s, v29.4h, v8.H[0]
    smlal v6.4s, v30.4h, v9.H[4]
    smlal v6.4s, v31.4h, v8.H[6]

    //O[5]
    smull v5.4s, v16.4h, v9.H[2]
    smlsl v5.4s, v17.4h, v8.H[0]
    smlsl v5.4s, v18.4h, v9.H[3]
    smlsl v5.4s, v19.4h, v9.H[1]

    smlal v5.4s, v20.4h, v8.H[1]
    smlal v5.4s, v21.4h, v9.H[4]
    smlal v5.4s, v22.4h, v9.H[0]
    smlsl v5.4s, v23.4h, v8.H[2]

    smlsl v5.4s, v24.4h, v9.H[5]
    smlsl v5.4s, v25.4h, v8.H[7]
    smlal v5.4s, v26.4h, v8.H[3]
    smlal v5.4s, v27.4h, v9.H[7]

    smlal v5.4s, v28.4h, v8.H[6]
    smlsl v5.4s, v29.4h, v8.H[4]
    smlsl v5.4s, v30.4h, v9.H[7]
    smlsl v5.4s, v31.4h, v8.H[5]

    //O[4]
    smull v4.4s, v16.4h, v9.H[3]
    smlal v4.4s, v17.4h, v8.H[2]
    smlsl v4.4s, v18.4h, v8.H[6]
    smlsl v4.4s, v19.4h, v9.H[6]

    smlsl v4.4s, v20.4h, v8.H[7]
    smlal v4.4s, v21.4h, v8.H[1]
    smlal v4.4s, v22.4h, v9.H[2]
    smlal v4.4s, v23.4h, v9.H[4]

    smlal v4.4s, v24.4h, v8.H[3]
    smlsl v4.4s, v25.4h, v8.H[5]
    smlsl v4.4s, v26.4h, v9.H[7]
    smlsl v4.4s, v27.4h, v9.H[0]

    smlal v4.4s, v28.4h, v8.H[0]
    smlal v4.4s, v29.4h, v9.H[1]
    smlal v4.4s, v30.4h, v9.H[5]
    smlal v4.4s, v31.4h, v8.H[4]

    //O[3]
    smull v3.4s, v16.4h, v9.H[4]
    smlal v3.4s, v17.4h, v8.H[5]
    smlsl v3.4s, v18.4h, v8.H[1]
    smlsl v3.4s, v19.4h, v9.H[0]

    smlsl v3.4s, v20.4h, v9.H[7]
    smlsl v3.4s, v21.4h, v9.H[1]
    smlsl v3.4s, v22.4h, v8.H[2]
    smlal v3.4s, v23.4h, v8.H[4]

    smlal v3.4s, v24.4h, v9.H[3]
    smlal v3.4s, v25.4h, v9.H[5]
    smlal v3.4s, v26.4h, v8.H[6]
    smlsl v3.4s, v27.4h, v8.H[0]

    smlsl v3.4s, v28.4h, v8.H[7]
    smlsl v3.4s, v29.4h, v9.H[7]
    smlsl v3.4s, v30.4h, v9.H[2]
    smlsl v3.4s, v31.4h, v8.H[3]

    //O[2]
    smull v2.4s, v16.4h, v9.H[5]
    smlal v2.4s, v17.4h, v9.H[0]
    smlal v2.4s, v18.4h, v8.H[3]
    smlsl v2.4s, v19.4h, v8.H[1]

    smlsl v2.4s, v20.4h, v8.H[6]
    smlsl v2.4s, v21.4h, v9.H[3]
    smlsl v2.4s, v22.4h, v9.H[7]
    smlsl v2.4s, v23.4h, v9.H[2]

    smlsl v2.4s, v24.4h, v8.H[5]
    smlsl v2.4s, v25.4h, v8.H[0]
    smlal v2.4s, v26.4h, v8.H[4]
    smlal v2.4s, v27.4h, v9.H[1]

    smlal v2.4s, v28.4h, v9.H[7]
    smlal v2.4s, v29.4h, v9.H[4]
    smlal v2.4s, v30.4h, v8.H[7]
    smlal v2.4s, v31.4h, v8.H[2]

    //O[1]
    smull v1.4s, v16.4h, v9.H[7]
    smlal v1.4s, v17.4h, v9.H[3]
    smlal v1.4s, v18.4h, v9.H[0]
    smlal v1.4s, v19.4h, v8.H[5]

    smlal v1.4s, v20.4h, v8.H[2]
    smlsl v1.4s, v21.4h, v8.H[0]
    smlsl v1.4s, v22.4h, v8.H[3]
    smlsl v1.4s, v23.4h, v8.H[6]

    smlsl v1.4s, v24.4h, v9.H[1]
    smlsl v1.4s, v25.4h, v9.H[4]
    smlsl v1.4s, v26.4h, v9.H[7]
    smlsl v1.4s, v27.4h, v9.H[5]

    smlsl v1.4s, v28.4h, v9.H[2]
    smlsl v1.4s, v29.4h, v8.H[7]
    smlsl v1.4s, v30.4h, v8.H[4]
    smlsl v1.4s, v31.4h, v8.H[1]

    //O[0]
    smull v0.4s, v16.4h, v9.H[7]
    smlal v0.4s, v17.4h, v9.H[6]
    smlal v0.4s, v18.4h, v9.H[5]
    smlal v0.4s, v19.4h, v9.H[4]

    smlal v0.4s, v20.4h, v9.H[3]
    smlal v0.4s, v21.4h, v9.H[2]
    smlal v0.4s, v22.4h, v9.H[1]
    smlal v0.4s, v23.4h, v9.H[0]

    smlal v0.4s, v24.4h, v8.H[7]
    smlal v0.4s, v25.4h, v8.H[6]
    smlal v0.4s, v26.4h, v8.H[5]
    smlal v0.4s, v27.4h, v8.H[4]

    smlal v0.4s, v28.4h, v8.H[3]
    smlal v0.4s, v29.4h, v8.H[2]
    smlal v0.4s, v30.4h, v8.H[1]
    smlal v0.4s, v31.4h, v8.H[0]
.endm

//************************************************************************
//void dct2_butterfly_h32_arm64(s16 *src, int i_src, s16 *dst, int width, int shift, int bit_depth);
//x0: coeff blk, 16 bit
//x1: i_src
//x2: resi blk, 16 bit
//x3: blk width
//x4: shift
//x5: bit_depth
//************************************************************************
function dct2_butterfly_h32_arm64
    sub sp, sp, #128
    st1 {v8.4s, v9.4s, v10.4s, v11.4s}, [sp], #64
    st1 {v12.4s, v13.4s, v14.4s, v15.4s}, [sp]
    sub sp, sp, #64

    mov w12, #32
    mov w6, #17
    mov w7, #42
    mov w8, #9
    mov w9, #25
    mov w10, #38
    mov w11, #44
    ins v14.h[0], w12
    ins v14.h[1], w12
    ins v14.h[2], w6
    ins v14.h[3], w7
    ins v14.h[4], w8
    ins v14.h[5], w9
    ins v14.h[6], w10
    ins v14.h[7], w11

    mov w8 , #4
    mov w9 , #13
    mov w10, #21
    mov w11, #29
    mov w12, #35
    mov w13, #40
    mov w14, #43
    mov w15, #45
    ins v15.h[0], w8
    ins v15.h[1], w9
    ins v15.h[2], w10
    ins v15.h[3], w11
    ins v15.h[4], w12
    ins v15.h[5], w13
    ins v15.h[6], w14
    ins v15.h[7], w15

    mov w8 , #2
    mov w9 , #7
    mov w10, #11
    mov w11, #15
    mov w12, #19
    mov w13, #23
    mov w14, #27
    mov w15, #30
    mov v8.h[0], w8
    mov v8.h[1], w9
    mov v8.h[2], w10
    mov v8.h[3], w11
    mov v8.h[4], w12
    mov v8.h[5], w13
    mov v8.h[6], w14
    mov v8.h[7], w15

    mov w8 , #34
    mov w9 , #36
    mov w10, #39
    mov w11, #41
    mov w12, #43
    mov w13, #44
    mov w14, #45
    mov w15, #45
    mov v9.h[0], w8
    mov v9.h[1], w9
    mov v9.h[2], w10
    mov v9.h[3], w11
    mov v9.h[4], w12
    mov v9.h[5], w13
    mov v9.h[6], w14
    mov v9.h[7], w15

    mov x11, #64                    // i_dst = 64
    lsl x1, x1, #1                  // i_src *= sizeof(s16)
    lsl x3, x3, #1
    mov x8, #0                      // i = 0
    lsl x12, x1, #3                 // 8*i_src
    lsl x6, x1, #2                  // 4*i_src
    lsl x7, x1, #1                  // 2*i_src
    cmp w5, #15                     // second transform: bit_depth == 15
    bne dct2_h32_2nd_loopx

dct2_h32_1st_loopx:
    add x10, x0, x8
    sub sp, sp, #384
    dct2_h32_w4_calcu_E_O_arm64

    add x9, sp, #128
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x9], #64 // E[0]-E[3]
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x9], #64 // E[4]-E[7]

    add v24.4s, v16.4s, v0.4s       // DST[0]
    add v25.4s, v17.4s, v1.4s       // DST[1]
    add v26.4s, v18.4s, v2.4s       // DST[2]
    add v27.4s, v19.4s, v3.4s       // DST[3]
    add v28.4s, v20.4s, v4.4s       // DST[4]
    add v29.4s, v21.4s, v5.4s       // DST[5]
    add v30.4s, v22.4s, v6.4s       // DST[6]
    add v31.4s, v23.4s, v7.4s       // DST[7]

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[0]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[4]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    mov x10, x2                     // dst

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store 4 rows: dst[0-7]
    st1 {v24.8h}, [x10], x11        // dst += i_dst
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    sub v31.4s, v16.4s, v0.4s       // DST[31]
    sub v30.4s, v17.4s, v1.4s       // DST[30]
    sub v29.4s, v18.4s, v2.4s       // DST[29]
    sub v28.4s, v19.4s, v3.4s       // DST[28]
    sub v27.4s, v20.4s, v4.4s       // DST[27]
    sub v26.4s, v21.4s, v5.4s       // DST[26]
    sub v25.4s, v22.4s, v6.4s       // DST[25]
    sub v24.4s, v23.4s, v7.4s       // DST[24]

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[24]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[28]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    add x10, x2, #48
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store 4 rows: dst[24-31]
    st1 {v24.8h}, [x10], x11
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [sp], #64     // O[8]-O[11]
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [sp], #64     // O[12]-O[15]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x9], #64 // E[8]-E[11]
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x9], #64 // E[12]-E[15]

    add v24.4s, v16.4s, v0.4s       // DST[8]
    add v25.4s, v17.4s, v1.4s       // DST[9]
    add v26.4s, v18.4s, v2.4s       // DST[10]
    add v27.4s, v19.4s, v3.4s       // DST[11]
    add v28.4s, v20.4s, v4.4s       // DST[12]
    add v29.4s, v21.4s, v5.4s       // DST[13]
    add v30.4s, v22.4s, v6.4s       // DST[14]
    add v31.4s, v23.4s, v7.4s       // DST[15]

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[8]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[12]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    add x10, x2, #16

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store 4 rows: dst[8-15]
    st1 {v24.8h}, [x10], x11
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    sub v31.4s, v16.4s, v0.4s       // DST[23]
    sub v30.4s, v17.4s, v1.4s       // DST[22]
    sub v29.4s, v18.4s, v2.4s       // DST[21]
    sub v28.4s, v19.4s, v3.4s       // DST[20]
    sub v27.4s, v20.4s, v4.4s       // DST[19]
    sub v26.4s, v21.4s, v5.4s       // DST[18]
    sub v25.4s, v22.4s, v6.4s       // DST[17]
    sub v24.4s, v23.4s, v7.4s       // DST[16]

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[16]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[20]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    add x10, x2, #32
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store 4 rows: dst[16-23]
    st1 {v24.8h}, [x10], x11
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    add x8, x8, #8
    add sp, sp, #256
    add x2, x2, #256
    cmp x8, x3
    blt dct2_h32_1st_loopx
    b   dct2_h32_end

dct2_h32_2nd_loopx:
    add x10, x0, x8
    sub sp, sp, #384
    dct2_h32_w4_calcu_E_O_arm64

    add x9, sp, #128
    sub x10, sp, #16
    sub sp, sp, #48
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x9], #64 // E[0]-E[3]
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x9], #64 // E[4]-E[7]
    st1 {v14.2d, v15.2d}, [sp]      // save v14, v15 (dct coeffs)
    st1 {v8.2d}, [x10]

    mov w13, #1
    lsl w13, w13, w5
    sub w15, w5, #20                // -shift = bit_depth - 20
    sub w14, w13, #1                // max_pel = (1<<bit_depth) - 1
    neg w13, w13                    // min_pel = -(1<<bit_depth)
    dup v8.4s, w15                  // for left shift
    dup v14.8h, w13                 // minvaL vector
    dup v15.8h, w14                 // maxvaL vector

    add v24.4s, v16.4s, v0.4s       // DST[0] = E[0] + O[0]
    add v25.4s, v17.4s, v1.4s       // DST[1]
    add v26.4s, v18.4s, v2.4s       // DST[2]
    add v27.4s, v19.4s, v3.4s       // DST[3]
    add v28.4s, v20.4s, v4.4s       // DST[4]
    add v29.4s, v21.4s, v5.4s       // DST[5]
    add v30.4s, v22.4s, v6.4s       // DST[6]
    add v31.4s, v23.4s, v7.4s       // DST[7]

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[0]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[4]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    mov x10, x2

    smax v24.8h, v24.8h, v14.8h
    smax v25.8h, v25.8h, v14.8h
    smax v26.8h, v26.8h, v14.8h
    smax v27.8h, v27.8h, v14.8h
    smin v24.8h, v24.8h, v15.8h
    smin v25.8h, v25.8h, v15.8h
    smin v26.8h, v26.8h, v15.8h
    smin v27.8h, v27.8h, v15.8h

    // store 4 rows: dst[0-7]
    st1 {v24.8h}, [x10], x11
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    sub v31.4s, v16.4s, v0.4s       // DST[31] = E[0] - O[0]
    sub v30.4s, v17.4s, v1.4s       // DST[30]
    sub v29.4s, v18.4s, v2.4s       // DST[29]
    sub v28.4s, v19.4s, v3.4s       // DST[28]
    sub v27.4s, v20.4s, v4.4s       // DST[27]
    sub v26.4s, v21.4s, v5.4s       // DST[26]
    sub v25.4s, v22.4s, v6.4s       // DST[25]
    sub v24.4s, v23.4s, v7.4s       // DST[24]

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[24]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[28]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    add x10, x2, #48
    smax v24.8h, v24.8h, v14.8h
    smax v25.8h, v25.8h, v14.8h
    smax v26.8h, v26.8h, v14.8h
    smax v27.8h, v27.8h, v14.8h
    smin v24.8h, v24.8h, v15.8h
    smin v25.8h, v25.8h, v15.8h
    smin v26.8h, v26.8h, v15.8h
    smin v27.8h, v27.8h, v15.8h

    // store 4 rows: dst[24-31]
    st1 {v24.8h}, [x10], x11
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x9], #64 // E[8]-E[11]
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x9], #64 // E[12]-E[15]
    sub x9, x9, #384
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x9], #64     // O[8]-O[11]
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x9], #64     // O[12]-O[15]

    add v24.4s, v16.4s, v0.4s       // DST[8]
    add v25.4s, v17.4s, v1.4s       // DST[9]
    add v26.4s, v18.4s, v2.4s       // DST[10]
    add v27.4s, v19.4s, v3.4s       // DST[11]
    add v28.4s, v20.4s, v4.4s       // DST[12]
    add v29.4s, v21.4s, v5.4s       // DST[13]
    add v30.4s, v22.4s, v6.4s       // DST[14]
    add v31.4s, v23.4s, v7.4s       // DST[15]

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[8]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[12]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    add x10, x2, #16

    smax v24.8h, v24.8h, v14.8h
    smax v25.8h, v25.8h, v14.8h
    smax v26.8h, v26.8h, v14.8h
    smax v27.8h, v27.8h, v14.8h
    smin v24.8h, v24.8h, v15.8h
    smin v25.8h, v25.8h, v15.8h
    smin v26.8h, v26.8h, v15.8h
    smin v27.8h, v27.8h, v15.8h

    // store 4 rows: dst[8-15]
    st1 {v24.8h}, [x10], x11
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    sub v31.4s, v16.4s, v0.4s       // DST[23]
    sub v30.4s, v17.4s, v1.4s       // DST[22]
    sub v29.4s, v18.4s, v2.4s       // DST[21]
    sub v28.4s, v19.4s, v3.4s       // DST[20]
    sub v27.4s, v20.4s, v4.4s       // DST[19]
    sub v26.4s, v21.4s, v5.4s       // DST[18]
    sub v25.4s, v22.4s, v6.4s       // DST[17]
    sub v24.4s, v23.4s, v7.4s       // DST[16]

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v10.2s, v24.2s, v26.2s
    trn1 v11.2s, v25.2s, v27.2s
    trn2 v12.2s, v24.2s, v26.2s
    trn2 v13.2s, v25.2s, v27.2s
    trn1 v24.4h, v10.4h, v11.4h     // dst[16]
    trn2 v25.4h, v10.4h, v11.4h
    trn1 v26.4h, v12.4h, v13.4h
    trn2 v27.4h, v12.4h, v13.4h

    trn1 v10.2s, v28.2s, v30.2s
    trn1 v11.2s, v29.2s, v31.2s
    trn2 v12.2s, v28.2s, v30.2s
    trn2 v13.2s, v29.2s, v31.2s
    trn1 v28.4h, v10.4h, v11.4h     // dst[20]
    trn2 v29.4h, v10.4h, v11.4h
    trn1 v30.4h, v12.4h, v13.4h
    trn2 v31.4h, v12.4h, v13.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    add x10, x2, #32
    smax v24.8h, v24.8h, v14.8h
    smax v25.8h, v25.8h, v14.8h
    smax v26.8h, v26.8h, v14.8h
    smax v27.8h, v27.8h, v14.8h
    smin v24.8h, v24.8h, v15.8h
    smin v25.8h, v25.8h, v15.8h
    smin v26.8h, v26.8h, v15.8h
    smin v27.8h, v27.8h, v15.8h

    // store 4 rows: dst[16-23]
    st1 {v24.8h}, [x10], x11
    st1 {v25.8h}, [x10], x11
    st1 {v26.8h}, [x10], x11
    st1 {v27.8h}, [x10], x11

    ld1 {v14.2d, v15.2d}, [sp], #32
    ld1 {v8.2d}, [sp], #16

    add x8, x8, #8
    add x2, x2, #256                // dst += i_dst*4
    add sp, sp, #384
    cmp x8, x3
    blt dct2_h32_2nd_loopx

dct2_h32_end:
    ld1 {v8.4s, v9.4s, v10.4s, v11.4s}, [sp], #64
    ld1 {v12.4s, v13.4s, v14.4s, v15.4s}, [sp], #64

    ret

//**********************************************************
// calculate E[0-31] and O[0-31] for 4 lines resi
// input:
//   x0: src
//   x11: i_src*8
//   x12: i_src*4
//   x13: i_src*2
//   x1: i_src
//   v10-v15: dct2 coeffs
// output:
//   sp[0-512]: E[0]-E[31]
//   sp[512-896]: O[0]-O[23]
//   v0-v7: O[24]-O[31]
//**********************************************************
.macro dct2_h64_w4_calcu_E_O_arm64
    mov x10, x0
    // EEEE[0-3]
    ld1 {v0.4h}, [x10], x11         // [0]
    ld1 {v1.4h}, [x10], x11         // [8*i_src]
    ld1 {v2.4h}, [x10], x11         // [16*i_src]
    ld1 {v3.4h}, [x10], x11         // [24*i_src]

    smull v16.4s, v0.4h, v11.h[7]   // src[0]*tm[0] == src[0]*tm[1]
    smull v17.4s, v2.4h, v10.h[5]   // src[16*i_src]*tm[16*64]
    smull v18.4s, v2.4h, v13.h[6]
    add   v4.4s, v16.4s, v17.4s     // EEEE[0]
    add   v5.4s, v16.4s, v18.4s     // EEEE[1]
    sub   v6.4s, v16.4s, v18.4s     // EEEE[2]
    sub   v7.4s, v16.4s, v17.4s     // EEEE[3]

    smull v20.4s, v1.4h, v10.h[3]
    smull v21.4s, v1.4h, v11.h[1]
    smull v22.4s, v1.4h, v12.h[6]
    smull v23.4s, v1.4h, v14.h[6]
    smlal v20.4s, v3.4h, v11.h[1]   // EEEO[0]
    smlsl v21.4s, v3.4h, v14.h[6]   // EEEO[1]
    smlsl v22.4s, v3.4h, v10.h[3]
    smlsl v23.4s, v3.4h, v12.h[6]

    add   v24.4s, v4.4s, v20.4s     // EEE[0]
    add   v25.4s, v5.4s, v21.4s     // EEE[1]
    add   v26.4s, v6.4s, v22.4s
    add   v27.4s, v7.4s, v23.4s
    sub   v31.4s, v4.4s, v20.4s     // EEE[7]
    sub   v30.4s, v5.4s, v21.4s     // EEE[6]
    sub   v29.4s, v6.4s, v22.4s
    sub   v28.4s, v7.4s, v23.4s

    // EEO
    add x10, x0, x12
    ld1 {v0.4h}, [x10], x11         // [4*i_src]
    ld1 {v1.4h}, [x10], x11         // [12*i_src]
    ld1 {v2.4h}, [x10], x11         // [20*i_src]
    ld1 {v3.4h}, [x10], x11         // [28*i_src]

    smull v16.4s, v0.4h, v10.h[2]
    smlal v16.4s, v1.4h, v10.h[4]
    smlal v16.4s, v2.4h, v10.h[7]
    smlal v16.4s, v3.4h, v11.h[4]   // EEO[0]

    smull v17.4s, v0.4h, v10.h[4]
    smlal v17.4s, v1.4h, v12.h[2]
    smlal v17.4s, v2.4h, v15.h[3]
    smlsl v17.4s, v3.4h, v13.h[2]   // EEO[1]

    smull v18.4s, v0.4h, v10.h[7]
    smlal v18.4s, v1.4h, v15.h[3]
    smlsl v18.4s, v2.4h, v11.h[4]
    smlsl v18.4s, v3.4h, v10.h[4]   // EEO[2]

    smull v19.4s, v0.4h, v11.h[4]
    smlsl v19.4s, v1.4h, v13.h[2]
    smlsl v19.4s, v2.4h, v10.h[4]
    smlal v19.4s, v3.4h, v15.h[3]   // EEO[3]

    smull v20.4s, v0.4h, v12.h[2]
    smlsl v20.4s, v1.4h, v10.h[7]
    smlsl v20.4s, v2.4h, v14.h[2]
    smlal v20.4s, v3.4h, v10.h[2]   // EEO[4]

    smull v21.4s, v0.4h, v13.h[2]
    smlsl v21.4s, v1.4h, v10.h[2]
    smlal v21.4s, v2.4h, v12.h[2]
    smlal v21.4s, v3.4h, v14.h[2]   // EEO[5]

    smull v22.4s, v0.4h, v14.h[2]
    smlsl v22.4s, v1.4h, v11.h[4]
    smlal v22.4s, v2.4h, v10.h[2]
    smlsl v22.4s, v3.4h, v10.h[7]   // EEO[6]

    smull v23.4s, v0.4h, v15.h[3]
    smlsl v23.4s, v1.4h, v14.h[2]
    smlal v23.4s, v2.4h, v13.h[2]
    smlsl v23.4s, v3.4h, v12.h[2]   // EEO[7]

    // EE
    add   v0.4s, v16.4s, v24.4s     // EE[0]
    add   v1.4s, v17.4s, v25.4s     // EE[1]
    add   v2.4s, v18.4s, v26.4s     // EE[2]
    add   v3.4s, v19.4s, v27.4s     // EE[3]
    add   v4.4s, v20.4s, v28.4s     // EE[4]
    add   v5.4s, v21.4s, v29.4s     // EE[5]
    add   v6.4s, v22.4s, v30.4s     // EE[6]
    add   v7.4s, v23.4s, v31.4s     // EE[7]

    mov   x9, sp                    // push EE[0-15]
    st1   {v0.4s, v1.4s, v2.4s, v3.4s}, [x9], #64
    st1   {v4.4s, v5.4s, v6.4s, v7.4s}, [x9], #64

    sub   v7.4s, v24.4s, v16.4s     // EE[15]
    sub   v6.4s, v25.4s, v17.4s     // EE[14]
    sub   v5.4s, v26.4s, v18.4s     // EE[13]
    sub   v4.4s, v27.4s, v19.4s     // EE[12]
    sub   v3.4s, v28.4s, v20.4s     // EE[11]
    sub   v2.4s, v29.4s, v21.4s     // EE[10]
    sub   v1.4s, v30.4s, v22.4s     // EE[9]
    sub   v0.4s, v31.4s, v23.4s     // EE[8]

    st1   {v0.4s, v1.4s, v2.4s, v3.4s}, [x9], #64
    st1   {v4.4s, v5.4s, v6.4s, v7.4s}, [x9]

    // EO
    add x10, x0, x13                // &src[2*i_src]

    ld1 {v0.4h}, [x10], x12         // [2*i_src]
    ld1 {v1.4h}, [x10], x12         // [6*i_src]
    ld1 {v2.4h}, [x10], x12         // [10*i_src]
    ld1 {v3.4h}, [x10], x12         // [14*i_src]
    ld1 {v4.4h}, [x10], x12         // [18*i_src]
    ld1 {v5.4h}, [x10], x12         // [22*i_src]
    ld1 {v6.4h}, [x10], x12         // [26*i_src]
    ld1 {v7.4h}, [x10], x12         // [30*i_src]

    smull v16.4s, v0.4h, v10.h[2]
    smlal v16.4s, v1.4h, v10.h[2]
    smlal v16.4s, v2.4h, v10.h[3]
    smlal v16.4s, v3.4h, v10.h[4]
    smlal v16.4s, v4.4h, v10.h[6]
    smlal v16.4s, v5.4h, v11.h[0]
    smlal v16.4s, v6.4h, v11.h[3]
    smlal v16.4s, v7.4h, v11.h[5]   // EO[0]

    smull v17.4s, v0.4h, v10.h[2]
    smlal v17.4s, v1.4h, v10.h[6]
    smlal v17.4s, v2.4h, v11.h[5]
    smlal v17.4s, v3.4h, v13.h[0]
    smlal v17.4s, v4.4h, v14.h[4]
    smlsl v17.4s, v5.4h, v15.h[5]
    smlsl v17.4s, v6.4h, v14.h[0]
    smlsl v17.4s, v7.4h, v12.h[4]

    smull v18.4s, v0.4h, v10.h[3]
    smlal v18.4s, v1.4h, v11.h[5]
    smlal v18.4s, v2.4h, v14.h[0]
    smlsl v18.4s, v3.4h, v15.h[0]
    smlsl v18.4s, v4.4h, v12.h[4]
    smlsl v18.4s, v5.4h, v10.h[6]
    smlsl v18.4s, v6.4h, v10.h[2]
    smlsl v18.4s, v7.4h, v11.h[0]

    smull v19.4s, v0.4h, v10.h[4]
    smlal v19.4s, v1.4h, v13.h[0]
    smlsl v19.4s, v2.4h, v15.h[0]
    smlsl v19.4s, v3.4h, v11.h[5]
    smlsl v19.4s, v4.4h, v10.h[2]
    smlsl v19.4s, v5.4h, v11.h[3]
    smlsl v19.4s, v6.4h, v14.h[4]
    smlal v19.4s, v7.4h, v13.h[4]

    smull v20.4s, v0.4h, v10.h[6]
    smlal v20.4s, v1.4h, v14.h[4]
    smlsl v20.4s, v2.4h, v12.h[4]
    smlsl v20.4s, v3.4h, v10.h[2]
    smlsl v20.4s, v4.4h, v12.h[1]
    smlal v20.4s, v5.4h, v15.h[0]
    smlal v20.4s, v6.4h, v11.h[0]
    smlal v20.4s, v7.4h, v10.h[4]

    smull v21.4s, v0.4h, v11.h[0]
    smlsl v21.4s, v1.4h, v15.h[5]
    smlsl v21.4s, v2.4h, v10.h[6]
    smlsl v21.4s, v3.4h, v11.h[3]
    smlal v21.4s, v4.4h, v15.h[0]
    smlal v21.4s, v5.4h, v10.h[4]
    smlal v21.4s, v6.4h, v11.h[5]
    smlsl v21.4s, v7.4h, v14.h[4]

    smull v22.4s, v0.4h, v11.h[3]
    smlsl v22.4s, v1.4h, v14.h[0]
    smlsl v22.4s, v2.4h, v10.h[2]
    smlsl v22.4s, v3.4h, v14.h[4]
    smlal v22.4s, v4.4h, v11.h[0]
    smlal v22.4s, v5.4h, v11.h[5]
    smlsl v22.4s, v6.4h, v13.h[4]
    smlsl v22.4s, v7.4h, v10.h[2]

    smull v23.4s, v0.4h, v11.h[5]
    smlsl v23.4s, v1.4h, v12.h[4]
    smlsl v23.4s, v2.4h, v11.h[0]
    smlal v23.4s, v3.4h, v13.h[4]
    smlal v23.4s, v4.4h, v10.h[4]
    smlsl v23.4s, v5.4h, v14.h[4]
    smlsl v23.4s, v6.4h, v10.h[2]
    smlal v23.4s, v7.4h, v15.h[5]

    smull v24.4s, v0.4h, v12.h[1]
    smlsl v24.4s, v1.4h, v11.h[3]
    smlsl v24.4s, v2.4h, v13.h[0]
    smlal v24.4s, v3.4h, v10.h[6]
    smlal v24.4s, v4.4h, v14.h[0]
    smlsl v24.4s, v5.4h, v10.h[3]
    smlsl v24.4s, v6.4h, v15.h[0]
    smlal v24.4s, v7.4h, v10.h[2]

    smull v25.4s, v0.4h, v12.h[4]
    smlsl v25.4s, v1.4h, v10.h[4]
    smlsl v25.4s, v2.4h, v15.h[5]
    smlal v25.4s, v3.4h, v10.h[3]
    smlsl v25.4s, v4.4h, v13.h[0]
    smlsl v25.4s, v5.4h, v12.h[1]
    smlal v25.4s, v6.4h, v10.h[6]
    smlal v25.4s, v7.4h, v15.h[0]

    smull v26.4s, v0.4h, v13.h[0]
    smlsl v26.4s, v1.4h, v10.h[2]
    smlal v26.4s, v2.4h, v13.h[4]
    smlal v26.4s, v3.4h, v12.h[4]
    smlsl v26.4s, v4.4h, v10.h[2]
    smlal v26.4s, v5.4h, v14.h[0]
    smlal v26.4s, v6.4h, v12.h[1]
    smlsl v26.4s, v7.4h, v10.h[3]

    smull v27.4s, v0.4h, v13.h[4]
    smlsl v27.4s, v1.4h, v10.h[3]
    smlal v27.4s, v2.4h, v11.h[3]
    smlsl v27.4s, v3.4h, v15.h[5]
    smlsl v27.4s, v4.4h, v11.h[5]
    smlal v27.4s, v5.4h, v10.h[2]
    smlsl v27.4s, v6.4h, v13.h[0]
    smlsl v27.4s, v7.4h, v14.h[0]

    smull v28.4s, v0.4h, v14.h[0]
    smlsl v28.4s, v1.4h, v11.h[0]
    smlal v28.4s, v2.4h, v10.h[2]
    smlsl v28.4s, v3.4h, v12.h[1]
    smlal v28.4s, v4.4h, v15.h[5]
    smlal v28.4s, v5.4h, v12.h[4]
    smlsl v28.4s, v6.4h, v10.h[3]
    smlal v28.4s, v7.4h, v10.h[6]

    smull v29.4s, v0.4h, v14.h[4]
    smlsl v29.4s, v1.4h, v12.h[1]
    smlal v29.4s, v2.4h, v10.h[4]
    smlsl v29.4s, v3.4h, v10.h[2]
    smlal v29.4s, v4.4h, v11.h[3]
    smlsl v29.4s, v5.4h, v13.h[4]
    smlsl v29.4s, v6.4h, v15.h[5]
    smlal v29.4s, v7.4h, v13.h[0]

    smull v30.4s, v0.4h, v15.h[0]
    smlsl v30.4s, v1.4h, v13.h[4]
    smlal v30.4s, v2.4h, v12.h[1]
    smlsl v30.4s, v3.4h, v11.h[0]
    smlal v30.4s, v4.4h, v10.h[3]
    smlsl v30.4s, v5.4h, v10.h[2]
    smlal v30.4s, v6.4h, v10.h[4]
    smlsl v30.4s, v7.4h, v11.h[3]

    smull v31.4s, v0.4h, v15.h[5]
    smlsl v31.4s, v1.4h, v15.h[0]
    smlal v31.4s, v2.4h, v14.h[4]
    smlsl v31.4s, v3.4h, v14.h[0]
    smlal v31.4s, v4.4h, v13.h[4]
    smlsl v31.4s, v5.4h, v13.h[0]
    smlal v31.4s, v6.4h, v12.h[4]
    smlsl v31.4s, v7.4h, v12.h[1]

    mov x6, sp
    mov x7, sp
    // load EE[0-7]
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x6], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x6]

    // store EO[8-15]
    st1 {v24.4s, v25.4s, v26.4s, v27.4s}, [x7], #64
    st1 {v28.4s, v29.4s, v30.4s, v31.4s}, [x7]

    sub v31.4s, v0.4s, v16.4s       // E[31]
    sub v30.4s, v1.4s, v17.4s       // E[30]
    sub v29.4s, v2.4s, v18.4s       // E[29]
    sub v28.4s, v3.4s, v19.4s       // E[28]
    sub v27.4s, v4.4s, v20.4s       // E[27]
    sub v26.4s, v5.4s, v21.4s       // E[26]
    sub v25.4s, v6.4s, v22.4s       // E[25]
    sub v24.4s, v7.4s, v23.4s       // E[24]

    mov x6, sp
    add x7, sp, #384
    // store E[24-31]
    st1 {v24.4s, v25.4s, v26.4s, v27.4s}, [x7], #64
    st1 {v28.4s, v29.4s, v30.4s, v31.4s}, [x7], #64

    add v24.4s, v0.4s, v16.4s       // E[0]
    add v25.4s, v1.4s, v17.4s       // E[1]
    add v26.4s, v2.4s, v18.4s       // E[2]
    add v27.4s, v3.4s, v19.4s       // E[3]
    add v28.4s, v4.4s, v20.4s       // E[4]
    add v29.4s, v5.4s, v21.4s       // E[5]
    add v30.4s, v6.4s, v22.4s       // E[6]
    add v31.4s, v7.4s, v23.4s       // E[7]

    mov x7, sp
    // load EO[8-15]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64
    // load EE[8-15]
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x6], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x6], #64

    // store E[0-7]
    st1 {v24.4s, v25.4s, v26.4s, v27.4s}, [x7], #64
    st1 {v28.4s, v29.4s, v30.4s, v31.4s}, [x7], #64

    add v24.4s, v0.4s, v16.4s       // E[8]
    add v25.4s, v1.4s, v17.4s       // E[9]
    add v26.4s, v2.4s, v18.4s       // E[10]
    add v27.4s, v3.4s, v19.4s       // E[11]
    add v28.4s, v4.4s, v20.4s       // E[12]
    add v29.4s, v5.4s, v21.4s       // E[13]
    add v30.4s, v6.4s, v22.4s       // E[14]
    add v31.4s, v7.4s, v23.4s       // E[15]

    // store E[8-15]
    st1 {v24.4s, v25.4s, v26.4s, v27.4s}, [x7], #64
    st1 {v28.4s, v29.4s, v30.4s, v31.4s}, [x7], #64

    sub v31.4s, v0.4s, v16.4s       // E[23]
    sub v30.4s, v1.4s, v17.4s       // E[22]
    sub v29.4s, v2.4s, v18.4s       // E[21]
    sub v28.4s, v3.4s, v19.4s       // E[20]
    sub v27.4s, v4.4s, v20.4s       // E[19]
    sub v26.4s, v5.4s, v21.4s       // E[18]
    sub v25.4s, v6.4s, v22.4s       // E[17]
    sub v24.4s, v7.4s, v23.4s       // E[16]
    st1 {v24.4s, v25.4s, v26.4s, v27.4s}, [x7], #64
    st1 {v28.4s, v29.4s, v30.4s, v31.4s}, [x7]

    // calculate O[0-31]
    add x10, x0, x1                // src + i_src

    ld1 {v16.4h}, [x10], x13        //src[ 1*line ]
    ld1 {v17.4h}, [x10], x13        //src[ 3*line ]
    ld1 {v18.4h}, [x10], x13        //src[ 5*line ]
    ld1 {v19.4h}, [x10], x13        //src[ 7*line ]
    ld1 {v20.4h}, [x10], x13        //src[ 9*line ]
    ld1 {v21.4h}, [x10], x13        //src[11*line ]
    ld1 {v22.4h}, [x10], x13        //src[13*line ]
    ld1 {v23.4h}, [x10], x13        //src[15*line ]
    ld1 {v24.4h}, [x10], x13        //src[17*line ]
    ld1 {v25.4h}, [x10], x13        //src[19*line ]
    ld1 {v26.4h}, [x10], x13        //src[21*line ]
    ld1 {v27.4h}, [x10], x13        //src[23*line ]
    ld1 {v28.4h}, [x10], x13        //src[25*line ]
    ld1 {v29.4h}, [x10], x13        //src[27*line ]
    ld1 {v30.4h}, [x10], x13        //src[29*line ]
    ld1 {v31.4h}, [x10], x13        //src[31*line ]

    smull v0.4s, v16.4h, v10.h[2]
    smlal v0.4s, v17.4h, v10.h[2]
    smlal v0.4s, v18.4h, v10.h[2]
    smlal v0.4s, v19.4h, v10.h[2]
    smlal v0.4s, v20.4h, v10.h[3]
    smlal v0.4s, v21.4h, v10.h[3]
    smlal v0.4s, v22.4h, v10.h[4]
    smlal v0.4s, v23.4h, v10.h[5]
    smlal v0.4s, v24.4h, v10.h[6]
    smlal v0.4s, v25.4h, v10.h[7]
    smlal v0.4s, v26.4h, v11.h[0]
    smlal v0.4s, v27.4h, v11.h[1]
    smlal v0.4s, v28.4h, v11.h[2]
    smlal v0.4s, v29.4h, v11.h[3]
    smlal v0.4s, v30.4h, v11.h[5]
    smlal v0.4s, v31.4h, v11.h[6]    // O[0]

    smull v1.4s, v16.4h, v10.h[2]
    smlal v1.4s, v17.4h, v10.h[3]
    smlal v1.4s, v18.4h, v10.h[5]
    smlal v1.4s, v19.4h, v11.h[0]
    smlal v1.4s, v20.4h, v11.h[3]
    smlal v1.4s, v21.4h, v12.h[0]
    smlal v1.4s, v22.4h, v12.h[5]
    smlal v1.4s, v23.4h, v13.h[3]
    smlal v1.4s, v24.4h, v14.h[1]
    smlal v1.4s, v25.4h, v14.h[7]
    smlal v1.4s, v26.4h, v15.h[6]
    smlsl v1.4s, v27.4h, v15.h[1]
    smlsl v1.4s, v28.4h, v14.h[3]
    smlsl v1.4s, v29.4h, v13.h[5]
    smlsl v1.4s, v30.4h, v12.h[7]
    smlsl v1.4s, v31.4h, v12.h[1]    // O[1]

    smull v2.4s, v16.4h, v10.h[2]
    smlal v2.4s, v17.4h, v10.h[5]
    smlal v2.4s, v18.4h, v11.h[2]
    smlal v2.4s, v19.4h, v12.h[1]
    smlal v2.4s, v20.4h, v13.h[3]
    smlal v2.4s, v21.4h, v14.h[5]
    smlsl v2.4s, v22.4h, v15.h[6]
    smlsl v2.4s, v23.4h, v14.h[3]
    smlsl v2.4s, v24.4h, v13.h[1]
    smlsl v2.4s, v25.4h, v12.h[0]
    smlsl v2.4s, v26.4h, v11.h[1]
    smlsl v2.4s, v27.4h, v10.h[4]
    smlsl v2.4s, v28.4h, v10.h[2]
    smlsl v2.4s, v29.4h, v10.h[2]
    smlsl v2.4s, v30.4h, v10.h[6]
    smlsl v2.4s, v31.4h, v11.h[3]    // O[2]

    smull v3.4s, v16.4h, v10.h[2]
    smlal v3.4s, v17.4h, v11.h[0]
    smlal v3.4s, v18.4h, v12.h[1]
    smlal v3.4s, v19.4h, v13.h[7]
    smlal v3.4s, v20.4h, v15.h[6]
    smlsl v3.4s, v21.4h, v14.h[1]
    smlsl v3.4s, v22.4h, v12.h[3]
    smlsl v3.4s, v23.4h, v11.h[1]
    smlsl v3.4s, v24.4h, v10.h[3]
    smlsl v3.4s, v25.4h, v10.h[2]
    smlsl v3.4s, v26.4h, v10.h[7]
    smlsl v3.4s, v27.4h, v12.h[0]
    smlsl v3.4s, v28.4h, v13.h[5]
    smlsl v3.4s, v29.4h, v15.h[4]
    smlal v3.4s, v30.4h, v14.h[3]
    smlal v3.4s, v31.4h, v12.h[5]    // O[3]

    smull v4.4s, v16.4h, v10.h[3]
    smlal v4.4s, v17.4h, v11.h[3]
    smlal v4.4s, v18.4h, v13.h[3]
    smlal v4.4s, v19.4h, v15.h[6]
    smlsl v4.4s, v20.4h, v13.h[5]
    smlsl v4.4s, v21.4h, v11.h[5]
    smlsl v4.4s, v22.4h, v10.h[3]
    smlsl v4.4s, v23.4h, v10.h[2]
    smlsl v4.4s, v24.4h, v11.h[2]
    smlsl v4.4s, v25.4h, v13.h[1]
    smlsl v4.4s, v26.4h, v15.h[4]
    smlal v4.4s, v27.4h, v13.h[7]
    smlal v4.4s, v28.4h, v11.h[6]
    smlal v4.4s, v29.4h, v10.h[4]
    smlal v4.4s, v30.4h, v10.h[2]
    smlal v4.4s, v31.4h, v11.h[1]    // O[4]

    smull v5.4s, v16.4h, v10.h[3]
    smlal v5.4s, v17.4h, v12.h[0]
    smlal v5.4s, v18.4h, v14.h[5]
    smlsl v5.4s, v19.4h, v14.h[1]
    smlsl v5.4s, v20.4h, v11.h[5]
    smlsl v5.4s, v21.4h, v10.h[2]
    smlsl v5.4s, v22.4h, v10.h[5]
    smlsl v5.4s, v23.4h, v12.h[3]
    smlsl v5.4s, v24.4h, v15.h[1]
    smlal v5.4s, v25.4h, v13.h[5]
    smlal v5.4s, v26.4h, v11.h[2]
    smlal v5.4s, v27.4h, v10.h[2]
    smlal v5.4s, v28.4h, v10.h[7]
    smlal v5.4s, v29.4h, v12.h[7]
    smlal v5.4s, v30.4h, v15.h[6]
    smlsl v5.4s, v31.4h, v13.h[1]    // O[5]

    smull v6.4s, v16.4h, v10.h[4]
    smlal v6.4s, v17.4h, v12.h[5]
    smlsl v6.4s, v18.4h, v15.h[6]
    smlsl v6.4s, v19.4h, v12.h[3]
    smlsl v6.4s, v20.4h, v10.h[3]
    smlsl v6.4s, v21.4h, v10.h[5]
    smlsl v6.4s, v22.4h, v12.h[7]
    smlal v6.4s, v23.4h, v15.h[4]
    smlal v6.4s, v24.4h, v12.h[1]
    smlal v6.4s, v25.4h, v10.h[3]
    smlal v6.4s, v26.4h, v10.h[6]
    smlal v6.4s, v27.4h, v13.h[1]
    smlsl v6.4s, v28.4h, v15.h[1]
    smlsl v6.4s, v29.4h, v12.h[0]
    smlsl v6.4s, v30.4h, v10.h[2]
    smlsl v6.4s, v31.4h, v10.h[7]    // O[6]

    smull v7.4s, v16.4h, v10.h[5]
    smlal v7.4s, v17.4h, v13.h[3]
    smlsl v7.4s, v18.4h, v14.h[3]
    smlsl v7.4s, v19.4h, v11.h[1]
    smlsl v7.4s, v20.4h, v10.h[2]
    smlsl v7.4s, v21.4h, v12.h[3]
    smlal v7.4s, v22.4h, v15.h[4]
    smlal v7.4s, v23.4h, v11.h[6]
    smlal v7.4s, v24.4h, v10.h[2]
    smlal v7.4s, v25.4h, v11.h[5]
    smlal v7.4s, v26.4h, v15.h[1]
    smlsl v7.4s, v27.4h, v12.h[5]
    smlsl v7.4s, v28.4h, v10.h[3]
    smlsl v7.4s, v29.4h, v11.h[0]
    smlsl v7.4s, v30.4h, v14.h[1]
    smlal v7.4s, v31.4h, v13.h[5]    // O[7]

    add x6, sp, #512
    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x6], #64
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x6], #64

    smull v0.4s, v16.4h, v10.h[6]
    smlal v0.4s, v17.4h, v14.h[1]
    smlsl v0.4s, v18.4h, v13.h[1]
    smlsl v0.4s, v19.4h, v10.h[3]
    smlsl v0.4s, v20.4h, v11.h[2]
    smlsl v0.4s, v21.4h, v15.h[1]
    smlal v0.4s, v22.4h, v12.h[1]
    smlal v0.4s, v23.4h, v10.h[2]
    smlal v0.4s, v24.4h, v12.h[0]
    smlsl v0.4s, v25.4h, v15.h[4]
    smlsl v0.4s, v26.4h, v11.h[3]
    smlsl v0.4s, v27.4h, v10.h[2]
    smlsl v0.4s, v28.4h, v12.h[7]
    smlal v0.4s, v29.4h, v14.h[3]
    smlal v0.4s, v30.4h, v10.h[7]
    smlal v0.4s, v31.4h, v10.h[5]    // O[8]

    smull v1.4s, v16.4h, v10.h[7]
    smlal v1.4s, v17.4h, v14.h[7]
    smlsl v1.4s, v18.4h, v12.h[0]
    smlsl v1.4s, v19.4h, v10.h[2]
    smlsl v1.4s, v20.4h, v13.h[1]
    smlal v1.4s, v21.4h, v13.h[5]
    smlal v1.4s, v22.4h, v10.h[3]
    smlal v1.4s, v23.4h, v11.h[5]
    smlsl v1.4s, v24.4h, v15.h[4]
    smlsl v1.4s, v25.4h, v11.h[1]
    smlsl v1.4s, v26.4h, v10.h[5]
    smlsl v1.4s, v27.4h, v14.h[3]
    smlal v1.4s, v28.4h, v12.h[3]
    smlal v1.4s, v29.4h, v10.h[2]
    smlal v1.4s, v30.4h, v12.h[5]
    smlsl v1.4s, v31.4h, v14.h[1]    // O[9]

    smull v2.4s, v16.4h, v11.h[0]
    smlal v2.4s, v17.4h, v15.h[6]
    smlsl v2.4s, v18.4h, v11.h[1]
    smlsl v2.4s, v19.4h, v10.h[7]
    smlsl v2.4s, v20.4h, v15.h[4]
    smlal v2.4s, v21.4h, v11.h[2]
    smlal v2.4s, v22.4h, v10.h[6]
    smlal v2.4s, v23.4h, v15.h[1]
    smlsl v2.4s, v24.4h, v11.h[3]
    smlsl v2.4s, v25.4h, v10.h[5]
    smlsl v2.4s, v26.4h, v14.h[7]
    smlal v2.4s, v27.4h, v11.h[5]
    smlal v2.4s, v28.4h, v10.h[4]
    smlal v2.4s, v29.4h, v14.h[5]
    smlsl v2.4s, v30.4h, v11.h[6]
    smlsl v2.4s, v31.4h, v10.h[3]    // O[10]

    smull v3.4s, v16.4h, v11.h[1]
    smlsl v3.4s, v17.4h, v15.h[1]
    smlsl v3.4s, v18.4h, v10.h[4]
    smlsl v3.4s, v19.4h, v12.h[0]
    smlal v3.4s, v20.4h, v13.h[7]
    smlal v3.4s, v21.4h, v10.h[2]
    smlal v3.4s, v22.4h, v13.h[1]
    smlsl v3.4s, v23.4h, v12.h[5]
    smlsl v3.4s, v24.4h, v10.h[2]
    smlsl v3.4s, v25.4h, v14.h[3]
    smlal v3.4s, v26.4h, v11.h[5]
    smlal v3.4s, v27.4h, v10.h[6]
    smlal v3.4s, v28.4h, v15.h[6]
    smlsl v3.4s, v29.4h, v10.h[7]
    smlsl v3.4s, v30.4h, v11.h[3]
    smlal v3.4s, v31.4h, v14.h[5]    // O[11]

    smull v4.4s, v16.4h, v11.h[2]
    smlsl v4.4s, v17.4h, v14.h[3]
    smlsl v4.4s, v18.4h, v10.h[2]
    smlsl v4.4s, v19.4h, v13.h[5]
    smlal v4.4s, v20.4h, v11.h[6]
    smlal v4.4s, v21.4h, v10.h[7]
    smlsl v4.4s, v22.4h, v15.h[1]
    smlsl v4.4s, v23.4h, v10.h[3]
    smlsl v4.4s, v24.4h, v12.h[7]
    smlal v4.4s, v25.4h, v12.h[3]
    smlal v4.4s, v26.4h, v10.h[4]
    smlal v4.4s, v27.4h, v15.h[6]
    smlsl v4.4s, v28.4h, v10.h[5]
    smlsl v4.4s, v29.4h, v12.h[1]
    smlal v4.4s, v30.4h, v13.h[1]
    smlal v4.4s, v31.4h, v10.h[2]    // O[12]

    smull v5.4s, v16.4h, v11.h[3]
    smlsl v5.4s, v17.4h, v13.h[5]
    smlsl v5.4s, v18.4h, v10.h[2]
    smlsl v5.4s, v19.4h, v15.h[4]
    smlal v5.4s, v20.4h, v10.h[4]
    smlal v5.4s, v21.4h, v12.h[7]
    smlsl v5.4s, v22.4h, v12.h[0]
    smlsl v5.4s, v23.4h, v11.h[0]
    smlal v5.4s, v24.4h, v14.h[3]
    smlal v5.4s, v25.4h, v10.h[2]
    smlal v5.4s, v26.4h, v14.h[5]
    smlsl v5.4s, v27.4h, v10.h[7]
    smlsl v5.4s, v28.4h, v12.h[1]
    smlal v5.4s, v29.4h, v12.h[5]
    smlal v5.4s, v30.4h, v10.h[5]
    smlsl v5.4s, v31.4h, v15.h[1]    // O[13]

    smull v6.4s, v16.4h, v11.h[5]
    smlsl v6.4s, v17.4h, v12.h[7]
    smlsl v6.4s, v18.4h, v10.h[6]
    smlal v6.4s, v19.4h, v14.h[3]
    smlal v6.4s, v20.4h, v10.h[2]
    smlal v6.4s, v21.4h, v15.h[6]
    smlsl v6.4s, v22.4h, v10.h[2]
    smlsl v6.4s, v23.4h, v14.h[1]
    smlal v6.4s, v24.4h, v10.h[7]
    smlal v6.4s, v25.4h, v12.h[5]
    smlsl v6.4s, v26.4h, v11.h[6]
    smlsl v6.4s, v27.4h, v11.h[3]
    smlal v6.4s, v28.4h, v13.h[1]
    smlal v6.4s, v29.4h, v10.h[5]
    smlsl v6.4s, v30.4h, v14.h[5]
    smlsl v6.4s, v31.4h, v10.h[2]    // O[14]

    smull v7.4s, v16.4h, v11.h[6]
    smlsl v7.4s, v17.4h, v12.h[1]
    smlsl v7.4s, v18.4h, v11.h[3]
    smlal v7.4s, v19.4h, v12.h[5]
    smlal v7.4s, v20.4h, v11.h[1]
    smlsl v7.4s, v21.4h, v13.h[1]
    smlsl v7.4s, v22.4h, v10.h[7]
    smlal v7.4s, v23.4h, v13.h[5]
    smlal v7.4s, v24.4h, v10.h[5]
    smlsl v7.4s, v25.4h, v14.h[1]
    smlsl v7.4s, v26.4h, v10.h[3]
    smlal v7.4s, v27.4h, v14.h[5]
    smlal v7.4s, v28.4h, v10.h[2]
    smlsl v7.4s, v29.4h, v15.h[1]
    smlsl v7.4s, v30.4h, v10.h[2]
    smlal v7.4s, v31.4h, v15.h[6]    // O[15]

    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x6], #64
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x6], #64

    smull v0.4s, v16.4h, v12.h[0]
    smlsl v0.4s, v17.4h, v11.h[5]
    smlsl v0.4s, v18.4h, v12.h[3]
    smlal v0.4s, v19.4h, v11.h[2]
    smlal v0.4s, v20.4h, v12.h[7]
    smlsl v0.4s, v21.4h, v11.h[0]
    smlsl v0.4s, v22.4h, v13.h[3]
    smlal v0.4s, v23.4h, v10.h[6]
    smlal v0.4s, v24.4h, v13.h[7]
    smlsl v0.4s, v25.4h, v10.h[4]
    smlsl v0.4s, v26.4h, v14.h[3]
    smlal v0.4s, v27.4h, v10.h[3]
    smlal v0.4s, v28.4h, v14.h[7]
    smlsl v0.4s, v29.4h, v10.h[2]
    smlsl v0.4s, v30.4h, v15.h[4]
    smlal v0.4s, v31.4h, v10.h[2]    // O[16]

    smull v1.4s, v16.4h, v12.h[1]
    smlsl v1.4s, v17.4h, v11.h[1]
    smlsl v1.4s, v18.4h, v13.h[5]
    smlal v1.4s, v19.4h, v10.h[3]
    smlal v1.4s, v20.4h, v15.h[1]
    smlsl v1.4s, v21.4h, v10.h[2]
    smlal v1.4s, v22.4h, v14.h[7]
    smlal v1.4s, v23.4h, v10.h[4]
    smlsl v1.4s, v24.4h, v13.h[3]
    smlsl v1.4s, v25.4h, v11.h[2]
    smlal v1.4s, v26.4h, v12.h[0]
    smlal v1.4s, v27.4h, v12.h[3]
    smlsl v1.4s, v28.4h, v11.h[0]
    smlsl v1.4s, v29.4h, v13.h[7]
    smlal v1.4s, v30.4h, v10.h[3]
    smlal v1.4s, v31.4h, v15.h[4]    // O[17]

    smull v2.4s, v16.4h, v12.h[3]
    smlsl v2.4s, v17.4h, v10.h[6]
    smlsl v2.4s, v18.4h, v14.h[7]
    smlal v2.4s, v19.4h, v10.h[2]
    smlsl v2.4s, v20.4h, v14.h[1]
    smlsl v2.4s, v21.4h, v11.h[1]
    smlal v2.4s, v22.4h, v11.h[6]
    smlal v2.4s, v23.4h, v13.h[1]
    smlsl v2.4s, v24.4h, v10.h[3]
    smlsl v2.4s, v25.4h, v15.h[6]
    smlal v2.4s, v26.4h, v10.h[3]
    smlsl v2.4s, v27.4h, v13.h[3]
    smlsl v2.4s, v28.4h, v11.h[5]
    smlal v2.4s, v29.4h, v11.h[2]
    smlal v2.4s, v30.4h, v13.h[7]
    smlsl v2.4s, v31.4h, v10.h[2]    // O[18]

    smull v3.4s, v16.4h, v12.h[5]
    smlsl v3.4s, v17.4h, v10.h[3]
    smlal v3.4s, v18.4h, v15.h[4]
    smlal v3.4s, v19.4h, v10.h[6]
    smlsl v3.4s, v20.4h, v12.h[0]
    smlsl v3.4s, v21.4h, v13.h[3]
    smlal v3.4s, v22.4h, v10.h[2]
    smlsl v3.4s, v23.4h, v14.h[5]
    smlsl v3.4s, v24.4h, v11.h[1]
    smlal v3.4s, v25.4h, v11.h[3]
    smlal v3.4s, v26.4h, v14.h[1]
    smlsl v3.4s, v27.4h, v10.h[2]
    smlal v3.4s, v28.4h, v13.h[7]
    smlal v3.4s, v29.4h, v11.h[5]
    smlsl v3.4s, v30.4h, v11.h[0]
    smlsl v3.4s, v31.4h, v14.h[7]    // O[19]

    smull v4.4s, v16.4h, v12.h[7]
    smlsl v4.4s, v17.4h, v10.h[2]
    smlal v4.4s, v18.4h, v14.h[1]
    smlal v4.4s, v19.4h, v11.h[6]
    smlsl v4.4s, v20.4h, v10.h[5]
    smlal v4.4s, v21.4h, v15.h[4]
    smlal v4.4s, v22.4h, v11.h[0]
    smlsl v4.4s, v23.4h, v11.h[2]
    smlsl v4.4s, v24.4h, v14.h[7]
    smlal v4.4s, v25.4h, v10.h[3]
    smlsl v4.4s, v26.4h, v12.h[1]
    smlsl v4.4s, v27.4h, v13.h[5]
    smlal v4.4s, v28.4h, v10.h[2]
    smlsl v4.4s, v29.4h, v13.h[3]
    smlsl v4.4s, v30.4h, v12.h[3]
    smlal v4.4s, v31.4h, v10.h[3]    // O[20]

    smull v5.4s, v16.4h, v13.h[1]
    smlsl v5.4s, v17.4h, v10.h[2]
    smlal v5.4s, v18.4h, v12.h[7]
    smlal v5.4s, v19.4h, v13.h[3]
    smlsl v5.4s, v20.4h, v10.h[2]
    smlal v5.4s, v21.4h, v12.h[5]
    smlal v5.4s, v22.4h, v13.h[5]
    smlsl v5.4s, v23.4h, v10.h[2]
    smlal v5.4s, v24.4h, v12.h[3]
    smlal v5.4s, v25.4h, v13.h[7]
    smlsl v5.4s, v26.4h, v10.h[2]
    smlal v5.4s, v27.4h, v12.h[1]
    smlal v5.4s, v28.4h, v14.h[1]
    smlsl v5.4s, v29.4h, v10.h[3]
    smlal v5.4s, v30.4h, v12.h[0]
    smlal v5.4s, v31.4h, v14.h[3]    // O[21]

    smull v6.4s, v16.4h, v13.h[3]
    smlsl v6.4s, v17.4h, v10.h[2]
    smlal v6.4s, v18.4h, v11.h[6]
    smlal v6.4s, v19.4h, v15.h[1]
    smlsl v6.4s, v20.4h, v11.h[0]
    smlal v6.4s, v21.4h, v10.h[6]
    smlsl v6.4s, v22.4h, v14.h[5]
    smlsl v6.4s, v23.4h, v12.h[1]
    smlal v6.4s, v24.4h, v10.h[2]
    smlsl v6.4s, v25.4h, v12.h[7]
    smlsl v6.4s, v26.4h, v13.h[7]
    smlal v6.4s, v27.4h, v10.h[3]
    smlsl v6.4s, v28.4h, v11.h[3]
    smlsl v6.4s, v29.4h, v15.h[6]
    smlal v6.4s, v30.4h, v11.h[2]
    smlsl v6.4s, v31.4h, v10.h[4]    // O[22]

    smull v7.4s, v16.4h, v13.h[5]
    smlsl v7.4s, v17.4h, v10.h[4]
    smlal v7.4s, v18.4h, v11.h[0]
    smlsl v7.4s, v19.4h, v14.h[5]
    smlsl v7.4s, v20.4h, v12.h[5]
    smlal v7.4s, v21.4h, v10.h[2]
    smlsl v7.4s, v22.4h, v11.h[5]
    smlal v7.4s, v23.4h, v15.h[6]
    smlal v7.4s, v24.4h, v11.h[6]
    smlsl v7.4s, v25.4h, v10.h[2]
    smlal v7.4s, v26.4h, v12.h[3]
    smlal v7.4s, v27.4h, v14.h[7]
    smlsl v7.4s, v28.4h, v11.h[1]
    smlal v7.4s, v29.4h, v10.h[3]
    smlsl v7.4s, v30.4h, v13.h[3]
    smlsl v7.4s, v31.4h, v13.h[7]    // O[23]

    st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x6], #64
    st1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x6], #64

    smull v0.4s, v16.4h, v13.h[7]
    smlsl v0.4s, v17.4h, v10.h[7]
    smlal v0.4s, v18.4h, v10.h[3]
    smlsl v0.4s, v19.4h, v12.h[7]
    smlsl v0.4s, v20.4h, v14.h[7]
    smlal v0.4s, v21.4h, v11.h[3]
    smlsl v0.4s, v22.4h, v10.h[2]
    smlal v0.4s, v23.4h, v12.h[0]
    smlsl v0.4s, v24.4h, v15.h[6]
    smlsl v0.4s, v25.4h, v12.h[1]
    smlal v0.4s, v26.4h, v10.h[2]
    smlsl v0.4s, v27.4h, v11.h[2]
    smlal v0.4s, v28.4h, v14.h[5]
    smlal v0.4s, v29.4h, v13.h[1]
    smlsl v0.4s, v30.4h, v10.h[4]
    smlal v0.4s, v31.4h, v10.h[6]    // O[24]

    smull v1.4s, v16.4h, v14.h[1]
    smlsl v1.4s, v17.4h, v11.h[2]
    smlal v1.4s, v18.4h, v10.h[2]
    smlsl v1.4s, v19.4h, v11.h[3]
    smlal v1.4s, v20.4h, v14.h[3]
    smlal v1.4s, v21.4h, v13.h[7]
    smlsl v1.4s, v22.4h, v11.h[1]
    smlal v1.4s, v23.4h, v10.h[2]
    smlsl v1.4s, v24.4h, v11.h[5]
    smlal v1.4s, v25.4h, v14.h[5]
    smlal v1.4s, v26.4h, v13.h[5]
    smlsl v1.4s, v27.4h, v11.h[0]
    smlal v1.4s, v28.4h, v10.h[2]
    smlsl v1.4s, v29.4h, v11.h[6]
    smlal v1.4s, v30.4h, v14.h[7]
    smlal v1.4s, v31.4h, v13.h[3]    // O[25]

    smull v2.4s, v16.4h, v14.h[3]
    smlsl v2.4s, v17.4h, v11.h[6]
    smlal v2.4s, v18.4h, v10.h[3]
    smlsl v2.4s, v19.4h, v10.h[4]
    smlal v2.4s, v20.4h, v12.h[1]
    smlsl v2.4s, v21.4h, v14.h[7]
    smlsl v2.4s, v22.4h, v13.h[7]
    smlal v2.4s, v23.4h, v11.h[3]
    smlsl v2.4s, v24.4h, v10.h[2]
    smlal v2.4s, v25.4h, v10.h[6]
    smlsl v2.4s, v26.4h, v12.h[5]
    smlal v2.4s, v27.4h, v15.h[4]
    smlal v2.4s, v28.4h, v13.h[3]
    smlsl v2.4s, v29.4h, v11.h[1]
    smlal v2.4s, v30.4h, v10.h[2]
    smlsl v2.4s, v31.4h, v11.h[0]    // O[26]

    smull v3.4s, v16.4h, v14.h[5]
    smlsl v3.4s, v17.4h, v12.h[3]
    smlal v3.4s, v18.4h, v10.h[7]
    smlsl v3.4s, v19.4h, v10.h[2]
    smlal v3.4s, v20.4h, v10.h[6]
    smlsl v3.4s, v21.4h, v12.h[1]
    smlal v3.4s, v22.4h, v14.h[3]
    smlal v3.4s, v23.4h, v14.h[7]
    smlsl v3.4s, v24.4h, v12.h[5]
    smlal v3.4s, v25.4h, v11.h[0]
    smlsl v3.4s, v26.4h, v10.h[2]
    smlal v3.4s, v27.4h, v10.h[5]
    smlsl v3.4s, v28.4h, v12.h[0]
    smlal v3.4s, v29.4h, v14.h[1]
    smlal v3.4s, v30.4h, v15.h[1]
    smlsl v3.4s, v31.4h, v12.h[7]    // O[27]

    smull v4.4s, v16.4h, v14.h[7]
    smlsl v4.4s, v17.4h, v13.h[1]
    smlal v4.4s, v18.4h, v11.h[5]
    smlsl v4.4s, v19.4h, v10.h[5]
    smlal v4.4s, v20.4h, v10.h[2]
    smlsl v4.4s, v21.4h, v10.h[4]
    smlal v4.4s, v22.4h, v11.h[3]
    smlsl v4.4s, v23.4h, v12.h[7]
    smlal v4.4s, v24.4h, v14.h[5]
    smlal v4.4s, v25.4h, v15.h[1]
    smlsl v4.4s, v26.4h, v13.h[3]
    smlal v4.4s, v27.4h, v11.h[6]
    smlsl v4.4s, v28.4h, v10.h[6]
    smlal v4.4s, v29.4h, v10.h[2]
    smlsl v4.4s, v30.4h, v10.h[3]
    smlal v4.4s, v31.4h, v11.h[2]    // O[28]

    smull v5.4s, v16.4h, v15.h[1]
    smlsl v5.4s, v17.4h, v13.h[7]
    smlal v5.4s, v18.4h, v12.h[5]
    smlsl v5.4s, v19.4h, v11.h[5]
    smlal v5.4s, v20.4h, v10.h[7]
    smlsl v5.4s, v21.4h, v10.h[3]
    smlal v5.4s, v22.4h, v10.h[2]
    smlsl v5.4s, v23.4h, v10.h[3]
    smlal v5.4s, v24.4h, v11.h[0]
    smlsl v5.4s, v25.4h, v11.h[6]
    smlal v5.4s, v26.4h, v12.h[7]
    smlsl v5.4s, v27.4h, v14.h[1]
    smlal v5.4s, v28.4h, v15.h[4]
    smlal v5.4s, v29.4h, v14.h[7]
    smlsl v5.4s, v30.4h, v13.h[5]
    smlal v5.4s, v31.4h, v12.h[3]    // O[29]

    smull v6.4s, v16.4h, v15.h[4]
    smlsl v6.4s, v17.4h, v14.h[5]
    smlal v6.4s, v18.4h, v13.h[7]
    smlsl v6.4s, v19.4h, v13.h[1]
    smlal v6.4s, v20.4h, v12.h[3]
    smlsl v6.4s, v21.4h, v11.h[6]
    smlal v6.4s, v22.4h, v11.h[2]
    smlsl v6.4s, v23.4h, v10.h[7]
    smlal v6.4s, v24.4h, v10.h[4]
    smlsl v6.4s, v25.4h, v10.h[2]
    smlal v6.4s, v26.4h, v10.h[2]
    smlsl v6.4s, v27.4h, v10.h[2]
    smlal v6.4s, v28.4h, v10.h[3]
    smlsl v6.4s, v29.4h, v10.h[6]
    smlal v6.4s, v30.4h, v11.h[1]
    smlsl v6.4s, v31.4h, v11.h[5]    // O[30]

    smull v7.4s, v16.4h, v15.h[6]
    smlsl v7.4s, v17.4h, v15.h[4]
    smlal v7.4s, v18.4h, v15.h[1]
    smlsl v7.4s, v19.4h, v14.h[7]
    smlal v7.4s, v20.4h, v14.h[5]
    smlsl v7.4s, v21.4h, v14.h[3]
    smlal v7.4s, v22.4h, v14.h[1]
    smlsl v7.4s, v23.4h, v13.h[7]
    smlal v7.4s, v24.4h, v13.h[5]
    smlsl v7.4s, v25.4h, v13.h[3]
    smlal v7.4s, v26.4h, v13.h[1]
    smlsl v7.4s, v27.4h, v12.h[7]
    smlal v7.4s, v28.4h, v12.h[5]
    smlsl v7.4s, v29.4h, v12.h[3]
    smlal v7.4s, v30.4h, v12.h[1]
    smlsl v7.4s, v31.4h, v12.h[0]    // O[31]

.endm

//************************************************************************
//void dct2_butterfly_h64_arm64(s16 *src, int i_src, s16 *dst, int width, int shift, int bit_depth);
//x0: coeff blk, 16 bit
//x1: i_src
//x2: resi blk, 16 bit
//x3: blk width
//x4: shift
//x5: bit_depth
//************************************************************************
function dct2_butterfly_h64_arm64
    sub sp, sp, #128
    st1 {v8.4s, v9.4s, v10.4s, v11.4s}, [sp], #64
    st1 {v12.4s, v13.4s, v14.4s, v15.4s}, [sp]
    sub sp, sp, #64

    mov w6, #47
    mov w7, #46
    mov w8, #45
    mov w9, #44
    mov w10, #43
    mov w11, #42
    mov w12, #41
    mov w13, #40
    ins v10.h[0], w6
    ins v10.h[1], w7
    ins v10.h[2], w8
    ins v10.h[3], w9
    ins v10.h[4], w10
    ins v10.h[5], w11
    ins v10.h[6], w12
    ins v10.h[7], w13

    movi v0.8h, #8
    movi v1.8h, #16
    movi v2.8h, #24
    movi v3.8h, #32
    movi v4.8h, #40
    sub v11.8h, v10.8h, v0.8h
    sub v12.8h, v10.8h, v1.8h
    sub v13.8h, v10.8h, v2.8h
    sub v14.8h, v10.8h, v3.8h
    sub v15.8h, v10.8h, v4.8h

    sub sp, sp, #1024               // tmp buffer

    lsl x1, x1, #1                  // i_src = width * sizeof(s16)
    mov x8, #0                      // i = 0
    lsl x11, x1, #3                 // 8*i_src
    lsl x12, x1, #2                 // 4*i_src
    lsl x13, x1, #1                 // 2*i_src
    mov x14, #128                   // i_dst = 64 * sizeof(s16)

    cmp w5, #15
    bne dct2_h64_2nd_loopx

dct2_h64_1st_loopx:

    dct2_h64_w4_calcu_E_O_arm64

    // load E[24-31]
    add x6, sp, #384
    add x7, sp, #384
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64

    // save coefs: v10 v11
    st1 {v10.4s, v11.4s}, [x7]

    add v24.4s, v16.4s, v0.4s       // dst[24]
    add v25.4s, v17.4s, v1.4s       // dst[25]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    add x9, x2, #48
    add x15, x2, #64
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[24-31]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s       // dst[32]
    sub v25.4s, v22.4s, v6.4s       // dst[33]
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[32-39]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    // calculate dst[0-7] dst[56-63]
    mov x6, sp
    add x7, sp, #512
    mov x9, x2                      // dst
    add x15, x2, #112               // &dst[56]

    // load E[0-7] O[0-7]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x7], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x7], #64

    add v24.4s, v16.4s, v0.4s       // dst[0]
    add v25.4s, v17.4s, v1.4s       // dst[1]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h       // dst[0]
    trn2 v25.4h, v8.4h, v9.4h       // dst[1]
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[0-7]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s
    sub v25.4s, v22.4s, v6.4s
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h       // dst[56]
    trn2 v25.4h, v8.4h, v9.4h       // dst[57]
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h       // dst[60]
    trn2 v29.4h, v8.4h, v9.4h       // dst[61]
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[56-63]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    // calculate dst[8-15] dst[48-55]
    // load E[8-15] O[8-15]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x7], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x7], #64

    add v24.4s, v16.4s, v0.4s       // dst[8]
    add v25.4s, v17.4s, v1.4s       // dst[9]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    add x9, x2, #16                 // &dst[8]
    add x15, x2, #96                // &dst[48]
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[8-15]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s       // dst[48]
    sub v25.4s, v22.4s, v6.4s       // dst[49]
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[48-55]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    // calculate dst[16-23] dst[40-47]
    // load E[16-23] O[16-23]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x7], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x7], #64

    add v24.4s, v16.4s, v0.4s       // dst[16]
    add v25.4s, v17.4s, v1.4s       // dst[17]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    add  x9, x2, #32                // &dst[16]
    add  x15, x2, #80               // &dst[40]
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[16-23]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s       // dst[40]
    sub v25.4s, v22.4s, v6.4s       // dst[41]
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

    sqrshrn v24.4h, v24.4s, #5
    sqrshrn v25.4h, v25.4s, #5
    sqrshrn v26.4h, v26.4s, #5
    sqrshrn v27.4h, v27.4s, #5
    sqrshrn v28.4h, v28.4s, #5
    sqrshrn v29.4h, v29.4s, #5
    sqrshrn v30.4h, v30.4s, #5
    sqrshrn v31.4h, v31.4s, #5

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    add x6, sp, #384
    // store dst[40-47]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    ld1 {v10.4s, v11.4s}, [x6]
    add x8, x8, #4
    add x0, x0, #8                  // src += 4
    add x2, x2, #512                // dst += i_dst*4
    cmp x8, x3
    blt dct2_h64_1st_loopx

    b dct2_h64_end

dct2_h64_2nd_loopx:

    dct2_h64_w4_calcu_E_O_arm64

    // load E[24-31]
    add x6, sp, #384
    add x7, sp, #384
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64

    // save coefs
    st1 {v10.4s, v11.4s, v12.4s, v13.4s}, [x7]

    mov w9, #1
    lsl w9, w9, w5
    sub w10, w5, #20                // -shift = bit_depth - 20
    sub w15, w9, #1                 // max_pel = (1<<bit_depth) - 1
    neg w9, w9                      // min_pel = -(1<<bit_depth)
    dup v12.8h, w9                  // min_pel
    dup v13.8h, w15                 // max_pel

    add v24.4s, v16.4s, v0.4s       // dst[24]
    add v25.4s, v17.4s, v1.4s       // dst[25]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    add x9, x2, #48
    add x15, x2, #64
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    // store dst[24-31]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s       // dst[32]
    sub v25.4s, v22.4s, v6.4s       // dst[33]
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    // store dst[32-39]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    // calculate dst[0-7] dst[56-63]
    mov x6, sp
    add x7, sp, #512
    mov x9, x2                      // dst
    add x15, x2, #112               // &dst[56]

    // load E[0-7] O[0-7]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x7], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x7], #64

    add v24.4s, v16.4s, v0.4s       // dst[0]
    add v25.4s, v17.4s, v1.4s       // dst[1]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h       // dst[0]
    trn2 v25.4h, v8.4h, v9.4h       // dst[1]
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    // store dst[0-7]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s
    sub v25.4s, v22.4s, v6.4s
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h       // dst[56]
    trn2 v25.4h, v8.4h, v9.4h       // dst[57]
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h       // dst[60]
    trn2 v29.4h, v8.4h, v9.4h       // dst[61]
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    // store dst[56-63]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    // calculate dst[8-15] dst[48-55]
    // load E[8-15] O[8-15]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x7], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x7], #64

    add v24.4s, v16.4s, v0.4s       // dst[8]
    add v25.4s, v17.4s, v1.4s       // dst[9]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    add x9, x2, #16                 // &dst[8]
    add x15, x2, #96                // &dst[48]
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    // store dst[8-15]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s       // dst[48]
    sub v25.4s, v22.4s, v6.4s       // dst[49]
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    // store dst[48-55]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    // calculate dst[16-23] dst[40-47]
    // load E[16-23] O[16-23]
    ld1 {v16.4s, v17.4s, v18.4s, v19.4s}, [x6], #64
    ld1 {v20.4s, v21.4s, v22.4s, v23.4s}, [x6], #64
    ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x7], #64
    ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [x7], #64

    add v24.4s, v16.4s, v0.4s       // dst[16]
    add v25.4s, v17.4s, v1.4s       // dst[17]
    add v26.4s, v18.4s, v2.4s
    add v27.4s, v19.4s, v3.4s
    add v28.4s, v20.4s, v4.4s
    add v29.4s, v21.4s, v5.4s
    add v30.4s, v22.4s, v6.4s
    add v31.4s, v23.4s, v7.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    add  x9, x2, #32                // &dst[16]
    add  x15, x2, #80               // &dst[40]
    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    // store dst[16-23]
    st1 {v24.4s}, [x9], x14
    st1 {v25.4s}, [x9], x14
    st1 {v26.4s}, [x9], x14
    st1 {v27.4s}, [x9], x14

    sub v24.4s, v23.4s, v7.4s       // dst[40]
    sub v25.4s, v22.4s, v6.4s       // dst[41]
    sub v26.4s, v21.4s, v5.4s
    sub v27.4s, v20.4s, v4.4s
    sub v28.4s, v19.4s, v3.4s
    sub v29.4s, v18.4s, v2.4s
    sub v30.4s, v17.4s, v1.4s
    sub v31.4s, v16.4s, v0.4s

#if !COMPILE_10BIT
    sqrshrn v24.4h, v24.4s, #12
    sqrshrn v25.4h, v25.4s, #12
    sqrshrn v26.4h, v26.4s, #12
    sqrshrn v27.4h, v27.4s, #12
    sqrshrn v28.4h, v28.4s, #12
    sqrshrn v29.4h, v29.4s, #12
    sqrshrn v30.4h, v30.4s, #12
    sqrshrn v31.4h, v31.4s, #12
#else
    dup   v8.4s, w10                  // for left shift
    srshl v24.4s, v24.4s, v8.4s
    srshl v25.4s, v25.4s, v8.4s
    srshl v26.4s, v26.4s, v8.4s
    srshl v27.4s, v27.4s, v8.4s
    srshl v28.4s, v28.4s, v8.4s
    srshl v29.4s, v29.4s, v8.4s
    srshl v30.4s, v30.4s, v8.4s
    srshl v31.4s, v31.4s, v8.4s

    sqxtn v24.4h, v24.4s
    sqxtn v25.4h, v25.4s
    sqxtn v26.4h, v26.4s
    sqxtn v27.4h, v27.4s
    sqxtn v28.4h, v28.4s
    sqxtn v29.4h, v29.4s
    sqxtn v30.4h, v30.4s
    sqxtn v31.4h, v31.4s
#endif

    trn1 v8.2s, v24.2s, v26.2s
    trn1 v9.2s, v25.2s, v27.2s
    trn2 v10.2s, v24.2s, v26.2s
    trn2 v11.2s, v25.2s, v27.2s
    trn1 v24.4h, v8.4h, v9.4h
    trn2 v25.4h, v8.4h, v9.4h
    trn1 v26.4h, v10.4h, v11.4h
    trn2 v27.4h, v10.4h, v11.4h

    trn1 v8.2s, v28.2s, v30.2s
    trn1 v9.2s, v29.2s, v31.2s
    trn2 v10.2s, v28.2s, v30.2s
    trn2 v11.2s, v29.2s, v31.2s
    trn1 v28.4h, v8.4h, v9.4h
    trn2 v29.4h, v8.4h, v9.4h
    trn1 v30.4h, v10.4h, v11.4h
    trn2 v31.4h, v10.4h, v11.4h

    trn1 v24.2d, v24.2d, v28.2d
    trn1 v25.2d, v25.2d, v29.2d
    trn1 v26.2d, v26.2d, v30.2d
    trn1 v27.2d, v27.2d, v31.2d

    smax v24.8h, v24.8h, v12.8h
    smax v25.8h, v25.8h, v12.8h
    smax v26.8h, v26.8h, v12.8h
    smax v27.8h, v27.8h, v12.8h
    smin v24.8h, v24.8h, v13.8h
    smin v25.8h, v25.8h, v13.8h
    smin v26.8h, v26.8h, v13.8h
    smin v27.8h, v27.8h, v13.8h

    add x6, sp, #384
    // store dst[40-47]
    st1 {v24.4s}, [x15], x14
    st1 {v25.4s}, [x15], x14
    st1 {v26.4s}, [x15], x14
    st1 {v27.4s}, [x15], x14

    ld1 {v10.4s, v11.4s, v12.4s, v13.4s}, [x6]
    add x8, x8, #4
    add x0, x0, #8                  // src += 4
    add x2, x2, #512                // dst += i_dst*4
    cmp x8, x3
    blt dct2_h64_2nd_loopx
dct2_h64_end:
    add sp, sp, #1024
    ld1 {v8.4s, v9.4s, v10.4s, v11.4s}, [sp], #64
    ld1 {v12.4s, v13.4s, v14.4s, v15.4s}, [sp], #64

    ret

itrans_dct8_pb4_coef:
.byte 42, 37, 27, 15, \
      37, 0, -37, -37, \
      27, -37, -15, 42, \
      15, -37, 42, -27
//*************************************************************************************************
//void uavs3e_itrans_dct8_pb4_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
//x0->coeff blk, 16 bit
//x1->resi blk, 16 bit
//*************************************************************************************************
function uavs3e_itrans_dct8_pb4_arm64
    adr x6, itrans_dct8_pb4_coef
    neg w15, w2
    dup v28.8h, w4
    dup v29.8h, w5
    dup v30.4s, w15

    //load 变换系数(4x4)
    ld1 {v0.16b}, [x6]

    //扩展到16bit
    sxtl2 v2.8h, v0.16b
    sxtl  v0.8h, v0.8b
    ext   v1.16b, v0.16b, v0.16b, #8
    ext   v3.16b, v2.16b, v2.16b, #8

    lsl x3, x3, #1

    cmp x2, #5
    bne dct8_pb4_2nd

    mov x8, #0
loopi_1st:
    add x9, x8, x0
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9]

    smull v16.4s, v0.4h, v4.h[0]
    smull v17.4s, v0.4h, v4.h[1]
    smull v18.4s, v0.4h, v4.h[2]
    smull v19.4s, v0.4h, v4.h[3]
    smlal v16.4s, v1.4h, v5.h[0]
    smlal v17.4s, v1.4h, v5.h[1]
    smlal v18.4s, v1.4h, v5.h[2]
    smlal v19.4s, v1.4h, v5.h[3]
    smlal v16.4s, v2.4h, v6.h[0]
    smlal v17.4s, v2.4h, v6.h[1]
    smlal v18.4s, v2.4h, v6.h[2]
    smlal v19.4s, v2.4h, v6.h[3]
    smlal v16.4s, v3.4h, v7.h[0]
    smlal v17.4s, v3.4h, v7.h[1]
    smlal v18.4s, v3.4h, v7.h[2]
    smlal v19.4s, v3.4h, v7.h[3]

    sqrshrn  v16.4h, v16.4s, #5
    sqrshrn2 v16.8h, v17.4s, #5
    sqrshrn  v17.4h, v18.4s, #5
    sqrshrn2 v17.8h, v19.4s, #5

    add x8, x8, #8
    smin v16.8h, v16.8h, v28.8h
    smin v17.8h, v17.8h, v28.8h
    smax v16.8h, v16.8h, v29.8h
    smax v17.8h, v17.8h, v29.8h

    cmp x8, x3
    st1 {v16.8h, v17.8h}, [x1], #32
    blt loopi_1st
    b   dct8_pb4_end

dct8_pb4_2nd:
    mov x8, #0                  // i = 0
loopi_2nd:
    add x9, x8, x0
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9]

    smull v16.4s, v0.4h, v4.h[0]
    smull v17.4s, v0.4h, v4.h[1]
    smull v18.4s, v0.4h, v4.h[2]
    smull v19.4s, v0.4h, v4.h[3]
    smlal v16.4s, v1.4h, v5.h[0]
    smlal v17.4s, v1.4h, v5.h[1]
    smlal v18.4s, v1.4h, v5.h[2]
    smlal v19.4s, v1.4h, v5.h[3]
    smlal v16.4s, v2.4h, v6.h[0]
    smlal v17.4s, v2.4h, v6.h[1]
    smlal v18.4s, v2.4h, v6.h[2]
    smlal v19.4s, v2.4h, v6.h[3]
    smlal v16.4s, v3.4h, v7.h[0]
    smlal v17.4s, v3.4h, v7.h[1]
    smlal v18.4s, v3.4h, v7.h[2]
    smlal v19.4s, v3.4h, v7.h[3]

    srshl v16.4s, v16.4s, v30.4s
    srshl v17.4s, v17.4s, v30.4s
    srshl v18.4s, v18.4s, v30.4s
    srshl v19.4s, v19.4s, v30.4s

    sqxtn  v16.4h, v16.4s
    sqxtn2 v16.8h, v17.4s
    sqxtn  v17.4h, v18.4s
    sqxtn2 v17.8h, v19.4s

    add x8, x8, #8
    smin v16.8h, v16.8h, v28.8h
    smin v17.8h, v17.8h, v28.8h
    smax v16.8h, v16.8h, v29.8h
    smax v17.8h, v17.8h, v29.8h

    cmp x8, x3
    st1 {v16.8h, v17.8h}, [x1], #32

    blt loopi_2nd

dct8_pb4_end:
    ret

itrans_dct8_pb8_coef:
.byte 44, 42, 39, 35, 30, 23, 16, 8, \
      42, 30, 8, -16, -35, -44, -39, -23, \
      39, 8, -30, -44, -23, 16, 42, 35, \
      35, -16, -44, -8, 39, 30, -23, -42, \
      30, -35, -23, 39, 16, -42, -8, 44, \
      23, -44, 16, 30, -42, 8, 35, -39, \
      16, -39, 42, -23, -8, 35, -44, 30, \
      8, -23, 35, -42, 44, -39, 30, -16
//*************************************************************************************************
//void uavs3e_itrans_dct8_pb8_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
//x0->coeff blk, 16 bit
//x1->resi blk, 16 bit
//*************************************************************************************************
function uavs3e_itrans_dct8_pb8_arm64
    sub sp, sp, #64
    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]
    adr x6, itrans_dct8_pb8_coef

    neg w15, w2
    dup v28.8h, w4                  // max_tr_val
    dup v29.8h, w5                  // min_tr_val
    dup v31.4s, w15                 // shift

    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x6], #32
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x6]
    sxtl v16.8h, v0.8b
    sxtl v17.8h, v1.8b
    sxtl v18.8h, v2.8b
    sxtl v19.8h, v3.8b
    sxtl v20.8h, v4.8b
    sxtl v21.8h, v5.8b
    sxtl v22.8h, v6.8b
    sxtl v23.8h, v7.8b

    lsl x3, x3, #1
    mov x8, #0                      // i = 0
dct8_pb8_loopi:
    add x9, x0, x8
    ld1 {v0.4h}, [x9], x3
    ld1 {v1.4h}, [x9], x3
    ld1 {v2.4h}, [x9], x3
    ld1 {v3.4h}, [x9], x3
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9]

    smull v24.4s, v16.4h, v0.h[0]
    smull v25.4s, v16.4h, v0.h[1]
    smull v26.4s, v16.4h, v0.h[2]
    smull v27.4s, v16.4h, v0.h[3]
    smlal v24.4s, v17.4h, v1.h[0]
    smlal v25.4s, v17.4h, v1.h[1]
    smlal v26.4s, v17.4h, v1.h[2]
    smlal v27.4s, v17.4h, v1.h[3]
    smlal v24.4s, v18.4h, v2.h[0]
    smlal v25.4s, v18.4h, v2.h[1]
    smlal v26.4s, v18.4h, v2.h[2]
    smlal v27.4s, v18.4h, v2.h[3]
    smlal v24.4s, v19.4h, v3.h[0]
    smlal v25.4s, v19.4h, v3.h[1]
    smlal v26.4s, v19.4h, v3.h[2]
    smlal v27.4s, v19.4h, v3.h[3]
    smlal v24.4s, v20.4h, v4.h[0]
    smlal v25.4s, v20.4h, v4.h[1]
    smlal v26.4s, v20.4h, v4.h[2]
    smlal v27.4s, v20.4h, v4.h[3]
    smlal v24.4s, v21.4h, v5.h[0]
    smlal v25.4s, v21.4h, v5.h[1]
    smlal v26.4s, v21.4h, v5.h[2]
    smlal v27.4s, v21.4h, v5.h[3]
    smlal v24.4s, v22.4h, v6.h[0]
    smlal v25.4s, v22.4h, v6.h[1]
    smlal v26.4s, v22.4h, v6.h[2]
    smlal v27.4s, v22.4h, v6.h[3]
    smlal v24.4s, v23.4h, v7.h[0]
    smlal v25.4s, v23.4h, v7.h[1]
    smlal v26.4s, v23.4h, v7.h[2]
    smlal v27.4s, v23.4h, v7.h[3]

    smull2 v12.4s, v16.8h, v0.h[0]
    smull2 v13.4s, v16.8h, v0.h[1]
    smull2 v14.4s, v16.8h, v0.h[2]
    smull2 v15.4s, v16.8h, v0.h[3]
    smlal2 v12.4s, v17.8h, v1.h[0]
    smlal2 v13.4s, v17.8h, v1.h[1]
    smlal2 v14.4s, v17.8h, v1.h[2]
    smlal2 v15.4s, v17.8h, v1.h[3]
    smlal2 v12.4s, v18.8h, v2.h[0]
    smlal2 v13.4s, v18.8h, v2.h[1]
    smlal2 v14.4s, v18.8h, v2.h[2]
    smlal2 v15.4s, v18.8h, v2.h[3]
    smlal2 v12.4s, v19.8h, v3.h[0]
    smlal2 v13.4s, v19.8h, v3.h[1]
    smlal2 v14.4s, v19.8h, v3.h[2]
    smlal2 v15.4s, v19.8h, v3.h[3]
    smlal2 v12.4s, v20.8h, v4.h[0]
    smlal2 v13.4s, v20.8h, v4.h[1]
    smlal2 v14.4s, v20.8h, v4.h[2]
    smlal2 v15.4s, v20.8h, v4.h[3]
    smlal2 v12.4s, v21.8h, v5.h[0]
    smlal2 v13.4s, v21.8h, v5.h[1]
    smlal2 v14.4s, v21.8h, v5.h[2]
    smlal2 v15.4s, v21.8h, v5.h[3]
    smlal2 v12.4s, v22.8h, v6.h[0]
    smlal2 v13.4s, v22.8h, v6.h[1]
    smlal2 v14.4s, v22.8h, v6.h[2]
    smlal2 v15.4s, v22.8h, v6.h[3]
    smlal2 v12.4s, v23.8h, v7.h[0]
    smlal2 v13.4s, v23.8h, v7.h[1]
    smlal2 v14.4s, v23.8h, v7.h[2]
    smlal2 v15.4s, v23.8h, v7.h[3]

    cmp x2, #5
    bne dct8_pb8_shift12

    sqrshrn  v24.4h, v24.4s, #5
    sqrshrn  v25.4h, v25.4s, #5
    sqrshrn  v26.4h, v26.4s, #5
    sqrshrn  v27.4h, v27.4s, #5
    sqrshrn2 v24.8h, v12.4s, #5
    sqrshrn2 v25.8h, v13.4s, #5
    sqrshrn2 v26.8h, v14.4s, #5
    sqrshrn2 v27.8h, v15.4s, #5

    b dct8_pb8_clip_store

dct8_pb8_shift12:
	srshl v24.4s, v24.4s, v31.4s
    srshl v25.4s, v25.4s, v31.4s
    srshl v26.4s, v26.4s, v31.4s
    srshl v27.4s, v27.4s, v31.4s
    srshl v12.4s, v12.4s, v31.4s
    srshl v13.4s, v13.4s, v31.4s
    srshl v14.4s, v14.4s, v31.4s
    srshl v15.4s, v15.4s, v31.4s

    sqxtn  v24.4h, v24.4s
    sqxtn  v25.4h, v25.4s
    sqxtn  v26.4h, v26.4s
    sqxtn  v27.4h, v27.4s
    sqxtn2 v24.8h, v12.4s
    sqxtn2 v25.8h, v13.4s
    sqxtn2 v26.8h, v14.4s
    sqxtn2 v27.8h, v15.4s

dct8_pb8_clip_store:
    add x8, x8, #8
    smin v24.8h, v24.8h, v28.8h
    smin v25.8h, v25.8h, v28.8h
    smin v26.8h, v26.8h, v28.8h
    smin v27.8h, v27.8h, v28.8h
    smax v24.8h, v24.8h, v29.8h
    smax v25.8h, v25.8h, v29.8h
    smax v26.8h, v26.8h, v29.8h
    smax v27.8h, v27.8h, v29.8h

    cmp x8, x3
    st1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x1], #64
    blt dct8_pb8_loopi

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
    ret

itrans_dct8_pb16_coef:
.byte 45, 44, 43, 42, 41, 39, 36, 34, 31, 28, 24, 20, 17, 13, 8, 4, \
      44, 41, 34, 24, 13, 0, -13, -24, -34, -41, -44, -44, -41, -34, -24, -13, \
      43, 34, 17, -4, -24, -39, -45, -41, -28, -8, 13, 31, 42, 44, 36, 20, \
      42, 24, -4, -31, -44, -39, -17, 13, 36, 45, 34, 8, -20, -41, -43, -28, \
      41, 13, -24, -44, -34, 0, 34, 44, 24, -13, -41, -41, -13, 24, 44, 34, \
      39, 0, -39, -39, 0, 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, -39, \
      36, -13, -45, -17, 34, 39, -8, -44, -20, 31, 41, -4, -43, -24, 28, 42, \
      34, -24, -41, 13, 44, 0, -44, -13, 41, 24, -34, -34, 24, 41, -13, -44, \
      31, -34, -28, 36, 24, -39, -20, 41, 17, -42, -13, 43, 8, -44, -4, 45, \
      28, -41, -8, 45, -13, -39, 31, 24, -42, -4, 44, -17, -36, 34, 20, -43, \
      24, -44, 13, 34, -41, 0, 41, -34, -13, 44, -24, -24, 44, -13, -34, 41, \
      20, -44, 31, 8, -41, 39, -4, -34, 43, -17, -24, 45, -28, -13, 42, -36, \
      17, -41, 42, -20, -13, 39, -43, 24, 8, -36, 44, -28, -4, 34, -45, 31, \
      13, -34, 44, -41, 24, 0, -24, 41, -44, 34, -13, -13, 34, -44, 41, -24, \
      8, -24, 36, -43, 44, -39, 28, -13, -4, 20, -34, 42, -45, 41, -31, 17, \
      4, -13, 20, -28, 34, -39, 42, -44, 45, -43, 41, -36, 31, -24, 17, -8
//*************************************************************************************************
//void uavs3e_itrans_dct8_pb16_arm64(s16 *coeff, s16 *resi, int shift, int line, int max_tr_val, int min_tr_val)
//x0->coeff blk, 16 bit
//x1->resi blk, 16 bit
//*************************************************************************************************
function uavs3e_itrans_dct8_pb16_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]
    adr x6, itrans_dct8_pb16_coef

    neg w15, w2
    dup v28.8h, w4                  // max_tr_val
    dup v29.8h, w5                  // min_tr_val
    dup v31.4s, w15                 // shift

    mov x10, #16                    // it_stride
    mov x14, #32                    // resi_stride
    mov x13, x3                     // line - i
    lsl x3, x3, #1                  // coeff_stride = line*sizeof(s16)
dct8_pb16_loopi:
    mov x12, #0                     // j
    mov x11, x1                     // resi + i*16
dct8_pb16_loopj:
    add x8, x6, x12                 // it + j
    mov x9, x0                      // coeff + i

    ld1 {v16.8b}, [x8], x10         // load it first 8 lines
    ld1 {v17.8b}, [x8], x10
    ld1 {v18.8b}, [x8], x10
    ld1 {v19.8b}, [x8], x10
    ld1 {v20.8b}, [x8], x10
    ld1 {v21.8b}, [x8], x10
    ld1 {v22.8b}, [x8], x10
    ld1 {v23.8b}, [x8], x10

    sxtl v16.8h, v16.8b
    sxtl v17.8h, v17.8b
    sxtl v18.8h, v18.8b
    sxtl v19.8h, v19.8b
    sxtl v20.8h, v20.8b
    sxtl v21.8h, v21.8b
    sxtl v22.8h, v22.8b
    sxtl v23.8h, v23.8b

    ld1 {v0.4h}, [x9], x3
    ld1 {v1.4h}, [x9], x3
    ld1 {v2.4h}, [x9], x3
    ld1 {v3.4h}, [x9], x3
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9], x3

    smull v24.4s, v16.4h, v0.h[0]
    smull v25.4s, v16.4h, v0.h[1]
    smull v26.4s, v16.4h, v0.h[2]
    smull v27.4s, v16.4h, v0.h[3]
    smlal v24.4s, v17.4h, v1.h[0]
    smlal v25.4s, v17.4h, v1.h[1]
    smlal v26.4s, v17.4h, v1.h[2]
    smlal v27.4s, v17.4h, v1.h[3]
    smlal v24.4s, v18.4h, v2.h[0]
    smlal v25.4s, v18.4h, v2.h[1]
    smlal v26.4s, v18.4h, v2.h[2]
    smlal v27.4s, v18.4h, v2.h[3]
    smlal v24.4s, v19.4h, v3.h[0]
    smlal v25.4s, v19.4h, v3.h[1]
    smlal v26.4s, v19.4h, v3.h[2]
    smlal v27.4s, v19.4h, v3.h[3]
    smlal v24.4s, v20.4h, v4.h[0]
    smlal v25.4s, v20.4h, v4.h[1]
    smlal v26.4s, v20.4h, v4.h[2]
    smlal v27.4s, v20.4h, v4.h[3]
    smlal v24.4s, v21.4h, v5.h[0]
    smlal v25.4s, v21.4h, v5.h[1]
    smlal v26.4s, v21.4h, v5.h[2]
    smlal v27.4s, v21.4h, v5.h[3]
    smlal v24.4s, v22.4h, v6.h[0]
    smlal v25.4s, v22.4h, v6.h[1]
    smlal v26.4s, v22.4h, v6.h[2]
    smlal v27.4s, v22.4h, v6.h[3]
    smlal v24.4s, v23.4h, v7.h[0]
    smlal v25.4s, v23.4h, v7.h[1]
    smlal v26.4s, v23.4h, v7.h[2]
    smlal v27.4s, v23.4h, v7.h[3]

    smull2 v12.4s, v16.8h, v0.h[0]
    smull2 v13.4s, v16.8h, v0.h[1]
    smull2 v14.4s, v16.8h, v0.h[2]
    smull2 v15.4s, v16.8h, v0.h[3]
    smlal2 v12.4s, v17.8h, v1.h[0]
    smlal2 v13.4s, v17.8h, v1.h[1]
    smlal2 v14.4s, v17.8h, v1.h[2]
    smlal2 v15.4s, v17.8h, v1.h[3]
    smlal2 v12.4s, v18.8h, v2.h[0]
    smlal2 v13.4s, v18.8h, v2.h[1]
    smlal2 v14.4s, v18.8h, v2.h[2]
    smlal2 v15.4s, v18.8h, v2.h[3]
    smlal2 v12.4s, v19.8h, v3.h[0]
    smlal2 v13.4s, v19.8h, v3.h[1]
    smlal2 v14.4s, v19.8h, v3.h[2]
    smlal2 v15.4s, v19.8h, v3.h[3]
    smlal2 v12.4s, v20.8h, v4.h[0]
    smlal2 v13.4s, v20.8h, v4.h[1]
    smlal2 v14.4s, v20.8h, v4.h[2]
    smlal2 v15.4s, v20.8h, v4.h[3]
    smlal2 v12.4s, v21.8h, v5.h[0]
    smlal2 v13.4s, v21.8h, v5.h[1]
    smlal2 v14.4s, v21.8h, v5.h[2]
    smlal2 v15.4s, v21.8h, v5.h[3]
    smlal2 v12.4s, v22.8h, v6.h[0]
    smlal2 v13.4s, v22.8h, v6.h[1]
    smlal2 v14.4s, v22.8h, v6.h[2]
    smlal2 v15.4s, v22.8h, v6.h[3]
    smlal2 v12.4s, v23.8h, v7.h[0]
    smlal2 v13.4s, v23.8h, v7.h[1]
    smlal2 v14.4s, v23.8h, v7.h[2]
    smlal2 v15.4s, v23.8h, v7.h[3]

    ld1 {v16.8b}, [x8], x10         // load it bottom 8 lines
    ld1 {v17.8b}, [x8], x10
    ld1 {v18.8b}, [x8], x10
    ld1 {v19.8b}, [x8], x10
    ld1 {v20.8b}, [x8], x10
    ld1 {v21.8b}, [x8], x10
    ld1 {v22.8b}, [x8], x10
    ld1 {v23.8b}, [x8], x10

    sxtl v16.8h, v16.8b
    sxtl v17.8h, v17.8b
    sxtl v18.8h, v18.8b
    sxtl v19.8h, v19.8b
    sxtl v20.8h, v20.8b
    sxtl v21.8h, v21.8b
    sxtl v22.8h, v22.8b
    sxtl v23.8h, v23.8b

    ld1 {v0.4h}, [x9], x3
    ld1 {v1.4h}, [x9], x3
    ld1 {v2.4h}, [x9], x3
    ld1 {v3.4h}, [x9], x3
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9], x3

    smlal v24.4s, v16.4h, v0.h[0]
    smlal v25.4s, v16.4h, v0.h[1]
    smlal v26.4s, v16.4h, v0.h[2]
    smlal v27.4s, v16.4h, v0.h[3]
    smlal v24.4s, v17.4h, v1.h[0]
    smlal v25.4s, v17.4h, v1.h[1]
    smlal v26.4s, v17.4h, v1.h[2]
    smlal v27.4s, v17.4h, v1.h[3]
    smlal v24.4s, v18.4h, v2.h[0]
    smlal v25.4s, v18.4h, v2.h[1]
    smlal v26.4s, v18.4h, v2.h[2]
    smlal v27.4s, v18.4h, v2.h[3]
    smlal v24.4s, v19.4h, v3.h[0]
    smlal v25.4s, v19.4h, v3.h[1]
    smlal v26.4s, v19.4h, v3.h[2]
    smlal v27.4s, v19.4h, v3.h[3]
    smlal v24.4s, v20.4h, v4.h[0]
    smlal v25.4s, v20.4h, v4.h[1]
    smlal v26.4s, v20.4h, v4.h[2]
    smlal v27.4s, v20.4h, v4.h[3]
    smlal v24.4s, v21.4h, v5.h[0]
    smlal v25.4s, v21.4h, v5.h[1]
    smlal v26.4s, v21.4h, v5.h[2]
    smlal v27.4s, v21.4h, v5.h[3]
    smlal v24.4s, v22.4h, v6.h[0]
    smlal v25.4s, v22.4h, v6.h[1]
    smlal v26.4s, v22.4h, v6.h[2]
    smlal v27.4s, v22.4h, v6.h[3]
    smlal v24.4s, v23.4h, v7.h[0]
    smlal v25.4s, v23.4h, v7.h[1]
    smlal v26.4s, v23.4h, v7.h[2]
    smlal v27.4s, v23.4h, v7.h[3]

    smlal2 v12.4s, v16.8h, v0.h[0]
    smlal2 v13.4s, v16.8h, v0.h[1]
    smlal2 v14.4s, v16.8h, v0.h[2]
    smlal2 v15.4s, v16.8h, v0.h[3]
    smlal2 v12.4s, v17.8h, v1.h[0]
    smlal2 v13.4s, v17.8h, v1.h[1]
    smlal2 v14.4s, v17.8h, v1.h[2]
    smlal2 v15.4s, v17.8h, v1.h[3]
    smlal2 v12.4s, v18.8h, v2.h[0]
    smlal2 v13.4s, v18.8h, v2.h[1]
    smlal2 v14.4s, v18.8h, v2.h[2]
    smlal2 v15.4s, v18.8h, v2.h[3]
    smlal2 v12.4s, v19.8h, v3.h[0]
    smlal2 v13.4s, v19.8h, v3.h[1]
    smlal2 v14.4s, v19.8h, v3.h[2]
    smlal2 v15.4s, v19.8h, v3.h[3]
    smlal2 v12.4s, v20.8h, v4.h[0]
    smlal2 v13.4s, v20.8h, v4.h[1]
    smlal2 v14.4s, v20.8h, v4.h[2]
    smlal2 v15.4s, v20.8h, v4.h[3]
    smlal2 v12.4s, v21.8h, v5.h[0]
    smlal2 v13.4s, v21.8h, v5.h[1]
    smlal2 v14.4s, v21.8h, v5.h[2]
    smlal2 v15.4s, v21.8h, v5.h[3]
    smlal2 v12.4s, v22.8h, v6.h[0]
    smlal2 v13.4s, v22.8h, v6.h[1]
    smlal2 v14.4s, v22.8h, v6.h[2]
    smlal2 v15.4s, v22.8h, v6.h[3]
    smlal2 v12.4s, v23.8h, v7.h[0]
    smlal2 v13.4s, v23.8h, v7.h[1]
    smlal2 v14.4s, v23.8h, v7.h[2]
    smlal2 v15.4s, v23.8h, v7.h[3]

    cmp x2, #5
    bne dct8_pb16_shift12

    sqrshrn  v24.4h, v24.4s, #5
    sqrshrn  v25.4h, v25.4s, #5
    sqrshrn  v26.4h, v26.4s, #5
    sqrshrn  v27.4h, v27.4s, #5
    sqrshrn2 v24.8h, v12.4s, #5
    sqrshrn2 v25.8h, v13.4s, #5
    sqrshrn2 v26.8h, v14.4s, #5
    sqrshrn2 v27.8h, v15.4s, #5

    b dct8_pb16_clip_store

dct8_pb16_shift12:
    srshl v24.4s, v24.4s, v31.4s
    srshl v25.4s, v25.4s, v31.4s
    srshl v26.4s, v26.4s, v31.4s
    srshl v27.4s, v27.4s, v31.4s
    srshl v12.4s, v12.4s, v31.4s
    srshl v13.4s, v13.4s, v31.4s
    srshl v14.4s, v14.4s, v31.4s
    srshl v15.4s, v15.4s, v31.4s

    sqxtn  v24.4h, v24.4s
    sqxtn  v25.4h, v25.4s
    sqxtn  v26.4h, v26.4s
    sqxtn  v27.4h, v27.4s
    sqxtn2 v24.8h, v12.4s
    sqxtn2 v25.8h, v13.4s
    sqxtn2 v26.8h, v14.4s
    sqxtn2 v27.8h, v15.4s

dct8_pb16_clip_store:
    add x11, x1, x12, lsl #1            // resi + i*16 + j
    add x12, x12, #8                    // j += 8

    smin v24.8h, v24.8h, v28.8h
    smin v25.8h, v25.8h, v28.8h
    smin v26.8h, v26.8h, v28.8h
    smin v27.8h, v27.8h, v28.8h
    smax v24.8h, v24.8h, v29.8h
    smax v25.8h, v25.8h, v29.8h
    smax v26.8h, v26.8h, v29.8h
    smax v27.8h, v27.8h, v29.8h

    cmp x12, #16
    st1 {v24.8h}, [x11], x14
    st1 {v25.8h}, [x11], x14
    st1 {v26.8h}, [x11], x14
    st1 {v27.8h}, [x11], x14

    blt dct8_pb16_loopj

    add  x0, x0, #8                     // coeff += 4
    add  x1, x1, #128                   // resi += resi_stride * 4
    subs x13, x13, #4                   // i += 4
    bgt dct8_pb16_loopi

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
    ret

itrans_dst7_pb4_coef:
.byte 15, 27, 37, 42, \
      37, 37, 0, -37, \
      42, -15, -37, 27, \
      27, -42, 37, -15
//*************************************************************************************************
//void uavs3e_itrans_dst7_pb4_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
//x0->coeff blk, 16 bit
//x1->resi blk, 16 bit
//*************************************************************************************************
function uavs3e_itrans_dst7_pb4_arm64
    adr x6, itrans_dst7_pb4_coef

    neg w15, w2
    dup v28.8h, w4
    dup v29.8h, w5
    dup v30.4s, w15

    //load 变换系数(4x4)
    ld1 {v0.16b}, [x6]

    //扩展到16bit
    sxtl2 v2.8h, v0.16b
    sxtl  v0.8h, v0.8b
    ext   v1.16b, v0.16b, v0.16b, #8
    ext   v3.16b, v2.16b, v2.16b, #8

    lsl x3, x3, #1

    cmp x2, #5
    bne dst7_pb4_2nd

    mov x8, #0
dst7_pb4_loopi_1st:
    add x9, x8, x0
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9]

    smull v16.4s, v0.4h, v4.h[0]
    smull v17.4s, v0.4h, v4.h[1]
    smull v18.4s, v0.4h, v4.h[2]
    smull v19.4s, v0.4h, v4.h[3]
    smlal v16.4s, v1.4h, v5.h[0]
    smlal v17.4s, v1.4h, v5.h[1]
    smlal v18.4s, v1.4h, v5.h[2]
    smlal v19.4s, v1.4h, v5.h[3]
    smlal v16.4s, v2.4h, v6.h[0]
    smlal v17.4s, v2.4h, v6.h[1]
    smlal v18.4s, v2.4h, v6.h[2]
    smlal v19.4s, v2.4h, v6.h[3]
    smlal v16.4s, v3.4h, v7.h[0]
    smlal v17.4s, v3.4h, v7.h[1]
    smlal v18.4s, v3.4h, v7.h[2]
    smlal v19.4s, v3.4h, v7.h[3]

    sqrshrn  v16.4h, v16.4s, #5
    sqrshrn2 v16.8h, v17.4s, #5
    sqrshrn  v17.4h, v18.4s, #5
    sqrshrn2 v17.8h, v19.4s, #5

    add x8, x8, #8
    smin v16.8h, v16.8h, v28.8h
    smin v17.8h, v17.8h, v28.8h
    smax v16.8h, v16.8h, v29.8h
    smax v17.8h, v17.8h, v29.8h

    cmp x8, x3
    st1 {v16.8h, v17.8h}, [x1], #32
    blt dst7_pb4_loopi_1st
    b   dst7_pb4_end

dst7_pb4_2nd:
    mov x8, #0                  // i = 0
dst7_pb4_loopi_2nd:
    add x9, x8, x0
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9]

    smull v16.4s, v0.4h, v4.h[0]
    smull v17.4s, v0.4h, v4.h[1]
    smull v18.4s, v0.4h, v4.h[2]
    smull v19.4s, v0.4h, v4.h[3]
    smlal v16.4s, v1.4h, v5.h[0]
    smlal v17.4s, v1.4h, v5.h[1]
    smlal v18.4s, v1.4h, v5.h[2]
    smlal v19.4s, v1.4h, v5.h[3]
    smlal v16.4s, v2.4h, v6.h[0]
    smlal v17.4s, v2.4h, v6.h[1]
    smlal v18.4s, v2.4h, v6.h[2]
    smlal v19.4s, v2.4h, v6.h[3]
    smlal v16.4s, v3.4h, v7.h[0]
    smlal v17.4s, v3.4h, v7.h[1]
    smlal v18.4s, v3.4h, v7.h[2]
    smlal v19.4s, v3.4h, v7.h[3]

    srshl v16.4s, v16.4s, v30.4s
    srshl v17.4s, v17.4s, v30.4s
    srshl v18.4s, v18.4s, v30.4s
    srshl v19.4s, v19.4s, v30.4s

    sqxtn  v16.4h, v16.4s
    sqxtn2 v16.8h, v17.4s
    sqxtn  v17.4h, v18.4s
    sqxtn2 v17.8h, v19.4s

    add x8, x8, #8
    smin v16.8h, v16.8h, v28.8h
    smin v17.8h, v17.8h, v28.8h
    smax v16.8h, v16.8h, v29.8h
    smax v17.8h, v17.8h, v29.8h

    cmp x8, x3
    st1 {v16.8h, v17.8h}, [x1], #32

    blt dst7_pb4_loopi_2nd

dst7_pb4_end:
    ret

itrans_dst7_pb8_coef:
.byte 8, 16, 23, 30, 35, 39, 42, 44, \
      23, 39, 44, 35, 16, -8, -30, -42, \
      35, 42, 16, -23, -44, -30, 8, 39, \
      42, 23, -30, -39, 8, 44, 16, -35, \
      44, -8, -42, 16, 39, -23, -35, 30, \
      39, -35, -8, 42, -30, -16, 44, -23, \
      30, -44, 35, -8, -23, 42, -39, 16, \
      16, -30, 39, -44, 42, -35, 23, -8
//*************************************************************************************************
//void uavs3e_itrans_dst7_pb8_arm64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val);
//x0->coeff blk, 16 bit
//x1->resi blk, 16 bit
//*************************************************************************************************
function uavs3e_itrans_dst7_pb8_arm64
    sub sp, sp, #64
    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]
    adr x6, itrans_dst7_pb8_coef

    neg w15, w2
    dup v28.8h, w4                  // max_tr_val
    dup v29.8h, w5                  // min_tr_val
    dup v31.4s, w15                 // shift

    ld1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x6], #32
    ld1 {v4.8b, v5.8b, v6.8b, v7.8b}, [x6]
    sxtl v16.8h, v0.8b
    sxtl v17.8h, v1.8b
    sxtl v18.8h, v2.8b
    sxtl v19.8h, v3.8b
    sxtl v20.8h, v4.8b
    sxtl v21.8h, v5.8b
    sxtl v22.8h, v6.8b
    sxtl v23.8h, v7.8b

    lsl x3, x3, #1
    mov x8, #0                      // i = 0
dst7_pb8_loopi:
    add x9, x0, x8
    ld1 {v0.4h}, [x9], x3
    ld1 {v1.4h}, [x9], x3
    ld1 {v2.4h}, [x9], x3
    ld1 {v3.4h}, [x9], x3
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9]

    smull v24.4s, v16.4h, v0.h[0]
    smull v25.4s, v16.4h, v0.h[1]
    smull v26.4s, v16.4h, v0.h[2]
    smull v27.4s, v16.4h, v0.h[3]
    smlal v24.4s, v17.4h, v1.h[0]
    smlal v25.4s, v17.4h, v1.h[1]
    smlal v26.4s, v17.4h, v1.h[2]
    smlal v27.4s, v17.4h, v1.h[3]
    smlal v24.4s, v18.4h, v2.h[0]
    smlal v25.4s, v18.4h, v2.h[1]
    smlal v26.4s, v18.4h, v2.h[2]
    smlal v27.4s, v18.4h, v2.h[3]
    smlal v24.4s, v19.4h, v3.h[0]
    smlal v25.4s, v19.4h, v3.h[1]
    smlal v26.4s, v19.4h, v3.h[2]
    smlal v27.4s, v19.4h, v3.h[3]
    smlal v24.4s, v20.4h, v4.h[0]
    smlal v25.4s, v20.4h, v4.h[1]
    smlal v26.4s, v20.4h, v4.h[2]
    smlal v27.4s, v20.4h, v4.h[3]
    smlal v24.4s, v21.4h, v5.h[0]
    smlal v25.4s, v21.4h, v5.h[1]
    smlal v26.4s, v21.4h, v5.h[2]
    smlal v27.4s, v21.4h, v5.h[3]
    smlal v24.4s, v22.4h, v6.h[0]
    smlal v25.4s, v22.4h, v6.h[1]
    smlal v26.4s, v22.4h, v6.h[2]
    smlal v27.4s, v22.4h, v6.h[3]
    smlal v24.4s, v23.4h, v7.h[0]
    smlal v25.4s, v23.4h, v7.h[1]
    smlal v26.4s, v23.4h, v7.h[2]
    smlal v27.4s, v23.4h, v7.h[3]

    smull2 v12.4s, v16.8h, v0.h[0]
    smull2 v13.4s, v16.8h, v0.h[1]
    smull2 v14.4s, v16.8h, v0.h[2]
    smull2 v15.4s, v16.8h, v0.h[3]
    smlal2 v12.4s, v17.8h, v1.h[0]
    smlal2 v13.4s, v17.8h, v1.h[1]
    smlal2 v14.4s, v17.8h, v1.h[2]
    smlal2 v15.4s, v17.8h, v1.h[3]
    smlal2 v12.4s, v18.8h, v2.h[0]
    smlal2 v13.4s, v18.8h, v2.h[1]
    smlal2 v14.4s, v18.8h, v2.h[2]
    smlal2 v15.4s, v18.8h, v2.h[3]
    smlal2 v12.4s, v19.8h, v3.h[0]
    smlal2 v13.4s, v19.8h, v3.h[1]
    smlal2 v14.4s, v19.8h, v3.h[2]
    smlal2 v15.4s, v19.8h, v3.h[3]
    smlal2 v12.4s, v20.8h, v4.h[0]
    smlal2 v13.4s, v20.8h, v4.h[1]
    smlal2 v14.4s, v20.8h, v4.h[2]
    smlal2 v15.4s, v20.8h, v4.h[3]
    smlal2 v12.4s, v21.8h, v5.h[0]
    smlal2 v13.4s, v21.8h, v5.h[1]
    smlal2 v14.4s, v21.8h, v5.h[2]
    smlal2 v15.4s, v21.8h, v5.h[3]
    smlal2 v12.4s, v22.8h, v6.h[0]
    smlal2 v13.4s, v22.8h, v6.h[1]
    smlal2 v14.4s, v22.8h, v6.h[2]
    smlal2 v15.4s, v22.8h, v6.h[3]
    smlal2 v12.4s, v23.8h, v7.h[0]
    smlal2 v13.4s, v23.8h, v7.h[1]
    smlal2 v14.4s, v23.8h, v7.h[2]
    smlal2 v15.4s, v23.8h, v7.h[3]

    cmp x2, #5
    bne dst7_pb8_shift12

    sqrshrn  v24.4h, v24.4s, #5
    sqrshrn  v25.4h, v25.4s, #5
    sqrshrn  v26.4h, v26.4s, #5
    sqrshrn  v27.4h, v27.4s, #5
    sqrshrn2 v24.8h, v12.4s, #5
    sqrshrn2 v25.8h, v13.4s, #5
    sqrshrn2 v26.8h, v14.4s, #5
    sqrshrn2 v27.8h, v15.4s, #5

    b dst7_pb8_clip_store

dst7_pb8_shift12:
	srshl v24.4s, v24.4s, v31.4s
    srshl v25.4s, v25.4s, v31.4s
    srshl v26.4s, v26.4s, v31.4s
    srshl v27.4s, v27.4s, v31.4s
    srshl v12.4s, v12.4s, v31.4s
    srshl v13.4s, v13.4s, v31.4s
    srshl v14.4s, v14.4s, v31.4s
    srshl v15.4s, v15.4s, v31.4s

    sqxtn  v24.4h, v24.4s
    sqxtn  v25.4h, v25.4s
    sqxtn  v26.4h, v26.4s
    sqxtn  v27.4h, v27.4s
    sqxtn2 v24.8h, v12.4s
    sqxtn2 v25.8h, v13.4s
    sqxtn2 v26.8h, v14.4s
    sqxtn2 v27.8h, v15.4s

dst7_pb8_clip_store:
    add x8, x8, #8
    smin v24.8h, v24.8h, v28.8h
    smin v25.8h, v25.8h, v28.8h
    smin v26.8h, v26.8h, v28.8h
    smin v27.8h, v27.8h, v28.8h
    smax v24.8h, v24.8h, v29.8h
    smax v25.8h, v25.8h, v29.8h
    smax v26.8h, v26.8h, v29.8h
    smax v27.8h, v27.8h, v29.8h

    cmp x8, x3
    st1 {v24.8h, v25.8h, v26.8h, v27.8h}, [x1], #64
    blt dst7_pb8_loopi

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
    ret

itrans_dst7_pb16_coef:
.byte 4, 8, 13, 17, 20, 24, 28, 31, 34, 36, 39, 41, 42, 43, 44, 45, \
      13, 24, 34, 41, 44, 44, 41, 34, 24, 13, 0, -13, -24, -34, -41, -44, \
      20, 36, 44, 42, 31, 13, -8, -28, -41, -45, -39, -24, -4, 17, 34, 43, \
      28, 43, 41, 20, -8, -34, -45, -36, -13, 17, 39, 44, 31, 4, -24, -42, \
      34, 44, 24, -13, -41, -41, -13, 24, 44, 34, 0, -34, -44, -24, 13, 41, \
      39, 39, 0, -39, -39, 0, 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, \
      42, 28, -24, -43, -4, 41, 31, -20, -44, -8, 39, 34, -17, -45, -13, 36, \
      44, 13, -41, -24, 34, 34, -24, -41, 13, 44, 0, -44, -13, 41, 24, -34, \
      45, -4, -44, 8, 43, -13, -42, 17, 41, -20, -39, 24, 36, -28, -34, 31, \
      43, -20, -34, 36, 17, -44, 4, 42, -24, -31, 39, 13, -45, 8, 41, -28, \
      41, -34, -13, 44, -24, -24, 44, -13, -34, 41, 0, -41, 34, 13, -44, 24, \
      36, -42, 13, 28, -45, 24, 17, -43, 34, 4, -39, 41, -8, -31, 44, -20, \
      31, -45, 34, -4, -28, 44, -36, 8, 24, -43, 39, -13, -20, 42, -41, 17, \
      24, -41, 44, -34, 13, 13, -34, 44, -41, 24, 0, -24, 41, -44, 34, -13, \
      17, -31, 41, -45, 42, -34, 20, -4, -13, 28, -39, 44, -43, 36, -24, 8, \
      8, -17, 24, -31, 36, -41, 43, -45, 44, -42, 39, -34, 28, -20, 13, -4
//*************************************************************************************************
//void uavs3e_itrans_dst7_pb16_arm64(s16 *coeff, s16 *resi, int shift, int line, int max_tr_val, int min_tr_val)
//x0->coeff blk, 16 bit
//x1->resi blk, 16 bit
//*************************************************************************************************
function uavs3e_itrans_dst7_pb16_arm64
    sub sp, sp, #64
    st1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp]

    adr x6, itrans_dst7_pb16_coef

    neg w15, w2
    dup v28.8h, w4                  // max_tr_val
    dup v29.8h, w5                  // min_tr_val
    dup v31.4s, w15                 // shift

    mov x10, #16                    // it_stride
    mov x14, #32                    // resi_stride
    mov x13, x3                     // line - i
    lsl x3, x3, #1                  // coeff_stride = line*sizeof(s16)
dst7_pb16_loopi:
    mov x12, #0                     // j
    mov x11, x1                     // resi + i*16
dst7_pb16_loopj:
    add x8, x6, x12                 // it + j
    mov x9, x0                      // coeff + i

    ld1 {v16.8b}, [x8], x10         // load it first 8 lines
    ld1 {v17.8b}, [x8], x10
    ld1 {v18.8b}, [x8], x10
    ld1 {v19.8b}, [x8], x10
    ld1 {v20.8b}, [x8], x10
    ld1 {v21.8b}, [x8], x10
    ld1 {v22.8b}, [x8], x10
    ld1 {v23.8b}, [x8], x10

    sxtl v16.8h, v16.8b
    sxtl v17.8h, v17.8b
    sxtl v18.8h, v18.8b
    sxtl v19.8h, v19.8b
    sxtl v20.8h, v20.8b
    sxtl v21.8h, v21.8b
    sxtl v22.8h, v22.8b
    sxtl v23.8h, v23.8b

    ld1 {v0.4h}, [x9], x3
    ld1 {v1.4h}, [x9], x3
    ld1 {v2.4h}, [x9], x3
    ld1 {v3.4h}, [x9], x3
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9], x3

    smull v24.4s, v16.4h, v0.h[0]
    smull v25.4s, v16.4h, v0.h[1]
    smull v26.4s, v16.4h, v0.h[2]
    smull v27.4s, v16.4h, v0.h[3]
    smlal v24.4s, v17.4h, v1.h[0]
    smlal v25.4s, v17.4h, v1.h[1]
    smlal v26.4s, v17.4h, v1.h[2]
    smlal v27.4s, v17.4h, v1.h[3]
    smlal v24.4s, v18.4h, v2.h[0]
    smlal v25.4s, v18.4h, v2.h[1]
    smlal v26.4s, v18.4h, v2.h[2]
    smlal v27.4s, v18.4h, v2.h[3]
    smlal v24.4s, v19.4h, v3.h[0]
    smlal v25.4s, v19.4h, v3.h[1]
    smlal v26.4s, v19.4h, v3.h[2]
    smlal v27.4s, v19.4h, v3.h[3]
    smlal v24.4s, v20.4h, v4.h[0]
    smlal v25.4s, v20.4h, v4.h[1]
    smlal v26.4s, v20.4h, v4.h[2]
    smlal v27.4s, v20.4h, v4.h[3]
    smlal v24.4s, v21.4h, v5.h[0]
    smlal v25.4s, v21.4h, v5.h[1]
    smlal v26.4s, v21.4h, v5.h[2]
    smlal v27.4s, v21.4h, v5.h[3]
    smlal v24.4s, v22.4h, v6.h[0]
    smlal v25.4s, v22.4h, v6.h[1]
    smlal v26.4s, v22.4h, v6.h[2]
    smlal v27.4s, v22.4h, v6.h[3]
    smlal v24.4s, v23.4h, v7.h[0]
    smlal v25.4s, v23.4h, v7.h[1]
    smlal v26.4s, v23.4h, v7.h[2]
    smlal v27.4s, v23.4h, v7.h[3]

    smull2 v12.4s, v16.8h, v0.h[0]
    smull2 v13.4s, v16.8h, v0.h[1]
    smull2 v14.4s, v16.8h, v0.h[2]
    smull2 v15.4s, v16.8h, v0.h[3]
    smlal2 v12.4s, v17.8h, v1.h[0]
    smlal2 v13.4s, v17.8h, v1.h[1]
    smlal2 v14.4s, v17.8h, v1.h[2]
    smlal2 v15.4s, v17.8h, v1.h[3]
    smlal2 v12.4s, v18.8h, v2.h[0]
    smlal2 v13.4s, v18.8h, v2.h[1]
    smlal2 v14.4s, v18.8h, v2.h[2]
    smlal2 v15.4s, v18.8h, v2.h[3]
    smlal2 v12.4s, v19.8h, v3.h[0]
    smlal2 v13.4s, v19.8h, v3.h[1]
    smlal2 v14.4s, v19.8h, v3.h[2]
    smlal2 v15.4s, v19.8h, v3.h[3]
    smlal2 v12.4s, v20.8h, v4.h[0]
    smlal2 v13.4s, v20.8h, v4.h[1]
    smlal2 v14.4s, v20.8h, v4.h[2]
    smlal2 v15.4s, v20.8h, v4.h[3]
    smlal2 v12.4s, v21.8h, v5.h[0]
    smlal2 v13.4s, v21.8h, v5.h[1]
    smlal2 v14.4s, v21.8h, v5.h[2]
    smlal2 v15.4s, v21.8h, v5.h[3]
    smlal2 v12.4s, v22.8h, v6.h[0]
    smlal2 v13.4s, v22.8h, v6.h[1]
    smlal2 v14.4s, v22.8h, v6.h[2]
    smlal2 v15.4s, v22.8h, v6.h[3]
    smlal2 v12.4s, v23.8h, v7.h[0]
    smlal2 v13.4s, v23.8h, v7.h[1]
    smlal2 v14.4s, v23.8h, v7.h[2]
    smlal2 v15.4s, v23.8h, v7.h[3]

    ld1 {v16.8b}, [x8], x10         // load it bottom 8 lines
    ld1 {v17.8b}, [x8], x10
    ld1 {v18.8b}, [x8], x10
    ld1 {v19.8b}, [x8], x10
    ld1 {v20.8b}, [x8], x10
    ld1 {v21.8b}, [x8], x10
    ld1 {v22.8b}, [x8], x10
    ld1 {v23.8b}, [x8], x10

    sxtl v16.8h, v16.8b
    sxtl v17.8h, v17.8b
    sxtl v18.8h, v18.8b
    sxtl v19.8h, v19.8b
    sxtl v20.8h, v20.8b
    sxtl v21.8h, v21.8b
    sxtl v22.8h, v22.8b
    sxtl v23.8h, v23.8b

    ld1 {v0.4h}, [x9], x3
    ld1 {v1.4h}, [x9], x3
    ld1 {v2.4h}, [x9], x3
    ld1 {v3.4h}, [x9], x3
    ld1 {v4.4h}, [x9], x3
    ld1 {v5.4h}, [x9], x3
    ld1 {v6.4h}, [x9], x3
    ld1 {v7.4h}, [x9], x3

    smlal v24.4s, v16.4h, v0.h[0]
    smlal v25.4s, v16.4h, v0.h[1]
    smlal v26.4s, v16.4h, v0.h[2]
    smlal v27.4s, v16.4h, v0.h[3]
    smlal v24.4s, v17.4h, v1.h[0]
    smlal v25.4s, v17.4h, v1.h[1]
    smlal v26.4s, v17.4h, v1.h[2]
    smlal v27.4s, v17.4h, v1.h[3]
    smlal v24.4s, v18.4h, v2.h[0]
    smlal v25.4s, v18.4h, v2.h[1]
    smlal v26.4s, v18.4h, v2.h[2]
    smlal v27.4s, v18.4h, v2.h[3]
    smlal v24.4s, v19.4h, v3.h[0]
    smlal v25.4s, v19.4h, v3.h[1]
    smlal v26.4s, v19.4h, v3.h[2]
    smlal v27.4s, v19.4h, v3.h[3]
    smlal v24.4s, v20.4h, v4.h[0]
    smlal v25.4s, v20.4h, v4.h[1]
    smlal v26.4s, v20.4h, v4.h[2]
    smlal v27.4s, v20.4h, v4.h[3]
    smlal v24.4s, v21.4h, v5.h[0]
    smlal v25.4s, v21.4h, v5.h[1]
    smlal v26.4s, v21.4h, v5.h[2]
    smlal v27.4s, v21.4h, v5.h[3]
    smlal v24.4s, v22.4h, v6.h[0]
    smlal v25.4s, v22.4h, v6.h[1]
    smlal v26.4s, v22.4h, v6.h[2]
    smlal v27.4s, v22.4h, v6.h[3]
    smlal v24.4s, v23.4h, v7.h[0]
    smlal v25.4s, v23.4h, v7.h[1]
    smlal v26.4s, v23.4h, v7.h[2]
    smlal v27.4s, v23.4h, v7.h[3]

    smlal2 v12.4s, v16.8h, v0.h[0]
    smlal2 v13.4s, v16.8h, v0.h[1]
    smlal2 v14.4s, v16.8h, v0.h[2]
    smlal2 v15.4s, v16.8h, v0.h[3]
    smlal2 v12.4s, v17.8h, v1.h[0]
    smlal2 v13.4s, v17.8h, v1.h[1]
    smlal2 v14.4s, v17.8h, v1.h[2]
    smlal2 v15.4s, v17.8h, v1.h[3]
    smlal2 v12.4s, v18.8h, v2.h[0]
    smlal2 v13.4s, v18.8h, v2.h[1]
    smlal2 v14.4s, v18.8h, v2.h[2]
    smlal2 v15.4s, v18.8h, v2.h[3]
    smlal2 v12.4s, v19.8h, v3.h[0]
    smlal2 v13.4s, v19.8h, v3.h[1]
    smlal2 v14.4s, v19.8h, v3.h[2]
    smlal2 v15.4s, v19.8h, v3.h[3]
    smlal2 v12.4s, v20.8h, v4.h[0]
    smlal2 v13.4s, v20.8h, v4.h[1]
    smlal2 v14.4s, v20.8h, v4.h[2]
    smlal2 v15.4s, v20.8h, v4.h[3]
    smlal2 v12.4s, v21.8h, v5.h[0]
    smlal2 v13.4s, v21.8h, v5.h[1]
    smlal2 v14.4s, v21.8h, v5.h[2]
    smlal2 v15.4s, v21.8h, v5.h[3]
    smlal2 v12.4s, v22.8h, v6.h[0]
    smlal2 v13.4s, v22.8h, v6.h[1]
    smlal2 v14.4s, v22.8h, v6.h[2]
    smlal2 v15.4s, v22.8h, v6.h[3]
    smlal2 v12.4s, v23.8h, v7.h[0]
    smlal2 v13.4s, v23.8h, v7.h[1]
    smlal2 v14.4s, v23.8h, v7.h[2]
    smlal2 v15.4s, v23.8h, v7.h[3]

    cmp x2, #5
    bne dst7_pb16_shift12

    sqrshrn  v24.4h, v24.4s, #5
    sqrshrn  v25.4h, v25.4s, #5
    sqrshrn  v26.4h, v26.4s, #5
    sqrshrn  v27.4h, v27.4s, #5
    sqrshrn2 v24.8h, v12.4s, #5
    sqrshrn2 v25.8h, v13.4s, #5
    sqrshrn2 v26.8h, v14.4s, #5
    sqrshrn2 v27.8h, v15.4s, #5

    b dst7_pb16_clip_store

dst7_pb16_shift12:
    srshl v24.4s, v24.4s, v31.4s
    srshl v25.4s, v25.4s, v31.4s
    srshl v26.4s, v26.4s, v31.4s
    srshl v27.4s, v27.4s, v31.4s
    srshl v12.4s, v12.4s, v31.4s
    srshl v13.4s, v13.4s, v31.4s
    srshl v14.4s, v14.4s, v31.4s
    srshl v15.4s, v15.4s, v31.4s

    sqxtn  v24.4h, v24.4s
    sqxtn  v25.4h, v25.4s
    sqxtn  v26.4h, v26.4s
    sqxtn  v27.4h, v27.4s
    sqxtn2 v24.8h, v12.4s
    sqxtn2 v25.8h, v13.4s
    sqxtn2 v26.8h, v14.4s
    sqxtn2 v27.8h, v15.4s

dst7_pb16_clip_store:
    add x11, x1, x12, lsl #1            // resi + i*16 + j
    add x12, x12, #8                    // j += 8

    smin v24.8h, v24.8h, v28.8h
    smin v25.8h, v25.8h, v28.8h
    smin v26.8h, v26.8h, v28.8h
    smin v27.8h, v27.8h, v28.8h
    smax v24.8h, v24.8h, v29.8h
    smax v25.8h, v25.8h, v29.8h
    smax v26.8h, v26.8h, v29.8h
    smax v27.8h, v27.8h, v29.8h

    cmp x12, #16
    st1 {v24.8h}, [x11], x14
    st1 {v25.8h}, [x11], x14
    st1 {v26.8h}, [x11], x14
    st1 {v27.8h}, [x11], x14

    blt dst7_pb16_loopj

    add  x0, x0, #8                     // coeff += 4
    add  x1, x1, #128                   // resi += resi_stride * 4
    subs x13, x13, #4                   // i += 4
    bgt dst7_pb16_loopi

    ld1 {v12.2d, v13.2d, v14.2d, v15.2d}, [sp], #64
    ret

#endif