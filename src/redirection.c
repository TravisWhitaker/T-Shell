// Standard: gnu99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "redirection.h"

typedef struct redirection_symbol {
	char* symbol;
	unsigned int index;
} redir_sym;

char** __args_in_range(char* argv[], int start, int end) {
	char** args = NULL;
	unsigned int index;
	for (unsigned int i = start; i < (end+1); i++) {
		index = i-start;
		args = realloc(args, (index+1) * sizeof(char*));
		if (i == end) args[index] = NULL;
		else args[index] = argv[i];
	}
	return args;
}

void __find_symbol(int argc, char* argv[], const char* symbol, redir_sym* rsym) {
	for (unsigned int i = 1; i < (argc-1); i++) {
		if (!strcmp(argv[i], symbol)) {
			rsym->symbol = argv[i];
			rsym->index = i;
			break;
		}
	}
}

int redirect_in(int argc, char* argv[]) {
	redir_sym isym = {NULL, 0};
	__find_symbol(argc, argv, "<", &isym);
	// '<' takes input from
	// '<<' 
	if (isym.symbol != NULL) {
		char** before = __args_in_range(argv, 0, isym.index);
		char** after = __args_in_range(argv, isym.index+1, argc);
		int ret;
		pid_t childPID = fork();
		FILE* inf = fopen(after[0], "r");
		if (childPID == 0) { // Child
			dup2(fileno(inf), 0); // Make inf be the read end
			if (execvp(before[0], before)) {
				perror(before[0]);
				exit(EXIT_FAILURE);
			}
		} else wait(&ret); // Parent
		fclose(inf);
		free(before);
		free(after);
		return REDIRECT_SUCCESS;
	} else return REDIRECT_FAILURE;
}

int redirect_out(int argc, char* argv[]) {
	redir_sym osym = {NULL, 0};
	__find_symbol(argc, argv, ">", &osym);
	// '>' overwrites the file
	// '>>' appends to the file
	if (osym.symbol != NULL) {
		char** before = __args_in_range(argv, 0, osym.index);
		char** after = __args_in_range(argv, osym.index+1, argc);
		int ret;
		int filedes[2];
		pipe(filedes);
		pid_t childPID = fork();
		if (childPID == 0) { // Child
			close(filedes[0]);
			dup2(filedes[1], 1); // Make stdout be the write end
			if (execvp(before[0], before)) {
				perror(before[0]);
				exit(EXIT_FAILURE);
			}
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

int redirect_pipe(int argc, char* argv[]) {
	redir_sym psym = {NULL, 0};
	__find_symbol(argc, argv, "|", &psym);
	if (psym.symbol != NULL) {
		char** before = __args_in_range(argv, 0, psym.index);
		char** after = __args_in_range(argv, psym.index+1, argc);
		int ret;
		int filedes[2];
		pipe(filedes);
		pid_t childPID = fork();
		if (childPID == 0) { // Child
			close(filedes[0]);
			dup2(filedes[1], 1); // Make stdout be the write end
			if (execvp(before[0], before)) {
				perror(before[0]);
				exit(EXIT_FAILURE);
			}
		} else { // Parent
			close(filedes[1]);
			wait(&ret);
		}
		childPID = fork();
		if (childPID == 0) { // Child
			close(filedes[1]);
			dup2(filedes[0], 0); // Make stdin be the read end
			if (execvp(after[0], after)) {
				perror(after[0]);
				exit(EXIT_FAILURE);
			}
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
