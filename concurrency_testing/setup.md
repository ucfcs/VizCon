Tests for this program can be broadly divided into two categories: unit tests and standalone tests.

# Unit Tests

Unit tests are in C files not located in the main folder. 

## Installing CGreen

Running these tests requires installing [CGreen](https://github.com/cgreen-devs/cgreen/releases). This can be finicky, especially without administrative permissions.<br/>

Also, make sure that you are using the latest release, because there are issues building from the current main branch.

### Linux / macOS

If you are on Linux or macOS, you will need [CMake](https://cmake.org/download/) and the GNU C++ compiler (aka g++). Then, if you are an administrator, you can install CGreen by running the following three commands in the CGreen root directory:

> make<br/>
> make test<br/>
> sudo make install

If you do not have administrative privileges, then the process is more complicated.<br/>

You will need to select a location for the library, install it there, and then modify the Makefile so it properly adds the library. I used "~/cgreen-lib". This can be done with the following commands:

> make<br/>
> make test<br/>
> make DESTDIR=\<destination\> install

Once this completes, change the value in the Makefile of "cgreenlocal" to 1 and "cgreenlocation" to the destination. On Linux (but not MacOS), you must also add this folder to the LD_LIBRARY_PATH:

> LD_LIBRARY_PATH=$LD_LIBRARY_PATH:\<destination\>/usr/local/<br/>
> export LD_LIBRARY_PATH

### Windows

If you are on Windows, you cannot install CGreen directly. Instead, download [MSYS2](https://www.msys2.org), then use it to run the following in the CGreen root directory:

> pacman -Syu<br/>
> pacman -S base-devel gcc cmake<br/>
> make unit<br/>
> make install

Note that the "make test" may report an error in the XML formatter. You can ignore this.

## Running

Before running these tests, make sure that "usecgreen" is set to 1 and "usememcheck" is set to 0.<br/>

Then, change the value of "testfile" to the appropriate test file name (e.g. "mutex_tests"). Then, use the following command:

> make

Note that this must be done in MSYS2 is on Windows. Don't worry; the windows.h library is still accessed.<br/>

Each file has an expected number of passes and exceptions, which you can find at the bottom of the file. Make sure to check that even if make returns an error.<br/>

Note that if you're having an issue with a specific test, you can replace the "Ensure" statement of every other test with an "xEnsure" to only run that one.

# Standalone Tests

Standalone tests are located in a subfolder of concurrency_testing. They are short, non-unit test programs with varying uses.<br/>

Note that the tests in the memory subfolder, while they run on their own, are intended for use with a memory checker. I recommend [Valgrind](https://valgrind.org) on Linux, [Leaks](https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/FindingLeaks.html) on macOS, and [Dr. Memory](https://drmemory.org) on Windows. There are prebuilt statements for these checkers in the makefile.

## Installing Valgrind

If you have administrative privileges, most versions of Linux will allow you to install Valgrind via the package manager. The exact command will vary depending on what package manager you are using.

## Installing Leaks

Leaks is included with Xcode. You should already have this for GCC.

## Installing Dr. Memory

If you have administrative privileges, download and install the MSI installer.<br/>

If you do not have administrative privileges, download the Dr. Memory ZIP, extract it to a specific location, and add that location to your PATH.<br/>

Note that, if run normally, some of the results with Dr. Memory will be false positives due to issues with MinGW. I have added a suppression file to cut down on false positives, but I make no guarantees. As a general rule, if you see a "__mingw" in a reported stack trace, you can ignore it.

## Running

Before running these tests, make sure that "usecgreen" is set to 0 and "usememcheck" is set to 1.<br/>

Additionally, change the value of "testfile" to the appropriate test path and file name (e.g. "memory/memory_test"). Then, use the following command:

> make

Note that, if Dr. Memory is involved, this must be done in vanilla Windows, not MSYS2.