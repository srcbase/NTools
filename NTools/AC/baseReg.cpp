#include "baseReg.h"
#include <Shlwapi.h>
namespace acm {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	regKey类	2014-08-13
//	注册表管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////

regKey::regKey(void)
	: m_hKey(NULL)
	, m_lLastError(ERROR_SUCCESS)
{
}


regKey::~regKey(void)
{
	close();
}

regKey::operator HKEY() const
{
	return m_hKey;
}
bool regKey::succeeded() const
{
	return m_hKey && m_lLastError == ERROR_SUCCESS;
}
LONG regKey::getLastError() const
{
	return m_lLastError;
}
bool regKey::create(HKEY hKeyParent, PCWSTR pKeyName, REGSAM samDesired /*= KEY_READ | KEY_WRITE*/)
{
	assert(hKeyParent);
	bool bl = false;
	DWORD dw;
	HKEY hKey = NULL;
	m_lLastError = RegCreateKeyExW(hKeyParent, pKeyName, 0, REG_NONE, REG_OPTION_NON_VOLATILE, samDesired, NULL, &hKey, &dw);
	if (m_lLastError == ERROR_SUCCESS)
	{
		bl = close();
		m_hKey = hKey;
	}

	return bl;
}

bool regKey::open(HKEY hKeyParent, PCWSTR pKeyName, REGSAM samDesired /*= KEY_READ | KEY_WRITE*/)
{
	assert(hKeyParent);
	bool bl = false;
	HKEY hKey = NULL;

	m_lLastError = RegOpenKeyExW(hKeyParent, pKeyName, 0, samDesired, &hKey);
	if (m_lLastError == ERROR_SUCCESS)
	{
		bl = close();
		m_hKey = hKey;
	}
	return bl;
}

bool regKey::readValue(PCWSTR pValueName, SBDataType &sbValue, DWORD *pdwType)
{
	ULONG nBytes = 0;
	m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, pdwType, NULL, &nBytes);
	if (m_lLastError == ERROR_MORE_DATA)
	{
		sbValue.resize(nBytes);
	}
	m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, pdwType, static_cast< LPBYTE >(sbValue.data()), &nBytes);
	return m_lLastError == ERROR_SUCCESS;
}

bool regKey::close()
{
	m_lLastError = ERROR_SUCCESS;
	if (m_hKey)
	{
		m_lLastError = RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
	return m_lLastError == ERROR_SUCCESS;
}
bool regKey::existsKey(PCWSTR pkeyName, DWORD *pType /*= NULL*/)
{
	m_lLastError = RegQueryValueEx(m_hKey, pkeyName, NULL, pType, NULL, NULL);
	return m_lLastError != ERROR_FILE_NOT_FOUND;
}
bool regKey::deleteKey(PCWSTR pkeyName)
{
	m_lLastError = RegDeleteValue(m_hKey, pkeyName);
	return m_lLastError == ERROR_SUCCESS;
}
bool regKey::createKey(PCWSTR pkeyName, PCWSTR pValue)
{
	return writeString(pkeyName, pValue);;
	//DWORD dwType = 0;
	//if (existsKey(pkeyName, &dwType))
	//{
	//	writeString(pkeyName, pValue);
	//}
	//else
	//{
	//	RegSetValueEx(m_hKey, pkeyName, NULL, REG_SZ, pValue, 0);
	//}
}
bool regKey::readUI4(PCWSTR pValueName, DWORD &dwValue)
{
	assert(m_hKey);
	ULONG nBytes = sizeof(DWORD);
	DWORD dwType;
	m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &nBytes);
	if(m_lLastError != ERROR_SUCCESS) return false;
	if (dwType != REG_DWORD)
	{
		m_lLastError = ERROR_INVALID_DATA;
		return false;
	}
	return true;
}

bool regKey::readUI8(PCWSTR pValueName, ULONGLONG &ullValue)
{
	assert(m_hKey);
	ULONG nBytes = sizeof(ULONGLONG);
	DWORD dwType;
	m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(&ullValue), &nBytes);
	if(m_lLastError != ERROR_SUCCESS) return false;
	if (dwType != REG_QWORD)
	{
		m_lLastError = ERROR_INVALID_DATA;
		return false;
	}
	return true;
}

bool regKey::readString(PCWSTR pValueName, std::wstring &wsValue)
{
	assert(m_hKey);
	DWORD dwType = 0;
	ULONG uiChars = MAX_PATH;
	ULONG nBytes = uiChars * sizeof(wchar_t);
	wchar_t *pValue = new wchar_t[uiChars];
	wsValue.clear();
	m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue), &nBytes);
	if(m_lLastError != ERROR_MORE_DATA)
	{
		delete [] pValue;
		uiChars = nBytes/sizeof(wchar_t);
		pValue = new wchar_t[uiChars];
		m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue), &nBytes);
	}
	if(dwType != REG_SZ && dwType != REG_EXPAND_SZ)
	{
		//delete [] pValue;
		m_lLastError = ERROR_INVALID_DATA;
		//return false;
	}
	if (pValue != NULL && nBytes !=0 && ((nBytes % sizeof(wchar_t) != 0) || (pValue[nBytes / sizeof(wchar_t) -1] != 0)))
	{
		//delete [] pValue;
		m_lLastError = ERROR_INVALID_DATA;
		//return false;
	}
	else if(nBytes == 0)
		pValue[0] = L'\0';
	if(m_lLastError == ERROR_SUCCESS)
		wsValue = pValue;
	delete [] pValue;
	return m_lLastError == ERROR_SUCCESS;
}
bool regKey::readStrings(PCWSTR pValueName, std::vector<std::wstring> &vecValue)
{
	assert(m_hKey);
	UINT uiChars = MAX_PATH;
	DWORD dwType = 0;
	ULONG nBytes = uiChars * sizeof(wchar_t);
	wchar_t *pValue = new wchar_t[uiChars];
	vecValue.clear();
	m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue), &nBytes);
	if(m_lLastError != ERROR_MORE_DATA)
	{
		delete[] pValue;
		uiChars = nBytes/sizeof(wchar_t);
		pValue = new wchar_t[uiChars];
		m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue), &nBytes);
	}
	if (dwType != REG_MULTI_SZ || (dwType == REG_MULTI_SZ && pValue != NULL
		&& (nBytes % sizeof(wchar_t) != 0 || nBytes / sizeof(wchar_t) < 1 
			|| pValue[nBytes / sizeof(wchar_t) -1] != 0 
			|| ((nBytes/sizeof(wchar_t))>1 && pValue[nBytes / sizeof(wchar_t) - 2] != 0))))
	{
		m_lLastError = ERROR_INVALID_DATA;
	}
	if (m_lLastError == ERROR_SUCCESS)
	{
		wchar_t *pOffset = pValue;
		std::size_t len = 0;
		while (pOffset < pValue+uiChars)
		{
			len = std::wcslen(pOffset);
			if(len <= 0) break;
			vecValue.push_back(pOffset);
			pOffset += len + 1;
		}
	}
	// 
	delete[] pValue;
	return m_lLastError == ERROR_SUCCESS;
}
bool regKey::readBinary(PCWSTR pValueName, SBDataType &sbValue)
{
	assert(m_hKey);
	DWORD dwType = 0;
	ULONG uiBytes = 0;
	uint8_t *pValue = NULL;
	m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, NULL, &uiBytes);
	if(m_lLastError != ERROR_MORE_DATA) 
	{
		pValue = new uint8_t[uiBytes];
		m_lLastError = ::RegQueryValueExW(m_hKey, pValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue), &uiBytes);
	}
	if (dwType != REG_BINARY)
	{
		//delete [] pValue;
		m_lLastError = ERROR_INVALID_DATA;
		//return false;
	}
	if(m_lLastError == ERROR_SUCCESS)
		sbValue.assign(pValue, pValue + uiBytes);
	delete [] pValue;
	return m_lLastError == ERROR_SUCCESS;
}

bool regKey::writeUI4(PCWSTR pValueName, DWORD dwValue)
{
	assert(m_hKey);
	m_lLastError = ::RegSetValueExW(m_hKey, pValueName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));
	return m_lLastError == ERROR_SUCCESS;
}

bool regKey::writeUI8(PCWSTR pValueName, ULONGLONG ullValue)
{
	assert(m_hKey);
	m_lLastError = ::RegSetValueExW(m_hKey, pValueName, 0, REG_QWORD, reinterpret_cast<const BYTE*>(&ullValue), sizeof(ULONGLONG));
	return m_lLastError == ERROR_SUCCESS;

}

bool regKey::writeString(PCWSTR pValueName, const std::wstring &wsValue, DWORD dwType /*= REG_SZ*/)
{
	assert(m_hKey);
	m_lLastError = ::RegSetValueExW(m_hKey, pValueName, 0, dwType, reinterpret_cast<const BYTE*>(wsValue.data()), (wsValue.length()+1)*sizeof(wchar_t));
	return m_lLastError == ERROR_SUCCESS;

}

bool regKey::writeBinary(PCWSTR pValueName, const SBDataType &sbValue)
{
	assert(m_hKey);
	m_lLastError = ::RegSetValueExW(m_hKey, pValueName, 0, REG_BINARY, reinterpret_cast<const BYTE*>(sbValue.data()), sbValue.size());
	return m_lLastError == ERROR_SUCCESS;

}
bool regKey::enumKey(DWORD iIndex, std::wstring &wsName, FILETIME *plastWriteTime /*= NULL*/, ULONG uiChars /*= MAX_PATH*/)
{
	assert(m_hKey);
	FILETIME ftLastWriteTime = {0};
	wchar_t *pName = new wchar_t[uiChars];
	if (plastWriteTime == NULL)
	{
		plastWriteTime = &ftLastWriteTime;
	}

	m_lLastError = ::RegEnumKeyExW(m_hKey, iIndex, pName, &uiChars, NULL, NULL, NULL, plastWriteTime);
	//if(m_lLastError != ERROR_SUCCESS)
	//{
	//	delete [] pName;
	//	return false;
	//}
	if(m_lLastError == ERROR_SUCCESS)
		wsName = pName;
	delete pName;
	return m_lLastError == ERROR_SUCCESS;
}

bool regKey::notifyChangeKeyValue(bool bWatchSubtree, DWORD dwNotifyFilter, HANDLE hEvent)
{
	if(!hEvent) return false;
	assert(m_hKey);
	m_lLastError = ::RegNotifyChangeKeyValue(m_hKey, bWatchSubtree, dwNotifyFilter, hEvent, TRUE);

	return m_lLastError == ERROR_SUCCESS;
}

bool regKey::readString(HKEY hKey, PCWSTR pSubKeyName, PCWSTR pValue, std::wstring &wsValue)
{
	assert(pSubKeyName);
	LSTATUS ls = ERROR_SUCCESS;
	DWORD dwType = 0;
	DWORD dwSize = 0;
	wsValue.clear();
	ls = SHGetValueW(hKey, pSubKeyName, pValue, &dwType, NULL, &dwSize);
	if (ls == ERROR_SUCCESS && dwSize > 0)
	{
		wchar_t *pbuf = new wchar_t[dwSize+1];
		memset(pbuf, 0, (dwSize+1)*sizeof(wchar_t));
		ls = SHGetValueW(hKey, pSubKeyName, pValue, &dwType, pbuf, &dwSize);
		if(ls == ERROR_SUCCESS)
			wsValue = pbuf;
		delete[] pbuf;
	}
	return ls == ERROR_SUCCESS;
}

bool regKey::writeString(HKEY hKey, PCWSTR pSubKeyName, PCWSTR pValue, const std::wstring &wsValue)
{
	assert(pSubKeyName);
	if(!pSubKeyName) return false;
	LSTATUS ls = SHSetValue(hKey, pSubKeyName, pValue, REG_SZ, wsValue.c_str(), wsValue.length()*sizeof(wchar_t));
	return ls == ERROR_SUCCESS;
}

}