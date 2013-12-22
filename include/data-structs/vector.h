#ifndef VECTOR_H_
#define VECTOR_H_

#include "data-structs/gentype.h"

typedef struct Vector {
	int size;
	GenType* array;
} Vector;

Vector vect_init(int size);
void empty(Vector* listPtr);
GenType get(Vector* listPtr, int index);
void set(Vector* listPtr, int index, GenType value);
void add(Vector* listPtr, int index, GenType value);
void delete(Vector* listPtr, int index);

#endif
