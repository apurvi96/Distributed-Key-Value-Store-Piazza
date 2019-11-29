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
{   
    string ip=ip_address;
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


void put_on_slave(int sock_fd,char *ip_address,char *port_number,string cs_ip,string cs_port,string key,string value,string table)
{   string ip=ip_address;
    string port=port_number;
    // we need connection two times one for succ and other for succ_of succ and also for put so making connect function
    
    
    // send key to get value from SS
    send_message(sock_fd,put_update_SS("put",key,value,table));
    string value1=receive_message(sock_fd);
    //cout<<"in put_on_slave -- value received from slave "<<value1<<endl;
    cout<<"value placed in "<<table<<" "<<"value is : "<<value1<<endl;
    
}


string delete_from_slave(int sock_fd,string ip_address,string port_number,string cs_ip,string cs_port,string key,string table)
{
    string ip=ip_address;
    string port=port_number;
    // we need connection two times one for succ and other for succ_of succ and also for put so making connect function
    
    
    // send key to get value from SS
    send_message(sock_fd,get_delete_SS("delete",key,table));
    string value=receive_message(sock_fd);
    cout<<"in delete_from_slave -- value received from slave "<<value<<endl;
    return value;
}

void update_on_slave(char *ip_address,char *port_number,string cs_ip,string cs_port,string key,string value,string table)
{   string ip=ip_address;
    string port=port_number;
    // we need connection two times one for succ and other for succ_of succ and also for put so making connect function
    
    int sock_fd=connect_to_the_slave(ip,port,cs_ip,cs_port);
    if(sock_fd<0)   
        cout<<"Not connected";
    // send key to get value from SS
    send_message(sock_fd,put_update_SS("update",key,value,table));
    string value1=receive_message(sock_fd);
    //cout<<"in put_on_slave -- value received from slave "<<value1<<endl;
    cout<<"value placed in "<<table<<" "<<"value is : "<<value1<<endl;
    
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
    char ar[1024];
    strcpy(ar,ip_port_avl);
    char* ip_address=strtok(ar,":");
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
      strcpy(ar,ip_port_avl1);
      char* ip_address1=strtok(ar,":");
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




void serve_put_request(int connectfd,string key,string value,string ip_port_cs)
{
   

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
    {    
        cout<<"No slave server is active"<<endl;
        send_message(connectfd,ack_data_string("ack","no_slave_server_failed"));
        return;
    }
    
    //extract ip and port of succ slave server
   char* ip_port_avl=(char*)(succ->ip_plus_port).c_str();
    char ar[1024];
    strcpy(ar,ip_port_avl); 
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");
    cout<<"ip_plus_port_after TOK "<<succ->ip_plus_port<<endl;

    // try to connect succ slave server
    int sock_fd=connect_to_the_slave(ip_address,port_number,cs_ip,cs_port);
    

    int hash_value1=consistent_hash(succ->ip_plus_port);
    cout<<"hashed value key hashed"<<hash_value1<<endl;

    // get IP and port of successor from AVL
    avl_tree av1;
    Node *pre1,*succ1; 
    av.Suc(root,pre1,succ1,hash_value1);

    char* ip_port_avl1=(char*)(succ1->ip_plus_port).c_str();
    strcpy(ar,ip_port_avl1);
    char* ip_address1=strtok(ar,":");
    char* port_number1=strtok(NULL,":");

    // try to connect succ_of_succ slave server
    int sock_fd1=connect_to_the_slave(ip_address1,port_number1,cs_ip,cs_port);
    
    if(sock_fd > 0 && sock_fd1 > 0)
    {
        // put on succ
        put_on_slave(sock_fd,ip_address,port_number,cs_ip,cs_port,key,value,"own");
        //put on succ_of_succ
        put_on_slave(sock_fd1,ip_address1,port_number1,cs_ip,cs_port,key,value,"prev");
        send_message(connectfd,ack_data_string("ack","put_success"));
    }

    else
        send_message(connectfd,ack_data_string("ack","commit_failed"));
   
}


void serve_delete_request(int connectfd,string key,string ip_port_cs)
{
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
    {   
        cout<<"No slave server is active"<<endl;
        send_message(connectfd,ack_data_string("ack","delete_failed"));
        return;
    }

    //extract ip and port of succ slave server
    char* ip_port_avl=(char*)(succ->ip_plus_port).c_str();
    char ar[1024];
    strcpy(ar,ip_port_avl);
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");

    int sock_fd1=connect_to_the_slave(connectfd,ip_address,port_number,cs_ip,cs_port);
    
    // find succ_of_succ
     int hash_value1=consistent_hash(succ->ip_plus_port);
    cout<<"hashed value key hashed"<<hash_value1<<endl;

    // get IP and port of successor from AVL
    avl_tree av1;
    Node *pre1,*succ1; 
    av.Suc(root,pre1,succ1,hash_value1);

   

    char* ip_port_avl1=(char*)(succ1->ip_plus_port).c_str();
    strcpy(ar,ip_port_avl1);
    char* ip_address1=strtok(ar,":");
    char* port_number1=strtok(NULL,":");

    int sock_fd2=connect_to_the_slave(connectfd,ip_address1,port_number1,cs_ip,cs_port);

    int value,value1;

    if(sock_fd1 > 0 && sock_fd2 > 0)
    {
        value=delete_from_slave(ip_address,port_number,cs_ip,cs_port,key,"own");
        value1=delete_from_slave(ip_address1,port_number1,cs_ip,cs_port,key,"prev");
        send_message(connectfd,value1);
    }
    else
        send_message(connectfd,ack_data_string("ack","commit_failed"));
      

}






void serve_update_request(int connectfd,string key,string value,string ip_port_cs)
{

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
    char ar[1024];
    strcpy(ar,ip_port_avl); 
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");
    cout<<"ip_plus_port_after TOK "<<succ->ip_plus_port<<endl;
     update_on_slave(ip_address,port_number,cs_ip,cs_port,key,value,"own");
     //send_message(connectfd,ack_data_string("ack","put_success"));
  
   // putting value in prev table in succ_of_succ
   
    int hash_value1=consistent_hash(succ->ip_plus_port);
    cout<<"hashed value key hashed"<<hash_value1<<endl;

    // get IP and port of successor from AVL
    avl_tree av1;
    Node *pre1,*succ1; 
    av.Suc(root,pre1,succ1,hash_value1);
    if(succ1 != NULL)
    {
        cout<<"fetched form AVL"<<succ1->key<<" "<<succ1->ip_plus_port<<endl;
        //extract ip and port of succ slave server
        char* ip_port_avl1=(char*)(succ1->ip_plus_port).c_str();
         strcpy(ar,ip_port_avl1);
        char* ip_address1=strtok(ar,":");
        char* port_number1=strtok(NULL,":");
        //put value in own table of successor
        update_on_slave(ip_address1,port_number1,cs_ip,cs_port,key,value,"prev");
    }
    else
        cout<<"No other slave server is active"<<endl;
    
    
    send_message(connectfd,ack_data_string("ack","update_success"));


}



<<<<<<< HEAD



void serve_update_request(int connectfd,string key,string value,string ip_port_cs)
{

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
    char ar[1024];
    strcpy(ar,ip_port_avl); 
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");
    cout<<"ip_plus_port_after TOK "<<succ->ip_plus_port<<endl;
     update_on_slave(ip_address,port_number,cs_ip,cs_port,key,value,"own");
     //send_message(connectfd,ack_data_string("ack","put_success"));
  
   // putting value in prev table in succ_of_succ
   
    int hash_value1=consistent_hash(succ->ip_plus_port);
    cout<<"hashed value key hashed"<<hash_value1<<endl;

    // get IP and port of successor from AVL
    avl_tree av1;
    Node *pre1,*succ1; 
    av.Suc(root,pre1,succ1,hash_value1);
    if(succ1 != NULL)
    {
        cout<<"fetched form AVL"<<succ1->key<<" "<<succ1->ip_plus_port<<endl;
        //extract ip and port of succ slave server
        char* ip_port_avl1=(char*)(succ1->ip_plus_port).c_str();
         strcpy(ar,ip_port_avl1);
        char* ip_address1=strtok(ar,":");
        char* port_number1=strtok(NULL,":");
        //put value in own table of successor
        put_on_slave(ip_address1,port_number1,cs_ip,cs_port,key,value,"prev");
    }
    else
        cout<<"No other slave server is active"<<endl;
    
    
    send_message(connectfd,ack_data_string("ack","update_success"));


}



=======
>>>>>>> updated json
void request_of_client(int connectfd,string ip_port_cs)
{
    string s;
    while(1)
    {
        /* TODO Coord mar ra h client k jate hi why why why??? 
            handle when null in avl(succ is largest one) initialise to the minimum
        */
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

        else if(strcmp(document1["req_type"].GetString(),"put")==0)
        {  
            
          /*TODO  testing with two slave servers
            check value and iterate map
            */

            assert(document1.IsObject());
            assert(document1.HasMember("key"));
            assert(document1.HasMember("value"));
            string key_v=document1["key"].GetString();
            string value_v=document1["value"].GetString();
            serve_put_request(connectfd,key_v,value_v,ip_port_cs);
             
        }  

        else if(strcmp(document1["req_type"].GetString(),"delete")==0)
        {   
            

            /* TODO  
                check in cache if present delete and continue
            */
            assert(document1.IsObject());
            assert(document1.HasMember("key"));
            string key_v=document1["key"].GetString();
           // send_message(connectfd,ack_data_string("ack","in parse"));
            serve_delete_request(connectfd,key_v,ip_port_cs);
             
        } 

        
         
        else if(strcmp(document1["req_type"].GetString(),"update")==0)
        {  
            
          /*TODO  testing with two slave servers
            check value and iterate map
            Check if present in cache then update also in cache and continue
            */

            assert(document1.IsObject());
            assert(document1.HasMember("key"));
            assert(document1.HasMember("value"));
            string key_v=document1["key"].GetString();
            string value_v=document1["value"].GetString();
            serve_update_request(connectfd,key_v,value_v,ip_port_cs);
             
        }   

    }



}

#endif