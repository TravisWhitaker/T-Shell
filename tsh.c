/***************************************************************************
    Copyright (C) 2013  Tyler J. Cromwell

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

/* Standard: gnu99 */

#include <dirent.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <CHashTable.h>	// I added a directory to my C Include Path
#include <CVector.h>	// which is why I am using the Arrows.
#include <StrUtil.h>	// Normally just use Double Quotes.

#define STRING_END '\0' // Null terminator, marks end of a string.
#define BLANK_SPACE 32 // The ASCII value for the Space character.

/*
 * Frees the memory pointed to by 'ptr'
 * Argument(s):
 *   void* ptr, pointer to ANY allocated memory
 * Memory Management:
 *   Nothing to worry about here.
 * Returns: void
 */
void release(void* ptr) {
	free(ptr);
	ptr = NULL;
}

/*
 * Clears the terminal screen and sets the cursor to the top-left corner
 * Argument(s):
 *   char* inputPtr, a pointer to user input.
 * Memory Management:
 *   Nothing to worry about here.
 * Return(s): void
 */
void clearScreen(char* inputPtr) {
	printf("%c[2J%c[1;1H", 27, 27); /* Clears the Screen and
	                                   resets Cursor position */
	rewind(stdout); /* Sets the stream position indicator
	                   to the beginning of the file */
	ftruncate(1,0); // Truncates the given file (from FD) to a given size
	free(inputPtr); // Frees user input
}

/*
 * Changes the current working directory.
 * Argument(s):
 *   CVector* tokens, a pointer to the tokenized user input
 *   int size, the size of the array of tokens
 * Memory Management:
 *   Nothing to worry about here.
 * Return(s): void
 */
void changeDir(CVector* tokens, int size) {
	if (size == 2)
		chdir(get(tokens, 1).String); /* Changes the Current Directory
		   	                              to the given directory */
	else if (size == 1)
		chdir(getenv("HOME")); /* Changes the Current Directory
		                          to the user's home directory */
	else printf("tsh: cd: too many arguments.\n");
}

/*
 * Gets the current (relative) directory
 * Argument(s):
 *   void
 * Memory Management:
 *   free the pointer to the path when done.
 * Returns: A pointer to name of the current directory
 */
char* currentDir(void) {
	char absoluteBuffer[BUFSIZ] = ""; // Absolute path buffer
	getcwd(absoluteBuffer, BUFSIZ);
	char* relativePath = strrchr(absoluteBuffer, '/'); // Relative path buffer
	char* relativeBuffer = calloc(strlen(relativePath), sizeof(char));
	int i = 0;
	while (i < (strlen(relativePath)-1)) {
		relativeBuffer[i] = relativePath[i+1];
		i++;
	}
	relativeBuffer[i] = STRING_END;
	return relativeBuffer;
}

/*
 * Runs an external program
 * Argument(s):
 *   const char* extBin, the path of the program to be run
 *   char** extArgv, potential arguments for the program
 * Memory Management:
 *   Nothing to worry about here
 * Returns: void
 */
void executeProgram(const char* extBin, char** extArgv) {
	int childExitStatus;
	pid_t childPID = fork(); /* Creates a new process for an
	                            external program to run in */
	if (childPID >= 0) { // Was fork successful?
		if (childPID == 0) execvp(extBin, extArgv); // Run child process
		else wait(&childExitStatus); // Parent (this) process waits for child to finish
	} else perror("fork");
}

/*
 * Reads a given file.
 * Argument(s):
 *   char* fileDir, the path to the file
 *   char** fileName, the file itself
 * Memory Management:
 *   Release the elements in the struct member 'array', and the array itself
 *   (CVector is included from the header file, 'CVector.h')
 * Returns: Array of the lines in the file
 */
CVector readFile(char* fileDir, char* fileName) {
	CVector contents = cv_init(0);
	char filePath[11]; // File path buffer, sized for '.tsh_alias'
	strcpy(filePath, fileDir);
	strcat(filePath, fileName);
	FILE* file = fopen(filePath, "r");
	if (file == NULL)
		printf("tsh: File not found or failed to open.\n");
	else {
		char line[BUFSIZ];
		int i = 0;
		while (fgets(line, BUFSIZ, file) != NULL) // While there is something to be read
			if (strchr(line, '#') == NULL && strlen(line) != 1) { // Ignores comments and empty lines
				char* modLine = calloc(strlen(line), sizeof(char)); // Line buffer
				memcpy(modLine, line, strlen(line)-1);
				modLine[strlen(line)-1] = STRING_END;
				add(&contents, i, (GenericType) modLine); // Add line to list of contents
				i++;
			}
		fclose(file);
	}
	return contents;
}

/*
 * Searches through 4 directories for the given program.
 * Argument(s):
 *   char** pathPtr, the path of the given program
 *   char** dirsPtr[], the directories being searched
 *   CVector** tokensPtr, a pointer to the tokenized user input
 * Memory Management:
 *   Nothing to worry about here.
 * Returns: Whether or not the program was found
 */
bool locateProgam(char** pathPtr, char** dirsPtr[], CVector* tokensPtr) {
	bool found = false;
	for (int i = 0; i < 4; i++) {
		DIR* binaryDir = opendir((*dirsPtr)[i]);
		if (binaryDir != NULL) { // Is directory open?
			struct dirent *entry = readdir(binaryDir);
			while (entry != NULL) { // While the directory has items
				if (!strcmp((*entry).d_name, get(tokensPtr, 0).String)) {
					*pathPtr = (*dirsPtr)[i];
					found = true;
					break;
				}
				entry = readdir(binaryDir);
			}
		} else printf("Failed to open '%s'.\n", (*dirsPtr)[i]);
		release(binaryDir); // Deletes the Directory buffer
	}
	return found;
}

/*
 * Defines how Control-C (SIGINT) behaves, by not letting it close the shell.
 * Argument(s):
 *   void
 * Memory Management:
 *   Nothing to worry about here
 * Return(s): void
 */
void ctrlC() {}

/*
 * The Shells main function, most of the work is done in here.
 * Argument(s):
 *   void
 * Memory Management:
 *   Nothing to worry about here
 * Returns: status code
 */
int main(void) {
	signal(SIGINT, ctrlC);	/* Sets the behavior for a Control Character,
	                           specifically Ctrl-C */
	//====================================================================================================
	// Alias Initialization
	CVector lines = readFile(".", "/.tsh_alias"); /* (As of now) the file must be in
	                                                 the same directory as the executable */
	CVector keys = cv_init(lines.size); // Initializes an Array of Keys
	CHashTable aliases = cht_init(lines.size); // Initializes a Hash Table of Command Aliases
	for (int i = 0; i < lines.size; i++) {
		char* key = 
			substr(
				get(&lines, i).String, 0,
				indexOf(get(&lines, i).String, BLANK_SPACE)
			);
		char* alias = 
			substr(
				get(&lines, i).String,
				indexOf(get(&lines, i).String, '\'')+1,
				strlen(get(&lines, i).String)-1
			);
		set(&keys, i, (GenericType) key);
		map(&aliases, key, alias);
		release(alias); // Deletes Alias buffer
		release(get(&lines, i).String); // Deletes Line buffer
	}
	release(lines.array); // Deletes the Array of Line buffers
	//====================================================================================================
	while (true) {
		//================================================================================================
		// Print Prompt and Current (Relative) Directory
		char* relativeDir = currentDir();
		char prompt[BUFSIZ] = "T-Shell: ";
		/* Block 1
		Here */if (strlen(relativeDir) > 0) {
			for (int i = 0; i < strlen(relativeDir); i++)
				prompt[9+i] = relativeDir[i];
		} else prompt[9] = '/';
		strcat(prompt, ")> \0");
		release(relativeDir); // Frees directory name buffer
		//================================================================================================
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
		 		CVector tokens = split(input, " "); // User input tokens
				//========================================================================================
				// Injecting command aliases into user input.
				int lcount = 0;	// Iteration Counter
				char argBuff[BUFSIZ];
				CVector args;
				for (int i = 0; lcount < keys.size; i++) {
					if (i >= keys.size) i = 0;
					if (!strcmp(get(&tokens, 0).String, get(&keys, i).String)) { // Does the command have an alias?
						strcpy(argBuff, lookUp(&aliases, get(&tokens, 0).String).String);
						args = split(argBuff, " ");
						for (int j = args.size-1; 0 < j; j--)
							add(&tokens, 1, get(&args, j));
						release(args.array); /* Deletes the Alias line buffer if the input
						                        command did not match the current key */
						break;
					}
					lcount++;
				}
				//========================================================================================
				if (!strcmp(get(&tokens, 0).String, "cd")) {changeDir(&tokens, tokens.size);}
				else { // Sets up argv, then runs the command
					char** dirs = (char* []){"/bin/", "/sbin/", "/usr/bin/", "/usr/sbin/"}; // Binary Locations
					char* path = "";
					if (locateProgam(&path, &dirs, &tokens)) {
						//================================================================================
						// Sets up argv for the external program
						char* extArgv[tokens.size];
						for (int j = 0; j < tokens.size; j++)
							extArgv[j] = get(&tokens, j).String;
						extArgv[tokens.size] = NULL;
						//================================================================================
						char binPath[strlen(path)+strlen(extArgv[0])]; // Path for the external program
						strcpy(binPath, path);
						strcat(binPath, extArgv[0]);
						executeProgram(binPath, extArgv); // Executes the external program
					}
					else printf("tsh: \'%s\' is not a recognized command...\n", input);
				}
				release(tokens.array); // Deletes the input tokens
				release(input); // Frees user input
			}
		} else release(input); // Frees user input
	}
	//====================================================================================================
	// Alias Freeing
	for (int i = 0; i < keys.size; i++) {
		unmap(&aliases, get(&keys, i).String); // Deletes a Bucket
		release(get(&keys, i).String); // Deletes a Key
	}
	release(keys.array); // Deletes the Array of Keys
	release(aliases.table); // Deletes the Hash Table of Command Aliases
	//====================================================================================================
	return 0;
}
