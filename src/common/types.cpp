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

void char2Letter(std::span<const char> chars, std::span<Letter> letters)
{
	if(chars.size() != letters.size())
	{
		throw std::invalid_argument("char2Letter(): Array sizes mismatch");
	}

	for(size_t k = 0; k < chars.size(); ++k)
	{
		letters[k] = static_cast<Letter>(::towupper(chars[k]) - 'A');
		if(letters[k] >= NUM_LETTERS)
		{
			throw std::invalid_argument("char2Letter(): Letter out of bound");
		}
	}
}

void letter2Char(std::span<const Letter> letters, std::span<char> chars)
{
	if(chars.size() != letters.size())
	{
		throw std::invalid_argument("char2Letter(): Array sizes mismatch");
	}

	for(size_t k = 0; k < chars.size(); ++k)
	{
		if(letters[k] >= NUM_LETTERS)
		{
			throw std::invalid_argument(std::format("char2Letter(): Letter out of bound, pos = {}", k));
		}
		chars[k] = static_cast<char>(letters[k]) + 'A';
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

} // namespace bombe
