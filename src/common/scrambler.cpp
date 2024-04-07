#include "scrambler.h"

#include "types.h"

namespace bombe {

Scrambler::Scrambler(ReflectorModel reflector_model, std::span<const RotorModel> rotor_models)
	: reflector_{reflector_model}
	, rotors_{rotor_models.size()}
{
	if((numRotors() != 3) && (numRotors() != 4))
	{
		throw std::invalid_argument("Wrong number of rotors");
	}

	const DoubleMap* left_reflector = &reflector_;
	for(size_t k = 0; k < numRotors(); ++k)
	{
		rotors_[k] = Rotor(rotor_models[k], left_reflector);
		left_reflector = &rotors_[k];
	}
}

void Scrambler::setRotorPosition(size_t rotor_idx, Letter position)
{
	assert(rotor_idx < rotors_.size());

	rotors_[rotor_idx].setPosition(position);
}

void Scrambler::setRotorRing(size_t rotor_idx, Letter ring_position)
{
	assert(rotor_idx < rotors_.size());

	rotors_[rotor_idx].setRing(ring_position);
}

} // namespace bombe
