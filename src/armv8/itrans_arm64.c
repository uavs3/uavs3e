#include "arm64.h"
#if defined(__arm64__)
#include "itrans_arm64.h"


void uavs3e_itrans_dct2_h4_w8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*8]);
    dct2_butterfly_h4_arm64(src, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h8_arm64(tmp, 4, dst, 4, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h4_w16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*16]);
    dct2_butterfly_h4_arm64(src, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h16_arm64(tmp, 4, dst, 4, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h4_w32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*32]);
    dct2_butterfly_h4_arm64(src, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h32_arm64(tmp, 4, dst, 4, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h8_w4_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*4]);
    dct2_butterfly_h8_arm64(src, 4, tmp, 4, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h4_arm64(tmp, dst, 8, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h8_w8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*8]);
    dct2_butterfly_h8_arm64(src, 8, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h8_arm64(tmp, 8, dst, 8, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h8_w16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*16]);
    dct2_butterfly_h8_arm64(src, 16, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h16_arm64(tmp, 8, dst, 8, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h8_w32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*32]);
    dct2_butterfly_h8_arm64(src, 32, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h32_arm64(tmp, 8, dst, 8, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h8_w64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*64]);
    dct2_butterfly_h8_arm64(src, 64, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h64_arm64(tmp, 8, dst, 8, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h16_w4_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*4]);
    dct2_butterfly_h16_arm64(src, 4, tmp, 4, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h4_arm64(tmp, dst, 16, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h16_w8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*8]);
    dct2_butterfly_h16_arm64(src, 8, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h8_arm64(tmp, 16, dst, 16, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h16_w16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*16]);
    dct2_butterfly_h16_arm64(src, 16, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h16_arm64(tmp, 16, dst, 16, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h16_w32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*32]);
    dct2_butterfly_h16_arm64(src, 32, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h32_arm64(tmp, 16, dst, 16, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h16_w64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*64]);
    dct2_butterfly_h16_arm64(src, 64, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h64_arm64(tmp, 16, dst, 16, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h32_w4_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*4]);
    dct2_butterfly_h32_arm64(src, 4, tmp, 4, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h4_arm64(tmp, dst, 32, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h32_w8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*8]);
    dct2_butterfly_h32_arm64(src, 8, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h8_arm64(tmp, 32, dst, 32, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h32_w16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*16]);
    dct2_butterfly_h32_arm64(src, 16, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h16_arm64(tmp, 32, dst, 32, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h32_w32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*32]);
    dct2_butterfly_h32_arm64(src, 32, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h32_arm64(tmp, 32, dst, 32, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h32_w64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*64]);
    dct2_butterfly_h32_arm64(src, 64, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h64_arm64(tmp, 32, dst, 32, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h64_w8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*8]);
    dct2_butterfly_h64_arm64(src, 8, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h8_arm64(tmp, 64, dst, 64, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h64_w16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*16]);
    dct2_butterfly_h64_arm64(src, 16, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h16_arm64(tmp, 64, dst, 64, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h64_w32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*32]);
    dct2_butterfly_h64_arm64(src, 32, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h32_arm64(tmp, 64, dst, 64, 20 - bit_depth, bit_depth);
}

void uavs3e_itrans_dct2_h64_w64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*64]);
    dct2_butterfly_h64_arm64(src, 64, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct2_butterfly_h64_arm64(tmp, 64, dst, 64, 20 - bit_depth, bit_depth);
}

#endif
