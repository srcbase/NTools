#pragma once
#if defined(_WIN32)
	#include <afxwin.h>         // MFC 核心组件和标准组件
	#include <afxext.h>         // MFC 扩展
	#include <ATLComTime.h>
	#include <comutil.h>
	#include <afxdisp.h>
	#include <comdef.h>
#endif
#import "D:\VSTOOLS\system32\msado15.dll" rename("EOF", "ADOEOF")
using namespace ADODB;
// sqlite3
#include "D:\VSTOOLS\AC\sqlite\sqlite3.h"
#pragma comment(lib, "D:\\VSTOOLS\\AC\\sqlite\\sqlite3.lib")

#include "d:\VSTOOLS\AC\ANormal.h"
#include "d:\VSTOOLS\AC\AGeneralObject.h"
#include "d:\VSTOOLS\AC\ATools.h"
namespace acm {

//// 释放记录集
//#define DEF_FREE_RST(prst)		if(prst != NULL)\
//										{\
//											if(prst->GetState() != adStateClosed)\
//												prst->Close();\
//											prst.Release();\
//											prst = NULL;\
//										}
//
// CMSQL 命令目标
class CMSQL
{
protected:
public:
	CMSQL()
		: m_beDBType(acg::BEDBTypeMSSql)
		, m_strLoginer(L"")
		, m_strLoginPW(L"")
		, m_strIP(L"")
		, m_strDBName(L"")
		, m_ptrCNN(nullptr), m_pSqlite(nullptr)
	{
	}
	CMSQL(acg::BEDBType vdbtype, const wchar_t *vlogname, const wchar_t *vlogpw, const wchar_t *vdbname, const wchar_t *vip = L"127.0.0.1")
		: m_beDBType(vdbtype)
		, m_strLoginer(vlogname?vlogname:L"")
		, m_strLoginPW(vlogpw?vlogpw:L"")
		, m_strIP(vip?vip:L"")
		, m_strDBName(vdbname?vdbname:L"")
		, m_ptrCNN(nullptr), m_pSqlite(nullptr)
	{
	}
	CMSQL(const CMSQL& vbsql)
		: m_beDBType(vbsql.m_beDBType)
		, m_strLoginer(vbsql.m_strLoginer)
		, m_strLoginPW(vbsql.m_strLoginPW)
		, m_strIP(vbsql.m_strIP)
		, m_strDBName(vbsql.m_strDBName)
		, m_ptrCNN(nullptr), m_pSqlite(nullptr)
	{
	}
	CMSQL& operator =(const CMSQL &vbsql)
	{
		if(this != &vbsql)
		{
			m_beDBType = vbsql.m_beDBType;
			m_strLoginer = vbsql.m_strLoginer;
			m_strLoginPW = vbsql.m_strLoginPW;
			m_strIP = vbsql.m_strIP;
			m_strDBName = vbsql.m_strDBName;

			m_ptrCNN = nullptr;
			m_pSqlite = nullptr;
		}
		return *this;
	}
		// 描述
	std::wstring description(int indent = 4) const
	{
		std::wostringstream desc;
		acg::outClassBegin(desc, L"CMSQL", indent);
		acg::outClassItem(desc, L"数据库", m_strDBName, indent << 1);
		acg::outClassItem(desc, L"IP", m_strIP, indent << 1);
		acg::outClassItem(desc, L"类型", acg::toStringBEDBType(m_beDBType), indent << 1);
		acg::outClassEnd(desc, L"CMSQL", indent);
		return desc.str();
	}
	virtual ~CMSQL()
	{
		this->clearParameter();
		this->close();
#ifdef DEBUG
		acg::CAApp::debug(L"free %s", description().c_str());
#endif
	}
	//virtual void Serialize(CArchive& ar);
public:
	// static
	static void free(_RecordsetPtr &prst)
	{
		if(prst != nullptr)
		{
			if(prst->GetState() != ::adStateClosed)
				prst->Close();
			prst = nullptr;
		}
	}
	static void free(_CommandPtr &pcmd)
	{
		if(pcmd != nullptr)
		{
			pcmd.Release();
			pcmd = nullptr;
		}
	}
	
public:
	// 
	bool open(acg::BEDBType vdbtype, const wchar_t *vlogname, const wchar_t *vlogpw, const wchar_t *vdbname, const wchar_t *vip);
	bool open(const wchar_t *vpfullinifile, const wchar_t *vpsection);
	//bool open(const wchar_t *vpcnnstring);
	bool open(void);
	void close(void);
	bool isOpen(void) const{
		if(m_beDBType == acg::BEDBTypeSqlite3)
			return m_pSqlite != nullptr;
		return m_ptrCNN != nullptr && m_ptrCNN->GetState() == adStateOpen;
	}
	// 执行
	COleVariant executeScalar(const wchar_t *vpcmdtext, acg::BEDBCmdType vcmdtype = acg::BEDBCmdTypeText);
	int executeNonQuery(LPCTSTR vpcmdtext, acg::BEDBCmdType vcmdtype = acg::BEDBCmdTypeText);
	LPDISPATCH executeReader(LPCTSTR vpcmdtext, acg::BEDBCmdType vcmdtype = acg::BEDBCmdTypeText, LONG vtimeout = 30L);
	// 根据参数执行对表vptable的插入操作，假定参数名就是对应表的字段名
	bool exeInsertByParameter(LPCTSTR vptable);
	// 根据参数执行对表vptable的修改操作，假定参数名就是对应表的字段名
	bool exeUpdateByParameter(LPCTSTR vptable, LPCTSTR vpwhere);
	// 得到链接字符串
	std::wstring getConnectionString(void);
	// 追加参数
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, acg::BEDataType vtype, long vsize, acg::DBVariant &vval, acg::BEParameterDirection vdirection = acg::BEParameterDirectionInput) {
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vtype, vdirection, vsize, vval);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, uint8_t vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, bool vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, const std::wstring &vval, int64_t vsize, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vsize, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, const wchar_t *vval, int64_t vsize, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vsize, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, int16_t vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, int32_t vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, int64_t vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, float vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, double vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, const std::vector<uint8_t> &vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(const wchar_t *vpname, const std::vector<wchar_t> &vval, acg::BEParameterDirection vdir = acg::BEParameterDirectionInput)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		if(!findParameter(vpname))
		{
			acg::CASQLParameter::PtrType ppar = new acg::CASQLParameter(vpname, vval, vdir);
			if(ppar)
				m_vecParameter.push_back(ppar);
			return ppar;
		}
		return nullptr;
	}
	acg::CASQLParameter::PtrType addParameter(acg::CASQLParameter::PtrType vpparam)
	{
		if(vpparam != nullptr && !findParameter(vpparam->getName().c_str()))
		{
			m_vecParameter.push_back(vpparam);
			return vpparam;
		}
		return nullptr;
	}
	void addParameter(acg::CASQLParameter::PtrArrayType &varrparam)
	{
		m_vecParameter.insert(m_vecParameter.end(), varrparam.begin(), varrparam.end());
	}
	void copyParameter(acg::CASQLParameter::PtrArrayType &varrparam)
	{
		clearParameter();
		m_vecParameter = varrparam;
	}
	// 清空参数；只释放目标对象的引用，不释放内存
	void clearParameter(void) {
		m_vecParameter.clear();	
	}
	// 设定参数vpname的值vval
	void setParameterValue(const wchar_t *vpname, acg::DBVariant &vval) {
		acg::CASQLParameter::PtrType ppar = this->findParameter(vpname);
		if(ppar != nullptr)
			ppar->setValue(vval);
	}
	acg::DBVariant getParameterValueDB(const wchar_t * vpname) {
		acg::CASQLParameter::PtrType ppar = this->findParameter(vpname);
		if(ppar != nullptr)
			return ppar->getValue();
		return acg::DBVariant();
	}
	COleVariant getParameterValue(const wchar_t * vpname) {
		acg::CASQLParameter::PtrType ppar = this->findParameter(vpname);
		COleVariant var;
		if(ppar != nullptr)
			acg::CAConvert::toOleVariant(var, ppar->getType(), ppar->getValue());
		return var;
	}
	acg::CASQLParameter::PtrType findParameter(const wchar_t *vpname) {
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		for(acg::CASQLParameter::PtrArrayType::size_type idx = 0; idx < this->m_vecParameter.size(); idx++)
		{
			if(this->m_vecParameter[idx]->getName() == vpname)
				return this->m_vecParameter[idx];
		}
		return nullptr;
	}
	// 事务控制
	void beginTrans(void) {
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(m_pSqlite)
				sqlite3_exec(m_pSqlite, "begin transaction;", nullptr, nullptr, nullptr);
		}
		else
		{
			if(m_ptrCNN != nullptr) m_ptrCNN->BeginTrans();
		}
	}
	void commitTrans(void) {
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(m_pSqlite)
				sqlite3_exec(m_pSqlite, "commit transaction;;", nullptr, nullptr, nullptr);
		}
		else
		{
			if(m_ptrCNN != nullptr) m_ptrCNN->CommitTrans();
		}
	}
	void rollbackTrans(void) {
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(m_pSqlite)
				sqlite3_exec(m_pSqlite, "ROLLBACK transaction;", nullptr, nullptr, nullptr);
		}
		else
		{
			if(m_ptrCNN != nullptr) m_ptrCNN->RollbackTrans();
		}
	}
private:

	void lastError() const {
		if(m_beDBType == acg::BEDBTypeSqlite3)
			acg::CAApp::writeLogs(acg::CAConvert::toWCharFromChar(sqlite3_errmsg(m_pSqlite)));
		else
			acg::CAApp::lastError();

	}

	// 刷新返回参数型的值
	void refreshOutputParameterValue(_CommandPtr& vpcmd) {
		acg::CASQLParameter::PtrArrayType::size_type idx;
		for(idx = 0; idx < m_vecParameter.size(); ++idx)
		{
			if (m_vecParameter[idx]->getDirection() == acg::BEParameterDirectionOutput 
				|| m_vecParameter[idx]->getDirection() == acg::BEParameterDirectionInputOUtput)
			{
				m_vecParameter[idx]->setValue(acg::CAConvert::toDBVariantFromOleVariant(m_vecParameter[idx]->getType(), 
					 vpcmd->Parameters->GetItem(_bstr_t(m_vecParameter[idx]->getName().c_str()))->Value));
			}
		}
	}

	// 构建命令参数
	void buildCommandParameter(sqlite3_stmt *stmt) {
		if(!stmt)
			return;
		COleVariant var;
		acg::CASQLParameter::PtrArrayType::const_iterator iter, end = m_vecParameter.cend();
		int idx;
		try
		{
			for(iter = m_vecParameter.cbegin(), idx = 0; iter != end; ++iter, ++idx)
			{
				switch((*iter)->getType())
				{
				case acg::BEDataTypeNone:		// = 0,		// VT_EMPTY
				//case BEDataTypeNULL:		//  = 1,		// VT_NULL
					sqlite3_bind_null(stmt, idx);
					break;
				case acg::BEDataTypeByte:		// ,			// VT_UI1
				case acg::BEDataTypeBoolean:		// ,		// VT_BOOL
				case acg::BEDataTypeInt16:		// VT_I2
				case acg::BEDataTypeInt32:		// ,		// VT_I4
					{
						int val = 0;
						if(acg::CAConvert::toSqliteValue(val, (*iter)->getValue()))
						{
							sqlite3_bind_int(stmt, idx, val);
						}
						else
						{
							sqlite3_bind_null(stmt, idx);
						}
					}
					break;
				case acg::BEDataTypeInt64:		// ,		// VT_I8
					{
						int64_t val = 0;
						if(acg::CAConvert::toSqliteValue(val, (*iter)->getValue()))
						{
							sqlite3_bind_int64(stmt, idx, val);
						}
						else
						{
							sqlite3_bind_null(stmt, idx);
						}
					}

					break;
				case acg::BEDataTypeFloat:		// ,		// VT_R4
				case acg::BEDataTypeDouble:		// ,		// VT_R8
				case acg::BEDataTypeDate:		// ,			// VT_DATE
				case acg::BEDataTypeTime:		// ,			// VT_DATE
				case acg::BEDataTypeDatetime:	// ,		// VT_DATE
					{
						double val = 0.0;
						if(acg::CAConvert::toSqliteValue(val, (*iter)->getValue()))
						{
							sqlite3_bind_double(stmt, idx, val);
						}
						else
						{
							sqlite3_bind_null(stmt, idx);
						}
					}
					break;
				case acg::BEDataTypeImage:		// ,		// 字节流 VT_ARRAY | VT_UI1。2011-07-12
					{
						std::vector<uint8_t> data;
						if(acg::CAConvert::toSqliteValue(data, (*iter)->getValue()))
						{
							sqlite3_bind_blob(stmt, idx, data.data(), data.size(), nullptr);
						}
						else
						{
							sqlite3_bind_null(stmt, idx);
						}

					}

					break;
				case acg::BEDataTypeString:		// ,		// VT_BSTR
				case acg::BEDataTypeText:		// 			// 大型字符串VT_BSTR
					{
						std::wstring ws;
						if(acg::CAConvert::toSqliteValue(ws, (*iter)->getValue()))
						{
							sqlite3_bind_text16(stmt, idx, ws.data(), ws.length(), nullptr);
						}
						else
						{
							sqlite3_bind_null(stmt, idx);
						}

					}

					break;
				}


				
				//acg::CAConvert::toOleVariant(var, (*iter)->getType(), (*iter)->getValue());
				//vpcmd->Parameters->Append(vpcmd->CreateParameter(_bstr_t((*iter)->getName().c_str()), 
				//	CMSQL::toMSSQLDataType((*iter)->getType()), 
				//	CMSQL::toMSSQLParameterDirection((*iter)->getDirection()), 
				//	(*iter)->getSize(), 
				//	var));

			}

			//for(acg::CASQLParameter::PtrArrayType::size_type idx = 0; idx < this->m_vecParameter.size(); ++idx)
			//{
			//	if((ppar = this->m_vecParameter[idx]) != nullptr)
			//	{
			//		//acg::CAApp::debug(L"%s", ppar->description().c_str());
			//		COleVariant var = acg::CAConvert::toOleVariantFromDBVariant(ppar->getType(), ppar->getValue());
			//		vpcmd->Parameters->Append(vpcmd->CreateParameter(_bstr_t(ppar->getName().c_str()), 
			//			CMSQL::toMSSQLDataType(ppar->getType()), 
			//			CMSQL::toMSSQLParameterDirection(ppar->getDirection()), 
			//			ppar->getSize(), 
			//			var));
			//	}
			//}
		}
		catch(_com_error& e)
		{
			CString str;
			if(iter != end)
				str.Format(_T("%s  [%s]"), (TCHAR*)(e.Description()), (*iter)->getName().c_str());
			else
				str.Format(_T("%s"), (TCHAR*)e.Description());
			acg::CAApp::writeLogs(str, _T("CMSQL::BuildCommandParameter_ERROR_"));
		}
		catch(...)
		{
			acg::CAApp::lastError();
		}
	}

	void buildCommandParameter(_CommandPtr& vpcmd) {
		if(vpcmd == NULL || m_vecParameter.size() <= 0)
			return ;
		//acg::CASQLParameter::PtrType ppar = nullptr;
		COleVariant var;
		acg::CASQLParameter::PtrArrayType::const_iterator iter, end = m_vecParameter.cend();
		try
		{
			for(iter = m_vecParameter.cbegin(); iter != end; ++iter)
			{
				acg::CAConvert::toOleVariant(var, (*iter)->getType(), (*iter)->getValue());
				vpcmd->Parameters->Append(vpcmd->CreateParameter(_bstr_t((*iter)->getName().c_str()), 
					CMSQL::toMSSQLDataType((*iter)->getType()), 
					CMSQL::toMSSQLParameterDirection((*iter)->getDirection()), 
					(*iter)->getSize(), 
					var));

			}

			//for(acg::CASQLParameter::PtrArrayType::size_type idx = 0; idx < this->m_vecParameter.size(); ++idx)
			//{
			//	if((ppar = this->m_vecParameter[idx]) != nullptr)
			//	{
			//		//acg::CAApp::debug(L"%s", ppar->description().c_str());
			//		COleVariant var = acg::CAConvert::toOleVariantFromDBVariant(ppar->getType(), ppar->getValue());
			//		vpcmd->Parameters->Append(vpcmd->CreateParameter(_bstr_t(ppar->getName().c_str()), 
			//			CMSQL::toMSSQLDataType(ppar->getType()), 
			//			CMSQL::toMSSQLParameterDirection(ppar->getDirection()), 
			//			ppar->getSize(), 
			//			var));
			//	}
			//}
		}
		catch(_com_error& e)
		{
			CString str;
			if(iter != end)
				str.Format(_T("%s  [%s]"), (TCHAR*)(e.Description()), (*iter)->getName().c_str());
			else
				str.Format(_T("%s"), (TCHAR*)e.Description());
			acg::CAApp::writeLogs(str, _T("CMSQL::BuildCommandParameter_ERROR_"));
		}
		catch(...)
		{
			acg::CAApp::lastError();
		}
	}
public:
	acg::BEDBType getDBType(void) const {				return m_beDBType;			}
	std::wstring getLoginName(void) const {				return m_strLoginer;		}
	std::wstring getLoginPW(void) const {				return m_strLoginPW;		}
	std::wstring getDataSource(void) const {			return m_strIP;				}
	std::wstring getDatabaseName(void) const {			return m_strDBName;			}
	acg::CASQLParameter::PtrArrayType& getParameterArray(void) {	return m_vecParameter;		}
public:
	/*****************************************************************************************
		<< --- static CMSQL::toRecordset		2014-03-25 --- >>
		说明：将sqlite3_stmt记录集转化成ADODB::RecordsetPtr
		参数：
		stmt		= 被转化的sqlite3记录集
		返回值：转化后的记录集ADODB::RecordsetPtr，调用者负责释放。
	*****************************************************************************************/
	static LPDISPATCH toRecordset(sqlite3_stmt *stmt);
	/*****************************************************************************************
		<< --- CAConvert::toMSSQLDataType		2013-11-20 --- >>
		说明：从自定义的数据类型到MSSql数据类型的转换
		参数：
		dbtype		= 被转换的类型
		返回值：返回转换后的DataTypeEnum类型
	*****************************************************************************************/
	static ADODB::DataTypeEnum toMSSQLDataType(acg::BEDataType dbtype)
	{
		ADODB::DataTypeEnum type = adEmpty;
		switch(dbtype)
		{
		case acg::BEDataTypeNone:		// = 0,		// VT_EMPTY
		//case BEDataTypeNULL:		//  = 1,		// VT_NULL
			type = adEmpty;
			break;
		case acg::BEDataTypeByte:		// ,			// VT_UI1
			type = adUnsignedTinyInt;
			break;
		case acg::BEDataTypeBoolean:		// ,		// VT_BOOL
			type = adBoolean;
			break;
		case acg::BEDataTypeString:		// ,		// VT_BSTR
			type = adVarWChar;
			break;
		case acg::BEDataTypeInt16:		// VT_I2
			type = adSmallInt;
			break;
		case acg::BEDataTypeInt32:		// ,		// VT_I4
			type = adInteger;
			break;
		case acg::BEDataTypeInt64:		// ,		// VT_I8
			type = adBigInt;
			break;
		case acg::BEDataTypeFloat:		// ,		// VT_R4
			type = adSingle;
			break;
		case acg::BEDataTypeDouble:		// ,		// VT_R8
			type = adDouble;
			break;
		case acg::BEDataTypeDate:		// ,			// VT_DATE
		case acg::BEDataTypeTime:		// ,			// VT_DATE
		case acg::BEDataTypeDatetime:	// ,		// VT_DATE
			type = adDate;
			break;
		case acg::BEDataTypeImage:		// ,		// 字节流 VT_ARRAY | VT_UI1。2011-07-12
			type = adLongVarBinary;
			break;
		case acg::BEDataTypeText:		// 			// 大型字符串VT_BSTR
			type = adLongVarWChar;
			break;
		}
		return type;
	}
	/*****************************************************************************************
		<< --- CAConvert::toSqliteDataType		2013-11-20 --- >>
		说明：从自定义的数据类型转换到sqlite3支持的类型
		参数：
		dbtype		= 被转换的类型
		返回值：转换后的sqlite3类型
	*****************************************************************************************/
	static int toSqliteDataType(acg::BEDataType dbtype)
	{
		int type = SQLITE_NULL;
		switch(dbtype)
		{
		case acg::BEDataTypeNone:		// = 0,		// VT_EMPTY
		//case BEDataTypeNULL:		//  = 1,		// VT_NULL
			type = SQLITE_NULL;
			break;
		case acg::BEDataTypeByte:		// ,			// VT_UI1
		case acg::BEDataTypeBoolean:		// ,		// VT_BOOL
		case acg::BEDataTypeInt16:		// VT_I2
		case acg::BEDataTypeInt32:		// ,		// VT_I4
		case acg::BEDataTypeInt64:		// ,		// VT_I8
			type = SQLITE_INTEGER;
			break;
		case acg::BEDataTypeFloat:		// ,		// VT_R4
		case acg::BEDataTypeDouble:		// ,		// VT_R8
		case acg::BEDataTypeDate:		// ,			// VT_DATE
		case acg::BEDataTypeTime:		// ,			// VT_DATE
		case acg::BEDataTypeDatetime:	// ,		// VT_DATE
			type = SQLITE_FLOAT;
			break;
		case acg::BEDataTypeImage:		// ,		// 字节流 VT_ARRAY | VT_UI1。2011-07-12
			type = SQLITE_BLOB;
			break;
		case acg::BEDataTypeString:		// ,		// VT_BSTR
		case acg::BEDataTypeText:		// 			// 大型字符串VT_BSTR
			type = SQLITE_TEXT;
			break;
		}
		return type;
	}
	static CommandTypeEnum toMSSQLCommandType(acg::BEDBCmdType type)
	{
		CommandTypeEnum cmd = ADODB::adCmdUnknown;
		switch(type)
		{
		case acg::BEDBCmdTypeText: //= 1,//adCmdText,	// 
			cmd = ADODB::adCmdText;
			break;
		case acg::BEDBCmdTypeProc: //= 4	//adCmdStoredProc
			cmd = ADODB::adCmdStoredProc;
			break;
		default:
			cmd = ADODB::adCmdUnknown;
			break;
		}
		return cmd;
	}
	static ADODB::ParameterDirectionEnum toMSSQLParameterDirection(acg::BEParameterDirection dir)
	{
		ADODB::ParameterDirectionEnum pd = ADODB::adParamInput;
		switch(dir)
		{
		case acg::BEParameterDirectionUnknown:			//= 0,//adParamUnknown,// = 0,
			pd = ADODB::adParamUnknown;
			break;
		case acg::BEParameterDirectionInput:			//= 1,//adParamInput,// = 1,
			pd = ADODB::adParamInput;
			break;
		case acg::BEParameterDirectionOutput:			//= 2,//adParamOutput,// = 2,
			pd = ADODB::adParamOutput;
			break;
		case acg::BEParameterDirectionInputOUtput:		//= 3,//adParamInputOutput,// = 3,
			pd = ADODB::adParamInputOutput;
			break;
		case acg::BEParameterDirectionReturnValue:		//= 4//adParamReturnValue// = 4
			pd = ADODB::adParamReturnValue;
			break;
		default:
			pd = ADODB::adParamUnknown;
			break;
		}
		return pd;
	}
	static acg::CASQLParameter::PtrType findParameter(const wchar_t *vpname, acg::CASQLParameter::PtrArrayType &vparams)
	{
		if(vpname == nullptr || std::wcslen(vpname) <= 0)
			return nullptr;
		for(acg::CASQLParameter::PtrArrayType::iterator iter = vparams.begin(), end = vparams.end(); iter != end; ++iter)
		{
			if((*iter)->getName() == vpname)
				return *iter;
		}
		/*
		for(acg::CASQLParameter::PtrArrayType::size_type idx = 0; idx < vparams.size(); idx++)
		{
			if(vparams[idx]->getName() == vpname)
				return vparams[idx];
		}*/
		return nullptr;
	}
private:
	acg::BEDBType		m_beDBType;		// 数据库类型
	std::wstring		m_strLoginer;	// 登录名
	std::wstring		m_strLoginPW;	// 密码
	std::wstring		m_strIP;		// 数据库所在机器IP
	std::wstring		m_strDBName;	// 数据库名称

	ADODB::_ConnectionPtr	m_ptrCNN;			// mssql,Oracle,access
	sqlite3					*m_pSqlite;			// sqlite3
	acg::CASQLParameter::PtrArrayType m_vecParameter;// 参数集合

};


}