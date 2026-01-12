#include <stdio.h>
#include <stdlib.h>
/* 参考 NRTDRV */

unsigned char check_turbo(void)
{
__asm
	ld	h,#0
	ld	l,#0

	ld	b, #0x1f
	ld	c, #0xa0			; 本体側のCTC($1fa0)があればturbo
	ld	a,7 ;00000111B
	out	(c), a
	out	(c), c
	in	a, (c)
	xor	c
	jr	nz,CHKEND
	ld	l,1
CHKEND:
__endasm;
}

unsigned char check_fm(void)
{
__asm
	ld	h,#0
	ld	l,#0

	ld	b, #0x07
	ld	c, #0x04			; FM音源のCTC($0704)があれば
	ld	a,7 ;00000111B
	out	(c), a
	out	(c), c
	in	a, (c)
	xor	c
	jr	nz,CHKEND
	ld	l,1
CHKEND1:
__endasm;
}

void main(void)
{
	outp(0x1fb0, 0x90);	/* 多色モード */
	if(inp(0x1fb0) == 0x90){
		outp(0x1fb0, 0x0);
		printf("X1 turbo Z\n");
	}else if(check_turbo()){
		printf("X1 turbo\n");
	}else{
		printf("X1\n");
	}

	if(check_fm()){
		printf("OPM found.\n");
	}else{
		printf("OPM not found.\n");
	}
}

