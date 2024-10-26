#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>

#include "diskcalc.h"
#include "memcalc.h"
#include "cpucalc.h"

#define _DEBUG 0
#define _INTERVAL_SEC 3
#define _INTERVAL_NSEC 0

/*  functions    */
void init_usage();
void init_arg(int count, char *values[]);
void init_monitor();

void *monitor_thread(void *proc_id);
bool get_procstatus(int _pid);

/*  variables   */
pid_t pid;
pthread_t tid;
bool run = false;

bool b_printcpu = false;
bool b_printmemory = false;
bool b_printdisk = false;

void sigHandle(int signum)
{
    printf("\n");
    printf("signal : %d , bye. \n", signum);

    pthread_join(tid, NULL);

    exit(0);
}

void print_help()
{
    printf("Usage: \n");
    printf(" -p, --p : process pid \n");
    printf(" -c, --c : print cpu \n");
    printf(" -m, --m : print memory \n");
    printf(" -d, --d : print disk \n");
    printf(" -h, --h : print help \n");
    printf("\n");

    exit(0);
}

void n_sleep(int sec, int nsec)
{
    struct timespec rem, req;

    req.tv_sec = sec;
    req.tv_nsec = nsec;

    nanosleep(&req, &rem);

    return;
}

int main(int argc, char *argv[])
{
    struct sysinfo info;

    init_arg(argc, argv);
    init_usage();
    init_monitor();

    run = true;

#if _DEBUG
    sysinfo(&info);
    printf("load info : %lu %lu %lu \n", info.loads[0], info.loads[1], info.loads[2] );
    printf("memory info : %lu %lu %lu \n", info.totalram, info.totalram-info.freeram, info.freeram );
#endif

    while (run)
    {
        printf("--------------------------------\n");
        if (b_printcpu)
        {
            getProcinfo(pid);
            getSystemStat();
            
            n_sleep(_INTERVAL_SEC, _INTERVAL_NSEC);
            printf("Linux cpu usage(%%) : %0.1f %% \n", cpu_usage);

            if (!calCpu())
            {
                raise(SIGINT);
            }
        }

        if (b_printmemory)
        {
            if(!readMemInfo(&mems))
            {
                printf(" '/proc/meminfo' read failed. \n");
            }
            else
            {
                real_mem = 0.0;
                pretext_mem = 0.0;

                real_mem = ((double)mems.mtot - (double)mems.fmree) / (double)mems.mtot * 100;
                pretext_mem = ((double)mems.mtot - (double)mems.mavail) / (double)mems.mtot * 100;

                printf("Linux real memory usage(%%) : %0.1f %% \n", real_mem);
                printf("Linux pretext memory usage(%%) : %0.1f %% \n", pretext_mem);
            }

            if(!ReadProcStatm(&pstatm, pid))
            {
                printf(" '/proc/%d/statm' read failed. \n", pid);
            }

            if(!readProcMemInfo(&pmems, pid))
            {
                printf(" '/proc/%d/status' read failed. \n", pid);
            }
            else
            {
                pmem = 0.0;
                pmem = ( (double)pmems.VmData / (double)mems.mtot ) * 100;
                printf("Process memory usage(%%) : %0.1f %% \n", pmem);
            }
        }

        if (b_printdisk)
        {
            if ((minfo = MountOpen()) == NULL)
            {
                printf(" '/proc/mounts' open failed. \n");
            }
            else
            {
                while(ReadMountInfo(minfo))
                {
                    Once_MountInfo(minfo->mountdir);
                }
                MountReadClose(minfo);
            }
        }
        printf("--------------------------------\n\n");
    }
    
    return 0;
}

void init_usage()
{
    // signal
    signal(SIGINT, sigHandle);
    signal(SIGSTOP, sigHandle);
    signal(SIGKILL, sigHandle);
    signal(SIGHUP, sigHandle);

    total_cpu_usage = 0.0;
    total_proc_time = 0.0;
    pcpu_usage = 0.0;
    cpu_usage = 0.0;

}

void init_arg(int count, char *values[])
{
    extern char *optarg;
    extern int optopt;

    char args_str[512] = {0,};
    int i = 0, j = 0;

	while( (i = getopt(count, values, "hcmdp:")) != -1 )
    {
		switch(i)
        {
			case 'h':
                print_help();
				break;
			case 'c':
                b_printcpu = true;
				break;
			case 'm':
                b_printmemory = true;
				break;
			case 'd':
                b_printdisk = true;
				break;
			case 'p':
				memcpy(args_str, optarg, strlen(optarg));
                for (j = 0; args_str[j] != '\0'; j++)
                {
                    if ( !isdigit(args_str[j]) )
                    {
                        print_help();
                    }
                }
                pid = atoi(optarg);
				break;

			case '?':
                print_help();
                break;
		}
	}

}

void init_monitor()
{
    pthread_t thr;
    pthread_attr_t attr;

    if(pthread_attr_init(&attr) != 0)
    {
        printf("pthread_attr_init failed. \n");
    }
    
    if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
    {
        printf("pthread_attr_setdetachstate failed. \n");
    }

    if(pthread_create(&thr, NULL, monitor_thread, (void *)pid) != 0)
    {
        printf("pthread_create failed. \n");
    }

    if(pthread_attr_destroy(&attr) != 0)
    {
        printf("pthread_attr_destroy failed. \n");
    }
    
    n_sleep(1, _INTERVAL_NSEC);
}

void *monitor_thread(void *proc_id)
{
    char *thread_name = "monotor_thread";
    int monitor_pid = (int)proc_id;

    pid_t self_pid = getpid();
    tid = pthread_self();

    printf("== Thread (%s) start. == \n", thread_name);

    while (run)
    {
        bool ret = get_procstatus(monitor_pid);
        if (!ret)
        {
            printf("process not alive. monitor exit. \n");
        }
        n_sleep(_INTERVAL_SEC, _INTERVAL_NSEC);
    }

    return NULL;
}

bool get_procstatus(int _pid)
{
    char buf[_BUFSIZE] = {0,};
    bool ret = false;
    int i = 0;

    snprintf(buf, sizeof(buf), "/proc/%d/stat", _pid);

    for (; i < 3; i++)
    {
        FILE * fp = fopen(buf, "r");
        if(fp != NULL)
        {
            fclose(fp);
            ret = true;

            break;
        }
        n_sleep(1, _INTERVAL_NSEC);
    }

    return ret;
}
