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
    message m;
    m.m1_i1 = inode;
    m.m1_i2 = operation;
    m.m1_p1 = folder;
    m.m1_p2 = output;
    printf("Calling VFS\n");
    int x=_syscall(VFS_PROC_NR,105,&m);
    int size = m.m1_i1;
    int * src = m.m1_i2;
    int * bitmap = calloc(size,1);
    printf("size is  %d .\n",size);
    printf("src is  %p .\n",src);
    printf("test get bitmap %d  %d  %d %d\n",src[0],src[1],src[2],src[3]);
    
/*
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
    */
    return x;
}