#include "wucinc.h"


namespace wuc {


horizontalLayoutUC* layoutControl(verticalLayoutUC *pParent, SIZE szControl, BEItemPosition oriPos)
{
	assert(pParent);
	//assert(szControl.cx > 0 && szControl.cy > 0);
	horizontalLayoutUC *phor = nullptr, *pRet = nullptr;
	verticalLayoutUC *pver = nullptr;
	containerUC *pContainer = nullptr;
	if(!pParent/* || szControl.cx <= 0 || szControl.cy <= 0*/) return nullptr;

	switch (oriPos)
	{
	case BEItemPositionTopLeft:
		{
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);

			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);

			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 右空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// 下空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
		}
		break;
	case BEItemPositionTopCenter:
		{
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// 左空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// 中
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);
			// 
			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 右空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// 下空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
		}
		break;
	case BEItemPositionTopRight:
		{	
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// 左空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// obj
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);
			// 
			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 下空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
		}
		break;
	case BEItemPositionCenterLeft:
		{
			// 上空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
			// 中行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// obj
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);
			// 
			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 中行：右空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// 下空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);

		}
		break;
	case BEItemPositionCenterCenter:
		{
			// 上空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
			// 中行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// 中行：左空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// obj
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);
			// 
			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 中行：右空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// 下空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
		}
		break;
	case BEItemPositionCenterRight:
		{
			// 上空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
			// 中行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// 中行：左空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// obj
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);
			// 
			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 下空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
		}
		break;
	case BEItemPositionBottomLeft:
		{
			// 上空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
			// obj
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// 
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);
			
			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 右空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
		}
		break;
	case BEItemPositionBottomCenter:
		{
			// 上空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
			// 
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// 左空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// obj
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);

			pRet = new horizontalLayoutUC();
			pver->add(pRet);
			// 右空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// 右空列
		}
		break;
	case BEItemPositionBottomRight:
		{
			// 上空行
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			if(szControl.cy < 0) phor->setFixedHeight(-szControl.cy);
			// 
			phor = new horizontalLayoutUC();
			pParent->add(phor);
			pContainer = phor;
			if(szControl.cy > 0) phor->setFixedHeight(szControl.cy);
			// 左空列
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx < 0) pver->setFixedWidth(-szControl.cx);
			// obj
			pver = new verticalLayoutUC();
			pContainer->add(pver);
			if(szControl.cx > 0) pver->setFixedWidth(szControl.cx);
			// 
			pRet = new horizontalLayoutUC();
			pver->add(pRet);

		}
		break;
	default:
		assert(!L"还未实现！！！");
		break;
	}
	return pRet;
}
horizontalLayoutUC* layoutControl(horizontalLayoutUC *pParent, SIZE szControl, BEItemPosition oriPos)
{
	assert(pParent);
	verticalLayoutUC *pver = new verticalLayoutUC();
	pParent->add(pver);
	return layoutControl(pver, szControl, oriPos);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 2015-12-11		kp007	控件布局
horizontalLayoutUC* layoutCenterIcon(verticalLayoutUC *pParent, gtc::image *pImage)
{
	assert(pParent);
	assert(pImage);
	if(!pImage) return nullptr;
	SIZE szImage = pImage->getImageSize();

	horizontalLayoutUC *pRet = layoutControl(pParent, szImage, BEItemPositionCenterCenter);
	assert(pRet);
	if (pRet)
	{
		pRet->setImage(pImage);
		pRet->setBkImage(0, 0, szImage.cx, szImage.cy);

	}
	return pRet;
	//verticalLayoutUC *pRet = nullptr;
	//horizontalLayoutUC *phor = nullptr;
	//verticalLayoutUC *pver = nullptr;
	//containerUC *pContainer = nullptr;
	//// 上行
	//phor = new horizontalLayoutUC();
	//pParent->add(phor);
	//// 中行
	//phor = new horizontalLayoutUC();
	//pParent->add(phor);
	//phor->setFixedHeight(szImage.cy);
	//pContainer = phor;

	//// 中行 左列
	//pver = new verticalLayoutUC();
	//pContainer->add(pver);
	//// 中行 中列
	//pver = new verticalLayoutUC();
	//pContainer->add(pver);
	//pRet = pver;
	//pver->setFixedWidth(szImage.cx);
	//pver->setImage(pImage);
	//pver->setBkImage(0, 0, szImage.cx, szImage.cy);
	//// 中行 右列
	//pver = new verticalLayoutUC();
	//pContainer->add(pver);

	//// 下行
	//phor = new horizontalLayoutUC();
	//pParent->add(phor);

	//return pRet;
}
/*extern*/ horizontalLayoutUC* layoutCenterIcon(verticalLayoutUC *pParent, const wchar_t *pIconKey)
{
	assert(pParent);
	assert(pIconKey);
	gtc::image *pImage = drawManager::findImage(pIconKey);
	assert(pImage);
	if(!pImage) return nullptr;
	
	return layoutCenterIcon(pParent, pImage);
}
/*extern*/ horizontalLayoutUC* layoutCenterIcon(horizontalLayoutUC *pParent, const wchar_t *pIconKey)
{
	assert(pParent);
	assert(pIconKey);
	gtc::image *pImage = drawManager::findImage(pIconKey);
	assert(pImage);
	if(!pImage) return nullptr;

	verticalLayoutUC *pver = new verticalLayoutUC();
	pParent->add(pver);
	return layoutCenterIcon(pver, pImage);
}

HContainerArrayType layoutGrid(verticalLayoutUC *pParent, IntArrayType &vecRow, IntArrayType &vecCol)
{
	assert(pParent);
	assert(vecRow.size() > 0);
	assert(vecCol.size() > 0);
	HContainerArrayType vecItem;
	if(!pParent || vecRow.size() == 0 || vecCol.size() == 0) return vecItem;
	horizontalLayoutUC *phor = nullptr;
	verticalLayoutUC *pver = nullptr;
	containerUC *pRow = nullptr;
	for (std::size_t row = 0; row < vecRow.size(); ++row)
	{
		pRow = new horizontalLayoutUC();
		pParent->add(pRow);
		if(vecRow[row] > 0) pRow->setFixedHeight(vecRow[row]);

		for (std::size_t col = 0; col < vecCol.size(); ++col)
		{
			pver = new verticalLayoutUC();
			pRow->add(pver);
			if(vecCol[col] > 0) pver->setFixedWidth(vecCol[col]);

			phor = new horizontalLayoutUC();
			pver->add(phor);

			vecItem.push_back(phor);
			//if (row % 2)
			//{
			//	if (col % 2) phor->setBkColor(0xFEFF9999);
			//	else phor->setBkColor(0xFE66CC00);
			//}
			//else
			//{
			//	if (col % 2) phor->setBkColor(0xFE99CC66);
			//	else phor->setBkColor(0xFECC9999);
			//}

		}
	}
	return vecItem;
}
HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, IntArrayType &vecRow, IntArrayType &vecCol)
{
	assert(pParent);
	assert(vecRow.size() > 0);
	assert(vecCol.size() > 0);
	if(!pParent || vecRow.size() == 0 || vecCol.size() == 0) return HContainerArrayType();

	verticalLayoutUC *pver = new verticalLayoutUC();
	pParent->add(pver);

	return layoutGrid(pver, vecRow, vecCol);
}

HContainerArrayType layoutGrid(verticalLayoutUC *pParent, int rows, int cols, int rowCY, int colCX)
{
	assert(pParent);
	assert(rows > 0 && cols > 0);
	if(!pParent || rows == 0 || cols == 0) return HContainerArrayType();

	IntArrayType vecRow(rows, rowCY);
	IntArrayType vecCol(cols, colCX);
	
	return layoutGrid(pParent, vecRow, vecCol);
}
HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, int rows, int cols, int rowCY, int colCX)
{
	assert(pParent);
	assert(rows > 0 && cols > 0);
	if(!pParent || rows == 0 || cols == 0) return HContainerArrayType();

	verticalLayoutUC *pver = new verticalLayoutUC();
	pParent->add(pver);
	return layoutGrid(pver, rows, cols, rowCY, colCX);
}

HContainerArrayType layoutGrid(verticalLayoutUC *pParent, int rows, int cols, int rowOddCY, int rowEvenCY, int colOddCX, int colEvenCX)
{
	assert(pParent);
	assert(rows > 0 && cols > 0);
	if(!pParent || rows == 0 || cols == 0) return HContainerArrayType();
	IntArrayType vecRow;
	IntArrayType vecCol;
	for (int idx = 0; idx < rows; ++idx)
	{
		if(idx % 2 == 0) vecRow.push_back(rowOddCY);
		else vecRow.push_back(rowEvenCY);
	}
	for (int idx = 0; idx < cols; ++idx)
	{
		if(idx % 2 == 0) vecCol.push_back(colOddCX);
		else vecCol.push_back(colEvenCX);
	}

	return layoutGrid(pParent, vecRow, vecCol);

}

HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, int rows, int cols, int rowOddCY, int rowEvenCY, int colOddCX, int colEvenCX)
{
	assert(pParent);
	assert(rows > 0 && cols > 0);
	if(!pParent || rows == 0 || cols == 0) return HContainerArrayType();

	verticalLayoutUC *pver = new verticalLayoutUC();
	pParent->add(pver);
	return layoutGrid(pver, rows, cols, rowOddCY, rowEvenCY, colOddCX, colEvenCX);
}

HContainerArrayType layoutGrid(verticalLayoutUC *pParent, int rows, int rowCY, IntArrayType &vecCol)
{
	assert(pParent);
	assert(rows > 0 && vecCol.size() > 0);
	if (rows <= 0 || vecCol.size() == 0) return HContainerArrayType();
	IntArrayType vecRow(rows, rowCY);
	return layoutGrid(pParent, vecRow, vecCol);
}
HContainerArrayType layoutGrid(horizontalLayoutUC *pParent, int rows, int rowCY, IntArrayType &vecCol)
{
	assert(pParent);
	assert(rows > 0 && vecCol.size() > 0);
	if (rows <= 0 || vecCol.size() == 0) return HContainerArrayType();
	verticalLayoutUC *pver = new verticalLayoutUC();
	pParent->add(pver);
	return layoutGrid(pver, rows, rowCY, vecCol);
}
}	// wuc