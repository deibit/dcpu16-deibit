//
//  cpu.cpp
//  dcpu-deibit
//
//  Created by david on 26/7/16.
//
//
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std; //TODO delete this, only for debug purposes
#include "cpu.h"

CPU::~CPU() {
    delete memory;
    delete ctx;
}

void CPU::boot(const std::string filename) {
    memory->load(filename);
}

void CPU::reset() {
    ctx->reset();
    delete memory;
    memory = nullptr;
}

void CPU::dump() {
    cout <<  "A  :" << hex << (*ctx)[A] << endl;
    cout <<  "B  :" << (*ctx)[B] << endl;
    cout <<  "C  :" << (*ctx)[C] << endl;
    cout <<  "X  :" << (*ctx)[X] << endl;
    cout <<  "Y  :" << (*ctx)[Y] << endl;
    cout <<  "Z  :" << (*ctx)[Z] << endl;
    cout <<  "I  :" << (*ctx)[I] << endl;
    cout <<  "J  :" << (*ctx)[J] << endl;
    cout <<  "SP :" << (*ctx)[SP] << endl;
    cout <<  "PC :" << (*ctx)[PC] << endl;
    cout <<  "EX :" << (*ctx)[EX] << endl;
    cout <<  "IA :" << (*ctx)[IA] << endl;

}

void CPU::run() {
    ctx->reset();
    if (!memory) {
        exit(EXIT_FAILURE);
    }
    
    while(!halt) {
        cycles += step();
        dump();
    }
    
    cout << "CPU halted. Program terminate" << endl;
    
    
}

/*
 *  Fetch a word from memory, next increment PC
 */
word CPU::fetch() {
    word next_word = (*memory)[(*ctx)[PC]];
    (*ctx)[PC]++;
    next_word = (next_word << 8 | next_word >> 8);
    cout << hex << "Fetched: " << next_word << endl;
    return next_word;
}

void CPU::skip() {
    word skipped = fetch();
    decoder::Instruction instr = decoder::decode(skipped);
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


word& CPU::decode_value(const word operand) {
    word next_word = 0x00;
    switch (operand) {
            
        // Inmediate mode / register
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
        case 0x05: case 0x06: case 0x07: case 0x1b: case 0x1c:
        case 0x1d:
            return (*ctx)[operand];
        
        // Direct (register) mode / [register]
        case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c:
        case 0x0d: case 0x0e: case 0x0f:
            return (*memory)[(*ctx)[operand & 0x07]];
            
        // Indirect [Register + next word]
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14:
        case 0x15: case 0x16: case 0x17:
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
    decoder::Instruction instr = decoder::decode(fetch());
    cout << "Op :" << hex << instr.o << endl;
    cout << "a  :" << hex << instr.a << endl;
    cout << "b  :" << hex << instr.b << endl;
    
    
    // First we deal with 0x18 operands (not a friend of opcoded in value/ref)
    if (instr.b == 0x18) { // PUSH / [--SP]
        (*ctx)[SP]--;
        (*memory)[(*ctx)[SP]] = decode_value(instr.a);
        return 0;
    }
    if (instr.a == 0x18) { // POP / [SP++]
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
            // "The Little Great Switch"
            switch (instr.b) {
                case 0x01:
                    (*ctx)[SP]--;
                    (*memory)[(*ctx)[SP]] = (*ctx)[SP] + 1;
                    (*ctx)[PC] = a;
                    cycles += 3;
                    break;
                default:
                    break;
            }
            break;

        case 0x01: // SET b, a
            decode_value(instr.b) = a;
            cycles += 1;
            break;
            
        // Arithmetics
        case 0x02: // ADD b, a
            overflow = decode_value(instr.b) + a;
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x03: // SUB b, a
            overflow = decode_value(instr.b) - a;
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x04: // MUL b, a
            overflow = decode_value(instr.b) * a;
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x05: // MLI b, a
            overflow = static_cast<int16_t>(decode_value(instr.b)) * static_cast<int16_t>(a);
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 2;
            break;
        case 0x06: // DIV b, a
            if (a == 0) {
                (*ctx)[EX] = 0;
            } else {
                overflow = (static_cast<uint>(decode_value(instr.b)) << 16) / a;
                (*ctx)[EX] = overflow;
                decode_value(instr.b) = overflow >> 16;
            }
            cycles += 3;
            break;
        case 0x07: // DVI b, a
            if (a == 0) {
                (*ctx)[EX] = 0;
            } else {
                overflow = (static_cast<int>(decode_value(instr.b)) << 16) / static_cast<int16_t>(a);
                (*ctx)[EX] = overflow;
                decode_value(instr.b) = overflow >> 16;
            }
            cycles += 3;
            break;
            
        // Logical
        case 0x08: // MOD b, a
            decode_value(instr.b) %= a;
            cycles += 3;
            break;
        case 0x09: // MDI b, a
            decode_value(instr.b) = static_cast<int16_t>(decode_value(instr.b)) % a;
            cycles += 3;
            break;
        case 0x0a: // AND b, a
            decode_value(instr.b) &= a;
            cycles += 1;
            break;
        case 0x0b: // BOR b, a
            decode_value(instr.b) |= a;
            cycles += 1;
            break;
        case 0x0c: // XOR b, a
            decode_value(instr.b) ^= a;
            cycles += 1;
            break;
            
        // Bit shifting
        case 0x0d: // SHR b, a
            (*ctx)[EX] = (static_cast<uint>(decode_value(instr.b)) << 16) >> a;
            decode_value(instr.b) >>= a;
            cycles += 1;
            break;
        case 0x0e: // ASR b, a
            (*ctx)[EX] = (static_cast<uint>(decode_value(instr.b)) << 16) >> a;
            decode_value(instr.b) = static_cast<int16_t>(decode_value(instr.b)) >> a;
            cycles += 1;
            break;
        case 0x0f: // SHL b, a
            (*ctx)[EX] = (static_cast<uint>(decode_value(instr.b)) << a) >> 16;
            decode_value(instr.b) <<= a;
            cycles += 1;
            break;
            
        // Branching
        case 0x10: // IFB b, a
            if (!((decode_value(instr.b) & a) != 0)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x11: // IFC b, a
            if (!((decode_value(instr.b) & a) == 0)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x12: // IFE b, a
            if (!(decode_value(instr.b) == a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x13: // IFN b, a
            if (!(decode_value(instr.b) != a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x14: // IFG b, a
            if (!(decode_value(instr.b) > a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x15: // IFA b, a
            if (!(decode_value(instr.b) > a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x16: // IFL b, a
            if (!(decode_value(instr.b) < a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
        case 0x17: // IFU b, a
            if (!(decode_value(instr.b) < a)) {
                cycles += 3;
                skip();
            }
            cycles += 2;
            break;
            
        // Other setters
        case 0x1a: // ADX b, a
            overflow = decode_value(instr.b) + a + (*ctx)[EX];
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 3;
            break;
        case 0x1b: // SBX b, a
            overflow = decode_value(instr.b) - a + (*ctx)[EX];
            (*ctx)[EX] = overflow >> 16;
            decode_value(instr.b) = overflow;
            cycles += 3;
            break;
        case 0x1e: // STI b, a
            decode_value(instr.b) = a;
            (*ctx)[I]++;
            (*ctx)[J]++;
            cycles += 2;
            break;
        case 0x1f: // STD b, a
            decode_value(instr.b) = a;
            (*ctx)[I]--;
            (*ctx)[J]--;
            cycles += 2;
            break;
            
        default:
            break;
    }
    
    if (((*ctx)[PC] == Memory::MAX_MEM - 1) ||
        ((instr.a|instr.b|instr.o) == 0x0000)){
        cout << instr.b << " " << instr.a << "  " << instr.o << endl;
        halt = true;
        return 0;
    }
    return 0;
}
