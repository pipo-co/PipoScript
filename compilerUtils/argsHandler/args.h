#ifndef ARGS_H_fc51f24f6009d9dada1771be6189418224888db1
#define ARGS_H_fc51f24f6009d9dada1771be6189418224888db1

#include <stdbool.h>
#include "compilerUtils/inputFilesHandler/inputFilesHandler.h"

typedef struct Args {

    InputFileQueue inputFiles;
    char *outputFileName;
    bool debug;

} Args;

extern Args args;

void parse_args(const int argc, char **argv);

#endif