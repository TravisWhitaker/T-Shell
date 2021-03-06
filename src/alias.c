// Standard: gnu99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-structs/hash.h"
#include "strutil/strutil.h"
#include "tsh.h"
#include "data-structs/vector.h"

/*
 * Reads T-Shells alias file for all aliased commands.
 * Memory Management:
 *   Free the elements in the struct member 'array', and the array itself
 *   (Vector is included from the header file, 'data-structs/vector.h')
 * Returns: Array of the lines in the file
 */
static Vector alias_read(void) {
	Vector contents = vector_init(0);
	char* filePath = construct_path(".tsh-alias");
	FILE* file = fopen(filePath, "a+");
	if (file != NULL) {
		char line[BUFFER_SIZE];
		unsigned int i = 0;
		while (fgets(line, BUFFER_SIZE, file) != NULL) // While there is something to be read
			if (strchr(line, '#') == NULL && strlen(line) != 1) { // Ignores comments and empty lines
				char* modLine = calloc(strlen(line), sizeof(char)); // Line buffer
				memcpy(modLine, line, strlen(line)-1);
				modLine[strlen(line)-1] = ASCII_NULL;
				vector_add(&contents, i, modLine); // Add line to list of contents
				i++;
			}
		fclose(file);
	}
	free(filePath);
	return contents;
}

void alias_init(HashTable* rawcmds, Vector* aliases) {
	Vector lines = alias_read();
	*aliases = vector_init(lines.size); // Initializes an Array of Aliases
	*rawcmds = hash_init(lines.size); // Initializes a Hash Table of actual commands
	for (unsigned int i = 0; i < lines.size; i++) {
		char* line = (char*) vector_get(&lines, i); // A line in the file
		char* alias = strutil_substring(line, 0, strutil_indexOf(line, ASCII_SPACE)); // The command alias (KEY)
		char* rawcmd = strutil_substring(line, strutil_indexOf(line, '\'')+1, strlen(line)-1); // The real command being run (VALUE)
		vector_set(aliases, i, alias);
		hash_map(rawcmds, alias, rawcmd);
		free(rawcmd);
		free(line);
	}
	free(lines.array);
}

void alias_free(HashTable* rawcmds, Vector* aliases) {
	for (unsigned int i = 0; i < aliases->size; i++) {
		hash_unmap(rawcmds, (char*) vector_get(aliases, i)); // Deletes a Bucket
		free(vector_get(aliases, i));
	}
	free(aliases->array);
	free(rawcmds->table);
}
