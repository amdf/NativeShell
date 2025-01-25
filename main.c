/*++

Copyright (c) Alex Ionescu.  All rights reserved.
Copyright (c) 2011 amdf.

    THIS CODE AND INFORMATION IS PROVIDED UNDER THE LESSER GNU PUBLIC LICENSE.
    PLEASE READ THE FILE "LICENSE" IN THE TOP LEVEL DIRECTORY.

Module Name:

    main.c

Abstract:

    The Native Command Line Interface (NCLI) is the command shell for the
    TinyKRNL OS.
    This module handles the main command line interface and command parsing.

Environment:

    Native mode

Revision History:

    Alex Ionescu - Started Implementation - 01-Mar-06
    Alex Ionescu - Reworked architecture - 23-Mar-06
    amdf         - Added process launch command - 25-Jan-11
    amdf         - Added move command - 20-Feb-11

--*/
#include "precomp.h"

HANDLE hKeyboard;
HANDLE hHeap;
HANDLE hKey;

#define __APP_VER__ "0.13.2"
#if defined(_M_AMD64) || defined(_AMD64_)
#define __NCLI_VER__ __APP_VER__ "-alpha x64"
#else
#define __NCLI_VER__ __APP_VER__ " x86"
#endif

WCHAR *helpstr[] =
{
  {
    L"\n"
    L"cd X     - Change directory to X    md X     - Make directory X\n"
    L"copy X Y - Copy file X to Y         poweroff - Power off PC\n"
    L"dir      - Show directory contents  pwd      - Print working directory\n"
    L"del X    - Delete file X            reboot   - Reboot PC\n"
    L"devtree  - Dump device tree         shutdown - Shutdown PC\n"
    L"\x0000"
  },
  {
    L"exit     - Exit shell               sysinfo  - Dump system information\n"
    L"lm       - List modules             vid      - Test screen output\n"
    L"lp       - List processes           move X Y - Move file X to Y\n"
    L"\n"
    L"If a command is not in the list, it is treated as an executable name\n"
    L"\n"
    L"\x0000"
  }
};
/*++
 * @name RtlClipProcessMessage
 *
 * The RtlClipProcessMessage routine
 *
 * @param Command
 *        FILLMEIN
 *
 * @return None.
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
VOID
RtlClipProcessMessage(PCHAR Command)
{
    WCHAR CurrentDirectory[MAX_PATH];
    WCHAR buf1[MAX_PATH];
    WCHAR buf2[MAX_PATH];
    UNICODE_STRING CurrentDirectoryString;
    CHAR CommandBuf[BUFFER_SIZE] = {0};
    UINT argc;
    CHAR **argv;

    strncpy(CommandBuf, Command, strnlen(Command, BUFFER_SIZE));

    argv = StringToArguments(&CommandBuf[0], &argc);
    
    if (0 == argc)
      return;

    if (!_strnicmp(argv[0], CMDSTR("exit")))
    {
        // Exit from shell
        DeinitHeapMemory( hHeap );
        NtTerminateProcess(NtCurrentProcess(), 0);
    }
    else if (!_strnicmp(argv[0], CMDSTR("argtest")))
    {
      UINT i = 0;
      
      RtlCliDisplayString("Args: %d\n", argc);

      if (argc > 1)
      {
        for (i = 1; i < argc; i++)
        {        
          if (NULL != argv[i])
            RtlCliDisplayString("Arg %d: %s\n", i, argv[i]);
          else {
            RtlCliDisplayString("Arg %d: NULL\n", i);
            break;
          }
        }
      }
    }
    else if (!_strnicmp(argv[0], CMDSTR("help")))
    {
        RtlCliDisplayString("%S", helpstr[0]);
        RtlCliDisplayString("%S", helpstr[1]);
    }
    else if (!_strnicmp(argv[0], CMDSTR("lm")))
    {
        // List Modules (!lm)
        RtlCliListDrivers();
    }
    else if (!_strnicmp(argv[0], CMDSTR("lp")))
    {
        // List Processes (!lp)
        RtlCliListProcesses();
    }
    else if (!_strnicmp(argv[0], CMDSTR("sysinfo")))
    {
        // Dump System Information (sysinfo)
        RtlCliDumpSysInfo();
    }
    else if (!_strnicmp(argv[0], CMDSTR("cd")))
    {
        // Set the current directory
        RtlCliSetCurrentDirectory(&Command[3]);
    }
    else if (!_strnicmp(argv[0], CMDSTR("drawtext")))
    {
#if (NTDDI_VERSION >= NTDDI_WIN7)
        UNICODE_STRING us;
        ANSI_STRING as;
        RtlInitAnsiString(&as, &Command[9]);
        RtlAnsiStringToUnicodeString(&us, &as, TRUE);
        NtDrawText(&us);
        RtlFreeUnicodeString(&us);
#else
        RtlCliDisplayString("\nNot supported prior to Win7\n");
#endif
    }
    else if (!_strnicmp(argv[0], CMDSTR("pwd")))
    {
        // Get the current directory
        RtlCliGetCurrentDirectory(CurrentDirectory);

        // Display it
        RtlInitUnicodeString(&CurrentDirectoryString, CurrentDirectory);
        RtlCliPrintString(&CurrentDirectoryString);
    }
    else if (!_strnicmp(argv[0], CMDSTR("dir")))
    {
        // List the current directory
        RtlCliListDirectory();
    }
    else if (!_strnicmp(argv[0], CMDSTR("devtree")))
    {
        // Dump hardware tree
        RtlCliListHardwareTree();
    }
    else if (!_strnicmp(argv[0], CMDSTR("shutdown")))
    {
      RtlCliShutdown();
    }
    else if (!_strnicmp(argv[0], CMDSTR("reboot")))
    {
      RtlCliReboot();
    }
    else if (!_strnicmp(argv[0], CMDSTR("poweroff")))
    {
      RtlCliPowerOff();
    }
    else if (!_strnicmp(argv[0], CMDSTR("vid")))
    {
      UINT j;
      WCHAR i, w;
      UNICODE_STRING us;

      LARGE_INTEGER delay;
      memset(&delay, 0x00, sizeof(LARGE_INTEGER));      
      delay.LowPart = 100000000;

      
      RtlInitUnicodeString(&us, L" ");

      //75x23
      RtlCliDisplayString("\nVid mode is 75x23\n\nCharacter test:");
                     
      j = 0;
      for (w = L'A'; w < 0xFFFF; w++) 
      {  
          j++;
          NtDelayExecution(FALSE, &delay);
          //w = i;
          if (w != L'\n' && w != L'\r')
          {
            RtlCliPutChar(w);
          } else
          {
            RtlCliPutChar(L' ');
          }
        if (j > 70) 
        {
          j = 0;
          RtlCliPutChar(L'\n');
        }
      }
    }  
    else if (!_strnicmp(argv[0], CMDSTR("copy")))
    {
      // Copy file
      if (argc > 2)
      {        
        GetFullPath(argv[2], buf1, FALSE);
        GetFullPath(argv[3], buf2, FALSE);
        RtlCliDisplayString("\nCopy %S to %S\n", buf1, buf2);
        if (FileExists(buf1))
        {
          if (!NtFileCopyFile(buf1, buf2))
          {
            RtlCliDisplayString("Failed.\n");
          }
        }
        else
        {
          RtlCliDisplayString("File does not exist.\n");
        }
      } else
      {
        RtlCliDisplayString("Not enough arguments.\n");
      }
    }
    else if (!_strnicmp(argv[0], CMDSTR("move")))
    {
      // Move/rename file
      if (argc > 2)
      {        
        GetFullPath(argv[2], buf1, FALSE);
        GetFullPath(argv[3], buf2, FALSE);
        RtlCliDisplayString("\nMove %S to %S\n", buf1, buf2);
        if (FileExists(buf1))
        {
          if (!NtFileMoveFile(buf1, buf2, FALSE))
          {
            RtlCliDisplayString("Failed.\n");
          }
        }
        else
        {
          RtlCliDisplayString("File does not exist.\n");
        }
      } else
      {
        RtlCliDisplayString("Not enough arguments.\n");
      }
    }
    else if (!_strnicmp(argv[0], CMDSTR("del")))
    {
      // Delete file
      if (argc > 1)
      {        
        GetFullPath(argv[2], buf1, FALSE);
        if (FileExists(buf1))
        {
          RtlCliDisplayString("\nDelete %S\n", buf1);

          if (!NtFileDeleteFile(buf1))
          {
            RtlCliDisplayString("Failed.\n");
          }
        }
        else
        {
          RtlCliDisplayString("File does not exist.\n");
        }
      } else
      {
        RtlCliDisplayString("Not enough arguments.\n");
      }
    }
    else if (!_strnicmp(argv[0], CMDSTR("md")))
    {
      // Make directory
      if (argc > 1)
      {        
        GetFullPath(argv[2], buf1, FALSE);

        RtlCliDisplayString("\nCreate directory %S\n", buf1);

        if (!NtFileCreateDirectory(buf1))
        {
          RtlCliDisplayString("Failed.\n");
        }
      } else
      {
        RtlCliDisplayString("Not enough arguments.\n");
      }
    }
    else
    if ((strlen(argv[0]) == 2) && (argv[0][1] == ':'))
    {
      // Change disk
      RtlCliSetCurrentDirectory(argv[0]);
      return;
    } else
    {      
      // Unknown command, try to find an executable and run it.
      WCHAR filename[MAX_PATH] = {0};
      BOOL bExist = FALSE;

      GetFullPath(argv[0], filename, FALSE);

      bExist = FileExists(filename);
      if (!bExist)
      {
        UINT uOrigLen = wcslen(filename);
        // RtlCliDisplayString("Not exist '%S'\n", filename);
        wcscat(filename, L".exe");
        bExist = FileExists(filename);
        // if (!bExist)
        //   RtlCliDisplayString("Not exist '%S'\n", filename);
      }

      if (bExist)
      {
        HANDLE hProcess;
        NTSTATUS status;
        ANSI_STRING as;
        UNICODE_STRING us;
        RtlInitAnsiString(&as, Command);
        RtlAnsiStringToUnicodeString(&us, &as, TRUE);
        
        NtClose(hKeyboard);

        status = CreateNativeProcess(filename, us.Buffer, &hProcess);
        if (NT_SUCCESS(status))
        {
          NtWaitForSingleObject(hProcess, FALSE, NULL);
        } else {
          RtlCliDisplayString("Failed to execute %s\n", Command);
        }
        RtlCliOpenInputDevice(&hKeyboard, KeyboardType);
        RtlFreeUnicodeString(&us);
      } else
      {
        RtlCliDisplayString("%s is not recognized as a command or an executable file name\n"
            "\nType \"help\" for the list of commands.\n", Command);
      }        
    }
}

/*++
 * @name RtlClipDisplayPrompt
 *
 * The RtlClipDisplayPrompt routine
 *
 * @param None.
 *
 * @return None.
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
VOID
RtlClipDisplayPrompt(VOID)
{
    WCHAR CurrentDirectory[MAX_PATH];
    ULONG DirSize;
    UNICODE_STRING DirString;

    //
    // Get the current directory
    //
    DirSize = RtlCliGetCurrentDirectory(CurrentDirectory) / sizeof(WCHAR);

    if (!RtlDosPathNameToNtPathName_U(CurrentDirectory,
                                      &DirString,
                                      NULL,
                                      NULL))
    {
        CurrentDirectory[DirSize] = L'>';
        CurrentDirectory[DirSize + 1] = UNICODE_NULL;
        RtlInitUnicodeString(&DirString, CurrentDirectory);
        RtlCliPrintString(&DirString);
    }

    RtlCliDisplayString("%S>>", CurrentDirectory);
}

/*++
 * @name main
 *
 * The main routine
 *
 * @param argc
 *        FILLMEIN
 *
 * @param argv[]
 *        FILLMEIN
 *
 * @param envp[]
 *        FILLMEIN
 *
 * @param DebugFlag
 *        FILLMEIN
 *
 * @return NTSTATUS
 *
 * @remarks Documentation for this routine needs to be completed.
 *
 *--*/
NTSTATUS
__cdecl
main(INT argc,
     PCHAR argv[],
     PCHAR envp[],
     ULONG DebugFlag OPTIONAL)
{
    PPEB Peb = NtCurrentPeb();
    NTSTATUS Status;
    PCHAR Command;

    hHeap = InitHeapMemory();
    hKey = NULL;

    //
    // Show banner
    //
    RtlCliDisplayString("Native Shell v" __NCLI_VER__ " (" __DATE__ " " __TIME__ ")\n\n");

    //
    // Setup keyboard input
    //
    Status = RtlCliOpenInputDevice(&hKeyboard, KeyboardType);

    //
    // Show initial prompt
    //
    RtlClipDisplayPrompt();

    //
    // Wait for a new line
    //
    while (TRUE)
    {

        // Get the line that was entered and display a new line

        Command = RtlCliGetLine(hKeyboard);
        RtlCliDisplayString("\n");


        // Make sure there's actually a command

        if (*Command)
        {
    
            // Process the command and do a new line again.
    
            RtlClipProcessMessage(Command);
            RtlCliDisplayString("\n");
        }


        // Display the prompt, and restart the loop

        RtlClipDisplayPrompt();
        continue;
    }

    DeinitHeapMemory( hHeap );
    NtTerminateProcess( NtCurrentProcess(), 0 );
    
    //
    // Return
    //
    return STATUS_SUCCESS;
}
