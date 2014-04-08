#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "Posting.h"
#include "Common.h"
#include <unordered_map>
#include <math.h>
#include <cstring>
#include <map>
#ifndef INDEX_H_
#define INDEX_H_


using namespace std;

class DictVal{
public:
	unsigned int total;
	string fileIndex;
	unsigned int start;
	unsigned int end;
	DictVal(unsigned int, string, unsigned int, unsigned int);
};


class UrlVal{
public:
	string url;
	unsigned int total;
	UrlVal();
	UrlVal(string, unsigned int);
};

class BlockVal{
public:
	unsigned int urlId;
	unsigned int start;
	unsigned int size;
	BlockVal();
};

class HitVal{
public:
	unsigned short pos;
	char tagType;
	HitVal(unsigned short, char);
};

class IndexVal{
public:
	unsigned int urlId;
	unsigned int total;
	vector<HitVal> hits;
	IndexVal(unsigned int);
};

class Index{

private:
	
	static void postToDict(string word, unsigned int total, unsigned short fileIndex, unsigned int start, unsigned int end);
	static void buildIndexDs(vector<Posting> postings, map<int,IndexVal>& outIndexMap);
	static void writeToFile(ofstream& f, vector<unsigned char> v);
	static void processIndexVal(ofstream& f, IndexVal& index, unsigned int& size);
	static void processBlockVal(ofstream& f, BlockVal& block);
	
public:
	static unordered_map<string, DictVal*> dict;
	static unordered_map<string, UrlVal*> urlMap;
	static int blockSize;
	static int uncomprBlockSize;
	static int firstHitSize;
	static int subHitSize;
	static int distBetweenFirst;
	static void buildDictFromFile();
	static void buildUrlMapFromFile();

	static int postIndexToTextFile(vector<Posting> postings, short fileIndex);
	
	static int postIndexToBinFile(vector<Posting> postings, short fileIndex);
	static int postIndexToComprBinFile(vector<Posting> postings, short fileIndex);
	static char* getIndexBuffer(unsigned int, unsigned int);
	static vector<IndexVal> getIndex(string);
	static unsigned int postIndex(vector<Posting> postings, unsigned short fileIndex);
	
};


#endif
