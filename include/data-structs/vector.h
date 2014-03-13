#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector {
	unsigned int size;
	void** array;
} Vector;

extern Vector vector_init(int size);
extern void vector_empty(Vector* listPtr);
extern void* vector_get(Vector* listPtr, int index);
extern void vector_set(Vector* listPtr, int index, void* value);
extern void vector_add(Vector* listPtr, unsigned int index, void* value);
extern void vector_delete(Vector* listPtr, unsigned int index);
extern Vector vector_split(char* string, const char* token);

#endif
