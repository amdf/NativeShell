
#ifndef NATIVEFILE_FUNCTIONS_H
#define NATIVEFILE_FUNCTIONS_H 1

#include <ntndk.h>

BOOLEAN NtFileOpenFile(HANDLE* phRetFile, WCHAR* pwszFileName, BOOLEAN bWrite, BOOLEAN bOverwrite);
BOOLEAN NtFileOpenDirectory(HANDLE* phRetFile, WCHAR* pwszFileName, BOOLEAN bWrite, BOOLEAN bOverwrite);

BOOLEAN NtFileReadFile(HANDLE hFile, LPVOID pOutBuffer, DWORD dwOutBufferSize, DWORD* pRetReadSize);
BOOLEAN NtFileWriteFile(HANDLE hFile, LPVOID lpData, DWORD dwBufferSize, DWORD* pRetWrittenSize);

BOOLEAN NtFileSeekFile(HANDLE hFile, LONGLONG lAmount);
BOOLEAN NtFileGetFilePosition(HANDLE hFile, LONGLONG* pRetCurrentPosition);
BOOLEAN NtFileGetFileSize(HANDLE hFile, LONGLONG* pRetFileSize);

BOOLEAN NtFileCloseFile(HANDLE hFile);

BOOLEAN NtFileCopyFile(WCHAR* pszSrc, WCHAR* pszDst);

BOOLEAN NtFileDeleteFile(PCWSTR filename);
BOOLEAN NtFileCreateDirectory(PCWSTR dirname);

BOOLEAN NtFileMoveFile(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, BOOLEAN ReplaceIfExists);

#endif
