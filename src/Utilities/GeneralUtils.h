#pragma once
#include <StringTable.h>
#include <CCINIClass.h>
#include <CellSpread.h>

#include <Helpers/Iterators.h>
#include <Helpers/Enumerators.h>

#include <string.h>
#include <iterator>
#include <vector>
#include <string>

#define MIN(x) std::numeric_limits<x>::min()
#define MAX(x) std::numeric_limits<x>::max()

class GeneralUtils
{
public:
	static bool IsValidString(const char* str);
	static void IntValidCheck(int* source, const char* section, const char* tag, int defaultValue, int min = MIN(int), int max = MAX(int));
	static void DoubleValidCheck(double* source, const char* section, const char* tag, double defaultValue, double min = MIN(double), double max = MAX(double));
	static const wchar_t* LoadStringOrDefault(const char* key, const wchar_t* defaultValue);
	static const wchar_t* LoadStringUnlessMissing(const char* key, const wchar_t* defaultValue);

	template<typename T>
	static constexpr T FastPow(T x, size_t n)
	{
		// Real fast pow calc x^n in O(log(n))
		T result = 1;
		T base = x;
		while (n)
		{
			if (n & 1) result *= base;
			base *= base;
			n >>= 1;
		}
		return result;
	}
};
