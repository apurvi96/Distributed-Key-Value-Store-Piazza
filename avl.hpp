#include<bits/stdc++.h>
using namespace std;

struct Node
{
    int key;
    string ip_plus_port;
    int min;
    int max;
    struct Node *left, *right; 
    Node(){
        key=0;
        ip_plus_port="";
        min = INT_MIN;
        max= INT_MAX;
        left = NULL;
        right = NULL;
    }
}; 

Node *root=NULL;
class avl_tree{
public:

//structure 


  


//finding min value of the tree
Node* minValue(Node* node) { 
    Node* current = node; 
    while (current->left != NULL) { 
        current = current->left; 
    } 
    return current; 
} 


//will give succ as well as pre
void Suc(Node* root1, Node*& pre, Node*& suc, int key) 
{ 
     
    if (root1 == NULL)  return ; 

    if (root1->key == key) 
    { 
        if (root1->left != NULL) 
        { 
            Node* tmp = root1->left; 
            while (tmp->right) 
                tmp = tmp->right; 
            pre = tmp ; 
        }  
        if (root1->right != NULL) 
        { 
            Node* tmp = root1->right ; 
            while (tmp->left) 
                tmp = tmp->left ; 
            suc = tmp ; 
        } 

        // if(root->left==NULL && root->right==NULL)
        // {
        //     suc=root;
        //     pre=root;
        // }
        return  ; 
    } 
   
    if (root1->key > key) 
    { 
        suc = root1 ; 
        Suc(root1->left, pre, suc, key) ; 
        
    } 
    else 
    { 
        pre = root1 ; 
        Suc(root1->right, pre, suc, key) ;
         
    } 
    if(pre!=NULL)
        cout<<"pre is "<<pre->key<<" "<<endl;
    if(suc!=NULL)
        cout<<"suc is "<<suc->key<<" "<<endl;
    cout<<endl;
} 





Node* maxValue(Node* node) { 
    Node* current = node; 
    while (current->right != NULL) { 
        current = current->right; 
    } 
    return current; 
} 
 
Node *newNode(int id,string ip_plus_port ) 
{   
    Node *temp =  new Node; 

    temp->key = id;
    temp->ip_plus_port = ip_plus_port;
    temp->left = temp->right = NULL;
    temp->min = id;
    temp->max = id;
    return temp;
}

Node *insert(Node *node, int key, string ip_plus_port)
{
    cout<<key<<" "<<ip_plus_port<<endl;
    if (node == NULL) {
        node = newNode(key,ip_plus_port);
        cout<<"newly created "<<node->key<<" "<<node->ip_plus_port<<endl;
        return node; 
    }
    if (key < node->key){ 
        if(node->min > key)
            node->min = key;
        node->left  = insert(node->left, key,ip_plus_port); 
    }
    else{
        if(node -> max < key)
            node->max = key;
        node->right = insert(node->right, key,ip_plus_port); 
    }
    return node; 
} 

Node *minValueNode(Node *node)
{
    Node *current = node;
    while (current->left != NULL)
        current = current->left;

    return current;
}
Node *deleteNode(Node *root1, int key)
{
    if (root1 == NULL)
        return root1;
    if (key < root1->key)
        root1->left = deleteNode(root1->left, key);
    else if (key > root1->key)
        root1->right = deleteNode(root1->right, key);
    else
    {
        if (root1->left == NULL)
        {
            Node *temp = root1->right;
            free(root1);
            return temp;
        }
        else if (root1->right == NULL)
        {
            Node *temp = root1->left;
            free(root1);
            return temp;
        }
        Node *temp = minValueNode(root1->right);
        root1->key = temp->key;
        root1->ip_plus_port=temp->ip_plus_port;
        root1->right = deleteNode(root1->right, temp->key);
    }
    if(root1!=NULL)
    cout<<"root1 data in avl "<<root1->key<<endl;
    return root1;
}



void  preorder(Node *root1)
{
    if(root1==NULL){
        cout<<"root->null"<<endl;
        return;
    }
    else{
         cout << root1->key << "->" << root1->ip_plus_port << "|";
        preorder(root1->left);
        
        preorder(root1->right);
   
 }
}

};