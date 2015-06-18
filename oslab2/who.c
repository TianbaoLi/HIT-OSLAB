#define __LIBRARY__
#include <unistd.h>
#include <asm/segment.h>  
char str[23];
int len;
int sys_iam(const char * name)
{
	int i=0;
	for (i=0;(get_fs_byte(name+i))!='\0';i++);
	if(i>23)
		return (-EINVAL);
	i=0;
	for (i=0;(str[i] = get_fs_byte(name+i))!='\0';i++);
	len=i;
	return i;
}
int sys_whoami(char* name, unsigned int size)
{
	int i=0;
	if(len>size)
		return (-EINVAL);
	for(i=0;i<len;i++)
		put_fs_byte(str[i],(name+i));
	return len;
}