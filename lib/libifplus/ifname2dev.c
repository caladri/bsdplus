/*
 * Copyright (c) 2002-2009 Juli Mallett. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */ 

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_mib.h>
#include <string.h>

#include "libifplus.h"

int
ifname2drivername(const char *ifname, char **drivernamep)
{
	char drivername[IFNAMSIZ]; /* XXX Can device names be longer?  */
	unsigned ifindex;
	size_t len;
	int mib[6];
	char *p;
	int rv;

	ifindex = if_nametoindex(ifname);
	if (ifindex == 0) {
		if (errno == 0)
			return (ENODEV);
		return (errno);
	}

	mib[0] = CTL_NET;
	mib[1] = PF_LINK;
	mib[2] = NETLINK_GENERIC;
	mib[3] = IFMIB_IFDATA;
	mib[4] = ifindex;
	mib[5] = IFDATA_DRIVERNAME;

	len = sizeof drivername;
	rv = sysctl(mib, 6, drivername, &len, NULL, 0);
	if (rv == -1) {
		if (errno == 0)
			return (ENODEV);
		return (errno);
	}

	if (len >= sizeof drivername)
		return (E2BIG);

	p = strdup(drivername);
	if (p == NULL)
		return (ENOMEM);

	*drivernamep = p;

	return (0);
}
