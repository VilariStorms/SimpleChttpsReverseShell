cc = gcc
cflags = -Wall -Wextra -Werror -pedantic -std=gnu99
ldflags = -lcurl
BIN=venv/bin/

all: build
build:
	@$(cc) $(cflags) -o main main.c $(ldflags)
	@echo compiled and built executable
	@python3 -m venv venv
	@echo created virtual environment
	$(BIN)pip install -r requirements.txt
	@echo installed python dependencies
	@echo run ./main [get\|post] to start the client
	@echo "run 'make shell' or 'source venv/bin/activate && python3 shell.py' to start the shell"
	@echo run make clean to clean up
	@echo done!
compile:
	@$(cc) $(cflags) -o main main.c $(ldflags)
	@echo compiled and built executable
	@echo run ./main [get\|post] to start the client
	@echo "run 'make shell' or 'source venv/bin/activate && python3 shell.py' to start the shell"
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


