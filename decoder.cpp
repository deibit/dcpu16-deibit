#include "decoder.h"

namespace decoder {

Instruction decode(const uint16_t instr) {
    uint16_t a = static_cast<uint16_t>(mask_6 & (instr >> 10));
    uint16_t b = static_cast<uint16_t>(mask_5 & (instr >> 5));
    uint16_t o = static_cast<uint16_t>(mask_5 & (instr));
    
    return {.a = a, .b = b, .o = o};
}

} // namespace decoder