#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 参考 MSX Datapack */

/* mainromの指定番地の値を得る */
char  read_mainrom(unsigned short adr) __sdcccall(1)
{
__asm
	ld	a,(#0xfcc1)	; exptbl
	call	#0x000c	; RDSLT
__endasm;
}

char read_slot(unsigned char slot, unsigned short adr) __sdcccall(1)
{
__asm
	push	de
	pop	hl
	call	#0x000c	; RDSLT
__endasm;
}

unsigned  short check_cpmver(void) __sdcccall(1)
{
__asm
	ld	c,#0x0c
	call	#5
	ld	d,h
	ld	e,l
__endasm;
}

unsigned  short check_dosver(void) __sdcccall(1)
{
__asm
	ld	c,#0x6f
	call	#5
	or	a
	ld	hl,0
	jr	nz,checkend
	ld	a,b
	cp	2
	ld	hl,#0x100
	jr	nc,checkend2
checkend:	; b = 0-2
	ld	d,h
	ld	e,l
;	ld	h,d
;	ld	l,e
checkend2:
__endasm;
}

unsigned char getcpu(void) __sdcccall(1)
{
__asm
	push	ix
	ld	a,(#0xfcc1)	; exptbl
	ld	d,a
	ld	e,0
	push	de
	pop	iy
	ld ix,#0x0183	; GETCPU(MAINROM)

	call	#0x001c	; CALSLT
	pop	ix
__endasm;
}

void enable_opll(unsigned char slot) __sdcccall(1)
{
__asm
	push	iy
	push	ix
	push	bc
	push	de
	push	hl
	ld	hl,#08000h
	ld	d,a
	ld	e,0
	push	de
	pop	iy
	ld ix,#0x04113	; INIOPL

	call	#0x001c	; CALSLT
	pop	hl
	pop	de
	pop	bc
	pop	ix
	pop	iy
__endasm;
}

#define exptbl_pointer ((volatile unsigned char *)0xfcc1)

char slot_chr[5];
unsigned char i, j, k, l, opll_mode = 0, cpu, opll_slot = 0;

void main(void)
{
__asm
	DI
__endasm;
	if(check_cpmver() == 0x22){
		printf("MSX-DOS ");
		if(!check_dosver()){
			printf("1 / ");
		}else{
			printf("%.4x / ", check_dosver());
		}
	}else{
		printf("Unknown DOS / ");
	}

	switch(read_mainrom(0x2d)){
		case 0:
			printf("MSX ");
			break;
		case 1:
			printf("MSX2 ");
			break;
		case 2:
			printf("MSX2+ ");
			break;
		case 3:
			printf("MSX turboR ");
			cpu = getcpu();
			switch(cpu){
				case 0:
					printf("Z80 ");
					break;
				case 1:
					printf("R800 ROM ");
					break;
				case 2:
					printf("R800 RAM ");
					break;
				default:
					printf("Unknown mode ");
					break;
			}
			break;
		default:
			printf("Unknown MSX ");
			break;
	}
	if(read_mainrom(0x2b) & 0x80){
		printf("/ 50 Hz\n");
	}else{
		printf("/ 60 Hz\n");
	}
__asm
	DI
__endasm;
	for(i = 0; i < 4; ++i){
		if(!exptbl_pointer[i]){
			printf("%d basic slot. \n", i);
			k = i;
			slot_chr[4] = '\0';
			for(l = 0; l < 4; ++l){
				slot_chr[l] = read_slot(k, 0x401c + l);
			}
			if(!strcmp(slot_chr, "OPLL")){
				printf("found ");
				for(l = 0; l < 4; ++l){
					slot_chr[l] = read_slot(k, 0x4018 + l);
				}
				if(!strcmp(slot_chr, "APRL")){
					printf("internal ");
					opll_mode = 1;
				}else{
					printf("external ");
					if(!opll_mode){
						opll_slot = k;
						opll_mode = 2;
					}
				}
				printf("OPLL basic slot %d \n", i);
			}
		}else{
			printf("%d expand slot. \n", i);
			for(j = 0; j < 4; ++j){
				k = j * 4 + i | 0x80;
				slot_chr[4] = '\0';
				for(l = 0; l < 4; ++l){
					slot_chr[l] = read_slot(k, 0x401c + l);
				}
				if(!strcmp(slot_chr, "OPLL")){
					printf("found ");
					for(l = 0; l < 4; ++l){
						slot_chr[l] = read_slot(k, 0x4018 + l);
					}
					if(!strcmp(slot_chr, "APRL")){
						printf("internal ");
						opll_mode = 1;
					}else{
						printf("external ");
						if(!opll_mode){
							opll_slot = k;
							opll_mode = 2;
						}
					}
					printf("OPLL expand slot %d - %d \n", i, j);
				}
			}
		}
	}
	if(!opll_mode)
		printf("OPLL not found.\n");
	if(opll_mode == 2){
		enable_opll(opll_slot);
		printf("OPLL ON");
	}
__asm
	EI
__endasm;
}
