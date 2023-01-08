#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include "../ServerPool//MyFileServer.h"


#pragma comment (lib, "Ws2_32.lib")

//#define DEFAULT_BUFLEN 32*1024
#define DEFAULT_PORT 5001

class CClientTCP
{
public:

	CClientTCP()
	{
		ZeroMemory(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(DEFAULT_PORT);
	}

	CClientTCP(const char *ip_serv)
	{
		ZeroMemory(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr(ip_serv);
		servaddr.sin_port = htons(DEFAULT_PORT);
	}
	~CClientTCP()
	{
		Close();
	}

	int Init()
	{
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}
		return 0;
	}

	void SetIp(const char *ip_serv)
	{
		servaddr.sin_addr.s_addr = inet_addr(ip_serv);
		//return Connect();
	}

	int Connect()
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1) return 1;
		// connect the client socket to server socket 
		if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
			return 1;
		}
		return 0;
	}

	int Connect(TSocketData &socketData)
	{
		if (!Connect()) {
			socketData.ClearHeaderBuffNull();
			socketData.ClientSocket = sockfd;
			return 0;
		}
		return 1;
	}

	/*int SendMess(char *mess, int size)
	{
		int iResult = 0;
		while (DEFAULT_BUFLEN < size) {
			iResult = _SendMess(mess, DEFAULT_BUFLEN);
			if (iResult < 0) break;
			mess += DEFAULT_BUFLEN;
			size -= DEFAULT_BUFLEN;
		}
		return _SendMess(mess, size);;
	}

	int ReadAll(char *p_buff, int size)
	{
		int iResult = 0;
		while (DEFAULT_BUFLEN < size) {
			iResult = _ReadAll(p_buff, size);
			if (iResult <= 0) break;
			p_buff += DEFAULT_BUFLEN;
			size -= DEFAULT_BUFLEN;
		}
		return  _ReadAll(p_buff, size);
	}
*/
	void Close()
	{
		shutdown(sockfd, SD_SEND);
		// cleanup
		closesocket(sockfd);
		WSACleanup();
	}

private:

	//int _SendMess(char *mess, int size)
	//{
	//	int iResult = 0;
	//	while (size) {
	//		iResult = send(sockfd, mess, size, 0);
	//		if (iResult < 0) break;
	//		size -= iResult;
	//		mess += iResult;
	//	}
	//	return iResult;
	//}


	//int _ReadAll(char *p_buff, int size)
	//{
	//	int iResult = 0;
	//	while (size) {
	//		iResult = recv(sockfd, p_buff, size, 0);
	//		if (iResult <= 0) break;
	//		p_buff += iResult;
	//		size -= iResult;
	//	}
	//	return iResult;
	//}

	struct sockaddr_in servaddr/*, cli*/;

	SOCKET sockfd/*, connfd*/;

};


struct CFileEx : CFile
{
	using CFile::Write;

	using CFile::Read;

	int Write(TSocketData& buff_cmd, CProgressBarCtrl *m_progress)
	{
		long size = 0;
		while (buff_cmd.header.size) {
	//		ZeroMemory(buff_cmd.p_buff, DEFAULT_BUFLEN);
			size = buff_cmd.ReadBuff(DEFAULT_BUFLEN);
			if (size < 1) return size;
			fwrite(buff_cmd.p_buff, size, 1, file);
			buff_cmd.header.size -= size;
			m_progress->StepIt();
		}
		return 1;
	}

	int Read(TSocketData& buff_cmd, CProgressBarCtrl *m_progress )
	{
		long size = 0;
		int iRes = 0;
		while (!feof(file)) {
		//	ZeroMemory(buff_cmd.p_buff, DEFAULT_BUFLEN);
			size = fread_s(buff_cmd.p_buff, DEFAULT_BUFLEN, sizeof(char), DEFAULT_BUFLEN, file);
			if (size == 0) break;
			iRes = buff_cmd.SendBuff(size);
			if (iRes < 1) break;
			m_progress->StepIt();
		}
		return iRes;
	}
};



struct CFileData
{
	char nameFile[MAX_PATH];
	unsigned long long off;
	unsigned long long size_file;
	char buff[DEFAULT_BUFLEN];

	CFileData() :off(0), size_file(0)
	{
		ZeroMemory(buff, DEFAULT_BUFLEN);
		nameFile[0] = 0;
	}

	CFileData(const char *_nameFile) :off(0), size_file(0)
	{
		ZeroMemory(buff, DEFAULT_BUFLEN);
		SetNameFile(_nameFile);
	}

	void SetNameFile(const char *_nameFile)
	{
		strncpy_s(nameFile, _nameFile, MAX_PATH - 1);
		nameFile[MAX_PATH - 1] = 0;
	}

	bool IsEndFile()
	{
		return off == size_file;
	}

	void ClearBuff()
	{
		ZeroMemory(buff, DEFAULT_BUFLEN);
	}
};


class CClientFile
{
public:

	CClientFile()
	{
		m_clientTCP.Init();
		ZeroMemory(buff, DEFAULT_BUFLEN);
	}

	void SetIp_adress(const char *ip_adress)
	{
		m_clientTCP.SetIp(ip_adress);
	}

	BOOL GetList(vector<ÑItemFile> & vItemFiles, const char *parentPath = NULL)
	{
		vItemFiles.clear();
		TSocketData socketData;
		if (m_clientTCP.Connect(socketData)) return FALSE;
		socketData.header.cmd = E_LIST_FILE;
		if (parentPath) {
			socketData.header.size = strlen(parentPath);
			socketData.p_buff = const_cast<char*>(parentPath);
		}
		return _GetList(vItemFiles, socketData);
	}

	BOOL GetList(vector<ÑItemFile> & vItemFiles, const char *search_file, const char *parentPath )
	{
		vItemFiles.clear();
		TSocketData socketData;
		if (m_clientTCP.Connect(socketData)) return FALSE;
		socketData.header.cmd = E_FIND_FILE;
		ZeroMemory(buff, DEFAULT_BUFLEN);
		socketData.p_buff = buff;
		int size = strlen(parentPath);
		memcpy(socketData.p_buff, &size, sizeof(int));
		socketData.p_buff += sizeof(int);
		socketData.header.size = sizeof(int);
		memcpy(socketData.p_buff , const_cast<char*>(parentPath), size);
		socketData.p_buff += size;
		socketData.header.size += size;
		size = strlen(search_file);
		memcpy(socketData.p_buff, &size, sizeof(int));
		socketData.p_buff += sizeof(int);
		socketData.header.size += sizeof(int);
		memcpy(socketData.p_buff, const_cast<char*>(search_file), size);
		socketData.header.size += size;
		socketData.p_buff = buff;
		return _GetList(vItemFiles, socketData);
	}

	BOOL _GetList(vector<ÑItemFile> & vItemFiles, TSocketData socketData)
	{
		int iRes = FALSE;
		if (socketData.SendHeader()) {
			if (socketData.header.size) iRes = socketData.SendBuff();
			iRes = socketData.ReadHeader();
			if (iRes) {
				int n = socketData.header.size / sizeof(ÑItemFile);
				vItemFiles.resize(n);
				socketData.p_buff = (char *)&vItemFiles[0];
				unsigned long size = socketData.header.size;
				return socketData.ReadAll(size);
			}
		}
		return FALSE;
	}

	BOOL GetParentPath(string &res)
	{
		TSocketData socketData;
		if (m_clientTCP.Connect(socketData)) return FALSE;
		socketData.p_buff = buff;
		socketData.header.cmd = E_DIR_PARENT;
		if (socketData.SendHeader()) {
			int iRes = socketData.ReadHeader();
			if (iRes) {
				ZeroMemory(buff, DEFAULT_BUFLEN);
				if (socketData.ReadBuff(socketData.header.size)) {
					res = socketData.p_buff;
				}
			}
		}
		return TRUE;
	}

	BOOL ReciveFile(const char *path_file_in, const char *path_file_out, CProgressBarCtrl *m_progress = NULL)
	{
		TSocketData socketData;
		if (m_clientTCP.Connect(socketData)) return FALSE;
		socketData.header.size = strlen(path_file_in);
		socketData.p_buff = const_cast<char*>(path_file_in);
		socketData.header.cmd = E_FILE_SERVER_FROM;
		int iRes = socketData.SendHeader();
		if (iRes < 1) return FALSE;
		iRes = socketData.SendBuff();
		if (iRes < 1) return FALSE;
		iRes = socketData.ReadHeader();
		if (iRes) {
			if (m_progress) {
				SettingProgressBarBySize(m_progress, socketData.header.size);
				m_progress->StepIt();
			}
			CFileEx curr_file;
			if (curr_file.Open_w(path_file_out)) {
				ZeroMemory(buff, DEFAULT_BUFLEN);
				socketData.p_buff = buff;
				if (m_progress) iRes = curr_file.Write(socketData, m_progress); else	iRes = curr_file.Write(socketData);
				return iRes > 0;
			}
		}
		return FALSE;
	}

	BOOL ReciveFile(CFileData &fileData)
	{
		fileData.ClearBuff();
		TSocketData socketData;
		if (m_clientTCP.Connect(socketData)) return FALSE;
		socketData.header.size = strlen(fileData.nameFile);
		socketData.p_buff = fileData.nameFile;
		socketData.header.cmd = E_FILE_SERVER_FROM_PART;
		socketData.header.param = fileData.off;
		int iRes = socketData.SendHeader();
		if (iRes < 1) return FALSE;
		iRes = socketData.SendBuff();
		if (iRes < 1) return FALSE;
		iRes = socketData.ReadHeader();
		if (iRes) {
			fileData.size_file = socketData.header.size;
			socketData.p_buff = fileData.buff;
			long size = socketData.ReadBuff(DEFAULT_BUFLEN);
			if (size > 0) {
				fileData.off += size;
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL SendFile(const char path_file_in[MAX_PATH], const char path_file_out[MAX_PATH], CProgressBarCtrl *m_progress = NULL)
	{
		TSocketData socketData;
		if (m_clientTCP.Connect(socketData)) return FALSE;
		socketData.header.cmd = E_FILE_SERVER_TO;
		int iRes = 0;
		CFileEx curr_file;
		if (curr_file.Open_r(path_file_out)) {
			socketData.header.size = curr_file.Size();
			ZeroMemory(buff, DEFAULT_BUFLEN);
			int m = strlen(path_file_in);
			socketData.p_buff = buff;
			*((int *)socketData.p_buff) = m;
			socketData.p_buff += sizeof(int);
			memmove(socketData.p_buff, path_file_in, m);
			socketData.p_buff += m;
			unsigned long size = DEFAULT_BUFLEN - sizeof(int) - m;
			size = curr_file.Read(socketData.p_buff, size);
			iRes = socketData.SendHeader();
			if( iRes < 1) return FALSE;
			socketData.p_buff = buff;
			iRes = socketData.SendBuff(sizeof(int) + m + size);
			if (iRes < 1) return FALSE;
			if (size < socketData.header.size) {
				if (m_progress) {
					SettingProgressBarBySize(m_progress, socketData.header.size);
					m_progress->StepIt();
				}
				ZeroMemory(buff, DEFAULT_BUFLEN);
				socketData.p_buff = buff;
				if( m_progress) return curr_file.Read(socketData, m_progress);
				return curr_file.Read(socketData);
			}
		}
		return iRes > 0;
	}

private:

	void SettingProgressBarBySize(CProgressBarCtrl *m_progress, unsigned long long size)
	{
		long r0 = DEFAULT_BUFLEN;
		long r = size / r0 + 1;
		m_progress->SetRange(0, r);
		m_progress->SetPos(0);
		m_progress->SetStep(1);
	}

	CClientTCP m_clientTCP;

	char buff[DEFAULT_BUFLEN];
};