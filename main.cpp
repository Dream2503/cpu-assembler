#include <iostream>
#include "cpu.hpp"

int main() {
    ALU alu;
    Register a = INT16_MIN, b = -1;
    Register res = a;
    alu.SUB(res, b);

    std::cout << a << " - " << b << " = " << res << std::endl;
    std::cout << (int16_t)a << " - " << (int16_t)b << " = " << (int16_t)res << std::endl;
    std::cout << "Zero flag: " << alu.Z << std::endl;
    std::cout << "Negative flag: " << alu.N << std::endl;
    std::cout << "Overflow flag: " << alu.O << std::endl;
    return 0;
}
