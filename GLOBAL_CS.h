#ifndef ABC
#define ABC
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <bits/stdc++.h>

struct thread_struct
{
	int sockfd; 
	string ip_plus_port;
	string ip_port_CS;
};

map<string,int> heartbeat_count;
bool data_migration;

struct heartbeat_struct
{
	string ip_cs;
};

#endif