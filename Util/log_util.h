#ifndef _LOG_UTL_H_ 
#define _LOG_UTL_H_ 1

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// void SET_LOG_STREAM(FILE *);
// void SET_LOG_ERR_STREAM(FILE *);
void CHECK_RENAME_LOG_FILES();

void OPEN_LOG_FILE();
void OPEN_ERR_FILE();

void CLOSE_LOG_FILE();
void CLOSE_ERR_FILE();

void LOG_MSG(const char *, const char *, ...);
void LOG_ERR(const char *, const char *, ...);

#endif
