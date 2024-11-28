#ifndef _DBG_VARFN_H_ 
#define _DBG_VARFN_H_ 1

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void SET_DEBUG(int);
// void SET_DEBUG_STREAM(FILE *);
void CLOSE_DEBUG_FILE();
void OPEN_DEBUG_FILE();
int IS_DEBUG();
void DEBUG_PRINT(const char *, const char *, ...);

#endif
