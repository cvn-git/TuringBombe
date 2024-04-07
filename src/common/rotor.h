#ifndef BOMBE_ROTOR_H
#define BOMBE_ROTOR_H

#include "types.h"

#include <bitset>

namespace bombe {

enum class RotorModel
{
	M_I = 1,
	M_II = 2,
	M_III = 3,
	M_IV = 4,
	M_V = 5,
	M_VI = 6,
	M_VII = 7,
	M_VIII = 8,
	M_BETA = 101,
	M_GAMMA = 102,
};

RotorModel getRotorModel(bool is_thin, size_t model_number);

class Rotor : public DoubleMap
{
public:
	Rotor() = default;

	Rotor(RotorModel model, const DoubleMap* left_reflector);

	Letter position() const
	{
		return position_;
	}

	void setPosition(Letter position);

	Letter ring() const
	{
		return ring_;
	}

	void setRing(Letter ring_position)
	{
		assert(ring_position < NUM_LETTERS);
		ring_ = ring_position;
	}

	bool isTurnover() const
	{
		return turnovers_[position_];
	}

private:
	const DoubleMap* left_reflector_;
	DoubleMap inward_map_;
	DoubleMap outward_map_;
	std::bitset<NUM_LETTERS> turnovers_;
	Letter position_{0};
	Letter ring_{0};
};

} // namespace bombe

#endif // BOMBE_ROTOR_H
