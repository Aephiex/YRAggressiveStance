#include "Body.h"

#include <AircraftClass.h>
#include <HouseClass.h>
#include <ScenarioClass.h>

TechnoExt::ExtContainer TechnoExt::ExtMap;

TechnoExt::ExtData::~ExtData()
{
}

// =============================
// load / save

template <typename T>
void TechnoExt::ExtData::Serialize(T& Stm)
{
	Stm
		.Process(this->TypeExtData)
		.Process(this->AggressiveStance)
		;
}

void TechnoExt::ExtData::LoadFromStream(YRAggressiveStanceStreamReader& Stm)
{
	Extension<TechnoClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void TechnoExt::ExtData::SaveToStream(YRAggressiveStanceStreamWriter& Stm)
{
	Extension<TechnoClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}

void TechnoExt::ExtData::InitAggressiveStance()
{
	this->AggressiveStance = this->TypeExtData->AggressiveStance.Get();
}

bool TechnoExt::ExtData::GetAggressiveStance() const
{
	// if this is a passenger then obey the configuration of the transport
	if (auto pTransport = this->OwnerObject()->Transporter)
	{
		auto pTransportExt = TechnoExt::ExtMap.Find(pTransport);
		return pTransportExt->GetAggressiveStance();
	}

	return this->AggressiveStance;
}

void TechnoExt::ExtData::ToggleAggressiveStance()
{
	
	if (this->AggressiveStance)
	{
		// toggle off aggressive stance
		this->AggressiveStance = false;
		// stop current target
		this->OwnerObject()->QueueMission(Mission::Guard, true);
		this->OwnerObject()->SetTarget(nullptr);
	}
	else
	{
		// toggle on aggressive stance
		this->AggressiveStance = true;
	}
}

bool TechnoExt::ExtData::CanToggleAggressiveStance()
{
	if (!this->TypeExtData->AggressiveStance_Togglable.isset())
	{
		// Only techno that are armed and open-topped can be aggressive stance.
		if (!(this->OwnerObject()->IsArmed() || this->TypeExtData->OwnerObject()->OpenTopped))
		{
			this->TypeExtData->AggressiveStance_Togglable = false;
			return false;
		}

		// Engineers and Agents are default to not allow aggressive stance.
		if (auto pInfantryTypeClass = abstract_cast<InfantryTypeClass*>(this->TypeExtData->OwnerObject()))
		{
			if (pInfantryTypeClass->Engineer || pInfantryTypeClass->Agent)
			{
				this->TypeExtData->AggressiveStance_Togglable = false;
				return false;
			}
		}

		this->TypeExtData->AggressiveStance_Togglable = true;
		return true;
	}

	return this->TypeExtData->AggressiveStance_Togglable.Get(true);
}

bool TechnoExt::LoadGlobals(YRAggressiveStanceStreamReader& Stm)
{
	return Stm
		.Success();
}

bool TechnoExt::SaveGlobals(YRAggressiveStanceStreamWriter& Stm)
{
	return Stm
		.Success();
}

// =============================
// container

TechnoExt::ExtContainer::ExtContainer() : Container("TechnoClass") { }

TechnoExt::ExtContainer::~ExtContainer() = default;


// =============================
// container hooks

DEFINE_HOOK(0x6F3260, TechnoClass_CTOR, 0x5)
{
	GET(TechnoClass*, pItem, ESI);

	TechnoExt::ExtMap.TryAllocate(pItem);

	return 0;
}

DEFINE_HOOK(0x6F4500, TechnoClass_DTOR, 0x5)
{
	GET(TechnoClass*, pItem, ECX);

	TechnoExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x70C250, TechnoClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x70BF50, TechnoClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(TechnoClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	TechnoExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x70C249, TechnoClass_Load_Suffix, 0x5)
{
	TechnoExt::ExtMap.LoadStatic();

	return 0;
}

DEFINE_HOOK(0x70C264, TechnoClass_Save_Suffix, 0x5)
{
	TechnoExt::ExtMap.SaveStatic();

	return 0;
}
