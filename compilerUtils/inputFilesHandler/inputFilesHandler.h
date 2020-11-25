#ifndef INPUT_FILES_HANDLER_H_fc51f24f6009d9dada1771be6189418224888db1
#define INPUT_FILES_HANDLER_H_fc51f24f6009d9dada1771be6189418224888db1

#include <stdio.h>

typedef struct InputFileNode {

    char *filename;
    struct InputFileNode *next;
} InputFileNode;

typedef struct InputFileQueue {

    struct InputFileNode *first;
    struct InputFileNode *last;
    FILE *currentFile;
    char *currentFilename;

} InputFileQueue;

void input_file_handler_queue_initialize(InputFileQueue *q);

void input_file_handler_enqueue(InputFileQueue *q, char *filename);

FILE * input_file_handler_dequeue(InputFileQueue *q);

void input_file_handler_finalize(InputFileQueue *q);

#endif