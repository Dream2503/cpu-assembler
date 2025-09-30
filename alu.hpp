#pragma once
#include "combinational_circuit.hpp"
#include "lsu.hpp"

/*
Arithmetic Logic Unit (ALU)

Provides basic arithmetic and logic operations on Register objects and updates standard ALU flags.

Flags:
- CF (Carry Flag): Set if an arithmetic carry/borrow occurs or during shift/rotate operations.
- ZF (Zero Flag): Set if the result of an operation is zero.
- SF (Sign Flag): Set to the most significant bit (MSB) of the result.
- OF (Overflow Flag): Set if signed overflow occurs in two's complement arithmetic.

Supported operations:
- ADD: Adds two registers using a ripple-carry adder.
- SUB: Subtracts two registers using two's complement addition.
- MUL: Multiplies two registers using shift-and-add method.
- INC/DEC: Increment or decrement a register by 1.
- NEG: Computes the two's complement negation of a register.
- SHL/SHR: Logical shift left/right.
- SAR: Arithmetic shift right.
- ROL/ROR: Rotate left/right.
- CMP: Compare two registers without modifying operands.
*/
class ALU {
public:
    Bit CF; // Carry Flag
    Bit ZF; // Zero Flag
    Bit SF; // Sign Flag
    Bit OF; // Overflow Flag

    /*
    Adds two registers and updates ALU flags.

    Performs bitwise addition of `lhs` and `rhs` using a ripple-carry adder.
    The result is stored in `lhs`.

    Flags updated:
    - ZF: Set to 1 if the result is zero.
    - SF: Set to the MSB of the result.
    - CF: Set to 1 if a carry out of MSB occurs.
    - OF: Set if signed overflow occurs.

    Parameters:
    - lhs: Left-hand side operand; stores the result.
    - rhs: Right-hand side operand.
    */
    constexpr void ADD(Register& lhs, const Register& rhs) noexcept {
        const Bit lhs_MSB_before = lhs.MSB();
        const Bit rhs_MSB = rhs.MSB();
        Bit carry = false;
        ZF = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [SUM, CARRY] = CombinationalCircuits::FULL_ADDER(lhs[i], rhs[i], carry);
            lhs[i] = SUM;
            carry = CARRY;

            if (SUM) {
                ZF = false;
            }
        }
        SF = lhs.MSB();
        CF = carry;
        OF = lhs_MSB_before == rhs_MSB & SF != lhs_MSB_before;
    }

    /*
    Subtracts rhs from lhs and updates ALU flags.

    Implements subtraction via two's complement addition:
        lhs = lhs + (~rhs + 1)
    Operates in-place.

    Flags updated:
    - ZF: Set to 1 if the result is zero.
    - SF: Set to MSB of the result.
    - CF: Set to 1 if borrow occurs from MSB.
    - OF: Set if signed overflow occurs.

    Parameters:
    - lhs: Left-hand side operand; stores the result.
    - rhs: Right-hand side operand.
    */
    constexpr void SUB(Register& lhs, const Register& rhs) noexcept {
        const Bit lhs_MSB_before = lhs.MSB();
        const Bit rhs_MSB = rhs.MSB();
        Bit carry = true;
        ZF = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [SUM, CARRY] = CombinationalCircuits::FULL_ADDER(lhs[i], ~rhs[i], carry);
            lhs[i] = SUM;
            carry = CARRY;

            if (SUM) {
                ZF = false;
            }
        }
        SF = lhs.MSB();
        CF = ~carry;
        OF = lhs_MSB_before != rhs_MSB & SF != lhs_MSB_before;
    }

    /*
    Multiplies two registers using shift-and-add, storing the result in lhs.

    Flags updated indirectly via ADD:
    - ZF, SF, CF, OF

    Parameters:
    - lhs: Multiplicand; will store the result.
    - rhs: Multiplier; read-only.
    - temp: Temporary register for shifting and accumulation; provided by caller.
    - zero: Temporary register representing zero; used for SHL flag updates.
    */
    constexpr void MUL(Register& lhs, const Register& rhs, Register& temp, const Register& zero) noexcept {
        LSU::MOV(temp, lhs);
        LSU::MOV(lhs, zero);

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            if (rhs[i]) {
                ADD(lhs, temp);
            }
            SHL(temp, 1, zero, temp);
        }
    }

    /*
    Integer division of lhs by rhs using repeated subtraction.

    Flags updated:
    - ZF: Set if quotient is zero.
    - SF: MSB of quotient.
    - CF: 0 (carry not affected in this SOC design)
    - OF: 0 (no signed overflow detection)

    Parameters:
    - lhs: Dividend; will store quotient after division.
    - rhs: Divisor; read-only.
    - quotient: Temporary register to store quotient; provided by caller.
    - temp: Temporary register for repeated subtraction; provided by caller.
    - zero: Zero register; provided by caller.
    */
    constexpr void DIV(Register& lhs, const Register& rhs, Register& quotient, Register& temp, const Register& zero) noexcept {
        CMP(rhs, zero, temp);

        if (ZF) {
            LSU::MOV(lhs, zero);
            ZF = CF = OF = true;
            SF = false;
            return;
        }
        LSU::MOV(quotient, zero);
        LSU::MOV(temp, lhs);

        while (true) {
            SUB(temp, rhs);

            if (CF) {
                ADD(temp, rhs);
                break;
            }
            INC(quotient);
        }
        LSU::MOV(lhs, quotient);
        SF = lhs.MSB();
        CMP(lhs, zero, temp);
        CF = false;
        OF = false;
    }

    /*
    Increments a register by 1 using ripple-carry addition.

    Flags updated:
    - ZF: Set if result is zero.
    - SF: MSB of the result.
    - OF: Set if signed overflow occurs (incrementing most positive value).
    - CF: Unaffected (like x86 INC instruction).

    Parameters:
    - reg: Register to increment; stores the result.
    */
    constexpr void INC(Register& reg) noexcept {
        const Bit MSB_before = reg.MSB();
        Bit carry = true;
        ZF = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [SUM, CARRY] = CombinationalCircuits::FULL_ADDER(reg[i], false, carry);
            reg[i] = SUM;
            carry = CARRY;

            if (SUM) {
                ZF = false;
            }
            if (!CARRY) {
                break;
            }
        }
        SF = reg.MSB();
        OF = MSB_before == false & SF == true;
    }

    /*
    Decrements a register by 1 using ripple-carry addition of -1.

    Flags updated:
    - ZF: Set if result is zero.
    - SF: MSB of the result.
    - OF: Set if signed overflow occurs (decrementing most negative value).
    - CF: Unaffected (like x86 DEC instruction).

    Parameters:
    - reg: Register to decrement; stores the result.
    */
    constexpr void DEC(Register& reg) noexcept {
        const Bit MSB_before = reg.MSB();
        Bit carry = true;
        ZF = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [SUM, CARRY] = CombinationalCircuits::FULL_ADDER(reg[i], true, carry);
            reg[i] = SUM;
            carry = CARRY;

            if (SUM) {
                ZF = false;
            }
            if (!CARRY) {
                break;
            }
        }
        SF = reg.MSB();
        OF = MSB_before == true && SF == false;
    }

    /*
    Negates a register (two's complement) and updates flags.

    Flags updated:
    - ZF: Set if result is zero.
    - SF: MSB of result.
    - CF: Set if original value was non-zero.
    - OF: Set if negating the most negative value.

    Parameters:
    - reg: Register to negate; stores the result.
    - temp: Temporary register; provided by caller.
    - zero: Temporary zero register for flag updates.
    */
    constexpr void NEG(Register& reg, Register& temp, const Register& zero) noexcept {
        LSU::MOV(temp, zero);
        SUB(temp, reg);
        LSU::MOV(reg, temp);
        CMP(reg, zero, temp);
        CF = !ZF;
        OF = reg.MSB() && ZF;
    }

    /*
    Logical left shift (SHL) by `count` bits.

    Flags updated:
    - CF: Last bit shifted out of MSB.
    - ZF: Set if result is zero.
    - SF: MSB of result.
    - OF: For 1-bit shifts, OF = SF XOR CF; otherwise 0.

    Parameters:
    - reg: Register to shift; stores the result.
    - count: Number of bits to shift.
    - zero: Zero register for flag comparison.
    - temp: Temporary register for CMP; provided by caller.
    */
    constexpr void SHL(Register& reg, const uint8_t count, const Register& zero, Register& temp) noexcept {
        if (count == 0) {
            SF = reg.MSB();
            CMP(reg, zero, temp);
            OF = CF = false;
            return;
        }
        if (count >= ARCHITECTURE) {
            CF = reg[ARCHITECTURE - 1];
            LSU::MOV(reg, zero);
            SF = false;
            CMP(reg, zero, temp);
            OF = false;
            return;
        }
        CF = reg[ARCHITECTURE - count];

        for (int8_t i = ARCHITECTURE - 1 - count; i >= 0; i--) {
            reg[i + count] = reg[i];
        }
        for (int8_t i = 0; i < count; i++) {
            reg[i] = false;
        }
        SF = reg.MSB();
        CMP(reg, zero, temp);
        OF = count == 1 ? SF ^ CF : false;
    }

    /*
    Logical right shift (SHR) by `count` bits.

    Flags updated:
    - CF: Last bit shifted out of LSB.
    - ZF: Set if result is zero.
    - SF: MSB of result.
    - OF: Always 0 for logical right shifts.

    Parameters:
    - reg: Register to shift; stores the result.
    - count: Number of bits to shift.
    - zero: Zero register for flag comparison.
    - temp: Temporary register for CMP; provided by caller.
    */
    constexpr void SHR(Register& reg, const uint8_t count, const Register& zero, Register& temp) noexcept {
        if (count == 0) {
            SF = reg.MSB();
            CMP(reg, zero, temp);
            OF = CF = false;
            return;
        }
        if (count >= ARCHITECTURE) {
            CF = reg[0];
            LSU::MOV(reg, zero);
            SF = false;
            CMP(reg, zero, temp);
            OF = false;
            return;
        }
        CF = reg[count - 1];

        for (uint8_t i = 0; i < ARCHITECTURE - count; i++) {
            reg[i] = reg[i + count];
        }
        for (uint8_t i = ARCHITECTURE - count; i < ARCHITECTURE; i++) {
            reg[i] = false;
        }
        SF = reg.MSB();
        CMP(reg, zero, temp);
        OF = false;
    }

    /*
    Arithmetic right shift (SAR) by `count` bits (sign-preserving).

    Flags updated:
    - CF: Last bit shifted out of LSB.
    - ZF: Set if result is zero.
    - SF: MSB of result (sign-preserving).
    - OF: Always 0 for arithmetic right shifts.

    Parameters:
    - reg: Register to shift; stores the result.
    - count: Number of bits to shift.
    - zero: Zero register for flag comparison.
    - temp: Temporary register for CMP; provided by caller.
    */
    constexpr void SAR(Register& reg, const uint8_t count, const Register& zero, Register& temp) noexcept {
        if (count == 0) {
            SF = reg.MSB();
            CMP(reg, zero, temp);
            OF = CF = false;
            return;
        }
        const Bit sign = reg.MSB();

        if (count >= ARCHITECTURE) {
            CF = reg[0];

            for (uint8_t i = 0; i < ARCHITECTURE; i++) {
                reg[i] = sign;
            }
            SF = sign;
            CMP(reg, zero, temp);
            OF = false;
            return;
        }
        CF = reg[count - 1];

        for (uint8_t i = 0; i < ARCHITECTURE - count; i++) {
            reg[i] = reg[i + count];
        }
        for (uint8_t i = ARCHITECTURE - count; i < ARCHITECTURE; i++) {
            reg[i] = sign;
        }
        SF = reg.MSB();
        CMP(reg, zero, temp);
        OF = false;
    }

    /*
    Rotate left (ROL) by `count` bits.

    Flags updated:
    - CF: Last bit rotated out of MSB.
    - ZF: Set if result is zero.
    - SF: MSB of result.
    - OF: For 1-bit rotates, OF = SF XOR CF; otherwise 0.

    Parameters:
    - reg: Register to rotate; stores the result.
    - count: Number of bits to rotate.
    - zero: Zero register for flag comparison.
    - temp: Temporary register for CMP; provided by caller.
    */
    constexpr void ROL(Register& reg, uint8_t count, const Register& zero, Register& temp) noexcept {
        count %= ARCHITECTURE;

        if (count == 0) {
            SF = reg.MSB();
            CMP(reg, zero, temp);
            OF = CF = false;
            return;
        }
        for (uint8_t c = 0; c < count; c++) {
            const Bit msb = reg[ARCHITECTURE - 1];

            for (int8_t i = ARCHITECTURE - 1; i > 0; i--) {
                reg[i] = reg[i - 1];
            }
            reg[0] = msb;
            CF = msb;
        }
        SF = reg.MSB();
        CMP(reg, zero, temp);
        OF = count == 1 ? SF ^ CF : false;
    }

    /*
    Rotate right (ROR) by `count` bits.

    Flags updated:
    - CF: Last bit rotated out of LSB.
    - ZF: Set if result is zero.
    - SF: MSB of result.
    - OF: For 1-bit rotates, OF = MSB XOR next MSB; otherwise 0.

    Parameters:
    - reg: Register to rotate; stores the result.
    - count: Number of bits to rotate.
    - zero: Zero register for flag comparison.
    - temp: Temporary register for CMP; provided by caller.
    */
    constexpr void ROR(Register& reg, uint8_t count, const Register& zero, Register& temp) noexcept {
        count %= ARCHITECTURE;

        if (count == 0) {
            SF = reg.MSB();
            CMP(reg, zero, temp);
            OF = CF = false;
            return;
        }
        for (uint8_t c = 0; c < count; c++) {
            const Bit lsb = reg[0];

            for (uint8_t i = 0; i < ARCHITECTURE - 1; i++) {
                reg[i] = reg[i + 1];
            }
            reg[ARCHITECTURE - 1] = lsb;
            CF = lsb;
        }
        SF = reg.MSB();
        CMP(reg, zero, temp);
        OF = count == 1 ? reg[ARCHITECTURE - 1] ^ reg[ARCHITECTURE - 2] : false;
    }

    /*
    Compares lhs and rhs without modifying them.

    Performs a virtual subtraction: temp = lhs - rhs, then updates flags
    (ZF, SF, CF, OF) as if SUB(lhs, rhs) was executed.

    Flags updated:
    - ZF: Set if lhs == rhs; cleared otherwise.
    - SF: MSB of (lhs - rhs).
    - CF: Set if a borrow occurs from the MSB.
    - OF: Set if signed overflow occurs during subtraction.

    Parameters:
    - lhs: Left-hand operand (read-only).
    - rhs: Right-hand operand (read-only).
    - temp: Temporary register provided by caller; used for computation.
    */
    constexpr void CMP(const Register& lhs, const Register& rhs, Register& temp) noexcept {
        LSU::MOV(temp, lhs);
        SUB(temp, rhs);
    }
};
