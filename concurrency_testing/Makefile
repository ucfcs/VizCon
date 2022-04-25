# Makefile for testing files.

# Change the name of the variable below to change the test suite run.
testfile = assignment.c

# Set the flag below to 1 in order to compile Cgreen.
# Note that this cannot be run on vanilla Windows.
# To test windows.h compatibility, use MSYS2.
usecgreen = 0

# Set the flag below to "1" in order to use the memory tester.
# Note that this cannot be run on MSYS2.
usememcheck = 0

# Linux/macOS Only: If the cgreen library was not make installed to /usr/local
# due to a lack of permissions, set cgreenlocal to "1"
# and cgreenlocation to the the location for use by the linker.
cgreenlocal = 0
cgreenlocation = ~/cgreen-lib

# MSYS2 - Used for testing Windows compatibility. Only for use with tests that do not require Dr. Memory.
ifneq (, $(findstring MSYS_NT, $(shell uname -s)))
#	Run: MSYS2 does not support memory checks.
    ifeq ($(usememcheck), 1)
        run:
	        @echo "Error: MSYS2 does not support memory checking"
    else
        run: compile
	        ./a.exe
    endif

#	Compile: Link CGreen if requested.
    ifeq ($(usecgreen), 1)
        compile:
	        gcc ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c /usr/local/lib/libcgreen.dll.a
    else
        compile:
	        gcc ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c
    endif

# Non-MSYS2 Windows - Only for use with tests that do not require Cgreen.
else ifeq ($(OS), Windows_NT)
#	Windows does not support CGreen.
    ifeq ($(usecgreen), 1)
        run:
	        @echo "Error: Windows does not support CGreen"

#	Run: Use Dr. Memory if requested.
    else ifeq ($(usememcheck), 1)
        run: compile
	        drmemory -suppress suppress.in -- a.exe
    else
        run: compile
	        ./a.exe
    endif

    compile:
	    gcc ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c

# Linux
else ifeq ($(shell uname -s), Linux)
#	Run: If requested, use valgrind to run a memory check.
    ifeq ($(usememcheck), 1)
        run: compile
	        valgrind --leak-check=full --show-leak-kinds=all -s ./a.out
    else
        run: compile
	        ./a.out
    endif

#	Compile: Link CGreen if requested.
#	         If CGreen is locally installed, make sure to use its location.
    ifeq ($(usecgreen), 1)
        ifeq ($(cgreenlocal), 1)
            compile:
	            gcc -pthread ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c $(cgreenlocation)/usr/local/lib/libcgreen.so -I $(cgreenlocation)/usr/local/include/
        else
            compile:
	            gcc -pthread ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c -lcgreen
        endif
    else
        compile:
	        gcc -pthread ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c
    endif

# macOS
else ifeq ($(shell uname -s), Darwin)
#	Run: If requested, use leaks to run a memory check.
    ifeq ($(usememcheck), 1)
        run: compile
	        leaks -atExit -- ./a.out
    else
        run: compile
	        ./a.out
    endif

#	Compile: Link CGreen if requested.
#	         If CGreen is locally installed, make sure to use its location.
    ifeq ($(usecgreen), 1)
        ifeq ($(cgreenlocal), 1)
            compile:
	            gcc -pthread ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c $(cgreenlocation)/usr/local/lib/libcgreen.dylib -I $(cgreenlocation)/usr/local/include/
				install_name_tool -change @rpath/libcgreen.1.dylib $(cgreenlocation)/usr/local/lib/libcgreen.1.dylib ./a.out
        else
            compile:
	            gcc -pthread ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c -lcgreen
        endif
    else
        compile:
	        gcc -pthread ../concurrency/utils.c ../concurrency/threads.c ../concurrency/semaphores.c ../concurrency/mutexes.c ../concurrency/vcuserlibrary.c ../concurrency/lldb_lib.c ./$(testfile).c
    endif

# All Unsupported OSes
else
run :
	@echo "Error: Unsupported OS"
endif