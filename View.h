// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ClientFile.h"
#include "AboutDlg.h"
#include "FileViewer.h"



struct CDataItemsFile
{
	string parentPath;

	vector<—ItemFile>  vItemFiles;

	CDataItemsFile()
	{
	}

	virtual BOOL Init()
	{
		CCtrlListFile ctrlFile;
		ctrlFile.GetList(vItemFiles);
		parentPath = ctrlFile.szPathParent;
		return TRUE;
	}

	virtual bool Update()
	{
		CCtrlListFile ctrlFile(parentPath.c_str());
		ctrlFile.GetList(vItemFiles);
		return vItemFiles.size() > 0;
	}

	virtual bool UpParentPath()
	{
		if (parentPath.length() && parentPath.back() == '\\')	parentPath.pop_back();
		int pos = parentPath.find_last_of('\\');
		if (pos > -1) {
			parentPath.erase(pos);
			CCtrlListFile ctrlFile(parentPath.c_str());
			ctrlFile.GetList(vItemFiles);
			return vItemFiles.size() > 0;
		}
		return false;
	}

	virtual bool DownParentPath(const char *folder)
	{
		string _parentPath = parentPath;
		if (_parentPath.length() && _parentPath.back() != '\\')	_parentPath.append("\\");
		_parentPath.append(folder);
		CCtrlListFile ctrlFile(_parentPath.c_str());
		ctrlFile.GetList(vItemFiles);
		if (vItemFiles.size() > 0) {
			parentPath = _parentPath;
			return true;
		}
		return false;
	}

	virtual bool SetPath(const char *folder)
	{
		string _parentPath = folder;
		CCtrlListFile ctrlFile(_parentPath.c_str());
		ctrlFile.GetList(vItemFiles);
		if (vItemFiles.size() > 0) {
			parentPath = _parentPath;
			return true;
		}
		return false;
	}

	virtual bool CopyFileTo(const char *nameFile, const char *pathTo, CProgressBarCtrl *m_progress)
	{
		return false;
	}

	virtual bool CopyFileFrom(const char *nameFile, const char *pathTo, CProgressBarCtrl *m_progress)
	{
		return false;
	}

	virtual bool ReadFilePart(CFileData &fileData)
	{
		string _parentPath = parentPath;
		if (_parentPath.length() && _parentPath.back() != '\\')	_parentPath.append("\\");
		_parentPath.append(fileData.nameFile);
		CFile file;
		if (file.Open_r(_parentPath.c_str())) {
			fileData.size_file = file.Size();
			fileData.off = file.Read(fileData.buff, sizeof(fileData.buff) - 1);
			fileData.buff[fileData.off] = 0;
			return true;
		}
		return false;
	}

	virtual bool FindFiles(const char *path, const char *serch_file, vector<—ItemFile>  &_vItemFiles)
	{
		_vItemFiles.clear();
		CCtrlListFile ctrlFile(path);
		ctrlFile.GetList(_vItemFiles, serch_file);
		return _vItemFiles.size() > 0;
	}

};

int CALLBACK SortFunc(LPARAM p1, LPARAM p2, LPARAM type);
int CALLBACK SortFuncName(LPARAM p1, LPARAM p2, LPARAM type);
int CALLBACK SortFuncSize(LPARAM p1, LPARAM p2, LPARAM type);
int CALLBACK SortFuncDate(LPARAM p1, LPARAM p2, LPARAM type);

class CView : public CWindowImpl<CView, CListViewCtrl>
{
public:

	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	//	HWND m_neighbor; // ÒÓÒÂ‰ÌËÈ ˝ÎÂÏÂÌÚ view
	CView *p_neighbor_view;

	CDataItemsFile *p_DataItemsFile;

	CPaneContainer *p_pane;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CView)

		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKey)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnDblclkList)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotifyHeader)

		//	MESSAGE_HANDLER(WM_SIZE, OnSize)
		//	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	CView() : p_DataItemsFile(NULL), p_pane(NULL), p_neighbor_view(NULL)
	{
	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;


		return 0;
	}

	void GetTextFilter(char text[256], int iField)
	{
		CHeaderCtrl h = GetHeader();
		HDITEM pHeaderItem = { 0 };
		pHeaderItem.mask = HDI_FILTER;
		pHeaderItem.type = HDFT_ISSTRING;
		pHeaderItem.fmt = HDF_STRING;

		HDTEXTFILTER hr = { 0 };
		hr.cchTextMax = sizeof(text);
		hr.pszText = text;
		pHeaderItem.pvFilter = &hr;
		// ƒÎˇ ÔÓ‚ÂÍË ÙËÎ¸Ú‡
		::SendMessage(h.m_hWnd, HDM_GETITEMW, iField, (LPARAM)&pHeaderItem);
		if (text[0]) h.GetItem(iField, &pHeaderItem);
	}

	LRESULT OnNotifyHeader(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		LPNMHEADER pNMHeader = (LPNMHEADER)lParam;

		switch (((LPNMHDR)lParam)->code)
		{
			case HDN_ITEMCLICKW:
			{
				SortItemByNField(pNMHeader->iItem);
				break;
			}
			case  HDN_FILTERCHANGE:
			{
				char t[256] = { 0 };
				GetTextFilter(t, pNMHeader->iItem);
				if (t[0])	SetDataToListView(t, pNMHeader->iItem);
				else SetDataToListView();
				break;
			}
			case HDN_FILTERBTNCLICK:
			{
				GetHeader().ClearFilter(pNMHeader->iItem);
				GetHeader().EditFilter(pNMHeader->iItem, TRUE);
				break;
			}
		}
		return 0;
	}



	LRESULT OnKey(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (wParam)
		{
		case VK_RETURN:

			EnterListView();
			break;
		case VK_TAB:

			if (p_neighbor_view) ::SetFocus(p_neighbor_view->m_hWnd);
			break;
		case VK_F5:
			//  ÓÔËÓ‚‡Ú¸ Ù‡ÈÎ ÔÓ ËÏÂÌË
			CopyFileListView();
			break;
		case VK_F3:
			ViewFileItem();
			break;
		case VK_F7:
			FindFileItem();
			break;
		}
		return 0;
	}


	void EnterListView()
	{
		bool bRes = false;
		—ItemFile *item = NULL;
		int index = GetNextItem(-1, LVNI_SELECTED);
		if (index == -1) return;
		item = (—ItemFile *)GetItemData(index);
		if (item->type == 0) { // ‰Îˇ Í‡Ú‡ÎÓ„Ó‚
			if (index == 0 && item->name_file[0] == '.')
				bRes = p_DataItemsFile->UpParentPath();
			else
				bRes = p_DataItemsFile->DownParentPath(item->name_file);
			if (bRes) {
				GetHeader().ClearAllFilters();
				SetDataToListView();
			}
		}
		else { // ‰Îˇ Ù‡ÈÎÓ‚

		}
	}

	LRESULT OnDblclkList(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		EnterListView();
		return 0;
	}


	class CWorkProgressBar : public IProgressBar
	{
	public:

		CWorkProgressBar(CView &_view, —ItemFile *_item) : view(_view), item(_item)
		{
		}

		void work(CProgressBarCtrl &m_progress)
		{
			if (view.p_DataItemsFile->CopyFileTo(item->name_file, view.p_neighbor_view->p_DataItemsFile->parentPath.c_str(), &m_progress)) {
				view.p_neighbor_view->p_DataItemsFile->Update();
				view.p_neighbor_view->SetDataToListView();
			}
			if (view.p_neighbor_view->p_DataItemsFile->CopyFileFrom(item->name_file, view.p_DataItemsFile->parentPath.c_str(), &m_progress)) {
				view.p_DataItemsFile->Update();
				view.SetDataToListView();
			}
		}

	private:

		—ItemFile *item;

		CView &view;
	};


	void CopyFileListView()
	{
		—ItemFile *item = NULL;
		int index = GetNextItem(-1, LVNI_SELECTED);
		if (index == -1) return;
		item = (—ItemFile *)GetItemData(index);
		if (item->type == 1) { // ‰Îˇ Ù‡ÈÎÓ‚

			CWorkProgressBar work(*this, item);

			CDlgProgressBar dlg(&work);
			dlg.DoModal();
			SetFocus();
		}
	}


	class CWorkDlgFileView : public IWorkDlgFileView
	{
	public:

		CWorkDlgFileView(CView &_view, —ItemFile *_item) : view(_view), item(_item)
		{
		}

		bool work(CFileData &data)
		{
			data.SetNameFile(item->name_file);
			return view.p_DataItemsFile->ReadFilePart(data);
		}

	private:

		—ItemFile *item;

		CView &view;
	};

	void ViewFileItem()
	{
		—ItemFile *item = NULL;
		int index = GetNextItem(-1, LVNI_SELECTED);
		if (index == -1) return;
		item = (—ItemFile *)GetItemData(index);
		if (item->type == 1) { // ‰Îˇ Ù‡ÈÎÓ‚
			CWorkDlgFileView work(*this, item);
			CDlgFileView dlg(&work);
			dlg.DoModal();
			SetFocus();
		}
	}


	class —FindFile : public AFindFile
	{
	public:
		—FindFile(CDataItemsFile *_DataItemsFile) :p_DataItemsFile(_DataItemsFile)
		{}

		bool work(CProgressBarCtrl &m_progress, CListBox &listBox) override
		{
			listBox.ResetContent();
			m_progress.StepIt(); // 2
			vector<—ItemFile> vItemFile;
			if (p_DataItemsFile->FindFiles(start_path.c_str(), file_template.c_str(), vItemFile)) {
				m_progress.StepIt(); // 3
				for (int i = 0; i < vItemFile.size(); ++i) {
					listBox.AddString(vItemFile[i].name_file);
				}
				m_progress.StepIt(); // 4
				return true;
			}
			m_progress.StepIt(); // 3
			m_progress.StepIt(); //4
			listBox.AddString("‘‡ÈÎÓ‚ ÌÂ Ì‡È‰ÂÌÓ");
			return false;
		}

	private:

		CDataItemsFile *p_DataItemsFile;
	};

	void FindFileItem()
	{
		—FindFile findFile(p_DataItemsFile);
		findFile.start_path = p_DataItemsFile->parentPath;
		CDlgFind dlg(&findFile);
		if (dlg.DoModal()) {
			char *file_name = strrchr(dlg.file_select, '\\');
			if (file_name) {
				*file_name = 0;
				file_name++;
			}
			else return;
			if (p_DataItemsFile->SetPath(dlg.file_select)) {
				SetDataToListView();
				SetFocus();
				int index = FindItem(file_name);
				SelectItem(index);
				EnsureVisible(index, TRUE);
			}
		}
	}


	void SetDataToListView()
	{
		if (p_DataItemsFile == NULL) return;
		if (p_pane) p_pane->SetTitle(p_DataItemsFile->parentPath.c_str());
		SetRedraw(FALSE);
		DeleteAllItems();
		vector<—ItemFile>  &vItemFiles = p_DataItemsFile->vItemFiles;
		if (vItemFiles.size() == 0) return;
		SYSTEMTIME st;
		char buff[128] = { 0 };
		int ind = 0, i = 0;
		if (vItemFiles[i].name_file[0] == '.' && vItemFiles[i].name_file[1] == '.' ) {
			ind = AddItem(0, 0, vItemFiles[i].name_file);
			vItemFiles[i].GetDate(st);
			TimeToStr(st, buff);
			SetItemText(ind, 2, buff);
			SetItemData(ind, (DWORD_PTR)&vItemFiles[i]);
			i++;
		}
		for (; i < vItemFiles.size(); ++i) {
			if (vItemFiles[i].type) {
				ind = AddItem(i, 0, vItemFiles[i].name_file);
				sprintf_s(buff, "%i", vItemFiles[i].size_file);
				SetItemText(ind, 1, buff);
			}
			else {
				sprintf_s(buff, "[%s]", vItemFiles[i].name_file);
				ind = AddItem(i, 0, buff);
			}
			vItemFiles[i].GetDate(st);
			TimeToStr(st, buff);
			SetItemText(ind, 2, buff);
			SetItemData(ind, (DWORD_PTR)&vItemFiles[i]);
		}
		SortItems(SortFunc, 0);
		SetRedraw(TRUE);
		SelectItem(0);
	}

	void SetDataToListView(const char *filter_str, int iFieldComp = 0)
	{
		if (p_DataItemsFile == NULL || iFieldComp < 0 || iFieldComp > 2  ) return;
		SetRedraw(FALSE);
		DeleteAllItems();
		vector<—ItemFile>  &vItemFiles = p_DataItemsFile->vItemFiles;
		if (vItemFiles.size() == 0) return;
		SYSTEMTIME st;
		char buff[3][128] = { 0 };
		char *field[3] = { 0 };
		int ind = 0, i = 0;
		if ( vItemFiles[i].name_file[0] == '.' && vItemFiles[i].name_file[1] == '.' ) {
			ind = AddItem(0, 0, vItemFiles[i].name_file);
			vItemFiles[i].GetDate(st);
			TimeToStr(st, buff[2]);
			SetItemText(ind, 2, buff[2]);
			SetItemData(ind, (DWORD_PTR)&vItemFiles[i]);
			i++;
		}
		for (; i < vItemFiles.size(); ++i) {
			if (vItemFiles[i].type) {
				field[0] = vItemFiles[i].name_file;
				sprintf_s(buff[1], "%i", vItemFiles[i].size_file);
				field[1] = buff[1];
			}
			else {
				sprintf_s(buff[0], "[%s]", vItemFiles[i].name_file);
				field[0] = buff[0];
				field[1] = 0;
			}
			vItemFiles[i].GetDate(st);
			TimeToStr(st, buff[2]);
			field[2] = buff[2];
			if (strstr(field[iFieldComp], filter_str) == 0) continue;
			ind = AddItem(i, 0, field[0]);
			if(field[1]) SetItemText(ind, 1, field[1]);
			SetItemText(ind, 2, field[2]);
			SetItemData(ind, (DWORD_PTR)&vItemFiles[i]);
		}
		SortItems(SortFunc, 0);
		SetRedraw(TRUE);
		SelectItem(0);
	}

	void SortItemByNField(int iField)
	{
		switch (iField) {
		case 0:
			SortItems(SortFuncName, 0);
			break;
		case 1:
			SortItems(SortFuncSize, 0);
			break;
		case 2:
			SortItems(SortFuncDate, 0);
			break;
		}
	}

	void TimeToStr(SYSTEMTIME &st, char str[64])
	{
		sprintf_s(str, 63, "%02i.%02i.%i %02i:%02i", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
	}

};


int CALLBACK SortFunc(LPARAM p1, LPARAM p2, LPARAM type)
{
	—ItemFile *item1 = (—ItemFile *)p1;
	—ItemFile *item2 = (—ItemFile *)p2;
	return item1->type > item2->type;
}

int CALLBACK SortFuncName(LPARAM p1, LPARAM p2, LPARAM type)
{
	—ItemFile *item1 = (—ItemFile *)p1;
	—ItemFile *item2 = (—ItemFile *)p2;
	return item1->type > item2->type || item1->type == item2->type && strcmp(item1->name_file, item2->name_file) > 0;
}

int CALLBACK SortFuncSize(LPARAM p1, LPARAM p2, LPARAM type)
{
	—ItemFile *item1 = (—ItemFile *)p1;
	—ItemFile *item2 = (—ItemFile *)p2;
	return item1->type != 0 && item2->type != 0 && item1->size_file < item2->size_file;
}

inline BOOL EQ_dot_dot(const char *name)
{
	return name[0] == '.' && name[1] == '.';
}

int CALLBACK SortFuncDate(LPARAM p1, LPARAM p2, LPARAM type)
{
	—ItemFile *item1 = (—ItemFile *)p1;
	—ItemFile *item2 = (—ItemFile *)p2;
	if (EQ_dot_dot(item1->name_file)) return 0;
	return  item1->type > item2->type || item1->type == item2->type && item1->date < item2->date;
}
