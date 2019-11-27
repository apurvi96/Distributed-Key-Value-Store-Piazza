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

	cout<<"soket id is"<<sock_fd<<endl;

	string s_ip="127.0.0.1";
	string s_port="6550";

	connect_f(sock_fd,s_ip,s_port);

	string recv_msg=receive_message(sock_fd);
	cout<<"final received msg is "<<recv_msg<<endl;

}