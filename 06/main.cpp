#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <stack>
#include <cstring>


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

    std::memcpy(&result[it1 + it2], &array[left + it1], (mid - left - it1) * sizeof(array[0]));
    std::memcpy(&result[mid - left + it2], &array[mid + it2], (right - mid - it2) * sizeof(array[0]));
    std::memcpy(&array[left], &result[0], result.size() * sizeof(T));
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
    std::vector<T> leftHalf(mid);
    std::vector<T> rightHalf(array.size() - mid);

    std::memcpy(&leftHalf[0], &array[0], leftHalf.size() * sizeof(array[0]));
    std::memcpy(&rightHalf[0], &array[mid], rightHalf.size() * sizeof(array[0]));

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
             const size_t chunkSize) : inputPath_(std::move(inputPath)),
                                       outputPath_(std::move(outputPath)),
                                       chunkSize(chunkSize) {}

    void startSorting() {
        createSortedChunks();
        mergeSortedChunks();
    }

private:

    void readChunkFromFileToVec(std::vector<T>& chunkData, std::ifstream& input) {
        input.read(reinterpret_cast<char *>(&chunkData[0]),
                   chunkData.size() * sizeof(chunkData[0]));
    }

    void writeChunkFromVecToFile(std::vector<T>& chunkData,
                                 const std::string& outputPathLocal) {
        std::ofstream output(outputPathLocal, std::ios::binary);
        output.write(reinterpret_cast<const char *>(&chunkData[0]),
                     chunkData.size() * sizeof(chunkData[0]));
        output.close();
    }

    void createSortedChunks() {
        std::vector<T> chunkData(this->chunkSize);
        std::ifstream input(this->inputPath_, std::ios::binary);
        while (!input.eof()) {
            readChunkFromFileToVec(chunkData, input);
            mergeSortMT(chunkData);
            std::string tmpName = std::tmpnam(nullptr);
            writeChunkFromVecToFile(chunkData, tmpPath + tmpName);
            this->sortedChunkFilenames.push(std::move(tmpName));
        }
        input.close();
    }

    void mergeSortedChunks() {
        if (this->sortedChunkFilenames.size() == 1) {
            std::string candidateName = this->sortedChunkFilenames.top();
            this->sortedChunkFilenames.pop();

            std::ifstream candidate(tmpPath + candidateName, std::ios::binary);
            std::ofstream output(this->outputPath_, std::ios::binary);

            T tmp;
            while (!candidate.eof()) {
                candidate.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
                output.write(reinterpret_cast<char *>(&tmp), sizeof(tmp));
            }

            candidate.close();
            output.close();
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

            mergeChunks(candidate1, candidate2, tmpPath + tmpName);

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

        mergeChunks(candidate1, candidate2, this->outputPath_);

        std::remove((tmpPath + candidate1Name).c_str());
        std::remove((tmpPath + candidate2Name).c_str());
    }

    void mergeChunks(std::ifstream& candidate1, std::ifstream& candidate2,
                     const std::string& outputPathLocal) {
        T value1;
        T value2;
        std::ofstream output(outputPathLocal, std::ios::binary);
        candidate1.read(reinterpret_cast<char *>(&value1), sizeof(value1));
        candidate2.read(reinterpret_cast<char *>(&value2), sizeof(value2));
        while (!candidate1.eof() && !candidate2.eof()) {
            if (value1 < value2) {
                output.write(reinterpret_cast<char *>(&value1), sizeof(value1));
                candidate1.read(reinterpret_cast<char *>(&value1), sizeof(value1));
            } else {
                output.write(reinterpret_cast<char *>(&value2), sizeof(value2));
                candidate2.read(reinterpret_cast<char *>(&value2), sizeof(value2));
            }
        }

        while (!candidate1.eof()) {
            output.write(reinterpret_cast<char *>(&value1), sizeof(value1));
            candidate1.read(reinterpret_cast<char *>(&value1), sizeof(value1));
        }

        while (!candidate2.eof()) {
            output.write(reinterpret_cast<char *>(&value2), sizeof(value2));
            candidate2.read(reinterpret_cast<char *>(&value2), sizeof(value2));
        }

        candidate1.close();
        candidate2.close();
        output.close();
    }

    const std::string tmpPath = "/var";
    const std::string inputPath_;
    const std::string outputPath_;
    const size_t chunkSize;
    std::stack<std::string> sortedChunkFilenames;
};

int main()
{
    FileSort<uint64_t> fileSort("data.dat", "result.dat", 100000);
    fileSort.startSorting();
    return 0;
}
