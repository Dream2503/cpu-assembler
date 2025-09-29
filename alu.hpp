#pragma once
#include "combinational_circuit.hpp"
#include "register.hpp"

/*
ALU (Arithmetic Logic Unit)

Provides basic arithmetic operations on Register objects and updates standard ALU flags.

Flags:
- Z (Zero): Set if the result of an operation is zero.
- N (Negative): Set to the most significant bit (MSB) of the result.
- O (Overflow): Set if signed overflow occurs in two's complement arithmetic.

This ALU currently supports:
- ADD: Adds two registers using a ripple-carry adder.
- SUB: Subtracts two registers using a ripple-borrow subtractor.
*/
class ALU {
public:
    Bit Z; // Zero flag
    Bit N; // Negative flag
    Bit O; // Overflow flag

    /*
    Adds two registers and updates ALU flags.

    Performs a bitwise addition of `lhs` and `rhs` using a ripple-carry adder. The result
    is stored in `lhs`.

    Flags updated:
    - Z: Set to 1 if the result is all zeros; otherwise 0.
    - N: Set to the MSB of the result.
    - O: Set if signed overflow occurs in two's complement arithmetic.

    Parameters:
    - lhs: Left-hand side operand; will also store the result.
    - rhs: Right-hand side operand.
    */
    constexpr void ADD(Register& lhs, const Register& rhs) noexcept {
        const Bit lhs_MSB = lhs.MSB();
        Bit carry = false;
        Z = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [SUM, CARRY] = CombinationalCircuits::FULL_ADDER(lhs[i], rhs[i], carry);
            lhs[i] = SUM;
            carry = CARRY;

            if (SUM) {
                Z = false;
            }
        }
        N = lhs.MSB();
        O = lhs_MSB == rhs.MSB() & lhs.MSB() != lhs_MSB;
    }

    /*
    Subtracts two registers and updates ALU flags.

    Performs a bitwise subtraction of `rhs` from `lhs` using a ripple-borrow full subtractor.
    The result is stored in `lhs`.

    Flags updated:
    - Z: Set to 1 if the result is all zeros; otherwise 0.
    - N: Set to the MSB of the result.
    - O: Set if signed overflow occurs in two's complement arithmetic.

    Parameters:
    - lhs: Left-hand side operand; will also store the result.
    - rhs: Right-hand side operand.
    */
    constexpr void SUB(Register& lhs, const Register& rhs) noexcept {
        const Bit lhs_MSB = lhs.MSB();
        Bit borrow = false;
        Z = true;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            const auto [DIFFERENCE, BORROW] = CombinationalCircuits::FULL_SUBTRACTOR(lhs[i], rhs[i], borrow);
            lhs[i] = DIFFERENCE;
            borrow = BORROW;

            if (DIFFERENCE) {
                Z = false;
            }
        }
        N = lhs.MSB();
        O = lhs_MSB != rhs.MSB() & lhs.MSB() != lhs_MSB;
    }
};
