#include "commonVariables.h"
#include "defines.h"

void intra_pred_pu_luma(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int bAbove, int bLeft, int bs_y, int bs_x, int bit_depth);
void core_intra_get_chroma(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int uiBitSize, int bAbove, int bLeft, int LumaMode, int bit_depth);