#include "commonVariables.h"
#include "defines.h"

void cpy_sao_param_blk(SAOBlkParam *saopara_dst, SAOBlkParam *saopara_src);

void getMergeNeighbor(avs3_enc_t *h, int lcu_x, int lcu_y, int input_MaxsizeInBit, SAOBlkParam(*rec_saoBlkParam)[NUM_SAO_COMPONENTS], int *MergeAvail, SAOBlkParam sao_merge_param[][NUM_SAO_COMPONENTS]);

void SAO_on_smb(avs3_enc_t *h, int lcu_x, int lcu_y, SAOBlkParam *saoBlkParam);

void SAO_on_block(void *handle, void *sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height, int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down);