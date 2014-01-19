// Standard: gnu99

#include <stdlib.h>

#include "stack.h"

static Node* initNode(GenType data, Node* next) {
	Node* newNode = calloc(1, sizeof(Node));
	newNode->data = data;
	newNode->next = next;
	return newNode;
}

void stack_pop(Node** stack) {
	if ((*stack)->next == NULL) {
		free((*stack));
		(*stack) = NULL;
	} else stack_pop(&(*stack)->next);
}

void stack_push(Node** stack, GenType data) {
	if ((*stack) == NULL) (*stack) = initNode(data, NULL);
	else if ((*stack)->data.Void == NULL) {
		(*stack)->data = data;
	} else stack_push(&(*stack)->next, data);
}

Node* stack_top(Node* stack) {
	if (stack->next == NULL) return stack;
	else return stack_top(stack->next);
}

void stack_empty(Node* stack) {
	while (stack->next != NULL) {
		Node* top = stack_top(stack);
		free(top);
	}
	free(stack);
	stack = NULL;
}

#ifdef STACK_DEBUG
#include <stdio.h>

int main(void) {
	Node* stack = initNode((GenType) "Node 1", NULL);
	printf("\n");
	printf("Top: %s\n", stack_top(stack)->data.String);
	stack_push(&stack, (GenType) "Node 2");
	printf("Top: %s\n", stack_top(stack)->data.String);
	stack_pop(&stack);
	printf("Top: %s\n", stack_top(stack)->data.String);
	stack_pop(&stack);
	if (stack == NULL)
		printf("Empty\n");
	printf("\n");
	return 0;
}
#endif
