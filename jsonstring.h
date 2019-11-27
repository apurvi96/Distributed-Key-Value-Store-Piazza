#ifndef jsonstring
#define jsonstring
#include <bits/stdc++.h>
using namespace std;

//CS: Coordination Server
//SS: Slave Server
//leader slave server: the successor of the slave server that went down

/*req_type - ack
	message - client_connected , slave_server_connected
*/
string acknowledge_string(string req_type, string message){
	string ackstr = "{ \"request_type\" : \"" + req_type + "\", \"message\" : \"" + message + "\" }";
	return ackstr;
}
	
/*client send get/delete requests to CS
req_type - get , delete
key - key
*/
string get_delete_CS(string req_type, int key){
	string gdreq = "{\"request_type\": \""   +   req_type    +    "\", \"key\" : \""+to_string(key) +"\"}";
	return gdreq;
}


/*client sends put/update request to CS
	req_type=put,update
	Key
	Value
*/
string put_update_CS(string req_type, int key, string value){
	string pureq = " { \"req_type\" : \""   +req_type+   "\", \"key\" : \""   +to_string(key)+   "\", \"value\" : \""   
					+value+   "\" } ";  
	return pureq;
}


/*handle migration - inform the leader slave server to proceed with migration
	role - leader
	pre_ip , succ_ip , s_of_succ_ip
*/
string inform_leader_migration(string role, string pre_ip, string succ_ip, string s_of_succ_ip){
	string inf = "{ \"role\" : \"”   +   role   +   ”\", \"pre_ip\" : \""   +pre_ip+   "\", \"succ_ip\" : \""   
				+succ_ip+   "\", \"s_of_succ_ip\" : \""   +s_of_succ_ip+   "\"}";
	return inf; 
}


/*update table to other member slave server
	role - member
	table - own,prev
*/
string update_table_SS(string role, string table){
	string updtss = "{ \"role\" : \"" + role + "\", \"table\" : \"" + table + "\" }";  
	return updtss;
}

/*send message from succ of succ to SS leader
	Send message from leader slave server to CS that migration done
	message - “ready for table” , “migration completed”
*/
string send_message_ready(string message){
	string msgrdy = "{ \"message\" : \""   +message+   "\"}";
	return msgrdy;
}

/*CS sends get/delete request to SS
	req_type=get, delete
	Key
	Table = own/prev
*/
string get_delete_SS(string req_type, int key, string table){
	string gdreq = " { \"req_type\" : \""   +req_type+   "\", \"key\" : \""   +to_string(key)+   "\",\"table\" : \""
					+table+   "\"} ";
	return gdreq;
}


/*CS sends put/update request to SS
	req_type=put, update
	Key
	Table = own/prev
*/
string put_update_SS(string role, int key, string value, string table){
	string pureq = " { \"role\" : \""   +role+   "\", \"key\" : \""   +to_string(key)+   "\", \"value\" : \""   
					+value+   "\", \"table\" : \""   +table+   "\" } ";  
	return pureq;
}

#endif



