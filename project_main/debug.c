#define _POSIX_SOURCE 1			/* for signal handling */
#define _SYSTEM 1			/* for negative error values */
#define _MINIX 1

#include <minix/config.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/ipc.h>
#include <minix/com.h>
#include <minix/callnr.h>
#include <minix/safecopies.h>
#include <minix/vfsif.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <sys/param.h>

#if DEBUG
#define dprintf(x) printf x
#else
#define dprintf(x)
#endif

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/queue.h>

#include "hgfs.h"

#include "type.h"
#include "const.h"
#include "proto.h"
#include "glo.h"

#include "inode.h"



int do_stat(char *path_tmp)
{
/* Retrieve inode status.
 */
  struct inode *ino;
  struct hgfs_attr attr;
  struct stat stat;
  char *path;

  int len = strlen(path_tmp) + 1;
  strlcat(path, path_tmp, len);

  ino_t ino_nr;
  int r;

  ino_nr = m_in.REQ_INODE_NR;

  /* Don't increase the inode refcount: it's already open anyway */
  if ((ino = find_inode(ino_nr)) == NULL)
	return EINVAL;

  attr.a_mask = HGFS_ATTR_MODE | HGFS_ATTR_SIZE | HGFS_ATTR_ATIME |
		HGFS_ATTR_MTIME | HGFS_ATTR_CTIME;

  if ((r = verify_inode(ino, path, &attr)) != OK)
	return r;

  stat.st_dev = state.dev;
  stat.st_ino = ino_nr;
  stat.st_mode = get_mode(ino, attr.a_mode);
  stat.st_uid = opt.uid;
  stat.st_gid = opt.gid;
  stat.st_rdev = NO_DEV;
  stat.st_size = ex64hi(attr.a_size) ? ULONG_MAX : ex64lo(attr.a_size);
  stat.st_atime = attr.a_atime;
  stat.st_mtime = attr.a_mtime;
  stat.st_ctime = attr.a_ctime;

  return stat.st_ino;
  /* We could make this more accurate by iterating over directory inodes'
   * children, counting how many of those are directories as well.
   * It's just not worth it.
   */
  stat.st_nlink = 0;
  if (ino->i_parent != NULL) stat.st_nlink++;
  if (IS_DIR(ino)) {
	stat.st_nlink++;
	if (HAS_CHILDREN(ino)) stat.st_nlink++;
  }

  return sys_safecopyto(m_in.m_source, m_in.REQ_GRANT, 0,
	(vir_bytes) &stat, sizeof(stat), D);
}




int inodeFinder(char* dir, char* file) {

  int len   = strlen(dir) + strlen(file) + 2;
  char *total = malloc(len);

  strlcat(total, dir, len);
  strlcat(total, "/", len);
  strlcat(total, file, len);

  int inode;
  int fd;

  return do_stat(total);

  fd = open(total, O_RDONLY);

/*
  if (fd < 0) {
      // some error occurred while opening the file
      // use [perror("Error opening the file");] to get error description
  }

  struct stat file_stat;
  int ret;
  ret = fstat (fd, &file_stat);
  if (ret < 0) {
     // error getting file stat
  } */

  //return inode = file_stat.st_ino;
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
