import os
import sys
os.chdir(os.path.dirname(sys.argv[0]))
message_list=open('./list/message_list.txt','rU')
for message_label in message_list:
	message_label=message_label.replace('\n','')
	os.system("python message_bin/message_encode.py " + 
		   "raw/"+message_label+".bin "+
		   "message_tran/"+message_label+".bin.txt "+
		   "build/"+message_label+".bin")