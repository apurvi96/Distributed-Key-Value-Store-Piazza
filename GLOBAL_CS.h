#ifndef ABC
#define ABC
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

struct thread_struct
{
	int sockfd; 
	string ip_plus_port;
	string ip_port_CS;
};


#endif