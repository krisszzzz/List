#ifndef LOG_INCLUDED
#define LOG_INCLUDED
#include <cstdio>
#include <stdarg.h>

#define __PRINT_ALL_INFO__(...) 																                \
PrintToLog("Error occured in file: %s, function: %s, line: %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);     \
fprintf(GetCurrentLogFile(), __VA_ARGS__);                                                      


void SetLogFile(FILE* log_file = nullptr);

void ResetLogFile();

void ResetAllLogFiles();

int PrintToLog(const char* format, ...);

FILE* GetCurrentLogFile(); 


#endif
