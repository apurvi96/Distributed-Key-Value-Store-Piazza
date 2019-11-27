#ifndef COMMON_FUNCN
#define COMMON_FUNCN
#include<bits/stdc++.h>
#include<iostream> //basic inp output
#include<stdio.h>
#include<string.h>
#include<sys/socket.h> //for socket creation
#include<arpa/inet.h> // in_addr structure
#include<sys/types.h> // defines various basic derived types
#include<errno.h>
#include<unistd.h> //read write
#include<cstring>
#include<string>
#include<pthread.h>
#include<openssl/sha.h>
#include<bits/stdc++.h>
using namespace std;
typedef long long int ll;

#define RING_SIZE 16
#define BUFF_SIZE 1024


ll consistent_hash(string s, int M)
 {
     int intLength = s.length() / 4;
     long sum = 0;
     for (int j = 0; j < intLength; j++) {
      string c = s.substr(j * 4, (j * 4) + 2);
      long mult = 1;
      for (int k = 0; k < c.length(); k++) {
	 sum += c[k] * mult;
	 mult *= 256;
      }
     }

    string  c = s.substr(intLength * 4);
     long mult = 1;
     for (int k = 0; k < c.length(); k++) {
      sum += c[k] * mult;
      mult *= 256;
     }
     
    cout<<(abs(sum)%M)<<endl; 
     return (abs(sum)%M);
   
  }

int initialize_socket(string ip,string port)
{

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

	//create structure

	struct sockaddr_in socket_struct;
	socket_struct.sin_family=AF_INET;
	socket_struct.sin_addr.s_addr =inet_addr(ip1);

	socket_struct.sin_port=htons(port1);


	//bind
	int bd=bind(sock_fd,(struct sockaddr *)&socket_struct,sizeof(socket_struct));

	if(bd<0)
	{
		perror("BIND FAILED");
	
	}


	return sock_fd;
}

void connect_f(int sock_fd, string ip,string port)
{

	//converting port  to int
	int port1=stoi(port);
	//cout<<"connect port is "<<port1<<endl;

	//converting ip to const char*
	const char *ip1=ip.c_str();
	//cout<<"connect ip is"<<ip1<<endl;


	//server structure
	struct sockaddr_in ip_server;
	ip_server.sin_family=AF_INET;
	ip_server.sin_addr.s_addr =inet_addr(ip1);
	//cout<<"cport is\n"<<cportn<<"\n";
	ip_server.sin_port=htons(port1);


	int con=connect(sock_fd,(struct sockaddr*)&ip_server,sizeof(ip_server));
	if(con<0)
	{
		perror("CANNOT CONNECT");
		
	}
}


void send_message(int sock_fd, string  msg)
{ 
  //cout<<"sock_fd "<<sock_fd<<endl; 
  //cout<<"string msg "<<msg<<endl; 
  char *char_pointer=(char*)msg.c_str();
  //cout<<"converted char pointer "<<char_pointer<<endl;
  char char_array[BUFF_SIZE];
  strcpy(char_array,char_pointer);
  cout<<"char pointer into array: "<<char_array<<endl;
  send(sock_fd,char_pointer,sizeof(char_array),0);

}


string receive_message(int ser_fd)
{   //cout<<"ser_fd "<<ser_fd<<endl;
	char Received_msg[BUFF_SIZE]={0};
    recv(ser_fd,Received_msg,BUFF_SIZE,0);
    //cout<<"rec msg "<<Received_msg<<endl;
    string Received_msg1=Received_msg;
    return Received_msg1;
}



#endif