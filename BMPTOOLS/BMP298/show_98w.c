/* RGBI converted Bitmap data loader for PC-98(MSX2 palette) OpenWatcom */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <string.h>

#define ON 1
#define OFF 0
#define ERROR 1
#define NOERROR 0

//#define WIDTH 256
//#define HEIGHT 212

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

/*パレット・セット*/
void pal_set(unsigned char pal_no, unsigned short color, unsigned char red, unsigned char green,
	unsigned char blue)
{
	outpw(0xa8, color);
	outpw(0xaa, green);
	outpw(0xac, red);
	outpw(0xae, blue);
}

//value < 0 黒に近づける。
//value = 0 設定した色
//value > 0 白に近づける。
void set_constrast(int value, unsigned char org_pal[MAXCOLOR][3], int pal_no)
{
	int j, k;
	int pal[3];


	for(j = 0; j < MAXCOLOR; j++){
		for(k = 0; k < 3; k++){
			if(value > 0)
				pal[k] = org_pal[j][k] + value;
			else if(value < 0)
				pal[k] = org_pal[j][k] * (15 + value) / 15;
			else
				pal[k] = org_pal[j][k];
			if(pal[k] < 0)
				pal[k] = 0;
			else if(pal[k] > 15)
				pal[k] = 15;
		}
		pal_set(pal_no, j, pal[0], pal[1], pal[2]);
	}
}

/* PC-98 VRAM 各プレーンのセグメント */
const unsigned int vram_segments[] = {0xA800, 0xB000, 0xB800, 0xE000};


void show_image(const char *filename, int WIDTH, int HEIGHT) {
	FILE *fp;
	int p, y;
	unsigned char *line_buf;

	fp = fopen(filename, "rb");
	if (!fp) {
		printf("Error: Cannot open file.\n");
		return;
	}

//	unsigned char line_buf[WIDTH / 8];
	line_buf = malloc(WIDTH / 8);

	/* グラフィック表示を有効化 */
	g_init(0);
	set_constrast(0, org_pal, 0);

	for (p = 0; p < 4; p++) {
		unsigned int seg = vram_segments[p];
		for (y = 0; y < HEIGHT; y++) {
			if (fread(line_buf, 1, WIDTH / 8, fp) < (WIDTH / 8)) break;
			_fmemcpy(MK_FP(seg, y * 80), line_buf, WIDTH / 8);
		}
	}

	free(line_buf);
	fclose(fp);

	cursor_switch(ON);
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
//		printf("Usage: show98 <file.bin>\n");
		printf("Usage: %s filename width height\n", argv[0]);
		return 1;
	}

	show_image(argv[1], atoi(argv[2]), atoi(argv[3]));

//	printf("Done. Press any key...");
//	getchar();
	return 0;
}
