#include <rtthread.h>

void Calculate(void)
{
	unsigned long x;
	unsigned long a;
	a = 1;
	
	for(x=0; x<4294967294; x++)
	{
		a = a + 1;
	}
}

