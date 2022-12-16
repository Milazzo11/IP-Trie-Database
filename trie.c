// File: trie.c 
//
// Description: module for an integer-keyed trie ADT data type
// 
// @author Maximus Milazzo (mam9563@rit.edu)
//
///////////////////////////////////////////////////////////



#include "trie.h"



/// Node is a pointer to a dynamically allocated struct Node_s.
typedef struct Node_s * Node;



/// Node_s is a binary tree node
struct Node_s {

    Entry entry;
    Node left;
    Node right;

};



/// Defines the struct for the Trie ADT
struct Trie_s {

    Node root;
    // root node

    size_t num_nodes;
    size_t leaf_nodes;
    size_t height;
    // trie data

    void (*ibt_show_value_w)(Entry entry, FILE * stream);
    // pointer to user-passed "show value" function

    void (*ibt_delete_entry)(Entry entry);
    // pointer to user-passed entry deletion function
    // should free any dynamically allocated memory used in entry values

};



// global constants for bit operations and sizes
// used by the application program

const size_t BITSPERBYTE = 8;        /// < number of bits in a byte
const size_t BITSPERWORD = 32;       /// < number of bits in a word
const size_t BYTESPERWORD = 4;       /// < number of bytes in a word
const size_t RADIX = 256;            /// < number of possible byte values



/// Creates and returns the initial Trie instance.

Trie ibt_create(void (*ext_show_value)(Entry entry, FILE * stream),
    void (*ext_delete_entry)(Entry entry)) {

    Trie trie = (Trie) malloc(sizeof(struct Trie_s));
    
    if (trie == NULL)  // signifies an allocation failure
        return NULL;

    trie->root = NULL;
    trie->num_nodes = 0;
    trie->leaf_nodes = 0;
    trie->height = 0;
    // sets initial values

    trie->ibt_show_value_w = ext_show_value;
    // assigns user-passed display function

    trie->ibt_delete_entry = ext_delete_entry;
    // assigns user-passed entry free function

    return trie;

}



/// Frees the memory for a single Trie node.
///
/// @param trie - the Trie instance
/// @param node - the node whose values are being deallocated

static void ibt_delete_value(Trie trie, Node node) {
    
    if (node->entry != NULL) {

        if (trie->ibt_delete_entry != NULL)  // calls user entry free function
            trie->ibt_delete_entry(node->entry);

        free(node->entry);

    }

    free(node);

}



/// Performs a post-order traversal on the Trie to free all allocated memory.
/// Makes calls to the ibt_delete_value function.
///
/// @param trie - the Trie instance
/// @param node - the current node being recursed upon (and eventually freed)

static void ibt_destroy_rec(Trie trie, Node node) {

    if (node == NULL)
        return;

    ibt_destroy_rec(trie, node->left);
    ibt_destroy_rec(trie, node->right);

    ibt_delete_value(trie, node);

}



/// Frees all dynamically allocates memory used in the Trie instance.

void ibt_destroy(Trie trie) {

    ibt_destroy_rec(trie, trie->root);
    free(trie);

}



/// Creates a new empty node.
///
/// @return the new node

static Node ibt_make_node(void) {

    Node node = (Node) malloc(sizeof(struct Node_s));

    node->entry = NULL;
    node->left = NULL;
    node->right = NULL;
    // sets all node values to NULL (node is "empty")

    return node;

}



/// Makes a new leaf node.
///
/// @param entry - data entry to put in node
///
/// @return the new node

static Node ibt_make_leaf(Entry entry) {
    
    Node node = (Node) malloc(sizeof(struct Node_s));
    
    node->entry = entry;
    node->left = NULL;
    node->right = NULL;
    // leaf node has entry but no child nodes

    return node;

}



/// Creates new entry.
///
/// @param key - the entry key
/// @param value - the entry value pointer
///
/// @return the new entry

static Entry ibt_make_entry(ikey_t key, ival_t value) {

    Entry new_ent = (Entry) malloc(sizeof(struct Entry_s));

    new_ent->key = key;
    new_ent->value = value;
    // sets key and value in entry

    return new_ent;

}



/// Creates a trie branch to distinguish between new node and existing node.
///
/// @param node - the starting branch node
/// @param key1 - the key passed originally that needs to be inserted
/// @param key2 - the key value associated with the already existing node
/// @param mask - bit mask used to view individual bits
/// @param val1 - the value passed originally that needs to be inserted
/// @param entry2 - the entry associated with the existing node
/// @param nc - node count increment (passed as pointer)
/// @param bh - branch height increment (passed as pointer)

static void ibt_make_branch(Node node, ikey_t key1, ikey_t key2, ikey_t mask,
    ival_t val1, Entry entry2, size_t * nc, size_t * bh) {
    
    Node cur = node;

    while (1) {  // branch creation loop

        ikey_t bit1 = key1 & mask;
        ikey_t bit2 = key2 & mask;

        if (bit1 == bit2) {  // checked bits are the same

            (*nc)++;
            (*bh)++;

            if (bit1 == 0) {  // similar bits are 1's

                cur->left = ibt_make_node();
                cur = cur->left;
                mask >>= 1;

                continue;

            } else {  // similar bits are 0's

                cur->right = ibt_make_node();
                cur = cur->right;
                mask >>= 1;
                continue;

            }

        } else {  // checked bits are different

            (*nc) += 2;
            (*bh)++;

            if (bit1 == 0) {  // key1 is leftmost value

                cur->left = ibt_make_leaf(ibt_make_entry(key1, val1));
                cur->right = ibt_make_leaf(entry2);

            } else {  // key2 is leftmost value

                cur->left = ibt_make_leaf(entry2);
                cur->right = ibt_make_leaf(ibt_make_entry(key1, val1));
            
            }

            return;

        }

    }

}



/// Inserts nodes into the Trie instance using iteration.
/// In this instance, iteration is used to maximize performance.
///
/// @param trie - the Trie instance
/// @param key - the key being inserted
/// @param value - the value being inserted

static void ibt_insert_iter(Trie trie, ikey_t key, ival_t value, size_t * bh) {

    ikey_t mask = 1;
    mask <<= (BITSPERWORD - 1);
    // creates mask

    Node cur = trie->root;

    while (1) {  // insertion loop

        if (cur->right == NULL && cur->left == NULL) {  // leaf node reached

            if (key != cur->entry->key) {  // make branch if keys are different
                
                Entry e_old = cur->entry;
                cur->entry = NULL;

                ibt_make_branch(cur, key, e_old->key, mask, value, e_old,
                    &trie->num_nodes, bh);
            
            }

            return;

        }

        ikey_t bit = key & mask;

        if (bit == 0) {  // bit is 0

            (*bh)++;

            if (cur->left == NULL) {  // no current left node (make one)

                cur->left = ibt_make_leaf(ibt_make_entry(key, value));
                trie->num_nodes++;

                return;

            }

            cur = cur->left;
            mask >>= 1;
            continue;

        } else {  // bit is 1

            (*bh)++;

            if (cur->right == NULL) {  // no current right node (make one)

                cur->right = ibt_make_leaf(ibt_make_entry(key, value));
                trie->num_nodes++;

                return;

            }

            cur = cur->right;
            mask >>= 1;
            continue;

        }

    }

}



/// Inserts a new node into the Trie instance.

void ibt_insert(Trie trie, ikey_t key, ival_t value) {

    trie->leaf_nodes++;

    if (trie->root == NULL) {  // handles empty tree case

        trie->root = ibt_make_leaf(ibt_make_entry(key, value));
        trie->height = 1;
        trie->num_nodes = 1;

        return;

    }

    size_t bh = 1;
    // holds the current branch height

    ibt_insert_iter(trie, key, value, &bh);

    if (bh > trie->height)  // updates trie height
        trie->height = bh;

}



/// Finds the closest match when an exact search result not found.
///
/// @param node - the current node being recursed upon
/// @param mode - specifies "left" or "right" mode depending on the
///     values of previous node traversals
///
/// @return the entry from the closest matching node

static Entry ibt_closest_match_rec(Node node, char mode) {
    
    if (node->left == NULL && node->right == NULL)  // closest leaf node reached
        return node->entry;

    if (mode == 'l') {  // left "mode" favors moving left down trie

        if (node->left != NULL) {
            
            return ibt_closest_match_rec(node->left, 'l');

        } else {

            return ibt_closest_match_rec(node->right, 'l');

        }

    } else {  // right "mode" favors moving right down trie

        if (node->right != NULL) {

            return ibt_closest_match_rec(node->right, 'r');

        } else {

            return ibt_closest_match_rec(node->left, 'r');

        }

    }

    return NULL;

}



/// Recursivelty searches trie for key query result.
///
/// @param node - the current node being recursed upon
/// @param key - the key to search for
/// @param mask - the bit mask to access key bits
///
/// @return the entry from the closest matching node

static Entry ibt_search_rec(Node node, ikey_t key, ikey_t mask) {
    
    if (node->left == NULL && node->right == NULL)  // leaf node reached
        return node->entry;

    ikey_t bit_rep = key & mask;

    if (bit_rep == 0) {  // bit is 0

        if (node->left == NULL)  // no exact match found
            return ibt_closest_match_rec(node->right, 'l');

        return ibt_search_rec(node->left, key, mask >> 1);

    } else {  // bit is 1

        if (node->right == NULL)  // no exact match found
            return ibt_closest_match_rec(node->left, 'r');

        return ibt_search_rec(node->right, key, mask >> 1);

    }

    return NULL;

}



/// Searches a Trie instance to find the closest match to a key.

Entry ibt_search(Trie trie, ikey_t key) {

    ikey_t mask = 1;
    mask <<= (BITSPERWORD - 1);

    if (trie->root == NULL) {  // handles unexpected empty trie error

        fprintf(stderr, "error: cannot query an empty trie\n");
        ibt_destroy(trie);

        exit(EXIT_FAILURE);

    }

    return ibt_search_rec(trie->root, key, mask);

}



/// Fetches the trie size.

size_t ibt_size(Trie trie) {

    return trie->leaf_nodes;

}



/// Calculates the number of internal trie nodes.

size_t ibt_node_count(Trie trie) {

    return trie->num_nodes - trie->leaf_nodes;

}



/// Fetches the trie height.

size_t ibt_height(Trie trie) {

    return trie->height;

}



/// Acts as a wrapper for user-passed display function to print an entry.

void ibt_show_value(Trie trie, Entry entry, FILE * stream) {

    if (trie->ibt_show_value_w == NULL) {  // handles incorrect ADT usage error

        fprintf(stderr, "error: no user display function defined\n");
        return;

    }
    // if user does not pass a display function pointer on creation an error
    // message displays; the program will still continue to run, though

    trie->ibt_show_value_w(entry, stream);
    // cals user-defined display function

}



/// Recursively displays the elements of the Trie instance.
///
/// @param trie - the Trie instance to display
/// @param node - current node being recursed upon
/// @param stream - the stream where display is output

static void ibt_show_rec(Trie trie, Node node, FILE * stream) {

    if (node == NULL)
        return;

    ibt_show_rec(trie, node->left, stream);

    if (node->entry != NULL)  // shows leaf values
        ibt_show_value(trie, node->entry, stream);

    ibt_show_rec(trie, node->right, stream);

}



/// Displays trie data.

void ibt_show(Trie trie, FILE * stream) {

    ibt_show_rec(trie, trie->root, stream);

}