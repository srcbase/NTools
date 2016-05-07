#include "baseException.h"

#include "AC/sysupport.h"
#include <sstream>
namespace gtc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	baseException类	2015-1-8
//	我的异常基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
baseException::baseException(int number /*= 0*/)
	: m_iNumber(number), m_iLine(0)
{
}
baseException::baseException(int number, const std::wstring &desc, const wchar_t *pFunction)
	: m_iNumber(number), m_iLine(0)
	, m_wsDescription(desc), m_wsFunction(pFunction ?pFunction : L"")
{
}
baseException::baseException(int number, const std::wstring &desc, const wchar_t *pType
	, const wchar_t *pFunction, const wchar_t *pFile, int line)
	: m_iNumber(number)
	, m_wsDescription(desc), m_wsType(pType ?pType :L"")
	, m_wsFunction(pFunction ?pFunction :L""), m_wsFile(pFile ?pFile :L""), m_iLine(line)
{
	//, m_wsType(ptype)
	WRITE_LOG(description().c_str());
}
baseException::baseException(const baseException &exc)
	: m_iNumber(exc.m_iNumber)
	, m_wsDescription(exc.m_wsDescription), m_wsType(exc.m_wsType)
	, m_wsFunction(exc.m_wsFunction), m_wsFile(exc.m_wsFile), m_iLine(exc.m_iLine)
{

}

baseException::~baseException() throw()
{
}

baseException& baseException::operator=(const baseException &exc)
{
	if (this != &exc)
	{
		m_iNumber = exc.m_iNumber;
		m_wsDescription = exc.m_wsDescription;
		m_wsType = exc.m_wsType;
		m_wsFunction = exc.m_wsFunction;
		m_wsFile = exc.m_wsFile;
		m_iLine = exc.m_iLine;
	}
	return *this;
}

const std::wstring& baseException::description() const
{
	if (m_wsFullDescription.empty())
	{
		std::wostringstream out;
		out << L"Exception(" << m_wsType << L":" << m_iNumber << L"): "
			<< m_wsDescription
			<< L" in " << m_wsFunction;
		if (m_iLine > 0)
		{
			out << L" at " << m_wsFile << L" (line " << m_iLine << L")";
		}
		m_wsFullDescription = out.str();

	}
	return m_wsFullDescription;
}

const char* baseException::what() const throw()
{
	return NULL;// description.c_str();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//	通用异常类	2015-1-8

//GTC_IMPLEMENT_EXCEPTION(logicException, baseException, "Logic exception")
//GTC_IMPLEMENT_EXCEPTION(runtimeException, baseException, "Runtime exception")
//GTC_IMPLEMENT_EXCEPTION(formatException, baseException, "Bad data format")
//GTC_IMPLEMENT_EXCEPTION(applicationException, baseException, "Application exception")

}