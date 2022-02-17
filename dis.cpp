/****************************************************************************
 * tgdisasm.c -- Joseph LoCicero, IV -- 96/5/09
                 Dave Shadoff        -- 96/5/09

    Simple TG Disassembler based on 65C02 opcode listing.

    Updates:
    --------
    Dave Shadoff  96/7/19  Place 'new' Hu6280 opcodes/addressing modes in,
                           due to new information available from Japanese
                           'Develo' system asembler.

 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <sys\stat.h>
#include <errno.h>

#include "format.h"
#include "dis.h"


/* DEFINE the line below if running under OS/2 (no ESPIPE) */
#define  USING_OS2


/* buffer size of opcode buffer */

//#define  OPBUF_SIZE    7  もとはTII命令が最長だから7でよかったが、DBDWコマンドを追加のため多くする。
#define  OPBUF_SIZE    32  /* db32個、dw16個まで指定できる*/


/* GLOBALS */

char opbuf[OPBUF_SIZE];



/* jologetchar, just to keep code clean, does a nice read(). */
int jologetchar(FILE *inf)
{
   unsigned char c;

   return ((fread(&c, 1, 1, inf) == 0) ? EOF: (int) c);
}


long inputnum(char * string)
{
   long   value = 0;
   char * c = (string + 1);

   if (*string != '$') {
      return(atol(string));
   }

   while (*c != '\0') {

     value *= 16;

     if ((*c >= '0') && (*c <= '9')) {
       value += (*c - '0');

     } else if ((*c >= 'A') && (*c <= 'F')) {
       value += (*c - 'A' + 10);

     } else if ((*c >= 'a') && (*c <= 'f')) {
       value += (*c - 'a' + 10);

     } else {

       return(-1);
     }

     
     c++;
   }

   return(value);
}


int dbflg=0;
int dwflg=0;
int db_cnt=0;

int main(int argc, char *argv[])
{
	FILE *inf;
	FILE *outf;
	char linebuf[256];
	unsigned char op;
	long ctr    = 0;			//ロケーションカウンタ
	long offset = 0;
	long length = -1;
	long datacnt= 0;			//出力データカウンタ（length分出力するため）

	int  i, size,wok;
	int  temp,lp;
	int loop;

	if (argc < 3) {
		fprintf(stderr, "Usage:  %s <infile> <outfile> [length] [offset] [startaddr] [-[b][w]]\n", argv[0]);
		fprintf(stderr, "   [length]     number of instructions to parse\n");
		fprintf(stderr, "   [offset]     first byte of the input file to parse\n");
		fprintf(stderr, "   [startaddr]  outfile's starting address\n");
		fprintf(stderr, " (numbers default to decimal; hexadecimal if preceeded by '$')\n");
		fprintf(stderr, " -b??   .DB $xx,$xx...(??)  \n");
		fprintf(stderr, " -w??   .DW $xxxx,$xxxx...(??)  \n");
		exit(1);
	}
	else {
		/* Open input and output files. */
		if ((inf = fopen(argv[1], "rb")) == NULL) {
			fprintf(stderr, "%s: can't open %s\n", argv[0], argv[1]);
			exit(1);
		}
		if ((outf = fopen(argv[2], "w")) == NULL) {
			fprintf(stderr, "%s: can't open %s\n", argv[0], argv[2]);
			exit(1);
		}

		dbflg=0;
		dwflg=0;
		db_cnt=0;
		for(i=1;i<argc;i++){
			if(argv[i][0]=='-'){
				if(argv[i][1]=='b'){
					dbflg=1;
					if(argv[i][3]>='0' && argv[i][3]<='9')
						db_cnt = (argv[i][2]-'0') * 10 + (argv[i][3]-'0');
					else
						db_cnt = (argv[i][2]-'0');
				}
				if(argv[i][1]=='w'){
					dwflg=1;
					if(argv[i][3]>='0' && argv[i][3]<='9')
						db_cnt = (argv[i][2]-'0') * 10 + (argv[i][3]-'0');
					else
						db_cnt = (argv[i][2]-'0');
				}
			}
		}
		if(db_cnt<=0)db_cnt=4;
		if(db_cnt>50)db_cnt=50;



		//最初のパラメータ……出力バイト数
		length = 0x20000;		//出力データバイト数の最大 2 mega bytes
		datacnt=0;
		/* length if requested */
		if (argc >= 4) {
			length = inputnum(argv[3]);
			if (length == -1) {
				fprintf(stderr, "Invalid length!");
				exit(1);
			}
		}

		//ファイルスキップバイト数
		/* Skip to a certain offset if requested */
		if (argc >= 5) {
			offset = inputnum(argv[4]);
			if (offset == -1) {
				fprintf(stderr, "Invalid offset!");
				exit(1);
			}
			if (fseek(inf, offset, SEEK_SET)) {
				fprintf(stderr, "End of input file reached!");
				exit(1);
			}
#ifdef USING_OS2
			if (errno == EBADF || errno == EINVAL) {
#else
			if (errno == EBADF || errno == EINVAL || errno == ESPIPE) {
#endif
				fprintf(stderr, "End of input file reached!");
				exit(1);
			}
		}

		//スタートアドレス（たとえばスタートバンクならe000からとか）
		/* Start numbering at requested integer */
		if (argc >= 6) {
			ctr = inputnum(argv[5]);
			if (ctr == -1) {
				fprintf(stderr, "Invalid start address !");
				exit(1);
			}
		}

		//論理アドレスを指定できます
		//（意味がない、これ以降使われていない変数。機能未実装？）
		/* Start numbering at requested integer */
		if (argc == 7) {
			offset = inputnum(argv[6]);
		}

		for (i = 0; i < OPBUF_SIZE; i++) {
			if ((temp = jologetchar(inf)) == EOF) {
				fprintf(stderr, "early file end !\n");
				exit(1);
			}
			opbuf[i] = temp;
		}

		//while (length != 0) {
		for(loop = 0;loop<0x20000;loop++){
			op = opbuf[0];
			//CTRをみて、指定長に達したら、残りを???で出力してEXIT
			wok = addr_info[optable[op].addr_mode].size;
			wok = wok+datacnt;

			//ぴったりの時や、余裕がまだあるときは処理します
			if (length<wok){	//半端なバイト数残ってしまったときです
				wok = length-datacnt;		//???を出力します
				for(lp=0;lp<wok;lp++){
					(*addr_info[AM_UNKNOWN].func)           /* function      */
					(linebuf, ctr, opbuf, "???");    /* parm's passed */
					fprintf(outf, "%s\n", linebuf);

					size = addr_info[AM_UNKNOWN].size;
					datacnt +=size;
					ctr += size;

					/* move buffer contents back */
					for (i = size; i < OPBUF_SIZE; i++) {
						opbuf[i-size] = opbuf[i];
					}
					/* refill buffer */
					for (i = OPBUF_SIZE-size; i < OPBUF_SIZE; i++) {
						if ((temp = jologetchar(inf)) == EOF) {
							fprintf(stderr, "early file end !\n");
							exit(1);
						}
						opbuf[i] = temp;
					}
				}
				break;
			}
			else if (length==wok){	//just file end.
			}
			else{}

			//出力文字列の作成
			if(dbflg==1){
				void data_db(char *outf, long ctr, unsigned char * op ,int suujicnt);
				data_db(linebuf, ctr, opbuf ,db_cnt);

				//文字列出力
				fprintf(outf, "%s\n", linebuf);

				//データ読み進めます。
				size = db_cnt;
				ctr += size;
				datacnt +=size;
				if (length==datacnt){	//just file end.
					break;
				}

			}
			//出力文字列の作成
			else if(dwflg==1){
				void data_dw(char *outf, long ctr, unsigned char * op ,int suujicnt);
				data_dw(linebuf, ctr, opbuf ,db_cnt*2);

				//文字列出力
				fprintf(outf, "%s\n", linebuf);

				//データ読み進めます。
				size = db_cnt*2;
				ctr += size;
				datacnt +=size;
				if (length==datacnt){	//just file end.
					break;
				}

			}
			else{
				/* This line is the real 'meat' of the disassembler: */
				(*addr_info[optable[op].addr_mode].func)           /* function      */
					(linebuf, ctr, opbuf, optable[op].opname);     /* parm's passed */

				//文字列出力
				fprintf(outf, "%s\n", linebuf);

				//もとのプログラムは、lengthはライン数です
				//         length--;
				//         if (!length)
				//            break;

				//データ読み進めます。
				size = addr_info[optable[op].addr_mode].size;
				ctr += size;
				datacnt +=size;
				if (length==datacnt){	//just file end.
					break;
				}
			}

			/* move buffer contents back */
			for (i = size; i < OPBUF_SIZE; i++) {
				opbuf[i-size] = opbuf[i];
			}

			/* refill buffer */
			for (i = OPBUF_SIZE-size; i < OPBUF_SIZE; i++) {
				if ((temp = jologetchar(inf)) == EOF) {
					fprintf(stderr, "!!! early file end !\n");
					exit(1);
				}
				opbuf[i] = temp;
			}
		} /* End of while read loop */
	} /* endif Were there enough arguments? */
	return(0);  /* return value to appease compiler */
}
