/* PC-88 CHR88.BAS 表示実験 By m@3 */
/* .COM版 */
/* 転送部インラインアセンブラ化 ZSDCC版 */

#include <stdio.h>
#include <stdlib.h>

#define VRAM_ADR 0xc000

#define PARTS_SIZE 0x1e00 //0x2000

unsigned char mainram_data[PARTS_SIZE];

FILE *stream[2];

#define ON 1
#define OFF 0
#define ERROR 1
#define NOERROR 0

void DI(void){
__asm
	DI
__endasm;
}

void EI(void){
__asm
	EI
__endasm;
}

void Set_RAM_MODE(void){
__asm
	DI
;	ld	a,(#0xe6c2)
;	or	#0x02
	ld	a,#0x3b
	out(#0x31),a
__endasm;
}

void Set_ROM_MODE(void){
__asm
;	ld	a,(#0xe6c2)
	ld	a,#0x39
	out(#0x31),a
	EI
__endasm;
}

short bload(char *loadfil, unsigned char *buffer, unsigned short size)
{
	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);
		return ERROR;
	}
//	printf("Load file %s. Address %x Size %x End %x\n", loadfil, buffer, size, buffer+size);
//	Set_RAM_MODE();
	fread( buffer, 1, 4, stream[0]);
	fread( buffer, 1, size, stream[0]);
//	Set_ROM_MODE();
	fclose(stream[0]);
	return NOERROR;
}

void  put_chr88_pat(unsigned char *mainram, unsigned char *vram)
{
__asm
	DISP equ 80 ;32

	ld	hl, 2
	add	hl, sp
	ld	c, (hl)
	inc	hl
	ld	b, (hl)	; bc=mainam
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)	; de = vram
	ld	l,c
	ld	h,b	; hl = mainram

	DI
;	ld	a,(#0xe6c2)
;	or	#0x02
;	out	(#0x31),a

	ld	b,32
	ld	c,DISP

looppat0:
	push	bc
	push	de

	xor	a
	out	(#0x5c),a

looppat1:
	ldi
	cp	a,c
	jr	nz,looppat1

	out	(#0x5f),a

	ld	bc,80-DISP
	add	hl,bc
	ex	de,hl
	add	hl,bc
	ex	de,hl

	pop	de
	pop	bc

	push bc
	push de
	out	(#0x5d),a

looppat2:
	ldi
	cp	a,c
	jr	nz,looppat2

	out	(#0x5f),a

	ld	bc,80-DISP
	add	hl,bc
	ex	de,hl
	add	hl,bc
	ex	de,hl

	pop	de
	pop	bc
	push bc
	out	(#0x5e),a

looppat3:
	ldi
	cp	a,c
	jr	nz,looppat3

	out	(#0x5f),a
	ld	bc,80-DISP
	add	hl,bc
	ex	de,hl
	add	hl,bc
	ex	de,hl

	pop bc

	djnz	looppat0

;	ld	a,(#0xe6c2)
;	out(#0x31),a
	EI

__endasm;
}

unsigned char *vram_adr;
unsigned char i, j;


int	main(int argc,char **argv)
{
	if (argc < 2){ //argv[1] == NULL){
		printf("PC-88 CHR88 file Loader.\n");
		return ERROR;
	}

	Set_RAM_MODE();

	if(bload(argv[1], mainram_data, PARTS_SIZE))
		return ERROR;

	j = 0;
	vram_adr = (unsigned char *)(VRAM_ADR);
	put_chr88_pat(mainram_data, vram_adr);
//	put_chr88_pat(&mainram_data[32], vram_adr + 80 * 32);

	return NOERROR;
}

