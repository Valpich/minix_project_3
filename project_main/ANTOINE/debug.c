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

/* Print the pathname given by a linked list pointed to by `sp'.  The
 * names are in reverse order.
 */
void printrec(struct stack *sp)
{
  if (sp->st_next != 0) {
	printrec(sp->st_next);
	putchar('/');
	printname(sp->st_dir->mfs_d_name);
  }
}

/* Print the current pathname.  */
void printpath(mode, nlcr)
int mode;
int nlcr;
{
  if (ftop->st_next == 0)
	putchar('/');
  else
	printrec(ftop);
  switch (mode) {
      case 1:
	printf(" (ino = %u, ", ftop->st_dir->d_inum);
	break;
      case 2:
	printf(" (ino = %u)", ftop->st_dir->d_inum);
	break;
  }
  if (nlcr) printf("\n");
}

int main ()
{
    printpath(1, 0);
}
