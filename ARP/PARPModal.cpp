// PARPModal.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ARP.h"
#include "PARPModal.h"
#include "afxdialogex.h"


// PARPModal 대화 상자입니다.

IMPLEMENT_DYNAMIC(PARPModal, CDialog)

PARPModal::PARPModal(CWnd* pParent /*=NULL*/)
	: CDialog(PARPModal::IDD, pParent)
{
	ResetHeader();
}

PARPModal::~PARPModal()
{
}

void PARPModal::ResetHeader()
{
	memset(m_proxy.proxy_ipaddr.addrs_i,0,4);
	memset(m_proxy.proxy_enetaddr.addrs_e,0,6);
	memset(m_proxy.proxy_devname,0,PM_DEVICE_NAME_LENGTH);
}

void PARPModal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_PM_DEVICE,mCombo_PARPDevice);
	DDX_Control(pDX,IDC_PM_IPADDRESS,mIP_PARPIP);
	DDX_Text(pDX,IDC_PM_ETHERNET,mEdit_PARPEther);
}


BEGIN_MESSAGE_MAP(PARPModal, CDialog)
	ON_BN_CLICKED(ID_PM_OK, &PARPModal::OnBnClickedPmOk)
	ON_BN_CLICKED(ID_PM_CANCEL, &PARPModal::OnBnClickedPmCancel)
END_MESSAGE_MAP()


// PARPModal 메시지 처리기입니다.
BOOL PARPModal::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->AddString("Default Device");
	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->SetCurSel(0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void PARPModal::SetProxyIPAddress(unsigned char* ip)
{
	memcpy(m_proxy.proxy_ipaddr.addrs_i,ip,4);
}

void PARPModal::SetProxyEtherAddress(unsigned char* ether)
{
	memcpy(m_proxy.proxy_enetaddr.addrs_e,ether,6);
}

void PARPModal::SetProxyDevName(unsigned char* devname)
{
	memcpy(m_proxy.proxy_devname,devname,strlen((char *)devname));
}
	
unsigned char* PARPModal::GetProxyIPAddress()
{
	return m_proxy.proxy_ipaddr.addrs_i;
}

unsigned char* PARPModal::GetProxyEtherAddress()
{
	return m_proxy.proxy_enetaddr.addrs_e;
}

unsigned char* PARPModal::GetProxyDevName()
{
	return m_proxy.proxy_devname;
}

void PARPModal::OnBnClickedPmOk()
{
	// 모달 대화상자에 입력된 값들을 메인 다이얼로그로 전송
	// Proxy Entry 값을 임의로 만든 Proxy Item 구조체에 값을 넣는다.
	// 참고 : 메인 대화상자에서는 PARP 클래스에 접근해서 Get함수로 값을 가져온다.

	// Device
	CString devname;
	GetDlgItemText(IDC_PM_DEVICE,devname);
	SetProxyDevName((unsigned char *)devname.GetString());

	// IP
	BYTE proxy_ip[4];
	mIP_PARPIP.GetAddress(proxy_ip[0],proxy_ip[1],proxy_ip[2],proxy_ip[3]);
	SetProxyIPAddress(proxy_ip);
	
	// Ethernet
	CString cstr, ether;
	GetDlgItemText(IDC_PM_ETHERNET,ether);
	unsigned char* arp_ether = (u_char *)malloc(sizeof(u_char)*6);
	for(int i=0;i<6;i++){
		AfxExtractSubString(cstr,ether,i,':');
		// strtoul -> 문자열을 원하는 진수로 변환 시켜준다.
		arp_ether[i] = (unsigned char)strtoul(cstr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';
	SetProxyEtherAddress(arp_ether);

	CDialog::OnOK();
}


void PARPModal::OnBnClickedPmCancel()
{
	// 모달 대화상자 종료
	CDialog::OnCancel();
}
