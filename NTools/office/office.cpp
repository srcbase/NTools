#include "office.h"

namespace office {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	excel类			2014-10-19
//	excel处理类描述
////////////////////////////////////////////////////////////////////////////////////////////////////////
excel::excel()
{
}
excel::~excel()
{

}
std::wstring excel::cellIndex(int32_t row, int32_t col)
{
	wchar_t cellidx[20] = {0};
	if(col / 25 > 0)
		std::swprintf(cellidx, 20, _T("%c%c%d"), 'A' + col/25-1, col % 25 + 'A', row+1);
	else
		std::swprintf(cellidx, 20, _T("%c%d"), 'A' + col, row+1);
	return cellidx;
}

}