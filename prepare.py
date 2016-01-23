import os
import sys
os.chdir(os.path.dirname(sys.argv[0]))
os.system("compile_tool")
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


