#include "Index.h"

using namespace std;

unordered_map<string, DictVal*> Index::dict;
unordered_map<string, UrlVal*> Index::urlMap;
int Index::blockSize = sizeof(int)+sizeof(short)+sizeof(char) + ((sizeof(short)*2)+sizeof(char))*3;
int Index::uncomprBlockSize = (sizeof(int)+sizeof(short)+sizeof(char))*4;
int Index::firstHitSize = sizeof(int)+sizeof(short)+sizeof(char);
int Index::subHitSize = (sizeof(short)*2)+sizeof(char);
int Index::distBetweenFirst = sizeof(short)+sizeof(char)+((sizeof(short)*2)+sizeof(char))*3;
/**
 * DictVal constructor
 */
DictVal::DictVal(unsigned int total, string fileIndex, unsigned int start, unsigned int end){
	this->total = total;
	this->fileIndex = fileIndex;
	this->start = start;
	this->end = end;
}
/**
 * UrlVal constructor
 */
UrlVal::UrlVal(string url, unsigned int total){
	this->total = total;
	this->url = url;
}
/**
 * IndexVal constructor
 */
IndexVal::IndexVal(unsigned int urlId){
	this->urlId = urlId;
	this->total = 0;
	this->hits = vector<HitVal>();
}


/**
 * HitVal constructor
 */
HitVal::HitVal(unsigned int pos, char tagType){
	this->urlId = pos;
	this->tagType = tagType;
}
/**
 * This function posts words with position values (of inverted index) to the dictionary file.
 */
void Index::postToDict(string word, unsigned int total, unsigned short fileIndex, unsigned int start, unsigned int end){
	ofstream dict;
	dict.open("dumps/dict",ios::out|ios::app);
	if(dict.is_open()){
		dict << word << " " << total << " " << fileIndex << " " << start << " " << end << "\n";
	}
	dict.close();
}

/**
 * This builds the dictionary from the given file. This is run before querying.
 */
void Index::buildDictFromFile(){
	ifstream inf("dumps/dict");
	string line;
	if(inf.is_open()){
		while(getline(inf,line)){
			vector<string> splitLine = split(line, ' ');
			if(splitLine.size() == 5){
				Index::dict.emplace(splitLine[0], new DictVal((unsigned int) stoi(splitLine[1]), splitLine[2], (unsigned int) stoi(splitLine[3]), (unsigned int) stoi(splitLine[4])));	
			}
			
		}	
	}
	inf.close();
}


/**
 * This builds the URL Map from the given file. This is run before querying.
 */
void Index::buildUrlMapFromFile(){
	ifstream inf("dumps/url/urlmap");
	string line;
	if(inf.is_open()){
		while(getline(inf,line)){
			vector<string> splitLine = split(line, ' ');
			if(splitLine.size() == 3){
				Index::urlMap.emplace(splitLine[0], new UrlVal(splitLine[1], (unsigned int) stoi(splitLine[2])));	
			}
		}	
	}
	inf.close();
}

/**
 * This function posts the inverted indexes to text file
 * Note that this is only for testing
 */
int Index::postIndexToTextFile(vector<Posting> postings, short fileIndex){
	ofstream f;	
	string filename;
	filename = "dumps/index/text/" + to_string(fileIndex);
	f.open(filename.c_str(),  ios::out|ios::app);
	
	unsigned int start = 0, end = 0;
	if(f.is_open()){
		start = (unsigned int) f.tellp();
		for(int i = 0; i<postings.size(); ++i){
			f << postings[i].getUrlId() << " " << postings[i].getPos() << " " << postings[i].getTagType() << ";";
		}
		f <<'\n';
		end = (unsigned int) f.tellp();
		f.close();
		postToDict(postings[0].getWord(), (unsigned int)postings.size(), fileIndex, start, end);
		return end;
	}
	return 0;
}


/**
 * This function posts the inverted indexes to binary file
 */
int Index::postIndexToBinFile(vector<Posting> postings, short fileIndex){
	ofstream f;	
	string filename;
	filename = "dumps/index/bin/" + to_string(fileIndex);
	f.open(filename.c_str(),  ios::out|ios::binary|ios::app);
	
	unsigned int start = 0, end = 0;
	if(f.is_open()){
		start = (unsigned int) f.tellp();
		for(int i = 0; i<postings.size(); ++i){
			unsigned int urlId = postings[i].getUrlId();
			unsigned short pos = postings[i].getPos();
			char tagType = postings[i].getTagType();
			f.write((char*)&urlId, sizeof(int));
			f.write((char*)&pos, sizeof(short));
			f.write((char*)&tagType, sizeof(char));
			
		}
		end = (unsigned int) f.tellp();
		f.close();
		postToDict(postings[0].getWord(), (unsigned int)postings.size(), fileIndex, start, end);
		return end;
	}
	return 0;
}


/**
 * This function posts the compressed inverted indexes to binary file
 */
int Index::postIndexToComprBinFile(vector<Posting> postings, short fileIndex){
	ofstream f;	
	string filename;
	filename = "dumps/index/bin/" + to_string(fileIndex);
	f.open(filename.c_str(),  ios::out|ios::binary|ios::app);
	
	unsigned int start = 0, end = 0;
	if(f.is_open()){
		start = (unsigned int) f.tellp();
		int prevUrlId;
		for(int i = 0; i<postings.size(); ++i){
			
			if(i%4 == 0){
				unsigned int urlId = postings[i].getUrlId();
				unsigned short pos = postings[i].getPos();
				char tagType = postings[i].getTagType();
				prevUrlId = urlId;
				f.write((char*)&urlId, sizeof(int));
				f.write((char*)&pos, sizeof(short));
				f.write((char*)&tagType, sizeof(char));
			}
			else{
				unsigned int urlId = postings[i].getUrlId();
				unsigned short diff = (unsigned short)(urlId - prevUrlId);
				prevUrlId = urlId;
				unsigned short pos = postings[i].getPos();
				char tagType = postings[i].getTagType();
				f.write((char*)&diff, sizeof(short));
				f.write((char*)&pos, sizeof(short));
				f.write((char*)&tagType, sizeof(char));
			}
		}
		end = (unsigned int) f.tellp();
		f.close();
		postToDict(postings[0].getWord(), (unsigned int)postings.size(), fileIndex, start, end);
		return end;
	}
	return 0;
}






void Index::buildIndexDs(vector<Posting> postings, map<int,IndexVal*>& outIndexMap){
	unsigned int currUrlId = 0;	
	IndexVal* index;
	for(int i=0; i<postings.size(); ++i){
		if(currUrlId != postings[i].getUrlId()){
			outUIds.push_back(postings[i].getUrlId());
			index = new IndexVal(postings[i].getUrlId());
			outIndexMap.emplace(postings[i].getUrlId(), index);
		}
		currUrlId = postings[i].getUrlId();
		index->hits.push_back(HitVal(postings[i].getPos(),postings[i].getTagType()));
		++index->total;
	}
}









/**
 * This function posts the compressed inverted indexes to binary file
 */
int Index::(vector<Posting> postings, short fileIndex){
	ofstream f;	
	string filename;
	filename = "dumps/index/bin/" + to_string(fileIndex);
	f.open(filename.c_str(),  ios::out|ios::binary|ios::app);
	
	unsigned int start = 0, end = 0;
	if(f.is_open()){
		start = (unsigned int) f.tellp();
		int prevUrlId;
		for(int i = 0; i<postings.size(); ++i){
			
			if(i%4 == 0){
				unsigned int urlId = postings[i].getUrlId();
				unsigned short pos = postings[i].getPos();
				char tagType = postings[i].getTagType();
				prevUrlId = urlId;
				f.write((char*)&urlId, sizeof(int));
				f.write((char*)&pos, sizeof(short));
				f.write((char*)&tagType, sizeof(char));
			}
			else{
				unsigned int urlId = postings[i].getUrlId();
				unsigned short diff = (unsigned short)(urlId - prevUrlId);
				prevUrlId = urlId;
				unsigned short pos = postings[i].getPos();
				char tagType = postings[i].getTagType();
				f.write((char*)&diff, sizeof(short));
				f.write((char*)&pos, sizeof(short));
				f.write((char*)&tagType, sizeof(char));
			}
		}
		end = (unsigned int) f.tellp();
		f.close();
		postToDict(postings[0].getWord(), (unsigned int)postings.size(), fileIndex, start, end);
		return end;
	}
	return 0;
}



/**
 * This function gets the inverted indexes from the particular file using the value obtained from the dictionary.
 * It returns the indexes for each word in the form of a vector of indexes.
 * This only works on the binary file.
 */
//vector<IndexVal> Index::getIndex(string word){
//	ifstream inf;
//	vector<IndexVal> indexVals;
//	if(dict[word] != NULL){
//		DictVal* dVal =  dict[word];
//		inf.open("dumps/index/bin/"+to_string(dVal->fileIndex),ios::binary | ios::out);
//		unsigned int size = dVal->end - dVal->start;
//		unsigned int totalBlocks = (int) ceil((float)size/(float)bufferSize);
//		unsigned int totalHits = size/hitSize;
//		char[bufferSize] buffer;
//		unsigned int hitCount = 0;
//		for(int i = 0; i<totalBlocks; ++i){
//			inf.read(buffer, bufferSize);
//			char[hitSize] hit;
//			if(hitCount < totalHits){
//				for(int j = 0; j < bufferSize/hitSize; ++j){
//					if(hitCount < totalHits){
//						unsigned int urlId;
//						unsigned short pos;
//						char type;
//						memcpy(&urlId, buffer + (j*hitSize), sizeof(int));
//						memcpy(&pos, buffer + (j*hitSize)+sizeof(int), sizeof(short));
//						memcpy(&type, buffer + (j*hitSize)+sizeof(int)+sizeof(short), sizeof(char));
//						++hitCount;
//						IndexVal val(urlId,pos,type);
//						indexVals.push_back(val);
//					}
//				}
//			}
//			
//		}
//		
//	}
//	return indexVals;
//}
