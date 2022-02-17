
dis6280

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

:start bank
:rom adress $00000-$01fff(+$2000) --> $e000-$ffff
dis6280 spacefighter.pce source.src $2000 $00000 $e000

:(If rom have a header. source address += $200)
dis6280 spacefighter.pce source.src $2000 $00200 $e000

:rom adress $22000-$23fff(+$2000) --> $8000-$9fff
dis6280 spacefighter.pce source.src $2000 $22200 $8000

:rom adress $16000-$1bfff --> $4000-$9fff
dis6280 spacefighter.pce source.src $6000 $16200 $4000

:output .db $??,$??....
dis6280.exe BALL.PCE BALL_DB.TXT $2000 $00200 $e000 -b8

---------------------------
3.PCEAS.exe where is it?

github
https://github.com/dshadoff/huc
https://github.com/BouKiCHi/HuSIC
https://github.com/uli/huc
https://github.com/Turboxray/PCEAS

DevKit HomePage
http://www.magicengine.com/mkit/
(16bit)

Included in Huc
(16bit)
Zeograd's lair(->HuC)
https://www.zeograd.com/index.php
Zophar's Domain(development->pce)
https://www.zophar.net/
(16bit)

---------------------------
4.License

Original production year 1996, license unknown.
The part I made shall be the same as the original license.

----------------------------------------------------------------

PCエンジンのROMを逆アセンブルして、バイナリデータからソースコードを生成します.

(できるだけ)PCEAS.exeのニーモニックに順ずる出力になるように修正しました.

オリジナルの製作年1996年で、ライセンスは不明です.
私の作った部分はオリジナルに準じるものとします.

dis6280 <infile> <outfile> [length] [offset] [startaddr] [-[b][w]]
  [length]     number of instructions to parse
  [offset]     first byte of the input file to parse
  [startaddr]  outfile's starting address
  (numbers default to decimal; hexadecimal if preceeded by '$')
  -b??   .DB $xx,$xx...(??)
  -w??   .DW $xxxx,$xxxx...(??)

  長さ、スタート位置、割り当てアドレス
  長さ  $2000で一バンクぶんです
  STARTADR ロム内の分析開始アドレス。.PCEならヘッダあるので $00200 $002200 $ 004200...などです。
  アセンブル時に割り当てるアドレス .ORGで宣言されるものです。 $0000 2000 4000 6000 ...A000 C000 E000のどれかです
  -b8 ニーモニックではなく、.DBで出力します
  -w16 ニーモニックではなく、.DWで出力します
