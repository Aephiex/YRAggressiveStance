#pragma once

#include "Commands.h"
#include <ObjectClass.h>
#include <AbstractClass.h>
#include <TechnoClass.h>
#include <FootClass.h>
#include <map>

// Select next idle harvester
class AggressiveStanceClass : public CommandClass
{
public:
	static std::map<TechnoClass*, bool> AggressiveStanceMap;
	// CommandClass
	virtual const char* GetName() const override;
	virtual const wchar_t* GetUIName() const override;
	virtual const wchar_t* GetUICategory() const override;
	virtual const wchar_t* GetUIDescription() const override;
	virtual void Execute(WWKey eInput) const override;
};
