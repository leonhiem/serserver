#ifndef PARSE_H
#define PARSE_H 1

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdint.h>

class Parse {
public:
Parse();
~Parse() {};

int strtoint(const char* in);
void scanvector(uint8_t *vec, const char* vector, int len);
void scanvector(uint8_t phases[], const char* vector, int max=15, int len=72);
int GetStr(const char* buf, const char* &s);
int GetInt(const char* buf, int &i);
int GetDouble(const char* buf, double &d);
};

#endif

