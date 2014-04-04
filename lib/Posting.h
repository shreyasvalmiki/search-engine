#include <string>
#include <vector>
#include <stdlib.h>
#include "Common.h"

#ifndef POSTING_H_
#define POSTING_H_

using namespace std;

class Posting{
private:
	string word;
	unsigned int urlId;
	unsigned short pos;
	char tagType;

public:
	Posting(string);
	string getWord();
	unsigned int getUrlId();
	unsigned short getPos();
	char getTagType();
};

#endif
