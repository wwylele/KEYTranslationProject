import os
import struct
import sys

BinName=sys.argv[1]
OrderName=sys.argv[2]

print("order:",BinName,">",OrderName)

bin=open(BinName,"rb")
orderFile=open(OrderName,"wb")
bin.seek(0x14,os.SEEK_SET)
Coff,Doff=struct.unpack('>II',bin.read(8))
count=(Doff-Coff)//4
bin.seek(Coff,os.SEEK_SET)

list=[]
for i in range(count):
	list.append( (i, struct.unpack('>I',bin.read(4))[0] ) )

list.sort(key=lambda PG:PG[1])

orderFile.write(struct.pack('I',count))
for i in range(count):
	orderFile.write(struct.pack('I',list[i][0]))

bin.close()
orderFile.close()