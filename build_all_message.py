import os
import sys
os.chdir(os.path.dirname(sys.argv[0]))
message_list=open('./list/message_list.txt','rU')
fileList=""
for message_label in message_list:
	message_label=message_label.replace('\n','')
	fileList+="message_tran/"+message_label+".bin.txt "
	os.system("python message_bin/message_encode.py " + 
		   "raw/"+message_label+".bin "+
		   "message_tran/"+message_label+".bin.txt "+
		   "build/"+message_label+".bin")
os.system("python brfnt/cjk_filter.py -o build/cjkset "+fileList)
os.chdir("./brfnt/brfnt_mod/Debug/")
os.system("brfnt_mod ../../../raw/GameFont1.brfnt ../../../build/cjkset ../../../build/GameFont1.brfnt")
os.system("brfnt_mod ../../../raw/GameFont2.brfnt ../../../build/cjkset ../../../build/GameFont2.brfnt")
os.chdir("../../../")
os.system("python arc/gamefontx_cat.py raw/GameFont1ArcHeader "+
		  "build/GameFont1.brfnt raw/GameFont1.gfa.extract/GameFont1.arc")
os.system("python arc/gamefontx_cat.py raw/GameFont2ArcHeader "+
		  "build/GameFont2.brfnt raw/GameFont2.gfa.extract/GameFont2.arc")
os.chdir("./gfa")
os.system("python gfa_encode.py ../raw/GameFont1.gfa.extract ../build/GameFont1.gfa")
os.system("python gfa_encode.py ../raw/GameFont2.gfa.extract ../build/GameFont2.gfa")