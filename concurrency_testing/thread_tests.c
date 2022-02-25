// This is a testing file that uses the Cgreen library.
// Further doumentation can be found at https://cgreen-devs.github.io/.
// Note that using Cgreen on Windows requires building and running it with MSYS2.

#include "unit_test.h"

#define QUEUE_NAMED_TEST_SIZE 5
#define START_TEST_SIZE 5

// Mutex definition (used in "start").
#ifdef _WIN32
    HANDLE mutex;
#elif __linux__ || __APPLE__
    pthread_mutex_t* mutex;
#endif

int testVals[QUEUE_NAMED_TEST_SIZE];
int threadCount = 0;
void* startTestThread(void* param);

// Threads - The list of thread tests begins below.
Describe(Threads);

// BeforeEach - Initialize objects used by multiple tests.
BeforeEach(Threads)
{
    // Seed the random number generator.
    srand(time(NULL));
}

// queue_first - 9 assertions.
//               Ensure that the test thread was properly made.
Ensure(Threads, queue_first)
{
    // Queue a sample thread.
    testVals[0] = rand() % 25;
    vcThreadQueue(startTestThread, &testVals[0]);

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
    assert_that(queued->func, is_equal_to(startTestThread));
    assert_that(queued->arg, is_equal_to(&testVals));

    // The return value should be null since it nothing has been returned.
    assert_that(queued->returnVal, is_null);
}

// queue_second - 7 assertions.
//                Create a second thread. Ensure the name and ID are correct
//                and that the list variables are properly updated.
Ensure(Threads, queue_second)
{
    // Queue a sample thread.
    testVals[0] = rand() % 25;
    vcThreadQueue(startTestThread, &testVals[0]);

    // Queue the second thread, then get it from the list.
    testVals[1] = rand() % 25;
    vcThreadQueue(startTestThread, &testVals[1]);
    CSThread* queued = vizconThreadListHead->next;

    // The mutex should not be null.
    assert_that(queued, is_not_null);

    // The ID should be 1, and the name should be "Thread 1".
    assert_that(queued->num, is_equal_to(1));
    assert_that(queued->name, is_equal_to_string("Thread 1"));

    // The "next" for the thread should be null since there are no others.
    assert_that(queued->next, is_null);

    // Check that the function and argument are saved correctly.
    assert_that(queued->func, is_equal_to(startTestThread));
    assert_that(queued->arg, is_equal_to(&testVals[1]));

    // The list tail should be the new thread.
    assert_that(vizconThreadList, is_equal_to(queued));
}

// queue_named - 7 assertions.
//               Create a named thread. Ensure that everything works
//               like a normal vcThreadQueue (except the name) and
//               that the list variables are properly updated.
Ensure(Threads, queue_named)
{
    // Create a random string of the specified length.
    // The for loop can generate all ASCII characters except a null char.
    char str[QUEUE_NAMED_TEST_SIZE + 1];
    int i;
    for(int i = 0; i < QUEUE_NAMED_TEST_SIZE; i++)
        str[i] = rand() % 126 + 1;
    str[QUEUE_NAMED_TEST_SIZE] = '\0';

    // Queue the thread, then get it from the list.
    int testVal = rand() % 25;
    vcThreadQueueNamed(startTestThread, &testVal, str);
    CSThread* queued = vizconThreadListHead;

    // The mutex should not be null.
    assert_that(queued, is_not_null);

    // The ID should be 0, and the name should be the string.
    assert_that(queued->num, is_equal_to(0));
    assert_that(queued->name, is_equal_to_string(str));

    // The "next" for the thread should be null since there are no others.
    assert_that(queued->next, is_null);

    // Check that the function and argument are saved correctly.
    assert_that(queued->func, is_equal_to(startTestThread));
    assert_that(queued->arg, is_equal_to(&testVal));

    // The list tail should be the new thread.
    assert_that(vizconThreadList, is_equal_to(queued));
}

// startTestThread - A simple thread method used by start.
//                   Parameter: A null pointer.
//                   Add one to the global thread counter and return.
//                   Returns: A null pointer.
void* startTestThread(void* param)
{
    // Surpress the "parameter unused" compiler warning.
    void* val = param;

    // Platform-dependent mutex locking.
    #ifdef _WIN32 // Windows version
        assert_that(WaitForSingleObject(mutex, INFINITE), is_equal_to(WAIT_OBJECT_0));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_mutex_lock(mutex), is_equal_to(0));
    #endif

    // Increment the thread counter.
    threadCount++;
    
    // Platform-dependent mutex unlocking.
    #ifdef _WIN32 // Windows version
        assert_that(ReleaseMutex(mutex), is_not_equal_to(0));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_mutex_unlock(mutex), is_equal_to(0));
    #endif

    return NULL;
}

// start - Variable nuber of assertions (3 in test body, 2 in each startTestThread instance).
//         Create several threads that each increment the same value.
//         Start the threads and check that the value is correct.
Ensure(Threads, start)
{
    // Platform-dependent mutex definition.
    #ifdef _WIN32 // Windows version
        assert_that(mutex = CreateMutexA(NULL, FALSE, "Test Mutex"), is_not_null);
    #elif __linux__ || __APPLE__ // POSIX version
        mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
        assert_that(pthread_mutex_init(mutex, NULL), is_equal_to(0));
    #endif

    // Queue the necessary number of threads.
    int i;
    for(i = 0; i < START_TEST_SIZE; i++)
        vcThreadQueue(startTestThread, NULL);

    // Launch the threads.
    vcThreadStart();

    // Ensure that the value is correct.
    assert_that(threadCount, is_equal_to(START_TEST_SIZE));

    // Destroy the mutex.
    #ifdef _WIN32 // Windows version
        assert_that(CloseHandle(mutex), is_not_equal_to(0));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_mutex_destroy(mutex), is_equal_to(0));
        free(mutex);
    #endif
}

// startParamThread - A simple thread method used by start_param.
//                    Parameter: int val.
//                    Add one the testVals entry corresponding to val.
//                    Returns: A null pointer.
void* startParamThread(void* param)
{
    // Recast the parameter.
    int val = *((int*) param);

    // Increment the testVals entry.
    testVals[val]++;

    return NULL;
}

// start_param - Variable number of assertions (START_TEST_SIZE).
//               Create several threads with specific random inputs.
//               Start the threads and check that each got the input.
Ensure(Threads, start_param)
{
    // Queue a thread for each member of testVals.
    int i;
    int params[START_TEST_SIZE];
    for(i = 0; i < START_TEST_SIZE; i++)
    {
        params[i] = i;
        vcThreadQueue(startParamThread, &params[i]);
    }

    // Run the threads.
    vcThreadStart();

    // Ensure that every testVals member is exactly 1.
    for(i = 0; i < START_TEST_SIZE; i++)
        assert_that(testVals[i], is_equal_to(1));
}

// ReturnThread - A simple thread method with a return value.
//                Parameter: int val
//                Add one to the parameter and return.
//                Returns: A pointer to the parameter plus one.
void* ReturnThread(void* param)
{
    // Recast the parameter, increment, and return.
    int* val = param;
    *val += 1;
    return (void*) val;
}

// AfterEach - Close any threads still open.
//             It's okay if there was a vcThreadStart or vcThreadReturn;
//             in that case, nothing will happen.
AfterEach(Threads)
{
    freeUserThreads();
}

// End of the suite.
// Total number of assertions: 26 + 3 * START_TEST_SIZE
// Total number of exceptions: 0

// main - Initialize and run the suite.
//        Everything else will be handled in the suite itself.
int main() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Threads, queue_first);
    add_test_with_context(suite, Threads, queue_second);
    add_test_with_context(suite, Threads, queue_named);
    add_test_with_context(suite, Threads, start);
    add_test_with_context(suite, Threads, start_param);
    return run_test_suite(suite, create_text_reporter());
}