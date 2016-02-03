import os
import sys
os.chdir(os.path.dirname(sys.argv[0]))
if not os.path.exists('./build'):
	os.mkdir('./build')
outputDir='./build/message_extract'
if not os.path.exists(outputDir):
	os.mkdir(outputDir)

message_list=open('./list/message_list.txt','rU')
for message_label in message_list:
	message_label=message_label.replace('\n','')
	os.system("python message_bin/message_decode.py " + 
		   "raw/"+message_label+".bin "+
		   os.path.join(outputDir,message_label+".bin.txt"))
message_list.close()