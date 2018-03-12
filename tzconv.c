/*
 * Copyright (c) 2018 Abel Abraham Camarillo Ojeda <acamari@verlet.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

void procfile(FILE *);

void
pstructtm(struct tm *tm)
{
	fprintf(stderr, "int tm_sec: %d\n", tm->tm_sec);
	fprintf(stderr, "int tm_min: %d\n", tm->tm_min);
	fprintf(stderr, "int tm_hour: %d\n", tm->tm_hour);
	fprintf(stderr, "int tm_mday: %d\n", tm->tm_mday);
	fprintf(stderr, "int tm_mon: %d\n", tm->tm_mon);
	fprintf(stderr, "int tm_year: %d\n", tm->tm_year);
	fprintf(stderr, "int tm_wday: %d\n", tm->tm_wday);
	fprintf(stderr, "int tm_yday: %d\n", tm->tm_yday);
	fprintf(stderr, "int tm_isdst: %d\n", tm->tm_isdst);
	fprintf(stderr, "long tm_gmtoff: %ld\n", tm->tm_gmtoff);
	fprintf(stderr, "char *tm_zone: %s\n", tm->tm_zone);
}

void
ptzname()
{
	printf("tzname[0]: <%s>\n", tzname[0]);
	printf("tzname[1]: <%s>\n", tzname[1]);
}

/*
 * receives as argument tz a new time zone to set in the current environment,
 * returns previous value of tz.
 * if tz is a null pointer, unsets TZ from the environment;
 */
char *
settz(char *tz)
{
	char *r;

	r = getenv("TZ");
	if (tz == NULL) {
		unsetenv("TZ");
	} else {
		setenv("TZ", tz, 1);
	}
	return r;
}

enum { ENONE = 0, EINFMT, EOUTFMT } tztotzerr;
/*
 * receives as orig argument a NUL-terminated timestamp string containing at
 * its beginning an string matching a strptime(3)'d format from origfmt
 * localized at the timezone stored in origtz and converts it to another
 * timestamp in destination timezone dsttz using the strftime(3) format at
 * dstfmt, converted string is saved on dst argument guaranteeing
 * NUL-termination of it and that no more of sz chars are stored (including
 * terminating NUL). If origtz argument is NULL current timezone is used. If
 * dsttz argument is NULL current timezone is used.
 *
 * Returns >0 in case of error, otherwise returns 0 and in next the address of the
 * next character from orig that would be read (according to strptime(3)
 * semantics) and saves in dst the string in the new format
 */
int
tztotz(char *dst, size_t sz, char **next, char *orig, char *dstfmt, char *origfmt, char *dsttz, char *origtz)
{
	char		*savedtz = NULL;
	struct tm	tm;
	time_t		t;
	int		err;

	err = 0;
	memset(&tm, 0, sizeof tm);
	savedtz = getenv("TZ");
	if ((*next = strptime(orig, origfmt, &tm)) == NULL) {
		err = EINFMT; /* string not found or error */
		goto ret;
	}
	if (origtz)
		settz(origtz);
	t = mktime(&tm);
	settz(dsttz ? dsttz : savedtz);
	if (strftime(dst, sz, dstfmt, localtime(&t)) == 0) {
		err = EOUTFMT;
		goto ret;
	}
ret:
	settz(savedtz);
	return(err ? err : 0);
}

void
usage()
{
	fprintf(stderr, "usage: tzconv [-f origtz] [-t dsttz] [-F origfmt] [-T dstfmt] [file ...]");
	exit(1);
}

/* origin tzzone: null means local time zone */
static char	*fflag = NULL;
/* dest tzzone: null means local time zone */
static char	*tflag = NULL;
/* origin timestamp format */
static char	*Fflag = "%Y-%m-%d %H:%M:%S %Z";
/* dest timestamp format */
static char 	*Tflag = "%Y-%m-%d %H:%M:%S %Z";
static int	exstatus = 0;

int
main(int argc, char *argv[])
{
	char	ch;

	while ((ch = getopt(argc, argv, "F:f:T:t:")) != -1) {
		switch (ch) {
		case 'F':
			Fflag = optarg;
			break;
		case 'T':
			Tflag = optarg;
			break;
		case 'f':
			fflag = optarg;
			break;
		case 't':
			tflag = optarg;
			break;
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc <= 0) { /* no args */
		procfile(stdin);
		return exstatus;
	}

	for (; *argv; argv++) {
		FILE *fp;
		if ((fp = fopen(*argv, "r")) == NULL)
			err(1, "fopen");
		procfile(fp);
		if (fclose(fp) == EOF)
			err(1, "fclose");
	}
	return exstatus;
}

void
procfile(FILE *fp)
{
	char *line = NULL;
	size_t linesz = 0;
	ssize_t linelen;

	while ((linelen = getline(&line, &linesz, fp)) != -1) {
		char	buf[1024];
		char	*suf; /* pointer to suffix text on string */

		if (tztotz(buf, sizeof buf, &suf, line,
		    Tflag, Fflag, tflag, fflag) == 0) {
			if (printf("%s%s", buf, suf) < 0)
				err(1, "printf");
		} else { /* theres any error, print line verbatim */
			exstatus = 1;
			if (printf("%s", line) < 0)
				err(1, "printf");
		}
	}

	free(line);
	if (ferror(fp))
		err(1, "getline");
}
