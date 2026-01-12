/* 16colorBitmap->PC-9801 RGBI convert */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

//#define WIDTH 256
//#define HEIGHT 212

#pragma pack(1)
typedef struct {
	WORD  bfType;
	DWORD bfSize;
	WORD  bfReserved1;
	WORD  bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
	DWORD biSize;
	long  biWidth;
	long  biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	long  biXPelsPerMeter;
	long  biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER;


// 前述のRGBI変換ロジックを使用
uint8_t rgb_to_rgbi(uint8_t r, uint8_t g, uint8_t b) {
	uint8_t res = 0;
	if (r >= 128) res |= 0x04; // Red
	if (g >= 128) res |= 0x02; // Green
	if (b >= 128) res |= 0x01; // Blue
	if (r > 192 || g > 192 || b > 192) res |= 0x08; // Intensity
	return res;
}

int main(int argc, char *argv[]) {
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	int WIDTH,HEIGHT;
	int plane_num = 4;

	if (argc < 3) {
		printf("Usage: %s <input.bmp> <output.bin> [<plane numbers 1-3>]\n", argv[0]);
		return 1;
	}
	if (argc >= 4)
		plane_num=atoi(argv[3]);
			if((plane_num > 4) || (plane_num < 1)) return 1;
	printf("plane numbers:%d\n",plane_num);

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) return 1;

	// BMPヘッダ・パレットの読み込み（簡易版：オフセット固定）
	fread(&bfh, sizeof(bfh), 1, fp);
	fread(&bih, sizeof(bih), 1, fp);

	WIDTH =(int)bih.biWidth;
	HEIGHT = (int)bih.biHeight;

//	uint8_t header[54];
//	fread(header, 1, 54, fp);

	fseek(fp, bfh.bfOffBits, SEEK_SET);

/*	struct { uint8_t b, g, r, res; } pal[16];
	fread(pal, 4, 16, fp);

	// RGBI変換マップの作成
	uint8_t rgbi_map[16];
	for(int i=0; i<16; i++) rgbi_map[i] = rgb_to_rgbi(pal[i].r, pal[i].g, pal[i].b);
*/
	// 画像データ読み込み (4bit BMP)
	// 1ピクセル4bit = 1バイト2ピクセル。
	uint8_t raw_pixels[HEIGHT][WIDTH / 2];
	for (int y = HEIGHT - 1; y >= 0; y--) {
		fread(raw_pixels[y], 1, WIDTH / 2, fp);
	}
	fclose(fp);

	// プレーン分離データの生成
//	static uint8_t planes[4][HEIGHT][WIDTH / 8]; 
	uint8_t *planes = malloc(4 * HEIGHT * WIDTH / 8);//[4][HEIGHT][WIDTH / 8]; 
	memset(planes, 0 , 4 * HEIGHT * WIDTH / 8);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x_byte = 0; x_byte < WIDTH / 2; x_byte++) {
			uint8_t two_pix = raw_pixels[y][x_byte];
			uint8_t p[2];
//			p[0] = rgbi_map[two_pix >> 4];   // 上位4bit（左ピクセル）
//			p[1] = rgbi_map[two_pix & 0x0F]; // 下位4bit（右ピクセル）

			p[0] = two_pix >> 4;   // 上位4bit（左ピクセル）
			p[1] = two_pix & 0x0F; // 下位4bit（右ピクセル）

			for (int i = 0; i < 2; i++) {
				int bit_pos = (x_byte * 2 + i);
				int byte_idx = bit_pos / 8;
				int bit_idx = 7 - (bit_pos % 8); // PC-98はMSB側が左ピクセル

				if (p[i] & 0x01) planes[0 * HEIGHT * WIDTH / 8 + y * WIDTH / 8 + byte_idx] |= (1 << bit_idx); // Blue
				if (p[i] & 0x02) planes[1 * HEIGHT * WIDTH / 8 + y * WIDTH / 8 + byte_idx] |= (1 << bit_idx); // Red
				if (p[i] & 0x04) planes[2 * HEIGHT * WIDTH / 8 + y * WIDTH / 8 + byte_idx] |= (1 << bit_idx); // Green
				if (p[i] & 0x08) planes[3 * HEIGHT * WIDTH / 8 + y * WIDTH / 8 + byte_idx] |= (1 << bit_idx); // Intensity
			}
		}
	}

	// 出力 (B -> R -> G -> I の順で書き出し)
	FILE *fout = fopen(argv[2], "wb");
	fwrite(planes, 1, plane_num * HEIGHT * WIDTH / 8, fout);
	fclose(fout);

	free(planes); //, 4 * HEIGHT * WIDTH / 8);

	printf("Done. Output size: %ld bytes\n", plane_num * HEIGHT * WIDTH / 8);
	return 0;
}