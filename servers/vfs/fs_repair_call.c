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
#include <stdlib.h>
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

int RC_CODE;

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
    for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
        if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
            message m;
            m.m_type = REQ_RECOVERY;
            m.REQ_DEV = vmp->m_dev;
            RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
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
    printf("inode received is %d.\n",inode);
    int file = open("/dev/c0d0p0s0", O_RDWR | O_NONBLOCK, 0);
    printf("file is %d.\n",file);
    sleep(5);
    for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; ++vmp) {
        if ( strcmp("/home", vmp->m_mount_path) == 0 ) {
            message m;
            m.m_type = REQ_DAMAGE;
            m.REQ_DEV = vmp->m_dev;
            m.m1_i1 = inode;
            m.m1_i2 = operation;
            m.m1_p1 = folder;
            RC_CODE = fs_sendrec(vmp->m_fs_e, &m);
        }
    }
    return 0;
}