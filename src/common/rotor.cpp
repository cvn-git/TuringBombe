#include "rotor.h"

#include <vector>

namespace bombe {

RotorModel getRotorModel(bool is_thin, size_t model_number)
{
	if(is_thin)
	{
		model_number += 100;
	}

	return static_cast<RotorModel>(model_number);
}

Rotor::Rotor(RotorModel model, const DoubleMap* left_reflector)
	: left_reflector_{left_reflector}
{
	std::string_view wiring;
	std::string_view turnovers;
	switch(model)
	{
	case RotorModel::M_I:
		wiring = "EKMFLGDQVZNTOWYHXUSPAIBRCJ";
		turnovers = "Q";
		break;

	case RotorModel::M_II:
		wiring = "AJDKSIRUXBLHWTMCQGZNPYFVOE";
		turnovers = "E";
		break;

	case RotorModel::M_III:
		wiring = "BDFHJLCPRTXVZNYEIWGAKMUSQO";
		turnovers = "V";
		break;

	case RotorModel::M_IV:
		wiring = "ESOVPZJAYQUIRHXLNFTGKDCMWB";
		turnovers = "J";
		break;

	case RotorModel::M_V:
		wiring = "VZBRGITYUPSDNHLXAWMJQOFECK";
		turnovers = "Z";
		break;

	case RotorModel::M_VI:
		wiring = "JPGVOUMFYQBENHZRDKASXLICTW";
		turnovers = "ZM";
		break;

	case RotorModel::M_VII:
		wiring = "NZJHGRCXMYSWBOUFAIVLPEKQDT";
		turnovers = "ZM";
		break;

	case RotorModel::M_VIII:
		wiring = "FKQHTLXOCBJSPDZRAMEWNIUYGV";
		turnovers = "ZM";
		break;

	case RotorModel::M_BETA:
		wiring = "LEYJVCNIXWPBQMDRTAKZGFUHOS";
		turnovers = "";
		break;

	case RotorModel::M_GAMMA:
		wiring = "FSOKANUERHMBTIYCWLQPZXVGJD";
		turnovers = "";
		break;

	default:
		throw std::invalid_argument("Invalid rotor model");
	}

	std::vector<Letter> turnover_letters(turnovers.size());
	char2Letter(turnovers, turnover_letters);
	for(const Letter letter : turnover_letters)
	{
		if(turnovers_[letter])
		{
			throw std::invalid_argument("Duplicated turnovers");
		}
		turnovers_.set(letter);
	}

	std::array<Letter, NUM_LETTERS> wiring_letters;
	char2Letter(wiring, wiring_letters);
	const DoubleMap& null_map = nullDoubleMap();
	for(Letter l1 = 0; l1 < NUM_LETTERS; ++l1)
	{
		const Letter l2 = wiring_letters[l1];
		inward_map_[l1] = null_map[l2 + NUM_LETTERS - l1];
		outward_map_[l2] = null_map[l1 + NUM_LETTERS - l2];
	}
	extendMap(inward_map_);
	extendMap(outward_map_);
}

void Rotor::setPosition(Letter position)
{
	assert(position < NUM_LETTERS);
	position_ = position;

	const DoubleMap& null_map = nullDoubleMap();
	for(Letter in = 0; in < NUM_LETTERS; ++in)
	{
		const Letter out = (*left_reflector_)[in + inward_map_[in + position_]];
		(*this)[in] = null_map[out + outward_map_[out + position_]];
	}
	extendMap(*this);
}

} // namespace bombe
