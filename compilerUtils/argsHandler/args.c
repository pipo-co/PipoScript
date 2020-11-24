#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "args.h"

static struct Args args;

static void usage(const char *progname) {
    fprintf(stderr,
        "Usage: %s [OPTION]... [SOURCE_FILE] \n"
        "\n"
        "   -h                  Imprime la ayuda y termina.\n"
        "   -D                  Debug: Activar logs.\n"
        "   -o <file_name>      Archivo de saldia.\n"
        "\n",
        progname);
    exit(1);
}

Args *parse_args(const int argc, char **argv) {

    // Default values
    args.outputFileName = "index.html";
    args.inputFileName = NULL;
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
        fprintf(stderr, "Source file wasn't provided. \n");
        usage(argv[0]);
        exit(1);
    }

    args.inputFileName = argv[optind++];
    
    if (optind < argc) {
        fprintf(stderr, "argument not accepted: ");
        while (optind < argc) {
            fprintf(stderr, "%s ", argv[optind++]);
        }
        fprintf(stderr, "\n");
    }

    char *next = strrchr(args.inputFileName, '.');
    if(next == NULL || strcmp(next, ".pipo")){
        fprintf(stderr, "Invalid source file extension. '.pipo' files expected.\n");
        exit(1);
    }

    return &args;
}