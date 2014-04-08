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
UrlVal::UrlVal(){}
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
 * BlockVal constructor
 */
BlockVal::BlockVal(){
	this->urlId = 0;
	this->start = 0;
	this->size = 0;
}
/**
 * HitVal constructor
 */
HitVal::HitVal(unsigned short pos, char tagType){
	this->pos = pos;
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
	filename = "dumps/index/comprbin/" + to_string(fileIndex);
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
 * Builds the data structure from the list of hits of the same word
 */
void Index::buildIndexDs(vector<Posting> postings, map<int,IndexVal>& outIndexMap){
	unsigned int currUrlId = 0;	
	IndexVal index(0);
	for(int i=0; i<postings.size(); ++i){
		if(i==0){
			index = IndexVal(postings[i].getUrlId());
		}
		else if(i!=0 && currUrlId != postings[i].getUrlId()){
			outIndexMap.emplace(currUrlId, index);
			index = IndexVal(postings[i].getUrlId());
		}
		
		index.hits.push_back(HitVal(postings[i].getPos(),postings[i].getTagType()));
		++index.total;
		currUrlId = postings[i].getUrlId();
		
		if(i == postings.size()-1){
			outIndexMap.emplace(currUrlId, index);
		}
	}
}

void Index::writeToFile(ofstream& f, vector<unsigned char> v){
	if(f.is_open()){
		for(int i=0; i<v.size(); ++i){
			f.write((char*)&v[i], Constants::CHAR_SIZE);
		}
	}
}
/**
 * Posts index value to the file. Each value is compressed.
 */
void Index::processIndexVal(ofstream& f, IndexVal& index, unsigned int& size){
	vector<unsigned char> comprUrlId;
	vector<unsigned char> comprTotal;
	vector<unsigned char> comprHitPos;
	vector<unsigned char> comprHitTagType;
	
	encode(index.urlId,comprUrlId);
	size += comprUrlId.size()*Constants::CHAR_SIZE;
	writeToFile(f,comprUrlId);
	
	encode(index.total,comprTotal);
	size += comprTotal.size()*Constants::CHAR_SIZE;
	writeToFile(f,comprTotal);
	for(int i=0; i<index.hits.size(); ++i){
		comprHitPos = vector<unsigned char>();
		encode(index.hits[i].pos,comprHitPos);
		size += comprHitPos.size()*Constants::CHAR_SIZE;
		writeToFile(f,comprHitPos);
		
		comprHitTagType = vector<unsigned char>();
		encode(index.hits[i].tagType,comprHitTagType);
		size += comprHitTagType.size()*Constants::CHAR_SIZE;
		writeToFile(f,comprHitTagType);
	}
}

/**
 * Posts each block value, compressed, in the auxiliary list for a word
 */
void Index::processBlockVal(ofstream& f, BlockVal& block){
	vector<unsigned char> comprUrlId;
	vector<unsigned char> comprStart;
	vector<unsigned char> comprSize;
	
	encode(block.urlId,comprUrlId);
	writeToFile(f,comprUrlId);
	
	encode(block.start,comprStart);
	writeToFile(f,comprStart);	

	encode(block.size,comprSize);
	writeToFile(f,comprSize);
	
}
/**
 * Posts compressed index into file
 */
unsigned int Index::postIndex(vector<Posting> postings, unsigned short fileIndex){
	map<int,IndexVal> indexMap;
	vector<BlockVal> bVals;
	unsigned int totalSize;
	unsigned int indStart = 0;
	unsigned int indEnd = 0;
	int i = 0;
	BlockVal bVal;
	ofstream f;	
	string filename;

	buildIndexDs(postings, indexMap);
	
	totalSize = indexMap.size();
	

	filename = "dumps/index/comprbin/" + to_string(fileIndex);
	f.open(filename.c_str(),  ios::out|ios::binary|ios::app);
	/*
	 * Posts the hits to the binary file 
	 */
	for(map<int,IndexVal>::iterator iter = indexMap.begin(); iter!=indexMap.end(); ++iter){
		if(i%Constants::URLS_IN_BLOCK == 0){
			bVal = BlockVal();
			bVal.urlId = iter->first;
			bVal.start = (unsigned int) f.tellp();
		}
		processIndexVal(f, iter->second, bVal.size);
		if((i%Constants::URLS_IN_BLOCK == Constants::URLS_IN_BLOCK-1) || i == totalSize-1){
			bVals.push_back(bVal);	
		}
		++i;
	}
	/*
	 * Posts auxiliary list
	 */
	indStart = (unsigned int) f.tellp();
	for(int i=0; i<bVals.size(); ++i){
		processBlockVal(f,bVals[i]);
	}
	indEnd = (unsigned int) f.tellp();
	postToDict(postings[0].getWord(), totalSize, fileIndex, indStart, indEnd);
	f.close();
	return indEnd;
}
