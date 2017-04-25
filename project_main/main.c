#include <stdio.h>
#include <stdlib.h>
#include "repair.h"


/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
    int * r=calloc(1017088,sizeof(long));
    
    int x=inodewalker(r);
    
    int z=zonewalker(r);
    directorywalker("hello");
    recovery(r);
    int inode = 2;
    damage(r,&inode);
    return 0;
}
