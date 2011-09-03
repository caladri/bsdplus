#

.for _lib in ${USE_LIBS}
CFLAGS+=-I${TOPDIR}/lib/lib${_lib}
LDFLAGS+=-L${TOPDIR}/lib/lib${_lib}
LDADD+=	-l${_lib}
DPADD+=${TOPDIR}/lib/lib${_lib}/lib${_lib}.a
.endfor

.include "${.CURDIR}/../../Makefile.inc"
