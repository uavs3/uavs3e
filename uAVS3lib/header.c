#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "header.h"
#include "defines.h"
#include "transform.h"
#include "vlc.h"

/* MD5 functions */
#define MD5FUNC(f, w, x, y, z, msg1, s,msg2 ) ( w += f(x, y, z) + msg1 + msg2,  w = w<<s | w>>(32-s),  w += x )
#define FF(x, y, z) (z ^ (x & (y ^ z)))
#define GG(x, y, z) (y ^ (z & (x ^ y)))
#define HH(x, y, z) (x ^ y ^ z)
#define II(x, y, z) (y ^ (x | ~z))

#define com_mcpy memcpy
#define com_mset memset

typedef struct uavs3e_com_md5_t {
    unsigned int     h[4]; /* hash state ABCD */
    unsigned char    msg[64]; /*input buffer (nal message) */
    unsigned int     bits[2]; /* number of bits, modulo 2^64 (lsb first)*/
} com_md5_t;

static void com_md5_trans(unsigned int *buf, unsigned int *msg)
{
    register unsigned int a, b, c, d;
    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];
    MD5FUNC(FF, a, b, c, d, msg[0], 7, 0xd76aa478); /* 1 */
    MD5FUNC(FF, d, a, b, c, msg[1], 12, 0xe8c7b756); /* 2 */
    MD5FUNC(FF, c, d, a, b, msg[2], 17, 0x242070db); /* 3 */
    MD5FUNC(FF, b, c, d, a, msg[3], 22, 0xc1bdceee); /* 4 */
    MD5FUNC(FF, a, b, c, d, msg[4], 7, 0xf57c0faf); /* 5 */
    MD5FUNC(FF, d, a, b, c, msg[5], 12, 0x4787c62a); /* 6 */
    MD5FUNC(FF, c, d, a, b, msg[6], 17, 0xa8304613); /* 7 */
    MD5FUNC(FF, b, c, d, a, msg[7], 22, 0xfd469501); /* 8 */
    MD5FUNC(FF, a, b, c, d, msg[8], 7, 0x698098d8); /* 9 */
    MD5FUNC(FF, d, a, b, c, msg[9], 12, 0x8b44f7af); /* 10 */
    MD5FUNC(FF, c, d, a, b, msg[10], 17, 0xffff5bb1); /* 11 */
    MD5FUNC(FF, b, c, d, a, msg[11], 22, 0x895cd7be); /* 12 */
    MD5FUNC(FF, a, b, c, d, msg[12], 7, 0x6b901122); /* 13 */
    MD5FUNC(FF, d, a, b, c, msg[13], 12, 0xfd987193); /* 14 */
    MD5FUNC(FF, c, d, a, b, msg[14], 17, 0xa679438e); /* 15 */
    MD5FUNC(FF, b, c, d, a, msg[15], 22, 0x49b40821); /* 16 */
    /* Round 2 */
    MD5FUNC(GG, a, b, c, d, msg[1], 5, 0xf61e2562); /* 17 */
    MD5FUNC(GG, d, a, b, c, msg[6], 9, 0xc040b340); /* 18 */
    MD5FUNC(GG, c, d, a, b, msg[11], 14, 0x265e5a51); /* 19 */
    MD5FUNC(GG, b, c, d, a, msg[0], 20, 0xe9b6c7aa); /* 20 */
    MD5FUNC(GG, a, b, c, d, msg[5], 5, 0xd62f105d); /* 21 */
    MD5FUNC(GG, d, a, b, c, msg[10], 9, 0x2441453); /* 22 */
    MD5FUNC(GG, c, d, a, b, msg[15], 14, 0xd8a1e681); /* 23 */
    MD5FUNC(GG, b, c, d, a, msg[4], 20, 0xe7d3fbc8); /* 24 */
    MD5FUNC(GG, a, b, c, d, msg[9], 5, 0x21e1cde6); /* 25 */
    MD5FUNC(GG, d, a, b, c, msg[14], 9, 0xc33707d6); /* 26 */
    MD5FUNC(GG, c, d, a, b, msg[3], 14, 0xf4d50d87); /* 27 */
    MD5FUNC(GG, b, c, d, a, msg[8], 20, 0x455a14ed); /* 28 */
    MD5FUNC(GG, a, b, c, d, msg[13], 5, 0xa9e3e905); /* 29 */
    MD5FUNC(GG, d, a, b, c, msg[2], 9, 0xfcefa3f8); /* 30 */
    MD5FUNC(GG, c, d, a, b, msg[7], 14, 0x676f02d9); /* 31 */
    MD5FUNC(GG, b, c, d, a, msg[12], 20, 0x8d2a4c8a); /* 32 */
    /* Round 3 */
    MD5FUNC(HH, a, b, c, d, msg[5], 4, 0xfffa3942); /* 33 */
    MD5FUNC(HH, d, a, b, c, msg[8], 11, 0x8771f681); /* 34 */
    MD5FUNC(HH, c, d, a, b, msg[11], 16, 0x6d9d6122); /* 35 */
    MD5FUNC(HH, b, c, d, a, msg[14], 23, 0xfde5380c); /* 36 */
    MD5FUNC(HH, a, b, c, d, msg[1], 4, 0xa4beea44); /* 37 */
    MD5FUNC(HH, d, a, b, c, msg[4], 11, 0x4bdecfa9); /* 38 */
    MD5FUNC(HH, c, d, a, b, msg[7], 16, 0xf6bb4b60); /* 39 */
    MD5FUNC(HH, b, c, d, a, msg[10], 23, 0xbebfbc70); /* 40 */
    MD5FUNC(HH, a, b, c, d, msg[13], 4, 0x289b7ec6); /* 41 */
    MD5FUNC(HH, d, a, b, c, msg[0], 11, 0xeaa127fa); /* 42 */
    MD5FUNC(HH, c, d, a, b, msg[3], 16, 0xd4ef3085); /* 43 */
    MD5FUNC(HH, b, c, d, a, msg[6], 23, 0x4881d05); /* 44 */
    MD5FUNC(HH, a, b, c, d, msg[9], 4, 0xd9d4d039); /* 45 */
    MD5FUNC(HH, d, a, b, c, msg[12], 11, 0xe6db99e5); /* 46 */
    MD5FUNC(HH, c, d, a, b, msg[15], 16, 0x1fa27cf8); /* 47 */
    MD5FUNC(HH, b, c, d, a, msg[2], 23, 0xc4ac5665); /* 48 */
    /* Round 4 */
    MD5FUNC(II, a, b, c, d, msg[0], 6, 0xf4292244); /* 49 */
    MD5FUNC(II, d, a, b, c, msg[7], 10, 0x432aff97); /* 50 */
    MD5FUNC(II, c, d, a, b, msg[14], 15, 0xab9423a7); /* 51 */
    MD5FUNC(II, b, c, d, a, msg[5], 21, 0xfc93a039); /* 52 */
    MD5FUNC(II, a, b, c, d, msg[12], 6, 0x655b59c3); /* 53 */
    MD5FUNC(II, d, a, b, c, msg[3], 10, 0x8f0ccc92); /* 54 */
    MD5FUNC(II, c, d, a, b, msg[10], 15, 0xffeff47d); /* 55 */
    MD5FUNC(II, b, c, d, a, msg[1], 21, 0x85845dd1); /* 56 */
    MD5FUNC(II, a, b, c, d, msg[8], 6, 0x6fa87e4f); /* 57 */
    MD5FUNC(II, d, a, b, c, msg[15], 10, 0xfe2ce6e0); /* 58 */
    MD5FUNC(II, c, d, a, b, msg[6], 15, 0xa3014314); /* 59 */
    MD5FUNC(II, b, c, d, a, msg[13], 21, 0x4e0811a1); /* 60 */
    MD5FUNC(II, a, b, c, d, msg[4], 6, 0xf7537e82); /* 61 */
    MD5FUNC(II, d, a, b, c, msg[11], 10, 0xbd3af235); /* 62 */
    MD5FUNC(II, c, d, a, b, msg[2], 15, 0x2ad7d2bb); /* 63 */
    MD5FUNC(II, b, c, d, a, msg[9], 21, 0xeb86d391); /* 64 */
    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

void com_md5_init(com_md5_t *md5)
{
    md5->h[0] = 0x67452301;
    md5->h[1] = 0xefcdab89;
    md5->h[2] = 0x98badcfe;
    md5->h[3] = 0x10325476;
    md5->bits[0] = 0;
    md5->bits[1] = 0;
}

void com_md5_update(com_md5_t *md5, void *buf_t, unsigned int len)
{
    unsigned char *buf;
    unsigned int i, idx, part_len;
    buf = (unsigned char *)buf_t;
    idx = (unsigned int)((md5->bits[0] >> 3) & 0x3f);
    md5->bits[0] += (len << 3);
    if (md5->bits[0] < (len << 3)) {
        (md5->bits[1])++;
    }
    md5->bits[1] += (len >> 29);
    part_len = 64 - idx;
    if (len >= part_len) {
        com_mcpy(md5->msg + idx, buf, part_len);
        com_md5_trans(md5->h, (unsigned int *)md5->msg);
        for (i = part_len; i + 63 < len; i += 64) {
            com_md5_trans(md5->h, (unsigned int *)(buf + i));
        }
        idx = 0;
    }
    else {
        i = 0;
    }
    if (len - i > 0) {
        com_mcpy(md5->msg + idx, buf + i, len - i);
    }
}

static void com_md5_update_s16(com_md5_t *md5, void *buf_t, unsigned int len)
{
    unsigned char *buf;
    unsigned int i, idx, part_len, j;
    unsigned char t[8196 * 10];
    buf = (unsigned char *)buf_t;
    idx = (unsigned int)((md5->bits[0] >> 3) & 0x3f);
    len = len * 2;
    for (j = 0; j < len; j += 2) {
        t[j] = (unsigned char)(*(buf));
        t[j + 1] = 0;
        buf++;
    }
    md5->bits[0] += (len << 3);
    if (md5->bits[0] < (len << 3)) {
        (md5->bits[1])++;
    }
    md5->bits[1] += (len >> 29);
    part_len = 64 - idx;
    if (len >= part_len) {
        com_mcpy(md5->msg + idx, t, part_len);
        com_md5_trans(md5->h, (unsigned int *)md5->msg);
        for (i = part_len; i + 63 < len; i += 64) {
            com_md5_trans(md5->h, (unsigned int *)(t + i));
        }
        idx = 0;
    }
    else {
        i = 0;
    }
    if (len - i > 0) {
        com_mcpy(md5->msg + idx, t + i, len - i);
    }
}

void com_md5_finish(com_md5_t *md5, unsigned char digest[16])
{
    unsigned char *pos;
    int cnt;
    cnt = (md5->bits[0] >> 3) & 0x3F;
    pos = md5->msg + cnt;
    *pos++ = 0x80;
    cnt = 64 - 1 - cnt;
    if (cnt < 8) {
        com_mset(pos, 0, cnt);
        com_md5_trans(md5->h, (unsigned int *)md5->msg);
        com_mset(md5->msg, 0, 56);
    }
    else {
        com_mset(pos, 0, cnt - 8);
    }
    com_mcpy((md5->msg + 14 * sizeof(unsigned int)), &md5->bits[0], sizeof(unsigned int));
    com_mcpy((md5->msg + 15 * sizeof(unsigned int)), &md5->bits[1], sizeof(unsigned int));
    com_md5_trans(md5->h, (unsigned int *)md5->msg);
    com_mcpy(digest, md5->h, 16);
    com_mset(md5, 0, sizeof(com_md5_t));
}

void com_md5_img(image_t *img, unsigned char digest[16])
{
    com_md5_t md5;
    int i, j;
    com_md5_init(&md5);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < img->height >> (i ? 1 : 0); j++) {
            int width = img->width >> (i ? 1 : 0);
#if (COMPILE_10BIT == 0)
            com_md5_update_s16(&md5, ((unsigned char *)img->plane[i]) + j * img->i_stride[i], width);
#else
            com_md5_update(&md5, ((unsigned char *)img->plane[i]) + j * img->i_stride[i] * 2, width * 2);
#endif
        }
    }

    com_md5_finish(&md5, digest);
}

int IsEqualRps(com_rpl_t *rpl, ref_man * Rps2, int lidx)
{
    for (int i = 0; i < MAXGOP; i++) {
        com_rpl_t *p;

        if (lidx == 0) {
           p = &Rps2[i].l0;
        } else {
           p = &Rps2[i].l1;
        }

        if (p->num == rpl->num && p->active == rpl->active) {
            int j;

            for (j = 0; j < rpl->num; j++) {
                if (p->delta_doi[j] != rpl->delta_doi[j]) {
                    break;
                }
            }
            if (j == rpl->num) {
                return i;
            }
        }
    }
    
    return -1;
}

static void write_rpl(bit_stream_t *strm, com_rpl_t *rpl)
{
    int ddoi_base = 0;
    UE_V("rpl_num", rpl->num, strm);

    if (rpl->num > 0) {
        UE_V("abs(delta_doi)", (rpl->delta_doi[0]), strm);

        if (rpl->delta_doi[0] != 0) {
            U_V(1, "delta_doi_sign", rpl->delta_doi[0] < 0, strm);
        }
        ddoi_base = rpl->delta_doi[0];
    }
    for (int i = 1; i < rpl->num; ++i) {
        int deltaRefPic = rpl->delta_doi[i] - ddoi_base;
        UE_V("abs(delta_delta_doi)", abs(deltaRefPic), strm);

        if (deltaRefPic != 0) {
            U_V(1, "delta_delta_doi_sign", deltaRefPic < 0, strm);

        }
        ddoi_base = rpl->delta_doi[i];
    }
}

int WriteSequenceHeader(bit_stream_t *strm, cfg_param_t *input)
{
    int  i;

    strm->byte_pos   = 0;
    strm->bits_to_go = 8;

    U_V(32, "seqence start code", 0x1b0, strm);
    U_V(8, "profile_id", input->profile_id, strm);
    U_V(8, "level_id", 0x6A, strm);
    U_V(1, "progressive_sequence", input->InterlaceCodingOption ^ 0x1, strm);
    U_V(1, "field_coded_sequence", input->InterlaceCodingOption, strm);

    U_V(1, "library_stream_flag", 0, strm);
    U_V(1, "library_picture_enable_flag", 0, strm);

    U_V(1, "marker bit", 1, strm);
    U_V(14, "horizontal_size", input->img_width, strm);
    U_V(1, "marker bit", 1, strm);
    U_V(14, "vertical_size", input->img_height, strm);

    U_V(2, "chroma format", 1, strm);
    U_V(3, "sample precision", (input->sample_bit_depth - 6) / 2, strm);

    if (input->profile_id == BASELINE10_PROFILE) { // 10bit profile
        U_V(3, "encoding precision", (input->bit_depth - 6) / 2, strm);
    }
    U_V(1, "marker bit", 1, strm);

    U_V(4, "aspect ratio information", 1, strm);
    U_V(4, "frame rate code", input->frame_rate_code, strm);
    U_V(1, "marker bit", 1, strm);
    U_V(18, "bit rate lower", (input->target_bitrate * 1000 / 400) & 0x3FFFF, strm);
    U_V(1, "marker bit", 1, strm);
    U_V(12, "bit rate upper", ((input->target_bitrate * 1000 / 400) >> 18) & 0xFFF, strm);
    U_V(1, "low delay", !input->succ_bfrms, strm);

    U_V(1, "temporal_id exsit flag", 0, strm);
    U_V(1, "marker bit", 1, strm);
    U_V(18, "bbv buffer size", 48829, strm);
    U_V(1, "marker bit", 1, strm);
    U_V(4, "max_dpb_size", 15, strm);

    U_V(1, "rpl1_index_exist_flag", 1, strm);
    U_V(1, "rpl1_same_as_rpl0_flag", 0, strm);
    U_V(1, "marker bit", 1, strm);

    UE_V("num_of_RPS", input->gop_size, strm);

    for (i = 0; i < input->gop_size; i++) {
        write_rpl(strm, &input->seq_ref_cfg[i].l0);
    }

    UE_V("num_of_RPS", input->gop_size, strm);
    for (i = 0; i < input->gop_size; i++) {
        write_rpl(strm, &input->seq_ref_cfg[i].l1);
    }
    UE_V("active_ref_minus1_L0", 1, strm);
    UE_V("active_ref_minus1_L1", 1, strm);

    U_V(3, "log2_max_cuwh - 2", LCU_SIZE_IN_BITS - 2, strm);
    U_V(2, "log2_min_cu_size - 2", 0, strm);
    U_V(2, "log2_max_part_ratio - 2", 1, strm);
    U_V(3, "max_split_times - 6", 0, strm);
    U_V(3, "log2_min_qt_size - 2", 1, strm);
    U_V(3, "log2_max_bt_size - 2", 4, strm);
    U_V(2, "log2_max_eqt_size - 2", 1, strm);
    U_V(1, "marker bit", 1, strm);

    U_V(1, "weight_quant_enable", input->use_wquant, strm);

    if (input->use_wquant) {
        U_V(1, "load seq weight quant data flag", 0, strm);
    }

    U_V(1, "secT enabled", input->tools.use_secT, strm);
    U_V(1, "SAO Enable Flag", input->tools.use_sao, strm);
    U_V(1, "ALF Enable Flag", 0, strm);
    U_V(1, "affine_enable", 0, strm);

    U_V(1, "smvd_enable", 0, strm);
    U_V(1, "ipcm_enable_flag", 0, strm);
    U_V(1, "amvr_enable", 0, strm);
    U_V(4, "num_of_hmvp", 1, strm);
    U_V(1, "umve_enable", 0, strm);
    U_V(1, "ipf_enable_flag", 0, strm);
    U_V(1, "tscpm_enable", 0, strm);
    U_V(1, "marker bit", 1, strm);

    U_V(1, "dt_enable", 0, strm);
    U_V(1, "pbt_enable", 1, strm);

    if (input->succ_bfrms) {
        U_V(5, "pic_reorder_delay", input->pic_reorder_delay, strm);
    }

    U_V(1, "Cross Loop Filter Flag", LOOPFLT_CROSS_SLICE, strm);
    U_V(1, "colocated_patch", 0, strm);
    U_V(1, "patch_stable", 1, strm);
    U_V(1, "patch_uniform", 1, strm);
    U_V(1, "marker bit", 1, strm);
    UE_V("patch_width - 1",  input->pic_width_in_lcu, strm);
    UE_V("patch_height - 1", input->pic_height_in_lcu, strm);

    U_V(2, "reserve bit", 0, strm);

    strm->byte_buf <<= strm->bits_to_go;
    strm->byte_buf |= (1 << (strm->bits_to_go - 1));
    strm->streamBuffer[strm->byte_pos++] = strm->byte_buf;
    strm->bits_to_go = 8;

    return strm->byte_pos * 8;
}

int WriteSequenceDisplayExtension(bit_stream_t *strm, const cfg_param_t *input)
{
    int color_description = 1;

    U_V(32, "sequence display extension start code", 0x1b5, strm);
    U_V(4, "extension id", 2, strm);
    U_V(3, "video format", 0, strm);
    U_V(1, "video range", 0, strm);
    U_V(1, "color description", color_description, strm);

    if (color_description) {
        U_V(8, "color primaries", input->ColourPrimary, strm);
        U_V(8, "transfer characteristics", input->TransferChar, strm);
        U_V(8, "matrix coefficients", input->MatrixCoeff, strm);
    }

    U_V(14, "display horizontal size", input->img_width, strm);
    U_V( 1, "marker bit", 1, strm);
    U_V(14, "display vertical size", input->img_height, strm);
    U_V( 1, "3D mode", 0, strm);

    strm->byte_buf <<= strm->bits_to_go;
    strm->byte_buf |= (1 << (strm->bits_to_go - 1));
    strm->streamBuffer[strm->byte_pos++] = strm->byte_buf;
    strm->bits_to_go = 8;

    return strm->byte_pos * 8;
}

int WriteHdrExtension(bit_stream_t *strm, const hdr_ext_data_t *hdr)
{
    U_V(32, "sequence display extension start code", 0x1b5, strm);
    U_V(4, "extension id", 12, strm);

    U_V(16, "display_primaries_x", hdr->display_primaries_x[0], strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "display_primaries_y", hdr->display_primaries_y[0], strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "display_primaries_x", hdr->display_primaries_x[1], strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "display_primaries_y", hdr->display_primaries_y[1], strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "display_primaries_x", hdr->display_primaries_x[2], strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "display_primaries_y", hdr->display_primaries_y[2], strm); U_V(1, "marker bit", 1, strm);

    U_V(16, "white_point_x", hdr->white_point_x, strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "white_point_y", hdr->white_point_y, strm); U_V(1, "marker bit", 1, strm);

    U_V(16, "max_display_mastering_luminance", hdr->max_display_mastering_luminance, strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "min_display_mastering_luminance", hdr->min_display_mastering_luminance, strm); U_V(1, "marker bit", 1, strm);

    U_V(16, "max_content_light_level", hdr->max_content_light_level, strm); U_V(1, "marker bit", 1, strm);
    U_V(16, "max_picture_average_light_level", hdr->max_picture_average_light_level, strm); U_V(1, "marker bit", 1, strm);

    U_V(16, "revered bits", 1, strm);

  
    strm->byte_buf <<= strm->bits_to_go;
    strm->byte_buf |= (1 << (strm->bits_to_go - 1));
    strm->streamBuffer[strm->byte_pos++] = strm->byte_buf;
    strm->bits_to_go = 8;

    return strm->byte_pos * 8;
}

int WriteSequenceUserData(bit_stream_t *strm)
{
    char user_data[1024];
    int i;
    size_t length;

    sprintf(user_data, " UVCodec-AVS3 @UVVision, Shenzhen (91440300MA5F2PCB5U), Version:%s_%s SHA-1:%s ", VERSION_STR, VERSION_TYPE, VERSION_SHA1);
    length = strlen(user_data);

    U_V(32, "user data start code", 0x1b2, strm);

    for (i = 0; i < length; i++) {
        U_V(8, "user data", user_data[i], strm);
    }

    strm->byte_buf <<= strm->bits_to_go;
    strm->byte_buf |= (1 << (strm->bits_to_go - 1));
    strm->streamBuffer[strm->byte_pos++] = strm->byte_buf;
    strm->bits_to_go = 8;

    return strm->byte_pos * 8;
}

void write_pic_hdr_i(avs3_enc_t *h)
{
    bit_stream_t *strm = &h->hdr_stream;
    int bbv_delay, bbv_delay_extension;
    cfg_param_t *input = (cfg_param_t *)h->input;
    i64s_t coi = h->coi;
    i64s_t poc = h->poc;
    int RPS_idx = ((coi - 1) + input->gop_size) % input->gop_size;
    int rpl_in_seqhdr_idx0 = IsEqualRps(&h->curr_RPS.l0, input->seq_ref_cfg, 0);
    int rpl_in_seqhdr_idx1 = IsEqualRps(&h->curr_RPS.l1, input->seq_ref_cfg, 1);

    U_V(32, "I picture start code", 0x1B3, strm);

    bbv_delay = 0xFFFF;
    bbv_delay_extension = 0xFF;

    U_V(32, "bbv delay", bbv_delay, strm);
    U_V( 1, "time_code_flag", 0, strm);

    U_V(8, "coi", coi & 0xFF, strm);

    if (input->succ_bfrms) {
        int displaydelay = (int)(poc - coi) + input->pic_reorder_delay;
        UE_V("picture_output_delay", displaydelay, strm);
    }
    if (!input->succ_bfrms) {
        UE_V("bbv check times", 0, strm);
    }
    U_V(1, "progressive frame", input->InterlaceCodingOption ^ 0x01, strm);

    if (input->InterlaceCodingOption) {
        U_V(1, "picture_structure", 1, strm);
    }

    U_V(1, "top field first", 0, strm);
    U_V(1, "repeat first field", 0, strm);

    if (input->InterlaceCodingOption) {
        U_V(1, "is top field", h->is_top_field, strm);
        U_V(1, "reserved bit for interlace coding", 0, strm);
    }

    U_V(1, "use RCS in SPS", rpl_in_seqhdr_idx0 >= 0, strm);

    if (rpl_in_seqhdr_idx0 >= 0) {
        UE_V("idx_of_rpl0_in_seqhdr", rpl_in_seqhdr_idx0, strm);
    } else {
        write_rpl(strm, &h->curr_RPS.l0);
    }

    U_V(1, "use RCS in SPS", rpl_in_seqhdr_idx1 >= 0, strm);

    if (rpl_in_seqhdr_idx1 >= 0) {
        UE_V("idx_of_rpl1_in_seqhdr", rpl_in_seqhdr_idx1, strm);
    } else {
        write_rpl(strm, &h->curr_RPS.l1);
    }

    U_V(1, "fixed picture qp", !h->adaptiveQP, strm);

    U_V(7, "I picture QP", h->img_org->initQP, strm);

    U_V(1, "loop filter disable", !input->tools.use_deblk, strm);

    if (input->tools.use_deblk) {
        U_V(1, "loop filter parameter flag", input->loop_filter_parameter_flag, strm);

        if (input->loop_filter_parameter_flag) {
            SE_V("alpha offset", input->alpha_c_offset, strm);
            SE_V("beta offset", input->beta_offset, strm);
        }
    }
    U_V(1, "chroma_quant_param_disable", !input->use_hdr_chroma_opt, strm);

    if (input->use_hdr_chroma_opt) {
        SE_V("chroma_quant_param_delta_u", h->chroma_delta_qp[0], strm);
        SE_V("chroma_quant_param_delta_v", h->chroma_delta_qp[1], strm);
    }

    if (input->use_wquant) {
        U_V(1, "pic weight quant enable", 1, strm);
        U_V(2, "pic weight quant data index", 0, strm);
    }
}

void write_pic_hdr_pb(avs3_enc_t *h)
{
    cfg_param_t *input = (cfg_param_t *)h->input;
    bit_stream_t *strm = &h->hdr_stream;

    int bbv_delay, bbv_delay_extension;
    int picture_coding_type;
    int RPS_idx = (h->coi - 1) % input->gop_size;
    int rpl_in_seqhdr_idx0 = IsEqualRps(&h->curr_RPS.l0, input->seq_ref_cfg, 0);
    int rpl_in_seqhdr_idx1 = IsEqualRps(&h->curr_RPS.l1, input->seq_ref_cfg, 1);

    if (h->type & P_FRM) {
        picture_coding_type = 1;
    } else {
        picture_coding_type = 2;
    }

    bbv_delay = 0xFFFF;
    bbv_delay_extension = 0xFF;

    U_V(24, "start code prefix", 1, strm);
    U_V(8, "PB picture start code", 0xB6, strm);

    U_V(1, "random_access_decodable_flag", h->p_cur_frm->randomaccese_flag, strm);

    //xyji 12.23
    U_V(32, "bbv delay", bbv_delay, strm);

    U_V(2, "picture coding type", picture_coding_type, strm);

    U_V(8, "coi", h->coi & 0xFF, strm);

    if (input->succ_bfrms) {
        int displaydelay = (int)(h->poc - h->coi) + input->pic_reorder_delay;
        UE_V("displaydelay", displaydelay, strm);
    }
    if (!input->succ_bfrms) {
        UE_V("bbv check times", 0, strm);
    }

    U_V(1, "progressive frame", input->InterlaceCodingOption ^ 0x1, strm);

    if (input->InterlaceCodingOption) {
        U_V(1, "picture_structure", 1, strm);
    }

    U_V(1, "top field first", 0, strm);
    U_V(1, "repeat first field", 0, strm);

    if (input->InterlaceCodingOption) {
        U_V(1, "is top field", h->is_top_field, strm);
        U_V(1, "reserved bit for interlace coding", 0, strm);
    }

    U_V(1, "use RCS in SPS", rpl_in_seqhdr_idx0 >= 0, strm);

    if (rpl_in_seqhdr_idx0 >= 0) {
        UE_V("idx_of_rpl0_in_seqhdr", rpl_in_seqhdr_idx0, strm);
    } else {
        write_rpl(strm, &h->curr_RPS.l0);
    }

    U_V(1, "use RCS in SPS", rpl_in_seqhdr_idx1 >= 0, strm);

    if (rpl_in_seqhdr_idx1 >= 0) {
        UE_V("idx_of_rpl1_in_seqhdr", rpl_in_seqhdr_idx1, strm);
    } else {
        write_rpl(strm, &h->curr_RPS.l1);
    }

    U_V(1, "num_ref_idx_active_override_flag", 1, strm);
    UE_V("rpl0 active", h->curr_RPS.l0.active - 1, strm);

    if (h->type & B_FRM) {
        UE_V("rpl1 active", h->curr_RPS.l1.active - 1, strm);
    }

    U_V(1, "fixed picture qp", !h->adaptiveQP, strm);
    U_V(7, "B picture QP", h->img_org->initQP, strm);

    if (!(picture_coding_type == 2 && 1 == 1)) {
        U_V(1, "reserved_bit", 0, strm);
    }

    U_V(1, "loop filter disable", !input->tools.use_deblk, strm);

    if (input->tools.use_deblk) {
        U_V(1, "loop filter parameter flag", input->loop_filter_parameter_flag, strm);

        if (input->loop_filter_parameter_flag) {
            SE_V("alpha offset", input->alpha_c_offset, strm);
            SE_V("beta offset", input->beta_offset, strm);
        }
    }
    U_V(1, "chroma_quant_param_disable", !input->use_hdr_chroma_opt, strm);

    if (input->use_hdr_chroma_opt) {
        SE_V("chroma_quant_param_delta_u", h->chroma_delta_qp[0], strm);
        SE_V("chroma_quant_param_delta_v", h->chroma_delta_qp[1], strm);
    }

    if (input->use_wquant) {
        U_V(1, "pic weight quant enable", 1, strm);
        U_V(2, "pic weight quant data index", 0, strm);
    }
}

int write_md5_usr_data(avs3_enc_t *h, image_t *img)
{
    char pic_sign[16];
    bit_stream_t *strm = &h->hdr_stream;

    U_V(24, "start code prefix", 1, strm);
    U_V( 8, "start code",     0xB2, strm);

    com_md5_img(img, pic_sign);

    /* write user data type */
    U_V(8, "start code", 0x10, strm);

    for (int i = 0; i < 16; i++) {
        U_V(8, "md5", pic_sign[i], strm);

        if (i % 2 == 1) {
            U_V(1, "md5 split", 1, strm);
        }
    }
    U_V(8, "md5 end", 0xFF, strm);

    return 0;
}

void finish_picture_header(bit_stream_t *strm)
{
    strm->byte_buf <<= strm->bits_to_go;
    strm->byte_buf |= (1 << (strm->bits_to_go - 1));
    strm->streamBuffer[strm->byte_pos++] = strm->byte_buf;
    strm->bits_to_go = 8;
    strm->byte_buf = 0;
}

void write_slice_header(avs3_enc_t *h, bit_stream_t *strm, int patch_idx, int slice_qp)
{
    const cfg_param_t *input = h->input;
    sao_enc_t *sao = &h->sao_enc;

    U_V(24, "start code prefix", 1, strm);

    U_V(8, "patch idx", patch_idx, strm);

    if (h->adaptiveQP) {
        U_V(1, "fixed_slice_qp", 0, strm);
        U_V(7, "slice_qp", slice_qp, strm);
    }

    if (input->tools.use_sao) {
        U_V(1, "sao_slice_flag_Y ", sao->sao_on[0], strm);
        U_V(1, "sao_slice_flag_Cb", sao->sao_on[1], strm);
        U_V(1, "sao_slice_flag_Cr", sao->sao_on[2], strm);
    } 

    if (strm->bits_to_go < 8) {
        strm->byte_buf = (strm->byte_buf << strm->bits_to_go) | (0xff >> (8 - strm->bits_to_go));
        strm->streamBuffer[strm->byte_pos++] = strm->byte_buf;
        strm->bits_to_go = 8;
    }
}

