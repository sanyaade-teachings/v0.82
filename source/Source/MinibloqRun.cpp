#include <Windows.h>


//Converts a c-string (NULL terminated) to a WCHAR:
LPWSTR cstrToWChar(LPCSTR value)
{
    LPWSTR result = NULL;
    int valueLen = lstrlenA(value);
    int resultLen = MultiByteToWideChar(CP_ACP, 0, value, valueLen, 0, 0);
    if (resultLen > 0)
    {
        result = SysAllocStringLen(0, resultLen);
        MultiByteToWideChar(CP_ACP, 0, value, valueLen, result, resultLen);
    }
    return result;
}


int main()
{
    //This is not portable wxWidgets code, because this program has to be
    //alone (without the wxWidgets DLLs). It's main function is to run
    //Minibloq, setting the XDFRunTime as the working directory:
	STARTUPINFO startInfo;
	PROCESS_INFORMATION processInformation;

    //Compile error (why?):
//	SecureZeroMemory(&processInformation, sizeof(processInformation));
//	SecureZeroMemory(&startInfo, sizeof(startInfo));
	ZeroMemory(&processInformation, sizeof(processInformation));
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);

	if (!CreateProcess(NULL,
                       cstrToWChar("Components/Minibloq/v1.0/Minibloq.exe"),
                       NULL,
                       NULL,
                       FALSE,
                       DETACHED_PROCESS,
                       NULL,
                       cstrToWChar("Components/XDFRunTime/Win/v1.0"),
                       &startInfo,
                       &processInformation ))
	{
		return 1;
	}
    return 0;
}
