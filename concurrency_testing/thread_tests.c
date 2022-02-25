// This is a testing file that uses the Cgreen library.
// Further doumentation can be found at https://cgreen-devs.github.io/.
// Note that using Cgreen on Windows requires building and running it with MSYS2.

#include "unit_test.h"

#define CREATE_NAMED_TEST_SIZE 5

int testVal;

// SimpleThread - A simple thread method.
//                Parameter: int val
//                Add one to the parameter and return.
//                Returns: A pointer to the parameter plus one.
void* SimpleThread(void* param)
{
    // Recast the parameter, increment, and return.
    int* val = param;
    *val += 1;
    return (void*) val;
}

// Threads - The list of thread tests begins below.
Describe(Threads);

// BeforeEach - Initialize objects used by multiple tests.
BeforeEach(Threads)
{
    // Seed the random number generator.
    srand(time(NULL));

    // Queue a sample thread.
    testVal = rand() % 25;
    vcThreadQueue(SimpleThread, &testVal);
}

// create_first - 9 assertions.
//                Ensure that the test thread was properly made.
Ensure(Threads, create_first)
{
    // Get the queued thread.
    CSThread* queued = vizconThreadListHead;

    // The thread should not be null.
    assert_that(queued, is_not_null);

    // The "next" property should be null since there are no others.
    assert_that(queued->next, is_null);

    // The thread tail should be the same as the saved head.
    assert_that(vizconThreadList, is_equal_to(queued));

    // The thread name should be "Thread 0" and the num should be 0.
    assert_that(queued->name, is_equal_to_string("Thread 0"));
    assert_that(queued->num, is_equal_to(0));

    // Platform-dependent thread property test.
    // A Windows thread is made but suspended, while
    // a Linux thread is not built until it is started.
    #ifdef _WIN32 // Windows version
        assert_that(queued->thread, is_not_null);
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(queued->thread, is_null);    
    #endif
    
    // Check that the function and argument are saved correctly.
    assert_that(queued->func, is_equal_to(SimpleThread));
    assert_that(queued->arg, is_equal_to(&testVal));

    // The return value should be null since it nothing has been returned.
    assert_that(queued->returnVal, is_null);
}

// create_second - 7 assertions.
//                 Create a second thread. Ensure the name and ID are correct
//                 and that the list variables are properly updated.
Ensure(Threads, create_second)
{
    // Queue the thread, then get it from the list.
    int testVal2 = rand() % 25;
    vcThreadQueue(SimpleThread, &testVal2);
    CSThread* queued = vizconThreadListHead->next;

    // The mutex should not be null.
    assert_that(queued, is_not_null);

    // The ID should be 1, and the name should be "Thread 1".
    assert_that(queued->num, is_equal_to(1));
    assert_that(queued->name, is_equal_to_string("Thread 1"));

    // The "next" for the thread should be null since there are no others.
    assert_that(queued->next, is_null);

    // Check that the function and argument are saved correctly.
    assert_that(queued->func, is_equal_to(SimpleThread));
    assert_that(queued->arg, is_equal_to(&testVal2));

    // The list tail should be the new thread.
    assert_that(vizconThreadList, is_equal_to(queued));
}

// create_named - 7 assertions.
//                Create a named thread. Ensure that everything works
//                like a normal vcThreadQueue (except the name) and
//                that the list variables are properly updated.
Ensure(Threads, create_named)
{
    // Create a random string of the specified length.
    // The for loop can generate all ASCII characters except a null char.
    char str[CREATE_NAMED_TEST_SIZE + 1];
    int i;
    for(int i = 0; i < CREATE_NAMED_TEST_SIZE; i++)
        str[i] = rand() % 126 + 1;
    str[CREATE_NAMED_TEST_SIZE] = '\0';

    // Queue the thread, then get it from the list.
    int testVal2 = rand() % 25;
    vcThreadQueueNamed(SimpleThread, &testVal2, str);
    CSThread* queued = vizconThreadListHead->next;

    // The mutex should not be null.
    assert_that(queued, is_not_null);

    // The ID should be 1, and the name should be "Thread 1".
    assert_that(queued->num, is_equal_to(1));
    assert_that(queued->name, is_equal_to_string(str));

    // The "next" for the thread should be null since there are no others.
    assert_that(queued->next, is_null);

    // Check that the function and argument are saved correctly.
    assert_that(queued->func, is_equal_to(SimpleThread));
    assert_that(queued->arg, is_equal_to(&testVal2));

    // The list tail should be the new thread.
    assert_that(vizconThreadList, is_equal_to(queued));
}

// AfterEach - Close any threads still open.
//             It's okay if there was a vcThreadStart or vcThreadReturn;
//             in that case, nothing will happen.
AfterEach(Threads)
{
    freeUserThreads();
}

// End of the suite.
// Total number of assertions: 23
// Total number of exceptions: 0

// main - Initialize and run the suite.
//        Everything else will be handled in the suite itself.
int main() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Threads, create_first);
    add_test_with_context(suite, Threads, create_second);
    add_test_with_context(suite, Threads, create_named);
    return run_test_suite(suite, create_text_reporter());
}