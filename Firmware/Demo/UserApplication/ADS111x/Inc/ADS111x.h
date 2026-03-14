#ifndef _ADS111X_H_
#define _ADS111X_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
													Includes
*************************************************************************/
#include "ADS111x_Types.h"
/*************************************************************************
													Macro definition
*************************************************************************/

/*************************************************************************
													Type definition
*************************************************************************/

/*************************************************************************
													Variables
*************************************************************************/

/*************************************************************************
													Global Functions
*************************************************************************/
Std_ReturnType ADS111X_Set_ConfigRegister(ADS111X_ConfigRegType* configReg);
Std_ReturnType ADS111X_Get_ConfigRegister(uint16* configReg);
Std_ReturnType ADS111X_Get_ConversionRegister(uint16* data);
Std_ReturnType ADS111X_StartConversion(void);
Std_ReturnType ADS111X_IsConversionFinished(boolean* stat);
boolean ADS111X_IsConnected(void);

void ADS111X_Init(void);

#ifdef __cplusplus
}
#endif

#endif