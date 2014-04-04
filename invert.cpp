#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include "lib/Common.h"
#include "lib/Posting.h"
#include "lib/Index.h"

using namespace std;

string tempWord = "";
short fileIndex = 0;
unsigned int sizeLimit = 1048576;

/**
 * Creates the inverted index in multiple binary files based on the file size limit specified
 */
void indexSortedFileToBin(string filename){
	ifstream inf;
	inf.open(filename.c_str());
	string line;
	string currWord = "";
	vector<Posting> postings;
	if(inf.is_open()){
		while(getline(inf,line)){
			if(line.size()>0){
				Posting post(line);
				if(currWord.size() > 0 && currWord.length() == post.getWord().length() && currWord.compare(post.getWord()) != 0){				
					unsigned int len = Index::postIndexToBinFile(postings, fileIndex);
					if(len >= sizeLimit){
						fileIndex++;
					}
					postings.clear();
				}
				postings.push_back(post);
				currWord = post.getWord();
			}
			
		}	
	}
	inf.close();
}

/**
 * Creates the inverted index in multiple compressed binary files based on the file size limit specified
 */
void indexSortedFileToComprBin(string filename){
	ifstream inf;
	inf.open(filename.c_str());
	string line;
	string currWord = "";
	vector<Posting> postings;
	if(inf.is_open()){
		while(getline(inf,line)){
			if(line.size()>0){
				Posting post(line);
				if(currWord.size() > 0 && currWord.compare(post.getWord()) != 0){
					//cout<<post.getWord()<<endl;			
					unsigned int len = Index::postIndexToComprBinFile(postings, fileIndex);
					if(len >= sizeLimit){
						fileIndex++;
					}
					postings.clear();
				}
				postings.push_back(post);
				currWord = post.getWord();
			}
			
		}	
	}
	inf.close();
}


/**
 * Creates the inverted index in multiple text files based on the file size limit specified
 */
void indexSortedFileToText(string filename){
	ifstream inf;
	inf.open(filename.c_str());
	string line;
	string currWord = "";
	vector<Posting> postings;
	if(inf.is_open()){
		while(getline(inf,line)){
			if(line.size()>0){
				Posting post(line);
				if(currWord.size() > 0 && currWord.compare(post.getWord()) != 0){	
							
					unsigned int len = Index::postIndexToTextFile(postings, fileIndex);
					if(len >= sizeLimit){
						fileIndex++;
					}
					postings.clear();
				}
				postings.push_back(post);
				currWord = post.getWord();
				//cout << postings.size()<<endl;
			}
			
		}	
	}
	inf.close();
}

/**
 * First argument to the program sets the size of each index file
 * 2nd argument (b/t) allows to save index files in binary or in text format
 */
int main(int argc, char* argv[]){
	bool isBin = true;
	if(argc > 1){
		sizeLimit = atoi(argv[1]);
		if(argc > 1){
			if(argv[2][0] == 't'){
				isBin = false;
			}
		}
	}
	string lineInput;
	while (cin >> lineInput) {
		cout << "Processing: " << lineInput << endl;
		if(isBin){
			indexSortedFileToComprBin("dumps/sorted/"+lineInput);
		}
  		else{
  			indexSortedFileToText("dumps/sorted/"+lineInput);
  		}
	}
}
