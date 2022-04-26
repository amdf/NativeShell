# NativeShell
Command line interface for Windows Native Mode.
It can perform some basic operations with Windows files and directories. 
Program runs before starting of main Windows components, so it has access to the entire file system and registry without many restrictions. 
All operations are implemented through the Native API (ntdll.dll). 

The sources are based on NCLI (Native Command Line Interface) from the TinyKRNL Project. A code from ZenWINX library partially used. 

## Requirements
Windows Driver Kit Version 7.1.0

## Build

Build command for WDK x86 Free Build Environment:

`build /g /w`

Build output is native.exe.

# Install

Copy native.exe to %systemroot%\system32\

Use add.reg from `install` directory.

# Uninstall
Use remove.reg from `install` directory.

Delete %systemroot%\system32\native.exe