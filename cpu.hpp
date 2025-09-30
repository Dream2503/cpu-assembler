#pragma once

/*
CPU Architecture Constants

- Defines architecture width and type aliases.
- Follows separation of concerns (SOC): only architecture and type info here.
*/

using uint8_t = unsigned char;
using int8_t = signed char;
constexpr uint8_t ARCHITECTURE = 16;

#include "alu.hpp"
