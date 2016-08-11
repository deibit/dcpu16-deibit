#include <string>

#pragma once

class CPU;

class Hardware {
public:
  std::string name;
  uint32_t ID;
  uint16_t version;
  uint16_t manufacturer;
  CPU* cpu;

  Hardware(std::string, uint32_t, uint16_t, uint16_t);
  virtual ~Hardware();

  virtual unsigned interrupt()=0;
  virtual unsigned tick()=0;

  void query();
  void setcpu(CPU*);
};
