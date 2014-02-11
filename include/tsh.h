#ifndef TSH_H
#define TSH_H

#define ASCII_BACKSPACE 8			// ASCII value for the Backspace character.
#define ASCII_ESCAPE	27			// ASCII value for the Escape character.
#define ASCII_SPACE		32 			// ASCII value for the Space character.
#define ASCII_NULL		0			// ASCII value for Null, marks the end of a string.
#define COLOR_WHITE		"\x1b[37m"	// Colors the following text White.
#define COLOR_CYAN		"\x1b[36m"	// Colors trailing text Cyan.
#define COLOR_MAGENTA	"\x1b[35m"	// Colors trailing text Magenta.
#define COLOR_BLUE		"\x1b[34m"	// Colors trailing text Blue.
#define COLOR_YELLOW	"\x1b[33m"	// Colors trailing text Yellow.
#define COLOR_GREEN		"\x1b[32m"	// Colors trailing text Green.
#define COLOR_RED		"\x1b[31m"	// Colors trailing text Red.
#define COLOR_RESET		"\x1b[0m"	// Resets all Terminal SGR parameters.
#define BUFFER_SIZE		BUFSIZ/32	// New buffer size.

extern char* construct_path(char* filename, size_t length);

/*
 * Frees the memory pointed to by 'ptr'
 * Argument(s):
 *   void* ptr: pointer to ANY allocated memory
 */
static inline void release(void* ptr) {
	free(ptr);
	ptr = NULL;
}

#endif
