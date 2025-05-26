/* PC-88 MUCOM88呼び出し実験 By m@3 */
/* .COM版 */
/* music2ファイルと演奏ファイルを用意してください */
/* ZSDCC版 */

#include <stdio.h>

FILE *stream[2];

#define ERROR 1
#define NOERROR 0

void DI(void){
__asm
	DI
__endasm;
}

void EI(void){
__asm
	EI
__endasm;
}

short bload(char *loadfil)
{
	unsigned short size;
	unsigned short *address;
	unsigned char buffer[2];

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);
		return ERROR;
	}
	fread( buffer, 1, 2, stream[0]);
	address = (unsigned short *)(buffer[0] + buffer[1] * 256);
	fread( buffer, 1, 2, stream[0]);
	size = (buffer[0] + buffer[1] * 256) - (unsigned short)address;
	printf("Load file %s. Address %x Size %x End %x\n", loadfil, address, size, (unsigned short)address + size);

	fread( address, 1, size, stream[0]);
	fclose(stream[0]);
	return NOERROR;
}

void play_bgm(void)
{
__asm
	call #0xb000
__endasm;
}

void stop_bgm(void)
{
__asm
	call #0xb003
__endasm;
}

void fade_bgm(void)
{
__asm
	call #0xb006
__endasm;
}



int	main(int argc,char **argv)
{
	if (argc < 2){
		printf("MUCOM Loader.\n");
		return ERROR;
	}

	if(bload("music2") == ERROR)
		return ERROR;

	if(bload(argv[1]) == ERROR)
		return ERROR;

	EI();

	play_bgm();
	getchar();
	stop_bgm();

	return NOERROR;
}

