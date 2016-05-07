#ifndef __SQLITE_H__
#define __SQLITE_H__

#pragma once
#include "AC/sysupport.h"
#include "db/dbtype.h"
#include "sqlite3.h"
#if defined(_DEBUG)
#	pragma comment(lib,"D:/NTools/db/sqlite/sqlite3s_d.lib")

#else
#	pragma comment(lib,"D:/NTools/db/sqlite/sqlite3s.lib")
#endif

namespace db {
namespace sqlite {

#define  SQLITE_ERRUSER_NORMAL		1000	// 自定义错误码
static std::wstring _errorInfoSqlite(int errCode);

#define SQLITE_EXCEPT(code) GTC_EXCEPT_EX(gtc::baseException::BSExceptionCodeDBAssertionFailed, code, _errorInfoSqlite(code))
//
////////////////////////////////////////////////////////////////////////////
//// exception类 2014-08-24
//// sqlite3异常类
////////////////////////////////////////////////////////////////////////////
//class exception
//{
//public:
//	exception(int errCode, PCWSTR pstrErr);
//	exception(const exception &e);
//	virtual ~exception();
//public:
//	int errorCode() const;
//	const std::wstring& errorMessage() const;
//
//public:
//	static std::wstring errorCodeToString(int errCode);
//private:
//	int				m_iErrCode;
//	std::wstring	m_wsErrInfo;
//};

//////////////////////////////////////////////////////////////////////////
// sqliteDataSet类 2014-08-24
// sqlite3记录集封装类
//////////////////////////////////////////////////////////////////////////
class sqliteDataSet : public dataSet
{
public:
	sqliteDataSet();
	sqliteDataSet(sqlite3 *pdb, sqlite3_stmt *pstmt, bool bEof);
	sqliteDataSet(const sqliteDataSet &rst);
	sqliteDataSet& operator=(const sqliteDataSet &rst);
	virtual ~sqliteDataSet();

	bool eof();
	void nextRow();
	void finalize();

	int getCols() const;
	int fieldIndex(const std::wstring &wsFieldName);
	std::wstring fieldName(int col);

	std::wstring fieldDeclType(int col);
	int fieldType(int col);

	virtual gtc::variant getValue(int col);
	virtual gtc::variant getValue(const std::wstring &wsFieldName);

	virtual int32_t	getInt(int col);
	virtual int32_t getInt(const std::wstring &wsFieldName);
	virtual int64_t	getInt64(int col);
	virtual int64_t getInt64(const std::wstring &wsFieldName);
	virtual double getDouble(int col);
	virtual double getDouble(const std::wstring &wsFieldName);
	virtual std::wstring getString(int col);
	virtual std::wstring getString(const std::wstring &wsFieldName);
	virtual SBDataType getBlob(int col);
	virtual SBDataType getBlob(const std::wstring &wsFieldName);
	virtual gtc::datetime getDatetime(int col);
	virtual gtc::datetime getDatetime(const std::wstring &wsFieldName);
	virtual bool isNull(int col);
	virtual bool isNull(const std::wstring &wsFieldName);

private:
	sqlite3			*m_pdb;
	sqlite3_stmt	*m_pstmt;
	bool			m_bEof;
	int				m_iCols;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	sqliteCommand  类		2015-9-29
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class sqliteCommand : public command
{
private:
	sqliteCommand();
	sqliteCommand(const sqliteCommand &cmd);
	sqliteCommand& operator=(const sqliteCommand &cmd);
public:
	sqliteCommand(database *pdb);
	virtual ~sqliteCommand();
public:
	virtual int exec(const std::wstring &ssql);
	// 执行；参数输入格式为：@<name>
	// 执行sql语句，返回语句影响的数据行数
	virtual int executeNonQuery(const std::wstring &ssql, bool clearParams = true);
	virtual int executeNonQuery(PCWSTR psql, bool clearParams = true);
	// 执行sql语句，放回查询结果的第0行0列值
	virtual gtc::variant executeScalar(const std::wstring &ssql, bool clearParams = true);
	virtual gtc::variant executeScalar(PCWSTR psql, bool clearParams = true);
	// 执行sql语句，放回查询结果
	virtual dataSet* executeReader(const std::wstring &ssql, bool clearParams = true);
	virtual dataSet* executeReader(PCWSTR psql, bool clearParams = true);
	
private:
	bool finalize(sqlite3_stmt *&pstmt);
	// 构建命令参数
	void buildCommandParameter(sqlite3_stmt *pstmt);
	// 得到参数名对应的索引好
	int checkParameterIndex(sqlite3_stmt *pstmt, PCSTR pparamName);
	// 编译sql语句
	sqlite3_stmt* compile(const std::wstring &ssql);

};
//////////////////////////////////////////////////////////////////////////
// sqliteDB类 2014-08-24
// sqlite3封装类
//////////////////////////////////////////////////////////////////////////
class sqliteDB : public database
{
private:
	sqliteDB(const sqliteDB &db);
	sqliteDB& operator=(const sqliteDB &db);
public:
	sqliteDB();
	virtual ~sqliteDB();

	virtual bool open(const std::wstring &dbFile, bool bFast = true);
	virtual bool close();
	virtual std::wstring getLastErrorInfo() const;

	// 事务支持
	virtual void beginTransaction();
	virtual void commitTransaction();
	virtual void rollbackTransaction();



public:
	virtual command* createCommand();
	virtual void* getCmdSet();
	virtual void setTimeout(int iMillisecs);

private:
	sqlite3			*m_pdb;			// 


};

}	// sqlite
}	// db

#endif	// __SQLITE_H__