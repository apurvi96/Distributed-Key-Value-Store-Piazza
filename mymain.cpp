#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "jsonstring.h"

#include <iostream>
 
using namespace rapidjson;
using namespace std;

Document doc;
int main(){
	string s=put_update_SS("1",1,"test","test2");
	cout<<"json str= "<<s<<endl;

	// In-situ parsing, decode strings directly in the source string. Source must be string. And paramater must not be const char*
	doc.ParseInsitu((char*)s.c_str());
	cout<<"role= "<<doc["role"].GetString()<<endl;


}