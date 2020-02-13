<h2>Distributed Key Value Store (Piazza)</h2>
<ul>
<li>Multiple clients will be communicating with a single coordinating server (Master) in a JSON based message format and send the data through sockets using TCP channel.</li>
  <li><b>Fault Tolerence</b>: Data storage is durable, i.e., the system does not lose data if a single node fails.Data replication factor of 2 using consistent hashing.</li>
  <li><b>Atomic Operations(Using 2 Phase Commit Protocol)</b>:Operations on the store are atomic. i.e., either the operation will succeed completely or fail altogether without any side effects. </li>
  </ul>
  
 <h2>System Architecture</h2>
  The System consists of 3 entities.<ol type="A"><li><b><i>Piazza Clients</i></b><br>
 They register or login to the system and access the data using following opeartions:
 <ol type="i"><li>GET &ltkey&GT</li><li>PUT &ltkey&gt &ltvalue&gt</li><li>DELETE &ltkey&gt</li><li>UPDATE &ltkey&gt &ltnew_value&gt</li>
  </ol></li><br>
  <li><b><i>Coordination Server</i></b><br><ol type="i"><li>Acts as an intermediate between the clients and slave server.</li><li>Whenever a new slave server registers itself, Coordination Server hashes the ip:port of the Slave Server. Each Slave Server is hashed and placed in a ring structure.</li><li>Whenever a client sends a request, key is hashed and and placed in the own table of next nearest hashed slave server and also in the prev table of the predecessor of this slave server.</li>
<li><b>WRITE THROUGH CACHE:</b>The coordinator contains a write-through LRU cache, and it uses the cache to serve requests without going to the (slave) key-value servers it coordinates. The slave key-value servers are contacted for a request only upon a cache miss on the coordinator.</li></ol>
  </li><br>
  <li><b><i>Slave Servers</b></i><br>Stores the actual data,i.e, key-value pair. Each slave server consistes of two tables:<ol type="i"><li>OWN TABLE : stores the first copies of keys with hash values greater than the ID of its immediate predecessor up to its own ID.</li><li>PREV TABLE : stores the keys whose first copies are stored in its predecessor.</li></ol></li></ol>
  
  <h2>Data Migration</h2>
 At any time, a key should be replicated in two slave servers. So whenever a new Server comes up or a Server goes down, data
must be migrated to ensure the required redundancy.<br>
<b><i>Case 1: When a new Server registers:-</i></b><ol>
   <li>Predecessor, successor and successor of successor are found using hashed id of new registered slave server.</li>
  <li>Successor updates its 'own' table by removing keys hashed between predecessor and new registered server.</li>
<li>These removed keys are filled in 'previous' table of successor server.</li>
  <li>Removed keys are filled in 'own' table of newly registered slave server.</li>
<li>Newly registered slave server updates its 'previous' table with 'own' table of predecessor.</li>
  <li>Successor of successor updates its 'previous' table with 'own' table of successor.</li>




</ol>
<b><i>Case 2: When a Servers goes down:-</i></b>
<ol type="i">
<li>The successor, predecessor and successor of successor is found out using the hashed id of the dead slave.</li>
<li>The successor(also known as leader) of dead slave is supposed to copy the content of it's 'previous' hash table to it's 'own' hash table All the values lying between predecessor and the successor of the dead slave, will now move to successor of dead slave; so we copy the previous of dead slave to own of successor.</li>
<li>The successor then sends the values of it's updated 'own' table to it's successor i.e successor of successor of the dead slave server.</li></ol>
<h2>Heartbeat Implementation</h2>
Heartbeat technique is used to find out if a slave server has gone down.Slave servers periodically send a message to the cordination server (CS) to inform it that it is alive. When the CS senses that message has not arrived from the slave server side, it gets to know that the particular slave server is down.<br>A map is maintained at CS for each slave server(ip:port)
and count of map is increamented on receiving of each alive msg.<br> A dedicated heartbeat thread is present on the slave server side which makes a UDP connection with the CS. This thread sends heartbeat message to CS for every 5 seconds. Also, there is a heart beat listener thread on the CS side to receive heartbeat message from different slave servers.<br>A timer thread is maintained at Coordination Server side which wakes up in every 30 sec and iterates over the map. Once count 0 is discovered, it knows that this slave server is down and thus data migration is initiated.
<h2>Compile and Run Program</h2>
<ol>
<li>Compile Coordination_Server.cpp (use -lpthreads to support pthreads) and run it as:<br> ./coord &ltip&gt&ltport&gt</li>
<li>Register Slave Servers at the CS by running slaveServer.cpp as: <br>./slave &ltip&gt&ltport&gt</li>
<li>Multiple clients can connect to the Coordination Server by running client.cpp as: <br>./client &ltip&gt&ltport&gt</li></ol>
A file "cs_config.txt" contains ip and port of the Coordination Server.
  

