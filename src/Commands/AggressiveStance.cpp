#include "AggressiveStance.h"
#include "Ext/Techno/Body.h"
#include <EventClass.h>
#include <HouseClass.h>
#include <Ext/Event/Body.h>
#include <Utilities/GeneralUtils.h>

const char* AggressiveStanceClass::GetName() const
{
	return "AggressiveStance";
}

const wchar_t* AggressiveStanceClass::GetUIName() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_AGGRESSIVE_STANCE", L"Aggressive Stance");
}

const wchar_t* AggressiveStanceClass::GetUICategory() const
{
	return CATEGORY_CONTROL;
}

const wchar_t* AggressiveStanceClass::GetUIDescription() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_AGGRESSIVE_STANCE_DESC", L"Aggressive Stance");
}

static inline const wchar_t* GetToggleOnPopupMessage()
{
	return GeneralUtils::LoadStringUnlessMissing("MSG:AGGRESSIVE_STANCE_ON", L"%i unit(s) entered Aggressive Stance.");
}

static inline const wchar_t* GetToggleOffPopupMessage()
{
	return GeneralUtils::LoadStringUnlessMissing("MSG:AGGRESSIVE_STANCE_OFF", L"%i unit(s) ceased Aggressive Stance.");
}

void AggressiveStanceClass::Execute(WWKey eInput) const
{
	std::vector<TechnoClass*> TechnoVectorAggressive;
	std::vector<TechnoClass*> TechnoVectorNonAggressive;

	// Get current selected units.
	// If all selected units are at aggressive stance, we should cancel their aggressive stance.
	// Otherwise, we should turn them into aggressive stance.
	bool isAnySelectedUnitTogglable = false;
	bool isAllSelectedUnitAggressiveStance = true;
	for (const auto& pUnit : ObjectClass::CurrentObjects())
	{
		// try to cast to TechnoClass
		TechnoClass* pTechno = abstract_cast<TechnoClass*>(pUnit);

		// if not a techno or is in berserk or is not controlled by the local player then ignore it
		if (!pTechno || pTechno->Berzerk || !pTechno->Owner->IsControlledByCurrentPlayer())
			continue;

		// If not togglable then exclude it from the iteration.
		if (auto pTechnoExt = TechnoExt::ExtMap.Find(pTechno))
		{
			if (pTechnoExt->CanToggleAggressiveStance())
			{
				isAnySelectedUnitTogglable = true;
				if (pTechnoExt->GetAggressiveStance())
				{
					TechnoVectorAggressive.push_back(pTechno);
				}
				else
				{
					isAllSelectedUnitAggressiveStance = false;
					TechnoVectorNonAggressive.push_back(pTechno);
				}
			}
		}
	}

	// If this boolean is false, then none of the selected units are togglable, meaning this hotket doesn't need to do anything.
	if (isAnySelectedUnitTogglable)
	{
		// If all selected units are aggressive stance, then cancel their aggressive stance;
		// otherwise, make all selected units aggressive stance.
		std::vector<TechnoClass*> TechnoVector;
		const wchar_t* Message;
		if (isAllSelectedUnitAggressiveStance)
		{
			TechnoVector = TechnoVectorAggressive;
			Message = GetToggleOffPopupMessage();
		}
		else
		{
			TechnoVector = TechnoVectorNonAggressive;
			Message = GetToggleOnPopupMessage();
		}
		for (auto pTechno : TechnoVector)
		{
			EventExt::RaiseToggleAggressiveStance(pTechno);

			auto pTechnoType = pTechno->GetTechnoType();
			auto pTechnoTypeExt = TechnoTypeExt::ExtMap.Find(pTechnoType);
			int voiceIndex;

			if (isAllSelectedUnitAggressiveStance)
			{
				voiceIndex = pTechnoTypeExt->VoiceExitAggressiveStance.Get();
			}
			else
			{
				voiceIndex = pTechnoTypeExt->VoiceEnterAggressiveStance.Get();
				if (voiceIndex < 0)
				{
					TypeList<int> voiceList = pTechnoType->VoiceAttack.Count ? pTechnoType->VoiceAttack : pTechnoType->VoiceMove;
					if (voiceList.Count)
					{
						unsigned int idxRandom = Randomizer::Global().Random();
						auto index = idxRandom % voiceList.Count;
						voiceIndex = voiceList.GetItem(index);
					}
				}
			}

			pTechno->QueueVoice(voiceIndex);
		}
		wchar_t buffer[0x1000];
		wsprintfW(buffer, Message, TechnoVector.size());
		MessageListClass::Instance->PrintMessage(buffer, RulesClass::Instance->MessageDelay, HouseClass::CurrentPlayer->ColorSchemeIndex);
	}
}
