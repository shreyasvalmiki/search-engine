#include<vector>
#include<string>
#include<cstring>
#include<sstream>

#ifndef COMMON_H_
#define COMMON_H_

using namespace std;
class Constants{
public:
	static int CHAR_SIZE;
	static int URLS_IN_BLOCK;
};
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
void encode(unsigned int value, vector<unsigned char>& output);
void decode(unsigned char* input, unsigned int size, vector<unsigned int>& out);
string to_lower(string str);
#endif
