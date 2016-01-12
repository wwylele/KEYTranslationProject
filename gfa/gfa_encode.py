# Tool for encoding gfa files
# inspired by 
#  - hcs64/vgm_ripping /gfa.bms (https://github.com/hcs64/vgm_ripping/tree/master/etc/gfo2png/gfa.bms)
#  - bpe encoding (http://www.csse.monash.edu.au/cluster/RJK/Compress/problem.html)
import os
import struct
import sys
import gfa_crc

if not os.path.isfile("bpe.exe"):
	print("need compiled bpe.exe (from bpe.c)")
	exit(1)

subfileInfo=[]
if len(sys.argv)<3:
	pathName=input("the path of files to encode:")
	outFileName=os.path.join(pathName,"../remake.gfa")
else:
	pathName=sys.argv[1]
	outFileName=sys.argv[2]
	print(outFileName,"<",pathName)
	
tempFileName=os.path.join(pathName,"../gfa.temp")
comFileName=os.path.join(pathName,"../gfa.comress")

tempFile=open(tempFileName,"wb")
for subfileName in os.listdir(pathName):
	fullPath=os.path.join(pathName,subfileName)
	if not os.path.isfile(fullPath):
		continue
	if subfileName==gfa_crc.crcTableFileName:
		crcFile=open(fullPath,'rt')
		crcTable=gfa_crc.readTable(crcFile)
		crcFile.close()
		continue
	Len=os.path.getsize(fullPath)
	info=[subfileName,tempFile.tell(),Len,0]
	subfileInfo.append(info)
	print(info)
	subfile=open(fullPath,'rb')
	tempFile.write(subfile.read(Len))
	subfile.close()
	currentPos=tempFile.tell();
	padding=32-tempFile.tell()%32
	tempFile.seek(padding,os.SEEK_CUR)

tempFile.close()

rawLen=os.path.getsize(tempFileName)

os.system("bpe " +tempFileName+ " " +comFileName+" 5000 4096 200 3")
os.remove(tempFileName);

comLen=os.path.getsize(comFileName)

outFile=open(outFileName,"wb")

#Skip the header
outFile.seek(0x2C,os.SEEK_CUR)
tableOff=outFile.tell()


outFile.write(struct.pack('I',len(subfileInfo)))
tableInfoOff=outFile.tell()
#Skip table info section
outFile.seek(len(subfileInfo)*16,os.SEEK_CUR)

idx=0
for i in subfileInfo:
	i[3]=outFile.tell()
	if idx==len(subfileInfo)-1:
		i[3]+=0x80000000
	outFile.write((i[0]+'\0').encode())
	idx+=1
tableEndOff=outFile.tell()
outFile.seek(32-outFile.tell()%32,os.SEEK_CUR)
dataOff=outFile.tell()
outFile.seek(tableInfoOff,os.SEEK_SET)
for i in subfileInfo:
	outFile.write(struct.pack('IIII',crcTable[i[0]],i[3],i[2],i[1]+dataOff))
outFile.seek(dataOff,os.SEEK_SET)
outFile.write(struct.pack("4sIIII",b"GFCP",1,1,rawLen,comLen))
comFile=open(comFileName,"rb")
outFile.write(comFile.read(comLen))
comFile.close()
os.remove(comFileName);

outFile.seek(0,os.SEEK_SET)
outFile.write(struct.pack("4sIIIIII",b"GFAC",0x300,1,tableOff,tableEndOff-tableOff,dataOff,comLen+0x14))
outFile.close()
