#ifndef BOMBE_REFLECTOR_H
#define BOMBE_REFLECTOR_H

#include "types.h"

namespace bombe {

enum class ReflectorModel
{
	REGULAR_B = 1,
	REGULAR_C = 2,
	THIN_B = 101,
	THIN_C = 102,
};

ReflectorModel getReflectorModel(bool is_m4, size_t model_number);

class Reflector : public DoubleMap
{
public:
	Reflector(ReflectorModel model);
};

} // namespace bombe

#endif // BOMBE_REFLECTOR_H
