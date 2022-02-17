#include <stdio.h>
#include <stdlib.h>
#include "BST.h"

#ifdef __cplusplus
extern "C" {
    #include "zmalloc.h"
}
#endif

bst_nodeptr search(bst_nodeptr root, char* data) {
    bst_nodeptr tmp = root;
    int cmp; 

    while (tmp != NULL) {
        cmp = strcmp(data, tmp->data);
        if (cmp < 0)
            tmp = tmp->left;
        else if (cmp == 0)
            break;
        else
            tmp = tmp->right;
    }

    return tmp;
}

void insert(bst_nodeptr root, char* data) {
    if (root == NULL)
        root = create_bst_node(data);

    bst_nodeptr tmp = root;
    int cmp;

    while (1) {
        cmp = strcmp(data, tmp->data);
        if (cmp < 0) {
            if (tmp->left == NULL) {
                tmp->left = create_bst_node(data);
                break;
            }
            else
                tmp = tmp->left;
        }
        else if (cmp == 0)
            break;
        else {
            if (tmp->right == NULL) {
                tmp->right = create_bst_node(data);
                break;
            }
            else
                tmp = tmp->right;
        }
    }
}

void remove(bst_nodeptr root, char* data) {
    if (root == NULL)
        return;

    bst_nodeptr tmp = root;
    int cmp; 

    while (tmp != NULL) {
        cmp = strcmp(data, tmp->data);
        if (cmp < 0)
            tmp = tmp->left;
        else if (cmp == 0) {
            // the node to be deleted is a leaf
            if (tmp->left == NULL && tmp->right == NULL) {
                free(tmp->data);
                tmp = NULL;
                break;
            }
            // the node to be deleted contains a child
            else if (tmp->left != NULL && tmp->right == NULL) {
                tmp->
            }
            // the node to be deleted contains two children
        }
        else
            tmp = tmp->right;
    }

    return tmp;
}

void inorder_traversal(bst_nodeptr root) {
    stack_ptr s = init_stack();
    bst_nodeptr tmp = root;
    while (tmp != NULL || s->count > 0) {
        while (tmp != NULL) {
            push(s, create_stack_node(tmp));
            tmp = tmp->left;
        }
        if (s->count > 0) {
            stack_nodeptr snode= pop(s);
            printf("%s ", snode->tnode->data);
            tmp = tmp->right;
        }
    }
}

void preorder_traversal(bst_nodeptr root) {
    stack_ptr s = init_stack();
    bst_nodeptr tmp = root;
    while (tmp != NULL || s->count > 0) {
        while (tmp != NULL) {
            printf("%s ", tmp->data);
            push(s, create_stack_node(tmp));
            tmp = tmp->left;
        }
        if (s->count > 0) {
            stack_nodeptr snode= pop(s);
            tmp = tmp->right;
        }
    }
}

void postorder_traversal(bst_nodeptr root) {
    stack_ptr s = init_stack();
    bst_nodeptr tmp = NULL;  // current visiting node
    bst_nodeptr pre = NULL;  // previous visiting node
    push(s, root);
    while (s->count > 0) {
        tmp = pop(s);
        if ((tmp->left == NULL && tmp->right == NULL) || (pre != NULL && (pre == tmp->left || pre == tmp->right))) {
            printf("%s ", tmp->data);
            pre = tmp;
        }
        else {
            if (tmp->right != NULL)
                push(s, tmp->right);
            if (tmp->left != NULL)
                push(s, tmp->left);
        }
    }
}

/*
assistant for non-recursive implementation for BST trasversal 
*/
stack_nodeptr creat_stack_node(bst_nodeptr tnode) {
	stack_nodeptr node = (stack_nodeptr)malloc(sizeof(stack_node));
    node->tnode = tnode;
	return node;
}

stack_ptr init_stack() {
	stack_ptr s = (stack_ptr)malloc(sizeof(stack));
	s->top = NULL;
	s->count = 0;

	return s;
}

int push(stack_ptr s, stack_nodeptr node) {
	if (s == NULL || node == NULL)
		return -1;

	// note: s->top==NULL make no differences
	node->bottom = s->top;
	s->top = node;
	s->count++;
	return 0;
}

stack_nodeptr pop(stack_ptr s) {
	if (s == NULL)
		return NULL;
	else if (s->top == NULL)
		return NULL;
	else {
		stack_nodeptr tmp = s->top;
		s->top = s->top->bottom;
		s->count--;
		tmp->bottom = NULL;

		return tmp;
	}
}