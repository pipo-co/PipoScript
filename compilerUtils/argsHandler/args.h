#ifndef ARGS_H_fc51f24f6009d9dada1771be6189418224888db1
#define ARGS_H_fc51f24f6009d9dada1771be6189418224888db1

#include <stdbool.h>

typedef struct Args {

    char *inputFileName;
    char *outputFileName;
    bool debug;

} Args;

Args *parse_args(const int argc, char **argv);

#endif