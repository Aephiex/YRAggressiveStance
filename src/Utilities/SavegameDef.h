#pragma once

// include this file whenever something is to be saved.

#include "Savegame.h"
#include <optional>
#include <vector>
#include <map>
#include <bitset>
#include <memory>

#include <ArrayClasses.h>
#include <FileSystem.h>
#include <FileFormats/SHP.h>
#include <RulesClass.h>
#include <SidebarClass.h>

#include "Swizzle.h"
#include "Debug.h"

namespace Savegame
{
	template <typename T>
	concept ImplementsUpperCaseSaveLoad = requires (YRAggressiveStanceStreamWriter & stmWriter, YRAggressiveStanceStreamReader & stmReader, T & value, bool registerForChange)
	{
		value.Save(stmWriter);
		value.Load(stmReader, registerForChange);
	};

	template <typename T>
	concept ImplementsLowerCaseSaveLoad = requires (YRAggressiveStanceStreamWriter & stmWriter, YRAggressiveStanceStreamReader & stmReader, T & value, bool registerForChange)
	{
		value.save(stmWriter);
		value.load(stmReader, registerForChange);
	};

	#pragma warning(push)
	#pragma warning(disable: 4702) // MSVC isn't smart enough and yells about unreachable code

	template <typename T>
	bool ReadYRAggressiveStanceStream(YRAggressiveStanceStreamReader& stm, T& value, bool registerForChange)
	{
		if constexpr (ImplementsUpperCaseSaveLoad<T>)
			return value.Load(stm, registerForChange);

		else if constexpr (ImplementsLowerCaseSaveLoad<T>)
			return value.load(stm, registerForChange);

		YRAggressiveStanceStreamObject<T> item;
		return item.ReadFromStream(stm, value, registerForChange);
	}

	template <typename T>
	bool WriteYRAggressiveStanceStream(YRAggressiveStanceStreamWriter& stm, const T& value)
	{
		if constexpr (ImplementsUpperCaseSaveLoad<T>)
			return value.Save(stm);

		if constexpr (ImplementsLowerCaseSaveLoad<T>)
			return value.save(stm);

		YRAggressiveStanceStreamObject<T> item;
		return item.WriteToStream(stm, value);
	}

	#pragma warning(pop)

	template <typename T>
	T* RestoreObject(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange)
	{
		T* ptrOld = nullptr;
		if (!Stm.Load(ptrOld))
			return nullptr;

		if (ptrOld)
		{
			std::unique_ptr<T> ptrNew = ObjectFactory<T>()(Stm);

			if (Savegame::ReadYRAggressiveStanceStream(Stm, *ptrNew, RegisterForChange))
			{
				YRAggressiveStanceSwizzle::RegisterChange(ptrOld, ptrNew.get());
				return ptrNew.release();
			}
		}

		return nullptr;
	}

	template <typename T>
	bool PersistObject(YRAggressiveStanceStreamWriter& Stm, const T* pValue)
	{
		if (!Savegame::WriteYRAggressiveStanceStream(Stm, pValue))
			return false;

		if (pValue)
			return Savegame::WriteYRAggressiveStanceStream(Stm, *pValue);

		return true;
	}

	template <typename T>
	bool YRAggressiveStanceStreamObject<T>::ReadFromStream(YRAggressiveStanceStreamReader& Stm, T& Value, bool RegisterForChange) const
	{
		bool ret = Stm.Load(Value);

		if (RegisterForChange)
			Swizzle swizzle(Value);

		return ret;
	}

	template <typename T>
	bool YRAggressiveStanceStreamObject<T>::WriteToStream(YRAggressiveStanceStreamWriter& Stm, const T& Value) const
	{
		Stm.Save(Value);
		return true;
	}


	// specializations

	template <typename T>
	struct Savegame::YRAggressiveStanceStreamObject<VectorClass<T>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, VectorClass<T>& Value, bool RegisterForChange) const
		{
			Value.Clear();
			int Capacity = 0;

			if (!Stm.Load(Capacity))
				return false;

			Value.Reserve(Capacity);

			for (auto ix = 0; ix < Capacity; ++ix)
			{
				if (!Savegame::ReadYRAggressiveStanceStream(Stm, Value.Items[ix], RegisterForChange))
					return false;
			}

			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const VectorClass<T>& Value) const
		{
			Stm.Save(Value.Capacity);

			for (auto ix = 0; ix < Value.Capacity; ++ix)
			{
				if (!Savegame::WriteYRAggressiveStanceStream(Stm, Value.Items[ix]))
					return false;
			}

			return true;
		}
	};

	template <typename T>
	struct Savegame::YRAggressiveStanceStreamObject<DynamicVectorClass<T>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, DynamicVectorClass<T>& Value, bool RegisterForChange) const
		{
			Value.Clear();
			int Capacity = 0;

			if (!Stm.Load(Capacity))
				return false;

			Value.Reserve(Capacity);

			if (!Stm.Load(Value.Count) || !Stm.Load(Value.CapacityIncrement))
				return false;

			for (auto ix = 0; ix < Value.Count; ++ix)
			{
				if (!Savegame::ReadYRAggressiveStanceStream(Stm, Value.Items[ix], RegisterForChange))
					return false;
			}

			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const DynamicVectorClass<T>& Value) const
		{
			Stm.Save(Value.Capacity);
			Stm.Save(Value.Count);
			Stm.Save(Value.CapacityIncrement);

			for (auto ix = 0; ix < Value.Count; ++ix)
			{
				if (!Savegame::WriteYRAggressiveStanceStream(Stm, Value.Items[ix]))
					return false;
			}

			return true;
		}
	};

	template <typename T>
	struct Savegame::YRAggressiveStanceStreamObject<TypeList<T>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, TypeList<T>& Value, bool RegisterForChange) const
		{
			if (!Savegame::ReadYRAggressiveStanceStream<DynamicVectorClass<T>>(Stm, Value, RegisterForChange))
				return false;

			return Stm.Load(Value.unknown_18);
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const TypeList<T>& Value) const
		{
			if (!Savegame::WriteYRAggressiveStanceStream<DynamicVectorClass<T>>(Stm, Value))
				return false;

			Stm.Save(Value.unknown_18);
			return true;
		}
	};

	template <>
	struct Savegame::YRAggressiveStanceStreamObject<CounterClass>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, CounterClass& Value, bool RegisterForChange) const
		{
			if (!Savegame::ReadYRAggressiveStanceStream<VectorClass<int>>(Stm, Value, RegisterForChange))
				return false;

			return Stm.Load(Value.Total);
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const CounterClass& Value) const
		{
			if (!Savegame::WriteYRAggressiveStanceStream<VectorClass<int>>(Stm, Value))
				return false;

			Stm.Save(Value.Total);
			return true;
		}
	};

	template <size_t Size>
	struct Savegame::YRAggressiveStanceStreamObject<std::bitset<Size>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, std::bitset<Size>& Value, bool RegisterForChange) const
		{
			unsigned char value = 0;
			for (auto i = 0u; i < Size; ++i)
			{
				auto pos = i % 8;

				if (pos == 0 && !Stm.Load(value))
					return false;

				Value.set(i, ((value >> pos) & 1) != 0);
			}

			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const std::bitset<Size>& Value) const
		{
			unsigned char value = 0;
			for (auto i = 0u; i < Size; ++i)
			{
				auto pos = i % 8;

				if (Value[i])
					value |= 1 << pos;

				if (pos == 7 || i == Size - 1)
				{
					Stm.Save(value);
					value = 0;
				}
			}

			return true;
		}
	};

	template <>
	struct Savegame::YRAggressiveStanceStreamObject<std::string>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, std::string& Value, bool RegisterForChange) const
		{
			size_t size = 0;

			if (Stm.Load(size))
			{
				std::vector<char> buffer(size);

				if (!size || Stm.Read(reinterpret_cast<byte*>(buffer.data()), size))
				{
					Value.assign(buffer.begin(), buffer.end());
					return true;
				}
			}
			return false;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const std::string& Value) const
		{
			Stm.Save(Value.size());
			Stm.Write(reinterpret_cast<const byte*>(Value.c_str()), Value.size());

			return true;
		}
	};

	template <typename T>
	struct Savegame::YRAggressiveStanceStreamObject<std::unique_ptr<T>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, std::unique_ptr<T>& Value, bool RegisterForChange) const
		{
			Value.reset(RestoreObject<T>(Stm, RegisterForChange));
			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const std::unique_ptr<T>& Value) const
		{
			return PersistObject(Stm, Value.get());
		}
	};

	template <typename T>
	struct Savegame::YRAggressiveStanceStreamObject<std::optional<T>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, std::optional<T>& Value, bool RegisterForChange) const
		{
			bool hasValue = false;
			if (!Stm.Load(hasValue))
				return false;

			if (hasValue)
				return Savegame::ReadYRAggressiveStanceStream(Stm, *Value, RegisterForChange);
			else
				Value.reset();

			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const std::optional<T>& Value) const
		{
			Stm.Save(Value.has_value());

			if (Value.has_value())
				return Savegame::WriteYRAggressiveStanceStream(Stm, *Value);

			return true;
		}
	};

	template <typename T>
	struct Savegame::YRAggressiveStanceStreamObject<std::vector<T>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, std::vector<T>& Value, bool RegisterForChange) const
		{
			Value.clear();

			size_t Capacity = 0;

			if (!Stm.Load(Capacity))
				return false;

			Value.reserve(Capacity);

			size_t Count = 0;

			if (!Stm.Load(Count))
				return false;

			Value.resize(Count);

			for (auto ix = 0u; ix < Count; ++ix)
			{
				if (!Savegame::ReadYRAggressiveStanceStream(Stm, Value[ix], RegisterForChange))
					return false;
			}

			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const std::vector<T>& Value) const
		{
			Stm.Save(Value.capacity());
			Stm.Save(Value.size());

			for (auto ix = 0u; ix < Value.size(); ++ix)
			{
				if (!Savegame::WriteYRAggressiveStanceStream(Stm, Value[ix]))
					return false;
			}

			return true;
		}
	};

	template <typename TKey, typename TValue>
	struct Savegame::YRAggressiveStanceStreamObject<std::map<TKey, TValue>>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, std::map<TKey, TValue>& Value, bool RegisterForChange) const
		{
			Value.clear();

			size_t Count = 0;
			if (!Stm.Load(Count))
			{
				return false;
			}

			for (auto ix = 0u; ix < Count; ++ix)
			{
				std::pair<TKey, TValue> buffer;
				if (!Savegame::ReadYRAggressiveStanceStream(Stm, buffer, RegisterForChange))
				{
					return false;
				}
				Value.insert(buffer);
			}

			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const std::map<TKey, TValue>& Value) const
		{
			Stm.Save(Value.size());

			for (const auto& item : Value)
			{
				if (!Savegame::WriteYRAggressiveStanceStream(Stm, item))
				{
					return false;
				}
			}
			return true;
		}
	};

	template <>
	struct Savegame::YRAggressiveStanceStreamObject<BuildType>
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, BuildType& Value, bool RegisterForChange) const
		{
			if (!Stm.Load(Value))
				return false;

			if (RegisterForChange)
				Swizzle swizzle(Value.CurrentFactory);

			return true;
		}

		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const BuildType& Value) const
		{
			Stm.Save(Value);
			return true;
		}
	};
}
