#include <cstdlib>
#include <string>

#pragma once

using word = std::uint16_t;

class Memory {
   public:
    static constexpr unsigned MAX_MEM = 0x10000;

    Memory();
    word& operator[] (word);
    void load(std::string);
    void dump();

   private:
    word memory[MAX_MEM];
};

