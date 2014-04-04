#include<vector>
#include<string>
#include<sstream>

#ifndef COMMON_H_
#define COMMON_H_

using namespace std;
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
void encode(unsigned int value, vector<unsigned char>& output);
void decode(unsigned char* input, unsigned int size, vector<unsigned int>& out);
#endif
