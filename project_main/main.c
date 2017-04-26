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
unsigned int chunk_size = sizeof(unsigned int) * CHAR_BIT;

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
 *				int2binstr		     		*
 *===========================================================================*/
char * int2binstr(i)
unsigned int i;
{
    size_t bits = sizeof(unsigned int) * CHAR_BIT;
    char * str = malloc(bits + 1);
    if(!str) return NULL;
    str[bits] = 0; 
    chunk_size = bits;
    unsigned u = *(unsigned *)&i;
    for(; bits--; u >>= 1)
        str[bits] = u & 1 ? '1' : '0';  
    return str;
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
    puts("Printing bitmap!");
    for (int j = 0; j < FS_BITMAP_CHUNKS(BLOCK_SIZE)*nblk; ++j){
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
 *				alloc			     		*
 *===========================================================================*/
char * alloc(nelem, elsize)
unsigned nelem, elsize;
{
    char *p;
    if ((p = (char *)malloc((size_t)nelem * elsize)) == 0) {
        exit(6);
    }
    memset((void *) p, 0, (size_t)nelem * elsize);
    return(p);
}

/*===========================================================================*
 *				alloc_bitmap	     		*
 *===========================================================================*/
bitchunk_t * alloc_bitmap(nblk)
int nblk;
{
    register bitchunk_t *bitmap;
    bitmap = (bitchunk_t *) alloc((unsigned) nblk, BLOCK_SIZE);
    *bitmap |= 1;
    return bitmap;
}

/*===========================================================================*
 *				free_bitmap		     		*
 *===========================================================================*/
void free_bitmap(p)
bitchunk_t *p;
{
    free((char *) p);
}

/*===========================================================================*
 *				reverse_bits		     		*
 *===========================================================================*/
unsigned int reverse_bits(num)
unsigned int num;
{
    unsigned int count = sizeof(num) * 8 - 1;
    unsigned int reverse_num = num;
     
    num >>= 1; 
    while(num)
    {
       reverse_num <<= 1;       
       reverse_num |= num & 1;
       num >>= 1;
       count--;
    }
    reverse_num <<= count;
    return reverse_num;
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
    int inode = 2;
    int operation = 1; // inode bitmap damage
    char * size = malloc(sizeof(char)*sizeof(int));
    damage(inode, operation, NULL);
    FILE * file = fopen("map.txt","r");
    int i = 0;
    fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);  //same as rewind(f);
	printf("file size is %lu\n", fsize);
	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, file);
	char * chunk = malloc((chunk_size+1)*sizeof(char));
	chunk[chunk_size] = 0;
	int N_MAP = fsize/(chunk_size*BLOCK_SIZE/sizeof(bitchunk_t));
	printf("N_MAP is %d\n", N_MAP);
	bitchunk_t *corrupted_map =alloc_bitmap(N_MAP);
	for (int i = 0; i < N_MAP; i++){
		int k;
		for (k = 0; k <= chunk_size -1 ; k++) {
			chunk[chunk_size-k-1] = string[i*chunk_size +k];
        }
        char * pEnd;
        unsigned int update = strtol(chunk,&pEnd,2);
        if(update != 0){
        	printf("update is %u \n", update);
        	sleep(1);
        }
        corrupted_map[i]=update;
	}
	print_bitmap(corrupted_map);
	fclose(file);
	close(file_descriptor);
    return 0;
}
