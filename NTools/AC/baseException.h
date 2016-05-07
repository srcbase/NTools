#ifndef __BASEEXCEPTION_H__
#define __BASEEXCEPTION_H__

#include <exception>
#include <string>
namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	baseException类	2015-1-8
//	我的异常基类
//	参考：OGRE
////////////////////////////////////////////////////////////////////////////////////////////////////////
class baseException : public std::exception {
public:
	// 异常码，所有自定义的异常都对应一个值
	enum BSExceptionCode {
		BSExceptionCodeDefault					= 0,
		BSExceptionCodeCannotWriteToFile		= 1,		// 写文件（或设备）失败
		BSExceptionCodeInvalidState				,			// 对象处于无效状态（即：需要用到的对象未准备好）
		BSExceptionCodeInvalidParams			,			// 无效参数
		BSExceptionCodeRenderingAPIError		,			// 不支持调用的API函数 
		BSExceptionCodeDuplicateItem			,			// 对象已经存在
		BSExceptionCodeItemNotFound				,			// 对象不存在
		BSExceptionCodeFileNotFound				,			// 文件不存在
		BSExceptionCodeInternalError			,			// 内部错误，如流设备内部错误
		BSExceptionCodeParseFailed				,			// 对数据解析，转换失败

		// 开源库
		BSExceptionCodeDBAssertionFailed		= 1000,		// 数据库相关
		BSExceptionCodeCurlAssertionFailed		,			// 与libcurl库相关
		BSExceptionCodeJSONAssertionFailed		,			// 与json库相关
		BSExceptionCodeXUnzipFailed				,			// 与xunzip库相关
		BSExceptionCodeXZipFailed				,			// 与xzip库相关

		BSExceptionCodeNotImplemented						// 未实现的功能
	};

protected:
	baseException(int number = 0);
public:
	baseException(int number, const std::wstring &desc, const wchar_t *pFunction);
	baseException(int number, const std::wstring &desc, const wchar_t *pType, const wchar_t *pFunction, const wchar_t *pFile, int line);
	baseException(const baseException &exc);
	baseException& operator=(const baseException &exc);
	~baseException() throw();


public:
	virtual const std::wstring& description() const;
	// Override std::exception::what
	const char* what() const throw();
public:
	virtual int getNumber() const throw() {					return m_iNumber;						}
	virtual const std::wstring& getType() const {			return m_wsType;						}
	virtual const std::wstring& getDescription() const {	return m_wsDescription;					}
	virtual int getLine() const {							return m_iLine;							}
	virtual const std::wstring& getFunction() const {		return m_wsFunction;					}
	virtual const std::wstring& getFile() const {			return m_wsFile;						}
protected:
	int				m_iNumber;					// 错误码
	std::wstring	m_wsType;					// 类型
	std::wstring	m_wsDescription;			// 描述
	mutable std::wstring m_wsFullDescription;	// 
	
	int				m_iLine;					// 异常发生的行号，对应 __LINE__
	std::wstring	m_wsFile;					// 所属文件，对应 __FILEW__
	std::wstring	m_wsFunction;					// 发生源，如函数名，对应 __FUNCTIONW__
	

};


//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-31 个性化异常类声明实现宏
#ifndef GTC_EXCEPT_IMPLEMENT
#define GTC_EXCEPT_IMPLEMENT(CLS, BASE, TYPE)	\
	class CLS : public BASE {	\
	public:						\
		CLS(int number, const std::wstring &description, const wchar_t *pFunction, const wchar_t *pfile, int line) \
			: BASE(number, description, TYPE, pFunction, pfile, line) \
		{} \
	};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	  类		2015-3-31
//	异常类的具体实现
////////////////////////////////////////////////////////////////////////////////////////////////////////
GTC_EXCEPT_IMPLEMENT(exceptionCannotWriteToFile,		baseException, L"exceptionCannotWriteToFile")
GTC_EXCEPT_IMPLEMENT(exceptionInvalidState,				baseException, L"exceptionInvalidState")
GTC_EXCEPT_IMPLEMENT(exceptionInvalidParams,			baseException, L"exceptionInvalidParams")
GTC_EXCEPT_IMPLEMENT(exceptionRenderingAPIError,		baseException, L"exceptionRenderingAPIError")
GTC_EXCEPT_IMPLEMENT(exceptionDuplicateItem,			baseException, L"exceptionDuplicateItem")
GTC_EXCEPT_IMPLEMENT(exceptionItemNotFound,				baseException, L"exceptionItemNotFound")
GTC_EXCEPT_IMPLEMENT(exceptionFileNotFound,				baseException, L"exceptionFileNotFound")
GTC_EXCEPT_IMPLEMENT(exceptionInternalError,			baseException, L"exceptionInternalError")
GTC_EXCEPT_IMPLEMENT(exceptionParseFailed,				baseException, L"exceptionParseFailed")

GTC_EXCEPT_IMPLEMENT(exceptionDBAssertionFailed,		baseException, L"exceptionDBAssertionFailed")
GTC_EXCEPT_IMPLEMENT(exceptionCurlAssertionFailed,		baseException, L"exceptionCurlAssertionFailed")
GTC_EXCEPT_IMPLEMENT(exceptionJSONAssertionFailed,		baseException, L"exceptionJSONAssertionFailed")
GTC_EXCEPT_IMPLEMENT(exceptionCodeXUnzipFailed,			baseException, L"exceptionCodeXUnzipFailed")
GTC_EXCEPT_IMPLEMENT(exceptionCodeXZipFailed,			baseException, L"exceptionCodeXZipFailed")

GTC_EXCEPT_IMPLEMENT(exceptionNotImplemented,			baseException, L"exceptionNotImplemented")


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	exceptionFactory  类		2015-3-31
//	异常工厂类，用来抛出异常
////////////////////////////////////////////////////////////////////////////////////////////////////////
class exceptionFactory {
private:
	exceptionFactory() {}
public:
	static void throwException(baseException::BSExceptionCode code, int iNumber, 
						const std::wstring &wsDesc, const wchar_t* function, const wchar_t* file, long line)
	{
		switch (code)
		{
		case baseException::BSExceptionCodeCannotWriteToFile:		// 写文件（或设备）失败
			throw exceptionCannotWriteToFile(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeInvalidState:			// 对象处于无效状态（即：需要用到的对象未准备好）
			throw exceptionInvalidState(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeInvalidParams:			// 无效参数
			throw exceptionInvalidParams(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeRenderingAPIError:		// 不支持调用的API函数 
			throw exceptionRenderingAPIError(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeDuplicateItem:			// 对象已经存在
			throw exceptionDuplicateItem(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeItemNotFound:			// 对象不存在
			throw exceptionItemNotFound(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeFileNotFound:			// 文件不存在
			throw exceptionFileNotFound(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeInternalError:			// 内部错误，如流设备内部错误
			throw exceptionInternalError(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeParseFailed:				// 对数据解析，转换失败
			throw exceptionParseFailed(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeDBAssertionFailed:		// 数据库相关
			throw exceptionDBAssertionFailed(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeCurlAssertionFailed:		// libcurl
			throw exceptionCurlAssertionFailed(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeJSONAssertionFailed:		// json
			throw exceptionJSONAssertionFailed(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeXUnzipFailed:			// 与xunzip库相关
			throw exceptionCodeXUnzipFailed(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeXZipFailed:				// 与xzip库相关
			throw exceptionCodeXZipFailed(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeNotImplemented:			// 未实现的功能
			throw exceptionNotImplemented(iNumber, wsDesc, function, file, line);
			break;
		case baseException::BSExceptionCodeDefault:
		default:
			throw baseException(iNumber, wsDesc, function, L"baseException", file, line);
			break;
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-31 代码中使用的异常宏
#ifndef GTC_EXCEPT
#define GTC_EXCEPT_DEFAULT(desc, code)				gtc::exceptionFactory::throwException(gtc::baseException::BSExceptionCodeDefault, code, desc, __FUNCTIONW__, __FILEW__, __LINE__)
#define GTC_EXCEPT(typeCode, desc)					gtc::exceptionFactory::throwException(typeCode, typeCode, desc, __FUNCTIONW__, __FILEW__, __LINE__)
#define GTC_EXCEPT_EX(typeCode, errorNumber, desc)	gtc::exceptionFactory::throwException(typeCode, errorNumber, desc, __FUNCTIONW__, __FILEW__, __LINE__)
#else
#define GTC_EXCEPT_EX(typeCode, errorNumber, desc)	GTC_EXCEPT(typeCode, desc)
#endif
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
////	idlNode类	2015-1-8
////	通用异常类宏，参考POCO 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define GTC_DECLARE_EXCEPTION_CODE(CLS, BASE, CODE) \
//	class CLS: public BASE														\
//	{																				\
//	public:																			\
//		CLS(int code = CODE);														\
//		CLS(const std::wstring& msg, int code = CODE);								\
//		CLS(const std::wstring& msg, const std::wstring& arg, int code = CODE);		\
//		CLS(const std::wstring& msg, const baseException& exc, int code = CODE);	\
//		CLS(const CLS& exc);														\
//		~CLS() throw();																\
//		CLS& operator = (const CLS& exc);											\
//		const char* name() const throw();											\
//		const char* className() const throw();										\
//		baseException* clone() const;												\
//		void rethrow() const;														\
//	};
//
//#define GTC_DECLARE_EXCEPTION(CLS, BASE) \
//	GTC_DECLARE_EXCEPTION_CODE(CLS, BASE, 0)
//
//#define GTC_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)													\
//	CLS::CLS(int code): BASE(code)																	\
//	{																								\
//	}																								\
//	CLS::CLS(const std::wstring& msg, int code): BASE(msg, code)										\
//	{																								\
//	}																								\
//	CLS::CLS(const std::wstring& msg, const std::wstring& arg, int code): BASE(msg, arg, code)		\
//	{																								\
//	}																								\
//	CLS::CLS(const std::wstring& msg, const baseException& exc, int code): BASE(msg, exc, code)	\
//	{																								\
//	}																								\
//	CLS::CLS(const CLS& exc): BASE(exc)																\
//	{																								\
//	}																								\
//	CLS::~CLS() throw()																				\
//	{																								\
//	}																								\
//	CLS& CLS::operator = (const CLS& exc)															\
//	{																								\
//		BASE::operator = (exc);																		\
//		return *this;																				\
//	}																								\
//	const char* CLS::name() const throw()															\
//	{																								\
//		return NAME;																				\
//	}																								\
//	const char* CLS::className() const throw()														\
//	{																								\
//		return typeid(*this).name();																\
//	}																								\
//	baseException* CLS::clone() const																\
//	{																								\
//		return new CLS(*this);																		\
//	}																								\
//	void CLS::rethrow() const																		\
//	{																								\
//		throw *this;																				\
//	}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
////	通用异常类	2015-1-8
//GTC_DECLARE_EXCEPTION(logicException, baseException)
//GTC_DECLARE_EXCEPTION(runtimeException, baseException)
//GTC_DECLARE_EXCEPTION(formatException, baseException)
//GTC_DECLARE_EXCEPTION(applicationException, baseException)



}
#endif