# Piazza-Distributed-Key-Value-Store

Steps to run the program:

1.Compile Coordination_Server.cpp (use -lpthreads to support pthreads), and run it as: ./coord {ip} {port}

2.Register Slave Servers at the CS by running slaveServer.cpp as: ./slave {ip} {port}

3.Multiple clients can connect to the Coordination Server by running client.cpp as: ./client {ip} {port}

A file "cs_config.txt" contains ip and port of the Coordination Server.

Operations Supported: GET, PUT, UPDATE, DELETE

System Architecture:
Clients send requests to perform various operations like get, put, update and delete key to Coordination Server.
Cordination Server then connects with the Slave Servers to serve the request.

Ring Structure using Consistent Hashing:
As soon as a new slave server registers itself, Coordination Server hashes the ip:port of the Slave Server.
Each Slave Server is hashed and placed in a ring structure.
Each Slave Server acts as a backup of the previous Slave Server in the ring.

Whenever a client sends a request, key is hashed and is placed in the own table of next slave server(successor of key in the
ring) and in the prev table of backup slave server(i.e the successor of successor of the key).

Heartbeat:
Coordination Server maintains which slave server is currently active in the system. A map is maintained for each ip:port
and maintains a count. Each slave server sends heartbeat(ack) to Coordination Server. A dedicated heartbeat thread keeps
track of this ack. Value for the corresponding ip:port is incremented in the map. A timer thread is maintained which wakes
up in every 30 sec and iterates over the map. Once count 0 is discovered, data migration is initiated.

Data Migration:
At any time, a key should be replicated in two slave servers. So whenever a new Server comes up or a Server goes down, data
must be migrated to ensure the required redundancy.
Case 1: When a new Server registers:-



Case 2: When a Servers goes down:-
1. the successor, predecessor and successor of successor is found out using the hashed id of the dead slave which is passed
to the successor(called "leader" for the duration of migration).
2. the successor is supposed to copy the content of it's 'previous' hash table to it's 'own' hash table, as the
'previous' hash table is supposed to store now the value of the 'own' table of the predecessor. All the values
lying between predecessor and the successor of the dead slave, will now move to successor of dead slave; so we copy the
previous of dead slave to own of successor.
3. The successor then sends the values of it's updated 'own' table to it's successor i.e successor of successor of the dead slave server.






