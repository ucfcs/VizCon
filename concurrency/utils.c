#include "utils.h"

// Reference to external function vcHalt (in vcuserlibrary.c)
extern void vcHalt(int exitCode);

// vizconError - Prints errors encountered by the user library.
//               The program closes after this method finishes.
void vizconError(char* func, int err)
{
    // Start building the message string.
    char message[MAX_ERROR_MESSAGE_LENGTH];

    // Platform-dependent error decoding.
    // If the error is less than 500, it's not from our library.
    // Get the corresponding string from the system's error descriptions.
    // Then print, close everything, and leave.
    #ifdef _WIN32 // Windows version
        LPSTR errorMessage;
        if(err < 500)
        {
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD) err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &errorMessage, 0, NULL);
            sprintf(message, "\nError from %s.\nsystem error %d: %s", func, err, errorMessage);
            message[MAX_ERROR_MESSAGE_LENGTH - 1] = '\0';
            printf("%s", message);
            vcHalt(err);
        }
    #elif __linux__ || __APPLE__ // POSIX version
        char* errorMessage;
        if(err < 500)
        {
            sprintf(message, "\nError from %s.\nerrno code %d", func, err);
            errno = err;
            perror(message);
            vcHalt(err);
        }
    #endif

    // If the error is 500 or greater, it's a VizCon-specific error.
    // Select the string related to the error.
    if(err >= 500)
    {
        switch(err)
        {
            case VC_ERROR_ABANDONED:
            {
                errorMessage = "A thread terminated without releasing a mutex lock it acquired.";
                break;
            }
            case VC_ERROR_TIMEOUT:
            {
                errorMessage = "An unexpected wait timeout occurred.";
                break;
            }
            case VC_ERROR_MEMORY:
            {
                errorMessage = "Not enough memory resources are available to process this command.";
                break;
            }
            case VC_ERROR_BADCOUNT:
            {
                errorMessage = "A semaphore was created with an invalid maximum permit value.";
                break;
            }
            case VC_ERROR_NAMEERROR:
            {
                errorMessage = "There was an error saving the internal mutex name.";
                break;
            }
            case VC_ERROR_DOUBLEUNLOCK:
            {
                errorMessage = "A thread attempted to unlock an already-unlocked mutex.";
                break;
            }
            case VC_ERROR_DOUBLELOCK:
            {
                errorMessage = "A thread attempted to lock a mutex that it already locked.";
                break;
            }
            case VC_ERROR_CROSSTHREADUNLOCK:
            {
                errorMessage = "A thread attempted to unlock an mutex that was locked by another thread.";
                break;
            }
            case VC_ERROR_SEMVALUELIMIT:
            {
                errorMessage = "Too many posts were made to a semaphore.";
                break;
            }
            case VC_ERROR_CREATEDISABLED:
            {
                errorMessage = "Threads may not be queued while threads are active.";
                break;
            }
            case VC_ERROR_THREADSACTIVE:
            {
                errorMessage = "An instance of a thread starting function is already active.";
                break;
            }
            default:
                errorMessage = "An unknown error has occurred.";
        }
    }

    // Print the message and leave.
    sprintf(message, "\nError from %s.\nvizcon error code %d: %s\n", func, err, errorMessage);
    message[MAX_ERROR_MESSAGE_LENGTH - 1] = '\0';
    printf("%s", message);
    vcHalt(err);
}