BASE = .
INCLUDE=$(BASE)/include
INCLUDE_JIT=$(INCLUDE)/luajit-2.0

all: build

update: clean build

build:
	@@mkdir -p $(BASE)/build/
	gcc -Wall -o $(BASE)/build/lude $(BASE)/src/lude.c -L$(BASE)/lib -llua -lm -ldl -DNATIVE

jit:
	@@mkdir -p $(BASE)/build/
	gcc -Wall -o $(BASE)/build/lude $(BASE)/src/lude.c -L$(BASE)/lib -lluajit-5.1 -lev -Wl,-rpath $(BASE)/lib -DJIT

clean:
	@@rm -rf $(BASE)/build/

.PHONY: build
