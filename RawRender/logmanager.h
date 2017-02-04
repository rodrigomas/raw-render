#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <string.h>
#include <time.h>
#include <stdio.h>

#define _MAX_TIME_ 256
#define LOG_DATE "%d/%m/%Y"
#define LOG_TIME "%H:%M:%S"

struct _LogManager
{
	time_t t0;

	FILE *File;		
};

typedef struct _LogManager LogManager;

void logWriteData(const char* Text);

void logInitialize(const char* FileName);

void logForceWrite(void);

void logFinish();


#endif