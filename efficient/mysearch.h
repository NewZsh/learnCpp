#ifndef _MYSEARCH_H
#define _MYSEARCH_H

/* search in an unsorted array
 Return index of first array[i] == item
 Return n if for all i, array[i] != item */
inline unsigned long first_eq_idx_v1(const unsigned int *array, unsigned long n, unsigned int item);
inline unsigned long first_eq_idx_v2(const unsigned int *array, unsigned long n, unsigned int item);
inline unsigned long first_eq_idx_v3(const unsigned int *array, unsigned long n, unsigned int item);
inline unsigned long first_eq_idx(unsigned int *array, unsigned long n, unsigned int item);

inline unsigned long first_idx(void *array, unsigned long n, void *item, bool *cmp);

#endif