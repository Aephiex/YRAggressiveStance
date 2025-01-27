#include <YRAggressiveStance.h>

#include <LoadOptionsClass.h>

#include <Ext/Techno/Body.h>
#include <Ext/TechnoType/Body.h>

#include <utility>

#pragma region Implementation details

#pragma region Concepts

// a hack to check if some type can be used as a specialization of a template
template <template <class...> class Template, class... Args>
void DerivedFromSpecialization(const Template<Args...>&);

template <class T, template <class...> class Template>
concept DerivedFromSpecializationOf =
	requires(const T & t) { DerivedFromSpecialization<Template>(t); };

template<typename TExt>
concept HasExtMap = requires { { TExt::ExtMap } -> DerivedFromSpecializationOf<Container>; };

template<typename TExt>
concept ExtDataConsiderPointerInvalidation = HasExtMap<TExt> && requires{
	{ TExt::ShouldConsiderInvalidatePointer }->std::convertible_to<const bool>;
}&& TExt::ShouldConsiderInvalidatePointer == true;

template <typename T>
concept Clearable = requires { T::Clear(); };

template <typename T>
concept PointerInvalidationSubscribable =
	requires (void* ptr, bool removed) { T::PointerGotInvalid(ptr, removed); };

template <typename T>
concept GlobalSaveLoadable = requires
{
	T::LoadGlobals(std::declval<YRAggressiveStanceStreamReader&>());
	T::SaveGlobals(std::declval<YRAggressiveStanceStreamWriter&>());
};

template <typename TAction, typename TProcessed, typename... ArgTypes>
concept DispatchesAction =
	requires (ArgTypes... args) { TAction::template Process<TProcessed>(args...); };

#pragma endregion

// calls:
// T::Clear()
// T::ExtMap.Clear()
struct ClearAction
{
	template <typename T>
	static bool Process()
	{
		if constexpr (Clearable<T>)
			T::Clear();
		else if constexpr (HasExtMap<T>)
			T::ExtMap.Clear();

		return true;
	}
};

// calls:
// T::PointerGotInvalid(void*, bool)
// T::ExtMap.PointerGotInvalid(void*, bool)
struct InvalidatePointerAction
{
	template <typename T>
	static bool Process(void* ptr, bool removed)
	{
		if constexpr (PointerInvalidationSubscribable<T>)
			T::PointerGotInvalid(ptr, removed);
		else if constexpr (ExtDataConsiderPointerInvalidation<T>)
			T::ExtMap.PointerGotInvalid(ptr, removed);

		return true;
	}
};

// calls:
// T::LoadGlobals(YRAggressiveStanceStreamReader&)
struct LoadGlobalsAction
{
	template <typename T>
	static bool Process(IStream* pStm)
	{
		if constexpr (GlobalSaveLoadable<T>)
		{
			YRAggressiveStanceByteStream stm(0);
			stm.ReadBlockFromStream(pStm);
			YRAggressiveStanceStreamReader reader(stm);

			return T::LoadGlobals(reader) && reader.ExpectEndOfBlock();
		}
		else
		{
			return true;
		}
	}
};

// calls:
// T::SaveGlobals(YRAggressiveStanceStreamWriter&)
struct SaveGlobalsAction
{
	template <typename T>
	static bool Process(IStream* pStm)
	{
		if constexpr (GlobalSaveLoadable<T>)
		{
			YRAggressiveStanceByteStream stm;
			YRAggressiveStanceStreamWriter writer(stm);

			return T::SaveGlobals(writer) && stm.WriteBlockToStream(pStm);
		}
		else
		{
			return true;
		}
	}
};

// this is a complicated thing that calls methods on classes. add types to the
// instantiation of this type, and the most appropriate method for each type
// will be called with no overhead of virtual functions.
template <typename... RegisteredTypes>
struct TypeRegistry
{
	__forceinline static void Clear()
	{
		dispatch_mass_action<ClearAction>();
	}

	__forceinline static void InvalidatePointer(void* ptr, bool removed)
	{
		dispatch_mass_action<InvalidatePointerAction>(ptr, removed);
	}

	__forceinline static bool LoadGlobals(IStream* pStm)
	{
		return dispatch_mass_action<LoadGlobalsAction>(pStm);
	}

	__forceinline static bool SaveGlobals(IStream* pStm)
	{
		return dispatch_mass_action<SaveGlobalsAction>(pStm);
	}

private:
	// TAction: the method dispatcher class to call with each type
	// ArgTypes: the argument types to call the method dispatcher's Process() method
	template <typename TAction, typename... ArgTypes>
		requires (DispatchesAction<TAction, RegisteredTypes, ArgTypes...> && ...)
	__forceinline static bool dispatch_mass_action(ArgTypes... args)
	{
		// (pack expression op ...) is a fold expression which
		// unfolds the parameter pack into a full expression
		return (TAction::template Process<RegisteredTypes>(args...) && ...);
	}
};

#pragma endregion

// Add more class names as you like
using YRAggressiveStanceTypeRegistry = TypeRegistry <
	TechnoExt,
	TechnoTypeExt
> ;

DEFINE_HOOK(0x7258D0, AnnounceInvalidPointer, 0x6)
{
	GET(AbstractClass* const, pInvalid, ECX);
	GET(bool const, removed, EDX);

	YRAggressiveStanceTypeRegistry::InvalidatePointer(pInvalid, removed);

	return 0;
}

DEFINE_HOOK(0x685659, Scenario_ClearClasses, 0xa)
{
	YRAggressiveStanceTypeRegistry::Clear();
	return 0;
}

DEFINE_HOOK(0x67D32C, SaveGame_YRAggressiveStance, 0x5)
{
	GET(IStream*, pStm, ESI);
	YRAggressiveStanceTypeRegistry::SaveGlobals(pStm);
	return 0;
}

DEFINE_HOOK(0x67E826, LoadGame_YRAggressiveStance, 0x6)
{
	GET(IStream*, pStm, ESI);
	YRAggressiveStanceTypeRegistry::LoadGlobals(pStm);
	return 0;
}
