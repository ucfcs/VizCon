run : compile
	./a.out
compile :
	gcc -pthread ./concurrency/utils.c ./concurrency/threads.c ./concurrency/semaphores.c ./concurrency/mutexes.c ./concurrency/vcuserlibrary.c ./concurrency_testing/thread_tests.c -lcgreen