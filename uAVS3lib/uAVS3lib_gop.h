#ifndef __uAVS3lib_GOP_H__
#define __uAVS3lib_GOP_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

#if defined(__GNUC__)
#define __USE_GNU
#include <sched.h>
#endif

#include "uAVS3lib.h"

/* ---------------------------------------------------------------------------
 *  function define
 */
uAVS3API void*     __cdecl avs3gop_lib_create(cfg_param_t *input, strm_out_t strm_callbak, rec_out_t rec_callback, void *priv_data, int objs, void **masks);
uAVS3API void      __cdecl avs3gop_lib_encode(void *handle, int bflush, int output_seq_end);
uAVS3API void      __cdecl avs3gop_lib_free(void *handle);
uAVS3API image_t * __cdecl avs3gop_lib_imgbuf(void *handle);
uAVS3API void      __cdecl avs3gop_lib_speed_adjust(void *handle, double adj);

/* ---------------------------------------------------------------------------
 *  function types define
 */
typedef void*    (__cdecl *avs3gop_lib_create_t)(cfg_param_t *input, strm_out_t strm_callbak, rec_out_t rec_callback, void *priv_data, int objs, void **masks);
typedef void     (__cdecl *avs3gop_lib_encode_t)(void *handle, int bflush, int output_seq_end);
typedef void     (__cdecl *avs3gop_lib_free_t)(void *handle);
typedef image_t *(__cdecl *avs3gop_lib_imgbuf_t)(void *handle);
typedef void     (__cdecl *avs3gop_lib_speed_adjust_t)(void *handle, double adj);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif /* __uAVS3lib_GOP_H__ */
