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
    CHAR CommandBuf[BUFFER_SIZE];

    //
    // Copy command line and break it to arguments
    //
    // if xargc = 3, then xargv[1], xargv[2], xargv[3] are available
    // xargv[1] is a command name, xargv[2] is the first parameter

    memset(CommandBuf, 0x00, BUFFER_SIZE);
    strncpy(CommandBuf, Command, strnlen(Command, BUFFER_SIZE));
    StringToArguments(CommandBuf);

    //
    // We'll call the handler for each command
    //
    if (!_strnicmp(Command, "exit", 4))
    {
        //
        // Exit from shell
        //
        DeinitHeapMemory( hHeap );
        NtTerminateProcess(NtCurrentProcess(), 0);
    }
    else if (!_strnicmp(Command, "test", 4))
    {
      UINT i = 0;
      
      RtlCliDisplayString("Args: %d\n", xargc);
      for (i = 1; i < xargc; i++)
      {        
        RtlCliDisplayString("Arg %d: %s\n", i, xargv[i]);
      }
    }
    else if (!_strnicmp(Command, "help", 4))
    {
        RtlCliDisplayString("%S", helpstr[0]);
        RtlCliDisplayString("%S", helpstr[1]);
    }
    else if (!_strnicmp(Command, "lm", 2))
    {
        //
        // List Modules (!lm)
        //
        RtlCliListDrivers();
    }
    else if (!_strnicmp(Command, "lp", 2))
    {
        //
        // List Processes (!lp)
        //
        RtlCliListProcesses();
    }
    else if (!_strnicmp(Command, "sysinfo", 7))
    {
        //
        // Dump System Information (sysinfo)
        //
        RtlCliDumpSysInfo();
    }
    else if (!_strnicmp(Command, "cd", 2))
    {
        //
        // Set the current directory
        //
        RtlCliSetCurrentDirectory(&Command[3]);
    }
    else if (!_strnicmp(Command, "drawtext", 8))
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
    else if (!_strnicmp(Command, "pwd", 3))
    {
        //
        // Get the current directory
        //
        RtlCliGetCurrentDirectory(CurrentDirectory);

        //
        // Display it
        //
        RtlInitUnicodeString(&CurrentDirectoryString, CurrentDirectory);
        RtlCliPrintString(&CurrentDirectoryString);
    }
    else if (!_strnicmp(Command, "dir", 3))
    {
        //
        // List the current directory
        //
        RtlCliListDirectory();
    }
    else if (!_strnicmp(Command, "devtree", 7))
    {
        //
        // Dump hardware tree
        //
        RtlCliListHardwareTree();
    }
    else if (!_strnicmp(Command, "shutdown", 8))
    {
      RtlCliShutdown();
    }
    else if (!_strnicmp(Command, "reboot", 6))
    {
      RtlCliReboot();
    }
    else if (!_strnicmp(Command, "poweroff", 6))
    {
      RtlCliPowerOff();
    }
    else if (!_strnicmp(Command, "vid", 6))
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
    else if (!_strnicmp(Command, "copy", 4))
    {
      // Copy file
      if (xargc > 2)
      {        
        GetFullPath(xargv[2], buf1, FALSE);
        GetFullPath(xargv[3], buf2, FALSE);
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
    else if (!_strnicmp(Command, "move", 4))
    {
      // Move/rename file
      if (xargc > 2)
      {        
        GetFullPath(xargv[2], buf1, FALSE);
        GetFullPath(xargv[3], buf2, FALSE);
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
    else if (!_strnicmp(Command, "del", 3))
    {
      // Delete file
      if (xargc > 1)
      {        
        GetFullPath(xargv[2], buf1, FALSE);
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
    else if (!_strnicmp(Command, "md", 2))
    {
      // Make directory
      if (xargc > 1)
      {        
        GetFullPath(xargv[2], buf1, FALSE);

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
    {
        //
        // Unknown command, try to find an executable and run it.
        // Executable name should be with an .exe extension.
        //
        
        WCHAR filename[MAX_PATH];
        ANSI_STRING as;
        UNICODE_STRING us;
        HANDLE hProcess;

        GetFullPath(IN xargv[1], OUT filename, FALSE);
    
        if (FileExists(filename))
        {
          RtlInitAnsiString(&as, Command);
          RtlAnsiStringToUnicodeString(&us, &as, TRUE);
                             
          NtClose(hKeyboard);

          CreateNativeProcess(filename, us.Buffer, &hProcess);

          RtlFreeUnicodeString(&us);

          NtWaitForSingleObject(hProcess, FALSE, NULL);
                    
          RtlCliOpenInputDevice(&hKeyboard, KeyboardType);
        } else
        {
          RtlCliDisplayString("%s not recognized\n"
              "Add .exe if you want to lauch executable file."
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

    //
    // Display it
    //
    CurrentDirectory[DirSize] = L'>';
    CurrentDirectory[DirSize + 1] = UNICODE_NULL;
    RtlInitUnicodeString(&DirString, CurrentDirectory);
    RtlCliPrintString(&DirString);
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
        //
        // Get the line that was entered and display a new line
        //
        Command = RtlCliGetLine(hKeyboard);
        RtlCliDisplayString("\n");

        //
        // Make sure there's actually a command
        //
        if (*Command)
        {
            //
            // Process the command and do a new line again.
            //
            RtlClipProcessMessage(Command);
            RtlCliDisplayString("\n");
        }

        //
        // Display the prompt, and restart the loop
        //
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
