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


long inodeFinder(char* dir, char* file) {


  int len   = strlen(dir) + strlen(file) + 2;
  char *total = malloc(len);

  strlcat(total, dir, len);
  strlcat(total, "/", len);
  strlcat(total, file, len);

  int fd;
  long inode;
  fd = open(total, "r+");

  if (fd < 0) {
      // some error occurred while opening the file
      // use [perror("Error opening the file");] to get error description
  }

  struct stat file_stat;
  int ret;
  ret = fstat(fd, &file_stat);
  if (ret < 0) {
     // error getting file stat
  }

  return inode = file_stat.st_ino;
}

/* List the files in "dir_name". */
static void list_dir (const char * dir_name) {
    DIR * d;

    /* Open the directory specified by "dir_name". */

    d = opendir (dir_name);

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
	printf ("Inode: %ld, Dir-Name: %s/%s\n", inodeFinder(dir_name, d_name), dir_name, d_name);

#if 0
	/* If you don't want to print the directories, use the
	   following line: */

        if (! (entry->d_type & DT_DIR)) {
	    printf ("%s/%s\n", dir_name, d_name);
	}

#endif /* 0 */

struct stat s;
stat(entry->d_name, &s);
if ((s.st_mode & S_IFMT) != S_IFDIR) {

      /* Check that the directory is not "d" or d's parent. */

      if (strcmp (d_name, "..") != 0 &&
          strcmp (d_name, ".") != 0) {
          int path_length;
          char path[PATH_MAX];

          path_length = snprintf (path, PATH_MAX,
                                  "%s/%s", dir_name, d_name);
          printf ("%s\n", path);
          if (path_length >= PATH_MAX) {
              fprintf (stderr, "Path length has got too long.\n");
              exit (EXIT_FAILURE);
          }
          /* Recursively call "list_dir" with the new path. */
          list_dir (path);
      }
  }

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

    DIR *mydir;
        struct dirent *myfile;
        struct stat mystat;

        char buf[512];
        mydir = opendir(p);
        while((myfile = readdir(mydir)) != NULL)
        {
            sprintf(buf, "%s/%s", p, myfile->d_name);
            stat(buf, &mystat);
            printf("%zu",mystat.st_size);
            printf(" %s\n", myfile->d_name);
            printf("Inode: %d\n", mystat->st_ino)
        }
        closedir(mydir);


    //list_dir (p);
    return 0;
}
