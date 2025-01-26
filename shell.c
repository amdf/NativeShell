/**
 * PROJECT:         Native Shell
 * COPYRIGHT:       LGPL; See LICENSE in the top level directory
 * FILE:            shell.c
 * DESCRIPTION:     Shell helper functions.
 * DEVELOPERS:      See CONTRIBUTORS.md in the top level directory
 */

#include "precomp.h"

/*
 *****************************************************************************
 * GetFullPath - Get a full path.
 *
 * filename: File name
 * out: String for full path
 * add_slash: Add slash to the end of string
 *
 * Returns: TRUE or FALSE
 *****************************************************************************
 */

BOOL GetFullPath(IN PCSTR filename, OUT PWSTR out, IN BOOL add_slash)
{
    UNICODE_STRING us;
    ANSI_STRING as;
    WCHAR cur_path[MAX_PATH];
    RtlCliGetCurrentDirectory(cur_path);

    if (NULL == filename || NULL == cur_path || NULL == out)
    {
        return FALSE;
    }

    if ((strlen(filename) > 1) && filename[1] == ':')
    {
        RtlInitAnsiString(&as, filename);
        RtlAnsiStringToUnicodeString(&us, &as, TRUE);

        wcscpy(out, us.Buffer);
        if (add_slash)
        {
            wcscat(out, L"\\");
        }

        RtlFreeUnicodeString(&us);
    }
    else
    {
        RtlInitAnsiString(&as, filename);
        RtlAnsiStringToUnicodeString(&us, &as, TRUE);

        wcscpy(out, cur_path);
        if (out[wcslen(out) - 1] != L'\\')
        {
            wcscat(out, L"\\");
        }
        wcscat(out, us.Buffer);
        if (add_slash)
        {
            wcscat(out, L"\\");
        }

        RtlFreeUnicodeString(&us);
    }
    return TRUE;
}

// Argument processing functions:
static CHAR *xargv[NUM_ARGS];

CHAR **StringToArguments(CHAR *string, UINT *argc)
{
    /* Extract whitespace- and quotes- delimited tokens from the given string
       and put them into the tokens array. Returns number of tokens
       extracted. Length specifies the current size of tokens[].
       THIS METHOD MODIFIES string.  */

    const char *whitespace = " \t\r\n";
    char *tokenEnd;
    const char *quoteCharacters = "\"\'";
    char *end = string + strlen(string);
    UINT length = NUM_ARGS;

    if ((NULL == string) || (NULL == argc) || (0 == length))
        return NULL;

    *argc = 0;

    while (1)
    {
        const char *q;
        /* Skip over initial whitespace.  */
        string += strspn(string, whitespace);
        if (!*string)
            break;

        for (q = quoteCharacters; *q; ++q)
        {
            if (*string == *q)
                break;
        }
        if (*q)
        {
            /* Token is quoted.  */
            char quote = *string++;
            tokenEnd = strchr(string, quote);
            /* If there is no endquote, the token is the rest of the string.  */
            if (!tokenEnd)
                tokenEnd = end;
        }
        else
        {
            tokenEnd = string + strcspn(string, whitespace);
        }

        *tokenEnd = 0;

        xargv[*argc] = string;

        *argc = *argc + 1;

        if ((tokenEnd == end) || (*argc >= length))
            break;
        string = tokenEnd + 1;
    }

    return xargv;
}

/******************************************************************************\
 * GetFileAttributesNt - Get File Attributes
 * fname: File name
\******************************************************************************/

ULONG GetFileAttributesNt(PCWSTR filename)
{
    OBJECT_ATTRIBUTES oa;
    FILE_BASIC_INFORMATION fbi;
    UNICODE_STRING nt_filename;

    RtlDosPathNameToNtPathName_U(filename, &nt_filename, NULL, NULL);
    InitializeObjectAttributes(&oa, &nt_filename, OBJ_CASE_INSENSITIVE, 0, 0);

    fbi.FileAttributes = 0;
    NtQueryAttributesFile(&oa, &fbi);

    return fbi.FileAttributes;
}

/******************************************************************************\
 * FolderExists - Check if folder exists
 * fFile: Folder
\******************************************************************************/

BOOL FolderExists(PCWSTR foldername)
{
    BOOL retval = FALSE;
    UNICODE_STRING u_filename, nt_filename;
    FILE_BASIC_INFORMATION fbi;
    OBJECT_ATTRIBUTES oa;
    NTSTATUS st;

    RtlInitUnicodeString(&u_filename, foldername);
    RtlDosPathNameToNtPathName_U(u_filename.Buffer, &nt_filename, NULL, NULL);

    InitializeObjectAttributes(&oa, &nt_filename, OBJ_CASE_INSENSITIVE, 0, 0);
    st = NtQueryAttributesFile(&oa, &fbi);

    retval = NT_SUCCESS(st);

    if (retval && (fbi.FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************\
 * FileExists - Checks if file exists
 * filename: File name
\******************************************************************************/

BOOL FileExists(PCWSTR filename)
{
    UNICODE_STRING u_filename, nt_filename;
    FILE_BASIC_INFORMATION fbi;
    OBJECT_ATTRIBUTES oa;
    NTSTATUS st;

    RtlInitUnicodeString(&u_filename, filename);
    RtlDosPathNameToNtPathName_U(u_filename.Buffer, &nt_filename, NULL, NULL);

    InitializeObjectAttributes(&oa, &nt_filename, OBJ_CASE_INSENSITIVE, 0, 0);
    st = NtQueryAttributesFile(&oa, &fbi);

    return NT_SUCCESS(st);
}

BOOLEAN DisplayString(WCHAR *pwszData)
{
    UNICODE_STRING ustrData;
    BOOLEAN bRet;

    bRet = SetUnicodeString(&ustrData, pwszData);

    if (bRet == FALSE)
        return FALSE;

    NtDisplayString(&ustrData);

    return TRUE;
}

BOOLEAN SetUnicodeString(UNICODE_STRING *pustrRet, WCHAR *pwszData)
{
    if (pustrRet == NULL || pwszData == NULL)
    {
        return FALSE;
    }

    pustrRet->Buffer = pwszData;
    pustrRet->Length = wcslen(pwszData) * sizeof(WCHAR);
    pustrRet->MaximumLength = pustrRet->Length + sizeof(WCHAR);

    return TRUE;
}

HANDLE InitHeapMemory(void)
{
    RTL_HEAP_PARAMETERS sHeapDef;
    HANDLE hHeap;

    // Init Heap Memory
    memset(&sHeapDef, 0, sizeof(RTL_HEAP_PARAMETERS));
    sHeapDef.Length = sizeof(RTL_HEAP_PARAMETERS);
    hHeap = RtlCreateHeap(HEAP_GROWABLE, NULL, 0x100000, 0x1000, NULL, &sHeapDef);

    return hHeap;
}

BOOLEAN DeinitHeapMemory(HANDLE hHeap)
{
    PVOID pRet;

    pRet = RtlDestroyHeap(hHeap);
    if (pRet == NULL)
        return TRUE;

    return FALSE;
}

PVOID kmalloc(HANDLE hHeap, int nSize)
{
    // if you wanna set new memory to zero, use HEAP_ZERO_MEMORY.
    PVOID pRet = RtlAllocateHeap(hHeap, 0, nSize);

    return pRet;
}

BOOLEAN kfree(HANDLE hHeap, PVOID pMemory)
{
    BOOLEAN bRet = RtlFreeHeap(hHeap, 0, pMemory);

    return bRet;
}

BOOLEAN AppendString(WCHAR *pszInput, WCHAR *pszAppend)
{
    int i, nAppendIndex;

    for (i = 0;; i++)
    {
        if (pszInput[i] == 0x0000)
        {
            break;
        }
    }

    nAppendIndex = 0;
    for (;;)
    {
        if (pszAppend[nAppendIndex] == 0x0000)
        {
            break;
        }
        pszInput[i] = pszAppend[nAppendIndex];

        nAppendIndex++;
        i++;
    }

    pszInput[i] = 0x0000; // set end of string.

    return TRUE;
}

UINT GetStringLength(WCHAR *pszInput)
{
    int i;

    for (i = 0;; i++)
    {
        if (pszInput[i] == 0x0000)
        {
            break;
        }
    }

    return i;
}
