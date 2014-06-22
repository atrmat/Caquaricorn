#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>                                                   
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MIN(x, y) ((x)<(y)?(x):(y))
#define MAX(x, y) ((x)>(y)?(x):(y))

#define TEMP_N 1024
#define TEMP_FILE_NAME "../data/temp.txt"
#define TEMP_FILE_NAME_N "/tmp/data/temp.txt"
#define INDEX_SAMPLE_FILE_NAME "../data/index_sample.txt"
#define UPDATE_SAMPLE_FILE_NAME "../data/update_sample.txt"
#define TEMP_PREFIX "temp_"
#define BLK_SIZE (64 * 1024)
#define ELEM_SIZE 64
#define MIN_LEN 10
#define MAX_LEN (ELEM_SIZE - sizeof(int64_t) - sizeof(int64_t) - 1)
#define BUFFER_SIZE 256
#define DISK_INDEX_SIZE 1024000
#define DISK_UPDATE_SIZE 10240000
#define TEMP_FILE_MAX_LINES 441000

struct Item {
	int64_t key;
	int64_t value;
};

double
gettime(struct timeval *start) {
	struct timeval end;
	gettimeofday(&end, NULL);
	return end.tv_sec - start->tv_sec + (end.tv_usec - start->tv_usec) / 1000000.0;
}
