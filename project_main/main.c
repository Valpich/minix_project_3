#include "../servers/mfs/fs.h"
#include "../servers/mfs/fs_repair.h"
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

#include "repair.h"


/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
    
   // int x=inodewalker();
    file_descriptor = open("/dev/c0d0p0s0", O_RDWR | O_NONBLOCK, 0);
    int z=zonewalker();
   // directorywalker("hello");
   // recovery();
    int inode = 2;
    int operation = 1; // inode bitmap damage
    damage(inode, operation, NULL);
    return 0;
}
