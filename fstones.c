/*
 *
 *
 * Paul Stephen Borile
 *
 *
 * f s t o n e s . c 	: Disk performance evaluator
 *
 */

static	char	*Version = "3.0";

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define	MAX_DIR_OPS	1000000 // 1 million

int	offset;
int	fd, fdsync;
struct	stat	buf;

int	seekerrs = 0, readerrs = 0, writerrs = 0;

int	nfiles = 0;
double	seektime;
double	servtime;

#define OPTS    "wrWRf:d:"

static struct option long_options[] = {
    {"write",      no_argument, 0,  'w' },
    {"read",       no_argument, 0,  'r' },
    {"rwrite",     no_argument, 0,  'W' },
    {"rread",      no_argument, 0,  'R' },
    {"dir",        required_argument, 0,  'd' },
    {"size",       required_argument, 0,  's' },
	{"blocksize",  required_argument, 0,  'b' },
    {"file",       required_argument, 0,  'f' },
    {0,         0,                 0,  0 }
};

void usage(char *str);

int main(argc, argv)
int	argc;
char	**argv;
{
	int option_index = 0;
	char filename[512];
	int dir_operations = 0;
	int dir_opt = 0;
	int rread_opt = 0;
	int rwrite_opt = 0;
	int read_opt = 0;
	int write_opt = 0;
	int size_in_GB = 10; // default 10GB
	int blocksize_in_K = 1024; // default 1 MB

	long long	bytes;
	long long	size;
	long int 	blocksize;
	long int	randsize; 
	long int	syncsize;
	long long	rnum;

	time_t	start, stop;

	char	*block;
	int		opt;
	int		rc = 0;
	char	str[1024];

    while ((opt = getopt_long(argc, argv, OPTS, long_options, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'f':
            strcpy(filename, optarg);
            break;

        case 'b':
			dir_opt = 1;
            blocksize_in_K = atoi(optarg);
            break;

        case 's':
            size_in_GB = atoi(optarg);
            break;

        case 'd':
            dir_operations = atoi(optarg);
            break;

        case 'R':
            rread_opt = 1;
            break;

        case 'W':
            rwrite_opt = 1;
            break;

        case 'r':
            read_opt = 1;
            break;

        case 'w':
            write_opt = 1;
            break;
        default:                 /* '?' */
HELP:
            fprintf(stderr, "Usage: %s [--read] [--write] [--rread] [--rwrite] --size <in_GB> --blocksize <in_K> --file <filename> --dir <num_of_dir_operations>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
	}

	blocksize = blocksize_in_K * 1024;
	block	= malloc(blocksize);
	bytes	= size_in_GB * 1024L * 1024L * 1024L; // total bytes for read/write ops
	syncsize	= size_in_GB * 1024 * 1024; // size for sync operations is 1/1024 of normal since O_SYNC write are slow
	randsize = syncsize;
	memset(block, 'F', blocksize);

	printf("** fstones %s - (C) 1989-2022 Paul Stephen Borile\n", Version);
	printf("Blocksize for read/write operations is %dK\n", blocksize_in_K);


	if (write_opt)
	{
		printf("Phase 1 ** Sequential file creation \n");
		printf("	   Creating a %Ld GB file\n", bytes/1024/1024/1024);

		time(&start);

		if ((fd = open(filename, O_CREAT|O_RDWR, 0644)) == -1 ) {
			perror(filename);
			exit(0);
		}

		long long donebytes = 0;
		int ops_done = 0;
		while (donebytes <= bytes)
		{
			rc = write(fd, block, blocksize);
			if (rc != blocksize) {
				printf("write error on block %d\n", ops_done);
				writerrs++;
			}
			donebytes += blocksize;
			ops_done++;
		}

		close(fd);

		time(&stop);
		if ((stop - start) == 0 ) {
			printf("Test too short, aborting\n");
		}
		else
		{
			printf("	   Elapsed time %ld - %LdM/sec\n", (stop-start),
								((bytes/(stop-start))/1024/1024));
			servtime = (double)((double)(stop-start)/(double)ops_done);
			printf("	   Average write service time %.9f ms.\n", servtime*1000);
		}
	}

	if (read_opt)
	{

		printf("Phase 2 ** Sequential file reading \n");

		time(&start);

		if ((fd = open(filename, O_CREAT|O_RDWR, 0644)) == -1 ) {
			perror(filename);
			exit(0);
		}

		long long donebytes = 0;
		int ops_done = 0;
		while (donebytes <= bytes)
		{
			rc = read(fd, block, blocksize);
			if (rc != blocksize) {
				printf("read error on block %d\n", ops_done);
				writerrs++;
			}
			donebytes += blocksize;
			ops_done++;
		}

		close(fd);

		time(&stop);
		if ((stop - start) == 0 ) {
			printf("Test too short, aborting\n");
		}
		else
		{
			printf("	   Elapsed time %ld - %LdM/sec\n", (stop-start),
								((bytes/(stop-start))/1024/1024));
			servtime = (double)((double)(stop-start)/(double)ops_done);
			printf("	   Average read service time %.9f ms.\n",
								servtime*1000);
		}
	}

	if (rread_opt)
	{

		printf("Phase 3 ** Random file read \n");

		time(&start);
		srand(start);

		if ((fd = open(filename, O_RDONLY)) == -1 ) {
			perror(filename);
			exit(0);
		}

		for (long int i=0; i<randsize; i++) {
			rnum = (rand() % ((bytes/blocksize)-1));
			rc = lseek(fd, rnum, SEEK_SET);
			if ( rc != rnum ) {
				printf("Seek error on block %Ld\n", rnum);
				seekerrs++;
			}
			rc = read(fd, block, blocksize);
			if (rc != blocksize) {
				printf("read error on block %ld\n", i);
				readerrs++;
			}
		}

		close(fd);

		time(&stop);
		if ((stop - start) == 0 )
		{
			printf("Test too short, aborting\n");
		}
		else
		{
			printf("	   Elapsed time %ld - %LdM/sec\n", (stop-start),
								((bytes/(stop-start))/1024/1024));
			seektime = (double)((double)(stop-start)/(double)randsize);
			printf("	   Average seek-read time %.9f ms.\n",
								seektime * 1000);
		}
	}

	if (rwrite_opt)
	{
		printf("Phase 4 ** Random file write \n");

		time(&start);
		srand(start);

		if ((fd = open(filename, O_WRONLY)) == -1 ) {
			perror(filename);
			exit(0);
		}

		for (long int i=0; i<randsize; i++) {
			rnum = (rand() % ((bytes/blocksize)-1));
			rc = lseek(fd, rnum, SEEK_SET);
			if ( rc != rnum ) {
				printf("Seek error on block %Ld\n", rnum);
				seekerrs++;
			}
			rc = write(fd, block, blocksize);
			if (rc != blocksize) {
				printf("write error on block %ld\n", i);
				writerrs++;
			}
		}

		close(fd);

		time(&stop);
		if ((stop - start) == 0 ) {
			printf("Test too short, aborting\n");
		}
		else
		{
			printf("	   Elapsed time %ld - %LdM/sec\n", (stop-start),
								((bytes/(stop-start))/1024/1024));
			seektime = (double)((double)(stop-start)/(double)randsize);
			printf("	   Average seek-write time %.9f ms.\n", seektime * 1000);
		}
	}

	if (dir_opt)
	{
		printf("Phase 5 ** Directory test \n");

		time(&start);
		stat(filename, &buf);
		for (int i=0; i<dir_operations; i++) {
			sprintf(str,"%s-%d", filename, i);
			link(filename, str);
			stat(str, &buf);
		}
		for (int i=0; i<dir_operations; i++) {
			sprintf(str,"%s-%d", filename, i);
			stat(str, &buf);
			unlink(str);
		}
		time(&stop);

		if ((stop - start) == 0 ) {
			printf("Test too short, aborting\n");
		}
		else
		{
			printf("	   Elapsed time %ld - index %ld/sec\n", (stop-start),
							dir_operations/(stop-start) );
		}
	}
}
