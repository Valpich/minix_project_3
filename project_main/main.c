#include "../servers/mfs/fs.h"
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

#include "repair.h"

#define btoa64(b)   (mul64u(b, BLOCK_SIZE))
#define BLOCK_SIZE 4096

unsigned int WORDS_PER_BLOCK;    /* # words in a block */
char * rwbuf;            /* one block buffer cache */
block_t thisblk;       /* block in buffer cache */
int file_descriptor;

/*===========================================================================*
 *              devio          *
 *===========================================================================*/
void devio(bno, dir)
block_t bno;
int dir;
{
  int r;

  if(!BLOCK_SIZE) exit(3);
  if (dir == READING && bno == thisblk) return;
  thisblk = bno;

#if 1
printf("%s at block %5d\n", dir == READING ? "reading " : "writing", bno);
#endif
printf("dev is %d SEEK_SET is  %d.\n", file_descriptor, SEEK_SET);
printf("file_descriptor is %d.\n",file_descriptor);
    if(file_descriptor != -1 ){
        printf("file_descriptor open is %d\n", file_descriptor);
      r= lseek64(file_descriptor, btoa64(bno), SEEK_SET, NULL);
      if (r != 0)
        printf("lseek64 failed");
      if (dir == READING) {
        if (read(file_descriptor, rwbuf, BLOCK_SIZE) == BLOCK_SIZE)
            return;
      } else {
        if (write(file_descriptor, rwbuf, BLOCK_SIZE) == BLOCK_SIZE){
            return;
        }
      }

      printf("%s: can't %s block %ld (error = 0x%x)\n", "fs_repair", dir == READING ? "read" : "write", (long) bno, errno);
      if (dir == READING) {
        printf("Continuing with a zero-filled block.\n");
        memset(rwbuf, 0, BLOCK_SIZE);
        return;
      }
      printf("");
  }else{
        printf("file_descriptor is not open.\n");
  }
}

/*===========================================================================*
 *              devwrite          *
 *===========================================================================*/
void devwrite(block, offset, buf, size)
long block;
long offset;
char *buf;
int size;
{
  if(!BLOCK_SIZE) exit(4);
  if (offset >= BLOCK_SIZE)
  {
    block += offset/BLOCK_SIZE;
    offset %= BLOCK_SIZE;
  }
  if (size != BLOCK_SIZE) devio(block, READING);
  memmove(&rwbuf[offset], buf, size);
  devio(block, WRITING);
}

/*===========================================================================*
 *              dumpbitmap          *
 *===========================================================================*/
void dumpbitmap(bitmap, bno, nblk)
bitchunk_t *bitmap;
block_t bno;
int nblk;
{
  register int i;
  register bitchunk_t *p = bitmap;
  if(!(rwbuf = malloc(BLOCK_SIZE))) exit(2);
  for (i = 0; i < nblk; i++, bno++, p += WORDS_PER_BLOCK){
    devwrite(bno, 0, (char *) p, BLOCK_SIZE);
  }
}

/*===========================================================================*
 *				print_bitmap	     		*
 *===========================================================================*/
void print_bitmap(bitmap)
bitchunk_t * bitmap;
{
    int nblk;
    if (type == IMAP){
        nblk = N_IMAP;
        puts("Printing inode bitmap!");
    }else if(type == ZMAP){
        nblk = N_ZMAP;
        puts("Printing zone bitmap!");
    }
    for (int j = 0; j < FS_BITMAP_CHUNKS(BLK_SIZE)*nblk; ++j){
        printf("%s\n", int2binstr(bitmap[j]));
        static int i = 0;
        if(i == 0){
        	sleep(5);
        	i++;
        }
    }
    puts("End of bitmap printing.");
}

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
    
   // int x=inodewalker();
   // int z=zonewalker();
   // directorywalker("hello");
   // recovery();
	file_descriptor = open("/dev/c0d0p0s1",O_RDWR);
	printf("file_descriptor is %d \n",file_descriptor );
	sleep(2);
    int inode = 2;
    int operation = 1; // inode bitmap damage
    damage(inode, operation, NULL);
    FILE * file = fopen("map.txt","w");
    bitchunk_t *corrupted_map;
    char chunk;
    int i = 0;
    char * c;
    while((c = getc(file_descriptor)) != EOF) {
    	
    	putchar(c);
	}
    return 0;
}
