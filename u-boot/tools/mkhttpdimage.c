/*
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WIN32__
#include <netinet/in.h>		/* for host / network byte order conversions	*/
#endif
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#if defined(__BEOS__) || defined(__NetBSD__) || defined(__APPLE__)
#include <inttypes.h>
#endif

#ifdef __WIN32__
typedef unsigned int __u32;

#define SWAP_LONG(x) \
	((__u32)( \
		(((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
		(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
		(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
		(((__u32)(x) & (__u32)0xff000000UL) >> 24) ))
typedef		unsigned char	uint8_t;
typedef		unsigned short	uint16_t;
typedef		unsigned int	uint32_t;

#define     ntohl(a)	SWAP_LONG(a)
#define     htonl(a)	SWAP_LONG(a)
#endif	/* __WIN32__ */

#ifndef	O_BINARY		/* should be define'd on __WIN32__ */
#define O_BINARY	0
#endif

#include <image.h>

extern int errno;

#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif

char *cmdname;

extern unsigned long crc32 (unsigned long crc, const char *buf, unsigned int len);

static	void	usage	(void);
static	void	print_header (image_trailer_t *);

char	*datafile;
char	*imagefile;

int dflag    = 0;

image_trailer_t trailer;
image_trailer_t *tlr = &trailer;

int
main (int argc, char **argv)
{
	int ifd;
	int dfd;
	uint32_t checksum;
	uint32_t addr;
	uint32_t ep;
	struct stat sbuf;
	unsigned char *ptr;
	char *name = "";

	cmdname = *argv;

	while (--argc > 0 && **++argv == '-') {
		while (*++*argv) {
			switch (**argv) {
			case 'd':
				if (--argc <= 0)
					usage ();
				datafile = *++argv;
				dflag = 1;
				goto NXTARG;
			default:
				usage ();
			}
		}
NXTARG:		;
	}

	if ((argc != 1) || (dflag == 0))
		usage();

	imagefile = *argv;

	if ((dfd = open(datafile, O_RDONLY|O_BINARY)) < 0) {
		fprintf (stderr, "%s: Can't open %s: %s\n",
			cmdname, datafile, strerror(errno));
		exit (EXIT_FAILURE);
	}

	if (fstat(dfd, &sbuf) < 0) {
		fprintf (stderr, "%s: Can't stat %s: %s\n",
			cmdname, datafile, strerror(errno));
		exit (EXIT_FAILURE);
	}

	ptr = (unsigned char *)mmap(0, sbuf.st_size,
				    PROT_READ, MAP_SHARED, dfd, 0);
	if (ptr == (unsigned char *)MAP_FAILED) {
		fprintf (stderr, "%s: Can't read %s: %s\n",
			cmdname, datafile, strerror(errno));
		exit (EXIT_FAILURE);
	}

	checksum = crc32 (0,
			  (const char *)(ptr),
			  sbuf.st_size
			 );

	(void) munmap((void *)ptr, sbuf.st_size);
	(void) close (dfd);

	memset (tlr, 0, sizeof(image_trailer_t));

	/* Build new header */
	tlr->it_magic = htonl(IT_MAGIC);
	tlr->it_size  = htonl(sbuf.st_size);
	tlr->it_dcrc  = htonl(checksum);

	print_header (tlr);

	ifd = open(imagefile, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0666);

	if (ifd < 0) {
		fprintf (stderr, "%s: Can't open %s: %s\n",
			cmdname, imagefile, strerror(errno));
		exit (EXIT_FAILURE);
	}

	if (write(ifd, tlr, sizeof(image_trailer_t)) != sizeof(image_trailer_t)) {
		fprintf (stderr, "%s: Write error on %s: %s\n",
			cmdname, imagefile, strerror(errno));
		exit (EXIT_FAILURE);
	}
	/* We're a bit of paranoid */
#if defined(_POSIX_SYNCHRONIZED_IO) && !defined(__sun__) && !defined(__FreeBSD__)
	(void) fdatasync (ifd);
#else
	(void) fsync (ifd);
#endif

	if (close(ifd)) {
		fprintf (stderr, "%s: Write error on %s: %s\n",
			cmdname, imagefile, strerror(errno));
		exit (EXIT_FAILURE);
	}

	exit (EXIT_SUCCESS);
}

void
usage ()
{
	fprintf (stderr, "Usage: %s  -d data_file image_tlr\n",
		cmdname);
	exit (EXIT_FAILURE);
}

static void
print_header (image_trailer_t *tlr)
{
	uint32_t size;

	size = ntohl(tlr->it_size);

	printf ("Data Size:    %d Bytes = %.2f kB = %.2f MB\n",
		size, (double)size / 1.024e3, (double)size / 1.048576e6 );
	printf ("Data Crc: 0x%08X\n", ntohl(tlr->it_dcrc));
	printf ("Magic:  0x%08X\n", ntohl(tlr->it_magic));

}

