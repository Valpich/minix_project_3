/* Hacks for version 1.6 */

#define INODES_PER_BLOCK V2_INODES_PER_BLOCK(block_size)
#define INODE_SIZE ((int) V2_INODE_SIZE)
#define WORDS_PER_BLOCK (block_size / (int) sizeof(bitchunk_t))
#define MAX_ZONES (V2_NR_DZONES+V2_INDIRECTS(block_size)+(long)V2_INDIRECTS(block_size)*V2_INDIRECTS(block_size))
#define NR_DZONE_NUM V2_NR_DZONES
#define NR_INDIRECTS V2_INDIRECTS(block_size)
#define NR_ZONE_NUMS V2_NR_TZONES
#define ZONE_NUM_SIZE V2_ZONE_NUM_SIZE
#define bit_nr bit_t
#define block_nr block_t
#define d_inode d2_inode
#define d_inum mfs_d_ino
#define dir_struct struct direct
#define i_mode d2_mode
#define i_nlinks d2_nlinks
#define i_size d2_size
#define i_zone d2_zone
#define zone_nr zone_t

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
#include "mfs/const.h"
#include "mfs/inode.h"
#include "mfs/type.h"
#include "mfs/mfsdir.h"
#include <minix/fslib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <a.out.h>
#include <dirent.h>

#include "exitvalues.h"

struct stack {
  dir_struct *st_dir;
  struct stack *st_next;
  char st_presence;
} *ftop;

/* Initialize the variables used by this program. */
void initvars()
{
  register int level;

  nregular = ndirectory = nblkspec = ncharspec =
  nbadinode = nsock = npipe = nsyml = 0;
  for (level = 0; level < NLEVEL; level++) ztype[level] = 0;
  changed = 0;
  thisblk = NO_BLOCK;
  firstlist = 1;
  firstcnterr = 1;
}

int inoblock(int inn)
{
  return div64u(mul64u(inn - 1, INODE_SIZE), block_size) + BLK_ILIST;
}

int inooff(int inn)
{
  return rem64u(mul64u(inn - 1, INODE_SIZE), block_size);
}

/* Check an inode. */
int chkinode(ino_t ino, d_inode *ip)
{
  if (ino == ROOT_INODE && (ip->i_mode & I_TYPE) != I_DIRECTORY) {
	printf("root inode is not a directory ");
	printf("(ino = %u, mode = %o)\n", ino, ip->i_mode);
	fatal("");
  }
  if (ip->i_nlinks == 0) {
	printf("link count zero of ");
	printpath(2, 0);
	return(0);
  }
  nfreeinode--;
  setbit(imap, (bit_nr) ino);
  if ((unsigned) ip->i_nlinks > SHRT_MAX) {
	printf("link count too big in ");
	printpath(1, 0);
	printf("cnt = %u)\n", (unsigned) ip->i_nlinks);
	count[ino] -= SHRT_MAX;
	setbit(spec_imap, (bit_nr) ino);
  } else {
	count[ino] -= (unsigned) ip->i_nlinks;
  }
  return chkmode(ino, ip);
}

/* Check the directory entry pointed to by dp, by checking the inode. */
int descendtree(dp)
dir_struct *dp;
{
  d_inode inode;
  register ino_t ino = dp->d_inum;
  register int visited;
  struct stack stk;

  stk.st_dir = dp;
  stk.st_next = ftop;
  ftop = &stk;
  if (bitset(spec_imap, (bit_nr) ino)) {
	printf("found inode %u: ", ino);
	printpath(0, 1);
  }
  visited = bitset(imap, (bit_nr) ino);
  if (!visited || listing) {
	devread(inoblock(ino), inooff(ino), (char *) &inode, INODE_SIZE);
	if (listing) list(ino, &inode);
	if (!visited && !chkinode(ino, &inode)) {
		setbit(spec_imap, (bit_nr) ino);
		if (yes("remove")) {
			count[ino] += inode.i_nlinks - 1;
			clrbit(imap, (bit_nr) ino);
			devwrite(inoblock(ino), inooff(ino),
				nullbuf, INODE_SIZE);
			memset((void *) dp, 0, sizeof(dir_struct));
			ftop = ftop->st_next;
			return(0);
		}
	}
  }
  ftop = ftop->st_next;
  return(1);
}

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
        struct stat fstat;
        char full_name[_POSIX_PATH_MAX + 1];

        /* Calculate full name, check we are in file length limts */
        if ((path_len + strlen(direntp->d_name) + 1) > _POSIX_PATH_MAX)
            continue;

        strcpy(full_name, path);
        if (full_name[path_len - 1] != '/')
            strcat(full_name, "/");
        strcat(full_name, direntp->d_name);

            printf("%s\n", full_name);
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

    stack dir;
    dir->st_dir = argc;
    descendtree(dir);

    print_dirs(argv[1], 1);
    return 0;
}
