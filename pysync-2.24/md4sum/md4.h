/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * md4.h - header file for md4c.c
 * $Id: md4.h 1.3 Sat, 18 Oct 2003 00:17:54 +1000 abo $
 * 
 * Copyright (C) 2000, 2001 by Martin Pool <mbp@samba.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef _MD4_H_
#define _MD4_H_

#include <sys/types.h>

#define MD4_HASHBYTES  16

typedef struct MD4Context {
  u_int32_t A, B, C, D;     /* state (ABCD) */
#ifdef __GNUC__
  u_int64_t bytes;
#else
  u_int32_t bytesHi, bytesLo;
#endif
  unsigned char buffer[64]; /* input buffer */
} MD4_CTX;

void   MD4Init(MD4_CTX *);
void   MD4Update(MD4_CTX *, const unsigned char *, unsigned int);
void   MD4Final(unsigned char [MD4_HASHBYTES], MD4_CTX *);
char * MD4End(MD4_CTX *, char *);
char * MD4File(const char *, char *);
char * MD4Data(const unsigned char *, unsigned int, char *);

#endif /* _MD4_H_ */
