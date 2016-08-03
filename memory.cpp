#include <fstream>
#include <iostream>
#include "memory.h"

void Memory::load(std::string filename) {
    try {
        std::ifstream input_file(filename,
                                 std::ios_base::in | std::ios_base::binary);

        input_file.read(reinterpret_cast<char *>(&memory[0]), MAX_MEM);
        input_file.close();

    } catch (...) {
        std::cerr << "Loading file error" << std::endl;
        exit(EXIT_FAILURE);
    }
}

word& Memory::operator[] (word i) {
    if (i > MAX_MEM - 1) {
        std::cerr << "Adress " << i << " is out of range" << std::endl;
        exit(EXIT_FAILURE);
    }
    return memory[i];
}

void Memory::dump() {
    //TODO Improve dumper
    int counter = 0;

    for (auto &op : memory) {
        if (op == 0) break;
        counter++;
    }
    std::cout << counter << std::endl;
}
