// Copyright (c) 2005-2015 Ross Smith II. See Mit LICENSE in /LICENSE

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


#define _WIN32_WINNT 0x501

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "UpdatePathVariable.h"
#include "debug.h"
#include "SystemMessage.h"

#define MAX_VALUE_LEN  32767

#define POSTFIXPATHREGKEY	L"Software\\PESetEnv\\Path\\Postfix"
#define ENVREGKEY       L"System\\CurrentControlSet\\Control\\Session Manager\\Environment"

#define APPNAME         L"PESetEnv"

#define MESSAGEBOXW(x)   MessageBoxW(NULL, x, APPNAME, MB_OK|MB_ICONERROR);
#define MESSAGEBOXA(x)   MessageBoxA(NULL, x, APPNAME, MB_OK|MB_ICONERROR);

#define BROADCAST_TIMEOUT (5000)

int UpdatePathVariable(void)
{
    HKEY hKey = NULL;
    PWCHAR szPath = NULL;
    PWCHAR szNewPath = NULL;
    PWCHAR szExpandedPath = NULL;
	DWORD rv = (DWORD) -1;

	D(("UpdatePathVariable() start"));

	for (;;)
	{
		WCHAR szVarName[MAX_VALUE_LEN];
		WCHAR szVarValue[MAX_VALUE_LEN];
		DWORD dwPathLen = 0;
		DWORD dwLen = 0;
		DWORD dwType;
		DWORD dwNewLen = 0;
		DWORD i = 0;

		rv = RegOpenKeyExW(HKEY_LOCAL_MACHINE, POSTFIXPATHREGKEY, 0, KEY_READ | KEY_QUERY_VALUE, &hKey);
		if (rv == ERROR_FILE_NOT_FOUND) {
			return ERROR_SUCCESS;
		}
		if (rv != ERROR_SUCCESS) {
			MESSAGEBOXW(SystemMessageW(rv));
			break;
		}

		for (;;)
		{
			dwLen = MAX_VALUE_LEN;
			rv = RegEnumValueW(hKey, i, szVarName, &dwLen, NULL, NULL, NULL, NULL);
			if (rv == ERROR_NO_MORE_ITEMS)
				break;
			if (rv != ERROR_SUCCESS)
			{
				MESSAGEBOXW(SystemMessageW(rv));
				break;
			}

			D(("dwPathLen=%5d, i=%5d, dwLen=%5d, szVarName=%S", dwPathLen, i, dwLen, szVarName));

			dwLen = MAX_VALUE_LEN;
			rv = RegQueryValueExW(hKey, szVarName, NULL, &dwType, (LPBYTE) szVarValue, &dwLen);
			if (rv == ERROR_NO_MORE_ITEMS)
				break;
			if (rv != ERROR_SUCCESS)
			{
				MESSAGEBOXW(SystemMessageW(rv));
				break;
			}

			D(("szVarValue=%S dwLen=%s", szVarValue, dwLen));

       		++i;

			if (dwLen == 0)
				continue;

			if (dwPathLen)
				++dwPathLen;
       		dwPathLen += dwLen;
		}
		if (rv != ERROR_NO_MORE_ITEMS && rv != ERROR_SUCCESS)
			break;

		D(("dwPathLen=%5d, i=%5d", dwPathLen, i));

		szPath = (PWCHAR) malloc(dwPathLen);
		if (szPath == NULL)
		{
			MESSAGEBOXW(SystemMessageW(rv));
		    break;
		}

		wcscpy(szPath, L"");

		i = 0;
		for (;;)
		{
	        dwLen = MAX_VALUE_LEN;
			rv = RegEnumValueW(hKey, i, szVarName, &dwLen, NULL, NULL, NULL, NULL);
			if (rv == ERROR_NO_MORE_ITEMS)
				break;
			if (rv != ERROR_SUCCESS)
			{
				MESSAGEBOXW(SystemMessageW(rv));
			    break;
			}

			D(("dwPathLen=%5d, i=%5d, szVarName=%S", dwPathLen, i, szVarName));

			dwLen = MAX_VALUE_LEN;
			rv = RegQueryValueExW(hKey, szVarName, NULL, &dwType, (LPBYTE) szVarValue, &dwLen);
			if (rv == ERROR_NO_MORE_ITEMS)
				break;
			if (rv != ERROR_SUCCESS)
			{
				MESSAGEBOXW(SystemMessageW(rv));
			    break;
			}

			D(("dwPathLen=%5d, szVarValue=%S szPath=%S", dwPathLen, szVarValue, szPath));

			++i;

			if (dwLen == 0)
				continue;

			if (wcslen(szPath))
				wcscat(szPath, L";");

			wcscat(szPath, szVarValue);
		}
		if (rv != ERROR_NO_MORE_ITEMS && rv != ERROR_SUCCESS)
			break;

		RegCloseKey(hKey);
		hKey = NULL;

		if (dwPathLen == 0)
		{
			// no paths need to be added
			rv = ERROR_SUCCESS;
			break;
		}

		rv = RegOpenKeyExW(HKEY_LOCAL_MACHINE, ENVREGKEY, 0, KEY_READ | KEY_WRITE | KEY_QUERY_VALUE, &hKey);
		if (rv != ERROR_SUCCESS)
		{
			MESSAGEBOXW(SystemMessageW(rv));
			break;
		}

		dwLen = MAX_VALUE_LEN;
		dwType = REG_SZ;
		rv = RegQueryValueExW(hKey, L"Path", NULL, &dwType, (LPBYTE) szVarValue, &dwLen);
		if (rv != ERROR_SUCCESS) {
			MESSAGEBOXW(SystemMessageW(rv));
			break;
		}

		dwNewLen = dwPathLen + dwLen + 4;
		D(("dwNewLen=%d", dwNewLen));

		szNewPath = (PWCHAR) malloc(dwNewLen + 4);
		if (szNewPath == NULL)
		{
			MESSAGEBOXW(SystemMessageW(GetLastError()));
		    break;
		}

		wcscpy(szNewPath, L"");

		if (dwLen)
		{
			wcscpy(szNewPath, szVarValue);
			wcscat(szNewPath, L";");
		}

		D(("szNewPath=%S", szNewPath));

		wcscat(szNewPath, szPath);

		D(("szNewPath=%S", szNewPath));

		szExpandedPath = (PWCHAR) malloc(32767);
		if (szExpandedPath == NULL)
		{
			MESSAGEBOXW(SystemMessageW(GetLastError()));
		    break;
		}
		if (!ExpandEnvironmentStringsW(szNewPath, szExpandedPath, 32767)) {
			MESSAGEBOXW(SystemMessageW(GetLastError()));
		    return GetLastError();
		}

		// todo only update path if running in PE
		if (dwType == REG_EXPAND_SZ)
		{
			rv = RegSetValueExW(hKey, L"Path", 0, dwType, (LPBYTE) szNewPath, wcslen(szNewPath) * sizeof(WCHAR));
		} else {
			rv = RegSetValueExW(hKey, L"Path", 0, dwType, (LPBYTE) szExpandedPath, wcslen(szExpandedPath) * sizeof(WCHAR));
		}

		if (rv != ERROR_SUCCESS)
		{
			MESSAGEBOXW(SystemMessageW(rv));
			break;
		}

		if (!SetEnvironmentVariableW(L"Path", szExpandedPath))
		{
			MESSAGEBOXW(SystemMessageW(GetLastError()));
			break;
		}

		static DWORD dummy;
		if (!SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment", SMTO_ABORTIFHUNG, BROADCAST_TIMEOUT, &dummy))
		{
			MESSAGEBOXW(SystemMessageW(GetLastError()));
			break;
		}

		rv = ERROR_SUCCESS;
		break;
	}

	if (szPath)
   		free(szPath);

	if (szNewPath)
   		free(szNewPath);

	if (szExpandedPath)
   		free(szExpandedPath);

	if (hKey)
		RegCloseKey(hKey);

	D(("UpdatePathVariable() returning %d", rv));

    return rv;
}