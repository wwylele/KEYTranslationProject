import os
import struct
BinName=input("message file:")
TxtName=input("txt file:")
os.system("copy "+BinName+" "+BinName+".remake")
bin=open(BinName+".remake","rb+")
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
		dic[current].append(row.replace('$','\n'))
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
bin.close()