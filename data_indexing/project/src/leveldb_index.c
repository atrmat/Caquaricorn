#include "ngram.h"
#include <leveldb/c.h>

char buffer[BUFFER_SIZE];
struct Request {
	char key[MAX_LEN + 16];
	int64_t value;
}request[DISK_INDEX_SIZE];
leveldb_t *db;
leveldb_options_t *options;
leveldb_cache_t *cache;
leveldb_readoptions_t *roptions;
char *err = NULL;
char *val;
size_t read_len;
int len;
FILE *file;
int64_t cnt, i, seekn, tot;
struct timeval start, end;

int
main(int argc, char *argv[]) {
	if (argc != 5) {
		fprintf(stderr, "Usage: ./leveldb_index [leveldb_directory] [sample_input] [index_output] [index_time_file]\n");
		return -1;
	}
	if ((file = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "Failed to open sample file \"%s\" for reading\n", argv[1]);
		return -1;
	}
	for (i = 0; fscanf(file, "%"PRId64"\t", &cnt) != EOF; ++i) {
		fgets(buffer, BUFFER_SIZE, file);
		for (len = strlen(buffer) - 1; len >= 0 && (buffer[len] == '\r' || buffer[len] == '\n'); buffer[len--] = 0);
		++len;
		strcpy(request[i].key, buffer);
	}
	fclose(file);

	if ((file = fopen(argv[4], "w")) == NULL) {
		fprintf(stderr,  "Failed to open time information file \"%s\" for reading\n", argv[4]);
		return -1;
	}
	fprintf(stdout, "Start indexing\n");
	srand((unsigned)time(NULL));
	options = leveldb_options_create();
	leveldb_options_set_create_if_missing(options, 1);
	// default: write_buffer_size: 4MB, block_cache: 8MB, block_size ~ 4KB
	leveldb_options_set_write_buffer_size(options, BLK_SIZE * 1024);
	leveldb_options_set_cache(options, cache = leveldb_cache_create_lru(BLK_SIZE * 2048));
	leveldb_options_set_block_size(options, BLK_SIZE);
	leveldb_options_set_compression(options, 0);
	db = leveldb_open(options, argv[1], &err);
	if (err != NULL) {
		fprintf(stderr, "Failed to open leveldb.\n");
		return -1;
	}
	leveldb_free(err);
	roptions = leveldb_readoptions_create();
	gettimeofday(&start, NULL);
	for (i = seekn = tot = 0; i < DISK_INDEX_SIZE; ++i) {
		val = leveldb_get(db, roptions, request[i].key, strlen(request[i].key), &read_len, &err);
		if (err != NULL) {
			fprintf(stderr, "Failed to read from leveldb.\n");
			return -1;
		}
		leveldb_free(err);
		if (read_len == 0)
			request[i].value = 0;
		else {
			val[read_len] = 0;
			sscanf(val, "%"PRId64"", &request[i].value);
		}
		leveldb_free(val);
		if ((i + 1) % 1000 == 0) {
			gettimeofday(&end, NULL);
			tot += (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
			fprintf(stdout, "\rIndexed %"PRId64" items in %"PRId64"us", i + 1, tot);
			fflush(stdout);
			fprintf(file, "Indexed %"PRId64" items in %"PRId64"us\n", i + 1, tot);
			fflush(file);
			gettimeofday(&start, NULL);
		}
	}
	fprintf(stdout, "\rIndexed %"PRId64" items in %"PRId64"us\n", i, tot);
	fprintf(file, "Indexed %"PRId64" items in %"PRId64"us\n", i, tot);
	fflush(file);
	fclose(file);
	leveldb_cache_destroy(cache);
	leveldb_readoptions_destroy(roptions);
	leveldb_options_destroy(options);
	leveldb_close(db);
	fprintf(stdout, "Used %"PRId64" us and seeked %"PRId64" times to index %"PRId64" keys\n", tot, seekn, i);
	if ((file = fopen(argv[3], "w")) == NULL) {
		fprintf(stdout, "Failed to open file \"%s\" for storing indexing results\n", argv[3]);
		return -1;
	}
	for (i = 0; i < DISK_INDEX_SIZE; ++i)
		fprintf(file, "%"PRId64"\t%s\n", request[i].value, request[i].key);
	fflush(file);
	fclose(file);
	return 0;
}
