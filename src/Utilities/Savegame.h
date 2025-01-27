#pragma once

#include "Stream.h"

#include <memory>
#include <type_traits>

namespace Savegame
{
	template <typename T>
	bool ReadYRAggressiveStanceStream(YRAggressiveStanceStreamReader& Stm, T& Value, bool RegisterForChange = true);

	template <typename T>
	bool WriteYRAggressiveStanceStream(YRAggressiveStanceStreamWriter& Stm, const T& Value);

	template <typename T>
	T* RestoreObject(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange = true);

	template <typename T>
	bool PersistObject(YRAggressiveStanceStreamWriter& Stm, const T* pValue);

	template <typename T>
	struct YRAggressiveStanceStreamObject
	{
		bool ReadFromStream(YRAggressiveStanceStreamReader& Stm, T& Value, bool RegisterForChange) const;
		bool WriteToStream(YRAggressiveStanceStreamWriter& Stm, const T& Value) const;
	};

	template <typename T>
	struct ObjectFactory
	{
		std::unique_ptr<T> operator() (YRAggressiveStanceStreamReader& Stm) const
		{
			return std::make_unique<T>();
		}
	};
}
