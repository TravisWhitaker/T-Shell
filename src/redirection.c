// Standard: gnu99

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "redirection.h"

typedef struct redirection_symbol {
	char* symbol;		// The symbol itself.
	unsigned int index; // Its index in argv.
} redir_sym;

/*
 * Raises an error with the given message, then terminates the program.
 * Argument(s):
 *   char* message: The message to be printed whhen the error is raised.
 */
static inline void raise_errno(char* message) {
	perror(message);
	exit(EXIT_FAILURE);
}

/*
 * Returns the arguments within a range (start <= N <= end).
 * Argument(s):
 *   char* argv[]: The command line arguments.
 *   int start: Index of the first argument.
 *   int end: Index of the end argument
 * Note for Memory Management:
 *   Free the returned array when done.
 * Returns:
 *   An array of command line arguments.
 */
static char** args_in_range(char* argv[], int start, int end) {
	char** args = NULL;
	unsigned int index;
	for (unsigned int i = start; i <= end; i++) {
		index = i-start;
		args = realloc(args, (index+1) * sizeof(char*));
		if (i == end) args[index] = NULL;
		else args[index] = argv[i];
	}
	return args;
}

/*
 * Finds the given redirection symbol.
 * Argument(s):
 *   int argc: The number of command line arguments.
 *   char* argv[]: The command line arguments.
 *   const char* symbol: The symbol to search for.
 *   redir_sym* sym: A pointer to the struct to store the symbol and its index in.
 */
static void find_symbol(int argc, char* argv[], const char* symbol, redir_sym* rsym) {
	for (unsigned int i = 1; i < (argc-1); i++) {
		if (!strcmp(argv[i], symbol)) {
			rsym->symbol = argv[i];
			rsym->index = i;
			break;
		}
	}
}

/*
 * Redirects the Standard Input of a program to be a given file.
 * Argument(s):
 *   int argc: The number of command line arguments.
 *   char* argv[]: The command line arguments.
 * Returns:
 *   A 1 for Success or 0 as Failure.
 */
int redirect_in(int argc, char* argv[]) {
	redir_sym isym = {NULL, 0};
	find_symbol(argc, argv, "<", &isym);
	if (isym.symbol != NULL) {
		char** before = args_in_range(argv, 0, isym.index);
		char** after = args_in_range(argv, isym.index+1, argc);
		int ret;
		pid_t childPID = fork();
		FILE* inf = fopen(after[0], "r");
		if (childPID == -1) // It broke
			raise_errno(before[0]);
		else if (childPID == 0) { // Child
			dup2(fileno(inf), 0); // Make inf be the read end
			if (execvp(before[0], before))
				raise_errno(before[0]);
		} else // Parent
			wait(&ret);
		fclose(inf);
		free(before);
		free(after);
		return REDIRECT_SUCCESS;
	} else return REDIRECT_FAILURE;
}

/*
 * Redirects the Standard Output of a program to a given file.
 * Argument(s):
 *   int argc: The number of command line arguments.
 *   char* argv[]: The command line arguments.
 * Returns:
 *   A 1 for Success or 0 as Failure.
 */
int redirect_out(int argc, char* argv[]) {
	redir_sym osym = {NULL, 0};
	find_symbol(argc, argv, ">", &osym);
	if (osym.symbol != NULL) {
		char** before = args_in_range(argv, 0, osym.index);
		char** after = args_in_range(argv, osym.index+1, argc);
		int ret;
		int filedes[2];
		pipe(filedes);
		pid_t childPID = fork();
		if (childPID == -1) // It broke
			raise_errno(before[0]);
		else if (childPID == 0) { // Child
			close(filedes[0]);
			dup2(filedes[1], 1); // Make stdout be the write end
			if (execvp(before[0], before))
				raise_errno(before[0]);
		} else { // Parent
			close(filedes[1]);
			wait(&ret);
		}
		FILE* outf = fopen(after[0], "w+");
		FILE* inf = fdopen(filedes[0], "r");
		char buffer[64];
		while (fgets(buffer, 64, inf) != NULL)
			fprintf(outf, "%s", buffer);
		fclose(outf);
		fclose(inf);
		free(before);
		free(after);
		return REDIRECT_SUCCESS;
	} else return REDIRECT_FAILURE;
}

/*
 * Redirects the Standard Output of a program to 
 * be the Standard Input of another program.
 * Argument(s):
 *   int argc: The number of command line arguments.
 *   char* argv[]: The command line arguments.
 * Returns:
 *   A 1 for Success or 0 as Failure.
 */
int redirect_pipe(int argc, char* argv[]) {
	redir_sym psym = {NULL, 0};
	find_symbol(argc, argv, "|", &psym);
	if (psym.symbol != NULL) {
		char** before = args_in_range(argv, 0, psym.index);
		char** after = args_in_range(argv, psym.index+1, argc);
		int ret;
		int filedes[2];
		pipe(filedes);
		pid_t childPID = fork(); // Run first program
		if (childPID == -1) // It broke
			raise_errno(before[0]);
		else if (childPID == 0) { // Child
			close(filedes[0]);
			dup2(filedes[1], 1); // Make stdout be the write end
			if (execvp(before[0], before))
				raise_errno(before[0]);
		} else { // Parent
			close(filedes[1]);
			wait(&ret);
		}
		childPID = fork(); // Run second program
		if (childPID == -1) // It broke
			raise_errno(before[0]);
		if (childPID == 0) { // Child
			close(filedes[1]);
			dup2(filedes[0], 0); // Make stdin be the read end
			if (execvp(after[0], after))
				raise_errno(before[0]);
		}
		else { // Parent
			close(filedes[0]);
			wait(&ret);
		}
		free(before);
		free(after);
		return REDIRECT_SUCCESS;
	} else return REDIRECT_FAILURE;
}
