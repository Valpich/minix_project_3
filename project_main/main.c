#include <stdio.h>
#include <stdlib.h>
#include "repair.h"


/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
    int * r=calloc(1017088,sizeof(long));
    
    int x=inodewalker(r);
    
    printf("x: %d\n",x);

    printf("r: %d  %d  %d\n",r[0],r[1],r[2]);
    
    FILE * file=fopen("blocks.txt","w");
    
    int i=0;
    while(r[i]!=0){
        i++;
    }
    printf("number of blocks referenced by inodes: %d\n",i);
    i=0;
    while(r[i]!=0){
        fprintf(file,"%d\n",r[i]);
        i++;
    }
    
    fclose(file);
    free(r);
    printf("Blocks referenced by inodes saved in blocks.txt\n");
    return 0;
}
