//
// File: place_ip.h
//
// Description: function declarations and constants for place_ip module
//
// @author: Max Milazzo (mam9563@rit.edu)
//
///////////////////////////////////////////////////////////



#ifndef PLACE_IP
#define PLACE_IP

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "trie.h"

#define BUFLEN 512
// maximum command query length



/// Converts IPV4 address in "dot" notation into numerical IP representation.
///
/// @param ip - the "dot" IP representation
///
/// @return the numerical IPV4 representation

ikey_t ipv4_to_num(char ip[BUFLEN]);



/// Converts numerical IP representation to IPV4 "dot" notation.
///
/// @param num - the numerical IP representation
///
/// @return the IPV4 address in "dot" notation

char * num_to_ipv4(ikey_t num);



/// Function to display trie entries of specified (char *) type.
///
/// @param entry - the trie node entry to display
/// @param stream - file stream to display entry

void place_ip_show_value(Entry entry, FILE * stream);



/// Function to free place_ip-specific trie entry data values.
/// This function DOES NOT free the trie entries, just the dynamically
/// allocated data within them.
///
/// @param entry - the entry to free data in

void delete_entry(Entry entry);



/// Parses a single CSV file line and inserts data as a Trie node.
///
/// @param trie - the Trie instance
/// @param data_line - the CSV file line

void read_to_trie(Trie trie, char * data_line);



/// Reads data from CSV file to Trie instance.
///
/// @param trie - the Trie instance
/// @param stream - file stream where data is being read from

void read_csv(Trie trie, FILE * stream);



/// Displays trie structure statistics.
///
/// @param trie - the Trie instance

void display_stats(Trie trie);



/// Converts string user command query to ikey_t (unsigned int) type.
///
/// @param query - the user search query
///
/// @return the IP numerical value

ikey_t convert_query(char query[BUFLEN]);



/// Handles user search query, passes to Trie instance, and displays result.
///
/// @param trie - the Trie instance
/// @param query - the user search query

void execute_query(Trie trie, char query[BUFLEN]);



#endif  // PLACE_IP