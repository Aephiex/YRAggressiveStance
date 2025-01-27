#include <HouseClass.h>
#include <Commands/AggressiveStance.h>
#include <Helpers/Macro.h>

// Cursor & target acquisition stuff not directly tied to other features can go here.

DEFINE_HOOK(0x6F858F, TechnoClass_EvaluateObject_AggressiveStance, 0x7)
{
	GET(TechnoClass*, pThis, EDI);
	GET(TechnoClass*, pTarget, ESI);
	if (pThis && pThis->Owner->IsControlledByHuman()
		&& pTarget && pTarget->WhatAmI() == AbstractType::Building)
	{
		if (AggressiveStanceClass::AggressiveStanceMap[pThis]) {
			return 0x6F88BF;
		}
	}
	return 0;
}
