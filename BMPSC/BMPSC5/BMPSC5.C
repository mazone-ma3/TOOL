/* Bitmap(16 colors) to MSX-SC5 Converter By m@3 */
/* gcc、clang等でコンパイルして下さい */

#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>

#define ERROR 1
#define NOERROR 0

#define MSX_WIDTH 256
#define MSX_HEIGHT 212

//#define MAXCOLOR 16

FILE *stream[2];

long i, j, k, width, height, filesize, headersize, datasize, filesize, headersize;

unsigned char read_pattern[MSX_WIDTH * MSX_HEIGHT + 2];
unsigned char write_pattern[MSX_WIDTH * MSX_HEIGHT + 2];
unsigned char pattern[10+1];

int conv(char *bitmapfil, char *sc5fil)
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
	width = read_pattern[0x0] + 256 * (read_pattern[0x1] + 256 * (read_pattern[0x2]  +  256 * read_pattern[0x3]));
	fread(read_pattern, 1, 4, stream[0]);	/* 縦幅 */
	height = read_pattern[0x0] + 256 * (read_pattern[0x1] + 256 * (read_pattern[0x2]  +  256 * read_pattern[0x3]));
	if((width > MSX_WIDTH) || (height > MSX_HEIGHT)){
		fclose(stream[0]);
		printf("Size over file %s.", bitmapfil);
		return ERROR;
	}else{
		printf("X Size %d / Y Size %d", width, height);
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
	printf("\nDataSize %d / Data Width %d", datasize, width);

	fread(read_pattern, 1, width / 2 * height, stream[0]);	/* ピクセルデータ */
	k = 0;
	for(j = 0; j < height; ++j){
		for(i = 0; i < (width / 2); ++i){
			write_pattern[i + (height - j - 1) * (MSX_WIDTH / 2)] = read_pattern[k++];
		}
	}

	fclose(stream[0]);


	if ((stream[1] = fopen( sc5fil, "wb")) == NULL) {
		printf("Can\'t open file %s.", sc5fil);
		return ERROR;
	}

	pattern[0] = 0xfe;
	fwrite(pattern, 1, 1, stream[1]);	/* MSX先頭ヘッダ */

	pattern[0] = 0;
	pattern[1] = 0;
	fwrite(pattern, 1, 2, stream[1]);	/* MSXヘッダ 開始アドレス */

	pattern[0] = (MSX_WIDTH / 2 * MSX_HEIGHT) % 256;
	pattern[1] = (MSX_WIDTH / 2 * MSX_HEIGHT) / 256;
	fwrite(pattern, 1, 2, stream[1]);	/* MSXヘッダ 終了アドレス */

	pattern[0] = 0;
	pattern[1] = 0;
	fwrite(pattern, 1, 2, stream[1]);	/* MSXヘッダ 0 */

	fwrite(write_pattern, MSX_WIDTH / 2 * MSX_HEIGHT, 1, stream[1]);

	fclose(stream[1]);

	return NOERROR;
}

int	main(int argc,char **argv)
{
	if (argc < 3){
		printf("Bitmap 16colors to MSX .SC5 file Converter.\n");
		return ERROR;
	}

	if(conv(argv[1], argv[2]))
		return ERROR;

	return NOERROR;
}
