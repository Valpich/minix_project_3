/*
I had need in something like this not so long ago (my difference is I
needed recursive scan) so I added only some comments... Sorry for recursion
but I was short of time and this was only part of internal one-time tool.
*/

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
#include <sys/stat.h>

/* Print all the dirs starting from <path> [maybe recursive]. */
int print_dirs(const char *path, int recursive)
{
    struct dirent *direntp = NULL;
    DIR *dirp = NULL;
    size_t path_len;

    /* Check input parameters. */
    if (!path)
        return -1;
    path_len = strlen(path);

    if (!path || !path_len || (path_len > _POSIX_PATH_MAX))
        return -1;

    /* Open directory */
    dirp = opendir(path);
    if (dirp == NULL)
        return -1;

    while ((direntp = readdir(dirp)) != NULL)
    {

      /* For every directory entry... */
      struct stat file_stat;
      char full_name[_POSIX_PATH_MAX + 1];

      /*

        FDP

      */

      int inode;
      int fd;






        /* Calculate full name, check we are in file length limts */
        if ((path_len + strlen(direntp->d_name) + 1) > _POSIX_PATH_MAX)
            continue;

        strcpy(full_name, path);
        if (full_name[path_len - 1] != '/')
            strcat(full_name, "/");
        strcat(full_name, direntp->d_name);

        fd = open(full_name, O_RDONLY);

        if (fd < 0) {
            // some error occurred while opening the file
            // use [perror("Error opening the file");] to get error description
        }

        int ret;
        ret = fstat (fd, &file_stat);
        if (ret < 0) {
           // error getting file stat
        }

        inode = file_stat.st_ino;

        /* Ignore special directories. */
        if ((strcmp(direntp->d_name, ".") == 0) ||
            (strcmp(direntp->d_name, "..") == 0))
            continue;

            printf("Inode: %d, File:%s\n", inode, full_name);
            if (recursive)
                print_dirs(full_name, 1);
    }

    /* Finalize resources. */
    (void)closedir(dirp);
    return 0;
}

/* We are taking first argument as initial path name. */
int main(int argc, const char* argv[])
{
    if (argc < 2)
        return -1;

    print_dirs(argv[1], 1);
    return 0;
}
