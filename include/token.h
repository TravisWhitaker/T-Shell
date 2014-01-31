// Standard: gnu99

#ifndef TOKEN_H
#define TOKEN_H

#include "data-structs/gentype.h";

typedef enum tokentype {
	/**************************************************/
	// Boolean Operators.
	OR,            // |
	AND,           // &
	NOT,           // !
	EQUALS,        // ==
	LESS_THAN,     // <
	GREATER_THAN,  // >
	LESS_EQUAL,    // <=
	GREATER_EQUAL, // >=
	/**************************************************/
	// Mathematical Operators.
	ADD,      // +
	SUBTRACT, // -
	MULTIPLY, // *
	DIVIDE,   // /
	MODULO,   // %
	ASSIGN,   // =    *Not just for math*
	/**************************************************/
	// Literals and Symbols.
	NUMBER,  // Constant Number literal. [0-9]+
	STRING,  // Constant String literal. "[a-zA-Z0-9]*"
	SYMBOL,  // Variable, Function.
	UNKNOWN, // 
	/**************************************************/
} TokenType;

typedef struct token {
	char* symbol;   // The identifier of the token.
	TokenType type; // The type of the token.
	GenType value;  // The value stored at the token.
	// Left?
	// Right?
} Token;

#endif

/*
RESERVED // Language keyword.
         //    import?
         //    routine
         //    return
         //    if
         //    else
         //    for
         //    while
         //    break
*/
