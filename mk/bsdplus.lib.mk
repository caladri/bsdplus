.include <bsd.lib.mk>

OSNAME!=uname
.if ${OSNAME} == "Darwin"
CFLAGS:=${CFLAGS:N-Wsystem-headers}
.endif
