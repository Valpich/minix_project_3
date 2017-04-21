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
    printf("%d", k->p_tdev);
    int x=inodewalker(dev);
    
    //printf("x: %d\n",x);
    //printf("r: %d  %d  %d\n",r[0],r[1],r[2]);
    
    FILE * file=fopen("blocks.txt","w");
    
    int i=0;

    
    fclose(file);
   // free(r);
    printf("Blocks referenced by inodes saved in blocks.txt\n");
    return 0;
}
