#include <lib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>

/*===========================================================================*
 *              devio          *
 *===========================================================================*/
void devio(bno, dir)
block_t bno;
int dir;
{
  int r;

  if(!BLOCK_SIZE) fatal("devio() with unknown block size");
  if (dir == READING && bno == thisblk) return;
  thisblk = bno;

#if 1
printf("%s at block %5d\n", dir == READING ? "reading " : "writing", bno);
#endif
printf("dev is %d SEEK_SET is  %d.\n", dev, SEEK_SET);
printf("file_descriptor is %d.\n",file_descriptor);
    if(file_descriptor != -1 ){
        printf("file_descriptor open is %d\n", file_descriptor);
      r= lseek64(file_descriptor, btoa64(bno), SEEK_SET, NULL);
      if (r != 0)
        fatal("lseek64 failed");
      if (dir == READING) {
        if (read(dev, rwbuf, BLOCK_SIZE) == BLOCK_SIZE)
            return;
      } else {
        if (write(dev, rwbuf, BLOCK_SIZE) == BLOCK_SIZE){
            return;
        }
      }

      printf("%s: can't %s block %ld (error = 0x%x)\n", "fs_repair", dir == READING ? "read" : "write", (long) bno, errno);
      if (dir == READING) {
        printf("Continuing with a zero-filled block.\n");
        memset(rwbuf, 0, BLOCK_SIZE);
        return;
      }
      fatal("");
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
  if(!BLOCK_SIZE) fatal("devwrite() with unknown block size");
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

  for (i = 0; i < nblk; i++, bno++, p += WORDS_PER_BLOCK){
    devwrite(bno, 0, (char *) p, BLOCK_SIZE);
  }
}


int inodewalker(){
    message m;
    int x=_syscall(VFS_PROC_NR,108,&m);
    return x;
}

int zonewalker(){
    message m;
    int x=_syscall(VFS_PROC_NR,109,&m);
    return x;
}

int directorywalker(char * path){
	message m;
    m.m1_p1=path;
    int x=_syscall(VFS_PROC_NR,110,&m);
    return x;
}

int recovery(){
    message m;
    int x=_syscall(VFS_PROC_NR,103,&m);
    return x;
}

int damage(int inode, int operation, char * folder){
    printf("Calling damage\n");
    int max = 1024*1024*32;
    int * output = calloc(max,sizeof(int));
    int j = 0;
    for(j = 0; j<max ; j++){
        output[j] = -1;
    }
    message m;
    m.m1_i1 = inode;
    m.m1_i2 = operation;
    m.m1_i3 = output;
    m.m1_p1 = folder;
    printf("Calling VFS\n");
    int x=_syscall(VFS_PROC_NR,105,&m);
    printf("test output %d  %d  %d %d\n",output[0],output[1],output[2],output[3]);
    FILE * file = fopen("map.txt","w");
    int i=0;
    while(output[i]!=-1){
        i++;
    }
    printf("map size is: %d\n.",i);
    i=0;
    while(output[i]!=-1){
        fprintf(file,"%d",output[i]);
        i++;
    }
    fclose(file);
    free(output);
    return x;
}