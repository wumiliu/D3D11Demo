
// Test_3DDlg.h : 头文件
//

#pragma once
#include <winsock.h>
#include <vector>
#include "Curve.h"

// CTest_3DDlg 对话框
class CTest_3DDlg : public CDialogEx
{
	// 构造
public:
	CTest_3DDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_TEST_3D_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnKickIdle(WPARAM wp, LPARAM lCount);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void CalculateFrameStats();
	float	Get_FPS();
	std::shared_ptr<AnimationCurve> m_Curve;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	bool m_bMove;
	POINT m_Pos;
	int m_index;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
