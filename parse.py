import string
import tarfile
import sys
#from lib import ParserThread as PT
from lib import Parser
from lib import Global as glb
'''
It requires the base tar file list to be piped to this program for it to start processing
'''

#totalThreads = 1

for line in sys.stdin.read().splitlines():
	glb.q.put(line)

#for i in range(totalThreads):
#	pt = PT.ParserThread()
#	pt.start()


p = Parser.Parser()
while glb.q.qsize() > 0:
	lsLine = glb.q.get()
	fileSet = set()
	
	with tarfile.open(lsLine, "r") as t:
		for entry in t:
			fname = entry.name
			if glb.reFnameMatch.match(fname):
				fileSet.add(glb.reFnameSplit.split(fname)[0])

		for fname in fileSet:
			print("Processing: "+fname)
			dataMem = t.getmember(fname+"_data")
			indexMem = t.getmember(fname+"_index")
			indexFile = t.extractfile(indexMem)
			dataFile = t.extractfile(dataMem)
			p.processGzip(indexFile,dataFile)
