#define BOOTFILE_CPP
#include "stdafx.h"
#include "standard.h"
#include "resource.h"
#include "BootFile.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include "stdio.h"


//******************************************************************************/
//    Operation Name: BootFile()
//
//    Processing: The default constructor. Initialize attributes.
//                Leave the file not opened();
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

BootFile::BootFile()
{
    PFileId = NULL;
    FileName[0] = '\0';
    CurrentOffSet= 0;
    FileSize = 0;
}


//******************************************************************************/
//    Operation Name: ~BootFile()
//
//    Processing:  The destructor. If the file is opened, close the file.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

BootFile::~BootFile(void)
{
    if ( Opened() )
        Close();
}


//******************************************************************************/
//    Operation Name: Close()
//
//    Processing:  Close the file if it is still opened. And null-out the
//                 boot file name.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

void
BootFile::Close( void )
{
    FileName[0] = '\0';
    if ( Opened() )
        ::fclose(PFileId);
    PFileId = NULL;
    CurrentOffSet = 0;
    FileSize = 0;
}

//******************************************************************************/
//    Operation Name: Open(CHAR *fileName)
//
//    Processing: 
//        Opens a given file as a read only mode. 
//        The method also finds out total size of the open file.
//        The code employs the fseek routine
//        to find out a file size. If the fseek is called to move the
//        file offset to an end of file, it returns the file size in bytes. 
//
//    Input Parameters: fileName -- name of the file to be opened.
//
//    Output Parameters:
//
//    Return Values:
//        eError  -- if the given file cannot be successfully opened. it returns the
//
//    Pre-Conditions: (!Opened())
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

E_ReturnStatus
BootFile::Open( const TCHAR *fileName)
{

    PRE_CONDITION(!Opened());

    if ( PFileId = ::_tfopen(fileName, "rb") ) // Always look at the current
                                               // working directory first.
    {
        _tcscpy(FileName, fileName);
        FileSize = GetFileSize();
        CurrentOffSet = 0;
        return(eSuccess);
    }
    return(eError);
}

//******************************************************************************/
//    Operation Name: Creat(CHAR *fileName)
//
//    Processing: 
//      Creates a given filename in the current working directory.
//      If the file already exist, the file is opened for writing and the file
//      is truncated to zero.
//
//    Input Parameters: fileName - a file name.
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
E_ReturnStatus
BootFile::Creat( const TCHAR *fileName )
{
    if (!( PFileId  = ::_tfopen( fileName, "wb")))
        return(eError);
    strcpy(FileName, fileName);
    return(eSuccess);
}

//******************************************************************************/
//    Operation Name: Write(CHAR pBuf, LONG nbytes)
//
//    Processing: 
//      Writes 'nbytes' of the given data to this file.
//
//    Input Parameters: pBuf -- points where the data to be written out.
//                      nbytes -- number of bytes to be written out.
//
//    Output Parameters:
//
//    Return Values: 
//      If the file IO fails, it returns a negative number
//      and errno indicates an actual error condition.
//      If the write to this file is successful, the method
//      returns a number of bytes of data written out.
//
//    Pre-Conditions:  ( Opened() )
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
LONG
BootFile::Write(UCHAR *pBuf, LONG nBytes )
{
    PRE_CONDITION(Opened());

    return(::fwrite( pBuf, 1, nBytes, PFileId ));
}

//******************************************************************************/
//    Operation Name: Read(UCHAR pBuf, LONG location, LONG nBytes)
//
//    Processing: 
//      Reads nbytes data from this Bootfile at a given file offset location.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      Returns total number of data bytes read in. If the file IO fails,
//      it returns a negative number and errno indicates actual error
//      condition.
//
//    Pre-Conditions: ( Opened() )
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
LONG
BootFile::Read(UCHAR *pBuf, LONG location, LONG nBytes )
{
    PRE_CONDITION( Opened() );
    DEBUG_PRE_CONDITION(pBuf);

    if ( location != CurrentOffSet )
    {
        if ( SetOffSet(location) )
            return(-1);
    }
    return (::fread(pBuf,1, nBytes, PFileId ));
}

//******************************************************************************/
//    Operation Name: Read(UCHAR pBuf, LONG nBytes)
//
//    Processing: 
//      Reads nbytes data from this Bootfile.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      Returns total number of data bytes read in. If the file IO fails,
//      it returns a negative number and errno indicates actual error
//      condition.
//
//    Pre-Conditions: ( Opened() )
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
LONG
BootFile::Read(UCHAR *pBuf, LONG nBytes )
{
    PRE_CONDITION( Opened() );

    return (::fread(pBuf,1, nBytes, PFileId ));
}

//******************************************************************************/
//    Operation Name: GetFileSize()
//
//    Processing: 
//        Gets the file size of this file.
//        Move the file offset to the end of the file and do ftell to
//        find out a file size.  Make sure the original file offset is 
//        restored.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: Size of this file in bytes.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
LONG 
BootFile::GetFileSize()
{
LONG offSet, fileSize;

    PRE_CONDITION( Opened() );

    offSet = (LONG) ftell(PFileId); // get the file size.
    fseek(PFileId, 0,  SEEK_END );     // move to the end of file.
    fileSize = (int) ftell(PFileId);  // get the file size.
    fseek(PFileId, offSet, SEEK_SET); // restore the original file offset.
    return(fileSize);
}

//******************************************************************************/
//    Operation Name: SetOffSet(offSet)
//
//    Processing: 
//      Set the current file offset pointer to a specified offset number.
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: eError - if fseek function fails or an given offset 
//       excceeds this file size.
//          
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
E_ReturnStatus 
BootFile::SetOffSet(LONG offSet)
{
    PRE_CONDITION( Opened() );

    if ( offSet > FileSize )
        return(eError);

    if ( fseek(PFileId, offSet, SEEK_SET) ) 
        return(eError);

    CurrentOffSet = offSet;
    return(eSuccess);
}
