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
#include <map>
#include <string>
#include <iostream>
#include <deque>
#include <vector>

using word = std::uint16_t;
// DEVICES {clock, screen}
static constexpr unsigned DEVICES = 2;

class Memory;
class Hardware;

enum : word {
    A = 0x00,
    B = 0x01,
    C = 0x02,
    X = 0x03,
    Y = 0x04,
    Z = 0x05,
    I = 0x06,
    J = 0x07,
    SP = 0x1b,
    PC = 0x1c,
    EX = 0x1d,
    IA = 0xd    // There is not hardcode value for IA in the specs
};

struct Instruction {
    word a;
    word b;
    word o;
};

struct Context {
    void reset();
    word& operator[](word);
    word registers[12] = {};
};

class CPU {
   public:
    CPU();
    virtual ~CPU();

    void boot(const std::string filename);
    void run();
    void reset();
    void dump();
    Context& context();

   private:
    unsigned cycles;
    bool halt;
    Memory* memory;
    bool queuing;
    Context* ctx;
    std::deque<int> iq;
    std::vector<Hardware*> devices;

    static const word mask_5 = 0x1f;  // 0b11111
    static const word mask_6 = 0x3f;  // 0b111111

    unsigned step();
    unsigned special(const Instruction&, word&);
    word fetch();
    void skip();
    const Instruction decode(word);
    //TODO  Probably we can use here a std::pair<word,word&>
    //      or similar and have just one function
    word& decode_value(word);
    word decode_hardcoded(word);
    unsigned interrupt();
};

#endif /* cpu_h */
