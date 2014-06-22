#include "as.h"

char buffer[BUFFER_SIZE], key[KEY_MAX_LENGTH + 1], value[WORD_MAX_LENGTH + 1];
int64_t i, j, k, cnt, kcur, vcur, kpos, vpos, totv, totuv, totk, idx[1530000];
int64_t space[50], vcnt[AS_NUMBER];
int tf, vf;
Pvoid_t PJSLKey[BCTN], PJSLValue;
PWord_t PJWCount;
Word_t Bytes;
FILE *file;
struct timeval start;
struct Key_Head head;
struct Key_Block kblock;
struct Value_Block vblock;
struct Item items[1530000];

int
check() {
	int64_t i, tabn, ft;

	for (i = space[0] = tabn = 0, ft = -1; buffer[i]; ++i)
		if (buffer[i] == '\t') {
			++tabn;
			if (ft == -1)
				ft = i;
		} else if (buffer[i] == ' ')
			space[++space[0]] = i;
	while (i > 0 && (buffer[i - 1] == '\r' || buffer[i - 1] == '\n'))
		buffer[--i] = 0;
	if (tabn == 4 && space[0] == 2) {
		if (space[1] > 0 && space[1] <= WORD_MAX_LENGTH && space[2] - space[1] - 1 > 0 && space[2] - space[1] - 1 <= WORD_MAX_LENGTH && ft - space[2] - 1 > 0 && ft - space[2] - 1 <= WORD_MAX_LENGTH) {
			space[++space[0]] = ft;
			return 0;
		} else
			return -1;
	} else
		return -1;
}

int
add(unsigned hash, int64_t cnt) {
	JSLI(PJSLValue, PJSLKey[hash], (uint8_t *)key);
	if (PJSLValue == PJERR) {
		fprintf(stderr, "Malloc Failed for \"PJSLKey[%u]\"\n", hash);
		return -1;
	}
	if (*((PWord_t *)PJSLValue) == 0)
		*((PWord_t *)PJSLValue) = (PWord_t)NULL;
	JSLI(PJWCount, *((PPvoid_t)PJSLValue), (uint8_t *)value);
	if (PJWCount == PJERR) {
		fprintf(stderr, "Malloc Failed for \"PJSLValue\"\n");
		return -1;
	}
	*PJWCount += cnt;
	return 0;
}

int
merge(int64_t f, int64_t l, int64_t r, int64_t cnt) {
	int64_t kl;
	unsigned hash;

	for (kl = 0; f + kl < l; ++kl)
		key[kl] = buffer[f + kl];
	key[kl] = 0;
	if (kl) {
		hash = BKDRHash(key);
		strncpy(value, buffer + l, r - l);
		value[r - l] = 0;
		if (add(hash, cnt))
			return -1;
	}
	for (; f + kl < r;) {
		key[kl] = buffer[f + kl];
		key[++kl] = 0;
		hash = BKDRHash(key);
		strncpy(value, buffer + f + kl, r - f - kl);
		if (r - f - kl)
			value[r - f - kl] = 0;
		else {
			value[0] = ' ';
			value[1] = 0;
		}
		if (add(hash, cnt))
			return -1;
	}
	return 0;
}

int
main(int argc, char *argv[]) {
	if (argc != 5) {
		fprintf(stderr, "Usage: ./as_build [data_dir] [temp_dir] [tree_file] [value_file]\n");
		return -1;
	}
	
	if (access(argv[2], F_OK)) {
		fprintf(stdout, "Directory \"%s\" does not exist, create it\n", argv[2]);
		if (mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
			fprintf(stderr, "Fail to create directory \"%s\"\n", argv[2]);
			return -1;
		}
	} else
		fprintf(stdout, "Directory \"%s\" already existed\n", argv[2]);
	gettimeofday(&start, NULL);
	for (i = 0; i < ZFN; ++i) {
		fprintf(stdout, "\r%.6lfs: Process Data %03"PRId64"---unzip...", gettime(&start), i);
		fflush(stdout);
		sprintf(buffer, "unzip %s/%s%"PRId64".csv.zip -d %s 1>/dev/null", argv[1], DATA_PREFIX, i, argv[2]);
		if (system(buffer) == -1) {
			fprintf(stderr, "Failed to unzip with command \"%s\"\n", buffer);
			return -1;
		}

		fprintf(stdout, "\r%.6lfs: Process Data %03"PRId64"---unzip---read...", gettime(&start), i);
		fflush(stdout);
		sprintf(buffer, "%s/%s%"PRId64".csv", argv[2], DATA_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to read source file \"%s\"\n", buffer);
			return -1;
		}
		for (j = 0; j < BCTN; ++j)
			PJSLKey[i] = (PWord_t)NULL;
		while (fgets(buffer, BUFFER_SIZE, file) != (char *)NULL) {
			if (!check()) {
				sscanf(buffer + space[space[0]] + 1, "%"PRId64"\t%"PRId64"", &k, &cnt);
				buffer[space[3]] = ' ';
				if (merge(0, 0, space[1], cnt))
					return -1;
				if (merge(space[1] + 1, space[1] + 1, space[2], cnt))
					return -1;
				if (merge(space[2] + 1, space[2] + 1, space[3], cnt))
					return -1;
				if (merge(0, space[1] + 1, space[2], cnt))
					return -1;
				if (merge(space[1] + 1, space[2] + 1, space[3], cnt))
					return -1;
				if (merge(0, space[2] + 1, space[3], cnt))
					return -1;
			}
		}
		fclose(file);
		sprintf(buffer, "%s/%s%"PRId64".csv", argv[2], DATA_PREFIX, i);
		remove(buffer);

		fprintf(stdout, "\r%.6lfs: Process Data %03"PRId64"---unzip---read---write...", gettime(&start), i);
		fflush(stdout);
		for (j = 0; j < BCTN; ++j) {
			sprintf(buffer, "%s/%s%"PRId64".txt", argv[2], TEMP_PREFIX, j);
			if ((file = fopen(buffer, "a")) == NULL) {
				fprintf(stderr, "Failed to append temporal file \"%s\"\n", buffer);
				return -1;
			}
			key[0] = 0;
			JSLF(PJSLValue, PJSLKey[j], (uint8_t *)key);
			while (PJSLValue != NULL) {
				value[0] = 0;
				JSLF(PJWCount, *((PPvoid_t)PJSLValue), (uint8_t *)value);
				while (PJWCount != NULL) {
					fprintf(file, "%"PRId64"\t%s\t%s\n", *PJWCount, key, value);
					JSLN(PJWCount, *((PPvoid_t)PJSLValue), (uint8_t *)value);
				}
				JSLFA(Bytes, *((PPvoid_t)PJSLValue));
				JSLN(PJSLValue, PJSLKey[j], (uint8_t *)key);
			}
			JSLFA(Bytes, PJSLKey[j]);
			fflush(file);
			fclose(file);
		}
		fprintf(stdout, "\r%.6lfs: Process Data %03"PRId64"---unzip---read---write---Done\n", gettime(&start), i);
	}

	if ((tf = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0766)) < 0) {
		fprintf(stderr, "Failed to write tree file \"%s\"\n", argv[3]);
		return -1;
	}
	if ((vf = open(argv[4], O_CREAT | O_TRUNC | O_WRONLY, 0766)) < 0) {
		fprintf(stderr, "Failed to write value file \"%s\"\n", argv[4]);
		return -1;
	}
	head.bctn = BCTN;
	lseek64(tf, HEAD_SIZE, SEEK_SET);
	memset(&kblock, 0, KEY_BLK_SIZE);
	lseek64(vf, 0, SEEK_SET);
	memset(&vblock, 0, VALUE_BLK_SIZE);
	for (i = totk = totv = totuv = vpos = vcur = 0, kcur = HEAD_SIZE; i < BCTN; ++i) {
		fprintf(stdout, "\r%.6lfs: Process Temp %04"PRId64"", gettime(&start), i);
		fflush(stdout);
		sprintf(buffer, "%s/%s%"PRId64".txt", argv[2], TEMP_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to read temporal file \"%s\"\n", buffer);
			return -1;
		}
		PJSLKey[i] = (PWord_t)NULL;
		while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
			sscanf(buffer, "%"PRId64"\t", &cnt);
			for (j = 0; buffer[j] != '\t'; ++j);
			for (k = ++j; buffer[k] != '\t'; ++k);
			strncpy(key, buffer + j, k - j);
			key[k - j] = 0;
			for (j = k + 1, k = strlen(buffer) - 1; k >= j && (buffer[k] == '\r' || buffer[k] == '\n'); --k)
				buffer[k] = 0;
			++k;
			strncpy(value, buffer + j, k - j);
			value[k - j] = 0;
			add((unsigned)i, cnt);
		}
		fclose(file);

		key[0] = 0;
		head.bctinfo[i].itemn = 0;
		JSLF(PJSLValue, PJSLKey[i], (uint8_t *)key);
		for (; PJSLValue != NULL;) {
			value[0] = 0;
			JSLF(PJWCount, *((PPvoid_t)PJSLValue), (uint8_t *)value);
			memset(vcnt, 0, sizeof(vcnt));
			while (PJWCount != NULL) {
				++totv;
				for (j = 0; j < AS_NUMBER && vblock.elem[vpos].value[j][0]; ++j)
					if (*PJWCount > vcnt[j] || (*PJWCount == vcnt[j] && strcmp(value, vblock.elem[vpos].value[j]) < 0))
						break;
				if (j < AS_NUMBER) {
					for (k = AS_NUMBER - 1; k > j; --k) {
						vcnt[k] = vcnt[k - 1];
						memcpy(vblock.elem[vpos].value[k], vblock.elem[vpos].value[k - 1], WORD_MAX_LENGTH + 1);
					}
					vcnt[j] = *PJWCount;
					memset(vblock.elem[vpos].value[j], 0, sizeof(vblock.elem[vpos].value[j]));
					strcpy(vblock.elem[vpos].value[j], value);
				}
				JSLN(PJWCount, *((PPvoid_t)PJSLValue), (uint8_t *)value);
			}
			JSLFA(Bytes, *((PPvoid_t)PJSLValue));
			for (j = 0; j < AS_NUMBER && vblock.elem[vpos].value[j][0]; ++j);
			totuv += j;
			strcpy(items[head.bctinfo[i].itemn].key, key);
			items[head.bctinfo[i].itemn].value = vcur + vpos * VALUE_ELEM_SIZE;
			idx[head.bctinfo[i].itemn] = head.bctinfo[i].itemn;
			++head.bctinfo[i].itemn;
			if (++vpos == VALUE_ELEM_NUM) {
				if (write(vf, &vblock, VALUE_BLK_SIZE) != VALUE_BLK_SIZE) {
					fprintf(stderr, "Failed to write value data to value file \"%s\"\n", argv[4]);
					return -1;
				}
				vpos = 0;
				vcur += VALUE_BLK_SIZE;
				memset(&vblock, 0, VALUE_BLK_SIZE);
			}
			JSLN(PJSLValue, PJSLKey[i], (uint8_t *)key);
		}
		JSLFA(Bytes, PJSLKey[i]);
		
		totk += head.bctinfo[i].itemn;
		head.bctinfo[i].start = lseek64(tf, 0, SEEK_CUR);
		head.bctinfo[i].blockn = head.bctinfo[i].itemn / KEY_ELEM_NUM + (head.bctinfo[i].itemn % KEY_ELEM_NUM != 0);
		MakeNewIndex(idx, 0, 0, head.bctinfo[i].itemn);
		for (j = kpos = 0; j < head.bctinfo[i].itemn; ++j) {
			strcpy(kblock.elems[kpos].key, items[idx[j]].key);
			kblock.elems[kpos].value = items[idx[j]].value;
			kblock.elems[kpos].child = ((j + 1) * KEY_ELEM_NUM < head.bctinfo[i].itemn ? kcur + (j + 1) * KEY_BLK_SIZE : -1);
			if (++kpos == KEY_ELEM_NUM) {
				if (write(tf, &kblock, KEY_BLK_SIZE) != KEY_BLK_SIZE) {
					fprintf(stderr, "Failed to write key block to tree file \"%s\"\n", argv[3]);
					return -1;
				}
				kpos = 0;
				memset(&kblock, 0, KEY_BLK_SIZE);
			}
		}
		if (kpos) {
			if (write(tf, &kblock, KEY_BLK_SIZE) != KEY_BLK_SIZE) {
				fprintf(stderr, "Failed to write key block to tree file \"%s\"\n", argv[3]);
				return -1;
			}
			kpos = 0;
			memset(&kblock, 0, KEY_BLK_SIZE);
		}
		kcur += head.bctinfo[i].blockn * KEY_BLK_SIZE;
	}
	if (vpos)
		if (write(vf, &vblock, VALUE_BLK_SIZE) != VALUE_BLK_SIZE) {
			fprintf(stderr, "Failed to write value data to value file \"%s\"\n", argv[4]);
			return -1;
		}
	fdatasync(vf);
	close(vf);
	lseek64(tf, 0, SEEK_SET);
	if (write(tf, &head, HEAD_SIZE) != HEAD_SIZE) {
		fprintf(stderr, "Failed to write head data to tree file \"%s\"\n", argv[3]);
		return -1;
	}
	fdatasync(tf);
	close(tf);
	fprintf(stdout, "\n%.6lfs: There are %"PRId64" keys and %"PRId64"/%"PRId64" values\n", gettime(&start), totk, totuv, totv);
	return 0;
}
