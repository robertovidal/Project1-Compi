#include "global.h"
#include "array.h"
#define CHAR_SIZE 75

/* Code taken from https://www.techiedelight.com/trie-implementation-insert-search-delete/
 * with little modifications
 */

typedef Array(char) Array_char;
typedef Array(int) Array_int;
typedef Array(Array_char) Array_chars;

// Data structure to store a Trie node
struct Trie{
    bool isLeaf;
    struct Trie* character[CHAR_SIZE]; 
    Array_char value;
    Array_chars vars;
};
 
struct Trie* getNewTrieNode();
 
void insertTrie(struct Trie *head, Array_char str, Array_char value, Array_chars vars);
 
struct Trie* searchTrie(struct Trie* head, Array_char str);

bool isLeafTrie(struct Trie* head);
 
bool hasChildrenTrie(struct Trie* curr);
 
bool deletionTrie(struct Trie **curr, char* str);