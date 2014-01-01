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

#include "data-structs/hash.h"
#include "redirection.h"
#include "strutil.h"
#include "data-structs/vector.h"

#define STRING_END '\0' // Null terminator, marks end of a string.
#define BLANK_SPACE 32 // The ASCII value for the Space character.
#define BUFFER_SIZE BUFSIZ/32 // New buffer size.

/*
 * Frees the memory pointed to by 'ptr'
 * Argument(s):
 *   void* ptr, pointer to ANY allocated memory
 */
static inline void release(void* ptr) {
	free(ptr);
	ptr = NULL;
}

/*
 * Clears the terminal screen and sets the cursor to the top-left corner
 * Although linux has its own clear command, I kept this for the sake of
 * introducing myself to ANSI Escape Codes.
 * Argument(s):
 *   char* inputPtr, a pointer to user input.
 */
static void clearScreen(char* inputPtr) {
	printf("%c[2J%c[1;1H", 27, 27); /* Clears the Screen and
	                                   resets Cursor position */
	rewind(stdout); /* Sets the stream position indicator
	                   to the beginning of the file */
	ftruncate(1, 0); // Truncates the STDOUT (1) to 0 bytes
	free(inputPtr); // Frees user input
}

/*
 * Changes the current working directory.
 * Argument(s):
 *   Vector* tokens, a pointer to the tokenized user input
## *   int size, the size of the array of tokens
 */
static void changeDir(Vector* tokens) {
	if (tokens->size == 2) {
		if (chdir(get(tokens, 1).String) == -1)
			perror("tsh");
	} // Changes to the given directory
	else if (tokens->size == 1) chdir(getenv("HOME"));
	// Changes to the user's home directory
	else printf("tsh: Too many arguments.\n");
}

/*
 * Gets the current (relative) directory
 * Note for Memory Management:
 *   Free the returned pointer when done.
 * Returns: A pointer to name of the current directory
 */
static char* currentDir(void) {
	char absoluteBuffer[BUFFER_SIZE] = ""; // Absolute path buffer
	getcwd(absoluteBuffer, BUFFER_SIZE);
	char* relativePath = strrchr(absoluteBuffer, '/'); // Relative path buffer
	char* relativeBuffer = calloc(strlen(relativePath), sizeof(char));
	unsigned int i = 0;
	for (; i < (strlen(relativePath)-1); i++)
		relativeBuffer[i] = relativePath[i+1];
	relativeBuffer[i] = STRING_END;
	return relativeBuffer;
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
				printf("tsh: \'%s\' is not a recognized command...\n", extArgv[0]);
				exit(EXIT_FAILURE);
			}
		} else wait(&childExitStatus); // Parent (this) process waits for child to finish
	} else perror("tsh"); // No it was not
}

/*
 * Reads a given file.
 * Argument(s):
 *   char** fileName, the file itself
 * Memory Management:
 *   Free the elements in the struct member 'array', and the array itself
 *   (Vector is included from the header file, 'Vector.h')
 * Returns: Array of the lines in the file
 */
static Vector readAlias(char* fileName) {
	#define USER getenv("USER") // User account name
	Vector contents = vect_init(0);
	unsigned int path_size = 0;
	char* filePath;
	if (!strcmp(USER, "root"))  {
		path_size = strlen("/")+strlen(USER)+1+11;
		filePath = realloc(NULL, path_size * sizeof(char));
		strncpy(filePath, "/", path_size);
	} else {
		path_size = strlen("/home/")+strlen(USER)+1+11;
		filePath = realloc(NULL, path_size * sizeof(char));
		strncpy(filePath, "/home/", path_size);
	}
	strncat(filePath, USER, path_size-strlen(USER)-1);
	strncat(filePath, "/", path_size-strlen("/")-1);
	strncat(filePath, fileName, path_size-strlen(filePath)-1);
	#undef USER
	FILE* file = fopen(filePath, "a+");
	if (file != NULL) {
		char line[BUFFER_SIZE];
		unsigned int i = 0;
		while (fgets(line, BUFFER_SIZE, file) != NULL) // While there is something to be read
			if (strchr(line, '#') == NULL && strlen(line) != 1) { // Ignores comments and empty lines
				char* modLine = calloc(strlen(line), sizeof(char)); // Line buffer
				memcpy(modLine, line, strlen(line)-1);
				modLine[strlen(line)-1] = STRING_END;
				add(&contents, i, (GenType) modLine); // Add line to list of contents
				i++;
			}
		fclose(file);
	}
	free(filePath);
	return contents;
}

/*
 * Defines how Control-C (SIGINT) behaves, by not letting it close the shell.
 */
static void ctrlC() {}

/*
 * The Shells main function.
 */
int main(void) {
	signal(SIGINT, ctrlC); /* Sets the behavior for a Control Character,
	                          specifically Ctrl-C (SIGINT) */
	//==============================================================================================
	// Alias Initialization
	Vector lines = readAlias(".tsh-alias");
	Vector aliases = vect_init(lines.size); // Initializes an Array of Aliases
	HashTable realcmds = ht_init(lines.size); // Initializes a Hash Table of actual commands
	for (unsigned int i = 0; i < lines.size; i++) {
		char* line = get(&lines, i).String; // A line in the file
		char* alias = substring(line, 0, indexOf(line, BLANK_SPACE)); // The command alias (KEY)
		char* realcmd = substring(line, indexOf(line, '\'')+1, strlen(line)-1); // The real command being run (VALUE)
		set(&aliases, i, (GenType) alias);
		map(&realcmds, alias, realcmd);
		release(realcmd); // Deletes Real Command buffer
		release(line); // Deletes Line buffer
	}
	release(lines.array); // Deletes the Array of Line buffers
	//==============================================================================================
	while (true) {
		//==========================================================================================
		// Print Prompt and Current (Relative) Directory
		char* relativeDir = currentDir();
		char prompt[BUFFER_SIZE] = "T-Shell: ";
		/* Block 1
		Here */if (strlen(relativeDir) > 0) {
			for (unsigned int i = 0; i < strlen(relativeDir); i++)
				prompt[9+i] = relativeDir[i];
		} else prompt[9] = '/';
		strncat(prompt, ")> \0", sizeof(prompt)-strlen(prompt)-1);
		release(relativeDir); // Frees directory name buffer
		//==========================================================================================
		char* input = readline(prompt);
		add_history(input);
		if (input == NULL) { // Exits when Ctrl-D is pressed
			printf("\n");
			break;
		} else if (input[0] != STRING_END) {
			if (!strcmp(input, "exit") || !strcmp(input, "quit") || !strcmp(input, "logout")) {
				release(input); // Frees user input
				break;
			}
			else if (!strcmp(input, "clear")) clearScreen(input);
			else {
		 		Vector tokens = vect_split(input, " "); // User input tokens
				//==================================================================================
				// Injecting the real commands into user input before running.
				unsigned int lcount = 0; // Iteration Counter
				char argBuff[BUFFER_SIZE];
				Vector args;
				for (unsigned int i = 0; lcount < aliases.size; i++) {
					if (i >= aliases.size) i = 0;
					if (!strcmp(get(&tokens, 0).String, get(&aliases, i).String)) { // Does the command have an alias?
						strncpy(argBuff, lookUp(&realcmds, get(&tokens, 0).String).String, sizeof(argBuff));
						args = vect_split(argBuff, " ");
						for (unsigned int j = args.size-1; j > 0; j--)
							add(&tokens, 1, get(&args, j));
						release(args.array); /* Deletes the Alias line buffer if the input
						                        command did not match the current key */
						break;
					}
					lcount++;
				}
				//==================================================================================
				if (!strcmp(get(&tokens, 0).String, "cd")) changeDir(&tokens);
				else {
					//------------------------------------------------------------------------------
					// Sets up argv, then runs the command
					char* extArgv[tokens.size+1];
					for (register unsigned int j = 0; j < tokens.size; j++)
						extArgv[j] = get(&tokens, j).String;
					extArgv[tokens.size] = NULL;
					if (!redirect_pipe(tokens.size+1, extArgv) &&
						!redirect_in(tokens.size+1, extArgv) &&
						!redirect_out(tokens.size+1, extArgv)
					) execute(extArgv); // Executes the external program
					//------------------------------------------------------------------------------
				}
				release(tokens.array); // Deletes the input tokens
				release(input); // Frees user input
			}
		} else release(input); // Frees user input
	}
	//==============================================================================================
	// Alias Freeing
	for (unsigned int i = 0; i < aliases.size; i++) {
		unmap(&realcmds, get(&aliases, i).String); // Deletes a Bucket
		release(get(&aliases, i).String); // Deletes a Key
	}
	release(aliases.array); // Deletes the Array of Aliases
	release(realcmds.table); // Deletes the Hash Table of Command Aliases
	//==============================================================================================
	return 0;
}

#undef BUFFER_SIZE
#undef BLANK_SPACE
#undef STRING_END
