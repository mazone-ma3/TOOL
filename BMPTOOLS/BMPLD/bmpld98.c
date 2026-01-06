/* Bitmap Loader for PC-9801 OpenWatcom */

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>

#define ON 1
#define OFF 0

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

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

void poke(int a, int b, char c) {
	*((char __far *)MK_FP(a,b)) = c;
}

char peek(int a, int b) {
	return *((char __far *)MK_FP(a,b));
}

/*カーソル及びファンクションキー表示の制御*/
void cursor_switch(unsigned short mode)
{
	if(mode)
		printf("\x1b[>1l\x1b[>5l");
	else
		printf("\x1b*\x1b[>1h\x1b[>5h");
}

void screen_switch(unsigned short mode)
{
	if(mode)
		outp(0xa2, 0x0d); /* 表示開始 */
	else
		outp(0xa2, 0x0c);
}

/*カーソルのＯＦＦ、ＶＲＡＭアドレスの設定*/
void g_init(unsigned short mode_f)
{
/*unsigned short g_driver, g_mode;
	g_driver = DETECT;
	g_mode = 1;
	initgraph(&g_driver, &g_mode ,"");
*/
	_disable();

	outp(0x6a, 1   ); /* 16色モード (0x6a=mode f/fp2)*/

/*	outp(0x6a, 0x07);  拡張モード変更可能
	outp(0x6a, 0x84);  gdc2.5mhz */

	outp(0xa2, 0x4b); /* GDC CSRFORMコマンド */
	outp(0xa0, (mode_f == 1)); /* L/R = 1 (縦方向の拡大係数)*/

	outp(0x68, 8   ); /* モードF/F1 (8で高解像度)*/

	outp(0x4a0,0xfff0);
	outpw(0x7c, 0);

	screen_switch(ON); /* 表示開始 */

	_enable();

	cursor_switch(OFF);

}

// VRAMの各プレーンへ書き込む関数
void out_vram(int x, int y, BYTE data, int plane) {
	unsigned int offset = (y * 80) + (x / 8);
	BYTE mask = 0x80 >> (x % 8);
	BYTE far *vram;

	switch(plane) {
		case 0: vram = (BYTE far *)MK_FP(0xA800, offset); break; // Blue
		case 1: vram = (BYTE far *)MK_FP(0xB000, offset); break; // Green
		case 2: vram = (BYTE far *)MK_FP(0xB800, offset); break; // Red
		case 3: vram = (BYTE far *)MK_FP(0xE000, offset); break; // Intensity
	}

	if (data) *vram |= mask;
	else	  *vram &= ~mask;
}

/* 256段階(8bit)を16段階(4bit)に変換するテーブル */
BYTE pal8to4[256];

void init_pal_table() {
	int i;
	for (i = 0; i < 256; i++) {
		/* (i / 17.0) を四捨五入する計算式 */
//		pal8to4[i] = (BYTE)((i + 8) / 17);
		pal8to4[i] = (BYTE)(i * 255 / 15);
	}
}

void set_palette(BYTE pal[16][4]) {
	int i;
	/* テーブルが初期化されていない場合のための安全策（またはmainで呼ぶ） */
	if (pal8to4[255] == 0) init_pal_table();

	for (i = 0; i < 16; i++) {
		outp(0xa8, i);			  /* パレット番号指定 */
		outp(0xaa, pal8to4[pal[i][1]]); /* Green (正確な変換) */
		outp(0xac, pal8to4[pal[i][2]]); /* Red   (正確な変換) */
		outp(0xae, pal8to4[pal[i][0]]); /* Blue  (正確な変換) */
	}
}
/*
void set_palette(BYTE pal[16][4]) {
	int i;
	for (i = 0; i < 16; i++) {
		outp(0xa8, i);		// パレット番号指定
		outp(0xaa, pal[i][1] >> 4); // Green (4bit)
		outp(0xac, pal[i][2] >> 4); // Red (4bit)
		outp(0xae, pal[i][0] >> 4); // Blue (4bit)
	}
}*/

/* VRAMプレーンのセグメント */
unsigned int vram_seg[] = { 0xA800, 0xB000, 0xB800, 0xE000 };

/* 8ピクセル分のデータをまとめて4枚のプレーンに書き込む */
void draw_8pixels(int byte_x, int y, BYTE *p8) {
	unsigned int offset = (y * 80) + byte_x;
	BYTE b = 0, g = 0, r = 0, i_pl = 0;
	int k;

	for (k = 0; k < 8; k++) {
		if (p8[k] & 1) b |= (0x80 >> k);
		if (p8[k] & 2) g |= (0x80 >> k);
		if (p8[k] & 4) r |= (0x80 >> k);
		if (p8[k] & 8) i_pl |= (0x80 >> k);
	}

	/* 4枚のプレーンへ転送 */
	for (k = 0; k < 4; k++) {
		BYTE far *vptr = (BYTE far *)MK_FP(vram_seg[k], offset);
		*vptr = (k==0)?b : (k==1)?g : (k==2)?r : i_pl;
	}
}

int load_bmp_fast(char *filename) {
	FILE *fp;
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	BYTE pal_data[16][4];
	BYTE line_buffer[640 / 2]; // 4bit BMPの1行分
	BYTE pixels_8[8];
	int x, y, k;
	int width_bytes;

	if ((fp = fopen(filename, "rb")) == NULL) return -1;

	fread(&bfh, sizeof(bfh), 1, fp);
	fread(&bih, sizeof(bih), 1, fp);
	fread(pal_data, 4, 16, fp);
	
	set_palette(pal_data); // パレット更新

	// 1行あたりのバイト数（4バイト境界）を計算
	width_bytes = (int)((bih.biWidth * 4 + 31) / 32) * 4;

	for (y = (int)bih.biHeight - 1; y >= 0; y--) {
		fseek(fp, bfh.bfOffBits + (long)(bih.biHeight - 1 - y) * width_bytes, SEEK_SET);
		fread(line_buffer, 1, width_bytes, fp);

		for (x = 0; x < bih.biWidth / 8; x++) { // 1行80バイト(640ピクセル)
			// 1バイト(2ピクセル)から4バイト(8ピクセル)分取り出す
			for (k = 0; k < 4; k++) {
				pixels_8[k * 2]	 = (line_buffer[x * 4 + k] >> 4) & 0x0F;
				pixels_8[k * 2 + 1] = line_buffer[x * 4 + k] & 0x0F;
			}
			draw_8pixels(x, y, pixels_8);
		}
	}

	fclose(fp);
	return 0;
}


int main(int argc, char *argv[])
{
	g_init(0);

	init_pal_table();

	if(load_bmp_fast(argv[1]))
		printf("Bitmap loader for PC-9801.\n");

	cursor_switch(ON);

	return 0;
}

