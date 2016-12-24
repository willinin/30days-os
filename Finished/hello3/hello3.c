#include "apilib.h"
#include <stdio.h>
void HariMain(void)
{
	unsigned char gdt[6];
	int a = 10;
	api_addr2(gdt,&a);
	api_end();
}
