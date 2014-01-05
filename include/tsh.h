#ifndef TSH_H
#define TSH_H

#define BLANK_SPACE 32 // The ASCII value for the Space character.
#define BUFFER_SIZE BUFSIZ/32 // New buffer size.
#define STRING_END '\0' // Null terminator, marks end of a string.

/*
 * Frees the memory pointed to by 'ptr'
 * Argument(s):
 *   void* ptr, pointer to ANY allocated memory
 */
static inline void release(void* ptr) {
	free(ptr);
	ptr = NULL;
}

#endif