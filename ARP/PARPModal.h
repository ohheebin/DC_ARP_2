#pragma once

// PARPModal 대화 상자입니다.

class PARPModal : public CDialog
{
	DECLARE_DYNAMIC(PARPModal)

public:
	PARPModal(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PARPModal();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PARP_MODAL };

private:
	inline void		ResetHeader( );

public:
	BOOL dd;
	//PARP 추가 다이얼로그
	CComboBox mCombo_PARPDevice;	// PARP ComboBox
	CIPAddressCtrl mIP_PARPIP;		// PARP IP Address
	CString mEdit_PARPEther;		// PARP Ethernet Address

	void	SetProxyIPAddress(unsigned char* ip);
	void	SetProxyEtherAddress(unsigned char* ether);
	void	SetProxyDevName(unsigned char* devname);
	
	unsigned char*	GetProxyIPAddress();
	unsigned char*	GetProxyEtherAddress();
	unsigned char*	GetProxyDevName();

protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	PROXY_ENTRY m_proxy;

public:
	afx_msg void OnBnClickedPmOk();
	afx_msg void OnBnClickedPmCancel();
};
