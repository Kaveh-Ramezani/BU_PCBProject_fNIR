#ifndef _COM_ADS111X_H_
#define _COM_ADS111X_H_

#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************************
                            Includes
*************************************************************************/
#include "Std_Types.h"
/*************************************************************************
                            Macro definition
*************************************************************************/
#define I2C_TIMEOUT 50u /*!< The default time out of the I2C communication. */
/*************************************************************************
                            Type definition
*************************************************************************/

/*************************************************************************
                            Variables
*************************************************************************/

/*************************************************************************
                            Functions
*************************************************************************/
Std_ReturnType Com_ADS111X_Read_ConversionRegister(uint16* outData);
Std_ReturnType Com_ADS111X_Write_ConfigRegister(uint16 inData);
Std_ReturnType Com_ADS111X_Read_ConfigRegister(uint16* outData);
Std_ReturnType Com_ADS111X_Read_LoThreshRegister(uint16* outData);
Std_ReturnType Com_ADS111X_Write_LoThreshRegister(uint16 inData);
Std_ReturnType Com_ADS111X_Read_HiThreshRegister(uint16* outData);
Std_ReturnType Com_ADS111X_Write_HiThreshRegister(uint16 inData);

Std_ReturnType Com_ADS111X_IsConnected(void);

#ifdef __cplusplus
}
#endif

#endif
