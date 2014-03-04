// Standard: gnu99

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-structs/hash.h"
#include "strutil/strutil.h"

#ifdef HASH_DEBUG
	#define COLOR_MAGENTA "\x1b[35m"
	#define COLOR_RESET	  "\x1b[0m"
#endif

#define BUCKET table->table[index] // A single bucket in the table.
#define SPACE 32 // The ASCII value for the Space character.

/*
 * Constructs a hash table using a struct named, HashTable.
 * The struct 'HashTable' has two members,
 * 	  int size: the number of buckets in the table
 * 	  GenType* table: a union pointer
 * 	  (GenType is included from the header file, 'HashTable.h')
 * Argument(s):
 * 	  int size: The size of the table.
 * 	  GenType* table: The pointer to the table.
 * Memory Management:
 * 	  free the pointer to the table when done.
 * Returns: The struct representing a hash table.
 */
HashTable hash_init(int size) {
	HashTable table;
	if (size < 1) table.size = 1;
	else table.size = size;
	table.table = calloc(size, sizeof(void*));
	return table;
}

/*
 * This function produces the value index for a key.
 * The hash code is produced from the remainder of the sum
 * of the ASCII values of string divided by the table size.
 * Argument(s):
 *	  char* key: the look up key.
 *	  int tableSize: the size of the hash table.
 * Returns: The table index for the value associated with the given key	.
 */
static int hash(char* key, int tableSize) {
	unsigned int hash = 0;
	for (unsigned int i = 0; i < strlen(key); i++)
		hash += (hash * 19) + key[i];
	return (hash % tableSize);
}

/*
 * Looks up the value associated with the given key.
 * Argument(s):
 *	  HashTable* table: struct pointer to the table.
 *	  char* key: the key that is used to find a value stored in the table.
 * Returns: The value associated with the given key.
 * 	  		(GenType is included from the header file, 'HashTable.h')
 */
void* hash_lookUp(HashTable* table, char* key) {
	#ifdef HASH_DEBUG
		printf(COLOR_MAGENTA "HASH: LOOKUP: Looking up \"%s\"\n" COLOR_RESET, key);
	#endif
	unsigned int index = hash(key, table->size);
	unsigned int iterations = 0;
	while (iterations < table->size) {
		if (index == table->size) index = 0; /* Here to make sure all buckets 
		                                        in the table are checked. */
		if (BUCKET != NULL) { // If there is something in this bucket
			#ifdef HASH_DEBUG
				printf(COLOR_MAGENTA "HASH: LOOKUP: > %s\n" COLOR_RESET, (char*) BUCKET);
			#endif
			char* buffer = strutil_substring((char*) BUCKET, 0, strutil_indexOf((char*) BUCKET, SPACE));
			char bucketKey[strlen(buffer)];
			for (unsigned int i = 0; i < strlen(buffer); i++)
				bucketKey[i] = buffer[i]; // Collecting the bucket key
			bucketKey[strlen(buffer)] = 0;
			free(buffer);
			if (!strcmp(key, bucketKey)) {// If the given key matches the key in the bucket.
				#ifdef HASH_DEBUG
					printf(COLOR_MAGENTA "HASH: LOOKUP: Found \"%s\"\n" COLOR_RESET, (char*) BUCKET);
				#endif
				return BUCKET;
			}
		}
		index++;
		iterations++;
	}
	#ifdef HASH_DEBUG
		printf(COLOR_MAGENTA "HASH: LOOKUP: Nothing found\n" COLOR_RESET);
	#endif
	return NULL;
}

/*
 * Associates a string (key) to another (value).
 * Argument(s):
 *	  HashTable* table: struct pointer to the table.
 *	  char* key: the string to be associated with another string.
 *	  char* value: the string to be stored in the table.
 * Returns: The index at which the value is stored.
 */
int hash_map(HashTable* table, char* key, char* value) {
	#ifdef HASH_DEBUG
		printf(COLOR_MAGENTA "HASH: MAP: Attempting to map \"%s\" to \"%s\"\n" COLOR_RESET, key, value);
	#endif
	unsigned int index = hash(key, table->size);
	char* buffer = calloc(strlen(value)+1, sizeof(char));
	strcpy(buffer, value);
	unsigned int iterations = 0;
	while (iterations < table->size) {
		if (index == table->size) index = 0;
		if (BUCKET == NULL) { // If there is nothing in this bucket
			#ifdef HASH_DEBUG
				printf(COLOR_MAGENTA "HASH: MAP: \"%s\" mapped to \"%s\" in bucket %d\n" COLOR_RESET, key, value, index);
			#endif
			BUCKET = buffer;
			return index;
		} else {
			char* bucketKey = strutil_substring((char*) BUCKET, 0, strlen(key));
			if (!strcmp(key, bucketKey)) { // If the given key matches the key in the bucket.
				#ifdef HASH_DEBUG
					printf(COLOR_MAGENTA "HASH: MAP: \"%s\" mapped to \"%s\" in bucket %d\n" COLOR_RESET, key, value, index);
				#endif
				free(bucketKey);
				free(BUCKET);
				BUCKET = buffer;
				return index;
			}
			free(bucketKey);
			index++;
		}
		iterations++;
	}
	return -1;
}

/*
 * Disassociates the given string from the value currently
 * in the table, then deletes that same value from the table.
 * Argument(s):
 *	  HashTable* table: struct pointer to the table.
 *	  char* key: the key that is used to find a value stored in the table.
 */
void hash_unmap(HashTable* table, char* key) {
	#ifdef HASH_DEBUG
		printf(COLOR_MAGENTA "HASH: UNMAP: Attempting to unmap \"%s\"\n" COLOR_RESET, key);
	#endif
	unsigned int index = hash(key, table->size);
	unsigned int iterations = 0;
	while (iterations < table->size) {
		if (index == table->size) index = 0;
		if (BUCKET != NULL) { // If there is something in this bucket
			char* buffer = strutil_substring((char*) BUCKET, 0, strlen(key));
			char bucketKey[strlen(buffer)];
			for (unsigned int i = 0; i < strlen(buffer); i++) {
				bucketKey[i] = buffer[i]; // Collecting the bucket key
				#ifdef HASH_DEBUG
					printf(COLOR_MAGENTA "HASH: UNMAP: %d\n" COLOR_RESET, buffer[i]);
				#endif
			}
			bucketKey[strlen(buffer)] = 0;
			#ifdef HASH_DEBUG
				printf(COLOR_MAGENTA "HASH: UNMAP: bucketKey: %s\n" COLOR_RESET, bucketKey);
			#endif
			free(buffer);
			if (!strcmp(key, bucketKey)) { // If the given key matches the key in the bucket.
				#ifdef HASH_DEBUG
					printf(COLOR_MAGENTA "HASH: UNMAP: Unmapped \"%s\" from bucket %d\n" COLOR_RESET, key, index);
				#endif
				free(BUCKET);
				BUCKET = NULL;
				break;
			}
		}
		index++;
		iterations++;
	}
}
