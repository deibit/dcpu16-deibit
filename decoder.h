#include <cstdint>

#ifndef decoder_h
#define decoder_h

namespace decoder {

const uint16_t mask_5 = 0x1f;  // 0b11111
const uint16_t mask_6 = 0x3f;  // 0b111111

struct Instruction {
  uint16_t a;
  uint16_t b;
  uint16_t o;
};

Instruction decode (const uint16_t);

} // namespace decoder


#endif /* decoder_h */
