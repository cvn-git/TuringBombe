#ifndef BOMBE_ENIGMA_H
#define BOMBE_ENIGMA_H

#include "scrambler.h"

namespace bombe {

class Enigma
{
public:
	Enigma(ReflectorModel reflector_model, std::span<const RotorModel> rotor_models);

	void configureRotors(std::string_view ringstellung, std::string_view grundstellung);

	void configureSteckers(std::string_view stecker_setting);

	void process(std::span<const Letter> input, std::span<Letter> output);

	void process(std::string_view input, std::span<char> output);

private:
	void resetSteckers();

	void stepScrambler();

private:
	Scrambler scrambler_;
	SingleMap steckers_;
};

} // namespace bombe

#endif // BOMBE_ENIGMA_H
