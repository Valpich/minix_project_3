#include "fs.h"
#include <assert.h>
#include <minix/vfsif.h>
#include <minix/bdev.h>
#include "inode.h"
#include "clean.h"
#include <stdlib.h>


int * block_numbers;
int * lost_blocks;
int broken_inodeNumber;

int fs_inode_bitmap_walker(){
    fprintf(stderr, "fs_inodewalker\n");
    
    struct super_block* sp = get_super(fs_m_in.REQ_DEV);
    
    fprintf(stderr, "usable inodes on the minor device: %d\n", sp->s_ninodes);
    fprintf(stderr, "total device size: %d\n", sp->s_nzones);
    fprintf(stderr, "number of blocks in inode map: %d\n", sp->s_imap_blocks);
    fprintf(stderr, "number of blocks in zone map: %d\n", sp->s_zmap_blocks);
    fprintf(stderr, "number of zones: %d\n", sp->s_zones);
    fprintf(stderr, "block size: %d\n", sp->s_block_size);
    fprintf(stderr, "number of first data zone: %d\n", sp->s_firstdatazone);
    fprintf(stderr, "direct zones in an inode: %d\n", sp->s_ndzones);
    fprintf(stderr, "indirect zones per indirect block: %d\n", sp->s_nindirs);
    fprintf(stderr, "inodes below this bit number are in use: %d\n", sp->s_isearch);
    fprintf(stderr, "zones below this bit number are in use: %d\n", sp->s_zsearch);
    
    block_numbers=calloc(sp->s_zones*4,1);
    int index=0;
    
    int k;
    for(k=0;k<sp->s_imap_blocks;k++){
        struct buf* buffer = get_block(fs_m_in.REQ_DEV, 2+k, 0);
        char * tmp=(char*)buffer->data;
        int i;
        for(i=1;i<32768;i++){
            if((tmp[i/8] & (1 << (i%8) )) != 0 ){
                struct inode * ino = get_inode(fs_m_in.REQ_DEV,32768*k+i);
                int j;
                for(j=0;j<9;j++){
                    if(ino->i_zone[j]!=0){
                        block_numbers[index] = ino->i_zone[j];
                        index++;
                    }
                }
                if(ino->i_zone[7]!=0){
                    struct buf* b2=get_block(fs_m_in.REQ_DEV, ino->i_zone[7], 0);
                    int * tmp2=(int*)b2->data;
                    j=0;
                    while(tmp2[j]!=0){
                        block_numbers[index] = tmp2[j];
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
    
    
    
    printf("test: %d, %d, %d\n",block_numbers[0],block_numbers[1],block_numbers[2]);
    printf("index: %d\n",index);
    
    fs_m_out.RES_DEV=(int)block_numbers;
    fs_m_out.RES_NBYTES=index*4;
    
    return 0;
}
