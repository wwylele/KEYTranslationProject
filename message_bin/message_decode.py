import os
import struct
import sys
if len(sys.argv)<3 :
	fileInName=input("message file:")
	fileOutName=fileInName+'.txt'
else:
	fileInName=sys.argv[1]
	fileOutName=sys.argv[2]
fileIn=open(fileInName,'rb')
fileOut=open(fileOutName,'w',encoding='utf-8')

print(fileInName,">",fileOutName)

fileIn.seek(0x14,os.SEEK_SET)
Coff,Doff=struct.unpack('>II',fileIn.read(8))

id=0
while Coff!=Doff:
	fileIn.seek(Coff,os.SEEK_SET)
	off,=struct.unpack('>I',fileIn.read(4))
	fileIn.seek(off+Doff,os.SEEK_SET)

	fileOut.write('@'+str(id)+'\n')
	h0,h1=struct.unpack('>HH',fileIn.read(4))
	if (h0,h1)!=(0x000A,0x5047):
		print("@",off+Doff,"PG begin",(h0,h1))
	while True:
		h0,h1=struct.unpack('>HH',fileIn.read(4))
		if (h0,h1)==(0x000B,0x5047):
			break
		elif (h0,h1)==(0x000A,0x5458):
			h0,h1=struct.unpack('>HH',fileIn.read(4))
			if h0!=0:
				print("@",off+Doff,"length",(h0,h1))
			text=fileIn.read(h1*2).decode('utf_16_be').replace('\n','$')
			fileOut.write(text+'\n')
		else:
			print("@",off+Doff,"end",(h0,h1))
			break

	Coff+=4
	id+=1

fileIn.close()
fileOut.close()