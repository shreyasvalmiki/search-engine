#include "Query.h"

unordered_map<string, DictVal*> Query::dict;
unordered_map<string, UrlVal*> Query::urlMap;

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
				Query::dict.emplace(splitLine[0], new DictVal((unsigned int) stoi(splitLine[1]), splitLine[2], (unsigned int) stoi(splitLine[3]), (unsigned int) stoi(splitLine[4])));	
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
				Query::urlMap.emplace(splitLine[0], new UrlVal(splitLine[1], (unsigned int) stoi(splitLine[2])));	
			}
		}	
	}
	inf.close();
}
