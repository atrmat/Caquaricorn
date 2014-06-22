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
		fprintf(stderr, "Usage: ./prepare [file_prefix] [number_of_files] [out_dir_name] [item_#_file] [length_#_file] [count_dist_file]\n");
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

	if (access(argv[3], F_OK)) {
		fprintf(stdout, "Directory \"%s\" does not exist, create it\n", argv[3]);
		if (mkdir(argv[3], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
			fprintf(stderr, "Failed to create directory \"%s\"\n", argv[3]);
			return -1;
		}
	} else
		fprintf(stdout, "Directory \"%s\" already existed\n", argv[3]);
	for (i = 0; i < TEMP_N; ++i) {
		sprintf(buffer, "%s/%s%d.txt", argv[3], TEMP_PREFIX, i);
		if ((tsf[i] = fopen(buffer, "w")) == NULL) {
			fprintf(stderr, "Failed to open file \"%s\" for writing temp strings\n", buffer);
			for (j = 0; j < i; ++j)
				fclose(tsf[j]);
			return -1;
		}
	}
	if ((itf = fopen(argv[4], "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for writing item numbers\n", argv[4]);
		return -1;
	}
	fn = atoi(argv[2]);
	for (fi = tot = itemn = Index[0] = 0; fi < fn; ++fi) {
		// use "unzip" command to prepare csv input file
		sprintf(buffer, "unzip %s%d.csv.zip 1>/dev/null", argv[1], fi);
		if (system(buffer) == -1) {
			fprintf(stderr, "Failed to execute command: \"%s\"\n", buffer);
			return -1;
		}
		sprintf(buffer, "%s%d.csv", argv[1], fi);
		if ((inf = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to read file \"%s\"\n", argv[1]);
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
		sprintf(buffer, "rm %s%d.csv", argv[1], fi);
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

	// fixing each temp file
	fprintf(stdout, "Start fixing temp files\n");
	for (i = 0; i < TEMP_N; ++i) {
		sprintf(buffer, "%s/%s%d.txt", argv[3], TEMP_PREFIX, i);
		fprintf(stdout, "\rWorking on temp file: \"%s\"", buffer);
		fflush(stdout);
		if ((tsf[i] = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to open file \"%s\" for reading temp strings\n", buffer);
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
				return -1;
			}
			(*PValNgramS) += itemn;
			JLI(PValTotC, PJLTotCount, Len);
			if (PValTotC == PJERR) {
				fprintf(stderr, "Malloc failed for \"PJLTotCount\"\n");
				return -1;
			}
			*PValTotC += itemn;
			if (*PValNgramS == itemn) {
				JLI(PValNgramC, PJLNgramCount, Len);
				if (PValNgramC == PJERR) {
					fprintf(stderr, "Malloc failed for \"PJLNgramCount\"\n");
					return -1;
				}
				++*PValNgramC;
			}

		}
		fclose(tsf[i]);
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
		fprintf(itf, "Temp file \"%s/%s%d\" uses %lu Bytes of memory\n", argv[3], TEMP_PREFIX, i, Bytes);
		fflush(itf);
	}
	fclose(itf);

	if ((lef = fopen(argv[5], "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for writing length number\n", argv[5]);
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
	if ((cdf = fopen(argv[6], "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for writing count distribuction\n", argv[6]);
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
