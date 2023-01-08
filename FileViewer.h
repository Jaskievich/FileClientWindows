#pragma once


class IWorkDlgFileView
{
public:

	virtual bool work(CFileData &data) = 0;

};


class CDlgFileView : public CDialogImpl<CDlgFileView>, public CDialogResize<CDlgFileView>
{
public:
	enum { IDD = IDD_DIALOG_VIEW };

	CFileData			m_FileData;

	CRichEditCtrl		m_edit;

	IWorkDlgFileView	*p_workDlgFileView;

	BEGIN_DLGRESIZE_MAP(CDlgFileView)
	
		DLGRESIZE_CONTROL(IDC_RICHEDIT21, DLSZ_SIZE_X | DLSZ_SIZE_Y)

	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CDlgFileView)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<CDlgFileView>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	CDlgFileView(IWorkDlgFileView *_workDlgFileView):p_workDlgFileView(_workDlgFileView)
	{
	}
	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init();
		CenterWindow(GetParent());
		m_edit = GetDlgItem(IDC_RICHEDIT21);
		if (p_workDlgFileView) p_workDlgFileView->work(m_FileData);
		if (m_FileData.nameFile[0]) SetWindowText(m_FileData.nameFile);
		if (m_FileData.buff[0]) {
			m_edit.AppendText(m_FileData.buff);
			m_edit.PostMessage(EM_SETSEL, -1, 0);
		}			
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};
