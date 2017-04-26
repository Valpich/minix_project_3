#include "../servers/mfs/fs.h"

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
    
   // int z=zonewalker();
   // directorywalker("hello");
   // recovery();
	int fd = open("/dev/c0d0p0s0", O_RDWR | O_NONBLOCK, 0);
    printf("file is %d.\n",file);
    int inode = 2;
    int operation = 1; // inode bitmap damage
    damage(inode, operation, fd, NULL);
    close(file);
    return 0;
}
