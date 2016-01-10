
import struct
charSet=set()
while True:
	txtName=input("a txt file name:")
	if txtName=='':
		break
	txt=open(txtName,'rU',encoding='utf_8_sig')
	for row in txt:
		row=row.replace('\n','')
		if row[:3]=='@@@':
			break
		elif row[:1]=='@':
			continue
		else:
			for c in row:
				d=c.encode("utf_16_le")
				d=d[0]+d[1]*256
				if d>=0x4E00 and d<0xA000:
					charSet.add(d)
	txt.close()

	

outName=input("the output file name:")
out=open(outName,'wb')
for d in charSet:
	out.write(struct.pack("H",d))
out.close()

