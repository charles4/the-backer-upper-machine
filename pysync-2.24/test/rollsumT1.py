#!/usr/bin/env python
"""
rollsum test program

     $Id: rollsumT1.py 1.4 Fri, 17 May 2002 02:56:30 +1000 abo $
Author	: Donovan Baarda <abo@minkirri.apana.org.au>
License	: GPL
Download: ftp://minkirri.apana.org.au/pub/python/pysync/

"""

from random import randint
from time import clock
import rollsum

# generate 8K random data
data=''
for i in range(8*1024):
    data=data+chr(randint(0,255))

# time digest
t=clock()
for i in range(80*1024):
    v=rollsum.new(data).digest()
t=clock()-t
print "80K x digest of 8K data gave %s in %8.4f seconds" % (hex(v),t)

# time rotation
a=rollsum.new(data)
t=clock()
for i in 10*data:
    a.rotate(i,i)
    v=a.digest()
t=clock()-t
print "80K x rotate of 8K data gave %s in %8.4f seconds" % (hex(v),t)
