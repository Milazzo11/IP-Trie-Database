// File: place_ip.c
//
// Description: uses trie ADT to create and access a database of IP addresses
//
// @author: Max Milazzo (mam9563@rit.edu)
//
///////////////////////////////////////////////////////////



#include "place_ip.h"


#define COUNTRYDAT 4
// the number of additional country datapoints provided in CSV file 


#define IPV4STR   15
// IPV4 string length maximum



/// Converts IPV4 string to unsigned integer (ikey_t) representation.

ikey_t ipv4_to_num(char ip[BUFLEN]) {

    unsigned char bytes[] = {0, 0, 0, 0};

    sscanf(ip, "%hhu.%hhu.%hhu.%hhu", bytes, bytes + 1, bytes + 2, bytes + 3);
    // gets IP byte values from "dot" format

    ikey_t result = (bytes[0] << (BITSPERBYTE * 3) |
        bytes[1] << (BITSPERBYTE * 2) | bytes[2] << BITSPERBYTE | bytes[3]);

    return result;

}



/// Converts unsigned integer (ikey_t) IP representation to "dotted" string.

char * num_to_ipv4(ikey_t num) {
 
    ikey_t mask = RADIX - 1;

    unsigned char bytes[4];
    char * result = (char *) calloc(IPV4STR + 1, sizeof(char));

    if (num == (ikey_t) -1) {

        strcpy(result, "INVALID");
        return result;

    }
    // returns the string "INVALID" for IP: 255.255.255.255

    bytes[0] = num & mask;
    bytes[1] = (num >> BITSPERBYTE) & mask;
    bytes[2] = (num >> (BITSPERBYTE * 2)) & mask;
    bytes[3] = (num >> (BITSPERBYTE * 3)) & mask;   
    sprintf(result, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
    // puts assigned byte values into their correct "dot" locations
    
    return result;

}



/// Custom function passed to Trie ADT for printing IP entry values.

void place_ip_show_value(Entry entry, FILE * stream) {

    char * raw_text = (char *) entry->value;
    char * ip = num_to_ipv4(entry->key);
    char val_info[COUNTRYDAT][strlen(raw_text) + 1];

    sscanf(raw_text, "\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"", 
        val_info[0], val_info[1], val_info[2], val_info[3]);
        // gets data from stored format

    fprintf(stream, "%u: (%s, %s: %s, %s, %s)\n", entry->key, ip, val_info[0],
        val_info[1], val_info[3], val_info[2]);
        // displays data with correct formatting

    free(ip);

}



/// Custom function passed to Trie ADT for freeing entry value data.

void delete_entry(Entry entry) {

    free(entry->value);

}



/// Parses a single line of CSV test and inserts data into Trie instance.

void read_to_trie(Trie trie, char * data_line) {
    
    ikey_t lower_num;
    ikey_t upper_num;

    char * lower_str = strtok(data_line, ",");
    sscanf(lower_str, "\"%u\"", &lower_num);
    // gets lower-bound IP

    char * upper_str = strtok(NULL, ",");
    sscanf(upper_str, "\"%u\"", &upper_num);
    // gets upper-bound IP

    char * country_data = strtok(NULL, "\n");

    char * storage_str1 = malloc(strlen(country_data) + 1);
    char * storage_str2 = malloc(strlen(country_data) + 1);
    // all string data must be dynamically allocated with unique pointer values
    // before inserting into the trie -- this memory is freed when the trie is
    // destroyed

    strcpy(storage_str1, country_data);
    strcpy(storage_str2, country_data);
    // stores remaining data in two different memory slots
    // (for lower and upper-bound IP addresses)
    
    ibt_insert(trie, lower_num, storage_str1);
    ibt_insert(trie, upper_num, storage_str2);
    // inserts data in trie

}



/// Reads each line of the CSV file, calling read_to_trie to process data.

void read_csv(Trie trie, FILE * stream) {

    char * buf = NULL;
    size_t blen = 0;
    char empty = 1;

    while (getline(&buf, &blen, stream) > 0) {

        if (ferror(stream)) {  // handles read error

            perror("read failed");

            ibt_destroy(trie);
            free(buf);
            fclose(stream);

            exit(EXIT_FAILURE);

        }

        read_to_trie(trie, buf);
        empty = 0;

    }

    if (empty) {  // handles empty dataset error

        fprintf(stderr, "error: empty dataset\n");
        
        ibt_destroy(trie);
        free(buf);
        fclose(stream);

        exit(EXIT_FAILURE);

    }

    free(buf);

}



/// Uses Trie statistics functions to display structure information.

void display_stats(Trie trie) {

    printf("\nheight: %zu\n", ibt_height(trie));
    printf("size: %zu\n", ibt_size(trie));
    printf("node_count: %zu\n\n\n", ibt_node_count(trie));

}



/// Converts user query into ikey_t (unsigned int) type.

ikey_t convert_query(char query[BUFLEN]) {

    long long raw_num = strtoll(query, NULL, 10);

    if (raw_num == LLONG_MAX) {  // entries are capped at LLONG_MAX

        fprintf(stderr, "error: key %lld is out of range\n", LLONG_MAX);
        return UINT_MAX;

    } else if (raw_num == LLONG_MIN) {  // entries are capped at LLONG_MIN

        fprintf(stderr, "error: key %lld is out of range\n", LLONG_MIN);
        return UINT_MAX;

    }

    return (ikey_t) raw_num;

}



/// Processes and executes a user search query, then displays search results.

void execute_query(Trie trie, char query[BUFLEN]) {

    ikey_t num_query;

    if (strchr(query, '.') == NULL) {  // query is in numeral notation
         
        num_query = convert_query(query);

    } else {  // query is in "dot" notation

        num_query = ipv4_to_num(query);

    }

    Entry res = ibt_search(trie, num_query);

    if (res == NULL) {  // handles unexpected query search failure

        fprintf(stderr, "error: query failure\n");
        ibt_destroy(trie);
        
        exit(EXIT_FAILURE);

    }

    ibt_show_value(trie, res, stdout);

}



/// Program entry point.
/// Creates Trie instance, calls necessary functions, and handles query loop.
///
/// @param argc - number of command line arguments
/// @param argv - command line arguments
///
/// @return EXIT_SUCCESS or EXIT_FAILURE if error occurs

int main(int argc, char * argv[]) {

    if (argc != 2) {  // handles incorrect command arguments error

        fprintf(stderr, "usage: place_ip filename\n");
        return EXIT_FAILURE;

    }

    FILE * fp = fopen(argv[1], "r");

    if (fp == NULL) {  // handles file error

        perror(argv[1]);
        return EXIT_FAILURE;

    }

    Trie trie = ibt_create(place_ip_show_value, delete_entry);

    if (trie == NULL) {  // handles trie memory allocation error
        
        fprintf(stderr, "error: failed to allocate memory for trie\n");
        fclose(fp);

        return EXIT_FAILURE;

    }

    read_csv(trie, fp);
    fclose(fp);

    display_stats(trie);

    puts("Enter an ipv4 string or a number (or a blank line to quit).");

    char query[BUFLEN];

    while (1) {  // query loop

        printf("> ");
        fgets(query, BUFLEN, stdin);
        
        if (query[0] == '\n' || feof(stdin)) {  // exits loop on '\n' or EOF

            printf("\n");
            break;

        }

        execute_query(trie, query);

    }
    

    ibt_destroy(trie);

    return EXIT_SUCCESS;

}