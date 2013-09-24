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

#include <CHashTable.h>	// I added a directory to my C Include Path
#include <CVector.h>	// which is why I am using the Arrows.
#include <StrUtil.h>	// Normally just use Double Quotes.

#define LINE_END '\0' // Null terminator, marks end of a string.
#define SPACE 32 // The ASCII value for the Space character.

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
	relativeBuffer[i] = LINE_END;
	return relativeBuffer;
}

void extProg(const char* extBin, char** extArgv) {
	int childExitStatus;
	pid_t childPID = fork(); /* Creates a new process for an
	                            external program to run in */
	if (childPID >= 0) { // Was fork successful?
		if (childPID == 0) execvp(extBin, extArgv); // Run child process
		else wait(&childExitStatus); // Parent (this) process waits for child to finish
	} else perror("fork");
}

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
				modLine[strlen(line)-1] = LINE_END;
				add(&contents, i, (GenericType) modLine); // Add line to list of contents
				i++;
			}
		fclose(file);
	}
	return contents;
}

void release(void* ptr) {
	free(ptr);
	ptr = NULL;
}

void ctrlC() {}

int main(int argc, char* argv[]) {
	signal(SIGINT, ctrlC);	/* Sets the behavior for a Control Character,
	                           specifically Ctrl-C */
	//====================================================================================================
	// Alias Initialization
	int a = 0;
	CVector lines = readFile(".", "/.tsh_alias"); /* (As of now) the file must be in
	                                                 the same directory as the executable */
	CVector keys = cv_init(lines.size); // Initializes an Array of Keys
	CHashTable aliases = cht_init(lines.size); // Initializes a Hash Table of Command Aliases
	while (a < lines.size) {
		char* key = 
			substr(
				get(&lines, a).String, 0,
				indexOf(get(&lines, a).String, SPACE)
			);
		char* alias = 
			substr(
				get(&lines, a).String,
				indexOf(get(&lines, a).String, '\'')+1,
				strlen(get(&lines, a).String)-1
			);
		set(&keys, a, (GenericType) key);
		map(&aliases, key, alias);
		release(alias); // Deletes Alias buffer
		release(get(&lines, a).String); // Deletes Line buffer
		a++;
	}
	release(lines.array); // Deletes the Array of Line buffers
	//====================================================================================================
	while (true) {
		//================================================================================================
		// Print Prompt and Current (Relative) Directory
		char* relativeDir = currentDir();
		/* Block 1
		Here */if (strlen(relativeDir) > 0) printf("T-Shell: %s)> ", relativeDir);
		else printf("T-Shell: /)> ");
		release(relativeDir);
		//================================================================================================
		char input[BUFSIZ] = ""; // Input buffer
		fgets(input, BUFSIZ, stdin);
		sscanf(input, "%[^\n]%*c", input); // Discards newline
		if (input[0] != '\n') {
			if (!strcmp(input, "exit") || !strcmp(input, "logout")) break;
			else if (!strcmp(input, "clear")) {
				printf("%c[2J%c[1;1H", 27, 27); /* Clears the Screen and
				                                   resets Cursor position */
				rewind(stdout); /* Sets the stream position indicator
				                   to the beginning of the file */
				ftruncate(1,0); // Truncates the given file (from FD) to a given size
			} else {
		 		CVector tokens = split(input, " "); // User input tokens
				int lcount = 0;	// Iteration Counter
				int i = 0; // Index Counter
				char argBuff[BUFSIZ];
				CVector args;
				while (lcount < keys.size) {
					if (i >= keys.size) i = 0;
					if (!strcmp(get(&tokens, 0).String, get(&keys, i).String)) {
						strcpy(argBuff, lookUp(&aliases, get(&tokens, 0).String).String);
						args = split(argBuff, " ");
						int j = args.size-1;
						while (0 < j) {
							add(&tokens, 1, get(&args, j));
							j--;
						}
						release(args.array); /* Deletes the Alias line buffer if the input
						                        command did not match the current key */
						break;
					}
					i++;
					lcount++;
				}
				if (!strcmp(get(&tokens, 0).String, "cd")) {
					if (tokens.size == 2)
						chdir(get(&tokens, 1).String); /* Changes the Current Directory
						                                  to the given directory */
					else if (tokens.size == 1)
						chdir(getenv("HOME")); /* Changes the Current Directory
						                          to the user's home directory */
					else printf("cd: too many arguments.\n");
				}
				else {
					char* dirs[] = {"/bin/", "/sbin/", "/usr/bin/", "/usr/sbin/"}; // Binary Locations
					char* path = "";
					int found = false;
					int i = 0;
					while (i < 4) {
						//================================================================================
						// Searches for the program
						DIR* binaryDir = opendir(dirs[i]);
						if (binaryDir != NULL) {
							struct dirent *entry = readdir(binaryDir);
							while (entry != NULL) {
								if (!strcmp((*entry).d_name, get(&tokens, 0).String)) {
									path = dirs[i];
									found = true;
									break;
								}
								entry = readdir(binaryDir);
							}
						} else printf("Failed to open directory.\n");
						release(binaryDir); // Deletes the Directory buffer
						i++;
						//================================================================================
					}
					if (found == true) {
						//================================================================================
						// Sets up argv for the external program
						char* extArgv[tokens.size];
						int j = 0;
						while (j < tokens.size) {
							extArgv[j] = get(&tokens, j).String;
							j++;
						}
						extArgv[j] = NULL;
						//================================================================================
						char binPath[strlen(path)+strlen(extArgv[0])];	// Path for the external program
						strcpy(binPath, path);
						strcat(binPath, extArgv[0]);
						extProg(binPath, extArgv);	// Executes the external program
					}
					else printf("tsh: \'%s\' is not a recognized command...\n", input);
				}
				release(tokens.array); // Deletes the input tokens
			}
		}
	}
	//====================================================================================================
	// Alias Freeing
	a = 0;
	while (a < keys.size) {
		unmap(&aliases, get(&keys, a).String); // Deletes a Bucket
		release(get(&keys, a).String); // Deletes a Key
		a++;
	}
	release(keys.array); // Deletes the Array of Keys
	release(aliases.table); // Deletes the Hash Table of Command Aliases
	//====================================================================================================
	return 0;
}