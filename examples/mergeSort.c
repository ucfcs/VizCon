#define size 10

int array[size];

void Merge(int begin, int mid, int end)
{
    int i, j, k;
    int length1 = mid - begin;
    int length2 = end - mid;
    int array1[length1];
    int array2[length2];

    for(i=0; i<length1; i++)
    {
        array1[i] = array[begin+i];
    }
    for(j=0; j<length2; j++)
    {
        array2[j] = array[mid+j];
    }

    i = 0;
    j = 0;
    k = begin;
    while(i < length1 && j < length2)
    {
        if(array1[i] <= array2[j])
        {
            array[k] = array1[i];
            i++;
        }
        else
        {
            array[k] = array2[j];
            j++;
        }
        k++;
    }

    while(i < length1)
    {
        array[k] = array1[i];
        i++;
        k++;
    }

    while(j < length2)
    {
        array[k] = array2[j];
        j++;
        k++;
    }

    return;
}

THREAD_RET MergeSort(THREAD_PARAM param)
{
    int *thisParam = (int*)param;
    int begin = thisParam[0];
    int end = thisParam[1];
    int mid = begin + (end - begin) / 2 + (end - begin) % 2;

    if(mid == end)
    {
        return 1;
    }
    thisParam[0] = begin;
    thisParam[1] = mid;
    MergeSort(thisParam);
    thisParam[0] = mid;
    thisParam[1] = end;
    MergeSort(thisParam);
    Merge(begin, mid, end);

    return 1;
}

void printArray(char* state)
{
    int i;
    printf("Array:");
    for(i=0; i<size; i++)
    {
        printf(" %d", array[i]);
    }
    printf("%s\n", state);
}

int main(void) 
{
    int i;
    int begin = 0;
    int mid = size/2 + size%2;
    int end = size;
    int param1[2] = {begin, mid};
    int param2[2] = {mid, end};
    srand(abs(vcThreadId()));
    for(i=0; i<size; i++)
    {
        array[i] = rand() % 100;
    }

    printArray(" -> Before\n");
    vcThreadQueue(MergeSort, param1);
    vcThreadQueue(MergeSort, param2);
    vcThreadStart();
    Merge(begin, mid, end);
    printArray(" -> After");

    return 1;
}