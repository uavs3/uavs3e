#include "arm64.h"
#if defined(__arm64__)
#ifndef __ITRANS_ARM64_H__
#define __ITRANS_ARM64_H__

void dct2_butterfly_h4_arm64(s16* src, s16* dst, int width, int shift, int bit_depth);
void dct2_butterfly_h8_arm64(s16* src, int i_src, s16* dst, int width, int shift, int bit_depth);
void dct2_butterfly_h16_arm64(s16* src, int i_src, s16* dst, int width, int shift, int bit_depth);
void dct2_butterfly_h32_arm64(s16* src, int i_src, s16* dst, int width, int shift, int bit_depth);
void dct2_butterfly_h64_arm64(s16* src, int i_src, s16* dst, int width, int shift, int bit_depth);

#endif
#endif
