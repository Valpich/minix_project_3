#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // for open
/* "readdir" etc. are defined here. */
#include <dirent.h>
/* limits.h defines "PATH_MAX". */
#include <limits.h>

#include <fcntl.h>

#include <sys/stat.h>


int main ()
{
    char * p = malloc(sizeof(char) * 128);
    printf("Enter you directory: ");
    scanf("%126s",p);

    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;

    char buf[512];
    mydir = opendir(p);
    while((myfile = readdir(mydir)) != NULL) {
        sprintf(buf, "%s/%s", p, myfile->d_name);
        stat(buf, &mystat);
        //printf("%zu",mystat.st_size);
        printf("DIR: %s\n\t", myfile->d_name);
        printf("Inode: %d\n\t", mystat.st_ino);
        if ((mystat.st_mode & S_IFMT) != S_IFDIR) {
          printf("DIR\n");
        }else{
          printf("NO DIR\n");
        }

    }
    closedir(mydir);

    return 0;
}
