#include<bits/stdc++.h>
#include "COMMON_FUNCN.h"
using namespace std;

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


	//listen
	if(listen(sock_fd,10)<0)
	{
		perror("LISTEN FAILED");
		
	}

	//create client server
	int ip_client_length;
	struct sockaddr_in ip_client;



	int client_fd=accept(sock_fd,(struct sockaddr *)&ip_client,(socklen_t*)&ip_client_length);

	send_message(client_fd,"hi from server");



}