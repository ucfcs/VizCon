// This is a testing file that uses the Cgreen library.
// Further doumentation can be found at https://cgreen-devs.github.io/.
// Note that using Cgreen on Windows requires building and running it with MSYS2.

#include "unit_test.h"

int testVal;

// SimpleThread - A simple thread method.
THREAD_RET SimpleThread(THREAD_PARAM param)
{
    // Just return the parameter.
    return param;
}

// Threads - The list of thread tests begins below.
Describe(Threads);

// BeforeEach - Initialize objects used by multiple tests.
BeforeEach(Threads)
{
    // Queue a sample thread.
    testVal = 7;
    vcThreadQueue(SimpleThread, &testVal);
    srand(time(NULL));
}

// create_first - Variable number of assertions (4 for Windows, 6 for POSIX).
//                Ensure that the test thread was properly made.
Ensure(Threads, create_first)
{
    // Get the queued thread.
    CSThread* queued = vizconThreadListHead;

    // The thread should not be null.
    assert_that(queued, is_not_null);

    // The "next" property should be null since there are no others.
    assert_that(queued->next, is_null);

    // The thread list's tail should both be testMutex.
    assert_that(vizconThreadList, is_equal_to(queued));

    // FIXME: The thread name should be "Thread 1".

    // Platform-dependent thread property tests.
    #ifdef _WIN32 // Windows version
        // A Windows thread is made but suspended.
        assert_that(queued->thread, is_not_null);

    #elif __linux__ || __APPLE__ // POSIX version
		// A Linux thread is not built until it is started.
        assert_that(queued->thread, is_null);
        assert_that(queued->func, is_equal_to(SimpleThread));
        assert_that(queued->arg, is_equal_to(&testVal));
    
    #endif
}

// AfterEach - Close any threads still open.
//             It's okay if there was a vcThreadStart or vcThreadReturn;
//             in that case, nothing will happen.
AfterEach(Threads)
{
    freeUserThreads();
}

// main - Initialize and run the suite.
//        Everything else will be handled in the suite itself.
int main() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Threads, create_first);
    return run_test_suite(suite, create_text_reporter());
}