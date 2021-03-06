from threading import Thread
import tarfile
import sys
from lib import Parser
from lib import Global as glb
'''
This creates crawler threads
'''
class ParserThread(Thread):
	def __init__(self):
		Thread.__init__(self)

	def stop(self):
		self._stop()


	def run(self):
		p = Parser.Parser(self.name)
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
