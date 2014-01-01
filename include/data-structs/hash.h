#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "data-structs/gentype.h"

typedef struct {
	int size;
	GenType* table;
} HashTable;

extern HashTable ht_init(int size);
extern GenType lookUp(HashTable* tablePtr, char* key);
extern void map(HashTable* tablePtr, char* key, char* value);
extern void unmap(HashTable* tablePtr, char* key);

#endif
