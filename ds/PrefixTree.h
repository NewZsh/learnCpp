#ifndef _PREFIXTREE_H
#define _PREFIXTREE_H

struct Prefix_tree_node {
    char* data;
    struct Prefix_tree_node* first_child;
    struct Prefix_tree_node* next_sibling;
};

typedef struct Prefix_tree_node prefix_tree_node, *prefix_tree_nodeptr;

prefix_tree_nodeptr create_node(char* s);
void add_word(prefix_tree_nodeptr root, char* word);
void query(prefix_tree_nodeptr root, char* query);
void traverse(prefix_tree_nodeptr root, char* prefix);

#endif