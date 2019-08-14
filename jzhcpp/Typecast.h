#ifndef TYPE_CAST_H
#define TYPE_CAST_H

#include "Define.h"

namespace TypeCast{
	static const char* strue = "true";
	static const char* sfalse = "false";

	template <typename Target, typename Source>
	struct Converter
	{
		;
	};

	// to numeric
	template <typename Source>
	struct Converter<short, Source>
	{
		static short convert(const Source& source) {
			return std::atoi(source);
		}
	};
	
	// unsign to deal
	template <typename Source>
	struct Converter<unsigned short, Source>
	{
		static unsigned short convert(const Source& source) {
			return std::atoi(source);
		}
	};

	template <typename Source>
	struct Converter<int, Source> 
	{
		static int convert(const Source& source) {
			return std::atoi(source);
		}
	};

	// 64λ��long int��ͬ
	template <typename Source>
	struct Converter<unsigned int, Source>
	{
		static unsigned int convert(const Source& source) {
			return std::stoul(source);
		}
	};

	template <typename Source>
	struct Converter<long, Source> 
	{
		static long convert(const Source& source) {
			return std::atol(source);
		}
	};

	template <typename Source>
	struct Converter<long long, Source> 
	{
		static long long convert(const Source& source) {
			return std::atoll(source);
		}
	};

	template <typename Source>
	struct Converter<unsigned long long, Source>
	{
		static long long convert(const Source& source) {
			return std::stoull(source);
		}
	};

	template <typename Source>
	struct Converter<double, Source> 
	{
		static double convert(const Source& source) {
			return std::atof(source);
		}
	};

	template <typename Source>
	struct Converter<float, Source> {
		static float convert(const Source& source) {
			return (float)std::atof(source);
		}
	};

	// to bool
	template <typename Source>
	struct Converter<bool, Source> 
	{
		// <���Source��integer�ŷ���bool���ͣ��������ʧ��>
		static typename std::enable_if <std::is_integral<Source>::value, bool>::type convert(Source source) {
			return !!source;
		}
	};

	static bool checkbool(const char* source, const size_t len, const char* str)
	{
		for (size_t i = 0; i < len; ++i) {
			if (source[i] != str[i]) {
				return false;
			}
		}
		return true;
	}

	static bool convert(const char* source) {
		unsigned int len = strlen(source);
		if (4 != len && 5 != len) {
			throw std::invalid_argument("argument is invalid");
		}
		bool r = true;
		if (4 == len) {
			r = checkbool(source, len, strue);
			if (r) 
				return true;
		}
		else {
			r = checkbool(source, len, sfalse);
			if (r)
				return false;
		}
		throw std::invalid_argument("argument is invalid");
	}

	template <>
	struct Converter<bool, std::string>
	{
		static bool convert(const std::string& source) {
			return TypeCast::convert(source.c_str());
		}
	};

	template <>
	struct Converter<bool, char*>
	{
		static bool convert(const char* source) {
			return TypeCast::convert(source);
		}
	};

	template <>
	struct Converter<bool, const char*>
	{
		static bool convert(const char* source) {
			return TypeCast::convert(source);
		}
	};


	template <unsigned N>
	static bool convert(const char(&source)[N]) {
		unsigned int len = N;
		if (4 != len && 5 != len) {
			throw std::invalid_argument("argument is invalid");
		}
		bool r = true;
		if (4 == len) {
			r = checkbool(source, len, strue);
			if (r)
				return true;
		}
		else {
			r = checkbool(source, len, sfalse);
			if (r)
				return false;
		}
		throw std::invalid_argument("argument is invalid");
	}

	template <unsigned N>
	struct Converter<bool, const char[N]>
	{
		static bool convert(const char(&source)[N]) {
			return TypeCast::convert(source);
		}
	};

	/* ��֧������ arr[] ת���� bool 
	template <unsigned int N>
	struct Converter<bool, char[N]>
	{
		static bool convert(const char(&source)[N]) {
			return typecast::convert<N>(source);
		}
	};*/ 

	// to string
	template <typename Source>
	struct Converter<std::string, Source>
	{
		static std::string convert(const Source& source) {
			return std::to_string(source);
		}
	};

	template <>
	struct Converter<std::string, bool>
	{
		static std::string convert(const bool& source) {
			if (source) {
				return std::string("true");
			} else {
				return std::string("false");
			}
		}
	};

	// <���Ͳ�һ��ʱ����Source���ͣ�����Converter����>
	template <typename Target, typename Source> 
	typename std::enable_if <!std::is_same <Target, Source>::value, Target>::type lexical_cast(const Source& source) {
		return TypeCast::Converter<Target, Source>::convert(source);
	}

	// <����һ��ֱ�ӷ���>
	template <typename Target, typename Source>
	typename std::enable_if <std::is_same <Target, Source>::value, Target>::type lexical_cast(const Source& source) {
		return source;
	}
}

#endif  // TYPE_CAST_H