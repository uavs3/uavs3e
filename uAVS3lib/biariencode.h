#ifndef _BIARIENCOD_H_
#define _BIARIENCOD_H_

// some definitions to increase the readability of the source code
#define B_BITS  10

#define QUARTER    (1 << (B_BITS-2))

#define LG_PMPS_SHIFTNO 2
#define LG_PMPS_SHIFTNO_PLUS1 3
#define MASK_LGPMPS_MPS 0x7ff

#define PROB_BITS                         11 // LPS_PROB(10-bit) + MPS(1-bit)
#define PROB_MASK                         ((1 << PROB_BITS) - 1) // mask for LPS_PROB + MPS
#define MAX_PROB                          ((1 << PROB_BITS) - 1) // equal to PROB_LPS + PROB_MPS, 0x7FF

extern i16u_t *com_tbl_scan[LCU_SIZE_IN_BITS];

static int uavs3e_get_log2(int v)
{
#ifdef _WIN32
    unsigned long index;
    _BitScanReverse(&index, v);
    return index;
#else
    return 31 - __builtin_clz(v);
#endif
}

static int aec_get_shift(int v)
{
#ifdef _WIN32
    unsigned long index;
    _BitScanReverse(&index, v);
    return 8 - index;
#else
    return __builtin_clz(v) - 23;
#endif
}


void lbac_encode_bin_for_rdoq(int bin, bin_ctx_t *bi_ct);

#endif  // BIARIENCOD_H

