#ifndef _MCAL_H_
#define _MCAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
                        Includes
*************************************************************************/
#include "Std_Types.h"
#include "stm32h5xx.h"
#include "stm32h5xx_hal.h"
/*************************************************************************
                        Macro definition
*************************************************************************/
#define MCU_ENDIAN_TYPE MCU_LITTLE_ENDIAN
#define MCU_BIG_ENDIAN  0x00
#define MCU_LITTLE_ENDIAN 0x01

/*************************************************************************
                        Type definition
*************************************************************************/

/*************************************************************************
                        Variables
*************************************************************************/

/*************************************************************************
                        Functions
*************************************************************************/
/******* INTERRUPTS *******/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
/* Generals */
void Error_Handler(void);
void Mcal_Init(void);

#ifdef __cplusplus
}
#endif

#endif
