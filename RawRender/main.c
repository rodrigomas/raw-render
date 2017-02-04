#include "mainui.h"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	char **argv;
	int argc;

	argv = (char**)CommandLineToArgvW(GetCommandLineW(), &argc);

	char strPathName[_MAX_PATH];
	GetModuleFileName(NULL, strPathName, _MAX_PATH);

	for (register int i = strlen(strPathName) - 1; i >= 0; i--)
	{
		if (strPathName[i] == '\\')
		{
			strPathName[i + 1] = '\0';
			break;
		}
	}

	SetCurrentDirectory(strPathName);

#else
int main(int argc, char **argv)
{
#endif

	char* scenefile = NULL;
	char* outputfile = NULL;
	BOOL verbose = FALSE;
	BOOL console = FALSE;

	logInitialize("log.txt");

	for (register int i = 1; i < argc; i++)
	{
		if (strncmp("-s", argv[i], 2) == 0)
		{
			scenefile = argv[i + 1];
			i++;
		}
		else if (strncmp("-o", argv[i], 2) == 0)
		{
			outputfile = argv[i + 1];
			i++;

		}
		else if (strncmp("-verbose", argv[i], 8) == 0)
		{
			verbose = TRUE;
		}
		else if (strncmp("-console", argv[i], 8) == 0)
		{
			console = TRUE;
		}
	}

	freopen("bug.txt", "w", stdout);

	muiInitMainUI(argc, argv, !console);

#ifdef _WIN32
	LocalFree(argv);
#endif	

	logWriteData("Stared");

	muiEngineRender(scenefile, outputfile);

	muiStart();	

	logWriteData("Ended");
	logForceWrite();
	logFinish();

	fclose(stdout);

	muiClose();

	return(0);
}