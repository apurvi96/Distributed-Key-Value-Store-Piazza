#include<bits/stdc++.h>
#include "COORD_FUNC.h"
#include "GLOBAL_CS.h"
#include "COMMON_FUNCN.h"
#include "jsonstring.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace std;
using namespace rapidjson;

Document document;



void* serve_requests(void* threadargs)
{
	cout<<"in serve requests"<<endl;
	int connectfd=((struct thread_struct*)threadargs)->sockfd;
	string ip_port=((struct thread_struct*)threadargs)->ip_plus_port;
	string ip_port_cs=((struct thread_struct*)threadargs)->ip_port_CS;
	cout<<ip_port<<endl;
	cout<<"connectfd"<<connectfd<<endl;
	cout<<ack_data_string("ack","connected")<<endl;
	send_message(connectfd,ack_data_string("ack","connected"));
	cout<<"sent"<<endl;
	string check_id=receive_message(connectfd);
	cout<<check_id<<endl;
	
	// If parsing gives error:
	if(document.ParseInsitu((char*)check_id.c_str()).HasParseError())
	{
		cout<<"Error while parsing"<<endl;
		send_message(connectfd,ack_data_string("ack","parse_error"));
	}
  
    else if(strcmp(document["id"].GetString(),"client")==0)
    {	
    	cout<<"in client"<<endl;
    	assert(document.IsObject());	
       	send_message(connectfd,ack_data_string("ack","ready_to_serve"));
       	request_of_client(connectfd,ip_port_cs);
    //    assert(document.IsObject());	
	   // send_message(connectfd,ack_data_string("ack","ready_to_serve"));
    //    request_of_client(connectfd);
    } 

    else if(strcmp(document["id"].GetString(),"slave_server")==0)
    {
       cout<<"in slave server"<<endl;
       assert(document.IsObject());
       register_slave_server(ip_port);
       send_message(connectfd,ack_data_string("ack","registration_successful"));
       cout<<"registered"<<endl;

       //TODO start HEARTBEAT
	}

}


int main(int argc,char **argv)
{
	string ip,port;
	if(argc<=2)
	{
		cout<<"enter ip port "<<endl;
		return 0;

	}
	ip=argv[1];
	port=argv[2];
	cout<<"my ip: "<<ip<<endl;
	cout<<"my port: "<<port<<endl;
	write_to_file(ip,port);

	int sock_fd=initialize_socket(ip,port);

	cout<<"socket id is"<<sock_fd<<endl;

	//listen
	if(listen(sock_fd,10)<0)
	{
		perror("LISTEN FAILED");
		
	}

	//create client server
	
	int connectfd;
	struct sockaddr_in ip_client;
	memset(&ip_client, '\0', sizeof(ip_client));
	int len;
	//=sizeof(sockaddr_in);
	pthread_t newThread[10];
	int i=0;
	cout<<"ready to accept"<<endl;
	// connectfd=accept(sock_fd,(struct sockaddr *)&ip_client,(socklen_t*)&len);
	// send_message(connectfd,ack_data_string("ack","slave_server_connected"));
	//accepting connections and creating new threads for clients
	// while(connectfd=accept(sock_fd,(struct sockaddr *)&ip_client,(socklen_t*)&len)>0)
	// {
	// 	cout<<receive_message(connectfd);
	// 	cout<<"in accept"<<endl;
	// 	// struct thread_struct* th=new thread_struct();
	// 	// th->sockfd=connectfd;
		// char* ipclient=new char[INET_ADDRSTRLEN];
		// inet_ntop(AF_INET,&(ip_client.sin_addr),ipclient,INET_ADDRSTRLEN);
		// th->ip_plus_port=string(ipclient)+":"+to_string(ntohs(ip_client.sin_port));
		// cout<<th->ip_plus_port<<endl;
	// 	//send_message(connectfd,ack_data_string("ack","slave_server_connected"));
	// 	//receive_message(connectfd);
		// pthread_create(&newThread[i],NULL,serve_requests,(void*)th);
		// pthread_join(newThread[i],NULL);
	// 	// i++;
	// 	cout<<"returned"<<endl;
	// 	close(connectfd);
		
	// }

	while(1){
		connectfd=accept(sock_fd,(struct sockaddr *)&ip_client,(socklen_t*)&len);
		if(connectfd<0){
			perror("error in accept()");
			exit(1);
		}
		cout<<"accepted conn"<<endl;
		struct thread_struct* th=new thread_struct();
		th->sockfd=connectfd;
		char* ipclient=new char[INET_ADDRSTRLEN];
		inet_ntop(AF_INET,&(ip_client.sin_addr),ipclient,INET_ADDRSTRLEN);
		th->ip_plus_port=string(ipclient)+":"+to_string(ntohs(ip_client.sin_port));
		cout<<th->ip_plus_port<<endl;
		th->ip_port_CS=ip+":"+port;
		pthread_create(&newThread[i],NULL,serve_requests,(void*)th);
		pthread_join(newThread[i],NULL);
		//send_message(connectfd,ack_data_string("ack","slave_server_connected"));
		//cout<<"recvd msg: "<<receive_message(connectfd);

	}


}