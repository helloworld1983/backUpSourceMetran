//$COMMON.H$
//    File Name:  BOOTFILE.H
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class:  BootFile
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  08/06/98 	blumee				Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  BOOTFILE_H
#define	 BOOTFILE_H

#include "afxcoll.h"
#include "standard.h"
const SHORT MAX_FILE_NAME_LENGTH = 200;

class BootFile 
{
  public:
    BootFile();
    ~BootFile(void);
	E_ReturnStatus Open( const TCHAR * filename);
	E_ReturnStatus Creat( const TCHAR * filename);
	inline E_Bool Opened() { if ( PFileId == NULL ) { return eFalse;} else { return eTrue; }}
	inline char *GetFileName() { return FileName; }
	LONG GetFileSize();
	virtual LONG Read(UCHAR *pBuf, LONG size);
    virtual LONG Read(UCHAR *pData, LONG offSet, LONG size);
	virtual LONG Write(UCHAR *pBuf, LONG size);
	virtual void  Close(void);
	inline FILE *GetFileId() { return PFileId; }
	inline LONG GetCurrentOffSet() { return CurrentOffSet; };
    E_ReturnStatus BootFile::SetOffSet(LONG offSet);

  private:

	// File descriptor to the Download image file.
		FILE  *PFileId;

	// An absolute file name which contains a full path where the file is.
		TCHAR FileName[MAX_FILE_NAME_LENGTH];

	// current offset to this boot file where the next read data is.
		LONG CurrentOffSet;

	// size of this Boot file.
		LONG FileSize;

};

#endif // BOOTFILE_H 
