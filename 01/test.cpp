#include <iostream>


#include "numbers.dat"


bool isPrimeNumber(int var);

size_t find(const int* data, size_t dataSize, int var);

int amountOfPrimeNumbers(int start, int end, const int* data, size_t dataSize);

int main(int argc, char* argv[])
{
    if ((argc % 2 == 0) || (argc == 1))
        return -1;

    // каким именно способом осуществляется подача данных для массива непонятно
    
    size_t dataSize = sizeof(Data) / sizeof(Data[0]);
    int* data = new int[Size];
    
    for (size_t i = 0; i < dataSize; i++)
        data[i] = Data[i];
    
    for (int i = 2; i < argc; i += 2)
        std::cout << amountOfPrimeNumbers(std::atoi(argv[i-1]), std::atoi(argv[i]),
        data, dataSize) << std::endl;

    delete[] data;
    
    return 0;
}

size_t find(const int* data, size_t dataSize, int var)
{
    for (size_t i = 0; i < dataSize; i++)
        if (data[i] == var)
            return i;
            
    return dataSize;
}

bool isPrimeNumber(int var)
{
    if (var == 1)
        return false;
        
    for (int i = 2; i*i <= var; i++)
        if (var % i == 0)
            return false;

    return true;
}

int amountOfPrimeNumbers(int start, int end, const int* data, size_t dataSize)
{
    size_t startIndex = find(data, dataSize, start);
    size_t endIndex = find(data, dataSize, end);
    
    if ((startIndex == dataSize) || (endIndex == dataSize))
        return 0;

    int amount = 0;

    for (size_t i = startIndex; i <= endIndex; i++)
        if (isPrimeNumber(data[i]))
            amount++;

    return amount;
}
