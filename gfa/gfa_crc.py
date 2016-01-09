import csv

crcTableFileName='crctable'
def writeTable(fileObj,crcTable):
	writer=csv.writer(fileObj)
	for (name,crc) in crcTable.items():
		writer.writerow([name,crc])
	
def readTable(fileObj):
	table={}
	reader=csv.reader(fileObj)
	for row in reader:
		if len(row)>=2 :
			table[row[0]]=int(row[1])
	return table

