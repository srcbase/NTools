#pragma once
#include "UC/wucList.h"

namespace wuc {

class folderViewUC;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IFolderCallbackUC 类		2015-2-5
//	该结构给folderViewUC的拥有者提供给其使用
////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFolderCallbackUC : public IListCallbackUC
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-10 列表控件中实际包含的节点数 >=0 and < iItem*cols	
	virtual std::size_t getNodeCount() = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-10 得到节点相对于自身节点集合的索引	
	virtual int getNodeIndex(int iItem, int iSubItem) = 0;
	virtual std::wstring getNodeKeyName(int nodeIndex) = 0;
	virtual std::wstring getNodeTipInfo(int nodeIndex) = 0;
	virtual int getSubItemIndex(int nodeIndex) const = 0;	// 根据节点索引得到其对应位置索引
	virtual bool getNodePosition(int nodeIndex, RECT *prcNode) const = 0;
	virtual bool ptInNode(int nodeIndex, POINT pt) = 0;		// 查验坐标是否仅在图标或文本区
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-10 设定节点状态 nodeIndex相对于自身控件内节点结合
	virtual bool setNodeState(int nodeIndex, DWORD dwAdd, DWORD dwRemove) = 0;
	virtual bool setNodeState(std::wstring wsKeyName, DWORD dwAdd, DWORD dwRemove) = 0;
	virtual DWORD getNodeState(int nodeIndex) = 0;
	virtual bool reverseNodeState(int nodeIndex, DWORD dwReverseState) = 0;
	virtual void setAllNodeState(DWORD dwAdd, DWORD dwRemove) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-5 使某个节点处于选中+焦点状态，其它节点的状态清空	
	virtual bool selectNodeOnly(int nodeIndex) = 0;
	virtual folderViewUC* getFocusFolderView() = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-5 控件位置改变
	virtual void positionChanged() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-2-6 所有的框和桌面的节点中只能有一个节点处于焦点状态
struct BSFocusedNodeInfo {
	folderViewUC	*pfolderView;	// 焦点节点所属框或桌面
	//int				iNodeIndex;		// 相对于框或桌面内部节点集合的索引
	std::wstring	wsKeyName;		// 节点Key名称；当当前操作框与本结构不一致时只能有key名区分了
	//bool			bDoneSelect;	// 是否执行过选中操作，避免重复调用dialogMain::setAllNodeState清空选中状态
	// 
	BSFocusedNodeInfo()
		: pfolderView(nullptr)
		//, iNodeIndex(-1)
		//, bDoneSelect(false)
	{}

	// 释放节点的焦点状态
	void releaseState();
	
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-2-9 只能有一个节点处于hot状态
struct BSHotNodeInfo {
	folderViewUC	*pfolderView;	// hot节点所属框或桌面
	int				iNodeIndex;		// 相对于框或桌面内部节点集合的索引
	//std::wstring	wsKeyName;		// 节点Key名称
	BSHotNodeInfo()
		: pfolderView(nullptr)
		, iNodeIndex(-1)
	{}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-22	listUC采用的两种显示方式
enum BEFolderStyleFlag {
	BEFolderStyleFlagList			= 0,	//	以列表框方式显示（1列多行）
	BEFolderStyleFlagIcon					//	以中等图标文本方式显示（多列多行）
											//		1、图标的小、中、大尺寸分别为32、48、96；最小16，最大256
											//		2、节点最小宽度为20，最大为260，当前宽度为SystemParametersInfo<SPI_ICONHORIZONTALSPACING>
											//		3、标签自适应，如：单行19 双行36...
											//		4、绘制标签，左右各预留2，上下各预留1
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	folderElementUC		2015-2-2
//	该元素以中等图标或列表等方式显示节点
//	节点由以下4部分组成：节点区、图标区、标签区。图标区和标签区包含在节点区内
//	节点区：大小为1个图标+最多2行文本（默认，选中时可全显示）
//	标签区：在标签显示范围的基础上，左右外扩展各2，上下外扩展1
//	图标区：大小为8的整数倍，绘制区域宽度为节点宽度，高度为图标高度上下外扩展2
//	标签区与图标区相隔1
////////////////////////////////////////////////////////////////////////////////////////////////////////
class folderElementUC : public listElementUC
{
public:
	folderElementUC();
	~folderElementUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual void Event(BSEventUC& event);
	virtual SIZE estimateSize(SIZE szAvailable);
	virtual void paint(HDC hdc, const RECT &rcPaint);
public:
	/****************************************************************************************
		<< --- 		2015-2-5 --- >>         
		说明：找到光标点所在位置的列索引，可能的话并返回其所在位置
		参数：
			pt			= 被查验的点
			prcNodePos	= 不为空时返回节点位置区域
		返回值：= -1未找到 >=0为列索引
	*****************************************************************************************/
	int findColumn(POINT pt, RECT *prcNodePos = NULL);
	
public:
	// IListItemUC
	virtual void drawItem(HDC hdc, const RECT& rcItem);
	virtual void setOwner(controlUC* pOwner);
public:
	//PCWSTR getFullPath(int iSubItem);
	//void setFullPath(int iSubItem, PCWSTR pfullPath);
	//BEFolderStyleFlag getFolderStyle() const;
	//void setFolderStyle(BEFolderStyleFlag style);
protected:
	folderViewUC		*m_pOwner;
	BEFolderStyleFlag	m_beFolderStyle;
	//std::vector<std::wstring>	m_vecFullPathSubs;	// 本行文件夹或文件的全路径集合，其数量等于列数，最后一个行可以不全
};




////////////////////////////////////////////////////////////////////////////////////////////////////////
//	folderViewUC类		2015-1-29
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class folderViewUC : public listUC
{
public:
	folderViewUC();
	~folderViewUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;
	virtual void Event(BSEventUC& event);
	virtual void setPosition(RECT pos);
	//virtual SIZE estimateSize(SIZE szAvailable);
public:
	/****************************************************************************************
		<< --- 	init	2015-2-4 --- >>         
		说明：初始化，并布局列表头和列表元素
		参数：
			styleFlag	= 列表框的表现形式
			szNode		= 在BEFolderStyleFlagIcon模式下单个元素的尺寸
			szNodeSpace	= 节点右侧的底部预留空间
		返回值：
	*****************************************************************************************/
	void init(BEFolderStyleFlag styleFlag, SIZE szNode, SIZE szNodeSpace);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-5 刷新指定节点区域，subItemIndex节点位置索引
	void invalidateEx(int subItemIndex);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-6 根据子项位置索引得到其位置
	bool getSubItemPosition(int subItemIndex, RECT &rcPos);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-23 根据子项位置索引得到其行列索引值
	bool getItemPositionIndex(int subItemIndex, int &refiItem, int &refiSubItem);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-4 根据列表样式和节点大小以及自身大小重新布局列标题和列元素	
	void refreshLayout(bool bforceUpdate = false);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-10 设定节点的hot状态
	void hoverNode(int iItem, int iSubItem);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-2-10 设定节点的选中状态
	//void selectNode(int iItem, int iSubItem, UINT keyState);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-11 只选中指定的节点，并使其它节点处于非选中状态
	int selectNodeOnly(int iItem, int iSubItem);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-11 得到指定节点的状态
	DWORD getNodeState(int iItem, int iSubItem);
	/****************************************************************************************
		<< --- setNodeFocus		2015-3-5 --- >>         
		说明：设定节点的focus状态，处于选中状态的节点必须处于选中状态
		参数：
			nodeIndex	= 得到焦点的节点索引（基于0，相对于框内节点集合）
		返回值：原先节点索引，如果原节点不属于本框返回-1
	*****************************************************************************************/
	int setNodeFocus(int nodeIndex);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-11 框内范围节点的选中，并返回焦点节点索引
	int selectRangeNode(int iItem, int iSubItem);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-23 	
	int getCols() const;
	int getRows() const;
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-6-30 查找点所在的列表项元素
	listElementUC* findElement(POINT pt) const;
protected:
	/****************************************************************************************
		<< --- 		2015-2-10 --- >>         
		说明：修正列表头的数量
		参数：
			targetCols	= 列表头的目标数量
			fixedWidth	= 列宽度
			bforceUpdateWidth	= 是否强制刷新所有列宽度
		返回值：是否修正过列数量
	*****************************************************************************************/
	bool modifyHeaderCount(int targetCols, int fixedWidth = 0, bool bforceUpdateWidth = false);
	/****************************************************************************************
		<< --- 		2015-2-10 --- >>         
		说明：修正列表元素数量
		参数：
		返回值：
	*****************************************************************************************/
	bool modifyElementCount(int targetRows, int fixedHeight = 0, bool bforceUpdateHeight = false);
public:
	BEFolderStyleFlag getFolderStyle() const;
	void setFolderStyle(BEFolderStyleFlag style);
	SIZE getNodeWorkSize() const;
	SIZE getNodeSize() const;
	void setNodeSize(SIZE szNode);
	SIZE getNodeSpaceSize() const;
	void setNodeSpaceSize(SIZE szNode);
	void setOwner(controlUC *pOwner);
	IFolderCallbackUC* getOwner() const;
	void setListItemHeight(int iHeight);
	int getListItemHeight() const;
	//BSFocusedNodeInfo& getFocusedNode();
	BSHotNodeInfo& getHotNode();
	bool isSetterHorizontal() const;
	void setSetterHorizontal(bool bHorizontal);
	bool isSetterAnywhere() const;
	void setSetterAnywhere(bool bAnywhere);
	int getCurrentNodeIndex() const;
	std::wstring toString() const;
protected:
	BEFolderStyleFlag	m_beFolderStyle;
	bool				m_bSetterHorizontal;		// 在BEFolderStyleFlagIcon模式下，=true 节点水平从左到右、从上到下；=false 从上到下、从左到右
	bool				m_bSetterAnywhere;			// 节点可以在容器的任意位置
	int					m_iListItemHeight;			// 在BEFolderStyleFlagList模式下的列元素行高
	
	SIZE				m_szNode;					// 节点绘制区大小
	SIZE				m_szNodeSpace;				// 节点间空隙，空除节点右侧和底部
	
	IFolderCallbackUC	*m_pOwner;
	int					m_iCurNodeIndex;			// 当前选中的节点索引（相对于本框节点集合）
	//static BSFocusedNodeInfo	mc_bsFocusedNode;	// 用静态的，以保证框和桌面间共享
	static BSHotNodeInfo		mc_bsHotNode;		// 用静态的，以保证框和桌面间共享
};



}	// end wuc