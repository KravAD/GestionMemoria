#include <iostream>
#include <windows.h>
#include <tchar.h>

#define SIZE 4096
#define BUFFER_SIZE 256

int main() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            SIZE,
            _T("SharedMemory"));

    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object (" << GetLastError() << ")." << std::endl;
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(hMapFile,
                                 FILE_MAP_ALL_ACCESS,
                                 0,
                                 0,
                                 SIZE);

    if (pBuf == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ")." << std::endl;

        CloseHandle(hMapFile);
        return 1;
    }

    _stprintf_s((TCHAR*)pBuf, BUFFER_SIZE, _T("Hello, child process!"));

    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL,
                       _T("ChildProcess.exe"),
                       NULL,
                       NULL,
                       FALSE,
                       0,
                       NULL,
                       NULL,
                       &si,
                       &pi)) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ")." << std::endl;
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    _tprintf(_T("Child reads: %s\n"), pBuf);

    UnmapViewOfFile(pBuf);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    CloseHandle(hMapFile);

    return 0;
}