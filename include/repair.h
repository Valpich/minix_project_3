#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


int inodewalker(){
    message m;
    int x=_syscall(VFS_PROC_NR,108,&m);
    return x;
}

int zonewalker(){
    message m;
    int x=_syscall(VFS_PROC_NR,109,&m);
    return x;
}

int directorywalker(char * path){
	message m;
    m.m1_p1=path;
    int x=_syscall(VFS_PROC_NR,110,&m);
    return x;
}

int recovery(){
    message m;
    int x=_syscall(VFS_PROC_NR,103,&m);
    return x;
}

int damage(int inode){
    message m;
    m.m1_i1=inode;
    int x=_syscall(VFS_PROC_NR,105,&m);
    return x;
}