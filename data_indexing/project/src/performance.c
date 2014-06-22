#define _LARGEFILE64_SOURCE
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

char buffer[1024 * 1024 + 1];
struct timeval start, end;
int file;
int64_t sdt, sst, i, j;

int
main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: ./performance mnt/btree.bin\n");
		return -1;
	}
	if ((file = open(argv[1],  O_LARGEFILE | O_RDONLY)) == -1) {
		fprintf(stderr, "Failed to open file \"%s\" for reading test\n", argv[1]);
		return -1;
	}
	if (lseek64(file, 0, SEEK_END) < ((off64_t)1 << 31)) {
		fprintf(stderr, "File \"%s\" is too small for reading test\n", argv[1]);
		return -1;
	}
	fprintf(stdout, "Disk Sequentially Reading Test...\n");
	lseek64(file, 0, SEEK_SET);
	for (i = sdt = 0 ; i < 1024; ++i) {
		gettimeofday(&start, NULL);
		if (read(file, buffer, (1 << 10)) != (1 << 10)) {
			fprintf(stderr, "Failed to read file \"%s\" sequentially\n", argv[1]);
			return -1;
		}
		gettimeofday(&end, NULL);
		sdt += (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	}
	fprintf(stdout, "Disk and Memory Sequentially Reading Test...\n");
	lseek64(file, 0, SEEK_SET);
	for (i = sst = 0; i < 1024; ++i) {
		gettimeofday(&start, NULL);
		for (j = 0; j < 64; ++j) {
			lseek64(file, (i << 20) + (j << 14) + ((j & 1) << 30), SEEK_SET);
			if (read(file, buffer, (1 << 14)) != (1 << 14)) {
				fprintf(stderr, "Failed to seek-and-read from file \"%s\"\n", argv[1]);
				return -1;
			}
		}
		gettimeofday(&end, NULL);
		sst += (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		fprintf(stdout, "\r%"PRId64"", i);
		fflush(stdout);
	}
	fprintf(stdout, "\r%"PRId64"\n", i);
	fprintf(stdout, "Disk Sequentially Reading 1MB: %.6lfus\n", (double) sdt / 1024);
	fprintf(stdout, "Disk Seek-and-Read 1MB: %.6lfus\n", (double) sst / 1024);
	close(file);
	return 0;
}
