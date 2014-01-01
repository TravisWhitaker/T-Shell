#ifndef ALIAS_H
#define ALIAS_H

#include "data-structs/hash.h"
#include "data-structs/vector.h"

extern void alias_init(HashTable* rawcmds, Vector* aliases);
extern void alias_free(HashTable* rawcmds, Vector* aliases);

#endif
