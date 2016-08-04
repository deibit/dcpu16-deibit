//
//  cpu.cpp
//  dcpu-deibit
//
//  Created by david on 26/7/16.
//
//
#include "cpu.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include "memory.h"

void Context::reset() {
    for (auto& reg : registers) reg = 0;
}

word& Context::operator[](word i) {
    if (i < 0x08) {
        return registers[i];
    } else
        switch (i) {
            case SP:
                return registers[0x08];
            case PC:
                return registers[0x09];
            case EX:
                return registers[0x0a];
            case IA:
                return registers[0x0b];
        }
    std::cerr << "Register access violation: " << i << " does not exists"
              << std::endl;
    exit(EXIT_FAILURE);
}

CPU::CPU()
    : cycles{0}, halt{false}, ctx{new Context}, memory{new Memory}, iq{256} {}

CPU::~CPU() {
    delete memory;
    delete ctx;
}

void CPU::boot(const std::string filename) { memory->load(filename); }

void CPU::reset() {
    ctx->reset();
    delete memory;
    memory = nullptr;
}

void CPU::dump() {
    std::cout << "A  :" << std::hex << (*ctx)[A] << std::endl
              << "B  :" << (*ctx)[B] << std::endl
              << "C  :" << (*ctx)[C] << std::endl
              << "X  :" << (*ctx)[X] << std::endl
              << "Y  :" << (*ctx)[Y] << std::endl
              << "Z  :" << (*ctx)[Z] << std::endl
              << "I  :" << (*ctx)[I] << std::endl
              << "J  :" << (*ctx)[J] << std::endl
              << "SP :" << (*ctx)[SP] << std::endl
              << "PC :" << (*ctx)[PC] << std::endl
              << "EX :" << (*ctx)[EX] << std::endl
              << "IA :" << (*ctx)[IA] << std::endl;
}

void CPU::run() {
    ctx->reset();
    if (!memory) {
        std::cerr << "Error no memory found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!halt) {
        cycles += step();
        dump();
    }

    std::cout << "CPU halted. Program terminated" << std::endl;
}

/*
 *  Fetch a word from memory, next increment PC
 */
word CPU::fetch() {
    word next_word = (*memory)[(*ctx)[PC]];
    (*ctx)[PC]++;
    next_word = (next_word << 8 | next_word >> 8);
    std::cout << std::hex << "Fetched: " << next_word << std::endl;
    return next_word;
}

void CPU::skip() {
    word skipped = fetch();
    Instruction instr = decode(skipped);
    auto f = [=](word test) {
        switch (test) {
            case 0x1a:
            case 0x1e:
            case 0x1f:
                fetch();
                break;
            default:
                if (0x10 <= test && test <= 0x17) fetch();
                break;
        }
    };
    f(instr.a);
    f(instr.b);
}

const Instruction CPU::decode(const word instr) {
    word a = static_cast<word>(mask_6 & (instr >> 10));
    word b = static_cast<word>(mask_5 & (instr >> 5));
    word o = static_cast<word>(mask_5 & (instr));

    return {.a = a, .b = b, .o = o};
}

word& CPU::decode_value(const word operand) {
    word next_word = 0x00;
    switch (operand) {
        // Inmediate mode / register
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x1b:
        case 0x1c:
        case 0x1d:
            return (*ctx)[operand];

        // Direct (register) mode / [register]
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
            return (*memory)[(*ctx)[operand & 0x07]];

        // Indirect [Register + next word]
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            cycles += 1;
            next_word = (*ctx)[PC];
            return (*memory)[(*ctx)[operand & 0x07] + fetch()];

        // [SP] / PEEK n
        case 0x19:
            return (*ctx)[SP];

        // [SP + next word] / PICK n
        case 0x1a:
            cycles += 1;
            return (*memory)[(*ctx)[SP] + fetch()];

        // [next word]
        case 0x1e:
            cycles += 1;
            return (*memory)[fetch()];
        default:
            std::cerr << "Value error: " << operand << " not recognized"
                      << std::endl;
            exit(EXIT_FAILURE);
    }
}

word CPU::decode_hardcoded(const word operand) {
    switch (operand) {
        case 0x1f:
            cycles += 1;
            return fetch();
        default:
            return operand - 0x21;
    }
}

/*
 *  Make a step, return the cycles of the instruction
 */
unsigned CPU::step() {
    Instruction instr = decode(fetch());
    std::cout << "Op :" << std::hex << instr.o << std::endl;
    std::cout << "a  :" << std::hex << instr.a << std::endl;
    std::cout << "b  :" << std::hex << instr.b << std::endl;

    // First we deal with 0x18 operands (not a friend of opcoded in value/ref)
    if (instr.b == 0x18) {  // PUSH / [--SP]
        (*ctx)[SP]--;
        (*memory)[(*ctx)[SP]] = decode_value(instr.a);
        return 0;
    }
    if (instr.a == 0x18) {  // POP / [SP++]
        (*memory)[(*ctx)[SP]] = decode_value(instr.b);
        (*ctx)[SP]++;
        return 0;
    }

    word a = 0x00;
    if (instr.a >= 0x1f) {
        a = decode_hardcoded(instr.a);
    } else {
        a = decode_value(instr.a);
    }

    // "The Great Switch"
    int overflow = 0;
    switch (instr.o) {
        case 0x00:
            cycles += special(instr, a);
            break;
        case 0x01:  // SET b, a
            decode_value(instr.b) = a;
            cycles += 1;
            break;

        // Arithmetics
        case 0x02:  // ADD b, a
            overflow = decode_value(instr.b) + a;
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x03:  // SUB b, a
            overflow = decode_value(instr.b) - a;
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x04:  // MUL b, a
            overflow = decode_value(instr.b) * a;
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x05:  // MLI b, a
            overflow = static_cast<int16_t>(decode_value(instr.b)) *
                       static_cast<int16_t>(a);
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x06:  // DIV b, a
            if (a == 0) {
                (*ctx)[EX] = 0;
            } else {
                overflow = (static_cast<uint>(decode_value(instr.b)) << 16) / a;
                (*ctx)[EX] = overflow;
                decode_value(instr.b) = overflow >> 16;
            }
            cycles += 3;
            break;
        case 0x07:  // DVI b, a
            if (a == 0) {
                (*ctx)[EX] = 0;
            } else {
                overflow = (static_cast<int>(decode_value(instr.b)) << 16) /
                           static_cast<int16_t>(a);
                (*ctx)[EX] = overflow;
                decode_value(instr.b) = overflow >> 16;
            }
            cycles += 3;
            break;

        // Logical
        case 0x08:  // MOD b, a
            decode_value(instr.b) %= a;
            cycles += 3;
            break;
        case 0x09:  // MDI b, a
            decode_value(instr.b) =
                static_cast<int16_t>(decode_value(instr.b)) % a;
            cycles += 3;
            break;
        case 0x0a:  // AND b, a
            decode_value(instr.b) &= a;
            cycles += 1;
            break;
        case 0x0b:  // BOR b, a
            decode_value(instr.b) |= a;
            cycles += 1;
            break;
        case 0x0c:  // XOR b, a
            decode_value(instr.b) ^= a;
            cycles += 1;
            break;

        // Bit shifting
        case 0x0d:  // SHR b, a
            (*ctx)[EX] = (static_cast<uint>(decode_value(instr.b)) << 16) >> a;
            decode_value(instr.b) >>= a;
            cycles += 1;
            break;
        case 0x0e:  // ASR b, a
            (*ctx)[EX] = (static_cast<uint>(decode_value(instr.b)) << 16) >> a;
            decode_value(instr.b) =
                static_cast<int16_t>(decode_value(instr.b)) >> a;
            cycles += 1;
            break;
        case 0x0f:  // SHL b, a
            (*ctx)[EX] = (static_cast<uint>(decode_value(instr.b)) << a) >> 16;
            decode_value(instr.b) <<= a;
            cycles += 1;
            break;

        // Branching
        case 0x10:  // IFB b, a
            if (!((decode_value(instr.b) & a) != 0)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x11:  // IFC b, a
            if (!((decode_value(instr.b) & a) == 0)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x12:  // IFE b, a
            if (!(decode_value(instr.b) == a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x13:  // IFN b, a
            if (!(decode_value(instr.b) != a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x14:  // IFG b, a
            if (!(decode_value(instr.b) > a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x15:  // IFA b, a
            if (!(decode_value(instr.b) > a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x16:  // IFL b, a
            if (!(decode_value(instr.b) < a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x17:  // IFU b, a
            if (!(decode_value(instr.b) < a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;

        // Other setters
        case 0x1a:  // ADX b, a
            overflow = decode_value(instr.b) + a + (*ctx)[EX];
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 3;
            break;
        case 0x1b:  // SBX b, a
            overflow = decode_value(instr.b) - a + (*ctx)[EX];
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 3;
            break;
        case 0x1e:  // STI b, a
            decode_value(instr.b) = a;
            (*ctx)[I]++;
            (*ctx)[J]++;
            cycles += 2;
            break;
        case 0x1f:  // STD b, a
            decode_value(instr.b) = a;
            (*ctx)[I]--;
            (*ctx)[J]--;
            cycles += 2;
            break;

        default:
            std::cerr << "Opcode error: " << instr.o << " not recognized"
                      << std::endl;
            exit(EXIT_FAILURE);
    }

    if (((*ctx)[PC] == Memory::MAX_MEM - 1) ||
        ((instr.a | instr.b | instr.o) == 0x0000)) {
        std::cout << instr.b << " " << instr.a << "  " << instr.o << std::endl;
        halt = true;
        return 0;
    }
    return 0;
}

unsigned CPU::special(const Instruction& instr, word a) {
    switch (instr.b) {
        case 0x01:
            (*ctx)[SP]--;
            (*memory)[(*ctx)[SP]] = (*ctx)[SP] + 1;
            (*ctx)[PC] = a;
            return 3;
        case 0x08:
            return 4;
        case 0x09:
            return 1;
        case 0x0a:
            return 1;
        case 0x0b:
            return 3;
        case 0x0c:
            return 2;
        case 0x10:
            return 2;
        case 0x11:
            return 4;
        case 0x12:
            return 4;

        default:
            return 0;
    }
}
