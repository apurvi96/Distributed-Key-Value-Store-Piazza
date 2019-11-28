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
void Suc(Node* root, Node*& pre, Node*& suc, int key) 
{ 
   
    if (root == NULL)  return ; 
    if (root->key == key) 
    { 
        if (root->left != NULL) 
        { 
            Node* tmp = root->left; 
            while (tmp->right) 
                tmp = tmp->right; 
            pre = tmp ; 
        }  
        if (root->right != NULL) 
        { 
            Node* tmp = root->right ; 
            while (tmp->left) 
                tmp = tmp->left ; 
            suc = tmp ; 
        } 

        if(root->left==NULL && root->right==NULL)
        {
            suc=root;
        }
        return  ; 
    } 
   
    if (root->key > key) 
    { 
        suc = root ; 
        Suc(root->left, pre, suc, key) ; 
    } 
    else 
    { 
        pre = root ; 
        Suc(root->right, pre, suc, key) ; 
    } 
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
Node *deleteNode(Node *root, int key)
{
    if (root == NULL)
        return root;
    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else
    {
        if (root->left == NULL)
        {
            Node *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            Node *temp = root->left;
            free(root);
            return temp;
        }
        Node *temp = minValueNode(root->right);
        root->key = temp->key;
        root->right = deleteNode(root->right, temp->key);
    }
    return root;
}



void inorder(Node *root)
{
    if(root==NULL){
        cout<<"root null"<<endl;
        return;
    }
    else{

        inorder(root->left);
        cout << root->key << "/" << root->ip_plus_port << ", ";
        inorder(root->right);
   
 }
}

};