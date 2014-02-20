// Standard: gnu99

#ifndef STACK_H
#define STACK_H

#include "data-structs/gentype.h"

// Represents a single node in the stack
typedef struct node {
	GenType data;      // Data contained in the Node
	struct node* next; // Pointer to next Node (NULL if none)
} Node;

extern void stack_pop(Node** stack);
extern void stack_push(Node** stack, GenType data);
extern Node* stack_top(Node* stack);
extern void stack_empty(Node* stack);

#endif