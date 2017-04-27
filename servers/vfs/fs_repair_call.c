#include "fs.h"

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
#include "../mfs/const.h"
#include "../mfs/inode.h"
#include "../mfs/type.h"
#include "../mfs/mfsdir.h"
#include <minix/fslib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <a.out.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <minix/vfsif.h>
#include <minix/const.h>
#include <minix/endpoint.h>
#include <stdio.h>
#include <minix/callnr.h>
#include <signal.h>
#include <sys/svrctl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/sysinfo.h>
#include <minix/type.h>
#include <minix/vm.h>
#include <string.h>
#include <machine/archtypes.h>
#include <lib.h>
#include <assert.h>
#include <string.h>

#include "fproc.h"
#include "vmnt.h"
#include "vnode.h"
#include "path.h"
#include "param.h"

#include <repair.h>
#define BLOCK_SIZE 4096

int RC_CODE;
unsigned int chunk_size = sizeof(unsigned int) * CHAR_BIT;

 /* reverse:  reverse string s in place */
void reverse(char s[])
{
   int i, j;
   char c;
   
   for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
       c = s[i];
       s[i] = s[j];
       s[j] = c;
   }
}

/* itoa:  convert n to characters in s */
void my_itoa(int n, char s[])
{
    int i, sign;
    
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
    i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
       s[i++] = '-';
   s[i] = '\0';
   reverse(s);
}

int do_inode_bitmap_walker(){
    puts("Call of do_inode_bitmap_walker");
    struct vmnt *vmp;
    for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
        if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
            message m;
            m.m_type = REQ_INODEWALKER;
            m.REQ_DEV = vmp->m_dev;
            RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
        }
    }
    return 0;
}

int do_zone_bitmap_walker(){
    puts("Call of do_zone_bitmap_walker");
    struct vmnt *vmp;
    for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
        if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
            message m;
            m.m_type = REQ_ZONEWALKER;
            m.REQ_DEV = vmp->m_dev;
            RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
        }
    }
    return 0;
}

int do_directory_bitmap_walker(){
    puts("Call of do_directory_bitmap_walker");
    struct vmnt *vmp;
    for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
        if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
            message m;
            m.m_type = REQ_DIRECTORYWALKER;
            m.REQ_DEV = vmp->m_dev;
            RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
        }
    }
    return 0;
}

int do_recovery(){
    puts("Call of do_recovery");
    struct vmnt *vmp;
    int * output_inode = (int *) m_in.m1_i1;
    int * output_zone = (int *) m_in.m1_i2;
    int * output_inode_list = (int *) m_in.m1_i3;
    char * size_inode = m_in.m1_p1;
    char * size_zone = m_in.m1_p2;
    char * size_inode_list = m_in.m1_p3;
    char str[10];
    char str2[10];
    char str3[10];
    endpoint_t endpoint = m_in.m_source;
    for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
        if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
            message m;
            m.m_type = REQ_RECOVERY;
            m.REQ_DEV = vmp->m_dev;
            RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
            int * bit_inode_src = (int *) m.RES_DEV;
            int N_IMAP = m.RES_NBYTES;
            int * bit_zone_src = (int *) m.RES_FILE_SIZE_HI;
            int N_ZMAP = m.RES_FILE_SIZE_LO;
            int size_imap = N_IMAP*chunk_size*BLOCK_SIZE/sizeof(bitchunk_t);
            int * bitmap_imap = calloc(size_imap,1);
            int size_zmap = N_ZMAP*chunk_size*BLOCK_SIZE/sizeof(bitchunk_t);
            int * bitmap_zmap = calloc(size_zmap,1);
            my_itoa(N_IMAP,str);
            my_itoa(N_ZMAP,str2);
            if(sys_datacopy(m.m_source, (vir_bytes) bit_inode_src, SELF, (vir_bytes) bitmap_imap, size_imap)==OK){
                printf("test copy source/bitmap copy %d  %d  %d %d\n",bitmap_imap[0],bitmap_imap[1],bitmap_imap[2],bitmap_imap[3]);
                printf("Copy source/bitmap ok.\n");
            } 
            if(sys_datacopy(SELF, (vir_bytes)bitmap_imap, endpoint , (vir_bytes)output_inode, size_imap)==OK){
                printf("Copy bitmap_imap/output_inode ok.\n");
            }
            if(sys_datacopy(SELF, (vir_bytes)str, endpoint , (vir_bytes)size_inode, 10)==OK){
                printf("test copy size/size_inode copy %s\n",size_inode);
                printf("Copy size_inode/size_inode ok.\n");
            }
            if(sys_datacopy(m.m_source, (vir_bytes) bit_zone_src, SELF, (vir_bytes) bitmap_zmap, size_zmap)==OK){
                printf("test copy source/bitmap_zmap copy %d  %d  %d %d\n",bitmap_zmap[0],bitmap_zmap[1],bitmap_zmap[2],bitmap_zmap[3]);
                printf("Copy source/bitmap_zmap ok.\n");
            } 
            if(sys_datacopy(SELF, (vir_bytes)bitmap_zmap, endpoint , (vir_bytes)output_zone, size_zmap)==OK){
                printf("Copy bitmap_zmap/output_zone ok.\n");
            }
            if(sys_datacopy(SELF, (vir_bytes)str2, endpoint , (vir_bytes)size_zone, 10)==OK){
                printf("test copy size/size_zone copy %s\n",size_zone);
                printf("Copy size/size_zone ok.\n");
            }
            int * inode_list_src = (int *) m.RES_INODE_NR;
            int SIZE_ILIST = m.RES_GID;
            my_itoa(SIZE_ILIST,str3);
            int * temp_list = calloc(SIZE_ILIST,1);
            if(sys_datacopy(m.m_source, (vir_bytes) inode_list_src, SELF, (vir_bytes) temp_list, SIZE_ILIST)==OK){
                printf("test copy source/temp_list copy %d  %d  %d %d\n",temp_list[0],temp_list[1],temp_list[2],temp_list[3]);
                printf("Copy source/temp_list ok.\n");
            } 
            if(sys_datacopy(SELF, (vir_bytes)temp_list, endpoint , (vir_bytes)output_inode_list, SIZE_ILIST)==OK){
                printf("Copy temp_list/output_inode_list ok.\n");
            }
            if(sys_datacopy(SELF, (vir_bytes)str3, endpoint , (vir_bytes)size_inode_list, 10)==OK){
                printf("test copy size/size_inode_list copy %s\n",size_inode_list);
                printf("Copy size_inode_list/size_inode_list ok.\n");
            }
        }
    }
    return 0;
}

int do_damage(){
    puts("Call of do_damage");
    struct vmnt *vmp;
    int inode = m_in.m1_i1;
    int operation = m_in.m1_i2;
    char * folder = m_in.m1_p1;
    char * size_pointer = m_in.m1_p2;
    int * output = (int *) m_in.m1_i3;
    endpoint_t endpoint = m_in.m_source;
    printf("inode received is %d.\n",inode);
    char str[10];
    for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
        if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
            message m;
            m.m_type = REQ_DAMAGE;
            m.REQ_DEV = vmp->m_dev;
            m.m1_i1 = inode;
            m.m1_i2 = operation;
            m.m1_p1 = folder;
            RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
            int * src = (int *) m.RES_DEV;
            int N_MAP = m.RES_NBYTES;
            my_itoa(N_MAP,str);
            int size = N_MAP*chunk_size*BLOCK_SIZE/sizeof(bitchunk_t);
            int * bitmap = calloc(size,1);
            printf("size is  %d .\n",size);
            printf("src is  %p .\n",src);
            if(sys_datacopy(m.m_source, (vir_bytes) src, SELF, (vir_bytes) bitmap, size)==OK){
                printf("test copy source/bitmap copy %d  %d  %d %d\n",bitmap[0],bitmap[1],bitmap[2],bitmap[3]);
                printf("Copy source/bitmap ok.\n");
            } 
            if(sys_datacopy(SELF, (vir_bytes)bitmap, endpoint , (vir_bytes)output, size)==OK){
                printf("Copy bitmap/output ok.\n");
            }
            if(sys_datacopy(SELF, (vir_bytes)str, endpoint , (vir_bytes)size_pointer, 10)==OK){
                printf("test copy size/size_pointer copy %s\n",size_pointer);
                printf("Copy size/size_pointer ok.\n");
            }
        }
    }
    return 0;
}