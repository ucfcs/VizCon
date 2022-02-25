# Makefile for testing suites.
# Note that these suites cannot be run on vanilla Windows.
# To test windows.h compatibility, use MSYS2.

# Do NOT test these suites with a memory checker (e.g. valgrind) unless
# any issues with the Cgreen library are ignored.

# Change the name of the variable below to change the test suite run.
testfile = mutex_tests

# MSYS2 - Used for testing Windows compatibility.
ifneq (, $(findstring MSYS_NT, $(shell uname -s)))
run : compile
	./a.exe
compile :
	gcc ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c /usr/local/lib/libcgreen.dll.a

# Linux
else ifeq ($(shell uname -s),Linux)
run : compile
#	valgrind --leak-check=full --show-leak-kinds=all -s ./a.out
	./a.out
compile :
	gcc -pthread ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c -lcgreen

# macOS
else ifeq ($(shell uname -s),Darwin)
run : compile
	./a.out
compile :
	gcc -pthread ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c -lcgreen
else
run :
	@echo "Error: Unsupported OS"
endif