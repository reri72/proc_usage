#include "cpucalc.h"

unsigned long long total_cpu_usage;
unsigned long total_proc_time;
double pcpu_usage;
double cpu_usage;

void getSystemStat()
{
    FILE * fp = fopen( "/proc/stat", "r");
    char buf[_BUFSIZE] = {0,};

    static unsigned long long tcpu_usage, btcpu_usage;
    unsigned long long user = 0, nice = 0, system = 0, idle = 0;
    unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
    double sum = 0;

    if( fp != NULL)
    {
        fgets(buf, _BUFSIZE, fp);

        sscanf(buf, "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                &user, &nice, &system, &idle,
                &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
        tcpu_usage = user + nice + system + idle;

        total_cpu_usage = tcpu_usage - btcpu_usage;
        btcpu_usage = tcpu_usage;
        
        sum = tcpu_usage + iowait +irq + softirq + steal + guest + guest_nice;
        cpu_usage = 100 - ( (idle * 100.0f) / sum ) ;

        fclose(fp);
    }
    else
    {
        printf(" '/proc/stat' file open failed. \n");
        tcpu_usage = 0;
    }
    
    return;
}

void getProcinfo(pid_t pid)
{
    char ppath[_BUFSIZE] = {0,};
    int cx = 0;

    cx = snprintf( ppath, sizeof(ppath), "/proc/%d/stat", pid);
    if(!cx)
    {
        return;
    }

    FILE * fp1 = fopen( ppath, "r");

    char buf1[_BUFSIZE] = {0,};

    unsigned long pUtime, pStime;
    unsigned long pCutime, pCstime;
    static unsigned long tproc_time, btproc_time;

    if( fp1 != NULL)
    {
        fgets(buf1, _BUFSIZE, fp1);

        //14(utime), 15(stime)
        //16(Cutime), 17(Cstime)
        sscanf(buf1, "%*d %*s %*c %*d %*d"
                    "%*d %*d %*d %*u %*u"
                    "%*u %*u %*u %lu %lu"
                    "%ld %ld %*d %*d %*d"
                    , &pUtime, &pStime, &pCutime, &pCstime);

        tproc_time = pUtime + pStime + pCutime + pCstime;

        total_proc_time = tproc_time - btproc_time;
        btproc_time = tproc_time;

        fclose(fp1);
    }
    else
    {
        printf(" '/proc/pid/stat' file open failed. \n");
        total_proc_time = 0;
    }
    
    return;
}

bool calCpu()
{
    // irix mode
    pcpu_usage = CPU_EA * total_proc_time * 100 / (float) total_cpu_usage ;

    if(pcpu_usage < 0)
    {
        return false;
    }
    else
    {
        printf("Process cpu usage(%%) : %.1f %%\n", pcpu_usage);
    }

    return true;
}

void getSystemUptime()
{
    FILE * fp = fopen( "/proc/uptime", "r");
    char buf[_BUFSIZE] = {0,};
    double u1 = 0, u2 = 0;

    if( fp != NULL)
    {
        fgets(buf, _BUFSIZE, fp);

        sscanf(buf, "%lf %lf", &u1, &u2);
        
        fclose(fp);
    }
    else
    {
        printf(" '/proc/uptime' file open failed. \n");
    }

    return;
}