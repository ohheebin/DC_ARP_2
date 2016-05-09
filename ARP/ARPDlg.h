
// ARPDlg.h : 헤더 파일
//

#if !defined(AFX_ARPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
#define AFX_ARPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LayerManager.h"		// Added by ClassView
#include "ARPLayer.h"			// Added by ClassView
#include "EthernetLayer.h"		// Added by ClassView
#include "NILayer.h"			// Added by ClassView
#include "PARPModal.h"			// Added by ClassView

// CARPDlg 대화 상자
class CARPDlg
: public CDialog,
  public CBaseLayer
{
// 생성입니다.
public:
	CARPDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ARP_DIALOG };
	
	//ARP 다이얼로그
	CListCtrl		mList_ARPCache;		// ARP Cache
	CIPAddressCtrl	mIP_ARPIP;			// ARP IP Address
	CListCtrl		mList_PARPEntry;	// PARP Entry	
	CString			mEdit_GARPEther;	// GARP Ethernet Address
	CComboBox		mCombo_MyDevice;	// MyDev ComboBox
	CString			mEdit_MyEther;		// MyDev Select Ethernet Address
	CIPAddressCtrl	mIP_MyIP;			// MyDev IP Address

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	
// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSendARP();
	afx_msg void OnSendGARP();
	afx_msg void OnSettingMyDev();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnAddPARP();
	afx_msg void OnBnClickedExit();
	afx_msg void OnARPItemDelete();
	afx_msg void OnARPAllDelete();
	afx_msg void OnPARPEntryDelete();
	afx_msg void OnGetMacAddress();

	unsigned char*	MacAddrToHexInt(CString ether);
	void	CacheTableUpdate();
	void	ProxyTableUpdate();
	
protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL			Receive( unsigned char* ppayload );
	inline void		SettingARPMsg();
	inline void		SettingGARPMsg();

private:
	CLayerManager	m_LayerMgr;

	enum {			ARP_INITIALIZING,
					ARP_OPERATING,
					ARP_ENDPROCESSING,
					ARP_DEVICE } ;

	void			SetDlgState( int state );
	inline void		EndofProcess( );

	BOOL			m_bSendReady;

// Object Layer
	CARPLayer*			m_ARP;
	CEthernetLayer*		m_Ether;
	CNILayer*			m_NI;

// Implementation
	UINT			m_wParam;
	DWORD			m_lParam;
public:
	afx_msg void OnLvnItemchangedParpTable(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // !defined(AFX_ARPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)