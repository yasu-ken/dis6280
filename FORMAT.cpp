/****************************************************************************
   format.c -- Joseph LoCicero, IV -- 22.2.96
               Dave Shadoff        -- 20.3.96

   line-formatting routines for disassembler


%sに幅指定できるのでしょうか。調べます。

 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "format.h"
void lineprint3_err(char *outf, long ctr, unsigned char * op, char *outstring);

	//──────────────────────────────
	// lineprinters, to keep code clean.
	//──────────────────────────────
void lineprint1(char *outf, long ctr, unsigned char * op, char *outstring)
{
   sprintf(outf, "_%04lX:  %-26s; %02X       ;",  ctr, outstring,*op);
}

	//──────────────────────────────
	// 
	//──────────────────────────────
	//@@@@@@@
	//for ???
void lineprint1000(char *outf, long ctr, unsigned char * op, char *outstring)
{
    //sprintf(outf, "_%04lX:  .DB $%02X                   ; %s ; %02X       ;", ctr, *op, outstring, *op);
    sprintf(outf, "_%04lX:  .DB $%02X                   ; %02X       ;", ctr, *op, *op);
}


	//──────────────────────────────
	// 
	//──────────────────────────────
void lineprint2(char *outf, long ctr, unsigned char * op, char *outstring)
{
    sprintf(outf, "_%04lX:  %-26s; %02X %02X    ;",ctr, outstring, *op, *(op+1));
}


	//──────────────────────────────
	// 
	//──────────────────────────────
	//for relative error display
	//0000F: FF 78 D4      BBS7  <$78, $FFFFFFE6		エラーとなる出力（例）
	//0000F:               .DB $FF,$78,$D4  ;BBS7  <$78, $FFFFFFE6   REL PROBLEM
void lineprint2_err(char *outf, long ctr, unsigned char * op, char *outstring)
{
	//sprintf(outf, "_%04lX:  .DB $%02X,$%02X               ; %s ; %02X %02X    ;",
	// ctr, *op, *(op+1), outstring, *op, *(op+1));
	sprintf(outf, "_%04lX:  .DB $%02X,$%02X               ; %02X %02X    ;",
		ctr, *op, *(op+1), *op, *(op+1));
}

	//──────────────────────────────
	// 
	//──────────────────────────────
void lineprint3(char *outf, long ctr, unsigned char * op, char *outstring)
{
	// ff ff ff のとき。BBS7  <$FF, （自分のコマンドの+3-1のアドレス）というのは
	// 有り得ないので、ただのデータと思われます。
	if(*op == 0xff)
		if(*(op+1) == 0xff)
			if(*(op+2) == 0xff){
				lineprint3_err(outf, ctr, op, outstring);
				return;
			}
   sprintf(outf, "_%04lX:  %-26s; %02X %02X %02X ;",
		ctr, outstring, *op, *(op+1), *(op+2));
}



	//──────────────────────────────
	// 
	//──────────────────────────────
	//for relative error display
	//0000F: FF 78 D4      BBS7  <$78, $FFFFFFE6		エラーとなる出力（例）
	//0000F:               .DB $FF,$78,$D4  ;BBS7  <$78, $FFFFFFE6   REL PROBLEM
void lineprint3_err(char *outf, long ctr, unsigned char * op, char *outstring)
{
    //sprintf(outf, "_%04lX:  .DB $%02X,$%02X,$%02X           ; %s  ; %02X %02X %02X ;"
	//		,ctr, *op, *(op+1), *(op+2), outstring, *op, *(op+1), *(op+2));
    sprintf(outf, "_%04lX:  .DB $%02X,$%02X,$%02X           ; %02X %02X %02X ;"
			,ctr, *op, *(op+1), *(op+2), *op, *(op+1), *(op+2));
}

	//──────────────────────────────
	// 
	//──────────────────────────────
void lineprint4(char *outf, long ctr, unsigned char * op, char *outstring)
{
   sprintf(outf, "_%04lX:  %-26s; %02X %02X %02X %02X ;",ctr, outstring, *op, *(op+1), *(op+2), *(op+3));

}



	//──────────────────────────────
	//TIIなどの７バイト系命令です
	//元の出力
	//0001E: 9C 00 20      STZ   $2000
	//00021: 73 00 20 01 20 FF 1F  TII   $2000, $2001, $1FFF  ;@
	//00028: 9C 01 0C      STZ   $0C01
	//
	//次のような出力にします
	//0001E: 9C 00 20      STZ   $2000
	//00021:               ;73 00 20 01 20 FF 1F
	//                     TII   $2000, $2001, $1FFF  ;@
	//──────────────────────────────
void lineprint7(char *outf, long ctr, unsigned char * op, char *outstring)
{
   sprintf(outf, "_%04lX:  %-26s; %02X %02X %02X %02X %02X %02X %02X ",
                   ctr,outstring, *op, *(op+1), *(op+2), *(op+3), *(op+4),
                      *(op+5), *(op+6));
}


	//──────────────────────────────
	// DB DW 出力
	//──────────────────────────────

char strbuf[256];
void data_db(char *outf, long ctr, unsigned char * op ,int suujicnt)
{
	int i;
	//sprintf(outf, "_%04lX:  .DB ",ctr);

	sprintf(strbuf, "_%04lX:  .DB ",ctr);
	strcpy(outf,strbuf);

	for(i=0;i<suujicnt;i++){
		sprintf(strbuf, "$%02X", *(op+i));
		strcat(outf,strbuf);

		if(i==(suujicnt-1))break;
		sprintf(strbuf, ",", *(op+i));
		strcat(outf,strbuf);
	}
}
void data_dw(char *outf, long ctr, unsigned char * op ,int suujicnt)
{
	int i;

	sprintf(strbuf, "_%04lX:  .DW ",ctr);
	strcpy(outf,strbuf);

	for(i=0;i<suujicnt;i+=2){
		sprintf(strbuf, "$%02X%02X",*(op+i+1),*(op+i));
		strcat(outf,strbuf);

		if(i>=(suujicnt-2))break;
		sprintf(strbuf, ",", *(op+i));
		strcat(outf,strbuf);
	}
}

//──────────────────────────────
//──────────────────────────────
/* common addressing-mode formatters */
/* look/act as wrappers around lineprint functions */

void implicit(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s", str);
   lineprint1(outf, ctr, op, buf);
}
//04462: 9B            ??? 
//04462: 9B            .DB $9B  ;??? 
//@@@@@@@
void unknown(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s", str);
   lineprint1000(outf, ctr, op, buf);
}

//@@@@@@@
void accumlator(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  A", str);
   lineprint1(outf, ctr, op, buf);
}

void immed(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  #$%02X", str, *(op+1));
   lineprint2(outf, ctr, op, buf);
}

//for TMA TAM

//このバグの対応
//	_893D:  TMA   #$FFFFFFFF          ; 43 FF    ;
//	.db $43,$ff

unsigned int henkantbl[9][2]={{0x1,0},{0x02,1},{0x04,2},{0x08,3},{0x10,4},{0x20,5},{0x040,6},{0x080,7},{0x0ff,0x0ff}};
void immed2(char *outf, long ctr, unsigned char * op, char *str)
{
	char buf[256];
	int wok;
	int lp;
	//for(lp=0;lp<8;lp++){
	for(lp=0;lp<9;lp++){
		if((*(op+1))==henkantbl[lp][0]){
			wok = henkantbl[lp][1];
			break;
		}
	}

	//！  ==8では、TMA #80などのとき、正常に判断できません。9個データを用意し、ダミーのところまでだったら変換不可データと判断。
	//if(lp==8){		//オペランドがあり得ない数値だったとき、???の出力→リターン
	if(lp==9){		//オペランドがあり得ない数値だったとき、???の出力→リターン
	   sprintf(buf, "%-4s", str);
		// lineprint1000(outf, ctr, op, buf);//本当は１バイトだけ処理したいのですが、むずかしいので２バイトまとめて。
                                         //どうせこのパターンが出るときはデータ領域です
	   lineprint2_err(outf, ctr, op, buf);
		return;
	}

	if( wok == 0x0ff ){
		//0xffのとき、たまにあります。ffでないときはほとんどないです。不思議
		sprintf(buf, ".db $43,$ff");   //TMA #$0ff はpceasでエラーになるため
		lineprint2(outf, ctr, op, buf);
		return;
	}

	sprintf(buf, "%-4s  #$%02X", str, wok);
	lineprint2(outf, ctr, op, buf);
}

void relative(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   long newadd;
   int offset = (int) *(op+1);


   if (offset >= 128)
      offset -= 256;

   newadd = (ctr+2) + offset;

	if((newadd <0)||
      (newadd >0x0ffff)){
	   sprintf(buf, "%-4s  $%04lX   REL PROBLEM", str, newadd);
	   lineprint2_err(outf, ctr, op, buf);
	}
	else{		//normal
	   sprintf(buf, "%-4s  $%04lX", str, newadd);
	   lineprint2(outf, ctr, op, buf);
	}
}

void ind_zp(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  <$%02X", str, *(op+1));
   lineprint2(outf, ctr, op, buf);
}

void ind_zpx(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  <$%02X,X", str, *(op+1));
   lineprint2(outf, ctr, op, buf);
}

void ind_zpy(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  <$%02X,Y", str, *(op+1));
   lineprint2(outf, ctr, op, buf);
}

void ind_zpind(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  [$%02X]", str, *(op+1));
   lineprint2(outf, ctr, op, buf);
}

void ind_zpix(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  [$%02X,X]", str, *(op+1));
   lineprint2(outf, ctr, op, buf);
}

void ind_zpiy(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  [$%02X],Y", str, *(op+1));
   lineprint2(outf, ctr, op, buf);
}

void absol(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  $%02X%02X", str, *(op+2), *(op+1));
   lineprint3(outf, ctr, op, buf);
}

void absind(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  [$%02X%02X]", str, *(op+2), *(op+1));
   lineprint3(outf, ctr, op, buf);
}

void absindx(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  [$%02X%02X,X]", str, *(op+2), *(op+1));
   lineprint3(outf, ctr, op, buf);
}

void absx(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  $%02X%02X,X", str, *(op+2), *(op+1));
   lineprint3(outf, ctr, op, buf);
}

void absy(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   sprintf(buf, "%-4s  $%02X%02X,Y", str, *(op+2), *(op+1));
   lineprint3(outf, ctr, op, buf);
}

void pseudorel(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];
   long newadd;
   int  offset = (int) *(op+2);

   if (offset >= 128)
      offset -= 256;

   newadd = (ctr+3) + offset;

	if((newadd <0)||
      (newadd >0x0ffff)){
	   sprintf(buf, "%-4s  <$%02X, $%04lX   REL PROBLEM", str, *(op+1), newadd);
	   lineprint3_err(outf, ctr, op, buf);
	}
	else{		//normal
	   sprintf(buf, "%-4s  <$%02X, $%04lX", str, *(op+1), newadd);
	   lineprint3(outf, ctr, op, buf);
	}
}

void tst_zp(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];

//   sprintf(buf, "%-4s  $%02X, <$%02X", str, *(op+1), *(op+2));
   sprintf(buf, "%-4s  #$%02X, <$%02X", str, *(op+1), *(op+2));
   lineprint3(outf, ctr, op, buf);
}

void tst_abs(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];

//   sprintf(buf, "%-4s  $%02X, $%02X%02X", str, *(op+1), *(op+3), *(op+2));
   sprintf(buf, "%-4s  #$%02X, $%02X%02X", str, *(op+1), *(op+3), *(op+2));
   lineprint4(outf, ctr, op, buf);
}

// TST #$60,<$10,X   A3 60 10
void tst_zpx(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];

//   sprintf(buf, "%-4s  $%02X, <$%02X", str, *(op+1), *(op+2));
//   sprintf(buf, "%-4s  #$%02X, <$%02X", str, *(op+1), *(op+2));
   sprintf(buf, "%-4s  #$%02X, <$%02X,X", str, *(op+1), *(op+2));
   lineprint3(outf, ctr, op, buf);
}


//#i,n,X
// TST #$09, $0002,X         ; B3 09 02 00 ;
void tst_absx(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];

//   sprintf(buf, "%-4s  $%02X, $%02X%02X", str, *(op+1), *(op+3), *(op+2));
//   sprintf(buf, "%-4s  #$%02X, $%02X%02X", str, *(op+1), *(op+3), *(op+2));
   sprintf(buf, "%-4s  #$%02X, $%02X%02X,X", str, *(op+1), *(op+3), *(op+2));
   lineprint4(outf, ctr, op, buf);
}

void xfer(char *outf, long ctr, unsigned char * op, char *str)
{
   char buf[256];

   sprintf(buf, "%-4s  $%02X%02X, $%02X%02X, $%02X%02X",
                str, *(op+2), *(op+1), *(op+4), *(op+3), *(op+6), *(op+5));
   lineprint7(outf, ctr, op, buf);
}
