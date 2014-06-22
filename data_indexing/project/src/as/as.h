#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <Judy.h>

#define ASMIN(x,y) ((x)<(y)?(x):(y))

#define ZFN 200
#define BUFFER_SIZE 256
#define DATA_PREFIX "googlebooks-eng-all-3gram-20090715-"
#define TEMP_PREFIX "as_temp_"

#define BCTN 1024
#define HEAD_SIZE (32*1024)
#define WORD_MAX_LENGTH 15
#define KEY_MAX_LENGTH ((WORD_MAX_LENGTH)*3+2)
#define KEY_BLK_SIZE (64*1024)
#define KEY_ELEM_NUM (KEY_BLK_SIZE/(KEY_MAX_LENGTH+1+2*sizeof(int64_t)))
#define VALUE_BLK_SIZE (4*1024)
#define VALUE_ELEM_SIZE 128
#define AS_NUMBER (VALUE_ELEM_SIZE/(WORD_MAX_LENGTH+1))
#define VALUE_ELEM_NUM (VALUE_BLK_SIZE/VALUE_ELEM_SIZE)

struct Key_Head {
	int64_t bctn;
	struct BcTreeInfo {
		int64_t itemn;
		int64_t blockn;
		int64_t start;
	} bctinfo[BCTN];
	int8_t fill[HEAD_SIZE - sizeof(int64_t) - BCTN * sizeof(struct BcTreeInfo)];
};

struct Key_Block {
	struct Key_Elem {
		char key[KEY_MAX_LENGTH + 1];
		int64_t value;
		int64_t child;
	} elems[KEY_ELEM_NUM];
};

struct Value_Block {
	struct Value_Elem {
		char value[AS_NUMBER][WORD_MAX_LENGTH + 1];
	} elem[VALUE_ELEM_NUM];
};

struct Item {
	char key[KEY_MAX_LENGTH + 1];
	int64_t value;
};

double
gettime(struct timeval *start) {
	struct timeval end;
	gettimeofday(&end, NULL);
	return end.tv_sec - start->tv_sec + (end.tv_usec - start->tv_usec) / 1000000.0;
}

unsigned
BKDRHash(char *str) {
	unsigned hash;

	for (hash = 0; *str; hash = (hash << 7) + (hash << 2) - hash + (*str++));
	return hash % BCTN;
}

int
MakeNewIndex(int64_t *index, int64_t offset, int64_t cur, int64_t itemn) {
	int64_t i;

	for (i = 0; i < KEY_ELEM_NUM && offset + i < itemn; ++i) {
		index[offset + i] = cur++;
		if ((offset + i + 1) * KEY_ELEM_NUM < itemn)
			cur = MakeNewIndex(index, (offset + i + 1) * KEY_ELEM_NUM, cur, itemn);
	}
	return cur;
}
