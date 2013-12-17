/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * MD4 message-digest algorithm
 * $Id: md4c.c 1.2 Sat, 18 Oct 2003 00:17:54 +1000 abo $
 * 
 * Copyright (C) 2000, 2001 by Martin Pool <mbp@samba.org>
 * Copyright (C) 1997-1999 by Andrew Tridgell
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* MD4 message digest algorithm.
 *
 * This was originally written by Andrew Tridgell for use in Samba. 
 * It was then modified by;
 * 
 * 2002-06-xx: Robert Weber <robert.weber@Colorado.edu> 
 *   optimisations and fixed >512M support.
 * 
 * 2002-06-27: Donovan Baarda <abo@minkirri.apana.org.au>
 *   further optimisations and cleanups.
 * 
 * 2003-01-30: Donovan Baarda <abo@minkirri.apana.org.au>
 *   refactored to RSA API.
 */

#include "md4.h"
#include <string.h>

#ifdef __GNUC__
#define HAVE_UINT64 1
#endif

/** padding data used for finalising */
static unsigned char PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void MD4Transform(MD4_CTX *md, u_int32_t const X[16]);


/**
 * These next routines are necessary because MD4 is specified in terms of
 * little-endian int32s, but we have a byte buffer.  On little-endian
 * platforms, we can just use the buffer pointer directly.
 **/
#if __BYTE_ORDER != __LITTLE_ENDIAN
void MD4Block(MD4_CTX *md, const unsigned char *in)
{
    u_int32_t        M[16];
    int i;

    for (i=0; i<16; i++) {
        M[i] = (in[3] << 24) | (in[2] << 16) | (in[1] << 8) | in[0];
        in += 4;
    }
    MD4Transform(md, M);
}

#define COPY2(out,x) { *(out) = (x); *(out+1) = (x) >> 8; }
#define COPY4(out,x) COPY2(out,x); COPY2(out+2,(x) >> 16);
#define COPY8(out,x) COPY4(out,x); COPY4(out+4,(x) >> 32);

#else /* __BYTE_ORDER != __LITTLE_ENDIAN */

#define MD4Block(md,p) MD4Transform(md, (u_int32_t *)p)
#define COPY4(out,x) { *(u_int32_t *)(out) = x; }
#define COPY8(out,x) { *(u_int64_t *)(out) = x; }

#endif /* __BYTE_ORDER != __LITTLE_ENDIAN */


/*
 * Initialise the MD4 accumulator.
 */
void   MD4Init(MD4_CTX *md)
{
    memset(md, 0, sizeof(*md));
    md->A = 0x67452301;
    md->B = 0xefcdab89;
    md->C = 0x98badcfe;
    md->D = 0x10325476;
#if HAVE_UINT64
    md->bytes = 0;
#else 
    md->bytesHi = md->bytesLo = 0;
#endif
}


/**
 * Feed some data into the MD4 accumulator.
 */
void   MD4Update(MD4_CTX *md, const unsigned char *in, unsigned int n)
{
    unsigned int buffer_len, buffer_gap;

    /* get buffer_len and increment bytes */
#ifdef HAVE_UINT64
    buffer_len = md->bytes & 0x3F;
    md->bytes += n;
#else /* HAVE_UINT64 */
    buffer_len = md->bytesLo & 0x3F;
    if ((md->bytesLo += n) < n) 
	md->bytesHi++;
#endif /* HAVE_UINT64 */

    /* If there's any leftover data in the buffer, then first we have
     * to make it up to a whole block to process it.  */
    if (buffer_len) {
        buffer_gap = 64 - buffer_len;
        if (buffer_gap <= n) {
            memcpy(&md->buffer[buffer_len], in, buffer_gap);
            MD4Block(md, md->buffer);
            in += buffer_gap;
            n -= buffer_gap;
            buffer_len = 0;
        }
    }
    /* process complete blocks of input */
    while (n >= 64) {
        MD4Block(md, in);
        in += 64;
        n -= 64;
    }
    /* Put remaining bytes onto buffer*/
    if (n) {
        memcpy(&md->buffer[buffer_len], in, n);
    }
}


/**
 * Finalise the MD4 accumulator and return the digest.
 **/
void   MD4Final(unsigned char out[MD4_HASHBYTES], MD4_CTX *md)
{
#ifdef HAVE_UINT64
    u_int64_t         b;
#else /* HAVE_UINT64 */
    u_int32_t        b[2];
#endif /* HAVE_UINT64 */
    unsigned char   buf[8];
    unsigned int    buffer_len, pad_len;

    /* get buffer_len and convert the byte count into a bit count buffer */
#ifdef HAVE_UINT64
    buffer_len = md->bytes & 0x3F;
    b = md->bytes << 3;
    COPY8(buf, b);
#else /* HAVE_UINT64 */
    buffer_len = md->bytesLo & 0x3F;
    b[0] = md->bytesLo << 3;
    b[1] = ((md->bytesHi << 3) | (md->bytesLo >> 29));
    COPY4(buf, b[0]);
    COPY4(buf + 4, b[1]);
#endif /* HAVE_UINT64 */

    /* calculate length, process the padding data and bit count */
    pad_len=(buffer_len <56) ? (56 - buffer_len) : (120 - buffer_len);
    MD4Update(md,PADDING,pad_len);
    MD4Update(md,buf,8);

    /* copy A, B, C, D into out */
    COPY4(out, md->A);
    COPY4(out + 4, md->B);
    COPY4(out + 8, md->C);
    COPY4(out + 12, md->D);
}


#define F(X,Y,Z) (((X)&(Y)) | ((~(X))&(Z)))
#define G(X,Y,Z) (((X)&(Y)) | ((X)&(Z)) | ((Y)&(Z)))
#define H(X,Y,Z) ((X)^(Y)^(Z))
#define lshift(x,s) (((x)<<(s)) | ((x)>>(32-(s))))

#define ROUND1(a,b,c,d,k,s) a = lshift(a + F(b,c,d) + X[k], s)
#define ROUND2(a,b,c,d,k,s) a = lshift(a + G(b,c,d) + X[k] + 0x5A827999,s)
#define ROUND3(a,b,c,d,k,s) a = lshift(a + H(b,c,d) + X[k] + 0x6ED9EBA1,s)

/**
 * Update an MD4 accumulator from a 16-int array.
 * 
 * \param X A series of integer, read little-endian from the file.
 **/
void MD4Transform(MD4_CTX *md, u_int32_t const X[16])
{
    u_int32_t        AA, BB, CC, DD;
    u_int32_t        A, B, C, D;

    A = md->A;
    B = md->B;
    C = md->C;
    D = md->D;
    AA = A;
    BB = B;
    CC = C;
    DD = D;

    ROUND1(A, B, C, D, 0, 3);
    ROUND1(D, A, B, C, 1, 7);
    ROUND1(C, D, A, B, 2, 11);
    ROUND1(B, C, D, A, 3, 19);
    ROUND1(A, B, C, D, 4, 3);
    ROUND1(D, A, B, C, 5, 7);
    ROUND1(C, D, A, B, 6, 11);
    ROUND1(B, C, D, A, 7, 19);
    ROUND1(A, B, C, D, 8, 3);
    ROUND1(D, A, B, C, 9, 7);
    ROUND1(C, D, A, B, 10, 11);
    ROUND1(B, C, D, A, 11, 19);
    ROUND1(A, B, C, D, 12, 3);
    ROUND1(D, A, B, C, 13, 7);
    ROUND1(C, D, A, B, 14, 11);
    ROUND1(B, C, D, A, 15, 19);

    ROUND2(A, B, C, D, 0, 3);
    ROUND2(D, A, B, C, 4, 5);
    ROUND2(C, D, A, B, 8, 9);
    ROUND2(B, C, D, A, 12, 13);
    ROUND2(A, B, C, D, 1, 3);
    ROUND2(D, A, B, C, 5, 5);
    ROUND2(C, D, A, B, 9, 9);
    ROUND2(B, C, D, A, 13, 13);
    ROUND2(A, B, C, D, 2, 3);
    ROUND2(D, A, B, C, 6, 5);
    ROUND2(C, D, A, B, 10, 9);
    ROUND2(B, C, D, A, 14, 13);
    ROUND2(A, B, C, D, 3, 3);
    ROUND2(D, A, B, C, 7, 5);
    ROUND2(C, D, A, B, 11, 9);
    ROUND2(B, C, D, A, 15, 13);

    ROUND3(A, B, C, D, 0, 3);
    ROUND3(D, A, B, C, 8, 9);
    ROUND3(C, D, A, B, 4, 11);
    ROUND3(B, C, D, A, 12, 15);
    ROUND3(A, B, C, D, 2, 3);
    ROUND3(D, A, B, C, 10, 9);
    ROUND3(C, D, A, B, 6, 11);
    ROUND3(B, C, D, A, 14, 15);
    ROUND3(A, B, C, D, 1, 3);
    ROUND3(D, A, B, C, 9, 9);
    ROUND3(C, D, A, B, 5, 11);
    ROUND3(B, C, D, A, 13, 15);
    ROUND3(A, B, C, D, 3, 3);
    ROUND3(D, A, B, C, 11, 9);
    ROUND3(C, D, A, B, 7, 11);
    ROUND3(B, C, D, A, 15, 15);

    A += AA;
    B += BB;
    C += CC;
    D += DD;

    md->A = A;
    md->B = B;
    md->C = C;
    md->D = D;
}
