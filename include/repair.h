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

int damage(int inode, int operation, int file_descriptor, char * folder){
    message m;
    m.m1_i1 = inode;
    m.m1_i2 = operation;
    m.m1_i3 = file_descriptor;
    m.m1_p1 = folder;
    printf("Calling VFS\n");
    int x=_syscall(VFS_PROC_NR,105,&m);
    return x;
}