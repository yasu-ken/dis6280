echo off

set path="C:\borland\bcc55\Bin"

BCC32.EXE -w-8057 dis.cpp format.cpp -Edis6280.exe
