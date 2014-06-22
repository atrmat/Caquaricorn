#include <hiredis/hiredis.h>
#include "ngram.h"

#define UPDATE_SIZE 1000000
#define SAMPLE_RANGE 80
#define INCREASE_THRESHOLD 100
char buffer[BUFFER_SIZE];
char ids[UPDATE_SIZE][BUFFER_SIZE];
FILE *rmt, *temp;
int64_t cnt, i, j, segt, tott;
int len;
struct timeval start, end;
redisContext* c;
redisReply* r;

int
main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: ./redis_update [redis_updating_time]\n");
		return -1;
	}
	srand((unsigned)time(NULL));
	if ((temp = fopen(TEMP_FILE_NAME, "r")) == NULL) {
		fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", TEMP_FILE_NAME);
		return -1;
	}
	for (i = 0; i < UPDATE_SIZE; ++i) {
		cnt = rand() % SAMPLE_RANGE;
		for (j = 0; j < SAMPLE_RANGE; ) {
			if (fgets(buffer, BUFFER_SIZE, temp) == NULL) {
				fprintf(stderr, "Failed to read temp file \"%s\"\n", TEMP_FILE_NAME);
				return -1;
			}
			for (len = strlen(buffer) - 1; buffer[len] && (buffer[len] == '\r' || buffer[len] == '\n'); buffer[len--] = 0);
			++len;
			if (len >= MIN_LEN && len <= MAX_LEN) {
				if (j == cnt)
					strcpy(ids[i], buffer);
				++j;
			}
		}
	}
	fclose(temp);
	gettimeofday(&start, NULL);
	c = redisConnect("127.0.0.1", 6379);
	if (c->err) {
		redisFree(c);
		fprintf(stderr, "Failed to connect to redisServer\n");
		return -1;
	}
	if ((rmt = fopen(argv[1], "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for recording running time\n", argv[2]);
		return -1;
	}
	for (i = tott = 0; i < UPDATE_SIZE; ++i) {
		if ((r = (redisReply*)redisCommand(c, "GET %s", ids[i])) == NULL) {
			fprintf(stderr, "Failed to execute command \"GET %s\"\n", ids[i]);
			redisFree(c);
			return -1;
		}
		if (r->type != REDIS_REPLY_STRING) {
			fprintf(stderr, "Bad return for command \"GET %s\"\n", ids[i]);
			freeReplyObject(r);
			redisFree(c);
			return -1;
		}
		cnt = atoi(r->str) + ((rand() & 1) ? -(rand() % atoi(r->str)) : rand() % INCREASE_THRESHOLD);
		freeReplyObject(r);
		if ((r = (redisReply*)redisCommand(c, "SET %s %"PRId64"", ids[i], cnt)) == NULL) {
			fprintf(stderr, "Failed to execute command \"SET %s %"PRId64"\"\n", ids[i], cnt);
			redisFree(c);
			return -1;
		}
		if (r->type != REDIS_REPLY_STATUS || strcmp(r->str, "OK")) {
			fprintf(stderr, "Bad return for command \"SET %s %"PRId64"\"\n", ids[i], cnt);
			freeReplyObject(r);
			redisFree(c);
			return -1;
		}
		freeReplyObject(r);
		if ((i + 1) % 100 == 0) {
			gettimeofday(&end, NULL);
			segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
			fprintf(rmt, "%"PRId64"\t%"PRId64"us\n", i + 1, segt);
			fflush(rmt);
			tott += segt;
			fprintf(stdout, "\r%"PRId64" in %"PRId64"us    ", i + 1, tott);
			gettimeofday(&start, NULL);
		}
	}
	fprintf(stdout, "\r%"PRId64" in %"PRId64"us    ", i, tott);
	redisFree(c);
	fflush(rmt);
	fclose(rmt);
	return 0;
}
