#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


int inodewalker(int * r){
    message m;
    m.m1_i1=r;
    int x=_syscall(VFS_PROC_NR,108,&m);
    return x;
}

int zonewalker(int * r){
    message m;
    m.m1_i1=r;
    m.m1_i2=0;
    int x=_syscall(VFS_PROC_NR,109,&m);
    return x;
}

int directorywalker(char * path){
	message m;
    m.m1_p1=path;
    int x=_syscall(VFS_PROC_NR,110,&m);
    return x;
}

int recovery(int * r){
    message m;
    m.m1_i1=r;
    m.m1_i2=0;
    int x=_syscall(VFS_PROC_NR,103,&m);
    return x;
}

int damage(int * r, int * inode){
    message m;
    m.m1_i1=r;
    m.m1_i2=inode;
    int x=_syscall(VFS_PROC_NR,105,&m);
    return x;
}