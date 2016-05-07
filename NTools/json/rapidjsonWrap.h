#ifndef __RAPIDJSONWRAP_H__
#define __RAPIDJSONWRAP_H__
#include "AC/basedef.h"
#include "json/rapidjson/document.h"

namespace gtc {
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-10-29 错误码
typedef enum {
	BEJsonCodeOk					= 0,	// 成功
	BEJsonCodeInvalidArgument		,		// 无效参数
	BEJsonCodeKeyNotFound			,		// 对象不存在
	BEJsonCodeTypeError				,		// 类型不匹配
}BEJsonCode;

static std::wstring _errorInfoJson(BEJsonCode code);

#define JSON_EXCEPT2(code, pdesc) GTC_EXCEPT_EX(gtc::baseException::BSExceptionCodeJSONAssertionFailed, code, pdesc)
#define JSON_EXCEPT(code) GTC_EXCEPT_EX(gtc::baseException::BSExceptionCodeJSONAssertionFailed, code, _errorInfoJson(code))

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	rapidJsonWrap  类		2015-10-29
//	针对rapidjson的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class rapidJsonWrap
{
private:
	rapidJsonWrap();
	rapidJsonWrap(const rapidJsonWrap &rjson);
	rapidJsonWrap& operator=(const rapidJsonWrap &rjson);
public:
	explicit rapidJsonWrap(const std::string &strjson);
	explicit rapidJsonWrap(rapidjson::Value *pVal);

	//bool emptyDoc() const {				return m_doc.Empty();								}
	/****************************************************************************************
		<< --- 	selectObject	2015-10-29 --- >>         
		说明：按指定路径选择对象作为当前操作对象
		参数：
			strPath	= 对象路径，名key用‘:’分割，如：key1:key2:key3
		返回值：对象是否被选中
	*****************************************************************************************/
	rapidjson::Value* selectObject(const std::string &strPath);
	rapidjson::Value* selectObject(const SStringArrayType &vecObjectPath);
	void selectEmpty();

	/****************************************************************************************
		<< --- 	nextItem	2015-10-29 --- >>         
		说明：移到数组对象的下一个原始
		参数：
		返回值：是否存在
	*****************************************************************************************/
	rapidjson::Value* nextItem();
	/****************************************************************************************
		<< --- 	asXXX	2015-10-29 --- >>         
		说明：以下asXXX函数针对m_pVal对象，得到其包含的对应值
		参数：
			strKey	= 要查找值的key名称，可以是相对多级名称哦，如：key4:key5
		返回值：
	*****************************************************************************************/
	bool asBoolean(const std::string &strKey);
	int asInt(const std::string &strKey);
	unsigned int asUInt(const std::string &strKey);
	int64_t asInt64(const std::string &strKey);
	uint64_t asUInt64(const std::string &strKey);
	double asDouble(const std::string &strKey);
	std::wstring asString(const std::string &strKey, UINT codePage = CP_UTF8);
//protected:
	// 从当前对象pCurVal中查找key名=strKey的对象
	static rapidjson::Value* nextObject(rapidjson::Value *pCurVal, const std::string &strKey);

	rapidjson::Value* findValue(const std::string &strKey);
private:
	rapidjson::Document m_doc;
	rapidjson::Value	*m_pVal;

	rapidjson::Value	*m_pArrayVal;
	int					m_iValCurIndex;

};



} // gtc
#endif // __RAPIDJSONWRAP_H__