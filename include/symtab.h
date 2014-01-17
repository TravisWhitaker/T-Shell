/* Standard: gnu99 */

#ifndef SYMTAB_H
#define SYMTAB_H

#include "data-structs/gentype.h"

typedef enum type {
	NUMBER,
	STRING
} Type;

typedef struct sym {
	char* identifier; // The Name of the Symbol.
	char* scope;      // The Scope the Symbol is in. Keep as char*?
	Type type;        // The Type of Symbol.
	GenType value;    // The Value contained in the Symbol.
} Symbol;

extern void symtab_add(char* identifier, char* scope, Type type, GenType value);
extern Symbol* symtab_find(char* identifier);
extern void symtab_delete_symbol(Symbol symbol);
extern void symtab_delete_id(char* identifier);
extern void symtab_empty(void);
extern void symtab_dump(void);

#endif

