#ifndef COORD_FUNC
#define COORD_FUNC
#include<fstream>
#include "COMMON_FUNCN.h"
#include "avl.hpp"
#include<bits/stdc++.h>
#include "jsonstring.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace std;
using namespace rapidjson;

Document document1;
int connect_without_bind(int sock_id,string ip,string port)
{

    cout<<"in connect_without_bind "<<endl;
  //converting port  to int
    int port1=stoi(port);
    //cout<<"port is "<<port1<<endl;

    //converting ip to const char*
    const char *ip1=ip.c_str();
    //cout<<"ip is"<<ip1<<endl;


    //socket create
    int sock_fd=socket(AF_INET,SOCK_STREAM,0);

        if(sock_fd<0)
        {
            perror("ERROR IN SOCKET CREATION");
        
        }

    //setopt 
    int opt=3;
    int setopt=setsockopt(sock_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt,sizeof(opt));

        if(setopt<0)
        {
            perror(" SOCKOPT FAILED");
                
        }
        cout<<"sock fd is "<<sock_fd<<endl;
       return sock_fd; 


}
// write ip port to file for slave servers and clients to access
void write_to_file(string ip, string port)
{
	ofstream fo;
	fo.open("cs_config.txt");
	fo << ip +"\n" + port;
	fo.close();
}

void register_slave_server(string ip_port)
{
	int hashed_ip=consistent_hash(ip_port);
	cout<<"hashed value"<<hashed_ip<<endl;
	avl_tree av;
	root=av.insert(root,hashed_ip,ip_port);
	av.inorder(root);
}

//connect to slave server for getting key and putting key:

int connect_to_the_slave(string ip,string port,string cs_ip,string cs_port)
{

    //int sock_fd=initialize_socket(cs_ip,cs_port);

    //cout<<"sock fd for cs and slave for get and put key "<<sock_fd<<endl;
    cout<<"ip port while connecting slave "<<ip<<" "<<port<<endl;
    int  sock_fd=connect_without_bind(sock_fd,ip,port);
    int n= connect_f(sock_fd,ip,port);
    if(n<0) 
        return -1;
    return sock_fd;

}

// taking value from the respective slave server 
string get_from_slave(char *ip_address,char *port_number,string cs_ip,string cs_port,string key,string table)
{   string ip=ip_address;
    string port=port_number;
    // we need connection two times one for succ and other for succ_of succ and also for put so making connect function
    
    int sock_fd=connect_to_the_slave(ip,port,cs_ip,cs_port);
    if(sock_fd<0)   
        return "Not connected";
    // send key to get value from SS
    send_message(sock_fd,get_delete_SS("get",key,table));
    string value=receive_message(sock_fd);
    cout<<"in get_from_slave -- value received from slave "<<value<<endl;
    return value;
    

   
}


void serve_get_request(int connectfd,string key,string ip_port_cs)
{

   //TODO check in cache
    /*if(present in chache) 
    give it right away  
    else */
    char* ip_port_cs_char=(char*)(ip_port_cs).c_str();

    // fetch ip and port of CS to connect succ and succ_of_succ
    char* ip_cs_char=strtok(ip_port_cs_char,":");
    char* port_cs_char=strtok(NULL,":");   
    string cs_ip=ip_cs_char;
    string cs_port=port_cs_char;

    int hash_value=consistent_hash(key);
    cout<<"hashed value key hashed"<<hash_value<<endl;

    // get IP and port of successor from AVL
    avl_tree av;
    Node *pre,*succ; 
    av.Suc(root,pre,succ,hash_value);
    if(succ != NULL)
    {
        cout<<"fetched form AVL"<<succ->key<<" "<<succ->ip_plus_port<<endl;
    }
    else
        cout<<"No slave server is active"<<endl;
    
    //extract ip and port of succ slave server
   char* ip_port_avl=(char*)(succ->ip_plus_port).c_str();

    char* ip_address=strtok(ip_port_avl,":");
    char* port_number=strtok(NULL,":");

    // connecting to the respective slave:
    string value=get_from_slave(ip_address,port_number,cs_ip,cs_port,key,"own");
    // if succ slave server is not up:

    if(value=="Not connected")
    {
      
      
      avl_tree av_sec;
      Node *pree,*succ_of_succ;
      string ipport_new_id=ip_port_avl;

      //hash succ that we've retrieved to find succ of succ
      int hash_value_succ_slave=consistent_hash(ipport_new_id);
      av_sec.Suc(root,pree,succ_of_succ,hash_value_succ_slave);
      char* ip_port_avl1=(char*)(succ_of_succ->ip_plus_port).c_str();
      char* ip_address1=strtok(ip_port_avl1,":");
      char* port_number1=strtok(NULL,":");
      string value1=get_from_slave(ip_address1,port_number1,cs_ip,cs_port,key,"prev");
       
      // will send to the client who has requested for the key -value
      //IN json format then client will parse it and retrieve from that
      send_message(connectfd,value1);

    } 
    // if succ slave server is up:
    else
    {
      // succ itself has the key value so return it to the client   
       
        send_message(connectfd,value);

    } 

    




}




void request_of_client(int connectfd,string ip_port_cs)
{
    string s;
    s=receive_message(connectfd);
    if(document1.ParseInsitu((char*)s.c_str()).HasParseError())
    {
    	cout<<"error in request for client parsing string"<<endl;
    	send_message(connectfd,ack_data_string("ack","parse_error"));
    }
    
    else if(strcmp(document1["req_type"].GetString(),"get")==0)
    {   
    	/* TODO TESTING 
            -- more than one SS
            -- when key is present get value
        */
        assert(document1.IsObject());
    	assert(document1.HasMember("key"));
        string key_v=document1["key"].GetString();
       // send_message(connectfd,ack_data_string("ack","in parse"));
        serve_get_request(connectfd,key_v,ip_port_cs);
         
    }



}

#endif