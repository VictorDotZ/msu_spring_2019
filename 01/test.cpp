#include <iostream>


#include "numbers.dat"


bool isPrimeNumber(int var);

size_t find(const int* data, size_t dataSize, int var);

size_t amountOfPrimeNumbers(int start, int end, const int* data, size_t dataSize);

int main(int argc, char* argv[])
{
    if ((argc % 2 == 0) || (argc == 1))
        return -1;

    for (int i = 2; i < argc; i += 2)
        std::cout << amountOfPrimeNumbers(std::atoi(argv[i-1]), std::atoi(argv[i]),
        Data, Size) << std::endl;
    
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

size_t amountOfPrimeNumbers(int start, int end, const int* data, size_t dataSize)
{
    size_t startIndex = find(data, dataSize, start);
    size_t endIndex = find(data, dataSize, end);
    
    if ((startIndex == dataSize) || (endIndex == dataSize))
        return 0;

    size_t amount = 0;

    for (size_t i = startIndex; i <= endIndex; i++)
        if (isPrimeNumber(data[i]))
            amount++;

    return amount;
}
