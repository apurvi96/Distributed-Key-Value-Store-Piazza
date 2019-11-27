#include <bits/stdc++.h>
// #include "GLOBAL_SS.h"
#include "COMMON_FUNCN.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "jsonstring.h"

using namespace rapidjson;

Document doc;

using namespace std;

#define FILENAME "cs_config.txt"

map<string, string> ownmap;
map<string, string> prevmap;

struct thread_data
{
	string ip;
	string port;


};
typedef struct thread_data thread_data;

bool check_key_present(string table,string key)
{
	if(table=="own")
	{

		if(ownmap.find(key)!=ownmap.end())
		{
			return true;
		}

		else
			return false;
	}


	else if(table=="prev")
	{

		if(prevmap.find(key)!=prevmap.end())
		{
			return true;
		}

		else
			return false;
	}
	return false;
}


string handle_get(string jsonFromCS)
{
	
	//parse for table and key
	string table=doc["table"].GetString();
	string key=doc["key"].GetString();

	string fail="key_error";

	if(table=="own")
	{
		if(check_key_present(table,key)==false)
		{
			return fail;
		}

		return ownmap[key];
	}

	else if(table=="prev")
	{
		//TODO: check if key exist return fail
		if(check_key_present(table,key)==false)
		{
			return fail;
		}

		return prevmap[key];
	}
	return "";
}

void handle_put(string jsonFromCS)
{

	string table=doc["table"].GetString();
	string key=doc["key"].GetString();
	string value=doc["value"].GetString();

	if(table=="own")
	{
		ownmap[key]=value;
	}

	else if(table=="prev")
	{
		prevmap[key]=value;
	}


}

string handle_update(string jsonFromCS)
{

	string table=doc["table"].GetString();
	string key=doc["key"].GetString();
	string value=doc["value"].GetString();

	string success="update_success";
	string fail="key_error";

	if(table=="own")
	{
		
		if(check_key_present(table,key)==false)
		{
			return fail;
		}

		ownmap[key]=value;
		return success;
	}

	else if(table=="prev")
	{
		if(check_key_present(table,key)==false)
		{
			return fail;
		}
		prevmap[key]=value;
		return success;
	}
	return "";
}


string handle_delete(string jsonFromCS)
{
	string table=doc["table"].GetString();
	string key=doc["key"].GetString();

	string success="delete_success";
	string fail="key_error";

	if(table=="own")
	{
		if(check_key_present(table,key)==false)
		{
			return fail;
		}

		ownmap.erase(key);
		return success;
	}

	else if(table=="prev")
	{
		if(check_key_present(table,key)==false)
		{
			return fail;
		}
		prevmap.erase(key);
		return success;
	}
	return "";
}


//heartbeat thread for SS
void *heartbeat_conn(void *ptr){
	string ack;
	int fd;
	string ss_ip, ss_port, cs_ip, cs_port;

	thread_data *tdata=(thread_data*)ptr;
	ss_ip=tdata->ip;
	ss_port=tdata->port;
	fd=initialize_socket(ss_ip, ss_port);



	//Reading ip and port of CS from file
	ifstream file(FILENAME);
	if (file.is_open()) {	    
	    getline(file, cs_ip);
	    getline(file, cs_port); 
	    file.close();
	}

	cout<<"csip: "<<cs_ip<<" cs_port: "<<cs_port<<endl;

	//Connecting to CS
	connect_f(fd, cs_ip, cs_port);

	string x=receive_message(fd);
	cout<<"recvd ack: "<<x<<endl;

	send_message(fd, identity_string("slave_server"));

	//TODO: complete heartbeat functionality
	//TODO : recv ack from CS for waiting

}

//thread that serves requests that it'll get from CS
void *serve_request(void *ptr)
{
	thread_data *tdata=(thread_data *)ptr;
	string ip_address=tdata->ip;
	string port_number=tdata->port;

	int sock_fd=initialize_socket(ip_address,port_number);

	//listen
	if(listen(sock_fd,10)<0)
	{
		perror("LISTEN FAILED");
		
	}

	//create client server
	int ip_client_length,client_fd;
	struct sockaddr_in ip_client;

	//accept while
	while((client_fd=accept(sock_fd,(struct sockaddr *)&ip_client,(socklen_t*)&ip_client_length))>0)
	{
		string jsonFromCS, role;

		jsonFromCS=receive_message(client_fd);
		if(doc.ParseInsitu((char*)jsonFromCS.c_str()).HasParseError()){
			cout << "Error while parsing the json string recvd from CS (for finding role). Try again" << endl;
			send_message(client_fd, ack_data_string("ack", "parse_error"));
			continue;
		}
		role=doc["role"].GetString();

		if(role=="get"){
			string val=handle_get(jsonFromCS);
			if(val=="key_error") send_message(client_fd, ack_data_string("ack", val));
			else send_message(client_fd, ack_data_string("data", val));//val will be the value corres to the key
		}
		else if(role=="put"){
			handle_put(jsonFromCS);

			cout<<"Key given: "<<doc["key"].GetString()<<"; Value put: "<<ownmap[doc["key"].GetString()]<<endl;
			send_message(client_fd, ack_data_string("ack", "put_success"));//no poss of key_error in put
		}
		else if(role=="update"){
			string val=handle_update(jsonFromCS);
			send_message(client_fd, ack_data_string("ack", val));//val will be either update_success or key_error
		}
		else if(role=="delete"){
			string val=handle_delete(jsonFromCS);
			send_message(client_fd, ack_data_string("ack", val));//val will be either delete_success or key_error
		}		
		else if(role=="leader"){

		}
		else if(role=="pre"){

		}
		else if(role=="succ_of_succ"){

		}
		else{
			cout<<"Wrong ip sent to SS by CS. Try again!"<<endl;
		}

	}

}

int main(int argc,char **argv)
{

	if(argc<=2)
	{
		cout<<"enter ip port "<<endl;
		return 0;

	}

	thread_data *ss_struct=new thread_data;

	//adding values to thred-data
	ss_struct->ip=argv[1];
	ss_struct->port=argv[2];

	pthread_t heartbeat,serve_req;
	// pthread_create(&heartbeat,NULL,heartbeat_conn,(void *)ss_struct);
	pthread_create(&serve_req,NULL,serve_request,(void *)ss_struct);

	// pthread_join(heartbeat,NULL);
	pthread_join(serve_req,NULL);


}