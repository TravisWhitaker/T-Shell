// Standard: gnu99

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "alias.h"
#include "configuration.h"
#include "data-structs/hash.h"
#include "redirection.h"
#include "strutil/strutil.h"
#include "tsh.h"
#include "data-structs/vector.h"

/*
 * Changes the current working directory.
 * Argument(s):
 *   Vector* tokens, a pointer to the tokenized user input
 *   int size, the size of the array of tokens
 */
static void changeDir(Vector* tokens) {
	if (tokens->size == 2) { // Changes to the given directory
		if (chdir(vector_get(tokens, 1).String) == -1) {
			printf(COLOR_RED);
			perror("T-Shell");
			printf(COLOR_RESET);
		}
	} // Changes to the user's home directory
	else if (tokens->size == 1) {
		if (chdir(getenv("HOME"))) {
			printf(COLOR_RED);
			perror("T-Shell");
			printf(COLOR_RESET);
		}
	}
	else printf(COLOR_RED "T-Shell: Too many arguments.\n" COLOR_RESET);
}

/*
 * Runs an external program.
 * Argument(s):
 *   char** extArgv, potential arguments for the program
 */
static void execute(char** extArgv) {
	int childExitStatus;
	pid_t childPID = fork(); /* Creates a new process for an
	                            external program to run in */
	if (childPID >= 0) { // Was fork successful?
		if (childPID == 0) {
			if (execvp(extArgv[0], extArgv) == -1) { // Run child process
				printf(COLOR_RED "T-Shell: \'%s\' is not a recognized command...\n" COLOR_RESET, extArgv[0]);
				exit(EXIT_FAILURE);
			}
		} else wait(&childExitStatus); // Parent (this) process waits for child to finish
	} else {
		printf(COLOR_RED);
		perror("T-Shell");
		printf(COLOR_RESET);
	}
}

/*
 * Constructs the absolute path for T-Shell's files (i.e. Alias, RC, and History).
 * Arguments:
 *   char* filename: The name of the file
 *   size_t length: The length of the filename.
 * Note for Memory Management:
 *   Free the returned path when done.
 * Returns:
 *   The absolute path of the file.
 */
char* construct_path(char* filename, size_t length) {
	char* filePath = calloc(BUFFER_SIZE, sizeof(char));
	strcpy(filePath, getenv("HOME"));
	strcat(filePath, "/");
	strcat(filePath, filename);
	return filePath;
}

/*
 * Defines how Control-C (SIGINT) behaves.
 */
static void ctrlC() {/* Intentionally Blank */}

/*
 * The Shells main function.
 */
int main(void) {
	signal(SIGINT, ctrlC); /* Sets the behavior for a Control Character,
	                          specifically Ctrl-C (SIGINT) */
	Configuration config = config_read(".tsh-rc", 7);
	HashTable rawcmds;
	Vector aliases;
	alias_init(&rawcmds, &aliases);
	char* history_path = construct_path(".tsh-history", 12);
	while (true) {
		//==================================================================================
		// Building the Prompt
		char promptb[strlen(config.prompt)+1];
		strcpy(promptb, config.prompt);
		strcat(promptb, "\0");
		char* prompt = calloc(BUFFER_SIZE, sizeof(char));
		int amount = 0;
		char** pieces = split_string(promptb, "%", &amount);
		for (size_t i = 0; i < amount; i++) {
			if (contains(pieces[i], "\\n")) { // Newline
				replaceAll(pieces[i], '\\', 127);
				replaceAll(pieces[i], 'n', 10);
			}
			char first = pieces[i][0];
			if (first == 'D') { // Directory
				char* dir = getcwd(NULL, 0);
				strcat(prompt, dir);
				strcat(prompt, pieces[i]+1);
				free(dir);
			} else if (first == 'U') { // Username
				strcat(prompt, getenv("USER"));
				strcat(prompt, pieces[i]+1);
			} else if (first == 'H') { // Hostname
				strcat(prompt, getenv("HOSTNAME"));
				strcat(prompt, pieces[i]+1);
			} else
				strcat(prompt, pieces[i]);
		}
		strcat(prompt, " \0");
		free(pieces);
		//==================================================================================
		char* input = readline(prompt);
		free(prompt);
		add_history(input);
		if (input == NULL) { // Exits when Ctrl-D is pressed
			printf("\n");
			break;
		} else if (input[0] != STRING_END) {
			append_history(1, history_path);
			if (!strcmp(input, "exit") || !strcmp(input, "quit") || !strcmp(input, "logout")) {
				release(input); // Frees user input
				break;
			} else {
		 		Vector tokens = vector_split(input, " "); // User input tokens
		 		#define COMMAND vector_get(&tokens, 0).String
				//==================================================================================
				// Injecting the real commands into user input before running.
				char line[BUFFER_SIZE];
				for (unsigned int i = 0; i < aliases.size; i++) {
					if (!strcmp(COMMAND, vector_get(&aliases, i).String)) { // Does the command have an alias?
						strncpy(line, hash_lookUp(&rawcmds, COMMAND).String, sizeof(line));
						Vector args = vector_split(line, " ");
						for (unsigned int j = args.size-1; j > 0; j--)
							vector_add(&tokens, 1, vector_get(&args, j));
						release(args.array); /* Deletes the Alias line buffer if the input
						                        command did not match the current key */
						break;
					}
				}
				//==================================================================================
				if (!strcmp(COMMAND, "cd")) changeDir(&tokens);
				else {
					//------------------------------------------------------------------------------
					// Sets up argv, then runs the command
					char* extArgv[tokens.size+1];
					for (register unsigned int j = 0; j < tokens.size; j++)
						extArgv[j] = vector_get(&tokens, j).String;
					extArgv[tokens.size] = NULL;
					if (!redirect_pipe(tokens.size+1, extArgv) &&
						!redirect_in(tokens.size+1, extArgv) &&
						!redirect_out(tokens.size+1, extArgv)
					) execute(extArgv); // Executes the external program
					//------------------------------------------------------------------------------
				}
				#undef COMMAND
				release(tokens.array); // Deletes the input tokens
				release(input); // Frees user input
			}
		} else release(input); // Frees user input
	}
	free(history_path);
	alias_free(&rawcmds, &aliases); // Alias Freeing
	return 0;
}

#undef COLOR_RED
#undef COLOR_RESET
#undef BLANK_SPACE
#undef BUFFER_SIZE
#undef STRING_END
