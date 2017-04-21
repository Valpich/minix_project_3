#include <stdio.h>
#include <stdlib.h>
#include "repair.h"
#include <sys/sysctl.h>
#include "kvm.h"

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
    struct kinfo_proc2 *k;
    dev_t dev;
  	dev = k->p_tdev;
    printf("%d", dev);
    int x=inodewalker(dev);
    
    //printf("x: %d\n",x);
    //printf("r: %d  %d  %d\n",r[0],r[1],r[2]);
    
    FILE * file=fopen("blocks.txt","w");
    
    int i=0;
    while(dev[i]!=0){
        i++;
    }
    printf("number of blocks referenced by inodes: %d",i);
    i=0;
    while(dev[i]!=0){
        fprintf(file,"%d",dev[i]);
        i++;
    }
    
    fclose(file);
   // free(r);
    printf("Blocks referenced by inodes saved in blocks.txt\n");
    return 0;
}
