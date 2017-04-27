#include <stdlib.h>
#include <unistd.h>
#include <ftw.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#define USE_FDS 15


int print_entry(const char *filepath, const struct stat *info, const int typeflag, struct FTW *pathinfo) {
    /* const char *const filename = filepath + pathinfo->base; */
    const double bytes = (double)info->st_size; /* Not exact if large! */

    char   *target;
    size_t  maxlen = 1023;
    ssize_t len;

    int status;
    struct stat st_buf;

    /* Get the status of the file system object. */
    status = stat (filepath, &st_buf);
    if (status != 0) {
        printf ("Error, errno = %d\n", errno);
        return 1;
    }

    if (typeflag == FTW_SL) {

        while (1) {
            target = malloc(maxlen + 1);
            if (target == NULL) {
                return ENOMEM;
            }

            len = readlink(filepath, target, maxlen);
            if (len == (ssize_t)-1) {
                const int saved_errno = errno;
                free(target);
                return saved_errno;
            }
            if (len >= (ssize_t)maxlen) {
                free(target);
                maxlen += 1024;
                continue;
            }

            target[len] = '\0';
            break;
        }

        printf(" %s -> %s\n", filepath, target);
        free(target);

    } else if (typeflag == FTW_SLN) {
        printf(" %s (dangling symlink)\n", filepath);
    } else if (typeflag == FTW_F) {
        printf("Inode: %llu ", st_buf.st_ino);
        printf("%s\n", filepath);
    } else if (typeflag == FTW_D || typeflag == FTW_DP) {
        printf("Inode: %llu ", st_buf.st_ino);
        printf("%s\n", filepath);
    } else if (typeflag == FTW_DNR) {
      printf(" %s/ (unreadable)\n", filepath);
    } else {
      printf(" %s (unknown)\n", filepath);
    }

  return 0;
}

int print_directory_tree(const char *const dirpath)
{
    int result;

    /* Invalid directory path? */
    if (dirpath == NULL || *dirpath == '\0') {
        return errno = EINVAL;
    }

    result = nftw(dirpath, print_entry, USE_FDS, FTW_PHYS);
    if (result >= 0) {
        errno = result;
    }

    return errno;
}

int main(int argc, char *argv[])
{
    int arg;

    if (argc < 2) {

        if (print_directory_tree(".")) {
            fprintf(stderr, "%s.\n", strerror(errno));
            return EXIT_FAILURE;
        }

    } else {

        for (arg = 1; arg < argc; arg++) {
            if (print_directory_tree(argv[arg])) {
                fprintf(stderr, "%s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }

    }

    return EXIT_SUCCESS;
}
