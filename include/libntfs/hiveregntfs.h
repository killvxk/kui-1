#pragma once

#include "hivereg.h"
#include "ntfsimp.h"

START_NAMESPACE_NTFS

class hiveregntfs : public hivereg
{
public:

	hiveregntfs(NtfsImp *pNtfsImp);

	virtual ~hiveregntfs(void);

public:

	virtual int FileOpenCallBack();

	virtual int FileSizeCallBack();

	virtual int FileCloseCallBack();

	virtual int FileReadCallBack(__int64 i64Offset, void *pBuffer, int iSize);

	virtual int FileWriteCallBack(__int64 i64Offset, const void *pBuffer, int iSize);

private:

	NtfsImp *m_pNtfsImp;
};

END_NAMESPACE_NTFS