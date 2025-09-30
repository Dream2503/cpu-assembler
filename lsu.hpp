#pragma once
#include "register.hpp"

/*
Load/Store Unit (LSU)

Handles register-to-register data movement only.
Follows Separation of Concerns (SOC): no arithmetic or logic here.
*/
class LSU {
public:
    /*
    MOV instruction: copies the value from src to dst.

    Parameters:
    - dst: Destination register; overwritten with src value.
    - src: Source register; value to copy.
    */
    static constexpr void MOV(Register& dst, const Register& src) noexcept {
        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            dst[i] = src[i];
        }
    }

    /*
    MOV instruction: loads an integral value into a register.

    Template requires the integral type to have enough bits for the register.

    Parameters:
    - reg: Register to load value into.
    - value: Integral value to copy into the register.
    */
    template <typename T>
    requires(sizeof(T) * 8 >= ARCHITECTURE)
    static constexpr void MOV(Register& reg, const T value) noexcept {
        for (uint8_t i = 0; i < ARCHITECTURE; i++) {
            reg[i] = Bit(value >> i & 1);
        }
    }
};
