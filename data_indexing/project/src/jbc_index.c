#include "jbc.h"
#include "jbc_blooms.h"

char buffer[BUFFER_SIZE];

struct Head head;
struct Block block, cache[TEMP_N];
Pvoid_t PJSLArray = (PWord_t)NULL;
PWord_t PValue;
Word_t Bytes;
int64_t cur, i, len, f, t, mid, seekn, tot;
struct timeval start, end;
FILE *sample, *itf;
unsigned hash;
int btree, cmp_res, cnt;
char use_cache;
counting_bloom_t *bloom;
char *bloom_file = "/tmp/datatmp/jbc_blooms";

int
main(int argc, char *argv[]) {
	if (argc != 6) {
		fprintf(stderr, "Usage: ./jbc_index [btree_file_name] [cache(y/n)] [sample_input] [index_output] [index_time_file]\n");
		return -1;
	}
	use_cache = (argv[2][0] == 'y');
	if ((sample = fopen(argv[3], "r")) == NULL) {
		fprintf(stderr, "Failed to open sample file \"%s\" for reading\n", argv[3]);
		return -1;
	}
	if ((itf = fopen(argv[5], "w")) == NULL) {
		fprintf(stderr, "Failed to open time information file \"%s\" for reading\n", argv[5]);
		return -1;
	}
	for (i = 0; fscanf(sample, "%d\t", &cnt) != EOF; ++i) {
		fgets(buffer, BUFFER_SIZE, sample);
		for (len = strlen(buffer) - 1; len >= 0 && (buffer[len] == '\r' || buffer[len] == '\n'); buffer[len--] = 0);
		++len;
		JSLI(PValue, PJSLArray, (uint8_t *)buffer);
		if (PValue == NULL) {
			fprintf(stderr, "Failed to alloc memory for reading sample file\n");
			return -1;
		}
		*PValue = 0;
	}
	fclose(sample);

	fprintf(stdout, "Start indexing\n");
	gettimeofday(&start, NULL);
	if ((btree = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open BTree file \"%s\" for reading\n", argv[1]);
		return -1;
	}
	lseek64(btree, 0, SEEK_SET);
	read(btree, &head, HEAD_SIZE);
	if (head.btreen != TEMP_N) {
		fprintf(stderr, "Error number of BTree %"PRId64" instead of %d\n", head.btreen, TEMP_N);
		return -1;
	}
	if (use_cache)
		for (i = 0; i < TEMP_N; ++i) {
			lseek64(btree, head.btreeinfo[i].start, SEEK_SET);
			read(btree, &cache[i], BLK_SIZE);
		}
	gettimeofday(&end, NULL);
	tot = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	buffer[0] = '\0';
	fprintf(stderr, "\n open the bloom filter\n");
	if (!(bloom = new_counting_bloom_from_file(CAPACITY, ERROR_RATE, bloom_file))) {
        	fprintf(stderr, "ERROR: Could not create bloom filter from file: %s \n",bloom_file);
        	return -1;
    }
	JSLF(PValue, PJSLArray, (uint8_t *)buffer);
	for (i = seekn = 0; PValue != NULL; ++i) {
		if (!counting_bloom_check(bloom, buffer, strlen(buffer))){
			fprintf(stderr, "the item %s no exist in bloom filter!\n",buffer);
			fprintf(itf, "the item %s no exist in bloom filter!\n",buffer);
			break;
		}
		gettimeofday(&start, NULL);
		hash = BKDRHash((uint8_t *)buffer);
		for (cur = 0; ; ) {
			if (cur >= ELEM_NUM)
				read(btree, &block, BLK_SIZE);
			for (f = 0, t = MIN(ELEM_NUM, head.btreeinfo[hash].itemn - cur) - 1; f < t; ) {
				mid = f + ((t - f + 1) >> 1);
				if (strcmp(use_cache && cur < ELEM_NUM ? cache[hash].elems[mid].key : block.elems[mid].key, buffer) > 0)
					t = mid - 1;
				else
					f = mid;
			}
			cmp_res = strcmp(use_cache && cur < ELEM_NUM ? cache[hash].elems[f].key : block.elems[f].key, buffer);
			if (cmp_res >= 0 || (use_cache && cur < ELEM_NUM ? cache[hash].elems[f].child : block.elems[f].child) == 0){
				if(cmp_res < 0)
					fprintf(stderr,"\n the buffer: %s is not found \n", buffer);
					fprintf(itf,"\n the buffer: %s is not found \n", buffer);
				break;
			}else {
				lseek64(btree, use_cache && cur < ELEM_NUM ? cache[hash].elems[f].child : block.elems[f].child, SEEK_SET);
				++seekn;
				cur = (cur + 1) * ELEM_NUM;
			}
		}
		gettimeofday(&end, NULL);
		tot += (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		*PValue = (strcmp(block.elems[f].key, buffer) == 0 ? block.elems[f].value : 0);
		if ((i + 1) % 1000 == 0) {
			fprintf(stdout, "\rIndexed %"PRId64" items in %"PRId64"us", i + 1, tot);
			fflush(stdout);
			fprintf(itf, "Indexed %"PRId64" items in %"PRId64"us\n", i + 1, tot);
			fflush(itf);
		}
		JSLN(PValue, PJSLArray, (uint8_t *)buffer);
	}
	fprintf(stdout, "\rIndexed %"PRId64" items in %"PRId64"us\n", i, tot);
	fprintf(itf, "Indexed %"PRId64" items in %"PRId64"us\n", i, tot);
	fprintf(stdout, "Used %"PRId64" us and seeked %"PRId64" times to index %"PRId64" keys\n", tot, seekn, i);
	fprintf(itf, "Used %"PRId64" us and seeked %"PRId64" times to index %"PRId64" keys\n", tot, seekn, i);
	fflush(itf);
	fclose(itf);
	close(btree);

	if ((sample = fopen(argv[4], "w")) == NULL) {
		fprintf(stdout, "Failed to open file \"%s\" for storing indexing results\n", argv[4]);
		return -1;
	}
	buffer[0] = '\0';
	JSLF(PValue, PJSLArray, (uint8_t *)buffer);
	while (PValue != NULL) {
		fprintf(sample, "%ld\t%s\n", *PValue, buffer);
		JSLN(PValue, PJSLArray, (uint8_t *)buffer);
	}
	JSLFA(Bytes, PJSLArray);
	free_counting_bloom(bloom);
	fflush(sample);
	fclose(sample);
	return 0;
}
