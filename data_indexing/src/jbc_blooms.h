#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "dablooms.h"

#define CAPACITY 100000000
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