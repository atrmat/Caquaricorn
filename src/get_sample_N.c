#include "ngram.h"

FILE *file;
int i, j, k;
char buffer[BUFFER_SIZE];
char samples[MAX(DISK_INDEX_SIZE, DISK_UPDATE_SIZE)][MAX_LEN + 16];
char temp[TEMP_FILE_MAX_LINES][MAX_LEN + 16];
struct Item item[MAX(DISK_INDEX_SIZE, DISK_UPDATE_SIZE)];

int
get_sample(char *dir, int size, char *temp_N) {
	FILE *file;
	int per = size / TEMP_N, i, j, itemn, pos, g;

	//for (i = pos = 0; i < TEMP_N; ++i)
	{
		i = atoi(temp_N); //open the temp file N
		pos = 0;
		sprintf(buffer, "%s/%s%d.txt", dir, TEMP_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\"\n", buffer);
			return -1;
		}
		for (itemn = 0; fgets(buffer, BUFFER_SIZE, file) != NULL; ) {
			// split each buffer and save it into the temp array
			for (j = strlen(buffer) - 1; j > 0 && (buffer[j] == '\r' || buffer[j] == '\n'); --j)
				buffer[j] = 0;
			for (j = 0; buffer[j] && buffer[j] != '\t'; ++j);
			g = strlen(buffer + j + 1);
			if (!buffer[j] || g < MIN_LEN || g > MAX_LEN)
				continue;
			strcpy(temp[itemn++], buffer);
		}
		// itemn is the string num of this temp file
		fclose(file);
		// randomize the temp array into the samples array
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
	if (argc != 3) {
		fprintf(stderr, "Usage: ./get_sample_N [temp_directory_name] [temp_N]\n");
		// ./get_sample_N /tmp/data 0
		return -1;
	}
	srand((unsigned)time(NULL));
	fprintf(stdout, "*** Generating index samples ***\n");
	if (get_sample(argv[1], DISK_INDEX_SIZE, argv[2]))
		return -1;
	sprintf(buffer,"%s/%s%s", argv[1], "index_sample_", argv[2]);
	fprintf(stdout, "\n\tGenerating the index samples to \"%s\"...", buffer);
	file = fopen(buffer, "w");
	int per = DISK_INDEX_SIZE/ TEMP_N;//
	for (i = 0; i < per; ++i) {
		j = i + rand() % (per - i);
		fprintf(file, "%s\n", samples[item[j].key]);
		item[j].key = item[i].key;
	}
	fflush(file);
	fclose(file);
	fprintf(stdout, "Done.\n");
	fprintf(stdout, "*** Generating update samples ***\n");
	if (get_sample(argv[1], DISK_UPDATE_SIZE, argv[2]))
		return -1;
	sprintf(buffer,"%s/%s%s", argv[1],"update_sample_", argv[2]);
	fprintf(stdout, "\n\tGenerating the update samples to \"%s\"...", buffer);
	file = fopen(buffer, "w");
	per = DISK_UPDATE_SIZE / TEMP_N;//
	for (i = 0; i < per; ++i) {
		j = i + rand() % (per - i);
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
