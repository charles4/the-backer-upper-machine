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

#include "rsync.h"
#include "checksum.h"

static void usage(void)
{
    fprintf(stderr, "Usage:  rollsumT1\n");
    exit(2);
}

/*
 * Test driver for rollsum.  
 */
#define COUNT (1024*1024)
#define BLOCKSIZE (1024)    
int main(int argc, char *argv[])
{
    int i;
    char data[BLOCKSIZE];
    unsigned long digest;
    
    if (argc != 1) {
	usage();
    }
    
    for (i=0;i<BLOCKSIZE;i++) {
        data[i]=(char)(i & 0xff);
    }
        
    for (i=0;i<COUNT;i++) {
        digest=rs_calc_weak_sum(data,BLOCKSIZE);
    }
    printf("weak_sum: %i x %i bytes gives %X\n",COUNT,BLOCKSIZE,digest);
}
