import sys
import os
import struct
arcH=sys.argv[1]
brfnt=sys.argv[2]
arc=sys.argv[3]
print(arc,"<",arcH,"+",brfnt)
arcHLen=os.path.getsize(arcH);
brfntLen=os.path.getsize(brfnt);
farc=open(arc,'wb')
farcH=open(arcH,'rb')
fbrfnt=open(brfnt,'rb')
farc.write(farcH.read(arcHLen))
farc.write(fbrfnt.read(brfntLen))
farc.seek(0x64,os.SEEK_SET)
farc.write(struct.pack('>I',brfntLen))

farc.close()