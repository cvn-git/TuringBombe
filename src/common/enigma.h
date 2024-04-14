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

	size_t numRotors() const
	{
		return std::holds_alternative<Scrambler<3>>(scrambler_) ? 3 : 4;
	}

private:
	void resetSteckers();

private:
	std::variant<std::monostate, Scrambler<3>, Scrambler<4>> scrambler_;
	SingleMap steckers_;
};

} // namespace bombe

#endif // BOMBE_ENIGMA_H
