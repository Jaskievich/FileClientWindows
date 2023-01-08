// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
using namespace std;
//#include "ClientFile.h"


#define ID_LIST_LEFT  10001		
#define ID_LIST_RIGHT 10002

#define IDM_EDIT 40020

struct CDataItemsFileServer final: public CDataItemsFile
{
	
	BOOL Init()
	{
	//	m_clientFile.SetIp_adress("192.168.6.2");
		int bRes = m_clientFile.GetParentPath(parentPath);
		if( bRes )
			return m_clientFile.GetList(vItemFiles);
		return FALSE;
	}

	BOOL Init(const char *ip_adress) 
	{
		m_clientFile.SetIp_adress(ip_adress);
		return Init();
	}

	bool Update() override
	{
		return m_clientFile.GetList(vItemFiles, parentPath.c_str());
	}

	bool UpParentPath() override
	{
		if (parentPath.length() && parentPath.back() == '\\')	parentPath.pop_back();
		int pos = parentPath.find_last_of('\\');
		if (pos > -1) {
			parentPath.erase(pos);
			return m_clientFile.GetList(vItemFiles, parentPath.c_str()) ;
		}
		return false;
	}

	bool DownParentPath(const char *folder) override
	{
		string _parentPath = parentPath;
		if (_parentPath.length() && _parentPath.back() != '\\')	_parentPath.append("\\");
		_parentPath.append(folder);
		if (m_clientFile.GetList(vItemFiles, _parentPath.c_str())) {
			parentPath = _parentPath;
			return true;
		}
		return false;
	}

	bool SetPath(const char *folder) override
	{
		string _parentPath = folder;
		if (m_clientFile.GetList(vItemFiles, _parentPath.c_str())) {
			if (vItemFiles.size()) {
				parentPath = _parentPath;
				return true;
			}
		}
		return false;
	}

	// Копировать файл от сервреа 
	bool CopyFileTo(const char *nameFile, const char *pathTo, CProgressBarCtrl *m_progress ) override
	{
		string path_file_in = parentPath, path_file_out = pathTo;
		path_file_in.append("\\");
		path_file_in.append(nameFile);
		path_file_out.append("\\");
		path_file_out.append(nameFile);
		return m_clientFile.ReciveFile(path_file_in.c_str(), path_file_out.c_str(), m_progress);
	}

	// Копировать файл на сервер
	bool CopyFileFrom(const char *nameFile, const char *pathTo, CProgressBarCtrl *m_progress ) override
	{
		string path_file_in = parentPath, path_file_out = pathTo;
		path_file_in.append("\\");
		path_file_in.append(nameFile);
		path_file_out.append("\\");
		path_file_out.append(nameFile);
		return m_clientFile.SendFile(path_file_in.c_str(), path_file_out.c_str(), m_progress);
	}

	bool ReadFilePart(CFileData &fileData) override 
	{
		string _parentPath = parentPath;
		if (_parentPath.length() && _parentPath.back() != '\\')	_parentPath.append("\\");
		_parentPath.append(fileData.nameFile);
		fileData.SetNameFile(_parentPath.c_str());
		return m_clientFile.ReciveFile(fileData);
	}

	bool FindFiles(const char *path, const char *serch_file, vector<СItemFile>  &_vItemFiles)
	{
		_vItemFiles.clear();
		m_clientFile.GetList(_vItemFiles, serch_file, path);
		return _vItemFiles.size() > 0;
	}

private:

	CClientFile m_clientFile;
};


class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CSplitterWindow			m_splitter;

	CPaneContainer			m_pane_left, m_pane_right;
	
	CView					m_view_left, m_view_right;

	CCommandBarCtrl			m_CmdBar;

	CDataItemsFile			m_dataItemsFile;

	CDataItemsFileServer	m_dataItemsFileServer;

	CProgressBarCtrl		m_ProgressBarCtrl;

	CEdit					m_filtr;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))	return TRUE;

		//	return m_view.PreTranslateMessage(pMsg);
		if (GetFocus() == m_view_left.m_hWnd)
			return m_view_left.PreTranslateMessage(pMsg);
		return m_view_right.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TREEPANE, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		
	//	NOTIFY_CODE_HANDLER(NM_RETURN, OnEnterList)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnConnectServer)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnCopyFile)
		COMMAND_ID_HANDLER(ID_VIEW, OnViewFile)
		COMMAND_ID_HANDLER(ID_FIND_FILE, OnFindFile)
	
		//	COMMAND_ID_HANDLER(ID_VIEW_TREEPANE, OnViewTreePane)
		//	COMMAND_ID_HANDLER(ID_PANE_CLOSE, OnTreePaneClose)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		m_CmdBar.AttachMenu(GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		CreateSimpleStatusBar();

		m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pane_left.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);
		m_pane_left.Create(m_splitter, _T("Сервер:"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_view_left.Create(m_pane_left, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, ID_LIST_LEFT);
		m_view_left.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);

		SetFilterField(m_view_left);
		
		m_pane_left.SetClient(m_view_left);

		m_pane_right.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);
		m_pane_right.Create(m_splitter, _T("Клиент:"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_view_right.Create(m_pane_right, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, ID_LIST_RIGHT);
		m_view_right.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
		SetFilterField(m_view_right);
		
		m_pane_right.SetClient(m_view_right);

		m_splitter.SetSplitterPanes(m_pane_left, m_pane_right);
		UpdateLayout();
		m_splitter.SetSplitterPosPct(50);
		
	
		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);
		UISetCheck(ID_VIEW_TREEPANE, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		BOOL bval = FALSE;
		::SendMessage(hWndToolBar, TB_ENABLEBUTTON, ID_FILE_SAVE, MAKELPARAM(bval, 0));
		::SendMessage(hWndToolBar, TB_ENABLEBUTTON, ID_EDIT_PASTE, MAKELPARAM(bval, 0));
		
		SetColumnToListView(m_view_right);
		SetColumnToListView(m_view_left);

		m_dataItemsFile.Init();

	//	m_dataItemsFileServer.Init();

	//	m_view_right.m_neighbor = m_view_left.m_hWnd;
		m_view_right.p_neighbor_view = &m_view_left;

		m_view_right.p_DataItemsFile = &m_dataItemsFile;
		
		m_view_right.p_pane = &m_pane_right;

	//	m_view_left.m_neighbor = m_view_right.m_hWnd;
		m_view_left.p_neighbor_view = &m_view_right;

		m_view_left.p_DataItemsFile = &m_dataItemsFileServer;

		m_view_left.p_pane = &m_pane_left;

		m_view_right.SetDataToListView();

	//	m_view_left.SetDataToListView();

		m_view_right.SetFocus();


		return 0;
	}

	void SetColumnToListView(CView &listView)
	{
		listView.AddColumn("Имя", 0);
		listView.AddColumn("Размер", 1);
		listView.AddColumn("Дата последней записи", 2);
		listView.SetColumnWidth(0, 300);
		listView.SetColumnWidth(1, 100);
		listView.SetColumnWidth(2, 200);
	}

	void SetFilterField(CView &view)
	{
		CHeaderCtrl header = view.GetHeader();
		LONG_PTR styles = header.GetWindowLongPtr(GWL_STYLE);
		header.SetWindowLongPtr(GWL_STYLE, styles | HDS_FILTERBAR);
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnConnectServer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CDlgConnectServ dlg;
		if (dlg.DoModal())	{
		//	if (m_dataItemsFileServer.Init("192.168.6.2")) {	
			if (m_dataItemsFileServer.Init(dlg.ip_adress)) {
				m_view_left.SetDataToListView();
				m_view_left.SetFocus();
			}
		}
		else {
			if (m_view_left.GetItemCount() > 0) m_view_left.SetFocus();
			else m_view_right.SetFocus();
		}
		
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnViewFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HWND hwndCurr = GetFocus();
		if (hwndCurr == m_view_left.m_hWnd)
			m_view_left.ViewFileItem();
		else if (hwndCurr == m_view_right.m_hWnd)
			m_view_right.ViewFileItem();
		return 0;
	}

	LRESULT OnFindFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HWND hwndCurr = GetFocus();
		if (hwndCurr == m_view_left.m_hWnd)
			m_view_left.FindFileItem();
		else if (hwndCurr == m_view_right.m_hWnd)
			m_view_right.FindFileItem();
		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnCopyFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HWND hwndCurr = GetFocus();
		if (hwndCurr == m_view_left.m_hWnd)
			m_view_left.CopyFileListView();
		else if(hwndCurr == m_view_right.m_hWnd)
			m_view_right.CopyFileListView();
		return 0;
	}

};
