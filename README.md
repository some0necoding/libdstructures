# Data Structures in C

Library of (more) complex data structures written in C.
 - Dynamic Arrays
 - Hashmaps
 - Linked Lists
 - Doubly Linked Lists
 - Circular Linked Lists
 - Queue
 - Stack

- [x] fix[hmap]: all key comparison should also check for string length equality, otherwise bitstrings with the same prefix but
      different lengths would be considered equal.
- [x] fix[hmap]: hmap_free should also free all entries and their key field which are all heap allocated
- [x] fix[hmap]: all hmap functions should return uint8_t (or uint32_t?) return values
- [x] docs[hmap]: write contracts for exposed methods
- [x] fix[hmap]: hmap_remove does not check for entry existance and could try to access key field on a NULL pointer
- [ ] perf[hmap]: hmap_remove does not resize the table when needed (the halving part in doubling-halving paradigm)
