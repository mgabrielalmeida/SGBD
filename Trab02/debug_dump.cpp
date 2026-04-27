// debug_dump.cpp - Dump the contents of all buckets and directory
#include <iostream>
#include <fstream>
#include "directory.h"
#include "bucket.h"

int main() {
    Directory dir;
    if (!loadDirectory(&dir)) {
        std::cerr << "Erro: nao foi possivel carregar o indice" << std::endl;
        return 1;
    }

    std::cout << "============ DIRETORIO ============" << std::endl;
    std::cout << "Profundidade Global (PG): " << dir.globalDepth << std::endl;
    std::cout << "Tamanho: " << dir.size << std::endl;
    std::cout << "Proximo ID de Bucket: " << dir.nextBucketId << std::endl;

    for (int i = 0; i < dir.size; i++) {
        std::cout << "  dir[" << i << "] -> bucket_" << dir.bucketIds[i] << std::endl;
    }

    std::cout << std::endl;

    // Dump em cada bucket
    bool seen[100] = {false};
    for (int i = 0; i < dir.size; i++) {
        int bid = dir.bucketIds[i];
        if (seen[bid]) continue;
        seen[bid] = true;

        Bucket b;
        if (loadBucket(bid, &b)) {
            std::cout << "============ BUCKET " << bid << " ============" << std::endl;
            std::cout << "Profundidade Local (PL): " << b.localDepth << std::endl;
            std::cout << "Contagem: " << b.count << std::endl;
            std::cout << "Entradas: ";
            for (int j = 0; j < b.count; j++) {
                std::cout << b.entries[j].linhaNum << " ";
            }
            std::cout << std::endl;
        }
    }

    freeDirectory(&dir);
    return 0;
}
