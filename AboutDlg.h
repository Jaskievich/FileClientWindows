// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};


class IProgressBar
{
public:

	virtual void work(CProgressBarCtrl &m_progress) = 0;

	virtual ~IProgressBar(){}

};


class CDlgProgressBar : public CDialogImpl<CDlgProgressBar>
{
public:
	enum { IDD = IDD_DIALOG_PROGRESS};

	CProgressBarCtrl m_progress;

	IProgressBar *p_ProgressBarWork;

	BEGIN_MSG_MAP(CDlgProgressBar)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	CDlgProgressBar(IProgressBar *_progressBar):p_ProgressBarWork(_progressBar)
	{
	}

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		m_progress = GetDlgItem(IDC_PROGRESS1);
		/*m_progress.SetRange(0, 100);
		m_progress.SetPos(0);
		m_progress.SetStep(1);*/

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{	
		if (wID == IDOK) {
			if (p_ProgressBarWork) p_ProgressBarWork->work(m_progress);
		}
		EndDialog(wID);
		return 0;
	}
};


class CDlgConnectServ : public CDialogImpl<CDlgConnectServ>
{
public:
	enum { IDD = IDD_DIALOG_CONNECT };

	char ip_adress[64];
	
	BEGIN_MSG_MAP(CDlgConnectServ)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_CHECK1, OnCheck)
	END_MSG_MAP()

	CDlgConnectServ() 
	{
		ZeroMemory(ip_adress, 64);
	}

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		CheckDlgButton(IDC_CHECK1, TRUE);	
		::SetFocus(GetDlgItem(IDC_EDIT1));
	//	SendMessage(GetDlgItem(IDC_CHECK1), BM_CLICK, 0, 0);
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int iRes = 0;
		if (wID == IDOK) {
			iRes = 1;
			GetDlgItemText(IDC_EDIT1, ip_adress, 63);
		}
		EndDialog(iRes);
		return 0;
	}

	LRESULT OnCheck(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL isCheck = IsDlgButtonChecked(IDC_CHECK1);
		::EnableWindow(GetDlgItem(IDC_EDIT1), isCheck);
		::EnableWindow(GetDlgItem(IDC_COMBO1), !isCheck);
		return 0;
	}
};



class AFindFile
{
public:
	
	string start_path;

	string file_template;

	virtual bool work( CProgressBarCtrl &m_progress, CListBox &listBox) = 0;

	virtual ~AFindFile(){}

};

/*
	Диалоговое окно поиска
*/
class CDlgFind : public CDialogImpl<CDlgFind>, public CDialogResize<CDlgFind>
{
public:
	enum { IDD = IDD_DIALOG_FIND };

	AFindFile *findFile;

	char file_select[MAX_PATH];

	CListBox m_listBox;

	CProgressBarCtrl m_progress;

	bool isFindFile ;
	
	BEGIN_DLGRESIZE_MAP(CDlgFind)

	//	DLGRESIZE_CONTROL(IDC_RICHEDIT21, DLSZ_SIZE_X | DLSZ_SIZE_Y)

	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CDlgFind)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDOK2, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<CDlgFind>)
	END_MSG_MAP()

	CDlgFind(AFindFile *_findFile )	:findFile(_findFile)
	{
		file_select[0] = 0;
		isFindFile = false;
	}

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init();
		CenterWindow(GetParent());
		if (findFile->start_path.length())
			SetDlgItemText(IDC_EDIT1, findFile->start_path.c_str());
		m_listBox = GetDlgItem(IDC_LIST1);
		m_progress = GetDlgItem(IDC_PROGRESS1);
		::SetFocus(GetDlgItem(IDC_EDIT2));
		const int n = 4; // Количество действий для поиска файлов
		m_progress.SetRange(0, n);
		m_progress.SetPos(0);
		m_progress.SetStep(1);
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int iRes = 0;
		if (wID == IDOK2) {
			char name_file[MAX_PATH] = { 0 };
			GetDlgItemText(IDC_EDIT2, name_file, MAX_PATH - 1);
			m_progress.StepIt(); // 1
			findFile->file_template = name_file;
			isFindFile = findFile->work(m_progress, m_listBox);
			return 0;
		}
		if (wID == IDOK && isFindFile) {
			iRes = 1;
			int index = m_listBox.GetCurSel();
			if (index > -1) {
				int len = m_listBox.GetTextLen(index);
				if (len <= MAX_PATH) 
					m_listBox.GetText(index, file_select);
			}
		}
		EndDialog(iRes);
		return 0;
	}
};