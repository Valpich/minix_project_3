#include "fs.h"
#include <assert.h>
#include <minix/vfsif.h>
#include <minix/bdev.h>
#include "inode.h"
#include "clean.h"
#include <stdlib.h>

#define BLOCK_SIZE 4096

int * block_ids;
int * lost_blocks;
int broken_inodeNumber;



void print_super_block(struct super_block * sp){
    printf("# usable inodes on the minor device: %d .\n", sp->s_ninodes);
    printf("total device size, including bit maps etc: %d .\n",sp->s_nzones);
    printf("# of blocks used by inode bit map: %h .\n",sp->s_imap_blocks);
    printf("# of blocks used by zone bit map: %h .\n",sp->s_zmap_blocks);
    printf("number of first data zone: %d .\n",sp->s_firstdatazone);
    printf("log2 of blocks/zone: %h .\n",sp->s_log_zone_size);
    printf("maximum file size on this device: %d .\n",sp->s_max_size);
    printf("magic number to recognize super-blocks: %h .\n",sp->s_magic);
    printf("try to avoid compiler-dependent padding: %h .\n",sp->s_pad2);
    printf("number of zones (replaces s_nzones in V2): %d .\n",sp->s_zones);
    printf("The following items are only used when the super_block is in memory.\n");
    printf("precalculated from magic number: %d .\n",sp->s_inodes_per_block);
    printf("whose super block is this? %d.\n",sp->s_dev);
    printf("set to 1 iff file sys mounted read only: %d .\n",sp->s_rd_only);
    printf("set to 1 iff not byte swapped file system: %d .\n",sp->s_native);
    printf("file system version, zero means bad magic: %d .\n",sp->s_version);
    printf("# direct zones in an inode: %d .\n",sp->s_ndzones);
    printf("# indirect zones per indirect block: %d .\n",sp->s_nindirs);
    printf("inodes below this bit number are in use: %d .\n",sp->s_isearch);
    printf("all zones below this bit number are in use: %d .\n",sp->s_zsearch);
}

int fs_inode_bitmap_walker(){
    puts("fs_inode_bitmap_walker");
    struct super_block* sp = get_super(fs_m_in.REQ_DEV);
    print_super_block(sp);
    block_ids=calloc(sp->s_zones*4,1);
    int index=0;
    int block_id;
    for(block_id=0; block_id<sp->s_imap_blocks;block_id++){
        struct buf* buffer = get_block(fs_m_in.REQ_DEV, 2+block_id, 0);
        char * tmp=(char*)buffer->data;
        int i;
        for(i=1;i<8*BLOCK_SIZE;i++){
            if((tmp[i/8] & (1 << (i%8) )) != 0 ){
                struct inode * ino = get_inode(fs_m_in.REQ_DEV,8*BLOCK_SIZE*block_id+i);
                int j;
                for(j=0;j<9;j++){
                    if(ino->i_zone[j]!=0){
                        block_ids[index] = ino->i_zone[j];
                        index++;
                    }
                }
                printf("i is: %d\n", i);
                if(ino->i_zone[7]!=0){
                    struct buf* b2=get_block(fs_m_in.REQ_DEV, ino->i_zone[7], 0);
                    int * tmp2=(int*)b2->data;
                    j=0;
                    while(tmp2[j]!=0){
                        block_ids[index] = tmp2[j];
                        index++;
                        j++;
                    }
                    put_block(b2,0);
                    put_inode(ino);
                }

            }
        }
        put_block(buffer,0);
    }
    fs_m_out.RES_DEV=(int)block_ids;
    fs_m_out.RES_NBYTES=index*4;
    return 0;
}
