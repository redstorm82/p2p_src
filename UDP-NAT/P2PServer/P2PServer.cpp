/* P2P ��������
 * 
 * �ļ�����P2PServer.c
 *
 * ���ڣ�2004-5-21
 *
 * ���ߣ�shootingstars(zhouhuis22@sina.com)
 *
 */

/*  Դ���޸�����:
 *
 *	����޸ĵĴ���������Ϊ�˽�ʡʱ����ԭ����(shootingstars)�Ĵ���Ļ�������
 *  �Ĳ����õġ�ԭ����İ�Ȩ��ԭ�������С����������޸ĵĲ��ֱ��˲����κε�����
 *  ֻ��Ϊ�˷�����ʹ���޸ĺ�Ĵ������UDP��͸������֤���ѡ�
 *
 *  ԭ���ߵĹ���UDP��͸�ļ�������Ͻ����һ�ݶ���UDP��͸������˵�������ǲ�
 *  �����ر���������ϸ�������ڴ���P2P���򿪷��Ĺ�����, ����������͸���˺�
 *  �õ�ʵ�֡����ھ������������ⷽ������⣬��˾������Լ�����ʹ��UDP����
 *  ��͸�ľ���Ҳд�������ҷ����� ϣ�����ڴ����ⷽ�湤����������������.
 *  
 *  Դ���޸��߼��:
 *
 *  Hwycheng Leo, like C/C++/Python/Perl, hate Java. 
 *  һֱʹ��C/C++���Դ�������������Ϳͻ��˷���Ŀ���������������������P2P��������
 *  ��������ȫ���ǿ���BitTorrent�������� - FlashBT(��̬�쳵). �����ڹ�˾���𿪷�
 *  P2P + IM ����ҵ��ƽ̨��
 *  
 *  �ʼ�/MSN: FlashBT@Hotmail.com           [��ӭ�Ȱ������̺�P2P���������Ž���]
 *  ������ҳ: http://www.hwysoft.com/chs/   [FlashBT(��̬�쳵)�Ĺٷ���ҳ]
 *  ����Blog: http://hwycheng.blogchina.com [��Ҫ���Լ�д��һЩ�������º��������]
 */

#pragma comment(lib, "ws2_32.lib")

#include "windows.h"
#include "..\proto.h"
#include "..\Exception.h"


USHORT g_nServerPort = SERVER_PORT;

UserList ClientList;

void InitWinSock()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Windows sockets 2.2 startup");
		throw Exception("");
	}
	else{
		printf("Using %s (Status: %s)\n",
			wsaData.szDescription, wsaData.szSystemStatus);
		printf("with API versions %d.%d to %d.%d\n\n",
			LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion),
			LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));
		
	}
}

SOCKET mksock(int type)
{
	SOCKET sock = socket(AF_INET, type, 0);
	if (sock < 0)
	{
        printf("create socket error");
		throw Exception("");
	}
	return sock;
}

stUserListNode GetUser(char *username)
{
	for(UserList::iterator UserIterator=ClientList.begin();
						UserIterator!=ClientList.end();
							++UserIterator)
	{
		if( strcmp( ((*UserIterator)->userName), username) == 0 )
			return *(*UserIterator);
	}
	throw Exception("not find this user");
}

int testNATProp()
{
	try{
		InitWinSock();
		
		SOCKET PrimaryUDP;
		PrimaryUDP = mksock(SOCK_DGRAM);

		sockaddr_in local;
		local.sin_family=AF_INET;
		local.sin_port= htons(g_nServerPort); 
		local.sin_addr.s_addr = htonl(INADDR_ANY);
		int nResult=bind(PrimaryUDP,(sockaddr*)&local,sizeof(sockaddr));
		if(nResult==SOCKET_ERROR)
			throw Exception("bind error");

		sockaddr_in sender;
		char recvbuf;
		memset(&recvbuf,0,sizeof(char));

		// ��ʼ��ѭ��.
		// ��ѭ���������漸������:
		// һ:��ȡ�ͻ��˵�½�͵ǳ���Ϣ,��¼�ͻ��б�
		// ��:ת���ͻ�p2p����
		for(;;)
		{
			int dwSender = sizeof(sender);
			int ret = recvfrom(PrimaryUDP, (char *)&recvbuf, sizeof(char), 0, (sockaddr *)&sender, &dwSender);
			if(ret <= 0)
			{
				printf("recv error");
				continue;
			}


			//  ������û�����Ϣ��¼���û��б���
			printf("Receive message from: %s:%ld\n", inet_ntoa( sender.sin_addr ), ntohs(sender.sin_port) );
		}
	}
	catch(Exception &e)
	{
		printf(e.GetMessage());
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[])
{
//	testNATProp();
// 	return 0;

	if ( argc > 1 )
	{
		g_nServerPort = atoi( argv[ 1 ] );
	}

	try{
		InitWinSock();
		
		SOCKET PrimaryUDP;
		PrimaryUDP = mksock(SOCK_DGRAM);

		sockaddr_in local;
		local.sin_family=AF_INET;
		local.sin_port= htons(g_nServerPort); 
		local.sin_addr.s_addr = htonl(INADDR_ANY);
		int nResult=bind(PrimaryUDP,(sockaddr*)&local,sizeof(sockaddr));
		if(nResult==SOCKET_ERROR)
			throw Exception("bind error");

		sockaddr_in sender;
		stMessage recvbuf;
		memset(&recvbuf,0,sizeof(stMessage));

		// ��ʼ��ѭ��.
		// ��ѭ���������漸������:
		// һ:��ȡ�ͻ��˵�½�͵ǳ���Ϣ,��¼�ͻ��б�
		// ��:ת���ͻ�p2p����
		for(;;)
		{
			int dwSender = sizeof(sender);
			int ret = recvfrom(PrimaryUDP, (char *)&recvbuf, sizeof(stMessage), 0, (sockaddr *)&sender, &dwSender);
			if(ret <= 0)
			{
				printf("recv error");
				continue;
			}
			else
			{
				int messageType = recvbuf.iMessageType;
				switch(messageType){
				case LOGIN:
					{
						//  ������û�����Ϣ��¼���û��б���
						stUserListNode *currentuser = new stUserListNode();
						strcpy(currentuser->userName, recvbuf.message.loginmember.userName);
						currentuser->ip = ntohl(sender.sin_addr.S_un.S_addr);
						currentuser->port = ntohs(sender.sin_port);

						BOOL bFound = FALSE;
						for(UserList::iterator UserIterator=ClientList.begin();
							UserIterator!=ClientList.end();
							++UserIterator)
						{
							if( strcmp( ((*UserIterator)->userName), recvbuf.message.loginmember.userName) == 0 )
							{
								bFound = TRUE;
								break;
							}
						}
						
						if ( !bFound )
						{
							printf("has a user login : %s <-> %s:%ld\n", recvbuf.message.loginmember.userName, inet_ntoa( sender.sin_addr ), ntohs(sender.sin_port) );
							ClientList.push_back(currentuser);
						}
						
						// �����Ѿ���½�Ŀͻ���Ϣ
						int nodecount = (int)ClientList.size();
						sendto(PrimaryUDP, (const char*)&nodecount, sizeof(int), 0, (const sockaddr*)&sender, sizeof(sender));
						for( UserList::iterator UserIterator=ClientList.begin();
								UserIterator!=ClientList.end();
								++UserIterator)
						{
							sendto(PrimaryUDP, (const char*)(*UserIterator), sizeof(stUserListNode), 0, (const sockaddr*)&sender, sizeof(sender)); 
						}
								
						printf("send user list information to: %s <-> %s:%ld\n", recvbuf.message.loginmember.userName, inet_ntoa( sender.sin_addr ), ntohs(sender.sin_port) );

						break;
					}
				case LOGOUT:
					{
						// ���˿ͻ���Ϣɾ��
						printf("has a user logout : %s <-> %s:%ld\n", recvbuf.message.logoutmember.userName, inet_ntoa( sender.sin_addr ), ntohs(sender.sin_port) );
						UserList::iterator removeiterator;
						for(UserList::iterator UserIterator=ClientList.begin();
							UserIterator!=ClientList.end();
							++UserIterator)
						{
							if( strcmp( ((*UserIterator)->userName), recvbuf.message.logoutmember.userName) == 0 )
							{
								removeiterator = UserIterator;
								break;
							}
						}
						if(*removeiterator != NULL)
							ClientList.remove(*removeiterator);
						break;
					}
				case P2PTRANS:
					{
						// ĳ���ͻ�ϣ�������������һ���ͻ�����һ������Ϣ
						printf("%s:%ld wants to p2p %s\n",inet_ntoa(sender.sin_addr), ntohs(sender.sin_port), recvbuf.message.translatemessage.userName );
						stUserListNode node = GetUser(recvbuf.message.translatemessage.userName);
						sockaddr_in remote;
						remote.sin_family=AF_INET;
						remote.sin_port= htons(node.port); 
						remote.sin_addr.s_addr = htonl(node.ip);

						in_addr tmp;
						tmp.S_un.S_addr = htonl(node.ip);

						stP2PMessage transMessage;
						transMessage.iMessageType = P2PSOMEONEWANTTOCALLYOU;
						transMessage.iStringLen = ntohl(sender.sin_addr.S_un.S_addr);
						transMessage.Port = ntohs(sender.sin_port);
                        
						sendto(PrimaryUDP,(const char*)&transMessage, sizeof(transMessage), 0, (const sockaddr *)&remote, sizeof(remote));
						printf( "tell %s <-> %s:%d to send p2ptrans message to: %s:%ld\n", 
							recvbuf.message.translatemessage.userName, inet_ntoa(remote.sin_addr), node.port, inet_ntoa(sender.sin_addr), ntohs(sender.sin_port) );

						break;
					}
				
				case GETALLUSER:
					{
						int command = GETALLUSER;
						sendto(PrimaryUDP, (const char*)&command, sizeof(int), 0, (const sockaddr*)&sender, sizeof(sender));

						int nodecount = (int)ClientList.size();
						sendto(PrimaryUDP, (const char*)&nodecount, sizeof(int), 0, (const sockaddr*)&sender, sizeof(sender));

						for(UserList::iterator UserIterator=ClientList.begin();
								UserIterator!=ClientList.end();
								++UserIterator)
						{
							sendto(PrimaryUDP, (const char*)(*UserIterator), sizeof(stUserListNode), 0, (const sockaddr*)&sender, sizeof(sender)); 
						}

						printf("send user list information to: %s <-> %s:%ld\n", recvbuf.message.loginmember.userName, inet_ntoa( sender.sin_addr ), ntohs(sender.sin_port) );

						break;
					}
				}
			}
		}

	}
	catch(Exception &e)
	{
		printf(e.GetMessage());
		return 1;
	}

	return 0;
}
