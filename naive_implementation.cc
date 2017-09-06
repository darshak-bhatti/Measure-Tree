#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cstdio>
#include <set>
#include <utility>
#include <limits.h>
#include <math.h>
#include "API.h"

#define LEFT 0
#define RIGHT 1

using namespace std;

typedef pair<int, int> interval_t;


struct intervals {
    int left;
    int right;
    intervals *next;
    intervals *prev;
};

struct m_tree_t {
    int key;
    int which; // 0:left; 1:right
    int other; //
    int measure;
    int min_u; // min of node-interval
    int max_u; //max of node-interval
    int leftmin;
    int rightmax;
    int height;
    
    m_tree_t * left;
    m_tree_t * right;
    m_tree_t * parent;
    intervals *assoc_interval_list;
};

vector<m_tree_t *> rb_st;
vector<m_tree_t *> ia_st; //interval association stack


m_tree_t * create_m_tree();
void insert_interval(m_tree_t * tree, int a, int b);
void delete_interval(m_tree_t * tree, int a, int b);
int query_length(m_tree_t * tree);
void destroy_m_tree(m_tree_t *tree);

void associate_interval(m_tree_t *tree, int a, int b);
intervals * insert_to_associated_ll(intervals *head, int a, int b);
void update_measures(m_tree_t *tmp);
void rebalance();


int query_length(m_tree_t * tree){
    return tree->measure;
}

m_tree_t * create_m_tree(){
    
    m_tree_t *tmp_node;
    tmp_node = (m_tree_t *)malloc(sizeof(m_tree_t));
    tmp_node->left = NULL;
    tmp_node->right = NULL;
    tmp_node->parent = NULL;
    tmp_node->assoc_interval_list = NULL;
    
    tmp_node->key = -1;
    tmp_node->which = -1;
    tmp_node->other = -1;
    tmp_node->measure = 0;
    tmp_node->min_u = -1;
    tmp_node->max_u = -1;
    tmp_node->leftmin = -1;
    tmp_node->rightmax = -1;
    
    return tmp_node;
}


void insert_interval(m_tree_t * tree, int a, int b){
    
    if( a > b){
        return;
    }
    
    
    if (tree->left == NULL){
        //Empty Tree
        
        m_tree_t *tmp_node_left, *tmp_node_right;
        tmp_node_left = (m_tree_t *)malloc(sizeof(m_tree_t));
        
        
        tree->key = a;
        tree->min_u = INT_MIN;
        tree->max_u = INT_MAX;
        tree->height = 2;
        tree->measure = 0;
        tree->leftmin = INT_MAX;
        tree->rightmax = INT_MIN;
        
        m_tree_t *left_child, *right_child;
        left_child = (m_tree_t *)malloc(sizeof(m_tree_t));
        right_child = (m_tree_t *)malloc(sizeof(m_tree_t));
        
        
        intervals * tmp_interval_left;
        tmp_interval_left = (intervals * )malloc(sizeof(intervals));
        tmp_interval_left->left = INT_MIN;
        tmp_interval_left->right = a;
        tmp_interval_left->next = NULL;
        tmp_interval_left->prev = NULL;
        
        left_child->key = 0;
        left_child->right = NULL;
        left_child->height = 0;
        left_child->measure = 0;
        left_child->left = (m_tree_t *)tmp_interval_left;
        
        left_child->min_u = tmp_interval_left->left;
        left_child->max_u = tmp_interval_left->right;
        left_child->leftmin = INT_MAX;
        left_child->rightmax = INT_MIN;
        left_child->assoc_interval_list = NULL;
        
        
        intervals * tmp_interval_right;
        tmp_interval_right = (intervals * )malloc(sizeof(intervals));
        tmp_interval_right->left = a;
        tmp_interval_right->right = INT_MAX;
        tmp_interval_right->next = NULL;
        tmp_interval_right->prev = NULL;
        
        
        right_child->key = 0;
        right_child->right = NULL;
        right_child->height = 0;
        right_child->measure = 0;
        right_child->left = (m_tree_t *)tmp_interval_right;
        right_child->min_u = tmp_interval_right->left;
        right_child->max_u = tmp_interval_right->right;
        right_child->leftmin = INT_MAX;
        right_child->rightmax = INT_MIN;
        right_child->assoc_interval_list = NULL;
        
        tree->left = left_child;
        tree->right = right_child;
        tree->min_u = tree->left->min_u;
        tree->max_u = tree->right->max_u;
        tree->leftmin = INT_MAX;
        tree->rightmax = INT_MIN;
        
      
        //Doing for B -------------------------------------------------
        m_tree_t *left_child_b, *right_child_b;
        
        tmp_node_right = (m_tree_t *)malloc(sizeof(m_tree_t));
        tmp_node_right->key = b;
        tmp_node_right->min_u = a;
        tmp_node_right->max_u = INT_MAX;
        tmp_node_right->height = 1;
        tmp_node_right->measure = 0;
        tmp_node_right->leftmin = INT_MAX;
        tmp_node_right->rightmax = INT_MIN;
        
        
        left_child_b = (m_tree_t *)malloc(sizeof(m_tree_t));
        intervals* tmp_interval_left_b = (intervals * )malloc(sizeof(intervals));
        tmp_interval_left_b->left = a;
        tmp_interval_left_b->right = b;
        tmp_interval_left_b->next = NULL;
        tmp_interval_left_b->prev = NULL;
        left_child_b->key = 0;
        left_child_b->right = NULL;
        left_child_b->height = 0;
        left_child_b->measure = 0;
        left_child_b->left = (m_tree_t *)tmp_interval_left_b;
        left_child_b->min_u = tmp_interval_left_b->left;
        left_child_b->max_u = tmp_interval_left_b->right;
        left_child_b->leftmin = INT_MAX;
        left_child_b->rightmax = INT_MIN;
        left_child_b->assoc_interval_list = NULL;
        
    
        
        right_child_b = (m_tree_t *)malloc(sizeof(m_tree_t));
        intervals* tmp_interval_right_b = (intervals * )malloc(sizeof(intervals));
        tmp_interval_right_b->left = b;
        tmp_interval_right_b->right = INT_MAX;
        tmp_interval_right_b->next = NULL;
        tmp_interval_right_b->prev = NULL;
        right_child_b->key = 0;
        right_child_b->right = NULL;
        right_child_b->height = 0;
        right_child_b->measure = 0;
        right_child_b->left = (m_tree_t *)tmp_interval_right_b;
        right_child_b->min_u = tmp_interval_right_b->left;
        right_child_b->max_u = tmp_interval_right_b->right;
        right_child_b->leftmin = INT_MAX;
        right_child_b->rightmax = INT_MIN;
        right_child_b->assoc_interval_list = NULL;
        
        
        
        tmp_node_right->left = left_child_b;
        tmp_node_right->right = right_child_b;
        
        
        tmp_node_right->min_u = tmp_node_right->left->min_u;
        tmp_node_right->max_u = tmp_node_right->right->max_u;
        
        
        tree->right = tmp_node_right;
        
        
        
        tree->min_u = tree->left->min_u;
        tree->max_u = tree->right->max_u;
        
        
        associate_interval(tree, a, b);
        
        return;
    }
    
    
    
    
    
    m_tree_t *tmp;
    tmp = tree;
    int flag=1; //0, key exists ; 1 key not there
    

    //Adding A
    while(tmp->right != NULL && flag==1){
        rb_st.push_back(tmp);
        if(a < tmp->key){
            tmp = tmp->left;
        } else if (a > tmp->key){
            tmp = tmp->right;
        } else {
            flag = 0;
        }
    }
    
    if (flag){
        intervals *abc;
        abc = (intervals *)tmp->left;
        
        m_tree_t *left_child, *right_child;
        left_child = (m_tree_t *)malloc(sizeof(m_tree_t));
        right_child = (m_tree_t *)malloc(sizeof(m_tree_t));
        left_child->key = 0;
        left_child->right = NULL;
        left_child->height = 0;
        left_child->measure = 0;
        
        intervals * tmp_interval_left, *tmp_interval_right;
        tmp_interval_left = (intervals * )malloc(sizeof(intervals));
        tmp_interval_left->left = abc->left;
        tmp_interval_left->right = a;
        tmp_interval_left->next = NULL;
        tmp_interval_left->prev = NULL;
        
        left_child->left = (m_tree_t *) tmp_interval_left;
        left_child->min_u = tmp_interval_left->left;
        left_child->max_u = tmp_interval_left->right;
        left_child->leftmin = tmp->leftmin;
        left_child->rightmax = tmp->rightmax;
        left_child->assoc_interval_list = tmp->assoc_interval_list;
        left_child->measure = 0;
        tmp->assoc_interval_list = NULL;
        
        
        
        right_child->key = 0;
        right_child->right = NULL;
        right_child->height = 0;
        right_child->measure = 0;
        right_child->leftmin = INT_MAX;
        right_child->rightmax = INT_MIN;
        right_child->assoc_interval_list = NULL;
        
        tmp_interval_right = (intervals * )malloc(sizeof(intervals));
        tmp_interval_right->left = a;
        tmp_interval_right->right = abc->right;
        tmp_interval_right->next = NULL;
        tmp_interval_right->prev = NULL;
        right_child->left = (m_tree_t *) tmp_interval_right;
        right_child->min_u = tmp_interval_right->left;
        right_child->max_u = tmp_interval_right->right;
        
        
        tmp->key = a;
        tmp->left = left_child;
        tmp->right = right_child;
        
        if(tmp->left->assoc_interval_list != NULL){
            if(tmp->left->rightmax == tmp->left->min_u){
                tmp->left->measure = 0;
            } else {
                tmp->left->measure = tmp->key - tmp->left->min_u;
            }
        }
        
        
        
        
        
        tmp->height += 1;
        tmp->leftmin = INT_MAX;
        tmp->rightmax = INT_MIN;
        
        tmp->min_u = tmp->left->min_u;
        tmp->max_u = tmp->right->max_u;
        
        
        rebalance();
        
    }
    
    
    flag =1;
    
    int x = query_length(tree);
    
    //For B
    tmp = tree;
    
    while(tmp->right != NULL && flag == 1){
        rb_st.push_back(tmp);
        if(b < tmp->key){
            tmp = tmp->left;
        } else if (b > tmp->key) {
            tmp = tmp->right;
        } else {
            flag = 0;
        }
    }
    
    if (flag){
        
        intervals *abc;
        abc = (intervals *)tmp->left;
        
        m_tree_t *left_child_b, *right_child_b;
        left_child_b = (m_tree_t *)malloc(sizeof(m_tree_t));
        right_child_b = (m_tree_t *)malloc(sizeof(m_tree_t));
        left_child_b->key = 0;
        left_child_b->right = NULL;
        left_child_b->height = 0;
        left_child_b->measure = 0;
        
        intervals * tmp_interval_left_b, *tmp_interval_right_b;
        tmp_interval_left_b = (intervals * )malloc(sizeof(intervals));
        tmp_interval_left_b->left = abc->left;
        tmp_interval_left_b->right = b;
        tmp_interval_left_b->next = NULL;
        tmp_interval_left_b->prev = NULL;
        
        left_child_b->left = (m_tree_t *) tmp_interval_left_b;
        left_child_b->min_u = tmp_interval_left_b->left;
        left_child_b->max_u = tmp_interval_left_b->right;
        left_child_b->leftmin = tmp->leftmin;
        left_child_b->rightmax = tmp->rightmax;
        left_child_b->assoc_interval_list = tmp->assoc_interval_list;
        tmp->assoc_interval_list = NULL;
        left_child_b->measure = 0;
        
        
        
        right_child_b->key = 0;
        right_child_b->right = NULL;
        right_child_b->height = 0;
        right_child_b->measure = 0;
        right_child_b->assoc_interval_list = NULL;
        
        tmp_interval_right_b = (intervals * )malloc(sizeof(intervals));
        tmp_interval_right_b->left = b;
        tmp_interval_right_b->right = abc->right;
        tmp_interval_right_b->next = NULL;
        tmp_interval_right_b->prev = NULL;
        right_child_b->left = (m_tree_t *) tmp_interval_right_b;
        right_child_b->min_u = tmp_interval_right_b->left;
        right_child_b->max_u = tmp_interval_right_b->right;
        right_child_b->leftmin = INT_MAX;
        right_child_b->rightmax = INT_MIN;
        
        tmp->key = b;
        tmp->left = left_child_b;
        tmp->right = right_child_b;
        tmp->height += 1;
        tmp->leftmin = INT_MAX;
        tmp->rightmax = INT_MIN;
        
        if(tmp->left->assoc_interval_list != NULL){
            if(tmp->left->rightmax == tmp->left->min_u){
                tmp->left->measure = 0;
            } else {
                tmp->left->measure = tmp->key - tmp->left->min_u;
            }
        }
        
        tmp->min_u = tmp->left->min_u;
        tmp->max_u = tmp->right->max_u;

        rebalance();

        
        
    }
    
    
    associate_interval(tree, a, b);
    
    
    return ;
    
}



void left_rotation(m_tree_t* node)
{
    m_tree_t* tmp_node;
    int tmp_key;
    int tmp_min_u;
    int tmp_max_u;
    int tmp_leftmin;
    int tmp_rightmax;
    int tmp_measure;
    intervals* tmp_associated_intervals;
    
    tmp_node = node->left;
    tmp_key = node->key;
    tmp_min_u = node->min_u;
    tmp_max_u = node->max_u;
    tmp_leftmin = node->leftmin;
    tmp_rightmax = node->rightmax;
    tmp_associated_intervals = node->assoc_interval_list;
    tmp_measure = node->measure;
    
    node->left = node->right;
    node->key = node->right->key;
    node->min_u = node->right->min_u;
    node->max_u = node->right->max_u;
    node->leftmin = node->right->leftmin;
    node->rightmax = node->right->rightmax;
    node->assoc_interval_list = node->right->assoc_interval_list;
    
    node->right = node->left->right;
    node->left->right = node->left->left;
    node->left->left = tmp_node;
    node->left->key = tmp_key;
    node->left->min_u = tmp_min_u;
    node->left->max_u = tmp_max_u;
    node->left->leftmin = tmp_leftmin;
    node->left->rightmax = tmp_rightmax;
    node->left->assoc_interval_list = tmp_associated_intervals;
    
    
    
    //Updating node->left
    
    node->left->min_u = node->left->left->min_u;
    node->left->max_u = node->left->right->max_u;
    node->left->leftmin = min(node->left->left->leftmin, node->left->right->leftmin);
    node->left->rightmax = max(node->left->left->rightmax, node->left->right->rightmax);
    
    //Updating the node
    node->min_u = node->left->min_u;
    node->max_u = node->right->max_u;
    node->leftmin = min(node->left->leftmin, node->right->leftmin);
    node->rightmax = max(node->left->rightmax, node->right->rightmax);
    
    update_measures(node->left);
    update_measures(node);

}

void right_rotation(m_tree_t* node)
{
    m_tree_t* tmp_node;
    int tmp_key;
    int tmp_min_u;
    int tmp_max_u;
    int tmp_leftmin;
    int tmp_rightmax;
    int tmp_measure;
    intervals* tmp_associated_intervals;
    
    tmp_node = node->right;
    tmp_key = node->key;
    tmp_min_u = node->min_u;
    tmp_max_u = node->max_u;
    tmp_leftmin = node->leftmin;
    tmp_rightmax = node->rightmax;
    tmp_associated_intervals = node->assoc_interval_list;
    tmp_measure = node->measure;
    
    node->right = node->left;
    node->key = node->left->key;
    node->min_u = node->left->min_u;
    node->max_u = node->left->max_u;
    node->leftmin = node->left->leftmin;
    node->rightmax = node->left->rightmax;
    node->assoc_interval_list = node->left->assoc_interval_list;
    
    node->left = node->right->left;
    node->right->left = node->right->right;
    node->right->right = tmp_node;
    node->right->key = tmp_key;
    node->right->min_u = tmp_min_u;
    node->right->max_u = tmp_max_u;
    node->right->leftmin = tmp_leftmin;
    node->right->rightmax = tmp_rightmax;
    node->right->assoc_interval_list = tmp_associated_intervals;
    
    
    //Updating node->right
    
    node->right->min_u = node->right->left->min_u;
    node->right->max_u = node->right->right->max_u;
    node->right->leftmin = min(node->right->left->leftmin, node->right->right->leftmin);
    node->right->rightmax = max(node->right->left->rightmax, node->right->right->rightmax);
    
    //Updating the node
    node->min_u = node->left->min_u;
    node->max_u = node->right->max_u;
    node->leftmin = min(node->left->leftmin, node->right->leftmin);
    node->rightmax = max(node->left->rightmax, node->right->rightmax);
    
    update_measures(node->right);
    update_measures(node);
    
}



void rebalance(){
    int finished = 0;
    m_tree_t *tmp_node;
    while((!finished) && rb_st.size()!=0)
    {
        int old_height,tmp_height;
        tmp_node = rb_st[rb_st.size() -1];
        rb_st.pop_back();
        old_height = tmp_node->height;
        
        if((tmp_node->left->height - tmp_node->right->height) == 2)
        {
            
            if((tmp_node->left->left->height - tmp_node->right->height) == 1)
            {
                right_rotation(tmp_node);
                
                tmp_node->right->height = tmp_node->right->left->height + 1;
                tmp_node->height = tmp_node->right->height + 1;
                
                
                
            }
            else
            {
                left_rotation(tmp_node->left);
                
                
                right_rotation(tmp_node);
                tmp_height = tmp_node->left->left->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
                
            }
        }
        else if ((tmp_node->right->height - tmp_node->left->height) == 2)
        {
            if((tmp_node->right->right->height - tmp_node->left->height) == 1)
            {
                left_rotation(tmp_node);
                tmp_node->left->height = tmp_node->left->right->height + 1;
                tmp_node->height = tmp_node->left->height + 1;
                
            }
            else
            {
                right_rotation(tmp_node->right);
                
                
                left_rotation(tmp_node);
                tmp_height = tmp_node->right->right->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
            }
        }
        else
        {
            if(tmp_node->left->height > tmp_node->right->height)
            {
                tmp_node->height = tmp_node->left->height + 1;
                
            }
            else
            {
                tmp_node->height = tmp_node->right->height + 1;
            }
            
            
        }
        
        if(tmp_node->height == old_height)
            finished = 1;
        
        
    }
    
    //Rebalance may terminate before stack gets empty hence go up till root
    
    while(rb_st.size()!=0){
        tmp_node = rb_st[rb_st.size() -1];
        rb_st.pop_back();
        
        tmp_node->min_u = tmp_node->left->min_u;
        tmp_node->max_u = tmp_node->right->max_u;
    }
    
    
    rb_st.clear();
}

void update_measures(m_tree_t *tmp){
    
    if(tmp->right->leftmin < tmp->min_u && tmp->left->rightmax >= tmp->max_u){
        tmp->measure = tmp->max_u - tmp->min_u;
    } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax >= tmp->max_u){
        tmp->measure = tmp->max_u - tmp->key + tmp->left->measure;
    } else if (tmp->right->leftmin < tmp->min_u && tmp->left->rightmax < tmp->max_u){
        tmp->measure = tmp->right->measure + tmp->key - tmp->min_u;
    } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax < tmp->max_u){
        tmp->measure = tmp->right->measure + tmp->left->measure;
    } else {
        //printf("Something Went wrong");
    }
    
    
}

void associate_interval(m_tree_t *tree, int a, int b){
    if(tree->left == NULL){
        return;
    }
    
    ia_st.clear();
    
    m_tree_t *tmp_node;
    tmp_node = tree;
    
    while(tmp_node->right != NULL){
        ia_st.push_back(tmp_node);
        if(a < tmp_node->key){
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }
    
    int tmp_min, tmp_max, tmp_measure;
    tmp_max = min(b, tmp_node->max_u);
    tmp_min = max(a, tmp_node->min_u);
    tmp_measure = tmp_max - tmp_min;
    
    tmp_node->measure = max(tmp_measure, tmp_node->measure);
    
    if(tmp_node->assoc_interval_list == NULL){
        tmp_node->leftmin = a;
        tmp_node->rightmax = b;
        
    }
    else
    {
        
        tmp_node->leftmin = min(a,tmp_node->leftmin);
        tmp_node->rightmax = max(b, tmp_node->rightmax);
        
    }
    
    
    tmp_node->assoc_interval_list = insert_to_associated_ll(tmp_node->assoc_interval_list, a, b);
    

    while (ia_st.size() != 0){
        m_tree_t *tmp = ia_st[ia_st.size() - 1];
        ia_st.pop_back();
        
        tmp->leftmin = min(tmp->leftmin, min(tmp->left->leftmin,tmp->right->leftmin));
        tmp->rightmax = max(tmp->rightmax, max(tmp->left->rightmax,tmp->right->rightmax));

        if(tmp->right->leftmin < tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - tmp->min_u;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - tmp->key + tmp->left->measure;
        } else if (tmp->right->leftmin < tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->key - tmp->min_u;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->left->measure;
        } else {
            //printf("Something Went wrong");
        }
        
    }
    
    
    
    //For B
    
    ia_st.clear();
    tmp_node = tree;
    
    while(tmp_node->right != NULL){
        ia_st.push_back(tmp_node);
        if(b < tmp_node->key){
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }
    
    tmp_min, tmp_max, tmp_measure;
    tmp_max = min(b, tmp_node->max_u);
    tmp_min = max(a, tmp_node->min_u);
    tmp_measure = tmp_max - tmp_min;
    tmp_node->measure = max(tmp_measure, tmp_node->measure);
    
    if(tmp_node->assoc_interval_list == NULL){
        tmp_node->leftmin = a;
        tmp_node->rightmax = b;
    }
    else
    {
        tmp_node->leftmin = min(a,tmp_node->leftmin);
        tmp_node->rightmax = max(b, tmp_node->rightmax);
        
    }
    tmp_node->assoc_interval_list = insert_to_associated_ll(tmp_node->assoc_interval_list, a, b);
    
    while (ia_st.size() != 0){
        m_tree_t *tmp = ia_st[ia_st.size() - 1];
        ia_st.pop_back();
        
        tmp->leftmin = min(tmp->leftmin, min(tmp->left->leftmin,tmp->right->leftmin));
        tmp->rightmax = max(tmp->rightmax, max(tmp->left->rightmax,tmp->right->rightmax));
        
        //tmp->measure
        if(tmp->right->leftmin < tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - tmp->min_u;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - tmp->key + tmp->left->measure;
        } else if (tmp->right->leftmin < tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->key - tmp->min_u;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->left->measure;
        } else {
            //printf("Something Went wrong");
        }
        
    }
    
}


intervals * insert_to_associated_ll(intervals *head, int a, int b){
    intervals *temp;
    temp = (intervals *)malloc(sizeof(intervals));
    if(head != NULL){
        head->prev = temp;
    }
    temp->left = a;
    temp->right = b;
    temp->next = head;
    temp->prev = NULL;
    
    head = temp;
    return head;
}

void delete_interval(m_tree_t * tree, int a, int b){
    if(tree->left == NULL){
        return;
    }
    
    ia_st.clear();
    m_tree_t *tmp_node;
    tmp_node = tree;
    
    while(tmp_node->right != NULL){
        ia_st.push_back(tmp_node);
        if(a < tmp_node->key){
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }
    
    if (tmp_node->assoc_interval_list == NULL){
        return ;
    } else if(tmp_node->assoc_interval_list->next == NULL){
        if(tmp_node->assoc_interval_list->left == a && tmp_node->assoc_interval_list->right == b){
            tmp_node->assoc_interval_list = NULL;
            tmp_node->leftmin = INT_MAX;
            tmp_node->rightmax = INT_MIN;
            tmp_node->measure = 0;
        }
    } else {
        intervals *tmp_list;
        tmp_list = tmp_node->assoc_interval_list;
        
        while(tmp_list != NULL){
            if(tmp_list->left == a && tmp_list->right == b){
                break;
            }
            tmp_list = tmp_list->next;
        }
        if(tmp_list == NULL){
            printf("\nNo interval found");
            return;
        }
        
        intervals *prev_list = tmp_list->prev;
        if(tmp_list->next != NULL){
            tmp_list->next->prev = prev_list;
            if(prev_list != NULL){
                prev_list->next = tmp_list->next;
            } else {
                tmp_node->assoc_interval_list = tmp_list->next;
            }
        } else {
            prev_list->next = NULL;
        }
        
        int min_list = INT_MAX; //min of interval list
        int max_list = INT_MIN; //max of interval list
        
        tmp_list = tmp_node->assoc_interval_list;
        while(tmp_list != NULL){
            min_list = min(min_list, tmp_list->left);
            max_list = max(max_list, tmp_list->right);
            tmp_list = tmp_list->next;
        }
        
        tmp_node->leftmin = min_list;
        tmp_node->rightmax = max_list;
        
        int tmp_min, tmp_max, tmp_measure;
        tmp_max = min(max_list, tmp_node->max_u);
        tmp_min = max(min_list, tmp_node->min_u);
        tmp_measure = tmp_max - tmp_min;
        
        tmp_node->measure = tmp_measure;
    }
    
    while (ia_st.size() != 0){
        m_tree_t *tmp = ia_st[ia_st.size() - 1];
        ia_st.pop_back();
        tmp->leftmin = min(tmp->left->leftmin,tmp->right->leftmin);
        tmp->rightmax = max(tmp->left->rightmax,tmp->right->rightmax);
        
        if(tmp->right->leftmin < tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - 1;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - tmp->key + tmp->left->measure;
        } else if (tmp->right->leftmin < tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->key - tmp->min_u;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->left->measure;
        } else {
            //printf("Something Went wrong");
        }
        
    }
    
    
    
    
    //For B
    
    ia_st.clear();
    tmp_node = tree;
    
    while(tmp_node->right != NULL){
        ia_st.push_back(tmp_node);
        if(b < tmp_node->key){
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }
    
    if (tmp_node->assoc_interval_list == NULL){
        return;
    } else if(tmp_node->assoc_interval_list->next == NULL){
        if(tmp_node->assoc_interval_list->left == a && tmp_node->assoc_interval_list->right == b){
            tmp_node->assoc_interval_list = NULL;
            tmp_node->leftmin = INT_MAX;
            tmp_node->rightmax = INT_MIN;
            tmp_node->measure = 0;
        }
    } else {
        intervals *tmp_list;
        tmp_list = tmp_node->assoc_interval_list;
        while(tmp_list != NULL){
            if(tmp_list->left == a && tmp_list->right == b){
                break;
            }
            tmp_list = tmp_list->next;
        }
        if(tmp_list == NULL){
            return;
        }
        
        
        intervals *prev_list = tmp_list->prev;
        if(tmp_list->next != NULL){
            tmp_list->next->prev = prev_list;
            if(prev_list != NULL){
                prev_list->next = tmp_list->next;
            } else {
                tmp_node->assoc_interval_list = tmp_list->next;
            }
        } else {
            prev_list->next = NULL;
        }
        
        
        int min_list = INT_MAX; //min of interval list
        int max_list = INT_MIN; //max of interval list
        
        tmp_list = tmp_node->assoc_interval_list;
        while(tmp_list != NULL){
            min_list = min(min_list, tmp_list->left);
            max_list = max(max_list, tmp_list->right);
            tmp_list = tmp_list->next;
        }
        
        tmp_node->leftmin = min_list;
        tmp_node->rightmax = max_list;
        
        int tmp_min, tmp_max, tmp_measure;
        tmp_max = min(max_list, tmp_node->max_u);
        tmp_min = max(min_list, tmp_node->min_u);
        tmp_measure = tmp_max - tmp_min;
        
        tmp_node->measure = tmp_measure;
    }
    
    while (ia_st.size() != 0){
        m_tree_t *tmp = ia_st[ia_st.size() - 1];
        ia_st.pop_back();
        
        tmp->leftmin = min(tmp->left->leftmin,tmp->right->leftmin);
        tmp->rightmax = max(tmp->left->rightmax,tmp->right->rightmax);

        if(tmp->right->leftmin < tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - 1;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax >= tmp->max_u){
            tmp->measure = tmp->max_u - tmp->key + tmp->left->measure;
        } else if (tmp->right->leftmin < tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->key - tmp->min_u;
        } else if (tmp->right->leftmin >= tmp->min_u && tmp->left->rightmax < tmp->max_u){
            tmp->measure = tmp->right->measure + tmp->left->measure;
        } else {
            //printf("Something Went wrong");
        }
        
    }
    
    return;
    
}



void destroy_m_tree(m_tree_t *tree){
    //cout<<"\nDestroying : "<<tree->key;
    rb_st.clear();
    
    m_tree_t *tmp_node = tree;
    rb_st.push_back(tree);
    while(rb_st.size() > 0 ) {
        tmp_node = rb_st[rb_st.size() - 1];
        rb_st.pop_back();
        
        if(tmp_node->right != NULL){
            rb_st.push_back(tmp_node->right);
            rb_st.push_back(tmp_node->left);
        } else{
            free(tmp_node->left);
        }
        free(tmp_node);
    }
    
    rb_st.clear();
    
}










