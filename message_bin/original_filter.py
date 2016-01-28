import os
import sys
import struct
charsetf=open(os.path.join(os.path.dirname(sys.argv[0]),'original_charset'),'rb')
charset=set()

while True:
	word=charsetf.read(2)
	if len(word)==0:
		break
	charset.add(word[0]+word[1]*256)
charsetf.close()

def validChar(c):
	if c=='\n':
		return True
	d=c.encode("utf_16_le")
	d=d[0]+d[1]*256
	if d>=0x4E00 and d<0xA000:
		return True
	return d in charset
