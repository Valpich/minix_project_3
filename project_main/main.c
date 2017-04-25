#include <stdio.h>
#include <stdlib.h>
#include "repair.h"


/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
    
   // int x=inodewalker();
    
   // int z=zonewalker();
   // directorywalker("hello");
   // recovery();
    int inode = 2;
    int operation = 1; // inode bitmap damage
    damage(inode, operation, NULL);
    return 0;
}
