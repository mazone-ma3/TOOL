/* .sc5->16colorBitmap convert 256x212 fixed auto palette */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#pragma pack(push, 1)
typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} BMPFileHeader;

typedef struct {
	uint32_t biSize;
	int32_t  biWidth;
	int32_t  biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t  biXPelsPerMeter;
	int32_t  biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BMPInfoHeader;

typedef struct {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t reserved;
} BMPPalette;
#pragma pack(pop)

#define MAXCOLOR 16

/* R G B */
unsigned char org_pal[MAXCOLOR][3] = {
	{  0,  0,  0},
	{  0,  0,  0},
	{  3, 13,  3},
	{  7, 15,  7},
	{  3,  3, 15},
	{  5,  7, 15},
	{ 11,  3,  3},
	{  5, 13, 15},
	{ 15,  3,  3},
	{ 15,  7,  7},
	{ 13, 13,  3},
	{ 13, 13,  9},
	{  3,  9,  3},
	{ 13,  5, 11},
	{ 11, 11, 11},
	{ 15, 15, 15},
};

long GetFileSize(const char* filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1; // ファイルが開けなかった場合
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp); // ftell()でも終端位置（サイズ）を取得可能
    fclose(fp);
    return size;
}

int main(int argc, char *argv[]) {
	BMPPalette bmp_pal[16] = {0};

	if (argc < 3) {
		printf("Usage: %s <input.sc5> <output.bmp>\n", argv[0]);
		return 1;
	}

	FILE *fin = fopen(argv[1], "rb");
	if (!fin) {
		perror("Input file error");
		return 1;
	}

	// 1. BSAVEヘッダー(7バイト)をスキップ
	fseek(fin, 7, SEEK_SET);

	// 2. 画像データ読み込み (212ライン x 128バイト)
	uint8_t img_data[212][128];
	fread(img_data, 1, 212 * 128, fin);

	fseek(fin, 0, SEEK_END);
//	printf("%x",ftell(fin));
	// 3. パレットデータ読み込み (オフセット 0x7680 + BSAVEヘッダー7バイト)
	if(ftell(fin) > (7 + 0x7680)){
		fseek(fin, 7 + 0x7680, SEEK_SET);
		for (int i = 0; i < 16; i++) {
			uint8_t p1 = fgetc(fin);
			uint8_t p2 = fgetc(fin);
			double r, g, b;
			// MSX: 0RRR0BBB 00000GGG (0-7) -> BMP: 0-255
			r = (p1 >> 4) & 0x07;
			b = p1 & 0x07;
			g = p2 & 0x07;
//			r = (r) ? ((r + 1) << 5) - 1 : 0;
//			g = (g) ? ((g + 1) << 5) - 1 : 0;
//			b = (b) ? ((b + 1) << 5) - 1 : 0;
			r = (r * 255) / 7;
			g = (g * 255) / 7;
			b = (b * 255) / 7;
			bmp_pal[i].red = round(r);
			bmp_pal[i].blue = round(b);
			bmp_pal[i].green = round(g);
			bmp_pal[i].reserved = 0;
		}
	}else{
		// 標準パレット 0-15(4bit) -> BMP:0-255(8bit)
		for (int i = 0; i < 16; i++) {
			double r, g, b;
			r = org_pal[i][0];
			g = org_pal[i][1];
			b = org_pal[i][2];
//			r = (r) ? ((r + 1) << 4) - 1 : 0;
//			g = (g) ? ((g + 1) << 4) - 1 : 0;
//			b = (b) ? ((b + 1) << 4) - 1 : 0;
			r = (r * 255) / 15;
			g = (g * 255) / 15;
			b = (b * 255) / 15;
			bmp_pal[i].red = round(r);
			bmp_pal[i].blue = round(b);
			bmp_pal[i].green = round(g);
			bmp_pal[i].reserved = 0;
		}
	}
	fclose(fin);

	// 4. BMPファイル書き出し準備
	FILE *fout = fopen(argv[2], "wb");
	BMPFileHeader bfh = {0x4D42, sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(bmp_pal) + (212 * 128), 0, 0, sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(bmp_pal)};
	BMPInfoHeader bih = {sizeof(BMPInfoHeader), 256, 212, 1, 4, 0, 212 * 128, 0, 0, 16, 16};

	fwrite(&bfh, sizeof(bfh), 1, fout);
	fwrite(&bih, sizeof(bih), 1, fout);
	fwrite(bmp_pal, sizeof(BMPPalette), 16, fout);

	// BMPは下から上の順に格納する必要がある
	for (int y = 211; y >= 0; y--) {
		fwrite(img_data[y], 1, 128, fout);
	}

	fclose(fout);
	printf("Successfully converted %s to %s\n", argv[1], argv[2]);
	return 0;
}