#include "concurrency.h"

void vizconError(int func, int err)
{
    printf("\nError from ");
    switch(func)
    {
        case 0:
        {
            printf("vcThreadAdd\n");
            break;
        }
        case 1:
        {
            printf("vcThreadStart\n");
            break;
        }
        case 2:
        {
            printf("vcThreadReturn\n");
            break;
        }
        case 3:
        {
            printf("vcSemCreate\n");
            break;
        }
        case 4:
        {
            printf("vcSemWait\n");
            break;
        }
        case 5:
        {
            printf("vcSemTryWait\n");
            break;
        }
        case 6:
        {
            printf("vcSemSignal\n");
            break;
        }
        case 7:
        {
            printf("vcSemValue\n");
            break;
        }
        case 8:
        {
            printf("vcMutexCreate\n");
            break;
        }
        case 9:
        {
            printf("vcMutexLock\n");
            break;
        }
        case 10:
        {
            printf("vcMutexTryLock\n");
            break;
        }
        case 11:
        {
            printf("vcMutexUnlock\n");
            break;
        }
        case 12:
        {
            printf("vcMutexStatus\n");
            break;
        }
        default:
        {
            printf("unknown function\n");
        }
    }
    #if defined(_WIN32) // windows
    LPSTR message;
    if(err < 500)
    {
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
        printf("system error ");
    }
    #elif defined(__linux__) || defined(__APPLE__)
    char* message;
    if(err < 500)
    {
        message = strerror(err);
        printf("errno ");
    }
    #endif
    if(err >= 500)
    {
        printf("vizcon error ");
        switch(err)
        {
            case 500:
            {
                message = "A thread terminated without releasing its mutex lock.";
                break;
            }
            case 501:
            {
                message = "An unexpected wait timeout occurred.";
                break;
            }
            default:
            {
                message = "An unknown error has occurred.";
            }
        }
    }
    printf("code %d: %s\n", err, message);
    exit(0);
}