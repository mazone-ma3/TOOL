/* Bitmap16(4)colors to AMIGA SPRITE(C format) Converter By m@3 */
/* gcc、clang等でコンパイルして下さい */

#include <stdio.h>
#include <stdlib.h>

/************************************************************************/
/*		BIT操作マクロ定義												*/
/************************************************************************/

/* BITデータ算出 */
#define BITDATA(n) (1 << (n))

/* BITセット */
#define BITSET(BITNUM, NUMERIC) {	\
	NUMERIC |= BITDATA(BITNUM);		\
}

/* BITクリア */
#define BITCLR(BITNUM, NUMERIC) {	\
	NUMERIC &= ~BITDATA(BITNUM);	\
}

/* BITチェック */
#define BITTST(BITNUM, NUMERIC) (NUMERIC & BITDATA(BITNUM))

/* BIT反転 */
#define BITNOT(BITNUM, NUMERIC) {	\
	NUMERIC ^= BITDATA(BITNUM);		\
}

#define ERROR 1
#define NOERROR 0

#define AMIGA_WIDTH 16
#define AMIGA_HEIGHT 256

unsigned char conv_tbl[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0 ,0 ,0 ,0 ,0 };

FILE *stream[2];

long i, j, k, l, m, write_size, header;
long AMIGAwidth, width, height, filesize, headersize, datasize;

unsigned char read_pattern[AMIGA_WIDTH * AMIGA_HEIGHT + 2];
unsigned char write_pattern[AMIGA_WIDTH * AMIGA_HEIGHT + 2];
unsigned char pattern[10+1];

unsigned char grp_buffer[AMIGA_WIDTH][AMIGA_HEIGHT];

int conv(char *bitmapfil, int argc)
{
	if ((stream[0] = fopen( bitmapfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", bitmapfil);
		return ERROR;
	}

	fread(read_pattern, 1, 2, stream[0]);	/* Bitmapヘッダ */
	if((read_pattern[0] != 'B') || (read_pattern[1] != 'M')){
		printf("Not Bitmap file %s.", bitmapfil);
		fclose(stream[0]);
		return ERROR;
	}
	fread(read_pattern, 1, 4, stream[0]);	/* ファイルサイズ */
	filesize = read_pattern[0x0] + 256 * (read_pattern[0x1] + 256 * (read_pattern[0x2]  +  256 * read_pattern[0x3]));

	fread(read_pattern, 1, 4, stream[0]);	/* 予約 */
	fread(read_pattern, 1, 4, stream[0]);	/* 画像データの開始番地 0x76*/
	fread(read_pattern, 1, 4, stream[0]);	/* ヘッダサイズbyte 40(0x28) */
	headersize = read_pattern[0x0] + 256 * (read_pattern[0x1] + 256 * (read_pattern[0x2]  +  256 * read_pattern[0x3]));

	fread(read_pattern, 1, 4, stream[0]);	/* 横幅 */
	AMIGAwidth = read_pattern[0x0] + 256 * (read_pattern[0x1] + 256 * (read_pattern[0x2]  +  256 * read_pattern[0x3]));
	fread(read_pattern, 1, 4, stream[0]);	/* 縦幅 */
	height = read_pattern[0x0] + 256 * (read_pattern[0x1] + 256 * (read_pattern[0x2]  +  256 * read_pattern[0x3]));
	if((AMIGAwidth > AMIGA_WIDTH) || (height > (AMIGA_HEIGHT))){
		fclose(stream[0]);
		printf("Size over file %s.", bitmapfil);
		return ERROR;
	}else{
//		printf("X Size %d / Y Size %d", AMIGAwidth, height);
	}

	fread(read_pattern, 1, 2, stream[0]);	/* 面の数 1 */
	fread(read_pattern, 1, 2, stream[0]);	/* 1ピクセル当たりのビット数 4 */
	if((read_pattern[0x0] != 0x04) || (read_pattern[0x1] != 0x00)){
		printf("Not 16colors file %s.", bitmapfil);
		fclose(stream[0]);
		return ERROR;
	}

	fread(read_pattern, 1, 4, stream[0]);	/* 圧縮形式 0 */
	fread(read_pattern, 1, 4, stream[0]);	/* 画像のデータサイズ */
//	datasize = read_pattern[0x0] + 256 * (read_pattern[0x1] + 256 * (read_pattern[0x2]  +  256 * read_pattern[0x3]));

	fread(read_pattern, 1, 4, stream[0]);	/* 1mあたりのピクセル数 横 */
	fread(read_pattern, 1, 4, stream[0]);	/* 1mあたりのピクセル数 縦 */

	fread(read_pattern, 1, 4, stream[0]);	/* カラーテーブル */
	fread(read_pattern, 1, 4, stream[0]);	/* カラーインデックス */
	fread(read_pattern, 1, 4*16, stream[0]);	/* カラーバレット x 16 */

	datasize = filesize - headersize - 4 - 4 - 16 * 4;
	width = (datasize / height) * 2;
//	printf("\nDataSize %d / Data Width %d", datasize, width);

	fread(read_pattern, 1, width / 2 * height, stream[0]);	/* ピクセルデータ */

	fclose(stream[0]);

///////////////////////////////////////////////////////////////////////////////

	k = 0;
	for(j = 0; j < height; ++j){
		for(i = 0; i < (width / 2); ++i){
//			if((j % 2)){
//			write_pattern[i + (height - j - 1) * (AMIGA_WIDTH / 2)] = read_pattern[k++];
				grp_buffer[i * 2 + 1][height / 1 - 1 - (j / 1)] = conv_tbl[read_pattern[k] % 16];
				grp_buffer[i * 2 + 0][height / 1 - 1 - (j / 1)] = conv_tbl[read_pattern[k] / 16];
//			}
			k++;
		}
	}
///////////////////////////////////////////////////////////////////////////////
	unsigned char color, data, n;

	for(j = 0; j < height; ++j){
		printf("\t");
		for(n = 0; n < 2; ++n){
			printf(" 0x");
			for(i = 0; i < AMIGAwidth / 8; ++i){
				data = 0;
				for(m = 0; m < 8; ++m){
					color = grp_buffer[i * 8 + m][j];
					if(BITTST(n, color))
						BITSET(7 - m, data);
				}
//				printf("%.1x", color);
				printf("%.2x", data);
			}
//			printf(" 0x%.2x,", data);
			printf(",");
		}
		printf("\n");
	}

	if (argc > 2){
		printf("\n");
		for(j = 0; j < height; ++j){
			printf("\t");
			for(n = 0; n < 2; ++n){
				printf(" 0x");
				for(i = 0; i < AMIGAwidth / 8; ++i){
					data = 0;
					for(m = 0; m < 8; ++m){
						color = grp_buffer[i * 8 + m][j];
						if(BITTST(n + 2, color))
							BITSET(7 - m, data);
					}
//					printf("%.1x", color);
					printf("%.2x", data);
				}
//				printf(" 0x%.2x,", data);
				printf(",");
			}
			printf("\n");
		}
	}

	return NOERROR;
}

int	main(int argc,char **argv)
{
	if (argc < 2){
		printf("Bitmap 4colors to AMIGA SPRITE(C format) Converter.\n");
		return ERROR;
	}

	if(conv(argv[1], argc))
		return ERROR;

	return NOERROR;
}
