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
    printf("Calling damage\n");
    int max = 1024*1024*32;
    int * output = calloc(max,1);
    int j = 0;
    for(j = 0; j<max ; j++){
        output[j] = -1;
    }
    message m;
    m.m1_i1 = inode;
    m.m1_i2 = operation;
    m.m1_i3 = output;
    m.m1_p1 = folder;
    printf("Calling VFS\n");
    int x=_syscall(VFS_PROC_NR,105,&m);
    printf("test output %d  %d  %d %d\n",output[0],output[1],output[2],output[3]);
    FILE * file = fopen("map.txt","w");
    int i=0;
    while(output[i]!=-1){
        i++;
    }
    printf("map size is: %d\n.",i);
    i=0;
    while(output[i]!=-1){
        fprintf(file,"%d",output[i]);
        i++;
    }
    fclose(file);
    free(output);
    return x;
}