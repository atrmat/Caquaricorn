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
	if (argc != 8) {
		fprintf(stderr, "Usage: ./prepare [out_dir_name 1] [item_#_file 2] [length_#_file 3] [count_dist_file 4] [number_of_temp_prefix 5] [from_temp_n] [to_temp_n]\n");
		// /root/cx_src/src/prepare_test2 /tmp/data /tmp/result/ino.txt /tmp/result/lno.txt /tmp/result/cdo.txt 3 0 1
		return -1;
	}
	if ((itf = fopen(argv[2], "a")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for writing item numbers\n", argv[2]);
		return -1;
	}
	// fixing each temp file
	fprintf(stdout, "Start fixing temp files\n");
	for (i = atoi(argv[6]); i <= atoi(argv[7]); ++i) {
	//for (i = 0; i < TEMP_N; ++i) {
		// fix the temp file i
		int temp_prefix_num = 0;// num of concurrent threads
		while (temp_prefix_num < atoi(argv[5])){
			sprintf(buffer, "%s/%s%d-%d.txt", argv[1], TEMP_PREFIX, i, temp_prefix_num);
			fprintf(stdout, "\rWorking on temp file: \"%s\" \n", buffer);
			//fflush(stdout);
			if ((tsf[i] = fopen(buffer, "r")) == NULL) {
				fprintf(stderr, "Failed to open file \"%s\" for reading temp strings\n", buffer);
				//continue;
				return -1;
			}
			while (fscanf(tsf[i], "%"PRId64"\t", &itemn) != EOF) {
				fgets(Index, BUFFER_SIZE, tsf[i]);
				for (Len = strlen(Index) - 1; Index[Len] == '\n' || Index[Len] == '\r'; Len--)
					Index[Len] = 0;
				++Len;
				JSLI(PValNgramS, PJSLNgram, (uint8_t *)Index);
				if (PValNgramS == PJERR) {
					fprintf(stderr, "Malloc failed for \"PJSLNgram\"\n");
					//return -1;
				}
				(*PValNgramS) += itemn;
				JLI(PValTotC, PJLTotCount, Len);
				if (PValTotC == PJERR) {
					fprintf(stderr, "Malloc failed for \"PJLTotCount\"\n");
					//return -1;
				}
				*PValTotC += itemn;
				if (*PValNgramS == itemn) {
					JLI(PValNgramC, PJLNgramCount, Len);
					if (PValNgramC == PJERR) {
						fprintf(stderr, "Malloc failed for \"PJLNgramCount\"\n");
						//return -1;
					}
					++*PValNgramC;
				}
			}
			sprintf(buffer, "rm %s/%s%d-%d.txt", argv[1], TEMP_PREFIX, i, temp_prefix_num++);
			if (system(buffer) == -1) {
					fprintf(stderr, "Failed to execute command: \"%s\"\n", buffer);
					//return -1;
			}
			fclose(tsf[i]);
		}

		// write the final temp file
		sprintf(buffer, "%s/%s%d.txt", argv[1], TEMP_PREFIX, i);
		if ((tsf[i] = fopen(buffer, "w")) == NULL) {
			fprintf(stderr, "Failed to open file \"%s\" for writing temp strings\n", buffer);
			return -1;
		}
		Index[0] = '\0';
		JSLF(PValNgramS, PJSLNgram, (uint8_t *)Index);
		while (PValNgramS != NULL) {
			fprintf(tsf[i], "%lu\t%s\n", *PValNgramS, Index);
			Count = *PValNgramS;
			JLI(PValCountC, PJLCountCount, Count);
			if (PValCountC == PJERR) {
				fprintf(stderr, "Malloc failed for \"PJLCountCount\"\n");
				return -1;
			}
			++*PValCountC;
			JSLN(PValNgramS, PJSLNgram, (uint8_t *)Index);
		}
		JSLFA(Bytes, PJSLNgram);
		fflush(tsf[i]);
		fclose(tsf[i]);
		fprintf(itf, "Temp file \"%s/%s%d\" uses %lu Bytes of memory\n", argv[1], TEMP_PREFIX, i, Bytes);
		fflush(itf);
	}
	fclose(itf);

	if ((lef = fopen(argv[3], "a")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for writing length number\n", argv[3]);
		return -1;
	}
	Total = NgramN = 0;
	JLF(PValTotC, PJLTotCount, Total);
	JLF(PValNgramC, PJLNgramCount, NgramN);
	while (PValTotC != NULL) {
		fprintf(lef, "%lu\t%lu\t%lu\n", Total, *PValNgramC, *PValTotC);
		JLN(PValTotC, PJLTotCount, Total);
		JLN(PValNgramC, PJLNgramCount, NgramN);
	}
	JLFA(Bytes, PJLTotCount);
	JLFA(Bytes, PJLNgramCount);
	fflush(lef);
	fclose(lef);
	if ((cdf = fopen(argv[4], "a")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for writing count distribuction\n", argv[4]);
		return -1;
	}
	Count = 0;
	JLF(PValCountC, PJLCountCount, Count);
	while (PValCountC != NULL) {
		fprintf(cdf, "%lu\t%lu\n", Count, *PValCountC);
		JLN(PValCountC, PJLCountCount, Count);
	}
	JLFA(Bytes, PJLCountCount);
	fflush(cdf);
	fclose(cdf);

	return 0;
}
