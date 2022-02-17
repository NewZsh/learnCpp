#include <stdio.h>
#include <stdlib.h>
#include "PrefixTree.h"

#define N 40000
#define L 100

int _str_len(char* s) {
    if (s == NULL)
        return -1;
    
    int i = 0;
    while (s[i++] != '\0');
    return i-1;
}

int _str_equal(char* s1, char* s2) {
    char* t1 = s1;
    char* t2 = s2;
    int tmp;
    while (*t1 && (tmp = (*t1++ == *t2++)) == 1);
    if (*t2)
        tmp = -1;
    return tmp;
}

char* _str_cpy(char* src) {
    if (src == NULL)
        return NULL;
    
    char* dst = (char*)malloc(sizeof(char) * _str_len(src));
    char* tmp = dst;
    while (*tmp++ = *src++);
    return dst;
}

char* _str_concat(char* s1, char* s2) {
    char* s = (char*)malloc(sizeof(char) * (_str_len(s1) + _str_len(s2) + 1));
    int len = 0;
    char* tmp = s1;
    while (*tmp != '\0')
        s[len++] = *tmp++;
    tmp = s2;
    while (*tmp != '\0')
        s[len++] = *tmp++; 
    s[len] = '\0';

    return s;
}

prefix_tree_nodeptr create_node(char* s) {
    prefix_tree_nodeptr node = (prefix_tree_nodeptr)malloc(sizeof(prefix_tree_node));
    node->data = (s == NULL)? s: _str_cpy(s);
    node->first_child = NULL;
    node->next_sibling = NULL;

    return node;    
}

void add_word(prefix_tree_nodeptr root, char* word) {
    if (word == NULL || root == NULL)
        return;
    if (word[0] == '\0')
        return;
    
    int i = 0;
    char s[4];
    s[3] = '\0';
    prefix_tree_nodeptr present = root;
    while ((s[i%3] = word[i++]) != '\0') {
        if (i % 3 != 0)
            continue;

        if (present->first_child == NULL) {
            present->first_child = create_node(s);
            present = present->first_child;
        }
        else {
            present = present->first_child;
            while (_str_equal(present->data, s) == -1 && present->next_sibling != NULL)
                present = present->next_sibling;
            if (_str_equal(present->data, s) == -1) {
                present->next_sibling = create_node(s); 
                present = present->next_sibling;
            }
        }
    }
}

void query(prefix_tree_nodeptr root, char* q) {
    if (q[0] == '\0' || root == NULL)
        return;
    
    int i = 0;
    char s[3];
    prefix_tree_nodeptr present = root;
    while ((s[i%3] = q[i++]) != '\0') {
        if (i % 3 == 0) {
            present = present->first_child;
            if (present == NULL)
                return;
            else {
                while (_str_equal(present->data, s) == -1 && (present = present->next_sibling) != NULL);
                if (present == NULL)
                    return;
            }
        }
    }

    printf("find %s\n", present->data);
    exit(-1);
    // char* prefix = (char*)malloc(sizeof(char) * (_str_len(q) - 3));
    // int l = 0;
    // while (l < _str_len(q) - 3)
    //     prefix[l] = q[l++];
    char* prefix = q;
    traverse(present, prefix);
}

void traverse(prefix_tree_nodeptr root, char* prefix) {
    if (prefix == NULL)
        return;
    if (root == NULL) {
        printf("%s\n", prefix);
        return;
    }
    
    char* new_prefix;
    if (root->data != NULL)
        new_prefix = _str_concat(prefix, root->data);
    else
        new_prefix = prefix;

    prefix_tree_nodeptr present = root->first_child;
    traverse(present, new_prefix);
    if (present != NULL)
        while ((present = present->next_sibling) != NULL)
            traverse(present, new_prefix);
}
    

int main(int nargs, char* argv[]) {
    if (nargs != 2) {
        printf("usage: run with a dictionary as parameter.\n");
        return -1;
    }

    /* read in dictionary */
    char* dict[N];
    char buf[L];
    int n = 0;
    FILE* fp;
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("file %s does not exist.\n", argv[0]);
        return -1;
    }

    while (n < N && fgets(buf, L, fp) != NULL) {
        buf[_str_len(buf)-1] = '\0';  // delete the new line symbol
        dict[n++] = _str_cpy(buf);
    }
    fclose(fp);

    char* c = NULL;
    prefix_tree_nodeptr root = create_node(c);
    for (int i = 0; i < n; i++) 
        add_word(root, dict[i]);
    char* prefix = "\0";

    char* q = "不小";
    query(root, q);
    return 0;
}