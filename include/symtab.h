/* Standard: gnu99 */

#ifndef SYMTAB_H
#define SYMTAB_H

#include "data-structs/gentype.h"

typedef struct sym {
	char* identifier;
	char* scope;
	char* type; // Keep as char* ?
	GenType value;
} Symbol;

extern void symtab_add(char* indentifier, char* scope, char* type, GenType value);
// Lookup
// Delete
extern void symtab_empty(void);
extern void symtab_dump(void);

#endif

