#ifndef TSH_H
#define TSH_H

#define ASCII_SPACE 32 			// The ASCII value for the Space character.
#define ASCII_NULL 0			// Null terminator, marks end of a string.
#define COLOR_RED	"\x1b[31m"
#define COLOR_RESET "\x1b[0m"
#define BUFFER_SIZE BUFSIZ/32	// New buffer size.

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
