#include "jbc.h"

#define FLIMIT (TEMP_N + 20)

char Index[BUFFER_SIZE];
char buffer[BUFFER_SIZE];
Pvoid_t   PJSLNgram = (PWord_t)NULL;
PWord_t   PValNgramS, PValTotC, PValNgramC, PValCountC;
Pvoid_t   PJLTotCount = (PWord_t)NULL, PJLNgramCount = (PWord_t)NULL, PJLCountCount = (PWord_t)NULL;
Word_t    Bytes, Total, NgramN, Count, Len;
FILE      *inf, *tsf[TEMP_N], *itf, *lef, *cdf;
int64_t tot, itemn;
int fi, fn, i, j;
struct rlimit rlim;


int
main(int argc, char *argv[]) {
	if (argc != 7) {
		fprintf(stderr, "Usage: ./prepare [file_prefix] [from_file_num] [number_of_files] [out_dir_name] [item_#_file] [temp_prefix_n]\n");
		// /root/cx_src/src/prepare_test /tmp/test/googlebooks-eng-all-5gram-20090715- 0 256 /tmp/data /tmp/result/ino.txt 0
		return -1;
	}

	// check and set maximun number of newly-created descriptors
	getrlimit(RLIMIT_NOFILE, &rlim);
	if (rlim.rlim_cur < FLIMIT) {
		fprintf(stderr, "Maximum number of newly-created descriptors \"%"PRId64"\" is not enough\n", rlim.rlim_cur);
		rlim.rlim_cur = rlim.rlim_max = FLIMIT;
		if (setrlimit(RLIMIT_NOFILE, &rlim)) {
			fprintf(stderr, "Failed to set the maximum number of newly-created descriptors\n");
			return -1;
		}
	}

	if (access(argv[4], F_OK)) {
		fprintf(stdout, "Directory \"%s\" does not exist, create it\n", argv[4]);
		if (mkdir(argv[4], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
			fprintf(stderr, "Failed to create directory \"%s\"\n", argv[4]);
			return -1;
		}
	} else
		fprintf(stdout, "Directory \"%s\" already existed\n", argv[4]);

	int temp_prefix_num = atoi(argv[6]);// num of concurrent threads
	for (i = 0; i < TEMP_N; ++i) {
		sprintf(buffer, "%s/%s%d-%d.txt", argv[4], TEMP_PREFIX, i, temp_prefix_num);
		if ((tsf[i] = fopen(buffer, "w")) == NULL) {
			fprintf(stderr, "Failed to open file \"%s\" for writing temp strings\n", buffer);
			for (j = 0; j < i; ++j)
				fclose(tsf[j]);
			return -1;
		}
	}
	if ((itf = fopen(argv[5], "a")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for writing item numbers\n", argv[5]);
		return -1;
	}
	
	
	fi = atoi(argv[2]);
	fn = fi + atoi(argv[3]);
	for (tot = itemn = Index[0] = 0; fi < fn; ++fi) {
	//for (fi = tot = itemn = Index[0] = 0; fi < fn; ++fi) {
		// use "unzip" command to prepare csv input file
		sprintf(buffer, "unzip %s%d.csv.zip -d /tmp/temp_csv_files/ 1>/dev/null", argv[1], fi);
		if (system(buffer) == -1) {
			fprintf(stderr, "Failed to execute command: \"%s\"\n", buffer);
			return -1;
		}
		sprintf(buffer, "/tmp/temp_csv_files/%s%d.csv", "googlebooks-eng-all-5gram-20090715-", fi);
		if ((inf = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to read file \"%s%d\"\n", "googlebooks-eng-all-5gram-20090715-", fi);
			return -1;
		}

		while (fgets(buffer, BUFFER_SIZE, inf) != (char *)NULL) {
			// data line format: "ngram TAB year TAB match_count TAB volume_count NEWLINE"
			// take the first TAB as the end of the ngram, and count the number of TAB
			for (Len = 0; buffer[Len] && buffer[Len] != '\t'; Len++)
				Index[Len] = buffer[Len];
			Index[Len] = 0;
			for (i = Len, j = 0; buffer[i]; j += (buffer[i++] == '\t'));
			if (j != 4 || Len <= 1)
				continue;
			sscanf(buffer + Len + 1, "%d\t%d", &i, &j);

			JSLI(PValNgramS, PJSLNgram, (uint8_t *)Index);
			if (PValNgramS == PJERR) {
				fprintf(stderr, "Malloc failed for \"PJSLNgram\"\n");
				return -1;
			}
			*PValNgramS += j;
			itemn += (*PValNgramS == j);
			tot += j;
			if (itemn % 200000 == 0) {
				fprintf(stdout, "\r%"PRId64" in %"PRId64" of %d", itemn, tot, fi);
				fflush(stdout);
			}
		}
		fclose(inf);

		// remove the unziped csv file
		sprintf(buffer, "rm /tmp/temp_csv_files/%s%d.csv", "googlebooks-eng-all-5gram-20090715-", fi);
		if (system(buffer) == -1) {
			fprintf(stderr, "Failed to execute command: \"%s\"\n", buffer);
			return -1;
		}
		fprintf(itf, "%"PRId64" in %"PRId64" of %d\n", itemn, tot, fi);
		fflush(itf);
		Index[0] = '\0';
		JSLF(PValNgramS, PJSLNgram, (uint8_t *)Index);
		while (PValNgramS != NULL) {
			fprintf(tsf[BKDRHash((uint8_t *)Index)], "%lu\t%s\n", *PValNgramS, Index);
			JSLN(PValNgramS, PJSLNgram, (uint8_t *)Index);
		}
		JSLFA(Bytes, PJSLNgram);
	}

	for (i = 0; i < TEMP_N; ++i) {
		fflush(tsf[i]);
		fclose(tsf[i]);
	}
	fprintf(stdout, "\r%"PRId64" in %"PRId64" of %d\n", itemn, tot, fi);
	return 0;
}
