#ifndef TEXTLIBRARY_H_
#define TEXTLIBRARY_H_

/********************************************************************/
/*                                                                  */
/* File Name    : TextLibrary.c                          			*/
/*                                                                  */
/* General      : StringTable used to keep a list of all the text   */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "StringTable.h"

//Get string
const char* textLibrary_GetString(E_StringID id);

#endif /* TEXTLIBRARY_H_ */
/* end of file */
