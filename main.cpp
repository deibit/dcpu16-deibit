#include "cpu.h"

int main(int argc, char** argv) {
    CPU cpu;
    cpu.boot(argv[1]);
    cpu.run();
    return 0;
}
