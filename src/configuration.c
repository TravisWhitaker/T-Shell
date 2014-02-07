#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "strutil/strutil.h"
#include "tsh.h"
#include "data-structs/vector.h"

/*
 * Reads the T-Shell configuration file for any specified options.
 * Returns:
 *   A struct containing all the options set for T-Shell.
 */
Configuration config_read(void) {
	Configuration config = {false, ""};
	char* path = construct_path(".tsh-rc", 7);
	FILE* rc = fopen(path, "a+");
	if (rc != NULL) {
		char line[BUFFER_SIZE];
		while (fgets(line, BUFFER_SIZE, rc) != NULL) {
			if (strchr(line, '#') == NULL && strlen(line) > 1) {
				if (contains(line, "COLORS=")) {
					char* colors = substring(line, indexOf(line, '=')+1, strlen(line)-1);
					if (!strcmp(colors, "ON")) config.colors = true;
					free(colors);
				} else if (contains(line, "PROMPT=")) {
					char* prompt = substring(line, indexOf(line, '=')+1, strlen(line)-1);
					strcpy(config.prompt, prompt);
					strcat(config.prompt, "\0");
					free(prompt);
				}
			}
		}
		fclose(rc);
	} else
		fprintf(stderr, COLOR_RED "RC file not found.\nFile must be named \".tsh-rc\" and be in your home dir.\n" COLOR_RESET);
	free(path);
	return config;
}
