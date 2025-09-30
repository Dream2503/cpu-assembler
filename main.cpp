#include <iostream>
#include "cpu.hpp"

int main() {
    ALU alu;
    Register* regs = Register::instantiate_register_set();
    Register& zero = regs[15]; // last register used as zero
    Register& temp = regs[14]; // second-last register as temporary

    // MOV test
    LSU::MOV(regs[0], 50);
    LSU::MOV(regs[1], -1);
    LSU::MOV(regs[2], regs[0]);
    std::cout << "MOV tests:\n";
    std::cout << "reg0 = " << static_cast<int16_t>(regs[0]) << ", reg1 = " << static_cast<int16_t>(regs[1])
              << ", reg2 = " << static_cast<int16_t>(regs[2]) << std::endl;

    // INC / DEC test
    alu.INC(regs[2]);
    alu.DEC(regs[0]);
    std::cout << "\nINC/DEC tests:\n";
    std::cout << "reg2 after INC = " << static_cast<int16_t>(regs[2]) << ", reg0 after DEC = " << static_cast<int16_t>(regs[0]) << std::endl;

    // ADD test
    alu.ADD(regs[3], regs[0]);
    alu.ADD(regs[3], regs[1]);
    std::cout << "\nADD test:\n";
    std::cout << "reg3 = reg0 + reg1 = " << static_cast<int16_t>(regs[3]) << std::endl;

    // SUB test
    alu.SUB(regs[4], regs[0]);
    alu.SUB(regs[4], regs[1]);
    std::cout << "\nSUB test:\n";
    std::cout << "reg4 = 0 - reg0 - reg1 = " << static_cast<int16_t>(regs[4]) << std::endl;

    // NEG test
    LSU::MOV(regs[5], 42);
    alu.NEG(regs[5], temp, zero);
    std::cout << "\nNEG test:\n";
    std::cout << "reg5 (neg of 42) = " << static_cast<int16_t>(regs[5]) << std::endl;

    // MUL test
    LSU::MOV(regs[6], 6);
    LSU::MOV(regs[7], 7);
    alu.MUL(regs[6], regs[7], temp, zero);
    std::cout << "\nMUL test:\n";
    std::cout << "6 * 7 = " << static_cast<int16_t>(regs[6]) << std::endl;

    // DIV test
    LSU::MOV(regs[8], 42);
    LSU::MOV(regs[9], 4);

    alu.DIV(regs[8], regs[9], regs[14], regs[15], regs[13]);

    std::cout << "\nDIV test:\n";
    std::cout << "42 / 7 = " << static_cast<int16_t>(regs[8]) << std::endl;


    // SHL / SHR / SAR test
    LSU::MOV(regs[10], 0b10101010);
    alu.SHL(regs[10], 1, zero, temp);
    std::cout << "\nSHL test:\n";
    std::cout << "reg10 << 1 = " << static_cast<int16_t>(regs[10]) << std::endl;

    alu.SHR(regs[10], 2, zero, temp);
    std::cout << "reg10 >> 2 = " << static_cast<int16_t>(regs[10]) << std::endl;

    LSU::MOV(regs[10], 0b10000000);
    alu.SAR(regs[10], 2, zero, temp);
    std::cout << "SAR reg10 >> 2 = " << static_cast<int16_t>(regs[10]) << std::endl;

    // ROL / ROR test
    LSU::MOV(regs[11], 0b1001);
    alu.ROL(regs[11], 2, zero, temp);
    std::cout << "\nROL test:\n";
    std::cout << "ROL reg11 by 2 = " << static_cast<int16_t>(regs[11]) << std::endl;

    alu.ROR(regs[11], 1, zero, temp);
    std::cout << "ROR reg11 by 1 = " << static_cast<int16_t>(regs[11]) << std::endl;

    // CMP test
    LSU::MOV(regs[12], 100);
    LSU::MOV(regs[13], 100);
    alu.CMP(regs[12], regs[13], temp);
    std::cout << "\nCMP test:\n";
    std::cout << "CMP reg12 and reg13 -> ZF: " << static_cast<bool>(alu.ZF) << ", SF: " << static_cast<bool>(alu.SF) << std::endl;

    // Final flags
    std::cout << "\nFinal Flags:\n";
    std::cout << "ZF: " << static_cast<bool>(alu.ZF) << ", SF: " << static_cast<bool>(alu.SF) << ", CF: " << static_cast<bool>(alu.CF)
              << ", OF: " << static_cast<bool>(alu.OF) << std::endl;

    // Clean up
    delete[] regs;
    return 0;
}
