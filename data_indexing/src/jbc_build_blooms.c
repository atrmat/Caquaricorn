#include "jbc.h"
#include "jbc_blooms.h"

char io_buffer[IO_BUFFER_SIZE];
char buffer[BUFFER_SIZE];
char TEMP_FILE_NAME_1[BUFFER_SIZE];
FILE *file, *bbt;
Pvoid_t PJSLArray = (PWord_t)NULL;
PWord_t PValue;
Word_t Bytes;
int64_t cur_pos, cnt, *idx, i, j, itemn, rdt, opt, wrt, segt, mems, totlen, fsize;
char *keypool;
int len, btree, io_offset, temp;
struct Item *items;
struct timeval start, end, gstart;
struct Head head;
struct Block block;

counting_bloom_t *bloom;

int
main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: ./jbc_build_blooms [temp_dir_name] [btree_file_name] [btree_building_time]\n");
		// /root/cx_src/src/jbc_build_blooms /tmp/datatmp /tmp/btree_blooms.bin /tmp/resulttmp/jbt_blooms.txt
		return -1;
	}
	srand((unsigned)time(NULL));
	sprintf(buffer,"%s/%s",argv[1],"jbc_blooms");
	if (!(bloom = new_counting_bloom(CAPACITY, ERROR_RATE, buffer))) {
        fprintf(stderr, "ERROR: Could not create bloom filter: %s \n",buffer);
        return -1;
    }
	gettimeofday(&gstart, NULL);
	for (cur_pos = HEAD_SIZE, rdt = opt = wrt = i = 0; i < TEMP_N; ++i) {
		// Reading old temp file
		sprintf(buffer, "%s/%s%"PRId64".txt", argv[1], TEMP_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", buffer);
			return -1;
		}
		gettimeofday(&end, NULL);
		fprintf(stdout, "\r%.2lfs: Reading old temp file %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		for (itemn = totlen = 0, PJSLArray = (Pvoid_t)NULL; fscanf(file, "%"PRId64"\t", &cnt) != EOF; ) {
			fgets(buffer, BUFFER_SIZE, file);
			len = strlen(buffer);
			for (--len; len >= 0 && (buffer[len] == '\r' || buffer[len] == '\n'); --len)
				buffer[len] = '\0';
			++len;
			JSLI(PValue, PJSLArray, (uint8_t *)buffer);
			if (PValue == NULL) {
				fprintf(stderr, "Failed to alloc memory for reading old temp file\n");
				return -1;
			}
			if (*PValue == 0) {
				totlen += len + 1;
				++itemn;
			}
			*PValue += cnt;
		}
		fclose(file);
		buffer[0] = '\0';
		JSLF(PValue, PJSLArray, (uint8_t *)buffer);
		for (j = len = 0; PValue != NULL; ++j) {
			counting_bloom_add(bloom, buffer, strlen(buffer));
			JSLN(PValue, PJSLArray, (uint8_t *)buffer);
		}
		JSLFA(Bytes, PJSLArray);
	}
	free_counting_bloom(bloom);
	return 0;
}
