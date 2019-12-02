#ifndef ABC
#define ABC
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <bits/stdc++.h>

map<string,int>heartbeat_count;
bool data_migration=false;		// global variable to check if data_migration is in progress and to pause other requests 
struct thread_struct
{
	int sockfd; 
	string ip_plus_port;
	string ip_port_CS;
};


struct heartbeat_struct
{
	string ip_cs;
};



#endif