#ifndef lru_cache
#define lru_cache
#include<iostream>
#include<string>
#include<algorithm>
#include<unordered_map>
#include <iostream>
#include <map>

using namespace std;
typedef long long int ll;

class node
{
public:
	string key;
	string value;
	node *prev;
	node *next;

	node(string k, string v)
	{
		key=k;
		value=v;
		prev=NULL;
		next=NULL;
	}
};


class lrucache
{
public:
   	int capacity;//total size of the cache
   	int count=0;//no. of elements currently in the cache
	node *front,*rear;
	unordered_map<string,node*> mapvalues;

	lrucache()
	{
		capacity=0;
		front=NULL;
		rear=NULL;
	}


	lrucache(int cap)
	{
		capacity=cap;
		front=NULL;
		rear=NULL;
	}

	//print the contents of the cache
	void display()
	{
		node *n1=front;
		//n1=front;
		while(n1!=NULL)
		{
			cout<<"cache key: "<<n1->key<<", cache value: "<<n1->value<<endl;
			n1=n1->next;
		}
	}

	//delete a node from cache
	void deletenode(node *n1)
	{

		cout<<"count is "<<count<<endl;
		count--;
		mapvalues.erase(n1->key);
		cout<<"erased from map"<<endl;

		if(n1==NULL)
			return;

		if(front==rear)
		{
			front=NULL;
			rear=NULL;
			return;
		}

		if(rear==n1)
		{
			n1->prev->next=NULL;
			rear=n1->prev;
			free(n1);
			return;
		}

		if(n1 == front)
		{
			n1->next->prev=NULL;
			front=n1->next;

		}
		else
		{	
			n1->prev->next=n1->next;
			n1->next->prev=n1->prev;
		}
		
		free(n1);
		cout<<"exiting deletenode"<<endl;

	}

	//create a node and push it to the front of deque
	void pushfront(string k,string v)
	{
		count++;
		if(front==NULL)
		{
			node *n1=new node(k,v);
			mapvalues[k]=n1;
			front=n1;
			rear=n1;
			return;
		}
		node *n1=new node(k,v);
		mapvalues[k]=n1;
		n1->next=front;
		front->prev=n1;
		front=n1;
	}

	//move an existing node to the front of deque(most recently used node <- n1)
	void movetohead(node *n1)
	{
		if(n1==front)
			return;

		if(rear->prev==NULL)
		{
			rear->prev=n1;
			n1->next=front;
			n1->prev=NULL;
			front=n1;
		}

		if(n1==rear)
		{
			rear=n1->prev;
			n1->prev->next=NULL;
			n1->next=front;
			n1->prev=NULL;
			front->prev=n1;
			front=n1;
			return;
		}
		n1->next->prev=n1->prev;
		n1->prev->next=n1->next;
		n1->next=front;
		n1->prev=NULL;
		front->prev=n1;
		front=n1;
	}

	void put_in_cache(string k,string v){
		if(capacity==0)
		{
			//cout<<"NO CACHE SPACE\n";
			return;
		}

		if(mapvalues.find(k)==mapvalues.end()){//put
			{
				if(count<capacity)
					{
						pushfront(k,v);
					}

					else 
					{
						deletenode(rear);//delete lru node to make space for new node in cache
						pushfront(k,v);
					}
			}
		}
	}

	//call update after 2 phase commit successful
	void update_in_cache(string k,string v)
	{
		if(capacity==0)
		{
			//cout<<"NO CACHE SPACE\n";
			return;
		}
		if(mapvalues.find(k)!=mapvalues.end())//update
		{
			node *n2=mapvalues[k];

			if(n2->value.compare(v)!=0)
			{
				n2->value=v;
			}

			movetohead(n2);
			cout<<"updated in cache "<<endl;
		}
		
	}

	string get_from_cache(string k)
	{
		if(capacity==0)
		{
			//cout<<"NO CACHE SPACE\n";
			return "-1";
		}
		if(mapvalues.find(k)==mapvalues.end())
		{

		}

		else
		{
			node *n1=mapvalues[k];
			string s=n1->value;
			movetohead(n1);
			return s;
			//No need to contact SS. Directly return value(json format string) to Client
		}
	}

	void delete_from_cache(string key)
	{	
		cout<<"entered delete_from_cache:"<<endl;
  
  		if(mapvalues.find(key)!=mapvalues.end())
  		{
	      	cout<<"found key in map "<<key<<endl;
	      	node *n1=mapvalues[key];
	      	cout<<"addr is "<<n1<<endl;
	        deletenode(n1);
	        cout<<"deleted from cache in delete_from_cache func:"<<endl;

      	} 

	}

};

lrucache cache(4);

#endif
