# Makefile for main
PROG=	 main
SRCS=	main.c

DPADD+=	${LIBMINIXFS} ${LIBBDEV} ${LIBSYS} ${LIBC}
LDADD+= -lminixfs -lbdev -lsys -lc

MAN=

BINDIR?= /sbin

DEFAULT_NR_BUFS= 1024
CPPFLAGS+= -DDEFAULT_NR_BUFS=${DEFAULT_NR_BUFS}

.include <minix.bootprog.mk>
