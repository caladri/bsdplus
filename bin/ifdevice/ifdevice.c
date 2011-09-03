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

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libifplus.h>

static void usage(void);

int
main(int argc, char *argv[])
{
	const char *ifname;
	char *driver;
	int error;
	bool ok;

	while (getopt(argc, argv, "") != -1)
		usage();
	argc -= optind;
	argv += optind;

	if (argc == 0)	
		usage();

	if (argc == 1) {
		error = ifname2drivername(*argv, &driver);
		if (error != 0)
			errc(1, error, "ifname2drivername");
		printf("%s\n", driver);
		free(driver);
		return (0);
	}

	ok = true;
	while (argc--) {
		ifname = *argv++;

		error = ifname2drivername(ifname, &driver);
		if (error != 0) {
			warnc(error, "ifname2drivername(\"%s\")", ifname);
			ok = false;
			continue;
		}
		printf("%s: %s\n", ifname, driver);
		free(driver);
	}

	if (!ok)
		return (1);

	return (0);
}

static void
usage(void)
{
	fprintf(stderr,
"ifdevice ifname ...\n");
	exit(1);
}
