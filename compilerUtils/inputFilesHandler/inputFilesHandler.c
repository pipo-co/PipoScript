#include "compilerUtils/inputFilesHandler/inputFilesHandler.h"

#include "compilerUtils/pipoUtils/pipoUtils.h"
#include <string.h>

void input_file_handler_queue_initialize(InputFileQueue *q) {
    q->currentFile = NULL;
    q->currentFilename = NULL;
    q->first = NULL;
    q->last = NULL;
}

void input_file_handler_enqueue(InputFileQueue *q, char *filename) {

    InputFileNode *node = emalloc(sizeof(*node));

    node->filename = filename;
    node->next = NULL;

    if(q->first == NULL)
        q->first = node;

    else
        q->last->next = node;

    q->last = node;
}

FILE * input_file_handler_dequeue(InputFileQueue *q) {

    InputFileNode *aux;
    char *extension;

    if(q->currentFile != NULL) {
        fclose(q->currentFile);
        q->currentFile = NULL;
    }

    while(q->first != NULL) {

        q->currentFilename = q->first->filename;

        aux = q->first;
        q->first = q->first->next;
        free(aux);

        // Enforce .pipo extension
        extension = strrchr(q->currentFilename, '.');

        if(extension == NULL || strcmp(extension, ".pipo")) {
            fprintf(stderr, "Invalid source file extension of file %s. '.pipo' extension is mandatory.\n", q->currentFilename);
            continue;
        }

        q->currentFile = fopen(q->currentFilename, "r");

        if(q->currentFile != NULL)
            return q->currentFile;

        perror("Could not open input file");
        fprintf(stderr, "File %s will be ignored", q->currentFilename);
    }

    q->last = NULL;

    return NULL;
}

void input_file_handler_finalize(InputFileQueue *q) {

    if(q->currentFile != NULL) {
        fclose(q->currentFile);
        q->currentFile = NULL;
    }

    InputFileNode *aux;

    while(q->first != NULL) {

        aux = q->first;
        q->first = q->first->next;
        free(aux);
    }
}