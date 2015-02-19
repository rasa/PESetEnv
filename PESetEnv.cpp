// Copyright (c) 2005-2015 Ross Smith II. See Mit LICENSE in /LICENSE

#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#endif

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif

#define _WIN32_WINNT 0x501

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
//#include <atlbase.h> // required for Stackwalker
#include <shellapi.h>
#include <tchar.h>

#include "Stackwalker.h"
#include "Optimize.h"
#include "SystemMessage.h"
#include "debug.h"
#include "UpdatePathVariable.h"
#include "getopt.h"

#include "version.h"

#define APPNAME			L"PESetEnv"
#define APPVERSION		VER_STRING2
#define APPCOPYRIGHT	VER_LEGAL_COPYRIGHT

#define MESSAGEBOXW(x)   MessageBoxW(NULL, x, APPNAME, MB_OK|MB_ICONERROR);
#define MESSAGEBOXA(x)   MessageBoxA(NULL, x, APPNAME, MB_OK|MB_ICONERROR);

#define DEFAULT_DESKTOP "winsta0\\default"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	D(("PESetEnv start"));

#ifdef _DEBUG_ALLOC
//    InitAllocCheck();
#endif

	int len = wcslen (GetCommandLineW()) + 1;
	LPSTR arg = (LPSTR) malloc(len * sizeof(char));
	int argc;
	LPWSTR *argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
	LPSTR *argv = (LPSTR *) malloc(argc * sizeof(char *));
	for (int i = 0; i < argc; ++i)
	{
		int len2 = WideCharToMultiByte(CP_ACP, 0, argvw[i], -1, arg, len, NULL, NULL);
		argv[i] = (LPSTR) malloc((len2 + 1) * sizeof(char));
		strcpy(argv[i], arg);
	}
	free (arg);

	opterr = 0;
	PTCHAR pszStartingDirectory = NULL;
	int c;
	int len3;
	while ((c = getopt(argc, argv, _T("vd:"))) != EOF)
    {
        switch (c)
        {
            case _T('d'):
				if (pszStartingDirectory)
					free(pszStartingDirectory);
				len3 = ExpandEnvironmentStrings(optarg, NULL, 0) + 1;
				pszStartingDirectory = (PTCHAR) malloc(len3 * sizeof(char));
				if (!ExpandEnvironmentStrings(optarg, pszStartingDirectory, len3))
				{
					MESSAGEBOXW(SystemMessageW(GetLastError()));
			        return GetLastError();
				}
                break;
            case _T('v'):
				MESSAGEBOXW(L"Copyright (c) 2005, Ross Smith. All Rights Reserved");
                break;
			default:
				// silently ignore invalid options
				//MESSAGEBOXW(L"Invalid Option");
				break;
        }
    }

	LPSTR szCmdLineTemp = (LPSTR) malloc(len * sizeof(char *));
	strcpy (szCmdLineTemp, "");

	for (; optind < argc; ++optind)
	{
		strcat(szCmdLineTemp, argv[optind]);
		if (optind < argc - 1)
			strcat(szCmdLineTemp, " ");
	}

	for (int i = 0; i < argc; ++i)
	{
		free(argv[i]);
	}

	if (strlen(szCmdLineTemp) == 0)
	{
		free(szCmdLineTemp);
		if (pszStartingDirectory)
			free(pszStartingDirectory);
		return -2;
	}

    int len4 = ExpandEnvironmentStrings(szCmdLineTemp, NULL, 0) + 1;
	LPSTR szCmdLine = (LPSTR) malloc(len4 * sizeof(char *));

    if (!ExpandEnvironmentStrings(szCmdLineTemp, szCmdLine, len4))
	{
		MESSAGEBOXW(SystemMessageW(GetLastError()));
		free(szCmdLine);
		free(szCmdLineTemp);
		if (pszStartingDirectory)
			free(pszStartingDirectory);
        return GetLastError();
	}

	free(szCmdLineTemp);

	int r = UpdatePathVariable();
	if (r != ERROR_SUCCESS)
	{
		// ignore the error
		// free(szCmdLine);
		// return r;
	}

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = DEFAULT_DESKTOP;

    DWORD rv = CreateProcess(NULL,             // No module name (use command line).
                      szCmdLine,        // Command line.
                      NULL,             // Process handle not inheritable.
                      NULL,             // Thread handle not inheritable.
                      FALSE,            // Set handle inheritance to FALSE.
                      0,                // No creation flags.
                      NULL,             // Use parent's environment block.
                      pszStartingDirectory,	// starting directory.
                      &si,              // Pointer to STARTUPINFO structure.
                      &pi);             // Pointer to PROCESS_INFORMATION structure.

	free (szCmdLine);
	if (pszStartingDirectory)
		free(pszStartingDirectory);

	if (!rv)
	{
		MESSAGEBOXW(SystemMessageW(GetLastError()));
        return GetLastError();
	}

    DWORD exitcode = (DWORD) -1;

	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &exitcode);
    CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	D(("PESetEnv returning %d", exitcode));

    return exitcode;
}
