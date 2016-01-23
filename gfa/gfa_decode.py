# Tool for decoding gfa files
# inspired by 
#  - hcs64/vgm_ripping /gfa.bms (https://github.com/hcs64/vgm_ripping/tree/master/etc/gfo2png/gfa.bms)
#  - bpe encoding (http://www.csse.monash.edu.au/cluster/RJK/Compress/problem.html)
import os
import struct
import sys
import gfa_crc

if len(sys.argv)<2:
	fileName=input("gfa file name:")
else:
	fileName=sys.argv[1]
subfileList=[]
crcTable={}
currentSubfile=0
fileOut=None

dir=fileName+".extract/"
if not os.path.exists(dir):
	os.mkdir(dir)

def openSubfile(index):
	global subfileList
	global dir
	global fileOut
	fileOut=open(dir+subfileList[index][0],'wb')

def outputData(aByte):
	global currentSubfile
	global fileOut
	global subfileList
	if fileOut==None:
		if aByte[0]!=0:
			print("None-zero ending padding!:", aByte[0])
		else:
			print("Zero ending padding!")
		return
	if subfileList[currentSubfile][2]!=0 :
		subfileList[currentSubfile][2]-=1
		if aByte[0]!=0:
			print("None-zero padding!:", aByte[0])
		return

	fileOut.write(aByte)
	subfileList[currentSubfile][1]-=1
	if subfileList[currentSubfile][1]==0:
		fileOut.close()
		print(subfileList[currentSubfile][0],"finished")
		currentSubfile+=1
		if len(subfileList)!=currentSubfile:
			openSubfile(currentSubfile)


fileIn=open(fileName,"rb")


if fileIn.read(4).decode()!='GFAC':
	print("magic!=GFAC!")
	fileIn.close()
	exit(1)

v1,v2,tableOff,tableLen,dataOff,dataLen=struct.unpack('IIIIII',fileIn.read(24))
print('v1=',v1)
print('v2=',v2)
print('table@',tableOff,' len=',tableLen)
print('data@',dataOff,' len=',dataLen)

fileIn.seek(tableOff,os.SEEK_SET)
subfileCount,=struct.unpack('i',fileIn.read(4))
for i in range(subfileCount):
	crc,nameOff,subfileLen,subfileOff=struct.unpack('IIII',fileIn.read(16))
	if nameOff>0x80000000:
		print("(the last file)")
		nameOff-=0x80000000
	
	if i!=0:
		paddingBefore=subfileOff-prevSubfileOff-prevSubfileLen
	else:
		paddingBefore=0
	savePos=fileIn.tell()
	fileIn.seek(nameOff,os.SEEK_SET)
	name=''
	while True:
		c=fileIn.read(1).decode()
		if c=='\0':
			break
		name+=c
	fileIn.seek(savePos,os.SEEK_SET)
	subfileList.append( [name,subfileLen,paddingBefore] )
	crcTable[name]=crc
	print("file:",name,"len=",subfileLen)
	prevSubfileOff=subfileOff
	prevSubfileLen=subfileLen

fileOut=open(dir+gfa_crc.crcTableFileName,'wt')
gfa_crc.writeTable(fileOut,crcTable)
fileOut.close();
fileOut=None

openSubfile(0)

fileIn.seek(dataOff,os.SEEK_SET)
if fileIn.read(4).decode()!='GFCP':
	print("magic!=GFCP!")
	fileIn.close()
	exit(1)

v3,v4,rawLen,comLen=struct.unpack('IIII',fileIn.read(16))

print('v3=',v3)
print('v4=',v4)
print('raw len=',rawLen,'com len=',comLen)

while rawLen!=0:
	left,right,stack=list(range(256)),list(range(256)),[]
	for i in range(256):
		left[i]=i
	c=0
	while True:
		count,=struct.unpack('B',fileIn.read(1))
		comLen-=1
		if count>127:
			c+=count-127
			count=0
		if c==256:
			break
		for j in range(count+1):
			left[c],=struct.unpack('B',fileIn.read(1))
			comLen-=1
			if c!=left[c]:
				right[c],=struct.unpack('B',fileIn.read(1))
				comLen-=1
			c+=1
		if c==256:
			break
	size,=struct.unpack('B',fileIn.read(1))
	size*=256
	sizeB,=struct.unpack('B',fileIn.read(1))
	size+=sizeB
	comLen-=2
	while True:
		if len(stack)!=0:
			c=stack.pop()
		else:
			if(size==0):
				break
			size-=1
			c,=struct.unpack('B',fileIn.read(1))
			comLen-=1
		if c==left[c]:
			outputData(struct.pack('B',c))
			rawLen-=1
		else:
			stack.append(right[c])
			stack.append(left[c])
	
if comLen!=0:
	print("Decompress error!")
	fileIn.close()
		
fileIn.close()
print("All finished!")