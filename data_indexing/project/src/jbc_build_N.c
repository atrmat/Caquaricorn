#include "jbc.h"

char io_buffer[IO_BUFFER_SIZE];
char buffer[BUFFER_SIZE];
FILE *file, *bbt;
Pvoid_t PJSLArray = (PWord_t)NULL;
PWord_t PValue;
Word_t Bytes;
int64_t cur_pos, cnt, *idx, i, j, itemn, rdt, opt, wrt, segt, mems, totlen, fsize;
char *keypool;
int len, btree, io_offset, temp;
struct Item *items;
struct timeval start, end, gstart;
struct Head head;
struct Head_N head_N;
struct Block block;

int
main(int argc, char *argv[]) {
	if (argc != 5) {
		fprintf(stderr, "Usage: ./jbc_build [temp_dir_name] [btree_file_name] [btree_building_time] [btreen] \n");
		// /root/cx_src/src/jbc_build_N /tmp/data/ /tmp/tmpbtree /tmp/result/jbt.txt i
		return -1;
	}
	sprintf(buffer,"%s%s",argv[3],argv[4]);
	if ((bbt = fopen(buffer, "w")) == NULL) {
		fprintf(stderr, "Failed to open file \"%s\" for recording running time\n", buffer);
		return -1;
	}
	srand((unsigned)time(NULL));
	sprintf(buffer,"%s%s",argv[2],argv[4]);
	if ((btree = open(buffer, O_CREAT | O_TRUNC | O_WRONLY, 0766)) < 0) {
		fprintf(stderr, "Failed to open file \"%s\" for writing BTree\n", buffer);
		return -1;
	}
	head_N.btreen = atoi(argv[4]);// the btree num
	i = atoi(argv[4]);
	gettimeofday(&gstart, NULL);
	lseek64(btree, HEAD_SIZE, SEEK_SET);
	//for (cur_pos = HEAD_SIZE, rdt = opt = wrt = i = 0; i < TEMP_N; ++i) 
	{
		cur_pos = HEAD_SIZE, rdt = opt = wrt = 0;
		// Reading old temp_N file
		sprintf(buffer, "%s/%s%"PRId64".txt", argv[1], TEMP_PREFIX, i);
		if ((file = fopen(buffer, "r")) == NULL) {
			fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", buffer);
			return -1;
		}
		gettimeofday(&end, NULL);
		fprintf(stdout, "\r%.2lfs: Reading old temp file %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		// get the item num of this temp_N file
		for (itemn = totlen = 0, PJSLArray = (Pvoid_t)NULL; fscanf(file, "%"PRId64"\t", &cnt) != EOF; ) {
			fgets(buffer, BUFFER_SIZE, file);
			len = strlen(buffer);
			// replace the '\r' and '\n' with '\0'
			for (--len; len >= 0 && (buffer[len] == '\r' || buffer[len] == '\n'); --len)
				buffer[len] = '\0';
			++len;
			// insert this modified buffer into the Judy array
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
		//
		if ((keypool = (char *)malloc(totlen)) == NULL) {
			fprintf(stderr, "Failed to alloc keypool for storing old temp strings\n");
			return -1;
		}
		if ((items = (struct Item *)malloc(itemn * sizeof(struct Item))) == NULL) {
			fprintf(stderr, "Failed to alloc memory for storing old temp items\n");
			return -1;
		}
		buffer[0] = '\0'; // start with the smallest string

		// find the first string in Judy array
		// then add the key&value to the items array
		JSLF(PValue, PJSLArray, (uint8_t *)buffer);
		for (j = len = 0; PValue != NULL; ++j) {
			items[j].key = len;
			items[j].value = *PValue;
			strcpy(keypool + len, buffer);
			len += strlen(buffer) + 1;
			JSLN(PValue, PJSLArray, (uint8_t *)buffer);// get the next string
		}
		JSLFA(Bytes, PJSLArray);// free the Judy array

		// Making new temp file
		gettimeofday(&end, NULL);
		fprintf(stdout, "\r%.2lfs: Making new temp file %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		if ((temp = open(TEMP_FILE_NAME_N, O_CREAT | O_TRUNC | O_WRONLY, 0666)) == -1) {
			fprintf(stderr, "Failed to open temp file \"%s\" for writing\n", TEMP_FILE_NAME_N);
			return -1;
		}
		for (io_offset = io_buffer[0] = 0; itemn > 0; ) {
			j = (rand() << 16) + rand();
			j = (j > 0 ? j % itemn : (-j) % itemn);
			if (io_offset + (len = strlen(keypool + items[j].key)) < IO_BUFFER_SIZE) {
				memcpy(io_buffer + io_offset, keypool + items[j].key, len);
				io_buffer[io_offset + len] = '\t';
				io_offset += len + 1;
				if (io_offset == IO_BUFFER_SIZE) {
					if (write(temp, io_buffer, IO_BUFFER_SIZE) != IO_BUFFER_SIZE) {
						fprintf(stderr, "Failed to write to temp file \"%s\"\n", TEMP_FILE_NAME_N);
						return -1;
					}
					io_offset = 0;
				}
			} else {
				memcpy(io_buffer + io_offset, keypool + items[j].key, IO_BUFFER_SIZE - io_offset);
				if (write(temp, io_buffer, IO_BUFFER_SIZE) != IO_BUFFER_SIZE) {
					fprintf(stderr, "Failed to write to temp file \"%s\"\n", TEMP_FILE_NAME_N);
					return -1;
				}
				memcpy(io_buffer, keypool + items[j].key + IO_BUFFER_SIZE - io_offset, len - (IO_BUFFER_SIZE - io_offset));
				io_buffer[len - (IO_BUFFER_SIZE - io_offset)] = '\t';
				io_offset = len - (IO_BUFFER_SIZE - io_offset) + 1;
			}
			io_buffer[io_offset] = 0;
			--items[j].value;
			if (items[j].value == 0)
				items[j] = items[--itemn];
		}
		if (write(temp, io_buffer, IO_BUFFER_SIZE) != IO_BUFFER_SIZE) {
			fprintf(stderr, "Failed to write to temp file \"%s\"\n", TEMP_FILE_NAME_N);
			return -1;
		}
		fdatasync(temp);
		posix_fadvise(temp, 0, 0, POSIX_FADV_DONTNEED);
		fsize = lseek64(temp, 0, SEEK_END);
		close(temp);
		free(keypool);
		free(items);

		// Reading temp data
		gettimeofday(&end, NULL);
		fprintf(stdout, "\r%.2lfs: Reading temp data %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		gettimeofday(&start, NULL);
		if ((temp = open(TEMP_FILE_NAME_N, O_RDONLY)) == -1) {
			fprintf(stderr, "Failed to open temp file \"%s\" for reading\n", TEMP_FILE_NAME_N);
			return -1;
		}
		if (read(temp, io_buffer, IO_BUFFER_SIZE) != IO_BUFFER_SIZE) {
			fprintf(stderr, "Failed to read temp file \"%s\"\n", TEMP_FILE_NAME_N);
			return -1;
		}
		for (itemn = totlen = io_offset = 0; io_buffer[io_offset]; ) {
			for (len = 0; io_buffer[io_offset] != '\t'; ) {
				buffer[len++] = io_buffer[io_offset];
				if (++io_offset == IO_BUFFER_SIZE) {
					if (read(temp, io_buffer, IO_BUFFER_SIZE) != IO_BUFFER_SIZE) {
						fprintf(stderr, "Failed to read temp file \"%s\"\n", TEMP_FILE_NAME_N);
						return -1;
					}
					io_offset = 0;
				}
			}
			if (++io_offset == IO_BUFFER_SIZE) {
				if (read(temp, io_buffer, IO_BUFFER_SIZE) != IO_BUFFER_SIZE) {
					fprintf(stderr, "Failed to read temp file \"%s\"\n", TEMP_FILE_NAME_N);
					return -1;
				}
				io_offset = 0;
			}
			buffer[len] = 0;
			++len;
			if (len < MIN_LEN || len > MAX_LEN)
				continue;
			JSLI(PValue, PJSLArray, (uint8_t *)buffer);
			if (PValue == PJERR) {
				fprintf(stderr, "Malloc failed for \"PJSLArray\"\n");
				return -1;
			}
			if (*PValue == 0) {
				itemn++;
				totlen += len + 1;
			}
			++(*PValue);
		}
		close(temp);
		gettimeofday(&end, NULL);
		segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		fprintf(bbt, "BTree %"PRId64" | %"PRId64"B | %"PRId64" items | RD %"PRId64"us | ", i, fsize, itemn, segt);
		rdt += segt;

		head_N.btreen_info.itemn = itemn;
		head_N.btreen_info.start = cur_pos;
		head_N.btreen_info.blockn = itemn / ELEM_NUM + (itemn % ELEM_NUM != 0);
		//Building BTree in memory
		gettimeofday(&end, NULL);
		fprintf(stdout, "\r%.2lfs: Building BTree %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		gettimeofday(&start, NULL);
		mems = HEAD_SIZE + BLK_SIZE + totlen + itemn * (sizeof(struct Item) + sizeof(int64_t));
		if ((keypool = (char *)malloc(totlen)) == NULL) {
			fprintf(stderr, "Failed to alloc keypool for storing old temp strings\n");
			return -1;
		}
		if ((items = (struct Item *)malloc(itemn * sizeof(struct Item))) == NULL) {
			fprintf(stderr, "Failed to alloc memory for storing old temp items\n");
			return -1;
		}
		if ((idx = (int64_t *)malloc(itemn * sizeof(int64_t))) == NULL) {
			fprintf(stderr, "Failed to alloc memory for storing new indexes\n");
			return -1;
		}
		buffer[0] = '\0';
		JSLF(PValue, PJSLArray, (uint8_t *)buffer); // find the first string in Judy array
		for (j = len = 0; PValue != NULL; ++j) {
			idx[j] = j;
			items[j].key = len;
			items[j].value = *PValue;
			strcpy(keypool + len, buffer);
			len += strlen(buffer) + 1;
			JSLN(PValue, PJSLArray, (uint8_t *)buffer); // find the next string in Judy array
		}
		JSLFA(Bytes, PJSLArray); // free the Judy array
		mems += Bytes;
		MakeNewIndex(idx, 0, 0, itemn);
		gettimeofday(&end, NULL);
		segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		fprintf(bbt, "OP %"PRId64"us | ", segt);
		opt += segt;

		// Writing BTree
		gettimeofday(&end, NULL);
		fprintf(stdout, "\r%.2lfs: Writing BTree %"PRId64"        ", gettime(&gstart), i);
		fflush(stdout);
		gettimeofday(&start, NULL);
		for (j = 0; j < itemn; ++j) {
			strcpy(block.elems[j % ELEM_NUM].key, keypool + items[idx[j]].key);
			block.elems[j % ELEM_NUM].value = items[idx[j]].value;
			block.elems[j % ELEM_NUM].child = ((j + 1) * ELEM_NUM < itemn ? cur_pos + (j + 1) * BLK_SIZE : 0);
			if (j % ELEM_NUM == ELEM_NUM - 1)
				if (write(btree, &block, BLK_SIZE) != BLK_SIZE) {
					fprintf(stderr, "Failed to write block to BTree file \"%s\"\n", argv[2]);
					return -1;
				}
		}
		if (j % ELEM_NUM)
			if (write(btree, &block, BLK_SIZE) != BLK_SIZE) {
				fprintf(stderr, "Failed to write block data to BTree file \"%s\"\n", argv[2]);
				return -1;
			}
		cur_pos += head_N.btreen_info.blockn * BLK_SIZE;
		free(keypool);
		free(items);
		free(idx);
		gettimeofday(&end, NULL);
		segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
		fprintf(bbt, "WR %"PRId64"B / %"PRId64"us | Mem %"PRId64"B |\n", (itemn / ELEM_NUM + (itemn % ELEM_NUM != 0)) * BLK_SIZE, segt, mems);
		wrt += segt;
		fflush(bbt);
	}// end of for (cur_pos = HEAD_SIZE, rdt = opt = wrt = i = 0; i < TEMP_N; ++i)

	// Writing head
	gettimeofday(&end, NULL);
	fprintf(stdout, "\n%.2lfs: Writing BTree%"PRId64" head\n", gettime(&gstart),i);
	gettimeofday(&start, NULL);
	lseek64(btree, 0, SEEK_SET);
	if (write(btree, &head_N, HEAD_SIZE) != HEAD_SIZE) {
		fprintf(stderr, "Failed to write head data to BTree file \"%s\"\n", argv[2]);
		return -1;
	}
	fdatasync(btree);
	posix_fadvise(btree, 0, 0, POSIX_FADV_DONTNEED);
	fsize = lseek64(btree, 0, SEEK_END);
	close(btree);
	gettimeofday(&end, NULL);
	segt = (int64_t)(end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
	fprintf(bbt, "Tot %"PRId64"B/%"PRId64"us | RD %"PRId64"us | OP %"PRId64"us | WR %"PRId64"us | Head %"PRId64"us |\n", fsize, rdt + opt + wrt + segt, rdt, opt, wrt, segt);
	fflush(bbt);
	fclose(bbt);
	return 0;
}
