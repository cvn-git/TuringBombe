#ifndef BOMBE_TYPES_H
#define BOMBE_TYPES_H

#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <span>
#include <stdexcept>
#include <string>

namespace bombe {

using Letter = uint8_t;

inline constexpr Letter NUM_LETTERS = 26;

using SingleMap = std::array<Letter, NUM_LETTERS>;
using DoubleMap = std::array<Letter, 2 * NUM_LETTERS>;

void char2Letter(std::span<const char> chars, std::span<Letter> letters);

void letter2Char(std::span<const Letter> letters, std::span<char> chars);

inline void extendMap(DoubleMap& map)
{
	std::copy(map.begin(), map.begin() + NUM_LETTERS, map.begin() + NUM_LETTERS);
}

const DoubleMap& nullDoubleMap();

std::string printMap(std::span<const Letter> map);

void assertSymmetricalMap(std::span<const Letter> map);

} // namespace bombe

#endif // BOMBE_TYPES_H
