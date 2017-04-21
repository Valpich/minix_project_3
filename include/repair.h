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