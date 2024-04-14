#include "scrambler.h"

#include "types.h"

namespace bombe {

template <size_t NUM_ROTORS>
Scrambler<NUM_ROTORS>::Scrambler(ReflectorModel reflector_model, std::span<const RotorModel, NUM_ROTORS> rotor_models)
	: reflector_{reflector_model}
{
	const DoubleMap* left_reflector = &reflector_;
	for(size_t k = 0; k < NUM_ROTORS; ++k)
	{
		rotors_[k] = Rotor(rotor_models[k], left_reflector);
		left_reflector = &rotors_[k];
	}
}

template <size_t NUM_ROTORS>
void Scrambler<NUM_ROTORS>::setRotorPosition(size_t rotor_idx, Letter position)
{
	assert(rotor_idx < rotors_.size());

	rotors_[rotor_idx].setPosition(position);
}

template <size_t NUM_ROTORS>
void Scrambler<NUM_ROTORS>::setRotorRing(size_t rotor_idx, Letter ring_position)
{
	assert(rotor_idx < rotors_.size());

	rotors_[rotor_idx].setRing(ring_position);
}

template <size_t NUM_ROTORS>
void Scrambler<NUM_ROTORS>::stepEnigma()
{
	const DoubleMap& null_map = nullDoubleMap();

	constexpr size_t slow_idx = NUM_ROTORS - 3;
	constexpr size_t mid_idx = NUM_ROTORS - 2;
	constexpr size_t fast_idx = NUM_ROTORS - 1;

	const Rotor& slow_rotor = rotor(slow_idx);
	const Rotor& mid_rotor = rotor(mid_idx);
	const Rotor& fast_rotor = rotor(fast_idx);

	if(mid_rotor.isTurnover())
	{
		// Double step
		setRotorPosition(slow_idx, null_map[slow_rotor.position() + 1]);
		setRotorPosition(mid_idx, null_map[mid_rotor.position() + 1]);
	}
	else if(fast_rotor.isTurnover())
	{
		setRotorPosition(mid_idx, null_map[mid_rotor.position() + 1]);
	}

	setRotorPosition(fast_idx, null_map[fast_rotor.position() + 1]);
}

// Instantiation
template class Scrambler<3>;
template class Scrambler<4>;

} // namespace bombe
