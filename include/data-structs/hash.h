#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "data-structs/gentype.h"

typedef struct {
	int size;
	GenType* table;
} HashTable;

HashTable ht_init(int size);
int hash(char* key, int tableSize);
GenType lookUp(HashTable* tablePtr, char* key);
void map(HashTable* tablePtr, char* key, char* value);
void unmap(HashTable* tablePtr, char* key);

#endif
