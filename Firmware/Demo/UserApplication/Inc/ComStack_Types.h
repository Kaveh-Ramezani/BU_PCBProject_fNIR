#ifndef _COMSTACK_TYPES_H_
#define _COMSTACK_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

typedef uint16 PduIdType;
typedef uint16 PduLengthType;

typedef struct {
  PduLengthType SduLength;
  uint8         *SduDataPtr;
}PduInfoType;

typedef uint8 Modbus_BufferStatusType;
#define BUFF_NOT_IN_USE  0x00
#define BUFF_IN_USE      0x01

typedef struct
{
  uint16                  bufferSizeType;
  PduInfoType             bufferData;
  Modbus_BufferStatusType bufferStatus;
}BufferType;

#ifdef __cplusplus
}
#endif

#endif
