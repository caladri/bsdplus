#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	SALTSIZE	32

static char const *saltchars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ./";

static void usage(void);

int
main(int argc, char *argv[])
{
	char saltbuf[SALTSIZE + 1];
	const char *pw, *salt;
	unsigned i;
	int ch;

	salt = NULL;

	while ((ch = getopt(argc, argv, "S:f:")) != -1) {
		switch (ch) {
		case 'S':
			salt = optarg;
			break;
		case 'f':
			if (!crypt_set_format(optarg))
				err(1, "crypt_set_format");
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1)
		usage();

	if (salt == NULL) {
		for (i = 0; i < SALTSIZE; i++)
			saltbuf[i] = saltchars[arc4random_uniform((uint32_t)strlen(saltchars))];
		saltbuf[i] = '\0';
		salt = saltbuf;
	}

	pw = crypt(argv[0], salt);
	if (pw == NULL)
		err(1, "crypt");

	printf("%s\n", pw);

	return (0);
}

static void
usage(void)
{
	fprintf(stderr,
"usage: genpw [-S salt] [-f format] password\n");
	exit(1);
}
