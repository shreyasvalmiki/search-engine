#include <iostream>
#include <fstream>
#include "lib/Index.h"
#include "lib/Common.h"
#include <string>
#include <stdio.h>
#include <unordered_map>

char* getIndex(string fileIndex,unsigned int start, unsigned int size){
	char* buff;
	buff = (char*) malloc (sizeof(char)*size);
	
	ifstream f;
	f.open("dumps/index/bin/"+fileIndex, ios::in|ios::binary);
	
	if(f){
		unsigned int u;
		f.read ((char*)(&u), sizeof (int));
		cout<<"URL: "<<u<<endl;
		f.seekg(start);
		f.read((char*)(buff), sizeof(char)*size);
		f.close();
		return buff;
	}
	return NULL;
}







int main(int argc, char* argv[]){
	cout<< "Starting the query engine. Please wait..."<<endl;
	Index::buildDictFromFile();
	Index::buildUrlMapFromFile();

	while(true){
		cout<<"Enter a query: ";
		string query = "";
		cin >> query;
		unordered_map<std::string,DictVal*>::const_iterator pair = Index::dict.find(query);
		if(pair != Index::dict.end()){
			cout << pair->first << endl << pair->second->fileIndex <<endl <<pair->second->total<<endl<< pair->second->start <<endl << pair->second->end <<endl;
			unsigned int size = pair->second->end - pair->second->start; 
			char* buffer = getIndex(pair->second->fileIndex, pair->second->start, size);
			int fullBlocks = (int)(size/Index::blockSize);
			unsigned int urlId;
			unsigned short pos;
			char type;
			memcpy(&urlId, buffer, sizeof(int));
			memcpy(&pos, buffer+sizeof(int), sizeof(short));
			memcpy(&type, buffer + sizeof(int)+sizeof(short), sizeof(char));
			cout<<"URL ID: "<<urlId << endl ;//<< "Pos" << pos << endl << "type" << type << endl;
			for(int i=0; i<3; ++i){
				unsigned short u;
				memcpy(&u, buffer+i*Index::subHitSize, sizeof(short));
				memcpy(&pos, buffer+sizeof(int)+i*Index::subHitSize, sizeof(short));
				memcpy(&type, buffer + sizeof(int)+sizeof(short)+i*Index::subHitSize, sizeof(char));
				cout<<"URL add: "<<u << endl << "Pos" << pos << endl << "type" << type << endl;
			}
		}
	}
}
