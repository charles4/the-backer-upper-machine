#!/usr/bin/env python

import os,sys,random
random.seed(11)

KB=2**10
MB=2**20

def randchar():
    return chr(random.randint(0,255))

def execute(cmd):
    print cmd
    os.system(cmd)

def gen_olddata(size):
    olddata=""
    while len(olddata)<size:
        data=""
        for i in range(KB):
            data=data+randchar()
        olddata=olddata+data
        sys.stdout.write('.'),sys.stdout.flush()
    print
    return olddata

def gen_newdata(olddata):
    pos,newdata=0,""
    while pos<len(olddata):
        length=random.randint(1,8*KB)
        action=random.random()
        data=""
        if action < 0.5:
            #unchanged
            data=olddata[pos:pos+length]
            pos=pos+length
        elif action < 0.7:
            #insert
            for i in range(length):
                data=data+randchar()
        elif action < 0.8:
            #replace
            for i in range(length):
                data=data+randchar()
            pos=pos+length
        elif action < 0.9:
            #delete
            pos=pos+length
        else:
            #copy
            ofs=random.randint(0,len(olddata)-length)
            data=olddata[ofs:ofs+length]
        newdata=newdata+data
        sys.stdout.write('.'),sys.stdout.flush()
    print
    return newdata

def get_olddata(name,size):
    if os.path.exists(name):
        print "reading existing olddata file",name
        olddata=open(name,"rb").read()
    else:
        print "generating new olddata file",name
        olddata=gen_olddata(size)
        open(name,"wb").write(olddata)
    return olddata

def get_newdata(name,olddata):
    if os.path.exists(name):
        print "reading existing newdata file",name
        newdata=open(name,"rb").read()
    else:
        print "generating new newdata file",name
        newdata=gen_newdata(olddata)
        open(name,"wb").write(newdata)
    return newdata

if __name__ == "__main__":
    from sys import argv,exit

    try:
        if len(argv)==1:
            size=256*KB
        elif argv[1][-1]=='K':
            size=int(argv[1][:-1])*KB
        elif argv[1][-1]=='M':
            size=int(argv[1][:-1])*MB
        else:
            size=int(argv[1])
    except:
        print """
Usage:
    %s [<size>[K|M]]
    	... generates test files oldfile.bin, newfile.bin and altfile.bin.

Where <size> specifies the filesize to generate and is a number
optionaly followed by K (KBytes) or M (MBytes) that defaults to 256K.
The file oldfile.bin is a basis file, newfile.bin is a modified target
of oldfile.bin, and altfile.bin is a totally different file.

All files contain random uncompressable data.
""" % os.path.basename(argv[0])
        exit(1)

    olddata=get_olddata("oldfile.bin",size)
    newdata=get_newdata("newfile.bin",olddata)
    altdata=get_olddata("attfile.bin",size)
