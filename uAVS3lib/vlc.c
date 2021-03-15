#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "codingUnit.h"
#include "vlc.h"

/*
*************************************************************************
* Function:UE_V, writes an ue(v) syntax element, returns the length in bits
* Input:
tracestring
the string for the trace file
value
the value to be coded
bitstream
the bit_stream_t the value should be coded into
* Output:
* Return:  Number of bits used by the coded syntax element
* Attention: This function writes always the bit buffer for the progressive scan flag, and
should not be used (or should be modified appropriately) for the interlace crap
When used in the context of the Parameter Sets, this is obviously not a
problem.
*************************************************************************
*/

int ue_v(int value, bit_stream_t *bitstream)
{
    int len, v, suffix_len, inf;

    len = ue_linfo(value, &inf);

    suffix_len = len / 2;
    v = (1 << suffix_len) | (inf & ((1 << suffix_len) - 1));

    writeUVLC2buffer(bitstream, v, len);

    return len;
}

/*
*************************************************************************
* Function:UE_V, writes an ue(v) syntax element, returns the length in bits
* Input:
tracestring
the string for the trace file
value
the value to be coded
bitstream
the bit_stream_t the value should be coded into
* Output:
* Return: Number of bits used by the coded syntax element
* Attention:
This function writes always the bit buffer for the progressive scan flag, and
should not be used (or should be modified appropriately) for the interlace crap
When used in the context of the Parameter Sets, this is obviously not a
problem.
*************************************************************************
*/

int se_v(int value, bit_stream_t *bitstream)
{
    int len, v, suffix_len, inf;

    len = se_linfo(value, &inf);
    
    suffix_len = len / 2;
    v = (1 << suffix_len) | (inf & ((1 << suffix_len) - 1));
 
    writeUVLC2buffer(bitstream, v, len);

    return len;
}


/*
*************************************************************************
* Function:U_V, writes a a n bit fixed length syntax element, returns the length in bits,
* Input:
tracestring
the string for the trace file
value
the value to be coded
bitstream
the bit_stream_t the value should be coded into
* Output:
* Return: Number of bits used by the coded syntax element
* Attention:This function writes always the bit buffer for the progressive scan flag, and
should not be used (or should be modified appropriately) for the interlace crap
When used in the context of the Parameter Sets, this is obviously not a
problem.
*************************************************************************
*/
int u_v(int n, int value, bit_stream_t *bitstream)
{
    writeUVLC2buffer(bitstream, value, n);
    return n;
}

int ue_linfo(int ue, int *info)
{
    int i, nn, len;

    nn = (ue + 1) / 2;

    for (i = 0; i < 16 && nn != 0; i++) {
        nn /= 2;
    }

    len = 2 * i + 1;
    *info = ue + 1 - (int) pow(2, i);
    return len;

}

int se_linfo(int v, int *info)
{

    int i, n, sign, nn, len;

    sign = 0;

    if (v <= 0) {
        sign = 1;
    }

    n = abs(v) << 1;

    //n+1 is the number in the code table.  Based on this we find length and info

    nn = n / 2;

    for (i = 0; i < 16 && nn != 0; i++) {
        nn /= 2;
    }

    len = i * 2 + 1;
    *info = n - (int) pow(2, i) + sign;
    return len;
}


/*
*************************************************************************
* Function:writes UVLC code to the appropriate buffer
* Input:
* Output:
* Return:
* Attention:
*************************************************************************
*/

void  writeUVLC2buffer(bit_stream_t *strm, int val, int len)
{
    int i;
    unsigned int mask = 1 << (len - 1);

    for (i = 0; i < len; i++) {
        strm->byte_buf <<= 1;

        if (val & mask) {
            strm->byte_buf |= 1;
        }

        strm->bits_to_go--;
        mask >>= 1;

        if (strm->bits_to_go == 0) {
            strm->bits_to_go = 8;
            strm->streamBuffer[strm->byte_pos++] = strm->byte_buf;
            strm->byte_buf = 0;
        }
    }
}

