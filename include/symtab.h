// Standard: gnu99

#ifndef SYMTAB_H
#define SYMTAB_H

#include "data-structs/gentype.h"

typedef enum type {
	NUMBER,   // Number data type (1, 3.14, -2, etc).
	STRING,   // String data type ("Hello", 'C', etc).
	FUNCTION, // Function type (foo(), bar(args), etc).
} Type;

typedef struct sym {
	char* uid;     // The Unique Identifier (Name) of the Symbol.
	char* scope;   // The Scope the Symbol is in. (Many need to change Data Type)
	Type type;     // The Type of Symbol.
	GenType value; // The Value contained in the Symbol. (Many need to change Data Type)
} Symbol;

extern void symtab_add(char* uid, char* scope, Type type, GenType value);
extern void symtab_delete(char* uid);
extern void symtab_dump(void);
extern void symtab_empty(void);
extern Symbol* symtab_find(char* uid);

#endif
