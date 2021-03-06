#include "Query.h"

bool operator< (const BMDocument& lhs, const BMDocument& rhs){
	return lhs.bm > rhs.bm;	
}

bool operator> (const BMDocument& lhs, const BMDocument& rhs){
	return lhs.bm < rhs.bm;	
}


unordered_map<string, DictVal> Query::dict;
unordered_map<unsigned int, UrlVal> Query::urlMap;
unordered_map<unsigned int, BMDocument> Query::bmMap;
unordered_map<string, unordered_map<unsigned int, vector<unsigned int>>> Query::cache;
unordered_map<string, vector<unsigned int>> Query::uncomprCache;
priority_queue<BMDocument,vector<BMDocument>, greater<vector<BMDocument>::value_type>> Query::dQueue;
double Query::k1 = 1.2;
double Query::b = 0.75;
double Query::dAvg = 0;
double Query::N = 0;


BMDocument::BMDocument(){}
BMDocument::BMDocument(unsigned int urlId, double bm){
	unordered_map<unsigned int,UrlVal>::const_iterator pair = Query::urlMap.find(urlId);
	if(pair != Query::urlMap.end()){
		this->url = pair->second;
	}
	else{
		this->url = UrlVal("",0);
	}
	this->bm = bm;
}
/**
 * Initializes list
 */
void Query::init(){
	buildDictFromFile();
	buildUrlMapFromFile();
	setDocConsts();
}


/**
 * Sets the document constants for BM25 calculation
 */
void Query::setDocConsts(){
	unsigned long long sum = 0;
	
	for(unordered_map<unsigned int,UrlVal>::iterator i = Query::urlMap.begin(); i!=Query::urlMap.end(); ++i){
		sum += i->second.total;
	}
	N = Query::urlMap.size();
	dAvg = (double) (sum/N);
	cout<<dAvg<<endl;
	cout<<N<<endl;
}
/**
 * Gets any compressed chunk of data and uncompresses it
 */
void Query::getUncompressed(string fileIndex, unsigned int start, unsigned int size, vector<unsigned int>& list){	
	unsigned char* buff;
	buff = (unsigned char*) malloc (Constants::CHAR_SIZE*size);
	
	ifstream f;
	f.open("dumps/index/comprbin/"+fileIndex, ios::in|ios::binary);
	
	if(f.is_open()){
		f.seekg(start);
		f.read((char*)buff, Constants::CHAR_SIZE*size);
		f.close();
		decode(buff, size, list);
	}
}
/**
 * Gets the auxiliary list and uncompresses it
 */
vector<unsigned int> Query::getIndexAuxList(DictVal dictVal){
	vector<unsigned int> list;
	unsigned int size = dictVal.end - dictVal.start;
	getUncompressed(dictVal.fileIndex,dictVal.start,size,list);
	return list;
}



/**
 * This builds the dictionary from the given file. This is run before querying.
 */
void Query::buildDictFromFile(){
	ifstream inf("dumps/dict");
	string line;
	if(inf.is_open()){
		while(getline(inf,line)){
			vector<string> splitLine = split(line, ' ');
			if(splitLine.size() == 5){
				Query::dict.emplace(splitLine[0], DictVal((unsigned int) stoi(splitLine[1]), splitLine[2], (unsigned int) stoi(splitLine[3]), (unsigned int) stoi(splitLine[4])));	
			}
		}
	}
	inf.close();
}

/**
 * This builds the URL Map from the given file. This is run before querying.
 */
void Query::buildUrlMapFromFile(){
	ifstream inf("dumps/url/urlmap");
	string line;
	if(inf.is_open()){
		while(getline(inf,line)){
			vector<string> splitLine = split(line, ' ');
			if(splitLine.size() == 3){
				Query::urlMap.emplace((unsigned int)stoi(splitLine[0]), UrlVal(splitLine[1], (unsigned int) stoi(splitLine[2])));
			}
		}
	}
	inf.close();
}
/**
 * Gets the compressed chunks and uncompresses it
 */
vector<unsigned int> Query::getChunk(string word, unsigned int chunkId, string fileIndex, unsigned int start, unsigned int size){
	unordered_map<string,vector<unsigned int>>::const_iterator pair = Query::uncomprCache.find(word+to_string(chunkId));

	if(pair != Query::uncomprCache.end()){
		return pair->second;
	}
	else{
		vector<unsigned int> v;
		getUncompressed(fileIndex,start,size,v);
		Query::uncomprCache.emplace(word+to_string(chunkId), v);
		return v;
	}
}

/**
 * Calculates BM25 score
 */
double Query::bm25(double d, double ft, double fdt){
	double K = (double)(k1*((1-Query::b)+Query::b*(d/Query::dAvg)));
	return (double)(log((double)((Query::N-ft+0.5)/(ft+0.5))) * (((k1+1)*fdt)/(K+fdt)));
}

/**
 * Calculates BM25 score for the document IDs that match. Gets only the chunk that is required
 */
void Query::getNextDoc(string word, DictVal dictVal, vector<unsigned int>& auxList, int& startIndex, unsigned int urlId, BMDocument& bmDoc){
	
	for(int i=startIndex; i<auxList.size(); i+=3){
		if(auxList[i]<urlId){
			startIndex = i;
			vector<unsigned int> v = getChunk(word, auxList[i], dictVal.fileIndex, auxList[i+1], auxList[i+2]);
			unsigned int j = 0;
			
			while(j<v.size()){
				if(v[j] == urlId){
					double bm = bm25((double)bmDoc.url.total, (double)dictVal.total, (double)v[j+1]);
					bmDoc.bm += bm;
					break;	
				}
				j = v[j+1]*2 + 1;
			}
			break;
		}
	}
}
/**
 * Reads each document ID in the index for the word, calculates the BM25 score for 
 * the document ID and ignores doc ID if it is already there in the map and compares it to other document lists.
 * This function reads all the chunks
 */
void Query::processPrime(vector<vector<unsigned int>>& auxLists, vector<string>& dictWords, vector<DictVal>& dictVals, int prime){
	vector<int> startMap;
	for(int i=0; i<dictWords.size(); ++i){
		startMap.push_back(0);
	}
	for(int i=0; i<auxLists[prime].size(); i+=3){		
		vector<unsigned int> v = getChunk(dictWords[prime], auxLists[prime][i], dictVals[prime].fileIndex, auxLists[prime][i+1], auxLists[prime][i+2]);
		unsigned int j = 0;

		while(j<v.size()){
			unordered_map<unsigned int, BMDocument>::const_iterator pair = Query::bmMap.find(v[j]);
			if(pair == bmMap.end()){
				BMDocument bmTemp(v[j],0);
				double bm = bm25((double)bmTemp.url.total, (double)dictVals[prime].total, (double)v[j+1]);
				bmTemp.bm = bm;
				for(int k=prime+1; k<dictWords.size(); ++k){
					getNextDoc(dictWords[k],dictVals[k],auxLists[k],startMap[k],v[j],bmTemp);
				}
				bmMap.emplace(v[j],bmTemp);
			}
			j = v[j+1]*2 + 1;
		}	
	}
}
/**
 * Breaks the words into tokens and processes the chunks one by one
 */
void Query::processQuery(string query){
	vector<string> words = split(to_lower(query), ' ');
	vector<DictVal> dictVals;
	vector<string> dictWords;
	vector<vector<unsigned int>> auxLists;
	int prime = 0;
	for(int i=0; i<words.size(); ++i){
		if(words[i].length()>0){
			unordered_map<string, DictVal>::const_iterator pair = Query::dict.find(words[i]);
			
			if(pair!=dict.end()){
				dictWords.push_back(pair->first);
				dictVals.push_back(pair->second);
				auxLists.push_back(getIndexAuxList(pair->second));
			}
		}
	}

	/*
	 * Processes each list
	 */
	for(int i=0; i<dictWords.size(); ++i){
		processPrime(auxLists, dictWords, dictVals, i);
	}
	/*
	 * Pushes the documents into the priority queue
	 */
	for(unordered_map<unsigned int,BMDocument>::iterator i = Query::bmMap.begin(); i!=Query::bmMap.end(); ++i){
		Query::dQueue.push(i->second);
	}
}
