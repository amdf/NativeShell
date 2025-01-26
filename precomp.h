/**
 * PROJECT:         Native Shell
 * COPYRIGHT:       LGPL; See LICENSE in the top level directory
 * FILE:            precomp.h
 * DESCRIPTION:     Precompiled header.
 * DEVELOPERS:      See CONTRIBUTORS.md in the top level directory
 */

#define WIN32_NO_STATUS
#define NTOS_MODE_USER
#include <stdio.h>
#include <stdarg.h>
#include <excpt.h>
#include <windef.h>
#include <winnt.h>
#include <ntndk.h>
#include <ntddkbd.h>
#include <sdkddkver.h>
#include "ntfile.h"
#include "ntreg.h"



//
// Device type for input/output
//
typedef enum _CON_DEVICE_TYPE
{
    KeyboardType,
    MouseType
} CON_DEVICE_TYPE;

//
// Display functions
//
NTSTATUS
__cdecl
RtlCliDisplayString(
    IN PCH Message,
    ...
);

NTSTATUS
RtlCliPrintString(
    IN PUNICODE_STRING Message
);

NTSTATUS
RtlCliPutChar(
    IN WCHAR Char
);

//
// Input functions
//
NTSTATUS
RtlCliOpenInputDevice(
    OUT PHANDLE Handle,
    IN CON_DEVICE_TYPE Type
);

CHAR
RtlCliGetChar(
    IN HANDLE hDriver
);

PCHAR
RtlCliGetLine(
    IN HANDLE hDriver
);

//
// System information functions
//
NTSTATUS
RtlCliListDrivers(
    VOID
);

NTSTATUS
RtlCliListProcesses(
    VOID
);

NTSTATUS
RtlCliDumpSysInfo(
    VOID
);

NTSTATUS
RtlCliShutdown(
    VOID
);

NTSTATUS
RtlCliReboot(
    VOID
);

NTSTATUS
RtlCliPowerOff(
    VOID
);

//
// Hardware functions
//
NTSTATUS
RtlCliListHardwareTree(
    VOID
);

//
// File functions
//
NTSTATUS
RtlCliListDirectory(
    PWCHAR CurrentDirectory
);

NTSTATUS
RtlCliSetCurrentDirectory(
    PCHAR Directory
);

ULONG
RtlCliGetCurrentDirectory(
    IN OUT PWSTR CurrentDirectory
);

// Keyboard:

HANDLE hKeyboard;

typedef struct _KBD_RECORD {
  WORD  wVirtualScanCode;
  DWORD dwControlKeyState;
  UCHAR AsciiChar;
  BOOL  bKeyDown;
} KBD_RECORD, *PKBD_RECORD;

void IntTranslateKey(PKEYBOARD_INPUT_DATA InputData, KBD_RECORD *kbd_rec);

#define RIGHT_ALT_PRESSED     0x0001 // the right alt key is pressed.
#define LEFT_ALT_PRESSED      0x0002 // the left alt key is pressed.
#define RIGHT_CTRL_PRESSED    0x0004 // the right ctrl key is pressed.
#define LEFT_CTRL_PRESSED     0x0008 // the left ctrl key is pressed.
#define SHIFT_PRESSED         0x0010 // the shift key is pressed.
#define NUMLOCK_ON            0x0020 // the numlock light is on.
#define SCROLLLOCK_ON         0x0040 // the scrolllock light is on.
#define CAPSLOCK_ON           0x0080 // the capslock light is on.
#define ENHANCED_KEY          0x0100 // the key is enhanced.

// Process:

NTSTATUS CreateNativeProcess(IN PCWSTR file_name, IN PCWSTR cmd_line, OUT PHANDLE hProcess);

#define BUFFER_SIZE 1024
#define NUM_ARGS 256

// Command processing:
#define CMDSTR(x) x, strlen(x)
CHAR **StringToArguments(CHAR *string, UINT *argc);

BOOL GetFullPath(IN PCSTR filename, OUT PWSTR out, IN BOOL add_slash);
BOOL FileExists(PCWSTR fname);

// Registry

NTSTATUS OpenKey(OUT PHANDLE pHandle, IN PWCHAR key);
NTSTATUS RegWrite(HANDLE hKey, INT type, PWCHAR key_name, PVOID data, DWORD size);

NTSTATUS RegReadValue(HANDLE hKey, PWCHAR key_name, OUT PULONG type, OUT PVOID data, IN ULONG buf_size, OUT PULONG out_size);

// Misc

// void FillUnicodeStringWithAnsi(OUT PUNICODE_STRING us, IN PCHAR as);

//===========================================================
//
// Helper Functions for ntreg.c
//
//===========================================================

BOOLEAN SetUnicodeString (
  UNICODE_STRING* pustrRet, 
  WCHAR* pwszData
  );
BOOLEAN 
  DisplayString (
  WCHAR* pwszData
  );
HANDLE 
  InitHeapMemory (void);
BOOLEAN 
  DeinitHeapMemory (
  HANDLE hHeap
  );
PVOID 
  kmalloc (
  HANDLE hHeap, 
  int nSize 
  );
BOOLEAN 
  kfree ( 
  HANDLE hHeap, 
  PVOID pMemory 
  );
BOOLEAN 
  AppendString(
  WCHAR* pszInput, 
  WCHAR* pszAppend
  );
UINT 
  GetStringLength(
  WCHAR* pszInput
  );
