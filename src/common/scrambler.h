#ifndef BOMBE_SCRAMBLER_H
#define BOMBE_SCRAMBLER_H

#include "reflector.h"
#include "rotor.h"

#include <vector>

namespace bombe {

class Scrambler
{
public:
	Scrambler(ReflectorModel reflector_model, std::span<const RotorModel> rotor_models);

	void setRotorPosition(size_t rotor_idx, Letter position);

	void setRotorRing(size_t rotor_idx, Letter ring_position);

	const DoubleMap& map() const
	{
		return rotors_.back();
	}

	const Rotor& rotor(size_t rotor_idx) const
	{
		return rotors_[rotor_idx];
	}

	size_t numRotors() const
	{
		return rotors_.size();
	}

private:
	Reflector reflector_;
	std::vector<Rotor> rotors_;
};

} // namespace bombe

#endif // BOMBE_SCRAMBLER_H
