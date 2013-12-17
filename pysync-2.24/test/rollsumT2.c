/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 * Copyright (C) 2002 by Donovan Baarda <abo@minkirri.apana.org.au>
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

#include <stdio.h>
#include <stdlib.h>

#include "rollsum.h"

static void usage(void)
{
    fprintf(stderr, "Usage:  rollsumT1\n");
    exit(2);
}

/*
 * Test driver for rollsum.  
 */
#define COUNT (1024*1024)
#define BLOCKSIZE 1024    
int main(int argc, char *argv[])
{
    int i;
    Rollsum sum;
    char c,data[BLOCKSIZE];
    unsigned long digest;
    
    if (argc != 1) {
	usage();
    }
    
    for (i=0;i<BLOCKSIZE;i++) {
        data[i]=(char)(i & 0xff);
    }
        
    RollsumInit(&sum);
    RollsumUpdate(&sum,data,BLOCKSIZE);
    for (i=0;i<(COUNT*BLOCKSIZE);i++) {
        c=data[i % BLOCKSIZE];
        RollsumRotate(&sum,c,c);
        digest=RollsumDigest(&sum);
    }
    printf("rollsum: %i x %i rotate gives %X\n",COUNT,BLOCKSIZE,digest);
}
