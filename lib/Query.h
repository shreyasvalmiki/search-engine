#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <math.h>
#include <queue>
#include "Common.h"
#include "Index.h"

#ifndef QUERY_H_
#define QUERY_H_

class BMDocument{
public:
	UrlVal url;
	double bm;
	BMDocument();
	BMDocument(unsigned int urlId, double bm);
};

class Query{
	
private:
	
	static void buildDictFromFile();
	static void buildUrlMapFromFile();
	static void setDocConsts();
	static vector<unsigned int> getIndexAuxList(DictVal dictVal);
	static double bm25(double d, double ft, double fdt);
	static void getUncompressed(string fileIndex, unsigned int start, unsigned int size, vector<unsigned int>& list);
	static vector<unsigned int> getChunk(string word, unsigned int chunkId, string fileIndex, unsigned int start, unsigned int size);
	static void getNextDoc(string word, DictVal dictVal, vector<unsigned int>& auxList, int& startIndex, unsigned int urlId, BMDocument& bmDoc);
	static void processPrime(vector<vector<unsigned int>>& auxLists, vector<string>& dictWords, vector<DictVal>& dictVals, int prime);
public:
	static double dAvg;
	static double N;
	static double k1;
	static double b;
	static unordered_map<string, DictVal> dict;
	static unordered_map<unsigned int, UrlVal> urlMap;
	static unordered_map<string, unordered_map<unsigned int, vector<unsigned int>>> cache;
	static unordered_map<string, vector<unsigned int>> uncomprCache;
	static unordered_map<unsigned int, BMDocument> bmMap;
	static priority_queue<BMDocument,vector<BMDocument>, greater<vector<BMDocument>::value_type>> dQueue;
	static void processQuery(string query);
	static void init();
};
bool operator< (const BMDocument& lhs, const BMDocument& rhs);
bool operator> (const BMDocument& lhs, const BMDocument& rhs);
#endif
