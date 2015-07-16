#ifndef __P3_PRIMITIVE_H__
#define __P3_PRIMITIVE_H__

typedef struct 
{
	int  iWidth;
	int  iHeight;
}	P3_SIZE;

typedef struct 
{
	int	x;
	int   y;
}	P3_POS;

typedef struct
{
	int left;
	int top;
	int right;
	int bottom;
}	P3_RECT;

#endif//__P3_PRIMITIVE_H__