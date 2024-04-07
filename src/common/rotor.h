#ifndef BOMBE_ROTOR_H
#define BOMBE_ROTOR_H

#include "types.h"

#include <bitset>
#include <vector>

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

	void setRing(Letter ring_position);

	bool isTurnover() const
	{
		return turnovers_[position_];
	}

private:
	DoubleMap inward_map_;
	DoubleMap outward_map_;
	const DoubleMap* left_reflector_;
	Letter position_{0};
	Letter ring_{0};
	std::bitset<NUM_LETTERS> turnovers_;
	std::vector<Letter> turnover_letters_;
};

} // namespace bombe

#endif // BOMBE_ROTOR_H
