#include "ngram.h"
#include <leveldb/c.h>

#define SYNC_KEY "*.*.*.*"

struct Request {
	char key[MAX_LEN + 16];
	int64_t value;
}request[DISK_UPDATE_SIZE];
leveldb_t *db;
leveldb_options_t *options;
leveldb_cache_t *cache;
leveldb_readoptions_t *roptions;
leveldb_writeoptions_t *woptions;
char *err = NULL;
int len;
size_t read_len;
char buffer[BUFFER_SIZE], *oval;
FILE *file;
int64_t segt, tott, cnt, i;
struct timeval start, end;

int
main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: ./leveldb_update [operations_file_name] [leveldb_directory] [update_time]\n");
		return -1;
	}

	if ((file = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Failed to open the operations file \"%s\"\n", argv[1]);
		return -1;
	}
	for (i = 0; fscanf(file, "%"PRId64"\t", &request[i].value) != EOF; ++i) {
		fgets(buffer, BUFFER_SIZE, file);
		for (len = strlen(buffer) - 1; len >= 0 && (buffer[len] == '\r' || buffer[len] == '\n'); buffer[len--] = 0);
		++len;
		strcpy(request[i].key, buffer);
	}
	fclose(file);

	if ((file = fopen(argv[3], "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for recording running time\n", argv[3]);
		return -1;
	}
	options = leveldb_options_create();
	leveldb_options_set_create_if_missing(options, 1);
	// default: write_buffer_size: 4MB, block_cache: 8MB, block_size ~ 4KB
	leveldb_options_set_write_buffer_size(options, BLK_SIZE * 1024);
	leveldb_options_set_cache(options, cache = leveldb_cache_create_lru(BLK_SIZE * 2048));
	leveldb_options_set_block_size(options, BLK_SIZE);
	leveldb_options_set_compression(options, 0);
	db = leveldb_open(options, argv[2], &err);
	if (err != NULL) {
			fprintf(stderr, "Failed to open leveldb.\n");
			return -1;
	}
	leveldb_free(err);
	woptions = leveldb_writeoptions_create();
	roptions = leveldb_readoptions_create();
	gettimeofday(&start, NULL);
	for (i = tott = 0; i < DISK_UPDATE_SIZE; ++i) {
		leveldb_writeoptions_set_sync(woptions, 0);
		oval = leveldb_get(db, roptions, request[i].key, strlen(request[i].key), &read_len, &err);
		if (err != NULL) {
			fprintf(stderr, "Failed to read from leveldb.\n");
			return -1;
		}
		leveldb_free(err);
		if (read_len == 0)
			cnt = 0;
		else {
			oval[read_len] = 0;
			sscanf(oval, "%"PRId64"", &cnt);
		}
		leveldb_free(oval);
		if (request[i].value == 0 || cnt + request[i].value == 0) {
			leveldb_delete(db, woptions, request[i].key, strlen(request[i].key), &err);
			if (err != NULL) {
				fprintf(stderr, "Failed to delete key/value pair from leveldb.\n");
				return -1;
			}
			leveldb_free(err);
		} else {
			sprintf(buffer, "%"PRId64"%c", cnt + request[i].value, '\0');
			leveldb_put(db, woptions, request[i].key, len, buffer, strlen(buffer), &err);
			if (err != NULL) {
				fprintf(stderr, "Failed to write to leveldb.\n");
				return -1;
			}
			leveldb_free(err);
		}
		if ((i + 1) % 10000 == 0) {
			leveldb_writeoptions_set_sync(woptions, 1);
			sprintf(buffer, "%04"PRId64"", i % 10000);
			leveldb_put(db, woptions, SYNC_KEY, 7, buffer, 4, &err);
			if (err != NULL) {
				fprintf(stderr, "Failed to write to leveldb.\n");
				return -1;
			}
			leveldb_free(err);
			sync();
			gettimeofday(&end, NULL);
			segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
			tott += segt;
			fprintf(file, "%"PRId64"\t%"PRId64"\n", i + 1, segt);
			fflush(file);
			fprintf(stdout, "\rOperated %"PRId64" items in %"PRId64"us", i + 1, tott);
			fflush(stdout);
			gettimeofday(&start, NULL);
		}
	}
	fprintf(stdout, "\rOperated %"PRId64" items in %"PRId64"us\n", i, tott);
	fflush(file);
	fclose(file);
	return 0;
}
