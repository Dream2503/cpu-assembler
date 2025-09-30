#pragma once
#include "bit.hpp"

/*
Register Class

Represents a fixed-width collection of bits (ARCHITECTURE) and provides access and utility
functions for bitwise operations.

Follows Separation of Concerns (SOC): only handles bit storage, conversion, and access.
No arithmetic or logic operations are implemented here.
*/
class Register {
    Bit bits[ARCHITECTURE] = {}; // Array storing individual bits of the register

    // Default constructor: initializes all bits to 0
    constexpr Register() = default;

public:
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
    requires(sizeof(T) * 8 == ARCHITECTURE)
    explicit constexpr operator T() const noexcept {
        T value = 0;

        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            value |= static_cast<bool>(bits[i]) << i;
        }
        return value;
    }

    /*
    Stream insertion operator for printing the register.

    Outputs bits from MSB to LSB.

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
            os << (bool)reg[ARCHITECTURE - i - 1];
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

    /*
    Allocates 16 registers dynamically.

    Returns:
    - Pointer to the first element of a dynamically allocated array of 16 Register objects.

    Notes:
    - It is the caller's responsibility to clear or initialize these registers as needed.
    - Caller must delete[] the returned pointer when done.
    */
    static Register* instantiate_register_set() noexcept { return new Register[16]; }

    // Disable assignment to enforce immutability after creation
    constexpr Register& operator=(const Register&) = delete;
    constexpr Register& operator=(Register&&) = delete;
};
