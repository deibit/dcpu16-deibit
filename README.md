##DCPU16

DCPU16 is a fictional CPU created by [Markus Persson](https://twitter.com/notch) creator of [Minecraft](https://en.wikipedia.org/wiki/Minecraft), 
for the now cancelled game [0x10c](https://en.wikipedia.org/wiki/0x10c).

Last specification of the CPU was 1.7. It consisted of a 16 bits word computer with 64k of RAM and a basic instruction set.

There are lots of implementation of the DCPU16 throughout the Internet (GitHub is full of them). 
It happens that implementing a CPU emulator totally worth as an exercise for learning [computer architecture](https://en.wikipedia.org/wiki/Computer_architecture).

This is by no means an introductory emulator with the only function of learning the principles of computer systems by emulating them, so
don't take it as a serious project.

###Build

```bash
mkdir build
cd build
cmake ..
make
```

###Test
`./dcpu16-deibit ../test/sample.bin`

###Compiler

There is not compiler. You should bring your files assembled for now.
