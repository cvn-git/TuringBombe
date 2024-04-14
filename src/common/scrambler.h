#ifndef BOMBE_SCRAMBLER_H
#define BOMBE_SCRAMBLER_H

#include "reflector.h"
#include "rotor.h"

#include <variant>

namespace bombe {

template <size_t NUM_ROTORS>
class Scrambler
{
public:
	Scrambler(ReflectorModel reflector_model, std::span<const RotorModel, NUM_ROTORS> rotor_models);

	void setRotorPosition(size_t rotor_idx, Letter position);

	void setRotorRing(size_t rotor_idx, Letter ring_position);

	void stepEnigma();

	const DoubleMap& map() const
	{
		return rotors_.back();
	}

	const Rotor& rotor(size_t rotor_idx) const
	{
		return rotors_[rotor_idx];
	}

private:
	Reflector reflector_;
	std::array<Rotor, NUM_ROTORS> rotors_;
};

} // namespace bombe

#endif // BOMBE_SCRAMBLER_H
