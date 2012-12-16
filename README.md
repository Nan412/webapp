webapp
======

a tool for building web applications

### To build: ###

Prerequisites (Unix only):

    * GNU Make 3.81 or newer
    * GCC

Linux:

``` bash
make PLATFORM=linux
```

MacOS X:

``` bash
make PLATFORM=macosx
```

POSIX:

``` bash
make PLATFORM=posix
```

### To test: ###

Run the following command to see if it works.

``` bash
./bin/webapp
```

### Link globally for development ###

``` bash
# Symbolically link to /usr/local/bin.
make link

# Remove the link.
make unlink
```
