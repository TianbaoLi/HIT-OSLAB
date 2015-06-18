#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
_syscall2(int,whoami,char*,name,unsigned int,size)
int main(int argc,char* argv[])
{
	char str[23]={0};
	int a=0;
	a=whoami(str,23);
    	printf("%s\n",str);
    	return 0;
}
