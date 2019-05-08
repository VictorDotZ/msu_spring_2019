#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <stack>


template <class T>
std::vector<T> takeSequence(const std::vector<T>& array, size_t left,
                            size_t right) {
    std::vector<T> result(right - left);
    for (size_t i = 0; i < result.size(); i++)
        result[i] = array[left + i];
    return result;
}


template <class T>
void merge(std::vector<T>& array, size_t left, size_t mid, size_t right) {
    size_t it1 = 0;
    size_t it2 = 0;
    std::vector<T> result(right - left);

    while ((left + it1 < mid) && (mid + it2 < right)) {
        if (array[left + it1] < array[mid + it2]) {
            result[it1 + it2] = array[left + it1];
            it1++;
        } else {
            result[it1 + it2] = array[mid + it2];
            it2++;
        }
    }

    while (left + it1 < mid) {
        result[it1 + it2] = array[left + it1];
        it1++;
    }

    while (mid + it2 < right) {
        result[it1 + it2] = array[mid + it2];
        it2++;
    }

    for (size_t i = 0; i < it1 + it2; i++)
        array[left + i] = result[i];
}


template <class T>
void mergeSort(std::vector<T>& array) {
    for (size_t i = 1; i < array.size(); i *= 2)
        for (size_t j = 0; j < array.size() - i; j += 2 * i)
            merge(array, j, j + i, std::min(j + 2 * i, array.size()));
}


template <class T>
void mergeSortMT(std::vector<T>& array) {
    size_t mid = array.size() / 2;
    std::vector<T> leftHalf = takeSequence(array, 0, mid);
    std::vector<T> rightHalf = takeSequence(array, mid, array.size());

    std::thread sortLeft(mergeSort<T>, std::ref(leftHalf));
    std::thread sortRight(mergeSort<T>, std::ref(rightHalf));

    sortLeft.join();
    sortRight.join();

    array = leftHalf;
    std::move(rightHalf.begin(), rightHalf.end(), std::back_inserter(array));
    merge(array, 0, mid, array.size());
}


template <class T>
class FileSort
{
public:
    FileSort(std::string inputPath, std::string outputPath,
             const size_t chunkSize) : inputPath(std::move(inputPath)),
                                       outputPath(std::move(outputPath)),
                                       chunkSize(chunkSize) {}

    void startSorting() {
        createSortedChunks();
        mergeSortedChunks();
    }

private:

    void readChunkFromFileToVec(std::vector<T>& chunkData) {
        size_t i = 0;
        for (; (i < chunkData.size()) && !this->input.eof(); i++)
            this->input.read(reinterpret_cast<char *>(&chunkData[i]),
                             sizeof(chunkData[i]));
        if (i != chunkData.size())
            chunkData.resize(i-1);
    }

    void writeChunkFromVecToFile(std::vector<T>& chunkData) {
        for (size_t i = 0; i < chunkData.size(); i++)
            this->output.write(reinterpret_cast<const char *>(&chunkData[i]),
                               sizeof(chunkData[i]));
    }

    void createSortedChunks() {
        std::vector<T> chunkData(this->chunkSize);
        this->input.open(this->inputPath, std::ios::binary);
        while (!this->input.eof()) {
            readChunkFromFileToVec(chunkData);
            mergeSortMT(chunkData);
            std::string tmpName = std::tmpnam(nullptr);
            this->output.open(tmpPath + tmpName, std::ios::binary);
            writeChunkFromVecToFile(chunkData);
            this->sortedChunkFilenames.push(std::move(tmpName));
            this->output.close();
        }
        this->input.close();
    }

    void mergeSortedChunks() {
        if (this->sortedChunkFilenames.size() == 1) {
            std::string candidateName = this->sortedChunkFilenames.top();
            this->sortedChunkFilenames.pop();

            std::ifstream candidate(tmpPath + candidateName, std::ios::binary);
            this->output.open(this->outputPath, std::ios::binary);

            T tmp;
            while (!candidate.eof()) {
                candidate.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
                this->output.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            }

            candidate.close();
            this->output.close();
            std::remove((tmpPath + candidateName).c_str());
            return;
        }

        while (this->sortedChunkFilenames.size() > 2) {
            std::string candidate1Name = this->sortedChunkFilenames.top();
            this->sortedChunkFilenames.pop();
            std::string candidate2Name = this->sortedChunkFilenames.top();
            this->sortedChunkFilenames.pop();

            std::ifstream candidate1(tmpPath + candidate1Name, std::ios::binary);
            std::ifstream candidate2(tmpPath + candidate2Name, std::ios::binary);
            std::string tmpName = std::tmpnam(nullptr);
            this->output.open(tmpPath + tmpName, std::ios::binary);

            mergeChunks(candidate1, candidate2);

            this->sortedChunkFilenames.push(std::move(tmpName));

            std::remove((tmpPath + candidate1Name).c_str());
            std::remove((tmpPath + candidate2Name).c_str());
        }

        std::string candidate1Name = this->sortedChunkFilenames.top();
        this->sortedChunkFilenames.pop();
        std::string candidate2Name = this->sortedChunkFilenames.top();
        this->sortedChunkFilenames.pop();

        std::ifstream candidate1(tmpPath + candidate1Name, std::ios::binary);
        std::ifstream candidate2(tmpPath + candidate2Name, std::ios::binary);
        this->output.open(this->outputPath, std::ios::binary);

        mergeChunks(candidate1, candidate2);

        std::remove((tmpPath + candidate1Name).c_str());
        std::remove((tmpPath + candidate2Name).c_str());
    }

    void mergeChunks(std::ifstream& candidate1, std::ifstream& candidate2) {
        T value1;
        T value2;
        candidate1.read(reinterpret_cast<char *>(&value1), sizeof(value1));
        candidate2.read(reinterpret_cast<char *>(&value2), sizeof(value2));
        while (!candidate1.eof() && !candidate2.eof()) {
            if (value1 < value2) {
                this->output.write(reinterpret_cast<char *>(&value1), sizeof(value1));
                candidate1.read(reinterpret_cast<char *>(&value1), sizeof(value1));
            } else {
                this->output.write(reinterpret_cast<char *>(&value2), sizeof(value2));
                candidate2.read(reinterpret_cast<char *>(&value2), sizeof(value2));
            }
        }

        while (!candidate1.eof()) {
            this->output.write(reinterpret_cast<char *>(&value1), sizeof(value1));
            candidate1.read(reinterpret_cast<char *>(&value1), sizeof(value1));
        }

        while (!candidate2.eof()) {
            this->output.write(reinterpret_cast<char *>(&value2), sizeof(value2));
            candidate2.read(reinterpret_cast<char *>(&value2), sizeof(value2));
        }

        candidate1.close();
        candidate2.close();
        this->output.close();
    }

    const std::string tmpPath = "/var";
    const std::string inputPath;
    const std::string outputPath;
    const size_t chunkSize;
    std::ifstream input;
    std::ofstream output;
    std::stack<std::string> sortedChunkFilenames;
};

int main()
{
    FileSort<uint64_t> fileSort("data", "result", 100000);
    fileSort.startSorting();
    return 0;
}
