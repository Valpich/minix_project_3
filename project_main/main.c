#include <stdio.h>
#include <stdlib.h>
#include "repair.h"

#include "fs.h"
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minix/ipc.h>
#include "const.h"
#include "inode.h"
#include "type.h"
#include "mfsdir.h"
#include "super.h"
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include "buf.h"

#include <sys/stat.h>
#include <dirent.h>

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){

    printf("Getting super node from device %llu ...\n", fs_dev);

    sb = get_super(fs_dev);int x=inodewalker(fs_dev);
    
    //printf("x: %d\n",x);
    //printf("r: %d  %d  %d\n",r[0],r[1],r[2]);
    
    FILE * file=fopen("blocks.txt","w");
    
    int i=0;

    
    fclose(file);
   // free(r);
    printf("Blocks referenced by inodes saved in blocks.txt\n");
    return 0;
}
