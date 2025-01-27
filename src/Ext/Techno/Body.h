#pragma once
#include <InfantryClass.h>
#include <AnimClass.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>
#include <Utilities/Macro.h>
#include <Ext/TechnoType/Body.h>

class BulletClass;

class TechnoExt
{
public:
	using base_type = TechnoClass;

	static constexpr DWORD Canary = 0x55555555;
	static constexpr size_t ExtPointerOffset = 0x34C;

	class ExtData final : public Extension<TechnoClass>
	{
	public:
		TechnoTypeExt::ExtData* TypeExtData;
		bool AggressiveStance;                  // Aggressive stance that will auto target buildings

		ExtData(TechnoClass* OwnerObject) : Extension<TechnoClass>(OwnerObject)
			, TypeExtData { nullptr }
			, AggressiveStance { false }
		{ }

		void OnEarlyUpdate();
		void UpdateTypeData(TechnoTypeClass* currentType);
		virtual ~ExtData() override;
		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }
		virtual void LoadFromStream(YRAggressiveStanceStreamReader& Stm) override;
		virtual void SaveToStream(YRAggressiveStanceStreamWriter& Stm) override;

		void InitAggressiveStance();
		bool GetAggressiveStance() const;
		void ToggleAggressiveStance();
		bool CanToggleAggressiveStance();

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

	class ExtContainer final : public Container<TechnoExt>
	{
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;

	static bool LoadGlobals(YRAggressiveStanceStreamReader& Stm);
	static bool SaveGlobals(YRAggressiveStanceStreamWriter& Stm);
};
