#ifndef GENTYPE_H_
#define GENTYPE_H_

typedef union gentype {
	char Character;
	short Short;
	int Integer;
	long Long;
	double Double;
	char* String;
	void* Void;
} GenType;

#endif
