#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <minix/ipc.h>
#include "inode.h"
#include "mfsdir.h"
#include "super.h"
#include "type.h"
#include "buf.h"


int main() {
    printf("Hello, World");
    getbitmaps();
    lsuper();
    printf("Hello, World2");
    return 0;
}