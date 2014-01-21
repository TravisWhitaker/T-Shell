// Standard: gnu99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtab.h"

typedef struct symtab {
	unsigned int size; // The number of Symbols in the table.
	Symbol* symbols;   // The array of Symbols.
} SymbolTable;

SymbolTable table; // The table itself.

/*
 * Constructs a Symbol then adds it to the table.
 * Argument(s):
 *   char* uid: The Unique Identifier (Name) of the Symbol.
 *   char* scope: The Scope the Symbol is in.
 *   SymType type: The SymType of Symbol.
 *   GenType value: The Value contained in the Symbol.
 * Note for Memory Management:
 *   Call 'symtab_empty()' when done.
 */
void symtab_add(char* uid, char* scope, SymType type, GenType value) {
	Symbol sym = {uid, scope, type, value};
	table.size++;
	table.symbols = realloc(table.symbols, table.size * sizeof(Symbol));
	table.symbols[table.size - 1] = sym;
}

/*
 * Constructs a Symbol then adds it to the table.
 * Argument(s):
 *   char* uid: The Unique Identifier (Name) of the Symbol.
 * Note for Memory Management:
 *   Call 'symtab_empty()' when done.
 */
void symtab_delete(char* uid) {
	table.size--;
	Symbol* newTable = calloc(table.size, sizeof(Symbol));
	unsigned int i = 0, o = 0;
	for (; i < table.size; i++) {
		if (!strncmp(table.symbols[i].uid, uid, strlen(table.symbols[i].uid))) o++;
		newTable[i] = table.symbols[i+o];
	}
	free(table.symbols);
	table.symbols = newTable;
}

/*
 * Prints information about the Symbol Table.
 */
void symtab_dump(void) {
	printf("Table Size: %d\n", table.size);
	for (unsigned int i = 0; i < table.size; i++) {
		Symbol temp = table.symbols[i];
		printf("[%d]: %s\n", i, temp.uid);
		printf("  -> %s\n", temp.scope);
		if (temp.type == NUMBER) {
			printf("  -> NUMBER\n");
			printf("  -> %d\n", temp.value.Integer);
		} else if (temp.type == STRING) {
			printf("  -> STRING\n");
			printf("  -> %s\n", temp.value.String);
		}
	}
}

/*
 * Sets the table size to Zero and frees the array of Symbols.
 */
void symtab_empty(void) {
	table.size = 0;
	free(table.symbols);
	table.symbols = NULL;
}

/*
 * Finds the Symbol with the given UID.
 * Argument(s):
 *   char* uid: The Unique Identifier (Name) of the Symbol.
 * Returns:
 *   A pointer the Symbol.
 */
Symbol* symtab_find(char* uid) {
	Symbol* target = NULL;
	for (unsigned int i = 0; i < table.size; i++) {
		if (!strncmp(table.symbols[i].uid, uid, strlen(table.symbols[i].uid)))
			target = &table.symbols[i];
	}
	return target;
}

#ifdef SYMTAB_DEBUG
// For testing
int main(void) {
	printf("Size: %d\n", table.size);
	symtab_add("Test", "local", NUMBER, (GenType) 2);
	symtab_add("Test2", "local", STRING, (GenType) "Hello");
	Symbol s = *symtab_find("Test2");
	printf("Size: %d\n", table.size);
	printf("UID: %s\n", s.uid);
	symtab_dump();
	symtab_delete("Test2");
	symtab_dump();
	symtab_empty();
	return 0;
}
#endif
