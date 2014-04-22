/*
 *
 * 
 * Paul Stephen Borile
 *
 *
 * d i s k s t o n e s . c 	: Disk performance evaluator
 *
 */

static	char	*Version = "1.1";

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#ifndef	O_LARGEFILE
#define	O_LARGEFILE	0x00
#endif
#define	B1k	(1024*128)

#define	MAX_DIR_OPS	100000

int	offset;
char	block1k[B1k];
char	*block;
char	str[20];
int	blksize;
int	fd, fdsync;
struct	stat	buf;

int	start, stop;
int	rnum;
int	bytes;
int	seekerrs = 0, readerrs = 0, writerrs = 0;

int	size;
int	randsize;
int	syncsize;
int	lines = 0;
int	totlines = 0;
int	nfiles = 0;
int	i,j;
int	rc;
double	seektime;
double	servtime;

main(argc, argv)
int	argc;
char	**argv;
{

	if ( argc != 3 ) usage(argv[0]);
	size	= atoi(argv[2]);
	syncsize = size/10;
	randsize= size/10;

	block	= block1k;
	blksize = B1k;
	bytes	= size * blksize;

	memset(block, 'P', blksize);
	for (i=79; (blksize-i) > 80; i+=80) {
		block[i] = '\n';
		lines++;
	}
	block[i]	= '\n';
	block[blksize-1]= '\n';
	lines		+=2;
	totlines	= lines * size;

	printf("Phase 0 ** fstones %s - (C) 1989 Paul Stephen Borile\n", Version);
	printf("Phase 1 ** Sequential file creation \n");
	printf("	   Creating a %d blocks file (%d byte blocks)\n", size
					, blksize);

	time(&start);

	if ((fd = open(argv[1], O_LARGEFILE|O_CREAT|O_RDWR, 0644)) == -1 ) {
		perror(argv[1]);
		exit(0);
	}

	for (i=0; i<size; i++) {
		rc = write(fd, block, blksize);
		if (rc != blksize) {
			printf("write error on block %d\n", i);
			writerrs++;
		}
	}

	close(fd);

	time(&stop);
	if ((stop - start) == 0 ) {
		printf("Test too short, aborting\n");
		goto phase2;
	}
	printf("	   Elapsed time %d - %dM/sec\n", (stop-start), 
						 ((bytes/(stop-start))/1024/1024));
	servtime = (double)((double)(stop-start)/(double)size);
	printf("	   Average write service time %.9f ms.\n",
						 servtime*1000);

/************************************ End of phase 1 ************************/
phase2:

	printf("Phase 2 ** Sequential file reading \n");

	time(&start);

	if ((fd = open(argv[1], O_LARGEFILE|O_RDONLY)) == -1 ) {
		perror(argv[1]);
		exit(0);
	}

	for (i=0; i<size; i++) {
		rc = read(fd, block, blksize);
		if (rc != blksize) {
			printf("read error on block %d\n", i);
			readerrs++;
		}
	}

	close(fd);

	time(&stop);
	if ((stop - start) == 0 ) {
		printf("Test too short, aborting\n");
		goto phase3;
	}
	printf("	   Elapsed time %d - %dM/sec\n", (stop-start), 
						 ((bytes/(stop-start))/1024/1024));
	servtime = (double)((double)(stop-start)/(double)size);
	printf("	   Average read service time %.9f ms.\n",
						 servtime*1000);

/************************************ End of phase 2 ************************/
phase3:

	printf("Phase 3 ** Random file read \n");

	time(&start);
	srand(start);

	if ((fd = open(argv[1], O_LARGEFILE|O_RDONLY)) == -1 ) {
		perror(argv[1]);
		exit(0);
	}

	for (i=0; i<randsize; i++) {
		rnum = (rand() % size);
		rc = lseek(fd, rnum*blksize, 0);
		if ( rc != rnum*blksize ) {
			printf("Seek error on block %d\n", rnum);
			seekerrs++;
		}
		rc = read(fd, block, blksize);
		if (rc != blksize) {
			printf("read error on block %d\n", i);
			readerrs++;
		}
	}

	close(fd);

	time(&stop);
	if ((stop - start) == 0 ) {
		printf("Test too short, aborting\n");
		goto phase4;
	}
	printf("	   Elapsed time %d - %dM/sec\n", (stop-start), 
						 ((bytes/(stop-start))/1024/1024));
	seektime = (double)((double)(stop-start)/(double)randsize);
	printf("	   Average seek-read time %.9f ms.\n",
						 seektime * 1000);

/************************************ End of phase 3 ************************/
phase4:

	printf("Phase 4 ** Random file write \n");

	time(&start);
	srand(start);

	if ((fd = open(argv[1], O_LARGEFILE|O_WRONLY)) == -1 ) {
		perror(argv[1]);
		exit(0);
	}

	for (i=0; i<randsize; i++) {
		rnum = (rand() % size);
		rc = lseek(fd, rnum*blksize, 0);
		if ( rc != rnum*blksize ) {
			printf("Seek error on block %d\n", rnum);
			seekerrs++;
		}
		rc = write(fd, block, blksize);
		if (rc != blksize) {
			printf("write error on block %d\n", i);
			writerrs++;
		}
	}

	close(fd);

	time(&stop);
	if ((stop - start) == 0 ) {
		printf("Test too short, aborting\n");
		goto phase5;
	}
	printf("	   Elapsed time %d - %dM/sec\n", (stop-start), 
						 ((bytes/(stop-start))/1024/1024));
	seektime = (double)((double)(stop-start)/(double)randsize);
	printf("	   Average seek-write time %.9f ms.\n",
						 seektime * 1000);

/************************************ End of phase 4 ************************/
phase5:

	printf("Phase 5 ** Directory test \n");

	time(&start);
	stat(argv[1], &buf);
	for (i=0; i<MAX_DIR_OPS; i++) {
		sprintf(str,"%s-%d", argv[1], i);
		link(argv[1], str);
		stat(str, &buf);
	}
	for (i=0; i<MAX_DIR_OPS; i++) {
		sprintf(str,"%s-%d", argv[1], i);
		stat(str, &buf);
		unlink(str);
	}
	time(&stop);

	if ((stop - start) == 0 ) {
		printf("Test too short, aborting\n");
		goto phase55;
	}
	
	printf("	   Elapsed time %d - index %d/sec\n", (stop-start), 
						 MAX_DIR_OPS/(stop-start) );

/************************************ End of phase 5 ************************/

phase55:
	printf("Phase 6 ** Synchronous file creation \n");
	printf("	   Creating a %d blocks file (%d byte blocks)\n", size
					, blksize);

	time(&start);

	if ((fd = open(argv[1], O_LARGEFILE|O_CREAT|O_RDWR|O_SYNC, 0644)) == -1 ) {
		perror(argv[1]);
		exit(0);
	}

	for (i=0; i<syncsize; i++) {
		rc = write(fd, block, blksize);
		if (rc != blksize) {
			printf("write error on block %d\n", i);
			writerrs++;
		}
	}

	close(fd);

	time(&stop);
	if ((stop - start) == 0 ) {
		printf("Test too short, aborting\n");
		goto phase6;
	}
	printf("	   Elapsed time %d - %dM/sec\n", (stop-start), 
						 ((bytes/(stop-start))/1024/1024));
	servtime = (double)((double)(stop-start)/(double)size);
	printf("	   Average write service time %.9f ms.\n",
						 servtime*1000);

/************************************ End of phase 5 ************************/
phase6:

	printf("Phase 7 ** Synchronous Random file write \n");

	time(&start);
	srand(start);

	if ((fd = open(argv[1], O_LARGEFILE|O_WRONLY|O_SYNC)) == -1 ) {
		perror(argv[1]);
		exit(0);
	}

	for (i=0; i<randsize; i++) {
		rnum = (rand() % size);
		rc = lseek(fd, rnum*blksize, 0);
		if ( rc != rnum*blksize ) {
			printf("Seek error on block %d\n", rnum);
			seekerrs++;
		}
		rc = write(fd, block, blksize);
		if (rc != blksize) {
			printf("write error on block %d\n", i);
			writerrs++;
		}
	}

	close(fd);

	time(&stop);
	if ((stop - start) == 0 ) {
		printf("Test too short, aborting\n");
		exit(0);
	}
	printf("	   Elapsed time %d - %dM/sec\n", (stop-start), 
						 ((bytes/(stop-start))/1024/1024));
	seektime = (double)((double)(stop-start)/(double)randsize);
	printf("	   Average seek-write time %.9f ms.\n",
						 seektime * 1000);

/************************************ End of phase 6 ************************/

}

usage(str)
char	*str;
{
	printf("usage : %s <filename> <size_in_128k_blocks>\n", str);
	exit(0);
}

stats()
{
	printf("Total %d seek errors\n", seekerrs);
	printf("Total %d read errors\n", readerrs);
	printf("Total %d write errors\n", writerrs);
}
