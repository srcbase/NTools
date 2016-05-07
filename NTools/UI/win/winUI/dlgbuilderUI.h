#pragma once
namespace wui{


class IDialogBuilderCallback
{
public:
	virtual controlUI* createControl(PCWSTR pstrClass) = 0;
};


class WINUI_API dialogBuilder
{
public:
	dialogBuilder();
	~dialogBuilder() {		//TRACE_CLASS_END(this)									
	}
	controlUI* create(stringOrID xml, PCWSTR type = NULL, IDialogBuilderCallback* pCallback = NULL,
		paintManagerUI* pManager = NULL, controlUI* pParent = NULL);
	controlUI* create(IDialogBuilderCallback* pCallback = NULL, paintManagerUI* pManager = NULL,
		controlUI* pParent = NULL);

	markup* getMarkup();

	void getLastErrorMessage(PWSTR pstrMessage, SIZE_T cchMax) const;
	void getLastErrorLocation(PWSTR pstrSource, SIZE_T cchMax) const;
private:
	controlUI* _parse(markupNode* parent, controlUI* pParent = NULL, paintManagerUI* pManager = NULL);

	markup m_xml;
	IDialogBuilderCallback* m_pCallback;
	PCWSTR m_pstrtype;
};


}