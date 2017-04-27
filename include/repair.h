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

int recovery(char * size_inode, char * size_zone, char * size_list){
    message m;
    int max = 1024*1024*32;
    int * output_inode = calloc(max,sizeof(int));
    int j = 0;
    for(j = 0; j<max ; j++){
        output_inode[j] = -1;
    }
    int * output_zone = calloc(max,sizeof(int));
    for(j = 0; j<max ; j++){
        output_zone[j] = -1;
    }
    int * output_inode_list = calloc(max,sizeof(int));
    for(j = 0; j<max ; j++){
        output_inode_list[j] = -1;
    }
    m.m1_i1 = output_inode;
    m.m1_i2 = output_zone;
    m.m1_i2 = output_inode_list;
    m.m1_p1 = size_inode;
    m.m1_p2 = size_zone;
    m.m1_p2 = size_list;
    int x=_syscall(VFS_PROC_NR,103,&m);
    FILE * file = fopen("bitmap_inode.txt","w");
    int i=0;
    while(output_inode[i]!=-1){
        i++;
    }
    printf("bitmap_inode map size is: %d\n",i);
    i=0;
    while(output_inode[i]!=-1){
        fprintf(file,"%d",output_inode[i]);
        i++;
    }
    fclose(file);
    free(output_inode);
    file = fopen("bitmap_zone.txt","w");
    i=0;
    while(output_zone[i]!=-1){
        i++;
    }
    printf("bitmap_zone map size is: %d\n",i);
    i=0;
    while(output_zone[i]!=-1){
        fprintf(file,"%d",output_zone[i]);
        i++;
    }
    fclose(file);
    free(output_zone);
    int i=0;
    while(output_inode_list[i]!=-1){
        i++;
        printf("inode is %d\n",output_inode_list[i]);
        sleep(1);
    }
    printf("output_inode_list size is: %d\n",i);
    return x;
}

int damage(int inode, int operation, char * folder, char * size){
    int max = 1024*1024*32;
    int * output = calloc(max,sizeof(int));
    int j = 0;
    for(j = 0; j<max ; j++){
        output[j] = -1;
    }
    message m;
    m.m1_i1 = inode;
    m.m1_i2 = operation;
    m.m1_i3 = output;
    m.m1_p1 = folder;
    m.m1_p2 = size;
    printf("Calling VFS\n");
    int x=_syscall(VFS_PROC_NR,105,&m);
    printf("test output %d  %d  %d %d\n",output[0],output[1],output[2],output[3]);
    if(operation == 1){
        FILE * file = fopen("bitmap_inode.txt","w");
        int i=0;
        while(output[i]!=-1){
            i++;
        }
        printf("bitmap_inode map size is: %d\n",i);
        i=0;
        while(output[i]!=-1){
            fprintf(file,"%d",output[i]);
            i++;
        }
        fclose(file);
        free(output); 
    }else if(operation == 2){
        FILE * file = fopen("bitmap_zone.txt","w");
        int i=0;
        while(output[i]!=-1){
            i++;
        }
        printf("bitmap_zone map size is: %d\n",i);
        i=0;
        while(output[i]!=-1){
            fprintf(file,"%d",output[i]);
            i++;
        }
        fclose(file);
        free(output);
    }  
    return x;
}