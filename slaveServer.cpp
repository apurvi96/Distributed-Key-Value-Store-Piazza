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
	assert(doc.HasMember("table"));
	assert(doc.HasMember("key"));
	assert(doc["table"].IsString());
	assert(doc["key"].IsString());
			
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
	assert(doc.HasMember("table"));
	assert(doc.HasMember("key"));
	assert(doc.HasMember("value"));
	assert(doc["table"].IsString());
	assert(doc["key"].IsString());
	assert(doc["value"].IsString());


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
	assert(doc.HasMember("table"));
	assert(doc.HasMember("key"));
	assert(doc.HasMember("value"));
	assert(doc["table"].IsString());
	assert(doc["key"].IsString());
	assert(doc["value"].IsString());

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
	assert(doc.HasMember("table"));
	assert(doc.HasMember("key"));
	assert(doc["table"].IsString());
	assert(doc["key"].IsString());

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


//------------------handle_leader_functions--------------------------------------------------------------------------------------- 

void fetch_ip_ports(char* pre_ip1, char* pre_port1, char* succ_of_succ_ip1, char* succ_of_succ_port1){
	assert(doc.HasMember("pre_ip"));
	// assert(doc.HasMember("succ_ip"));
	assert(doc.HasMember("succ_of_succ"));

	assert(doc["pre_ip"].IsString());
	// assert(doc["succ_ip"].IsString());
	assert(doc["succ_of_succ"].IsString());

	string pre_ipport, succ_ipport, succ_of_succ_ipport;
	pre_ipport=doc["pre_ip"].GetString();//has ip:port of predeceesor
	// succ_ip_ipport=doc["succ_ip"].GetString();
	succ_of_succ_ipport=doc["succ_of_succ"].GetString();

	// char* pre_ip1, pre_port1, succ_of_succ_ip1,succ_of_succ_port1;
	pre_ip1=strtok((char*)pre_ipport.c_str(),":");
	pre_port1=strtok(NULL,":");
	succ_of_succ_ip1=strtok((char*)succ_of_succ_ipport.c_str(),":");
	succ_of_succ_port1=strtok(NULL,":");
	cout<<"ip ports being fetched: "<<pre_ip1<<" "<<pre_port1<<" "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;

}

void update_own(){

	for(auto it: prevmap)
	{
		string p_key=it.first;
		string p_value=it.second;
		ownmap[p_key]=p_value;


	}

	prevmap.clear();

}

int connect_pre(string ss_ip,string ss_port,string pre_ip1,string pre_port1,string role,string table)
{

	int sock_fd=initialize_socket(ss_ip,ss_port);
	connect_f(sock_fd,pre_ip1,pre_port1);
	send_message(sock_fd,update_table_SS(role,table));
	return sock_fd;
}

void send_table(int fd, string table){//1. leader will tell the pred to send its own to leader's prev
										//2. leader will send it's own to prev of succ_of succ
	string resp="";
	if(table=="own"){
		//send_message() --send size of map
		for(auto x:ownmap){
			resp+=x.first+":"+x.second+"|";
		}
		resp=resp.substr(0,resp.length()-1);		
	}
	else if(table=="prev"){
		for(auto x:prevmap){
			resp+=x.first+":"+x.second+"|";
		}
		resp=resp.substr(0,resp.length()-1);	
	}

	cout<<"Here's the map content being sent: "<<resp<<endl;
	send_message(fd, resp);

	
}

void receive_table(int sockfd, string table){// 1.leader will receive pred's own table into it's prev table
								//2. succ_of_succ will receive leader's own table into it's prev table

	string resp_table=receive_message(sockfd);
	if(resp_table=="" || resp_table.empty()){
		cout<<"Recieved empty table"<<endl;
		return;
	}
	cout<<"Here's the map content recvd: "<<resp_table<<endl;
	vector<char*> kv;
	char* keyval = strtok((char*)resp_table.c_str(), "|"); 
	cout<<"keyval: "<<keyval<<endl;
	kv.push_back(keyval);
	while (keyval != NULL) {
        keyval = strtok(NULL, "|"); 
        cout<<keyval<<endl;
        kv.push_back(keyval);
    }

  //   if(table=="own"){
  //   	ownmap.clear();
  //   	for(int i=0;i<kv.size();i++){
		// 	char* s=kv[i];
		// 	string key=strtok(s,":");
		// 	string val=strtok(NULL,":");
		// 	cout<<"key: "<<key<<" val: "<<val<<endl;
		// 	ownmap[key]=val;
		// }
  //   }
    if(table=="prev"){
    	prevmap.clear();
    	for(int i=0;i<kv.size();i++){
			char* s=kv[i];
			string key=strtok(s,":");
			string val=strtok(NULL,":");
			cout<<"key: "<<key<<" val: "<<val<<endl;;
			prevmap[key]=val;
		}
    }

}

void handle_leader(string ss_ip,string ss_port,int coord_fd)
{
	char *pre_ip1, *pre_port1, *succ_of_succ_ip1, *succ_of_succ_port1;
	fetch_ip_ports(pre_ip1, pre_port1, succ_of_succ_ip1,succ_of_succ_port1);
	cout<<"ip ports recvd: "<<pre_ip1<<" "<<pre_port1<<" "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;
	update_own();
	int sock_fd;
	sock_fd=connect_pre(ss_ip,ss_port,pre_ip1,pre_port1,"pre","own");
	receive_table(sock_fd,"prev");

	close(sock_fd);
	sock_fd=connect_pre(ss_ip,ss_port,succ_of_succ_ip1,succ_of_succ_port1,"succ_of_succ","prev");
	receive_message(sock_fd);
	send_table(sock_fd,"own");
	send_message(coord_fd,send_message_ready("migration_completed"));
	close(sock_fd);
}

//-----------------------------------heartbeat thread for SS----------------------------------------------------------------------------
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
	receive_message(fd);
	//TODO: complete heartbeat functionality
	

}

//------------------thread that serves requests that it'll get from CS-------------------------------------------------------
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

		assert(doc.IsObject());
		assert(doc.HasMember("role")); //checks if doc has member named "role"
		assert(doc["role"].IsString());
		
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
			handle_leader(ip_address,port_number,client_fd);
		
		}
		else if(role=="pre"){
			assert(doc.HasMember("table")); //checks if doc has member named "table"
			assert(doc["table"].IsString());
			string table1=doc["table"].GetString();
			send_table(client_fd,table1);
			

		}
		else if(role=="succ_of_succ"){
			send_message(client_fd,send_message_ready(“ready_for_table”));
			assert(doc.HasMember("table")); //checks if doc has member named "table"
			assert(doc["table"].IsString());
			string table1=doc["table"].GetString();
			
			receive_table(client_fd,table);

		}
		else{
			cout<<"Wrong input sent to SS by CS. Try again!"<<endl;
		}

	}

}
//------------------------------------------main-----------------------------------------------------------------------------------
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
	 pthread_create(&heartbeat,NULL,heartbeat_conn,(void *)ss_struct);
	pthread_create(&serve_req,NULL,serve_request,(void *)ss_struct);

	 pthread_join(heartbeat,NULL);
	pthread_join(serve_req,NULL);


}