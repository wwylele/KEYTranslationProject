import os
import struct
import sys
import original_filter
if len(sys.argv)<4 :
	BinName=input("message file:")
	NewBinName=BinName+".remake"
	TxtName=input("txt file:")
else:
	BinName=sys.argv[1]
	NewBinName=sys.argv[3]
	TxtName=sys.argv[2]

print(BinName,NewBinName)
#os.system("copy /y "+BinName+" "+NewBinName)
ob=open(BinName,"rb")
nb=open(NewBinName,"wb")
ob.seek(0x14,os.SEEK_SET)
Coff,=struct.unpack('>I',ob.read(4))
ob.seek(0,os.SEEK_SET)
nb.write(ob.read(Coff))
ob.close()
nb.close()

bin=open(NewBinName,"rb+")
txt=open(TxtName,"rU",encoding='utf_8_sig')
bin.seek(0x14,os.SEEK_SET)
Coff,Doff=struct.unpack('>II',bin.read(8))
count=(Doff-Coff)//4
dic=[]
for i in range(count):
	dic.append(["@"+str(i)])

current=0
for row in txt:
	row=row.replace('\n','')
	if row[:3]=='@@@':
		break
	if row[:2]=='@@':
		continue
	elif row[:1]=='@':
		current=int(row[1:])
		dic[current]=[]
	else:
		row=row.replace('$','\n')
		for c in row:
			if not original_filter.validChar(c):
				print(">>>>Invalid character @",current)
		dic[current].append(row)
txt.close()

D=Doff
for PG in dic:
	bin.seek(Coff,os.SEEK_SET)
	bin.write(struct.pack('>I',D-Doff))
	Coff+=4
	bin.seek(D,os.SEEK_SET)
	bin.write(struct.pack('>HH',0x000A,0x5047))
	for TX in PG:
		bin.write(struct.pack('>HH',0x000A,0x5458))
		bin.write(struct.pack('>HH',0,len(TX)))
		bin.write(TX.encode('utf_16_be'))
	bin.write(struct.pack('>HH',0x000B,0x5047))
	D=bin.tell()
bin.seek(8,os.SEEK_SET)
bin.write(struct.pack('>I',D))
bin.close()