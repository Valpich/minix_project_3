#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>


int inodewalker(int * r){
    message m;
    m.m1_i1=r;
    printf("avant syscall\n");
    int x=_syscall(VFS_PROC_NR,108,&m);
    printf("apres syscall\n");
    return x;
}

int zonewalker(int * r){
    message m;
    m.m1_i1=r;
    m.m1_i2=0;
    printf("avant syscall\n");
    int x=_syscall(VFS_PROC_NR,109,&m);
    printf("apres syscall\n");
    return x;
}

int directorywalker(char * path){
	message m;
    m.m1_p1=path;
    puts("NOT IMPLEMENTED");
    printf("avant syscall\n");
    int x=_syscall(VFS_PROC_NR,110,&m);
    printf("apres syscall\n");
    return x;
}