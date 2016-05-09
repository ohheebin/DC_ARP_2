
// ARP.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//
#if !defined(AFX_ARP_H__3D24E705_A889_431A_B388_17290947B99B__INCLUDED_)
#define AFX_ARP_H__3D24E705_A889_431A_B388_17290947B99B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// CARPApp:
// 이 클래스의 구현에 대해서는 ARP.cpp을 참조하십시오.
//

class CARPApp : public CWinApp
{
public:
	CARPApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CARPApp theApp;

#endif // !defined(AFX_ARP_H__3D24E705_A889_431A_B388_17290947B99B__INCLUDED_)