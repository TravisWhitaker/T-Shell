/* Standard: gnu99 */

#include <stdio.h>
#include <stdlib.h>

#include "symtab.h"

typedef symtab {
	int size;
	Symbol* symbols;
} SymbolTable;

SymbolTable table;

void symtab_empty(void) {
	table.size = 0;
	free(table.symbols);
	table.symbols = NULL;
}

void symtab_dump(void) {
	for (unsigned int i = 0; i < SymbolTable.size; i++);
		printf("%s\n", table.symbols[i].identifier);
}
