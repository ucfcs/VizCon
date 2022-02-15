// This is a testing file that uses the Cgreen library.
// Further doumentation can be found at https://cgreen-devs.github.io/.
// Note that using Cgreen on Windows requires building and running it with MSYS2.

#include "unit_test.h"

#define ISOLATION_TEST_SIZE 5

vcMutex *testMutex;

// Mutexes - The list of mutex tests begins below.
Describe(Mutexes);

// BeforeEach - Initialize objects used by multiple tests.
BeforeEach(Mutexes)
{
    testMutex = vcMutexCreate("Test");
    srand(time(NULL));
}

// create_first - 8 assertions.
//                Ensure that the test mutex was properly made.
Ensure(Mutexes, create_first)
{
    // The mutex should not be null.
    assert_that(testMutex, is_not_null);

    // The "next" property should be null since there are no others.
    assert_that(testMutex->next, is_null);

    // The contained "mutex" struct should not be null.
    assert_that(testMutex->mutex, is_not_null);

    // The struct should have a mutex object, be available (true),
    // and have a holderID of 0.
    assert_that(testMutex->mutex->mutex, is_not_null);
    assert_that(testMutex->mutex->available, is_true);
    assert_that(testMutex->mutex->holderID, is_equal_to(0));

    // The mutexListHead and mutexListTail values should both be testMutex.
    assert_that(vizconMutexListHead, is_equal_to(testMutex));
    assert_that(vizconMutexListTail, is_equal_to(testMutex));
}

// create_second - 6 assertions.
//                 Create a second mutex and ensure it was properly made.
Ensure(Mutexes, create_second)
{
    // Create the mutex.
    vcMutex* testMutex2 = vcMutexCreate("Test2");

    // The mutex should not be null.
    assert_that(testMutex2, is_not_null);

    // The "next" for testMutex should be testMutex2.
    // The "next" for testMutex2 should be null since there are no others.
    assert_that(testMutex->next, is_equal_to(testMutex2));
    assert_that(testMutex2->next, is_null);

    // Make sure the head and tail pointers point to the correct wrappers.
    assert_that(vizconMutexListHead, is_equal_to(testMutex));
    assert_that(vizconMutexListTail, is_equal_to(testMutex2));
}

// status - 3 assertions.
//          Ensure that vcMutexStatus works.
Ensure(Mutexes, status)
{
    // Mutex starts unlocked. Status should be 1 (true).
    assert_that(vcMutexStatus(testMutex), is_true);

    // Lock the mutex. Status should now be 0 (false).
    vcMutexLock(testMutex);
    assert_that(vcMutexStatus(testMutex), is_false);

    // Unlock the mutex. Status should now be 1 (true).
    vcMutexUnlock(testMutex);
    assert_that(vcMutexStatus(testMutex), is_true);
}

// trylock_status - 6 assertions.
//                  Ensure that vcMutexTrylock works.
Ensure(Mutexes, trylock_status)
{
    // Mutex starts unlocked.
    // vcMutexTrylock should lock and return 1. Then, mutex status should be 0.
    assert_that(vcMutexTrylock(testMutex), is_true);
    assert_that(vcMutexStatus(testMutex), is_false);

    // vcMutexTrylock should now return 0. Mutex status should stay 0.
    assert_that(vcMutexTrylock(testMutex), is_false);
    assert_that(vcMutexStatus(testMutex), is_false);

    // Unlock the mutex. vcMutexTrylock should now work again.
    vcMutexUnlock(testMutex);
    assert_that(vcMutexTrylock(testMutex), is_true);
    assert_that(vcMutexStatus(testMutex), is_false);
}

// isolation - Variable number of assertions (ISOLATION_TEST_SIZE).
//             Ensure that changing one mutex doesn't change another.
Ensure(Mutexes, isolation)
{
    // Create an array of several mutexes.
    vcMutex* mutexes[ISOLATION_TEST_SIZE];
    int i;
    for(i = 0; i < ISOLATION_TEST_SIZE; i++)
        mutexes[i] = vcMutexCreate(NULL);
    
    // Lock one random member of the array, then check the status of everyone.
    int targeted = rand() % ISOLATION_TEST_SIZE;
    vcMutexLock(mutexes[targeted]);
    for(i = 0; i < ISOLATION_TEST_SIZE; i++)
    {
        // If this was the targeted mutex, it should be locked (false).
        if(i == targeted)
            assert_that(vcMutexStatus(mutexes[i]), is_false);

        // If this was not the targeted mutex, it should be unlocked (true).
        else
            assert_that(vcMutexStatus(mutexes[i]), is_true);
    }
}

// AfterEach - Close any mutexes still open.
//             It's okay if there was a vcThreadStart or vcThreadReturn;
//             in that case, nothing will happen.
AfterEach(Mutexes)
{
    closeAllMutexes();
}

// main - Initialize and run the suite.
//        Everything else will be handled in the suite itself.
int main() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Mutexes, create_first);
    add_test_with_context(suite, Mutexes, create_second);
    add_test_with_context(suite, Mutexes, status);
    add_test_with_context(suite, Mutexes, trylock_status);
    add_test_with_context(suite, Mutexes, isolation);
    return run_test_suite(suite, create_text_reporter());
}