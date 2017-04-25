#include "fs.h"
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
    char * dest=(char *)m_in.m1_i1;
    printf("destination, entering virtual file system: %d\n",(int)dest);
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
    char * dest=(char *)m_in.m1_i1;
    printf("destination, entering virtual file system: %d\n",(int)dest);
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
    return 0;
}
