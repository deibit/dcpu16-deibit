//
//  cpu.h
//  dcpu-deibit
//
//  Created by david on 26/7/16.
//
//
#ifndef cpu_h
#define cpu_h

#include <cstdlib>
#include <string>
#include <map>
#include "memory.h"

class Memory;

enum : word {
                A = 0x00, B = 0x01, C = 0x02,
                X = 0x03, Y = 0x04, Z = 0x05,
                I = 0x06, J = 0x07,
                SP = 0x1b, PC = 0x1c, EX = 0x1d, IA = 0xd
            };

struct Context {
    void reset() {
        for (auto& reg: registers) reg = 0;
    }
    
    word& operator[](const word i) {
        if (i < 0x08) {
            return registers[i];
        } else switch (i) {
            case SP:
                return registers[0x08];
            case PC:
                return registers[0x09];
            case EX:
                return registers[0x0a];
            case IA:
                return registers[0x0b];
        }
        //TODO Signal error when 'i' will is out of range
        exit(EXIT_FAILURE);
    }
    
    word registers[12] = {};
};

class CPU {
public:
    CPU() : cycles{0}, halt{false},
    ctx{new Context}, memory{new Memory} {}
    ~CPU();
    
    void boot(const std::string filename);
    void run();
    void reset();
    
    void dump();
    
private:
    unsigned cycles;
    bool halt;
    Context *ctx;
    Memory *memory;
    
    unsigned step();
    word fetch();
    void skip();
    word& decode_value(const word);
    word decode_hardcoded(const word);
};



#endif /* cpu_h */
