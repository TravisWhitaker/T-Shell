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
 
char* currentDir(void) {
	char absoluteBuffer[BUFSIZ] = "";
	getcwd(absoluteBuffer, BUFSIZ);
	char* relativePath = strrchr(absoluteBuffer, '/');
	char* relativeBuffer = calloc(strlen(relativePath), sizeof(char));
	int i = 0;
	while (i < (strlen(relativePath)-1)) {
		relativeBuffer[i] = relativePath[i+1];
		i++;
	}
	relativeBuffer[i] = '\0';
	return relativeBuffer;
}

void extProg(const char* extBin, char** extArgv) {
	int childExitStatus;
	pid_t childPID = fork();
	if (childPID >= 0) {
		if (childPID == 0) execvp(extBin, extArgv);
		else wait(&childExitStatus);
	} else perror("fork");
}

CVector readFile(char* fileDir, char* fileName) {
	CVector contents = cv_init(0);
	char filePath[11];
	strcpy(filePath, fileDir);
	strcat(filePath, fileName);
	FILE* file = fopen(filePath, "r");
	if (file == NULL)
		printf("tsh: File not found or failed to open.\n");
	else {
		char line[BUFSIZ];
		int i = 0;
		while (fgets(line, BUFSIZ, file) != NULL)
			if (strchr(line, '#') == NULL && strlen(line) != 1) {
				char* modLine = calloc(strlen(line), sizeof(char));
				memcpy(modLine, line, strlen(line)-1);
				modLine[strlen(line)-1] = '\0';
				add(&contents, i, (GenericType) modLine);
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
	//======================================================================
	// Alias Initialization
	int a = 0;
	CVector lines = readFile(".", "/.tsh_alias"); // (As of now) the file must be in the same directory as the executable
	CVector keys = cv_init(lines.size);			// Initializes an Array of Keys
	CHashTable aliases = cht_init(lines.size);	// Initializes a Hash Table of Command Aliases
	while (a < lines.size) {
		char* key = 
			substr(
				get(&lines, a).String, 0,
				indexOf(get(&lines, a).String, 32)
			);
		char* alias = 
			substr(
				get(&lines, a).String,
				indexOf(get(&lines, a).String, '\'')+1,
				strlen(get(&lines, a).String)-1
			);
		set(&keys, a, (GenericType) key);
		map(&aliases, key, alias);
		release(alias);					// Deletes Alias buffer
		release(get(&lines, a).String);	// Deletes Line buffer
		a++;
	}
	release(lines.array); // Deletes the Array of Line buffers
	//======================================================================
	while (true) {
		//======================================================================
		// Print Prompt and Current (Relative) Working Directory
		char* relativeDir = currentDir();
		/* Block 1
		Here */if (strlen(relativeDir) > 0) printf("T-Shell: %s)> ", relativeDir);
		else printf("T-Shell: /)> ");
		release(relativeDir);
		//======================================================================
		char input[BUFSIZ] = "";
		fgets(input, BUFSIZ, stdin);
		sscanf(input, "%[^\n]%*c", input); // Discards newline
		if (input[0] != '\n') {
			if (!strcmp(input, "exit") || !strcmp(input, "logout")) break;
			else if (!strcmp(input, "clear")) {
				printf("%c[2J%c[1;1H", 27, 27); /* Clears the Screen and
												   resets Cursor position */
				rewind(stdout); /* Sets the stream position indicator
								   to the beginning of the file */
				ftruncate(1,0); // Truncates given file (from FD) to a given size
			} else {
		 		CVector separation = split(input, " ");
				int lcount = 0;	// Iteration Counter
				int i = 0;		// Index Counter
				char argBuff[BUFSIZ];
				CVector args;
				while (lcount < keys.size) {
					if (i >= keys.size) i = 0;
					if (!strcmp(get(&separation, 0).String, get(&keys, i).String)) {
						strcpy(argBuff, lookUp(&aliases, get(&separation, 0).String).String);
						args = split(argBuff, " ");
						int j = args.size-1;
						while (0 < j) {
							add(&separation, 1, get(&args, j));
							j--;
						}
						release(args.array); /* Deletes the Alias line buffer if the input
												command did not match the current key */
						break;
					}
					i++;
					lcount++;
				}
				if (!strcmp(get(&separation, 0).String, "cd")) {
					if (separation.size == 2)
						chdir(get(&separation, 1).String);	// Changes the Current Working Directory to the given directory
					else if (separation.size == 1)
						chdir(getenv("HOME"));	// Changes the Current Working Directory to the user's home directory
					else printf("cd: too many arguments.\n");
				}
				else {
					char* dirs[] = {"/bin/", "/sbin/", "/usr/bin/", "/usr/sbin/"}; // Binary Locations
					char* path = "";
					int found = false;
					int i = 0;
					while (i < 4) {
						//======================================================================
						// Searches for the program
						DIR* binaryDir = opendir(dirs[i]);
						if (binaryDir != NULL) {
							struct dirent *entry = readdir(binaryDir);
							while (entry != NULL) {
								if (!strcmp((*entry).d_name, get(&separation, 0).String)) {
									path = dirs[i];
									found = true;
									break;
								}
								entry = readdir(binaryDir);
							}
						} else printf("Failed to open directory.\n");
						release(binaryDir); // Deletes the Directory buffer
						i++;
						//======================================================================
					}
					if (found == true) {
						//==================================================
						// Sets up argv for the external program
		 				char* extArgv[separation.size];
		 				int j = 0;
		 				while (j < separation.size) {
		 					extArgv[j] = get(&separation, j).String;
		 					j++;
		 				}
		 				extArgv[j] = NULL;
						//==================================================
						char binPath[strlen(path)+strlen(extArgv[0])];	// The Path for the external program
						strcpy(binPath, path);
						strcat(binPath, extArgv[0]);
		 				extProg(binPath, extArgv);	// Executes the external program
					}
					else printf("tsh: \'%s\' is not a recognized command...\n", input);
				}
				release(separation.array); // Deletes the user input buffer
			}
		}
	}
	//======================================================================
	// Alias Freeing
	a = 0;
	while (a < keys.size) {
		unmap(&aliases, get(&keys, a).String); 	// Deletes a Bucket
		release(get(&keys, a).String); 			// Deletes a Key
		a++;
	}
	release(keys.array);	// Deletes the Array of Keys
	release(aliases.table); // Deletes the Hash Table of Command Aliases
	//======================================================================
	return 0;
}