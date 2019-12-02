#include<bits/stdc++.h>
#include "COMMON_FUNCN.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "jsonstring.h"

using namespace rapidjson;
using namespace std;



#define FILENAME "cs_config.txt"

//**************************************do get and delete together*****************************************//

void request_for_get_delete(string type,string key,int sock_fd)
{
   Document document1;
   send_message(sock_fd,get_delete_CS(type,key));
   string value_json=receive_message(sock_fd);
   cout<<"recvd msg: "<<value_json<<endl;
   if(document1.ParseInsitu((char*)value_json.c_str()).HasParseError())
   {
       cout<<"error in request for client parsing string"<<endl;
   }
        
   else if(strcmp(document1["req_type"].GetString(),"data")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value=document1["message"].GetString();
            cout<<"value for the "<<key<<" is "<<value<<endl;


    }

    else if(strcmp(document1["req_type"].GetString(),"ack")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value=document1["message"].GetString();
            cout<<value<<endl;


    }
          


}




//*************************************************do put and update together**************************************//


void request_for_update_put(string type,string key,string value, int sock_fd)
{
  Document document1;
   send_message(sock_fd,put_update_CS(type,key,value));
   string value_json=receive_message(sock_fd);// 1) ack+no_slave_server_active
                                              // 2) ack+put_success  
                                              // 3) ack+commit_failed
   cout<<"recvd msg:: "<<value_json<<endl;
   if(document1.ParseInsitu((char*)value_json.c_str()).HasParseError())

   {
       cout<<"error in request for client parsing string"<<endl;
   }
        
   else if(strcmp(document1["req_type"].GetString(),"data")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value1=document1["message"].GetString();
            cout<<value1<<endl;



    }

    else if(strcmp(document1["req_type"].GetString(),"ack")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value1=document1["message"].GetString();
            cout<<value1<<endl;


    }
          

}

//*********************************************************************************************//


int main(int argc,char **argv)
{

	if(argc<=2)
	{
		cout<<"enter ip port "<<endl;
		return 0;

	}
	string ip_address=argv[1];
	string port_number=argv[2];
	
	int sock_fd=initialize_socket(ip_address,port_number);

	cout<<"socket id is"<<sock_fd<<endl;
     
    ifstream file(FILENAME);
    string cs_ip,cs_port;

    if (file.is_open()) 
    {	    
	    getline(file, cs_ip);
	    getline(file, cs_port); 
	    file.close();
	}
    cout<<" cs ip "<<cs_ip<<" port"<<cs_port<<endl;
    connect_f(sock_fd,cs_ip,cs_port);
    cout<<"after connect_f "<<endl;
	cout<<receive_message(sock_fd)<<endl;
    cout<<"connected in client"<<endl;
    send_message(sock_fd,identity_string("client"));
    cout<<receive_message(sock_fd)<<endl;
    cout<<"in client ready to serve"<<endl;

    cout<<"command format : command_type:key:value"<<endl;
    while(true)
    {

     string command;
     cout<<"command >> ";
     cin>>command;

     if(command=="exit")
     {
     	cout<<"TATA!!!! "<<endl;
     	return 0;
     }
     
     char *command_char=(char*)command.c_str();
     string type=strtok(command_char,":");


    
     if(type=="get"||type=="delete")
     {
        char *get_char=strtok(NULL,":");
     	string key=get_char;
     	// if(key==NULL)
     	// {
     	// 	cout<<"wrong command enter again\n";
     	// 	continue;
     	// }
     	request_for_get_delete(type,key,sock_fd);
     } 


     if(type=="put"||type=="update")
     {
        char *get_char=strtok(NULL,":");
     	string key=get_char;
     	// if(key==NULL)
     	// {
     	// 	cout<<"wrong command enter again\n";
     	// 	continue;
     	// }
     	char *get_val=strtok(NULL,":");
     	string value=get_val;
     	// if(value==NULL)
     	// {
     	// 	cout<<"wrong command enter again\n";
     	// 	continue;
     	// }
       request_for_update_put(type,key,value,sock_fd);
     }
    cout<<endl;

   }

}

//**********************************************************************************************************//