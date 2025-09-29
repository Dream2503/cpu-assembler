#pragma once
#include "bit.hpp"

/*
Register

Represents a fixed-width collection of bits (ARCHITECTURE) and provides basic access, conversion,
and utility functions for bitwise operations.

This class can be used as a building block for ALU operations or other combinational logic
simulations.
*/
class Register {
    Bit bits[ARCHITECTURE] = {}; // Array storing individual bits of the register

public:
    // Default constructor: initializes all bits to 0
    constexpr Register() = default;

    /*
    Constructs a register from an integral value.

    The constructor populates the register bits from the least significant bit (LSB) to the most
    significant bit (MSB) of the value.

    Template requires that the source type has enough bits to fit the register.

    Parameters:
    - value: Integral value to initialize the register.
    */
    template <typename T>
    requires(sizeof(T) * 8 >= ARCHITECTURE)
    constexpr Register(const T value) noexcept {
        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            bits[i] = Bit(value >> i & 1);
        }
    }

    // Const access operator: returns the bit at position i
    constexpr Bit operator[](const uint8_t i) const noexcept { return bits[i]; }

    // Non-const access operator: returns a reference to the bit at position i
    constexpr Bit& operator[](const uint8_t i) noexcept { return bits[i]; }

    /*
    Converts the Register to an integral type.

    Iterates through each bit and reconstructs the integer value.

    Template type T must be able to hold ARCHITECTURE bits.

    Returns:
    - T: Integral representation of the register.
    */
    template <typename T>
    constexpr operator T() const noexcept {
        T value = 0;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            value |= bits[i] << i;
        }
        return value;
    }

    /*
    Stream insertion operator for printing the register.

    Outputs the bits from MSB to LSB.

    Example:
    For a 4-bit register with bits 1 0 1 1, prints: 1011

    Parameters:
    - os: Output stream
    - reg: Register to print

    Returns:
    - Reference to the output stream
    */
    friend auto& operator<<(auto& os, const Register& reg) {
        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            os << reg[ARCHITECTURE - i - 1];
        }
        return os;
    }

    /*
    Returns the most significant bit (MSB) of the register.

    Useful for determining sign in signed arithmetic operations.

    Returns:
    - Bit: The MSB of the register
    */
    constexpr Bit MSB() const noexcept { return bits[ARCHITECTURE - 1]; }
};
