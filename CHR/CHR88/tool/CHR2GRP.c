/* PC-88 CHR88.BAS ->.grp•ÏŠ· (‰¡512dot)  for GCC By m@3 */

#include <stdio.h>
#include <stdlib.h>

#define VRAM_ADR 0xc000

#define PARTS_SIZE 0x1e00 //0x2000

unsigned char mainram_data[2][PARTS_SIZE];

FILE *stream[2];

#define ON 1
#define OFF 0
#define ERROR 1
#define NOERROR 0

#define SIZE 80
#define LINE 32
#define WIDTH (256 / 8)
#define PLANE 3

short bload(char *loadfil, char *savefil, unsigned char *buffer1,unsigned char *buffer2, unsigned short size)
{
	unsigned short i, j, k;
	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);
		return ERROR;
	}
	if ((stream[1] = fopen( savefil, "wb")) == NULL) {
		printf("Can\'t open file %s.", savefil);
		fclose(stream[0]);
		return ERROR;
	}
	fread( buffer1, 1, 4, stream[0]);
	fwrite( buffer1, 1, 4, stream[1]);
	fread( buffer1, 1, size, stream[0]);

	k = 0;
	for(j = 0; j < LINE; ++j){
		for(i = 0; i < WIDTH; ++i){
			buffer2[k + 0] = buffer1[(i + j * SIZE * PLANE) ];
			buffer2[k + 1] = buffer1[(i + j * SIZE * PLANE) + SIZE];
			buffer2[k + 2] = buffer1[(i + j * SIZE * PLANE) + SIZE * 2];
			k += PLANE;
		}
	}
	for(j = 0; j < LINE; ++j){
		for(i = 0; i < WIDTH; ++i){
			buffer2[k + 0] = buffer1[(i + j * SIZE * PLANE) + WIDTH];
			buffer2[k + 1] = buffer1[(i + j * SIZE * PLANE) + WIDTH + SIZE];
			buffer2[k + 2] = buffer1[(i + j * SIZE * PLANE) + WIDTH + SIZE * 2];
			k += PLANE;
		}
	}

	fwrite( buffer2, 1, size, stream[1]);
	fclose(stream[0]);
	return NOERROR;
}

unsigned char *vram_adr;
unsigned char i, j;


int	main(int argc,char **argv)
{
	if (argc < 3){ //argv[1] == NULL){
		printf("CHR88.BAS to PC-88 .grp file conv.\n");
		return ERROR;
	}

	if(bload(argv[1], argv[2], (unsigned char *)&mainram_data[0], (unsigned char *)&mainram_data[1], PARTS_SIZE))
		return ERROR;

	return NOERROR;
}

