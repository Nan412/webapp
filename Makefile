BASE = .

all: build

update: clean build

build:
	@@mkdir -p $(BASE)/build/
	gcc -Wall -o $(BASE)/build/lude $(BASE)/src/lude.c -L$(BASE)/lib -llua -lm -ldl

clean:
	@@rm -rf $(BASE)/build/

.PHONY: build
