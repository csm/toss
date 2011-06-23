//
//  main.c
//  toss
//
//  Created by Casey Marshall on 6/22/11.
//  Copyright 2011 Memeo, Inc. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

struct config
{
    bool directories;
    bool verbose;
    bool force;
    enum { INTERACTIVE_ALL, INTERACTIVE_ONCE, INTERACTIVE_OFF } interactive;
    bool recursive;
};

static struct config config = { false, false, false, INTERACTIVE_OFF, false };

static void opterror(const char *progname, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "%s: ", progname);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "Try `%s --help' for more information.\n", progname);
    va_end(ap);
    exit(EXIT_FAILURE);
}

int main (int argc, const char * const argv[])
{
    const char *optstring = "dfhiIPRrvVW";
    const struct option longopts[] = {
        { "directories", no_argument, NULL, 'd' },
        { "force", no_argument, NULL, 'f' },
        { "interactive", optional_argument, NULL, 'i' },
        { "recursive", no_argument, NULL, 'r' },
        { "verbose", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'V' },
        { NULL, 0, NULL, 0 }
    };
    
    int c;
    while ((c = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch (c)
        {
            case 'd':
                config.directories = true;
                break;
                
            case 'r':
            case 'R':
                config.recursive = true;
                config.directories = true;
                break;
                
            case 'i':
                if (optarg != NULL)
                {
                    if (strcasecmp(optarg, "never") == 0)
                        config.interactive = INTERACTIVE_OFF;
                    else if (strcasecmp(optarg, "always") == 0)
                        config.interactive = INTERACTIVE_ALL;
                    else if (strcasecmp(optarg, "once") == 0)
                        config.interactive = INTERACTIVE_ONCE;
                    else
                        opterror(argv[0], "invalid argument to --interactive: %s", optarg);
                }
                else
                {
                    config.interactive = INTERACTIVE_ALL;
                }
                break;
                
            case 'r':
                config.recursive = true;
                break;
                
            case 'v':
                config.verbose = true;
                
            default:
                break;
        }
    }
    
    // insert code here...
    CFShow(CFSTR("Hello, World!\n"));
    return 0;
}

