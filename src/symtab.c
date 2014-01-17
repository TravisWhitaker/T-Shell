/* Standard: gnu99 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtab.h"

typedef struct symtab {
	int size;
	Symbol* symbols;
} SymbolTable;

SymbolTable table; // The table itself.

Symbol* symtab_find(char* identifier) {
	Symbol* target = NULL;
	for (unsigned int i = 0; i < table.size; i++) {
		if (!strcmp(table.symbols[i].identifier, identifier))
			*target = table.symbols[i];
	}
	return target;
}

void symtab_empty(void) {
	table.size = 0;
	free(table.symbols);
	table.symbols = NULL;
}

void symtab_dump(void) {
	for (unsigned int i = 0; i < table.size; i++)
		printf("%s\n", table.symbols[i].identifier);
}
