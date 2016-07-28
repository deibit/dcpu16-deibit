#include <cstdlib>
#include <fstream>
#include <iostream>
#include "decoder.h"
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
        decoder::Instruction instr = decoder::decode(op);
        std::cout << "OP:          " << op  << endl;
        std::cout << "Instruction: " << hex << instr.o << " ";
        std::cout << "Value for a: " << hex << instr.a << " ";
        std::cout << "Value for b: " << hex << instr.b << endl;
    }
    cout << counter << endl;
}