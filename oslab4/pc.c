#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
int Read()
{
	FILE *fp=fopen("buff.txt","rb");
	int i;
	if(fp==NULL)
	{
		printf("buff open error in reading\n");
		fflush(stdout);
		return -1;
	}
	int tmp[10];
	for(i=0;i<10;i++)
	{
		tmp[i]=-1;
		fread(&tmp[i],sizeof(int),1,fp);
	}
	fclose(fp);
	printf("Child PID:%d\tData:%d\n",getpid(),tmp[0]);
	fflush(stdout);
	fp=fopen("buff.txt","wb");
	for(i=1;i<10;i++)
		if(tmp[i]>0)
			fwrite(&tmp[i],sizeof(int),1,fp);
	fclose(fp);
	return tmp[0];
}
int Write(int x)
{
	FILE *fp=fopen("buff.txt","ab+");
	if(fp==NULL)
	{
		printf("buff open error in writing\n");
		fflush(stdout);
		return -1;
	}
	fwrite(&x,sizeof(int),1,fp);
	fclose(fp);
	return 1;
}
int main()
{
	sem_t *empty,*full,*mutex;
	empty=(sem_t *)sem_open("empty",O_CREAT,0777,10);
	full=(sem_t *)sem_open("full",O_CREAT,0777,0);
	mutex=(sem_t *)sem_open("mutex",O_CREAT,0777,1);
	int i,j,k;
	if(!fork())
	{
		for(i=0;i<500;i++)
		{
			sem_wait(empty);
			sem_wait(mutex);
			Write(i);
			sem_post(mutex);
			sem_post(full);
		}
		return 0;
	}
	for(j=0;j<10;j++)
		if(!fork())
		{
			for(k=0;k<500/10;k++)
			{
				sem_wait(full);
				sem_wait(mutex);
				Read();
				sem_post(mutex);
				sem_post(empty);
			}
			return 0;
		}
	for(i=0;i<10;i++)
		wait(NULL);
	sem_unlink("empty");
	sem_unlink("full");
	sem_unlink("mutex");
	return 0;
}