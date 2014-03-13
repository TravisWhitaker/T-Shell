#ifndef HASHTABLE_H_
#define HASHTABLE_H_

typedef struct {
	unsigned int size;
	void** table;
} HashTable;

extern HashTable hash_init(int size);
extern void* hash_lookUp(HashTable* table, char* key);
extern int hash_map(HashTable* table, char* key, char* value);
extern void hash_unmap(HashTable* table, char* key);

#endif
