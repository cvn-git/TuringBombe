#include "enigma.h"

namespace bombe {

Enigma::Enigma(ReflectorModel reflector_model, std::span<const RotorModel> rotor_models)
	: scrambler_{reflector_model, rotor_models}
{
	resetSteckers();
}

void Enigma::configureRotors(std::string_view ringstellung, std::string_view grundstellung)
{
	const size_t num_rotors = scrambler_.numRotors();
	if((ringstellung.size() != num_rotors) || (grundstellung.size() != num_rotors))
	{
		throw std::invalid_argument("Invalid ringstellung/grundstellung size");
	}

	std::vector<Letter> ring_positions(num_rotors);
	char2Letter(ringstellung, ring_positions);
	for(size_t k = 0; k < num_rotors; ++k)
	{
		scrambler_.setRotorRing(k, ring_positions[k]);
	}

	std::vector<Letter> grundstellung_letters(num_rotors);
	char2Letter(grundstellung, grundstellung_letters);
	const DoubleMap& null_map = nullDoubleMap();
	for(size_t rotor_idx = 0; rotor_idx < num_rotors; ++rotor_idx)
	{
		const Rotor& rotor = scrambler_.rotor(rotor_idx);
		scrambler_.setRotorPosition(rotor_idx, null_map[grundstellung_letters[rotor_idx] + NUM_LETTERS - rotor.ring()]);
	}
}

void Enigma::configureSteckers(std::string_view stecker_setting)
{
	const size_t numSteckers = stecker_setting.size() / 2;
	if((numSteckers * 2) != stecker_setting.size())
	{
		throw std::invalid_argument("stecker_setting must have even length");
	}

	resetSteckers();

	std::vector<Letter> stecker_letters(numSteckers * 2);
	char2Letter(stecker_setting, stecker_letters);
	for(size_t idx = 0; idx < numSteckers; ++idx)
	{
		const auto l1 = stecker_letters[idx * 2];
		const auto l2 = stecker_letters[idx * 2 + 1];
		if(l1 != l2)
		{
			throw std::invalid_argument("Invalid stecker pair");
		}
		if((steckers_[l1] != l1) || (steckers_[l2] != l2))
		{
			throw std::invalid_argument("Duplicated steckers");
		}
		steckers_[l1] = l2;
		steckers_[l2] = l1;
	}
}

void Enigma::process(std::span<const Letter> input, std::span<Letter> output)
{
	if(input.size() != output.size())
	{
		throw std::invalid_argument("Enigma::process(): input/output must have the same size");
	}

	for(size_t k = 0; k < input.size(); ++k)
	{
		stepScrambler();
		output[k] = steckers_[scrambler_.map()[steckers_[input[k]]]];
	}
}

void Enigma::process(std::string_view input, std::span<char> output)
{
	std::vector<Letter> input_letters(input.size());
	std::vector<Letter> output_letters(output.size());

	char2Letter(input, input_letters);
	process(input_letters, output_letters);
	letter2Char(output_letters, output);
}

void Enigma::resetSteckers()
{
	for(Letter k = 0; k < NUM_LETTERS; ++k)
	{
		steckers_[k] = k;
	}
}

void Enigma::stepScrambler()
{
	const DoubleMap& null_map = nullDoubleMap();

	const size_t num_rotors = scrambler_.numRotors();
	const size_t slow_idx = num_rotors - 3;
	const size_t mid_idx = num_rotors - 2;
	const size_t fast_idx = num_rotors - 1;

	const Rotor& slow_rotor = scrambler_.rotor(slow_idx);
	const Rotor& mid_rotor = scrambler_.rotor(mid_idx);
	const Rotor& fast_rotor = scrambler_.rotor(fast_idx);

	if(mid_rotor.isTurnover())
	{
		// Double step
		scrambler_.setRotorPosition(slow_idx, null_map[slow_rotor.position() + 1]);
		scrambler_.setRotorPosition(mid_idx, null_map[mid_rotor.position() + 1]);
	}
	else if(fast_rotor.isTurnover())
	{
		scrambler_.setRotorPosition(mid_idx, null_map[mid_rotor.position() + 1]);
	}

	scrambler_.setRotorPosition(fast_idx, null_map[fast_rotor.position() + 1]);
}

} // namespace bombe
