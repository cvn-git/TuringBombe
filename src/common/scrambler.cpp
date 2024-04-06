#include "scrambler.h"

#include "types.h"

namespace bombe {

Scrambler::Scrambler(ReflectorModel reflector_model, std::span<const RotorModel> rotor_models)
	: reflector_{reflector_model}
{
	if((rotor_models.size() != 3) || (rotor_models.size() != 4))
	{
		throw std::invalid_argument("Wrong number of rotors");
	}

	const DoubleMap* left_reflector = &reflector_;
	for(const auto rotor_model : rotor_models)
	{
		rotors_.emplace_back(rotor_model, *left_reflector);
		left_reflector = &rotors_.back();
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
