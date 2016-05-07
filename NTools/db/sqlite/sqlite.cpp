#include "sqlite.h"

//#pragma comment(lib, "D:/NTools/db/sqlite/sqlite3.lib")

namespace db {
namespace sqlite {
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-31 sqlite错误码的描述
static std::wstring _errorInfoSqlite(int errCode)
{
	switch (errCode)
	{
	case SQLITE_OK          : return L"SQLITE_OK";
	case SQLITE_ERROR       : return L"SQLITE_ERROR<SQL error or missing database>";
	case SQLITE_INTERNAL    : return L"SQLITE_INTERNAL<Internal logic error in SQLite>";
	case SQLITE_PERM        : return L"SQLITE_PERM<Access permission denied>";
	case SQLITE_ABORT       : return L"SQLITE_ABORT<Callback routine requested an abort>";
	case SQLITE_BUSY        : return L"SQLITE_BUSY<The database file is locked>";
	case SQLITE_LOCKED      : return L"SQLITE_LOCKED<A table in the database is locked>";
	case SQLITE_NOMEM       : return L"SQLITE_NOMEM<A malloc() failed>";
	case SQLITE_READONLY    : return L"SQLITE_READONLY<Attempt to write a readonly database>";
	case SQLITE_INTERRUPT   : return L"SQLITE_INTERRUPT<Operation terminated by sqlite3_interrupt()>";
	case SQLITE_IOERR       : return L"SQLITE_IOERR<Some kind of disk I/O error occurred>";
	case SQLITE_CORRUPT     : return L"SQLITE_CORRUPT<The database disk image is malformed>";
	case SQLITE_NOTFOUND    : return L"SQLITE_NOTFOUND<Unknown opcode in sqlite3_file_control()>";
	case SQLITE_FULL        : return L"SQLITE_FULL<Insertion failed because database is full>";
	case SQLITE_CANTOPEN    : return L"SQLITE_CANTOPEN<Unable to open the database file>";
	case SQLITE_PROTOCOL    : return L"SQLITE_PROTOCOL<Database lock protocol error>";
	case SQLITE_EMPTY       : return L"SQLITE_EMPTY<Database is empty>";
	case SQLITE_SCHEMA      : return L"SQLITE_SCHEMA<The database schema changed>";
	case SQLITE_TOOBIG      : return L"SQLITE_TOOBIG<String or BLOB exceeds size limit>";
	case SQLITE_CONSTRAINT  : return L"SQLITE_CONSTRAINT<Abort due to constraint violation>";
	case SQLITE_MISMATCH    : return L"SQLITE_MISMATCH<Data type mismatch>";
	case SQLITE_MISUSE      : return L"SQLITE_MISUSE<Library used incorrectly>";
	case SQLITE_NOLFS       : return L"SQLITE_NOLFS<Uses OS features not supported on host>";
	case SQLITE_AUTH        : return L"SQLITE_AUTH<Authorization denied>";
	case SQLITE_FORMAT      : return L"SQLITE_FORMAT<Auxiliary database format error>";
	case SQLITE_RANGE       : return L"SQLITE_RANGE<2nd parameter to sqlite3_bind out of range>";
	case SQLITE_NOTADB      : return L"SQLITE_NOTADB<File opened that is not a database file>";
	case SQLITE_NOTICE      : return L"SQLITE_NOTICE<Notifications from sqlite3_log()>";
	case SQLITE_WARNING     : return L"SQLITE_WARNING<Warnings from sqlite3_log()>";
	case SQLITE_ROW         : return L"SQLITE_ROW<sqlite3_step() has another row ready>";
	case SQLITE_DONE        : return L"SQLITE_DONE<sqlite3_step() has finished executing>";
	//case CPPSQLITE_ERROR    : return "CPPSQLITE_ERROR";
	default: return L"SQLITE_UNKNOWN_ERROR";
	}
	return L"SQLITE_UNKNOWN_ERROR";
}
////////////////////////////////////////////////////////////////////////////
//// exception类 2014-08-24
//// sqlite3异常类
////////////////////////////////////////////////////////////////////////////
//
//exception::exception(int errCode, PCWSTR pstrErr)
//	: m_iErrCode(errCode)
//{
//	m_wsErrInfo = errorCodeToString(errCode);
//	wchar_t pbuf[40] = {0};
//	std::swprintf(pbuf, _countof(pbuf), L"[%d]", m_iErrCode);
//	m_wsErrInfo += pbuf;
//	m_wsErrInfo += pstrErr;
//}
//
//exception::exception(const exception &e)
//	: m_wsErrInfo(e.m_wsErrInfo)
//	, m_iErrCode(e.m_iErrCode)
//{
//
//}
//exception::~exception()
//{
//
//}
//int exception::errorCode() const
//{
//	return m_iErrCode;
//}
//const std::wstring& exception::errorMessage() const
//{
//	return m_wsErrInfo;
//}
//std::wstring exception::errorCodeToString(int errCode)
//{
//	switch (errCode)
//	{
//	case SQLITE_OK          : return L"SQLITE_OK";
//	case SQLITE_ERROR       : return L"SQLITE_ERROR";
//	case SQLITE_INTERNAL    : return L"SQLITE_INTERNAL";
//	case SQLITE_PERM        : return L"SQLITE_PERM";
//	case SQLITE_ABORT       : return L"SQLITE_ABORT";
//	case SQLITE_BUSY        : return L"SQLITE_BUSY";
//	case SQLITE_LOCKED      : return L"SQLITE_LOCKED";
//	case SQLITE_NOMEM       : return L"SQLITE_NOMEM";
//	case SQLITE_READONLY    : return L"SQLITE_READONLY";
//	case SQLITE_INTERRUPT   : return L"SQLITE_INTERRUPT";
//	case SQLITE_IOERR       : return L"SQLITE_IOERR";
//	case SQLITE_CORRUPT     : return L"SQLITE_CORRUPT";
//	case SQLITE_NOTFOUND    : return L"SQLITE_NOTFOUND";
//	case SQLITE_FULL        : return L"SQLITE_FULL";
//	case SQLITE_CANTOPEN    : return L"SQLITE_CANTOPEN";
//	case SQLITE_PROTOCOL    : return L"SQLITE_PROTOCOL";
//	case SQLITE_EMPTY       : return L"SQLITE_EMPTY";
//	case SQLITE_SCHEMA      : return L"SQLITE_SCHEMA";
//	case SQLITE_TOOBIG      : return L"SQLITE_TOOBIG";
//	case SQLITE_CONSTRAINT  : return L"SQLITE_CONSTRAINT";
//	case SQLITE_MISMATCH    : return L"SQLITE_MISMATCH";
//	case SQLITE_MISUSE      : return L"SQLITE_MISUSE";
//	case SQLITE_NOLFS       : return L"SQLITE_NOLFS";
//	case SQLITE_AUTH        : return L"SQLITE_AUTH";
//	case SQLITE_FORMAT      : return L"SQLITE_FORMAT";
//	case SQLITE_RANGE       : return L"SQLITE_RANGE";
//	case SQLITE_ROW         : return L"SQLITE_ROW";
//	case SQLITE_DONE        : return L"SQLITE_DONE";
//	//case CPPSQLITE_ERROR    : return "CPPSQLITE_ERROR";
//	default: return L"UNKNOWN_ERROR";
//	}
//	return L"UNKNOWN_ERROR";
//}

//////////////////////////////////////////////////////////////////////////
// sqliteDataSet类 2014-08-24
// sqlite3记录集封装类
//////////////////////////////////////////////////////////////////////////

sqliteDataSet::sqliteDataSet()
	: m_pdb(nullptr), m_pstmt(nullptr)
	, m_bEof(true)
	, m_iCols(0)
{

}
sqliteDataSet::sqliteDataSet(sqlite3 *pdb, sqlite3_stmt *pstmt, bool bEof)
	: m_pdb(pdb), m_pstmt(pstmt)
	, m_bEof(bEof)
{
	m_iCols = sqlite3_column_count(m_pstmt);
}
sqliteDataSet::sqliteDataSet(const sqliteDataSet &rst)
	: m_pdb(rst.m_pdb), m_pstmt(rst.m_pstmt)
	, m_bEof(rst.m_bEof)
	, m_iCols(rst.m_iCols)
{
	const_cast<sqliteDataSet &>(rst).m_pstmt = nullptr;
}
sqliteDataSet& sqliteDataSet::operator=(const sqliteDataSet &rst)
{
	if (this != &rst)
	{
		try
		{
			finalize();
		}
		catch (...)
		{
		}
		m_pdb = rst.m_pdb;
		m_pstmt = rst.m_pstmt;
		m_bEof = rst.m_bEof;
		m_iCols = rst.m_iCols;
		const_cast<sqliteDataSet &>(rst).m_pstmt = nullptr;
	}
	return *this;
}
sqliteDataSet::~sqliteDataSet()
{
	try
	{
		finalize();
	}
	catch (...)
	{
	}
}

int sqliteDataSet::getCols() const
{
	assert(m_pstmt);
	return m_iCols;
}

int sqliteDataSet::fieldIndex(const std::wstring &wsFieldName)
{
	assert(m_pstmt);
	if(wsFieldName.empty()) return -1;
	for (int idx = 0; idx < m_iCols; ++idx)
	{
		PCWSTR pfield = (PCWSTR)sqlite3_column_name16(m_pstmt, idx);
		if(wsFieldName == pfield) return idx;
	}
	return -1;
}

std::wstring sqliteDataSet::fieldName(int col)
{
	assert(m_pstmt);
	if(!m_pstmt || col < 0 || col >= m_iCols) return L"";
	return (PCWSTR)sqlite3_column_name16(m_pstmt, col);
}

std::wstring sqliteDataSet::fieldDeclType(int col)
{
	assert(m_pstmt);
	if(!m_pstmt || col < 0 || col >= m_iCols) return L"";
	return (PCWSTR)sqlite3_column_decltype16(m_pstmt, col);
}

int sqliteDataSet::fieldType(int col)
{
	assert(m_pstmt);
	if(!m_pstmt || col < 0 || col >= m_iCols) return -1;
	return sqlite3_column_type(m_pstmt, col);

}

gtc::variant sqliteDataSet::getValue(int col)
{
	assert(m_pstmt);
	int fldtype = fieldType(col);
	if(fldtype == SQLITE_NULL) return gtc::variant::null;
	else if(fldtype == SQLITE_INTEGER) return getInt(col);
	else if(fldtype == SQLITE_FLOAT) return getDouble(col);
	else if(fldtype == SQLITE_TEXT) return getString(col);
	else if(fldtype == SQLITE_BLOB) return getBlob(col);

	return gtc::variant();
}
gtc::variant sqliteDataSet::getValue(const std::wstring &wsFieldName)
{
	return getValue(fieldIndex(wsFieldName));
}
int32_t	sqliteDataSet::getInt(int col)
{
	assert(m_pstmt);
	if(fieldType(col) == SQLITE_NULL) return 0;
	return sqlite3_column_int(m_pstmt, col);
}
int32_t sqliteDataSet::getInt(const std::wstring &wsFieldName)
{
	return getInt(fieldIndex(wsFieldName));
}
int64_t	sqliteDataSet::getInt64(int col)
{
	assert(m_pstmt);
	if(fieldType(col) == SQLITE_NULL) return 0;
	return sqlite3_column_int64(m_pstmt, col);

}
int64_t sqliteDataSet::getInt64(const std::wstring &wsFieldName)	
{
	return getInt64(fieldIndex(wsFieldName));
}
double sqliteDataSet::getDouble(int col)
{
	assert(m_pstmt);
	if(fieldType(col) == SQLITE_NULL) return 0.0;
	return sqlite3_column_double(m_pstmt, col);

}
double sqliteDataSet::getDouble(const std::wstring &wsFieldName)
{
	return getDouble(fieldIndex(wsFieldName));
}
std::wstring sqliteDataSet::getString(int col)
{
	assert(m_pstmt);
	if(fieldType(col) == SQLITE_NULL) return L"";
	return (PCWSTR)sqlite3_column_text16(m_pstmt, col);

}
std::wstring sqliteDataSet::getString(const std::wstring &wsFieldName)
{
	return getString(fieldIndex(wsFieldName));
}
SBDataType sqliteDataSet::getBlob(int col)
{
	assert(m_pstmt);
	SBDataType sbdata;
	if(fieldType(col) == SQLITE_NULL) return sbdata;
	int len = sqlite3_column_bytes(m_pstmt, col);
	const uint8_t *pdata = (const uint8_t *)sqlite3_column_blob(m_pstmt, col);
	if(len > 0 && pdata)
		sbdata.assign(pdata, pdata + len);
	return sbdata;

}
SBDataType sqliteDataSet::getBlob(const std::wstring &wsFieldName)
{
	return getBlob(fieldIndex(wsFieldName));
}
gtc::datetime sqliteDataSet::getDatetime(int col)
{
	assert(m_pstmt);
	gtc::datetime dt;
	if(fieldType(col) == SQLITE_TEXT)
	{
		std::wstring wsdt = (PCWSTR)sqlite3_column_text16(m_pstmt, col);
		if(!dt.parse(wsdt)) GTC_EXCEPT(gtc::baseException::BSExceptionCodeInvalidParams, L"无效的时间格式");
	}
	else if (fieldType(col) == SQLITE_INTEGER)
	{
		dt = gtc::timestamp::fromEpochTime(sqlite3_column_int64(m_pstmt, col));
	}
	else
	{
		assert(!L"处理其他类型转换到时间");
	}
	return dt;
}
gtc::datetime sqliteDataSet::getDatetime(const std::wstring &wsFieldName)
{
	return getDatetime(fieldIndex(wsFieldName));
}
bool sqliteDataSet::isNull(int col)
{
	return fieldType(col) == SQLITE_NULL;
}
bool sqliteDataSet::isNull(const std::wstring &wsFieldName)
{
	return isNull(fieldIndex(wsFieldName));
}
bool sqliteDataSet::eof()
{
	assert(m_pstmt);
	return m_bEof;
}
void sqliteDataSet::nextRow()
{
	assert(m_pstmt && m_pdb);
	int rsp = sqlite3_step(m_pstmt);
	if (rsp == SQLITE_DONE)
	{
		m_bEof = true;
	}
	else if (rsp == SQLITE_ROW)
	{
	}
	else
	{
		finalize();
	}
}
void sqliteDataSet::finalize()
{
	if (m_pdb && m_pstmt)
	{
		int rsp = sqlite3_finalize(m_pstmt);
		m_pstmt = NULL;
		//if(rsp != SQLITE_OK) throw exception(rsp, (PCWSTR)sqlite3_errmsg16(m_pdb));
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	sqliteCommand  类		2015-9-29
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
sqliteCommand::sqliteCommand(database *pdb)
	: command(pdb)
{

}
sqliteCommand::~sqliteCommand()
{

}
bool sqliteCommand::finalize(sqlite3_stmt *&pstmt)
{
	assert(m_pdb);
	if (pstmt)
	{
		int rsp = sqlite3_finalize(pstmt);
		pstmt = NULL;

		return rsp == SQLITE_OK;
	}
	return true;
}



sqlite3_stmt* sqliteCommand::compile(const std::wstring &ssql)
{
	const wchar_t* szTail= NULL;
	sqlite3_stmt *pstmt = NULL;
	sqlite3 *ps3 = static_cast<sqlite3 *>(m_pdb->getCmdSet());
	assert(ps3);
	int nRet = sqlite3_prepare16_v2(ps3, ssql.c_str(), -1, &pstmt, (const void **)&szTail);
	if(nRet != SQLITE_OK && pstmt)
		sqlite3_finalize(pstmt);

	return pstmt;
}
int sqliteCommand::checkParameterIndex(sqlite3_stmt *pstmt, PCSTR pparamName)
{
	assert(pstmt);
	if(!pstmt) return -1;
	int nParam = sqlite3_bind_parameter_index(pstmt, pparamName);
	//assert(nParam > 0);
	//const char* szError= sqlite3_errmsg(m_pdb);
	return nParam;
}

// 构建命令参数
void sqliteCommand::buildCommandParameter(sqlite3_stmt *pstmt)
{
	assert(pstmt);
	std::string sname;
	int idx = 0, rsp = SQLITE_OK;
	for (parameter::ArrayType::iterator iter = m_vecParameter.begin(); iter != m_vecParameter.end(); ++iter)
	{
		sname = gtc::convert::toChar((*iter)->getName().c_str());
		if(sname.substr(0, 1) != "@") sname = "@" + sname;
		idx = checkParameterIndex(pstmt, sname.c_str());
		if (idx <= 0) continue;
		switch((*iter)->getType())
		{
		case BEDataTypeNone:		// VT_EMPTY			无						adEmpty					SQLITE_NULL
			rsp = sqlite3_bind_null(pstmt, idx);
			break;
		case BEDataTypeByte:		// VT_UI1			uint8_t					adUnsignedTinyInt		SQLITE_INTEGER
			rsp = sqlite3_bind_int(pstmt, idx, (*iter)->getValue().nVal);
			break;
		case BEDataTypeBoolean:		// VT_BOOL			bool					adBoolean				SQLITE_INTEGER
			rsp = sqlite3_bind_int(pstmt, idx, (*iter)->getValue().bVal);
			break;
		case BEDataTypeInt16:		// VT_I2			int16_t					adSmallInt				SQLITE_INTEGER
			rsp = sqlite3_bind_int(pstmt, idx, (*iter)->getValue().iVal);
			break;
		case BEDataTypeInt32:		// VT_I4			int32_t					adInteger				SQLITE_INTEGER
			rsp = sqlite3_bind_int(pstmt, idx, (*iter)->getValue().lVal);
			break;
		case BEDataTypeInt64:		// VT_I8			int64_t					adBigInt				SQLITE_INTEGER
			rsp = sqlite3_bind_int64(pstmt, idx, (*iter)->getValue().llVal);
			break;
		case BEDataTypeString:		// VT_BSTR			wstring					adVarWChar				SQLITE_TEXT
			{
				if((*iter)->getValue().pwsVal)
					rsp = sqlite3_bind_text16(pstmt, idx, (*iter)->getValue().pwsVal->c_str(), -1, NULL);
				else
					rsp = sqlite3_bind_null(pstmt, idx);
			}
			break;
		case BEDataTypeFloat:		// VT_R4			float					adSingle				SQLITE_FLOAT
			rsp = sqlite3_bind_double(pstmt, idx, (*iter)->getValue().fltVal);
			break;
		case BEDataTypeDouble:		// VT_R8			double					adDouble				SQLITE_FLOAT
			rsp = sqlite3_bind_double(pstmt, idx, (*iter)->getValue().dblVal);
			break;
		case BEDataTypeDate:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			{
				if((*iter)->getValue().dataType == BEDataTypeTime 
					|| (*iter)->getValue().dataType == BEDataTypeDate
					|| (*iter)->getValue().dataType == BEDataTypeDatetime)
				{
					gtc::datetime dt = gtc::timestamp::fromEpochTime((*iter)->getValue().tmVal);
					rsp = sqlite3_bind_text16(pstmt, idx, dt.format(L"%Y-%m-%d").c_str(), -1, NULL);

				}
				else
					rsp = sqlite3_bind_null(pstmt, idx);
			}
			break;
		case BEDataTypeTime:		// VT_DATE			tm						adDate					SQLITE_FLOAT
			{
				if((*iter)->getValue().dataType == BEDataTypeTime 
					|| (*iter)->getValue().dataType == BEDataTypeDate
					|| (*iter)->getValue().dataType == BEDataTypeDatetime)
				{
					gtc::datetime dt = gtc::timestamp::fromEpochTime((*iter)->getValue().tmVal);
					dt.assign(TIME_TYPE_YEAR, TIME_TYPE_MONTH, TIME_TYPE_DAY, dt.hour(), dt.minute(), dt.second());
					rsp = sqlite3_bind_text16(pstmt, idx, dt.format().c_str(), -1, NULL);

				}
				else
					rsp = sqlite3_bind_null(pstmt, idx);
			}
			break;
		case BEDataTypeDatetime:	// VT_DATE			tm						adDate					SQLITE_FLOAT
			{
				if((*iter)->getValue().dataType == BEDataTypeTime 
					|| (*iter)->getValue().dataType == BEDataTypeDate
					|| (*iter)->getValue().dataType == BEDataTypeDatetime)
				{
					gtc::datetime dt = gtc::timestamp::fromEpochTime((*iter)->getValue().tmVal);
					rsp = sqlite3_bind_text16(pstmt, idx, dt.format().c_str(), -1, NULL);

				}
				else
					rsp = sqlite3_bind_null(pstmt, idx);
			}
			break;
		case BEDataTypeImage:		// VT_ARRAY|VT_UI1	std::vector<uint8_t>	adLongVarBinary			SQLITE_BLOB
			{
				if((*iter)->getValue().pbinaryVal && (*iter)->getValue().size() > 0)
					rsp = sqlite3_bind_blob(pstmt, idx, (*iter)->getValue().pbinaryVal->data(), (*iter)->getValue().size(), NULL);
				else
					rsp = sqlite3_bind_null(pstmt, idx);
			}
			break;
		case BEDataTypeText:		// VT_BSTR			std::vector<wchar_t>	adLongVarWChar			SQLITE_TEXT
			{
				if((*iter)->getValue().ptextVal && (*iter)->getValue().size() > 0)
					rsp = sqlite3_bind_blob(pstmt, idx, (*iter)->getValue().ptextVal->data(), (*iter)->getValue().size()*sizeof(wchar_t), NULL);
				else
					rsp = sqlite3_bind_null(pstmt, idx);
			}
			break;
		default:
			assert(false);
			break;
		}

		assert(rsp == SQLITE_OK);
		if(rsp != SQLITE_OK) SQLITE_EXCEPT(rsp);
	}
}
int sqliteCommand::exec(const std::wstring &ssql)
{
	assert(m_pdb);
	int rsp = 0;
	m_wsCmdText = ssql;
	std::string str = gtc::stringUtil::toChar(ssql.c_str());
	beginTransaction();
	sqlite3 *ps3 = static_cast<sqlite3 *>(m_pdb->getCmdSet());
	assert(ps3);
	rsp = sqlite3_exec(ps3, str.c_str(), NULL, NULL, NULL);
	commitTransaction();

	return rsp;
}
// 执行sql语句，返回语句影响的数据行数
int sqliteCommand::executeNonQuery(const std::wstring &ssql, bool clearParams /*= true*/)
{
	assert(m_pdb);
	int rsp = 0;
	sqlite3_stmt *pstmt = compile(ssql);

	if (!pstmt) return 0;
	m_wsCmdText = ssql;
	sqlite3 *ps3 = static_cast<sqlite3 *>(m_pdb->getCmdSet());
	assert(ps3);

	buildCommandParameter(pstmt);
	rsp = sqlite3_step(pstmt);
	if(rsp == SQLITE_DONE) rsp = sqlite3_changes(ps3);
	else
	{
		finalize(pstmt);
		SQLITE_EXCEPT(rsp);
	}
	finalize(pstmt);
	if(clearParams && ssql.find(L'@') != std::wstring::npos) clearParameter();
	return rsp;
}
int sqliteCommand::executeNonQuery(PCWSTR psql, bool clearParams /*= true*/)
{
	assert(psql);
	if(!psql || std::wcslen(psql) <= 0) return 0;
	std::wstring wsql = psql;
	return executeNonQuery(wsql, clearParams);
}
// 执行sql语句，放回查询结果的第0行0列值
gtc::variant sqliteCommand::executeScalar(const std::wstring &ssql, bool clearParams /*= true*/)
{
	assert(m_pdb);
	gtc::variant var = gtc::variant::null;
	m_wsCmdText = ssql;
	sqliteDataSet *prst = dynamic_cast<sqliteDataSet *>(executeReader(ssql));
	if (prst && !prst->eof())
	{
		switch(prst->fieldType(0))
		{
		case SQLITE_INTEGER:
			var = prst->getInt(0);
			break;
		case SQLITE_FLOAT:
			var = prst->getDouble(0);
			break;
		case SQLITE_TEXT:
			var = prst->getString(0);
			break;
		case SQLITE_BLOB:
			var = prst->getBlob(0);
			break;
		}
	}
	if(clearParams && ssql.find(L'@') != std::wstring::npos) clearParameter();
	if(prst) delete prst;
	return var;
}
gtc::variant sqliteCommand::executeScalar(PCWSTR psql, bool clearParams /*= true*/)
{
	assert(psql);
	if(!psql || std::wcslen(psql) <= 0) return gtc::variant::null;
	std::wstring wsql = psql;
	return executeScalar(wsql, clearParams);
}
// 执行sql语句，放回查询结果
dataSet* sqliteCommand::executeReader(const std::wstring &ssql, bool clearParams /*= true*/)
{
	assert(m_pdb);
	int rsp = 0;
	m_wsCmdText = ssql;
	sqlite3_stmt *pstmt = compile(ssql);
	if (!pstmt) return nullptr;

	sqlite3 *ps3 = static_cast<sqlite3 *>(m_pdb->getCmdSet());
	assert(ps3);
	buildCommandParameter(pstmt);
	rsp = sqlite3_step(pstmt);
	//if(clearParams) clearParameter();
	if(rsp == SQLITE_DONE) return new sqliteDataSet(ps3, pstmt, true);
	else if(rsp == SQLITE_ROW) return new sqliteDataSet(ps3, pstmt, false);
	else SQLITE_EXCEPT(rsp);

	if(pstmt) finalize(pstmt);
	if(clearParams && ssql.find(L'@') != std::wstring::npos) clearParameter();
	return nullptr;
}
dataSet* sqliteCommand::executeReader(PCWSTR psql, bool clearParams /*= true*/)
{
	assert(psql);
	if(!psql || std::wcslen(psql) <= 0) return nullptr;
	std::wstring wsql = psql;
	return executeReader(wsql, clearParams);
}
//////////////////////////////////////////////////////////////////////////
// sqliteDB类 2014-08-24
// sqlite3封装类
//////////////////////////////////////////////////////////////////////////

sqliteDB::sqliteDB()
	: m_pdb(NULL)
{

}
sqliteDB::~sqliteDB()
{
	try
	{
		close();
	}
	catch (...)
	{
	}
}
sqliteDB::sqliteDB(const sqliteDB &db)
	: m_pdb(db.m_pdb)
{
}
sqliteDB& sqliteDB::operator=(const sqliteDB &db)
{
	if (this != &db)
	{
		m_pdb = db.m_pdb;
	}
	return *this;
}
bool sqliteDB::open(const std::wstring &dbFile, bool bFast /*= true*/)
{
	sqlite3_initialize();
	int ret = sqlite3_open16(dbFile.c_str(), &m_pdb);
	//int ret = sqlite3_open_v2(gtc::stringUtil::toChar(dbFile.c_str()).c_str(), &m_pdb, SQLITE_OPEN_READWRITE ,NULL);
	if(ret != SQLITE_OK)
	{
		if(m_pdb) sqlite3_close(m_pdb);
		m_pdb = nullptr;
		return false;
	}
	// 
	m_wsCnnString = dbFile;

	// 参考http://www.linuxidc.com/Linux/2012-04/58684.htm
	// FULL synchronous很安全但很慢
	if(bFast) sqlite3_exec(m_pdb, "PRAGMA synchronous=OFF", NULL, NULL, NULL);

	return true;
}
bool sqliteDB::close()
{
	if (m_pdb)
	{
		if (sqlite3_close(m_pdb) == SQLITE_OK)
		{
			m_pdb = nullptr;
			return true;
		}
	}
	sqlite3_shutdown();
	return false;
}
std::wstring sqliteDB::getLastErrorInfo() const
{
	wchar_t pbuf[256] = {0};
	if(m_pdb)
		std::swprintf(pbuf, _countof(pbuf), L"%s(#%d)", (PCWSTR)sqlite3_errmsg16(m_pdb), sqlite3_errcode(m_pdb));
	return pbuf;
}
void sqliteDB::beginTransaction()
{
	sqlite3_exec(m_pdb, "begin transaction", NULL, NULL, NULL);
}
void sqliteDB::commitTransaction()
{
	sqlite3_exec(m_pdb, "commit transaction", NULL, NULL, NULL);
}
void sqliteDB::rollbackTransaction()
{
	sqlite3_exec(m_pdb, "rollback transaction", NULL, NULL, NULL);
}
void sqliteDB::setTimeout(int iMillisecs)
{
	__super::setTimeout(iMillisecs);
	sqlite3_busy_timeout(m_pdb, m_iTimeout);
}
command* sqliteDB::createCommand()
{
	return new sqliteCommand(this);
}
void* sqliteDB::getCmdSet()
{
	return m_pdb;
}
}	// end ::sqlite
}	// end ::db