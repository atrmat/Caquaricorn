#include "ngram.h"

FILE *file;
int i, j, k;
char buffer[BUFFER_SIZE];
char samples[MAX(DISK_INDEX_SIZE, DISK_UPDATE_SIZE)][MAX_LEN + 16];
char temp[TEMP_FILE_MAX_LINES][MAX_LEN + 16];
struct Item item[MAX(DISK_INDEX_SIZE, DISK_UPDATE_SIZE)];

int
get_sample(char *dir, int size) {
	FILE *file;
	int per = size / TEMP_N, i, j, itemn, pos, g;

	for (i = pos = 0; i < TEMP_N; ++i) {
		sprintf(buffer, "%s/%s%d.txt", dir, TEMP_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\"\n", buffer);
			return -1;
		}
		for (itemn = 0; fgets(buffer, BUFFER_SIZE, file) != NULL; ) {
			for (j = strlen(buffer) - 1; j > 0 && (buffer[j] == '\r' || buffer[j] == '\n'); --j)
				buffer[j] = 0;
			for (j = 0; buffer[j] && buffer[j] != '\t'; ++j);
			g = strlen(buffer + j + 1);
			if (!buffer[j] || g < MIN_LEN || g > MAX_LEN)
				continue;
			strcpy(temp[itemn++], buffer);
		}
		fclose(file);
		for (j = 0; j < per; ++j) {
			g = j + rand() % (itemn - j);
			item[pos].key = pos;
			item[pos].value = (j & 3);
			strcpy(samples[pos++], temp[g]);
			if (g != j)
				strcpy(temp[g], temp[j]);
		}
		fprintf(stdout, "\r\tProcessed temp file %d", i);
		fflush(stdout);
	}
	return 0;
}

int
main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: ./get_sample [temp_directory_name]\n");
		return -1;
	}
	srand((unsigned)time(NULL));
	fprintf(stdout, "*** Generating index samples ***\n");
	if (get_sample(argv[1], DISK_INDEX_SIZE))
		return -1;
	fprintf(stdout, "\n\tGenerating the index samples to \"%s\"...", INDEX_SAMPLE_FILE_NAME);
	file = fopen(INDEX_SAMPLE_FILE_NAME, "w");
	for (i = 0; i < DISK_INDEX_SIZE; ++i) {
		j = i + rand() % (DISK_INDEX_SIZE - i);
		fprintf(file, "%s\n", samples[item[j].key]);
		item[j].key = item[i].key;
	}
	fflush(file);
	fclose(file);
	fprintf(stdout, "Done.\n");
	fprintf(stdout, "*** Generating update samples ***\n");
	if (get_sample(argv[1], DISK_UPDATE_SIZE))
		return -1;
	fprintf(stdout, "\n\tGenerating the update samples to \"%s\"...", UPDATE_SAMPLE_FILE_NAME);
	file = fopen(UPDATE_SAMPLE_FILE_NAME, "w");
	for (i = 0; i < DISK_UPDATE_SIZE; ++i) {
		j = i + rand() % (DISK_UPDATE_SIZE - i);
		for (k = 0; samples[item[j].key] && samples[item[j].key][k] != '\t'; ++k);
		switch (item[j].value) {
			case 0:
				fprintf(file, "0%s\n", samples[item[j].key] + k);
				break;
			case 1:
				fprintf(file, "1%s\n", samples[item[j].key] + k);
				break;
			case 2:
				fprintf(file, "-1%s\n", samples[item[j].key] + k);
				break;
			default:
				fprintf(file, "-%s\n", samples[item[j].key]);
				break;
		}
		item[j] = item[i];
	}
	fflush(file);
	fclose(file);
	fprintf(stdout, "Done.\n");
	return 0;
}
