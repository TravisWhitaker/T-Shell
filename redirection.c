/***************************************************************************
    The MIT License (MIT)
    
    Copyright (c) 2013 Tyler Cromwell
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
***************************************************************************/

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
	int index;
} redir_sym;

char** __args_in_range(char* argv[], int start, int end) {
	char** args = NULL;
	int index;
	for (int i = start; i < (end+1); i++) {
		index = i-start;
		args = realloc(args, (index+1) * sizeof(char*));
		if (i == end) args[index] = NULL;
		else args[index] = argv[i];
	}
	return args;
}

void __find_symbol(int argc, char* argv[], const char* symbol, redir_sym* rsym) {
	for (int i = 1; i < (argc-1); i++) {
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
	if (isym.symbol != NULL) {
		char** before = __args_in_range(argv, 0, isym.index);
		char** after = __args_in_range(argv, isym.index+1, argc);
		int ret;
		pid_t childPID = fork();
		FILE* inf = fopen(after[0], "r");
		if (childPID == 0) { // Child
        	dup2(fileno(inf), 0); // Make tmpfile be the read end
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
