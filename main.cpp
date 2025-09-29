#include "cpu.hpp"
#include <iostream>

int main() {
    ALU alu;
    Register a = 50, b = -1;
    Register res = a;
    alu.INC(res);

    std::cout << a << " + " << b << " = " << res << std::endl;
    std::cout << (int16_t)a << " + " << (int16_t)b << " = " << (int16_t)res << std::endl;
    std::cout << "Zero flag: " << alu.ZF << std::endl;
    std::cout << "Negative flag: " << alu.SF << std::endl;
    std::cout << "Overflow flag: " << alu.OF << std::endl;
    return 0;
}
