# Makefile for testing suites.
# Note that these suites cannot be run on vanilla Windows.
# To test windows.h compatibility, use MSYS2.

# Do NOT test these suites with a memory checker (e.g. valgrind) unless
# any issues with the Cgreen library and MinGW libraries are ignored.

# Change the name of the variable below to change the test suite run.
testfile = thread_tests

# macOS Only: If the cgreen library was not make installed to /usr/local due to a lack of permissions,
# set the location for use by the linker.
cgreenloc = ~/cgreen-lib

# MSYS2 - Used for testing Windows compatibility. Only for use with tests that do not require Dr. Memory.
ifneq (, $(findstring MSYS_NT, $(shell uname -s)))
run : compile
	./a.exe
compile :
	gcc ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c /usr/local/lib/libcgreen.dll.a
#	gcc ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c

# Non-MSYS2 Windows - Only for use with tests that do not require Cgreen.
else ifeq ($(OS),Windows_NT)
run : compile
#	drmemory -- a.exe
	./a.exe
compile :
	gcc ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c

# Linux
else ifeq ($(shell uname -s),Linux)
run : compile
	./a.out
#	valgrind --leak-check=full --show-leak-kinds=all -s ./a.out
compile :
	gcc -pthread ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c -lcgreen
#	gcc -pthread ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c

# macOS
else ifeq ($(shell uname -s),Darwin)
run : compile
	./a.out
#	valgrind --leak-check=full --show-leak-kinds=all -s ./a.out
compile :
	gcc -pthread ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c $(cgreenloc)/lib/libcgreen.dylib -I $(cgreenloc)/include/
	install_name_tool -change @rpath/libcgreen.1.dylib $(cgreenloc)/lib/libcgreen.1.dylib ./a.out
#	gcc -pthread ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/$(testfile).c
else
run :
	@echo "Error: Unsupported OS"
endif