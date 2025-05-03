#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

#include <mutex>
#include <thread>
#include <vector>

#include "ScopeTimer.h"
using Timer = ScopeTimer<seconds>;

#define DATA_PATH "."
//#define DATA_PATH "/home/faculty/shreiner/public_html/CS-351/Project-2"

const char *path = DATA_PATH "/Data.bin";
const size_t DefaultNumThreads = 8;

template <typename Type>
Type *loadData(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open data file");
        exit(EXIT_FAILURE);
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("Failed to get file size");
        exit(EXIT_FAILURE);
    }

    void *memory = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (memory == MAP_FAILED) {
        perror("Failed to mmap file");
        exit(EXIT_FAILURE);
    }

    return reinterpret_cast<Type *>(memory);
}

int main(int argc, char *argv[]) {
    Timer totalTimer{"total program"};
    Timer serialPreThreads{"serial before threads"};

    size_t numThreads = argc > 1 ? std::stol(argv[1]) : DefaultNumThreads;

    using Byte = unsigned char;
    using Bytes = Byte *;
    Bytes data = loadData<Byte>(path);

    size_t numHashes = *reinterpret_cast<size_t *>(data);
    if (numHashes == 0 || numHashes > 1000000000) {
        std::cerr << "Sanity check failed: numHashes = " << numHashes << std::endl;
        exit(EXIT_FAILURE);
    }

    data += sizeof(numHashes);

    using Hash = unsigned long long;
    using Hashes = std::vector<Hash>;
    Hashes hashes(numHashes);

    size_t hashID = 0;
    std::mutex getHashData;
    std::mutex storeHash;

    --numThreads;
    serialPreThreads.elapsed();

    for (size_t id = 0; id <= numThreads; ++id) {
        std::thread t([&]() {
            while (true) {
                size_t numBytes;
                size_t myHashID;
                Bytes myHashData;

                {
                    std::lock_guard lock{getHashData};
                    if (hashID >= numHashes) break;

                    myHashID = hashID++;
                    numBytes = *reinterpret_cast<size_t *>(data);
                    myHashData = data + sizeof(numBytes);
                    data += sizeof(numBytes) + numBytes;
                }

                Bytes bytes = reinterpret_cast<Bytes>(myHashData);

                Hash hash = 104395301;
                const Hash multiplier = 2654435789;
                for (auto j = 0; j < numBytes; ++j) {
                    hash += (multiplier * bytes[j]) ^ (hash >> 23);
                }

                {
                    std::lock_guard lock{storeHash};
                    hashes[myHashID] = hash;
                }
            }
        });

        id < numThreads ? t.detach() : t.join();
    }

    {
        Timer outputTimer{"results output"};
        std::string outpath{argv[0]};
        outpath += ".txt";
        std::ofstream output{outpath};

        for (auto hash : hashes) {
            output << hash << "\n";
        }

        output.close();
        outputTimer.elapsed();
    }

    totalTimer.elapsed();
    return 0;
}
