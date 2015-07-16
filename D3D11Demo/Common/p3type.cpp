#include "p3types.h"
CMyRect::CMyRect()
{
	left = top = right = bottom = 0;
}

CMyRect::CMyRect( int l, int t, int r, int b )
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

CMyRect::CMyRect( const P3_RECT& rc )
{
	left = rc.left;
	top = rc.top;
	right = rc.right;
	bottom = rc.bottom;
}

CMyRect::CMyRect(const CMyRect& src)
{
	SetRect(src.left,src.top,src.right,src.bottom);
}

CMyRect& CMyRect::operator=(const CMyRect& src)
{
	SetRect(src.left,src.top,src.right,src.bottom);
	return *this;
}

CMyRect::operator P3_RECT*()
{
	return this;
}

CMyRect::operator const P3_RECT*()
{
	return this;
}

int CMyRect::GetAbsWidth()
{
	return abs(right-left);
}

int CMyRect::GetAbsHeight()
{
	return abs(bottom-top);
}

int CMyRect::Width() const
{
	return right - left;
}

int CMyRect::Height() const
{
	return bottom - top;
}

void CMyRect::SetRect( int iLeft, int iTop, int iRight, int iBottom )
{
	left = iLeft;
	top = iTop;
	right = iRight;
	bottom = iBottom;
}

bool CMyRect::EqualRect( CMyRect* r2 )
{
	return (this->left==r2->left
		&&this->top==r2->top
		&&this->bottom==r2->bottom
		&&this->right==r2->right);
}

int CMyRect::RevokeHeight()
{
	return top-bottom;
}

void CMyRect::SetRectEmpty(  )
{
	left=0;
	right=0;
	top=0;
	bottom=0;
}

bool CMyRect::IsRectEmpty( ) const
{
	return (left == right )  && (top == bottom);
}

bool CMyRect::IsRectNull()
{
	return (top==0&&left==0&&right==0&&bottom==0);
}

void CMyRect::NormalizeRect()
{
	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}

	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}

bool CMyRect::IntersectRect( const CMyRect *lprcSrc1, const CMyRect *lprcSrc2 )
{
	if (lprcSrc1 == NULL || lprcSrc2 == NULL || lprcSrc1->IsRectEmpty() || lprcSrc2->IsRectEmpty() ||
		lprcSrc1->left >= lprcSrc2->right ||
		lprcSrc2->left >= lprcSrc1->right ||
		lprcSrc1->top >= lprcSrc2->bottom ||
		lprcSrc2->top >= lprcSrc1->bottom)
	{
		SetRectEmpty();
		return false;
	}
#define max_fun(a,b)    (((a) > (b)) ? (a) : (b))
#define min_fun(a,b)    (((a) < (b)) ? (a) : (b))
	left = max_fun(lprcSrc1->left, lprcSrc2->left);
	right = min_fun(lprcSrc1->right, lprcSrc2->right);
	top = max_fun(lprcSrc1->top, lprcSrc2->top);
	bottom = min_fun(lprcSrc1->bottom, lprcSrc2->bottom);

	return true;
}

bool CMyRect::UnionRect( CMyRect* lprc1, const CMyRect* lprc2 )
{
	left = __min(lprc1->left, lprc2->left);
	right = __max(lprc1->right, lprc2->right);
	top = __min(lprc1->top, lprc2->top);
	bottom = __max(lprc1->bottom, lprc2->bottom);
	return left < right && top < bottom;
}

void CMyRect::OffsetRect( int x/*偏移X坐标*/, int y/*偏移Y坐标*/ )
{
	left += x;
	right += x;
	top += y;
	bottom += y;
}

void CMyRect::OffsetRect( P3_POS point/*偏移的坐标*/ )
{
	OffsetRect(point.x, point.y);
}

void CMyRect::OffsetRect( P3_SIZE size/*大小*/ )
{
	OffsetRect(size.iWidth, size.iHeight);
}

bool CMyRect::PtInRect( P3_POS pt/*坐标*/ )
{
	return pt.x >= left
		&& pt.x < right
		&& pt.y >= top
		&& pt.y < bottom;
}

void CMyRect::InflateRect( int dx, int dy )
{
	left -= dx;
	right += dx;
	top -= dy;
	bottom += dy;
}

void CMyRect::InflateRect( const CMyRect* lpRect )
{
	left -= lpRect->left;
	top -= lpRect->top;
	right += lpRect->right;
	bottom += lpRect->bottom;
}

void CMyRect::InflateRect( int l, int t, int r, int b )
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}

void CMyRect::DeflateRect( const CMyRect* lpRect )
{
	left += lpRect->left;
	top += lpRect->top;
	right -= lpRect->right;
	bottom -= lpRect->bottom;
}

void CMyRect::DeflateRect( int l, int t, int r, int b )
{
	left += l;
	top += t;
	right -= r;
	bottom -= b;
}

void CMyRect::DeflateRect( int x/*左右的大小都减小*/, int y/*上下的大小都减小*/ )
{
	left += x;
	top += y;
	right -= x;
	bottom -= y;
}

CMyRectTracker::CMyRectTracker()
{
	
}

int CMyRectTracker::HitTest(P3_POS point)
{
	TrackerHit hitResult = hitNothing;
	
	if (this->PtInRect(point))
	{
		if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
			hitResult = (TrackerHit)HitTestHandles(point);
		else
			hitResult = hitMiddle;
	}
	return hitResult;
}

int CMyRectTracker::GetHandleMask() const
{
	int mask = 0x0F;   // always have 4 corner handles
	int size = m_nHandleSize*3;
	if (abs(this->Width()) - size > 4)
		mask |= 0x50;
	if (abs(this->Height()) - size > 4)
		mask |= 0xA0;
	return mask;
}

int CMyRectTracker::HitTestHandles(P3_POS point) const
{
	
	
	return hitMiddle;   // no handle hit, but hit object (or object border)
}

void CMyRectTracker::GetHandleRect(int nHandle, CMyRect* pHandleRect) const
{

	
}