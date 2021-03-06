import binascii
import codecs
import gzip
import string
import sys
import re
from queue import Queue
from lib import Global as glb
from bs4 import BeautifulSoup


class Parser():
	
	#def __init__(self):

	'''
	Saves the list into buckets
	'''
	def saveBuckets(self,postingList):
		for key in postingList:
			with open("dumps/buckets/"+key,'a') as f:
				for l in postingList[key]:
					f.write(l)

	def assignBucket(self,postingList,fChr,word,brLetter,insStr):
		if len(word) > 0 and word[1] >= brLetter:
			if fChr+'1' in postingList:
				postingList[fChr+'1'].append(insStr)
			else:
				postingList[fChr+'1'] = [insStr]
		else:
			if fChr in postingList:
				postingList[fChr].append(insStr)
			else:
				postingList[fChr] = [insStr]
	

	'''
	This function assigns different buckets based on the characters in the word.
	'''
	def bucketPosting(self,postingList,word,urlId,wordCount,tagType):
		fChr = word[0]
		insStr = word+' '+str(urlId)+' '+str(wordCount)+' '+tagType+'\n'
		if not glb.reFirstChar.match(fChr):
			if 'other' in postingList:
				postingList['other'].append(insStr)
			else:
				postingList['other'] = [insStr]
		else:
			if fChr == 'a' or fChr == 't':
				self.assignBucket(postingList,fChr,word,'n',insStr)
			
			elif fChr == 's':
				self.assignBucket(postingList,fChr,word,'h',insStr)
			
			else:
				if fChr in postingList:
					postingList[fChr].append(insStr)
				else:
					postingList[fChr] = [insStr]

	'''
	This function extracts the required information for each word from provided tags
	'''
	def processTagsAndExtract(self,postingList,tags,urlId, wordCount, tagType):
		for tag in tags:
			t = tag.extract()
			for word in glb.reWordSplit.split(t.get_text()):
				word = word.lower()
				if len(word)>0:
					wordCount += 1
					self.bucketPosting(postingList,word,urlId,wordCount,tagType)
		return wordCount

	'''
	This parses the HTML that is returned from the dump. It also decodes the gzip file that is provided as one of its parameters
	based on size.
	'''
	def parseHtml(self,f,start,size,urlId):
		wordCount = 0
		postingList = {}
		f.seek(start)
		html = ''
		try:
			html = f.read(size).decode('ascii','ignore')
			if html != '':
				soup = BeautifulSoup(html)
				if soup.style:
					soup.style.extract()
				if soup.script:
					soup.script.extract()
		
		
				wordCount = self.processTagsAndExtract(postingList,soup.find_all('title'),urlId, wordCount, 't')
				wordCount = self.processTagsAndExtract(postingList,soup.find_all('h1'),urlId, wordCount, '1')
				wordCount = self.processTagsAndExtract(postingList,soup.find_all('h2'),urlId, wordCount, '2')
				wordCount = self.processTagsAndExtract(postingList,soup.find_all('h3'),urlId, wordCount, '3')
				wordCount = self.processTagsAndExtract(postingList,soup.find_all('h4'),urlId, wordCount, '4')
				wordCount = self.processTagsAndExtract(postingList,soup.find_all('h5'),urlId, wordCount, '5')	
				for string in soup.strings:
					for word in glb.reWordSplit.split(string):
						if len(word)>0:
							word = word.lower()
							wordCount += 1
							self.bucketPosting(postingList,word,urlId,wordCount,'p')
						
		except:
			pass
	
		self.saveBuckets(postingList)
	
		return wordCount

	#def mapUrl(status, dFileGz)

	'''
	This opens each tar file and parses through all the gzip files to further process it into words and meta data
	'''
	def processGzip(self,indexFile,dataFile):
		with gzip.GzipFile(fileobj=indexFile,mode='rb') as iFileGz, gzip.GzipFile(fileobj=dataFile,mode='rb') as dFileGz:
			bLine = iFileGz.readline()
			currPointer = 0
			i = 0
			while True:
				size = 0
				if not bLine: break
				try:
					line = bLine.decode('ascii', 'ignore')
					splits = line.split(' ')
					url = splits[0]
					size = int(splits[3])
					status = splits[6]
				
					if status == 'ok\n' or status == 'ok':
						glb.urlId += 1
				
						wordCount = self.parseHtml(dFileGz,currPointer,size,glb.urlId)
						with open("dumps/url/urlmap",'a') as uMap:
							uMap.write(str(glb.urlId)+" "+url+" "+str(wordCount)+'\n')

					currPointer += size
					bLine = iFileGz.readline()
				
				except:
					#print(bLine)
					pass
			
			
				
			


