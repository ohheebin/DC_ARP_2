// EthernetLayer.cpp: implementation of the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARP.h"
#include "ARPLayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CARPLayer::CARPLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
	cache_head = NULL;
	proxy_head = NULL;
}

CARPLayer::~CARPLayer()
{
}

void CARPLayer::ResetHeader()
{
	m_sHeader.arp_hardtype = 0x0000;
	m_sHeader.arp_prottype = 0x0000;
	m_sHeader.arp_hardsize = 0x06;
	m_sHeader.arp_protsize = 0x04;
	m_sHeader.arp_op = 0x0000;
	memset(m_sHeader.arp_srcenet.addrs_e, 0, 6);
	memset(m_sHeader.arp_srcip.addrs_i, 0, 4);
	memset(m_sHeader.arp_dstenet.addrs_e, 0, 6);
	memset(m_sHeader.arp_dstip.addrs_i, 0, 4);
}

void CARPLayer::SetHardType(unsigned short hardtype)
{
	m_sHeader.arp_hardtype = hardtype;
}
void CARPLayer::SetProtType(unsigned short prottype)
{
	m_sHeader.arp_prottype = prottype;
}

void CARPLayer::SetOpcode(unsigned short op)
{
	m_sHeader.arp_op = op;
}
void CARPLayer::SetSenderEtherAddress(unsigned char* enet)
{
	ETHERNET_ADDR s_ether;
	s_ether.addr_e0 = enet[0];
	s_ether.addr_e1 = enet[1];
	s_ether.addr_e2 = enet[2];
	s_ether.addr_e3 = enet[3];
	s_ether.addr_e4 = enet[4];
	s_ether.addr_e5 = enet[5];

	memcpy(m_sHeader.arp_srcenet.addrs_e, s_ether.addrs_e, 6);
 }
void CARPLayer::SetSenderIPAddress(unsigned char* ip)
{
	IP_ADDR s_ip;
	s_ip.addr_i0 = ip[0];
	s_ip.addr_i1 = ip[1];
	s_ip.addr_i2 = ip[2];
	s_ip.addr_i3 = ip[3];

	memcpy(m_sHeader.arp_srcip.addrs_i, s_ip.addrs_i, 4);
}
void CARPLayer::SetTargetEtherAddress(unsigned char* enet)
{
	ETHERNET_ADDR t_ether;
	t_ether.addr_e0 = enet[0];
	t_ether.addr_e1 = enet[1];
	t_ether.addr_e2 = enet[2];
	t_ether.addr_e3 = enet[3];
	t_ether.addr_e4 = enet[4];
	t_ether.addr_e5 = enet[5];

	memcpy(m_sHeader.arp_dstenet.addrs_e, t_ether.addrs_e, 6);
}
void CARPLayer::SetTargetIPAddress(unsigned char* ip)
{
	IP_ADDR t_ip;
	t_ip.addr_i0 = ip[0];
	t_ip.addr_i1 = ip[1];
	t_ip.addr_i2 = ip[2];
	t_ip.addr_i3 = ip[3];

	memcpy(m_sHeader.arp_dstip.addrs_i, t_ip.addrs_i, 4);
}

unsigned char* CARPLayer::GetSenderEtherAddress()
{
	return m_sHeader.arp_srcenet.addrs_e;
}
unsigned char* CARPLayer::GetSenderIPAddress()
{
	return m_sHeader.arp_srcip.addrs_i;
}
unsigned char* CARPLayer::GetTargetEtherAddress()
{
	return m_sHeader.arp_dstenet.addrs_e;
}
unsigned char* CARPLayer::GetTargetIPAddress()
{
	return m_sHeader.arp_dstip.addrs_i;
}

BOOL CARPLayer::Send(unsigned char *ppayload, int nlength)
{
	BOOL bSuccess = FALSE ;
	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
	return bSuccess ;
}
	
BOOL CARPLayer::Receive( unsigned char* ppayload )
{
	// ���� �������� ���� payload�� ���� ������ header������ �°� ����.
	PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;
	
	m_Ether = (CEthernetLayer *)GetUnderLayer();

	int index;

	// Case 1 : ARP
	// ��� ARP �޽����� ���� ip�� �� ip�� ����,
	// ��� ARP �޽����� ��� ip�� �� ip�� �޶���ϰ�, 
	// ��� ARP �޽����� ���� ip�� ��� ARP �޽����� ��� ip�� �޶�� �Ѵ�.
	// �׸��� Proxy Table�� �������� �ʴ� entry��� �Ѵ�.
	if(!memcmp((char *)pFrame->arp_dstip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4) &&
		memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4) &&
		memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)pFrame->arp_dstip.addrs_i,4) &&
		!ProxySearchExist(pFrame->arp_dstip))
	{
		if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST) // ���κ��� ��û ���� ���
		{
			m_Ether->SetEnetDstAddress(pFrame->arp_srcenet.addrs_e);
			m_Ether->SetEnetSrcAddress(m_sHeader.arp_srcenet.addrs_e);
			SetHardType(ntohs(ARP_HARDTYPE));
			SetProtType(ntohs(ARP_PROTOTYPE_IP));
			SetOpcode(ntohs(ARP_OPCODE_REPLY));
			SetSenderEtherAddress(m_sHeader.arp_srcenet.addrs_e);
			SetSenderIPAddress(m_sHeader.arp_srcip.addrs_i);
			SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
			SetTargetIPAddress(pFrame->arp_srcip.addrs_i);
			
			// Cache Table�� entry �߰�
			index = CacheAddItem(ppayload);

			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)index);

			mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
		}
		else if(ntohs(pFrame->arp_op) == ARP_OPCODE_REPLY) // ��뿡�� ���� ���� ���
		{
			index = CacheUpdate(ppayload);
			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*)index);
		}
	}
	// GARP�� ���
	else if(!memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)pFrame->arp_dstip.addrs_i,4))
	{
		if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST &&
			!memcmp((char *)pFrame->arp_srcip.addrs_i,(char*)m_sHeader.arp_srcip.addrs_i,4))
		{
			SetOpcode(ntohs(ARP_OPCODE_REPLY));
			SetSenderEtherAddress(m_sHeader.arp_srcenet.addrs_e);
			mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
		}
		else if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST &&
			CacheSearchExist(pFrame->arp_dstip))
		{
			index = CacheUpdate(ppayload);
			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) index);
		}
	}
	// Proxy ARP�� ���
	else
	{
		if(ntohs(pFrame->arp_op) == ARP_OPCODE_REQUEST &&
			ProxySearchExist(pFrame->arp_dstip)) // ���κ��� ��û ���� ���
		{
			// ��밡 ��û�� IP�� ���ּҸ� ����� ������.
			m_Ether->SetEnetDstAddress(pFrame->arp_srcenet.addrs_e);
			m_Ether->SetEnetSrcAddress(m_sHeader.arp_srcenet.addrs_e);
			SetHardType(ntohs(ARP_HARDTYPE));
			SetProtType(ntohs(ARP_PROTOTYPE_IP));
			SetOpcode(ntohs(ARP_OPCODE_REPLY));
			// Sender�� Ether�� IP Address�� Proxy���� ����
			SetTargetEtherAddress(pFrame->arp_srcenet.addrs_e);
			SetTargetIPAddress(pFrame->arp_srcip.addrs_i);
			
			// Cache Table�� entry �߰�
			CacheAddItem(ppayload);

			bSuccess = mp_aUpperLayer[0]->Receive((unsigned char*) 256);

			mp_UnderLayer->Send((unsigned char*) &m_sHeader,ARP_HEADER_SIZE);
		}
	}

	return bSuccess;
}

// Cache Table
int CARPLayer::CacheAddItem(unsigned char* ppayload)
{
	CACHE_ENTRY cache;
	LPARP_NODE currNode;
	LPARP_NODE newNode = (LPARP_NODE)malloc(sizeof(ARP_NODE));

	if(ppayload == NULL)
	{
		// Cache entry ��� ����
		cache.cache_enetaddr = m_sHeader.arp_dstenet;
		cache.cache_ipaddr = m_sHeader.arp_dstip;
		cache.cache_type = CACHE_INCOMPLETE;
		cache.cache_ttl = CACHE_TTL;
	}
	else
	{
		PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
		cache.cache_enetaddr = pFrame->arp_srcenet;
		cache.cache_ipaddr = pFrame->arp_srcip;
		cache.cache_type = CACHE_COMPLETE;
		cache.cache_ttl = CACHE_TTL;
	}	
	newNode->key = cache;
	newNode->next = NULL;

	// IP �ߺ� �˻�
	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,newNode->key.cache_ipaddr.addrs_i,4))
		{
			return -1;
		}
		currNode = currNode->next;
	}
	if(!memcmp(m_sHeader.arp_dstip.addrs_i,m_sHeader.arp_srcip.addrs_i,4))
	{
		return -1;
	}

	// Cache entry�� �߰�
	currNode = cache_head;
	if(cache_head == NULL)
	{
		newNode->next = cache_head;
		cache_head = newNode;
	}
	else
	{
		while(currNode->next!=NULL)
			currNode = currNode->next;
		newNode->next = currNode->next;
		currNode->next = newNode;
	}

	return newNode->key.cache_ipaddr.addr_i3;
}

void CARPLayer::CacheDeleteItem(int index)
{
	LPARP_NODE currNode;
	LPARP_NODE tempNode;

	int nodeIndex = 0;

	// Cache entry���� ����
	currNode = cache_head;
	if(index==0)
	{
		tempNode = cache_head;
		cache_head = cache_head->next;
		free(tempNode);
	}
	else
	{
		index--;
		while(currNode->next!=NULL){
			if(index == nodeIndex)
				break;
			currNode = currNode->next;
			nodeIndex++;
		}
		tempNode = currNode->next;
		currNode->next = tempNode->next;
		free(tempNode);
	}
}

void CARPLayer::CacheAllDelete()
{
	// Cache entry ��� ����
	LPARP_NODE tempNode;
	while(cache_head!=NULL)
	{
		tempNode = cache_head;
		cache_head = cache_head->next;
		free(tempNode);
	}
}

void CARPLayer::CacheDeleteByTimer(int tID)
{
	LPARP_NODE currNode;
	LPARP_NODE tempNode;

	int ipIndex = 0;

	// Cache entry���� ����
	currNode = cache_head;
	if(currNode==NULL)
		return;

	if(currNode->next==NULL)
	{
		tempNode = cache_head;
		cache_head = cache_head->next;
		free(tempNode);
	}
	else
	{
		while(currNode->next!=NULL){
			if(currNode == cache_head && currNode->key.cache_ipaddr.addr_i3==tID)
			{
				tempNode = currNode;
				currNode = tempNode->next;
				free(tempNode);
				cache_head = currNode;
				break;
			}
			if(currNode->next->key.cache_ipaddr.addr_i3==tID)
			{
				tempNode = currNode->next;
				currNode->next = tempNode->next;
				free(tempNode);
				break;
			}		
			currNode = currNode->next;
		}
	}
}

int CARPLayer::CacheUpdate(unsigned char* ppayload)
{
	// Cache table ����
	// Cache table�� Mac address�� �����ϰ�, Ÿ�̸� ��ȣ�� return���ش�.
	PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
	LPARP_NODE currNode;

	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,pFrame->arp_srcip.addrs_i,4))
		{
			// Update ethernet address and status
			memcpy(currNode->key.cache_enetaddr.addrs_e,pFrame->arp_srcenet.addrs_e,6);
			currNode->key.cache_type = CACHE_COMPLETE;
			break;
		}
		currNode = currNode->next;
	}
	if(currNode==NULL) // ���� ip�ּҰ� Cache table�� ���� ��� : ������ ARP�� ��û�� ���
		return pFrame->arp_srcip.addr_i3+255;
	return currNode->key.cache_ipaddr.addr_i3;
}

BOOL CARPLayer::CacheSearchExist(IP_ADDR ip)
{
	// Cache table�� ã���� �ϴ� ip�� �ִ��� �˻�
	// ã���� TRUE, ��ã���� FALSE
	LPARP_NODE currNode;
	
	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,ip.addrs_i,4))
		{
			return TRUE;
		}
		currNode = currNode->next;
	}
	return FALSE;
}

CACHE_ENTRY CARPLayer::GetSelectCacheEntry(IP_ADDR ip)
{
	// Cache table�� �ִ� etnry�� return.
	LPARP_NODE currNode;

	currNode = cache_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.cache_ipaddr.addrs_i,ip.addrs_i,4))
		{
			if(currNode->key.cache_type == CACHE_COMPLETE)
			{
				return currNode->key;
			}
		}
		currNode = currNode->next;
	}
	memset(currNode->key.cache_enetaddr.addrs_e,0,6);
	return currNode->key;
}

// Proxy Table
void CARPLayer::ProxyAddItem(PROXY_ENTRY proxy)
{
	// Proxy table�� �߰�
	LPPARP_NODE currNode;
	LPPARP_NODE newNode = (LPPARP_NODE)malloc(sizeof(PARP_NODE));

	newNode->key = proxy;
	newNode->next = NULL;

	// IP �ߺ� �˻�
	currNode = proxy_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.proxy_ipaddr.addrs_i,newNode->key.proxy_ipaddr.addrs_i,4))
		{
			return;
		}
		currNode = currNode->next;
	}

	// Proxy entry�� �߰�
	currNode = proxy_head;
	if(proxy_head == NULL)
	{
		newNode->next = proxy_head;
		proxy_head = newNode;
	}
	else
	{
		while(currNode->next!=NULL)
			currNode = currNode->next;
		newNode->next = currNode->next;
		currNode->next = newNode;
	}
}

void CARPLayer::ProxyDeleteItem(int index)
{
	LPPARP_NODE currNode;
	LPPARP_NODE tempNode;

	int nodeIndex = 0;	
	
	// Proxy entry���� ����
	currNode = proxy_head;
	if(index==0)
	{
		tempNode = proxy_head;
		proxy_head = proxy_head->next;
		free(tempNode);
	}
	else
	{
		index--;
		while(currNode->next!=NULL){
			if(index == nodeIndex)
				break;
			currNode = currNode->next;
			nodeIndex++;
		}
		tempNode = currNode->next;
		currNode->next = tempNode->next;
		free(tempNode);
	}
}

BOOL CARPLayer::ProxySearchExist(IP_ADDR ip)
{
	// Proxy table�� ã���� �ϴ� ip�� �ִ��� �˻�
	// ã���� TRUE, ��ã���� FALSE
	LPPARP_NODE currNode;
	
	currNode = proxy_head;
	while(currNode!=NULL)
	{
		if(!memcmp(currNode->key.proxy_ipaddr.addrs_i,ip.addrs_i,4))
		{
			// ��û ���� ip�� proxy table���� ���� ã�� ethernet�� ip �ּҸ� ����
			SetSenderEtherAddress(currNode->key.proxy_enetaddr.addrs_e);
			SetSenderIPAddress(currNode->key.proxy_ipaddr.addrs_i);
			return TRUE;
		}
		currNode = currNode->next;
	}
	return FALSE;
}