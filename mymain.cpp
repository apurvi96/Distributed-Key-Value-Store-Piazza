#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "jsonstring.h"

#include <iostream>
 
using namespace rapidjson;
using namespace std;

Document doc;
int main(){
	string s=acknowledge_string("1","test");
	cout<<"json str= "<<s<<endl;

	// In-situ parsing, decode strings directly in the source string. Source must be string.
	doc.ParseInsitu((char*)s.c_str());
	cout<<"message= "<<doc["message"].GetString()<<endl;


}