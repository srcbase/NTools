#ifndef _AUTILITY_H_
#define _AUTILITY_H_
#pragma once
#include <ios>
#include <ostream>
#include <iomanip>
//#pragma warning(disable:4996)

#include "AC/basedef.h"

//#include <boost/archive/iterators/base64_from_binary.hpp>
//#include <boost/archive/iterators/insert_linebreaks.hpp>
//#include <boost/archive/iterators/transform_width.hpp>
//#include <boost/archive/iterators/ostream_iterator.hpp>

namespace gtc {
	// ¹«¹²º¯Êý¾²Ì¬¿â
	static std::wostream& outClassBegin(std::wostream &os, const std::wstring &cls, int indent)
	{
		os << std::setw(indent) << std::left << L"" << cls.c_str() << L"\n";
		os << std::setw(indent) << L"" << std::setw(2) << L"[" << L"\n";
		return os;
	}
	static std::wostream& outClassEnd(std::wostream &os, const std::wstring &cls, int indent)
	{
		os << std::setw(indent) << std::left << L"" << std::setw(2) << L"]" << cls.c_str() << L"\n";
		return os;
	}

	template<class Value>
	static std::wostream& outClassItem(std::wostream &os, const std::wstring &title, Value &val, int indent)
	{
		os << std::right << std::setw(indent+10) /*<< " *" << std::setw(16)*/ << title.c_str() /* << std::setw(16)*/ << "  >  " << val << std::endl;// "\n";
		return os;
	}
	template<class Type>
	static std::wostream& outBaseClass(std::wostream &os, Type &ty, int indent)
	{
		os << std::setw(indent) << std::left << L"" << L"base class:" << "\n" << ty.description(indent);
		return os;
	}



}
#endif
