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
 *   Vector* tokens: a pointer to the tokenized user input
 */
static void changeDir(Vector* tokens) {
	int error = 0;
	if (tokens->size == 2) // Changes to the given directory
		error = chdir(vector_get(tokens, 1).String);
	else if (tokens->size == 1) // Changes to the user's home directory
		error = chdir(getenv("HOME"));
	else
		printf(COLOR_RED "T-Shell: cd: Too many arguments.\n" COLOR_RESET);
	if (error) {
		printf(COLOR_RED);
		perror("T-Shell: cd");
		printf(COLOR_RESET);
	}
}

/*
 * Runs an external program.
 * Argument(s):
 *   char** extArgv: potential arguments for the program
 */
static void execute(char** extArgv) {
	int childExitStatus;
	pid_t childPID = fork(); /* Creates a new process for an
	                            external program to run in */
	if (childPID >= 0) { // Was fork successful?
		if (childPID == 0) {
			if (execvp(extArgv[0], extArgv) == -1) { // Run child process
				printf(COLOR_RED "T-Shell: exec: \'%s\' is not a recognized command...\n" COLOR_RESET, extArgv[0]);
				exit(EXIT_FAILURE);
			}
		} else wait(&childExitStatus); // Parent (this) process waits for child to finish
	} else {
		printf(COLOR_RED);
		perror("T-Shell: fork");
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
	Configuration config = config_read();
	HashTable rawcmds;
	Vector aliases;
	alias_init(&rawcmds, &aliases);
	char* history_path = construct_path(".tsh-history", 12);
	while (true) {
		//==================================================================================
		// Building the Prompt
		char promptb[strlen(config.prompt)];
		strcpy(promptb, config.prompt);
		char* prompt = calloc(BUFFER_SIZE, sizeof(char));
		int amount = 0;
		char** pieces = split_string(promptb, "%", &amount);
		for (size_t i = 0; i < amount; i++) {
			if (contains(pieces[i], "\\n")) { // Newline
				replaceAll(pieces[i], '\\', 8);
				replaceAll(pieces[i], 'n', 10);
			}
			char first = pieces[i][0];
			if (first == 'D') { // Directory
				char* dir = getcwd(NULL, 0);
				if (config.colors) strcat(prompt, COLOR_YELLOW);
				strcat(prompt, dir);
				if (config.colors) strcat(prompt, COLOR_RESET);
				strcat(prompt, pieces[i]+1);
				free(dir);
			} else if (first == 'U') { // Username
				if (config.colors) strcat(prompt, COLOR_CYAN);
				strcat(prompt, getenv("USER"));
				if (config.colors) strcat(prompt, COLOR_RESET);
				strcat(prompt, pieces[i]+1);
			} else if (first == 'H') { // Hostname
				if (config.colors) strcat(prompt, COLOR_MAGENTA);
				strcat(prompt, getenv("HOSTNAME"));
				if (config.colors) strcat(prompt, COLOR_RESET);
				strcat(prompt, pieces[i]+1);
			} else
				strcat(prompt, pieces[i]);
		}
		strcat(prompt, " \0");
		free(pieces);
		//==================================================================================
		char* input = readline(prompt); // Get User input
		free(prompt);
		add_history(input); // Add input to History list
		if (input == NULL) { // Exits when Ctrl-D is pressed
			printf("\n");
			break;
		} else if (input[0] != ASCII_NULL) {
			append_history(1, history_path); // Write input to History file
			if (!strcmp(input, "exit") || !strcmp(input, "quit") || !strcmp(input, "logout")) {
				free(input);
				break;
			} else if (!strcmp(input, "history clear")) {
				truncate(history_path, 0);
			} else {
				//==================================================================================
				// Expands Tilde '~' to the Users Home Directory
				if (contains(input, "~")) {
					char* home = getenv("HOME");
					int freei = 0;
					int tokenNum = 0;
					int length = 1;
					char* tempInput = NULL;
					char** tokens = split_string(input, " ", &tokenNum);
					length += strlen(tokens[0]);
					tempInput = realloc(tempInput, ++length * sizeof(char));
					strcpy(tempInput, tokens[0]);
					strcat(tempInput, " ");
					for (register unsigned int i = 1; i < tokenNum; i++) {
						if (tokens[i][0] == '~') {
							char* sub = substring(tokens[i], 1, strlen(tokens[i]));
							char* temp = calloc(strlen(home)+strlen(sub)+1, sizeof(char));
							strcpy(temp, home);
							strcat(temp, sub);
							free(sub);
							tokens[i] = temp;
							freei = i;
						}
						length += strlen(tokens[i]);
						tempInput = realloc(tempInput, length * sizeof(char));
						strcat(tempInput, tokens[i]);
						if (i < (tokenNum-1)) {
							tempInput = realloc(tempInput, ++length * sizeof(char));
							strcat(tempInput, " ");
						}
					}
					free(tokens[freei]);
					free(tokens);
					free(input);
					input = tempInput;
				}
				//==================================================================================
		 		Vector tokens = vector_split(input, " "); // User input tokens
		 		#define COMMAND vector_get(&tokens, 0).String
				//==================================================================================
				// Injecting the real commands into user input before running.
				char line[BUFFER_SIZE];
				for (register unsigned int i = 0; i < aliases.size; i++) {
					if (!strcmp(COMMAND, vector_get(&aliases, i).String)) { // Does the command have an alias?
						strncpy(line, hash_lookUp(&rawcmds, COMMAND).String, sizeof(line));
						Vector args = vector_split(line, " ");
						for (register unsigned int j = args.size-1; j > 0; j--)
							vector_add(&tokens, 1, vector_get(&args, j));
						free(args.array);
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
				free(tokens.array);
				free(input);
			}
		} else free(input);
	}
	free(history_path); // Free History file path
	alias_free(&rawcmds, &aliases); // Alias Freeing
	return 0;
}

