#include "dbtype.h"

namespace db {

parameter::parameter()
	: m_beParamDirection(BEParameterDirectionUnknown)
{

}
parameter::parameter(const parameter &param)
	: m_wsName(param.m_wsName)
	, m_varValue(param.m_varValue), m_beParamDirection(param.m_beParamDirection)
{

}
const parameter& parameter::operator=(const parameter &param)
{
	if (this != &param)
	{
		m_wsName = param.m_wsName;
		m_varValue = param.m_varValue;
		m_beParamDirection = param.m_beParamDirection;
	}
	return *this;
}
std::wstring parameter::getName() const
{
	return m_wsName;
}
void parameter::setName(PCWSTR pname)
{
	m_wsName = pname? pname: L"";
}
const gtc::variant& parameter::getValue() const
{
	return m_varValue;
}
void parameter::setValue(const gtc::variant &val)
{
	m_varValue = val;
}

void parameter::setValue(int val)
{
	m_varValue = val;
}
void parameter::setValue(int64_t val)
{
	m_varValue = val;

}
void parameter::setValue(double val)
{
	m_varValue = val;

}
void parameter::setValue(std::wstring &val)
{
	m_varValue = val;
}
void parameter::setValue(const std::wstring &val)
{
	m_varValue = val;

}
void parameter::setValue(const gtc::datetime &val)
{
	m_varValue = val;
}
void parameter::setValue(SBDataType &val)
{
	m_varValue = val;
}
void parameter::setValue(const SBDataType &val)
{
	m_varValue = val;
}
BEParameterDirection parameter::getDirection() const
{
	return m_beParamDirection;
}
void parameter::setDirection(BEParameterDirection direction)
{
	m_beParamDirection = direction;
}

BEDataType parameter::getType() const
{
	return m_varValue.dataType;
}

parameter::parameter(const parameter *ppar)
{
	assert(ppar);
	m_wsName = ppar->m_wsName;
	m_varValue = ppar->m_varValue;
	m_beParamDirection = ppar->m_beParamDirection;
}
parameter::parameter(std::wstring wsName, const gtc::variant &var, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(var), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(gtc::variant::null), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, bool val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, std::wstring &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val, false), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, const std::wstring &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}

parameter::parameter(std::wstring wsName, PCWSTR val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val?val:L""), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, uint8_t val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, int16_t val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, int32_t val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, int64_t val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, float val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, double val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, const std::tm &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, const gtc::datetime &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName,  SBDataType &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val, false), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, SWDataType &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val, false), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, const SBDataType &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter::parameter(std::wstring wsName, const SWDataType &val, BEParameterDirection dir /*= BEParameterDirectionInput*/)
	: m_wsName(wsName), m_varValue(val), m_beParamDirection(dir)
{

}
parameter* parameter::find(const std::wstring &wsName, const ArrayType &vecParams)
{
	if(vecParams.size() == 0) return nullptr;
	for (ArrayType::const_iterator iter = vecParams.cbegin(); iter != vecParams.cend(); ++iter)
	{
		if((*iter)->getName() == wsName)
			return *iter;
	}
	return nullptr;
}
void parameter::free(ArrayType &vecParams)
{
	for (ArrayType::iterator iter = vecParams.begin(); iter != vecParams.end(); ++iter)
	{
		delete *iter;
	}
	vecParams.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	command  类    2015-9-28
//	数据库命令对象
////////////////////////////////////////////////////////////////////////////////////////////////////////
command::command(database *pdb)
	: m_pdb(pdb)
{
	assert(m_pdb);
}
command::~command()
{
	//SEND_LOG(L"command free ...");
	parameter::free(m_vecParameter);
}

void command::beginTransaction()
{
	if(m_pdb) m_pdb->beginTransaction();
}
void command::commitTransaction()
{
	if(m_pdb) m_pdb->commitTransaction();
}
void command::rollbackTransaction()
{
	if(m_pdb) m_pdb->rollbackTransaction();
}

int command::executeInsertByParameter(PCWSTR ptable)
{
	assert(ptable && m_vecParameter.size() > 0);
	std::wstring wsFields, wsValues, wssql(L"INSERT INTO ");
	for (parameter::ArrayType::iterator iter = m_vecParameter.begin(); iter != m_vecParameter.end(); ++iter)
	{
		wsFields += (*iter)->getName();
		wsValues += L"@" + (*iter)->getName();
		if (iter != m_vecParameter.end()-1)
		{
			wsFields += L",";
			wsValues += L",";
		}
	}
	wssql += ptable;
	wssql += L"(" + wsFields + L") VALUES(" + wsValues + L")";

	return executeNonQuery(wssql);
}
int command::executeUpdateByParameter(PCWSTR ptable, PCWSTR pwhere)
{
	assert(ptable && m_vecParameter.size() > 0);
	std::wstring wsUpdates, wssql(L"UPDATE "), wstr, wsWhere(pwhere);
	for (parameter::ArrayType::iterator iter = m_vecParameter.begin(); iter != m_vecParameter.end(); ++iter)
	{
		wsUpdates += (*iter)->getName() + L"=@" + (*iter)->getName();
		if (iter != m_vecParameter.end()-1)
		{
			wsUpdates += L",";
		}
	}
	wssql += ptable;
	wssql += L" SET " + wsUpdates;
	// 
	gtc::stringUtil::trimLeft(wsWhere);
	//gtc::convert::trimLeft(wsWhere);
	wstr = wsWhere;
	//gtc::convert::toUpper(wstr);
	gtc::stringUtil::toUpper(wstr);
	if(wstr.length() > 6 && wstr.substr(0, 6) == L"WHERE ")
		wssql += L" " + wsWhere;
	else
		wssql += L" WHERE " + wsWhere;

	return executeNonQuery(wssql);;
}
// 绑定参数
parameter* command::addParameter(PCWSTR pparamName, int iValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, iValue);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(PCWSTR pparamName, int64_t llValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, llValue);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(PCWSTR pparamName, double dblValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, dblValue);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(PCWSTR pparamName, std::wstring &wsValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, wsValue);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(PCWSTR pparamName, const std::wstring &wsValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, wsValue);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(PCWSTR pparamName, const gtc::datetime &dtValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, dtValue);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(PCWSTR pparamName, SBDataType &sbData)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, sbData);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(PCWSTR pparamName, const SBDataType &sbData)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName, sbData);
	m_vecParameter.push_back(pParam);

	return pParam;
}
parameter* command::addParameter(parameter *par)
{
	assert(par);
	parameter *pParam = nullptr;
	if(pParam = findParameter(par->getName())) return pParam;

	pParam = new parameter(par);
	m_vecParameter.push_back(pParam);
	return pParam;
}
void command::addParameter(parameter::ArrayType &params)
{
	for (parameter::ArrayType::iterator iter = params.begin(); iter != params.end(); ++iter)
	{
		addParameter(*iter);
	}
}
// 绑定空参数值
parameter* command::addParameter(PCWSTR pparamName)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	if(pParam = findParameter(pparamName)) return pParam;

	pParam = new parameter(pparamName);
	m_vecParameter.push_back(pParam);

	return pParam;

}
parameter* command::findParameter(std::wstring wsName)
{
	return parameter::find(wsName, m_vecParameter);
}

void command::clearParameter()
{
	parameter::free(m_vecParameter);
}


parameter* command::updateParameter(PCWSTR pparamName, int iValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(iValue);
	}
	return pParam;
}
parameter* command::updateParameter(PCWSTR pparamName, int64_t llValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(llValue);
	}
	return pParam;
}
parameter* command::updateParameter(PCWSTR pparamName, double dblValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(dblValue);
	}
	return pParam;
}
parameter* command::updateParameter(PCWSTR pparamName, std::wstring &wsValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(wsValue);
	}
	return pParam;
}
parameter* command::updateParameter(PCWSTR pparamName, const std::wstring &wsValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(wsValue);
	}
	return pParam;
}

parameter* command::updateParameter(PCWSTR pparamName, const gtc::datetime &dtValue)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(dtValue);
	}
	return pParam;
}
parameter* command::updateParameter(PCWSTR pparamName, SBDataType &sbData)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(sbData);
	}
	return pParam;
}
parameter* command::updateParameter(PCWSTR pparamName, const SBDataType &sbData)
{
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(sbData);
	}
	return pParam;
}
parameter* command::updateParameter(PCWSTR pparamName)
{	//将指定参数值设为null
	assert(pparamName);
	parameter *pParam = nullptr;
	pParam = findParameter(pparamName);
	assert(pParam);
	if(pParam)
	{
		pParam->setValue(gtc::variant::null);
	}
	return pParam;
}	

//////////////////////////////////////////////////////////////////////////
// database类 2014-08-24
// 数据库抽象类
//////////////////////////////////////////////////////////////////////////

database::database()
	: m_iTimeout(30000)
{

}
database::~database()
{

}

void database::setTimeout(int iMillisecs)
{
	m_iTimeout = iMillisecs;
}
bool database::open(const std::wstring &dbFile)
{
	return false;
}
std::wstring database::getLastErrorInfo() const
{
	return L"";
}
void database::beginTransaction()
{

}
void database::commitTransaction()
{

}
void database::rollbackTransaction()
{

}

}	// end wuc