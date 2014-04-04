#include "Common.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
	    elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}


/**
* Encodes an unsigned variable-length integer using the MSB algorithm.
*/
void encode(unsigned int value, vector<unsigned char>& output) {
	while (value > 127) {
		output.push_back((unsigned char)((value & 127) | 128));
		value >>= 7;
	}
	output.push_back(value);
}

/**
* Decodes an unsigned variable-length integer using the MSB algorithm.
*/
void decode(unsigned char* input, unsigned int size, vector<unsigned int>& out) {
	int ind = -1;
	vector<unsigned char> temp;
	for (unsigned int i = 0; i < size; ++i) {
		++ind;
		temp.push_back(input[i]);

		if(!(input[i] & 128)){
			unsigned int ret = 0;
			for(int j = 0; j <= ind; ++j){
				ret |= (temp[j]&127) << (7*j);
			}
			out.push_back(ret);
			temp = vector<unsigned char>();
			ind = -1;
		}
	}
}