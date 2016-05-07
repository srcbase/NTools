#ifndef __KPDESKINFO_H__
#define __KPDESKINFO_H__
#pragma once

#include <string>
#include <vector>
#include <map>
#include <cwchar>
#include <sstream>
#include <cstdint>
#include <ctime>
#include <queue>
#include <deque>
#include <assert.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <ShellAPI.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"shell32.lib")

#include <Msi.h>
#pragma comment(lib,"Msi.lib")

#include "AC/basedef.h"
#include "KP/KPDeskDef.h"
#include "AC/sysupport.h"

#define IDLNODE_INIT_INDEX		-1000		
namespace gtc {
//////////////////////////////////////////////////////////////////////////
// lnk文件头
typedef struct BSLinkHead
{
	DWORD		dwID;
	DWORD		dwGUID[4];
	DWORD		dwFlags;
	DWORD		dwFileAttributes;
	FILETIME	dwCreationTime;
	FILETIME	dwModificationTime;
	FILETIME	dwLastaccessTime;
	DWORD		dwFileLen;
	DWORD		dwIconNum;
	DWORD		dwWinStyle;
	DWORD		dwHotkey;
	DWORD		dwReserved1;
	DWORD		dwReserved2;
}BSLinkHead;

#define LNK_HASIDLIST	0x1
#define LNK_FILEDIR		0x2
#define LNK_HASDES		0x4
#define LNK_HASPATH		0x8
#define LNK_HASWORKDIR	0x10
#define LNK_HASCMD		0x20

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	BSNodeMoveInfo moveNodeCache类	2014-12-25
//	没有成功移动节点的缓存；如：节点被其它操作锁定
////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct BSNodeMoveInfo {
	wchar_t		pkeyName[MAX_PATH];
	POINT		pos;
	BSNodeMoveInfo()
	{
		pkeyName[0] = L'\0';
		pos.x = pos.y = 0;
	}
	BSNodeMoveInfo(PCWSTR pName, POINT pt)
	{
		if(pName)
			std::wcsncpy(pkeyName, pName, _countof(pkeyName));
		else
			pkeyName[0] = L'\0';
		memcpy(&pos, &pt, sizeof(pos));
	}
	bool operator==(const BSNodeMoveInfo &data) const
	{
		return std::wcsncmp(pkeyName, data.pkeyName, _countof(pkeyName)) == 0;
	}
	bool operator==(PCWSTR pName) const
	{
		return std::wcsncmp(pkeyName, pName, _countof(pkeyName)) == 0;
	}
}BSNodeMoveInfo;
class moveNodeCache {
public:
	moveNodeCache();
	~moveNodeCache();

	bool push(PCWSTR pkeyName, POINT pt);
	bool pop(std::wstring &keyName, POINT &pt);
	bool exists(PCWSTR pkeyName);
	bool isEmpty() const;
	std::size_t size() const;
protected:
	CRITICAL_SECTION			m_csLock;	// 数据保护锁
	std::deque<BSNodeMoveInfo>	m_queCache;	// 数据队列
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	monitorDirCache	2014-12-24
//	windows目录(文件夹)状态监控数据缓存类
////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct BSMonitorData {
	LONG	lEvent;
	wchar_t	pFullPath1[MAX_PATH];
	wchar_t	pFullPath2[MAX_PATH];
	BSMonitorData()
		: lEvent(0)
	{
		pFullPath1[0] = pFullPath2[0] = L'\0';
	}
	BSMonitorData(LONG Event, PCWSTR pfullPath1, PCWSTR pfullPath2)
		: lEvent(Event)
	{
		if (pfullPath1)
			std::wcsncpy(pFullPath1, pfullPath1, _countof(pFullPath1));
		else
			pFullPath1[0] = L'\0';
		if (pfullPath2)
			std::wcsncpy(pFullPath2, pfullPath2, _countof(pFullPath2));
		else
			pFullPath2[0] = L'\0';
	}
	
}BSMonitorData;
class monitorDirCache {
public:
	monitorDirCache();
	~monitorDirCache();

	void push(LONG Event, PCWSTR pfullPath1, PCWSTR pfullPath2);
	bool pop(BSMonitorData &data);
	bool isEmpty() const;
	std::size_t size() const;
protected:
	CRITICAL_SECTION			m_csLock;	// 数据保护锁
	std::queue<BSMonitorData>	m_queCache;	// 数据队列
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	nodesCache类	2014-10-30
//	节点队列
////////////////////////////////////////////////////////////////////////////////////////////////////////
class nodesCache {
public:
	nodesCache();
	~nodesCache();

	bool push(std::wstring fullPath);
	// 得到并移除队列队尾中一个节点
	bool pop(std::wstring &fullPath);
	// 得到一个节队尾点，但不移除
	bool getNode(std::wstring &fullPath);
	// 如果队尾节点等于fullPath，就移除队尾节点
	bool popNode(const std::wstring &fullPath);
	bool isEmpty();
	bool exists(std::wstring fullPath);
	std::size_t size();
protected:
	CRITICAL_SECTION			m_csLock;	// 数据保护锁
	std::deque<std::wstring>	m_queCache;	// 数据队列
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	idlNode类	2014-04-30
//	桌面信息管理
////////////////////////////////////////////////////////////////////////////////////////////////////////
class idlNode {
public:
	// 文档或文件夹节点信息数组；key=名称，不能用索引，重新排序后索引值会发生变化， value=idlNode*
	typedef std::map<std::wstring, gtc::idlNode *>	IDINodeMap;
	typedef std::vector<idlNode *>	IDINodeArray;
public:
	typedef enum BENodeShowFlag {
		BENodeShowFlagNone		= 0,
		BENodeShowFlagHide		,	// 节点在容器外
		BENodeShowFlagShow		,		// 节点在容器内
		BENodeShowFlagCut		,		// 节点在容器边界上
	};
private:
	idlNode(){}
public:
	idlNode(std::wstring wsname, std::wstring wssize, std::wstring wstype, std::wstring wsmodifydt);
//public:
//	idlNode(PCWSTR pname, int32_t idx, const SIZE &sz);
//	idlNode(PCWSTR pname, PCWSTR ptypeName, PCWSTR pfullpath, SFGAOF attributs);
//	idlNode(PCWSTR pname, PCWSTR ptypeName, PCWSTR pfullpath, SFGAOF attributs, LPITEMIDLIST pidlst);
	virtual ~idlNode();
public:
	// free
	/*****************************************************************************************
		<< --- static kpdeskAdmin::free		2014-06-03 --- >>
		说明：是否节点信息
		参数：
		vecdata		= 被释放的节点数据集合
		返回值：
	*****************************************************************************************/
	static void free(IDINodeMap &mapdata);
	static void free(IDINodeArray &arrdata);
	/****************************************************************************************
		<< --- createNodeIIDL		2014-10-28 --- >>         
		说明：从桌面查找指定全路径节点的IIDLS*
		参数：
			wsFullPath = 被查找节点全路径
		返回值：节点IIDLS指针，节点不存在返回空。〈〈〈需要调用者释放〉〉〉
	*****************************************************************************************/
	static LPITEMIDLIST createNodeIIDL(const std::wstring &wsFullPath);
	/****************************************************************************************
		<< --- createNodeIIDL		2014-10-31 --- >>         
		说明：根据节点属性（显示名、类型、大小、修改日期）从shell中查找节点
		参数：
			displayName	= 被查找节点显示名
			lvType		= 被查找节点类型
			lvSize		= 被查找节点大小
			lvModifyDT	= 被查找节点修改日期
		返回值：节点IIDLS指针，节点不存在返回空。〈〈〈需要调用者释放〉〉〉
	*****************************************************************************************/
	static LPITEMIDLIST createNodeIIDL(const std::wstring &displayName, const std::wstring &lvType, const std::wstring &lvSize, const std::wstring &lvModifyDT);

	/****************************************************************************************
		<< --- inPublicDesktop		2014-11-27 --- >>         
		说明：查看指定显示名和类型的节点是否在公共桌面内
		参数：
			displayName	= 被查找节点显示名
			lvType		= 被查找节点类型
		返回值：是否在公共桌面内
	*****************************************************************************************/
	static bool inPublicDesktop(const std::wstring &displayName, const std::wstring &lvType);
public:
	// 
	// 节点类型
	BEDeskGroupFlag getGroupType();
	// 小图标索引
	int32_t getSmallIcon();
	// 大图标索引
	int32_t getLagerIcon();
	// 选中小图标索引
	int32_t getSmallOpenIcon();
	// 选中大图标索引
	int32_t getLagerOpenIcon();
	HICON getIcon();
	std::wstring getTargetPath();
	std::wstring getTargetType();
	//void copyIIDList(LPITEMIDLIST piidl);
	/****************************************************************************************
		<< --- update		2014-10-27 --- >>         
		说明：根据节点ITEMIDLIST值，更新其属性：大小、修改日期
		参数：
		返回值：是否执行成功
	*****************************************************************************************/
	bool update(LPITEMIDLIST piidl);
	bool update(const idlNode *pNode);
private:
	bool isSystemGroup(const std::wstring &wsLVType, const std::wstring &wsTargetType);
	bool isFolderGroup(const std::wstring &wsLVType, const std::wstring &wsTargetType);
	bool isSoftGroup(const std::wstring &wsLVType, const std::wstring &wsTargetType);
	bool isCompressGroup(const std::wstring &wsLVType, const std::wstring &wsTargetType);
	bool isDocumentGroup(const std::wstring &wsLVType, const std::wstring &wsTargetType);
	bool isPictureGroup(const std::wstring &wsLVType, const std::wstring &wsTargetType);
private:
	//static std::wstring readShortcut(PCWSTR pLnk);
	static bool readShortcutFile(PCWSTR pLnk, std::wstring &wsTargetPath);
	static BEDeskGroupFlag getGroupFlagByExtname(const std::wstring &fullpath);
public:
	// 根据LPITEMIDLIST创建一个节点
	static idlNode* createNode(LPITEMIDLIST pidl);
	// 得到key名
	static bool parseKeyName(LPITEMIDLIST pidl, std::wstring &wsKeyName);
	// 解析文件类型
	static bool parseFileType(PCWSTR pfullPath, std::wstring &wsFileType);
	//// 更新节点信息
	//static void updateNode(LPITEMIDLIST pidl, idlNode *pNode);
	// 是否为临时文件（其会自动删除）
	static bool isTemporaryFile(LPITEMIDLIST pidl);
	static bool isTemporaryFile(PCWSTR pfullPath);
	static bool isTemporaryFile(std::wstring fullPath);
	// 全路径文件（或系统节点 ::{GUID}）是否在shell中
	static bool exists(std::wstring fullPath);
	// 枚举文件类型信息及其对应扩展名
	static void enumFileTypeExtensionInfo();
	// 仅枚举桌面上文件类型信息及其对应扩展名
	static void enumDeskFileTypeExtInfo();
	static void addFileTypeExtInfo(std::wstring &wsType, std::wstring &wsExt);
	// 得到枚举的文件类型-扩展名集合
	static std::multimap<std::wstring, std::wstring>& getFileTypeExtensionInfo();
private:
	// key=文件类型（如：文本文件） value=扩展名（如：.txt）。注意主键可以重复哦，如：.txt、.wtx都叫“文本文当”
	static std::multimap<std::wstring, std::wstring> mc_mapFileTypeExtension;	
	
public:
	std::wstring getKeyName() const;
	const std::wstring& getName() const {			return m_wsName;								}
	void setName(PCWSTR pName) {					m_wsName = pName ? pName : L"";					}
	const std::wstring& getFileName() const {				return m_wsFileName;							}
	void setFileName(PCWSTR pfilename) {			m_wsFileName = pfilename ?pfilename :L"";		}
	int getIndex() {								return m_iIndex;								}
	void setIndex(int idx) {						m_iIndex = idx;									}
	const std::wstring& getFullPath() const {		return m_wsFullPath;							}
	void setFullPath(PCWSTR pfullpath) {			m_wsFullPath = pfullpath?pfullpath:L"";			}
	const std::wstring& getDirectory() const {		return m_wsDirectory;							}
	void setDirectory(PCWSTR pdir) {				m_wsDirectory = pdir ?pdir :L"";				}
	const std::wstring& getLVSize() const {			return m_wsLVSize;								}
	void setLVSize(PCWSTR psize) {					m_wsLVSize = psize ?psize :L"";					}
	const std::wstring& getLVType() const {			return m_wsLVTypeName;							}
	void setLVTypeName(PCWSTR ptype) {				m_wsLVTypeName = ptype?ptype:L"";				}
	const std::wstring& getLVModifyDT() const {		return m_wsLVModifyDT;							}
	void setLVModifyDT(PCWSTR pmodifydt) {			m_wsLVModifyDT = pmodifydt?pmodifydt:L"";		}

	SFGAOF getAttributes() const {					return m_ulAttributes;							}
	void setAttributes(DWORD attributes) {			m_ulAttributes = attributes;					}
	bool isHide() const {							return m_beShow == BENodeShowFlagHide;			}
	bool isCut() const {							return m_beShow == BENodeShowFlagCut;			}
	bool isShow() const {							return m_beShow == BENodeShowFlagShow;			}
	BENodeShowFlag getShowFlag() const {			return m_beShow;								}
	void setShowFlag(BENodeShowFlag showFlag) {		m_beShow = showFlag;							}
	bool isTemporary() const {						return m_bTemporary;							}
	void setTemporary(bool temp = true) {			m_bTemporary = temp;							}
	SIZE getSizeBox() const {						return m_szNode;								}
	void setSizeBox(const SIZE &sz) {				m_szNode = sz;									}
	std::size_t getFileSize() const {				return m_uSize;									}
	void setFileSize(std::size_t filesize) {		m_uSize = filesize;								}
	DATE getCreatedTime() const {					return m_dtCreated;								}
	void setCreatedTime(DATE dtCreated) {			m_dtCreated = dtCreated;						}
	int32_t getOptFlag() const {					return m_iOptFlag;								}
	void setOptFlag(int32_t flag) {					m_iOptFlag = flag;								}
	//LPITEMIDLIST getIIDL() const {					return m_pIDLST;								}
	int32_t getPOSIndex() const {					return m_iPOSIndex;								}
	void setPOSIndex(int32_t posIndex) {			m_iPOSIndex = posIndex;							}
	std::wstring toString() {
		std::wostringstream out;
		out << L"类型：" << ::toString(getGroupType()) 
			<< L"\t属性：0x" << std::hex << std::uppercase << m_ulAttributes << std::dec << std::nouppercase
			<< L"\t标记：" << (m_beShow == BENodeShowFlagHide ? L"hide" : (m_beShow== BENodeShowFlagCut ?L"cut" : L"show")) << L" " << (m_bTemporary ? L"temp" : L"" )
			<< L"\tLV类型：" << getLVType() << L"  LV大小：" << getLVSize() << L"  LV日期：" << getLVModifyDT()
			<< L"\t节点：" << getIndex() << L"(" << m_szNode.cx << L"," << m_szNode.cy << L")"
			//<< L"\t类型名：" << getTypeName() << L"=" << getLVType()
			<< L"\t显示名：" << getName()
			<< L"\tKey名：<" << getKeyName() << L">"
			//<< L"\tLV属性：" << m_wsLVTypeName << L" " << m_uSize << L" " << m_wsLVCreated
			//<< L"\t全路径：" << getFullPath() 
			//<< L"\t目标类型：" << getTargetType()
			//<< L"\t目标路径：" << getTargetPath()
			<< std::ends;
		return out.str();
	}
private:
	BEDeskGroupFlag	m_beGroupFlag;		// 
	//LPITEMIDLIST	m_pIDLST;			// 重枚举的节点复制而来

	std::wstring	m_wsName;			// 显示名(可能不带扩展名)
	std::wstring	m_wsFileName;		// 带扩展名的文件名，
	std::wstring	m_wsLVSize;			// 8
	std::wstring	m_wsLVTypeName;		// 根据桌面SysListView32得到的节点类型	// 
	std::wstring	m_wsLVModifyDT;		// 8

	SFGAOF			m_ulAttributes;		// 属性标记，按位bit区分
	//DWORD			m_dwFileAttributes;	// 文件属性
	int32_t			m_iIndex;			// 节点在SysListView中的索引，注意：当重新排序图标时索引会改变
	//std::wstring	m_wsTypeName;		// 类型名
	std::wstring	m_wsFullPath;		// 全路径名
	std::wstring	m_wsTargetPath;		// 快捷键对应的目标文件路径
	std::wstring	m_wsTargetType;		// 目标文件类型名
	std::wstring	m_wsDirectory;		// 节点所属目录（如：桌面、公共桌面等）
	int32_t			m_iSmallIcon;		// 小图标索引16*16
	int32_t			m_iLagerIcon;		// 大图标索引
	int32_t			m_iSmallOpenIcon;	// 选中小图标索引
	int32_t			m_iLagerOpenIcon;	// 选中大图标索引
	HICON			m_hIcon;			// 文件图标句柄

	std::size_t		m_uSize;			// 文件大小		// 可以通过SysListView32得到
	DATE			m_dtCreated;		// 被创建时的时间	
	//std::wstring	m_wsLVCreated;		// 被创建时的时间	// 可以通过SysListView32得到
	std::time_t		m_dtLastModify;		// 最后被修改时的时间

	BENodeShowFlag	m_beShow;			// 图标是否需要隐藏（如：图标不在窗口的可视区就需要隐藏），隐藏的图标被移到桌面外
	bool			m_bTemporary;		// 图标时临时的（如：“我的电脑”），系统退出时要删除

	int32_t			m_iPOSIndex;		// 所在位置索引（组窗口内：从左到右、从上到下。桌面上：从上到下、从左到右）
	int32_t			m_iOptFlag;			// 操作标记
	SIZE			m_szNode;			// 节点的尺寸
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	kpdeskAdmin类	2014-04-30
//	桌面信息管理
////////////////////////////////////////////////////////////////////////////////////////////////////////
class kpdeskAdmin {
private:
	kpdeskAdmin(const kpdeskAdmin &admin);
	kpdeskAdmin& operator =(const kpdeskAdmin &admin);
public:
	kpdeskAdmin();
	virtual ~kpdeskAdmin();
public:
	/*****************************************************************************************
		<< --- kpdeskAdmin::open		2014-05-30 --- >>
		说明：得到桌面接口指针，内存分配指针
		参数：
			hLVWnd	= 桌面SysListView32窗口句柄
		返回值：是否执行成功
	*****************************************************************************************/
	bool open(HWND hLVWnd);
	bool isOpen() const;
	/*****************************************************************************************
		<< --- kpdeskAdmin::close		2014-05-30 --- >>
		说明：是否open分配的指针
		参数：
		返回值：
	*****************************************************************************************/
	void close();

	/*****************************************************************************************
		<< --- kpdeskAdmin::enumNodes		2014-05-30 --- >>
		说明：枚举桌面节点
		参数：
		返回值：
			>= 0	成功，节点数
			< 0		失败
	*****************************************************************************************/
	int enumNodes();

	/****************************************************************************************
		<< --- size		2014-11-6 --- >>         
		说明：返回枚举的桌面节点数量
		参数：
		返回值：>=0
	*****************************************************************************************/
	int size() const;

	/*****************************************************************************************
		<< --- kpdeskAdmin::freeNodes		2014-05-30 --- >>
		说明：释放并清空节点
		参数：
		返回值：
	*****************************************************************************************/
	void free();
	/****************************************************************************************
		<< --- freeNone		2014-11-6 --- >>         
		说明：释放索引为-1的节点
		参数：
		返回值：
	*****************************************************************************************/
	void freeNone();

	/****************************************************************************************
		<< --- clearOptFlag		2014-11-6 --- >>         
		说明：清除桌面节点的索引
		参数：
		返回值：
	*****************************************************************************************/
	void clearOptFlag();

	/****************************************************************************************
		<< --- getNodeKeyNames		2014-12-8 --- >>         
		说明：得到属于指定组标记的节点集合
		参数：
			groupFlag	= 被查找节点的组标记
		返回值：指定组标记节点集合
	*****************************************************************************************/
	std::vector<std::wstring> getNodeKeyNames(BEDeskGroupFlag groupFlag);
public:
	/****************************************************************************************
		<< --- existsNode		2014-11-3 --- >>         
		说明：查验指定节点是否存在，如果存在返回节点索引
		参数：
			keyName	= 被查验的节点名称
		返回值：节点指针，不存在返回空
	*****************************************************************************************/
	idlNode* existsNode(const std::wstring &keyName);
	idlNode* existsNodeSEH(const std::wstring &keyName);
	/****************************************************************************************
		<< --- existsNode		2014-11-21 --- >>         
		说明：查找节点索引对应对象
		参数：
			nodeIndex	= 被查找节点索引
		返回值：节点指针，不存在返回空
	*****************************************************************************************/
	idlNode* existsNode(const int nodeIndex);
	idlNode* existsNodeSEH(const int nodeIndex);

	/****************************************************************************************
		<< --- findNode		2014-11-3 --- >>         
		说明：根据节点key名称查找节点对象
		参数：
			keyName	= 被查验的节点
		返回值：节点指针，不存在返回NULL
	*****************************************************************************************/
	gtc::idlNode* findNode(const std::wstring &keyName, PCWSTR pDesc);
	/****************************************************************************************
		<< --- findNode		2014-11-3 --- >>         
		说明：根据节点索引查找节点对象
		参数：
			nodeIndex	= 被查验的节点索引
		返回值：节点指针，不存在返回NULL
		= -1，无法锁保护
		= 0，失败
		= 1，成功
	*****************************************************************************************/
	int findNode(const int nodeIndex, gtc::idlNode *&pNode);

	/****************************************************************************************
		<< --- addNode		2014-11-3 --- >>         
		说明：添加节点
		参数：
			keyName		= 被删除节点key名称
			prefNode	= 返回新增节点指针
		返回值：
		= -1，无法锁保护
		= 0，失败
		= 1，成功
		= 2，节点已经存在
	*****************************************************************************************/
	int addNode(const std::wstring &keyName, idlNode *& prefNode);

	/****************************************************************************************
		<< --- deleteNode		2014-11-3 --- >>         
		说明：删除节点
		参数：
			keyName	= 被删除节点key名称
		返回值：
			= -1，无法锁保护
			= 0，失败
			= 1，成功
	*****************************************************************************************/
	int deleteNode(const std::wstring &keyName);

	/****************************************************************************************
		<< --- renameNode		2014-11-3 --- >>         
		说明：重命名节点
		参数：
			oldKeyName	= 原节点key名称
			newKeyName	= 重命名后节点的key名称
		返回值：
	*****************************************************************************************/
	bool renameNode(std::wstring oldKeyName, std::wstring newKeyName);

	/****************************************************************************************
		<< --- updateNode		2014-11-17 --- >>         
		说明：更新节点信息及索引
		参数：
			keyName	= 被更新节点key名称
		返回值：
			= -1，无法锁保护
			= 0，失败
			= 1，成功
			= 2，节点不存在（可能为临时文件或还未添加）
	*****************************************************************************************/
	int updateNode(std::wstring keyName);
	//bool updateNode(std::wstring keyName, std::wstring tmpKeyName);
	/****************************************************************************************
		<< --- getNodeIIDLs		2014-11-3 --- >>         
		说明：根据桌面节点索引在shell查找节点对应的ITEMIDLIST*
		参数：
			nodeIndex = 桌面节点索引
		返回值：节点ITEMIDLIST指针，失败返回NULL〈〈〈需要调用者释放〉〉〉
	*****************************************************************************************/
	LPITEMIDLIST getNodeIIDLs(int nodeIndex);

	/****************************************************************************************
		<< --- refreshNodeOffset		2014-11-3 --- >>         
		说明：移动节点时要追加的偏移量；当开启、切换图标大小时要刷新偏移量
		参数：
		返回值：
	*****************************************************************************************/
	void refreshNodeOffset();

	/****************************************************************************************
		<< --- refreshNodeMaxSize		2014-11-3 --- >>         
		说明：重新得到节点的最大尺寸
		参数：
		返回值：
	*****************************************************************************************/
	void refreshNodeMaxSize();

	/****************************************************************************************
		<< --- findNodeIndex		2014-11-3 --- >>         
		说明：无锁保护。查找指定节点对象对应桌面节点索引
		参数：
			pNode	= 被查找对象
		返回值：桌面节点索引，节点不存在返回-1
	*****************************************************************************************/
	int findNodeIndex(const gtc::idlNode *pNode);

	/****************************************************************************************
		<< --- isEqualNode		2014-11-3 --- >>         
		说明：无锁保护。查验桌面上节点与shell中的节点是否一致
		参数：
			nodeIndex	= 被比较节点，来至桌面节点索引
			pNode		= 被比较节点，来至shell枚举的节点对象
		返回值：是否一致
	*****************************************************************************************/
	bool isEqualNode(int nodeIndex, const gtc::idlNode *pNode);
	bool isEqualNode(const std::wstring &wsName, const std::wstring &wsSize, const std::wstring &wsType, const std::wstring &wsDT, const gtc::idlNode *pNode);

	/****************************************************************************************
		<< --- setNodeListViewInfo		2014-11-3 --- >>         
		说明：无锁保护。根据桌面节点属性更新节点对象
		参数：
			pNode		= 被更新对象
			nodeIndex	= 桌面源节点索引
			ptpos		= 不为空时，保存节点位置坐标
		返回值：
	*****************************************************************************************/
	void setNodeListViewInfo(gtc::idlNode *pNode, int nodeIndex, POINT *ptpos = NULL);
	/****************************************************************************************
		<< --- moveto		2014-11-3 --- >>         
		说明：移动节点
		参数：
			keyName	= 被移动节点key名称
			pt		= 目标位置
		返回值：
			= -1，无法锁保护
			= 0，失败
			= 1，成功
			= 2，节点不存在（可能为临时文件或还未添加）
	*****************************************************************************************/
	int moveto(const std::wstring &keyName, POINT pt);
	int movetoSEH(const std::wstring &keyName, POINT pt);
	/****************************************************************************************
		<< --- refreshAllNode		2014-11-19 --- >>         
		说明：刷新所有节点
		参数：
		返回值：
	*****************************************************************************************/
	bool refreshAllNode();

	/****************************************************************************************
		<< --- refreshNodeSize		2014-11-3 --- >>         
		说明：刷新所有节点的尺寸
		参数：
		返回值：
	*****************************************************************************************/
	void refreshAllNodeSize();

	/****************************************************************************************
		<< --- redrawNode		2014-11-3 --- >>         
		说明：重新绘制节点
		参数：
			nodeIndex	= 被绘制节点索引
		返回值：
	*****************************************************************************************/
	void redrawNode(int nodeIndex);

	/****************************************************************************************
		<< --- refreshHideNode		2014-11-3 --- >>         
		说明：重新移动所有隐藏的节点
		参数：
		返回值：
	*****************************************************************************************/
	void refreshHideNode();
	/****************************************************************************************
		<< --- redrawCutNode		2014-11-3 --- >>         
		说明：重新绘制cut节点
		参数：
		返回值：
	*****************************************************************************************/
	void redrawCutNode();

	/****************************************************************************************
		<< --- restoreNodePosition		2014-11-3 --- >>         
		说明：恢复桌面节点到原来位置
		参数：
		返回值：
	*****************************************************************************************/
	void restoreNodePosition();
	///****************************************************************************************
	//	<< --- decrementNodeIndex		2014-11-3 --- >>         
	//	说明：
	//	参数：
	//	返回值：
	//*****************************************************************************************/
	//void decrementNodeIndex(std::wstring keyName);
public:
	const gtc::idlNode::IDINodeMap& getNodes() const;
	SIZE getNodeMaxSize() const;
	SIZE getNodeOffset() const;
private:
	/****************************************************************************************
		<< --- saveNodePosition		2014-11-3 --- >>         
		说明：保存节点原有位置，并根据节点桌面索引更新节点对象
		参数：
		返回值：
	*****************************************************************************************/
	void saveNodePosition();
	/****************************************************************************************
		<< --- makeNode		2014-12-18 --- >>         
		说明：将节点存入集合中
		参数：
		返回值：
	*****************************************************************************************/
	void makeNode(idlNode *pNode);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	仅仅为了SEH	2015-1-7
	idlNode* wrapExistsNode(int nodeIndex);
	idlNode* wrapExistsNode(const std::wstring &keyName);
	int wrapMoveto(const std::wstring &keyName, POINT pt);
protected:
	gtc::idlNode::IDINodeMap	m_mapNodes;			// 桌面节点信息
	StringMapType			m_mapSystemNodes;	// 系统节点集合。key=显示名，如：控制面板；value=::{CLSID}
	CRITICAL_SECTION			m_csLockNodes;		// m_mapNodes保护锁
	
	SIZE						m_szMaxNode;		// 节点的最大尺寸
	SIZE						m_szOffset;			// 移动节点时加上的偏移量（当节点移到0，0时其坐标并不为0，0；？？？）

	HWND						m_hDeskLV;			// 桌面SysListView32窗口句柄
	//IShellFolder2				*m_pshFolder;		// 
};


}
#endif
