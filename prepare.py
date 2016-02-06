import os
import sys
os.chdir(os.path.dirname(sys.argv[0]))

if not os.path.exists('./build'):
	os.mkdir('./build')


os.system("compile_tool")
if not os.path.exists('./raw'):
	print("Please place all needed sub files in the folder 'raw'.")
	exit(-1)
os.system("python gfa/gfa_decode.py raw/GameFont1.gfa")
os.system("python gfa/gfa_decode.py raw/GameFont2.gfa")

ahlen=[0,0x280,0x2C0]
for i in [1,2]:
	arcN="raw/GameFont%d.gfa.extract/GameFont%d.arc"%(i,i)
	brfnt="raw/GameFont%d.brfnt"%i
	arcHeader="raw/GameFont%dArcHeader"%i
	alen=os.path.getsize(arcN)
	arc=open(arcN,"rb")
	brfnt=open(brfnt,"wb")
	arcHeader=open(arcHeader,"wb")
	arcHeader.write(arc.read(ahlen[i]))
	brfnt.write(arc.read(alen-ahlen[i]))
	arc.close()
	arcHeader.close()
	brfnt.close()
	os.remove(arcN)

outputDir='./build/message_order'
if not os.path.exists(outputDir):
	os.mkdir(outputDir)

message_list=open('./list/message_list.txt','rU')
for message_label in message_list:
	message_label=message_label.replace('\n','')
	os.system("python message_bin/message_getorder.py " + 
		   "raw/"+message_label+".bin "+
		   os.path.join(outputDir,message_label+".order"))
message_list.close()
