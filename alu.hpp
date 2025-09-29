#pragma once
#include "combinational_circuit.hpp"
#include "register.hpp"

/*
Arithmetic Logic Unit

Provides basic arithmetic operations on Register objects and updates standard ALU flags.

Flags:
- Z (Zero): Set if the result of an operation is zero.
- N (Negative): Set to the most significant bit (MSB) of the result.
- O (Overflow): Set if signed overflow occurs in two's complement arithmetic.

This ALU currently supports:
- ADD: Adds two registers using a ripple-carry adder.
- SUB: Subtracts two registers using a ripple-borrow subtractor.
- MUL: Multiplies two registers using a shift-and-add method.
*/
class ALU {
public:
    Bit CF; // Carry Flag
    Bit ZF; // Zero Flag
    Bit SF; // Sign Flag
    Bit OF; // Overflow Flag

    /*
    Adds two registers and updates ALU flags.

    Performs a bitwise addition of `lhs` and `rhs` using a ripple-carry adder.
    The result is stored in `lhs`.

    Flags updated:
    - ZF (Zero Flag): Set to 1 if the result is all zeros; otherwise 0.
    - SF (Sign Flag): Set to the most significant bit (MSB) of the result.
    - CF (Carry Flag): Set to 1 if there is a carry out of the most significant bit; otherwise 0.
    - OF (Overflow Flag): Set if signed overflow occurs in two's complement arithmetic.

    Parameters:
    - lhs: Left-hand side operand; will also store the result.
    - rhs: Right-hand side operand.
    */
    constexpr void ADD(Register& lhs, const Register& rhs) noexcept {
        const Bit lhs_MSB_before = lhs.MSB();
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
        OF = (lhs_MSB_before == rhs.MSB()) && (lhs_MSB_before != SF);
    }

    /*
    Subtracts two registers and updates ALU flags.

    Performs a bitwise subtraction of `rhs` from `lhs` using a ripple-borrow full subtractor.
    The result is stored in `lhs`.

    Flags updated:
    - ZF (Zero Flag): Set to 1 if the result is all zeros; otherwise 0.
    - SF (Sign Flag): Set to the most significant bit (MSB) of the result.
    - CF (Carry/Borrow Flag): Set to 1 if a borrow occurs from the most significant bit; otherwise 0.
    - OF (Overflow Flag): Set if signed overflow occurs in two's complement arithmetic.

    Parameters:
    - lhs: Left-hand side operand; will also store the result.
    - rhs: Right-hand side operand.
    */
    constexpr void SUB(Register& lhs, const Register& rhs) noexcept {
        const Bit lhs_MSB_before = lhs.MSB();
        Bit borrow = false;
        ZF = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [DIFFERENCE, BORROW] = CombinationalCircuits::FULL_SUBTRACTOR(lhs[i], rhs[i], borrow);
            lhs[i] = DIFFERENCE;
            borrow = BORROW;

            if (DIFFERENCE) {
                ZF = false;
            }
        }
        SF = lhs.MSB();
        CF = borrow;
        OF = (lhs_MSB_before != rhs.MSB()) && (lhs_MSB_before != SF);
    }

    /*
    Multiplies two registers and updates ALU flags.

    Performs a bitwise multiplication of `lhs` by `rhs` using the shift-and-add method.
    Each addition is performed using the `ADD` function, which updates ALU flags.
    The result is stored in `lhs`.

    Flags updated (via ADD):
    - ZF (Zero Flag): Set to 1 if the result is all zeros; otherwise 0.
    - SF (Sign Flag): Set to the most significant bit (MSB) of the result.
    - CF (Carry Flag): Set to 1 if a carry occurs in the last addition; otherwise 0.
    - OF (Overflow Flag): Set if signed overflow occurs in any addition step.

    Parameters:
    - lhs: Left-hand side operand; will also store the result.
    - rhs: Right-hand side operand.
    */
    constexpr void MUL(Register& lhs, const Register& rhs) noexcept {
        Register result, multiplicand = lhs;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            if (rhs[i]) {
                ADD(result, multiplicand);
            }
            multiplicand <<= 1;
        }
        lhs = result;
    }

    // constexpr void DIV(Register& lhs, const Register& rhs) noexcept {
    //     Handle division by zero
    //     if (rhs.isZero()) {
    //         Z = true;
    //         N = false;
    //         O = true; // Use overflow flag to indicate error
    //         lhs = {};
    //         return;
    //     }
    //
    //     // Record the sign of the result
    //     bool negative_result = lhs.MSB() ^ rhs.MSB();
    //
    //     // Take absolute values
    //     Register dividend = lhs;
    //     Register divisor = rhs;
    //     if (lhs.MSB()) dividend = TWO_COMPLEMENT(lhs);
    //     if (rhs.MSB()) divisor = TWO_COMPLEMENT(rhs);
    //
    //     Register quotient{};
    //     Register remainder{};
    //
    //     // Unsigned shift-and-subtract division
    //     for (int8_t i = ARCHITECTURE - 1; i >= 0; i--) {
    //         remainder <<= 1;
    //         remainder[0] = dividend[i];
    //
    //         Register temp = remainder;
    //         SUB(temp, divisor); // Updates flags
    //
    //         if (!N) { // If subtraction did not go negative
    //             remainder = temp;
    //             quotient[i] = true;
    //         }
    //     }
    //
    //     // Apply the sign
    //     if (negative_result) quotient = TWO_COMPLEMENT(quotient);
    //
    //     // Check for overflow: special case INT_MIN / -1
    //     if (lhs.MSB() && rhs.MSB() && quotient.MSB() == 0 && lhs == TWO_COMPLEMENT(rhs)) {
    //         O = true;
    //     }
    //
    //     lhs = quotient;
    //
    //     // Update final flags
    //     Z = lhs.isZero();
    //     N = lhs.MSB();
    // }

    /*
    Increments a register by 1 and updates ALU flags.

    Performs a bitwise increment of `reg` using a ripple-carry full adder.
    Early termination is applied once no further carry propagates.

    Flags updated:
    - ZF (Zero Flag): Set to 1 if the result is all zeros; otherwise 0.
    - SF (Sign Flag): Set to the most significant bit (MSB) of the result.
    - OF (Overflow Flag): Set to 1 if signed overflow occurs.
    - CF is unaffected (like x86 INC instruction).

    Parameters:
    - reg: Register to increment; will store the result.
    */
    constexpr void INC(Register& reg) noexcept {
        const Bit MSB_before = reg.MSB();
        Bit carry = true;
        ZF = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [SUM, CARRY] = CombinationalCircuits::FULL_ADDER(reg[i], Bit(false), carry);
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
        OF = MSB_before == Bit(false) & SF == Bit(true);
    }

    /*
    Decrements a register by 1 and updates ALU flags.

    Performs a bitwise subtraction of 1 from `reg` using a ripple-borrow full subtractor.
    Early termination is applied once no further borrow propagates.

    Flags updated:
    - ZF (Zero Flag): Set to 1 if the result is all zeros; otherwise 0.
    - SF (Sign Flag): Set to the most significant bit (MSB) of the result.
    - OF (Overflow Flag): Set if signed overflow occurs when decrementing.
    - CF is unaffected (like x86 DEC instruction).

    Parameters:
    - reg: Register to decrement; will store the result.
    */
    constexpr void DEC(Register& reg) noexcept {
        const Bit MSB_before = reg.MSB();
        Bit borrow = true;
        ZF = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [DIFF, BORROW] = CombinationalCircuits::FULL_SUBTRACTOR(reg[i], Bit(false), borrow);
            reg[i] = DIFF;
            borrow = BORROW;

            if (DIFF) {
                ZF = false;
            }
            if (!BORROW) {
                break;
            }
        }
        SF = reg.MSB();
        OF = MSB_before == Bit(true) & SF == Bit(false);
    }

    /*
    Negates a register and updates ALU flags.

    Performs two's complement negation: reg = -reg.
    Uses DEC/ADD internally for simplicity and sets flags properly.

    Flags updated:
    - ZF (Zero Flag): Set to 1 if the result is zero; otherwise 0.
    - SF (Sign Flag): Set to the most significant bit (MSB) of the result.
    - CF (Carry Flag): Set to 1 if operand was non-zero; otherwise 0.
    - OF (Overflow Flag): Set if negating the most negative value (two's complement).

    Parameters:
    - reg: Register to negate; will store the result.
    */
    // constexpr void NEG(Register& reg) noexcept {
    //     Register zero;
    //     SUB(zero, reg); // zero - reg = -reg
    //     reg = zero;
    //
    //     CF = (reg != Register{}); // CF = 1 if original reg was non-zero
    //     ZF = (reg == Register{});
    //     SF = reg.MSB();
    //     // OF already handled correctly via SUB
    // }

    /*
    Performs a logical left shift (SHL) on a register and updates ALU flags.

    Each bit in `reg` is shifted left by `count` positions.
    The most significant bit shifted out becomes the Carry Flag (CF).
    The Sign Flag (SF), Zero Flag (ZF), and Overflow Flag (OF) are updated.

    Flags updated:
    - CF (Carry Flag): Last bit shifted out of MSB.
    - ZF (Zero Flag): Set if result is all zeros.
    - SF (Sign Flag): Set to the MSB of the result.
    - OF (Overflow Flag): Set if MSB changes during a 1-bit shift (signed overflow).

    Parameters:
    - reg: Register to shift; will store the result.
    - count: Number of bits to shift left.
    */
    /*
Performs a logical left shift (SHL) on a register and updates ALU flags.

Shifts all bits in `reg` left by `count` positions.
The most significant bits shifted out become the Carry Flag (CF).
Lower bits are filled with zeros.

Parameters:
- reg: Register to shift; will store the result.
- count: Number of bits to shift left (0 <= count <= ARCHITECTURE).

Flags updated:
- CF (Carry Flag): Last bit shifted out of MSB.
- ZF (Zero Flag): Set if result is all zeros.
- SF (Sign Flag): Set to the MSB of the result.
- OF (Overflow Flag): For 1-bit shifts, OF = SF XOR CF; otherwise 0.
*/
    constexpr void SHL(Register& reg, uint8_t count) noexcept {
        if (count == 0) {
            return;
        }
        if (count >= ARCHITECTURE) {
            count = ARCHITECTURE;
        }

        // Capture the last bit shifted out for CF
        CF = reg[ARCHITECTURE - count];

        // Shift bits in bulk
        for (int i = ARCHITECTURE - 1 - count; i >= 0; i--) {
            reg[i + count] = reg[i];
        }
        // Clear lower bits
        for (int i = 0; i < count; i++) {
            reg[i] = false;
        }

        SF = reg.MSB();
        ZF = (reg == Register{});
        OF = (count == 1) ? (SF ^ CF) : false;
    }

    /*
    Performs a logical right shift (SHR) on a register and updates ALU flags.

    Shifts all bits in `reg` right by `count` positions.
    The least significant bits shifted out become the Carry Flag (CF).
    Higher bits are filled with zeros.

    Parameters:
    - reg: Register to shift; will store the result.
    - count: Number of bits to shift right (0 <= count <= ARCHITECTURE).

    Flags updated:
    - CF (Carry Flag): Last bit shifted out of LSB.
    - ZF (Zero Flag): Set if result is all zeros.
    - SF (Sign Flag): Set to the MSB of the result.
    - OF (Overflow Flag): Always 0 for logical right shifts.
    */
    constexpr void SHR(Register& reg, uint8_t count) noexcept {
        if (count == 0) {
            return;
        }
        if (count >= ARCHITECTURE) {
            count = ARCHITECTURE;
        }

        CF = reg[count - 1];

        // Shift bits in bulk
        for (uint8_t i = 0; i < ARCHITECTURE - count; i++) {
            reg[i] = reg[i + count];
        }
        // Clear upper bits
        for (uint8_t i = ARCHITECTURE - count; i < ARCHITECTURE; i++) {
            reg[i] = false;
        }

        SF = reg.MSB();
        ZF = (reg == Register{});
        OF = false;
    }

    /*
    Performs an arithmetic right shift (SAR) on a register and updates ALU flags.

    Shifts all bits in `reg` right by `count` positions.
    The sign bit (MSB) is preserved.
    The least significant bits shifted out become the Carry Flag (CF).

    Parameters:
    - reg: Register to shift; will store the result.
    - count: Number of bits to shift right (0 <= count <= ARCHITECTURE).

    Flags updated:
    - CF (Carry Flag): Last bit shifted out of LSB.
    - ZF (Zero Flag): Set if result is all zeros.
    - SF (Sign Flag): Preserved MSB.
    - OF (Overflow Flag): Always 0.
    */
    constexpr void SAR(Register& reg, uint8_t count) noexcept {
        if (count == 0) {
            return;
        }
        if (count >= ARCHITECTURE) {
            count = ARCHITECTURE;
        }

        Bit sign = reg.MSB();
        CF = reg[count - 1];

        // Shift bits in bulk
        for (uint8_t i = 0; i < ARCHITECTURE - count; i++) {
            reg[i] = reg[i + count];
        }
        // Fill upper bits with sign
        for (uint8_t i = ARCHITECTURE - count; i < ARCHITECTURE; i++) {
            reg[i] = sign;
        }

        SF = reg.MSB();
        ZF = (reg == Register{});
        OF = false;
    }

    /*
    Performs a rotate left (ROL) on a register and updates ALU flags.

    Rotates all bits in `reg` left by `count` positions.
    Bits shifted out from MSB wrap around to LSB.

    Parameters:
    - reg: Register to rotate; will store the result.
    - count: Number of bits to rotate left (0 <= count).

    Flags updated:
    - CF (Carry Flag): Last bit rotated out of MSB.
    - ZF (Zero Flag): Set if result is all zeros.
    - SF (Sign Flag): Set to MSB of the result.
    - OF (Overflow Flag): For 1-bit rotates, OF = SF XOR CF; otherwise 0.
    */
    constexpr void ROL(Register& reg, uint8_t count) noexcept {
        if (count == 0) {
            return;
        }
        count %= ARCHITECTURE;

        for (uint8_t c = 0; c < count; c++) {
            Bit msb = reg[ARCHITECTURE - 1];
            for (int i = ARCHITECTURE - 1; i > 0; i--) {
                reg[i] = reg[i - 1];
            }
            reg[0] = msb;
            CF = msb;
        }

        SF = reg.MSB();
        ZF = (reg == Register{});
        OF = (count == 1) ? (SF ^ CF) : false;
    }

    /*
    Performs a rotate right (ROR) on a register and updates ALU flags.

    Rotates all bits in `reg` right by `count` positions.
    Bits shifted out from LSB wrap around to MSB.

    Parameters:
    - reg: Register to rotate; will store the result.
    - count: Number of bits to rotate right (0 <= count).

    Flags updated:
    - CF (Carry Flag): Last bit rotated out of LSB.
    - ZF (Zero Flag): Set if result is all zeros.
    - SF (Sign Flag): Set to MSB of the result.
    - OF (Overflow Flag): For 1-bit rotates, OF = MSB XOR next MSB; otherwise 0.
    */
    constexpr void ROR(Register& reg, uint8_t count) noexcept {
        if (count == 0) {
            return;
        }
        count %= ARCHITECTURE;

        for (uint8_t c = 0; c < count; c++) {
            Bit lsb = reg[0];
            for (uint8_t i = 0; i < ARCHITECTURE - 1; i++) {
                reg[i] = reg[i + 1];
            }
            reg[ARCHITECTURE - 1] = lsb;
            CF = lsb;
        }

        SF = reg.MSB();
        ZF = (reg == Register{});
        OF = (count == 1) ? (reg[ARCHITECTURE - 1] ^ reg[ARCHITECTURE - 2]) : false;
    }
