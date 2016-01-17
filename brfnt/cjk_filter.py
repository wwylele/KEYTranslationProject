
import struct
import sys
import getopt

opts,args=getopt.getopt(sys.argv[1:],'o:')

print("cjk_filter")
charSet=set()
for txtName in args:
	print(txtName)
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

for (o,v) in opts:
	if o=='-o':
		outName=v
		break;
print("total characters number=", len(charSet))
print("->",outName)
out=open(outName,'wb')
for d in charSet:
	out.write(struct.pack("H",d))
out.close()

