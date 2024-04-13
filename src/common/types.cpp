#include "types.h"

namespace {

class NullDoubleMap : public bombe::DoubleMap
{
public:
	NullDoubleMap()
	{
		for(bombe::Letter k = 0; k < bombe::NUM_LETTERS; ++k)
		{
			(*this)[k] = k;
		}

		bombe::extendMap(*this);
	}
};

} // anonymous namespace

namespace bombe {

Letter char2Letter(char ch)
{
	const auto letter = static_cast<Letter>(::toupper(ch) - 'A');
	if(letter >= NUM_LETTERS)
	{
		throw std::invalid_argument("char2Letter(): Letter out of bound");
	}
	return letter;
}

void char2Letter(std::span<const char> chars, std::span<Letter> letters)
{
	if(chars.size() != letters.size())
	{
		throw std::invalid_argument("char2Letter(): Array sizes mismatch");
	}

	for(size_t k = 0; k < chars.size(); ++k)
	{
		letters[k] = char2Letter(chars[k]);
	}
}

char letter2Char(Letter letter)
{
	if(letter >= NUM_LETTERS)
	{
		throw std::invalid_argument("char2Letter(): Letter out of bound");
	}
	return static_cast<char>(letter) + 'A';
}

void letter2Char(std::span<const Letter> letters, std::span<char> chars)
{
	if(chars.size() != letters.size())
	{
		throw std::invalid_argument("char2Letter(): Array sizes mismatch");
	}

	for(size_t k = 0; k < chars.size(); ++k)
	{
		chars[k] = letter2Char(letters[k]);
	}
}

const DoubleMap& nullDoubleMap()
{
	static const NullDoubleMap null;
	return null;
}

std::string printMap(std::span<const Letter> map)
{
	std::string output(map.size(), ' ');
	letter2Char(map, output);
	return output;
}

void assertSymmetricalMap(std::span<const Letter> map)
{
	if(map.size() < NUM_LETTERS)
	{
		throw std::runtime_error("Insufficient map");
	}

	for(Letter l1 = 0; l1 < NUM_LETTERS; ++l1)
	{
		const Letter l2 = map[l1];
		if(map[l2] != l1)
		{
			throw std::runtime_error("Map not symmetrical");
		}
	}
}

} // namespace bombe
