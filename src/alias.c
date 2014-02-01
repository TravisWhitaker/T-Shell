// Standard: gnu99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-structs/hash.h"
#include "strutil/strutil.h"
#include "tsh.h"
#include "data-structs/vector.h"

/*
 * Reads a given file.
 * Argument(s):
 *   char** fileName, the file itself
 * Memory Management:
 *   Free the elements in the struct member 'array', and the array itself
 *   (Vector is included from the header file, 'data-structs/vector.h')
 * Returns: Array of the lines in the file
 */
static Vector alias_read(char* fileName) {
	Vector contents = vector_init(0);
	char* filePath = construct_path(fileName);
	FILE* file = fopen(filePath, "a");
	if (file != NULL) {
		char line[BUFFER_SIZE];
		unsigned int i = 0;
		while (fgets(line, BUFFER_SIZE, file) != NULL) // While there is something to be read
			if (strchr(line, '#') == NULL && strlen(line) != 1) { // Ignores comments and empty lines
				char* modLine = calloc(strlen(line), sizeof(char)); // Line buffer
				memcpy(modLine, line, strlen(line)-1);
				modLine[strlen(line)-1] = STRING_END;
				vector_add(&contents, i, (GenType) modLine); // Add line to list of contents
				i++;
			}
		fclose(file);
	}
	free(filePath);
	return contents;
}

/*
 *
 */
void alias_init(HashTable* rawcmds, Vector* aliases) {
	Vector lines = alias_read(".tsh-alias");
	*aliases = vector_init(lines.size); // Initializes an Array of Aliases
	*rawcmds = hash_init(lines.size); // Initializes a Hash Table of actual commands
	for (unsigned int i = 0; i < lines.size; i++) {
		char* line = vector_get(&lines, i).String; // A line in the file
		char* alias = substring(line, 0, indexOf(line, BLANK_SPACE)); // The command alias (KEY)
		char* rawcmd = substring(line, indexOf(line, '\'')+1, strlen(line)-1); // The real command being run (VALUE)
		vector_set(aliases, i, (GenType) alias);
		hash_map(rawcmds, alias, rawcmd);
		release(rawcmd); // Delete Real Command buffer
		release(line); // Delete Line buffer
	}
	release(lines.array); // Delete the Array of Line buffers
}

/*
 *
 */
void alias_free(HashTable* rawcmds, Vector* aliases) {
	for (unsigned int i = 0; i < aliases->size; i++) {
		hash_unmap(rawcmds, vector_get(aliases, i).String); // Deletes a Bucket
		release(vector_get(aliases, i).String); // Deletes a Key
	}
	release(aliases->array); // Deletes the Array of Aliases
	release(rawcmds->table); // Deletes the Hash Table of Command Aliases
}

#undef BLANK_SPACE
#undef BUFFER_SIZE
#undef STRING_END
