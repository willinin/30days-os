#include "apilib.h"
#include <stdio.h>


void HariMain(void)
{
    char s[20];
    int a = 100;
    int addr = api_addr(&a);
    sprintf(s,"0x%x",addr);
    api_putstr0(s);
	api_end();
}
