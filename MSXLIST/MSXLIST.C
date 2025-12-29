
/*MSX_BASIC Dump List表示　バージョン1.0*/
/*By 1994/2025 m@3**/
/*(MSX_BASIC Copyright By Microsoft)*/
/*参考:MSX_TECHNICAL HANDBOOK & MSX Datapack vol.1*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define ESC "\x01b"
#define BUFFSIZE 32768
#define VERSION "1.0"
#define MACHINE "GCC"
#define MODE 1

/*unsigned char buff[BUFFSIZE];    */

char kana[] = "をぁぃぅぇぉゃゅょっ　あいうえおかきくけこさしすせそ";
char kana2[] = "たちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわん";
char kana3[] = "　。「」、・ヲァィゥェォャュョッーアイウエオカキクケコサシスセソ\タチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワン゛゜";

void put_title(void)
{
	printf(ESC"[32m""[MSX_BASIC DUMP Utility]\n");
	printf(ESC"[31m""[For "MACHINE" Ver " VERSION"]\n");
	printf(ESC"[33m""By 1994/2025 m@3\n"ESC"[37m");
}

int m_choice(int a)
{
	int b;
	b =(a == 2) * 3;
	return(b);
}

int main( int argc, char *argv[])
{
	unsigned char buff[BUFFSIZE];
	int inhandle, bytes;
	FILE *fp;
	unsigned char a, b, c, d;
	unsigned int j, fl, pt;
	unsigned int num;
//	unsigned 
	char *str, *fil;//, *malloc();
	int mode = 1;

	if (argc <= 1){
		put_title();
		printf("\nUsage: MSX2 filename[.bas] [-option] \n\n"
		"options:\n"
		"   -a0 Put original data\n"
		"   -a1 Put arranged data[Normal]\n", argv[0]);
		return (0);
	}
	fil = malloc(strlen(argv[1]) + 4 + 1);
	if (fil == NULL){
		printf("Out of memory\n");
		return (1);
	}
	strcpy( fil, argv[1]);
	if (strchr( argv[1], '.') == NULL){
		str = ".BAS";
		strcat( fil, str);
	}
	if (argc != 1){
		for (j=1;j<argc;j++){
			str = argv[j];
			if (*str == '-' || *str == '/'){
				str++;
				if ((*str=='A' || *str=='a') &&
				*(str+1)>='0' && *(str+1)<='2')
				mode = *(str+1)-'0';
			}
		}
	}

	if ((inhandle = open( fil, O_RDONLY | O_BINARY)) < 0){
		put_title();
		printf("Can\'t open file %s\n", fil);
		return (1);
	}

/*翻訳開始*/
	while ((bytes = read( inhandle, buff, BUFFSIZE)) > 0){
/*このwhile文はダミー（ＭＳＸ－ＢＡＳＩＣではファイルサイズが３２Ｋ未満！）*/

		fl = 0;							/*文字列か予約語かの判定用*/
		if (buff[ 0] == 0xff)			/*ＢＡＳＩＣファイルか識別*/
			buff[ 0] = 0;
		else {
			put_title();
			printf("This file is not MSX_BASIC\'s file.\n");
			return (1);
		}
		for ( j = 0; j <= bytes; j++){
			pt = 0;						/*前後の空白を表示するかのフラグ*/
			a = buff[ j];
			if(j > 1)
				b = buff[ j - 1];
			else
				b = 0;
			c = buff[ j + 1];
			if(j > 5)
				d = buff[ j - 5];			/*行頭での命令なら０が入る*/
			else
				d =0;
			str = "";
			switch( a){
				case 0 :
					fl = 0;
					j++;
					num = buff[ j]+ buff[ j + 1]*256;
					j = j + 2;
					if (num == 0){		/*テキストのＥＮＤコード*/
					printf("");
						close(inhandle);
						return (0);
						}
					num = buff[ j] + buff[ j + 1]*256;
					if ((j - 3) != 0)
						printf("\n");
					j++;
					printf("%u ", num);		/*行頭の行番号*/
					break;
				case 0x0b : printf("&O%o",buff[ j + 1]+buff[ j + 2] * 256);
							j = j + 2;			/*８進数*/
							break;
				case 0x0e :
				case 0x1c :	printf("%u",buff[ j + 1] + buff[ j + 2] * 256);
							j = j + 2;			/*１０進数２バイト*/
							break;
				case 0x1d : if (buff[j+1] > 127)
								printf("-");
							printf("(.%u%u%u%u%u%ue%d)",
							buff[j+2]/16,buff[j+2]%16,
							buff[j+3]/16,buff[j+3]%16,
							buff[j+4]/16,buff[j+4]%16,
							((buff[j+1]<<1)>>1)-64
							);

							j = j + 4;			/*単精度実数定数*/

							break;
				case 0x1f : if (buff[j+1] > 127)
								printf("-");
							printf("(.%u%u%u%u%u%u%u%u%u%u%u%u%u%ue%d)",
							buff[j+2]/16,buff[j+2]%16,
							buff[j+3]/16,buff[j+3]%16,
							buff[j+4]/16,buff[j+4]%16,
							buff[j+5]/16,buff[j+5]%16,
							buff[j+6]/16,buff[j+6]%16,
							buff[j+7]/16,buff[j+7]%16,
							buff[j+8]/16,buff[j+8]%16,
							((buff[j+1]<<1)>>1)-64
							);

							j = j + 8;			/*倍精度実数定数*/

							break;
				case 0x0d :

				case 0x0c :	printf("&H%X", buff[ j + 1] + buff[ j + 2] * 256);
							j = j + 2;			/*１６進数*/
							break;
				case 0x0f :	j++;				/*行番号（文中）*/
							printf("%u", buff[ j]);
							break;
				case 0x22 : if (fl <= 1)
								fl = fl ^ 1;	/*ダブルクウォーテーション*/
							else if (fl == 2)
									fl = 4;
							else if (fl == 4)
									fl = 2;
							break;
				case ':'  : if (fl == 2)
								fl = 0;
							if (c == 0xa1){
								str = "ELSE";
								pt = 3;
							j++;
							}
							else if (c == 0x8f
									&& buff[j + 2] == 0xe6){
								if (d == 0 || b == ':')
									str = "REM";	/*本当はアポストロフィ*/
								else
									str = ":REM";
								fl = 3;
								j = j + 2;
								if (buff[ j + 1] != ' ')
									pt = 2;
							}
							else
								printf (":");	/*マルチステートメント*/
							break;
				case 0x11 :						/*０から９の定数*/
				case 0x12 :
				case 0x13 :
				case 0x14 :
				case 0x15 :
				case 0x16 :
				case 0x17 :
				case 0x18 :
				case 0x19 :
				case 0x1a :printf("%u",a - 0x11);break;
			}

			if (a >= ' ' + (mode != 0) && a <= '~' && a != ':')
				printf ("%c", a);				/*ＡＳＣＩＩ７ビット文字*/
			else if (fl != 0){					/*文字列の処理*/
				if(a == 32)
					printf("%c", a);
				else if ((a >= 160 && a <= 221))
//					printf("%c", a);			/*カタカナ*/
					printf("%c%c", kana3[(a - 160)*2], kana3[(a - 160)*2+1]);					else if(a == 222)
					printf("゛");
				else if(a == 223)
					printf("゜");
				else if ((a >= 134 && a <= 159))
//					printf("%c", a + 32);		/*ひらがなをカタカナにする*/
					printf("%c%c", kana[(a - 134)*2], kana[(a - 134)*2+1]);		/*ひらがなをSJISにする*/
				else if ((a >= 224 && a <= 255))
//					printf("%c", a - 32);		/*同上（ＭＳＸ独自フォント）*/
					printf("%c%c", kana2[(a - 224)*2], kana2[(a - 224)*2+1]);						else if (a == 1 || a == 255 || (a >= 127 && a <= 133)){
					printf("@");				/*グラフィックキャラクタ*/
					j = j + (a == 1);
				}
			}
			else {
				switch (a){				/*１バイト命令（予約語）の処理*/
					case 0x32 :str="";break;
					case 0xee :str=">";
						if (c >= 0xee && c <= 0xf0)
							pt = (mode == 2);
						else if (b >= 0xee && b <= 0xf0)
							pt = (mode == 2) * 2;
						else
							pt = m_choice(mode);
						break;
					case 0xef :str="=";
						if (c >= 0xee && c <= 0xf0)
							pt = (mode == 2);
						else if (b >= 0xee && b <= 0xf0)
							pt = (mode == 2) * 2;
						else
							pt = m_choice(mode);
						break;
					case 0xf0 :str="<";
						if (c >= 0xee && c <= 0xf0)
							pt = (mode == 2);
						else if (b >= 0xee && b <= 0xf0)
							pt = (mode == 2) * 2;
						else
							pt = m_choice(mode);
						break;
					case 0xf1 :str="+";pt = m_choice(mode);break;
					case 0xf2 :str="-";pt = m_choice(mode);break;
					case 0xf3 :str="*";pt = m_choice(mode);break;
					case 0xf4 :str="/";pt = m_choice(mode);break;
					case 0xf5 :str="^";pt = m_choice(mode);break;
					case 0xfc :str="\\";pt = m_choice(mode);break;
					case 0xf6 :str="AND";pt = 1;
						if ( c != 2)
							pt = pt | 2;
						break;
					case 0xe9 :str="ATTR$";break;
					case 0xa9 :str="AUTO";pt = 2;break;
					case 0xc9 :str="BASE";break;
					case 0xc0 :str="BEEP";break;
					case 0xcf :str="BLOAD";pt = 2;break;
					case 0xd0 :str="BSAVE";pt = 2;break;
					case 0xca :str="CALL";pt = 2;break;
					case 0xbc :str="CIRCLE";pt = 2;break;
					case 0x92 :str="CLEAR";pt = 2;break;
					case 0x9b :str="CLOAD";pt = 2;break;
					case 0xb4 :str="CLOSE";pt = 2;break;
					case 0x9f :str="CLS";break;
					case 0xd7 :str="CMD";pt = 2;break;
					case 0xbd :str="COLOR";
						if ( c != 0xef && c != 0x3a)
							pt = 2;
						break;
					case 0x99 :str="CONT";break;
					case 0xd6 :str="COPY";pt = 2;break;
					case 0x9a :str="CSAVE";pt = 2;break;
					case 0xe8 :str="CSRLIN";pt = 2;break;
					case 0x84 :str="DATA";
						fl = 2;
						if ( c != ' ')
							pt = 2;
						break;
					case 0x97 :str="DEF";pt = 2;break;
					case 0xae :str="DEFDBL";pt = 2;break;
					case 0xac :str="DEFINT";pt = 2;break;
					case 0xad :str="DEFSNG";pt = 2;break;
					case 0xab :str="DEFSTR";pt = 2;break;
					case 0xa8 :str="DELETE";pt = 2;break;
					case 0x86 :str="DIM";pt = 2;break;
					case 0xbe :str="DRAW";pt = 2;break;
					case 0xea :str="DSKI$";pt = 2;break;
					case 0xd1 :str="DSKO$";pt = 2;break;
					case 0x81 :str="END";break;
					case 0xf9 :str="EQV";pt = 3;break;
					case 0xa5 :str="ERASE";pt = 2;break;
					case 0xe1 :str="ERL";break;
					case 0xe2 :str="ERR";break;
					case 0xa6 :str="ERROR";pt = 2;break;
					case 0xb1 :str="FILED";pt = 2;break;
					case 0xb7 :str="FILES";
						if (b != ':')
							pt = 2;
						break;
					case 0xde :str="FN";pt = 2;break;
					case 0x82 :str="FOR";pt = 2;
						if (b != ':'  && b != ' ' &&
							b != 0xb0 && d != 0)
							pt = pt | 1;
						break;
					case 0xb2 :str="GET";pt = 2;break;
					case 0x8d :str="GOSUB";
						if (b == ':'  || d == 0 ||
							b == 0xda || b == 0xa1)
							pt = 2;
						else
							pt = 3;
						break;
					case 0x89 :str="GOTO";
						if (b == 0x3a || b == ' '  || d == 0 ||
							b == 0xda || b == 0xa6 || b == 0xa1)
							pt = 2;
						else
							pt = 3;
						break;
					case 0x8b :str="IF";pt = 2;break;
					case 0xfa :str="IMP";pt = 3;break;
					case 0xec :str="INKEY$";break;
					case 0x85 :str="INPUT";
						if(c != '$')
							pt = 2;
						break;
					case 0xe5 :str="INSTR";break;
					case 0xd5 :str="IPL";pt = 2;break;
					case 0xcc :str="KEY";break;
					case 0xd4 :str="KILL";pt = 2;break;
					case 0x88 :str="LET";pt = 2;break;
					case 0xbb :str="LFILES";pt = 2;break;
					case 0xaf :str="LINE";pt = 2;break;
					case 0x93 :str="LIST";pt = 2;break;
					case 0x9e :str="LLIST";pt = 2;break;
					case 0xb5 :str="LOAD";pt = 2;break;
					case 0xd8 :str="LOCATE";pt = 2;break;
					case 0x9d :str="LPRINT";pt = 2;
						break;
					case 0xb8 :str="LSET";pt = 2;break;
					case 0xcd :str="MAX";break;
					case 0xb6 :str="MERGE";pt = 2;break;
					case 0xfb :str="MOD";pt = 3;break;
					case 0xce :str="MOTOR";pt = 2;break;
					case 0xd3 :str="NAME";pt = 2;break;
					case 0x94 :str="NEW";break;
					case 0x83 :str="NEXT";
						if (c != ':')
							pt = 2;
						break;
					case 0xe0 :str="NOT";pt = 3;break;
					case 0xeb :str="OFF";
						if (b != ':' && d != 0)
							pt = 1;
						if (b != ':')
							pt = pt | 2;
						break;
					case 0x95 :str="ON";
						if (b != ':' && d != 0)
							pt = 1;
						if (c != ':')
							pt = pt | 2;
						break;
					case 0xb0 :str="OPEN";pt = 2;break;
					case 0xf7 :str="OR";pt = 1;
						if (c != ':')
							pt = pt | 2;
						break;
					case 0x9c :str="OUT";pt = 2;break;
					case 0xbf :str="PAINT";pt = 2;break;
					case 0xc1 :str="PLAY";pt = 2;break;
					case 0xed :str="POINT";break;
					case 0x98 :str="POKE";pt = 2;break;
					case 0xc3 :str="PRESET";
						if (c != ':')
							pt = 2;
						break;
					case 0x91 :str="PRINT";pt = 2;
						break;
					case 0xc2 :str="PSET";
						if (c != ':')
							pt = 2;
						break;
					case 0xb3 :str="PUT";pt = 2;break;
					case 0x87 :str="READ";pt = 2;break;
					case 0x8f :str="REM";
						fl = 3;
						if (c != ' ')
							pt = 2;
						break;
					case 0xaa :str="RENUM";pt = 2;break;
					case 0x8c :str="RESTORE";pt = 2;break;
					case 0xa7 :str="RESUME";
						if (c == 0x0d || c == 0x0e || c == ' ')
							pt = 2;
						break;
					case 0x8e :str="RETURN";
						if (c == 0x0d || c == 0x0e ||
							c == ' ')
							pt = 2;
						break;
					case 0xb9 :str="RSET";pt = 2;break;
					case 0x8a :str="RUN";pt = 2;break;
					case 0xba :str="SAVE";pt = 2;break;
					case 0xc5 :str="SCREEN";pt = 2;break;
					case 0xd2 :str="SET";pt = 2;break;
					case 0xc4 :str="SOUND";pt = 2;break;
					case 0xdf :str="SPC(";break;
					case 0xc7 :str="SPRITE";break;
					case 0xdc :str="STEP";
						if (b != 0xf2)
							pt = 3;
						break;
					case 0x90 :str="STOP";break;
					case 0xe3 :str="STRING$";break;
					case 0xa4 :str="SWAP";pt = 2;break;
					case 0xdb :str="TAB(";break;
					case 0xda :str="THEN";pt = 3;break;
					case 0xcb :str="TIME";break;
					case 0xd9 :str="TO";pt = 3;break;
					case 0xa3 :str="TROFF";break;
					case 0xa2 :str="TRON";break;
					case 0xe4 :str="USING";pt = 2;break;
					case 0xdd :str="USR";break;
					case 0xe7 :str="VARPTR";break;
					case 0xc8 :str="VDP";break;
					case 0xc6 :str="VPOKE";pt = 2;break;
					case 0x96 :str="WAIT";break;
					case 0xa0 :str="WIDTH";pt = 2;break;
					case 0xf8 :str="XOR";pt = 1;
						if (c != ':')
							pt = pt | 2;
						break;
					case 0xff :				/*２バイト命令の処理*/
						j++;
						switch (buff[j]){
							case 0x86 :str="ABS";break;
							case 0x95 :str="ASC";break;
							case 0x8e :str="ATN";break;
							case 0xa0 :str="CDBL";break;
							case 0x96 :str="CHR$";break;
							case 0x9e :str="CINT";break;
							case 0x8c :str="COS";break;
							case 0x9f :str="CSNG";break;
							case 0xaa :str="CVD";break;
							case 0xa8 :str="CBI";break;
							case 0xa9 :str="CVS";break;
							case 0xa6 :str="DSKF";break;
							case 0xab :str="EOF";break;
							case 0x8b :str="EXP";break;
							case 0xa1 :str="FIX";break;
							case 0xa7 :str="FPOS";break;
							case 0x8f :str="FRE";break;
							case 0x9b :str="HEX$";break;
							case 0x90 :str="INP";break;
							case 0x85 :str="INT";break;
							case 0x81 :str="LEFT$";break;
							case 0x92 :str="LEN";break;
							case 0xac :str="LOC";break;
							case 0xad :str="LOF";break;
							case 0x8a :str="LOG";break;
							case 0x9c :str="LPOS";break;
							case 0x83 :str="MID$";break;
							case 0xb0 :str="MKD$";break;
							case 0xae :str="MKI$";break;
							case 0xaf :str="MKS$";break;
							case 0x9a :str="OCT$";break;
							case 0xa5 :str="PAD";break;
							case 0xa4 :str="PDL";break;
							case 0x97 :str="PEEK";break;
							case 0x91 :str="POS";break;
							case 0x82 :str="RIGHT$";break;
							case 0x88 :str="RND";break;
							case 0x84 :str="SGN";break;
							case 0x89 :str="SIN";break;
							case 0x99 :str="SPACE$";break;
							case 0x87 :str="SQR";break;
							case 0xa2 :str="STICK";break;
							case 0x93 :str="STR$";break;
							case 0xa3 :str="STRIG";break;
							case 0x8d :str="TAN";break;
							case 0x94 :str="VAL";break;
							case 0x98 :str="VPEEK";break;
							default   :str="?";
						}
						break;
				}
			}
			if ((pt == 1 || pt == 3) && mode != 0)
				printf(" ");
			while (*str != '\0'){
				printf("%c", *str);			/*文字列の表示*/
				str++;
			}
		if ((pt == 2 || pt == 3) && mode != 0)
			printf(" ");
		}
	}
	close(inhandle);
}
/*(switch~caseの嵐)*/
