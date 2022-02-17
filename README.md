# dis6280
PC Engine( HuC6280 ) Disassembler

It is a disassembler of the PC engine.

Disassemble the ROM and Generate source code from binary data.

(As much as possible) Fixed the output to follow the mnemonics of PCEAS.exe.


---------------------------
1.compilation

(Borland C++ 5.5)

set path="C:\borland\bcc55\Bin"

BCC32.EXE -w-8057 dis.cpp format.cpp -Edis6280.exe

(I think it can be compiled with gcc.)

---------------------------
2.command line

dis6280 <infile> <outfile> [length] [offset] [startaddr] [-[b][w]]

  [length]     number of instructions to parse
  
  [offset]     first byte of the input file to parse
  
  [startaddr]  outfile's starting address
  
  (numbers default to decimal; hexadecimal if preceeded by '$')
  
  -b??   .DB $xx,$xx...(??)
  
  -w??   .DW $xxxx,$xxxx...(??)
