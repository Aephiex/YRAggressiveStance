#pragma region Ares Copyrights
/*
 *Copyright (c) 2008+, All Ares Contributors
 *All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *3. All advertising materials mentioning features or use of this software
 *   must display the following acknowledgement:
 *   This product includes software developed by the Ares Contributors.
 *4. Neither the name of Ares nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY ITS CONTRIBUTORS ''AS IS'' AND ANY
 *EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *DISCLAIMED. IN NO EVENT SHALL THE ARES CONTRIBUTORS BE LIABLE FOR ANY
 *DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma endregion

#pragma once

#include <Windows.h>
#include <ranges>
#include "Template.h"

#include "INIParser.h"
#include "SavegameDef.h"

#include <InfantryTypeClass.h>
#include <AircraftTypeClass.h>
#include <UnitTypeClass.h>
#include <BuildingTypeClass.h>
#include <WarheadTypeClass.h>
#include <SuperWeaponTypeClass.h>
#include <FootClass.h>
#include <Powerups.h>
#include <VocClass.h>
#include <VoxClass.h>
#include <CRT.h>
#include <LocomotionClass.h>

namespace detail
{
	template <typename T, bool allocate = false>
	inline bool read(T& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		if (parser.ReadString(pSection, pKey))
		{
			using base_type = std::remove_pointer_t<T>;
			auto const pValue = parser.value();
			T parsed;
			if constexpr (allocate)
				parsed = base_type::FindOrAllocate(pValue);
			else
				parsed = base_type::Find(pValue);

			if (parsed || INIClass::IsBlank(pValue))
			{
				value = parsed;
				return true;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, pValue);
			}
		}

		return false;
	}

	template <>
	inline bool read<bool>(bool& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		bool buffer;

		if (parser.ReadBool(pSection, pKey, &buffer))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid boolean value [1, true, yes, 0, false, no]");
		}

		return false;
	}

	template <>
	inline bool read<int>(int& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		int buffer;

		if (parser.ReadInteger(pSection, pKey, &buffer))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid number");
		}

		return false;
	}

	template <>
	inline bool read<unsigned short>(unsigned short& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		int buffer;

		if (parser.ReadInteger(pSection, pKey, &buffer))
		{
			value = static_cast<unsigned short>(buffer);
			return true;
		}

		return false;
	}

	template <>
	inline bool read<BYTE>(BYTE& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		int buffer;

		if (parser.ReadInteger(pSection, pKey, &buffer))
		{
			if (buffer <= 255 && buffer >= 0)
			{
				value = static_cast<BYTE>(buffer); // shut up shut up shut up C4244
				return true;
			}
			else
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid number between 0 and 255 inclusive.");
			}
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid number");
		}

		return false;
	}

	template <>
	inline bool read<float>(float& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		double buffer;

		if (parser.ReadDouble(pSection, pKey, &buffer))
		{
			value = static_cast<float>(buffer);
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid floating point number");
		}

		return false;
	}

	template <>
	inline bool read<double>(double& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		double buffer;

		if (parser.ReadDouble(pSection, pKey, &buffer))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid floating point number");
		}

		return false;
	}

	template <>
	inline bool read<Point2D>(Point2D& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		if (parser.Read2Integers(pSection, pKey, (int*)&value))
			return true;

		return false;
	}

	template <>
	inline bool read<Vector2D<double>>(Vector2D<double>& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		if (parser.Read2Doubles(pSection, pKey, (double*)&value))
			return true;

		return false;
	}

	template <>
	inline bool read<Vector3D<float>>(Vector3D<float>& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		if (parser.Read<float, 3>(pSection, pKey, (float*)&value))
			return true;

		return false;
	}

	template <>
	inline bool read<CoordStruct>(CoordStruct& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		if (parser.Read3Integers(pSection, pKey, (int*)&value))
			return true;

		return false;
	}

	template <>
	inline bool read<ColorStruct>(ColorStruct& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		ColorStruct buffer;

		if (parser.Read3Bytes(pSection, pKey, reinterpret_cast<byte*>(&buffer)))
		{
			value = buffer;
			return true;
		}
		else if (!parser.empty())
		{
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a valid R,G,B color");
		}

		return false;
	}

	template <>
	inline bool read<PartialVector2D<int>>(PartialVector2D<int>& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		value.ValueCount = parser.ReadMultipleIntegers(pSection, pKey, (int*)&value, 2);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	template <>
	inline bool read<PartialVector2D<double>>(PartialVector2D<double>& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		value.ValueCount = parser.ReadMultipleDoubles(pSection, pKey, (double*)&value, 2);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	template <>
	inline bool read<PartialVector3D<int>>(PartialVector3D<int>& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		value.ValueCount = parser.ReadMultipleIntegers(pSection, pKey, (int*)&value, 3);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	template <>
	inline bool read<PartialVector3D<double>>(PartialVector3D<double>& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		value.ValueCount = parser.ReadMultipleDoubles(pSection, pKey, (double*)&value, 3);

		if (value.ValueCount > 0)
			return true;

		return false;
	}

	// Wait, I know it's ugly to copy paste the same shit every single time, I know compile time reflexion is easy
	// but first you need to make sure no one else is fucking around, which is still common atm
	template <typename T, bool allocate = false> requires std::is_enum_v<T>
	inline bool read(T& value, INI_EX& parser, const char* pSection, const char* pKey) = delete;

	template <>
	inline bool read<Mission>(Mission& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		if (parser.ReadString(pSection, pKey))
		{
			auto const mission = MissionControlClass::FindIndex(parser.value());

			if (mission != Mission::None)
			{
				value = mission;
				return true;
			}
			else if (!parser.empty())
			{
				Debug::INIParseFailed(pSection, pKey, parser.value(), "Invalid Mission name");
			}
		}

		return false;
	}

	template <typename T>
	void parse_values(std::vector<T>& vector, INI_EX& parser, const char* pSection, const char* pKey)
	{
		for (auto&& part : std::string_view { parser.value() } | std::views::split(','))
		{
			T buffer = T();
			std::string_view&& cur { part.begin(),part.end() };
			auto pCur = cur.data();
			// you're on a buffer so you can play this shit like that
			const_cast<char&>(*cur.end()) = 0;
			if (Parser<T>::Parse(pCur, &buffer))
				vector.push_back(buffer);
			else if (!INIClass::IsBlank(pCur))
				Debug::INIParseFailed(pSection, pKey, pCur);
		}
	}

	template <typename Lookuper, typename T>
	void parse_indexes(std::vector<T>& vector, INI_EX& parser, const char* pSection, const char* pKey)
	{
		for (auto&& part : std::string_view { parser.value() } | std::views::split(','))
		{
			std::string_view&& cur { part.begin(),part.end() };
			// you forgot to trim, suckers
			auto pCur = cur.data() + cur.find_first_not_of(" \t\r");
			*const_cast<char*>(cur.data() + cur.find_last_not_of(" \t\r") + 1) = 0;
			int idx = Lookuper::FindIndex(pCur);
			if (idx != -1)
				vector.push_back(idx);
			else if (!INIClass::IsBlank(pCur))
				Debug::INIParseFailed(pSection, pKey, pCur);
		}
	}
}


// Valueable

template <typename T>
template <bool Allocate>
void __declspec(noinline) Valueable<T>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	detail::read<T, Allocate>(this->Value, parser, pSection, pKey);
}

template <typename T>
bool Valueable<T>::Load(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange)
{
	return Savegame::ReadYRAggressiveStanceStream(Stm, this->Value, RegisterForChange);
}

template <typename T>
bool Valueable<T>::Save(YRAggressiveStanceStreamWriter& Stm) const
{
	return Savegame::WriteYRAggressiveStanceStream(Stm, this->Value);
}


// ValueableIdx

template <typename Lookuper>
void __declspec(noinline) ValueableIdx<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		const char* val = parser.value();
		int idx = Lookuper::FindIndex(val);

		if (idx != -1 || INIClass::IsBlank(val))
			this->Value = idx;
		else
			Debug::INIParseFailed(pSection, pKey, val);
	}
}


// Nullable

template <typename T>
template <bool Allocate>
void __declspec(noinline) Nullable<T>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		const char* val = parser.value();

		if (!_strcmpi(val, "<default>") || INIClass::IsBlank(val))
			this->Reset();
		else if (detail::read<T, Allocate>(this->Value, parser, pSection, pKey))
			this->HasValue = true;
	}
}

template <typename T>
bool Nullable<T>::Load(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange)
{
	this->Reset();
	auto ret = Savegame::ReadYRAggressiveStanceStream(Stm, this->HasValue);

	if (ret && this->HasValue)
		ret = Savegame::ReadYRAggressiveStanceStream(Stm, this->Value, RegisterForChange);

	return ret;
}

template <typename T>
bool Nullable<T>::Save(YRAggressiveStanceStreamWriter& Stm) const
{
	auto ret = Savegame::WriteYRAggressiveStanceStream(Stm, this->HasValue);

	if (this->HasValue)
		ret = Savegame::WriteYRAggressiveStanceStream(Stm, this->Value);

	return ret;
}


// NullableIdx

template <typename Lookuper>
void __declspec(noinline) NullableIdx<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		const char* val = parser.value();
		int idx = Lookuper::FindIndex(val);

		if (idx != -1 || INIClass::IsBlank(val))
		{
			this->Value = idx;
			this->HasValue = true;
		}
		else
		{
			Debug::INIParseFailed(pSection, pKey, val);
		}
	}
}


// Promotable

template <typename T>
void __declspec(noinline) Promotable<T>::Read(INI_EX& parser, const char* const pSection, const char* const pBaseFlag, const char* const pSingleFlag)
{

	// read the common flag, with the trailing dot being stripped
	char flagName[0x40];
	auto const pSingleFormat = pSingleFlag ? pSingleFlag : pBaseFlag;
	auto res = _snprintf_s(flagName, _TRUNCATE, pSingleFormat, "");
	if (res > 0 && flagName[res - 1] == '.')
	{
		flagName[res - 1] = '\0';
	}

	T placeholder;
	if (detail::read(placeholder, parser, pSection, flagName))
		this->SetAll(placeholder);

	// read specific flags
	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Rookie");
	detail::read(this->Rookie, parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Veteran");
	detail::read(this->Veteran, parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "Elite");
	detail::read(this->Elite, parser, pSection, flagName);
};

template <typename T>
bool Promotable<T>::Load(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange)
{
	return Savegame::ReadYRAggressiveStanceStream(Stm, this->Rookie, RegisterForChange)
		&& Savegame::ReadYRAggressiveStanceStream(Stm, this->Veteran, RegisterForChange)
		&& Savegame::ReadYRAggressiveStanceStream(Stm, this->Elite, RegisterForChange);
}

template <typename T>
bool Promotable<T>::Save(YRAggressiveStanceStreamWriter& Stm) const
{
	return Savegame::WriteYRAggressiveStanceStream(Stm, this->Rookie)
		&& Savegame::WriteYRAggressiveStanceStream(Stm, this->Veteran)
		&& Savegame::WriteYRAggressiveStanceStream(Stm, this->Elite);
}


// ValueableVector

template <typename T>
void __declspec(noinline) ValueableVector<T>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();
		detail::parse_values<T>(*this, parser, pSection, pKey);
	}
}

template <typename T>
bool ValueableVector<T>::Load(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange)
{
	size_t size = 0;
	if (Savegame::ReadYRAggressiveStanceStream(Stm, size, RegisterForChange))
	{
		this->clear();
		this->reserve(size);

		for (size_t i = 0; i < size; ++i)
		{
			value_type buffer = value_type();
			Savegame::ReadYRAggressiveStanceStream(Stm, buffer, false);
			this->push_back(std::move(buffer));

			if (RegisterForChange)
				Swizzle swizzle(this->back());
		}

		return true;
	}

	return false;
}

template <>
inline bool ValueableVector<bool>::Load(YRAggressiveStanceStreamReader& stm, bool registerForChange)
{
	size_t size = 0;
	if (Savegame::ReadYRAggressiveStanceStream(stm, size, registerForChange))
	{
		this->clear();

		for (size_t i = 0; i < size; ++i)
		{
			bool value;

			if (!Savegame::ReadYRAggressiveStanceStream(stm, value, false))
				return false;

			this->emplace_back(value);
		}

		return true;
	}

	return false;
}

template <typename T>
bool ValueableVector<T>::Save(YRAggressiveStanceStreamWriter& Stm) const
{
	auto size = this->size();
	if (Savegame::WriteYRAggressiveStanceStream(Stm, size))
	{
		for (auto const& item : *this)
		{
			if (!Savegame::WriteYRAggressiveStanceStream(Stm, item))
				return false;
		}

		return true;
	}

	return false;
}

template <>
inline bool ValueableVector<bool>::Save(YRAggressiveStanceStreamWriter& stm) const
{
	auto size = this->size();
	if (Savegame::WriteYRAggressiveStanceStream(stm, size))
	{
		for (bool item : *this)
		{
			if (!Savegame::WriteYRAggressiveStanceStream(stm, item))
				return false;
		}

		return true;
	}

	return false;
}

// NullableVector

template <typename T>
void __declspec(noinline) NullableVector<T>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();
		auto const non_default = _strcmpi(parser.value(), "<default>");
		this->hasValue = non_default;

		if (non_default)
			detail::parse_values<T>(*this, parser, pSection, pKey);
	}
}

template <typename T>
bool NullableVector<T>::Load(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange)
{
	this->clear();

	if (Savegame::ReadYRAggressiveStanceStream(Stm, this->hasValue, RegisterForChange))
		return !this->hasValue || ValueableVector<T>::Load(Stm, RegisterForChange);

	return false;
}

template <typename T>
bool NullableVector<T>::Save(YRAggressiveStanceStreamWriter& Stm) const
{
	if (Savegame::WriteYRAggressiveStanceStream(Stm, this->hasValue))
		return !this->hasValue || ValueableVector<T>::Save(Stm);

	return false;
}


// ValueableIdxVector

template <typename Lookuper>
void __declspec(noinline) ValueableIdxVector<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();
		detail::parse_indexes<Lookuper>(*this, parser, pSection, pKey);
	}
}


// NullableIdxVector

template <typename Lookuper>
void __declspec(noinline) NullableIdxVector<Lookuper>::Read(INI_EX& parser, const char* pSection, const char* pKey)
{
	if (parser.ReadString(pSection, pKey))
	{
		this->clear();
		auto const non_default = _strcmpi(parser.value(), "<default>") != 0;
		this->hasValue = non_default;

		if (non_default)
		{
			detail::parse_indexes<Lookuper>(*this, parser, pSection, pKey);
		}
	}
}

// Damageable

template <typename T>
void __declspec(noinline) Damageable<T>::Read(INI_EX& parser, const char* const pSection, const char* const pBaseFlag, const char* const pSingleFlag)
{
	// read the common flag, with the trailing dot being stripped
	char flagName[0x40];
	auto const pSingleFormat = pSingleFlag ? pSingleFlag : pBaseFlag;
	auto res = _snprintf_s(flagName, _TRUNCATE, pSingleFormat, "");

	if (res > 0 && flagName[res - 1] == '.')
		flagName[res - 1] = '\0';

	this->BaseValue.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "ConditionYellow");
	this->ConditionYellow.Read(parser, pSection, flagName);

	_snprintf_s(flagName, _TRUNCATE, pBaseFlag, "ConditionRed");
	this->ConditionRed.Read(parser, pSection, flagName);
};

template <typename T>
bool Damageable<T>::Load(YRAggressiveStanceStreamReader& Stm, bool RegisterForChange)
{
	return Savegame::ReadYRAggressiveStanceStream(Stm, this->BaseValue, RegisterForChange)
		&& Savegame::ReadYRAggressiveStanceStream(Stm, this->ConditionYellow, RegisterForChange)
		&& Savegame::ReadYRAggressiveStanceStream(Stm, this->ConditionRed, RegisterForChange);
}

template <typename T>
bool Damageable<T>::Save(YRAggressiveStanceStreamWriter& Stm) const
{
	return Savegame::WriteYRAggressiveStanceStream(Stm, this->BaseValue)
		&& Savegame::WriteYRAggressiveStanceStream(Stm, this->ConditionYellow)
		&& Savegame::WriteYRAggressiveStanceStream(Stm, this->ConditionRed);
}
