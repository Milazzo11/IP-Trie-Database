// File: trie.h
//
// Description: header for integer-keyed trie data type ADT
//
// @author Maximus Milazzo (mam9563@rit.edu)
//
///////////////////////////////////////////////////////////



#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>



/// Public unsigned, integer key type for entries in the trie.
typedef unsigned int ikey_t;



/// Void pointer to trie value.
typedef void * ival_t;



/// Defines a single trie entry with a unique identifer key and place value.
struct Entry_s {

    ival_t value;
    ikey_t key;

};



/// Defines Entry as a pointer to the entry struct.
/// This allows for more efficient swapping/movement of entry values in nodes.
typedef struct Entry_s * Entry;



/// Trie is a pointer to the Trie ADT.
typedef struct Trie_s * Trie;



// constant values for bit processing available to application

extern const size_t BITSPERBYTE;        /// < number of bits in a byte
extern const size_t BITSPERWORD;        /// < number of bits in a word
extern const size_t BYTESPERWORD;       /// < number of bytes in a word
extern const size_t RADIX;              /// < number of possible byte values



/// Create a Trie instance and initialize its fields.
///
/// @param ext_show_value - a function pointer that the user passes
///     to indicate how to display entry key and values properly
/// @param ext_delete_entry - a function pointer that the user passes
///     to indicate how values within a trie entry are freed
///
/// @return pointer to the Trie object instance or NULL on failure
///
/// @post Trie is NULL on failure or initialized with a NULL trie root

Trie ibt_create(void (*ext_show_value)(Entry entry, FILE * stream),
    void (*ext_delete_entry)(Entry entry));



/// Destroy the trie and free all storage.
/// Uses the delete_value function and Trie's delete_entry function to
/// free app-specific entry value data;
/// If the Trie's delete_value function is NULL, then it is assumed
/// no memory deallocation needs to happen within the entry (or that all entry
/// values are directly stored in the ival_t type).
///
/// @param trie - a pointer to a Trie instance
///
/// @pre trie is a valid Trie instance pointer
/// @post the storage associated with the Trie and all data has been freed

void ibt_destroy(Trie trie);



/// Insert an entry into the Trie as long as the entry is not already present.
///
/// @param trie - a pointer to a Trie instance
/// @param key - the unique key ID associated with each trie leaf node
/// @param value - the value entry associated with the leaf node
///
/// @post the trie has grown to include a new entry IFF not already present

void ibt_insert(Trie trie, ikey_t key, ival_t value);



/// Search for the key in the trie by finding
/// the closest entry that matches key in the Trie.
///
/// @param trie - a pointer to a Trie instance
/// @param key - the key to find
///
/// @return entry representing the found entry or a null entry for not found

Entry ibt_search(Trie trie, ikey_t key);



/// Get the size of the trie or number of leaf elements.
///
/// @param trie - a pointer to a Trie instance
///
/// @return size of trie 

size_t ibt_size(Trie trie);



/// Get the node count of the trie: the number of internal nodes.
///
/// @param trie - a pointer to a Trie instance
///
/// @return the count of internal nodes

size_t ibt_node_count(Trie trie);



/// Get height of the trie.
///
/// @param trie - a pointer to a Trie instance
///
/// @return height of trie

size_t ibt_height(Trie trie);



/// Displays an individual value in a the node.
///
/// @param trie - a pointer to a Trie instance
/// @param entry - structure to hold node data
/// @param stream - file stream to display data

void ibt_show_value(Trie trie, Entry entry, FILE * stream);



/// Perform an in-order traversal to show each (key, value) in the trie.
/// Uses Trie's show_value function to show each leaf node's data,
/// and if the function is NULL, output each key and value in hexadecimal.
///
/// @param trie - a pointer to a Trie instance
/// @param stream - the stream destination of output

void ibt_show(Trie trie, FILE * stream);



#endif  // TRIE_H