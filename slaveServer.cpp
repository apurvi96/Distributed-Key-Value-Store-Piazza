#include <bits/stdc++.h>
// #include "GLOBAL_SS.h"
#include "COMMON_FUNCN.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "jsonstring.h"
#include <semaphore.h>

using namespace rapidjson;

Document doc,doc1,doc2;

using namespace std;

#define FILENAME "cs_config.txt"

map<string, string> ownmap;
map<string, string> prevmap;

int readers=0;//indicating there are no readers in critical section
int writers=1;//Allowing 1 writer at a time in critical section
mutex mx;

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

void print_map(string map_name){

	cout<<"----------------------------------------------"<<endl;
	// if(map_name=="own"){
		cout<<"Printing ownmap:"<<endl;
		for(auto x: ownmap){
			cout<<x.first<<"	"<<x.second<<endl;
		}
	// }

	cout<<"----------------------------------------------"<<endl;

	// if(map_name=="prev"){
		cout<<"Printing prevmap:"<<endl;
		for(auto x: prevmap){
			cout<<x.first<<"	"<<x.second<<endl;
		}
	// }
	cout<<"-----------------------------------------------"<<endl;
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
	assert(doc.HasMember("succ_of_succ_ip"));

	assert(doc["pre_ip"].IsString());
	// assert(doc["succ_ip"].IsString());
	assert(doc["succ_of_succ_ip"].IsString());

	string pre_ipport, succ_ipport, succ_of_succ_ipport;
	pre_ipport=doc["pre_ip"].GetString();//has ip:port of predeceesor
	// succ_ip_ipport=doc["succ_ip"].GetString();
	succ_of_succ_ipport=doc["succ_of_succ_ip"].GetString();

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

	// int sock_fd=initialize_socket(ss_ip,ss_port);
	int sock_fd=initialize_socket_without_bind();// SS that was acting as a server, now wants to communicate to 
	//other SS as a client(to get their tables for data_migration)..hence no need to bind
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
	print_map("");
	
}

void receive_table(int sockfd, string table){// 1.leader will receive pred's own table into it's prev table
								//2. succ_of_succ will receive leader's own table into it's prev table

	string resp_table=receive_message(sockfd);
	if(resp_table=="" || resp_table.empty()){
		cout<<"Recieved empty table"<<endl;
		return;
	}
	cout<<"Here's the map content recvd: "<<resp_table<<endl;
	vector<string> kv;
	char* keyval = strtok((char*)resp_table.c_str(), "|"); 
	// cout<<"keyval: "<<keyval<<endl;
	kv.push_back(keyval);
	while ((keyval = strtok(NULL, "|"))!=NULL) {
        // cout<<keyval<<endl;
        kv.push_back(keyval);
    }

    cout<<"recvd table name: "<<table<<endl;
    if(table=="own"){
    	cout<<"In own"<<endl;
    	ownmap.clear();
    	for(int i=0;i<kv.size();i++){
			char* s=(char*)kv[i].c_str();
			string key=strtok(s,":");
			string val=strtok(NULL,":");
			cout<<"key: "<<key<<" val: "<<val<<endl;
			ownmap[key]=val;
		}
    }
    if(table=="prev"){
    	cout<<"In prev"<<endl;
    	prevmap.clear();
    	for(int i=0;i<kv.size();i++){
    		// cout<<"In kv for"<<endl;
			char* s=(char*)kv[i].c_str();
			// cout<<"char* s in kv vect: "<<s<<endl;
			string key=strtok(s,":");
			// cout<<"key after STRTOK: "<<key<<" and s now: "<<s<<endl;
			string val=strtok(NULL,":");
			// cout<<"val after STRTOK: "<<val<<endl;
			cout<<"key: "<<key<<" val: "<<val<<endl;;
			prevmap[key]=val;
		}
    }
    print_map("");

}

void handle_leader(string ss_ip,string ss_port,int coord_fd)
{
	char *pre_ip1, *pre_port1, *succ_of_succ_ip1, *succ_of_succ_port1;



	assert(doc.HasMember("pre_ip"));
	// assert(doc.HasMember("succ_ip"));
	assert(doc.HasMember("succ_of_succ_ip"));

	assert(doc["pre_ip"].IsString());
	// assert(doc["succ_ip"].IsString());
	assert(doc["succ_of_succ_ip"].IsString());

	string pre_ipport, succ_ipport, succ_of_succ_ipport;
	pre_ipport=doc["pre_ip"].GetString();//has ip:port of predeceesor
	// succ_ip_ipport=doc["succ_ip"].GetString();
	succ_of_succ_ipport=doc["succ_of_succ_ip"].GetString();

	// char* pre_ip1, pre_port1, succ_of_succ_ip1,succ_of_succ_port1;
	pre_ip1=strtok((char*)pre_ipport.c_str(),":");
	pre_port1=strtok(NULL,":");
	succ_of_succ_ip1=strtok((char*)succ_of_succ_ipport.c_str(),":");
	succ_of_succ_port1=strtok(NULL,":");
	// cout<<"ip ports being fetched: "<<pre_ip1<<" "<<pre_port1<<" "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;



	// fetch_ip_ports(pre_ip1, pre_port1, succ_of_succ_ip1,succ_of_succ_port1);
	cout<<"ip ports recvd: "<<pre_ip1<<" "<<pre_port1<<" "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;

	//updating own table of leader(leader is the successor of the slave server that went down) using its prev table
	update_own();

	//updating prev table of leader using own table of predecessor
	int sock_fd;
	sock_fd=connect_pre(ss_ip,ss_port,pre_ip1,pre_port1,"pre","own");
	receive_table(sock_fd,"prev");

	close(sock_fd); //TODO : needed?

	//sending own table of leader to update prev table of succ_of_succ 
	sock_fd=connect_pre(ss_ip,ss_port,succ_of_succ_ip1,succ_of_succ_port1,"succ_of_succ","prev");
	receive_message(sock_fd);//msg recvd: ready_for_table
	send_table(sock_fd,"own");

	send_message(coord_fd,send_message_ready("migration_completed"));//sending to CS
	close(sock_fd);
}
//-----------------------functions to handle new register migration---------------------------------------------------------------
void new_ss_fetch_ip_ports(char* pre_ip1, char* pre_port1,char* succ_ip1, char* succ_port1, char* succ_of_succ_ip1, char* succ_of_succ_port1){
	assert(doc2.HasMember("pre_ip"));
	assert(doc2.HasMember("succ_ip"));
	assert(doc2.HasMember("succ_of_succ_ip"));

	assert(doc2["pre_ip"].IsString());
	assert(doc2["succ_ip"].IsString());
	assert(doc2["succ_of_succ_ip"].IsString());

	string pre_ipport, succ_ipport, succ_of_succ_ipport;
	pre_ipport=doc2["pre_ip"].GetString();//has ip:port of predeceesor
	succ_ipport=doc2["succ_ip"].GetString();
	succ_of_succ_ipport=doc2["succ_of_succ_ip"].GetString();

	// char* pre_ip1, pre_port1, succ_of_succ_ip1,succ_of_succ_port1;

	pre_ip1=strtok((char*)pre_ipport.c_str(),":");
	pre_port1=strtok(NULL,":");
	succ_ip1=strtok((char*)succ_ipport.c_str(),":");
	succ_port1=strtok(NULL,":");
	
	succ_of_succ_ip1=strtok((char*)succ_of_succ_ipport.c_str(),":");
	succ_of_succ_port1=strtok(NULL,":");
	cout<<"ip ports being fetched: "<<pre_ip1<<" "<<pre_port1<<" "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;

}
//------------------------------------------------------------------------------------------------------------------

int new_ss_connect(string ss_ip,string ss_port,string succ_ip,string succ_port,string role,string pre_ip_to_send,string ss_ip_to_send,string succ_of_succ_ip_to_send)
{
	// int sock_fd=initialize_socket(ss_ip,ss_port);
	int sock_fd=initialize_socket_without_bind();
	connect_f(sock_fd,succ_ip,succ_port);
	send_message(sock_fd,inform_leader_migration(role,pre_ip_to_send,ss_ip_to_send,succ_of_succ_ip_to_send));
	return sock_fd;
}

void handle_new_ss_leader(string new_ss_ip,string new_ss_port,string mig_from_cs)
{
	cout<<"Inside handle_new_ss_leader"<<endl;
	char *pre_ip1, *pre_port1,*succ_ip1, *succ_port1, *succ_of_succ_ip1, *succ_of_succ_port1;




	assert(doc2.HasMember("pre_ip"));
	assert(doc2.HasMember("succ_ip"));
	assert(doc2.HasMember("succ_of_succ_ip"));

	assert(doc2["pre_ip"].IsString());
	assert(doc2["succ_ip"].IsString());
	assert(doc2["succ_of_succ_ip"].IsString());

	string pre_ipport, succ_ipport, succ_of_succ_ipport;
	pre_ipport=doc2["pre_ip"].GetString();//has ip:port of predeceesor
	succ_ipport=doc2["succ_ip"].GetString();
	succ_of_succ_ipport=doc2["succ_of_succ_ip"].GetString();

	// char* pre_ip1, pre_port1, succ_of_succ_ip1,succ_of_succ_port1;

	pre_ip1=strtok((char*)pre_ipport.c_str(),":");
	pre_port1=strtok(NULL,":");
	succ_ip1=strtok((char*)succ_ipport.c_str(),":");
	succ_port1=strtok(NULL,":");
	
	succ_of_succ_ip1=strtok((char*)succ_of_succ_ipport.c_str(),":");
	succ_of_succ_port1=strtok(NULL,":");
	// cout<<"ip ports being fetched: "<<pre_ip1<<" "<<pre_port1<<" "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;





	// new_ss_fetch_ip_ports(pre_ip1, pre_port1,succ_ip1,succ_port1,succ_of_succ_ip1,succ_of_succ_port1);
	cout<<"ip ports recvd: "<<pre_ip1<<" "<<pre_port1<<" "<<succ_ip1<<" "<<succ_port1<<" "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;

	int sockfd;
	//new slave server connects to its succ to get succ's prev into its own.
	string pre_ip_to_send=string(pre_ip1)+":"+string(pre_port1);
	string ss_ip_to_send=new_ss_ip+":"+new_ss_port;
	string succ_of_succ_ip_to_send=string(succ_of_succ_ip1)+":"+string(succ_of_succ_port1);
	sockfd=new_ss_connect(new_ss_ip,new_ss_port,succ_ip1,succ_port1,"new_ss_succ",pre_ip_to_send,ss_ip_to_send,succ_of_succ_ip_to_send);
	receive_table(sockfd,"own");

	//new slave server connects to its pre to get pre's own into its prev
	sockfd=new_ss_connect(new_ss_ip,new_ss_port,pre_ip1,pre_port1,"new_ss_pre","","","");
	receive_table(sockfd,"prev");


}

void update_new_ss_own(int llimit, int ulimit){
	prevmap.clear();
	vector<string> keys_to_delete;
	if(llimit<ulimit){
		for(auto x:ownmap){
			int hashed_key=consistent_hash(x.first);

			cout<<"0. key: "<<x.first<<", hashed key: "<<hashed_key<<endl;

			if(llimit<=hashed_key && hashed_key<ulimit)
			{
				cout<<"0.entered if for hashed key: "<<hashed_key<<endl;
				prevmap.insert({x.first,x.second});//have put value from ownmap to its prevmap
				cout<<"0.have put value from ownmap to its prevmap"<<endl;
				keys_to_delete.push_back(x.first);
				// ownmap.erase(x.first);//don't modify map while accessing its elements (in for)
			}
		}
		for(int i=0;i<keys_to_delete.size();i++){
			ownmap.erase(keys_to_delete[i]);
		}
	}
	
	else if(llimit>ulimit){//due to ring
		cout<<"ownmap size:"<<ownmap.size()<<endl;
		for(auto x:ownmap){
			int hashed_key=consistent_hash(x.first);

			cout<<"1. key: "<<x.first<<", hashed key: "<<hashed_key<<endl;

			if((llimit<=hashed_key && hashed_key<RING_SIZE) || (0<=hashed_key && hashed_key<ulimit))
			{
				cout<<"1.entered if for hashed key: "<<hashed_key<<endl;
				prevmap.insert({x.first,x.second});//have put value from ownmap to its prevmap
				cout<<"1.have put value from ownmap to its prevmap"<<endl;
				keys_to_delete.push_back(x.first);
				// ownmap.erase(x.first);//don't modify map while accessing its elements (in for)
			}
		}
		for(int i=0;i<keys_to_delete.size();i++){
			ownmap.erase(keys_to_delete[i]);
		}
	}
	else{
		cout<<"OH! llimit==ulimit"<<endl;
	}

}

void handle_new_ss_succ(string jsonFromSS, string ip_address,string port_number)
{
	cout<<"Entered handle_new_ss_succ"<<endl;
	char *succ_of_succ_ip1, *succ_of_succ_port1;

	//For consistent hashing, get pre_ip:port, succ_of_succ_ip:port, and to connect to succ_of_succ, get the ip and port of succ_of_succ separately

	assert(doc.HasMember("pre_ip"));
	assert(doc.HasMember("succ_ip"));
	assert(doc.HasMember("succ_of_succ_ip"));

	assert(doc["pre_ip"].IsString());
	assert(doc["succ_ip"].IsString());
	assert(doc["succ_of_succ_ip"].IsString());

	string pre_ipport, new_ss_ipport, succ_of_succ_ipport, succ_of_succ_ipport1;
	pre_ipport=doc["pre_ip"].GetString();//has ip:port of predeceesor
	new_ss_ipport=doc["succ_ip"].GetString();
	succ_of_succ_ipport=doc["succ_of_succ_ip"].GetString();

	succ_of_succ_ipport1=succ_of_succ_ipport;

	succ_of_succ_ip1=strtok((char*)succ_of_succ_ipport1.c_str(),":");
	succ_of_succ_port1=strtok(NULL,":");
	cout<<"ip ports being fetched: "<<succ_of_succ_ip1<<" "<<succ_of_succ_port1<<endl;

	cout<<"Param being passed to consistent_hash:- pre_ipport: "<<pre_ipport<<" new_ss_ipport: "<<new_ss_ipport<<endl;
	int lower_limit=consistent_hash(pre_ipport);
	cout<<"lower limit: "<<lower_limit<<endl;
	int upper_limit=consistent_hash(new_ss_ipport);
	cout<<"upper_limit: "<<upper_limit<<endl;
	update_new_ss_own(lower_limit, upper_limit);

	if(ip_address==succ_of_succ_ip1 && port_number==succ_of_succ_port1){
		return;
	}
	int fd=new_ss_connect(ip_address, port_number, succ_of_succ_ip1, succ_of_succ_port1, "new_ss_succ_of_succ", "","","");



	string rmsg=receive_message(fd);//receives ack here other slave server: ready_for_table
	cout<<"revcd msg after new_ss_connect:"<<rmsg<<endl;



	send_table(fd, "own");
	string rmsg1=receive_message(fd); //recvd msg (from SS): ack+new_ss_succ_of_succ_done
	cout<<"revcd msg1 after new_ss_connect:"<<rmsg1<<endl;
	close(fd);

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

	string x=receive_message(fd);//msg received: ack + connected
	cout<<"recvd ack: "<<x<<endl;

	send_message(fd, identity_string("slave_server"));

	string recv_msg=receive_message(fd);//1. msg received: ack + registration_successful (case for 1st server/single server)
										//2. msg received: ack + migration_new_server

	//parse receive msg

	if(doc1.ParseInsitu((char*)recv_msg.c_str()).HasParseError()){
		cout << "Error while parsing the json string recvd from CS (for finding message). Try again. json string: " << recv_msg<<endl;
		send_message(fd, ack_data_string("ack", "parse_error"));
		pthread_exit(NULL);
	}

	assert(doc1.IsObject());
	assert(doc1.HasMember("message")); //checks if doc1 has member named "role"
	assert(doc1["message"].IsString());
	
	string message=doc1["message"].GetString();

	if(message=="migration_new_server")
	{
		send_message(fd, ack_data_string("ack","ready_for_migration"));//sending to CS

		cout<<"waiting to recv msg having ip ports of 3 from CS"<<endl;
		string mig_from_cs=receive_message(fd);//msg recvd: new_SS_leader + ip:ports of pre, succ, succ_of_succ
		cout<<"recvd msg to new server..migration time: "<<mig_from_cs<<endl;

		//extract role
		if(doc2.ParseInsitu((char*)mig_from_cs.c_str()).HasParseError()){
			cout << "Error while parsing the json string recvd from CS (for finding role). Try again. json string: " << mig_from_cs << endl;
			send_message(fd, ack_data_string("ack", "parse_error"));
			pthread_exit(NULL);
		}

		assert(doc2.IsObject());
		assert(doc2.HasMember("role")); //checks if doc2 has member named "role"
		assert(doc2["role"].IsString());
		string role=doc2["role"].GetString();

		cout<<"role of new server: "<<role<<endl;

		//Newly registered Slave Server is the new_ss_leader
		if(role=="new_ss_leader")
		{
			handle_new_ss_leader(ss_ip,ss_port,mig_from_cs);
			send_message(fd,ack_data_string("ack","migration_ss_done"));
		}
	}
	close(fd);

	while(1)
	{
		//make udp connection to cs for heartbeat
		int udp_fd;
		struct sockaddr_in servaddr;

		//make structure
		memset(&servaddr, '\0', sizeof(servaddr));

		servaddr.sin_addr.s_addr = inet_addr(cs_ip.c_str()); 
	    servaddr.sin_port = htons(UDP_PORT); 
	    servaddr.sin_family = AF_INET;  

	    // create datagram socket 
	    udp_fd = socket(AF_INET, SOCK_DGRAM, 0); 
	      
	    // connect to server 
	    if(connect(udp_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	    { 
	        printf("\n Error : Connect Failed \n"); 
	        exit(0); 
	    } 

	    //send udp message
	    string to_send=ss_ip+":"+ss_port;
	    send_message(udp_fd,ack_data_string("heartbeat",to_send));

	    sleep(4);

	}

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
		cout<<"Inside while(accept) in serveReq"<<endl;

		jsonFromCS=receive_message(client_fd);//msg recvd: {role} + ip:ports of pre, succ(self), succ_of_succ
		cout<<"recvd msg: "<<jsonFromCS<<endl;
		if(doc.ParseInsitu((char*)jsonFromCS.c_str()).HasParseError()){
			cout << "Error while parsing the json string recvd from CS (for finding role). Try again. json string: " << jsonFromCS << endl;
			send_message(client_fd, ack_data_string("ack", "parse_error"));
			continue;
		}

		assert(doc.IsObject());
		assert(doc.HasMember("role")); //checks if doc has member named "role"
		assert(doc["role"].IsString());
		
		role=doc["role"].GetString();
		cout<<"role(@serve_request): "<<role<<endl;

		if(role=="get"){
			//----------locking--------------------
			int flag=0; //setting flag in each reader(1st) to know if it has acquired lock then it should unlock while exiting cs
			if(writers==0){//i.e a writer is in critical section
				mx.lock();//1st reader will try to acquire lock; 
				flag=1;//1st reader will set flag=1 to know it has acquired the lock and need to release it
			}
			readers++;//count of readers in critical section
			string val=handle_get(jsonFromCS);

			if(val=="key_error") send_message(client_fd, ack_data_string("ack", val));
			else send_message(client_fd, ack_data_string("data", val));//val will be the value corres to the key

			readers--;//decremented as soon as a reader exits critical section
			if(flag==1){
				mx.unlock();//release the lock if acquired identified with the help of flag
			}
			//---------unlocked--------------------

			print_map("");
		}
		else if(role=="put"){
			//---------implementing lock------------
			mx.lock();
			while(readers!=0);
			writers--;

			handle_put(jsonFromCS);

			writers++;
			mx.unlock();
			//---------unlocked--------------------

			assert(doc.HasMember("table"));
			assert(doc["table"].IsString());
			string table=doc["table"].GetString();

			if(table=="own")
			{
				cout<<"Key given: "<<doc["key"].GetString()<<"; Value put: "<<ownmap[doc["key"].GetString()]<<endl;
			}

			else if(table=="prev")
			{
				cout<<"Key given: "<<doc["key"].GetString()<<"; Value put: "<<prevmap[doc["key"].GetString()]<<endl;
			}

			
			send_message(client_fd, ack_data_string("ack", "put_success"));//no poss of key_error in put
			print_map(table);

		}
		else if(role=="update"){
			//---------implementing lock------------
			mx.lock();
			while(readers!=0);
			writers--;

			string val=handle_update(jsonFromCS);
			send_message(client_fd, ack_data_string("ack", val));//val will be either update_success or key_error

			writers++;
			mx.unlock();
			//---------unlocked--------------------

			assert(doc.HasMember("table"));
			assert(doc["table"].IsString());
			string table=doc["table"].GetString();
			print_map(table);

		}
		else if(role=="delete"){
			//----------locking--------------------
			mx.lock();
			while(readers!=0);
			writers--;

			string val=handle_delete(jsonFromCS);
			send_message(client_fd, ack_data_string("ack", val));//val will be either delete_success or key_error

			writers++;
			mx.unlock();
			//---------unlocked--------------------
			
			assert(doc.HasMember("table"));
			assert(doc["table"].IsString());
			string table=doc["table"].GetString();
			print_map(table);
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
			send_message(client_fd,send_message_ready("ready_for_table"));
			assert(doc.HasMember("table")); //checks if doc has member named "table"
			assert(doc["table"].IsString());
			string table1=doc["table"].GetString();



			
			//updating prev table of succ_of_succ to update own table of leader  



			receive_table(client_fd,table1);

		}

		else if(role=="new_ss_pre")
		{
			send_table(client_fd,"own");
		}

		else if(role=="new_ss_succ")
		{
			handle_new_ss_succ(jsonFromCS,ip_address,port_number);
			send_table(client_fd, "prev");
		}

		else if(role=="new_ss_succ_of_succ")
		{
			send_message(client_fd, send_message_ready("ready_for_table"));
			receive_table(client_fd, "prev");//(new server)receives own table of new server's succr in its prev table
			send_message(client_fd, ack_data_string("ack", "new_ss_succ_of_succ_done"));
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