#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	printf("atof(35,5)=%7.3f\n",(float)atof("35,5"));
	printf("atof(35.5)=%7.3f\n",(float)atof("35.5"));
	return 0;
}
