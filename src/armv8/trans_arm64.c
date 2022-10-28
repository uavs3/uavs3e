#include "arm64.h"
#if defined(__arm64__)
void uavs3e_trans_dct2_w4_h4_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*4]);
    tx_dct2_pb4_arm64(src, tmp, 4, 4, 0 + bit_depth - 8);
    tx_dct2_pb4_arm64(tmp, dst, 4, 4, 7);
}

void uavs3e_trans_dct2_w4_h8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*8]);
    tx_dct2_pb4_arm64(src, tmp, 8, 8, 0 + bit_depth - 8);
    tx_dct2_pb8_arm64(tmp, dst, 4, 4, 8);
}

void uavs3e_trans_dct2_w4_h16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*16]);
    tx_dct2_pb4_arm64(src, tmp, 16, 16, 0 + bit_depth - 8);
    tx_dct2_pb16_arm64(tmp, dst, 4, 4, 9);
}

void uavs3e_trans_dct2_w4_h32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*32]);
    tx_dct2_pb4_arm64(src, tmp, 32, 32, 0 + bit_depth - 8);
    tx_dct2_pb32_arm64(tmp, dst, 4, 4, 10);
}

void uavs3e_trans_dct2_w8_h4_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*4]);
    tx_dct2_pb8_arm64(src, tmp, 4, 4, 1 + bit_depth - 8);
    tx_dct2_pb4_arm64(tmp, dst, 8, 8, 7);
}

void uavs3e_trans_dct2_w8_h8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8 * 8]);
    tx_dct2_pb8_arm64(src, tmp, 8, 8, 1 + bit_depth - 8);
    tx_dct2_pb8_arm64(tmp, dst, 8, 8, 8);
}

void uavs3e_trans_dct2_w8_h16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*16]);
    tx_dct2_pb8_arm64(src, tmp, 16, 16, 1 + bit_depth - 8);
    tx_dct2_pb16_arm64(tmp, dst, 8, 8, 9);
}

void uavs3e_trans_dct2_w8_h32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*32]);
    tx_dct2_pb8_arm64(src, tmp, 32, 32, 1 + bit_depth - 8);
    tx_dct2_pb32_arm64(tmp, dst, 8, 8, 10);
}

void uavs3e_trans_dct2_w8_h64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*64]);
    tx_dct2_pb8_arm64(src, tmp, 64, 64, 1 + bit_depth - 8);
    tx_dct2_pb64_arm64(tmp, dst, 8, 8, 11);
}

void uavs3e_trans_dct2_w16_h4_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*4]);
    tx_dct2_pb16_arm64(src, tmp, 4, 4, 2 + bit_depth - 8);
    tx_dct2_pb4_arm64(tmp, dst, 16, 16, 7);
}

void uavs3e_trans_dct2_w16_h8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*8]);
    tx_dct2_pb16_arm64(src, tmp, 8, 8, 2 + bit_depth - 8);
    tx_dct2_pb8_arm64(tmp, dst, 16, 16, 8);
}

void uavs3e_trans_dct2_w16_h16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16 * 16]);
    tx_dct2_pb16_arm64(src, tmp, 16, 16, 2 + bit_depth - 8);
    tx_dct2_pb16_arm64(tmp, dst, 16, 16, 9);
}

void uavs3e_trans_dct2_w16_h32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*32]);
    tx_dct2_pb16_arm64(src, tmp, 32, 32, 2 + bit_depth - 8);
    tx_dct2_pb32_arm64(tmp, dst, 16, 16, 10);
}

void uavs3e_trans_dct2_w16_h64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*64]);
    tx_dct2_pb16_arm64(src, tmp, 64, 64, 2 + bit_depth - 8);
    tx_dct2_pb64_arm64(tmp, dst, 16, 16, 11);
}

void uavs3e_trans_dct2_w32_h4_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*4]);
    tx_dct2_pb32_arm64(src, tmp, 4, 4, 3 + bit_depth - 8);
    tx_dct2_pb4_arm64(tmp, dst, 32, 32, 7);
}

void uavs3e_trans_dct2_w32_h8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*8]);
    tx_dct2_pb32_arm64(src, tmp, 8, 8, 3 + bit_depth - 8);
    tx_dct2_pb8_arm64(tmp, dst, 32, 32, 8);
}

void uavs3e_trans_dct2_w32_h16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*16]);
    tx_dct2_pb32_arm64(src, tmp, 16, 16, 3 + bit_depth - 8);
    tx_dct2_pb16_arm64(tmp, dst, 32, 32, 9);
}

void uavs3e_trans_dct2_w32_h32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32 * 32]);
    tx_dct2_pb32_arm64(src, tmp, 32, 32, 3 + bit_depth - 8);
    tx_dct2_pb32_arm64(tmp, dst, 32, 32, 10);
}

void uavs3e_trans_dct2_w32_h64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*64]);
    tx_dct2_pb32_arm64(src, tmp, 64, 64, 3 + bit_depth - 8);
    tx_dct2_pb64_arm64(tmp, dst, 32, 32, 11);
}

void uavs3e_trans_dct2_w64_h8_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*8]);
    tx_dct2_pb64_arm64(src, tmp, 8, 8, 4 + bit_depth - 8);
    tx_dct2_pb8_arm64(tmp, dst, 64, 32, 8);
}

void uavs3e_trans_dct2_w64_h16_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*16]);
    tx_dct2_pb64_arm64(src, tmp, 16, 16, 4 + bit_depth - 8);
    tx_dct2_pb16_arm64(tmp, dst, 64, 32, 9);
}

void uavs3e_trans_dct2_w64_h32_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*32]);
    tx_dct2_pb64_arm64(src, tmp, 32, 32, 4 + bit_depth - 8);
    tx_dct2_pb32_arm64(tmp, dst, 64, 32, 10);
}

void uavs3e_trans_dct2_w64_h64_arm64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64 * 64]);
    tx_dct2_pb64_arm64(src, tmp, 64, 64, 4 + bit_depth - 8);
    tx_dct2_pb64_arm64(tmp, dst, 64, 32, 11);
}

#endif
