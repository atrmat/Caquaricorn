#include "ngram.h"
#include <leveldb/c.h>
#include <Judy.h>

#define SYNC_KEY "*.*.*.*"

char buffer[BUFFER_SIZE];
leveldb_t *db;
leveldb_options_t *options;
leveldb_cache_t *cache;
leveldb_readoptions_t *roptions;
leveldb_writeoptions_t *woptions;
char *err = NULL;
size_t read_len;
int len;
FILE *file, *lbt;
Pvoid_t PJSLArray = (PWord_t)NULL;
PWord_t PValue;
Word_t Bytes;
int64_t cnt, i, j, itemn, segt, tott, totlen, fsize, all, got, used;
double cur_time;
char *keypool, *oval, nval[BUFFER_SIZE];
struct Item *items;
struct timeval start, end, gstart;

void
sig_handler(int sig) {
	gettimeofday(&start, NULL);
	fprintf(stdout, "\nClose leveldb before exit: Start!\n");
	leveldb_cache_destroy(cache);
	leveldb_readoptions_destroy(roptions);
	leveldb_writeoptions_destroy(woptions);
	leveldb_options_destroy(options);
	leveldb_close(db);
	fprintf(stdout, "Close leveldb before exit: Done!\n");
	gettimeofday(&end, NULL);
	segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	fprintf(lbt, "Oper %"PRId64" + Close %"PRId64" = Total %"PRId64"us\n", tott, segt, tott + segt);
	fflush(lbt);
	fclose(lbt);
	exit(-1);
}

int
main(int argc, char *argv[]) {
	signal(SIGINT, sig_handler);
	if (argc != 4) {
		fprintf(stderr, "Usage: ./leveldb_build [temp_dir_name] [leveldb_directory] [leveldb_building_time]\n");
		return -1;
	}
	if ((lbt = fopen(argv[3], "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for recording running time\n", argv[3]);
		return -1;
	}
	srand((unsigned)time(NULL));
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
	// leveldb_readoptions_set_fill_cache(roptions, 0);
	gettimeofday(&gstart, NULL);
	for (tott = i = 0; i < TEMP_N; ++i) {
		cur_time = gettime(&gstart);
		// Reading old temp file
		sprintf(buffer, "%s/%s%"PRId64".txt", argv[1], TEMP_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", buffer);
			return -1;
		}
		fprintf(stdout, "%.2lfs: Reading old temp file %"PRId64"\n", gettime(&gstart), i);
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
		if ((keypool = (char *)malloc(totlen)) == NULL) {
			fprintf(stderr, "Failed to alloc keypool for storing old temp strings\n");
			return -1;
		}
		if ((items = (struct Item *)malloc(itemn * sizeof(struct Item))) == NULL) {
			fprintf(stderr, "Failed to alloc memory for storing old temp items\n");
			return -1;
		}
		buffer[0] = '\0';
		JSLF(PValue, PJSLArray, (uint8_t *)buffer);
		for (j = len = 0; PValue != NULL; ++j) {
			items[j].key = len;
			items[j].value = *PValue;
			strcpy(keypool + len, buffer);
			keypool[len + strlen(buffer)] = 0;
			len += strlen(buffer) + 1;
			JSLN(PValue, PJSLArray, (uint8_t *)buffer);
		}
		JSLFA(Bytes, PJSLArray);

		// Making new temp file
		fprintf(stdout, "%.2lfs: Making new temp file %"PRId64"\n", gettime(&gstart), i);
		fflush(stdout);
		if ((file = fopen(TEMP_FILE_NAME, "w")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for writing\n", TEMP_FILE_NAME);
			return -1;
		}
		for (all = 0; itemn > 0; ++all) {
			j = rand() % itemn;
			fprintf(file, "%s\n", keypool + items[j].key);
			if ((--items[j].value) == 0)
				items[j] = items[--itemn];
		}
		fflush(file);
		fsize = ftell(file);
		fclose(file);
		free(keypool);
		free(items);

		// Reading temp data and building leveldb
		fprintf(stdout, "%.2lfs: Reading temp data %"PRId64"\n", gettime(&gstart), i);
		fflush(stdout);
		gettimeofday(&start, NULL);
		if ((file = fopen(TEMP_FILE_NAME, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", TEMP_FILE_NAME);
			return -1;
		}
		leveldb_writeoptions_set_sync(woptions, 0);
		for (got = used = 0; fgets(buffer, BUFFER_SIZE, file) != NULL; ) {
			++got;
			for (len = strlen(buffer) - 1; len >=0 && (buffer[len - 1] == '\r' || buffer[len] == '\n'); --len)
				buffer[len] = 0;
			if (len < MIN_LEN || len > MAX_LEN)
				continue;
			oval = leveldb_get(db, roptions, buffer, len, &read_len, &err);
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
			sprintf(nval, "%"PRId64"%c", cnt + 1, '\0');
			leveldb_put(db, woptions, buffer, len, nval, strlen(nval), &err);
			if (err != NULL) {
				fprintf(stderr, "Failed to write to leveldb.\n");
				return -1;
			}
			leveldb_free(err);
			if ((++used) % 100000 == 0) {
				fprintf(stdout, "\r%"PRId64" / %"PRId64" / %"PRId64"", used, got, all);
				fflush(stdout);
			}
		}
		fclose(file);
		leveldb_writeoptions_set_sync(woptions, 1);
		sprintf(nval, "%04"PRId64"", i);
		leveldb_put(db, woptions, SYNC_KEY, 7, nval, 4, &err);
		if (err != NULL) {
			fprintf(stderr, "Failed to write to leveldb.\n");
			return -1;
		}
		leveldb_free(err);
		sync();
		gettimeofday(&end, NULL);
		segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		fprintf(lbt, "Temp %"PRId64" | %"PRId64"B | %"PRId64" items | OP %"PRId64"us\n", i, fsize, used, segt);
		fflush(lbt);
		fprintf(stdout, "\r%.2lf: %"PRId64" / %"PRId64" / %"PRId64" -- %"PRId64"us / %.2lfop/s ** ", gettime(&gstart), used, got, all, segt, (double)used * 1000000.0 / segt);
		cur_time = (gettime(&gstart) - cur_time) * (TEMP_N - i - 1);
		fprintf(stdout, "Left %dd ", (int)cur_time / 86400);
		cur_time -= (int)(cur_time / 86400) * 86400;
		fprintf(stdout, "%dh ", (int)cur_time / 3600);
		cur_time -= (int)(cur_time / 3600) * 3600;
		fprintf(stdout, "%dm ", (int)cur_time / 60);
		cur_time -= (int)(cur_time / 60) * 60;
		fprintf(stdout, "%.2lfs\n", cur_time);
		tott += segt;
	}

	gettimeofday(&start, NULL);
	leveldb_cache_destroy(cache);
	leveldb_readoptions_destroy(roptions);
	leveldb_writeoptions_destroy(woptions);
	leveldb_options_destroy(options);
	leveldb_close(db);
	gettimeofday(&end, NULL);
	segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	fprintf(lbt, "Oper %"PRId64" + Close %"PRId64" = Total %"PRId64"us\n", tott, segt, tott + segt);
	fflush(lbt);
	fclose(lbt);
	return 0;
}
