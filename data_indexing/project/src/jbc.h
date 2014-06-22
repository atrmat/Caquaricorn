#include "ngram.h"
#include <Judy.h>

#define TOTAL 1000000
#define BUFFER_SIZE 256
#define IO_BUFFER_SIZE (16 * 1024)
#define HEAD_SIZE (32 * 1024)
#define ELEM_NUM (BLK_SIZE / ELEM_SIZE)

struct BTreeInfo {
		int64_t itemn;
		int64_t blockn;
		int64_t start;
};

struct Head {
	int64_t btreen;
	struct BTreeInfo btreeinfo[TEMP_N];
	int8_t fill[HEAD_SIZE - sizeof(int64_t) - TEMP_N * sizeof(struct BTreeInfo)];
};

struct Head_N {
	int64_t btreen; // btree num
	struct BTreeInfo btreen_info;
	int8_t fill[HEAD_SIZE - sizeof(int64_t) - sizeof(struct BTreeInfo)];
};

struct Block {
	struct Elem {
		char key[MAX_LEN + 1];
		int64_t value;
		int64_t child;
	} elems[ELEM_NUM];
};

unsigned
BKDRHash(uint8_t *str) {
	unsigned hash;

	for (hash = 0; *str; hash = (hash << 7) + (hash << 2) - hash + (*str++));
	return hash % TEMP_N;
}

int
MakeNewIndex(int64_t *index, int64_t offset, int64_t cur, int64_t itemn) {
	int64_t i;

	for (i = 0; i < ELEM_NUM && offset + i < itemn; ++i) {
		index[offset + i] = cur++;
		if ((offset + i + 1) * ELEM_NUM < itemn)
			cur = MakeNewIndex(index, (offset + i + 1) * ELEM_NUM, cur, itemn);
	}
	return cur;
}
