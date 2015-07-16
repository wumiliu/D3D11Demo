#ifndef _P3_TYPES_H
#define _P3_TYPES_H

#include "P3Primitive.h"
#include <stdlib.h>
//区域(CUIRect(left,top,right,bottom))
class CMyRect: public P3_RECT
{
public:
	CMyRect();
	CMyRect(int l, int t, int r, int b);

	CMyRect(const P3_RECT& rc);
	CMyRect(const CMyRect& src);
	CMyRect& operator=(const CMyRect& src);


	//获取宽度
	int Width() const;

	//获取高度
	int Height() const;

	//CRect转P3_RECT
	operator P3_RECT*();

	//CRect转const P3_RECT
	operator const P3_RECT*();

	//设置区域
	void SetRect(int iLeft, int iTop, int iRight, int iBottom);

	bool EqualRect(CMyRect* r2);

	void NormalizeRect();

	//求两矩形的相交区域(若不相交返回false,当前区域置空;若相交则计算出两矩形的相交区域)
	bool IntersectRect(const CMyRect *lprcSrc1, const CMyRect *lprcSrc2 );

	bool UnionRect(CMyRect*  lprc1, const CMyRect* lprc2);

	//偏移大小(左右X坐标同时向右移动,上下Y坐标同时向下移动)
	void OffsetRect(int x/*偏移X坐标*/, int y/*偏移Y坐标*/);

	//偏移大小(OffsetRect(point.x, point.y))
	void OffsetRect(P3_POS point/*偏移的坐标*/);

	//偏移大小
	void OffsetRect(P3_SIZE size/*大小*/);

	//宽度的绝对值
	int GetAbsWidth();

	//高度的绝对值
	int GetAbsHeight();

	//反向高度(top-bottom)
	int RevokeHeight();

	//置空区域(top=left=bottom=right=0)
	void SetRectEmpty();

	//区域是否为空((left == right )  && (top == bottom))
	bool IsRectEmpty() const;

	//区域是否不存在(top,left,right,bottom都为0时返回真)
	bool IsRectNull();

	//坐标是否在本区域内(左开右闭:若rect.left<=point.x<rect.right 且 rect.top<=point.y<rect.bottom 则point在rect内)
	bool PtInRect(P3_POS pt/*坐标*/);

	/*增大或减小指定矩形的宽和高。(InflateRect函数在矩形的左和右增加dx,在矩形的上下增加dy。 dx和dy参数是有符号值。正数增加宽和高，负数减小。) */
	void InflateRect( int dx, int dy);

	void InflateRect(const CMyRect* lpRect);	

	void InflateRect(int l, int t, int r, int b) ;	

	void DeflateRect(const CMyRect* lpRect) ;

	//缩小矩形的边长
	void DeflateRect(int l, int t, int r, int b) ;

	//缩小矩形的边长
	void DeflateRect(int x/*左右的大小都减小*/, int y/*上下的大小都减小*/);
};

class CMyRectTracker :public CMyRect
{
public:
	// Style Flags
	enum StyleFlags
	{
		solidLine = 1, dottedLine = 2, hatchedBorder = 4,
		resizeInside = 8, resizeOutside = 16, hatchInside = 32,
	};

	// Hit-Test codes
	enum TrackerHit
	{
		hitNothing = -1,
		hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
		hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8
	};
	CMyRectTracker();
	int HitTest(P3_POS point);
	int HitTestHandles(P3_POS point) const;
	int GetHandleMask() const;
	void GetHandleRect(int nHandle, CMyRect* pHandleRect) const;
private:
	CMyRect	m_Rect;
	// Attributes
	int m_nStyle;      // current state
	int m_nHandleSize;
};

#endif