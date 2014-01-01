#ifndef VECTOR_H
#define VECTOR_H

#include "data-structs/gentype.h"

typedef struct Vector {
	int size;
	GenType* array;
} Vector;

extern Vector vector_init(int size);
extern void vector_empty(Vector* listPtr);
extern GenType vector_get(Vector* listPtr, int index);
extern void vector_set(Vector* listPtr, int index, GenType value);
extern void vector_add(Vector* listPtr, int index, GenType value);
extern void vector_delete(Vector* listPtr, int index);

#endif
