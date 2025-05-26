/* PC-98 Screen to BSAVE CONV.(Analog 16 colors) for GCC-ia16 By m@3 */
/* �S�̔� */

#define _BORLANDC_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <libi86/string.h>

#define ON 1
#define OFF 0

#define BUFFSIZE 16384

/************************************************************************/
/*		BIT����}�N����`												*/
/************************************************************************/

/* BIT�f�[�^�Z�o */
#define BITDATA(n) (1 << (n))

/* BIT�Z�b�g */
#define BITSET(BITNUM, NUMERIC) {	\
	NUMERIC |= BITDATA(BITNUM);		\
}

/* BIT�N���A */
#define BITCLR(BITNUM, NUMERIC) {	\
	NUMERIC &= ~BITDATA(BITNUM);	\
}

/* BIT�`�F�b�N */
#define BITTST(BITNUM, NUMERIC) (NUMERIC & BITDATA(BITNUM))

/* BIT���] */
#define BITNOT(BITNUM, NUMERIC) {	\
	NUMERIC ^= BITDATA(BITNUM);		\
}

unsigned char __far *flame[4]
	 = {MK_FP(0xa800,0)	,MK_FP(0xb000,0),MK_FP(0xb800,0),MK_FP(0xe000,0)};

FILE *stream[2];

void cursor_switch(short);
void screen_switch(short);


int conv(char *savefil, unsigned char __far *flame)
{
	long i;
	int k=0, l=0;
	unsigned char pattern[BUFFSIZE];
	long size, write_size;

	if ((stream[0] = fopen( savefil, "wb")) == NULL) {
		printf("Can\'t open file %s.\n", savefil);

		fclose(stream[0]);
		return 1;
	}
	size = 0x8000;
	write_size = BUFFSIZE;
	while(size){
		if(size > BUFFSIZE){
			size -= BUFFSIZE;
		}else{
			write_size = size;
			size = 0;
		}
		_fmemcpy(pattern, flame, write_size);
		fwrite(pattern, 1, write_size, stream[0]);
		flame += write_size;
	}
//	fwrite(flame, 1, 0x4000, stream[0]);
	fwrite("\x1a", 1, 1, stream[0]);
	fclose(stream[0]);

	return 0;
}

void g_init(void)
{
	_disable();
	outportb(0x6a, 1   ); /* 16�F���[�h (0x6a=mode f/fp2)*/
	outportb(0xa2, 0x4b);
	outportb(0xa0, 0); /* L/R = 1 (�c�����̊g��W��)*/
	outportb(0x68, 8   ); /* ���[�hF/F1 (8�ō��𑜓x)*/
	outportb(0x4a0,0xfff0);
	outportb(0x7c, 0);
	_enable();
}

/*�I������*/
void end()
{
}

/*�J�[�\���y�уt�@���N�V�����L�[�\���̐���*/
void cursor_switch(short mode)
{
	if(mode)
		printf("\x1b[>1l\x1b[>5l");
	else
		printf("\x1b*\x1b[>1h\x1b[>5h");
}

void screen_switch(short mode)
{
	if(mode)
		outportb(0xa2, 0x0d); /* �\���J�n */
	else
		outportb(0xa2, 0x0c);
}


/*�y�[�W�؂�ւ�*/
void setpage(short visual, short active)
{
	outportb(0xa4, visual);
	outportb(0xa6, active);
}

/*�p���b�g�E�Z�b�g*/
void pal_set(short color, unsigned char red, unsigned char green,
	unsigned char blue)
{
		outportb(0xa8, color);
		outportb(0xaa, green);
		outportb(0xac, red);
		outportb(0xae, blue);
}

void pal_all(unsigned char color[16][3])
{
	short i;
	for(i = 0; i < 16; i++)
		pal_set(i, color[i][0], color[i][1], color[i][2]);
}


int	main(int argc,char **argv){

	char dst[100];

	if ((argv[1] == NULL) || (argc < 2)){
		printf("Screen to BSAVE Converter for PC-98.\n");
		return 1;
	}

	screen_switch(OFF);
	g_init();
	setpage(0,0);
	screen_switch(ON);

	snprintf(dst, sizeof dst, "%s.grb", argv[1]);
	conv(dst, flame[0]);
	snprintf(dst, sizeof dst, "%s.grr", argv[1]);
	conv(dst, flame[1]);
	snprintf(dst, sizeof dst, "%s.grg", argv[1]);
	conv(dst, flame[2]);
	snprintf(dst, sizeof dst, "%s.gri", argv[1]);
	conv(dst, flame[3]);

	end();

	return 0;
}
