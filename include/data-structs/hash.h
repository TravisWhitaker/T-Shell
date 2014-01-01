#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "data-structs/gentype.h"

typedef struct {
	int size;
	GenType* table;
} HashTable;

extern HashTable hash_init(int size);
extern GenType hash_lookUp(HashTable* tablePtr, char* key);
extern void hash_map(HashTable* tablePtr, char* key, char* value);
extern void hash_unmap(HashTable* tablePtr, char* key);

#endif
