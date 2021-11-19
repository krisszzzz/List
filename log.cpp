
//#include<stdio.h>
//#include<stdarg.h>
#include "log.h"
#include <assert.h>

const int    MAX_LOG_AMOUNT             = 3;

static int   NUM_OF_LOG_FILES           = 0;
static FILE* LOG_FILES[MAX_LOG_AMOUNT]  = { stderr, stderr, stderr };

void SetLogFile(FILE* log_file)
{
    if(log_file != nullptr && NUM_OF_LOG_FILES < MAX_LOG_AMOUNT) {
        LOG_FILES[NUM_OF_LOG_FILES++] = log_file;
    }

     if(NUM_OF_LOG_FILES > MAX_LOG_AMOUNT) {
        fprintf(LOG_FILES[MAX_LOG_AMOUNT - 1], "WARNING: You write RememberLogFile() too much - you can use independetly only %d log files\n",
                MAX_LOG_AMOUNT);
        NUM_OF_LOG_FILES = MAX_LOG_AMOUNT - 1;
     }
}

void ResetLogFile()
{
    if(NUM_OF_LOG_FILES <= 0) {
        printf("You use ResetLogFile(), but don't use you SetLogFile(), or probably you use ResetLogFile() twice or more.\n");
        assert(!"Error");
    }

    if(NUM_OF_LOG_FILES > MAX_LOG_AMOUNT) {
        printf("Incorrect value of NUM_OF_LOG_FILES --- is bigger than MAX_LOG_AMOUNT (NUM_OF_LOG_FILES = %d)\n",
                NUM_OF_LOG_FILES);
        assert(!"Error");
    }

    LOG_FILES[--NUM_OF_LOG_FILES] = stderr;
}

void ResetAllLogFiles()
{
    for(size_t log_file_count = 0; log_file_count < MAX_LOG_AMOUNT; ++log_file_count)
    {
        if(LOG_FILES[log_file_count] != stderr) {
            fclose(LOG_FILES[log_file_count]);
        }
    }
}

int PrintToLog(const char* format, ...)
{
    
    if(NUM_OF_LOG_FILES > MAX_LOG_AMOUNT) {
        printf("Inccorect value of NUM_OF_LOG_FILES --> %d\n", NUM_OF_LOG_FILES);
        assert(!"Error");
    }
    va_list arg_ptr;
    va_start(arg_ptr, format);
    int num_of_char_printed = 0;
    if(NUM_OF_LOG_FILES == 0) {
        num_of_char_printed = vfprintf(LOG_FILES[0], format, arg_ptr);    
    } else {
        num_of_char_printed = vfprintf(LOG_FILES[NUM_OF_LOG_FILES - 1], format, arg_ptr);
    }
    
    va_end(arg_ptr);
    return num_of_char_printed;
}

FILE* GetCurrentLogFile()
{
    return (NUM_OF_LOG_FILES == 0) ? LOG_FILES[0] : LOG_FILES[NUM_OF_LOG_FILES - 1];
}