#include <iostream>
#include <fstream>
#include "lib/Index.h"
#include "lib/Common.h"
#include "lib/Query.h"
#include <string>
#include <cstring>
#include <stdio.h>
#include <unordered_map>

void clearAll(){
	Query::uncomprCache = unordered_map<string, vector<unsigned int>>();
	Query::bmMap = unordered_map<unsigned int, BMDocument>();
	Query::dQueue = priority_queue<BMDocument,vector<BMDocument>, greater<vector<BMDocument>::value_type>>();
	
}
int main(int argc, char* argv[]){
	cout<< "Starting the query engine. Please wait..."<<endl;
	Query::init();
	string query = "";
	while(true){
		
		int resultCount = 20;	
		if(query.length() == 0){
			cout<<"Enter a query: ";
			getline(cin,query);
			clearAll();
			if(query.length()>0){
				cout<<"Processing query: "<<query<<endl;
				Query::processQuery(query);
				int i=0;
				while(!Query::dQueue.empty()){
					cout<<Query::dQueue.top().url.url<<endl;
					Query::dQueue.pop();
					if(++i >= resultCount){
						Query::dQueue = priority_queue<BMDocument,vector<BMDocument>, greater<vector<BMDocument>::value_type>>();
						query="";
						break;
					}
				}
				cout<<endl<<endl<<"Press any key..."<<endl;
				string temp;
				cin>>temp;
				query="";
			}
		}	
		
		
		
		
	}
}
