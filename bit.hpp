#pragma once

/*
- This class represents a single-bit logical value (`0` or `1`).
- It provides implementations of basic logic gate operations (NOT, AND, OR, XOR, etc.)
  as operator overloads, modeling how digital circuits behave at the bit level.
- Additional composite gates (XNOR, NAND, NOR) are also provided.

Usage:
- Each `Bit` object acts like a boolean but supports gate-like operators.
- Functions return `Bit` values instead of `bool` to maintain composability in circuit modeling.
- Useful for building combinational logic circuits, adders, subtractors, and higher-level
  digital designs.
*/
class Bit {
    bool x = false; // Internal boolean representation of the bit (0 = false, 1 = true)

public:
    constexpr Bit() = default;
    constexpr Bit(const bool x) noexcept : x(x) {}
    constexpr operator bool() const { return x; }

    /*
     Truth Table (NOT gate):
       x | ~x
      ---|----
       0 |  1
       1 |  0

     Logical NOT (inverter).
     Produces the complement of the input bit.
     */
    constexpr Bit operator~() const noexcept;

    /*
     Truth Table (AND gate):
       x  y | x & y
      ------|-------
       0  0 |   0
       0  1 |   0
       1  0 |   0
       1  1 |   1

     Logical AND.
     Produces 1 if *both* inputs are 1, otherwise 0.
     */
    constexpr Bit operator&(const Bit& y) const noexcept;

    /*
     Truth Table (OR gate):
       x  y | x | y
      ------|-------
       0  0 |   0
       0  1 |   1
       1  0 |   1
       1  1 |   1

     Logical OR.
     Produces 1 if *at least one* input is 1, otherwise 0.
     */
    constexpr Bit operator|(const Bit& y) const noexcept;

    /*
     Truth Table (XOR gate):
       x  y | x ^ y
      ------|-------
       0  0 |   0
       0  1 |   1
       1  0 |   1
       1  1 |   0

     Logical XOR (exclusive OR).
     Produces 1 if the inputs differ, otherwise 0.
     */
    constexpr Bit operator^(const Bit& y) const noexcept;

    /*
     Truth Table (XNOR gate):
       x  y | ~(x ^ y)
      ------|----------
       0  0 |     1
       0  1 |     0
       1  0 |     0
       1  1 |     1

     Logical XNOR (equivalence).
     Produces 1 if the inputs are the same, otherwise 0.
     */
    constexpr Bit XNOR(const Bit& y) const noexcept;

    /*
     Truth Table (NAND gate):
       x  y | ~(x & y)
      ------|----------
       0  0 |     1
       0  1 |     1
       1  0 |     1
       1  1 |     0

     Logical NAND.
     Produces 1 unless *both* inputs are 1.
     */
    constexpr Bit NAND(const Bit& y) const noexcept;

    /*
     Truth Table (NOR gate):
       x  y | ~(x | y)
      ------|----------
       0  0 |     1
       0  1 |     0
       1  0 |     0
       1  1 |     0

     Logical NOR.
     Produces 1 only if *both* inputs are 0.
     */
    constexpr Bit NOR(const Bit& y) const noexcept;

    /*
     Truth Table (Equality):
       x  y | x == y
      ------|--------
       0  0 |   1
       0  1 |   0
       1  0 |   0
       1  1 |   1

     Logical equivalence.
     Produces 1 if inputs are equal, otherwise 0.
     Implemented using XNOR.
     */
    constexpr Bit operator==(const Bit& y) const noexcept;

    /*
     Truth Table (Inequality):
       x  y | x != y
      ------|--------
       0  0 |   0
       0  1 |   1
       1  0 |   1
       1  1 |   0

     Logical inequality.
     Produces 1 if inputs differ, otherwise 0.
     Implemented using XOR.
     */
    constexpr Bit operator!=(const Bit& y) const noexcept;
};

constexpr Bit Bit::operator~() const noexcept { return !x; }
constexpr Bit Bit::operator&(const Bit& y) const noexcept { return x & y.x; }
constexpr Bit Bit::operator|(const Bit& y) const noexcept { return x | y.x; }
constexpr Bit Bit::operator^(const Bit& y) const noexcept { return x ^ y.x; }
constexpr Bit Bit::XNOR(const Bit& y) const noexcept { return ~(*this ^ y); }
constexpr Bit Bit::NAND(const Bit& y) const noexcept { return ~(*this & y); }
constexpr Bit Bit::NOR(const Bit& y) const noexcept { return ~(*this | y); }
constexpr Bit Bit::operator==(const Bit& y) const noexcept { return XNOR(y); }
constexpr Bit Bit::operator!=(const Bit& y) const noexcept { return *this ^ y; }
