#include "StdAfx.h"
#include "makeup.h"

///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{ 
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
///////////////////////////////////////////////////////////////////////////////////////

namespace wui {



markupNode::markupNode() : m_pOwner(NULL)
{
	////TRACE_CLASS_BEGIN(this)
}

markupNode::markupNode(markup* pOwner, int iPos) : m_pOwner(pOwner), m_iPos(iPos), m_nAttributes(0)
{
	////TRACE_CLASS_END(this)
}

markupNode markupNode::getSibling()
{
	if( m_pOwner == NULL ) return markupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
	if( iPos == 0 ) return markupNode();
	return markupNode(m_pOwner, iPos);
}

bool markupNode::hasSiblings() const
{
	if( m_pOwner == NULL ) return false;
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
	return iPos > 0;
}

markupNode markupNode::getChild()
{
	if( m_pOwner == NULL ) return markupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
	if( iPos == 0 ) return markupNode();
	return markupNode(m_pOwner, iPos);
}

markupNode markupNode::getChild(PCWSTR pstrName)
{
	if( m_pOwner == NULL ) return markupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
	while( iPos != 0 ) {
		if( _tcscmp(m_pOwner->m_pstrXML + m_pOwner->m_pElements[iPos].iStart, pstrName) == 0 ) {
			return markupNode(m_pOwner, iPos);
		}
		iPos = m_pOwner->m_pElements[iPos].iNext;
	}
	return markupNode();
}

bool markupNode::hasChildren() const
{
	if( m_pOwner == NULL ) return false;
	return m_pOwner->m_pElements[m_iPos].iChild != 0;
}

markupNode markupNode::getParent()
{
	if( m_pOwner == NULL ) return markupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iParent;
	if( iPos == 0 ) return markupNode();
	return markupNode(m_pOwner, iPos);
}

bool markupNode::isValid() const
{
	return m_pOwner != NULL;
}

PCWSTR markupNode::getName() const
{
	if( m_pOwner == NULL ) return NULL;
	return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
}

PCWSTR markupNode::getValue() const
{
	if( m_pOwner == NULL ) return NULL;
	return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
}

PCWSTR markupNode::getAttributeName(int iIndex)
{
	if( m_pOwner == NULL ) return NULL;
	if( m_nAttributes == 0 ) _mapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return L"";
	return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iName;
}

PCWSTR markupNode::getAttributeValue(int iIndex)
{
	if( m_pOwner == NULL ) return NULL;
	if( m_nAttributes == 0 ) _mapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return L"";
	return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue;
}

PCWSTR markupNode::getAttributeValue(PCWSTR pstrName)
{
	if( m_pOwner == NULL ) return NULL;
	if( m_nAttributes == 0 ) _mapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( _tcscmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
	}
	return L"";
}

bool markupNode::getAttributeValue(int iIndex, PWSTR pstrValue, SIZE_T cchMax)
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _mapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return false;
	_tcsncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue, cchMax);
	return true;
}

bool markupNode::getAttributeValue(PCWSTR pstrName, PWSTR pstrValue, SIZE_T cchMax)
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _mapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( _tcscmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) {
			_tcsncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[i].iValue, cchMax);
			return true;
		}
	}
	return false;
}

int markupNode::getAttributeCount()
{
	if( m_pOwner == NULL ) return 0;
	if( m_nAttributes == 0 ) _mapAttributes();
	return m_nAttributes;
}

bool markupNode::hasAttributes()
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _mapAttributes();
	return m_nAttributes > 0;
}

bool markupNode::hasAttribute(PCWSTR pstrName)
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _mapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( _tcscmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return true;
	}
	return false;
}

void markupNode::_mapAttributes()
{
	m_nAttributes = 0;
	PCWSTR pstr = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
	PCWSTR pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
	pstr += _tcslen(pstr) + 1;
	while( pstr < pstrEnd ) {
		m_pOwner->_skipWhitespace(pstr);
		m_aAttributes[m_nAttributes].iName = pstr - m_pOwner->m_pstrXML;
		pstr += _tcslen(pstr) + 1;
		m_pOwner->_skipWhitespace(pstr);
		if( *pstr++ != L'\"' ) return; // if( *pstr != L'\"') ) { pstr = ::CharNext(pstr); return; }

		m_aAttributes[m_nAttributes++].iValue = pstr - m_pOwner->m_pstrXML;
		if( m_nAttributes >= MAX_XML_ATTRIBUTES ) return;
		pstr += _tcslen(pstr) + 1;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
//
//
//

markup::markup(PCWSTR pstrXML)
{
	//TRACE_CLASS_BEGIN(this)
	m_pstrXML = NULL;
	m_pElements = NULL;
	m_nElements = 0;
	m_bPreserveWhitespace = true;
	if( pstrXML != NULL ) load(pstrXML);
}

markup::~markup()
{
	//TRACE_CLASS_END(this)
	release();
}

bool markup::isValid() const
{
	return m_pElements != NULL;
}

void markup::setPreserveWhitespace(bool bPreserve)
{
	m_bPreserveWhitespace = bPreserve;
}

bool markup::load(PCWSTR pstrXML)
{
	release();
	SIZE_T cchLen = _tcslen(pstrXML) + 1;
	m_pstrXML = static_cast<PWSTR>(malloc(cchLen * sizeof(TCHAR)));
	::CopyMemory(m_pstrXML, pstrXML, cchLen * sizeof(TCHAR));
	bool bRes = _parse();
	if( !bRes ) release();
	return bRes;
}

bool markup::loadFromMem(BYTE* pByte, DWORD dwSize, int encoding)
{
#ifdef _UNICODE
	if (encoding == XMLFILE_ENCODING_UTF8)
	{
		if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
		{
			pByte += 3; dwSize -= 3;
		}
		DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

		m_pstrXML = static_cast<PWSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
		::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
		m_pstrXML[nWide] = L'\0';
	}
	else if (encoding == XMLFILE_ENCODING_ASNI)
	{
		DWORD nWide = ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

		m_pstrXML = static_cast<PWSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
		::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
		m_pstrXML[nWide] = L'\0';
	}
	else
	{
		if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
		{
			dwSize = dwSize / 2 - 1;

			if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
			{
				pByte += 2;

				for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
				{
					register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
					pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
					pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
				}
			}
			else
			{
				pByte += 2;
			}

			m_pstrXML = static_cast<PWSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
			::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
			m_pstrXML[dwSize] = L'\0';

			pByte -= 2;
		}
	}
#else // !_UNICODE
	if (encoding == XMLFILE_ENCODING_UTF8)
	{
		if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
		{
			pByte += 3; dwSize -= 3;
		}
		DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

		LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1)*sizeof(WCHAR)));
		::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, w_str, nWide );
		w_str[nWide] = L'\0';

		DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, NULL, 0, NULL, NULL);

		m_pstrXML = static_cast<PWSTR>(malloc((wide + 1)*sizeof(TCHAR)));
		::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)w_str, nWide, m_pstrXML, wide, NULL, NULL);
		m_pstrXML[wide] = L'\0');

		free(w_str);
	}
	else if (encoding == XMLFILE_ENCODING_UNICODE)
	{
		if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
		{
			dwSize = dwSize / 2 - 1;

			if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
			{
				pByte += 2;

				for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
				{
					register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
					pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
					pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
				}
			}
			else
			{
				pByte += 2;
			}

			DWORD nWide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByte, dwSize, NULL, 0, NULL, NULL);
			m_pstrXML = static_cast<PWSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
			::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)pByte, dwSize, m_pstrXML, nWide, NULL, NULL);
			m_pstrXML[nWide] = L'\0');

			pByte -= 2;
		}
	}
	else
	{
		m_pstrXML = static_cast<PWSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
		::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
		m_pstrXML[dwSize] = L'\0');
	}
#endif // _UNICODE

	bool bRes = _parse();
	if( !bRes ) release();
	return bRes;
}

bool markup::loadFromFile(PCWSTR pstrFilename, int encoding)
{
	release();
	stringex sFile = paintManagerUI::getResourcePath();
	if( paintManagerUI::getResourceZip().empty() ) {
		sFile += pstrFilename;
		HANDLE hFile = ::CreateFile(sFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE ) return _failed(L"Error opening file");
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 ) return _failed(L"File is empty");
		if ( dwSize > 4096*1024 ) return _failed(L"File too large");

		DWORD dwRead = 0;
		BYTE* pByte = new BYTE[ dwSize ];
		::ReadFile( hFile, pByte, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );

		if( dwRead != dwSize ) {
			delete[] pByte;
			release();
			return _failed(L"Could not read file");
		}
		bool ret = loadFromMem(pByte, dwSize, encoding);
		delete[] pByte;

		return ret;
	}
	else {
		sFile += paintManagerUI::getResourceZip();
		HZIP hz = OpenZip((void*)sFile.data(), 0, 2);
		ZIPENTRY ze; 
		int i; 
		if( FindZipItem(hz, pstrFilename, true, &i, &ze) != 0 ) return _failed(L"Could not find ziped file");
		DWORD dwSize = ze.unc_size;
		if( dwSize == 0 ) return _failed(L"File is empty");
		if ( dwSize > 4096*1024 ) return _failed(L"File too large");
		BYTE* pByte = new BYTE[ dwSize ];
		int res = UnzipItem(hz, i, pByte, dwSize, 3);
		if( res != 0x00000000 && res != 0x00000600) {
			delete[] pByte;
			CloseZip(hz);
			return _failed(L"Could not unzip file");
		}
		CloseZip(hz);
		bool ret = loadFromMem(pByte, dwSize, encoding);
		delete[] pByte;

		return ret;
	}
}

void markup::release()
{
	if( m_pstrXML != NULL ) free(m_pstrXML);
	if( m_pElements != NULL ) free(m_pElements);
	m_pstrXML = NULL;
	m_pElements = NULL;
	m_nElements;
}

void markup::getLastErrorMessage(PWSTR pstrMessage, SIZE_T cchMax) const
{
	_tcsncpy(pstrMessage, m_szErrorMsg, cchMax);
}

void markup::getLastErrorLocation(PWSTR pstrSource, SIZE_T cchMax) const
{
	_tcsncpy(pstrSource, m_szErrorXML, cchMax);
}

markupNode markup::getRoot()
{
	if( m_nElements == 0 ) return markupNode();
	return markupNode(this, 1);
}

bool markup::_parse()
{
	_reserveElement(); // Reserve index 0 for errors
	::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
	::ZeroMemory(m_szErrorXML, sizeof(m_szErrorXML));
	PWSTR pstrXML = m_pstrXML;
	return _parse(pstrXML, 0);
}

bool markup::_parse(PWSTR& pstrText, ULONG iParent)
{
	_skipWhitespace(pstrText);
	ULONG iPrevious = 0;
	for( ; ; ) 
	{
		if( *pstrText == L'\0' && iParent <= 1 ) return true;
		_skipWhitespace(pstrText);
		if( *pstrText != L'<' ) return _failed(L"Expected start tag", pstrText);
		if( pstrText[1] == L'/' ) return true;
		*pstrText++ = L'\0';
		_skipWhitespace(pstrText);
		// Skip comment or processing directive
		if( *pstrText == L'!' || *pstrText == L'?' ) {
			TCHAR ch = *pstrText;
			if( *pstrText == L'!' ) ch = L'-';
			while( *pstrText != L'\0' && !(*pstrText == ch && *(pstrText + 1) == L'>') ) pstrText = ::CharNext(pstrText);
			if( *pstrText != L'\0' ) pstrText += 2;
			_skipWhitespace(pstrText);
			continue;
		}
		_skipWhitespace(pstrText);
		// Fill out element structure
		XMLELEMENT* pEl = _reserveElement();
		ULONG iPos = pEl - m_pElements;
		pEl->iStart = pstrText - m_pstrXML;
		pEl->iParent = iParent;
		pEl->iNext = pEl->iChild = 0;
		if( iPrevious != 0 ) m_pElements[iPrevious].iNext = iPos;
		else if( iParent > 0 ) m_pElements[iParent].iChild = iPos;
		iPrevious = iPos;
		// Parse name
		PCWSTR pstrName = pstrText;
		_skipIdentifier(pstrText);
		PWSTR pstrNameEnd = pstrText;
		if( *pstrText == L'\0' ) return _failed(L"Error parsing element name", pstrText);
		// Parse attributes
		if( !_parseAttributes(pstrText) ) return false;
		_skipWhitespace(pstrText);
		if( pstrText[0] == L'/' && pstrText[1] == L'>' )
		{
			pEl->iData = pstrText - m_pstrXML;
			*pstrText = L'\0';
			pstrText += 2;
		}
		else
		{
			if( *pstrText != L'>' ) return _failed(L"Expected start-tag closing", pstrText);
			// Parse node data
			pEl->iData = ++pstrText - m_pstrXML;
			PWSTR pstrDest = pstrText;
			if( !_parseData(pstrText, pstrDest, L'<') ) return false;
			// Determine type of next element
			if( *pstrText == L'\0' && iParent <= 1 ) return true;
			if( *pstrText != L'<' ) return _failed(L"Expected end-tag start", pstrText);
			if( pstrText[0] == L'<' && pstrText[1] != L'/' ) 
			{
				if( !_parse(pstrText, iPos) ) return false;
			}
			if( pstrText[0] == L'<' && pstrText[1] == L'/' ) 
			{
				*pstrDest = L'\0';
				*pstrText = L'\0';
				pstrText += 2;
				_skipWhitespace(pstrText);
				SIZE_T cchName = pstrNameEnd - pstrName;
				if( _tcsncmp(pstrText, pstrName, cchName) != 0 ) return _failed(L"Unmatched closing tag", pstrText);
				pstrText += cchName;
				_skipWhitespace(pstrText);
				if( *pstrText++ != L'>' ) return _failed(L"Unmatched closing tag", pstrText);
			}
		}
		*pstrNameEnd = L'\0';
		_skipWhitespace(pstrText);
	}
}

markup::XMLELEMENT* markup::_reserveElement()
{
	if( m_nElements == 0 ) m_nReservedElements = 0;
	if( m_nElements >= m_nReservedElements ) {
		m_nReservedElements += (m_nReservedElements / 2) + 500;
		m_pElements = static_cast<XMLELEMENT*>(realloc(m_pElements, m_nReservedElements * sizeof(XMLELEMENT)));
	}
	return &m_pElements[m_nElements++];
}

void markup::_skipWhitespace(PCWSTR& pstr) const
{
	while( *pstr > L'\0' && *pstr <= L' ' ) pstr = ::CharNext(pstr);
}

void markup::_skipWhitespace(PWSTR& pstr) const
{
	while( *pstr > L'\0' && *pstr <= L' ' ) pstr = ::CharNext(pstr);
}

void markup::_skipIdentifier(PCWSTR& pstr) const
{
	// 属性只能用英文，所以这样处理没有问题
	while( *pstr != L'\0' && (*pstr == L'_' || *pstr == L':' || _istalnum(*pstr)) ) pstr = ::CharNext(pstr);
}

void markup::_skipIdentifier(PWSTR& pstr) const
{
	// 属性只能用英文，所以这样处理没有问题
	while( *pstr != L'\0' && (*pstr == L'_' || *pstr == L':' || _istalnum(*pstr)) ) pstr = ::CharNext(pstr);
}

bool markup::_parseAttributes(PWSTR& pstrText)
{   
	if( *pstrText == L'>' ) return true;
	*pstrText++ = L'\0';
	_skipWhitespace(pstrText);
	while( *pstrText != L'\0' && *pstrText != L'>' && *pstrText != L'/' ) {
		_skipIdentifier(pstrText);
		PWSTR pstrIdentifierEnd = pstrText;
		_skipWhitespace(pstrText);
		if( *pstrText != L'=' ) return _failed(L"Error while parsing attributes", pstrText);
		*pstrText++ = L' ';
		*pstrIdentifierEnd = L'\0';
		_skipWhitespace(pstrText);
		if( *pstrText++ != L'\"' ) return _failed(L"Expected attribute value", pstrText);
		PWSTR pstrDest = pstrText;
		if( !_parseData(pstrText, pstrDest, L'\"') ) return false;
		if( *pstrText == L'\0' ) return _failed(L"Error while parsing attribute string", pstrText);
		*pstrDest = L'\0';
		if( pstrText != pstrDest ) *pstrText = L' ';
		pstrText++;
		_skipWhitespace(pstrText);
	}
	return true;
}

bool markup::_parseData(PWSTR& pstrText, PWSTR& pstrDest, wchar_t cEnd)
{
	while( *pstrText != L'\0' && *pstrText != cEnd ) {
		if( *pstrText == L'&' ) {
			while( *pstrText == L'&' ) {
				_parseMetaChar(++pstrText, pstrDest);
			}
			if (*pstrText == cEnd)
				break;
		}

		if( *pstrText == L' ' ) {
			*pstrDest++ = *pstrText++;
			if( !m_bPreserveWhitespace ) _skipWhitespace(pstrText);
		}
		else {
			PWSTR pstrTemp = ::CharNext(pstrText);
			while( pstrText < pstrTemp) {
				*pstrDest++ = *pstrText++;
			}
		}
	}
	// Make sure that MapAttributes() works correctly when it parses
	// over a value that has been transformed.
	PWSTR pstrFill = pstrDest + 1;
	while( pstrFill < pstrText ) *pstrFill++ = L' ';
	return true;
}

void markup::_parseMetaChar(PWSTR& pstrText, PWSTR& pstrDest)
{
	if( pstrText[0] == L'a' && pstrText[1] == L'm' && pstrText[2] == L'p' && pstrText[3] == L';' ) {
		*pstrDest++ = L'&';
		pstrText += 4;
	}
	else if( pstrText[0] == L'l' && pstrText[1] == L't' && pstrText[2] == L';' ) {
		*pstrDest++ = L'<';
		pstrText += 3;
	}
	else if( pstrText[0] == L'g' && pstrText[1] == L't' && pstrText[2] == L';' ) {
		*pstrDest++ = L'>';
		pstrText += 3;
	}
	else if( pstrText[0] == L'q' && pstrText[1] == L'u' && pstrText[2] == L'o' && pstrText[3] == L't' && pstrText[4] == L';' ) {
		*pstrDest++ = L'\"';
		pstrText += 5;
	}
	else if( pstrText[0] == L'a' && pstrText[1] == L'p' && pstrText[2] == L'o' && pstrText[3] == L's' && pstrText[4] == L';' ) {
		*pstrDest++ = L'\'';
		pstrText += 5;
	}
	else {
		*pstrDest++ = L'&';
	}
}

bool markup::_failed(PCWSTR pstrError, PCWSTR pstrLocation)
{
	// Register last error
	TRACE(L"XML Error(%u)%s", ::GetLastError(), pstrError);
	if( pstrLocation != NULL ) TRACE(pstrLocation);
	_tcsncpy(m_szErrorMsg, pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
	_tcsncpy(m_szErrorXML, pstrLocation != NULL ? pstrLocation : L"", _countof(m_szErrorXML) - 1);
	return false; // Always return 'false'
}
}