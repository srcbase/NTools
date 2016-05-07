#ifndef __FILE_H__
#define __FILE_H__
#pragma once
#include "sysupport.h"

namespace gtc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	file类	2014-04-25
//	常用文件操作类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class file
{
public:
	// 当目标发现重名时，且操作标记包含FOF_RENAMEONCOLLISION时触发本回调函数以出来新旧文件名
	typedef bool (CALLBACK *ENUMFILEMAPPROC)(LPSHNAMEMAPPING, LPVOID);
private:
	struct HANDLETOMAPPINGS 
	{
		UINT              uNumberOfMappings;  // Number of mappings in the array.
		LPSHNAMEMAPPING   lpSHNameMapping;    // Pointer to the array of mappings.
	};
	// 文件操作时目标有重名，且操作标记包含FOF_RENAMEONCOLLISION时调用本函数
	static int WINAPI _EnumFileMapping(HANDLE hNameMappings, ENUMFILEMAPPROC lpfnFileMapProc, LPVOID pData);
public:
	/*****************************************************************************************
		<< --- static app::message		2014-04-25 --- >>
		说明：浏览选择文件
		参数：
		vpinfo		= 要显示的消息
		vptitle		= 消息框的标题
		vtype		= 消息框中命令按钮类型
		viswriter	= 是否将消息写入日志文件
		返回值：选中的命令按钮标记
	*****************************************************************************************/
	static std::wstring getAppPath(void)
	{
		wchar_t str[MAX_PATH];
		std::wstring path;
#if defined(_UNICODE)
		path = _wpgmptr;
#else
		path = convert::toWChar(_pgmptr);
#endif
		uint32_t len = 0;
		// 得到系统参数
	#if defined(_WIN32)
		len = ::GetModuleFileNameW(nullptr, str, MAX_PATH);
		str[len] = '\0';
	#endif
		path = str;
		path = path.substr(0, path.rfind('\\'));//.Left(path.ReverseFind('\\'));
		return path;
	}
	/*****************************************************************************************
		<< --- static file::removeLog		2014-04-30 --- >>
		说明：删除当前的日志文件
		参数：
		pfullfile	= 文件全路径
		返回值：是否执行成功
	*****************************************************************************************/
	bool remove(const wchar_t *pfullfile)
	{
		return std::remove(convert::toChar(pfullfile).c_str()) == 0;
	}
	/*****************************************************************************************
		<< --- static file::removeLog		2014-04-30 --- >>
		说明：删除当前的日志文件
		参数：
		返回值：是否执行成功
	*****************************************************************************************/
	bool removeLog(const wchar_t *pnameflag = L"app")
	{
		std::wstring path(getAppPath());
		char stimer[40] = {0};
		char logname[40] = {0};

		std::time_t tm = {0};
		std::time(&tm);
		std::tm *ptminfo = std::localtime(&tm);
		assert(ptminfo);
		std::strftime(stimer, sizeof(stimer), "%Y-%m-%d %H:%M:%S", ptminfo);
		std::strftime(logname, sizeof(logname), "%Y%m%d.ino", ptminfo);
		path += L"\\";
		path += _NDEF_DIR_GTDATA;
		path += L"\\";
		path += _NDEF_DIR_GTLOG;
		path += L"\\";
		path += pnameflag;
		path += convert::toWChar(logname);

		return remove(path.c_str());
	}
	/*****************************************************************************************
		<< --- static file::size		2014-04-25 --- >>
		说明：读取文件的尺寸（单位：字节）
		参数：
		pfullfile	= 数据文件全路径
		返回值：
			>= 0：文件长度（单位：字节）
			< 0	：失败
	*****************************************************************************************/
	static int64_t size(const std::wstring &fileName)
	{
		return size(fileName.c_str());
	}
	static int64_t size(const wchar_t *pfullfile)
	{
		int64_t len = -1;
		std::ifstream file(pfullfile, std::ifstream::in | std::ifstream::_Nocreate);
		if(file)
		{
			file.seekg(0, file.end);
			len = file.tellg();
			file.close();
		}
		return len;
	}

	/*****************************************************************************************
		<< --- static file::read		2014-04-25 --- >>
		说明：将文件数据以二进制形式保存到字节数组verdata中
		参数：
		pfullfile	= 数据文件全路径
		vecdata		= 保存数据
		返回值：是否执行成功
	*****************************************************************************************/
	static bool read(const wchar_t *pfullfile, SBDataType &vecdata)
	{
		int64_t len = size(pfullfile);
		std::ifstream file(pfullfile, std::ifstream::in | std::ifstream::binary);
		if(file)
		{
			if(len > 0)
			{
				vecdata.resize((uint32_t)len);
				file.read((char*)vecdata.data(), len);
			}
			file.close();
			return true;
		}
		return false;

	}
	static bool read(const wchar_t *pfullfile, std::string &strData)
	{
		if(!::PathFileExists(pfullfile)) return false;
		int64_t len = size(pfullfile);
		std::ifstream file(pfullfile);
		if (file)
		{
			if(len > 0)
			{
				strData.resize((uint32_t)len);
				file.read((char*)strData.data(), len);
			}

		}
		return len > 0;
	}
	/*****************************************************************************************
		<< --- static file::write		2014-04-25 --- >>
		说明：将数据以二进制形式写入文件
		参数：
		pfullfile	= 数据文件全路径
		vecdata		= 被保存数据
		isAppend	= true:将数据追加到文件尾部；false:覆盖原文件，如果文件不存在就创建新文件
		返回值：是否执行成功
	*****************************************************************************************/
	static bool write(const wchar_t *pfullfile, const char *pData, std::size_t dataLen, bool isAppend = false)
	{
		assert(pfullfile);
		assert(pData);
		std::ofstream::openmode mode = isAppend ?std::ofstream::app :std::ofstream::trunc;
		std::ofstream file(pfullfile, mode | std::ifstream::binary | std::ifstream::in);
		if(file)
		{
			file.write(pData, dataLen);
			file.close();
			return true;
		}
		return false;

	}
	static bool write(const wchar_t *pfullfile, const std::vector<uint8_t> &vecdata, bool isAppend = false)
	{	
		return write(pfullfile, (const char *)vecdata.data(), vecdata.size(), isAppend);
		//std::ofstream::openmode mode = isAppend ?std::ofstream::app :std::ofstream::trunc;
		//std::ofstream file(pfullfile, mode | std::ifstream::binary | std::ifstream::in);
		//if(file)
		//{
		//	file.write((const char *)vecdata.data(), vecdata.size());
		//	file.close();
		//	return true;
		//}
		//return false;
	}
	/*****************************************************************************************
		<< --- static file::writeLine		2014-04-25 --- >>
		说明：往文件中新增一行数据
		参数：
		pfullfile	= 数据文件全路径
		pdata		= 被写入的字符串
		返回值：是否执行成功
	*****************************************************************************************/
	static bool writeLine(const wchar_t *pfullfile, const wchar_t *pdata)
	{
		std::wofstream file(pfullfile, std::wofstream::app | std::wofstream::out);
		if(file)
		{
			// 支持中文输出
			file.imbue(std::locale("", std::locale::all ^ std::locale::numeric));
			file << pdata << std::endl;
			file.close();
			return true;
		}
		return false;
	}
	/*****************************************************************************************
		<< --- static file::existsDirectory		2014-04-25 --- >>
		说明：查验目录或文件是否存在
		参数：
		vpdirectory	= 被查验的目录或文件
		返回值：
			true	：目录存在
			false	：不存在
	*****************************************************************************************/
	static bool exists(const wchar_t *vpdirOrFile)
	{
		assert(vpdirOrFile);
		return ::PathFileExists(vpdirOrFile) == TRUE;
	//	bool bl = false;
	//#if defined(_WIN32)
	//	HANDLE hfind = NULL;
	//	WIN32_FIND_DATAW data = {0};
	//	try
	//	{
	//		hfind = ::FindFirstFileW(vpdirectory, &data);
	//		if(hfind == INVALID_HANDLE_VALUE || hfind == (HANDLE)(LONG)INVALID_FILE_SIZE)
	//			bl = false;
	//		else
	//			bl = true;
	//	}
	//	catch(...)
	//	{
	//		bl =false;
	//	}
	//	if(hfind != NULL)
	//		FindClose(hfind);
	//#endif
	//	return bl;
	}
	/****************************************************************************************
		<< --- fileAttributeInfo		2014-11-10 --- >>         
		说明：得到文件或文件夹属性信息
		参数：
			pFullPath		= 文件（或文件夹）全路径
			fileAttribute	= 被填充的文件属性信息
		返回值：是否执行成功
	*****************************************************************************************/
	static bool fileAttributeInfo(PCWSTR pFullPath, BSFileAttribute &fileAttribute);
	/*****************************************************************************************
		<< --- static file::browseFile		2014-04-25 --- >>
		说明：浏览选择文件
		参数：
		vpfilter	= 所浏览文件过滤器
		pname		= 不为空时，保存选中文件的文件名（不含扩展名）
		pext		= 不为空时，保存选中文件的扩展名
		返回值：全路径文件名
	*****************************************************************************************/
	static std::wstring browseFile(const wchar_t *vpfilter, std::wstring *pname = nullptr, std::wstring *pext = nullptr)
	{
		std::wstring file;
#if defined(_AFXDLL)
#if defined(_UNICODE)
		CFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST,vpfilter);
		if(dlg.DoModal() == IDOK)
		{
			if(pext)
				*pext = dlg.GetFileExt().GetBuffer();
			if(pname)
			{
				file = dlg.GetFileName().GetBuffer();
				*pname = file.substr(0, file.find('.'));
			}
			file = dlg.GetPathName().GetBuffer();
		}
#else
		CFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, convert::toChar(vpfilter).c_str());
		if(dlg.DoModal() == IDOK)
		{
			if(pext)
				*pext = convert::toWChar(dlg.GetFileExt().GetBuffer());
			if(pname)
			{
				file = convert::toWChar(dlg.GetFileName().GetBuffer());
				*pname = file.substr(0, file.find('.'));
			}
			file = convert::toWChar(dlg.GetPathName().GetBuffer());
		}
#endif
#endif
		return file;
	}

	/****************************************************************************************
		<< --- 	modifyFileNameBaseNumber	2015-2-26 --- >>         
		说明：根据序号修正文件名
		参数：
			wsFullPath	= 需要修正的文件全路径，不含扩展名
			pSuffix		= 修正名的后缀，一般为文件扩展名，如：.lnk
		返回值：修正后的文件夹全路径
	*****************************************************************************************/
	static std::wstring modifyFileNameBaseNumber(const std::wstring &wsFullPath, PCWSTR pSuffix = NULL);
	/****************************************************************************************
		<< --- 	updateFileTime	2015-2-27 --- >>         
		说明：更新文件时间
		参数：
			wsFullPath	= 被操作的文件
			timeFlag	= 需要更新哪部分时间
		返回值：是否执行成功
	*****************************************************************************************/
	static bool updateFileTime(const std::wstring &wsFullPath, UINT timeFlag = FILE_ATTRIBUTE_ALL_TIME);
	/****************************************************************************************
		<< --- 		2015-3-17 --- >>         
		说明：解析文件，提取文件名、扩展名、父路径
		参数：
			wsFullPath			= 被解析文件或目录的全路径
			wsName				= 被提取的文件名或文件夹
			poutExtName			= 不为空时，返回扩展名，如：.txt
			poutParentFolder	= 不为空时，返回文件或文件夹的父目录
		返回值：
	*****************************************************************************************/
	static bool analysisFilePath(std::wstring wsFullPath, std::wstring &wsName
		, std::wstring *poutExtName = NULL, std::wstring *poutParentFolder = NULL);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-18 打开模板文件。该文件存在于：	1)%ALLUSERSPROFILE%\Templates 2)%USERPROFILE%\Templates 3)%SystemRoot%\shellnew
	static HANDLE openTemplateFile(PCWSTR pfileName);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-16 创建并初始化FileOperation*接口
	static bool createAndInitializeFileOperation(REFIID riid, void **ppv);
	/****************************************************************************************
		<< --- 	createFolder	2015-3-16 --- >>         
		说明：在指定文件夹内创建新的文件夹。createFolderEx采用IFileOperation实现，xp系统不支持。
		参数：
			pRootName	= 父目录，即新文件夹的父目录
			pfolderName	= 新文件夹的名称
			nFlag		= 操作标记
			poutNew		= 不为空时，保存新建文件的IShellItem*
		返回值：返回新建文件全路径
	*****************************************************************************************/
	static std::wstring createFolder(PCWSTR pRootName, PCWSTR pfolderName, DWORD nFlag = FOF_RENAMEONCOLLISION);
	static bool createFolderEx(PCWSTR pRootName, PCWSTR pfolderName, DWORD nFlag = FOF_RENAMEONCOLLISION, IShellItem **poutNew = NULL);

	/****************************************************************************************
		<< --- 	createFile	2015-3-16 --- >>         
		说明：在指定文件夹内创建新的文件。createFileEx采用IFileOperation实现，xp系统不支持。
		参数：
			pRootName		= 父目录，即新文件夹的父目录
			pfileName		= 新文件的名称，如：newfile.txt
			pTemplateName	= 参考的模板文件，即：以此模板文件的内容创建新文件。
								该文件存在于：	1)%ALLUSERSPROFILE%\Templates
											2)%USERPROFILE%\Templates
											3)%SystemRoot%\shellnew
			nFlag			= 操作标记
			poutNew			= 不为空时，保存新建文件的IShellItem*
		返回值：是否创建成功
	*****************************************************************************************/
	static bool createFile(PCWSTR pRootName, PCWSTR pfileName, PCWSTR pTemplateName = NULL, DWORD nFlag = FOF_RENAMEONCOLLISION, IShellItem **poutNew = NULL);
	static bool createFileEx(PCWSTR pRootName, PCWSTR pfileName, PCWSTR pTemplateName = NULL, DWORD nFlag = FOF_RENAMEONCOLLISION, IShellItem **poutNew = NULL);

	/****************************************************************************************
		<< --- 	copyFile	2015-3-17 --- >>         
		说明：复制文件或文件夹。copyFileEx采用IFileOperation实现，xp系统不支持。
		参数：
			pToFolder	= 目标文件所属文件夹
			pName		= 目标文件或文件夹的名称
			pFrom		= 源文件或文件夹
			nFlag		= 操作标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool copyFile(PCWSTR pToFolder, PCWSTR pName, PCWSTR pFrom, DWORD nFlag = FOF_ALLOWUNDO);
	static bool copyFileEx(PCWSTR pToFolder, PCWSTR pName, PCWSTR pFrom, DWORD nFlag = FOF_ALLOWUNDO);

	/****************************************************************************************
		<< --- 	copyFiles	2015-3-17 --- >>         
		说明：一次复制多个文件。copyFilesEx采用IFileOperation实现，xp系统不支持。
		参数：
			pToFolder	= 目标文件夹
			vecFiles	= 被操作的文件夹或文件集合
			nFlag		= 操作标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool copyFiles(PCWSTR pToFolder, StringArrayType &vecFiles, DWORD nFlag = FOF_ALLOWUNDO, ENUMFILEMAPPROC plfnFileMapProc = NULL, LPVOID pData = NULL);
	static bool copyFilesEx(PCWSTR pToFolder, StringArrayType &vecFiles, DWORD nFlag = FOF_ALLOWUNDO);

	/****************************************************************************************
		<< --- 	moveFile	2015-3-17 --- >>         
		说明：移动文件。moveFileEx采用IFileOperation实现，xp系统不支持。
		参数：
			pToFolder	= 目标文件夹
			pFrom		= 被移动的文件或文件夹
			pNewName	= 不为空时，指定一个新文件名
			nFlag		= 操作标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool moveFile(PCWSTR pToFolder, PCWSTR pFrom, PCWSTR pNewName = NULL, DWORD nFlag = FOF_ALLOWUNDO);
	static bool moveFileEx(PCWSTR pToFolder, PCWSTR pFrom, PCWSTR pNewName = NULL, DWORD nFlag = FOF_ALLOWUNDO);

	/****************************************************************************************
		<< --- 	moveFiles	2015-3-17 --- >>         
		说明：一次移动多个文件。moveFilesEx采用IFileOperation实现，xp系统不支持。
		参数：
			pToFolder	= 目标文件夹
			vecFiles	= 被操作的文件夹或文件集合
			nFlag		= 操作标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool moveFiles(PCWSTR pToFolder, StringArrayType &vecFiles, DWORD nFlag = FOF_ALLOWUNDO);
	static bool moveFilesEx(PCWSTR pToFolder, StringArrayType &vecFiles, DWORD nFlag = FOF_ALLOWUNDO);

	/****************************************************************************************
		<< --- 	deleteFile	2015-3-17 --- >>         
		说明：删除指定文件或文件夹。deleteFileEx采用IFileOperation实现，xp系统不支持。
		参数：
			pFrom		= 被删除的文件或文件夹
			nFlag		= 操作标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool deleteFile(PCWSTR pFrom, DWORD nFlag = FOF_ALLOWUNDO|FOF_NOCONFIRMATION);
	static bool deleteFileEx(PCWSTR pFrom, DWORD nFlag = FOF_ALLOWUNDO|FOF_NOCONFIRMATION);

	/****************************************************************************************
		<< --- 	deleteFiles	2015-3-17 --- >>         
		说明：一次删除多个文件。deleteFilesEx采用IFileOperation实现，xp系统不支持。
		参数：
			vecFiles	= 被操作的文件夹或文件集合
			nFlag		= 操作标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool deleteFiles(StringArrayType &vecFiles, DWORD nFlag = FOF_ALLOWUNDO|FOF_NOCONFIRMATION);
	static bool deleteFilesEx(StringArrayType &vecFiles, DWORD nFlag = FOF_ALLOWUNDO|FOF_NOCONFIRMATION);

	/****************************************************************************************
		<< --- 	renameFile	2015-3-17 --- >>         
		说明：重命名文件或文件夹。renameFileEx采用IFileOperation实现，xp系统不支持。
		参数：
			pFrom		= 被重命名的文件或文件夹
			pNewName	= 不为空时，指定一个新文件名
			nFlag		= 操作标记；FOF_RENAMEONCOLLISION：如果目标文件存在，则自动序号文件名，如：xxx (2).txt
		返回值：是否执行成功
	*****************************************************************************************/
	static bool renameFile(PCWSTR pFrom, PCWSTR pNewName, DWORD nFlag = FOF_ALLOWUNDO);
	static bool renameFileEx(PCWSTR pFrom, PCWSTR pNewName, DWORD nFlag = FOF_ALLOWUNDO);

	/****************************************************************************************
		<< --- 	renameFiles	2015-3-17 --- >>         
		说明：一次重命名多个文件。renameFilesEx采用IFileOperation实现，xp系统不支持。
			执行后各文件变为newfile(1).txt, newfile(2).txt, and newfile(3).txt
		参数：
			vecFiles	= 被操作的文件夹或文件集合
			pNewName	= 新的名称
			nFlag		= 操作标记
		返回值：是否执行成功
	*****************************************************************************************/
	static bool renameFiles(StringArrayType &vecFiles, PCWSTR pNewName, DWORD nFlag = FOF_ALLOWUNDO);
	static bool renameFilesEx(StringArrayType &vecFiles, PCWSTR pNewName, DWORD nFlag = FOF_ALLOWUNDO);

	/****************************************************************************************
		<< --- 	createShortCut	2015-3-18 --- >>         
		说明：创建文件或文件夹的快捷方式
		参数：
			pToFolder		= 快捷键文件所属的文件夹
			pLnkName		= 快捷键文件名，如果有同名文件存在则自动修正文件名
			ptargetFilePath	= 快捷键文件所表示的目标文件或文件夹
			pParam			= 命令行变参数
			pIconFilePath	= 快捷键图标所在文件
			iconIndex		= 图标文件中的索引
		返回值：是否执行成功
	*****************************************************************************************/
	static bool createShortCut(PCWSTR pToFolder, PCWSTR pLnkName, PCWSTR ptargetFilePath
		, PCWSTR pParam = NULL, PCWSTR pIconFilePath = NULL, int iconIndex = 0);
	static bool createShortCut(PCWSTR pToFolder, PCWSTR ptargetFilePath);
	static bool createShortCuts(PCWSTR pToFolder, StringArrayType &vecTargetFiles);
	/****************************************************************************************
		<< --- 	undoDelete	2015-5-12 --- >>         
		说明：恢复被删除的文件
		参数：
			vecFiles		= 需要撤销删除的文件
			dtThisLaterUTC	= 此时间点以后被删除的文件才可被恢复
		返回值：是否执行成功
	*****************************************************************************************/
	static bool undoDelete(StringArrayType &vecFiles, DATE dtThisLaterUTC);
};

}
#endif	// end __FILE_H__