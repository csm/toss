This is `toss`, a command-line interface to the trash on Mac OS X, which provides an interface compatible with `rm(1)`.

The intention here is for toss to be a drop-in replacement for `rm` that moves items to the Trash folder instead of deleting them.

It is not complete yet. You shouldn't use it or replace `/bin/rm` with it quite yet.

    Usage: toss [OPTION]... [FILE]...
    Toss the FILE(s) into the trash.
    
      -d                        remove directories as well as regular files
      -f, --force               ignore nonexistent files, never prompt
      -i                        prompt before every move
      -I                        prompt once before removing more than three files, or
                                when removing recursively. Less intrusive than -i,
                                while still giving protection against most mistakes
          --interactive[=WHEN]  prompt according to WHEN: never, once (-I), or
                                always (-i). Without WHEN, prompt always.
          --one-file-system     when removing a hierarchy recursively, skip any
                                directory that is on a file system different from
                                that of the corresponding command line argument
          --no-preserve-root    do not treat `/' specially
          --preserve-root       do not remove `/' (default)
      -r, -R, --recursive       remove directories and their contents recursively
                                (implies -d)
      -u, --unlink              unlink files instead of moving them to the trash
      -v, --verbose             explain what is being done
      -h, --help                show this help and exit
          --version             show version info and exit

Some recipies to remember:

* Move a directory to the trash, even if not empty (also, faster):

        toss -d -f PATH

* Act like `rm`:

        toss -u [options] PATH