webapp
======

a tool for building web applications

## Build from source ##

### MacOS/Linux/Unix ###

#### Prerequisites ####

* GNU Make
* gcc
* Git

#### MacOS / Linux ####

``` bash
make
```

#### Other ####

*defaults to posix*

``` bash
make PLATFORM=other
```

#### Test ####

Run the following command to see if it works.

``` bash
./bin/webapp
```

#### Link globally for development ####

``` bash
# Symbolically link to /usr/local/bin.
make link

# Remove the link.
make unlink
```
