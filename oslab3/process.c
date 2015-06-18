#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>

#define HZ	100

void cpuio_bound(int last, int cpu_time, int io_time);

int main(int argc, char * argv[])
{
	pid_t pid1;
	pid_t pid2;
	pid_t pid3;
	pid_t pid4;
	pid_t pid5;
	if((pid1=fork())==0)
	{
		printf("Child Process 1\n");
		cpuio_bound(10,1,0);
	}
	else if((pid2=fork())==0)
	{
		printf("Child Process 2\n");
		cpuio_bound(10,0,1);
	}
	else if((pid3=fork())==0)
	{
		printf("Child Process 3\n");
		cpuio_bound(10,1,1);
	}
	else if((pid4=fork())==0)
	{
		printf("Child Process 4\n");
		cpuio_bound(10,1,9);
	}
	else if((pid5=fork())==0)
	{
		printf("Child Process 5\n");
		cpuio_bound(10,3,7);
	}
	else if(pid1==-1||pid2==-1||pid3==-1||pid4==-1||pid5==-1)
	{
		extern int errno;
		char *errormsg=strerror(errno);
		printf("Error:%s\n",errormsg);
		return 0;
	}
	else
	{
		printf("****** Parent Process ****\n");
		printf("The pid of parent is %d\n",getpid());
		printf("The pid of child1 is %d\n",pid1);
		printf("The pid of child2 is %d\n",pid2);
		printf("The pid of child3 is %d\n",pid3);
		printf("The pid of child4 is %d\n",pid4);
		printf("The pid of child5 is %d\n",pid5);
		printf("**************************\n");
	}
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	return 0;
}

/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start_time, current_time;
	clock_t utime, stime;
	int sleep_time;

	while (last > 0)
	{
		/* CPU Burst */
		times(&start_time);
		/* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
		 * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
		 * 加上很合理。*/
		do
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		} while ( ( (utime + stime) / HZ )  < cpu_time );
		last -= cpu_time;

		if (last <= 0 )
			break;

		/* IO Burst */
		/* 用sleep(1)模拟1秒钟的I/O操作 */
		sleep_time=0;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
	}
}

