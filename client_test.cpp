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

Document document1;

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

	////////////////////////////////////////////////////////////put
	connect_f(sock_fd,s_ip,s_port);

	send_message(sock_fd,put_update_SS("put","aa","12","own"));
	string recv_msg=receive_message(sock_fd);
	cout<<"json received "<<recv_msg<<endl;

	close(sock_fd);

	sleep(10);

	sock_fd=initialize_socket(ip_address,port_number);

	cout<<"soket id is"<<sock_fd<<endl;

	s_ip="127.0.0.1";
	s_port="6550";

	connect_f(sock_fd,s_ip,s_port);

	send_message(sock_fd,put_update_SS("put","bb","22","own"));
	recv_msg=receive_message(sock_fd);
	cout<<"json received "<<recv_msg<<endl;
	
	close(sock_fd);
	//////////////////////////////////////////////////////////////get

	sleep(10);

	sock_fd=initialize_socket(ip_address,port_number);

	cout<<"soket id is"<<sock_fd<<endl;

	s_ip="127.0.0.1";
	s_port="6550";

	connect_f(sock_fd,s_ip,s_port);

	send_message(sock_fd,get_delete_SS("get","bb","own"));
	recv_msg=receive_message(sock_fd);
	cout<<"json received "<<recv_msg<<endl;
	
	close(sock_fd);


	//////////////////////////////////////////////////////////////delete

	sleep(10);

	sock_fd=initialize_socket(ip_address,port_number);

	cout<<"soket id is"<<sock_fd<<endl;

	s_ip="127.0.0.1";
	s_port="6550";

	connect_f(sock_fd,s_ip,s_port);

	send_message(sock_fd,get_delete_SS("delete","a1a","own"));
	recv_msg=receive_message(sock_fd);
	cout<<"json received "<<recv_msg<<endl;
	
	close(sock_fd);

	////////////////////////////////////////////////////////////update
	sleep(10);

	sock_fd=initialize_socket(ip_address,port_number);

	cout<<"soket id is"<<sock_fd<<endl;

	s_ip="127.0.0.1";
	s_port="6550";

	connect_f(sock_fd,s_ip,s_port);

	send_message(sock_fd,put_update_SS("update","bb","new22","own"));
	recv_msg=receive_message(sock_fd);
	cout<<"json received "<<recv_msg<<endl;
	
	close(sock_fd);
	
}