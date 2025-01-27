#pragma once
#include <TechnoTypeClass.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>

class Matrix3D;

class TechnoTypeExt
{
public:
	using base_type = TechnoTypeClass;

	static constexpr DWORD Canary = 0x11111111;
	static constexpr size_t ExtPointerOffset = 0xDF4;

	class ExtData final : public Extension<TechnoTypeClass>
	{
	public:
		Valueable<bool> AggressiveStance;
		Nullable<bool> AggressiveStance_Togglable;
		Valueable<bool> AggressiveStance_Exempt;
		ValueableIdx<VocClass> VoiceEnterAggressiveStance;
		ValueableIdx<VocClass> VoiceExitAggressiveStance;


		ExtData(TechnoTypeClass* OwnerObject) : Extension<TechnoTypeClass>(OwnerObject)
			, AggressiveStance { false }
			, AggressiveStance_Togglable {}
			, AggressiveStance_Exempt { false }
			, VoiceEnterAggressiveStance { -1 }
			, VoiceExitAggressiveStance { -1 }
		{ }

		virtual ~ExtData() = default;
		virtual void LoadFromINIFile(CCINIClass* pINI) override;
		virtual void Initialize() override;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }

		virtual void LoadFromStream(YRAggressiveStanceStreamReader& Stm) override;
		virtual void SaveToStream(YRAggressiveStanceStreamWriter& Stm) override;

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

	class ExtContainer final : public Container<TechnoTypeExt>
	{
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;

	static TechnoTypeClass* GetTechnoType(ObjectTypeClass* pType);
};
