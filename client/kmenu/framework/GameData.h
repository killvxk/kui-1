
#ifndef __GAME_DATA_H__
#define __GAME_DATA_H__

#include "stdafx.h"
#include "OpenHMI.h"
#include "dbmgr.h"

struct fileInfo 
{
	CString strName;
	CString strSize;
	CString strDir;
	bool	bSel;
	fileInfo() {}
	fileInfo(const CString& name, const CString& size, const CString& dir) { strName = name; strSize = size; strDir = dir; bSel = false; }
	~fileInfo() {}
};

CString AfxUDExpandPath2(const CString& filter, const CString& GamePath, CString& root, CString& match, CString& dir);
void AfxUDGetAllFile2(const CString& filter, OpenHMI::Array2<fileInfo>& info, const CString& root, const CString& match, const CString& dir);

void AfxUDGetAllSaveDir2(const CString& GameName, CString& SaveDir);
void AfxUDCreateDirectory2(const CString& dir);

bool AfxUDCopyFile2(const CString& root, const CString& match, const CString& dir);


#endif

