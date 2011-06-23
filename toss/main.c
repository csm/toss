//
//  main.c
//  toss
//
//  Created by Casey Marshall on 6/22/11.
//  Copyright 2011 Memeo, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if DEBUG
#include "commit.inc"
#else
#define TOSS_VERSION "0.0.1"
#endif

struct config
{
    bool directories;
    bool verbose;
    bool force;
    enum { INTERACTIVE_ALL, INTERACTIVE_ONCE, INTERACTIVE_OFF } interactive;
    bool recursive;
    bool no_preserve_root;
    bool one_file_system;
};

static struct config config = { false, false, false, INTERACTIVE_OFF, false, false, false };

static void opterror(const char *progname, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if (fmt != NULL)
    {
        fprintf(stderr, "%s: ", progname);
        vfprintf(stderr, fmt, ap);
    }
    fprintf(stderr, "Try `%s --help' for more information.\n", progname);
    va_end(ap);
    exit(EXIT_FAILURE);
}

static void showhelp(const char *progname)
{
    printf("Usage: %s [OPTION]... [FILE]...\n", progname);
    printf("Toss the FILE(s) into the trash.\n\n");
    printf("  -d                       remove directories as well as regular files\n");
    printf("  -f, --force              ignore nonexistent files, never prompt\n");
    printf("  -i                       prompt before every move\n");
    printf("  -I                       prompt once before removing more than three files, or\n");
    printf("                           when removing recursively. Less intrusive than -i,\n");
    printf("                           while still giving protection against most mistakes\n");
    printf("     --interactive[=WHEN]  prompt according to WHEN: never, once (-I), or\n");
    printf("                           always (-i). Without WHEN, prompt always.\n");
    printf("     --one-file-system     when removing a hierarchy recursively, skip any\n");
    printf("                           directory that is on a file system different from\n");
    printf("                           that of the corresponding command line argument\n");
    printf("     --no-preserve-root    do not treat `/' specially\n");
    printf("     --preserve-root       do not remove `/' (default)\n");
    printf("  -r, -R, --recursive      remove directories and their contents recursively\n");
    printf("                           (implies -d)\n");
    printf("  -v, --verbose            explain what is being done\n");
    printf("  -h, --help               show this help and exit\n");
    printf("      --version            show version info and exit\n");
}

static void showversion(const char *progname)
{
    printf("%s version %s\n\n", progname, TOSS_VERSION);
}

static bool prompt(const char *progname, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf("%s: ", progname);
    vprintf(fmt, ap);
    putchar('\n');
    char *line = NULL;
    size_t linelen;
    if (getline(&line, &linelen, stdin) > 0)
    {
        if (strcasecmp("y", line) == 0 || strcasecmp("yes", line) == 0)
        {
            return true;
        }
    }
    return false;
}

#define OPT_ONE_FILE_SYSTEM  1024
#define OPT_PRESERVE_ROOT    1025
#define OPT_NO_PRESERVE_ROOT 1026
#define OPT_VERSION          1027

int main (int argc, char * const *argv)
{
    const char *progname = "toss";
    const char *optstring = "dfhiIPRrvVW";
    const struct option longopts[] = {
        { "directories", no_argument, NULL, 'd' },
        { "force", no_argument, NULL, 'f' },
        { "interactive", optional_argument, NULL, 'i' },
        { "recursive", no_argument, NULL, 'r' },
        { "verbose", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, OPT_VERSION },
        { "one-file-system", no_argument, NULL, OPT_ONE_FILE_SYSTEM },
        { "no-preserve-root", no_argument, NULL, OPT_NO_PRESERVE_ROOT },
        { "preserve-root", no_argument, NULL, OPT_PRESERVE_ROOT },
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
                        opterror(progname, "invalid argument to --interactive: %s", optarg);
                }
                else
                {
                    config.interactive = INTERACTIVE_ALL;
                }
                break;
                
            case 'v':
                config.verbose = true;
                
            case 'h':
                showhelp(progname);
                exit(EXIT_SUCCESS);
                
            case OPT_VERSION:
                showversion(progname);
                exit(EXIT_SUCCESS);
                
            case OPT_NO_PRESERVE_ROOT:
                config.no_preserve_root = true;
                break;
                
            case OPT_PRESERVE_ROOT:
                config.no_preserve_root = false;
                break;
                
            case OPT_ONE_FILE_SYSTEM:
                config.one_file_system = true;
                break;
                
            case '?':
                opterror(progname, NULL);
                
            default:
                break;
        }
    }
    
    if (optind >= argc)
    {
        opterror(progname, "missing argument\n");
    }
    
    if ((argc - optind > 3 || config.recursive) && config.interactive == INTERACTIVE_ONCE)
    {
        if (!prompt(progname, "remove all arguments?"))
        {
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = optind; i < argc; i++)
    {
        FSRef fsref;
        Boolean isDir;
        OSStatus err = FSPathMakeRef((const UInt8 *) argv[i], &fsref, &isDir);
        if (err == fnfErr && !config.force)
        {
            fprintf(stderr, "%s: %s: no such file or directory\n", progname, argv[i]);
            continue;
        }
        
        if (isDir)
        {
            
        }
        else
        {
            bool move = true;
            if (config.interactive == INTERACTIVE_ALL)
            {
                move = prompt(progname, "remove regular file %s?", argv[i]);
            }
            
            if (move)
            {
                err = FSMoveObjectToTrashSync(&fsref, NULL, 0);
                if (err == noErr)
                {
                    if (config.verbose)
                        printf("%s\n", argv[i]);
                }
                else
                {
                    switch (err)
                    {
                        case permErr:
                            fprintf(stderr, "%s: cannot move %s: permission denied\n", progname, argv[i]);
                            break;
                            
                        default:
                            fprintf(stderr, "%s: cannot move %s: OSStatus %d\n", progname, argv[i], err);
                            break;
                    }
                }
            }
        }
    }
    
    exit(EXIT_SUCCESS);
}

