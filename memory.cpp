#include <cstdlib>
#include <fstream>
#include <iostream>
#include "memory.h"

using namespace std;

void Memory::load(std::string filename) {
    try {
        std::ifstream input_file(filename,
                                 std::ios_base::in | std::ios_base::binary);

        input_file.read(reinterpret_cast<char *>(&memory[0]), MAX_MEM);
        input_file.close();

    } catch (...) {
        cout << "Loading file error" << endl;
        exit(EXIT_FAILURE);
    }
}

word& Memory::operator[] (const word i) {
    //TODO Signal error when i is out of range
    return memory[i];
}

void Memory::dump() {
    //TODO Improve dumper
    int counter = 0;

    for (auto &op : memory) {
        if (op == 0) break;
        counter++;
    }
    cout << counter << endl;
}
