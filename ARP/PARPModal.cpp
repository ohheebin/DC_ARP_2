// PARPModal.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ARP.h"
#include "PARPModal.h"
#include "afxdialogex.h"


// PARPModal ��ȭ �����Դϴ�.

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


// PARPModal �޽��� ó�����Դϴ�.
BOOL PARPModal::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->AddString("Default Device");
	((CComboBox*)GetDlgItem(IDC_PM_DEVICE))->SetCurSel(0);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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
	// ��� ��ȭ���ڿ� �Էµ� ������ ���� ���̾�α׷� ����
	// Proxy Entry ���� ���Ƿ� ���� Proxy Item ����ü�� ���� �ִ´�.
	// ���� : ���� ��ȭ���ڿ����� PARP Ŭ������ �����ؼ� Get�Լ��� ���� �����´�.

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
		// strtoul -> ���ڿ��� ���ϴ� ������ ��ȯ �����ش�.
		arp_ether[i] = (unsigned char)strtoul(cstr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';
	SetProxyEtherAddress(arp_ether);

	CDialog::OnOK();
}


void PARPModal::OnBnClickedPmCancel()
{
	// ��� ��ȭ���� ����
	CDialog::OnCancel();
}
