#ifndef INC_EXTERNALEEPROM_H_
#define INC_EXTERNALEEPROM_H_

/******************************************************************************/
//
//  File Name		: ExternalEeprom.h
//
//	General       	: Contains function for init and communicate with external Eeprom
//
//  Product Name	: FH310_m4
//
//
//  Revision History:
//          Rev:      Date:       		Editor:
//          01        Jul 12, 2017	        Quyen Ngo(MV)
//          Description:  InitialRevision
//
/******************************************************************************/

#define EEPROM_READ_STABLE_TIME_MS				1

int16_t extEeprom_Write(uint8_t block, uint8_t* buf, uint8_t page, uint8_t offset, uint8_t len);

int16_t extEeprom_Read(uint8_t block, uint8_t* buf, uint8_t page,uint8_t offset, uint8_t len);

void extEeprom_ResetSW(void);

#endif /* end of file */
