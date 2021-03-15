#ifndef _VLC_H_
#define _VLC_H_

int se_v(int value, bit_stream_t *part);
int ue_v(int value, bit_stream_t *part);
int u_v(int n, int value, bit_stream_t *part);

#define SE_V(flag,v,strm) se_v(v,strm)
#define UE_V(flag,v,strm) ue_v(v,strm)
#define U_V(n,flag,v,strm) u_v(n,v,strm)

void  writeUVLC2buffer(bit_stream_t *strm, int val, int len);

int  ue_linfo(int n, int *info);
int  se_linfo(int n, int *info);

#endif

