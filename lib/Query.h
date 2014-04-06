#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Common.h"

#ifndef QUERY_H_
#define QUERY_H_

class Query{
public:
	static unordered_map<string, DictVal*> dict;
	static unordered_map<string, UrlVal*> urlMap;
	static void buildDictFromFile();
	static void buildUrlMapFromFile();
}

#endif
