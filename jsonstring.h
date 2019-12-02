#ifndef jsonstring
#define jsonstring
#include <bits/stdc++.h>
using namespace std;

//CS: Coordination Server
//SS: Slave Server
//leader slave server: the successor of the slave server that went down


/*Used to send either acknowledgemnt(parse_error, key_error, successful put, del, update), or 
for successful get, use it to send data from SS to CS, or data from CS to Client  
	req_type - ack, data
	message - client_connected , slave_server_connected, {actual value via get opn}, parse_error, key_error, put_success, 
	delete_success, update_success, registration_successful, migration_new_server, ready_for_migration, no_slave_server_active,{ip:port},
	migration_ss_done, new_ss_succ_of_succ_done
*/
string ack_data_string(string req_type, string message){
	string ackstr = "{ \"req_type\" : \"" + req_type + "\", \"message\" : \"" + message + "\" }";
	return ackstr;
}

/*informs CS if SS is connecting to it or Client
	id - slave_server , client
*/
string identity_string(string id){
	string idstr = "{ \"req_type\" : \"identity\", \"id\" : \"" + id + "\" }";
	return idstr;
}
	
/*client send get/delete requests to CS
req_type - get , delete
key - key
*/
string get_delete_CS(string req_type, string key){
	string gdreq = "{\"req_type\": \""   +   req_type    +    "\", \"key\" : \""+key +"\"}";
	return gdreq;
}


/*client sends put/update request to CS
	req_type=put,update
	Key
	Value
*/
string put_update_CS(string req_type, string key, string value){
	string pureq = " { \"req_type\" : \""   +req_type+   "\", \"key\" : \""   +key+   "\", \"value\" : \""   
					+value+   "\" } ";  
	return pureq;
}


/*handle migration - inform the leader slave server to proceed with migration
	role - leader,new_ss_leader, new_ss_pre,new_ss_succ,new_ss_succ_of_succ
	pre_ip , succ_ip , succ_of_succ_ip- ip:ports of these 3
*/
string inform_leader_migration(string role, string pre_ip, string succ_ip, string s_of_succ_ip){
	string inf = "{ \"role\" : \""   +   role   +   "\", \"pre_ip\" : \""   +pre_ip+   "\", \"succ_ip\" : \""   
				+succ_ip+   "\", \"succ_of_succ_ip\" : \""   +s_of_succ_ip+   "\"}";
	return inf; 
}


/*update table to other member slave server
	role - pre, succ_of_succ
	table - own,prev
*/
string update_table_SS(string role, string table){
	string updtss = "{ \"role\" : \"" + role + "\", \"table\" : \"" + table + "\" }";  
	return updtss;
}

/*send message from succ of succ to SS leader,
	Send message from leader slave server to CS that migration done
	message - “ready_for_table” , “migration_completed”
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
string get_delete_SS(string role, string key, string table){
	string gdreq = " { \"role\" : \""   +role+   "\", \"key\" : \""   +key+   "\",\"table\" : \""
					+table+   "\"} ";
	return gdreq;
}


/*CS sends put/update request to SS
	req_type=put, update
	Key
	Table = own/prev
*/
string put_update_SS(string role, string key, string value, string table){
	string pureq = " { \"role\" : \""   +role+   "\", \"key\" : \""   +key+   "\", \"value\" : \""   
					+value+   "\", \"table\" : \""   +table+   "\" } ";  
	return pureq;
}




#endif
