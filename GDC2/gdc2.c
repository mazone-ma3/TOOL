/* GDCクロック切り替え By m@3 2024/11/2- */
/* for OpenWatcom */
/* 参考 UNDOCUMENTED 9801/9821 Vol.2、PC-9801 Programmer's Bible、gdc10.lzh */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <i86.h>
#include <unistd.h>

union REGS reg;

unsigned char __far *gdcport;

void set_gdcclock(int mode)
{
	gdcport = MK_FP(0, 0x54d);
	if(!mode){
		reg.h.ch = 0x80;	/* 200Line(LOWER) */
		reg.h.ah = 0x42;
		int86(0x18, &reg, &reg);

		*gdcport &= 0xdf;	/* 2.5MHz(OFF) */

		reg.h.ch = 0xc0;	/* 400Line(ALL) */
		reg.h.ah = 0x42;
		int86(0x18, &reg, &reg);

		outp(0x06a, 0x82);	/* GDC CLOCK-1 2.5MHz */
		outp(0x06a, 0x84);	/* GDC CLOCK-2 2.5MHz */
	}else{
		reg.h.ch = 0x80;	/* 200Line(LOWER) */
		reg.h.ah = 0x42;
		int86(0x18, &reg, &reg);

		*gdcport |= 0x20;	/* 5MHz(ON) */

		reg.h.ch = 0xc0;	/* 400Line(ALL) */
		reg.h.ah = 0x42;
		int86(0x18, &reg, &reg);

		outp(0x06a, 0x83);	/* GDC CLOCK-1 5MHz */
		outp(0x06a, 0x85);	/* GDC CLOCK-2 5MHz */
	}
}

void main(int argc, char **argv)
{
	if(argc < 2){
		printf("GDC 2.5MHz\n");
		set_gdcclock(0);
	}else{
		printf("GDC 5MHz\n");
		set_gdcclock(1);
	}
}
