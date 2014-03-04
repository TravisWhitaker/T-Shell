#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
				if (strutil_contains(line, "COLORS=")) {
					char* colors = strutil_substring(line, strutil_indexOf(line, '=')+1, strlen(line)-1);
					if (!strcmp(colors, "ON")) config.colors = true;
					free(colors);
				} else if (strutil_contains(line, "PROMPT=")) {
					char* prompt = strutil_substring(line, strutil_indexOf(line, '=')+1, strlen(line)-1);
					strcpy(config.prompt, prompt);
					strcat(config.prompt, "\0");
					free(prompt);
				}
			}
		}
		fclose(rc);
	} else
		fprintf(stderr, COLOR_RED "RC file not found.\nFile must be named \".tsh-rc\" and be in your home directory.\n" COLOR_RESET);
	free(path);
	return config;
}

/*
 * Builds the prompt based on the format read in from '~/.tsh-rc'.
 * Argument(s):
 *   Configuration* config: a pointer to the shell configuration.
 * Note for Memory Management:
 *   Free the returned string when done.
 * Returns:
 *   The shell prompt.
 */
char* config_build_prompt(Configuration* config) {
	char promptb[strlen(config->prompt)];
	strcpy(promptb, config->prompt);
	int length = 0;
	char* prompt = calloc(0, sizeof(char));
	int amount = 0;
	char** pieces = strutil_split(promptb, "%", &amount);
	for (size_t i = 0; i < amount; i++) { // For each token
		if (strutil_contains(pieces[i], "\\n")) { // Newline
			strutil_replaceAll(pieces[i], '\\', ASCII_BACKSPACE);
			strutil_replaceAll(pieces[i], 'n', ASCII_NEWLINE);
		}
		char first = pieces[i][0];
		if (first == 'D') { // Directory
			char* dir = getcwd(NULL, 0);
			if (config->colors) {
				length += COLOR_LENGTH+1;
				prompt = realloc(prompt, length * sizeof(char));
				strncat(prompt, COLOR_YELLOW, COLOR_LENGTH);
				strncat(prompt, "\0", 1);
			}
			length += strlen(dir);
			prompt = realloc(prompt, length * sizeof(char));
			strncat(prompt, dir, strlen(dir));
			if (config->colors) {
				length += COLOR_LENGTH;
				prompt = realloc(prompt, length * sizeof(char));
				strncat(prompt, COLOR_RESET, strlen(COLOR_RESET));
				strncat(prompt, "\0", 1);
			}
			length += strlen(pieces[i]+1)+1;
			prompt = realloc(prompt, length * sizeof(char));
			strncat(prompt, pieces[i]+1, strlen(pieces[i]+1));
			strncat(prompt, "\0", 1);
			free(dir);
		} else if (first == 'U') { // Username
			if (config->colors) {
				length += COLOR_LENGTH+1;
				prompt = realloc(prompt, length * sizeof(char));
				strncat(prompt, COLOR_CYAN, strlen(COLOR_CYAN));
				strncat(prompt, "\0", 1);
			}
			length += strlen(getenv("USER"))+1;
			prompt = realloc(prompt, length * sizeof(char));
			strncat(prompt, getenv("USER"), strlen(getenv("USER")));
			strncat(prompt, "\0", 1);
			if (config->colors) {
				length += COLOR_LENGTH;
				prompt = realloc(prompt, length * sizeof(char));
				strncat(prompt, COLOR_RESET, strlen(COLOR_RESET));
				strncat(prompt, "\0", 1);
			}
			length += strlen(pieces[i]+1)+1;
			prompt = realloc(prompt, length * sizeof(char));
			strncat(prompt, pieces[i]+1, strlen(pieces[i]+1));
			strncat(prompt, "\0", 1);
		} else if (first == 'H') { // Hostname
			if (config->colors) {
				length += COLOR_LENGTH+1;
				prompt = realloc(prompt, length * sizeof(char));
				strncat(prompt, COLOR_MAGENTA, strlen(COLOR_MAGENTA));
				strncat(prompt, "\0", 1);
			}
			length += strlen(getenv("HOSTNAME"))+1;
			prompt = realloc(prompt, length * sizeof(char));
			strncat(prompt, getenv("HOSTNAME"), strlen(getenv("HOSTNAME")));
			strncat(prompt, "\0", 1);
			if (config->colors) {
				length += COLOR_LENGTH;
				prompt = realloc(prompt, length * sizeof(char));
				strncat(prompt, COLOR_RESET, strlen(COLOR_RESET));
				strncat(prompt, "\0", 1);
			}
			length += strlen(pieces[i]+1)+1;
			prompt = realloc(prompt, length * sizeof(char));
			strncat(prompt, pieces[i]+1, strlen(pieces[i]+1));
			strncat(prompt, "\0", 1);
		} else {
			length += strlen(pieces[i])+1;
			prompt = realloc(prompt, length * sizeof(char));
			strncat(prompt, pieces[i], strlen(pieces[i]));
			strncat(prompt, "\0", 1);
		}
	}
	free(pieces);
	return prompt;
}
