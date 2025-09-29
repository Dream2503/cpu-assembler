#pragma once
#include "bit.hpp"

/*
- This class provides implementations of basic combinational logic used in digital circuits.
- It focuses on full adder operations, which are the building blocks for arithmetic circuits such
  as adders and subtractors.

Usage:
- These functions operate on `Bit` objects (custom wrapper for boolean values) and model the
  behavior of hardware logic gates. They can be composed to build multi-bit ripple-carry adders or
  more complex arithmetic components.
*/
class CombinationalCircuits {
public:
    /*
    Result of a half-adder operation.
    Contains the SUM and CARRY bits for a single-bit addition (no carry-in).
    */
    struct HALF_ADDER_RESULT {
        Bit SUM; // Sum output of the half adder
        Bit CARRY; // Carry output of the half adder
    };

    /*
     x  y | SUM(x + y)
    ------|------------
     0  0 |     0
     0  1 |     1
     1  0 |     1
     1  1 |     0

    Computes the SUM bit for a half-adder:
    sum <- x ^ y
    */
    static constexpr Bit HALF_ADDER_SUM(const Bit& x, const Bit& y) noexcept;

    /*
     x  y | CARRY(x + y)
    ------|--------------
     0  0 |      0
     0  1 |      0
     1  0 |      0
     1  1 |      1

    Computes the carry-out bit for a half-adder:
    carry <- x & y
    */
    static constexpr Bit HALF_ADDER_CARRY(const Bit& x, const Bit& y) noexcept;

    /*
    Performs a half-adder operation on two bits.

    Combines HALF_ADDER_SUM and HALF_ADDER_CARRY to produce both
    sum and carry-out simultaneously.
    */
    static constexpr HALF_ADDER_RESULT HALF_ADDER(const Bit& x, const Bit& y) noexcept;


    /*
    Result of a half-subtractor operation.
    Contains the DIFFERENCE and BORROW bits for a single-bit subtraction (no borrow-in).
    */
    struct HALF_SUBTRACTOR_RESULT {
        Bit DIFFERENCE; // Difference output of the half subtractor
        Bit BORROW; // Borrow output of the half subtractor
    };

    /*
     x  y | DIFFERENCE(x - y)
    ------|-------------------
     0  0 |        0
     0  1 |        1
     1  0 |        1
     1  1 |        0

    Computes the DIFFERENCE bit for a half-subtractor:
    difference <- x ^ y
    */
    static constexpr Bit HALF_SUBTRACTOR_DIFFERENCE(const Bit& x, const Bit& y) noexcept;

    /*
     x  y | BORROW(x - y)
    ------|---------------
     0  0 |       0
     0  1 |       1
     1  0 |       0
     1  1 |       0

    Computes the borrow-out bit for a half-subtractor:
    borrow <- ~x & y
    */
    static constexpr Bit HALF_SUBTRACTOR_BORROW(const Bit& x, const Bit& y) noexcept;

    /*
    Performs a half-subtractor operation on two bits.

    Combines HALF_SUBTRACTOR_DIFFERENCE and HALF_SUBTRACTOR_BORROW to produce both
    difference and borrow-out simultaneously.
    */
    static constexpr HALF_SUBTRACTOR_RESULT HALF_SUBTRACTOR(const Bit& x, const Bit& y) noexcept;


    /*
    Result of a full-adder operation.
    Contains the SUM and CARRY bits for a single-bit addition with carry-in.
    */
    struct FULL_ADDER_RESULT {
        Bit SUM; // Sum output of the full adder
        Bit CARRY; // Carry output of the full adder
    };

    /*
     x  y  c | SUM(x + y + c)
    ---------|----------------
     0  0  0 |      0
     0  0  1 |      1
     0  1  0 |      1
     0  1  1 |      0
     1  0  0 |      1
     1  0  1 |      0
     1  1  0 |      0
     1  1  1 |      1

    Computes the SUM bit for a full-adder:
    sum <- x ^ y ^ c

    Implemented using HALF_ADDER:
    sum <- HALF_ADDER_SUM(HALF_ADDER_SUM(x, y), c)
    */
    static constexpr Bit FULL_ADDER_SUM(const Bit& x, const Bit& y, const Bit& c) noexcept;

    /*
     x  y  c | CARRY(x + y + c)
    ---------|------------------
     0  0  0 |        0
     0  0  1 |        0
     0  1  0 |        0
     0  1  1 |        1
     1  0  0 |        0
     1  0  1 |        1
     1  1  0 |        1
     1  1  1 |        1

    Computes the carry-out bit for a full-adder:
    carry <- (x & y) | ((x ^ y) & c)

    Implemented using HALF_ADDER:
    carry <- HALF_ADDER_CARRY(x, y) | HALF_ADDER_CARRY(HALF_ADDER_SUM(x, y), c)
    */
    static constexpr Bit FULL_ADDER_CARRY(const Bit& x, const Bit& y, const Bit& c) noexcept;

    /*
    Performs a full-adder operation on three bits.

    Combines FULL_ADDER_SUM and FULL_ADDER_CARRY to produce both
    sum and carry-out simultaneously.
    */
    static constexpr FULL_ADDER_RESULT FULL_ADDER(const Bit& x, const Bit& y, const Bit& c) noexcept;


    /*
    Result of a full-subtractor operation.
    Contains the DIFFERENCE and BORROW bits for a single-bit subtraction with borrow-in.
    */
    struct FULL_SUBTRACTOR_RESULT {
        Bit DIFFERENCE; // Difference output of the full subtractor
        Bit BORROW; // Borrow output of the full subtractor
    };

    /*
     x  y  b | DIFFERENCE(x - y - b)
    ---------|-----------------------
     0  0  0 |          0
     0  0  1 |          1
     0  1  0 |          1
     0  1  1 |          0
     1  0  0 |          1
     1  0  1 |          0
     1  1  0 |          0
     1  1  1 |          1

    Computes the DIFFERENCE bit for a full-subtractor:
    difference <- x ^ y ^ b

    Implemented using HALF_SUBTRACTOR:
    difference <- HALF_SUBTRACTOR_DIFFERENCE(HALF_SUBTRACTOR_DIFFERENCE(x, y), b)
    */
    static constexpr Bit FULL_SUBTRACTOR_DIFFERENCE(const Bit& x, const Bit& y, const Bit& b) noexcept;

    /*
     x  y  b | BORROW(x - y - b)
    ---------|-------------------
     0  0  0 |          0
     0  0  1 |          1
     0  1  0 |          1
     0  1  1 |          1
     1  0  0 |          0
     1  0  1 |          0
     1  1  0 |          0
     1  1  1 |          1

    Computes the borrow-out bit for a full-subtractor:
    borrow <- (~x & y) | (~(x ^ y) & b)

    Implemented using HALF_SUBTRACTOR:
    borrow <- HALF_SUBTRACTOR_BORROW(x, y) | HALF_SUBTRACTOR_BORROW(HALF_SUBTRACTOR_DIFFERENCE(x, y), b)
    */
    static constexpr Bit FULL_SUBTRACTOR_BORROW(const Bit& x, const Bit& y, const Bit& b) noexcept;

    /*
    Performs a full-subtractor operation on three bits.

    Combines FULL_SUBTRACTOR_DIFFERENCE and FULL_SUBTRACTOR_BORROW to produce both
    difference and borrow-out simultaneously.
    */
    static constexpr FULL_SUBTRACTOR_RESULT FULL_SUBTRACTOR(const Bit& x, const Bit& y, const Bit& b) noexcept;
};


constexpr Bit CombinationalCircuits::HALF_ADDER_SUM(const Bit& x, const Bit& y) noexcept { return x ^ y; }
constexpr Bit CombinationalCircuits::HALF_ADDER_CARRY(const Bit& x, const Bit& y) noexcept { return x & y; }
constexpr CombinationalCircuits::HALF_ADDER_RESULT CombinationalCircuits::HALF_ADDER(const Bit& x, const Bit& y) noexcept {
    return {HALF_ADDER_SUM(x, y), HALF_ADDER_CARRY(x, y)};
}

constexpr Bit CombinationalCircuits::HALF_SUBTRACTOR_DIFFERENCE(const Bit& x, const Bit& y) noexcept { return x ^ y; }
constexpr Bit CombinationalCircuits::HALF_SUBTRACTOR_BORROW(const Bit& x, const Bit& y) noexcept { return ~x & y; }
constexpr CombinationalCircuits::HALF_SUBTRACTOR_RESULT CombinationalCircuits::HALF_SUBTRACTOR(const Bit& x, const Bit& y) noexcept {
    return {HALF_SUBTRACTOR_DIFFERENCE(x, y), HALF_SUBTRACTOR_BORROW(x, y)};
}

constexpr Bit CombinationalCircuits::FULL_ADDER_SUM(const Bit& x, const Bit& y, const Bit& c) noexcept {
    return HALF_ADDER_SUM(HALF_ADDER_SUM(x, y), c);
}
constexpr Bit CombinationalCircuits::FULL_ADDER_CARRY(const Bit& x, const Bit& y, const Bit& c) noexcept {
    return HALF_ADDER_CARRY(x, y) | HALF_ADDER_CARRY(HALF_ADDER_SUM(x, y), c);
}
constexpr CombinationalCircuits::FULL_ADDER_RESULT CombinationalCircuits::FULL_ADDER(const Bit& x, const Bit& y, const Bit& c) noexcept {
    return {FULL_ADDER_SUM(x, y, c), FULL_ADDER_CARRY(x, y, c)};
}

constexpr Bit CombinationalCircuits::FULL_SUBTRACTOR_DIFFERENCE(const Bit& x, const Bit& y, const Bit& b) noexcept {
    return HALF_SUBTRACTOR_DIFFERENCE(HALF_SUBTRACTOR_DIFFERENCE(x, y), b);
}
constexpr Bit CombinationalCircuits::FULL_SUBTRACTOR_BORROW(const Bit& x, const Bit& y, const Bit& b) noexcept {
    return HALF_SUBTRACTOR_BORROW(x, y) | HALF_SUBTRACTOR_BORROW(HALF_SUBTRACTOR_DIFFERENCE(x, y), b);
}
constexpr CombinationalCircuits::FULL_SUBTRACTOR_RESULT CombinationalCircuits::FULL_SUBTRACTOR(const Bit& x, const Bit& y, const Bit& b) noexcept {
    return {FULL_SUBTRACTOR_DIFFERENCE(x, y, b), FULL_SUBTRACTOR_BORROW(x, y, b)};
}
