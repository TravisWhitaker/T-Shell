#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdbool.h>

typedef struct config {
	bool colors;    	// Should the prompt be colored
	char prompt[32];	// Prompt format
} Configuration;

extern Configuration config_read(void);
extern char* config_build_prompt(Configuration* config);

#endif
