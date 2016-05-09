//
// ARPDlg.cpp : 구현 파일
// 201007142 양태훈
//

#include "stdafx.h"
#include "ARP.h"
#include "ARPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CARPDlg 대화 상자

CARPDlg::CARPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CARPDlg::IDD, pParent), 
	CBaseLayer( "ARPDlg" ),
	m_bSendReady(FALSE)
{
	mEdit_GARPEther = _T("");
	mEdit_MyEther = _T("");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// 계층 연결
	m_LayerMgr.AddLayer( this ) ;	
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ;
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) ) ;

	// 레이어를 연결한다. (레이어 생성)
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *ARP ( *ARPDlg ) ) ) )" );

	// 레이어 객체 생성
	m_ARP = (CARPLayer *)m_LayerMgr.GetLayer("ARP");
	m_Ether = (CEthernetLayer *)m_LayerMgr.GetLayer("Ethernet");
	m_NI = (CNILayer *)m_LayerMgr.GetLayer("NI");
}

BOOL CARPDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message )
	{
	case WM_KEYDOWN :
		switch( pMsg->wParam )
		{
		case VK_RETURN : 
			if ( ::GetDlgCtrlID( ::GetFocus( ) ) == IDC_EDIT_MSG ) 
				OnSendARP( ) ;					return FALSE ;
		case VK_ESCAPE : return FALSE ;
		}
		break ;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// 컨트롤과 변수를 잇는 작업
	DDX_Control(pDX,IDC_ARP_TABLE,mList_ARPCache);
	DDX_Control(pDX,IDC_ARP_IPADDRESS,mIP_ARPIP);
	DDX_Control(pDX,IDC_PARP_TABLE,mList_PARPEntry);
	DDX_Text(pDX,IDC_GARP_ETHER,mEdit_GARPEther);
	DDX_Control(pDX,IDC_MYDEV_LIST,mCombo_MyDevice);
	DDX_Text(pDX,IDC_MYDEV_ETHERNET,mEdit_MyEther);
	DDX_Control(pDX,IDC_MYDEV_IPADDRESS,mIP_MyIP);
}

BEGIN_MESSAGE_MAP(CARPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ARP_IPSEND, &CARPDlg::OnSendARP)
	ON_BN_CLICKED(IDC_GARP_SEND, &CARPDlg::OnSendGARP)
	ON_BN_CLICKED(IDC_MYDEV_SELECT, &CARPDlg::OnSettingMyDev)
	ON_BN_CLICKED(IDC_CANCEL, &CARPDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_PARP_ADD, &CARPDlg::OnAddPARP)
	ON_BN_CLICKED(IDC_EXIT, &CARPDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_ARP_ITEMDELETE, &CARPDlg::OnARPItemDelete)
	ON_BN_CLICKED(IDC_ARP_ALLDELETE, &CARPDlg::OnARPAllDelete)
	ON_BN_CLICKED(IDC_PARP_DELETE, &CARPDlg::OnPARPEntryDelete)
	ON_CBN_SELCHANGE(IDC_MYDEV_LIST, &CARPDlg::OnGetMacAddress)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PARP_TABLE, &CARPDlg::OnLvnItemchangedParpTable)
END_MESSAGE_MAP()


// CARPDlg 메시지 처리기

BOOL CARPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetDlgState(ARP_INITIALIZING);
	SetDlgState(ARP_DEVICE);

	/*********************************************************/
	// 리스트 컨트롤 Column 추가
	((CListCtrl*)GetDlgItem(IDC_ARP_TABLE))->InsertColumn(0,"IP Address",LVCFMT_LEFT,130);
	((CListCtrl*)GetDlgItem(IDC_ARP_TABLE))->InsertColumn(1,"Ethernet Address",LVCFMT_LEFT,150);
	((CListCtrl*)GetDlgItem(IDC_ARP_TABLE))->InsertColumn(2,"Status",LVCFMT_LEFT,80);

	((CListCtrl*)GetDlgItem(IDC_PARP_TABLE))->InsertColumn(0,"Device",LVCFMT_LEFT,100);
	((CListCtrl*)GetDlgItem(IDC_PARP_TABLE))->InsertColumn(1,"IP Address",LVCFMT_LEFT,120);
	((CListCtrl*)GetDlgItem(IDC_PARP_TABLE))->InsertColumn(2,"Ethernet Address",LVCFMT_LEFT,140);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SC_CLOSE )
	{
		if ( MessageBox( "Are you sure ?", "Question", 
			MB_YESNO | MB_ICONQUESTION ) == IDNO )
			return ;
		else EndofProcess( ) ;
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CARPDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer( ) ;
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CARPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CARPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CARPDlg::SetDlgState( int state )
{
	UpdateData( TRUE ) ;
	int i;
	CString device_description;

	CButton*	pPARP_AddButton = (CButton*) GetDlgItem( IDC_PARP_ADD ) ;
	CButton*	pPARP_DeleteButton = (CButton*) GetDlgItem( IDC_PARP_DELETE ) ;
	CButton*	pGARP_SendButton = (CButton*) GetDlgItem( IDC_GARP_SEND ) ;
	CButton*	pMY_SelectButton = (CButton*) GetDlgItem( IDC_MYDEV_SELECT ) ;
	CButton*	pARP_IPSendButton = (CButton*) GetDlgItem( IDC_ARP_IPSEND ) ;
	CButton*	pARP_ItemDeleteButton = (CButton*) GetDlgItem( IDC_ARP_ITEMDELETE ) ;
	CButton*	pARP_AllDeleteButton = (CButton*) GetDlgItem( IDC_ARP_ALLDELETE ) ;
	CButton*	p_ExitButton = (CButton*) GetDlgItem( IDC_EXIT ) ;
	CButton*	p_CancelButton = (CButton*) GetDlgItem( IDC_CANCEL ) ;

	CEdit*		pGARP_EtherEdit = (CEdit*) GetDlgItem( IDC_GARP_ETHER ) ;
	CEdit*		pMY_EtherEdit = (CEdit*) GetDlgItem( IDC_MYDEV_ETHERNET ) ;

	CComboBox*	pMY_EtherComboBox = (CComboBox*)GetDlgItem( IDC_MYDEV_LIST );

	CIPAddressCtrl* pARP_IPAddrCtrl = (CIPAddressCtrl*) GetDlgItem( IDC_ARP_IPADDRESS ) ;
	CIPAddressCtrl* pMY_IPAddrCtrl = (CIPAddressCtrl*) GetDlgItem( IDC_MYDEV_IPADDRESS ) ;

	switch(state)
	{
	case ARP_INITIALIZING:
		pPARP_AddButton->EnableWindow(FALSE);
		pPARP_DeleteButton->EnableWindow(FALSE);
		pGARP_SendButton->EnableWindow(FALSE);
		pARP_IPSendButton->EnableWindow(FALSE);
		pARP_ItemDeleteButton->EnableWindow(FALSE);
		pARP_AllDeleteButton->EnableWindow(FALSE);
		p_ExitButton->EnableWindow(FALSE);

		pGARP_EtherEdit->EnableWindow(FALSE);
		pARP_IPAddrCtrl->EnableWindow(FALSE);
		break;
	case ARP_OPERATING:
		pPARP_AddButton->EnableWindow(TRUE);
		pPARP_DeleteButton->EnableWindow(TRUE);
		pGARP_SendButton->EnableWindow(TRUE);
		pARP_IPSendButton->EnableWindow(TRUE);
		pARP_ItemDeleteButton->EnableWindow(TRUE);
		pARP_AllDeleteButton->EnableWindow(TRUE);
		p_ExitButton->EnableWindow(TRUE);

		pGARP_EtherEdit->EnableWindow(TRUE);
		pARP_IPAddrCtrl->EnableWindow(TRUE);

		pMY_EtherComboBox->EnableWindow(FALSE);
		pMY_IPAddrCtrl->EnableWindow(FALSE);
		pMY_SelectButton->EnableWindow(FALSE);
		p_CancelButton->EnableWindow(FALSE);
		break;
	case ARP_ENDPROCESSING:
		pPARP_AddButton->EnableWindow(FALSE);
		pPARP_DeleteButton->EnableWindow(FALSE);
		pGARP_SendButton->EnableWindow(FALSE);
		pARP_IPSendButton->EnableWindow(FALSE);
		pARP_ItemDeleteButton->EnableWindow(FALSE);
		pARP_AllDeleteButton->EnableWindow(FALSE);
		p_ExitButton->EnableWindow(FALSE);
		
		pGARP_EtherEdit->EnableWindow(FALSE);
		pARP_IPAddrCtrl->EnableWindow(FALSE);

		pMY_EtherComboBox->EnableWindow(TRUE);
		pMY_IPAddrCtrl->EnableWindow(TRUE);
		pMY_SelectButton->EnableWindow(TRUE);
		p_CancelButton->EnableWindow(TRUE);
		break;
	case ARP_DEVICE:
		for(i=0;i<NI_COUNT_NIC;i++){
			if(!m_NI->GetAdapterObject(i))
				break;
			device_description = m_NI->GetAdapterObject(i)->description;
			device_description.Trim();
			pMY_EtherComboBox->AddString(device_description);
		}
		
		pMY_EtherComboBox->SetCurSel(0);
		CString nicName = m_NI->GetAdapterObject(0)->name;
		
		// 콤보박스에서 0번 인덱스의 NICard의 Mac 주소를 보여줌
		mEdit_MyEther = m_NI->GetNICardAddress((char *)nicName.GetString());
		
		break;
	}

	UpdateData( FALSE ) ;
}

BOOL CARPDlg::Receive( unsigned char* ppayload )
{
	// Reply 메시지를 받은 경우를 처리하는 함수
	// 요청한 Mac address를 받은 것이므로
	// 요청했던 IP의 타이머를 죽이고, (요청후 기다리는 최대 소요 시간)
	// 새로 타이머를 할당한다. (갱신된 Mac Address의 IP 유효 시간)

	// Cache Table 갱신
	CacheTableUpdate();
	

	int index = (int)ppayload;
	if(index>=0){
		KillTimer(index);
		SetTimer(index,TIMER_VALIDTIME,NULL);
	}
	return TRUE;
}

// ARP Group
void CARPDlg::OnARPItemDelete() // ItemDelete 버튼
{
	// ARP Cache에서 Item 하나 삭제
	UpdateData( TRUE ) ;
	POSITION pos = mList_ARPCache.GetFirstSelectedItemPosition();

	int nItem;
	if(pos!=NULL)
	{
		// 인덱스를 찾아서 삭제
		while(pos)
			nItem = mList_ARPCache.GetNextSelectedItem(pos);
		m_ARP->CacheDeleteItem(nItem);

		CacheTableUpdate();
	}

	UpdateData( FALSE ) ;
}

void CARPDlg::OnARPAllDelete() // AllDelete 버튼
{
	// ARP Cache에서 Item 모두 삭제
	UpdateData( TRUE ) ;

	// Cache table의 모든 Entry 삭제
	m_ARP->CacheAllDelete();
	
	// Cache table 갱신
	CacheTableUpdate();

	UpdateData( FALSE ) ;
}

void CARPDlg::OnSendARP() // Send(&S) 버튼
{
	// ARP Request 전송
	UpdateData( TRUE ) ;

	BYTE ip[4];
	IP_ADDR chk_ip;
	mIP_ARPIP.GetAddress(ip[0],ip[1],ip[2],ip[3]);
	chk_ip.addr_i0 = ip[0]; chk_ip.addr_i1 = ip[1];
	chk_ip.addr_i2 = ip[2]; chk_ip.addr_i3 = ip[3];

	// Cache table에 내가 요청하고자 하는 ip가 있는 경우
	if(m_ARP->CacheSearchExist(chk_ip))
	{
		CACHE_ENTRY cache_entry = m_ARP->GetSelectCacheEntry(chk_ip);		
		if(cache_entry.cache_enetaddr.addrs_e!=NULL)
		{
			CString ether;
			ether.Format("IP Exist\nEthernet : %.02x:%.02x:%.02x:%.02x:%.02x:%.02x",cache_entry.cache_enetaddr.addr_e0
				,cache_entry.cache_enetaddr.addr_e1
				,cache_entry.cache_enetaddr.addr_e2
				,cache_entry.cache_enetaddr.addr_e3
				,cache_entry.cache_enetaddr.addr_e4
				,cache_entry.cache_enetaddr.addr_e5);

			// 경고 메시지 출력
			AfxMessageBox(ether);
		}
		return;
	}

	IP_ADDR arp_ip;
	mIP_MyIP.GetAddress(ip[0],ip[1],ip[2],ip[3]);
	arp_ip.addr_i0 = ip[0]; arp_ip.addr_i1 = ip[1];
	arp_ip.addr_i2 = ip[2]; arp_ip.addr_i3 = ip[3];
	
	// 보내고자 하는 IP가 Proxy table에 존재하면 안된다.
	if(m_ARP->ProxySearchExist(chk_ip))
		return;
	// 나의 IP와 요청하고자 하는 IP의 주소가 같으면 안된다. (GARP로 따로 처리)
	if(!memcmp(chk_ip.addrs_i,arp_ip.addrs_i,4))
		return;

	SettingARPMsg(); // ARP Message 설정
	m_ARP->Send(NULL,ARP_HEADER_SIZE);

	// Cache Table에 추가
	int index;
	// Entry가 정상적으로 table에 추가가 된 경우 Timer 설정
	if((index = m_ARP->CacheAddItem(NULL))>-1)
		SetTimer(index,TIMER_REPLYTIME,NULL);

	// Cache table 갱신
	CacheTableUpdate();

	UpdateData( FALSE ) ;
}

// GARP Group
void CARPDlg::OnSendGARP() // Send(&G) 버튼
{
	// Gratuitous ARP 전송
	UpdateData( TRUE ) ;
	
	SettingGARPMsg(); // GARP Msg
	m_ARP->Send(NULL,ARP_HEADER_SIZE); // 전송
	
	UpdateData( FALSE ) ;
}

// MyDevice Group
void CARPDlg::OnSettingMyDev() // Select 버튼
{
	// 사용할 Device 선택
	UpdateData( TRUE ) ;
	unsigned char my_ip[4];
	mIP_MyIP.GetAddress(my_ip[0],my_ip[1],my_ip[2],my_ip[3]);
	
	// 설정한 Ethernet 주소와 IP 주소를 ARP 헤더로 추가
	m_ARP->SetSenderIPAddress(my_ip);
	m_ARP->SetSenderEtherAddress(MacAddrToHexInt(mEdit_MyEther));

	// 선택한 NICard로 Adapter 등록
	int nIndex = mCombo_MyDevice.GetCurSel();
	m_NI->SetAdapterNumber(nIndex);

	// Packet Driver 동작
	m_NI->PacketStartDriver();
	
	// 화면 설정
	SetDlgState( ARP_OPERATING ) ;

	UpdateData( FALSE ) ;
}

// Proxy ARP Entry Group
void CARPDlg::OnAddPARP() // Proxy ARP Entry Add 버튼
{
	// Proxy ARP Entry를 추가하기 위한 대화상자를 생성, 호출
	PARPModal dlg;
	if(dlg.DoModal() == IDOK)
	{
		// 생성된 모달 뷰에서 확인 버튼을 눌렀을 때의 처리

		// PARPModal 클래스에 접근하여, 데이터를 가져와 Proxy Struct 객체를 생성한다.
		PROXY_ENTRY proxy;
		memcpy(proxy.proxy_ipaddr.addrs_i,dlg.GetProxyIPAddress(),4);
		if(m_ARP->ProxySearchExist(proxy.proxy_ipaddr)){
			AfxMessageBox("IP가 테이블에 존재합니다.");
			return;
		}
		memcpy(proxy.proxy_enetaddr.addrs_e,dlg.GetProxyEtherAddress(),6);
		memcpy(proxy.proxy_devname,dlg.GetProxyDevName(),PM_DEVICE_NAME_LENGTH);

		// 생성한 객체를 Proxy entry의 table에 추가
		m_ARP->ProxyAddItem(proxy);
		
		// 추가 후 Proxy table 갱신
		ProxyTableUpdate();
	}
}

void CARPDlg::OnPARPEntryDelete()
{
	// Proxy ARP Entry Delete 버튼
	POSITION pos = mList_PARPEntry.GetFirstSelectedItemPosition();
	
	int nItem;
	if(pos!=NULL)
	{
		// table의 선택된 item의 인덱스를 반환
		while(pos)
			nItem = mList_PARPEntry.GetNextSelectedItem(pos);

		// proxy entry에서 선택된 item 삭제
		m_ARP->ProxyDeleteItem(nItem);

		// 해당 entry 삭제 후 Proxy table 갱신
		ProxyTableUpdate();
	}
}

// Main Window
void CARPDlg::OnBnClickedExit() // 종료 Button
{
	// ARP 동작 종료
	UpdateData( TRUE ) ;
	
	// 화면 설정
	SetDlgState(ARP_ENDPROCESSING);

	// Cache talbe의 모든 값을 삭제
	m_ARP->CacheAllDelete();

	// Cache table 갱신
	CacheTableUpdate();

	UpdateData( FALSE ) ;
}

void CARPDlg::OnBnClickedCancel() // 취소 Button
{
	// 메인 윈도우 닫기
	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
}

void CARPDlg::OnGetMacAddress()
{
	UpdateData ( TRUE );
	// NICard 이름으로 Mac address 가져오기
	CComboBox*	pMY_EtherComboBox = (CComboBox*)GetDlgItem( IDC_MYDEV_LIST );
	
	// ComboBox에서 선택된 인덱스 번호 가져오기
	int sIndex = pMY_EtherComboBox->GetCurSel();

	// 가져온 인덱스 번호로 NIC의 Adapter 이름 가져오기
	CString nicName = m_NI->GetAdapterObject(sIndex)->name;
	
	// Adapter 이름으로 Mac Address를 가져오기
	mEdit_MyEther = m_NI->GetNICardAddress((char *)nicName.GetString());

	UpdateData ( FALSE );
}


//
// Helper 함수
//

// Timer
void CARPDlg::OnTimer(UINT nIDEvent) 
{
	// 타이머가 종료시 발생 되는 이벤트 함수
	
	// 발생된 타이머를 종료/
	KillTimer( nIDEvent );

	// 발생된 타이머 번호와 맞는 Entry를 찾아서 삭제
	m_ARP->CacheDeleteByTimer(nIDEvent);

	// 삭제 후 Table을 갱신
	CacheTableUpdate();

	CDialog::OnTimer(nIDEvent);
}

unsigned char* CARPDlg::MacAddrToHexInt(CString ether)
{
	// 콜론(:)으로 구분 되어진 Ethernet 주소를
	// 콜론(:)을 토큰으로 한 바이트씩 값을 가져와서 Ethernet배열에 넣어준다.
	CString cstr;
	unsigned char *arp_ether = (u_char *)malloc(sizeof(u_char)*6);

	for(int i=0;i<6;i++){
		AfxExtractSubString(cstr,ether,i,':');
		// strtoul -> 문자열을 원하는 진수로 변환 시켜준다.
		arp_ether[i] = (unsigned char)strtoul(cstr.GetString(),NULL,16);
	}
	arp_ether[6] = '\0';

	return arp_ether;
}

void CARPDlg::CacheTableUpdate()
{
	// Cache Table에 리스트 추가
	// 리스트를 한 번 전부 제거한 후에,
	// 실제 테이블이 저장되어져 있는 연결리스트로 구성된
	// ARP Layer에서 생성된 cache_head를 통해 추가한다.

	mList_ARPCache.DeleteAllItems();

	LPARP_NODE head;
	LPARP_NODE currNode;

	head = m_ARP->cache_head;
	currNode = head;
	while(currNode!=NULL)
	{
		LV_ITEM lvItem;
		lvItem.iItem = mList_ARPCache.GetItemCount();
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;

		// IP
		CString ip;
		ip.Format("%d.%d.%d.%d",currNode->key.cache_ipaddr.addr_i0,
			currNode->key.cache_ipaddr.addr_i1,
			currNode->key.cache_ipaddr.addr_i2,
			currNode->key.cache_ipaddr.addr_i3);
		mList_ARPCache.SetItemText(lvItem.iItem, 0, (LPSTR)(LPCTSTR)ip);
		
		lvItem.pszText = (LPSTR)(LPCTSTR)ip;
		mList_ARPCache.InsertItem(&lvItem);

		// Ethernet
		CString ether;
		ether.Format("%.02x:%.02x:%.02x:%.02x:%.02x:%.02x",currNode->key.cache_enetaddr.addr_e0,
			currNode->key.cache_enetaddr.addr_e1,
			currNode->key.cache_enetaddr.addr_e2,
			currNode->key.cache_enetaddr.addr_e3,
			currNode->key.cache_enetaddr.addr_e4,
			currNode->key.cache_enetaddr.addr_e5);
		mList_ARPCache.SetItemText(lvItem.iItem, 1, (LPSTR)(LPCTSTR)ether);

		// Type
		CString type;
		switch(currNode->key.cache_type)
		{
		case CACHE_INCOMPLETE: type = "incomplete";	break;
		case CACHE_COMPLETE: type = "complete";		break;
		}
		mList_ARPCache.SetItemText(lvItem.iItem, 2, (LPSTR)(LPCTSTR)type);
		
		currNode = currNode->next;
	}
}

void CARPDlg::ProxyTableUpdate()
{
	// Proxy Table에 리스트 추가
	// 리스트를 한 번 전부 제거한 후에,
	// 실제 테이블이 저장되어져 있는 연결리스트로 구성된
	// ARP Layer에서 생성된 proxy_head를 통해 추가한다.

	mList_PARPEntry.DeleteAllItems();

	LPPARP_NODE head;
	LPPARP_NODE currNode;

	head = m_ARP->proxy_head;
	currNode = head;
	while(currNode!=NULL)
	{
		LV_ITEM lvItem;
		lvItem.iItem = mList_PARPEntry.GetItemCount();
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;

		// Device
		CString type;
		type = currNode->key.proxy_devname;
		mList_PARPEntry.SetItemText(lvItem.iItem, 0, (LPSTR)(LPCTSTR)type);

		lvItem.pszText = (LPSTR)(LPCTSTR)type;
		mList_PARPEntry.InsertItem(&lvItem);

		// IP
		CString ip;
		ip.Format("%d.%d.%d.%d",currNode->key.proxy_ipaddr.addr_i0,
			currNode->key.proxy_ipaddr.addr_i1,
			currNode->key.proxy_ipaddr.addr_i2,
			currNode->key.proxy_ipaddr.addr_i3);
		mList_PARPEntry.SetItemText(lvItem.iItem, 1, (LPSTR)(LPCTSTR)ip);	

		// Ethernet
		CString ether;
		ether.Format("%.02x:%.02x:%.02x:%.02x:%.02x:%.02x",currNode->key.proxy_enetaddr.addr_e0,
			currNode->key.proxy_enetaddr.addr_e1,
			currNode->key.proxy_enetaddr.addr_e2,
			currNode->key.proxy_enetaddr.addr_e3,
			currNode->key.proxy_enetaddr.addr_e4,
			currNode->key.proxy_enetaddr.addr_e5);
		mList_PARPEntry.SetItemText(lvItem.iItem, 2, (LPSTR)(LPCTSTR)ether);
	
		currNode = currNode->next;
	}
}

inline void CARPDlg::SettingARPMsg()
{
	// ARP Message 설정

	unsigned char ether_DestEther[6];
	unsigned char arp_SenderIP[4];
	unsigned char arp_TargetIP[4];
	unsigned char *arp_SenderEther;
	unsigned char arp_TargetEther[6];
	
	arp_SenderEther = (u_char *)malloc(sizeof(u_char)*6);

	// Get IP
	mIP_MyIP.GetAddress(arp_SenderIP[0],arp_SenderIP[1],arp_SenderIP[2],arp_SenderIP[3]);
	mIP_ARPIP.GetAddress(arp_TargetIP[0],arp_TargetIP[1],arp_TargetIP[2],arp_TargetIP[3]);

	// Get Ethernet
	arp_SenderEther = MacAddrToHexInt(mEdit_MyEther);
	memset(arp_TargetEther,0,6); // 상대 Mac주소는 모르는 상태

	// ARP Header Setting
	// ByteOrdering
	m_ARP->SetHardType(ntohs(ARP_HARDTYPE));		// 1
	m_ARP->SetProtType(ntohs(ARP_PROTOTYPE_IP));	// 0x0800
	m_ARP->SetOpcode(ntohs(ARP_OPCODE_REQUEST));	// 1
	// Non-ByteOrdering
	m_ARP->SetSenderEtherAddress(arp_SenderEther);	// sender ether
	m_ARP->SetSenderIPAddress(arp_SenderIP);		// sender ip
	m_ARP->SetTargetEtherAddress(arp_TargetEther);	// target ether
	m_ARP->SetTargetIPAddress(arp_TargetIP);		// target ip

	// Ethernet Header Setting
	memset(ether_DestEther,0xff,6);	// Broadcast
	m_Ether->SetEnetDstAddress(ether_DestEther);	// broadcast ether
	m_Ether->SetEnetSrcAddress(arp_SenderEther);	// sender ether
}

inline void CARPDlg::SettingGARPMsg()
{
	// Gratuitous ARP Message 설정
	// 참고 : Sender와 Target의 IP가 동일하게 설정

	unsigned char broadcast[6];
	unsigned char arp_SenderIP[4];
	unsigned char arp_TargetIP[4];
	unsigned char *arp_SenderEther;
	unsigned char arp_TargetEther[6];

	arp_SenderEther = (u_char *)malloc(sizeof(u_char)*6);

	// Get IP
	mIP_MyIP.GetAddress(arp_SenderIP[0],arp_SenderIP[1],arp_SenderIP[2],arp_SenderIP[3]);
	mIP_ARPIP.GetAddress(arp_TargetIP[0],arp_TargetIP[1],arp_TargetIP[2],arp_TargetIP[3]);

	// Get Ethernet
	arp_SenderEther = MacAddrToHexInt(mEdit_GARPEther);
	memset(arp_TargetEther,0,6);

	// ARP Header Setting
	// ByteOrdering
	m_ARP->SetHardType(ntohs(ARP_HARDTYPE));		// 1
	m_ARP->SetProtType(ntohs(ARP_PROTOTYPE_IP));	// 0x0800
	m_ARP->SetOpcode(ntohs(ARP_OPCODE_REQUEST));	// 1
	// Non-ByteOrdering
	m_ARP->SetSenderEtherAddress(arp_SenderEther);	// sender ether
	m_ARP->SetSenderIPAddress(arp_SenderIP);		// sender ip
	m_ARP->SetTargetEtherAddress(arp_TargetEther);	// target ether
	m_ARP->SetTargetIPAddress(arp_SenderIP);		// target ip

	// Ethernet Header Setting
	memset(broadcast,0xff,6);	// Broadcast
	m_Ether->SetEnetDstAddress(broadcast);			// broadcast ether
	m_Ether->SetEnetSrcAddress(arp_SenderEther);	// sender ether
}

void CARPDlg::OnLvnItemchangedParpTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	*pResult = 0;
}
