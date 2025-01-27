#include "Body.h"
#include <HouseClass.h>
#include <Ext/TechnoType/Body.h>

// Cursor & target acquisition stuff not directly tied to other features can go here.

DEFINE_HOOK(0x6F858F, TechnoClass_EvaluateObject_AggressiveStance, 0x7)
{
	GET(TechnoClass*, pThis, EDI);
	GET(TechnoClass*, pTarget, ESI);
	if (pThis && pThis->Owner->IsControlledByHuman()
		&& pTarget && pTarget->WhatAmI() == AbstractType::Building)
	{
		// Fallback to unmodded behavior if the building is an exempt of aggressive stance.
		if (auto pTargetTypeExt = TechnoTypeExt::ExtMap.Find(pTarget->GetTechnoType()))
		{
			if (pTargetTypeExt->AggressiveStance_Exempt)
			{
				return 0;
			}
		}

		if (auto pTechnoExt = TechnoExt::ExtMap.Find(pThis))
		{
			if (pTechnoExt->GetAggressiveStance())
			{
				return 0x6F88BF;
			}
		}
	}
	return 0;
}
