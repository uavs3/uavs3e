#include "intrinsic.h"

#define CALCU_ETYPE_16(s0, s1, s2, t0, t1, t2, t3, etype) \
                       t3 = _mm_min_epu8(s0, s1);   \
                       t1 = _mm_cmpeq_epi8(t3, s0); \
                       t2 = _mm_cmpeq_epi8(t3, s1); \
                       t0 = _mm_subs_epi8(t2, t1);  \
                       t3 = _mm_min_epu8(s1, s2);   \
                       t1 = _mm_cmpeq_epi8(t3, s1); \
                       t2 = _mm_cmpeq_epi8(t3, s2); \
                       t3 = _mm_subs_epi8(t1, t2);  \
                       etype = _mm_adds_epi8(t0, t3);

#define CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, etype) \
	t3 = _mm_min_epu16(s0, s1);   \
	t1 = _mm_cmpeq_epi16(t3, s0); \
	t2 = _mm_cmpeq_epi16(t3, s1); \
	t0 = _mm_subs_epi16(t2, t1);  \
	t3 = _mm_min_epu16(s1, s2);   \
	t1 = _mm_cmpeq_epi16(t3, s1); \
	t2 = _mm_cmpeq_epi16(t3, s2); \
	t3 = _mm_subs_epi16(t1, t2);  \
	etype = _mm_adds_epi16(t0, t3);   \
	etype = _mm_adds_epi16(etype, cst2);

#define CALCU_ETYPE_8_NOADD2(s0, s1, s2, t0, t1, t2, t3, etype) \
	t3 = _mm_min_epu16(s0, s1);   \
	t1 = _mm_cmpeq_epi16(t3, s0); \
	t2 = _mm_cmpeq_epi16(t3, s1); \
	t0 = _mm_subs_epi16(t2, t1);  \
	t3 = _mm_min_epu16(s1, s2);   \
	t1 = _mm_cmpeq_epi16(t3, s1); \
	t2 = _mm_cmpeq_epi16(t3, s2); \
	t3 = _mm_subs_epi16(t1, t2);  \
	etype = _mm_adds_epi16(t0, t3);

#define CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype) \
    t3 = _mm_min_epu8(s0, s1);   \
    t1 = _mm_cmpeq_epi8(t3, s0); \
    t2 = _mm_cmpeq_epi8(t3, s1); \
    t0 = _mm_subs_epi8(t2, t1);  \
    t3 = _mm_min_epu8(s1, s2);   \
    t1 = _mm_cmpeq_epi8(t3, s1); \
    t2 = _mm_cmpeq_epi8(t3, s2); \
    t3 = _mm_subs_epi8(t1, t2);  \
    etype = _mm_adds_epi8(t0, t3); \
    etype = _mm_adds_epi8(etype, cst2);

#define CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype) \
	t3 = _mm_min_epu16(s0, s1);   \
	t1 = _mm_cmpeq_epi16(t3, s0); \
	t2 = _mm_cmpeq_epi16(t3, s1); \
	t0 = _mm_subs_epi16(t2, t1);  \
	t3 = _mm_min_epu16(s1, s2);   \
	t1 = _mm_cmpeq_epi16(t3, s1); \
	t2 = _mm_cmpeq_epi16(t3, s2); \
	t3 = _mm_subs_epi16(t1, t2);  \
	etype = _mm_adds_epi16(t0, t3); \
	etype = _mm_adds_epi16(etype, cst2);

void SAO_on_block_sse128(void *handle, void* sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height, 
    int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down)
{
    avs3_enc_t *h = (avs3_enc_t *)handle;
    SAOBlkParam *saoBlkParam = (SAOBlkParam *)sao_data;
    int type;
    int start_x, end_x, start_y, end_y;
    int start_x_r0, end_x_r0, start_x_r, end_x_r, start_x_rn, end_x_rn;
    int x, y;
    pel_t *src, *dst;
    int i_src, i_dst;
    __m128i zero = _mm_setzero_si128();
    __m128i off0, off1, off2, off3, off4;
    __m128i s0, s1, s2;
    __m128i t0, t1, t2, t3, t4, etype;
    __m128i c0, c1, c2, c3, c4;
    __m128i mask;
    int smb_available_upleft = (smb_available_up && smb_available_left);
    int smb_available_upright = (smb_available_up && smb_available_right);
    int smb_available_leftdown = (smb_available_down && smb_available_left);
    int smb_available_rightdwon = (smb_available_down && smb_available_right);

    if (!(lcu_pix_height && lcu_pix_width))
    {
        return;
    }

    i_src = h->img_sao.i_stride [compIdx];
    i_dst = h->img_rec->i_stride[compIdx];
    src = h->img_sao.plane [compIdx] + pix_y * i_src + pix_x;
    dst = h->img_rec->plane[compIdx] + pix_y * i_dst + pix_x;

    type = saoBlkParam->typeIdc;

    switch (type) {
    case SAO_TYPE_EO_0: {
        int end_x_16;

        start_x = smb_available_left ? 0 : 1;
        end_x = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
        end_x_16 = end_x - ((end_x - start_x) & 0x0f);

        mask = _mm_load_si128((__m128i*)(intrinsic_mask[end_x - end_x_16 - 1]));
        if (lcu_pix_width == 4){
            c0 = _mm_set1_epi16(-2);
            c1 = _mm_set1_epi16(-1);
            c2 = _mm_set1_epi16(0);
            c3 = _mm_set1_epi16(1);
            c4 = _mm_set1_epi16(2);

            off0 = _mm_set1_epi16((char)saoBlkParam->offset[0]);
            off1 = _mm_set1_epi16((char)saoBlkParam->offset[1]);
            off2 = _mm_set1_epi16((char)saoBlkParam->offset[2]);
            off3 = _mm_set1_epi16((char)saoBlkParam->offset[3]);
            off4 = _mm_set1_epi16((char)saoBlkParam->offset[4]);

            for (y = 0; y < lcu_pix_height; y++) {
                //diff = src[start_x] - src[start_x - 1];
                //leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                s0 = _mm_loadl_epi64((__m128i*)&src[start_x - 1]);
                s0 = _mm_unpacklo_epi8(s0, zero);
                s1 = _mm_srli_si128(s0, 2);
                s2 = _mm_srli_si128(s0, 4);

                t3 = _mm_min_epu16(s0, s1);
                t1 = _mm_cmpeq_epi16(t3, s0);
                t2 = _mm_cmpeq_epi16(t3, s1);
                t0 = _mm_subs_epi16(t2, t1); //leftsign

                t3 = _mm_min_epu16(s1, s2);
                t1 = _mm_cmpeq_epi16(t3, s1);
                t2 = _mm_cmpeq_epi16(t3, s2);
                t3 = _mm_subs_epi16(t1, t2); //rightsign

                etype = _mm_adds_epi16(t0, t3); //edgetype

                t0 = _mm_cmpeq_epi16(etype, c0);
                t1 = _mm_cmpeq_epi16(etype, c1);
                t2 = _mm_cmpeq_epi16(etype, c2);
                t3 = _mm_cmpeq_epi16(etype, c3);
                t4 = _mm_cmpeq_epi16(etype, c4);

                t0 = _mm_and_si128(t0, off0);
                t1 = _mm_and_si128(t1, off1);
                t2 = _mm_and_si128(t2, off2);
                t3 = _mm_and_si128(t3, off3);
                t4 = _mm_and_si128(t4, off4);

                t0 = _mm_adds_epi16(t0, t1);
                t2 = _mm_adds_epi16(t2, t3);
                t0 = _mm_adds_epi16(t0, t4);
                t0 = _mm_adds_epi16(t0, t2);//get offset

                t1 = _mm_adds_epi16(t0, s1);
                t0 = _mm_packus_epi16(t1, zero); //saturated

                _mm_maskmoveu_si128(t0, mask, (char_t*)(dst + start_x));

                dst += i_dst;
                src += i_src;
            }
        }
        else{
            c0 = _mm_set1_epi8(-2);
            c1 = _mm_set1_epi8(-1);
            c2 = _mm_set1_epi8(0);
            c3 = _mm_set1_epi8(1);
            c4 = _mm_set1_epi8(2);

            off0 = _mm_set1_epi8((char)saoBlkParam->offset[0]);
            off1 = _mm_set1_epi8((char)saoBlkParam->offset[1]);
            off2 = _mm_set1_epi8((char)saoBlkParam->offset[2]);
            off3 = _mm_set1_epi8((char)saoBlkParam->offset[3]);
            off4 = _mm_set1_epi8((char)saoBlkParam->offset[4]);

            for (y = 0; y < lcu_pix_height; y++) {
                //diff = src[start_x] - src[start_x - 1];
                //leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                for (x = start_x; x < end_x; x += 16) {
                    s0 = _mm_loadu_si128((__m128i*)&src[x - 1]);
                    s1 = _mm_loadu_si128((__m128i*)&src[x]);
                    s2 = _mm_loadu_si128((__m128i*)&src[x + 1]);

                    t3 = _mm_min_epu8(s0, s1);
                    t1 = _mm_cmpeq_epi8(t3, s0);
                    t2 = _mm_cmpeq_epi8(t3, s1);
                    t0 = _mm_subs_epi8(t2, t1); //leftsign

                    t3 = _mm_min_epu8(s1, s2);
                    t1 = _mm_cmpeq_epi8(t3, s1);
                    t2 = _mm_cmpeq_epi8(t3, s2);
                    t3 = _mm_subs_epi8(t1, t2); //rightsign

                    etype = _mm_adds_epi8(t0, t3); //edgetype

                    t0 = _mm_cmpeq_epi8(etype, c0);
                    t1 = _mm_cmpeq_epi8(etype, c1);
                    t2 = _mm_cmpeq_epi8(etype, c2);
                    t3 = _mm_cmpeq_epi8(etype, c3);
                    t4 = _mm_cmpeq_epi8(etype, c4);

                    t0 = _mm_and_si128(t0, off0);
                    t1 = _mm_and_si128(t1, off1);
                    t2 = _mm_and_si128(t2, off2);
                    t3 = _mm_and_si128(t3, off3);
                    t4 = _mm_and_si128(t4, off4);

                    t0 = _mm_adds_epi8(t0, t1);
                    t2 = _mm_adds_epi8(t2, t3);
                    t0 = _mm_adds_epi8(t0, t4);
                    t0 = _mm_adds_epi8(t0, t2);//get offset

                    //add 8 nums once for possible overflow
                    t1 = _mm_cvtepi8_epi16(t0);
                    t0 = _mm_srli_si128(t0, 8);
                    t2 = _mm_cvtepi8_epi16(t0);
                    t3 = _mm_unpacklo_epi8(s1, zero);
                    t4 = _mm_unpackhi_epi8(s1, zero);

                    t1 = _mm_adds_epi16(t1, t3);
                    t2 = _mm_adds_epi16(t2, t4);
                    t0 = _mm_packus_epi16(t1, t2); //saturated

                    if (x != end_x_16){
                        _mm_storeu_si128((__m128i*)(dst + x), t0);
                    }
                    else{
                        _mm_maskmoveu_si128(t0, mask, (char_t*)(dst + x));
                        break;
                    }
                }
                dst += i_dst;
                src += i_src;
            }
        }
    }
        break;
    case SAO_TYPE_EO_90: {
        int end_x_16 = lcu_pix_width - 15;

        start_y = smb_available_up ? 0 : 1;
        end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);

        dst += start_y * i_dst;
        src += start_y * i_src;

        if (lcu_pix_width == 4){
            mask = _mm_set_epi32(0, 0, 0, -1);
            c0 = _mm_set1_epi16(-2);
            c1 = _mm_set1_epi16(-1);
            c2 = _mm_set1_epi16(0);
            c3 = _mm_set1_epi16(1);
            c4 = _mm_set1_epi16(2);

            off0 = _mm_set1_epi16((char)saoBlkParam->offset[0]);
            off1 = _mm_set1_epi16((char)saoBlkParam->offset[1]);
            off2 = _mm_set1_epi16((char)saoBlkParam->offset[2]);
            off3 = _mm_set1_epi16((char)saoBlkParam->offset[3]);
            off4 = _mm_set1_epi16((char)saoBlkParam->offset[4]);

            for (y = start_y; y < end_y; y++) {
                s0 = _mm_loadl_epi64((__m128i*)(src - i_src));
                s1 = _mm_loadl_epi64((__m128i*)src);
                s2 = _mm_loadl_epi64((__m128i*)(src + i_src));

                s0 = _mm_unpacklo_epi8(s0, zero);
                s1 = _mm_unpacklo_epi8(s1, zero);
                s2 = _mm_unpacklo_epi8(s2, zero);

                t3 = _mm_min_epu16(s0, s1);
                t1 = _mm_cmpeq_epi16(t3, s0);
                t2 = _mm_cmpeq_epi16(t3, s1);
                t0 = _mm_subs_epi16(t2, t1); //upsign

                t3 = _mm_min_epu16(s1, s2);
                t1 = _mm_cmpeq_epi16(t3, s1);
                t2 = _mm_cmpeq_epi16(t3, s2);
                t3 = _mm_subs_epi16(t1, t2); //downsign

                etype = _mm_adds_epi16(t0, t3); //edgetype

                t0 = _mm_cmpeq_epi16(etype, c0);
                t1 = _mm_cmpeq_epi16(etype, c1);
                t2 = _mm_cmpeq_epi16(etype, c2);
                t3 = _mm_cmpeq_epi16(etype, c3);
                t4 = _mm_cmpeq_epi16(etype, c4);

                t0 = _mm_and_si128(t0, off0);
                t1 = _mm_and_si128(t1, off1);
                t2 = _mm_and_si128(t2, off2);
                t3 = _mm_and_si128(t3, off3);
                t4 = _mm_and_si128(t4, off4);

                t0 = _mm_adds_epi8(t0, t1);
                t2 = _mm_adds_epi8(t2, t3);
                t0 = _mm_adds_epi8(t0, t4);
                t0 = _mm_adds_epi8(t0, t2);//get offset

                //add 8 nums once for possible overflow
                t1 = _mm_adds_epi16(t0, s1);
                t0 = _mm_packus_epi16(t1, zero); //saturated

                _mm_maskmoveu_si128(t0, mask, (char_t*)(dst));

                dst += i_dst;
                src += i_src;
            }
        }
        else{
            c0 = _mm_set1_epi8(-2);
            c1 = _mm_set1_epi8(-1);
            c2 = _mm_set1_epi8(0);
            c3 = _mm_set1_epi8(1);
            c4 = _mm_set1_epi8(2);

            off0 = _mm_set1_epi8((char)saoBlkParam->offset[0]);
            off1 = _mm_set1_epi8((char)saoBlkParam->offset[1]);
            off2 = _mm_set1_epi8((char)saoBlkParam->offset[2]);
            off3 = _mm_set1_epi8((char)saoBlkParam->offset[3]);
            off4 = _mm_set1_epi8((char)saoBlkParam->offset[4]);

            if (lcu_pix_width & 0x0f){
                mask = _mm_set_epi32(0, -1, -1, -1);

                for (y = start_y; y < end_y; y++) {
                    for (x = 0; x < lcu_pix_width; x += 16) {
                        s0 = _mm_loadu_si128((__m128i*)&src[x - i_src]);
                        s1 = _mm_loadu_si128((__m128i*)&src[x]);
                        s2 = _mm_loadu_si128((__m128i*)&src[x + i_src]);

                        t3 = _mm_min_epu8(s0, s1);
                        t1 = _mm_cmpeq_epi8(t3, s0);
                        t2 = _mm_cmpeq_epi8(t3, s1);
                        t0 = _mm_subs_epi8(t2, t1); //upsign

                        t3 = _mm_min_epu8(s1, s2);
                        t1 = _mm_cmpeq_epi8(t3, s1);
                        t2 = _mm_cmpeq_epi8(t3, s2);
                        t3 = _mm_subs_epi8(t1, t2); //downsign

                        etype = _mm_adds_epi8(t0, t3); //edgetype

                        t0 = _mm_cmpeq_epi8(etype, c0);
                        t1 = _mm_cmpeq_epi8(etype, c1);
                        t2 = _mm_cmpeq_epi8(etype, c2);
                        t3 = _mm_cmpeq_epi8(etype, c3);
                        t4 = _mm_cmpeq_epi8(etype, c4);

                        t0 = _mm_and_si128(t0, off0);
                        t1 = _mm_and_si128(t1, off1);
                        t2 = _mm_and_si128(t2, off2);
                        t3 = _mm_and_si128(t3, off3);
                        t4 = _mm_and_si128(t4, off4);

                        t0 = _mm_adds_epi8(t0, t1);
                        t2 = _mm_adds_epi8(t2, t3);
                        t0 = _mm_adds_epi8(t0, t4);
                        t0 = _mm_adds_epi8(t0, t2);//get offset

                        //add 8 nums once for possible overflow
                        t1 = _mm_cvtepi8_epi16(t0);
                        t0 = _mm_srli_si128(t0, 8);
                        t2 = _mm_cvtepi8_epi16(t0);
                        t3 = _mm_unpacklo_epi8(s1, zero);
                        t4 = _mm_unpackhi_epi8(s1, zero);

                        t1 = _mm_adds_epi16(t1, t3);
                        t2 = _mm_adds_epi16(t2, t4);
                        t0 = _mm_packus_epi16(t1, t2); //saturated

                        if (x < end_x_16){
                            _mm_storeu_si128((__m128i*)(dst + x), t0);
                        }
                        else{
                            _mm_maskmoveu_si128(t0, mask, (char_t*)(dst + x));
                            break;
                        }
                    }
                    dst += i_dst;
                    src += i_src;
                }
            }
            else{
                for (y = start_y; y < end_y; y++) {
                    for (x = 0; x < lcu_pix_width; x += 16) {
                        s0 = _mm_loadu_si128((__m128i*)&src[x - i_src]);
                        s1 = _mm_loadu_si128((__m128i*)&src[x]);
                        s2 = _mm_loadu_si128((__m128i*)&src[x + i_src]);

                        t3 = _mm_min_epu8(s0, s1);
                        t1 = _mm_cmpeq_epi8(t3, s0);
                        t2 = _mm_cmpeq_epi8(t3, s1);
                        t0 = _mm_subs_epi8(t2, t1); //upsign

                        t3 = _mm_min_epu8(s1, s2);
                        t1 = _mm_cmpeq_epi8(t3, s1);
                        t2 = _mm_cmpeq_epi8(t3, s2);
                        t3 = _mm_subs_epi8(t1, t2); //downsign

                        etype = _mm_adds_epi8(t0, t3); //edgetype

                        t0 = _mm_cmpeq_epi8(etype, c0);
                        t1 = _mm_cmpeq_epi8(etype, c1);
                        t2 = _mm_cmpeq_epi8(etype, c2);
                        t3 = _mm_cmpeq_epi8(etype, c3);
                        t4 = _mm_cmpeq_epi8(etype, c4);

                        t0 = _mm_and_si128(t0, off0);
                        t1 = _mm_and_si128(t1, off1);
                        t2 = _mm_and_si128(t2, off2);
                        t3 = _mm_and_si128(t3, off3);
                        t4 = _mm_and_si128(t4, off4);

                        t0 = _mm_adds_epi8(t0, t1);
                        t2 = _mm_adds_epi8(t2, t3);
                        t0 = _mm_adds_epi8(t0, t4);
                        t0 = _mm_adds_epi8(t0, t2);//get offset

                        //add 8 nums once for possible overflow
                        t1 = _mm_cvtepi8_epi16(t0);
                        t0 = _mm_srli_si128(t0, 8);
                        t2 = _mm_cvtepi8_epi16(t0);
                        t3 = _mm_unpacklo_epi8(s1, zero);
                        t4 = _mm_unpackhi_epi8(s1, zero);

                        t1 = _mm_adds_epi16(t1, t3);
                        t2 = _mm_adds_epi16(t2, t4);
                        t0 = _mm_packus_epi16(t1, t2); //saturated

                        _mm_storeu_si128((__m128i*)(dst + x), t0);
                    }
                    dst += i_dst;
                    src += i_src;
                }
            }
        }
    }
        break;
    case SAO_TYPE_EO_135: {
        __m128i mask_r0, mask_r, mask_rn;
        int end_x_r0_16, end_x_r_16, end_x_rn_16;

        c0 = _mm_set1_epi8(-2);
        c1 = _mm_set1_epi8(-1);
        c2 = _mm_set1_epi8(0);
        c3 = _mm_set1_epi8(1);
        c4 = _mm_set1_epi8(2);

        off0 = _mm_set1_epi8((char)saoBlkParam->offset[0]);
        off1 = _mm_set1_epi8((char)saoBlkParam->offset[1]);
        off2 = _mm_set1_epi8((char)saoBlkParam->offset[2]);
        off3 = _mm_set1_epi8((char)saoBlkParam->offset[3]);
        off4 = _mm_set1_epi8((char)saoBlkParam->offset[4]);

        start_x_r0 = smb_available_upleft ? 0 : 1;
        end_x_r0 = smb_available_up ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;
        start_x_r = smb_available_left ? 0 : 1;
        end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
        start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
        end_x_rn = smb_available_rightdwon ? lcu_pix_width : (lcu_pix_width - 1);

        end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f);
        end_x_r_16 = end_x_r - ((end_x_r - start_x_r) & 0x0f);
        end_x_rn_16 = end_x_rn - ((end_x_rn - start_x_rn) & 0x0f);


        //first row
        for (x = start_x_r0; x < end_x_r0; x += 16) {
            s0 = _mm_loadu_si128((__m128i*)&src[x - i_src - 1]);
            s1 = _mm_loadu_si128((__m128i*)&src[x]);
            s2 = _mm_loadu_si128((__m128i*)&src[x + i_src + 1]);

            t3 = _mm_min_epu8(s0, s1);
            t1 = _mm_cmpeq_epi8(t3, s0);
            t2 = _mm_cmpeq_epi8(t3, s1);
            t0 = _mm_subs_epi8(t2, t1); //upsign

            t3 = _mm_min_epu8(s1, s2);
            t1 = _mm_cmpeq_epi8(t3, s1);
            t2 = _mm_cmpeq_epi8(t3, s2);
            t3 = _mm_subs_epi8(t1, t2); //downsign

            etype = _mm_adds_epi8(t0, t3); //edgetype

            t0 = _mm_cmpeq_epi8(etype, c0);
            t1 = _mm_cmpeq_epi8(etype, c1);
            t2 = _mm_cmpeq_epi8(etype, c2);
            t3 = _mm_cmpeq_epi8(etype, c3);
            t4 = _mm_cmpeq_epi8(etype, c4);

            t0 = _mm_and_si128(t0, off0);
            t1 = _mm_and_si128(t1, off1);
            t2 = _mm_and_si128(t2, off2);
            t3 = _mm_and_si128(t3, off3);
            t4 = _mm_and_si128(t4, off4);

            t0 = _mm_adds_epi8(t0, t1);
            t2 = _mm_adds_epi8(t2, t3);
            t0 = _mm_adds_epi8(t0, t4);
            t0 = _mm_adds_epi8(t0, t2);//get offset

            //add 8 nums once for possible overflow
            t1 = _mm_cvtepi8_epi16(t0);
            t0 = _mm_srli_si128(t0, 8);
            t2 = _mm_cvtepi8_epi16(t0);
            t3 = _mm_unpacklo_epi8(s1, zero);
            t4 = _mm_unpackhi_epi8(s1, zero);

            t1 = _mm_adds_epi16(t1, t3);
            t2 = _mm_adds_epi16(t2, t4);
            t0 = _mm_packus_epi16(t1, t2); //saturated

            if (x != end_x_r0_16){
                _mm_storeu_si128((__m128i*)(dst + x), t0);
            }
            else{
                mask_r0 = _mm_load_si128((__m128i*)(intrinsic_mask[end_x_r0 - end_x_r0_16 - 1]));
                _mm_maskmoveu_si128(t0, mask_r0, (char_t*)(dst + x));
                break;
            }
        }
        dst += i_dst;
        src += i_src;

        mask_r = _mm_load_si128((__m128i*)(intrinsic_mask[end_x_r - end_x_r_16 - 1]));
        //middle rows
        for (y = 1; y < lcu_pix_height - 1; y++) {
            for (x = start_x_r; x < end_x_r; x += 16) {
                s0 = _mm_loadu_si128((__m128i*)&src[x - i_src - 1]);
                s1 = _mm_loadu_si128((__m128i*)&src[x]);
                s2 = _mm_loadu_si128((__m128i*)&src[x + i_src + 1]);

                t3 = _mm_min_epu8(s0, s1);
                t1 = _mm_cmpeq_epi8(t3, s0);
                t2 = _mm_cmpeq_epi8(t3, s1);
                t0 = _mm_subs_epi8(t2, t1); //upsign

                t3 = _mm_min_epu8(s1, s2);
                t1 = _mm_cmpeq_epi8(t3, s1);
                t2 = _mm_cmpeq_epi8(t3, s2);
                t3 = _mm_subs_epi8(t1, t2); //downsign

                etype = _mm_adds_epi8(t0, t3); //edgetype

                t0 = _mm_cmpeq_epi8(etype, c0);
                t1 = _mm_cmpeq_epi8(etype, c1);
                t2 = _mm_cmpeq_epi8(etype, c2);
                t3 = _mm_cmpeq_epi8(etype, c3);
                t4 = _mm_cmpeq_epi8(etype, c4);

                t0 = _mm_and_si128(t0, off0);
                t1 = _mm_and_si128(t1, off1);
                t2 = _mm_and_si128(t2, off2);
                t3 = _mm_and_si128(t3, off3);
                t4 = _mm_and_si128(t4, off4);

                t0 = _mm_adds_epi8(t0, t1);
                t2 = _mm_adds_epi8(t2, t3);
                t0 = _mm_adds_epi8(t0, t4);
                t0 = _mm_adds_epi8(t0, t2);//get offset

                //add 8 nums once for possible overflow
                t1 = _mm_cvtepi8_epi16(t0);
                t0 = _mm_srli_si128(t0, 8);
                t2 = _mm_cvtepi8_epi16(t0);
                t3 = _mm_unpacklo_epi8(s1, zero);
                t4 = _mm_unpackhi_epi8(s1, zero);

                t1 = _mm_adds_epi16(t1, t3);
                t2 = _mm_adds_epi16(t2, t4);
                t0 = _mm_packus_epi16(t1, t2); //saturated

                if (x != end_x_r_16){
                    _mm_storeu_si128((__m128i*)(dst + x), t0);
                }
                else{
                    _mm_maskmoveu_si128(t0, mask_r, (char_t*)(dst + x));
                    break;
                }
            }
            dst += i_dst;
            src += i_src;
        }
        //last row
        for (x = start_x_rn; x < end_x_rn; x += 16) {
            s0 = _mm_loadu_si128((__m128i*)&src[x - i_src - 1]);
            s1 = _mm_loadu_si128((__m128i*)&src[x]);
            s2 = _mm_loadu_si128((__m128i*)&src[x + i_src + 1]);

            t3 = _mm_min_epu8(s0, s1);
            t1 = _mm_cmpeq_epi8(t3, s0);
            t2 = _mm_cmpeq_epi8(t3, s1);
            t0 = _mm_subs_epi8(t2, t1); //upsign

            t3 = _mm_min_epu8(s1, s2);
            t1 = _mm_cmpeq_epi8(t3, s1);
            t2 = _mm_cmpeq_epi8(t3, s2);
            t3 = _mm_subs_epi8(t1, t2); //downsign

            etype = _mm_adds_epi8(t0, t3); //edgetype

            t0 = _mm_cmpeq_epi8(etype, c0);
            t1 = _mm_cmpeq_epi8(etype, c1);
            t2 = _mm_cmpeq_epi8(etype, c2);
            t3 = _mm_cmpeq_epi8(etype, c3);
            t4 = _mm_cmpeq_epi8(etype, c4);

            t0 = _mm_and_si128(t0, off0);
            t1 = _mm_and_si128(t1, off1);
            t2 = _mm_and_si128(t2, off2);
            t3 = _mm_and_si128(t3, off3);
            t4 = _mm_and_si128(t4, off4);

            t0 = _mm_adds_epi8(t0, t1);
            t2 = _mm_adds_epi8(t2, t3);
            t0 = _mm_adds_epi8(t0, t4);
            t0 = _mm_adds_epi8(t0, t2);//get offset

            //add 8 nums once for possible overflow
            t1 = _mm_cvtepi8_epi16(t0);
            t0 = _mm_srli_si128(t0, 8);
            t2 = _mm_cvtepi8_epi16(t0);
            t3 = _mm_unpacklo_epi8(s1, zero);
            t4 = _mm_unpackhi_epi8(s1, zero);

            t1 = _mm_adds_epi16(t1, t3);
            t2 = _mm_adds_epi16(t2, t4);
            t0 = _mm_packus_epi16(t1, t2); //saturated

            if (x != end_x_rn_16){
                _mm_storeu_si128((__m128i*)(dst + x), t0);
            }
            else{
                mask_rn = _mm_load_si128((__m128i*)(intrinsic_mask[end_x_rn - end_x_rn_16 - 1]));
                _mm_maskmoveu_si128(t0, mask_rn, (char_t*)(dst + x));
                break;
            }
        }
    }
        break;
    case SAO_TYPE_EO_45: {
        __m128i mask_r0, mask_r, mask_rn;
        int end_x_r0_16, end_x_r_16, end_x_rn_16;

        c0 = _mm_set1_epi8(-2);
        c1 = _mm_set1_epi8(-1);
        c2 = _mm_set1_epi8(0);
        c3 = _mm_set1_epi8(1);
        c4 = _mm_set1_epi8(2);

        off0 = _mm_set1_epi8((char)saoBlkParam->offset[0]);
        off1 = _mm_set1_epi8((char)saoBlkParam->offset[1]);
        off2 = _mm_set1_epi8((char)saoBlkParam->offset[2]);
        off3 = _mm_set1_epi8((char)saoBlkParam->offset[3]);
        off4 = _mm_set1_epi8((char)saoBlkParam->offset[4]);

        start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
        end_x_r0 = smb_available_upright ? lcu_pix_width : (lcu_pix_width - 1);
        start_x_r = smb_available_left ? 0 : 1;
        end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
        start_x_rn = smb_available_leftdown ? 0 : 1;
        end_x_rn = smb_available_down ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;

        end_x_r0_16 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x0f);
        end_x_r_16 = end_x_r - ((end_x_r - start_x_r) & 0x0f);
        end_x_rn_16 = end_x_rn - ((end_x_rn - start_x_rn) & 0x0f);


        //first row
        for (x = start_x_r0; x < end_x_r0; x += 16) {
            s0 = _mm_loadu_si128((__m128i*)&src[x - i_src + 1]);
            s1 = _mm_loadu_si128((__m128i*)&src[x]);
            s2 = _mm_loadu_si128((__m128i*)&src[x + i_src - 1]);

            t3 = _mm_min_epu8(s0, s1);
            t1 = _mm_cmpeq_epi8(t3, s0);
            t2 = _mm_cmpeq_epi8(t3, s1);
            t0 = _mm_subs_epi8(t2, t1); //upsign

            t3 = _mm_min_epu8(s1, s2);
            t1 = _mm_cmpeq_epi8(t3, s1);
            t2 = _mm_cmpeq_epi8(t3, s2);
            t3 = _mm_subs_epi8(t1, t2); //downsign

            etype = _mm_adds_epi8(t0, t3); //edgetype

            t0 = _mm_cmpeq_epi8(etype, c0);
            t1 = _mm_cmpeq_epi8(etype, c1);
            t2 = _mm_cmpeq_epi8(etype, c2);
            t3 = _mm_cmpeq_epi8(etype, c3);
            t4 = _mm_cmpeq_epi8(etype, c4);

            t0 = _mm_and_si128(t0, off0);
            t1 = _mm_and_si128(t1, off1);
            t2 = _mm_and_si128(t2, off2);
            t3 = _mm_and_si128(t3, off3);
            t4 = _mm_and_si128(t4, off4);

            t0 = _mm_adds_epi8(t0, t1);
            t2 = _mm_adds_epi8(t2, t3);
            t0 = _mm_adds_epi8(t0, t4);
            t0 = _mm_adds_epi8(t0, t2);//get offset

            //add 8 nums once for possible overflow
            t1 = _mm_cvtepi8_epi16(t0);
            t0 = _mm_srli_si128(t0, 8);
            t2 = _mm_cvtepi8_epi16(t0);
            t3 = _mm_unpacklo_epi8(s1, zero);
            t4 = _mm_unpackhi_epi8(s1, zero);

            t1 = _mm_adds_epi16(t1, t3);
            t2 = _mm_adds_epi16(t2, t4);
            t0 = _mm_packus_epi16(t1, t2); //saturated

            if (x != end_x_r0_16){
                _mm_storeu_si128((__m128i*)(dst + x), t0);
            }
            else{
                mask_r0 = _mm_load_si128((__m128i*)(intrinsic_mask[end_x_r0 - end_x_r0_16 - 1]));
                _mm_maskmoveu_si128(t0, mask_r0, (char_t*)(dst + x));
                break;
            }
        }
        dst += i_dst;
        src += i_src;

        mask_r = _mm_load_si128((__m128i*)(intrinsic_mask[end_x_r - end_x_r_16 - 1]));
        //middle rows
        for (y = 1; y < lcu_pix_height - 1; y++) {
            for (x = start_x_r; x < end_x_r; x += 16) {
                s0 = _mm_loadu_si128((__m128i*)&src[x - i_src + 1]);
                s1 = _mm_loadu_si128((__m128i*)&src[x]);
                s2 = _mm_loadu_si128((__m128i*)&src[x + i_src - 1]);

                t3 = _mm_min_epu8(s0, s1);
                t1 = _mm_cmpeq_epi8(t3, s0);
                t2 = _mm_cmpeq_epi8(t3, s1);
                t0 = _mm_subs_epi8(t2, t1); //upsign

                t3 = _mm_min_epu8(s1, s2);
                t1 = _mm_cmpeq_epi8(t3, s1);
                t2 = _mm_cmpeq_epi8(t3, s2);
                t3 = _mm_subs_epi8(t1, t2); //downsign

                etype = _mm_adds_epi8(t0, t3); //edgetype

                t0 = _mm_cmpeq_epi8(etype, c0);
                t1 = _mm_cmpeq_epi8(etype, c1);
                t2 = _mm_cmpeq_epi8(etype, c2);
                t3 = _mm_cmpeq_epi8(etype, c3);
                t4 = _mm_cmpeq_epi8(etype, c4);

                t0 = _mm_and_si128(t0, off0);
                t1 = _mm_and_si128(t1, off1);
                t2 = _mm_and_si128(t2, off2);
                t3 = _mm_and_si128(t3, off3);
                t4 = _mm_and_si128(t4, off4);

                t0 = _mm_adds_epi8(t0, t1);
                t2 = _mm_adds_epi8(t2, t3);
                t0 = _mm_adds_epi8(t0, t4);
                t0 = _mm_adds_epi8(t0, t2);//get offset

                //add 8 nums once for possible overflow
                t1 = _mm_cvtepi8_epi16(t0);
                t0 = _mm_srli_si128(t0, 8);
                t2 = _mm_cvtepi8_epi16(t0);
                t3 = _mm_unpacklo_epi8(s1, zero);
                t4 = _mm_unpackhi_epi8(s1, zero);

                t1 = _mm_adds_epi16(t1, t3);
                t2 = _mm_adds_epi16(t2, t4);
                t0 = _mm_packus_epi16(t1, t2); //saturated

                if (x != end_x_r_16){
                    _mm_storeu_si128((__m128i*)(dst + x), t0);
                }
                else{
                    _mm_maskmoveu_si128(t0, mask_r, (char_t*)(dst + x));
                    break;
                }
            }
            dst += i_dst;
            src += i_src;
        }
        for (x = start_x_rn; x < end_x_rn; x += 16) {
            s0 = _mm_loadu_si128((__m128i*)&src[x - i_src + 1]);
            s1 = _mm_loadu_si128((__m128i*)&src[x]);
            s2 = _mm_loadu_si128((__m128i*)&src[x + i_src - 1]);

            t3 = _mm_min_epu8(s0, s1);
            t1 = _mm_cmpeq_epi8(t3, s0);
            t2 = _mm_cmpeq_epi8(t3, s1);
            t0 = _mm_subs_epi8(t2, t1); //upsign

            t3 = _mm_min_epu8(s1, s2);
            t1 = _mm_cmpeq_epi8(t3, s1);
            t2 = _mm_cmpeq_epi8(t3, s2);
            t3 = _mm_subs_epi8(t1, t2); //downsign

            etype = _mm_adds_epi8(t0, t3); //edgetype

            t0 = _mm_cmpeq_epi8(etype, c0);
            t1 = _mm_cmpeq_epi8(etype, c1);
            t2 = _mm_cmpeq_epi8(etype, c2);
            t3 = _mm_cmpeq_epi8(etype, c3);
            t4 = _mm_cmpeq_epi8(etype, c4);

            t0 = _mm_and_si128(t0, off0);
            t1 = _mm_and_si128(t1, off1);
            t2 = _mm_and_si128(t2, off2);
            t3 = _mm_and_si128(t3, off3);
            t4 = _mm_and_si128(t4, off4);

            t0 = _mm_adds_epi8(t0, t1);
            t2 = _mm_adds_epi8(t2, t3);
            t0 = _mm_adds_epi8(t0, t4);
            t0 = _mm_adds_epi8(t0, t2);//get offset

            //add 8 nums once for possible overflow
            t1 = _mm_cvtepi8_epi16(t0);
            t0 = _mm_srli_si128(t0, 8);
            t2 = _mm_cvtepi8_epi16(t0);
            t3 = _mm_unpacklo_epi8(s1, zero);
            t4 = _mm_unpackhi_epi8(s1, zero);

            t1 = _mm_adds_epi16(t1, t3);
            t2 = _mm_adds_epi16(t2, t4);
            t0 = _mm_packus_epi16(t1, t2); //saturated

            if (x != end_x_rn_16){
                _mm_storeu_si128((__m128i*)(dst + x), t0);
            }
            else{
                mask_rn = _mm_load_si128((__m128i*)(intrinsic_mask[end_x_rn - end_x_rn_16 - 1]));
                _mm_maskmoveu_si128(t0, mask_rn, (char_t*)(dst + x));
                break;
            }
        }
    }
        break;
    default: {
        printf("Not a supported SAO types\n");
        assert(0);
        exit(-1);
    }
    }
}

void sao_get_stats_sse128(void *handle, void* sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height,
    int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down)
{
    int type;
    int start_x, end_x, start_y, end_y;
    int start_x_r0, end_x_r0, start_x_r, end_x_r, start_x_rn, end_x_rn;
    int x, y;
    SAOStatData *saostatsData = (SAOStatData *)sao_data;
    SAOStatData *statsDate;
    avs3_enc_t *h = (avs3_enc_t *)handle;
    ALIGNED_16(i16s_t diffs[64 + 16]);
    ALIGNED_16(char_t etypes[64 + 16]);
    int smb_available_upleft = (smb_available_up && smb_available_left);
    int smb_available_upright = (smb_available_up && smb_available_right);
    int smb_available_leftdown = (smb_available_down && smb_available_left);
    int smb_available_rightdwon = (smb_available_down && smb_available_right);

    pel_t *rec, *org;
    int i_rec, i_org;
    __m128i zero = _mm_setzero_si128();
    __m128i cst2 = _mm_set1_epi8(2);
    __m128i s0, s1, s2, s3;
    __m128i t0, t1, t2, t3, etype;
    int i;

    i_rec = h->img_rec->i_stride[compIdx];
    i_org = h->img_org->i_stride[compIdx];
    if (lcu_pix_width == 4){
        for (type = 0; type < NUM_SAO_NEW_TYPES; type++) {
            rec = h->img_rec->plane[compIdx] + pix_y * i_rec + pix_x;
            org = h->img_org->plane[compIdx] + pix_y * i_org + pix_x;
     
            statsDate = &(saostatsData[type]);
            switch (type) {
            case SAO_TYPE_EO_0:
                start_x = smb_available_left ? 0 : 1;
                end_x = smb_available_right ? 4 : (4 - 1);
                //end_x_16 = end_x - ((end_x - start_x) & 0x0f);
              
                for (y = 0; y < lcu_pix_height; y++) {
                    //diff = src[start_x] - src[start_x - 1];
                    //leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                    s0 = _mm_loadl_epi64((__m128i*)(rec - 1));
                    s1 = _mm_srli_si128(s0, 1);
                    s2 = _mm_srli_si128(s0, 2);
                    s3 = _mm_loadl_epi64((__m128i*)(org));
     
                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)
     
                    _mm_storel_epi64((__m128i*)(etypes), etype);
     
                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
     
                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                    _mm_storel_epi64((__m128i*)(diffs), s0);
     
                    for (x = start_x; x < end_x; x++) {
                        i = etypes[x];
                        statsDate->diff[i] += diffs[x];
                        statsDate->count[i]++;
                    }
                    org += i_org;
                    rec += i_rec;
                }
                break;
            case SAO_TYPE_EO_90:
                start_y = smb_available_up ? 0 : 1;
                end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);
                rec += start_y*i_rec;
                org += start_y*i_org;
                for (y = start_y; y < end_y; y++) {
                    int i0, i1, i2, i3;
                    s0 = _mm_loadl_epi64((__m128i*)(rec - i_rec));
                    s1 = _mm_loadl_epi64((__m128i*)rec);
                    s2 = _mm_loadl_epi64((__m128i*)(rec + i_rec));
                    s3 = _mm_loadl_epi64((__m128i*)org);
     
                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype);
                    i0 = (char_t)_mm_extract_epi8(etype, 0);
                    i1 = (char_t)_mm_extract_epi8(etype, 1);
                    i2 = (char_t)_mm_extract_epi8(etype, 2);
                    i3 = (char_t)_mm_extract_epi8(etype, 3);
     
                    statsDate->count[i0]++;
                    statsDate->count[i1]++;
                    statsDate->count[i2]++;
                    statsDate->count[i3]++;

                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                       
                    s0 = _mm_cvtepi16_epi32(s0);
                    statsDate->diff[i0] += _mm_extract_epi32(s0, 0);
                    statsDate->diff[i1] += _mm_extract_epi32(s0, 1);
                    statsDate->diff[i2] += _mm_extract_epi32(s0, 2);
                    statsDate->diff[i3] += _mm_extract_epi32(s0, 3);
                    
                    org += i_org;
                    rec += i_rec;
                }
     
                break;
            case SAO_TYPE_EO_135:
                start_x_r0 = smb_available_upleft ? 0 : 1;
                end_x_r0 = smb_available_up ? (smb_available_right ? 4 : (4 - 1)) : 1;
                start_x_r = smb_available_left ? 0 : 1;
                end_x_r = smb_available_right ? 4 : (4 - 1);
                start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (4 - 1);
                end_x_rn = smb_available_rightdwon ? 4 : (4 - 1);
     
                s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 - i_rec - 1]);
                s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0]);
                s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 + i_rec + 1]);
                s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r0));
     
                CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)
     
                _mm_storel_epi64((__m128i*)(etypes), etype);
     
                s0 = _mm_unpacklo_epi8(s1, zero);
                s2 = _mm_unpacklo_epi8(s3, zero);
     
                s0 = _mm_subs_epi16(s2, s0);    //org - rec
                _mm_storel_epi64((__m128i*)(diffs), s0);
                     
                for (x = 0; x < end_x_r0 - start_x_r0; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                org += i_org;
                rec += i_rec;
     
                //middle rows
                for (y = 1; y < lcu_pix_height - 1; y++) {
                    s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r - i_rec - 1]);
                    s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r]);
                    s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r + i_rec + 1]);
                    s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r));
     
                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)
     
                    _mm_storel_epi64((__m128i*)(etypes), etype);
     
                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
     
                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                    _mm_storel_epi64((__m128i*)(diffs), s0);
                         
                    for (x = 0; x < end_x_r - start_x_r; x++) {
                        i = etypes[x];
                        statsDate->diff[i] += diffs[x];
                        statsDate->count[i]++;
                    }
                    org += i_org;
                    rec += i_rec;
                }
                //last row
                s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn - i_rec - 1]);
                s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn]);
                s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn + i_rec + 1]);
                s3 = _mm_loadl_epi64((__m128i*)(org + start_x_rn));
     
                CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)
     
                _mm_storel_epi64((__m128i*)(etypes), etype);
     
                s0 = _mm_unpacklo_epi8(s1, zero);
                s2 = _mm_unpacklo_epi8(s3, zero);
     
                s0 = _mm_subs_epi16(s2, s0);    //org - rec
                _mm_storel_epi64((__m128i*)(diffs), s0);
                     
                for (x = 0; x < end_x_rn - start_x_rn; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                break;
            case SAO_TYPE_EO_45:
                start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (4 - 1);
                end_x_r0 = smb_available_upright ? 4 : (4 - 1);
                start_x_r = smb_available_left ? 0 : 1;
                end_x_r = smb_available_right ? 4 : (4 - 1);
                start_x_rn = smb_available_leftdown ? 0 : 1;
                end_x_rn = smb_available_down ? (smb_available_right ? 4 : (4 - 1)) : 1;
     
                s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 - i_rec + 1]);
                s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0]);
                s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 + i_rec - 1]);
                s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r0));
     
                CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)
     
                _mm_storel_epi64((__m128i*)(etypes), etype);
     
                s0 = _mm_unpacklo_epi8(s1, zero);
                s2 = _mm_unpacklo_epi8(s3, zero);
     
                s0 = _mm_subs_epi16(s2, s0);    //org - rec
                _mm_storel_epi64((__m128i*)(diffs), s0);
                     
                for (x = 0; x < end_x_r0 - start_x_r0; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                org += i_org;
                rec += i_rec;
     
                //middle rows
                for (y = 1; y < lcu_pix_height - 1; y++) {
                    s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r - i_rec + 1]);
                    s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r]);
                    s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r + i_rec - 1]);
                    s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r));
     
                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)
     
                    _mm_storel_epi64((__m128i*)(etypes), etype);
     
                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
     
                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                    s1 = _mm_subs_epi16(s3, s1);
                    _mm_storel_epi64((__m128i*)(diffs), s0);
                         
                    for (x = 0; x < end_x_r - start_x_r; x++) {
                        i = etypes[x];
                        statsDate->diff[i] += diffs[x];
                        statsDate->count[i]++;
                    }
                    org += i_org;
                    rec += i_rec;
                }
                s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn - i_rec + 1]);
                s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn]);
                s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn + i_rec - 1]);
                s3 = _mm_loadl_epi64((__m128i*)(org + start_x_rn));
     
                CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)
     
                _mm_storel_epi64((__m128i*)(etypes), etype);
     
                s0 = _mm_unpacklo_epi8(s1, zero);
                s2 = _mm_unpacklo_epi8(s3, zero);
     
                s0 = _mm_subs_epi16(s2, s0);    //org - rec
                _mm_storel_epi64((__m128i*)(diffs), s0);
                     
                for (x = 0; x < end_x_rn - start_x_rn; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                break;
            default:
                printf("Not a supported SAO types\n");
                assert(0);
                exit(-1);
     
            }
        }
    }
    else{
        for (type = 0; type < NUM_SAO_NEW_TYPES; type++) {
            rec = h->img_rec->plane[compIdx] + pix_y * i_rec + pix_x;
            org = h->img_org->plane[compIdx] + pix_y * i_org + pix_x;

            statsDate = &(saostatsData[type]);
            switch (type) {
            case SAO_TYPE_EO_0:
                start_x = smb_available_left ? 0 : 1;
                end_x = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
                //end_x_16 = end_x - ((end_x - start_x) & 0x0f);
                if (!((end_x - start_x) & 0x0f)){
                    __m128i t4, t5, t6;
                    __m128i diff0, diff1, diff2, diff3, diff4;
                    __m128i cn0, cn1, cn2, cn3, cn4;
                    __m128i c0, c1, c2, c3, c4;
                    __m128i d03;
                    int d4 = 0;

                    d03 = zero;
                    c0 = _mm_set1_epi16(-2);
                    c1 = _mm_set1_epi16(-1);
                    c2 = _mm_set1_epi16(0);
                    c3 = _mm_set1_epi16(1);
                    c4 = _mm_set1_epi16(2);
                    cn0 = cn1 = cn2 = cn3 = cn4 = zero;

                    for (y = 0; y < lcu_pix_height; y++) {
                        //diff = src[start_x] - src[start_x - 1];
                        //leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
                        diff0 = zero;
                        diff1 = zero;
                        diff2 = zero;
                        diff3 = zero;
                        diff4 = zero;

                        for (x = start_x; x < end_x; x += 16) {
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - 1]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + 1]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16(s0, s1, s2, t0, t1, t2, t3, etype);

                            t5 = _mm_cvtepi8_epi16(etype); //low 8
                            t6 = _mm_cvtepi8_epi16(_mm_srli_si128(etype, 8));

                            t0 = _mm_cmpeq_epi16(t5, c0);
                            t1 = _mm_cmpeq_epi16(t5, c1);
                            t2 = _mm_cmpeq_epi16(t5, c2);
                            t3 = _mm_cmpeq_epi16(t5, c3);
                            t4 = _mm_cmpeq_epi16(t5, c4);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

                            t0 = _mm_cmpeq_epi16(t6, c0);
                            t1 = _mm_cmpeq_epi16(t6, c1);
                            t2 = _mm_cmpeq_epi16(t6, c2);
                            t3 = _mm_cmpeq_epi16(t6, c3);
                            t4 = _mm_cmpeq_epi16(t6, c4);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
                        }
                        diff0 = _mm_hadds_epi16(diff0, diff1);
                        diff2 = _mm_hadds_epi16(diff2, diff3);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff2);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff4);

                        d03 = _mm_add_epi32(d03, _mm_cvtepi16_epi32(diff0));
                        d4 += _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8)), 0);

                        org += i_org;
                        rec += i_rec;
                    }

                    t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
                    _mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
                    statsDate->diff[4] += d4;

                    cn0 = _mm_hadds_epi16(cn0, cn1);
                    cn2 = _mm_hadds_epi16(cn2, cn3);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn2);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn4);

                    cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
                    cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
                    _mm_storeu_si128((__m128i*)statsDate->count, cn1);
                    statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
                }
                else{
                    for (y = 0; y < lcu_pix_height; y++) {
                        //diff = src[start_x] - src[start_x - 1];
                        //leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);

                        for (x = start_x; x < end_x; x += 16) {
                            i = x - start_x;
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - 1]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + 1]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype);

                            _mm_store_si128((__m128i*)(etypes + i), etype);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);
                            _mm_store_si128((__m128i*)(diffs + i), s0);
                            _mm_store_si128((__m128i*)(diffs + i + 8), s1);
                        }

                        for (x = 0; x < end_x - start_x; x++) {
                            i = etypes[x];
                            statsDate->diff[i] += diffs[x];
                            statsDate->count[i]++;
                        }
                        org += i_org;
                        rec += i_rec;
                    }

                }
                break;
            case SAO_TYPE_EO_90:
                start_y = smb_available_up ? 0 : 1;
                end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);
                rec += start_y*i_rec;
                org += start_y*i_org;
                if (!(lcu_pix_width & 0x0f)){
                    __m128i t4, t5, t6;
                    __m128i diff0, diff1, diff2, diff3, diff4;
                    __m128i cn0, cn1, cn2, cn3, cn4;
                    __m128i c0, c1, c2, c3, c4;
                    __m128i d03;
                    int d4 = 0;

                    d03 = zero;
                    c0 = _mm_set1_epi16(-2);
                    c1 = _mm_set1_epi16(-1);
                    c2 = _mm_set1_epi16(0);
                    c3 = _mm_set1_epi16(1);
                    c4 = _mm_set1_epi16(2);
                    cn0 = cn1 = cn2 = cn3 = cn4 = zero;

                    for (y = start_y; y < end_y; y++) {
                        diff0 = zero;
                        diff1 = zero;
                        diff2 = zero;
                        diff3 = zero;
                        diff4 = zero;
                        for (x = 0; x < lcu_pix_width; x += 16) {
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16(s0, s1, s2, t0, t1, t2, t3, etype)

                            t5 = _mm_cvtepi8_epi16(etype); //low 8
                            t6 = _mm_cvtepi8_epi16(_mm_srli_si128(etype, 8));

                            t0 = _mm_cmpeq_epi16(t5, c0);
                            t1 = _mm_cmpeq_epi16(t5, c1);
                            t2 = _mm_cmpeq_epi16(t5, c2);
                            t3 = _mm_cmpeq_epi16(t5, c3);
                            t4 = _mm_cmpeq_epi16(t5, c4);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

                            t0 = _mm_cmpeq_epi16(t6, c0);
                            t1 = _mm_cmpeq_epi16(t6, c1);
                            t2 = _mm_cmpeq_epi16(t6, c2);
                            t3 = _mm_cmpeq_epi16(t6, c3);
                            t4 = _mm_cmpeq_epi16(t6, c4);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
                        }
                        diff0 = _mm_hadds_epi16(diff0, diff1);
                        diff2 = _mm_hadds_epi16(diff2, diff3);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff2);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff4);

                        d03 = _mm_add_epi32(d03, _mm_cvtepi16_epi32(diff0));
                        d4 += _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8)), 0);

                        org += i_org;
                        rec += i_rec;
                    }
                    t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
                    _mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
                    statsDate->diff[4] += d4;

                    cn0 = _mm_hadds_epi16(cn0, cn1);
                    cn2 = _mm_hadds_epi16(cn2, cn3);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn2);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn4);

                    cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
                    cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
                    _mm_storeu_si128((__m128i*)statsDate->count, cn1);
                    statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
                }
                else{
                    for (y = start_y; y < end_y; y++) {
                        for (x = 0; x < lcu_pix_width; x += 16) {
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

                                _mm_store_si128((__m128i*)(etypes + x), etype);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);
                            _mm_store_si128((__m128i*)(diffs + x), s0);
                            _mm_store_si128((__m128i*)(diffs + x + 8), s1);
                        }

                        for (x = 0; x < lcu_pix_width; x++) {
                            i = etypes[x];
                            statsDate->diff[i] += diffs[x];
                            statsDate->count[i]++;
                        }

                        org += i_org;
                        rec += i_rec;
                    }
                }
                break;
            case SAO_TYPE_EO_135:
                start_x_r0 = smb_available_upleft ? 0 : 1;
                end_x_r0 = smb_available_up ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;
                start_x_r = smb_available_left ? 0 : 1;
                end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
                start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
                end_x_rn = smb_available_rightdwon ? lcu_pix_width : (lcu_pix_width - 1);

                for (x = start_x_r0; x < end_x_r0; x += 16) {
                    i = x - start_x_r0;
                    s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
                    s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                    s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
                    s3 = _mm_loadu_si128((__m128i*)(org + x));

                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

                        _mm_store_si128((__m128i*)(etypes + i), etype);

                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s1 = _mm_unpackhi_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
                    s3 = _mm_unpackhi_epi8(s3, zero);

                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                    s1 = _mm_subs_epi16(s3, s1);
                    _mm_store_si128((__m128i*)(diffs + i), s0);
                    _mm_store_si128((__m128i*)(diffs + i + 8), s1);
                }

                for (x = 0; x < end_x_r0 - start_x_r0; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                org += i_org;
                rec += i_rec;

                //middle rows
                if (!((end_x_r - start_x_r)&0x0f)){
                    __m128i t4, t5, t6;
                    __m128i diff0, diff1, diff2, diff3, diff4;
                    __m128i cn0, cn1, cn2, cn3, cn4;
                    __m128i c0, c1, c2, c3, c4;
                    __m128i d03;
                    int d4 = 0;

                    d03 = zero;
                    c0 = _mm_set1_epi16(-2);
                    c1 = _mm_set1_epi16(-1);
                    c2 = _mm_set1_epi16(0);
                    c3 = _mm_set1_epi16(1);
                    c4 = _mm_set1_epi16(2);
                    cn0 = cn1 = cn2 = cn3 = cn4 = zero;

                    for (y = 1; y < lcu_pix_height - 1; y++) {
                        diff0 = zero;
                        diff1 = zero;
                        diff2 = zero;
                        diff3 = zero;
                        diff4 = zero;
                        for (x = start_x_r; x < end_x_r; x += 16) {
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16(s0, s1, s2, t0, t1, t2, t3, etype)

                            t5 = _mm_cvtepi8_epi16(etype); //low 8
                            t6 = _mm_cvtepi8_epi16(_mm_srli_si128(etype, 8));

                            t0 = _mm_cmpeq_epi16(t5, c0);
                            t1 = _mm_cmpeq_epi16(t5, c1);
                            t2 = _mm_cmpeq_epi16(t5, c2);
                            t3 = _mm_cmpeq_epi16(t5, c3);
                            t4 = _mm_cmpeq_epi16(t5, c4);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

                            t0 = _mm_cmpeq_epi16(t6, c0);
                            t1 = _mm_cmpeq_epi16(t6, c1);
                            t2 = _mm_cmpeq_epi16(t6, c2);
                            t3 = _mm_cmpeq_epi16(t6, c3);
                            t4 = _mm_cmpeq_epi16(t6, c4);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
                        }
                        diff0 = _mm_hadds_epi16(diff0, diff1);
                        diff2 = _mm_hadds_epi16(diff2, diff3);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff2);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff4);

                        d03 = _mm_add_epi32(d03, _mm_cvtepi16_epi32(diff0));
                        d4 += _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8)), 0);

                        org += i_org;
                        rec += i_rec;
                    }
                    t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
                    _mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
                    statsDate->diff[4] += d4;

                    cn0 = _mm_hadds_epi16(cn0, cn1);
                    cn2 = _mm_hadds_epi16(cn2, cn3);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn2);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn4);

                    cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
                    cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
                    _mm_storeu_si128((__m128i*)statsDate->count, cn1);
                    statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
                }
                else{
                    for (y = 1; y < lcu_pix_height - 1; y++) {
                        for (x = start_x_r; x < end_x_r; x += 16) {
                            i = x - start_x_r;
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

                                _mm_store_si128((__m128i*)(etypes + i), etype);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);
                            _mm_store_si128((__m128i*)(diffs + i), s0);
                            _mm_store_si128((__m128i*)(diffs + i + 8), s1);
                        }
                        for (x = 0; x < end_x_r - start_x_r; x++) {
                            i = etypes[x];
                            statsDate->diff[i] += diffs[x];
                            statsDate->count[i]++;
                        }
                        org += i_org;
                        rec += i_rec;
                    }
                }
                //last row
                for (x = start_x_rn; x < end_x_rn; x += 16) {
                    i = x - start_x_rn;
                    s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
                    s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                    s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
                    s3 = _mm_loadu_si128((__m128i*)(org + x));

                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

                        _mm_store_si128((__m128i*)(etypes + i), etype);

                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s1 = _mm_unpackhi_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
                    s3 = _mm_unpackhi_epi8(s3, zero);

                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                    s1 = _mm_subs_epi16(s3, s1);
                    _mm_store_si128((__m128i*)(diffs + i), s0);
                    _mm_store_si128((__m128i*)(diffs + i + 8), s1);
                }
                for (x = 0; x < end_x_rn - start_x_rn; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                break;
            case SAO_TYPE_EO_45:
                start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
                end_x_r0 = smb_available_upright ? lcu_pix_width : (lcu_pix_width - 1);
                start_x_r = smb_available_left ? 0 : 1;
                end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
                start_x_rn = smb_available_leftdown ? 0 : 1;
                end_x_rn = smb_available_down ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;

                for (x = start_x_r0; x < end_x_r0; x += 16) {
                    s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
                    s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                    s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
                    s3 = _mm_loadu_si128((__m128i*)(org + x));

                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

                        i = x - start_x_r0;
                    _mm_store_si128((__m128i*)(etypes + i), etype);

                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s1 = _mm_unpackhi_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
                    s3 = _mm_unpackhi_epi8(s3, zero);

                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                    s1 = _mm_subs_epi16(s3, s1);
                    _mm_store_si128((__m128i*)(diffs + i), s0);
                    _mm_store_si128((__m128i*)(diffs + i + 8), s1);
                }
                for (x = 0; x < end_x_r0 - start_x_r0; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                org += i_org;
                rec += i_rec;

                //middle rows
                if (!((end_x_r - start_x_r) & 0x0f)){
                    __m128i t4, t5, t6;
                    __m128i diff0, diff1, diff2, diff3, diff4;
                    __m128i cn0, cn1, cn2, cn3, cn4;
                    __m128i c0, c1, c2, c3, c4;
                    __m128i d03;
                    int d4 = 0;

                    d03 = zero;
                    c0 = _mm_set1_epi16(-2);
                    c1 = _mm_set1_epi16(-1);
                    c2 = _mm_set1_epi16(0);
                    c3 = _mm_set1_epi16(1);
                    c4 = _mm_set1_epi16(2);
                    cn0 = cn1 = cn2 = cn3 = cn4 = zero;

                    for (y = 1; y < lcu_pix_height - 1; y++) {
                        diff0 = zero;
                        diff1 = zero;
                        diff2 = zero;
                        diff3 = zero;
                        diff4 = zero;
                        for (x = start_x_r; x < end_x_r; x += 16) {
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16(s0, s1, s2, t0, t1, t2, t3, etype)

                            t5 = _mm_cvtepi8_epi16(etype); //low 8
                            t6 = _mm_cvtepi8_epi16(_mm_srli_si128(etype, 8));

                            t0 = _mm_cmpeq_epi16(t5, c0);
                            t1 = _mm_cmpeq_epi16(t5, c1);
                            t2 = _mm_cmpeq_epi16(t5, c2);
                            t3 = _mm_cmpeq_epi16(t5, c3);
                            t4 = _mm_cmpeq_epi16(t5, c4);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

                            t0 = _mm_cmpeq_epi16(t6, c0);
                            t1 = _mm_cmpeq_epi16(t6, c1);
                            t2 = _mm_cmpeq_epi16(t6, c2);
                            t3 = _mm_cmpeq_epi16(t6, c3);
                            t4 = _mm_cmpeq_epi16(t6, c4);

                            cn0 = _mm_adds_epi16(cn0, t0);
                            cn1 = _mm_adds_epi16(cn1, t1);
                            cn2 = _mm_adds_epi16(cn2, t2);
                            cn3 = _mm_adds_epi16(cn3, t3);
                            cn4 = _mm_adds_epi16(cn4, t4);

                            diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
                            diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
                            diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
                            diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
                            diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
                        }
                        diff0 = _mm_hadds_epi16(diff0, diff1);
                        diff2 = _mm_hadds_epi16(diff2, diff3);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff2);
                        diff4 = _mm_hadds_epi16(diff4, zero);
                        diff0 = _mm_hadds_epi16(diff0, diff4);

                        d03 = _mm_add_epi32(d03, _mm_cvtepi16_epi32(diff0));
                        d4 += _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8)), 0);

                        org += i_org;
                        rec += i_rec;
                    }
                    t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
                    _mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
                    statsDate->diff[4] += d4;

                    cn0 = _mm_hadds_epi16(cn0, cn1);
                    cn2 = _mm_hadds_epi16(cn2, cn3);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn2);
                    cn4 = _mm_hadds_epi16(cn4, zero);
                    cn0 = _mm_hadds_epi16(cn0, cn4);

                    cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
                    cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
                    _mm_storeu_si128((__m128i*)statsDate->count, cn1);
                    statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
                }
                else{
                    for (y = 1; y < lcu_pix_height - 1; y++) {
                        for (x = start_x_r; x < end_x_r; x += 16) {
                            s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
                            s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                            s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
                            s3 = _mm_loadu_si128((__m128i*)(org + x));

                            CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

                                i = x - start_x_r;
                            _mm_store_si128((__m128i*)(etypes + i), etype);

                            s0 = _mm_unpacklo_epi8(s1, zero);
                            s1 = _mm_unpackhi_epi8(s1, zero);
                            s2 = _mm_unpacklo_epi8(s3, zero);
                            s3 = _mm_unpackhi_epi8(s3, zero);

                            s0 = _mm_subs_epi16(s2, s0);    //org - rec
                            s1 = _mm_subs_epi16(s3, s1);
                            _mm_store_si128((__m128i*)(diffs + i), s0);
                            _mm_store_si128((__m128i*)(diffs + i + 8), s1);
                        }
                        for (x = 0; x < end_x_r - start_x_r; x++) {
                            i = etypes[x];
                            statsDate->diff[i] += diffs[x];
                            statsDate->count[i]++;
                        }
                        org += i_org;
                        rec += i_rec;
                    }
                }
                for (x = start_x_rn; x < end_x_rn; x += 16) {
                    s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
                    s1 = _mm_loadu_si128((__m128i*)&rec[x]);
                    s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
                    s3 = _mm_loadu_si128((__m128i*)(org + x));

                    CALCU_ETYPE_16_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

                        i = x - start_x_rn;
                    _mm_store_si128((__m128i*)(etypes + i), etype);

                    s0 = _mm_unpacklo_epi8(s1, zero);
                    s1 = _mm_unpackhi_epi8(s1, zero);
                    s2 = _mm_unpacklo_epi8(s3, zero);
                    s3 = _mm_unpackhi_epi8(s3, zero);

                    s0 = _mm_subs_epi16(s2, s0);    //org - rec
                    s1 = _mm_subs_epi16(s3, s1);
                    _mm_store_si128((__m128i*)(diffs + i), s0);
                    _mm_store_si128((__m128i*)(diffs + i + 8), s1);
                }
                for (x = 0; x < end_x_rn - start_x_rn; x++) {
                    i = etypes[x];
                    statsDate->diff[i] += diffs[x];
                    statsDate->count[i]++;
                }
                break;
            default:
                printf("Not a supported SAO types\n");
                assert(0);
                exit(-1);

            }
        }
    }
}

void sao_get_stats_sse128_10bit(void *handle, void* sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height,
	int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down)
{
	int type;
	int start_x, end_x, start_y, end_y;
	int start_x_r0, end_x_r0, start_x_r, end_x_r, start_x_rn, end_x_rn;
	int x, y;
	SAOStatData *saostatsData = (SAOStatData *)sao_data;
	SAOStatData *statsDate;
	avs3_enc_t *h = (avs3_enc_t *)handle;
	ALIGNED_16(i16s_t diffs[64 + 16]);
	ALIGNED_16(i16s_t etypes[64 + 16]);
	int bit_depth = h->input->bit_depth;
    int smb_available_upleft = (smb_available_up && smb_available_left);
    int smb_available_upright = (smb_available_up && smb_available_right);
    int smb_available_leftdown = (smb_available_down && smb_available_left);
    int smb_available_rightdwon = (smb_available_down && smb_available_right);

	pel_t *rec, *org;
	int i_rec, i_org;
	__m128i zero = _mm_setzero_si128();
	__m128i cst2 = _mm_set1_epi16(2);
	__m128i s0, s1, s2, s3, s4, s5, s6, s7;
	__m128i t0, t1, t2, t3, etype;
	int i;

	i_rec = h->img_rec->i_stride[compIdx];
	i_org = h->img_org->i_stride[compIdx];
	if (lcu_pix_width == 4){
		for (type = 0; type < NUM_SAO_NEW_TYPES; type++) {
			rec = h->img_rec->plane[compIdx] + pix_y * i_rec + pix_x;
			org = h->img_org->plane[compIdx] + pix_y * i_org + pix_x;

			statsDate = &(saostatsData[type]);
			switch (type) {
			case SAO_TYPE_EO_0:
				start_x = smb_available_left ? 0 : 1;
				end_x = smb_available_right ? 4 : (4 - 1);
				//end_x_16 = end_x - ((end_x - start_x) & 0x0f);

				for (y = 0; y < lcu_pix_height; y++) {
					//diff = src[start_x] - src[start_x - 1];
					//leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
					s0 = _mm_loadu_si128((__m128i*)(rec - 1));
					s1 = _mm_srli_si128(s0, 2);
					s2 = _mm_srli_si128(s0, 4);
					s3 = _mm_loadl_epi64((__m128i*)(org));

					CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

					_mm_storel_epi64((__m128i*)(etypes), etype);

					s0 = _mm_subs_epi16(s3, s1);    //org - rec
					_mm_storel_epi64((__m128i*)(diffs), s0);

					for (x = start_x; x < end_x; x++) {
						i = etypes[x];
						statsDate->diff[i] += diffs[x];
						statsDate->count[i]++;
					}
					org += i_org;
					rec += i_rec;
				}
				break;
			case SAO_TYPE_EO_90:
				start_y = smb_available_up ? 0 : 1;
				end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);
				rec += start_y*i_rec;
				org += start_y*i_org;
				for (y = start_y; y < end_y; y++) {
					int i0, i1, i2, i3;
					s0 = _mm_loadl_epi64((__m128i*)(rec - i_rec));
					s1 = _mm_loadl_epi64((__m128i*)rec);
					s2 = _mm_loadl_epi64((__m128i*)(rec + i_rec));
					s3 = _mm_loadl_epi64((__m128i*)org);

					CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype);
					i0 = (pel_t)_mm_extract_epi16(etype, 0);
					i1 = (pel_t)_mm_extract_epi16(etype, 1);
					i2 = (pel_t)_mm_extract_epi16(etype, 2);
					i3 = (pel_t)_mm_extract_epi16(etype, 3);

					statsDate->count[i0]++;
					statsDate->count[i1]++;
					statsDate->count[i2]++;
					statsDate->count[i3]++;

					s0 = _mm_subs_epi16(s3, s1);    //org - rec

					s0 = _mm_cvtepi16_epi32(s0);
					statsDate->diff[i0] += _mm_extract_epi32(s0, 0);
					statsDate->diff[i1] += _mm_extract_epi32(s0, 1);
					statsDate->diff[i2] += _mm_extract_epi32(s0, 2);
					statsDate->diff[i3] += _mm_extract_epi32(s0, 3);

					org += i_org;
					rec += i_rec;
				}

				break;
			case SAO_TYPE_EO_135:
				start_x_r0 = smb_available_upleft ? 0 : 1;
				end_x_r0 = smb_available_up ? (smb_available_right ? 4 : (4 - 1)) : 1;
				start_x_r = smb_available_left ? 0 : 1;
				end_x_r = smb_available_right ? 4 : (4 - 1);
				start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (4 - 1);
				end_x_rn = smb_available_rightdwon ? 4 : (4 - 1);

				s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 - i_rec - 1]);
				s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0]);
				s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 + i_rec + 1]);
				s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r0));

				CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

					_mm_storel_epi64((__m128i*)(etypes), etype);

				s0 = _mm_subs_epi16(s3, s1);    //org - rec
				_mm_storel_epi64((__m128i*)(diffs), s0);

				for (x = 0; x < end_x_r0 - start_x_r0; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				org += i_org;
				rec += i_rec;

				//middle rows
				for (y = 1; y < lcu_pix_height - 1; y++) {
					s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r - i_rec - 1]);
					s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r]);
					s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r + i_rec + 1]);
					s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r));

					CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

						_mm_storel_epi64((__m128i*)(etypes), etype);

					s0 = _mm_subs_epi16(s3, s1);    //org - rec
					_mm_storel_epi64((__m128i*)(diffs), s0);

					for (x = 0; x < end_x_r - start_x_r; x++) {
						i = etypes[x];
						statsDate->diff[i] += diffs[x];
						statsDate->count[i]++;
					}
					org += i_org;
					rec += i_rec;
				}
				//last row
				s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn - i_rec - 1]);
				s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn]);
				s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn + i_rec + 1]);
				s3 = _mm_loadl_epi64((__m128i*)(org + start_x_rn));

				CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

					_mm_storel_epi64((__m128i*)(etypes), etype);

				s0 = _mm_subs_epi16(s3, s1);    //org - rec
				_mm_storel_epi64((__m128i*)(diffs), s0);

				for (x = 0; x < end_x_rn - start_x_rn; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				break;
			case SAO_TYPE_EO_45:
				start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (4 - 1);
				end_x_r0 = smb_available_upright ? 4 : (4 - 1);
				start_x_r = smb_available_left ? 0 : 1;
				end_x_r = smb_available_right ? 4 : (4 - 1);
				start_x_rn = smb_available_leftdown ? 0 : 1;
				end_x_rn = smb_available_down ? (smb_available_right ? 4 : (4 - 1)) : 1;

				s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 - i_rec + 1]);
				s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0]);
				s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r0 + i_rec - 1]);
				s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r0));

				CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

					_mm_storel_epi64((__m128i*)(etypes), etype);

				s0 = _mm_subs_epi16(s3, s1);    //org - rec
				_mm_storel_epi64((__m128i*)(diffs), s0);

				for (x = 0; x < end_x_r0 - start_x_r0; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				org += i_org;
				rec += i_rec;

				//middle rows
				for (y = 1; y < lcu_pix_height - 1; y++) {
					s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_r - i_rec + 1]);
					s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_r]);
					s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_r + i_rec - 1]);
					s3 = _mm_loadl_epi64((__m128i*)(org + start_x_r));

					CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

						_mm_storel_epi64((__m128i*)(etypes), etype);

					s0 = _mm_subs_epi16(s3, s1);    //org - rec
					s1 = _mm_subs_epi16(s3, s1);
					_mm_storel_epi64((__m128i*)(diffs), s0);

					for (x = 0; x < end_x_r - start_x_r; x++) {
						i = etypes[x];
						statsDate->diff[i] += diffs[x];
						statsDate->count[i]++;
					}
					org += i_org;
					rec += i_rec;
				}
				s0 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn - i_rec + 1]);
				s1 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn]);
				s2 = _mm_loadl_epi64((__m128i*)&rec[start_x_rn + i_rec - 1]);
				s3 = _mm_loadl_epi64((__m128i*)(org + start_x_rn));

				CALCU_ETYPE_8_NEW(s0, s1, s2, t0, t1, t2, t3, etype)

					_mm_storel_epi64((__m128i*)(etypes), etype);

				s0 = _mm_subs_epi16(s3, s1);    //org - rec
				_mm_storel_epi64((__m128i*)(diffs), s0);

				for (x = 0; x < end_x_rn - start_x_rn; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				break;
			default:
				printf("Not a supported SAO types\n");
				assert(0);
				exit(-1);

			}
		}
	}
	else{
		for (type = 0; type < NUM_SAO_NEW_TYPES; type++) {
			rec = h->img_rec->plane[compIdx] + pix_y * i_rec + pix_x;
			org = h->img_org->plane[compIdx] + pix_y * i_org + pix_x;

			statsDate = &(saostatsData[type]);
			switch (type) {
			case SAO_TYPE_EO_0:
				start_x = smb_available_left ? 0 : 1;
				end_x = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
				__m128i t4, t5, t6;
				//end_x_16 = end_x - ((end_x - start_x) & 0x0f);
				if (!((end_x - start_x) & 0x0f)){
					
					__m128i diff0, diff1, diff2, diff3, diff4;
					__m128i cn0, cn1, cn2, cn3, cn4;
					__m128i c0, c1, c2, c3, c4;
					__m128i d03;
					int d4 = 0;

					d03 = zero;
					c0 = _mm_set1_epi16(-2);
					c1 = _mm_set1_epi16(-1);
					c2 = _mm_set1_epi16(0);
					c3 = _mm_set1_epi16(1);
					c4 = _mm_set1_epi16(2);
					cn0 = cn1 = cn2 = cn3 = cn4 = zero;

					for (y = 0; y < lcu_pix_height; y++) {
						//diff = src[start_x] - src[start_x - 1];
						//leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
						diff0 = zero;
						diff1 = zero;
						diff2 = zero;
						diff3 = zero;
						diff4 = zero;

						for (x = start_x; x < end_x; x += 16) {
							s0 = _mm_loadu_si128((__m128i*)&rec[x - 1]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + 1]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x + 7]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + 9]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8_NOADD2(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8_NOADD2(s4, s5, s6, t0, t1, t2, t3, t6);

							t0 = _mm_cmpeq_epi16(t5, c0);
							t1 = _mm_cmpeq_epi16(t5, c1);
							t2 = _mm_cmpeq_epi16(t5, c2);
							t3 = _mm_cmpeq_epi16(t5, c3);
							t4 = _mm_cmpeq_epi16(t5, c4);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

							t0 = _mm_cmpeq_epi16(t6, c0);
							t1 = _mm_cmpeq_epi16(t6, c1);
							t2 = _mm_cmpeq_epi16(t6, c2);
							t3 = _mm_cmpeq_epi16(t6, c3);
							t4 = _mm_cmpeq_epi16(t6, c4);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
						}
						diff0 = _mm_hadds_epi16(diff0, diff1);
						diff2 = _mm_hadds_epi16(diff2, diff3);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff0 = _mm_hadds_epi16(diff0, diff2);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff2 = _mm_cvtepi16_epi32(diff0);
						diff0 = _mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8));
						diff0 = _mm_hadd_epi32(diff2, diff0);

						d03 = _mm_add_epi32(d03, diff0);
						d4 += ((short)_mm_extract_epi16(diff4, 0)) + ((short)_mm_extract_epi16(diff4, 1));

						org += i_org;
						rec += i_rec;
					}

					t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
					_mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
					statsDate->diff[4] += d4;

					cn0 = _mm_hadds_epi16(cn0, cn1);
					cn2 = _mm_hadds_epi16(cn2, cn3);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn2);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn4);

					cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
					cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
					_mm_storeu_si128((__m128i*)statsDate->count, cn1);
					statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
				}
				else{
					for (y = 0; y < lcu_pix_height; y++) {
						//diff = src[start_x] - src[start_x - 1];
						//leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);

						for (x = start_x; x < end_x; x += 16) {
							i = x - start_x;
							s0 = _mm_loadu_si128((__m128i*)&rec[x - 1]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + 1]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x + 7]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + 9]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);

							_mm_store_si128((__m128i*)(etypes + i), t5);
							_mm_store_si128((__m128i*)(etypes + i + 8), t6);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);
							_mm_store_si128((__m128i*)(diffs + i), s0);
							_mm_store_si128((__m128i*)(diffs + i + 8), s1);
						}

						for (x = 0; x < end_x - start_x; x++) {
							i = etypes[x];
							statsDate->diff[i] += diffs[x];
							statsDate->count[i]++;
						}
						org += i_org;
						rec += i_rec;
					}

				}
				break;
			case SAO_TYPE_EO_90:
				start_y = smb_available_up ? 0 : 1;
				end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);
				rec += start_y*i_rec;
				org += start_y*i_org;
				if (!(lcu_pix_width & 0x0f)){
					__m128i t4, t5, t6;
					__m128i diff0, diff1, diff2, diff3, diff4;
					__m128i cn0, cn1, cn2, cn3, cn4;
					__m128i c0, c1, c2, c3, c4;
					__m128i d03;
					int d4 = 0;

					d03 = zero;
					c0 = _mm_set1_epi16(-2);
					c1 = _mm_set1_epi16(-1);
					c2 = _mm_set1_epi16(0);
					c3 = _mm_set1_epi16(1);
					c4 = _mm_set1_epi16(2);
					cn0 = cn1 = cn2 = cn3 = cn4 = zero;

					for (y = start_y; y < end_y; y++) {
						diff0 = zero;
						diff1 = zero;
						diff2 = zero;
						diff3 = zero;
						diff4 = zero;
						for (x = 0; x < lcu_pix_width; x += 16) {
							s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 8]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 8]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8_NOADD2(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8_NOADD2(s4, s5, s6, t0, t1, t2, t3, t6);

							t0 = _mm_cmpeq_epi16(t5, c0);
							t1 = _mm_cmpeq_epi16(t5, c1);
							t2 = _mm_cmpeq_epi16(t5, c2);
							t3 = _mm_cmpeq_epi16(t5, c3);
							t4 = _mm_cmpeq_epi16(t5, c4);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

							t0 = _mm_cmpeq_epi16(t6, c0);
							t1 = _mm_cmpeq_epi16(t6, c1);
							t2 = _mm_cmpeq_epi16(t6, c2);
							t3 = _mm_cmpeq_epi16(t6, c3);
							t4 = _mm_cmpeq_epi16(t6, c4);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
						}
						diff0 = _mm_hadds_epi16(diff0, diff1);
						diff2 = _mm_hadds_epi16(diff2, diff3);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff0 = _mm_hadds_epi16(diff0, diff2);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff2 = _mm_cvtepi16_epi32(diff0);
						diff0 = _mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8));
						diff0 = _mm_hadd_epi32(diff2, diff0);

						d03 = _mm_add_epi32(d03, diff0);
						d4 += ((short)_mm_extract_epi16(diff4, 0)) + ((short)_mm_extract_epi16(diff4, 1));

						org += i_org;
						rec += i_rec;
					}
					t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
					_mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
					statsDate->diff[4] += d4;

					cn0 = _mm_hadds_epi16(cn0, cn1);
					cn2 = _mm_hadds_epi16(cn2, cn3);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn2);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn4);

					cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
					cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
					_mm_storeu_si128((__m128i*)statsDate->count, cn1);
					statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
				}
				else{
					for (y = start_y; y < end_y; y++) {
						for (x = 0; x < lcu_pix_width; x += 16) {
							s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 8]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 8]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);

							_mm_store_si128((__m128i*)(etypes + x), t5);
							_mm_store_si128((__m128i*)(etypes + x + 8), t6);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);
							_mm_store_si128((__m128i*)(diffs + x), s0);
							_mm_store_si128((__m128i*)(diffs + x + 8), s1);
						}

						for (x = 0; x < lcu_pix_width; x++) {
							i = etypes[x];
							statsDate->diff[i] += diffs[x];
							statsDate->count[i]++;
						}

						org += i_org;
						rec += i_rec;
					}
				}
				break;
			case SAO_TYPE_EO_135:
				start_x_r0 = smb_available_upleft ? 0 : 1;
				end_x_r0 = smb_available_up ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;
				start_x_r = smb_available_left ? 0 : 1;
				end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
				start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
				end_x_rn = smb_available_rightdwon ? lcu_pix_width : (lcu_pix_width - 1);

				for (x = start_x_r0; x < end_x_r0; x += 16) {
					i = x - start_x_r0;
					s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
					s1 = _mm_loadu_si128((__m128i*)&rec[x]);
					s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
					s3 = _mm_loadu_si128((__m128i*)(org + x));

					s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 7]);
					s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
					s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 9]);
					s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

					CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
					CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);

					_mm_store_si128((__m128i*)(etypes + i), t5);
					_mm_store_si128((__m128i*)(etypes + i + 8), t6);

					s0 = _mm_subs_epi16(s3, s1);    //org - rec
					s1 = _mm_subs_epi16(s7, s5);
					_mm_store_si128((__m128i*)(diffs + i), s0);
					_mm_store_si128((__m128i*)(diffs + i + 8), s1);
				}

				for (x = 0; x < end_x_r0 - start_x_r0; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				org += i_org;
				rec += i_rec;

				//middle rows
				if (!((end_x_r - start_x_r) & 0x0f)){
					__m128i t4, t5, t6;
					__m128i diff0, diff1, diff2, diff3, diff4;
					__m128i cn0, cn1, cn2, cn3, cn4;
					__m128i c0, c1, c2, c3, c4;
					__m128i d03;
					int d4 = 0;

					d03 = zero;
					c0 = _mm_set1_epi16(-2);
					c1 = _mm_set1_epi16(-1);
					c2 = _mm_set1_epi16(0);
					c3 = _mm_set1_epi16(1);
					c4 = _mm_set1_epi16(2);
					cn0 = cn1 = cn2 = cn3 = cn4 = zero;

					for (y = 1; y < lcu_pix_height - 1; y++) {
						diff0 = zero;
						diff1 = zero;
						diff2 = zero;
						diff3 = zero;
						diff4 = zero;
						for (x = start_x_r; x < end_x_r; x += 16) {
							s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 7]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 9]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8_NOADD2(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8_NOADD2(s4, s5, s6, t0, t1, t2, t3, t6);

							t0 = _mm_cmpeq_epi16(t5, c0);
							t1 = _mm_cmpeq_epi16(t5, c1);
							t2 = _mm_cmpeq_epi16(t5, c2);
							t3 = _mm_cmpeq_epi16(t5, c3);
							t4 = _mm_cmpeq_epi16(t5, c4);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

							t0 = _mm_cmpeq_epi16(t6, c0);
							t1 = _mm_cmpeq_epi16(t6, c1);
							t2 = _mm_cmpeq_epi16(t6, c2);
							t3 = _mm_cmpeq_epi16(t6, c3);
							t4 = _mm_cmpeq_epi16(t6, c4);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
						}
						diff0 = _mm_hadds_epi16(diff0, diff1);
						diff2 = _mm_hadds_epi16(diff2, diff3);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff0 = _mm_hadds_epi16(diff0, diff2);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff2 = _mm_cvtepi16_epi32(diff0);
						diff0 = _mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8));
						diff0 = _mm_hadd_epi32(diff2, diff0);

						d03 = _mm_add_epi32(d03, diff0);
						d4 += ((short)_mm_extract_epi16(diff4, 0)) + ((short)_mm_extract_epi16(diff4, 1));

						org += i_org;
						rec += i_rec;
					}
					t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
					_mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
					statsDate->diff[4] += d4;

					cn0 = _mm_hadds_epi16(cn0, cn1);
					cn2 = _mm_hadds_epi16(cn2, cn3);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn2);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn4);

					cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
					cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
					_mm_storeu_si128((__m128i*)statsDate->count, cn1);
					statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
				}
				else{
					for (y = 1; y < lcu_pix_height - 1; y++) {
						for (x = start_x_r; x < end_x_r; x += 16) {
							i = x - start_x_r;
							s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 7]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 9]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);

							_mm_store_si128((__m128i*)(etypes + i), t5);
							_mm_store_si128((__m128i*)(etypes + i + 8), t6);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);

							_mm_store_si128((__m128i*)(diffs + i), s0);
							_mm_store_si128((__m128i*)(diffs + i + 8), s1);
						}
						for (x = 0; x < end_x_r - start_x_r; x++) {
							i = etypes[x];
							statsDate->diff[i] += diffs[x];
							statsDate->count[i]++;
						}
						org += i_org;
						rec += i_rec;
					}
				}
				//last row
				for (x = start_x_rn; x < end_x_rn; x += 16) {
					i = x - start_x_rn;
					s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec - 1]);
					s1 = _mm_loadu_si128((__m128i*)&rec[x]);
					s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 1]);
					s3 = _mm_loadu_si128((__m128i*)(org + x));

					s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 7]);
					s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
					s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 9]);
					s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

					CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
					CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);

					_mm_store_si128((__m128i*)(etypes + i), t5);
					_mm_store_si128((__m128i*)(etypes + i + 8), t6);

					s0 = _mm_subs_epi16(s3, s1);    //org - rec
					s1 = _mm_subs_epi16(s7, s5);

					_mm_store_si128((__m128i*)(diffs + i), s0);
					_mm_store_si128((__m128i*)(diffs + i + 8), s1);
				}
				for (x = 0; x < end_x_rn - start_x_rn; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				break;
			case SAO_TYPE_EO_45:
				start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
				end_x_r0 = smb_available_upright ? lcu_pix_width : (lcu_pix_width - 1);
				start_x_r = smb_available_left ? 0 : 1;
				end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
				start_x_rn = smb_available_leftdown ? 0 : 1;
				end_x_rn = smb_available_down ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;

				for (x = start_x_r0; x < end_x_r0; x += 16) {
					i = x - start_x_r0;
					s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
					s1 = _mm_loadu_si128((__m128i*)&rec[x]);
					s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
					s3 = _mm_loadu_si128((__m128i*)(org + x));

					s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 9]);
					s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
					s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 7]);
					s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

					CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
					CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);

					_mm_store_si128((__m128i*)(etypes + i), t5);
					_mm_store_si128((__m128i*)(etypes + i + 8), t6);

					s0 = _mm_subs_epi16(s3, s1);    //org - rec
					s1 = _mm_subs_epi16(s7, s5);

					_mm_store_si128((__m128i*)(diffs + i), s0);
					_mm_store_si128((__m128i*)(diffs + i + 8), s1);
				}
				for (x = 0; x < end_x_r0 - start_x_r0; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				org += i_org;
				rec += i_rec;

				//middle rows
				if (!((end_x_r - start_x_r) & 0x0f)){
					__m128i t4, t5, t6;
					__m128i diff0, diff1, diff2, diff3, diff4;
					__m128i cn0, cn1, cn2, cn3, cn4;
					__m128i c0, c1, c2, c3, c4;
					__m128i d03;
					int d4 = 0;

					d03 = zero;
					c0 = _mm_set1_epi16(-2);
					c1 = _mm_set1_epi16(-1);
					c2 = _mm_set1_epi16(0);
					c3 = _mm_set1_epi16(1);
					c4 = _mm_set1_epi16(2);
					cn0 = cn1 = cn2 = cn3 = cn4 = zero;

					for (y = 1; y < lcu_pix_height - 1; y++) {
						diff0 = zero;
						diff1 = zero;
						diff2 = zero;
						diff3 = zero;
						diff4 = zero;
						for (x = start_x_r; x < end_x_r; x += 16) {
							s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 9]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 7]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8_NOADD2(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8_NOADD2(s4, s5, s6, t0, t1, t2, t3, t6);

							t0 = _mm_cmpeq_epi16(t5, c0);
							t1 = _mm_cmpeq_epi16(t5, c1);
							t2 = _mm_cmpeq_epi16(t5, c2);
							t3 = _mm_cmpeq_epi16(t5, c3);
							t4 = _mm_cmpeq_epi16(t5, c4);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s0, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s0, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s0, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s0, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s0, t4));

							t0 = _mm_cmpeq_epi16(t6, c0);
							t1 = _mm_cmpeq_epi16(t6, c1);
							t2 = _mm_cmpeq_epi16(t6, c2);
							t3 = _mm_cmpeq_epi16(t6, c3);
							t4 = _mm_cmpeq_epi16(t6, c4);

							cn0 = _mm_adds_epi16(cn0, t0);
							cn1 = _mm_adds_epi16(cn1, t1);
							cn2 = _mm_adds_epi16(cn2, t2);
							cn3 = _mm_adds_epi16(cn3, t3);
							cn4 = _mm_adds_epi16(cn4, t4);

							diff0 = _mm_adds_epi16(diff0, _mm_and_si128(s1, t0));
							diff1 = _mm_adds_epi16(diff1, _mm_and_si128(s1, t1));
							diff2 = _mm_adds_epi16(diff2, _mm_and_si128(s1, t2));
							diff3 = _mm_adds_epi16(diff3, _mm_and_si128(s1, t3));
							diff4 = _mm_adds_epi16(diff4, _mm_and_si128(s1, t4));
						}
						diff0 = _mm_hadds_epi16(diff0, diff1);
						diff2 = _mm_hadds_epi16(diff2, diff3);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff0 = _mm_hadds_epi16(diff0, diff2);
						diff4 = _mm_hadds_epi16(diff4, zero);
						diff2 = _mm_cvtepi16_epi32(diff0);
						diff0 = _mm_cvtepi16_epi32(_mm_srli_si128(diff0, 8));
						diff0 = _mm_hadd_epi32(diff2, diff0);

						d03 = _mm_add_epi32(d03, diff0);
						d4 += ((short)_mm_extract_epi16(diff4, 0)) + ((short)_mm_extract_epi16(diff4, 1));

						org += i_org;
						rec += i_rec;
					}
					t0 = _mm_loadu_si128((__m128i*)(statsDate->diff));
					_mm_storeu_si128((__m128i*)statsDate->diff, _mm_add_epi32(t0, d03));
					statsDate->diff[4] += d4;

					cn0 = _mm_hadds_epi16(cn0, cn1);
					cn2 = _mm_hadds_epi16(cn2, cn3);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn2);
					cn4 = _mm_hadds_epi16(cn4, zero);
					cn0 = _mm_hadds_epi16(cn0, cn4);

					cn1 = _mm_loadu_si128((__m128i*)statsDate->count);
					cn1 = _mm_sub_epi32(cn1, _mm_cvtepi16_epi32(cn0));
					_mm_storeu_si128((__m128i*)statsDate->count, cn1);
					statsDate->count[4] -= _mm_extract_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(cn0, 8)), 0);
				}
				else{
					for (y = 1; y < lcu_pix_height - 1; y++) {
						for (x = start_x_r; x < end_x_r; x += 16) {
							i = x - start_x_r;
							s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
							s1 = _mm_loadu_si128((__m128i*)&rec[x]);
							s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
							s3 = _mm_loadu_si128((__m128i*)(org + x));

							s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 9]);
							s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
							s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 7]);
							s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

							CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
							CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);
							
							_mm_store_si128((__m128i*)(etypes + i), t5);
							_mm_store_si128((__m128i*)(etypes + i + 8), t6);

							s0 = _mm_subs_epi16(s3, s1);    //org - rec
							s1 = _mm_subs_epi16(s7, s5);

							_mm_store_si128((__m128i*)(diffs + i), s0);
							_mm_store_si128((__m128i*)(diffs + i + 8), s1);
						}
						for (x = 0; x < end_x_r - start_x_r; x++) {
							i = etypes[x];
							statsDate->diff[i] += diffs[x];
							statsDate->count[i]++;
						}
						org += i_org;
						rec += i_rec;
					}
				}
				//last row
				if (lcu_pix_height == 0) {
					org -= i_org * 2;
					rec -= i_rec * 2;
				}
				for (x = start_x_rn; x < end_x_rn; x += 16) {
					i = x - start_x_rn;
					s0 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 1]);
					s1 = _mm_loadu_si128((__m128i*)&rec[x]);
					s2 = _mm_loadu_si128((__m128i*)&rec[x + i_rec - 1]);
					s3 = _mm_loadu_si128((__m128i*)(org + x));

					s4 = _mm_loadu_si128((__m128i*)&rec[x - i_rec + 9]);
					s5 = _mm_loadu_si128((__m128i*)&rec[x + 8]);
					s6 = _mm_loadu_si128((__m128i*)&rec[x + i_rec + 7]);
					s7 = _mm_loadu_si128((__m128i*)(org + x + 8));

					CALCU_ETYPE_8(s0, s1, s2, t0, t1, t2, t3, t5);
					CALCU_ETYPE_8(s4, s5, s6, t0, t1, t2, t3, t6);

					_mm_store_si128((__m128i*)(etypes + i), t5);
					_mm_store_si128((__m128i*)(etypes + i + 8), t6);

					s0 = _mm_subs_epi16(s3, s1);    //org - rec
					s1 = _mm_subs_epi16(s7, s5);
					_mm_store_si128((__m128i*)(diffs + i), s0);
					_mm_store_si128((__m128i*)(diffs + i + 8), s1);
				}
				for (x = 0; x < end_x_rn - start_x_rn; x++) {
					i = etypes[x];
					statsDate->diff[i] += diffs[x];
					statsDate->count[i]++;
				}
				break;
			default:
				printf("Not a supported SAO types\n");
				assert(0);
				exit(-1);

			}
		}
	}
}


void SAO_on_block_sse128_10bit(void *handle, void* sao_data, int compIdx, int pix_y, int pix_x, int lcu_pix_height,
	int lcu_pix_width, int smb_available_left, int smb_available_right, int smb_available_up, int smb_available_down)
{
	avs3_enc_t *h = (avs3_enc_t *)handle;
	SAOBlkParam *saoBlkParam = (SAOBlkParam *)sao_data;
	int type;
	int start_x, end_x, start_y, end_y;
	int start_x_r0, end_x_r0, start_x_r, end_x_r, start_x_rn, end_x_rn;
	int x, y;
	pel_t *src, *dst;
	int i_src, i_dst;
	int max_pixel = (1 << h->input->bit_depth) - 1;
	__m128i off0, off1, off2, off3, off4;
	__m128i s0, s1, s2;
	__m128i t0, t1, t2, t3, t4, etype;
	__m128i c0, c1, c2, c3, c4;
	__m128i mask;
	__m128i min_val = _mm_setzero_si128();
	__m128i max_val = _mm_set1_epi16(max_pixel);
    int smb_available_upleft = (smb_available_up && smb_available_left);
    int smb_available_upright = (smb_available_up && smb_available_right);
    int smb_available_leftdown = (smb_available_down && smb_available_left);
    int smb_available_rightdwon = (smb_available_down && smb_available_right);

	i_src = h->img_sao.i_stride[compIdx];
	i_dst = h->img_rec->i_stride[compIdx];
	src = h->img_sao.plane[compIdx] + pix_y * i_src + pix_x;
	dst = h->img_rec->plane[compIdx] + pix_y * i_dst + pix_x;

	type = saoBlkParam->typeIdc;

	switch (type) {
	case SAO_TYPE_EO_0: {
							int end_x_8;

							start_x = smb_available_left ? 0 : 1;
							end_x = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
							end_x_8 = end_x - ((end_x - start_x) & 0x07);

							mask = _mm_load_si128((__m128i*)(intrinsic_mask_10bit[end_x - end_x_8 - 1]));
							if (lcu_pix_width == 4){
								c0 = _mm_set1_epi16(-2);
								c1 = _mm_set1_epi16(-1);
								c2 = _mm_set1_epi16(0);
								c3 = _mm_set1_epi16(1);
								c4 = _mm_set1_epi16(2);

								off0 = _mm_set1_epi16((pel_t)saoBlkParam->offset[0]);
								off1 = _mm_set1_epi16((pel_t)saoBlkParam->offset[1]);
								off2 = _mm_set1_epi16((pel_t)saoBlkParam->offset[2]);
								off3 = _mm_set1_epi16((pel_t)saoBlkParam->offset[3]);
								off4 = _mm_set1_epi16((pel_t)saoBlkParam->offset[4]);

								for (y = 0; y < lcu_pix_height; y++) {
									//diff = src[start_x] - src[start_x - 1];
									//leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
									s0 = _mm_loadu_si128((__m128i*)&src[start_x - 1]);
									s1 = _mm_srli_si128(s0, 2);
									s2 = _mm_srli_si128(s0, 4);

									t3 = _mm_min_epu16(s0, s1);
									t1 = _mm_cmpeq_epi16(t3, s0);
									t2 = _mm_cmpeq_epi16(t3, s1);
									t0 = _mm_subs_epi16(t2, t1); //leftsign

									t3 = _mm_min_epu16(s1, s2);
									t1 = _mm_cmpeq_epi16(t3, s1);
									t2 = _mm_cmpeq_epi16(t3, s2);
									t3 = _mm_subs_epi16(t1, t2); //rightsign

									etype = _mm_adds_epi16(t0, t3); //edgetype

									t0 = _mm_cmpeq_epi16(etype, c0);
									t1 = _mm_cmpeq_epi16(etype, c1);
									t2 = _mm_cmpeq_epi16(etype, c2);
									t3 = _mm_cmpeq_epi16(etype, c3);
									t4 = _mm_cmpeq_epi16(etype, c4);

									t0 = _mm_and_si128(t0, off0);
									t1 = _mm_and_si128(t1, off1);
									t2 = _mm_and_si128(t2, off2);
									t3 = _mm_and_si128(t3, off3);
									t4 = _mm_and_si128(t4, off4);

									t0 = _mm_adds_epi16(t0, t1);
									t2 = _mm_adds_epi16(t2, t3);
									t0 = _mm_adds_epi16(t0, t4);
									t0 = _mm_adds_epi16(t0, t2);//get offset

									t1 = _mm_adds_epi16(t0, s1);
									t1 = _mm_min_epi16(t1, max_val);
									t1 = _mm_max_epi16(t1, min_val);
                                    _mm_maskmoveu_si128(t1, mask, (char_t*)(dst + start_x));

									dst += i_dst;
									src += i_src;
								}
							}
							else{
								c0 = _mm_set1_epi16(-2);
								c1 = _mm_set1_epi16(-1);
								c2 = _mm_set1_epi16(0);
								c3 = _mm_set1_epi16(1);
								c4 = _mm_set1_epi16(2);

								off0 = _mm_set1_epi16((pel_t)saoBlkParam->offset[0]);
								off1 = _mm_set1_epi16((pel_t)saoBlkParam->offset[1]);
								off2 = _mm_set1_epi16((pel_t)saoBlkParam->offset[2]);
								off3 = _mm_set1_epi16((pel_t)saoBlkParam->offset[3]);
								off4 = _mm_set1_epi16((pel_t)saoBlkParam->offset[4]);

								for (y = 0; y < lcu_pix_height; y++) {
									//diff = src[start_x] - src[start_x - 1];
									//leftsign = diff > 0 ? 1 : (diff < 0 ? -1 : 0);
									for (x = start_x; x < end_x; x += 8) {
										s0 = _mm_loadu_si128((__m128i*)&src[x - 1]);
										s1 = _mm_loadu_si128((__m128i*)&src[x]);
										s2 = _mm_loadu_si128((__m128i*)&src[x + 1]);

										t3 = _mm_min_epu16(s0, s1);
										t1 = _mm_cmpeq_epi16(t3, s0);
										t2 = _mm_cmpeq_epi16(t3, s1);
										t0 = _mm_subs_epi16(t2, t1); //leftsign

										t3 = _mm_min_epu16(s1, s2);
										t1 = _mm_cmpeq_epi16(t3, s1);
										t2 = _mm_cmpeq_epi16(t3, s2);
										t3 = _mm_subs_epi16(t1, t2); //rightsign

										etype = _mm_adds_epi16(t0, t3); //edgetype

										t0 = _mm_cmpeq_epi16(etype, c0);
										t1 = _mm_cmpeq_epi16(etype, c1);
										t2 = _mm_cmpeq_epi16(etype, c2);
										t3 = _mm_cmpeq_epi16(etype, c3);
										t4 = _mm_cmpeq_epi16(etype, c4);

										t0 = _mm_and_si128(t0, off0);
										t1 = _mm_and_si128(t1, off1);
										t2 = _mm_and_si128(t2, off2);
										t3 = _mm_and_si128(t3, off3);
										t4 = _mm_and_si128(t4, off4);

										t0 = _mm_adds_epi16(t0, t1);
										t2 = _mm_adds_epi16(t2, t3);
										t0 = _mm_adds_epi16(t0, t4);
										t0 = _mm_adds_epi16(t0, t2);//get offset

										t1 = _mm_adds_epi16(t0, s1);
										t1 = _mm_min_epi16(t1, max_val);
										t1 = _mm_max_epi16(t1, min_val);

										if (x != end_x_8){
											_mm_storeu_si128((__m128i*)(dst + x), t1);
										}
										else{
											_mm_maskmoveu_si128(t1, mask, (char_t*)(dst + x));
											break;
										}
									}
									dst += i_dst;
									src += i_src;
								}
							}
	}
		break;
	case SAO_TYPE_EO_90: {
							 int end_x_8 = lcu_pix_width - 7;

							 start_y = smb_available_up ? 0 : 1;
							 end_y = smb_available_down ? lcu_pix_height : (lcu_pix_height - 1);

							 dst += start_y * i_dst;
							 src += start_y * i_src;

							 if (lcu_pix_width == 4){
								 mask = _mm_set_epi32(0, 0, -1, -1);
								 c0 = _mm_set1_epi16(-2);
								 c1 = _mm_set1_epi16(-1);
								 c2 = _mm_set1_epi16(0);
								 c3 = _mm_set1_epi16(1);
								 c4 = _mm_set1_epi16(2);

								 off0 = _mm_set1_epi16((pel_t)saoBlkParam->offset[0]);
								 off1 = _mm_set1_epi16((pel_t)saoBlkParam->offset[1]);
								 off2 = _mm_set1_epi16((pel_t)saoBlkParam->offset[2]);
								 off3 = _mm_set1_epi16((pel_t)saoBlkParam->offset[3]);
								 off4 = _mm_set1_epi16((pel_t)saoBlkParam->offset[4]);

								 for (y = start_y; y < end_y; y++) {
									 s0 = _mm_loadu_si128((__m128i*)(src - i_src));
									 s1 = _mm_loadu_si128((__m128i*)src);
									 s2 = _mm_loadu_si128((__m128i*)(src + i_src));

									 t3 = _mm_min_epu16(s0, s1);
									 t1 = _mm_cmpeq_epi16(t3, s0);
									 t2 = _mm_cmpeq_epi16(t3, s1);
									 t0 = _mm_subs_epi16(t2, t1); //upsign

									 t3 = _mm_min_epu16(s1, s2);
									 t1 = _mm_cmpeq_epi16(t3, s1);
									 t2 = _mm_cmpeq_epi16(t3, s2);
									 t3 = _mm_subs_epi16(t1, t2); //downsign

									 etype = _mm_adds_epi16(t0, t3); //edgetype

									 t0 = _mm_cmpeq_epi16(etype, c0);
									 t1 = _mm_cmpeq_epi16(etype, c1);
									 t2 = _mm_cmpeq_epi16(etype, c2);
									 t3 = _mm_cmpeq_epi16(etype, c3);
									 t4 = _mm_cmpeq_epi16(etype, c4);

									 t0 = _mm_and_si128(t0, off0);
									 t1 = _mm_and_si128(t1, off1);
									 t2 = _mm_and_si128(t2, off2);
									 t3 = _mm_and_si128(t3, off3);
									 t4 = _mm_and_si128(t4, off4);

									 t0 = _mm_adds_epi16(t0, t1);
									 t2 = _mm_adds_epi16(t2, t3);
									 t0 = _mm_adds_epi16(t0, t4);
									 t0 = _mm_adds_epi16(t0, t2);//get offset

									 //add 8 nums once for possible overflow
									 t1 = _mm_adds_epi16(t0, s1);
									 t1 = _mm_min_epi16(t1, max_val);
									 t1 = _mm_max_epi16(t1, min_val);

									 _mm_maskmoveu_si128(t1, mask, (char_t*)(dst));

									 dst += i_dst;
									 src += i_src;
								 }
							 }
							 else{
								 c0 = _mm_set1_epi16(-2);
								 c1 = _mm_set1_epi16(-1);
								 c2 = _mm_set1_epi16(0);
								 c3 = _mm_set1_epi16(1);
								 c4 = _mm_set1_epi16(2);

								 off0 = _mm_set1_epi16((pel_t)saoBlkParam->offset[0]);
								 off1 = _mm_set1_epi16((pel_t)saoBlkParam->offset[1]);
								 off2 = _mm_set1_epi16((pel_t)saoBlkParam->offset[2]);
								 off3 = _mm_set1_epi16((pel_t)saoBlkParam->offset[3]);
								 off4 = _mm_set1_epi16((pel_t)saoBlkParam->offset[4]);

								 if (lcu_pix_width & 0x07){
									 mask = _mm_set_epi32(0, 0, -1, -1);

									 for (y = start_y; y < end_y; y++) {
										 for (x = 0; x < lcu_pix_width; x += 8) {
											 s0 = _mm_loadu_si128((__m128i*)&src[x - i_src]);
											 s1 = _mm_loadu_si128((__m128i*)&src[x]);
											 s2 = _mm_loadu_si128((__m128i*)&src[x + i_src]);

											 t3 = _mm_min_epu16(s0, s1);
											 t1 = _mm_cmpeq_epi16(t3, s0);
											 t2 = _mm_cmpeq_epi16(t3, s1);
											 t0 = _mm_subs_epi16(t2, t1); //upsign

											 t3 = _mm_min_epu16(s1, s2);
											 t1 = _mm_cmpeq_epi16(t3, s1);
											 t2 = _mm_cmpeq_epi16(t3, s2);
											 t3 = _mm_subs_epi16(t1, t2); //downsign

											 etype = _mm_adds_epi16(t0, t3); //edgetype

											 t0 = _mm_cmpeq_epi16(etype, c0);
											 t1 = _mm_cmpeq_epi16(etype, c1);
											 t2 = _mm_cmpeq_epi16(etype, c2);
											 t3 = _mm_cmpeq_epi16(etype, c3);
											 t4 = _mm_cmpeq_epi16(etype, c4);

											 t0 = _mm_and_si128(t0, off0);
											 t1 = _mm_and_si128(t1, off1);
											 t2 = _mm_and_si128(t2, off2);
											 t3 = _mm_and_si128(t3, off3);
											 t4 = _mm_and_si128(t4, off4);

											 t0 = _mm_adds_epi16(t0, t1);
											 t2 = _mm_adds_epi16(t2, t3);
											 t0 = _mm_adds_epi16(t0, t4);
											 t0 = _mm_adds_epi16(t0, t2);//get offset

											 t1 = _mm_adds_epi16(t0, s1);
											 t1 = _mm_min_epi16(t1, max_val);
											 t1 = _mm_max_epi16(t1, min_val);

											 if (x < end_x_8){
												 _mm_storeu_si128((__m128i*)(dst + x), t1);
											 }
											 else{
												 _mm_maskmoveu_si128(t1, mask, (char_t*)(dst + x));
												 break;
											 }
										 }
										 dst += i_dst;
										 src += i_src;
									 }
								 }
								 else{
									 for (y = start_y; y < end_y; y++) {
										 for (x = 0; x < lcu_pix_width; x += 8) {
											 s0 = _mm_loadu_si128((__m128i*)&src[x - i_src]);
											 s1 = _mm_loadu_si128((__m128i*)&src[x]);
											 s2 = _mm_loadu_si128((__m128i*)&src[x + i_src]);

											 t3 = _mm_min_epu16(s0, s1);
											 t1 = _mm_cmpeq_epi16(t3, s0);
											 t2 = _mm_cmpeq_epi16(t3, s1);
											 t0 = _mm_subs_epi16(t2, t1); //upsign

											 t3 = _mm_min_epu16(s1, s2);
											 t1 = _mm_cmpeq_epi16(t3, s1);
											 t2 = _mm_cmpeq_epi16(t3, s2);
											 t3 = _mm_subs_epi16(t1, t2); //downsign

											 etype = _mm_adds_epi16(t0, t3); //edgetype

											 t0 = _mm_cmpeq_epi16(etype, c0);
											 t1 = _mm_cmpeq_epi16(etype, c1);
											 t2 = _mm_cmpeq_epi16(etype, c2);
											 t3 = _mm_cmpeq_epi16(etype, c3);
											 t4 = _mm_cmpeq_epi16(etype, c4);

											 t0 = _mm_and_si128(t0, off0);
											 t1 = _mm_and_si128(t1, off1);
											 t2 = _mm_and_si128(t2, off2);
											 t3 = _mm_and_si128(t3, off3);
											 t4 = _mm_and_si128(t4, off4);

											 t0 = _mm_adds_epi16(t0, t1);
											 t2 = _mm_adds_epi16(t2, t3);
											 t0 = _mm_adds_epi16(t0, t4);
											 t0 = _mm_adds_epi16(t0, t2);//get offset

											 t1 = _mm_adds_epi16(t0, s1);
											 t1 = _mm_min_epi16(t1, max_val);
											 t1 = _mm_max_epi16(t1, min_val);

											 _mm_storeu_si128((__m128i*)(dst + x), t1);
										 }
										 dst += i_dst;
										 src += i_src;
									 }
								 }
							 }
	}
		break;
	case SAO_TYPE_EO_135: {
							  __m128i mask_r0, mask_r, mask_rn;
							  int end_x_r0_8, end_x_r_8, end_x_rn_8;

							  c0 = _mm_set1_epi16(-2);
							  c1 = _mm_set1_epi16(-1);
							  c2 = _mm_set1_epi16(0);
							  c3 = _mm_set1_epi16(1);
							  c4 = _mm_set1_epi16(2);

							  off0 = _mm_set1_epi16((pel_t)saoBlkParam->offset[0]);
							  off1 = _mm_set1_epi16((pel_t)saoBlkParam->offset[1]);
							  off2 = _mm_set1_epi16((pel_t)saoBlkParam->offset[2]);
							  off3 = _mm_set1_epi16((pel_t)saoBlkParam->offset[3]);
							  off4 = _mm_set1_epi16((pel_t)saoBlkParam->offset[4]);

							  start_x_r0 = smb_available_upleft ? 0 : 1;
							  end_x_r0 = smb_available_up ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;
							  start_x_r = smb_available_left ? 0 : 1;
							  end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
							  start_x_rn = smb_available_down ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
							  end_x_rn = smb_available_rightdwon ? lcu_pix_width : (lcu_pix_width - 1);

							  end_x_r0_8 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x07);
							  end_x_r_8 = end_x_r - ((end_x_r - start_x_r) & 0x07);
							  end_x_rn_8 = end_x_rn - ((end_x_rn - start_x_rn) & 0x07);


							  //first row
							  for (x = start_x_r0; x < end_x_r0; x += 8) {
								  s0 = _mm_loadu_si128((__m128i*)&src[x - i_src - 1]);
								  s1 = _mm_loadu_si128((__m128i*)&src[x]);
								  s2 = _mm_loadu_si128((__m128i*)&src[x + i_src + 1]);

								  t3 = _mm_min_epu16(s0, s1);
								  t1 = _mm_cmpeq_epi16(t3, s0);
								  t2 = _mm_cmpeq_epi16(t3, s1);
								  t0 = _mm_subs_epi16(t2, t1); //upsign

								  t3 = _mm_min_epu16(s1, s2);
								  t1 = _mm_cmpeq_epi16(t3, s1);
								  t2 = _mm_cmpeq_epi16(t3, s2);
								  t3 = _mm_subs_epi16(t1, t2); //downsign

								  etype = _mm_adds_epi16(t0, t3); //edgetype

								  t0 = _mm_cmpeq_epi16(etype, c0);
								  t1 = _mm_cmpeq_epi16(etype, c1);
								  t2 = _mm_cmpeq_epi16(etype, c2);
								  t3 = _mm_cmpeq_epi16(etype, c3);
								  t4 = _mm_cmpeq_epi16(etype, c4);

								  t0 = _mm_and_si128(t0, off0);
								  t1 = _mm_and_si128(t1, off1);
								  t2 = _mm_and_si128(t2, off2);
								  t3 = _mm_and_si128(t3, off3);
								  t4 = _mm_and_si128(t4, off4);

								  t0 = _mm_adds_epi16(t0, t1);
								  t2 = _mm_adds_epi16(t2, t3);
								  t0 = _mm_adds_epi16(t0, t4);
								  t0 = _mm_adds_epi16(t0, t2);//get offset


								  t1 = _mm_adds_epi16(t0, s1);
								  t1 = _mm_min_epi16(t1, max_val);
								  t1 = _mm_max_epi16(t1, min_val);

								  if (x != end_x_r0_8){
									  _mm_storeu_si128((__m128i*)(dst + x), t1);
								  }
								  else{
									  mask_r0 = _mm_load_si128((__m128i*)(intrinsic_mask_10bit[end_x_r0 - end_x_r0_8 - 1]));
									  _mm_maskmoveu_si128(t1, mask_r0, (char_t*)(dst + x));
									  break;
								  }
							  }
							  dst += i_dst;
							  src += i_src;

							  mask_r = _mm_load_si128((__m128i*)(intrinsic_mask_10bit[end_x_r - end_x_r_8 - 1]));
							  //middle rows
							  for (y = 1; y < lcu_pix_height - 1; y++) {
								  for (x = start_x_r; x < end_x_r; x += 8) {
									  s0 = _mm_loadu_si128((__m128i*)&src[x - i_src - 1]);
									  s1 = _mm_loadu_si128((__m128i*)&src[x]);
									  s2 = _mm_loadu_si128((__m128i*)&src[x + i_src + 1]);

									  t3 = _mm_min_epu16(s0, s1);
									  t1 = _mm_cmpeq_epi16(t3, s0);
									  t2 = _mm_cmpeq_epi16(t3, s1);
									  t0 = _mm_subs_epi16(t2, t1); //upsign

									  t3 = _mm_min_epu16(s1, s2);
									  t1 = _mm_cmpeq_epi16(t3, s1);
									  t2 = _mm_cmpeq_epi16(t3, s2);
									  t3 = _mm_subs_epi16(t1, t2); //downsign

									  etype = _mm_adds_epi16(t0, t3); //edgetype

									  t0 = _mm_cmpeq_epi16(etype, c0);
									  t1 = _mm_cmpeq_epi16(etype, c1);
									  t2 = _mm_cmpeq_epi16(etype, c2);
									  t3 = _mm_cmpeq_epi16(etype, c3);
									  t4 = _mm_cmpeq_epi16(etype, c4);

									  t0 = _mm_and_si128(t0, off0);
									  t1 = _mm_and_si128(t1, off1);
									  t2 = _mm_and_si128(t2, off2);
									  t3 = _mm_and_si128(t3, off3);
									  t4 = _mm_and_si128(t4, off4);

									  t0 = _mm_adds_epi16(t0, t1);
									  t2 = _mm_adds_epi16(t2, t3);
									  t0 = _mm_adds_epi16(t0, t4);
									  t0 = _mm_adds_epi16(t0, t2);//get offset

									  t1 = _mm_adds_epi16(t0, s1);
									  t1 = _mm_min_epi16(t1, max_val);
									  t1 = _mm_max_epi16(t1, min_val);

									  if (x != end_x_r_8){
										  _mm_storeu_si128((__m128i*)(dst + x), t1);
									  }
									  else{
										  _mm_maskmoveu_si128(t1, mask_r, (char_t*)(dst + x));
										  break;
									  }
								  }
								  dst += i_dst;
								  src += i_src;
							  }
							  //last row
							  for (x = start_x_rn; x < end_x_rn; x += 8) {
								  s0 = _mm_loadu_si128((__m128i*)&src[x - i_src - 1]);
								  s1 = _mm_loadu_si128((__m128i*)&src[x]);
								  s2 = _mm_loadu_si128((__m128i*)&src[x + i_src + 1]);

								  t3 = _mm_min_epu16(s0, s1);
								  t1 = _mm_cmpeq_epi16(t3, s0);
								  t2 = _mm_cmpeq_epi16(t3, s1);
								  t0 = _mm_subs_epi16(t2, t1); //upsign

								  t3 = _mm_min_epu16(s1, s2);
								  t1 = _mm_cmpeq_epi16(t3, s1);
								  t2 = _mm_cmpeq_epi16(t3, s2);
								  t3 = _mm_subs_epi16(t1, t2); //downsign

								  etype = _mm_adds_epi16(t0, t3); //edgetype

								  t0 = _mm_cmpeq_epi16(etype, c0);
								  t1 = _mm_cmpeq_epi16(etype, c1);
								  t2 = _mm_cmpeq_epi16(etype, c2);
								  t3 = _mm_cmpeq_epi16(etype, c3);
								  t4 = _mm_cmpeq_epi16(etype, c4);

								  t0 = _mm_and_si128(t0, off0);
								  t1 = _mm_and_si128(t1, off1);
								  t2 = _mm_and_si128(t2, off2);
								  t3 = _mm_and_si128(t3, off3);
								  t4 = _mm_and_si128(t4, off4);

								  t0 = _mm_adds_epi16(t0, t1);
								  t2 = _mm_adds_epi16(t2, t3);
								  t0 = _mm_adds_epi16(t0, t4);
								  t0 = _mm_adds_epi16(t0, t2);//get offset

								  t1 = _mm_adds_epi16(t0, s1);
								  t1 = _mm_min_epi16(t1, max_val);
								  t1 = _mm_max_epi16(t1, min_val);

								  if (x != end_x_rn_8){
									  _mm_storeu_si128((__m128i*)(dst + x), t1);
								  }
								  else{
									  mask_rn = _mm_load_si128((__m128i*)(intrinsic_mask_10bit[end_x_rn - end_x_rn_8 - 1]));
									  _mm_maskmoveu_si128(t1, mask_rn, (char_t*)(dst + x));
									  break;
								  }
							  }
	}
		break;
	case SAO_TYPE_EO_45: {
							 __m128i mask_r0, mask_r, mask_rn;
							 int end_x_r0_8, end_x_r_8, end_x_rn_8;

							 c0 = _mm_set1_epi16(-2);
							 c1 = _mm_set1_epi16(-1);
							 c2 = _mm_set1_epi16(0);
							 c3 = _mm_set1_epi16(1);
							 c4 = _mm_set1_epi16(2);

							 off0 = _mm_set1_epi16((pel_t)saoBlkParam->offset[0]);
							 off1 = _mm_set1_epi16((pel_t)saoBlkParam->offset[1]);
							 off2 = _mm_set1_epi16((pel_t)saoBlkParam->offset[2]);
							 off3 = _mm_set1_epi16((pel_t)saoBlkParam->offset[3]);
							 off4 = _mm_set1_epi16((pel_t)saoBlkParam->offset[4]);

							 start_x_r0 = smb_available_up ? (smb_available_left ? 0 : 1) : (lcu_pix_width - 1);
							 end_x_r0 = smb_available_upright ? lcu_pix_width : (lcu_pix_width - 1);
							 start_x_r = smb_available_left ? 0 : 1;
							 end_x_r = smb_available_right ? lcu_pix_width : (lcu_pix_width - 1);
							 start_x_rn = smb_available_leftdown ? 0 : 1;
							 end_x_rn = smb_available_down ? (smb_available_right ? lcu_pix_width : (lcu_pix_width - 1)) : 1;

							 end_x_r0_8 = end_x_r0 - ((end_x_r0 - start_x_r0) & 0x07);
							 end_x_r_8 = end_x_r - ((end_x_r - start_x_r) & 0x07);
							 end_x_rn_8 = end_x_rn - ((end_x_rn - start_x_rn) & 0x07);


							 //first row
							 for (x = start_x_r0; x < end_x_r0; x += 8) {
								 s0 = _mm_loadu_si128((__m128i*)&src[x - i_src + 1]);
								 s1 = _mm_loadu_si128((__m128i*)&src[x]);
								 s2 = _mm_loadu_si128((__m128i*)&src[x + i_src - 1]);

								 t3 = _mm_min_epu16(s0, s1);
								 t1 = _mm_cmpeq_epi16(t3, s0);
								 t2 = _mm_cmpeq_epi16(t3, s1);
								 t0 = _mm_subs_epi16(t2, t1); //upsign

								 t3 = _mm_min_epu16(s1, s2);
								 t1 = _mm_cmpeq_epi16(t3, s1);
								 t2 = _mm_cmpeq_epi16(t3, s2);
								 t3 = _mm_subs_epi16(t1, t2); //downsign

								 etype = _mm_adds_epi16(t0, t3); //edgetype

								 t0 = _mm_cmpeq_epi16(etype, c0);
								 t1 = _mm_cmpeq_epi16(etype, c1);
								 t2 = _mm_cmpeq_epi16(etype, c2);
								 t3 = _mm_cmpeq_epi16(etype, c3);
								 t4 = _mm_cmpeq_epi16(etype, c4);

								 t0 = _mm_and_si128(t0, off0);
								 t1 = _mm_and_si128(t1, off1);
								 t2 = _mm_and_si128(t2, off2);
								 t3 = _mm_and_si128(t3, off3);
								 t4 = _mm_and_si128(t4, off4);

								 t0 = _mm_adds_epi16(t0, t1);
								 t2 = _mm_adds_epi16(t2, t3);
								 t0 = _mm_adds_epi16(t0, t4);
								 t0 = _mm_adds_epi16(t0, t2);//get offset

								 t1 = _mm_adds_epi16(t0, s1);
								 t1 = _mm_min_epi16(t1, max_val);
								 t1 = _mm_max_epi16(t1, min_val);

								 if (x != end_x_r0_8){
									 _mm_storeu_si128((__m128i*)(dst + x), t1);
								 }
								 else{
									 mask_r0 = _mm_load_si128((__m128i*)(intrinsic_mask_10bit[end_x_r0 - end_x_r0_8 - 1]));
									 _mm_maskmoveu_si128(t1, mask_r0, (char_t*)(dst + x));
									 break;
								 }
							 }
							 dst += i_dst;
							 src += i_src;

							 mask_r = _mm_load_si128((__m128i*)(intrinsic_mask_10bit[end_x_r - end_x_r_8 - 1]));
							 //middle rows
							 for (y = 1; y < lcu_pix_height - 1; y++) {
								 for (x = start_x_r; x < end_x_r; x += 8) {
									 s0 = _mm_loadu_si128((__m128i*)&src[x - i_src + 1]);
									 s1 = _mm_loadu_si128((__m128i*)&src[x]);
									 s2 = _mm_loadu_si128((__m128i*)&src[x + i_src - 1]);

									 t3 = _mm_min_epu16(s0, s1);
									 t1 = _mm_cmpeq_epi16(t3, s0);
									 t2 = _mm_cmpeq_epi16(t3, s1);
									 t0 = _mm_subs_epi16(t2, t1); //upsign

									 t3 = _mm_min_epu16(s1, s2);
									 t1 = _mm_cmpeq_epi16(t3, s1);
									 t2 = _mm_cmpeq_epi16(t3, s2);
									 t3 = _mm_subs_epi16(t1, t2); //downsign

									 etype = _mm_adds_epi16(t0, t3); //edgetype

									 t0 = _mm_cmpeq_epi16(etype, c0);
									 t1 = _mm_cmpeq_epi16(etype, c1);
									 t2 = _mm_cmpeq_epi16(etype, c2);
									 t3 = _mm_cmpeq_epi16(etype, c3);
									 t4 = _mm_cmpeq_epi16(etype, c4);

									 t0 = _mm_and_si128(t0, off0);
									 t1 = _mm_and_si128(t1, off1);
									 t2 = _mm_and_si128(t2, off2);
									 t3 = _mm_and_si128(t3, off3);
									 t4 = _mm_and_si128(t4, off4);

									 t0 = _mm_adds_epi16(t0, t1);
									 t2 = _mm_adds_epi16(t2, t3);
									 t0 = _mm_adds_epi16(t0, t4);
									 t0 = _mm_adds_epi16(t0, t2);//get offset

									 t1 = _mm_adds_epi16(t0, s1);
									 t1 = _mm_min_epi16(t1, max_val);
									 t1 = _mm_max_epi16(t1, min_val);

									 if (x != end_x_r_8){
										 _mm_storeu_si128((__m128i*)(dst + x), t1);
									 }
									 else{
										 _mm_maskmoveu_si128(t1, mask_r, (char_t*)(dst + x));
										 break;
									 }
								 }
								 dst += i_dst;
								 src += i_src;
							 }
							 for (x = start_x_rn; x < end_x_rn; x += 8) {
								 s0 = _mm_loadu_si128((__m128i*)&src[x - i_src + 1]);
								 s1 = _mm_loadu_si128((__m128i*)&src[x]);
								 s2 = _mm_loadu_si128((__m128i*)&src[x + i_src - 1]);

								 t3 = _mm_min_epu16(s0, s1);
								 t1 = _mm_cmpeq_epi16(t3, s0);
								 t2 = _mm_cmpeq_epi16(t3, s1);
								 t0 = _mm_subs_epi16(t2, t1); //upsign

								 t3 = _mm_min_epu16(s1, s2);
								 t1 = _mm_cmpeq_epi16(t3, s1);
								 t2 = _mm_cmpeq_epi16(t3, s2);
								 t3 = _mm_subs_epi16(t1, t2); //downsign

								 etype = _mm_adds_epi16(t0, t3); //edgetype

								 t0 = _mm_cmpeq_epi16(etype, c0);
								 t1 = _mm_cmpeq_epi16(etype, c1);
								 t2 = _mm_cmpeq_epi16(etype, c2);
								 t3 = _mm_cmpeq_epi16(etype, c3);
								 t4 = _mm_cmpeq_epi16(etype, c4);

								 t0 = _mm_and_si128(t0, off0);
								 t1 = _mm_and_si128(t1, off1);
								 t2 = _mm_and_si128(t2, off2);
								 t3 = _mm_and_si128(t3, off3);
								 t4 = _mm_and_si128(t4, off4);

								 t0 = _mm_adds_epi16(t0, t1);
								 t2 = _mm_adds_epi16(t2, t3);
								 t0 = _mm_adds_epi16(t0, t4);
								 t0 = _mm_adds_epi16(t0, t2);//get offset

								 t1 = _mm_adds_epi16(t0, s1);
								 t1 = _mm_min_epi16(t1, max_val);
								 t1 = _mm_max_epi16(t1, min_val);

								 if (x != end_x_rn_8){
									 _mm_storeu_si128((__m128i*)(dst + x), t1);
								 }
								 else{
									 mask_rn = _mm_load_si128((__m128i*)(intrinsic_mask_10bit[end_x_rn - end_x_rn_8 - 1]));
									 _mm_maskmoveu_si128(t1, mask_rn, (char_t*)(dst + x));
									 break;
								 }
							 }
	}
		break;
	default: {
				 printf("Not a supported SAO types\n");
				 assert(0);
				 exit(-1);
	}
	}
}

