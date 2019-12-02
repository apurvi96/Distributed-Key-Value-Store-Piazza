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
#include "GLOBAL_CS.h"
#include  "lru_cache.hpp"
using namespace std;
using namespace rapidjson;




void new_reg_migration(int sock_fd,string ip_port)
{
    int hash_value=consistent_hash(ip_port);
    avl_tree av;
    Node *pre=NULL,*succ=NULL,*pre1=NULL,*succ_of_succ=NULL; 
    // fetch predecessor and successor of new reg server
    av.Suc(root,pre,succ,hash_value);
    if(succ==NULL)
    {
        succ=av.minValue(root);
    }

    if(pre==NULL)
    {
        pre=av.maxValue(root);
    }


    // fetch pre1 and succ_of_succ of new reg succ
    av.Suc(root,pre1,succ_of_succ,succ->key);

    if(succ_of_succ==NULL)
    {
        succ_of_succ=av.minValue(root);
    } 

    if(pre1==NULL)
    {
        pre1=av.maxValue(root);
    }

    // send to new SS to proceed migration
    send_message(sock_fd,ack_data_string("ack","migration_new_server"));
    cout<<receive_message(sock_fd)<<endl;   // receive msg: ack + "ready_for_migration"
    root=av.insert(root,hash_value,ip_port);
    cout<<"returned form AVL"<<endl;
    cout<<"pre is "<<pre->ip_plus_port<<" succ is "<<succ->ip_plus_port<<" succ_of_succ "<<succ_of_succ->ip_plus_port<<endl;
    send_message(sock_fd,inform_leader_migration("new_ss_leader",pre->ip_plus_port,succ->ip_plus_port, succ_of_succ->ip_plus_port));
    cout<<"returned from send_message "<<endl;
    cout<<receive_message(sock_fd)<<endl;       // receive message: "migration_ss_done" , "parse_error" 
}


//------------------------------------------------------------------------------------------------------------------------

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



//---------------------------------------------------------------------------------------------------------------


void* timer(void* threadargs)
{  

  
  while(1)
  {	
   
   sleep(30);
   cout<<"i am awake to monitor heartbeat!!"<<endl;
   cout<<"------------------------------------------heartbeat_count-----------------------------------------"<<endl;
   for(auto x:heartbeat_count)
        {
            cout<<x.first<<" "<<x.second<<endl;
        }
     cout<<"------------------------------------------------------------------------------------------"<<endl;
   vector<string> values_to_delete; 
   for(auto it=heartbeat_count.begin();it!=heartbeat_count.end();it++)
   {
        cout<<"iterating heartbeat_count map"<<endl;
   	  if(it->second==0)
   	  {
   	  	cout<<"inside it->second=0"<<endl;
        data_migration=true;                   
   	  	int hash_key;
   	  	hash_key=consistent_hash(it->first);
        cout<<"slave server : "<<it->first<<" "<<it->second<<" "<<"hashed at "<<hash_key<<endl;
        avl_tree av;
        Node *pre=NULL,*succ=NULL,*succ_of_succ=NULL,*pre1=NULL;
        av.Suc(root,pre,succ,hash_key); 
        if(succ==NULL)
        {
            succ=av.minValue(root);
        }

        if(pre==NULL)
        {
            pre=av.maxValue(root);
        }

        av.Suc(root,pre1,succ_of_succ,succ->key);

        if(succ_of_succ==NULL)
        {
            succ_of_succ=av.minValue(root);
        } 

        if(pre1==NULL)
        {
            pre1=av.maxValue(root);
        }   
        cout<<"pre is "<<pre->ip_plus_port<<" "<<"succ is "<<succ->ip_plus_port<<" "<<"succ_of_succ is "<<succ_of_succ->ip_plus_port<<endl;
        char ar1[1024];
        char ar[1024];
        char ar2[1024];
        strcpy(ar,(char*)(succ->ip_plus_port).c_str());
        strcpy(ar1,(char*)(succ->ip_plus_port).c_str());
        strcpy(ar2,(char*)(succ_of_succ->ip_plus_port).c_str());
        root=av.deleteNode(root,hash_key);
        cout<<"returned from delete AVL, root is "<<root->key<<" "<<root->ip_plus_port<<endl;
        //erasing slave  from map table
        //heartbeat_count.erase(it->first);
        values_to_delete.push_back(it->first);
       
        cout<<"ar after delete when doing strtok "<<ar<<endl;
        char *ip_address=strtok(ar,":");
        char *port_number=strtok(NULL,":");  
        cout<<"original "<<succ_of_succ->ip_plus_port<<endl;      
        int sock_fd=initialize_socket_without_bind();      //socket fd created with succ SS as leader to proceed with migration
        connect_f(sock_fd,ip_address,port_number);

        // not required since ip and port are not required individually
        /* string pre_ip,succ_ip,succ_of_succ_ip;
        succ_ip=ip_address;

        strcpy(ar,(pre->ip_plus_port).c_str());
        ip_address=strtok(ar,":");
        port_number=strtok(NULL,":");
        pre_ip=ip_address;

        strcpy(ar,(succ_of_succ->ip_plus_port).c_str());
        ip_address=strtok(ar,":");
        port_number=strtok(NULL,":");
        succ_of_succ_ip=ip_address;*/

        send_message(sock_fd,inform_leader_migration("leader",pre->ip_plus_port,ar1,ar2));
        cout<<receive_message(sock_fd); //msg recvd from leader(succ) SS: "migration_completed"  
        

   	  }
      

   }

   for(auto it=values_to_delete.begin(); it!=values_to_delete.end(); it++)
        heartbeat_count.erase(*it);

      for(auto x:heartbeat_count)
        {
            heartbeat_count[x.first]=0;
        }
      
   data_migration=false;
  }  
  


}


//------------------------------------------------------------------------------------------------------------------



void* heartbeat_func(void* threadargs)
{   
    cout<<"inside heartbeat_func:"<<endl;
    Document document1;
    int port_num=UDP_PORT;
    const char* ip_num=(((struct heartbeat_struct*)threadargs)->ip_cs).c_str();

    int connectfd;
    struct sockaddr_in sockaddr_struct;
    int len = sizeof(sockaddr_struct);


    if((connectfd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)

    {
        perror("socket failed");
        pthread_exit(NULL);
    }
    
   // struct sockaddr_in ip_server;
    sockaddr_struct.sin_family = AF_INET;
    sockaddr_struct.sin_addr.s_addr =htonl(INADDR_ANY);
    sockaddr_struct.sin_port=htons(port_num);

    if(bind(connectfd, (struct sockaddr*)&sockaddr_struct, sizeof(sockaddr_struct)) < 0)
    {
        perror("Bind failed in UDP");
        pthread_exit(NULL);
    }

    string receive_val;
    while(1)
    {

        receive_val=receive_message(connectfd);     // received msg from SS: heartbeat + ip:port
        cout<<receive_val<<endl;

        if(document1.ParseInsitu((char*)receive_val.c_str()).HasParseError())
        {
            cout<<"error in request for client parsing string."<<endl;
        }
        
        else if(strcmp(document1["req_type"].GetString(),"heartbeat")==0)
        { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string ip_port=document1["message"].GetString();
            cout<<"increment heartbeat of ip_port"<<ip_port<<endl;
            heartbeat_count[ip_port]++;
            cout<<"incremented as "<<heartbeat_count[ip_port]<<endl;
        }

    }
}



//------------------------------------------------------------------------------------------------------------------

// write ip port to file for slave servers and clients to access
void write_to_file(string ip, string port)
{
	ofstream fo;
	fo.open("cs_config.txt");
	fo << ip +"\n" + port;
	fo.close();
}

//------------------------------------------------------------------------------------------------------------------

void register_slave_server(string ip_port)
{
	int hashed_ip=consistent_hash(ip_port);
	cout<<"hashed value"<<hashed_ip<<endl;
	avl_tree av;
	root=av.insert(root,hashed_ip,ip_port);
	av.preorder(root);
}


//---------------------------------------------------------------------------------------------------------------------

//connect to slave server for getting key and putting key:



// int connect_to_the_slave(string ip,string port,string cs_ip,string cs_port)
// {

//     //int sock_fd=initialize_socket(cs_ip,cs_port);

//     //cout<<"sock fd for cs and slave for get and put key "<<sock_fd<<endl;
//     cout<<"ip port while connecting slave "<<ip<<" "<<port<<endl;
//     int  sock_fd=connect_without_bind(sock_fd,ip,port);
//     // int n= connect_f(sock_fd,ip,port);
//     // if(n<0) 
//     //     return -1;
//     return sock_fd;

// }


//------------------------------------------------------------------------------------------------------------//

// taking value from the respective slave server 
string get_from_slave(int sock_fd,char *ip_address,char *port_number,string cs_ip,string cs_port,string key,string table)
{   Document document1;
    string ip=ip_address;
    string port=port_number;
    // we need connection two times one for succ and other for succ_of succ and also for put so making connect function
    
    // int sock_fd=connect_to_the_slave(ip,port,cs_ip,cs_port);
    // if(sock_fd<0)   
    //     return "Not connected";
    // send key to get value from SS
    send_message(sock_fd,get_delete_SS("get",key,table));
    string value=receive_message(sock_fd);
    cout<<"in get from slave server "<<value<<endl;
    string value1=value;
    if(!(document1.ParseInsitu((char*)value1.c_str()).HasParseError()))
    {
       
        
    
        if(strcmp(document1["req_type"].GetString(),"data")==0)
        {
         assert(document1.IsObject());
         assert(document1.HasMember("message"));
         string value_vv=document1["message"].GetString();
         cache.put_in_cache(key,value_vv);
        } 

    }

    cout<<"in get_from_slave -- value received from slave "<<value<<endl;
    return value;
    
  
}

//-----------------------------------------------------------------------------------------------------------------//

void put_on_slave(int sock_fd,char *ip_address,char *port_number,string cs_ip,string cs_port,string key,string value,string table)
{   string ip=ip_address;
    string port=port_number;
    // we need connection two times one for succ and other for succ_of succ and also for put so making connect function
    
    cout<<"inside put_on_slave"<<endl;   
    // send key to get value from SS
    send_message(sock_fd,put_update_SS("put",key,value,table));
    cout<<"msg sent"<<endl;
    string value1=receive_message(sock_fd);     // received: ack+put_success
    //cout<<"in put_on_slave -- value received from slave "<<value1<<endl;
    cout<<"value placed in "<<table<<" "<<"value is : "<<value1<<endl;
    
}


//-------------------------------------------------------------------------------------------------------------------------//

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

//-------------------------------------------------------------------------------------------------------------------//

string update_on_slave(int sock_fd,char *ip_address,char *port_number,string cs_ip,string cs_port,string key,string value,string table)
{   string ip=ip_address;
    string port=port_number;
    // we need connection two times one for succ and other for succ_of succ and also for put so making connect function
    
    
    // send key to get value from SS
    send_message(sock_fd,put_update_SS("update",key,value,table));
    string value1=receive_message(sock_fd);
    //cout<<"in put_on_slave -- value received from slave "<<value1<<endl;
    cout<<"value placed in "<<table<<" "<<"value is : "<<value1<<endl;
    return value1;
    
}


//----------------------------------------------------------------------------------------------------------------------------//

void serve_get_request(int connectfd,string key,string ip_port_cs)
{   Document document1;

   
    char* ip_port_cs_char=(char*)(ip_port_cs).c_str();

    // fetch ip and port of CS to connect succ and succ_of_succ
    char* ip_cs_char=strtok(ip_port_cs_char,":");
    char* port_cs_char=strtok(NULL,":");   
    string cs_ip=ip_cs_char;
    string cs_port=port_cs_char;

    if(root->left==NULL && root->right==NULL)
    {    
        char am[1024];
        strcpy(am,(char*)root->ip_plus_port.c_str());
        char* ip_root_char=strtok(am,":");
        char* port_root_char=strtok(NULL,":");   
        string root_ip=ip_root_char;
        string root_port=port_root_char;
        int sock_fd1=initialize_socket_without_bind();
        connect_f(sock_fd1,root_ip,root_port);
        string value_from_slave=get_from_slave(sock_fd1,ip_root_char,port_root_char,cs_ip,cs_port,key,"own");
        string value_from_slave1=value_from_slave;
        if(!(document1.ParseInsitu((char*)value_from_slave1.c_str()).HasParseError()))
        {
            if(strcmp(document1["req_type"].GetString(),"data")==0)
            {
             assert(document1.IsObject());
             assert(document1.HasMember("message"));
             string value_vv=document1["message"].GetString();
             cache.put_in_cache(key,value_vv);
            } 
        }
        send_message(connectfd,value_from_slave);
        return;
    }

    int hash_value=consistent_hash(key);
    cout<<"hashed value key hashed"<<hash_value<<endl;

    // get IP and port of successor from AVL
    avl_tree av;
    Node *pre=NULL,*succ=NULL; 
    av.Suc(root,pre,succ,hash_value);
    if(succ != NULL)
    {
        cout<<"fetched form AVL"<<succ->key<<" "<<succ->ip_plus_port<<endl;
    }
    // else
    //     cout<<"No slave server is active"<<endl;
    
     if(succ==NULL)
    {
        succ=av.minValue(root);
    }

    if(pre==NULL)
    {
        pre=av.maxValue(root);
    }

    //extract ip and port of succ slave server
   char* ip_port_avl=(char*)(succ->ip_plus_port).c_str();
    char ar[1024];
    strcpy(ar,ip_port_avl);
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");

    int sock_fd2=initialize_socket_without_bind();
    connect_f(sock_fd2,ip_address,port_number);
    // connecting to the respective slave:
    string value=get_from_slave(sock_fd2,ip_address,port_number,cs_ip,cs_port,key,"own");
    // if succ slave server is not up:

    if(sock_fd2 < 0)
    {
      
      
      avl_tree av_sec;
      Node *pree=NULL,*succ_of_succ=NULL;
      string ipport_new_id=ip_port_avl;

      //hash succ that we've retrieved to find succ of succ
      int hash_value_succ_slave=consistent_hash(ipport_new_id);
      av_sec.Suc(root,pree,succ_of_succ,hash_value_succ_slave);
        if(succ_of_succ==NULL)
        {
            succ_of_succ=av_sec.minValue(root);
        } 

        if(pree==NULL)
        {
            pree=av_sec.maxValue(root);
        }   
      char arr[1024];
      char* ip_port_avl1=(char*)(succ_of_succ->ip_plus_port).c_str();
      strcpy(arr,ip_port_avl1);
      char* ip_address1=strtok(arr,":");
      char* port_number1=strtok(NULL,":");

      int sock_fd3=initialize_socket_without_bind();
      connect_f(sock_fd3,ip_address1,port_number1);
      string value1=get_from_slave(sock_fd3,ip_address1,port_number1,cs_ip,cs_port,key,"prev");
       cout<<"after get from slave in get "<<value1<<endl;
      // will send to the client who has requested for the key -value
      //IN json format then client will parse it and retrieve from that
      send_message(connectfd,value1);

    } 
    // if succ slave server is up:
    else
    {
      // succ itself has the key value so return it to the client   
       cout<<"after get from slave in get "<<value<<endl;
        send_message(connectfd,value);

    } 



}


//-------------------------------------------------------------------------------------------------------------------------------------//

void serve_put_request(int connectfd,string key,string value,string ip_port_cs)
{
   
    cout<<"key inside serve_put_request "<<key<<endl;
    char* ip_port_cs_char=(char*)(ip_port_cs).c_str();

    // fetch ip and port of CS to connect succ and succ_of_succ
    char* ip_cs_char=strtok(ip_port_cs_char,":");
    char* port_cs_char=strtok(NULL,":");   
    string cs_ip=ip_cs_char;
    string cs_port=port_cs_char;

    int hash_value=consistent_hash(key);
    cout<<"hashed value key hashed"<<hash_value<<endl;

    if(root->left==NULL && root->right==NULL)
    {    
        char am[1024];
        strcpy(am,(char*)root->ip_plus_port.c_str());
        char* ip_root_char=strtok(am,":");
        char* port_root_char=strtok(NULL,":");   
        string root_ip=ip_root_char;
        string root_port=port_root_char;
        int sock_fd1=initialize_socket_without_bind();
        connect_f(sock_fd1,root_ip,root_port);
        put_on_slave(sock_fd1,ip_root_char,port_root_char,cs_ip,cs_port,key,value,"own");
        send_message(connectfd,ack_data_string("ack","put_success"));
        return;
    }

    // get IP and port of successor from AVL
    avl_tree av;
    Node *pre=NULL,*succ=NULL; 
    cout<<"going to find succ"<<endl;
    av.Suc(root,pre,succ,hash_value);
    cout<<"returned from AVL Suc"<<endl;
    if(succ != NULL)
    {
        cout<<"fetched form AVL"<<succ->key<<" "<<succ->ip_plus_port<<endl;
    }
    // else
    // {    
    //     cout<<"No slave server is active"<<endl;
    //     send_message(connectfd,ack_data_string("ack","no_slave_server_active"));
    //     return;
    // }
    
    if(succ==NULL)
    {
        succ=av.minValue(root);
       // cout<<suc
    }

    if(pre==NULL)
    {
        pre=av.maxValue(root);
    }

    //extract ip and port of succ slave server
   char* ip_port_avl=(char*)(succ->ip_plus_port).c_str();
    char ar[1024];
    strcpy(ar,ip_port_avl); 
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");
    cout<<"ip_plus_port_after TOK "<<succ->ip_plus_port<<endl;

    

    int hash_value1=consistent_hash(succ->ip_plus_port);
    cout<<"hashed value key hashed"<<hash_value1<<endl;

    // get IP and port of successor from AVL
    avl_tree av1;
    Node *pre1=NULL,*succ1=NULL; 
    av.Suc(root,pre1,succ1,hash_value1);
    
    if(succ1==NULL)
    {
        succ1=av1.minValue(root);
    } 

    if(pre1==NULL)
    {
        pre1=av1.maxValue(root);
    }   

    char arr[1024];
    char* ip_port_avl1=(char*)(succ1->ip_plus_port).c_str();
    strcpy(arr,ip_port_avl1);
    char* ip_address1=strtok(arr,":");
    char* port_number1=strtok(NULL,":");

    // try to connect succ slave server
    //int sock_fd=connect_to_the_slave(ip_address,port_number,cs_ip,cs_port);
    int sock_fd=initialize_socket_without_bind();
    cout<<"connecting to 1st "<<ip_address<<" "<<port_number<<endl;
   
    connect_f(sock_fd,ip_address,port_number);
    
    // try to connect succ_of_succ slave server
    //int sock_fd1=connect_to_the_slave(ip_address1,port_number1,cs_ip,cs_port);
    int sock_fd1=initialize_socket_without_bind();
    cout<<"connecting to 2nd "<<ip_address1<<" "<<port_number1<<endl;
    connect_f(sock_fd1,ip_address1,port_number1);
    
    
    //Ensuring 2 phase commit -- if it is able to connect to both succ and succ_of_succ then only put success
    if(sock_fd > 0 && sock_fd1 > 0)
    {
        // put on succ
       // cout<<"put on "<<ip_address<<" "<<port_number<<endl;
        put_on_slave(sock_fd,ip_address,port_number,cs_ip,cs_port,key,value,"own");
        //put on succ_of_succ
        put_on_slave(sock_fd1,ip_address1,port_number1,cs_ip,cs_port,key,value,"prev");
        send_message(connectfd,ack_data_string("ack","put_success"));
    }

    else
        send_message(connectfd,ack_data_string("ack","commit_failed"));
   
}


//----------------------------------------------------------------------------------------------------------------------------------//

void serve_delete_request(int connectfd,string key,string ip_port_cs)
{
    char* ip_port_cs_char=(char*)(ip_port_cs).c_str();
    cout<<"key in serve_delete_request: "<<key<<endl;
    // fetch ip and port of CS to connect succ and succ_of_succ
    char* ip_cs_char=strtok(ip_port_cs_char,":");
    char* port_cs_char=strtok(NULL,":");   
    string cs_ip=ip_cs_char;
    string cs_port=port_cs_char;
    string value,value1;

    // if only one SS is present delete from own table of root(only one SS available)
    if(root->left==NULL && root->right==NULL)
    {    
        char am[1024];
        strcpy(am,(char*)root->ip_plus_port.c_str());
        char* ip_root_char=strtok(am,":");
        char* port_root_char=strtok(NULL,":");   
        string root_ip=ip_root_char;
        string root_port=port_root_char;
        int sock_fd1=initialize_socket_without_bind();
        connect_f(sock_fd1,root_ip,root_port);
        value=delete_from_slave(sock_fd1,ip_root_char,port_root_char,cs_ip,cs_port,key,"own");
        cout<<"going to delete from cache, key->"<<key<<endl;
        cache.delete_from_cache(key);
        cout<<"deleted from cache"<<endl;
        send_message(connectfd,value);
        cout<<"msg sent to client"<<endl;
        return;
    }

    int hash_value=consistent_hash(key);
    cout<<"hashed value key hashed"<<hash_value<<endl;

    // get IP and port of successor from AVL
    avl_tree av;
    Node *pre=NULL,*succ=NULL; 
    av.Suc(root,pre,succ,hash_value);
    
     if(succ != NULL)
    {
        cout<<"fetched form AVL"<<succ->key<<" "<<succ->ip_plus_port<<endl;
    }
    // else
    // {   
    //     cout<<"No slave server is active"<<endl;
    //     send_message(connectfd,ack_data_string("ack","delete_failed"));
    //     return;
    // }

    if(succ==NULL)
    {
        succ=av.minValue(root);
    }

    if(pre==NULL)
    {
        pre=av.maxValue(root);
    }

   
    //extract ip and port of succ slave server
    char* ip_port_avl=(char*)(succ->ip_plus_port).c_str();
    char ar[1024];
    strcpy(ar,ip_port_avl);
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");

    //int sock_fd1=connect_to_the_slave(ip_address,port_number,cs_ip,cs_port);
    int sock_fd1=initialize_socket_without_bind();
    connect_f(sock_fd1,ip_address,port_number);
    // find succ_of_succ
     int hash_value1=consistent_hash(succ->ip_plus_port);
    cout<<"hashed value key hashed"<<hash_value1<<endl;

    // get IP and port of successor from AVL
    avl_tree av1;
    Node *pre1=NULL,*succ1=NULL; 
    av.Suc(root,pre1,succ1,hash_value1);

    if(succ1==NULL)
    {
        succ1=av1.minValue(root);
    } 

    if(pre1==NULL)
    {
        pre1=av1.maxValue(root);
    } 
    cout<<succ1->key<<" "<<succ1->ip_plus_port<<endl;
    char* ip_port_avl1=(char*)(succ1->ip_plus_port).c_str();
    char arr[1024];
    strcpy(arr,ip_port_avl1);
    char* ip_address1=strtok(arr,":");
    char* port_number1=strtok(NULL,":");
 
    int sock_fd2=initialize_socket_without_bind();
    connect_f(sock_fd2,ip_address1,port_number1);
    //int sock_fd2=connect_to_the_slave(ip_address1,port_number1,cs_ip,cs_port);

    

    if(sock_fd1 > 0 && sock_fd2 > 0)
    {   
        //delete from own table of succ 
        value=delete_from_slave(sock_fd1,ip_address,port_number,cs_ip,cs_port,key,"own");
        //delete from prev table of succ_of_succ
        value1=delete_from_slave(sock_fd2,ip_address1,port_number1,cs_ip,cs_port,key,"prev");
        cache.delete_from_cache(key);
        send_message(connectfd,value1);
    }
    else
        send_message(connectfd,ack_data_string("ack","commit_failed"));
      

}



//********************************************************************************************************************//


void serve_update_request(int connectfd,string key,string value,string ip_port_cs)
{  Document document1;

  char* ip_port_cs_char=(char*)(ip_port_cs).c_str();

    // fetch ip and port of CS to connect succ and succ_of_succ
    char* ip_cs_char=strtok(ip_port_cs_char,":");
    char* port_cs_char=strtok(NULL,":");   
    string cs_ip=ip_cs_char;
    string cs_port=port_cs_char;

    if(root->left==NULL && root->right==NULL)
    {    
        cout<<"in serve_update_request "<<root->ip_plus_port<<endl;
        char am[1024];
        strcpy(am,(char*)root->ip_plus_port.c_str());
        char* ip_root_char=strtok(am,":");
        char* port_root_char=strtok(NULL,":");   
        string root_ip=ip_root_char;
        string root_port=port_root_char;
        int sock_fd1=initialize_socket_without_bind();
        connect_f(sock_fd1,root_ip,root_port);
        cout<<"ip and port "<<root_ip<<" "<<root_port<<endl;
         string msg_from_slave=update_on_slave(sock_fd1,ip_root_char,port_root_char,cs_ip,cs_port,key,value,"own");
         string msg_from_slave1=msg_from_slave;
          if(!(document1.ParseInsitu((char*)msg_from_slave1.c_str()).HasParseError()))
        {
            if(strcmp(document1["req_type"].GetString(),"ack")==0 && strcmp(document1["message"].GetString(),"update_success")==0)
            {    
                assert(document1.IsObject());
                assert(document1.HasMember("message"));
                string value_vv=document1["message"].GetString();
                cache.update_in_cache(key,value);
            }

        
       }

        send_message(connectfd,ack_data_string("ack","update_success"));
       
        return;
    }

    int hash_value=consistent_hash(key);
    cout<<"hashed value key hashed"<<hash_value<<endl;

    // get IP and port of successor from AVL
    avl_tree av;
    Node *pre=NULL,*succ=NULL; 
    av.Suc(root,pre,succ,hash_value);
    
    if(succ==NULL)
    {
        succ=av.minValue(root);
    }

    if(pre==NULL)
    {
        pre=av.maxValue(root);
    }
 

    //extract ip and port of succ slave server
    char* ip_port_avl=(char*)(succ->ip_plus_port).c_str();
    char ar[1024];
    strcpy(ar,ip_port_avl); 
    char* ip_address=strtok(ar,":");
    char* port_number=strtok(NULL,":");
    cout<<"ip_plus_port_after TOK "<<succ->ip_plus_port<<endl;

    //int sock_fd1=connect_to_the_slave(ip_address,port_number,cs_ip,cs_port);
    int sock_fd1=initialize_socket_without_bind();
    connect_f(sock_fd1,ip_address,port_number);

    
    //send_message(connectfd,ack_data_string("ack","put_success"));
  
    // putting value in prev table in succ_of_succ
   
   
     
    if(succ != NULL)
    {
         cout<<"fetched form AVL"<<succ->key<<" "<<succ->ip_plus_port<<endl;
         

    }
    // else
        
    // {
    //     cout<<"No slave server is active"<<endl;
    //     send_message(connectfd,ack_data_string("ack","update_failed"));
    //     return;
    // }
    
    int hash_value1=consistent_hash(succ->ip_plus_port);
    cout<<"hashed value key hashed"<<hash_value1<<endl;


    // get IP and port of successor from AVL
    avl_tree av1;
    Node *pre1=NULL,*succ1=NULL; 
    av.Suc(root,pre1,succ1,hash_value1);
    if(succ1==NULL)
    {
        succ1=av1.minValue(root);
    } 

    if(pre1==NULL)
    {
        pre1=av1.maxValue(root);
    }
    cout<<"fetched form AVL"<<succ1->key<<" "<<succ1->ip_plus_port<<endl;
        //extract ip and port of succ slave server
    char* ip_port_avl1=(char*)(succ1->ip_plus_port).c_str();
    char arr[1024];
    strcpy(arr,ip_port_avl1);
    char* ip_address1=strtok(arr,":");
    char* port_number1=strtok(NULL,":");
        //put value in own table of successor

    //int sock_fd2=connect_to_the_slave(ip_address1,port_number1,cs_ip,cs_port);
    int sock_fd2=initialize_socket_without_bind();
    connect_f(sock_fd2,ip_address1,port_number1);

    if(sock_fd1>0 && sock_fd2>0)
    {
       // update in own table of succ
       string st1=update_on_slave(sock_fd1,ip_address,port_number,cs_ip,cs_port,key,value,"own");
       // update in prev table of succ_of_succ
       string st2=update_on_slave(sock_fd2,ip_address1,port_number1,cs_ip,cs_port,key,value,"prev");
       string st3=st2;
        if(!(document1.ParseInsitu((char*)st3.c_str()).HasParseError()))
        {
            if(strcmp(document1["req_type"].GetString(),"ack")==0 && strcmp(document1["message"].GetString(),"update_success")==0)
            {    
                assert(document1.IsObject());
                assert(document1.HasMember("message"));
                string value_vv=document1["message"].GetString();
                cache.update_in_cache(key,value);
            }

        
     }

       send_message(connectfd,st2);

       


    }
    else
    {
        send_message(connectfd,ack_data_string("ack","commit_failed"));
   
    }

}


//*****************************************************************************************************************************************//

void request_of_client(int connectfd,string ip_port_cs)
{   Document document1;
    string s;
    while(1)
    {
        
        

        s=receive_message(connectfd);        // received type: put,get,delete,update+key+value
        if(s.empty())
            return;
        cout<<"req received from client"<<s<<endl;

        // dont do anything if migration;
        
        while(data_migration==true);

        if(document1.ParseInsitu((char*)s.c_str()).HasParseError())
        {
    	   cout<<"error in request for client parsing string"<<endl;
    	   send_message(connectfd,ack_data_string("ack","parse_error"));
        }
        
        else if(strcmp(document1["req_type"].GetString(),"get")==0)
        {   

           
            assert(document1.IsObject());
        	assert(document1.HasMember("key"));
            string key_v=document1["key"].GetString();
           // send_message(connectfd,ack_data_string("ack","in parse"));
            //serve_get_request(connectfd,key_v,ip_port_cs);

           if(cache.mapvalues.find(key_v)!=cache.mapvalues.end())
            {
                cout<<"key "<<key_v<<" is present in cache"<<endl;
                string value1=cache.get_from_cache(key_v);
                send_message(connectfd,ack_data_string("data",value1)); 
            }
           // send_message(connectfd,ack_data_string("ack","in parse"));
             else
             {
                cout<<"key "<<key_v<<" not present in cache. Fetching from SS"<<endl;                
                serve_get_request(connectfd,key_v,ip_port_cs);
                cout<<"serve_get_request done"<<endl;
            }
             
             
        }

        else if(strcmp(document1["req_type"].GetString(),"put")==0)
        {  
            
          

            assert(document1.IsObject());
            assert(document1.HasMember("key"));
            assert(document1.HasMember("value"));
            string key_v=document1["key"].GetString();
            string value_v=document1["value"].GetString();
            serve_put_request(connectfd,key_v,value_v,ip_port_cs);
             
        }  

        else if(strcmp(document1["req_type"].GetString(),"delete")==0)
        {   
            

            
            assert(document1.IsObject());
            assert(document1.HasMember("key"));
            string key_v=document1["key"].GetString();
           // send_message(connectfd,ack_data_string("ack","in parse"));
            serve_delete_request(connectfd,key_v,ip_port_cs);
             
        } 

        
         
        else if(strcmp(document1["req_type"].GetString(),"update")==0)
        {  
            
          

            assert(document1.IsObject());
            assert(document1.HasMember("key"));
            assert(document1.HasMember("value"));
            string key_v=document1["key"].GetString();
            string value_v=document1["value"].GetString();
            serve_update_request(connectfd,key_v,value_v,ip_port_cs);
             
        }  

        cout<<"*************************************AVL*****************************************************************************"<<endl; 
        avl_tree av;
        av.preorder(root); 
        cout<<"******************************************************************************************************************"<<endl;  

        cout<<"\n************************************Cache*******************************************************************"<<endl;  
        cache.display();
        cout<<"*******************************************************************************************************"<<endl;
    }
     
      


}

//*************************************end_of_coordination_server***********************************************//

#endif