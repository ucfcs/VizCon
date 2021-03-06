// This is a testing file that uses the Cgreen library.
// Further doumentation can be found at https://cgreen-devs.github.io/.
// Note that using Cgreen on Windows requires building and running it with MSYS2.

#include "unit_test.h"

vcSemaphore firstSem;
long long int permitTarget = 0;
int permitFlag = 0;

// Semaphores - The list of semaphore tests begins below.
Describe(Semaphores);

// BeforeEach - Initialize objects used by multiple tests.
BeforeEach(Semaphores)
{
    firstSem = vcSemCreate(2);
}

// create_first - 9 assertions.
//                Ensure that the test semaphore was properly made.
Ensure(Semaphores, create_first)
{
    // The semaphore should not be null.
    assert_that(firstSem, is_not_null);

    // The "next" property should be null since there are no others.
    assert_that(firstSem->next, is_null);

    // The semaphore value and maximum value should be the provided value (2).
    assert_that(firstSem->count, is_equal_to(2));
    assert_that(firstSem->maxCount, is_equal_to(2));

    // Platform-dependent check that the internal semaphore
    // has exactly the predefined value (2).
    // Use the system trylock function instead of the VC version
    // because the VC version hasn't been checked yet.
    #ifdef _WIN32 // Windows version
        assert_that(WaitForSingleObject(firstSem->sem, 0), is_equal_to(WAIT_OBJECT_0));
        assert_that(WaitForSingleObject(firstSem->sem, 0), is_equal_to(WAIT_OBJECT_0));
        assert_that(WaitForSingleObject(firstSem->sem, 0), is_equal_to(WAIT_TIMEOUT));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(sem_trywait(firstSem->sem), is_equal_to(0));
        assert_that(sem_trywait(firstSem->sem), is_equal_to(0));
        sem_trywait(firstSem->sem);
        assert_that(errno, is_equal_to(EAGAIN));
    #endif

    // The mutexListHead and mutexListTail values should both be firstSem.
    assert_that(vizconSemListHead, is_equal_to(firstSem));
    assert_that(vizconSemList, is_equal_to(firstSem));
}

// create_second - 5 assertions.
//                 Create a second mutex. Ensure the name and ID are correct
//                 and that the list variables are properly updated.
Ensure(Semaphores, create_second)
{
    // Create the mutex.
    vcSem secondSem = vcSemCreate(1);

    // The mutex should not be null.
    assert_that(secondSem, is_not_null);

    // The "next" for firstSem should be secondSem.
    // The "next" for secondSem should be null since there are no others.
    assert_that(firstSem->next, is_equal_to(secondSem));
    assert_that(secondSem->next, is_null);

    // Make sure the head and tail pointers point to the correct wrappers.
    assert_that(vizconSemListHead, is_equal_to(firstSem));
    assert_that(vizconSemList, is_equal_to(secondSem));
}

// create_initial - 9 assertions.
//                  Ensure that the initial and max are properly set.
Ensure(Semaphores, create_initial)
{
    // Create the mutex.
    vcSem initialSem = vcSemCreateInitial(2, 1);

    // The mutex should not be null.
    assert_that(initialSem, is_not_null);

    // The "next" for firstSem should be initialSem.
    // The "next" for initialSem should be null since there are no others.
    assert_that(firstSem->next, is_equal_to(initialSem));
    assert_that(initialSem->next, is_null);

    // Make sure the head and tail pointers point to the correct wrappers.
    assert_that(vizconSemListHead, is_equal_to(firstSem));
    assert_that(vizconSemList, is_equal_to(initialSem));

    // The semaphore value should be the initial value,
    // and the max value should be the given one.
    assert_that(initialSem->count, is_equal_to(1));
    assert_that(initialSem->maxCount, is_equal_to(2));

    // Platform-dependent check that the internal semaphore
    // has exactly the predefined value (1).
    // Use the system trylock function instead of the VC version
    // because the VC version hasn't been checked yet.
    // The max count will not be checked since we aren't relying on OS-level detection.
    #ifdef _WIN32 // Windows version
        assert_that(WaitForSingleObject(initialSem->sem, 0), is_equal_to(WAIT_OBJECT_0));
        assert_that(WaitForSingleObject(initialSem->sem, 0), is_equal_to(WAIT_TIMEOUT));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(sem_trywait(initialSem->sem), is_equal_to(0));
        sem_trywait(initialSem->sem);
        assert_that(errno, is_equal_to(EAGAIN));
    #endif
}

// bad_count - 1 skipped assertion, 1 exception.
//             Attempt to create a semaphore with an invalid permit count.
//             This will result in an a vizcon error, code 502.
Ensure(Semaphores, bad_count)
{
    // Pick a random number between 0 and -100.
    int numPermits = 0 - (rand() % 101);

    // Attempt to make the permit.
    vcSem badSem = vcSemCreate(numPermits);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// bad_initial_count - 1 skipped assertion, 1 exception.
//                     Attempt to create a semaphore with an invalid initial permit count.
//                     This will result in an a vizcon error, code 502.
Ensure(Semaphores, bad_initial_count)
{
    // Pick a random number between -1 and -100.
    int numPermits = (-1) - (rand() % 100);

    // Attempt to make the permit.
    vcSem badSem = vcSemCreateInitial(1, numPermits);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// bad_max_count - 1 skipped assertion, 1 exception.
//                 Attempt to create a semaphore with an invalid max permit count.
//                 This will result in an a vizcon error, code 502.
Ensure(Semaphores, bad_max_count)
{
    // Pick a random number between 0 and -100.
    int numPermits = 0 - (rand() % 101);

    // Attempt to make the permit.
    vcSem badSem = vcSemCreateInitial(numPermits, 1);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// small_max_count - 1 skipped assertion, 1 exception.
//                   Attempt to create a semaphore with a max count smaller than the initial count.
//                   This will result in an a vizcon error, code 502.
Ensure(Semaphores, small_max_count)
{
    // Pick two random numbers between 1 and 100.
    int rand1 = rand() % 100 + 1;
    int rand2 = rand() % 100 + 1;

    // Attempt to make the permit so the smaller number is the max.
    vcSem badSem;
    if(rand1 == rand2)
        badSem = vcSemCreateInitial(rand1, rand1 + rand2);
    else if(rand1 > rand2)
        badSem = vcSemCreateInitial(rand2, rand1);
    else
        badSem = vcSemCreateInitial(rand1, rand2);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// waitThread - The thread used by the lock test.
//              Parameter: int addend, int flagVal
//              Adds the addend to the global permitTarget,
//              but only if the global permitFlag is 0.
//              Returns: an unused value.
void* waitThread(void* param)
{
    // Recast the two parameters.
    int* vals = param;
    int addend = vals[0];
    int flagVal = vals[1];

    // Take the one available permit.
    vcSemWait(firstSem);

    // Check whether a flag is placed.
    // If so, add the addend to permitTarget.
    if(permitFlag == 0)
    {
        permitTarget += addend;
        permitFlag = flagVal;
    }

    // Platform-dependent semaphore release.
    // Use the system unlock function instead of the VC version
    // because the VC version hasn't been checked yet.
    // Check the return value so we're informed if something went wrong.
    #ifdef _WIN32 // Windows version
        assert_that(ReleaseSemaphore(firstSem->sem, 1, NULL), is_not_equal_to(0));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(sem_post(firstSem->sem), is_equal_to(0));
    #endif

    return 0;
}

// wait - 4 assertions (2 in test body, 1 in each waitThread instance).
//        Ensure that vcSemWait works.
Ensure(Semaphores, wait)
{
    // Take one permit, since it isn't needed.
    // Verify it was taken.
    vcSemWait(firstSem);
    assert_that(firstSem->count, is_equal_to(1));

    // Create a 2-D array of argument pairs,
    // then select a random one to go "first".
    int args1[2] = {5, 1};
    int args2[2] = {10, 2};
    int* args[2] = {args1, args2};
    int first = rand() % 2;

    // Create two threads.
    // Each will try to add a number (5 or 10) the permitTarget value.
    // A lock will stop one, and when it is resumed,
    // the continueThread flag will cause it to skip the addition.
    // The randomized "first" index is used so the order is not guaranteed.
    vcThreadQueue(waitThread, (void*) args[first]);
    vcThreadQueue(waitThread, (void*) args[!first]);
    vcThreadStart();

    // When they finish, check that the one scheduled first got the lock.
    // If there's an issue, the tautological falsehood will inform us.
    if(permitFlag == 1)
        assert_that(permitTarget, is_equal_to(5));
    else if(permitFlag == 2)
        assert_that(permitTarget, is_equal_to(10));
    else
        assert_that(0, is_equal_to(1));
}

// waitMultThread - The thread used by the wait_mult test.
//                  Parameter: int addend, int flagVal
//                  Adds the addend to the global permitTarget,
//                  but only if the global permitFlag is 0.
//                  Returns: an unused value.
void* waitMultThread(void* param)
{
    // Recast the two parameters.
    int* vals = param;
    int addend = vals[0];
    int flagVal = vals[1];

    // Take both permits.
    vcSemWaitMult(firstSem, 2);

    // Check whether a flag is placed.
    // If so, add the addend to permitTarget.
    if(permitFlag == 0)
    {
        permitTarget += addend;
        permitFlag = flagVal;
    }

    // Platform-dependent semaphore release.
    // Use the system unlock function instead of the VC version
    // because the VC version hasn't been checked yet.
    // Check the return value so we're informed if something went wrong.
    // Also increase and assert the count, because vcSemWaitMult checks that.
    #ifdef _WIN32 // Windows version
        assert_that(ReleaseSemaphore(firstSem->sem, 1, NULL), is_not_equal_to(0));
        assert_that(ReleaseSemaphore(firstSem->sem, 1, NULL), is_not_equal_to(0));
        firstSem->count += 2;
        assert_that(firstSem->count, is_equal_to(2));
    #elif __linux__ || __APPLE__ // POSIX version
        assert_that(sem_post(firstSem->sem), is_equal_to(0));
        assert_that(sem_post(firstSem->sem), is_equal_to(0));
        firstSem->count += 2;
        assert_that(firstSem->count, is_equal_to(2));
    #endif

    return 0;
}

// wait_mult - 7 assertions (1 in test body, 3 in each waitMultThread instance), 1 skipped assertion.
//             Ensure that vcSemWaitMult works.
Ensure(Semaphores, wait_mult)
{
    // Create a 2-D array of argument pairs,
    // then select a random one to go "first".
    int args1[2] = {5, 1};
    int args2[2] = {10, 2};
    int* args[2] = {args1, args2};
    int first = rand() % 2;

    // Create two threads.
    // Each will try to add a number (5 or 10) the permitTarget value.
    // A lock will stop one, and when it is resumed,
    // the continueThread flag will cause it to skip the addition.
    // The randomized "first" index is used so the order is not guaranteed.
    vcThreadQueue(waitMultThread, args[first]);
    vcThreadQueue(waitMultThread, args[!first]);
    vcThreadStart();

    // When they finish, check that the one scheduled first got the lock.
    // If there's an issue, the tautological falsehood will inform us.
    if(permitFlag == 1)
        assert_that(permitTarget, is_equal_to(5));
    else if(permitFlag == 2)
        assert_that(permitTarget, is_equal_to(10));
    else
        assert_that(0, is_equal_to(1));
}

// waitThread - The thread used by the lock test.
//              Parameter: int flagVal
//              Adds the addend to the global permitTarget,
//              but only if the global permitFlag is 0.
//              Returns: an unused value.
void* waitTwoThread(void* param)
{
    // Recast the parameter.
    int flagVal = *((int*) param);

    // Place the lock.
    vcSemWait(firstSem);

    // If this thread is first, waste some time.
    if(permitFlag == 0)
    {
        permitFlag = flagVal;

        // Platform-dependent 5 millisecond (5000 microsecond) wait.
        #ifdef _WIN32 // Windows version
            Sleep(5);
        #elif __linux__ || __APPLE__ // POSIX version
            usleep(5000);
        #endif
    }

    // Since there is no signal, both would only get here
    // if vcSemWait recognizes the two permits.
    assert_that(firstSem->count, is_equal_to(0));

    return 0;
}

// wait_two - 2 assertions (1 in each waitThread instance).
//            Wait with two permits, so both threads run at the same time.
Ensure(Semaphores, wait_two)
{
    // Create a 2-D array of argument pairs,
    // then select a random one to go "first".
    int args[2] = {1, 2};
    int first = rand() % 2;

    // Create two threads.
    // Each will try to add a number (5 or 10) the permitTarget value.
    // A lock will stop one, and when it is resumed,
    // the continueThread flag will cause it to skip the addition.
    // The randomized "first" index is used so the order is not guaranteed.
    vcThreadQueue(waitTwoThread, &args[first]);
    vcThreadQueue(waitTwoThread, &args[!first]);
    vcThreadStart();
}

// signalThread - The thread used by the signal test.
//                Parameter: int addend, int flagVal
//                Add the addend to the global permitTarget,
//                but also make sure that the previous thread has finished.
//                Returns: an unused value.
void* signalThread(void* param)
{
    // Recast the two parameters.
    int* vals = param;
    int addend = vals[0];
    int flagVal = vals[1];

    // Take the permit.
    vcSemWait(firstSem);

    // Check whether a flag is placed.
    // If not, set the flag value.
    if(permitFlag == 0)
    {
        assert_that(permitTarget, is_equal_to(0));
        permitFlag = flagVal;
    }

    // If so, check that the value is correct.
    // If there's an issue, the tautological falsehood will inform us.
    else
    {
        if(permitFlag == 1)
            assert_that(permitTarget, is_equal_to(5));
        else if(permitFlag == 2)
            assert_that(permitTarget, is_equal_to(10));
        else
            assert_that(0, is_equal_to(1));
    }

    // Add the addend to indicate it got this far.
    permitTarget += addend;

    // Remove the lock.
    vcSemSignal(firstSem);

    return 0;
}

// signal - 4 assertions (2 in test body, 1 in each signalThread instance).
//          Ensure that vcSemSignal works.
Ensure(Semaphores, signal)
{
    // Take one permit, since it isn't needed.
    // Verify it was taken.
    vcSemWait(firstSem);
    assert_that(firstSem->count, is_equal_to(1));

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
    vcThreadQueue(signalThread, args[first]);
    vcThreadQueue(signalThread, args[!first]);
    vcThreadStart();

    // When they finish, check that both got to make the edit.
    assert_that(permitTarget, is_equal_to(15));
}

// signalMultThread - The thread used by the signal_mult test.
//                Parameter: int addend, int flagVal
//                Add the addend to the global permitTarget,
//                but also make sure that the previous thread has finished.
//                Returns: an unused value.
void* signalMultThread(void* param)
{
    // Recast the two parameters.
    int* vals = param;
    int addend = vals[0];
    int flagVal = vals[1];

    // Take the permit.
    vcSemWaitMult(firstSem, 2);

    // Check whether a flag is placed.
    // If not, set the flag value.
    if(permitFlag == 0)
    {
        assert_that(permitTarget, is_equal_to(0));
        permitFlag = flagVal;
    }

    // If so, check that the value is correct.
    // If there's an issue, the tautological falsehood will inform us.
    else
    {
        if(permitFlag == 1)
            assert_that(permitTarget, is_equal_to(5));
        else if(permitFlag == 2)
            assert_that(permitTarget, is_equal_to(10));
        else
            assert_that(0, is_equal_to(1));
    }

    // Add the addend to indicate it got this far.
    permitTarget += addend;

    // Remove the lock.
    vcSemSignalMult(firstSem, 2);

    return 0;
}

// signal_mult - 3 assertions (1 in test body, 1 in each signalThread instance).
//               Ensure that vcSemSignal works.
Ensure(Semaphores, signal_mult)
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
    vcThreadQueue(signalMultThread, args[first]);
    vcThreadQueue(signalMultThread, args[!first]);
    vcThreadStart();

    // When they finish, check that both got to make the edit.
    assert_that(permitTarget, is_equal_to(15));
}

// signal_over - 1 assertion, 1 skipped assertion, 1 exception.
//               Attempt to signal a semaphore beyond the max.
//               This will result in an a vizcon error, code 513.
Ensure(Semaphores, signal_over)
{
    // Verify that the semaphore is at max value.
    assert_that(firstSem->count, is_equal_to(firstSem->maxCount));

    // Signal the semaphore. This should throw an error 513.
    vcSemSignal(firstSem);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// signal_mult_over - 1 assertion, 1 skipped assertion, 1 exception.
//                    Attempt to signal a semaphore beyond the max with vcSemSignalMult.
//                    This will result in an a vizcon error, code 513.
Ensure(Semaphores, signal_mult_over)
{
    // Take the semaphore once so it's one away from max value.
    // Then verify that the wait was successful.
    vcSemWait(firstSem);
    assert_that(firstSem->count, is_equal_to(firstSem->maxCount - 1));

    // Signal the semaphore twice. This should throw an error 513.
    vcSemSignalMult(firstSem, 2);

    // Tautological falsehood.
    // If it's reported, then the program didn't close correctly.
    assert_that(0, is_true);
}

// value - 6 assertions.
//         Ensure that vcSemValue works.
Ensure(Semaphores, value)
{
    // Mutex starts with two permits.
    assert_that(vcSemValue(firstSem), is_equal_to(2));

    // Take one permit and the value should be one.
    // Repeat and it should be 0.
    vcSemWait(firstSem);
    assert_that(vcSemValue(firstSem), is_equal_to(1));
    vcSemWait(firstSem);
    assert_that(vcSemValue(firstSem), is_equal_to(0));

    // Return both permits and the value should be 2 again.
    vcSemSignalMult(firstSem, 2);
    assert_that(vcSemValue(firstSem), is_equal_to(2));

    // Remove both permits and it should be 0.
    // Add one back and it should be 1.
    vcSemWaitMult(firstSem, 2);
    assert_that(vcSemValue(firstSem), is_equal_to(0));
    vcSemSignal(firstSem);
    assert_that(vcSemValue(firstSem), is_equal_to(1));
}

// trywait - 10 assertions.
//           Ensure that vcMutexTrywait works.
Ensure(Semaphores, trywait)
{
    // Semaphore starts with two permits.
    // vcSemTrywait should get one permit and return 1.
    // Then, semaphore value should be 1.
    assert_that(vcSemTryWait(firstSem), is_true);
    assert_that(vcSemValue(firstSem), is_equal_to(1));

    // Try to wait for two permits.
    // Since only one is available, this should fail.
    // The value should remain the same.
    assert_that(vcSemTryWaitMult(firstSem, 2), is_false);
    assert_that(vcSemValue(firstSem), is_equal_to(1));

    // Release one permit, then try to wait for two again.
    // This should work, leaving a value of zero.
    vcSemSignal(firstSem);
    assert_that(vcSemTryWaitMult(firstSem, 2), is_true);
    assert_that(vcSemValue(firstSem), is_equal_to(0));

    // Try to wait for one, then two.
    // Neither should work or change the value.
    assert_that(vcSemTryWait(firstSem), is_false);
    assert_that(vcSemValue(firstSem), is_equal_to(0));
    assert_that(vcSemTryWaitMult(firstSem, 2), is_false);
    assert_that(vcSemValue(firstSem), is_equal_to(0));
}

// AfterEach - Close any semaphores still open.
//             It's okay if there was a vcThreadStart or vcThreadReturn;
//             in that case, nothing will happen.
AfterEach(Semaphores)
{
    closeAllSemaphores();
}

// End of the suite.
// Total number of assertions: 61
// Total number of exceptions: 6  (bad_count: 02, bad_initial_count: 02, bad_max_count: 02, small_max_count: 02
//                                 signal_over: 14, signal_mult_over: 14)

// main - Initialize and run the suite.
//        Everything else will be handled in the suite itself.
int main() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Semaphores, create_first);
    add_test_with_context(suite, Semaphores, create_second);
    add_test_with_context(suite, Semaphores, create_initial);
    add_test_with_context(suite, Semaphores, bad_count);
    add_test_with_context(suite, Semaphores, bad_initial_count);
    add_test_with_context(suite, Semaphores, bad_max_count);
    add_test_with_context(suite, Semaphores, small_max_count);
    add_test_with_context(suite, Semaphores, wait);
    add_test_with_context(suite, Semaphores, wait_mult);
    add_test_with_context(suite, Semaphores, wait_two);
    add_test_with_context(suite, Semaphores, signal);
    add_test_with_context(suite, Semaphores, signal_mult);
    add_test_with_context(suite, Semaphores, signal_over);
    add_test_with_context(suite, Semaphores, signal_mult_over);
    add_test_with_context(suite, Semaphores, value);
    add_test_with_context(suite, Semaphores, trywait);
    return run_test_suite(suite, create_text_reporter());
}
