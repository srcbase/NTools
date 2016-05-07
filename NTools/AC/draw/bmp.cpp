#include "bmp.h"

namespace gtc{
bmpFile::bmpFile(void)
	: m_pbsQuad(NULL)
	, m_pbtData(NULL)
{
	memset(&m_bsFileHeader, 0, sizeof(m_bsFileHeader));
	memset(&m_bsInfoHeader, 0, sizeof(m_bsInfoHeader));
}


bmpFile::~bmpFile(void)
{
	if(m_pbsQuad) delete [] m_pbsQuad;
	if(m_pbtData) delete [] m_pbtData;
}

const BSBitmapInfoHeader& bmpFile::getBitmapInfo() const
{
	return m_bsInfoHeader;
}

bool bmpFile::read(std::wstring wsFullPath)
{
	assert(!wsFullPath.empty());
	std::FILE *pFile = NULL;
	pFile = std::fopen(gtc::convert::toChar(wsFullPath.c_str()).c_str(), "rb");
	if (!pFile) return false;
	// 读文件头
	int sz = sizeof(BSBitmapFileHeader);
	std::size_t dataLen = std::fread(&m_bsFileHeader, sizeof(BSBitmapFileHeader), 1, pFile);
	// 是否为bmp文件
	if (m_bsFileHeader.wType != 0x4d42)
	{	
		std::fclose(pFile);
		return false;
	}
	// 读信息头
	dataLen = std::fread(&m_bsInfoHeader, sizeof(BSBitmapInfoHeader), 1, pFile);
	if(dataLen != 1)
	{
		std::fclose(pFile);
		return false;
	}
	// 调色板 1\4\8
	if (m_bsInfoHeader.wBitCount < 16)
	{
		DWORD len = 1 << m_bsInfoHeader.wBitCount;
		m_pbsQuad = new BSRGBQuad[len];
		std::fseek(pFile, m_bsInfoHeader.dwSize-sizeof(BSBitmapFileHeader), SEEK_SET);
		dataLen = std::fread(m_pbsQuad, sizeof(BSRGBQuad), len, pFile);
	}

	std::fclose(pFile);
	return true;
}

bool bmpFile::create(std::wstring wsFullPath, LONG width, LONG height, COLORREF clrFill)
{	
	assert(wsFullPath.length() > 0 && width > 0 && height > 0);
	// 采用24位真彩
	memset(&m_bsFileHeader, 0, sizeof(m_bsFileHeader));
	memset(&m_bsInfoHeader, 0, sizeof(m_bsInfoHeader));
	// 文件头
	DWORD realWidth = (width * 24 + 31)/8;
	realWidth = realWidth/4*4;

	m_bsFileHeader.wType = 0x4d42;
	m_bsFileHeader.dwOffBits = sizeof(m_bsFileHeader) + sizeof(m_bsInfoHeader);
	m_bsFileHeader.dwSize = realWidth*height*3 + m_bsFileHeader.dwOffBits;
	// 信息头
	m_bsInfoHeader.dwSize = sizeof(BSBitmapInfoHeader);
	m_bsInfoHeader.lWidth = width;
	m_bsInfoHeader.lHeight = height;
	m_bsInfoHeader.wPlanes = 1;
	m_bsInfoHeader.wBitCount = 24;
	// 无颜色表
	// 数据
	m_pbtData = new BYTE[realWidth*height*3];
	assert(m_pbtData);
	for (DWORD idx = 0; idx < realWidth*height;)
	{
		m_pbtData[idx++] = GetRValue(clrFill);
		m_pbtData[idx++] = GetGValue(clrFill);
		m_pbtData[idx++] = GetBValue(clrFill);
	}
	// 写入文件
	std::FILE *pFile = std::fopen(gtc::convert::toChar(wsFullPath.c_str()).c_str(), "w");
	if(!pFile) return false;
	std::size_t wlen = std::fwrite(&m_bsFileHeader, sizeof(m_bsFileHeader), 1, pFile);
	wlen = std::fwrite(&m_bsInfoHeader, sizeof(m_bsInfoHeader), 1, pFile);
	wlen = std::fwrite(m_pbtData, sizeof(BYTE), realWidth*height*3, pFile);
	std::fclose(pFile);

	return true;
}

}