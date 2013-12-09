#include "jbc.h"

char buffer[BUFFER_SIZE];
char keypool[13 * 1024 * 1024];

struct Item tmpitems[450000], items[450000];

struct Head ohead, nhead;
struct Head_N ohead_N, nhead_N;
struct Block block;
Pvoid_t PJSLArray[TEMP_N];
Pvoid_t PJSLArray_N;
PWord_t PValue;
Word_t Bytes;
int64_t nindex[450000], tmpindex[450000];
FILE *file;
int64_t cur_pos, cnt, on, odn, i, j, k, mems, rdt, opt, wrt, segt, totlen;
int len, obtree, nbtree, cmp_res;
struct timeval start, end, gstart;

int
main(int argc, char *argv[]) {
	if (argc != 6) {
		fprintf(stderr, "Usage: ./jbc_update_N [operations_file_name] [old_btree_file] [new_btree_file] [update_time] [temp_n]\n");
		// /root/cx_src/src/jbc_update_N /tmp/data/update_sample.txt /tmp/btree.bin /tmp/modified_btree.bin /tmp/result/jut.txt i
		return -1;
	}
	gettimeofday(&gstart, NULL);
	fprintf(stdout, "%.2lfs: Reading operation files start\n", gettime(&gstart));
	fflush(stdout);
	if ((file = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Failed to open the operations file \"%s\"\n", argv[1]);
		return -1;
	}
	/*
	for (i = 0; i < TEMP_N; ++i)
		PJSLArray[i] = (PWord_t)NULL;
	*/
	for (on = odn = 0; fscanf(file, "%"PRId64"\t", &cnt) != EOF; ++on) {
		fgets(buffer, BUFFER_SIZE, file);
		len = strlen(buffer);
		for (--len; len >= 0 && (buffer[len] == '\r' || buffer[len] == '\n'); --len)
			buffer[len] = '\0';
		++len;
		//JSLI(PValue, PJSLArray[BKDRHash((uint8_t *)buffer)], (uint8_t *)buffer);
		JSLI(PValue, PJSLArray_N, (uint8_t *)buffer);
		if (PValue == NULL) {
			fprintf(stderr, "Failed to alloc memory for storing operations\n");
			return -1;
		}
		if (*PValue == 0)
			++odn;
		*PValue += cnt;
		if ((on + 1) % 1000 == 0) {
			fprintf(stdout, "\r%.2lfs: Got %"PRId64" operations", gettime(&gstart), on + 1);
			fflush(stdout);
		}
	}
	fclose(file);
	fprintf(stdout, "\r%.2lfs: Reading operation files done, got %"PRId64" / %"PRId64"\n", gettime(&gstart), odn, on);
	
	sprintf(buffer,"%s_%s",argv[4],argv[5]);
	if ((file = fopen(buffer, "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for recording running time\n", buffer);
		return -1;
	}
	if ((obtree = open(argv[2], O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open file \"%s\" for reading old BTree\n", argv[2]);
		return -1;
	}
	if (read(obtree, &ohead_N, HEAD_SIZE) != HEAD_SIZE) {
		fprintf(stderr, "Failed to read old BTree file \"%s\"\n", argv[2]);
		return -1;
	}
	/*if (ohead.btreen != TEMP_N) {
		fprintf(stderr, "Error number of sub BTrees\n");
		return -1;
	}*/
	gettimeofday(&start, NULL);
	sprintf(buffer,"%s%s",argv[3],argv[5]);
	if ((nbtree = open(buffer, O_CREAT | O_TRUNC | O_WRONLY, 0766)) < 0) {
		fprintf(stderr, "Failed to open file \"%s\" for writing new BTree\n", buffer);
		return -1;
	}
	gettimeofday(&end, NULL);
	rdt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	nhead_N.btreen = TEMP_N;
	lseek64(nbtree, HEAD_SIZE, SEEK_SET);
	//for (cur_pos = HEAD_SIZE, opt = wrt = i = 0; i < TEMP_N; ++i) 
	cur_pos = HEAD_SIZE, opt = wrt = 0;
	{
		// Reading old btree information
		fprintf(stdout, "\r%.2lfs: Reading BTree %"PRId64" ", gettime(&gstart), i);
		fflush(stdout);
		gettimeofday(&start, NULL);
		lseek64(obtree, ohead_N.btreen_info.start, SEEK_SET);
		for (j = totlen = 0; j < ohead_N.btreen_info.itemn; j += ELEM_NUM) {
			if (read(obtree, &block, BLK_SIZE) != BLK_SIZE) {
				fprintf(stderr, "Failed to read old BTree file \"%s\"\n", argv[2]);
				return -1;
			}
			for (k = 0; k < ELEM_NUM; ++k) {
				tmpitems[j + k].value = block.elems[k].value;
				strcpy(keypool + totlen, block.elems[k].key);
				tmpitems[j + k].key = totlen;
				totlen += strlen(block.elems[k].key) + 1;
			}
		}
		gettimeofday(&end, NULL);
		segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		fprintf(file, "| BTree %"PRId64" | RD %"PRId64" items %"PRId64"B %"PRId64"us ", i, ohead_N.btreen_info.itemn, ohead_N.btreen_info.blockn * BLK_SIZE, segt);
		rdt += segt;

		// Updating operations
		fprintf(stdout, "\r%.2lfs: Building BTree %"PRId64" ", gettime(&gstart), i);
		fflush(stdout);
		gettimeofday(&start, NULL);
		MakeNewIndex(tmpindex, 0, 0, ohead_N.btreen_info.itemn);
		for (j = 0; j < ohead_N.btreen_info.itemn; ++j)
			nindex[tmpindex[j]] = j;
		buffer[0] = '\0';
		//JSLF(PValue, PJSLArray[i], (uint8_t *)buffer);
		JSLF(PValue, PJSLArray_N, (uint8_t *)buffer);
		for (j = k = 0, on = 0; j < ohead_N.btreen_info.itemn || PValue != NULL;) {
			if (PValue == NULL)
				items[k++] = tmpitems[nindex[j++]];
			else if (j == ohead_N.btreen_info.itemn) {
				if (*PValue > 0) {
					items[k].value = *PValue;
					strcpy(keypool + totlen, buffer);
					items[k++].key = totlen;
					totlen += strlen(buffer) + 1;
				}
				JSLN(PValue, PJSLArray_N, (uint8_t *)buffer);
				//JSLN(PValue, PJSLArray[i], (uint8_t *)buffer);
				++on;
			} else if ((cmp_res = strcmp(keypool + tmpitems[nindex[j]].key, buffer)) == 0) {
				if (*PValue != 0 && *PValue + tmpitems[nindex[j]].value != 0) {
					items[k].key = tmpitems[nindex[j]].key;
					items[k++].value = tmpitems[nindex[j]].value + *PValue;
				}
				++j;
				JSLN(PValue, PJSLArray_N, (uint8_t *)buffer);
				//JSLN(PValue, PJSLArray[i], (uint8_t *)buffer);
				++on;
			} else if (cmp_res > 0) {
				if (*PValue > 0) {
					items[k].value = *PValue;
					strcpy(keypool + totlen, buffer);
					items[k++].key = totlen;
					totlen += strlen(buffer) + 1;
				}
				JSLN(PValue, PJSLArray_N, (uint8_t *)buffer);
				//JSLN(PValue, PJSLArray[i], (uint8_t *)buffer);
				++on;
			} else
				items[k++] = tmpitems[nindex[j++]];
		}
		JSLFA(Bytes, PJSLArray_N);
		//JSLFA(Bytes, PJSLArray[i]);
		gettimeofday(&end, NULL);
		segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		fprintf(file, "| OP %"PRId64" items got %"PRId64" items %"PRId64"us ", on, k, segt);
		opt += segt;

		// Writing new BTree information
		fprintf(stdout, "\r%.2lfs: Writing BTree %"PRId64" ", gettime(&gstart), i);
		fflush(stdout);
		gettimeofday(&start, NULL);
		nhead_N.btreen_info.start = cur_pos;
		nhead_N.btreen_info.itemn = k;
		nhead_N.btreen_info.blockn = k / ELEM_NUM + (k % ELEM_NUM != 0);
		MakeNewIndex(nindex, 0, 0, k);
		for (j = 0; j < k; ++j) {
			strcpy(block.elems[j % ELEM_NUM].key, keypool + items[nindex[j]].key);
			block.elems[j % ELEM_NUM].value = items[nindex[j]].value;
			block.elems[j % ELEM_NUM].child = ((j + 1) * ELEM_NUM < k ? cur_pos + (j + 1) * BLK_SIZE : 0);
			if (j % ELEM_NUM == ELEM_NUM - 1)
				if (write(nbtree, &block, BLK_SIZE) != BLK_SIZE) {
					fprintf(stderr, "Failed to write block to new BTree file \"%s\"\n", argv[3]);
					return -1;
				}
		}
		if (j % ELEM_NUM)
			if (write(nbtree, &block, BLK_SIZE) != BLK_SIZE) {
				fprintf(stderr, "Failed to write block data to new BTree file \"%s\"\n", argv[3]);
				return -1;
			}
		cur_pos += nhead_N.btreen_info.blockn * BLK_SIZE;
		gettimeofday(&end, NULL);
		segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		mems = totlen + MAX(k, ohead_N.btreen_info.itemn) * sizeof(int64_t) + MAX(k, 2 * ohead_N.btreen_info.itemn) * sizeof(struct Item);
		fprintf(file, "| WR %"PRId64"B / %"PRId64"us | Mem %"PRId64"B+ %luB |\n", nhead_N.btreen_info.blockn * BLK_SIZE, segt, mems, Bytes);
		wrt += segt;
	}

	fprintf(stdout, "\n%.2lfs: Writing BTree head\n", gettime(&gstart));
	gettimeofday(&start, NULL);
	lseek64(nbtree, 0, SEEK_SET);
	if (write(nbtree, &nhead_N, HEAD_SIZE) != HEAD_SIZE) {
		fprintf(stderr, "Failed to write new head data to BTree file \"%s\"\n", argv[3]);
		return -1;
	}
	fdatasync(nbtree);
	posix_fadvise(nbtree, 0, 0, POSIX_FADV_DONTNEED);
	gettimeofday(&end, NULL);
	segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	fprintf(file, "Total %"PRId64"B -> %"PRId64"B in %"PRId64"us | RD %"PRId64"us | OP %"PRId64"us | WR %"PRId64"us | HEAD %"PRId64"us\n", lseek64(obtree, 0, SEEK_END), lseek64(nbtree, 0, SEEK_END), rdt + opt + wrt + segt, rdt, opt, wrt, segt);
	close(nbtree);
	close(obtree);
	fclose(file);
	return 0;
}
