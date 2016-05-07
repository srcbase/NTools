// D:\VSTOOLS\MC\BSQL.cpp : 实现文件
//

#include "stdafx.h"
#include "MSQL.h"
#include <cwchar>
namespace acm {
// CASQLField

// CMSQL
// 
// CMSQL 成员函数
bool CMSQL::open(acg::BEDBType vdbtype, const wchar_t *vlogname, const wchar_t *vlogpw, const wchar_t *vdbname, const wchar_t *vip)
{
	this->m_beDBType = vdbtype;
	this->m_strLoginer = vlogname;
	this->m_strLoginPW = vlogpw;
	this->m_strDBName = vdbname;
	this->m_strIP = vip;
	return open();
}
bool CMSQL::open(const wchar_t *vpfullinifile, const wchar_t *vpsection)
{
	if(vpfullinifile == NULL || ::lstrlen(vpfullinifile) <= 0 ||
		vpsection == NULL || ::lstrlen(vpsection) <= 0)
		return false;
	TCHAR buf[80] = {0};
	DWORD len = ::GetPrivateProfileString(vpsection, DBKEY_LOGIN, L"", buf, _countof(buf), vpfullinifile);
	buf[len] = '\0';
	this->m_strLoginer = buf;
	len = ::GetPrivateProfileString(vpsection, DBKEY_PASSWORD, L"", buf, _countof(buf), vpfullinifile);
	buf[len] = '\0';
	this->m_strLoginPW = buf;
	len = ::GetPrivateProfileString(vpsection, DBKEY_NAMEORPATH, L"", buf, _countof(buf), vpfullinifile);
	buf[len] = '\0';
	this->m_strDBName = buf;
	len = ::GetPrivateProfileString(vpsection, DBKEY_IP, L"", buf, _countof(buf), vpfullinifile);
	buf[len] = '\0';
	this->m_strIP = buf;
	this->m_beDBType = (acg::BEDBType)::GetPrivateProfileInt(vpsection, DBKEY_TYPE, acg::BEDBTypeMSSql, vpfullinifile);
	/*len = ::GetPrivateProfileString(vpsection, _T("DBType"), _T("MSSQL"), buf, _countof(buf), vpfullinifile);
	buf[len] = '\0';
	if(path.CompareNoCase(_T("MSSQL")) == 0)
		this->m_euDBType = NS_DB::BEDBTypeMSSql;
	else if(path.CompareNoCase(_T("ACCESS")) == 0)
		this->m_euDBType = NS_DB::BEDBTypeAccess;
	else if(path.CompareNoCase(_T("ORACLE")) == 0)
		this->m_euDBType = NS_DB::BEDBTypeOracle;
	else if(path.CompareNoCase(_T("SQLITE3")) == 0)
		this->m_euDBType = NS_DB::BEDBTypeSqlite3;*/
	return open();

}
bool CMSQL::open(void)
{
	bool bret = true;
	wchar_t info[120] = {0};
	try
	{
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(!m_pSqlite)
			{
				int ret = sqlite3_open16(m_strDBName.c_str(), &m_pSqlite);
				if(ret != SQLITE_OK)
				{
					std::swprintf(info, _countof(info), L"打开数据库失败！%s", sqlite3_errmsg16(m_pSqlite));
					throw info;
				}
			}
		}
		else
		{
			if(!m_ptrCNN)
			{
				this->m_ptrCNN.CreateInstance(__uuidof(Connection));
				this->m_ptrCNN->CursorLocation = adUseServer;
				this->m_ptrCNN->IsolationLevel = adXactCursorStability;
				this->m_ptrCNN->ConnectionString = _bstr_t(getConnectionString().c_str());
				if(!SUCCEEDED(this->m_ptrCNN->Open(_bstr_t(L""), _bstr_t(L""), _bstr_t(L""), adConnectUnspecified)))
				{
					throw L"打开数据库失败！";
				}
			}
		}
	}
	catch(_com_error& e)
	{
		bret = false;
		acg::CAApp::writeLogs(e, L"CBSQL_OPEN_ERROR");
		close();
	}
	catch(const wchar_t *perr)
	{
		bret = false;
		acg::CAApp::writeLogs(perr, L"CBSQL_OPEN_ERROR");
		close();
	}
	catch(...)
	{
		bret = false;
		lastError();
		close();
	}
	return bret;
}
void CMSQL::close(void)
{
	if(m_beDBType == acg::BEDBTypeSqlite3)
	{
		if(m_pSqlite)
			sqlite3_close(m_pSqlite);
		m_pSqlite = nullptr;
	}
	else
	{
		if(m_ptrCNN != NULL)
		{
			if(m_ptrCNN->GetState() != adStateClosed)
				m_ptrCNN->Close();
			//m_ptrCNN.Release();
			m_ptrCNN = NULL;
		}
	}

}

// 执行
COleVariant CMSQL::executeScalar(const wchar_t *vpcmdtext, acg::BEDBCmdType vcmdtype)
{
	COleVariant var;
	_RecordsetPtr prst = nullptr;
	_CommandPtr pcmd = nullptr;
	sqlite3_stmt *stmt = nullptr;
	try
	{
		var.Clear();
		var.ChangeType(VT_NULL);
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(sqlite3_prepare16_v2(m_pSqlite, vpcmdtext, -1, &stmt, nullptr) == SQLITE_OK)
			{
				buildCommandParameter(stmt);
				if(sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_count(stmt) > 0)
				{
					// 获取第一行第一列的值
					int type = sqlite3_column_type(stmt, 0);
					switch (type)
					{
					case SQLITE_INTEGER:
						var.ChangeType(VT_I4);
						var.lVal = sqlite3_column_int(stmt, 0);
						break;
					case SQLITE_FLOAT:
						var.ChangeType(VT_R8);
						var.dblVal = sqlite3_column_double(stmt, 0);
						break;
					case SQLITE_TEXT:
						var.ChangeType(VT_BSTR);
						var.bstrVal = _bstr_t(sqlite3_column_text16(stmt, 0));
						break;
					case SQLITE_BLOB:
						{
							uint8_t *pdata = (uint8_t*)sqlite3_column_blob(stmt, 0);
							int datalen = sqlite3_column_bytes(stmt, 0);
							acg::CAConvert::toOleVariant(var, pdata, datalen);
						}
						break;
					case SQLITE_NULL:
					default:
						break;
					}
				}

			}

		}
		else
		{
			if(vcmdtype == acg::BEDBCmdTypeProc)
			{
				pcmd.CreateInstance(__uuidof(Command));
				pcmd->ActiveConnection = this->m_ptrCNN;
				pcmd->CommandText = vpcmdtext;
				//pcmd->CommandTimeout = vtimeout;
				// 创建返回值参数
				pcmd->Parameters->Append(pcmd->CreateParameter(_bstr_t(L"retval"), adInteger, adParamReturnValue, -1, _variant_t(-999l)));
				this->buildCommandParameter(pcmd);
				pcmd->Execute(NULL, NULL, adCmdStoredProc);
				// 得到返回值
				var = pcmd->Parameters->GetItem(_bstr_t(L"retval"))->Value;
				// 刷新输出参数
				refreshOutputParameterValue(pcmd);
			}
			else
			{
				prst = this->executeReader(vpcmdtext, vcmdtype);
				if(prst != NULL && !(prst->ADOEOF && prst->BOF))
				{
					var = prst->GetFields()->GetItem(0L)->GetValue();
				}
			}
		}
	}
	catch(_com_error& e)
	{
		var = vtMissing;
		acg::CAApp::writeLogs(e, L"CMSQL::ExecuteScalar_ERROR_");
	}
	catch(...)
	{
		var = vtMissing;
		lastError();
	}
	// 
	CMSQL::free(pcmd);
	CMSQL::free(prst);
	if(stmt)
		sqlite3_finalize(stmt);
	return var;
}

int CMSQL::executeNonQuery(LPCTSTR vpcmdtext, acg::BEDBCmdType vcmdtype)
{
	// 存储过程则得到返回值,否则得到@@rowcount
	int affected = 0;
	_RecordsetPtr prst = NULL;
	CString sqlcmd;
	COleVariant var;
	try
	{
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(sqlite3_exec(m_pSqlite, _bstr_t(vpcmdtext), nullptr, nullptr, nullptr) == SQLITE_OK)
			{
				affected = sqlite3_changes(m_pSqlite);
			}

		}
		else
		{
			sqlcmd.Format(_T("%s  ;SELECT @@ROWCOUNT"), vpcmdtext);
			prst = this->executeReader(sqlcmd, vcmdtype);
			if(prst != NULL)
			{
				var = prst->NextRecordset(NULL)->GetFields()->GetItem(0L)->GetValue();
				var.ChangeType(VT_I4);
				affected = var.lVal;
			}
		}
	}
	catch(_com_error& e)
	{
		affected = -1;
		acg::CAApp::writeLogs(e, _T("CMSQL::ExecuteNonQuery_ERROR_"));
	}
	catch(...)
	{
		affected = -1;
		lastError();
	}

	// 
	CMSQL::free(prst);

	return affected;
}

LPDISPATCH CMSQL::executeReader(LPCTSTR vpcmdtext, acg::BEDBCmdType vcmdtype, LONG vtimeout)
{
	_RecordsetPtr prst = NULL;
	_CommandPtr pcmd = NULL;
	LPDISPATCH pdis = NULL;
	sqlite3_stmt *stmt = nullptr;
	try
	{
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(sqlite3_prepare16_v2(m_pSqlite, vpcmdtext, -1, &stmt, nullptr) == SQLITE_OK)
			{
				buildCommandParameter(stmt);
				pdis = toRecordset(stmt);
			}
		}
		else
		{
			pcmd.CreateInstance(__uuidof(Command));
			pcmd->ActiveConnection = this->m_ptrCNN;
			pcmd->CommandText = vpcmdtext;
			pcmd->CommandTimeout = vtimeout;
			this->buildCommandParameter(pcmd);
			prst = pcmd->Execute(NULL, NULL, CMSQL::toMSSQLCommandType(vcmdtype));
			if(prst != NULL)
			{
				prst->QueryInterface(IID_IDispatch, (void**)&pdis);
			}
		}
	}
	catch(_com_error& e)
	{
		acg::CAApp::writeLogs(e, _T("CMSQL::ExecuteReader_ERROR_"));
		CMSQL::free(prst);
		pdis = NULL;
	}
	catch(...)
	{
		CMSQL::free(prst);
		pdis = NULL;
		lastError();
	}
	if(stmt)
		sqlite3_finalize(stmt);
	return pdis;
}
// 根据参数执行对表vptable的插入操作，假定参数名就是对应表的字段名
bool CMSQL::exeInsertByParameter(LPCTSTR vptable)
{
	//CString strsql, fields, values;
	if(m_ptrCNN == NULL || vptable == NULL || lstrlen(vptable) <= 0 || m_vecParameter.size() <= 0)
		return false;
	bool bl = true;
	//_RecordsetPtr prst = NULL;
	_CommandPtr pcmd = NULL;
	sqlite3_stmt *stmt = nullptr;
	acg::CASQLParameter::PtrArrayType::const_iterator iter, end = m_vecParameter.end();
	std::wostringstream os;
	try
	{
		os << L"INSERT INTO " << vptable << L"(";
		for(iter = m_vecParameter.begin(); iter != end; ++iter)//	for(acg::CASQLParameter::PtrArrayType::size_type idx = 0, end = m_vecParameter.size(); idx < end; idx++)
		{
			if(*iter)
			{
				os << (*iter)->getName();
				if(iter != (end-1))
					os << L",";
			}
				
		}
		os << L") VALUES(";
		for(iter = m_vecParameter.begin(); iter != end; ++iter)//	for(acg::CASQLParameter::PtrArrayType::size_type idx = 0, end = m_vecParameter.size(); idx < end; idx++)
		{
			os << L"?";
			if(iter != (end-1))
				os << L",";
		}
		if(m_beDBType == acg::BEDBTypeSqlite3)
			os << L");" << std::endl;
		else
			os << L")" << std::endl;
		//acg::CAApp::debug(L"%s", os.str().c_str());
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(sqlite3_prepare16_v2(m_pSqlite, os.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK)
			{
				buildCommandParameter(stmt);
				sqlite3_step(stmt);
			}
		}
		else
		{
			pcmd.CreateInstance(__uuidof(Command));
			pcmd->ActiveConnection = this->m_ptrCNN;
			pcmd->CommandText = os.str().c_str();
			//pcmd->CommandTimeout = vtimeout;
			this->buildCommandParameter(pcmd);
			pcmd->Execute(NULL, NULL, ADODB::adCmdText);
		}

	}
	catch(_com_error& e)
	{
		bl = false;
		CString strsql;
		if(iter != end)
			strsql.Format(_T("%s(Code: %d-%d FieldName:%s)"), (TCHAR*)e.Description(),e.WCode(),e.Error(), (*iter)->getName());
		else
			strsql.Format(_T("%s(Code: %d-%d)"), (TCHAR*)e.Description(),e.WCode(),e.Error());
		acg::CAApp::writeLogs(strsql, _T("CMSQL::ExeInsertByParameter_ERROR_"));
	}
	catch(...)
	{
		bl = false;
		lastError();
	}
	// release
	//CMSQL::free(prst);
	clearParameter();
	if(stmt)
		sqlite3_finalize(stmt);
	return bl;
}

// 根据参数执行对表vptable的修改操作，假定参数名就是对应表的字段名
bool CMSQL::exeUpdateByParameter(LPCTSTR vptable, LPCTSTR vpwhere)
{
	CString swhere, str(vpwhere);
	if(m_ptrCNN == NULL || vptable == NULL || lstrlen(vptable) <= 0 || m_vecParameter.size() <= 0)
		return false;
	bool bl = true;
	//_RecordsetPtr prst = NULL;
	_CommandPtr pcmd = NULL;
	sqlite3_stmt *stmt = nullptr;
	acg::CASQLParameter::PtrType ppar = nullptr;
	std::wostringstream os;
	str.MakeUpper();
	if(!str.IsEmpty())
	{
		if(str.Find(_T("WHERE ")) < 0)
			swhere = CString(_T(" WHERE ")) + CString(vpwhere);
	}
	try
	{
		os << L"UPDATE " << vptable << L" SET ";
		for(acg::CASQLParameter::PtrArrayType::const_iterator iter = m_vecParameter.cbegin(), end = m_vecParameter.cend(); iter != end; ++iter)
		{
			os << (*iter)->getName() << L"=?";
			if(iter != end-1)
				os << L",";
		}
		os << swhere.GetBuffer();
		if(m_beDBType == acg::BEDBTypeSqlite3)
			os << L";" << std::endl;
		else
			os << std::endl;
		//acg::CAApp::debug(L"%s", os.str().c_str());
		if(m_beDBType == acg::BEDBTypeSqlite3)
		{
			if(sqlite3_prepare16_v2(m_pSqlite, os.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK)
			{
				buildCommandParameter(stmt);
				sqlite3_step(stmt);
			}
		}
		else
		{
			pcmd.CreateInstance(__uuidof(Command));
			pcmd->ActiveConnection = this->m_ptrCNN;
			pcmd->CommandText = os.str().c_str();
			this->buildCommandParameter(pcmd);
			pcmd->Execute(NULL, NULL, ADODB::adCmdText);
		}

	}
	catch(_com_error& e)
	{
		bl = FALSE;
		if(ppar != NULL)
			str.Format(_T("%s(Code: %d-%d FieldName:%s)"), (TCHAR*)e.Description(),e.WCode(),e.Error(), ppar->getName().c_str());
		else
			str.Format(_T("%s(Code: %d-%d)"), (TCHAR*)e.Description(),e.WCode(),e.Error());
		acg::CAApp::writeLogs(str, _T("CMSQL::ExeUpdateByParameter_ERROR_"));
	}
	catch(...)
	{
		bl = false;
		acg::CAApp::lastError();
	}
	// release
	if(stmt)
		sqlite3_finalize(stmt);
	clearParameter();
	return bl;
}

std::wstring CMSQL::getConnectionString(void)
{
	//std::wstring cnn;
	wchar_t buf[512] = {0};
	switch(this->m_beDBType)
	{
	case acg::BEDBTypeMSSql:			// = 1,		// MSSQL
		std::swprintf(buf, 512, L"Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s", m_strLoginPW.c_str(), m_strLoginer.c_str(), m_strDBName.c_str(), m_strIP.c_str());
		break;
	case acg::BEDBTypeOracle:		//,			// 
		std::swprintf(buf, 512, L"Extended Properties=PLSQLRSet=1;Provider=OraOLEDB.Oracle.1;User ID=%s;Password=%s;Data Source=(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=%s)(PORT=1521)))(CONNECT_DATA=(SERVICE_NAME=%s)));Persist Security Info=True", m_strLoginer.c_str(), m_strLoginPW.c_str(), m_strIP.c_str(), m_strDBName.c_str());
		break;
	case acg::BEDBTypeAccess:		//,			// 
		std::swprintf(buf, 512, L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Persist Security Info=False", m_strDBName.c_str());
		break;
	case acg::BEDBTypeSqlite3:		//			// 
		std::swprintf(buf, 512, L"Driver={SQLite3 ODBC Driver};Database=%s;StepAPI=0;SyncPragma=;NoTXN=0;Timeout=;ShortNames=0;LongNames=0;NoCreat=0;NoWCHAR=1;FKSupport=0;JournalMode=;OEMCP=0;LoadExt=;BigInt=0;PWD=", m_strDBName.c_str());
		break;
	}
	return buf;
}

/*****************************************************************************************
	<< --- static CMSQL::toRecordset		2014-03-25 --- >>
	说明：将sqlite3_stmt记录集转化成ADODB::RecordsetPtr
	参数：
	stmt		= 被转化的sqlite3记录集
	返回值：转化后的记录集ADODB::RecordsetPtr，调用者负责释放。
*****************************************************************************************/
LPDISPATCH CMSQL::toRecordset(sqlite3_stmt *stmt)
{
	if(!stmt)
		return nullptr;
	_variant_t varnull;
	varnull.vt = VT_ERROR;
	varnull.scode = DISP_E_PARAMNOTFOUND;
	_RecordsetPtr prst = nullptr;
	int col;
	_bstr_t bsname;
	try
	{
		prst.CreateInstance(__uuidof(Recordset));
		const int cols = sqlite3_column_count(stmt);
		// 创建字段
		for(col = 0; col < cols; ++col)
		{
			bsname = sqlite3_column_name16(stmt, col);
			switch (sqlite3_column_type(stmt, col))
			{
			case SQLITE_INTEGER:
				prst->Fields->Append(bsname, ADODB::adInteger, 4, adFldUpdatable);
				break;
			case SQLITE_FLOAT:
				prst->Fields->Append(bsname, ADODB::adDouble, 8, adFldUpdatable);
				break;
			case SQLITE_BLOB:
				prst->Fields->Append(bsname, ADODB::adLongVarBinary, 16, adFldUpdatable);
				break;
			case SQLITE_TEXT:
			default:
				prst->Fields->Append(bsname, ADODB::adVarWChar, 254, adFldUpdatable);
				break;
			}
		}
		prst->Open(varnull, varnull, adOpenKeyset, adLockOptimistic, adCmdText);
		// 
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			prst->AddNew();
			for(col = 0; col < cols; ++col)
			{
				bsname = sqlite3_column_name16(stmt, col);
				// 创建字段
				switch (sqlite3_column_type(stmt, col))
				{
				case SQLITE_INTEGER:
					{
						int val = sqlite3_column_int(stmt, col);
						prst->Fields->GetItem(bsname)->PutValue(val);
					}

					break;
				case SQLITE_FLOAT:
					{
						double val = sqlite3_column_double(stmt, col);
						prst->Fields->GetItem(bsname)->PutValue(val);
					}
					break;
				case SQLITE_BLOB:
					{
						COleVariant var;
						int size = sqlite3_column_bytes(stmt, col);
						acg::CAConvert::toOleVariant(var, (uint8_t *)sqlite3_column_blob(stmt, col), size);
						prst->Fields->GetItem(bsname)->PutValue(var);
					}

					break;
				case SQLITE_TEXT:
				default:
					prst->Fields->GetItem(bsname)->PutValue(_bstr_t(sqlite3_column_text16(stmt, col)));
					break;
				}
			}
			prst->Update();
		}
		//prst->Fields->Append(L"FID", adInteger, 4, adFldUpdatable);
		//prst->Fields->Append(L"FWorkNo", adVarChar, 120, adFldUpdatable);
		//prst->Fields->Append(L"FName", adVarChar, 120, adFldUpdatable);
		//prst->Fields->Append(L"FSexName", adVarChar, 120, adFldUpdatable);
		//prst->Fields->Append(L"FDptName", adVarChar, 120, adFldUpdatable);
		//prst->Fields->Append(L"FOfficeName", adVarChar, 120, adFldUpdatable);


		prst->MoveFirst();
	}
	catch(_com_error &e)
	{
		acg::CAApp::writeLogs(e, L"CMSQL::toRecordset");
	}
	catch(...)
	{
		acg::CAApp::lastError();
	}

	return prst;

}





}