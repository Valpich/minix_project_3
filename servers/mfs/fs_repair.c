#include "fs.h"
#include <assert.h>
#include <minix/vfsif.h>
#include <minix/bdev.h>
#include "inode.h"
#include "clean.h"
#include <stdlib.h>

#define BLOCK_SIZE 4096

int * block_ids;
int * lost_blocks_ids;
int damaged_inode_number;

void print_inode(struct inode * ino){
    printf("file type, protection, etc: %d .\n", ino->i_mode);
    printf("how many links to this file: %d .\n",ino->i_nlinks);
    printf("user id of the file's owner: %d .\n",ino->i_uid);
    printf("group number: %d .\n",ino->i_gid);
    printf("current file size in bytes: %d .\n",ino->i_size);
    printf("time of last access (V2 only): %d .\n",ino->i_atime);
    printf("when was file data last changed: %d .\n",ino->i_mtime);
    printf("when was inode itself changed (V2 only): %d .\n",ino->i_ctime);
    int i = 0;
    for(i = 0; i<V2_NR_TZONES ;i++){
        printf("zone numbers for direct, ind, and dbl ind: %d .\n",ino->i_zone[i]);
    }
    printf("The following items are not present on the disk.\n");
    printf("which device is the inode on: %d .\n",ino->i_dev);
    printf("inode number on its (minor) device: %d.\n",ino->i_num);
    printf("# times inode used; 0 means slot is free: %d .\n",ino->i_count);
    printf("# direct zones (Vx_NR_DZONES) : %d .\n",ino->i_ndzones);
    printf("# indirect zones per indirect block: %d .\n",ino->i_nindirs);
    printf("CLEAN or DIRTY: %d .\n",ino->i_dirt);
    printf("set on LSEEK, cleared on READ/WRITE: %d .\n",ino->i_seek);
    printf("the ATIME, CTIME, and MTIME bits are here: %d .\n",ino->i_update);
}

void print_super_block(struct super_block * sp){
    printf("# usable inodes on the minor device: %d .\n", sp->s_ninodes);
    printf("total device size, including bit maps etc: %d .\n",sp->s_nzones);
    printf("# of blocks used by inode bit map: %d .\n",sp->s_imap_blocks);
    printf("# of blocks used by zone bit map: %d .\n",sp->s_zmap_blocks);
    printf("number of first data zone: %d .\n",sp->s_firstdatazone);
    printf("log2 of blocks/zone: %d .\n",sp->s_log_zone_size);
    printf("maximum file size on this device: %d .\n",sp->s_max_size);
    printf("magic number to recognize super-blocks: %d .\n",sp->s_magic);
    printf("try to avoid compiler-dependent padding: %d .\n",sp->s_pad2);
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
    int i=0;
    int j=0;
    for(block_id=0; block_id<sp->s_imap_blocks;block_id++){
        struct buf* block_buffer = get_block(fs_m_in.REQ_DEV, 2+block_id, 0);
        char * address=(char*)block_buffer->data;
        for(i=1;i<8*BLOCK_SIZE;i++){
            if((address[i/8] & (1 << (i%8) )) != 0 ){
                struct inode * found_inode = get_inode(fs_m_in.REQ_DEV,8*BLOCK_SIZE*block_id+i);
                print_inode(found_inode);
                for(j=0;j<=8;j++){
                    if(found_inode->i_zone[j]!=0){
                        block_ids[index] = found_inode->i_zone[j];
                        index++;
                    }
                }
                if(found_inode->i_zone[7]!=0){
                    struct buf* block_buffer_2=get_block(fs_m_in.REQ_DEV, found_inode->i_zone[7], 0);
                    int * address_2=(int*)block_buffer_2->data;
                    j=0;
                    while(address_2[j]!=0){
                        block_ids[index] = address_2[j];
                        index++;
                        j++;
                    }
                    put_block(block_buffer_2,0);
                    put_inode(found_inode);
                }
            }
        }
        put_block(block_buffer,0);
    }
    fs_m_out.RES_DEV=(int)block_ids;
    fs_m_out.RES_NBYTES=index*4;
    return 0;
}

int fs_zone_bitmap_walker(){
    puts("fs_zone_bitmap_walker");
    return 0;
}

int fs_directory_bitmap_walker(){
    puts("fs_directory_bitmap_walker");
    return 0;
}
