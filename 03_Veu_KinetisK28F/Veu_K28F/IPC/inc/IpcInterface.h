#ifndef INC_IPCINTERFACE_H_
#define INC_IPCINTERFACE_H_

typedef struct
{
        uint8_t Header;
        uint8_t Request;
        uint32_t Size;
        uint32_t Data;
}Message;

#endif /* INC_IPCINTERFACE_H_ */
