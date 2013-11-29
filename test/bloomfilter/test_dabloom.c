#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "dablooms.h"

#define CAPACITY 100000
#define ERROR_RATE .05

enum {
    TEST_PASS,
    TEST_WARN,
    TEST_FAIL,
};

struct stats {
    int true_positives;
    int true_negatives;
    int false_positives;
    int false_negatives;
};

static void chomp_line(char *word)
{
    char *p;
    if ((p = strchr(word, '\r'))) {
        *p = '\0';
    }
    if ((p = strchr(word, '\n'))) {
        *p = '\0';
    }
}

int main()
{
	counting_bloom_t *bloom;
	char word[256];
	char *words_file = "test_words";
	char *bloom_file = "test_dabloom.bin";
	FILE *fp;
	int i;
	int result = 0;
	if (!(bloom = new_counting_bloom(CAPACITY, ERROR_RATE, bloom_file))) {
        	fprintf(stderr, "ERROR: Could not create bloom filter: %s \n",bloom_file);
        	return TEST_FAIL;
    	}
	if (!(fp = fopen(words_file, "r"))) {
		fprintf(stderr, "ERROR: Could not open words file\n");
		return TEST_FAIL;
	}
	fprintf(stderr, "adding all items into the bloom filter\n");
	for (i = 0; fgets(word, sizeof(word), fp) && (i < CAPACITY); i++) {
		chomp_line(word);
		fprintf(stderr, "add the item %s \n",word);
		counting_bloom_add(bloom, word, strlen(word));
	}
	fseek(fp, 0, SEEK_SET);
	for (i = 0; fgets(word, sizeof(word), fp) && (i < CAPACITY); i++) {
		if (i % 5 == 0) {
			chomp_line(word);
			fprintf(stderr, "delete the n/5 item %s \n",word);
			counting_bloom_remove(bloom, word, strlen(word));
		}
	}
	fseek(fp, 0, SEEK_SET);
	fprintf(stderr, "check all items in the bloom filter\n");
	for (i = 0; fgets(word, sizeof(word), fp) && (i < CAPACITY); i++) {
		chomp_line(word);
		result = counting_bloom_check(bloom, word, strlen(word));
		if (!result)
			fprintf(stderr, "the item %s no exist!\n",word);
	}
	free_counting_bloom(bloom);
	fclose(fp);	
	return TEST_PASS;
}
