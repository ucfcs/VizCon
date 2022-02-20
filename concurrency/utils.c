#include "utils.h"

// vizconCreateName - Creates a string that can be used to name a struct.
//                    Returns: a pointer to the created string.
char* vizconCreateName(int type, int value)
{
    // Determine the number of characters in value.
    int i = 1, temp = value / 10;
    while(temp != 0)
    {
        i++;
        temp = temp / 10;
    }

    // Allocate the string. Error out if necessary.
    char* ret = (char*) malloc(sizeof(char) * (11 + i));
    if(ret == NULL)
        vizconError("create function", VC_ERROR_MEMORY);
    
    // Add the value to a string based on the type and return.
    switch(type)
    {
        // VC_TYPE_THREAD - Thread.
        case VC_TYPE_THREAD:
        {
            sprintf(ret, "Thread %d", value);
            return ret;
        }

        // VC_TYPE_SEM - Semaphore.
        case VC_TYPE_SEM:
        {
            sprintf(ret, "Semaphore %d", value);
            return ret;
        }

        // VC_TYPE_MUTEX - Mutex lock.
        case VC_TYPE_MUTEX:
        {
            sprintf(ret, "Mutex %d", value);
            return ret;
        }

        // Default - If anything else, return nothing.
        default:
            return NULL;
    }
}

// vizconStringLength - Calculate the length of the name string.
//                      Returns: the length.
int vizconStringLength(char* name)
{
    int i;
    for(i = 0; name[i] != '\0'; i++);
    return i;
}

// vizconError - Prints errors encountered by the user library.
//               The program closes after this method finishes.
void vizconError(char* func, int err)
{
    // Start building the message string.
    char message[200];
    sprintf(message, "\nError from %s.\n", func);

    // Platform-dependent error decoding.
    // If the error is less than 500, it's not from our library.
    // Get the corresponding string from the system's error descriptions.
    #if defined(_WIN32) // Windows version
    LPSTR errorMessage;
    if(err < 500)
    {
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorMessage, 0, NULL);
        sprintf(message, "%ssystem error", errorMessage);
    }

    #elif defined(__linux__) || defined(__APPLE__) // POSIX version
    char* errorMessage;
    if(err < 500)
    {
        sprintf(errorMessage, "%serrno code %d", message, err);
        printf("%s", errorMessage);
        errno = err;
        perror(message);
        exit(0);
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
            default:
                errorMessage = "An unknown error has occurred.";
        }
        sprintf(message, "vizcon error code %d: %s\n", err, errorMessage);
    }
    printf("%s", message);
    exit(0);
}