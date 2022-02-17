#ifndef __BINARYTREE_H
#define __BINARYTREE_H

typedef struct binaryTreeNode {
    void* data;
    struct binaryTreeNode *left;
    struct binaryTreeNode *right;
} binaryTreeNode;

typedef struct binaryTreeIter {
    binaryTreeNode *next;
    int order;
} binaryTreeIter;

typedef struct binaryTree {
    binaryTreeNode *root;
    binaryTreeNode *min_node;
    binaryTreeNode *max_node;
    long depth;
    long num_nodes;
    long num_leaves;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
};

/* Functions implemented as macros */
#define treeSetDupMethod(t,m) ((t)->dup = (m))
#define treeSetFreeMethod(t,m) ((t)->free = (m))
#define treeSetMatchMethod(t,m) ((t)->match = (m))
#define treeGetDupMethod(t) ((t)->dup)
#define treeGetFreeMethod(t) ((t)->free)
#define treeGetMatchMethod(t) ((t)->match)
#define min(t) do {
    if ((t)->min_node) \
        return ((t)->min_node->data); \
    else \
        return NULL; \
} while (0)
#define max(t) do {
    if ((t)->max_node) \
        return ((t)->max_node->data); \
    else \
        return NULL; \
} while (0)

/* Prototypes */
bst_nodeptr search(bst_nodeptr root, char* data);
void insert(bst_nodeptr root, char* data);
void remove(bst_nodeptr root, char* data);
void inorder_traversal(bst_nodeptr root);
void preorder_traversal(bst_nodeptr root);
void postorder_traversal(bst_nodeptr root);

/* Directions for iterators */
#define BT_FIRST_PRE 0
#define BT_FIRST_IN 1
#define BT_FIRST_POST 1

#endif