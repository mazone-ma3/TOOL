#include <stdio.h>
#include <stdlib.h>
/* éQçl Bookworm's Library */

void main(void)
{
__asm
	DI
__endasm;

	if(inp(0x31) & 0x80)
		printf("V1 Mode\n");
	else
		printf("V2 Mode\n");

	if(inp(0x6e) & 0x80)
		printf("4MHz\n");
	else
		printf("8MHz\n");

	outp(0x44, 0x00);
	outp(0x45, 0x00);
	outp(0x44, 0x00);
	outp(0x45, 0x01);
	outp(0x44, 0x00);
	if(inp(0x45) != 1)
		printf("0x44 not found\n");
	else{
		outp(0x44 ,0xff);
		if(inp(0x45) == 0x01)
			printf("0x44 = OPNA\n");
		else
			printf("0x44 = OPN\n");
	}

	outp(0xa8, 0x00);
	outp(0xa9, 0x00);
	outp(0xa8, 0x00);
	outp(0xa9, 0x01);
	outp(0xa8, 0x00);
	if(inp(0xa9) != 1)
		printf("0xa8 not found\n");
	else{
		outp(0xa8 ,0xff);
		if(inp(0xa9) == 0x01)
			printf("0xa8 = OPNA\n");
		else
			printf("0xa8 = OPN\n");
	}

__asm
	EI
__endasm;
}
