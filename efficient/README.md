## search the first occurence of an item in an unsorted array

  From **Chapter 3.4** of **fxtbook**, which discuesses `first_eq_idx_v1` and `first_eq_idx_v3` in `mysearch.h`.

  Notice that the book's implementation can be further improved, using a trick of moving the pointer (`array++` and `*array`) instead of indexing (`array[k]`). `first_eq_idx_v2` is improved from `first_eq_idx_v1`, `first_eq_idx_v4` is improved from `first_eq_idx_v3`, and both show significantly improvement on time cost.

  The following four cases shows the running time for array of size: 0.1 million, 0.5 million, 1 million and 5 million, respectively.

        $ ../test/search.benchmark 100000000
        first_eq_idx_v1 (found): : 100000000 items in 63 ms
        first_eq_idx_v2 (found): : 100000000 items in 56 ms
        first_eq_idx_v3 (found): : 100000000 items in 48 ms
        first_eq_idx (found): : 100000000 items in 46 ms

        $ ../test/search.benchmark 500000000
        first_eq_idx_v1 (found): : 500000000 items in 290 ms
        first_eq_idx_v2 (found): : 500000000 items in 250 ms
        first_eq_idx_v3 (found): : 500000000 items in 211 ms
        first_eq_idx (found): : 500000000 items in 203 ms

        $ ../test/search.benchmark 1000000000
        first_eq_idx_v1 (found): : 1000000000 items in 580 ms
        first_eq_idx_v2 (found): : 1000000000 items in 500 ms
        first_eq_idx_v3 (found): : 1000000000 items in 422 ms
        first_eq_idx (found): : 1000000000 items in 405 ms

        $ ../test/search.benchmark 5000000000
        first_eq_idx_v1 (found): : 5000000000 items in 2897 ms
        first_eq_idx_v2 (found): : 5000000000 items in 2503 ms
        first_eq_idx_v3 (found): : 5000000000 items in 2106 ms
        first_eq_idx (found): : 5000000000 items in 2023 ms