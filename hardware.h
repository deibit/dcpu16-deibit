#include <string>

#ifndef hardware_h
#define hardware_h

class CPU;

class Hardware {
public:
  std::string name;
  uint32_t ID;
  uint16_t version;
  uint16_t manufacturer;
  CPU* cpu;

  virtual unsigned interrupt();
  virtual unsigned tick();
  void query();
  void setcpu(CPU*);
  virtual ~Hardware();
};

#endif // hardware_h
