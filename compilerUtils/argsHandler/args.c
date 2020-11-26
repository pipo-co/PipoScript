#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "args.h"

Args args;

static void usage(const char *progname) {
    fprintf(stderr,
        "Usage: %s [OPTIONS] source_files ... \n"
        "\n"
        "   -h                  Imprime la ayuda y termina.\n"
        "   -D                  Debug: Activar logs.\n"
        "   -o <file_name>      Archivo de saldia.  Por defecto index.html.\n"
        "\n",
        progname);
    exit(1);
}

void parse_args(const int argc, char **argv) {

    // Default values
    input_file_handler_queue_initialize(&args.inputFiles);
    args.outputFileName = "index.html";
    args.debug = false;

    int c;
    
    while (true) {

        //Los parametros que llevan argumentos son seguidos por ':'
        c = getopt(argc, argv, "hDo:");
        if (c == -1)
            break;

        switch (c) {
            case 'h':
                usage(argv[0]);
                break;
            case 'D':
                args.debug = true;
                break;
            case 'o':
                args.outputFileName = optarg;
                break;
        }
    }

    if(optind >= argc){
        fprintf(stderr, "No source files provided. \n");
        usage(argv[0]);
        exit(1);
    }

    while(optind < argc) {
        input_file_handler_enqueue(&args.inputFiles, argv[optind++]);
    }
}