#include "reflector.h"

namespace bombe {

ReflectorModel getReflectorModel(bool is_m4, size_t model_number)
{
	if(is_m4)
	{
		model_number += 100;
	}

	return static_cast<ReflectorModel>(model_number);
}

Reflector::Reflector(ReflectorModel model)
{
	std::string_view wiring;
	switch(model)
	{
	case ReflectorModel::REGULAR_B:
		wiring = "AYBRCUDHEQFSGLIPJXKNMOTZVW";
		break;

	case ReflectorModel::REGULAR_C:
		wiring = "AFBVCPDJEIGOHYKRLZMXNWQTSU";
		break;

	case ReflectorModel::REGULAR_A:
		wiring = "AEBJCMDZFLGYHXIVKWNROQPUST";
		break;

	case ReflectorModel::THIN_B:
		wiring = "AEBNCKDQFUGYHWIJLOMPRXSZTV";
		break;

	case ReflectorModel::THIN_C:
		wiring = "ARBDCOEJFNGTHKIVLMPWQZSXUY";
		break;

	default:
		throw std::invalid_argument("Invalid reflector model");
	}

	std::array<Letter, NUM_LETTERS> wiring_letters;
	char2Letter(wiring, wiring_letters);

	for(size_t pair = 0; pair < (NUM_LETTERS / 2); ++pair)
	{
		const Letter l1 = wiring_letters[pair * 2];
		const Letter l2 = wiring_letters[pair * 2 + 1];
		(*this)[l1] = l2;
		(*this)[l2] = l1;
	}
	extendMap(*this);
}

} // namespace bombe
