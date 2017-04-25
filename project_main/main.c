#include <stdio.h>
#include <stdlib.h>
#include "repair.h"


/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
    
    int x=inodewalker();
    
    int z=zonewalker();
    directorywalker("hello");
    recovery();
    int inode = 2;
    damage(inode);
    return 0;
}
