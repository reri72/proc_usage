#include "diskcalc.h"

struct statvfs diskusage;
Mountinfo *minfo;

Mountinfo *MountOpen()
{
    Mountinfo *mountinfo = (Mountinfo*)malloc( sizeof(Mountinfo) );
    memset(mountinfo, 0x00, sizeof(Mountinfo));

    if( !( mountinfo->fp = fopen("/proc/mounts", "r") ) )
    {
        free(mountinfo);
        return NULL;
    }

    return mountinfo;
}

Mountinfo *ReadMountInfo(Mountinfo *minfo)
{
    char buf[1024] = {0,};
    int rootdir = 0;

    struct statfs lstatfs;
    struct stat lstat;

    memset(&lstatfs, 0x00, sizeof(struct statfs));
    memset(&lstat, 0x00, sizeof(struct stat));

    // /proc/mounts 의 행을 모두 읽는다
    while( fgets(buf, 1024, minfo->fp) )
    {
        sscanf(buf, "%128s %128s %128s", minfo->devname, minfo->mountdir, minfo->fstype);
        
        if ( strcmp(minfo->mountdir, "/") == 0)
        {
            rootdir = 1;
        }
        else
        {
            rootdir = 0;
        }

        // stat : get file status
        if( stat(minfo->devname, &lstat) == 0 || rootdir )
        {
            // block device(storage device) 확인한다.
            if( ( S_ISBLK(lstat.st_mode) && strstr(buf, minfo->mountdir) ) || rootdir )
            {
                statfs(minfo->mountdir, &lstatfs);

                minfo->blocks = lstatfs.f_blocks * (lstatfs.f_bsize / KB); 
                minfo->avail = lstatfs.f_bavail * (lstatfs.f_bsize / KB); 

                return minfo;
            }
        }
    }

    return NULL;
}

void MountReadClose(Mountinfo *minfo)
{
    fclose(minfo->fp);
    if(minfo)
    {
        free(minfo);
    }
}

void Once_MountInfo(char *dir)
{
    unsigned long long total = 0, used = 0;
    double usage_percentage = 0;

    if (statvfs(dir, &diskusage) != 0 )
    {
        return;
    }

    total = diskusage.f_blocks * diskusage.f_frsize; // 전체 공간
    used = (diskusage.f_blocks - diskusage.f_bfree) * diskusage.f_frsize; // 사용 중인 공간
    usage_percentage = ((double)used / total) * 100;

    printf("Linux disk usage(%%) - [%s] : %0.2f %% \n", dir, ((double)used / total * 100) );
}
