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

int inodeFinder(char* dir, char* file) {

  int len   = strlen(dir) + strlen(file) + 2;
  char *total = malloc(len);

  strlcat(total, dir, len);
  strlcat(total, "/", len);
  strlcat(total, file, len);

  int fd, inode;
  struct stat buf;

  int ret;

  fd = open(total, O_RDONLY);

  if (fd < 0) {
    //perror ("open");
    //return 0;
  }

  fstat(fd, &buf);
  if ( ret <0 ) {
       perror ("fstat");
       return -1;
  }

  int d = close(fd);

  return buf.st_ino;
}

/* List the files in "dir_name". */
static void list_dir (const char * dir_name) {
    DIR * d;

    /* Open the directory specified by "dir_name". */

    d = opendir (dir_name);

    /* Check it was opened. */
    if (! d) {
        fprintf (stderr, "Cannot open directory '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent * entry;
        const char * d_name;

        /* "Readdir" gets subsequent entries from "d". */
        entry = readdir (d);
        if (! entry) {
            /* There are no more entries in this directory, so break
               out of the while loop. */
            break;
        }
        d_name = entry->d_name;
        /* Print the name of the file and directory. */
	printf ("Inode: %d, Dir-Name: %s/%s\n", inodeFinder(dir_name, d_name), dir_name, d_name);

#if 0
	/* If you don't want to print the directories, use the
	   following line: */

        if (! (entry->d_type & DT_DIR)) {
	    printf ("%s/%s\n", dir_name, d_name);
	}

#endif /* 0 */

    }
    /* After going through all the entries, close the directory. */
    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
}

int main ()
{
    char * p = malloc(sizeof(char) * 128);
    printf("Enter you directory: ");
    scanf("%126s",p);
    list_dir (p);
    return 0;
}
