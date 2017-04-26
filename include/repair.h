#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
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

int damage(int inode, int operation, char * folder){
    int * output = calloc(1024*1024,4);
    message m;
    m.m1_i1 = inode;
    m.m1_i2 = operation;
    m.m1_p1 = folder;
    m.m1_p2 = output;
    printf("Calling VFS\n");
    int x=_syscall(VFS_PROC_NR,105,&m);
    FILE * file = fopen("map.txt","w");
    int i=0;
    while(output[i]!=0){
        i++;
    }
    printf("map size is: %d",i);
    i=0;
    while(output[i]!=0){
        fprintf(file,"%d",output[i]);
        i++;
    }
    fclose(file);
    free(output);
    return x;
}