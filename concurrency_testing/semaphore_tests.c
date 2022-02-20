// This is a testing file that uses the Cgreen library.
// Further doumentation can be found at https://cgreen-devs.github.io/.
// Note that using Cgreen on Windows requires building and running it with MSYS2.

#include "unit_test.h"

vcSemaphore* testSem;

// Semaphores - The list of semaphore tests begins below.
Describe(Semaphores);

// BeforeEach - Initialize objects used by multiple tests.
BeforeEach(Semaphores)
{
    testSem = vcSemCreate(2);
}

// create_first - 3 assertions.
//                Ensure that the test semaphore was properly made.
Ensure(Semaphores, create_first)
{
    // The mutex should not be null.
    assert_that(testSem, is_not_null);

    // The "next" property should be null since there are no others.
    assert_that(testSem->next, is_null);

    // The contained "sem" should not be null.
    assert_that(testSem->sem, is_not_null);

    // FIXME: Add tests for the global semaphore list.
}

// AfterEach - To be determined.
AfterEach(Semaphores){}

// main - Initialize and run the suite.
//        Everything else will be handled in the suite itself.
int main() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Semaphores, create_first);
    return run_test_suite(suite, create_text_reporter());
}