#include "vcuserlibrary.h"

#define accountCount 4

vcSem *accountSemArray;
vcSem semPrint;
vcSem tellerSem;
vcSem mainSem;
int accountArray[accountCount];

void DisplayBalance(int teller, int accountNum, int balance)
{
    vcSemWait(semPrint);
    printf("Teller %d says that account number %d has a balance of %d\n", teller, accountNum, balance);
    vcSemSignal(semPrint);
}

void DisplayError(int teller, int accountNum, int amount)
{
    vcSemWait(semPrint);
    printf("Teller %d says that account number %d cannot withdraw %d\n", teller, accountNum, amount);
    vcSemSignal(semPrint);
}

void AddSalary(int accountNum, int amount)
{
    vcSemWait(accountSemArray[accountNum]);
    accountArray[accountNum] = accountArray[accountNum] + amount;
    vcSemSignal(accountSemArray[accountNum]);
}

int Withdraw(int accountNum, int amount)
{
    vcSemWait(accountSemArray[accountNum]);
    if (amount <= accountArray[accountNum])
    {
        accountArray[accountNum] = accountArray[accountNum] - amount;
        vcSemSignal(accountSemArray[accountNum]);
        return 1;
    }
    vcSemSignal(accountSemArray[accountNum]);
    return 0;
}

void* Teller(void* param)
{
    int accountNum, amount, teller = (int)param;
    srand(vcThreadId());
    while (1)
    {
        vcSemWait(tellerSem);
        accountNum = rand() % accountCount;
        if (rand() % 2)
        {
            amount = rand() % 300;
            if (Withdraw(accountNum, amount))
            {
                DisplayBalance(teller, accountNum, accountArray[accountNum]);
            }
            else
            {
                DisplayError(teller, accountNum, amount);
            }
        }
        else
        {
            DisplayBalance(teller, accountNum, accountArray[accountNum]);
        }
        vcSemSignal(mainSem);
    }
    return (void*)0;
}

void* MainBranch(void* param)
{
    int accountNum, amount;
    srand(vcThreadId());
    while (1)
    {
        accountNum = rand() % accountCount;
        amount = rand() % 100;
        AddSalary(accountNum, amount);
        vcSemSignal(tellerSem);
        vcSemWait(mainSem);
    }
    return (void*)0;
}

int main()
{
    int i;
    srand(vcThreadId());
    accountSemArray = (vcSem*)malloc(sizeof(vcSem)*accountCount);
    semPrint = vcSemCreate(1);
    tellerSem = vcSemCreateInitial(1, 0);
    mainSem = vcSemCreateInitial(1, 0);
    for (i = 0; i < accountCount; i++)
    {
        accountSemArray[i] = vcSemCreate(1);
        accountArray[i] = rand() % 100;
    }
    vcThreadQueue(MainBranch, NULL);
    vcThreadQueue(Teller, (void*)1);
    vcThreadQueue(Teller, (void*)2);
    vcThreadStart();
    return 0;
}
