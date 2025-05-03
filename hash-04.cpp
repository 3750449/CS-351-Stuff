
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

#include <barrier>
#include <mutex>
#include <thread>
#include <vector>

const char *path = DATA_PATH "/Data.bin";
const size_t DefaultNumThreads = 8;

template <typename Type> 
Type *loadData(const char *path) {
    int fd = open(path, O_RDONLY);
    off_t size = lseek(fd, 0, SEEK_END);

    void *memory = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

    return reinterpret_cast<Type *>(memory);
}

int main(int argc, char *argv[]) {
    size_t numThreads = argc > 1 ? std::stol(argv[1]) : DefaultNumThreads;

    using Byte = unsigned char;
    using Bytes = Byte *;
    Bytes data = loadData<Byte>(path);

    size_t numHashes = *reinterpret_cast<size_t *>(data);
    data += sizeof(numHashes);

    using Hash = unsigned long long;
    using Hashes = std::vector<Hash>;
    Hashes hashes(numHashes);

    size_t hashID = 0;

    std::mutex getHashData;
    std::mutex storeHash;
    std::barrier barrier(numThreads);

    --numThreads;
    for (size_t id = 0; id <= numThreads; ++id) {
        std::thread t([&]() {
            while (hashID < numHashes) {
                size_t numBytes;
                size_t myHashID;
                Bytes myHashData;
            
                {
                    std::lock_guard lock{getHashData};
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
                    std::lock_guard{storeHash};
                    hashes[myHashID] = hash;
                }
            }

            barrier.arrive_and_wait();
        });

        id < numThreads ? t.detach() : t.join();
    }

        std::string outpath{argv[0]};
        outpath += ".txt";
        std::ofstream output{outpath};

        for (auto hash : hashes) {
            output << hash << "\n";
        }

        output.close();
}