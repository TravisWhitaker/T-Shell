#ifndef VECTOR_H_
#define VECTOR_H_

#include "data-structs/gentype.h"

typedef struct Vector {
	int size;
	GenType* array;
} Vector;

extern Vector vect_init(int size);
extern void empty(Vector* listPtr);
extern GenType get(Vector* listPtr, int index);
extern void set(Vector* listPtr, int index, GenType value);
extern void add(Vector* listPtr, int index, GenType value);
extern void delete(Vector* listPtr, int index);

#endif
