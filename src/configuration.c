#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "strutil/strutil.h"
#include "tsh.h"
#include "data-structs/vector.h"

Configuration config_read(char* filename, size_t length) {
	Configuration config = {""};
	char* path = construct_path(filename, length);
	FILE* rc = fopen(path, "a+");
	if (rc != NULL) {
		char line[BUFFER_SIZE];
		size_t i = 0;
		while (fgets(line, BUFFER_SIZE, rc) != NULL) {
			if (strchr(line, '#') == NULL && strlen(line) > 1) {
				if (contains(line, "PROMPT=")) {
					char* prompt = substring(line, indexOf(line, '=')+1, strlen(line)-1);
					strcpy(config.prompt, prompt);
					strcat(config.prompt, "\0");
					free(prompt);
				}
				i++;
			}
		}
		fclose(rc);
	} else {
		fprintf(stderr, COLOR_RED "RC file not found.\n" COLOR_RESET);
		fprintf(stderr, COLOR_RED "File must be named \".tsh-rc\" and be in your home dir.\n" COLOR_RESET);
	}
	free(path);
	return config;
}
