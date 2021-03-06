#include "Posting.h"

using namespace std;

Posting::Posting(string line){
	
	vector<string> v = split(line, ' ');
	if(v.size() == 4){
		word = v[0];
		urlId = (unsigned int) atoi(v[1].c_str());
		pos = (unsigned short) atoi(v[2].c_str());
		tagType = v[3].at(0);
	}
}

string Posting::getWord(){
	return word;
}
unsigned int Posting::getUrlId(){
	return urlId;
}
unsigned short Posting::getPos(){
	return pos;
}
char Posting::getTagType(){
	return tagType;
}
