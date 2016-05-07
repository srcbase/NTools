#pragma once
namespace wui {



enum
{
	XMLFILE_ENCODING_UTF8 = 0,
	XMLFILE_ENCODING_UNICODE = 1,
	XMLFILE_ENCODING_ASNI = 2,
};

class markup;
class markupNode;


class WINUI_API markup
{
	friend markupNode;
public:
	markup(PCWSTR pstrXML = NULL);
	~markup();

	bool load(PCWSTR pstrXML);
	bool loadFromMem(BYTE* pByte, DWORD dwSize, int encoding = XMLFILE_ENCODING_UTF8);
	bool loadFromFile(PCWSTR pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
	void release();
	bool isValid() const;

	void setPreserveWhitespace(bool bPreserve = true);
	void getLastErrorMessage(PWSTR pstrMessage, SIZE_T cchMax) const;
	void getLastErrorLocation(PWSTR pstrSource, SIZE_T cchMax) const;

	markupNode getRoot();

private:
	typedef struct tagXMLELEMENT
	{
		ULONG iStart;
		ULONG iChild;
		ULONG iNext;
		ULONG iParent;
		ULONG iData;
	} XMLELEMENT;

	PWSTR m_pstrXML;
	XMLELEMENT* m_pElements;
	ULONG m_nElements;
	ULONG m_nReservedElements;
	wchar_t m_szErrorMsg[100];
	wchar_t m_szErrorXML[50];
	bool m_bPreserveWhitespace;

private:
	bool _parse();
	bool _parse(PWSTR& pstrText, ULONG iParent);
	XMLELEMENT* _reserveElement();
	inline void _skipWhitespace(PWSTR& pstr) const;
	inline void _skipWhitespace(PCWSTR& pstr) const;
	inline void _skipIdentifier(PWSTR& pstr) const;
	inline void _skipIdentifier(PCWSTR& pstr) const;
	bool _parseData(PWSTR& pstrText, PWSTR& pstrData, wchar_t cEnd);
	void _parseMetaChar(PWSTR& pstrText, PWSTR& pstrDest);
	bool _parseAttributes(PWSTR& pstrText);
	bool _failed(PCWSTR pstrError, PCWSTR pstrLocation = NULL);
};


class WINUI_API markupNode
{
	friend markup;
private:
	markupNode();
	markupNode(markup* pOwner, int iPos);

public:
	bool isValid() const;

	markupNode getParent();
	markupNode getSibling();
	markupNode getChild();
	markupNode getChild(PCWSTR pstrName);

	bool hasSiblings() const;
	bool hasChildren() const;
	PCWSTR getName() const;
	PCWSTR getValue() const;

	bool hasAttributes();
	bool hasAttribute(PCWSTR pstrName);
	int getAttributeCount();
	PCWSTR getAttributeName(int iIndex);
	PCWSTR getAttributeValue(int iIndex);
	PCWSTR getAttributeValue(PCWSTR pstrName);
	bool getAttributeValue(int iIndex, PWSTR pstrValue, SIZE_T cchMax);
	bool getAttributeValue(PCWSTR pstrName, PWSTR pstrValue, SIZE_T cchMax);

private:
	void _mapAttributes();

	enum { MAX_XML_ATTRIBUTES = 64 };

	typedef struct
	{
		ULONG iName;
		ULONG iValue;
	} XMLATTRIBUTE;

	int m_iPos;
	int m_nAttributes;
	XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];
	markup* m_pOwner;
};
}