/* MSX-SC5->CHR88.BAS CONV. for GCC */
/* �E������ */

#include <stdio.h>

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

#define MSXWIDTH 256
#define MSXLINE 32 //212
#define X1WIDTH (640/8)
#define X1LINE 400
#define PCGSIZEX 80 //1
#define PCGSIZEY 32 //8
#define PLANE 3
#define PCGPARTS 1 //256 * 8 / 64

#define RAM_ADR 0x2000

FILE *stream[2];

unsigned char conv_tbl[16] = { 0, 0, 4, 4, 1, 1, 2, 5, 2, 2, 6, 6 ,4 ,3 ,7 ,7 };

int conv(char *loadfil, char*savefil)
{

	long i, j,k,y, x, xx, yy, no, max_xx;
	unsigned char pattern[PCGSIZEX*PCGSIZEY*PCGPARTS*PLANE+1 + 4];
	unsigned char x1color[X1WIDTH][X1LINE][PLANE];
	unsigned char msxcolor[MSXWIDTH * 2][MSXLINE * 2];
	unsigned char color;

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", loadfil);

		fclose(stream[0]);
		return 1;
	}
	if ((stream[1] = fopen( savefil, "wb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", savefil);

		fclose(stream[1]);
		return 1;
	}

//	header = RAM_ADR + WIDTH * LINE * 3;//0x5e00(����);

	fread(pattern, 1, 1, stream[0]);	/* MSX�擪��ǂݎ̂Ă� */
	fread(pattern, 1, 4, stream[0]);	/* MSX�w�b�_���ǂݎ̂Ă� */
/*
	pattern[0] = RAM_ADR % 256;
	pattern[1] = RAM_ADR / 256;
	pattern[2] = header % 256;
	pattern[3] = header / 256;
*/
	fread(pattern, 1, 2, stream[0]);	/* MSX�w�b�_��ǂݎ̂Ă� */

	for(y = 0; y < MSXLINE * 2; ++y){
		for(x = 0; x < MSXWIDTH*2 ; ++x){
			msxcolor[x][y] = 0;
		}
	}

	i = fread(pattern, 1, MSXWIDTH / 4, stream[0]);	/* �ǂݎ̂Ă� */

	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 2; ++x){
			i = fread(pattern, 1, 1, stream[0]);	/* 2dot�� */
			if(i < 1)
				break;

			/* �F�����Ɗg�� */
			msxcolor[0 + x * 4][y] = (pattern[0] >>4) & 0x0f;
			msxcolor[1 + x * 4][y] = (pattern[0] >>4) & 0x0f;
			msxcolor[2 + x * 4][y] = pattern[0] & 0x0f;
			msxcolor[3 + x * 4][y] = pattern[0] & 0x0f;
		}
	}


	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 2; ++x){
			i = fread(pattern, 1, 1, stream[0]);	/* 2dot�� */
			if(i < 1)
				break;

			/* �F�����Ɗg�� */
			msxcolor[0 + x * 4 + 256][y] = (pattern[0] >>4) & 0x0f;
			msxcolor[1 + x * 4 + 256][y] = (pattern[0] >>4) & 0x0f;
			msxcolor[2 + x * 4 + 256][y] = pattern[0] & 0x0f;
			msxcolor[3 + x * 4 + 256][y] = pattern[0] & 0x0f;
		}
	}
	fclose(stream[0]);
//	printf("MSX file Closed.\n");

	for(y = 0; y < X1LINE; ++y){
		for(x = 0; x < X1WIDTH ; ++x){
			for(i = 0; i < PLANE; ++i){
				x1color[x][y][i] = 0;
			}
		}
	}

	max_xx =0 ;
	for(y = 0; y < X1LINE; ++y){
		xx = 0;
		j = 0;
		/* MSX2�̉�ʔ��������ۑ����� */
		for(x = 0; x < MSXWIDTH * 2; ++x){
			color = conv_tbl[msxcolor[x][y]];	/* �F�ϊ� */

			for(i = 0; i < PLANE; ++i){
				if(BITTST(i, color)){
					BITSET(7-j, x1color[xx][y][i]);
				}else{
					BITCLR(7-j, x1color[xx][y][i]);
				}
			}

			++j;
			if(j == 8){
				++xx;
				j = 0;
				if(max_xx < x){
					max_xx = xx;
//					printf("max width = %d \n",max_xx);
				}
			}
		}
	}
	printf("max width = %d \n",max_xx);
//	printf("X1 Convert Done.\n");

	j = 4; //0;
//	for(i = 0; i < PLANE; ++i){		/* 3 plane */
		xx=0;
		yy=0;
		for(no = 0; no < PCGPARTS; ++no){	/* 256 blocks */
			printf("\nno =%d ",no);

			for(y = 0; y < PCGSIZEY; ++y){			/* 1*8dot*8line pcg */

	for(i = 0; i < PLANE; ++i){		/* 3 plane */

				for(x = 0; x < PCGSIZEX ; ++x){

//					if((x+xx) >= max_xx) { //(X1WIDTH)){
//						xx=0;
//						yy+=PCGSIZEY;
//					}

					printf("x=%d y=%d ",x+xx, y+yy);

					pattern[j++] = x1color[x + xx][y + yy][i];
//					printf("%dbytes\n",++j);
				}

	}

			}
//			xx+=PCGSIZEX;
		}
//	}

	printf("%d Bytes.\n",j);

	pattern[0] = RAM_ADR % 256;
	pattern[1] = RAM_ADR / 256;
	pattern[2] = (j - 4) % 256;
	pattern[3] = (j - 4) / 256;
//	fwrite(pattern, 1, 4, stream[1]);	/* 88�w�b�_������ */

	i = fwrite(pattern, 1, j, stream[1]);	/* 8dot��*2 */
/*	if(i < 1){
		break;
	}*/
	fclose(stream[1]);

	return 0;
}


int	main(int argc,char **argv){

	if (argv[1] == NULL)
		return 1;
	if (argv[2] == NULL)
		return 1;

	conv(argv[1], argv[2]);

	return 0;
}
