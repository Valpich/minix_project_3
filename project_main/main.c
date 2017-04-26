#include "../servers/mfs/fs.h"

#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <minix/config.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/u64.h>
#include "../servers/mfs/const.h"
#include "../servers/mfs/inode.h"
#include "../servers/mfs/type.h"
#include "../servers/mfs/mfsdir.h"
#include <minix/fslib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <a.out.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <minix/vfsif.h>
#include <minix/const.h>
#include <minix/endpoint.h>
#include <stdlib.h>
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
	int file = open("/dev/c0d0p0s0", O_RDWR | O_NONBLOCK, 0);
    printf("file is %d.\n",file);
    sleep(5);
    int inode = 2;
    int operation = 1; // inode bitmap damage
    damage(inode, operation, NULL);
    return 0;
}
