/* 16colorBitmap->.sc5 convert 256x212 fixed with palette */

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

void convert_palette(BMPPalette *in, uint8_t *out) {
	// MSX Palette: 0RRR0BBB 00000GGG (各4bit、実際は上位3bitを使用)
	// BMP(0-255) -> MSX(0-7)
	uint8_t r, g, b;
//	r = (in->red) ? ((in->red+1) >> 5) - 1 : 0;
//	g = (in->green) ? ((in->green+1) >> 5) - 1: 0;
//	b = (in->blue) ?  ((in->blue+1) >> 5) - 1 : 0;
	r = round((double)in->red * 7 / 255);
	g = round((double)in->green * 7 / 255);
	b = round((double)in->blue * 7 / 255);

	out[0] = (r << 4) | b;
	out[1] = g;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s <input.bmp> <output.sc5>\n", argv[0]);
		return 1;
	}

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) return 1;

	BMPFileHeader bfh;
	BMPInfoHeader bih;
	fread(&bfh, sizeof(bfh), 1, fp);
	fread(&bih, sizeof(bih), 1, fp);

	if (bih.biBitCount != 4) {
		fprintf(stderr, "Error: Only 4-bit (16 colors) BMP is supported.\n");
		fclose(fp);
		return 1;
	}

	// パレット読み込み
	BMPPalette bmp_pal[16];
	fread(bmp_pal, sizeof(BMPPalette), 16, fp);

	// 画像データ読み込み (BMPは下から上に向かって記録されている点に注意)
	uint8_t img_data[212][128]; // 256px / 2 = 128bytes
	fseek(fp, bfh.bfOffBits, SEEK_SET);
	for (int y = 211; y >= 0; y--) {
		fread(img_data[y], 1, 128, fp);
	}
	fclose(fp);

	// SC5ファイル書き出し
	FILE *out = fopen(argv[2], "wb");
	
	// 1. BSAVE Header
	uint8_t header[] = {0xFE, 0x00, 0x00, 0xFF, 0x6A, 0x00, 0x00}; 
	// 0x0000 - 0x6AFF (27392 bytes)
	fwrite(header, 1, 7, out);

	// 2. Image Data
	fwrite(img_data, 1, 212 * 128, out);

	// 3. Padding (VRAM上のパレット位置 0x7680 まで空きを埋める)
	uint8_t padding = 0x00;
	for (int i = 0; i < (0x7680 - (212 * 128)); i++) {
		fwrite(&padding, 1, 1, out);
	}

	// 4. MSX Palette
	for (int i = 0; i < 16; i++) {
		uint8_t msx_pal[2];
		convert_palette(&bmp_pal[i], msx_pal);
		fwrite(msx_pal, 1, 2, out);
	}

	fclose(out);
	printf("Successfully converted to %s\n", argv[2]);
	return 0;
}