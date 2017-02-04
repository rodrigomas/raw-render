#include "logmanager.h"

static const char LOG_VERSION[] = "0.1.1.2";

//extern LogManager _MainLogManager = NULL;
static LogManager _MainLogManager;

static char* logWriteDateTime(const char* Format);

static LogManager *logGetInstance()
{	
	return &_MainLogManager;
}

char* logWriteDateTime(const char* Format)
{
	static char Tmp[_MAX_TIME_];
	//  Ponteiro para estrutura de Tempo e Data
	struct tm *ptr;
	//  Tempo e Data Locais
	time_t lt;
	//  Obtendo o Tempo e Data do Sistema
	lt = time(NULL);
	//  Colocando os Dados na estrutura
	ptr = localtime(&lt);
	//  Escrevendo a string
	strftime(Tmp, _MAX_TIME_, Format, ptr);

	return Tmp;

}

void logForceWrite(void)
{
	if (!logGetInstance()->File)
		return;

	fflush(logGetInstance()->File);
}

void logInitialize(const char* FileName)
{

	_MainLogManager.t0 = time(NULL);
	_MainLogManager.File = NULL;

	fopen_s(&logGetInstance()->File, FileName, "r");

	if (logGetInstance()->File == NULL)
	{
		fopen_s(&logGetInstance()->File, FileName, "w");
	}
	else
	{
		fclose(logGetInstance()->File);
		fopen_s(&logGetInstance()->File, FileName, "a");
	}

	//  Falha de Abertura
	if (!logGetInstance()->File) {
		return;
	}

	fprintf_s(logGetInstance()->File, "RawRender Log File - Copyright (c) Rodrigo Marques\n");
	fprintf_s(logGetInstance()->File, "Version: %s\n", LOG_VERSION, 7);

	//  Marcação de data ( Nome ) e hora de criação
	fprintf_s(logGetInstance()->File, "Date: %s\n", logWriteDateTime(LOG_DATE), _MAX_TIME_);
	fprintf_s(logGetInstance()->File, "Time: %s\n\n", logWriteDateTime(LOG_TIME), _MAX_TIME_);
}

void logWriteData(const char* Text)
{
	if (Text== NULL || strcmp(Text,"") == 0 || logGetInstance()->File == NULL)
		return;

	//  Grava os dados e Marcação de hora
	fprintf(logGetInstance()->File, "[%s] : %s\n", logWriteDateTime(LOG_TIME), Text);
}

void logFinish()
{
	if (logGetInstance()->File != NULL)
		fclose(logGetInstance()->File);
}