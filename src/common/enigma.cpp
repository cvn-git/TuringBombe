#include "enigma.h"

namespace bombe {

Enigma::Enigma(ReflectorModel reflector_model, std::span<const RotorModel> rotor_models)
{
	if(rotor_models.size() == 3)
	{
		scrambler_.emplace<Scrambler<3>>(reflector_model, std::span<const RotorModel, 3>(rotor_models));
	}
	else
	{
		scrambler_.emplace<Scrambler<4>>(reflector_model, std::span<const RotorModel, 4>(rotor_models));
	}

	resetSteckers();
}

void Enigma::configureRotors(std::string_view ringstellung, std::string_view grundstellung)
{
	const size_t num_rotors = numRotors();
	if((ringstellung.size() != num_rotors) || (grundstellung.size() != num_rotors))
	{
		throw std::invalid_argument("Invalid ringstellung/grundstellung size");
	}

	std::vector<Letter> ring_positions(num_rotors);
	char2Letter(ringstellung, ring_positions);
	for(size_t k = 0; k < num_rotors; ++k)
	{
		if(num_rotors == 3)
		{
			std::get<Scrambler<3>>(scrambler_).setRotorRing(k, ring_positions[k]);
		}
		else
		{
			std::get<Scrambler<4>>(scrambler_).setRotorRing(k, ring_positions[k]);
		}
	}

	std::vector<Letter> grundstellung_letters(num_rotors);
	char2Letter(grundstellung, grundstellung_letters);
	const DoubleMap& null_map = nullDoubleMap();
	for(size_t rotor_idx = 0; rotor_idx < num_rotors; ++rotor_idx)
	{
		if(num_rotors == 3)
		{
			auto& scrambler = std::get<Scrambler<3>>(scrambler_);
			scrambler.setRotorPosition(
				rotor_idx,
				null_map[grundstellung_letters[rotor_idx] + NUM_LETTERS - scrambler.rotor(rotor_idx).ring()]);
		}
		else
		{
			auto& scrambler = std::get<Scrambler<4>>(scrambler_);
			scrambler.setRotorPosition(
				rotor_idx,
				null_map[grundstellung_letters[rotor_idx] + NUM_LETTERS - scrambler.rotor(rotor_idx).ring()]);
		}
	}
}

void Enigma::configureSteckers(std::string_view stecker_setting)
{
	std::string stecker_setting_pruned;
	for(const char ch : stecker_setting)
	{
		if((ch != ':') && (ch != ' '))
		{
			stecker_setting_pruned.push_back(ch);
		}
	}

	const size_t numSteckers = stecker_setting_pruned.size() / 2;
	if((numSteckers * 2) != stecker_setting_pruned.size())
	{
		throw std::invalid_argument("stecker_setting must have even length");
	}

	resetSteckers();

	std::vector<Letter> stecker_letters(numSteckers * 2);
	char2Letter(stecker_setting_pruned, stecker_letters);
	for(size_t idx = 0; idx < numSteckers; ++idx)
	{
		const auto l1 = stecker_letters[idx * 2];
		const auto l2 = stecker_letters[idx * 2 + 1];
		if(l1 == l2)
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
		if(numRotors() == 3)
		{
			auto& scrambler = std::get<Scrambler<3>>(scrambler_);
			scrambler.stepEnigma();
			output[k] = steckers_[scrambler.map()[steckers_[input[k]]]];
		}
		else
		{
			auto& scrambler = std::get<Scrambler<4>>(scrambler_);
			scrambler.stepEnigma();
			output[k] = steckers_[scrambler.map()[steckers_[input[k]]]];
		}
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

} // namespace bombe
