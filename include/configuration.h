#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdbool.h>

typedef struct config {
	bool colors;
	char prompt[32];	// Prompt format
} Configuration;

extern Configuration config_read(void);

#endif
