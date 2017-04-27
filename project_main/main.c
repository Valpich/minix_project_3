#include "../servers/mfs/fs.h"
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
#include "../servers/mfs/const.h"
#include "../servers/mfs/inode.h"
#include "../servers/mfs/type.h"
#include "../servers/mfs/mfsdir.h"
#include <minix/fslib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <a.out.h>
#include <dirent.h>

#include "repair.h"

#define btoa64(b)   (mul64u(b, BLOCK_SIZE))
#define BLOCK_SIZE 4096
block_t BLK_IMAP = 2;			 	 /* starting block for imap */

unsigned int WORDS_PER_BLOCK = BLOCK_SIZE / (int)sizeof(bitchunk_t);;    /* # words in a block */
char * rwbuf;            /* one block buffer cache */
block_t thisblk;       /* block in buffer cache */
int file_descriptor;
unsigned int chunk_size = sizeof(unsigned int) * CHAR_BIT;
int N_MAP;
int N_IMAP;
int N_ZMAP;

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

#if 0
printf("%s at block %5d\n", dir == READING ? "reading " : "writing", bno);
#endif
  if(file_descriptor != -1 ){
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
  puts("Dumping bitmap on the disk");
  static int j = 0;
  for (i = 0; i < nblk; i++, bno++, p += WORDS_PER_BLOCK){
  	if(i<3){
  		printf("dumping %s\n", int2binstr(*p));
  		sleep(1);
  		j++;
  	}
    devwrite(bno, 0, (char *) p, BLOCK_SIZE);
  }
  j = 0;
  sleep(5);
  puts("bitmap saved on the disk");
}

/*===========================================================================*
 *				print_bitmap_2	     		*
 *===========================================================================*/
void print_bitmap_2(bitmap, block_size)
bitchunk_t * bitmap;
int block_size;
{
  puts("Printing bitmap!");
  for (int j = 0; j < FS_BITMAP_CHUNKS(BLOCK_SIZE)*block_size; ++j){
    printf("%s\n", int2binstr(bitmap[j]));
    if(bitmap[j] != 0){
     sleep(5);
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
  if ((p = (char *)calloc((size_t)nelem , elsize)) == 0) {
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
 *              compare_bitmaps_2          *
 *===========================================================================*/
int compare_bitmaps_2(bitmap, bitmap2, block_size)
bitchunk_t * bitmap;
bitchunk_t * bitmap2;
int block_size;
{
  int j;
  int corrupted = 0;
  char * chunk;
  char * chunk2;
  for(j=0; j<FS_BITMAP_CHUNKS(BLOCK_SIZE)*block_size; ++j){
    chunk = int2binstr(bitmap[j]);
    chunk2 = int2binstr(bitmap2[j]);
    int k = 0;
    int u = 0;
    for (k = strlen(chunk) -1; k >= 0 ; k--) {
      if(chunk[k] != chunk2[k]){
        corrupted++;
      }
      u++;
    }
  }
  if(corrupted != 0){
    printf("Found %d corrupted data between the two bitmap.\n", corrupted);
  }else{
    puts("No difference between bitmaps found.");
  }
  return corrupted;
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
 *				damage_inode		     		*
 *===========================================================================*/
void damage_inode(inode, device)
int inode;
const char * device;
{
	file_descriptor = open(device,O_RDWR);
	printf("file_descriptor is %d \n",file_descriptor );
  int operation = 1; // inode bitmap damage
  char size[10];
  damage(inode, operation, NULL, size);
  FILE * file = fopen("bitmap_inode.txt","r");
  int i = 0;
  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);  //same as rewind(f);
	printf("file size is %lu\n", fsize);
	printf("returned size is %s\n", size);
	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, file);
	char * chunk = calloc((chunk_size+1), sizeof(char));
	chunk[chunk_size] = 0;
	char * p_end;
  unsigned int received = strtol(size,&p_end,10);
  N_MAP = received;
  printf("N_MAP is %d\n", N_MAP);
  bitchunk_t *corrupted_map = alloc_bitmap(N_MAP);
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
 print_bitmap_2(corrupted_map, N_MAP);
 dumpbitmap(corrupted_map, BLK_IMAP, N_MAP);
 free(corrupted_map);
 fclose(file);
 close(file_descriptor);
}

/*===========================================================================*
 *        damage_zone            *
 *===========================================================================*/
void damage_zone(zone, device)
int zone;
const char * device;
{
  file_descriptor = open(device,O_RDWR);
  printf("file_descriptor is %d \n",file_descriptor );
  int operation = 2; // zone bitmap damage
  char size[10];
  damage(zone, operation, NULL, size);
  FILE * file = fopen("bitmap_zone.txt","r");
  int i = 0;
  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);  //same as rewind(f);
  printf("file size is %lu\n", fsize);
  printf("returned size is %s\n", size);
  char *string = malloc(fsize + 1);
  fread(string, fsize, 1, file);
  char * chunk = calloc((chunk_size+1), sizeof(char));
  chunk[chunk_size] = 0;
  char * p_end;
  unsigned int received = strtol(size,&p_end,10);
  N_MAP = received;
  printf("N_MAP is %d\n", N_MAP);
  bitchunk_t *corrupted_map = alloc_bitmap(N_MAP);
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
  print_bitmap_2(corrupted_map, N_MAP);
  dumpbitmap(corrupted_map, BLK_IMAP + N_MAP, N_MAP);
  free(corrupted_map);
  fclose(file);
  close(file_descriptor);
}

/*===========================================================================*
 *        damage_zone            *
 *===========================================================================*/
void recover_zone(zone, device)
int zone;
const char * device;
{
  damage_zone(zone,device);
}

/*===========================================================================*
 *        recover_inode            *
 *===========================================================================*/
void recover_inode(device)
const char * device;
{
  file_descriptor = open(device,O_RDWR);
  printf("file_descriptor is %d \n",file_descriptor );
  char size_inode[10];
  char size_zone[10];
  char size_list[10];
  recovery(size_inode,size_zone, size_list);
  FILE * file = fopen("bitmap_inode.txt","r");
  int i = 0;
  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);  //same as rewind(f);
  printf("file size is %lu\n", fsize);
  printf("returned size_inode is %s\n", size_inode);
  char *string = malloc(fsize + 1);
  fread(string, fsize, 1, file);
  char * chunk = calloc((chunk_size+1), sizeof(char));
  chunk[chunk_size] = 0;
  char * p_end;
  unsigned int received = strtol(size_inode,&p_end,10);
  N_IMAP = received;
  printf("N_IMAP is %d\n", N_IMAP);
  bitchunk_t *inode_map = alloc_bitmap(N_IMAP);
  for (int i = 0; i < N_IMAP; i++){
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
    inode_map[i]=update;
  }
  dumpbitmap(inode_map, BLK_IMAP , N_IMAP);
  fclose(file);
  file = fopen("bitmap_zone.txt","r");
  i = 0;
  fseek(file, 0, SEEK_END);
  fsize = ftell(file);
  fseek(file, 0, SEEK_SET);  //same as rewind(f);
  printf("file size is %lu\n", fsize);
  printf("returned size_zone is %s\n", size_zone);
  string = malloc(fsize + 1);
  fread(string, fsize, 1, file);
  chunk = calloc((chunk_size+1), sizeof(char));
  chunk[chunk_size] = 0;
  char * p_end_2;
  received = strtol(size_zone,&p_end_2,10);
  N_ZMAP = received;
  printf("N_ZMAP is %d\n", N_ZMAP);
  bitchunk_t * corrupted_map = alloc_bitmap(N_ZMAP);
  for (int i = 0; i < N_ZMAP; i++){
    int k;
    for (k = 0; k <= chunk_size -1 ; k++) {
      chunk[chunk_size-k-1] = string[i*chunk_size +k];
    }
    char * pEnd2;
    unsigned int update = strtol(chunk,&pEnd2,2);
    if(update != 0){
      printf("update is %u \n", update);
      sleep(1);
    }
    corrupted_map[i]=update;
  }
  dumpbitmap(corrupted_map, BLK_IMAP + N_IMAP, N_ZMAP);
  free(corrupted_map);
  fclose(file);
  file = fopen("list_inode.txt","r");
  i = 0;
  fseek(file, 0, SEEK_END);
  fsize = ftell(file);
  fseek(file, 0, SEEK_SET);  //same as rewind(f);
  printf("file size is %lu\n", fsize);
  printf("returned size_zone is %s\n", size_zone);
  string = malloc(fsize + 1);
  fread(string, fsize, 1, file);
  chunk = calloc((chunk_size+1), sizeof(char));
  chunk[chunk_size] = 0;
  printf("N_IMAP is %d\n", N_IMAP);
  bitchunk_t * recovered_map = alloc_bitmap(N_IMAP);
  for (int i = 0; i < N_IMAP; i++){
    int k;
    for (k = 0; k <= chunk_size -1 ; k++) {
      if(i*chunk_size +k < fsize)
        chunk[chunk_size-k-1] = string[i*chunk_size +k];
      else
        chunk[chunk_size-k-1] = 0;
    }
    char * pEnd2;
    unsigned int update = strtol(chunk,&pEnd2,2);
    if(update != 0){
      printf("update with recovery us %u \n", update);
      sleep(1);
    }
    recovered_map[i]=update;
  }
  if(compare_bitmaps_2(recovered_map, inode_map, N_IMAP) != 0){
    dumpbitmap(recovered_map, BLK_IMAP, N_IMAP);
    puts("INODE DAMAGE FIXED");
    sleep(5);
  }else{
    puts("NO INODE DAMAGE TO FIX");
    sleep(5);
  }
  free(recovered_map);
  free(inode_map);
  fclose(file);
  close(file_descriptor);
}

/*===========================================================================*
 *        list_inodes            *
 *===========================================================================*/
int * list_inodes(device)
const char * device;
{
  struct inode * ino;
  int * inode_list = NULL;

  return inode_list;
}

/*===========================================================================*
 *        clean_stdin                                         *
 *===========================================================================*/
int clean_stdin(){
  while (getchar()!='\n');
  return 1;
}

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char *argv[]){
  int operationSelectedNumber;
  char directory_name[100];
  int to_damage = 0;
  char * device = "/dev/c0d0p0s1";
  char c;
  do{
    puts("----------- Welcome -----------");
    printf("My pid is: %d.\n",getpid());
    puts("1. DirectoryWalker");
    puts("2. iNodeBitMapWalker");
    puts("3. ZoneBitMapWalker");
    puts("4. Damage inode");
    puts("5. Damage zone");
    puts("6. Recover inode");
    puts("7. Recover zone");
    puts("8. Exit");
    puts("");
    do{
      puts("Enter your choice:");
    } while (((scanf("%d%c", &operationSelectedNumber, &c)!=2 || c!='\n') && clean_stdin()) || operationSelectedNumber<1 || operationSelectedNumber>9);;
    switch (operationSelectedNumber) {
      case 1:
        puts("Enter directory name: ");
        scanf("%s", directory_name);
        directorywalker(directory_name);
      break;
      case 2:
        inodewalker();
      break;
      case 3:
        zonewalker();
      break;
      case 4:
        puts("Enter the inode to damage:");
        scanf("%c", directory_name);
        to_damage = atoi(directory_name);
        if(to_damage>=0)damage_inode(to_damage,device);
      break;
      case 5:
        puts("Enter the zone to damage:");
        scanf("%c", directory_name);
        to_damage = atoi(directory_name);
        if(to_damage>=0)damage_zode(to_damage,device);
      break;
      case 6:
        recover_inode(device);
      break;
      case 7:
        puts("Enter the zone to recover:");
        scanf("%c", directory_name);
        to_damage = atoi(directory_name);
        if(to_damage>=0)recover_zone(to_damage,device);
      break;
      case 8:
        exit(0);
      default:
        puts("Default Operation selected");
      break;
    }
  }while(1);
  return 0;
}
