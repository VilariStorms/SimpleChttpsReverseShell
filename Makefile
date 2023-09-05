# Simple makefile for main.c that also sets up a python virtual enc
cc = gcc
cflags = -Wall -Wextra -Werror -pedantic -std=gnu99
ldflags = -lcurl
BIN=venv/bin/

all: help
	@echo Run \'make build\' to compile and build the executable
	@echo Run \'make help\' to see all available commands
help:
	@echo "'make everything' compiles and builds executable, creates virtual environment, installs python dependencies and runs the python shell in the foreground and the main executable in the background"
	@echo "'make build' compiles and builds executable, creates virtual environment, installs python dependencies but does not run anything"
	@echo "'make client' runs the main executable which is the client. It will run with the arg post"
	@echo "'make clean' removes the executable and virtual environment"
	@echo "'make help' prints this help message"
	@echo "'make compile' compiles and builds executable but does not create virtual environment or install python dependencies"
everything: build run
build:
	@$(cc) $(cflags) -o main main.c $(ldflags)
	@echo compiled and built executable
	@python3 -m venv venv
	@echo created virtual environment
	$(BIN)pip install -r requirements.txt
	@echo installed python dependencies
	@echo done
compile:
	@$(cc) $(cflags) -o main main.c $(ldflags)
	@echo compiled and built executable
run:
	./main post & $(BIN)python3 shell.py
	@echo done

shell:
	$(BIN)python3 shell.py
client:
	./main post
clean:
	@rm -f main
	@echo removed executable
	@rm -rf venv
	@echo removed virtual environment
	@echo cleaned up


