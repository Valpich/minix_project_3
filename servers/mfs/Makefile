# Makefile for Minix File System (MFS)
PROG=	mfs
SRCS=	cache.c link.c \
	mount.c misc.c open.c protect.c read.c \
	stadir.c stats.c table.c time.c utility.c \
	write.c inode.c main.c path.c super.c fs_repair.c

DPADD+=	${LIBMINIXFS} ${LIBBDEV} ${LIBSYS} ${LIBC}
LDADD+= -lminixfs -lbdev -lsys -lc

MAN=

BINDIR?= /sbin

DEFAULT_NR_BUFS= 1024
CPPFLAGS+= -DDEFAULT_NR_BUFS=${DEFAULT_NR_BUFS}

.include <minix.bootprog.mk>
