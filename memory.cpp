#include <fstream>
#include <iostream>
#include "memory.h"

Memory::Memory() : memory{} {

}

void Memory::load(std::string filename) {
    try {
        std::ifstream input_file(filename,
                                 std::ios_base::in | std::ios_base::binary);

        // size_t size = input_file.;
        // if (size > MAX_INPUT) {
        //     std::cerr << "Error. File is above memory size" << std::endl;
        //     exit(EXIT_FAILURE);
        // }
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
