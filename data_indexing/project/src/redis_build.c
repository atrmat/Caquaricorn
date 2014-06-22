#include <hiredis/hiredis.h>
#include <Judy.h>
#include "ngram.h"

#define BUILD_SIZE 2000000000
char buffer[BUFFER_SIZE];
FILE *file, *rbt, *temp;
Pvoid_t PJSLArray = (PWord_t)NULL;
PWord_t PValue;
Word_t Bytes;
int64_t cnt, i, j, n, itemn, segt, tott, totlen, lastt, umem, itemt;
char *keypool;
int len;
struct Item *items;
struct timeval start, end, gstart;
redisContext* c;
redisReply* r;

int
main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: ./redis_build [temp_dir_name] [redis_building_time]\n");
		return -1;
	}
	c = redisConnect("127.0.0.1", 6379);
	if (c->err) {
		redisFree(c);
		fprintf(stderr, "Failed to connect to redisServer\n");
		return -1;
	}
	if ((rbt = fopen(argv[2], "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for recording running time\n", argv[2]);
		return -1;
	}
	srand((unsigned)time(NULL));
	gettimeofday(&gstart, NULL);
	for (i = n = tott = lastt = itemt = 0; i < TEMP_N && (BUILD_SIZE < 0 || n < BUILD_SIZE); ++i) {
		// Reading old temp file
		sprintf(buffer, "%s/%s%"PRId64".txt", argv[1], TEMP_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", buffer);
			return -1;
		}
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
			len += strlen(buffer) + 1;
			JSLN(PValue, PJSLArray, (uint8_t *)buffer);
		}
		JSLFA(Bytes, PJSLArray);

		// Making new temp file
		fprintf(stdout, "\r%.2lfs: Making new temp file %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		if ((temp = fopen(TEMP_FILE_NAME, "w")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for writing\n", TEMP_FILE_NAME);
			return -1;
		}
		for (; itemn > 0; ) {
			j = (rand() << 16) + rand();
			j = (j > 0 ? j % itemn : (-j) % itemn);
			if (fputs(keypool + items[j].key, temp) == EOF || putc('\n', temp) == EOF) {
				fprintf(stderr, "Failed to write to temp file \"%s\"\n", TEMP_FILE_NAME);
				return -1;
			}
			--items[j].value;
			if (items[j].value == 0)
				items[j] = items[--itemn];
		}
		fflush(temp);
		fclose(temp);
		free(keypool);
		free(items);

		// Reading temp data
		fprintf(stdout, "\r%.2lfs: Reading temp data %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		if ((temp = fopen(TEMP_FILE_NAME, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", TEMP_FILE_NAME);
			return -1;
		}
		gettimeofday(&start, NULL);
		while (fgets(buffer, BUFFER_SIZE, temp) != NULL) {
			for (len = strlen(buffer) - 1; buffer[len] && (buffer[len] == '\r' || buffer[len] == '\n'); buffer[len--] = 0);
			++len;
			if (len < MIN_LEN || len > MAX_LEN)
				continue;
			if ((r = (redisReply*)redisCommand(c, "INCR %s", buffer)) == NULL) {
				fprintf(stderr, "Failed to execute command \"INCR %s\"\n", buffer);
				redisFree(c);
				return -1;
			}
			if (r->type != REDIS_REPLY_INTEGER) {
				fprintf(stderr, "Bad return for command \"INCR %s\"\n", buffer);
				freeReplyObject(r);
				redisFree(c);
				return -1;
			}
			if (r->integer == 1)
				++itemt;
			freeReplyObject(r);
			if (++n % 312500 == 0) {
				gettimeofday(&end, NULL);
				segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
				if ((r = (redisReply*)redisCommand(c, "INFO Memory")) == NULL) {
					fprintf(stderr, "Failed to execute command \"INFO Memory\"\n");
					redisFree(c);
					return -1;
				}
				if (r->type != REDIS_REPLY_STRING) {
					fprintf(stderr, "Bad return for command \"INFO Memory\"\n");
					freeReplyObject(r);
					redisFree(c);
					return -1;
				}
				for (j = 0; r->str[j] && r->str[j] != ':'; ++j);
				sscanf(r->str + j + 1, "%"PRId64"", &umem);
				freeReplyObject(r);
				fprintf(rbt, "%"PRId64"\t%"PRId64"\t%"PRId64"us\t%"PRId64"B\n", n, itemt, segt, umem);
				fflush(rbt);
				tott += segt + lastt;
				lastt = 0;
				fprintf(stdout, "\rFile %"PRId64": %"PRId64" of %"PRId64" in %"PRId64"us used %"PRId64"B    ", i, n, itemt, tott, umem);
				gettimeofday(&start, NULL);
			}
			if (BUILD_SIZE > 0 && n >= BUILD_SIZE)
				break;
		}
		if (n % 312500) {
			gettimeofday(&end, NULL);
			lastt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		} else
			lastt = 0;
		fclose(temp);
	}
	redisFree(c);
	fflush(rbt);
	fclose(rbt);
	return 0;
}
