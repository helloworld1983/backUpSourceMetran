/*
 * stringtools.c
 *
 *  Created on: Mar 7, 2016
 *      Author: QUOCVIET
 */
/*
 * stringtools.cpp
 *
 *  Created on: Sep 22, 2015
 *      Author: bui phuoc
 *
 *      Revision:
 *      Sep 03, 2015	bui phuoc
 *						add function GetStrLength() to get length of a string in pixels
 *		Sep 22, 2015	bui phuoc
 *						add function AsciiToFont() to convert a string in ASCII to system font
 *		Sep 22, 2015	bui phuoc
 *						add function IntToAscii() to convert a number to ASCII string
 *		Sep 22, 2015	bui phuoc
 *						add function IntToFont() to convert a number to system font for display purpose
 */

/********************************************************************/
/*                                                                  */
/* File Name    : stringtools.c                          			*/
/*                                                                  */
/* General      : stringtools provides all string tool 				*/
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
#include <stdlib.h>
#include <math.h>
#include <RTC.h>
#include "stringtools.h"
#include "stdio.h"

char g_bufferStringTool[10];
char bufferDateString[30];

/****************************************************************************************/
/*                                                                                      */
/* General 	-Convert int to Ascii								                        */
/*                                                                                      */
/* Details  -This operation convert an integer number to ASCII string character			*/
/*		then return number of character of the output string. A desired number of		*/
/*		character in string output is supported as an option. if number of character	*/
/*		is bigger than string size, the '0' character is filled in front of the string.	*/
/*		Otherwise, keep string as it is													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) long int a : the number need to be converted							*/
/*                 char* buff : pointer to save string after conversion.		 		*/
/*				   int numOfDigit : desired length of string.							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int - size of string		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int stringtools_IntToAscii(long int a, char* buff, int numOfDigit)
{
	int index = 0;
	char temp;
	long int input = a;
	if (input == 0)
	{
		if(numOfDigit == 0)	//not required number of digit
			buff[index++] = '0';	//default
		else
		{
			int j = 0;
			for(j = 0; j < numOfDigit; j++)
				buff[index++] = '0';
		}
		return index;
	}

	if(input < 0)
	{
		buff[index++] = '-';
		input = -input;
	}

	//convert number to string and store in temporary memmory
	char tempBuff[10] = {'0'};
	int i = 0;
	while (input)
	{
		temp = '0' + input % 10;
		tempBuff[i++] = temp;
		input /= 10;
	}

	//cope from temporary buffer to buffer output
	int k = i;
	if(numOfDigit > k)	//numOfDigit != 0
	{
		for(; k < numOfDigit; k++)
			buff[index++] = '0';
	}

	while(i)
		buff[index++] = tempBuff[--i];

	return index;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Reverses a string 'str' of length 'len'							        */
/*                                                                                      */
/* Details  -Reverses a string 'str' of length 'len'								    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) char *str : pointer string 											*/
/*                 int len : length of string									 		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                            				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void stringtools_Reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Convert int to string											            */
/*                                                                                      */
/* Details  -Converts a given integer x to string str[].  d is the number				*/
/*           of digits required in output. If d is more than the number					*/
/*           of digits in x, then 0s are added at the beginning.						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) char str[] : pointer string 											*/
/*                 int x : number convert												*/
/* 				   int d : number of digits											 	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int - length string after convert		                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int stringtools_IntToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x%10) + '0';
		x = x/10;
	}

	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
		str[i++] = '0';

	stringtools_Reverse(str, i);
	str[i] = '\0';
	return i;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Converts a floating point number to string									*/
/*                                                                                      */
/* Details  -Converts a given floating x to string str[].  d is the number				*/
/*           of digits required in output. If d is more than the number					*/
/*           of digits in x, then 0s are added at the beginning.						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) char str[] : pointer string 											*/
/*                 char *res : buffer res												*/
/* 				   afterpoint : point    											 	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE									                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void ftoa(float n, char *res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// convert integer part to string
	int i = stringtools_IntToStr(ipart, res, 1);

	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot

		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);

		stringtools_IntToStr((int)fpart, res + i + 1, afterpoint);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- Convert flow output to string												*/
/*                                                                                      */
/* Details  - Convert flow output to string												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) char* buff - pointer to save string after conversion 				*/
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : char - string after convert									        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
char* ConvertFCFlowToString(unsigned char value)
{
	//char *buffer = malloc(10);

	for(int i = 0; i < 10; i++)
	{
		g_bufferStringTool[i] = '\0';
	}

	float floatValue = (float)value/20;

	sprintf(&g_bufferStringTool[0],"%0.2f",floatValue);

	return g_bufferStringTool;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- Get current date to string												*/
/*                                                                                      */
/* Details  - Get current date to string												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 															    */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : char - string after convert									        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
char* GetDateString()
{
	//char buffer[30] = { 0 };//{'\0'};

	for(int i = 0; i < 30; i++)
	{
		bufferDateString[i] = '\0';
	}

	char yearBuffer[5] = { 0 };//{'\0'};
	char monthBuffer[3] = { 0 };//{'\0'};
	char dayBuffer[3] = { 0 };//{'\0'};

	char hourBuffer[3] = { 0 };//{'\0'};
	char minuteBuffer[3] = { 0 };//{'\0'};
	char secondBuffer[3] = { 0 };//{'\0'};

	Real_Time_Clock clockTime;

	realtimeclock_gettime (&clockTime);
	sprintf(yearBuffer,"%.4d",clockTime.year);
	sprintf(monthBuffer,"%.2d",clockTime.month);
	sprintf(dayBuffer,"%.2d",clockTime.mday);

	sprintf(hourBuffer,"%.2d",clockTime.hour);
	sprintf(minuteBuffer,"%.2d",clockTime.min);
	sprintf(secondBuffer,"%.2d",clockTime.sec);

	strcat(bufferDateString,yearBuffer);
	strcat(bufferDateString,"/");
	strcat(bufferDateString,monthBuffer);
	strcat(bufferDateString,"/");
	strcat(bufferDateString,dayBuffer);

	strcat(bufferDateString," ");
	strcat(bufferDateString," ");

	strcat(bufferDateString,hourBuffer);
	strcat(bufferDateString,":");
	strcat(bufferDateString,minuteBuffer);
	strcat(bufferDateString,":");
	strcat(bufferDateString,secondBuffer);
	return bufferDateString;
}

