/*
 * util.h
 *
 *  Created on: Aug 25, 2016
 *      Author: WINDOWS
 */

#ifndef INC_UTIL_H_
	#define INC_UTIL_H_

#include <lpc_types.h>
#include <stddef.h>
#include "board.h"
#include "ff.h"


#define DELAY_200MS 200/portTICK_PERIOD_MS

int CharToAscci(int8_t ch);

int HexCharToInt(int8_t a);

uint16_t util_CompCrcCcitt(uint16_t initCRC, uint8_t *pData, uint32_t nBytes);

size_t find_index(const uint16_t a[], size_t size, int value );
void UpperCase(char* s);

uint32_t util_DecCharToInt(int8_t *s, size_t size);
uint32_t GetValue(FIL* file, int8_t headedCh, uint32_t* byteCount, uint8_t len, bool isHex);
uint32_t HexStrToInt(int8_t *s, uint8_t size);

#endif /* INC_UTIL_H_ */
