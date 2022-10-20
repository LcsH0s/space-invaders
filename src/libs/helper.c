#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "helper.h"

void show_help()
{
    printf("usage: huffmaninator [-cx] [-s \"string\"] [-f filepath]\n");
    printf("Mode :\n"
           "    -c                     compresser\n"
           "    -x                     extraire\n"
           "\nInput :\n"
           "    -f [filepath]          utiliser le fichier de configuration du dépôt\n"
           "    -s \"[string]\"        utiliser un fichier de configuration par arbre de travail\n");
}

struct args arg_parse(int argc, char **argv)
{
    int c;
    struct args args = {
        COMPRESS_MODE,
        0,
        NULL,
        NULL,
        DEFAULT_DEST};

    if (argc == 1)
    {
        printf("usage: huffmaninator [-cx] [-s \"string\"] [-f filepath] [-o output_file] -> Use -h for help\n");
        exit(0);
    }

    while ((c = getopt(argc, argv, "cxhf:s:o:")) != -1)
    {
        switch (c)
        {
        case 'x':
            args.mode = EXTRACT_MODE;
            break;
        case 'c':
            args.mode = COMPRESS_MODE;
            break;
        case 'f':
            args.input_mode = FILE_MODE;
            args.filepath = optarg;
            break;
        case 'o':
            args.dest = optarg;
            break;
        case 's':
            args.input_mode = STR_MODE;
            args.str = optarg;
            break;
        case 'h':
            show_help();
            exit(0);
        default:
            abort();
        }
    }
    if (args.input_mode == 0)
    {
        printf("No input specified. Use -h for help.\n");
        printf("usage: huffmaninator [-cx] [-s \"string\"] [-f filepath]\n");
        exit(0);
    }
    if ((args.mode == EXTRACT_MODE) & ((args.input_mode == STR_MODE) | (args.filepath == NULL)))
    {
        printf("Extract mode only supports file input. Please specify an input file\n");
        exit(1);
    }
    return args;
}

struct input get_input(struct args args)
{
    struct input r;
    if (args.input_mode == STR_MODE)
    {
        r.len = strlen(args.str);
        r.str = malloc(sizeof(char) * r.len);
        strcpy(r.str, args.str);
        return r;
    }
    else if (access(args.filepath, F_OK) == 0)
    {
        FILE *input_file = fopen(args.filepath, "rb");
        if (input_file)
        {
            fseek(input_file, 0, SEEK_END);
            r.len = ftell(input_file);
            fseek(input_file, 0, SEEK_SET);
            r.str = malloc(r.len);
            if (r.str)
            {
                fread(r.str, 1, r.len, input_file);
            }
            fclose(input_file);
            return r;
        }
    }
    else
    {
        printf("ERROR: Unable to access file: %s\n", args.filepath);
        exit(-1);
    }
}

FILE *file_init(struct args args)
{
    FILE *fp;
    if (args.mode == COMPRESS_MODE)
    {
        fp = fopen(args.dest, "wb");

        if (fp == NULL)
        {
            printf("\n The file '%s' could not be opened", args.dest);
            exit(1);
        }
        // CODE
        fputc('t', fp);
        // END CODE
        if (ferror(fp))
        {
            printf("\n Error writing in file : %s", args.dest);
            fclose(fp);
            exit(1);
        }
        else
        {
            fclose(fp);
            fp = fopen(args.dest, "wb");

            if (fp == NULL)
            {
                printf("\n The file '%s' could not be opened", args.dest);
                exit(1);
            }
        }
    }
    else if (args.mode == EXTRACT_MODE)
    {
        fp = fopen(args.dest, "rb");

        if (fp == NULL)
        {
            printf("\n The file '%s' could not be opened", args.dest);
            exit(1);
        }
        // CODE
        fgetc(fp);
        // END CODE
        if (ferror(fp))
        {
            printf("\n Error reading file : %s", args.dest);
            fclose(fp);
            exit(1);
        }
        else
        {
            fclose(fp);
            fp = fopen(args.dest, "rb");

            if (fp == NULL)
            {
                printf("\n The file '%s' could not be opened", args.dest);
                exit(1);
            }
        }
    }
    return fp;
}