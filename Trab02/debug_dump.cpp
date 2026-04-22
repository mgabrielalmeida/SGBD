// debug_dump.cpp - Dump the contents of all buckets and directory
#include <iostream>
#include <fstream>
#include "directory.h"
#include "bucket.h"

int main() {
    Directory dir;
    if (!loadDirectory(&dir)) {
        std::cerr << "Cannot load directory" << std::endl;
        return 1;
    }

    std::cout << "=== DIRECTORY ===" << std::endl;
    std::cout << "Global Depth (PG): " << dir.globalDepth << std::endl;
    std::cout << "Size: " << dir.size << std::endl;
    std::cout << "Next Bucket ID: " << dir.nextBucketId << std::endl;

    for (int i = 0; i < dir.size; i++) {
        std::cout << "  dir[" << i << "] -> bucket_" << dir.bucketIds[i] << std::endl;
    }

    std::cout << std::endl;

    // Dump each unique bucket
    bool seen[100] = {false};
    for (int i = 0; i < dir.size; i++) {
        int bid = dir.bucketIds[i];
        if (seen[bid]) continue;
        seen[bid] = true;

        Bucket b;
        if (loadBucket(bid, &b)) {
            std::cout << "=== BUCKET " << bid << " ===" << std::endl;
            std::cout << "  Local Depth (PL): " << b.localDepth << std::endl;
            std::cout << "  Count: " << b.count << std::endl;
            std::cout << "  Entries: ";
            for (int j = 0; j < b.count; j++) {
                std::cout << b.entries[j].linhaNum << " ";
            }
            std::cout << std::endl;
        }
    }

    freeDirectory(&dir);
    return 0;
}
