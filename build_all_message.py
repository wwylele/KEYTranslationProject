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