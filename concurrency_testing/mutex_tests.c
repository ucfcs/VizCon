// This is a testing file that uses the Cgreen library.
// Further doumentation can be found at https://cgreen-devs.github.io/.
// Note that using Cgreen on Windows requires building and running it with MSYS2.

#include "unit_test.h"

#define CREATE_NAMED_TEST_SIZE 5
#define ISOLATION_TEST_SIZE 5

vcMutex *firstMutex;
long long int lockTarget = 0;
int lockFlag = 0;

// Mutexes - The list of mutex tests begins below.
//           Note that these tests assume that threading works correctly.
Describe(Mutexes);

// BeforeEach - Initialize objects used by multiple tests.
BeforeEach(Mutexes)
{
    firstMutex = vcMutexCreate();
    srand(time(NULL));
}

// create_first - 9 assertions.
//                Ensure that the test mutex was properly made.
Ensure(Mutexes, create_first)
{
    // The mutex should not be null.
    assert_that(firstMutex, is_not_null);

    // The "next" property should be null since there are no others.
    assert_that(firstMutex->next, is_null);

    // The ID should be 0, and the name should be "Mutex 0".
    assert_that(firstMutex->num, is_equal_to(0));
    assert_that(firstMutex->name, is_equal_to_string("Mutex 0"));

    // The mutex should be available (true) and have a holderID of 0.
    assert_that(firstMutex->available, is_true);
    assert_that(firstMutex->holderID, is_equal_to(0));

    // Platform-dependent check that the internal mutex is available.
    // Use the system trylock function instead of the VC version
    // because the VC version hasn't been checked yet.
    #ifdef _WIN32 // Windows version
        assert_that(WaitForSingleObject(firstMutex->mutex, 0), is_equal_to(WAIT_OBJECT_0));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_mutex_trylock(firstMutex->mutex), is_equal_to(0));
    #endif

    // The mutexListHead and mutexListTail values should both be firstMutex.
    assert_that(vizconMutexListHead, is_equal_to(firstMutex));
    assert_that(vizconMutexList, is_equal_to(firstMutex));
}

// create_second - 7 assertions.
//                 Create a second mutex. Ensure the name and ID are correct
//                 and that the list variables are properly updated.
Ensure(Mutexes, create_second)
{
    // Create the mutex.
    vcMutex* secondMutex = vcMutexCreate();

    // The mutex should not be null.
    assert_that(secondMutex, is_not_null);

    // The ID should be 1, and the name should be "Mutex 1".
    assert_that(secondMutex->num, is_equal_to(1));
    assert_that(secondMutex->name, is_equal_to_string("Mutex 1"));

    // The "next" for firstMutex should be secondMutex.
    // The "next" for secondMutex should be null since there are no others.
    assert_that(firstMutex->next, is_equal_to(secondMutex));
    assert_that(secondMutex->next, is_null);

    // Make sure the head and tail pointers point to the correct wrappers.
    assert_that(vizconMutexListHead, is_equal_to(firstMutex));
    assert_that(vizconMutexList, is_equal_to(secondMutex));
}

// create_named - 11 assertions.
//                Create a named mutex. Ensure that everything works
//                like a normal vcMutexCreate and
//                that the list variables are properly updated.
Ensure(Mutexes, create_named)
{
    // Create a random string of the specified length.
    // The for loop can generate all ASCII characters except a null char.
    char str[CREATE_NAMED_TEST_SIZE + 1];
    int i;
    for(int i = 0; i < CREATE_NAMED_TEST_SIZE; i++)
        str[i] = rand() % 126 + 1;
    str[CREATE_NAMED_TEST_SIZE] = '\0';

    // Create the mutex.
    vcMutex* secondMutex = vcMutexCreateNamed(str);

    // The mutex should not be null.
    assert_that(secondMutex, is_not_null);

    // The ID should be 1, and the name should be "Mutex 1".
    assert_that(secondMutex->num, is_equal_to(1));
    assert_that(secondMutex->name, is_equal_to_string(str));

    // The internal struct should be built in the same way as a vcMutexCreate.
    assert_that(secondMutex->mutex, is_not_null);
    assert_that(secondMutex->available, is_true);
    assert_that(secondMutex->holderID, is_equal_to(0));

    // Platform-dependent check that the internal mutex is available.
    // Use the system trylock function instead of the VC version
    // because the VC version hasn't been checked yet.
    #ifdef _WIN32 // Windows version
        assert_that(WaitForSingleObject(firstMutex->mutex, 0), is_equal_to(WAIT_OBJECT_0));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_mutex_trylock(firstMutex->mutex), is_equal_to(0));
    #endif

    // The nexts and lists should be set in the same way as a vcMutexCreate.
    assert_that(firstMutex->next, is_equal_to(secondMutex));
    assert_that(secondMutex->next, is_null);
    assert_that(vizconMutexListHead, is_equal_to(firstMutex));
    assert_that(vizconMutexList, is_equal_to(secondMutex));
}

// lockThread - The thread used by the lock test.
//              Parameter: int addend, int flagVal
//              Adds the addend to the global lockTarget,
//              but only if the global lockFlag is 0.
//              Returns: an unused value.
THREAD_RET lockThread(THREAD_PARAM param)
{
    // Recast the two parameters.
    int* vals = param;
    int addend = vals[0];
    int flagVal = vals[1];

    // Place the lock.
    vcMutexLock(firstMutex);

    // Check whether a flag is placed.
    // If so, add the addend to lockTarget.
    if(lockFlag == 0)
    {
        lockTarget += addend;
        lockFlag = flagVal;
    }

    // Platform-dependent mutex unlock.
    // Use the system unlock function instead of the VC version
    // because the VC version hasn't been checked yet.
    // Check the return value so we're informed if something went wrong.
    #ifdef _WIN32 // Windows version
        assert_that(ReleaseMutex(firstMutex->mutex), is_not_equal_to(0));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_mutex_unlock(firstMutex->mutex), is_equal_to(0));
    #endif

    return 0;
}

// lock - 3 assertions (1 in test body, 1 in each lockThread instance).
//        Ensure that vcMutexLock works.
Ensure(Mutexes, lock)
{
    // Create a 2-D array of argument pairs,
    // then select a random one to go "first".
    int args1[2] = {5, 1};
    int args2[2] = {10, 2};
    int* args[2] = {args1, args2};
    int first = rand() % 2;

    // Create two threads.
    // Each will try to add a number (5 or 10) the lockTarget value.
    // A lock will stop one, and when it is resumed,
    // the continueThread flag will cause it to skip the addition.
    // The randomized "first" index is used so the order is not guaranteed.
    vcThreadQueue(lockThread, (THREAD_PARAM) args[first]);
    vcThreadQueue(lockThread, (THREAD_PARAM) args[!first]);
    vcThreadStart();

    // When they finish, check that the one scheduled first got the lock.
    if(lockFlag == 1)
        assert_that(lockTarget, is_equal_to(5));
    else if(lockFlag == 2)
        assert_that(lockTarget, is_equal_to(10));
}

// unlockThread - The thread used by the unlock test.
//                Parameter: int addend, int flagVal
//                Add the addend to the global lockTarget,
//                but also make sure that the previous thread has finished.
//                Returns: an unused value.
THREAD_RET unlockThread(THREAD_PARAM param)
{
    // Recast the two parameters.
    int* vals = param;
    int addend = vals[0];
    int flagVal = vals[1];

    // Place the lock.
    vcMutexLock(firstMutex);

    // Check whether a flag is placed.
    // If not, add the addend to lockTarget.
    if(lockFlag == 0)
    {
        assert_that(lockTarget, is_equal_to(0));
        lockFlag = flagVal;
    }

    // If so, check that the value is correct.
    else
    {
        if(lockFlag == 1)
            assert_that(lockTarget, is_equal_to(5));
        else
            assert_that(lockTarget, is_equal_to(10));
    }

    // Add the addend to indicate it got this far.
    lockTarget += addend;

    // Remove the lock.
    vcMutexUnlock(firstMutex);

    return 0;
}

// unlock - 3 assertions (1 in test body, 1 in each lockThread instance).
//          Ensure that vcMutexUnlock works.
Ensure(Mutexes, unlock)
{
    // Create a 2-D array of argument pairs,
    // then select a random one to go "first".
    int args1[2] = {5, 1};
    int args2[2] = {10, 2};
    int* args[2] = {args1, args2};
    int first = rand() % 2;

    // Create two threads.
    // Each will try to add a number (5 or 10) the lockTarget value.
    // A lock will stop one, and when it is resumed,
    // the continueThread flag will cause it to skip the addition.
    // The randomized "first" index is used so the order is not guaranteed.
    vcThreadQueue(unlockThread, (THREAD_PARAM) args[first]);
    vcThreadQueue(unlockThread, (THREAD_PARAM) args[!first]);
    vcThreadStart();

    // When they finish, check that both got to make the edit.
    assert_that(lockTarget, is_equal_to(15));
}

// status - 3 assertions.
//          Ensure that vcMutexStatus works.
Ensure(Mutexes, status)
{
    // Mutex starts unlocked. Status should be 1 (true).
    assert_that(vcMutexStatus(firstMutex), is_true);

    // Lock the mutex. Status should now be 0 (false).
    vcMutexLock(firstMutex);
    assert_that(vcMutexStatus(firstMutex), is_false);

    // Unlock the mutex. Status should now be 1 (true).
    vcMutexUnlock(firstMutex);
    assert_that(vcMutexStatus(firstMutex), is_true);
}

// trylock - 6 assertions.
//           Ensure that vcMutexTrylock works.
Ensure(Mutexes, trylock)
{
    // Mutex starts unlocked.
    // vcMutexTrylock should lock and return 1. Then, mutex status should be 0.
    assert_that(vcMutexTrylock(firstMutex), is_true);
    assert_that(vcMutexStatus(firstMutex), is_false);

    // vcMutexTrylock should now return 0. Mutex status should stay 0.
    assert_that(vcMutexTrylock(firstMutex), is_false);
    assert_that(vcMutexStatus(firstMutex), is_false);

    // Unlock the mutex. vcMutexTrylock should now work again.
    vcMutexUnlock(firstMutex);
    assert_that(vcMutexTrylock(firstMutex), is_true);
    assert_that(vcMutexStatus(firstMutex), is_false);
}

// double_lock - 1 assertion, 1 skipped assertion, 1 exception.
//               Attempt to lock a locked mutex in the same thread.
//               This will result in an a vizcon error, code 511.
Ensure(Mutexes, double_lock)
{
    // Lock the mutex. This should work fine.
    vcMutexLock(firstMutex);

    // Platform-dependent thread check.
    // Verify that this thread placed the lock.
    #ifdef _WIN32 // Windows version
        assert_that(GetCurrentThreadId(), is_equal_to(firstMutex->holderID));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_self(), is_equal_to(firstMutex->holderID));
    #endif

    // Lock the mutex again. This should throw an error 511.
    vcMutexLock(firstMutex);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// double_unlock - 1 assertion, 1 skipped assertion, 1 exception.
//                 Attempt to unlock an unlocked mutex.
//                 This will result in an a vizcon error, code 510.
Ensure(Mutexes, double_unlock)
{
    // Verify that the mutex is unlocked.
    assert_that(vcMutexStatus(firstMutex), is_true);

    // Unlock the mutex. This should throw an error 510.
    vcMutexUnlock(firstMutex);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// crossUnlockThread - The thread used by the cross_unlock test.
//                     Parameter: an unused value.
//                     Attempts to unlock the mutex locked by the spawner.
//                     Returns: an unused value.
THREAD_RET crossUnlockThread(THREAD_PARAM param)
{
    // Platform-dependent thread check.
    // Verify that this thread didn't place the lock.
    #ifdef _WIN32 // Windows version
        assert_that(GetCurrentThreadId(), is_not_equal_to(firstMutex->holderID));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(pthread_self(), is_not_equal_to(firstMutex->holderID));
    #endif

    // Unlock the mutex. This should throw an error 512.
    vcMutexUnlock(firstMutex);
    return 0;
}

// cross_unlock - 1 assertion, 1 skipped assertion, 1 exception.
//                Attempt to unlock an mutex locked by another thread.
//                This will result in an a vizcon error, code 512.
Ensure(Mutexes, cross_unlock)
{
    // Lock the mutex.
    vcMutexLock(firstMutex);

    // Create a thread that will try to unlock the mutex.
    // It should encounter an error.
    vcThreadQueue(crossUnlockThread, NULL);
    vcThreadStart();

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
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

// End of the suite.
// Total number of assertions: 45 + ISOLATION_TEST_SIZE
// Total number of exceptions: 3

// main - Initialize and run the suite.
//        Everything else will be handled in the suite itself.
int main() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Mutexes, create_first);
    add_test_with_context(suite, Mutexes, create_second);
    add_test_with_context(suite, Mutexes, create_named);
    add_test_with_context(suite, Mutexes, lock);
    add_test_with_context(suite, Mutexes, unlock);
    add_test_with_context(suite, Mutexes, status);
    add_test_with_context(suite, Mutexes, trylock);
    add_test_with_context(suite, Mutexes, double_lock);
    add_test_with_context(suite, Mutexes, double_unlock);
    add_test_with_context(suite, Mutexes, cross_unlock);
    add_test_with_context(suite, Mutexes, isolation);
    return run_test_suite(suite, create_text_reporter());
}